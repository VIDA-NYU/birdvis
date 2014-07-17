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

// temporary (this should not be here)
#include <QDialog>

#include "ColormapModules.hpp"
#include "RenderSystem.hpp"

#include "DataflowTypes.hpp"

#include "Global.hpp"

#include "Labeling.hpp"

#include "bvdata.hpp"


using namespace std;
using namespace dataflow;

#define xDEBUG_COLORMAPMODULES
#define xDEBUG_COLORMAPRENDERER

/************************
 * ColormapRendererDesc *
 ************************/
ColormapRendererDesc::ColormapRendererDesc(){
    this->parentColormap = NULL;
}

// ColormapRendererDesc::ColormapRendererDesc(Colormap1d *colormap, 
// 					   ColormapRenderer::Handle newRange, ColormapRenderer::Handle newColorbar,
// 					   double xx, double yy, double ww, double hh, double min_value, double max_value){
//     double r = 7.5;//handle_radius;
//     double aw = 15;
//     double ah = 18;

    
//     this->range = newRange;
//     this->colorbar = newColorbar;

//     this->parentColormap = colormap;

//     int numberOfStops = colormap->stops.size();

//     for(int i = 0 ; i < numberOfStops ; i++){
// 	Colormap1d::Stop *stop = (*colormap)[i];
		
// 	double stop_coef = (stop->getValue() - min_value) / (max_value - min_value);

// 	ColormapRenderer::Handle currentHandle(ColormapRenderer::Handle::HANDLE, 
// 					       xx + ww * stop_coef -  aw/2.0, yy + hh + 2, aw,ah);
// 	currentHandle.stop = stop;
// 	this->handles.push_back(currentHandle);
//     }

//     Colormap1d::Stop *stop = (*colormap)[0];
//     stop->active = true;

//     stop = (*colormap)[numberOfStops - 1];
//     stop->active = true;
// }

void ColormapRendererDesc::init(Colormap1d *colormap, 
				ColormapRenderer::Handle newRange, 
				ColormapRenderer::Handle newColorbar,
				double xx, double yy, 
				double ww, double hh, 
				double min_value, double max_value)
{    
    double aw = 13;
    double ah = 13;

    this->handles.clear();

    this->range = newRange;
    this->colorbar = newColorbar;

    this->parentColormap = colormap;

    int numberOfStops = colormap->stops.size();

    for(int i = 0 ; i < numberOfStops ; i++){
	Colormap1d::Stop *stop = (*colormap)[i];
		
	double stop_coef = (stop->getValue() - min_value) / (max_value - min_value);

	ColormapRenderer::Handle currentHandle(ColormapRenderer::Handle::HANDLE, 
					       xx + ww * (stop_coef) -  aw/2.0, yy + hh - ah/2.0, aw,ah);
	currentHandle.stop = stop;
	this->handles.push_back(currentHandle);
    }

    Colormap1d::Stop *stop = (*colormap)[0];
    stop->active = true;

    stop = (*colormap)[numberOfStops - 1];
    stop->active = true;
}

    
ColormapRenderer::Handle ColormapRendererDesc::getRangeHandle(){
    return this->range;
}

void ColormapRendererDesc::setRangeHandle(ColormapRenderer::Handle newRange){
    this->range = newRange;
}

ColormapRenderer::Handle ColormapRendererDesc::getColorBarHandle(){
    return this->colorbar;
}

void ColormapRendererDesc::setColorbarHandle(ColormapRenderer::Handle newColorbar){
    this->colorbar = newColorbar;
}

int ColormapRendererDesc::getNumColormapHandles(){
    return this->handles.size();
}

ColormapRenderer::Handle ColormapRendererDesc::getColormapHandle(int index){
    return this->handles.at(index);
}


/****************************
 * ColormapRenderer::Handle *
 ****************************/

ColormapRenderer::Handle::Handle()
{
    this->type = ColormapRenderer::Handle::NONE;
    this->x = 0;
    this->y = 0;
    this->w = 0;
    this->h = 0;
}

ColormapRenderer::Handle::Handle(Type type, double x, double y, double w, double h)
{
    this->type = type;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}
    
bool 
ColormapRenderer::Handle::contains(double xx, double yy)
{
    return (x <= xx && xx < x + w && y <= yy && yy < y + h); 
}


void ColormapRenderer::Handle::setBox(double xx, double yy, double ww, double hh)
{
    x = xx;
    y = yy;
    w = ww;
    h = hh;
}

bool ColormapRenderer::Handle::isActive(){
    return this->stop->active;
}

