#version 330 core

uniform samplerCube myCubeSampler;
uniform float exposure;

in vec3 texCoord;

out vec4 fragColor;

void main(void)
{
	vec3 color = texture(myCubeSampler, texCoord).rgb * exposure;
	fragColor = vec4(color, 1.0);
}