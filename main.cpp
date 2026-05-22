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
#include <string>

#define NR_POINT_LIGHTS 2

struct lightProp {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 exampleColor;

    float outerCutOff;
    float cutoff;
    float constants;
    float linear;
    float quadratic;

    int amount;

    friend std::ostream& operator<<(std::ostream& os, const lightProp& p) {
        os << "ambient: " << p.ambient.x << " " << p.ambient.y << " " << p.ambient.z;
        return os;
    }
    
} directionalLights, pointLights, spotLights;

struct materialsProp {
    int diffuse;
    int specular;

    float shinness;
} floorMaterials, cubesMaterials;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, ImGuiIO& io);
unsigned int loadTexture(char const* path);
void setUniforms();
void setupLights(Shader& shader);
void spotLight(Shader& shader, const lightProp& light);
void pointlight(Shader& shader, const lightProp& light, int& i);
void directionalLight(Shader& shader, const lightProp& light);
void initView(Shader& shader, glm::mat4 view, glm::mat4 projection);
void setupMaterials(Shader shader, materialsProp materials);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
int numberOfPointLights = NR_POINT_LIGHTS;

glm::vec3 camPos = glm::vec3(1.0f, 0.0f, 10.0f);
Camera camera(camPos);

glm::vec3 globalAmbient = glm::vec3(0.005);
glm::vec3 pointLightColor = glm::vec3(1.9f, 0.0f, 0.0f);

// Propierties
float lightSize = 1.0;

glm::vec3 lightPos = glm::vec3(8.0f, 15.3f, 15.0f);
glm::vec3 lightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
glm::vec3 lightColor = pointLightColor;


