#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../headers/Shader.h"
#include "../headers/Utils.h"
#include "../headers/Camera.h"
#include "../headers/Armadillo.h"
#include "../headers/CubeMap.h"
#include "../headers/Quad.h"

using namespace std;

// ************** FUNCTION DECLERATIONS ***************
void init();
void display();
void reshape(GLFWwindow* window, int width, int height);
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void initFonts(int windowWidth, int windowHeight);

void activateDeferredRendering();
void activateBlurRendering();
void activateCompositeRendering();
void renderCubeMap();
void renderArmadillo(glm::vec3 tAmount);
float calculateDeltaTime();
void renderAllTexts();
void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

// ************** VARIABLES ****************
GLFWwindow* window;
int gWidth = 1920, gHeight = 1080;

Camera* camera;

Shader* cubeMapShader;
Shader* armadilloShader;
Shader* quadShader;
Shader* lightShader;
Shader* blurShader;
Shader* tonemapShader;
Shader* textShader;

CubeMap* myCubeMap;
GLuint myCubeMapTexture;

Armadillo* myArmadillo;

Quad* compositeQuad;
Quad* blurQuad;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view;
glm::mat4 projection;

float rotationAmount = 180.0f;
float exposure = 1;

bool pressR = false;
bool pressM = true;
int pressV = 0;
bool pressSpace = false;
bool press0 = false;
bool press1 = true;
bool press2 = false;
bool press3 = false;
bool press4 = false;
bool press5 = false;
bool press6 = false;

bool isRotating = false;
bool enableGamma = true;

unsigned int gBuffer, gPosition, gNormal;
unsigned int blurFBO, blurTexture, blurDepthBuffer;
unsigned int compositeFBO, compositeTexture;
unsigned int gDepth;

GLuint compositeDepthBuffer;
GLuint gTextVBO, textVAO;

glm::vec3 lastFront;
glm::vec3 currFront;
float camSpeed = 0;

float currentFrame = 0.0f;
float lastFrame = 0.0f;
float deltaTime = 0;
float sumDelta = 0;
int blurSize = 0;

int fps = 0;
float fpsDisplayTime = 0;

float keyAlpha = 0.18;


int windowedPosX = 100;
int windowedPosY = 100;
int windowedWidth = 1920;
int windowedHeight = 1080;

string topLeft, topRight, bottomLeft, bottomRight;
float bottomLeftTimer = 0;

