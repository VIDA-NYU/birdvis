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

#ifndef _DS_HPP_
#define _DS_HPP_

#include <vector>
#include <string>
#include <map>
#include <cfloat>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/tokenizer.hpp>

#include "datatypes.hpp"
#include "Array.hpp"



using namespace std;

// using namespace datatypes;

namespace dataset {

// comment

/*############################################################################*/
/*# Coordinate ###############################################################*/
/*############################################################################*/

    // new class to load the data
    class Coordinate {

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	friend class boost::serialization::access;
	///////////////////////////////////////////////////////////////////

    public:
	double x; //longitude
	double y; //latitude
	
	Coordinate(){
	    this->x = 0;
	    this->y = 0;
	}

	Coordinate(double x, double y){
	    this->x = x;
	    this->y = y;
	}		

    };

/*############################################################################*/
/*# Coordinate Vector ########################################################*/
/*############################################################################*/

    class CoordinateVector
    {

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	friend class boost::serialization::access;
	///////////////////////////////////////////////////////////////////

    public:
	vector<Coordinate> coordinates; 
	double minLat;
	double maxLat;
	double minLon;
	double maxLon;

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
	    ar & coordinates;
	    ar & minLat;
	    ar & maxLat;
	    ar & minLon;
	    ar & maxLon;

	}
	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
	    ar & coordinates;
	    ar & minLat;
	    ar & maxLat;
	    ar & minLon;
	    ar & maxLon;
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	//////////////////////////////////////////////////////////////////

	CoordinateVector(){
	    minLat = DBL_MAX;
	    maxLat = -DBL_MAX;
	    minLon = DBL_MAX;
	    maxLon = -DBL_MAX;
	}

	void addPoint(double x, double y){


	    if(x < minLon)
		minLon = x;

	    if(x > maxLon)
		maxLon = x;

	    if(y < minLat)
		minLat = y;

	    if(y > maxLat)
		maxLat = y;

	    Coordinate p(x,y);
	    coordinates.push_back(p);
	}	

	double getMinLon(){
	    return this->minLon;
	}

	double getMaxLon(){
	    return this->maxLon;
	}

	double getMinLat(){
	    return this->minLat;
	}

	double getMaxLat(){
	    return this->maxLat;
	}

	int getNumPoints(){
	    return coordinates.size();
	}

    }; // CoordinateVector

/*############################################################################*/
/*# IndexedValue  ############################################################*/
/*############################################################################*/

    class IndexedValue{

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	friend class boost::serialization::access;
	///////////////////////////////////////////////////////////////////

    public:
	int index;
	double value;

	IndexedValue() {
	    this->index = 0;
	    this->value = 0.0;
	}

	IndexedValue(int i, double v){
	    this->index = i;
	    this->value = v;
	}
    };

