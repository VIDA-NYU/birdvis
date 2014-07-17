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

#include "BasicModules.hpp"

#include "RenderSystem.hpp"
#include "DataflowTypes.hpp"
#include "Global.hpp"

using namespace dataflow;
using namespace rendersystem;

#define xDEBUG_BUTTON
#define xDEBUG_CURVEDATA
#define xDEBUG_CURVEDATA_2
#define xDEBUG_OFFSETMODULE
#define xDEBUG_LONLATGRIDVALUE

/*******************
 * OffsetModule *
 *******************/

OffsetModule::OffsetModule():
    Module("OffsetModule")
{
    addInputPort("input_int");
    addOutputPort("output_int");
}

int  
OffsetModule::getOffset()
{
    return int_offset;
}

int  
OffsetModule::getOrder()
{
    return int_order;
}

void 
OffsetModule::setOffset(int offset)
{
    int_offset = offset;
    outdated = true;
    
#ifdef DEBUG_OFFSETMODULE
    cout << "OffsetModule::setOffset()" << endl;
    cout << "   New Offset is " << offset << endl;
#endif

}

void 
OffsetModule::setOrder(int order)
{

#ifdef DEBUG_OFFSETMODULE
    cout << "OffsetModule::setOrder(...)" << endl;
    cout << "   current order " << int_order << endl;
    cout << "   current offset " << int_offset << endl;
    cout << "   new order " << order << endl;
    cout << "   new offset " << int_offset << endl;
#endif
    

    int_order = order;
    outdated = true;
}

void 
OffsetModule::process()
{
#ifdef DEBUG_OFFSETMODULE
    cout << "OffsetModule::process()" << endl;
#endif

    int input_int;
    if (!data::SingleInt::getInputValue(this,"input_int",input_int))
    {
	cout << "   could not retrieve input_int" << endl;
	return;
    }
#ifdef DEBUG_OFFSETMODULE
    cout << "   input_int   = " << input_int << endl;
    cout << "   output_port = " << getOutputPort("output_int") << endl;
    cout << "   offset      = " << int_offset << endl;
#endif
    getOutputPort("output_int")->setData
	(new data::SingleInt( (input_int + int_offset) % int_order  ),true);
}

/*************
 * IntModule *
 *************/

IntModule::IntModule():
    Module("IntModule"),
    int_value(0)
{
    //cout << "IntModule::IntModule()" << endl;
    this->addOutputPort("int_value");
    this->setInt(int_value);
}

void 
IntModule::process()
{}

int  
IntModule::getInt()
{
    return int_value;
}

void 
IntModule::setInt(int i)
{
    int_value = i;
    getOutputPort("int_value")->setData
	(new data::Single<int>(int_value),true);
}

/*******************
 * VectorIntModule *
 *******************/

VectorIntModule::VectorIntModule():
    Module("VectorIntModule")
{
    this->addOutputPort("values");
}

void VectorIntModule::init(vector<int> values){
//     cout << "VectorIntInit()" << endl;
//     cout << "Size of new Vector " << values.size() << endl;

//     cout << "Clearing Vecotr" << endl;
    this->values.clear();

//    cout << "Assign Values" << endl;
    this->values = values;

//    cout << "Set output port" << endl;
    getOutputPort("values")->setData
	(new data::Single< vector < int > >(values),true);
}

int VectorIntModule::getDimension(){
    return this->values.size();
}

int  VectorIntModule::getValueAt(int index){
    return this->values.at(index);
}

void VectorIntModule::setValueAt(int index, int value){
    this->values[index] = value;
    getOutputPort("values")->setData
	(new data::Single< vector < int > >(values),true);
}

void VectorIntModule::process(){
    
}


/****************
 * LonLatModule *
 ****************/

LonLatModule::LonLatModule():
    Module("LonLatModule"),
    longitude(0),
    latitude(0)
{
    this->addOutputPort("lonlat");
}
    
void  
LonLatModule::getLonLat(double &longitude, double &latitude)
{
    longitude = this->longitude;
    latitude  = this->latitude;
}

void 
LonLatModule::setLonLat(double longitude, double latitude)
{
    this->longitude = longitude;
    this->latitude  = latitude;
    getOutputPort("lonlat")->setData
	(new data::Single<LonLatModule*>(this),true);
}

