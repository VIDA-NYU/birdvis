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

#include "TagCloudModules.hpp"
#include "BasicModules.hpp"
#include "Global.hpp"
#include "DataflowTypes.hpp"

//#include "datatypes.hpp"
//#include "Vec2.hpp"


#define xDEBUG_TAGCLOUDRENDERER
#define xDEBUG_TAGCLOUDDATA

using namespace bvdata;

/*************************
 * TagCloudSpecification *
 *************************/

TagCloudSpecification::TagCloudSpecification() 
{}
    
void 
TagCloudSpecification::addTag(string tag, double size)
{
    this->tags.push_back(tag);
    this->sizes.push_back(size);
}
    
bool 
TagCloudSpecification::range(double &min_size, double &max_size)
{
    bool result = false;
	
    double a = 0;
    double b = 0;
    int i;
    for (i=0;i<sizes.size();i++)
    {
	double s = sizes.at(i);
	    
	if (!result)
	{
	    result = true;
	    a = s;
	    b = s;
	}
	else {
	    if (s < a)
		a = s;
	    if (s > b)
		b = s;
	}
    }
	
    if (result)
    {
	min_size = a;
	max_size = b;
    }
    return result;
} // end range
    
/****************
 * TagCloudData *
*****************/

TagCloudData::TagCloudData():
    Module("TagCloudData")
{
    this->addInputPort("selection");
    this->addInputPort("time_index");
    this->addInputPort("species_file");
    this->addInputPort("mask");
    
    this->addInputPort("world_window");
    this->addInputPort("screen_window");

    this->addOutputPort("tag_cloud");
}

void TagCloudData::process(){
#ifdef DEBUG_TAGCLOUDDATA
    cout << "TagCloudData::process()" << endl;
#endif

    //
    Repository* repo = Global::instance()->getRepository();

    typedef data::Single<Selection *> SingleSelectionPtr;

    //check if predictors are available
    bvdata::File* speciesFile;
    
    if (!data::Single<bvdata::File*>::getInputValue(this,"species_file",speciesFile))
    {
#ifdef DEBUG_TAGCLOUDDATA
	cout << "   could not retrieve species file" << endl;
#endif
	getOutputPort("tag_cloud")->setData(NULL,true);
	return;
    }

#ifdef DEBUG_TAGCLOUDDATA
    cout << "   file = " << speciesFile << endl;
#endif

    if (speciesFile == NULL)
    {
#ifdef DEBUG_TAGCLOUDDATA
	cout << "   no input file" << endl;
#endif
	getOutputPort("tag_cloud")->setData(NULL,true);
	return;
    }

    //get mask

#ifdef DEBUG_TAGCLOUDDATA
	cout << "   about to get the mask" << endl;
#endif

    vector<int> mask;
    
    if (!data::Single< vector<int> >::getInputValue(this,"mask",mask))
    {
#ifdef DEBUG_TAGCLOUDDATA
	cout << "   could not retrieve mask" << endl;
#endif
	getOutputPort("tag_cloud")->setData(NULL,true);
	return;
    }

#ifdef DEBUG_TAGCLOUDDATA
    cout << "Mask ok..." << endl;
#endif

    //selection window
    int x_samples = speciesFile->predictors_xcells;//250;
    int y_samples = speciesFile->predictors_ycells;//125;
    bool integrate_all = false;

#ifdef DEBUG_TAGCLOUDDATA
    cout << "x_samples " << x_samples << endl;
    cout << "y_samples " << y_samples << endl;
#endif

    Selection *selection = NULL;
    if (!SingleSelectionPtr::getInputValue(this,"selection",selection))
	integrate_all = true;      

    ViewRect selectionRectangle = selection->getSelectionWindow();

    // time_index
    int time_index;
    if (!data::SingleInt::getInputValue(this,"time_index",time_index))
    {
	#ifdef DEBUG_TAG_CLOUD_DATA
	cout << "   could not retrieve time_index" << endl;
	#endif

	return;
    }

#ifdef DEBUG_TAGCLOUDDATA
    cout << "Time Index ok..." << endl;
#endif

    if(!speciesFile->hasPredictors()){
	//set output port
	getOutputPort("tag_cloud")->setData
	    (NULL,true);

	return;
    }
    
    int xMin, xMax, yMin, yMax;

    ViewRect world_window = speciesFile->getPredictorsQuad();//getDefaultLonLatQuad();

    if (integrate_all)
    {
	xMin = 0; yMin = 0;
	xMax = x_samples-1; yMax = y_samples-1;
    }
    else {
	xMin = ((selectionRectangle.x - world_window.x)/(world_window.w)) * x_samples;
	if(xMin  < 0) xMin = 0;
	if(xMin  >= x_samples) xMin = x_samples - 1;
	
	xMax = ((selectionRectangle.x + 
		 selectionRectangle.w - world_window.x)/(world_window.w)) * x_samples;
	if(xMax  < 0) xMax = 0;
	if(xMax  >= x_samples) xMax = x_samples - 1;

	yMin = ((selectionRectangle.y - world_window.y)/(world_window.h)) * y_samples;
	if(yMin  < 0) yMin = 0;
	if(yMin  >= y_samples) yMin = y_samples - 1;
    
	yMax = ((selectionRectangle.y + 
		 selectionRectangle.h - world_window.y)/(world_window.h)) * y_samples;
	if(yMax  < 0) yMax = 0;
	if(yMax  >= y_samples) yMax = y_samples - 1;    
    }          


#ifdef DEBUG_TAGCLOUDDATA
    cout << "xMin " << xMin << endl;
    cout << "xMax " << xMax << endl;
    cout << "yMin " << yMin << endl;    
    cout << "yMax " << yMax << endl;
#endif


    // compute new region
    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window)){
#ifdef DEBUG_TAGCLOUDDATA
    cout << "Could not retrieve screen window" << endl;
#endif
	return;
    }
