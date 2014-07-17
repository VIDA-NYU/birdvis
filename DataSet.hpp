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

#ifndef DATASET_H_
#define DATASET_H_

#include <vector>
#include <string>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include "Species.hpp"

using namespace std;

class DataSet {
    ///////////////////////////////////////////////////////////////////
    // BOOST SERIALIZATION STUFF
    friend class boost::serialization::access;
    ///////////////////////////////////////////////////////////////////

private:
    vector<Species*> species;

    ///////////////////////////////////////////////////////////////////
    // BOOST SERIALIZATION STUFF
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
    	ar & species;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
    	ar & species;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    //////////////////////////////////////////////////////////////////

public:
    DataSet();
    ~DataSet();
    void     addSpecies(string filename);
    void     addSpecies(Species *species);
    Species* getSpecies(int index);
    int      getNumberOfSpecies();    
    void     merge(DataSet ds);
};

///////////////////////////////////////////////////////
// BOOST
BOOST_CLASS_VERSION(DataSet, 0)
///////////////////////////////////////////////////////

#endif /* DATASET_H_ */
