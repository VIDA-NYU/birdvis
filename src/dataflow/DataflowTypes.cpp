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

#include "DataflowTypes.hpp"

using namespace dataflow::data;

// Bool

Bool::Bool(bool value) {
    this->value = value;
}

bool 
Bool::getInputValue(Module *m, string port_name, bool &result)
{
    Bool *data = (dynamic_cast<Bool *>  
		  (m->getInputPort(port_name)->getData()));
    if (data != NULL) {
	result = data->value;
	return true;
    }
    else return false;
}

// Integer

Integer::Integer(int v) {
    this->value = v;
}

// Double
Double::Double(double v){
    this->value = v;
}

// Array

Array::Array(datatypes::Array *value, bool delete_array_on_delete)
{
    this->value = value;
    this->delete_array_on_delete = delete_array_on_delete;
}

Array::~Array()
{
    if (delete_array_on_delete && value != NULL)
    {
	cout << "dataflow::Array deleting... " << endl;
	delete value;
    }
}

// Font

FontData::FontData(Font *value) {
    this->value = value;
}

// Colormap1d

// Colormap1d::Colormap1d(datatypes::Colormap1d *value) {
//     this->value = value;
// }


//SingleGridPtr

SingleGridPtr::SingleGridPtr(datatypes::Grid *v)
{
    this->value = v;
}

SingleGridPtr::~SingleGridPtr()
{
    delete this->value;
}

bool 
SingleGridPtr::getInputValue(Module *m, string port_name, datatypes::Grid** result)
{
    SingleGridPtr *data = (dynamic_cast< data::SingleGridPtr* >  
			   (m->getInputPort(port_name)->getData()));
    if (data != NULL) {
	(*result) = data->value;
	return true;
    }
    else return false;
}

