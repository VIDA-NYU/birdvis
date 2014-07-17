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
#include <iostream>
#include <sstream>
#include <cstdlib>

#include "RenderSystem.hpp"
//#include "TextureManager.hpp"

#define DRAW_TEXT
#define DEBUG_RENDER_SYSTEM

namespace rendersystem {

    /****************
     * RenderSystem *
     ****************/

    RenderSystem::FontTexture::FontTexture(
	Font *font, 
	GLuint texture_id, 
	bool nearest, 
	bool mipmap)
    {
	this->font       = font;
	this->texture_id = texture_id;
	this->nearest    = nearest;
	this->mipmap     = mipmap;
    }

    void
    RenderSystem::FontTexture::startTexture()
    {
	glBindTexture(GL_TEXTURE_2D, this->texture_id);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (nearest && !mipmap)
	{
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else if (nearest && mipmap)
	{
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if (!nearest && mipmap)
	{
	    // cout << "case !nearest && mipmap" << endl;
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if (!nearest && !mipmap)
	{
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glEnable(GL_TEXTURE_2D);

    }

    void
    RenderSystem::FontTexture::endTexture()
    {
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
    }


    int
    RenderSystem::FontTexture::pt_size()
    {
	return font->pt_size;
    }


    RenderSystem::RenderSystem(string path)
    {
#ifdef DEBUG_RENDER_SYSTEM
	cout << "RenderSystem::RenderSystem(" << path << ")" << endl;
#endif

	// initialize 
	for (int i=0;i<256;i++)
	    this->font_textures[i] = NULL;

	{ // load mimapped default font
	    int i = 200;

	    stringstream description_filename, texture_filename;
	    description_filename << path << "fonts/arial_" << i << ".description";
	    texture_filename     << path << "fonts/arial_" << i << ".sgi";

	    Font *font                 = Font_load(description_filename.str().c_str(),
						   texture_filename.str().c_str());
	    GLuint texture_id          = this->makeTexture(font->texture,4);
	    bool nearest = false;
	    bool mipmap  = true;
	    this->font_textures[i]     = new FontTexture(font, texture_id, nearest, mipmap);
	    this->default_font_texture = font_textures[i];

#ifdef DEBUG_RENDER_SYSTEM
	    cout << "   font size " << i << " texture id " << texture_id << endl;
#endif
	} // load mipmapped default font

	for (int i=7;i<20;i++) 
	{
	    stringstream description_filename, texture_filename;
	    description_filename << path << "fonts/arial_" << i << ".description";
	    texture_filename     << path << "fonts/arial_" << i << ".sgi";

	    Font *font              = Font_load(description_filename.str().c_str(),
						texture_filename.str().c_str());
	    GLuint texture_id       = this->makeTexture(font->texture,0);

	    bool nearest = true;
	    bool mipmap  = false;
	    this->font_textures[i]  = new FontTexture(font, texture_id);

#ifdef DEBUG_RENDER_SYSTEM
	    cout << "   font size " << i << " texture id " << texture_id << endl;
#endif
	}

	// current scale 
	current_scale.push(1.0);

	// defaults
	glEnable(GL_MULTISAMPLE);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0,0,0,1);
	// glEnable(GL_LIGHTING);
	// glEnable(GL_LIGHT0);
	// glEnable(GL_MULTISAMPLE);

    } // end RenderSystem

    RenderSystem::FontTexture* 
    RenderSystem::figureOutBestFontTexture(float target_pt_size)
    {
	float target_screen_size = target_pt_size * current_scale.top();
	int font_size = (int) ceil(target_screen_size);
	float error = fabs(target_screen_size - font_size);

	FontTexture *font_texture = NULL;
	if (font_size < 256 && font_size >=0 && error < 1.e-6) {
	    //cout << "trying font " << font_size  << endl;
	    //cout << "error       " << (target_screen_size - font_size) << endl;
	    font_texture = font_textures[font_size];
	}

	if (font_texture == NULL)
	    font_texture = this->default_font_texture;
//     else
// 	cout << "... found match at size " << font_size << endl;


	return font_texture;
    }

    void 
    RenderSystem::pushMatrix()
    {
	// we assume we are in modelview
	glPushMatrix();
	current_scale.push(current_scale.top());
    }

    void 
    RenderSystem::translate(float x, float y)
    {
 	glTranslatef(x,y,0);
    }
    
    void 
    RenderSystem::rotate(float degrees)
    {
 	glRotatef(degrees,0,0,1);
    }

    void 
    RenderSystem::scale(float sx, float sy)
    {
	glScalef(sx,sy,1.0);
	current_scale.top() *= sy;
    }

    void 
    RenderSystem::popMatrix()
    {
	glPopMatrix();
	current_scale.pop();
    }

    void 
    RenderSystem::drawHandle(
	double x, double y, double w, double h, 
	double base = 0.7)
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


	glBegin(GL_LINE_LOOP);//begin drawing of polygon
    
	glColor3f(0,0,0);
	glVertex2f(x0,y2);
	glVertex2f(x0,y1);
	glVertex2f(x1,y0);
	glVertex2f(x2,y1);
	glVertex2f(x2,y2);
    
	glEnd();//end drawing of polygon


    }

    void 
    RenderSystem::startViewport(float x, float y, float w, float h,
				bool scissor, bool blend)
    {
	glViewport(x,y,w,h);
    
	//
	if (scissor)
	{
	    glScissor((int)x,(int)y,(int)w,(int)h);
	    glEnable(GL_SCISSOR_TEST);		
	}
    
	// prepare right aspect ratio
    
	// update projection to long,lat range
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(x,x+w,y,y+h,-10, 10);
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (blend)
	    glEnable(GL_BLEND);

	this->scissor = scissor;
	this->blend   = blend;

    }

    void 
    RenderSystem::startViewport(ViewRect screen_window_rect,
				ViewRect world_window_rect,
				ViewRect scissor_window_rect,
				bool blend)
    {

	glViewport(screen_window_rect.x,
		   screen_window_rect.y,
		   screen_window_rect.w,
		   screen_window_rect.h);
    
	//
	glScissor((int)scissor_window_rect.x,
		  (int)scissor_window_rect.y,
		  (int)scissor_window_rect.w,
		  (int)scissor_window_rect.h);
	glEnable(GL_SCISSOR_TEST);		
    
	// prepare right aspect ratio
    
	// update projection to long,lat range
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(world_window_rect.x,
		world_window_rect.x + world_window_rect.w,
		world_window_rect.y,
		world_window_rect.y + world_window_rect.h,
		-10, 10);
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (blend)
	    glEnable(GL_BLEND);

	this->scissor = true;
	this->blend   = blend;

    }


    void 
    RenderSystem::startViewport(ViewRect rect, bool scissor, bool blend)
    {
	this->startViewport(rect.x, rect.y, 
			    rect.w, rect.h, 
			    scissor, blend);
    }

    void 
    RenderSystem::startViewport(ViewRect screen_window_rect,
				ViewRect world_window_rect,
				bool scissor, bool blend)
    {
	this->startViewport(screen_window_rect.x, 
			    screen_window_rect.y, 
			    screen_window_rect.w, 
			    screen_window_rect.h,
			    world_window_rect.x, 
			    world_window_rect.y, 
			    world_window_rect.w, 
			    world_window_rect.h, 
			    scissor, blend);
    }

    void 
    RenderSystem::startViewport(
	float sw_x, float sw_y, float sw_w, float sw_h,
	float ww_x, float ww_y, float ww_w, float ww_h,
	bool scissor, bool blend)
    {

	glViewport(sw_x,sw_y,sw_w,sw_h);
    
	//
	if (scissor)
	{
	    glScissor((int)sw_x,(int)sw_y,(int)sw_w,(int)sw_h);
	    glEnable(GL_SCISSOR_TEST);		
	}
    
	// prepare right aspect ratio
    
	// update projection to long,lat range
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(ww_x,ww_x+ww_w,ww_y,ww_y+ww_h,-10, 10);
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (blend)
	    glEnable(GL_BLEND);

	this->scissor = scissor;
	this->blend   = blend;

    }

    void 
    RenderSystem::endViewport()
    {
	if (blend)
	    glDisable(GL_BLEND);

	//
	if (scissor)
	{
	    glDisable(GL_SCISSOR_TEST);		
	}
    } // end RenderSystem::endViewport

    void 
    RenderSystem::drawLineStrip(int num_points, float *points)
    {
	glBegin(GL_LINE_STRIP);
	for(int i = 0 ; i < num_points ; i++){
	    glVertex2f(points[2*i],points[2*i+1]);
	}
	glEnd();
    } // end RenderSystem::drawLineStrip

    void 
    RenderSystem::drawQuad(float x, float y, float w, float h, GLuint type)
    {
	// assuming the GL_MODELVIEW is on
	glBegin(type);
	glVertex2f(x,y);
	glVertex2f(x+w,y);
	glVertex2f(x+w,y+h);
	glVertex2f(x,y+h);
	glEnd();
    } // end RenderSystem::drawQuad

    void 
    RenderSystem::drawQuadWithTexture(float x, float y, float w, float h, GLuint texture_id){
	//cout << "drawQuadWithTexture() texture_id = " << texture_id << endl;
	glBindTexture(GL_TEXTURE_2D,texture_id);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(x,y);
	glTexCoord2f(1,0);
	glVertex2f(x+w,y);
	glTexCoord2f(1,1);
	glVertex2f(x+w,y+h);
	glTexCoord2f(0,1);
	glVertex2f(x,y+h);
	glEnd();	
	glDisable(GL_TEXTURE_2D);
    }


    void 
    RenderSystem::drawQuad(ViewRect rect, GLuint type)
    {
	// assuming the GL_MODELVIEW is on
	glBegin(type);
	glVertex2f(rect.x,rect.y);
	glVertex2f(rect.x+rect.w,rect.y);
	glVertex2f(rect.x+rect.w,rect.y+rect.h);
	glVertex2f(rect.x,rect.y+rect.h);
	glEnd();
    } // end RenderSystem::drawQuad

    void 
    RenderSystem::drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, GLuint type)
    {
	// assuming the GL_MODELVIEW is on
	glBegin(type);
	glVertex2f(x0,y0);
	glVertex2f(x1,y1);
	glVertex2f(x2,y2);
	glEnd();
    } // end RenderSystem::drawQuad

    void 
    RenderSystem::drawQuadWithColors(float x, float y, float w, float h, 
				     Color &c0, Color &c1, Color &c2, Color &c3, 
				     GLuint type)
    {
	// assuming the GL_MODELVIEW is on
	glBegin(type);
	setColor(c0);
	glVertex2f(x,y);
	setColor(c1);
	glVertex2f(x+w,y);
	setColor(c2);
	glVertex2f(x+w,y+h);
	setColor(c3);
	glVertex2f(x,y+h);
	glEnd();
    } // end RenderSystem::drawQuad

    void 
    RenderSystem::setColor(const Color &color)
    {
	glColor4f(color.r,color.g,color.b,color.a);
    }

    void 
    RenderSystem::setColor(const Color *color)
    {
	glColor4f(color->r,color->g,color->b,color->a);
    }


    void 
    RenderSystem::drawSegment(float x0, float y0, float x1, float y1)
    {
	// assuming the GL_MODELVIEW is on
	glBegin(GL_LINES);
	glVertex2f(x0,y0);
	glVertex2f(x1,y1);
	glEnd();
    } // end RenderSystem::drawQuad


    void 
    RenderSystem::setColor(float r, float g, float b, float a)
    {
	glColor4f(r,g,b,a);
    } // end RenderSystem::drawQuad


    ViewRect 
    RenderSystem::textBoundingBox(const char *text, float text_size, 
				  float text_x0, float text_y0, float *advance)
    {
#ifdef DRAW_TEXT    
	FontTextIterator *fti = 
	    Font_text_iterator(this->default_font_texture->font,
			       text,
			       text_size,
			       text_x0, text_y0);
	float x,y,w,h;
	FontTextIterator_bb(fti,&x,&y,&w,&h);
	ViewRect result(x,y,w,h);
	if (advance != NULL)
	    *advance = fti->x;
	free(fti);
	return result;
#else
	ViewRect result(0,0,1,1);
	return result;
#endif
    }



    GLuint 
    RenderSystem::makeTexture(const Image *img, int mip_map_level)
    {

	GLuint texture;

	// creat an rgba texture for the single byte texture
	int i;
	int w = img->width;
	int h = img->height;
	byte *rgba = (byte *) malloc(sizeof(byte) * 4 * w * h);
	for (i=0;i<w*h;i++)
	{
	    byte value = img->buffer[i];
	    rgba[4*i] = 255;
	    rgba[4*i+1] = 255;
	    rgba[4*i+2] = 255;
	    rgba[4*i+3] = value;
	}
    
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
    
	if (mip_map_level > 0)
	{
	    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
	    // GL_LINEAR_MIPMAP_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
    
	/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); */
	/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);


	if (mip_map_level > 0)
	{
	    gluBuild2DMipmaps(
		GL_TEXTURE_2D, 4,            /* target, level */
		w, h, GL_RGBA, 
		GL_UNSIGNED_BYTE,   /* external format, type */
		rgba                       /* pixels */
		);
	}
	else {
	    glTexImage2D(
		GL_TEXTURE_2D, 0,            /* target, level */
		GL_RGBA8,                    /* internal format */
		w, h, 0,                     /* width, height, border */
		GL_RGBA,                     /* external format */
		GL_UNSIGNED_BYTE,            /* type */
		rgba                         /* pixels */
		);
	}
    
	return texture;

    } // end GLWidget::makeTexture


