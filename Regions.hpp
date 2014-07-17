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

// RegionSet: maps of geographical regions and its names (GDAL)

#ifndef REGIONS_HPP
#define REGIONS_HPP

#include <vector>
#include <GL/glew.h>

#ifdef __APPLE__
#include <GDAL/ogrsf_frmts.h>
#else
//#include <gdal/ogrsf_frmts.h>
#include <ogrsf_frmts.h>
#endif

/*************
 * RegionSet *
 *************/

class RegionSet {

private:

    std::vector<std::string> names;

    std::vector<OGRGeometry*> regions;

public:

    RegionSet();

    int size();

    std::string getName(int index);

    OGRGeometry* getRegion(int index);

    void addRegionsFromFile(std::string filename, std::string layer_name, int fields);

    void gl_draw_region_lines(int index);

    void gl_draw_region_triangles(int index);

    void gl_draw_all_region_triangles();

    void gl_draw_all_region_lines();

};



#endif


