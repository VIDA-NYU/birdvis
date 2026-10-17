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

#include <iostream>

#include "DataflowTypes.hpp"
#include "PlotModules.hpp"
#include "RenderSystem.hpp"

#include "Labeling.hpp"
#include "Global.hpp"


using namespace dataflow;
using namespace std;

#define xDEBUG_PLOTCONTROLLER
#define xDEBUG_PLOTRENDERER

/*******************
 * PlotController *
 *******************/

PlotController::PlotController():
    Node(), 
    PickableObject()
{
}

void
PlotController::setPlotRenderer(PlotRenderer* renderer)
{
    this->plot_renderer = renderer;
}

bool 
PlotController::event_mouse_contains(int x, int y) 
{ 
    return screen_window.contains(x,y);
}

string 
PlotController::event_node_name(eventsystem::State *event_state) 
{ 
    return "PlotController"; 
}

void 
PlotController::event_mouse_mouse(eventsystem::State *event_state) 
{
}

eventsystem::PickableObject*
PlotController::event_mouse_pick(eventsystem::State *event_state) 
{ 
    if (event_state->button == eventsystem::LEFT)
	return this;
    else
	return NULL;
}

void
PlotController::rectangleSignal(rectanglelayout::Rectangle r)
{
    screen_window.init(r.x,r.y,r.w,r.h);
}

bool 
PlotController::event_mouse_wheel(eventsystem::State *event_state) 
{ 
    // cout << "(PlotController) event_mouse_wheel " << event_state->wheel_dx << endl;

    if (plot_renderer == NULL)
	return false;

    ViewRect vr = plot_renderer->getWorldWindow();

    float x_min_view = vr.x;
    float x_max_view = vr.x + vr.w;
    float y_min_view = vr.y;
    float y_max_view = vr.y + vr.h;


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
	
    vr.init(x_min_view, y_min_view, x_max_view-x_min_view, y_max_view-y_min_view);
    plot_renderer->setWorldWindow(vr);
    return true;
}

// PickableObject *mm_event_mouse_drop(State *event_state) { return NULL; }

// int mm_size() { return 0; }

// Node *mm_get_child(int index) { return NULL; }

void 
PlotController::event_mouse_drag(eventsystem::State *event_state) 
{

    double dx_screen = event_state->x - event_state->last_x;
    double dy_screen = event_state->y - event_state->last_y;

    double tx = dx_screen/screen_window.w;
    double ty = dy_screen/screen_window.h;

    ViewRect ww = plot_renderer->getWorldWindow();

    double x = ww.x;
    double y = ww.y;
    double w = ww.w;
    double h = ww.h;

    float x_min_view = ww.x;
    float x_max_view = ww.x + ww.w;
    float y_min_view = ww.y;
    float y_max_view = ww.y + ww.h;

    // double side = max(screen_window.w,screen_window.h);
    double dx_world = tx * w;
    double dy_world = ty * h;

    y_min_view += -dy_world;
    y_max_view += -dy_world;

    x_min_view += -dx_world;
    x_max_view += -dx_world;

    ww.x = x_min_view;
    ww.w = x_max_view - x_min_view;
    
    ww.y = y_min_view;
    ww.h = y_max_view - y_min_view;
	    

    plot_renderer->setWorldWindow(ww);

}

/****************
 * PlotRenderer *
 ****************/

PlotRenderer::PlotRenderer():
    Module("PlotRenderer")
{
    // this->addInputPort("curves");
    // this->addInputPort("time_index");

    this->addInputPort("screen_window");

    this->addInputPort("curves");

    this->addInputPort("time_indices");

    this->addInputPort("title");

    this->addInputPort("file");
    
    // world_window.init(-10,-20,360,650);
    // world_window.init(-10,-20,360,4000);

    ViewRect v;
    v.x = -1;
    v.w = 370;
    v.y = ColorConstants::NORMALIZE_CURVES_MIN_PLOT_Y;
    v.h = ColorConstants::NORMALIZE_CURVES_MAX_PLOT_Y - ColorConstants::NORMALIZE_CURVES_MIN_PLOT_Y;

    world_window = v;


    // this->addInputPort("set_view_global_graph");
    // this->addInputPort("default");
    // this->addInputPort("time_coords");
    // this->addOutputPort("default");
}

ViewRect
PlotRenderer::getWorldWindow()
{
    return world_window;
}

void
PlotRenderer::setWorldWindow(ViewRect ww)
{
    this->outdated = true;
    world_window = ww;
}

