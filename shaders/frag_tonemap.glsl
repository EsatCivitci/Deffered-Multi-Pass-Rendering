#version 330 core

uniform sampler2D myColorSampler;
uniform float key;        
uniform bool  enableGamma;
uniform int   level;

in  vec2 TexCoords;
out vec4 FragColor;

void main() {
    float avgLogL = textureLod(myColorSampler, vec2(0.5), level).a;
    float avgL    = exp(avgLogL);

    vec3 hdr = textureLod(myColorSampler, TexCoords, 0.0).rgb;
    float Li = dot(hdr, vec3(0.2126, 0.7152, 0.0722)); 

    float Ls = Li * (key / avgL);
    float Ltm = Ls / (1+Ls);

    vec3 color = (hdr / Li) * Ltm;

    if (enableGamma) {
        color = pow(color, vec3(1.0/2.2));
    }

    FragColor = vec4(color, 1.0);
}