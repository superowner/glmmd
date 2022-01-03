#include <cstdio>
#include <pmx/PmxModel.h>
#include <utils/CodeConverter.h>
#include <stdexcept>

#define PMX_LOADER_THROW(cond, msg)        \
    do                                     \
    {                                      \
        if (cond)                          \
            throw std::runtime_error(msg); \
    } while (0)

namespace pmx
{

    template <size_t N>
    void freadFloat(void *p, FILE *fp)
    {
        fread(p, sizeof(float), N, fp);
    }

    template <typename T>
    void freadUint(T &n, size_t size, FILE *fp)
    {
        fread(&n, size, 1, fp);
    }

    template <typename T>
    void freadInt(T &n, size_t size, FILE *fp)
    {
        fread(&n, size, 1, fp);
    }

    void freadTextBuf(std::string &buf, FILE *fp)
    {
        uint32_t bufSize;
        freadUint(bufSize, 4, fp);
        if (bufSize)
        {
            char *charBuf = new char[bufSize];
            fread(charBuf, 1, bufSize, fp);
            buf = std::string(charBuf, bufSize);
            delete[] charBuf;
        }
    }

    void Model::freadVertex(Vertex &vert, FILE *fp)
    {
        freadFloat<3>(&vert.position, fp);
        freadFloat<3>(&vert.normal, fp);
        freadFloat<2>(&vert.uv, fp);

        auto additionalUVCount = info.byteSize[ADDITIONAL_UV];
        if (additionalUVCount)
        {
            vert.additionalUV.resize(additionalUVCount);
            for (auto &v : vert.additionalUV)
                freadFloat<4>(&v, fp);
        }

        freadUint(vert.deformMethod, 1, fp);
        auto idxSize = info.byteSize[BONE_IDX_SIZE];
        switch (vert.deformMethod)
        {
        case 0: // BDEF1
            freadInt(vert.bindedBone[0], idxSize, fp);
            vert.weights.x = 1.0f;
            break;
        case 1: // BDEF2
            freadInt(vert.bindedBone[0], idxSize, fp);
            freadInt(vert.bindedBone[1], idxSize, fp);
            freadFloat<1>(&vert.weights.x, fp);
            vert.weights.y = 1.0f - vert.weights.x;
            break;
        case 2: // BDEF4
            freadInt(vert.bindedBone[0], idxSize, fp);
            freadInt(vert.bindedBone[1], idxSize, fp);
            freadInt(vert.bindedBone[2], idxSize, fp);
            freadInt(vert.bindedBone[3], idxSize, fp);
            freadFloat<4>(&vert.weights, fp);
            break;
        case 3: // SDEF
        {
            freadInt(vert.bindedBone[0], idxSize, fp);
            freadInt(vert.bindedBone[1], idxSize, fp);
            freadFloat<1>(&vert.weights.x, fp);
            vert.weights.y = 1.0f - vert.weights.x;
            glm::vec3 tmp;
            freadFloat<3>(&tmp, fp);
            freadFloat<3>(&tmp, fp);
            freadFloat<3>(&tmp, fp);
        }
        break;
        default:
            PMX_LOADER_THROW(1, "failed to load model");
        }

        freadFloat<1>(&vert.edgeScale, fp);
    }

    void Model::freadMaterial(Material &mat, FILE *fp)
    {
        freadTextBuf(mat.name, fp);
        freadTextBuf(mat.nameEN, fp);
        freadFloat<4>(&mat.diffuse, fp);
        freadFloat<3>(&mat.specular, fp);
        freadFloat<1>(&mat.specIntensity, fp);
        freadFloat<3>(&mat.ambient, fp);
        fread(&mat.bitFlag, 1, 1, fp);
        freadFloat<4>(&mat.edgeColor, fp);
        freadFloat<1>(&mat.edgeSize, fp);
        freadInt(mat.diffuseTexId, info.byteSize[TEX_IDX_SIZE], fp);
        freadInt(mat.sphereTexId, info.byteSize[TEX_IDX_SIZE], fp);
        fread(&mat.sphereMode, 1, 1, fp);
        fread(&mat.sharedToonFlag, 1, 1, fp);
        if (mat.sharedToonFlag)
            fread(&mat.toonTexId, 1, 1, fp);
        else
            freadInt(mat.toonTexId, info.byteSize[TEX_IDX_SIZE], fp);
        freadTextBuf(mat.memo, fp);
        freadUint(mat.indexCount, 4, fp);
    }

    void Model::loadFromFile(const std::string &filename)
    {
        FILE *fp;
        fp = fopen(filename.c_str(), "rb");
        PMX_LOADER_THROW(!fp, "failed to load model");

        fread(info.header, 4, 1, fp);
        info.header[4] = '\0';
        PMX_LOADER_THROW(std::string(info.header) != "PMX ", "failed to load model");

        freadFloat<1>(&info.version, fp);
        PMX_LOADER_THROW(info.version != 2.0f, "failed to load model");

        uint8_t bs;
        freadUint(bs, 1, fp);
        PMX_LOADER_THROW(bs != 8, "failed to load model");

        fread(info.byteSize, 1, 8, fp);

        freadTextBuf(info.modelName, fp);
        freadTextBuf(info.modelNameEN, fp);
        freadTextBuf(info.comment, fp);
        freadTextBuf(info.commentEN, fp);

        uint32_t vertCount;
        freadUint(vertCount, 4, fp);
        vertices.resize(vertCount);
        for (auto &vert : vertices)
            freadVertex(vert, fp);

        uint32_t indexCount;
        freadUint(indexCount, 4, fp);
        indices.resize(indexCount);
        for (auto &i : indices)
            freadUint(i, info.byteSize[VERT_IDX_SIZE], fp);

        uint32_t textureCount;
        freadUint(textureCount, 4, fp);
        texturePath.resize(textureCount);

        std::string modelFileDir;
        auto splitPos = filename.rfind('/');
        if (splitPos == std::string::npos)
            splitPos = filename.rfind('\\');
        if (splitPos != std::string::npos)
            modelFileDir = filename.substr(0, splitPos + 1);

        for (auto &tp : texturePath)
        {
            std::string rawPath;
            freadTextBuf(rawPath, fp);
            tp = modelFileDir + convertToNativeEncoding(rawPath, info.byteSize[ENCODING] ? EncodingType::UTF8 : EncodingType::UTF16_LE);
        }

        uint32_t materialCount;
        freadUint(materialCount, 4, fp);
        materials.resize(materialCount);

        for (auto &m : materials)
            freadMaterial(m, fp);
        fclose(fp);
    }
}