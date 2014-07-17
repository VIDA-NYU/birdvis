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

#ifndef LOCATIONS_H_
#define LOCATIONS_H_

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <vector>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

class Locations {

public:

    int getNumLocations();
    void getLocation(int index, double* lat, double* lon);

private:
    //members
    vector<double>  latitudes;
    vector<double>  longitudes;

    //member functions
    void readCsvMeasureSets(string inputCsvFile);
    void getDayYear(string date, int* day, int* year);  

public:
    Locations();
    
    Locations(string locationsfile);

    // int                       getNumberOfMeasureSets();
    // int                       getId();
    // string                    getName();
    // MeasureSet*               getMeasureSet(int index);
    // vector<MeasureSetCenter> *getMeanPath();
    // void addMeasureSet(MeasureSet *measure);
    // void toString(bool verbose);
    // void setName(string name);
    // void consolidate();

};

#endif /* SPECIES_H_ */

// int main(){
//     cout << "Testando" << endl;
//     Locations* loc = new Locations("../data/common.locs.csv");
//     return 0;
// }
