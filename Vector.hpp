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

#ifndef VECTOR_H
#define VECTOR_H

#include <iosfwd>

namespace linalg {
 
    class vec3;

    struct vec2 {
        double x, y;

        vec2();
        vec2(double x_, double y_);
        vec2(const vec2&);
        explicit vec2(const vec3&);
        
        double length() const;
        double squaredLength() const;
        
        vec2& normalise();

        const vec2& operator+() const;
        vec2 operator-() const;

        vec2 operator+(const vec2& b) const;
        vec2 operator-(const vec2& b) const;

        vec2 operator*(double b) const;
        vec2 operator*(const vec2& b) const; // component-wise multiplication

        vec2 operator/(double b) const;

        vec2& operator+=(const vec2& b);
        vec2& operator-=(const vec2& b);
        vec2& operator*=(double b);
        vec2& operator*=(const vec2& b); // component-wise multiplication
        vec2& operator/=(double b);

        bool operator==(const vec2& b) const;
        bool operator!=(const vec2& b) const;

        static const vec2 zero;
        static const vec2 one;
    };

    vec2 operator*(double a, const vec2& b);
    
   
    double dot(const vec2& a, const vec2& b);



    std::ostream& operator<<(std::ostream& o, const vec2& v);




    struct vec3 {
        double x, y, z;
        
        vec3();
        vec3(double x_, double y_, double z_);
        vec3(const vec2& a, double z_);
        
        double length() const;
        double squaredLength() const;
        
        vec3& normalise();

        const vec3& operator+() const;
        vec3 operator-() const;

        vec3 operator+(const vec3& b) const;
        vec3 operator-(const vec3& b) const;

        vec3 operator*(double b) const;
        vec3 operator*(const vec3& b) const; // component-wise multiplication
        vec3 operator/(double b) const;
        
        vec3& operator+=(const vec3& b);
        vec3& operator-=(const vec3& b);
        vec3& operator*=(double b);
        vec3& operator*=(const vec3& b); // component-wise multiplication
        vec3& operator/=(double b);

        static const vec3 zero;
    };


    vec3 operator*(double a, const vec3& b);

   
    double dot(const vec3& a, const vec3& b);

    vec3 cross(const vec3& a, const vec3& b);


    std::ostream& operator<<(std::ostream& o, const vec3& v);
}

#endif
