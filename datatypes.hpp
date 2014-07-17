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

#ifndef DATATYPES
#define DATATYPES

#include <vector>
#include <string>
#include <map>
#include <stack>

#include "Curve.hpp"


#include "Color.hpp"
#include "ViewRect.hpp"

using namespace std;

namespace datatypes {

// Rectangle System

    class RectangleSystem {

    public:

	enum Side {LEFT, RIGHT, TOP, BOTTOM};

	enum Orientation { LEFT_RIGHT, TOP_DOWN };

    public:

	class Item {

	public:

	    vector<Item*> top_neighbours;
	    vector<Item*> bottom_neighbours;
	    vector<Item*> right_neighbours;
	    vector<Item*> left_neighbours;

	    bool  positioned;
	    bool  visited;
	    bool  visible;

	    string name;

	    float w,h,w_weight,h_weight;

	    // coordinates computed by the rectangle system
	    float x0rs, y0rs, x1rs, y1rs;

	    Item(string name)
	    {
		w = h = 0;
		w_weight      = h_weight = 1.0;
		visible       = true;
		this->name    = name;
	    }

	    double getCoordinate(int dim, int index)
	    {
		if (dim == 0)
		{
		    if (index == 0)
			return x0rs;
		    else if (index == 1)
			return x1rs;
		}
		else if (dim == 1)
		{
		    if (index == 0)
			return y0rs;
		    else if (index == 1)
			return y1rs;
		}
		return -1;
	    }

	    double weight(int dim)
	    {
		if (!visible)
		    return 0.0;

		if (dim == 0)
		{
		    return w_weight;
		}
		else if (dim == 1)
		{
		    return h_weight;
		}
		return 0.0;
	    }

	    double length(int dim)
	    {
		if (!visible)
		    return 0.0;

		if (dim == 0)
		{
		    return w;
		}
		else if (dim == 1)
		{
		    return h;
		}
		return 0.0;
	    }

	    void setCoordinate(int dim, int index, double value)
	    {
		if (dim == 0)
		{
		    if (index == 0)
		    {
			x0rs = value;
			return;
		    }
		    else if (index == 1)
		    {
			x1rs = value;
			return;
		    }
		}
		else if (dim == 1)
		{
		    if (index == 0)
		    {
			y0rs = value;
			return;
		    }
		    else if (index == 1)
		    {
			y1rs = value;
			return;
		    }
		}
	    }


	    int neighbourhoodSize(Side s)
	    {
		switch(s) {
		case LEFT:
		    return left_neighbours.size();
		case RIGHT:
		    return right_neighbours.size();
		case TOP:
		    return top_neighbours.size();
		case BOTTOM:
		    return bottom_neighbours.size();
		default:
		    return -1;
		}
	    }

	    Item *neighbour(Side s, int index)
	    {
		switch(s) {
		case LEFT:
		    return left_neighbours.at(index);
		case RIGHT:
		    return right_neighbours.at(index);
		case TOP:
		    return top_neighbours.at(index);
		case BOTTOM:
		    return bottom_neighbours.at(index);
		default:
		    return NULL;
		}
	    }

	    ViewRect getViewRect()
	    {
	        ViewRect vr;
	        vr.init(x0rs, y0rs, x1rs - x0rs, y1rs - y0rs);
	        return vr;
	    }

	    void init(float w, float h, float w_weight, float h_weight)
	    {
		this->w = w;
		this->h = h;
		this->w_weight = w_weight;
		this->h_weight = h_weight;
	    }

	    string str() 
	    {
		char text[1024];
		sprintf(text,
			"x0rs: %f  y0rs: %f  x1rs: %f  y1rs: %f (wrs: %f hrs: %f)",
			x0rs,y0rs,x1rs,y1rs,
			x1rs - x0rs,
			y1rs - y0rs);
		return string(text);
	    }

	    bool contains(float pointX, float pointY){
		bool horizontal = false;
		bool vertical   = false;

		horizontal = (x0rs <= pointX) 
		    && (pointX <= x1rs);
		vertical =   (y0rs <= pointY) 
		    && (pointY <= y1rs);

		return horizontal && vertical;
	    }

	};

    public:


	vector<Item*> items;
	map< string, Item* > dictionary;
	float x,y,w,h;

	Item *top;
	Item *bottom;
	Item *right;
	Item *left;

