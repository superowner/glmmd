#version 330 core
layout (location = 0) in vec3 position;
layout (location = 3) in float deformMethod;
layout (location = 4) in vec4 bindedBone;
layout (location = 5) in vec4 weights;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

uniform int boneCount;
uniform sampler2D boneTransformTex;

mat4 getTransformMat(float index);

void main()
{
    mat4 modelVert = model;
    if (boneCount != 0){
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
    gl_Position = lightSpaceMatrix * modelVert * vec4(position, 1.0f);
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