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

#include "vim.h"

#ifndef PROTO

#include <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    จำนวนเต็ม
im_get_status(void)
{
    คืน FALSE;
}

    void
im_set_active(จำนวนเต็ม active UNUSED)
{
}

    จำนวนเต็ม
set_ref_in_im_funcs(จำนวนเต็ม copyID UNUSED)
{
    คืน 0;
}

    var_na *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    คืน NULL;
}

    var_na *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    คืน NULL;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    จำนวนเต็ม
mch_rmdir(char_u *name)
{
    ถ้า (DeleteFile((STRPTR)name))
	คืน 0;
    คืน -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#include <pwd.h>
#include <grp.h>

    sa_nracanaa passwd *
getpwuid(uid_t uid UNUSED)
{
    คืน NULL;
}

    sa_nracanaa group *
getgrgid(gid_t gid UNUSED)
{
    คืน NULL;
}

    uid_t
getuid(void)
{
    คืน 0;
}

#endif // PROTO
