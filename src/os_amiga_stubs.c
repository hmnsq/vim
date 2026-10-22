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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifndef PROTO

#ଅନ୍ତର୍ଭାଵ <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
im_get_status(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ FALSE;
}

    ଶୂନ୍ୟ
im_set_active(ପୂର୍ଣ୍ଣସଂଖ୍ୟା active UNUSED)
{
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_im_funcs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID UNUSED)
{
    ଫେରନ୍ତୁ 0;
}

    ଅକ୍ଷର *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ ଖାଲି;
}

    ଅକ୍ଷର *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_rmdir(char_u *name)
{
    ଯଦି (DeleteFile((STRPTR)name))
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#ଅନ୍ତର୍ଭାଵ <pwd.h>
#ଅନ୍ତର୍ଭାଵ <grp.h>

    ସଂରଚନା passwd *
getpwuid(uid_t uid UNUSED)
{
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସଂରଚନା group *
getgrgid(gid_t gid UNUSED)
{
    ଫେରନ୍ତୁ ଖାଲି;
}

    uid_t
getuid(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ 0;
}

#endif // PROTO
