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
    enum ByteSizeIndex
    {
        ENCODING = 0,
        ADDITIONAL_UV = 1,
        VERT_IDX_SIZE = 2,
        TEX_IDX_SIZE = 3,
        MAT_IDX_SIZE = 4,
        BONE_IDX_SIZE = 5,
        MORPH_IDX_SIZE = 6,
        RIGIDBODY_IDX_SIZE = 7,
    };
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

        std::string modelName;
        std::string modelNameEN;
        std::string comment;
        std::string commentEN;
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

        glm::vec3 edgeColor;
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

    struct Model
    {
        ModelInfo info;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<std::string> texturePath;
        std::vector<Material> materials;

        void loadFromFile(const std::string &filename);
        void freadVertex(Vertex &vert, FILE *fp);
        void freadMaterial(Material &mat, FILE *fp);
    };

}

#endif