#ifndef BUILD_H
#define BUILD_H


#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
using namespace std;

class BUILD 
{
public: 
    vector<float> vertices;
    unsigned int VBO, VAO;
    const unsigned int SCR_WIDTH = 1920;
    const unsigned int SCR_HEIGHT = 1080;


    BUILD(vector<float> vertices)
    {
        this->vertices = vertices;

        setupBuffers();
    }

    void draw(Shader& shader, Camera &camera, glm::vec3 flagPos) {

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        glm::mat4 flagModel = glm::mat4(1.0f);
        flagModel = glm::translate(flagModel, flagPos);
        shader.setMat4("model", flagModel);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        /*glBindVertexArray(0);*/
    }

private:

    void setupBuffers() {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
        // note that we update the lamp's position attribute's stride to reflect the updated buffer data
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
};


#endif

