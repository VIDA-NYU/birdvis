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

#ifndef TAG_CLOUD_MODULES_H
#define TAG_CLOUD_MODULES_H

#include "ViewRect.hpp"
#include "Dataflow.hpp"
#include "BasicModules.hpp"

using namespace dataflow;

//namespace tagCloud { 

/*************************
 * TagCloudSpecification *
 *************************/

class TagCloudSpecification
{
public:
    int key;
    ViewRect window;
    vector<string> tags;
    vector<double> sizes;

    TagCloudSpecification();
    
    void addTag(string tag, double size);
    
    /*!
     * Returns wheter could find a non-empty range of values
     */
    bool range(double &min_size, double &max_size);
    
}; // end class TagCloudSpecification

class TagCloudRenderer;

/****************
 * TagCloudData *
*****************/

class TagCloudData: 
    public Module {
    
private:    
    ViewRect           screen_window;
    
public:
    TagCloudData();
    void process();
}; 


/**********************
 * SimpleTagCloudData *
 *********************/

class SimpleTagCloudData: 
    public Module {
    
private:    
    ViewRect           screen_window;
    
public:
    SimpleTagCloudData();
    void process();
}; 


/********************
 * TagCloudRenderer *
*********************/

class TagCloudRenderer: 
    public Module {
    
private:

    bool visible;

public:

    static const double min_scale = 1.0;
    static const double max_scale = 4.0;

public:

    TagCloudRenderer();

public:

    bool isVisible();
    void setVisible(bool visible);
	

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
    ViewRect positionTags(vector<TagCloudItem*> *items, float W, float hsep, float vsep);
    ViewRect computeTagCloud(vector<TagCloudItem*> *items, double ar_target);
    void render(TagCloudSpecification *tcs, 
		float left_margin,
		float right_margin,
		float bottom_margin, 
		float top_margin);
    void process();
    
}; // end of class TagCloudRenderer

//} 

#endif