void ColormapRenderer::Handle::setActive(bool active){
    this->stop->active = active;
}

/********************
 * ColormapRenderer *
 ********************/

ColormapRenderer::ColormapRenderer(): 
    Module("ColormapRenderer")
{
	
    // input ports
    this->addInputPort("default");
    this->addInputPort("colormap");
    this->addInputPort("range");
    this->addInputPort("screen_window");
    this->addInputPort("title");
	
    // output ports
    this->addOutputPort("default");
	
    this->colormapDescription = new ColormapRendererDesc();
}

ColormapRenderer::Handle*
ColormapRenderer::getRangeBar(){
    return &(this->colormapDescription->range);
}

void 
ColormapRenderer::toogleActiveNearestHandle(double x, double y){

    //look for activate handles to hide    
    int numberOfHandles = this->colormapDescription->getNumColormapHandles();
    if(numberOfHandles < 2)
	return;

    for(int i = numberOfHandles - 1 ; i > 0  ; i--){
	Handle currentHandle = this->colormapDescription->getColormapHandle(i);	    	    
	    
	if(currentHandle.contains(x,y) && currentHandle.isActive()){			   
	    currentHandle.setActive(false);
	    return;
	}	    
    }

    // activate the nearest handle
    int minIndex = numberOfHandles - 2;
    Handle currentHandle = this->colormapDescription->getColormapHandle(numberOfHandles - 2);	    	    
    double minDist = ((x - (currentHandle.x + currentHandle.w/2.0)) * 
		      (x - (currentHandle.x + currentHandle.w/2.0))) + 
	((y - (currentHandle.y + currentHandle.h/2.0)) * 
	 (y - (currentHandle.y + currentHandle.h/2.0)));
	
    for(int i = numberOfHandles - 3 ; i > 0  ; i--){
	currentHandle = this->colormapDescription->getColormapHandle(i);
	    
	double dist = ((x - (currentHandle.x + currentHandle.w/2.0)) * 
		       (x - (currentHandle.x + currentHandle.w/2.0))) + 
	    ((y - (currentHandle.y + currentHandle.h/2.0)) * 
	     (y - (currentHandle.y + currentHandle.h/2.0)));

	if(dist < minDist){		
	    minIndex = i;
	    minDist = dist;
	}	    
    }

    currentHandle = this->colormapDescription->getColormapHandle(minIndex);

    if(!currentHandle.isActive()){	
	currentHandle.setActive(true);
    }
}

TypeAndIndex
ColormapRenderer::pick(double x, double y)
{
#ifdef DEBUG_COLORMAPMODULES
    cout << "====> ColormapRenderer.pick(" << x << "," << y << ")" << endl;
#endif

    TypeAndIndex ret;
    int numberOfHandles = this->colormapDescription->getNumColormapHandles();
    
    for(int i = numberOfHandles - 1 ; i >= 0  ; i--){
	Handle currentHandle = this->colormapDescription->getColormapHandle(i);

	if(currentHandle.contains(x,y) && currentHandle.isActive()){		
	    ret.t = currentHandle.type;
	    ret.stopIndex = i;		
	    return ret;
	}
    }

    //colorbar
    Handle colorbarHandle = this->colormapDescription->getColorBarHandle();	    	    

    if(colorbarHandle.contains(x,y)){		

	ret.t = colorbarHandle.type;
	ret.stopIndex = -1;		
	return ret;
    }

    //range
    Handle rangeHandle = this->colormapDescription->getRangeHandle();	    	   

    if(rangeHandle.contains(x,y)){		

	ret.t = rangeHandle.type;
	ret.stopIndex = -1;		
	return ret;
    }
	

    ret.t = Handle::NONE;

    return ret;
}

