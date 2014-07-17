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

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "Array.hpp"

using namespace std;

using namespace datatypes;

// Index

Index::Index(int dimension)
{
    this->dimension = dimension;
    this->indices = new vector<int>[dimension];
}

int Index::memSize()
{
    int size = sizeof(*this);
    for (int i=0;i<dimension;i++)
	size += sizeof(int) * indices[i].capacity();
    return size;
}


Index::~Index()
{
    // cout << "datatypes::Index deleting " << this->memSize()/1024.0 << "Kb" << endl; 
    delete [] this->indices;
}

void Index::add(int dimension, int index)
{
    this->indices[dimension].push_back(index);
}

void Index::add(int dimension, int *indices, int count)
{
    for (int i=0;i<count;i++)
	this->indices[dimension].push_back(indices[i]);
}

int Index::getSourceDimension(){
    return dimension;
}
    
int Index::getDimensionSize(int sourceDimension) {
    if (indices[sourceDimension].size() > 0 && indices[sourceDimension][0] < 0)
    {
	return 0;
    }
    else {
	return indices[sourceDimension].size();
    }
}

string Index::str()
{
    ostringstream s;
    for (int i=0;i<this->dimension;i++)
    {
	vector<int> *v = &this->indices[i];
	s << "[" << i << "] = ";
	for (int j=0;j<v->size();j++)
	{
	    s << v->at(j) << " ";
	}
	s << endl;
    }
    return s.str();
}

// int translate(int input){
// 	return dictionary[input];
// }

void Index::translate(int* input, int *output) {

    int sourceDimension = this->getSourceDimension();

    int j = 0;
    for(int i = 0 ; i < sourceDimension ; i++) {

	// cout << " sourceDimension " << sourceDimension << endl;
	    
	int dimFirstIndex = (this->indices[i])[0];
	    
	if (dimFirstIndex < 0)
	{
	    // cout << "translation " << i << " = " << (abs(this->indices[i][0])-1) << endl;
	    output[i] = abs(this->indices[i][0])-1;
	}
	else {
	    // cout << "translation " << i << " = " << this->indices[i][input[j]] <<  endl;
	    output[i] = this->indices[i][input[j]];
	    j++;
	}
    }
}

int Index::getSourceSampleIndex(int sourceDimensionIndex, int currentSampleIndex){
    return this->indices[sourceDimensionIndex][currentSampleIndex];
}

// Array

double Array::getValueAt(int* coordinates) { return 0; };

Coordinate Array::getCoordinateVal(int dimensionIndex, int sampleIndex){ return Coordinate(0) ;};


double Array::getValueAt(int i)
{
    int index[] = {i};
    return this->getValueAt(index);
}

double Array::getValueAt(int i, int j)
{
    int index[] = {i, j};
    return this->getValueAt(index);
}

double Array::getValueAt(int i, int j, int k)
{
    int index[] = {i,j,k};
    return this->getValueAt(index);
}

string Array::getDimensionName(int d) { return ""; };
int Array::getDimension() { return 0; };
int Array::getDimensionSize(int d) { return 0; };

Array* Array::subArray(Index* subIndex) { return NULL; };

long Array::getNumEntries() 
{
    long v = 1;
    for (int i=0;i<getDimension();i++)
	v *= getDimensionSize(i);
    return v;
}

double Array::getSum(int x_min, int x_max, int y_min, int y_max){
    double sum = 0;
    for(int i = x_min ; i <= x_max  ; i++){
	for(int j = y_min ; j <= y_max  ; j++){
	    sum += this->getValueAt(i,j);
	}
    }

    return sum;
}

Array* Array::slice(vector<int> *dimIndex, vector<int> *dimVal){
    // cout << "Slice a Concrete Array" << endl;

    Index* index = new Index(this->getDimension());
    
    int k = 0;
    for(int i = 0 ; i < this->getDimension() ; i++){
	if(i == (*dimIndex)[k]){
	    //slice it
	    index->add(i,-((*dimVal)[k]+1));
	    k++;
	}
	else{
	    //keep everything
	    int size = this->getDimensionSize(i);
	    for(int j = 0 ; j < size  ; j++){
		index->add(i,j);
	    }
	}
    }
    return this->subArray(index);
}

