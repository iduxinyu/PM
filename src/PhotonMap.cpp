#pragma once

#include"PhotonMap.h"



Photon::Photon(glm::vec3 Position, glm::vec3 Direction, glm::vec3 Power)
{
    position=Position;
    power=Power;

    int t=(int)(acos(Direction.z)*256.0/M_PI);

    if(t>255) 
        t=255;
    theta=(unsigned char)t;

    int p=(int)(atan2(Direction.y,Direction.x)*256.0/(2.0*M_PI));

    if(p>255)
        p=255;
    else if(p<0)
        p+=256;

    phi=(unsigned char)p; 



}

struct Node{
    Photon* photon;
    Node* left;
    Node* right;
};





PhotonMap::PhotonMap(int maxPhotonNum)
{
    storedPhotons = 0;
	maxPhotons = maxPhotonNum;
	photons = (Photon*)malloc(sizeof(Photon) * (maxPhotons));
	if(photons == NULL) {
		std::cout<<"Out of memory initalizing photon map"<<std::endl;
		exit(-1);
	}
	bboxMin.x = bboxMin.y = bboxMin.z = 1e5f;
	bboxMax.x = bboxMax.y = bboxMax.z = -1e5f;

    for (int i = 0; i < 256; i++) {
		float angle = float(i) * (1.0 / 256.0) * M_PI;
		cosTheta[i] = cos(angle);
		sinTheta[i] = sin(angle);
		cosPhi[i] = cos(2.0 * angle);
		sinPhi[i] = sin(2.0 * angle);
	}
}


void PhotonMap::store(Photon* p)
{
    if(storedPhotons>=maxPhotons)
        return;
    photons[storedPhotons]= *p; //存储内容到photons中
    storedPhotons++;
    if(p->position.x>bboxMax.x)
        bboxMax.x=p->position.x;
    if(p->position.x<bboxMin.x)
        bboxMin.x=p->position.x;
    if(p->position.y>bboxMax.y)
        bboxMax.y=p->position.y;
    if(p->position.y<bboxMin.y)
        bboxMin.y=p->position.y;
    if(p->position.z>bboxMax.z)
        bboxMax.z=p->position.z;
    if(p->position.z<bboxMin.z)
        bboxMin.z=p->position.z;

}

int PhotonMap::findMedian(int* photonIndex, int start,int end, int middle,int axis)
{
    //std::cout<<"findMedian: "<<start<<" - "<<end<<std::endl;
    if(start==end){
        return start;
    }
    if(axis==0)//sort by x value
    {   
        
        int pivot=photonIndex[start];
        int i=start;
        int j=end;
        if(i>=j)
        {
            return i;
        }
        while(i<j)
        {
            
            while(photons[photonIndex[j]].position.x>=photons[pivot].position.x && j>start)
            {
                j--;
            }
        

                
            while(photons[photonIndex[i]].position.x<=photons[pivot].position.x && i<end)
            {
                i++;
            }
            if(i<j)
            {
                int tmp=photonIndex[j];
                photonIndex[j]=photonIndex[i];
                photonIndex[i]=tmp;
            }
            
        }

        //change j and pivot
        photonIndex[start]=photonIndex[j];
        photonIndex[j]=pivot;

        if(j==middle)
        {
            std::cout<<"sort x: median is: "<<photonIndex[j]<<", j is "<< j <<std::endl;
            return j;
        }
        else if(middle<j)
        {
            return findMedian(photonIndex,start,j-1,middle,axis);
        }
        else
        {
            return findMedian(photonIndex,j+1,end,middle,axis);
        }    

    }
    if(axis==1)//sort by y value
    { 
        
        int pivot=photonIndex[start];
        int i=start;
        int j=end;
        if(i>=j)
        {
            return i;
        }
        while(i<j)
        {
            
            while(photons[photonIndex[j]].position.y>=photons[pivot].position.y && j>start)
            {
                j--;
            }
        

                
            while(photons[photonIndex[i]].position.y<=photons[pivot].position.y && i<end)
            {
                i++;
            }
            if(i<j)
            {
                int tmp=photonIndex[j];
                photonIndex[j]=photonIndex[i];
                photonIndex[i]=tmp;
            }
            
        }

        //change j and pivot
        photonIndex[start]=photonIndex[j];
        photonIndex[j]=pivot;

        if(j==middle)
        {
            std::cout<<"sort y: median is: "<<photonIndex[j]<<", j is "<< j <<std::endl;
            return j;
        }
        else if(middle<j)
        {
            return findMedian(photonIndex,start,j-1,middle,axis);
        }
        else
        {
            return findMedian(photonIndex,j+1,end,middle,axis);
        }    

    }
    if(axis==2)//sort by z value
    {   
        
        int pivot=photonIndex[start];
        int i=start;
        int j=end;
        if(i>=j)
        {
            return i;
        }
        while(i<j)
        {
            
            while(photons[photonIndex[j]].position.z>=photons[pivot].position.z && j>start)
            {
                j--;
            }
        

                
            while(photons[photonIndex[i]].position.z<=photons[pivot].position.z && i<end)
            {
                i++;
            }
            if(i<j)
            {
                int tmp=photonIndex[j];
                photonIndex[j]=photonIndex[i];
                photonIndex[i]=tmp;
            }
            
        }

        //change j and pivot
        photonIndex[start]=photonIndex[j];
        photonIndex[j]=pivot;

        if(j==middle)
        {
            std::cout<<"sort z: median is: "<<photonIndex[j]<<", j is "<< j <<std::endl;
            return j;
        }
        else if(middle<j)
        {
            return findMedian(photonIndex,start,j-1,middle,axis);
        }
        else
        {
            return findMedian(photonIndex,j+1,end,middle,axis);
        }    

    }
}



