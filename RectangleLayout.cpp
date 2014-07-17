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

#include "RectangleLayout.hpp"

#include "Rotation.hpp"

#define xDEBUG_RECTANGLESLOT
#define xDEBUG_RECTANGLELAYOUT

namespace rectanglelayout {

    /*************
     * Transform *	     
     *************/

    Rectangle::Rectangle():
	x(0),
	y(0),
	w(1),
	h(1)
    {}

    Rectangle::Rectangle(float x, float y, float w, float h):
	x(x),
	y(y),
	w(w),
	h(h)
    {}

    void
    Rectangle::operator()(float x, float y, float w, float h)
    {
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
    }

    bool
    Rectangle::contains(float xx, float yy)
    {
	return (x <= xx && xx < x + w && y <= yy && yy < y + h);
    }

    ostream& 
    operator<<(ostream& strm, const Rectangle& r) 
    {
	strm << "Rectangle: "
	     << "(x: " << r.x 
	     <<", y: " << r.y 
	     <<", w: " << r.w 
	     <<", h: " << r.h << ")";
    }


    /*********
     * Index *	     
     *********/

    Index::Index():
	x(0),
	y(0)
    {}

    Index::Index(int x, int y):
	x(x),
	y(y)
    {}

    void
    Index::operator()(int x, int y)
    {
	this->x = x;
	this->y = y;
    }

    ostream& 
    operator<<(ostream& strm, const Index& index) 
    {
	strm << "Index: [" << index.x <<", " << index.y <<"]";
    }

    /********
     * Node *	     
     ********/
    
    Node::Node(RectangleLayout *rectangle_layout):
	rectangle_layout(rectangle_layout),
	parent(NULL)
    {}

    Node*
    Node::getParent() const
    {
	return parent;
    }

    Index
    Node::getIndexOnParent() const
    {
	return index_on_parent;
    }

    void
    Node::setParent(Node *parent, Index index_on_parent)
    {
	this->parent = parent;
	this->index_on_parent = index_on_parent;
    }

    Leaf*
    Node::asLeaf()
    {
	return 0;
    }

    bool
    Node::isLeaf() const
    {
	return false;
    }

    Grid*
    Node::asGrid()
    {
	return 0;
    }

    bool
    Node::isGrid() const
    {
	return false;
    }

    RectangleLayout* 
    Node::getRectangleLayout()
    {
	return rectangle_layout;
    }

    Node::rectangle_signal_connection_t 
    Node::connect(
	Node::rectangle_signal_t::slot_function_type subscriber)
    {

#ifdef DEBUG_RECTANGLELAYOUT
	cout << "Node::connect(...)" << endl; 
#endif
	Node::rectangle_signal_connection_t result = rectangle_signal.connect(subscriber);
#ifdef DEBUG_RECTANGLELAYOUT
	cout << "   connection: " << &result << endl;
#endif

	return result;
    }

    void 
    Node::disconnect(
	Node::rectangle_signal_connection_t subscriber)
    {
	subscriber.disconnect();
    }

    void 
    Node::emitRectangleSignal(Rectangle r)
    {
	rectangle_signal(r);
    }

    /********
     * Leaf *	     
     ********/

    Leaf::Leaf(RectangleLayout* rectangle_layout):
	Node::Node(rectangle_layout)
    { }

    Leaf*
    Leaf::asLeaf()
    {
	return this;
    }

    bool
    Leaf::isLeaf() const
    {
	return true;
    }

    ostream& 
    operator<<(ostream& strm, const Leaf& leaf)
    {
	if (leaf.getParent() == NULL)
	    return strm << "Leaf (root)   [" << &leaf << "]";
	else {
	    Index idx = leaf.getIndexOnParent();
	    return strm << "Leaf (index_on_parent: " 
			<< idx << "  parent: [" 
			<< leaf.getParent() << "])  [" 
			<< &leaf << "]";
	}
    }


    /********
     * Grid *	     
     ********/

    Grid::Grid(RectangleLayout *rectangle_layout, int xcells, int ycells):
	Node::Node(rectangle_layout),
	xcells(xcells),
	ycells(ycells)
    {
	xstops = new float[xcells + 1];
	ystops = new float[ycells + 1];
	cells  = new Node* [xcells * ycells];
	
	float cw = 1.0 / xcells;
	float ch = 1.0 / ycells;
	
	for (int i=1;i<xcells;i++)
	    xstops[i] = i * cw;
	
	for (int i=1;i<ycells;i++)
	    ystops[i] = i * ch;

	xstops[0]      = 0.0;
	ystops[0]      = 0.0;
	xstops[xcells] = 1.0;
	ystops[ycells] = 1.0;

	for (int i=0;i<xcells;i++)
	    for (int j=0;j<ycells;j++)
		cells[i*ycells + j] = NULL;
    }