/*############################################################################*/
/*# CoordinateValueSet #######################################################*/
/*############################################################################*/

    class CoordinateValueSet {

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	friend class boost::serialization::access;
	///////////////////////////////////////////////////////////////////

    public:
	int year, month, day;
	CoordinateVector *coordinate_vector;
	vector<IndexedValue> values;
	double sum;

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
	    ar & year;
	    ar & month;
	    ar & day;
	    ar & coordinate_vector;
	    ar & values;
	    ar & sum;
	}
	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
	    ar & year;
	    ar & month;
	    ar & day;
	    ar & coordinate_vector;
	    ar & values;
	    ar & sum;
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	//////////////////////////////////////////////////////////////////

	CoordinateValueSet(int year, int month, int day, CoordinateVector *coords)
	{
	    this->year = year;
	    this->month = month;
	    this->day = day;
	    this->coordinate_vector = coords;
	    this->sum = 0;
	}

	CoordinateValueSet()
	{
	    this->year  = 0;
	    this->month = 0;
	    this->day   = 0;
	    this->coordinate_vector = NULL;
	}

	void add(int index, double value)
	{
	    IndexedValue v(index,value);
	    this->values.push_back(v);

	    //
	    this->sum += value;
	}

	double** makeGrid(int resX, int resY, double minLon, double minLat, 
			  double maxLon, double maxLat){
	    
	    double **data = new double *[resX];
	    int count[resX][resY];

	    for(int j = 0 ; j < resX ; j++){
		data[j] = new double[resY];
		for(int k = 0 ; k < resY ; k++){
		    data[j][k] = 0;
		    count[j][k] = 0;
		}
	    }

// 	    double minLon = coordinate_vector->getMinLon();
// 	    double maxLon = coordinate_vector->getMaxLon();
// 	    double minLat = coordinate_vector->getMinLat();
// 	    double maxLat = coordinate_vector->getMaxLat();
	  	    
	    //cout << "Get Deltas" << endl;
	    double deltaX = (maxLon - minLon)/resX;
	    double deltaY = (maxLat - minLat)/resY;
	    
	    //cout << "DeltaX " << deltaX << " DeltaY " << deltaY << endl;


	    int numNonZeroPoints = values.size();

	    int numPoints = this->coordinate_vector->getNumPoints();
	    
	    //cout << "Computing Values" << endl;

	    for(int j = 0 ; j < numNonZeroPoints ; j++){
		//cout << "Get Point " << j << endl;
		IndexedValue point = values.at(j);

		//cout << "Point Index " << point.index << endl;
		//cout << "Get Coordinates " << point.index << endl;
		Coordinate coords  = coordinate_vector->coordinates.at(point.index);
		double lon = coords.x;
		double lat = coords.y;

		//cout << "Lon " << lon << " Lat " << lat << endl;
		
		double cellXX = (lon - minLon)/(maxLon - minLon);

		if(cellXX == 1.0)
		    cellXX = 0.99999;

		double cellYY = (lat - minLat)/(maxLat - minLat);

		if(cellYY == 1.0)
		    cellYY = 0.99999;//resY - 1;
		
		//cout << "CellXX " << cellXX << " CellYY " << cellYY << endl; 


		int cellX = (int)(cellXX * resX);
		int cellY = (int)(cellYY * resY);
		
		//cout << "CellX " << cellX << " CellY " << cellY << endl; 
		
		//cout << "Wrinting values" << endl;
		data[cellX][cellY]  += point.value;
	    }

	    for(int j = 0 ; j < numPoints ; j++){
		Coordinate coords  = coordinate_vector->coordinates.at(j);
		double lon = coords.x;
		double lat = coords.y;

		
		double cellXX = (lon - minLon)/(maxLon - minLon);

		if(cellXX == 1.0)
		    cellXX = 0.99999;

		double cellYY = (lat - minLat)/(maxLat - minLat);

		if(cellYY == 1.0)
		    cellYY = 0.99999;//resY - 1;

		int cellX = (int)(cellXX * resX);
		int cellY = (int)(cellYY * resY);
		
		count[cellX][cellY] += 1;
	    }

	    for(int j = 0 ; j < resX ; j++){
		for(int k = 0 ; k < resY ; k++){
		    if(count[j][k] == 0)
			count[j][k] = 0;		    
		    else
			data[j][k] /= count[j][k];
		}
	    }		

	    //cout << "Normalized" << endl;

	    return data;
	
	}
	
    };

