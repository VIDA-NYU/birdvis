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

#ifndef SPREADSHEET_HPP
#define SPREADSHEET_HPP

#include <vector>

#include "Global.hpp"
#include "EventSystem.hpp"
#include "RectangleLayout.hpp"
#include "RenderSystem.hpp"

#include <boost/any.hpp>

#include "ViewRect.hpp"

#include "BasicModules.hpp"

using namespace rendersystem;

/*************
 * Component *
 *************/

class Component:
    public eventsystem::Node
{
public:
    virtual void     installAt(rectanglelayout::Node *area);
    virtual void     render();
    virtual ViewRect getBoundingBox();
};


/****************
 * ListViewItem *
 ****************/

class ListViewItem:
    public eventsystem::PickableObject,
    public eventsystem::Node
{

private:

    string       label;

    float        fontSize;

    ViewRect     labelBounds;

    float        x, y;

    boost::any   customObject;

public:

    ListViewItem(string label, float font_size);

    ViewRect        getBoundingBox();

    void            render();

    // virtual void    event_render(State *event_state);

    void            setPosition(float x, float y);

    void            setCustomObject(boost::any custom_object);

    boost::any      getCustomObject();

public: // eventsystem::Node

    virtual bool event_mouse_contains(int x, int y);

    virtual eventsystem::PickableObject* event_mouse_pick(
	eventsystem::State *event_state);

public: // eventsystem::Node

    // virtual void event_mouse_drag(eventsystem::State *event_state);

    // virtual void event_mouse_release(eventsystem::State *event_state);
    virtual void event_render(eventsystem::State *event_state);

};

/************
 * ListView *
 ************/

class ListView:
    public Component
{
private:

    std::vector<ListViewItem*>   items; 

    ViewRect                     itemsBoundingBox;

private:

    void                         update();

public:

    ListView();

    void         addItem(ListViewItem *item);

    virtual void render();

    virtual void installAt(rectanglelayout::Node *area);

    virtual ViewRect getBoundingBox();

public:

    virtual bool event_mouse_contains(int x, int y);

};

/**************
 * ScrollPane *
 **************/

class ScrollPane:
    public Component,
    public rectanglelayout::RectangleSlot
{

private:

    Component* component;

    ViewRect   screenWindow;

    float      deltaX, deltaY;

    string     title;

private:

    void updateDeltas();

public:

    ScrollPane();

    void         setComponent(Component *c);

    virtual void render();

    virtual void rectangleSignal(rectanglelayout::Rectangle r);

    virtual ViewRect getBoundingBox();

    virtual void installAt(rectanglelayout::Node *area);

public:

    virtual bool event_mouse_contains(int x, int y);

    virtual bool event_mouse_wheel(eventsystem::State *event_state);

    virtual void event_children_offset(float &offsetX, float &offsetY);

    void setTitle(string title);
    
};




// /************
//  * TextItem *
//  ************/
// class TextItem:
//     public ListView::Item
// {
// private:
//     string          text;
//     float           font_size;
//     AxisAlignedBox  boundingBox;
// public:
//     public TextItem
//     virtual void installAt(rectanglelayout::Node *area);
//     virtual void render();
//     // virtual void event_render(State *event_state);
// };

/***************
 * Spreadsheet *
 ***************/

class Spreadsheet:
    public eventsystem::Node,
    public eventsystem::PickableObject
{
private:

    Component  *env;

    rectanglelayout::RectangleLayout rectangle_layout;
    rectanglelayout::Rectangle       rectangle;

    bool                    changing_cells_dimension;
    rectanglelayout::Handle changing_cells_dimension_handle;


    std::vector<Component* > components;

private:

    void emitRectangleLayoutSignals();

public:

    Spreadsheet();
    void resize(float x, float y, float w, float h);

public:
    
    void installComponent(Component *c, int xcell, int  ycell);
    void installComponent(Component *c, int *cell_path, int depth);
    void installComponent(Component *c, rectanglelayout::Node *area = NULL);
    void render();
    void clear();

private:

    void set_map_bg_color(eventsystem::State *event_state);
    void set_sel_transparency(eventsystem::State *event_state);


public: // pick and drag
    virtual eventsystem::PickableObject* event_mouse_pick(
	eventsystem::State *event_state);

    virtual void event_mouse_drag(eventsystem::State *event_state);

    virtual void event_mouse_release(eventsystem::State *event_state);

    virtual bool event_mouse_contains(int x, int y);

    virtual bool event_mouse_drop(eventsystem::State *event_state);

    virtual bool event_key_press(eventsystem::State *event_state);


    // virtual void    event_mouse_move(State *event_state);
    // virtual bool    event_mouse_drop(State *event_state);
    // virtual bool    event_mouse_wheel(State *event_state);
    // virtual bool    event_key_press(State *event_state);
    // virtual int     event_num_children();
    // virtual void    event_set_parent(Node *parent_node);
    // virtual Node   *event_parent();
    // virtual void    event_mouse_enter(State *event_state);
    // virtual void    event_mouse_exit(State *event_state);
    // virtual string  event_node_name(State *event_state);
    // void    event_add_node(Node *n);
    // Node   *event_child(int index);

}; // end Spreadsheet

#endif
