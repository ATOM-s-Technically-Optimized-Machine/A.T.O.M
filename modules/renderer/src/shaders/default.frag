#version 410 core

in vec3 fragPos;

out vec4 FragColor;

void main() {
    vec3 normal = normalize(cross(dFdx(fragPos), dFdy(fragPos)));
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normal, lightDir), 0.0);
    float brightness = 0.15 + diff;
    FragColor = vec4(vec3(0.1, 0.9, 0.2) * brightness, 1.0);
}