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

#include<queue>
#include<set>
#include<map>
#include<iostream>
#include<algorithm>
#include<exception>
#include<sstream>


#include "Dataflow.hpp"

#define xDEBUG_MODULE
#define xDEBUG_PORT
#define xDEBUG_CONNECTION


using namespace dataflow;

/********
 * Port *
 ********/

Port::Port(Module *module, string name, TYPE type)
{
    this->module = module;
    this->name   = name;
    this->type   = type;
    this->data   = NULL;
}

Port::~Port()
{
    // cout << "   destroying port " << name << endl;
}

Connection *Port::addConnection(Connection *c)
{
    // cout << "adding connection " << c->source->module->name << "." << c->source->name << " -> " 
    // 	 << c->target->module->name << "." << c->target->name << " on " 
    // 	 << (this->type == INPUT ? "input" : "output") << " port" << endl;
    this->connections.push_back(c);
    return c;
}

void Port::deleteConnection(Connection *c)
{
    vector<Connection*>::iterator it;
    it = find(connections.begin(), connections.end(), c);
    if (it != connections.end())
	connections.erase(it);
}

void Port::deleteData()
{
    // cout << "Port.deleteData()... current data " << long(data) << endl;
    if (data != NULL)
	delete data;
    data = NULL;
}

void Port::setData(Data *d, bool outdateDownstream)
{
    this->deleteData();
    data = d;
    if (outdateDownstream)
	this->outdateDownstream();
}

Data *Port::getData()
{
    if (type == Port::OUTPUT)
	return this->data;
    else {
	if (this->data == NULL && this->connections.size() > 0)
	{
	    if (this->connections.at(0)->source->module->outdated)
		return this->connections.at(0)->source->getData();
	    else if (this->data != NULL)
		return this->data;
	    else
		return this->connections.at(0)->source->getData();
	}
	else
	    return this->data;
    }
}

vector<Data* > 
Port::getAllData()
{
    vector<Data* > result;
    if (type == Port::OUTPUT)
    {
	result.push_back(this->data);
    }
    else {
	if (this->data == NULL)
	{
	    for (int i=0;i<this->connections.size();i++)
	    {
		Data *data = this->connections.at(i)->source->getData();
		result.push_back(data);
	    }
	}
	else {
	    result.push_back(this->data);
	}
    }
    return result;
}


void Port::outdateDownstream()
{
    if (type == Port::INPUT)
    {
	// cout << "Outdate Input Port: " << name << " on module " << this->module->name << endl;
	this->module->outdateDownstream();
    }
    else {
	//cout << "Outdate Output Port: " << name << " on module " << this->module->name << endl;
	vector<Connection*>::iterator it;
	for (it = this->connections.begin(); it < this->connections.end(); it++){	  
	    if(*it == NULL){
		cout << "Connection Null" << endl;
		exit(1);
	    }
	    (*it)->target->outdateDownstream();
	}
    }
}

/**********
 * Module *
 **********/

Module::Module()
{
    this->name = "";
    this->outdated = true;
}

Module::Module(string name)
{
    this->name = name;
    this->outdated = true;
}

Module::~Module()
{
    // cout << "destroying module " << name << endl;
    vector<Port*>::iterator it;
    for (int t=Port::INPUT; t <= Port::OUTPUT; t++)
	for (it=ports[t].begin(); it < ports[t].end(); it++)
	    delete *it;
}

Port *Module::getPort(string name, Port::TYPE type)
{
    if (ports_name_map[type].count(name) == 0)
	return NULL;
    else 
	return ports_name_map[type][name];
}

Port *Module::getInputPort(string name)
{
    return this->getPort(name, Port::INPUT);
}

Port *Module::getOutputPort(string name)
{
    return this->getPort(name, Port::OUTPUT);
}


void
Module::deleteAllConnectionsFrom(Module *mod)
{
    vector<Connection*> connections_to_delete;
    vector<Port*>::iterator itp;
    vector<Connection*>::iterator itc;
    
    for (itp=ports[Port::INPUT].begin(); itp < ports[Port::INPUT].end(); itp++)
    {
	Port* p = *itp;
	for (itc=p->connections.begin(); itc < p->connections.end(); itc++)
	{
	    Connection *c = *itc;
	    if (c->source->module == mod)
	    {		
		connections_to_delete.push_back(c);		
		//p->connections.erase(itc);
	    }
	}
    }

    for (int i=0;i<connections_to_delete.size();i++)
    {
#ifdef DEBUG_MODULE
	cout << "   delete one connection from " << mod->name << " to " << this->name << endl;
#endif
	delete connections_to_delete[i];
    }
}


void
Module::deleteAllConnectionsFromOrTo(Module *mod)
{
    vector<Connection*> connections_to_delete;
    vector<Port*>::iterator itp;
    vector<Connection*>::iterator itc;
    for (int t=Port::INPUT; t <= Port::OUTPUT; t++)
	for (itp=ports[t].begin(); itp < ports[t].end(); itp++)
	{
	    Port* p = *itp;
	    for (itc=p->connections.begin(); itc < p->connections.end(); itc++)
	    {
		Connection *c = *itc;
		if (c->oppositeModule(p) == mod)
		{
		    connections_to_delete.push_back(c);
		}
	    }
	}

    for (int i=0;i<connections_to_delete.size();i++)
    {
#ifdef DEBUG_MODULE
	cout << "   delete one connection from " << mod->name << " to " << this->name << endl;
#endif
	delete connections_to_delete[i];
    }
}





