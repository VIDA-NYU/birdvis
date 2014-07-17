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

#ifndef ARRAY_H
#define ARRAY_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <limits>
#include <cstdio>

using namespace std;

namespace datatypes {

    class Coordinate{
    public:
	float value;

	Coordinate(){
	    value = std::numeric_limits<float>::signaling_NaN();
	}

	Coordinate(float v){
	    value = v;
	}

	Coordinate(int d, int m, int y){

	    struct tm tm_val;
 
	    char buffer[100];

	    //cout << "d, y, m" << d << "," << m << "," << y << endl;


	    sprintf(buffer, "%04d-%02d-%02d", y,m,d);

	    if (strptime(buffer,"%Y-%m-%d",&tm_val) == NULL)
		printf("error\n\n");

	    tm_val.tm_hour  = 0;
	    tm_val.tm_min   = 0;
	    tm_val.tm_sec   = 0;
	    tm_val.tm_wday  = 0;
	    tm_val.tm_yday  = 0;
	    tm_val.tm_isdst = 0;


	    //strftime(buf, 128, "%Y-%m-%d", &tm_val);
    
	    time_t t = mktime(&tm_val);


	    double x = ((double) t) / (60 * 60 * 24 * 1000);
	    this->value = (float) x;
	}	

    };


    class ConcreteArray;

    class Index {
    public:
	int dimension;
	vector<int> *indices;

	Index(int dimension);
	~Index();

	void add(int dimension, int index);
	void add(int dimension, int *indices, int count);
	int getSourceDimension();
	int getDimensionSize(int sourceDimension);
	int getSourceSampleIndex(int sourceDimensionIndex, int currentSampleIndex);
	string str();

        void translate(int* input, int *output);
	int memSize();
    };

    class Array {
    public:
	virtual ~Array() {};
	virtual int memSize() { return 0; }
	virtual double getValueAt(int* coordinates);
	virtual string getDimensionName(int d);
	virtual int getDimension();
	virtual Coordinate getCoordinateVal(int dimensionIndex, int sampleIndex);	
	virtual int getDimensionSize(int d);
	virtual string str();
	virtual long getNumEntries();
	virtual Array* subArray(Index* subIndex);
	virtual Array* slice(vector<int> *dimIndex, vector<int> *dimVal);
	virtual double getValueAt(int i);
	virtual double getValueAt(int i, int j);
	virtual double getValueAt(int i, int j, int k);
	virtual void   getRange(double *min, double *max);
	virtual double getSum(int x_min, int x_max, int y_min, int y_max);
	ConcreteArray* copy();
    };


    class LinkedArray : public Array {    
    public:    
	int            dimension;
	int            *dictionary;
	int            *buffer;
	Index*         index;
	Array*         parentArray;
	LinkedArray(Index* ind, Array* parent);
	~LinkedArray();
	Index* getIndex();
	int getDimension();
	string getDimensionName(int d);
	int getDimensionSize(int d);
	Array* subArray(Index* subIndex);
	double getValueAt(int* coordinates);
	int memSize();
	Coordinate getCoordinateVal(int dimensionIndex, int sampleIndex);	
    };


    class ConcreteArray : public Array{
    public:
	int              dimension;
	string          *dimensionName;
	int             *numSamplesPerDim;
	int             *sizes; //int sizes
	double          *data;
	int              numEntries;

	Coordinate **coordinateValues;

	ConcreteArray(int          dimension, 
		      string      *dimensionName, 
		      int         *numSamples, 
		      double      *data);
	~ConcreteArray();
	int memSize();
	void setValueAt(int *coordinates, double newValue);
	void setValueAt(int i, double newValue);
	void setValueAt(int i, int j,  double newValue);
	void setCoordinateValueAt(int dimensionIndex, int sampleIndex, Coordinate val);
	Coordinate getCoordinateVal(int dimensionIndex, int sampleIndex);	
	int getDimension();
	string getDimensionName(int d);
	int getDimensionSize(int d);
	Array* subArray(Index* subInd);
	ConcreteArray* shift(int dimensionIndex, int amountOfShift);
	double getValueAt(int *coordinates);
	ConcreteArray& operator+=(Array& a);
	ConcreteArray& operator-=(Array& a);
    };

} // end namespace datatypes
#endif