void 
ColormapRenderer::process()
{

#ifdef DEBUG_COLORMAPRENDERER
    cout << "ColormapRenderer::process()" << endl;
#endif

    Colormap1d *colormap;
    if (!data::SingleColormap1dPtr::getInputValue(this,"colormap",colormap))
	return;

#ifdef DEBUG_COLORMAPRENDERER
    cout << "    colormap number of stops: " << colormap->size() << endl;
#endif

    // cout << "   colormap...OK" << endl;

    // screen_window
    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	return;


    // screen_window
    string title;
    if (!data::SingleString::getInputValue(this,"title",title))
	title = "";

    // cout << "   window....OK" << endl;

    // range
    data::Vector<double> *d = NULL;
    d = dynamic_cast< data::Vector<double>* >  
	(getInputPort("range")->getData());
    vector<double> *range = d->value;
    
    Colormap1d::Stop *first =  (*colormap)[0];
    Colormap1d::Stop *last  =  (*colormap)[colormap->size()-1];

    double min_value = range->at(0);
    double max_value = range->at(1);

    // cout << "         range " << range << " " << min_value << "," << max_value << endl;
	   
    // cout << "         screen_window " << screen_window.str() << endl;

    // render system
    rendersystem::RenderSystem *renderSystem = 
	Global::instance()->getRenderSystem();

    // screen window
    float x = screen_window.x;
    float y = screen_window.y;
    float w = screen_window.w;
    float h = screen_window.h;

    if(w == 0 || h == 0)
	return;

    float xx = x + margin_left;
    float yy = y + margin_bottom;
    float ww = w - margin_left - margin_right;
    float hh = h - margin_top - margin_bottom;

    // start drawing session of elements in a specific viewport
    renderSystem->startViewport(x,y,w,h);

    // draw background
    renderSystem->setColor(ColorConstants::COLORMAP_BG); // .2,.2,.2
    renderSystem->drawQuad(x,y,w,h);

    // update handle
    Handle range_handle(Handle::RANGE, x,y,w,h);

    char buf[1024];

    // { // draw range labels
    // 	renderSystem->pushMatrix();

    // 	renderSystem->translate(xx,0);

    // 	sprintf(buf,"Colormap Range: [%10.4f          ,          %10.4f]", 
    // 		colormap->getMinValue(), colormap->getMaxValue());
		
    // 	ViewRect r(0,h - margin_top + 3,180,22);
    // 	glColor4f(0.7,0.7,0.7,0.9);
	    

    // 	double text_height = 12;
    // 	renderSystem->setColor(1,1,1);
    // 	renderSystem->drawText(buf, text_height, r.x+6,r.y+6);

    // 	renderSystem->popMatrix();
    // } // draw range labels

    { // draw labels
	
#ifdef DEBUG_COLORMAPRENDERER
	cout << "   drawing labels..." << endl;
#endif

	string measure = "XXXXX";
	ViewRect vr = renderSystem->textBoundingBox(measure.c_str(),14,0,0);
	int width_fit  = ww / vr.w;
	// int height_fit = hh / vr.h;
	int xticks = floor(0.5 * width_fit);

	xticks = max(xticks,2);

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

	vector<double>* labels = lbl.labelExtended(min_value, 
						   max_value, 
						   xticks, Q, 6, 
						   true, W);

	for (int i=0;i<labels->size();i++)
	{
	    double currVal =  labels->at(i);
		    
	    if (abs(currVal) < 1.0e-8)
		currVal = 0.0;

	    char text[64];
	    sprintf(text,"%.7g",currVal);
		    
	    glColor3f(1,1,1);
	    double text_height = 14;
		    
	    double text_x0 = xx + ww * (currVal - min_value)/(max_value - min_value);
	    // float  text_y0 = y + text_y;

	    renderSystem->drawCenteredText(text, text_height, 
					   text_x0, y, 0, 
					   margin_bottom + text_fix);


	    // renderSystem->setColor(.3,.3,.3);
	    renderSystem->setColor(.8,.8,.8);
	    renderSystem->drawSegment(text_x0, yy, 
				      text_x0, y + h);


	}

	// cout << "... finished" << endl;

    } // draw labels

    { // draw color rectangles

#ifdef DEBUG_COLORMAPRENDERER
	cout << "   drawing color rectangles..." << endl;
#endif

	renderSystem->pushMatrix();

	float sx = ww / (max_value - min_value);
	float sy = hh;

	renderSystem->translate(xx,yy);
	renderSystem->scale(sx,sy);
	renderSystem->translate(-min_value,0);

	Colormap1d::Stop *second_last  =  (*colormap)[colormap->size()-2];

// 	if (first->getValue() > min_value)
// 	{
// 	    renderSystem->setColor(first->color);
// 	    renderSystem->drawQuad(min_value,0,first->getValue()-min_value,1);
// 	}
// 	if (last->getValue() < max_value)
// 	{
// 	    if (last->kind == Colormap1d::Stop::DISCRETE)
// 		renderSystem->setColor(second_last->color);
// 	    else
// 		renderSystem->setColor(last->color);
// 	    renderSystem->drawQuad(last->getValue(),0,max_value-last->getValue(),1);
// 	}

	for (int i=0;i<colormap->size()-1;i++)
	{
	    // cout << "colormap stop " << i << endl;
	    Colormap1d::Stop *a = (*colormap)[i];
	    Colormap1d::Stop *b = (*colormap)[i+1];
		    
	    double ax = a->getValue();
	    double bx = b->getValue();

#ifdef DEBUG_COLORMAPRENDERER
	    cout << "   drawing color rectangle starting at " << ax << endl;
#endif

	    if(a->kind == Colormap1d::Stop::DISCRETE){
		renderSystem->setColor(a->color);
		renderSystem->drawQuad(ax,0.0,bx-ax,1.0);
	    }
	    else{
		//renderSystem->setColor(a->color);
		renderSystem->drawQuadWithColors(ax,0.0,bx-ax,1.0, 
						 a->color, b->color, b->color, a->color);
	    }
		
	}

	renderSystem->popMatrix();

    } // draw color rectangles


    double colorMapMinValue = colormap->getMinValue();
    double colorMapMaxValue = colormap->getMaxValue();

    double min_coef = (colorMapMinValue - min_value) / 
	(max_value - min_value);
    double max_coef = (colorMapMaxValue - min_value) / 
	(max_value - min_value);


    {// update handles

	//insert colorbar_handle
	Handle colorbar_handle(Handle::COLORBAR, 
			       xx+ww*min_coef,yy,ww*(max_coef-min_coef),hh);

// 	cout << "DESCRIPTION INIT" << endl;
// 	cout << "   min_value " << min_value << endl;
// 	cout << "   max_value " << max_value << endl;

#ifdef DEBUG_COLORMAPRENDERER
	cout << "colormapDescription->init(...)" << endl;
#endif
	this->colormapDescription->init(colormap, range_handle, 
					colorbar_handle, xx, yy, ww, hh,
					min_value, max_value);
    }// end of update handles

    { // draw handles

#ifdef DEBUG_COLORMAPRENDERER
	cout << "   drawing color handles..." << endl;
#endif

	int numberOfHandles = this->colormapDescription->getNumColormapHandles();

	for(int i = 0 ; i < numberOfHandles ; i++){
	    Handle currentHandle = this->colormapDescription->getColormapHandle(i);
	    if(currentHandle.stop->active){
		//cout << "Draw Handle " << i << " x " << currentHandle.x << " y " << currentHandle.y  << endl;
		renderSystem->drawHandle(currentHandle.x, currentHandle.y, 
					 currentHandle.w, currentHandle.h,0.5);			    
	    }
	}

    } // end draw handles
	 

    { // draw title

#ifdef DEBUG_COLORMAPRENDERER
	cout << "   drawing title..." << endl;
#endif

    	double tw = 30;
    	double th = 20;
    	double tx = screen_window.x + 4; // screen_window.w - 4 - tw;
    	double ty = screen_window.y + screen_window.h - 4 - th;

    	renderSystem->setColor(0.1,0.1,0.1,0.8);
    	renderSystem->drawQuad(tx,ty,tw,th);

    	renderSystem->setColor(1,1,1);
    	renderSystem->drawHorizontallyCenteredText(title.c_str(), 14, tx + tw/2.0, ty+6);

    } // end title

    // end drawing session of elements in specific viewport
    renderSystem->endViewport();
	    
} // show colormap


