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

#ifndef COLORMAP_MODULES_HPP
#define COLORMAP_MODULES_HPP


#include "Dataflow.hpp"

#include "Colormap1d.hpp"

#include "EventSystem.hpp"


class TypeAndIndex;
class ColormapRenderer;

#define xDEBUG_COLORMAPCONTROLLER
#define xDEBUG_COLORMAPRENDERER

/************************
 * ColormapRendererDesc *
 ************************/

class ColormapRendererDesc;
    

/********************
 * ColormapRenderer *
 ********************/

class ColormapRenderer: 
    public dataflow::Module {
public:
    class Handle {
    public:
	enum Type { NONE, LEFT_HANDLE, RIGHT_HANDLE, HANDLE, COLORBAR, RANGE };
    public:
	double x,y,w,h;
	Type type;
	Colormap1d::Stop *stop;
	Handle();
	Handle(Type type, double x, double y, double w, double h);
	bool contains(double xx, double yy);
	void setBox(double xx, double yy, double ww, double hh);
	bool isActive();
	void setActive(bool active);
    };

public:

    const static float margin_left   = 38;
    const static float margin_right  = 5;
    const static float margin_bottom = 20;
    const static float margin_top    = 12;
    const static float handle_radius = 6;
    const static float text_fix      = 0;

    ColormapRendererDesc *colormapDescription;

    ColormapRenderer();

    //Handle::Type pick(double x, double y);
    TypeAndIndex pick(double x, double y);
    void toogleActiveNearestHandle(double x, double y);

    Handle *getRangeBar();
	
    void process();

};

/************************
 * ColormapRendererDesc *
 ************************/

class ColormapRendererDesc{
public:
    vector<ColormapRenderer::Handle> handles;
    ColormapRenderer::Handle range;
    ColormapRenderer::Handle colorbar;

    Colormap1d *parentColormap;
    
    ColormapRendererDesc();

//     ColormapRendererDesc(Colormap1d *colormap, 
// 			 ColormapRenderer::Handle newRange, ColormapRenderer::Handle newColorbar,
// 			 double xx, double yy, double ww, double hh, double min_value, double max_value);
    
    void init(Colormap1d *colormap, 
	      ColormapRenderer::Handle newRange, ColormapRenderer::Handle newColorbar,
	      double xx, double yy, double ww, double hh, double min_value, double max_value);

    ColormapRenderer::Handle getRangeHandle();
    void setRangeHandle(ColormapRenderer::Handle newRange);

    ColormapRenderer::Handle getColorBarHandle();
    void setColorbarHandle(ColormapRenderer::Handle newColorbar);

    int                      getNumColormapHandles();
    ColormapRenderer::Handle getColormapHandle(int index);

};
    

/****************
 * TypeAndIndex *
 ****************/

class TypeAndIndex
{
public:
    ColormapRenderer::Handle::Type t;
    int                            stopIndex; // auxiliary integer
};

/**********************
 * ColormapController *
 **********************/

class ColormapController: 
    public dataflow::Module, 
    public eventsystem::Node, 
    public eventsystem::PickableObject 
{
private:

    void set_colormap_stop_color(eventsystem::State *event_state);
    void set_colormap_stop_locations(eventsystem::State *event_state);
    void set_colormap_stop_kind(eventsystem::State *event_state);

public:

    static Colormap1d *COLORMAPS[12];
    static const int NUM_COLORMAPS;

public:

    Colormap1d *used_colormaps[12];
    Colormap1d *colormap;
    double min_value;
    double max_value;
    ColormapRenderer *renderer;
    //ColormapRenderer::Handle::Type picked_object;
    TypeAndIndex picked_object;
    int colormap_index;

    ColormapController();

    void setRenderer(ColormapRenderer *renderer);

public:
	
    void process();
	
public: // mouse manager stuff

    string event_node_name(eventsystem::State *event_state);

    bool event_mouse_contains(int x, int y);

    void event_mouse_mouse(eventsystem::State *event_state);

    eventsystem::PickableObject*
    event_mouse_pick(eventsystem::State *event_state);

    bool event_mouse_wheel(eventsystem::State *event_state);

    void event_mouse_drag(eventsystem::State *event_state);

    void another_colormap(int offset);

    bool  event_key_press(eventsystem::State *event_state);



}; // end ColormapController



#endif

