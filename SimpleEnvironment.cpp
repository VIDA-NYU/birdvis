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

#include "SimpleEnvironment.hpp"

#include <QtGui>
#include <QtOpenGL>
#include <QDialog>

#include <boost/any.hpp>

#define xDEBUG_SIMPLEENVIRONMENT



/***************************
 * SimpleEnvironmentMatrix *
 ***************************/

SimpleEnvironmentMatrix::SimpleEnvironmentMatrix(int rows, int cols):
    cols(cols),
    rows(rows)
{
    for (int i=0;i<rows;i++)
    {
	for (int j=0;j<cols;j++)
	{
	    SimpleEnvironment *se = new SimpleEnvironment(
		&mod_time_index,
		&mod_world_window,
		&selection_repository);
	    
	    simple_environments.push_back(se);

	    // add simple environment
	    event_add_node(se);
	}
    }
}

SimpleEnvironment* 
SimpleEnvironmentMatrix::getSimpleEnvironment(int r, int c){
    if(r < 0 || r >= this->rows || c < 0 || c >= this->cols)
	return NULL;
    else
	return this->simple_environments[r * cols + c];
}

void 
SimpleEnvironmentMatrix::rectangleSignal(rectanglelayout::Rectangle rect)
{
    screen_window.init(rect.x, rect.y, rect.w, rect.h);
}

void 
SimpleEnvironmentMatrix::installAt(rectanglelayout::Node *area)
{
    if (!area->isLeaf())
    {
	return;
    }

    rectanglelayout::RectangleLayout* rs = area->getRectangleLayout();

    // subdivide lead into multiple regions
    rectanglelayout::Grid* root    = rs->subdivide(area->asLeaf(),cols,rows);

    for (int i=0;i<rows;i++)
    {
	for (int j=0;j<cols;j++)
	{
	    int index = i * cols + j;
	    rectanglelayout::Node* area = rs->getNode(root,j,i);
	    simple_environments[index]->installAt(area);
	}
    }
    connectToRectangleSignal(root);

}


void 
SimpleEnvironmentMatrix::render()
{
    for (int i=0;i<rows;i++)
    {
	for (int j=0;j<cols;j++)
	{
	    int index = i * cols + j;
	    simple_environments[index]->render();
	}
    }
}

bool 
SimpleEnvironmentMatrix::event_mouse_contains(int x, int y)
{
    return screen_window.contains(x,y);
}

bool 
SimpleEnvironmentMatrix::event_key_press(eventsystem::State *event_state) 
{
    //this->update_dataset(NULL);
    QString st;
    bool ok;

    switch(event_state->key)
    {
    case (eventsystem::Key_Left):

	//cout << "!!!!!!!!!!!!!!!!!!!!!Simple Environment press left" << endl;

	for(int i = 0 ; i < this->rows ; i++){
	    	for(int j = 0 ; j < this->cols ; j++){
		    SimpleEnvironment* se = this->getSimpleEnvironment(i, j);
		    se->decrementOffsetSignal(0);
		}
	}
	//mod_time_index.setInt((mod_time_index.getInt() + 51) % 52);
	return true;
	
    case (eventsystem::Key_Right):

	// if(this->mod_file.fileLoaded()){
// 	    numTimeSteps = this->mod_file.getFile()->getNumTimeSteps();
// 	    mod_time_index.setInt((mod_time_index.getInt() + 1) % numTimeSteps);
// 	}

	for(int i = 0 ; i < this->rows ; i++){
	    	for(int j = 0 ; j < this->cols ; j++){
		    SimpleEnvironment* se = this->getSimpleEnvironment(i, j);
		    se->incrementOffsetSignal(0);
		}
	}


//	mod_time_index.setInt((mod_time_index.getInt() + 1) % 52);
	return true;
    default:
	return false;
    }
    return false;
}

/*********************
 * SimpleEnvironment *
 *********************/

