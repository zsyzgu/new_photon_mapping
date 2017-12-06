#include"bmp.h"
#include<cstdio>
#include<fstream>
#include<iostream>
#include<string>
#include<cmath>

using namespace std;

Bmp::Bmp( int H , int W ) {
	Initialize( H , W );
}

Bmp::~Bmp() {
	Release();
}

void Bmp::Initialize( int H , int W ) {
	strHead.bfReserved1 = 0;
	strHead.bfReserved2 = 0;
	strHead.bfOffBits = 54;

	strInfo.biSize = 40;
	strInfo.biPlanes = 1;
	strInfo.biHeight = H;
	strInfo.biWidth = W;
	strInfo.biBitCount = 24;
	strInfo.biCompression = 0;
	strInfo.biSizeImage = H * W * 3;
	strInfo.biXPelsPerMeter = 0;
	strInfo.biYPelsPerMeter = 0;
	strInfo.biClrUsed = 0;
	strInfo.biClrImportant = 0;

	strHead.bfSize = strInfo.biSizeImage + strInfo.biBitCount;

	ima = new IMAGEDATA*[H];
	for ( int i = 0 ; i < H ; i++ )
		ima[i] = new IMAGEDATA[W];
}

void Bmp::Release() {
	for ( int i = 0 ; i < strInfo.biHeight ; i++ )
		delete[] ima[i];

	delete[] ima;
}

void Bmp::Input( std::string file ) {
	Release();

	FILE *fpi = fopen( file.c_str() , "rb" );
	word bfType;
	fread( &bfType , 1 , sizeof( word ) , fpi );
	fread( &strHead , 1 , sizeof( BITMAPFILEHEADER ) , fpi );
	fread( &strInfo , 1 , sizeof( BITMAPINFOHEADER ) , fpi );
	
	RGBQUAD Pla;
	for ( int i = 0 ; i < ( int ) strInfo.biClrUsed ; i++ ) {
		fread( ( char * ) & ( Pla.rgbBlue ) , 1 , sizeof( byte ) , fpi );
		fread( ( char * ) & ( Pla.rgbGreen ) , 1 , sizeof( byte ) , fpi );
		fread( ( char * ) & ( Pla.rgbRed ) , 1 , sizeof( byte ) , fpi );
	}

	Initialize( strInfo.biHeight , strInfo.biWidth );
	for(int i = 0 ; i < strInfo.biHeight ; i++ )
		for(int j = 0 ; j < strInfo.biWidth ; j++ ) {
			fread( &ima[i][j].blue , 1 , sizeof( byte ) , fpi );
			fread( &ima[i][j].green , 1 , sizeof( byte ) , fpi );
			fread( &ima[i][j].red , 1 , sizeof( byte ) , fpi );
		}

	fclose( fpi );
}

void Bmp::Output( std::string file ) {
	FILE *fpw = fopen( file.c_str() , "wb" );

	word bfType = 0x4d42;
	fwrite( &bfType , 1 , sizeof( word ) , fpw );
	fwrite( &strHead , 1 , sizeof( BITMAPFILEHEADER ) , fpw );
	fwrite( &strInfo , 1 , sizeof( BITMAPINFOHEADER ) , fpw );

	for ( int i = 0 ; i < strInfo.biHeight ; i++ )
		for ( int j = 0 ; j < strInfo.biWidth ; j++ ) {
			fwrite( &ima[i][j].blue , 1 , sizeof( byte ) , fpw );
			fwrite( &ima[i][j].green , 1 , sizeof( byte ) , fpw );
			fwrite( &ima[i][j].red , 1 , sizeof( byte ) , fpw );
		}
	
	fclose( fpw );
}

void Bmp::SetColor( int i , int j , Color col ) {
	ima[i][j].red = ( int ) ( col.r * 255 );
	ima[i][j].green = ( int ) ( col.g * 255 );
	ima[i][j].blue = ( int ) ( col.b * 255 );
}

Color Bmp::GetSmoothColor( double u , double v ) {
	double U = ( u - floor( u ) ) * strInfo.biHeight;
	double V = ( v - floor( v ) ) * strInfo.biWidth;
	int U1 = ( int ) floor( U + EPS ) , U2 = U1 + 1;
	int V1 = ( int ) floor( V + EPS ) , V2 = V1 + 1;
	double rat_U = U2 - U;
	double rat_V = V2 - V;
	if ( U1 < 0 ) U1 = strInfo.biHeight - 1; if ( U2 == strInfo.biHeight ) U2 = 0;
	if ( V1 < 0 ) V1 = strInfo.biWidth - 1; if ( V2 == strInfo.biWidth ) V2 = 0;
	Color ret;
	ret = ret + ima[U1][V1].GetColor() * rat_U * rat_V;
	ret = ret + ima[U1][V2].GetColor() * rat_U * ( 1 - rat_V );
	ret = ret + ima[U2][V1].GetColor() * ( 1 - rat_U ) * rat_V;
	ret = ret + ima[U2][V2].GetColor() * ( 1 - rat_U ) * ( 1 - rat_V );
	return ret;
}
