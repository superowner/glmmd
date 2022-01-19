#include <utils/GlobalConfig.h>
#include <utils/JsonParser.hpp>
GlobalConfig::GlobalConfig(const std::string &filename)
    : ScreenWidth(1440),
      ScreenHeight(960),
      ShadowMapWidth(2048),
      ShadowMapHeight(2048),
      AASamples(1),
      LightCamWidth(50.0f),
      LightCamHeight(50.0f),
      LightCamNear(1.0f),
      LightCamFar(40.0f),
      FontPath(""),
      FontSize(20.0f)
{
    JsonParser parser;
    if (!parser.loadFromFile(filename.c_str()))
        return;
    auto err = parser.parse();
    if (err.state != PARSE_SUCCESS)
        err.print();
    auto &root = parser.root();
    if (!root.isObj())
        return;
    fill(root, "ScreenWidth", &ScreenWidth, 'u');
    fill(root, "ScreenHeight", &ScreenHeight, 'u');
    fill(root, "ShadowMapWidth", &ShadowMapWidth, 'u');
    fill(root, "ShadowMapHeight", &ShadowMapHeight, 'u');
    fill(root, "AASamples", &AASamples, 'u');

    fill(root, "LightCamWidth", &LightCamWidth, 'f');
    fill(root, "LightCamHeight", &LightCamHeight, 'f');
    fill(root, "LightCamNear", &LightCamNear, 'f');
    fill(root, "LightCamFar", &LightCamFar, 'f');

    fill(root, "FontPath", &FontPath, 's');
    fill(root, "FontSize", &FontSize, 'f');
}

void GlobalConfig::fill(const JsonNode &node, const std::string &key, void *ptr, char ty)
{
    if (node.findKey(key))
    {
        switch (ty)
        {
        case 'u':
            *((unsigned int *)ptr) = node[key].toInt();
            break;
        case 'f':
            *((float *)ptr) = node[key].getNum();
            break;
        case 's':
            *((std::string *)ptr) = node[key].getStr();
        default:
            break;
        }
    }
}