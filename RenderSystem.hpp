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

#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include <stack>

#include "Color.hpp"
#include "ViewRect.hpp"

#include "image.h"
#include "font.h"

#include "Texture.hpp"

using namespace std;

namespace rendersystem {

    class RenderSystem 
    {

    public:
	class FontTexture 
	{
	public:
	    Font  *font;
	    GLuint texture_id; // texture id
	    bool   mipmap;
	    bool   nearest;    
	public:
	    FontTexture(Font *font, GLuint texture_id, bool nearest=false, bool mipmap=false);
	    int pt_size();
	    void startTexture();
	    void endTexture();
	};
    private:

	FontTexture*  font_textures[256];
	FontTexture*  default_font_texture;

	bool          blend;
	bool          scissor;

	stack<float>  current_scale;

    private:
    
	FontTexture *figureOutBestFontTexture(float target_pt_size);

    public:

	RenderSystem(string path);

	void drawText(const char *text, float text_size, 
		      float text_x0, float text_y0);

	void drawRightAlignedText(const char *text, 
				  float text_height, 
				  float x, float y,
				  float w, float margin);

	void drawHorizontallyCenteredText(const char *text, 
					  float  text_height, 
					  float cx, float base_line_y);

	void drawCenteredText(const char *text, 
			      float  text_height, 
			      float x, float y,
			      float w, float h);

	void drawHandle(double x, double y, double w, double h, double base);

	ViewRect textBoundingBox(const char *text, float text_size, 
				 float text_x0, float text_y0,
				 float *advance = NULL);

	void drawLineStrip(int num_points, float *points);

	void drawQuad(float x, float y, float w, float h, GLuint type=GL_QUADS);

	void drawQuadWithTexture(float x, float y, float w, float h, GLuint texture_id);

	void drawQuad(ViewRect rect, GLuint type);

	void drawTriangle(float x0, float y0, 
			  float x1, float y1, 
			  float x2, float y2, 
			  GLuint type=GL_TRIANGLES);

	void drawQuadWithColors(float x, float y, float w, float h, 
				Color &c0, Color &c1, Color &c2, Color &c3, 
				GLuint type=GL_QUADS);

	void drawSegment(float x0, float y0, float x1, float y1);

	void setColor(float r, float g, float b, float a=1.0);

	void setColor(const Color &color);

	void setColor(const Color *color);

	void startViewport(float x, float y, float w, float h,
			   bool scissor=true, bool blend=true);

	void startViewport(float sw_x, float sw_y, float sw_w, float sw_h,
			   float ww_x, float ww_y, float ww_w, float ww_h,
			   bool scissor=true, bool blend=true);

	void startViewport(ViewRect rect, bool scissor=true, bool blend=true);

	void startViewport(ViewRect screen_window_rect,
			   ViewRect world_window_rect,
			   bool scissor=true, bool blend=true);

	void startViewport(ViewRect screen_window_rect,
			   ViewRect world_window_rect,
			   ViewRect scissor_window_rect,
			   bool blend=true);

	void pushMatrix();

	void translate(float x, float y);

	void rotate(float degrees);

	void scale(float sx, float sy);

	void popMatrix();

	void endViewport();

	// GLuint allocateTextureId();
// 	void bindTexture(GLuint texture_id);
 	Texture* createTextureFromImage(string file, GLuint texture_id);
// 	void releaseTextureId(GLuint texture_id);


    private:

	GLuint makeTexture(const Image *img, int mip_map_level);

    };

}

#endif
































// 	//
// 	glEnable(GL_MULTISAMPLE);

// 	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
// 	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

// 	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	    
// 	// glEnable(GL_LIGHTING);
// 	// glEnable(GL_LIGHT0);
// 	// glEnable(GL_MULTISAMPLE);

// 	// initialize font
// 	string font_description_filename = get_resource("arial.description");
// 	string font_texture_filename     = get_resource("arial.sgi");
// 	font_description = Font_load(font_description_filename.c_str(),
// 				 font_texture_filename.c_str());
// 	font_texture_id = make_texture(font_description->texture,5);
	
// 	//init help texture
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 	face_tex = load_texture((char*)get_resource("help2.rgb").c_str());
// 	//printf("help texure: %d %d\n",face_tex->width,face_tex->height);
	
