#include <cstdio>
#include <pmx/PmxModel.h>
#include <stdexcept>
#include <utils/FileHelper.h>
#include <utils/CodeConverter.h>
#include <utils/Logger.h>

#define PMX_LOADER_THROW(cond, msg)        \
    do                                     \
    {                                      \
        if (cond)                          \
            throw std::runtime_error(msg); \
    } while (0)

namespace pmx
{
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

    auto additionalUVCount = info.additionalUVCount();
    if (additionalUVCount)
    {
        vert.additionalUV.resize(additionalUVCount);
        for (auto &v : vert.additionalUV)
            freadFloat<4>(&v, fp);
    }

    freadUint(vert.deformMethod, 1, fp);
    auto idxSize = info.boneIndexSize();
    switch (vert.deformMethod)
    {
    case 0: // BDEF1
        freadInt(vert.bindedBone[0], idxSize, fp);
        vert.weights.x = 1.0f;
        break;
    case 1: // BDEF2
        freadInt(vert.bindedBone[0], idxSize, fp);
        freadInt(vert.bindedBone[1], idxSize, fp);
        freadFloat(&vert.weights.x, fp);
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
        freadFloat(&vert.weights.x, fp);
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

    freadFloat(&vert.edgeScale, fp);
}

void Model::freadMaterial(Material &mat, FILE *fp)
{
    freadTextBuf(mat.name, fp);
    freadTextBuf(mat.nameEN, fp);
    freadFloat<4>(&mat.diffuse, fp);
    freadFloat<3>(&mat.specular, fp);
    freadFloat(&mat.specIntensity, fp);
    freadFloat<3>(&mat.ambient, fp);
    fread(&mat.bitFlag, 1, 1, fp);
    freadFloat<4>(&mat.edgeColor, fp);
    freadFloat(&mat.edgeSize, fp);
    freadInt(mat.diffuseTexId, info.textureIndexSize(), fp);
    freadInt(mat.sphereTexId, info.textureIndexSize(), fp);
    fread(&mat.sphereMode, 1, 1, fp);
    fread(&mat.sharedToonFlag, 1, 1, fp);
    if (mat.sharedToonFlag)
        fread(&mat.toonTexId, 1, 1, fp);
    else
        freadInt(mat.toonTexId, info.textureIndexSize(), fp);
    freadTextBuf(mat.memo, fp);
    freadUint(mat.indexCount, 4, fp);
}

void Model::freadBone(Bone &bone, FILE *fp)
{
    freadTextBuf(bone.name, fp);
    freadTextBuf(bone.nameEN, fp);
    if (!info.isUtf8())
    {
        bone.name = UTF16_LE_to_UTF8(bone.name);
        bone.nameEN = UTF16_LE_to_UTF8(bone.nameEN);
    }
    freadFloat<3>(&bone.position, fp);
    freadInt(bone.parentIndex, info.boneIndexSize(), fp);
    freadInt(bone.deformOrder, 4, fp);
    freadUint(bone.bitFlag, 2, fp);
    if (bone.bitFlag & 0x0001)
        freadInt(bone.endBoneIndex, info.boneIndexSize(), fp);
    else
        freadFloat<3>(&bone.endPos, fp);

    if ((bone.bitFlag & 0x0100) || (bone.bitFlag & 0x0200))
    {
        freadInt(bone.attribIndex, info.boneIndexSize(), fp);
        freadFloat(&bone.attribWeight, fp);
    }

    if (bone.bitFlag & 0x0400)
        freadFloat<3>(&bone.lockedAxis, fp);
    if (bone.bitFlag & 0x0800)
    {
        freadFloat<3>(&bone.localAxisX, fp);
        freadFloat<3>(&bone.localAxisZ, fp);
    }
    if (bone.bitFlag & 0x2000)
        freadInt(bone.extParentKey, 4, fp);
    if (bone.bitFlag & 0x0020)
    {
        freadInt(bone.IK_targetBoneIndex, info.boneIndexSize(), fp);
        freadUint(bone.IK_loopCount, 4, fp);
        freadFloat(&bone.IK_limitAngle, fp);

        uint32_t IK_linkCount;
        freadUint(IK_linkCount, 4, fp);
        bone.IK_linkList.resize(IK_linkCount);
        for (auto &l : bone.IK_linkList)
        {
            freadInt(l.index, info.boneIndexSize(), fp);
            freadUint(l.angleLimitOn, 1, fp);
            if (l.angleLimitOn)
            {
                freadFloat<3>(&l.minAngle, fp);
                freadFloat<3>(&l.maxAngle, fp);
            }
        }
    }
}

void Model::loadFromFile(const std::string &filename)
{
    GLMMD_LOG_INFO("Loading model from {}", filename);
    FILE *fp;
    fp = fopen(filename.c_str(), "rb");
    PMX_LOADER_THROW(!fp, "failed to load model");

    fread(info.header, 4, 1, fp);
    info.header[4] = '\0';
    PMX_LOADER_THROW(std::string(info.header) != "PMX ", "failed to load model");

    freadFloat(&info.version, fp);
    PMX_LOADER_THROW(info.version != 2.0f, "failed to load model");

    uint8_t bs;
    freadUint(bs, 1, fp);
    PMX_LOADER_THROW(bs != 8, "failed to load model");

    fread(info.byteSize, 1, 8, fp);

    freadTextBuf(info.modelName, fp);
    freadTextBuf(info.modelNameEN, fp);
    freadTextBuf(info.comment, fp);
    freadTextBuf(info.commentEN, fp);
    if (!info.isUtf8())
    {
        info.modelName = UTF16_LE_to_UTF8(info.modelName);
        info.modelNameEN = UTF16_LE_to_UTF8(info.modelNameEN);
        info.comment = UTF16_LE_to_UTF8(info.comment);
        info.commentEN = UTF16_LE_to_UTF8(info.commentEN);
    }

    uint32_t vertCount;
    freadUint(vertCount, 4, fp);
    vertices.resize(vertCount);
    for (auto &vert : vertices)
        freadVertex(vert, fp);

    uint32_t indexCount;
    freadUint(indexCount, 4, fp);
    indices.resize(indexCount);
    for (auto &i : indices)
        freadUint(i, info.vertexIndexSize(), fp);

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
        tp = modelFileDir + convertToNativeEncoding(rawPath, info.isUtf8() ? EncodingType::UTF8 : EncodingType::UTF16_LE);
    }

    uint32_t materialCount;
    freadUint(materialCount, 4, fp);
    materials.resize(materialCount);
    for (auto &m : materials)
        freadMaterial(m, fp);

    uint32_t boneCount;
    freadUint(boneCount, 4, fp);
    bones.resize(boneCount);
    for (auto &b : bones)
        freadBone(b, fp);
    fclose(fp);

    GLMMD_LOG_INFO("\n----------------\nModel Name: {}\nComment: {}\nVertex Count: {}\nIndex Count: {}\nMaterial Count: {}\n----------------",
                   info.modelName,
                   info.comment,
                   vertices.size(),
                   indices.size(),
                   materials.size());
}
} // namespace pmx