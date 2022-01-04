#ifndef PMX_MODEL_H_
#define PMX_MODEL_H_

/*
   byte |  uint8_t | 1
  sbyte |   int8_t | 1

 ushort | uint16_t | 2
  short |  int16_t | 2

   uint | uint32_t | 4
    int |  int32_t | 4

  float |    float | 4
 float2 |     vec2 | 8
 float3 |     vec3 | 8
 float4 |     vec4 | 8

TextBuf: 4 + buffer_size

*/

#include <cstdint>

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace pmx
{
    struct ModelInfo
    {
        char header[5];
        float version;

        uint8_t byteSize[8];
        //[0]: encoding method        | 0:UTF16 1:UTF8
        //[1]: additional UV          | 0~4
        //[2]: Vertex Index Size      | 1,2,4
        //[3]: Texture Index Size     | 1,2,4
        //[4]: Material Index Size    | 1,2,4
        //[5]: Bone Index Size        | 1,2,4
        //[6]: Morph Index Size       | 1,2,4
        //[7]: Rigidbody Index Size   | 1,2,4

        // utf8
        std::string modelName;
        std::string modelNameEN;
        std::string comment;
        std::string commentEN;

        inline bool isUtf8() { return byteSize[0]; }
        inline uint8_t additionalUVCount() { return byteSize[1]; }
        inline uint8_t vertexIndexSize() { return byteSize[2]; }
        inline uint8_t textureIndexSize() { return byteSize[3]; }
        inline uint8_t materialIndexSize() { return byteSize[4]; }
        inline uint8_t boneIndexSize() { return byteSize[5]; }
        inline uint8_t morphIndexSize() { return byteSize[6]; }
        inline uint8_t rigidbodyIndexSize() { return byteSize[7]; }
    };

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 uv;

        std::vector<glm::vec4> additionalUV;
        uint8_t deformMethod; // 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF
        int32_t bindedBone[4];
        glm::vec4 weights;

        float edgeScale;
    };

    struct Material
    {
        std::string name;
        std::string nameEN;

        glm::vec4 diffuse;
        glm::vec3 specular;
        float specIntensity;
        glm::vec3 ambient;

        uint8_t bitFlag;
        // 0x01: double side
        // 0x02: ground shadow
        // 0x04: self shadow map
        // 0x08: self shadow
        // 0x10: edge

        glm::vec4 edgeColor;
        float edgeSize;

        int32_t diffuseTexId;
        int32_t sphereTexId;
        uint8_t sphereMode;
        // 0: disabled
        // 1: mul
        // 2: add
        // 3: subtexture

        uint8_t sharedToonFlag;
        int32_t toonTexId;

        std::string memo;
        uint32_t indexCount;
    };

    struct IKLink
    {
        int32_t index;
        uint8_t angleLimitOn;
        glm::vec3 minAngle;
        glm::vec3 maxAngle;
    };

    struct Bone
    {
        std::string name;   // utf8
        std::string nameEN; // utf8

        glm::vec3 position;
        int32_t parentIndex;
        int32_t deformOrder;

        uint16_t bitFlag;
        // 0x0001 : heading representing method | 0: coordinate 1: another bone
        // 0x0002 : allow rotation
        // 0x0004 : allow translation
        // 0x0008 : display
        // 0x0010 : allow operation
        // 0x0020 : IK

        // 0x0080 : local attribution | 0: user deformation / IK link / multi attribution 1:parent local deformation
        // 0x0100 : rotation attribution
        // 0x0200 : translation attribution

        // 0x0400 : lock axis
        // 0x0800 : local axis

        // 0x1000 : deformation after physics
        // 0x2000 : external parent deformation

        int32_t endBoneIndex;
        glm::vec3 endPos;

        int32_t attribIndex;
        float attribWeight;

        glm::vec3 lockedAxis;
        glm::vec3 localAxisX;
        glm::vec3 localAxisZ;

        int32_t extParentKey;

        int32_t IK_targetBoneIndex;
        uint32_t IK_loopCount;
        float IK_limitAngle;
        std::vector<IKLink> IK_linkList;
    };

    struct Model
    {
        ModelInfo info;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<std::string> texturePath;
        std::vector<Material> materials;
        std::vector<Bone> bones;

        void loadFromFile(const std::string &filename);
        void freadVertex(Vertex &vert, FILE *fp);
        void freadMaterial(Material &mat, FILE *fp);
        void freadBone(Bone &bone, FILE *fp);
    };

}

#endif