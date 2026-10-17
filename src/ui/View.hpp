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

#ifndef VIEW_H_
#define VIEW_H_

#include <iostream>
using namespace std;

class View {
protected:
    int x; //lower left corner in screen space
    int y; //lower left corner in screen space
    int width;
    int height;
    float backgroundColor[4];

public:

    View();
    View(int x, int y, int width, int height);
    int  getX();
    void setX(int newX);
    int  getY();
    void setY(int newY);
    int  getWidth();
    void setWidth(int newWidth);
    int  getHeight();
    void setHeight(int newHeight);
    void  getBackgroundColor(float *r,float *g,
	                     float *b,float *a);
    void setBackgroundColor(float r,float g,
	                    float b,float a);
    
    bool contains(int x, int y);

    bool isInResize(int x, int y);
    void translate(int dx, int dy);
    void resize(int dx, int dy);
    virtual void paint();
};


#endif /* VIEW_H_ */
