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

#include<algorithm>

#include "csv2grid.hpp"
#include "Tokenizer.hpp"
#include "Locations.hpp"



Csv2grid::Csv2grid(){
}

int getMonthIndex(string monthName){
    if(monthName.compare("Jan") == 0){
	return 1;
    }
    else if(monthName.compare("Feb") == 0){
	return 2;
    }
    else if(monthName.compare("Mar") == 0){
	return 3;
    }
    else if(monthName.compare("Apr") == 0){
	return 4;
    }
    else if(monthName.compare("May") == 0){
	return 5;
    }
    else if(monthName.compare("Jun") == 0){
	return 6;
    }
    else if(monthName.compare("Jul") == 0){
	return 7;
    }
    else if(monthName.compare("Aug") == 0){
	return 8;
    }
    else if(monthName.compare("Sep") == 0){
	return 9;
    }
    else if(monthName.compare("Oct") == 0){
	return 10;
    }
    else if(monthName.compare("Nov") == 0){
	return 11;
    }
    else if(monthName.compare("Dec") == 0){
	return 12;
    }
    else{
	return -1;
    }
}

void Csv2grid::getDayMonth(string date, int* day, int* month){
    //remove "..."
    date = date.substr(1,date.size()-2);

    Tokenizer tk(date, "_");

    string s = tk.next();

    //At this point s holds the name of the month
    *month = getMonthIndex(s);

    s = tk.next();

    int d;
    istringstream myStream(s);

    myStream>>d;
    *day = d;
}

bool isInsideCellIJ(double currentLon, double currentLat,
		    double minLon, double minLat,
		    double maxLon, double maxLat,
		    int resX, int resY,
		    int indexRow, int indexColumn){
    double deltaX   = (maxLon - minLon)/((double)resX);
    double cellXMin = indexColumn*deltaX + minLon;
    double cellXMax = cellXMin + deltaX;

    bool satisfiesXCondition = 
	(cellXMin <= currentLon) && (currentLon <= cellXMax);

    double deltaY = (maxLat - minLat)/((double)resY);
    double cellYMin = indexRow*deltaY + minLat;
    double cellYMax = cellYMin + deltaY;


    bool satisfiesYCondition = 
	(cellYMin <= currentLat) && (currentLat <= cellYMax);

    return satisfiesYCondition && satisfiesXCondition;

}

