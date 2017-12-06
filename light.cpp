#include"light.h"
#include<sstream>
#include<string>
#include<cmath>
#include<cstdlib>
#include<ctime>
#include<iostream>
#define ran() ( double( rand() % RAND_MAX ) / RAND_MAX )

Light::Light() {
	sample = rand();
	next = NULL;
}

void Light::Input( std::string var , std::stringstream& fin ) {
	if ( var == "color=" ) color.Input( fin );
}

Color Light::CalnIrradiance( Primitive* pri , Vector3 V , int* hash ) {
	Color ret;

	double dist2 = V.Module2();
	V = V.GetUnitVector();
	double dot = V.Dot( pri->crash.N );

	if ( dot > EPS ) {
		if ( hash != NULL ) *hash = ( *hash + GetSample() ) % HASH_MOD;

		if ( pri->GetMaterial()->diff > EPS ) {
			double diff = pri->GetMaterial()->diff * dot / dist2;
			ret += color * diff;
		}
		if ( pri->GetMaterial()->spec > EPS ) {
			double spec = pri->GetMaterial()->spec * pow( dot , SPEC_POWER ) / dist2;
			ret += color * spec;
		}
	}

	return ret;
}

void PointLight::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	Light::Input( var , fin );
}

bool PointLight::Collide( Vector3 ray_O , Vector3 ray_V ) {
	return false;
}

Photon PointLight::EmitPhoton() {
	Photon ret;
	ret.power = color / color.Power();
	ret.pos = O;
	ret.dir.AssRandomVector();
	return ret;
}

Color PointLight::GetIrradiance( Primitive* pri , Primitive* primitive_head , int shade_quality , int* hash ) {
	Vector3 V = O - pri->crash.C;
	double dist = V.Module();

	for ( Primitive* now = primitive_head ; now != NULL ; now = now->GetNext() )
		if ( now->Collide( pri->crash.C , V ) && ( now->crash.dist < dist ) ) return Color();

	return CalnIrradiance( pri , V , hash );
}

void AreaLight::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Light::Input( var , fin );
}

bool AreaLight::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	Vector3 N = ( Dx * Dy ).GetUnitVector();
	double d = N.Dot( ray_V );
	if ( fabs( d ) < EPS ) return false;
	double l = ( N * O.Dot( N ) - ray_O ).Dot( N ) / d;
	if ( l < EPS ) return false;

	Vector3 C = ( ray_O + ray_V * l ) - O;
	if ( fabs( Dx.Dot( C ) ) > Dx.Dot( Dx ) ) return false;
	if ( fabs( Dy.Dot( C ) ) > Dy.Dot( Dy ) ) return false;

	crash_dist = l;
	return true;
}

Photon AreaLight::EmitPhoton() {
	Photon ret;
	ret.power = color / color.Power();
	ret.pos = O + Dx * ( ran() * 2 - 1 ) + Dy * ( ran() * 2 - 1 );
	ret.dir.AssRandomVector();
	return ret;
}

Color AreaLight::GetIrradiance( Primitive* pri , Primitive* primitive_head , int shade_quality , int* hash ) {
	Color ret;

	for ( int i = -2 ; i < 2 ; i++ )
		for ( int j = -2 ; j < 2 ; j++ )
			for ( int k = 0 ; k < shade_quality ; k++ ) {
				Vector3 V = O - pri->crash.C + Dx * ( ( ran() + i ) / 2 ) + Dy * ( ( ran() + j ) / 2 );
				double dist = V.Module();

				bool shade = false;
				for ( Primitive* now = primitive_head ; now != NULL ; now = now->GetNext() )
					if ( now->Collide( pri->crash.C , V ) && ( now->crash.dist < dist ) ) {
						shade = true;
						break;
					}
				
				if ( shade == false ) ret += CalnIrradiance( pri , V , NULL );
			}
	
	ret /= 16.0 * shade_quality;
	return ret;
}
