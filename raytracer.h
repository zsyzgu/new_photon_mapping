#ifndef RAYTRACER_H
#define RAYTRACER_H

#include"bmp.h"
#include"scene.h"
#include"photontracer.h"
#include<string>

extern const double SPEC_POWER;
extern const int MAX_DREFL_DEP;
extern const int MAX_RAYTRACING_DEP;
extern const int HASH_FAC;
extern const int HASH_MOD;

class Raytracer {
	std::string input , output;
	Scene* scene;
	Photonmap* photonmap;
	Color CalnDiffusion( Primitive* pri , int* hash );
	Color CalnReflection( Primitive* pri , Vector3 ray_V , int dep , int* hash );
	Color CalnRefraction( Primitive* pri , Vector3 ray_V , int dep , int* hash );
	Color RayTracing( Vector3 ray_O , Vector3 ray_V , int dep , int* hash );

public:
	Raytracer();
	~Raytracer();
	
	void SetInput( std::string file ) { input = file; }
	void SetOutput( std::string file ) { output = file; }
	void Run();
};

#endif
