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

#ifndef DATAFLOW
#define DATAFLOW

#include<map>
#include<vector>
#include<string>

using namespace std;

namespace dataflow {

    class Data;
    class Port;
    class Module;
    class Connection;

    class Data {
    public:
	virtual ~Data() {}
    };
    
    class Port {
    public:
	enum TYPE { INPUT, OUTPUT };
	Module *module;
	TYPE    type;
	string  name;
	Data   *data;
	vector<Connection*> connections;
	
	Port(Module *module, string name, TYPE type);
	~Port();
	
	Connection*     addConnection(Connection *c);
	void            deleteConnection(Connection *c);
	void            deleteData();
	void            setData(Data *d, bool outdateDownstream=false);
	Data*           getData();
	vector<Data* >  getAllData();
	void            outdateDownstream();
    };
    
    class Module {
    public:
	string name;
	vector<Port*> ports[2];
	map<string,Port*> ports_name_map[2];
	bool   outdated;

	Module();	
	Module(string name);
	~Module();
	Port *addPort(string name, Port::TYPE type);
	Port *addInputPort(string name);
	Port *addOutputPort(string name);
	Port *getPort(string name, Port::TYPE type);
	
	Port *getInputPort(string name);
	Port *getOutputPort(string name);

	void  deleteAllConnectionsFrom(Module *module);
	void  deleteAllConnectionsFromOrTo(Module *module);

	void  setName(string name);
	void  outdateDownstream();
	string dataflowDescription();
	
	void run();
	virtual void process(); // this should be updated
    };
    
    class Connection {
    public:
	Connection(Port *source, Port *target);
	~Connection();
	Port *oppositePort(Port *p);
	Module* oppositeModule(Port *p);
	Port *source;
	Port *target;
    };
    
} // end namespace: dataflow

#endif


