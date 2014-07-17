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

#ifndef SIMPLE_ENVIRONMENT
#define SIMPLE_ENVIRONMENT

#include <map>

#include "ViewRect.hpp"

#include "EventSystem.hpp"

#include "RectangleLayout.hpp"

#include "Spreadsheet.hpp"

#include "ColormapModules.hpp"

#include "TagCloudModules.hpp"

#include "bvdata.hpp"

#include "modules.hpp"

#include "PlotModules.hpp"

#include "BasicModules.hpp"

#include <boost/signals2.hpp>

#include <boost/bind.hpp>

#include <Environment.hpp>

/*********************
 * SimpleEnvironment *
 *********************/
class SimpleEnvironment:
    public Component,
    public rectanglelayout::RectangleSlot
{
private:

    ViewRect                            screen_window;

    // screen window
    ScreenWindowModule                  mod_screen_window;

    // expression module
    StringModule                        mod_expression;

    // file module
    FileModule                          mod_file; 

    // time module
    IntModule*                          mod_time_index;

    //Tag Cloud Maks
    VectorIntModule                     mod_tc_mask;
    
    // time module
    OffsetModule                        mod_offseted_time_index;

    // world window
    WorldWindowModule*                  mod_world_window;

    // info window and renderer
    ScreenWindowModule                  mod_info_window;
    InfoRenderer                        mod_info_renderer;
    
    ScreenWindowModule                  mod_photo_window;
    PhotoRenderer                       mod_photo_renderer;

    // global module
    LonLatModule                        mod_longitude_latitude;

    // map borders on/off, interpolation mode
    IntModule                           mod_map_borders_mode;
    IntModule                           mod_interpolation_mode;
    IntModule                           mod_stencil_flag;

    // single map infrastructure
    ScreenWindowModule                  mod_map_window;
    MapBordersRenderer                  mod_map_borders_renderer;
    ScalarFieldDisplayList              mod_scalar_field_display_list;
    ScalarFieldRenderer                 mod_scalar_field_renderer;
    ScalarFieldData                     mod_scalar_field_data;

    // compute value of lon/lat
    LonLatGridValue                     mod_lonlat_grid_value;

    // draw selections on map
    MapSelectionsRenderer               mod_map_selections_renderer;

    // draw labels on map
    MapLabels                           mod_map_labels;

    // control map zoom  and pan
    ZoomAndPanController                zoom_and_pan_controller;

    // selection controller
    SelectionController*                selection_controller;

    // tag cloud infrastructure
    TagCloudOnSelectionsInfrastructure* tag_cloud_infrastructure;

    //
    CursorRenderer                      mod_cursor_renderer;
    Button*                             btn_offset_plus;
    Button*                             btn_offset_minus;
    Button*                             btn_habitat_on_off;
    Button*                             btn_search;
    Button*                             btn_interpolate;
    Button*                             btn_stencil;

    // plot infrastructure
    PlotController                      mod_plot_controller;
    PlotRenderer                        mod_plot_renderer;
    ScreenWindowModule                  mod_plot_window;

    // colormap
    ColormapController                  mod_colormap_controller;
    ColormapRenderer                    mod_colormap_renderer;
    ScreenWindowModule                  mod_colormap_window;

    // curves data
    CurveData                           mod_integral_curve;

    // selection repository
    SelectionRepository*                selection_repository;

    ScreenWindowModule                  mod_tag_cloud_window;
    SimpleTagCloudData                  mod_tag_cloud_data;
    TagCloudRenderer                    mod_tag_cloud_renderer;

    // selection -> curve
    map<Selection*, CurveData* >        map_selection_to_curve;

    // render tag clouds on second environment
    TagCloudRenderer                    mod_in_map_tag_cloud_renderer;  
    map<Selection* , TagCloudData*>     map_selection_to_tag_cloud_data;

public:
    SimpleEnvironment(IntModule           *mod_time_index, 
		      WorldWindowModule   *mod_world,
		      SelectionRepository *selection_repository);

    void installAt(rectanglelayout::Node *area);

    void incrementOffsetSignal(int state);

    void decrementOffsetSignal(int state);

    void toggleHabitatTC(int state);

    void toggleInterpolate(int state);

    void toggleStencil(int state);

    void search(int state);

    bool event_mouse_drop(eventsystem::State *event_state);

    void rectangleSignal(rectanglelayout::Rectangle rect);

    bool event_mouse_contains(int x, int y);

    void render();

    bool event_key_press(eventsystem::State *event_state);

    void receiveNewSelectionSignal(Selection *sel);

    void receiveDeleteSelectionSignal(Selection *sel);


};


/***************************
 * SimpleEnvironmentMatrix *
 ***************************/
class SimpleEnvironmentMatrix:
    public Component,
    public rectanglelayout::RectangleSlot
{
private:

    int cols;
    int rows;
    
    ViewRect                            screen_window;

    // time module
    IntModule                           mod_time_index;
    
    // world window
    WorldWindowModule                   mod_world_window;

    //
    vector<SimpleEnvironment*>          simple_environments;

    // selection repository
    SelectionRepository                 selection_repository;
    

public:

    SimpleEnvironmentMatrix(int rows, int cols);

    SimpleEnvironment* getSimpleEnvironment(int r, int c);

    void installAt(rectanglelayout::Node *area);

    void rectangleSignal(rectanglelayout::Rectangle rect);

    bool event_mouse_contains(int x, int y);

    void render();

    bool event_key_press(eventsystem::State *event_state);

    // void receiveNewSelectionSignal(Selection *sel);

    // void receiveDeleteSelectionSignal(Selection *sel);

};









