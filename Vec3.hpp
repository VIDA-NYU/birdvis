/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// Copyright 2011-2013 NYU Polytechnic School of Engineering.                      //
// Copyright 2010-2011 University of Utah.                                         //
//                                                                                 //
// Permission is hereby granted, free of charge, to any person obtaining a copy    //
// of this software and associated documentation files (the "Software"), to dea    //
// in the Software without restriction, including without limitation the rights    //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       //
// copies of the Software, and to permit persons to whom the Software is           //
// furnished to do so, subject to the following conditions:		           //
// 										   //
// The above copyright notice and this permission notice shall be included in	   //
// all copies or substantial portions of the Software.			           //
// 										   //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR	   //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,	   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          // 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
// THE SOFTWARE.                                                                   //
//                                                                                 // 
/////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Vec3

#ifndef VEC3_H_
#define VEC3_H_

#include <string>
#include <cstdio>
#include <iostream>

#include <math.h>

using namespace std;

class Vec3{
public:
    double x, y,z;

    Vec3()
    {
	this->x = 0;
	this->y = 0;
	this->z = 0;
    }

    Vec3(double x, double y, double z)
    {
	this->x = x;
	this->y = y;
	this->z = z;
    }

    Vec3(const Vec3 &v)
    {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
    }

    Vec3 & operator=(const Vec3 &rhs) {
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	return *this;  // Return a reference to myself.
    }

    Vec3 & operator()(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
	return *this;
    }

    Vec3 & operator+=(const Vec3 &rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
    }

    Vec3 & operator-=(const Vec3 &rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
    }

    Vec3 & operator/=(double c) {
	if(c != 0){
	    this->x /= c;
	    this->y /= c;
	    this->z /= c;
	}
	return *this;
    }    

    Vec3 & operator*=(double c) {
	this->x *= c;
	this->y *= c;
	this->z *= c;
	return *this;
    }    

    double operator*(const Vec3 &rhs) const {
	return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
    }    

    const Vec3 operator+(const Vec3 &rhs) const {
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    const Vec3 operator-(const Vec3 &rhs) const {
	return Vec3(x - rhs.x, y - rhs.y,z - rhs.z);
    }

    const Vec3 operator-() const {
	return Vec3(-x, -y, -z);
    }

    const Vec3 operator*(double c) const {
	return Vec3(*this) *= c;
    }

    const Vec3 operator/(double c) const {
	return Vec3(*this) /= c;
    }

    bool operator==(const Vec3 &other) const {
	return (this->x == other.x && this->y == other.y 
		&& this->z == other.z);
    }

    bool operator!=(const Vec3 &other) const {
	return !(*this == other);
    }     

    string str() {
	char buffer[100];
	sprintf(buffer,"(%lf,%lf,%lf)",x,y,z);
	return string(buffer);
    }

    double len2() const {
	return x * x + y * y + z * z;
    }

    double len() const {
	return sqrt(x * x + y * y + z*z);
    }

    Vec3 & normalize() {
	(*this) /= len();
	return (*this);
    }

    const Vec3 & normalized() const {
	return Vec3(*this) /= len();
    }

    double &operator [] (int i)
    { 
	switch(i){
	case(0):
	    return x;
	case(1):
	    return y;
	case(2):
	    return z;	    
	// default:
	//     printf("Error!! This is a Vec3, unable to get position %d\n",i);
	//     return 0;	    
	}
    }

    const Vec3 operator%(const Vec3 &rhs){
	return Vec3((this->y)*rhs.z - (this->z)*rhs.y,
		    (this->z)*rhs.x - (this->x)*rhs.z,
		    (this->x)*rhs.y - (this->y)*rhs.x);
    }
#define CROSS %

};


#endif /* VEC3_H_ */
