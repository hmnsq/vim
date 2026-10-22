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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		y_append;	    // TRUE when appending
ಸ್ಥಿರ yankreg_T	*y_previous = ರಿಕ್ತ; // ptr to last written yankreg

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	stuff_yank(ಪೂರ್ಣಾಙ್ಕ, char_u *);
ಸ್ಥಿರ ಶೂನ್ಯ	put_reedit_in_typebuf(ಪೂರ್ಣಾಙ್ಕ silent);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	put_in_typebuf(char_u *s, ಪೂರ್ಣಾಙ್ಕ esc, ಪೂರ್ಣಾಙ್ಕ colon, ಪೂರ್ಣಾಙ್ಕ silent);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	yank_copy_ಸಾಲು(ಸಂರಚನಾ block_def *bd, long y_idx, ಪೂರ್ಣಾಙ್ಕ exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
ಸ್ಥಿರ ಶೂನ್ಯ	copy_yank_reg(yankreg_T *reg);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ	dis_ಸಂದೇಶ(char_u *p, ಪೂರ್ಣಾಙ್ಕ skip_esc);

#ಯದಿ defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ y_regs;
}
#endif

#ಯದಿ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(ಪೂರ್ಣಾಙ್ಕ reg)
{
    ಪ್ರತ್ಯಾಗಮ &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ y_current;
}

    yankreg_T *
get_y_previous(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ y_previous;
}

    ಶೂನ್ಯ
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    ಶೂನ್ಯ
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    ಶೂನ್ಯ
reset_y_append(ಶೂನ್ಯ)
{
    y_append = FALSE;
}