	RectangleSystem(){
	    x = y = 0.0;
	    w = h = 1.0;

	    top = new Item("top");
	    bottom = new Item("bottom");
	    right  = new Item("right");
	    left  = new Item("left");

	    dictionary["top"]    = top;
	    dictionary["bottom"] = bottom;
	    dictionary["right"]  = right;
	    dictionary["left"]   = left;
	}

	Item *operator[](string name)
	{
	    return dictionary[name];
	}

	void setBounds(float x, float y, float w, float h)
	{
	    this->x = x; this->y = y;
	    this->w = w; this->h = h;
	}


	void connect(Item *a, Item *b, Orientation orient)
	{
	    if (orient == LEFT_RIGHT)
	    {
		a->right_neighbours.push_back(b);
		b->left_neighbours.push_back(a);
	    }
	    else if (orient == TOP_DOWN)
	    {
		a->bottom_neighbours.push_back(b);
		b->top_neighbours.push_back(a);
	    }
	}


	Item *addItem(
	    float w, float h, float w_weight, float h_weight,
	    string name)
	{
	    Item *rsi = new Item(name);
	    rsi->init(w,h,w_weight,h_weight);
	    dictionary[name] = rsi;
	    items.push_back(rsi);
	    return rsi;
	}


	// void update()
	// {
	// 	cout << "update" << endl;
	// 	top->update_top(y+h);
	// 	bottom->update_bottom(y);
	// 	right->update_right(x);
	// 	left->update_left(x+w);

	// 	for (int i=0;i<items.size();i++)
	// 	{
	// 	    // items.at(i)->update_finalize();
	// 	}
	// }

	void range(int dim, double &min, double &max)
	{
	    if (dim == 0)
	    {
		min = x;
		max = x + w;
	    }
	    else {
		min = y;
		max = y + h;
	    }
	}

