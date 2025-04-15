#include "SPPMScene.h"

SPPMScene::SPPMScene(float initRadius)
{
    accRadiance=new glm::vec3[SCR_WIDTH*SCR_HEIGHT];
    nearestPhotonNum=new int[SCR_WIDTH*SCR_HEIGHT];
    pixelRadius=new float[SCR_WIDTH*SCR_HEIGHT];
    for(int i=0;i<SCR_WIDTH*SCR_HEIGHT;i++)
    {
        accRadiance[i]=glm::vec3(0.0);
        nearestPhotonNum[i]=0;
        pixelRadius[i]=initRadius;
    }

    

}

KDTree* SPPMScene::rayTracing(Camera camera, int r)
{
    std::cout<<"begin ray Tracing"<<std::endl;
    //按像素发出光线查找

    //初始化一个vector 用于存储本轮所有的命中点
    std::vector<HitPoint*> hitpoints;

    //std::ofstream f("waterRefr.txt", std::ios::app); //创建data.txt文件，写入时采用追加方式
    //f<<"--------------------"<<std::endl;
   
   
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

            //init cameraRay, jitter accordng to round
            CameraRay ray(camera.Position);

            glm::vec4 screenPos(u,v, 0.0f, 1.0f);

            glm::vec4 worldPos=camera.GetInvViewMatrix()*camera.GetInvProjMatrix(SCR_WIDTH,SCR_HEIGHT)*screenPos;
            worldPos/=worldPos.w;

           
            //jitter
            std::default_random_engine e;
            e.seed(r);
            std::uniform_real_distribution<float> randu(0.0,2.0*M_PI);
            float randomTheta=randu(e);

            ray.origin=camera.Position+glm::vec3(cos(randomTheta),sin(randomTheta),0.0f)*APERTURE;

            ray.dir(glm::normalize(glm::vec3(worldPos.x,worldPos.y,worldPos.z)-ray.origin));

            //找到最近的相交的物体
            ObjectIntersection* obj=ray.traceCameraRay(spheres,planes,model->meshes);

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

                   //因为默认是diffuse ,将该点的位置和颜色存起来
                   HitPoint *p=new HitPoint(iPoint, glm::vec2(i,j), pixelRadius[i*SCR_WIDTH+j]);
                   hitpoints.push_back(p);
                

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

                if(obj->type==2)
                {
                    std::cout<<"ray: ("<<i<<", "<<j<<") intersect with model"<<obj->index<<std::endl;

                    glm::vec3 normal=obj->normal;

                    
                    //f<<"normal: ("<<obj->normal.x<<", "<<obj->normal.y<<", "<<obj->normal.z<<")"<<std::endl;

                    //目前我们只加入了水面，此时我们默认所有的物体是透明的 但此时是水体，目前只模拟水面的散射
                    //直接计算折射
                    ray.waterRefr(iPoint, normal);
                    //f<<"newDir: ("<<i<<","<<j<<"):  ("<<ray.direction.x<<", "<<ray.direction.y<<", "<<ray.direction.z<<")"<<std::endl;
                }


                obj = ray.traceCameraRay(spheres, planes,model->meshes);
                bounces++;

            }

            if(obj->index!=-1 && bounces>=MAX_BOUNCES) //到达bounces数的话,即使的specular 和 transparent都要存储能量
            {
               
               glm::vec3 iPoint=ray.origin+ (obj->t) * ray.direction;
               if(iPoint.z>-5.0 && iPoint.z<0.0 && abs(iPoint.x)<1.5 && abs(iPoint.y)<1.5) 
               {
                  
                   
                    HitPoint *p=new HitPoint(iPoint, glm::vec2((float)i/SCR_WIDTH,(float)j/SCR_HEIGHT),pixelRadius[i*SCR_WIDTH+j]);
                    hitpoints.push_back(p);
                   
                  
               }

            }


            
            
            
            //f<<i*SCR_WIDTH*5+5*j<<": "<<rayTraceResult[i*SCR_WIDTH*5+5*j]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+1]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+2]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+3]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+4]<<std::endl;
           
          
           

       }
    }

    //f<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
    //f.close();	

    //for(int i=0;i<hitpoints.size();i++)
    //    std::cout<<"hitpoint "<<i<<": ("<<hitpoints[i]->pos.x<<", "<<hitpoints[i]->pos.y<<", "<<hitpoints[i]->pos.z<<")"<<std::endl;

    //construct KDTree for hitPonts
    KDTree*  hitPointTree=new KDTree();
    hitPointTree->root=hitPointTree->build(hitpoints,0,hitpoints.size()-1,0);
    //hitPointTree->printTree();

    return hitPointTree;
    

 
}