void Array::getRange(double *min, double *max){

    int numDimension = this->getDimension();

    int dimensionSize[numDimension];

    for(int i = 0 ; i < numDimension ; i++){
	dimensionSize[i] = this->getDimensionSize(i);
    }

    int v[numDimension];
    for (int i=0;i<numDimension;i++)
	v[i] = 0;

    double minT;
    double maxT;

    double val = this->getValueAt(v);
    minT = val;
    maxT = val;


    for (int i=0;i<numDimension;i++)
	v[i] = -1;


    int k = 0;
    while (k >= 0)
    {
	v[k]++;

	if (v[k] >= dimensionSize[k])
	{
	    v[k] = -1;
	    k--;
	}
	else if (k < numDimension-1)
	{
	    k++;
	}
	else {
	    double tempVal = this->getValueAt(v);

// 	    cout << "tempVal v[" << v[k] << "] " << tempVal << endl;
	    if(tempVal < minT){
		minT = tempVal;
	    }
	    if(tempVal > maxT){
		maxT = tempVal;
	    }
	}
	    
    }

//     cout << "minT " << minT << endl;
//     cout << "maxT " << minT << endl;

    *min = minT;
    *max = maxT;

}

ConcreteArray* Array::copy()
{

     cout << "Array Copy =================================" << endl;

    int n = this->getDimension();

//     cout << "Dimensions" << n << endl;

    int dimSizes[n];
    string dimNames[n];

    for (int i=0;i<n;i++)
    {
	dimSizes[i] = this->getDimensionSize(i);
	dimNames[i] = this->getDimensionName(i);
    }

    ConcreteArray *result = new ConcreteArray(n, dimNames, dimSizes, NULL);

    int v[n];
    for (int i=0;i<n;i++)
	v[i] = -1;

    int k = 0;
    while (k >= 0)
    {
	v[k]++;

	if (v[k] >= this->getDimensionSize(k))
	{
	    v[k] = -1;
	    k--;
	}
	else if (k < n-1)
	{
	    k++;
	}
	else {
	    result->setValueAt(v, this->getValueAt(v));
	}
    }


    for(int i = 0 ; i < n ; i++){
	int dimSize = this->getDimensionSize(i);

	for(int j = 0 ; j < dimSize ; j++){
	    double x = this->getCoordinateVal(i,j).value;
	    result->setCoordinateValueAt(i,j,x);
	}
    }

    return result;
}





string Array::str()
{
    ostringstream s;

    int n = this->getDimension();
    // cout << "my dimension "<< this->getDimension() << endl;

    int v[n];
    for (int i=0;i<n;i++){

	v[i] = -1;
	s << this->getDimensionName(i);
	s << " ";
    }

    s << endl;

    int k = 0;
    while (k >= 0)
    {
	v[k]++;

	if (v[k] >= this->getDimensionSize(k))
	{
	    v[k] = -1;
	    k--;
	}
	else if (k < n-1)
	{
	    k++;
	}
	else {
	    s << "[";
	    for (int j=0;j<n;j++)
	    {
		s << v[j];
		if (j < n-1)
		    s << ",";
	    }
	    s << "] ";
	    s << this->getValueAt(v) << endl;
	}
    }

    return s.str();
}

// LinkedArray

LinkedArray::LinkedArray(Index* ind, Array* parent)
{

    this->index = ind;

    this->parentArray = parent;

    this->buffer = new int[this->parentArray->getDimension()];

    int countDim = 0;
    for(int i = 0 ; i < ind->getSourceDimension() ; i++){
	if(ind->getDimensionSize(i) > 0){
	    countDim++;
	}	    	
    }

    this->dictionary = new int[countDim];

    int k = 0;
    int i = 0;
    while(i < ind->getSourceDimension()){
	if(ind->getDimensionSize(i) > 0){
	    this->dictionary[k] = i;
	    k++;
	}
	    
	i++;
    }

    this->dimension = countDim;
}

LinkedArray::~LinkedArray()
{
    // cout << "datatypes::LinkedArray deleting " << this->memSize()/1024.0 << "Kb" << endl; 

    delete this->index;
    delete [] this->dictionary;
    delete [] this->buffer;
}


