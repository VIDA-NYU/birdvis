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

#ifndef BASICMODULES_HPP
#define BASICMODULES_HPP

#include <vector>

#include "Color.hpp"

//#include "dataflow.hpp"
#include "Dataflow.hpp"

#include "RectangleLayout.hpp"

#include "ViewRect.hpp"

#include "EventSystem.hpp"

#define xDEBUG_CURVEDATA



/*********
 * Graph *
 *********/

class Graph {
public:
    vector<float> curve;
    Color color;

    Graph();
    Graph(Color c);
    Graph(vector<float> curve, Color c);
};


/*************
 * CurveData *
 *************/

class CurveData:
    public dataflow::Module
{
private:
    string expression;
    Graph graph;
    int x_samples;
    int y_samples;

public:
    CurveData(int x_samples = 250, int y_samples = 125);
    void setExpression(string expression);
    string getExpression();
    void process();
    void setXSamples(int samples);
    void setYSamples(int samples);
};



/****************
 * WindowModule *
 ****************/

class ScreenWindowModule:
    public dataflow::Module,
    public rectanglelayout::RectangleSlot
{
private:

    ViewRect screen_window;

public:

    ScreenWindowModule();

    ViewRect getScreenWindow();

    void process();

    void rectangleSignal(rectanglelayout::Rectangle r);

    void installAt();

};

/*************
 * IntModule *
 *************/

class IntModule:
    public dataflow::Module
{
private:

    int int_value;

public:

    IntModule();

    int  getInt();
    void setInt(int i);
    void process();

};

/*******************
 * VectorIntModule *
 *******************/

class VectorIntModule:
    public dataflow::Module
{
private:

    vector<int> values;

public:

    VectorIntModule();

    void init(vector<int> values);
    int getDimension();
    int  getValueAt(int index);
    void setValueAt(int index, int value);
    void process();

};

/****************
 * LonLatModule *
 ****************/

class LonLatModule:
    public dataflow::Module
{
private:
    double longitude;
    double latitude;
public:
    LonLatModule();
    void getLonLat(double &longitude, double &latitude);
    void setLonLat(double longitude, double latitude);
    void process();
};


/***************
 * LonLatValue *
 ***************/

class LonLatGridValue:
    public dataflow::Module
{
public:
    LonLatGridValue();
    void process();
};




/****************
 * OffsetModule *
 ****************/

class OffsetModule:
    public dataflow::Module
{
private:
    int int_offset;
    int int_order;
public:
    OffsetModule();
    int  getOffset();
    void setOffset(int offset);
    int  getOrder();
    void setOrder(int order);
    void process();
};

/****************
 * StringModule *
 ****************/

class StringModule:
    public dataflow::Module
{
private:

    string string_value;

public:

    StringModule();

    string  getString();
    void setString(string i);
    void process();

};

/*********************
 * WorldWindowModule *
 *********************/

class WorldWindowModule:
    public dataflow::Module
{

private:

    ViewRect world_window;

public:

    WorldWindowModule();

    void process();

    ViewRect getWorldWindow();

    void setWorldWindow(ViewRect ww);

};


/*************
 * Selection *
 *************/

class Selection:
    public dataflow::Module
{
private:

    static const float COLORS[10][3];
    static const int NUM_COLORS;
    static int CURRENT_COLOR;

private:

    ViewRect selection_window;

    Color    color;

    int      user_integer;

public:

    Selection();

    void     process();

    ViewRect getSelectionWindow();

    bool     contains(float x, float y);

    void     setSelectionWindow(ViewRect ww);

    void     setUserInteger(int i);

    int      getUserInteger();

    Color    getColor();

};


/***********************
 * SelectionRepository *
 ***********************/

class SelectionRepository
{
public:

    typedef boost::signals2::signal<void (Selection*)>  add_selection_signal_t;
    typedef boost::signals2::signal<void (Selection*)>  del_selection_signal_t;
    typedef boost::signals2::connection                 signal_connection_t;

private:

    vector<Selection* >     selections;
    
    add_selection_signal_t  add_selection_signal;

    del_selection_signal_t  del_selection_signal;

public:

    SelectionRepository();

    Selection *getSelectionOn(float x, float y);

    Selection *addSelection(ViewRect vr);

    Selection *deleteSelection(Selection *sel);

    signal_connection_t connectToAddSelectionSignal(
	add_selection_signal_t::slot_function_type subscriber);

    signal_connection_t connectToDelSelectionSignal(
	del_selection_signal_t::slot_function_type subscriber);