void SPPMScene::updateFlux(Photon *p, KDNode* currentNode)
{
    //std::cout<<"update Flux"<<std::endl;
    //遍历整个KDTree
   
    if(currentNode==nullptr) return;

    float distance=0.0f;
    if(p->position.x<currentNode->min.x) distance+=currentNode->min.x - p->position.x;
    if(p->position.x>currentNode->max.x) distance+=p->position.x - currentNode->max.x;

    if(p->position.y<currentNode->min.y) distance+=currentNode->min.y - p->position.y;
    if(p->position.y>currentNode->max.y) distance+=p->position.y - currentNode->max.y;

    if(p->position.z<currentNode->min.z) distance+=currentNode->min.z - p->position.z;
    if(p->position.z>currentNode->max.z) distance+=p->position.z - currentNode->max.z;

    if(distance > currentNode->maxr) return;

    //p是否能影响到当前 hitpoint
    if(glm::distance(p->position,currentNode->point->pos)<currentNode->point->radius)
    {
       

        //更新 hitpoint 的颜色
        //std::cout<<"is updating Flux, uv is: "<<currentNode->point->ij.x<<", "<<currentNode->point->ij.y<<std::endl;

        float reflectivity=0.3;
        float lambertBrdf=reflectivity/M_PI;
        glm::vec3 flux=lambertBrdf*p->power;

        currentNode->point->flux += flux;

        //更新hitPoint 附近光子数
        currentNode->point->photonsN=currentNode->point->photonsN + 1;

        //std::cout<<"PhotonSN: &&&&&&&&&&&&&&&&&&&&&&&: "<<currentNode->point->photonsN<<std::endl;

        //std::cout<<"is updating Flux, uv is: ("<<currentNode->point->ij.x<<", "<<currentNode->point->ij.y<<"). flux is: ("<<currentNode->point->flux.x<<", "<<currentNode->point->flux.y<<", "<<currentNode->point->flux.z<<")"<<std::endl;
        
    }

    

    //去左右子树
    updateFlux(p, currentNode->l);
    updateFlux(p, currentNode->r);
        
    
}

