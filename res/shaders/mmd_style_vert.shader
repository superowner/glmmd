#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float deformMethod;
layout (location = 4) in vec4 bindedBone;
layout (location = 5) in vec4 weights;

out vec3 norm;
out vec2 texCoord;
out vec3 fragPos;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform sampler2D boneTransform;

int boneCount;
mat4 getTransformMat(float index)
{
    mat4 result;
    for (int i = 0; i < 4; i++)
    {
        vec4 m = texture(boneTransform, vec2(0.25 * (i + 0.5), (index + 0.5) / boneCount));
        result[i][0] = m.x;
        result[i][1] = m.y;
        result[i][2] = m.z;
        result[i][3] = m.w;
    }
    return result;
}

out vec3 testColor;

void main()
{
    mat4 modelVert;
    boneCount = textureSize(boneTransform, 0).y;
    if (deformMethod == 1)
    {
        modelVert = model * getTransformMat(bindedBone.x);
    }
    else if (deformMethod == 2)
    {
        modelVert = model * (weights.x * getTransformMat(bindedBone.x) + (1.0 - weights.x) * getTransformMat(bindedBone.y));
    }
    else if (deformMethod == 4)
    {
        modelVert = model * (weights.x * getTransformMat(bindedBone.x) 
        + weights.y * getTransformMat(bindedBone.y) 
        + weights.z * getTransformMat(bindedBone.z) 
        + weights.w * getTransformMat(bindedBone.w));
    }

    gl_Position = projection * view * modelVert * vec4(aPos, 1.0);
    norm = normalize(mat3(transpose(inverse(model))) * aNormal);
    texCoord = aTexCoord;
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    testColor =  vec3(modelVert[0][0], modelVert[1][1], modelVert[2][2]);
}