int LinkedArray::memSize()
{
    return 
	sizeof(*this) +                                    // the local struct size
	sizeof(int) * this->parentArray->getDimension() +  // translation buffer
	sizeof(int) * this->dimension +                    // the array of dictionary
	this->index->memSize();          // the index size (considered part of LinkedArray)
}

Index* LinkedArray::getIndex(){
    return this->index;
}

int LinkedArray::getDimension() {
    return this->dimension;
}

//can allow projected axis???
string LinkedArray::getDimensionName(int d){
    // cout << "parent array " << long(parentArray) << endl;
    // cout << parentArray->str() << endl;
    return parentArray->getDimensionName(this->dictionary[d]);
}

int LinkedArray::getDimensionSize(int d){
    return (this->index->indices[this->dictionary[d]]).size();
}

Array* LinkedArray::subArray(Index* subIndex){   
    LinkedArray* subArrayLinked = new LinkedArray(subIndex, this);	
    return subArrayLinked;
}

double LinkedArray::getValueAt(int* coordinates){
    // cout << "(LinkedArray "<< ((long)this) <<").getValueAt(";
    // for (int i=0;i<this->getDimension();i++)
    //     cout << coordinates[i] << " ";
    // cout << ")" << endl;
    
    // translate coordinates
    this->index->translate(coordinates, this->buffer);

    // cout << "GET VALUE AT " << this->getDimension() << endl;
    // for(int i = 0 ; i < this->getDimension() ; i++) {
    // 	cout << "coordinates[i] size = " << this->getDimensionSize(i) << endl;
    // 	cout << "coordinates[i] = "      << parentCoordinates[i] << endl;
    // }

    double result = this->parentArray->getValueAt(this->buffer);

    return result;
}

Coordinate LinkedArray::getCoordinateVal(int dimensionIndex, int sampleIndex){
    int parentDimIndex = this->dictionary[dimensionIndex];
    int parentSampleIndex = this->index->getSourceSampleIndex(parentDimIndex, sampleIndex);

    return this->parentArray->getCoordinateVal(parentDimIndex, parentSampleIndex);
}	

ConcreteArray::ConcreteArray(int          dimension, 
			     string      *dimensionName, 
			     int         *numSamples, 
			     double      *data){
    this->dimension = dimension;
    
    this->dimensionName = new string[dimension];
    for (int i=0;i<dimension;i++)
	this->dimensionName[i] = dimensionName[i];
    
    this->numSamplesPerDim = new int[dimension];
    memcpy(this->numSamplesPerDim, numSamples, sizeof(int) * dimension);
    
    int prod = 0;
    { // 
	this->sizes = new int[dimension];
	this->sizes[dimension - 1] = 1;
	for(int i = dimension - 2 ; i > -1 ; i--){
	    this->sizes[i] = this->sizes[i+1] * (numSamplesPerDim[i+1]);
	}
	prod = this->sizes[0] * numSamplesPerDim[0];
    } //

    this->data = new double[prod];
    if (data != NULL)
    {
	memcpy(this->data, data, sizeof(double) * prod);
    }


    this->numEntries = prod;

    //allocate space for the coordinates
    this->coordinateValues = new Coordinate *[dimension];

    for(int i = 0 ; i < dimension ; i++){
	this->coordinateValues[i] = new Coordinate[numSamplesPerDim[i]];
    }
}


int ConcreteArray::memSize()
{
    return 
	sizeof(this) + // local structure
	this->numEntries * sizeof(double) + // data 
    	this->dimension * sizeof(string) +  // dimnames
    	this->dimension * sizeof(int) +     // numSamplesPerDim
    	this->dimension * sizeof(int);      // sizes
}


ConcreteArray::~ConcreteArray() 
{
    // cout << "datatypes::ConcreteArray deleting " << this->memSize()/1024.0 << "Kb" << endl; 

    delete [] dimensionName;
    delete [] numSamplesPerDim;
    delete [] sizes;
    delete [] data;
}


int ConcreteArray::getDimension() {
    return this->dimension;
}

string ConcreteArray::getDimensionName(int d){
    return this->dimensionName[d];
}

int ConcreteArray::getDimensionSize(int d){
    return this->numSamplesPerDim[d];
}

Array* ConcreteArray::subArray(Index* subInd) 
{
    LinkedArray* subArrayLinked = new LinkedArray(subInd, this);	
    return subArrayLinked;	
}    

