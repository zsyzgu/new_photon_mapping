#ifndef PHOTONTRACER
#define PHOTONTRACER

#include"scene.h"
#include"photonmap.h"

extern const int MAX_PHOTONS;
extern const int MAX_PHOTONTRACING_DEP;

class Photontracer {
	Scene* scene;
	Photonmap* photonmap;

	void PhotonTracing( Photon , int dep );
	bool PhotonDiffusion( Primitive* , Photon , int dep , double* prob );
	bool PhotonReflection( Primitive* , Photon , int dep , double* prob );
	bool PhotonRefraction( Primitive* , Photon , int dep , double* prob );

public:
	void SetScene( Scene* input ) { scene = input; }
	Photonmap* GetPhotonmap() { return photonmap; }
	void Run();
};

#endif
