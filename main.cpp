#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <tools/shader.h>
#include <tools/camera.h>
#include <tools/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, ImGuiIO& io);
unsigned int loadTexture(char const* path);
void setupMaterials(Shader shader, unsigned int diffuse, unsigned int specular, float shinness);
void initView(Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 camPosition);
void setupLights(Shader shader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 lightPosition, glm::vec3 lightDirection, float outerCutOff,
    float cutOff, float constant, float linear, float quadratic);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

glm::vec3 camPos = glm::vec3(1.0f, 0.0f, 10.0f);
Camera camera(camPos);

// Propierties
float lightSize = 1.0;

glm::vec3 lightPos = glm::vec3(8.0f, 15.3f, 15.0f);
glm::vec3 lightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

glm::vec3 globalAmbient = glm::vec3(0.3f);

float cubeSize = 2.5f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool fistMouse = true;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_WIDTH / 2.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfwGetPrimaryMonitor(); full screen
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
    glDepthFunc(GL_LESS);  

    stbi_set_flip_vertically_on_load(true);

    
    Shader floorShader("shaders/plane.vs", "shaders/plane.fs");
    Shader cubesShader("shaders/cube.vs", "shaders/cube.fs");
    Shader lightShader("shaders/light.vs", "shaders/light.fs");

    float cubeVertices[] = {
    // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // CAMBIA LOS SHADERS DEL PISO PORQUE LAS NORMALES ESTAN OFF

    float planeVertices[] = {
        // positions        // normals         // texture Coords
        0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,        1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,        1.0f, 0.0f,
       -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,        0.0f, 1.0f,

       -0.5,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,        0.0f, 1.0f,
        0.5, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,        1.0f, 0.0f,
       -0.5, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,        0.0f, 0.0f,
    };
 

    float lightVertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };

    // positions all containers
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0, -1.5, 5.0f),
        glm::vec3(5.0, -1.5, 5.0f),
        glm::vec3(10.0, -1.5, 5.0f),

        glm::vec3(0.0, -1.5, 0.0f),
        glm::vec3(5.0, -1.5, 0.0f),
        glm::vec3(10.0, -1.5, 0.0f),
    
        glm::vec3(0.0, -1.5, -5.0f),
        glm::vec3(5.0, -1.5, -5.0f),
        glm::vec3(10.0, -1.5, -5.0f),
    };

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // plane things
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int floorTexture = loadTexture("textures/wall.jpg");
    unsigned int flashLightTexture = loadTexture("textures/flashlightpattern.png");
    unsigned int cubesTexture = loadTexture("textures/container2.png");
    unsigned int cubeSpecular = loadTexture("textures/container2_specular.png");

    floorShader.use();

    cubesShader.use();

    lightShader.use();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window, io);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // set uniforms
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::vec3 cubeColor = globalAmbient;

        // floor/plane things
        floorShader.use();
        initView(floorShader, view, projection, camera.Position);
        setupMaterials(floorShader, 0, 0, 32.0f);
        setupLights(floorShader, globalAmbient, glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), camera.Position, camera.Front, 17.5f, 12.5f, 1.0f, 0.07f, 0.017f);

        // cube propierties
        cubesShader.use();
        initView(cubesShader, view, projection, camera.Position);
        setupMaterials(cubesShader, 2, 3, 32.0f);
        setupLights(cubesShader, globalAmbient, glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), camera.Position, camera.Front, 17.5f, 12.5f, 1.0f, 0.07f, 0.017f);

        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", lightColor);

        // lights things
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(lightSize, lightSize, lightSize));
        lightShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // plane things
        floorShader.use();
        glBindVertexArray(floorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, flashLightTexture);
        floorShader.setInt("material.flashLight", 1);
        glm::mat4 floorModel = glm::mat4(1.0f);
        floorModel = glm::translate(floorModel, glm::vec3(0.0f, -3.0f, 0.0f));
        floorModel = glm::rotate(floorModel, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        float planeSize = 40.0f;
        floorModel = glm::scale(floorModel, glm::vec3(planeSize, planeSize, planeSize));
        floorShader.setMat4("model", floorModel);
        glDrawArrays(GL_TRIANGLES, 0, 10);
        glBindVertexArray(0);

        // cubes things
        cubesShader.use();
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cubesTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, cubeSpecular);

        for (unsigned int i = 0; i < 9; i++) {
            glm::mat4 cubeModel = glm::mat4(1.0f);
            cubeModel = glm::translate(cubeModel, cubePositions[i]);
            cubeModel = glm::scale(cubeModel, glm::vec3(cubeSize, cubeSize, cubeSize));
            cubesShader.setMat4("model", cubeModel);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        ImGui::Begin("Esto se supone que es una ventana con imgui");
        ImGui::Text("Hola mundo de la programcion grafica.");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        lastFrame = currentFrame;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &floorVAO);
    glDeleteVertexArrays(1, &floorVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &cubeVBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void setupMaterials(Shader shader, unsigned int diffuse, unsigned int specular, float shinness) 
{
    shader.setInt("material.diffuse", diffuse);
    shader.setInt("material.specular", specular);
    shader.setFloat("material.shininess", shinness);
}

void initView(Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 camPosition)
{
   shader.setMat4("view", view);
   shader.setMat4("projection", projection);
   shader.setVec3("viewPos", camPosition);
}

void setupLights(Shader shader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 lightPosition, glm::vec3 lightDirection, float outerCutOff,
    float cutOff, float constant, float linear, float quadratic)
{

    shader.setVec3("light.ambient", ambient);
    shader.setVec3("light.diffuse", diffuse);
    shader.setVec3("light.specular", specular);
    shader.setVec3("light.position", lightPosition);
    shader.setVec3("light.direction", lightDirection);

    shader.setFloat("light.outerCutOff", glm::cos(glm::radians(outerCutOff)));
    shader.setFloat("light.cutOff", glm::cos(glm::radians(cutOff)));
    shader.setFloat("light.constant", constant);
    shader.setFloat("light.linear", linear);
    shader.setFloat("light.quadratic", quadratic);
}

void processInput(GLFWwindow* window, ImGuiIO& io)
{
    const float cameraSpeed = 2.5f * deltaTime;
    const float movementSpeed = sin(0.009f);

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

    // reset camera

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        camPos.z = 0.0f;
        camPos.x = 0.0f;
        camPos.y = 0.0f;
        camera.Position = camPos;
    }


    // flag/Light movement

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        lightPos.z -= movementSpeed;
        camera.showVec3("Light Position: ", lightPos);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        lightPos.z += movementSpeed;
        camera.showVec3("Light Position: ", lightPos);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        lightPos.x += movementSpeed;
        camera.showVec3("Light Position: ", lightPos);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        lightPos.x -= movementSpeed;
        camera.showVec3("Light Position: ", lightPos);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        lightPos.y -= movementSpeed;
        camera.showVec3("Light Position: ", lightPos);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        lightPos.y += movementSpeed;
        camera.showVec3("Light Position: ", lightPos);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        lightPos.x = 0.0f;
        lightPos.z = 0.0f;
        lightPos.y = 0.0f;
        camera.showVec3("Light Position: ", lightPos);
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