Species* Csv2grid::parseFile(string mfilename, string lfilename,
					int resX, int resY){
    //load locations
    cout << "Reading " << mfilename << endl;
    cout << "Get Locations" << endl;
    Locations *loc = new Locations(lfilename);
    cout << "Locations file read" << endl;
    
    double minLon =  180.0;
    double maxLon = -180.0;
    double minLat =  180.0;
    double maxLat = -180.0;

    int numLoc = loc->getNumLocations();
    cout << "Get Boundaries" << endl;
    {// get min-max lon-lat	
	
	for(int i = 0 ; i < numLoc ; i++){
	    double currentLat;
	    double currentLon;
	    loc->getLocation(i,&currentLat, &currentLon);

	    //test lon
	    if(currentLon < minLon){
		minLon = currentLon;
	    }
	    if(currentLon > maxLon){
		maxLon = currentLon;
	    }

	    //test lat
	    if(currentLat < minLat){
		minLat = currentLat;
	    }
	    if(currentLat > maxLat){
		maxLat = currentLat;
	    }	    
	}
    }

    cout << "MinLon " << minLon << " maxLon " << maxLon << 
	" minLat " << minLat << " maxLat " << maxLat << endl;

    Species *spec = new Species();

    //setup species name
    Tokenizer token1(mfilename, "/");
    token1.next();
    token1.next();
    string tempStr = token1.next();
    Tokenizer token2(tempStr, ".");
    string name = token2.next();
    //cout << "Species name " << name << endl;
    spec->setName(name);
    
      
    //build measurement sets
    
    ifstream file;
    file.open(mfilename.c_str());
	
    string line;
    cout << "Building Measurement sets" << endl;
    // read days and years
    {
	getline(file,line);	    
	Tokenizer token(line, ",");

	// throw away "" 
	token.next();

	//first valid token
	string temp = token.next();

	while(temp.compare("") != 0){
	    int day;
	    int month;

	    this->getDayMonth(temp, &day, &month);

	    //change the contructor to get day and year
	    // integers
	    MeasureSet* measurement = 
		new MeasureSet(day,month,minLon,maxLon,
			       minLat,maxLat,
			       resX,resY);
	    spec->addMeasureSet(measurement);
	    temp = token.next();
	}
    }
    cout << "Measure sets created" << endl;


    //read measurements
    int numDays = spec->getNumberOfMeasureSets();
    // cout << "NumLoc " << numLoc <<  endl;
    // cout << "NumDays " << numDays <<  endl;    

    //alocate memory
    double** measurements = new double*[numLoc];
    for(int i = 0 ; i < numLoc ; i++){
	measurements[i] = new double[numDays];
    }

    cout << "Measure sets allocated" << endl;    
    int countLine = 0;

    while (! file.eof() ){

	string temp;
	try {

	    getline (file,line);
	    //cout << line  << endl;
	    int countCol = 0;

	    if (line.length() == 0 || line.length() == 1)
		continue;	
	    Tokenizer token(line, ",");
	
	    // throw away line index  
	    token.next();
		
	    //first measurementsurement
	    temp = token.next();
		
	    while(temp.compare("") != 0){
		double value = atof(temp.c_str());
		
		measurements[countLine][countCol] = value; 
		++countCol;
		temp = token.next();
		//cout << "Token " << temp  << endl;
	    }

	    countLine++;		
	}
	catch (...) {
	    cout << "(WARNING) could not read line" << endl; 
	}
    }

    double min = 100.0;
    double max = -1;

    int rowMin = -1;
    int colMin = -1;

    int rowMax = -1;
    int colMax = -1;

    for(int i = 0 ; i < numLoc ; i++){
	for(int j = 0 ; j < numDays ; j++){
	    if(measurements[i][j] < min){
		min = measurements[i][j];
		rowMin = i;
		colMin = j;
	    }
	    if(measurements[i][j] > max){
		max = measurements[i][j];
		rowMax = i;
		colMax = j;
	    }
	}
    }

    cout << "Species min " << min << " Species max " << max << endl;
    cout << "Loc min " << rowMin << " Day min " << colMin << endl;
    cout << "Loc max " << rowMax << " Day max " << colMax << endl;


    file.close();
    
    cout << "Finished reading" << endl;

    // cout << "printing measurements" << endl;
    // for(int k = 1 ; k < 2 ; k++){
    // 	for(int l = 0 ; l < 20 ; l++){		
    // 	    cout << measurements[k][l] << " ";
    // 	}
    // 	cout << endl;
    // } 

    //process grid    
    double grid[resY][resX];
    int gridCount[resY][resX];

  



    // //for each cell in the grid
    // for(int l = 1 ; l < 2 ; l++){
    // 	for(int k = 10 ; k < 11 ; k++){

    // 	    //compute the value in the grid
    // 	    double prod = 1.0;
    // 	    int count = 0;


    //         //cell coordinates
    // 	    double deltaX   = (maxLon - minLon)/((double)resX);
    // 	    double cellXMin = k*deltaX + minLon;
    // 	    double cellXMax = cellXMin + deltaX;
	    
    // 	    double deltaY = (maxLat - minLat)/((double)resY);
    // 	    double cellYMin = l*deltaY + minLat;
    // 	    double cellYMax = cellYMin + deltaY;

    // 	    cout << "Cell = [ " << cellXMin << "," << cellXMax << "] x ["
    // 		 << cellYMin << "," << cellYMax << "]" << endl;

    // 	    for(int i = 0 ; i < numLoc ; i++){

    // 		double currentLat;
    // 		double currentLon;
    // 		loc->getLocation(i,&currentLat, &currentLon);

    // 		if(isInsideCellIJ(currentLon, currentLat,
    // 				  minLon,  minLat,
    // 				  maxLon,  maxLat,
    // 				  resX,  resY,
    // 				  l, k)){			
    // 		    // cout << "Point (" << currentLon << ", " << currentLat << ") is in " << 
    // 		    // 	    "Cell = [ " << cellXMin << "," << cellXMax << "] x "
    // 		    // 	  << cellYMin << "," << cellYMax << "] its measurement is "
    // 		    // 	 << measurements[i][20] << endl;

    // 		    count++;
    // 		    //double weight = (1.0-measurements[i][j]);
    // 		    //cout << "weight " << weight << endl;
    // 		    //prod *= (1-measurements[i][j]);
    // 		    //break;
    // 		}
    // 	    }
    // 	    //cout << "Produtorio " << prod << endl;
    // 	    //grid[l][k] = 1-prod;
    // 	}	    	    
    // }	








    //initialize grid
    for(int k = 0 ; k < resX ; k++){
	for(int l = 0 ; l < resY ; l++){		
	    //grid[l][k] = 1.0;
	    grid[l][k] = 0.0;
	    gridCount[l][k] = 0.0;
	}
    } 


    //for each day
    for(int j = 0 ; j < numDays ; j++){	   
	cout << "Day " << j << endl;
	MeasureSet* measureSet = spec->getMeasureSet(j);	    


	//compute the value in the grid
	for(int i = 0 ; i < numLoc ; i++){
	    
	    double currentLat;
	    double currentLon;
	    loc->getLocation(i,&currentLat, &currentLon);


	    int indexXCell = (int)(((currentLon - minLon)/(maxLon - minLon))*resX);	    
	    int indexYCell = (int)(((currentLat - minLat)/(maxLat - minLat))*resY);
	    
	    //grid[indexYCell][indexXCell] *= (1.0-measurements[i][j]); 
	    grid[indexYCell][indexXCell] += measurements[i][j]; 
	    gridCount[indexYCell][indexXCell] += 1; 	    
	}


	for(int k = 0 ; k < resX ; k++){
	    for(int l = 0 ; l < resY ; l++){		
		//measureSet->set_value_at(l, k, 1.0 - grid[l][k]);
		//grid[l][k] = 1.0;		
		if(gridCount[l][k] == 0)
		    measureSet->set_value_at(l, k, 0);
		else
		    measureSet->set_value_at(l, k, (grid[l][k]/gridCount[l][k]));
		grid[l][k] = 0.0;		
		gridCount[l][k] = 0.0;		
	    }
	} 
    }

    cout << "Finished!" << endl;

    // cout << "printing grid" << endl;
    // for(int k = 0 ; k < resX ; k++){
    // 	for(int l = 0 ; l < resY ; l++){		
    // 	    cout << grid[l][k] << " ";
    // 	}
    // 	cout << endl;
    // } 

   

    //release memory
    for(int i = 0 ; i < numLoc ; i++){
	delete[] measurements[i];
    }

    delete[] measurements;
    return spec;
}