void SPPMScene::castPhotons(KDTree* tree, int roundPhotonNum, int round)
{
    srand(0);

    float photonPower =1.0f;

    std::cout<<"SPPMScene begin castPhotons"<<std::endl;
    //std::ofstream f("castPhotons.txt", std::ios::app); //创建data.txt文件，写入时采用追加方式
    //f<<"--------------------"<<std::endl;
   

    //遍历光源发出的所有光子
    for(int i=0;i<roundPhotonNum;i++)
    {
        std::cout<<"photon "<<i<<std::endl;

        int bounces =1 ,spec =0 ;
        bool diffuse=false;

        glm::vec3 photonColor=glm::vec3(1.0f,1.0f,1.0f);


        //初始化Ray的位置和方向
        PhotonRay ray(light->pos);
        ray.randDir((float)(round+1)); //将轮数设为方向的初始化种子

        // Photon is outside the room
		 
		if(fabs(ray.origin.x) > 1.5 || fabs(ray.origin.y) > 1.5) {
			continue;
		}


        //找到第一次的bounce处
        //判断Ray 和场景中所有物体是否相交，找到最近的交点，在这里bounce
        ObjectIntersection* obj=ray.tracePhotonRay(spheres,planes,model->meshes);

       while(obj->index!=-1 && bounces<MAX_BOUNCES)
       {
            glm::vec3 iPoint=ray.origin+ (obj->t) * ray.direction;

                if(iPoint.z<-5.0 || iPoint.z>0.0 || abs(iPoint.x)>1.5 || abs(iPoint.y)>1.5) 
                {  
                    std::cout<<"photon "<<i<<" out of the scene"<<std::endl;
                    
                    //f<<"photon "<<i<<" out of the scene"<<std::endl;
                    
                    
                    break;
                }

                
                if(obj->type ==1 ) //if intersect with plane
                {
                    //存储光线方向和撞击点

                    //计算光子能量
                    std::cout<<"photon "<<i<<" intersect with plane"<<obj->index<<std::endl;
                    //f<<"photon "<<i<<" intersect with plane"<<obj->index<<std::endl;

                    glm::vec3 normal=planes[obj->index]->pNormal(iPoint,ray.origin);

                    float NdotL=std::max(glm::dot(normal,normalize(ray.origin-iPoint)),0.0f); //Lambert

                    photonColor=photonColor*planes[obj->index]->color;


                    // Scaling power by number of bounces, uncomment costTheta for Lambertian reflection

                    photonColor=photonColor/sqrt((float)bounces); //NdotL ???

                    Photon *p=new Photon(iPoint,ray.direction,photonColor*photonPower);

                    if(spec>0 && diffuse== false){
                        std::cout<<"photon "<<i<<" stored by caustic map"<<std::endl;
                        //map->store(p);  //caustics
                        p->power=p->power*3.0f;
                        updateFlux(p,tree->root);
                        diffuse=true;
                    }
                    else{
                        //globalMap  
                        //光子停住并查找附近的hitpoint,更新hitPoint所对应的像素颜色
                        updateFlux(p,tree->root);

                    }

                    ray.diffuseRefl(iPoint);  //墙面默认漫反射

                    delete p;

                }


                if(obj->type==0)  //if intersect with ball
                {
                    std::cout<<"photon "<<i<<" intersect with sphere"<<obj->index<<std::endl;
                    //f<<"photon "<<i<<" intersect with sphere"<<obj->index<<std::endl;
                    spec++;
                    photonColor=photonColor*spheres[obj->index]->color;
                    photonColor=photonColor/sqrt((float)bounces);

                    Photon *p=new Photon(iPoint,ray.direction,photonColor*photonPower);

                    std::cout<<"photon "<<i<<" stored by global map"<<std::endl;
                    updateFlux(p,tree->root);

                    glm::vec3 normal=spheres[obj->index]->sNormal(iPoint);

                    if(spheres[obj->index]->opticalProp==SPECULAR)
                    {
                        ray.specularRefl(iPoint,normal);
                    }
                    else if(spheres[obj->index]->opticalProp==REFRACT)
                    {
                        ray.refr(iPoint,normal);
                    }

                    delete p;

                    
                }

                
                if(obj->type==2) //intersect with mesh ,这里mesh 从外面导入，依靠texture 来确定颜色，因为是水面，所以手动设置水面颜色为 （1，1，1）
                {
                    std::cout<<"photon "<<i<<" intersect with mesh"<<obj->index<<std::endl;
                    //f<<"photon "<<i<<" intersect with mesh"<<obj->index<<std::endl;
                    spec++;
                    
                    photonColor=photonColor/sqrt((float)bounces);

                    Photon *p=new Photon(iPoint,ray.direction,photonColor*photonPower);

                    std::cout<<"photon "<<i<<" stored by global map"<<std::endl;
                    updateFlux(p,tree->root);
                   
                    glm::vec3 normal=obj->normal;

                    ray.waterRefr(iPoint,normal);
                    //f<<"newDir: ("<<ray.direction.x<<", "<<ray.direction.y<<", "<<ray.direction.z<<")"<<std::endl;

                    delete p;

                }
                

                obj = ray.tracePhotonRay(spheres, planes,model->meshes);
	      	    bounces++;
            
       }

       //f<<"+++++++++++++++++++++++"<<std::endl;



       
    }
    //f.close();
    //tree->printTree();

}

