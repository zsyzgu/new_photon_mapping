#include"photontracer.h"
#include"scene.h"
#include<cstdlib>
#include<iostream>
#define ran() ( double( rand() % RAND_MAX ) / RAND_MAX )

const int MAX_PHOTONS = 10000000;
const int MAX_PHOTONTRACING_DEP = 10;

bool Photontracer::PhotonDiffusion( Primitive* pri , Photon photon , int dep , double* prob ) {
	Material* material = pri->GetMaterial();
	double eta = ( material->diff + material->spec ) * material->color.Power();

	if ( eta <= ran() * ( *prob ) ) {
		*prob -= eta;
		return false;
	}

	photon.dir = pri->crash.N.Diffuse();
	photon.power = photon.power * material->color / material->color.Power();
	PhotonTracing( photon , dep + 1 );
	return true;
}

bool Photontracer::PhotonReflection( Primitive* pri , Photon photon , int dep , double* prob ) {
	Material* material = pri->GetMaterial();
	double eta = material->refl * material->color.Power();

	if ( eta <= ran() * ( *prob ) ) {
		*prob -= eta;
		return false;
	}
	
	photon.dir = photon.dir.Reflect( pri->crash.N );
	if ( material->drefl > EPS ) {
		Vector3 Dx = photon.dir.GetAnVerticalVector();
		Vector3 Dy = photon.dir * Dx;
		Dx = Dx.GetUnitVector() * material->drefl;
		Dy = Dy.GetUnitVector() * material->drefl;
		double x , y;
		do {
			x = ran() * 2 - 1;
			y = ran() * 2 - 1;
		} while ( x * x + y * y > 1 );
		x *= material->drefl;
		y *= material->drefl;
		photon.dir += Dx * x + Dy * y;
	}
	photon.power = photon.power * material->color / material->color.Power();
	PhotonTracing( photon , dep + 1 );
	return true;
}

bool Photontracer::PhotonRefraction( Primitive* pri , Photon photon , int dep , double* prob ) {
	Material* material = pri->GetMaterial();
	double eta = material->refr;
	if ( pri->crash.front == false ) {
		Color absor = material->absor * -pri->crash.dist;
		Color trans = Color( exp( absor.r ) , exp( absor.g ) , exp( absor.b ) );
		eta *= trans.Power();
		photon.power = photon.power * trans / trans.Power();
	}

	if ( eta <= ran() * ( *prob ) ) {
		*prob -= eta;
		return false;
	}
	
	double n = material->rindex;
	if ( pri->crash.front ) n = 1 / n;
	photon.dir = photon.dir.Refract( pri->crash.N , n );
	PhotonTracing( photon , dep + 1 );
	return true;
}

void Photontracer::PhotonTracing( Photon photon , int dep ) {
	if ( dep > MAX_PHOTONTRACING_DEP ) return;
	Primitive* nearest_primitive = scene->FindNearestPrimitive( photon.pos , photon.dir );

	if ( nearest_primitive != NULL ) {
		Primitive* primitive = nearest_primitive->PrimitiveCopy();
		
		photon.pos = primitive->crash.C;
		if ( primitive->GetMaterial()->diff > EPS /*&& dep > 1*/ )
			photonmap->Store( photon );

		double prob = 1;
		if ( PhotonDiffusion( primitive , photon , dep , &prob ) == false )
		if ( PhotonReflection( primitive , photon , dep , &prob ) == false )
		if ( PhotonRefraction( primitive , photon , dep , &prob ) == false );

		delete primitive;
	}
}

void Photontracer::Run() {
	int n = scene->GetCamera()->GetEmitPhotons();
	photonmap = new Photonmap( scene->GetCamera()->GetMaxPhotons() );
	photonmap->SetEmitPhotons( n );

	double total_power = 0;
	for ( Light* light = scene->GetLightHead() ; light != NULL ; light = light->GetNext() )
		total_power += light->GetColor().Power();
	double photon_power = total_power / n;
	
	int emited_photons = 0;
	for ( Light* light = scene->GetLightHead() ; light != NULL ; light = light->GetNext() ) {
		double light_power = light->GetColor().Power();
		while ( light_power >= photon_power ) {
			if ( ( ++emited_photons & 1048575 ) == 0 ) std::cout << "Emited photons: " << emited_photons << std::endl;
			Photon photon = light->EmitPhoton();
			photon.power *= total_power;
			PhotonTracing( photon , 1 );
			light_power -= photon_power;
		}
		std::cout << "Stored photons: " << photonmap->GetStoredPhotons() << std::endl;
	}
	

	std::cout << "Tree balancing..." << std::endl;
	photonmap->Balance();
}
