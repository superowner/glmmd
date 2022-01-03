#version 330 core

in vec3 norm;
in vec2 texCoord;

struct Material
{
    vec4 diffuse;
    vec3 specular;
    float specularIntensity;
    vec3 ambient;

    int hasDiffuseTex;
    sampler2D diffuseTex;

    int sphereTexMode;
    sampler2D sphereTex;

    int hasToon;
    sampler2D toonTex;
    
    vec4 edgeColor;
    float edgeSize;
}

uniform Material mat;

void main()
{
    vec4 texColor;
    if (mat.hasDiffuseTex > 0)
        texColor = texture(mat.diffuseTex, texCoord);
    else
        texColor = mat.diffuse;

    if (texColor.a * mat.diffuse.a < 0.01)
        discard;


}