/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __QNXNTO__
# include <sys/procmgr.h>
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	USE_TMPNAM

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POSIX	    // Used by pty.c

#ਜੇ defined(FEAT_GUI_PHOTON)
ਬਾਹਰੀ ਪੂਰਨ is_photon_ਉਪਲਬਧ;
#ਜੇ_ਅੰਤ
