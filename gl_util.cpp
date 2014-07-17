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

#include "gl_util.hpp"

namespace gl_util { // gl_util namespace

    void draw_arrow(double x, double y, double w, double h, double base)
    {
	double y0 = y;
	double y1 = y + h*base;
	double y2 = y + h;

	double x0 = x;
	double x1 = x + w/2.0;
	double x2 = x + w;

	glBegin(GL_POLYGON);//begin drawing of polygon
    
	glColor3f(0.5,0.5,0.5);
	glVertex2f(x0,y2);

	glColor3f(0.5,0.5,0.5);
	glVertex2f(x0,y1);

	glColor3f(0.7,0.7,0.7);
	glVertex2f(x1,y0);

	glColor3f(1,1,1);
	glVertex2f(x2,y1);

	glColor3f(1,1,1);
	glVertex2f(x2,y2);

	glEnd();//end drawing of polygon
    }

    void draw_circle(double cx, double cy, double radius)
    {
	int steps = 128;
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0,0,0);
	glVertex2f(cx,cy);
	glColor3f(0.8,0.8,0.8);
	for (int i=0;i<=steps;i++)
	{
	    double theta = ((1.0 * i) / steps) * M_PI * 2;
	    double cos_theta = cos(theta);
	    double sin_theta = sin(theta);
	    double x = cx + radius * cos_theta;
	    double y = cy + radius * sin_theta;
	    glVertex2f(x,y);
	}
	glEnd();//end drawing of polygon
    }

    void draw_centered_text(const char *text, 
			    Font  *font_desc, 
			    GLuint font_tex_id, 
			    float  text_height, 
			    float x, float y,
			    float w, float h)
    {
	// write something
	float x_u,y_u,w_u,h_u;
	float x_tx, y_tx, w_tx, h_tx;
	    
	GLfloat text_pos[] = 
	    {  0,  0, 
	       1,  0, 
	       1,  -1, 
	       0,  -1 };
	    
	{ // pass with blurred text
		
	    // ASSUMING:
	    // glEnable(GL_BLEND);
	    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	    // glDisable(GL_BLEND);
		
	    glBindTexture(GL_TEXTURE_2D, font_tex_id);
	    glEnable(GL_TEXTURE_2D);
		
	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
	    //
	    FontTextIterator *fti =
		Font_text_iterator(font_desc,
				   text,
				   text_height,
				   0, 0);
		
	    float xc, yc, wc, hc;
	    FontTextIterator_bb(fti,&xc,&yc,&wc,&hc);
		
	    float tx = x + (w - wc)/2.0 - xc;
	    float ty = y + (h - text_height)/2.0 - yc;
		
	    // cout << "xc,yc,wc,hc" << xc << " " << yc << " " << wc << " " << hc << endl;
		
	    FontTextIterator_set_text_position(fti,tx,ty);
		
	    glBegin(GL_QUADS);
	    while (FontTextIterator_next(fti, &x_u,&y_u,&w_u,&h_u,
					 &x_tx, &y_tx, &w_tx, &h_tx))
	    {
		// add a quad on the buffer with texture coords!
		GLuint quad_ids[4];
		for (int i=0;i<4;i++)
		{
			
		    float xx = x_u + text_pos[2*i] * w_u;
		    float yy = y_u + text_pos[2*i+1] * h_u;
		    // Transform_point(current_transform,xx,yy,&xx,&yy);
			
		    float tx = x_tx + text_pos[2*i] * w_tx; // texture x
		    float ty = y_tx + text_pos[2*i+1] * h_tx; // texture x = no texture
			
		    // printf("xx: %10.4f  yy: %10.4f  tx: %10.4f  ty: %10.4f\n",xx,yy,tx,ty);
		    glTexCoord2f(tx,ty);
		    glVertex2f(xx,yy);
		}
	    }
	    glEnd();
		
	    glBindTexture(GL_TEXTURE_2D, 0);	    
	    glDisable(GL_TEXTURE_2D);
		
	    FontTextIterator_free(fti);
	}
    }


    void draw_horizontally_centered_text(const char *text, 
					 Font  *font_desc, 
					 GLuint font_tex_id, 
					 float  text_height, 
					 float cx, float base_line_y)
    {
	// write something
	float x_u,y_u,w_u,h_u;
	float x_tx, y_tx, w_tx, h_tx;
	    
	GLfloat text_pos[] = 
	    {  0,  0, 
	       1,  0, 
	       1,  -1, 
	       0,  -1 };
	    
	{ // pass with blurred text
		
	    // ASSUMING:
	    // glEnable(GL_BLEND);
	    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	    // glDisable(GL_BLEND);
		
	    glBindTexture(GL_TEXTURE_2D, font_tex_id);
	    glEnable(GL_TEXTURE_2D);
		
	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
	    //
	    FontTextIterator *fti =
		Font_text_iterator(font_desc,
				   text,
				   text_height,
				   0, 0);
		
	    float xc, yc, wc, hc;
	    FontTextIterator_bb(fti,&xc,&yc,&wc,&hc);
		
	    float tx = cx - wc/2.0;
	    float ty = base_line_y;
		
	    // cout << "xc,yc,wc,hc" << xc << " " << yc << " " << wc << " " << hc << endl;
		
	    FontTextIterator_set_text_position(fti,tx,ty);
		
	    glBegin(GL_QUADS);
	    while (FontTextIterator_next(fti, &x_u,&y_u,&w_u,&h_u,
					 &x_tx, &y_tx, &w_tx, &h_tx))
	    {
		// add a quad on the buffer with texture coords!
		GLuint quad_ids[4];
		for (int i=0;i<4;i++)
		{
			
		    float xx = x_u + text_pos[2*i] * w_u;
		    float yy = y_u + text_pos[2*i+1] * h_u;
		    // Transform_point(current_transform,xx,yy,&xx,&yy);
			
		    float tx = x_tx + text_pos[2*i] * w_tx; // texture x
		    float ty = y_tx + text_pos[2*i+1] * h_tx; // texture x = no texture
			
		    // printf("xx: %10.4f  yy: %10.4f  tx: %10.4f  ty: %10.4f\n",xx,yy,tx,ty);
		    glTexCoord2f(tx,ty);
		    glVertex2f(xx,yy);
		}
	    }
	    glEnd();
		
	    glBindTexture(GL_TEXTURE_2D, 0);	    
	    glDisable(GL_TEXTURE_2D);
		
	    FontTextIterator_free(fti);
	}
    }

    void draw_quad(double x, double y, double w, double h, GLuint type)
    {
	glBegin(type);
	glVertex2f(x,y);
	glVertex2f(x+w,y);
	glVertex2f(x+w,y+h);
	glVertex2f(x,y+h);
	glEnd();
    }

    void draw_text(const char *text, 
		   Font  *font_desc, 
		   GLuint font_tex_id, 
		   float  text_height, 
		   float text_x0, 
		   float text_y0)
    {
	// write something
	float x_t,y_t,w_t,h_t;
	float x_u,y_u,w_u,h_u;
	float x_tx, y_tx, w_tx, h_tx;
    
	GLfloat text_pos[] = 
	    {  0,  0, 
	       1,  0, 
	       1,  -1, 
	       0,  -1 };

	{ // pass with blurred text

	    // ASSUMING:
	    // glEnable(GL_BLEND);
	    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	
	    //
	    FontTextIterator *fti =
		Font_text_iterator(font_desc,
				   text,
				   text_height,
				   text_x0,
				   text_y0);

	    // while (FontTextIterator_next_tight(fti, &x_t,&y_t,&w_t,&h_t))
	    // {
	    // 	glColor3f(1,1,0);
	    // 	draw_quad(x_t,y_t,w_t,h_t,GL_LINE_LOOP);
	    // }
	    // FontTextIterator_reset(fti);

	    glBindTexture(GL_TEXTURE_2D, font_tex_id);
	    glEnable(GL_TEXTURE_2D);
	    
	    while (FontTextIterator_next(fti, &x_u,&y_u,&w_u,&h_u,
					 &x_tx, &y_tx, &w_tx, &h_tx))
	    {
		// glColor3f(0,0,1);
		// draw_quad(x_u,y_u,w_u,-h_u,GL_LINE_LOOP);
		
		
 		// add a quad on the buffer with texture coords!
		// glColor3f(1,1,1);
		
		glBegin(GL_QUADS);
		for (int i=0;i<4;i++)
		{
		    
		    float xx = x_u + text_pos[2*i] * w_u;
		    float yy = y_u + text_pos[2*i+1] * h_u;
		    // Transform_point(current_transform,xx,yy,&xx,&yy);
		    
		    float tx = x_tx + text_pos[2*i] * w_tx; // texture x
		    float ty = y_tx + text_pos[2*i+1] * h_tx; // texture x = no texture
		    
#ifdef DEBUG_GL_UTIL
		    printf("xx: %10.4f  yy: %10.4f  tx: %10.4f  ty: %10.4f\n",xx,yy,tx,ty);
#endif
		    glTexCoord2f(tx,ty);
		    glVertex2f(xx,yy);
 		}
		glEnd();

	    }


	    glBindTexture(GL_TEXTURE_2D, 0);	    
	    glDisable(GL_TEXTURE_2D);


	    FontTextIterator_free(fti);

	    // glDisable(GL_BLEND);
	}
    }

    void draw_right_aligned_text(const char *text, 
				 Font  *font_desc, 
				 GLuint font_tex_id, 
				 float  text_height, 
				 float x, float y,
				 float w, float margin)
    {
	// write something
	float x_u,y_u,w_u,h_u;
	float x_tx, y_tx, w_tx, h_tx;
    
	GLfloat text_pos[] = 
	    {  0,  0, 
	       1,  0, 
	       1,  -1, 
	       0,  -1 };

	{ // pass with blurred text

	    // ASSUMING:
	    // glEnable(GL_BLEND);
	    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	    // glDisable(GL_BLEND);

	    glBindTexture(GL_TEXTURE_2D, font_tex_id);
	    glEnable(GL_TEXTURE_2D);
	
	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	    //
	    FontTextIterator *fti =
		Font_text_iterator(font_desc,
				   text,
				   text_height,
				   0, 0);

	    float xc, yc, wc, hc;
	    FontTextIterator_bb(fti,&xc,&yc,&wc,&hc);

	    float tx = x + w - wc - margin;
	    float ty = y; // + (h - text_height)/2.0 - yc;

	    // cout << "xc,yc,wc,hc" << xc << " " << yc << " " << wc << " " << hc << endl;

	    FontTextIterator_set_text_position(fti,tx,ty);

	    glBegin(GL_QUADS);
	    while (FontTextIterator_next(fti, &x_u,&y_u,&w_u,&h_u,
					 &x_tx, &y_tx, &w_tx, &h_tx))
	    {
		// add a quad on the buffer with texture coords!
		GLuint quad_ids[4];
		for (int i=0;i<4;i++)
		{
		
		    float xx = x_u + text_pos[2*i] * w_u;
		    float yy = y_u + text_pos[2*i+1] * h_u;
		    // Transform_point(current_transform,xx,yy,&xx,&yy);

		    float tx = x_tx + text_pos[2*i] * w_tx; // texture x
		    float ty = y_tx + text_pos[2*i+1] * h_tx; // texture x = no texture

		    // printf("xx: %10.4f  yy: %10.4f  tx: %10.4f  ty: %10.4f\n",xx,yy,tx,ty);
		    glTexCoord2f(tx,ty);
		    glVertex2f(xx,yy);
		}
	    }
	    glEnd();
	
	    glBindTexture(GL_TEXTURE_2D, 0);	    
	    glDisable(GL_TEXTURE_2D);

	    FontTextIterator_free(fti);
	}
    }

} // end gl_util namespace
