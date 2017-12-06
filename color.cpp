#include"color.h"
#include<sstream>

Color operator + ( const Color& A , const Color& B ) {
	return Color( A.r + B.r , A.g + B.g , A.b + B.b );
}

Color operator - ( const Color& A , const Color& B ) {
	return Color( A.r - B.r , A.g - B.g , A.b - B.b );
}

Color operator * ( const Color& A , const Color& B ) {
	return Color( A.r * B.r , A.g * B.g , A.b * B.b );
}

Color operator * ( const Color& A , const double& k ) {
	return Color( A.r * k , A.g * k , A.b * k );
}

Color operator / ( const Color& A , const double& k ) {
	return Color( A.r / k , A.g / k , A.b / k );
}

Color& operator += ( Color& A , const Color& B ) {
	A = A + B;
	return A;
}

Color& operator -= ( Color& A , const Color& B ) {
	A = A - B;
	return A;
}

Color& operator *= ( Color& A , const double& k ) {
	A = A * k;
	return A;
}

Color& operator /= ( Color& A , const double& k ) {
	A = A / k;
	return A;
}

Color Color::Confine() {
	return Color( std::min( r , 1.0 ) , std::min( g , 1.0 ) , std::min( b , 1.0 ) );
}

double Color::Power() {
	return ( r + g + b ) / 3;
}

void Color::Input( std::stringstream& fin ) {
	fin >> r >> g >> b;
}
