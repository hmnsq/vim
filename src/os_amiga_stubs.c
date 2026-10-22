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

#அந்தர்பாவ "vim.h"

#ifndef PROTO

#அந்தர்பாவ <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    முழு
im_get_status(வெறுமை)
{
    திரும்பு FALSE;
}

    வெறுமை
im_set_active(முழு active UNUSED)
{
}

    முழு
set_ref_in_im_funcs(முழு copyID UNUSED)
{
    திரும்பு 0;
}

    எழுத்து *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    திரும்பு வெற்று;
}

    எழுத்து *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    திரும்பு வெற்று;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    முழு
mch_rmdir(char_u *name)
{
    எனில் (DeleteFile((STRPTR)name))
	திரும்பு 0;
    திரும்பு -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#அந்தர்பாவ <pwd.h>
#அந்தர்பாவ <grp.h>

    கட்டமைப்பு passwd *
getpwuid(uid_t uid UNUSED)
{
    திரும்பு வெற்று;
}

    கட்டமைப்பு group *
getgrgid(gid_t gid UNUSED)
{
    திரும்பு வெற்று;
}

    uid_t
getuid(வெறுமை)
{
    திரும்பு 0;
}

#endif // PROTO
