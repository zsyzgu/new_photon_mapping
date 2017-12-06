#ifndef LIGHT_H
#define LIGHT_H

#include"vector3.h"
#include"color.h"
#include"primitive.h"
#include"photonmap.h"
#include<sstream>
#include<string>
#include<cmath>

extern const double EPS;
extern const int HASH_MOD;
extern const double SPEC_POWER;

class Light {
protected:
	int sample;
	Color color;
	Light* next;

	Color CalnIrradiance( Primitive* pri , Vector3 V , int* hash );

public:
	double crash_dist;

	Light();
	~Light() {}
	
	int GetSample() { return sample; }
	Color GetColor() { return color; }
	Light* GetNext() { return next; }
	void SetNext( Light* light ) { next = light; }

	virtual void Input( std::string , std::stringstream& );
	virtual Vector3 GetO() = 0;
	virtual bool Collide( Vector3 ray_O , Vector3 ray_V ) = 0;
	virtual Color GetIrradiance( Primitive* pri , Primitive* primitive_head , int shade_quality , int* hash ) = 0;
	virtual Photon EmitPhoton() = 0;
};

class PointLight : public Light {
	Vector3 O;
public:
	PointLight() : Light() {}
	~PointLight() {}
	
	Vector3 GetO() { return O; }
	void Input( std::string , std::stringstream& );
	bool Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetIrradiance( Primitive* pri , Primitive* primitive_head , int shade_quality , int* hash );
	Photon EmitPhoton();
};

class AreaLight : public Light {
	Vector3 O , Dx , Dy;
public:
	AreaLight() : Light() {}
	~AreaLight() {}
	
	Vector3 GetO() { return O; }
	void Input( std::string , std::stringstream& );
	bool Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetIrradiance( Primitive* pri , Primitive* primitive_head , int shade_quality , int* hash );
	Photon EmitPhoton();
};

#endif
