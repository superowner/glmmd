#include <pmx/VmdData.h>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <utils/CodeConverter.h>
#include <pmx/FileHelper.h>
void freadBoneKeyFrameRecord(BoneKeyFrameRecord &b, FILE *fp)
{
    char boneNameBuffer[16]{0};
    fread(boneNameBuffer, 1, 15, fp);
    b.boneName = shiftJIS_to_UTF8(std::string(boneNameBuffer));

    freadUint(b.frameTime, 4, fp);
    freadFloat<3>(&b.translation, fp);
    freadFloat<4>(&b.rotation, fp);

    uint32_t tmp;

    for (uint8_t i = 0; i < 4; ++i)
    {
        freadUint(b.xCurve[i], 1, fp);
        freadUint(tmp, 3, fp);
    }
    for (uint8_t i = 0; i < 4; ++i)
    {
        freadUint(b.yCurve[i], 1, fp);
        freadUint(tmp, 3, fp);
    }
    for (uint8_t i = 0; i < 4; ++i)
    {
        freadUint(b.zCurve[i], 1, fp);
        freadUint(tmp, 3, fp);
    }
    for (uint8_t i = 0; i < 4; ++i)
    {
        freadUint(b.rCurve[i], 1, fp);
        freadUint(tmp, 3, fp);
    }
}
void freadMorphKeyFrameRecord(MorphKeyFrameRecord &m, FILE *fp)
{
    char morphNameBuffer[16]{0};
    fread(morphNameBuffer, sizeof(char), 15, fp);
    m.morphName = shiftJIS_to_UTF8(std::string(morphNameBuffer));
    freadUint(m.frameTime, 4, fp);
    freadFloat<1>(&m.weight, fp);
}
void VmdData::loadFromFile(const std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp)
        throw std::runtime_error("failed to load vmd data");
    char header[31];
    fread(header, 1, 30, fp);
    header[30] = '\0';
    if (!strcmp(header, "Vocaloid Motion Data file"))
        ver = 1;
    else if (!strcmp(header, "Vocaloid Motion Data 0002"))
        ver = 2;
    else
        throw std::runtime_error("failed to load vmd data");

    char nameBuffer[21]{0};
    fread(nameBuffer, 1, ver * 10, fp);
    modelName = shiftJIS_to_UTF8(std::string(nameBuffer));

    uint32_t boneKeyFrameCount;
    freadUint(boneKeyFrameCount, 4, fp);
    boneKeyFrameList.resize(boneKeyFrameCount);
    for (auto &b : boneKeyFrameList)
        freadBoneKeyFrameRecord(b, fp);

    uint32_t morphKeyFrameCount;
    freadUint(morphKeyFrameCount, 4, fp);
    morphKeyFrameList.resize(morphKeyFrameCount);
    for (auto &m : morphKeyFrameList)
        freadMorphKeyFrameRecord(m, fp);
}