/**********************
 * ColormapController *
 **********************/

ColormapController::ColormapController(): 
    Module("ColormapController"), eventsystem::Node()
{
    colormap_index = 0;

    //init colormaps
    for(int i = 0 ; i < NUM_COLORMAPS ; i++)	
	used_colormaps[i] = ColormapController::COLORMAPS[i]->copy();


    // use default color map (composition)
    colormap = this->used_colormaps[colormap_index];//ColormapController::COLORMAPS[colormap_index]->copy();
    min_value = -0.05;
    max_value = 1.05;
    

    this->addInputPort("default");
    this->addInputPort("screen_window");

    // output
    this->addOutputPort("range");
    this->addOutputPort("screen_window");
    this->addOutputPort("default");
    this->addOutputPort("colormap");

}

void 
ColormapController::setRenderer(ColormapRenderer *renderer)
{
    this->renderer = renderer;
}

void 
ColormapController::process() 
{
    getOutputPort("colormap")->setData
	(new data::SingleColormap1dPtr(colormap));

    getOutputPort("range")->setData
	(new data::Vector<double>(min_value,max_value));

    // screen_window
    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	return;

#ifdef DEBUG_COLORMAPCONTROLLER
    cout << "ColormapController::process()" << endl;
    cout << "    screen_window " << screen_window.str() << endl;
#endif

    getOutputPort("screen_window")->setData
	(new data::SingleViewRect(screen_window));
}
	