#ifdef DEBUG_TAGCLOUDDATA
    cout << "Screen window ok..." << endl;
#endif
    // cout << "   screen window.....OK" << endl;

    //ViewRect world_window;
    if (!data::SingleViewRect::getInputValue(this,"world_window",world_window)){
#ifdef DEBUG_TAGCLOUDDATA
	cout << "Could not retrieve worl windowMask" << endl;
#endif
	return;
    }

#ifdef DEBUG_TAGCLOUDDATA
    cout << "World Window ok..." << endl;
#endif
    // cout << "   world window.....OK" << endl;

    //find out sum range in array        
    ScreenWorldTransform swt(screen_window, world_window);
    ViewRect selectionRectangle_screen_window = swt.world2screen(selectionRectangle);


    // the tag cloud should be drawin into this rectangle
    TagCloudSpecification *tcSpec = new TagCloudSpecification();
    tcSpec->window = selectionRectangle_screen_window;


#ifdef DEBUG_TAG_CLOUD_DATA
    cout << "render tag cloud at" << t->window.str() << endl;
#endif

    //Query R
    // int numberOfPredictors = speciesFile->getNumPredictors();
//     string predictorsNames[] = {
// 		"BarrenLand"	  ,
// 		"CultivatedCrops" ,
// 		"Developed_High"  ,
// 		"Developed_Low"	  ,
// 		"Developed_Med"   ,
// 	        "Developed_Open"  ,
// 		"Elevation"       ,
// 		"EmergentWetlands",
// 		"Forest_Deciduous",
// 		"Forest_Evergreen",
// 		"Forest_Mixed"	  ,
// 		"Grassland" 	  ,
// 		"Ice_Snow"	  ,
// 	        "OpenWater"       ,
// 		"Pasture"         ,
// 		"Shrub_scrub"	  ,
// 		"WoodyWetlands"	  };

//     string predictorsFancyNames[] = {
// 		"Barren Land"	   ,
// 		"Cultivated Crops" ,
// 		"Developed-High"   ,
// 		"Developed-Low"	   ,
// 		"Developed-Med"    ,
// 	        "Developed-Open"   ,
// 		"Elevation"        ,
// 		"Emergent Wetlands",
// 		"Deciduous Forest" ,
// 		"Evergreen Forest" ,
// 		"Mixed Forest"	   ,
// 		"Grassland" 	   ,
// 		"Ice-Snow"	   ,
// 	        "Open Water"       ,
// 		"Pasture"          ,
// 		"Shrub-scrub"	   ,
// 		"Woody Wetlands"   };

#ifdef DEBUG_TAG_CLOUD_DATA
    cout << "Building TC..." << endl;
#endif
    int numberOfActiveTags = 0;
    for(int i = 0 ; i < mask.size() ; i++){
	numberOfActiveTags += (mask.at(i));
    }

