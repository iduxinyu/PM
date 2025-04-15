#pragma once

#include <algorithm>
#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class HitPoint{
public:
    glm::vec3 pos;
    glm::ivec2 ij;
    glm::vec3 flux;

    float radius;
    int photonsN;

public:
    HitPoint(){}
    HitPoint(glm::vec3 Pos, glm::vec2 IJ, float Radius)
    {
        pos=Pos;
        ij=IJ;
        radius=Radius;
        flux=glm::vec3(0.0);
        photonsN=0;
    }

};

class KDNode{
public:
    HitPoint* point;
    glm::vec3 min;
    glm::vec3 max;

    float maxr;

    KDNode *l, *r; 
public:
    KDNode(){}

    void deleteNode(KDNode* n)
    {
        if(n==nullptr) return;

        deleteNode(n->l);
        deleteNode(n->r);
        delete n->point;
        delete n;
    }

    void printNode(KDNode* n)
    {
        if(n==nullptr) return;
        //std::cout<<"uv: ("<<n->point->ij.x<<", "<<n->point->ij.y<<").  flux: ("<<n->point->flux.x<<", "<<n->point->flux.y<<", "<<n->point->flux.z<<""<<std::endl;
        std::cout<<"pos: ("<<n->point->pos.x<<","<<n->point->pos.y<<","<<n->point->pos.z<<")  photonSN: "<<n->point->photonsN<<std::endl;
        
        printNode(n->l);
        printNode(n->r);
        
    }
};

class KDTree{
public:
    KDNode* root;


public:
    KDTree(){}

    KDNode* build(std::vector<HitPoint*> hitpoints, int start, int end, int compareAxis);
    void update();


    glm::vec3 findMinEdge(std::vector<HitPoint*> hitpoints, int start, int end);
    glm::vec3 findMaxEdge(std::vector<HitPoint*> hitpoints, int start, int end);
    float findMaxR(std::vector<HitPoint*> hitpoints, int start, int end);

    void deleteTree()
    {
        root->deleteNode(root);
    }

    void printTree()
    {
        root->printNode(root);
    }

};