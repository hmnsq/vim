/* vi:set ts=8 sw=8 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NBDEBUG_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NBDEBUG_H

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG

# ifndef ASSERT
#  define ASSERT(c) \
    ਜੇ (!(c)) \
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

# define NBDLEVEL(flags)	(nb_debug != ਨਿਰਾ && (nb_dlevel & (flags)))

# define NBDEBUG_TRACE	1

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
		WT_ENV = 1,		// look for env var if set
		WT_WAIT,		// look for ~/.gvimwait if set
		WT_STOP			// look for ~/.gvimstop if set
} WtWait;


ਖਾਲੀ		 nbdbg(ਅੱਖਰ *, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

ਖਾਲੀ nbdebug_wait(u_int wait_flags, ਅੱਖਰ *wait_var, u_int wait_secs);
ਖਾਲੀ nbdebug_log_init(ਅੱਖਰ *log_var, ਅੱਖਰ *level_var);

ਬਾਹਰੀ FILE	*nb_debug;
ਬਾਹਰੀ u_int	 nb_dlevel;		// nb_debug verbosity level

#ਨਹੀਂਤਾਂ		// not NBDEBUG

# ifndef ASSERT
#  define ASSERT(c)
# endif

/*
 * The following 3 stubs are needed because a macro cannot be used because of
 * the variable number of arguments.
 */

ਖਾਲੀ
nbdbg(
	ਅੱਖਰ		*fmt,
	...)
{
}

#ਜੇ_ਅੰਤ // NBDEBUG
#ਜੇ_ਅੰਤ // NBDEBUG_H