int main() {

    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW!!" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(gWidth, gHeight, "HW2", NULL, NULL);
    if (!window) {
        cerr << "Failed to create window!!" << endl;
        glfwTerminate();
		exit(-1);
    }

    glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

    if (glewInit() != GLEW_OK){
        cerr << "Failed to initialize GLEW!!" << endl;
        return -1;
    }

    init();

	glfwSetKeyCallback(window, keyboard); 
    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, reshape);

    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);


    while (!glfwWindowShouldClose(window))
	{

		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glm::quat q_up = glm::angleAxis(glm::radians(89.9f), glm::vec3(1.0f, 0.0f, 0.0f));  
    glm::quat q_down = glm::angleAxis(glm::radians(-89.9f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    
    camera = new Camera(q_up,q_down);

    cubeMapShader = new Shader("shaders/vert_cubemap.glsl", "shaders/frag_cubemap.glsl");

    myCubeMap = new CubeMap();
    myCubeMapTexture = loadCubeMapTexture();

    armadilloShader = new Shader("shaders/vert_armadillo.glsl", "shaders/frag_armadillo.glsl");

    myArmadillo = new Armadillo("objects/armadillo.obj");

    compositeQuad = new Quad();
    quadShader = new Shader("shaders/vert_quad.glsl", "shaders/frag_quad.glsl");

    lightShader = new Shader("shaders/vert_light.glsl", "shaders/frag_light.glsl");

    blurQuad = new Quad();
    blurShader = new Shader("shaders/vert_blur.glsl", "shaders/frag_blur.glsl");

    tonemapShader = new Shader("shaders/vert_tonemap.glsl", "shaders/frag_tonemap.glsl");

    textShader = new Shader("shaders/vert_text.glsl", "shaders/frag_text.glsl");

    activateDeferredRendering();
    activateCompositeRendering();
    activateBlurRendering();

    lastFront = camera->getFrontVector();
    currFront = camera->getFrontVector();

    initFonts(gWidth, gHeight);

    topLeft = "CUBEMAP";
}

void calculateFPS() {
    fps++;
    fpsDisplayTime += deltaTime;

    if (fpsDisplayTime >= 1.0f) {
        topRight = to_string(fps);

        fpsDisplayTime = 0;
        fps = 0;
    } 
}

void display() {
    glfwSwapInterval(pressV);
    deltaTime = calculateDeltaTime();
    currFront = camera->getFrontVector();
    calculateFPS();

    if (press1) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderAllTexts();

        glm::vec3 camPos = {0.0f, 0.0f, 0.0f};
        camera->setCamPos(camPos);

        // glDepthMask(GL_FALSE);
        // renderCubeMap();
        // glDepthMask(GL_TRUE);

        glDepthFunc(GL_LEQUAL);    // allow fragments with depth == 1.0
        glDepthMask(GL_FALSE);     // don't write to depth buffer
        glDisable(GL_CULL_FACE);   // draw inside of the cube
    
        renderCubeMap();           // your sky‐cube draw call
    
        // 3) restore state
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    else if (press2){
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        

        glm::vec3 camPos = {0.0f, 0.0f, 5.0f};
        camera->setCamPos(camPos);

        glm::vec3 tAmount = glm::vec3(0.0f);
        renderArmadillo(tAmount);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        quadShader->use();
        quadShader->setInt("qTexture", 0);
    
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
    
        compositeQuad->draw();

    }

    else if (press3) {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glm::vec3 camPos = {0.0f, 0.0f, 5.0f};
        camera->setCamPos(camPos);

        glm::vec3 tAmount = glm::vec3(0.0f);
        renderArmadillo(tAmount);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        quadShader->use();
        quadShader->setInt("qTexture", 0);
    
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gNormal);
    
        compositeQuad->draw();
    }

    else if (press4) {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glm::vec3 camPos = {0.0f, 0.0f, 5.0f};
        camera->setCamPos(camPos);

        glm::vec3 tAmount = glm::vec3(0.0f);
        renderArmadillo(tAmount);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        lightShader->use();
        lightShader->setInt("gPosition", 0);
        lightShader->setInt("gNormal", 1);
        lightShader->setVec3("viewPos", camera->getCamPosition());
        lightShader->setFloat("exposure", exposure);
    
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);

        compositeQuad->draw();
    }

    else if (press5) {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glm::vec3 camPos = {0.0f, 0.0f, 0.0f};
        camera->setCamPos(camPos);

        glm::vec3 tAmount = glm::vec3(0.0f,0.0f,-5.0f);
        renderArmadillo(tAmount);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthFunc(GL_LEQUAL);    // allow fragments with depth == 1.0
        glDepthMask(GL_FALSE);     // don't write to depth buffer
        glDisable(GL_CULL_FACE);   // draw inside of the cube
    
        renderCubeMap();          
    
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    
        lightShader->use();
        lightShader->setInt("gPosition", 0);
        lightShader->setInt("gNormal", 1);
        lightShader->setInt("myCubeSampler", 2);
        lightShader->setVec3("viewPos", camera->getCamPosition());
        lightShader->setFloat("exposure", exposure);
    
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        compositeQuad->draw();

        glDisable(GL_BLEND);

    }

    else if (press6) {
        float angleChange = acos(glm::clamp(glm::dot(currFront, lastFront), -1.0f, 1.0f));
        camSpeed = angleChange / deltaTime;
        lastFront = currFront;

        if (camSpeed <= 0.5) {
            if (blurSize > 0){
                sumDelta += deltaTime;
                if (sumDelta >= 1.0f/blurSize){
                    blurSize = std::max(blurSize - 1, 0);
                    sumDelta = 0;
                }
            }
        }
        else {
            blurSize = glm::clamp(int(camSpeed), 0, 10);
            sumDelta = 0;
        }



        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glm::vec3 camPos = {0.0f, 0.0f, 0.0f};
        camera->setCamPos(camPos);

        glm::vec3 tAmount = glm::vec3(0.0f,0.0f,-5.0f);
        renderArmadillo(tAmount);

        glBindFramebuffer(GL_FRAMEBUFFER, compositeFBO);
        glViewport(0, 0, gWidth, gHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthFunc(GL_LEQUAL);   
        glDepthMask(GL_FALSE);     
        glDisable(GL_CULL_FACE);   

        renderCubeMap();          

        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);


        lightShader->use();
        lightShader->setInt("gPosition", 0);
        lightShader->setInt("gNormal", 1);
        lightShader->setInt("myCubeSampler", 2);
        lightShader->setVec3("eyePos", camera->getCamPosition());
        lightShader->setFloat("exposure", exposure);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        
        compositeQuad->draw();

        glDisable(GL_BLEND);




        if (press0) {
            glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
            glViewport(0, 0, gWidth, gHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            blurShader->use();
            blurShader->setInt("myColorSample", 0);
            blurShader->setBool("enableBlur", pressM);
            blurShader->setBool("isRotating", isRotating);
            blurShader->setInt("blurSize", blurSize);
    
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, compositeTexture);

            glDisable(GL_DEPTH_TEST);
            blurQuad->draw();
            glEnable(GL_DEPTH_TEST);


            glBindTexture(GL_TEXTURE_2D, blurTexture);
            glTextureBarrier(); // Add this to prevent undefined behaviour
            glGenerateMipmap(GL_TEXTURE_2D);

            

            int level = floor(log2(float(max(gWidth,gHeight))));

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            tonemapShader->use();
            tonemapShader->setInt("myColorSampler", 0);
            tonemapShader->setFloat("key", keyAlpha);
            tonemapShader->setBool("enableGamma", enableGamma);
            tonemapShader->setInt("level", level);


            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, blurTexture);

            glDisable(GL_DEPTH_TEST);
            blurQuad->draw();
            glEnable(GL_DEPTH_TEST);
        }
        else {

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //glBindFramebuffer(GL_FRAMEBUFFER, compositeFBO);
            glViewport(0, 0, gWidth, gHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            blurShader->use();
            blurShader->setInt("myColorSample", 0);
            blurShader->setBool("enableBlur", pressM);
            blurShader->setBool("isRotating", isRotating);
            blurShader->setInt("blurSize", blurSize);
    
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, compositeTexture);

            glDisable(GL_DEPTH_TEST);
            blurQuad->draw();
            glEnable(GL_DEPTH_TEST);
        }

        // TextureLOD(texCoord, level) texcoord could be vec2(0.5,0.5)
    }

    renderAllTexts();
}

