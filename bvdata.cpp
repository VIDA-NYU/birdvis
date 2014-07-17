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

#include <string>
#include <map>
#include <cfloat>
#include <cstdio>
#include <dirent.h>//Dir object
#include <cstddef> // NULL
#include <boost/tokenizer.hpp>
#include <ctime>
#include<algorithm>

#include "bvdata.hpp"

#define xDEBUG_BVDATAREPOSITORY
#define xDEBUG_BVDATAFILE


using namespace boost::gregorian;

/******************************
 * FileInvalidFormatException *
 ******************************/

bvdata::FileInvalidFormatException::FileInvalidFormatException(string message){
    this->message = message;    
}

bvdata::FileInvalidFormatException::~FileInvalidFormatException() throw (){
}

void bvdata::FileInvalidFormatException::report(){
    cout << this->message << endl;
}

/*************
 * Predictor *
 *************/

bvdata::Predictor::Predictor(){
}

bvdata::File::File(bvdata::Repository *repo, string filename, int id)
{
    this->repository = repo;
    this->id = id;
    this->filename = filename;
    this->loaded = false;
    this->thumb_photo_file = "";
    this->pie_chart_file = "";
    

    // use the filename radical as the 
    // name
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("/.");
    vector<string> tokens;
    tokenizer tk(this->filename, sep);
    for (tokenizer::iterator tok_iter = 
	     tk.begin();tok_iter != tk.end(); ++tok_iter)
    {
	tokens.push_back(*tok_iter);
    }
    if (tokens.size() > 1)
	this->name = tokens.at(tokens.size()-2);
    else
	this->name = this->filename;
}

string
bvdata::File::getName()
{
    return name;
}

string 
bvdata::File::getSciName(){
    return this->sci_name;
}

bool 
bvdata::File::hasPredictors(){
    //int numberOfVariables = this->repository->evalInt("length(" + this->shortname + ")");
    //return (numberOfVariables > 2);
    return (this->predictors.size() > 0);
}


vector<float> 
bvdata::File::getVariableCurve(string variable_internal_name, 
			       int xmin, int xmax, 
			       int ymin, int ymax){    
    vector<float> result;

    //R query
    stringstream ss;
    ss << "bv.curve(" << this->shortname << "$" << variable_internal_name << "," << 
	xmin << "," << xmax << "," << ymin << "," << ymax <<")";

    // numeric matrix
    Rcpp::NumericVector  M = this->repository->evalNumericVector(ss.str());
    if (!this->repository->evalOK())
    {
	cout << "   problem evaluating matrix." << endl;
	return result;
    }

    Rcpp::NumericVector::iterator it;

    for (it=M.begin(); it<M.end(); it++){
	result.push_back(*it);	
    }
    
return result;

}

void 
bvdata::File::installInEvaluationEnvironmentAs(string varName)
{
    this->load();
    stringstream ss;
    ss << varName << " = " << "FILES[[" << this->id << "]]$variables";
    repository->runExpression(ss.str());
}

int 
bvdata::File::getNumTimeSteps(){
    return this->num_time_steps; 
}

int bvdata::File::getNumPredictors(){
    return this->predictors.size();
}

bvdata::Predictor bvdata::File::getPredictor(int index){
    return this->predictors.at(index);
}

bool compareTags(bvdata::Predictor a, bvdata::Predictor b)
{
    return a.tag.compare(b.tag) < 0;
}

bool bvdata::File::fileInTheRightFormat(){
    //TO DO
    return true;
}

ViewRect 
bvdata::File::getPredictorsQuad(){
    return this->predictors_quad;
}

ViewRect 
bvdata::File::getProbSDQuad(){
    return this->quad;
}

