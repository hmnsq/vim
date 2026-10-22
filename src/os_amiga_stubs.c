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

#ضمن "vim.h"

#ifndef PROTO

#ضمن <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    صحيح
im_get_status(فراغ)
{
    أرجع FALSE;
}

    فراغ
im_set_active(صحيح active UNUSED)
{
}

    صحيح
set_ref_in_im_funcs(صحيح copyID UNUSED)
{
    أرجع 0;
}

    حرف *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    أرجع عدم;
}

    حرف *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    أرجع عدم;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    صحيح
mch_rmdir(char_u *name)
{
    𐤀𐤌 (DeleteFile((STRPTR)name))
	أرجع 0;
    أرجع -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#ضمن <pwd.h>
#ضمن <grp.h>

    بنية passwd *
getpwuid(uid_t uid UNUSED)
{
    أرجع عدم;
}

    بنية group *
getgrgid(gid_t gid UNUSED)
{
    أرجع عدم;
}

    uid_t
getuid(فراغ)
{
    أرجع 0;
}

#endif // PROTO