double ConcreteArray::getValueAt(int *coordinates) 
{
    double *currAddress = data;
    for(int i = 0 ; i < this->dimension ; i++) {
	currAddress += (coordinates[i]*sizes[i]);
    }
    return *currAddress;
}

void ConcreteArray::setValueAt(int *coordinates, double newValue)
{
    double *currAddress = data;
    for(int i = 0 ; i < this->dimension ; i++) {
	currAddress += (coordinates[i]*sizes[i]);
    }
    *currAddress = newValue;
}

void ConcreteArray::setValueAt(int i, double newValue)
{
    int c[] = {i};
    this->setValueAt(c,newValue);
}

void ConcreteArray::setCoordinateValueAt(int dimensionIndex, int sampleIndex, Coordinate val)
{
    this->coordinateValues[dimensionIndex][sampleIndex] = val;
}


void ConcreteArray::setValueAt(int i, int j,  double newValue)
{
    int c[] = {i, j};
    this->setValueAt(c,newValue);
}

ConcreteArray& ConcreteArray::operator+=(Array& a)
{
    int n = this->getDimension();

    int v[n];
    for (int i=0;i<n;i++)
	v[i] = -1;

    int k = 0;
    while (k >= 0)
    {
	v[k]++;

	if (v[k] >= this->getDimensionSize(k))
	{
	    v[k] = -1;
	    k--;
	}
	else if (k < n-1)
	{
	    k++;
	}
	else {
	    double a_v = a.getValueAt(v);
	    this->setValueAt(v, getValueAt(v) + a_v);
	}
    }

    return (*this);
}

ConcreteArray& ConcreteArray::operator-=(Array& a)
{
    int n = this->getDimension();

    int v[n];
    for (int i=0;i<n;i++)
	v[i] = -1;

    int k = 0;
    while (k >= 0)
    {
	v[k]++;

	if (v[k] >= this->getDimensionSize(k))
	{
	    v[k] = -1;
	    k--;
	}
	else if (k < n-1)
	{
	    k++;
	}
	else {
	    double a_v = a.getValueAt(v);
	    this->setValueAt(v, getValueAt(v) - a_v);
	}
    }
    return (*this);
}

Coordinate ConcreteArray::getCoordinateVal(int dimensionIndex, int sampleIndex){

    //need to check is the values are in the correct range
    //not done yet
    
    return this->coordinateValues[dimensionIndex][sampleIndex];

}

ConcreteArray* ConcreteArray::shift(int dimensionIndex, int amountOfShift){
    
    int n = this->dimension;

    if(dimensionIndex < 0 || dimensionIndex >= n)
	return NULL;

    int samplesInDimensionShifted = this->getDimensionSize(dimensionIndex);

    string names[n];

    for(int i = 0 ; i < n ; i++){
	names[i] = this->dimensionName[i];
    }

    int dimSizes[n];

    for(int i = 0 ; i < n ; i++){
	dimSizes[i] = this->numSamplesPerDim[i];
    }

    ConcreteArray *x = new ConcreteArray(n,names, dimSizes, NULL);


    int v[n];
    for (int i=0;i<n;i++)
	v[i] = -1;

    int k = 0;
    while (k >= 0)
    {
	v[k]++;

	if (v[k] >= this->getDimensionSize(k))
	{
	    v[k] = -1;
	    k--;
	}
	else if (k < n-1)
	{
	    k++;
	}
	else {
	    //do something
	    int currentIndex = v[dimensionIndex] + amountOfShift;

	    if(currentIndex < 0 || currentIndex > samplesInDimensionShifted){
		x->setValueAt(v,0);
	    }
	    else{

		int u[n];
		for (int i=0;i<n;i++)
		    u[i] = v[i];
		
		u[dimensionIndex] += amountOfShift;

		x->setValueAt(v,this->getValueAt(u));
	    }
	}
    }    

    
    for(int i = 0 ; i < n ; i++){
	int dimSize = this->getDimensionSize(i);

	for(int j = 0 ; j < dimSize ; j++){
	    double value = this->getCoordinateVal(i,j).value;
	    x->setCoordinateValueAt(i,j,value);
	}
    }

    return x;

}