void 
bvdata::File::load(bool force)
{
    if (!force && loaded)
	return;

#ifdef DEBUG_BVDATAFILE
    cout << "bvdata::File::load(...)"<< endl;
    cout << "   filename"<< this->filename << endl;
#endif

    bool checkFormat = fileInTheRightFormat();
    if(!checkFormat){
	stringstream sError;
	
	sError << "The file " << this->filename << " could not be loaded";
	
	throw FileInvalidFormatException(sError.str());
	return;
    }

    stringstream ss;

    ss << "load(\"" << this->filename << "\")" << endl;
    ss << "FILES[[" << this->id 
       << "]] = list(species=bv.data.list$bv.species, dates=bv.data.list$bv.dates,"
	" grid=bv.data.list$bv.grid.spec, variables=bv.data.list$bv.variables)" << endl;
    ss << this->shortname << " = bv.data.list$bv.variables" << endl;

    //
    repository->runExpression(ss.str());    

    // update name
    // TODO: throw and capture execptions if the fields are not defined in the file
    try{
	name = repository->evalString("bv.data.list$bv.species");
    }
    catch(...){
	throw FileInvalidFormatException("File in the wrong format: bv.data.list$bv.species is missing");
    }

    try{
	sci_name = repository->evalString("bv.data.list$bv.species_sci");
    }
    catch(...){
	// throw FileInvalidFormatException("File in the wrong format: bv.data.list$bv.species_sci is missing")	
	sci_name = "";
    }

    try{
	thumb_photo_file = repository->evalString("bv.data.list$photo_file");
    }
    catch(...){
	thumb_photo_file = "";
	// throw FileInvalidFormatException("File in the wrong format: bv.data.list$photo_file is missing");
    }

    try{
	pie_chart_file = repository->evalString("bv.data.list$pie_chart_file");
    }
    catch(...){
	pie_chart_file = "";
	// throw FileInvalidFormatException("File in the wrong format: bv.data.list$pie_chart_file is missing");
    }


    // collect some data
    Rcpp::NumericVector grid_spec = 
	repository->evalNumericVector("as.double(bv.data.list$bv.grid.spec)");
    this->quad.init(grid_spec[0], grid_spec[1], grid_spec[2], grid_spec[3]);
    this->xcells = (int) grid_spec[4];
    this->ycells = (int) grid_spec[5];
    this->num_time_steps = repository->evalInt("length(bv.data.list$bv.dates)");

    Rcpp::NumericVector grid_spec_predictors = 
	repository->evalNumericVector("as.double(bv.data.list$bv.grid.spec.predictors)");

#ifdef DEBUG_BVDATAFILE
    cout << "Predictors Data " << grid_spec_predictors[0] << " " 
	 << grid_spec_predictors[1] << " "
	 << grid_spec_predictors[2] << " "
	 << grid_spec_predictors[3] << " "
	 << grid_spec_predictors[4] << " "
	 << grid_spec_predictors[5] << endl;
#endif

    this->predictors_quad.init(grid_spec_predictors[0], grid_spec_predictors[1], 
			       grid_spec_predictors[2], grid_spec_predictors[3]);
    this->predictors_xcells = (int) grid_spec_predictors[4];
    this->predictors_ycells = (int) grid_spec_predictors[5];

    //populate the predictors vetor
    int numberOfPredictors = repository->evalInt("length(bv.data.list$bv.predictors)");

#ifdef DEBUG_BVDATAFILE
    cout << "Number of Predictors " << numberOfPredictors << endl;
#endif

    for(int i = 1 ; i <= (numberOfPredictors/2) ; i++){
	Predictor tmp;

	stringstream ss_internal;
	ss_internal << "bv.data.list$bv.predictors[[" << (2*i-1) << "]]";
	string internalName = repository->evalString(ss_internal.str());
#ifdef DEBUG_BVDATAFILE
	cout << "Internal " << internalName << endl;
#endif
	stringstream ss_tag;
	ss_tag << "bv.data.list$bv.predictors[[" << (2*i) << "]]";

	string tag = repository->evalString(ss_tag.str());
#ifdef DEBUG_BVDATAFILE
	cout << "Tag " << tag << endl;	
#endif
	tmp.internal_name = internalName;
	tmp.tag = tag;
	this->predictors.push_back(tmp);
    }

    //sort predictor in alphabetical order
    sort(predictors.begin(), predictors.end(), compareTags);

#ifdef DEBUG_BVDATAFILE
    cout << "   x:      " << quad.x << endl;
    cout << "   y:      " << quad.y << endl;
    cout << "   w:      " << quad.w << endl;
    cout << "   h:      " << quad.h << endl;

    cout << "   xcells: " << xcells << endl;
    cout << "   ycells: " << ycells << endl;
    cout << "NUMBER OF TIME STEPS = " << this->num_time_steps << endl;
#endif

    //string dates[this->num_time_steps];
    string x[this->num_time_steps];

    for(int i = 0 ; i < this->num_time_steps ; i++){
	stringstream ss;
	ss << "bv.data.list$bv.dates[[" << (i+1) << "]]$" ;
	//cout << ss.str() << endl;
	string sDate = ss.str();    
	

	int day = repository->evalInt(sDate + "mday");
	int month = repository->evalInt(sDate + "mon");
	month++; //month between 0 and 11

	int year = repository->evalInt(sDate + "year");	
	year += 1900;

	//string ithDate = repository->evalString(sDate);
	//cout << "Date " << i << " = " << month << "|" << day << "|" << year << endl;
	stringstream ssDate;
	ssDate << year << "-" << month << "-" << day;
	x[i] = ssDate.str();
    }


//     string x[] = { 
// 	"2009-01-05","2009-01-12","2009-01-19","2009-01-26","2009-02-02",
// 	"2009-02-09","2009-02-16","2009-02-23","2009-03-02","2009-03-09",
// 	"2009-03-16","2009-03-23","2009-03-30","2009-04-06","2009-04-13",
// 	"2009-04-20","2009-04-27","2009-05-04","2009-05-11","2009-05-18",
// 	"2009-05-25","2009-06-01","2009-06-08","2009-06-15","2009-06-22",
// 	"2009-06-29","2009-07-06","2009-07-13","2009-07-20","2009-07-27",
// 	"2009-08-03","2009-08-10","2009-08-17","2009-08-24","2009-08-31",
// 	"2009-09-07","2009-09-14","2009-09-21","2009-09-28","2009-10-05",
// 	"2009-10-12","2009-10-19","2009-10-26","2009-11-02","2009-11-09",
// 	"2009-11-16","2009-11-23","2009-11-30","2009-12-07","2009-12-14",
// 	"2009-12-21","2009-12-28"};

    for (int i=0;i<num_time_steps;i++)
    {
#ifdef DEBUG_BVDATAREPOSITORY
	cout << "processing " << x[i] << endl;
#endif
	
	struct tm tm_val;

	if (strptime(x[i].c_str(),"%Y-%m-%d",&tm_val) == NULL)
	    printf("error\n\n");
	
	    tm_val.tm_hour  = 0;
	    tm_val.tm_min   = 0;
	    tm_val.tm_sec   = 0;
	    tm_val.tm_wday  = 0;
	    tm_val.tm_yday  = 0;
	    tm_val.tm_isdst = 0;
	    
	    int year  = tm_val.tm_year + 1900;
	    int month = tm_val.tm_mon + 1;
	    int day   = tm_val.tm_mday;

#ifdef DEBUG_BVDATAREPOSITORY
	    cout << "year " << year << endl;
	    cout << "month " << month << endl;
	    cout << "day " << day << endl;
#endif
	    
	    dates.push_back(date(year,month,day));
    }
    
    //
    loaded = true;

#ifdef DEBUG_BVDATAFILE
    cout << "   Finished Loading File" << endl;
#endif


}

