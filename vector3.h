#ifndef VECTOR3_H
#define VECTOR3_H

#include<sstream>

extern const double EPS;
extern const double PI;

class Vector3 {
public:
	double x , y , z;
	
	Vector3( double X = 0 , double Y = 0 , double Z = 0 ) : x( X ) , y( Y ) , z( Z ) {}
	~Vector3() {}

	friend Vector3 operator + ( const Vector3& , const Vector3& );
	friend Vector3 operator - ( const Vector3& , const Vector3& );
	friend Vector3 operator * ( const Vector3& , const double& );
	friend Vector3 operator / ( const Vector3& , const double& );
	friend Vector3 operator * ( const Vector3& , const Vector3& ); //cross product
	friend Vector3& operator += ( Vector3& , const Vector3& );
	friend Vector3& operator -= ( Vector3& , const Vector3& );
	friend Vector3& operator *= ( Vector3& , const double& );
	friend Vector3& operator /= ( Vector3& , const double& );
	friend Vector3& operator *= ( Vector3& , const Vector3& );
	friend Vector3 operator - ( const Vector3& );
	double Dot( const Vector3& );
	double Module2();
	double Module();
	double Distance2( Vector3& );
	double Distance( Vector3& );
	double& GetCoord( int axis );
	Vector3 GetUnitVector();
	void AssRandomVector();
	Vector3 GetAnVerticalVector();
	bool IsZeroVector();
	void Input( std::stringstream& fin );
	Vector3 Reflect( Vector3 N );
	Vector3 Refract( Vector3 N , double n );
	Vector3 Diffuse();
	Vector3 Rotate( Vector3 axis , double theta );
};

#endif
