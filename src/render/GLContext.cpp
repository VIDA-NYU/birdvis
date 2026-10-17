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

#include <GL/glew.h>

#include <QApplication>
#include <QDir>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include <stdexcept>

#include <iostream>

#include "Global.hpp"

#include "GLContext.hpp"

#include "Environment.hpp"
#include "SimpleEnvironment.hpp"



using namespace std;

GLContext::GLContext(QWidget *parent):
    QOpenGLWidget(parent)
{
    cout << "GLContext::GLContext(...)" << endl;

    setWindowTitle(tr("BirdVis"));
    setFocusPolicy(Qt::StrongFocus);

    if (format().stencilBufferSize() <= 0)
	qWarning("Could not get stencil buffer; results will be suboptimal");
    else 
	cout << "   stencil works" << endl;

    // track mouse movement
    this->setMouseTracking(true);

    cout << "   creating event manager...";
    event_manager = new eventsystem::Manager();
    cout << "   done" << endl;

    cout << "   creating spreadsheet...";
    spreadsheet = new Spreadsheet();
    cout << "   done" << endl;

    cout << "   registering spreadsheet on event_manager...";
    event_manager->event_add_node(spreadsheet);
    cout << "   done" << endl;

}

GLContext::~GLContext()
{
    if(context()) disconnect(context(),nullptr,this,nullptr);
    cleanupGL();
}

void GLContext::cleanupGL() {
    if(!ready) return;
    makeCurrent();
    Global::instance()->releaseGLResources();
    ready=false;
    doneCurrent();
}


QSize 
GLContext::minimumSizeHint() const
{
    return QSize(50, 50);
}


QSize 
GLContext::sizeHint() const
{
    return QSize(400, 400);
}


void 
GLContext::initializeGL() 
{
    try {
    // initialize GLEW to allow FBO (offscreen rendering on frame buffer objects)
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
	/* Problem: glewInit failed, something is seriously wrong. */
	fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    if(err!=GLEW_OK || !GLEW_EXT_framebuffer_object || format().stencilBufferSize()<8)
        throw std::runtime_error("BirdVis requires desktop OpenGL 2.1 with framebuffer objects and an 8-bit stencil buffer");
    while(glGetError()!=GL_NO_ERROR) {}
    connect(context(),&QOpenGLContext::aboutToBeDestroyed,this,&GLContext::cleanupGL,Qt::DirectConnection);
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));



    Global::instance()->main_widget = this;

    cout << "GLContext::initializeGL()" << endl;

    string path = QDir(qEnvironmentVariable("BIRDVIS_RESOURCES", BIRDVIS_RESOURCE_DIR)).absolutePath().toStdString() + "/";

    cout << "   path: " << path << endl;
    cout << "   initializing render system...";
    // initialize rendering system
    Global::instance()->initializeRenderSystem(path);
    cout << "   done." << endl;

    cout << "   initializing USA maps...";
    // initialize rendering system
    Global::instance()->initializeUSAMapsAndInstallCallLists(path);
    cout << "   done." << endl;

    //
    // this->setAutoFillBackground(false);


    ready=true;
    if(uiInstalled) return;
    uiInstalled=true;
    { // install files view

	bvdata::Repository* repo = Global::instance()->getRepository();

	ListView* files_view = new ListView();
	for (int i=0;i<repo->files.size();i++)
	{
	    bvdata::File *f = repo->files.at(i);
	    // if (f->shortname == "ib")
	    // f->load();
	    ListViewItem *item = new ListViewItem(f->getName()+" ("+f->shortname+")",13);
	    item->setCustomObject(f);
	    files_view->addItem(item);
	}
	
	int celladdr[] = { 0, 0, 0, 0};
	ScrollPane *scrollPane = new ScrollPane();
	scrollPane->setTitle("Species");
	scrollPane->setComponent(files_view);
	spreadsheet->installComponent( scrollPane, celladdr, 2);
    }


    { // Environment view

    	string names[] = {  "Single Species", 
    			    "Multi-Species",
			    "Single Species (Variation)" };

    	ListView* environments_list_view = new ListView();
    	for (int i=0;i<3;i++)
	{
    	    ListViewItem *item = new ListViewItem(names[i],13);
	    environments_list_view->addItem(item);
	    item->setCustomObject(names[i]);
	}

	int celladdr[] = { 0, 0, 0, 1};
	ScrollPane *scrollPane = new ScrollPane();
	scrollPane->setTitle("Environments");
	scrollPane->setComponent(environments_list_view);
	spreadsheet->installComponent( scrollPane, celladdr, 2);
    }
    // spreadsheet->emitRectangleLayoutSignals();




    // 	// string names[] = {  "Elevation", 
    // 	// 		    "Open Water" , 
    // 	// 		    "Ice-Snow", 
    // 	// 		    "Developed-Open", 
    // 	// 		    "Developed-Low",  
    // 	// 		    "Developed-Med", 
    // 	// 		    "Developed-High", 
    // 	// 		    "Barren Land",  
    // 	// 		    "Forest-Deciduous", 
    // 	// 		    "Forest-Evergreen",	
    // 	// 		    "Forest-Mixed",  
    // 	// 		    "Shrub-scrub", 
    // 	// 		    "Grassland", 
    // 	// 		    "Pasture",  
    // 	// 		    "Cultivated Crops", 
    // 	// 		    "Woody Wetlands",   
    // 	// 		    "Emergent Wetlands" };

    // 	// ListView* predictors_view = new ListView();
    // 	// for (int i=0;i<17;i++)
    // 	//     predictors_view->addItem(new ListViewItem(names[i],13));

    // 	// {
    // 	//     int celladdr[] = { 0, 0, 0, 0 };
    // 	//     ScrollPane *scrollPane = new ScrollPane();
    // 	//     scrollPane->setComponent(predictors_view);
    // 	//     spreadsheet->installComponent( scrollPane, celladdr, 2);
    // 	// }
    // }


