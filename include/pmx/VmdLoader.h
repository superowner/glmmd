#ifndef VMD_LOADER_H
#define VMD_LOADER_H
#include <vector>
#include <string>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace vmd
{

    /*with key frame list and curve list
    input: float time -> output: (uint32_t leftKeyFrame, uint32_t rightKeyFrame, float(n) interpolationTime)*/
    struct BoneKeyFrameRecord
    {
        std::string boneName; //Shift_JIS

        uint32_t frameTime;
        glm::vec3 translation;
        glm::quat rotation;

        uint8_t XCurve[4];
        uint8_t YCurve[4];
        uint8_t ZCurve[4];
        uint8_t RCurve[4];
    };

    struct MorphKeyFrameRecord
    {
        std::string morphName;
        uint32_t frameTime;
        float weight;
    };

    struct CameraKeyFrameRecord
    {
        uint32_t frameTime;
        float distance;
        glm::vec3 position;
        glm::vec3 rotation;
        uint8_t curve[24];
        float fov;
        uint8_t ortho;
    };

    struct LightKeyFrameRecord
    {
        uint32_t frameTime;
        glm::vec3 color;
        glm::vec3 direction;
    };
    struct VmdData
    {
        int ver;
        std::string modelName; //Shift-JIS
        std::vector<BoneKeyFrameRecord> boneKeyFrameList;
        std::vector<MorphKeyFrameRecord> morphKeyFrameList;
        std::vector<CameraKeyFrameRecord> cameraKeyFrameList;
        std::vector<LightKeyFrameRecord> lightKeyFrameList;
    };
}
#endif