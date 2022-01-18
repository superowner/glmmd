#ifndef GLOBAL_CONFIG_H_
#define GLOBAL_CONFIG_H_

#include <string>

struct GlobalConfig
{
    unsigned int ScreenWidth;
    unsigned int ScreenHeight;
    unsigned int ShadowMapWidth;
    unsigned int ShadowMapHeight;
    unsigned int AASamples;

    GlobalConfig(const std::string &filename);
};

#endif