void 
LonLatModule::process()
{
    getOutputPort("lonlat")->setData
	(new data::Single<LonLatModule*>(this),true);
}

/**********
 * Button *
 **********/

Button::Button(ViewRect quad, string text, long alignment):
    quad(quad),
    text(text),
    font_size(16),
    alignment(alignment),
    toggle(false),
    state(0)
{
    screen_window.init(0,0,1,1);
    bg_color.init(.1,.1,.1,.8);
    text_color_0.init(1,1,1);
    text_color_1.init(1,0,0);
}

ViewRect 
Button::getScreenQuad()
{
    float x,y,w,h;
    if ((LEFT & alignment) != 0)
    {
	x = screen_window.x + quad.x;
    }
    else if ((CENTER & alignment) != 0)
    {
	x = screen_window.x + screen_window.w/2.0 + quad.x;
    }
    else if ((RIGHT & alignment) != 0)
    {
	x = screen_window.x + screen_window.w + quad.x;
    }

    if ((BOTTOM & alignment) != 0)
    {
	y = screen_window.y + quad.y;
    }
    else if ((MIDDLE & alignment) != 0)
    {
	y = screen_window.y + screen_window.h/2.0 + quad.y;
    }
    else if ((TOP & alignment) != 0)
    {
	y = screen_window.y + screen_window.h + quad.y;
    }

    ViewRect r(x,y,quad.w, quad.h);

    return r;
}

void Button::setScreenQuad(ViewRect n_screen_quad){
    screen_window = n_screen_quad;
}
    
void 
Button::rectangleSignal(rectanglelayout::Rectangle r)
{
    screen_window.init(r.x,r.y,r.w,r.h);
}

bool 
Button::event_mouse_contains(int x, int y)
{
    return this->getScreenQuad().contains(x,y);
}

eventsystem::PickableObject* 
Button::event_mouse_pick(eventsystem::State* event_state)
{
    return this;
}

bool    
Button::event_mouse_drop(eventsystem::State *event_state)
{
    if (event_state->picked_object == this)
    {
	state = (state + 1) % 2;
	this->action();
	return true;
    }
    return false;
}

void    
Button::render()
{
#ifdef DEBUG_BUTTON
    cout << "Button::render()" << endl;
#endif    
    ViewRect r = getScreenQuad();

    float cx = r.x + r.w/2.0;
    float cy = r.y + r.h/2.0;

    //
    RenderSystem *renderSystem = Global::instance()->getRenderSystem();
    renderSystem->startViewport(screen_window);

    renderSystem->setColor(bg_color);
    renderSystem->drawQuad(r.x,r.y,r.w,r.h);
 
#ifdef DEBUG_BUTTON
    cout << "   state: " << state << endl;
    cout << "   toggle: " << toggle << endl;
    cout << "   toggle: " << text_color_1.str() << endl;
#endif    

    renderSystem->setColor(text_color_0);
    if (state == 1 && toggle)
	renderSystem->setColor(text_color_1);
    renderSystem->drawCenteredText(text.c_str(), 
				   font_size, 
				   r.x, r.y,
				   r.w, r.h);

    renderSystem->endViewport();
}

void    
Button::action()
{
    action_signal(state, this);
}

Button::action_signal_connection_t 
Button::connect(Button::action_signal_t::slot_function_type subscriber)
{
    return action_signal.connect(subscriber);
}

void 
Button::disconnect(Button::action_signal_connection_t connection)
{
    connection.disconnect();
}

/*******************
 * LonLatGridValue *
 *******************/

LonLatGridValue::LonLatGridValue():
    Module("LonLatGridValue")
{
    this->addInputPort("lonlat");
    this->addInputPort("grid");
    this->addOutputPort("value");
}