bool compare_predictors(Predictor *a, Predictor *b)
{
    return a->getName().compare(b->getName()) < 0;
}


PredictorList* Csv2grid::parseFile_predictors(string filename){

    //load locations    
    double minLon =  180.0;
    double maxLon = -180.0;
    double minLat =  180.0;
    double maxLat = -180.0;

    vector<double> latitudes;
    vector<double> longitudes;

    ifstream file;
	
    //read latitudes and longitudes
    file.open(filename.c_str());	    
    
    if(file.fail()){	
	cout << "Error while openning file " << filename << endl;
	return NULL;
    }

    //create pointSets
    vector<Predictor*> *predictors = new vector<Predictor*>();

    try { // throw away first line
	string line;
	getline (file,line);
    }
    catch (...) {
	cout << "(WARNING) could not read line" << endl; 
    }



    //elevation
    string names[] = { "Elevation", "Open Water" , 
		       "Ice-Snow", "Developed-Open", "Developed-Low",  
		       "Developed-Med", "Developed-High", "Barren Land",  
		       "Forest-Deciduous", "Forest-Evergreen",	"Forest-Mixed",  
		       "Shrub-scrub", "Grassland", "Pasture",  "Cultivated Crops", 
		       "Woody Wetlands",   "Emergent Wetlands"};
    
    for (int i=0;i<17;i++)
    {
	predictors->push_back(new Predictor(names[i],80));
    }


     while (! file.eof() ){
	 string line;
	 getline (file,line);
	 
	 if (line.length() == 0)
	     continue;
	 
	 Tokenizer token(line, ",");

	 string temp;

	 // X 
	 token.next();

	 //throw away "xcenter"
	 string longitude = token.next();
	 double lon = atof(longitude.c_str());

	 //throw away "ycenter"
	 string latitude = token.next();
	 double lat = atof(latitude.c_str());
	    
	 //throw away "region.number"
	 token.next();

	 //throw away "region.mc"
	 token.next();

	 //throw away "time.intervals"
	 string day = token.next();
	 int dayInt = atoi(day.c_str());

	 //throw away "unique.locs"
	 token.next();

	 //throw away "I.STATIONARY"
	 token.next();

	 //throw away "YEAR"
	 token.next();

	 //throw away "DAY"
	 token.next();

	 //throw away "TIME"
	 token.next();
 
	 //throw away "EFFOT_HRS"
	 token.next();

	 //throw away "EFFORT_DISTANCE_KM"
	 token.next();

	 //throw away "NUMBER_OBSERVERS"
	 token.next();

	 int count = 0;

	 for (int i=0;i<17;i++)
	 {
	     temp = token.next();

	     PointSet* psDayInt = predictors->at(count)->
		 getPointSet(dayInt - 1);

	     double value = atof(temp.c_str());

	     psDayInt->addPoint(new Point(lon, lat, value));
	     count++;
	 }

     }

     file.close();

     std::sort(predictors->begin(),predictors->end(),compare_predictors);



     return new PredictorList(predictors);
}

// int main(){
//     cout << "I'm gonna have myself elected" << endl;
//     Csv2grid *parser = new Csv2grid();
//     parser->parseFile("../data/Eastern_Wood-Pewee.pred.st.cube.csv", 
// 		      "../data/common.locs.csv", 250, 125);
//     return 0;
// }
