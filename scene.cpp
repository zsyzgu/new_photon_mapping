#include"scene.h"
#include<string>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include<ctime>

Scene::Scene() {
	primitive_head = NULL;
	light_head = NULL;
	background_color = Color();
	camera = new Camera;
}

Scene::~Scene() {
	while ( primitive_head != NULL ) {
		Primitive* next_head = primitive_head->GetNext();
		if ( primitive_head->GetMaterial()->texture != NULL )
			delete primitive_head->GetMaterial()->texture;
		delete primitive_head;
		primitive_head = next_head;
	}

	while ( light_head != NULL ) {
		Light* next_head = light_head->GetNext();
		delete light_head;
		light_head = next_head;
	}

	delete camera;
}

void Scene::CreateScene( std::string file ) {
	srand( 1995 - 05 - 12 );
	std::ifstream fin( file.c_str() );

	std::string obj;
	while ( fin >> obj ) {
		Primitive* new_primitive = NULL;
		Light* new_light = NULL;

		if ( obj == "primitive" ) {
			std::string type; fin >> type;
			if ( type == "sphere" ) new_primitive = new Sphere;
			if ( type == "plane" ) new_primitive = new Plane;
			if ( type == "rectangle" ) new_primitive = new Rectangle;
			if ( new_primitive != NULL ) {
				new_primitive->SetNext( primitive_head );
				primitive_head = new_primitive;
			}
		} else
		if ( obj == "light" ) {
			std::string type; fin >> type;
			if ( type == "point" ) new_light = new PointLight;
			if ( type == "area" ) new_light = new AreaLight;
			if ( new_light != NULL ) {
				new_light->SetNext( light_head );
				light_head = new_light;
			}
		} else
		if ( obj != "background" && obj != "camera" ) continue;

		fin.ignore( 1024 , '\n' );
		
		std::string order;
		while ( getline( fin , order , '\n' ) ) {
			std::stringstream fin2( order );
			std::string var; fin2 >> var;
			if ( var == "end" ) break;

			if ( obj == "background" && var == "color=" ) background_color.Input( fin2 );
			if ( obj == "primitive" && new_primitive != NULL ) new_primitive->Input( var , fin2 );
			if ( obj == "light" && new_light != NULL ) new_light->Input( var , fin2 );
			if ( obj == "camera" ) camera->Input( var , fin2 );
		}
	}

	camera->Initialize();
}

Primitive* Scene::FindNearestPrimitive( Vector3 ray_O , Vector3 ray_V ) {
	Primitive* ret = NULL;

	for ( Primitive* now = primitive_head ; now != NULL ; now = now->GetNext() )
		if ( now->Collide( ray_O , ray_V ) && ( ret == NULL || now->crash.dist < ret->crash.dist ) ) ret = now;

	return ret;
}

Light* Scene::FindNearestLight( Vector3 ray_O , Vector3 ray_V ) {
	Light* ret = NULL;

	for ( Light* now = light_head ; now != NULL ; now = now->GetNext() )
		if ( now->Collide( ray_O , ray_V ) && ( ret == NULL || now->crash_dist < ret->crash_dist ) ) ret = now;

	return ret;
}
