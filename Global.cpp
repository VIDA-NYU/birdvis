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

#include <unistd.h>

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
    cout << "R_HOME = " << pathvar << endl;

    render_system = NULL;

    repository    = new bvdata::Repository();

    char buffer[1000];
    getcwd(buffer,999);
    string directory(buffer);
    directory = directory + "/data";
    cout << "   data directory: " << directory << endl;
    repository->addFilesFromDirectory(directory);

    //
    usa_counties = NULL;
    usa_states   = NULL;

    // keep texture of logo in GL memory
    this->bv_logo = new Texture();
    this->bv_logo->load("resources/bvlogo.png");
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

void 
Global::initializeRenderSystem(string path)
{
    this->render_system = new rendersystem::RenderSystem(path);
}

void 
Global::initializeUSAMapsAndInstallCallLists(string path)
{
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
    //
    cout << "estimateUSAreaAtRectangle(...)" << endl;


    // assuming usa_are_gl_list is on
    GLuint framebuffer, texture;
    GLenum status;

    // what is the right aspect ratio
    double ar = w / h;
    
    // compute width and height to have almost 1M pixels
    // TH * ar * TH = 1M
    // TH^2 = 1M/ar
    // TH   = sqrt(1M/ar)
    // TW   = TH*ar
    int TEXHEIGHT = (int)   sqrt(1e6 / ar);
    int TEXWIDTH  = (int) ( TEXHEIGHT * ar );

    //
    unsigned char buffer[4 * TEXWIDTH * TEXHEIGHT]; // 2M bytes for this calculation allways allocated

    //
    cout << "   TEXWIDTH x TEXHEIGHT = " << TEXWIDTH << " x " << TEXHEIGHT << " -----> " 
	 << (TEXWIDTH * TEXHEIGHT) << endl;

    //
    if (TEXWIDTH == 0 || TEXHEIGHT == 0)
	return 0.0;

    // create texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXWIDTH, TEXHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, TEXWIDTH, TEXHEIGHT, 
    // 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    //
    glGenFramebuffersEXT(1, &framebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);

    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
    	cout << "    Problem. Error code: " << status << endl;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    // GLenum errCode;
    // const GLubyte *errString;
    // if ((errCode = glGetError()) != GL_NO_ERROR) {
    // 	errString = gluErrorString(errCode);
    // 	fprintf (stderr, "OpenGL Error: %s\n", errString);
    // }

    // Handle error here
    // Your code to draw content to the FBO
    // ...
    // Make the window the target

    // cout << "   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);" << endl;
    //Your code to use the contents of the FBO


    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

    glViewport(0,0,TEXWIDTH, TEXHEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(x,x+w,y,y+h,-10, 10);
    // glOrtho(0,1,0,1,-10, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // turn on stencil

    // glClearStencil(0);
    glClearColor(0,0,0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glColor4f(1.0,1.0,1.0,1.0);   //(1.0,1.0,1.0,1.0);
    // glColor3i(255,255,255);   //(1.0,1.0,1.0,1.0);
    glCallList(usa_area_gl_list); // turn on stencil polygons

    // double x0 = x + w/2.0;
    // double y0 = y + h/2.0;
    // double x1 = x + w;
    // double y1 = y + h;

    // double x0 = 0;
    // double y0 = 0;
    // double x1 = 0.5;
    // double y1 = 0.5;

    // glColor3f(1.0,1.0,1.0);
    // glBegin(GL_QUADS);
    // glVertex2f(x0, y0);
    // glVertex2f(x1, y0);
    // glVertex2f(x1, y1);
    // glVertex2f(x0, y1);
    // glEnd();

    // glFlush();


    glReadPixels(0,0,TEXWIDTH,TEXHEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,buffer); 
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    glDeleteTextures(1, &texture);
    glDeleteFramebuffersEXT(1, &framebuffer);

    // glEnable(GL_STENCIL_TEST);
    // glStencilFunc(GL_ALWAYS, 0x0, 0xff);
    // glStencilOp(GL_INCR, GL_INCR, GL_INCR);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glColorMask(0,0,0,0);


    // for (int i=0;i<TEXHEIGHT;i++)
    // {
    // 	for (int j=0;j<TEXHEIGHT;j++)
    // 	{
    // 	    printf("%02x ", buffer[i * TEXWIDTH * 4 + 4 * j]);
    // 	    // cout << buffer[i]    << endl;
    // 	}
    // 	cout << endl;
    // }

    // glEnable(GL_STENCIL_TEST);
    // glStencilFunc(GL_ALWAYS, 0x0, 0xff);
    // glStencilOp(GL_INCR, GL_INCR, GL_INCR);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glColorMask(0,0,0,0);
    // glColorMask(1,1,1,1);

    //


    // draw quad
    // glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
    // glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);

    //
    // glColor3f(1.0,1.0,1.0);
    // glBegin(GL_QUADS);
    // glVertex2f(x,y);
    // glVertex2f(x+w,y);
    // glVertex2f(x+w,y+h);
    // glVertex2f(x,y+h);
    // glEnd();

    // disable
    // glDisable(GL_STENCIL_TEST);
    
    // 




    //


    int sum = 0;
    for (int i=0;i<TEXHEIGHT * TEXWIDTH * 4;i+=4)
	sum += (buffer[i] == 0 ? 0 : 1);

    double coef = ((double) sum) / (TEXHEIGHT * TEXWIDTH);

    cout << "   coef = " << coef << endl;

    return coef;


    // return coef * w * h;
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


void 
Global::keyword_bounds(string keywords, float *xywh)
{
    // tokenizer
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("=[];,:\" \n");

    int position = keywords.find(" "); // find first space
    while ( position != string::npos ) 
    {
	keywords.replace( position, 1, "." );
	position = keywords.find( " ", position + 1 );
    } 


    char cmd_template[] = "wget http://maps.google.com/maps/geo?q=%s -O - "
	"| egrep \"coordinates|north|south|east|west\"";
    char cmd_instance[2048];
    sprintf(cmd_instance,cmd_template,keywords.c_str());

    char buffer[2048];
    FILE *fp = popen(cmd_instance,"r");

    vector<string> tokens;

    float north, south, east, west, lon, lat;

    while (fgets(buffer, 2048, fp) != NULL)
    {
    	string s(buffer);

	// clear
	tokens.clear();

	// tokens
	tokenizer tk(s, sep);
	for (tokenizer::iterator tok_iter = tk.begin();tok_iter != tk.end(); ++tok_iter)
	{
	    std::cout << "<" << *tok_iter << "> ";
	    tokens.push_back(*tok_iter);
	}
	std::cout << "\n";

	if (tokens.size() > 0)
	{
	    string key = tokens.at(0);
	    if (key == "north")
		sscanf(tokens.at(1).c_str(),"%f",&north);
	    else if (key == "south")
		sscanf(tokens.at(1).c_str(),"%f",&south);
	    else if (key == "east")
		sscanf(tokens.at(1).c_str(),"%f",&east);
	    else if (key == "west")
		sscanf(tokens.at(1).c_str(),"%f",&west);
	    else if (key == "coordinates")
	    {
		sscanf(tokens.at(1).c_str(),"%f",&lon);
		sscanf(tokens.at(2).c_str(),"%f",&lat);
	    }
	}
    	// printf("%s", buffer);
    }
    pclose(fp);

    //
    printf("coordinates: %f, %f\n",lon,lat);
    printf("north: %f\n",north);
    printf("south: %f\n",south);
    printf("east: %f\n",east);
    printf("west: %f\n",west);

    float cx = (east+west)/2.0;
    float cy = (south+north)/2.0;
    float w  = fabs(east-west);
    float h  = fabs(north-south);
    float side = max(w,h);

    xywh[0] = cx - side/2.0;
    xywh[1] = cy - side/2.0;
    xywh[2] = side;
    xywh[3] = side;
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

#ifdef __APPLE__
    char url_template[]="open \"http://maps.google.com/maps?q=BirdVis@%f,%f&ll=%f,%f&spn=%f,%f\"";
#else
    char url_template[]="firefox \"http://maps.google.com/maps?q=BirdVis@%f,%f&ll=%f,%f&spn=%f,%f\"";
#endif

    char url_instance[200];
    sprintf(url_instance,url_template,lat,lon,lat,lon,latspan,lonspan);
    cout << url_instance << endl;
    system(url_instance);
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
    cout << "Global::open_image_file()" << endl;

    string command;

#ifdef __APPLE__
    command = "open ";
#else
    command = "open "; //TODO
#endif

    command = command + filename;

    system(command.c_str());

    //TODO get exception and report if the file does not exist
}
