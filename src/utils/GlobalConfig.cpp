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
      LightCamFar(40.0f)
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
    if (root.findKey("ScreenWidth"))
        ScreenWidth = root["ScreenWidth"].toInt();
    if (root.findKey("ScreenHeight"))
        ScreenHeight = root["ScreenHeight"].toInt();
    if (root.findKey("ShadowMapWidth"))
        ShadowMapWidth = root["ShadowMapWidth"].toInt();
    if (root.findKey("ShadowMapHeight"))
        ShadowMapHeight = root["ShadowMapHeight"].toInt();
    if (root.findKey("AASamples"))
        AASamples = root["AASamples"].toInt();
    if (root.findKey("LightCamWidth"))
        LightCamWidth = root["LightCamWidth"].getNum();
    if (root.findKey("LightCamHeight"))
        LightCamHeight = root["LightCamHeight"].getNum();
    if (root.findKey("LightCamNear"))
        LightCamNear = root["LightCamNear"].getNum();
    if (root.findKey("LightCamFar"))
        LightCamFar = root["LightCamFar"].getNum();
}