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

#അന്തര്ഭാവ "vim.h"

#ifndef PROTO

#അന്തര്ഭാവ <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    പൂർണ്ണസംഖ്യ
im_get_status(ശൂന്യം)
{
    തിരികെ FALSE;
}

    ശൂന്യം
im_set_active(പൂർണ്ണസംഖ്യ active UNUSED)
{
}

    പൂർണ്ണസംഖ്യ
set_ref_in_im_funcs(പൂർണ്ണസംഖ്യ copyID UNUSED)
{
    തിരികെ 0;
}

    അക്ഷരം *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    തിരികെ ശൂന്യ;
}

    അക്ഷരം *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    തിരികെ ശൂന്യ;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    പൂർണ്ണസംഖ്യ
mch_rmdir(char_u *name)
{
    എങ്കിൽ (DeleteFile((STRPTR)name))
	തിരികെ 0;
    തിരികെ -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#അന്തര്ഭാവ <pwd.h>
#അന്തര്ഭാവ <grp.h>

    ഘടന passwd *
getpwuid(uid_t uid UNUSED)
{
    തിരികെ ശൂന്യ;
}

    ഘടന group *
getgrgid(gid_t gid UNUSED)
{
    തിരികെ ശൂന്യ;
}

    uid_t
getuid(ശൂന്യം)
{
    തിരികെ 0;
}

#endif // PROTO
