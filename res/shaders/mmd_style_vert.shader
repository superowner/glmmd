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

uniform int boneCount;
uniform sampler2D boneTransformTex;

mat4 getTransformMat(float index);

void main()
{
    mat4 modelVert = model;

    if (boneCount > 0)
    {
        if (deformMethod == 1)
        {
            modelVert = modelVert * getTransformMat(bindedBone.x);
        }
        else if (deformMethod == 2)
        {
            modelVert = modelVert * (weights.x * getTransformMat(bindedBone.x) + (1.0 - weights.x) * getTransformMat(bindedBone.y));
        }
        else if (deformMethod == 4)
        {
            modelVert = modelVert * (weights.x * getTransformMat(bindedBone.x) 
            + weights.y * getTransformMat(bindedBone.y) 
            + weights.z * getTransformMat(bindedBone.z) 
            + weights.w * getTransformMat(bindedBone.w));
        }
    }
    gl_Position = projection * view * modelVert * vec4(aPos, 1.0);
    norm = normalize(mat3(transpose(inverse(modelVert))) * aNormal);
    texCoord = aTexCoord;
    fragPos = vec3(modelVert * vec4(aPos, 1.0));
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
}

mat4 getTransformMat(float index)
{
    mat4 result;
    for (int i = 0; i < 4; i++)
    {
        vec4 m = texture(boneTransformTex, vec2(0.25 * (i + 0.5), (index + 0.5) / boneCount));
        result[i][0] = m.x;
        result[i][1] = m.y;
        result[i][2] = m.z;
        result[i][3] = m.w;
    }
    return result;
}