SimpleEnvironment::SimpleEnvironment(
    IntModule *mod_time_index, 
    WorldWindowModule   *mod_world_window,
    SelectionRepository *selection_repository
):

    mod_time_index(mod_time_index),
    mod_world_window(mod_world_window),
    selection_repository(selection_repository)
{

    mod_stencil_flag.setInt(1);
    mod_interpolation_mode.setInt(1);

    { // global clock
	mod_offseted_time_index.setOrder(52);
	mod_offseted_time_index.setOffset(0);
	new Connection(mod_time_index->getOutputPort("int_value"),
		       mod_offseted_time_index.getInputPort("input_int"));

    } // global clock


    { // wiring colormap
	new Connection(mod_colormap_window.getOutputPort("screen_window"),
		       mod_colormap_controller.getInputPort("screen_window"));
	new Connection(mod_colormap_controller.getOutputPort("colormap"),
		       mod_colormap_renderer.getInputPort("colormap"));
	new Connection(mod_colormap_controller.getOutputPort("range"),
		       mod_colormap_renderer.getInputPort("range"));
	new Connection(mod_colormap_window.getOutputPort("screen_window"),
		       mod_colormap_renderer.getInputPort("screen_window"));

	//
	mod_colormap_controller.setRenderer(&mod_colormap_renderer);

	mod_colormap_renderer.getInputPort("title")->
	    setData(new data::SingleString("p"));


	// register child event listener
	event_add_node(&mod_colormap_controller);
    } // wiring colormap


    { // wiring plot
    	new Connection(mod_plot_window.getOutputPort("screen_window"),
    		       mod_plot_renderer.getInputPort("screen_window"));
    	new Connection(mod_integral_curve.getOutputPort("curve"),
    		       mod_plot_renderer.getInputPort("curves"));

    	new Connection(this->mod_file.getOutputPort("file"),
    		       mod_integral_curve.getInputPort("file"));

	new Connection(this->mod_file.getOutputPort("file"),
    		       mod_plot_renderer.getInputPort("file"));

	new Connection(mod_offseted_time_index.getOutputPort("output_int"),
		       mod_plot_renderer.getInputPort("time_indices"));


	mod_plot_controller.setPlotRenderer(&mod_plot_renderer);

	string plot_title = "Occurrence (p)";
	mod_plot_renderer.getInputPort("title")->setData(new data::SingleString(plot_title));

	event_add_node(&mod_plot_controller);
    } // wiring plot


    { // wiring info renderer
    	new Connection(mod_file.getOutputPort("file"),
    		       mod_info_renderer.getInputPort("file"));
    	new Connection(mod_info_window.getOutputPort("screen_window"),
    		       mod_info_renderer.getInputPort("screen_window"));
	new Connection(mod_longitude_latitude.getOutputPort("lonlat"),
		       mod_info_renderer.getInputPort("lonlat"));
    } // wiring info renderer


// wiring...
    {
    	new Connection(mod_photo_window.getOutputPort("screen_window"),
    		       mod_photo_renderer.getInputPort("screen_window"));
    }


    { // zoom & pan and selection controller
	zoom_and_pan_controller.setWorldWindowModule(mod_world_window);
	zoom_and_pan_controller.setLonLatModule(&mod_longitude_latitude);
	selection_controller = new SelectionController(selection_repository, 
						       mod_world_window);
	zoom_and_pan_controller.event_add_node(selection_controller);
	event_add_node(&zoom_and_pan_controller);
    } // zoom & pan and selection controller


    { // mod_cursor_renderer

	new Connection(mod_longitude_latitude.getOutputPort("lonlat"),
		       mod_cursor_renderer.getInputPort("lonlat"));

	new Connection(mod_map_window.getOutputPort("screen_window"),
		       mod_cursor_renderer.getInputPort("screen_window"));

	new Connection(mod_world_window->getOutputPort("world_window"),
		       mod_cursor_renderer.getInputPort("world_window"));

    } // mod_cursor_renderer

    { // mod_lonlat_grid_value

	new Connection(mod_longitude_latitude.getOutputPort("lonlat"),
		       mod_lonlat_grid_value.getInputPort("lonlat"));

	new Connection(mod_scalar_field_data.getOutputPort("grid"),
		       mod_lonlat_grid_value.getInputPort("grid"));
	
    } // mod_lonlat_grid_value


    { // mod_map_borders_renderer

	new Connection(mod_world_window->getOutputPort("world_window"),
		       mod_map_borders_renderer.getInputPort("world_window"));

	new Connection(mod_map_window.getOutputPort("screen_window"),
		       mod_map_borders_renderer.getInputPort("screen_window"));

	new Connection(mod_map_borders_mode.getOutputPort("int_value"),
		       mod_map_borders_renderer.getInputPort("map_borders_mode"));

    } // mod_map_borders_renderer


    { // mod_scalar_field_data

	new Connection(mod_expression.getOutputPort("string_value"),
		       mod_scalar_field_data.getInputPort("expression"));

	new Connection(mod_offseted_time_index.getOutputPort("output_int"),
		       mod_scalar_field_data.getInputPort("time_index"));

    	new Connection(mod_file.getOutputPort("file"),
    		       mod_scalar_field_data.getInputPort("file"));

    } // mod_scalar_field_data


    { // mod_scalar_field_display_list

	new Connection(mod_scalar_field_data.getOutputPort("grid"),
		       mod_scalar_field_display_list.getInputPort("grid"));

	new Connection(mod_colormap_controller.getOutputPort("colormap"),
		       mod_scalar_field_display_list.getInputPort("colormap"));

	new Connection(mod_interpolation_mode.getOutputPort("int_value"),
		       mod_scalar_field_display_list.getInputPort("interpolation_mode"));

    } // mod_scalar_field_display_list

    { // mod_scalar_field_renderer

	new Connection(mod_world_window->getOutputPort("world_window"),
		       mod_scalar_field_renderer.getInputPort("world_window"));

	new Connection(mod_map_window.getOutputPort("screen_window"),
		       mod_scalar_field_renderer.getInputPort("screen_window"));

	new Connection(mod_scalar_field_display_list.getOutputPort("display_list"),
		       mod_scalar_field_renderer.getInputPort("display_list"));

	new Connection(mod_stencil_flag.getOutputPort("int_value"),
		       mod_scalar_field_renderer.getInputPort("stencil_flag"));		

    } // mod_scalar_field_renderer


    { // mod_map_labels

	new Connection(mod_map_window.getOutputPort("screen_window"),
		       mod_map_labels.getInputPort("screen_window"));

	new Connection(mod_offseted_time_index.getOutputPort("output_int"),
		       mod_map_labels.getInputPort("time_index"));

	new Connection(mod_lonlat_grid_value.getOutputPort("value"),
		       mod_map_labels.getInputPort("value"));

	new Connection(mod_file.getOutputPort("file"),
		       mod_map_labels.getInputPort("file"));


	mod_map_labels.getInputPort("var_name")->setData(new data::SingleString("p"));

    } // mod_map_labels


    { // mod_map_selections_renderer

	new Connection(mod_map_window.getOutputPort("screen_window"),
		       mod_map_selections_renderer.getInputPort("screen_window"));

	new Connection(mod_world_window->getOutputPort("world_window"),
		       mod_map_selections_renderer.getInputPort("world_window"));

    } // mod_map_selections_renderer


    { // tag cloud

	new Connection(mod_offseted_time_index.getOutputPort("output_int"),
		       mod_tag_cloud_data.getInputPort("time_index"));

	new Connection(mod_file.getOutputPort("file"),
		       mod_tag_cloud_data.getInputPort("species_file"));

	new Connection(mod_tag_cloud_window.getOutputPort("screen_window"),
		       mod_tag_cloud_data.getInputPort("screen_window"));

	new Connection(mod_tag_cloud_data.getOutputPort("tag_cloud"),
		       mod_tag_cloud_renderer.getInputPort("tag_clouds"));

	new Connection(mod_tag_cloud_window.getOutputPort("screen_window"),
		       mod_tag_cloud_renderer.getInputPort("clipping_region"));

	new Connection(mod_file.getOutputPort("file"),
		       mod_tag_cloud_data.getInputPort("species_file"));

	mod_tag_cloud_renderer.getInputPort("black_text")->
	    setData(new data::SingleInt(1));

	mod_tag_cloud_renderer.getInputPort("title")->
	    setData(new data::SingleString("Habitat Preference at this Date"));
    } // tag cloud


    { // in map tag cloud renderer

	new Connection(mod_map_window.getOutputPort("screen_window"),
		       mod_in_map_tag_cloud_renderer.getInputPort("clipping_region"));
	// mod_in_map_tag_cloud_renderer.getInputPort("black_text")->
	//     setData(new data::SingleInt(1));

	// mod_in_map_tag_cloud_renderer.getInputPort("title")->
	//     setData(new data::SingleString("Habitat Preference at this Date"));

    } // in map tag cloud renderer


    { // connect buttons

	// + button
	ViewRect btn_minus_area(124 + 4,-28,30,24);
	btn_offset_minus = new Button(btn_minus_area,"<", 
				      (Button::LEFT | Button::TOP));
	btn_offset_minus->connect(boost::bind(&SimpleEnvironment::decrementOffsetSignal, this,_1));

	// - button
	ViewRect btn_plus_area(124  + 4 + 30 + 4, -28, 30, 24);
	btn_offset_plus = new Button(btn_plus_area,">", 
				     (Button::LEFT | Button::TOP));
	btn_offset_plus->connect(boost::bind(&SimpleEnvironment::incrementOffsetSignal, this,_1));

	// habitat button
	ViewRect btn_habitat_on_off_area(-70, 4, 60, 22);
	btn_habitat_on_off = new Button(btn_habitat_on_off_area,"Habitat", 
				     (Button::RIGHT | Button::BOTTOM));
	btn_habitat_on_off->font_size = 13;
	btn_habitat_on_off->bg_color     = *ColorConstants::MAP_BTN_BG;
	btn_habitat_on_off->text_color_0 = *ColorConstants::MAP_BTN_TEXT;
	btn_habitat_on_off->text_color_1 = *ColorConstants::MAP_BTN_OFF;
	btn_habitat_on_off->toggle       = true;

	btn_habitat_on_off->connect(
	    boost::bind(&SimpleEnvironment::toggleHabitatTC, this, _1));
	mod_in_map_tag_cloud_renderer.setVisible(false); // start not visible
	mod_in_map_tag_cloud_renderer.setVisible(false); // start not visible

	// search button
	ViewRect btn_search_area(-134, 4, 60, 22);
	btn_search = new Button(btn_search_area,"Search", 
				     (Button::RIGHT | Button::BOTTOM));
	btn_search->font_size    = 13;
	btn_search->bg_color     = *ColorConstants::MAP_BTN_BG;
	btn_search->text_color_0 = *ColorConstants::MAP_BTN_TEXT;
	btn_search->text_color_1 = *ColorConstants::MAP_BTN_OFF;
	btn_search->connect(
	    boost::bind(&SimpleEnvironment::search, this, _1));

	// interpolate button
	ViewRect btn_interpolate_area(-204, 4, 66, 22);
	btn_interpolate = new Button(btn_interpolate_area,"Interpolate", 
				(Button::RIGHT | Button::BOTTOM));
	btn_interpolate->font_size    = 13;
	int state = mod_interpolation_mode.getInt();
	btn_interpolate->state = state;
	btn_interpolate->toggle = state;
	btn_interpolate->bg_color     = *ColorConstants::MAP_BTN_BG;
	btn_interpolate->text_color_0 = *ColorConstants::MAP_BTN_TEXT;
	btn_interpolate->text_color_1 = *ColorConstants::MAP_BTN_OFF;
	btn_interpolate->connect(
	    boost::bind(&SimpleEnvironment::toggleInterpolate, this, _1));

	// crop button
	ViewRect btn_stencil_area(-270, 4, 60, 22);
	btn_stencil = new Button(btn_stencil_area,"Crop", 
				(Button::RIGHT | Button::BOTTOM));
	btn_stencil->font_size    = 13;
	state = mod_stencil_flag.getInt();
	btn_stencil->state = state;
	btn_stencil->toggle = state;
	btn_stencil->bg_color     = *ColorConstants::MAP_BTN_BG;
	btn_stencil->text_color_0 = *ColorConstants::MAP_BTN_TEXT;
	btn_stencil->text_color_1 = *ColorConstants::MAP_BTN_OFF;
	btn_stencil->connect(
	    boost::bind(&SimpleEnvironment::toggleStencil, this, _1));

    } // connect buttons

    // order of event process
    zoom_and_pan_controller.event_add_node(selection_controller);
    selection_controller->event_add_node(btn_offset_minus);
    selection_controller->event_add_node(btn_offset_plus);
    selection_controller->event_add_node(btn_habitat_on_off);
    selection_controller->event_add_node(btn_search);
    selection_controller->event_add_node(btn_interpolate);
    selection_controller->event_add_node(btn_stencil);

    // // tcinfra
    // tag_cloud_infrastructure = new TagCloudOnSelectionsInfrastructure(
    // 	&mod_screen_window,
    // 	mod_world_window,
    // 	&mod_offseted_time_index,
    // 	mod_file,
    // 	selection_repository,
    // 	time_offset);

    // // receive selection signal
    selection_repository->connectToAddSelectionSignal(
    	boost::bind(&SimpleEnvironment::receiveNewSelectionSignal, this, _1));
    selection_repository->connectToDelSelectionSignal(
    	boost::bind(&SimpleEnvironment::receiveDeleteSelectionSignal, this, _1));
}

