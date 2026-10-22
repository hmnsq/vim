/* vi:set ts=8 sw=8 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NBDEBUG_H
#ವ್ಯಾಖ್ಯಾನಿಸು NBDEBUG_H

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG

# ifndef ASSERT
#  define ASSERT(c) \
    ಆದರೆ (!(c)) \
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

# define NBDLEVEL(flags)	(nb_debug != ಖಾಲಿ && (nb_dlevel & (flags)))

# define NBDEBUG_TRACE	1

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
		WT_ENV = 1,		// look for env var if set
		WT_WAIT,		// look for ~/.gvimwait if set
		WT_STOP			// look for ~/.gvimstop if set
} WtWait;


ಶೂನ್ಯ		 nbdbg(ಅಕ್ಷರ *, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

ಶೂನ್ಯ nbdebug_wait(u_int wait_flags, ಅಕ್ಷರ *wait_var, u_int wait_secs);
ಶೂನ್ಯ nbdebug_log_init(ಅಕ್ಷರ *log_var, ಅಕ್ಷರ *level_var);

ಬಾಹ್ಯ FILE	*nb_debug;
ಬಾಹ್ಯ u_int	 nb_dlevel;		// nb_debug verbosity level

#ಇಲ್ಲದಿದ್ದರೆ		// not NBDEBUG

# ifndef ASSERT
#  define ASSERT(c)
# endif

/*
 * The following 3 stubs are needed because a macro cannot be used because of
 * the variable number of arguments.
 */

ಶೂನ್ಯ
nbdbg(
	ಅಕ್ಷರ		*fmt,
	...)
{
}

#ಆದರೆ_ಅಂತ್ಯ // NBDEBUG
#ಆದರೆ_ಅಂತ್ಯ // NBDEBUG_H