double
bvdata::File::sumVariableOnSpatialQuadRange(string var_name, 
					    ViewRect input_quad, 
					    int time_index)
{
#ifdef DEBUG_BVDATAFILE
    cout << "bvdata::File::sumVariableOnSpatialQuadRange(...)" << endl;
    
    cout << "   quad: "       << quad.str()       << endl;
    cout << "   input_quad: " << input_quad.str() << endl;
#endif

    int x0;
    int y0;
    int x1;
    int y1;

    stringstream ss;

    if(var_name.compare("p") == 0 || var_name.compare("sd") == 0){
	x0 = (int) ceil(xcells * (input_quad.x - quad.x)/quad.w);
	y0 = (int) ceil(ycells * (input_quad.y - quad.y)/quad.h);
	x1 = (int) (xcells * (input_quad.x + input_quad.w - quad.x)/quad.w);
	y1 = (int) (ycells * (input_quad.y + input_quad.h - quad.y)/quad.h);

	if ((x0 > x1) || (x1 < 0) || (x0 >= xcells) || 
	    (y0 > y1) || (y1 < 0) || (y0 >= ycells))
	    return 0.0;

	    x0 = max(x0,0);
	    x1 = min(x1,xcells-1);
	    y0 = max(y0,0);
	    y1 = min(y1,ycells-1);
    }
    else{
	x0 = (int) ceil(predictors_xcells * (input_quad.x - predictors_quad.x)/predictors_quad.w);
	y0 = (int) ceil(predictors_ycells * (input_quad.y - predictors_quad.y)/predictors_quad.h);
	x1 = (int) (predictors_xcells * (input_quad.x + input_quad.w - predictors_quad.x)/predictors_quad.w);
	y1 = (int) (predictors_ycells * (input_quad.y + input_quad.h - predictors_quad.y)/predictors_quad.h);
	
	if ((x0 > x1) || (x1 < 0) || (x0 >= predictors_xcells) || 
	    (y0 > y1) || (y1 < 0) || (y0 >= predictors_ycells))
	    return 0.0;

	x0 = max(x0,0);
	x1 = min(x1,predictors_xcells-1);
	y0 = max(y0,0);
	y1 = min(y1,predictors_ycells-1);
    }
#ifdef DEBUG_BVDATAFILE
    cout << "   x0: " << x0 << endl;
    cout << "   y0: " << y0 << endl;
    cout << "   x1: " << x1 << endl;
    cout << "   y1: " << y1 << endl;
#endif


    
    ss << "sum(FILES[[" << this->id << "]]$variables$" 
       << var_name << "[[" << (time_index+1) << "]][" << (x0+1) << ":" << (x1+1) << ","
       << (y0+1) << ":" << (y1+1) << "])";

    //cout << ss.str() << endl;

    double result = repository->evalDouble(ss.str());

    return result;
}

