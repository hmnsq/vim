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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO

#ਅਨ੍ਤਰ੍ਭਾਵ <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    ਪੂਰਨ
im_get_ਸਥਿਤੀ(ਖਾਲੀ)
{
    ਵਾਪਸ FALSE;
}

    ਖਾਲੀ
im_ਸੈੱਟ_ਸਰਗਰਮ(ਪੂਰਨ ਸਰਗਰਮ ਅਣਵਰਤਿਆ)
{
}

    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_im_funcs(ਪੂਰਨ copyID ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ 0;
}

    ਅੱਖਰ *
did_ਸੈੱਟ_imactivatefunc(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਨਿਰਾ;
}

    ਅੱਖਰ *
did_ਸੈੱਟ_imstatusfunc(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    ਪੂਰਨ
mch_rmdir(char_u *name)
{
    ਜੇ (DeleteFile((STRPTR)name))
	ਵਾਪਸ 0;
    ਵਾਪਸ -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <pwd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <grp.h>

    ਬਣਤਰ passwd *
getpwuid(uid_t uid ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਨਿਰਾ;
}

    ਬਣਤਰ ਸਮੂਹ *
getgrgid(gid_t gid ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਨਿਰਾ;
}

    uid_t
getuid(ਖਾਲੀ)
{
    ਵਾਪਸ 0;
}

#ਜੇ_ਅੰਤ // PROTO