//     cout << "   creating ScrollPane..." << endl;
//    ScrollPane *scrollPane = new ScrollPane();
//     scrollPane->setComponent(list_view);
//     spreadsheet->installComponent( scrollPane, 0, 0 );

//     ListView* list_view = new ListView();
//     string names[] = {  "Elevation", 
//     			"Open Water" , 
//     			"Ice-Snow", 
//     			"Developed-Open", 
//     			"Developed-Low",  
//     			"Developed-Med", 
//     			"Developed-High", 
//     			"Barren Land",  
//     			"Forest-Deciduous", 
//     			"Forest-Evergreen",	
//     			"Forest-Mixed",  
//     			"Shrub-scrub", 
//     			"Grassland", 
//     			"Pasture",  
//     			"Cultivated Crops", 
//     			"Woody Wetlands",   
//     			"Emergent Wetlands" };

//     for (int i=0;i<17;i++)
//     	list_view->addItem(new ListViewItem(names[i],13));
    
//     cout << "   creating ScrollPane..." << endl;
//     ScrollPane *scrollPane = new ScrollPane();
//     scrollPane->setComponent(list_view);
//     int celladdr[] = { 0, 0, 0, 2 };
//     spreadsheet->installComponent( scrollPane, celladdr, 2 );


//     cout << "   creating OccurrenceEnvironment..." << endl;
// #if 1
//     OccurrenceEnvironment* occ_env = new OccurrenceEnvironment();
//     spreadsheet->installComponent( occ_env, 1, 0 );
// #else
//     SimpleEnvironment* simple_env = new SimpleEnvironment();
//     spreadsheet->installComponent( simple_env, 1, 0 );
// #endif

    // spreadsheet->clear();


    } catch(const std::exception &error) {
        ready=false;
        qCritical() << "BirdVis graphics initialization failed:" << error.what();
        QTimer::singleShot(0,qApp,[]{ QCoreApplication::exit(1); });
    }

}