Port *Module::addPort(string name, Port::TYPE type)
{
    Port *p = getPort(name, type);
    if (p != NULL)
	return p;
    p = new Port(this, name, type);
    this->ports[type].push_back(p);
    this->ports_name_map[type][name] = p;
    return p;
}

Port *Module::addInputPort(string name)
{
    return this->addPort(name, Port::INPUT);
}

Port *Module::addOutputPort(string name)
{
    return this->addPort(name, Port::OUTPUT);
}

void Module::setName(string name){
    this->name = name;
}

void Module::outdateDownstream()
{
    // if (this->name.compare("ArrayBuilder") == 0)
    // cout << "Module ArrayBuilder was  Outdated" << endl;
    
    // cout << "Module Outdated: " << name << endl;
    this->outdated = true;
    vector<Port*>::iterator it;
    for (it = this->ports[Port::OUTPUT].begin(); it < this->ports[Port::OUTPUT].end(); it++)
	    (*it)->outdateDownstream();
}

void Module::run()
{
    if (!this->outdated)
	return;

    vector<Port*>::iterator itp;
    for (itp=ports[Port::INPUT].begin();itp<ports[Port::INPUT].end();itp++)
    {
	Port *p = *itp;
	vector<Connection*>::iterator itc;
	for (itc=p->connections.begin();itc < p->connections.end();itc++)
	{
	    Connection *c = *itc;
	    c->source->module->run();
	}
    }

    //
    // cout << "(Module "<< (void*)(this) << ").process()   // " << name << endl;
    this->process();

    this->outdated = false;

}

void Module::process()
{
}

string Module::dataflowDescription()
{
    int free_module_id = 0;
    int free_connection_id = 0;
    int free_port_id = 0;

    map<Module*     , int> map_modules;
    map<Module*     , int>::iterator it_map_modules;
    map<Connection* , int> map_connections;
    map<Connection* , int>::iterator it_map_connections;
    map<Port*       , int> map_ports;
    map<Port*       , int>::iterator it_map_ports;

    set<Module* > queued;
    queued.insert(this);

    queue<Module* > Q;
    Q.push(this);

    // bfs
    while (Q.size() > 0)
    {
	Module *m = Q.front();
	Q.pop();
	map_modules[m] = free_module_id++;
	vector<Port*>::iterator it;
	for (int t=Port::INPUT; t <= Port::OUTPUT; t++)
	{
	    for (it=m->ports[t].begin(); it != m->ports[t].end(); it++)
	    {
		Port *p = *it;
		map_ports[p] = free_port_id++;
		vector<Connection*>::iterator it;
		for (it = p->connections.begin(); it < p->connections.end(); it++)
		{
		    Connection *c = *it;
		    if (map_connections.count(c) == 0)
		    {
			map_connections[c] = free_connection_id++;
			Port *pp = c->oppositePort(p);
			if (queued.count(pp->module) == 0)
			{
			    // cout << "enqueue " << pp->module->name << endl;
			    queued.insert(pp->module);
			    Q.push(pp->module);
			}
		    } // of connection was not mapped
		} // end connections of port
	    } // end ports
	}// type of port
    } // end bfs

    // now write
    stringstream ss;

    // modules
    for (it_map_modules = map_modules.begin();it_map_modules!= map_modules.end(); it_map_modules++)
    {
	Module *m = it_map_modules->first;
	int id = it_map_modules->second;
	ss << "m = Module(" << id << "," << "\"" << m->name << "\")" << endl;
	ss << "map_modules[" << id << "] = m" << endl;
    }

    // ports
    for (it_map_ports = map_ports.begin();it_map_ports!= map_ports.end(); it_map_ports++)
    {
	Port *p = it_map_ports->first;
	int id = it_map_ports->second;
	int id_module = map_modules[p->module];
	ss << "p = Port(" << id << "," << "\"" << p->name << "\", " 
	   << (p->type == Port::INPUT ? "0" : "1")  << ", map_modules[" 
	   << id_module << "])" << endl;
	ss << "map_ports[" << id << "] = p" << endl;
	// ss << "c = ModulePortConnection(map_modules[" 
	// << map_modules[p->module] << "], p)" << endl;
	// ss << "connections_module_ports.append(c)" << endl;
    }

    // connections
    for (it_map_connections = map_connections.begin();
	 it_map_connections!= map_connections.end(); it_map_connections++)
    {
	Connection *c = it_map_connections->first;
	int id = it_map_connections->second;
	int source_id = map_ports[c->source];
	int target_id = map_ports[c->target];
	ss << "c = Connection(" << id << ", map_ports[" << source_id << "], map_ports[" 
	   << target_id  << "])" << endl;
	ss << "connections.append(c)" << endl;
    }

    return ss.str();
}


/**************
 * Connection *
 **************/

Connection::Connection(Port *source, Port *target)
{
    if (source->type != Port::OUTPUT || target->type != Port::INPUT)
	throw exception();

    this->source = source;
    this->target = target;
    source->addConnection(this);
    target->addConnection(this);
}

Connection::~Connection()
{
    // cout << "destroying connection " << this->source->module->name << "." << this->source->name 
    // 	 << " -> " << this->target->module->name << "." << this->target->name << endl;
    this->source->deleteConnection(this);
    this->target->deleteConnection(this);
}

Port*
Connection::oppositePort(Port *p)
{
    if (this->source == p)
	return this->target;
    else if (this->target == p)
	return this->source;
    else
	return NULL;
}


Module*
Connection::oppositeModule(Port *p)
{
    Port *p_opposite = this->oppositePort(p);
    return p_opposite->module;
}

