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

#ifndef SPECIES_H_
#define SPECIES_H_

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <vector>
#include <algorithm>
#include <map>
#include <set>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>


#include "MeasureSet.hpp"

using namespace std;

class Species {
    ///////////////////////////////////////////////////////////////////
    // BOOST SERIALIZATION STUFF
    friend class boost::serialization::access;
    ///////////////////////////////////////////////////////////////////
public:
    double min_value;
    double max_value;

private:
    //members
    int                  id;
    vector<MeasureSet*>  measure_sets;
    string               name;

    //member functions
    void readCsvMeasureSets(string inputCsvFile);
    void getDayYear(string date, int* day, int* year);

    // static int           nextIdAvl;

    ///////////////////////////////////////////////////////////////////
    // BOOST SERIALIZATION STUFF
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
    	ar & id;
    	ar & name;
    	ar & measure_sets;
	ar & min_value;
	ar & max_value;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
    	ar & id;
    	ar & name;
    	ar & measure_sets;
	ar & min_value;
	ar & max_value;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    //////////////////////////////////////////////////////////////////

public:
    Species();
    ~Species();
    Species(string filename);

    int                       getNumberOfMeasureSets();
    int                       getId();
    string                    getName();
    MeasureSet*               getMeasureSet(int index);
    vector<MeasureSetCenter> *getMeanPath();
    void addMeasureSet(MeasureSet *measure);
    void toString(bool verbose);
    void setName(string name);
    void consolidate();

    // //manage id's
    // static int getNextIndex(){
    // 	return Species::nextIdAvl++;
    // }

};
// int Species::nextIdAvl = 0;

///////////////////////////////////////////////////////
// BOOST
BOOST_CLASS_VERSION(Species, 0)
///////////////////////////////////////////////////////

#endif /* SPECIES_H_ */
