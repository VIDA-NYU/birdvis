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

#include "Global.hpp"
#include "AreaEstimator.hpp"
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <QFileInfo>
#include <QMessageBox>

#include <unistd.h>
#include <QDir>

#include <Util.hpp>

/*************
 * Constants *
 *************/
float   ColorConstants::SEL_TRANSPARENCY  = 0.3;

bool    ColorConstants::NORMALIZE_CURVES  = true;

float   ColorConstants::NORMALIZE_CURVES_MIN_PLOT_Y = -0.01;
float   ColorConstants::NORMALIZE_CURVES_MAX_PLOT_Y =  0.22;

Color*  ColorConstants::COLORMAP_BG      = new Color(.35,.35,.35);
Color*  ColorConstants::TITLE_BG         = new Color(.35,.35,.35);
Color*  ColorConstants::TITLE_TEXT       = new Color(1,1,1);
Color*  ColorConstants::PLOT_BG          = new Color(.9,.9,.9);
Color*  ColorConstants::PLOT_GRID        = new Color(.8,.8,.8);
Color*  ColorConstants::PLOT_TEXT        = new Color(.3,.3,.3);
Color*  ColorConstants::PLOT_CURSORS     = new Color(.3,.3,.3);
Color*  ColorConstants::PLOT_TITLE_BG    = new Color(.2,.2,.2);
Color*  ColorConstants::PLOT_TITLE_TEXT  = new Color(.1,.1,.1);
Color*  ColorConstants::PLOT_FRAME_BG    = new Color(.9,.9,.9,.9);
Color*  ColorConstants::MAP_BG           = new Color(.8,.8,.8);
Color*  ColorConstants::MAP_BORDERS      = new Color(.70,.70,.70);
					 
Color*  ColorConstants::MAP_BTN_BG       = new Color(.9,.9,.9,.4);
Color*  ColorConstants::MAP_BTN_TEXT     = new Color(.5,.5,.5);
Color*  ColorConstants::MAP_BTN_ON       = new Color(.5,.5,.5);
Color*  ColorConstants::MAP_BTN_OFF      = new Color(.0,.5,.0);




/**********
 * Global *
 **********/

Global *Global::s_instance = NULL;

// const Color Global::COLORMAP_BG = Color(0.5,0.5,0.5);

Global::~Global()
{
}

Global::Global()
{
    cout << "Global::Global()" << endl;
    char *pathvar = getenv("R_HOME");
    cout << "R_HOME = " << (pathvar ? pathvar : "(unset)") << endl;

    render_system = NULL;

    repository    = new bvdata::Repository();

    string directory = qEnvironmentVariable("BIRDVIS_DATA", BIRDVIS_DATA_DIR).toStdString();
    cout << "   data directory: " << directory << endl;
    repository->addFilesFromDirectory(directory);

    //
    usa_counties = NULL;
    usa_states   = NULL;

    // keep texture of logo in GL memory
    this->bv_logo = new Texture();
    this->bv_logo->load(QDir(qEnvironmentVariable("BIRDVIS_RESOURCES", BIRDVIS_RESOURCE_DIR)).filePath("bvlogo.png").toStdString());
    // this->bv_logo->load("birdvis.app/Contents/Resources/logo.jpg");
    // this->bv_logo->load("birdvis.app/Contents/Resources/x.png");
}

Global*
Global::instance()
{
    if (!s_instance)
    {
	s_instance = new Global();
    }
    return s_instance;
}

// ViewRect 
// Global::xy_ranges()
// {
//     ViewRect vr;
//     double x = x_coords->getValueAt(0);
//     double w = x_coords->getValueAt(x_coords->getDimensionSize(0)-1) - x;
//     double y = y_coords->getValueAt(0);
//     double h = y_coords->getValueAt(y_coords->getDimensionSize(0)-1) - y;
//     vr.init(x,y,w,h);
//     return vr;
// }

