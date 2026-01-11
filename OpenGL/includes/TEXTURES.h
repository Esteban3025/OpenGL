#ifndef TEXTURES_H
#define TEXTURES_H

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <stb_image.h>
#include <iostream>

class Textures {
public :
    int width, height, nrChannels;
    unsigned int textureID;
    std::string Name;

    Textures(const std::string& name, const std::string& path)
    {
        Name = name;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            std::cout << "Texture data load: " << name.c_str() << std::endl;
        }
        else
        {
            std::cout << "Failed to load the " << name.c_str() << " texture" << std::endl;
        }

        stbi_image_free(data);
    }

    void use(GLuint textureUnit = GL_TEXTURE0) {
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    GLuint seeId() const 
    {
        return textureID;
    }


    ~Textures() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
        }
    }
};

#endif