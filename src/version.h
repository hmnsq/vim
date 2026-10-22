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
#تعریف_کرو VIM_TOSTR_(a)			#a
#تعریف_کرو VIM_TOSTR(a)			VIM_TOSTR_(a)

// Values that change for a new release.
#تعریف_کرو VIM_نسخہ_MAJOR		9
#تعریف_کرو VIM_نسخہ_MINOR		2
#تعریف_کرو VIM_نسخہ_BUILD		286
#تعریف_کرو VIM_نسخہ_BUILD_BCD		0x11e
#تعریف_کرو VIM_نسخہ_DATE_ONLY		"2026 Feb 14"

// Values based on the above
#تعریف_کرو VIM_نسخہ_MAJOR_STR		VIM_TOSTR(VIM_نسخہ_MAJOR)
#تعریف_کرو VIM_نسخہ_MINOR_STR		VIM_TOSTR(VIM_نسخہ_MINOR)
#تعریف_کرو VIM_نسخہ_100	    (VIM_نسخہ_MAJOR * 100 + VIM_نسخہ_MINOR)

#تعریف_کرو VIM_نسخہ_BUILD_STR		VIM_TOSTR(VIM_نسخہ_BUILD)
#اگر_غیر_تعریف_شدہ VIM_نسخہ_PATCHLEVEL
# define VIM_نسخہ_PATCHLEVEL		0
#اگر_ختم

// Patchlevel with leading zeros
// For compatibility with the installer from "vim-win32-installer" and WinGet.
// For details see https://github.com/vim/vim-win32-installer/pull/277
// and https://github.com/vim/vim-win32-installer/pull/285
#اگر VIM_نسخہ_PATCHLEVEL < 10
# define LEADZERO(x) 000 ## x
#وگرنہ_اگر VIM_نسخہ_PATCHLEVEL < 100
# define LEADZERO(x) 00 ## x
#وگرنہ_اگر VIM_نسخہ_PATCHLEVEL < 1000
# define LEADZERO(x) 0 ## x
#ورنہ
# define LEADZERO(x) x
#اگر_ختم

#تعریف_کرو VIM_نسخہ_PATCHLEVEL_STR	VIM_TOSTR(LEADZERO(VIM_نسخہ_PATCHLEVEL))
// Used by MacOS port; should be one of: development, alpha, beta, final
#تعریف_کرو VIM_نسخہ_RELEASE		final

/*
 * VIM_VERSION_NODOT is used for the runtime directory name.
 * VIM_VERSION_SHORT is copied into the swap file (max. length is 6 chars).
 * VIM_VERSION_MEDIUM is used for the startup-screen.
 * VIM_VERSION_LONG is used for the ":version" command and "Vim -h".
 */
#تعریف_کرو VIM_نسخہ_NODOT     "vim" VIM_نسخہ_MAJOR_STR VIM_نسخہ_MINOR_STR
#تعریف_کرو VIM_نسخہ_SHORT     VIM_نسخہ_MAJOR_STR "." VIM_نسخہ_MINOR_STR
#تعریف_کرو VIM_نسخہ_MEDIUM    VIM_نسخہ_SHORT
#تعریف_کرو VIM_نسخہ_LONG_ONLY "VIM - Vi IMproved " VIM_نسخہ_MEDIUM
#تعریف_کرو VIM_نسخہ_LONG_سر VIM_نسخہ_LONG_ONLY " (" VIM_نسخہ_DATE_ONLY
#تعریف_کرو VIM_نسخہ_LONG      VIM_نسخہ_LONG_سر ")"
#تعریف_کرو VIM_نسخہ_LONG_DATE VIM_نسخہ_LONG_سر ", compiled "