void Global::releaseGLResources() {
    delete render_system;
    render_system=nullptr;
    for(GLuint id : {usa_states_gl_list, usa_counties_gl_list, usa_area_gl_list})
        if(id) glDeleteLists(id,1);
    usa_states_gl_list=usa_counties_gl_list=usa_area_gl_list=0;
}

void 
Global::initializeRenderSystem(string path)
{
    this->render_system = new rendersystem::RenderSystem(path);
}

void 
Global::initializeUSAMapsAndInstallCallLists(string path)
{
    delete usa_states;
    delete usa_counties;
    usa_states   = new RegionSet();
    usa_states->addRegionsFromFile(path+"usa_st.shp", "usa_st", 0x2);
    usa_counties = new RegionSet();
    usa_counties->addRegionsFromFile(path+"counties.shp", "counties", 0x3);

    // store in open gl: usa_states_gl_list
    usa_states_gl_list = glGenLists(1);
    glNewList(usa_states_gl_list, GL_COMPILE);
    glColor4f(
	ColorConstants::MAP_BORDERS->r,
	ColorConstants::MAP_BORDERS->g,
	ColorConstants::MAP_BORDERS->b,
	ColorConstants::MAP_BORDERS->a);
    usa_states->gl_draw_all_region_lines();
    glEndList();

    // store in open gl: usa_counties_gl_list
    usa_counties_gl_list = glGenLists(1);
    glNewList(usa_counties_gl_list, GL_COMPILE);
    glColor4f(
	ColorConstants::MAP_BORDERS->r,
	ColorConstants::MAP_BORDERS->g,
	ColorConstants::MAP_BORDERS->b,
	ColorConstants::MAP_BORDERS->a);
    usa_counties->gl_draw_all_region_lines();
    glEndList();

    // store in open gl: usa_areas_gl_list
    usa_area_gl_list = glGenLists(1);
    glNewList(usa_area_gl_list, GL_COMPILE);
    glColor4f(1,1,1,1);
    usa_states->gl_draw_all_region_triangles();
    glEndList();


    // test estimateUSAreaAtRectangle

    util::ClockTimer ct;
    ct.start();
    this->estimateUSAreaCoefficientAtRectangle(-125, 25, 58,25);
    double t = ct.elapsed();
    printf("this->estimateUSAreaAtRectangle(-125, 25, 58,25)  took %.6f sec\n", t);
}

double 
Global::estimateUSAreaCoefficientAtRectangle(double x, double y, double w, double h)
{
    return estimateAreaFraction(usa_area_gl_list, x, y, w, h);
}

string 
Global::getRegionName(double lon, double lat)
{
    string region_name("---");
    RegionSet *r = usa_counties;
    for (int i=0;i<r->size();i++)
    {
	OGRGeometry *g = r->getRegion(i);
	OGRPoint p;
	p.setX(lon);
	p.setY(lat);
	p.setZ(0.0);
	
	// printf("test if %10.4f %10.4f is in %s\n",lon,lat,r->getName(i).c_str());
	if (g->Contains(&p))
	{
	    // printf("YES!!!\n");
	    region_name = r->getName(i);
	    break;
	}
    }
    return region_name;
}

		// RegionSet *region_set = region_sets[0];


		// int numLocations = loc->getNumLocations();
		// us_map_disp_list = glGenLists(1);

		// glNewList(us_map_disp_list, GL_COMPILE);

		// glLineWidth(1.0);
		// glColor3f(1.0,1.0,1.0);

		// for (int i=0;i<region_set->regions.size();i++)
		// {
		//     string name = region_set->names.at(i);
		//     size_t found = name.find("Alaska");
		//     if (found == string::npos) {
		// 	found = name.find("Hawaii");
		// 	if (found == string::npos) {
		// 	    OGRGeometry *g = region_set->regions.at(i);
		// 	    gl_draw_geometry(g);
		// 	}
		//     }
		// }
	    
		// glEndList();


    // void init_stencil_buffer_map(){
    // 	glClearStencil(0);
    // 	glClear(GL_STENCIL_BUFFER_BIT);
    // 	glEnable(GL_STENCIL_TEST);
    // 	glStencilFunc(GL_ALWAYS, 0x0, 0xff);
    // 	glStencilOp(GL_INCR, GL_INCR, GL_INCR);

    // 	//cout << local_region_set_index << endl;

    // 	    //draw to stencil

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
    // 	glDisable(GL_STENCIL_TEST);
    // }



