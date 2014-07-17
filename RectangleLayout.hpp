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

#ifndef RECTANGLE_LAYOUT_HPP
#define RECTANGLE_LAYOUT_HPP

#include <iostream>
#include <vector>

#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include "Matrix.hpp"

using namespace std;
using namespace linalg;

namespace rectanglelayout {

    class Node;
    class Leaf;
    class Grid;
    class RectangleLayout;
    

    /*************
     * Rectangle *	     
     *************/

    class Rectangle
    {
    private:

	friend ostream& operator<<(ostream&, const Rectangle&);

    public:
	float x;
	float y;
	float w;
	float h;
	Rectangle();
	Rectangle(float x, float y, float w, float h);
	void operator()(float x, float y, float w, float h);
	bool contains(float xx, float yy);
    };

    /*********
     * Index *	     
     *********/

    class Index
    {
    private:

	friend ostream& operator<<(ostream&, const Index&);

    public:
	int x;
	int y;

	Index();
	Index(int x, int y);
	void operator()(int x, int y);
    };

    /********
     * Node *	     
     ********/

    class Node
    {

    public:

	typedef boost::signals2::signal<void (Rectangle)>  rectangle_signal_t;

	typedef boost::signals2::connection       rectangle_signal_connection_t;

    private:

	RectangleLayout       *rectangle_layout;

	Node                  *parent;

	Index                 index_on_parent;

	rectangle_signal_t    rectangle_signal;

    public:

	Node(RectangleLayout *rectangle_layout);
	
	Node*         getParent() const;

	Index         getIndexOnParent() const;

	void          setParent(Node *parent, Index index_on_parent);

	virtual bool  isLeaf() const;

	virtual bool  isGrid() const;

	virtual Leaf* asLeaf();

	virtual Grid* asGrid();

	RectangleLayout* getRectangleLayout();

    public: 
 
	rectangle_signal_connection_t 
	connect(rectangle_signal_t::slot_function_type subscriber);

	void 
	disconnect(rectangle_signal_connection_t subscriber);

	void          
	emitRectangleSignal(Rectangle r);

    };

    /********
     * Leaf *	     
     ********/

    class Leaf:
	public Node
    {
    private:

	friend ostream& operator<<(ostream&, const Leaf&);

    public:
	Leaf(RectangleLayout *rectangle_layout);

	virtual Leaf* asLeaf();
	virtual bool  isLeaf() const;
    };

    /********
     * Grid *	     
     ********/

    class Grid: 
	public Node

    {
    private:
	
	friend ostream& operator<<(ostream&, const Grid&);

    private:

	int            xcells;
	int            ycells;

	float         *xstops;
	float         *ystops;

	Node         **cells;

    public:

	Grid(RectangleLayout *rectangle_layout, int xcells, int ycells);

	~Grid();

	Node   *getNode(int xindex, int yindex) const;

	Node   *getNode(int linear_index) const;

	void    setNode(Node *node, int xindex, int yindex);

	void    setNode(Node *node, Index index);

	int     size()  const;

	virtual Grid* asGrid();

	virtual bool  isGrid() const;

	int     getNumXCells();
	
	int     getNumYCells();

	void    setXStop(int stop, float x);

	void    setYStop(int stop, float y);

	void    moveXStop(int stop, float dx);

	void    moveYStop(int stop, float dy);

	Rectangle     getCanonicalRectangle(int linear_index);

    };

    /**********
     * Handle *	     
     **********/

    class Handle
    {
    public:
	Grid *grid;
	int   dim;
	int   stop;
	mat3  matrix;

	Handle();
	Handle(Grid *grid, int dim, int stop);

	void move(float dx, float dy);

    private:
	friend ostream& operator<<(ostream&, const Handle&);
    };

    /*******************
     * RectangleLayout *	     
     *******************/

    class RectangleLayout
    {

    private:

	Node *root;

    public:

	RectangleLayout();

	~RectangleLayout();

	Node *getRoot();

	Node* getNode(Grid *parent, int xindex, int yindex);

	Grid *subdivide(Leaf *leaf, int xcells, int ycells);

	Leaf *deleteNode(Node *node);

	void  reparent(Node *node, Node *new_parent, Index index_on_new_parent);

	bool findHandle(float x, float y, Rectangle rect, Handle &handle);

    public:

	class Iterator {
	public:

	    class Item {
	    public:

		Item();
		Item(Node *node, int linear_index, int depth);

		Node     *node;
		int       linear_index;
		int       depth;
		mat3      matrix;

		Rectangle rect();

	    private:
		friend ostream& operator<<(ostream&, const  Item &);
	    };

	private:
	    Node            *iter_root;
	    vector<Item>     stack;
	    Item             current_item;

	public:
	    Iterator(Node *node, Rectangle rectangle);

	    Item *next();

	    Item *pop();
	};

    };


    /*****************
     * RectangleSlot *
     *****************/
    class RectangleSlot
    {
    private:
	
	Node::rectangle_signal_connection_t  signal_connection;
	
    public:
	
	RectangleSlot();
	
	virtual void connectToRectangleSignal(rectanglelayout::Node *area);
	
	virtual void rectangleSignal(rectanglelayout::Rectangle r);
	
    };


}

#endif
