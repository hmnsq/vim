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
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_TOSTR_(a)			#a
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_TOSTR(a)			VIM_TOSTR_(a)

// Values that change for a new release.
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_MAJOR		9
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_MINOR		2
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_BUILD		286
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_BUILD_BCD		0x11e
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_DATE_ONLY		"2026 Feb 14"

// Values based on the above
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_MAJOR_STR		VIM_TOSTR(VIM_ಆವೃತ್ತಿ_MAJOR)
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_MINOR_STR		VIM_TOSTR(VIM_ಆವೃತ್ತಿ_MINOR)
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_100	    (VIM_ಆವೃತ್ತಿ_MAJOR * 100 + VIM_ಆವೃತ್ತಿ_MINOR)

#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_BUILD_STR		VIM_TOSTR(VIM_ಆವೃತ್ತಿ_BUILD)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VIM_ಆವೃತ್ತಿ_PATCHLEVEL
# define VIM_ಆವೃತ್ತಿ_PATCHLEVEL		0
#ಆದರೆ_ಅಂತ್ಯ

// Patchlevel with leading zeros
// For compatibility with the installer from "vim-win32-installer" and WinGet.
// For details see https://github.com/vim/vim-win32-installer/pull/277
// and https://github.com/vim/vim-win32-installer/pull/285
#ಆದರೆ VIM_ಆವೃತ್ತಿ_PATCHLEVEL < 10
# define LEADZERO(x) 000 ## x
#ಅಥವಾಆದರೆ VIM_ಆವೃತ್ತಿ_PATCHLEVEL < 100
# define LEADZERO(x) 00 ## x
#ಅಥವಾಆದರೆ VIM_ಆವೃತ್ತಿ_PATCHLEVEL < 1000
# define LEADZERO(x) 0 ## x
#ಇಲ್ಲದಿದ್ದರೆ
# define LEADZERO(x) x
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_PATCHLEVEL_STR	VIM_TOSTR(LEADZERO(VIM_ಆವೃತ್ತಿ_PATCHLEVEL))
// Used by MacOS port; should be one of: development, alpha, beta, final
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_RELEASE		final

/*
 * VIM_VERSION_NODOT is used for the runtime directory name.
 * VIM_VERSION_SHORT is copied into the swap file (max. length is 6 chars).
 * VIM_VERSION_MEDIUM is used for the startup-screen.
 * VIM_VERSION_LONG is used for the ":version" command and "Vim -h".
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_NODOT     "vim" VIM_ಆವೃತ್ತಿ_MAJOR_STR VIM_ಆವೃತ್ತಿ_MINOR_STR
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_SHORT     VIM_ಆವೃತ್ತಿ_MAJOR_STR "." VIM_ಆವೃತ್ತಿ_MINOR_STR
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_MEDIUM    VIM_ಆವೃತ್ತಿ_SHORT
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_LONG_ONLY "VIM - Vi IMproved " VIM_ಆವೃತ್ತಿ_MEDIUM
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_LONG_ತಲೆ VIM_ಆವೃತ್ತಿ_LONG_ONLY " (" VIM_ಆವೃತ್ತಿ_DATE_ONLY
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_LONG      VIM_ಆವೃತ್ತಿ_LONG_ತಲೆ ")"
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಆವೃತ್ತಿ_LONG_DATE VIM_ಆವೃತ್ತಿ_LONG_ತಲೆ ", compiled "
