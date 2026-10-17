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

#include "Species.hpp"
#include "Tokenizer.hpp"

Species::Species() {
    this->id = 0; 
    this->name = "";
}

Species::Species(string filename) {
    this->id = 0; // Species::getNextIndex();	
    this->name = filename;
    this->readCsvMeasureSets(filename);
}

Species::~Species()
{
    for (int i=0;i<this->measure_sets.size();i++)
	delete measure_sets.at(i);
}

int Species::getId(){
    return this->id;
}

string Species::getName(){
    return this->name;
}

void Species::setName(string name){
    this->name = name;
}

MeasureSet* Species::getMeasureSet(int index)
{
    return this->measure_sets.at(index % this->measure_sets.size());
}

int Species::getNumberOfMeasureSets()
{
    return this->measure_sets.size();
}

vector<MeasureSetCenter> *Species::getMeanPath()
{
    vector<MeasureSetCenter> *result = new vector<MeasureSetCenter>();
    for (int i=0;i<this->getNumberOfMeasureSets();i++)
    {
	result->push_back(this->getMeasureSet(i)->getCenter());
    }
    return result;
}

void Species::getDayYear(string date, int* day, int* year){
    //remove "..."
    date = date.substr(1,date.size()-2);

    Tokenizer tk(date, ".");

    string s = tk.next();
    int d;
    istringstream myStream(s);

    myStream>>d;
    *day = d;

    s = tk.next();
    int y;
    istringstream myStream2(s);

    myStream2>>y;
    *year = y;
}


void Species::consolidate()
{
    if (measure_sets.size() == 0)
    {
	min_value = 0;
	max_value = 0;
	return;
    }
    measure_sets.at(0)->get_value_range(min_value, max_value);
    MeasureSet *m;
    for (int i=1;i<measure_sets.size();i++)
    {
	m = measure_sets.at(i);
	double min_candidate, max_candidate;
	m->get_value_range(min_candidate,max_candidate);
	if (min_candidate < min_value)
	    min_value = min_candidate;
	if (max_candidate > max_value)
	    max_value = max_candidate;
    }
}

void Species::readCsvMeasureSets(string filename)
{
    set<double> latitudes;
    set<double> longitudes;

    // get latitudes and longitudes 
    {
	ifstream file;
	
	file.open(filename.c_str());	    
	
	if(file.fail()){	
	    cout << "Erro while openning file " << filename << endl;
	    return;
	}

	
	string line;

	// throw away first line
	getline(file,line);


	//read measurements
	while (! file.eof() ){

	    try {
		getline (file,line);

		if (line.length() == 0)
		    continue;

		Tokenizer token(line, ",");

		// throw away 
		token.next();
		string longitude_st = token.next();
		string latitude_st  = token.next();
		double longitude = atof(longitude_st.c_str());
		double latitude  = atof(latitude_st.c_str());
		
		//cout << longitude << "," << latitude << endl;
		
		longitudes.insert(longitude);
		latitudes.insert(latitude);
	    }
	    catch (...) {
		cout << "(WARNING) could not read line" << endl; 
	    }
	}

	file.close();
    }    

    // cout << "x_samples, y_samples " << longitudes.size() << "," << latitudes.size() << endl; 
    // cout << "x_min, x_max " << *longitudes.begin() << "," << *longitudes.rbegin() << endl; 
    // cout << "y_min, y_max " << *latitudes.begin() << "," << *latitudes.rbegin() << endl; 

    //build reverse index latitudes
    map<double,int> reverse_index_lat;
    set<double>::iterator it;
    int count = 0;

    for ( it=latitudes.begin() ; it != latitudes.end(); it++ ){
	reverse_index_lat[*it] = count;
	++count;
    }

    //build reverse index longitude
    count = 0;
    map<double,int> reverse_index_lon;
    for ( it=longitudes.begin() ; it != longitudes.end(); it++ ){
	reverse_index_lon[*it] = count;
	++count;
    }

    int num_x_samples = longitudes.size();
    int num_y_samples = latitudes.size();

    double x_min = *longitudes.begin();
    double x_max = *longitudes.rbegin();
    double y_min = *latitudes.begin();
    double y_max = *latitudes.rbegin(); 

    // vector<MeasureSet*> *result = new vector<MeasureSet*>();

    cout << "First pass done" << endl;

    // second pass - build measurement sets
    {
	ifstream file;
	file.open(filename.c_str());
	
	string line;

	// read days and years
	{
	    getline(file,line);	    
	    Tokenizer token(line, ",");

	    // throw away "" 
	    token.next();

	    // throw away "xxx"
	    token.next();

	    // throw away "yyy"
	    token.next();

	    //first valid token
	    string temp = token.next();

	    while(temp.compare("") != 0){
		int day;
		int year;

		this->getDayYear(temp, &day, &year);

		//change the contructor to get day and year
		// integers
		MeasureSet* measurement = 
		    new MeasureSet(day,year,x_min,x_max,
				   y_min,y_max,
				   num_x_samples,num_y_samples);
		measure_sets.push_back(measurement);
		temp = token.next();
	    }
	}
	cout << "Measure sets created" << endl;

	//read measurements
	while (! file.eof() ){
	    string temp;

	    try {
		getline (file,line);
		if (line.length() == 0)
		    continue;
	
		Tokenizer token(line, ",");
		// throw away line index  
		token.next();
		// read longitude  
		temp = token.next();
		double longitude =  atof(temp.c_str());
		// read latitude  
		temp = token.next();
		double latitude =  atof(temp.c_str());		
		//first measurement
		temp = token.next();
		int count = 0;

		int x = reverse_index_lon[longitude];
		int y = reverse_index_lat[latitude];
		
		while(temp.compare("") != 0){
		    double value = atof(temp.c_str());
		    MeasureSet* measure = measure_sets.at(count);
		    measure->set_value_at(y,x,value);
		    ++count;
		    temp = token.next();
		}
		
	    }
	    catch (...) {
		cout << "(WARNING) could not read line" << endl; 
	    }
	}

	file.close();
    }
    cout << "Finished reading" << endl;
    // this->measure_sets = result;
    // MeasureSet *testSet = result->at(0);    
    // testSet->to_string(true);
}

void Species::addMeasureSet(MeasureSet *measure){
    this->measure_sets.push_back(measure);
}

void Species::toString(bool verbose){
    cout << "Species toString" << endl;
    cout << "Species " << this->name << endl;
    cout << "Species ID = " << id << endl;
    cout << "Species MeasureSets size = " << this->measure_sets.size() << endl;
    // if(verbose){
    // 	this->measure_sets.to_string(verbose);
    // }
}
// int main(){
 
//     Species *colinus = new Species("../data/colinus.csv");
//     return 0;

// }

// int main(){
//     return 0;
// }