#ifdef DEBUG_TAG_CLOUD_DATA
    cout << "Building TC1..." << endl;
#endif

    int indices[numberOfActiveTags];
    int count = 0;
    for(int i = 0 ; i < mask.size() ; i++){
	if(mask[i]){
	    indices[count] = i;
	    count++;
	}
    }

#ifdef DEBUG_TAG_CLOUD_DATA
    cout << "Building TC2..." << endl;
#endif
    //count = 0;
    //int numberOfPredictors = speciesFile->getNumPredictors();

    vector<string> predictorsNames;//[numberOfActiveTags];//numberOfActiveTags];
    vector<string> predictorsFancyNames;//[numberOfActiveTags];

    for(int i = 0 ; i < numberOfActiveTags ; i++){       
	    bvdata::Predictor tmp = speciesFile->getPredictor(indices[i]);
	    predictorsNames.push_back(tmp.internal_name);
	    predictorsFancyNames.push_back(tmp.tag);
	    count++;	
    }

#ifdef DEBUG_TAG_CLOUD_DATA
    cout << "Building TC3..." << endl;
#endif

    vector<double> result = repo->getPredictorsCurveValue(speciesFile,predictorsNames
							  ,time_index+1, xMin + 1, xMax+1,
							  yMin + 1, yMax + 1);

    if(result.size() == 0){
	getOutputPort("tag_cloud")->setData(NULL,true);
	return;
    }

    vector<double>::iterator it;

    //for(int i = 0 ; i < numberOfPredictors ; i++){
    int i = 0;
    for (it=result.begin() ; it<result.end() ; it++){
	double value = *it;
    
	//add to spec
	tcSpec->addTag(predictorsFancyNames[i],value);
	i++;
    }

    //set output port
    getOutputPort("tag_cloud")->setData
	(new data::Single<TagCloudSpecification *>(tcSpec),true);
}

/**********************
 * SimpleTagCloudData *
 **********************/

SimpleTagCloudData::SimpleTagCloudData():
    Module("SimpleTagCloudData")
{
    this->addInputPort("time_index");
    this->addInputPort("species_file");
    this->addInputPort("screen_window");

    this->addOutputPort("tag_cloud");
}

void 
SimpleTagCloudData::process(){
#ifdef DEBUG_TAGCLOUDDATA
    cout << "TagCloudData::process()" << endl;
#endif

    //
    Repository* repo = Global::instance()->getRepository();

    typedef data::Single<Selection *> SingleSelectionPtr;

    //check if predictors are available
    bvdata::File* speciesFile;
    
    if (!data::Single<bvdata::File*>::getInputValue(this,"species_file",speciesFile))
    {
#ifdef DEBUG_TAGCLOUDDATA
	cout << "   could not retrieve species file" << endl;
#endif
	getOutputPort("tag_cloud")->setData(NULL,true);
	return;
    }

#ifdef DEBUG_TAGCLOUDDATA
    cout << "   file = " << speciesFile << endl;
#endif

    if (speciesFile == NULL)
    {
#ifdef DEBUG_TAGCLOUDDATA
	cout << "   no input file" << endl;
#endif
	getOutputPort("tag_cloud")->setData(NULL,true);
	return;
    }

    //get mask
//     vector<int> mask;
    
//     if (!data::Single< vector<int> >::getInputValue(this,"mask",mask))
//     {
// #ifdef DEBUG_TAGCLOUDDATA
// 	cout << "   could not retrieve species mask" << endl;
// #endif
// 	getOutputPort("tag_cloud")->setData(NULL,true);
// 	return;
//     }

// #ifdef DEBUG_TAGCLOUDDATA
//     cout << "Mask ok..." << endl;
// #endif

    //selection window
    int x_samples = speciesFile->predictors_xcells;//250;
    int y_samples = speciesFile->predictors_ycells;//125;
    bool integrate_all = false;

//     Selection *selection = NULL;
//     if (!SingleSelectionPtr::getInputValue(this,"selection",selection))
// 	integrate_all = true;      
    //ViewRect selectionRectangle = selection->getSelectionWindow();

    // time_index
    int time_index;
    if (!data::SingleInt::getInputValue(this,"time_index",time_index))
    {
	#ifdef DEBUG_TAG_CLOUD_DATA
	cout << "   could not retrieve time_index" << endl;
	#endif

	return;
    }

#ifdef DEBUG_TAGCLOUDDATA
    cout << "Time Index ok..." << endl;
#endif

    if(!speciesFile->hasPredictors()){
	//set output port
	getOutputPort("tag_cloud")->setData(NULL,true);

	return;
    }
    
    int xMin, xMax, yMin, yMax;

    //ViewRect world_window = speciesFile->getPredictorsQuad();//getDefaultLonLatQuad();

    xMin = 0; yMin = 0;
    xMax = x_samples-1; yMax = y_samples-1;

    //Compute new region
    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"screen_window",screen_window))
	return;

    // the tag cloud should be drawin into this rectangle
    TagCloudSpecification *tcSpec = new TagCloudSpecification();
    tcSpec->window = screen_window;