void 
LonLatGridValue::process()
{
#ifdef DEBUG_LONLATGRIDVALUE
    cout << "LonLatGridValue::process()" << endl;
#endif

    typedef data::Single<LonLatModule*> SinglaLonLatModulePtr;

    datatypes::Grid* grid;
    if (!data::SingleGridPtr::getInputValue(this,"grid",&grid))
    {
#ifdef DEBUG_LONLATGRIDVALUE
	cout << "   could not load grid" << endl;
#endif
	return;
    }

    LonLatModule* lonlatModule;
    if (!SinglaLonLatModulePtr::getInputValue(this,"lonlat",lonlatModule))
    {
#ifdef DEBUG_LONLATGRIDVALUE
	cout << "   could not load LonLatModule" << endl;
#endif
	return;
    }

    double lon, lat;
    lonlatModule->getLonLat(lon, lat);

    double value;
    if (!grid->getValueAtCoordinates(lon, lat, value))
	value = 0.0;

    getOutputPort("value")->setData
	(new data::Single<double>(value),true);
}

/****************
 * StringModule *
 ****************/

StringModule::StringModule():
    Module("StringModule"),
    string_value("")
{

#ifdef DEBUG_STRINGMODULE
    cout << "StringModule::StringModule" << endl;
#endif

    this->addOutputPort("string_value");
}

void 
StringModule::process()
{}

string  
StringModule::getString()
{
    return string_value;
}

void 
StringModule::setString(string i)
{
    string_value = i;
    getOutputPort("string_value")->setData
	(new data::Single<string>(string_value),true);
}


/**********************
 * ScreenWindowModule *
 **********************/

ScreenWindowModule::ScreenWindowModule():
    Module("ScreenWindowModule")
{
    this->addOutputPort("screen_window");
}

ViewRect
ScreenWindowModule::getScreenWindow()
{
    return screen_window;
}

void
ScreenWindowModule::process()
{
}

void
ScreenWindowModule::rectangleSignal(rectanglelayout::Rectangle r)
{
    screen_window.init(r.x,r.y,r.w,r.h);
    getOutputPort("screen_window")->setData
	(new data::SingleViewRect(screen_window),true);
}

/*********************
 * WorldWindowModule *
 *********************/

WorldWindowModule::WorldWindowModule():
    Module("WorldWindowModule")
{
    this->addOutputPort("world_window");
    getOutputPort("world_window")->setData
	(new data::SingleViewRect(world_window),true);
}

void
WorldWindowModule::process()
{}

ViewRect 
WorldWindowModule::getWorldWindow()
{
    return world_window;
}

void 
WorldWindowModule::setWorldWindow(ViewRect ww)
{
    world_window = ww;
    getOutputPort("world_window")->setData
	(new data::SingleViewRect(world_window),true);
}


/*************
 * Selection *
 *************/

Selection::Selection():
    Module("Selection"),
    user_integer(0)
{
    this->addOutputPort("selection_window");
    this->addOutputPort("selection");
    this->addOutputPort("user_integer");
    getOutputPort("selection_window")->setData
	(new data::SingleViewRect(selection_window),true);
    getOutputPort("selection")->setData
	(new data::Single<Selection *>(this),true);

    color.init(
	Selection::COLORS[Selection::CURRENT_COLOR][0],
	Selection::COLORS[Selection::CURRENT_COLOR][1],
	Selection::COLORS[Selection::CURRENT_COLOR][2]);

    Selection::CURRENT_COLOR = (Selection::CURRENT_COLOR+1) % Selection::NUM_COLORS;
}

void
Selection::process()
{}

ViewRect 
Selection::getSelectionWindow()
{
    return selection_window;
}

void 
Selection::setSelectionWindow(ViewRect ww)
{
    selection_window = ww;
    getOutputPort("selection_window")->setData
	(new data::SingleViewRect(selection_window),true);
    getOutputPort("selection")->setData
	(new data::Single<Selection *>(this),true);
}

void     
Selection::setUserInteger(int i)
{
    user_integer = i;
    // send a signal to 
    getOutputPort("user_integer")->setData
	(new data::SingleInt(user_integer),true);
}

int
Selection::getUserInteger()
{
    return user_integer;
}

bool
Selection::contains(float x, float y)
{
    return selection_window.contains(x,y);
}    


Color
Selection::getColor()
{
    return color;
}