void bvdata::Repository::initREnvironmentFunctions(){
    string curve_function="\
bv.curve = function(lst, xmin, xmax, ymin, ymax)\n\
{\n\
    result = rep(0,length(lst))\n\
    for (i in 1:length(lst))\n\
    {\n\
        M = lst[[i]]\n\
        if (xmin == 1 && ymin == 1)\n\
             result[i] = M[xmax, ymax]\n\
        else if (xmin == 1)\n\
             result[i] = M[xmax, ymax] - M[xmax, ymin - 1]\n\
        else if (ymin == 1)\n\
             result[i] = M[xmax, ymax] - M[xmin - 1, ymax]\n\
        else\n\
             result[i] = M[xmax, ymax]   - M[xmax, ymin-1] -\n\
                         M[xmin-1, ymax] + M[xmin-1,ymin-1]\n\
    }\n\
    return(result)\n\
}\n\
";

    this->runExpression(curve_function);

    //

    string integral_function="\
bv.integral = function(lst, time, xmin, xmax, ymin, ymax)\n\
{\n\
        M = lst[[time]]\n\
        if (xmin == 1 && ymin == 1)\n\
             result = M[xmax, ymax]\n\
        else if (xmin == 1)\n\
             result = M[xmax, ymax] - M[xmax, ymin - 1]\n\
        else if (ymin == 1)\n\
             result = M[xmax, ymax] - M[xmin - 1, ymax]\n\
        else\n\
             result = M[xmax, ymax]   - M[xmax, ymin-1] -\n\
                         M[xmin-1, ymax] + M[xmin-1,ymin-1]\n\
    return(result)\n\
}\n\
";

    this->runExpression(integral_function);


    string getIndex_function="\
getIndex = function(lst, elmt)\n\
{\n\    
    for (i in 1:length(lst))\n\
    {\n\
        if(elmt == lst[[i]])\n\
           return(i)\n\
    }\n\
    return(-1)\n\
}\n\
";

    this->runExpression(getIndex_function);

}

void bvdata::Repository::initREnvironment(){
    // make sure there is enough room to 300 dataset
    this->runExpression("FILES = c(list(), rep(NA,300))");


    initREnvironmentFunctions();


    { // for the moment make a list of default dates

	string x[] = { 
	    "2009-01-05","2009-01-12","2009-01-19","2009-01-26","2009-02-02",
	    "2009-02-09","2009-02-16","2009-02-23","2009-03-02","2009-03-09",
	    "2009-03-16","2009-03-23","2009-03-30","2009-04-06","2009-04-13",
	    "2009-04-20","2009-04-27","2009-05-04","2009-05-11","2009-05-18",
	    "2009-05-25","2009-06-01","2009-06-08","2009-06-15","2009-06-22",
	    "2009-06-29","2009-07-06","2009-07-13","2009-07-20","2009-07-27",
	    "2009-08-03","2009-08-10","2009-08-17","2009-08-24","2009-08-31",
	    "2009-09-07","2009-09-14","2009-09-21","2009-09-28","2009-10-05",
	    "2009-10-12","2009-10-19","2009-10-26","2009-11-02","2009-11-09",
	    "2009-11-16","2009-11-23","2009-11-30","2009-12-07","2009-12-14",
	    "2009-12-21","2009-12-28"};

	for (int i=0;i<52;i++)
	{
#ifdef DEBUG_BVDATAREPOSITORY
	    cout << "processing " << x[i] << endl;
#endif

	    struct tm tm_val;
	    if (strptime(x[i].c_str(),"%Y-%m-%d",&tm_val) == NULL)
		printf("error\n\n");

	    tm_val.tm_hour  = 0;
	    tm_val.tm_min   = 0;
	    tm_val.tm_sec   = 0;
	    tm_val.tm_wday  = 0;
	    tm_val.tm_yday  = 0;
	    tm_val.tm_isdst = 0;

	    int year  = tm_val.tm_year + 1900;
	    int month = tm_val.tm_mon + 1;
	    int day   = tm_val.tm_mday;

#ifdef DEBUG_BVDATAREPOSITORY
	    cout << "year " << year << endl;
	    cout << "month " << month << endl;
	    cout << "day " << day << endl;
#endif

	    default_dates.push_back(date(year,month,day));
	}

    } // default_dates

}

bvdata::Repository::Repository() 
{
#ifdef DEBUG_BVDATAREPOSITORY
cout << "bvdata::Repository::Repository() " << endl;
#endif

    // init internal variables vector
    this->internalVariables.push_back("bv.dates");                
    this->internalVariables.push_back("bv.grid.spec");                
    this->internalVariables.push_back("bv.grid.spec.predictors");                
    this->internalVariables.push_back("bv.predictors");                
    this->internalVariables.push_back("bv.species");                
    this->internalVariables.push_back("bv.variables");                

    this->internalVariables.push_back("r");                
    this->internalVariables.push_back("c");
    this->internalVariables.push_back("t");                
    this->internalVariables.push_back("xmin");                
    this->internalVariables.push_back("xmax");                
    this->internalVariables.push_back("ymin");                
    this->internalVariables.push_back("ymax");                
    this->internalVariables.push_back("FILES");                

#ifdef DEBUG_BVDATAREPOSITORY
cout << "   bvdata::Repository::initREnvironment() " << endl;
#endif

initREnvironment();

    // default_lonlat_quad
    default_lonlat_quad = ViewRect(-125, 25, 58,25);
#ifdef DEBUG_BVDATAREPOSITORY
cout << "   finished Repository Constructor" << endl;
#endif
}

