/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Define the version number, name, etc.
 * The patchlevel is in included_patches[], in version.c.
 */

// Trick to turn a number into a string.
#وصف_ڪريو VIM_TOSTR_(a)			#a
#وصف_ڪريو VIM_TOSTR(a)			VIM_TOSTR_(a)

// Values that change for a new release.
#وصف_ڪريو VIM_نسخو_MAJOR		9
#وصف_ڪريو VIM_نسخو_MINOR		2
#وصف_ڪريو VIM_نسخو_BUILD		286
#وصف_ڪريو VIM_نسخو_BUILD_BCD		0x11e
#وصف_ڪريو VIM_نسخو_DATE_ONLY		"2026 Feb 14"

// Values based on the above
#وصف_ڪريو VIM_نسخو_MAJOR_STR		VIM_TOSTR(VIM_نسخو_MAJOR)
#وصف_ڪريو VIM_نسخو_MINOR_STR		VIM_TOSTR(VIM_نسخو_MINOR)
#وصف_ڪريو VIM_نسخو_100	    (VIM_نسخو_MAJOR * 100 + VIM_نسخو_MINOR)

#وصف_ڪريو VIM_نسخو_BUILD_STR		VIM_TOSTR(VIM_نسخو_BUILD)
#جيڪڏهن_وصف_نه_ٿيل VIM_نسخو_PATCHLEVEL
# define VIM_نسخو_PATCHLEVEL		0
#جيڪڏهن_پورو

// Patchlevel with leading zeros
// For compatibility with the installer from "vim-win32-installer" and WinGet.
// For details see https://github.com/vim/vim-win32-installer/pull/277
// and https://github.com/vim/vim-win32-installer/pull/285
#إذا VIM_نسخو_PATCHLEVEL < 10
# define LEADZERO(x) 000 ## x
#وإلاإذا VIM_نسخو_PATCHLEVEL < 100
# define LEADZERO(x) 00 ## x
#وإلاإذا VIM_نسخو_PATCHLEVEL < 1000
# define LEADZERO(x) 0 ## x
#وإلا
# define LEADZERO(x) x
#جيڪڏهن_پورو

#وصف_ڪريو VIM_نسخو_PATCHLEVEL_STR	VIM_TOSTR(LEADZERO(VIM_نسخو_PATCHLEVEL))
// Used by MacOS port; should be one of: development, alpha, beta, final
#وصف_ڪريو VIM_نسخو_RELEASE		final

/*
 * VIM_VERSION_NODOT is used for the runtime directory name.
 * VIM_VERSION_SHORT is copied into the swap file (max. length is 6 chars).
 * VIM_VERSION_MEDIUM is used for the startup-screen.
 * VIM_VERSION_LONG is used for the ":version" command and "Vim -h".
 */
#وصف_ڪريو VIM_نسخو_NODOT     "vim" VIM_نسخو_MAJOR_STR VIM_نسخو_MINOR_STR
#وصف_ڪريو VIM_نسخو_SHORT     VIM_نسخو_MAJOR_STR "." VIM_نسخو_MINOR_STR
#وصف_ڪريو VIM_نسخو_MEDIUM    VIM_نسخو_SHORT
#وصف_ڪريو VIM_نسخو_LONG_ONLY "VIM - Vi IMproved " VIM_نسخو_MEDIUM
#وصف_ڪريو VIM_نسخو_LONG_سر VIM_نسخو_LONG_ONLY " (" VIM_نسخو_DATE_ONLY
#وصف_ڪريو VIM_نسخو_LONG      VIM_نسخو_LONG_سر ")"
#وصف_ڪريو VIM_نسخو_LONG_DATE VIM_نسخو_LONG_سر ", compiled "