string 
ColormapController::event_node_name(eventsystem::State *event_state) { 
    return "ColormapController"; 
}

bool 
ColormapController::event_mouse_contains(int x, int y) { 

    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	return false;

    return screen_window.contains(x,y);

}

void 
ColormapController::event_mouse_mouse(eventsystem::State *event_state) {}

eventsystem::PickableObject*
ColormapController::event_mouse_pick(eventsystem::State *event_state) { 

    if (renderer == NULL)
	return NULL;

#ifdef DEBUG_COLORMAPCONTROLLER
    cout << "ColormapController::event_mouse_pick(...)" << endl;
#endif

    if (event_state->button == eventsystem::RIGHT && !event_state->shift){

#ifdef DEBUG_COLORMAPCONTROLLER
	cout << "    Right Button" << endl;
#endif
	renderer->toogleActiveNearestHandle(event_state->x, event_state->y);
    }
    else{

#ifdef DEBUG_COLORMAPCONTROLLER
	cout << "    Left Button" << endl;
#endif

	picked_object = renderer->pick(event_state->x, event_state->y);
    }
    //cout << "   ====> picked_object " << picked_object << endl;

    return this;

}

//auxiliary function

bool 
ColormapController::event_mouse_wheel(eventsystem::State *event_state) 
{ 

#ifdef DEBUG_COLORMAPCONTROLLER
    cout << "Colormap Controller: event_mouse_wheel " 
	 << event_state->wheel_dx << endl;
#endif


    double w = (max_value - min_value);
    double c = (max_value + min_value)/2.0;
		
    double coef =  (event_state->wheel_dx > 0 ? 1.0/1.1 : 1.1);
    w *= coef;
    min_value = c - w/2.0;
    max_value = c + w/2.0;

    getOutputPort("range")->setData
	(new data::Vector<double>(min_value,max_value),true);
    return true;
}


void
ColormapController::set_colormap_stop_color(eventsystem::State *event_state) 
{
    TypeAndIndex h;
    Color  c;
    QColor color_in;
    QColor color_out;

    h = renderer->pick(event_state->x, event_state->y);

    // TODO: move the Qt dependency somewhere else
    if (h.stopIndex >= 0)
    {
	c = colormap->stops[h.stopIndex]->color;
	color_in.setRgbF(c.r,c.g,c.b);
	color_out = QColorDialog::getColor(color_in, NULL);
	if (color_out.isValid())
	{
	    colormap->stops[h.stopIndex]->color.init(color_out.redF(),
						     color_out.greenF(),
						     color_out.blueF(),
						     1.0);
	    getOutputPort("colormap")->setData(new data::SingleColormap1dPtr(colormap),true);
	}
    }
}

void
ColormapController::set_colormap_stop_kind(eventsystem::State *event_state) 
{
    TypeAndIndex h;
    Color  c;
    QColor color_in;
    QColor color_out;

    h = renderer->pick(event_state->x, event_state->y);

    // TODO: move the Qt dependency somewhere else
    if (h.stopIndex >= 0)
    {
	// c = colormap->stops[h.stopIndex]->color;
	// color_in.setRgbF(c.r,c.g,c.b);
	// color_out = QColorDialog::getColor(color_in, NULL);
	// if (color_out.isValid())
	// {
	Colormap1d::Stop::Kind kind = colormap->stops[h.stopIndex]->kind;
	colormap->stops[h.stopIndex]->kind = 
	    (kind == Colormap1d::Stop::CONTINUOUS ? 
	     Colormap1d::Stop::DISCRETE : Colormap1d::Stop::CONTINUOUS);
	getOutputPort("colormap")->setData(new data::SingleColormap1dPtr(colormap),true);
	// }
    }
}

void
ColormapController::set_colormap_stop_locations(eventsystem::State *event_state) 
{
    bool ok;
    QString text = QInputDialog::getText(NULL, 
					 QDialog::tr("QInputDialog::getText()"),
					 QDialog::tr("Stop Values:"), 
					 QLineEdit::Normal,
					 QDialog::tr("seq(0, 1, 0.2)"), 
					 &ok);
    if (!ok || text.isEmpty())
	return;

    string exp = "sort("+text.toStdString()+")";

    bvdata::Repository* R = Global::instance()->getRepository();
    
    vector<double> stops;

    // read stops
    try{
	Rcpp::NumericVector v = R->evalNumericVector(exp);
	Rcpp::NumericVector::iterator it;
	for (it=v.begin() ; it<v.end() ; it++){
	    double value = *it;
	    stops.push_back(value);
	}
    }
    catch(...){
	cout << "   Invalid R expression to generate stops" << endl;
	return;
    }

    //
    if (stops.size() >= 2)
    {
	colormap->setStops(stops);
	getOutputPort("colormap")->setData(new data::SingleColormap1dPtr(colormap),true);
    }
}


