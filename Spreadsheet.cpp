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

#include  <boost/bind.hpp>

#include "Spreadsheet.hpp"
#include "Environment.hpp"
#include "SimpleEnvironment.hpp"


using namespace rectanglelayout;
using namespace rendersystem;

#define xDEBUG_LISTVIEWITEM
#define xDEBUG_LISTVIEW
#define xDEBUG_SCROLLPANE
#define xDEBUG_SPREADSHEET
#define xDEBUG_SPREADSHEET_RECTANGLE_LAYOUT_SIGNALS
/*************
 * Component *
 *************/

void 
Component::installAt(rectanglelayout::Node *area) {}

void
Component::render() {}

ViewRect
Component::getBoundingBox()
{
    ViewRect result(0,0,0,0);
    return result;
}

/****************
 * ListViewItem *
 ****************/

ListViewItem::ListViewItem(string label, float fontSize):
    label(label),
    fontSize(fontSize),
    customObject(NULL)
{
    RenderSystem* renderSystem = 
	Global::instance()->getRenderSystem();

    float advance;
    labelBounds = renderSystem->textBoundingBox(label.c_str(),
						fontSize,
						0, 0,
						&advance);

    x = 0;
    y = 0;
}

void  
ListViewItem::setCustomObject(boost::any obj)
{
    customObject = obj;
}

boost::any  
ListViewItem::getCustomObject()
{
    return customObject;
}

void            
ListViewItem::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
}


ViewRect 
ListViewItem::getBoundingBox()
{
    ViewRect result = labelBounds;
    result.translate(x,y);
    return result;
}

void
ListViewItem::render()
{
    rendersystem::RenderSystem *renderSystem = Global::instance()->getRenderSystem();
    renderSystem->drawText(label.c_str(), fontSize, x, y);
}

bool
ListViewItem::event_mouse_contains(int x, int y)
{
    ViewRect bb = this->getBoundingBox();

#ifdef DEBUG_LISTVIEWITEM
    cout << "ListViewItem::event_mouse_contains( " << x << " , " << y << ")" << endl;
    cout << "   Bounding was " << bb.str() << endl;
#endif

    return bb.contains(x,y);
}

eventsystem::PickableObject* 
ListViewItem::event_mouse_pick(eventsystem::State *event_state)
{
    return this;
}

void 
ListViewItem::event_render(eventsystem::State *event_state)
{

#ifdef DEBUG_LISTVIEWITEM
    cout << "ListViewItem::event_render(...) "<< endl;
#endif

    // cout << "----------------------- event render" << endl;
    float xx = event_state->pick_offset_x - x;
    float yy = event_state->pick_offset_y - y;

    float rx = event_state->x - xx;
    float ry = event_state->y - yy;

    ViewRect base = labelBounds;

#ifdef DEBUG_LISTVIEWITEM
    cout << " pick: " << event_state->pick_x << ", " << event_state->pick_y << endl;
    cout << " curr: " << event_state->x << ", " << event_state->y << endl;
    cout << " base: " << base.str() << endl;
#endif


    float margin = 3;
    float bbx = rx + base.x - margin;
    float bby = ry + base.y - margin;
    float bbw = base.w + 2 * margin;
    float bbh = base.h + 2 * margin;

    rendersystem::RenderSystem* renderSystem = 
	Global::instance()->getRenderSystem();

    renderSystem->setColor(1,1,1,0.7);
    renderSystem->drawQuad(bbx,bby,bbw,bbh);

    renderSystem->setColor(0,0,0);
    renderSystem->drawText(label.c_str(), fontSize, rx, ry);
}


/************
 * ListView *
 ************/

ListView::ListView()
{
    itemsBoundingBox.init(0,0,0,0);
}

