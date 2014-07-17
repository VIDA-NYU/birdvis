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

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

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

#include "Texture.hpp"

#include <boost/signals2.hpp>

#include <boost/bind.hpp>

/**************
 * FileModule *
 **************/

class FileModule:
    public dataflow::Module
{
private:
    bvdata::File* file;
public:
    FileModule();
    bvdata::File*  getFile();
    void setFile(bvdata::File* f);    
    void process();
    bool fileLoaded();
};

/*************************
 * ScalarFieldDataModule *
 *************************/

class ScalarFieldData:
    public dataflow::Module
{
private:

    int time_offset;

public:

    ScalarFieldData();

    void process();

};

/**************************
 * ScalarFieldDisplayList *
 **************************/

class ScalarFieldDisplayList:
    public dataflow::Module
{
private:

    GLuint id_list;
    GLuint id_texture;

    bool   allocated;

public:

    ScalarFieldDisplayList();

    void process();

};


/***********************
 * ScalarFieldRenderer *
 ***********************/

class ScalarFieldRenderer: 
    public Module 
{
public:

    ScalarFieldRenderer();

    void process();

};


/*************
 * MapLabels *
 *************/

class MapLabels: 
    public Module 
{
public:

    MapLabels();

    void process();

};


/******************
 * CursorRenderer *
 ******************/

class CursorRenderer: 
    public Module 
{
public:
    CursorRenderer();
    void process();
};


/*****************
 * InfoRenderer *
 *****************/

class InfoRenderer: 
    public Module 
{
public:
    //Button*      btn_test_button;
    ScrollPane *scrollPane;
    InfoRenderer();
    void process();
};


/*****************
 * PhotoRenderer *
 *****************/

class PhotoRenderer: 
    public Module 
{
private:
    // GLuint texture_id;
//     bool   texture_loaded;
    Texture* texture;
    bool     useLogo;
public:
    //Button*      btn_test_button;
    //ScrollPane *scrollPane;    
    PhotoRenderer();
    ~PhotoRenderer();
    void load(string filename);
    void deleteTexture();
    void process();
    void toggleUseLogo();
};



class OccurrenceEnvironment;

/***********
 * Toolbar *
 ***********/

class Toolbar:
    public Component,
    public rectanglelayout::RectangleSlot
{
private:
    ViewRect                   screen_window;

    OccurrenceEnvironment*     occurrence_env;

    Button*      btn_toggle_cursor;
    Button*      btn_toggle_habitat;
    Button*      btn_toggle_integral_curve;
    Button*      btn_search;
    Button*      btn_google_maps;
    Button*      btn_interpolate;
    Button*      btn_toggle_stencil;
    Button*      btn_pie_chart;

public:

    Toolbar(OccurrenceEnvironment     *occurrence_env);

    void installAt(rectanglelayout::Node *area);

    void render();

    // void incrementOffsetSignal();

    // void decrementOffsetSignal();

    void rectangleSignal(rectanglelayout::Rectangle r);

    bool event_mouse_contains(int x, int y);

    void toggleCursor(int state);

    void toggleHabitat(int state);

    void toggleInterpolate(int state);

    void search(int state);

    void google_maps(int state);

    void interpolate(int state);

    void show_hide_global_integral_curves(int state);

    void toggle_stencil(int state);

    void pie_chart(int state);

};

/*********************
 * PredictorsToolBar *
 *********************/

class PredictorsToolbar:
    public Component,
    public rectanglelayout::RectangleSlot
{
private:
    ViewRect                   screen_window;

    OccurrenceEnvironment*     occurrence_env;
    vector<Button*>            predictors_btns;


    // Button*      btn_toggle_cursor;
//     Button*      btn_toggle_habitat;
//     Button*      btn_toggle_integral_curve;
//     Button*      btn_search;
//     Button*      btn_google_maps;
//     Button*      btn_interpolate;
//     Button*      btn_toggle_stencil;

public:

    PredictorsToolbar(OccurrenceEnvironment     *occurrence_env);

    void installAt(rectanglelayout::Node *area);

    void render();

    void init(vector<string> predictors);

    // void incrementOffsetSignal();

    // void decrementOffsetSignal();

    void rectangleSignal(rectanglelayout::Rectangle r);

    bool event_mouse_contains(int x, int y);

    void clickEvent(int state, Button* btn);

    void addButton(Button* btn);

};

/*****************
 * TitleRenderer *
 *****************/

class TitleRenderer: 
    public Module 
{
private:
    
    string title;

public:

    TitleRenderer();

    string getTitle();

    void setTitle(string title);

    void process();

};


/**************************************
 * TagCloudOnSelectionsInfrastructure *
 **************************************/

class TagCloudOnSelectionsInfrastructure
{
public:

    FileModule*                       mod_file; 
    OffsetModule*                     mod_offseted_time_index; 
    TagCloudRenderer                  mod_tag_cloud_renderer;  
    WorldWindowModule*                mod_world_window;
    ScreenWindowModule*               mod_screen_window;
    VectorIntModule*                  mod_tagcloud_mask;
    map<Selection* , TagCloudData*>   map_selection_to_data;
    
    TagCloudOnSelectionsInfrastructure(
	ScreenWindowModule*     mod_screen_window,
	WorldWindowModule*      mod_world_window,
	OffsetModule*           mod_offseted_time_index,
	FileModule*             mod_file,
	IntModule*              mod_tagcloud_on_off,
	SelectionRepository*    selection_repository,
	VectorIntModule*        mod_tagcloud_mask,
	int                     time_offset);

    void render();

    void receiveDeleteSelectionSignal(Selection *sel);

