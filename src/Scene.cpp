#include "Scene.h"



float Spheres::sIntersect(glm::vec3 origin, glm::vec3 direction){
    float a=glm::dot(direction,direction);
    float b=2*glm::dot((origin-center),direction);
    float c=glm::dot((origin-center),(origin-center))-radius*radius;

    float d=b*b-4*a*c;

    float t1=(-b+glm::sqrt(d))/(2*a);
    float t2=(-b-glm::sqrt(d))/(2*a);

    if(t1<0.0)
        return NOT_INTERSECTED;
    else if(t2>0.0)
        return t2;
    else
        return NOT_INTERSECTED;


}


glm::vec3 Spheres::sNormal(glm::vec3 iPoint) {
  	glm::vec3 normal(iPoint.x - center.x, iPoint.y - center.y, iPoint.z - center.z);
  	normal=glm::normalize(normal);
  	return normal;
}

float Planes::pIntersect(glm::vec3 origin, glm::vec3 direction)
{
    if(axis == 0) {
  	 	if(direction.x != 0.0)                         
    		return  (distanceFromO - origin.x) / direction.x; 
  	}
  	else if(axis == 1) {
  		if(direction.y != 0.0)
  			return  (distanceFromO - origin.y) / direction.y;
  	}
  	else if(axis == 2) {
  		if(direction.z != 0.0)
  			return  (distanceFromO - origin.z) / direction.z;
  	}
  	return NOT_INTERSECTED;
}


glm::vec3 Planes::pNormal(glm::vec3 iPoint, glm::vec3 origin) {
	int a = axis;
  	glm::vec3 normal(0, 0, 0);
  	if(a == 0)
  		normal.x = origin.x - distanceFromO;
  	if(a == 1)
  		normal.y = origin.y - distanceFromO; 
  	if(a == 2)
  		normal.z = origin.z - distanceFromO; 
  	normal=normalize(normal);
  	return normal;
}



void Scene::init(){

    //init 3 spheres and 5 planes
    Spheres* sOne=new Spheres(glm::vec3(-1.0,-1.0,-3.0),0.5,SPECULAR);
    Spheres* sTwo=new Spheres(glm::vec3(.6, -1.2, -2.5),0.3,REFRACT);
    Spheres* sThree=new Spheres(glm::vec3(-.2, -1.3, -2.0),0.2,REFRACT);

    Planes* left=new Planes(xAxis, 1.5, glm::vec3(0.75,0.4,0.6));
    Planes* right=new Planes(xAxis, -1.5, glm::vec3(0.5,0.75,0.5));
    Planes* top=new Planes(yAxis, 1.5, glm::vec3(0.5,0.5,0.75));
    Planes* bottom=new Planes(yAxis, -1.5, glm::vec3(0.75,0.75,0.75));
    Planes* back=new Planes(zAxis, -5.0, glm::vec3(0.75,0.75,0.75));

    


    //put objects into vectors

    spheres.push_back(sOne);
    spheres.push_back(sTwo);
    spheres.push_back(sThree);

    planes.push_back(left);
    planes.push_back(right);
    planes.push_back(top);
    planes.push_back(bottom);
    planes.push_back(back);



    //init Light

    Light* l=new Light(glm::vec3(0.0,1.2,-3.), glm::vec3(1.0,1.0,1.0), 100, 50000);
    light = l;


    //init map and global Map
    map=new PhotonMap(mP);
    globalMap=new PhotonMap(mP);


    //cast photons

}