void SPPMScene::updateAccRadiance(KDNode* n,float alpha) 
{
    if(n==nullptr) return;
    //根据UV 确定对应的像素
    int i=n->point->ij.x;
    int j=n->point->ij.y;


    //更新每个像素积累的光子数
    float N=float(nearestPhotonNum[i*SCR_WIDTH+j]);
    float M=float(n->point->photonsN);
    nearestPhotonNum[i*SCR_WIDTH+j]=N+alpha*M;

    //更新半径
    float r=pixelRadius[i*SCR_WIDTH+j];
    float r1=r*std::sqrt((N+alpha*M)/(N+M));
    pixelRadius[i*SCR_WIDTH+j]=r1;

     //更新积累的通量
    
     if(N+M>0)
        accRadiance[i*SCR_WIDTH+j]=(accRadiance[i*SCR_WIDTH+j]+n->point->flux)*((N+alpha*M)/(N+M));
     
     //std::cout<<"accRadiance 2: "<<accRadiance[i*SCR_WIDTH+j].x<<","<<accRadiance[i*SCR_WIDTH+j].y<<", "<<accRadiance[i*SCR_WIDTH+j].z<<")"<<std::endl;
    


    updateAccRadiance(n->l,alpha);
    updateAccRadiance(n->r,alpha);
}



void SPPMScene::SPPM(Camera camera, float alpha,int round, int numPhotonAll, int roundPhotonNum)
{
    
        //ray tracing and store hitPoints，每轮的的ray tracing 方向略有抖动
        //build KDTree for hit points


        
        KDTree* hitPointTree =rayTracing(camera, round);
        std::cout<<"SPPM end ray Tracing"<<std::endl;

        //hitPointTree->printTree();

        //photon mapping
        castPhotons(hitPointTree, roundPhotonNum, round);


        //更新积累的通量

        updateAccRadiance(hitPointTree->root, alpha);

        /*
        for(int i=0;i<SCR_WIDTH*SCR_HEIGHT;i++)
        {
            std::cout<<"radiance: ("<<accRadiance[i].x<<","<<accRadiance[i].y<<", "<<accRadiance[i].z<<")"<<std::endl;
        }
        */

        //更新像素颜色，并进行本轮的渲染

        for(int i=0;i<SCR_WIDTH;i++)
            for(int j=0;j<SCR_HEIGHT;j++)
            {
                float r=pixelRadius[i*SCR_WIDTH+j];
                float n=(float)nearestPhotonNum[i*SCR_WIDTH+j];
                if(r*r*n>0.0)
                {
                    rayTraceResult[i*SCR_WIDTH*5+5*j+2]=(accRadiance[i*SCR_WIDTH+j].x)/((float)M_PI*r*r*n);
                    rayTraceResult[i*SCR_WIDTH*5+5*j+3]=(accRadiance[i*SCR_WIDTH+j].y)/((float)M_PI*r*r*n);
                    rayTraceResult[i*SCR_WIDTH*5+5*j+4]=(accRadiance[i*SCR_WIDTH+j].z)/((float)M_PI*r*r*n);
                }
                

               

                //std::cout<<"uv: ("<<rayTraceResult[i*SCR_WIDTH*5+5*j+0]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+1]<<")  color: ("<<rayTraceResult[i*SCR_WIDTH*5+5*j+2]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+3]<<", "<<rayTraceResult[i*SCR_WIDTH*5+5*j+4]<<")"<<std::endl;
                
            }


        //delete 之前动态创建的KDTree ,hitPoints

        hitPointTree->deleteTree();

           
}