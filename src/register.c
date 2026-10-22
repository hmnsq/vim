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

#അന്തര്ഭാവ "vim.h"

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
സ്ഥിതം yankreg_T	y_regs[അക്കം_REGISTERS];

സ്ഥിതം yankreg_T	*y_current;	    // ptr to current yankreg
സ്ഥിതം പൂർണ്ണസംഖ്യ		y_append;	    // TRUE when appending
സ്ഥിതം yankreg_T	*y_previous = ശൂന്യ; // ptr to last written yankreg

സ്ഥിതം പൂർണ്ണസംഖ്യ	stuff_yank(പൂർണ്ണസംഖ്യ, char_u *);
സ്ഥിതം ശൂന്യം	put_reedit_in_typebuf(പൂർണ്ണസംഖ്യ silent);
സ്ഥിതം പൂർണ്ണസംഖ്യ	put_in_typebuf(char_u *s, പൂർണ്ണസംഖ്യ esc, പൂർണ്ണസംഖ്യ colon, പൂർണ്ണസംഖ്യ silent);
സ്ഥിതം പൂർണ്ണസംഖ്യ	yank_copy_വരി(ഘടന block_def *bd, long y_idx, പൂർണ്ണസംഖ്യ exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
സ്ഥിതം ശൂന്യം	copy_yank_reg(yankreg_T *reg);
#endif
സ്ഥിതം ശൂന്യം	dis_സന്ദേശം(char_u *p, പൂർണ്ണസംഖ്യ skip_esc);

#എങ്കിൽ defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(ശൂന്യം)
{
    തിരികെ y_regs;
}
#endif

#എങ്കിൽ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(പൂർണ്ണസംഖ്യ reg)
{
    തിരികെ &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(ശൂന്യം)
{
    തിരികെ y_current;
}

    yankreg_T *
get_y_previous(ശൂന്യം)
{
    തിരികെ y_previous;
}

    ശൂന്യം
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    ശൂന്യം
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    ശൂന്യം
reset_y_append(ശൂന്യം)
{
    y_append = FALSE;
}


#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
സ്ഥിതം char_u	*വ്യഞ്ജകം_വരി = ശൂന്യ;
സ്ഥിതം exarg_T	*വ്യഞ്ജകം_eap = ശൂന്യ;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    പൂർണ്ണസംഖ്യ
get_വ്യഞ്ജകം_register(ശൂന്യം)
{
    char_u	*new_വരി;

    new_വരി = getcmdline('=', 0L, 0, 0);
    എങ്കിൽ (new_വരി == ശൂന്യ)
	തിരികെ NUL;
    എങ്കിൽ (*new_വരി == NUL)	// use previous line
	vim_free(new_വരി);
    അല്ലെങ്കിൽ
	set_വ്യഞ്ജകം_വരി(new_വരി, ശൂന്യ);
    തിരികെ '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    ശൂന്യം
set_വ്യഞ്ജകം_വരി(char_u *new_വരി, exarg_T *eap)
{
    vim_free(വ്യഞ്ജകം_വരി);
    വ്യഞ്ജകം_വരി = new_വരി;
    വ്യഞ്ജകം_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_വ്യഞ്ജകം_വരി(ശൂന്യം)
{
    char_u	*വ്യഞ്ജകം_copy;
    char_u	*rv;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	nested = 0;

    എങ്കിൽ (വ്യഞ്ജകം_വരി == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    വ്യഞ്ജകം_copy = vim_strsave(വ്യഞ്ജകം_വരി);
    എങ്കിൽ (വ്യഞ്ജകം_copy == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    എങ്കിൽ (nested >= 10)
	തിരികെ വ്യഞ്ജകം_copy;

    ++nested;
    rv = eval_to_string_eap(വ്യഞ്ജകം_copy, TRUE, വ്യഞ്ജകം_eap, FALSE);
    --nested;
    vim_free(വ്യഞ്ജകം_copy);
    തിരികെ rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    സ്ഥിതം char_u *
get_വ്യഞ്ജകം_വരി_src(ശൂന്യം)
{
    എങ്കിൽ (വ്യഞ്ജകം_വരി == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ vim_strsave(വ്യഞ്ജകം_വരി);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    പൂർണ്ണസംഖ്യ
valid_yank_reg(
    പൂർണ്ണസംഖ്യ	    regname,
    പൂർണ്ണസംഖ്യ	    writing)	    // if TRUE check for writable registers
{
    എങ്കിൽ (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#അല്ലെങ്കിൽ
				    "/#.%:"
#endif
					, regname) != ശൂന്യ)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#എങ്കിൽ defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#അല്ലെങ്കിലെങ്കിൽ defined(FEAT_CLIPBOARD_PROVIDER)
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
	തിരികെ TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    അല്ലെങ്കിൽ എങ്കിൽ (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	സന്ദേശം_warn_missing_clipboard();
	തിരികെ FALSE;
    }
#endif
    തിരികെ FALSE;
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
    പൂർണ്ണസംഖ്യ
get_yank_register(പൂർണ്ണസംഖ്യ regname, പൂർണ്ണസംഖ്യ writing)
{
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    ret = FALSE;

    y_append = FALSE;
    എങ്കിൽ ((regname == 0 || regname == '"') && !writing && y_previous != ശൂന്യ)
    {
	y_current = y_previous;
	തിരികെ ret;
    }
    i = regname;
    എങ്കിൽ (VIM_ISDIGIT(i))
	i -= '0';
    അല്ലെങ്കിൽ എങ്കിൽ (ASCII_ISLOWER(i))
	i = Char_Ord_താഴെയുള്ള(i) + 10;
    അല്ലെങ്കിൽ എങ്കിൽ (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    അല്ലെങ്കിൽ എങ്കിൽ (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    അല്ലെങ്കിൽ എങ്കിൽ (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	എങ്കിൽ (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	അല്ലെങ്കിൽ
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#അല്ലെങ്കിലെങ്കിൽ defined(FEAT_CLIPBOARD_PROVIDER)
    അല്ലെങ്കിൽ എങ്കിൽ (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    അല്ലെങ്കിൽ എങ്കിൽ (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    അല്ലെങ്കിൽ		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    എങ്കിൽ (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    തിരികെ ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    ശൂന്യം *
get_register(
    പൂർണ്ണസംഖ്യ		name,
    പൂർണ്ണസംഖ്യ		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    പൂർണ്ണസംഖ്യ		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	എങ്കിൽ (name == '*' && clip_star.available)
	{
	    എങ്കിൽ (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	എങ്കിൽ (name == '+' && clip_plus.available)
	{
	    എങ്കിൽ (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    എങ്കിൽ (reg == ശൂന്യ)
	തിരികെ (ശൂന്യം *)ശൂന്യ;

    *reg = *y_current;
    എങ്കിൽ (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	എങ്കിൽ (reg->y_size == 0 || y_current->y_array == ശൂന്യ)
	    reg->y_array = ശൂന്യ;
	അല്ലെങ്കിൽ
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	എങ്കിൽ (reg->y_array != ശൂന്യ)
	{
	    വേണ്ടി (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		എങ്കിൽ (reg->y_array[i].string == ശൂന്യ)
		{
		    // The allocation failed so clean up and exit
		    ആകുമ്പോൾ (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    തിരികെ (ശൂന്യം *)ശൂന്യ;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    അല്ലെങ്കിൽ
	y_current->y_array = ശൂന്യ;
    തിരികെ (ശൂന്യം *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    ശൂന്യം
put_register(പൂർണ്ണസംഖ്യ name, ശൂന്യം *reg)
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

#എങ്കിൽ defined(FEAT_CLIPBOARD)
    ശൂന്യം
free_register(ശൂന്യം *reg)
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
    പൂർണ്ണസംഖ്യ
yank_register_mline(പൂർണ്ണസംഖ്യ regname)
{
    എങ്കിൽ (regname != 0 && !valid_yank_reg(regname, FALSE))
	തിരികെ FALSE;
    എങ്കിൽ (regname == '_')		// black hole is always empty
	തിരികെ FALSE;
    get_yank_register(regname, FALSE);
    തിരികെ (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
do_record(പൂർണ്ണസംഖ്യ c)
{
    char_u	    *p;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    പൂർണ്ണസംഖ്യ		    retval;

    എങ്കിൽ (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	എങ്കിൽ (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	അല്ലെങ്കിൽ
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    അല്ലെങ്കിൽ			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	സന്ദേശം("");
	p = get_recorded();
	എങ്കിൽ (p == ശൂന്യ)
	    retval = FAIL;
	അല്ലെങ്കിൽ
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
    തിരികെ retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
stuff_yank(പൂർണ്ണസംഖ്യ regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    എങ്കിൽ (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	തിരികെ FAIL;
    }
    എങ്കിൽ (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	തിരികെ OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    എങ്കിൽ (y_append && y_current->y_array != ശൂന്യ)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	എങ്കിൽ (tmp == ശൂന്യ)
	{
	    vim_free(p);
	    തിരികെ FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    അല്ലെങ്കിൽ
    {
	free_yank_all();
	എങ്കിൽ ((y_current->y_array = ALLOC_ONE(string_T)) == ശൂന്യ)
	{
	    vim_free(p);
	    തിരികെ FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    തിരികെ OK;
}

/*
 * Last executed register (@ command)
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ execreg_lastc = NUL;

#എങ്കിൽ defined(FEAT_VIMINFO)
    പൂർണ്ണസംഖ്യ
get_execreg_lastc(ശൂന്യം)
{
    തിരികെ execreg_lastc;
}

    ശൂന്യം
set_execreg_lastc(പൂർണ്ണസംഖ്യ lastc)
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
    സ്ഥിതം char_u *
execreg_വരി_continuation(string_T *വരികൾ, long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    പൂർണ്ണസംഖ്യ		j;
    char_u	*str;

    ga_init2(&ga, വലുപ്പം(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    ആകുമ്പോൾ (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(വരികൾ[cmd_start].string);
	എങ്കിൽ (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    നിർത്തുക;
    }

    // join all the lines
    tmp = &വരികൾ[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    വേണ്ടി (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &വരികൾ[j];
	p = skipwhite(tmp->string);
	എങ്കിൽ (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    എങ്കിൽ (ga.ga_len > 400)
	    {
		എങ്കിൽ (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		അല്ലെങ്കിൽ
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
    തിരികെ str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
do_execreg(
    പൂർണ്ണസംഖ്യ	    regname,
    പൂർണ്ണസംഖ്യ	    colon,		// insert ':' before each line
    പൂർണ്ണസംഖ്യ	    addcr,		// always add '\n' to end of line
    പൂർണ്ണസംഖ്യ	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		retval = OK;
    പൂർണ്ണസംഖ്യ		remap;

    // repeat previous one
    എങ്കിൽ (regname == '@')
    {
	എങ്കിൽ (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    തിരികെ FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    എങ്കിൽ (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	തിരികെ FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    എങ്കിൽ (regname == '_')
	തിരികെ OK;

    // use last command line
    എങ്കിൽ (regname == ':')
    {
	എങ്കിൽ (last_cmdline == ശൂന്യ)
	{
	    emsg(_(e_no_previous_command_വരി));
	    തിരികെ FAIL;
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
	എങ്കിൽ (p != ശൂന്യ)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    എങ്കിൽ (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    അല്ലെങ്കിൽ
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    അല്ലെങ്കിൽ എങ്കിൽ (regname == '=')
    {
	p = get_വ്യഞ്ജകം_വരി();
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    അല്ലെങ്കിൽ എങ്കിൽ (regname == '.')		// use last inserted text
    {
	p = get_last_തിരുകുക_save();
	എങ്കിൽ (p == ശൂന്യ)
	{
	    emsg(_(e_no_inserted_text_yet));
	    തിരികെ FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    അല്ലെങ്കിൽ
    {
	get_yank_register(regname, FALSE);
	എങ്കിൽ (y_current->y_array == ശൂന്യ)
	    തിരികെ FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	വേണ്ടി (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    പൂർണ്ണസംഖ്യ	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    എങ്കിൽ (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		എങ്കിൽ (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    തിരികെ FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    എങ്കിൽ (colon && i > 0)
	    {
		p = skipwhite(str);
		എങ്കിൽ (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_വരി_continuation(y_current->y_array, &i);
		    എങ്കിൽ (str == ശൂന്യ)
			തിരികെ FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    എങ്കിൽ (free_str)
		vim_free(str);
	    എങ്കിൽ (escaped == ശൂന്യ)
		തിരികെ FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    എങ്കിൽ (retval == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		തിരികെ FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    തിരികെ retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    സ്ഥിതം ശൂന്യം
put_reedit_in_typebuf(പൂർണ്ണസംഖ്യ silent)
{
    എങ്കിൽ (restart_edit == NUL)
	തിരികെ;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    എങ്കിൽ (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    അല്ലെങ്കിൽ എങ്കിൽ (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    അല്ലെങ്കിൽ എങ്കിൽ (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    എങ്കിൽ (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
put_in_typebuf(
    char_u	*s,
    പൂർണ്ണസംഖ്യ		esc,
    പൂർണ്ണസംഖ്യ		colon,	    // add ':' before the line
    പൂർണ്ണസംഖ്യ		silent)
{
    പൂർണ്ണസംഖ്യ		retval = OK;

    put_reedit_in_typebuf(silent);
    എങ്കിൽ (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    എങ്കിൽ (retval == OK)
    {
	char_u	*p;

	എങ്കിൽ (esc)
	    p = vim_strsave_escape_csi(s);
	അല്ലെങ്കിൽ
	    p = s;
	എങ്കിൽ (p == ശൂന്യ)
	    retval = FAIL;
	അല്ലെങ്കിൽ
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	എങ്കിൽ (esc)
	    vim_free(p);
    }
    എങ്കിൽ (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    തിരികെ retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
തിരുകുക_reg(
    പൂർണ്ണസംഖ്യ		regname,
    പൂർണ്ണസംഖ്യ		literally_arg)	// insert literally, not as if typed
{
    long	i;
    പൂർണ്ണസംഖ്യ		retval = OK;
    char_u	*arg;
    പൂർണ്ണസംഖ്യ		allocated;
    പൂർണ്ണസംഖ്യ		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    എങ്കിൽ (got_int)
	തിരികെ FAIL;

    // check for valid regname
    എങ്കിൽ (regname != NUL && !valid_yank_reg(regname, FALSE))
	തിരികെ FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    എങ്കിൽ (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    അല്ലെങ്കിൽ എങ്കിൽ (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	എങ്കിൽ (arg == ശൂന്യ)
	    തിരികെ FAIL;
	stuffescaped(arg, literally);
	എങ്കിൽ (allocated)
	    vim_free(arg);
    }
    അല്ലെങ്കിൽ				// name or number register
    {
	എങ്കിൽ (get_yank_register(regname, FALSE))
	    literally = TRUE;
	എങ്കിൽ (y_current->y_array == ശൂന്യ)
	    retval = FAIL;
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (i = 0; i < y_current->y_size; ++i)
	    {
		എങ്കിൽ (regname == '-' && y_current->y_type == MCHAR)
		{
		    പൂർണ്ണസംഖ്യ dir = BACKWARD;
		    എങ്കിൽ ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			എങ്കിൽ (u_save_cursor() == FAIL)
			    തിരികെ FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			എങ്കിൽ (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, ശൂന്യ, dir, 1L, PUT_CURSEND);
		}
		അല്ലെങ്കിൽ
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    എങ്കിൽ (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    തിരികെ retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    പൂർണ്ണസംഖ്യ
get_spec_reg(
    പൂർണ്ണസംഖ്യ		regname,
    char_u	**argp,
    പൂർണ്ണസംഖ്യ		*allocated,	// return: TRUE when value was allocated
    പൂർണ്ണസംഖ്യ		errmsg)		// give error message when failing
{
    പൂർണ്ണസംഖ്യ		cnt;

    *argp = ശൂന്യ;
    *allocated = FALSE;
    തിരഞ്ഞെടുപ്പ് (regname)
    {
	അവസ്ഥ '%':		// file name
	    എങ്കിൽ (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    തിരികെ TRUE;

	അവസ്ഥ '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    തിരികെ TRUE;

#ifdef FEAT_EVAL
	അവസ്ഥ '=':		// result of expression
	    *argp = get_വ്യഞ്ജകം_വരി();
	    *allocated = TRUE;
	    തിരികെ TRUE;
#endif

	അവസ്ഥ ':':		// last command line
	    എങ്കിൽ (last_cmdline == ശൂന്യ && errmsg)
		emsg(_(e_no_previous_command_വരി));
	    *argp = last_cmdline;
	    തിരികെ TRUE;

	അവസ്ഥ '/':		// last search-pattern
	    എങ്കിൽ (last_തിരയൽ_pat() == ശൂന്യ && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_തിരയൽ_pat();
	    തിരികെ TRUE;

	അവസ്ഥ '.':		// last inserted text
	    *argp = get_last_തിരുകുക_save();
	    *allocated = TRUE;
	    എങ്കിൽ (*argp == ശൂന്യ && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    തിരികെ TRUE;

	അവസ്ഥ Ctrl_F:		// Filename under cursor
	അവസ്ഥ Ctrl_P:		// Path under cursor, expand via "path"
	    എങ്കിൽ (!errmsg)
		തിരികെ FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, ശൂന്യ);
	    *allocated = TRUE;
	    തിരികെ TRUE;

	അവസ്ഥ Ctrl_W:		// word under cursor
	അവസ്ഥ Ctrl_A:		// WORD (mnemonic All) under cursor
	    എങ്കിൽ (!errmsg)
		തിരികെ FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : ശൂന്യ;
	    *allocated = TRUE;
	    തിരികെ TRUE;

	അവസ്ഥ Ctrl_L:		// Line under cursor
	    എങ്കിൽ (!errmsg)
		തിരികെ FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    തിരികെ TRUE;

	അവസ്ഥ '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    തിരികെ TRUE;
    }

    തിരികെ FALSE;
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
    പൂർണ്ണസംഖ്യ
cmdline_paste_reg(
    പൂർണ്ണസംഖ്യ regname,
    പൂർണ്ണസംഖ്യ literally_arg,	// Insert text literally instead of "as typed"
    പൂർണ്ണസംഖ്യ remcr)		// don't add CR characters
{
    long	i;
    പൂർണ്ണസംഖ്യ		literally = literally_arg;

    എങ്കിൽ (get_yank_register(regname, FALSE))
	literally = TRUE;
    എങ്കിൽ (y_current->y_array == ശൂന്യ)
	തിരികെ FAIL;

    വേണ്ടി (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	എങ്കിൽ ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	എങ്കിൽ (got_int)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    ശൂന്യം
shift_delete_registers(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    വേണ്ടി (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    എങ്കിൽ (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = ശൂന്യ;		// set register one to empty
}

#എങ്കിൽ defined(FEAT_EVAL)
    സ്ഥിതം ശൂന്യം
add_regtype_to_dict(പൂർണ്ണസംഖ്യ regname, dict_T *dict, char_u *buf, പൂർണ്ണസംഖ്യ bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    തിരഞ്ഞെടുപ്പ് (get_reg_type(regname, &reglen))
    {
	അവസ്ഥ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    നിർത്തുക;
	അവസ്ഥ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    നിർത്തുക;
	അവസ്ഥ MBLOCK:
	    buflen = vim_snprintf_safelen((അക്ഷരം *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    buf[0] = NUL;
	    buflen = 0;
	    നിർത്തുക;
    }
    (ശൂന്യം)dict_add_string_len(dict, "regtype", buf, (പൂർണ്ണസംഖ്യ)buflen);
}

    ശൂന്യം
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    പൂർണ്ണസംഖ്യ		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    എങ്കിൽ (recursive)
	തിരികെ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ;

    // yanked text contents
    വേണ്ടി (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (പൂർണ്ണസംഖ്യ)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    എങ്കിൽ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ശൂന്യം)dict_add_string_len(v_event, "regname", buf, (പൂർണ്ണസംഖ്യ)buflen);

    // motion type: inclusive or exclusive
    (ശൂന്യം)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (ശൂന്യം)dict_add_string_len(v_event, "operator", buf, (പൂർണ്ണസംഖ്യ)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, വലുപ്പം(buf));

    // selection type - visual or not
    (ശൂന്യം)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_ഇനങ്ങൾ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    സ്ഥിതം ശൂന്യം
put_do_autocmd(
	പൂർണ്ണസംഖ്യ	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *തിരുകുക,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	പൂർണ്ണസംഖ്യ	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    സ്ഥിതം bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    പൂർണ്ണസംഖ്യ		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    എങ്കിൽ (recursive || regname == '_')
	തിരികെ;

    എങ്കിൽ (regname != '.' && തിരുകുക == ശൂന്യ
	    && reg == ശൂന്യ)
	// Can happen when pasting text in normal mode in a terminal buffer
	തിരികെ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    എങ്കിൽ (regname == '.')
    {
	എങ്കിൽ (last_തിരുകുക_ga.ga_data != ശൂന്യ)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_തിരുകുക_ga.ga_data,
		    (പൂർണ്ണസംഖ്യ)last_തിരുകുക_ga.ga_len);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (തിരുകുക != ശൂന്യ)
    {
	list_append_string(list, തിരുകുക->string, (പൂർണ്ണസംഖ്യ)തിരുകുക->length);
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (പൂർണ്ണസംഖ്യ)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    എങ്കിൽ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ശൂന്യം)dict_add_string_len(v_event, "regname", buf, (പൂർണ്ണസംഖ്യ)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (ശൂന്യം)dict_add_string_len(v_event, "operator", buf, (പൂർണ്ണസംഖ്യ)buflen);

    add_regtype_to_dict(regname, v_event, buf, വലുപ്പം(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (ശൂന്യം)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_ഇനങ്ങൾ_ro(v_event);

    recursive = true;
    textlock++;
    എങ്കിൽ (post)
	apply_autocmds(EVENT_TEXTPUTPOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    അല്ലെങ്കിൽ
	apply_autocmds(EVENT_TEXTPUTPRE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    ശൂന്യം
init_yank(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < അക്കം_REGISTERS; ++i)
	y_regs[i].y_array = ശൂന്യ;
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
clear_registers(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < അക്കം_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	എങ്കിൽ (y_current->y_array != ശൂന്യ)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    സ്ഥിതം ശൂന്യം
free_yank(long n)
{
    എങ്കിൽ (y_current->y_array == ശൂന്യ)
	തിരികെ;

    long	    i;

    വേണ്ടി (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    ശൂന്യം
free_yank_all(ശൂന്യം)
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
    പൂർണ്ണസംഖ്യ
op_yank(oparg_T *oap, പൂർണ്ണസംഖ്യ deleting, പൂർണ്ണസംഖ്യ mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    പൂർണ്ണസംഖ്യ			yanktype = oap->motion_type;
    long		yanklines = oap->വരി_എണ്ണം;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    ഘടന block_def	bd;

				    // check for read-only register
    എങ്കിൽ (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	തിരികെ FAIL;
    }
    എങ്കിൽ (oap->regname == '_')	    // black hole: nothing to do
	തിരികെ OK;

#ifdef FEAT_CLIPBOARD
    എങ്കിൽ ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	സന്ദേശം_warn_missing_clipboard();
    }
#endif

    എങ്കിൽ (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    എങ്കിൽ (y_append && y_current->y_array != ശൂന്യ)
	y_current = &newreg;
    അല്ലെങ്കിൽ
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    എങ്കിൽ (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(വലുപ്പം(string_T) * yanklines, TRUE);
    എങ്കിൽ (y_current->y_array == ശൂന്യ)
    {
	y_current = curr;
	തിരികെ FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    എങ്കിൽ (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	എങ്കിൽ (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    വേണ്ടി ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	തിരഞ്ഞെടുപ്പ് (y_current->y_type)
	{
	    അവസ്ഥ MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		എങ്കിൽ (yank_copy_വരി(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    പോകുക fail;
		നിർത്തുക;

	    അവസ്ഥ MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		എങ്കിൽ ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == ശൂന്യ)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    പോകുക fail;
		}
		നിർത്തുക;

	    അവസ്ഥ MCHAR:
		{
		    പൂർണ്ണസംഖ്യ tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (പൂർണ്ണസംഖ്യ)STRLEN(bd.textstart);
		    എങ്കിൽ (tmp < bd.textlen)
			bd.textlen = tmp;

		    എങ്കിൽ (yank_copy_വരി(&bd, y_idx, FALSE) == FAIL)
			പോകുക fail;
		    നിർത്തുക;
		}
		// NOTREACHED
	}
    }

    എങ്കിൽ (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	എങ്കിൽ (new_ptr == ശൂന്യ)
	    പോകുക fail;
	വേണ്ടി (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	എങ്കിൽ (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	എങ്കിൽ (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == ശൂന്യ)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    എങ്കിൽ (pnew == ശൂന്യ)
	    {
		y_idx = y_current->y_size - 1;
		പോകുക fail;
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
	അല്ലെങ്കിൽ
	    y_idx = 0;
	ആകുമ്പോൾ (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    എങ്കിൽ (mess)			// Display message about yank?
    {
	എങ്കിൽ (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	എങ്കിൽ (yanklines > p_report)
	{
	    അക്ഷരം namebuf[100];

	    എങ്കിൽ (oap->regname == NUL)
		*namebuf = NUL;
	    അല്ലെങ്കിൽ
		vim_snprintf(namebuf, വലുപ്പം(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    എങ്കിൽ (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    അല്ലെങ്കിൽ
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	എങ്കിൽ (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	എങ്കിൽ (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    എങ്കിൽ (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	എങ്കിൽ (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    എങ്കിൽ (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	എങ്കിൽ (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	എങ്കിൽ (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    എങ്കിൽ (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# എങ്കിൽ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	എങ്കിൽ (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    എങ്കിൽ (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    തിരികെ OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    തിരികെ FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
yank_copy_വരി(ഘടന block_def *bd, long y_idx, പൂർണ്ണസംഖ്യ exclude_trailing_space)
{
    char_u	*pnew;

    എങ്കിൽ (exclude_trailing_space)
	bd->endspaces = 0;
    എങ്കിൽ ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == ശൂന്യ)
	തിരികെ FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    എങ്കിൽ (exclude_trailing_space)
    {
	പൂർണ്ണസംഖ്യ s = bd->textlen + bd->endspaces;

	ആകുമ്പോൾ (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    തിരികെ OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    സ്ഥിതം ശൂന്യം
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    വലുപ്പം(string_T) * y_current->y_size, TRUE);
    എങ്കിൽ (y_current->y_array == ശൂന്യ)
	y_current->y_size = 0;
    അല്ലെങ്കിൽ
	വേണ്ടി (j = 0; j < y_current->y_size; ++j)
	{
	    എങ്കിൽ ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == ശൂന്യ)
	    {
		free_yank(j);
		y_current->y_size = 0;
		നിർത്തുക;
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
    ശൂന്യം
do_put(
    പൂർണ്ണസംഖ്യ		regname,
    char_u	*വ്യഞ്ജകം_result,	// result for regname "=" when compiled
    പൂർണ്ണസംഖ്യ		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    പൂർണ്ണസംഖ്യ		yanklen;
    പൂർണ്ണസംഖ്യ		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    പൂർണ്ണസംഖ്യ		y_type;
    long	y_size;
    പൂർണ്ണസംഖ്യ		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = ശൂന്യ;
    yankreg_T	*y_current_used = ശൂന്യ;
    long	nr_വരികൾ = 0;
    string_T	തിരുകുക_string;
    പൂർണ്ണസംഖ്യ		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned പൂർണ്ണസംഖ്യ cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(ശൂന്യം)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    എങ്കിൽ (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    എങ്കിൽ (regname == '.')
    {
	എങ്കിൽ (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	എങ്കിൽ (has_textput_events)
	    add_last_തിരുകുക++;
#endif

	(ശൂന്യം)stuff_inserted((dir == FORWARD ? (എണ്ണം == -1 ? 'o' : 'a') :
				    (എണ്ണം == -1 ? 'O' : 'i')), എണ്ണം, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	എങ്കിൽ (has_textputpre())
	    put_do_autocmd('.', ശൂന്യ, ശൂന്യ, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	എങ്കിൽ (has_textputpost())
	    put_do_autocmd('.', ശൂന്യ, ശൂന്യ, true, dir);

	എങ്കിൽ (has_textput_events && --add_last_തിരുകുക == 0)
	    ga_clear(&last_തിരുകുക_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	എങ്കിൽ ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	തിരികെ;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    തിരുകുക_string.string = ശൂന്യ;
    എങ്കിൽ (regname == '=' && വ്യഞ്ജകം_result != ശൂന്യ)
	തിരുകുക_string.string = വ്യഞ്ജകം_result;
    അല്ലെങ്കിൽ എങ്കിൽ (get_spec_reg(regname, &തിരുകുക_string.string, &allocated, TRUE)
		&& തിരുകുക_string.string == ശൂന്യ)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	തിരികെ;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    എങ്കിൽ (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	പോകുക end;

    എങ്കിൽ (തിരുകുക_string.string != ശൂന്യ)
    {
	തിരുകുക_string.length = STRLEN(തിരുകുക_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	എങ്കിൽ (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    വേണ്ടി (;;)
	    {
		y_size = 0;
		ptr = തിരുകുക_string.string;
		ptrlen = തിരുകുക_string.length;
		ആകുമ്പോൾ (ptr != ശൂന്യ)
		{
		    എങ്കിൽ (y_array != ശൂന്യ)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    എങ്കിൽ (tmp == ശൂന്യ)
		    {
			എങ്കിൽ (y_array != ശൂന്യ)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (y_array != ശൂന്യ)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			എങ്കിൽ (*tmp == NUL)
			{
			    y_type = MLINE;
			    നിർത്തുക;
			}
		    }
		    ptr = tmp;
		}
		എങ്കിൽ (y_array != ശൂന്യ)
		    നിർത്തുക;
		y_array = ALLOC_MULT(string_T, y_size);
		എങ്കിൽ (y_array == ശൂന്യ)
		    പോകുക end;
	    }
	}
	അല്ലെങ്കിൽ
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &തിരുകുക_string;
	}
#ifdef FEAT_EVAL
	എങ്കിൽ (has_textputpre())
	    put_do_autocmd(regname, ശൂന്യ, &തിരുകുക_string, false, dir);
#endif
    }
    അല്ലെങ്കിൽ
    {
#ifdef FEAT_EVAL
	എങ്കിൽ (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, ശൂന്യ, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    എങ്കിൽ (y_type == MLINE)
    {
	എങ്കിൽ (flags & PUT_വരി_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    എങ്കിൽ (u_save_cursor() == FAIL)
		പോകുക end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    എങ്കിൽ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    എങ്കിൽ (ptr == ശൂന്യ)
		പോകുക end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    എങ്കിൽ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    എങ്കിൽ (ptr == ശൂന്യ)
		പോകുക end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_വരികൾ;
	    dir = FORWARD;
	}
	എങ്കിൽ (flags & PUT_വരി_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    എങ്കിൽ (flags & PUT_വരി)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    എങ്കിൽ (y_size == 0 || y_array == ശൂന്യ)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	പോകുക end;
    }

    എങ്കിൽ (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	എങ്കിൽ (lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	    lnum = curbuf->b_ml.ml_വരി_എണ്ണം + 1;
	എങ്കിൽ (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    പോകുക end;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	എങ്കിൽ (dir == BACKWARD)
	    (ശൂന്യം)hasFolding(lnum, &lnum, ശൂന്യ);
	അല്ലെങ്കിൽ
	    (ശൂന്യം)hasFolding(lnum, ശൂന്യ, &lnum);
#endif
	എങ്കിൽ (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	എങ്കിൽ ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    പോകുക end;
#ifdef FEAT_FOLDING
	എങ്കിൽ (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	അല്ലെങ്കിൽ
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (u_save_cursor() == FAIL)
	പോകുക end;

    എങ്കിൽ (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	എങ്കിൽ (gchar_cursor() == TAB)
	{
	    പൂർണ്ണസംഖ്യ viscol = getviscol();
	    പൂർണ്ണസംഖ്യ ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    എങ്കിൽ (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#അല്ലെങ്കിൽ
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    അല്ലെങ്കിൽ
		curwin->w_cursor.coladd = 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    എങ്കിൽ (y_type == MBLOCK)
    {
	പൂർണ്ണസംഖ്യ	delcount;
	പൂർണ്ണസംഖ്യ	incr = 0;
	ഘടന block_def bd;
	long	j;
	പൂർണ്ണസംഖ്യ	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	എങ്കിൽ (dir == FORWARD && c != NUL)
	{
	    എങ്കിൽ (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, ശൂന്യ, &endcol2, 0);
	    അല്ലെങ്കിൽ
		getvcol(curwin, &curwin->w_cursor, ശൂന്യ, ശൂന്യ, &col, 0);

	    എങ്കിൽ (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    അല്ലെങ്കിൽ
		എങ്കിൽ (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	അല്ലെങ്കിൽ
	    getvcol(curwin, &curwin->w_cursor, &col, ശൂന്യ, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	എങ്കിൽ (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    എങ്കിൽ (dir == FORWARD && c == NUL)
		++col;
	    എങ്കിൽ (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    എങ്കിൽ (c == TAB)
	    {
		എങ്കിൽ (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		എങ്കിൽ (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	വേണ്ടി (i = 0; i < y_size; ++i)
	{
	    പൂർണ്ണസംഖ്യ		    spaces = 0;
	    അക്ഷരം	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    എങ്കിൽ (curwin->w_cursor.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	    {
		എങ്കിൽ (ml_append(curbuf->b_ml.ml_വരി_എണ്ണം, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    നിർത്തുക;
		++nr_വരികൾ;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    ആകുമ്പോൾ (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    എങ്കിൽ (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    അല്ലെങ്കിൽ എങ്കിൽ (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		എങ്കിൽ (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		എങ്കിൽ (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (പൂർണ്ണസംഖ്യ)y_array[i].length;

	    എങ്കിൽ ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		ആകുമ്പോൾ (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		എങ്കിൽ (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    എങ്കിൽ (yanklen + spaces != 0
		     && എണ്ണം > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		നിർത്തുക;
	    }

	    totlen = എണ്ണം * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    എങ്കിൽ (newp == ശൂന്യ)
		നിർത്തുക;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    വേണ്ടി (j = 0; j < എണ്ണം; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		എങ്കിൽ ((j < എണ്ണം - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		അല്ലെങ്കിൽ
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
	    എങ്കിൽ (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_വരികൾ(lnum, 0, curwin->w_cursor.lnum - nr_വരികൾ, nr_വരികൾ);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	എങ്കിൽ (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	എങ്കിൽ (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    എങ്കിൽ (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	അല്ലെങ്കിൽ
	    curwin->w_cursor.lnum = lnum;
    }
    അല്ലെങ്കിൽ
    {
	pos_T	new_cursor;

	yanklen = (പൂർണ്ണസംഖ്യ)y_array[0].length;

	// Character or Line mode
	എങ്കിൽ (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    എങ്കിൽ (dir == FORWARD && gchar_cursor() != NUL)
	    {
		എങ്കിൽ (has_mbyte)
		{
		    പൂർണ്ണസംഖ്യ bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    എങ്കിൽ (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    ++col;
		    എങ്കിൽ (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	അല്ലെങ്കിൽ എങ്കിൽ (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	എങ്കിൽ (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    പൂർണ്ണസംഖ്യ		first_byte_off = 0;

	    എങ്കിൽ (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		എങ്കിൽ (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		എങ്കിൽ (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, ശൂന്യ, &vcol, ശൂന്യ, 0);
		}
	    }

	    എങ്കിൽ (എണ്ണം == 0 || yanklen == 0)
	    {
		എങ്കിൽ (VIsual_active)
		    lnum = end_lnum;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (എണ്ണം > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    അല്ലെങ്കിൽ
	    {
		totlen = എണ്ണം * yanklen;
		ചെയ്യുക
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    എങ്കിൽ (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			എങ്കിൽ (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			അല്ലെങ്കിൽ
			    col = MAXCOL;
		    }
		    എങ്കിൽ (VIsual_active && col > oldlen)
		    {
			lnum++;
			തുടരുക;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    എങ്കിൽ (newp == ശൂന്യ)
			പോകുക end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
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
		    എങ്കിൽ (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    എങ്കിൽ (VIsual_active)
			lnum++;
		} ആകുമ്പോൾ (VIsual_active && lnum <= end_lnum);

		എങ്കിൽ (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    എങ്കിൽ (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    അല്ലെങ്കിൽ
		curwin->w_cursor.col -= first_byte_off;
	}
	അല്ലെങ്കിൽ
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    പൂർണ്ണസംഖ്യ		indent;
	    പൂർണ്ണസംഖ്യ		orig_indent = 0;
	    പൂർണ്ണസംഖ്യ		indent_diff = 0;	// init for gcc
	    പൂർണ്ണസംഖ്യ		first_indent = TRUE;
	    പൂർണ്ണസംഖ്യ		lendiff = 0;
	    long	cnt;

	    എങ്കിൽ (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    വേണ്ടി (cnt = 1; cnt <= എണ്ണം; ++cnt)
	    {
		i = 0;
		എങ്കിൽ (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (പൂർണ്ണസംഖ്യ)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    എങ്കിൽ (newp == ശൂന്യ)
			പോകുക error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    എങ്കിൽ (newp == ശൂന്യ)
			പോകുക error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		വേണ്ടി (; i < y_size; ++i)
		{
		    എങ്കിൽ (y_type != MCHAR || i < y_size - 1)
		    {
			എങ്കിൽ (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    പോകുക error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_വരികൾ;
		    എങ്കിൽ (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			എങ്കിൽ (cnt == എണ്ണം && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			എങ്കിൽ (*ptr == '#' && preprocs_ഇടത്())
			    indent = 0;     // Leave # lines at start
			അല്ലെങ്കിൽ എങ്കിൽ (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			അല്ലെങ്കിൽ എങ്കിൽ (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			അല്ലെങ്കിൽ എങ്കിൽ ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(ശൂന്യം)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			എങ്കിൽ (cnt == എണ്ണം && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		എങ്കിൽ (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    എങ്കിൽ (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		എങ്കിൽ (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_വരികൾ, 0L);

	    // note changed text for displaying and folding
	    എങ്കിൽ (y_type == MCHAR)
		changed_വരികൾ(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_വരികൾ);
	    അല്ലെങ്കിൽ
		changed_വരികൾ(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_വരികൾ);
	    എങ്കിൽ (y_current_used != ശൂന്യ && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		പോകുക end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    എങ്കിൽ (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		എങ്കിൽ (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    അല്ലെങ്കിൽ
		curbuf->b_op_end.col = 0;

	    എങ്കിൽ (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		എങ്കിൽ (y_type == MLINE)
		{
		    എങ്കിൽ (lnum >= curbuf->b_ml.ml_വരി_എണ്ണം)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
		    അല്ലെങ്കിൽ
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		അല്ലെങ്കിൽ
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    എങ്കിൽ (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		എങ്കിൽ (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    അല്ലെങ്കിൽ	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_വരികൾ);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    പൂർണ്ണസംഖ്യ len = ml_get_curline_len();
    എങ്കിൽ (curwin->w_cursor.col > len)
    {
	എങ്കിൽ (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    എങ്കിൽ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    എങ്കിൽ (has_textputpost())
    {
	എങ്കിൽ (തിരുകുക_string.string == ശൂന്യ)
	    put_do_autocmd(regname, y_current, ശൂന്യ, true, dir);
	അല്ലെങ്കിൽ
	    put_do_autocmd(regname, ശൂന്യ, &തിരുകുക_string, true, dir);
    }
#endif

    എങ്കിൽ (allocated)
	vim_free(തിരുകുക_string.string);
    എങ്കിൽ (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    പൂർണ്ണസംഖ്യ
get_register_name(പൂർണ്ണസംഖ്യ അക്കം)
{
    എങ്കിൽ (അക്കം == -1)
	തിരികെ '"';
    അല്ലെങ്കിൽ എങ്കിൽ (അക്കം < 10)
	തിരികെ അക്കം + '0';
    അല്ലെങ്കിൽ എങ്കിൽ (അക്കം == DELETION_REGISTER)
	തിരികെ '-';
#എങ്കിൽ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    അല്ലെങ്കിൽ എങ്കിൽ (അക്കം == STAR_REGISTER)
	തിരികെ '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    അല്ലെങ്കിൽ എങ്കിൽ (clipmethod == CLIPMETHOD_PROVIDER && അക്കം == REAL_PLUS_REGISTER)
	തിരികെ '+';
    അല്ലെങ്കിൽ എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER && അക്കം == PLUS_REGISTER)
	തിരികെ '+';
#endif
    അല്ലെങ്കിൽ
	തിരികെ അക്കം + 'a' - 10;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    പൂർണ്ണസംഖ്യ
get_unname_register(ശൂന്യം)
{
    തിരികെ y_previous == ശൂന്യ ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    ശൂന്യം
ex_display(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    പൂർണ്ണസംഖ്യ		name;
    പൂർണ്ണസംഖ്യ		attr;
    char_u	*arg = eap->arg;
    പൂർണ്ണസംഖ്യ		clen;
    പൂർണ്ണസംഖ്യ		type;
    string_T	തിരുകുക;

    silence_w23_w24_സന്ദേശം++;
    എങ്കിൽ (arg != ശൂന്യ && *arg == NUL)
	arg = ശൂന്യ;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    സന്ദേശം_puts_title(_("\nType Name Content"));
    വേണ്ടി (i = -1; i < അക്കം_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	തിരഞ്ഞെടുപ്പ് (get_reg_type(name, ശൂന്യ))
	{
	    അവസ്ഥ MLINE: type = 'l'; നിർത്തുക;
	    അവസ്ഥ MCHAR: type = 'c'; നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:	type = 'b'; നിർത്തുക;
	}
	എങ്കിൽ (arg != ശൂന്യ && vim_strchr(arg, name) == ശൂന്യ
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == ശൂന്യ)
#endif
		)
	    തുടരുക;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (ശൂന്യം)may_get_selection(name);
#endif

	എങ്കിൽ (i == -1)
	{
	    എങ്കിൽ (y_previous != ശൂന്യ)
		yb = y_previous;
	    അല്ലെങ്കിൽ
		yb = &(y_regs[0]);
	}
	അല്ലെങ്കിൽ
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	എങ്കിൽ (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != ശൂന്യ &&
		    (yb == y_previous || yb == &y_regs[0])))
	    തുടരുക;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	എങ്കിൽ (yb->y_array != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ do_show = FALSE;

	    വേണ്ടി (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    എങ്കിൽ (do_show || yb->y_size == 0)
	    {
		സന്ദേശം_putchar('\n');
		സന്ദേശം_puts("  ");
		സന്ദേശം_putchar(type);
		സന്ദേശം_puts("  ");
		സന്ദേശം_putchar('"');
		സന്ദേശം_putchar(name);
		സന്ദേശം_puts("   ");

		n = (പൂർണ്ണസംഖ്യ)Columns - 11;
		വേണ്ടി (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    എങ്കിൽ (j)
		    {
			സന്ദേശം_puts_attr("^J", attr);
			n -= 2;
		    }
		    വേണ്ടി (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			സന്ദേശം_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		എങ്കിൽ (n > 1 && yb->y_type == MLINE)
		    സന്ദേശം_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    തിരുകുക = get_last_തിരുകുക();
    എങ്കിൽ ((p = തിരുകുക.string) != ശൂന്യ
		  && (arg == ശൂന്യ || vim_strchr(arg, '.') != ശൂന്യ) && !got_int
						      && !message_filtered(p))
    {
	സന്ദേശം_puts("\n  c  \".   ");
	dis_സന്ദേശം(p, TRUE);
    }

    // display last command line
    എങ്കിൽ (last_cmdline != ശൂന്യ && (arg == ശൂന്യ || vim_strchr(arg, ':') != ശൂന്യ)
			       && !got_int && !message_filtered(last_cmdline))
    {
	സന്ദേശം_puts("\n  c  \":   ");
	dis_സന്ദേശം(last_cmdline, FALSE);
    }

    // display current file name
    എങ്കിൽ (curbuf->b_fname != ശൂന്യ
	    && (arg == ശൂന്യ || vim_strchr(arg, '%') != ശൂന്യ) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	സന്ദേശം_puts("\n  c  \"%   ");
	dis_സന്ദേശം(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    എങ്കിൽ ((arg == ശൂന്യ || vim_strchr(arg, '#') != ശൂന്യ) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	എങ്കിൽ (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    സന്ദേശം_puts("\n  c  \"#   ");
	    dis_സന്ദേശം(fname, FALSE);
	}
    }

    // display last search pattern
    എങ്കിൽ (last_തിരയൽ_pat() != ശൂന്യ
		 && (arg == ശൂന്യ || vim_strchr(arg, '/') != ശൂന്യ) && !got_int
				      && !message_filtered(last_തിരയൽ_pat()))
    {
	സന്ദേശം_puts("\n  c  \"/   ");
	dis_സന്ദേശം(last_തിരയൽ_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    എങ്കിൽ (വ്യഞ്ജകം_വരി != ശൂന്യ && (arg == ശൂന്യ || vim_strchr(arg, '=') != ശൂന്യ)
				  && !got_int && !message_filtered(വ്യഞ്ജകം_വരി))
    {
	സന്ദേശം_puts("\n  c  \"=   ");
	dis_സന്ദേശം(വ്യഞ്ജകം_വരി, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_സന്ദേശം--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    സ്ഥിതം ശൂന്യം
dis_സന്ദേശം(
    char_u	*p,
    പൂർണ്ണസംഖ്യ		skip_esc)	    // if TRUE, ignore trailing ESC
{
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		l;

    n = (പൂർണ്ണസംഖ്യ)Columns - 6;
    ആകുമ്പോൾ (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    സന്ദേശം_outtrans_len(p, l);
	    p += l;
	}
	അല്ലെങ്കിൽ
	    സന്ദേശം_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#എങ്കിൽ defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    ശൂന്യം
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
get_reg_type(പൂർണ്ണസംഖ്യ regname, long *reglen)
{
    തിരഞ്ഞെടുപ്പ് (regname)
    {
	അവസ്ഥ '%':		// file name
	അവസ്ഥ '#':		// alternate file name
	അവസ്ഥ '=':		// expression
	അവസ്ഥ ':':		// last command line
	അവസ്ഥ '/':		// last search-pattern
	അവസ്ഥ '.':		// last inserted text
	അവസ്ഥ Ctrl_F:		// Filename under cursor
	അവസ്ഥ Ctrl_P:		// Path under cursor, expand via "path"
	അവസ്ഥ Ctrl_W:		// word under cursor
	അവസ്ഥ Ctrl_A:		// WORD (mnemonic All) under cursor
	അവസ്ഥ '_':		// black hole: always empty
	    തിരികെ MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    എങ്കിൽ (regname != NUL && !valid_yank_reg(regname, FALSE))
	തിരികെ MAUTO;

    get_yank_register(regname, FALSE);

    എങ്കിൽ (y_current->y_array != ശൂന്യ)
    {
	എങ്കിൽ (reglen != ശൂന്യ && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	തിരികെ y_current->y_type;
    }
    തിരികെ MAUTO;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    സ്ഥിതം char_u *
getreg_wrap_one_വരി(char_u *s, പൂർണ്ണസംഖ്യ flags)
{
    എങ്കിൽ (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	എങ്കിൽ (list != ശൂന്യ)
	{
	    എങ്കിൽ (list_append_string(list, ശൂന്യ, -1) == FAIL)
	    {
		list_free(list);
		തിരികെ ശൂന്യ;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	തിരികെ (char_u *)list;
    }
    തിരികെ s;
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
get_reg_contents(പൂർണ്ണസംഖ്യ regname, പൂർണ്ണസംഖ്യ flags)
{
    linenr_T	i;
    char_u	*retval;
    പൂർണ്ണസംഖ്യ		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    എങ്കിൽ (regname == '=')
    {
	എങ്കിൽ (flags & GREG_NO_വ്യഞ്ജകം)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (flags & GREG_വ്യഞ്ജകം_SRC)
	    തിരികെ getreg_wrap_one_വരി(get_വ്യഞ്ജകം_വരി_src(), flags);
	തിരികെ getreg_wrap_one_വരി(get_വ്യഞ്ജകം_വരി(), flags);
    }

    എങ്കിൽ (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    എങ്കിൽ (regname != NUL && !valid_yank_reg(regname, FALSE))
	തിരികെ ശൂന്യ;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    എങ്കിൽ (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	എങ്കിൽ (retval == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (allocated)
	    തിരികെ getreg_wrap_one_വരി(retval, flags);
	തിരികെ getreg_wrap_one_വരി(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    എങ്കിൽ (y_current->y_array == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	എങ്കിൽ (list == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	വേണ്ടി (i = 0; i < y_current->y_size; ++i)
	{
	    എങ്കിൽ (list_append_string(list, y_current->y_array[i].string,
		(പൂർണ്ണസംഖ്യ)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		തിരികെ ശൂന്യ;
	    }
	}
	തിരികെ (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    വേണ്ടി (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	എങ്കിൽ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    എങ്കിൽ (retval == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Copy the lines of the yank register into the string.
    len = 0;
    വേണ്ടി (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	എങ്കിൽ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    തിരികെ retval;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
init_write_reg(
    പൂർണ്ണസംഖ്യ		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    പൂർണ്ണസംഖ്യ		must_append,
    പൂർണ്ണസംഖ്യ		*yank_type UNUSED)
{
    എങ്കിൽ (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	തിരികെ FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    എങ്കിൽ (!y_append && !must_append)
	free_yank_all();
    തിരികെ OK;
}

    സ്ഥിതം ശൂന്യം
finish_write_reg(
    പൂർണ്ണസംഖ്യ		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    എങ്കിൽ (name != '"')
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
    ശൂന്യം
write_reg_contents(
    പൂർണ്ണസംഖ്യ		name,
    char_u	*str,
    പൂർണ്ണസംഖ്യ		maxlen,
    പൂർണ്ണസംഖ്യ		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    ശൂന്യം
write_reg_contents_lst(
    പൂർണ്ണസംഖ്യ		name,
    char_u	**strings,
    പൂർണ്ണസംഖ്യ		maxlen UNUSED,
    പൂർണ്ണസംഖ്യ		must_append,
    പൂർണ്ണസംഖ്യ		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    എങ്കിൽ (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	എങ്കിൽ (strings[0] == ശൂന്യ)
	    s = (char_u *)"";
	അല്ലെങ്കിൽ എങ്കിൽ (strings[1] != ശൂന്യ)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_വരികൾ), name);
	    തിരികെ;
	}
	അല്ലെങ്കിൽ
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	തിരികെ;
    }

    എങ്കിൽ (name == '_')	    // black hole: nothing to do
	തിരികെ;

    എങ്കിൽ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	തിരികെ;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    ശൂന്യം
write_reg_contents_ex(
    പൂർണ്ണസംഖ്യ		name,
    char_u	*str,
    പൂർണ്ണസംഖ്യ		maxlen,
    പൂർണ്ണസംഖ്യ		must_append,
    പൂർണ്ണസംഖ്യ		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    എങ്കിൽ (maxlen >= 0)
	len = maxlen;
    അല്ലെങ്കിൽ
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    എങ്കിൽ (name == '/')
    {
	set_last_തിരയൽ_pat(str, RE_തിരയൽ, TRUE, TRUE);
	തിരികെ;
    }

    എങ്കിൽ (name == '#')
    {
	എങ്കിൽ (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  തിരികെ;
	}

	buf_T	*buf;

	എങ്കിൽ (VIM_ISDIGIT(*str))
	{
	    പൂർണ്ണസംഖ്യ	അക്കം = atoi((അക്ഷരം *)str);

	    buf = buflist_findnr(അക്കം);
	    എങ്കിൽ (buf == ശൂന്യ)
		semsg(_(e_buffer_nr_does_not_exist), (long)അക്കം);
	}
	അല്ലെങ്കിൽ
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	എങ്കിൽ (buf == ശൂന്യ)
	    തിരികെ;
	curwin->w_alt_fnum = buf->b_fnum;
	തിരികെ;
    }

    എങ്കിൽ (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ (must_append && വ്യഞ്ജകം_വരി != ശൂന്യ)
	{
	    s = concat_str(വ്യഞ്ജകം_വരി, p);
	    vim_free(p);
	    p = s;
	}
	set_വ്യഞ്ജകം_വരി(p, ശൂന്യ);
	തിരികെ;
    }

    എങ്കിൽ (name == '_')	    // black hole: nothing to do
	തിരികെ;

    എങ്കിൽ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	തിരികെ;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#എങ്കിൽ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    ശൂന്യം
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    പൂർണ്ണസംഖ്യ		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    പൂർണ്ണസംഖ്യ		str_list)	// TRUE if str is char_u **
{
    പൂർണ്ണസംഖ്യ		type;			// MCHAR, MLINE or MBLOCK
    പൂർണ്ണസംഖ്യ		lnum;
    long	start;
    long	i;
    പൂർണ്ണസംഖ്യ		extra;
    പൂർണ്ണസംഖ്യ		newlines;		// number of lines added
    പൂർണ്ണസംഖ്യ		extraline = 0;		// extra line at the end
    പൂർണ്ണസംഖ്യ		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    എങ്കിൽ (y_ptr->y_array == ശൂന്യ)		// NULL means empty register
	y_ptr->y_size = 0;

    എങ്കിൽ (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    അല്ലെങ്കിൽ
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    എങ്കിൽ (str_list)
    {
	വേണ്ടി (ss = (char_u **) str; *ss != ശൂന്യ; ++ss)
	    ++newlines;
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (i = 0; i < len; i++)
	    എങ്കിൽ (str[i] == '\n')
		++newlines;
	എങ്കിൽ (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	എങ്കിൽ (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    എങ്കിൽ (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	തിരികെ;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * വലുപ്പം(string_T), TRUE);
    എങ്കിൽ (pp == ശൂന്യ)	// out of memory
	തിരികെ;
    വേണ്ടി (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    എങ്കിൽ (str_list)
    {
	വേണ്ടി (ss = (char_u **) str; *ss != ശൂന്യ; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    എങ്കിൽ (type == MBLOCK)
	    {
		പൂർണ്ണസംഖ്യ charlen = vim_strsize(*ss);

		എങ്കിൽ (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (start = 0; start < len + extraline; start += i + 1)
	{
	    പൂർണ്ണസംഖ്യ charlen = 0;

	    വേണ്ടി (i = start; i < len;)	// find the end of the line
	    {
		എങ്കിൽ (str[i] == '\n')
		    നിർത്തുക;
		എങ്കിൽ (type == MBLOCK)
		{
		    എങ്കിൽ (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    അല്ലെങ്കിൽ
			charlen += byte2cells(str[i]);
		}

		എങ്കിൽ (str[i] == NUL)
		    i++; // registers can have NUL chars
		അല്ലെങ്കിൽ
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    എങ്കിൽ (charlen > maxlen)
		maxlen = charlen;
	    എങ്കിൽ (append)
	    {
		--lnum;
		extra = (പൂർണ്ണസംഖ്യ)y_ptr->y_array[lnum].length;
	    }
	    അല്ലെങ്കിൽ
		extra = 0;
	    s = alloc(i + extra + 1);
	    എങ്കിൽ (s == ശൂന്യ)
		നിർത്തുക;
	    എങ്കിൽ (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    എങ്കിൽ (append)
		vim_free(y_ptr->y_array[lnum].string);
	    എങ്കിൽ (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    ആകുമ്പോൾ (--extra >= 0)
	    {
		എങ്കിൽ (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    എങ്കിൽ (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    അല്ലെങ്കിൽ
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
