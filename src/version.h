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
#నిర్వచించు VIM_TOSTR_(a)			#a
#నిర్వచించు VIM_TOSTR(a)			VIM_TOSTR_(a)

// Values that change for a new release.
#నిర్వచించు VIM_సంస్కరణ_MAJOR		9
#నిర్వచించు VIM_సంస్కరణ_MINOR		2
#నిర్వచించు VIM_సంస్కరణ_BUILD		286
#నిర్వచించు VIM_సంస్కరణ_BUILD_BCD		0x11e
#నిర్వచించు VIM_సంస్కరణ_DATE_ONLY		"2026 Feb 14"

// Values based on the above
#నిర్వచించు VIM_సంస్కరణ_MAJOR_STR		VIM_TOSTR(VIM_సంస్కరణ_MAJOR)
#నిర్వచించు VIM_సంస్కరణ_MINOR_STR		VIM_TOSTR(VIM_సంస్కరణ_MINOR)
#నిర్వచించు VIM_సంస్కరణ_100	    (VIM_సంస్కరణ_MAJOR * 100 + VIM_సంస్కరణ_MINOR)

#నిర్వచించు VIM_సంస్కరణ_BUILD_STR		VIM_TOSTR(VIM_సంస్కరణ_BUILD)
#నిర్వచితం_కాకపోతే VIM_సంస్కరణ_PATCHLEVEL
# define VIM_సంస్కరణ_PATCHLEVEL		0
#అయితే_ముగింపు

// Patchlevel with leading zeros
// For compatibility with the installer from "vim-win32-installer" and WinGet.
// For details see https://github.com/vim/vim-win32-installer/pull/277
// and https://github.com/vim/vim-win32-installer/pull/285
#ఒకవేళ VIM_సంస్కరణ_PATCHLEVEL < 10
# define LEADZERO(x) 000 ## x
#లేదావేళ VIM_సంస్కరణ_PATCHLEVEL < 100
# define LEADZERO(x) 00 ## x
#లేదావేళ VIM_సంస్కరణ_PATCHLEVEL < 1000
# define LEADZERO(x) 0 ## x
#లేకపోతే
# define LEADZERO(x) x
#అయితే_ముగింపు

#నిర్వచించు VIM_సంస్కరణ_PATCHLEVEL_STR	VIM_TOSTR(LEADZERO(VIM_సంస్కరణ_PATCHLEVEL))
// Used by MacOS port; should be one of: development, alpha, beta, final
#నిర్వచించు VIM_సంస్కరణ_RELEASE		final

/*
 * VIM_VERSION_NODOT is used for the runtime directory name.
 * VIM_VERSION_SHORT is copied into the swap file (max. length is 6 chars).
 * VIM_VERSION_MEDIUM is used for the startup-screen.
 * VIM_VERSION_LONG is used for the ":version" command and "Vim -h".
 */
#నిర్వచించు VIM_సంస్కరణ_NODOT     "vim" VIM_సంస్కరణ_MAJOR_STR VIM_సంస్కరణ_MINOR_STR
#నిర్వచించు VIM_సంస్కరణ_SHORT     VIM_సంస్కరణ_MAJOR_STR "." VIM_సంస్కరణ_MINOR_STR
#నిర్వచించు VIM_సంస్కరణ_MEDIUM    VIM_సంస్కరణ_SHORT
#నిర్వచించు VIM_సంస్కరణ_LONG_ONLY "VIM - Vi IMproved " VIM_సంస్కరణ_MEDIUM
#నిర్వచించు VIM_సంస్కరణ_LONG_తల VIM_సంస్కరణ_LONG_ONLY " (" VIM_సంస్కరణ_DATE_ONLY
#నిర్వచించు VIM_సంస్కరణ_LONG      VIM_సంస్కరణ_LONG_తల ")"
#నిర్వచించు VIM_సంస్కరణ_LONG_DATE VIM_సంస్కరణ_LONG_తల ", compiled "
