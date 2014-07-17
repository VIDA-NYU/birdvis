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

#include "DataSet.hpp"

DataSet::DataSet(){
}

DataSet::~DataSet(){
    for (int i=0;i<species.size();i++)
	delete species.at(i);
}

void DataSet::addSpecies(string filename){
    Species* newSpecies = new Species(filename);
    newSpecies->consolidate();
    species.push_back(newSpecies);
}

void DataSet::addSpecies(Species *s){
    species.push_back(s);
    s->consolidate();
}

Species* DataSet::getSpecies(int index){
    return this->species.at(index % this->species.size());
}

/**
 * This method brings all species from ds dataset 
 * to the current dataset and clears the second list
 */
void DataSet::merge(DataSet ds)
{
    for (int i=0;i<ds.species.size();i++)
    {
	Species* s = ds.species.at(i);
	species.push_back(s);
    }
    ds.species.clear();
}

int DataSet::getNumberOfSpecies()
{
    return this->species.size();
}
