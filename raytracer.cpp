#include"raytracer.h"
#include<cstdlib>
#include<iostream>
#define ran() ( double( rand() % RAND_MAX ) / RAND_MAX )

const double SPEC_POWER = 20;
const int MAX_DREFL_DEP = 2;
const int MAX_RAYTRACING_DEP = 10;
const int HASH_FAC = 7;
const int HASH_MOD = 10000007;

Raytracer::Raytracer() {
	scene = new Scene;
	photonmap = NULL;
}

Raytracer::~Raytracer() {
	if ( scene != NULL ) delete scene;
	if ( photonmap != NULL ) delete photonmap;
}

Color Raytracer::CalnDiffusion( Primitive* pri , int* hash ) {
	Color color = pri->GetMaterial()->color;

	if ( pri->GetMaterial()->texture != NULL ) color = color * pri->GetTexture();
	
	Color ret = color * scene->GetBackgroundColor() * pri->GetMaterial()->diff;

	Camera* camera = scene->GetCamera();
	ret += color * photonmap->GetIrradiance( pri->crash.C , pri->crash.N , camera->GetSampleDist() , camera->GetSamplePhotons() ) * pri->GetMaterial()->diff;
	
	//for ( Light* light = scene->GetLightHead() ; light != NULL ; light = light->GetNext() )
	//	ret += color * light->GetIrradiance( pri , scene->GetPrimitiveHead() , scene->GetCamera()->GetShadeQuality() , hash );

	return ret;
}

Color Raytracer::CalnReflection( Primitive* pri , Vector3 ray_V , int dep , int* hash ) {
	ray_V = ray_V.Reflect( pri->crash.N );

	if ( pri->GetMaterial()->drefl < EPS || dep > MAX_DREFL_DEP )
		return RayTracing( pri->crash.C , ray_V , dep + 1 , hash ) * pri->GetMaterial()->color * pri->GetMaterial()->refl;

	Vector3 Dx = ray_V.GetAnVerticalVector();
	Vector3 Dy = ray_V * Dx;
	Dx = Dx.GetUnitVector() * pri->GetMaterial()->drefl;
	Dy = Dy.GetUnitVector() * pri->GetMaterial()->drefl;

	Color ret;
	for ( int k = 0 ; k < 16 * scene->GetCamera()->GetDreflQuality() ; k++ ) {
		double x , y;
		do {
			x = ran() * 2 - 1;
			y = ran() * 2 - 1;
		} while ( x * x + y * y > 1 );
		x *= pri->GetMaterial()->drefl;
		y *= pri->GetMaterial()->drefl;

		ret += RayTracing( pri->crash.C , ray_V + Dx * x + Dy * y , dep + MAX_DREFL_DEP , NULL );
	}

	ret = ret * pri->GetMaterial()->color * pri->GetMaterial()->refl / ( 16 * scene->GetCamera()->GetDreflQuality() );
	return ret;
}

Color Raytracer::CalnRefraction( Primitive* pri , Vector3 ray_V , int dep , int* hash ) {
	double n = pri->GetMaterial()->rindex;
	if ( pri->crash.front ) n = 1 / n;
	
	ray_V = ray_V.Refract( pri->crash.N , n );
	
	Color rcol = RayTracing( pri->crash.C , ray_V , dep + 1 , hash );
	if ( pri->crash.front ) return rcol * pri->GetMaterial()->refr;
	Color absor = pri->GetMaterial()->absor * -pri->crash.dist;
	Color trans = Color( exp( absor.r ) , exp( absor.g ) , exp( absor.b ) );
	return rcol * trans * pri->GetMaterial()->refr;
}

Color Raytracer::RayTracing( Vector3 ray_O , Vector3 ray_V , int dep , int* hash ) {
	if ( dep > MAX_RAYTRACING_DEP ) return Color();

	Color ret;
	Primitive* nearest_primitive = scene->FindNearestPrimitive( ray_O , ray_V );
	Light* nearest_light = scene->FindNearestLight( ray_O , ray_V );

	if ( nearest_light != NULL && ( nearest_primitive == NULL || nearest_light->crash_dist < nearest_primitive->crash.dist ) ) {
		if ( hash != NULL ) *hash = ( *hash + nearest_light->GetSample() ) % HASH_MOD;
		ret += nearest_light->GetColor().Confine();
	}
	
	if ( nearest_primitive != NULL ) {
		if ( hash != NULL ) *hash = ( *hash + nearest_primitive->GetSample() ) % HASH_MOD;
		Primitive* primitive = nearest_primitive->PrimitiveCopy();
		if ( primitive->GetMaterial()->diff > EPS ) ret += CalnDiffusion( primitive , hash );
		if ( primitive->GetMaterial()->refl > EPS ) ret += CalnReflection( primitive , ray_V , dep , hash );
		if ( primitive->GetMaterial()->refr > EPS ) ret += CalnRefraction( primitive , ray_V , dep , hash );
		delete primitive;
	}

	if ( hash != NULL ) *hash = ( *hash * HASH_FAC ) % HASH_MOD;
	if ( dep == 1 ) ret = ret.Confine();
	return ret;
}

void Raytracer::Run() {
	Camera* camera = scene->GetCamera();
	scene->CreateScene( input );

	Photontracer* photontracer = new Photontracer;
	photontracer->SetScene( scene );
	photontracer->Run();
	photonmap = photontracer->GetPhotonmap();
	delete photontracer;

	Vector3 ray_O = camera->GetO();
	int H = camera->GetH() , W = camera->GetW();
	int** sample = new int*[H];
	for ( int i = 0 ; i < H ; i++ ) {
		sample[i] = new int[W];
		for ( int j = 0 ; j < W ; j++ )
			sample[i][j] = 0;
	}

	for ( int i = 0 ; i < H ; std::cout << "Sampling:   " << ++i << "/" << H << std::endl ) {
		for ( int j = 0 ; j < W ; j++ ) {
			Vector3 ray_V = camera->Emit( i , j );
			Color color = RayTracing( ray_O , ray_V , 1 , &sample[i][j] );
			camera->SetColor( i , j , color );
		}
	}

	/*for ( int i = 0 ; i < H ; std::cout << "Resampling: " << ++i << "/" << H << std::endl ) {
		for ( int j = 0 ; j < W ; j++ ) {
			if ( ( i == 0 || sample[i][j] == sample[i - 1][j] ) && ( i == H - 1 || sample[i][j] == sample[i + 1][j] ) &&
			     ( j == 0 || sample[i][j] == sample[i][j - 1] ) && ( j == W - 1 || sample[i][j] == sample[i][j + 1] ) ) continue;

			Color color;
			for ( int r = -1 ; r <= 1 ; r++ )
				for ( int c = -1 ; c <= 1 ; c++ ) {
					Vector3 ray_V = camera->Emit( i + ( double ) r / 3 , j + ( double ) c / 3 );
					color += RayTracing( ray_O , ray_V , 1 , NULL ) / 9;
				}
			camera->SetColor( i , j , color );
		}
	}*/
	
	for ( int i = 0 ; i < H ; i++ )
		delete[] sample[i];
	delete[] sample;

	Bmp* bmp = new Bmp( H , W );
	camera->Output( bmp );
	bmp->Output( output );
	delete bmp;
}
