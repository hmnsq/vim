/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر_تعریف_شدہ __QNXNTO__
# include <sys/procmgr.h>
#اگر_ختم

#تعریف_کرو	USE_TMPNAM

#تعریف_کرو POSIX	    // Used by pty.c

#اگر defined(FEAT_GUI_PHOTON)
خارجی صحیح is_photon_دستیاب;
#اگر_ختم