void PhotonMap::buildKdTree(int* photonIndex, int start, int end, int axis)
{
    if(start>=end)
    {
        return;
    }
    //find median
    int median= findMedian(photonIndex,start,end, (start+end)/2,axis);

    axis=(axis+1)%3;
    
    buildKdTree(photonIndex,start,median-1,axis);
    buildKdTree(photonIndex,median+1,end,axis);

    /*
    std::ofstream f("data.txt", std::ios::app); //创建data.txt文件，写入时采用追加方式
    
    for(int i=0;i<storedPhotons;i++)
    {
	    f << photonIndex[i] << ",  ";
    }
    f<<std::endl;
    
    f.close();	
    */

}

void PhotonMap::insertHeap(glm::vec3 rayPos, int* photonIndex, int currentI, int* heap, int k, int* size)
{
    
    //当前点入堆
    int i=photonIndex[currentI];
    if(*size<k)// 如果未满
    {
        heap[*size]=i;
        *size=*size+1;
        
        //sort heap;
        int child=*size-1;
        int father=(child-1)/2;
       

        while(child>0)
        {
           
            glm::vec3 childPos=photons[heap[child]].position;
            glm::vec3 fatherPos=photons[heap[father]].position;
            

            if(glm::distance(childPos,rayPos)>glm::distance(fatherPos,rayPos))
            {
                
                //互换位置并更新序号
                int tmp=heap[child];
                heap[child]=heap[father];
                heap[father]=tmp;

                child=father;
                father=(child-1)/2;
                
               
            }
            else
            {
                break;
            }
        }

        
    }
    else 
    {
       
        //和最顶端比较
        glm::vec3 pos=photons[i].position;
        glm::vec3 top=photons[heap[0]].position;

        if(glm::distance(pos,rayPos)<glm::distance(top,rayPos))
        {
            //入堆并sort;

            heap[0]=i;

            int father=0;
            int leftchild=father*2+1;
            int rightchild=father*2+2;

            while(leftchild<k)
            {

                glm::vec3 fatherPos=photons[heap[father]].position;
                glm::vec3 leftchildPos=photons[heap[leftchild]].position;




                if(rightchild<k) //右节点存在
                {
                    glm::vec3 rightchildPos=photons[heap[rightchild]].position;


                    if(glm::distance(fatherPos,rayPos)<glm::distance(leftchildPos,rayPos)) 
                    {
                        if(glm::distance(rightchildPos,rayPos)>glm::distance(leftchildPos,rayPos))//右节点距离更大，和右节点换
                        {
                            int tmp=heap[rightchild];
                            heap[rightchild]=heap[father];
                            heap[father]=tmp;

                            father=rightchild;

                            leftchild=father*2+1;
                            rightchild=father*2+2;
                        }
                        else
                        {
                            int tmp=heap[leftchild];
                            heap[leftchild]=heap[father];
                            heap[father]=tmp;

                            father=leftchild;

                            leftchild=father*2+1;
                            rightchild=father*2+2;
                        }

                    }
                    else if(glm::distance(fatherPos,rayPos)<glm::distance(rightchildPos,rayPos)) 
                    {
                        int tmp=heap[rightchild];
                            heap[rightchild]=heap[father];
                            heap[father]=tmp;

                            father=rightchild;

                            leftchild=father*2+1;
                            rightchild=father*2+2;
                    }
                    else
                    {
                        break;
                    }
                    
                }
                else //不存在，只和左节点比
                {
                    if(glm::distance(fatherPos,rayPos)<glm::distance(leftchildPos,rayPos))
                    {
                        int tmp=heap[leftchild];
                        heap[leftchild]=heap[father];
                        heap[father]=tmp;

                        father=leftchild;

                        leftchild=father*2+1;
                        rightchild=father*2+2;
                    }
                    else
                    {
                        break;
                    }
                }


                
            }
            
        }


    }
}

void PhotonMap::searchTree(glm::vec3 rayPos, int* photonIndex, int* heap, int start, int end, int k, int* size)
{
    
    if(start==end)
    {
        //当前点入堆
        insertHeap(rayPos, photonIndex, start, heap, k, size);
        


    }
    else if(start<end)
    {
        int middle=(start+end)/2;
        //当前点入堆
        insertHeap(rayPos, photonIndex, middle, heap, k, size);
        

        //搜索左子树
        searchTree(rayPos, photonIndex, heap,start,middle-1,k,size);

        //搜索右子树
        searchTree(rayPos, photonIndex, heap,middle+1,end,k,size);
    }

   
}

glm::vec3 PhotonMap::findNearest(int* photonIndex, glm::vec3 p, int k)
{


   
    int size=0;
    int low=0;
    int high=storedPhotons-1;
    int biggestHeap[k];

    
    searchTree(p, photonIndex, biggestHeap,low, high, k, &size);

    /*
    std::ofstream f("heap.txt", std::ios::app); //创建data.txt文件，写入时采用追加方式
    f<<"--------------------"<<std::endl;
    f<<"size: "<<size<<"。  ";
    for(int i=0;i<k;i++)
    {
	    f << biggestHeap[i] << ",  ";
    }
    f<<std::endl;
    
    f.close();	
    */
    
    //计算能量


    float radius=glm::distance(p,photons[biggestHeap[0]].position);
    glm::vec3 flux=glm::vec3(0.0f);
    for(int i=0;i<size;i++)
    {
        Photon pho=photons[biggestHeap[i]];

        //因为积累photon 的地方都是diffuse表面，所以使用lambert brdf
        float reflectivity=0.3;
        float lambertBrdf=reflectivity/(float)M_PI;
        flux+=lambertBrdf*pho.power;

    }

    flux=flux/((float)M_PI*radius*radius*size);

    return flux;

    
}
