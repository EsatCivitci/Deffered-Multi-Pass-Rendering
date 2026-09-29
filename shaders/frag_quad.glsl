#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D gTexture;

void main()
{
    vec3 color = texture(gTexture, TexCoords).rgb;

    FragColor = vec4(color, 1);  
}