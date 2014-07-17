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

#include <string>
#include <stack>
#include <iostream>
#include <algorithm>

#include "EventSystem.hpp"

#define xDEBUG_EVENTSYSTEM_MANAGER

namespace eventsystem {

    /*********
     * State *
     *********/

    State::State()
    {
	x = y = pick_x = pick_y = last_x = last_y = 0;
	shift = ctrl = alt = false;
	picked_object = NULL;
	button = NONE;
    }

    /******************
     * PickableObject *
     ******************/

    PickableObject::PickableObject() 
    {}
    
    void 
    PickableObject::event_mouse_drag(State *event_state) 
    {}

    void 
    PickableObject::event_mouse_release(State *event_state) 
    {}

    void 
    PickableObject::event_render(State *event_state) 
    {}

    /********
     * Node *
     ********/

    Node::Node() 
    {}

    bool 
    Node::event_mouse_contains(int x, int y) 
    { 
	return false; 
    }

    void    
    Node::event_mouse_move(State *event_state) 
    {}

    PickableObject*
    Node::event_mouse_pick(State *event_state) 
    { 
	return NULL; 
    }

    bool    
    Node::event_mouse_drop(State *event_state) 
    { 
	return false;
    }

    bool 
    Node::event_mouse_wheel(State *event_state) 
    {
	return false;
    }

    bool    
    Node::event_key_press(State *event_state) 
    { 
	return false; 
    }

    void    
    Node::event_children_offset(float &x, float &y)
    {
	x = 0;
	y = 0;
    }

    int     
    Node::event_num_children() 
    { 
	return this->event_children.size(); 
    }	    

    void    
    Node::event_set_parent(Node *parent_node) 
    { 
	this->event_node_parent = parent_node; 
    }

    Node*
    Node::event_parent() 
    { 
	return this->event_node_parent; 
    }

    void    
    Node::event_mouse_enter(State *event_state) 
    {}

	    
    void    
    Node::event_mouse_exit(State *event_state) 
    {}

    string  
    Node::event_node_name(State *event_state) 
    { 
	return ""; 
    }

    void    
    Node::event_add_node(Node *n) 
    { 
	event_children.push_back(n); n->event_node_parent = this; 
    }

    void    
    Node::event_del_node(Node *n) 
    { 
	vector<Node*>::iterator it;
	it = find(event_children.begin(),event_children.end(),n);
	if (it != event_children.end())
	    event_children.erase(it);
	n->event_node_parent = NULL; 
    }

    Node*
    Node::event_child(int index) 
    { 
	return event_children.at(index); 
    }
    

    /*****************
     * Manager::Root *
     *****************/

    Manager::Root::Root()
    {
	this->event_node_parent = NULL;
    }

    bool  
    Manager::Root::event_mouse_contains(int x, int y) 
    { 
	return true; 
    }

    string 
    Manager::Root::event_node_name(State *event_state) 
    { 
	return "Root"; 
    }

    /********************
     * Manager::DFSItem *
     ********************/

    Manager::DFSItem::DFSItem() 
    {}

    Manager::DFSItem::DFSItem(Node *n, int i, float offsetX, float offsetY) 
    { 
	node = n; 
	index = i; 
	this->offsetX = offsetX;
	this->offsetY = offsetY;
    }

    void 
    Manager::DFSItem::init(Node *n, int i, float offsetX, float offsetY) 
    { 
	node = n; 
	index = i; 
	this->offsetX = offsetX;
	this->offsetY = offsetY;
    }


    Manager::Manager() 
    {
	event_state.mouse_node = &root;
    }

    void 
    Manager::event_render()
    {
	if (event_state.picked_object != NULL)
	{
	    event_state.picked_object->event_render(&event_state);
	}
    }

    void 
    Manager::event_add_node(Node *n)
    {
	root.event_add_node(n);
    }

    Node*
    Manager::hit(int x, int y, float &offX, float &offY) {
	DFSItem u, v;
	DFSItem item(&root,-1, 0, 0);
	stack<DFSItem> stack;
	stack.push(item);
	while (stack.size() > 0)
	{
	    DFSItem u = stack.top();
	    stack.pop();
	    // cout << "=== Pop: " << u.node->event_node_name(&event_state) 
	    // 	 << " index " << u.index << endl;

	    if (u.index == -1)
	    {
		if (u.node->event_mouse_contains(x + u.offsetX, y + u.offsetY))
		{
		    u.index = 0;
		    // cout << "=== Push: " << u.node->event_node_name(&event_state) 
		    // 	 << " index " << u.index << endl;
		    stack.push(u);
		}
	    }
	    else
	    { // node contains x,y see if its children also contains
		if (u.index < u.node->event_num_children())
		{
		    int curr_index = u.index;
		    u.index++;
		    stack.push(u);
		    // cout << "=== Push: " << u.node->event_node_name(&event_state) 
		    // 	 << " index " << u.index << endl;

		    float offset_x, offset_y;
		    u.node->event_children_offset(offset_x, offset_y);
		    // cout << "   -----> " << offset_x << ", " << offset_y << endl;
		    v.init(u.node->event_child(curr_index), -1, 
			   u.offsetX + offset_x,
			   u.offsetY + offset_y);
		    // cout << "=== Push: " << v.node->event_node_name(&event_state) 
		    // 	 << " index " << v.index << endl;

		    stack.push(v);
		}

		// check if 
		else {

		    // cout << "=== return: " << u.node->event_node_name(&event_state)  << endl;
		    offX = u.offsetX;
		    offY = u.offsetY;
		    return u.node;
		}
	    } // check if child at u.index contains x,y otherwise we found!
	} // while stack != NULL

	return NULL;

    } // hit

