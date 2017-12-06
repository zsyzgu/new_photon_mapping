#ifndef CAMERA_H
#define CAMERA_H

#include"vector3.h"
#include"color.h"
#include"bmp.h"
#include<string>
#include<sstream>

extern const double STD_LENS_WIDTH; //the width of lens in the scene
extern const double STD_LENS_HEIGHT;
extern const int STD_IMAGE_WIDTH;
extern const int STD_IMAGE_HEIGTH;
extern const int STD_SHADE_QUALITY; //caln shade :: how many times will be run (*16)
extern const int STD_DREFL_QUALITY; //caln drefl :: how many times will be run (*16)
extern const int STD_MAX_PHOTONS;
extern const int STD_EMIT_PHOTONS;
extern const int STD_SAMPLE_PHOTONS;
extern const double STD_SAMPLE_DIST;

class Camera {
	Vector3 O , N , Dx , Dy;
	double lens_W , lens_H;
	int W , H;
	Color** data;
	double shade_quality;
	double drefl_quality;
	int max_photons;
	int emit_photons;
	int sample_photons;
	double sample_dist;

public:
	Camera();
	~Camera();
	
	Vector3 GetO() { return O; }
	int GetW() { return W; }
	int GetH() { return H; }
	void SetColor( int i , int j , Color color ) { data[i][j] = color; }
	double GetShadeQuality() { return shade_quality; }
	double GetDreflQuality() { return drefl_quality; }
	int GetMaxPhotons() { return max_photons; }
	int GetEmitPhotons() { return emit_photons; }
	int GetSamplePhotons() { return sample_photons; }
	double GetSampleDist() { return sample_dist; }

	Vector3 Emit( double i , double j );
	void Initialize();
	void Input( std::string var , std::stringstream& fin );
	void Output( Bmp* );
};

#endif
