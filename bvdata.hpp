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

#ifndef _BVDATA_HPP
#define _BVDATA_HPP

#include <boost/date_time/gregorian/gregorian.hpp>

#include <vector>
#include <exception>

#include <Rcpp.h>
#include <RInside.h>  // for the embedded R via RInside

#include "datatypes.hpp"

using namespace std;
using namespace datatypes;

namespace bvdata {   

    class FileInvalidFormatException : public exception {
    public:
	string message;

	FileInvalidFormatException(string message);    
	~FileInvalidFormatException() throw ();   
	void report();
    };

    class File;
    class Repository;

    class Predictor{
    public:
	string internal_name;
	string tag;
	Predictor();	
    };

    class File {
    public:

	Repository *repository;
	int         id;
	string      filename;
	string      name;
	string      sci_name; //TODO init this!!!!!!!!!
	string      shortname;

	string      thumb_photo_file; //TODO init this!!!!!!!!!
	string      pie_chart_file; //TODO init this!!!!!!!!!

	bool        loaded;

	int         xcells, ycells;
	ViewRect    quad; // in longitude and latitude
	int         num_time_steps;

	int         predictors_xcells, predictors_ycells;
	ViewRect    predictors_quad; // in longitude and latitude
	vector<Predictor> predictors;

	//vector<struct tm> dates;
	vector<boost::gregorian::date> dates;

	File(Repository *repo, string filename, int id);

	bool fileInTheRightFormat();
	void load(bool force=false);

	string getName();
	string getSciName();

	int getNumTimeSteps();
	int getNumPredictors();
	Predictor getPredictor(int index);

	boost::gregorian::date  getDate(int index);

	ViewRect getPredictorsQuad();
	ViewRect getProbSDQuad();

	void installInEvaluationEnvironmentAs(string varName);

	double sumVariableOnSpatialQuadRange(string var_name, 
					     ViewRect input_quad, 
					     int time_index);
	
	bool hasPredictors();
	
	vector<float> getVariableCurve(string variable_internal_name, 
				       int xmin, int xmax, int ymin, int ymax);
    };

    class Repository {
    private:

	vector<boost::gregorian::date>      default_dates;

	ViewRect                            default_lonlat_quad;

	vector<string>                      internalVariables;

	bool                                shortNameAlreadyExists(string shortname);

	File*                               getFileByShortName(string shortname);

	void initREnvironment();

	void initREnvironmentFunctions();

    public:

	RInside           R; // the R environment is where data and queries live

	map<string,File*> filename2file;

	vector<File*>     files;

	bool              last_eval_ok;

	Repository();

	void addFile(string filename, string shortname);

	void addFilesFromDirectory(string directory);

	bool hasDataFileExtension(string filename);

	Rcpp::NumericMatrix evalNumericMatrix(string expression);

	void runExpression(string expression);

	Rcpp::NumericVector evalNumericVector(string expression);

	int     evalInt(string expression);

	double  evalDouble(string expression);

	string evalString(string expression);

	bool    evalOK();

	void    clearREnvironment();
	void    removeFileFromMemory(File* f);          

	ViewRect getDefaultLonLatQuad();

	boost::gregorian::date  getDefaultDate(int index);

	int       getNumDefaultDates();

	int       getNumFiles();

	void setTime(int time);

	void setVariable(string variableName, int variableValue);
	
	double getCurveMaximum(File* f, string variable_internal_name,
			       int xMin, int xMax, int yMin, int yMax);

	File*     getFile(int index);
	File*     getFile(string shortname);

	datatypes::Grid* get2DGrid(string dataset_name, string variable_internal_name, int time);

	datatypes::Grid* get2DGrid(File* f, string variable_internal_name, int time);

	datatypes::Grid* get2DGridVariation(File* f, string variable_internal_name, int time);

	ViewRect getVariableQuad(string dataset_name, string variable_name);

	ViewRect getVariableQuad(File* dataset, string variable_name);

	vector<double> getPredictorsCurveValue(File* speciesFile, 
					       vector<string> predictorsNames, 
					       int time_index, int xmin, int xmax, 
					       int ymin, int ymax);

    }; // end class Repository

} // end namespace bvdata

#endif
