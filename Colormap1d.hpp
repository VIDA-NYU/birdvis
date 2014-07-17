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

#ifndef COLORMAP1D_HPP
#define COLORMAP1D_HPP

#include <vector>

#include "Color.hpp"

using namespace std;

/**************
 * Colormap1d *
 **************/

class Colormap1d
{
public: // static colormaps 
    static Colormap1d *hot();
    static Colormap1d *copper();
    static Colormap1d *divergentBlueRed();
    static Colormap1d *divergentBlueRed3Colors();
    static Colormap1d *divergentBlueRed5Colors();
    static Colormap1d *divergentBlueWhiteRed9();
    static Colormap1d *divergentBlueRed257Colors();
    static Colormap1d *yellow2blue();
    static Colormap1d *greyScale();
    static Colormap1d *heatedObject();
    static Colormap1d *linearGreyScale();
    static Colormap1d *seqWhiteRed9();
    static Colormap1d *seqWhiteRedCont();

public:
    class Stop {
    public:
	enum Kind { CONTINUOUS, DISCRETE };
	Colormap1d *parent;
	Color       color;
	double      value;//coefficient;
	Kind        kind;
	bool        active;
    
	Stop(Colormap1d *parent, Color color, 
	     double value, Kind kind);

	Stop(double value);

	double getValue();

	void copyTo(Stop *s);

    };

public:
    vector<Stop*> stops;
//     double        min_value;
//     double        max_value;
    Color         out_of_range_color;
	
    Colormap1d();
    Colormap1d(int n);

    ~Colormap1d();

    Color getColor(double value);

    void setRange(double min_value, double max_value);

    double getMaxValue();
    double getMinValue();

    int size();

    Stop *operator[](int index);

    Colormap1d *copy();

    void setStops(vector<double> new_stops);

}; // end class Colormpa1d

#endif
