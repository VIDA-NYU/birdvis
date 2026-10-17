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

#ifndef VIEWRECT_HPP
#define VIEWRECT_HPP

#include <string>
#include <cstdio>

using namespace std;

class ViewRect
{
private:
    float minimum_w;
    float minimum_h;

public:

    enum Corner {TOP_LEFT, BOTTOM_LEFT, BOTTOM_RIGHT, TOP_RIGHT};
    
public:
    double x, y, w, h;
    ViewRect();
    ViewRect(double x, double y, double w, double h);
    ~ViewRect();
    void init(double x, double y, double w, double h);
    void scale(double s);
    void translate(double tx, double ty);
    string str();
    bool contains(double xx, double yy);
    void resizeToIncludePoint(double xx, double yy);
    void resizeToIncludeRect(const ViewRect &vr);
    double maxY() { return y + h; }
    double minY() { return y; }
    double maxX() { return x + w; }
    double minX() { return x; }
    Corner closestCorner(double xx, double yy, double &distance);
    void resizeRelativeToCorner(Corner corner, double dx, double dy);
};

#endif
