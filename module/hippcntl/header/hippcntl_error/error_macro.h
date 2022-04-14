/**
create: Yangyao CHEN, 2019/12/28
    [write   ] em - error macros.
*/ 
#ifndef _HIPPCNTL_ERROR_MACRO_H_
#define _HIPPCNTL_ERROR_MACRO_H_

/**
Expand to comma separated constexpr strings as debug functions arguments.
Here prefix `em` means error macro. Postfix is one of the following:
    F：     function name.
    FF:     file and function name.
    FLF:    file name, line number and function name.
    FLFB:   same as FLF, but print a end-line character after each item
            so that they are coarser.
    
    PF:     'pretty' version of function name.
    FPF:    file and 'pretty' version of function name.
    FLPF:   file name, line number and 'pretty' version of function name.
    FLPFB:  same as FLPF, but print a end-line character after each item
            so that they are coarser.

The prefix `ems` means a short version of `em`. The suffix can be
F, FF, FLF, PF, FPF, FLPF

Examples
----------
// Print current source file name and function name to the `cout`:
prt(cout, emFF, "begins execution");
// This expanded by pre-processor as: 
prt( cout, "[file] ", __FILE__, ", [func] ", __func__, ' ', "begins execution");
// In the following pre-processor steps, __FILE__ and __func__ are expanded
// further into the filename and function name. For example, in the
// main.cpp file's main function, the output is
[file] main.cpp, [func] main begins execution

// Use the short version, like
pout << emsFF, " begins execution";
// This is expanded as
pout << __FILE__, ", ", __func__, " begins execution";
// The output is
main.cpp, main begins execution

*/
#define emF "[func] ", __func__, ' '
#define emFF \
    "[file] ", __FILE__, \
    ", [func] ", __func__, ' '
#define emFLF \
    "[file] ", __FILE__, \
    ", [line] ", __LINE__, \
    ", [func] ", __func__, ' '
#define emFLFB \
    "[file] ", __FILE__, \
    "\n[line] ", __LINE__, \
    "\n[func] ", __func__, '\n'

#define emPF "[func] ", __PRETTY_FUNCTION__, ' '
#define emFPF \
    "[file] ", __FILE__, \
    ", [func] ", __PRETTY_FUNCTION__, ' '
#define emFLPF \
    "[file] ", __FILE__, \
    ", [line] ", __LINE__, \
    ", [func] ", __PRETTY_FUNCTION__, ' '
#define emFLPFB \
    "[file] ", __FILE__, \
    "\n[line] ", __LINE__, \
    "\n[func] ", __PRETTY_FUNCTION__, '\n'

#define emsF __func__
#define emsFF __FILE__, ", ", __func__
#define emsFLF __FILE__, ", ", __LINE__, ", ", __func__

#define emsPF __PRETTY_FUNCTION__
#define emsFPF __FILE__, ", ", __PRETTY_FUNCTION__
#define emsFLPF __FILE__, ", ", __LINE__, ", ", __PRETTY_FUNCTION__

/**
Formatted IO helpers for ``info()`` of classes.
*/
#define HIPPCNTL_CLASS_INFO(class_name) \
    "<" #class_name "> {", (void *)this, ", size=", sizeof(class_name), "}\n"
#define HIPPCNTL_CLASS_INFO_INLINE(class_name) \
    "<" #class_name "> "
#define HIPPCNTL_CLASS_INFO_INDENT 2
#define HIPPCNTL_CLASS_INFO_SUBHEAD "   |- "
#define HIPPCNTL_CLASS_INFO_INDENT_STR(level) \
    string(level*HIPPCNTL_CLASS_INFO_INDENT, ' ') + HIPPCNTL_CLASS_INFO_SUBHEAD

#endif	//_HIPPCNTL_ERROR_MACRO_H_