void 
bvdata::Repository::addFile(string filename, string shortname)
{
    // in R we should add one to the ID

    File *f = new File(this,filename,files.size()+1); 
    
    f->shortname = shortname;

    files.push_back(f);
    
    filename2file[filename] = f;

    
} // end add File


bool 
bvdata::Repository::hasDataFileExtension(string filename){

    // use the filename radical as the 
    // name
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("/.");
    vector<string> tokens;
    tokenizer tk(filename, sep);
    for (tokenizer::iterator tok_iter = 
	     tk.begin();tok_iter != tk.end(); ++tok_iter)
    {
	// cout << "token " << *tok_iter << endl;
	tokens.push_back(*tok_iter);
    }
    if (tokens.size() < 2)
	return false;

    string extension = tokens.at(tokens.size()-1);
    // cout <<  "extension " << extension << endl;
    if (extension.compare("RData") == 0)
	return true;
    else 
	return false;

} // end hasDataFileExtension

// auxiliary function

string 
getShortName(string fileName){

    int size = fileName.size();
    char shortName[size+1];

    int shortNameSize = 0;
    bool afterUnderScore = true;
    for(int i = 0 ; i < size ; i++){
	char charac = fileName.at(i);

	if(afterUnderScore && charac != '_'){
	    afterUnderScore = false;

	    if('A' <= charac && charac <= 'Z'){
		charac = 'a' + (charac - 'A');
	    }

	    shortName[shortNameSize] = charac;
	    shortNameSize++;	    
	    continue;
	}
	else if(charac == '_'){
	    //is initial
	    afterUnderScore = true;
	    continue;
	}
	else if(charac == '.'){
	    //reached extension
	    break;
	}
    }

    shortName[shortNameSize] = '\0';    

    return string(shortName);
}

bool bvdata::Repository::shortNameAlreadyExists(string shortname){
    int numFiles = this->getNumFiles();

//cout << "Number of Files " << numFiles << endl;

    for(int i = 0 ; i < numFiles ; i++){
          File* f = this->getFile(i);
          if(f->shortname.compare(shortname) == 0)
               return true;                  
    }

int numInternalVariables = this->internalVariables.size();

    for(int i = 0 ; i < numInternalVariables ; i++){
string varName = this->internalVariables.at(i);
          if(shortname.compare(varName) == 0)
               return true;                  
    }

    return false;
}

// the current version does not check if
// the shortname already exist

void    
bvdata::Repository::clearREnvironment(){
	stringstream ss;
	ss << "rm(list=ls()[-getIndex(ls(),\"FILES\")]);gc();";
	this->runExpression(ss.str());
	this->initREnvironmentFunctions();
}

void    
bvdata::Repository::removeFileFromMemory(bvdata::File* f){
    
//     if(f != NULL){
// 	stringstream ss;
// 	string sn = f->shortname;
// 	//list(species=bv.data.list$bv.species, dates=bv.data.list$bv.dates,"
// 	//" grid=bv.data.list$bv.grid.spec, variables=bv.data.list$bv.variables)" << 
// 	ss << "rm(bv.data.list);rm(" << f->shortname << ");FILES[[" << f->id << "]] = list();" << "gc();";
// 	this->runExpression(ss.str());	
//     }
}

void 
bvdata::Repository::addFilesFromDirectory(string directory)
{
    //cout << "Repository::addFilesFromDirectory " << directory << endl;

    DIR* currentDirectory = opendir(directory.c_str());

    if(currentDirectory == NULL)
	printf("Error while opening the directory %s\n", 
	       directory.c_str());

    struct dirent *dirEntry;

    while(1){

	dirEntry = readdir(currentDirectory);

	if (dirEntry == NULL)
	    break;

	string filename(directory+"/"+dirEntry->d_name);

	// cout << "directory   " << directory << endl;
	// cout << "trying file " << filename << endl;

	if(!this->hasDataFileExtension(filename))
	    continue;

	// cout << "It has the extension!!! " << filename << endl;

	// check if filename is already on the list
	if (filename2file.count(filename) > 0)
	    continue;

	// cout << "------------> add file " << filename << endl;

        //define shortname
	string shortname = getShortName(dirEntry->d_name);

        bool shortNameExists = this->shortNameAlreadyExists(shortname);
        int suffix = 1;

        while(shortNameExists){
             stringstream ss;
             ss << shortname << "." << suffix;
             suffix++;

	     shortname = ss.str();
	     shortNameExists = this->shortNameAlreadyExists(shortname);
        }


	// else add
	this->addFile(filename, shortname);

	// cout << "Entry Name " << dirEntry->d_name << " Shortname " << shortname << endl; 

    } // end scanning directory

} // end add FilesFrom Directory