    void 
    RenderSystem::drawText(const char *text, 
			   float text_size,
			   float text_x0, 
			   float text_y0)
    {

#ifdef DRAW_TEXT

	FontTexture *font_texture = this->figureOutBestFontTexture(text_size);
	//cout << "using texture "  << font_texture->pt_size() << endl;


	// write something
	// float x_t,y_t,w_t,h_t;
	float x_u,y_u,w_u,h_u;
	float x_tx, y_tx, w_tx, h_tx;
    
	GLfloat text_pos[] = 
	    {  0,  0, 
	       1,  0, 
	       1,  -1, 
	       0,  -1 };

// std::array<GLfloat, 16> projection;
// std::array<GLfloat, 16> modelview;
// std::array<GLfloat, 3>  screen_coords;

// glGetFloatv(GL_PROJECTION_MATRIX, projection.data());
// glGetFloatv(GL_MODELVIEW_MATRIX, modelview.data());

// gluProject(world_coords[0], world_coords[1], world_coords[2],
//     modelview.data(), projection.data(),
//     screen_coords.data(), screen_coords.data() + 1, screen_coords.data() + 2);    


	{ // pass with blurred text

	    // ASSUMING:
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	    //
	    FontTextIterator *fti =
		Font_text_iterator(font_texture->font,
				   text,
				   text_size,
				   text_x0,
				   text_y0);

	    // while (FontTextIterator_next_tight(fti, &x_t,&y_t,&w_t,&h_t))
	    // {
	    // 	glColor3f(1,1,0);
	    // 	draw_quad(x_t,y_t,w_t,h_t,GL_LINE_LOOP);
	    // }
	    // FontTextIterator_reset(fti);

	    // begin texture
	    font_texture->startTexture();
	    
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
		    
		     // printf("xx: %10.4f  yy: %10.4f  tx: %10.4f  ty: %10.4f\n",
		     //        xx,yy,tx,ty);

		    glTexCoord2f(tx,ty);
		    glVertex2f(xx,yy);

		    // cout << "vertex " << xx << ", " << yy << endl;
		}
		glEnd();

	    }