void renderAllTexts() {
    ostringstream oss;
    string s;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,gWidth,gHeight);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec3 color = glm::vec3(1.0f, 0.2f, 0.2f);

    // Top Right
    renderText("FPS: ", gWidth - 200, gHeight - 60 , 1, color);
    renderText(topRight, gWidth - 90, gHeight - 60, 1, color);

    // Bottom Right
    renderText("vsync: ", gWidth - 250, 250 , 0.6, color);
    if (pressV) s = "true";
    else s = "false";
    renderText(s, gWidth - 150, 250, 0.6, color);

    renderText("exposure: ", gWidth - 300, 200 , 0.6, color);
    oss << fixed << setprecision(2) << exposure;
    s = oss.str();   
    renderText(s, gWidth - 150, 200, 0.6f, color);
    oss.clear();     
    oss.str("");       
    
    renderText("motion blur: ", gWidth - 325, 150 , 0.6, color);
    if (pressM) s = "true";
    else s = "false";
    renderText(s, gWidth - 150, 150, 0.6, color);

    renderText("key: ", gWidth - 225, 100 , 0.6, color);
    oss << fixed << setprecision(2) << keyAlpha;
    s = oss.str();   
    renderText(s, gWidth - 150, 100, 0.6f, color);
    oss.clear();     
    oss.str("");   
    
    renderText("gamma: ", gWidth - 275, 50 , 0.6, color);
    float val = 2.2;
    if (!enableGamma) val = 1;
    oss << fixed << setprecision(1) << val;
    s = oss.str();   
    renderText(s, gWidth - 150, 50, 0.6f, color);
    oss.clear();     
    oss.str("");   

    // Top Left
    renderText(topLeft, 20, gHeight - 60, 0.9, color);

    // Bottom Left
    if (bottomLeftTimer < 1) {
        bottomLeftTimer += deltaTime;
        renderText(bottomLeft, 20, 50, 1, color);
    }
  
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void activateBlurRendering() {
    glGenFramebuffers(1, &blurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);

    glGenTextures(1, &blurTexture);
    glBindTexture(GL_TEXTURE_2D, blurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gWidth, gHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);
    glGenRenderbuffers(1, &blurDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, blurDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gWidth, gHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, blurDepthBuffer);

    GLenum drawBufs[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBufs);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void activateCompositeRendering() {
    glGenFramebuffers(1, &compositeFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, compositeFBO);

    glGenTextures(1, &compositeTexture);
    glBindTexture(GL_TEXTURE_2D, compositeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gWidth, gHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenRenderbuffers(1, &compositeDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, compositeDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gWidth, gHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, compositeDepthBuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,compositeTexture,0);
    GLenum drawBuf = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &drawBuf);   

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, compositeTexture, 0);
}