void 
SimpleEnvironment::incrementOffsetSignal(int state)
{
    if(this->mod_file.fileLoaded()){
	int numTimeSteps = this->mod_file.getFile()->getNumTimeSteps();
	int curr_off = mod_offseted_time_index.getOffset();
	mod_offseted_time_index.setOffset( (curr_off + 1) % numTimeSteps);
    }
}

void 
SimpleEnvironment::decrementOffsetSignal(int state)
{
    if(this->mod_file.fileLoaded()){
	int numTimeSteps = this->mod_file.getFile()->getNumTimeSteps();
	int new_off = (mod_offseted_time_index.getOffset() + (numTimeSteps - 1)) % numTimeSteps;
	mod_offseted_time_index.setOffset(new_off);
    }
}

void 
SimpleEnvironment::toggleHabitatTC(int state)
{
    mod_in_map_tag_cloud_renderer.setVisible((state % 2) == 1);
    // if (state % 2 == 0)
    // 	// set visible
    // int new_off = mod_time_index.getInt() - 1;
    // if (new_off < 0)
    // 	new_off = 52 - 1;
    // mod_time_index.setInt(new_off);
}

void 
SimpleEnvironment::toggleInterpolate(int state){
    this->btn_interpolate->toggle = state;
    this->btn_interpolate->state = state;    
    mod_interpolation_mode.setInt(state);
}