bool 
ColormapController::event_key_press(eventsystem::State *event_state) 
{
#ifdef DEBUG_COLORMAPCONTROLLER
    cout << "OccurrenceEnvironment::event_key_press()" << endl;
#endif
    int numStops;


    switch(event_state->key)
    {
    case eventsystem::Key_Tab:
	this->another_colormap(+1);	    
	break;
    case eventsystem::Key_Q:
	
	numStops = colormap->stops.size();
#ifdef DEBUG_COLORMAPCONTROLLER
	cout << "Colormap Summary" << endl;
	cout << "Number of Stops " << numStops << endl;
#endif
	for(int i = 0 ; i < numStops ; i++){
	    cout << "Stop " << i << " = " << colormap->stops.at(i)->value << endl;
	}
	break;

    case eventsystem::Key_Space:
	// if on top of a handle change its color by a custom color
	this->set_colormap_stop_color(event_state);
	break;
    case eventsystem::Key_C:
	// if on top of a handle change its color by a custom color
	this->set_colormap_stop_kind(event_state);
	break;
    case eventsystem::Key_X:
	this->set_colormap_stop_locations(event_state);
	break;
    default:
	return false;
    }
    return false;
}

// PickableObject *mm_event_mouse_drop(State *event_state) { return NULL; }

// int mm_size() { return 0; }

// Node *mm_get_child(int index) { return NULL; }

