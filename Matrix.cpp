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

#include <cmath>
#include <algorithm>
#include <ostream>

#include "Matrix.hpp"

namespace linalg {

    const mat3 mat3::identity(1, 0, 0,
                              0, 1, 0,
                              0, 0, 1);

    mat3::mat3() {}
    
    mat3::mat3(double a, double b, double c,
               double d, double e, double f,
               double g, double h, double i)
    {
        m[0][0] = a;
        m[0][1] = b;
        m[0][2] = c;
        m[1][0] = d;
        m[1][1] = e;
        m[1][2] = f;
        m[2][0] = g;
        m[2][1] = h;
        m[2][2] = i;
    }
    
    double
    mat3::operator()(int i, int j) const
    {
        return m[i][j];
    }
    
    double&
    mat3::operator()(int i, int j)
    {
        return m[i][j];
    }
    
    vec3
    mat3::row(int i) const
    {
        return vec3(m[i][0], m[i][1], m[i][2]);
    }
    

    mat3&
    mat3::operator+=(const mat3& b)
    {
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                m[i][j] += b.m[i][j];
        return *this;
    }

    mat3&
    mat3::operator-=(const mat3& b)
    {
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                m[i][j] -= b.m[i][j];
        return *this;
    }

    mat3&
    mat3::operator*=(double x)
    {
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                m[i][j] *= x;
        return *this;
    }
    
    mat3&
    mat3::operator/=(double x)
    {
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                m[i][j] /= x;
        return *this;
    }

    vec2
    mat3::transformLinear(const vec2& v) const
    {
        return vec2(
            m[0][0]*v.x + m[0][1]*v.y,
            m[1][0]*v.x + m[1][1]*v.y
            );
    }

    vec2
    mat3::transformAffine(const vec2& v) const
    {
        return vec2(
            m[0][0]*v.x + m[0][1]*v.y + m[0][2],
            m[1][0]*v.x + m[1][1]*v.y + m[1][2]
            );
    }
 


    void
    mat3::makeTransform(const vec2& pos, const vec2& scale, const Radian& rot)
    {
        if (pos == vec2::zero && scale == vec2(1,1) && rot == Radian::zero) {
            *this = identity;
            return;
        }
        
        m[0][0] =  std::cos(rot.radians()) * scale.x;
        m[0][1] = -std::sin(rot.radians()) * scale.y;
        m[0][2] =  pos.x;
        
        m[1][0] =  std::sin(rot.radians()) * scale.x;
        m[1][1] =  std::cos(rot.radians()) * scale.y;
        m[1][2] =  pos.y;

        m[2][0] = 0;
        m[2][1] = 0;
        m[2][2] = 1;
    }
    
    vec2
    mat3::getPosition() const
    {return vec2(m[0][2], m[1][2]);}

    vec2
    mat3::getScale() const
    {
        return vec2(
            vec2(m[0][0], m[1][0]).length(),
            vec2(m[0][1], m[1][1]).length()
            );
    }

    Radian
    mat3::getRotation() const
    {return Radian(std::atan2(m[1][0], m[0][0]));}


    mat3
    mat3::transpose() const
    {
        mat3 t;
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                t(j,i) = m[i][j];
        return t;
    }


    mat3
    mat3::inverse() const
    {
        // Invert a 3x3 using cofactors.  This is about 8 times faster than
        // the Numerical Recipes code which uses Gaussian elimination.
        mat3 im;
        im.m[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];
        im.m[0][1] = m[0][2]*m[2][1] - m[0][1]*m[2][2];
        im.m[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];
        im.m[1][0] = m[1][2]*m[2][0] - m[1][0]*m[2][2];
        im.m[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];
        im.m[1][2] = m[0][2]*m[1][0] - m[0][0]*m[1][2];
        im.m[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0];
        im.m[2][1] = m[0][1]*m[2][0] - m[0][0]*m[2][1];
        im.m[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0];

        double det =
            m[0][0]*im.m[0][0] +
            m[0][1]*im.m[1][0] +
            m[0][2]*im.m[2][0];

        double invDet = 1.0/det;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                im.m[i][j] *= invDet;
        
        return im;
    }
    



    const mat3&
    operator+(const mat3& a)
    {
        return a;
    }
    
    mat3
    operator-(const mat3& a)
    {
        return mat3(
            -a(0,0), -a(0,1), -a(0,2),
            -a(1,0), -a(1,1), -a(1,2),
            -a(2,0), -a(2,1), -a(2,2)
            );
    }

    mat3
    operator+(const mat3& a, const mat3& b)
    {
        mat3 c;
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                c(i,j) = a(i,j) + b(i,j);
        return c;
    }

    mat3
    operator-(const mat3& a, const mat3& b)
    {
        mat3 c;
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                c(i,j) = a(i,j) - b(i,j);
        return c;
    }
   

    mat3
    operator*(const mat3& a, const mat3& b)
    {
        mat3 c;
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j) {
                double r = 0;
                for (int k=0; k<3; ++k)
                    r += a(i, k) * b(k, j);
                c(i,j) = r;
            }
        return c;
    }
    
    mat3 operator*(double a, const mat3& b)
    {
        mat3 c;
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                c(i,j) = a * b(i,j);
        return c;
    }
    
    mat3 operator*(const mat3& a, double b)
    {
        mat3 c;
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                c(i,j) = a(i,j) * b;
        return c;
    }



    vec3
    operator*(const mat3& a, const vec3& b)
    {
        return vec3(
            a(0,0) * b.x + a(0,1) * b.y + a(0,2) * b.z,
            a(1,0) * b.x + a(1,1) * b.y + a(1,2) * b.z,
            a(2,0) * b.x + a(2,1) * b.y + a(2,2) * b.z
            );
    }



    std::ostream& operator<<(std::ostream& o, const mat3& m)
    {
        o << "[";
        const char *rsep = "";
        for (int i=0; i<3; ++i, rsep = " ") {
            o << rsep << "[";
            const char *sep = "";
            for (int j=0; j<3; ++j, sep=" , ")
                o << sep << m(i, j);
            o << "]";
        }
        return o << "]";
    }
}

