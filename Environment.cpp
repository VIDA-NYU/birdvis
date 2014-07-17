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

#include "Environment.hpp"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <fstream>

#include <boost/any.hpp>

#include <QtGui>

#include <Util.hpp>

using namespace dataflow;
using namespace std;

#define xDEBUG_FILEMODULE
#define xDEBUG_SCALARFIELDDATA
#define xDEBUG_SCALARFIELDDISPLAYLIST
#define xDEBUG_SCREENWINDOWMODULE
#define xDEBUG_INTMODULE
#define xDEBUG_STRINGMODULE
#define xDEBUG_WORLDWINDOWMODULE
#define xDEBUG_ZOOMANDPANCONTROLLER
#define xDEBUG_SCREENWORLDTRANSFORM
#define xDEBUG_MAPBORDERSRENDERER
#define xDEBUG_SCALARFIELDRENDERER
#define xDEBUG_MAPLABELS
#define xDEBUG_TITLERENDERER
#define xDEBUG_OCCURRENCEENVIRONMENT
	
#define xDEBUG_MAPCELLINFRASTRUCTURE
#define xDEBUG_SELECTIONCONTROLLER
#define xDEBUG_MAPSELECTIONSRENDER
#define xDEBUG_CURSORRENDERER
#define xDEBUG_PREDICTORTOOLBAR

#define xBGCOLOR_PAPER_IMAGES

#define xDEBUG_PHOTORENDERER

/**************
 * FileModule *
 **************/

FileModule::FileModule():
    Module("FileModule"),
    file(0)
{
#ifdef DEBUG_FILEMODULE
    cout << "FileModule::FileModule()" << endl;
#endif

    this->addOutputPort("file");
    this->setFile(NULL);
}

void 
FileModule::process()
{}

bvdata::File*  
FileModule::getFile()
{
    return file;
}

void 
FileModule::setFile(bvdata::File *f)
{
    file = f;
    getOutputPort("file")->setData
	(new data::Single<bvdata::File *>(f),true);
}

bool
FileModule::fileLoaded(){
    return (this->file != NULL);
}


/*******************
 * ScalarFieldData *
 *******************/

ScalarFieldData::ScalarFieldData():
    Module("ScalarFieldData"),
    time_offset(0)
{
#ifdef DEBUG_SCALARFIELDDATA
    cout << "ScalarFieldData::ScalarFieldData()" << endl;
#endif
    this->addInputPort("time_index");
    this->addInputPort("expression");
    //this->addInputPort("file_shortname");
    //this->addInputPort("variable_internal_name");
    this->addInputPort("file");
    this->addOutputPort("grid");
}

void 
ScalarFieldData::process()
{

#ifdef DEBUG_SCALARFIELDDATA
    cout << "ScalarFieldData::process()" << endl;
#endif

    using namespace bvdata;

    //expression
    string expression;
    if (!data::SingleString::getInputValue(this,"expression",expression))
    {
	cout << "   could not retrieve expression" << endl;
	return;
    }

#ifdef DEBUG_SCALARFIELDDATA
    cout << "   retrieved expression " << expression << endl;
#endif

    // time_index
    int time_index;
    if (!data::SingleInt::getInputValue(this,"time_index",time_index))
    {
	cout << "   could not retrieve time_index" << endl;
	return;
    }

#ifdef DEBUG_SCALARFIELDDATA
    cout << "   time index " << time_index << endl;
#endif

    typedef  data::Single<bvdata::File *> SingleFilePtr;

    File* f;
    if (!SingleFilePtr::getInputValue(this,"file",f))
    {
	cout << "   could not retrieve file" << endl;
	return;
    }

    // now we are able to process??
    Repository *repo = Global::instance()->getRepository();

    Grid *grid = NULL;
    
    if(expression.compare("map") == 0)
	grid = repo->get2DGrid(f, "p", time_index + 1);
    else if(expression.compare("variation") == 0)
	grid = repo->get2DGridVariation(f, "p", time_index + 1);

    if (grid == NULL)
    {
	this->getOutputPort("grid")->setData(NULL, true);
	cout << "   problem evaluating matrix." << endl;
	return;
    }

    this->getOutputPort("grid")->setData(new data::SingleGridPtr(grid), true);

#ifdef DEBUG_SCALARFIELDDATA
    cout << "   finished ScalarFieldData process" << time_index << endl;
#endif

}


/**************************
 * ScalarFieldDisplayList *
 **************************/

ScalarFieldDisplayList::ScalarFieldDisplayList():
    Module("ScalarFieldDisplayList"),
    id_list(0),
    allocated(false)
{
    this->addInputPort("grid");
    this->addInputPort("colormap");
    this->addInputPort("interpolation_mode");

    this->addOutputPort("display_list");
}

