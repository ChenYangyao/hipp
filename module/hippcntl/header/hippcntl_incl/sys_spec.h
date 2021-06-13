#if defined (__unix__)
#define _HIPP_SYS_SPEC_UNIX
#endif

#if defined (__APPLE__) && defined (__MACH__)
#define _HIPP_SYS_SPEC_MACH
#endif

#if defined (__linux__)
#define _HIPP_SYS_SPEC_LINUX
#endif 

#if defined (_HIPP_SYS_SPEC_UNIX) || defined (_HIPP_SYS_SPEC_MACH)
#define _HIPP_SYS_SPEC_POSIX
#endif