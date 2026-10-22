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

#اگر_تعریف_نشده PROTO

#ضمن <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    صحيح
im_get_وضعیت(فراغ)
{
    أرجع FALSE;
}

    فراغ
im_مجموعه_فعال(صحيح فعال استفادهنشده)
{
}

    صحيح
مجموعه_ref_درون_im_funcs(صحيح copyID استفادهنشده)
{
    أرجع 0;
}

    حرف *
did_مجموعه_imactivatefunc(optset_T *آرگومانها استفادهنشده)
{
    أرجع عدم;
}

    حرف *
did_مجموعه_imstatusfunc(optset_T *آرگومانها استفادهنشده)
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
    إذا (DeleteFile((STRPTR)name))
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
getpwuid(uid_t uid استفادهنشده)
{
    أرجع عدم;
}

    بنية گروه *
getgrgid(gid_t gid استفادهنشده)
{
    أرجع عدم;
}

    uid_t
getuid(فراغ)
{
    أرجع 0;
}

#پایان_اگر // PROTO