// 	glBindTexture(GL_TEXTURE_2D, face_tex->name);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// 	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
// 	for(int i = 0 ; i < face_tex->height ; i++){
// 	    for(int j = 0 ; j < face_tex->width ; j++){
// 		w[i][j][0] = face_tex->data[i][j][0];
// 		w[i][j][1] = face_tex->data[i][j][1];
// 		w[i][j][2] = face_tex->data[i][j][2];
// 		w[i][j][3] = face_tex->data[i][j][3];
// 	    }
// 	}
	
// 	free(face_tex->data);
	
// 	gluBuild2DMipmaps(
// 	    GL_TEXTURE_2D, 4,            /* target, level */
// 	    face_tex->width, 
// 	    face_tex->height, 
// 	    GL_RGBA, 
// 	    GL_UNSIGNED_BYTE,            /* external format, type */
// 	    w                            /* pixels */
// 	    );
// 	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, face_tex->width,
// 	// 		 face_tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,w);    
	
	
// 	//initialize interpolation texture
// 	glGenTextures(1,&interpolation_tex);
	
// 	glBindTexture(GL_TEXTURE_2D, interpolation_tex);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	
// 	// font = Font_load(font_description, font_texture);
// 	// string font_description("../font/font.description");
// 	// string font_texture("../font/font.sgi");
// 	// const char *a = font_description.c_str();
// 	// const char *b = font_texture.c_str();
	
// 	// font = Font_load(font_description.c_str(),font_texture.c_str());
// 	// blur_font_description = Font_load(font_description_filename.c_str(),
// 	// 				 font_texture_filename.c_str());
    
// 	// // add texture
// 	// blur_font_texture_id = make_texture(blur_font_description->texture,4);

// 	//triangulate us map
// 	triangulateMap();
// 	map_triangulation_disp_list = glGenLists(1);
// 	glNewList(map_triangulation_disp_list, GL_COMPILE);    
// 	int tcount = map_triangulation.size()/3;
// 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// 	glColorMask(0,0,0,0);
// 	glBegin(GL_TRIANGLES);
// 	for (int k=0;k<tcount;k++)
// 	{
// 	    const Vector2d &p1 = map_triangulation[k*3+0];
// 	    const Vector2d &p2 = map_triangulation[k*3+1];
// 	    const Vector2d &p3 = map_triangulation[k*3+2];
		
// 	    glVertex2f(p1.GetX(),p1.GetY());
// 	    glVertex2f(p2.GetX(),p2.GetY());
// 	    glVertex2f(p3.GetX(),p3.GetY());
// 	}

// 	glEnd();
	
// 	glColorMask(1,1,1,1);
// 	glEndList();


// 	//read locations file
// 	loc = new Locations(get_resource("common.locs.csv"));

// 	if(true){
// 	    //generate display to draw locations
// 	    int numLocations = loc->getNumLocations();
// 	    GLuint disp_list = glGenLists(1);
// 	    locations_display_lists.push_back(disp_list);

// 	    glNewList(disp_list, GL_COMPILE);
// 	    glPointSize(2.0);
// 	    glColor3f(1.0,1.0,1.0);
// 	    glBegin(GL_POINTS);
// 	    for(int j = 0 ; j < numLocations; j++){
// 		double lat;
// 		double lon;
// 		loc->getLocation(j,&lat,&lon);
// 		//draw locations
// 		glVertex2f(lon,lat);
// 	    }
// 	    glEnd();
// 	    glEndList();

// 	    //Generate a displaylist, for the maps,

// 	    {//generate us map displaylist
// 		RegionSet *region_set = region_sets[0];


// 		int numLocations = loc->getNumLocations();
// 		us_map_disp_list = glGenLists(1);

// 		glNewList(us_map_disp_list, GL_COMPILE);

// 		glLineWidth(1.0);
// 		glColor3f(1.0,1.0,1.0);

// 		for (int i=0;i<region_set->regions.size();i++)
// 		{
// 		    string name = region_set->names.at(i);
// 		    size_t found = name.find("Alaska");
// 		    if (found == string::npos) {
// 			found = name.find("Hawaii");
// 			if (found == string::npos) {
// 			    OGRGeometry *g = region_set->regions.at(i);
// 			    gl_draw_geometry(g);
// 			}
// 		    }
// 		}
	    
// 		glEndList();
// 	    }//generate us map playlist


// 	    {//generate counties map displaylist
// 		RegionSet *region_set = region_sets[1];


