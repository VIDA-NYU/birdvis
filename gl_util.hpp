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

#ifndef _GLUTIL_H
#define _GLUTIL_H

#define xDEBUG_GL_UTIL

#include <QtOpenGL>

#include "font.h"

namespace gl_util { // gl_util namespace

    void draw_arrow(double x, double y, double w, double h, double base = 0.7);

    void draw_circle(double cx, double cy, double radius);

    void draw_centered_text(const char *text, 
			    Font  *font_desc, 
			    GLuint font_tex_id, 
			    float  text_height, 
			    float x, float y,
			    float w, float h);


    void draw_horizontally_centered_text(const char *text, 
					 Font  *font_desc, 
					 GLuint font_tex_id, 
					 float  text_height, 
					 float cx, float base_line_y);

    void draw_quad(double x, double y, double w, double h, GLuint type=GL_QUADS);

    void draw_text(const char *text, 
		   Font  *font_desc, 
		   GLuint font_tex_id, 
		   float  text_height, 
		   float text_x0, 
		   float text_y0);

    void draw_right_aligned_text(const char *text, 
				 Font  *font_desc, 
				 GLuint font_tex_id, 
				 float  text_height, 
				 float x, float y,
				 float w, float margin);

} // end gl_util namespace

#endif