    void receiveNewSelectionSignal(Selection *sel);
};


/*************************
 * MapCellInfrastrucutre *
 *************************/

class MapCellInfrastructure
{
public:

    ScreenWindowModule                  mod_screen_window;
    MapBordersRenderer                  mod_map_borders_renderer;
    ScalarFieldDisplayList              mod_scalar_field_display_list;
    ScalarFieldRenderer                 mod_scalar_field_renderer;
    ScalarFieldData                     mod_scalar_field_data;

    OffsetModule                        mod_offseted_time_index; 

    LonLatGridValue                     mod_lonlat_grid_value;

    MapSelectionsRenderer               mod_map_selections_renderer;
    
    MapLabels                           mod_map_labels;

    ZoomAndPanController                zoom_and_pan_controller;

    SelectionController*                selection_controller;

    TagCloudOnSelectionsInfrastructure* tag_cloud_infrastructure;
    
    CursorRenderer                      mod_cursor_renderer;

    FileModule*                         mod_file;

    Button*                             btn_offset_plus;
    Button*                             btn_offset_minus;

    MapCellInfrastructure(
	WorldWindowModule*      mod_world_window,
	StringModule*           mod_expression,
	IntModule*              mod_time_index,
	ColormapController*     mod_colormap_controller,
	IntModule*              mod_map_borders_mode,
	IntModule*              mod_interpolation_mode,
	IntModule*              mod_stencil_flag,
	FileModule*             mod_file,
	LonLatModule*           mod_longitude_latitude,
	IntModule*              mod_cursor_on_off,
	IntModule*              mod_tag_cloud_on_off,
	VectorIntModule*        mod_tagcloud_mask,
	SelectionRepository*    selection_repository,
	int                     time_offset,
	int                     max_time_offset,
	string                  var_name);

    void installAt(rectanglelayout::Node *area);

    void render();

    void receiveDeleteSelectionSignal(Selection *sel);

    void receiveNewSelectionSignal(Selection *sel);

    void incrementOffsetSignal();

    void decrementOffsetSignal();

    void setMaxTimeOffset(int max_time_offset);

};

/*************************
 * OccurrenceEnvironment *
 *************************/

class OccurrenceEnvironment:
    public Component,
    public rectanglelayout::RectangleSlot
{
private:
    // modules
    int xcells;
    int ycells;
    int num_expressions;

    ViewRect screen_window;

    // expression modules
    FileModule                            mod_file;

    // expression modules
    vector<StringModule*>                 mod_expressions;

    // colormap: one for each expression
    vector<ColormapController*>           mod_colormap_controllers;
    vector<ColormapRenderer*>             mod_colormap_renderers;
    vector<ScreenWindowModule*>           mod_colormap_windows;

    // plots: one for each expression
    vector<PlotController*>               plot_controllers;
    vector<PlotRenderer*>                 mod_plot_renderers;
    vector<ScreenWindowModule*>           mod_plot_windows;

    ScreenWindowModule                    mod_title_window;
    TitleRenderer                         mod_title_renderer;
    WorldWindowModule                     mod_world_window;
    IntModule                             mod_time_index;
    ScreenWindowModule                    mod_info_window;
    InfoRenderer                          mod_info_renderer;

    ScreenWindowModule                    mod_photo_window;
    PhotoRenderer                         mod_photo_renderer;


    // global module
    LonLatModule                          mod_longitude_latitude;

    IntModule                             mod_map_borders_mode;
    IntModule                             mod_interpolation_mode;
    IntModule                             mod_stencil_flag;

    // on off modules
    IntModule                             mod_cursor_on_off;
    IntModule                             mod_habitat_tc_on_off;

    //Tag Cloud Maks
    VectorIntModule                       mod_tc_mask;

    // all cells
    vector<MapCellInfrastructure* >       map_cells;

    // integral curves: one for each expression
    vector<CurveData* >                   mod_integral_curves;

    // selections
    SelectionRepository                   selection_repository;
    map<Selection*, CurveData* >          map_selection_to_curve;

    // toolbar
    Toolbar*                              toolbar;
    
    rectanglelayout::Node*                cinfo;
    rectanglelayout::Node*                cphoto;
    PredictorsToolbar*                    p_toolbar;

    // Predictors List    
    //ScrollPane*                           predictorsList;

    void cellInfo(int x, int y, int &time_offset, int &expression);

public:
    
    void receiveDeleteSelectionSignal(Selection *sel);

    void receiveNewSelectionSignal(Selection *sel);

public:

    OccurrenceEnvironment(int xcells, int ycells, int num_expressions);
    ~OccurrenceEnvironment();

    MapCellInfrastructure* getMapCellInfrastructure(int xcell, int ycell);

    void installAt(rectanglelayout::Node *area);

    void render();
    
    bool event_mouse_contains(int x, int y);

    void rectangleSignal(rectanglelayout::Rectangle rect);

    bool event_key_press(eventsystem::State *event_state);

    bool event_mouse_drop(eventsystem::State *event_state);

    void show_hide_global_integral_curves(int state);

    void toggle_interpolate(int state);

    void toggle_stencil(int state);

    void open_pie_chart();

    void change_tc_mask(int state, int index);

public:

    friend class Toolbar;

};

// /********************
//  * WindowController *
//  ********************/

// class WindowController:
//     eventsystem::Node
// {
// private:
//     WindowModule target_window;
// public:
//     WindowModule();
//     void process();
// };

// /************************
//  * MigrationEnvironment *
//  ************************/

// class MigrationEnvironment
// {
// private:
// public:
// };

#endif