void 
SimpleEnvironment::toggleStencil(int state){
    this->btn_stencil->toggle = state;
    this->btn_stencil->state = state;
    mod_stencil_flag.setInt(state);
}


void 
SimpleEnvironment::rectangleSignal(rectanglelayout::Rectangle rect)
{
    screen_window.init(rect.x, rect.y, rect.w, rect.h);
}

void 
SimpleEnvironment::installAt(rectanglelayout::Node *area)
{
    if (!area->isLeaf())
    {
	return;
    }

    rectanglelayout::RectangleLayout* rs = area->getRectangleLayout();

    // subdivide lead into multiple regions
    rectanglelayout::Grid* root    = rs->subdivide(area->asLeaf(),1,2);
    root->setYStop(1,0.81);

    // top 
    rectanglelayout::Node* top         = rs->getNode(root,0,1);
    rectanglelayout::Grid* top_grid    = rs->subdivide(top->asLeaf(),2,1);
    top_grid->setXStop(1,0.5);
    rectanglelayout::Node* top_left  = rs->getNode(top_grid,0,0);
    rectanglelayout::Node* top_right = rs->getNode(top_grid,0,1);

    // down
    rectanglelayout::Node* bottom      = rs->getNode(root,0,0);
    
    // left & right
    rectanglelayout::Grid* bottom_grid  = rs->subdivide(bottom->asLeaf(),1,2);
    rectanglelayout::Node* bottom_bottom  = rs->getNode(bottom_grid,0,0);
    rectanglelayout::Node* bottom_top = rs->getNode(bottom_grid,0,1);
    
    // bottom side
    rectanglelayout::Grid* bottom_bottom_grid   = rs->subdivide(bottom_bottom->asLeaf(),1,2);
    rectanglelayout::Node* bottom_bottom_top    = rs->getNode(bottom_bottom_grid,0,1);
    rectanglelayout::Node* bottom_bottom_bottom = rs->getNode(bottom_bottom_grid,0,0);

    // top side
    rectanglelayout::Grid* bottom_top_grid    = rs->subdivide(bottom_top->asLeaf(),1,2);
    rectanglelayout::Node* bottom_top_top     = rs->getNode(bottom_top_grid,0,1);
    rectanglelayout::Node* bottom_top_bottom  = rs->getNode(bottom_top_grid,0,0);
    bottom_top_grid->setYStop(1,0.1);


    // photo pael
    mod_photo_window.connectToRectangleSignal(top_left);

    // infor panel
    mod_info_window.connectToRectangleSignal(top_right);

    // map and colormap
    mod_map_window.connectToRectangleSignal(bottom_top_top);
    zoom_and_pan_controller.connectToRectangleSignal(bottom_top_top);
    selection_controller->connectToRectangleSignal(bottom_top_top);

    //
    mod_colormap_window.connectToRectangleSignal(bottom_top_bottom);
    
    //
    btn_offset_plus->connectToRectangleSignal(bottom_top_top);
    btn_offset_minus->connectToRectangleSignal(bottom_top_top);
    btn_habitat_on_off->connectToRectangleSignal(bottom_top_top);
    btn_search->connectToRectangleSignal(bottom_top_top);
    btn_interpolate->connectToRectangleSignal(bottom_top_top);
    btn_stencil->connectToRectangleSignal(bottom_top_top);

    // map and colormap
    mod_plot_window.connectToRectangleSignal(bottom_bottom_top);
    mod_plot_controller.connectToRectangleSignal(bottom_bottom_top);

    // TODO: tag cloud alone
    mod_tag_cloud_window.connectToRectangleSignal(bottom_bottom_bottom);

    // connect the environment with the big area
    connectToRectangleSignal(root);
}

