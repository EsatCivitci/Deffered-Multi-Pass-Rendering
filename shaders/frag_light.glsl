#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform vec3 eyePos;  
uniform float exposure;

vec3 I = vec3(1, 1, 1);          // point light intensity
vec3 Iamb = vec3(0.8, 0.8, 0.8); // ambient light intensity
vec3 kd = vec3(0.7, 0, 1);     // diffuse reflectance coefficient
vec3 ka = vec3(0.3, 0.3, 0.3);   // ambient reflectance coefficient
vec3 ks = vec3(0.8, 0.8, 0.8);   // specular reflectance coefficient
vec3 lightPos = vec3(0, 0, 100);   // light position in world coordinates


void main()
{
    // Read G-buffer textures
    vec3 fragWorldPos = texture(gPosition, TexCoords).rgb;
    vec3 fragWorldNor  = normalize(texture(gNormal, TexCoords).rgb);

	// Add this to the block for combining the render of cubemap and armadillo
    if (length(fragWorldPos) < 0.001)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0); // fully transparent
        return;
    }

    vec3 L = normalize(lightPos - vec3(fragWorldPos));
	vec3 V = normalize(eyePos - vec3(fragWorldPos));
	vec3 H = normalize(L + V);
	vec3 N = normalize(fragWorldNor);

	float NdotL = dot(N, L); // for diffuse component
	float NdotH = dot(N, H); // for specular component

	vec3 diffuseColor = I * kd * max(0, NdotL);
	vec3 specularColor = I * ks * pow(max(0, NdotH), 100);
	vec3 ambientColor = vec3(0, 0, 0);

	vec3 color = diffuseColor + specularColor + ambientColor;

	FragColor = vec4(color * exposure, 1);
}
