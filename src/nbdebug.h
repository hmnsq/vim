/* vi:set ts=8 sw=8 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */


#ifndef NBDEBUG_H
#define NBDEBUG_H

#ifdef NBDEBUG

# ifndef ASSERT
#  define ASSERT(c) \
    यदि (!(c)) \
    { \
	fprintf(stderr, "Assertion failed: line %d, file %s\n", \
		__LINE__, __FILE__); \
	fflush(stderr); \
	abort(); \
    }
# endif

# define nbdebug(a) nbdbg a

# define NB_TRACE		0x00000001
# define NB_TRACE_VERBOSE	0x00000002
# define NB_TRACE_COLONCMD	0x00000004
# define NB_PRINT		0x00000008
# define NB_DEBUG_ALL		0xffffffff

# define NBDLEVEL(flags)	(nb_debug != रिक्त && (nb_dlevel & (flags)))

# define NBDEBUG_TRACE	1

प्रकारनिर्देश गणना {
		WT_ENV = 1,		// look for env var if set
		WT_WAIT,		// look for ~/.gvimwait if set
		WT_STOP			// look for ~/.gvimstop if set
} WtWait;


शून्य		 nbdbg(वर्ण *, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

शून्य nbdebug_wait(u_int wait_flags, वर्ण *wait_var, u_int wait_secs);
शून्य nbdebug_log_init(वर्ण *log_var, वर्ण *level_var);

बाह्य FILE	*nb_debug;
बाह्य u_int	 nb_dlevel;		// nb_debug verbosity level

#अन्यथा		// not NBDEBUG

# ifndef ASSERT
#  define ASSERT(c)
# endif

/*
 * The following 3 stubs are needed because a macro cannot be used because of
 * the variable number of arguments.
 */

शून्य
nbdbg(
	वर्ण		*fmt,
	...)
{
}

#endif // NBDEBUG
#endif // NBDEBUG_H