void Scene::castPhotons()
{
    srand(0);

    float photonPower =1.0f;

     std::cout<<"castPhotons"<<std::endl;

    //遍历光源发出的所有光子
    for(int i=0;i<light->numPhotons;i++)
    {
        std::cout<<i<<std::endl;

        int bounces =1 ,spec =0 ;
        bool diffuse=false;

        glm::vec3 photonColor=glm::vec3(1.0f,1.0f,1.0f);


        //初始化Ray的位置和方向
        PhotonRay ray(light->pos);
        ray.randDir(1.0);

        // Photon is outside the room
		 
		if(fabs(ray.origin.x) > 1.5 || fabs(ray.origin.y) > 1.5) {
			continue;
		}


        //找到第一次的bounce处
        //判断Ray 和场景中所有物体是否相交，找到最近的交点，在这里bounce
        ObjectIntersection* obj=ray.tracePhotonRay(spheres,planes);

       while(obj->index!=-1 && bounces<MAX_BOUNCES)
       {
            glm::vec3 iPoint=ray.origin+ (obj->t) * ray.direction;

                if(iPoint.z<-5.0 || iPoint.z>0.0 || abs(iPoint.x)>1.5 || abs(iPoint.y)>1.5) 
                {  
                    std::cout<<"photon "<<i<<" out of the scene"<<std::endl;
                    break;
                }

                
                if(obj->type ==1 ) //if intersect with plane
                {
                    //存储光线方向和撞击点

                    //计算光子能量
                    std::cout<<"photon "<<i<<" intersect with plane"<<obj->index<<std::endl;

                    glm::vec3 normal=planes[obj->index]->pNormal(iPoint,ray.origin);

                    float NdotL=std::max(glm::dot(normal,normalize(ray.origin-iPoint)),0.0f); //Lambert

                    photonColor=photonColor*planes[obj->index]->color;


                    // Scaling power by number of bounces, uncomment costTheta for Lambertian reflection

                    photonColor=photonColor/sqrt((float)bounces); //NdotL ???

                    Photon *p=new Photon(iPoint,ray.direction,photonColor*photonPower);

                    if(spec>0 && diffuse== false){
                        std::cout<<"photon "<<i<<" stored by map"<<std::endl;
                        map->store(p);  //caustics
                        diffuse=true;
                    }
                    else{
                        std::cout<<"photon "<<i<<" stored by global map"<<std::endl;
                        globalMap->store(p); //global
                    }

                    ray.diffuseRefl(iPoint);  //墙面默认漫反射

                }


                if(obj->type==0)  //if intersect with ball
                {
                    std::cout<<"photon "<<i<<" intersect with sphere"<<obj->index<<std::endl;
                    spec++;
                    photonColor=photonColor*spheres[obj->index]->color;
                    photonColor=photonColor/sqrt((float)bounces);

                    Photon *p=new Photon(iPoint,ray.direction,photonColor*photonPower);

                    std::cout<<"photon "<<i<<" stored by global map"<<std::endl;
                    globalMap->store(p);

                    glm::vec3 normal=spheres[obj->index]->sNormal(iPoint);

                    if(spheres[obj->index]->opticalProp==SPECULAR)
                    {
                        ray.specularRefl(iPoint,normal);
                    }
                    else if(spheres[obj->index]->opticalProp==REFRACT)
                    {
                        ray.refr(iPoint,normal);
                    }



                    
                }


                obj = ray.tracePhotonRay(spheres, planes);
	      	    bounces++;
            
       }





       
    }

}

int* Scene::kdTree()
{
    int length=globalMap->storedPhotons;
    std::cout<<"length: "<<length<<std::endl;

    photonIndex=new int[length];

    for(int i=0;i<length;i++)
    {
        photonIndex[i]=i;
    }

    globalMap->buildKdTree(photonIndex,0,length-1,0);

    /*
    std::ofstream f("data.txt", std::ios::app); //创建data.txt文件，写入时采用追加方式
    f<<"--------------------"<<std::endl;
    for(int i=0;i<globalMap->storedPhotons;i++)
    {
	    f << photonIndex[i] << ",  ";
    }
    f<<std::endl;
    
    f.close();	
    */
}