#ifdef DEBUG_TAG_CLOUD_DATA
    cout << "render tag cloud at" << t->window.str() << endl;
#endif

    int numberOfPredictors = speciesFile->getNumPredictors();
//     for(int i = 0 ; i < numberOfPredictors ; i++){
// 	bvdata::Predictor tmp = speciesFile->getPredictor(i);
// 	predictorsNames[i] = tmp.internal_name;
// 	predictorsFancyNames[i] = tmp.tag;
//     }

    //count = 0;
    //int numberOfPredictors = speciesFile->getNumPredictors();

    vector<string> predictorsNames;//[numberOfActiveTags];//numberOfActiveTags];
    vector<string> predictorsFancyNames;//[numberOfActiveTags];

    for(int i = 0 ; i < numberOfPredictors ; i++){       
	    bvdata::Predictor tmp = speciesFile->getPredictor(i);
	    predictorsNames.push_back(tmp.internal_name);
	    predictorsFancyNames.push_back(tmp.tag);
    }

#ifdef DEBUG_TAG_CLOUD_DATA
    cout << "Building TC3..." << endl;
#endif

    vector<double> result = repo->getPredictorsCurveValue(speciesFile,predictorsNames
							  ,time_index+1, xMin + 1, xMax+1,
							  yMin + 1, yMax + 1);

    if(result.size() == 0){
	getOutputPort("tag_cloud")->setData(NULL,true);
	return;
    }

    vector<double>::iterator it;

    //for(int i = 0 ; i < numberOfPredictors ; i++){
    int i = 0;
    for (it=result.begin() ; it<result.end() ; it++){
	double value = *it;
    
	//add to spec
	tcSpec->addTag(predictorsFancyNames[i],value);
	i++;
    }

    //set output port
    getOutputPort("tag_cloud")->setData
	(new data::Single<TagCloudSpecification *>(tcSpec),true);
}


/********************
 * TagCloudItem *
*********************/

TagCloudRenderer::TagCloudItem::TagCloudItem(string text, double relativeSize)
{
    this->text = text;
    this->relativeSize = relativeSize;
    this->x = 0;
    this->y = 0;
    this->fontSize = 1.0;
}

ViewRect 
TagCloudRenderer::TagCloudItem::rect()
{
    return ViewRect(x + base_x, y + base_y, base_w, base_h);
}

/********
 * Line *
*********/

TagCloudRenderer::Line::Line() 
{
    index = 0;
    len   = 0;
    bounds.init(0,0,0,0);
}

TagCloudRenderer::Line::Line(int index, int len, ViewRect bounds) 
{
    this->index = index;
    this->len = len;
    this->bounds = bounds;
}

void 
TagCloudRenderer::Line::init(int index, int len, ViewRect bounds) {
    this->index = index;
    this->len = len;
    this->bounds = bounds;
}

/********************
 * TagCloudRenderer *
*********************/


TagCloudRenderer::TagCloudRenderer(): 
    Module("TagCloudRenderer"),
    visible(true)
{
    this->addInputPort("clipping_region");
    this->addInputPort("tag_clouds");
    this->addInputPort("black_text");
    this->addInputPort("title");
    this->addInputPort("onoff");
}

void 
TagCloudRenderer::setVisible(bool v)
{
    this->visible = v;
    this->outdated = true;
}

bool 
TagCloudRenderer::isVisible()
{
    return this->visible;
}