	    // end texture
	    font_texture->endTexture();

	    FontTextIterator_free(fti);

	    glDisable(GL_BLEND);

	} // end character iterator

#endif

    } // end RenderSystem::drawText


    void 
    RenderSystem::drawRightAlignedText(
	const char *text, 
	float  text_height, 
	float x, float y,
	float w, float margin)
    {

#ifdef DRAW_TEXT

	FontTexture *font_texture = this->figureOutBestFontTexture(text_height);

	// write something
	float x_u,y_u,w_u,h_u;
	float x_tx, y_tx, w_tx, h_tx;
    
	GLfloat text_pos[] = 
	    {  0,  0, 
	       1,  0, 
	       1,  -1, 
	       0,  -1 };

	{ 
	    // ASSUMING:
	    // glEnable(GL_BLEND);
	    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	    // glDisable(GL_BLEND);

	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	    //
	    FontTextIterator *fti =
		Font_text_iterator(font_texture->font,
				   text,
				   text_height,
				   0, 0);

	    float xc, yc, wc, hc;
	    FontTextIterator_bb(fti,&xc,&yc,&wc,&hc);

	    float tx = x + w - wc - margin;
	    float ty = y; // + (h - text_height)/2.0 - yc;

	    tx = floor(tx);
	    ty = floor(ty);

	    // cout << "xc,yc,wc,hc" << xc << " " << yc << " " << wc << " " << hc << endl;

	    FontTextIterator_set_text_position(fti,tx,ty);



	    // begin texture
	    font_texture->startTexture();
	
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
	
	    // end texture
	    font_texture->endTexture();

	    FontTextIterator_free(fti);
	}

#endif

    }

    void 
    RenderSystem::drawCenteredText(const char *text, 
				   float  text_height, 
				   float x, float y,
				   float w, float h)
    {

#ifdef DRAW_TEXT

	FontTexture *font_texture = this->figureOutBestFontTexture(text_height);

	// write something
	float x_u,y_u,w_u,h_u;
	float x_tx, y_tx, w_tx, h_tx;
	    
	GLfloat text_pos[] = 
	    {  0,  0, 
	       1,  0, 
	       1,  -1, 
	       0,  -1 };
	    
	{ // pass with blurred text
		
	    //
	    FontTextIterator *fti =
		Font_text_iterator(font_texture->font,
				   text,
				   text_height,
				   0, 0);
		
	    float xc, yc, wc, hc;
	    FontTextIterator_bb(fti,&xc,&yc,&wc,&hc);
		
	    hc = (int)(text_height * 0.75);

	    float tx = x + (w - wc)/2.0;
	    float ty = y + (h - hc)/2.0;

	    tx = floor(tx);
	    ty = floor(ty);
		
	    // cout << "xc,yc,wc,hc" << xc << " " << yc << " " << wc << " " << hc << endl;
		
	    FontTextIterator_set_text_position(fti,tx,ty);
		
	    // begin texture
	    font_texture->startTexture();
		
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
		
	    // end texture
	    font_texture->endTexture();
		
	    FontTextIterator_free(fti);
	}

#endif

    }

    void 
    RenderSystem::drawHorizontallyCenteredText(
	const char *text, 
	float  text_height, 
	float cx, float base_line_y)
    {

#ifdef DRAW_TEXT

	FontTexture *font_texture = this->figureOutBestFontTexture(text_height);

	// write something
	float x_u,y_u,w_u,h_u;
	float x_tx, y_tx, w_tx, h_tx;
	    
	GLfloat text_pos[] = 
	    {  0,  0, 
	       1,  0, 
	       1,  -1, 
	       0,  -1 };
	    
	{ // pass with blurred text
		
	    //
	    FontTextIterator *fti =
		Font_text_iterator(font_texture->font,
				   text,
				   text_height,
				   0, 0);
		
	    float xc, yc, wc, hc;
	    FontTextIterator_bb(fti,&xc,&yc,&wc,&hc);
		
	    float tx = cx - wc/2.0;
	    float ty = base_line_y;
		
	    tx = floor(tx);
	    ty = floor(ty);

	    // cout << "xc,yc,wc,hc" << xc << " " << yc << " " << wc << " " << hc << endl;
		
	    FontTextIterator_set_text_position(fti,tx,ty);
		
	    // begin texture
	    font_texture->startTexture();

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

	    // end texture
	    font_texture->endTexture();
		
	    FontTextIterator_free(fti);
	}

#endif
    }

