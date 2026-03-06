#ifndef RENDERER_HPP_
    #define RENDERER_HPP_

#include <string>

namespace renderer {

    unsigned int loadShaderProgram(const std::string &vertPath, const std::string &fragPath);
    unsigned int loadTexture(const std::string &textPath);

}

#endif /* !RENDERER_HPP_ */
