#pragma once
#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <fstream>
#include <iostream>

#define M_PI 3.14159265358979323846

class Photon{

public:
    glm::vec3 position;
    glm::vec3 power;

    int plane; //the axis of plane

    char theta; //入射光线与 z 轴夹角
    char phi;   //入射光线到 xy 的投影与x 轴的夹角

    Photon(glm::vec3 Position, glm::vec3 Direction, glm::vec3 Power);

};



class PhotonMap{

public:
    int storedPhotons;
    int halfStoredPhotons;
    int maxPhotons;

    float cosTheta[256], sinTheta[256], cosPhi[256], sinPhi[256]; //映射0 到255 对应的cos sin 值

    Photon* photons;

    glm::vec3 bboxMin, bboxMax;

    PhotonMap(int maxPhotonNum);

    void store(Photon* p);

    int findMedian(int* photonIndex, int start,int end, int middle,int axis);
    void buildKdTree(int* photonIndex, int start, int end, int axis);

    void insertHeap(glm::vec3 rayPos, int* photonIndex, int currentI, int* heap, int k, int* size);
    void searchTree(glm::vec3 rayPos, int* photonIndex, int* heap, int start, int end, int k, int* size);
    glm::vec3 findNearest(int* photonIndex, glm::vec3 p, int k);

};

class PhotonData{

public:
    unsigned int VAO,VBO;
    float* vertices;

    PhotonData(Photon* photons,int storedPhotons)
    {
        vertices=(float*)malloc(sizeof(float)*storedPhotons*6);
        std::cout<<"storedPhotons: "<<storedPhotons<<std::endl;

        
        for(int i=0;i<storedPhotons;i++)
        {
            vertices[i*6]=photons[i].position.x;
            vertices[i*6+1]=photons[i].position.y;
            vertices[i*6+2]=photons[i].position.z;
            
            vertices[i*6+3]=photons[i].power.x;
            vertices[i*6+4]=photons[i].power.y;
            vertices[i*6+5]=photons[i].power.z;
        }

 
        glGenVertexArrays(1, &VAO);
	    glBindVertexArray(VAO);//绑定VAO
	    
	    glGenBuffers(1, &VBO);//生成ID，并给予缓冲
	    glBindBuffer(GL_ARRAY_BUFFER, VBO);//把VBO绑到GL_ARRAY_BUFFER上代表VBO是一个顶点缓冲
	    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*storedPhotons*6, vertices, GL_STATIC_DRAW);//把vertices放到顶点缓存中

        //链接顶点属性，告诉GPU如何理解传入的顶点数据
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //连接到GL_ARRAY_BUFFER绑定的VBO
	    glEnableVertexAttribArray(0);

	    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float))); //连接到GL_ARRAY_BUFFER绑定的VBO
	    glEnableVertexAttribArray(1);

        
        glBindVertexArray(0);//解绑

    }

};