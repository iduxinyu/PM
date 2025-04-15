#pragma once

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>

class Quad{

public:
    unsigned int VAO;
    unsigned int VBO;

public:
    Quad()
    {
        
        float vertices[] = {
            // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &VAO);
	    glBindVertexArray(VAO);//绑定VAO
	    
	    glGenBuffers(1, &VBO);//生成ID，并给予缓冲
	    glBindBuffer(GL_ARRAY_BUFFER, VBO);//把VBO绑到GL_ARRAY_BUFFER上代表VBO是一个顶点缓冲
	    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//把vertices放到顶点缓存中

        //链接顶点属性，告诉GPU如何理解传入的顶点数据
	    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //连接到GL_ARRAY_BUFFER绑定的VBO
	    glEnableVertexAttribArray(0);

	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float))); //连接到GL_ARRAY_BUFFER绑定的VBO
	    glEnableVertexAttribArray(1);

        

    }



    ~Quad()
    {
        ///释放资源
	    glDeleteVertexArrays(1, &VAO);
	    glDeleteBuffers(1, &VBO);
    }


};