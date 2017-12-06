#include"camera.h"
#include<cstdio>
#include<string>
#include<sstream>
#include<iostream>

const double STD_LENS_WIDTH = 0.88;
const double STD_LENS_HEIGHT = 0.88;
const int STD_IMAGE_WIDTH = 420;
const int STD_IMAGE_HEIGHT = 420;
const int STD_SHADE_QUALITY = 4;
const int STD_DREFL_QUALITY = 4;
const int STD_MAX_PHOTONS = 2000000;
const int STD_EMIT_PHOTONS = 1000000;
const int STD_SAMPLE_PHOTONS = 100;
const double STD_SAMPLE_DIST = 1;

Camera::Camera() {
	O = Vector3( 0 , 0 , 0 );
	N = Vector3( 0 , 1 , 0 );
	lens_W = STD_LENS_WIDTH;
	lens_H = STD_LENS_HEIGHT;
	W = STD_IMAGE_WIDTH;
	H = STD_IMAGE_HEIGHT;
	shade_quality = STD_SHADE_QUALITY;
	drefl_quality = STD_DREFL_QUALITY;
	max_photons = STD_MAX_PHOTONS;
	emit_photons = STD_EMIT_PHOTONS;
	sample_photons = STD_SAMPLE_PHOTONS;
	sample_dist = STD_SAMPLE_DIST;
	data = NULL;
}

Camera::~Camera() {
	if ( data == NULL ) {
		for ( int i = 0 ; i < H ; i++ )
			delete[] data[i];
		delete[] data;
	}
}

void Camera::Initialize() {
	N = N.GetUnitVector();
	Dx = N.GetAnVerticalVector();
	Dy = Dx * N;
	Dx = Dx * lens_W / 2;
	Dy = Dy * lens_H / 2;

	data = new Color*[H];
	for ( int i = 0 ; i < H ; i++ )
		data[i] = new Color[W];
}

Vector3 Camera::Emit( double i , double j ) {
	return N + Dy * ( 2 * i / H - 1 ) + Dx * ( 2 * j / W - 1 );
}

void Camera::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "N=" ) N.Input( fin );
	if ( var == "lens_W=" ) fin >> lens_W;
	if ( var == "lens_H=" ) fin >> lens_H;
	if ( var == "image_W=" ) fin >> W;
	if ( var == "image_H=" ) fin >> H;
	if ( var == "shade_quality=" ) fin >> shade_quality;
	if ( var == "drefl_quality=" ) fin >> drefl_quality;
	if ( var == "max_photons=" ) fin >> max_photons;
	if ( var == "emit_photons=" ) fin >> emit_photons;
	if ( var == "sample_photons=" ) fin >> sample_photons;
	if ( var == "sample_dist=" ) fin >> sample_dist;
}

void Camera::Output( Bmp* bmp ) {
	bmp->Initialize( H , W );

	for ( int i = 0 ; i < H ; i++ )
		for ( int j = 0 ; j < W ; j++ )
			bmp->SetColor( i , j , data[i][j] );
}
