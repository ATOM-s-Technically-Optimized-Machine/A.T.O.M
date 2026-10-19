#ifndef RENDERER_HPP_
    #define RENDERER_HPP_

#include "resources/ResourceTypes.hpp"
#include <string>

namespace renderer {

    unsigned int loadShaderProgram(
        const std::string &vertPath,
        const std::string &fragPath,
        const std::string &defines = ""
    );
    unsigned int loadTextureFromImageData(const resources::ImageData &data);

    [[deprecated("Use resources::loadTexture instead")]]
    unsigned int loadTexture(const std::string &textPath);

}

#endif /* !RENDERER_HPP_ */
