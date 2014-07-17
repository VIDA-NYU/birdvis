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

#include <iostream>

#include "ViewRect.hpp"
#include "Vec2.hpp"

using namespace std;

ViewRect::ViewRect()
{
    this->minimum_w = 0.5;
    this->minimum_h = 0.5;

    this->init(-125.926587, 8.232996, 62.092132, 62.092132);
}

ViewRect::~ViewRect() {}

ViewRect::ViewRect(double x, double y, double w, double h)
{
    this->minimum_w = 0.5;
    this->minimum_h = 0.5;

    this->init(x,y,w,h);
}

void ViewRect::init(double x, double y, double w, double h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}
    
void ViewRect::scale(double s)
{
    double cx = this->x + this->w/2.0;
    double cy = this->y + this->h/2.0;
    this->w *= s;
    this->h *= s;
    this->x = cx - this->w/2.0;
    this->y = cy - this->h/2.0;
}

void ViewRect::translate(double tx, double ty)
{
    this->x += tx;
    this->y += ty;
}

string ViewRect::str()
{
    char buffer[128];
    sprintf(buffer,"ViewRect %f %f %f %f", x, y, w, h);
    return string(buffer);
}

bool ViewRect::contains(double xx, double yy)
{
    return (x <= xx && xx < x + w && y <= yy && yy < y + h); 
}

void ViewRect::resizeToIncludePoint(double xx, double yy)
{
    double dx = xx - x;
    double dy = yy - y;

    if (dx < 0) {
	x = x + dx;
	w = w - dx;
    }
    if (dx > w) {
	w = dx;
    }

    if (dy < 0) {
	y = y + dy;
	h = h - dy;
    }
    if (dy > h) {
	h = dy;
    }
}

void ViewRect::resizeToIncludeRect(const ViewRect &vr)
{
    resizeToIncludePoint( vr.x         ,     vr.y        );
    resizeToIncludePoint( vr.x         ,     vr.y + vr.h );
    resizeToIncludePoint( vr.x + vr.w  ,     vr.y + vr.h );
    resizeToIncludePoint( vr.x + vr.w  ,     vr.y        );
}

void ViewRect::resizeRelativeToCorner(ViewRect::Corner corner, double dx, double dy)
{
    // treat case where the corner passes
    
//     cout << this->str() << endl;
//     cout << "Dx = " << dx << " Dy = " << dy << endl;

    double new_x;
    double new_y;
    double new_w;
    double new_h;

    switch (corner) {
    case TOP_LEFT:
	//x += dx;
	//w -= dx;
        //h += dy;
	new_x = x + dx;
	new_y = y;
	new_w = w - dx;
	new_h = h + dy;
	break;
    case BOTTOM_LEFT:
// 	x += dx;
// 	w -= dx;
// 	y += dy;
// 	h -= dy;
	new_x = x + dx;
	new_y = y + dy;
	new_w = w - dx;
	new_h = h - dy;
	break;
    case BOTTOM_RIGHT:
// 	w += dx;
// 	y += dy;
// 	h -= dy;
	new_x = x;
	new_y = y + dy;
	new_w = w + dx;
	new_h = h - dy;
	break;
    case TOP_RIGHT:
// 	w += dx;
// 	h += dy;
	new_x = x;
	new_y = y;
	new_w = w + dx;
	new_h = h + dy;
	break;
    }

//     cout << "new_x = " << new_x << endl;
//     cout << "new_y = " << new_y << endl;
//     cout << "new_w = " << new_w << endl;
//     cout << "new_h = " << new_h << endl;

    double xx = min(new_x, new_x + new_w);
    double yy = min(new_y, new_y + new_h);
    double ww = abs(new_w);
    double hh = abs(new_h);

//     cout << "xx = " << xx << endl;
//     cout << "yy = " << yy << endl;
//     cout << "ww = " << ww << endl;
//     cout << "hh = " << hh << endl;


     if(ww < this->minimum_w)
 	ww = this->minimum_w;

     if(hh < this->minimum_h)
 	hh = this->minimum_h;

//     cout << "xx = " << xx << endl;
//     cout << "yy = " << yy << endl;
//     cout << "ww = " << ww << endl;
//     cout << "hh = " << hh << endl;


    this->x = xx;
    this->y = yy;
    this->w = ww;
    this->h = hh;

}

ViewRect::Corner ViewRect::closestCorner(double xx, double yy, double &distance)
{
    Vec2 p(xx,yy);
    Vec2 c0(x,y+h);
    Vec2 c1(x,y);
    Vec2 c2(x+w,y);
    Vec2 c3(x+w,y+h);
    
    double d0 = (c0 - p).len();
    double d1 = (c1 - p).len();
    double d2 = (c2 - p).len();
    double d3 = (c3 - p).len();

    if (d0 <= d1 && d0 <= d2 && d0 <= d3)
    {
	distance = d0;
	return ViewRect::TOP_LEFT;
    }
    else if (d1 <= d2 && d1 <= d3)
    {
	distance = d1;
	return ViewRect::BOTTOM_LEFT;
    }
    else if (d2 <= d3)
    {
	distance = d2;
	return ViewRect::BOTTOM_RIGHT;
    }
    else
    {
	distance = d3;
	return ViewRect::TOP_RIGHT;
    }
}

