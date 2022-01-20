#include <algorithm>
#include <animator/PmxBoneAnimator.h>
#include <cassert>
#include <functional>
#include <unordered_map>
#include <utils/Logger.h>
#define IK_SOLVER_TOL 1e-8f

glm::mat4 BoneTransform::getLocalMatrix()
{
    glm::mat4 trans = glm::translate(glm::mat4(1.0), localTranslation + localTranslationOffset);
    return trans * glm::mat4_cast(localRotation);
}

glm::vec3 BoneTransform::getGlobalPosition()
{
    return glm::vec3(globalMatrix[3][0], globalMatrix[3][1], globalMatrix[3][2]);
}

PmxBoneAnimator::PmxBoneAnimator(const pmx::Model &model, const VmdData &motion)
    : m_frameCount(0)
{
    std::unordered_map<std::string, uint32_t> boneNameMap;
    for (uint32_t i = 0; i < model.bones.size(); ++i)
        boneNameMap[model.bones[i].name] = i;

    m_keyFrameTable.resize(model.bones.size());
    for (auto &kf : motion.boneKeyFrameList)
    {
        auto ite = boneNameMap.find(kf.boneName);
        if (ite != boneNameMap.end())
            m_keyFrameTable[ite->second].push_back(kf);
        m_frameCount = std::max(m_frameCount, kf.frameTime);
    }
    GLMMD_LOG_INFO("Max frame: {}", m_frameCount);
    for (auto &kfl : m_keyFrameTable)
        std::sort(kfl.begin(), kfl.end(),
                  [](const BoneKeyFrameRecord &kf1, const BoneKeyFrameRecord &kf2)
                  { return kf1.frameTime < kf2.frameTime; });

    m_boneTransformList.resize(model.bones.size());
    for (uint32_t i = 0; i < model.bones.size(); ++i)
    {
        auto &b = m_boneTransformList[i];
        const auto &mb = model.bones[i];
        b.parentIndex = mb.parentIndex;
        b.bitFlag = mb.bitFlag;
        b.attribIndex = mb.attribIndex;
        b.attribWeight = mb.attribWeight;

        b.position = mb.position;
        b.inverseOffset = glm::translate(glm::mat4(1.0f), -mb.position);
        b.localTranslationOffset = mb.position - (mb.parentIndex == -1 ? glm::vec3(0.0f)
                                                                       : (model.bones[mb.parentIndex].position));

        if (mb.bitFlag & 0x0020) // is IK
        {
            b.IK_targetBoneIndex = mb.IK_targetBoneIndex;
            b.IK_loopCount = mb.IK_loopCount;
            b.IK_limitAngle = mb.IK_limitAngle;
            b.IK_linkList = mb.IK_linkList;
            m_IKList.push_back(i);
        }
    }

    for (uint32_t i = 0; i < model.bones.size(); ++i)
        if (!model.bones[i].bitFlag & 0x1000)
            m_transformOrderBeforePhysics.push_back(i);
        else
            m_transformOrderAfterPhysics.push_back(i);
    std::sort(m_transformOrderBeforePhysics.begin(),
              m_transformOrderBeforePhysics.end(),
              [&model](uint32_t i, uint32_t j)
              {
                  if (model.bones[i].deformOrder != model.bones[j].deformOrder)
                      return model.bones[i].deformOrder < model.bones[j].deformOrder;
                  return i < j;
              });

    std::sort(m_transformOrderAfterPhysics.begin(),
              m_transformOrderAfterPhysics.end(),
              [&model](uint32_t i, uint32_t j)
              {
                  if (model.bones[i].deformOrder != model.bones[j].deformOrder)
                      return model.bones[i].deformOrder < model.bones[j].deformOrder;
                  return i < j;
              });

    m_finalTransformBuffer.resize(model.bones.size() * 16);
}

PmxBoneAnimator::~PmxBoneAnimator() {}

glm::vec3 PmxBoneAnimator::identityTranslation(0.0f, 0.0f, 0.0f);
glm::quat PmxBoneAnimator::identityRotation(1.0f, 0.0f, 0.0f, 0.0f);

float PmxBoneAnimator::calcCurve(const uint8_t *curve, float t)
{
    float x1 = curve[0] / 127.0f;
    float y1 = curve[1] / 127.0f;
    float x2 = curve[2] / 127.0f;
    float y2 = curve[3] / 127.0f;

    std::function<float(float)> fx = [x1, x2, t](float x) -> float
    { return x * (3.f * (1.f - x) * ((1.f - x) * x1 + x * x2) + x * x) - t; };
    float t0 = t * (t * t + 3.0f * (1.0f - t) * ((1.0f - t) * y1 + t * y2));

    float l = 0.f, r = 1.f, m = 0.5f, fm = fx(m);
    while (fabs(fm) > FLT_EPSILON)
    {
        if (fm > 0)
            r = m;
        else
            l = m;
        m = 0.5f * (l + r);
        fm = fx(m);
    }
    return m * (m * m + 3.0f * (1.0f - m) * ((1.0f - m) * y1 + m * y2));
}

