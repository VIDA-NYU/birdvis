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


#include "Util.hpp"

#include <ctime>

util::ClockTimer::ClockTimer() {}

void 
util::ClockTimer::start() 
{ 
    t0 = (double) clock(); 
}

double 
util::ClockTimer::elapsed()
{ 
    double t1 = clock();
    return (t1 - t0) / CLOCKS_PER_SEC; 
}


namespace util {

void 
computeMaxAndCenteredRectanglePreservingAspectRatio
(
    float  w_source, float  h_source,
    float  x_target, float  y_target,
    float  w_target, float  h_target,
    float& x_result, float& y_result, 
    float& w_result, float& h_result
)
{
    // take care of aspect ratio
    float ar_source = w_source/h_source;
    float ar_target = w_target/h_target;

    // cout << "w_source  " << w_source  << endl;
    // cout << "h_source  " << h_source  << endl;
    // cout << "ar_source " << ar_source << endl;
    // cout << "ar_target  " << ar_target  << endl;

    if (ar_source <= ar_target)
    {
	float scale =  h_target / h_source;
	w_result = w_source * scale;
	h_result = h_target;
	y_result = y_target;
	x_result = x_target + (w_target - w_result)/2.0;
    }
    else 
    {
	float scale =  w_target / w_source;
	h_result = h_source * scale;
	w_result = w_target;
	x_result = x_target;
	y_result = y_target + (h_target - h_result)/2.0;
    }

    // cout << "x_result " << x_result << endl;
    // cout << "y_result " << y_result << endl;
    // cout << "w_result " << w_result << endl;
    // cout << "h_result " << h_result << endl;

}

};

 // {
 //   "name": "utah",
 //   "Status": {
 //     "code": 200,
 //     "request": "geocode"
 //   },
 //   "Placemark": [ {
 //     "id": "p1",
 //     "address": "Utah, USA",
 //     "AddressDetails": {
 //    "Accuracy" : 2,
 //    "Country" : {
 //       "AdministrativeArea" : {
 // 	  "AdministrativeAreaName" : "UT"
 //       },
 //       "CountryName" : "USA",
 //       "CountryNameCode" : "US"
 //    }
 // },
 //     "ExtendedData": {
 //       "LatLonBox": {
 // 	 "north": 41.4956835,
 // 	 "south": 37.0765055,
 // 	 "east": -106.9958306,
 // 	 "west": -115.1916316
 //       }
 //     },
 //     "Point": {
 //       "coordinates": [ -111.0937311, 39.3209801, 0 ]
 //     }
 //   } ]
 // }

// #include <stdio.h>
// #include <string>
// #include <iostream>
// #include<boost/tokenizer.hpp>

// using namespace std;
// using namespace boost;

// void get_center_and_bounding_box(string keywords, float *xywh)
// {
//     // tokenizer
//     typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
//     boost::char_separator<char> sep("=[];,:\" \n");

//     char buffer[2048];
//     FILE *fp = popen("wget http://maps.google.com/maps/geo?q=new+york -O - "
// 	       "| egrep \"coordinates|north|south|east|west\"", 
//     	       "r");

//     vector<string> tokens;


//     float north, south, east, west, lon, lat;

//     while (fgets(buffer, 2048, fp) != NULL)
//     {
//     	string s(buffer);

// 	// clear
// 	tokens.clear();

// 	// tokens
// 	tokenizer tk(s, sep);
// 	for (tokenizer::iterator tok_iter = tk.begin();tok_iter != tk.end(); ++tok_iter)
// 	{
// 	    std::cout << "<" << *tok_iter << "> ";
// 	    tokens.push_back(*tok_iter);
// 	}
// 	std::cout << "\n";

// 	if (tokens.size() > 0)
// 	{
// 	    string key = tokens.at(0);
// 	    if (key == "north")
// 		sscanf(tokens.at(1).c_str(),"%f",&north);
// 	    else if (key == "south")
// 		sscanf(tokens.at(1).c_str(),"%f",&south);
// 	    else if (key == "east")
// 		sscanf(tokens.at(1).c_str(),"%f",&east);
// 	    else if (key == "west")
// 		sscanf(tokens.at(1).c_str(),"%f",&west);
// 	    else if (key == "coordinates")
// 	    {
// 		sscanf(tokens.at(1).c_str(),"%f",&lon);
// 		sscanf(tokens.at(2).c_str(),"%f",&lat);
// 	    }
// 	}
//     	// printf("%s", buffer);
//     }
//     pclose(fp);

//     //
//     printf("coordinates: %f, %f\n",lon,lat);
//     printf("north: %f\n",north);
//     printf("south: %f\n",south);
//     printf("east: %f\n",east);
//     printf("west: %f\n",west);

//     xywh[0] = west;
//     xywh[1] = south;
//     xywh[2] = east-west;
//     xywh[3] = north-south;
// }

// int main()
// {
//     get_center_and_bounding_box("new+york");
// }






    // char query_template[] = "wget http://maps.google.com/maps/geo?q=%s -O /tmp/_geo";
    // char query_instance[2048];
    // sprintf(query_instance,query_template,keywords.c_str());
    // cout << query_instance << endl;
    // system(query_instance);
    
    // // get lon,lat
    // char grep_cmd[] = "grep coordinates /tmp/_geo > /tmp/_coords";
    // system(grep_cmd);

    // //
    // char cat_cmd[] = "cat /tmp/_coords";
    // system(cat_cmd);
    

    // FILE *fp;

    // float north = 0.0;
    // float south = 0.0;
    // float east  = 0.0;
    // float west  = 0.0;

    // std::string str = ";;Hello|world||-foo--bar;yow;baz|";

    // return EXIT_SUCCESS;
