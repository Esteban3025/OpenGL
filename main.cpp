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
void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

float planeSize = 40.0f;
float flagSize = 3.0f;
float cubeSize = 2.5f;
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 10.0f);
Camera camera(camPos);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_WIDTH / 2.0f;

glm::vec3 cameraPosition = camera.getPosition();
glm::vec3 lastCamPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 lightPos(0.0f, 0.0f, -10.0f);

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

    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);
    int monitorWidth = mode->width;
    int monitorHeight = mode->height;
    std::cout << "Primary monitor width: " << monitorWidth << " pixels" << std::endl;
    std::cout << "Primary monitor height: " << monitorHeight << " pixels" << std::endl;

    Shader ourShader("shader.vs", "shader.fs");
    Shader flagShader("flagShader.vs", "flagShader.fs");
    Shader cubeShader("cubeShader.vs", "cubeShader.fs");
    Shader fontShader("fontShader.vs", "fontShader.fs");

    Textures floorTexture("floor", "textures/tiles_floor_1.png");
    Textures cubeTexture("cube", "textures/wooden_crate_2.png");

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "fonts/ARIAL.TTF", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        return -1;
    }

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
        glm::vec3(5.0f, 0.0f,  -30.0f),
        glm::vec3(0.0f, 0.0f,  -20.0f),
        glm::vec3(9.0f, 0.0f,  -11.0f),
        glm::vec3(-9.0f, 0.0f, -10.0f),
        glm::vec3(-5.0f, 0.0f, -5.3f),
    };

    float planeVertices[] = {
        // positions          // texture coords
         0.9f,  0.9f, 0.0f,   1.0f, 1.0f, // top right
         0.9f, -0.9f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.9f, -0.9f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.9f,  0.9f, 0.0f,   0.0f, 1.0f  // top left 
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

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    unsigned int VBOs[3], VAOs[3], EBO;
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

    // configure VAO/VBO for texture quads of the fonts
    // -----------------------------------
    glGenVertexArrays(1, &VAOt);
    glGenBuffers(1, &VBOt);
    glBindVertexArray(VAOt);
    glBindBuffer(GL_ARRAY_BUFFER, VBOt);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // find path to font
    std::string font_name = "fonts/ARIAL.TTF";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 2);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    ourShader.setInt("floorTexture", 0);
    
    flagShader.use();

    cubeShader.use();   
    cubeShader.setInt("material.diffuse", 1);

    fontShader.use();
    fontShader.setMat4("projection", projection);
    
    
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

     
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        floorTexture.use();

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
        cubeShader.setVec3("light.position", lightPos);

        // light properties
        cubeShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        cubeShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        cubeShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        
        cubeShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);

        // material properties
        cubeShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
        cubeShader.setFloat("material.shininess", 64.0f);
       
        glBindVertexArray(VAOs[1]); // cube things
        
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, -1.0f, 0.0f));
        cubeModel = glm::scale(cubeModel, glm::vec3(cubeSize, cubeSize, cubeSize));

        cubeShader.setMat4("model", cubeModel);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        cubeTexture.use(GL_TEXTURE1);
       
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);

        flagShader.use();

        glBindVertexArray(VAOs[2]); // flag|tringle things 

        glm::mat4 flagModel = glm::mat4(1.0f);
        flagModel = glm::translate(flagModel, lightPos);
        flagModel = glm::scale(flagModel, glm::vec3(flagSize, flagSize, flagSize));
        flagShader.setMat4("model", flagModel);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        /*lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;*/


        /*lightPos.z = -45.0f + sin(glfwGetTime()) * 100.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;*/
      
        flagShader.setMat4("view", view);
        flagShader.setMat4("projection", projection);

        RenderText(fontShader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(fontShader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

        lastFrame = currentFrame;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);

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

void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    s.use();
    s.setVec3("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE2);
    glBindVertexArray(VAOt);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBOt);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