/*############################################################################*/
/*# Variable #################################################################*/
/*############################################################################*/

    class Variable{

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	friend class boost::serialization::access;
	///////////////////////////////////////////////////////////////////
    private:
	datatypes::ConcreteArray* curvePoints;

    public:
	enum Type { OCCURRENCE, STANDARD_DEVIATION, PREDICTOR };
    public:
	Type type;
	string name;
	vector<CoordinateValueSet*> value_sets; //time steps

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
	    ar & type;
	    ar & name;
	    ar & value_sets;
	}
	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
	    ar & type;
	    ar & name;
	    ar & value_sets;
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	//////////////////////////////////////////////////////////////////

	Variable(Type t, string n){
	    this->type = t;
	    this->name = n;
	    curvePoints = NULL;
	}

	Variable()
	{
	    this->type  = OCCURRENCE;
	    this->name  = "";
	    curvePoints = NULL;
	}


	void add(CoordinateValueSet *cvs)
	{
	    value_sets.push_back(cvs);
	}

	CoordinateValueSet* getCoordinateValueSet(int index){
	    return this->value_sets.at(index);
	}

	double* getSumVector(){
	    int numTimeSteps = this->value_sets.size();

	    double *sums = new double[numTimeSteps];

	    for(int i = 0 ; i < numTimeSteps ; i++){
		sums[i] = value_sets.at(i)->sum;
	    }	    

	    return sums;
	}

	int getNumTimeSteps(){
	    return this->value_sets.size();
	}

	double *getAverageVector(){
	    int numTimeSteps = this->value_sets.size();

	    double *avgs = new double[numTimeSteps];

	    for(int i = 0 ; i < numTimeSteps ; i++){
		CoordinateValueSet* valSet = value_sets.at(i);
		avgs[i] =((valSet->sum)/(valSet->values.size()));
	    }	   

	    return avgs;
	}
	
	void getRangeXY(double *minX, double *maxX, double *minY, double *maxY){

	    int numTimeSteps = this->value_sets.size();

	    if(numTimeSteps == 0)
		return;

	    CoordinateValueSet* tempCVS = this->value_sets.at(0);

	    double minXX = tempCVS->coordinate_vector->minLon;
	    double maxXX = tempCVS->coordinate_vector->maxLon;
	    double minYY = tempCVS->coordinate_vector->minLat;
	    double maxYY = tempCVS->coordinate_vector->maxLat;

	    for(int i = 1 ; i < numTimeSteps ; i++){
		tempCVS = this->value_sets.at(i);
		
		double minXXX = tempCVS->coordinate_vector->minLon;
		double maxXXX = tempCVS->coordinate_vector->maxLon;
		double minYYY = tempCVS->coordinate_vector->minLat;
		double maxYYY = tempCVS->coordinate_vector->maxLat;

		if(minXXX < minXX)
		    minXX = minXXX;
		if(maxXXX < maxXX)
		    maxXX = maxXXX;
		if(minYYY < minYY)
		    minYY = minYYY;
		if(maxYYY < maxYY)
		    maxYY = maxYYY;
	    }
	
	    *minX = minXX;
	    *maxX = maxXX;
	    *minY = minYY;
	    *maxY = maxYY;
	}

	datatypes::ConcreteArray *getCubeGridArray(int resX, int resY){
	    string dimName[3] = {"Time", "Longitude", "Latitude"};

	    int numSamples[3] = {this->value_sets.size(), resX, resY};


	    datatypes::ConcreteArray* cube = 
		new datatypes::ConcreteArray(3, &dimName[0], &numSamples[0], NULL);

	    int numTimeSteps = this->value_sets.size();

	    double minLon, minLat, maxLon, maxLat;

	    this->getRangeXY(&minLon, &maxLon, &minLat, &maxLat);


	    for(int i = 0 ; i < numTimeSteps ; i++){
		//cout << "i " << i << endl;
 		double **slice = this->value_sets.at(i)->makeGrid(resX, resY, 
								  minLon, minLat, maxLon, maxLat);		
		int coord[3];
		coord[0] = i;

		for(int j = 0 ; j < resX ; j++){		    
		    //cout << "j " << j << endl;
		    coord[1] = j;

		    for(int k = 0 ; k < resY ; k++){
			//cout << "i " << i << " j " << j << " k " << k << endl;
			coord[2] = k;
			cube->setValueAt(coord, abs(slice[j][k]));

		    }

		}

		delete [] slice;
	    }

	    //set coordinates	    
	    for(int i = 0 ; i < numTimeSteps ; i++){
		CoordinateValueSet* timeStep = value_sets.at(i);
		cube->setCoordinateValueAt(0, i, 
					   datatypes::Coordinate(timeStep->day, timeStep->month, timeStep->year));
	    }
	    	    
	    double deltaLon = (maxLon - minLon) / resX;
	    double deltaLat = (maxLat - minLat) / resY;

	    for(int j = 0 ; j < resX ; j++){		    
		cube->setCoordinateValueAt(1, j, datatypes::Coordinate(minLon + j * deltaLon));
	    }

	    for(int k = 0 ; k < resY ; k++){
		cube->setCoordinateValueAt(2, k, datatypes::Coordinate(minLat + k * deltaLat));
	    }

	    return cube;
	}


	void getInterpIndex(double x, double* samples, int numSamples, int *interpIndex, double *interpolationLambda){

	    int curIndex = numSamples / 2;

	    int lower = 0; 
	    int upper = numSamples - 1;

	    while(true){
		int index = (lower + upper)/2;

		if(samples[index] <= x && samples[index + 1] > x ){
		    //found the result
		    *interpIndex = index;
		   
		    *interpolationLambda = (x - samples[index])/(samples[index + 1] - samples[index]);
		    
		    break;
		}
		else if(samples[index] > x){
		    upper = index;
		}
		else{ // if(samples[index + 1] <= x )
		    lower = index + 1;
		} 
	    }
	    
	}

	datatypes::ConcreteArray*
	getCubeGrid(datatypes::Coordinate x0, datatypes::Coordinate x1, int resX, 
		    datatypes::Coordinate y0, datatypes::Coordinate y1, int resY, 
		    datatypes::Coordinate t0, datatypes::Coordinate t1, int resT){

// 	    cout << "Computing Grid" << endl;

// 	    string dimensionNames[3] = {"Time", "Longitude", "Latitude"};

// 	    int sampleSizes[3] = {resT, resX, resY};

// 	    datatypes::ConcreteArray *ret = 
// 		new datatypes::ConcreteArray(3, dimensionNames, sampleSizes, NULL);

// 	    int coordArray[3];

// 	    double deltaT = (t1.value - t0.value)/resT;
// 	    double deltaX = (x1.value - x0.value)/resX;
// 	    double deltaY = (y1.value - y0.value)/resY;

// 	    cout << "deltaT " << deltaT << endl;
// 	    cout << "deltaX " << deltaX << endl;
// 	    cout << "deltaY " << deltaY << endl;

// 	    int prevInterpIndex = -1;
	    
// 	    double **leftSlice  = NULL;
// 	    double **rightSlice = NULL;

// 	    //set coordinates values
// 	    cout << "Set Coordinate Values" << endl;
// 	    for(int i = 0 ; i < resT ; i++){
// 		double lambdaT = t0.value + i * (deltaT);
// 		ret->setCoordinateValueAt(0, i, lambdaT);		
// 	    }

// 	    for(int j = 0 ; j < resX ; j++){
// 		double lambdaX = x0.value + j * deltaX;
// 		ret->setCoordinateValueAt(1, j, lambdaX);
// 	    }

// 	    for(int k = 0 ; k < resY ; k++){
// 		double lambdaY = y0.value + k * deltaY;
// 		ret->setCoordinateValueAt(2, k, lambdaY);
// 	    }

// 	    int numTimeStepsDataset = this->value_sets.size();

// 	    double dateIndices[numTimeStepsDataset];

// 	    cout << "Converting dates to double" << endl;

// 	    for(int i = 0 ; i < numTimeStepsDataset ; i++){
// 		CoordinateValueSet* x = value_sets.at(i);
// 		datatypes::Coordinate c(x->day,x->month,x->year);
// 		dateIndices[i] = c.value;
// 	    }

// 	    //
// 	    for(int i = 0 ; i < resT ; i++){		
// 		cout << "Time step " << i << endl;

// 		coordArray[0] = i;	    
// 		double lambdaT = t0.value + i * (deltaT);


// 		//get interpolation index in time
// 		int interpIndex;
// 		double interpolationLambda;

// 		//double x, double* samples, int numSamples, int *interpIndex, double *interpolationLambda
// 		getInterpIndex(lambdaT, dateIndices, numTimeStepsDataset, &interpIndex, &interpolationLambda);

// 		cout << "interpIndex " << interpIndex << endl;


// 		CoordinateValueSet* timeIntervalLeft   = value_sets.at(interpIndex);
// 		CoordinateValueSet* timeIntervalRight  = value_sets.at(interpIndex + 1);

// 		//interpolate these
// 		if(prevInterpIndex != interpIndex){
// 		    //interpolate each time step in x and y
// 		    if(leftSlice != NULL)
// 			delete leftSlice;
		    
// 		    if(rightSlice != NULL)
// 			delete rightSlice;
		    
// 		    cout << "LeftTime " << timeIntervalLeft << endl;
// 		    cout << "RightTime " << timeIntervalRight << endl;

// 		    leftSlice  = timeIntervalLeft->makeGrid(resX, resY); //interpolateInXY(timeIntervalLeft, resX, resY);
// 		    rightSlice = timeIntervalRight->makeGrid(resX, resY); //interpolateInXY(timeIntervalRight, resX, resY);

// 		    cout << "Slices Computed" << endl;
		   
// 		    prevInterpIndex = interpIndex;
// 		}
// 		else{
// 		    cout << "Fall between the same slices" << endl;
// 		}


// 		for(int j = 0 ; j < resX ; j++){
// 		    coordArray[1] = j; 

// 		    for(int k = 0 ; k < resY ; k++){
// 			coordArray[2] = k; 
// 			ret->setValueAt(coordArray, leftSlice[j][k] + interpolationLambda * (rightSlice[j][k] - leftSlice[j][k]));
// 		    }
// 		}

// 	    }// end for resT

// 	    if(leftSlice != NULL)
// 		delete leftSlice;

// 	    if(rightSlice != NULL)
// 		delete rightSlice;

//	    return ret;
	    return NULL;

	}//end getCubeGrid

	datatypes::ConcreteArray* getCurveOnPoints(datatypes::selection::Selection *selection){
	    if(selection == NULL && this->curvePoints != NULL)
		return this->curvePoints;
	    else{
		bool overEntireSet = (selection == NULL);
		int numTimeSteps = this->getNumTimeSteps();
		
		double data[numTimeSteps];
	    
		for(int i = 0 ; i < numTimeSteps ; i++){
		    data[i] = 0;
		}

		string name = "Time";

		datatypes::ConcreteArray * array =  
		    new datatypes::ConcreteArray(1, &name, &numTimeSteps, NULL);
	    
		for(int i = 0 ; i < numTimeSteps ; i++){
		    CoordinateValueSet* value_set = this->value_sets.at(i);
		    int numPoints = value_set->values.size();
		
		    for(int j = 0 ; j < numPoints  ; j++){
			IndexedValue val = value_set->values.at(j);
			Coordinate coord = value_set->coordinate_vector->coordinates.at(val.index);

			if(overEntireSet || selection->contains(coord.x, coord.y))
			    data[i] += (val.value);
		    }
		
		    int totalPoints = value_set->coordinate_vector->coordinates.size();

		    data[i] = data[i]/totalPoints;
		}

		for(int i = 0 ; i < numTimeSteps ; i++){
		    array->setValueAt(i,data[i]);
		}    
	    
		this->curvePoints = array;
	    }
	    
	    return this->curvePoints;	    
	}

    };

