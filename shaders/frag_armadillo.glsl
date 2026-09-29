#version 330 core

in vec3 fragWorldPos;
in vec3 fragWorldNor;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;

void main(void)
{
    gPosition = vec4(fragWorldPos,1);
    gNormal = vec4(normalize(fragWorldNor), 0.0);
}
