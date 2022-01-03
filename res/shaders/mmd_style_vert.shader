#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 norm;
out vec2 texCoord;
out vec3 fragPos;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    norm = normalize(mat3(transpose(inverse(model))) * aNormal);
    texCoord = aTexCoord;
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
}