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

#ifndef PLOT_CONTROLLER_H
#define PLOT_CONTROLLER_H

#include "EventSystem.hpp"
#include "Dataflow.hpp"
#include "ViewRect.hpp"
#include "RectangleLayout.hpp"
#include "BasicModules.hpp"

class PlotRenderer;


/******************
 * PlotController *
 ******************/

class PlotController: 
    public eventsystem::Node, 
    public eventsystem::PickableObject,
    public rectanglelayout::RectangleSlot
{

private:
    
    ViewRect       screen_window;

    PlotRenderer*  plot_renderer;

public:
	
    PlotController();

    void setPlotRenderer(PlotRenderer* renderer);

public: // mouse manager stuff

    bool event_mouse_contains(int x, int y);

    string event_node_name(eventsystem::State *event_state);

    void event_mouse_mouse(eventsystem::State *event_state);

    eventsystem::PickableObject *event_mouse_pick(eventsystem::State *event_state);

    bool event_mouse_wheel(eventsystem::State *event_state);

    void event_mouse_drag(eventsystem::State *event_state); 

public: // rectangle slot

    void rectangleSignal(rectanglelayout::Rectangle r);


}; // end class PlotController


/****************
 * PlotRenderer *
 ****************/

class PlotRenderer: 
    public dataflow::Module
{

private:

    ViewRect world_window;

public:

    const static float margin_left   = 5;
    const static float margin_right  = 60;
    const static float margin_bottom = 25;
    const static float margin_top    = 24;
    const static float handle_radius = 6;
    const static float text_fix      = 6;

public:

    PlotRenderer();

    ViewRect getWorldWindow();

    void     setWorldWindow(ViewRect ww);

    void     process();
	
};

#endif