void
ListView::update()
{   
    ViewRect bb(0,0,0,0);
    float xx = 0;
    float yy = 0;
    float sep = 8;
    for (int i=items.size()-1;i>=0;i--)
    {
	ListViewItem *item = items.at(i);
	item->setPosition((int) xx, (int) yy);
	ViewRect item_bb = item->getBoundingBox();
	bb.resizeToIncludeRect(item_bb);
	yy = bb.y + bb.h + sep;
    }
    bb.x -= 4;
    bb.y -= 4;
    bb.w += 8;
    bb.h += 8;
    itemsBoundingBox = bb;
}

void
ListView::addItem(ListViewItem *item)
{
    items.push_back(item);
    update();
    event_add_node(item);
}

void 
ListView::render()
{
    rendersystem::RenderSystem *renderSystem = Global::instance()->getRenderSystem();


    // renderSystem->startViewport(screenWindow.x,screenWindow.y,
    // 				screenWindow.w,screenWindow.h,
    // 				true, true);

    renderSystem->setColor(1,1,1);
    renderSystem->drawQuad(itemsBoundingBox.x,itemsBoundingBox.y,
    			   itemsBoundingBox.w,itemsBoundingBox.h);
    renderSystem->setColor(0,0,0);
    
    // renderSystem->pushMatrix();


    // float margin = 2;
    // renderSystem->translate((int) (screenWindow.x - itemsBoundingBox.x + margin),
    // 			    (int) (screenWindow.y - itemsBoundingBox.y + margin));

    for (int i=items.size()-1;i>=0;i--)
    {
	ListViewItem *item = items.at(i);
	item->render();
    }

    // renderSystem->popMatrix();


    // Color c1(1,1,1,0);
    // Color c2(1,1,1,1);
    // int size = 40;
    // renderSystem->drawQuadWithColors(
    // 	screenWindow.x + screenWindow.w - size, screenWindow.y,
    // 	size, screenWindow.h,
    // 	c1, c2, c2, c1);

    // renderSystem->endViewport();
}


void
ListView::installAt(rectanglelayout::Node *node)
{
    // should be installed using a ScrollPane
}

ViewRect
ListView::getBoundingBox()
{
    return itemsBoundingBox;
}

bool 
ListView::event_mouse_contains(int x, int y)
{
#ifdef DEBUG_LISTVIEW
    cout << "ListView::event_mouse_contains( " << x << " , " << y << ")" << endl;
    cout << "   Bounding was " << itemsBoundingBox.str() << endl;
#endif
    return itemsBoundingBox.contains(x,y);
}


/**************
 * ScrollPane *
 **************/

ScrollPane::ScrollPane():
    RectangleSlot(),
    component(NULL),
    deltaX(0),
    deltaY(0),
    title("Scroll Pane")
{}


void 
ScrollPane::installAt(rectanglelayout::Node *area)
{
    this->connectToRectangleSignal(area); 

    // if(this->component != NULL)
// 	component->installAt(area);

}

void
ScrollPane::setComponent(Component *c)
{   
    component = c;
    event_add_node(c);
}

void
ScrollPane::updateDeltas()
{   
    // float max_delta_y = screenWindow.y + screenWindow.h - bb.h - bb.y;
    // component = c;
    // implement this
}

void
ScrollPane::setTitle(string title)
{   
    this->title = title;
    // float max_delta_y = screenWindow.y + screenWindow.h - bb.h - bb.y;
    // component = c;
    // implement this
}

