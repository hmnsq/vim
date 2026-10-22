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
    puur_naa_nka
im_get_status(void)
{
    ꦮꦭꦶ FALSE;
}

    void
im_set_active(puur_naa_nka active UNUSED)
{
}

    puur_naa_nka
set_ref_in_im_funcs(puur_naa_nka copyID UNUSED)
{
    ꦮꦭꦶ 0;
}

    var_na *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    ꦮꦭꦶ NULL;
}

    var_na *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    ꦮꦭꦶ NULL;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    puur_naa_nka
mch_rmdir(char_u *name)
{
    ꦪꦼꦤ꧀ (DeleteFile((STRPTR)name))
	ꦮꦭꦶ 0;
    ꦮꦭꦶ -1;
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
    ꦮꦭꦶ NULL;
}

    sa_nracanaa group *
getgrgid(gid_t gid UNUSED)
{
    ꦮꦭꦶ NULL;
}

    uid_t
getuid(void)
{
    ꦮꦭꦶ 0;
}

#endif // PROTO
