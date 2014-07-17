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

#include <math.h>
#include "Labeling.hpp"

#define xDEBUG_LABELING

double min(double a , double b){
    if(a <= b)
	return a;
    else 
	return b;
}

double max(double a , double b){
    if(a >= b)
	return a;
    else 
	return b;
}

int Labeling::length(double *a){
    return ( sizeof ( a ) / sizeof ( a[0] ) );
};

double Labeling::modulo(double m, double d){
    
    double result = m;

    while(result > d)
	result -= d;

    return result;
};

double Labeling::simplicity(double q, double *Q, int sizeQ, 
			    double j, double lmin, double lmax, 
			    double lstep, int indexqQ){
    double eps = 1e-10;
    int n = sizeQ;
    int i = indexqQ;//Q.index(q)+1;//find q in Q
    int v = 1; 
    
    bool cond1 = (modulo(lmin, lstep) < eps);
    bool cond2 = (modulo(lstep - lmin,  lstep) < eps);

    if ((cond1 || cond2 ) && lmin <= 0 && lmax >= 0) 
	return (n-i)/(n-1.0) + v - j;
    else 
	return 0;
};
    
double Labeling::simplicityMax(double q, double *Q, int sizeQ,
			       double j, int indexqQ){
    int n = sizeQ;
    int i = indexqQ;//Q.index(q)+1;//find q in Q
    int v = 1;
    return (n-i)/(n-1.0) + v - j;
};

double Labeling::density(double k, double m, double dmin, 
			 double dmax, double lmin, double lmax){
    double r = (k-1.0) / (lmax-lmin);
    double rt = (m-1.0) / (max(lmax, dmax) - min(lmin, dmin));
    return 2 - max( r/rt, rt/r );
};

double Labeling::densityMax(double k, double m){
    if (k >= m)
	return 2 - (k-1.0)/(m-1.0);
    else
	return 1;
};

double Labeling::coverage(double dmin, double dmax, 
			  double lmin, double lmax){
    double range = dmax-dmin;
    return (1 - 0.5 * (pow(dmax-lmax,2)+ pow(dmin-lmin, 2)) / pow(0.1 * range, 2));
};

double Labeling::coverageMax(double dmin, double dmax, 
			     double span){
    double range = dmax-dmin;
	if (span > range){
	    double half = (span-range)/2.0;
	    return 1 - pow(half, 2) / pow(0.1*range, 2);
	}
	else{
	    return 1;
	}
};

double Labeling::legibility(double lmin, double lmax, 
			    double lstep){
    return 1;
};

double Labeling::legibilityMax(double lmin, double lmax, 
			       double lstep){
    return 1;
};

//Q = {1,5,2,2.5,4,3}
//w = [0.25,0.2,0.5,0.05]
vector<double>* Labeling::labelExtended(double dmin, double dmax, 
					double m, double *Q, int sizeQ,
					bool onlyInside, double *w){
#ifdef DEBUG_LABELING
    cout << "Labeling::labelExtended(...)" << endl;
#endif

    double result[] = {0,0,0,0,0};
    double bestScore = -2;	
    int sizeOfQ = sizeQ;
    //cout << "Size of Q " << sizeOfQ << endl;


    double j = 1;//
    bool stop1 = false;

    while(!stop1){//goes up to infinity
	for(int qIndex = 0 ; qIndex < sizeOfQ ; qIndex++){

	    double sm = simplicityMax(Q[qIndex], Q, sizeQ, j, qIndex);

	    if(w[0] * sm + w[1] + w[2] + w[3] < bestScore){
		stop1 = true;
		break;
	    }
		
	    double k = 2; //
	    bool stop2 = false;
	    while(!stop2){
		double dm = densityMax(k,m);		    
		    
		if(w[0] * sm + w[1] + w[2] * dm + w[3] < bestScore)
		    break;
		    

		double delta = (dmax-dmin)/(k+1.0)/j/(Q[qIndex]);//(dmax - dmin)/((k + 1)/(j * Q[qIndex]));//different in the implementation and the paper

		int z = ceil(log10(delta));
		bool stop3 = false;
		while(!stop3){

		    double step = Q[qIndex] * j * pow(10,z);
		    double cm = coverageMax(dmin,dmax,step*(k-1.0));

		    if(w[0] * sm + w[1] * cm + w[2] * dm + w[3] < 
		       bestScore)
			break;
			
		    double min_start = floor(dmax/step)*j - (k-1.0)*j;
		    double max_start = ceil(dmin/step)*j;

		    if (min_start > max_start){
			z = z+1;
			break;
		    }

		    for(double start = int(min_start) ;
			start <= max_start ; 
			start = start + 1.0/j){
			double lmin = start * (step/j);//start * step;//different on the paper and implementation
			double lmax = lmin + (k-1.0)*step;
			double lstep = step;

			double s = simplicity(Q[qIndex], Q,sizeQ, j, lmin ,
					      lmax , lstep, qIndex);
			double c = coverage(dmin,dmax,lmin,lmax);
			double d = density(k,m,dmin,dmax,lmin,lmax);
			double l = legibility(lmin, lmax, lstep);

			double score = 
			    w[0] * s + w[1] * c + w[2] * d + w[3] * l;

			if(score > bestScore && 
			   ((!onlyInside) || (lmin >= dmin && lmax <= dmax))){
			    bestScore = score;
			    result[0] = lmin;
			    result[1] = lmax;
			    result[2] = lstep;
			    result[3] = Q[qIndex];
			    result[4] =  k;
			}
			
			//cout << "Start " << start << endl;

		    }

		    z++;
		    //cout << "Z " << z << endl;
		}

		k++;
		//cout << "K " << k << endl;
	    }
	
	}
	j++;   
	//cout << "J " << j << endl;
    }

#ifdef DEBUG_LABELING
    cout << "   finished computation" << endl;
#endif
    
    double lower = result[0];
    double upper = result[1];
    double stepSize = result[2];
    double curr = lower;
    int count = 0;
    vector<double> *ticks = new vector<double>();



    for (int i=0;i<result[4];i++)
    {
    	ticks->push_back(curr);
	curr += stepSize;
    }

    return ticks;
};


