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

#include "modules.hpp"
#include <string>

using namespace dataflow;
using namespace std;
using namespace datatypes;

namespace modules { // modules namespace


    /***************
     * PanelModule *
     ***************/

    PanelModule::PanelModule(): 
	Module("PanelModule")
    {

	this->addOutputPort("species_time_indices");
	this->addOutputPort("dataset_array");

	this->addOutputPort("dataset_repository");
	this->addOutputPort("dataset_index");

	this->addOutputPort("x_coords");
	this->addOutputPort("y_coords");
	this->addOutputPort("screen_window_0");

	this->addOutputPort("map_screen_window");
	this->addOutputPort("colormap_screen_window");
	this->addOutputPort("integrals_screen_window");
	this->addOutputPort("title_screen_window");
	this->addOutputPort("predictors_screen_window");

	this->addOutputPort("species_names");

	this->addOutputPort("font");
	this->addOutputPort("font_texture_id");

	// the state of a panel
	this->addOutputPort("expression");
	this->addOutputPort("time_index");
	this->addOutputPort("files");

	//
	this->addOutputPort("species_indices");

	// 
	this->addOutputPort("time_coords");
    }

    void
    PanelModule::setDataset(datatypes::Array *dataset_array, 
			    datatypes::Array *x_coords, 
			    datatypes::Array *y_coords)
    {
	Port *p;

	p= this->getOutputPort("dataset_array");
	p->setData(new data::Array(dataset_array));
	p->outdateDownstream();

	p = this->getOutputPort("x_coords");
	p->setData(new data::Array(x_coords));
	p->outdateDownstream();

	p = this->getOutputPort("y_coords");
	p->setData(new data::Array(y_coords));
	p->outdateDownstream();
    }

    void 
    PanelModule::setSpeciesAndTimeIndices(int species, int time)
    {
	Port *p = this->getOutputPort("species_time_indices");
	p->setData(new data::Vector<int>(species,time));
	p->outdateDownstream();
    }

    void 
    PanelModule::process()
    {}


    /*************************
     * ScalarField2DRenderer *
     *************************/

