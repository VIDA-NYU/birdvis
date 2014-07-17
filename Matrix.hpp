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

#ifndef MATRIX_H
#define MATRIX_H

#include <iosfwd>

#include "Vector.hpp"
#include "Rotation.hpp"

namespace linalg {

    // homogeneous transform matrix
    class mat3 {
        double m[3][3];

        template<class Archive>
        friend void serialize(Archive& ar, mat3& m, const unsigned version);
    public:
        mat3();
        mat3(double a, double b, double c,
             double d, double e, double f,
             double g, double h, double i);
#if 0
        const double* operator[](unsigned i) const;
        double* operator[](unsigned i);
#endif
        double& operator()(int i, int j);
        double operator()(int i, int j) const;

        vec3 row(int i) const;

        mat3& operator+=(const mat3& b);
        mat3& operator-=(const mat3& b);
        mat3& operator*=(double x);
        mat3& operator/=(double x);

        vec2 transformLinear(const vec2& v) const;
        vec2 transformAffine(const vec2& v) const;
        
        void makeTransform(const vec2& pos, const vec2& scale, const Radian& rot);
        vec2 getPosition() const;
        vec2 getScale() const;
        Radian getRotation() const;

        mat3 transpose() const;
        
        mat3 inverse() const;

        static const mat3 identity;
    };

    const mat3& operator+(const mat3& a);
    mat3 operator-(const mat3& a);

    mat3 operator+(const mat3& a, const mat3& b);
    mat3 operator-(const mat3& a, const mat3& b);

    mat3 operator*(const mat3& a, const mat3& b);
    mat3 operator*(double a, const mat3& b);
    mat3 operator*(const mat3& a, double b);

    vec3 operator*(const mat3& a, const vec3& b);
    
    std::ostream& operator<<(std::ostream& o, const mat3& m);
}


#endif
