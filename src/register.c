/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * register.c: functions for managing registers
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * Registers:
 *	0 = unnamed register, for normal yanks and puts
 *   1..9 = registers '1' to '9', for deletes
 * 10..35 = registers 'a' to 'z' ('A' to 'Z' for appending)
 *     36 = delete register '-'
 *     37 = Selection register '*'. Only if FEAT_CLIPBOARD defined
 *     38 = Clipboard register '+'. Only if FEAT_CLIPBOARD and FEAT_X11
 *                                  or FEAT_WAYLAND_CLIPBOARD defined
 */
ಸ್ಥಿರ yankreg_T	y_regs[ಅಂಕೆ_REGISTERS];

ಸ್ಥಿರ yankreg_T	*y_current;	    // ptr to current yankreg
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		y_append;	    // TRUE when appending
ಸ್ಥಿರ yankreg_T	*y_ಹಿಂದಿನ = ಖಾಲಿ; // ptr to last written yankreg

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	stuff_yank(ಪೂರ್ಣಾಂಕ, char_u *);
ಸ್ಥಿರ ಶೂನ್ಯ	put_reedit_ಒಳ_typebuf(ಪೂರ್ಣಾಂಕ silent);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	put_ಒಳ_typebuf(char_u *s, ಪೂರ್ಣಾಂಕ esc, ಪೂರ್ಣಾಂಕ colon, ಪೂರ್ಣಾಂಕ silent);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	yank_copy_ಸಾಲು(ರಚನೆ ಖಂಡ_def *bd, ದೀರ್ಘ y_idx, ಪೂರ್ಣಾಂಕ exclude_trailing_space);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
ಸ್ಥಿರ ಶೂನ್ಯ	copy_yank_reg(yankreg_T *reg);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	dis_ಸಂದೇಶ(char_u *p, ಪೂರ್ಣಾಂಕ skip_esc);

#ಆದರೆ defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ y_regs;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_ರಿಜಿಸ್ಟರ್(ಪೂರ್ಣಾಂಕ reg)
{
    ಹಿಂತಿರುಗಿ &y_regs[reg];
}
#ಆದರೆ_ಅಂತ್ಯ

    yankreg_T *
get_y_current(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ y_current;
}

    yankreg_T *
get_y_ಹಿಂದಿನ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ y_ಹಿಂದಿನ;
}

    ಶೂನ್ಯ
ಗಣ_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    ಶೂನ್ಯ
ಗಣ_y_ಹಿಂದಿನ(yankreg_T *yreg)
{
    y_ಹಿಂದಿನ = yreg;
}

    ಶೂನ್ಯ
reset_y_append(ಶೂನ್ಯ)
{
    y_append = FALSE;
}