    void 
    Manager::mouse_move(int x, int y)
    {
	event_state.last_x = event_state.x;
	event_state.last_y = event_state.y;
	event_state.x = x;
	event_state.y = y;

	// check node where mouse is on top
	float offx, offy;
	Node *hit_node = this->hit(x,y,offx, offy);

	event_state.offset_x = offx;
	event_state.offset_y = offy;
	
	// signal exit and enter
	if (hit_node != event_state.mouse_node)
	{

#ifdef DEBUG_EVENTSYSTEM_MANAGER
	    cout << "-----> exit  " << 
		event_state.mouse_node->event_node_name(&event_state) << endl;
	    cout << "-----> enter " << 
		hit_node->event_node_name(&event_state) << endl;
#endif

	    event_state.mouse_node->event_mouse_exit(&event_state);
	    event_state.mouse_node = hit_node;
	    event_state.mouse_node->event_mouse_enter(&event_state);

	}

	// signal mouse move
	Node *curNode = event_state.mouse_node;
	while(curNode != NULL){
	    curNode->event_mouse_move(&event_state);
	    curNode = curNode->event_node_parent;
	    //event_state.mouse_node->event_mouse_move(&event_state);
	}

	// if there was an object picked drag it
	if (event_state.picked_object != NULL) 
	{

#ifdef DEBUG_EVENTSYSTEM_MANAGER
	    cout << "drag object " << event_state.picked_object << endl;
#endif

	    event_state.picked_object->event_mouse_drag(&event_state);
	}
    }
    
    void 
    Manager::mouse_press(int x, int y, Button button, bool shift, bool ctrl, bool alt)
    {
	event_state.x = x;
	event_state.y = y;
	event_state.button = button;
	event_state.shift = shift;
	event_state.ctrl  = ctrl;
	event_state.alt = alt;

	PickableObject *obj = NULL;


	// drop
	Node* curNode = event_state.mouse_node;		

	while(curNode != NULL){		    
	    obj = curNode->event_mouse_pick(&event_state);
	    if(obj != NULL)
		break;
	    else
		curNode = curNode->event_node_parent;
	}



	// // pick obect
	// obj = event_state.mouse_node->event_mouse_pick(&event_state);
	// cout << "Object Mouse Picked " << long(obj) << endl;

	//
	event_state.picked_object = obj;

	if (obj != NULL)
	{
	    event_state.pick_x = x;
	    event_state.pick_y = y;
	    event_state.pick_offset_x = x + event_state.offset_x;
	    event_state.pick_offset_y = y + event_state.offset_y;
	}

    }


    void 
    Manager::mouse_release(int x, int y, Button button, bool shift, bool ctrl, bool alt)
    {

#ifdef DEBUG_EVENTSYSTEM_MANAGER
	cout << "Manager::mouse_release() " << endl;
#endif

	event_state.x = x;
	event_state.y = y;
	event_state.button = button;
	event_state.shift = shift;
	event_state.ctrl  = ctrl;
	event_state.alt = alt;

	// drop
	Node* curNode = event_state.mouse_node;		

	while(curNode != NULL){		    

#ifdef DEBUG_EVENTSYSTEM_MANAGER
	    cout << "   event_node_name " 
	    	 << curNode->event_node_name(&event_state) << endl;
#endif

	    bool processed = curNode->event_mouse_drop(&event_state);
	    if(processed)
		break;
	    else
		curNode = curNode->event_node_parent;
	}

	//event_state.mouse_node->event_mouse_drop(&event_state);

	if (event_state.picked_object != NULL)
	    event_state.picked_object->event_mouse_release(&event_state);

	event_state.picked_object = NULL;
    }

    void 
    Manager::mouse_wheel(int dx, int dy)
    {
	event_state.wheel_dx = dx;
	event_state.wheel_dy = dy;


	Node* curNode = event_state.mouse_node;		

	while(curNode != NULL){

#ifdef DEBUG_EVENTSYSTEM_MANAGER
	    cout << "Wheel CurNode " << curNode->event_node_name(&event_state) << endl;
#endif

	    bool processed = curNode->event_mouse_wheel(&event_state);
	    if(processed)
		break;
	    else
		curNode = curNode->event_node_parent;
	}
    }


    void 
    Manager::key_press(Key key, bool shift, bool ctrl, bool alt)
    {
	event_state.key = key;
	event_state.shift = shift;
	event_state.ctrl  = ctrl;
	event_state.alt = alt;

#ifdef DEBUG_EVENTSYSTEM_MANAGER
	cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Manager Key Press Event" << endl;
	cout << "" << endl;
#endif


	// key press
	Node* curNode = event_state.mouse_node;		

	while(curNode != NULL){

#ifdef DEBUG_EVENTSYSTEM_MANAGER
	    cout << "CurNode " << curNode->event_node_name(&event_state) << endl;
#endif

	    bool processed = curNode->event_key_press(&event_state);
	    if(processed)
		break;
	    else
		curNode = curNode->event_node_parent;
	}
    }

} // end namespace mouse
