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

#اگر_غیر_تعریف_شدہ PROTO

#شامل <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    صحیح
im_get_حالت(خالی)
{
    واپس FALSE;
}

    خالی
im_سیٹ_فعال(صحیح فعال غیر_مستعمل)
{
}

    صحیح
سیٹ_ref_اندر_im_funcs(صحیح copyID غیر_مستعمل)
{
    واپس 0;
}

    حرف *
did_سیٹ_imactivatefunc(optset_T *دلائل غیر_مستعمل)
{
    واپس تہی;
}

    حرف *
did_سیٹ_imstatusfunc(optset_T *دلائل غیر_مستعمل)
{
    واپس تہی;
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
	واپس 0;
    واپس -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#شامل <pwd.h>
#شامل <grp.h>

    ساخت passwd *
getpwuid(uid_t uid غیر_مستعمل)
{
    واپس تہی;
}

    ساخت گروہ *
getgrgid(gid_t gid غیر_مستعمل)
{
    واپس تہی;
}

    uid_t
getuid(خالی)
{
    واپس 0;
}

#اگر_ختم // PROTO