// const float Selection::COLORS[10][3] = { 
//     {31 /255.0, 119/255.0, 180/255.0},
//     {255/255.0, 127/255.0, 14 /255.0},
//     {44 /255.0, 160/255.0, 44 /255.0},
//     {214/255.0, 39 /255.0, 40 /255.0},
//     {148/255.0, 103/255.0, 189/255.0},
//     {140/255.0, 86 /255.0, 75 /255.0},
//     {227/255.0, 119/255.0, 194/255.0},
//     {127/255.0, 127/255.0, 127/255.0},
//     {188/255.0, 189/255.0, 34 /255.0},
//     {23 /255.0, 190/255.0, 207/255.0}};
    

const float Selection::COLORS[10][3] = { 
    {31 /255.0, 119/255.0, 180/255.0},
    {44 /255.0, 160/255.0, 44 /255.0},
    {148/255.0, 103/255.0, 189/255.0},
    {23 /255.0, 190/255.0, 207/255.0},
    {214/255.0, 39 /255.0, 40 /255.0},
    {227/255.0, 119/255.0, 194/255.0},
    {140/255.0, 86 /255.0, 75 /255.0},
    {188/255.0, 189/255.0, 34 /255.0},
    {127/255.0, 127/255.0, 127/255.0},
    {255/255.0, 127/255.0, 14 /255.0}
    };

const int Selection::NUM_COLORS = 10;

int Selection::CURRENT_COLOR = 0;

/***********************
 * SelectionRepository *
 ***********************/

SelectionRepository::SelectionRepository()
{}

Selection*
SelectionRepository::getSelectionOn(float x, float y)
{
    for (int i=selections.size()-1;i>=0;i--)
    {
	Selection *sel = selections.at(i);
	if (sel->contains(x,y))
	    return sel;
    }
    return NULL;
}

Selection*
SelectionRepository::addSelection(ViewRect ww)
{
    Selection *sel = new Selection();
    sel->setSelectionWindow(ww);
    selections.push_back(sel);
    this->emitAddSelectionSignal(sel);
    return sel;
}

SelectionRepository::signal_connection_t 
SelectionRepository::connectToAddSelectionSignal(
    add_selection_signal_t::slot_function_type subscriber)
{
   SelectionRepository::signal_connection_t result = 
       add_selection_signal.connect(subscriber);
   return result;
}

SelectionRepository::signal_connection_t 
SelectionRepository::connectToDelSelectionSignal(
    del_selection_signal_t::slot_function_type subscriber)
{
   SelectionRepository::signal_connection_t result = 
       del_selection_signal.connect(subscriber);
   return result;
}



void  
SelectionRepository::disconnect(signal_connection_t subscriber)
{
    subscriber.disconnect();
}

Selection*
SelectionRepository::deleteSelection(Selection *sel)
{
    vector<Selection*>::iterator it;
    this->emitDelSelectionSignal(sel);
    it = find(selections.begin(),selections.end(),sel);
    if (it != selections.end())
	selections.erase(it);
}

void 
SelectionRepository::emitAddSelectionSignal(Selection *sel)
{
    add_selection_signal(sel);
}

void 
SelectionRepository::emitDelSelectionSignal(Selection *sel)
{
    del_selection_signal(sel);
}

int SelectionRepository::getNumberOfSelections(){
    return this->selections.size();
}

Selection* SelectionRepository::getSelection(int index){
    if(index < 0 || index >= this->selections.size()){
	return NULL;
    }

    return this->selections.at(index);
}


/************************
 * SelectionController *
 ************************/

SelectionController::SelectionController(SelectionRepository* sel_repo, 
					 WorldWindowModule*   mod_world_window)
{
    this->selection_repository = sel_repo;
    this->mod_world_window = mod_world_window;
}

void 
SelectionController::rectangleSignal(rectanglelayout::Rectangle r)
{
    screen_window.init(r.x,r.y,r.w,r.h);
}

bool 
SelectionController::event_mouse_contains(int x, int y)
{
#ifdef DEBUG_SELECTIONCONTROLLER
    cout << "SelectionController::event_mouse_contains(...)" << endl;
    cout << "    answer: " << screen_window.contains(x, y) << endl;
#endif
    return screen_window.contains(x, y);
}

