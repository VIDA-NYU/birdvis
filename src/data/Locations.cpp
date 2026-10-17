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

#include "Locations.hpp"
#include "Tokenizer.hpp"

Locations::Locations(){
}

Locations::Locations(string locationsfile){
    
    // get latitudes and longitudes 
    {
	ifstream file;
	
	file.open(locationsfile.c_str());	    
	
	if(file.fail()){	
	    cout << "Error while openning file " << locationsfile << endl;
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

		// throw index away 
		token.next();

		string latitude_st  = token.next();
		string longitude_st = token.next();
		double longitude = atof(longitude_st.c_str());
		double latitude  = atof(latitude_st.c_str());
		
		//cout << longitude << "," << latitude << endl;
		
		this->longitudes.push_back(longitude);
		this->latitudes.push_back(latitude);
	    }
	    catch (...) {
		cout << "(WARNING) could not read line" << endl; 
	    }
	}

	file.close();
    }   
}

int Locations::getNumLocations(){

    //this is supposed to be equal to longitudes.size()
    return this->latitudes.size();

}

void Locations::getLocation(int index, double* lat, double* lon){
    *lat = this->latitudes.at(index);
    *lon = this->longitudes.at(index);
}