ViewRect 
TagCloudRenderer::positionTags(
    vector<TagCloudItem*> *items, float W, float hsep, float vsep)
{

    vector<Line> lines;


    // loop on items trying to position them
    TagCloudRenderer::Line line;
    ViewRect zero_rect(0,0,0,0);
    line.init(0,0,zero_rect);
    
    float x,y;
    x = 0; y = 0;
    int i = 0;
    while (i < items->size())
    {
	TagCloudItem *item = items->at(i);
	if (x + item->base_x + item->base_w <= W)
	{
	    item->x = x;
	    item->y = 0;
	    line.bounds.resizeToIncludeRect(item->rect());
	    x += item->base_x + item->base_w + hsep;
	    // cout << "   set x of item " << i << " line len incremented" << endl;
	    line.len++;
	    i++;
	}
	else if (x == 0) { // doesnt fit!
	    return zero_rect;
	    exit(0);
	}
	else {
	    // cout << "   line add:    i0 " << line.index << " len " << line.len << " i1 " << (line.index + line.len - 1) << " view rect " << line.bounds.str() << endl;
	    lines.push_back(line);
	    line.init(i,0,zero_rect);
	    x = 0;
	}
    }
    if (line.len > 0)
	lines.push_back(line);

    for (int i=0;i<lines.size();i++)
    {
	line = lines.at(i);
	// cout << "line " << i << " i0 " << line.index << " len " << line.len << " i1 " << (line.index + line.len - 1) << " view rect " << line.bounds.str() << endl;
    }

    y = 0;
    ViewRect result(0,0,0,0);
    line = lines.at(0);
    for (int j=0;j<line.len;j++) 
	result.resizeToIncludeRect(items->at(line.index + j)->rect());

    for (int i=1;i<lines.size();i++)
    {
	line = lines.at(i);
	double y_offset = lines.at(i-1).bounds.minY() + 
	    lines.at(i).bounds.maxY() + hsep;
	for (int j=0;j<line.len;j++) {
	    items->at(line.index + j)->y = y - y_offset;
	    result.resizeToIncludeRect(items->at(line.index + j)->rect());
	}
	y -= y_offset;
    }

    return result;

}

ViewRect 
TagCloudRenderer::computeTagCloud(vector<TagCloudItem*> *items, double ar_target)
{
    ViewRect result(0,0,0,0);
    if (items->size() == 0)
	return result;
    
    //
    rendersystem::RenderSystem *renderSystem = Global::instance()->getRenderSystem();
    
    // font description
    double oneline_min_x;
    double oneline_min_y;
    double oneline_max_x;
    double oneline_max_y;
    
    double max_item_w;
    
    // measure base size of tags and keep the largest width of a word,
    // the sum of the widths and the hight
    
    ViewRect rect_one_line(0,0,0,0);
    
    float largestTagWidth = 0;
    
    float x = 0;
    float y = 0;
    for (int i=0;i<items->size();i++)
    {
		
	TagCloudItem *item = items->at(i);

	float advance;

	ViewRect base = renderSystem->textBoundingBox(
	    item->text.c_str(),
	    item->relativeSize,0.,0.,
	    &advance);

	item->x = x;
	item->y = y;
	item->base_x  = base.x;
	item->base_y  = base.y;
	item->base_w  = base.w;
	item->base_h  = base.h;
	item->base_advance = advance;

	if (largestTagWidth < base.w + base.x)
	    largestTagWidth = base.x + base.w;

	rect_one_line.resizeToIncludeRect(item->rect());

	x += item->base_advance;

    } // end measure tags

    float hsep = 2.0 * min_scale;
    float vsep = min_scale;

    // compute aspect ratio
    float W_left = largestTagWidth;
    float W_right = rect_one_line.w + hsep * (items->size() - 1);
	    
    double   ar_left, ar_right;
    ViewRect vr;

    // compute ar_left
    vr = this->positionTags(items,W_left,hsep,vsep);
    ar_left = W_left / vr.h;

    // compute ar_right
    vr = this->positionTags(items,W_right,hsep,vsep);
    ar_right = W_right / vr.h;

#ifdef DEBUG_SCALARFIELDDATA
    cout << "W_left, ar_left " << W_left << ", " << ar_left << endl;
    cout << "W_right, ar_right " << W_right << ", " << ar_right << endl;
#endif

    //
    if (ar_target <= ar_left)
    {
	vr = this->positionTags(items,W_left,hsep,vsep);
    }
    else if (ar_target >= ar_right)
    {
	vr = this->positionTags(items,W_right,hsep,vsep);
    }
    else { // binary_search
	while (W_right - W_left > 1e-3)
	{
	    double W_mid = (W_left + W_right)/2.0;
	    vr = this->positionTags(items,W_mid,hsep,vsep);
	    double ar_mid = W_mid / vr.h;

#ifdef DEBUG_SCALARFIELDDATA
	    cout << "W_mid, ar_mid, ar_target " << W_mid << ", " << ar_mid << ", " << ar_target << endl;
#endif

	    if (ar_mid < ar_target) {
		W_left = W_mid;
		ar_left = ar_mid;
	    }
	    else if (ar_mid > ar_target) {
		W_right = W_mid;
		ar_right = ar_mid;
	    }
	    else {
		W_left   = W_mid;
		W_right  = W_mid;
		ar_left  = ar_mid;
		ar_right = ar_mid;
	    }
	} // end binary search

    } // end else
		    

    //
    if (ar_target - ar_left < ar_right - ar_target)
    {
#ifdef DEBUG_SCALARFIELDDATA	
	cout << "winner is left: W_left, ar_left, ar_target " << W_left << ", " << ar_left << ", " << ar_target << endl;
#endif
	vr = this->positionTags(items,W_left,hsep,vsep);
    }
    else
    {
	vr = this->positionTags(items,W_right,hsep,vsep);
#ifdef DEBUG_SCALARFIELDDATA
	cout << "winner is right: W_right, ar_right, ar_target " << W_right << ", " << ar_right << ", " << ar_target << endl;
#endif
    }
#ifdef DEBUG_SCALARFIELDDATA
    cout << "view rect " << vr.str() << endl;
#endif

    return vr;

} // end compute tag positions

