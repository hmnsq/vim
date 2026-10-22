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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

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
ਸਥਿਰ yankreg_T	y_regs[ਅੰਕ_REGISTERS];

ਸਥਿਰ yankreg_T	*y_current;	    // ptr to current yankreg
ਸਥਿਰ ਪੂਰਨ		y_append;	    // TRUE when appending
ਸਥਿਰ yankreg_T	*y_ਪਿਛਲਾ = ਨਿਰਾ; // ptr to last written yankreg

ਸਥਿਰ ਪੂਰਨ	stuff_yank(ਪੂਰਨ, char_u *);
ਸਥਿਰ ਖਾਲੀ	put_reedit_ਵਿੱਚ_typebuf(ਪੂਰਨ silent);
ਸਥਿਰ ਪੂਰਨ	put_ਵਿੱਚ_typebuf(char_u *s, ਪੂਰਨ esc, ਪੂਰਨ colon, ਪੂਰਨ silent);
ਸਥਿਰ ਪੂਰਨ	yank_copy_ਸਤਰ(ਬਣਤਰ ਖੰਡ_def *bd, ਲੰਮਾ y_idx, ਪੂਰਨ exclude_trailing_space);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
ਸਥਿਰ ਖਾਲੀ	copy_yank_reg(yankreg_T *reg);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	dis_ਸੁਨੇਹਾ(char_u *p, ਪੂਰਨ skip_esc);