    Grid::~Grid()
    {
	delete [] xstops;
	delete [] ystops;
	for (int i=0;i<xcells * ycells;i++)
	    if (cells[i] != NULL)
		delete cells[i];
	delete [] cells;
    }

    Node*
    Grid::getNode(int xindex, int yindex) const
    {
	return cells[xindex * ycells + yindex];
    }

    Node*
    Grid::getNode(int linear_index) const
    {
	return cells[linear_index];
    }

    int
    Grid::size() const
    {
	return xcells * ycells;
    }


    void
    Grid::setNode(Node *node, int xindex, int yindex)
    {
	cells[xindex * ycells + yindex] = node;
    }

    void
    Grid::setNode(Node *node, Index index)
    {
	cells[index.x * ycells + index.y] = node;
    }

    Grid*
    Grid::asGrid()
    {
	return this;
    }

    bool
    Grid::isGrid() const
    {
	return true;
    }

    int     
    Grid::getNumXCells()
    {
	return xcells;
    }
    
    int     
    Grid::getNumYCells()
    {
	return ycells;
    }

    void    
    Grid::moveXStop(int stop, float dx)
    {
	xstops[stop] += dx;
	if (dx > 0 && xstops[stop] > xstops[stop+1])
	    xstops[stop] = xstops[stop+1];
	else if (dx < 0 && xstops[stop] < xstops[stop-1])
	    xstops[stop] = xstops[stop-1];

    }

    void    
    Grid::moveYStop(int stop, float dy)
    {
	ystops[stop] += dy;
	if (dy > 0 && ystops[stop] > ystops[stop+1])
	    ystops[stop] = ystops[stop+1];
	else if (dy < 0 && ystops[stop] < ystops[stop-1])
	    ystops[stop] = ystops[stop-1];
    }

    void    
    Grid::setXStop(int stop, float x)
    {
	xstops[stop] = x;
    }
    
    void    
    Grid::setYStop(int stop, float y)
    {
	ystops[stop] = y;
    }

    Rectangle
    Grid::getCanonicalRectangle(int linear_index)
    {
	int xindex = linear_index / ycells;
	int yindex = linear_index % ycells;

	float x0 = xstops[xindex];
	float y0 = ystops[yindex];
	float x1 = xstops[xindex+1];
	float y1 = ystops[yindex+1];

	Rectangle result(x0,y0,x1-x0,y1-y0);
	return result;
    }


    ostream& 
    operator<<(ostream& strm, const Grid& grid)
    {
	if (grid.getParent() == NULL)
	    return strm << "Grid " << grid.xcells 
			<< " x " << grid.ycells 
			<< " (root)  [" << &grid << "]";
	else {
	    Index idx = grid.getIndexOnParent();
	    strm << "Grid " << grid.xcells 
		 << " x " << grid.ycells 
		 << " (index_on_parent: " << idx << ", parent: ["
		 << grid.getParent() << "])   [" 
		 << &grid << "]";

	}
    }

    /*******************
     * Handle *	     
     *******************/

    Handle::Handle():
	grid(NULL),
	dim(0),
	stop(0)
    {}

    Handle::Handle(Grid *grid, int dim, int stop):
	grid(grid),
	dim(dim),
	stop(stop)
    {}


    void
    Handle::move(float dx, float dy)
    {
	vec2 p0(0,0);
	p0 = matrix.transformAffine(p0);
	vec2 p1(dx,dy);
	p1 = matrix.transformAffine(p1);

	dx = (p1 - p0).x;
	dy = (p1 - p0).y;

	if (dim == 0)
	{
	    grid->moveXStop(stop, dx);
	}
	else 
	{
	    grid->moveYStop(stop, dy);
	}
    }



    ostream& operator<<(ostream& strm, const Handle& handle)
    {
	return strm << "Handle grid: " << *handle.grid 
		    << " dim:  " << handle.dim 
		    << " stop: " << handle.stop << endl;
    }

    /*******************
     * RectangleLayout *	     
     *******************/

    RectangleLayout::RectangleLayout():
	root(new Leaf(this))
    {}

    RectangleLayout::~RectangleLayout()
    {
	delete root;
    }

    Node*
    RectangleLayout::getRoot()
    {
	return root;
    }

    Leaf*
    RectangleLayout::deleteNode(Node *node)
    {
	Leaf* leaf = new Leaf(this);
	if (node == root)
	{
	    root = leaf;
	}
	else
	{
	    Node *parent       = node->getParent();
	    Index parent_index = node->getIndexOnParent();
	    this->reparent(leaf, parent, parent_index);
	}
	return leaf;
    }

