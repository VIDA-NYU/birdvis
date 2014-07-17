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

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <iostream>

#include "datatypes.hpp"

#include "Vec2.hpp"

using namespace datatypes;

// Point
Point::Point(){
    this->x_coord = 0;
    this->y_coord = 0;
    this->scalar = 0;
}

Point::Point(double x, double y, double v){
    this->x_coord = x;
    this->y_coord = y;
    this->scalar = v;
}

Point::~Point(){
}

double Point::getXCoord(){
    return this->x_coord;
}

double Point::getYCoord(){
    return this->y_coord;
}

double Point::getScalar(){
    return this->scalar;
}

// PointSet
vector<Point*> *points;
	
PointSet::PointSet(){
    this->points = new vector<Point*>();
}

PointSet::~PointSet(){
    if(this->points != NULL)
	delete this->points;
}

void PointSet::addPoint(Point *point){
    this->points->push_back(point);
}

Point* PointSet::getPoint(int i){
    if((i < 0) || (i >= this->points->size()))
	return NULL;
    return this->points->at(i);
}

int PointSet::getNumPoints(){
    return this->points->size();
}

// Predictor

Predictor::Predictor(){
    this->name = string("");
    this->pointSets = new vector<PointSet*>();
    this->curve = NULL;
}

Predictor::Predictor(string name){
    this->name = name;
    this->pointSets = new vector<PointSet*>();
    this->curve = NULL;
}

Predictor::Predictor(string name, int num_time_steps) {
    this->name = name;
    this->pointSets = new vector<PointSet*>();
    for(int i = 0 ; i < num_time_steps ; i++){
	this->pointSets->push_back(new PointSet());
    }
    this->curve = NULL;
}


Predictor::~Predictor(){
    if(this->pointSets != NULL)
	delete pointSets;
}

int Predictor::getNumPointSets(){
    return this->pointSets->size();
}

PointSet* Predictor::getPointSet(int i){
    if((i < 0) || (i >= this->pointSets->size()))
	return NULL;
    return this->pointSets->at(i);
}





string Predictor::getName(){
    return this->name;
}

void Predictor::addPointSet(PointSet* pointSet){
    this->pointSets->push_back(pointSet);
}

Curve* Predictor::getCurve(){
    
    if (this->curve != NULL)
	return this->curve;

    //check if this is right?????

    int numPointSets = this->getNumPointSets();

    double data[numPointSets];

    for(int i = 0 ; i < numPointSets ; i++){
	data[i] = 0;
    }


    for(int i = 0 ; i < numPointSets ; i++){
	PointSet* ps = this->getPointSet(i);

	int numPoints = ps->getNumPoints();


	for(int j = 0 ; j < numPoints  ; j++){
	    data[i] += (ps->getPoint(j)->getScalar());
	}

	data[i] = data[i]/numPoints;
    }

    string dimName[1] = {"Time"};

    ConcreteArray * array =  
	new ConcreteArray(1, dimName, &numPointSets, data);

    double color[4];
    color[0] = 1.0;
    color[1] = 1.0;
    color[2] = 1.0;    
    color[3] = 1.0;

    Curve* curve = new Curve(array, color);

    this->curve = curve;

    return curve;
}

Curve* Predictor::getCurveSel(selection::Selection *sel){
    
    //check if this is right?????

    int numPointSets = this->getNumPointSets();

    double data[numPointSets];

    for(int i = 0 ; i < numPointSets ; i++){
	PointSet* ps = this->getPointSet(i);

	int numPoints = ps->getNumPoints();

	for(int j = 0 ; j < numPoints  ; j++){
	    Point* point = ps->getPoint(i);
	    if(sel->contains(point->getXCoord(),point->getYCoord()))
		data[i] += (point->getScalar());
	}

	data[i] = data[i]/numPointSets;
    }

    string dimName("Time");

    ConcreteArray * array =  new ConcreteArray(1, &dimName, 
					       &numPointSets, data);

    double color[4];
    color[0] = 1.0;
    color[1] = 1.0;
    color[2] = 1.0;    
    color[3] = 1.0;

    Curve* curve = new Curve(array, color);

}

// class ViewRect

// class Colormap1d::Stop


/// Selection
	
int datatypes::selection::Selection::ID_COUNT = 0;

 
