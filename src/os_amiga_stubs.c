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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO

#ಅನ್ತರ್ಭಾವ <proto/dos.h>

/*
 * Input Method (IM) stubs.
 * These are referenced unconditionally from optiondefs.h function pointer
 * tables, but AmigaOS has no X11 input method framework.
 */
    ಪೂರ್ಣಾಂಕ
im_get_ಸ್ಥಿತಿ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಶೂನ್ಯ
im_ಗಣ_ಸಕ್ರಿಯ(ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ ಬಳಸದ)
{
}

    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_im_funcs(ಪೂರ್ಣಾಂಕ copyID ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ 0;
}

    ಅಕ್ಷರ *
did_ಗಣ_imactivatefunc(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಅಕ್ಷರ *
did_ಗಣ_imstatusfunc(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Remove a directory.
 * os_amiga.c provides most mch_* functions but mch_rmdir() was missing.
 * AmigaDOS DeleteFile() works for empty directories.
 */
    ಪೂರ್ಣಾಂಕ
mch_rmdir(char_u *name)
{
    ಆದರೆ (DeleteFile((STRPTR)name))
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * POSIX user/group database stubs.
 * AmigaOS is a single-user system with no passwd/group database.
 * The struct declarations exist in the NDK headers but the functions
 * are not implemented in libnix.
 */
#ಅನ್ತರ್ಭಾವ <pwd.h>
#ಅನ್ತರ್ಭಾವ <grp.h>

    ರಚನೆ passwd *
getpwuid(uid_t uid ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ರಚನೆ ಗುಂಪು *
getgrgid(gid_t gid ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    uid_t
getuid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ_ಅಂತ್ಯ // PROTO