void 
ColormapController::event_mouse_drag(eventsystem::State *event_state) 
{
    if (event_state->button == eventsystem::LEFT){
#ifdef DEBUG_COLORMAPMODULES
    cout << "$$$$$$$$$$$$$$$$$$$$$ event_mouse_drag " << picked_object.t << endl;
#endif
    if (picked_object.t == ColormapRenderer::Handle::NONE)
	return;
	
    double rx,ry,rw,rh;

    double ww = renderer->getRangeBar()->w;
    //cout << "    ww " << ww << endl;
	
    if(ww <= 1.0e-10)
	return;


    double delta_x     = event_state->x - event_state->last_x;
    double delta_value = (delta_x/ww) * (max_value - min_value);


    if (picked_object.t == ColormapRenderer::Handle::HANDLE)
    {
#ifdef DEBUG_COLORMAPMODULES
	cout << "  HANDLE OBJECT" << endl;
#endif
	Colormap1d::Stop* pickedStop = 
	    colormap->stops.at(picked_object.stopIndex);
	
	bool left  = delta_value < 0;
	bool right = delta_value > 0;

	int  num_stops = colormap->stops.size();

	int  active_index = picked_object.stopIndex;
	int  bound_index_left;
	int  bound_index_right;

	//get left bound
	bound_index_left = -1;
	for(int i = active_index - 1 ; i>=0 ; i--){
	    ColormapRenderer::Handle currentHandle = 
		this->renderer->colormapDescription->getColormapHandle(i);
	    
	    if (currentHandle.isActive())
	    {
		bound_index_left = i;
		break;
	    }
	}

	//get the right bound
	bound_index_right = num_stops;
	for(int i = active_index+1 ; i<num_stops ; i++){
	    ColormapRenderer::Handle currentHandle = 
		this->renderer->colormapDescription->getColormapHandle(i);
	    
	    if (currentHandle.isActive())
	    {
		bound_index_right = i;
		break;
	    }
	}

	if (bound_index_left == -1)
	{ // moving leftmost handle

#ifdef DEBUG_COLORMAPMODULES
	    cout << "  MOVE THE FIRST HANDLE" << endl;
	    cout << "  PICKED INDEX " << active_index << endl;
	    cout << "  RIGHT BOUND " << bound_index_right << endl;
	    cout << "  Colormap min value " << colormap->getMinValue() << endl;
	    cout << "  Colormap max value " << colormap->getMaxValue() << endl;
#endif

	    Colormap1d::Stop* rightBound = 
		colormap->stops.at(bound_index_right);
	    
	    //scale linearly
	    int numberOfHandlesInInterval = 
		bound_index_right - active_index - 1;

#ifdef DEBUG_COLORMAPMODULES
	    cout << "  numberOfHandles " << numberOfHandlesInInterval << endl;
#endif	   

	    double coefficients[numberOfHandlesInInterval];
	    
	    for(int i = picked_object.stopIndex + 1 ; i < bound_index_right ; i++){
		Colormap1d::Stop* currentStop = 
		    colormap->stops.at(i);

		coefficients[i - (picked_object.stopIndex + 1)] = (currentStop->value - pickedStop->value)/
		    (rightBound->value - pickedStop->value);
	    }
		
	    pickedStop->value += delta_value;

	    if(pickedStop->value >= rightBound->value){
		pickedStop->value = (rightBound->value - 0.0000001);
	    }
		
	    for(int i = picked_object.stopIndex + 1 ; i < bound_index_right ; i++){
		Colormap1d::Stop* currentStop = 
		    colormap->stops.at(i);
		    
		currentStop->value = pickedStop->value + 
		    coefficients[i - (picked_object.stopIndex + 1)] * (rightBound->value - pickedStop->value);
	    }
#ifdef DEBUG_COLORMAPMODULES
	    cout << "  New Value  " << pickedStop->value << endl;
	    cout << "  Right Bound Value " << rightBound->value << endl;
#endif
	}
	else if (bound_index_right == num_stops)
	{ // moving right handle
#ifdef DEBUG_COLORMAPMODULES
	    cout << "  MOVE THE LAST HANDLE" << endl;	
	    cout << "  BOUND INDEX LEFT " << bound_index_left << endl;
#endif    

	    Colormap1d::Stop* leftBound = 
		colormap->stops.at(bound_index_left);
	    
	    //scale linearly
	    int numberOfHandlesInInterval = 
		picked_object.stopIndex - bound_index_left - 1;
	   
	    double coefficients[numberOfHandlesInInterval];
	    
	    for(int i = bound_index_left + 1 ; i < picked_object.stopIndex ; i++){
		Colormap1d::Stop* currentStop = colormap->stops.at(i);
		
		coefficients[i - (bound_index_left + 1)] = 
		    (currentStop->value - leftBound->value) /
		    (pickedStop->value - leftBound->value);
	    }
		
	    pickedStop->value += delta_value;

	    if(pickedStop->value <= leftBound->value){
		pickedStop->value = (leftBound->value + 0.0000001);
	    }

	    for(int i = bound_index_left + 1 ; i < picked_object.stopIndex ; i++){
		Colormap1d::Stop* currentStop = 
		    colormap->stops.at(i);
		    
		currentStop->value = leftBound->value + 
		    coefficients[i - (bound_index_left + 1)] * 
		    (pickedStop->value - leftBound->value);
	    }

	}
	else {
#ifdef DEBUG_COLORMAPMODULES
	    cout << "  MOVE INTERMIDIATE HANDLE" << endl;
#endif

	    Colormap1d::Stop* pickedStop = colormap->stops.at(picked_object.stopIndex);	    	 

	    Colormap1d::Stop* minStop = colormap->stops.at(bound_index_left);
	    Colormap1d::Stop* maxStop = colormap->stops.at(bound_index_right);
	    
	    int numberOfHandlesInInterval = 
		(bound_index_right - bound_index_left) - 1;
	    
	    double coefficients[numberOfHandlesInInterval];
	    
	    for(int i = bound_index_left + 1 ; i < picked_object.stopIndex ; i++){
		Colormap1d::Stop* currentStop = 
		    colormap->stops.at(i);
		
		if(pickedStop->value == minStop->value){		    
#ifdef DEBUG_COLORMAPCONTROLLER
		    cout << "Lascou !!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
#endif
		    exit(0);
		}
		else
		    coefficients[i - (bound_index_left + 1)] = (currentStop->value - minStop->value)/
			(pickedStop->value - minStop->value);
	    }
		
	    for(int i = picked_object.stopIndex + 1 ; i < bound_index_right  ; i++){
		Colormap1d::Stop* currentStop = 
		    colormap->stops.at(i);

		if(maxStop->value == pickedStop->value){		    
#ifdef DEBUG_COLORMAPCONTROLLER
		    cout << "Lascou !!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
#endif
		    exit(0);
		}
		else
		    coefficients[i- (bound_index_left + 1)] = (currentStop->value - pickedStop->value)/
			(maxStop->value - pickedStop->value);
	    }

	    pickedStop->value += delta_value;

	    if(pickedStop->value <= minStop->value){
		pickedStop->value = (minStop->value + 0.0000001);
	    }
	    if(pickedStop->value >= maxStop->value){
		pickedStop->value = (maxStop->value - 0.0000001);
	    }

	    for(int i = bound_index_left + 1 ; i < picked_object.stopIndex ; i++){
		Colormap1d::Stop* currentStop = 
		    colormap->stops.at(i);
		
		currentStop->value = minStop->value + 
		    coefficients[i - (bound_index_left + 1)] * 
		    (pickedStop->value - minStop->value);	    
	    }
		
	    for(int i = picked_object.stopIndex + 1 ; i < bound_index_right  ; i++){
		Colormap1d::Stop* currentStop = 
		    colormap->stops.at(i);
	    
		currentStop->value = pickedStop->value + 
		    coefficients[i- (bound_index_left + 1)] * (maxStop->value - pickedStop->value);	    
	    }

	}
    }
    else if (picked_object.t == ColormapRenderer::Handle::COLORBAR)
    {

#ifdef DEBUG_COLORMAPCONTROLLER
	cout << "    picked_object == ColormapRenderer::Handle::COLORBAR " << ww << endl;
#endif

	int numberOfStops = colormap->stops.size();

	for(int i = 0 ; i < numberOfStops ; i++){
	    Colormap1d::Stop* stop = colormap->stops.at(i);
	    stop->value += delta_value;
	}

	double colormap_min_value = colormap->getMinValue();
	double colormap_max_value = colormap->getMaxValue();

	if (min_value > colormap_min_value){
	    double variation = min_value - colormap_min_value;
	    min_value = colormap_min_value;
	    max_value -= variation;
	}
	if (max_value < colormap_max_value){
	    double variation = colormap_max_value - max_value;
	    min_value += variation;

	    max_value = colormap_max_value;
	}
    }
    else if (picked_object.t == ColormapRenderer::Handle::RANGE)
    {

#ifdef DEBUG_COLORMAPCONTROLLER
	cout << "    ColormapRenderer::Handle::RANGE " << ww << endl;
#endif

	min_value -= delta_value;
	max_value -= delta_value;
    }

    getOutputPort("colormap")->setData
	(new data::SingleColormap1dPtr(colormap),true);

    getOutputPort("range")->setData
	(new data::Vector<double>(min_value,max_value),true);
    }
}