void
TagCloudRenderer::render(TagCloudSpecification *tcs, 
			 float left_margin,
                         float right_margin,
                         float bottom_margin, 
                         float top_margin)
{
    double min_factor;
    double max_factor;
    bool   has_range = tcs->range(min_factor, max_factor);

    if (!has_range)
	return;

    if (min_factor == max_factor)
	max_factor += 1.0;
	

    // black_text?
    int black_text = 0;
    if (!data::SingleInt::getInputValue(this,"black_text",black_text))
	black_text = 0;

    bool black = black_text > 0;


    vector<TagCloudItem *> *tags = new vector<TagCloudItem *>();
    for (int i=0;i< tcs->tags.size();i++)
    {
	string tag  = tcs->tags.at(i);
	double size = tcs->sizes.at(i);
	double relativeSize = min_scale + (size - min_factor)/
		(max_factor - min_factor) * (max_scale - min_scale);
	TagCloudItem *item = new TagCloudItem(tag,relativeSize);
	tags->push_back(item);
    }

    double x = tcs->window.x + left_margin;
    double y = tcs->window.y + bottom_margin;
    double w = tcs->window.w - left_margin - right_margin;
    double h = tcs->window.h - top_margin - bottom_margin;

#ifdef DEBUG_TAGCLOUDRENDERER
    cout << "   selection window (screen coords) " << tcs->window.str() << endl;
#endif


    double margin_left   = 5;
    double margin_right  = 5;
    double margin_bottom = 5;
    double margin_top    = 5;

    double xx = x + margin_left;
    double yy = y + margin_bottom;
    double ww = w - margin_left - margin_right;
    double hh = h - margin_bottom - margin_top;

    ViewRect tagsBoxBounds = computeTagCloud(tags,ww/hh);

    rendersystem::RenderSystem *renderSystem = Global::instance()->getRenderSystem();

    // assume viewport was already started

    double scale = min(ww/tagsBoxBounds.w,hh/tagsBoxBounds.h);
    double xoff = (ww - scale * tagsBoxBounds.w)/2.0;
    double yoff = (hh - scale * tagsBoxBounds.h)/2.0;


    if (!black) { // white shadow

	int deltax =  1;
	int deltay = -1;


	renderSystem->pushMatrix();
	renderSystem->translate(xx + xoff + deltax,yy + yoff + deltay);
	renderSystem->scale(scale,scale);

	renderSystem->translate(-tagsBoxBounds.x,-tagsBoxBounds.y);
	    
	for (int i=0;i<tags->size();i++)
	{
	    TagCloudItem *item = tags->at(i);

	    double fontSize = 1;

	    // float coef = 1.0 - item->relativeSize/max_scale;
	    // coef = 0.5 * coef;
	    renderSystem->setColor(1,1,1);

	    renderSystem->drawText(item->text.c_str(), 
				   item->relativeSize, 
				   item->x,item->y);
	}

	renderSystem->popMatrix();

    } // white shadow





    renderSystem->pushMatrix();
    renderSystem->translate(xx + xoff,yy + yoff);
    renderSystem->scale(scale,scale);

#ifdef DEBUG_SCALARFIELDDATA
    cout << "scale: " << scale << endl;
#endif

    renderSystem->translate(-tagsBoxBounds.x,-tagsBoxBounds.y);
	    
    for (int i=0;i<tags->size();i++)
    {
	TagCloudItem *item = tags->at(i);

	double fontSize = 1;

#ifdef DEBUG_SCALARFIELDDATA
	cout << "name " << item->text << " relative size " 
	     << item->relativeSize 
	     << "rel_size / max_scale " 
	     << (item->relativeSize/max_scale)
	     << endl;
#endif

	float coef = 1.0 - item->relativeSize/max_scale;

	// if (black)
	//     coef = 1-coef;
	// else
	coef = 0.1 * coef;
       


	renderSystem->setColor(coef,coef,coef);

#ifdef DEBUG_SCALARFIELDDATA
	cout << "drawing item " << item->text << " in " 
	     << item->x * ww / sizeTextBox.x  << " "
	     << item->y * hh / sizeTextBox.y  << endl;
#endif

	renderSystem->drawText(item->text.c_str(), 
			       item->relativeSize, 
			       item->x,item->y);
		
    }

    renderSystem->popMatrix();

}

