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
#تعریف_کن VIM_TOSTR_(a)			#a
#تعریف_کن VIM_TOSTR(a)			VIM_TOSTR_(a)

// Values that change for a new release.
#تعریف_کن VIM_نسخه_MAJOR		9
#تعریف_کن VIM_نسخه_MINOR		2
#تعریف_کن VIM_نسخه_BUILD		286
#تعریف_کن VIM_نسخه_BUILD_BCD		0x11e
#تعریف_کن VIM_نسخه_DATE_ONLY		"2026 Feb 14"

// Values based on the above
#تعریف_کن VIM_نسخه_MAJOR_STR		VIM_TOSTR(VIM_نسخه_MAJOR)
#تعریف_کن VIM_نسخه_MINOR_STR		VIM_TOSTR(VIM_نسخه_MINOR)
#تعریف_کن VIM_نسخه_100	    (VIM_نسخه_MAJOR * 100 + VIM_نسخه_MINOR)

#تعریف_کن VIM_نسخه_BUILD_STR		VIM_TOSTR(VIM_نسخه_BUILD)
#اگر_تعریف_نشده VIM_نسخه_PATCHLEVEL
# define VIM_نسخه_PATCHLEVEL		0
#پایان_اگر

// Patchlevel with leading zeros
// For compatibility with the installer from "vim-win32-installer" and WinGet.
// For details see https://github.com/vim/vim-win32-installer/pull/277
// and https://github.com/vim/vim-win32-installer/pull/285
#إذا VIM_نسخه_PATCHLEVEL < 10
# define LEADZERO(x) 000 ## x
#وإلاإذا VIM_نسخه_PATCHLEVEL < 100
# define LEADZERO(x) 00 ## x
#وإلاإذا VIM_نسخه_PATCHLEVEL < 1000
# define LEADZERO(x) 0 ## x
#وإلا
# define LEADZERO(x) x
#پایان_اگر

#تعریف_کن VIM_نسخه_PATCHLEVEL_STR	VIM_TOSTR(LEADZERO(VIM_نسخه_PATCHLEVEL))
// Used by MacOS port; should be one of: development, alpha, beta, final
#تعریف_کن VIM_نسخه_RELEASE		final

/*
 * VIM_VERSION_NODOT is used for the runtime directory name.
 * VIM_VERSION_SHORT is copied into the swap file (max. length is 6 chars).
 * VIM_VERSION_MEDIUM is used for the startup-screen.
 * VIM_VERSION_LONG is used for the ":version" command and "Vim -h".
 */
#تعریف_کن VIM_نسخه_NODOT     "vim" VIM_نسخه_MAJOR_STR VIM_نسخه_MINOR_STR
#تعریف_کن VIM_نسخه_SHORT     VIM_نسخه_MAJOR_STR "." VIM_نسخه_MINOR_STR
#تعریف_کن VIM_نسخه_MEDIUM    VIM_نسخه_SHORT
#تعریف_کن VIM_نسخه_LONG_ONLY "VIM - Vi IMproved " VIM_نسخه_MEDIUM
#تعریف_کن VIM_نسخه_LONG_سر VIM_نسخه_LONG_ONLY " (" VIM_نسخه_DATE_ONLY
#تعریف_کن VIM_نسخه_LONG      VIM_نسخه_LONG_سر ")"
#تعریف_کن VIM_نسخه_LONG_DATE VIM_نسخه_LONG_سر ", compiled "
