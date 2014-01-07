/**
	@file
	yarp - yarp
			- based on collect example: collectsnumbers and operate on them.
			- demonstrates use of C++ and the STL in a Max external
			- also demonstrates use of a mutex for thread safety
			- on Windows, demonstrate project setup for static linking to the Microsoft Runtime

	@ingroup	examples

	Copyright 2009 - Cycling '74
	Timothy Place, tim@cycling74.com

	2014 - Hplus
	Johnty Wang -  johntywang@gmail.com
*/

#include "ext.h"
#include "ext_obex.h"
#include "ext_strings.h"
#include "ext_common.h"
#include "ext_systhread.h"
#include "yarp/os/all.h"

#include <vector>
using namespace std;

// a wrapper for cpost() only called for debug builds on Windows
// to see these console posts, run the DbgView program (part of the SysInternals package distributed by Microsoft)
#if defined( NDEBUG ) || defined( MAC_VERSION )
#define DPOST
#else
#define DPOST cpost
#endif

// a macro to mark exported symbols in the code without requiring an external file to define them
#ifdef WIN_VERSION
	// note that this is the required syntax on windows regardless of whether the compiler is msvc or gcc
	#define T_EXPORT __declspec(dllexport)
#else // MAC_VERSION
	// the mac uses the standard gcc syntax, you should also set the -fvisibility=hidden flag to hide the non-marked symbols
	#define T_EXPORT __attribute__((visibility("default")))
#endif

// a c++ class representing a number, and types for a vector of those numbers
class number {
	private:
		double value;
	public:
		number(double& newValue)
		{
			value = newValue;
		}

		void setValue(const double& newValue)
		{
			value = newValue;
		}

		void getValue(double& retrievedValue)
		{
			retrievedValue = value;
		}
};
typedef std::vector<number>		numberVector;
typedef numberVector::iterator	numberIterator;


// max object instance data
typedef struct _yarp {
	t_object			c_box;
	numberVector		*c_vector;	// note: you must store this as a pointer and not directly as a member of the object's struct
	void				*c_outlet;
	void				*c_inlet;
	t_systhread_mutex	c_mutex;
	yarp::os::Network *yarp;
	yarp::os::BufferedPort<yarp::os::Bottle> *port;
} t_yarp;


// prototypes
void*	yarp_new(t_symbol *s, long argc, t_atom *argv);
//void*   yarp_new(t_symbol *s);
void	yarp_free(t_yarp* x);
void	yarp_assist(t_yarp *x, void *b, long m, long a, char *s);
void	yarp_bang(t_yarp *x);
void	yarp_count(t_yarp *x);
void	yarp_int(t_yarp *x, long value);
void	yarp_float(t_yarp *x, double value);
void	yarp_list(t_yarp *x, t_symbol *msg, long argc, t_atom *argv);
void	yarp_clear(t_yarp *x);

void	yarp_read(t_yarp *x, symbol *s);
void	yarp_readbang(t_yarp *x);

// yarp init

void init_yarp(t_yarp	*x);

bool	checkAddr(char* addr);


// globals
static t_class	*s_yarp_class = NULL;

/************************************************************************************/

int T_EXPORT main(void)
{
	t_class	*c = class_new("yarp",
							(method)yarp_new,
							(method)yarp_free,
							sizeof(t_yarp),
							(method)NULL,
							A_GIMME,
							0);

	common_symbols_init();

	class_addmethod(c, (method)yarp_bang,	"bang",			0);
	class_addmethod(c, (method)yarp_int,	"int",			A_LONG,	0);
	class_addmethod(c, (method)yarp_float,	"float",		A_FLOAT,0);
	class_addmethod(c, (method)yarp_list,	"list",			A_GIMME,0);
	class_addmethod(c, (method)yarp_clear,	"clear",		0);
	class_addmethod(c, (method)yarp_count,	"count",		0);
	class_addmethod(c, (method)yarp_assist, "assist",		A_CANT, 0);
	class_addmethod(c, (method)yarp_read,   "read",			A_SYM, 0);
	class_addmethod(c, (method)yarp_readbang, "bang",		0);
	class_addmethod(c, (method)stdinletinfo,	"inletinfo",	A_CANT, 0);
	
	class_register(_sym_box, c);
	s_yarp_class = c;

	return 0;
}


/************************************************************************************/
// Object Creation Method

void *yarp_new(t_symbol *s, long argc, t_atom *argv)
{
	poststring("new yarp!\n");
	char str[32];
	sprintf(str, "num args=%ld", argc);
	poststring(str);
	
	t_yarp	*x;

	x = (t_yarp*)object_alloc(s_yarp_class);
	if (x) {
		systhread_mutex_new(&x->c_mutex, 0);
		x->c_outlet = outlet_new(x, NULL);
		x->c_inlet = inlet_new(x, "bang");
		x->c_vector = new numberVector;
		x->c_vector->reserve(10);
		//yarp_list(x, _sym_list, argc, argv);
	}

	init_yarp(x);
	if (argc==2) {
		if (strcmp("read", atom_getsym(argv)->s_name) == 0) {
			char str[32];
			sprintf(str, "%s", atom_getsym(argv+1)->s_name);
			post("reader addr = %s",str);
			if (checkAddr(str))
				x->port->open(str);
		}
	}
	else {
		x->port->open("/hello_max");
	}
	return(x);
}

