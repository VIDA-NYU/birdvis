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

#ifndef LABELING_H_
#define LABELING_H_

#include <iostream>
#include <vector>

using namespace std;

class Labeling{
private:

    int length(double *a);
    double modulo(double m, double d);

public:
    Labeling(){
    };

    double simplicity(double q, double *Q, int sizeQ, double j, 
		      double lmin, double lmax, double lstep, int indexqQ);    
    double simplicityMax(double q, double *Q, int sizeQ, double j, int indexqQ);
    double density(double k, double m, double dmin, 
		   double dmax, double lmin, double lmax);
    double densityMax(double k, double m);
    double coverage(double dmin, double dmax, double lmin, double lmax);

    double coverageMax(double dmin, double dmax, double span);

    double legibility(double lmin, double lmax, double lstep);

    double legibilityMax(double lmin, double lmax, double lstep);

    vector<double>* labelExtended(double dmin, double dmax, double m, 
			  double *Q, int sizeQ, bool onlyInside, 
			  double *w);

};

#endif
