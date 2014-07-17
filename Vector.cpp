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
#include <ostream>

#include "Vector.hpp"

namespace linalg {

    const vec2 vec2::zero(0,0);
    const vec2 vec2::one(1,1);
    const vec3 vec3::zero(0,0,0);


    vec2::vec2() : x(0), y(0)
    {}

    vec2::vec2(double a, double b) : x(a), y(b)
    {}

    vec2::vec2(const vec2& v) : x(v.x), y(v.y)
    {}

    vec2::vec2(const vec3& v) : x(v.x), y(v.y)
    {}
    
    double
    vec2::length() const
    {
        return std::sqrt(x*x + y*y);
    }
    
    double
    vec2::squaredLength() const
    {
        return x*x + y*y;
    }
    
    vec2&
    vec2::normalise()
    {
        double len = length();
        x /= len;
        y /= len;
        return *this;
    }
    

    vec2&
    vec2::operator+=(const vec2& b)
    {
        x += b.x;
        y += b.y;
        return *this;
    }
    
    vec2&
    vec2::operator-=(const vec2& b)
    {
        x -= b.x;
        y -= b.y;
        return *this;
    }

    vec2&
    vec2::operator*=(double b)
    {
        x *= b;
        y *= b;
        return *this;
    }

    vec2&
    vec2::operator*=(const vec2& b)
    {
        x *= b.x;
        y *= b.y;
        return *this;
    }

    vec2&
    vec2::operator/=(double b)
    {
        x /= b;
        y /= b;
        return *this;
    }


    const vec2&
    vec2::operator+() const
    {
        return *this;
    }

    vec2
    vec2::operator-() const
    {
        return vec2(-x, -y);
    }

    vec2
    vec2::operator+(const vec2& b) const
    {
        return vec2(x+b.x, y+b.y);
    }
    
    vec2
    vec2::operator-(const vec2& b) const
    {
        return vec2(x-b.x, y-b.y);
    }
    
    vec2
    vec2::operator*(double b) const
    {
        return vec2(x*b, y*b);
    }
    
    vec2
    operator*(double a, const vec2& b)
    {
        return vec2(a*b.x, a*b.y);
    }
    
    vec2
    vec2::operator*(const vec2& b) const
    {
        return vec2(x*b.x, y*b.y);
    }

    vec2
    vec2::operator/(double b) const
    {
        return vec2(x/b, y/b);
    }


    bool
    vec2::operator==(const vec2& b) const
    {
        return x == b.x && y == b.y;
    }

    bool
    vec2::operator!=(const vec2& b) const
    {
        return x != b.x || y != b.y;
    }


    double
    dot(const vec2& a, const vec2& b)
    {
        return a.x*b.x + a.y*b.y;
    }




    std::ostream& operator<<(std::ostream& o, const vec2& v)
    {
        return o << "(" << v.x << " , " << v.y << ")";
    }



    vec3::vec3() : x(0), y(0), z(0)
    {}

    vec3::vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_)
    {}

    vec3::vec3(const vec2& a, double z_) : x(a.x), y(a.y), z(z_)
    {}
    
    
    double
    vec3::length() const
    {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    double
    vec3::squaredLength() const
    {
        return x*x + y*y + z*z;
    }
    
    vec3&
    vec3::normalise()
    {
        double len = length();
        x /= len;
        y /= len;
        z /= len;
        return *this;
    }
    

    vec3&
    vec3::operator+=(const vec3& b)
    {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }
    
    vec3&
    vec3::operator-=(const vec3& b)
    {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        return *this;
    }

    vec3&
    vec3::operator*=(double b)
    {
        x *= b;
        y *= b;
        z *= b;
        return *this;
    }

    vec3&
    vec3::operator*=(const vec3& b)
    {
        x *= b.x;
        y *= b.y;
        z *= b.z;
        return *this;
    }

    vec3&
    vec3::operator/=(double b)
    {
        x /= b;
        y /= b;
        z /= b;
        return *this;
    }


    const vec3&
    vec3::operator+() const
    {
        return *this;
    }
    
    vec3
    vec3::operator-() const
    {
        return vec3(-x, -y, -z);
    }

    vec3
    vec3::operator+(const vec3& b) const
    {
        return vec3(x+b.x, y+b.y, z+b.z);
    }
    
    vec3
    vec3::operator-(const vec3& b) const
    {
        return vec3(x-b.x, y-b.y, z-b.z);
    }
    
    vec3
    vec3::operator*(double b) const
    {
        return vec3(x*b, y*b, z*b);
    }
    
    vec3
    operator*(double a, const vec3& b)
    {
        return vec3(a*b.x, a*b.y, a*b.z);
    }
    
    vec3
    vec3::operator*(const vec3& b) const
    {
        return vec3(x*b.x, y*b.y, z*b.z);
    }

    vec3
    vec3::operator/(double b) const
    {
        return vec3(x/b, y/b, z/b);
    }



    double
    dot(const vec3& a, const vec3& b)
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }


    vec3
    cross(const vec3& a, const vec3& b)
    {
        return vec3(a.y*b.z - a.z*b.y,
                    a.z*b.x - a.x*b.z,
                    a.x*b.y - a.y*b.x);
    }
    

    std::ostream& operator<<(std::ostream& o, const vec3& v)
    {
        return o << "(" << v.x << " , " << v.y << " , " << v.z << ")";
    }
}