glm::vec3 pointLightPositions[] = {
        glm::vec3(20.0f,  0.0f,  0.0f),
        glm::vec3(20.5f, 0.0f, -10.0f),
        /*glm::vec3(-4.0f,  0.0f, -12.0f),
        glm::vec3(-20.0f,  0.0f, -3.0f)*/
};

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

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // glDepthMask(GL_FALSE);

    stbi_set_flip_vertically_on_load(true);


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

    Shader cubesShader("shaders/cube.vs", "shaders/cube.fs");
    Shader floorShader("shaders/plane.vs", "shaders/plane.fs");
    Shader lightShader("shaders/light.vs", "shaders/light.fs");
    Shader singleColorShader("shaders/bordercolor.vs", "shaders/bordercolor.fs");

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

    unsigned int floorTexture = loadTexture("textures/tiles_floor_1.png");
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        /*ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();*/

        // set uniforms
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::vec3 cubeColor = globalAmbient;

        // floor/plane things
        floorShader.use();
        initView(floorShader, view, projection);
        setupMaterials(floorShader, floorMaterials);

        // cube propierties
        cubesShader.use();
        initView(cubesShader, view, projection);
        setupMaterials(cubesShader, cubesMaterials);
        
        singleColorShader.use();
        initView(singleColorShader, view, projection);

        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", lightColor);

        // lights things
        glBindVertexArray(cubeVAO);

        for (int i =0 ; i < numberOfPointLights; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(lightSize, lightSize, lightSize));
            lightShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // plane things
        glStencilMask(0x00); // Make sure we dont update the stencil buffer while drawing
        floorShader.use();
        glBindVertexArray(floorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, flashLightTexture);
        setupLights(floorShader);
        floorShader.setInt("material.flashLight", 1);
        glm::mat4 floorModel = glm::mat4(1.0f);
        floorModel = glm::translate(floorModel, glm::vec3(0.0f, -3.0f, 0.0f));
        floorModel = glm::rotate(floorModel, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        float planeSize = 40.0f;
        floorModel = glm::scale(floorModel, glm::vec3(planeSize, planeSize, planeSize));
        floorShader.setMat4("model", floorModel);
        glDrawArrays(GL_TRIANGLES, 0, 10);
        glBindVertexArray(0);

        glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
        glStencilMask(0xFF); // enable writing to the stencil buffer

        // cubes things
        cubesShader.use();
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cubesTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, cubeSpecular);

        setupLights(cubesShader);

 
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::translate(cubeModel, glm::vec3(-1.0f, 0.0f, -1.0f));
        // cubeModel = glm::scale(cubeModel, glm::vec3(cubeSize, cubeSize, cubeSize));
        cubesShader.setMat4("model", cubeModel);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        cubeModel = glm::translate(cubeModel, glm::vec3(2.0f, 0.0f, 0.0f));
        // cubeModel = glm::scale(cubeModel, glm::vec3(cubeSize, cubeSize, cubeSize));
        cubesShader.setMat4("model", cubeModel);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // all fragments should pass the stencil test
        glStencilMask(0x00); // dont draw stuff
        glDisable(GL_DEPTH_TEST); // disable 

        singleColorShader.use();

        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cubesTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, cubeSpecular);
        
        glm::mat4 cubeWithBorder = glm::mat4(1.0f);
        float scaling = 1.1f;
        cubeWithBorder = glm::translate(cubeWithBorder, glm::vec3(-1.0f, 0, -1.0f));
        cubeWithBorder = glm::scale(cubeWithBorder, glm::vec3(scaling, scaling, scaling));
        singleColorShader.setMat4("model", cubeWithBorder);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        cubeWithBorder = glm::translate(cubeWithBorder, glm::vec3(2.0f, 0, 0.0f));
        cubeWithBorder = glm::scale(cubeWithBorder, glm::vec3(scaling, scaling, scaling));
        singleColorShader.setMat4("model", cubeWithBorder);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
    
        /*ImGui::Begin("Esto se supone que es una ventana con imgui");
        ImGui::Text("Hola mundo de la programcion grafica.");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/
        
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

void setupMaterials(Shader shader, materialsProp materials) 
{
    shader.use();
    shader.setInt("material.diffuse", materials.diffuse);
    shader.setInt("material.specular", materials.specular);
    shader.setFloat("material.shininess", materials.shinness);
}

void initView(Shader& shader, glm::mat4 view, glm::mat4 projection)
{
   shader.use();
   shader.setMat4("view", view);
   shader.setMat4("projection", projection);
   shader.setVec3("viewPos", camera.Position);
}


void directionalLight(Shader& shader, const lightProp& light)
{
    shader.use();
    shader.setVec3("dirLight.ambient", light.ambient);
    shader.setVec3("dirLight.diffuse", light.diffuse);
    shader.setVec3("dirLight.specular", light.specular);
    shader.setVec3("dirLight.direction", light.direction);
}

void spotLight(Shader& shader, const lightProp& light)
{
    shader.use();

    shader.setVec3("spotLight.position", light.position);
    shader.setVec3("spotLight.direction", light.direction);
    shader.setVec3("spotLight.ambient", light.ambient);
    shader.setVec3("spotLight.diffuse", light.diffuse);
    shader.setVec3("spotLight.specular", light.specular);
    shader.setFloat("spotLight.constant", light.constants);
    shader.setFloat("spotLight.linear", light.linear);
    shader.setFloat("spotLight.quadratic", light.quadratic);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(light.cutoff)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(light.outerCutOff)));

    shader.setVec3("spotLight.colorExample", light.exampleColor);
}

void pointlight(Shader& shader, const lightProp& light, int& i)
{
    std::string index = std::to_string(i);

    shader.use();
    shader.setVec3("pointLights[" + index + "].position", pointLightPositions[i]);
    shader.setVec3("pointLights[" + index + "].ambient", light.ambient);
    shader.setVec3("pointLights[" + index + "].diffuse", light.diffuse);
    shader.setVec3("pointLights[" + index + "].specular", light.specular);
    shader.setFloat("pointLights[" + index + "].constant", light.constants);
    shader.setFloat("pointLights[" + index + "].linear", light.linear);
    shader.setFloat("pointLights[" + index + "].quadratic", light.quadratic);
    shader.setInt("pointLights.amount", light.amount);
   
}

void setupLights(Shader& shader) 
{
    setUniforms();
    shader.use();

    directionalLight(shader, directionalLights);
    spotLight(shader, spotLights);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        pointlight(shader, pointLights, i);
    }
}

void setUniforms()
{
    directionalLights.ambient = globalAmbient;
    directionalLights.diffuse = glm::vec3(0.4f);
    directionalLights.specular = glm::vec3(0.5f);
    directionalLights.direction = lightDir;
    
    pointLights.ambient = pointLightColor;
    pointLights.diffuse = glm::vec3(1.0f);
    pointLights.specular = glm::vec3(1.0f);
    //pointLights.position = camera.Position;
    // pointLights position se agrega dentro de su funcion 
    pointLights.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    pointLights.outerCutOff = 20.5f; // 17.5f
    pointLights.cutoff = 15.5f; // 12.5f
    pointLights.constants = 1.0f;
    pointLights.linear = 0.09f;
    pointLights.quadratic = 0.032f;
    pointLights.amount = numberOfPointLights;

    spotLights.exampleColor = glm::vec3(0.0f, 0.0, 1.0f); // Esto fue para testear se puede borrar sin problemas (creo)

    spotLights.ambient = glm::vec3(1.0f);
    spotLights.diffuse = glm::vec3(1.0f);
    spotLights.specular = glm::vec3(1.0f);
    spotLights.position = camera.Position;
    spotLights.direction = camera.Front;
    spotLights.outerCutOff = 20.5f; // 17.5f
    spotLights.cutoff = 15.5f; // 12.5f
    spotLights.constants = 1.0f;
    spotLights.linear = 0.07f;
    spotLights.quadratic = 0.032f;

    floorMaterials.diffuse = 0;
    floorMaterials.specular = 0;
    floorMaterials.shinness = 64.0f;

    cubesMaterials.diffuse = 2;
    cubesMaterials.specular = 3;
    cubesMaterials.shinness = 32.0f;
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

