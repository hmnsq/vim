/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر_تعریف_شده __QNXNTO__
# include <sys/procmgr.h>
#پایان_اگر

#تعریف_کن	USE_TMPNAM

#تعریف_کن POSIX	    // Used by pty.c

#إذا defined(FEAT_GUI_PHOTON)
خارجي صحيح is_photon_موجود;
#پایان_اگر