glm::vec3 PmxBoneAnimator::lerpTranslation(const glm::vec3 &r0, const glm::vec3 &r1,
                                           float tx, float ty, float tz)
{
    return glm::vec3(r0.x * (1.0f - tx) + r1.x * tx,
                     r0.y * (1.0f - ty) + r1.y * ty,
                     r0.z * (1.0f - tz) + r1.z * tz);
}

float PmxBoneAnimator::clamp(float x, float floor, float ceil)
{
    if (x < floor)
        return floor;
    if (x > ceil)
        return ceil;
    return x;
}
BoneKeyFrameInterpolation PmxBoneAnimator::interpolate(uint32_t boneIndex, float frameTime) const
{
    assert(boneIndex >= 0 && boneIndex < m_keyFrameTable.size());
    const auto &keyFrameList = m_keyFrameTable[boneIndex];
    if (keyFrameList.empty())
        return {-1, -1, 0.0f, 0.0f, 0.0f, 0.0f};
    if (frameTime < keyFrameList[0].frameTime)
    {
        float t = frameTime / (float)keyFrameList[0].frameTime;
        return {-1, 0, t, t, t, t};
    }
    if (frameTime >= keyFrameList.back().frameTime)
        return {int32_t(keyFrameList.size() - 1), -1, 0.0f, 0.0f, 0.0f, 0.0f};

    int32_t l = 0, r = (int32_t)keyFrameList.size() - 1, m = (l + r) / 2;
    while (r - l > 1)
    {
        if (frameTime < keyFrameList[m].frameTime)
            r = m;
        else
            l = m;

        m = (l + r) / 2;
    }
    float t = (frameTime - keyFrameList[m].frameTime) /
              (float)(keyFrameList[m + 1].frameTime - keyFrameList[m].frameTime);
    float tx = calcCurve(keyFrameList[m].xCurve, t);
    float ty = calcCurve(keyFrameList[m].yCurve, t);
    float tz = calcCurve(keyFrameList[m].zCurve, t);
    float tr = calcCurve(keyFrameList[m].rCurve, t);

    return {m, m + 1, tx, ty, tz, tr};
}

void PmxBoneAnimator::setBoneLocalTransform(uint32_t boneIndex, float frameTime)
{
    assert(boneIndex >= 0 && boneIndex < m_boneTransformList.size());
    auto &tr = m_boneTransformList[boneIndex];
    const auto &kf = m_keyFrameTable[boneIndex];
    auto itpl = interpolate(boneIndex, frameTime);

    if (itpl.leftKeyFrame == itpl.rightKeyFrame) // no key frame
    {
        tr.localTranslation = identityTranslation;
        tr.localRotation = identityRotation;
    }
    else if (itpl.leftKeyFrame == -1) // before the first key frame
    {
        tr.localTranslation = lerpTranslation(identityTranslation,
                                              kf[0].translation,
                                              itpl.tx, itpl.ty, itpl.tz);
        tr.localRotation = glm::slerp(identityRotation, kf[0].rotation, itpl.tr);
    }
    else if (itpl.rightKeyFrame == -1) // after the last key frame
    {
        tr.localTranslation = kf.back().translation;
        tr.localRotation = kf.back().rotation;
    }
    else
    {
        tr.localTranslation = lerpTranslation(kf[itpl.leftKeyFrame].translation,
                                              kf[itpl.rightKeyFrame].translation,
                                              itpl.tx, itpl.ty, itpl.tz);
        tr.localRotation = glm::slerp(kf[itpl.leftKeyFrame].rotation,
                                      kf[itpl.rightKeyFrame].rotation,
                                      itpl.tr);
    }
}

void PmxBoneAnimator::setAttribTransform()
{
    for (uint32_t i = 0; i < m_boneTransformList.size(); ++i)
    {
        auto &tr = m_boneTransformList[i];
        if (tr.bitFlag & 0x0100)
            tr.localRotation = glm::slerp(identityRotation, m_boneTransformList[tr.attribIndex].localRotation, tr.attribWeight) * tr.localRotation;
        if (tr.bitFlag & 0x0200)
            tr.localTranslation += m_boneTransformList[tr.attribIndex].localTranslation * tr.attribWeight;
    }
}

