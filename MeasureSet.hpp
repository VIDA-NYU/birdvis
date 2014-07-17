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

#ifndef MEASURESET_H_
#define MEASURESET_H_

#include <string>
#include <fstream>
#include <iostream>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

// #include <vector>
using namespace std;

class MeasureSetCenter;

class MeasureSet {
    ///////////////////////////////////////////////////////////////////
    // BOOST SERIALIZATION STUFF
    friend class boost::serialization::access;
    ///////////////////////////////////////////////////////////////////
private:

public:
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    int    x_samples;
    int    y_samples;
    int    day;
    int    year;
    double *values;

    ///////////////////////////////////////////////////////////////////
    // BOOST SERIALIZATION STUFF
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
    	ar & min_x;
    	ar & max_x;
    	ar & min_y;
    	ar & max_y;
	ar & x_samples;
	ar & y_samples;
    	ar & day;
    	ar & year;
	int n = x_samples * y_samples;
	for (int i=0;i<n;i++)
	    ar & values[i];
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
    	ar & min_x;
    	ar & max_x;
    	ar & min_y;
    	ar & max_y;
	ar & x_samples;
	ar & y_samples;
    	ar & day;
    	ar & year;
	int n = x_samples * y_samples;
	values = new double[n];
	for (int i=0;i<n;i++)
	    ar & values[i];
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    //////////////////////////////////////////////////////////////////

    MeasureSet(int day, int year,
	       double min_x, double max_x,
	       double min_y, double max_y,
	       int x_samples, int y_samples);
    MeasureSet() {};
    virtual ~MeasureSet();

    void set_value_at(int row, int column, double value);
    void add_value_at(int row, int column, double value);
    void set_min_x(double value);
    void set_max_x(double value);
    void set_min_y(double value);
    void set_max_y(double value);
    void set_x_samples(int samples);
    void set_y_samples(int samples);

    int get_x_samples();
    int get_y_samples();

    double get_value_at(int row, int column);
    //double get_value_at(int row, int column);

    void   get_value_and_coord_at(int row, int column, double &value, double &x, double &y);
    void   get_value_range(double &min_value, double &max_value);

    double get_grid_value_at(double lon, double lat, int &row, int &col, bool &in_grid);



    double get_min_x();
    double get_max_x();
    double get_min_y();
    double get_max_y();

    double retrieve_average_rectangle(double lon_min, double lat_min,
                                      double lon_max, double lat_max);

    void to_string(bool print_values);

    // static vector<MeasureSet*> read_csv_measure_sets(string filename);

    MeasureSetCenter getCenter();

};

///////////////////////////////////////////////////////
// BOOST
BOOST_CLASS_VERSION(MeasureSet, 0)
///////////////////////////////////////////////////////

class MeasureSetCenter {
public:
    MeasureSetCenter() {};
    MeasureSet *measure_set;
    double x;
    double y;
    double weight;
};


#endif /* MEASURESET_H_ */