void 
ColormapController::another_colormap(int offset)
{
    Colormap1d *old_colormap = colormap;
    int new_index            = colormap_index;
    if (offset < 0)
    {
	new_index--;
	if (new_index < 0)
	    new_index = new_index + ColormapController::NUM_COLORMAPS;
	colormap_index = new_index;
	colormap = this->used_colormaps[colormap_index];//ColormapController::COLORMAPS[colormap_index]->copy();
    }
    else {
	new_index++;
	if (new_index == ColormapController::NUM_COLORMAPS)
	    new_index = 0;
	colormap_index = new_index;
	colormap = this->used_colormaps[colormap_index];
    }

    //colormap->setRange(old_colormap->getMinValue(), old_colormap->getMaxValue());
    
    int numStops = colormap->stops.size();

    double colormapMinValue = colormap->getMinValue();
    double colormapMaxValue = colormap->getMaxValue();

    double colormapRange = colormapMaxValue - colormapMinValue;

    double coefficients[numStops];

    for(int i = 0 ; i < numStops ; i++){
	Colormap1d::Stop* stop = colormap->stops.at(i);
	coefficients[i] = (stop->getValue() - colormapMinValue)/colormapRange;
    }

    double colormapNewMinValue = old_colormap->getMinValue();
    double colormapNewMaxValue = old_colormap->getMaxValue();
    double colormapNewRange    = colormapNewMaxValue - colormapNewMinValue;

    for(int i = 0 ; i < numStops ; i++){
	Colormap1d::Stop* stop = colormap->stops.at(i);
	stop->value = colormapNewMinValue + coefficients[i] *(colormapNewRange);
    }

    getOutputPort("colormap")->setData
	(new data::SingleColormap1dPtr(colormap),true);

}

Colormap1d* ColormapController::COLORMAPS[12] = 
{ 
    Colormap1d::seqWhiteRed9(),
    Colormap1d::seqWhiteRedCont(),
    Colormap1d::divergentBlueWhiteRed9(),
    Colormap1d::divergentBlueRed257Colors(),
    Colormap1d::hot(),
    Colormap1d::divergentBlueRed(),
    Colormap1d::divergentBlueRed3Colors(),
    Colormap1d::divergentBlueRed5Colors(),
    Colormap1d::yellow2blue(),
    Colormap1d::greyScale(),
    Colormap1d::heatedObject(),
    Colormap1d::linearGreyScale(),
};
    
const int ColormapController::NUM_COLORMAPS = 12;
