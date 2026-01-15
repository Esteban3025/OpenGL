#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <map>

#include <shaders/SHADER.h>
#include <CAMERA.h>
#include <TEXTURES.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

float planeSize = 40.0f;
float flagSize = 3.0f;
float cubeSize = 2.5f;
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cubePos = glm::vec3(0.0f, -1.5f, 0.0f);
Camera camera(camPos);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_WIDTH / 2.0f;

glm::vec3 cameraPosition = camera.getPosition();
glm::vec3 lastCamPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 lightPos(0.0f, 0.0f, -10.0f);
glm::vec3 emissionColor(0.0f, 0.0f, 0.5f);

unsigned int VBOs[3], VAOs[3], EBO;

bool fistMouse = true;

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;

unsigned int VAOt, VBOt;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfwGetPrimaryMonitor();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GameEngine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);
    int monitorWidth = mode->width;
    int monitorHeight = mode->height;
    std::cout << "Primary monitor width: " << monitorWidth << " pixels" << std::endl;
    std::cout << "Primary monitor height: " << monitorHeight << " pixels" << std::endl;


    Shader ourShader("shaders/shader.vs", "shaders/shader.fs");
    Shader flagShader("shaders/flagShader.vs", "shaders/flagShader.fs");
    Shader cubeShader("shaders/cubeShader.vs", "shaders/cubeShader.fs");

    unsigned int diffuseMap = loadTexture("textures/container2.png");
    unsigned int floorTexture = loadTexture("textures/tiles_floor_1.png");
    unsigned int cubeSpecular = loadTexture("textures/container2_specular.png");
    unsigned int emission = loadTexture("textures/map.png");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 Positions[] = {
        glm::vec3(5.5f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    float planeVertices[] = {
        // positions          // texture coords    // normals
         0.9f,  0.9f, 0.0f,   1.0f, 1.0f,        0.0f, 0.0f, 1.0f,  // top right
         0.9f, -0.9f, 0.0f,   1.0f, 0.0f,        0.0f, 0.0f, 1.0f,   // bottom right
        -0.9f, -0.9f, 0.0f,   0.0f, 0.0f,        0.0f, 0.0f, 1.0f,  // bottom left
        -0.9f,  0.9f, 0.0f,   0.0f, 1.0f,        0.0f, 0.0f, 1.0f,  // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    float flagVertices[] = {
        // positions         
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(10.0f,  0.2f,  10.0f),
        glm::vec3(2.5f, -0.3f, -0.4f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    glGenVertexArrays(3, VAOs);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);

    // floor things
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // cubes things 
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // flag things
    glBindVertexArray(VAOs[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(flagVertices), flagVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
   
    
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    ourShader.setInt("floorTexture", 0);
    ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    ourShader.setVec3("objectColor", glm::vec3(5.0f, 0.8f, 3.0f));
    ourShader.setVec3("viewPos", camera.Position);
    ourShader.setVec3("lightPos", pointLightPositions[0]);
    

    cubeShader.use();   
    cubeShader.setInt("material.diffuse", 1);
    cubeShader.setInt("material.specular", 2);
    cubeShader.setInt("material.emission", 3);  
    

    flagShader.use();
   

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
     
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        ourShader.use();

        glBindVertexArray(VAOs[0]);  // floor things
        glm::mat4 planeModel = glm::mat4(1.0f);
        planeModel = glm::translate(planeModel, glm::vec3(0.0f, -3.0f, 0.0f));
        planeModel = glm::rotate(planeModel, glm::radians(-90.0f), glm::vec3(5.0f, 0.0f, 0.0f));
        planeModel = glm::scale(planeModel, glm::vec3(planeSize, planeSize, planeSize));
        ourShader.setMat4("model", planeModel);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view = camera.GetViewMatrix();

        glm::vec3 currentCamPosition = camera.getPosition();

        if (currentCamPosition != lastCamPosition)
        {
            /*std::cout << "Camera Position" << std::endl;
            std::cout << "x: " << cameraPosition.x << " " << "y: " << cameraPosition.y << " " << "z: " << cameraPosition.z << std::endl;
            std::cout << "Last Camera Position" << std::endl;
            std::cout << "x: " << std::round(lastCamPosition.x) << " " << "y: " << std::round(lastCamPosition.y) << " " << "z: " << std::round(lastCamPosition.z) << std::endl;*/

            lastCamPosition = currentCamPosition;
        }
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        cubeShader.use();
        cubeShader.setVec3("viewPos", camera.Position);
        cubeShader.setFloat("material.shininess", 64.0f);

        // directional light
        cubeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        cubeShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        cubeShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        cubeShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        cubeShader.setFloat("pointLights[0].constant", 1.0f);
        cubeShader.setFloat("pointLights[0].linear", 0.09f);
        cubeShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        cubeShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        cubeShader.setFloat("pointLights[1].constant", 1.0f);
        cubeShader.setFloat("pointLights[1].linear", 0.09f);
        cubeShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        cubeShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        cubeShader.setFloat("pointLights[2].constant", 1.0f);
        cubeShader.setFloat("pointLights[2].linear", 0.09f);
        cubeShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        cubeShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        cubeShader.setFloat("pointLights[3].constant", 1.0f);
        cubeShader.setFloat("pointLights[3].linear", 0.09f);
        cubeShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        cubeShader.setVec3("spotLight.position", camera.Position);
        cubeShader.setVec3("spotLight.direction", camera.Front);
        cubeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        cubeShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        cubeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        cubeShader.setFloat("spotLight.constant", 1.0f);
        cubeShader.setFloat("spotLight.linear", 0.09f);
        cubeShader.setFloat("spotLight.quadratic", 0.032f);
        cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        cubeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

       
        glBindVertexArray(VAOs[1]); 

        for (int i = 0; i < 2; i++)
        {
            glm::mat4 cubeModel = glm::mat4(1.0f);
            cubeModel = glm::translate(cubeModel, Positions[i]);

            /*cubeModel = glm::rotate(cubeModel, (float)glfwGetTime(), glm::vec3(0.5f, 0.9f, 1.0f));*/
            cubeModel = glm::scale(cubeModel, glm::vec3(cubeSize, cubeSize, cubeSize));

            cubeShader.setMat4("model", cubeModel);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cubeSpecular);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, emission);
       
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);

        flagShader.use();

        glBindVertexArray(VAOs[2]); // flag|triangle things 

        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 flagModel = glm::mat4(1.0f);
            flagModel = glm::translate(flagModel, pointLightPositions[i]);
            flagModel = glm::scale(flagModel, glm::vec3(0.2f)); // Make it a smaller cube
            flagShader.setMat4("model", flagModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        /*lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;*/

        emissionColor.y = sin(glfwGetTime()) * 1.0f;


        /*lightPos.z = -45.0f + sin(glfwGetTime()) * 100.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;*/
      
        flagShader.setMat4("view", view);
        flagShader.setMat4("projection", projection);
        
        lastFrame = currentFrame;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(4, VAOs);
    glDeleteBuffers(4, VBOs);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    const float cameraSpeed = 2.5f * deltaTime;
    const float flagMovementSpeed = 0.01f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RUN, deltaTime);
    }


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }



    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        lightPos.x = lightPos.x + sin(flagMovementSpeed);
    }  
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        lightPos.x = lightPos.x - sin(flagMovementSpeed);
    }  
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        lightPos.z = lightPos.z - sin(flagMovementSpeed);
    }  
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        lightPos.z = lightPos.z + sin(flagMovementSpeed);
    }  
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        lightPos.y = lightPos.y + sin(flagMovementSpeed);
    }  
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        lightPos.y = lightPos.y - sin(flagMovementSpeed);
    }  
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        lightPos.x = 0.0f;
        lightPos.y = 0.0f;
        lightPos.z = 0.0f;
    }  
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);


    if (fistMouse) {
        lastX = xpos;
        lastY = ypos;
        fistMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