    ScalarField2DRenderer::ScalarField2DRenderer(): Module("ScalarField2DRenderer")
    {
	// input ports
	this->addInputPort("grid");
	this->addInputPort("texture");

	this->addInputPort("default");
	this->addInputPort("colormap");
	this->addInputPort("bool_interpolate");

	// output ports
	this->addOutputPort("default");

	//configure auxiliar texture
	// glGenTextures(1,&interpolation_tex);
	// glBindTexture(GL_TEXTURE_2D, interpolation_tex);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LAMP);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    void ScalarField2DRenderer::process()
    {
	cout << "ScalarField2DRenderer::process()" << endl;

	datatypes::Grid *grid;
	if (!data::SingleGridPtr::getInputValue(this,"grid",&grid))
	    return;

	// cout << "   grid.......OK" << endl;

	Colormap1d *colormap;
	if (!data::SingleColormap1dPtr::getInputValue(this,"colormap",colormap))
	    return;

	// cout << "   colormap...OK" << endl;


	// bool interpolate;
	// if (!data::Bool::getInputValue(this,"bool_interpolate",interpolate))
	// 	return;

	// cout << "   interpolate.......OK" << endl;


	// cout << y_coords->str() << endl ;
	int x_samples = grid->xcells;
	int y_samples = grid->ycells;

	// cout << " ==========> " << x_samples << " " << y_samples << endl;
	// cout << array->str() << endl ;

	// if(!interpolate){	    

	double quad_x = grid->quad.x;
	double quad_y = grid->quad.y;
	double cw = grid->quad.w / x_samples;
	double ch = grid->quad.h / y_samples;

	// cout << "cw ch" << cw << " " << ch << endl;

	glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glBegin(GL_QUADS); //starts drawing of points
	for (int i=0;i<y_samples;i++)
	{

	    for (int j=0;j<x_samples;j++)
	    {

		double value = grid->getValue(j,i);
		double cx, cy;

		cx = quad_x + j * cw;
		cy = quad_y + i * ch;
		    
		if (colormap != NULL)
		{
		    Color c = colormap->getColor(value);
		    glColor4f(c.r,c.g,c.b,c.a);
		}
		else {
		    glColor3f(value,value,0);
		}
		    
		glVertex2f( cx      , cy + ch );
		glVertex2f( cx      , cy      );
		glVertex2f( cx + cw , cy      );
		glVertex2f( cx + cw , cy + ch );
		    
	    }
	} // show_grid == 1
	glEnd();//end drawing of points
	glPopAttrib();
		

	// }
	// else{
	// 	int width = x_samples;
	// 	int height = y_samples;

	// 	GLuint interpolation_tex = 
	// 	(dynamic_cast<data::Integer* > (getInputPort("texture")->getData()))->value;

	// 	double x_min = 
	// 	(dynamic_cast<data::Double* > (getInputPort("x_coords_min")->getData()))->value;


	// 	double x_max = 
	// 	(dynamic_cast<data::Double* > (getInputPort("x_coords_max")->getData()))->value;


	// 	double y_min = 
	// 	(dynamic_cast<data::Double* > (getInputPort("y_coords_min")->getData()))->value;


	// 	double y_max = 
	// 	(dynamic_cast<data::Double* > (getInputPort("y_coords_max")->getData()))->value;


	// 	GLubyte textureData[height][width][3];
	// 	//build texture
	// 	int c;
	// 	for (int i = 0; i < height; i++) {
	// 	    for (int j = 0; j < width; j++) {			
	// 		double cx, cy;			
	// 		double value = array->getValueAt(j,i);
	// 		cx = x_coords->getValueAt(j);
	// 		cy = y_coords->getValueAt(i);
			
	// 		double r,g,b;
	// 		if (colormap != NULL)
	// 		{
	// 		    Color c = colormap->getColor(value);
	// 		    //glColor4f(c.r,c.g,c.b,c.a);
	// 		    r = c.r;
	// 		    g = c.g;
	// 		    b = c.b;
	// 		}
	// 		else {
	// 		    //glColor3f(value,value,0);
	// 		    r = value;
	// 		    g = value;
	// 		    b = 0;
	// 		}

	// 		textureData[i][j][0] = (GLubyte) (255 * r); 
	// 		textureData[i][j][1] = (GLubyte) (255 * g);
	// 		textureData[i][j][2] = (GLubyte) (255 * b);
	// 	    }
	// 	}

	// 	glPushAttrib(GL_TEXTURE_BIT);
	// 	glBindTexture(GL_TEXTURE_2D, interpolation_tex);
	// 	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// 	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, 
	// 		     height, 0, GL_RGB, 
	// 		     GL_UNSIGNED_BYTE, &textureData[0][0][0]);
		

	// 	glEnable(GL_TEXTURE_2D);
	// 	glBegin(GL_QUADS);
	// 	glTexCoord2f(0,0);
	// 	glVertex2f(x_min,y_min);

	// 	glTexCoord2f(1,0);
	// 	glVertex2f(x_max,y_min);

	// 	glTexCoord2f(1,1);
	// 	glVertex2f(x_max,y_max);

	// 	glTexCoord2f(0,1);
	// 	glVertex2f(x_min,y_max);
	// 	glEnd();

	// 	glDisable(GL_TEXTURE_2D);
	// 	glBindTexture(GL_TEXTURE_2D, 0);
	// 	glPopAttrib();


	// }

	glDisable(GL_STENCIL_TEST);

    } // show colormap
    

    /*******************
     * GraphController *
     *******************/

    GraphController::GraphController(): Module("GraphController"), Node(), PickableObject()
    {
	this->addInputPort("curves");
	this->addInputPort("time_index");
	this->addInputPort("screen_window");
	this->addInputPort("curves_new_dataset");

	this->addInputPort("set_view_global_graph");
	this->addInputPort("default");
	this->addInputPort("time_coords");
	this->addOutputPort("default");
    }

    void GraphController::process()
    {	   	    

	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return;

	cout << "   window.....OK" << endl;

	int time_index = (dynamic_cast< data::Single<int>* >  
			  (getInputPort("time_index")->getData()))->value;

	using namespace boost::gregorian;

	vector<date> *time_coords = 
	    (dynamic_cast< data::Single<vector<date>*> *>(  
		getInputPort("time_coords")->getData()))->value;

	date day0 = time_coords->at(0);
	date day1 = time_coords->at(time_coords->size()-1);

	vector<datatypes::Curve*>* curves = NULL;
	    
	// data::Vector<datatypes::Curve*> *curvesVector = dynamic_cast< data::Vector<datatypes::Curve*>* >  
// 		(getInputPort("curves")->getData());

	data::Vector<datatypes::Curve*> *curvesVector = 
	    dynamic_cast< data::Vector<datatypes::Curve*>* > (
		getInputPort("curves_new_dataset")->getData() );

	    
	if(curvesVector == NULL)
	    return;

	curves = curvesVector->value;

	double max = -1;

	if(curves == NULL){
	    return;
	}

	int numCurves = curves->size();
	    
	for(int i = 0 ; i < numCurves ; i++){
	    Curve *curve = curves->at(i);
	    double curveMax = curve->getMax();
	    if(curveMax > max){
		max = curveMax;
	    }
	}
	    
	int time_length = 2;
	    
	if(numCurves > 0){
	    datatypes::Curve *curve = curves->at(0);
	    time_length = curve->getNumPoints();
	}
	else{
	    return;
	}

	bool reset_view = 
	    (dynamic_cast<data::Bool* > 
	     (getInputPort("set_view_global_graph")->getData()))->value;


	if(reset_view){
	    this->x_min_view = 0;
	    this->x_max_view = (day1 - day0).days();


	    this->y_min_view = 0;
	    this->y_max_view = max;

	    cout << "X " << this->x_max_view << " Y " << this->y_max_view  << endl;

	    (getInputPort("set_view_global_graph")->setData(new data::Bool(false)));
	}


	// screen window
	float x = screen_window.x;
	float y = screen_window.y;
	float w = screen_window.w;
	float h = screen_window.h;
	    
	float xx = x + margin_left;
	float yy = y + margin_bottom;
	float ww = w - margin_left - margin_right;
	float hh = h - margin_top - margin_bottom;
	    
	rendersystem::RenderSystem *renderSystem = 
	    Global::instance()->getRenderSystem();

	renderSystem->startViewport(x,y,w,h);


	{ // draw y-labels

	    Labeling lbl;
	    double Q[] = {1,5,2,2.5,4,3};
	    double W[] = {0.25,0.2,0.5,0.05}; 

	    // cout << "labelExtended" << endl;
	    // cout << "    dmin       = " << min_value << "," << endl;
	    // cout << "    dmax       = " << max_value << "," << endl;
	    // cout << "    m          = " << 10 << "," << endl;
	    // cout << "    Q          = {1,5,2.5,4,3}" << "," << endl;
	    // cout << "    sizeQ      = 6" << "," << endl;
	    // cout << "    onlyInside = true" << "," << endl;
	    // cout << "    w          = {0.25,0.2,0.5,0.05}" << endl;

	    vector<double>* labels = lbl.labelExtended(y_min_view, 
						       y_max_view, 8, Q, 6, 
						       true, W);

	    for (int i=0;i<labels->size();i++)
	    {
		double currVal =  labels->at(i);
		    
		if (abs(currVal) < 1.0e-8)
		    currVal = 0.0;

		char text[64];
		sprintf(text,"%.7g",currVal);
		    
		double text_height = 14;
		    
		double text_x0 = xx + ww + 3;
		float  text_y0 = yy + hh * (currVal - y_min_view)/(y_max_view - y_min_view);
		    
		renderSystem->setColor(0.8,0.8,0.8);
		renderSystem->drawText(text, text_height, floor(text_x0), floor(text_y0));

		renderSystem->setColor(.2,.2,.2);
		renderSystem->drawSegment(xx, text_y0, xx + ww, text_y0);

	    }

	    delete labels;

	    // cout << "... finished" << endl;

	} // draw y-labels


	{ // draw x-labels

	    Labeling lbl;
	    double Q[] = {1,5,2,2.5,4,3};
	    double W[] = {0.25,0.2,0.5,0.05}; 

	    // cout << "labelExtended" << endl;
	    // cout << "    dmin       = " << min_value << "," << endl;
	    // cout << "    dmax       = " << max_value << "," << endl;
	    // cout << "    m          = " << 10 << "," << endl;
	    // cout << "    Q          = {1,5,2.5,4,3}" << "," << endl;
	    // cout << "    sizeQ      = 6" << "," << endl;
	    // cout << "    onlyInside = true" << "," << endl;
	    // cout << "    w          = {0.25,0.2,0.5,0.05}" << endl;

	    vector<double>* labels = lbl.labelExtended(x_min_view, 
						       x_max_view, 6, Q, 6, 
						       true, W);

	    date_facet* output_facet = new date_facet();
	    output_facet->format("%b %d");
	    stringstream ss;
	    ss.imbue(locale(locale::classic(), output_facet));

	    for (int i=0;i<labels->size();i++)
	    {
		double currVal =  (int) labels->at(i);
		    
		// cout << "currVal " << currVal << endl;
		// cout << "day0 "    << day0 << endl;
		date d_i = day0 + days(currVal);
		// cout << "d_i     "    << d_i << endl;

		ss.str("");
		ss << d_i;

		double text_height = 14;
		    
		double text_x0 = xx + (currVal-x_min_view)/(x_max_view-x_min_view) * ww;
		float  text_y0 = yy - 17;
		    
		renderSystem->setColor(0.8,0.8,0.8);
		renderSystem->drawHorizontallyCenteredText(
		    ss.str().c_str(), text_height, floor(text_x0), floor(text_y0));

		renderSystem->setColor(.2,.2,.2);
		renderSystem->drawSegment(text_x0, yy, text_x0, yy+hh);

	    }

	} // draw x-labels



	{//Render		

	    renderSystem->pushMatrix();
	    renderSystem->translate(xx,yy);

	    float sx = ww/(1.0*(x_max_view-x_min_view));
	    float sy = (1.0*hh)/(y_max_view-y_min_view);

	    renderSystem->scale(sx,sy);

	    renderSystem->translate(-x_min_view,-y_min_view);

	    for(int i = 0 ; i < numCurves ; i++){
		    
		datatypes::Curve *curve = curves->at(i);
		    
		int numPoints = curve->getNumPoints();
		double color[4];
		curve->getColor(color);


		float curve_points[2 * numPoints];

		for(int j = 0 ; j < numPoints ; j++){

		    double val_y = curve->getValue(j);
		    date   d_j   = time_coords->at(j);
		    double val_x = (d_j - day0).days();

		    curve_points[2*j]   = val_x;
		    curve_points[2*j+1] = val_y;

		    // cout << "x, y " << val_x << ", " << val_y << endl;

		}

		renderSystem->setColor(color[0],color[1],color[2]);

		renderSystem->drawLineStrip(numPoints, curve_points);

		// cout << "r,g,b,a " << color[0] << "," << color[1] << 
		// "," << color[2] << "," << color[3] << endl;
		    
	    }

	    // cout << "sx, sy " << sx << ", " << sy << endl;

	    renderSystem->popMatrix();

	} // end curves

	     
	{ //
		
	    date currentDate = time_coords->at(time_index);
	    double tt = (time_coords->at(time_index) - day0).days();
	    float time_coef = (tt-x_min_view)/(x_max_view-x_min_view);

	    renderSystem->setColor(1.0,1.0,1.0);

	    renderSystem->drawSegment(
		xx + time_coef * ww, yy,
		xx + time_coef * ww,yy + hh);

	    double tx = xx;
	    double ty = yy + hh - 24;
	    double tw = 120;
	    double th = 24;

	    renderSystem->setColor(0.1,0.1,0.1,0.8);
	    renderSystem->drawQuad(tx,ty,tw,th);

	    date_facet* output_facet = new date_facet();
	    output_facet->format("%b %d, %Y");
	    stringstream ss;
	    ss.imbue(locale(locale::classic(), output_facet));
	    ss << currentDate;


	    renderSystem->setColor(1,1,1);
	    renderSystem->drawHorizontallyCenteredText(ss.str().c_str(), 
						       16, tx + tw/2.0, ty+6);

	}

	renderSystem->setColor(.2,.2,.2);

	renderSystem->drawQuad(x,y,w,h,GL_LINE_LOOP);

	renderSystem->endViewport();


    } // end process
	

    /*******************
     * GraphController *
     *******************/

    bool 
    GraphController::event_mouse_contains(int x, int y) { 

	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return false;

	return screen_window.contains(x,y);

    }

    string 
    GraphController::event_node_name(eventsystem::State *event_state) { 
	return "GraphController"; 
    }

    void 
    GraphController::event_mouse_mouse(eventsystem::State *event_state) {}

    eventsystem::PickableObject*
    GraphController::event_mouse_pick(eventsystem::State *event_state) { 

	return this;

	// if (renderer == NULL)
	// 	return NULL;

	// cout << "event_mouse_pick" << endl;

	// picked_object = renderer->pick(event_state->x, event_state->y);

	// cout << "   ====> picked_object " << picked_object << endl;

	// return this;

    }

    bool 
    GraphController::event_mouse_wheel(eventsystem::State *event_state) 
    { 
	cout << "(GraphController) event_mouse_wheel " << event_state->wheel_dx << endl;

	double x = x_min_view;
	double y = y_min_view;
	double w = x_max_view - x_min_view;
	double h = y_max_view - y_min_view;

	ViewRect vr;
	vr.init(x,y,w,h);

	double coef =  (event_state->wheel_dx > 0 ? 1.0/1.1 : 1.1);

	vr.scale(coef);

	if (!event_state->shift)
	{
	    x_min_view = vr.x;
	    x_max_view = vr.x + vr.w;
	    cout << "======> x update" << endl;
	}

	if (!event_state->ctrl)
	{
	    y_min_view = vr.y;
	    y_max_view = vr.y + vr.h;
	    cout << "======> y update" << endl;
	}
	    
	this->outdateDownstream();
	return true;
    }

    // PickableObject *mm_event_mouse_drop(State *event_state) { return NULL; }

    // int mm_size() { return 0; }

    // Node *mm_get_child(int index) { return NULL; }

    void 
    GraphController::event_mouse_drag(eventsystem::State *event_state) 
    {

	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return;

	double dx_screen = event_state->x - event_state->last_x;
	double dy_screen = event_state->y - event_state->last_y;

	double tx = dx_screen/screen_window.w;
	double ty = dy_screen/screen_window.h;

	double x = x_min_view;
	double y = y_min_view;
	double w = x_max_view - x_min_view;
	double h = y_max_view - y_min_view;

	// double side = max(screen_window.w,screen_window.h);
	double dx_world = tx * w;
	double dy_world = ty * h;

	y_min_view += -dy_world;
	y_max_view += -dy_world;

	x_min_view += -dx_world;
	x_max_view += -dx_world;
	    
	// ViewRect vr;
	// vr.init(x,y,w,h);


	// cout << "event_mouse_drag " << picked_object << endl;

	// if (picked_object == ColormapRenderer::Handle::NONE)
	// 	return;


	// double ww = renderer->range_handle->w;
	// double delta_x = event_state->x - event_state->last_x;
	// double delta_value = (delta_x/ww) * (max_value - min_value);

	// cout << "    ww " << ww << endl;
	// cout << "    delta_x " << delta_x << endl;
	// cout << "    delta_value " << delta_value << endl;

	// if (picked_object == ColormapRenderer::Handle::LEFT_HANDLE)
	// {
	// 	colormap->min_value += delta_value;
	// 	if (event_state->shift)
	// 	    colormap->max_value -= delta_value;


	// 	if (colormap->min_value > colormap->max_value)
	// 	    colormap->max_value = colormap->min_value;
	// 	if (min_value > colormap->min_value)
	// 	    min_value = colormap->min_value;
	// 	if (max_value < colormap->max_value)
	// 	    max_value = colormap->max_value;
	// }
	// else if (picked_object == ColormapRenderer::Handle::RIGHT_HANDLE)
	// {
	// 	colormap->max_value += delta_value;
	// 	if (event_state->shift)
	// 	    colormap->min_value -= delta_value;
	// 	if (colormap->min_value > colormap->max_value)
	// 	    colormap->min_value = colormap->max_value;
	// 	if (min_value > colormap->min_value)
	// 	    min_value = colormap->min_value;
	// 	if (max_value < colormap->max_value)
	// 	    max_value = colormap->max_value;
	// }
	// else if (picked_object == ColormapRenderer::Handle::COLORBAR)
	// {
	// 	colormap->min_value += delta_value;
	// 	colormap->max_value += delta_value;
	// 	if (min_value > colormap->min_value)
	// 	    min_value = colormap->min_value;
	// 	if (max_value < colormap->max_value)
	// 	    max_value = colormap->max_value;
	// }
	// else if (picked_object == ColormapRenderer::Handle::RANGE)
	// {
	// 	min_value -= delta_value;
	// 	max_value -= delta_value;
	// }

	// getOutputPort("colormap")->setData
	// 	(new data::Colormap1d(colormap),true);

	// getOutputPort("range")->setData
	// 	(new data::Vector<double>(min_value,max_value),true);

    }




    /***************
     * MapRenderer *
     ***************/

    MapRenderer::MapRenderer(): Module("MapRenderer")
    {
	this->addInputPort("maps_display_lists");
	this->addInputPort("display_list_index");
	this->addInputPort("default");
	
	this->addOutputPort("default");

    }
    
    void 
    MapRenderer::process()
    {
	vector<GLuint>* display_lists = NULL;
	int disp_list_index = -1;
	    
	display_lists = dynamic_cast< data::Vector<GLuint>* >  
	    (getInputPort("maps_display_lists")->getData())->value;
	
	disp_list_index = dynamic_cast< data::Integer* >  
	    (getInputPort("display_list_index")->getData())->value;
	
	if(disp_list_index > -1 && 
	   disp_list_index < display_lists->size()){
	    // cout << "**************Call Map Disp List " << endl;
	    // cout << "Index " << disp_list_index << endl;
	    // cout << "Disp LIst Used " << display_lists->at(disp_list_index) << endl;

	    // cout << "Disp LIst (0) " << display_lists->at(0) << endl;
	    // cout << "Disp LIst (1) " << display_lists->at(1) << endl;

	    // cout << "Num disp lists " << display_lists->size() << endl;
	    glCallList(display_lists->at(disp_list_index));
	}

    }

    /****************
     * ArrayBuilder *
     ****************/

    ArrayBuilder::ArrayBuilder(): Module("ArrayBuilder")
    {
	this->addInputPort("array");
	this->addInputPort("x_coords");
	this->addInputPort("y_coords");

	this->addInputPort("species_indices");
	this->addInputPort("time_index");
	this->addInputPort("expression");
	this->addInputPort("selections");
	this->addInputPort("dataset");
	this->addInputPort("dataset_index");

	this->addOutputPort("result_new_dataset");
	this->addOutputPort("result");
	this->addOutputPort("species_changed"); // we don't need this port anymore
	this->addOutputPort("curves");
	this->addOutputPort("curves_new_dataset");
	this->addOutputPort("predictors");

	prev_species_indices = NULL;

	curves             = new vector<datatypes::Curve*>();
	curves_new_dataset = new vector<datatypes::Curve*>();

	A = NULL;
	run = 0;
    }
	
    string 
    ArrayBuilder::buildKey(vector<int> *species, selection::Selection *selection, string expression){

	ostringstream s;
	    
	s << "Species: ";

	int numSpecies = species->size();

	for (int i=0;i<numSpecies;i++)
	{
	    s << species->at(i) << " ";
	}

	if(selection == NULL)
	    s << "Selections: NONE";
	else
	    s << "Selections: " << selection->id;

	s << " " << expression ;

	return s.str();
	    
    }

    void 
    ArrayBuilder::process()
    {

	// new datasets
	dataset::DatasetRepository *dsr = (dynamic_cast< data::Single<dataset::DatasetRepository *>* >  
					   (getInputPort("dataset")->getData()))->value;

	//int dataset_index = 0;
	data::Single<int> *x = (dynamic_cast< data::Single<int>* >  
				(getInputPort("dataset_index")->getData()));
	int test = x->value;
	    
	int dataset_index = test;

	if(dataset_index == -1)
	    return;

	vector<dataset::Variable *>* predictors = dsr->getPredictorVariables(dataset_index);
	this->addOutputPort("predictors")->setData(
	    new data::Single<vector<dataset::Variable *>*> (predictors),true);

	// old stuff
	// array
	datatypes::Array *array = (dynamic_cast< data::Array* >  
				   (getInputPort("array")->getData()))->value;

	// species indices
	vector<int> *species = (dynamic_cast< data::Vector<int>* >  
				(getInputPort("species_indices")->getData()))->value;
	////////////////////////////////////////////////////////////////////////////

	// time index
	int time_index = (dynamic_cast< data::Single<int>* >  
			  (getInputPort("time_index")->getData()))->value;

	// expression
	string expression = (dynamic_cast< data::Single<string>* >  
			     (getInputPort("expression")->getData()))->value;

	if(dsr != NULL){
	    Coordinate t0(5,1,2009);
	    Coordinate t1(28,12,2009);

	    datatypes::ConcreteArray *B = //dsr->getCubeGridWithoutInterpolation(250,125, dataset_index);
		dsr->getCubeGridWithoutInterpolation(250,125, 0, expression);

	    vector<int> slice_dim(1);
	    vector<int> slice_dim_index(1);

	    slice_dim[0] = 0;
	    slice_dim_index[0] = time_index;

	    datatypes::Array *a = B->slice(&slice_dim, &slice_dim_index);
	    A = a->copy();			
		
	    delete a;
	}

	this->getOutputPort("result")->setData(new data::Array(A,true), true);

	//set species changed signal
	this->getOutputPort("species_changed")->setData(new data::Bool(true), true);

	data::Single<vector<selection::Selection*> *> *d = 
	    dynamic_cast<data::Single<vector<selection::Selection*> *> *>
	    (getInputPort("selections")->getData());

	vector<selection::Selection *> *selections = NULL;
	int numSelections = 0;
	if (d != NULL)
	{
	    selections = d->value;
	    numSelections = selections->size();
	}

	//set curves
	double colorNewCurve[4] = {1.0,1.0,0.0,0.0};
	curves_new_dataset->clear();
	    
	ConcreteArray* graph = dsr->getSumVectorWithoutInterpolation(dataset_index, 125, 250, NULL, expression);

	datatypes::Curve* mainCurve = new datatypes::Curve(graph,colorNewCurve);

	curves_new_dataset->push_back(mainCurve);

	for(int l = 0 ; l < numSelections ; l++){
	    selection::Selection* curSelection = selections->at(l);

	    ConcreteArray* selGraph = 
		dsr->getSumVectorWithoutInterpolation(dataset_index, 125, 250,
						      curSelection, expression);	    
	    double color[4];
	    color[0] = curSelection->color.r;
	    color[1] = curSelection->color.g;
	    color[2] = curSelection->color.b;
	    color[3] = curSelection->color.a;

	    datatypes::Curve* selCurve = 
		new datatypes::Curve(selGraph,color);		

	    curves_new_dataset->push_back(selCurve);
	}
	    

	data::Vector<datatypes::Curve*> *curvesNewDatasetDataFlow =
	    new data::Vector<datatypes::Curve*>(curves_new_dataset);
	    
	this->getOutputPort("curves_new_dataset")->setData(curvesNewDatasetDataFlow, true);
	    
	prev_species_indices = species;
    }



    /********
     * Sink *
     ********/

    Sink::Sink(): Module("Sink")
    {
	this->addInputPort("default");
    }

    /***************************
     * MapController::Listener *
     ***************************/

    MapController::Listener::Listener() 
    {}

    void 
    MapController::Listener::map_controller_update(
	MapController *mapController, ViewRect world_window) 
    {}

    /*****************
     * MapController *
     *****************/

    MapController::MapController(): 
	Module("MapController")
    {
	this->addInputPort("begin_or_end"); // if true start else end
	this->addInputPort("screen_window");
	this->addInputPort("use_map_stencil");
	this->addInputPort("map_triangles_display_list");
	
	this->addInputPort("dataset_repository");
	this->addInputPort("dataset_index");
	this->addInputPort("time_index");
	
	this->addOutputPort("screen_window");
	this->addOutputPort("world_window");
	
	// new rectangle being created
	this->addOutputPort("selections");
	this->addOutputPort("new_selection");
	
	// new rectangle being created
	this->addOutputPort("mouse_lonlat"); // Vec2
	
	this->addInputPort("default");
	this->addOutputPort("default");
	
	// tag clouds
	this->addOutputPort("tag_clouds");
	
	this->pick_mode = NONE;
	
	//
	//
	new_rectangle_color_index = 0;
    }

    void 
    MapController::addListener(Listener *l)
    {
	listeners.push_back(l);
    }
    
    void 
    MapController::notify()
    {
	for (int i=0;i<listeners.size();i++)
	    listeners.at(i)->map_controller_update(this, world_window);
    }

    void 
    MapController::set_world_region(double x, double y, double w, double h)
    {
	this->world_window.init(x,y,w,h);
	getOutputPort("world_window")->
	    setData(new data::SingleViewRect(world_window),true);
	this->notify();
    }
    
    void 
    MapController::process()
    {
	cout << "MapController::process()" << endl;
	
	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return;
	    
	cout << "   window.....OK" << endl;
	    
	bool start = 
	    (dynamic_cast<data::Bool* > 
	     (getInputPort("begin_or_end")->getData()))->value;
	    
	// cout << "screen_window " << screen_window.str() << endl;
	// cout << "world_window "  << world_window.str() << endl;
	    
	    
	// {
	// 	float px = screen_window.x;
	// 	float py = screen_window.y;
	// 	float pw = screen_window.w;
	// 	float ph = screen_window.h;

	// 	glViewport(px,py,pw,ph);
    
	// 	//
	// 	glScissor((int)px,(int)py,(int)pw,(int)ph);
	// 	glEnable(GL_SCISSOR_TEST);		

	// 	// prepare right aspect ratio

	// 	// update projection to long,lat range
	// 	glMatrixMode(GL_PROJECTION);
	// 	glLoadIdentity();
	// 	glOrtho(px,px+pw,py,py+ph,-10, 10);

	// 	glMatrixMode(GL_MODELVIEW);
	// 	glLoadIdentity();
		
	// 	glColor3f(1,1,1);
	// 	gl_util::draw_circle(px + pw/2.0,py + pw/2.0, pw/2.0);
	// }

	if (start)
	{

	    float px = screen_window.x;
	    float py = screen_window.y;
	    float pw = screen_window.w;
	    float ph = screen_window.h;
	    int side = std::max(pw,ph);
	    glViewport((int)(px + (pw - side) / 2), (int)(py + (ph - side) / 2), side, side);
    
	    //
	    glScissor((int)px,(int)py,(int)pw,(int)ph);
	    glEnable(GL_SCISSOR_TEST);		

	    // prepare right aspect ratio

	    // update projection to long,lat range
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    double x_min = world_window.x;
	    double y_min = world_window.y;
	    double x_max = x_min + world_window.w;
	    double y_max = y_min + world_window.h;
	    glOrtho(x_min,x_max,y_min,y_max,-10, 10);



	    glClearStencil(0);
	    glClear(GL_STENCIL_BUFFER_BIT);

	    bool stencil_bool = 
		(dynamic_cast<data::Bool* > (getInputPort("use_map_stencil")->getData()))->value;
	    // cout << "BOOL PORT WORKS" << endl;
	    
	    if(stencil_bool){
		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0x0, 0xff);
		glStencilOp(GL_INCR, GL_INCR, GL_INCR);
		
		// 	//glCallList()

		// 	cout << "Before get the list" << endl;
		GLuint map_disp_list = dynamic_cast< data::Integer* >  
		    (getInputPort("map_triangles_display_list")->getData())->value;	
		glColorMask(0,0,0,0);
		glCallList(map_disp_list);
		glColorMask(1,1,1,1);


		glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		
	    }
	    else{
		glDisable(GL_STENCIL_TEST);
	    }

	    // glEnable(GL_BLEND);
	    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	} // end setup viewport
	else 
	{
	    glScissor(0,0,1,1);
	    glDisable(GL_SCISSOR_TEST);
	    glDisable(GL_STENCIL_TEST);
	    glOrtho(-1,1,-1,1,-1,1);

	    // glDisable(GL_BLEND);

	}

	// pass forward
	getOutputPort("screen_window")->setData(new data::SingleViewRect(screen_window));
	getOutputPort("world_window")->setData(new data::SingleViewRect(world_window));


	updateSelectionTagCloudInfo();

    }