void 
ScalarFieldDisplayList::process()
{
#ifdef DEBUG_SCALARFIELDDISPLAYLIST
    cout << "ScalarFieldDisplayList::process()" << endl;
#endif

    datatypes::Grid *grid;
    if (!data::SingleGridPtr::getInputValue(this,"grid",&grid))
	return;

#ifdef DEBUG_SCALARFIELDDISPLAYLIST
    cout << "   grid.......OK" << endl;
    cout << "   grid pointer " << grid  << endl;
#endif

    Colormap1d *colormap;
    if (!data::SingleColormap1dPtr::getInputValue(this,"colormap",colormap))
	return;

#ifdef DEBUG_SCALARFIELDDISPLAYLIST
    cout << "   colormap.......OK" << endl;
#endif

    // time_index
    int interpolation_mode;
    if (!data::SingleInt::getInputValue(this,"interpolation_mode",
					interpolation_mode))
    {
	cout << "   could not retrieve interpolation_mode" << endl;
	return;
    }
    //interpolation_mode = !interpolation_mode;

    if (!allocated)
    {
	id_list = glGenLists(1);
	glGenTextures(1,&id_texture);
	allocated = true;
    }

#ifdef DEBUG_SCALARFIELDDISPLAYLIST
    cout << "   id_list" << id_list << endl;
#endif

    int x_samples = grid->xcells;
    int y_samples = grid->ycells;
    
    double quad_x = grid->quad.x;
    double quad_y = grid->quad.y;
    double quad_w = grid->quad.w;
    double quad_h = grid->quad.h;
    double cw = grid->quad.w / x_samples;
    double ch = grid->quad.h / y_samples;

#ifdef DEBUG_SCALARFIELDDISPLAYLIST
    cout << "x_samples " << x_samples << endl;
    cout << "y_samples " << y_samples << endl;
    cout << "quad_x " << grid->quad.x << endl;
    cout << "quad_y " << grid->quad.y << endl;
    cout << "quad_w " << grid->quad.w << endl;
    cout << "quad_h " << grid->quad.h << endl;
#endif

    // if (interpolation_mode == 0)
    // {

    // 	//
    // 	glNewList(id_list,GL_COMPILE);


    // 	// cout << "cw ch" << cw << " " << ch << endl;

    // 	glBegin(GL_QUADS); //starts drawing of points
    // 	for (int i=0;i<y_samples;i++)
    // 	{

    // 	    for (int j=0;j<x_samples;j++)
    // 	    {

    // 		double value = grid->getValue(j,i);
    // 		double cx, cy;

    // 		cx = quad_x + j * cw;
    // 		cy = quad_y + i * ch;
		    
    // 		if (colormap != NULL)
    // 		{
    // 		    Color c = colormap->getColor(value);
    // 		    glColor4f(c.r,c.g,c.b,c.a);
    // 		}
    // 		else {
    // 		    glColor3f(value,value,0);
    // 		}
		    
    // 		glVertex2f( cx      , cy + ch );
    // 		glVertex2f( cx      , cy      );
    // 		glVertex2f( cx + cw , cy      );
    // 		glVertex2f( cx + cw , cy + ch );
		    
    // 	    }
    // 	} // show_grid == 1
    // 	glEnd();//end drawing of points

    // 	glEndList();

    // }

    // else
    // {

	GLubyte textureData[y_samples][x_samples][3];
	//build texture
	int c;
	for (int i = 0; i < y_samples; i++) {
	    for (int j = 0; j < x_samples; j++) {			
		double value = grid->getValue(j,i);

		// double cx, cy;
		//cx = quad_x + j * cw;
		//cy = quad_y + i * ch;

		double r,g,b;
		if (colormap != NULL)
		{
		    Color c = colormap->getColor(value);
		    r = c.r;
		    g = c.g;
		    b = c.b;
		}
		else {
		    //glColor3f(value,value,0);
		    r = value;
		    g = value;
		    b = 0;
		}
		
		textureData[i][j][0] = (GLubyte) (255 * r); 
		textureData[i][j][1] = (GLubyte) (255 * g);
		textureData[i][j][2] = (GLubyte) (255 * b);
	    }
	}

	
	glEnable(GL_TEXTURE_2D);
	glPushAttrib(GL_TEXTURE_BIT);
	glBindTexture(GL_TEXTURE_2D, id_texture);
	if (interpolation_mode == 1)
	{
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else 
	{
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#ifdef DEBUG_SCALARFIELDDISPLAYLIST
	cout << " ----------------------------> ID_TEXTURE: " << id_texture << endl;
	cout << " ----------------------------> x_samples:  " << x_samples << endl;
	cout << " ----------------------------> y_samples:  " << y_samples << endl;
#endif

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 
		     0, 3, 
		     x_samples, 
		     y_samples, 
		     0, GL_RGB, 
		     GL_UNSIGNED_BYTE, 
		     &textureData[0][0][0]);

	glBindTexture(GL_TEXTURE_2D, 0);
	glPopAttrib();
	glDisable(GL_TEXTURE_2D);

	//
	glNewList(id_list,GL_COMPILE);

	glEnable(GL_TEXTURE_2D);
	glPushAttrib(GL_TEXTURE_BIT);
	glBindTexture(GL_TEXTURE_2D, id_texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(quad_x,quad_y);
	
	glTexCoord2f(1,0);
	glVertex2f(quad_x+quad_w,quad_y);
	
	glTexCoord2f(1,1);
	glVertex2f(quad_x+quad_w,quad_y+quad_h);
	
	glTexCoord2f(0,1);
	glVertex2f(quad_x,quad_y+quad_h);
	glEnd();
	

	glBindTexture(GL_TEXTURE_2D, 0);
	glPopAttrib();
	glDisable(GL_TEXTURE_2D);

	glEndList();

	// }

    
    getOutputPort("display_list")->setData
	(new data::SingleInt((int)id_list),true);

}

/**********************
 * MapBordersRenderer *
 **********************/

MapBordersRenderer::MapBordersRenderer(): Module("MapBordersRenderer")
{

    this->addInputPort("screen_window");
    this->addInputPort("world_window");
    this->addInputPort("map_borders_mode");
}
    
void 
MapBordersRenderer::process()
{
    // cout << "MapBordersRenderer::process()" << endl;

    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	return;
    // cout << "   screen window.....OK" << endl;

    ViewRect world_window;
    if (!data::SingleViewRect::getInputValue(this,"world_window",world_window))
	return;
    // cout << "   world window.....OK" << endl;

    // time_index
    int map_borders_mode;
    if (!data::SingleInt::getInputValue(this,"map_borders_mode",map_borders_mode))
    {
	cout << "   could not retrieve map_borders_mode" << endl;
	return;
    }

    if (map_borders_mode == 2)
	return;


    if (screen_window.w == 0 || screen_window.h == 0)
	return;

    Global *global = Global::instance();

    if (global->usa_states == NULL)
    {
	cout << "   usa states not loaded... returning without rendering" << endl;
    }

    ScreenWorldTransform swt(screen_window, world_window);
    ViewRect adj_screen_window = swt.getAdjustedScreenWindow();

    RenderSystem *renderSystem = global->getRenderSystem();
    renderSystem->startViewport(adj_screen_window, 
				world_window,
				screen_window);


    // renderSystem->setColor(0,0,0.2);
    // renderSystem->drawQuad(world_window.x, world_window.y,
    // 			   world_window.w, world_window.h);

    //
    // glCallList(global->usa_counties_gl_list);
    if (map_borders_mode == 0)
	glCallList(global->usa_states_gl_list);
    else if (map_borders_mode == 1)
	glCallList(global->usa_counties_gl_list);

    // global->usa_states->gl_draw_all_region_lines();

    renderSystem->endViewport();

}


/***********************
 * ScalarFieldRenderer *
 ***********************/

ScalarFieldRenderer::ScalarFieldRenderer():
    Module("ScalarFieldRenderer")
{
    this->addInputPort("world_window");
    this->addInputPort("screen_window");
    this->addInputPort("stencil_flag");
    this->addInputPort("display_list");


    // this->addInputPort("texture");
    // this->addInputPort("default");
    // this->addInputPort("bool_interpolate");
}

void 
ScalarFieldRenderer::process()
{

#ifdef DEBUG_SCALARFIELDRENDERER
    cout << "ScalarFieldRenderer::process()" << endl;
#endif

    int int_display_list;
    if (!data::SingleInt::getInputValue(this,"display_list",int_display_list))
	return;

#ifdef DEBUG_SCALARFIELDRENDERER
    cout << "   display list.......OK" << endl;
#endif

    int int_stencil_flag;
    if (!data::SingleInt::getInputValue(this,"stencil_flag",int_stencil_flag))
	return;

//			
    GLuint display_list = int_display_list;

    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	return;

#ifdef DEBUG_SCALARFIELDRENDERER
    cout << "   screen window.....OK" << endl;
#endif

    ViewRect world_window;
    if (!data::SingleViewRect::getInputValue(this,"world_window",world_window))
	return;

#ifdef DEBUG_SCALARFIELDRENDERER
    cout << "   world window.....OK" << endl;
#endif

    Global *global = Global::instance();

    ScreenWorldTransform swt(screen_window, world_window);
    ViewRect adj_screen_window = swt.getAdjustedScreenWindow();

    RenderSystem *renderSystem = global->getRenderSystem();
    renderSystem->startViewport(adj_screen_window, 
				world_window,
				screen_window);

    // turn on stencil
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    if(int_stencil_flag){
#ifdef DEBUG_SCALARFIELDRENDERER
    cout << "   Set stencil buffer" << endl;
#endif
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0x0, 0xff);
	glStencilOp(GL_INCR, GL_INCR, GL_INCR);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColorMask(0,0,0,0);
    

	glCallList(global->usa_area_gl_list);
    // global->usa_states->gl_draw_all_region_triangles();

    // glColorMask(1,1,1,1);
	glColorMask(1,1,1,1);
    }
    else{
#ifdef DEBUG_SCALARFIELDRENDERER
    cout << "   Disabling stencil buffer" << endl;
#endif
	glDisable(GL_STENCIL_TEST);
    }

    glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glCallList(display_list);
    
    glDisable(GL_STENCIL_TEST);

    // end viewport
    renderSystem->endViewport();

}


/******************
 * CursorRenderer *
 ******************/

CursorRenderer::CursorRenderer():
    Module("ScalarFieldRenderer")
{
    this->addInputPort("world_window");
    this->addInputPort("screen_window");
    this->addInputPort("lonlat");
    this->addInputPort("onoff");
}

void CursorRenderer::process()
{
    typedef data::Single<LonLatModule*> SinglaLonLatModulePtr;

#ifdef DEBUG_CURSORRENDERER
    cout << "CursorRenderer::process()" << endl;
#endif

    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	return;

#ifdef DEBUG_CURSORRENDERER
    cout << "   screen_window.....OK" << endl;
#endif

    ViewRect world_window;
    if (!data::SingleViewRect::getInputValue(this,"world_window",world_window))
	return;

#ifdef DEBUG_CURSORRENDERER
    cout << "   world_window.....OK" << endl;
#endif

    LonLatModule* lonlatModule;
    if (!SinglaLonLatModulePtr::getInputValue(this,"lonlat",lonlatModule))
	return;

#ifdef DEBUG_CURSORRENDERER
    cout << "   lonlat...........OK" << endl;
#endif

    int on;
    if (!data::SingleInt::getInputValue(this,"onoff",on))
	on = 1;

    // do not render the cursor if it is off
    if (!on)
	return;


    double lon, lat;
    lonlatModule->getLonLat(lon, lat);

    ScreenWorldTransform swt(screen_window, world_window);
    float x;
    float y;
    swt.world2screen(lon,lat,x,y);

    RenderSystem *renderSystem = Global::instance()->getRenderSystem();
    renderSystem->startViewport(screen_window);

    renderSystem->setColor(0,0,0);
    float r = 7;
    float seg1[] = {x-r,y,x+r,y};
    float seg2[] = {x,y-r,x,y+r};
    glLineWidth(2.0);
    renderSystem->drawLineStrip(2,seg1);
    renderSystem->drawLineStrip(2,seg2);
    glLineWidth(1.0);

    renderSystem->endViewport();

}


/***********************
 * MapLabels *
 ***********************/

MapLabels::MapLabels():
    Module("MapLabels")
{
    this->addInputPort("screen_window");
    this->addInputPort("time_index");
    this->addInputPort("file");
    this->addInputPort("value");
    this->addInputPort("var_name");
}

string dateToString(boost::gregorian::date){

    string month;

//     swith(date.month){
//     case()
//     }

}

void 
MapLabels::process()
{
#ifdef DEBUG_MAPLABELS
    cout << "MapLabels::process()" << endl;
#endif
    
    // datatypes::Grid *grid;
    // if (!data::SingleGridPtr::getInputValue(this,"grid",grid))
    // 	return;
    // cout << "   grid.......OK" << endl;

    // Colormap1d *colormap;
    // if (!data::SingleColormap1dPtr::getInputValue(this,"colormap",colormap))
    // 	return;

    bvdata::File* speciesFile;
    
    if (!data::Single<bvdata::File*>::getInputValue(this,"file",speciesFile))
    {
#ifdef DEBUG_TAGCLOUDDATA
	cout << "   could not retrieve species file" << endl;
#endif
	getOutputPort("tag_cloud")->setData(NULL,true);
	return;
    }
    
    string dateString;



    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
    	return;

#ifdef DEBUG_MAPLABELS
    cout << "   screen window.....OK" << endl;
#endif

    bool valid_var_name = true;
    bool valid_value    = true;
    bool valid_date     = true;


    string var_name="";
    if (!data::SingleString::getInputValue(this,"var_name",var_name))
    	valid_var_name = false;

#ifdef DEBUG_MAPLABELS
    cout << "   var name.....OK" << endl;
#endif


    double value;
    if (!data::Single<double>::getInputValue(this,"value",value))
	valid_value = false;

#ifdef DEBUG_MAPLABELS
    cout << "   value.....OK" << endl;
#endif

    int time_index;
    if (!data::SingleInt::getInputValue(this,"time_index",time_index))
	valid_date = false;
    
#ifdef DEBUG_MAPLABELS
    cout << "   time_index.......OK" << endl;
#endif

    // write date on port
    string date_st("");
    if(speciesFile != NULL && valid_date)
    {
 #ifdef DEBUG_MAPLABELS
    cout << "   Test is true" << endl;
    cout << "   before to_tm call" << endl;
    cout << "   time_index " << time_index << endl;   
    cout << "   month " << speciesFile->getDate(time_index).month() << " Day " << speciesFile->getDate(time_index).day() <<
	" Year " << speciesFile->getDate(time_index).year() << endl;
#endif       
	struct tm tm_val = to_tm(speciesFile->getDate(time_index));

 #ifdef DEBUG_MAPLABELS
	cout << "   Test is true" << endl;
    cout << "   before to_tm call" << endl;
#endif       


	char buf[128];
	strftime(buf, 128, "%b %d, %Y", &tm_val);
	string aux(buf);
	date_st = aux;
    }
 
 #ifdef DEBUG_MAPLABELS
    cout << "   Got Date from file.......OK" << endl;
#endif

    string value_st("");
    if (valid_value)
    {
	char buf[30];
	sprintf(buf,"%.4f",value);
	string aux(buf);
	value_st = aux;
    }
    
    string prefix("");
    if (valid_var_name)
    {
        prefix = var_name+"=";
    }

    // ViewRect world_window;
    // if (!data::SingleViewRect::getInputValue(this,"world_window",world_window))
    // 	return;
    // cout << "   world window.....OK" << endl;


    // ScreenWorldTransform swt(screen_window, world_window);
    // ViewRect adj_screen_window = swt.getAdjustedScreenWindow();

    Global *global = Global::instance();
    RenderSystem *renderSystem = global->getRenderSystem();
    renderSystem->startViewport(screen_window);

    // if (0.2 * screen_window.h > 16 && 0.6 * screen_window.w > 120)
    // { // standard size

    {
	double tx = screen_window.x + 4;
	double ty = screen_window.y + screen_window.h - 28;
	double tw = 120;
	double th = 24;

	renderSystem->setColor(0.1,0.1,0.1,0.8);
	renderSystem->drawQuad(tx,ty,tw,th);

	renderSystem->setColor(1,1,1);
	renderSystem->drawHorizontallyCenteredText(date_st.c_str(), 
						   16, tx + tw/2.0, ty+6);
    }
    // }

    // else  // shrinked size
    // {
    {
	double tw = 100;
	double th = 24;
	double tx = screen_window.x + 4 + 120 + 4 + 30 + 4 + 30 + 4; // screen_window.w - 4 - tw;
	double ty = screen_window.y + screen_window.h - 28;

	renderSystem->setColor(0.1,0.1,0.1,0.8);
	renderSystem->drawQuad(tx,ty,tw,th);

	renderSystem->setColor(1,1,1);
	string st = prefix+value_st;
	renderSystem->drawHorizontallyCenteredText(st.c_str(), 16, tx + tw/2.0, ty+6);
    }

    // }

    // end viewport
    renderSystem->endViewport();

}

/***********************
 * InfoRenderer *
 ***********************/

InfoRenderer::InfoRenderer():
    Module("InfoRenderer")
{
    this->addInputPort("file");
    this->addInputPort("screen_window");
    this->addInputPort("lonlat");
// 	bvdata::Repository* repo = Global::instance()->getRepository();

// 	ListView* files_view = new ListView();
// 	for (int i=0;i<repo->files.size();i++)
// 	{
// 	    bvdata::File *f = repo->files.at(i);
// //	    if (f->shortname == "ib")
// 	    //	f->load();
// 	    ListViewItem *item = new ListViewItem(f->getName()+" ("+f->shortname+")",13);
// 	    item->setCustomObject(f);
// 	    files_view->addItem(item);
// 	}
	
// 	int celladdr[] = { 0, 0, 0, 0};
// 	ScrollPane *scrollPane = new ScrollPane();
// 	scrollPane->setTitle("Species");
// 	scrollPane->setComponent(files_view);
// 	spreadsheet->installComponent( scrollPane, celladdr, 2);


}

void 
InfoRenderer::process()
{
    typedef  data::Single<bvdata::File *> SingleFilePtr;



#ifdef DEBUG_INFORENDERER
    cout << "InfoRenderer::process()" << endl;
#endif
    
    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
    	return;

    bvdata::File *file = NULL;
    string species_name = "";
    string species_sci_name = "";
    if (!SingleFilePtr::getInputValue(this,"file",file) || file == NULL){
	species_name = "----";
	species_sci_name = "----";
    }
    else{
	species_name = file->getName();
	species_sci_name = file->getSciName();
	if(this->scrollPane != NULL)
	    //update scroll pane
	    ;
    }

#ifdef DEBUG_INFORENDERER
    cout << "   screen window.....OK" << endl;
#endif


#ifdef DEBUG_INFORENDERER
    cout << "LonLatGridValue::process()" << endl;
#endif

    typedef data::Single<LonLatModule*> SinglaLonLatModulePtr;
    LonLatModule* lonlatModule;
    if (!SinglaLonLatModulePtr::getInputValue(this,"lonlat",lonlatModule))
    {
#ifdef DEBUG_INFORENDERER
	cout << "   could not load LonLatModule" << endl;
#endif
	return;
    }

    double lon, lat;
    lonlatModule->getLonLat(lon, lat);

    char buffer[1024];
    sprintf(buffer,"Lon, Lat: %.4f, %.4f",lon,lat);

    //

    // ViewRect world_window;
    // if (!data::SingleViewRect::getInputValue(this,"world_window",world_window))
    // 	return;
    // cout << "   world window.....OK" << endl;

    // ScreenWorldTransform swt(screen_window, world_window);
    // ViewRect adj_screen_window = swt.getAdjustedScreenWindow();

    Global *global = Global::instance();

    string region_name = global->getRegionName(lon,lat);

    RenderSystem *renderSystem = global->getRenderSystem();

    renderSystem->startViewport(screen_window);
    
    renderSystem->setColor(ColorConstants::TITLE_BG);
    renderSystem->drawQuad(screen_window.x,screen_window.y,
			   screen_window.w,screen_window.h);
    
    renderSystem->setColor(ColorConstants::TITLE_TEXT);

    float x = screen_window.x;
    float y = screen_window.y;
    float w = screen_window.w;
    float h = screen_window.h;

    float font_big   = 18;
    float font_medium   = 18;
    float font_small = 14;

    float cx = x + 4;
    float cy = y + h;

    float dy_big   = -30;
    float dy_small = -24;

    cy += dy_big;
    renderSystem->drawText(species_name.c_str(), font_big, cx, cy);

    cy += dy_small;
    renderSystem->drawText(species_sci_name.c_str(), font_medium, cx, cy);

    cy += dy_big;
    cy += dy_small;
    cy += dy_small;
    renderSystem->drawText(buffer, font_small, cx, cy);

    cy += dy_small;
    renderSystem->drawText(region_name.c_str(), font_small, cx, cy);

    // end viewport
    renderSystem->endViewport();
}

/*****************
 * PhotoRenderer *
 *****************/

PhotoRenderer::PhotoRenderer():
    Module("PhotoRenderer")
{
    this->useLogo = false;
    this->texture = NULL;
    this->addInputPort("screen_window");
}

PhotoRenderer::~PhotoRenderer()
{
#ifdef DEBUG_PHOTORENDERER
    cout << "   Deleting PhotoRenderer" << endl;
#endif
    if(this->texture != NULL)
	delete texture;
}

void 
PhotoRenderer::process()
{

#ifdef DEBUG_PHOTORENDERER
    cout << "void PhotoRenderer::process()" << endl;
#endif

    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
    	return;

#ifdef DEBUG_PHOTORENDERER
    cout << "   screen window ok..." << endl;
#endif


//     typedef  data::Single<bvdata::File *> SingleFilePtr;
//     bvdata::File *file = NULL;
//     if (!SingleFilePtr::getInputValue(this,"file",file)){
// 	return;
//     }

// #ifdef DEBUG_PHOTORENDERER
//     cout << "   file ok..." << endl;
// #endif
   

    Global *global = Global::instance();

    RenderSystem *renderSystem = global->getRenderSystem();

    renderSystem->startViewport(screen_window);


    
    if(this->useLogo || this->texture == NULL || !this->texture->isLoaded()){
	//cout << "Texture is not Loaded" << endl;
// 	renderSystem->setColor(ColorConstants::TITLE_BG);
// 	renderSystem->drawQuad(screen_window.x,screen_window.y,
// 			       screen_window.w,screen_window.h);
	Global *global = Global::instance();

	Color black(1.0,1.0,1.0);
	renderSystem->setColor(black);
	renderSystem->drawQuad(screen_window.x,screen_window.y,
			       screen_window.w,screen_window.h);
	

	float x_picture, y_picture, w_picture, h_picture; // screen coords
	float w_texture = (float) global->bv_logo->getWidth();
	float h_texture = (float) global->bv_logo->getHeight();
	float margin = 3;
	util::computeMaxAndCenteredRectanglePreservingAspectRatio(
	    w_texture, h_texture,
	    screen_window.x + margin, screen_window.y + margin, screen_window.w - 2 * margin, screen_window.h - 2 * margin,
	    x_picture, y_picture, w_picture, h_picture);

	renderSystem->drawQuadWithTexture(x_picture, y_picture, w_picture, h_picture, 
					  global->bv_logo->getId());
    }
    else{

	Color black(0,0,0);
	renderSystem->setColor(black);
	renderSystem->drawQuad(screen_window.x,screen_window.y,
			       screen_window.w,screen_window.h);


	float x_picture, y_picture, w_picture, h_picture; // screen coords
	float w_texture = (float)this->texture->getWidth();
	float h_texture = (float)this->texture->getHeight();
	float margin = 3;
	util::computeMaxAndCenteredRectanglePreservingAspectRatio(
	    w_texture, h_texture,
	    screen_window.x + margin, screen_window.y + margin, screen_window.w - 2 * margin, screen_window.h - 2 * margin,
	    x_picture, y_picture, w_picture, h_picture);

	// // take care of aspect ratio
	// float ar_texture = w_texture/h_texture;
	// float ar_screen  = ((float)screen_window.w)/screen_window.h;

	// cout << "w_texture  " << w_texture  << endl;
	// cout << "h_texture  " << h_texture  << endl;
	// cout << "ar_texture " << ar_texture << endl;
	// cout << "ar_screen  " << ar_screen  << endl;


	// if (ar_texture <= ar_screen)
	// {
	//     float scale =  screen_window.h / h_texture;
	//     w_picture = w_texture * scale;
	//     h_picture = screen_window.h;
	//     y_picture = screen_window.y;
	//     x_picture = screen_window.x + (screen_window.w - w_picture)/2.0;
	// }
	// else 
	// {
	//     float scale =  screen_window.w / w_texture;
	//     h_picture = h_texture * scale;
	//     w_picture = screen_window.w;
	//     x_picture = screen_window.x;
	//     y_picture = screen_window.y + (screen_window.h - h_picture)/2.0;
	// }


	// cout << "x_picture " << x_picture << endl;
	// cout << "y_picture " << y_picture << endl;
	// cout << "w_picture " << w_picture << endl;
	// cout << "h_picture " << h_picture << endl;

	renderSystem->drawQuadWithTexture(x_picture, y_picture, w_picture, h_picture,
					  this->texture->getId());
    }

    // end viewport
    renderSystem->endViewport();
}

void PhotoRenderer::load(string filename){
    //load here
    //TODO check if the file exists

    if(this->texture != NULL && this->texture->isLoaded()){
	this->deleteTexture();
    }

    this->texture = new Texture();
    texture->load(filename);
    
}

void PhotoRenderer::deleteTexture(){
    if(this->texture!=NULL && this->texture->isLoaded()){
	this->texture->release();
	delete this->texture;
    }
}

void PhotoRenderer::toggleUseLogo(){
    this->useLogo = !(this->useLogo);
}

/***********************
 * TitleRenderer *
 ***********************/

TitleRenderer::TitleRenderer():
    Module("TitleRenderer")
{
    this->addInputPort("screen_window");
}

void 
TitleRenderer::setTitle(string title)
{
    this->title = title;
}

string 
TitleRenderer::getTitle()
{
    return title;
}


void 
TitleRenderer::process()
{

#ifdef DEBUG_TITLERENDERER
    cout << "TitleRenderer::process()" << endl;
#endif
    
    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
    	return;

#ifdef DEBUG_TITLERENDERER
    cout << "   screen window.....OK" << endl;
#endif

    // ViewRect world_window;
    // if (!data::SingleViewRect::getInputValue(this,"world_window",world_window))
    // 	return;
    // cout << "   world window.....OK" << endl;


    // ScreenWorldTransform swt(screen_window, world_window);
    // ViewRect adj_screen_window = swt.getAdjustedScreenWindow();

    Global *global = Global::instance();
    RenderSystem *renderSystem = global->getRenderSystem();

    
    ViewRect bb = renderSystem->textBoundingBox(title.c_str(), 1.0, 0, 0);
    
    float fontSize = min(19.0, floor (screen_window.h * 0.7)/bb.h);
    
    renderSystem->startViewport(screen_window);
    
    renderSystem->setColor(ColorConstants::TITLE_BG);
    renderSystem->drawQuad(screen_window.x,screen_window.y,
			   screen_window.w,screen_window.h);
    
    renderSystem->setColor(ColorConstants::TITLE_TEXT);
    renderSystem->drawCenteredText(title.c_str(), 
				   fontSize, 
				   (int) (screen_window.x + screen_window.w/2.0),
				   (int) (screen_window.y + screen_window.h/2.0),
				   0,0);
    // }

    // else  // shrinked size
    // {

    // }

    // end viewport
    renderSystem->endViewport();
}

/**************************************
 * TagCloudOnSelectionsInfrastructure *
 **************************************/

TagCloudOnSelectionsInfrastructure::TagCloudOnSelectionsInfrastructure(
    ScreenWindowModule*     mod_screen_window,
    WorldWindowModule*      mod_world_window,
    OffsetModule*           mod_offseted_time_index,
    FileModule*             mod_file,
    IntModule*              mod_tagcloud_on_off,
    SelectionRepository*    selection_repository,
    VectorIntModule*        mod_tagcloud_mask,
    int                     time_offset)
{
    this->mod_file = mod_file;
    this->mod_screen_window = mod_screen_window;
    this->mod_world_window  = mod_world_window;
    this->mod_offseted_time_index = mod_offseted_time_index;
    this->mod_tagcloud_mask = mod_tagcloud_mask;
    

    new Connection(mod_screen_window->getOutputPort("screen_window"),
		   mod_tag_cloud_renderer.getInputPort("clipping_region"));

    new Connection(mod_tagcloud_on_off->getOutputPort("int_value"),
		   mod_tag_cloud_renderer.getInputPort("onoff"));

    // connect
    SelectionRepository::signal_connection_t connection_1 =  
	selection_repository->connectToAddSelectionSignal(
	    boost::bind(&TagCloudOnSelectionsInfrastructure::receiveNewSelectionSignal, 
			this, _1));

    // connect
    SelectionRepository::signal_connection_t connection_2 =  
	selection_repository->connectToDelSelectionSignal(
	    boost::bind(
		&TagCloudOnSelectionsInfrastructure::receiveDeleteSelectionSignal, 
		this, _1));
    
}

void 
TagCloudOnSelectionsInfrastructure::render()
{
    mod_tag_cloud_renderer.outdated = true;
    mod_tag_cloud_renderer.run();
}

void 
TagCloudOnSelectionsInfrastructure::receiveDeleteSelectionSignal(Selection *sel)
{
    if (map_selection_to_data.count(sel) > 0)
    {
	TagCloudData* c = map_selection_to_data[sel];
    
	mod_tag_cloud_renderer.deleteAllConnectionsFromOrTo(c);
	mod_world_window->deleteAllConnectionsFromOrTo(c);
	mod_file->deleteAllConnectionsFromOrTo(c);
	mod_screen_window->deleteAllConnectionsFromOrTo(c);
	mod_offseted_time_index->deleteAllConnectionsFromOrTo(c);
	mod_tagcloud_mask->deleteAllConnectionsFromOrTo(c);

	c->deleteAllConnectionsFromOrTo(sel);

	map_selection_to_data.erase(sel);

	delete c;
    } // disconnect Selection and delete it
}

void 
TagCloudOnSelectionsInfrastructure::receiveNewSelectionSignal(Selection *sel)
{
    if(this->mod_file->fileLoaded()){
	TagCloudData *tgd = new TagCloudData();

	new Connection(sel->getOutputPort("selection"),
		       tgd->getInputPort("selection"));

	new Connection(mod_offseted_time_index->getOutputPort("output_int"),
		       tgd->getInputPort("time_index"));

	new Connection(mod_file->getOutputPort("file"),
		       tgd->getInputPort("species_file"));

	new Connection(mod_world_window->getOutputPort("world_window"),
		       tgd->getInputPort("world_window"));

	new Connection(mod_screen_window->getOutputPort("screen_window"),
		       tgd->getInputPort("screen_window"));

	new Connection(tgd->getOutputPort("tag_cloud"),
		       mod_tag_cloud_renderer.getInputPort("tag_clouds"));

// 	cout << "Values port " << mod_tagcloud_mask->getOutputPort("values") << endl;
// 	cout << "Maks Port " << tgd->getInputPort("mask") << endl;

	new Connection(mod_tagcloud_mask->getOutputPort("values"),
		       tgd->getInputPort("mask"));

	// associate
	map_selection_to_data[sel] = tgd;
    }
}

/*************************
 * MapCellInfrastructure *
 *************************/

MapCellInfrastructure::MapCellInfrastructure (
    WorldWindowModule*             mod_world_window,
    StringModule*                  mod_expression,
    IntModule*                     mod_time_index,
    ColormapController*            mod_colormap_controller,
    IntModule*                     mod_map_borders_mode,
    IntModule*                     mod_interpolation_mode,
    IntModule*                     mod_stencil_flag,
    FileModule*                    mod_file,
    LonLatModule*                  mod_longitude_latitude,
    IntModule*                     mod_cursor_on_off,
    IntModule*                     mod_tagcloud_on_off,
    VectorIntModule*               mod_tagcloud_mask,
    SelectionRepository*           selection_repository,
    int                            time_offset,
    int                            max_time_offset,
    string                         var_name)
{

#ifdef DEBUG_MAPCELLINFRASTRUCTURE
    cout << "MapCellInfrastructure::MapCellInfrastructure(...)" << endl;
#endif

    zoom_and_pan_controller.setWorldWindowModule(mod_world_window);
    zoom_and_pan_controller.setLonLatModule(mod_longitude_latitude);

    selection_controller = new SelectionController(selection_repository, 
						   mod_world_window);

    this->mod_file = mod_file;

    { // mod_cursor_renderer

	new Connection(mod_longitude_latitude->getOutputPort("lonlat"),
		       mod_cursor_renderer.getInputPort("lonlat"));

	new Connection(mod_screen_window.getOutputPort("screen_window"),
		       mod_cursor_renderer.getInputPort("screen_window"));

	new Connection(mod_world_window->getOutputPort("world_window"),
		       mod_cursor_renderer.getInputPort("world_window"));

	new Connection(mod_cursor_on_off->getOutputPort("int_value"),
		       mod_cursor_renderer.getInputPort("onoff"));

    } // mod_cursor_renderer


    { // mod_offseted_time_index

	mod_offseted_time_index.setOrder(max_time_offset);
	mod_offseted_time_index.setOffset(time_offset);
	new Connection(mod_time_index->getOutputPort("int_value"),
		       mod_offseted_time_index.getInputPort("input_int"));

    } // mod_offseted_time_index

    { // mod_lonlat_grid_value
	new Connection(mod_longitude_latitude->getOutputPort("lonlat"),
		       mod_lonlat_grid_value.getInputPort("lonlat"));

	new Connection(mod_scalar_field_data.getOutputPort("grid"),
		       mod_lonlat_grid_value.getInputPort("grid"));
	
    } // mod_lonlat_grid_value
#ifdef DEBUG_MAPCELLINFRASTRUCTURE
    cout << "   wire mod_map_borders_renderer" << endl;
#endif
    { // mod_map_borders_renderer

	new Connection(mod_world_window->getOutputPort("world_window"),
		       mod_map_borders_renderer.getInputPort("world_window"));

	new Connection(mod_screen_window.getOutputPort("screen_window"),
		       mod_map_borders_renderer.getInputPort("screen_window"));

	new Connection(mod_map_borders_mode->getOutputPort("int_value"),
		       mod_map_borders_renderer.getInputPort("map_borders_mode"));

    } // mod_map_borders_renderer


#ifdef DEBUG_MAPCELLINFRASTRUCTURE
    cout << "   wire mod_scalar_field_data" << endl;
#endif

    { // mod_scalar_field_data

	new Connection(mod_expression->getOutputPort("string_value"),
		       mod_scalar_field_data.getInputPort("expression"));

	new Connection(mod_offseted_time_index.getOutputPort("output_int"),
		       mod_scalar_field_data.getInputPort("time_index"));

    	new Connection(mod_file->getOutputPort("file"),
    		       mod_scalar_field_data.getInputPort("file"));

// 	this->addInputPort("file_shortname");

    } // mod_scalar_field_data


#ifdef DEBUG_MAPCELLINFRASTRUCTURE
    cout << "   wire mod_scalar_field_renderer" << endl;
#endif

    { // mod_scalar_field_display_list

	new Connection(mod_scalar_field_data.getOutputPort("grid"),
		       mod_scalar_field_display_list.getInputPort("grid"));

	new Connection(mod_colormap_controller->getOutputPort("colormap"),
		       mod_scalar_field_display_list.getInputPort("colormap"));

	new Connection(mod_interpolation_mode->getOutputPort("int_value"),
		       mod_scalar_field_display_list.getInputPort("interpolation_mode"));

    } // mod_scalar_field_display_list

    { // mod_scalar_field_renderer

	new Connection(mod_world_window->getOutputPort("world_window"),
		       mod_scalar_field_renderer.getInputPort("world_window"));

	new Connection(mod_screen_window.getOutputPort("screen_window"),
		       mod_scalar_field_renderer.getInputPort("screen_window"));

	new Connection(mod_scalar_field_display_list.getOutputPort("display_list"),
		       mod_scalar_field_renderer.getInputPort("display_list"));

	new Connection(mod_stencil_flag->getOutputPort("int_value"),
		       mod_scalar_field_renderer.getInputPort("stencil_flag"));

    } // mod_scalar_field_renderer


    { // mod_map_labels

	new Connection(mod_screen_window.getOutputPort("screen_window"),
		       mod_map_labels.getInputPort("screen_window"));

	new Connection(mod_offseted_time_index.getOutputPort("output_int"),
		       mod_map_labels.getInputPort("time_index"));

	new Connection(mod_lonlat_grid_value.getOutputPort("value"),
		       mod_map_labels.getInputPort("value"));

	new Connection(mod_file->getOutputPort("file"),
		       mod_map_labels.getInputPort("file"));

	mod_map_labels.getInputPort("var_name")->setData(new data::SingleString(var_name));

    } // mod_map_labels


    { // mod_map_selections_renderer

	new Connection(mod_screen_window.getOutputPort("screen_window"),
		       mod_map_selections_renderer.getInputPort("screen_window"));

	new Connection(mod_world_window->getOutputPort("world_window"),
		       mod_map_selections_renderer.getInputPort("world_window"));

    } // mod_map_selections_renderer


    { // connect buttons

	// + button
	ViewRect btn_minus_area(124 + 4,-28,30,24);
	btn_offset_minus = new Button(btn_minus_area,"<",
				      Button::LEFT | Button::TOP);
	btn_offset_minus->connect(boost::bind(&MapCellInfrastructure::decrementOffsetSignal, this));

	// - button
	ViewRect btn_plus_area(124  + 4 + 30 + 4, -28, 30, 24);
	btn_offset_plus = new Button(btn_plus_area,">",
				      Button::LEFT | Button::TOP);
	btn_offset_plus->connect(
	    boost::bind(&MapCellInfrastructure::incrementOffsetSignal, this));

    } // connect buttons

    // order of event process
    zoom_and_pan_controller.event_add_node(selection_controller);
    selection_controller->event_add_node(btn_offset_minus);
    selection_controller->event_add_node(btn_offset_plus);

    // tcinfra
    tag_cloud_infrastructure = new TagCloudOnSelectionsInfrastructure(
	&mod_screen_window,
	mod_world_window,
	&mod_offseted_time_index,
	mod_file,
	mod_tagcloud_on_off,
	selection_repository,
	mod_tagcloud_mask,
	time_offset);

    // connect
    SelectionRepository::signal_connection_t connection_1 =  
	selection_repository->connectToAddSelectionSignal(
	    boost::bind(&MapCellInfrastructure::receiveNewSelectionSignal, this, _1));

    // connect
    SelectionRepository::signal_connection_t connection_2 =  
	selection_repository->connectToDelSelectionSignal(
	    boost::bind(&MapCellInfrastructure::receiveDeleteSelectionSignal, this, _1));

}

void 
MapCellInfrastructure::setMaxTimeOffset(int max_time_offset){
    this->mod_offseted_time_index.setOrder(max_time_offset);
}

void 
MapCellInfrastructure::incrementOffsetSignal()
{
#ifdef DEBUG_MAPCELLINFRASTRUCTURE
    cout << "MapCellInfrastructure::incrementOffsetSignal()" << endl;
#endif

    if(this->mod_file->fileLoaded()){
	int curr_off = mod_offseted_time_index.getOffset();
#ifdef DEBUG_MAPCELLINFRASTRUCTURE
	cout << "   Current Offset " << curr_off << endl;
	cout << "   New Offset " << ((curr_off + 1) % mod_offseted_time_index.getOrder()) << endl; 
#endif
	mod_offseted_time_index.setOffset( (curr_off + 1) % mod_offseted_time_index.getOrder());
    }
}

void 
MapCellInfrastructure::decrementOffsetSignal()
{
    if(this->mod_file->fileLoaded()){
	int new_off = mod_offseted_time_index.getOffset() - 1;
	if (new_off < 0)
	    new_off = mod_offseted_time_index.getOrder() - 1;
	mod_offseted_time_index.setOffset(new_off);
    }
}

void 
MapCellInfrastructure::receiveNewSelectionSignal(Selection *sel)
{
#ifdef DEBUG_MAPCELLINFRASTRUCTURE
    cout << "MapCellInfrastructure::receiveNewSelectionSignal(...)" << endl;
#endif

    if(this->mod_file->fileLoaded()){
	new Connection(sel->getOutputPort("selection"),
		       mod_map_selections_renderer.getInputPort("selections"));
	mod_map_selections_renderer.outdated = true;
    }
}


void 
MapCellInfrastructure::receiveDeleteSelectionSignal(Selection *sel)
{
#ifdef DEBUG_MAPCELLINFRASTRUCTURE
    cout << "MapCellInfrastructure::receiveDeleteSelectionSignal(...)" << endl;
#endif
    mod_map_selections_renderer.deleteAllConnectionsFromOrTo(sel);
}

void
MapCellInfrastructure::installAt(rectanglelayout::Node* area)
{
    // source module
    mod_screen_window.connectToRectangleSignal(area);

    // controllers
    zoom_and_pan_controller.connectToRectangleSignal(area);
    selection_controller->connectToRectangleSignal(area);
    
    //
    btn_offset_plus->connectToRectangleSignal(area);
    btn_offset_minus->connectToRectangleSignal(area);
}

void
MapCellInfrastructure::render()
{
    ViewRect sw = mod_screen_window.getScreenWindow();

    RenderSystem *renderSystem = Global::instance()->getRenderSystem();
    renderSystem->startViewport(sw.x,
				sw.y,
				sw.w,
				sw.h);

    renderSystem->setColor(ColorConstants::MAP_BG);

    renderSystem->drawQuad(sw.x, sw.y, sw.w, sw.h);
    renderSystem->endViewport();

    mod_scalar_field_renderer.outdated = true;
    mod_scalar_field_renderer.run();

    mod_map_borders_renderer.outdated = true;
    mod_map_borders_renderer.run();

    mod_map_selections_renderer.outdated = true;
    mod_map_selections_renderer.run();

    mod_map_labels.outdated = true;
    mod_map_labels.run();


    // render tag cloud selection on map
    tag_cloud_infrastructure->render();


    mod_cursor_renderer.outdated = true;
    mod_cursor_renderer.run();

    btn_offset_plus->render();
    btn_offset_minus->render();

}

/***********
 * Toolbar *
 ***********/

void
Toolbar::toggleCursor(int state)
{
    occurrence_env->mod_cursor_on_off.setInt(state);
}

void
Toolbar::toggleHabitat(int state)
{
    occurrence_env->mod_habitat_tc_on_off.setInt(state);
}

void 
Toolbar::toggleInterpolate(int state){
    this->btn_interpolate->toggle = state;
    this->btn_interpolate->state = state;
}

void 
Toolbar::search(int state)
{
    string x="";
    bool   ok;
    QString st = QInputDialog::getText(NULL,//this->parent, 
				       "Search (e.g. Florida)",
				       "Value", 
				       QLineEdit::Normal,
				       x.c_str(), 
				       &ok);
    if (ok && !st.isEmpty())
    {
	float x[4];
	Global::instance()->keyword_bounds(st.toStdString(), x);
	ViewRect vr(x[0],x[1],x[2],x[3]);
	occurrence_env->mod_world_window.setWorldWindow(vr);
    }
}

void 
Toolbar::show_hide_global_integral_curves(int state)
{
    occurrence_env->show_hide_global_integral_curves(state);
}

void
Toolbar::toggle_stencil(int state){
    this->btn_toggle_stencil->toggle = state;
    this->btn_toggle_stencil->state = state;

    occurrence_env->toggle_stencil(state);
}

void 
Toolbar::google_maps(int state)
{
    Global::instance()->open_google_maps_link(
	occurrence_env->mod_world_window.getWorldWindow());
}

void 
Toolbar::pie_chart(int state){
    this->occurrence_env->open_pie_chart();
}

void 
Toolbar::interpolate(int state){
    this->btn_interpolate->toggle = state;// (this->btn_interpolate->toggle + 1)%2;

//    cout << "Toggle " << this->btn_interpolate->toggle << endl;

    occurrence_env->toggle_interpolate(state);
}

Button*
new_button(ViewRect rect, string name, bool toggle)
{
    Button *btn = new Button(rect,name, 
			       (Button::LEFT | Button::TOP));
    btn->font_size = 13;
    btn->bg_color.init(0.2,0.2,0.2);
    btn->text_color_0.init(1,1,1);
    btn->text_color_1.init(0.5,1,0.5);
    btn->toggle = toggle;
    return btn;
}

Toolbar::Toolbar(OccurrenceEnvironment     *occurrence_env):
    occurrence_env(occurrence_env)
{

    ViewRect rect;

    float btn_w = 100;
    float btn_h = 22;

    float x0 = 8;

    float x  = x0;
    float y  = -30;

    float h_sep = 8;
    float v_sep = 8;

    int num_cols = 2;

    int col = 0;

    { // toggle cursor

	rect.init(x,y,btn_w,btn_h);
	btn_toggle_cursor = new_button(rect,"Cursor",true); 
	btn_toggle_cursor->connect(
	    boost::bind(&Toolbar::toggleCursor, this, _1));

	//
	this->event_add_node(btn_toggle_cursor);

    } // 

    col++;
    if (col == num_cols)
    {
	col = 0;
	x = x0;
	y += -v_sep -btn_h;
    }
    else {
	x += h_sep + btn_w;
    }
	
    { // toggle habitat

	rect.init(x,y,btn_w,btn_h);
	btn_toggle_habitat = new_button(rect,"Habitat",true); 
	btn_toggle_habitat->connect(
	    boost::bind(&Toolbar::toggleHabitat, this, _1));

	//
	this->event_add_node(btn_toggle_habitat);

    } // 


    col++;
    if (col == num_cols)
    {
	col = 0;
	x = x0;
	y += -v_sep -btn_h;
    }
    else {
	x += h_sep + btn_w;
    }

    { // toggle search
	rect.init(x,y,btn_w,btn_h);
	btn_toggle_integral_curve = new_button(rect,"Integrals",true); 
	btn_toggle_integral_curve->state = 1;
	btn_toggle_integral_curve->connect(
	    boost::bind(&Toolbar::show_hide_global_integral_curves, this, _1));
	
	//
	this->event_add_node(btn_toggle_integral_curve);

    } // 

    col++;
    if (col == num_cols)
    {
	col = 0;
	x = x0;
	y += -v_sep -btn_h;
    }
    else {
	x += h_sep + btn_w;
    }
	


    { // toggle interpolate
	rect.init(x,y,btn_w,btn_h);
	btn_interpolate = new_button(rect,"Interpolate",false); 
	int state = this->occurrence_env->mod_interpolation_mode.getInt();
	btn_interpolate->state = state;
	btn_interpolate->toggle = state;
	btn_interpolate->connect(
	    boost::bind(&Toolbar::interpolate, this, _1));

	//
	this->event_add_node(btn_interpolate);

    } // 
    col++;
    if (col == num_cols)
    {
	col = 0;
	x = x0;
	y += -v_sep -btn_h;
    }
    else {
	x += h_sep + btn_w;
    }

    { // toggle interpolate
	rect.init(x,y,btn_w,btn_h);
	btn_toggle_stencil = new_button(rect,"Crop to Map",false); 
	int state = this->occurrence_env->mod_stencil_flag.getInt();
	btn_toggle_stencil->state = state;
	btn_toggle_stencil->toggle = state;
	btn_toggle_stencil->connect(
	    boost::bind(&Toolbar::toggle_stencil, this, _1));

	//
	this->event_add_node(btn_toggle_stencil);

    } // 
    col++;
    if (col == num_cols)
    {
	col = 0;
	x = x0;
	y += -v_sep -btn_h;
    }
    else {
	x += h_sep + btn_w;
    }

    { // toggle search
	rect.init(x,y,btn_w,btn_h);
	btn_search = new_button(rect,"Search",false); 
	btn_search->connect(
	    boost::bind(&Toolbar::search, this, _1));

	//
	this->event_add_node(btn_search);
    } // 


    col++;
    if (col == num_cols)
    {
	col = 0;
	x = x0;
	y += -v_sep -btn_h;
    }
    else {
	x += h_sep + btn_w;
    }


    { // toggle maps
	rect.init(x,y,btn_w,btn_h);
	btn_google_maps = new_button(rect,"Google Maps",false); 
	btn_google_maps->connect(
	    boost::bind(&Toolbar::google_maps, this, _1));

	//
	this->event_add_node(btn_google_maps);

    } // 


    col++;
    if (col == num_cols)
    {
	col = 0;
	x = x0;
	y += -v_sep -btn_h;
    }
    else {
	x += h_sep + btn_w;
    }


    { // toggle maps
	rect.init(x,y,btn_w,btn_h);
	btn_pie_chart = new_button(rect,"Pub x Pri Lands",false); 
	btn_pie_chart->connect(
	    boost::bind(&Toolbar::pie_chart, this, _1));

	//
	this->event_add_node(btn_pie_chart);

    } // 


	// // + button
	// ViewRect btn_minus_area(124 + 4,-28,30,24);
	// btn_offset_minus = new Button(btn_minus_area,"<", 
	// 			      (Button::LEFT | Button::TOP));
	// btn_offset_minus->connect(boost::bind(&SimpleEnvironment::decrementOffsetSignal, this,_1));

	// // - button
	// ViewRect btn_plus_area(124  + 4 + 30 + 4, -28, 30, 24);
	// btn_offset_plus = new Button(btn_plus_area,">", 
	// 			     (Button::LEFT | Button::TOP));
	// btn_offset_plus->connect(boost::bind(&SimpleEnvironment::incrementOffsetSignal, this,_1));

	// // habitat button
	// ViewRect btn_habitat_on_off_area(-70, 4, 60, 22);
	// btn_habitat_on_off = new Button(btn_habitat_on_off_area,"Habitat", 
	// 			     (Button::RIGHT | Button::BOTTOM));
	// btn_habitat_on_off->font_size = 13;
	// btn_habitat_on_off->bg_color     = *ColorConstants::MAP_BTN_BG;
	// btn_habitat_on_off->text_color_0 = *ColorConstants::MAP_BTN_TEXT;
	// btn_habitat_on_off->text_color_1 = *ColorConstants::MAP_BTN_OFF;
	// btn_habitat_on_off->toggle       = true;

	// btn_habitat_on_off->connect(
	//     boost::bind(&SimpleEnvironment::toggleHabitatTC, this, _1));
	// mod_in_map_tag_cloud_renderer.setVisible(false); // start not visible
	// mod_in_map_tag_cloud_renderer.setVisible(false); // start not visible

	// // search button
	// ViewRect btn_search_area(-134, 4, 60, 22);
	// btn_search = new Button(btn_search_area,"Search", 
	// 			     (Button::RIGHT | Button::BOTTOM));
	// btn_search->font_size    = 13;
	// btn_search->bg_color     = *ColorConstants::MAP_BTN_BG;
	// btn_search->text_color_0 = *ColorConstants::MAP_BTN_TEXT;
	// btn_search->text_color_1 = *ColorConstants::MAP_BTN_OFF;
	// btn_search->connect(
	//     boost::bind(&SimpleEnvironment::search, this, _1));


}

void 
Toolbar::installAt(rectanglelayout::Node *area)
{
    this->connectToRectangleSignal(area);
    btn_toggle_cursor->connectToRectangleSignal(area);
    btn_toggle_habitat->connectToRectangleSignal(area);
    btn_search->connectToRectangleSignal(area);
    btn_google_maps->connectToRectangleSignal(area);
    btn_toggle_integral_curve->connectToRectangleSignal(area);
    btn_interpolate->connectToRectangleSignal(area);
    btn_toggle_stencil->connectToRectangleSignal(area);
    btn_pie_chart->connectToRectangleSignal(area);
}

void 
Toolbar::render()
{    
    RenderSystem *renderSystem = Global::instance()->getRenderSystem();
    renderSystem->startViewport(screen_window);

    renderSystem->setColor(ColorConstants::TITLE_BG);
    renderSystem->drawQuad(screen_window.x,screen_window.y,screen_window.w,screen_window.h);

    renderSystem->endViewport();

    btn_toggle_cursor->render();
    btn_toggle_habitat->render();
    btn_search->render();
    btn_google_maps->render();
    btn_toggle_integral_curve->render();
    btn_interpolate->render();    
    btn_toggle_stencil->render();        
    btn_pie_chart->render();    
}

void 
Toolbar::rectangleSignal(rectanglelayout::Rectangle r)
{
    screen_window.init(r.x,r.y,r.w,r.h);
}

bool 
Toolbar::event_mouse_contains(int x, int y)
{
    return screen_window.contains(x,y);
}









/*********************
 * PredictorsToolbar *
 *********************/

PredictorsToolbar::PredictorsToolbar(OccurrenceEnvironment     *occurrence_env):
    occurrence_env(occurrence_env)
{
}

void PredictorsToolbar::clickEvent(int state, Button* btn)
{

    cout << "PredictorsToolbar::clickEvent(...)" << endl;
    
    int index = -1;
    int numberOfButtons = this->predictors_btns.size();

    for(int i = 0 ; i < numberOfButtons ; i++){
	if(btn == this->predictors_btns.at(i)){
	    index = i;
	    break;
	}
    }

    

    cout << "Index " << index << endl;
    this->occurrence_env->change_tc_mask(state, index);
}

void PredictorsToolbar::init(vector<string> predictors){
#ifdef DEBUG_PREDICTORTOOLBAR
    cout << "PredictorsToolbar::init(...)" << endl;
#endif    

    ViewRect rect;

    float btn_w = 120;
    float btn_h = 22;

    float x0 = 8;

    float x  = x0;
    float y  = -30;

    float h_sep = 8;
    float v_sep = 8;

    //int num_cols = (int) (this->screen_window.w/(btn_w + 2 * h_sep));
    int num_cols = 3;

    int col = 0;

    int numberOfPredictors = predictors.size();

#ifdef DEBUG_PREDICTORTOOLBAR
    cout << "Number of Predictors " << numberOfPredictors << endl;
    cout << "Number of Columns " << num_cols << endl;
#endif

    for(int i = 0 ; i < numberOfPredictors ; i++){
	rect.init(x,y,btn_w,btn_h);
	Button* btn = new_button(rect, predictors.at(i),true); 
	btn->connect(
	    boost::bind(&PredictorsToolbar::clickEvent, this, _1, _2));
	
	btn->state = 1;
	btn->toggle = 1;

	this->event_add_node(btn);

	this->addButton(btn);

	//
	col++;
	if (col == num_cols)
	{
	    col = 0;
	    x = x0;
	    y += -v_sep -btn_h;
	}
	else {
	    x += h_sep + btn_w;
	}
    }	

    //need to add a method to change the dataset

}

void 
PredictorsToolbar::addButton(Button* btn){
    this->predictors_btns.push_back(btn);
}

void 
PredictorsToolbar::installAt(rectanglelayout::Node *area)
{
    this->connectToRectangleSignal(area);

    int numberOfButtons = this->predictors_btns.size();
    for(int i = 0 ; i < numberOfButtons ; i++){
	Button* bt = this->predictors_btns.at(i);
	bt->connectToRectangleSignal(area);
    }

}

void 
PredictorsToolbar::render()
{
#ifdef DEBUG_PREDICTORTOOLBAR
    cout << "Render Predictors Toolbar" << endl;
#endif
    RenderSystem *renderSystem = Global::instance()->getRenderSystem();
    renderSystem->startViewport(screen_window);

    renderSystem->setColor(ColorConstants::TITLE_BG);
    renderSystem->drawQuad(screen_window.x,screen_window.y,screen_window.w,screen_window.h);

    renderSystem->endViewport();

    int numberOfButtons = this->predictors_btns.size();
#ifdef DEBUG_PREDICTORTOOLBAR
    cout << "Number of Buttons " << numberOfButtons << endl;
#endif
    for(int i = 0 ; i < numberOfButtons ; i++){
	Button* bt = this->predictors_btns.at(i);
	bt->render();
    }

}

void 
PredictorsToolbar::rectangleSignal(rectanglelayout::Rectangle r)
{
    screen_window.init(r.x,r.y,r.w,r.h);
}

bool 
PredictorsToolbar::event_mouse_contains(int x, int y)
{
#ifdef DEBUG_PREDICTORTOOLBAR
    cout << "PredictorsToolbar::event_mouse_contains(...) " << screen_window.contains(x,y)  << endl;
#endif

    return screen_window.contains(x,y);
}

/*************************
 * OccurrenceEnvironment *
 *************************/

OccurrenceEnvironment::OccurrenceEnvironment(int xcells, int ycells, int num_expressions):
    xcells(xcells),
    ycells(ycells),
    num_expressions(num_expressions)
{
    using namespace modules;

#ifdef DEBUG_OCCURRENCEENVIRONMENT
    cout << "OccurrenceEnvironment::OccurrenceEnvironment()" << endl;
#endif

    this->p_toolbar = NULL;

    //set default stencil flag to be true
    this->mod_stencil_flag.setInt(1);
    this->mod_interpolation_mode.setInt(1);

    // initialize expressions, colormaps, controllers, and windows
    for (int i=0;i<num_expressions;i++)
    {
	mod_expressions.push_back(new StringModule());

	ColormapController* cc = new ColormapController();

	// num expression
	if (num_expressions == 2 && i == 1)
	{
	    cc->min_value = -1.1;
	    cc->max_value =  1.1;
	    cc->another_colormap(+1);
	    cc->another_colormap(+1);

	    cc->used_colormaps[2]->setRange(-1,1);
	    Colormap1d *cmap = cc->used_colormaps[2];
	    float n = cmap->stops.size();
	    for (int j=0;j<cmap->stops.size();j++)
	    {
		Colormap1d::Stop *s = cmap->stops[j];
		s->value = 2 * j* 1 / (n-1) - 1;

	    }

	    // cc->addRegionsFromFile
	}

	ColormapRenderer*   cr = new ColormapRenderer();
	ScreenWindowModule* cw = new ScreenWindowModule();

	string var_name = "p";
	if (num_expressions > 1 && i == 1)
	    var_name = "dp";

	cr->getInputPort("title")->setData(new data::SingleString(var_name));
	
	cc->setRenderer(cr);

	mod_colormap_controllers.push_back(cc);
	mod_colormap_renderers.push_back(cr);
	mod_colormap_windows.push_back(cw);

	// wiring
    	new Connection(cw->getOutputPort("screen_window"),
    		       cc->getInputPort("screen_window"));

    	new Connection(cc->getOutputPort("colormap"),
    		       cr->getInputPort("colormap"));

    	new Connection(cc->getOutputPort("range"),
    		       cr->getInputPort("range"));
    
    	new Connection(cw->getOutputPort("screen_window"),
    		       cr->getInputPort("screen_window"));

	// register to receive events
	event_add_node(cc);

	
	// add plot controller
//	if(i == 0){
	PlotController*     pc = new PlotController();
	PlotRenderer*       pr = new PlotRenderer();
	ScreenWindowModule* pw = new ScreenWindowModule();

	string plot_title = "Occurrence (p)";
	if (i == 1)
	    plot_title = "Occurrence Variation (dp)";
	pr->getInputPort("title")->setData(new data::SingleString(plot_title));

    	new Connection(pw->getOutputPort("screen_window"),
    		       pr->getInputPort("screen_window"));
	
    	new Connection(this->mod_file.getOutputPort("file"),
    		       pr->getInputPort("file"));

	pc->setPlotRenderer(pr);

	plot_controllers.push_back(pc);
	mod_plot_renderers.push_back(pr);
	mod_plot_windows.push_back(pw);

	// register to receive events
	event_add_node(pc);

	// add curve data for the integral curves
	CurveData *cd = new CurveData(250,125);
	mod_integral_curves.push_back(cd);
	

    	new Connection(this->mod_file.getOutputPort("file"),
    		       cd->getInputPort("file"));

    	new Connection(cd->getOutputPort("curve"),
		       pr->getInputPort("curves"));
//	}

	// register to get add selection
	SelectionRepository::signal_connection_t connection_1 =  
	    selection_repository.connectToAddSelectionSignal(
		boost::bind(&OccurrenceEnvironment::receiveNewSelectionSignal, this, _1));

	// register to get 
	SelectionRepository::signal_connection_t connection_2 =  
	    selection_repository.connectToDelSelectionSignal(
		boost::bind(&OccurrenceEnvironment::receiveDeleteSelectionSignal, this, _1));

    }

    // the grid has xcells columns and ycells * num_expression rows
    // create array of MapCellInfrastructure
    for (int i=0;i<xcells;i++)
	for (int j=0;j<ycells * num_expressions;j++)
	{

#ifdef DEBUG_OCCURRENCEENVIRONMENT
	    cout << "   create cell infrastructure " << i << endl;
#endif
	 
	    int expression;
	    int time_offset;

	    string var_name = "p";
	    if (num_expressions > 1 && expression == 0)
		var_name = "dp";

	    // compute time offset and expression
	    cellInfo(i,j,time_offset,expression);  	    	    

	    MapCellInfrastructure *cellInfra = 
		new MapCellInfrastructure(
		    &mod_world_window,
		    mod_expressions[expression],
		    &mod_time_index,
		    mod_colormap_controllers[expression],
		    &mod_map_borders_mode,
		    &mod_interpolation_mode,
		    &mod_stencil_flag,
		    &mod_file,
		    &mod_longitude_latitude,
		    &mod_cursor_on_off,
		    &mod_habitat_tc_on_off,
		    &this->mod_tc_mask,
		    &selection_repository,
		    time_offset,
		    52,
		    var_name);

#ifdef DEBUG_OCCURRENCEENVIRONMENT
	    cout << "   cellInfo " << i << " " << j << endl;
	    cout << "   time_offset " << time_offset << endl;
	    cout << "   real_offset " << cellInfra->mod_offseted_time_index.getOffset() << endl;
#endif

	    
	    map_cells.push_back(cellInfra);

	    // register the zoom and pan controller as the top
	    event_add_node(&cellInfra->zoom_and_pan_controller);

#ifdef DEBUG_OCCURRENCEENVIRONMENT
	    cout << "Output Port offset_time_index " << cellInfra->mod_offseted_time_index.getOutputPort("output_int") << endl;
	    cout << "Expression " << expression << endl;
	    cout << "mod_plot_renderers[expression] " << mod_plot_renderers[expression] << endl;
	    cout << "Output Port time_indices " << mod_plot_renderers[expression]->getInputPort("time_indices")  << endl;
#endif

	    new Connection(cellInfra->mod_offseted_time_index.getOutputPort("output_int"),
			   mod_plot_renderers[expression]->getInputPort("time_indices"));
	}

    // wiring...
    {
    	new Connection(mod_title_window.getOutputPort("screen_window"),
    		       mod_title_renderer.getInputPort("screen_window"));

    	new Connection(mod_file.getOutputPort("file"),
    		       mod_info_renderer.getInputPort("file"));

    	new Connection(mod_info_window.getOutputPort("screen_window"),
    		       mod_info_renderer.getInputPort("screen_window"));

	new Connection(mod_longitude_latitude.getOutputPort("lonlat"),
		       mod_info_renderer.getInputPort("lonlat"));
    }

// wiring...
    {
//     	new Connection(mod_file.getOutputPort("file"),
//     		       mod_photo_renderer.getInputPort("file"));

    	new Connection(mod_photo_window.getOutputPort("screen_window"),
    		       mod_photo_renderer.getInputPort("screen_window"));
    }

    { // toolbar

	toolbar = new Toolbar(this);

	event_add_node(toolbar);

    }

}

void 
OccurrenceEnvironment::show_hide_global_integral_curves(int state)
{
    for (int i=0;i<mod_integral_curves.size();i++)
    {
	CurveData* curve = mod_integral_curves[i];
	curve->deleteAllConnectionsFromOrTo(mod_plot_renderers[i]);
    }
    if (state)
    {
	for (int i=0;i<mod_integral_curves.size();i++)
	{
	    CurveData* curve = mod_integral_curves[i];
	    new Connection(curve->getOutputPort("curve"),
			   mod_plot_renderers[i]->getInputPort("curves"));
	}
    }
}

void
OccurrenceEnvironment::cellInfo(int x, int y, int &time_offset, int &expression)
{
    int logical_row = y / num_expressions;
    int logical_col = x;
    expression = num_expressions - 1 - (y % num_expressions);
    time_offset = (ycells - 1 - logical_row) * xcells + logical_col; 
}

MapCellInfrastructure*
OccurrenceEnvironment::getMapCellInfrastructure(int xcell, int ycell)
{
    return map_cells[xcell * ycells + ycell];
}

void
OccurrenceEnvironment::installAt(rectanglelayout::Node *area)
{
    if (!area->isLeaf())
    {

#ifdef DEBUG_OCCURRENCEENVIRONMENT
	cout << "Cannot install Occurrence Environment in a non-leaf area";
#endif

	return;
    }

    rectanglelayout::RectangleLayout* rs = area->getRectangleLayout();

    // subdivide lead into multiple regions
    rectanglelayout::Grid* root    = rs->subdivide(area->asLeaf(),1,3);
    root->setYStop(1,0.06 * num_expressions);
    root->setYStop(2,0.75);

//     rectanglelayout::Node* top     = rs->getNode(root,0,3);
//     cout << "TOP POINTER " << top << endl << endl << endl << endl;
    rectanglelayout::Node* plots   = rs->getNode(root,0,2);
    rectanglelayout::Node* middle  = rs->getNode(root,0,1);
    rectanglelayout::Node* bottom  = rs->getNode(root,0,0);
    
    rectanglelayout::Grid* cells   = rs->subdivide(middle->asLeaf(),
						   xcells,
						   ycells * num_expressions);
    int k = 0;
    for (int i=0;i<xcells;i++)
	for (int j=0;j<ycells * num_expressions;j++)
	{
	    rectanglelayout::Node *cell_ij = rs->getNode(cells,i,j);
	    map_cells[k]->installAt(cell_ij);
	    k++;
	}

    rectanglelayout::Grid* cmaps = rs->subdivide(bottom->asLeaf(),
						 1, num_expressions);

    for (int i=0;i<num_expressions;i++)
    {
	rectanglelayout::Node *cmap_i = rs->getNode(cmaps,0,num_expressions-1-i);
	mod_colormap_windows[i]->connectToRectangleSignal(cmap_i);
    }

    // hack
    int aux = num_expressions;
    num_expressions = 1;

    // do not subdivide
    rectanglelayout::Grid* cplots = rs->subdivide(plots->asLeaf(),
						  num_expressions + 3
						  , 1);

    // if (num_expressions == 2)
    // 	cplots->setXStop(1,0.30);
    	
    cplots->setXStop(num_expressions,0.20);
    cplots->setXStop(num_expressions+1,0.44);
    cplots->setXStop(num_expressions+2,0.82);

    for (int i=0;i<num_expressions;i++)
    {
	rectanglelayout::Node *cplots_i = rs->getNode(cplots,i+2,0);
	mod_plot_windows[i]->connectToRectangleSignal(cplots_i);
	plot_controllers[i]->connectToRectangleSignal(cplots_i);
    }

    //photo cell
    cphoto = rs->getNode(cplots,0,0);
    mod_photo_window.connectToRectangleSignal(cphoto);

    //rectanglelayout::Node* cinfo = rs->getNode(cplots,num_expressions,0);
    cinfo = rs->getNode(cplots,1,0);//num_expressions,0);
    rectanglelayout::Node* ctoolbar = rs->getNode(cplots,num_expressions+2,0);
  
    // hack
    num_expressions = aux;

    //mod_title_window.connectToRectangleSignal(top);
    mod_info_window.connectToRectangleSignal(cinfo);
//    predictorsList = new ScrollPane();
    //predictorsList->installAt(cinfo);
//     predictorsList->setTitle("Species");
//     this->ss_container->installComponent( predictorsList, cinfo);

    // install toolbar
    toolbar->installAt(ctoolbar);


    // connect the environment with the big area
    connectToRectangleSignal(root);

}

OccurrenceEnvironment::~OccurrenceEnvironment(){
#ifdef DEBUG_OCCURRENCEENVIRONMENT
    cout << "Deleting Environment" << endl;
#endif

    //FileModule                            mod_file;

//     // expression modules
//     vector<StringModule*>                 mod_expressions;

//     // colormap: one for each expression
//     vector<ColormapController*>           mod_colormap_controllers;
//     vector<ColormapRenderer*>             mod_colormap_renderers;
//     vector<ScreenWindowModule*>           mod_colormap_windows;

//     // plots: one for each expression
//     vector<PlotController*>               plot_controllers;
//     vector<PlotRenderer*>                 mod_plot_renderers;
//     vector<ScreenWindowModule*>           mod_plot_windows;

//     // all cells
//     vector<MapCellInfrastructure* >       map_cells;

//     // integral curves: one for each expression
//     vector<CurveData* >                   mod_integral_curves;

//     // selections
//     SelectionRepository                   selection_repository;
//     map<Selection*, CurveData* >          map_selection_to_curve;

//     // toolbar
//     Toolbar*                              toolbar;
    
//     rectanglelayout::Node*                cinfo;
//     rectanglelayout::Node*                cphoto;

//     if(p_toolbar!= NULL)
//        delete p_toolbar

}

void         
OccurrenceEnvironment::render()
{
    for (int i=0;i<num_expressions;i++)
    {
	mod_colormap_renderers[i]->outdated = true;
	mod_colormap_renderers[i]->run();

	if(i == 0){
	mod_plot_renderers[i]->outdated = true;
	mod_plot_renderers[i]->run();
	}
	
    }

    int k=0;
    for (int i=0;i<xcells;i++)
	for (int j=0;j<ycells * num_expressions;j++)
	    map_cells[k++]->render();

    mod_title_renderer.outdated = true;
    mod_title_renderer.run();

    mod_info_renderer.outdated = true;
    mod_info_renderer.run();

    mod_photo_renderer.outdated = true;
    mod_photo_renderer.run();

    toolbar->render();
    if(p_toolbar != NULL)
	p_toolbar->render();

}

bool 
OccurrenceEnvironment::event_mouse_contains(int x, int y)
{

#ifdef DEBUG_OCCURRENCEENVIRONMENT
    cout << "OccurrenceEnvironment::event_mouse_contains( " << x 
	 << " , " << y << ")" << endl;
    cout << "   Bounding was " << screen_window.str() << endl;
#endif

    return screen_window.contains(x,y);

}

void 
OccurrenceEnvironment::rectangleSignal(rectanglelayout::Rectangle rect)
{

#ifdef DEBUG_OCCURRENCEENVIRONMENT
    cout << "OccurrenceEnvironment::rectangleSignal()" << endl;
    cout << "   Rectangle was " << rect << endl;
#endif

    screen_window.init(rect.x, rect.y, rect.w, rect.h);

}

void write_wf(dataflow::Module *m, string filename)
{
    // write python script fil
    ofstream myfile(filename.c_str());
    if (myfile.is_open())
    {
	myfile << 
	    "\
class Module(object):\n\
    def __init__(self, key, name):\n\
        self.key = key\n\
        self.name = name\n\
\n\
class Port(object):\n\
    def __init__(self, key, name, kind, module):\n\
        self.key = key\n\
        self.name = name\n\
        self.kind = kind\n\
        self.module = module\n\
\n\
class Connection(object):\n\
    def __init__(self, key, source, target):\n\
        self.key = key\n\
        self.source = source\n\
        self.target = target\n\
\n\
map_modules = {}\n\
map_ports = {}\n\
connections = []\n\
";
	myfile << m->dataflowDescription();

	myfile <<
	    "\n\n\
modules = map_modules.values()\n\
ports   = map_ports.values()\n\
";
	myfile.close();
    }
    else cout << "Unable to open file";
}

bool 
OccurrenceEnvironment::event_key_press(eventsystem::State *event_state) 
{

    //this->update_dataset(NULL);
    QString st;
    bool ok;
    int numTimeSteps;
    int state;   

//int numberOfMaps;

#ifdef DEBUG_OCCURRENCEENVIRONMENT
    cout << "OccurrenceEnvironment::event_key_press()" << endl;
#endif

    switch(event_state->key)
    {
    case (eventsystem::Key_Left):

#ifdef DEBUG_OCCURRENCEENVIRONMENT
	cout << "   left <--"  << endl;
#endif

	if(this->mod_file.fileLoaded()){
// 	    numTimeSteps = this->mod_file.getFile()->getNumTimeSteps();
// 	    mod_time_index.setInt((mod_time_index.getInt() + (numTimeSteps - 1)) % numTimeSteps);
	    int numberOfMaps = this->map_cells.size();
	    for(int i = 0 ; i < numberOfMaps ; i++){
		MapCellInfrastructure* m = this->map_cells[i];
		m->decrementOffsetSignal();
	    }
	}
	
	return true;

    case (eventsystem::Key_Right):

#ifdef DEBUG_OCCURRENCEENVIRONMENT
	cout << "   right -->"  << endl;
#endif

	if(this->mod_file.fileLoaded()){
	    //numTimeSteps = this->mod_file.getFile()->getNumTimeSteps();	    
	    //mod_time_index.setInt((mod_time_index.getInt() + 1) % numTimeSteps);


	    int numberOfMaps = this->map_cells.size();
	    for(int i = 0 ; i < numberOfMaps ; i++){
		MapCellInfrastructure* m = this->map_cells[i];
		m->incrementOffsetSignal();
	    }

	}

	return true;
	break;
    case eventsystem::Key_L:
    	// mod_colormap_controllers[0]->another_colormap(+1);
	this->mod_photo_renderer.toggleUseLogo();
    	break;
    case eventsystem::Key_M:
	mod_map_borders_mode.setInt(
	    (mod_map_borders_mode.getInt() + 1) % 3);
	return true;
	break;
    case eventsystem::Key_I:
	state = (mod_interpolation_mode.getInt() + 1) % 2;

	this->toolbar->toggleInterpolate(state);
	mod_interpolation_mode.setInt(state);
	return true;
	break;
    case eventsystem::Key_S:
	state = (mod_stencil_flag.getInt() + 1) % 2;

	this->toolbar->toggle_stencil(state);
	mod_stencil_flag.setInt(state);
	return true;
	break;
    case eventsystem::Key_W:
	write_wf(&mod_time_index, "bvwf.py");
	cout << "wrote file bvwf.py" << endl;
	//printCurrentREnvironment();
	return true;
	break;
    case eventsystem::Key_Backspace:	    
	st = QInputDialog::getText(NULL,//this->parent, 
				   "Expression",
				   "Value", 
				   QLineEdit::Normal,
				   mod_expressions[0]->getString().c_str(), 
				   &ok);
	if (ok && !st.isEmpty())
	{
	    mod_expressions[0]->setString(st.toStdString());
	    return true;
	}
	break;
    default:
	return false;
    }
    return false;
}

bool    
OccurrenceEnvironment::event_mouse_drop(eventsystem::State *event_state)
{    
    bvdata::Repository *repo = Global::instance()->getRepository();
    //repo->clearREnvironment();    

#ifdef DEBUG_OCCURRENCEENVIRONMENT
    cout << "OccurrenceEnvironment::event_mouse_drop()" << endl;
#endif

    eventsystem::PickableObject *po = event_state->picked_object;
    ListViewItem *lvi = dynamic_cast<ListViewItem*>(po);

    if (lvi == NULL)
    {

#ifdef DEBUG_OCCURRENCEENVIRONMENT
	cout << "   Could not cast to ListViewItem" << endl;
#endif

	return false;

    }

    bvdata::File *f;
    try {
	f = boost::any_cast<bvdata::File*>(lvi->getCustomObject());
    }
    catch (boost::bad_any_cast)
    {
	return false;
    }

    if (f == NULL)
	return false;    

    try{
	repo->removeFileFromMemory(this->mod_file.getFile());    
	f->load();
    }
    catch(bvdata::FileInvalidFormatException e){
	e.report();
	return true;
    }

    mod_file.setFile(f);
    

    {//load photo texture
	//f->thumb_photo_file = "wood_thrush_thumb_2.jpg";	
	//f->thumb_photo_file = "images/indigo_bunting.jpg";
	this->mod_photo_renderer.load(f->thumb_photo_file);	
    }



    //
#ifdef DEBUG_OCCURRENCEENVIRONMENT
    cout << "    Species: "+f->getName() << endl;
#endif

    mod_title_renderer.setTitle("Species: "+f->getName());   

    if (num_expressions > 0)
    {
	//string expression = f->shortname+"$p[[t]]";
	string expression = "map";
	mod_expressions[0]->setString(expression);

	mod_integral_curves[0]->setExpression(
	    "bv.curve("+f->shortname+"$p,xmin,xmax,ymin,ymax)");


	double scale = repo->getCurveMaximum(f, "p",  1,  f->xcells, 1,  f->ycells);
	
#ifdef DEBUG_OCCURRENCEENVIRONMENT
	cout << "   Max Value = " << scale << endl;
#endif

	ViewRect v = mod_plot_renderers[0]->getWorldWindow();
	
	v.x = -1;
	v.w = 370;

	// v.y = (-0.08)*scale;
	// v.h = scale * 1.1;
	v.y = ColorConstants::NORMALIZE_CURVES_MIN_PLOT_Y;
	v.h = ColorConstants::NORMALIZE_CURVES_MAX_PLOT_Y - ColorConstants::NORMALIZE_CURVES_MIN_PLOT_Y;

	mod_plot_renderers[0]->setWorldWindow(v);
    }
    if (num_expressions > 1)
    {
//	string expression = 
// 	    f->shortname+"$p[[t]] - " + 
// 	    f->shortname+"$p[[ if(t==1) t else t-1 ]]";
	string expression = "variation";
	mod_expressions[1]->setString(expression);

	//int numTimeSteps = f->getNumTimeSteps();

// 	stringstream ss;
// 	ss << "c=bv.curve(" << f->shortname <<
// 	    "$p,xmin,xmax,ymin,ymax);c[2:" << 
// 	    numTimeSteps << "]=c[2:" << 
// 	    numTimeSteps << "]-c[1:" << (numTimeSteps - 1) << "];c";
// 	mod_integral_curves[1]->setExpression(ss.str());
    }

    {
#ifdef DEBUG_OCCURRENCEENVIRONMENT
	cout << "   update the number of x_samples and y_samples" << endl;
	cout << "   f->xcells " << f->xcells << endl;
	cout << "   f->ycells " << f->ycells << endl;

#endif

	//update the number of x_samples and y_samples 
	int numberOfCurves = mod_integral_curves.size();//this->selection_repository.getNumberOfSelections();

	for(int i = 0 ; i < numberOfCurves ; i++){
	    
	    CurveData *cd = mod_integral_curves.at(i);//map_selection_to_curve[s];
		cd->setXSamples(f->xcells);
		cd->setYSamples(f->ycells);
		cd->setExpression("bv.curve(" + f->shortname + "$p,xmin,xmax,ymin,ymax)");
	    
	}
    }


    {//set predictors tool bar
//     if(this->p_toolbar != NULL)
// 	delete p_toolbar;
    
    vector<string> predictorsTags;
    vector<int>    mask;
    int numberOfPredictors = f->getNumPredictors();

    for(int i = 0 ; i < numberOfPredictors ; i++){
	predictorsTags.push_back(f->getPredictor(i).tag);
	mask.push_back(1);
    }

//     p_toolbar = new PredictorsToolbar(this);
//     p_toolbar->init(predictorsTags);
//     p_toolbar->installAt(cinfo);
//     event_add_node(p_toolbar);
     this->mod_tc_mask.init(mask);
    }


    //set something
    int numTimeSteps = f->getNumTimeSteps();
    for(int i = 0 ; i < this->xcells ; i++){
	for(int j = 0 ; j < this->ycells ; j++){
	    MapCellInfrastructure* mci = getMapCellInfrastructure(i,j);
	    mci->setMaxTimeOffset(numTimeSteps);
	}
    }

#ifdef DEBUG_OCCURRENCEENVIRONMENT
	cout << "   finish drop event" << endl;
#endif


    return true;
}

void 
OccurrenceEnvironment::receiveNewSelectionSignal(Selection *sel)
{

#ifdef DEBUG_OCCURRENCEENVIRONMENT
    cout << "OccurrenceEnvironment::receiveNewSelectionSignal(...)" << endl;
#endif

    if(this->mod_file.fileLoaded()){

	bvdata::File* f = this->mod_file.getFile();

	CurveData *curveData = new CurveData(f->xcells, f->ycells);
	
	//mod_integral_curves.push_back(curveData);
	
	new Connection(sel->getOutputPort("selection"),
		       curveData->getInputPort("selection"));

	new Connection(this->mod_file.getOutputPort("file"),
		       curveData->getInputPort("file"));

	if (mod_file.getFile() != NULL)
	{
	    curveData->setExpression(
		"bv.curve(" + mod_file.getFile()->shortname + "$p,xmin,xmax,ymin,ymax)");

	    new Connection(curveData->getOutputPort("curve"),
			   mod_plot_renderers[0]->getInputPort("curves"));
    
	    // add sel->curveData into map
	    map_selection_to_curve[sel] = curveData;
	}

	// new Connection(mod_expressions[0]->getOutputPort("expression"),
	// 		   curveData->getInputPort("expression"));

    }
    
    // new Connection(sel->getOutputPort("selection"),
    // 		   curveData->getInputPort("selection"));
    //mod_map_selections_renderer.outdated = true;
}


void 
OccurrenceEnvironment::receiveDeleteSelectionSignal(Selection *sel)
{

#ifdef DEBUG_OCCURRENCEENVIRONMENT
    cout << "OccurrenceEnvironment::receiveDeleteSelectionSignal(...)" << endl;
    cout << "   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! I am deleting...." << endl;
#endif

    if (map_selection_to_curve.count(sel) > 0)
    {
#ifdef DEBUG_OCCURRENCEENVIRONMENT
	cout << "   erasing curve" << endl;
#endif
	CurveData *c = map_selection_to_curve[sel];
	mod_plot_renderers[0]->deleteAllConnectionsFrom(c);
	
	this->mod_file.deleteAllConnectionsFromOrTo(c);

	c->deleteAllConnectionsFrom(sel);
	map_selection_to_curve.erase(sel);
	delete c;

    }
}

void 
OccurrenceEnvironment::toggle_interpolate(int state){
//     int curInterpMode = this->mod_interpolation_mode.getInt();
//     int newInterpMode = (curInterpMode+1)%2;

    this->mod_interpolation_mode.setInt(state);

}

void
OccurrenceEnvironment::open_pie_chart(){
    cout << "OccurrenceEnvironment::open_pie_chart()" << endl;

	if(this->mod_file.fileLoaded()){
	    bvdata::File* file = this->mod_file.getFile();
	    
	    string pieChartFile = file->pie_chart_file;

	    if(pieChartFile.compare("") != 0){
		Global::instance()->open_image_file(pieChartFile);	    
	    }
	}
}

void 
OccurrenceEnvironment::toggle_stencil(int state){
    this->mod_stencil_flag.setInt(state);
}

void 
OccurrenceEnvironment::change_tc_mask(int state, int index){
    this->mod_tc_mask.setValueAt(index, state);
}

