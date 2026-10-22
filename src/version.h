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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_TOSTR_(a)			#a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_TOSTR(a)			VIM_TOSTR_(a)

// Values that change for a new release.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_MAJOR		9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_MINOR		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_BUILD		286
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_BUILD_BCD		0x11e
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_DATE_ONLY		"2026 Feb 14"

// Values based on the above
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_MAJOR_STR		VIM_TOSTR(VIM_ਸੰਸਕਰਣ_MAJOR)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_MINOR_STR		VIM_TOSTR(VIM_ਸੰਸਕਰਣ_MINOR)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_100	    (VIM_ਸੰਸਕਰਣ_MAJOR * 100 + VIM_ਸੰਸਕਰਣ_MINOR)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_BUILD_STR		VIM_TOSTR(VIM_ਸੰਸਕਰਣ_BUILD)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VIM_ਸੰਸਕਰਣ_PATCHLEVEL
# define VIM_ਸੰਸਕਰਣ_PATCHLEVEL		0
#ਜੇ_ਅੰਤ

// Patchlevel with leading zeros
// For compatibility with the installer from "vim-win32-installer" and WinGet.
// For details see https://github.com/vim/vim-win32-installer/pull/277
// and https://github.com/vim/vim-win32-installer/pull/285
#ਜੇ VIM_ਸੰਸਕਰਣ_PATCHLEVEL < 10
# define LEADZERO(x) 000 ## x
#ਜਾਂਜੇ VIM_ਸੰਸਕਰਣ_PATCHLEVEL < 100
# define LEADZERO(x) 00 ## x
#ਜਾਂਜੇ VIM_ਸੰਸਕਰਣ_PATCHLEVEL < 1000
# define LEADZERO(x) 0 ## x
#ਨਹੀਂਤਾਂ
# define LEADZERO(x) x
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_PATCHLEVEL_STR	VIM_TOSTR(LEADZERO(VIM_ਸੰਸਕਰਣ_PATCHLEVEL))
// Used by MacOS port; should be one of: development, alpha, beta, final
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_RELEASE		final

/*
 * VIM_VERSION_NODOT is used for the runtime directory name.
 * VIM_VERSION_SHORT is copied into the swap file (max. length is 6 chars).
 * VIM_VERSION_MEDIUM is used for the startup-screen.
 * VIM_VERSION_LONG is used for the ":version" command and "Vim -h".
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_NODOT     "vim" VIM_ਸੰਸਕਰਣ_MAJOR_STR VIM_ਸੰਸਕਰਣ_MINOR_STR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_SHORT     VIM_ਸੰਸਕਰਣ_MAJOR_STR "." VIM_ਸੰਸਕਰਣ_MINOR_STR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_MEDIUM    VIM_ਸੰਸਕਰਣ_SHORT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_LONG_ONLY "VIM - Vi IMproved " VIM_ਸੰਸਕਰਣ_MEDIUM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_LONG_ਸਿਰ VIM_ਸੰਸਕਰਣ_LONG_ONLY " (" VIM_ਸੰਸਕਰਣ_DATE_ONLY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_LONG      VIM_ਸੰਸਕਰਣ_LONG_ਸਿਰ ")"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਸੰਸਕਰਣ_LONG_DATE VIM_ਸੰਸਕਰਣ_LONG_ਸਿਰ ", compiled "