    dataset::DatasetRepository*
    MapController::getDatasetRepository()
    {
	data::Single<dataset::DatasetRepository *>* port_data = 
	    dynamic_cast< data::Single<dataset::DatasetRepository *>* >  
	    (getInputPort("dataset_repository")->getData());

	if (port_data != NULL)
	    return port_data->value;
	else
	    return NULL;
    }

    int 
    MapController::getDatasetIndex()
    {
	data::Single<int> *port_data = 
	    dynamic_cast< data::Single<int>* > (getInputPort("dataset_index")->getData());
		
	if(port_data == NULL)
	    return -1;
	else
	    return port_data->value;
    }

    int 
    MapController::getTimeIndex()
    {
	data::Single<int> *port_data = 
	    dynamic_cast< data::Single<int>* > (getInputPort("time_index")->getData());
		
	if(port_data == NULL)
	    return -1;
	else
	    return port_data->value;
    }

    void 
    MapController::updateSelectionTagCloudInfo()
    {
	    
	data::Single<int> *x;

	int numSelections = selections.size();

	dataset::Dataset* ds;
	    
	dataset::DatasetRepository *dsr = this->getDatasetRepository();
	if(dsr == NULL)
	    return;

	int dataset_index = this->getDatasetIndex();
	int time_index = this->getTimeIndex();

	if (dataset_index < 0 || time_index < 0)
	    return;

	ds = dsr->getDataset(dataset_index);

	vector<datatypes::TagCloudSpec*> *tagClouds = new vector<datatypes::TagCloudSpec*>();

	// vector<datatypes::TagCloudSpec*> *tagClouds = new vector<datatypes::TagCloudSpec*>();

	for(int i = 0 ; i < numSelections  ; i++){

	    selection::Selection* curSelection = this->selections.at(i);
	    
	    if(curSelection->render_mode == selection::Selection::TAG_CLOUD){
		//create tag cloud

		double xw,yw,ww,hw;

		xw = curSelection->quad.x;
		yw = curSelection->quad.y;
		ww = curSelection->quad.w;
		hw = curSelection->quad.h;

		double x0, y0, x1, y1;

		world2screen(xw, yw, x0, y0);
		world2screen(xw + ww, yw + hw, x1, y1);

		TagCloudSpec *t = new TagCloudSpec();
		t->key = curSelection->id;
		t->window.init(x0, y0, x1 - x0, y1-y0);

		cout << "render tag cloud at" << t->window.str() << endl;

		for(int j = 0 ; j < ds->getNumVariables() ; j++){
		    dataset::Variable* v = ds->getVariable(j);
		    if(v->type == dataset::Variable::PREDICTOR){
			datatypes::ConcreteArray* curve = 
			    dsr->getSumVectorWithoutInterpolation(
				dataset_index, j, 250,125,curSelection);

			int index = (int) ((time_index/51.0)*79.0);

			float value = curve->getValueAt(&index);
			t->addTag(v->name,value);
		    }
		}

		tagClouds->push_back(t);			
	    }
	}

	getOutputPort("tag_clouds")->setData
	    (new data::Single<vector<datatypes::TagCloudSpec*> *>(tagClouds),true);
    }

