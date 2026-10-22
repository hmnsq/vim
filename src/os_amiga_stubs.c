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

#अन्तर्भाव "vim.h"

#ifndef PROTO

#अन्तर्भाव <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    पूर्णाङ्क
im_get_status(शून्य)
{
    प्रत्यागम FALSE;
}

    शून्य
im_set_active(पूर्णाङ्क active UNUSED)
{
}

    पूर्णाङ्क
set_ref_in_im_funcs(पूर्णाङ्क copyID UNUSED)
{
    प्रत्यागम 0;
}

    वर्ण *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    प्रत्यागम रिक्त;
}

    वर्ण *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    प्रत्यागम रिक्त;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    पूर्णाङ्क
mch_rmdir(char_u *name)
{
    जइ (DeleteFile((STRPTR)name))
	प्रत्यागम 0;
    प्रत्यागम -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#अन्तर्भाव <pwd.h>
#अन्तर्भाव <grp.h>

    संरचना passwd *
getpwuid(uid_t uid UNUSED)
{
    प्रत्यागम रिक्त;
}

    संरचना group *
getgrgid(gid_t gid UNUSED)
{
    प्रत्यागम रिक्त;
}

    uid_t
getuid(शून्य)
{
    प्रत्यागम 0;
}

#endif // PROTO