void 
SelectionController::event_mouse_drag(eventsystem::State *event_state)
{
    ViewRect world_window = mod_world_window->getWorldWindow();
    ScreenWorldTransform swt(screen_window, world_window);

    if (action == SelectionController::NEW)
    {
	float wx0, wy0, wx1, wy1;
	swt.screen2world(event_state->pick_x, event_state->pick_y, wx0, wy0);
	swt.screen2world(event_state->x, event_state->y, wx1, wy1);

	float xx = min(wx0,wx1);
	float yy = min(wy0,wy1);
	float ww = fabs(wx0-wx1);
	float hh = fabs(wy0-wy1);

	ViewRect selection_window(xx,yy,ww,hh);

	hit_selection->setSelectionWindow(selection_window);
    }

    else if (action == SelectionController::MOVE)
    {
	float wx0, wy0, wx1, wy1;
	swt.screen2world(event_state->last_x, event_state->last_y, wx0, wy0);
	swt.screen2world(event_state->x,      event_state->y,      wx1, wy1);

	ViewRect sw = hit_selection->getSelectionWindow();
	sw.translate(wx1-wx0, wy1-wy0);
	hit_selection->setSelectionWindow(sw);
    }

    else if (action == SelectionController::RESIZE)
    {
	float wx0, wy0, wx1, wy1;
	swt.screen2world(event_state->last_x, event_state->last_y, wx0, wy0);
	swt.screen2world(event_state->x,      event_state->y,      wx1, wy1);
	ViewRect sw = hit_selection->getSelectionWindow();
	sw.resizeRelativeToCorner(resize_corner,wx1-wx0, wy1-wy0);
	hit_selection->setSelectionWindow(sw);
    }


    // else if (action == MOVE)
    // {
    // 	float wx0, wy0, wx1, wy1;
    // 	swt.screen2world(event_state->last_x, pick_state->last_y, wx0, wy0);
    // 	swt.screen2world(event_state->x,      event_state->y,     wx1, wy1);

    // 	ViewRect ww = hit_selection->getScreenWindow();
    // 	ww.translate(wx1-wx0, wy1-wy0);
    // 	hit_selection->setWorldWindow(ww);
    // }
}
    
eventsystem::PickableObject*
SelectionController::event_mouse_pick(eventsystem::State *event_state)
{
#ifdef DEBUG_SELECTIONCONTROLLER
	cout << "SelectionController::event_mouse_pick(...)" << endl;
#endif

    ViewRect world_window = mod_world_window->getWorldWindow();
    ScreenWorldTransform swt(screen_window, world_window);

    float x, y;
    swt.screen2world(event_state->x, event_state->y, x, y);

    if (event_state->button == eventsystem::RIGHT && !event_state->shift)
    {

	hit_selection = selection_repository->getSelectionOn(x,y);

	if (hit_selection != NULL)
	{
	    ViewRect selection_window = hit_selection->getSelectionWindow();
	    double d;
	    resize_corner = selection_window.closestCorner(x, y, d);
	    // swt.world2screen(event_state->x, event_state->y, x, y);
	    action = SelectionController::RESIZE;
	    return this;
	}

	ViewRect sw(x,y,0,0);
	hit_selection = selection_repository->addSelection(sw);
	action        = SelectionController::NEW;
	return this;
    }
    else if (event_state->button == eventsystem::LEFT)
    {
	hit_selection = selection_repository->getSelectionOn(x,y);

	if (hit_selection == NULL)
	{
	    action = SelectionController::IDLE;
	    return NULL;
	}

#ifdef DEBUG_SELECTIONCONTROLLER
	cout << "   distance to corner: " << d << endl;
#endif

	action = SelectionController::MOVE;
	return this;
    }
    return NULL;
}

void 
SelectionController::event_mouse_release(eventsystem::State *event_state)
{
    hit_selection = NULL;
    action = IDLE;
}

bool
SelectionController::event_key_press(eventsystem::State *event_state) 
{
    ViewRect world_window = mod_world_window->getWorldWindow();
    ScreenWorldTransform swt(screen_window, world_window);

    float x, y;
    swt.screen2world(event_state->x, event_state->y, x, y);


    Selection *hit_sel = selection_repository->getSelectionOn(x,y);

    if (hit_sel == NULL)
	return false;

    switch(event_state->key)
    {
    case (eventsystem::Key_Delete):	       		
	selection_repository->deleteSelection(hit_sel);
	return true;
    default:
	break;
    }
    return false;
}

