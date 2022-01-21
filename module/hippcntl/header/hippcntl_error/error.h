/**
create: Yangyao CHEN, 2019/12/28
The total header file composed of all headers in this error sub-module.
*/ 

#ifndef _HIPPCNTL_ERROR_H_
#define _HIPPCNTL_ERROR_H_

/**
Exception class layers.

std::exception
   |- ErrApp: application indicator
   |  |- ErrAppDefault -----------o---o
   |  |- ErrAppUnknown            |   |
   |  |- ErrAppSystem  -------------------o
   |  |- ErrAppMPI                |   |   |
   |  |- ErrAppOpenMP             |   |   |
   |                              |   |   |
   |- ErrClass: class indicator   |   |   |
      |- ErrClassDefault  ------------|---o
      |- ErrClassUnknown          |   |   |    
      |- ErrClassRuntime  --------|---o   |
      |- ErrClassLogic  ----------o   |   |
      |- ErrClassMemory           |   |   |
      |- ErrClassCast             |   |   |   
      |- ErrClassIO               |   |   |
                                  |   |   |- ErrSystem: system call errors
                                  |   |- ErrRuntime: runtime errors  
                                  |- ErrLogic: logic errors 

Each class in the layers has 
- a ``what()`` virtual method that returns a brief error message represented by 
  the current instance;
- a ``whats()`` virtual method that returns more verbose error message.
Note that ``std::exception`` has only ``what()``.

Examples
----------
try{
    void *p = aligned_alloc(...);
    if( !p )
        throw ErrSystem( errno );
}catch( std::ErrClassDefault &ec ){
    cout << ec.whats() << endl;  // or ec.what() that reports the current layer error
}catch( std::exception &e ){
    cout << e.what() << endl;
}

The hierarchy structure of exceptions is defined in the following headers.
*/
#include "error_application.h"
#include "error_class.h"
#include "error_type.h" 


/**
Useful macros for reporting exceptions.
*/
#include "error_macro.h"


#endif	//_HIPPCNTL_ERROR_H_