#ਜੇ defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(ਖਾਲੀ)
{
    ਵਾਪਸ y_regs;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_ਰਜਿਸਟਰ(ਪੂਰਨ reg)
{
    ਵਾਪਸ &y_regs[reg];
}
#ਜੇ_ਅੰਤ

    yankreg_T *
get_y_current(ਖਾਲੀ)
{
    ਵਾਪਸ y_current;
}

    yankreg_T *
get_y_ਪਿਛਲਾ(ਖਾਲੀ)
{
    ਵਾਪਸ y_ਪਿਛਲਾ;
}

    ਖਾਲੀ
ਸੈੱਟ_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    ਖਾਲੀ
ਸੈੱਟ_y_ਪਿਛਲਾ(yankreg_T *yreg)
{
    y_ਪਿਛਲਾ = yreg;
}

    ਖਾਲੀ
reset_y_append(ਖਾਲੀ)
{
    y_append = FALSE;
}


#ਜੇ defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
ਸਥਿਰ char_u	*ਵਿਅੰਜਕ_ਸਤਰ = ਨਿਰਾ;
ਸਥਿਰ exarg_T	*ਵਿਅੰਜਕ_eap = ਨਿਰਾ;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    ਪੂਰਨ
get_ਵਿਅੰਜਕ_ਰਜਿਸਟਰ(ਖਾਲੀ)
{
    char_u	*new_ਸਤਰ;

    new_ਸਤਰ = getcmdline('=', 0L, 0, 0);
    ਜੇ (new_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ NUL;
    ਜੇ (*new_ਸਤਰ == NUL)	// use previous line
	vim_free(new_ਸਤਰ);
    ਨਹੀਂਤਾਂ
	ਸੈੱਟ_ਵਿਅੰਜਕ_ਸਤਰ(new_ਸਤਰ, ਨਿਰਾ);
    ਵਾਪਸ '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    ਖਾਲੀ
ਸੈੱਟ_ਵਿਅੰਜਕ_ਸਤਰ(char_u *new_ਸਤਰ, exarg_T *eap)
{
    vim_free(ਵਿਅੰਜਕ_ਸਤਰ);
    ਵਿਅੰਜਕ_ਸਤਰ = new_ਸਤਰ;
    ਵਿਅੰਜਕ_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_ਵਿਅੰਜਕ_ਸਤਰ(ਖਾਲੀ)
{
    char_u	*ਵਿਅੰਜਕ_copy;
    char_u	*rv;
    ਸਥਿਰ ਪੂਰਨ	nested = 0;

    ਜੇ (ਵਿਅੰਜਕ_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    ਵਿਅੰਜਕ_copy = vim_strsave(ਵਿਅੰਜਕ_ਸਤਰ);
    ਜੇ (ਵਿਅੰਜਕ_copy == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    ਜੇ (nested >= 10)
	ਵਾਪਸ ਵਿਅੰਜਕ_copy;

    ++nested;
    rv = eval_ਮੰਜ਼ਿਲ_string_eap(ਵਿਅੰਜਕ_copy, TRUE, ਵਿਅੰਜਕ_eap, FALSE);
    --nested;
    vim_free(ਵਿਅੰਜਕ_copy);
    ਵਾਪਸ rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ਸਥਿਰ char_u *
get_ਵਿਅੰਜਕ_ਸਤਰ_src(ਖਾਲੀ)
{
    ਜੇ (ਵਿਅੰਜਕ_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ vim_strsave(ਵਿਅੰਜਕ_ਸਤਰ);
}
#ਜੇ_ਅੰਤ // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    ਪੂਰਨ
valid_yank_reg(
    ਪੂਰਨ	    regname,
    ਪੂਰਨ	    writing)	    // if TRUE check for writable registers
{
    ਜੇ (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
				    "/#.%:="
#ਨਹੀਂਤਾਂ
				    "/#.%:"
#ਜੇ_ਅੰਤ
					, regname) != ਨਿਰਾ)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#ਜੇ defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#ਜਾਂਜੇ defined(FEAT_CLIPBOARD_PROVIDER)
	    || ( // If -clipboard, then these registers only exist when
		 // clipmethod is set to provider.
	     clipmethod == CLIPMETHOD_PROVIDER && (
		 regname == '*'
		 || regname == '+'
	    ))
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
	    || (!writing && regname == '~')
#ਜੇ_ਅੰਤ
							)
	ਵਾਪਸ TRUE;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    ਨਹੀਂਤਾਂ ਜੇ (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	ਸੁਨੇਹਾ_warn_missing_clipboard();
	ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ FALSE;
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
    ਪੂਰਨ
get_yank_ਰਜਿਸਟਰ(ਪੂਰਨ regname, ਪੂਰਨ writing)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    ret = FALSE;

    y_append = FALSE;
    ਜੇ ((regname == 0 || regname == '"') && !writing && y_ਪਿਛਲਾ != ਨਿਰਾ)
    {
	y_current = y_ਪਿਛਲਾ;
	ਵਾਪਸ ret;
    }
    i = regname;
    ਜੇ (VIM_ISDIGIT(i))
	i -= '0';
    ਨਹੀਂਤਾਂ ਜੇ (ASCII_ISLOWER(i))
	i = Char_Ord_ਨੀਵਾਂ(i) + 10;
    ਨਹੀਂਤਾਂ ਜੇ (ASCII_ISUPPER(i))
    {
	i = Char_Ord_ਉੱਪਰ(i) + 10;
	y_append = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (regname == '-')
	i = DELETION_ਰਜਿਸਟਰ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    ਨਹੀਂਤਾਂ ਜੇ (clip_ਤਾਰਾ.ਉਪਲਬਧ && regname == '*')
    {
	i = ਤਾਰਾ_ਰਜਿਸਟਰ;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    ਨਹੀਂਤਾਂ ਜੇ (clip_plus.ਉਪਲਬਧ && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	ਜੇ (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_ਰਜਿਸਟਰ;
	ਨਹੀਂਤਾਂ
# endif
	    i = PLUS_ਰਜਿਸਟਰ;
	ret = TRUE;
    }
#ਜਾਂਜੇ defined(FEAT_CLIPBOARD_PROVIDER)
    ਨਹੀਂਤਾਂ ਜੇ (regname == '*')
    {
	i = ਤਾਰਾ_ਰਜਿਸਟਰ;
	ret = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (regname == '+')
    {
	i = REAL_PLUS_ਰਜਿਸਟਰ;
	ret = TRUE;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
    ਨਹੀਂਤਾਂ ਜੇ (!writing && regname == '~')
	i = TILDE_ਰਜਿਸਟਰ;
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    ਜੇ (writing)	// remember the register we write into for do_put()
	y_ਪਿਛਲਾ = y_current;
    ਵਾਪਸ ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    ਖਾਲੀ *
get_ਰਜਿਸਟਰ(
    ਪੂਰਨ		name,
    ਪੂਰਨ		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    ਪੂਰਨ		i;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	ਜੇ (name == '*' && clip_ਤਾਰਾ.ਉਪਲਬਧ)
	{
	    ਜੇ (clip_isautosel_ਤਾਰਾ())
		clip_update_selection(&clip_ਤਾਰਾ);
	    may_get_selection(name);
	}
	ਜੇ (name == '+' && clip_plus.ਉਪਲਬਧ)
	{
	    ਜੇ (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#ਜੇ_ਅੰਤ

    get_yank_ਰਜਿਸਟਰ(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    ਜੇ (reg == ਨਿਰਾ)
	ਵਾਪਸ (ਖਾਲੀ *)ਨਿਰਾ;

    *reg = *y_current;
    ਜੇ (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	ਜੇ (reg->y_ਮਾਪ == 0 || y_current->y_array == ਨਿਰਾ)
	    reg->y_array = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    reg->y_array = ALLOC_MULT(string_T, reg->y_ਮਾਪ);
	ਜੇ (reg->y_array != ਨਿਰਾ)
	{
	    ਲਈ (i = 0; i < reg->y_ਮਾਪ; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].ਲੰਮਾਈ);
		ਜੇ (reg->y_array[i].string == ਨਿਰਾ)
		{
		    // The allocation failed so clean up and exit
		    ਜਦੋਂ (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    ਵਾਪਸ (ਖਾਲੀ *)ਨਿਰਾ;
		}

		reg->y_array[i].ਲੰਮਾਈ = y_current->y_array[i].ਲੰਮਾਈ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	y_current->y_array = ਨਿਰਾ;
    ਵਾਪਸ (ਖਾਲੀ *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    ਖਾਲੀ
put_ਰਜਿਸਟਰ(ਪੂਰਨ name, ਖਾਲੀ *reg)
{
    get_yank_ਰਜਿਸਟਰ(name, 0);
    free_yank_all();
    *y_current = *(yankreg_T *)reg;
    vim_free(reg);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    // Send text written to clipboard register to the clipboard.
    may_ਸੈੱਟ_selection();
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_CLIPBOARD)
    ਖਾਲੀ
free_ਰਜਿਸਟਰ(ਖਾਲੀ *reg)
{
    yankreg_T tmp;

    tmp = *y_current;
    *y_current = *(yankreg_T *)reg;
    free_yank_all();
    vim_free(reg);
    *y_current = tmp;
}
#ਜੇ_ਅੰਤ

/*
 * return TRUE if the current yank register has type MLINE
 */
    ਪੂਰਨ
yank_ਰਜਿਸਟਰ_mline(ਪੂਰਨ regname)
{
    ਜੇ (regname != 0 && !valid_yank_reg(regname, FALSE))
	ਵਾਪਸ FALSE;
    ਜੇ (regname == '_')		// black hole is always empty
	ਵਾਪਸ FALSE;
    get_yank_ਰਜਿਸਟਰ(regname, FALSE);
    ਵਾਪਸ (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
do_ਰਿਕਾਰਡ(ਪੂਰਨ c)
{
    char_u	    *p;
    ਸਥਿਰ ਪੂਰਨ	    regname;
    yankreg_T	    *old_y_ਪਿਛਲਾ, *old_y_current;
    ਪੂਰਨ		    retval;

    ਜੇ (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	ਜੇ (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    ਨਹੀਂਤਾਂ			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	ਸੁਨੇਹਾ("");
	p = get_recorded();
	ਜੇ (p == ਨਿਰਾ)
	    retval = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    // Remove escaping for CSI and K_SPECIAL in multi-byte chars.
	    vim_unescape_csi(p);

	    // We don't want to change the default register here, so save and
	    // restore the current register name.
	    old_y_ਪਿਛਲਾ = y_ਪਿਛਲਾ;
	    old_y_current = y_current;

	    retval = stuff_yank(regname, p);

	    y_ਪਿਛਲਾ = old_y_ਪਿਛਲਾ;
	    y_current = old_y_current;
	}
    }
    ਵਾਪਸ retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
stuff_yank(ਪੂਰਨ regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    ਜੇ (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	ਵਾਪਸ OK;
    }

    plen = STRLEN(p);
    get_yank_ਰਜਿਸਟਰ(regname, TRUE);
    ਜੇ (y_append && y_current->y_array != ਨਿਰਾ)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_ਮਾਪ - 1]);
	tmplen = pp->ਲੰਮਾਈ + plen;
	tmp = alloc(tmplen + 1);
	ਜੇ (tmp == ਨਿਰਾ)
	{
	    vim_free(p);
	    ਵਾਪਸ FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->ਲੰਮਾਈ, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->ਲੰਮਾਈ = tmplen;
    }
    ਨਹੀਂਤਾਂ
    {
	free_yank_all();
	ਜੇ ((y_current->y_array = ALLOC_ONE(string_T)) == ਨਿਰਾ)
	{
	    vim_free(p);
	    ਵਾਪਸ FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].ਲੰਮਾਈ = plen;
	y_current->y_ਮਾਪ = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
	y_current->y_ਸਮਾਂ_ਸੈੱਟ = vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
    }
    ਵਾਪਸ OK;
}

/*
 * Last executed register (@ command)
 */
ਸਥਿਰ ਪੂਰਨ execreg_lastc = NUL;

#ਜੇ defined(FEAT_VIMINFO)
    ਪੂਰਨ
get_execreg_lastc(ਖਾਲੀ)
{
    ਵਾਪਸ execreg_lastc;
}

    ਖਾਲੀ
ਸੈੱਟ_execreg_lastc(ਪੂਰਨ lastc)
{
    execreg_lastc = lastc;
}
#ਜੇ_ਅੰਤ

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
    ਸਥਿਰ char_u *
execreg_ਸਤਰ_continuation(string_T *ਸਤਰਾਂ, ਲੰਮਾ *idx)
{
    garray_T	ga;
    ਲੰਮਾ	cmd_ਸ਼ੁਰੂ = *idx;
    ਲੰਮਾ	cmd_ਸਿਰਾ = *idx;
    string_T	*tmp;
    ਪੂਰਨ		j;
    char_u	*str;

    ga_init2(&ga, ਆਕਾਰ(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    ਜਦੋਂ (--cmd_ਸ਼ੁਰੂ > 0)
    {
	char_u	*p;

	p = skipwhite(ਸਤਰਾਂ[cmd_ਸ਼ੁਰੂ].string);
	ਜੇ (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    ਰੋਕੋ;
    }

    // join all the lines
    tmp = &ਸਤਰਾਂ[cmd_ਸ਼ੁਰੂ];
    ga_concat_len(&ga, tmp->string, tmp->ਲੰਮਾਈ);
    ਲਈ (j = cmd_ਸ਼ੁਰੂ + 1; j <= cmd_ਸਿਰਾ; j++)
    {
	char_u	*p;

	tmp = &ਸਤਰਾਂ[j];
	p = skipwhite(tmp->string);
	ਜੇ (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    ਜੇ (ga.ga_len > 400)
	    {
		ਜੇ (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		ਨਹੀਂਤਾਂ
		    ga.ga_growsize = ga.ga_len;
	    }
	    p++;
	    ga_concat_len(&ga, p, (tmp->string + tmp->ਲੰਮਾਈ) - p);
	}
    }
    ga_append(&ga, NUL);
    str = vim_strnsave(ga.ga_ਅੰਕੜੇ, ga.ga_len);
    ga_clear(&ga);

    *idx = cmd_ਸ਼ੁਰੂ;
    ਵਾਪਸ str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
do_execreg(
    ਪੂਰਨ	    regname,
    ਪੂਰਨ	    colon,		// insert ':' before each line
    ਪੂਰਨ	    addcr,		// always add '\n' to end of line
    ਪੂਰਨ	    silent)		// set "silent" flag in typeahead buffer
{
    ਲੰਮਾ	i;
    char_u	*p;
    ਪੂਰਨ		retval = OK;
    ਪੂਰਨ		remap;

    // repeat previous one
    ਜੇ (regname == '@')
    {
	ਜੇ (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_ਵਰਤਿਆ_ਰਜਿਸਟਰ));
	    ਵਾਪਸ FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    ਜੇ (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	ਵਾਪਸ FAIL;
    }
    execreg_lastc = regname;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#ਜੇ_ਅੰਤ

    // black hole: don't stuff anything
    ਜੇ (regname == '_')
	ਵਾਪਸ OK;

    // use last command line
    ਜੇ (regname == ':')
    {
	ਜੇ (ਆਖਰੀ_cmdline == ਨਿਰਾ)
	{
	    emsg(_(e_no_ਪਿਛਲਾ_command_ਸਤਰ));
	    ਵਾਪਸ FAIL;
	}
	// don't keep the cmdline containing @:
	VIM_CLEAR(new_ਆਖਰੀ_cmdline);
	// Escape all control characters with a CTRL-V
	p = vim_strsave_escaped_ext(ਆਖਰੀ_cmdline,
		    (char_u *)"\001\002\003\004\005\006\007"
			  "\010\011\012\013\014\015\016\017"
			  "\020\021\022\023\024\025\026\027"
			  "\030\031\032\033\034\035\036\037",
		    Ctrl_V, FALSE);
	ਜੇ (p != ਨਿਰਾ)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    ਜੇ (VIsual_ਸਰਗਰਮ && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_ਵਿੱਚ_typebuf(p + 5, TRUE, TRUE, silent);
	    ਨਹੀਂਤਾਂ
		retval = put_ਵਿੱਚ_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਨਹੀਂਤਾਂ ਜੇ (regname == '=')
    {
	p = get_ਵਿਅੰਜਕ_ਸਤਰ();
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	retval = put_ਵਿੱਚ_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ ਜੇ (regname == '.')		// use last inserted text
    {
	p = get_ਆਖਰੀ_ਪਾਓ_save();
	ਜੇ (p == ਨਿਰਾ)
	{
	    emsg(_(e_no_inserted_ਪਾਠ_yet));
	    ਵਾਪਸ FAIL;
	}
	retval = put_ਵਿੱਚ_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    ਨਹੀਂਤਾਂ
    {
	get_yank_ਰਜਿਸਟਰ(regname, FALSE);
	ਜੇ (y_current->y_array == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_ਵਿੱਚ_typebuf(silent);
	ਲਈ (i = y_current->y_ਮਾਪ; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    ਪੂਰਨ	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    ਜੇ (y_current->y_type == MLINE || i < y_current->y_ਮਾਪ - 1
								     || addcr)
	    {
		ਜੇ (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    ਵਾਪਸ FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    ਜੇ (colon && i > 0)
	    {
		p = skipwhite(str);
		ਜੇ (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_ਸਤਰ_continuation(y_current->y_array, &i);
		    ਜੇ (str == ਨਿਰਾ)
			ਵਾਪਸ FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    ਜੇ (free_str)
		vim_free(str);
	    ਜੇ (escaped == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    ਜੇ (retval == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		ਵਾਪਸ FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_ਸਿਰਾ_reg_executing = FALSE;
    }
    ਵਾਪਸ retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ਸਥਿਰ ਖਾਲੀ
put_reedit_ਵਿੱਚ_typebuf(ਪੂਰਨ silent)
{
    ਜੇ (restart_edit == NUL)
	ਵਾਪਸ;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    ਜੇ (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    ਨਹੀਂਤਾਂ ਜੇ (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    ਨਹੀਂਤਾਂ ਜੇ (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    ਜੇ (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    ਸਥਿਰ ਪੂਰਨ
put_ਵਿੱਚ_typebuf(
    char_u	*s,
    ਪੂਰਨ		esc,
    ਪੂਰਨ		colon,	    // add ':' before the line
    ਪੂਰਨ		silent)
{
    ਪੂਰਨ		retval = OK;

    put_reedit_ਵਿੱਚ_typebuf(silent);
    ਜੇ (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    ਜੇ (retval == OK)
    {
	char_u	*p;

	ਜੇ (esc)
	    p = vim_strsave_escape_csi(s);
	ਨਹੀਂਤਾਂ
	    p = s;
	ਜੇ (p == ਨਿਰਾ)
	    retval = FAIL;
	ਨਹੀਂਤਾਂ
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	ਜੇ (esc)
	    vim_free(p);
    }
    ਜੇ (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    ਵਾਪਸ retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
ਪਾਓ_reg(
    ਪੂਰਨ		regname,
    ਪੂਰਨ		literally_arg)	// insert literally, not as if typed
{
    ਲੰਮਾ	i;
    ਪੂਰਨ		retval = OK;
    char_u	*arg;
    ਪੂਰਨ		allocated;
    ਪੂਰਨ		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    ਜੇ (got_int)
	ਵਾਪਸ FAIL;

    // check for valid regname
    ਜੇ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ਵਾਪਸ FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#ਜੇ_ਅੰਤ

    ਜੇ (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    ਨਹੀਂਤਾਂ ਜੇ (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	ਜੇ (arg == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	stuffescaped(arg, literally);
	ਜੇ (allocated)
	    vim_free(arg);
    }
    ਨਹੀਂਤਾਂ				// name or number register
    {
	ਜੇ (get_yank_ਰਜਿਸਟਰ(regname, FALSE))
	    literally = TRUE;
	ਜੇ (y_current->y_array == ਨਿਰਾ)
	    retval = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (i = 0; i < y_current->y_ਮਾਪ; ++i)
	    {
		ਜੇ (regname == '-' && y_current->y_type == MCHAR)
		{
		    ਪੂਰਨ dir = BACKWARD;
		    ਜੇ ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			ਜੇ (u_save_cursor() == FAIL)
			    ਵਾਪਸ FAIL;
			del_chars((ਲੰਮਾ)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			ਜੇ (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(Ctrl_R);
		    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(regname);
		    do_put(regname, ਨਿਰਾ, dir, 1L, PUT_CURSEND);
		}
		ਨਹੀਂਤਾਂ
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    ਜੇ (y_current->y_type == MLINE || i < y_current->y_ਮਾਪ - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    ਵਾਪਸ retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    ਪੂਰਨ
get_spec_reg(
    ਪੂਰਨ		regname,
    char_u	**argp,
    ਪੂਰਨ		*allocated,	// return: TRUE when value was allocated
    ਪੂਰਨ		errmsg)		// give error message when failing
{
    ਪੂਰਨ		cnt;

    *argp = ਨਿਰਾ;
    *allocated = FALSE;
    ਚੋਣ (regname)
    {
	ਹਾਲਤ '%':		// file name
	    ਜੇ (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    ਵਾਪਸ TRUE;

	ਹਾਲਤ '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    ਵਾਪਸ TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਹਾਲਤ '=':		// result of expression
	    *argp = get_ਵਿਅੰਜਕ_ਸਤਰ();
	    *allocated = TRUE;
	    ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ ':':		// last command line
	    ਜੇ (ਆਖਰੀ_cmdline == ਨਿਰਾ && errmsg)
		emsg(_(e_no_ਪਿਛਲਾ_command_ਸਤਰ));
	    *argp = ਆਖਰੀ_cmdline;
	    ਵਾਪਸ TRUE;

	ਹਾਲਤ '/':		// last search-pattern
	    ਜੇ (ਆਖਰੀ_ਖੋਜ_pat() == ਨਿਰਾ && errmsg)
		emsg(_(e_no_ਪਿਛਲਾ_regular_ਵਿਅੰਜਕ));
	    *argp = ਆਖਰੀ_ਖੋਜ_pat();
	    ਵਾਪਸ TRUE;

	ਹਾਲਤ '.':		// last inserted text
	    *argp = get_ਆਖਰੀ_ਪਾਓ_save();
	    *allocated = TRUE;
	    ਜੇ (*argp == ਨਿਰਾ && errmsg)
		emsg(_(e_no_inserted_ਪਾਠ_yet));
	    ਵਾਪਸ TRUE;

	ਹਾਲਤ Ctrl_F:		// Filename under cursor
	ਹਾਲਤ Ctrl_P:		// Path under cursor, expand via "path"
	    ਜੇ (!errmsg)
		ਵਾਪਸ FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, ਨਿਰਾ);
	    *allocated = TRUE;
	    ਵਾਪਸ TRUE;

	ਹਾਲਤ Ctrl_W:		// word under cursor
	ਹਾਲਤ Ctrl_A:		// WORD (mnemonic All) under cursor
	    ਜੇ (!errmsg)
		ਵਾਪਸ FALSE;
	    cnt = ਲੱਭੋ_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (ਲੱਭੋ_IDENT|ਲੱਭੋ_STRING) : ਲੱਭੋ_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : ਨਿਰਾ;
	    *allocated = TRUE;
	    ਵਾਪਸ TRUE;

	ਹਾਲਤ Ctrl_L:		// Line under cursor
	    ਜੇ (!errmsg)
		ਵਾਪਸ FALSE;

	    *argp = ml_get_buf(curwin->w_ਬਫਰ,
			curwin->w_cursor.lnum, FALSE);
	    ਵਾਪਸ TRUE;

	ਹਾਲਤ '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    ਵਾਪਸ TRUE;
    }

    ਵਾਪਸ FALSE;
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
    ਪੂਰਨ
cmdline_paste_reg(
    ਪੂਰਨ regname,
    ਪੂਰਨ literally_arg,	// Insert text literally instead of "as typed"
    ਪੂਰਨ remcr)		// don't add CR characters
{
    ਲੰਮਾ	i;
    ਪੂਰਨ		literally = literally_arg;

    ਜੇ (get_yank_ਰਜਿਸਟਰ(regname, FALSE))
	literally = TRUE;
    ਜੇ (y_current->y_array == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਲਈ (i = 0; i < y_current->y_ਮਾਪ; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	ਜੇ ((y_current->y_type == MLINE || i < y_current->y_ਮਾਪ - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	ਜੇ (got_int)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    ਖਾਲੀ
ਖਿਸਕਾਓ_delete_registers(ਖਾਲੀ)
{
    ਪੂਰਨ		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    ਲਈ (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    ਜੇ (!y_append)
	y_ਪਿਛਲਾ = y_current;
    y_regs[1].y_array = ਨਿਰਾ;		// set register one to empty
}

#ਜੇ defined(FEAT_EVAL)
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_regtype_ਮੰਜ਼ਿਲ_dict(ਪੂਰਨ regname, dict_T *dict, char_u *buf, ਪੂਰਨ bufsize)
{
    size_t  buflen;
    ਲੰਮਾ    reglen;
    // Register type
    ਚੋਣ (get_reg_type(regname, &reglen))
    {
	ਹਾਲਤ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ਰੋਕੋ;
	ਹਾਲਤ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ਰੋਕੋ;
	ਹਾਲਤ MBLOCK:
	    buflen = vim_snprintf_safelen((ਅੱਖਰ *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    ਰੋਕੋ;
	ਮੂਲ:
	    buf[0] = NUL;
	    buflen = 0;
	    ਰੋਕੋ;
    }
    (ਖਾਲੀ)dict_ਜੋੜੋ_string_len(dict, "regtype", buf, (ਪੂਰਨ)buflen);
}

    ਖਾਲੀ
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    ਸਥਿਰ ਪੂਰਨ	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    ਪੂਰਨ		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ਜੇ (recursive)
	ਵਾਪਸ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ;

    // yanked text contents
    ਲਈ (n = 0; n < reg->y_ਮਾਪ; n++)
	list_append_string(list, reg->y_array[n].string, (ਪੂਰਨ)reg->y_array[n].ਲੰਮਾਈ);
    list->lv_ਤਾਲਾ = VAR_FIXED;
    ਜੇ (dict_ਜੋੜੋ_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ਖਾਲੀ)dict_ਜੋੜੋ_string_len(v_event, "regname", buf, (ਪੂਰਨ)buflen);

    // motion type: inclusive or exclusive
    (ਖਾਲੀ)dict_ਜੋੜੋ_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (ਖਾਲੀ)dict_ਜੋੜੋ_string_len(v_event, "operator", buf, (ਪੂਰਨ)buflen);

    ਜੋੜੋ_regtype_ਮੰਜ਼ਿਲ_dict(oap->regname, v_event, buf, ਆਕਾਰ(buf));

    // selection type - visual or not
    (ਖਾਲੀ)dict_ਜੋੜੋ_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    ਸਥਿਰ ਖਾਲੀ
put_do_autocmd(
	ਪੂਰਨ	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *ਪਾਓ,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	ਪੂਰਨ	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    ਸਥਿਰ bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    ਪੂਰਨ		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ਜੇ (recursive || regname == '_')
	ਵਾਪਸ;

    ਜੇ (regname != '.' && ਪਾਓ == ਨਿਰਾ
	    && reg == ਨਿਰਾ)
	// Can happen when pasting text in normal mode in a terminal buffer
	ਵਾਪਸ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ਜੇ (regname == '.')
    {
	ਜੇ (ਆਖਰੀ_ਪਾਓ_ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, ਆਖਰੀ_ਪਾਓ_ga.ga_ਅੰਕੜੇ,
		    (ਪੂਰਨ)ਆਖਰੀ_ਪਾਓ_ga.ga_len);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਪਾਓ != ਨਿਰਾ)
    {
	list_append_string(list, ਪਾਓ->string, (ਪੂਰਨ)ਪਾਓ->ਲੰਮਾਈ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (n = 0; n < reg->y_ਮਾਪ; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (ਪੂਰਨ)reg->y_array[n].ਲੰਮਾਈ);
    }

    list->lv_ਤਾਲਾ = VAR_FIXED;
    ਜੇ (dict_ਜੋੜੋ_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ਖਾਲੀ)dict_ਜੋੜੋ_string_len(v_event, "regname", buf, (ਪੂਰਨ)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (ਖਾਲੀ)dict_ਜੋੜੋ_string_len(v_event, "operator", buf, (ਪੂਰਨ)buflen);

    ਜੋੜੋ_regtype_ਮੰਜ਼ਿਲ_dict(regname, v_event, buf, ਆਕਾਰ(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (ਖਾਲੀ)dict_ਜੋੜੋ_bool(v_event, "visual", VIsual_ਸਰਗਰਮ);

    // Lock the dictionary and its keys
    dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(v_event);

    recursive = true;
    textlock++;
    ਜੇ (post)
	apply_autocmds(EVENT_TEXTPUTPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    ਨਹੀਂਤਾਂ
	apply_autocmds(EVENT_TEXTPUTPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#ਜੇ_ਅੰਤ

/*
 * set all the yank registers to empty (called from main())
 */
    ਖਾਲੀ
init_yank(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < ਅੰਕ_REGISTERS; ++i)
	y_regs[i].y_array = ਨਿਰਾ;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
clear_registers(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < ਅੰਕ_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	ਜੇ (y_current->y_array != ਨਿਰਾ)
	    free_yank_all();
    }
}
#ਜੇ_ਅੰਤ

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    ਸਥਿਰ ਖਾਲੀ
free_yank(ਲੰਮਾ n)
{
    ਜੇ (y_current->y_array == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲੰਮਾ	    i;

    ਲਈ (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    ਖਾਲੀ
free_yank_all(ਖਾਲੀ)
{
    free_yank(y_current->y_ਮਾਪ);
}

/*
 * Yank the text between "oap->start" and "oap->end" into a yank register.
 * If we are to append (uppercase register), we first yank into a new yank
 * register and then concatenate the old and the new one (so we keep the old
 * one in case of out-of-memory).
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
op_yank(oparg_T *oap, ਪੂਰਨ deleting, ਪੂਰਨ mess)
{
    ਲੰਮਾ		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    ਪੂਰਨ			yanktype = oap->motion_type;
    ਲੰਮਾ		yanklines = oap->ਸਤਰ_ਗਿਣਤੀ;
    linenr_T		yankendlnum = oap->ਸਿਰਾ.lnum;
    char_u		*pnew;
    ਬਣਤਰ ਖੰਡ_def	bd;

				    // check for read-only register
    ਜੇ (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	ਵਾਪਸ FAIL;
    }
    ਜੇ (oap->regname == '_')	    // black hole: nothing to do
	ਵਾਪਸ OK;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਜੇ ((!clip_ਤਾਰਾ.ਉਪਲਬਧ && oap->regname == '*') ||
	(!clip_plus.ਉਪਲਬਧ && oap->regname == '+'))
    {
	oap->regname = 0;
	ਸੁਨੇਹਾ_warn_missing_clipboard();
    }
#ਜੇ_ਅੰਤ

    ਜੇ (!deleting)		    // op_delete() already set y_current
	get_yank_ਰਜਿਸਟਰ(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    ਜੇ (y_append && y_current->y_array != ਨਿਰਾ)
	y_current = &newreg;
    ਨਹੀਂਤਾਂ
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    ਜੇ (       oap->motion_type == MCHAR
	    && oap->ਸ਼ੁਰੂ.ਖਾਨਾ == 0
	    && !oap->inclusive
	    && (!oap->is_VIsual || *p_sel == 'o')
	    && !oap->ਖੰਡ_ਬਹੁਲਕ
	    && oap->ਸਿਰਾ.ਖਾਨਾ == 0
	    && yanklines > 1)
    {
	yanktype = MLINE;
	--yankendlnum;
	--yanklines;
    }

    y_current->y_ਮਾਪ = yanklines;
    y_current->y_type = yanktype;   // set the yank register type
    y_current->y_ਚੌੜਾਈ = 0;
    y_current->y_array = lalloc_clear(ਆਕਾਰ(string_T) * yanklines, TRUE);
    ਜੇ (y_current->y_array == ਨਿਰਾ)
    {
	y_current = curr;
	ਵਾਪਸ FAIL;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    y_current->y_ਸਮਾਂ_ਸੈੱਟ = vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ

    y_idx = 0;
    lnum = oap->ਸ਼ੁਰੂ.lnum;

    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_ਚੌੜਾਈ = oap->ਸਿਰਾ_vcol - oap->ਸ਼ੁਰੂ_vcol;

	ਜੇ (curwin->w_curswant == MAXCOL && y_current->y_ਚੌੜਾਈ > 0)
	    y_current->y_ਚੌੜਾਈ--;
    }

    ਲਈ ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	ਚੋਣ (y_current->y_type)
	{
	    ਹਾਲਤ MBLOCK:
		ਖੰਡ_prep(oap, &bd, lnum, FALSE);
		ਜੇ (yank_copy_ਸਤਰ(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    ਜਾਓ fail;
		ਰੋਕੋ;

	    ਹਾਲਤ MLINE:
		y_current->y_array[y_idx].ਲੰਮਾਈ = ml_get_len(lnum);
		ਜੇ ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].ਲੰਮਾਈ)) == ਨਿਰਾ)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    ਜਾਓ fail;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ MCHAR:
		{
		    ਪੂਰਨ tmp;

		    charwise_ਖੰਡ_prep(oap->ਸ਼ੁਰੂ, oap->ਸਿਰਾ, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (ਪੂਰਨ)STRLEN(bd.textstart);
		    ਜੇ (tmp < bd.textlen)
			bd.textlen = tmp;

		    ਜੇ (yank_copy_ਸਤਰ(&bd, y_idx, FALSE) == FAIL)
			ਜਾਓ fail;
		    ਰੋਕੋ;
		}
		// NOTREACHED
	}
    }

    ਜੇ (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	ਲੰਮਾ j;

	new_ptr = ALLOC_MULT(string_T, curr->y_ਮਾਪ + y_current->y_ਮਾਪ);
	ਜੇ (new_ptr == ਨਿਰਾ)
	    ਜਾਓ fail;
	ਲਈ (j = 0; j < curr->y_ਮਾਪ; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
	curr->y_ਸਮਾਂ_ਸੈੱਟ = vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ

	ਜੇ (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	ਜੇ (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == ਨਿਰਾ)
	{
	    pnew = alloc(curr->y_array[curr->y_ਮਾਪ - 1].ਲੰਮਾਈ + y_current->y_array[0].ਲੰਮਾਈ + 1);
	    ਜੇ (pnew == ਨਿਰਾ)
	    {
		y_idx = y_current->y_ਮਾਪ - 1;
		ਜਾਓ fail;
	    }

	    --j;
	    STRCPY(pnew, curr->y_array[j].string);
	    STRCPY(pnew + curr->y_array[j].ਲੰਮਾਈ, y_current->y_array[0].string);
	    vim_free(curr->y_array[j].string);
	    curr->y_array[j].string = pnew;
	    curr->y_array[j].ਲੰਮਾਈ = curr->y_array[j].ਲੰਮਾਈ + y_current->y_array[0].ਲੰਮਾਈ;
	    ++j;
	    VIM_CLEAR_STRING(y_current->y_array[0]);
	    y_idx = 1;
	}
	ਨਹੀਂਤਾਂ
	    y_idx = 0;
	ਜਦੋਂ (y_idx < y_current->y_ਮਾਪ)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_ਮਾਪ = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    ਜੇ (mess)			// Display message about yank?
    {
	ਜੇ (yanktype == MCHAR
		&& !oap->ਖੰਡ_ਬਹੁਲਕ
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	ਜੇ (yanklines > p_report)
	{
	    ਅੱਖਰ namebuf[100];

	    ਜੇ (oap->regname == NUL)
		*namebuf = NUL;
	    ਨਹੀਂਤਾਂ
		vim_snprintf(namebuf, ਆਕਾਰ(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_ਸ਼ੁਰੂ = oap->ਸ਼ੁਰੂ;
	curbuf->b_op_ਸਿਰਾ = oap->ਸਿਰਾ;
	ਜੇ (yanktype == MLINE && !oap->ਖੰਡ_ਬਹੁਲਕ)
	{
	    curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
	    curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = MAXCOL;
	}
	ਜੇ (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_ਸਿਰਾ);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    ਜੇ (curr == &y_regs[REAL_PLUS_ਰਜਿਸਟਰ] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	ਜੇ (curr != &(y_regs[REAL_PLUS_ਰਜਿਸਟਰ]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_ਰਜਿਸਟਰ]));
	call_clip_provider_ਸੈੱਟ('+');
    }
    ਜੇ (curr == &y_regs[ਤਾਰਾ_ਰਜਿਸਟਰ] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	ਜੇ (curr != &(y_regs[ਤਾਰਾ_ਰਜਿਸਟਰ]))
	    copy_yank_reg(&(y_regs[ਤਾਰਾ_ਰਜਿਸਟਰ]));
	call_clip_provider_ਸੈੱਟ('*');
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	ਜੇ (clip_ਤਾਰਾ.ਉਪਲਬਧ
		&& (curr == &(y_regs[ਤਾਰਾ_ਰਜਿਸਟਰ])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    ਜੇ (curr != &(y_regs[ਤਾਰਾ_ਰਜਿਸਟਰ]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[ਤਾਰਾ_ਰਜਿਸਟਰ]));

	    clip_own_selection(&clip_ਤਾਰਾ);
	    clip_gen_ਸੈੱਟ_selection(&clip_ਤਾਰਾ);
	}

# ਜੇ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	ਜੇ (clip_plus.ਉਪਲਬਧ
		&& (curr == &(y_regs[PLUS_ਰਜਿਸਟਰ])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    ਜੇ (curr != &(y_regs[PLUS_ਰਜਿਸਟਰ]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_ਰਜਿਸਟਰ]));

	    clip_own_selection(&clip_plus);
	    clip_gen_ਸੈੱਟ_selection(&clip_plus);
	}
# endif
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
    ਜੇ (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#ਜੇ_ਅੰਤ
    ਵਾਪਸ OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    ਵਾਪਸ FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    ਸਥਿਰ ਪੂਰਨ
yank_copy_ਸਤਰ(ਬਣਤਰ ਖੰਡ_def *bd, ਲੰਮਾ y_idx, ਪੂਰਨ exclude_trailing_space)
{
    char_u	*pnew;

    ਜੇ (exclude_trailing_space)
	bd->endspaces = 0;
    ਜੇ ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    ਜੇ (exclude_trailing_space)
    {
	ਪੂਰਨ s = bd->textlen + bd->endspaces;

	ਜਦੋਂ (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_ਸਿਰ_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].ਲੰਮਾਈ = (size_t)(pnew - y_current->y_array[y_idx].string);

    ਵਾਪਸ OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    ਸਥਿਰ ਖਾਲੀ
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    ਲੰਮਾ	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    ਆਕਾਰ(string_T) * y_current->y_ਮਾਪ, TRUE);
    ਜੇ (y_current->y_array == ਨਿਰਾ)
	y_current->y_ਮਾਪ = 0;
    ਨਹੀਂਤਾਂ
	ਲਈ (j = 0; j < y_current->y_ਮਾਪ; ++j)
	{
	    ਜੇ ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].ਲੰਮਾਈ)) == ਨਿਰਾ)
	    {
		free_yank(j);
		y_current->y_ਮਾਪ = 0;
		ਰੋਕੋ;
	    }
	    y_current->y_array[j].ਲੰਮਾਈ = curr->y_array[j].ਲੰਮਾਈ;
	}
    y_current = curr;
}
#ਜੇ_ਅੰਤ

/*
 * Put contents of register "regname" into the text.
 * Caller must check "regname" to be valid!
 * "flags": PUT_FIXINDENT	make indent look nice
 *	    PUT_CURSEND		leave cursor after end of new text
 *	    PUT_LINE		force linewise put (":put")
 *	    PUT_BLOCK_INNER     in block mode, do not add trailing spaces
 */
    ਖਾਲੀ
do_put(
    ਪੂਰਨ		regname,
    char_u	*ਵਿਅੰਜਕ_ਨਤੀਜਾ,	// result for regname "=" when compiled
    ਪੂਰਨ		dir,		// BACKWARD for 'P', FORWARD for 'p'
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    ਪੂਰਨ		yanklen;
    ਪੂਰਨ		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	ਖਾਨਾ;
    ਲੰਮਾ	i;			// index in y_array[]
    ਪੂਰਨ		y_type;
    ਲੰਮਾ	y_ਮਾਪ;
    ਪੂਰਨ		oldlen;
    ਲੰਮਾ	y_ਚੌੜਾਈ = 0;
    colnr_T	vcol;
    string_T	*y_array = ਨਿਰਾ;
    yankreg_T	*y_current_ਵਰਤਿਆ = ਨਿਰਾ;
    ਲੰਮਾ	nr_ਸਤਰਾਂ = 0;
    string_T	ਪਾਓ_string;
    ਪੂਰਨ		allocated = FALSE;
    pos_T	orig_ਸ਼ੁਰੂ = curbuf->b_op_ਸ਼ੁਰੂ;
    pos_T	orig_ਸਿਰਾ = curbuf->b_op_ਸਿਰਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cur_ve_flags = get_ve_flags();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(ਖਾਲੀ)may_get_selection(regname);
#ਜੇ_ਅੰਤ

    // Remove any preinserted text (issue #19329)
    ਜੇ (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_ਸ਼ੁਰੂ = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_ਸਿਰਾ = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    ਜੇ (regname == '.')
    {
	ਜੇ (VIsual_ਸਰਗਰਮ)
	    stuffcharReadbuff(VIsual_ਬਹੁਲਕ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	ਜੇ (has_textput_events)
	    ਜੋੜੋ_ਆਖਰੀ_ਪਾਓ++;
#ਜੇ_ਅੰਤ

	(ਖਾਲੀ)stuff_inserted((dir == FORWARD ? (ਗਿਣਤੀ_2 == -1 ? 'o' : 'a') :
				    (ਗਿਣਤੀ_2 == -1 ? 'O' : 'i')), ਗਿਣਤੀ_2, FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	ਜੇ (has_textputpre())
	    put_do_autocmd('.', ਨਿਰਾ, ਨਿਰਾ, false, dir);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (has_textputpost())
	    put_do_autocmd('.', ਨਿਰਾ, ਨਿਰਾ, true, dir);

	ਜੇ (has_textput_events && --ਜੋੜੋ_ਆਖਰੀ_ਪਾਓ == 0)
	    ga_clear(&ਆਖਰੀ_ਪਾਓ_ga);
#ਜੇ_ਅੰਤ

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	ਜੇ ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	ਵਾਪਸ;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    ਪਾਓ_string.string = ਨਿਰਾ;
    ਜੇ (regname == '=' && ਵਿਅੰਜਕ_ਨਤੀਜਾ != ਨਿਰਾ)
	ਪਾਓ_string.string = ਵਿਅੰਜਕ_ਨਤੀਜਾ;
    ਨਹੀਂਤਾਂ ਜੇ (get_spec_reg(regname, &ਪਾਓ_string.string, &allocated, TRUE)
		&& ਪਾਓ_string.string == ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#ਜੇ_ਅੰਤ
	ਵਾਪਸ;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    ਜੇ (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	ਜਾਓ ਸਿਰਾ;

    ਜੇ (ਪਾਓ_string.string != ਨਿਰਾ)
    {
	ਪਾਓ_string.ਲੰਮਾਈ = STRLEN(ਪਾਓ_string.string);

	y_type = MCHAR;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    ਲਈ (;;)
	    {
		y_ਮਾਪ = 0;
		ptr = ਪਾਓ_string.string;
		ptrlen = ਪਾਓ_string.ਲੰਮਾਈ;
		ਜਦੋਂ (ptr != ਨਿਰਾ)
		{
		    ਜੇ (y_array != ਨਿਰਾ)
			y_array[y_ਮਾਪ].string = ptr;
		    ++y_ਮਾਪ;
		    tmp = vim_strchr(ptr, '\n');
		    ਜੇ (tmp == ਨਿਰਾ)
		    {
			ਜੇ (y_array != ਨਿਰਾ)
			    y_array[y_ਮਾਪ - 1].ਲੰਮਾਈ = ptrlen;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (y_array != ਨਿਰਾ)
			{
			    *tmp = NUL;
			    y_array[y_ਮਾਪ - 1].ਲੰਮਾਈ = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_ਮਾਪ - 1].ਲੰਮਾਈ + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			ਜੇ (*tmp == NUL)
			{
			    y_type = MLINE;
			    ਰੋਕੋ;
			}
		    }
		    ptr = tmp;
		}
		ਜੇ (y_array != ਨਿਰਾ)
		    ਰੋਕੋ;
		y_array = ALLOC_MULT(string_T, y_ਮਾਪ);
		ਜੇ (y_array == ਨਿਰਾ)
		    ਜਾਓ ਸਿਰਾ;
	    }
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    y_ਮਾਪ = 1;		// use fake one-line yank register
	    y_array = &ਪਾਓ_string;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (has_textputpre())
	    put_do_autocmd(regname, ਨਿਰਾ, &ਪਾਓ_string, false, dir);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_ਰਜਿਸਟਰ(regname, FALSE);
	    put_do_autocmd(regname, y_current, ਨਿਰਾ, false, dir);
	}
#ਜੇ_ਅੰਤ
	get_yank_ਰਜਿਸਟਰ(regname, FALSE);

	y_type = y_current->y_type;
	y_ਚੌੜਾਈ = y_current->y_ਚੌੜਾਈ;
	y_ਮਾਪ = y_current->y_ਮਾਪ;
	y_array = y_current->y_array;
	y_current_ਵਰਤਿਆ = y_current;
    }

    ਜੇ (y_type == MLINE)
    {
	ਜੇ (flags & PUT_ਸਤਰ_ਵੰਡੋ)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    ਜੇ (u_save_cursor() == FAIL)
		ਜਾਓ ਸਿਰਾ;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    ਜੇ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    ਜੇ (ptr == ਨਿਰਾ)
		ਜਾਓ ਸਿਰਾ;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.ਖਾਨਾ;
	    ਜੇ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    ਜੇ (ptr == ਨਿਰਾ)
		ਜਾਓ ਸਿਰਾ;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_ਸਤਰਾਂ;
	    dir = FORWARD;
	}
	ਜੇ (flags & PUT_ਸਤਰ_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_ਸਿਰਾ;
	    dir = FORWARD;
	}
	curbuf->b_op_ਸ਼ੁਰੂ = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_ਸਿਰਾ = curwin->w_cursor;	// default for '] mark
    }

    ਜੇ (flags & PUT_ਸਤਰ)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    ਜੇ (y_ਮਾਪ == 0 || y_array == ਨਿਰਾ)
    {
	semsg(_(e_nothing_ਵਿੱਚ_ਰਜਿਸਟਰ_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	ਜਾਓ ਸਿਰਾ;
    }

    ਜੇ (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_ਮਾਪ + 1;
	ਜੇ (lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;
	ਜੇ (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    ਜਾਓ ਸਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	ਜੇ (dir == BACKWARD)
	    (ਖਾਲੀ)hasFolding(lnum, &lnum, ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    (ਖਾਲੀ)hasFolding(lnum, ਨਿਰਾ, &lnum);
#ਜੇ_ਅੰਤ
	ਜੇ (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	ਜੇ ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    ਜਾਓ ਸਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਜੇ (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	ਨਹੀਂਤਾਂ
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_ਸ਼ੁਰੂ = curwin->w_cursor;	// for mark_adjust()
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (u_save_cursor() == FAIL)
	ਜਾਓ ਸਿਰਾ;

    ਜੇ (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	ਜੇ (gchar_cursor() == TAB)
	{
	    ਪੂਰਨ viscol = getviscol();
	    ਪੂਰਨ ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    ਜੇ (dir == FORWARD ?
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#ਨਹੀਂਤਾਂ
		    ts - (viscol % ts) != 1
#ਜੇ_ਅੰਤ
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    ਨਹੀਂਤਾਂ
		curwin->w_cursor.coladd = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

    // Block mode
    ਜੇ (y_type == MBLOCK)
    {
	ਪੂਰਨ	delcount;
	ਪੂਰਨ	incr = 0;
	ਬਣਤਰ ਖੰਡ_def bd;
	ਲੰਮਾ	j;
	ਪੂਰਨ	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	ਜੇ (dir == FORWARD && c != NUL)
	{
	    ਜੇ (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &ਖਾਨਾ, ਨਿਰਾ, &endcol2, 0);
	    ਨਹੀਂਤਾਂ
		getvcol(curwin, &curwin->w_cursor, ਨਿਰਾ, ਨਿਰਾ, &ਖਾਨਾ, 0);

	    ਜੇ (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.ਖਾਨਾ += (*mb_ptr2len)(ml_get_cursor());
	    ਨਹੀਂਤਾਂ
		ਜੇ (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.ਖਾਨਾ;
	    ++ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ
	    getvcol(curwin, &curwin->w_cursor, &ਖਾਨਾ, ਨਿਰਾ, &endcol2, 0);

	ਖਾਨਾ += curwin->w_cursor.coladd;
	ਜੇ (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.ਖਾਨਾ))
	{
	    ਜੇ (dir == FORWARD && c == NUL)
		++ਖਾਨਾ;
	    ਜੇ (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.ਖਾਨਾ;
	    ਜੇ (c == TAB)
	    {
		ਜੇ (dir == BACKWARD && curwin->w_cursor.ਖਾਨਾ)
		    curwin->w_cursor.ਖਾਨਾ--;
		ਜੇ (dir == FORWARD && ਖਾਨਾ - 1 == endcol2)
		    curwin->w_cursor.ਖਾਨਾ++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	ਲਈ (i = 0; i < y_ਮਾਪ; ++i)
	{
	    ਪੂਰਨ		    spaces = 0;
	    ਅੱਖਰ	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    ਜੇ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		ਜੇ (ml_append(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    ਰੋਕੋ;
		++nr_ਸਤਰਾਂ;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    ਜਦੋਂ (cts.cts_vcol < ਖਾਨਾ && *cts.cts_ptr != NUL)
	    {
		// Count a tab for what it's worth (if list mode not on)
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    vcol = cts.cts_vcol;
	    ptr = cts.cts_ptr;
	    bd.textcol = (colnr_T)(ptr - oldp);
	    clear_chartabsize_arg(&cts);

	    shortline = (vcol < ਖਾਨਾ) || (vcol == ਖਾਨਾ && !*ptr) ;

	    ਜੇ (vcol < ਖਾਨਾ) // line too short, pad with spaces
		bd.startspaces = ਖਾਨਾ - vcol;
	    ਨਹੀਂਤਾਂ ਜੇ (vcol > ਖਾਨਾ)
	    {
		bd.endspaces = vcol - ਖਾਨਾ;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		ਜੇ (has_mbyte)
		    bd.textcol -= (*mb_ਸਿਰ_off)(oldp, oldp + bd.textcol);
		ਜੇ (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (ਪੂਰਨ)y_array[i].ਲੰਮਾਈ;

	    ਜੇ ((flags & PUT_ਖੰਡ_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		//ਖੰਡ
		spaces = y_ਚੌੜਾਈ + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		ਜਦੋਂ (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		ਜੇ (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    ਜੇ (yanklen + spaces != 0
		     && ਗਿਣਤੀ_2 > ((ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_ਪਾਠ_too_long));
		ਰੋਕੋ;
	    }

	    totlen = ਗਿਣਤੀ_2 * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    ਜੇ (newp == ਨਿਰਾ)
		ਰੋਕੋ;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    ਲਈ (j = 0; j < ਗਿਣਤੀ_2; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		ਜੇ ((j < ਗਿਣਤੀ_2 - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		ਨਹੀਂਤਾਂ
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
	    ਜੇ (i == 0)
		curwin->w_cursor.ਖਾਨਾ += bd.startspaces;
	}

	ਬਦਲਿਆ_ਸਤਰਾਂ(lnum, 0, curwin->w_cursor.lnum - nr_ਸਤਰਾਂ, nr_ਸਤਰਾਂ);

	// Set '[ mark.
	curbuf->b_op_ਸ਼ੁਰੂ = curwin->w_cursor;
	curbuf->b_op_ਸ਼ੁਰੂ.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_ਸਿਰਾ.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = bd.textcol + totlen - 1;
	ਜੇ (curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ < 0)
	    curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;
	curbuf->b_op_ਸਿਰਾ.coladd = 0;
	ਜੇ (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_ਸਿਰਾ;
	    curwin->w_cursor.ਖਾਨਾ++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    ਜੇ (curwin->w_cursor.ਖਾਨਾ > len)
		curwin->w_cursor.ਖਾਨਾ = len;
	}
	ਨਹੀਂਤਾਂ
	    curwin->w_cursor.lnum = lnum;
    }
    ਨਹੀਂਤਾਂ
    {
	pos_T	new_cursor;

	yanklen = (ਪੂਰਨ)y_array[0].ਲੰਮਾਈ;

	// Character or Line mode
	ਜੇ (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    ਜੇ (dir == FORWARD && gchar_cursor() != NUL)
	    {
		ਜੇ (has_mbyte)
		{
		    ਪੂਰਨ bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    ਖਾਨਾ += bytelen;
		    ਜੇ (yanklen)
		    {
			curwin->w_cursor.ਖਾਨਾ += bytelen;
			curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ += bytelen;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ++ਖਾਨਾ;
		    ਜੇ (yanklen)
		    {
			++curwin->w_cursor.ਖਾਨਾ;
			++curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ;
		    }
		}
	    }
	    curbuf->b_op_ਸ਼ੁਰੂ = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	ਨਹੀਂਤਾਂ ਜੇ (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	ਜੇ (y_type == MCHAR && y_ਮਾਪ == 1)
	{
	    linenr_T	ਸਿਰਾ_lnum = 0; // init for gcc
	    linenr_T	ਸ਼ੁਰੂ_lnum = lnum;
	    ਪੂਰਨ		ਪਹਿਲਾ_byte_off = 0;

	    ਜੇ (VIsual_ਸਰਗਰਮ)
	    {
		ਸਿਰਾ_lnum = curbuf->b_visual.vi_ਸਿਰਾ.lnum;
		ਜੇ (ਸਿਰਾ_lnum < curbuf->b_visual.vi_ਸ਼ੁਰੂ.lnum)
		    ਸਿਰਾ_lnum = curbuf->b_visual.vi_ਸ਼ੁਰੂ.lnum;
		ਜੇ (ਸਿਰਾ_lnum > ਸ਼ੁਰੂ_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.ਖਾਨਾ = ਖਾਨਾ;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, ਨਿਰਾ, &vcol, ਨਿਰਾ, 0);
		}
	    }

	    ਜੇ (ਗਿਣਤੀ_2 == 0 || yanklen == 0)
	    {
		ਜੇ (VIsual_ਸਰਗਰਮ)
		    lnum = ਸਿਰਾ_lnum;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਗਿਣਤੀ_2 > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_ਪਾਠ_too_long));
	    ਨਹੀਂਤਾਂ
	    {
		totlen = ਗਿਣਤੀ_2 * yanklen;
		ਕਰੋ
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    ਜੇ (lnum > ਸ਼ੁਰੂ_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			ਜੇ (getvpos(&pos, vcol) == OK)
			    ਖਾਨਾ = pos.ਖਾਨਾ;
			ਨਹੀਂਤਾਂ
			    ਖਾਨਾ = MAXCOL;
		    }
		    ਜੇ (VIsual_ਸਰਗਰਮ && ਖਾਨਾ > oldlen)
		    {
			lnum++;
			ਜਾਰੀ;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    ਜੇ (newp == ਨਿਰਾ)
			ਜਾਓ ਸਿਰਾ;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)ਖਾਨਾ);
		    ptr = newp + ਖਾਨਾ;
		    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
		    {
			mch_memmove(ptr, y_array[0].string, (size_t)yanklen);
			ptr += yanklen;
		    }
		    mch_memmove(ptr, oldp + ਖਾਨਾ, (size_t)(oldlen - ਖਾਨਾ) + 1);	    // +1 for NUL

		    // compute the byte offset for the last character
		    ਪਹਿਲਾ_byte_off = mb_ਸਿਰ_off(newp, ptr - 1);

		    // Note: this may free "newp"
		    ml_replace(lnum, newp, FALSE);

		    inserted_ਬਾਈਟਾਂ(lnum, ਖਾਨਾ, totlen);

		    // Place cursor on last putted char.
		    ਜੇ (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			ਬਦਲਿਆ_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.ਖਾਨਾ += (colnr_T)(totlen - 1);
		    }
		    ਜੇ (VIsual_ਸਰਗਰਮ)
			lnum++;
		} ਜਦੋਂ (VIsual_ਸਰਗਰਮ && lnum <= ਸਿਰਾ_lnum);

		ਜੇ (VIsual_ਸਰਗਰਮ) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_ਸਿਰਾ = curwin->w_cursor;
	    curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ -= ਪਹਿਲਾ_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    ਜੇ (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.ਖਾਨਾ;
	    ਨਹੀਂਤਾਂ
		curwin->w_cursor.ਖਾਨਾ -= ਪਹਿਲਾ_byte_off;
	}
	ਨਹੀਂਤਾਂ
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    ਪੂਰਨ		indent;
	    ਪੂਰਨ		orig_indent = 0;
	    ਪੂਰਨ		indent_diff = 0;	// init for gcc
	    ਪੂਰਨ		ਪਹਿਲਾ_indent = TRUE;
	    ਪੂਰਨ		lendiff = 0;
	    ਲੰਮਾ	cnt;

	    ਜੇ (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    ਲਈ (cnt = 1; cnt <= ਗਿਣਤੀ_2; ++cnt)
	    {
		i = 0;
		ਜੇ (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + ਖਾਨਾ;
		    totlen = (ਪੂਰਨ)y_array[y_ਮਾਪ - 1].ਲੰਮਾਈ;
		    newp = alloc(ml_get_len(lnum) - ਖਾਨਾ + totlen + 1);
		    ਜੇ (newp == ਨਿਰਾ)
			ਜਾਓ error;
		    STRCPY(newp, y_array[y_ਮਾਪ - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(ਖਾਨਾ + yanklen + 1);
		    ਜੇ (newp == ਨਿਰਾ)
			ਜਾਓ error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)ਖਾਨਾ);
					    // append to first line
		    mch_memmove(newp + ਖਾਨਾ, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		ਲਈ (; i < y_ਮਾਪ; ++i)
		{
		    ਜੇ (y_type != MCHAR || i < y_ਮਾਪ - 1)
		    {
			ਜੇ (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    ਜਾਓ error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_ਸਤਰਾਂ;
		    ਜੇ (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			ਜੇ (cnt == ਗਿਣਤੀ_2 && i == y_ਮਾਪ - 1)
			    lendiff = ml_get_len(lnum);
			ਜੇ (*ptr == '#' && preprocs_ਖੱਬਾ())
			    indent = 0;     // Leave # lines at start
			ਨਹੀਂਤਾਂ ਜੇ (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    ਪਹਿਲਾ_indent = FALSE;
			}
			ਨਹੀਂਤਾਂ ਜੇ ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(ਖਾਲੀ)ਸੈੱਟ_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			ਜੇ (cnt == ਗਿਣਤੀ_2 && i == y_ਮਾਪ - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		ਜੇ (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    ਜੇ (y_type == MLINE)
	    {
		curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
		ਜੇ (dir == FORWARD)
		    curbuf->b_op_ਸ਼ੁਰੂ.lnum++;
	    }
	    mark_adjust(curbuf->b_op_ਸ਼ੁਰੂ.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_ਸਤਰਾਂ, 0L);

	    // note changed text for displaying and folding
	    ਜੇ (y_type == MCHAR)
		ਬਦਲਿਆ_ਸਤਰਾਂ(curwin->w_cursor.lnum, ਖਾਨਾ,
					 curwin->w_cursor.lnum + 1, nr_ਸਤਰਾਂ);
	    ਨਹੀਂਤਾਂ
		ਬਦਲਿਆ_ਸਤਰਾਂ(curbuf->b_op_ਸ਼ੁਰੂ.lnum, 0,
					   curbuf->b_op_ਸ਼ੁਰੂ.lnum, nr_ਸਤਰਾਂ);
	    ਜੇ (y_current_ਵਰਤਿਆ != ਨਿਰਾ && (y_current_ਵਰਤਿਆ != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_ਰਜਿਸਟਰ_ਬਦਲਿਆ_while_using_it));
		ਜਾਓ ਸਿਰਾ;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_ਸਿਰਾ.lnum = new_lnum;
	    ਖਾਨਾ = MAX(0, (colnr_T)y_array[y_ਮਾਪ - 1].ਲੰਮਾਈ - lendiff);
	    ਜੇ (ਖਾਨਾ > 1)
	    {
		curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = ਖਾਨਾ - 1;
		ਜੇ (y_array[y_ਮਾਪ - 1].ਲੰਮਾਈ > 0)
		    curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ -= mb_ਸਿਰ_off(y_array[y_ਮਾਪ - 1].string,
						y_array[y_ਮਾਪ - 1].string + y_array[y_ਮਾਪ - 1].ਲੰਮਾਈ - 1);
	    }
	    ਨਹੀਂਤਾਂ
		curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;

	    ਜੇ (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		ਜੇ (y_type == MLINE)
		{
		    ਜੇ (lnum >= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		    ਨਹੀਂਤਾਂ
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.ਖਾਨਾ = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
		    curbuf->b_op_ਸਿਰਾ = curwin->w_cursor;
		    ਜੇ (ਖਾਨਾ > 1)
			curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = ਖਾਨਾ - 1;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.ਖਾਨਾ = 0;
		ਜੇ (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ਨਹੀਂਤਾਂ	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_ਸਤਰਾਂ);
    curwin->w_ਸੈੱਟ_curswant = true;

    // Make sure the cursor is not after the NUL.
    ਪੂਰਨ len = ml_get_curline_len();
    ਜੇ (curwin->w_cursor.ਖਾਨਾ > len)
    {
	ਜੇ (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.ਖਾਨਾ - len;
	curwin->w_cursor.ਖਾਨਾ = len;
    }

ਸਿਰਾ:
    ਜੇ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_ਸ਼ੁਰੂ = orig_ਸ਼ੁਰੂ;
	curbuf->b_op_ਸਿਰਾ = orig_ਸਿਰਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (has_textputpost())
    {
	ਜੇ (ਪਾਓ_string.string == ਨਿਰਾ)
	    put_do_autocmd(regname, y_current, ਨਿਰਾ, true, dir);
	ਨਹੀਂਤਾਂ
	    put_do_autocmd(regname, ਨਿਰਾ, &ਪਾਓ_string, true, dir);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (allocated)
	vim_free(ਪਾਓ_string.string);
    ਜੇ (regname == '=')
	vim_free(y_array);

    VIsual_ਸਰਗਰਮ = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    ਪੂਰਨ
get_ਰਜਿਸਟਰ_name(ਪੂਰਨ ਅੰਕ)
{
    ਜੇ (ਅੰਕ == -1)
	ਵਾਪਸ '"';
    ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕ < 10)
	ਵਾਪਸ ਅੰਕ + '0';
    ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕ == DELETION_ਰਜਿਸਟਰ)
	ਵਾਪਸ '-';
#ਜੇ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕ == ਤਾਰਾ_ਰਜਿਸਟਰ)
	ਵਾਪਸ '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    ਨਹੀਂਤਾਂ ਜੇ (clipmethod == CLIPMETHOD_PROVIDER && ਅੰਕ == REAL_PLUS_ਰਜਿਸਟਰ)
	ਵਾਪਸ '+';
    ਨਹੀਂਤਾਂ ਜੇ (clipmethod != CLIPMETHOD_PROVIDER && ਅੰਕ == PLUS_ਰਜਿਸਟਰ)
	ਵਾਪਸ '+';
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਅੰਕ + 'a' - 10;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    ਪੂਰਨ
get_unname_ਰਜਿਸਟਰ(ਖਾਲੀ)
{
    ਵਾਪਸ y_ਪਿਛਲਾ == ਨਿਰਾ ? -1 : y_ਪਿਛਲਾ - &y_regs[0];
}
#ਜੇ_ਅੰਤ

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    ਖਾਲੀ
ex_display(exarg_T *eap)
{
    ਪੂਰਨ		i, n;
    ਲੰਮਾ	j;
    char_u	*p;
    yankreg_T	*yb;
    ਪੂਰਨ		name;
    ਪੂਰਨ		attr;
    char_u	*arg = eap->arg;
    ਪੂਰਨ		clen;
    ਪੂਰਨ		type;
    string_T	ਪਾਓ;

    silence_w23_w24_ਸੁਨੇਹਾ++;
    ਜੇ (arg != ਨਿਰਾ && *arg == NUL)
	arg = ਨਿਰਾ;
    attr = HL_ATTR(HLF_8);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#ਜੇ_ਅੰਤ

    // Highlight title
    ਸੁਨੇਹਾ_puts_title(_("\nType Name Content"));
    ਲਈ (i = -1; i < ਅੰਕ_REGISTERS && !got_int; ++i)
    {
	name = get_ਰਜਿਸਟਰ_name(i);
	ਚੋਣ (get_reg_type(name, ਨਿਰਾ))
	{
	    ਹਾਲਤ MLINE: type = 'l'; ਰੋਕੋ;
	    ਹਾਲਤ MCHAR: type = 'c'; ਰੋਕੋ;
	    ਮੂਲ:	type = 'b'; ਰੋਕੋ;
	}
	ਜੇ (arg != ਨਿਰਾ && vim_strchr(arg, name) == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == ਨਿਰਾ)
#ਜੇ_ਅੰਤ
		)
	    ਜਾਰੀ;	    // did not ask for this register

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (ਖਾਲੀ)may_get_selection(name);
#ਜੇ_ਅੰਤ

	ਜੇ (i == -1)
	{
	    ਜੇ (y_ਪਿਛਲਾ != ਨਿਰਾ)
		yb = y_ਪਿਛਲਾ;
	    ਨਹੀਂਤਾਂ
		yb = &(y_regs[0]);
	}
	ਨਹੀਂਤਾਂ
	    yb = &(y_regs[i]);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != ਨਿਰਾ &&
		    (yb == y_ਪਿਛਲਾ || yb == &y_regs[0])))
	    ਜਾਰੀ;	    // do not list register being written to, the
			    // pointer can be freed
#ਜੇ_ਅੰਤ

	ਜੇ (yb->y_array != ਨਿਰਾ)
	{
	    ਪੂਰਨ do_show = FALSE;

	    ਲਈ (j = 0; !do_show && j < yb->y_ਮਾਪ; ++j)
		do_show = !ਸੁਨੇਹਾ_filtered(yb->y_array[j].string);

	    ਜੇ (do_show || yb->y_ਮਾਪ == 0)
	    {
		ਸੁਨੇਹਾ_putchar('\n');
		ਸੁਨੇਹਾ_puts("  ");
		ਸੁਨੇਹਾ_putchar(type);
		ਸੁਨੇਹਾ_puts("  ");
		ਸੁਨੇਹਾ_putchar('"');
		ਸੁਨੇਹਾ_putchar(name);
		ਸੁਨੇਹਾ_puts("   ");

		n = (ਪੂਰਨ)Columns - 11;
		ਲਈ (j = 0; j < yb->y_ਮਾਪ && n > 1; ++j)
		{
		    ਜੇ (j)
		    {
			ਸੁਨੇਹਾ_puts_attr("^J", attr);
			n -= 2;
		    }
		    ਲਈ (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			ਸੁਨੇਹਾ_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		ਜੇ (n > 1 && yb->y_type == MLINE)
		    ਸੁਨੇਹਾ_puts_attr("^J", attr);
		ਬਾਹਰਲਾ_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    ਪਾਓ = get_ਆਖਰੀ_ਪਾਓ();
    ਜੇ ((p = ਪਾਓ.string) != ਨਿਰਾ
		  && (arg == ਨਿਰਾ || vim_strchr(arg, '.') != ਨਿਰਾ) && !got_int
						      && !ਸੁਨੇਹਾ_filtered(p))
    {
	ਸੁਨੇਹਾ_puts("\n  c  \".   ");
	dis_ਸੁਨੇਹਾ(p, TRUE);
    }

    // display last command line
    ਜੇ (ਆਖਰੀ_cmdline != ਨਿਰਾ && (arg == ਨਿਰਾ || vim_strchr(arg, ':') != ਨਿਰਾ)
			       && !got_int && !ਸੁਨੇਹਾ_filtered(ਆਖਰੀ_cmdline))
    {
	ਸੁਨੇਹਾ_puts("\n  c  \":   ");
	dis_ਸੁਨੇਹਾ(ਆਖਰੀ_cmdline, FALSE);
    }

    // display current file name
    ਜੇ (curbuf->b_fname != ਨਿਰਾ
	    && (arg == ਨਿਰਾ || vim_strchr(arg, '%') != ਨਿਰਾ) && !got_int
					&& !ਸੁਨੇਹਾ_filtered(curbuf->b_fname))
    {
	ਸੁਨੇਹਾ_puts("\n  c  \"%   ");
	dis_ਸੁਨੇਹਾ(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    ਜੇ ((arg == ਨਿਰਾ || vim_strchr(arg, '#') != ਨਿਰਾ) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	ਜੇ (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !ਸੁਨੇਹਾ_filtered(fname))
	{
	    ਸੁਨੇਹਾ_puts("\n  c  \"#   ");
	    dis_ਸੁਨੇਹਾ(fname, FALSE);
	}
    }

    // display last search pattern
    ਜੇ (ਆਖਰੀ_ਖੋਜ_pat() != ਨਿਰਾ
		 && (arg == ਨਿਰਾ || vim_strchr(arg, '/') != ਨਿਰਾ) && !got_int
				      && !ਸੁਨੇਹਾ_filtered(ਆਖਰੀ_ਖੋਜ_pat()))
    {
	ਸੁਨੇਹਾ_puts("\n  c  \"/   ");
	dis_ਸੁਨੇਹਾ(ਆਖਰੀ_ਖੋਜ_pat(), FALSE);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // display last used expression
    ਜੇ (ਵਿਅੰਜਕ_ਸਤਰ != ਨਿਰਾ && (arg == ਨਿਰਾ || vim_strchr(arg, '=') != ਨਿਰਾ)
				  && !got_int && !ਸੁਨੇਹਾ_filtered(ਵਿਅੰਜਕ_ਸਤਰ))
    {
	ਸੁਨੇਹਾ_puts("\n  c  \"=   ");
	dis_ਸੁਨੇਹਾ(ਵਿਅੰਜਕ_ਸਤਰ, FALSE);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#ਜੇ_ਅੰਤ
    silence_w23_w24_ਸੁਨੇਹਾ--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    ਸਥਿਰ ਖਾਲੀ
dis_ਸੁਨੇਹਾ(
    char_u	*p,
    ਪੂਰਨ		skip_esc)	    // if TRUE, ignore trailing ESC
{
    ਪੂਰਨ		n;
    ਪੂਰਨ		l;

    n = (ਪੂਰਨ)Columns - 6;
    ਜਦੋਂ (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	ਜੇ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ਸੁਨੇਹਾ_outtrans_len(p, l);
	    p += l;
	}
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#ਜੇ defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    ਖਾਲੀ
dnd_yank_drag_ਅੰਕੜੇ(char_u *str, ਲੰਮਾ len)
{
    yankreg_T *curr;

    curr = y_current;
    y_current = &y_regs[TILDE_ਰਜਿਸਟਰ];
    free_yank_all();
    str_ਮੰਜ਼ਿਲ_reg(y_current, MCHAR, str, len, 0L, FALSE);
    y_current = curr;
}
#ਜੇ_ਅੰਤ


/*
 * Return the type of a register.
 * Used for getregtype()
 * Returns MAUTO for error.
 */
    char_u
get_reg_type(ਪੂਰਨ regname, ਲੰਮਾ *reglen)
{
    ਚੋਣ (regname)
    {
	ਹਾਲਤ '%':		// file name
	ਹਾਲਤ '#':		// alternate file name
	ਹਾਲਤ '=':		//ਵਿਅੰਜਕ
	ਹਾਲਤ ':':		// last command line
	ਹਾਲਤ '/':		// last search-pattern
	ਹਾਲਤ '.':		// last inserted text
	ਹਾਲਤ Ctrl_F:		// Filename under cursor
	ਹਾਲਤ Ctrl_P:		// Path under cursor, expand via "path"
	ਹਾਲਤ Ctrl_W:		// word under cursor
	ਹਾਲਤ Ctrl_A:		// WORD (mnemonic All) under cursor
	ਹਾਲਤ '_':		// black hole: always empty
	    ਵਾਪਸ MCHAR;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#ਜੇ_ਅੰਤ

    ਜੇ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ਵਾਪਸ MAUTO;

    get_yank_ਰਜਿਸਟਰ(regname, FALSE);

    ਜੇ (y_current->y_array != ਨਿਰਾ)
    {
	ਜੇ (reglen != ਨਿਰਾ && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_ਚੌੜਾਈ;
	ਵਾਪਸ y_current->y_type;
    }
    ਵਾਪਸ MAUTO;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    ਸਥਿਰ char_u *
getreg_wrap_one_ਸਤਰ(char_u *s, ਪੂਰਨ flags)
{
    ਜੇ (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	ਜੇ (list != ਨਿਰਾ)
	{
	    ਜੇ (list_append_string(list, ਨਿਰਾ, -1) == FAIL)
	    {
		list_free(list);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    list->lv_ਪਹਿਲਾ->li_tv.vval.v_string = s;
	}
	ਵਾਪਸ (char_u *)list;
    }
    ਵਾਪਸ s;
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
get_reg_contents(ਪੂਰਨ regname, ਪੂਰਨ flags)
{
    linenr_T	i;
    char_u	*retval;
    ਪੂਰਨ		allocated;
    ਲੰਮਾ	len;

    // Don't allow using an expression register inside an expression
    ਜੇ (regname == '=')
    {
	ਜੇ (flags & GREG_NO_ਵਿਅੰਜਕ)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (flags & GREG_ਵਿਅੰਜਕ_SRC)
	    ਵਾਪਸ getreg_wrap_one_ਸਤਰ(get_ਵਿਅੰਜਕ_ਸਤਰ_src(), flags);
	ਵਾਪਸ getreg_wrap_one_ਸਤਰ(get_ਵਿਅੰਜਕ_ਸਤਰ(), flags);
    }

    ਜੇ (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    ਜੇ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ਵਾਪਸ ਨਿਰਾ;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ਜੇ (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	ਜੇ (retval == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (allocated)
	    ਵਾਪਸ getreg_wrap_one_ਸਤਰ(retval, flags);
	ਵਾਪਸ getreg_wrap_one_ਸਤਰ(vim_strsave(retval), flags);
    }

    get_yank_ਰਜਿਸਟਰ(regname, FALSE);
    ਜੇ (y_current->y_array == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	ਜੇ (list == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	ਲਈ (i = 0; i < y_current->y_ਮਾਪ; ++i)
	{
	    ਜੇ (list_append_string(list, y_current->y_array[i].string,
		(ਪੂਰਨ)y_current->y_array[i].ਲੰਮਾਈ) == FAIL)
	    {
		list_free(list);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}
	ਵਾਪਸ (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    ਲਈ (i = 0; i < y_current->y_ਮਾਪ; ++i)
    {
	len += (ਲੰਮਾ)y_current->y_array[i].ਲੰਮਾਈ;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ਜੇ (y_current->y_type == MLINE || i < y_current->y_ਮਾਪ - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    ਜੇ (retval == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Copy the lines of the yank register into the string.
    len = 0;
    ਲਈ (i = 0; i < y_current->y_ਮਾਪ; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (ਲੰਮਾ)y_current->y_array[i].ਲੰਮਾਈ;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ਜੇ (y_current->y_type == MLINE || i < y_current->y_ਮਾਪ - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    ਵਾਪਸ retval;
}

    ਸਥਿਰ ਪੂਰਨ
init_write_reg(
    ਪੂਰਨ		name,
    yankreg_T	**old_y_ਪਿਛਲਾ,
    yankreg_T	**old_y_current,
    ਪੂਰਨ		must_append,
    ਪੂਰਨ		*yank_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	ਵਾਪਸ FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_ਪਿਛਲਾ = y_ਪਿਛਲਾ;
    *old_y_current = y_current;

    get_yank_ਰਜਿਸਟਰ(name, TRUE);
    ਜੇ (!y_append && !must_append)
	free_yank_all();
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
ਅੰਤ_write_reg(
    ਪੂਰਨ		name,
    yankreg_T	*old_y_ਪਿਛਲਾ,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_ਸੈੱਟ_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    ਜੇ (name != '"')
	y_ਪਿਛਲਾ = old_y_ਪਿਛਲਾ;
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
    ਖਾਲੀ
write_reg_contents(
    ਪੂਰਨ		name,
    char_u	*str,
    ਪੂਰਨ		maxlen,
    ਪੂਰਨ		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    ਖਾਲੀ
write_reg_contents_lst(
    ਪੂਰਨ		name,
    char_u	**ਲੜੀਆਂ,
    ਪੂਰਨ		maxlen ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		must_append,
    ਪੂਰਨ		yank_type,
    ਲੰਮਾ	ਖੰਡ_len)
{
    yankreg_T  *old_y_ਪਿਛਲਾ, *old_y_current;

    ਜੇ (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	ਜੇ (ਲੜੀਆਂ[0] == ਨਿਰਾ)
	    s = (char_u *)"";
	ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀਆਂ[1] != ਨਿਰਾ)
	{
	    semsg(_(e_ਰਜਿਸਟਰ_char_cannot_contain_ਗੁਣਜ_ਸਤਰਾਂ), name);
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ
	    s = ਲੜੀਆਂ[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, ਖੰਡ_len);
	ਵਾਪਸ;
    }

    ਜੇ (name == '_')	    // black hole: nothing to do
	ਵਾਪਸ;

    ਜੇ (init_write_reg(name, &old_y_ਪਿਛਲਾ, &old_y_current, must_append,
		&yank_type) == FAIL)
	ਵਾਪਸ;

    str_ਮੰਜ਼ਿਲ_reg(y_current, yank_type, (char_u *)ਲੜੀਆਂ, -1, ਖੰਡ_len, TRUE);

    ਅੰਤ_write_reg(name, old_y_ਪਿਛਲਾ, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_ਸੈੱਟ(name);
# endif
}

    ਖਾਲੀ
write_reg_contents_ex(
    ਪੂਰਨ		name,
    char_u	*str,
    ਪੂਰਨ		maxlen,
    ਪੂਰਨ		must_append,
    ਪੂਰਨ		yank_type,
    ਲੰਮਾ	ਖੰਡ_len)
{
    yankreg_T	*old_y_ਪਿਛਲਾ, *old_y_current;
    ਲੰਮਾ	len;

    ਜੇ (maxlen >= 0)
	len = maxlen;
    ਨਹੀਂਤਾਂ
	len = (ਲੰਮਾ)STRLEN(str);

    // Special case: '/' search pattern
    ਜੇ (name == '/')
    {
	ਸੈੱਟ_ਆਖਰੀ_ਖੋਜ_pat(str, RE_ਖੋਜ, TRUE, TRUE);
	ਵਾਪਸ;
    }

    ਜੇ (name == '#')
    {
	ਜੇ (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  ਵਾਪਸ;
	}

	buf_T	*buf;

	ਜੇ (VIM_ISDIGIT(*str))
	{
	    ਪੂਰਨ	ਅੰਕ = atoi((ਅੱਖਰ *)str);

	    buf = buflist_findnr(ਅੰਕ);
	    ਜੇ (buf == ਨਿਰਾ)
		semsg(_(e_ਬਫਰ_nr_does_not_exist), (ਲੰਮਾ)ਅੰਕ);
	}
	ਨਹੀਂਤਾਂ
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	ਜੇ (buf == ਨਿਰਾ)
	    ਵਾਪਸ;
	curwin->w_alt_fnum = buf->b_fnum;
	ਵਾਪਸ;
    }

    ਜੇ (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ (must_append && ਵਿਅੰਜਕ_ਸਤਰ != ਨਿਰਾ)
	{
	    s = concat_str(ਵਿਅੰਜਕ_ਸਤਰ, p);
	    vim_free(p);
	    p = s;
	}
	ਸੈੱਟ_ਵਿਅੰਜਕ_ਸਤਰ(p, ਨਿਰਾ);
	ਵਾਪਸ;
    }

    ਜੇ (name == '_')	    // black hole: nothing to do
	ਵਾਪਸ;

    ਜੇ (init_write_reg(name, &old_y_ਪਿਛਲਾ, &old_y_current, must_append,
		&yank_type) == FAIL)
	ਵਾਪਸ;

    str_ਮੰਜ਼ਿਲ_reg(y_current, yank_type, str, len, ਖੰਡ_len, FALSE);

    ਅੰਤ_write_reg(name, old_y_ਪਿਛਲਾ, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_ਸੈੱਟ(name);
# endif
}
#ਜੇ_ਅੰਤ	// FEAT_EVAL

#ਜੇ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    ਖਾਲੀ
str_ਮੰਜ਼ਿਲ_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    ਪੂਰਨ		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    ਲੰਮਾ	len,		// length of string
    ਲੰਮਾ	blocklen,	// width of Visual block
    ਪੂਰਨ		str_list)	// TRUE if str is char_u **
{
    ਪੂਰਨ		type;			// MCHAR, MLINE or MBLOCK
    ਪੂਰਨ		lnum;
    ਲੰਮਾ	ਸ਼ੁਰੂ;
    ਲੰਮਾ	i;
    ਪੂਰਨ		extra;
    ਪੂਰਨ		newlines;		// number of lines added
    ਪੂਰਨ		extraline = 0;		// extra line at the end
    ਪੂਰਨ		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    ਲੰਮਾ	maxlen;

    ਜੇ (y_ptr->y_array == ਨਿਰਾ)		// NULL means empty register
	y_ptr->y_ਮਾਪ = 0;

    ਜੇ (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    ਨਹੀਂਤਾਂ
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    ਜੇ (str_list)
    {
	ਲਈ (ss = (char_u **) str; *ss != ਨਿਰਾ; ++ss)
	    ++newlines;
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = 0; i < len; i++)
	    ਜੇ (str[i] == '\n')
		++newlines;
	ਜੇ (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	ਜੇ (y_ptr->y_ਮਾਪ > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    ਜੇ (y_ptr->y_ਮਾਪ + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	ਵਾਪਸ;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_ਮਾਪ + newlines) * ਆਕਾਰ(string_T), TRUE);
    ਜੇ (pp == ਨਿਰਾ)	// out of memory
	ਵਾਪਸ;
    ਲਈ (lnum = 0; lnum < y_ptr->y_ਮਾਪ; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    ਜੇ (str_list)
    {
	ਲਈ (ss = (char_u **) str; *ss != ਨਿਰਾ; ++ss, ++lnum)
	{
	    pp[lnum].ਲੰਮਾਈ = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].ਲੰਮਾਈ);
	    ਜੇ (type == MBLOCK)
	    {
		ਪੂਰਨ charlen = vim_strsize(*ss);

		ਜੇ (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (ਸ਼ੁਰੂ = 0; ਸ਼ੁਰੂ < len + extraline; ਸ਼ੁਰੂ += i + 1)
	{
	    ਪੂਰਨ charlen = 0;

	    ਲਈ (i = ਸ਼ੁਰੂ; i < len;)	// find the end of the line
	    {
		ਜੇ (str[i] == '\n')
		    ਰੋਕੋ;
		ਜੇ (type == MBLOCK)
		{
		    ਜੇ (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    ਨਹੀਂਤਾਂ
			charlen += byte2cells(str[i]);
		}

		ਜੇ (str[i] == NUL)
		    i++; // registers can have NUL chars
		ਨਹੀਂਤਾਂ
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= ਸ਼ੁਰੂ;			// i is now length of line
	    ਜੇ (charlen > maxlen)
		maxlen = charlen;
	    ਜੇ (append)
	    {
		--lnum;
		extra = (ਪੂਰਨ)y_ptr->y_array[lnum].ਲੰਮਾਈ;
	    }
	    ਨਹੀਂਤਾਂ
		extra = 0;
	    s = alloc(i + extra + 1);
	    ਜੇ (s == ਨਿਰਾ)
		ਰੋਕੋ;
	    ਜੇ (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    ਜੇ (append)
		vim_free(y_ptr->y_array[lnum].string);
	    ਜੇ (i > 0)
		mch_memmove(s + extra, str + ਸ਼ੁਰੂ, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].ਲੰਮਾਈ = extra;
	    ++lnum;
	    ਜਦੋਂ (--extra >= 0)
	    {
		ਜੇ (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_ਮਾਪ = lnum;
    ਜੇ (type == MBLOCK)
	y_ptr->y_ਚੌੜਾਈ = (blocklen < 0 ? maxlen - 1 : blocklen);
    ਨਹੀਂਤਾਂ
	y_ptr->y_ਚੌੜਾਈ = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_ਸਮਾਂ_ਸੈੱਟ = vim_ਸਮਾਂ();
# endif
}
#ਜੇ_ਅੰਤ // FEAT_CLIPBOARD || FEAT_EVAL