void Scene::rayTracing(Camera camera)
{
     //按像素发出光线查找

     //std::ofstream f("flux2.txt", std::ios::app); //创建data.txt文件，写入时采用追加方式
     
     int index=0;

     for(int i=0; i<SCR_WIDTH;i++)
     {
        for(int j=0; j<SCR_HEIGHT;j++)
        {
             //init pos
             float u=2.0f*(((float)i+0.5f)/((float)SCR_WIDTH))-1.0f;
             float v=2.0f*(((float)j+0.5f)/((float)SCR_HEIGHT))-1.0f;
             rayTraceResult[i*SCR_WIDTH*5+5*j]=u;
             rayTraceResult[i*SCR_WIDTH*5+5*j+1]=v;
             rayTraceResult[i*SCR_WIDTH*5+5*j+2]=0.0;
             rayTraceResult[i*SCR_WIDTH*5+5*j+3]=0.0;
             rayTraceResult[i*SCR_WIDTH*5+5*j+4]=0.0;
             //set parameter
             int bounces =1;

             //init cameraRay
             CameraRay ray(camera.Position);

             glm::vec4 screenPos(u,v, 0.0f, 1.0f);

             glm::vec4 worldPos=camera.GetInvViewMatrix()*camera.GetInvProjMatrix(SCR_WIDTH,SCR_HEIGHT)*screenPos;
             worldPos/=worldPos.w;

             ray.dir(glm::normalize(glm::vec3(worldPos.x,worldPos.y,worldPos.z)-camera.Position));

             //找到最近的相交的物体
             ObjectIntersection* obj=ray.traceCameraRay(spheres,planes);

             while(obj->index!=-1 && bounces<MAX_BOUNCES)
             { 
                glm::vec3 iPoint=ray.origin+ (obj->t) * ray.direction;

                if(iPoint.z<-5.0 || iPoint.z>0.0 || abs(iPoint.x)>1.5 || abs(iPoint.y)>1.5) 
                {  
                    std::cout<<"ray: ("<<i<<", "<<j<<") out of the scene"<<std::endl;
                    break;
                }

                
                if(obj->type ==1 ) //if intersect with plane 默认是diffuse 
                {
                    
                    std::cout<<"ray: ("<<i<<", "<<j<<") intersect with plane"<<obj->index<<std::endl;

                    //因为默认是diffuse 直接查找kdtree，计算碰撞点的位置，能量，存储起来
                    glm::vec3 flux= globalMap->findNearest(photonIndex, iPoint, 100);

                    std::cout<<"flux: "<<flux.x <<", "<<flux.y<<", "<<flux.z<<std::endl;

                    //把pixel位置存储到

                    
                    rayTraceResult[i*SCR_WIDTH*5+5*j+2]=flux.x;
                    rayTraceResult[i*SCR_WIDTH*5+5*j+3]=flux.y;
                    rayTraceResult[i*SCR_WIDTH*5+5*j+4]=flux.z;

                    break; 


                }


                if(obj->type==0)  //if intersect with ball
                {
                    std::cout<<"ray: ("<<i<<", "<<j<<") intersect with sphere"<<obj->index<<std::endl;
                    

                    glm::vec3 normal=spheres[obj->index]->sNormal(iPoint);

                    if(spheres[obj->index]->opticalProp==SPECULAR)
                    {
                        ray.refl(iPoint,normal);
                    }
                    else if(spheres[obj->index]->opticalProp==REFRACT)
                    {
                        ray.refr(iPoint,normal);
                    }



                    
                }


                obj = ray.traceCameraRay(spheres, planes);
	      	    bounces++;

             }

             if(obj->index!=-1 && bounces==MAX_BOUNCES) //到达bounces数的话,即使的specular 和 transparent都要存储能量
             {
                
                glm::vec3 iPoint=ray.origin+ (obj->t) * ray.direction;
                if(iPoint.z>-5.0 && iPoint.z<0.0 && abs(iPoint.x)<1.5 && abs(iPoint.y)<1.5) 
                {
                    glm::vec3 flux= globalMap->findNearest(photonIndex, iPoint, 100);
                    rayTraceResult[i*SCR_WIDTH*5+5*j+2]=flux.x;
                    rayTraceResult[i*SCR_WIDTH*5+5*j+3]=flux.y;
                    rayTraceResult[i*SCR_WIDTH*5+5*j+4]=flux.z;
                }

             }


             
             
             
             //f<<i*SCR_WIDTH*5+5*j<<": "<<rayTraceResult[i*SCR_WIDTH*5+5*j]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+1]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+2]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+3]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+4]<<std::endl;
             //f<<index<<": "<<rayTraceResult[index]<<", "<<rayTraceResult[index+1]<<", "<<rayTraceResult[index+2]<<", "<<rayTraceResult[index+3]<<", "<<rayTraceResult[index+4]<<std::endl;
             
            
             index+=5;
            

        }
     }

     //f<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
     //f.close();	

  

}



void PhotonRay::randDir(float s)
{
    float temp[3];
	for(int i = 0; i < 3; i++) 
		temp[i] = (float)rand() * 2 * s / RAND_MAX - s;
	direction.x = temp[0];
	direction.y = temp[1];
	direction.z = temp[2];
	direction=normalize(direction);
}

