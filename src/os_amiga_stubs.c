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

#అన్తర్భావ "vim.h"

#నిర్వచితం_కాకపోతే PROTO

#అన్తర్భావ <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    పూర్ణాంకం
im_get_స్థితి(శూన్య)
{
    తిరిగి FALSE;
}

    శూన్య
im_సమితి_చురుకైన(పూర్ణాంకం చురుకైన వాడని)
{
}

    పూర్ణాంకం
సమితి_ref_లోపల_im_funcs(పూర్ణాంకం copyID వాడని)
{
    తిరిగి 0;
}

    అక్షరం *
did_సమితి_imactivatefunc(optset_T *వాదనలు వాడని)
{
    తిరిగి ఖాళీ;
}

    అక్షరం *
did_సమితి_imstatusfunc(optset_T *వాదనలు వాడని)
{
    తిరిగి ఖాళీ;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    పూర్ణాంకం
mch_rmdir(char_u *name)
{
    ఒకవేళ (DeleteFile((STRPTR)name))
	తిరిగి 0;
    తిరిగి -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#అన్తర్భావ <pwd.h>
#అన్తర్భావ <grp.h>

    నిర్మాణం passwd *
getpwuid(uid_t uid వాడని)
{
    తిరిగి ఖాళీ;
}

    నిర్మాణం సమూహం *
getgrgid(gid_t gid వాడని)
{
    తిరిగి ఖాళీ;
}

    uid_t
getuid(శూన్య)
{
    తిరిగి 0;
}

#అయితే_ముగింపు // PROTO
