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
    số_nguyên
im_get_status(void)
{
    trả_về FALSE;
}

    void
im_set_active(số_nguyên active UNUSED)
{
}

    số_nguyên
set_ref_in_im_funcs(số_nguyên copyID UNUSED)
{
    trả_về 0;
}

    char *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    trả_về NULL;
}

    char *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    trả_về NULL;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    số_nguyên
mch_rmdir(char_u *name)
{
    nếu (DeleteFile((STRPTR)name))
	trả_về 0;
    trả_về -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#include <pwd.h>
#include <grp.h>

    struct passwd *
getpwuid(uid_t uid UNUSED)
{
    trả_về NULL;
}

    struct group *
getgrgid(gid_t gid UNUSED)
{
    trả_về NULL;
}

    uid_t
getuid(void)
{
    trả_về 0;
}

#endif // PROTO
