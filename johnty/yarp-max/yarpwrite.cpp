/**
	@file
	yarpwrite - yarpwrite numbers and operate on them.
			- demonstrates use of C++ and the STL in a Max external
			- also demonstrates use of a mutex for thread safety
			- on Windows, demonstrate project setup for static linking to the Microsoft Runtime

	@ingroup	examples

	Copyright 2009 - Cycling '74
	Timothy Place, tim@cycling74.com
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
typedef struct _yarpwrite {
	t_object			c_box;
	numberVector		*c_vector;	// note: you must store this as a pointer and not directly as a member of the object's struct
	void				*c_outlet;
	t_systhread_mutex	c_mutex;
	yarp::os::Network *yarp;
	yarp::os::BufferedPort<yarp::os::Bottle> *port;
} t_yarpwrite;


// prototypes
void*	yarpwrite_new(t_symbol *s, long argc, t_atom *argv);
//void*   yarpwrite_new(t_symbol *s);
void	yarpwrite_free(t_yarpwrite* x);
void	yarpwrite_assist(t_yarpwrite *x, void *b, long m, long a, char *s);
void	yarpwrite_bang(t_yarpwrite *x);
void	yarpwrite_count(t_yarpwrite *x);
void	yarpwrite_int(t_yarpwrite *x, long value);
void	yarpwrite_float(t_yarpwrite *x, double value);
void	yarpwrite_list(t_yarpwrite *x, t_symbol *msg, long argc, t_atom *argv);
void	yarpwrite_clear(t_yarpwrite *x);

// yarp init

void init_yarp(t_yarpwrite	*x);


// globals
static t_class	*s_yarpwrite_class = NULL;

/************************************************************************************/

int T_EXPORT main(void)
{
	t_class	*c = class_new("yarpwrite",
							(method)yarpwrite_new,
							(method)yarpwrite_free,
							sizeof(t_yarpwrite),
							(method)NULL,
							A_GIMME,
							0);

	common_symbols_init();

	class_addmethod(c, (method)yarpwrite_bang,	"bang",			0);
	class_addmethod(c, (method)yarpwrite_int,	"int",			A_LONG,	0);
	class_addmethod(c, (method)yarpwrite_float,	"float",		A_FLOAT,0);
	class_addmethod(c, (method)yarpwrite_list,	"list",			A_GIMME,0);
	class_addmethod(c, (method)yarpwrite_clear,	"clear",		0);
	class_addmethod(c, (method)yarpwrite_count,	"count",		0);
	class_addmethod(c, (method)yarpwrite_assist, "assist",		A_CANT, 0);
	class_addmethod(c, (method)stdinletinfo,	"inletinfo",	A_CANT, 0);

	class_register(_sym_box, c);
	s_yarpwrite_class = c;

	return 0;
}


/************************************************************************************/
// Object Creation Method

void *yarpwrite_new(t_symbol *s, long argc, t_atom *argv)
{
	poststring("new yarpwrite!\n");
	t_yarpwrite	*x;

	x = (t_yarpwrite*)object_alloc(s_yarpwrite_class);
	if (x) {
		systhread_mutex_new(&x->c_mutex, 0);
		x->c_outlet = outlet_new(x, NULL);
		x->c_vector = new numberVector;
		x->c_vector->reserve(10);
		yarpwrite_list(x, _sym_list, argc, argv);
	}

	init_yarp(x);
	x->port->open("/hello_max");
	return(x);
}

//void *yarpwrite_new(t_symbol* s) {
//	poststring("new yarpwrite with parameter!\n");
//	poststring(s->s_name);
//	t_yarpwrite	*x;
//
//	x = (t_yarpwrite*)object_alloc(s_yarpwrite_class);
//	if (x) {
//		systhread_mutex_new(&x->c_mutex, 0);
//		x->c_outlet = outlet_new(x, NULL);
//		x->c_vector = new numberVector;
//		x->c_vector->reserve(10);
//		//yarpwrite_list(x, _sym_list, argc, argv);
//	}
//	init_yarp(x);
//	x->port->open(s->s_name);
//	return(x);
//}

void init_yarp(t_yarpwrite	*x) {
	x->yarp = new yarp::os::Network();
	x->port = new yarp::os::BufferedPort<yarp::os::Bottle>();
}


void yarpwrite_free(t_yarpwrite *x)
{
	x->port->close();
	delete x->port;
	delete x->yarp;
	systhread_mutex_free(x->c_mutex);
	delete x->c_vector;
}


/************************************************************************************/
// Methods bound to input/inlets

void yarpwrite_assist(t_yarpwrite *x, void *b, long msg, long arg, char *dst)
{
	if (msg==1)
		strcpy(dst, "input");
	else if (msg==2)
		strcpy(dst, "output");
}


void yarpwrite_bang(t_yarpwrite *x)
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
		yarpwrite_clear(x);

		DPOST("about to outlet\n", ac);
		outlet_anything(x->c_outlet, _sym_list, ac, av); // don't want to call outlets in mutexes either

		DPOST("about to delete\n", ac);
		delete[] av;
	}
	else
		systhread_mutex_unlock(x->c_mutex);
}


void yarpwrite_count(t_yarpwrite *x)
{
	outlet_int(x->c_outlet, x->c_vector->size());
}


void yarpwrite_int(t_yarpwrite *x, long value)
{
	yarpwrite_float(x, value);
	post("int val = %ld", (long)value);

	char name[32];
	sprintf(name, "/max_port_%ld",value);

	if (!x->port->isClosed())
		x->port->close();
	x->port->open(name);


}


void yarpwrite_float(t_yarpwrite *x, double value)
{
	systhread_mutex_lock(x->c_mutex);
	x->c_vector->push_back(value);
	systhread_mutex_unlock(x->c_mutex);
}


void yarpwrite_list(t_yarpwrite *x, t_symbol *msg, long argc, t_atom *argv)
{
	systhread_mutex_lock(x->c_mutex);
	for (int i=0; i<argc; i++) {
		double value = atom_getfloat(argv+i);
		x->c_vector->push_back(value);
	}
	systhread_mutex_unlock(x->c_mutex);
}


void yarpwrite_clear(t_yarpwrite *x)
{
	systhread_mutex_lock(x->c_mutex);
	x->c_vector->clear();
	systhread_mutex_unlock(x->c_mutex);
}

