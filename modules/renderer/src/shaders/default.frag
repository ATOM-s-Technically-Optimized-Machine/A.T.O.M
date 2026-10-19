#version 410 core

in vec3 fragPos;

#ifdef HAS_UV
    uniform sampler2D uTexture;
    in vec2 fragUV;
#endif

out vec4 FragColor;

void main() {
    #ifdef HAS_UV
        FragColor = texture(uTexture, fragUV);
    #else
        FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    #endif

    vec3 normal = normalize(cross(dFdx(fragPos), dFdy(fragPos)));
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normal, lightDir), 0.0);
    float brightness = 0.15 + diff;
    FragColor.rgb *= brightness;
}
