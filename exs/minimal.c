#include <stdarg.h> 
#include <stdio.h> 

#include <squirrel.h> 
#include <sqstdio.h> 
#include <sqstdaux.h> 
#include <sqstdstring.h>

#ifdef _MSC_VER
#pragma comment (lib ,"libsquirrel.lib")
#pragma comment (lib ,"libsqstdlib.lib")
#endif

#ifdef SQUNICODE 

#define scvprintf vfwprintf
#else 

#define scvprintf vfprintf
#endif 

void printfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stdout, s, vl);
	va_end(vl);
}

void errorfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stderr, s, vl);
	va_end(vl);
}

void call_foo(HSQUIRRELVM v, int n, float f, const SQChar *s)
{
	SQInteger top = sq_gettop(v); //saves the stack size before the call
	sq_pushroottable(v); //pushes the global table
	sq_pushstring(v, _SC("foo"), -1);
	if (SQ_SUCCEEDED(sq_get(v, -2)))
	{ //gets the field 'foo' from the global table
		sq_pushroottable(v); //push the 'this' (in this case is the global table)
		sq_pushinteger(v, n);
		sq_pushfloat(v, f);
		sq_pushstring(v, s, -1);
		sq_call(v, 4, SQFalse, SQTrue); //calls the function 
	}

	sq_settop(v, top); //restores the original stack size
}

void call_print_prova(HSQUIRRELVM v)
{
	SQInteger top = sq_gettop(v);	     //saves the stack size before the call
	sq_pushroottable(v);			     //pushes the global table
	sq_pushstring(v, _SC("myprint"), -1);
	if (SQ_SUCCEEDED(sq_get(v, -2)))
	{
		//gets the field 'foo' from the global table
		sq_pushroottable(v); //push the 'this' (in this case is the global table)
		SQChar* str = _SC("Ep, gent!");
		sq_pushstring(v, str, -1);
		sq_call(v, 2, SQFalse, SQTrue); //calls the function
	}
	sq_settop(v, top); //restores the original stack size
}

void call_print_hulla(HSQUIRRELVM v)
{
	SQInteger top = sq_gettop(v);	     //saves the stack size before the call
	sq_pushroottable(v);			     //pushes the global table
	sq_pushstring(v, _SC("hola"), -1);
	if (SQ_SUCCEEDED(sq_get(v, -2)))
	{
		//gets the field 'foo' from the global table
		sq_pushroottable(v); //push the 'this' (in this case is the global table)
		sq_call(v, 1, SQFalse, SQTrue); //calls the function
	}
	sq_settop(v, top); //restores the original stack size
}

SQInteger print_prova(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v);
	SQChar* str;
	sq_getstring(v, -1, &str);
	SQPRINTFUNCTION pf = sq_getprintfunc(v);
	pf(v, str);

	return 0;
}

SQInteger register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char *fname)
{
	sq_pushroottable(v);
	sq_pushstring(v, fname, -1);
	sq_newclosure(v, f, 0, 0); //create a new function
	sq_newslot(v, -3, SQFalse);
	sq_pop(v, 1); //pops the root table
}

//SQInteger register_global_var(HSQUIRRELVM v, );
//SQChar* prgm = _SC("print(\"Hello World!\"); return 9");

SQChar* prgm = _SC(" function hulla() { \
	print(\"hello guys\\n\"); \
}");


int main(int argc, char* argv[])
{
	HSQUIRRELVM v;
	v = sq_open(1024); // creates a VM with initial stack size 1024 


	//REGISTRATION OF STDLIB
	//sq_pushroottable(v); //push the root table where the std function will be registered
	//sqstd_register_iolib(v);  //registers a library
	// ... call here other stdlibs string,math etc...
	//sq_pop(v,1); //pops the root table
	//END REGISTRATION OF STDLIB

	sqstd_seterrorhandlers(v); //registers the default error handlers

	sq_setprintfunc(v, printfunc, errorfunc); //sets the print function

	SQInteger retval = 0;
	SQInteger oldtop = sq_gettop(v);
	if (SQ_SUCCEEDED(sq_compilebuffer(v, prgm, scstrlen(prgm), _SC("simple lines"), SQTrue)))
	{
		sq_pushroottable(v);

		//show retval
		if (SQ_SUCCEEDED(sq_call(v, 1, retval, SQTrue)) && retval) {
			//scprintf(_SC("\n"));
			sq_pushroottable(v);
			sq_pushstring(v, _SC("print"), -1);
			sq_get(v, -2);
			sq_pushroottable(v);
			sq_push(v, -4);
			sq_call(v, 2, SQFalse, SQTrue);
			retval = 0;
			//scprintf(_SC("\n"));
		}

	}

	sq_settop(v, oldtop);


	register_global_func(v, print_prova, _SC("myprint"));

	call_print_prova(v);

	call_print_hulla(v);

	sq_pushroottable(v); //push the root table(were the globals of the script will be stored)
	if (SQ_SUCCEEDED(sqstd_dofile(v, _SC("test.nut"), SQFalse, SQTrue))) // also prints syntax errors if any 
	{
		call_foo(v, 1, 2.5, _SC("teststring"));
	}

	sq_pop(v, 1); //pops the root table
	sq_close(v);

	return 0;
}