#ಯದಿ defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
ಸ್ಥಿರ char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು = ರಿಕ್ತ;
ಸ್ಥಿರ exarg_T	*ಅಭಿವ್ಯಕ್ತಿ_eap = ರಿಕ್ತ;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_ಅಭಿವ್ಯಕ್ತಿ_register(ಶೂನ್ಯ)
{
    char_u	*new_ಸಾಲು;

    new_ಸಾಲು = getcmdline('=', 0L, 0, 0);
    ಯದಿ (new_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ NUL;
    ಯದಿ (*new_ಸಾಲು == NUL)	// use previous line
	vim_free(new_ಸಾಲು);
    ಅನ್ಯಥಾ
	set_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(new_ಸಾಲು, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    ಶೂನ್ಯ
set_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(char_u *new_ಸಾಲು, exarg_T *eap)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	nested = 0;

    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    ಅಭಿವ್ಯಕ್ತಿ_copy = vim_strsave(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು);
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_copy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    ಯದಿ (nested >= 10)
	ಪ್ರತ್ಯಾಗಮ ಅಭಿವ್ಯಕ್ತಿ_copy;

    ++nested;
    rv = eval_to_string_eap(ಅಭಿವ್ಯಕ್ತಿ_copy, TRUE, ಅಭಿವ್ಯಕ್ತಿ_eap, FALSE);
    --nested;
    vim_free(ಅಭಿವ್ಯಕ್ತಿ_copy);
    ಪ್ರತ್ಯಾಗಮ rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ಸ್ಥಿರ char_u *
get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು_src(ಶೂನ್ಯ)
{
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ vim_strsave(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    ಪೂರ್ಣಾಙ್ಕ
valid_yank_reg(
    ಪೂರ್ಣಾಙ್ಕ	    regname,
    ಪೂರ್ಣಾಙ್ಕ	    writing)	    // if TRUE check for writable registers
{
    ಯದಿ (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#ಅನ್ಯಥಾ
				    "/#.%:"
#endif
					, regname) != ರಿಕ್ತ)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#ಯದಿ defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#ನೋಚೇದ್ಯದಿ defined(FEAT_CLIPBOARD_PROVIDER)
	    || ( // If -clipboard, then these registers only exist when
		 // clipmethod is set to provider.
	     clipmethod == CLIPMETHOD_PROVIDER && (
		 regname == '*'
		 || regname == '+'
	    ))
#endif
#ifdef FEAT_DND
	    || (!writing && regname == '~')
#endif
							)
	ಪ್ರತ್ಯಾಗಮ TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    ಅನ್ಯಥಾ ಯದಿ (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	ಸಂದೇಶ_warn_missing_clipboard();
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಪೂರ್ಣಾಙ್ಕ
get_yank_register(ಪೂರ್ಣಾಙ್ಕ regname, ಪೂರ್ಣಾಙ್ಕ writing)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    ret = FALSE;

    y_append = FALSE;
    ಯದಿ ((regname == 0 || regname == '"') && !writing && y_previous != ರಿಕ್ತ)
    {
	y_current = y_previous;
	ಪ್ರತ್ಯಾಗಮ ret;
    }
    i = regname;
    ಯದಿ (VIM_ISDIGIT(i))
	i -= '0';
    ಅನ್ಯಥಾ ಯದಿ (ASCII_ISLOWER(i))
	i = Char_Ord_ಕೆಳಗಿನ(i) + 10;
    ಅನ್ಯಥಾ ಯದಿ (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    ಅನ್ಯಥಾ ಯದಿ (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    ಅನ್ಯಥಾ ಯದಿ (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	ಯದಿ (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	ಅನ್ಯಥಾ
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#ನೋಚೇದ್ಯದಿ defined(FEAT_CLIPBOARD_PROVIDER)
    ಅನ್ಯಥಾ ಯದಿ (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    ಅನ್ಯಥಾ ಯದಿ (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    ಅನ್ಯಥಾ		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    ಯದಿ (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    ಶೂನ್ಯ *
get_register(
    ಪೂರ್ಣಾಙ್ಕ		name,
    ಪೂರ್ಣಾಙ್ಕ		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    ಪೂರ್ಣಾಙ್ಕ		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	ಯದಿ (name == '*' && clip_star.available)
	{
	    ಯದಿ (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	ಯದಿ (name == '+' && clip_plus.available)
	{
	    ಯದಿ (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    ಯದಿ (reg == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)ರಿಕ್ತ;

    *reg = *y_current;
    ಯದಿ (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	ಯದಿ (reg->y_size == 0 || y_current->y_array == ರಿಕ್ತ)
	    reg->y_array = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	ಯದಿ (reg->y_array != ರಿಕ್ತ)
	{
	    ಪ್ರತಿ (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		ಯದಿ (reg->y_array[i].string == ರಿಕ್ತ)
		{
		    // The allocation failed so clean up and exit
		    ಯಾವತ್ (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)ರಿಕ್ತ;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    ಅನ್ಯಥಾ
	y_current->y_array = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    ಶೂನ್ಯ
put_register(ಪೂರ್ಣಾಙ್ಕ name, ಶೂನ್ಯ *reg)
{
    get_yank_register(name, 0);
    free_yank_all();
    *y_current = *(yankreg_T *)reg;
    vim_free(reg);

#ifdef FEAT_CLIPBOARD
    // Send text written to clipboard register to the clipboard.
    may_set_selection();
#endif
}

#ಯದಿ defined(FEAT_CLIPBOARD)
    ಶೂನ್ಯ
free_register(ಶೂನ್ಯ *reg)
{
    yankreg_T tmp;

    tmp = *y_current;
    *y_current = *(yankreg_T *)reg;
    free_yank_all();
    vim_free(reg);
    *y_current = tmp;
}
#endif

/*
 * return TRUE if the current yank register has type MLINE
 */
    ಪೂರ್ಣಾಙ್ಕ
yank_register_mline(ಪೂರ್ಣಾಙ್ಕ regname)
{
    ಯದಿ (regname != 0 && !valid_yank_reg(regname, FALSE))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (regname == '_')		// black hole is always empty
	ಪ್ರತ್ಯಾಗಮ FALSE;
    get_yank_register(regname, FALSE);
    ಪ್ರತ್ಯಾಗಮ (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
do_record(ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u	    *p;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    ಪೂರ್ಣಾಙ್ಕ		    retval;

    ಯದಿ (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	ಯದಿ (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	ಅನ್ಯಥಾ
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    ಅನ್ಯಥಾ			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	ಸಂದೇಶ("");
	p = get_recorded();
	ಯದಿ (p == ರಿಕ್ತ)
	    retval = FAIL;
	ಅನ್ಯಥಾ
	{
	    // Remove escaping for CSI and K_SPECIAL in multi-byte chars.
	    vim_unescape_csi(p);

	    // We don't want to change the default register here, so save and
	    // restore the current register name.
	    old_y_previous = y_previous;
	    old_y_current = y_current;

	    retval = stuff_yank(regname, p);

	    y_previous = old_y_previous;
	    y_current = old_y_current;
	}
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
stuff_yank(ಪೂರ್ಣಾಙ್ಕ regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    ಯದಿ (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    ಯದಿ (y_append && y_current->y_array != ರಿಕ್ತ)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	ಯದಿ (tmp == ರಿಕ್ತ)
	{
	    vim_free(p);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    ಅನ್ಯಥಾ
    {
	free_yank_all();
	ಯದಿ ((y_current->y_array = ALLOC_ONE(string_T)) == ರಿಕ್ತ)
	{
	    vim_free(p);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Last executed register (@ command)
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ execreg_lastc = NUL;

#ಯದಿ defined(FEAT_VIMINFO)
    ಪೂರ್ಣಾಙ್ಕ
get_execreg_lastc(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ execreg_lastc;
}

    ಶೂನ್ಯ
set_execreg_lastc(ಪೂರ್ಣಾಙ್ಕ lastc)
{
    execreg_lastc = lastc;
}
#endif

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
execreg_ಸಾಲು_continuation(string_T *ಸಾಲುಗಳು, long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    ಪೂರ್ಣಾಙ್ಕ		j;
    char_u	*str;

    ga_init2(&ga, ಪರಿಮಾಣ(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    ಯಾವತ್ (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(ಸಾಲುಗಳು[cmd_start].string);
	ಯದಿ (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    ಭಙ್ಗ;
    }

    // join all the lines
    tmp = &ಸಾಲುಗಳು[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    ಪ್ರತಿ (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &ಸಾಲುಗಳು[j];
	p = skipwhite(tmp->string);
	ಯದಿ (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    ಯದಿ (ga.ga_len > 400)
	    {
		ಯದಿ (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		ಅನ್ಯಥಾ
		    ga.ga_growsize = ga.ga_len;
	    }
	    p++;
	    ga_concat_len(&ga, p, (tmp->string + tmp->length) - p);
	}
    }
    ga_append(&ga, NUL);
    str = vim_strnsave(ga.ga_data, ga.ga_len);
    ga_clear(&ga);

    *idx = cmd_start;
    ಪ್ರತ್ಯಾಗಮ str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
do_execreg(
    ಪೂರ್ಣಾಙ್ಕ	    regname,
    ಪೂರ್ಣಾಙ್ಕ	    colon,		// insert ':' before each line
    ಪೂರ್ಣಾಙ್ಕ	    addcr,		// always add '\n' to end of line
    ಪೂರ್ಣಾಙ್ಕ	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    ಪೂರ್ಣಾಙ್ಕ		remap;

    // repeat previous one
    ಯದಿ (regname == '@')
    {
	ಯದಿ (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    ಯದಿ (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    ಯದಿ (regname == '_')
	ಪ್ರತ್ಯಾಗಮ OK;

    // use last command line
    ಯದಿ (regname == ':')
    {
	ಯದಿ (last_cmdline == ರಿಕ್ತ)
	{
	    emsg(_(e_no_previous_command_ಸಾಲು));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	// don't keep the cmdline containing @:
	VIM_CLEAR(new_last_cmdline);
	// Escape all control characters with a CTRL-V
	p = vim_strsave_escaped_ext(last_cmdline,
		    (char_u *)"\001\002\003\004\005\006\007"
			  "\010\011\012\013\014\015\016\017"
			  "\020\021\022\023\024\025\026\027"
			  "\030\031\032\033\034\035\036\037",
		    Ctrl_V, FALSE);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    ಯದಿ (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    ಅನ್ಯಥಾ
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    ಅನ್ಯಥಾ ಯದಿ (regname == '=')
    {
	p = get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು();
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    ಅನ್ಯಥಾ ಯದಿ (regname == '.')		// use last inserted text
    {
	p = get_last_ಸೇರಿಸು_save();
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    emsg(_(e_no_inserted_text_yet));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    ಅನ್ಯಥಾ
    {
	get_yank_register(regname, FALSE);
	ಯದಿ (y_current->y_array == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	ಪ್ರತಿ (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    ಪೂರ್ಣಾಙ್ಕ	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    ಯದಿ (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		ಯದಿ (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    ಯದಿ (colon && i > 0)
	    {
		p = skipwhite(str);
		ಯದಿ (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_ಸಾಲು_continuation(y_current->y_array, &i);
		    ಯದಿ (str == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    ಯದಿ (free_str)
		vim_free(str);
	    ಯದಿ (escaped == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    ಯದಿ (retval == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
put_reedit_in_typebuf(ಪೂರ್ಣಾಙ್ಕ silent)
{
    ಯದಿ (restart_edit == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    ಯದಿ (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    ಅನ್ಯಥಾ ಯದಿ (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    ಅನ್ಯಥಾ ಯದಿ (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    ಯದಿ (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_in_typebuf(
    char_u	*s,
    ಪೂರ್ಣಾಙ್ಕ		esc,
    ಪೂರ್ಣಾಙ್ಕ		colon,	    // add ':' before the line
    ಪೂರ್ಣಾಙ್ಕ		silent)
{
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;

    put_reedit_in_typebuf(silent);
    ಯದಿ (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    ಯದಿ (retval == OK)
    {
	char_u	*p;

	ಯದಿ (esc)
	    p = vim_strsave_escape_csi(s);
	ಅನ್ಯಥಾ
	    p = s;
	ಯದಿ (p == ರಿಕ್ತ)
	    retval = FAIL;
	ಅನ್ಯಥಾ
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	ಯದಿ (esc)
	    vim_free(p);
    }
    ಯದಿ (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಙ್ಕ
ಸೇರಿಸು_reg(
    ಪೂರ್ಣಾಙ್ಕ		regname,
    ಪೂರ್ಣಾಙ್ಕ		literally_arg)	// insert literally, not as if typed
{
    long	i;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    char_u	*arg;
    ಪೂರ್ಣಾಙ್ಕ		allocated;
    ಪೂರ್ಣಾಙ್ಕ		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    ಯದಿ (got_int)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // check for valid regname
    ಯದಿ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ಪ್ರತ್ಯಾಗಮ FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    ಯದಿ (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    ಅನ್ಯಥಾ ಯದಿ (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	ಯದಿ (arg == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	stuffescaped(arg, literally);
	ಯದಿ (allocated)
	    vim_free(arg);
    }
    ಅನ್ಯಥಾ				// name or number register
    {
	ಯದಿ (get_yank_register(regname, FALSE))
	    literally = TRUE;
	ಯದಿ (y_current->y_array == ರಿಕ್ತ)
	    retval = FAIL;
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (i = 0; i < y_current->y_size; ++i)
	    {
		ಯದಿ (regname == '-' && y_current->y_type == MCHAR)
		{
		    ಪೂರ್ಣಾಙ್ಕ dir = BACKWARD;
		    ಯದಿ ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			ಯದಿ (u_save_cursor() == FAIL)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			ಯದಿ (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, ರಿಕ್ತ, dir, 1L, PUT_CURSEND);
		}
		ಅನ್ಯಥಾ
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    ಯದಿ (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    ಪೂರ್ಣಾಙ್ಕ
get_spec_reg(
    ಪೂರ್ಣಾಙ್ಕ		regname,
    char_u	**argp,
    ಪೂರ್ಣಾಙ್ಕ		*allocated,	// return: TRUE when value was allocated
    ಪೂರ್ಣಾಙ್ಕ		errmsg)		// give error message when failing
{
    ಪೂರ್ಣಾಙ್ಕ		cnt;

    *argp = ರಿಕ್ತ;
    *allocated = FALSE;
    ವಿಕಲ್ಪ (regname)
    {
	ಪಕ್ಷ '%':		// file name
	    ಯದಿ (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಪಕ್ಷ '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    ಪ್ರತ್ಯಾಗಮ TRUE;

#ifdef FEAT_EVAL
	ಪಕ್ಷ '=':		// result of expression
	    *argp = get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು();
	    *allocated = TRUE;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
#endif

	ಪಕ್ಷ ':':		// last command line
	    ಯದಿ (last_cmdline == ರಿಕ್ತ && errmsg)
		emsg(_(e_no_previous_command_ಸಾಲು));
	    *argp = last_cmdline;
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಪಕ್ಷ '/':		// last search-pattern
	    ಯದಿ (last_ಹುಡುಕು_pat() == ರಿಕ್ತ && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_ಹುಡುಕು_pat();
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಪಕ್ಷ '.':		// last inserted text
	    *argp = get_last_ಸೇರಿಸು_save();
	    *allocated = TRUE;
	    ಯದಿ (*argp == ರಿಕ್ತ && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಪಕ್ಷ Ctrl_F:		// Filename under cursor
	ಪಕ್ಷ Ctrl_P:		// Path under cursor, expand via "path"
	    ಯದಿ (!errmsg)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, ರಿಕ್ತ);
	    *allocated = TRUE;
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಪಕ್ಷ Ctrl_W:		// word under cursor
	ಪಕ್ಷ Ctrl_A:		// WORD (mnemonic All) under cursor
	    ಯದಿ (!errmsg)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : ರಿಕ್ತ;
	    *allocated = TRUE;
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಪಕ್ಷ Ctrl_L:		// Line under cursor
	    ಯದಿ (!errmsg)
		ಪ್ರತ್ಯಾಗಮ FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಪಕ್ಷ '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಪೂರ್ಣಾಙ್ಕ
cmdline_paste_reg(
    ಪೂರ್ಣಾಙ್ಕ regname,
    ಪೂರ್ಣಾಙ್ಕ literally_arg,	// Insert text literally instead of "as typed"
    ಪೂರ್ಣಾಙ್ಕ remcr)		// don't add CR characters
{
    long	i;
    ಪೂರ್ಣಾಙ್ಕ		literally = literally_arg;

    ಯದಿ (get_yank_register(regname, FALSE))
	literally = TRUE;
    ಯದಿ (y_current->y_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತಿ (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	ಯದಿ ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	ಯದಿ (got_int)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    ಶೂನ್ಯ
shift_delete_registers(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    ಪ್ರತಿ (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    ಯದಿ (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = ರಿಕ್ತ;		// set register one to empty
}

#ಯದಿ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಶೂನ್ಯ
add_regtype_to_dict(ಪೂರ್ಣಾಙ್ಕ regname, dict_T *dict, char_u *buf, ಪೂರ್ಣಾಙ್ಕ bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    ವಿಕಲ್ಪ (get_reg_type(regname, &reglen))
    {
	ಪಕ್ಷ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ MBLOCK:
	    buflen = vim_snprintf_safelen((ವರ್ಣ *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    buf[0] = NUL;
	    buflen = 0;
	    ಭಙ್ಗ;
    }
    (ಶೂನ್ಯ)dict_add_string_len(dict, "regtype", buf, (ಪೂರ್ಣಾಙ್ಕ)buflen);
}

    ಶೂನ್ಯ
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    ಪೂರ್ಣಾಙ್ಕ		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // yanked text contents
    ಪ್ರತಿ (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (ಪೂರ್ಣಾಙ್ಕ)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    ಯದಿ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ಶೂನ್ಯ)dict_add_string_len(v_event, "regname", buf, (ಪೂರ್ಣಾಙ್ಕ)buflen);

    // motion type: inclusive or exclusive
    (ಶೂನ್ಯ)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (ಶೂನ್ಯ)dict_add_string_len(v_event, "operator", buf, (ಪೂರ್ಣಾಙ್ಕ)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, ಪರಿಮಾಣ(buf));

    // selection type - visual or not
    (ಶೂನ್ಯ)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_ವಸ್ತುಗಳು_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
put_do_autocmd(
	ಪೂರ್ಣಾಙ್ಕ	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *ಸೇರಿಸು,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	ಪೂರ್ಣಾಙ್ಕ	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    ಸ್ಥಿರ bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    ಪೂರ್ಣಾಙ್ಕ		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ಯದಿ (recursive || regname == '_')
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (regname != '.' && ಸೇರಿಸು == ರಿಕ್ತ
	    && reg == ರಿಕ್ತ)
	// Can happen when pasting text in normal mode in a terminal buffer
	ಪ್ರತ್ಯಾಗಮ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ಯದಿ (regname == '.')
    {
	ಯದಿ (last_ಸೇರಿಸು_ga.ga_data != ರಿಕ್ತ)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_ಸೇರಿಸು_ga.ga_data,
		    (ಪೂರ್ಣಾಙ್ಕ)last_ಸೇರಿಸು_ga.ga_len);
    }
    ಅನ್ಯಥಾ ಯದಿ (ಸೇರಿಸು != ರಿಕ್ತ)
    {
	list_append_string(list, ಸೇರಿಸು->string, (ಪೂರ್ಣಾಙ್ಕ)ಸೇರಿಸು->length);
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (ಪೂರ್ಣಾಙ್ಕ)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    ಯದಿ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ಶೂನ್ಯ)dict_add_string_len(v_event, "regname", buf, (ಪೂರ್ಣಾಙ್ಕ)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (ಶೂನ್ಯ)dict_add_string_len(v_event, "operator", buf, (ಪೂರ್ಣಾಙ್ಕ)buflen);

    add_regtype_to_dict(regname, v_event, buf, ಪರಿಮಾಣ(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (ಶೂನ್ಯ)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_ವಸ್ತುಗಳು_ro(v_event);

    recursive = true;
    textlock++;
    ಯದಿ (post)
	apply_autocmds(EVENT_TEXTPUTPOST, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    ಅನ್ಯಥಾ
	apply_autocmds(EVENT_TEXTPUTPRE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    ಶೂನ್ಯ
init_yank(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_REGISTERS; ++i)
	y_regs[i].y_array = ರಿಕ್ತ;
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
clear_registers(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	ಯದಿ (y_current->y_array != ರಿಕ್ತ)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_yank(long n)
{
    ಯದಿ (y_current->y_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    long	    i;

    ಪ್ರತಿ (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    ಶೂನ್ಯ
free_yank_all(ಶೂನ್ಯ)
{
    free_yank(y_current->y_size);
}

/*
 * Yank the text between "oap->start" and "oap->end" into a yank register.
 * If we are to append (uppercase register), we first yank into a new yank
 * register and then concatenate the old and the new one (so we keep the old
 * one in case of out-of-memory).
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
op_yank(oparg_T *oap, ಪೂರ್ಣಾಙ್ಕ deleting, ಪೂರ್ಣಾಙ್ಕ mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    ಪೂರ್ಣಾಙ್ಕ			yanktype = oap->motion_type;
    long		yanklines = oap->ಸಾಲು_ಎಣಿಕೆ;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    ಸಂರಚನಾ block_def	bd;

				    // check for read-only register
    ಯದಿ (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (oap->regname == '_')	    // black hole: nothing to do
	ಪ್ರತ್ಯಾಗಮ OK;

#ifdef FEAT_CLIPBOARD
    ಯದಿ ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	ಸಂದೇಶ_warn_missing_clipboard();
    }
#endif

    ಯದಿ (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    ಯದಿ (y_append && y_current->y_array != ರಿಕ್ತ)
	y_current = &newreg;
    ಅನ್ಯಥಾ
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    ಯದಿ (       oap->motion_type == MCHAR
	    && oap->start.col == 0
	    && !oap->inclusive
	    && (!oap->is_VIsual || *p_sel == 'o')
	    && !oap->block_mode
	    && oap->end.col == 0
	    && yanklines > 1)
    {
	yanktype = MLINE;
	--yankendlnum;
	--yanklines;
    }

    y_current->y_size = yanklines;
    y_current->y_type = yanktype;   // set the yank register type
    y_current->y_width = 0;
    y_current->y_array = lalloc_clear(ಪರಿಮಾಣ(string_T) * yanklines, TRUE);
    ಯದಿ (y_current->y_array == ರಿಕ್ತ)
    {
	y_current = curr;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    ಯದಿ (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	ಯದಿ (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    ಪ್ರತಿ ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	ವಿಕಲ್ಪ (y_current->y_type)
	{
	    ಪಕ್ಷ MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		ಯದಿ (yank_copy_ಸಾಲು(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    ಗಚ್ಛ fail;
		ಭಙ್ಗ;

	    ಪಕ್ಷ MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		ಯದಿ ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == ರಿಕ್ತ)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    ಗಚ್ಛ fail;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ MCHAR:
		{
		    ಪೂರ್ಣಾಙ್ಕ tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(bd.textstart);
		    ಯದಿ (tmp < bd.textlen)
			bd.textlen = tmp;

		    ಯದಿ (yank_copy_ಸಾಲು(&bd, y_idx, FALSE) == FAIL)
			ಗಚ್ಛ fail;
		    ಭಙ್ಗ;
		}
		// NOTREACHED
	}
    }

    ಯದಿ (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	ಯದಿ (new_ptr == ರಿಕ್ತ)
	    ಗಚ್ಛ fail;
	ಪ್ರತಿ (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	ಯದಿ (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	ಯದಿ (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == ರಿಕ್ತ)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    ಯದಿ (pnew == ರಿಕ್ತ)
	    {
		y_idx = y_current->y_size - 1;
		ಗಚ್ಛ fail;
	    }

	    --j;
	    STRCPY(pnew, curr->y_array[j].string);
	    STRCPY(pnew + curr->y_array[j].length, y_current->y_array[0].string);
	    vim_free(curr->y_array[j].string);
	    curr->y_array[j].string = pnew;
	    curr->y_array[j].length = curr->y_array[j].length + y_current->y_array[0].length;
	    ++j;
	    VIM_CLEAR_STRING(y_current->y_array[0]);
	    y_idx = 1;
	}
	ಅನ್ಯಥಾ
	    y_idx = 0;
	ಯಾವತ್ (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    ಯದಿ (mess)			// Display message about yank?
    {
	ಯದಿ (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	ಯದಿ (yanklines > p_report)
	{
	    ವರ್ಣ namebuf[100];

	    ಯದಿ (oap->regname == NUL)
		*namebuf = NUL;
	    ಅನ್ಯಥಾ
		vim_snprintf(namebuf, ಪರಿಮಾಣ(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    ಯದಿ (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    ಅನ್ಯಥಾ
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    ಯದಿ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	ಯದಿ (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	ಯದಿ (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    ಯದಿ (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	ಯದಿ (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    ಯದಿ (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	ಯದಿ (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	ಯದಿ (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    ಯದಿ (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# ಯದಿ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	ಯದಿ (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    ಯದಿ (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#ಯದಿ defined(FEAT_EVAL)
    ಯದಿ (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    ಪ್ರತ್ಯಾಗಮ OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
yank_copy_ಸಾಲು(ಸಂರಚನಾ block_def *bd, long y_idx, ಪೂರ್ಣಾಙ್ಕ exclude_trailing_space)
{
    char_u	*pnew;

    ಯದಿ (exclude_trailing_space)
	bd->endspaces = 0;
    ಯದಿ ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    ಯದಿ (exclude_trailing_space)
    {
	ಪೂರ್ಣಾಙ್ಕ s = bd->textlen + bd->endspaces;

	ಯಾವತ್ (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    ಪ್ರತ್ಯಾಗಮ OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    ಪರಿಮಾಣ(string_T) * y_current->y_size, TRUE);
    ಯದಿ (y_current->y_array == ರಿಕ್ತ)
	y_current->y_size = 0;
    ಅನ್ಯಥಾ
	ಪ್ರತಿ (j = 0; j < y_current->y_size; ++j)
	{
	    ಯದಿ ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == ರಿಕ್ತ)
	    {
		free_yank(j);
		y_current->y_size = 0;
		ಭಙ್ಗ;
	    }
	    y_current->y_array[j].length = curr->y_array[j].length;
	}
    y_current = curr;
}
#endif

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
    ಪೂರ್ಣಾಙ್ಕ		regname,
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_result,	// result for regname "=" when compiled
    ಪೂರ್ಣಾಙ್ಕ		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    ಪೂರ್ಣಾಙ್ಕ		yanklen;
    ಪೂರ್ಣಾಙ್ಕ		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    ಪೂರ್ಣಾಙ್ಕ		y_type;
    long	y_size;
    ಪೂರ್ಣಾಙ್ಕ		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = ರಿಕ್ತ;
    yankreg_T	*y_current_used = ರಿಕ್ತ;
    long	nr_ಸಾಲುಗಳು = 0;
    string_T	ಸೇರಿಸು_string;
    ಪೂರ್ಣಾಙ್ಕ		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned ಪೂರ್ಣಾಙ್ಕ cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(ಶೂನ್ಯ)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    ಯದಿ (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    ಯದಿ (regname == '.')
    {
	ಯದಿ (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	ಯದಿ (has_textput_events)
	    add_last_ಸೇರಿಸು++;
#endif

	(ಶೂನ್ಯ)stuff_inserted((dir == FORWARD ? (ಎಣಿಕೆ == -1 ? 'o' : 'a') :
				    (ಎಣಿಕೆ == -1 ? 'O' : 'i')), ಎಣಿಕೆ, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	ಯದಿ (has_textputpre())
	    put_do_autocmd('.', ರಿಕ್ತ, ರಿಕ್ತ, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	ಯದಿ (has_textputpost())
	    put_do_autocmd('.', ರಿಕ್ತ, ರಿಕ್ತ, true, dir);

	ಯದಿ (has_textput_events && --add_last_ಸೇರಿಸು == 0)
	    ga_clear(&last_ಸೇರಿಸು_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	ಯದಿ ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	ಪ್ರತ್ಯಾಗಮ;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    ಸೇರಿಸು_string.string = ರಿಕ್ತ;
    ಯದಿ (regname == '=' && ಅಭಿವ್ಯಕ್ತಿ_result != ರಿಕ್ತ)
	ಸೇರಿಸು_string.string = ಅಭಿವ್ಯಕ್ತಿ_result;
    ಅನ್ಯಥಾ ಯದಿ (get_spec_reg(regname, &ಸೇರಿಸು_string.string, &allocated, TRUE)
		&& ಸೇರಿಸು_string.string == ರಿಕ್ತ)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    ಯದಿ (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	ಗಚ್ಛ end;

    ಯದಿ (ಸೇರಿಸು_string.string != ರಿಕ್ತ)
    {
	ಸೇರಿಸು_string.length = STRLEN(ಸೇರಿಸು_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	ಯದಿ (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    ಪ್ರತಿ (;;)
	    {
		y_size = 0;
		ptr = ಸೇರಿಸು_string.string;
		ptrlen = ಸೇರಿಸು_string.length;
		ಯಾವತ್ (ptr != ರಿಕ್ತ)
		{
		    ಯದಿ (y_array != ರಿಕ್ತ)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    ಯದಿ (tmp == ರಿಕ್ತ)
		    {
			ಯದಿ (y_array != ರಿಕ್ತ)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (y_array != ರಿಕ್ತ)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			ಯದಿ (*tmp == NUL)
			{
			    y_type = MLINE;
			    ಭಙ್ಗ;
			}
		    }
		    ptr = tmp;
		}
		ಯದಿ (y_array != ರಿಕ್ತ)
		    ಭಙ್ಗ;
		y_array = ALLOC_MULT(string_T, y_size);
		ಯದಿ (y_array == ರಿಕ್ತ)
		    ಗಚ್ಛ end;
	    }
	}
	ಅನ್ಯಥಾ
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &ಸೇರಿಸು_string;
	}
#ifdef FEAT_EVAL
	ಯದಿ (has_textputpre())
	    put_do_autocmd(regname, ರಿಕ್ತ, &ಸೇರಿಸು_string, false, dir);
#endif
    }
    ಅನ್ಯಥಾ
    {
#ifdef FEAT_EVAL
	ಯದಿ (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, ರಿಕ್ತ, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    ಯದಿ (y_type == MLINE)
    {
	ಯದಿ (flags & PUT_ಸಾಲು_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    ಯದಿ (u_save_cursor() == FAIL)
		ಗಚ್ಛ end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    ಯದಿ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    ಯದಿ (ptr == ರಿಕ್ತ)
		ಗಚ್ಛ end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    ಯದಿ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    ಯದಿ (ptr == ರಿಕ್ತ)
		ಗಚ್ಛ end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_ಸಾಲುಗಳು;
	    dir = FORWARD;
	}
	ಯದಿ (flags & PUT_ಸಾಲು_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    ಯದಿ (flags & PUT_ಸಾಲು)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    ಯದಿ (y_size == 0 || y_array == ರಿಕ್ತ)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	ಗಚ್ಛ end;
    }

    ಯದಿ (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	ಯದಿ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
	ಯದಿ (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    ಗಚ್ಛ end;
    }
    ಅನ್ಯಥಾ ಯದಿ (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	ಯದಿ (dir == BACKWARD)
	    (ಶೂನ್ಯ)hasFolding(lnum, &lnum, ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    (ಶೂನ್ಯ)hasFolding(lnum, ರಿಕ್ತ, &lnum);
#endif
	ಯದಿ (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	ಯದಿ ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    ಗಚ್ಛ end;
#ifdef FEAT_FOLDING
	ಯದಿ (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	ಅನ್ಯಥಾ
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (u_save_cursor() == FAIL)
	ಗಚ್ಛ end;

    ಯದಿ (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	ಯದಿ (gchar_cursor() == TAB)
	{
	    ಪೂರ್ಣಾಙ್ಕ viscol = getviscol();
	    ಪೂರ್ಣಾಙ್ಕ ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    ಯದಿ (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#ಅನ್ಯಥಾ
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    ಅನ್ಯಥಾ
		curwin->w_cursor.coladd = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    ಯದಿ (y_type == MBLOCK)
    {
	ಪೂರ್ಣಾಙ್ಕ	delcount;
	ಪೂರ್ಣಾಙ್ಕ	incr = 0;
	ಸಂರಚನಾ block_def bd;
	long	j;
	ಪೂರ್ಣಾಙ್ಕ	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	ಯದಿ (dir == FORWARD && c != NUL)
	{
	    ಯದಿ (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, ರಿಕ್ತ, &endcol2, 0);
	    ಅನ್ಯಥಾ
		getvcol(curwin, &curwin->w_cursor, ರಿಕ್ತ, ರಿಕ್ತ, &col, 0);

	    ಯದಿ (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    ಅನ್ಯಥಾ
		ಯದಿ (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	ಅನ್ಯಥಾ
	    getvcol(curwin, &curwin->w_cursor, &col, ರಿಕ್ತ, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	ಯದಿ (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    ಯದಿ (dir == FORWARD && c == NUL)
		++col;
	    ಯದಿ (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    ಯದಿ (c == TAB)
	    {
		ಯದಿ (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		ಯದಿ (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	ಪ್ರತಿ (i = 0; i < y_size; ++i)
	{
	    ಪೂರ್ಣಾಙ್ಕ		    spaces = 0;
	    ವರ್ಣ	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    ಯದಿ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಯದಿ (ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    ಭಙ್ಗ;
		++nr_ಸಾಲುಗಳು;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    ಯಾವತ್ (cts.cts_vcol < col && *cts.cts_ptr != NUL)
	    {
		// Count a tab for what it's worth (if list mode not on)
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    vcol = cts.cts_vcol;
	    ptr = cts.cts_ptr;
	    bd.textcol = (colnr_T)(ptr - oldp);
	    clear_chartabsize_arg(&cts);

	    shortline = (vcol < col) || (vcol == col && !*ptr) ;

	    ಯದಿ (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    ಅನ್ಯಥಾ ಯದಿ (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		ಯದಿ (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		ಯದಿ (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (ಪೂರ್ಣಾಙ್ಕ)y_array[i].length;

	    ಯದಿ ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		ಯಾವತ್ (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		ಯದಿ (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    ಯದಿ (yanklen + spaces != 0
		     && ಎಣಿಕೆ > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		ಭಙ್ಗ;
	    }

	    totlen = ಎಣಿಕೆ * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    ಯದಿ (newp == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    ಪ್ರತಿ (j = 0; j < ಎಣಿಕೆ; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		ಯದಿ ((j < ಎಣಿಕೆ - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		ಅನ್ಯಥಾ
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
	    ಯದಿ (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_ಸಾಲುಗಳು(lnum, 0, curwin->w_cursor.lnum - nr_ಸಾಲುಗಳು, nr_ಸಾಲುಗಳು);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	ಯದಿ (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	ಯದಿ (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    ಯದಿ (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	ಅನ್ಯಥಾ
	    curwin->w_cursor.lnum = lnum;
    }
    ಅನ್ಯಥಾ
    {
	pos_T	new_cursor;

	yanklen = (ಪೂರ್ಣಾಙ್ಕ)y_array[0].length;

	// Character or Line mode
	ಯದಿ (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    ಯದಿ (dir == FORWARD && gchar_cursor() != NUL)
	    {
		ಯದಿ (has_mbyte)
		{
		    ಪೂರ್ಣಾಙ್ಕ bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    ಯದಿ (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ++col;
		    ಯದಿ (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	ಅನ್ಯಥಾ ಯದಿ (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	ಯದಿ (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    ಪೂರ್ಣಾಙ್ಕ		first_byte_off = 0;

	    ಯದಿ (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		ಯದಿ (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		ಯದಿ (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, ರಿಕ್ತ, &vcol, ರಿಕ್ತ, 0);
		}
	    }

	    ಯದಿ (ಎಣಿಕೆ == 0 || yanklen == 0)
	    {
		ಯದಿ (VIsual_active)
		    lnum = end_lnum;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (ಎಣಿಕೆ > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    ಅನ್ಯಥಾ
	    {
		totlen = ಎಣಿಕೆ * yanklen;
		ಕುರು
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    ಯದಿ (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			ಯದಿ (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			ಅನ್ಯಥಾ
			    col = MAXCOL;
		    }
		    ಯದಿ (VIsual_active && col > oldlen)
		    {
			lnum++;
			ಅನುವರ್ತ;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    ಯದಿ (newp == ರಿಕ್ತ)
			ಗಚ್ಛ end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
		    {
			mch_memmove(ptr, y_array[0].string, (size_t)yanklen);
			ptr += yanklen;
		    }
		    mch_memmove(ptr, oldp + col, (size_t)(oldlen - col) + 1);	    // +1 for NUL

		    // compute the byte offset for the last character
		    first_byte_off = mb_head_off(newp, ptr - 1);

		    // Note: this may free "newp"
		    ml_replace(lnum, newp, FALSE);

		    inserted_bytes(lnum, col, totlen);

		    // Place cursor on last putted char.
		    ಯದಿ (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    ಯದಿ (VIsual_active)
			lnum++;
		} ಯಾವತ್ (VIsual_active && lnum <= end_lnum);

		ಯದಿ (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    ಯದಿ (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    ಅನ್ಯಥಾ
		curwin->w_cursor.col -= first_byte_off;
	}
	ಅನ್ಯಥಾ
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    ಪೂರ್ಣಾಙ್ಕ		indent;
	    ಪೂರ್ಣಾಙ್ಕ		orig_indent = 0;
	    ಪೂರ್ಣಾಙ್ಕ		indent_diff = 0;	// init for gcc
	    ಪೂರ್ಣಾಙ್ಕ		first_indent = TRUE;
	    ಪೂರ್ಣಾಙ್ಕ		lendiff = 0;
	    long	cnt;

	    ಯದಿ (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    ಪ್ರತಿ (cnt = 1; cnt <= ಎಣಿಕೆ; ++cnt)
	    {
		i = 0;
		ಯದಿ (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (ಪೂರ್ಣಾಙ್ಕ)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    ಯದಿ (newp == ರಿಕ್ತ)
			ಗಚ್ಛ error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    ಯದಿ (newp == ರಿಕ್ತ)
			ಗಚ್ಛ error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		ಪ್ರತಿ (; i < y_size; ++i)
		{
		    ಯದಿ (y_type != MCHAR || i < y_size - 1)
		    {
			ಯದಿ (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    ಗಚ್ಛ error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_ಸಾಲುಗಳು;
		    ಯದಿ (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			ಯದಿ (cnt == ಎಣಿಕೆ && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			ಯದಿ (*ptr == '#' && preprocs_ಎಡ())
			    indent = 0;     // Leave # lines at start
			ಅನ್ಯಥಾ ಯದಿ (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			ಅನ್ಯಥಾ ಯದಿ (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			ಅನ್ಯಥಾ ಯದಿ ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(ಶೂನ್ಯ)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			ಯದಿ (cnt == ಎಣಿಕೆ && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		ಯದಿ (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    ಯದಿ (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		ಯದಿ (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_ಸಾಲುಗಳು, 0L);

	    // note changed text for displaying and folding
	    ಯದಿ (y_type == MCHAR)
		changed_ಸಾಲುಗಳು(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_ಸಾಲುಗಳು);
	    ಅನ್ಯಥಾ
		changed_ಸಾಲುಗಳು(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_ಸಾಲುಗಳು);
	    ಯದಿ (y_current_used != ರಿಕ್ತ && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		ಗಚ್ಛ end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    ಯದಿ (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		ಯದಿ (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    ಅನ್ಯಥಾ
		curbuf->b_op_end.col = 0;

	    ಯದಿ (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		ಯದಿ (y_type == MLINE)
		{
		    ಯದಿ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		    ಅನ್ಯಥಾ
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		ಅನ್ಯಥಾ
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    ಯದಿ (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		ಯದಿ (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ಅನ್ಯಥಾ	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_ಸಾಲುಗಳು);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    ಪೂರ್ಣಾಙ್ಕ len = ml_get_curline_len();
    ಯದಿ (curwin->w_cursor.col > len)
    {
	ಯದಿ (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    ಯದಿ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    ಯದಿ (has_textputpost())
    {
	ಯದಿ (ಸೇರಿಸು_string.string == ರಿಕ್ತ)
	    put_do_autocmd(regname, y_current, ರಿಕ್ತ, true, dir);
	ಅನ್ಯಥಾ
	    put_do_autocmd(regname, ರಿಕ್ತ, &ಸೇರಿಸು_string, true, dir);
    }
#endif

    ಯದಿ (allocated)
	vim_free(ಸೇರಿಸು_string.string);
    ಯದಿ (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_register_name(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ)
{
    ಯದಿ (ಅಂಕೆ == -1)
	ಪ್ರತ್ಯಾಗಮ '"';
    ಅನ್ಯಥಾ ಯದಿ (ಅಂಕೆ < 10)
	ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ + '0';
    ಅನ್ಯಥಾ ಯದಿ (ಅಂಕೆ == DELETION_REGISTER)
	ಪ್ರತ್ಯಾಗಮ '-';
#ಯದಿ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    ಅನ್ಯಥಾ ಯದಿ (ಅಂಕೆ == STAR_REGISTER)
	ಪ್ರತ್ಯಾಗಮ '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    ಅನ್ಯಥಾ ಯದಿ (clipmethod == CLIPMETHOD_PROVIDER && ಅಂಕೆ == REAL_PLUS_REGISTER)
	ಪ್ರತ್ಯಾಗಮ '+';
    ಅನ್ಯಥಾ ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER && ಅಂಕೆ == PLUS_REGISTER)
	ಪ್ರತ್ಯಾಗಮ '+';
#endif
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ + 'a' - 10;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_unname_register(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ y_previous == ರಿಕ್ತ ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    ಶೂನ್ಯ
ex_display(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    ಪೂರ್ಣಾಙ್ಕ		name;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		clen;
    ಪೂರ್ಣಾಙ್ಕ		type;
    string_T	ಸೇರಿಸು;

    silence_w23_w24_ಸಂದೇಶ++;
    ಯದಿ (arg != ರಿಕ್ತ && *arg == NUL)
	arg = ರಿಕ್ತ;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\nType Name Content"));
    ಪ್ರತಿ (i = -1; i < ಅಂಕೆ_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	ವಿಕಲ್ಪ (get_reg_type(name, ರಿಕ್ತ))
	{
	    ಪಕ್ಷ MLINE: type = 'l'; ಭಙ್ಗ;
	    ಪಕ್ಷ MCHAR: type = 'c'; ಭಙ್ಗ;
	    ಸ್ವಭಾವ:	type = 'b'; ಭಙ್ಗ;
	}
	ಯದಿ (arg != ರಿಕ್ತ && vim_strchr(arg, name) == ರಿಕ್ತ
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == ರಿಕ್ತ)
#endif
		)
	    ಅನುವರ್ತ;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (ಶೂನ್ಯ)may_get_selection(name);
#endif

	ಯದಿ (i == -1)
	{
	    ಯದಿ (y_previous != ರಿಕ್ತ)
		yb = y_previous;
	    ಅನ್ಯಥಾ
		yb = &(y_regs[0]);
	}
	ಅನ್ಯಥಾ
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	ಯದಿ (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != ರಿಕ್ತ &&
		    (yb == y_previous || yb == &y_regs[0])))
	    ಅನುವರ್ತ;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	ಯದಿ (yb->y_array != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ do_show = FALSE;

	    ಪ್ರತಿ (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    ಯದಿ (do_show || yb->y_size == 0)
	    {
		ಸಂದೇಶ_putchar('\n');
		ಸಂದೇಶ_puts("  ");
		ಸಂದೇಶ_putchar(type);
		ಸಂದೇಶ_puts("  ");
		ಸಂದೇಶ_putchar('"');
		ಸಂದೇಶ_putchar(name);
		ಸಂದೇಶ_puts("   ");

		n = (ಪೂರ್ಣಾಙ್ಕ)Columns - 11;
		ಪ್ರತಿ (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    ಯದಿ (j)
		    {
			ಸಂದೇಶ_puts_attr("^J", attr);
			n -= 2;
		    }
		    ಪ್ರತಿ (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			ಸಂದೇಶ_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		ಯದಿ (n > 1 && yb->y_type == MLINE)
		    ಸಂದೇಶ_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    ಸೇರಿಸು = get_last_ಸೇರಿಸು();
    ಯದಿ ((p = ಸೇರಿಸು.string) != ರಿಕ್ತ
		  && (arg == ರಿಕ್ತ || vim_strchr(arg, '.') != ರಿಕ್ತ) && !got_int
						      && !message_filtered(p))
    {
	ಸಂದೇಶ_puts("\n  c  \".   ");
	dis_ಸಂದೇಶ(p, TRUE);
    }

    // display last command line
    ಯದಿ (last_cmdline != ರಿಕ್ತ && (arg == ರಿಕ್ತ || vim_strchr(arg, ':') != ರಿಕ್ತ)
			       && !got_int && !message_filtered(last_cmdline))
    {
	ಸಂದೇಶ_puts("\n  c  \":   ");
	dis_ಸಂದೇಶ(last_cmdline, FALSE);
    }

    // display current file name
    ಯದಿ (curbuf->b_fname != ರಿಕ್ತ
	    && (arg == ರಿಕ್ತ || vim_strchr(arg, '%') != ರಿಕ್ತ) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	ಸಂದೇಶ_puts("\n  c  \"%   ");
	dis_ಸಂದೇಶ(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    ಯದಿ ((arg == ರಿಕ್ತ || vim_strchr(arg, '#') != ರಿಕ್ತ) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	ಯದಿ (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    ಸಂದೇಶ_puts("\n  c  \"#   ");
	    dis_ಸಂದೇಶ(fname, FALSE);
	}
    }

    // display last search pattern
    ಯದಿ (last_ಹುಡುಕು_pat() != ರಿಕ್ತ
		 && (arg == ರಿಕ್ತ || vim_strchr(arg, '/') != ರಿಕ್ತ) && !got_int
				      && !message_filtered(last_ಹುಡುಕು_pat()))
    {
	ಸಂದೇಶ_puts("\n  c  \"/   ");
	dis_ಸಂದೇಶ(last_ಹುಡುಕು_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು != ರಿಕ್ತ && (arg == ರಿಕ್ತ || vim_strchr(arg, '=') != ರಿಕ್ತ)
				  && !got_int && !message_filtered(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು))
    {
	ಸಂದೇಶ_puts("\n  c  \"=   ");
	dis_ಸಂದೇಶ(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_ಸಂದೇಶ--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dis_ಸಂದೇಶ(
    char_u	*p,
    ಪೂರ್ಣಾಙ್ಕ		skip_esc)	    // if TRUE, ignore trailing ESC
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		l;

    n = (ಪೂರ್ಣಾಙ್ಕ)Columns - 6;
    ಯಾವತ್ (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ಸಂದೇಶ_outtrans_len(p, l);
	    p += l;
	}
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#ಯದಿ defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    ಶೂನ್ಯ
dnd_yank_drag_data(char_u *str, long len)
{
    yankreg_T *curr;

    curr = y_current;
    y_current = &y_regs[TILDE_REGISTER];
    free_yank_all();
    str_to_reg(y_current, MCHAR, str, len, 0L, FALSE);
    y_current = curr;
}
#endif


/*
 * Return the type of a register.
 * Used for getregtype()
 * Returns MAUTO for error.
 */
    char_u
get_reg_type(ಪೂರ್ಣಾಙ್ಕ regname, long *reglen)
{
    ವಿಕಲ್ಪ (regname)
    {
	ಪಕ್ಷ '%':		// file name
	ಪಕ್ಷ '#':		// alternate file name
	ಪಕ್ಷ '=':		// expression
	ಪಕ್ಷ ':':		// last command line
	ಪಕ್ಷ '/':		// last search-pattern
	ಪಕ್ಷ '.':		// last inserted text
	ಪಕ್ಷ Ctrl_F:		// Filename under cursor
	ಪಕ್ಷ Ctrl_P:		// Path under cursor, expand via "path"
	ಪಕ್ಷ Ctrl_W:		// word under cursor
	ಪಕ್ಷ Ctrl_A:		// WORD (mnemonic All) under cursor
	ಪಕ್ಷ '_':		// black hole: always empty
	    ಪ್ರತ್ಯಾಗಮ MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    ಯದಿ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ಪ್ರತ್ಯಾಗಮ MAUTO;

    get_yank_register(regname, FALSE);

    ಯದಿ (y_current->y_array != ರಿಕ್ತ)
    {
	ಯದಿ (reglen != ರಿಕ್ತ && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	ಪ್ರತ್ಯಾಗಮ y_current->y_type;
    }
    ಪ್ರತ್ಯಾಗಮ MAUTO;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    ಸ್ಥಿರ char_u *
getreg_wrap_one_ಸಾಲು(char_u *s, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಯದಿ (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	ಯದಿ (list != ರಿಕ್ತ)
	{
	    ಯದಿ (list_append_string(list, ರಿಕ್ತ, -1) == FAIL)
	    {
		list_free(list);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	ಪ್ರತ್ಯಾಗಮ (char_u *)list;
    }
    ಪ್ರತ್ಯಾಗಮ s;
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
get_reg_contents(ಪೂರ್ಣಾಙ್ಕ regname, ಪೂರ್ಣಾಙ್ಕ flags)
{
    linenr_T	i;
    char_u	*retval;
    ಪೂರ್ಣಾಙ್ಕ		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    ಯದಿ (regname == '=')
    {
	ಯದಿ (flags & GREG_NO_ಅಭಿವ್ಯಕ್ತಿ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (flags & GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC)
	    ಪ್ರತ್ಯಾಗಮ getreg_wrap_one_ಸಾಲು(get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು_src(), flags);
	ಪ್ರತ್ಯಾಗಮ getreg_wrap_one_ಸಾಲು(get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(), flags);
    }

    ಯದಿ (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    ಯದಿ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ಯದಿ (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	ಯದಿ (retval == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (allocated)
	    ಪ್ರತ್ಯಾಗಮ getreg_wrap_one_ಸಾಲು(retval, flags);
	ಪ್ರತ್ಯಾಗಮ getreg_wrap_one_ಸಾಲು(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    ಯದಿ (y_current->y_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	ಯದಿ (list == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಪ್ರತಿ (i = 0; i < y_current->y_size; ++i)
	{
	    ಯದಿ (list_append_string(list, y_current->y_array[i].string,
		(ಪೂರ್ಣಾಙ್ಕ)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
	ಪ್ರತ್ಯಾಗಮ (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    ಪ್ರತಿ (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ಯದಿ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    ಯದಿ (retval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Copy the lines of the yank register into the string.
    len = 0;
    ಪ್ರತಿ (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ಯದಿ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
init_write_reg(
    ಪೂರ್ಣಾಙ್ಕ		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    ಪೂರ್ಣಾಙ್ಕ		must_append,
    ಪೂರ್ಣಾಙ್ಕ		*yank_type UNUSED)
{
    ಯದಿ (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    ಯದಿ (!y_append && !must_append)
	free_yank_all();
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
finish_write_reg(
    ಪೂರ್ಣಾಙ್ಕ		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    ಯದಿ (name != '"')
	y_previous = old_y_previous;
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
    ಪೂರ್ಣಾಙ್ಕ		name,
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		maxlen,
    ಪೂರ್ಣಾಙ್ಕ		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    ಶೂನ್ಯ
write_reg_contents_lst(
    ಪೂರ್ಣಾಙ್ಕ		name,
    char_u	**strings,
    ಪೂರ್ಣಾಙ್ಕ		maxlen UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		must_append,
    ಪೂರ್ಣಾಙ್ಕ		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    ಯದಿ (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	ಯದಿ (strings[0] == ರಿಕ್ತ)
	    s = (char_u *)"";
	ಅನ್ಯಥಾ ಯದಿ (strings[1] != ರಿಕ್ತ)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_ಸಾಲುಗಳು), name);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (name == '_')	    // black hole: nothing to do
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    ಶೂನ್ಯ
write_reg_contents_ex(
    ಪೂರ್ಣಾಙ್ಕ		name,
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		maxlen,
    ಪೂರ್ಣಾಙ್ಕ		must_append,
    ಪೂರ್ಣಾಙ್ಕ		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    ಯದಿ (maxlen >= 0)
	len = maxlen;
    ಅನ್ಯಥಾ
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    ಯದಿ (name == '/')
    {
	set_last_ಹುಡುಕು_pat(str, RE_ಹುಡುಕು, TRUE, TRUE);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (name == '#')
    {
	ಯದಿ (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  ಪ್ರತ್ಯಾಗಮ;
	}

	buf_T	*buf;

	ಯದಿ (VIM_ISDIGIT(*str))
	{
	    ಪೂರ್ಣಾಙ್ಕ	ಅಂಕೆ = atoi((ವರ್ಣ *)str);

	    buf = buflist_findnr(ಅಂಕೆ);
	    ಯದಿ (buf == ರಿಕ್ತ)
		semsg(_(e_buffer_nr_does_not_exist), (long)ಅಂಕೆ);
	}
	ಅನ್ಯಥಾ
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	ಯದಿ (buf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	curwin->w_alt_fnum = buf->b_fnum;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (must_append && ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು != ರಿಕ್ತ)
	{
	    s = concat_str(ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು, p);
	    vim_free(p);
	    p = s;
	}
	set_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(p, ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (name == '_')	    // black hole: nothing to do
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#ಯದಿ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    ಶೂನ್ಯ
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    ಪೂರ್ಣಾಙ್ಕ		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    ಪೂರ್ಣಾಙ್ಕ		str_list)	// TRUE if str is char_u **
{
    ಪೂರ್ಣಾಙ್ಕ		type;			// MCHAR, MLINE or MBLOCK
    ಪೂರ್ಣಾಙ್ಕ		lnum;
    long	start;
    long	i;
    ಪೂರ್ಣಾಙ್ಕ		extra;
    ಪೂರ್ಣಾಙ್ಕ		newlines;		// number of lines added
    ಪೂರ್ಣಾಙ್ಕ		extraline = 0;		// extra line at the end
    ಪೂರ್ಣಾಙ್ಕ		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    ಯದಿ (y_ptr->y_array == ರಿಕ್ತ)		// NULL means empty register
	y_ptr->y_size = 0;

    ಯದಿ (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    ಅನ್ಯಥಾ
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    ಯದಿ (str_list)
    {
	ಪ್ರತಿ (ss = (char_u **) str; *ss != ರಿಕ್ತ; ++ss)
	    ++newlines;
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = 0; i < len; i++)
	    ಯದಿ (str[i] == '\n')
		++newlines;
	ಯದಿ (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	ಯದಿ (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    ಯದಿ (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * ಪರಿಮಾಣ(string_T), TRUE);
    ಯದಿ (pp == ರಿಕ್ತ)	// out of memory
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    ಯದಿ (str_list)
    {
	ಪ್ರತಿ (ss = (char_u **) str; *ss != ರಿಕ್ತ; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    ಯದಿ (type == MBLOCK)
	    {
		ಪೂರ್ಣಾಙ್ಕ charlen = vim_strsize(*ss);

		ಯದಿ (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (start = 0; start < len + extraline; start += i + 1)
	{
	    ಪೂರ್ಣಾಙ್ಕ charlen = 0;

	    ಪ್ರತಿ (i = start; i < len;)	// find the end of the line
	    {
		ಯದಿ (str[i] == '\n')
		    ಭಙ್ಗ;
		ಯದಿ (type == MBLOCK)
		{
		    ಯದಿ (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    ಅನ್ಯಥಾ
			charlen += byte2cells(str[i]);
		}

		ಯದಿ (str[i] == NUL)
		    i++; // registers can have NUL chars
		ಅನ್ಯಥಾ
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    ಯದಿ (charlen > maxlen)
		maxlen = charlen;
	    ಯದಿ (append)
	    {
		--lnum;
		extra = (ಪೂರ್ಣಾಙ್ಕ)y_ptr->y_array[lnum].length;
	    }
	    ಅನ್ಯಥಾ
		extra = 0;
	    s = alloc(i + extra + 1);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ಯದಿ (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    ಯದಿ (append)
		vim_free(y_ptr->y_array[lnum].string);
	    ಯದಿ (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    ಯಾವತ್ (--extra >= 0)
	    {
		ಯದಿ (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    ಯದಿ (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    ಅನ್ಯಥಾ
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