// 		int numLocations = loc->getNumLocations();
// 		county_map_disp_list = glGenLists(1);

// 		glNewList(county_map_disp_list, GL_COMPILE);
// 		glColor3f(1.0,1.0,1.0);
// 		glLineWidth(1.0);

// 		for (int i=0;i<region_set->regions.size();i++)
// 		{
// 		    string name = region_set->names.at(i);
// 		    size_t found = name.find("Alaska");
// 		    if (found == string::npos) {
// 			found = name.find("Hawaii");
// 			if (found == string::npos) {
// 			    OGRGeometry *g = region_set->regions.at(i);
// 			    gl_draw_geometry(g);
// 			}
// 		    }
// 		}
	    
// 		glEndList();

// 	    }//generate counties map playlist
// 	} 

// 	//
// 	this->spreadsheet.wire();


//     } // Canvas::initializeGL




// /////////////////////////////////////////////////////////////////////////////////////////////
// // OpenGL auxiliar stuff
// /////////////////////////////////////////////////////////////////////////////////////////////





//     void draw_centered_text(const char *text, 
// 			    float text_height, 
// 			    float x, float y, 
// 			    float w, float h);

//     void draw_horizontally_centered_text(const char *text, 
// 					 Font  *font_desc, 
// 					 GLuint font_tex_id, 
// 					 float  text_height, 
// 					 float cx, float base_line_y)
//     {
// 	// write something
// 	float x_u,y_u,w_u,h_u;
// 	float x_tx, y_tx, w_tx, h_tx;
	    
// 	GLfloat text_pos[] = 
// 	    {  0,  0, 
// 	       1,  0, 
// 	       1,  -1, 
// 	       0,  -1 };
	    
// 	{ // pass with blurred text
		
// 	    // ASSUMING:
// 	    // glEnable(GL_BLEND);
// 	    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
// 	    // glDisable(GL_BLEND);
		
// 	    glBindTexture(GL_TEXTURE_2D, font_tex_id);
// 	    glEnable(GL_TEXTURE_2D);
		
// 	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 	    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
// 	    //
// 	    FontTextIterator *fti =
// 		Font_text_iterator(font_desc,
// 				   text,
// 				   text_height,
// 				   0, 0);
		
// 	    float xc, yc, wc, hc;
// 	    FontTextIterator_bb(fti,&xc,&yc,&wc,&hc);
		
// 	    float tx = cx - wc/2.0;
// 	    float ty = base_line_y;
		
// 	    // cout << "xc,yc,wc,hc" << xc << " " << yc << " " << wc << " " << hc << endl;
		
// 	    FontTextIterator_set_text_position(fti,tx,ty);
		
// 	    glBegin(GL_QUADS);
// 	    while (FontTextIterator_next(fti, &x_u,&y_u,&w_u,&h_u,
// 					 &x_tx, &y_tx, &w_tx, &h_tx))
// 	    {
// 		// add a quad on the buffer with texture coords!
// 		GLuint quad_ids[4];
// 		for (int i=0;i<4;i++)
// 		{
			
// 		    float xx = x_u + text_pos[2*i] * w_u;
// 		    float yy = y_u + text_pos[2*i+1] * h_u;
// 		    // Transform_point(current_transform,xx,yy,&xx,&yy);
			
// 		    float tx = x_tx + text_pos[2*i] * w_tx; // texture x
// 		    float ty = y_tx + text_pos[2*i+1] * h_tx; // texture x = no texture
			
// 		    // printf("xx: %10.4f  yy: %10.4f  tx: %10.4f  ty: %10.4f\n",xx,yy,tx,ty);
// 		    glTexCoord2f(tx,ty);
// 		    glVertex2f(xx,yy);
// 		}
// 	    }
// 	    glEnd();
		
// 	    glBindTexture(GL_TEXTURE_2D, 0);	    
// 	    glDisable(GL_TEXTURE_2D);
		
// 	    FontTextIterator_free(fti);
// 	}
//     }

//     void draw_quad(double x, double y, double w, double h, GLuint type=GL_QUADS)
//     {
// 	glBegin(type);
// 	glVertex2f(x,y);
// 	glVertex2f(x+w,y);
// 	glVertex2f(x+w,y+h);
// 	glVertex2f(x,y+h);
// 	glEnd();
//     }