rendersystem::RenderSystem*
Global::getRenderSystem()
{
    return this->render_system;
}

bvdata::Repository* 
Global::getRepository()
{
    return this->repository;
}


bool
Global::keyword_bounds(string keywords, float *xywh) {
    const QString query=QString::fromStdString(keywords).trimmed();
    for (RegionSet *set : {usa_states, usa_counties}) {
        for(int i=0;i<set->size();++i) {
            if (QString::fromStdString(set->getName(i)).compare(query,Qt::CaseInsensitive)!=0) continue;
            OGREnvelope bounds;
            set->getRegion(i)->getEnvelope(&bounds);
            const double side=1.1*std::max(bounds.MaxX-bounds.MinX,bounds.MaxY-bounds.MinY);
            xywh[0]=(bounds.MaxX+bounds.MinX-side)/2;
            xywh[1]=(bounds.MaxY+bounds.MinY-side)/2;
            xywh[2]=xywh[3]=side;
            return true;
        }
    }
    QMessageBox::information(main_widget,"Place not found",
        "Enter a state name (Florida) or a county and state (Graham, Kansas). City lookup is not available.");
    return false;
}

void 
Global::open_google_maps_link(ViewRect ww)
{

    float ar = ww.w/ww.h;

    double lon = ww.x + ww.w/2.0;
    double lat = ww.y + ww.h/2.0;

    // double lon = ww.x + ww.w/2.0;
    // double lat = ww.y + ww.h/2.0;

    double lonspan = ww.w;
    double latspan = ww.h;

    // if (ar > 1)
    // {
    // 	lonspan /= ar;
    // 	latspan /= ar;
    // }
    // else {
    // 	lonspan *= ar;
    // 	latspan *= ar;
    // }

// http://maps.google.com/maps/geo?ll=40.6822,-111.896439&spn=0.380636,0.891953
// http://maps.google.com/maps?f=q&source=s_q&hl=en&geocode=&q=Utah&aq=0&sll=-90,40.66074&sspn=0.245168,18.786621&ie=UTF8&hq=&hnear=Utah&t=h&z=7
// http://maps.google.com/?ie=UTF8&q=birds&t=h&hl=en&hq=birds&hnear=Salt+Lake+City,+Salt+Lake,+Utah&split=1&sll=-90,40.66424&sspn=1.82603,1.82603&ll=40.6822,-111.896439&spn=0.380636,0.891953&z=11
// http://maps.google.com/?ie=UTF8&q=birds&ll=%.5f,%.5f&spn=%.5f,%.5f&t=h&z=8

    QUrl url("https://www.google.com/maps");
    QUrlQuery query;
    query.addQueryItem("q",QString::number(lat,'f',6)+","+QString::number(lon,'f',6));
    url.setQuery(query);
    QDesktopServices::openUrl(url);
}

// void search(string st)
// {
//     float bounds[4];
//     keyword_bounds(st,bounds);
//     if (active_panel != NULL)
//     {
// 	active_panel->mod_map_controller.
// 	    set_world_region(bounds[0],bounds[1],bounds[2],bounds[3]);
//     }
// }

void                         
Global::open_image_file(string filename){
    const QFileInfo image(QString::fromStdString(filename));
    if (!image.isFile()) {
        QMessageBox::information(main_widget,"Image unavailable", "This dataset's image was not included: " + image.fileName());
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(image.absoluteFilePath()));
}