void 
ScrollPane::render()
{

#ifdef DEBUG_SCROLLPANE
    cout << "AREEEEEEEEEEER:LIJSDL:FL:GWNM*(#@H*HFOIHNSLFKNOISGH" << endl;
#endif

    rendersystem::RenderSystem *renderSystem = Global::instance()->getRenderSystem();

    float x = screenWindow.x;
    float y = screenWindow.y;
    float w = screenWindow.w;
    float h = screenWindow.h;

    float top_margin = 30;

    renderSystem->startViewport(x, y, w, h,
				true, true);

    renderSystem->setColor(1,1,1);
    renderSystem->drawQuad(screenWindow.x,screenWindow.y,
    			   screenWindow.w,screenWindow.h);


    float xx = x;
    float yy = y;
    float ww = w;
    float hh = h - top_margin;

    ViewRect bb = component->getBoundingBox();

    float tx = -bb.x;
    float ty = yy + hh - bb.h - bb.y;

    tx += deltaX;
    ty += deltaY;
    
    renderSystem->pushMatrix();

    renderSystem->translate((int) tx, (int) ty);

    component->render();

    renderSystem->popMatrix();

    Color c1(1,1,1,0);
    Color c2(1,1,1,1);
    int size = 10;

    renderSystem->drawQuadWithColors(
	screenWindow.x + screenWindow.w - size, screenWindow.y,
	size, screenWindow.h,
	c1, c2, c2, c1);

    renderSystem->drawQuadWithColors(
	screenWindow.x, screenWindow.y,
	screenWindow.w, size,
	c2, c2, c1, c1);

    { // title
	float font_size = 15;
	renderSystem->setColor(ColorConstants::TITLE_BG);
	renderSystem->drawQuad(x,y+h-top_margin,w,top_margin);
    
	renderSystem->setColor(ColorConstants::TITLE_TEXT);
	renderSystem->drawCenteredText(title.c_str(), 
				       font_size, 
				       x, y + h - top_margin,
				       w,top_margin);
    } // title


    renderSystem->endViewport();
}

void 
ScrollPane::event_children_offset(float &offsetX, float &offsetY)
{
#ifdef DEBUG_SCROLLPANE
    cout << "ScrollPane::event_children_offset(...)" << endl;
#endif



    float x = screenWindow.x;
    float y = screenWindow.y;
    float w = screenWindow.w;
    float h = screenWindow.h;
    float top_margin = 30;
    float xx = x;
    float yy = y;
    float ww = w;
    float hh = h - top_margin;

    ViewRect bb = component->getBoundingBox();
    offsetX = -bb.x;
    offsetY = yy + hh - bb.h - bb.y;
    offsetX += deltaX;
    offsetY += deltaY;
    offsetX = -offsetX;
    offsetY = -offsetY;
#ifdef DEBUG_SCROLLPANE
    cout << "   offset: " << offsetX << ", " << offsetY << endl;
#endif
}

void         
ScrollPane::rectangleSignal(Rectangle r)
{
#ifdef DEBUG_SCROLLPANE
    cout << "ListView::receiveRectangleMessage(...)" << endl;
    cout << "   rectangle: " << r << endl;
#endif
    float m = 0;
    screenWindow.init(r.x + m, r.y + m, r.w -2*m, r.h -2*m);
}

ViewRect
ScrollPane::getBoundingBox()
{
    return screenWindow;
}

bool 
ScrollPane::event_mouse_contains(int x, int y)
{
#ifdef DEBUG_SCROLLPANE
    cout << "ScrollPane::event_mouse_contains( " << x << " , " << y << ")" << endl;
    cout << "   Bounding was " << screenWindow.str() << endl;
#endif
    return screenWindow.contains(x,y);
}


bool 
ScrollPane::event_mouse_wheel(eventsystem::State *event_state)
{
    if (event_state->wheel_dy < 0)
	deltaY += 15;
    else
	deltaY -= 15;
}



/***************
 * Spreadsheet *
 ***************/

Spreadsheet::Spreadsheet()
{
#ifdef DEBUG_SPREADSHEET
    cout << "Spreadsheet::Spreadsheet(...)" << endl;
#endif
    RectangleLayout *rl = &rectangle_layout;
    rectanglelayout::Grid *root = 
	rl->subdivide(rectangle_layout.getRoot()->asLeaf(),2,1);
    root->setXStop(1,0.14);

    rectanglelayout::Leaf *left = rl->getNode(root,0,0)->asLeaf();
    rectanglelayout::Grid* left_grid = rl->subdivide(left,1,2);
    left_grid->setYStop(1,0.8);

    this->env      = NULL;
}