void 
GLContext::paintGL()
{
    if(!ready) return;
    // cout << "GLContext::paintGL()" << endl;

    Global::instance()->getRenderSystem()->setPixelRatio(devicePixelRatioF());
    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilMask(~0u);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // render spreadsheet
    spreadsheet->render();


    // RenderSystem* renderSystem = Global::instance()->getRenderSystem();
    
    // // this should be called through the render system
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // renderSystem->startViewport(0,0,this->width(),this->height(),
    // 				false, true);
    // // draw background rectangle
    // renderSystem->setColor(.4,.0,.0);
    // renderSystem->drawQuad(0,0,this->width(),this->height());

    // renderSystem->endViewport();


    RenderSystem* renderSystem = Global::instance()->getRenderSystem();
    
    // this should be called through the render system
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderSystem->startViewport(0,0,this->width(),this->height(),
    				false, true);

    event_manager->event_render();

    renderSystem->endViewport();
   



    // draw background rectangle
    // renderSystem->setColor(.4,.0,.0);
    // renderSystem->drawQuad(0,0,this->width(),this->height());

    // glViewport(0,0,this->width(),this->height());
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // glOrtho(0,this->width(),0,this->height(), -1, 1);
    // glEnable(GL_BLEND);
    // glDisable(GL_BLEND);


}

void 
GLContext::resizeGL(int width, int height)
{
    spreadsheet->resize(0,0,this->width(), this->height());
    this->update();

    // canvas->resize_screen(this->width(), this->height());
    // this->update();
} 

void 
GLContext::mousePressEvent(QMouseEvent *event)
{
    makeCurrent();
    { // mouse manager stuff
	
    	bool shift = (event->modifiers() & Qt::ShiftModifier);
    	bool alt   = (event->modifiers() & Qt::AltModifier);
    	bool ctrl  = (event->modifiers() & Qt::ControlModifier);

        int x = event->position().x();
        int y = this->height() - event->position().y();

    	eventsystem::Button b = eventsystem::NONE;
    	if (event->button() == Qt::LeftButton)
    	    b = eventsystem::LEFT;
    	if (event->button() == Qt::RightButton)
    	    b = eventsystem::RIGHT;
        if (event->button() == Qt::MiddleButton)
    	    b = eventsystem::MID;

        event_manager->mouse_move(x,y);
    	event_manager->mouse_press(x,y,b,shift,ctrl,alt);

    } // mouse manager stuff
    
    this->update();
}

void 
GLContext::mouseMoveEvent(QMouseEvent *event)
{
    makeCurrent();

    { // mouse manager stuff
	
        int x = event->position().x();
        int y = this->height() - event->position().y();

    	event_manager->mouse_move(x,y);

    } // mouse manager stuff

    this->update();

}

void 
GLContext::mouseReleaseEvent(QMouseEvent *event)
{
    makeCurrent();

    { // mouse manager stuff
	
    	bool shift = (event->modifiers() & Qt::ShiftModifier);
    	bool alt   = (event->modifiers() & Qt::AltModifier);
    	bool ctrl  = (event->modifiers() & Qt::ControlModifier);

        int x = event->position().x();
        int y = this->height() - event->position().y();

    	eventsystem::Button b = eventsystem::NONE;
    	if (event->button() == Qt::LeftButton)
    	    b = eventsystem::LEFT;
    	if (event->button() == Qt::RightButton)
    	    b = eventsystem::RIGHT;
        if (event->button() == Qt::MiddleButton)
    	    b = eventsystem::MID;

        event_manager->mouse_move(x,y);
    	event_manager->mouse_release(x,y,b,shift,ctrl,alt);

    } // mouse manager stuff


    // this->selectionMode = false;
    this->update();

}

void 
GLContext::wheelEvent(QWheelEvent *event)
{
    makeCurrent();
    { // mouse manager stuff
	
        // Legacy controllers use dx for zoom and dy for scrolling.
        event_manager->mouse_move(event->position().x(), height()-event->position().y());
        const QPoint delta = event->angleDelta().isNull() ? event->pixelDelta() : event->angleDelta();
        const int step = delta.y() != 0 ? delta.y() : delta.x();
        if (step != 0) event_manager->mouse_wheel(step, step);

    } // mouse manager stuff

    this->update();

}

void 
GLContext::keyPressEvent(QKeyEvent *event)
{
    makeCurrent();

    // cout << "---------------------------- Qt Keyboard event" << endl;

    { // event manager

    	bool shift = (event->modifiers() & Qt::ShiftModifier);
    	bool alt   = (event->modifiers() & Qt::AltModifier);
    	bool ctrl  = (event->modifiers() & Qt::ControlModifier);

    	eventsystem::Key k = (eventsystem::Key) event->key();

    	event_manager->key_press(k,shift,ctrl,alt);

    } // end event manager

    this->update();

}
