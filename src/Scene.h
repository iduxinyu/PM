#pragma once 

#include <iostream>

//#include <glm/glm.hpp>
#include <math.h>
#include <cstdlib>
#include <vector>
#include <random>

#include "PhotonMap.h"
#include "camera.h"
#include "Model.h"


#define DIFFUSE 0
#define SPECULAR 1
#define REFRACT 2

#define NOT_INTERSECTED (1.0e6) //对应无限大

#define MAX_BOUNCES 5


#define mP 3000000   //光子图能存储的最大光子数量

enum Axis{xAxis=0,yAxis=1,zAxis=2};

const unsigned int SCR_WIDTH = 400;
const unsigned int SCR_HEIGHT = 400;


class Spheres
{
public:
    glm::vec3 center,color;
    float radius;
    int opticalProp;
    Spheres(){
        radius=0;
    }
    Spheres(glm::vec3 Center,float r,int t)
    {
        center.x=Center.x;
        center.y=Center.y;
        center.z=Center.z;

        radius=r;

        opticalProp=t;
        color=glm::vec3(1.0,1.0,1.0);

    }

    float sIntersect(glm::vec3 origin, glm::vec3 direction);

    glm::vec3 sNormal(glm::vec3 p);

};


class Planes 
{
public:
    enum Axis axis;
    glm::vec3 color;
    float distanceFromO;
    int opticalProp;

    Planes(enum Axis a,float d, glm::vec3 color )
    {
        axis =a;
        distanceFromO=d;
        this->color=color;
        opticalProp=0; //所有的墙壁默认diffuse
    }

    float pIntersect(glm::vec3 origin, glm::vec3 direction);

    glm::vec3 pNormal(glm::vec3 iPoint, glm::vec3 origin);
};


class ObjectIntersection
{
public:
    int type; //sphere ? plane?
    int index;
    float t;

    glm::vec3 normal;

    ObjectIntersection()
    {
        type=-1;
        index=-1;
        normal=glm::vec3(0.0);
        t=NOT_INTERSECTED;
    }
    

};




class PhotonRay
{
public:
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 color;

    PhotonRay()
    {

    }

    PhotonRay(glm::vec3 Origin)
    {
        origin=Origin;
    }

    void randDir(float s); //随机化光线方向
    ObjectIntersection* tracePhotonRay(std::vector<Spheres*> spheres, std::vector<Planes*> planes);
    ObjectIntersection* tracePhotonRay(std::vector<Spheres*> spheres, std::vector<Planes*> planes, std::vector<Mesh> meshes);
    void diffuseRefl(glm::vec3 iPoint); // 从当前点进行漫反射，计算反射后光线方向
    void specularRefl(glm::vec3 iPoint, glm::vec3 normal);
    void refr(glm::vec3 iPoint, glm::vec3 normal);
    void waterRefr(glm::vec3 iPoint, glm::vec3 normal);
};

class CameraRay
{
public:
    glm::vec3 origin;
    glm::vec3 direction;

    CameraRay()
    {

    }
    CameraRay(glm::vec3 Origin)
    {
        origin=Origin;
    }
    ObjectIntersection* traceCameraRay(std::vector<Spheres*> spheres, std::vector<Planes*> planes );
    ObjectIntersection* traceCameraRay(std::vector<Spheres*> spheres, std::vector<Planes*> planes, std::vector<Mesh> meshes);
    void dir(glm::vec3 d);
    void refl(glm::vec3 iPoint, glm::vec3 normal)
    {
        direction = glm::normalize(glm::reflect(direction,normal));
    
        origin=iPoint;
        
    
    }
    void refr(glm::vec3 iPoint, glm::vec3 normal)
    {
        direction=glm::refract(direction,normal,1.0f/1.5f); //eta of glass : 1.5
        origin=iPoint;
    }

    void waterRefr(glm::vec3 iPoint, glm::vec3 normal)
    {
        //如果ray 的方向和法线方向呈钝角就是从空气进入水面
        
        if(glm::dot(direction,normal)<=0.0)
        {
            direction=glm::normalize(glm::refract(direction,normal,1.0f/1.33f));
            
        }
        else
        {
            glm::vec3 oldDir=direction;
            direction=glm::normalize(glm::refract(direction,-normal,1.33f));

            if(std::isnan(direction.x) ||std::isnan(direction.y)||std::isnan(direction.z))
            {
                direction=glm::normalize(glm::reflect(oldDir,-normal));
                //std::cout<<"****************************** Reflect:"<<direction.x<<", "<<direction.y<<", "<<direction.z<<")"<<std::endl;
            }
                
            
        }
        
        origin=iPoint;
    }
};


class Light
{

public:
    int numPhotons;
    glm::vec3 pos,color;
    float power;
    
    Light(glm::vec3 Pos, glm::vec3 Color, float Power, int NumPhotons)
    {
        pos=Pos;
        color=Color;
        power=Power;
        numPhotons=NumPhotons;

    }

};


class Scene
{  

public: 
    Light* light;

    std::vector<Spheres*> spheres;
    std::vector<Planes*> planes;

    PhotonMap* map;
    PhotonMap* globalMap;

    int* photonIndex; //kdTree

    float* rayTraceResult; //
    
    Scene()
    {
        rayTraceResult=new float[SCR_WIDTH*SCR_HEIGHT*5];
    }

    void init();
    void castPhotons();
    int* kdTree();
    void rayTracing(Camera camera);
    

};