    string 
    MapController::event_node_name(eventsystem::State *event_state) { 

	return "MapController"; 

    }

    bool 
    MapController::event_mouse_drop(eventsystem::State *event_state) {	    
	return false;	    
    }

    bool 
    MapController::event_mouse_contains(int x, int y) 
    { 
	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return false;

	// cout << "event_contains MapController " << data->value.str() 
	// 	 << " x,y " << x << ", " << y << endl;

	return screen_window.contains(x,y);

    }

    void 
    MapController::setWorldWindow(ViewRect ww)
    {
	// programatically set world window: don't notify
	this->world_window = ww;

	getOutputPort("world_window")->setData
	    (new data::SingleViewRect(world_window),true);
    }

    void 
    MapController::world2screen(double x, double y, double &xx, double &yy)
    {
	// update viewport (to range in pixel size correctly)
	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return;

	double px = screen_window.x;
	double py = screen_window.y;
	double pw = screen_window.w;
	double ph = screen_window.h;
	int side = std::max(pw,ph);

	px = (int)(px + (pw - side) / 2);
	py = (int)(py + (ph - side) / 2);

	double coef_x = (x - world_window.x)/(world_window.w);
	double coef_y = (y - world_window.y)/(world_window.h);

	xx = px + coef_x * side;
	yy = py + coef_y * side;

    } // end view2world

    void 
    MapController::screen2world(int x, int y, double &wx, double &wy)
    {
	// update viewport (to range in pixel size correctly)
	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return;

	double px = screen_window.x;
	double py = screen_window.y;
	double pw = screen_window.w;
	double ph = screen_window.h;
	int side = std::max(pw,ph);
    
	px = (int)(px + (pw - side) / 2);
	py = (int)(py + (ph - side) / 2);

	double coef_x = (x - px)/(side);
	double coef_y = (y - py)/(side);

	wx = world_window.x + coef_x * world_window.w;
	wy = world_window.y + coef_y * world_window.h;

    } // end view2world

    eventsystem::PickableObject*
    MapController::event_mouse_pick(eventsystem::State *event_state) { 

	if (event_state->button == eventsystem::LEFT)
	{
	    // cout << "    Picked Map" << endl;

	    int numSelections = this->selections.size();

	    double wx, wy;
	    this->screen2world(event_state->x, event_state->y, wx, wy);

	    // check if there is some box in the selection
	    selection::Selection* hit_selection = NULL;
	    for(int i = numSelections - 1 ; i > -1  ; i--){
		selection::Selection* curSelection = this->selections.at(i);
		if(curSelection->contains(wx,wy)){
		    hit_selection = curSelection;
		    break;
		}
	    }


	    //
	    if (hit_selection != NULL)
	    {
		double d;
		ViewRect::Corner corner = hit_selection->quad.closestCorner(wx, wy, d);
		if (d < min(hit_selection->quad.w * 0.05, hit_selection->quad.h * 0.05))
		{
		    resizingSelection = hit_selection;
		    resizingCorner = corner;
		    pick_mode = RESIZE_REGION;
		    return this;
		}
		else
		{
		    movingSelection = hit_selection;
		    pick_mode = MOVE_REGION;
		    return this;
		}
	    }
	    else 
	    {
		// if no selection was touched... pan
		pick_mode = MAP;
		return this;
	    }



	}
	else if (event_state->button == eventsystem::RIGHT)
	{

	    // cout << "Picked NEW_REGION" << endl;
	    screen2world(event_state->x, event_state->y, 
			 this->world_pick_x, this->world_pick_y);

	    pick_mode = NEW_REGION;

	    ViewRect quad(this->world_pick_x,
			  this->world_pick_y,
			  0,0);

	    Color c(COLORS[new_rectangle_color_index][0],
		    COLORS[new_rectangle_color_index][1],
		    COLORS[new_rectangle_color_index][2]);

	    new_selection = new selection::Selection(c, quad);
	    new_rectangle_color_index = (new_rectangle_color_index + 1) % NUM_COLORS;

	    getOutputPort("new_selection")->setData
		(new data::Single<selection::Selection*>(new_selection),true);

	    return this;
	}

	return NULL;
    }

