#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool enableHdr;
uniform float hdrExposure;

void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
    if (enableHdr)
    {
        const float gamma = 2.2;
        vec3 hdrColor = pow(FragColor.rgb, vec3(gamma));
        vec3 result = pow(vec3(1.0) - exp(-hdrColor * hdrExposure), vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}