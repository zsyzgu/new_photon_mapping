#include"primitive.h"
#include<sstream>
#include<cstdio>
#include<string>
#include<cmath>
#include<iostream>
#include<cstdlib>

Material::Material() {
	color = absor = Color();
	refl = refr = diff = spec = 0;
	rindex = 0;
	drefl = 0;
	texture = NULL;
}

void Material::Input( std::string var , std::stringstream& fin ) {
	if ( var == "color=" ) color.Input( fin );
	if ( var == "absor=" ) absor.Input( fin );
	if ( var == "refl=" ) fin >> refl;
	if ( var == "refr=" ) fin >> refr;
	if ( var == "diff=" ) fin >> diff;
	if ( var == "spec=" ) fin >> spec;
	if ( var == "drefl=" ) fin >> drefl;
	if ( var == "rindex=" ) fin >> rindex;
	if ( var == "texture=" ) {
		std::string file; fin >> file;
		texture = new Bmp;
		texture->Input( file );
	}
}

Primitive::Primitive() {
	sample = rand();
	material = new Material;
	next = NULL;
}

Primitive::Primitive( const Primitive& primitive ) {
	*this = primitive;
	material = new Material;
	*material = *primitive.material;
}

Primitive::~Primitive() {
	delete material;
}

void Primitive::Input( std::string var , std::stringstream& fin ) {
	material->Input( var , fin );
}

Sphere::Sphere() : Primitive() {
	De = Vector3( 0 , 0 , 1 );
	Dc = Vector3( 0 , 1 , 0 );
}

void Sphere::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "De=" ) De.Input( fin );
	if ( var == "Dc=" ) Dc.Input( fin );
	Primitive::Input( var , fin );
}

bool Sphere::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	Vector3 P = ray_O - O;
	double b = -P.Dot( ray_V );
	double det = b * b - P.Module2() + R * R;

	if ( det > EPS ) {
		det = sqrt( det );
		double x1 = b - det  , x2 = b + det;

		if ( x2 < EPS ) return false;
		if ( x1 > EPS ) {
			crash.dist = x1;
			crash.front = true;
		} else {
			crash.dist = x2;
			crash.front = false;
		} 
	} else 
		return false;

	crash.C = ray_O + ray_V * crash.dist;
	crash.N = ( crash.C - O ).GetUnitVector();
	if ( crash.front == false ) crash.N = -crash.N;
	return true;
}

Color Sphere::GetTexture() {
	Vector3 I = ( crash.C - O ).GetUnitVector();
	double a = acos( -I.Dot( De ) );
	double b = acos( std::min( std::max( I.Dot( Dc ) / sin( a ) , -1.0 ) , 1.0 ) );
	double u = a / PI , v = b / 2 / PI;
	if ( I.Dot( Dc * De ) < 0 ) v = 1 - v;
	return material->texture->GetSmoothColor( u , v );
}

Primitive* Sphere::PrimitiveCopy() {
	Sphere* ret = new Sphere( *this );
	return ret;
}

void Plane::Input( std::string var , std::stringstream& fin ) {
	if ( var == "N=" ) N.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Primitive::Input( var , fin );
}

bool Plane::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	N = N.GetUnitVector();
	double d = N.Dot( ray_V );
	if ( fabs( d ) < EPS ) return false;
	double l = ( N * R - ray_O ).Dot( N ) / d;
	if ( l < EPS ) return false;

	crash.dist = l;
	crash.front = ( d < 0 );
	crash.C = ray_O + ray_V * crash.dist;
	crash.N = ( crash.front ) ? N : -N;
	return true;
}

Color Plane::GetTexture() {
	double u = crash.C.Dot( Dx ) / Dx.Module2();
	double v = crash.C.Dot( Dy ) / Dy.Module2();
	return material->texture->GetSmoothColor( u , v );
}

Primitive* Plane::PrimitiveCopy() {
	Plane* ret = new Plane( *this );
	return ret;
}

void Rectangle::Input( std::string var , std::stringstream& fin ) {
	if ( var == "N=" ) N.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Primitive::Input( var , fin );
}

bool Rectangle::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	N = N.GetUnitVector();
	double d = N.Dot( ray_V );
	if ( fabs( d ) < EPS ) return false;
	double l = ( N * R - ray_O ).Dot( N ) / d;
	if ( l < EPS ) return false;

	crash.dist = l;
	crash.front = ( d < 0 );
	crash.C = ray_O + ray_V * crash.dist;
	crash.N = ( crash.front ) ? N : -N;
	double u = crash.C.Dot( Dx ) / Dx.Module2();
	double v = crash.C.Dot( Dy ) / Dy.Module2();
	return 0 <= u && u <= 1 && 0 <= v && v <= 1;
}

Color Rectangle::GetTexture() {
	double u = crash.C.Dot( Dx ) / Dx.Module2();
	double v = crash.C.Dot( Dy ) / Dy.Module2();
	return material->texture->GetSmoothColor( u , v );
}

Primitive* Rectangle::PrimitiveCopy() {
	Rectangle* ret = new Rectangle( *this );
	return ret;
}