void PmxBoneAnimator::setBoneGlobalTransformBeforePhysics()
{
    for (auto &i : m_transformOrderBeforePhysics)
    {
        int32_t parentIndex = m_boneTransformList[i].parentIndex;
        if (parentIndex == -1)
            m_boneTransformList[i].globalMatrix = glm::translate(m_boneTransformList[i].getLocalMatrix(),
                                                                 m_boneTransformList[i].position);
        else
            m_boneTransformList[i].globalMatrix = m_boneTransformList[parentIndex].globalMatrix *
                                                  m_boneTransformList[i].getLocalMatrix();
    }
}
void PmxBoneAnimator::setBoneGlobalTransformAfterPhysics()
{
    for (auto &i : m_transformOrderAfterPhysics)
    {
        int32_t parentIndex = m_boneTransformList[i].parentIndex;
        if (parentIndex == -1)
            m_boneTransformList[i].globalMatrix = m_boneTransformList[i].getLocalMatrix();
        else
            m_boneTransformList[i].globalMatrix = m_boneTransformList[parentIndex].globalMatrix *
                                                  m_boneTransformList[i].getLocalMatrix();
    }
}

void PmxBoneAnimator::solveIK()
{
    for (auto &i : m_IKList)
    {
        auto &IKBone = m_boneTransformList[i];
        glm::vec3 targetPos = IKBone.getGlobalPosition();

        for (uint32_t j = 0; j < IKBone.IK_linkList.size(); ++j)
        {
            auto &b = m_boneTransformList[IKBone.IK_linkList[j].index];
            b.IK_rotation = identityRotation;
            b.localRotationOrigin = b.localRotation;
        }

        for (uint32_t loopCount = 0; loopCount < IKBone.IK_loopCount; ++loopCount)
        {
            for (uint32_t j = 0; j < IKBone.IK_linkList.size(); ++j)
            {
                glm::vec3 endEffectorPos = m_boneTransformList[IKBone.IK_targetBoneIndex].getGlobalPosition();

                pmx::IKLink &linkedBone = IKBone.IK_linkList[j];
                auto &currentBone = m_boneTransformList[linkedBone.index];

                glm::vec3 currentBonePos = currentBone.getGlobalPosition();
                auto &pos = currentBonePos;

                if (glm::length(targetPos - endEffectorPos) < IK_SOLVER_TOL ||
                    glm::length(targetPos - currentBonePos) < IK_SOLVER_TOL ||
                    glm::length(endEffectorPos - currentBonePos) < IK_SOLVER_TOL)
                {
                    loopCount = IKBone.IK_loopCount;
                    break;
                }
                glm::vec3 targetDir = targetPos - currentBonePos;
                glm::vec3 endEffectorDir = endEffectorPos - currentBonePos;

                float rotAngle = std::acos(clamp(glm::dot(targetDir, endEffectorDir) /
                                                     (glm::length(targetDir) * glm::length(endEffectorDir)),
                                                 -1.0f, 1.0f));

                if (rotAngle == 0.0f)
                    continue;
                glm::mat3 inverseAxisTransform = glm::mat3(currentBone.globalMatrix);
                glm::vec3 rotAxis = glm::inverse(inverseAxisTransform) * glm::cross(endEffectorDir, targetDir);
                // Add local transform
                if (glm::length(rotAxis) == 0.0f)
                    continue;
                glm::quat rot(float(cos(0.5f * rotAngle)),
                              glm::normalize(rotAxis) * float(sin(0.5f * rotAngle)));

                currentBone.IK_rotation = rot * currentBone.IK_rotation;
                if (linkedBone.angleLimitOn)
                {
                    glm::vec3 rotEulerAngles = glm::eulerAngles(currentBone.IK_rotation);

                    rotEulerAngles.x = clamp(rotEulerAngles.x, linkedBone.minAngle.x, linkedBone.maxAngle.x);
                    rotEulerAngles.y = clamp(rotEulerAngles.y, linkedBone.minAngle.y, linkedBone.maxAngle.y);
                    rotEulerAngles.z = clamp(rotEulerAngles.z, linkedBone.minAngle.z, linkedBone.maxAngle.z);
                    currentBone.IK_rotation = glm::quat(rotEulerAngles);
                }

                currentBone.localRotation = currentBone.IK_rotation * currentBone.localRotationOrigin;
                setBoneGlobalTransformBeforePhysics();
                setBoneGlobalTransformAfterPhysics();
            }
        }
    }
}

void PmxBoneAnimator::updateBoneTransform(float frameTime, float deltaTime)
{
    for (uint32_t i = 0; i < m_boneTransformList.size(); ++i)
        setBoneLocalTransform(i, frameTime);
    setBoneGlobalTransformBeforePhysics();
    setBoneGlobalTransformAfterPhysics();
    solveIK();
    setAttribTransform();
    setBoneGlobalTransformBeforePhysics();
    setBoneGlobalTransformAfterPhysics();
    for (uint32_t i = 0; i < m_boneTransformList.size(); ++i)
    {
        glm::mat4 finalTransform = m_boneTransformList[i].globalMatrix * m_boneTransformList[i].inverseOffset;
        for (uint8_t j = 0; j < 16; ++j)
            m_finalTransformBuffer[i * 16 + j] = *(&finalTransform[0][0] + j);
    }
}

const float *PmxBoneAnimator::finalTransformBuffer() const
{
    return &m_finalTransformBuffer[0];
}
