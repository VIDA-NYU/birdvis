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

#ifndef ROTATION_H
#define ROTATION_H

#include <iosfwd>

#include "Vector.hpp"

namespace linalg {

    class vec2;

    class Degree;

    class Radian {
        double value;

        template<class Archive>
        friend void serialize(Archive& ar, Radian& r, const unsigned version);
        
    public:

        Radian();
        Radian(const Radian& r);
        explicit Radian(double x);
        
        Radian& operator=(const Radian& r);
        
        double degrees() const;
        double radians() const;

        const Radian& operator+() const;
        Radian operator+(const Radian& r) const;

        Radian operator-() const;
        Radian operator-(const Radian& r) const;

        Radian operator*(double x) const;
        Radian operator/(double x) const;

        bool operator<(const Radian& r) const;
        bool operator<=(const Radian& r) const;
        bool operator>(const Radian& r) const;
        bool operator>=(const Radian& r) const;
        bool operator==(const Radian& r) const;
        bool operator!=(const Radian& r) const;

        Radian& operator+=(const Radian& r);
        Radian& operator-=(const Radian& r);
        Radian& operator*=(double x);
        Radian& operator/=(const double& x);

        vec2 operator*(const vec2& v) const;

        Radian(const Degree& d);
        Radian& operator=(const Degree& d);
        Radian operator+(const Degree& d) const;
        Radian operator-(const Degree& d) const;
        Radian& operator+=(const Degree& d);
        Radian& operator-=(const Degree& d);



        static const Radian zero;
    };


    class Degree {
        double value;

        template<class Archive>
        friend void serialize(Archive& ar, Degree& d, const unsigned version);
    public:

        Degree();

        explicit Degree(double x);
        
        Degree& operator=(const Degree& d);
        
        double degrees() const;
        double radians() const;


        const Degree& operator+() const;
        Degree operator+(const Degree& d) const;

        Degree operator-() const;
        Degree operator-(const Degree& d) const;
        
        Degree operator*(double x) const;

        Degree operator/(double x) const;


        bool operator<(const Degree& d) const;
        bool operator<=(const Degree& d) const;
        bool operator>(const Degree& d) const;
        bool operator>=(const Degree& d) const;
        bool operator==(const Degree& d) const;
        bool operator!=(const Degree& d) const;


        Degree& operator+=(const Degree& d);
        Degree& operator-=(const Degree& d);
        Degree& operator*=(double x);
        Degree& operator/=(const double& x);

        vec2 operator*(const vec2& v) const;

        Degree(const Radian& d);

        Degree& operator=(const Radian& r);
        Degree operator+(const Radian& r) const;
        Degree operator-(const Radian& r) const;
        Degree& operator+=(const Radian& r);
        Degree& operator-=(const Radian& r);



        static const Degree zero;
    };

    Radian operator*(double x, const Radian& r);
    Degree operator*(double x, const Degree& d);



    std::ostream& operator<<(std::ostream& o, const Radian& r);
    std::ostream& operator<<(std::ostream& o, const Degree& d);

}



#endif