void activateDeferredRendering() {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gWidth, gHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gWidth, gHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    unsigned int gDepth;
    glGenRenderbuffers(1, &gDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gWidth, gHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);
    
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
}

void renderArmadillo(glm::vec3 tAmount) {

    if (!pressR) {
        rotationAmount += 2.0f;
    }

    if (rotationAmount - 360 >= 0) rotationAmount = 0;


    model = glm::mat4(1.0f);
    view  = camera->getViewMatrix();
    projection = glm::perspective(glm::radians(60.0f), (float)gWidth/gHeight, 0.1f, 100.0f);

    model = glm::translate(model, tAmount);
    model = glm::rotate(model, glm::radians(rotationAmount), glm::vec3(0, 1, 0));

    armadilloShader->use();

    armadilloShader->setMat4("modelingMatrix", model);
    armadilloShader->setMat4("viewingMatrix", view);
    armadilloShader->setMat4("projectionMatrix", projection);

    myArmadillo->draw();
}

void renderCubeMap() {
    view  = camera->getViewMatrix();
    projection = glm::perspective(glm::radians(60.0f), (float)gWidth/gHeight, 0.1f, 100.0f);

    cubeMapShader->use();

    cubeMapShader->setMat4("viewingMatrix", glm::mat4(glm::mat3(view)));
    cubeMapShader->setMat4("projectionMatrix", projection);

    cubeMapShader->setInt("myCubeSampler", 0);
    cubeMapShader->setFloat("exposure", exposure);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, myCubeMapTexture);
    
    myCubeMap->draw();
}

float calculateDeltaTime(){
    currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    return deltaTime;
}

void switchFullScreen(GLFWwindow* window) {
    if (!pressSpace) {
        glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        GLFWmonitor*   mon  = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(mon);
        glfwSetWindowMonitor(window,
                             mon,
                             0, 0,
                             mode->width, mode->height,
                             mode->refreshRate);
    }
    else {
        glfwSetWindowMonitor(window,
                             nullptr,
                             windowedPosX, windowedPosY,
                             windowedWidth, windowedHeight,
                             0);
    }
}

