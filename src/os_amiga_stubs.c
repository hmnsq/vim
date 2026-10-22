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

#অন্তর্ভাব "vim.h"

#ifndef PROTO

#অন্তর্ভাব <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    পূর্ণাঙ্ক
im_get_status(শূন্য)
{
    প্রত্যাগম FALSE;
}

    শূন্য
im_set_active(পূর্ণাঙ্ক active UNUSED)
{
}

    পূর্ণাঙ্ক
set_ref_in_im_funcs(পূর্ণাঙ্ক copyID UNUSED)
{
    প্রত্যাগম 0;
}

    বর্ণ *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    প্রত্যাগম রিক্ত;
}

    বর্ণ *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    প্রত্যাগম রিক্ত;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    পূর্ণাঙ্ক
mch_rmdir(char_u *name)
{
    যদি (DeleteFile((STRPTR)name))
	প্রত্যাগম 0;
    প্রত্যাগম -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#অন্তর্ভাব <pwd.h>
#অন্তর্ভাব <grp.h>

    সংরচনা passwd *
getpwuid(uid_t uid UNUSED)
{
    প্রত্যাগম রিক্ত;
}

    সংরচনা group *
getgrgid(gid_t gid UNUSED)
{
    প্রত্যাগম রিক্ত;
}

    uid_t
getuid(শূন্য)
{
    প্রত্যাগম 0;
}

#endif // PROTO
