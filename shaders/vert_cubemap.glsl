#version 330 core

layout(location = 0) in vec3 inVertex;

uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;

out vec3 texCoord;

void main(void)
{
	texCoord = inVertex;
	vec4 P = projectionMatrix * viewingMatrix * vec4(inVertex, 1.0);
	gl_Position = vec4(P.xy,  P.w,  P.w);
}