ObjectIntersection* PhotonRay::tracePhotonRay(std::vector<Spheres*> spheres, std::vector<Planes*> planes )
{
   
    ObjectIntersection* obj=new ObjectIntersection();

    for(int i=0;i<spheres.size();i++)
    {
        float t=spheres[i]->sIntersect(origin,direction);
        if(t < obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=0;
            obj->index=i;
        }
    }

    for(int i=0;i<planes.size();i++)
    {
        float t=planes[i]->pIntersect(origin,direction);
        if(t < obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=1;
            obj->index=i;
        }
    }

    return obj;
}
ObjectIntersection* PhotonRay::tracePhotonRay(std::vector<Spheres*> spheres, std::vector<Planes*> planes, std::vector<Mesh> meshes)
{   
    ObjectIntersection* obj=new ObjectIntersection();

    for(int i=0;i<spheres.size();i++)
    {
        float t=spheres[i]->sIntersect(origin,direction);
        if(t < obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=0;
            obj->index=i;
        }
    }

    for(int i=0;i<planes.size();i++)
    {
        float t=planes[i]->pIntersect(origin,direction);
        if(t < obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=1;
            obj->index=i;
        }
    }

    for(int i=0;i<meshes.size();i++)
    {
        glm::vec3 norm;
        float t=meshes[i].intersect(origin,direction, &norm);
        if(t<obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=2;
            obj->normal=norm;
            obj->index=i;
        }
    }

    return obj;

}

void PhotonRay::diffuseRefl(glm::vec3 iPoint)
{
    PhotonRay newRef(iPoint); 

    newRef.randDir(1.0);
    
    origin=iPoint;

    direction=newRef.direction;
}


void PhotonRay::specularRefl(glm::vec3 iPoint, glm::vec3 normal)
{
    direction = glm::normalize(glm::reflect(direction,normal));

    origin=iPoint;
    

}
void PhotonRay::refr(glm::vec3 iPoint, glm::vec3 normal)
{
    

    direction=glm::refract(direction,normal,1.0f/1.5f); //eta of glass : 1.5
    origin=iPoint;

  	
}
void PhotonRay::waterRefr(glm::vec3 iPoint, glm::vec3 normal)
    {
        std::cout<<"photon Water Refr: ";
        //如果ray 的方向和法线方向呈钝角就是从空气进入水面
        if(glm::dot(direction,normal)<=0.0)
        {
            direction=glm::normalize(glm::refract(direction,normal,1.0f/1.33f));
            std::cout<<"++++++++++++++++++++++++++++++++++up the water: ("<<direction.x<<", "<<direction.y<<", "<<direction.z<<std::endl;
        }
        else
        {
            glm::vec3 oldDir=direction;
            direction=glm::normalize(glm::refract(direction,-normal,1.33f));
            //全内反射
            if(std::isnan(direction.x) ||std::isnan(direction.y)||std::isnan(direction.z))
                direction=glm::normalize(glm::reflect(oldDir,-normal));
            std::cout<<"@@@@@@@@@@@@@@@@@@lower the water: ("<<direction.x<<", "<<direction.y<<", "<<direction.z<<")"<<std::endl;
        }
        
        origin=iPoint;
    }


ObjectIntersection* CameraRay::traceCameraRay(std::vector<Spheres*> spheres, std::vector<Planes*> planes )
{
    ObjectIntersection* obj=new ObjectIntersection();

    for(int i=0;i<spheres.size();i++)
    {
        float t=spheres[i]->sIntersect(origin,direction);
        if(t < obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=0;
            obj->index=i;
        }
    }

    for(int i=0;i<planes.size();i++)
    {
        float t=planes[i]->pIntersect(origin,direction);
        if(t < obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=1;
            obj->index=i;
        }
    }

    return obj;
}

ObjectIntersection* CameraRay::traceCameraRay(std::vector<Spheres*> spheres, std::vector<Planes*> planes, std::vector<Mesh> meshes)
{   
    ObjectIntersection* obj=new ObjectIntersection();

    for(int i=0;i<spheres.size();i++)
    {
        float t=spheres[i]->sIntersect(origin,direction);
        if(t < obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=0;
            obj->index=i;
        }
    }

    for(int i=0;i<planes.size();i++)
    {
        float t=planes[i]->pIntersect(origin,direction);
        if(t < obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=1;
            obj->index=i;
        }
    }

    for(int i=0;i<meshes.size();i++)
    {
        glm::vec3 norm;
        float t=meshes[i].intersect(origin,direction,&norm);
        if(t<obj->t && t>1.0e-5){
            obj->t=t;
            obj->type=2;
            obj->normal=norm;
            obj->index=i;
        }
    }

    return obj;

}

void CameraRay::dir(glm::vec3 d)
{
    direction=d;
        
}