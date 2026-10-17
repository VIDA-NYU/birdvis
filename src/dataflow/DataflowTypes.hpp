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

#ifndef DATAFLOW_TYPES_H
#define DATAFLOW_TYPES_H

#include <vector>

#include "Dataflow.hpp"

#include "Array.hpp"

#include "datatypes.hpp"

#include "Vec2.hpp"

#include "font.h"

#include "ViewRect.hpp"

#include "Colormap1d.hpp"

//#include "Curve.hpp"

namespace dataflow {

    namespace data {
	
	class Bool: public Data
	{
	public:
	    bool value;
	    Bool(bool b);
	    static bool getInputValue(Module *m, string port_name, bool &result);
	};	

	class Integer: public Data
	{
	public:
	    int value;
	    Integer(int v);
	};

	class Double: public Data
	{
	public:
	    double value;
	    Double(double v);
	};


	class Array: public Data
	{
	public:
	    bool delete_array_on_delete;
	    datatypes::Array *value;
	    Array(datatypes::Array *value, bool delete_array_on_delete=false);
	    ~Array();
	};

	class FontData: public Data
	{
	public:
	    Font *value;
	    FontData(Font *value);
	};

	template <class T>
	class Vector: public Data
	{
	public:
	    vector<T> *value;
	    Vector(vector<T> *value);
	    Vector(T a);
	    Vector(T a, T b);
	    Vector(T a, T b, T c);
	};

	// class Colormap1d: public Data
	// {
	// public:
	//     Colormap1d *value;
	//     Colormap1d(Colormap1d *value);
	// };

	template <class T>
	class Single: public Data
	{
	public:
	    // bool destroy_value_on_destroy;
	    T value;
	    Single(T value); //, bool destroy_value_on_destroy=false);
	    // ~Single();
	    static bool getInputValue(Module *m, string port_name, T &result);
	};

// Single

	template <class T> Single<T>::Single(T v)
	{
	    this->value = v;
	}

	// template <class T> Single<T>::~Single()
	// {
	//     if (destroy_value_on_destroy)
	// 	delete this->value;
	// }

	template <class T> 
	bool 
	Single<T>::getInputValue(Module *m, string port_name, T &result)
	{
	    Single<T> *data = (dynamic_cast< data::Single<T>* >  
			       (m->getInputPort(port_name)->getData()));
	    if (data != NULL) {
		result = data->value;
		return true;
	    }
	    else return false;
	}


	typedef data::Single<Colormap1d *> SingleColormap1dPtr;
	//typedef data::Single<datatypes::Grid *>       SingleGridPtr;
	typedef data::Single<int>                     SingleInt;
	typedef data::Single<string>                  SingleString;
	typedef data::Single<ViewRect>                SingleViewRect;
	typedef data::Single<Vec2>                    SingleVec2;


	// template <class T> 
	// vector<T>
	// Single<T>::getAllInputValues(Module *m, string port_name)
	// {
	//     vector<T> result;
	//     vector<Data*> data_vec = m->getInputPort(port_name)->getAllData();
	//     // for (int i=0;i<data_vec.size();i++)
	//     // {
	//     // 	Single<T> *data = dynamic_cast< data::Single<T>* >(data_vec.at(i));  
	//     // 	if (data != NULL)
	//     // 	    result.push_back(data->value);
	//     // }
	//     return result;
	// }

// class SingleGridPtr

	class SingleGridPtr: public Data
	{
	public:
	    // bool destroy_value_on_destroy;
	    datatypes::Grid *value;
	    SingleGridPtr(datatypes::Grid * value); //, bool destroy_value_on_destroy=false);
	    ~SingleGridPtr();
	    static bool getInputValue(Module *m, string port_name, datatypes::Grid** result);
	};


// Vector

	template <class T> Vector<T>::Vector(vector<T> *v)
	{
	    this->value = v;
	}

	template <class T> Vector<T>::Vector(T a)
	{
	    this->value = new vector<T>();
	    this->value->push_back(a);
	}

	template <class T> Vector<T>::Vector(T a, T b)
	{
	    this->value = new vector<T>;
	    this->value->push_back(a);
	    this->value->push_back(b);
	}

	template <class T> Vector<T>::Vector(T a, T b, T c)
	{
	    this->value = new vector<T>();
	    this->value->push_back(a);
	    this->value->push_back(b);
	    this->value->push_back(c);
	}

    } // end namespace data

} // end namespace dataflow

#endif