    bool 
    MapController::event_mouse_wheel(eventsystem::State *event_state) 
    { 
	cout << "MapController: event_mouse_wheel " 
	     << event_state->wheel_dx << endl;

	double coef =  (event_state->wheel_dx > 0 ? 1.0/1.1 : 1.1);
	world_window.scale(coef);

	this->notify();

	getOutputPort("world_window")->setData
	    (new data::SingleViewRect(world_window),true);

	return true;
    }


    // int mm_size() { return 0; }

    // Node *mm_get_child(int index) { return NULL; }

    void 
    MapController::event_mouse_drag(eventsystem::State *event_state) 
    {
	cout << "event_mouse_drag " << endl;

	// update viewport (to range in pixel size correctly)
	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return;

	double dx_screen = event_state->x - event_state->last_x;
	double dy_screen = event_state->y - event_state->last_y;

	double side = max(screen_window.w,screen_window.h);
	double dx_world = (1.0 * dx_screen)/side * world_window.w;
	double dy_world = (1.0 * dy_screen)/side * world_window.h;
	    
	if (pick_mode == MAP)
	{
	    world_window.translate(-dx_world, -dy_world);
	    getOutputPort("world_window")->setData
		(new data::SingleViewRect(world_window),true);

	    this->notify(); // notify

	}
	else if (pick_mode == NEW_REGION)
	{
	    double new_x, new_y;
	    cout << "Screen Coordinates " << event_state->x << ", " << event_state->y << endl;

	    this->screen2world(event_state->x, event_state->y,
			       new_x, new_y);

	    double x = min(new_x,world_pick_x);
	    double y = min(new_y,world_pick_y);
	    double w = max(new_x,world_pick_x) - x;
	    double h = max(new_y,world_pick_y) - y;
		
	    new_selection->quad.x = x;
	    new_selection->quad.y = y;
	    new_selection->quad.w = w;
	    new_selection->quad.h = h;

	    cout << "new selection: rect " << x << "," << y << "," << w << "," << h << endl;

	    // send signal of outdated
	    getOutputPort("new_selection")->outdateDownstream();
	}
	else if (pick_mode == MOVE_REGION)
	{
	    movingSelection->quad.translate(dx_world, dy_world);

	    // send signal of outdated selections
	    getOutputPort("selections")->outdateDownstream();
	}

	else if (pick_mode == RESIZE_REGION)
	{
	    resizingSelection->
		quad.resizeRelativeToCorner(resizingCorner,dx_world, dy_world);

	    // send signal of outdated selections
	    getOutputPort("selections")->outdateDownstream();
	}
    }

    void 
    MapController::event_mouse_release(eventsystem::State *event_state)
    { 
	cout << "release object" << endl;
	if (pick_mode == NEW_REGION)
	{
	    this->selections.push_back(new_selection);
	    this->new_selection = NULL;
		
	    // add new selection to selection list
	    getOutputPort("selections")->setData
		(new data::Single<vector<selection::Selection*> *>(&selections),true);

	    // add new selection to selection list
	    getOutputPort("new_selection")->setData(NULL,true);
	}

	this->movingSelection = NULL;
	this->resizingSelection = NULL;
	pick_mode = NONE;
    }

    void 
    MapController::event_mouse_move(eventsystem::State *event_state){

	int x_screen = (event_state->x);
	int y_screen = (event_state->y);
	    
	double wx,wy;

	screen2world(x_screen, y_screen, wx, wy);
		
	this->mouse_longitude = wx;
	this->mouse_latitude  = wy;

	Vec2 lonlat(mouse_longitude, mouse_latitude);

	// add new selection to selection list
	getOutputPort("mouse_lonlat")->setData
	    (new data::Single<Vec2>(lonlat),true);

    }

