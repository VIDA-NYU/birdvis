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

#ifndef GLOBAL_H
#define GLOBAL_H

#include "GL/glew.h"

#include "datatypes.hpp"

#include "RenderSystem.hpp"

#include "Array.hpp"

#include "bvdata.hpp"

#include "Regions.hpp"

#include "Color.hpp"

#include "Texture.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>


#include <QtGui>
#include <QWidget>

using namespace boost::gregorian;

using namespace datatypes;

////////////////////////////////////////////////////////////////////////////////////////////////
// 


class ColorConstants
{
public:
    static Color* COLORMAP_BG;
    static Color* TITLE_BG;
    static Color* TITLE_TEXT;
    static Color* PLOT_BG;
    static Color* PLOT_GRID;
    static Color* PLOT_TEXT;
    static Color* PLOT_CURSORS;
    static Color* PLOT_TITLE_BG;
    static Color* PLOT_TITLE_TEXT;
    static Color* PLOT_FRAME_BG;
    static Color* MAP_BG;
    static Color* MAP_BORDERS;
    static Color* MAP_BTN_BG;
    static Color* MAP_BTN_TEXT;
    static Color* MAP_BTN_ON;
    static Color* MAP_BTN_OFF;

    static float  SEL_TRANSPARENCY;
    static bool   NORMALIZE_CURVES;

    static float  NORMALIZE_CURVES_MIN_PLOT_Y;
    static float  NORMALIZE_CURVES_MAX_PLOT_Y;

};

class Global {

private:

    static Global *s_instance;

    Global();
    ~Global();


public:

    // Define on the initializeGL
    rendersystem::RenderSystem      *render_system;

    // Repository
    bvdata::Repository              *repository;

public:
    QWidget *main_widget;

    // GDAL files describing the boundaries of states and counties
    RegionSet                       *usa_states;
    RegionSet                       *usa_counties;

    // some installed opengl call lists
    GLuint                           usa_states_gl_list;
    GLuint                           usa_counties_gl_list;
    GLuint                           usa_area_gl_list;

    Texture*                         bv_logo;

public:

    static Global*               instance();

    // ViewRect                     xy_ranges();

    void                         initializeRenderSystem(string path);

    rendersystem::RenderSystem*  getRenderSystem();

    bvdata::Repository*          getRepository();

    void                         initializeUSAMapsAndInstallCallLists(string path);

    string                       getRegionName(double lon, double lat);

    void                         keyword_bounds(string keywords, float *xywh);

    void                         open_google_maps_link(ViewRect ww);

    void                         open_image_file(string filename);

    double                       estimateUSAreaCoefficientAtRectangle(double x, double y, double w, double h);

};


#endif
