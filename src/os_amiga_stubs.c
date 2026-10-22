/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * os_amiga_stubs.c
 *
 * Stubs for functions referenced by Vim but not available on AmigaOS.
 * Split into a separate file to keep os_amiga.c clean.
 */

#અન્તર્ભાવ "vim.h"

#ifndef PROTO

#અન્તર્ભાવ <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    પૂર્ણાંક
im_get_status(શૂન્ય)
{
    પાછા FALSE;
}

    શૂન્ય
im_set_active(પૂર્ણાંક active UNUSED)
{
}

    પૂર્ણાંક
set_ref_in_im_funcs(પૂર્ણાંક copyID UNUSED)
{
    પાછા 0;
}

    અક્ષર *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    પાછા ખાલી;
}

    અક્ષર *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    પાછા ખાલી;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    પૂર્ણાંક
mch_rmdir(char_u *name)
{
    જો (DeleteFile((STRPTR)name))
	પાછા 0;
    પાછા -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#અન્તર્ભાવ <pwd.h>
#અન્તર્ભાવ <grp.h>

    સંરચના passwd *
getpwuid(uid_t uid UNUSED)
{
    પાછા ખાલી;
}

    સંરચના group *
getgrgid(gid_t gid UNUSED)
{
    પાછા ખાલી;
}

    uid_t
getuid(શૂન્ય)
{
    પાછા 0;
}

#endif // PROTO
