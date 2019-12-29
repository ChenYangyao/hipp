/**
 * creat: Yangyao CHEN, 2019/12/28
 *      [write   ] em - error macros.
 */ 

#ifndef _HIPPCNTL_ERROR_MACRO_H_
#define _HIPPCNTL_ERROR_MACRO_H_

/**
 * expand to comma separated constexpr strings as debug functions arguments.
 * Here prefix `em` means error macro. Postfix is one of the following:
 *      FF:     file and function name.
 *      FLF:    file name, line number and function name.
 *      FPF:    file and 'pretty' version of function name.
 *      FLPF:   file name, line number and 'pretty' version of function name.
 * 
 * Examples
 * ----------
 *  // print current source file name and function name to the `cout`
 *  prt( cout, emFF );
 *  // This expands by pre-processor as: 
 *  //      prt( cout, "[ file ] ", __FILE__, ", [ function ] ", __func__ );
 *  // In the following pre-processor steps, __FILE__ and __func__ are expanded
 *  // further into the filename and function name
 */
#define emFF "[ file ] ", __FILE__, \
    ", [ function ] ", __func__
#define emFLF "[ file ] ", __FILE__, \
    ", [ line ] ", __LINE__, \
    ", [ function ] ", __func__
#define emFPF "[ file ] ", __FILE__, \
    ", [ function ] ", __PRETTY_FUNCTION__
#define emFLPF "[ file ] ", __FILE__, \
    ", [ line ] ", __LINE__, \
    ", [ function ] ", __PRETTY_FUNCTION__

#endif	//_HIPPCNTL_ERROR_MACRO_H_