void
Spreadsheet::clear()
{
    if (components.size() > 2)
    {
	event_del_node(components[2]);
	components.erase(components.begin()+2);
    }

    RectangleLayout *rl = &rectangle_layout;
    rectanglelayout::Node *node = 
	rl->getNode(rectangle_layout.getRoot()->asGrid(),1,0);
    // event_del_node(installed
    rl->deleteNode(node);

    // c->installAt(area);
    // components.push_back(c);
    // emitRectangleLayoutSignals();

    // // 
    // event_add_node(c);
}

void 
Spreadsheet::resize(float x, float y, float w, float h)
{
#ifdef DEBUG_SPREADSHEET
    cout << "Spreadsheet::resize(...)" << endl;
#endif

    rectangle(x,y,w,h);

    this->emitRectangleLayoutSignals();
    // cout <<  "   " << rectangle << endl;
    // emit update signal to everyone
}

void 
Spreadsheet::installComponent(Component *c, rectanglelayout::Node *area)
{
#ifdef DEBUG_SPREADSHEET
    cout << "Spreadsheet::installComponent" << endl;
#endif
    if (area == NULL)
    {
	// find first leaf area
	area = rectangle_layout.getNode(rectangle_layout.getRoot()->asGrid(),0,0);
    }

#ifdef DEBUG_SPREADSHEET
    cout << "   c->installAt(area)" << endl;
#endif

    c->installAt(area);
    components.push_back(c);
    emitRectangleLayoutSignals();

    // 
    event_add_node(c);

}

void 
Spreadsheet::installComponent(Component *c, int xcell, int  ycell)
{
    rectanglelayout::Node *area = rectangle_layout.getNode(
	rectangle_layout.getRoot()->asGrid(),xcell,ycell);

#ifdef DEBUG_SPREADSHEET
    cout << "   c->installAt(area)" << endl;
#endif

    c->installAt(area);
    components.push_back(c);
    emitRectangleLayoutSignals();

    // 
    event_add_node(c);
}

void 
Spreadsheet::installComponent(Component *c, int *cell_path, int  depth)
{
    rectanglelayout::Node *node = rectangle_layout.getRoot();
    for (int i=0;i<depth;i++)
    {
	int x = cell_path[ 2*i     ];
	int y = cell_path[ 2*i + 1 ];
	node = rectangle_layout.getNode(node->asGrid(),x,y);
    }
    
#ifdef DEBUG_SPREADSHEET
    cout << "   c->installAt()" << endl;
#endif

    c->installAt(node);
    components.push_back(c);
    emitRectangleLayoutSignals();

    // 
    event_add_node(c);
}

void
Spreadsheet::set_map_bg_color(eventsystem::State *event_state) 
{
    Color  c;
    QColor color_in;
    QColor color_out;

    c = *ColorConstants::MAP_BG;
    color_in.setRgbF(c.r,c.g,c.b);
    color_out = QColorDialog::getColor(color_in, NULL);
    if (color_out.isValid())
    {
	ColorConstants::MAP_BG->init(color_out.redF(),
				     color_out.greenF(),
				     color_out.blueF(),
				     1.0);
	emitRectangleLayoutSignals();
    }
}

void
Spreadsheet::set_sel_transparency(eventsystem::State *event_state) 
{
    bool ok;

    double transparency = ColorConstants::SEL_TRANSPARENCY;
    
    stringstream ss("");
    ss << transparency;


    QString text = QInputDialog::getText(NULL, 
					 QDialog::tr("QInputDialog::getText()"),
					 QDialog::tr("Stop Values:"), 
					 QLineEdit::Normal,
					 QDialog::tr(ss.str().c_str()), 
					 &ok);
    if (!ok || text.isEmpty())
	return;

    string exp = text.toStdString();

    bvdata::Repository* R = Global::instance()->getRepository();
    
    // read stops
    try{
	transparency = R->evalDouble(exp);
	ColorConstants::SEL_TRANSPARENCY = transparency;
	emitRectangleLayoutSignals();
    }
    catch(...){
	cout << "   Invalid R expression to generate stops" << endl;
	return;
    }
}



