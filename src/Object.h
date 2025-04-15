#pragma once

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>

#define SCR_WIDTH 300
#define SCR_HEIGHT 300



class VertexData{
public:
    unsigned int VAO;
    unsigned int VBO;
    float* ver;

public:
    VertexData()
    {
        glGenVertexArrays(1, &VAO); 
	    glGenBuffers(1, &VBO);//生成ID，并给予缓冲
	    
    }

    void update(float* vertices, unsigned int size)
    {
         
        /*
        for(int i=0;i<SCR_WIDTH;i++)
        for(int j=0;j<SCR_HEIGHT;j++)
        {
         std::cout<<"uv::: ("<<vertices[i*SCR_WIDTH*5+5*j+0]<<", "<<vertices[i*SCR_WIDTH*5+5*j+1]<<")  color: ("<<vertices[i*SCR_WIDTH*5+5*j+2]<<", "<<vertices[i*SCR_WIDTH*5+5*j+3]<<", "<<vertices[i*SCR_WIDTH*5+5*j+4]<<")"<<std::endl;
         
        }
         */
	    glBindVertexArray(VAO);//绑定VAO
	    
	    glBindBuffer(GL_ARRAY_BUFFER, VBO);//把VBO绑到GL_ARRAY_BUFFER上代表VBO是一个顶点缓冲
	    glBufferData(GL_ARRAY_BUFFER,  sizeof(float)*size, vertices, GL_STREAM_DRAW);//把vertices放到顶点缓存中

        //链接顶点属性，告诉GPU如何理解传入的顶点数据
	    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); //连接到GL_ARRAY_BUFFER绑定的VBO
	    glEnableVertexAttribArray(0);

	    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2*sizeof(float))); //连接到GL_ARRAY_BUFFER绑定的VBO
	    glEnableVertexAttribArray(1);

        
        glBindVertexArray(0);//解绑
    }

};


