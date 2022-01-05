#ifndef PMX_BONE_ANIMATOR_H_
#define PMX_BONE_ANIMATOR_H_

#include <pmx/PmxModel.h>
#include <pmx/VmdData.h>

struct BoneKeyFrameInterpolation
{
    int32_t leftKeyFrame;  // -1 : no left key frame
    int32_t rightKeyFrame; // -1 : no right key frame
    float tx, ty, tz, tr;  // 0.0 ~ 1.0
};

struct BoneTransform
{
    int32_t parentIndex;
    uint16_t bitFlag;
    int32_t attribIndex;
    float attribWeight;
    glm::vec3 position;
    glm::mat4 inverseOffset;
    glm::vec3 localTranslationOffset;
    glm::vec3 localTranslation;
    glm::quat localRotation;
    glm::mat4 globalMatrix;
    glm::mat4 getLocalMatrix();
    glm::vec3 getGlobalPosition();

    int32_t IK_targetBoneIndex;
    uint32_t IK_loopCount;
    float IK_limitAngle;
    std::vector<pmx::IKLink> IK_linkList;
};

class PmxBoneAnimator
{
private:
    std::vector<BoneTransform> m_boneTransformList;
    std::vector<std::vector<BoneKeyFrameRecord>> m_keyFrameTable;
    std::vector<uint32_t> m_transformOrderBeforePhysics;
    std::vector<uint32_t> m_transformOrderAfterPhysics;
    std::vector<float> m_finalTransformBuffer;
    std::vector<uint32_t> m_IKList;

    static glm::vec3 identityTranslation;
    static glm::quat identityRotation;
    static float calcCurve(const uint8_t *curve, float t);
    static glm::vec3 lerpTranslation(const glm::vec3 &r0, const glm::vec3 &r1, float tx, float ty, float tz);
    static glm::quat slerpRotation(const glm::quat &r0, const glm::quat &r1, float tr);
    static float clamp(float x, float floor, float ceil);

    BoneKeyFrameInterpolation interpolate(uint32_t boneIndex, float frameTime) const;
    void setBoneLocalTransform(uint32_t boneIndex, float frameTime);
    void setBoneGlobalTransformBeforePhysics();
    void setBoneGlobalTransformAfterPhysics();
    void solveIK();

public:
    PmxBoneAnimator(const pmx::Model &model, const VmdData &motion);
    ~PmxBoneAnimator();

    void updateBoneTransform(float frameTime, float deltaTime);
    const float *finalTransformBuffer() const;
};

#endif