bool 
Spreadsheet::event_key_press(eventsystem::State *event_state) 
{
    switch(event_state->key)
    {
    case eventsystem::Key_B:
	this->set_map_bg_color(event_state);
	break;
    case eventsystem::Key_T:
	this->set_sel_transparency(event_state);
	break;
    default:
	return false;
    }
    return false;
}

void 
Spreadsheet::emitRectangleLayoutSignals()
{
#ifdef DEBUG_SPREADSHEET_RECTANGLE_LAYOUT_SIGNALS
    cout << "Spreadsheet::emitRectangleLayoutSignals()" << endl;
#endif

    // iterate over the leaves
    RectangleLayout::Iterator it(rectangle_layout.getRoot(), rectangle);
    RectangleLayout::Iterator::Item* item;
    while (1)
    {
        item = it.next();
        if (item == NULL)
    	    break;

	float m = 1;
	Rectangle r = item->rect();

#ifdef DEBUG_SPREADSHEET_RECTANGLE_LAYOUT_SIGNALS
	cout << "   rectangle: " << r << endl;
#endif

	float x0 = r.x;
	float x1 = r.x + r.w;
	float y0 = r.y;
	float y1 = r.y + r.h;

	r.x  = floor(x0);
	r.w  = floor(x1) - r.x;
	r.x += m;
	r.w -= 2*m;
	
	r.y  = floor(y0);
	r.h  = floor(y1) - r.y;
	r.y += m;
	r.h -= 2*m;

	item->node->emitRectangleSignal(r);
    }
}

void 
Spreadsheet::render()
{
    // cout << "Spreadsheet::render()" << endl;

    RenderSystem* renderSystem = Global::instance()->getRenderSystem();
    
    // this should be called through the render system
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderSystem->startViewport(rectangle.x,rectangle.y,
				rectangle.w,rectangle.h,
				false, true);
    // iterate over the leaves
    RectangleLayout::Iterator it(rectangle_layout.getRoot(), rectangle);
    RectangleLayout::Iterator::Item* item;
    while (1)
    {
        item = it.next();
        if (item == NULL)
    	    break;

	if (item->node->isLeaf())
	{

	    float m = 1;
	    Rectangle r = item->rect();
	    r.x += m;
	    r.y += m;
	    r.w -= 2 * m;
	    r.h -= 2 * m;
	    // item->node->emitRectangleSignal(r);

	    // draw background rectangle
	    renderSystem->setColor(.1,.1,.1);
	    renderSystem->drawQuad(r.x, r.y, 
	     			   r.w, r.h);
	}
    }
    renderSystem->endViewport();


    for (int i=0;i<components.size();i++)
    {
	Component *c = components.at(i);
	c->render();
    }

}

eventsystem::PickableObject*
Spreadsheet::event_mouse_pick(eventsystem::State *event_state)
{
    changing_cells_dimension = rectangle_layout.findHandle(
	event_state->x, event_state->y, 
	rectangle, changing_cells_dimension_handle);

    if (changing_cells_dimension)
	return this;
    else
	return NULL;
}

void 
Spreadsheet::event_mouse_drag(eventsystem::State *event_state)
{
    if (changing_cells_dimension)
    {
	float dx = event_state->x - event_state->last_x;
	float dy = event_state->y - event_state->last_y;
	changing_cells_dimension_handle.move(dx,dy);
	this->emitRectangleLayoutSignals();
    }
}

void 
Spreadsheet::event_mouse_release(eventsystem::State *event_state)
{
    changing_cells_dimension = false;
}


bool 
Spreadsheet::event_mouse_contains(int x, int y)
{
#ifdef DEBUG_SPREADSHEET
    cout << "Spreadsheet::event_mouse_contains( " << x << " , " << y << ")" << endl;
    cout << "   Bounding was " << rectangle << endl;
#endif
    return rectangle.contains(x,y);
}