    Node*
    RectangleLayout::getNode(Grid *parent, int xindex, int yindex)
    {
	Node *node = parent->getNode(xindex, yindex);
	// if (node == NULL)
	// {
	//     node = new Leaf();
	//     Index idx(xindex, yindex);
	//     this->reparent(node, parent, idx);
	// }
	return node;
    }

    void
    RectangleLayout::reparent(
	Node *node, Node *new_parent, Index index_on_new_parent)
    {
	if (node->getParent() != NULL)
	{
	    node->getParent()->
		asGrid()->setNode(NULL, node->getIndexOnParent());
	}

	if (new_parent != NULL)
	{
	    // todo: check there is nothing at the index
	    new_parent->asGrid()->
		setNode(node, index_on_new_parent);
	}

	node->setParent(new_parent, index_on_new_parent);
    }

    Grid*
    RectangleLayout::subdivide(Leaf *leaf, int xcells, int ycells)
    {
	Index idx;

	// save index
	Node *leaf_parent = leaf->getParent();
	Index leaf_idx    = leaf->getIndexOnParent();

	// reparent to null
	this->reparent(leaf, NULL, idx);

	// create grid node
	Grid *g = new Grid(this, xcells, ycells);

	// reparent grid node
	this->reparent(g,leaf_parent, leaf_idx);

	// grid is the new root
	if (leaf == root)
	    root = g;

	// make it the top left leaf of the new grid
	// idx(0,ycells-1);
	// this->reparent(leaf, g, idx);

	for (int i=0;i<xcells;i++)
	    for (int j=0;j<ycells;j++)
	    {
		idx(i,j);
		if (i == 0 && j == ycells-1)
		    this->reparent(leaf, g, idx); // keep original 
		                                  // one as top-left
		else
		    this->reparent(new Leaf(this), g, idx); // add new leaves
	    }

	return g;
    }

    bool
    RectangleLayout::findHandle(float x, float y, Rectangle rect, Handle &handle)
    {
	
	enum Side {Left, Right, Top, Bottom};
	
	Side  hit_side;
	Leaf *hit_leaf = NULL;
	    
	// first step: find leaf that contains x,y
	RectangleLayout::Iterator it(this->getRoot(), rect);
	RectangleLayout::Iterator::Item* item;
	while (1)
	{
	    item = it.next();
	    if (item == NULL)
		break;

	    // is it a leaf?
	    if (item->node->isLeaf())
	    {
		// check if x,y is in the rect
		Rectangle r = item->rect();

#ifdef DEBUG_RECTANGLELAYOUT
		cout << "   " << r << endl;
		cout << "   " << "xx, yy:" << x << " x " << y << endl;
#endif

		// did not contain the x,y
		if (!r.contains(x,y))
		{
		    continue;
		}
		    
		hit_leaf = item->node->asLeaf();

		// found leaf containing point x,y
		float left    = x - r.x;
		float right   = r.x + r.w - x;
		float bottom  = y - r.y;
		float top     = r.y + r.h - y;

		if (left <= right && left <= top && left <= bottom)
		{
		    hit_side = Left;
		}
		else if (right <= top && right <= bottom)
		{
		    hit_side = Right;
		}
		else if (top <= bottom)
		{
		    hit_side = Top;
		}
		else
		{
		    hit_side = Bottom;
		}

		break;
	    }

	} // end search for leaf and side

	if (hit_leaf == NULL || hit_leaf == root)
	    return false;

#ifdef DEBUG_RECTANGLELAYOUT
	cout << "   hit_leaf: " << *hit_leaf << endl;
	cout << "   hit_side: " << hit_side << endl; 
#endif

	// go bottom-up from hit leaf searching for the handle node
	Grid *g = hit_leaf->getParent()->asGrid();
	Index idx = hit_leaf->getIndexOnParent();
	mat3  matrix = it.pop()->matrix;  


#ifdef DEBUG_RECTANGLELAYOUT
	cout << "   going up from hit leaf..." << endl;
#endif

	while (g != NULL)
	{
#ifdef DEBUG_RECTANGLELAYOUT
	    cout << "   g = " << *g << endl;
#endif
	    if (hit_side == Left && idx.x > 0)
	    {
		handle.grid      = g;
		handle.dim       = 0;
		handle.stop      = idx.x;
		handle.matrix    = matrix.inverse();
	        return true;
	    }

	    else if (hit_side == Right && idx.x <  g->getNumXCells() - 1)
	    {
		handle.grid      = g;
		handle.dim       = 0;
		handle.stop      = idx.x + 1;
		handle.matrix    = matrix.inverse();
		return true;
	    }

	    else if (hit_side == Top && idx.y < g->getNumYCells() - 1)
	    {
		handle.grid      = g;
		handle.dim       = 1;
		handle.stop      = idx.y + 1;
		handle.matrix    = matrix.inverse();
		return true;
	    }

	    else if (hit_side == Bottom && idx.y > 0)
	    {
		handle.grid      = g;
		handle.dim       = 1;
		handle.stop      = idx.y;
		handle.matrix    = matrix.inverse();
		return true;
	    }
	    
	    idx          = g->getIndexOnParent();
	    Node *parent = g->getParent();
	    if (parent != NULL)
	    {
		g = parent->asGrid();
		matrix = it.pop()->matrix;
	    }
	    else {
		g = NULL;
	    }
	}

	return false;

    }

