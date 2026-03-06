#version 410 core

uniform sampler2D uTexture;

in vec3 fragPos;
in vec2 fragUV;

out vec4 FragColor;

void main() {
    vec3 normal = normalize(cross(dFdx(fragPos), dFdy(fragPos)));
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normal, lightDir), 0.0);
    float brightness = 0.15 + diff;
    FragColor = texture(uTexture, fragUV);
    FragColor.rgb *= brightness;
}
