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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifndef PROTO

#ಅನ್ತರ್ಭಾವ <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    ಪೂರ್ಣಾಙ್ಕ
im_get_status(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಶೂನ್ಯ
im_set_active(ಪೂರ್ಣಾಙ್ಕ active UNUSED)
{
}

    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_im_funcs(ಪೂರ್ಣಾಙ್ಕ copyID UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ವರ್ಣ *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ವರ್ಣ *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_rmdir(char_u *name)
{
    ಯದಿ (DeleteFile((STRPTR)name))
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#ಅನ್ತರ್ಭಾವ <pwd.h>
#ಅನ್ತರ್ಭಾವ <grp.h>

    ಸಂರಚನಾ passwd *
getpwuid(uid_t uid UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸಂರಚನಾ group *
getgrgid(gid_t gid UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    uid_t
getuid(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}

#endif // PROTO