    void  disconnect(signal_connection_t subscriber);

    void  emitAddSelectionSignal(Selection *sel);

    void  emitDelSelectionSignal(Selection *sel);

    int getNumberOfSelections();

    Selection* getSelection(int index);

};

/***********************
 * Button *
 ***********************/

class Button:
    public eventsystem::Node,
    public eventsystem::PickableObject,
    public rectanglelayout::RectangleSlot
{
public:

    typedef boost::signals2::signal<void (int, Button*)>  action_signal_t;

    typedef boost::signals2::connection       action_signal_connection_t;

public:

    enum AlignmentVar {LEFT=1, CENTER=2, RIGHT=4, BOTTOM=8, MIDDLE=16, TOP=32};

private:

    ViewRect               screen_window;

    ViewRect               quad;

    string                 text;


    long                   alignment;                 

    action_signal_t        action_signal;


public:

    int                    state;

    bool                   toggle;

    Color                  bg_color;

    Color                  text_color_0;

    Color                  text_color_1;

    float                  font_size;

public:

    Button(ViewRect quad, string text, long alignment);

    void rectangleSignal(rectanglelayout::Rectangle r);

    void setScreenQuad(ViewRect n_screen_quad);

    ViewRect getScreenQuad();

    bool event_mouse_contains(int x, int y);

    eventsystem::PickableObject* event_mouse_pick(eventsystem::State* event_state);

    virtual bool event_mouse_drop(eventsystem::State *event_state);

    virtual void action();
    
    virtual void render();

public:

    action_signal_connection_t 
    connect(action_signal_t::slot_function_type subscriber);

    void 
    disconnect(action_signal_connection_t connection);


};

/***********************
 * SelectionController *
 ***********************/

class SelectionController:
    public eventsystem::Node,
    public eventsystem::PickableObject,
    public rectanglelayout::RectangleSlot
{
private:

    enum Action { IDLE, NEW, RESIZE, MOVE };

    SelectionRepository* selection_repository;

    WorldWindowModule*   mod_world_window;

    ViewRect             screen_window;

    Selection*           hit_selection;

    ViewRect::Corner     resize_corner;

    Action               action;

public:

    SelectionController(
	SelectionRepository* sel_repo,
	WorldWindowModule*   mod_world_window);

    void setSelectionRepository();

    void rectangleSignal(rectanglelayout::Rectangle r);

    bool event_mouse_contains(int x, int y);

    void event_mouse_drag(eventsystem::State* event_state);
    
    eventsystem::PickableObject* event_mouse_pick(eventsystem::State* event_state);

    void event_mouse_release(eventsystem::State *event_state);

    bool event_key_press(eventsystem::State *event_state);

};

/************************
 * ZoomAndPanController *
 ************************/

class ZoomAndPanController:
    public eventsystem::Node,
    public eventsystem::PickableObject,
    public rectanglelayout::RectangleSlot
{

private:

    WorldWindowModule* mod_world_window;

    LonLatModule*      mod_longitude_latitude;

    ViewRect           screen_window;

public:

    ZoomAndPanController();

    void setWorldWindowModule(WorldWindowModule *wwmod);

    void setLonLatModule(LonLatModule *llmod);

    void rectangleSignal(rectanglelayout::Rectangle r);

    bool event_mouse_contains(int x, int y);

    void event_mouse_drag(eventsystem::State* event_state);
    
    bool event_mouse_wheel(eventsystem::State* event_state);

    eventsystem::PickableObject* event_mouse_pick(eventsystem::State* event_state);

    void event_mouse_move(eventsystem::State *event_state);

};


/************************
 * ScreenWorldTransform *
 ************************/

class ScreenWorldTransform {
private:

    ViewRect screen_window;

    ViewRect world_window;

    ViewRect adjusted_screen_window;

public:

    ScreenWorldTransform(ViewRect screen_window,
			 ViewRect world_window);

    ViewRect getAdjustedScreenWindow();

    void     screen2world(float sx, float sy, float &wx, float &wy);
    void     world2screen(float wx, float wy, float &sx, float &sy);

    ViewRect world2screen(ViewRect world_window);

};

/**********************
 * MapBordersRenderer *
 **********************/

class MapBordersRenderer: 
    public dataflow::Module 
{
public:

    MapBordersRenderer();

    void process();

};


/*************************
 * MapSelectionsRenderer *
 *************************/

class MapSelectionsRenderer: 
    public dataflow::Module 
{
public:

    MapSelectionsRenderer();

    void process();

};


#endif
