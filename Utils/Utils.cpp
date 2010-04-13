#include "utils.h"


/**
 * Register an external printFunction
 */
PyObject *printFunction = NULL;
void registerPrintFunction(PyObject *pF){
    Py_XINCREF(pF);             /* Add a reference to new callback */
    Py_XDECREF(printFunction);  /* Dispose of previous callback */
    printFunction = pF;         /* Remember new callback */
}

/**
 * Output the message to a file...
 */
int tprintf(const char *format, ...){
    va_list vl;
	va_start(vl, format);   
	tvprintf(format, vl);
	va_end(vl);

	return 0;
}

/**
 * Output the message to a file...
 */
int tvprintf(const char *format, va_list vl){
    static char message[1024];

	vsprintf(message, format, vl);

	if( printFunction == NULL )
		printf( "%s", message );
	else {
		PyObject *arglist;
		PyObject *result;
		arglist = Py_BuildValue("(s)", message);
		result = PyEval_CallObject(printFunction,arglist);
		Py_DECREF(arglist);
		if (result == NULL) {
			// Unbind function
			registerPrintFunction(NULL);
			throwError( "The Python print function threw an exception!" );
		}
		Py_DECREF(result);
	}

	return 0;
}