/************************
 * ZoomAndPanController *
 ************************/

ZoomAndPanController::ZoomAndPanController()
{}

void
ZoomAndPanController::setWorldWindowModule(WorldWindowModule *wwmod)
{
    mod_world_window = wwmod;
}

void
ZoomAndPanController::setLonLatModule(LonLatModule *llmod)
{
    mod_longitude_latitude = llmod;
}

void 
ZoomAndPanController::rectangleSignal(rectanglelayout::Rectangle r)
{
    screen_window.init(r.x,r.y,r.w,r.h);
}

bool 
ZoomAndPanController::event_mouse_contains(int x, int y)
{
    return screen_window.contains(x, y);
}

void 
ZoomAndPanController::event_mouse_drag(eventsystem::State *event_state)
{

    ViewRect world_window = mod_world_window->getWorldWindow();

    ScreenWorldTransform swt(screen_window, world_window);

    float wx0, wy0, wx1, wy1;
    swt.screen2world(event_state->last_x, event_state->last_y, wx0, wy0);
    swt.screen2world(event_state->x, event_state->y, wx1, wy1);

    world_window.translate(wx0-wx1,wy0-wy1);
    mod_world_window->setWorldWindow(world_window);

}
    
bool 
ZoomAndPanController::event_mouse_wheel(eventsystem::State *event_state)
{

#ifdef DEBUG_ZOOMANDPANCONTROLLER
    cout << "ZoomAndPanController::event_mouse_wheel(...)" << endl;
#endif

    double coef =  (event_state->wheel_dx > 0 ? 1.0/1.1 : 1.1);

    ViewRect world_window = mod_world_window->getWorldWindow();

    world_window.scale(coef);

    mod_world_window->setWorldWindow(world_window);

    return true;
}

eventsystem::PickableObject*
ZoomAndPanController::event_mouse_pick(eventsystem::State *event_state)
{
    if (event_state->button == eventsystem::LEFT)
    {
	return this;
    }
    else 
    {
	return NULL;
    }
}

void
ZoomAndPanController::event_mouse_move(eventsystem::State *event_state)
{
 
    ViewRect world_window = mod_world_window->getWorldWindow();

    ScreenWorldTransform swt(screen_window, world_window);

    float lon, lat;
    swt.screen2world(event_state->last_x, event_state->last_y, lon, lat);

    if (mod_longitude_latitude != NULL)
    {
	mod_longitude_latitude->setLonLat(lon, lat);
    }
}


/************************
 * ScreenWorldTransform *
 ************************/

ScreenWorldTransform::ScreenWorldTransform(
    ViewRect screen_window, ViewRect world_window):
    screen_window(screen_window),
    world_window(world_window)
{

    // keep aspect ratio making the screen window
    // cover a larger area

    float ww_ar = world_window.w / world_window.h;
    float sw_ar = screen_window.w / screen_window.h;

    adjusted_screen_window = screen_window;

    if (sw_ar < ww_ar)
    {
	adjusted_screen_window.w = ww_ar * screen_window.h;
	adjusted_screen_window.x = 
	    screen_window.x - (adjusted_screen_window.w -
			       screen_window.w)/2.0;
    }
    else if (sw_ar > ww_ar)
    {
	adjusted_screen_window.h = screen_window.w / ww_ar;
	adjusted_screen_window.y = 
	    screen_window.y - (adjusted_screen_window.h -
			       screen_window.h)/2.0;
    }
}

void     
ScreenWorldTransform::screen2world(float sx, float sy, float &wx, float &wy)
{
    float xcoef = (sx - adjusted_screen_window.x)/adjusted_screen_window.w;
    float ycoef = (sy - adjusted_screen_window.y)/adjusted_screen_window.h;
    wx = world_window.x + xcoef * world_window.w;
    wy = world_window.y + ycoef * world_window.h;
}

void
ScreenWorldTransform::world2screen(float wx, float wy, float &sx, float &sy){
    float xcoef = (wx - world_window.x)/world_window.w;
    float ycoef = (wy - world_window.y)/world_window.h;

    sx = adjusted_screen_window.x + xcoef * adjusted_screen_window.w;
    sy = adjusted_screen_window.y + ycoef * adjusted_screen_window.h;
}

