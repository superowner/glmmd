#version 330 core

in vec3 norm;
in vec2 texCoord;
in vec3 fragPos;
in vec4 fragPosLightSpace;

struct Material
{
    vec4 diffuseColor;
    vec3 specularColor;
    float specIntensity;
    vec3 ambientColor;

    int hasDiffuseTex;
    sampler2D diffuseTex;

    int sphereTexMode;
    sampler2D sphereTex;

    int hasToon;
    sampler2D toonTex;
    
    vec4 edgeColor;
    float edgeSize;
};

struct DirectionalLight
{
    vec3 dir;
    vec3 diffuseLight;
    vec3 ambientLight;
};

uniform Material mat;
uniform DirectionalLight mainLight;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform sampler2D shadowMap;

out vec4 FragColor;

float calcShadow()
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    if (currentDepth > 1.0)
    return 0;
    float bias = max(0.002 * (1.0 - dot(norm, mainLight.dir)), 0.001);
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    float shadow = 0;
    int kernelRadius =0;
    for (int x = -kernelRadius; x <= kernelRadius; ++x)
    {
        for (int y = -kernelRadius; y <= kernelRadius; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= (2.0 * kernelRadius + 1.0) * (2.0 * kernelRadius + 1.0);
    return shadow;
}

void main()
{
    vec3 result;

    vec4 texColor;
    if (mat.hasDiffuseTex > 0)
    {
        texColor = texture(mat.diffuseTex, texCoord) * mat.diffuseColor.a;
    }
    else
    {
        texColor = mat.diffuseColor;
    }
    float opacity = texColor.a;
    if (opacity < 0.01) discard;

    float shadow = calcShadow();
    vec3 toonShadow = vec3(1.0, 1.0, 1.0);
    if (mat.hasToon > 0)
    toonShadow = texture(mat.toonTex, vec2(0.5, shadow*0.99)).xyz;

    vec3 _ambient = mainLight.ambientLight * mat.ambientColor;
    vec3 _diffuse = mainLight.diffuseLight * toonShadow;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 refDir = reflect(-mainLight.dir, norm);
    if (mat.sphereTexMode == 0 || mat.sphereTexMode == 3)
    {
        vec3 _specular = vec3(0.0, 0.0, 0.0);
        if (mat.hasDiffuseTex == 0)
        {
            float spec = pow(max(dot(viewDir, refDir), 0.0), max(mat.specIntensity, 1.0));
            vec3 _specular = spec * mat.specularColor;
        }
        result = texColor.xyz * (_ambient + _diffuse) + _specular;
    }
    else //if (mat.sphereTexMode < 3)
    {
        vec3 upVec = vec3(0.0, 1.0, 0.0);
        vec3 uAxis = normalize(cross(upVec, norm));
        vec3 vAxis = normalize(cross(norm, uAxis));
        float u = dot(uAxis, viewDir);
        float v = dot(vAxis, viewDir);
        vec4 _specular = texture(mat.sphereTex, vec2(0.5 + 0.5 * u, 0.5 + 0.5 * v));
        if (mat.sphereTexMode == 2)
        {
            result = texColor.xyz * (_ambient + _diffuse) + _specular.xyz;
        }
        else
        {
            result = texColor.xyz * (_ambient + _diffuse) * _specular.xyz;
            opacity *= _specular.a;
        }
    }
    FragColor = vec4(result, opacity);
}