bool    
SimpleEnvironment::event_mouse_drop(eventsystem::State *event_state)
{

    eventsystem::PickableObject *po = event_state->picked_object;
    ListViewItem *lvi = dynamic_cast<ListViewItem*>(po);

    if (lvi == NULL)
    {
	return false;
    }


    bvdata::File *f;
    try {
	f = boost::any_cast<bvdata::File*>(lvi->getCustomObject());
    }
    catch (boost::bad_any_cast)
    {
	return false;
    }

    if (f == NULL)
	return false;

    mod_file.setFile(f);

    f->load();

    {//load photo texture
	this->mod_photo_renderer.load(f->thumb_photo_file);	
    }


    int numTimeSteps = f->getNumTimeSteps();
    mod_offseted_time_index.setOrder(numTimeSteps);


    string expression = "map";//f->shortname+"$p[[t]]";
    mod_expression.setString(expression);

    mod_integral_curve.setExpression(
	"bv.curve("+f->shortname+"$p,xmin,xmax,ymin,ymax)");

    {//set predictors mask
	vector<string> predictorsTags;
	vector<int>    mask;
	int numberOfPredictors = f->getNumPredictors();

	for(int i = 0 ; i < numberOfPredictors ; i++){
	    predictorsTags.push_back(f->getPredictor(i).tag);
	    mask.push_back(1);
	}
	this->mod_tc_mask.init(mask);
    }
	
    {
	//update the number of x_samples and y_samples 
	mod_integral_curve.setXSamples(f->xcells);
	mod_integral_curve.setYSamples(f->ycells);
	
	int numberOfCurves = this->selection_repository->getNumberOfSelections();//mod_integral_curves.size();
	
	for(int i = 0 ; i < numberOfCurves ; i++){
	    Selection* s = this->selection_repository->getSelection(i);
	    CurveData *cd = map_selection_to_curve[s];//mod_integral_curves.at(i);
	    cd->setXSamples(f->xcells);
	    cd->setYSamples(f->ycells);
	    
	    cd->setExpression(
		"bv.curve(" + f->shortname + "$p,xmin,xmax,ymin,ymax)");
	}

    }



    //cout << "Max Value = " << scale << endl;
    //set plot renderer max value
    bvdata::Repository *repo = Global::instance()->getRepository();

    double scale = repo->getCurveMaximum(f, "p",  1,  f->xcells, 1,  f->ycells);

    ViewRect v = mod_plot_renderer.getWorldWindow();
	
    v.x = -1;
    v.w = 370;

    // v.y = (-0.08)*scale;
    // v.h = scale * 1.1; 

    v.y = ColorConstants::NORMALIZE_CURVES_MIN_PLOT_Y;
    v.h = ColorConstants::NORMALIZE_CURVES_MAX_PLOT_Y - ColorConstants::NORMALIZE_CURVES_MIN_PLOT_Y;

    mod_plot_renderer.setWorldWindow(v);
    
    return true;
}

