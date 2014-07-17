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

#ifndef __TEXTURE__
#define __TEXTURE__

#include <GL/glew.h>
#include <iostream>

using namespace std;

class Texture{
private:
    GLuint   texture_id;
    unsigned width;
    unsigned height;
    unsigned pitch;
    unsigned bitsPerPixel;
    unsigned bytesPerPixel;
    bool     loaded;

public:
    Texture();
    //Texture(unsigned w, unsigned h, unsigned bpp);
    //Texture(string filename);
    ~Texture();

    unsigned getWidth();
    unsigned getHeight();
    unsigned getBitsPerPixel();
    unsigned getBytesPerPixel();
    
    bool isLoaded();
    void load(string filename);

    void     setLoaded(bool l);
    void bind();

    GLuint getId();
    void   release();
    string str();

};

#endif
