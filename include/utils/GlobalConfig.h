#ifndef GLOBAL_CONFIG_H_
#define GLOBAL_CONFIG_H_

#include <string>

struct JsonNode;

struct GlobalConfig
{
    unsigned int ScreenWidth;
    unsigned int ScreenHeight;
    unsigned int ShadowMapWidth;
    unsigned int ShadowMapHeight;
    unsigned int AASamples;
    float LightCamWidth;
    float LightCamHeight;
    float LightCamNear;
    float LightCamFar;

    GlobalConfig(const std::string &filename);
    void fill(const JsonNode &, const std::string &key, void *ptr, char ty);
};

#endif