void initFonts(int windowWidth, int windowHeight)
{
    // Set OpenGL options
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "windowWidth = " << windowWidth << std::endl;
    std::cout << "windowHeight = " << windowHeight << std::endl;

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
    textShader->use();
    glUniformMatrix4fv(glGetUniformLocation(textShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 0, &face))
    //if (FT_New_Face(ft, "/usr/share/fonts/truetype/gentium-basic/GenBkBasR.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
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
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (GLuint) face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    //
    // Configure VBO for texture quads
    //
    GLuint vaoLocal, vbo;
    glGenVertexArrays(1, &vaoLocal);
    assert(vaoLocal > 0);
    textVAO = vaoLocal;
    glBindVertexArray(vaoLocal);

    glGenBuffers(1, &gTextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state	
    glUseProgram(textShader->ID);
    glUniform3f(glGetUniformLocation(textShader->ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindVertexArray(textVAO);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)

        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    bottomLeftTimer = 0;
    if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        bottomLeft = "R";
        pressR = !pressR;
    }    
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        bottomLeft = "M";
        pressM = !pressM;
    }       
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        bottomLeft = "V";
        if (pressV) pressV = 0;
        else pressV = 1;
    }   
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        bottomLeft = "G";
        enableGamma = !enableGamma;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        bottomLeft = "SPACE";
        switchFullScreen(window);
        pressSpace = !pressSpace;
    }
    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
    {
        bottomLeft = "PLUS";
        if (exposure <= 32) exposure *= 2;
    }    
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
    {
        bottomLeft = "MINUS";
        if (exposure >= 0.1) exposure /= 2;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        bottomLeft = "PAGE_UP";
        if (keyAlpha <= 32) keyAlpha *= 2;
    }    
    if (key == GLFW_KEY_S&& action == GLFW_PRESS)
    {
        bottomLeft = "PAGE_DOWN";
        if (keyAlpha >= 0.04) keyAlpha /= 2;
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        bottomLeft = "1";
        topLeft = "CUBEMAP";
        press0 = false;
        press1 = true;
        press2 = false;
        press3 = false;
        press4 = false;
        press5 = false;
        press6 = false;

    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        bottomLeft = "2";
        topLeft = "WORLD_POS_MODEL";
        press0 = false;
        press1 = false;
        press2 = true;
        press3  =false;
        press4 = false;
        press5 = false;
        press6 = false;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        bottomLeft = "3";
        topLeft = "WORLD_NOR_MODEL";
        press0 = false;
        press1 = false;
        press2 = false;
        press3 = true;
        press4 = false;
        press5 = false;
        press6 = false;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        bottomLeft = "4";
        topLeft = "DEFFERED_LIGHTING";
        press0 = false;
        press1 = false;
        press2 = false;
        press3 = false;
        press4 = true;
        press5 = false;
        press6 = false;
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
        bottomLeft = "5";
        topLeft = "COMPOSITE";
        press0 = false;
        press1 = false;
        press2 = false;
        press3 = false;
        press4 = false;
        press5 = true;
        press6 = false;
    }    
    if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
        bottomLeft = "6";
        topLeft = "COMPOSITE/MOTION_BLUR";
        press0 = false;
        press1 = false;
        press2 = false;
        press3 = false;
        press4 = false;
        press5 = false;
        press6 = true;
    }
    if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        bottomLeft = "0";
        press0 = !press0;
        if (press0) topLeft = "TONEMAPPED";
        else  topLeft = "COMPOSITE/MOTION_BLUR";
        press1 = false;
        press2 = false;
        press3 = false;
        press4 = false;
        press5 = false;
        press6 = true;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS)
        {
            bottomLeftTimer = 0;
            bottomLeft = "MIDDLE_MOUSE_BUTTON";
            double xpos, ypos;
            isRotating = true;
            glfwGetCursorPos(window, &xpos, &ypos);
            camera->startMovement((float)xpos, (float)ypos);
        }
        else if (action == GLFW_RELEASE)
        {
            isRotating = false;;
            camera->endMovement();
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    camera->moveCam((float)xpos, (float)ypos);
}

void reshape(GLFWwindow* window, int width, int height) {
    gWidth  = width;
    gHeight = height;
    glViewport(0, 0, width, height);

    // cout << gWidth << "  Reshape  " << gHeight << endl;
  
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                 width, height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
  
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                 width, height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
  
    glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH_COMPONENT,
                          width, height);

    glBindTexture(GL_TEXTURE_2D, compositeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                    width, height, 0,
                    GL_RGBA, GL_FLOAT, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, compositeDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,
                            GL_DEPTH_COMPONENT,
                            width, height);

    glBindTexture(GL_TEXTURE_2D, blurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                    width, height, 0,
                    GL_RGBA, GL_FLOAT, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, blurDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,
                            GL_DEPTH_COMPONENT,
                            width, height);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    textShader->use();
    glm::mat4 proj = glm::ortho(0.0f,
                                static_cast<float>(width),
                                0.0f,
                                static_cast<float>(height));
    glUniformMatrix4fv(glGetUniformLocation(textShader->ID, "projection"),
                       1, GL_FALSE, glm::value_ptr(proj));

}
  