void
PlotRenderer::process()
{

    typedef data::Single<Graph *> SingleGraphPtr;

#ifdef DEBUG_PLOTRENDERER
    cout << "PlotRenderer::process" << endl;
#endif

    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	return;

#ifdef DEBUG_PLOTRENDERER
    cout << "   window.....OK" << endl;
#endif

    vector<Data*> vec_data;

    // cursors
    vector<int> time_indices;
    vec_data = this->getInputPort("time_indices")->getAllData();



    for (int i=0;i<vec_data.size();i++)
    {
        Data *dd = vec_data[i];
	if (dd == NULL)
	    continue;
	try
	{
	    data::SingleInt *d = dynamic_cast<data::SingleInt*>(dd);
	    time_indices.push_back(d->value);
	}
	catch (...) {
#ifdef DEBUG_PLOTRENDERER
	    cout << "   could not read time index" << endl;
#endif
	}
    }
	
    // title
    string title;
    if (!data::SingleString::getInputValue(this,"title",title))
	title = "Plot";

#ifdef DEBUG_PLOTRENDERER
    cout << "   Plot Title: " << title << endl;
#endif

    // graph curves
    Graph* graph;
    vector<Graph*> graphs;
    vec_data = this->getInputPort("curves")->getAllData();
    for (int i=0;i<vec_data.size();i++)
    {
        Data *dd = vec_data[i];
	if (dd == NULL)
	    continue;
	try
	{
	    SingleGraphPtr *d = dynamic_cast<SingleGraphPtr *>(dd);
	    graphs.push_back(d->value);
	}
	catch (...) {

#ifdef DEBUG_PLOTRENDERER
	    cout << "   could not read curve" << endl;
#endif
	}
    }


#ifdef DEBUG_PLOTRENDERER
    cout << "   reading file" << endl;
#endif


    //get File
    bvdata::File* speciesFile;
    
    if (!data::Single<bvdata::File*>::getInputValue(this,"file",speciesFile))
    {
#ifdef DEBUG_PLOTRENDERER
	cout << "   could not retrieve species file" << endl;
#endif
	return;
    }

#ifdef DEBUG_PLOTRENDERER
    cout << "   file = " << speciesFile << endl;
#endif


    // vector<Graph*> graphs;
    // graphs.push_back(graph);

    // using namespace boost::gregorian;

    // vector<date> *time_coords = 
    // 	(dynamic_cast< data::Single<vector<date>*> *>(  
    // 	    getInputPort("time_coords")->getData()))->value;

    bvdata::Repository *repo = 
	Global::instance()->getRepository();

    //
    date day0;
    date day1;

    if(speciesFile == NULL){
	day0 = repo->getDefaultDate(0);
	day1 = repo->getDefaultDate(repo->getNumDefaultDates()-1);
    }
    else{
	day0 = speciesFile->getDate(0);
	int numTimeSteps = speciesFile->getNumTimeSteps();
	day1 = speciesFile->getDate(numTimeSteps - 1);
    }

    // vector<datatypes::Curve*>* curves = NULL;
	    
    // data::Vector<datatypes::Curve*> *curvesVector = 
    //   dynamic_cast< data::Vector<datatypes::Curve*>* >  
    // 	(getInputPort("curves")->getData());

    float x_min_view = world_window.x;
    float x_max_view = world_window.x + world_window.w;
    float y_min_view = world_window.y;
    float y_max_view = world_window.y + world_window.h;

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

    renderSystem->setColor(ColorConstants::PLOT_BG);
    renderSystem->drawQuad(screen_window.x,screen_window.y,
			   screen_window.w,screen_window.h);

    string measure = "Apr 31, 2009";
    ViewRect vr = renderSystem->textBoundingBox(measure.c_str(),14,0,0);
    int width_fit = ww / vr.w;
    int height_fit = hh / vr.h;
 
    int xticks = floor(0.6 * width_fit);
    int yticks = floor(0.5 * height_fit);

    // delete before exit: ugly...
    vector<double>* xlabels;
    vector<double>* ylabels;

    { // define ylabels
	if (yticks > 0)
	{ // draw y-labels
	    Labeling lbl;
	    double Q[] = {1,5,2,2.5,4,3};
	    double W[] = {0.25,0.2,0.5,0.05}; 


#ifdef DEBUG_PLOTRENDERER
    cout << "   Creating Y Labels" << endl;
    cout << "   y_min_view " << y_min_view << endl;
    cout << "   y_min_view " << y_max_view << endl;
#endif
    if(y_min_view >= y_max_view){
	cout << "Limits are equal" << endl;
	y_min_view -= 0.5;
	y_min_view += 0.5;
    }

	    ylabels = lbl.labelExtended(y_min_view, y_max_view, 
					yticks, Q, 6, 
					true, W);

#ifdef DEBUG_PLOTRENDERER
    cout << "   Y Labels Created" << endl;
#endif

	}
	else {
	    ylabels = new vector<double>();
	}
    } // define ylabels

    { // define xlabels
	if (xticks > 0)
	{ // draw x-labels
#ifdef DEBUG_PLOTRENDERER
    cout << "   Creating X Labels" << endl;
#endif

	    Labeling lbl;


#ifdef DEBUG_PLOTRENDERER
    cout << "   X Labels Created" << endl;
#endif


	    double Q[] = {1,5,2,2.5,4,3};
	    double W[] = {0.25,0.2,0.5,0.05}; 
	    xlabels = lbl.labelExtended(x_min_view, x_max_view, 
					xticks, Q, 6, 
					true, W);
	}
	else {
	    xlabels = new vector<double>();
	}
    } // xlabels

#ifdef DEBUG_PLOTRENDERER
    cout << "   xticks = " << xticks << endl;
    cout << "   yticks = " << yticks << endl;
    cout << "   y_min_value = " << y_min_view << endl;
    cout << "   y_max_value = " << y_max_view << endl;
#endif

    { // draw grid
	for (int i=0;i<ylabels->size();i++)
	{
	    double currVal =  ylabels->at(i);
	    if (abs(currVal) < 1.0e-8)
		currVal = 0.0;
	    float  text_y0 = yy + hh * (currVal - y_min_view)/(y_max_view - y_min_view);
	    renderSystem->setColor(ColorConstants::PLOT_GRID);
	    renderSystem->drawSegment(xx, text_y0, xx + ww, text_y0);
	}
	for (int i=0;i<xlabels->size();i++)
	{
	    double currVal =  (int) xlabels->at(i);
	    double text_x0 = xx + (currVal-x_min_view)/(x_max_view-x_min_view) * ww;
	    renderSystem->setColor(ColorConstants::PLOT_GRID);
	    renderSystem->drawSegment(text_x0, yy, text_x0, yy+hh);
	}
    } // draw grid

    { // draw cursors
	for (int i=0;i<time_indices.size();i++)
	{
	    float currVal;
	    if(speciesFile == NULL)
		currVal = (repo->getDefaultDate(time_indices[i]) - day0).days();
	    else
		currVal = (speciesFile->getDate(time_indices[i]) - day0).days();


	    date_facet* output_facet = new date_facet();
	    output_facet->format("%b %d, %Y");
	    stringstream ss;
	    ss.imbue(locale(locale::classic(), output_facet));
	    date d_i = day0 + days(currVal);
	    ss.str("");
	    ss << d_i;



	    double text_x0 = xx + (currVal-x_min_view)/(x_max_view-x_min_view) * ww;
	    renderSystem->setColor(ColorConstants::PLOT_CURSORS);
	    glLineWidth(2.0);
	    renderSystem->drawSegment(text_x0, yy, text_x0, yy+hh);
	    glLineWidth(1.0);

	    float tw = 5;
	    float th = 5;
	    
	    renderSystem->drawTriangle(text_x0 - tw, yy + hh,
				       text_x0,      yy + hh - th,
				       text_x0 + tw, yy + hh);

	    renderSystem->drawTriangle(text_x0 - tw, yy ,
				       text_x0,      yy  + th,
				       text_x0 + tw, yy );

	    renderSystem->setColor(0,0,0,0.45);
	    renderSystem->pushMatrix();
	    renderSystem->translate(text_x0, yy + hh/2.0);
	    renderSystem->rotate(90);
	    renderSystem->drawHorizontallyCenteredText(ss.str().c_str(), 13, 0, 3);
	    renderSystem->popMatrix();
	    
	}

    } // draw cursors


    // if (time_indices.size() == 1) 
    // { // draw date if a single time_index

    // 	date_facet* output_facet = new date_facet();
    // 	output_facet->format("%b %d, %Y");
    // 	stringstream ss;
    // 	ss.imbue(locale(locale::classic(), output_facet));

    // 	float currVal = (repo->getDefaultDate(time_indices[0]) - day0).days();
    // 	// double currVal =  (int) xlabels->at(i);
		    
    // 	// cout << "currVal " << currVal << endl;
    // 	// cout << "day0 "    << day0 << endl;
    // 	date d_i = day0 + days(currVal);
    // 	// cout << "d_i     "    << d_i << endl;
	
    // 	ss.str("");
    // 	ss << d_i;

    // 	string date_st = ss.str();
	
    // 	double tx = xx + 4;
    // 	double ty = yy + hh - 28;
    // 	double tw = 120;
    // 	double th = 24;

    // 	renderSystem->setColor(0.2,0.2,0.2,0.8);
    // 	renderSystem->drawQuad(tx,ty,tw,th);

    // 	renderSystem->setColor(1,1,1);
    // 	renderSystem->drawHorizontallyCenteredText(date_st.c_str(), 
    // 						   16, tx + tw/2.0, ty+6);

    // } // draw cursors

#ifdef DEBUG_PLOTRENDERER
    cout << "   number of graphs is  = " << graphs.size() << endl;
#endif

    { // draw curves		

    	renderSystem->pushMatrix();
    	renderSystem->translate(xx,yy);

    	float sx = ww/(1.0*(x_max_view-x_min_view));
    	float sy = (1.0*hh)/(y_max_view-y_min_view);

    	renderSystem->scale(sx,sy);

    	renderSystem->translate(-x_min_view,-y_min_view);

	

    	for(int i = 0 ; i < graphs.size() ; i++){
		    
	    Graph* graph = graphs[i];

    	    int numPoints = graph->curve.size();

#ifdef DEBUG_PLOTRENDERER
	    if(speciesFile != NULL && numPoints !=  speciesFile->getNumTimeSteps()){
		cout << "   The number of points in graph is different from the one in the data file!!!!" << graphs.size() << endl;
		return;
	    }
#endif

    	    // double color[4];
    	    // curve->getColor(color);

    	    float curve_points[2 * numPoints];

    	    for(int j = 0 ; j < numPoints ; j++){

    		double val_y = graph->curve[j];
    		date   d_j;
		if(speciesFile == NULL)
		    d_j = repo->getDefaultDate(j);
		else
		    d_j = speciesFile->getDate(j);

    		double val_x = (d_j - day0).days();

    		curve_points[2*j]   = val_x;
    		curve_points[2*j+1] = val_y;

    		// cout << "x, y " << val_x << ", " << val_y << endl;

    	    }

	    glLineWidth(2.0);

    	    renderSystem->setColor(graph->color); // color[0],color[1],color[2]);
    	    renderSystem->drawLineStrip(numPoints, curve_points);

	    glLineWidth(1.0);

    	    // cout << "r,g,b,a " << color[0] << "," << color[1] << 
    	    // "," << color[2] << "," << color[3] << endl;
		    
    	}

    	// cout << "sx, sy " << sx << ", " << sy << endl;

    	renderSystem->popMatrix();

    } // end curves

    { // draw frame in 4 rectangles semi opaque rectangles

	renderSystem->setColor(ColorConstants::PLOT_FRAME_BG);
	renderSystem->drawQuad(x, y, w, margin_bottom);	
	renderSystem->drawQuad(x, yy+hh, w, margin_top);
	renderSystem->drawQuad(x, yy, margin_left, hh);
	renderSystem->drawQuad(xx+ww, yy, margin_right, hh);
    } // draw frame in 4 rectangles semi opaque rectangles

    { // draw labels

	for (int i=0;i<ylabels->size();i++)
	{
	    double currVal =  ylabels->at(i);
		    
	    if (abs(currVal) < 1.0e-8)
		currVal = 0.0;

	    char text[64];
	    sprintf(text,"%.7g",currVal);
		    
	    double text_height = 14;
		    
	    double text_x0 = xx + ww + 3;
	    float  text_y0 = yy + hh * (currVal - y_min_view)/(y_max_view - y_min_view);
		    
	    renderSystem->setColor(ColorConstants::PLOT_TEXT);
	    renderSystem->drawText(text, text_height, floor(text_x0), floor(text_y0));

	} // y-labels


	date_facet* output_facet = new date_facet();
	output_facet->format("%b %d, %Y");
	stringstream ss;
	ss.imbue(locale(locale::classic(), output_facet));

	for (int i=0;i<xlabels->size();i++)
	{
	    double currVal =  (int) xlabels->at(i);
		    
	    // cout << "currVal " << currVal << endl;
	    // cout << "day0 "    << day0 << endl;
	    date d_i = day0 + days(currVal);
	    // cout << "d_i     "    << d_i << endl;

	    ss.str("");
	    ss << d_i;

	    double text_height = 14;
		    
	    double text_x0 = xx + (currVal-x_min_view)/(x_max_view-x_min_view) * ww;
	    float  text_y0 = yy - 17;
		    
	    renderSystem->setColor(ColorConstants::PLOT_TEXT);
	    renderSystem->drawHorizontallyCenteredText(
		ss.str().c_str(), text_height, floor(text_x0), floor(text_y0));

	}

    } // draw labels

    delete xlabels;
    delete ylabels;

    { // draw title
	
	// ViewRect bb    = renderSystem->textBoundingBox(title.c_str(), 14.0, 0, 0);
	// float fontSize = min(.0, floor (screen_window.h * 0.7)/bb.h);

    	double tw = 30;
    	double th = 20;
    	double tx = xx + ww/2.0; // screen_window.w - 4 - tw;
    	double ty = yy + hh + 6;

    	// renderSystem->setColor(0.1,0.1,0.1,0.8);
    	// renderSystem->drawQuad(tx,ty,tw,th);

    	renderSystem->setColor(ColorConstants::PLOT_TITLE_TEXT);
    	renderSystem->drawHorizontallyCenteredText(title.c_str(), 16, tx, ty);

    } // end draw title
	     
    renderSystem->endViewport();


} // end process
	






