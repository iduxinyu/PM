#include "HitPoint.h"


bool cmpHitPointX(HitPoint *a, HitPoint *b)
{
    return a->pos.x < b->pos.x;
}

bool cmpHitPointY(HitPoint *a, HitPoint *b)
{
    return a->pos.y < b->pos.y;
}

bool cmpHitPointZ(HitPoint *a, HitPoint *b)
{
    return a->pos.z < b->pos.z;
}

glm::vec3 KDTree::findMinEdge(std::vector<HitPoint*> hitpoints, int start, int end)
{
    glm::vec3 result=glm::vec3(1e100,1e100,1e100);

    for(int i=start;i<=end;i++)
    {
        if(hitpoints[i]->pos.x<result.x)
        {
            result.x=hitpoints[i]->pos.x;
        }
        if(hitpoints[i]->pos.y<result.y)
        {
            result.y=hitpoints[i]->pos.y;
        }
        if(hitpoints[i]->pos.z<result.z)
        {
            result.z=hitpoints[i]->pos.z;
        }
    }

    return result;
}

glm::vec3 KDTree::findMaxEdge(std::vector<HitPoint*> hitpoints, int start, int end)
{
    glm::vec3 result=glm::vec3(-1e100,-1e100,-1e100);

    for(int i=start;i<=end;i++)
    {
        if(hitpoints[i]->pos.x>result.x)
        {
            result.x=hitpoints[i]->pos.x;
        }
        if(hitpoints[i]->pos.y>result.y)
        {
            result.y=hitpoints[i]->pos.y;
        }
        if(hitpoints[i]->pos.z>result.z)
        {
            result.z=hitpoints[i]->pos.z;
        }
    }

    return result;
}

float KDTree::findMaxR(std::vector<HitPoint*> hitpoints, int start, int end)
{
    float result=0.0;
    for(int i=start;i<=end;i++)
    {
        if(hitpoints[i]->radius>result)
        {
            result=hitpoints[i]->radius;
        }
       
    }

    return result;

}

KDNode* KDTree::build(std::vector<HitPoint*> hitpoints, int start, int end, int compareAxis)
{
    if(end < start)
        return nullptr;
    KDNode *p=new KDNode();
    p->min=findMinEdge(hitpoints,start,end);
    p->max=findMaxEdge(hitpoints,start,end);
    p->maxr=findMaxR(hitpoints,start,end);

    int m=(start+end)/2;

    if(compareAxis==0) //比较x
        std::nth_element(hitpoints.begin()+start,hitpoints.begin()+m,hitpoints.begin()+end,cmpHitPointX);
    else if(compareAxis==1)
        std::nth_element(hitpoints.begin()+start,hitpoints.begin()+m,hitpoints.begin()+end,cmpHitPointY);
    else if(compareAxis==2)
        std::nth_element(hitpoints.begin()+start,hitpoints.begin()+m,hitpoints.begin()+end,cmpHitPointZ);

    p->point=hitpoints[m];

    if(m%5000==0)
        std::cout<<"m: "<<m<<", hitpoint.uv:"<<p->point->ij.x<<", "<<p->point->ij.y<<std::endl;

    p->l=build(hitpoints,start,m-1,(compareAxis+1)%3);
    p->r=build(hitpoints,m+1,end,(compareAxis+1)%3);

    return p;

}