void         
SimpleEnvironment::render()
{

    RenderSystem *renderSystem = Global::instance()->getRenderSystem();

    // map bg
    {
	ViewRect map_window = mod_map_window.getScreenWindow();
	renderSystem->startViewport(map_window.x,map_window.y,
				    map_window.w,map_window.h);
	renderSystem->setColor(ColorConstants::MAP_BG);
	renderSystem->drawQuad(map_window.x,map_window.y,map_window.w,map_window.h);
	renderSystem->endViewport();
    }
    // tc bg
    {
	ViewRect tag_cloud_window = mod_tag_cloud_window.getScreenWindow();
	renderSystem->startViewport(tag_cloud_window.x,tag_cloud_window.y,
				    tag_cloud_window.w,tag_cloud_window.h);
	renderSystem->setColor(ColorConstants::PLOT_BG);
	renderSystem->drawQuad(tag_cloud_window.x,tag_cloud_window.y,
			       tag_cloud_window.w,tag_cloud_window.h);
	renderSystem->endViewport();
    }

    mod_scalar_field_renderer.outdated = true;
    mod_scalar_field_renderer.run();

    mod_map_borders_renderer.outdated = true;
    mod_map_borders_renderer.run();

    mod_map_selections_renderer.outdated = true;
    mod_map_selections_renderer.run();

    mod_colormap_renderer.outdated = true;
    mod_colormap_renderer.run();

    mod_plot_renderer.outdated = true;
    mod_plot_renderer.run();

    mod_tag_cloud_renderer.outdated = true;
    mod_tag_cloud_renderer.run();

    mod_photo_renderer.outdated = true;
    mod_photo_renderer.run();


    // render tag cloud selection on map
    // tag_cloud_infrastructure->render();

    mod_in_map_tag_cloud_renderer.outdated = true;
    mod_in_map_tag_cloud_renderer.run();

    mod_map_labels.outdated = true;
    mod_map_labels.run();

    btn_offset_plus->render();
    btn_offset_minus->render();
    btn_habitat_on_off->render();
    btn_search->render();
    btn_interpolate->render();
    btn_stencil->render();

    // mod_title_renderer.outdated = true;
    // mod_title_renderer.run();

    mod_info_renderer.outdated = true;
    mod_info_renderer.run();

}

