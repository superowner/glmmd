#include <pmx/PmxBoneAnimator.h>
#include <cassert>
#include <unordered_map>
#include <algorithm>
glm::mat4 BoneTransform::getLocalMatrix()
{
    // + localTranslationOffset ???
    glm::mat4 trans = glm::translate(glm::mat4(1.0), localTranslation + localTranslationOffset);
    return trans * glm::mat4_cast(localRotation);
}

PmxBoneAnimator::PmxBoneAnimator(const pmx::Model &model, const VmdData &motion)
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
    }
    for (auto &kfl : m_keyFrameTable)
        std::sort(kfl.begin(), kfl.end(),
                  [](const BoneKeyFrameRecord &kf1, const BoneKeyFrameRecord &kf2)
                  { return kf1.frameTime < kf2.frameTime; });

    m_boneTransformList.resize(model.bones.size());
    for (uint32_t i = 0; i < model.bones.size(); ++i)
    {
        m_boneTransformList[i].parentIndex = model.bones[i].parentIndex;
        m_boneTransformList[i].position = model.bones[i].position;
        m_boneTransformList[i].inverseOffset = glm::translate(glm::mat4(1.0f), -model.bones[i].position);
        m_boneTransformList[i].localTranslationOffset = model.bones[i].position -
                                                        (model.bones[i].parentIndex == -1 ? glm::vec3(0.0f)
                                                                                          : (model.bones[model.bones[i].parentIndex].position));
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
    float y1 = curve[2] / 127.0f;
    float y2 = curve[3] / 127.0f;
    return t * (t * t + 3.0f * (1.0f - t) * ((1.0f - t) * y1 + t * y2));
}

glm::vec3 PmxBoneAnimator::lerpTranslation(const glm::vec3 &r0, const glm::vec3 &r1,
                                           float tx, float ty, float tz)
{
    return glm::vec3(r0.x * (1.0f - tx) + r1.x * tx,
                     r0.y * (1.0f - ty) + r1.y * ty,
                     r0.z * (1.0f - tz) + r1.z * tz);
}

glm::quat PmxBoneAnimator::slerpRotation(const glm::quat &r0, const glm::quat &r1, float tr)
{
    float d = r0.x * r1.x + r0.y * r1.y + r0.z * r1.z + r0.w * r1.w;
    if (d > 1.0f)
        d = 1.0f;
    if (d < 0.0f)
        return slerpRotation(-r0, r1, tr);

    float alpha = acos(d);

    float a, b;
    if (alpha > FLT_EPSILON)
    {
        a = sin(alpha * (1.0f - tr)) / sin(alpha);
        b = sin(alpha * tr) / sin(alpha);
    }
    else
    {
        a = 1.0f - tr;
        b = tr;
    }
    return a * r0 + b * r1;
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
        return;
    }
    if (itpl.leftKeyFrame == -1) // before the first key frame
    {
        tr.localTranslation = lerpTranslation(identityTranslation,
                                              kf[0].translation,
                                              itpl.tx, itpl.ty, itpl.tz);
        tr.localRotation = slerpRotation(identityRotation, kf[0].rotation, itpl.tr);
        return;
    }
    if (itpl.rightKeyFrame == -1) // after the last key frame
    {
        tr.localTranslation = kf.back().translation;
        tr.localRotation = kf.back().rotation;
        return;
    }
    tr.localTranslation = lerpTranslation(kf[itpl.leftKeyFrame].translation,
                                          kf[itpl.rightKeyFrame].translation,
                                          itpl.tx, itpl.ty, itpl.tz);
    tr.localRotation = slerpRotation(kf[itpl.leftKeyFrame].rotation,
                                     kf[itpl.rightKeyFrame].rotation,
                                     itpl.tr);
}

void PmxBoneAnimator::setBoneGlobalTransformBeforePhysics()
{
    for (auto &i : m_transformOrderBeforePhysics)
    {
        int32_t parentIndex = m_boneTransformList[i].parentIndex;
        if (parentIndex == -1)
            // translate bone.position ???
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
            // translate bone.position ???
            m_boneTransformList[i].globalMatrix = m_boneTransformList[i].getLocalMatrix();
        else
            m_boneTransformList[i].globalMatrix = m_boneTransformList[parentIndex].globalMatrix *
                                                  m_boneTransformList[i].getLocalMatrix();
    }
}

void PmxBoneAnimator::updateBoneTransform(float frameTime, float deltaTime)
{
    for (uint32_t i = 0; i < m_boneTransformList.size(); ++i)
        setBoneLocalTransform(i, frameTime);
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
