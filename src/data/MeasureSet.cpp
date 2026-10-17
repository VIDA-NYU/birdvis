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

/*
 * MeasureSet.cpp
 *
 *  Created on: Aug 30, 2010
 *      Author: nivan
 */

#include "MeasureSet.hpp"

MeasureSet::MeasureSet(int day, int year, double min_x, double max_x,
		       double min_y, double max_y,
		       int x_samples, int y_samples)
{
    this->day = day;
    this->year = year;
    this->min_x = min_x;
    this->max_x = max_x;
    this->min_y = min_y;
    this->max_y = max_y;
    this->x_samples = x_samples;
    this->y_samples = y_samples;
    this->values = new double[x_samples * y_samples];

    for(int i = 0 ; i < x_samples * y_samples  ; i++)
	this->values[i] = 0.0;
}

MeasureSet::~MeasureSet() {
    delete [] this->values;
}

//the (0,0) position is the lower left corner
void MeasureSet::set_value_at(int row, int column, double value){
    this->values[row*(this->get_x_samples()) + column] = value;
}

void MeasureSet::add_value_at(int row, int column, double value){

    //cout << "Value Before = " << this->values[row*(this->get_x_samples()) + column] << endl;

    this->values[row*(this->get_x_samples()) + column] = 
	this->values[row*(this->get_x_samples()) + column]+value;
    //cout << "Value After = " << this->values[row*(this->get_x_samples()) + column] << endl;
}


MeasureSetCenter MeasureSet::getCenter() {

    double sum = 0.0;
    double weighted_sum_x = 0.0;
    double weighted_sum_y = 0.0;

    double x0 = this->min_x;
    double x1 = this->max_x;
    double y0 = this->min_y;
    double y1 = this->max_y;

    // double dx = (x1-x0) / (this->x_samples - 1);
    // double dy = (y1-y0) / (this->y_samples - 1);

    double area_unit = 1.0 / (x_samples * y_samples);

    for (int i=0;i<this->y_samples;i++) {
	for (int j=0;j<this->x_samples;j++) {
	    double value = this->values[i*x_samples + j];
	    weighted_sum_x += j * value * area_unit;
	    weighted_sum_y += i * value * area_unit;
	    sum += value;
	}
    }

    if (sum == 0)
	sum = 1.0;

    weighted_sum_x = weighted_sum_x / (sum * area_unit);
    weighted_sum_y = weighted_sum_y / (sum * area_unit);


    // cout << "ws " << weighted_sum_x << "  " << weighted_sum_y << endl;

    MeasureSetCenter result;
    result.x = x0 + (weighted_sum_x/x_samples) * (x1-x0);
    result.y = y0 + (weighted_sum_y/y_samples) * (y1-y0);
    result.measure_set = this;
    result.weight = sum;


    // cout << "msc " << day << "  " << result.x << " " << result.y << " " << result.weight << endl;

    return result;
}

void MeasureSet::set_min_x(double value){
    this->min_x = value;
}

void MeasureSet::set_max_x(double value){
    this->max_x = value;
}

void MeasureSet::set_min_y(double value){
    this->min_y = value;
}

void MeasureSet::set_max_y(double value){
    this->max_y = value;
}

void MeasureSet::set_x_samples(int samples){
    this->x_samples = samples;
}

void MeasureSet::set_y_samples(int samples){
    this->y_samples = samples;
}

int MeasureSet::get_x_samples(){
    return this->x_samples;
}

int MeasureSet::get_y_samples(){
    return this->y_samples;
}

double MeasureSet::get_value_at(int row, int column){
    return this->values[row*(this->get_x_samples()) + column];
}

double MeasureSet::get_min_x(){
    return this->min_x;
}

double MeasureSet::get_max_x(){
    return this->max_x;
}

double MeasureSet::get_min_y(){
    return this->min_y;
}

double MeasureSet::get_max_y(){
    return this->max_y;
}

void MeasureSet::get_value_range(double &min_value, double &max_value){
    double a, b;
    a = b = this->values[0];
    int n = this->x_samples * this->y_samples;
    for(int i = 0 ; i <  n; i++){
	double v = this->values[i];
	if (v < a) a = v;
	if (v > b) b = v;
    }
    min_value = a;
    max_value = b;
}

void   MeasureSet::get_value_and_coord_at(int row, int column, double &value, double &x, double &y)
{
    value = this->values[row*(this->get_x_samples()) + column];
    x = this->min_x + column * (this->max_x - this->min_x) / (this->x_samples - 1);
    y = this->min_y + row *    (this->max_y - this->min_y) / (this->y_samples - 1);
}

double MeasureSet::get_grid_value_at(double lon, double lat, int &row, int &col, bool &in_grid)
{
    double cw = (max_x - min_x)/(x_samples-1);
    double ch = (max_y - min_y)/(y_samples-1);
    
    double grid_x = min_x - cw/2.0;
    double grid_w = max_x - min_x + cw;
    double grid_y = min_y - ch/2.0;
    double grid_h = max_y - min_y + ch;

    col = (int) ((lon - grid_x)/grid_w * x_samples);
    row = (int) ((lat - grid_y)/grid_h * y_samples);

    if (col >= 0 && col < x_samples && row >= 0 && row < y_samples)
    {
	in_grid = true;
	return this->values[row*(this->get_x_samples()) + col];
    }
    in_grid = false;
    return 0;
}

void MeasureSet::to_string(bool print_values){
    cout << "---Measure Set " << this->day 
	 << "," << this->year << " Summary" << endl;
    cout << "Num x samples = " << x_samples << " , ";
    cout << "Num y samples = " << y_samples << endl;

    cout << "Min x = " << min_x << " , ";
    cout << "Max x = " << max_x << endl;

    cout << "Min y = " << min_y << " , ";
    cout << "Max y = " << max_y << endl;

    if(print_values){
	for(int j = 0 ; j < this->y_samples ; j++){
		for(int i = 0 ; i < this->x_samples ; i++){
		cout << "Value " << i << "," << j << " " << 
		    this->get_value_at(j,i) << endl;
	    }
	}
    }
    
    cout << "--- End Measure Set " << this->day 
	 << "," << this->year << " Summary" << endl;
    
}

double MeasureSet::retrieve_average_rectangle(double lon_min, 
					      double lat_min,
					      double lon_max, 
					      double lat_max){
    return 0.0;
}
