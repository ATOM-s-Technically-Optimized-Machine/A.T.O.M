#include <renderer/glad/glad.h>
#include "renderer/Renderer.hpp"
#include "renderer/stb_image.h"
#include <fstream>
#include <iterator>
#include <iostream>

namespace renderer {

unsigned int loadShaderProgram(const std::string &vertPath, const std::string &fragPath)
{
    std::ifstream vert_stream(vertPath);
    std::ifstream frag_stream(fragPath);

    std::string vert_shader((std::istreambuf_iterator<char>(vert_stream)), std::istreambuf_iterator<char>());
    std::string frag_shader((std::istreambuf_iterator<char>(frag_stream)), std::istreambuf_iterator<char>());

    const char *vert_raw = vert_shader.c_str();
    const char *frag_raw = frag_shader.c_str();

    int success;
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vert_raw, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        std::cerr << "Shader error: " << log << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &frag_raw, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
        std::cerr << "Shader error: " << log << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int loadTexture(const std::string &textPath)
{
    int w, h, channels;
    unsigned char *data = stbi_load(textPath.c_str(), &w, &h, &channels, 0);
    unsigned int tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return tex;
}

}