bool 
SimpleEnvironment::event_mouse_contains(int x, int y)
{
    return screen_window.contains(x,y);
}

bool 
SimpleEnvironment::event_key_press(eventsystem::State *event_state) 
{
    //this->update_dataset(NULL);
    QString st;
    bool ok;
    int state;

    switch(event_state->key)
    {
    // case (eventsystem::Key_Left):

    // 	mod_time_index.setInt((mod_time_index.getInt() - 1) % 52);
    // 	return true;

    // case (eventsystem::Key_Right):

    // 	mod_time_index.setInt((mod_time_index.getInt() + 1) % 52);
    // 	return true;
    // case eventsystem::Key_Tab:
    // 	mod_colormap_controllers[0]->another_colormap(+1);
    // 	break;
    case eventsystem::Key_M:
	mod_map_borders_mode.setInt(
	    (mod_map_borders_mode.getInt() + 1) % 3);
	break;
    case eventsystem::Key_I:
	state = (mod_interpolation_mode.getInt() + 1) % 2;
	this->toggleInterpolate(state);
	break;
    case eventsystem::Key_S:
	state = (mod_stencil_flag.getInt() + 1) % 2;
	this->toggleStencil(state);
	break;
    // case eventsystem::Key_W:
    // 	write_wf(&mod_time_index, "bvwf.py");
    // 	break;
    case eventsystem::Key_Backspace:	    
	st = QInputDialog::getText(NULL,//this->parent, 
				   "Expression",
				   "Value", 
				   QLineEdit::Normal,
				   mod_expression.getString().c_str(), 
				   &ok);
	if (ok && !st.isEmpty())
	{
	    mod_expression.setString(st.toStdString());
	}
	break;
    default:
	return false;
    }
    return false;
}