ViewRect
ScreenWorldTransform::world2screen(ViewRect world_window)
{
    float x0,y0;
    this->world2screen(world_window.x,world_window.y,x0,y0);
    float x1,y1;
    this->world2screen(world_window.x+world_window.w,
		       world_window.y+world_window.h,x1,y1);
    ViewRect result(x0,y0,x1-x0,y1-y0);
    return result;
}

ViewRect
ScreenWorldTransform::getAdjustedScreenWindow()
{
    return adjusted_screen_window;
}

/*************************
 * MapSelectionsRenderer *
 *************************/

MapSelectionsRenderer::MapSelectionsRenderer(): Module("MapSelectionsRenderer")
{
    this->addInputPort("screen_window");
    this->addInputPort("world_window");
    this->addInputPort("selections");
}
    
void 
MapSelectionsRenderer::process()
{
#ifdef DEBUG_MAPSELECTIONSRENDERER
    cout << "MapSelectionsRenderer::process()" << endl;
#endif
    // cout << "MapSelectionsRenderer::process()" << endl;
    typedef data::Single<Selection *> SingleSelectionPtr;


    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	return;

#ifdef DEBUG_MAPSELECTIONSRENDERER
    cout << "   screen window.....OK" << endl;
#endif

    ViewRect world_window;
    if (!data::SingleViewRect::getInputValue(this,"world_window",world_window))
	return;

#ifdef DEBUG_MAPSELECTIONSRENDERER
    cout << "   world window.....OK" << endl;
#endif

    vector<Selection *> selections;
    vector<Data *> vec_data = this->getInputPort("selections")->getAllData();
    for (int i=0;i<vec_data.size();i++)
    {
	SingleSelectionPtr* data = dynamic_cast<SingleSelectionPtr*>(vec_data.at(i));  
	if (data != NULL)
	    selections.push_back(data->value);
    }
    
#ifdef DEBUG_MAPSELECTIONSRENDERER
    cout << "   number of selections " << selections.size() << endl;
#endif

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


    for (int i=0;i<selections.size();i++)
    {
	Selection *sel = selections[i];
	ViewRect sw = sel->getSelectionWindow();

	Color c = sel->getColor();
	renderSystem->setColor(c.r,c.g,c.b, ColorConstants::SEL_TRANSPARENCY);
	renderSystem->drawQuad(sw.x,sw.y,sw.w,sw.h);

	renderSystem->setColor(sel->getColor());
	renderSystem->drawQuad(sw.x,sw.y,sw.w,sw.h,GL_LINE_LOOP);
    }

    renderSystem->endViewport();

}

/*********
 * Graph *
 *********/

Graph::Graph()
{}

Graph::Graph(Color c)
{
    this->color = c;
}

Graph::Graph(vector<float> curve, Color c)
{
    this->curve = curve;
    this->color = c;
}

/*************
 * CurveData *
 *************/

CurveData::CurveData(int x_samples, int y_samples):
    Module("CurveData")
{
    this->x_samples = x_samples;
    this->y_samples = y_samples;

    this->addInputPort("selection");
    this->addInputPort("file");
    this->addOutputPort("curve");
}

string
CurveData::getExpression(){
    return this->expression;
}

void 
CurveData::setExpression(string expression)
{

#ifdef DEBUG_CURVEDATA
    cout << "CurveData::setExpression(...)" << endl;
    cout << "   " << expression << endl;
#endif

    this->expression = expression;
    this->outdated = true;
}

void 
CurveData::setXSamples(int samples){
    this->x_samples = samples;
}

void 
CurveData::setYSamples(int samples){
    this->y_samples = samples;
}

