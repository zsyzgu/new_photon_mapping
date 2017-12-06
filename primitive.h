#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include"color.h"
#include"vector3.h"
#include"bmp.h"
#include<iostream>
#include<sstream>
#include<string>

extern const double EPS;
extern const double PI;

class Material {
public:
	Color color , absor;
	double refl , refr;
	double diff , spec;
	double rindex;
	double drefl;
	Bmp* texture;

	Material();
	~Material() {}

	void Input( std::string , std::stringstream& );
};

class Crash {
public:
	Vector3 N , C;
	double dist;
	bool front;
};

class Primitive {
protected:
	int sample;
	Material* material;
	Primitive* next;

public:
	Crash crash;

	Primitive();
	Primitive( const Primitive& );
	~Primitive();
	
	int GetSample() { return sample; }
	Material* GetMaterial() { return material; }
	Primitive* GetNext() { return next; }
	void SetNext( Primitive* primitive ) { next = primitive; }

	virtual void Input( std::string , std::stringstream& );
	virtual bool Collide( Vector3 ray_O , Vector3 ray_V ) = 0;
	virtual Color GetTexture() = 0;
	virtual Primitive* PrimitiveCopy() = 0;
};

class Sphere : public Primitive {
	Vector3 O , De , Dc;
	double R;

public:
	Sphere();
	~Sphere() {}

	void Input( std::string , std::stringstream& );
	bool Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetTexture();
	Primitive* PrimitiveCopy();
};

class Plane : public Primitive {
	Vector3 N , Dx , Dy;
	double R;

public:
	Plane() : Primitive() {}
	~Plane() {}

	void Input( std::string , std::stringstream& );
	bool Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetTexture();
	Primitive* PrimitiveCopy();
};

class Rectangle : public Primitive {
	Vector3 N , Dx , Dy;
	double R;

public:
	Rectangle() : Primitive() {}
	~Rectangle() {}

	void Input( std::string , std::stringstream& );
	bool Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetTexture();
	Primitive* PrimitiveCopy();
};

#endif
