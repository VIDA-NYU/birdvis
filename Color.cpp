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

#include <GL/glew.h>
#include "Color.hpp"
#include <cstdio>

/*********
 * Color *
 *********/

Color::Color() { 
    r = g = b = 0.0; 
    a = 1.0; 
}

Color::Color(float r, float g, float b, float a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void 
Color::init(float r, float g, float b, float a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

Color 
Color::mix(float c, Color &other) {
    Color result;
    result.r = r * c + (1.0-c) * other.r;
    result.g = g * c + (1.0-c) * other.g;
    result.b = b * c + (1.0-c) * other.b;
    result.a = a * c + (1.0-c) * other.a;
    return result;
}

void 
Color::call_glClearColor()
{
    glClearColor(r,g,b,a);
}

void 
Color::call_glColor(float transparency)
{
    glColor4f(r,g,b,a * transparency);
}

string 
Color::str()
{
    char buffer[64];
    sprintf(buffer,"r: %.2f g: %.2f b: %.2f a: %.2f\n",r,g,b,a);
    string result(buffer);
    return result;
}

