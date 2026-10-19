#version 410 core

#ifdef HAS_UV
    layout (location = 2) in vec2 aUV;
    out vec2 fragUV;
#endif

#ifdef HAS_COLOR
    layout (location = 1) in vec3 aCOLOR;
    out vec3 fragColor;
#endif

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;

void main() {
    fragPos = vec3(model * vec4(aPos, 1.0));

    #ifdef HAS_UV
        fragUV = aUV;
    #endif

    #ifdef HAS_COLOR
        fragColor = aCOLOR;
    #endif
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