void 
TagCloudRenderer::process() 
{
    typedef data::Single<TagCloudSpecification* > SingleTagCloudSpecPtr;

    if (!isVisible())
	return;

    // on or off?
    int on;
    if (!data::SingleInt::getInputValue(this,"onoff",on))
	on = 1;
    if (!on)
	return;

#ifdef DEBUG_TAGCLOUDRENDERER
    cout << "TagCloudRenderer::process()" << endl;
#endif

    rendersystem::RenderSystem *renderSystem = Global::instance()->getRenderSystem();

    vector<TagCloudSpecification*> tag_clouds;
    vector<Data* > vec_data = this->getInputPort("tag_clouds")->getAllData();

#ifdef DEBUG_TAGCLOUDRENDERER
    cout << "   Number of Specifications " << vec_data.size() << endl;
#endif

    for (int i=0;i<vec_data.size();i++)
    {
        Data *dd = vec_data[i];
	if (dd == NULL)
	    continue;
	try
	{
	    SingleTagCloudSpecPtr *d = dynamic_cast<SingleTagCloudSpecPtr *>(dd);
	    tag_clouds.push_back(d->value);
	}
	catch (...) {
#ifdef DEBUG_TAGCLOUDRENDERER
	    cout << "   could not read curve" << endl;
#endif
	}

    } // end grabing vec data


#ifdef DEBUG_TAGCLOUDRENDERER
    cout << "   number of tag cloud specs:" << tag_clouds.size() << endl;
#endif

    ViewRect screen_window;
    if (!data::SingleViewRect::getInputValue(this,"clipping_region",screen_window))
	return;

    string title;
    bool has_title = true;
    if (!data::SingleString::getInputValue(this,"title",title))
	has_title = false;

#ifdef DEBUG_TAGCLOUDRENDERER
    cout << "   clipping region: " << screen_window.str() << endl;
#endif

    double x = screen_window.x;
    double y = screen_window.y;
    double w = screen_window.w;
    double h = screen_window.h;

    if (w == 0 || h == 0) return;

    // prepare clipping plane

    float top_margin = 32;
    
    renderSystem->startViewport(x,y,w,h);

    for (int i=0;i<tag_clouds.size();i++)
    {
	TagCloudSpecification* tag_cloud_spec = tag_clouds[i];

	if (!has_title)
	{
	    this->render(tag_cloud_spec,0,0,0,0);
	}
	else 
	{
	    this->render(tag_cloud_spec,10,10,10,top_margin + 10);
	}
    }

    if (has_title)
    {
	
	float font_size = 17;
	renderSystem->setColor(ColorConstants::TITLE_BG);
	renderSystem->drawQuad(x,y+h-top_margin,w,top_margin);
    
	renderSystem->setColor(ColorConstants::TITLE_TEXT);
	renderSystem->drawCenteredText(title.c_str(), 
				       font_size, 
				       x, y + h - top_margin,
				       w,top_margin);
    }

    renderSystem->endViewport();

}
