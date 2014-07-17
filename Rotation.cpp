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

#include "Rotation.hpp"

namespace linalg {

    namespace {
        const double rad2deg = 180/M_PI;
        const double deg2rad = M_PI/180;
    }
    
    const Radian Radian::zero(0);
    const Degree Degree::zero(0);


    Radian::Radian() : value(0)
    {}

    Radian::Radian(const Radian& r) : value(r.value)
    {}

    Radian::Radian(double x) : value(x)
    {}
    
    Radian&
    Radian::operator=(const Radian& r)
    {
        value = r.value;
        return *this;
    }

    double
    Radian::degrees() const
    {
        return value * rad2deg;
    }

    double
    Radian::radians() const
    {
        return value;
    }

    const Radian&
    Radian::operator+() const {
        return *this;
    }

    Radian
    Radian::operator+(const Radian& r) const
    {
        return Radian(value + r.value);
    }

    Radian
    Radian::operator-() const
    {
        return Radian(-value);
    }

    Radian
    Radian::operator-(const Radian& r) const
    {
        return Radian(value - r.value);
    }

    Radian
    Radian::operator*(double x) const
    {
        return Radian(value*x);
    }

    Radian
    operator*(double x, const Radian& r)
    {
        return r * x;
    }

    Radian
    Radian::operator/(double x) const
    {
        return Radian(value / x);
    }

    bool
    Radian::operator<(const Radian& r) const
    {
        return value < r.value;
    }

    bool
    Radian::operator<=(const Radian& r) const
    {
        return value <= r.value;
    }
    
    bool
    Radian::operator>(const Radian& r) const
    {
        return value > r.value;
    }

    bool
    Radian::operator>=(const Radian& r) const
    {
        return value >= r.value;
    }
    
    bool
    Radian::operator==(const Radian& r) const
    {
        return value == r.value;
    }

    bool
    Radian::operator!=(const Radian& r) const
    {
        return value != r.value;
    }

    Radian&
    Radian::operator+=(const Radian& r)
    {
        value += r.value;
        return *this;
    }

    Radian&
    Radian::operator-=(const Radian& r)
    {
        value -= r.value;
        return *this;
    }

    Radian&
    Radian::operator*=(double x)
    {
        value *= x;
        return *this;
    }
    
    Radian&
    Radian::operator/=(const double& x)
    {
        value = x;
        return *this;
    }



    vec2
    Radian::operator*(const vec2& v) const
    {
        double theta = radians();
        const double c = std::cos(theta);
        const double s = std::sin(theta);
        return vec2(
            c*v.x - s*v.y,
            s*v.x + c*v.y
            );
    }


    Radian::Radian(const Degree& d) :
        value(d.radians())
    {}

    Radian&
    Radian::operator=(const Degree& d)
    {
        value = d.radians();
        return *this;
    }

    Radian
    Radian::operator+(const Degree& d) const
    {
        return Radian(value + d.radians());
    }

    Radian
    Radian::operator-(const Degree& d) const
    {
        return Radian(value - d.radians());
    }

    Radian& Radian::operator+=(const Degree& d)
    {
        value += d.radians();
        return *this;
    }

    Radian&
    Radian::operator-=(const Degree& d)
    {
        value -= d.radians();
        return *this;
    }




    Degree::Degree() : value(0) {}

    Degree::Degree(double x) : value(x) {}
        
    Degree&
    Degree::operator=(const Degree& d)
    {
        value = d.value;
        return *this;
    }
        
    double
    Degree::degrees() const
    {
        return value;
    }
    
    double
    Degree::radians() const
    {
        return value * deg2rad;
    }


    const Degree&
    Degree::operator+() const
    {
        return *this;
    }
    
    Degree
    Degree::operator+(const Degree& d) const
    {
        return Degree(value + d.value);
    }

    Degree
    Degree::operator-() const
    {
        return Degree(-value);
    }

    Degree
    Degree::operator-(const Degree& d) const
    {
        return Degree(value - d.value);
    }
        
    Degree
    Degree::operator*(double x) const
    {
        return Degree(value*x);
    }

    Degree
    operator*(double x, const Degree& d)
    {
        return d * x;
    }

    Degree
    Degree::operator/(double x) const
    {
        return Degree(value / x);
    }


    bool
    Degree::operator<(const Degree& d) const
    {
        return value < d.value;
    }
    
    bool
    Degree::operator<=(const Degree& d) const
    {
        return value <= d.value;
    }
    
    bool
    Degree::operator>(const Degree& d) const
    {
        return value > d.value;
    }
    
    bool
    Degree::operator>=(const Degree& d) const
    {
        return value >= d.value;
    }
    
    bool
    Degree::operator==(const Degree& d) const
    {
        return value == d.value;
    }
    
    bool
    Degree::operator!=(const Degree& d) const
    {
        return value != d.value;
    }


    Degree&
    Degree::operator+=(const Degree& d)
    {
        value += d.value;
        return *this;
    }

    Degree&
    Degree::operator-=(const Degree& d)
    {
        value -= d.value;
        return *this;
    }

    Degree&
    Degree::operator*=(double x)
    {
        value *= x;
        return *this;
    }

    Degree&
    Degree::operator/=(const double& x)
    {
        value = x; return *this;
    }


    
    vec2
    Degree::operator*(const vec2& v) const
    {
        double theta = radians();
        const double c = std::cos(theta);
        const double s = std::sin(theta);
        return vec2(
            c*v.x - s*v.y,
            s*v.x + c*v.y
            );
    }


    Degree::Degree(const Radian& r) : value(r.degrees())
    { }


    Degree&
    Degree::operator=(const Radian& r)
    {
        value = r.degrees();
        return *this;
    }

    Degree
    Degree::operator+(const Radian& r) const
    {
        return Degree(value + r.degrees());
    }

    Degree
    Degree::operator-(const Radian& r) const
    {
        return Degree(value - r.degrees());
    }

    Degree&
    Degree::operator+=(const Radian& r)
    {
        value += r.degrees();
        return *this;
    }

    Degree&
    Degree::operator-=(const Radian& r)
    {
        value -= r.degrees();
        return *this;
    }



    std::ostream&
    operator<<(std::ostream& o, const Radian& r)
    {
        return o << r.radians() << " rad";
    }
    

    std::ostream&
    operator<<(std::ostream& o, const Degree& d)
    {
        return o << d.degrees() << " °";
    }


}

