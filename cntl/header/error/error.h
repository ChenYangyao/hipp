#ifndef _HIPPCNTL_ERROR_H_
#define _HIPPCNTL_ERROR_H_
#include "error_application.h"
#include "error_class.h"
#include "error_type.h"

#include "error_macro.h"

/**
 * exception class layers
 * 
 * std::exception
 *         |- ErrApp: application oriented error
 *         |       |- ErrAppDefault -----------o
 *         |       |- ErrAppUnknown            |
 *         |       |- ErrAppSystem  -----------------------------------------------o
 *         |       |- ErrAppMPI                |                                   |
 *         |       |- ErrAppOpenMP             |                                   |
 *         |                                   |                                   |
 *         |- ErrClass: logic class of error   |                                   |
 *                 |- ErrClassDefault  --------------------------------------------o
 *                 |- ErrClassUnknown          |                                   |    
 *                 |- ErrClassRuntime----------o                                   |
 *                 |- ErrClassLogic            |                                   |
 *                 |- ErrClassMemory           |                                   |
 *                 |- ErrClassCast             |                                   |   
 *                 |- ErrClassIO               |                                   |
 *                                             |                                   |- ErrSystem: system call errors
 *                                             |
 *                                             |- ErrRuntime: runtime errors 
 * 
 * Each class in the layers has a what() virtual method reporting the error in the current class,
 * and a whats() virtual method reporint the full error information along the derivation chain.
 * 
 * Examples:
 *  try{
 *      void *p = aligned_alloc(...);
 *      if( !p )
 *          throw ErrSystem( errno );
 *  }catch( std::ErrClassDefault &ec ){
 *      cout << ec.whats() << endl;         // or ec.what() that reports the current layer error
 *  }catch( std::exception &e ){
 *      cout << e.what() << endl;
 *  }
 */ 


#endif	//_HIPPCNTL_ERROR_H_