	void update()
	{
	    //
	    Item *uu;
	    Item *vv;
	    Item *ww;
	    Item *aa;
	    Item *bb;

	    typedef pair<Item*,int> DFSItem;


	    string pairs[]      = {"left","right","bottom","top"};
	    int    dimensions[] = {0, 1};
	    Side   traversal_sides[]      = {RIGHT, LEFT, TOP, BOTTOM};


	    int n = 2;


	    for (int i=0; i<n; i++)
	    {
		Item *min_item = (*this)[pairs[2*i]];
		Item *max_item = (*this)[pairs[2*i+1]];

		int dim = dimensions[i];

		Side traversal_side = traversal_sides[2*i];
		Side reverse_traversal_side = traversal_sides[2*i+1];

		double min_value, max_value;
		this->range(dim,min_value,max_value);

		min_item->positioned = true;
		min_item->visited    = false;
		min_item->setCoordinate(dim,1,min_value);

		max_item->positioned = true;
		max_item->visited    = false;
		max_item->setCoordinate(dim,0,max_value);

		// clear flags
		for (int i=0;i<items.size();i++)
		{
		    items.at(i)->positioned = false;
		    items.at(i)->visited    = false;
		}

#ifdef RECTANGLE_SYSTEM_DEBUG
		cout << endl << "======= DIMENSION: " << dim << endl << endl;
#endif

		{ // first run

		    // while there exists a path from bottom to top adjust 
		    // the positions of these vertices
		    vector<DFSItem> stack;
		    DFSItem root(min_item,0);
		    stack.push_back(root);

		    while (stack.size() > 0)
		    {
			DFSItem top = stack.back();
			uu = top.first;
			int index = top.second;


#ifdef RECTANGLE_SYSTEM_DEBUG
			cout << "top: " << uu->name << " index " << index << endl;
#endif

			// push one more vertex
			if (index < uu->neighbourhoodSize(traversal_side))
			{
			    vv = uu->neighbour(traversal_side, index);

#ifdef RECTANGLE_SYSTEM_DEBUG
			    cout << "   child: " << vv->name << endl;
#endif

			    // found interval: position nodes
			    if (vv->positioned) { 
			
#ifdef RECTANGLE_SYSTEM_DEBUG
				cout << "      positioned..." << endl;
#endif

				// found interval: aa = p np np np ... np p = bb
				// where p stands for positioned
				// and np stands for not positioned
			
				float sum_weighted = 0.0;
				float sum_fixed    = 0.0;
				int   aa_index     = -1;

				bb = vv; 
			
				for (int i=stack.size()-1;i>=0;i--) 
				{
				    ww = stack.at(i).first;

#ifdef RECTANGLE_SYSTEM_DEBUG
				    cout << "             backing up: " << ww->name 
					 << " len: " << ww->length(dim) 
					 << " weight: " << ww->weight(dim) << endl;
#endif

				    if (ww->positioned)
				    {
					aa_index = i;
					break;
				    }
				    else 
				    {
					if (ww->weight(dim) <= 0.0)
					    sum_fixed += ww->length(dim);
					else 
					    sum_weighted += ww->length(dim) * ww->weight(dim);
				    }
				} // figure out aa, bb, sum_fixed, sum_weighted

				aa = stack.at(aa_index).first;

				// y1 of rectangle system (as it was positioned it is known)
				float y0 = aa->getCoordinate(dim,1); 

				// y0 of rectangle system (as it was positioned it is known)
				float y1 = bb->getCoordinate(dim,0); 

				float alpha = (y1 - y0 - sum_fixed) / (sum_weighted);

#ifdef RECTANGLE_SYSTEM_DEBUG
				cout << "             sum_weighted: " << sum_weighted << endl;
				cout << "             sum_fixed:    " << sum_fixed << endl;
				cout << "             alpha:        " << alpha << endl;
				cout << "             coord1:           " << y1 << endl;
				cout << "             coord0:           " << y0 << endl;
#endif

				float yy = y0;
				for (int i=aa_index+1;i<stack.size();i++)
				{
				    ww = stack.at(i).first;

#ifdef RECTANGLE_SYSTEM_DEBUG
				    cout << "             positioning: " << ww->name << endl;
				    cout << "             coord: " << yy << endl;
#endif

				    if (ww->weight(dim) <= 0.0) 
				    {
#ifdef RECTANGLE_SYSTEM_DEBUG
					cout << "                case 1: " << endl;
#endif
					ww->setCoordinate(dim,0,yy);
					yy = yy + ww->length(dim);
					ww->setCoordinate(dim,1,yy);
				    }
				    else 
				    {
#ifdef RECTANGLE_SYSTEM_DEBUG
					cout << "                case 2: " 
					     << (alpha * ww->weight(dim) * ww->length(dim)) << endl;
					cout << "                    coord: " << yy << endl;
#endif
					ww->setCoordinate(dim,0,yy);
					yy = yy + (alpha * ww->length(dim) * ww->weight(dim));
					ww->setCoordinate(dim,1,yy);
				    }
				    ww->positioned = true;
				} // set positions

				// go to next child
				stack.pop_back();
				top.second = index + 1;
				stack.push_back(top);

			    } // found interval: position nodes

			    else if (vv->visited)
			    { // advance top index
				// cout << "      child was visited before..." << endl;
				stack.pop_back();
				top.second = index + 1;
				stack.push_back(top);
			    } // advance top index

			    else 
			    { // advance top index and push "v"
				// cout << "      push: " << vv->name << endl;
				stack.pop_back();
				top.second = index + 1;
				stack.push_back(top);
				DFSItem aux(vv, 0);
				stack.push_back(aux);
			    } // advance top index and push "v"
			}

			// 
			else 
			{ // tag as visited and continue
			    uu->visited = true;
			    stack.pop_back();
			}

		    } // end dfs first run

		}

		{ // bottom-up vertical dfs pass

		    // no vertex position was fixed ...
		    for (int i=0;i<items.size();i++)
		    {
			items.at(i)->visited    = false;
		    }
		    // dfs defining

		    // while there exists a path from bottom to top adjust 
		    // the positions of these vertices
		    vector<DFSItem> stack;
		    DFSItem root(min_item,0);
		    stack.push_back(root);

		    while (stack.size() > 0)
		    {
			DFSItem top = stack.back();
			uu = top.first;
			int index = top.second;

			// cout << "top: " << uu->name << " index " << index << endl;

			// push one more vertex
			if (index < uu->neighbourhoodSize(traversal_side))
			{
			    // cout << "   child: " << vv->name << endl;
			    vv = uu->neighbour(traversal_side, index);

			    if (vv->visited)
			    { // advance top index
				// cout << "      child was visited before..." << endl;
				stack.pop_back();
				top.second = index + 1;
				stack.push_back(top);
			    } // advance top index

			    else 
			    { // advance top index and push "v"
				if (!vv->positioned)
				{
				    vv->setCoordinate(dim,0,uu->getCoordinate(dim,1));
				    vv->setCoordinate(dim,1,vv->getCoordinate(dim,0) + vv->length(dim));
				    vv->positioned = true;
				}

				// cout << "      push: " << vv->name << endl;
				stack.pop_back();
				top.second = index + 1;
				stack.push_back(top);
				DFSItem aux(vv, 0);
				stack.push_back(aux);
			    } // advance top index and push "v"
			}

			// 
			else 
			{ // tag as visited and continue
			    uu->visited = true;
			    stack.pop_back();
			}
		    }

		} // bottom-up vertical dfs pass


		{ // top-down vertical dfs pass

		    // no vertex position was fixed ...
		    for (int i=0;i<items.size();i++)
		    {
			items.at(i)->visited    = false;
		    }
		    // dfs defining

		    // while there exists a path from bottom to top adjust 
		    // the positions of these vertices
		    vector<DFSItem> stack;
		    DFSItem root(max_item,0);
		    stack.push_back(root);

		    while (stack.size() > 0)
		    {
			DFSItem top = stack.back();
			uu = top.first;
			int index = top.second;

			// cout << "top: " << uu->name << " index " << index << endl;

			// push one more vertex
			if (index < uu->neighbourhoodSize(reverse_traversal_side))
			{
			    // cout << "   child: " << vv->name << endl;
			    vv = uu->neighbour(reverse_traversal_side, index);

			    if (vv->visited)
			    { // advance top index
				// cout << "      child was visited before..." << endl;
				stack.pop_back();
				top.second = index + 1;
				stack.push_back(top);
			    } // advance top index

			    else 
			    { // advance top index and push "v"
				if (!vv->positioned)
				{
				    vv->setCoordinate(dim,1,uu->getCoordinate(dim,0));
				    vv->setCoordinate(dim,0,uu->getCoordinate(dim,0) - vv->length(dim));
				    vv->positioned = true;
				}

				// cout << "      push: " << vv->name << endl;
				stack.pop_back();
				top.second = index + 1;
				stack.push_back(top);
				DFSItem aux(vv, 0);
				stack.push_back(aux);
			    } // advance top index and push "v"
			}

			// 
			else 
			{ // tag as visited and continue
			    uu->visited = true;
			    stack.pop_back();
			}
		    }

		} // top-down vertical dfs pass

	    } // end cases (horiz and vertical)

	} // end update()

    }; // end RectangleSystem