void 
SimpleEnvironment::receiveNewSelectionSignal(Selection *sel)
{

//    if(this->mod_file.fileLoaded()){
        { // selection drawing
	    new Connection(sel->getOutputPort("selection"),
			   mod_map_selections_renderer.getInputPort("selections"));
	    mod_map_selections_renderer.outdated = true;
	} // selection drawing
	
	{ // curve into plot

	    bvdata::File* f = this->mod_file.getFile();
	    CurveData *curveData;
	    
	    if(f != NULL){
		cout << "XSamples " << f->xcells << " YSamples " << f->ycells << endl;
		curveData = new CurveData(f->xcells, f->ycells);
	    }
	    else{
		curveData = new CurveData();
	    }

	    new Connection(sel->getOutputPort("selection"),
			   curveData->getInputPort("selection"));
	    
	    new Connection(this->mod_file.getOutputPort("file"),
			   curveData->getInputPort("file"));



	    if (mod_file.getFile() != NULL)
	    {
		curveData->setExpression(
		    "bv.curve(" + mod_file.getFile()->shortname + "$p,xmin,xmax,ymin,ymax)");
	    }

	    new Connection(curveData->getOutputPort("curve"),
			   mod_plot_renderer.getInputPort("curves"));
    
	    // add sel->curveData into map
	    map_selection_to_curve[sel] = curveData;

	} // curve into plot
	

	{ // tag cloud drawing

	    TagCloudData *tgd = new TagCloudData();

	    new Connection(sel->getOutputPort("selection"),
			   tgd->getInputPort("selection"));
	    
	    new Connection(mod_offseted_time_index.getOutputPort("output_int"),
			   tgd->getInputPort("time_index"));
	
	    new Connection(mod_file.getOutputPort("file"),
			   tgd->getInputPort("species_file"));
	
	    new Connection(mod_world_window->getOutputPort("world_window"),
			   tgd->getInputPort("world_window"));
	
	    new Connection(mod_map_window.getOutputPort("screen_window"),
			   tgd->getInputPort("screen_window"));
	
	    new Connection(tgd->getOutputPort("tag_cloud"),
			   mod_in_map_tag_cloud_renderer.getInputPort("tag_clouds"));

	    new Connection(mod_tc_mask.getOutputPort("values"),
			   tgd->getInputPort("mask"));

	    map_selection_to_tag_cloud_data[sel] = tgd;

	} // tag cloud drawing
	//   }
}

void 
SimpleEnvironment::receiveDeleteSelectionSignal(Selection *sel)
{
    mod_map_selections_renderer.deleteAllConnectionsFrom(sel);

    if (map_selection_to_curve.count(sel) > 0)
    {
	CurveData *c = map_selection_to_curve[sel];
	mod_plot_renderer.deleteAllConnectionsFromOrTo(c);
	c->deleteAllConnectionsFromOrTo(sel);
	this->mod_file.deleteAllConnectionsFromOrTo(c);
	map_selection_to_curve.erase(sel);
	delete c;
    }

    // tag cloud drawing
    if (map_selection_to_tag_cloud_data.count(sel) > 0)
    {

	TagCloudData* c = map_selection_to_tag_cloud_data[sel];

	mod_in_map_tag_cloud_renderer.deleteAllConnectionsFromOrTo(c);
	mod_world_window->deleteAllConnectionsFromOrTo(c);
	mod_file.deleteAllConnectionsFromOrTo(c);
	mod_map_window.deleteAllConnectionsFromOrTo(c);
	mod_offseted_time_index.deleteAllConnectionsFromOrTo(c);
	mod_tc_mask.deleteAllConnectionsFromOrTo(c);

	c->deleteAllConnectionsFrom(sel);

	map_selection_to_tag_cloud_data.erase(sel);

	delete c;

    } // tag cloud drawing

}


void 
SimpleEnvironment::search(int state)
{
#ifdef DEBUG_SIMPLEENVIRONMENT
    cout << "SimpleEnvironment::search(int state)" << endl;
#endif

    string x="";
    bool   ok;
    QString st = QInputDialog::getText(NULL,//this->parent, 
				       "Search (e.g. Florida)",
				       "Value", 
				       QLineEdit::Normal,
				       x.c_str(), 
				       &ok);
    if (ok && !st.isEmpty())
    {
	float x[4];
	Global::instance()->keyword_bounds(st.toStdString(), x);
	ViewRect vr(x[0],x[1],x[2],x[3]);
	mod_world_window->setWorldWindow(vr);
    }
}
