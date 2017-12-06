#ifndef SCENE_H
#define SCENE_H

#include"color.h"
#include"vector3.h"
#include"primitive.h"
#include"light.h"
#include"camera.h"
#include<string>
#include<fstream>
#include<sstream>

class Scene {
	Primitive* primitive_head;
	Light* light_head;
	Color background_color;
	Camera* camera;

public:
	Scene();
	~Scene();
	
	Primitive* GetPrimitiveHead() { return primitive_head; }
	Light* GetLightHead() { return light_head; }
	Color GetBackgroundColor() { return background_color; }
	Camera* GetCamera() { return camera; }

	void CreateScene( std::string file );
	Primitive* FindNearestPrimitive( Vector3 ray_O , Vector3 ray_V );
	Light* FindNearestLight( Vector3 ray_O , Vector3 ray_V );
};

#endif