//void *yarp_new(t_symbol* s) {
//	poststring("new yarp with parameter!\n");
//	poststring(s->s_name);
//	t_yarp	*x;
//
//	x = (t_yarp*)object_alloc(s_yarp_class);
//	if (x) {
//		systhread_mutex_new(&x->c_mutex, 0);
//		x->c_outlet = outlet_new(x, NULL);
//		x->c_vector = new numberVector;
//		x->c_vector->reserve(10);
//		//yarp_list(x, _sym_list, argc, argv);
//	}
//	init_yarp(x);
//	x->port->open(s->s_name);
//	return(x);
//}

void init_yarp(t_yarp	*x) {
	x->yarp = new yarp::os::Network();
	x->port = new yarp::os::BufferedPort<yarp::os::Bottle>();
}


void yarp_free(t_yarp *x)
{
	x->port->close();
	delete x->port;
	delete x->yarp;
	systhread_mutex_free(x->c_mutex);
	delete x->c_vector;
}


/************************************************************************************/
// Methods bound to input/inlets

void yarp_assist(t_yarp *x, void *b, long msg, long arg, char *dst)
{
	if (msg==1)
		strcpy(dst, "input");
	else if (msg==2)
		strcpy(dst, "output");
}


void yarp_bang(t_yarp *x)
{
	numberIterator iter, begin, end;
	int i = 0;
	long ac = 0;
	t_atom *av = NULL;
	double value;

	DPOST("head\n");
	systhread_mutex_lock(x->c_mutex);
	ac = x->c_vector->size();

	DPOST("ac=%ld\n", ac);
	if (ac)
		av = new t_atom[ac];

	if (ac && av) {
		DPOST("assigning begin and end\n");
		begin = x->c_vector->begin();
		end = x->c_vector->end();

		DPOST("assigning iter\n");
		iter = begin;

		DPOST("entering for\n", ac);
		for (;;) {
			DPOST("i=%i\n", i);
			(*iter).getValue(value);
			atom_setfloat(av+i, value);

			DPOST("incrementing\n");
			i++;
			iter++;

			DPOST("comparing\n");
			if (iter == end)
				break;
		}
		systhread_mutex_unlock(x->c_mutex);	// must unlock before calling _clear() or we will deadlock

		DPOST("about to clear\n", ac);
		yarp_clear(x);

		DPOST("about to outlet\n", ac);
		outlet_anything(x->c_outlet, _sym_list, ac, av); // don't want to call outlets in mutexes either

		DPOST("about to delete\n", ac);
		delete[] av;
	}
	else
		systhread_mutex_unlock(x->c_mutex);
}


void yarp_count(t_yarp *x)
{
	outlet_int(x->c_outlet, x->c_vector->size());
}

//sets up yarp reader with port name
void yarp_read(t_yarp *x, symbol *s)
{
	poststring("yarp read!");
	post(s->s_name);
	if (!x->port->isClosed())
		x->port->close();
		
	if (checkAddr(s->s_name))
		x->port->open(s->s_name);

	//just pass the input directly out
	t_atom* argv = NULL;
	argv = new t_atom();
	atom_setsym(argv, s);
	outlet_anything(x->c_outlet, gensym("output"), 1, argv);
	delete argv;
}

void yarp_int(t_yarp *x, long value)
{
	yarp_float(x, value);
	post("int val = %ld", (long)value);
}


void yarp_float(t_yarp *x, double value)
{
	systhread_mutex_lock(x->c_mutex);
	x->c_vector->push_back(value);
	systhread_mutex_unlock(x->c_mutex);
}


void yarp_list(t_yarp *x, t_symbol *msg, long argc, t_atom *argv)
{

	char str[32];
	sprintf(str, "num= %ld",argc);
	poststring(str);

	/*systhread_mutex_lock(x->c_mutex);
	for (int i=0; i<argc; i++) {
		double value = atom_getfloat(argv+i);
		x->c_vector->push_back(value);
	}
	systhread_mutex_unlock(x->c_mutex);*/
}

void yarp_readbang(t_yarp *x)
{
	//post("metro bang!");
	if (x->port->getPendingReads()) {
		post("   read:");
		yarp::os::Bottle *input = x->port->read();
		if (input != NULL) {
			t_atom* argv = new t_atom();
			atom_setsym(argv, gensym(input->toString().c_str()));
			outlet_anything(x->c_outlet, gensym("read"), 1, argv);
			delete argv;
		
		}
	}

}


void yarp_clear(t_yarp *x)
{
	systhread_mutex_lock(x->c_mutex);
	x->c_vector->clear();
	systhread_mutex_unlock(x->c_mutex);
}

bool checkAddr(char* addr) 
{
	if (addr[0] != '/') {
		error("port name must start with '/'");
		return false;
	}
	else {
		return true;
	}
}