    /***********************************
     * RectangleLayout::Iterator::Item *	     
     ***********************************/

    RectangleLayout::Iterator::Item::Item():
	node(NULL),
	linear_index(0),
	depth(0)
    {}

    RectangleLayout::Iterator::Item::Item(
	Node *node, int linear_index, int depth):
	node(node),
	linear_index(linear_index),
	depth(depth)
    {}
    
    Rectangle
    RectangleLayout::Iterator::Item::rect()
    {
	vec2 p0(0,0);
	p0 = this->matrix.transformAffine(p0);
	vec2 p1(1,1);
	p1 = this->matrix.transformAffine(p1);
	vec2 size = p1 - p0;
	Rectangle result(p0.x,p0.y,size.x,size.y);
	return result;
    }

    ostream& 
    operator<<(ostream& strm, 
	       const RectangleLayout::Iterator::Item &item)
    {
	for (int i=0;i<3*item.depth;i++)
	    strm << " ";
	strm << "Item (node: " << item.node << ", linear_index: " 
	     << item.linear_index << ")";
    }

    /*****************************
     * RectangleLayout::Iterator *	     
     *****************************/

    RectangleLayout::Iterator::Iterator(Node *node, Rectangle rect)
    {
	this->iter_root = node;
	Item item(node, -1, 0);

	vec2 pos(rect.x, rect.y);
	vec2 scale(rect.w, rect.h);
	item.matrix.makeTransform(pos,scale,Radian::zero);

	stack.push_back(item);
    }

    RectangleLayout::Iterator::Item*
    RectangleLayout::Iterator::pop()
    {
	if (stack.size() > 0)
	{
	    current_item = stack.back();
	    stack.pop_back();
	    return &current_item;
	}
	else return NULL;
    }


    RectangleLayout::Iterator::Item*
    RectangleLayout::Iterator::next()
    {
	if (stack.size() == 0)
	    return NULL;

	while (stack.size() > 0)
	{
	    // print whole stack
	    // for (int i=0;i<stack.size();i++)
	    // 	cout << stack[i] << endl;

	    Item top = stack.back();
	    stack.pop_back();
		
	    if (top.linear_index == -1) // first appearence of node
	    {
		// push next index
		if (top.node!=NULL && top.node->isGrid())
		{
		    top.linear_index = 0; // schedule child 0
		    stack.push_back(top);
		}
		current_item = top;
		return &current_item;
	    }
	    else // check cell of a grid
	    {
		Grid *g = top.node->asGrid();

		if (top.linear_index < g->size())
		{
		    int curr_index = top.linear_index;
		    top.linear_index += 1;

		    Node *child = g->getNode(curr_index);

		    // schedule
		    Item child_top(child, -1, top.depth+1);

		    // set matrix
		    Rectangle canonicalRect = g->getCanonicalRectangle(curr_index);
		    vec2 pos(canonicalRect.x, canonicalRect.y);
		    vec2 scale(canonicalRect.w, canonicalRect.h);
		    mat3 aux;
		    aux.makeTransform(pos,scale,Radian::zero);
		    child_top.matrix = top.matrix * aux;

		    stack.push_back(top);
		    stack.push_back(child_top);
		}
	    }
 	} // while there is stuff

	return NULL;
    }



/*****************
 * RectangleSlot *
 *****************/

    RectangleSlot::RectangleSlot() {}

    void 
    RectangleSlot::connectToRectangleSignal(rectanglelayout::Node *area)
    {
#ifdef DEBUG_RECTANGLESLOT
	cout << "RectangleSlot::installAt(...)" << endl;
#endif
	signal_connection = 
	    area->connect(boost::bind(&RectangleSlot::rectangleSignal, this, _1));
    }

    void 
    RectangleSlot::rectangleSignal(Rectangle r)
    {
#ifdef DEBUG_RECTANGLESLOT
	cout << "RectangleSlot::rectangleSignal()" << endl;
#endif
    }


} // end namespace: rectanglelayout
