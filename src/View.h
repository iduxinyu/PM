// Init Windows
// mouse Keyboard
//load Images
//load Shader

#pragma once

#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "SPPMScene.h"
#include "Camera.h"
#include "Shader.h"
#include "Object.h"
#include "Cube.h"




class View 
{

private:
    GLFWwindow* window;
    
    SPPMScene* scene;

    Camera mainCamera;
    

public:
    View();
    int initWindow();
    int display();
    int displaySPPM();


private:
    //回调函数
    //void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    //void processInput(GLFWwindow *window);


};