    namespace selection {

	class Area {
	public:
	    virtual bool contains(double xx, double yy) { return false; }
	    virtual int getNumRectangles() { return 0; }
	    virtual void getRectangle(int index, double &x, double &y, 
				      double &w, double &h) 
	    { 
		return; 
	    }
	};

	class Rectangle: public Area {
	public:
	    double x,y,w,h;
	    Rectangle(double xx, double yy, double ww, double hh)
	    {
		x = xx;
		y = yy;
		w = ww;
		h = hh;
	    }
	    bool contains(double xx, double yy)
	    {
		return (x <= xx && xx < x + w && y <= yy && yy < y + h); 
	    }
	    virtual int getNumRectangles() { return 1; }
	    void getRectangle(int index, double &x, double &y, 
			      double &w, double &h) 
	    { 
		if (index == 0)
		{
		    x = this->x;
		    y = this->y;
		    w = this->w;
		    h = this->h;
		}
	    }


	};

	class AreaSet: public Area
	{
	public:
	    vector<Area*> areas;
	    AreaSet() { }
	    void addArea(Area *a) { areas.push_back(a); }
	    bool contains(double xx, double yy) { 
		vector<Area*>::iterator it;
		for (it = areas.begin();it < areas.end(); it++)
		{
		    if ((*it)->contains(xx,yy))
			return true;
		}
		return false;
	    }
	    int getNumRectangles() { 
		int c = 0;
		for (int i=0;i<areas.size();i++)
		    c += areas.at(i)->getNumRectangles();
		return c;
	    }
	    void getRectangle(int index, double &x, double &y, 
			      double &w, double &h) 
	    {
		int c = 0;
		for (int i=0;i<areas.size();i++) {
		    int nri = areas.at(i)->getNumRectangles();
		    if (c + nri >= index)
		    {
			areas.at(i)->getRectangle(index - c, x, y, w, h);
			return;
		    }
		    c += nri;
		}
	    }

	};

