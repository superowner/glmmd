#ifndef VMD_LOADER_H
#define VMD_LOADER_H
#include <vector>
#include <string>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

struct BoneKeyFrameRecord
{
    std::string boneName; // utf8
    uint32_t frameTime;
    glm::vec3 translation;
    glm::quat rotation;

    uint8_t xCurve[4];
    uint8_t YCurve[4];
    uint8_t zCurve[4];
    uint8_t RCurve[4];
};

struct MorphKeyFrameRecord
{
    std::string morphName; // utf8
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
    std::string modelName; // utf8
    std::vector<BoneKeyFrameRecord> boneKeyFrameList;
    std::vector<MorphKeyFrameRecord> morphKeyFrameList;
    std::vector<CameraKeyFrameRecord> cameraKeyFrameList;
    std::vector<LightKeyFrameRecord> lightKeyFrameList;

    void loadFromFile(const std::string &filename);
};

#endif