void 
CurveData::process()
{

    typedef data::Single<Selection *> SingleSelectionPtr;
    typedef data::Single<Graph *>     SingleGraphPtr;


#ifdef DEBUG_CURVEDATA
    cout << "CurveData::process()" << endl;
    // cout << "   " << expression << endl;
#endif

    if (this->expression.compare("") == 0)
    {

#ifdef DEBUG_CURVEDATA
	cout << "   curve has no expression" << endl;
#endif

	this->getOutputPort("curve")->setData(NULL);
	return;
    }

#ifdef DEBUG_CURVEDATA
    cout << "   curve expression is " << this->expression << endl;
#endif

    bvdata::File* speciesFile;
    if (!data::Single<bvdata::File*>::getInputValue(this,"file",speciesFile))
    {
#ifdef DEBUG_TAGCLOUDDATA
	cout << "   could not retrieve species file" << endl;
#endif

	getOutputPort("curve")->setData(NULL,true);
	return;
    }

    bool integrate_all = false;

    Selection *selection = NULL;
    if (!SingleSelectionPtr::getInputValue(this,"selection",selection))
	integrate_all = true;


#ifdef DEBUG_CURVEDATA
    cout << "   integrate_all: " << integrate_all << endl;
#endif

    // cout << "   screen window.....OK" << endl;
    bvdata::Repository *repo = Global::instance()->getRepository();

    //find out the sum ranges   
    ViewRect world_window = speciesFile->getProbSDQuad();//repo->getDefaultLonLatQuad();
    int xMin, xMax, yMin, yMax;

    double denominator = 1.0;

    double cells_per_area_unit = (speciesFile->xcells * speciesFile->ycells) / 
	(speciesFile->quad.w * speciesFile->quad.h);

    if (integrate_all)
    {
	xMin = 0; yMin = 0;
	xMax = x_samples-1; yMax = y_samples-1;

#ifdef DEBUG_CURVEDATA
    cout << "   this->x_samples: " << x_samples << endl;
    cout << "   this->y_samples: " << y_samples << endl;
#endif

        if (ColorConstants::NORMALIZE_CURVES)
        {
	    denominator = cells_per_area_unit * (speciesFile->quad.w * speciesFile->quad.h) *
		Global::instance()->estimateUSAreaCoefficientAtRectangle(
		    speciesFile->quad.x,
		    speciesFile->quad.y,
		    speciesFile->quad.w,
		    speciesFile->quad.h);
        }

    }
    else {

	ViewRect selection_window = selection->getSelectionWindow();

	xMin = ((selection_window.x - world_window.x)/(world_window.w)) * x_samples;
	if(xMin  < 0) xMin = 0;
	if(xMin  >= x_samples) xMin = x_samples - 1;
    
	xMax = ((selection_window.x + 
		 selection_window.w - world_window.x)/(world_window.w)) * x_samples;
	if(xMax  < 0) xMax = 0;
	if(xMax  >= x_samples) xMax = x_samples - 1;

	yMin = ((selection_window.y - world_window.y)/(world_window.h)) * y_samples;
	if(yMin  < 0) yMin = 0;
	if(yMin  >= y_samples) yMin = y_samples - 1;
    
	yMax = ((selection_window.y + 
		 selection_window.h - world_window.y)/(world_window.h)) * y_samples;
	if(yMax  < 0) yMax = 0;
	if(yMax  >= y_samples) yMax = y_samples - 1;


	// TODO need to have this rectangle right
        if (ColorConstants::NORMALIZE_CURVES)
        {
	    denominator = cells_per_area_unit * (selection_window.w * selection_window.h) *
		Global::instance()->estimateUSAreaCoefficientAtRectangle(selection_window.x,
									 selection_window.y,
									 selection_window.w,
									 selection_window.h);
	}
    }

#ifdef DEBUG_CURVEDATA
	cout << "   xMin, xMax, yMin, yMax: " 
	     << xMin << ", " << xMax << ", " 
	     << yMin << ", " << yMax << endl;
	// cout << "   " << expression << endl;
#endif


    //R query
    vector<float> curve = speciesFile->getVariableCurve("p", xMin+1, xMax+1, yMin+1, yMax+1);

    vector<float>::iterator it;

    Color black(0,0,0);
    this->graph.curve.clear();
    this->graph.color = (selection != NULL ? selection->getColor() : black);

    float sum = 0;
    for (int i=0;i<curve.size();i++)
    {
	if (denominator > 0.0)
	    this->graph.curve.push_back(curve[i]/denominator);
	else
	    this->graph.curve.push_back(0.0);
	sum += curve[i];
    }

#ifdef DEBUG_CURVEDATA
    cout << "   Sum = " << sum << endl;
#endif

    
    this->getOutputPort("curve")->setData(new SingleGraphPtr(&this->graph),true);
}