#ಆದರೆ defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
ಸ್ಥಿರ char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು = ಖಾಲಿ;
ಸ್ಥಿರ exarg_T	*ಅಭಿವ್ಯಕ್ತಿ_eap = ಖಾಲಿ;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
get_ಅಭಿವ್ಯಕ್ತಿ_ರಿಜಿಸ್ಟರ್(ಶೂನ್ಯ)
{
    char_u	*new_ಸಾಲು;

    new_ಸಾಲು = getcmdline('=', 0L, 0, 0);
    ಆದರೆ (new_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ NUL;
    ಆದರೆ (*new_ಸಾಲು == NUL)	// use previous line
	vim_free(new_ಸಾಲು);
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(new_ಸಾಲು, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    ಶೂನ್ಯ
ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(char_u *new_ಸಾಲು, exarg_T *eap)
{
    vim_free(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು);
    ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು = new_ಸಾಲು;
    ಅಭಿವ್ಯಕ್ತಿ_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(ಶೂನ್ಯ)
{
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_copy;
    char_u	*rv;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	nested = 0;

    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    ಅಭಿವ್ಯಕ್ತಿ_copy = vim_strsave(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು);
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_copy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    ಆದರೆ (nested >= 10)
	ಹಿಂತಿರುಗಿ ಅಭಿವ್ಯಕ್ತಿ_copy;

    ++nested;
    rv = eval_ಗಮ್ಯ_string_eap(ಅಭಿವ್ಯಕ್ತಿ_copy, TRUE, ಅಭಿವ್ಯಕ್ತಿ_eap, FALSE);
    --nested;
    vim_free(ಅಭಿವ್ಯಕ್ತಿ_copy);
    ಹಿಂತಿರುಗಿ rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ಸ್ಥಿರ char_u *
get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು_src(ಶೂನ್ಯ)
{
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ vim_strsave(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು);
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    ಪೂರ್ಣಾಂಕ
valid_yank_reg(
    ಪೂರ್ಣಾಂಕ	    regname,
    ಪೂರ್ಣಾಂಕ	    writing)	    // if TRUE check for writable registers
{
    ಆದರೆ (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
				    "/#.%:="
#ಇಲ್ಲದಿದ್ದರೆ
				    "/#.%:"
#ಆದರೆ_ಅಂತ್ಯ
					, regname) != ಖಾಲಿ)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#ಆದರೆ defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#ಅಥವಾಆದರೆ defined(FEAT_CLIPBOARD_PROVIDER)
	    || ( // If -clipboard, then these registers only exist when
		 // clipmethod is set to provider.
	     clipmethod == CLIPMETHOD_PROVIDER && (
		 regname == '*'
		 || regname == '+'
	    ))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
	    || (!writing && regname == '~')
#ಆದರೆ_ಅಂತ್ಯ
							)
	ಹಿಂತಿರುಗಿ TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	ಸಂದೇಶ_warn_missing_clipboard();
	ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Set y_current and y_append, according to the value of "regname".
 * Cannot handle the '_' register.
 * Must only be called with a valid register name!
 *
 * If regname is 0 and writing, use register 0
 * If regname is 0 and reading, use previous register
 *
 * Return TRUE when the register should be inserted literally (selection or
 * clipboard).
 */
    ಪೂರ್ಣಾಂಕ
get_yank_ರಿಜಿಸ್ಟರ್(ಪೂರ್ಣಾಂಕ regname, ಪೂರ್ಣಾಂಕ writing)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    ret = FALSE;

    y_append = FALSE;
    ಆದರೆ ((regname == 0 || regname == '"') && !writing && y_ಹಿಂದಿನ != ಖಾಲಿ)
    {
	y_current = y_ಹಿಂದಿನ;
	ಹಿಂತಿರುಗಿ ret;
    }
    i = regname;
    ಆದರೆ (VIM_ISDIGIT(i))
	i -= '0';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ASCII_ISLOWER(i))
	i = Char_Ord_ಕೆಳಗಿನ(i) + 10;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ASCII_ISUPPER(i))
    {
	i = Char_Ord_ಮೇಲೆ(i) + 10;
	y_append = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regname == '-')
	i = DELETION_ರಿಜಿಸ್ಟರ್;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ && regname == '*')
    {
	i = ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clip_plus.ಲಭ್ಯ && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	ಆದರೆ (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_ರಿಜಿಸ್ಟರ್;
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    i = PLUS_ರಿಜಿಸ್ಟರ್;
	ret = TRUE;
    }
#ಅಥವಾಆದರೆ defined(FEAT_CLIPBOARD_PROVIDER)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regname == '*')
    {
	i = ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್;
	ret = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regname == '+')
    {
	i = REAL_PLUS_ರಿಜಿಸ್ಟರ್;
	ret = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!writing && regname == '~')
	i = TILDE_ರಿಜಿಸ್ಟರ್;
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    ಆದರೆ (writing)	// remember the register we write into for do_put()
	y_ಹಿಂದಿನ = y_current;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    ಶೂನ್ಯ *
get_ರಿಜಿಸ್ಟರ್(
    ಪೂರ್ಣಾಂಕ		name,
    ಪೂರ್ಣಾಂಕ		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    ಪೂರ್ಣಾಂಕ		i;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	ಆದರೆ (name == '*' && clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ)
	{
	    ಆದರೆ (clip_isautosel_ನಕ್ಷತ್ರ())
		clip_update_selection(&clip_ನಕ್ಷತ್ರ);
	    may_get_selection(name);
	}
	ಆದರೆ (name == '+' && clip_plus.ಲಭ್ಯ)
	{
	    ಆದರೆ (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    get_yank_ರಿಜಿಸ್ಟರ್(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    ಆದರೆ (reg == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)ಖಾಲಿ;

    *reg = *y_current;
    ಆದರೆ (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	ಆದರೆ (reg->y_ಗಾತ್ರ == 0 || y_current->y_array == ಖಾಲಿ)
	    reg->y_array = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    reg->y_array = ALLOC_MULT(string_T, reg->y_ಗಾತ್ರ);
	ಆದರೆ (reg->y_array != ಖಾಲಿ)
	{
	    ಗಾಗಿ (i = 0; i < reg->y_ಗಾತ್ರ; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].ಉದ್ದಳತೆ);
		ಆದರೆ (reg->y_array[i].string == ಖಾಲಿ)
		{
		    // The allocation failed so clean up and exit
		    ಆಗ (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)ಖಾಲಿ;
		}

		reg->y_array[i].ಉದ್ದಳತೆ = y_current->y_array[i].ಉದ್ದಳತೆ;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	y_current->y_array = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    ಶೂನ್ಯ
put_ರಿಜಿಸ್ಟರ್(ಪೂರ್ಣಾಂಕ name, ಶೂನ್ಯ *reg)
{
    get_yank_ರಿಜಿಸ್ಟರ್(name, 0);
    free_yank_all();
    *y_current = *(yankreg_T *)reg;
    vim_free(reg);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    // Send text written to clipboard register to the clipboard.
    may_ಗಣ_selection();
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_CLIPBOARD)
    ಶೂನ್ಯ
free_ರಿಜಿಸ್ಟರ್(ಶೂನ್ಯ *reg)
{
    yankreg_T tmp;

    tmp = *y_current;
    *y_current = *(yankreg_T *)reg;
    free_yank_all();
    vim_free(reg);
    *y_current = tmp;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * return TRUE if the current yank register has type MLINE
 */
    ಪೂರ್ಣಾಂಕ
yank_ರಿಜಿಸ್ಟರ್_mline(ಪೂರ್ಣಾಂಕ regname)
{
    ಆದರೆ (regname != 0 && !valid_yank_reg(regname, FALSE))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (regname == '_')		// black hole is always empty
	ಹಿಂತಿರುಗಿ FALSE;
    get_yank_ರಿಜಿಸ್ಟರ್(regname, FALSE);
    ಹಿಂತಿರುಗಿ (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
do_ದಾಖಲೆ(ಪೂರ್ಣಾಂಕ c)
{
    char_u	    *p;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    regname;
    yankreg_T	    *old_y_ಹಿಂದಿನ, *old_y_current;
    ಪೂರ್ಣಾಂಕ		    retval;

    ಆದರೆ (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	ಆದರೆ (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	ಸಂದೇಶ("");
	p = get_recorded();
	ಆದರೆ (p == ಖಾಲಿ)
	    retval = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Remove escaping for CSI and K_SPECIAL in multi-byte chars.
	    vim_unescape_csi(p);

	    // We don't want to change the default register here, so save and
	    // restore the current register name.
	    old_y_ಹಿಂದಿನ = y_ಹಿಂದಿನ;
	    old_y_current = y_current;

	    retval = stuff_yank(regname, p);

	    y_ಹಿಂದಿನ = old_y_ಹಿಂದಿನ;
	    y_current = old_y_current;
	}
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
stuff_yank(ಪೂರ್ಣಾಂಕ regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    ಆದರೆ (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	ಹಿಂತಿರುಗಿ OK;
    }

    plen = STRLEN(p);
    get_yank_ರಿಜಿಸ್ಟರ್(regname, TRUE);
    ಆದರೆ (y_append && y_current->y_array != ಖಾಲಿ)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_ಗಾತ್ರ - 1]);
	tmplen = pp->ಉದ್ದಳತೆ + plen;
	tmp = alloc(tmplen + 1);
	ಆದರೆ (tmp == ಖಾಲಿ)
	{
	    vim_free(p);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->ಉದ್ದಳತೆ, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->ಉದ್ದಳತೆ = tmplen;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	free_yank_all();
	ಆದರೆ ((y_current->y_array = ALLOC_ONE(string_T)) == ಖಾಲಿ)
	{
	    vim_free(p);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].ಉದ್ದಳತೆ = plen;
	y_current->y_ಗಾತ್ರ = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
	y_current->y_ಸಮಯ_ಗಣ = vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Last executed register (@ command)
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ execreg_lastc = NUL;

#ಆದರೆ defined(FEAT_VIMINFO)
    ಪೂರ್ಣಾಂಕ
get_execreg_lastc(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ execreg_lastc;
}

    ಶೂನ್ಯ
ಗಣ_execreg_lastc(ಪೂರ್ಣಾಂಕ lastc)
{
    execreg_lastc = lastc;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * When executing a register as a series of ex-commands, if the
 * line-continuation character is used for a line, then join it with one or
 * more previous lines. Note that lines are processed backwards starting from
 * the last line in the register.
 *
 * Arguments:
 *   lines - list of lines in the register
 *   idx - index of the line starting with \ or "\. Join this line with all the
 *	   immediate predecessor lines that start with a \ and the first line
 *	   that doesn't start with a \. Lines that start with a comment "\
 *	   character are ignored.
 *
 * Returns the concatenated line. The index of the line that should be
 * processed next is returned in idx.
 */
    ಸ್ಥಿರ char_u *
execreg_ಸಾಲು_continuation(string_T *ಸಾಲುಗಳು, ದೀರ್ಘ *idx)
{
    garray_T	ga;
    ದೀರ್ಘ	cmd_ಆರಂಭ = *idx;
    ದೀರ್ಘ	cmd_ಕೊನೆ = *idx;
    string_T	*tmp;
    ಪೂರ್ಣಾಂಕ		j;
    char_u	*str;

    ga_init2(&ga, ಗಾತ್ರ(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    ಆಗ (--cmd_ಆರಂಭ > 0)
    {
	char_u	*p;

	p = skipwhite(ಸಾಲುಗಳು[cmd_ಆರಂಭ].string);
	ಆದರೆ (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    ನಿಲ್ಲಿಸು;
    }

    // join all the lines
    tmp = &ಸಾಲುಗಳು[cmd_ಆರಂಭ];
    ga_concat_len(&ga, tmp->string, tmp->ಉದ್ದಳತೆ);
    ಗಾಗಿ (j = cmd_ಆರಂಭ + 1; j <= cmd_ಕೊನೆ; j++)
    {
	char_u	*p;

	tmp = &ಸಾಲುಗಳು[j];
	p = skipwhite(tmp->string);
	ಆದರೆ (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    ಆದರೆ (ga.ga_len > 400)
	    {
		ಆದರೆ (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		ಇಲ್ಲದಿದ್ದರೆ
		    ga.ga_growsize = ga.ga_len;
	    }
	    p++;
	    ga_concat_len(&ga, p, (tmp->string + tmp->ಉದ್ದಳತೆ) - p);
	}
    }
    ga_append(&ga, NUL);
    str = vim_strnsave(ga.ga_ದತ್ತಾಂಶ, ga.ga_len);
    ga_clear(&ga);

    *idx = cmd_ಆರಂಭ;
    ಹಿಂತಿರುಗಿ str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
do_execreg(
    ಪೂರ್ಣಾಂಕ	    regname,
    ಪೂರ್ಣಾಂಕ	    colon,		// insert ':' before each line
    ಪೂರ್ಣಾಂಕ	    addcr,		// always add '\n' to end of line
    ಪೂರ್ಣಾಂಕ	    silent)		// set "silent" flag in typeahead buffer
{
    ದೀರ್ಘ	i;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		retval = OK;
    ಪೂರ್ಣಾಂಕ		remap;

    // repeat previous one
    ಆದರೆ (regname == '@')
    {
	ಆದರೆ (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_ಬಳಸಿದ_ರಿಜಿಸ್ಟರ್));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    ಆದರೆ (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    execreg_lastc = regname;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#ಆದರೆ_ಅಂತ್ಯ

    // black hole: don't stuff anything
    ಆದರೆ (regname == '_')
	ಹಿಂತಿರುಗಿ OK;

    // use last command line
    ಆದರೆ (regname == ':')
    {
	ಆದರೆ (ಕೊನೆಯ_cmdline == ಖಾಲಿ)
	{
	    emsg(_(e_no_ಹಿಂದಿನ_command_ಸಾಲು));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	// don't keep the cmdline containing @:
	VIM_CLEAR(new_ಕೊನೆಯ_cmdline);
	// Escape all control characters with a CTRL-V
	p = vim_strsave_escaped_ext(ಕೊನೆಯ_cmdline,
		    (char_u *)"\001\002\003\004\005\006\007"
			  "\010\011\012\013\014\015\016\017"
			  "\020\021\022\023\024\025\026\027"
			  "\030\031\032\033\034\035\036\037",
		    Ctrl_V, FALSE);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_ಒಳ_typebuf(p + 5, TRUE, TRUE, silent);
	    ಇಲ್ಲದಿದ್ದರೆ
		retval = put_ಒಳ_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regname == '=')
    {
	p = get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು();
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	retval = put_ಒಳ_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regname == '.')		// use last inserted text
    {
	p = get_ಕೊನೆಯ_ಸೇರಿಸು_save();
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    emsg(_(e_no_inserted_ಪಠ್ಯ_yet));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	retval = put_ಒಳ_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	get_yank_ರಿಜಿಸ್ಟರ್(regname, FALSE);
	ಆದರೆ (y_current->y_array == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_ಒಳ_typebuf(silent);
	ಗಾಗಿ (i = y_current->y_ಗಾತ್ರ; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    ಪೂರ್ಣಾಂಕ	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    ಆದರೆ (y_current->y_type == MLINE || i < y_current->y_ಗಾತ್ರ - 1
								     || addcr)
	    {
		ಆದರೆ (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    ಆದರೆ (colon && i > 0)
	    {
		p = skipwhite(str);
		ಆದರೆ (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_ಸಾಲು_continuation(y_current->y_array, &i);
		    ಆದರೆ (str == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    ಆದರೆ (free_str)
		vim_free(str);
	    ಆದರೆ (escaped == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    ಆದರೆ (retval == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_ಕೊನೆ_reg_executing = FALSE;
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
put_reedit_ಒಳ_typebuf(ಪೂರ್ಣಾಂಕ silent)
{
    ಆದರೆ (restart_edit == NUL)
	ಹಿಂತಿರುಗಿ;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    ಆದರೆ (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    ಆದರೆ (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_ಒಳ_typebuf(
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		esc,
    ಪೂರ್ಣಾಂಕ		colon,	    // add ':' before the line
    ಪೂರ್ಣಾಂಕ		silent)
{
    ಪೂರ್ಣಾಂಕ		retval = OK;

    put_reedit_ಒಳ_typebuf(silent);
    ಆದರೆ (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    ಆದರೆ (retval == OK)
    {
	char_u	*p;

	ಆದರೆ (esc)
	    p = vim_strsave_escape_csi(s);
	ಇಲ್ಲದಿದ್ದರೆ
	    p = s;
	ಆದರೆ (p == ಖಾಲಿ)
	    retval = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	ಆದರೆ (esc)
	    vim_free(p);
    }
    ಆದರೆ (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
ಸೇರಿಸು_reg(
    ಪೂರ್ಣಾಂಕ		regname,
    ಪೂರ್ಣಾಂಕ		literally_arg)	// insert literally, not as if typed
{
    ದೀರ್ಘ	i;
    ಪೂರ್ಣಾಂಕ		retval = OK;
    char_u	*arg;
    ಪೂರ್ಣಾಂಕ		allocated;
    ಪೂರ್ಣಾಂಕ		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    ಆದರೆ (got_int)
	ಹಿಂತಿರುಗಿ FAIL;

    // check for valid regname
    ಆದರೆ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ಹಿಂತಿರುಗಿ FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	ಆದರೆ (arg == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	stuffescaped(arg, literally);
	ಆದರೆ (allocated)
	    vim_free(arg);
    }
    ಇಲ್ಲದಿದ್ದರೆ				// name or number register
    {
	ಆದರೆ (get_yank_ರಿಜಿಸ್ಟರ್(regname, FALSE))
	    literally = TRUE;
	ಆದರೆ (y_current->y_array == ಖಾಲಿ)
	    retval = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (i = 0; i < y_current->y_ಗಾತ್ರ; ++i)
	    {
		ಆದರೆ (regname == '-' && y_current->y_type == MCHAR)
		{
		    ಪೂರ್ಣಾಂಕ dir = BACKWARD;
		    ಆದರೆ ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			ಆದರೆ (u_save_cursor() == FAIL)
			    ಹಿಂತಿರುಗಿ FAIL;
			del_chars((ದೀರ್ಘ)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			ಆದರೆ (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    Append_Char_ಗಮ್ಯ_Redobuff(Ctrl_R);
		    Append_Char_ಗಮ್ಯ_Redobuff(regname);
		    do_put(regname, ಖಾಲಿ, dir, 1L, PUT_CURSEND);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    ಆದರೆ (y_current->y_type == MLINE || i < y_current->y_ಗಾತ್ರ - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    ಪೂರ್ಣಾಂಕ
get_spec_reg(
    ಪೂರ್ಣಾಂಕ		regname,
    char_u	**argp,
    ಪೂರ್ಣಾಂಕ		*allocated,	// return: TRUE when value was allocated
    ಪೂರ್ಣಾಂಕ		errmsg)		// give error message when failing
{
    ಪೂರ್ಣಾಂಕ		cnt;

    *argp = ಖಾಲಿ;
    *allocated = FALSE;
    ಆಯ್ಕೆ (regname)
    {
	ಸ್ಥಿತಿ '%':		// file name
	    ಆದರೆ (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    ಹಿಂತಿರುಗಿ TRUE;

	ಸ್ಥಿತಿ '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    ಹಿಂತಿರುಗಿ TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಸ್ಥಿತಿ '=':		// result of expression
	    *argp = get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು();
	    *allocated = TRUE;
	    ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ ':':		// last command line
	    ಆದರೆ (ಕೊನೆಯ_cmdline == ಖಾಲಿ && errmsg)
		emsg(_(e_no_ಹಿಂದಿನ_command_ಸಾಲು));
	    *argp = ಕೊನೆಯ_cmdline;
	    ಹಿಂತಿರುಗಿ TRUE;

	ಸ್ಥಿತಿ '/':		// last search-pattern
	    ಆದರೆ (ಕೊನೆಯ_ಹುಡುಕು_pat() == ಖಾಲಿ && errmsg)
		emsg(_(e_no_ಹಿಂದಿನ_regular_ಅಭಿವ್ಯಕ್ತಿ));
	    *argp = ಕೊನೆಯ_ಹುಡುಕು_pat();
	    ಹಿಂತಿರುಗಿ TRUE;

	ಸ್ಥಿತಿ '.':		// last inserted text
	    *argp = get_ಕೊನೆಯ_ಸೇರಿಸು_save();
	    *allocated = TRUE;
	    ಆದರೆ (*argp == ಖಾಲಿ && errmsg)
		emsg(_(e_no_inserted_ಪಠ್ಯ_yet));
	    ಹಿಂತಿರುಗಿ TRUE;

	ಸ್ಥಿತಿ Ctrl_F:		// Filename under cursor
	ಸ್ಥಿತಿ Ctrl_P:		// Path under cursor, expand via "path"
	    ಆದರೆ (!errmsg)
		ಹಿಂತಿರುಗಿ FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, ಖಾಲಿ);
	    *allocated = TRUE;
	    ಹಿಂತಿರುಗಿ TRUE;

	ಸ್ಥಿತಿ Ctrl_W:		// word under cursor
	ಸ್ಥಿತಿ Ctrl_A:		// WORD (mnemonic All) under cursor
	    ಆದರೆ (!errmsg)
		ಹಿಂತಿರುಗಿ FALSE;
	    cnt = ಕಂಡುಹಿಡಿ_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (ಕಂಡುಹಿಡಿ_IDENT|ಕಂಡುಹಿಡಿ_STRING) : ಕಂಡುಹಿಡಿ_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : ಖಾಲಿ;
	    *allocated = TRUE;
	    ಹಿಂತಿರುಗಿ TRUE;

	ಸ್ಥಿತಿ Ctrl_L:		// Line under cursor
	    ಆದರೆ (!errmsg)
		ಹಿಂತಿರುಗಿ FALSE;

	    *argp = ml_get_buf(curwin->w_ಬಫರ್,
			curwin->w_cursor.lnum, FALSE);
	    ಹಿಂತಿರುಗಿ TRUE;

	ಸ್ಥಿತಿ '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    ಹಿಂತಿರುಗಿ TRUE;
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Paste a yank register into the command line.
 * Only for non-special registers.
 * Used by CTRL-R command in command-line mode
 * insert_reg() can't be used here, because special characters from the
 * register contents will be interpreted as commands.
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
cmdline_paste_reg(
    ಪೂರ್ಣಾಂಕ regname,
    ಪೂರ್ಣಾಂಕ literally_arg,	// Insert text literally instead of "as typed"
    ಪೂರ್ಣಾಂಕ remcr)		// don't add CR characters
{
    ದೀರ್ಘ	i;
    ಪೂರ್ಣಾಂಕ		literally = literally_arg;

    ಆದರೆ (get_yank_ರಿಜಿಸ್ಟರ್(regname, FALSE))
	literally = TRUE;
    ಆದರೆ (y_current->y_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಗಾಗಿ (i = 0; i < y_current->y_ಗಾತ್ರ; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	ಆದರೆ ((y_current->y_type == MLINE || i < y_current->y_ಗಾತ್ರ - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	ಆದರೆ (got_int)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    ಶೂನ್ಯ
ಸರಿಸು_delete_registers(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    ಗಾಗಿ (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    ಆದರೆ (!y_append)
	y_ಹಿಂದಿನ = y_current;
    y_regs[1].y_array = ಖಾಲಿ;		// set register one to empty
}

#ಆದರೆ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_regtype_ಗಮ್ಯ_dict(ಪೂರ್ಣಾಂಕ regname, dict_T *dict, char_u *buf, ಪೂರ್ಣಾಂಕ bufsize)
{
    size_t  buflen;
    ದೀರ್ಘ    reglen;
    // Register type
    ಆಯ್ಕೆ (get_reg_type(regname, &reglen))
    {
	ಸ್ಥಿತಿ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MBLOCK:
	    buflen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    buf[0] = NUL;
	    buflen = 0;
	    ನಿಲ್ಲಿಸು;
    }
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(dict, "regtype", buf, (ಪೂರ್ಣಾಂಕ)buflen);
}

    ಶೂನ್ಯ
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    ಪೂರ್ಣಾಂಕ		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // yanked text contents
    ಗಾಗಿ (n = 0; n < reg->y_ಗಾತ್ರ; n++)
	list_append_string(list, reg->y_array[n].string, (ಪೂರ್ಣಾಂಕ)reg->y_array[n].ಉದ್ದಳತೆ);
    list->lv_ಬೀಗ = VAR_FIXED;
    ಆದರೆ (dict_ಕೂಡಿಸು_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(v_event, "regname", buf, (ಪೂರ್ಣಾಂಕ)buflen);

    // motion type: inclusive or exclusive
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(v_event, "operator", buf, (ಪೂರ್ಣಾಂಕ)buflen);

    ಕೂಡಿಸು_regtype_ಗಮ್ಯ_dict(oap->regname, v_event, buf, ಗಾತ್ರ(buf));

    // selection type - visual or not
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_ಗಣ_ವಸ್ತುಗಳು_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
put_do_autocmd(
	ಪೂರ್ಣಾಂಕ	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *ಸೇರಿಸು,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	ಪೂರ್ಣಾಂಕ	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    ಸ್ಥಿರ bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    ಪೂರ್ಣಾಂಕ		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ಆದರೆ (recursive || regname == '_')
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (regname != '.' && ಸೇರಿಸು == ಖಾಲಿ
	    && reg == ಖಾಲಿ)
	// Can happen when pasting text in normal mode in a terminal buffer
	ಹಿಂತಿರುಗಿ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ಆದರೆ (regname == '.')
    {
	ಆದರೆ (ಕೊನೆಯ_ಸೇರಿಸು_ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, ಕೊನೆಯ_ಸೇರಿಸು_ga.ga_ದತ್ತಾಂಶ,
		    (ಪೂರ್ಣಾಂಕ)ಕೊನೆಯ_ಸೇರಿಸು_ga.ga_len);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೇರಿಸು != ಖಾಲಿ)
    {
	list_append_string(list, ಸೇರಿಸು->string, (ಪೂರ್ಣಾಂಕ)ಸೇರಿಸು->ಉದ್ದಳತೆ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (n = 0; n < reg->y_ಗಾತ್ರ; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (ಪೂರ್ಣಾಂಕ)reg->y_array[n].ಉದ್ದಳತೆ);
    }

    list->lv_ಬೀಗ = VAR_FIXED;
    ಆದರೆ (dict_ಕೂಡಿಸು_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(v_event, "regname", buf, (ಪೂರ್ಣಾಂಕ)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(v_event, "operator", buf, (ಪೂರ್ಣಾಂಕ)buflen);

    ಕೂಡಿಸು_regtype_ಗಮ್ಯ_dict(regname, v_event, buf, ಗಾತ್ರ(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_bool(v_event, "visual", VIsual_ಸಕ್ರಿಯ);

    // Lock the dictionary and its keys
    dict_ಗಣ_ವಸ್ತುಗಳು_ro(v_event);

    recursive = true;
    textlock++;
    ಆದರೆ (post)
	apply_autocmds(EVENT_TEXTPUTPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    ಇಲ್ಲದಿದ್ದರೆ
	apply_autocmds(EVENT_TEXTPUTPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * set all the yank registers to empty (called from main())
 */
    ಶೂನ್ಯ
init_yank(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_REGISTERS; ++i)
	y_regs[i].y_array = ಖಾಲಿ;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
clear_registers(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	ಆದರೆ (y_current->y_array != ಖಾಲಿ)
	    free_yank_all();
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_yank(ದೀರ್ಘ n)
{
    ಆದರೆ (y_current->y_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ದೀರ್ಘ	    i;

    ಗಾಗಿ (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    ಶೂನ್ಯ
free_yank_all(ಶೂನ್ಯ)
{
    free_yank(y_current->y_ಗಾತ್ರ);
}

/*
 * Yank the text between "oap->start" and "oap->end" into a yank register.
 * If we are to append (uppercase register), we first yank into a new yank
 * register and then concatenate the old and the new one (so we keep the old
 * one in case of out-of-memory).
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
op_yank(oparg_T *oap, ಪೂರ್ಣಾಂಕ deleting, ಪೂರ್ಣಾಂಕ mess)
{
    ದೀರ್ಘ		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    ಪೂರ್ಣಾಂಕ			yanktype = oap->motion_type;
    ದೀರ್ಘ		yanklines = oap->ಸಾಲು_ಎಣಿಕೆ;
    linenr_T		yankendlnum = oap->ಕೊನೆ.lnum;
    char_u		*pnew;
    ರಚನೆ ಖಂಡ_def	bd;

				    // check for read-only register
    ಆದರೆ (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (oap->regname == '_')	    // black hole: nothing to do
	ಹಿಂತಿರುಗಿ OK;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಆದರೆ ((!clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ && oap->regname == '*') ||
	(!clip_plus.ಲಭ್ಯ && oap->regname == '+'))
    {
	oap->regname = 0;
	ಸಂದೇಶ_warn_missing_clipboard();
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!deleting)		    // op_delete() already set y_current
	get_yank_ರಿಜಿಸ್ಟರ್(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    ಆದರೆ (y_append && y_current->y_array != ಖಾಲಿ)
	y_current = &newreg;
    ಇಲ್ಲದಿದ್ದರೆ
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    ಆದರೆ (       oap->motion_type == MCHAR
	    && oap->ಆರಂಭ.ಲಂಬಸಾಲು == 0
	    && !oap->inclusive
	    && (!oap->is_VIsual || *p_sel == 'o')
	    && !oap->ಖಂಡ_ಬಹುಲಕ
	    && oap->ಕೊನೆ.ಲಂಬಸಾಲು == 0
	    && yanklines > 1)
    {
	yanktype = MLINE;
	--yankendlnum;
	--yanklines;
    }

    y_current->y_ಗಾತ್ರ = yanklines;
    y_current->y_type = yanktype;   // set the yank register type
    y_current->y_ಅಗಲ = 0;
    y_current->y_array = lalloc_clear(ಗಾತ್ರ(string_T) * yanklines, TRUE);
    ಆದರೆ (y_current->y_array == ಖಾಲಿ)
    {
	y_current = curr;
	ಹಿಂತಿರುಗಿ FAIL;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    y_current->y_ಸಮಯ_ಗಣ = vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ

    y_idx = 0;
    lnum = oap->ಆರಂಭ.lnum;

    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_ಅಗಲ = oap->ಕೊನೆ_vcol - oap->ಆರಂಭ_vcol;

	ಆದರೆ (curwin->w_curswant == MAXCOL && y_current->y_ಅಗಲ > 0)
	    y_current->y_ಅಗಲ--;
    }

    ಗಾಗಿ ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	ಆಯ್ಕೆ (y_current->y_type)
	{
	    ಸ್ಥಿತಿ MBLOCK:
		ಖಂಡ_prep(oap, &bd, lnum, FALSE);
		ಆದರೆ (yank_copy_ಸಾಲು(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    ಹೋಗು fail;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ MLINE:
		y_current->y_array[y_idx].ಉದ್ದಳತೆ = ml_get_len(lnum);
		ಆದರೆ ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].ಉದ್ದಳತೆ)) == ಖಾಲಿ)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    ಹೋಗು fail;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ MCHAR:
		{
		    ಪೂರ್ಣಾಂಕ tmp;

		    charwise_ಖಂಡ_prep(oap->ಆರಂಭ, oap->ಕೊನೆ, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (ಪೂರ್ಣಾಂಕ)STRLEN(bd.textstart);
		    ಆದರೆ (tmp < bd.textlen)
			bd.textlen = tmp;

		    ಆದರೆ (yank_copy_ಸಾಲು(&bd, y_idx, FALSE) == FAIL)
			ಹೋಗು fail;
		    ನಿಲ್ಲಿಸು;
		}
		// NOTREACHED
	}
    }

    ಆದರೆ (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	ದೀರ್ಘ j;

	new_ptr = ALLOC_MULT(string_T, curr->y_ಗಾತ್ರ + y_current->y_ಗಾತ್ರ);
	ಆದರೆ (new_ptr == ಖಾಲಿ)
	    ಹೋಗು fail;
	ಗಾಗಿ (j = 0; j < curr->y_ಗಾತ್ರ; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
	curr->y_ಸಮಯ_ಗಣ = vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	ಆದರೆ (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == ಖಾಲಿ)
	{
	    pnew = alloc(curr->y_array[curr->y_ಗಾತ್ರ - 1].ಉದ್ದಳತೆ + y_current->y_array[0].ಉದ್ದಳತೆ + 1);
	    ಆದರೆ (pnew == ಖಾಲಿ)
	    {
		y_idx = y_current->y_ಗಾತ್ರ - 1;
		ಹೋಗು fail;
	    }

	    --j;
	    STRCPY(pnew, curr->y_array[j].string);
	    STRCPY(pnew + curr->y_array[j].ಉದ್ದಳತೆ, y_current->y_array[0].string);
	    vim_free(curr->y_array[j].string);
	    curr->y_array[j].string = pnew;
	    curr->y_array[j].ಉದ್ದಳತೆ = curr->y_array[j].ಉದ್ದಳತೆ + y_current->y_array[0].ಉದ್ದಳತೆ;
	    ++j;
	    VIM_CLEAR_STRING(y_current->y_array[0]);
	    y_idx = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    y_idx = 0;
	ಆಗ (y_idx < y_current->y_ಗಾತ್ರ)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_ಗಾತ್ರ = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    ಆದರೆ (mess)			// Display message about yank?
    {
	ಆದರೆ (yanktype == MCHAR
		&& !oap->ಖಂಡ_ಬಹುಲಕ
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	ಆದರೆ (yanklines > p_report)
	{
	    ಅಕ್ಷರ namebuf[100];

	    ಆದರೆ (oap->regname == NUL)
		*namebuf = NUL;
	    ಇಲ್ಲದಿದ್ದರೆ
		vim_snprintf(namebuf, ಗಾತ್ರ(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_ಆರಂಭ = oap->ಆರಂಭ;
	curbuf->b_op_ಕೊನೆ = oap->ಕೊನೆ;
	ಆದರೆ (yanktype == MLINE && !oap->ಖಂಡ_ಬಹುಲಕ)
	{
	    curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = 0;
	    curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = MAXCOL;
	}
	ಆದರೆ (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_ಕೊನೆ);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    ಆದರೆ (curr == &y_regs[REAL_PLUS_ರಿಜಿಸ್ಟರ್] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	ಆದರೆ (curr != &(y_regs[REAL_PLUS_ರಿಜಿಸ್ಟರ್]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_ರಿಜಿಸ್ಟರ್]));
	call_clip_provider_ಗಣ('+');
    }
    ಆದರೆ (curr == &y_regs[ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	ಆದರೆ (curr != &(y_regs[ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್]))
	    copy_yank_reg(&(y_regs[ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್]));
	call_clip_provider_ಗಣ('*');
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	ಆದರೆ (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ
		&& (curr == &(y_regs[ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    ಆದರೆ (curr != &(y_regs[ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್]));

	    clip_own_selection(&clip_ನಕ್ಷತ್ರ);
	    clip_gen_ಗಣ_selection(&clip_ನಕ್ಷತ್ರ);
	}

# ಆದರೆ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	ಆದರೆ (clip_plus.ಲಭ್ಯ
		&& (curr == &(y_regs[PLUS_ರಿಜಿಸ್ಟರ್])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    ಆದರೆ (curr != &(y_regs[PLUS_ರಿಜಿಸ್ಟರ್]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_ರಿಜಿಸ್ಟರ್]));

	    clip_own_selection(&clip_plus);
	    clip_gen_ಗಣ_selection(&clip_plus);
	}
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
yank_copy_ಸಾಲು(ರಚನೆ ಖಂಡ_def *bd, ದೀರ್ಘ y_idx, ಪೂರ್ಣಾಂಕ exclude_trailing_space)
{
    char_u	*pnew;

    ಆದರೆ (exclude_trailing_space)
	bd->endspaces = 0;
    ಆದರೆ ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    ಆದರೆ (exclude_trailing_space)
    {
	ಪೂರ್ಣಾಂಕ s = bd->textlen + bd->endspaces;

	ಆಗ (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_ತಲೆ_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].ಉದ್ದಳತೆ = (size_t)(pnew - y_current->y_array[y_idx].string);

    ಹಿಂತಿರುಗಿ OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    ದೀರ್ಘ	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    ಗಾತ್ರ(string_T) * y_current->y_ಗಾತ್ರ, TRUE);
    ಆದರೆ (y_current->y_array == ಖಾಲಿ)
	y_current->y_ಗಾತ್ರ = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (j = 0; j < y_current->y_ಗಾತ್ರ; ++j)
	{
	    ಆದರೆ ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].ಉದ್ದಳತೆ)) == ಖಾಲಿ)
	    {
		free_yank(j);
		y_current->y_ಗಾತ್ರ = 0;
		ನಿಲ್ಲಿಸು;
	    }
	    y_current->y_array[j].ಉದ್ದಳತೆ = curr->y_array[j].ಉದ್ದಳತೆ;
	}
    y_current = curr;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Put contents of register "regname" into the text.
 * Caller must check "regname" to be valid!
 * "flags": PUT_FIXINDENT	make indent look nice
 *	    PUT_CURSEND		leave cursor after end of new text
 *	    PUT_LINE		force linewise put (":put")
 *	    PUT_BLOCK_INNER     in block mode, do not add trailing spaces
 */
    ಶೂನ್ಯ
do_put(
    ಪೂರ್ಣಾಂಕ		regname,
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಫಲಿತಾಂಶ,	// result for regname "=" when compiled
    ಪೂರ್ಣಾಂಕ		dir,		// BACKWARD for 'P', FORWARD for 'p'
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    ಪೂರ್ಣಾಂಕ		yanklen;
    ಪೂರ್ಣಾಂಕ		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	ಲಂಬಸಾಲು;
    ದೀರ್ಘ	i;			// index in y_array[]
    ಪೂರ್ಣಾಂಕ		y_type;
    ದೀರ್ಘ	y_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		oldlen;
    ದೀರ್ಘ	y_ಅಗಲ = 0;
    colnr_T	vcol;
    string_T	*y_array = ಖಾಲಿ;
    yankreg_T	*y_current_ಬಳಸಿದ = ಖಾಲಿ;
    ದೀರ್ಘ	nr_ಸಾಲುಗಳು = 0;
    string_T	ಸೇರಿಸು_string;
    ಪೂರ್ಣಾಂಕ		allocated = FALSE;
    pos_T	orig_ಆರಂಭ = curbuf->b_op_ಆರಂಭ;
    pos_T	orig_ಕೊನೆ = curbuf->b_op_ಕೊನೆ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cur_ve_flags = get_ve_flags();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(ಶೂನ್ಯ)may_get_selection(regname);
#ಆದರೆ_ಅಂತ್ಯ

    // Remove any preinserted text (issue #19329)
    ಆದರೆ (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_ಆರಂಭ = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_ಕೊನೆ = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    ಆದರೆ (regname == '.')
    {
	ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	    stuffcharReadbuff(VIsual_ಬಹುಲಕ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	ಆದರೆ (has_textput_events)
	    ಕೂಡಿಸು_ಕೊನೆಯ_ಸೇರಿಸು++;
#ಆದರೆ_ಅಂತ್ಯ

	(ಶೂನ್ಯ)stuff_inserted((dir == FORWARD ? (ಎಣಿಕೆ_2 == -1 ? 'o' : 'a') :
				    (ಎಣಿಕೆ_2 == -1 ? 'O' : 'i')), ಎಣಿಕೆ_2, FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	ಆದರೆ (has_textputpre())
	    put_do_autocmd('.', ಖಾಲಿ, ಖಾಲಿ, false, dir);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (has_textputpost())
	    put_do_autocmd('.', ಖಾಲಿ, ಖಾಲಿ, true, dir);

	ಆದರೆ (has_textput_events && --ಕೂಡಿಸು_ಕೊನೆಯ_ಸೇರಿಸು == 0)
	    ga_clear(&ಕೊನೆಯ_ಸೇರಿಸು_ga);
#ಆದರೆ_ಅಂತ್ಯ

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	ಆದರೆ ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	ಹಿಂತಿರುಗಿ;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    ಸೇರಿಸು_string.string = ಖಾಲಿ;
    ಆದರೆ (regname == '=' && ಅಭಿವ್ಯಕ್ತಿ_ಫಲಿತಾಂಶ != ಖಾಲಿ)
	ಸೇರಿಸು_string.string = ಅಭಿವ್ಯಕ್ತಿ_ಫಲಿತಾಂಶ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (get_spec_reg(regname, &ಸೇರಿಸು_string.string, &allocated, TRUE)
		&& ಸೇರಿಸು_string.string == ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    ಆದರೆ (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	ಹೋಗು ಕೊನೆ;

    ಆದರೆ (ಸೇರಿಸು_string.string != ಖಾಲಿ)
    {
	ಸೇರಿಸು_string.ಉದ್ದಳತೆ = STRLEN(ಸೇರಿಸು_string.string);

	y_type = MCHAR;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    ಗಾಗಿ (;;)
	    {
		y_ಗಾತ್ರ = 0;
		ptr = ಸೇರಿಸು_string.string;
		ptrlen = ಸೇರಿಸು_string.ಉದ್ದಳತೆ;
		ಆಗ (ptr != ಖಾಲಿ)
		{
		    ಆದರೆ (y_array != ಖಾಲಿ)
			y_array[y_ಗಾತ್ರ].string = ptr;
		    ++y_ಗಾತ್ರ;
		    tmp = vim_strchr(ptr, '\n');
		    ಆದರೆ (tmp == ಖಾಲಿ)
		    {
			ಆದರೆ (y_array != ಖಾಲಿ)
			    y_array[y_ಗಾತ್ರ - 1].ಉದ್ದಳತೆ = ptrlen;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (y_array != ಖಾಲಿ)
			{
			    *tmp = NUL;
			    y_array[y_ಗಾತ್ರ - 1].ಉದ್ದಳತೆ = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_ಗಾತ್ರ - 1].ಉದ್ದಳತೆ + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			ಆದರೆ (*tmp == NUL)
			{
			    y_type = MLINE;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ptr = tmp;
		}
		ಆದರೆ (y_array != ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		y_array = ALLOC_MULT(string_T, y_ಗಾತ್ರ);
		ಆದರೆ (y_array == ಖಾಲಿ)
		    ಹೋಗು ಕೊನೆ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    y_ಗಾತ್ರ = 1;		// use fake one-line yank register
	    y_array = &ಸೇರಿಸು_string;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (has_textputpre())
	    put_do_autocmd(regname, ಖಾಲಿ, &ಸೇರಿಸು_string, false, dir);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_ರಿಜಿಸ್ಟರ್(regname, FALSE);
	    put_do_autocmd(regname, y_current, ಖಾಲಿ, false, dir);
	}
#ಆದರೆ_ಅಂತ್ಯ
	get_yank_ರಿಜಿಸ್ಟರ್(regname, FALSE);

	y_type = y_current->y_type;
	y_ಅಗಲ = y_current->y_ಅಗಲ;
	y_ಗಾತ್ರ = y_current->y_ಗಾತ್ರ;
	y_array = y_current->y_array;
	y_current_ಬಳಸಿದ = y_current;
    }

    ಆದರೆ (y_type == MLINE)
    {
	ಆದರೆ (flags & PUT_ಸಾಲು_ವಿಭಜಿಸು)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    ಆದರೆ (u_save_cursor() == FAIL)
		ಹೋಗು ಕೊನೆ;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    ಆದರೆ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    ಆದರೆ (ptr == ಖಾಲಿ)
		ಹೋಗು ಕೊನೆ;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.ಲಂಬಸಾಲು;
	    ಆದರೆ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    ಆದರೆ (ptr == ಖಾಲಿ)
		ಹೋಗು ಕೊನೆ;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_ಸಾಲುಗಳು;
	    dir = FORWARD;
	}
	ಆದರೆ (flags & PUT_ಸಾಲು_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_ಕೊನೆ;
	    dir = FORWARD;
	}
	curbuf->b_op_ಆರಂಭ = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_ಕೊನೆ = curwin->w_cursor;	// default for '] mark
    }

    ಆದರೆ (flags & PUT_ಸಾಲು)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    ಆದರೆ (y_ಗಾತ್ರ == 0 || y_array == ಖಾಲಿ)
    {
	semsg(_(e_nothing_ಒಳ_ರಿಜಿಸ್ಟರ್_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	ಹೋಗು ಕೊನೆ;
    }

    ಆದರೆ (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_ಗಾತ್ರ + 1;
	ಆದರೆ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
	ಆದರೆ (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    ಹೋಗು ಕೊನೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	ಆದರೆ (dir == BACKWARD)
	    (ಶೂನ್ಯ)hasFolding(lnum, &lnum, ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    (ಶೂನ್ಯ)hasFolding(lnum, ಖಾಲಿ, &lnum);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	ಆದರೆ ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    ಹೋಗು ಕೊನೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಆದರೆ (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_ಆರಂಭ = curwin->w_cursor;	// for mark_adjust()
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (u_save_cursor() == FAIL)
	ಹೋಗು ಕೊನೆ;

    ಆದರೆ (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	ಆದರೆ (gchar_cursor() == TAB)
	{
	    ಪೂರ್ಣಾಂಕ viscol = getviscol();
	    ಪೂರ್ಣಾಂಕ ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    ಆದರೆ (dir == FORWARD ?
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#ಇಲ್ಲದಿದ್ದರೆ
		    ts - (viscol % ts) != 1
#ಆದರೆ_ಅಂತ್ಯ
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    ಇಲ್ಲದಿದ್ದರೆ
		curwin->w_cursor.coladd = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

    // Block mode
    ಆದರೆ (y_type == MBLOCK)
    {
	ಪೂರ್ಣಾಂಕ	delcount;
	ಪೂರ್ಣಾಂಕ	incr = 0;
	ರಚನೆ ಖಂಡ_def bd;
	ದೀರ್ಘ	j;
	ಪೂರ್ಣಾಂಕ	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	ಆದರೆ (dir == FORWARD && c != NUL)
	{
	    ಆದರೆ (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &ಲಂಬಸಾಲು, ಖಾಲಿ, &endcol2, 0);
	    ಇಲ್ಲದಿದ್ದರೆ
		getvcol(curwin, &curwin->w_cursor, ಖಾಲಿ, ಖಾಲಿ, &ಲಂಬಸಾಲು, 0);

	    ಆದರೆ (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.ಲಂಬಸಾಲು += (*mb_ptr2len)(ml_get_cursor());
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.ಲಂಬಸಾಲು;
	    ++ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    getvcol(curwin, &curwin->w_cursor, &ಲಂಬಸಾಲು, ಖಾಲಿ, &endcol2, 0);

	ಲಂಬಸಾಲು += curwin->w_cursor.coladd;
	ಆದರೆ (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.ಲಂಬಸಾಲು))
	{
	    ಆದರೆ (dir == FORWARD && c == NUL)
		++ಲಂಬಸಾಲು;
	    ಆದರೆ (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.ಲಂಬಸಾಲು;
	    ಆದರೆ (c == TAB)
	    {
		ಆದರೆ (dir == BACKWARD && curwin->w_cursor.ಲಂಬಸಾಲು)
		    curwin->w_cursor.ಲಂಬಸಾಲು--;
		ಆದರೆ (dir == FORWARD && ಲಂಬಸಾಲು - 1 == endcol2)
		    curwin->w_cursor.ಲಂಬಸಾಲು++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	ಗಾಗಿ (i = 0; i < y_ಗಾತ್ರ; ++i)
	{
	    ಪೂರ್ಣಾಂಕ		    spaces = 0;
	    ಅಕ್ಷರ	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    ಆದರೆ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಆದರೆ (ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    ನಿಲ್ಲಿಸು;
		++nr_ಸಾಲುಗಳು;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    ಆಗ (cts.cts_vcol < ಲಂಬಸಾಲು && *cts.cts_ptr != NUL)
	    {
		// Count a tab for what it's worth (if list mode not on)
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    vcol = cts.cts_vcol;
	    ptr = cts.cts_ptr;
	    bd.textcol = (colnr_T)(ptr - oldp);
	    clear_chartabsize_arg(&cts);

	    shortline = (vcol < ಲಂಬಸಾಲು) || (vcol == ಲಂಬಸಾಲು && !*ptr) ;

	    ಆದರೆ (vcol < ಲಂಬಸಾಲು) // line too short, pad with spaces
		bd.startspaces = ಲಂಬಸಾಲು - vcol;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vcol > ಲಂಬಸಾಲು)
	    {
		bd.endspaces = vcol - ಲಂಬಸಾಲು;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		ಆದರೆ (has_mbyte)
		    bd.textcol -= (*mb_ತಲೆ_off)(oldp, oldp + bd.textcol);
		ಆದರೆ (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (ಪೂರ್ಣಾಂಕ)y_array[i].ಉದ್ದಳತೆ;

	    ಆದರೆ ((flags & PUT_ಖಂಡ_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		//ಖಂಡ
		spaces = y_ಅಗಲ + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		ಆಗ (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		ಆದರೆ (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    ಆದರೆ (yanklen + spaces != 0
		     && ಎಣಿಕೆ_2 > ((ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_ಪಠ್ಯ_too_long));
		ನಿಲ್ಲಿಸು;
	    }

	    totlen = ಎಣಿಕೆ_2 * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    ಆದರೆ (newp == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    ಗಾಗಿ (j = 0; j < ಎಣಿಕೆ_2; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		ಆದರೆ ((j < ಎಣಿಕೆ_2 - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    totlen -= spaces;  // didn't use these spaces
	    }

	    // may insert some spaces after the new text
	    vim_memset(ptr, ' ', (size_t)bd.endspaces);
	    ptr += bd.endspaces;

	    // move the text after the cursor to the end of the line.
	    mch_memmove(ptr, oldp + bd.textcol + delcount,
				(size_t)(oldlen - bd.textcol - delcount + 1));
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);

	    ++curwin->w_cursor.lnum;
	    ಆದರೆ (i == 0)
		curwin->w_cursor.ಲಂಬಸಾಲು += bd.startspaces;
	}

	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(lnum, 0, curwin->w_cursor.lnum - nr_ಸಾಲುಗಳು, nr_ಸಾಲುಗಳು);

	// Set '[ mark.
	curbuf->b_op_ಆರಂಭ = curwin->w_cursor;
	curbuf->b_op_ಆರಂಭ.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_ಕೊನೆ.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = bd.textcol + totlen - 1;
	ಆದರೆ (curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು < 0)
	    curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;
	curbuf->b_op_ಕೊನೆ.coladd = 0;
	ಆದರೆ (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_ಕೊನೆ;
	    curwin->w_cursor.ಲಂಬಸಾಲು++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು > len)
		curwin->w_cursor.ಲಂಬಸಾಲು = len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_cursor.lnum = lnum;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	pos_T	new_cursor;

	yanklen = (ಪೂರ್ಣಾಂಕ)y_array[0].ಉದ್ದಳತೆ;

	// Character or Line mode
	ಆದರೆ (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    ಆದರೆ (dir == FORWARD && gchar_cursor() != NUL)
	    {
		ಆದರೆ (has_mbyte)
		{
		    ಪೂರ್ಣಾಂಕ bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    ಲಂಬಸಾಲು += bytelen;
		    ಆದರೆ (yanklen)
		    {
			curwin->w_cursor.ಲಂಬಸಾಲು += bytelen;
			curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು += bytelen;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ++ಲಂಬಸಾಲು;
		    ಆದರೆ (yanklen)
		    {
			++curwin->w_cursor.ಲಂಬಸಾಲು;
			++curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು;
		    }
		}
	    }
	    curbuf->b_op_ಆರಂಭ = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	ಆದರೆ (y_type == MCHAR && y_ಗಾತ್ರ == 1)
	{
	    linenr_T	ಕೊನೆ_lnum = 0; // init for gcc
	    linenr_T	ಆರಂಭ_lnum = lnum;
	    ಪೂರ್ಣಾಂಕ		ಮೊದಲ_byte_off = 0;

	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	    {
		ಕೊನೆ_lnum = curbuf->b_visual.vi_ಕೊನೆ.lnum;
		ಆದರೆ (ಕೊನೆ_lnum < curbuf->b_visual.vi_ಆರಂಭ.lnum)
		    ಕೊನೆ_lnum = curbuf->b_visual.vi_ಆರಂಭ.lnum;
		ಆದರೆ (ಕೊನೆ_lnum > ಆರಂಭ_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, ಖಾಲಿ, &vcol, ಖಾಲಿ, 0);
		}
	    }

	    ಆದರೆ (ಎಣಿಕೆ_2 == 0 || yanklen == 0)
	    {
		ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		    lnum = ಕೊನೆ_lnum;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎಣಿಕೆ_2 > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_ಪಠ್ಯ_too_long));
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		totlen = ಎಣಿಕೆ_2 * yanklen;
		ಮಾಡು
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    ಆದರೆ (lnum > ಆರಂಭ_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			ಆದರೆ (getvpos(&pos, vcol) == OK)
			    ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು;
			ಇಲ್ಲದಿದ್ದರೆ
			    ಲಂಬಸಾಲು = MAXCOL;
		    }
		    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && ಲಂಬಸಾಲು > oldlen)
		    {
			lnum++;
			ಮುಂದುವರಿ;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    ಆದರೆ (newp == ಖಾಲಿ)
			ಹೋಗು ಕೊನೆ;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)ಲಂಬಸಾಲು);
		    ptr = newp + ಲಂಬಸಾಲು;
		    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
		    {
			mch_memmove(ptr, y_array[0].string, (size_t)yanklen);
			ptr += yanklen;
		    }
		    mch_memmove(ptr, oldp + ಲಂಬಸಾಲು, (size_t)(oldlen - ಲಂಬಸಾಲು) + 1);	    // +1 for NUL

		    // compute the byte offset for the last character
		    ಮೊದಲ_byte_off = mb_ತಲೆ_off(newp, ptr - 1);

		    // Note: this may free "newp"
		    ml_replace(lnum, newp, FALSE);

		    inserted_ಬೈಟುಗಳು(lnum, ಲಂಬಸಾಲು, totlen);

		    // Place cursor on last putted char.
		    ಆದರೆ (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			ಬದಲಾಯಿತು_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.ಲಂಬಸಾಲು += (colnr_T)(totlen - 1);
		    }
		    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
			lnum++;
		} ಆಗ (VIsual_ಸಕ್ರಿಯ && lnum <= ಕೊನೆ_lnum);

		ಆದರೆ (VIsual_ಸಕ್ರಿಯ) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_ಕೊನೆ = curwin->w_cursor;
	    curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು -= ಮೊದಲ_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    ಆದರೆ (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.ಲಂಬಸಾಲು;
	    ಇಲ್ಲದಿದ್ದರೆ
		curwin->w_cursor.ಲಂಬಸಾಲು -= ಮೊದಲ_byte_off;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    ಪೂರ್ಣಾಂಕ		indent;
	    ಪೂರ್ಣಾಂಕ		orig_indent = 0;
	    ಪೂರ್ಣಾಂಕ		indent_diff = 0;	// init for gcc
	    ಪೂರ್ಣಾಂಕ		ಮೊದಲ_indent = TRUE;
	    ಪೂರ್ಣಾಂಕ		lendiff = 0;
	    ದೀರ್ಘ	cnt;

	    ಆದರೆ (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    ಗಾಗಿ (cnt = 1; cnt <= ಎಣಿಕೆ_2; ++cnt)
	    {
		i = 0;
		ಆದರೆ (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + ಲಂಬಸಾಲು;
		    totlen = (ಪೂರ್ಣಾಂಕ)y_array[y_ಗಾತ್ರ - 1].ಉದ್ದಳತೆ;
		    newp = alloc(ml_get_len(lnum) - ಲಂಬಸಾಲು + totlen + 1);
		    ಆದರೆ (newp == ಖಾಲಿ)
			ಹೋಗು error;
		    STRCPY(newp, y_array[y_ಗಾತ್ರ - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(ಲಂಬಸಾಲು + yanklen + 1);
		    ಆದರೆ (newp == ಖಾಲಿ)
			ಹೋಗು error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)ಲಂಬಸಾಲು);
					    // append to first line
		    mch_memmove(newp + ಲಂಬಸಾಲು, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		ಗಾಗಿ (; i < y_ಗಾತ್ರ; ++i)
		{
		    ಆದರೆ (y_type != MCHAR || i < y_ಗಾತ್ರ - 1)
		    {
			ಆದರೆ (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    ಹೋಗು error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_ಸಾಲುಗಳು;
		    ಆದರೆ (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			ಆದರೆ (cnt == ಎಣಿಕೆ_2 && i == y_ಗಾತ್ರ - 1)
			    lendiff = ml_get_len(lnum);
			ಆದರೆ (*ptr == '#' && preprocs_ಎಡ())
			    indent = 0;     // Leave # lines at start
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    ಮೊದಲ_indent = FALSE;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(ಶೂನ್ಯ)ಗಣ_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			ಆದರೆ (cnt == ಎಣಿಕೆ_2 && i == y_ಗಾತ್ರ - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		ಆದರೆ (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    ಆದರೆ (y_type == MLINE)
	    {
		curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = 0;
		ಆದರೆ (dir == FORWARD)
		    curbuf->b_op_ಆರಂಭ.lnum++;
	    }
	    mark_adjust(curbuf->b_op_ಆರಂಭ.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_ಸಾಲುಗಳು, 0L);

	    // note changed text for displaying and folding
	    ಆದರೆ (y_type == MCHAR)
		ಬದಲಾಯಿತು_ಸಾಲುಗಳು(curwin->w_cursor.lnum, ಲಂಬಸಾಲು,
					 curwin->w_cursor.lnum + 1, nr_ಸಾಲುಗಳು);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಬದಲಾಯಿತು_ಸಾಲುಗಳು(curbuf->b_op_ಆರಂಭ.lnum, 0,
					   curbuf->b_op_ಆರಂಭ.lnum, nr_ಸಾಲುಗಳು);
	    ಆದರೆ (y_current_ಬಳಸಿದ != ಖಾಲಿ && (y_current_ಬಳಸಿದ != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_ರಿಜಿಸ್ಟರ್_ಬದಲಾಯಿತು_while_using_it));
		ಹೋಗು ಕೊನೆ;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_ಕೊನೆ.lnum = new_lnum;
	    ಲಂಬಸಾಲು = MAX(0, (colnr_T)y_array[y_ಗಾತ್ರ - 1].ಉದ್ದಳತೆ - lendiff);
	    ಆದರೆ (ಲಂಬಸಾಲು > 1)
	    {
		curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು - 1;
		ಆದರೆ (y_array[y_ಗಾತ್ರ - 1].ಉದ್ದಳತೆ > 0)
		    curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು -= mb_ತಲೆ_off(y_array[y_ಗಾತ್ರ - 1].string,
						y_array[y_ಗಾತ್ರ - 1].string + y_array[y_ಗಾತ್ರ - 1].ಉದ್ದಳತೆ - 1);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;

	    ಆದರೆ (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		ಆದರೆ (y_type == MLINE)
		{
		    ಆದರೆ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		    ಇಲ್ಲದಿದ್ದರೆ
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		    curbuf->b_op_ಕೊನೆ = curwin->w_cursor;
		    ಆದರೆ (ಲಂಬಸಾಲು > 1)
			curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು - 1;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		ಆದರೆ (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_ಸಾಲುಗಳು);
    curwin->w_ಗಣ_curswant = true;

    // Make sure the cursor is not after the NUL.
    ಪೂರ್ಣಾಂಕ len = ml_get_curline_len();
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು > len)
    {
	ಆದರೆ (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.ಲಂಬಸಾಲು - len;
	curwin->w_cursor.ಲಂಬಸಾಲು = len;
    }

ಕೊನೆ:
    ಆದರೆ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_ಆರಂಭ = orig_ಆರಂಭ;
	curbuf->b_op_ಕೊನೆ = orig_ಕೊನೆ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (has_textputpost())
    {
	ಆದರೆ (ಸೇರಿಸು_string.string == ಖಾಲಿ)
	    put_do_autocmd(regname, y_current, ಖಾಲಿ, true, dir);
	ಇಲ್ಲದಿದ್ದರೆ
	    put_do_autocmd(regname, ಖಾಲಿ, &ಸೇರಿಸು_string, true, dir);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (allocated)
	vim_free(ಸೇರಿಸು_string.string);
    ಆದರೆ (regname == '=')
	vim_free(y_array);

    VIsual_ಸಕ್ರಿಯ = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    ಪೂರ್ಣಾಂಕ
get_ರಿಜಿಸ್ಟರ್_name(ಪೂರ್ಣಾಂಕ ಅಂಕೆ)
{
    ಆದರೆ (ಅಂಕೆ == -1)
	ಹಿಂತಿರುಗಿ '"';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಕೆ < 10)
	ಹಿಂತಿರುಗಿ ಅಂಕೆ + '0';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಕೆ == DELETION_ರಿಜಿಸ್ಟರ್)
	ಹಿಂತಿರುಗಿ '-';
#ಆದರೆ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಕೆ == ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್)
	ಹಿಂತಿರುಗಿ '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod == CLIPMETHOD_PROVIDER && ಅಂಕೆ == REAL_PLUS_ರಿಜಿಸ್ಟರ್)
	ಹಿಂತಿರುಗಿ '+';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER && ಅಂಕೆ == PLUS_ರಿಜಿಸ್ಟರ್)
	ಹಿಂತಿರುಗಿ '+';
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಅಂಕೆ + 'a' - 10;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    ಪೂರ್ಣಾಂಕ
get_unname_ರಿಜಿಸ್ಟರ್(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ y_ಹಿಂದಿನ == ಖಾಲಿ ? -1 : y_ಹಿಂದಿನ - &y_regs[0];
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    ಶೂನ್ಯ
ex_display(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		i, n;
    ದೀರ್ಘ	j;
    char_u	*p;
    yankreg_T	*yb;
    ಪೂರ್ಣಾಂಕ		name;
    ಪೂರ್ಣಾಂಕ		attr;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		clen;
    ಪೂರ್ಣಾಂಕ		type;
    string_T	ಸೇರಿಸು;

    silence_w23_w24_ಸಂದೇಶ++;
    ಆದರೆ (arg != ಖಾಲಿ && *arg == NUL)
	arg = ಖಾಲಿ;
    attr = HL_ATTR(HLF_8);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\nType Name Content"));
    ಗಾಗಿ (i = -1; i < ಅಂಕೆ_REGISTERS && !got_int; ++i)
    {
	name = get_ರಿಜಿಸ್ಟರ್_name(i);
	ಆಯ್ಕೆ (get_reg_type(name, ಖಾಲಿ))
	{
	    ಸ್ಥಿತಿ MLINE: type = 'l'; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MCHAR: type = 'c'; ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:	type = 'b'; ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (arg != ಖಾಲಿ && vim_strchr(arg, name) == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
		)
	    ಮುಂದುವರಿ;	    // did not ask for this register

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (ಶೂನ್ಯ)may_get_selection(name);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (i == -1)
	{
	    ಆದರೆ (y_ಹಿಂದಿನ != ಖಾಲಿ)
		yb = y_ಹಿಂದಿನ;
	    ಇಲ್ಲದಿದ್ದರೆ
		yb = &(y_regs[0]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    yb = &(y_regs[i]);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != ಖಾಲಿ &&
		    (yb == y_ಹಿಂದಿನ || yb == &y_regs[0])))
	    ಮುಂದುವರಿ;	    // do not list register being written to, the
			    // pointer can be freed
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (yb->y_array != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ do_show = FALSE;

	    ಗಾಗಿ (j = 0; !do_show && j < yb->y_ಗಾತ್ರ; ++j)
		do_show = !ಸಂದೇಶ_filtered(yb->y_array[j].string);

	    ಆದರೆ (do_show || yb->y_ಗಾತ್ರ == 0)
	    {
		ಸಂದೇಶ_putchar('\n');
		ಸಂದೇಶ_puts("  ");
		ಸಂದೇಶ_putchar(type);
		ಸಂದೇಶ_puts("  ");
		ಸಂದೇಶ_putchar('"');
		ಸಂದೇಶ_putchar(name);
		ಸಂದೇಶ_puts("   ");

		n = (ಪೂರ್ಣಾಂಕ)Columns - 11;
		ಗಾಗಿ (j = 0; j < yb->y_ಗಾತ್ರ && n > 1; ++j)
		{
		    ಆದರೆ (j)
		    {
			ಸಂದೇಶ_puts_attr("^J", attr);
			n -= 2;
		    }
		    ಗಾಗಿ (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			ಸಂದೇಶ_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		ಆದರೆ (n > 1 && yb->y_type == MLINE)
		    ಸಂದೇಶ_puts_attr("^J", attr);
		ಹೊರಹೋಗು_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    ಸೇರಿಸು = get_ಕೊನೆಯ_ಸೇರಿಸು();
    ಆದರೆ ((p = ಸೇರಿಸು.string) != ಖಾಲಿ
		  && (arg == ಖಾಲಿ || vim_strchr(arg, '.') != ಖಾಲಿ) && !got_int
						      && !ಸಂದೇಶ_filtered(p))
    {
	ಸಂದೇಶ_puts("\n  c  \".   ");
	dis_ಸಂದೇಶ(p, TRUE);
    }

    // display last command line
    ಆದರೆ (ಕೊನೆಯ_cmdline != ಖಾಲಿ && (arg == ಖಾಲಿ || vim_strchr(arg, ':') != ಖಾಲಿ)
			       && !got_int && !ಸಂದೇಶ_filtered(ಕೊನೆಯ_cmdline))
    {
	ಸಂದೇಶ_puts("\n  c  \":   ");
	dis_ಸಂದೇಶ(ಕೊನೆಯ_cmdline, FALSE);
    }

    // display current file name
    ಆದರೆ (curbuf->b_fname != ಖಾಲಿ
	    && (arg == ಖಾಲಿ || vim_strchr(arg, '%') != ಖಾಲಿ) && !got_int
					&& !ಸಂದೇಶ_filtered(curbuf->b_fname))
    {
	ಸಂದೇಶ_puts("\n  c  \"%   ");
	dis_ಸಂದೇಶ(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    ಆದರೆ ((arg == ಖಾಲಿ || vim_strchr(arg, '#') != ಖಾಲಿ) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	ಆದರೆ (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !ಸಂದೇಶ_filtered(fname))
	{
	    ಸಂದೇಶ_puts("\n  c  \"#   ");
	    dis_ಸಂದೇಶ(fname, FALSE);
	}
    }

    // display last search pattern
    ಆದರೆ (ಕೊನೆಯ_ಹುಡುಕು_pat() != ಖಾಲಿ
		 && (arg == ಖಾಲಿ || vim_strchr(arg, '/') != ಖಾಲಿ) && !got_int
				      && !ಸಂದೇಶ_filtered(ಕೊನೆಯ_ಹುಡುಕು_pat()))
    {
	ಸಂದೇಶ_puts("\n  c  \"/   ");
	dis_ಸಂದೇಶ(ಕೊನೆಯ_ಹುಡುಕು_pat(), FALSE);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // display last used expression
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು != ಖಾಲಿ && (arg == ಖಾಲಿ || vim_strchr(arg, '=') != ಖಾಲಿ)
				  && !got_int && !ಸಂದೇಶ_filtered(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು))
    {
	ಸಂದೇಶ_puts("\n  c  \"=   ");
	dis_ಸಂದೇಶ(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು, FALSE);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
    silence_w23_w24_ಸಂದೇಶ--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dis_ಸಂದೇಶ(
    char_u	*p,
    ಪೂರ್ಣಾಂಕ		skip_esc)	    // if TRUE, ignore trailing ESC
{
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		l;

    n = (ಪೂರ್ಣಾಂಕ)Columns - 6;
    ಆಗ (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ಸಂದೇಶ_outtrans_len(p, l);
	    p += l;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#ಆದರೆ defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    ಶೂನ್ಯ
dnd_yank_drag_ದತ್ತಾಂಶ(char_u *str, ದೀರ್ಘ len)
{
    yankreg_T *curr;

    curr = y_current;
    y_current = &y_regs[TILDE_ರಿಜಿಸ್ಟರ್];
    free_yank_all();
    str_ಗಮ್ಯ_reg(y_current, MCHAR, str, len, 0L, FALSE);
    y_current = curr;
}
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Return the type of a register.
 * Used for getregtype()
 * Returns MAUTO for error.
 */
    char_u
get_reg_type(ಪೂರ್ಣಾಂಕ regname, ದೀರ್ಘ *reglen)
{
    ಆಯ್ಕೆ (regname)
    {
	ಸ್ಥಿತಿ '%':		// file name
	ಸ್ಥಿತಿ '#':		// alternate file name
	ಸ್ಥಿತಿ '=':		//ಅಭಿವ್ಯಕ್ತಿ
	ಸ್ಥಿತಿ ':':		// last command line
	ಸ್ಥಿತಿ '/':		// last search-pattern
	ಸ್ಥಿತಿ '.':		// last inserted text
	ಸ್ಥಿತಿ Ctrl_F:		// Filename under cursor
	ಸ್ಥಿತಿ Ctrl_P:		// Path under cursor, expand via "path"
	ಸ್ಥಿತಿ Ctrl_W:		// word under cursor
	ಸ್ಥಿತಿ Ctrl_A:		// WORD (mnemonic All) under cursor
	ಸ್ಥಿತಿ '_':		// black hole: always empty
	    ಹಿಂತಿರುಗಿ MCHAR;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ಹಿಂತಿರುಗಿ MAUTO;

    get_yank_ರಿಜಿಸ್ಟರ್(regname, FALSE);

    ಆದರೆ (y_current->y_array != ಖಾಲಿ)
    {
	ಆದರೆ (reglen != ಖಾಲಿ && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_ಅಗಲ;
	ಹಿಂತಿರುಗಿ y_current->y_type;
    }
    ಹಿಂತಿರುಗಿ MAUTO;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    ಸ್ಥಿರ char_u *
getreg_wrap_one_ಸಾಲು(char_u *s, ಪೂರ್ಣಾಂಕ flags)
{
    ಆದರೆ (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	ಆದರೆ (list != ಖಾಲಿ)
	{
	    ಆದರೆ (list_append_string(list, ಖಾಲಿ, -1) == FAIL)
	    {
		list_free(list);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    list->lv_ಮೊದಲ->li_tv.vval.v_string = s;
	}
	ಹಿಂತಿರುಗಿ (char_u *)list;
    }
    ಹಿಂತಿರುಗಿ s;
}

/*
 * Return the contents of a register as a single allocated string or as a list.
 * Used for "@r" in expressions and for getreg().
 * Returns NULL for error.
 * Flags:
 *	GREG_NO_EXPR	Do not allow expression register
 *	GREG_EXPR_SRC	For the expression register: return expression itself,
 *			not the result of its evaluation.
 *	GREG_LIST	Return a list of lines instead of a single string.
 */
    char_u *
get_reg_contents(ಪೂರ್ಣಾಂಕ regname, ಪೂರ್ಣಾಂಕ flags)
{
    linenr_T	i;
    char_u	*retval;
    ಪೂರ್ಣಾಂಕ		allocated;
    ದೀರ್ಘ	len;

    // Don't allow using an expression register inside an expression
    ಆದರೆ (regname == '=')
    {
	ಆದರೆ (flags & GREG_NO_ಅಭಿವ್ಯಕ್ತಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (flags & GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC)
	    ಹಿಂತಿರುಗಿ getreg_wrap_one_ಸಾಲು(get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು_src(), flags);
	ಹಿಂತಿರುಗಿ getreg_wrap_one_ಸಾಲು(get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(), flags);
    }

    ಆದರೆ (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    ಆದರೆ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ಆದರೆ (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	ಆದರೆ (retval == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (allocated)
	    ಹಿಂತಿರುಗಿ getreg_wrap_one_ಸಾಲು(retval, flags);
	ಹಿಂತಿರುಗಿ getreg_wrap_one_ಸಾಲು(vim_strsave(retval), flags);
    }

    get_yank_ರಿಜಿಸ್ಟರ್(regname, FALSE);
    ಆದರೆ (y_current->y_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	ಆದರೆ (list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಗಾಗಿ (i = 0; i < y_current->y_ಗಾತ್ರ; ++i)
	{
	    ಆದರೆ (list_append_string(list, y_current->y_array[i].string,
		(ಪೂರ್ಣಾಂಕ)y_current->y_array[i].ಉದ್ದಳತೆ) == FAIL)
	    {
		list_free(list);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
	ಹಿಂತಿರುಗಿ (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    ಗಾಗಿ (i = 0; i < y_current->y_ಗಾತ್ರ; ++i)
    {
	len += (ದೀರ್ಘ)y_current->y_array[i].ಉದ್ದಳತೆ;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ಆದರೆ (y_current->y_type == MLINE || i < y_current->y_ಗಾತ್ರ - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    ಆದರೆ (retval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Copy the lines of the yank register into the string.
    len = 0;
    ಗಾಗಿ (i = 0; i < y_current->y_ಗಾತ್ರ; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (ದೀರ್ಘ)y_current->y_array[i].ಉದ್ದಳತೆ;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ಆದರೆ (y_current->y_type == MLINE || i < y_current->y_ಗಾತ್ರ - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    ಹಿಂತಿರುಗಿ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
init_write_reg(
    ಪೂರ್ಣಾಂಕ		name,
    yankreg_T	**old_y_ಹಿಂದಿನ,
    yankreg_T	**old_y_current,
    ಪೂರ್ಣಾಂಕ		must_append,
    ಪೂರ್ಣಾಂಕ		*yank_type ಬಳಸದ)
{
    ಆದರೆ (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_ಹಿಂದಿನ = y_ಹಿಂದಿನ;
    *old_y_current = y_current;

    get_yank_ರಿಜಿಸ್ಟರ್(name, TRUE);
    ಆದರೆ (!y_append && !must_append)
	free_yank_all();
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಮುಕ್ತಾಯ_write_reg(
    ಪೂರ್ಣಾಂಕ		name,
    yankreg_T	*old_y_ಹಿಂದಿನ,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_ಗಣ_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    ಆದರೆ (name != '"')
	y_ಹಿಂದಿನ = old_y_ಹಿಂದಿನ;
    y_current = old_y_current;
}

/*
 * Store string "str" in register "name".
 * "maxlen" is the maximum number of bytes to use, -1 for all bytes.
 * If "must_append" is TRUE, always append to the register.  Otherwise append
 * if "name" is an uppercase letter.
 * Note: "maxlen" and "must_append" don't work for the "/" register.
 * Careful: 'str' is modified, you may have to use a copy!
 * If "str" ends in '\n' or '\r', use linewise, otherwise use characterwise.
 */
    ಶೂನ್ಯ
write_reg_contents(
    ಪೂರ್ಣಾಂಕ		name,
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		maxlen,
    ಪೂರ್ಣಾಂಕ		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    ಶೂನ್ಯ
write_reg_contents_lst(
    ಪೂರ್ಣಾಂಕ		name,
    char_u	**ಅಕ್ಷರಸರಣಿಗಳು,
    ಪೂರ್ಣಾಂಕ		maxlen ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		must_append,
    ಪೂರ್ಣಾಂಕ		yank_type,
    ದೀರ್ಘ	ಖಂಡ_len)
{
    yankreg_T  *old_y_ಹಿಂದಿನ, *old_y_current;

    ಆದರೆ (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು[0] == ಖಾಲಿ)
	    s = (char_u *)"";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು[1] != ಖಾಲಿ)
	{
	    semsg(_(e_ರಿಜಿಸ್ಟರ್_char_cannot_contain_ಅಪವರ್ತ್ಯ_ಸಾಲುಗಳು), name);
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    s = ಅಕ್ಷರಸರಣಿಗಳು[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, ಖಂಡ_len);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (name == '_')	    // black hole: nothing to do
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (init_write_reg(name, &old_y_ಹಿಂದಿನ, &old_y_current, must_append,
		&yank_type) == FAIL)
	ಹಿಂತಿರುಗಿ;

    str_ಗಮ್ಯ_reg(y_current, yank_type, (char_u *)ಅಕ್ಷರಸರಣಿಗಳು, -1, ಖಂಡ_len, TRUE);

    ಮುಕ್ತಾಯ_write_reg(name, old_y_ಹಿಂದಿನ, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_ಗಣ(name);
# endif
}

    ಶೂನ್ಯ
write_reg_contents_ex(
    ಪೂರ್ಣಾಂಕ		name,
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		maxlen,
    ಪೂರ್ಣಾಂಕ		must_append,
    ಪೂರ್ಣಾಂಕ		yank_type,
    ದೀರ್ಘ	ಖಂಡ_len)
{
    yankreg_T	*old_y_ಹಿಂದಿನ, *old_y_current;
    ದೀರ್ಘ	len;

    ಆದರೆ (maxlen >= 0)
	len = maxlen;
    ಇಲ್ಲದಿದ್ದರೆ
	len = (ದೀರ್ಘ)STRLEN(str);

    // Special case: '/' search pattern
    ಆದರೆ (name == '/')
    {
	ಗಣ_ಕೊನೆಯ_ಹುಡುಕು_pat(str, RE_ಹುಡುಕು, TRUE, TRUE);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (name == '#')
    {
	ಆದರೆ (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  ಹಿಂತಿರುಗಿ;
	}

	buf_T	*buf;

	ಆದರೆ (VIM_ISDIGIT(*str))
	{
	    ಪೂರ್ಣಾಂಕ	ಅಂಕೆ = atoi((ಅಕ್ಷರ *)str);

	    buf = buflist_findnr(ಅಂಕೆ);
	    ಆದರೆ (buf == ಖಾಲಿ)
		semsg(_(e_ಬಫರ್_nr_does_not_exist), (ದೀರ್ಘ)ಅಂಕೆ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	curwin->w_alt_fnum = buf->b_fnum;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (must_append && ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು != ಖಾಲಿ)
	{
	    s = concat_str(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು, p);
	    vim_free(p);
	    p = s;
	}
	ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(p, ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (name == '_')	    // black hole: nothing to do
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (init_write_reg(name, &old_y_ಹಿಂದಿನ, &old_y_current, must_append,
		&yank_type) == FAIL)
	ಹಿಂತಿರುಗಿ;

    str_ಗಮ್ಯ_reg(y_current, yank_type, str, len, ಖಂಡ_len, FALSE);

    ಮುಕ್ತಾಯ_write_reg(name, old_y_ಹಿಂದಿನ, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_ಗಣ(name);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ	// FEAT_EVAL

#ಆದರೆ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    ಶೂನ್ಯ
str_ಗಮ್ಯ_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    ಪೂರ್ಣಾಂಕ		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    ದೀರ್ಘ	len,		// length of string
    ದೀರ್ಘ	blocklen,	// width of Visual block
    ಪೂರ್ಣಾಂಕ		str_list)	// TRUE if str is char_u **
{
    ಪೂರ್ಣಾಂಕ		type;			// MCHAR, MLINE or MBLOCK
    ಪೂರ್ಣಾಂಕ		lnum;
    ದೀರ್ಘ	ಆರಂಭ;
    ದೀರ್ಘ	i;
    ಪೂರ್ಣಾಂಕ		extra;
    ಪೂರ್ಣಾಂಕ		newlines;		// number of lines added
    ಪೂರ್ಣಾಂಕ		extraline = 0;		// extra line at the end
    ಪೂರ್ಣಾಂಕ		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    ದೀರ್ಘ	maxlen;

    ಆದರೆ (y_ptr->y_array == ಖಾಲಿ)		// NULL means empty register
	y_ptr->y_ಗಾತ್ರ = 0;

    ಆದರೆ (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    ಇಲ್ಲದಿದ್ದರೆ
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    ಆದರೆ (str_list)
    {
	ಗಾಗಿ (ss = (char_u **) str; *ss != ಖಾಲಿ; ++ss)
	    ++newlines;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = 0; i < len; i++)
	    ಆದರೆ (str[i] == '\n')
		++newlines;
	ಆದರೆ (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	ಆದರೆ (y_ptr->y_ಗಾತ್ರ > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    ಆದರೆ (y_ptr->y_ಗಾತ್ರ + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	ಹಿಂತಿರುಗಿ;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_ಗಾತ್ರ + newlines) * ಗಾತ್ರ(string_T), TRUE);
    ಆದರೆ (pp == ಖಾಲಿ)	// out of memory
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (lnum = 0; lnum < y_ptr->y_ಗಾತ್ರ; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    ಆದರೆ (str_list)
    {
	ಗಾಗಿ (ss = (char_u **) str; *ss != ಖಾಲಿ; ++ss, ++lnum)
	{
	    pp[lnum].ಉದ್ದಳತೆ = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].ಉದ್ದಳತೆ);
	    ಆದರೆ (type == MBLOCK)
	    {
		ಪೂರ್ಣಾಂಕ charlen = vim_strsize(*ss);

		ಆದರೆ (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (ಆರಂಭ = 0; ಆರಂಭ < len + extraline; ಆರಂಭ += i + 1)
	{
	    ಪೂರ್ಣಾಂಕ charlen = 0;

	    ಗಾಗಿ (i = ಆರಂಭ; i < len;)	// find the end of the line
	    {
		ಆದರೆ (str[i] == '\n')
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (type == MBLOCK)
		{
		    ಆದರೆ (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    ಇಲ್ಲದಿದ್ದರೆ
			charlen += byte2cells(str[i]);
		}

		ಆದರೆ (str[i] == NUL)
		    i++; // registers can have NUL chars
		ಇಲ್ಲದಿದ್ದರೆ
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= ಆರಂಭ;			// i is now length of line
	    ಆದರೆ (charlen > maxlen)
		maxlen = charlen;
	    ಆದರೆ (append)
	    {
		--lnum;
		extra = (ಪೂರ್ಣಾಂಕ)y_ptr->y_array[lnum].ಉದ್ದಳತೆ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		extra = 0;
	    s = alloc(i + extra + 1);
	    ಆದರೆ (s == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    ಆದರೆ (append)
		vim_free(y_ptr->y_array[lnum].string);
	    ಆದರೆ (i > 0)
		mch_memmove(s + extra, str + ಆರಂಭ, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].ಉದ್ದಳತೆ = extra;
	    ++lnum;
	    ಆಗ (--extra >= 0)
	    {
		ಆದರೆ (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_ಗಾತ್ರ = lnum;
    ಆದರೆ (type == MBLOCK)
	y_ptr->y_ಅಗಲ = (blocklen < 0 ? maxlen - 1 : blocklen);
    ಇಲ್ಲದಿದ್ದರೆ
	y_ptr->y_ಅಗಲ = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_ಸಮಯ_ಗಣ = vim_ಸಮಯ();
# endif
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_CLIPBOARD || FEAT_EVAL
