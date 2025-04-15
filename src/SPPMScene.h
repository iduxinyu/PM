#pragma once


#include "Scene.h"
#include "HitPoint.h"


const static float APERTURE=0.001;

class SPPMScene: public Scene{


public:

    Model* model;

   glm::vec3* accRadiance;
   int* nearestPhotonNum;
   float* pixelRadius;

    

public:
    SPPMScene(){};
    SPPMScene(float initRadius);
    
    KDTree* rayTracing(Camera camera, int r);
    void castPhotons(KDTree* tree, int roundPhotonNum, int round);
    void SPPM(Camera camera, float alpha, int round, int numPhotonAll, int roundPhotonNum);

    void updateFlux(Photon *p, KDNode* currentNode);
    void updateAccRadiance(KDNode* n, float alpha);


};