	class SelectionX: public AreaSet {
	public:
	    enum render_mode{ REGULAR, TAG_CLOUD};
	    
	public:
	    static int ID_COUNT;
	    render_mode render_mode;

	    int id;

	    Color color;
	    SelectionX(Color c) : AreaSet() {
		color = c;
		ID_COUNT++;
		this->id = ID_COUNT;
		render_mode = REGULAR;
	    }

	    void toggleRenderMode()
	    {
		if (render_mode == REGULAR)
		    render_mode = TAG_CLOUD;
		else
		    render_mode = REGULAR;
	    }

	};
       
	class Selection {
	public:

	    enum render_mode{ REGULAR, TAG_CLOUD};
	    
	public:

	    static int ID_COUNT;

	    int         id;
	    Color       color;
	    ViewRect    quad;     // world quad
	    ViewRect    aux_quad; // aux quad
	    render_mode render_mode;

	    Selection(Color c, ViewRect quad) {
		ID_COUNT++;
		this->id          = ID_COUNT;
		this->color       = c;
		this->render_mode = REGULAR;
		this->quad        = quad;
	    }

	    void toggleRenderMode()
	    {
		if (render_mode == REGULAR)
		    render_mode = TAG_CLOUD;
		else
		    render_mode = REGULAR;
	    }

	    bool contains(double x, double y)
	    {
		return quad.contains(x, y);
	    }

	};


    } // end namespace: selection


//
    class Point
    {
    public:
	double x_coord; //longitude
	double y_coord; //latitude
	double scalar;
	
	Point();
	Point(double x, double y, double v);

	~Point();
	double getXCoord();
	double getYCoord();
	double getScalar();

    };

    class PointSet
    {
    public:
	vector<Point*> *points;
	
	PointSet();
	~PointSet();
	void addPoint(Point *point);
	Point* getPoint(int i);
	int getNumPoints();

    };

    class Predictor
    {
    public:
	string name;
	vector<PointSet*> *pointSets;
	Curve *curve;

	Predictor();
	Predictor(string name);
	Predictor(string name, int num_time_steps);
	~Predictor();
	int getNumPointSets();
	PointSet* getPointSet(int i);
	void addPointSet(PointSet* pointSet);
	Curve* getCurve();
	Curve* getCurveSel(selection::Selection *sel);
	string getName();

    };
	    
    class PredictorList
    {
    public:
	vector<Predictor*> *predictors;
	PredictorList(vector<Predictor*> *predictors)
	{
	    this->predictors = predictors;
	}
	PredictorList()
	{
	    this->predictors = new vector<Predictor*>();
	}
	void addPredictor(Predictor* p)
	{
	    this->predictors->push_back(p);
	}

    };


    class TagCloudSpec
    {
    public:
	int key;
	ViewRect window;
	vector<string> tags;
	vector<double> sizes;
	TagCloudSpec() {};

	void addTag(string tag, double size)
	{
	    this->tags.push_back(tag);
	    this->sizes.push_back(size);
	}

	/*!
	 * Returns wheter could find a non-empty range of values
	 */
	bool range(double &min_size, double &max_size)
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

    }; // end class TagCloudSpec






    class Grid
    {
    public:
	ViewRect   quad;
	int        xcells, ycells;
	double    *values;

	Grid(int xcells, int ycells, ViewRect quad)
	{
	    this->xcells = xcells;
	    this->ycells = ycells;
	    this->quad   = quad;
	    values = new double[xcells * ycells];
	}

	~Grid()
	{
	    delete [] values;
	}

	double getValue(int x, int y)
	{
	    return values[y * xcells + x];
	}

	void setValue(int x, int y, double v)
	{
	    values[y * xcells + x] = v;
	}

	bool getValueAtCoordinates(double lon, double lat, double &output)
	{
	    double cw = quad.w / xcells;
	    double ch = quad.h / ycells;

	    int cellx = (int) ((lon - quad.x)/quad.w * xcells);
	    int celly = (int) ((lat - quad.y)/quad.h * ycells);

	    if (cellx < 0 || cellx >= xcells || celly < 0 || celly >= ycells)
		{
		    return false;
		}
		    
		    
		    output = getValue(cellx, celly);
		    return true;
		    

	    


	}
	
    };

} // end namespace: datatypes

#endif
