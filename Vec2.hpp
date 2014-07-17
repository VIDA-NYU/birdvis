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
// Vec2

#ifndef VEC2_H_
#define VEC2_H_

#include <cmath>

class Vec2{
public:
    double x, y;

    Vec2()
    {
	this->x = 0;
	this->y = 0;
    }

    Vec2(double x, double y)
    {
	this->x = x;
	this->y = y;
    }

    Vec2(const Vec2 &v)
    {
	this->x = v.x;
	this->y = v.y;
    }

    Vec2 & operator=(const Vec2 &rhs) {
	this->x = rhs.x;
	this->y = rhs.y;
	return *this;  // Return a reference to myself.
    }

    Vec2 & operator()(double x, double y) {
	this->x = x;
	this->y = y;
	return *this;
    }

    Vec2 & operator+=(const Vec2 &rhs) {
	x += rhs.x;
	y += rhs.y;
	return *this;
    }

    Vec2 & operator-=(const Vec2 &rhs) {
	x -= rhs.x;
	y -= rhs.y;
	return *this;
    }

    Vec2 & operator/=(double c) {
	this->x /= c;
	this->y /= c;
	return *this;
    }    

    Vec2 & operator*=(double c) {
	this->x *= c;
	this->y *= c;
	return *this;
    }    

    double operator*(const Vec2 &rhs) const {
	return this->x * rhs.x + this->y * rhs.y;
    }    

    const Vec2 operator+(const Vec2 &rhs) const {
	return Vec2(x + rhs.x, y + rhs.y);
    }

    const Vec2 operator-(const Vec2 &rhs) const {
	return Vec2(x - rhs.x, y - rhs.y);
    }

    const Vec2 operator-() const {
	return Vec2(-x, -y);
    }

    const Vec2 operator*(double c) const {
	return Vec2(*this) *= c;
    }

    const Vec2 operator/(double c) const {
	return Vec2(*this) /= c;
    }

    bool operator==(const Vec2 &other) const {
	return (this->x == other.x && this->y == other.y);
    }

    bool operator!=(const Vec2 &other) const {
	return !(*this == other);
    }     

    string str() {
	char buffer[100];
	sprintf(buffer,"(%f,%f)",x,y);
	return string(buffer);
    }

    double len2() const {
	return x * x + y * y;
    }

    double len() const {
	return sqrt(x * x + y * y);
    }

    Vec2 & normalize() {
	(*this) /= len();
	return (*this);
    }

    const Vec2 & normalized() const {
	return Vec2(*this) /= len();
    }

    void rotate(double angle) { // angle in radians, counterclockwise
	double cossine = cos(angle);
	double sine = sin(angle);

	double x = (this->x)*cossine - (this->y)*sine;
	double y = (this->x)*sine    + (this->y)*cossine;

	this->x = x;
	this->y = y;
    }   

    Vec2 perp() const {
	return Vec2(-y,x);
    }

};

#endif /* VEC2_H_ */
