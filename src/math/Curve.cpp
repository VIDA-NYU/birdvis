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

#include "Curve.hpp"

using namespace datatypes;

Curve::Curve(){
}

Curve::Curve(ConcreteArray *graph, double col[4]){
 
    this->graph = graph;//.assign(graph.begin(), graph.end());
      
    this->color[0] = col[0];
    this->color[1] = col[1];
    this->color[2] = col[2];    
    this->color[3] = col[3];

    double min, max;
    graph->getRange(&min, &max);

    this->min = min;
    this->max = max;
}

Curve::~Curve()
{
    if(this->graph != NULL){
	cout << "deleting curve" << endl;
	delete this->graph;
    }
}
           
int Curve::getNumPoints(){
    return this->graph->getDimensionSize(0);
}

double Curve::getValue(int index){
    return this->graph->getValueAt(&index);
}

void Curve::getColor(double* col){
    col[0] = this->color[0];
    col[1] = this->color[1];
    col[2] = this->color[2];
    col[3] = this->color[3];
}

double Curve::getMin(){
    return this->min;
}

double Curve::getMax(){
    return this->max;
}
