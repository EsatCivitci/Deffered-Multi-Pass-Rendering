#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D myColorSampler;
uniform bool enableBlur;
uniform bool isRotating;
uniform int blurSize;

void main()
{        
    vec2 texelSize = 1.0 / vec2(textureSize(myColorSampler, 0));
    vec3 sum = vec3(0);
    if (enableBlur) {


        for (int x = -blurSize; x <= blurSize; ++x) {
            for (int y = -blurSize; y <= blurSize; ++y) {
                sum += texture(myColorSampler, TexCoords + (vec2(x,y) * texelSize)).rgb;
            }
        }

        float denom = (blurSize*2+1) * (blurSize*2+1);
        sum = sum / denom;
    }

    else {
        sum = texture(myColorSampler, TexCoords).rgb;  
    }

    float lum = dot(sum, vec3(0.2126, 0.7152, 0.0722));
    float logLum = log(max(lum, 1e-6));

    FragColor = vec4(sum, logLum);
}
