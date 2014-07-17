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

#ifndef MODULES_H
#define MODULES_H

#include <GL/glew.h>

#include <ctime>

#include <vector>

#include <QtOpenGL>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/tokenizer.hpp>

#include "Dataflow.hpp"
#include "DataflowTypes.hpp"
#include "Array.hpp"
#include "font.h"
#include "Labeling.hpp"
#include "datatypes.hpp"
#include "Curve.hpp"

#include "ViewRect.hpp"
#include "Color.hpp"

#include "gl_util.hpp"
#include "Vec2.hpp"

#include "Global.hpp"

#include "Colormap1d.hpp"

#include "ds.hpp"

#include "EventSystem.hpp"

using namespace dataflow;
using namespace std;
using namespace datatypes;

namespace modules { // modules namespace


    class PanelModule: public Module {
    public:
	
	PanelModule();

	void setDataset(datatypes::Array *dataset_array, 
			datatypes::Array *x_coords, 
			datatypes::Array *y_coords);

	void setSpeciesAndTimeIndices(int species, int time);

	void process();

    };

    class ScalarField2DRenderer: public Module {
    public:

	ScalarField2DRenderer();

	void process();

    };
    
    class MapRenderer: public Module {
    public:
	
	MapRenderer();

	void process();

    };

    class GraphController: 
	public Module, 
	public eventsystem::Node, 
	public eventsystem::PickableObject {

    public:
	
	const static float margin_left   = 5;
	const static float margin_right  = 60;
	const static float margin_bottom = 25;
	const static float margin_top    = 20;
	const static float handle_radius = 6;
	const static float text_fix      = 6;

	double x_min_view;
	double x_max_view;
	double y_min_view;
	double y_max_view;

	GraphController();

	void process();
	
    public: // mouse manager stuff

	bool event_mouse_contains(int x, int y);

	string event_node_name(eventsystem::State *event_state);

	void event_mouse_mouse(eventsystem::State *event_state);

	eventsystem::PickableObject *event_mouse_pick(eventsystem::State *event_state);

	bool event_mouse_wheel(eventsystem::State *event_state);

	void event_mouse_drag(eventsystem::State *event_state); 

    }; // end class GraphController
    
    class ArrayBuilder: public Module {

    public:

	map<string, Curve *>       integrals;
	vector<int>*               prev_species_indices;
	vector<datatypes::Curve*> *curves;
	vector<datatypes::Curve*> *curves_new_dataset;

	int run;

	Array *A;

	ArrayBuilder();
	
	string buildKey(vector<int> *species, selection::Selection *selection, string expression);

	void process();
    };

    class Sink: public Module {
    public:
	Sink();
    };

    class MapController: public Module, public eventsystem::Node, public eventsystem::PickableObject {
    public:
	
	enum PickMode { NONE, MAP, NEW_REGION, MOVE_REGION, RESIZE_REGION };

    public:

	static const float COLORS[11][4];
	static const int NUM_COLORS;

    public:

	class Listener {
	public:
	    Listener();
	    virtual void map_controller_update(MapController *mapController, 
					       ViewRect world_window);
	};

    public:

	//vector<datatypes::TagCloudSpec*> tag_clouds;	

	vector<selection::Selection*> selections;
	ViewRect world_window;

	PickMode pick_mode;
	selection::Selection* movingSelection;

	selection::Selection* resizingSelection;
	ViewRect::Corner      resizingCorner;


	//
	double   world_pick_x;
	double   world_pick_y;

	//
	selection::Selection *new_selection;
	int new_rectangle_color_index;

	//
	Color *colors;
	int    num_colors;

	vector<Listener*> listeners;

	//
	double mouse_latitude;
	double mouse_longitude;

	MapController();

	void addListener(Listener *l);
	
	void notify();

	void set_world_region(double x, double y, double w, double h);
	
	void process();

	dataset::DatasetRepository *getDatasetRepository();

	int getDatasetIndex();

	int getTimeIndex();

	void updateSelectionTagCloudInfo();


    public: // mouse manager stuff

	string event_node_name(eventsystem::State *event_state);

	bool event_mouse_drop(eventsystem::State *event_state);

	bool event_mouse_contains(int x, int y);

	void setWorldWindow(ViewRect ww);

	void world2screen(double x, double y, double &xx, double &yy);

	void screen2world(int x, int y, double &wx, double &wy);

	eventsystem::PickableObject *event_mouse_pick(eventsystem::State *event_state);

	bool event_mouse_wheel(eventsystem::State *event_state);

	// int mm_size() { return 0; }

	// Node *mm_get_child(int index) { return NULL; }

	void event_mouse_drag(eventsystem::State *event_state);

	void event_mouse_release(eventsystem::State *event_state);

	void event_mouse_move(eventsystem::State *event_state);

	void open_google_maps_link();

	bool event_key_press(eventsystem::State *event_state);

    }; // end class MapController


    class MapSelectionRenderer: public Module {
    public:
	
	MapSelectionRenderer();

	void process();

    }; // end map selection renderer


    ////////////////////////////////////////////////////////////////////////////////



    class PredictorsController: public Module, public eventsystem::Node, public eventsystem::PickableObject {

    public:

	static const double min_scale = 1.0;
	static const double max_scale = 4.0;

    public:

    	PredictorsController();

    public:
    	class TagCloudItem {
    	public:

    	    double x, y;
    	    double fontSize;
    	    string text;
    	    double relativeSize;

	    // origin (0,0) is the base point
    	    double base_x, base_y, base_w, base_h, base_advance; 

    	    TagCloudItem(string text, double relativeSize);

	    ViewRect rect();

    	};

    public:
	class Line {
	public:

	    int index, len;

	    ViewRect bounds;

	    Line();

	    Line(int index, int len, ViewRect bounds);

	    void init(int index, int len, ViewRect bounds);

	};
    public:

	ViewRect positionTags(vector<TagCloudItem*> *items, float W, 
			      float hsep, float vsep);

    	ViewRect computeTagCloud(vector<TagCloudItem*> *items, double ar_target);

    	void process();
	
    public: // mouse manager stuff

    	bool event_mouse_contains(int x, int y);

    	void event_mouse_mouse(eventsystem::State *event_state);

    	eventsystem::PickableObject *event_mouse_pick(eventsystem::State *event_state);

    	bool event_mouse_wheel(eventsystem::State *event_state);

    	void event_mouse_drag(eventsystem::State *event_state);

    }; // end ColormapController


    class MapLabelsRenderer: public Module {
    public:

	MapLabelsRenderer();

	void process();

    }; // end class MapLabelsRenderer


    class MapData: public Module {
    public:

	MapData();
	
	void process();

    };

    class SelectionsToTagCloud: public Module {
    public:

	SelectionsToTagCloud();

	void world2screen(double x, double y, double &xx, double &yy);

	void process();
    };

}

#endif