/*############################################################################*/
/*# Dataset ##################################################################*/
/*############################################################################*/

    class Dataset {

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	friend class boost::serialization::access;
	///////////////////////////////////////////////////////////////////

    public:
	string name;
	vector <Variable*> variables;

	///////////////////////////////////////////////////////////////////
	// BOOST SERIALIZATION STUFF
	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
	    ar & name;
	    ar & variables;
	}
	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
	    ar & name;
	    ar & variables;
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	//////////////////////////////////////////////////////////////////

	Dataset(string name)
	{
	    this->name = name;
	}

	int getNumVariables(){
	    return this->variables.size();
	}

	Variable* getVariable(int index){
    	    return this->variables.at(index);
    	}

	Variable* getDatasetVariable(int index){
	    if(index < 0 || index > this->getNumVariables())
		return NULL;
	    else
		return variables.at(index);
	}

	// this linear search can be slow, but we are going to have few variables per dataset
	Variable* getDatasetVariable(string varName){ // assume that variables names are unique
	    int numVar = this->getNumVariables();

	    for(int i = 0 ; i < numVar ; i++){
		Variable *var = this->variables.at(i);
		if(var->name.compare(varName) == 0){
		    return var;
		}
	    }
	    
	    return NULL;
	}

	void add(Variable *v)
	{
	    variables.push_back(v);
	}

	void str(){
	    cout << "Dataset " << name << endl;
	    cout << "Number of Variables" << variables.size() << endl;

	    int numOfVar = variables.size();

	    for(int i = 0 ; i < numOfVar ; i++){
		Variable* var = variables.at(i);
		cout << "Var " << i << " = " << var->name << endl; 
		cout << "VarSize " << var->getNumTimeSteps() << endl;
	    }
	    //vector <Variable*> variables;
	}


	datatypes::ConcreteArray* getCubeGrid(int indexVar, 
					      datatypes::Coordinate x0, datatypes::Coordinate x1, int resX, 
					      datatypes::Coordinate y0, datatypes::Coordinate y1, int resY, 
					      datatypes::Coordinate t0, datatypes::Coordinate t1, int resT){					      
//int day0, int m0, int y0, int day1, int m1, int y1, int resT){
	    
	    // need to compute expression
	    // not done yet
	    
		Variable *var = this->getDatasetVariable(indexVar);

		if(var == NULL)
		    return NULL;


		datatypes::ConcreteArray *array = var->getCubeGrid( x0,  x1,  resX, y0,  y1,  resY, t0,  t1,  resT);
	
		return array;
	}


    };


    class DatasetRepository{
    public:	

	datatypes::ConcreteArray* 
	getCubeGridWithoutInterpolation(int resX, int resY, int indexDataset)
	{
	    
	    // need to compute expression
	    // not done yet
	    string key = buildGetCubeGridKeyWithoutInterpolation(indexDataset, resX, resY, "A[0]"); //	    

	    if(cube_grid_repository.count(key) == 0){
		Variable *var = this->datasets.at(indexDataset)->getDatasetVariable(0);

		if(var == NULL)
		    return NULL;
		
		datatypes::ConcreteArray *array = var->getCubeGridArray(resX, resY);	       


		cube_grid_repository[key] = array;
				
		return array;
	    }
	    else{
		return cube_grid_repository[key];
	    }

	    return NULL;
	}

	string buildKeyComputeExpression(string expression, int resX, int resY){
	    ostringstream s;
 
	    s << "Dataset: ";	    

	    for(int i = 0 ; i < this->getNumDatasets() ; i++){	    
		Dataset* ds = this->datasets.at(i);
		s << ds->name << " ";
	    }
	    
	    s << "Expression: " << expression ;

	    s << " resX: " << resX << " resY: " << resY;

	    return s.str();

	}

	datatypes::ConcreteArray* computeExpression(string expression, int resX, int resY){
	    string key = buildKeyComputeExpression(expression, resX, resY); //	    
	    
	    if(cube_grid_repository.count(key) == 0){

		vector<string> tokens;
	    
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("[] \t\n");
	    
		tokenizer tk(expression, sep);
		for (tokenizer::iterator tok_iter = tk.begin();tok_iter != tk.end(); ++tok_iter)
		{		
		    tokens.push_back(*tok_iter);
		}
		
		int ds_index = tokens.at(0)[0] - (int) ('A');
	    
		int time_delta   = atoi(tokens.at(1).c_str());

		datatypes::ConcreteArray *array =
		    getCubeGridWithoutInterpolation(resX, resY, ds_index)->shift(0,time_delta);
	    
		datatypes::ConcreteArray *A = array->copy();

		for (int i=2;i<tokens.size();i+=3)
		{
		    char op = tokens.at(i)[0];
		    int ds_index_i = tokens.at(i+1)[0] - (int) ('A');
		    int time_delta_i   = atoi(tokens.at(i+2).c_str());
		
		    datatypes::Array *a_i = 
			getCubeGridWithoutInterpolation(resX, resY, ds_index_i)->shift(0,time_delta_i);

		    if (op == '+')
			(*A) += (*a_i);
		    else
			(*A) -= (*a_i);
		
		    delete a_i;
		}

		cube_grid_repository[key] = A;

		return A;
	    }
	    else{
		return cube_grid_repository[key];
	    }
	}


    public:
	vector<Dataset*>                             datasets;
	map<string, datatypes::ConcreteArray*>       curve_repository;
	map<string, datatypes::ConcreteArray*>       cube_grid_repository;

	DatasetRepository(){
	}
	
	int getNumDatasets(){
	    return datasets.size();
	}

	Dataset* getDataset(int index){
	    if(index < 0 || index > this->getNumDatasets())
		return NULL;
	    else
		return datasets.at(index);
	}

	void addDataset(Dataset* ds){
	    this->datasets.push_back(ds);
	}


	string buildGetIntegralCurveKey(int indexDataset, string variableName, 
					datatypes::selection::Selection *sel)
	{
	    
	    ostringstream s;
	    
	    s << "Dataset: ";

	    Dataset* ds = this->datasets.at(indexDataset);
	    
	    s << ds->name << " Variable: " ;

	    s << variableName << " " << sel->id;

	    return s.str();
	}

	double* getSumVector(int indexDataset, string variableName, 
			     datatypes::selection::Selection *sel = NULL){
	    
// 	    bool curveOverEntireSet = (sel == NULL);

// 	    string key = buildGetIntegralCurveKey(indexDataset, variableName, sel); //

// 	    if(curve_repository.count(key) == 0){
// 		double *sumVec = this->datasets.at(indexDataset)->getDatasetVariable(variableName)->getSumVector();
// 		curve_repository[key] = sumVec;
// 		return sumVec;
// 	    }
// 	    else{
// 		return curve_repository[key];
// 	    }
	}

	string buildGetCubeGridKey(int indexDataset, string variableName, int resX, int resY, string expression = "A[0]"){
	    ostringstream s;
	    
	    s << "Dataset: ";

 	    Dataset* ds = this->datasets.at(indexDataset);
	    
	    s << ds->name << " Variable: " ;

	    s << variableName << " resX: " << resX << " resY: " << resY;

	    return s.str();
	}

	datatypes::ConcreteArray* getCubeGrid(int indexDataset, string variableName, 
					      datatypes::Coordinate x0, datatypes::Coordinate x1, int resX, 
					      datatypes::Coordinate y0, datatypes::Coordinate y1, int resY, 
					      datatypes::Coordinate t0, datatypes::Coordinate t1, int resT){
	    
	    // need to compute expression
	    // not done yet

	    string key = buildGetCubeGridKey(indexDataset, variableName, resX, resY); //	    
	    
	    if(cube_grid_repository.count(key) == 0){

		Variable *var = this->datasets.at(indexDataset)->getDatasetVariable(variableName);

		if(var == NULL)
		    return NULL;


		datatypes::ConcreteArray *array = var->getCubeGrid( x0,  x1,  resX, y0,  y1,  resY, t0,  t1,  resT);
		
		cube_grid_repository[key] = array;
		return array;
	    }
	    else{
		return cube_grid_repository[key];
	    }
	}

	string 
	buildGetCubeGridKeyWithoutInterpolation(int indexDataset, 
						int resX, int resY, string expression)
	{
	    ostringstream s;
	    
	    s << "Dataset: ";

 	    Dataset* ds = this->datasets.at(indexDataset);
	    
	    s << ds->name << " Expression: " ;

	    s << expression << " resX: " << resX << " resY: " << resY;

	    return s.str();
	}

	datatypes::ConcreteArray* 
	getCubeGridWithoutInterpolation(int resX, int resY, int indexDataset, string expression){
	    // need to compute expression
	    // not done yet
	    datatypes::ConcreteArray* x = computeExpression(expression, resX, resY);
	 
	    return x;
	}

	string 
	buildGetIntegralCurveKey(int indexDataset, int resX, int resY, 
				 string expression, datatypes::selection::Selection *sel){
	    	    
	    ostringstream s;
	    
	    s << "Dataset: ";
	    
	    Dataset* ds = this->datasets.at(indexDataset);
	    
	    s << ds->name << " resX: " << resX << " resY: " << resY << " Expression: " << expression ;
	    
	    if(sel == NULL)
		s << " Selection: NONE";
	    else
		s << " Selection: " << sel->id;

	    return s.str();
	}

	datatypes::ConcreteArray* 
	getSumVectorWithoutInterpolation(int indexDataset, int resX, int resY,
					 datatypes::selection::Selection *sel, 
					 string expression)
	{

	    bool curveOverEntireSet = (sel == NULL);

	    string key = buildGetIntegralCurveKey(indexDataset, resX, resY, expression, sel); //

	    if(curve_repository.count(key) == 0){
		//compute now
		datatypes::ConcreteArray *array =
		    computeExpression(expression, resX, resY);
		    //getCubeGridWithoutInterpolation(resX, resY, indexDataset, expression);

		int numTimeSteps = array->getDimensionSize(0);

		string name = "time";

		datatypes::ConcreteArray *curve =
		    new datatypes::ConcreteArray(1, &name, &numTimeSteps, NULL);
		
	       
		//get indices bounding box
		int boundIndices[2][2];
	    
	    
		if(curveOverEntireSet){
		    boundIndices[0][0] = 0;
		    boundIndices[0][1] = resX - 1;
		    boundIndices[1][0] = 0;
		    boundIndices[1][1] = resY - 1;
		}
		else{
		    int x_min = 100000000;
		    int x_max = -1;
		    int y_min = 100000000;
		    int y_max = -1;
		    
		    for (int i=0 ; i < resX ;i++)
		    {
			double x = (array->getCoordinateVal(1,i)).value;
			
			for (int k=0 ; k < resY ; k++){			    
			    double y  = array->getCoordinateVal(2,k).value;
			    
		
			    if(sel->contains(x,y)){
				if(i < x_min)
				    x_min = i;
				if(i > x_max)
				    x_max = i;
				
				if(k < y_min)
				    y_min = k;
				
				if(k > y_max)
				    y_max = k;
			    }
			}
			
		    }				    
		    
		    boundIndices[0][0] = x_min;
		    boundIndices[0][1] = x_max;
		    boundIndices[1][0] = y_min;
		    boundIndices[1][1] = y_max;		    
		}
		// cout << "Bounding Box " << boundIndices[0][0] << " "
// 		     << boundIndices[0][1] << " "
// 		     << boundIndices[1][0] << " " 
// 		     << boundIndices[1][1] << endl;
		
		int coord[3];
		
		for(int i = 0 ; i < numTimeSteps ; i++){
		    double sum = 0;
		    coord[0] = i;
		    
		    for(int j = boundIndices[0][0] ; j <= boundIndices[0][1] ; j++){
			coord[1] = j;
			for(int k = boundIndices[1][0] ; k < boundIndices[1][1] ; k++){
			    coord[2] = k;
			    sum += array->getValueAt(coord);
			}
		    }

		    curve->setValueAt(i, sum);
		}
		
		curve_repository[key] = curve;
		return curve;	    		
	    }
	    else{
		cout << "Curve Cached" << endl;
		return curve_repository[key];
	    }
	}

	vector<Variable*>*
	getPredictorVariables(int indexDataset, 
			      datatypes::selection::Selection* selection = NULL){

	    cout << "Get Predictor Variables 000000000000000000000000000" << endl;
	    
	    Dataset* ds = this->getDataset(indexDataset);

	    vector<Variable *> *vec = new vector<Variable *>();

	    for(int i = 0 ; i < ds->getNumVariables() ; i++){
		Variable *var = ds->getDatasetVariable(i);

		if(var->type == Variable::PREDICTOR){
		    //
		    //cout << "Var Name " << var->name << endl;
		    vec->push_back(var);
		}
	    }

	    return vec;
	}

	string buildGetIntegralCurveKey(int indexDataset, int indexVariable, int resX, 
					int resY, datatypes::selection::Selection *sel){
	    ostringstream s;
	    
	    s << "Dataset: ";
	    
 	    Dataset* ds = this->datasets.at(indexDataset);
	    
	    s << ds->name << " Variable: " ;

	    s << ds->variables.at(indexVariable)->name << " resX: " << resX << " resY: " << resY;

	    if(sel == NULL){
		s << " Selection: NONE";
	    }
	    else{
		s << " Selection: " << sel->id;
	    }

	    return s.str();
	}

	datatypes::ConcreteArray* getSumVectorWithoutInterpolation(int indexDataset, int indexVariable, 
								   int resX, int resY,
								   datatypes::selection::Selection *sel){
	    bool curveOverEntireSet = (sel == NULL);

	    string key = buildGetIntegralCurveKey(indexDataset, indexVariable, resX, resY, sel); //

	    if(curve_repository.count(key) == 0){
		//compute now
		datatypes::ConcreteArray *array =
		    this->getDataset(indexDataset)->variables.at(indexDataset)->getCurveOnPoints(sel);

		curve_repository[key] = array;
		return array;	    		
	    }
	    else{
		return curve_repository[key];
	    }
	}

// 	string buildGetCubeGridInterpolateKey(int indexDataset, string variableName, int resX, int resY, int resT){

// 	    ostringstream s;
	    
// 	    s << "Dataset: ";

//  	    Dataset* ds = this->datasets.at(indexDataset);
	    
// 	    s << ds->name << " Variable: " ;

// 	    s << variableName << " resX: " << resX << " resY: " << resY << " resT: " << resT;

// 	    return s.str();
// 	}

// 	double** interpolateSlices(double **slice1, double **slice2, int resX, int resY, double lambda){
// 	    double **interpSlice = new double *[resX];

// 	    for(int i = 0 ; i < resX ; i++){
// 		interpSlice[i] = new double[resY];

// 		for(int j = 0 ; j < resY ; j++){
// 		    interpSlice[i][j] = slice1[i][j] * lambda + slice2[i][j]*(1 - lambda);
// 		}
// 	    }

// 	    return interpSlice;
// 	}

// 	double*** getCubeGridInterpolateT(int indexDataset, string variableName, int resX, int resY, int resT, string expression){
	    
// 	    if(resT < 2)
// 		return NULL;

// 	    string key = buildGetCubeGridInterpolateKey(indexDataset, variableName, resX, resY, resT); //

// 	    if(cube_grid_repository.count(key) == 0){
// 		double ***cubeGrid = this->getCubeGrid(indexDataset, variableName,  resX,  resY, expression);

// 		//interpolate in time
// 		double ***cubeGridInterpolated = new double **[resT];
// 		int numTimeSteps = this->datasets.at(indexDataset)->getDatasetVariable(variableName)->value_sets.size();
// 		double interpIndex = ((double) (numTimeSteps-1))/(resT-1);

// 		for(int i = 0 ; i < resT ; i++){
// 		    double currIndex = i*interpIndex;
		    
// 		    int prevIndex = ((int) currIndex);
// 		    int nextIndex = prevIndex + 1;
			
// 		    double lambda = currIndex - prevIndex;

// 		    //interpolate slices prevIndex and nextIndex
// 		    if(nextIndex == numTimeSteps){
// 			lambda = 0;
// 			nextIndex = prevIndex;
// 		    }
		    
// 		    cubeGridInterpolated[i] = interpolateSlices(cubeGrid[prevIndex], cubeGrid[nextIndex], resX, resY, lambda);
// 		}

// 		return cubeGridInterpolated;
// 	    }
// 	    else{
// 		return cube_grid_repository[key];
// 	    }

// 	}	
    };

}//end of namespace dataset

///////////////////////////////////////////////////////
// BOOST
BOOST_IS_BITWISE_SERIALIZABLE(dataset::Coordinate)
BOOST_IS_BITWISE_SERIALIZABLE(dataset::IndexedValue)

BOOST_CLASS_VERSION(dataset::Coordinate, 0)
BOOST_CLASS_VERSION(dataset::CoordinateVector, 0)
BOOST_CLASS_VERSION(dataset::Dataset, 0)
BOOST_CLASS_VERSION(dataset::Variable, 0)
BOOST_CLASS_VERSION(dataset::IndexedValue, 0)
BOOST_CLASS_VERSION(dataset::CoordinateValueSet, 0)
///////////////////////////////////////////////////////

#endif