// /**************************************
//  * TagCloudOnSelectionsInfrastructure *
//  **************************************/

// class TagCloudOnSelectionsInfrastructure
// {
// public:

//     FileModule*                       mod_file; 
//     OffsetModule*                     mod_offseted_time_index; 
//     TagCloudRenderer                  mod_tag_cloud_renderer;  
//     WorldWindowModule*                mod_world_window;
//     ScreenWindowModule*               mod_screen_window;
//     map<Selection* , TagCloudData*>   map_selection_to_data;
    
//     TagCloudOnSelectionsInfrastructure(
// 	ScreenWindowModule*     mod_screen_window,
// 	WorldWindowModule*      mod_world_window,
// 	OffsetModule*           mod_offseted_time_index,
// 	FileModule*             mod_file,
// 	SelectionRepository*    selection_repository,
// 	int                     time_offset);

//     void render();

//     void receiveDeleteSelectionSignal(Selection *sel);

//     void receiveNewSelectionSignal(Selection *sel);
// };


// /*************************
//  * MapCellInfrastrucutre *
//  *************************/

// class MapCellInfrastructure
// {
// public:

//     ScreenWindowModule                  mod_screen_window;
//     MapBordersRenderer                  mod_map_borders_renderer;
//     ScalarFieldDisplayList              mod_scalar_field_display_list;
//     ScalarFieldRenderer                 mod_scalar_field_renderer;
//     ScalarFieldData                     mod_scalar_field_data;

//     OffsetModule                        mod_offseted_time_index; 

//     LonLatGridValue                     mod_lonlat_grid_value;

//     MapSelectionsRenderer               mod_map_selections_renderer;
    
//     MapLabels                           mod_map_labels;

//     ZoomAndPanController                zoom_and_pan_controller;

//     SelectionController*                selection_controller;

//     TagCloudOnSelectionsInfrastructure* tag_cloud_infrastructure;
    
//     CursorRenderer                      mod_cursor_renderer;

//     Button*                             btn_offset_plus;
//     Button*                             btn_offset_minus;

//     MapCellInfrastructure(
// 	WorldWindowModule*      mod_world_window,
// 	StringModule*           mod_expression,
// 	IntModule*              mod_time_index,
// 	ColormapController*     mod_colormap_controller,
// 	IntModule*              mod_map_borders_mode,
// 	IntModule*              mod_interpolation_mode,
// 	FileModule*             mod_file,
// 	LonLatModule*           mod_longitude_latitude,
// 	SelectionRepository*    selection_repository,
// 	int                     time_offset,
// 	string                  var_name);

//     void installAt(rectanglelayout::Node *area);

//     void render();

//     void receiveDeleteSelectionSignal(Selection *sel);

//     void receiveNewSelectionSignal(Selection *sel);

//     void incrementOffsetSignal();

//     void decrementOffsetSignal();

// };


// /*************************
//  * OccurrenceEnvironment *
//  *************************/

// class OccurrenceEnvironment:
//     public Component,
//     public rectanglelayout::RectangleSlot
// {

// private:
//     // modules
//     int xcells;
//     int ycells;
//     int num_expressions;

//     ViewRect screen_window;

//     // expression modules
//     FileModule                            mod_file;

//     // expression modules
//     vector<StringModule*>                 mod_expressions;

//     // colormap: one for each expression
//     vector<ColormapController*>           mod_colormap_controllers;
//     vector<ColormapRenderer*>             mod_colormap_renderers;
//     vector<ScreenWindowModule*>           mod_colormap_windows;

//     // plots: one for each expression
//     vector<PlotController*>               plot_controllers;
//     vector<PlotRenderer*>                 mod_plot_renderers;
//     vector<ScreenWindowModule*>           mod_plot_windows;

//     ScreenWindowModule                    mod_title_window;
//     TitleRenderer                         mod_title_renderer;
//     WorldWindowModule                     mod_world_window;
//     IntModule                             mod_time_index;
//     ScreenWindowModule                    mod_info_window;
//     InfoRenderer                          mod_info_renderer;

//     // global module
//     LonLatModule                          mod_longitude_latitude;

//     IntModule                             mod_map_borders_mode;
//     IntModule                             mod_interpolation_mode;

//     // all cells
//     vector<MapCellInfrastructure* >       map_cells;

//     // integral curves: one for each expression
//     vector<CurveData* >                   mod_integral_curves;

//     // selections
//     SelectionRepository                   selection_repository;
//     map<Selection*, CurveData* >          map_selection_to_curve;

//     void cellInfo(int x, int y, int &time_offset, int &expression);

// public:
    
//     void receiveDeleteSelectionSignal(Selection *sel);

//     void receiveNewSelectionSignal(Selection *sel);

// public:

//     OccurrenceEnvironment();

//     MapCellInfrastructure* getMapCellInfrastructure(int xcell, int ycell);

//     void installAt(rectanglelayout::Node *area);

//     void render();
    
//     bool event_mouse_contains(int x, int y);

//     void rectangleSignal(rectanglelayout::Rectangle rect);

//     bool event_key_press(eventsystem::State *event_state);

//     bool event_mouse_drop(eventsystem::State *event_state);

// };

#endif