void 
bvdata::Repository::runExpression(string expression)
{
    this->last_eval_ok = false;
    try {
#ifdef DEBUG_BVDATAREPOSITORY
	cout << "bvdata::Repository::runExpression(...)" << endl;
#endif
//cout << "Expression: " << expression << endl;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

#ifdef DEBUG_BVDATAREPOSITORY
	boost::char_separator<char> sep("\n");
	tokenizer tk(expression, sep);
	for (tokenizer::iterator tok_iter = 
		 tk.begin();tok_iter != tk.end(); ++tok_iter)
	{
	    cout << "   " << *tok_iter << endl;
	}
#endif
	
	R.parseEval(expression);
	this->last_eval_ok = true;
	
    } catch(std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    } catch(...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
}


Rcpp::NumericVector 
bvdata::Repository::evalNumericVector(string expression)
{
#ifdef DEBUG_BVDATAREPOSITORY
    cout << "bvdata::Repository::evalNumericVector(" << expression << ")"<< endl;
#endif
    this->last_eval_ok = false;
    try {

	SEXP ans = R.parseEval(expression);
	Rcpp::NumericVector result(ans); 

	// convert SEXP ans to a vector of doubles
	// for (int i=0; i< v.size(); i++) {           // show the result
	//     std::cout << "In C++ element " << i << " is " << v[i] << std::endl;
	// }
	this->last_eval_ok = true;
	return result;

    } catch(std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    } catch(...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
}

int
bvdata::Repository::evalInt(string expression)
{
#ifdef DEBUG_BVDATAREPOSITORY
    cout << "bvdata::Repository::evalInt(" << expression << ")"<< endl;
#endif
    this->last_eval_ok = false;
    try {

	SEXP ans = R.parseEval(expression);

	int result = Rcpp::as<int>((SEXP) ans);

	this->last_eval_ok = true;
	return result;

    } catch(std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    } catch(...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
}

double
bvdata::Repository::evalDouble(string expression)
{
#ifdef DEBUG_BVDATAREPOSITORY
    cout << "bvdata::Repository::evalDouble(" << expression << ")"<< endl;
#endif
    this->last_eval_ok = false;
    try {

	SEXP ans = R.parseEval(expression);
	double result = Rcpp::as<double>((SEXP) ans);

	this->last_eval_ok = true;
	return result;

    } catch(std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
	exit(1);
    } catch(...) {
        std::cerr << "Unknown exception caught" << std::endl;
	exit(1);
    }
}

// vector<struct tm>
// bvdata::Repository::evalTimeVector(string expression)
// {
//     cout << "bvdata::Repository::evalTimeVector(" << expression << ")"<< endl;
//     this->last_eval_ok = false;
//     try {

// 	SEXP ans = R.parseEval(expression);
// 	int result = Rcpp::as<time_t>((SEXP) ans);

// 	this->last_eval_ok = true;
// 	return result;

//     } catch(std::exception& ex) {
//         std::cerr << "Exception caught: " << ex.what() << std::endl;
//     } catch(...) {
//         std::cerr << "Unknown exception caught" << std::endl;
//     }
// }

Rcpp::NumericMatrix 
bvdata::Repository::evalNumericMatrix(string expression)
{
#ifdef DEBUG_BVDATAREPOSITORY
    cout << "bvdata::Repository::evalNumericMatrix(" << expression << ")"<< endl;
#endif

    this->last_eval_ok = false;
    try {

	SEXP ans = R.parseEval(expression);
	Rcpp::NumericMatrix result(ans); 

	// convert SEXP ans to a vector of doubles
	// for (int i=0; i< v.size(); i++) {           // show the result
	//     std::cout << "In C++ element " << i << " is " << v[i] << std::endl;
	// }
	this->last_eval_ok = true;
	return result;

    } catch(std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    } catch(...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
}


string 
bvdata::Repository::evalString(string expression)
{
#ifdef DEBUG_BVDATAREPOSITORY
    cout << "bvdata::Repository::evalNumericMatrix(" << expression << ")"<< endl;
#endif

    this->last_eval_ok = false;
    try {
	SEXP ans = R.parseEval(expression);
	string result = Rcpp::as<string>((SEXP) ans);

	this->last_eval_ok = true;
	return result;


    } catch(std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
	throw ex;
    } catch(...) {
        std::cerr << "Unknown exception caught" << std::endl;
	throw exception();
    }
}


bool 
bvdata::Repository::evalOK()
{
    return last_eval_ok;
}

ViewRect 
bvdata::Repository::getDefaultLonLatQuad()
{
    return default_lonlat_quad;
}

boost::gregorian::date 
bvdata::Repository::getDefaultDate(int index)
{
    return default_dates[index];
}

int
bvdata::Repository::getNumDefaultDates()
{
    return default_dates.size();
}

int       
bvdata::Repository::getNumFiles(){
    return this->files.size();
}

bvdata::File*     
bvdata::Repository::getFile(int index){
    if(index < 0 || index >= this->files.size())
       return NULL;

    return this->files.at(index);

}

bvdata::File*     
bvdata::Repository::getFile(string shortname){
    int numFiles = this->getNumFiles();
    for(int i = 0 ; i < numFiles ; i++){
	File* f = this->getFile(i);
	if((f->shortname).compare(shortname) == 0){
	    return f;
	}
    }

    return NULL;
}

boost::gregorian::date 
bvdata::File::getDate(int index)
{
#ifdef DEBUG_BVDATAFILE
    cout << "boost::gregorian::date bvdata::File::getDate(int index)"<< endl;
    cout << "   index "<< index << endl;
    cout << "   number of dates "<< this->dates.size() << endl;
#endif

    //struct tm tm_val = this->dates.at(index);

boost::gregorian::date d = this->dates.at(index);

#ifdef DEBUG_BVDATAFILE
    cout << "got tm struct"<< endl;
    cout << "   mon " << d.month() << endl;
    cout << "   day " << d.day() << endl;
    cout << "   year "<< d.year() << endl;
#endif   

    return d;//date(tm_val.tm_year , tm_val.tm_mon, tm_val.tm_mday);//default_dates[index];
}

void
bvdata::Repository::setTime(int time){
    stringstream ss;
    ss.str("");
    ss << "t = " << time;
    this->runExpression(ss.str());
}

void
bvdata::Repository::setVariable(string variableName, int variableValue){
    stringstream ss;
    ss.str("");
    ss << variableName << " = " << variableValue;
    this->runExpression(ss.str());
}

double 
bvdata::Repository::getCurveMaximum(File* f, string variable_internal_name,
                                    int xMin, int xMax, int yMin, int yMax){
#ifdef DEBUG_BVDATAREPOSITORY
    cout << "   double bvdata::Repository::getCurveMaximum(...)" << endl;
    cout << "   variable name " << variable_internal_name << endl;
    cout << "   xmin " << xMin << endl;
    cout << "   xmax " << xMax << endl;
    cout << "   ymin " << yMin << endl;
    cout << "   ymax " << yMax << endl;
#endif

	    stringstream ss;
	    ss << "max(bv.curve(" << f->shortname << "$" << variable_internal_name << "," << xMin << "," << 
                   xMax << "," << yMin << "," <<  yMax <<"))";

	    double result = this->evalDouble(ss.str());	    
            return result;
}

datatypes::Grid* 
bvdata::Repository::get2DGrid(bvdata::File* f, string variable_internal_name, int time){
    // numeric matrix
    //this->setTime(time_index + 1);

    if(f == NULL)
	return NULL;

    ViewRect quad = this->getVariableQuad(f, variable_internal_name);

    stringstream ss;
    ss << f->shortname << "$" << variable_internal_name << "[[" << time  << "]]";
   
    string expression = ss.str();

    Rcpp::NumericMatrix  M = this->evalNumericMatrix(expression);
    if (!this->evalOK())
    {
	return NULL;
    }

    int xcells = M.nrow();
    int ycells = M.ncol();

    datatypes::Grid *grid = new Grid(xcells, ycells, quad);
    grid->setValue(0,0,M(0,0));

    double minValue = 0;
    int forIndex = -1;

    for (int y=1;y<ycells;y++){
        double v = M(0,y)-M(0,y-1);
	if(v < minValue){
	    minValue = v;
	    forIndex = 1;
	}
	if(fabs(v) < 1e-9)
	    v = 0.0;
	grid->setValue(0,y,v);
    }
    for (int x=1;x<xcells;x++){	
	double v = M(x,0)-M(x-1,0);
	if(v < minValue){
	    minValue = v;
	    forIndex = 2;
	}
	if(fabs(v) < 1e-8)
	    v = 0.0;

	grid->setValue(x,0,v);
    }
    for (int y=1;y<ycells;y++)
	for (int x=1;x<xcells;x++){
	 
	    double v = M(x,y) + M(x-1,y-1) - M(x,y-1) - M(x-1,y);

	    if(v < minValue){
		minValue = v;
		forIndex = 3;
	    }

	    if(fabs(v) < 1e-9)
		v = 0.0;

	    grid->setValue(x,y,v);
	}

return grid;
}

datatypes::Grid* bvdata::Repository::get2DGrid(string dataset_name, string variable_internal_name, int time){
   
    File* f = this->getFile(dataset_name);
    return this->get2DGrid(f, variable_internal_name, time);

}

bvdata::File* bvdata::Repository::getFileByShortName(string shortname){
    
    File* f = NULL;

    int numberOfFiles = this->getNumFiles();

    for(int i = 0 ; i < numberOfFiles ; i++){
	f = this->getFile(i);
	if((f->shortname).compare(shortname) == 0)
	    return f;
    }

    return f;
}

ViewRect bvdata::Repository::getVariableQuad(string dataset_name, string variable_name){
//     if(variable_name.compare("p") == 0 || variable_name.compare("sd") == 0)
// 	return this->getDefaultLonLatQuad();

//     int numberOfPredictors = f->getNumPredictors();

//     for(int i = 0 ; i < numberOfPredictors ; i++){
	
// 	Predictor p = getPredictor(i);

// 	if(variable_name.compare(p.internal_name) == 0)
// 	    return f->predictors_quad;
//     }
    File* f  = this->getFileByShortName(dataset_name);
    return this->getVariableQuad(f, variable_name);
}

ViewRect bvdata::Repository::getVariableQuad(File* dataset, string variable_name){
    File* f  = dataset;

    if(variable_name.compare("p") == 0 || variable_name.compare("sd") == 0)
	return this->getDefaultLonLatQuad();

    int numberOfPredictors = f->getNumPredictors();

    for(int i = 0 ; i < numberOfPredictors ; i++){
	
	Predictor p = f->getPredictor(i);

	if(variable_name.compare(p.internal_name) == 0)
	    return f->predictors_quad;
    }
}

datatypes::Grid* bvdata::Repository::get2DGridVariation(bvdata::File* f, string variable_internal_name, int time){

// 	string expression = 
// 	    f->shortname+"$p[[t]] - " + 
// 	    f->shortname+"$p[[ if(t==1) t else t-1 ]]";

	int previoustime = time;
	if(time > 1)
	    previoustime--;


	if(f == NULL)
	    return NULL;

	ViewRect quad = this->getVariableQuad(f, variable_internal_name);

	stringstream ss;
	ss << f->shortname << "$" << variable_internal_name << "[[" << time  << "]] - " <<
	    f->shortname << "$" << variable_internal_name << "[[" <<  previoustime << "]]";
   
	string expression = ss.str();

	Rcpp::NumericMatrix  M = this->evalNumericMatrix(expression);
	if (!this->evalOK())
	{
	    return NULL;
	}
	
	int xcells = M.nrow();
	int ycells = M.ncol();

	datatypes::Grid *grid = new Grid(xcells, ycells, quad);
	grid->setValue(0,0,M(0,0));

	double minValue = 0;
	int forIndex = -1;

	for (int y=1;y<ycells;y++){
	    double v = M(0,y)-M(0,y-1);
	    if(v < minValue){
		minValue = v;
		forIndex = 1;
	    }
	    if(fabs(v) < 1e-9)
		v = 0.0;
	    grid->setValue(0,y,v);
	}
	for (int x=1;x<xcells;x++){	
	    double v = M(x,0)-M(x-1,0);
	    if(v < minValue){
		minValue = v;
		forIndex = 2;
	    }
	    if(fabs(v) < 1e-8)
		v = 0.0;
	    
	    grid->setValue(x,0,v);
	}
	for (int y=1;y<ycells;y++)
	    for (int x=1;x<xcells;x++){
		
		double v = M(x,y) + M(x-1,y-1) - M(x,y-1) - M(x-1,y);

		if(v < minValue){
		    minValue = v;
		    forIndex = 3;
		}

		if(fabs(v) < 1e-9)
		    v = 0.0;
		
		grid->setValue(x,y,v);
	    }
	
	return grid;
}

vector<double> 
bvdata::Repository::getPredictorsCurveValue(File* speciesFile, 
					    vector<string> predictorsNames, 
					    int time_index, int xmin, int xmax, 
					    int ymin, int ymax){
    vector<double> result;

    stringstream ss;
    
    int numberOfActiveTags = predictorsNames.size();

    ss << "c(";
    for(int i = 0 ; i < numberOfActiveTags ; i++){
	if (i > 0)
	    ss << ", ";
	//getData
	ss << "bv.integral(";
	ss << speciesFile->shortname;
	ss << "$";
	ss << predictorsNames[i];
	ss << ", ";
	ss << (time_index);
	ss << ",";
	ss << (xmin) << "," << (xmax) << "," << (ymin) << "," << (ymax) << ")";
    }
    ss << ")";

    //get value
    Rcpp::NumericVector predictorIntensity = this->evalNumericVector(ss.str());
    
    if (!this->evalOK())
    {
	cout << "   problem evaluating vector." << endl;
	return result;
    }

    Rcpp::NumericVector::iterator it;

    for (it=predictorIntensity.begin() ; it<predictorIntensity.end() ; it++){
	double value = *it;
	result.push_back(value);
    }

    return result;
}