//     GLuint 
//     RenderSystem::allocateTextureId(){
// 	GLuint texture_id;
// 	glGenTextures(1, &texture_id);
// 	return texture_id;
//     }

//     void 
//     RenderSystem::bindTexture(GLuint texture_id){
// 	TextureManager* tex_man = TextureManager::Inst(); 
// 	bool succeded = tex_man->BindTexture(texture_id);

// 	//check error
//     }

    Texture*
    RenderSystem::createTextureFromImage(string file, GLuint texture_id){
	return NULL;
    }
// #ifdef DEBUG_RENDER_SYSTEM
// 	cout << "RenderSystem::createTextureFromImage(string file, GLuint texture_id)" << endl;
// #endif

// 	if(file.compare("") != 0){
// 	    //check if the file exists in the images directory
	    
// 	    // load image
// 	    const unsigned int texID = texture_id;

// 	    TextureManager* tex_man = TextureManager::Inst(); 
// 	    bool succeded = tex_man->LoadTexture(file.c_str(), texID);
// 	    if(!succeded){
// #ifdef DEBUG_RENDER_SYSTEM	
// 	cout << "   failed to load the image" << endl;
// #endif
// 	    }
// 	    else{
// #ifdef DEBUG_RENDER_SYSTEM
// 		cout << "   image loaded with success" << endl;
// #endif
// 	    }
// 	}

//     }

//     void 
//     RenderSystem::releaseTextureId(GLuint texture_id){
// 	glDeleteTextures(1, &texture_id);
//     }

} // end namespace block: rendersystem