    void MapController::open_google_maps_link()
    {
	    
	ViewRect ww = world_window;

	float ar = ww.w/ww.h;

	double lon = mouse_longitude;
	double lat = mouse_latitude;


	double lonspan = ww.w;
	double latspan = ww.h;

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


    bool 
    MapController::event_key_press(eventsystem::State *event_state) 
    {
	typedef data::Single<vector<selection::Selection* > *> 
	    SingleVectorSelectionPtrPtr;


	cout << "MapController::event_key_press(...)" << endl;

	//cout << "++++++++++++++++++++++ Map Controller listen key press" << endl;
	int x_screen = (event_state->x);
	int y_screen = (event_state->y);

	int numSelections = selections.size();
	bool deleted_something = false;

	double wx,wy;

	screen2world(x_screen, y_screen, wx, wy);

	dataset::DatasetRepository *dsr;
	data::Single<int> *x;

	int dataset_index;
	int time_index;

	dataset::Dataset* ds;
	    
	switch(event_state->key)
	{
	case (eventsystem::Key_I):
	    open_google_maps_link();
	    break;		
	case (eventsystem::Key_Delete):	       		
	    //cout << "Delete Key Mouse Screen Coordinates " << event_state->x << " " << event_state->y << endl;		

		
	    for(int i = numSelections - 1 ; i > -1  ; i--){
		selection::Selection* curSelection = this->selections.at(i);
		    
		if(curSelection->contains(wx,wy)){
		    this->selections.erase(this->selections.begin() +i);
		    deleted_something = true;
		    break;
		}
	    }		    

	    getOutputPort("selections")->setData(
		new SingleVectorSelectionPtrPtr(&selections),true);

	    return deleted_something;
	    break;
	    //toogle selection render mode
	case (eventsystem::Key_R):	       		
	    cout << "   Toggle Tag Cloud Lenses..." << endl;

	    for(int i = numSelections - 1 ; i > -1  ; i--){
		selection::Selection* curSelection = this->selections.at(i);
		    
		if(curSelection->contains(wx,wy)){
		    cout << "      hit one selection!" << endl;
		    curSelection->toggleRenderMode();

		    getOutputPort("selections")->setData(
			new SingleVectorSelectionPtrPtr(&selections),true);

		    return true; // it was consumed
		    break;
		}
	    }		    

	    // updateSelectionTagCloudInfo();

	    break;

	default:
	    break;
	}

	return false;
    }


    const float MapController::COLORS[11][4] = { 
	{31 /255.0, 119/255.0, 180/255.0},
	{255/255.0, 127/255.0, 14 /255.0},
	{44 /255.0, 160/255.0, 44 /255.0},
	{214/255.0, 39 /255.0, 40 /255.0},
	{148/255.0, 103/255.0, 189/255.0},
	{140/255.0, 86 /255.0, 75 /255.0},
	{227/255.0, 119/255.0, 194/255.0},
	{127/255.0, 127/255.0, 127/255.0},
	{188/255.0, 189/255.0, 34 /255.0},
	{23 /255.0, 190/255.0, 207/255.0},
	{23 /255.0, 190/255.0, 207/255.0}};
    
    const int MapController::NUM_COLORS = 11;

/************************
 * MapSelectionRenderer *
 ************************/

    MapSelectionRenderer::MapSelectionRenderer(): Module("MapSelectionRenderer")
    {
	this->addInputPort("new_selection"); // new selection being created
	this->addInputPort("selections");    // current selections

	this->addInputPort("default");
	this->addOutputPort("default");
    }

    void MapSelectionRenderer::process()
    {

	{ // selections rendering
	    data::Single<vector<selection::Selection*> *> *d = 
		dynamic_cast<data::Single<vector<selection::Selection*> *> *>
		(getInputPort("selections")->getData());

	    if (d != NULL)
	    {
		vector<selection::Selection *> *selections = d->value;
		for (int i=0;i<selections->size();i++)
		{
		    selection::Selection *sel = selections->at(i);
	    
		    double x,y,w,h;
		    x = sel->quad.x;
		    y = sel->quad.y;
		    w = sel->quad.w;
		    h = sel->quad.h;

// 			cout << "map selection renderer rect " 
// 			     << x << "," << y << "," << w << "," << h << endl;
		    //
		    // double sx0,sy0,sx1,sy1;
		    // map_controller->world2screen(x,y,sx0,sy0);
		    // map_controller->world2screen(x+w,y+h,sx1,sy1);
		    
		    sel->color.call_glColor();
		    gl_util::draw_quad(x,y,w,h,GL_LINE_LOOP);
		    
		    glEnable(GL_BLEND);
		    
		    sel->color.call_glColor(0.3);
		    gl_util::draw_quad(x,y,w,h);
		    
		    glDisable(GL_BLEND);
		}
	    }
	} // end selections rendering



	{ // new selection rendering
	    data::Single<selection::Selection*> *d = 
		dynamic_cast<data::Single<selection::Selection*> *>
		(getInputPort("new_selection")->getData());

	    if (d == NULL)
		return;

	    selection::Selection *sel = d->value;
	    
	    double x,y,w,h;
	    x = sel->quad.x;
	    y = sel->quad.y;
	    w = sel->quad.w;
	    h = sel->quad.h;
		
	    //cout << "map selection renderer rect " << x << "," << y << "," << w << "," << h << endl;

	    //
	    // double sx0,sy0,sx1,sy1;
	    // map_controller->world2screen(x,y,sx0,sy0);
	    // map_controller->world2screen(x+w,y+h,sx1,sy1);

	    sel->color.call_glColor();
	    gl_util::draw_quad(x,y,w,h,GL_LINE_LOOP);

	    glEnable(GL_BLEND);
	    
	    sel->color.call_glColor(0.3);
	    gl_util::draw_quad(x,y,w,h);
	    
	    glDisable(GL_BLEND);

	} // end new selection rendering

    } // end process


   /************************
    * PredictorsController *
    ************************/

    PredictorsController::PredictorsController(): 
	Module("PredictorsController"), eventsystem::Node()
    {
	// colormap_index = 0;
    
	// // use default color map (composition)
	// colormap = ColormapController::COLORMAPS[colormap_index]->copy();
	// min_value = -0.05;
	// max_value = 1.05;
    
	this->addInputPort("default");
	this->addInputPort("screen_window");
	this->addInputPort("predictors");
	this->addInputPort("time_index");

	this->addInputPort("selections");

	// // output
	this->addOutputPort("default");
    }

    PredictorsController::TagCloudItem::TagCloudItem(string text, double relativeSize){
	this->text = text;
	this->relativeSize = relativeSize;
	this->x = 0;
	this->y = 0;
	// this->w = 1;
	// this->h = 1;
	this->fontSize = 1.0;
    }

    ViewRect     
    PredictorsController::TagCloudItem::rect()
    {
	// float xx, float yy
	return ViewRect(x + base_x, y + base_y, base_w, base_h);
    }


    PredictorsController::Line::Line() {
	index = 0;
	len = 0;
	bounds.init(0,0,0,0);
    }
    PredictorsController::Line::Line(int index, int len, ViewRect bounds) {
	this->index = index;
	this->len = len;
	this->bounds = bounds;
    }
    void 
    PredictorsController::Line::init(int index, int len, ViewRect bounds) {
	this->index = index;
	this->len = len;
	this->bounds = bounds;
    }

    ViewRect 
    PredictorsController::positionTags(vector<TagCloudItem*> *items, float W, 
				       float hsep, float vsep)
    {
	// cout << "positionTags( " << W << " ) " << endl;

	vector<Line> lines;


	// loop on items trying to position them
	Line line;
	ViewRect zero_rect(0,0,0,0);
	line.init(0,0,zero_rect);

	float x,y;
	x = 0; y = 0;
	int i = 0;
	while (i < items->size())
	{
	    TagCloudItem *item = items->at(i);
	    if (x + item->base_x + item->base_w <= W)
	    {
		item->x = x;
		item->y = 0;
		line.bounds.resizeToIncludeRect(item->rect());
		x += item->base_x + item->base_w + hsep;
		// cout << "   set x of item " << i << " line len incremented" << endl;
		line.len++;
		i++;
	    }
	    else if (x == 0) { // doesnt fit!
		return zero_rect;
		exit(0);
	    }
	    else {
		// cout << "   line add:    i0 " << line.index << " len " << line.len << " i1 " << (line.index + line.len - 1) << " view rect " << line.bounds.str() << endl;
		lines.push_back(line);
		line.init(i,0,zero_rect);
		x = 0;
	    }
	}
	if (line.len > 0)
	    lines.push_back(line);

	for (int i=0;i<lines.size();i++)
	{
	    line = lines.at(i);
	    // cout << "line " << i << " i0 " << line.index << " len " << line.len << " i1 " << (line.index + line.len - 1) << " view rect " << line.bounds.str() << endl;
	}

	y = 0;
	ViewRect result(0,0,0,0);
	line = lines.at(0);
	for (int j=0;j<line.len;j++) 
	    result.resizeToIncludeRect(items->at(line.index + j)->rect());

	for (int i=1;i<lines.size();i++)
	{
	    line = lines.at(i);
	    double y_offset = lines.at(i-1).bounds.minY() + 
		lines.at(i).bounds.maxY() + hsep;
	    for (int j=0;j<line.len;j++) {
		items->at(line.index + j)->y = y - y_offset;
		result.resizeToIncludeRect(items->at(line.index + j)->rect());
	    }
	    y -= y_offset;
	}

	return result;

	// // second pass adjusting y
	// double curr_y = 0;
	// double y = 0;
	// ViewRect y_line(0,0,0,0);
	// for (int i=0;i<items->size;i++)
	// {
	// 	TagCloudItem *item = items->at(i);
	// 	if (item->y == curr_y)
	// 	{
	// 	    y_line.resizeToIncludePoint(0,item->base_y);	
	// 	    y_line.resizeToIncludePoint(0,item->base_y + item->base_h);
	// 	}
	// 	else 
	// }


	// int i = 0;
	// while (i < items->size())
	// {
	// 	TagCloudItem *item = items->at(i);

	// 	if (x + item->base_x + item->base_w <= W)
	// 	{
	// 	    item->x = x;
	// 	    item->y = y;
	// 	    result.resizeToIncludeRect(item->rect());
	// 	    x += item->base_x + item->base_w + hsep;
	// 	    i++;
	// 	}
	// 	else if (x == 0) { // doesnt fit!
	// 	    result.x = 0;
	// 	    result.y = 0;
	// 	    result.w = 0;
	// 	    result.h = 0;
	// 	    return result;
	// 	}
	// 	else {
	// 	    x  = 0;
	// 	    y -= 1; 
	// 	}
	// }

    }

    ViewRect 
    PredictorsController::computeTagCloud(vector<TagCloudItem*> *items, double ar_target)
    {
	ViewRect result(0,0,0,0);
	if (items->size() == 0)
	    return result;

	//
	rendersystem::RenderSystem *renderSystem = 
	    Global::instance()->getRenderSystem();

	// font description
	double oneline_min_x;
	double oneline_min_y;
	double oneline_max_x;
	double oneline_max_y;

	double max_item_w;

	// measure base size of tags and keep the largest width of a word,
	// the sum of the widths and the hight

	ViewRect rect_one_line(0,0,0,0);

	float largestTagWidth = 0;

	float x = 0;
	float y = 0;
	for (int i=0;i<items->size();i++)
	{

	    TagCloudItem *item = items->at(i);

	    float advance;

	    ViewRect base = renderSystem->textBoundingBox(
		item->text.c_str(),
		item->relativeSize,0.,0.,
		&advance);

	    item->x = x;
	    item->y = y;
	    item->base_x  = base.x;
	    item->base_y  = base.y;
	    item->base_w  = base.w;
	    item->base_h  = base.h;
	    item->base_advance = advance;

	    if (largestTagWidth < base.w + base.x)
		largestTagWidth = base.x + base.w;

	    rect_one_line.resizeToIncludeRect(item->rect());

	    x += item->base_advance;

	} // end measure tags

	float hsep = 2.0 * min_scale;
	float vsep = min_scale;

	// compute aspect ratio
	float W_left = largestTagWidth;
	float W_right = rect_one_line.w + hsep * (items->size() - 1);
	    
	double   ar_left, ar_right;
	ViewRect vr;

	// compute ar_left
	vr = this->positionTags(items,W_left,hsep,vsep);
	ar_left = W_left / vr.h;

	// compute ar_right
	vr = this->positionTags(items,W_right,hsep,vsep);
	ar_right = W_right / vr.h;

	// cout << "W_left, ar_left " << W_left << ", " << ar_left << endl;
	// cout << "W_right, ar_right " << W_right << ", " << ar_right << endl;

	//
	if (ar_target <= ar_left)
	{
	    vr = this->positionTags(items,W_left,hsep,vsep);
	}
	else if (ar_target >= ar_right)
	{
	    vr = this->positionTags(items,W_right,hsep,vsep);
	}
	else { // binary_search
	    while (W_right - W_left > 1e-3)
	    {
		double W_mid = (W_left + W_right)/2.0;
		vr = this->positionTags(items,W_mid,hsep,vsep);
		double ar_mid = W_mid / vr.h;

		// cout << "W_mid, ar_mid, ar_target " << W_mid << ", " << ar_mid << ", " << ar_target << endl;

		if (ar_mid < ar_target) {
		    W_left = W_mid;
		    ar_left = ar_mid;
		}
		else if (ar_mid > ar_target) {
		    W_right = W_mid;
		    ar_right = ar_mid;
		}
		else {
		    W_left   = W_mid;
		    W_right  = W_mid;
		    ar_left  = ar_mid;
		    ar_right = ar_mid;
		}
	    } // end binary search

	} // end else
		    

	//
	if (ar_target - ar_left < ar_right - ar_target)
	{
	    // cout << "winner is left: W_left, ar_left, ar_target " << W_left << ", " << ar_left << ", " << ar_target << endl;
	    vr = this->positionTags(items,W_left,hsep,vsep);
	}
	else
	{
	    vr = this->positionTags(items,W_right,hsep,vsep);
	    // cout << "winner is right: W_right, ar_right, ar_target " << W_right << ", " << ar_right << ", " << ar_target << endl;
	}

	// cout << "view rect " << vr.str() << endl;

	return vr;

    } // end compute tag positions


    void 
    PredictorsController::process() 
    {
	data::Single<vector<dataset::Variable *>*> *p = 
	    (dynamic_cast<data::Single<vector<dataset::Variable *>*> * >  
	     (getInputPort("predictors")->getData()));

	if(p == NULL)
	    return;

	vector<dataset::Variable *> *predictors = p->value;

	int time_index = (dynamic_cast< data::Single<int>* >  
			  (getInputPort("time_index")->getData()))->value;


	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return;

	double x = screen_window.x;
	double y = screen_window.y;
	double w = screen_window.w;
	double h = screen_window.h;


	if (w == 0 || h == 0) return;

	vector<TagCloudItem *> *tags = new vector<TagCloudItem *>();
	{ // compute tag cloud

	    double min_factor;
	    double max_factor;
	    for (int i=0;i< predictors->size();i++)
	    {
		dataset::Variable *pred = predictors->at(i);
		string     name   = pred->name;

		// BIG HACK
		int    index  = time_index;
		datatypes::ConcreteArray* curve = pred->getCurveOnPoints(NULL);
		index = (int) ((time_index/51.0)*79.0);

		double factor = curve->getValueAt(&index);

		if (factor < 1e-10)
		    factor = 0.0;

		if (i == 0)
		{
		    min_factor = factor;
		    max_factor = factor;
		}
		else if (max_factor < factor)
		{
		    max_factor = factor;
		}
		else if (min_factor > factor)
		{
		    min_factor = factor;
		}


		TagCloudItem *item = new TagCloudItem(name,factor);
		tags->push_back(item);
	    }


	    //		
	    if (min_factor == max_factor)
		max_factor += 1;

	    for (int i=0;i<tags->size();i++)
	    {
		TagCloudItem *item = tags->at(i);
		item->relativeSize = min_scale + (item->relativeSize - min_factor)/
		    (max_factor - min_factor) * (max_scale - min_scale);
	    }
	} //

	double margin_left = 25;
	double margin_right = 25;
	double margin_bottom = 25;
	double margin_top = 25;

	double xx = x + margin_left;
	double yy = y + margin_bottom;
	double ww = w - margin_left - margin_right;
	double hh = h - margin_bottom - margin_top;

	ViewRect tagsBoxBounds = computeTagCloud(tags,ww/hh);


	rendersystem::RenderSystem *renderSystem = 
	    Global::instance()->getRenderSystem();

	renderSystem->startViewport(xx,yy,ww,hh);

	double scale = min(ww/tagsBoxBounds.w,hh/tagsBoxBounds.h);
	double xoff = (ww - scale * tagsBoxBounds.w)/2.0;
	double yoff = (hh - scale * tagsBoxBounds.h)/2.0;


	renderSystem->pushMatrix();

	renderSystem->translate(xx + xoff,yy + yoff);
	renderSystem->scale(scale,scale);

	// cout << "scale: " << scale << endl;

	renderSystem->translate(-tagsBoxBounds.x,-tagsBoxBounds.y);
	    
	// glColor3f(0,0,1);
	// gl_util::draw_quad(tagsBoxBounds.x,tagsBoxBounds.y,
	// 		       tagsBoxBounds.w,tagsBoxBounds.h,
	// 		       GL_LINE_LOOP);

	for (int i=0;i<tags->size();i++)
	{
	    TagCloudItem *item = tags->at(i);

	    double fontSize = 1;

	    // cout << "name " << item->text << " relative size " 
	    //      << item->relativeSize 
	    //      << "rel_size / max_scale " 
	    //      << (item->relativeSize/max_scale)
	    //      << endl;

	    renderSystem->setColor(item->relativeSize/max_scale,
				   item->relativeSize/max_scale,
				   item->relativeSize/max_scale);

	    // glColor3f(0,0,1);

	    // FontTextIterator *fti =
	    //     Font_text_iterator(font_description,
	    // 		       item->text.c_str(),
	    // 		       item->relativeSize,
	    // 		       item->x,
	    // 		       item->y);
	    // float bbx,bby,bbw,bbh;
	    // glColor3f(0,0,1);
	    // FontTextIterator_bb(fti, &bbx, &bby, &bbw, &bbh);
	    // gl_util::draw_quad(bbx,bby,bbw,bbh,GL_LINE_LOOP);
	    // free(fti);


	    // cout << "drawing item " << item->text << " in " 
	    //      << item->x * ww / sizeTextBox.x  << " "
	    //      << item->y * hh / sizeTextBox.y  << endl;
	    renderSystem->drawText(item->text.c_str(), 
				   item->relativeSize, 
				   item->x,item->y);
		
	}

	renderSystem->popMatrix();

	renderSystem->endViewport();

    }
	
    bool 
    PredictorsController::event_mouse_contains(int x, int y) { 

	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return false;
    
	return screen_window.contains(x,y);
    
    }

    void 
    PredictorsController::event_mouse_mouse(eventsystem::State *event_state) 
    {}

    eventsystem::PickableObject*
    PredictorsController::event_mouse_pick(eventsystem::State *event_state) { 
	return NULL;
    }

    bool PredictorsController::event_mouse_wheel(eventsystem::State *event_state) 
    { 	    
	return true;
    }

    void 
    PredictorsController::event_mouse_drag(eventsystem::State *event_state) 
    {}

   /*********************
    * MapLabelsRenderer *
    *********************/

    MapLabelsRenderer::MapLabelsRenderer(): Module("MapLabelsRenderer")
    {
	// input ports
	this->addInputPort("screen_window");
	this->addInputPort("lonlat");
	this->addInputPort("grid");
	this->addInputPort("date");

	// output ports
	this->addOutputPort("default");
    }

    void 
    MapLabelsRenderer::process()
    {
	cout << "MapLabelsRenderer::process()" << endl;

	datatypes::Grid *grid;
	if (!data::SingleGridPtr::getInputValue(this,"grid",&grid))
	    return;

	cout << "   grid.......OK" << endl;

	Vec2 lonlat;
	bool has_lonlat = data::SingleVec2::getInputValue(this,"lonlat",lonlat);

	cout << "   lonlat.....OK" << endl;

	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return;

	cout << "   window.....OK" << endl;

	string date_st;
	if (!data::SingleString::getInputValue(this,"date",date_st))
	    return;

	cout << "   date.......OK" << endl;



	// screen window
	float x = screen_window.x;
	float y = screen_window.y;
	float w = screen_window.w;
	float h = screen_window.h;

	rendersystem::RenderSystem *renderSystem = 
	    Global::instance()->getRenderSystem();

	renderSystem->startViewport(x,y,w,h);

	char buf[1024];

	{ // latitude and longitude label
	    if (has_lonlat)
	    {
		sprintf(buf,"lon: %10.4f  lat: %10.4f",lonlat.x,lonlat.y);
	    }
	    else 
	    {
		sprintf(buf,"------------");
	    }

	    ViewRect r(x,y,180,22);
	    glColor4f(0.1,0.1,0.1,0.9);
	    
	    // draw quad
	    renderSystem->setColor(.1,.1,.1,.9);
	    renderSystem->drawQuad(r.x,r.y,r.w,r.h);

	    double text_height = 12;
	    renderSystem->setColor(1,1,1);
	    renderSystem->drawText(buf, text_height, r.x+6,r.y+6);

	} // end latitude and longitude labels

	{ // Value
	    if (has_lonlat) 
	    {
		double value;
		if (grid->getValueAtCoordinates(lonlat.x, lonlat.y, value))
		    sprintf(buf,"val: %10.5f",value);
		else 
		    sprintf(buf,"val: --------");
	    }
	    else 
	    {
		sprintf(buf,"val: --------");
	    }

	    double ww = 90;
	    double hh = 22;
	    ViewRect r(x+w - ww,y,ww,hh);

	    // draw quad
	    renderSystem->setColor(.1,.1,.1,.9);
	    renderSystem->drawQuad(r.x,r.y,r.w,r.h);

	    double text_height = 12;
	    renderSystem->setColor(1,1,1);
	    renderSystem->drawText(buf, text_height, r.x+6,r.y+6);

	} // end latitude and longitude labels



	// write date

	double tx = x + w - 120;
	double ty = y + h - 24;
	double tw = 120;
	double th = 24;

	renderSystem->setColor(0.1,0.1,0.1,0.8);
	renderSystem->drawQuad(tx,ty,tw,th);

	// date_facet* output_facet = new date_facet();
	// output_facet->format("%b %d, %Y");
	// stringstream ss;
	// ss.imbue(locale(locale::classic(), output_facet));
	// ss << currentDate;


	renderSystem->setColor(1,1,1);
	renderSystem->drawHorizontallyCenteredText(date_st.c_str(), 
						   16, tx + tw/2.0, ty+6);


	//


	renderSystem->endViewport();

    } // end process


   /***********
    * MapData *
    ***********/

    MapData::MapData(): 
	Module("MapData")
    {
	this->addInputPort("time_index");
	this->addInputPort("files");
	this->addInputPort("expression");
	this->addInputPort("default");

	this->addOutputPort("grid");
	this->addOutputPort("date");
	this->addOutputPort("default");
    }

    string dateToString(boost::gregorian::date d){
	string month;

	switch(d.month()){
	case(1):
	    month = string("Jan");
	    break;
	case(2):
	    month = string("Feb");
	    break;
	case(3):
	    month = string("Mar");
	    break;
	case(4):
	    month = string("Apr");
	    break;
	case(5):
	    month = string("May");
	    break;
	case(6):
	    month = string("Jun");
	    break;
	case(7):
	    month = string("Jul");
	    break;
	case(8):
	    month = string("Aug");
	    break;
	case(9):
	    month = string("Sep");
	    break;
	case(10):
	    month = string("Oct");
	    break;
	case(11):
	    month = string("Nov");
	    break;
	case(12):
	    month = string("Dec");
	    break;
	}

	//strftime(buf, 128, "%b %d, %Y", &tm_val);       
	stringstream ss;
	ss << month << " " << d.day() << ", " << d.year();


	return ss.str();
    }
	
    void 
    MapData::process()
    {
	cout << "MapData::process()" << endl; 

	using namespace bvdata;

	typedef data::Single<vector<File*> *> SingleFileVectorPtr;

	// time_index
	int time_index;
	if (!data::SingleInt::getInputValue(this,"time_index",time_index))
	{
	    cout << "   could not retrieve time_index" << endl;
	    return;
	}
	    
	// expression
	string expression;
	if (!data::SingleString::getInputValue(this,"expression",expression))
	{
	    cout << "   could not retrieve expression" << endl;
	    return;
	}

	// files
	vector<File*> *files;
	if (!SingleFileVectorPtr::getInputValue(this,"files",files))
	{
	    cout << "   could not retrieve expression" << endl;
	    return;
	}

	cout << "   time index " << time_index << " expression " << expression << endl;
	    
	// now we are able to process??
	Repository *repo = Global::instance()->getRepository();

	stringstream ss;

	//
	for (int i=0;i<files->size();i++) 
	{
	    int char_code = (int)('A') + i;
	    char ch = char_code;
	    ss.str("");
	    ss << ch;
	    string var_name = ss.str();

	    cout << "   file " << i << " will be var " << var_name << endl;
	    files->at(i)->installInEvaluationEnvironmentAs(var_name);
	}
	    
	ss.str("");
	ss << "t = " << (time_index + 1);
	repo->runExpression(ss.str());

	// cout << "   trying to ls() in R..." << endl;
	// repo->runExpression("print(ls())");

	// numeric matrix
	Rcpp::NumericMatrix  M = repo->evalNumericMatrix(expression);
	if (!repo->evalOK())
	{
	    cout << "   problem evaluating matrix." << endl;
	    return;
	}

	int xcells = M.nrow();
	int ycells = M.ncol();
	    
	Grid *grid = new Grid(xcells, ycells, files->at(0)->quad);
	for (int y=0;y<ycells;y++)
	    for (int x=0;x<xcells;x++)
		grid->setValue(x,y,M(x,y));

	this->getOutputPort("grid")->setData(new data::Single<Grid *>(grid), true);


	// write date on port
// 	struct tm tm_val = files->at(0)->dates[time_index];
// 	char buf[128];
// 	// date_facet* output_facet = new date_facet();
// 	// output_facet->format("%b %d, %Y");
// 	// stringstream ss;
// 	// ss.imbue(locale(locale::classic(), output_facet));
// 	// ss << currentDate;

// 	strftime(buf, 128, "%b %d, %Y", &tm_val);
// 	string date_st(buf);

	boost::gregorian::date tm_val = files->at(0)->dates[time_index];
	string date_st = dateToString(tm_val);
	
	this->getOutputPort("date")->setData(new data::Single<string>(date_st), true);

	// cache matrix
	// repo->runExpression("print("+expression+")");

	//
	// data::Single<int> *x = 
	// 	(dynamic_cast< data::Single<string>* >  
	// 	 (getInputPort("expression")->getData()));
	//


	// // new datasets
	// dataset::DatasetRepository *dsr = 
	// 	(dynamic_cast< data::Single<dataset::DatasetRepository *>* >  
	// 	 (getInputPort("dataset")->getData()))->value;

	// int test = x->value;
	    
	// int dataset_index = test;

	// if(dataset_index == -1)
	// 	return;

	// vector<dataset::Variable *>* predictors = 
	// 	dsr->getPredictorVariables(dataset_index);
	// this->addOutputPort("predictors")->setData(
	// 	new data::Single<vector<dataset::Variable *>*> (predictors),true);

	// // old stuff
	// // array
	// datatypes::Array *array = (dynamic_cast< data::Array* >  
	// 			       (getInputPort("array")->getData()))->value;

	// // species indices
	// vector<int> *species = (dynamic_cast< data::Vector<int>* >  
	// 			    (getInputPort("species_indices")->getData()))->value;
	// ////////////////////////////////////////////////////////////////////////////

	// // time index
	// int time_index = (dynamic_cast< data::Single<int>* >  
	// 		      (getInputPort("time_index")->getData()))->value;

	// // expression
	// string expression = (dynamic_cast< data::Single<string>* >  
	// 			 (getInputPort("expression")->getData()))->value;

	// if(dsr != NULL){
	// 	Coordinate t0(5,1,2009);
	// 	Coordinate t1(28,12,2009);

	// 	datatypes::ConcreteArray *B = //dsr->getCubeGridWithoutInterpolation(250,125, dataset_index);
	// 	    dsr->getCubeGridWithoutInterpolation(250,125, 0, expression);

	// 	vector<int> slice_dim(1);
	// 	vector<int> slice_dim_index(1);

	// 	slice_dim[0] = 0;
	// 	slice_dim_index[0] = time_index;

	// 	datatypes::Array *a = B->slice(&slice_dim, &slice_dim_index);
	// 	A = a->copy();			
		
	// 	delete a;
	// }

	// this->getOutputPort("result")->setData(new data::Array(A,true), true);

	// //set species changed signal
	// this->getOutputPort("species_changed")->setData(new data::Bool(true), true);

	// data::Single<vector<selection::Selection*> *> *d = 
	// 	dynamic_cast<data::Single<vector<selection::Selection*> *> *>
	// 	(getInputPort("selections")->getData());

	// vector<selection::Selection *> *selections = NULL;
	// int numSelections = 0;
	// if (d != NULL)
	// {
	// 	selections = d->value;
	// 	numSelections = selections->size();
	// }

	// //set curves
	// double colorNewCurve[4] = {1.0,1.0,0.0,0.0};
	// curves_new_dataset->clear();
	    
	// ConcreteArray* graph = dsr->getSumVectorWithoutInterpolation(dataset_index, 125, 250, NULL, expression);

	// datatypes::Curve* mainCurve = new datatypes::Curve(graph,colorNewCurve);

	// curves_new_dataset->push_back(mainCurve);

	// for(int l = 0 ; l < numSelections ; l++){
	// 	selection::Selection* curSelection = selections->at(l);

	// 	ConcreteArray* selGraph = 
	// 	    dsr->getSumVectorWithoutInterpolation(dataset_index, 125, 250,
	// 						  curSelection, expression);	    
	// 	double color[4];
	// 	color[0] = curSelection->color.r;
	// 	color[1] = curSelection->color.g;
	// 	color[2] = curSelection->color.b;
	// 	color[3] = curSelection->color.a;

	// 	datatypes::Curve* selCurve = 
	// 	    new datatypes::Curve(selGraph,color);		

	// 	curves_new_dataset->push_back(selCurve);
	// }
	    

	// data::Vector<datatypes::Curve*> *curvesNewDatasetDataFlow =
	// 	new data::Vector<datatypes::Curve*>(curves_new_dataset);
	    
	// this->getOutputPort("curves_new_dataset")->setData(curvesNewDatasetDataFlow, true);
	    
	// prev_species_indices = species;
    }

    /***********************
    * SelectionsToTagCloud *
    ************************/

    SelectionsToTagCloud::SelectionsToTagCloud(): 
	Module("SelectionsToTagCloud")
    {
	this->addInputPort("time_index");
	this->addInputPort("files");
	this->addInputPort("selections");
	this->addInputPort("world_window");
	this->addInputPort("screen_window");

	this->addOutputPort("tag_clouds");
    }

    void 
    SelectionsToTagCloud::world2screen(double x, double y, double &xx, double &yy)
    {
	// update viewport (to range in pixel size correctly)
	ViewRect screen_window;
	if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	    return;

	ViewRect world_window;
	if (!data::SingleViewRect::getInputValue(this,"world_window",world_window))
	    return;

	// cout << "   world_window  "<< world_window.str() << endl;
	// cout << "   screen_window  "<< screen_window.str() << endl;

	double px = screen_window.x;
	double py = screen_window.y;
	double pw = screen_window.w;
	double ph = screen_window.h;
	int side = std::max(pw,ph);

	px = (int)(px + (pw - side) / 2);
	py = (int)(py + (ph - side) / 2);

	double coef_x = (x - world_window.x)/(world_window.w);
	double coef_y = (y - world_window.y)/(world_window.h);

	xx = px + coef_x * side;
	yy = py + coef_y * side;

    } // end view2world

	
    void 
    SelectionsToTagCloud::process()
    {
	cout << "SelectionsToTagCloud::process()" << endl; 

	using namespace selection;
	using namespace bvdata;

	typedef data::Single<vector<File*> *> SingleFileVectorPtr;
	typedef data::Single<vector<Selection*> *> SingleSelectionVectorPtr;
	typedef data::Single<vector<TagCloudSpec*> *> SingleTagCloudSpecVectorPtr;

	// time_index
	int time_index;
	if (!data::SingleInt::getInputValue(this,"time_index",time_index))
	{
	    cout << "   could not retrieve time_index" << endl;
	    return;
	}
	    
	// files
	vector<File*> *files;
	if (!SingleFileVectorPtr::getInputValue(this,"files",files))
	{
	    cout << "   could not retrieve expression" << endl;
	    return;
	}

	// files
	vector<selection::Selection*> *selections;
	if (!SingleSelectionVectorPtr::getInputValue(this,"selections",selections))
	{
	    cout << "   could not retrieve selections" << endl;
	    return;
	}

	// now we are able to process??
	Repository *repo = Global::instance()->getRepository();

	// assume the species 

	vector<TagCloudSpec *> *tag_clouds = new vector<TagCloudSpec *>();

	// check status of selections
	for (int i=0;i<selections->size();i++)
	{
	    Selection *sel = selections->at(i);

	    if (sel->render_mode != Selection::TAG_CLOUD)
		continue;

	    // make specific behavior of using all
	    // the variables of 
	    cout << "   found selection in Tag Cloud State" << endl;


	    TagCloudSpec *tcs = new TagCloudSpec();
	    for (int j=0;j<files->size();j++)
	    {
		File *file = files->at(j);

		double weight = 
		    file->sumVariableOnSpatialQuadRange("p",sel->quad,time_index);

		tcs->addTag(file->getName(),weight);

		cout << "   add tag "<< file->getName() << " weight " << weight << endl;
	    }

	    double xw = sel->quad.x;
	    double yw = sel->quad.y;
	    double ww = sel->quad.w;
	    double hw = sel->quad.h;

	    cout << "   selection quad  "<< sel->quad.str() << endl;

	    double x0, y0, x1, y1;
		
	    world2screen(xw, yw, x0, y0);
	    world2screen(xw + ww, yw + hw, x1, y1);

	    tcs->window.init(x0, y0, x1 - x0, y1-y0);


	    ViewRect vr(x0,y0,x1-x0,y1-y0);
	    cout << "   tag cloud window  "<< vr.str() << endl;


	    // add tag cloud
	    tag_clouds->push_back(tcs);

	}

	this->getOutputPort("tag_clouds")->
	    setData(new SingleTagCloudSpecVectorPtr(tag_clouds), true);
    }

}
