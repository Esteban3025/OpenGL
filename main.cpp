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

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

float planeSize = 40.0f;
float flagSize = 3.0f;
float cubeSize = 2.5f;
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cubePos = glm::vec3(0.0f, -1.5f, 0.0f);
glm::vec3 flagPos = glm::vec3(0.0f, 0.0f, 0.0f);
Camera camera(camPos);

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
    /*glDepthFunc(GL_LESS);*/

    stbi_set_flip_vertically_on_load(true);

    Shader modelShader("shaders/model.vs", "shaders/model.fs");
    Shader flagShader("shaders/flagShader.vs", "shaders/flagShader.fs");
    Shader cubeShader("shaders/cubeShader.vs", "shaders/cubeShader.fs");
    Shader gunShader("shaders/gunShader.vs", "shaders/gunShader.fs");

    Model ourModel("textures/sponza/sponza.obj");
    Model gunModel("textures/shotgun/shotgun_1.obj");
    
    unsigned int cubeTexture = loadTexture("textures/wooden_crate_4.png");

    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
  

    // Flag and lights data 
    unsigned int ligthCubeVBO, ligthCubeVAO;
    glGenVertexArrays(1, &ligthCubeVAO);
    glGenBuffers(1, &ligthCubeVBO);
    glBindVertexArray(ligthCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, ligthCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // cube random data
    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    flagShader.use();

    cubeShader.use();
    cubeShader.setInt("texture1", 0);

    modelShader.use();

    gunShader.use();
    

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

        // CALCULATION FOR MAKE A VIEW FOR PARENT-CHILD RELATIONSHIP
        glm::mat4 cameraWorld = glm::translate(glm::mat4(1.0f), camPos);
        glm::mat4 cameraRotation = camera.getCameraRotation();
        cameraRotation = glm::inverse(cameraRotation);
        cameraWorld *= cameraRotation;

        glm::mat4 worldView = glm::inverse(cameraWorld);

        // glm::mat4 cubeWorld = cameraWorld * modelOfTheChildObject;
        // modelShaderOfTheCHild.setMat4("model", cubeWorld);
        // modelShaderOfTheCHild.setMat4("view", worldView);

        modelShader.use();
        
        modelShader.setVec3("viewPos", camera.Position);

        // light properties
        modelShader.setVec3("light.position", flagPos);
        modelShader.setVec3("light.ambient", 0.02f, 0.02f, 0.02f);
        modelShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        modelShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("light.constant", 1.0f);
        modelShader.setFloat("light.linear", 0.09f);
        modelShader.setFloat("light.quadratic", 0.032f);

        // material properties
        modelShader.setFloat("material.shininess", 32.0f);
        modelShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        modelShader.setMat4("model", model);
        ourModel.Draw(modelShader);

        // ************************* GUN MODEL  *******************************************

        gunShader.use();

        // view/projection transformations
        gunShader.setMat4("projection", projection);
        gunShader.setMat4("view", worldView);

        // render the loaded model
        glm::mat4 gunModelMatrix = glm::mat4(1.0f);
        gunModelMatrix = glm::translate(gunModelMatrix, glm::vec3(0.4f, -0.1f, -1.0f)); // translate it down so it's at the center of the scene
        /*gunModelMatrix = glm::scale(gunModelMatrix, glm::vec3(0.01f, 0.01f, 0.01f));*/
        gunModelMatrix = glm::rotate(gunModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 1.0f));

        glm::mat4 cubeWorld = cameraWorld * gunModelMatrix;
        gunShader.setMat4("model", cubeWorld);
        gunModel.Draw(gunShader);
        
        
        /// ************************ FLAG LIGHTSS THINGS ******s****************************
        flagShader.use();
        flagShader.setMat4("projection", projection);
        flagShader.setMat4("view", view);

        glm::mat4 flagModel = glm::mat4(1.0f);
        flagModel = glm::translate(flagModel, flagPos);
        flagModel = glm::scale(flagModel, glm::vec3(0.1f, 0.1f, 0.1f));
        flagShader.setMat4("model", flagModel);

        glBindVertexArray(ligthCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        /// ************************  RANDOM CUBE THINGS **********************************

        cubeShader.use();

        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);

        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.0f, 0.0f));
        cubeModel = glm::scale(cubeModel, glm::vec3(0.3f, 0.3f, 0.3f));

        cubeShader.setMat4("model", cubeModel);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        ImGui::Begin("Esto se supone que es una ventana con imgui");
        io.MouseDrawCursor = true;
        ImGui::Text("Hola mundo de la programcion grafica.");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        lastFrame = currentFrame;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &ligthCubeVAO);
    glDeleteBuffers(1, &ligthCubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVAO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, ImGuiIO& io)
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

    // ESCAPE THE CURSOR 
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        /*glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);*/
        io.MouseDrawCursor = false;
        glfwSetCursorPosCallback(window, mouse_callback);
    }


    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        io.MouseDrawCursor = true;
        /*glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);*/
        glfwSetCursorPosCallback(window, mouse_callback);
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

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        flagPos.z += flagMovementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        flagPos.z -= flagMovementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        flagPos.x += flagMovementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        flagPos.x -= flagMovementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        flagPos.y -= flagMovementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        flagPos.y += flagMovementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        flagPos.x = 0.0f;
        flagPos.z = 0.0f;
        flagPos.y = 0.0f;
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


