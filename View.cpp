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

#include "View.hpp"

View::View(){
}

View::View(int x, int y, int width, int height){
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->setBackgroundColor(0.05,0.05,0.05,0.93);
}

int View::getX(){
    return this->x;
}

void View::setX(int newX){
    this->x = newX;
}

int View::getY(){
    return this->y;
}

void View::setY(int newY){
    this->y = newY;
}

int View::getWidth(){
    return this->width;
}

void View::setWidth(int newWidth){
    this->width = newWidth;
}

int View::getHeight(){
    return this->height;
}

void View::setHeight(int newHeight){
    this->height = newHeight;
}

bool View::contains(int x, int y){
    bool horizontal = false;
    bool vertical   = false;

    horizontal = (this->x <= x) && (x <= (this->x + this->width));
    vertical =   (this->y <= y) && (y <= (this->y + this->height));

    // cout << "horizontal = " << horizontal << endl;
    // cout << "vertical = " << vertical << endl;

    return horizontal && vertical;
}

bool View::isInResize(int x, int y){
    int pi1;
    int pi2;
    int pi3;
    
    int vec1[2];
    vec1[0] = x - (this->x + this->width - 10);
    vec1[1] = y - (this->y+this->height);

    int vec2[2];
    vec2[0] = x - (this->x + this->width);
    vec2[1] = y - (this->y+this->height);

    int vec3[2];
    vec3[0] = x - (this->x + this->width);
    vec3[1] = y - (this->y+this->height - 10);

    pi1 = vec1[0]*vec2[0] + vec1[1]*vec2[1] ;
    pi2 = vec1[0]*vec3[0] + vec1[1]*vec3[1] ;
    pi3 = vec3[0]*vec2[0] + vec3[1]*vec2[1] ;

    // cout << "pi1 = " << pi1 << endl;
    // cout << "pi2 = " << pi2 << endl;
    // cout << "pi3 = " << pi3 << endl;

    int score = 0;

    if(pi1 < 0) score--;
    if(pi2 < 0) score--;
    if(pi3 < 0) score--;

    cout << "Result = " << (score <=-2) << endl;

    return score <=-2;
}



void View::getBackgroundColor(float *r,float *g,
			 float *b,float *a){
    (*r) = this->backgroundColor[0];
    (*g) = this->backgroundColor[1];
    (*b) = this->backgroundColor[2];
    (*a) = this->backgroundColor[3];
}

void View::setBackgroundColor(float r,float g,
			float b,float a){
    this->backgroundColor[0] = r;
    this->backgroundColor[1] = g;
    this->backgroundColor[2] = b;
    this->backgroundColor[3] = a;
}

void View::translate(int dx, int dy){
    this->x += dx;
    this->y += dy;
}

void View::resize(int dx, int dy){
    if(this->width + dx <= 50) this->width = 50;
    else this->width += dx;
    if(this->height + dy <= 50) this->height = 50;
    else this->height += dy;
}

void View::paint() { };