bool 
Spreadsheet::event_mouse_drop(eventsystem::State *event_state)
{
    eventsystem::PickableObject *po = event_state->picked_object;
    ListViewItem *lvi = dynamic_cast<ListViewItem*>(po);

    if (lvi == NULL)
    {
	return false;
    }

    // void* ptr = ;
    string data;
    try {
	data = boost::any_cast<string>(lvi->getCustomObject());
    }
    catch(const boost::bad_any_cast)
    {
	return false;
    }

    cout << data << endl;


    if (event_state->shift) 
    {
	this->clear();
	return false;
    }

    bool ok;
    int rows=1;
    int cols=1;
    QString st;
    st = QInputDialog::getText(Global::instance()->main_widget, 
			       "Spreadsheet",
			       "Rows and Columns:", 
			       QLineEdit::Normal,
			       "1 1", 
			       &ok);
    if (ok && !st.isEmpty())
    {
	try{
	    sscanf(st.toStdString().c_str(),"%d %d",&rows, &cols);
	}		
	catch(std::exception& e)
	{
	    cout << "catched exception\n";
	    return false;
	}
	this->clear();
	     
    }

    //
    if(this->env != NULL)
    {
	OccurrenceEnvironment *occ_env = dynamic_cast<OccurrenceEnvironment*>(this->env);   
     	SimpleEnvironmentMatrix   *sem = dynamic_cast<SimpleEnvironmentMatrix*>(this->env);

	if(occ_env != NULL){
	    cout << "This is a Occurrence Environment" << endl;
	    delete occ_env;
	}
	else if(sem != NULL){
	    cout << "This is a Simple Environment Matrix" << endl;
	    delete sem;
	}

    }

        if (data.compare("Single Species") == 0)
        {
	    OccurrenceEnvironment* occ_env = new OccurrenceEnvironment(cols, rows, 1);
	    this->env = occ_env;
	    this->installComponent( occ_env, 1, 0 );
	}
	else if (data.compare("Single Species (Variation)") == 0)
	{
	    OccurrenceEnvironment* occ_env = new OccurrenceEnvironment(cols, rows, 2);
	    this->env = occ_env;
	    this->installComponent( occ_env, 1, 0 );
	}
	else if (data.compare("Multi-Species") == 0)
	{
	    SimpleEnvironmentMatrix* simple_env = 
		new SimpleEnvironmentMatrix(rows,cols);
	    this->env = simple_env;
	    this->installComponent( simple_env, 1, 0 );
	}

    

    return true;
}



//     bvdata::File *f = (bvdata::File *) lvi->getCustomObject();

//     if (f == NULL)
// 	return false;

//     mod_file.setFile(f);

//     f->load();

//     mod_title_renderer.setTitle("Species: "+f->getName());

//     if (num_expressions > 0)
//     {
// 	string expression = f->shortname+"$p[[t]]";
// 	mod_expressions[0]->setString(expression);

// 	mod_integral_curves[0]->setExpression(
// 	    "bv.curve("+f->shortname+"$p,xmin,xmax,ymin,ymax)");

//     }
//     if (num_expressions > 1)
//     {
// 	string expression = 
// 	    f->shortname+"$p[[t]] - " + 
// 	    f->shortname+"$p[[ if(t==1) t else t-1 ]]";
// 	mod_expressions[1]->setString(expression);


// 	mod_integral_curves[1]->setExpression(
// 	    "c=bv.curve("+f->shortname+"$p,xmin,xmax,ymin,ymax);c[2:52]=c[2:52]-c[1:51];c");
// 	// mod_integral_curves[1]->setExpression(
// 	//     "bv.curve("+f->shortname+"$sd,xmin,xmax,ymin,ymax)");
//     }

//     return true;


// #ifdef DEBUG_SPREADSHEET
//     cout << "Spreadsheet::event_mouse_contains( " << x << " , " << y << ")" << endl;
//     cout << "   Bounding was " << rectangle << endl;
// #endif
//     return rectangle.contains(x,y);



