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

#include "Regions.hpp"

#include "Triangulator.h"

#include <iostream>
#include <sstream>

using namespace std;

#define xDEBUG_REGIONSET

/*************
 * RegionSet *
 *************/

RegionSet::RegionSet() {};

int 
RegionSet::size() 
{ 
    return regions.size(); 
}

string 
RegionSet::getName(int index) 
{ 
    return names.at(index); 
}

OGRGeometry* 
RegionSet::getRegion(int index) 
{ 
    return regions.at(index); 
}

void
RegionSet::addRegionsFromFile(string filename, string layer_name, int fields)
{

    RegionSet *result = this;

    OGRRegisterAll();
    OGRDataSource       *poDS;
    poDS = OGRSFDriverRegistrar::Open(filename.c_str(), FALSE );
    if( poDS == NULL )
    {
        printf("Open failed. (%s)\n",filename.c_str());
        exit( 1 );
    }
    OGRLayer  *poLayer;
    poLayer = poDS->GetLayerByName(layer_name.c_str());

    OGRFeature *poFeature;
    poLayer->ResetReading();

    // create list of geometries
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
	stringstream name;
	bool added = false;
	if (fields & (1 << 0)) {
	    name << poFeature->GetFieldAsString(0);
	    added = true;
	}
	if (fields & (1 << 1)) {
	    if (added)
		name << ", ";
	    name << poFeature->GetFieldAsString(1);
	}
	if (fields & (1 << 2)) {
	    if (added)
		name << ", ";
	    name << poFeature->GetFieldAsString(1);
	}


	size_t found;

	found = name.str().find("Alaska");
	if (found != string::npos)
	    continue;

	found = name.str().find("Hawaii");
	if (found != string::npos)
	    continue;


	result->names.push_back(name.str());

        // OGRFeature::DestroyFeature( poFeature );





	// we know the field 1 is always the name of the region...
	result->regions.push_back(poFeature->GetGeometryRef());

    }
    // OGRDataSource::DestroyDataSource( poDS );
}

void
RegionSet::gl_draw_region_lines(int index)
{

    OGRGeometry *g = this->getRegion(index);

    if (g == NULL)
	return;

    vector<OGRGeometry*> stack;

    stack.push_back(g);

    while (stack.size() > 0)
    {

	OGRGeometry* g = stack.back();
	stack.pop_back();

	if (wkbFlatten(g->getGeometryType()) == wkbMultiPolygon) {
	    OGRMultiPolygon *mp = (OGRMultiPolygon*) g;
	    for (int i=0;i<mp->getNumGeometries();i++) {
		// printf("polygon\n");
		stack.push_back(mp->getGeometryRef(i));
	    }
	}

	else if (wkbFlatten(g->getGeometryType()) == wkbPolygon) {
	    OGRLinearRing *r = ((OGRPolygon*) g)->getExteriorRing();
	    OGRPoint p;
	    // glEnable(GL_BLEND);
	    // glBlendFunc(GL_ONE,GL_ONE);
	    glBegin(GL_LINE_LOOP); //starts drawing of points
	    for (int i=0;i<r->getNumPoints();i++)
	    {
		double x = r->getX(i);
		double y = r->getY(i);
		// printf("%f %f\n",x,y);
		// if (i % 2) // ) == 0 || (i % 4) == 1)
		// 	glColor3f(1,1,1);
		// else
		glVertex2f(x,y);

	    }
	    glEnd();//end drawing of points
	    // glDisable(GL_BLEND);

	}
    }
}

void 
RegionSet::gl_draw_region_triangles(int index)
{

    OGRGeometry *g = this->getRegion(index);

    if (g == NULL)
	return;

    vector<OGRGeometry*> stack;

    stack.push_back(g);

    while (stack.size() > 0)
    {

	OGRGeometry* g = stack.back();
	stack.pop_back();


	if (wkbFlatten(g->getGeometryType()) == wkbMultiPolygon) {
	    OGRMultiPolygon *mp = (OGRMultiPolygon*) g;
	   
	    for (int k=0;k<mp->getNumGeometries();k++) {
		stack.push_back(mp->getGeometryRef(k));
	    }
	}

	else if (wkbFlatten(g->getGeometryType()) == wkbPolygon) {

	    // draw only the exterior ring!

	    OGRLinearRing *r = ((OGRPolygon*) g)->getExteriorRing();
	    
	    Triangulator *triangulator = createTriangulator();
	    
	    double x,y,z;

	    for (int k=0;k<r->getNumPoints() - 1;k++)
	    {		
		x = r->getX(k);
		y = r->getY(k);
		z = 0.0;
		triangulator->addPoint(x,y,z);
	    }

	    unsigned int tcount;
	    unsigned int *indices = triangulator->triangulate(tcount);

	    glBegin(GL_TRIANGLES);
	    for (int k=0;k<3 * tcount;k++)
	    {
	    	int index = indices[k];
	    	triangulator->getPoint(index,x,y,z);
		glVertex2f(x,y);
	    }
	    glEnd();

	    // release triangulator
	    releaseTriangulator(triangulator);

	}
    }
}

void 
RegionSet::gl_draw_all_region_triangles()
{
    for(int i=0;i<regions.size();i++)
	this->gl_draw_region_triangles(i);
}

void 
RegionSet::gl_draw_all_region_lines()
{
    for(int i=0;i<regions.size();i++)
	this->gl_draw_region_lines(i);
}
