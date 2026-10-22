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

#شامل "vim.h"

#ifndef PROTO

#شامل <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    صحیح
im_get_status(تهی)
{
    بازگشت FALSE;
}

    تهی
im_set_active(صحیح active UNUSED)
{
}

    صحیح
set_ref_in_im_funcs(صحیح copyID UNUSED)
{
    بازگشت 0;
}

    نویسه *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    بازگشت پوچ;
}

    نویسه *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    بازگشت پوچ;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    صحیح
mch_rmdir(char_u *name)
{
    اگر (DeleteFile((STRPTR)name))
	بازگشت 0;
    بازگشت -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#شامل <pwd.h>
#شامل <grp.h>

    ساختار passwd *
getpwuid(uid_t uid UNUSED)
{
    بازگشت پوچ;
}

    ساختار group *
getgrgid(gid_t gid UNUSED)
{
    بازگشت پوچ;
}

    uid_t
getuid(تهی)
{
    بازگشت 0;
}

#endif // PROTO
