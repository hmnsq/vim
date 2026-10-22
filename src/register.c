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

#అన్తర్భావ "vim.h"

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
స్థిర yankreg_T	y_regs[అంకె_REGISTERS];

స్థిర yankreg_T	*y_current;	    // ptr to current yankreg
స్థిర పూర్ణాంకం		y_append;	    // TRUE when appending
స్థిర yankreg_T	*y_మునుపటి = ఖాళీ; // ptr to last written yankreg

స్థిర పూర్ణాంకం	stuff_yank(పూర్ణాంకం, char_u *);
స్థిర శూన్య	put_reedit_లోపల_typebuf(పూర్ణాంకం silent);
స్థిర పూర్ణాంకం	put_లోపల_typebuf(char_u *s, పూర్ణాంకం esc, పూర్ణాంకం colon, పూర్ణాంకం silent);
స్థిర పూర్ణాంకం	yank_copy_పంక్తి(నిర్మాణం ఖండం_def *bd, దీర్ఘ y_idx, పూర్ణాంకం exclude_trailing_space);
#నిర్వచితమైతే HAVE_CLIPMETHOD
స్థిర శూన్య	copy_yank_reg(yankreg_T *reg);
#అయితే_ముగింపు
స్థిర శూన్య	dis_సందేశం(char_u *p, పూర్ణాంకం skip_esc);

#ఒకవేళ defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(శూన్య)
{
    తిరిగి y_regs;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_రిజిస్టరు(పూర్ణాంకం reg)
{
    తిరిగి &y_regs[reg];
}
#అయితే_ముగింపు

    yankreg_T *
get_y_current(శూన్య)
{
    తిరిగి y_current;
}

    yankreg_T *
get_y_మునుపటి(శూన్య)
{
    తిరిగి y_మునుపటి;
}

    శూన్య
సమితి_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    శూన్య
సమితి_y_మునుపటి(yankreg_T *yreg)
{
    y_మునుపటి = yreg;
}

    శూన్య
reset_y_append(శూన్య)
{
    y_append = FALSE;
}


#ఒకవేళ defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
స్థిర char_u	*సమాసం_పంక్తి = ఖాళీ;
స్థిర exarg_T	*సమాసం_eap = ఖాళీ;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    పూర్ణాంకం
get_సమాసం_రిజిస్టరు(శూన్య)
{
    char_u	*new_పంక్తి;

    new_పంక్తి = getcmdline('=', 0L, 0, 0);
    ఒకవేళ (new_పంక్తి == ఖాళీ)
	తిరిగి NUL;
    ఒకవేళ (*new_పంక్తి == NUL)	// use previous line
	vim_free(new_పంక్తి);
    లేకపోతే
	సమితి_సమాసం_పంక్తి(new_పంక్తి, ఖాళీ);
    తిరిగి '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    శూన్య
సమితి_సమాసం_పంక్తి(char_u *new_పంక్తి, exarg_T *eap)
{
    vim_free(సమాసం_పంక్తి);
    సమాసం_పంక్తి = new_పంక్తి;
    సమాసం_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_సమాసం_పంక్తి(శూన్య)
{
    char_u	*సమాసం_copy;
    char_u	*rv;
    స్థిర పూర్ణాంకం	nested = 0;

    ఒకవేళ (సమాసం_పంక్తి == ఖాళీ)
	తిరిగి ఖాళీ;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    సమాసం_copy = vim_strsave(సమాసం_పంక్తి);
    ఒకవేళ (సమాసం_copy == ఖాళీ)
	తిరిగి ఖాళీ;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    ఒకవేళ (nested >= 10)
	తిరిగి సమాసం_copy;

    ++nested;
    rv = eval_గమ్యం_string_eap(సమాసం_copy, TRUE, సమాసం_eap, FALSE);
    --nested;
    vim_free(సమాసం_copy);
    తిరిగి rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    స్థిర char_u *
get_సమాసం_పంక్తి_src(శూన్య)
{
    ఒకవేళ (సమాసం_పంక్తి == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి vim_strsave(సమాసం_పంక్తి);
}
#అయితే_ముగింపు // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    పూర్ణాంకం
valid_yank_reg(
    పూర్ణాంకం	    regname,
    పూర్ణాంకం	    writing)	    // if TRUE check for writable registers
{
    ఒకవేళ (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#నిర్వచితమైతే FEAT_EVAL
				    "/#.%:="
#లేకపోతే
				    "/#.%:"
#అయితే_ముగింపు
					, regname) != ఖాళీ)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#ఒకవేళ defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#లేదావేళ defined(FEAT_CLIPBOARD_PROVIDER)
	    || ( // If -clipboard, then these registers only exist when
		 // clipmethod is set to provider.
	     clipmethod == CLIPMETHOD_PROVIDER && (
		 regname == '*'
		 || regname == '+'
	    ))
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DND
	    || (!writing && regname == '~')
#అయితే_ముగింపు
							)
	తిరిగి TRUE;
#నిర్వచితం_కాకపోతే FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    లేకపోతే ఒకవేళ (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	సందేశం_warn_missing_clipboard();
	తిరిగి FALSE;
    }
#అయితే_ముగింపు
    తిరిగి FALSE;
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
    పూర్ణాంకం
get_yank_రిజిస్టరు(పూర్ణాంకం regname, పూర్ణాంకం writing)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    ret = FALSE;

    y_append = FALSE;
    ఒకవేళ ((regname == 0 || regname == '"') && !writing && y_మునుపటి != ఖాళీ)
    {
	y_current = y_మునుపటి;
	తిరిగి ret;
    }
    i = regname;
    ఒకవేళ (VIM_ISDIGIT(i))
	i -= '0';
    లేకపోతే ఒకవేళ (ASCII_ISLOWER(i))
	i = Char_Ord_దిగువ(i) + 10;
    లేకపోతే ఒకవేళ (ASCII_ISUPPER(i))
    {
	i = Char_Ord_పైకి(i) + 10;
	y_append = TRUE;
    }
    లేకపోతే ఒకవేళ (regname == '-')
	i = DELETION_రిజిస్టరు;
#నిర్వచితమైతే FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    లేకపోతే ఒకవేళ (clip_నక్షత్రం.అందుబాటు && regname == '*')
    {
	i = నక్షత్రం_రిజిస్టరు;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    లేకపోతే ఒకవేళ (clip_plus.అందుబాటు && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	ఒకవేళ (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_రిజిస్టరు;
	లేకపోతే
# endif
	    i = PLUS_రిజిస్టరు;
	ret = TRUE;
    }
#లేదావేళ defined(FEAT_CLIPBOARD_PROVIDER)
    లేకపోతే ఒకవేళ (regname == '*')
    {
	i = నక్షత్రం_రిజిస్టరు;
	ret = TRUE;
    }
    లేకపోతే ఒకవేళ (regname == '+')
    {
	i = REAL_PLUS_రిజిస్టరు;
	ret = TRUE;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DND
    లేకపోతే ఒకవేళ (!writing && regname == '~')
	i = TILDE_రిజిస్టరు;
#అయితే_ముగింపు
    లేకపోతే		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    ఒకవేళ (writing)	// remember the register we write into for do_put()
	y_మునుపటి = y_current;
    తిరిగి ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    శూన్య *
get_రిజిస్టరు(
    పూర్ణాంకం		name,
    పూర్ణాంకం		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    పూర్ణాంకం		i;

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	ఒకవేళ (name == '*' && clip_నక్షత్రం.అందుబాటు)
	{
	    ఒకవేళ (clip_isautosel_నక్షత్రం())
		clip_update_selection(&clip_నక్షత్రం);
	    may_get_selection(name);
	}
	ఒకవేళ (name == '+' && clip_plus.అందుబాటు)
	{
	    ఒకవేళ (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#అయితే_ముగింపు

    get_yank_రిజిస్టరు(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    ఒకవేళ (reg == ఖాళీ)
	తిరిగి (శూన్య *)ఖాళీ;

    *reg = *y_current;
    ఒకవేళ (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	ఒకవేళ (reg->y_కొలత == 0 || y_current->y_array == ఖాళీ)
	    reg->y_array = ఖాళీ;
	లేకపోతే
	    reg->y_array = ALLOC_MULT(string_T, reg->y_కొలత);
	ఒకవేళ (reg->y_array != ఖాళీ)
	{
	    కొరకు (i = 0; i < reg->y_కొలత; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].నిడివి);
		ఒకవేళ (reg->y_array[i].string == ఖాళీ)
		{
		    // The allocation failed so clean up and exit
		    అయితే (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    తిరిగి (శూన్య *)ఖాళీ;
		}

		reg->y_array[i].నిడివి = y_current->y_array[i].నిడివి;
	    }
	}
    }
    లేకపోతే
	y_current->y_array = ఖాళీ;
    తిరిగి (శూన్య *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    శూన్య
put_రిజిస్టరు(పూర్ణాంకం name, శూన్య *reg)
{
    get_yank_రిజిస్టరు(name, 0);
    free_yank_all();
    *y_current = *(yankreg_T *)reg;
    vim_free(reg);

#నిర్వచితమైతే FEAT_CLIPBOARD
    // Send text written to clipboard register to the clipboard.
    may_సమితి_selection();
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_CLIPBOARD)
    శూన్య
free_రిజిస్టరు(శూన్య *reg)
{
    yankreg_T tmp;

    tmp = *y_current;
    *y_current = *(yankreg_T *)reg;
    free_yank_all();
    vim_free(reg);
    *y_current = tmp;
}
#అయితే_ముగింపు

/*
 * return TRUE if the current yank register has type MLINE
 */
    పూర్ణాంకం
yank_రిజిస్టరు_mline(పూర్ణాంకం regname)
{
    ఒకవేళ (regname != 0 && !valid_yank_reg(regname, FALSE))
	తిరిగి FALSE;
    ఒకవేళ (regname == '_')		// black hole is always empty
	తిరిగి FALSE;
    get_yank_రిజిస్టరు(regname, FALSE);
    తిరిగి (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
do_రికార్డు(పూర్ణాంకం c)
{
    char_u	    *p;
    స్థిర పూర్ణాంకం	    regname;
    yankreg_T	    *old_y_మునుపటి, *old_y_current;
    పూర్ణాంకం		    retval;

    ఒకవేళ (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	ఒకవేళ (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	లేకపోతే
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    లేకపోతే			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	సందేశం("");
	p = get_recorded();
	ఒకవేళ (p == ఖాళీ)
	    retval = FAIL;
	లేకపోతే
	{
	    // Remove escaping for CSI and K_SPECIAL in multi-byte chars.
	    vim_unescape_csi(p);

	    // We don't want to change the default register here, so save and
	    // restore the current register name.
	    old_y_మునుపటి = y_మునుపటి;
	    old_y_current = y_current;

	    retval = stuff_yank(regname, p);

	    y_మునుపటి = old_y_మునుపటి;
	    y_current = old_y_current;
	}
    }
    తిరిగి retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    స్థిర పూర్ణాంకం
stuff_yank(పూర్ణాంకం regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    ఒకవేళ (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	తిరిగి FAIL;
    }
    ఒకవేళ (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	తిరిగి OK;
    }

    plen = STRLEN(p);
    get_yank_రిజిస్టరు(regname, TRUE);
    ఒకవేళ (y_append && y_current->y_array != ఖాళీ)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_కొలత - 1]);
	tmplen = pp->నిడివి + plen;
	tmp = alloc(tmplen + 1);
	ఒకవేళ (tmp == ఖాళీ)
	{
	    vim_free(p);
	    తిరిగి FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->నిడివి, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->నిడివి = tmplen;
    }
    లేకపోతే
    {
	free_yank_all();
	ఒకవేళ ((y_current->y_array = ALLOC_ONE(string_T)) == ఖాళీ)
	{
	    vim_free(p);
	    తిరిగి FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].నిడివి = plen;
	y_current->y_కొలత = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#నిర్వచితమైతే FEAT_VIMINFO
	y_current->y_సమయం_సమితి = vim_సమయం();
#అయితే_ముగింపు
    }
    తిరిగి OK;
}

/*
 * Last executed register (@ command)
 */
స్థిర పూర్ణాంకం execreg_lastc = NUL;

#ఒకవేళ defined(FEAT_VIMINFO)
    పూర్ణాంకం
get_execreg_lastc(శూన్య)
{
    తిరిగి execreg_lastc;
}

    శూన్య
సమితి_execreg_lastc(పూర్ణాంకం lastc)
{
    execreg_lastc = lastc;
}
#అయితే_ముగింపు

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
    స్థిర char_u *
execreg_పంక్తి_continuation(string_T *పంక్తులు, దీర్ఘ *idx)
{
    garray_T	ga;
    దీర్ఘ	cmd_ప్రారంభం = *idx;
    దీర్ఘ	cmd_చివర = *idx;
    string_T	*tmp;
    పూర్ణాంకం		j;
    char_u	*str;

    ga_init2(&ga, పరిమాణం(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    అయితే (--cmd_ప్రారంభం > 0)
    {
	char_u	*p;

	p = skipwhite(పంక్తులు[cmd_ప్రారంభం].string);
	ఒకవేళ (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    ఆపు;
    }

    // join all the lines
    tmp = &పంక్తులు[cmd_ప్రారంభం];
    ga_concat_len(&ga, tmp->string, tmp->నిడివి);
    కొరకు (j = cmd_ప్రారంభం + 1; j <= cmd_చివర; j++)
    {
	char_u	*p;

	tmp = &పంక్తులు[j];
	p = skipwhite(tmp->string);
	ఒకవేళ (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    ఒకవేళ (ga.ga_len > 400)
	    {
		ఒకవేళ (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		లేకపోతే
		    ga.ga_growsize = ga.ga_len;
	    }
	    p++;
	    ga_concat_len(&ga, p, (tmp->string + tmp->నిడివి) - p);
	}
    }
    ga_append(&ga, NUL);
    str = vim_strnsave(ga.ga_దత్తాంశం, ga.ga_len);
    ga_clear(&ga);

    *idx = cmd_ప్రారంభం;
    తిరిగి str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
do_execreg(
    పూర్ణాంకం	    regname,
    పూర్ణాంకం	    colon,		// insert ':' before each line
    పూర్ణాంకం	    addcr,		// always add '\n' to end of line
    పూర్ణాంకం	    silent)		// set "silent" flag in typeahead buffer
{
    దీర్ఘ	i;
    char_u	*p;
    పూర్ణాంకం		retval = OK;
    పూర్ణాంకం		remap;

    // repeat previous one
    ఒకవేళ (regname == '@')
    {
	ఒకవేళ (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_వాడిన_రిజిస్టరు));
	    తిరిగి FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    ఒకవేళ (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	తిరిగి FAIL;
    }
    execreg_lastc = regname;

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#అయితే_ముగింపు

    // black hole: don't stuff anything
    ఒకవేళ (regname == '_')
	తిరిగి OK;

    // use last command line
    ఒకవేళ (regname == ':')
    {
	ఒకవేళ (చివరి_cmdline == ఖాళీ)
	{
	    emsg(_(e_no_మునుపటి_command_పంక్తి));
	    తిరిగి FAIL;
	}
	// don't keep the cmdline containing @:
	VIM_CLEAR(new_చివరి_cmdline);
	// Escape all control characters with a CTRL-V
	p = vim_strsave_escaped_ext(చివరి_cmdline,
		    (char_u *)"\001\002\003\004\005\006\007"
			  "\010\011\012\013\014\015\016\017"
			  "\020\021\022\023\024\025\026\027"
			  "\030\031\032\033\034\035\036\037",
		    Ctrl_V, FALSE);
	ఒకవేళ (p != ఖాళీ)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    ఒకవేళ (VIsual_చురుకైన && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_లోపల_typebuf(p + 5, TRUE, TRUE, silent);
	    లేకపోతే
		retval = put_లోపల_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#నిర్వచితమైతే FEAT_EVAL
    లేకపోతే ఒకవేళ (regname == '=')
    {
	p = get_సమాసం_పంక్తి();
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి FAIL;
	retval = put_లోపల_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#అయితే_ముగింపు
    లేకపోతే ఒకవేళ (regname == '.')		// use last inserted text
    {
	p = get_చివరి_చొప్పించు_save();
	ఒకవేళ (p == ఖాళీ)
	{
	    emsg(_(e_no_inserted_పాఠ్యం_yet));
	    తిరిగి FAIL;
	}
	retval = put_లోపల_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    లేకపోతే
    {
	get_yank_రిజిస్టరు(regname, FALSE);
	ఒకవేళ (y_current->y_array == ఖాళీ)
	    తిరిగి FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_లోపల_typebuf(silent);
	కొరకు (i = y_current->y_కొలత; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    పూర్ణాంకం	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    ఒకవేళ (y_current->y_type == MLINE || i < y_current->y_కొలత - 1
								     || addcr)
	    {
		ఒకవేళ (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    తిరిగి FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    ఒకవేళ (colon && i > 0)
	    {
		p = skipwhite(str);
		ఒకవేళ (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_పంక్తి_continuation(y_current->y_array, &i);
		    ఒకవేళ (str == ఖాళీ)
			తిరిగి FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    ఒకవేళ (free_str)
		vim_free(str);
	    ఒకవేళ (escaped == ఖాళీ)
		తిరిగి FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    ఒకవేళ (retval == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		తిరిగి FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_చివర_reg_executing = FALSE;
    }
    తిరిగి retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    స్థిర శూన్య
put_reedit_లోపల_typebuf(పూర్ణాంకం silent)
{
    ఒకవేళ (restart_edit == NUL)
	తిరిగి;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    ఒకవేళ (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    లేకపోతే ఒకవేళ (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    లేకపోతే ఒకవేళ (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    ఒకవేళ (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    స్థిర పూర్ణాంకం
put_లోపల_typebuf(
    char_u	*s,
    పూర్ణాంకం		esc,
    పూర్ణాంకం		colon,	    // add ':' before the line
    పూర్ణాంకం		silent)
{
    పూర్ణాంకం		retval = OK;

    put_reedit_లోపల_typebuf(silent);
    ఒకవేళ (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    ఒకవేళ (retval == OK)
    {
	char_u	*p;

	ఒకవేళ (esc)
	    p = vim_strsave_escape_csi(s);
	లేకపోతే
	    p = s;
	ఒకవేళ (p == ఖాళీ)
	    retval = FAIL;
	లేకపోతే
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	ఒకవేళ (esc)
	    vim_free(p);
    }
    ఒకవేళ (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    తిరిగి retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
చొప్పించు_reg(
    పూర్ణాంకం		regname,
    పూర్ణాంకం		literally_arg)	// insert literally, not as if typed
{
    దీర్ఘ	i;
    పూర్ణాంకం		retval = OK;
    char_u	*arg;
    పూర్ణాంకం		allocated;
    పూర్ణాంకం		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    ఒకవేళ (got_int)
	తిరిగి FAIL;

    // check for valid regname
    ఒకవేళ (regname != NUL && !valid_yank_reg(regname, FALSE))
	తిరిగి FAIL;

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#అయితే_ముగింపు

    ఒకవేళ (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    లేకపోతే ఒకవేళ (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	ఒకవేళ (arg == ఖాళీ)
	    తిరిగి FAIL;
	stuffescaped(arg, literally);
	ఒకవేళ (allocated)
	    vim_free(arg);
    }
    లేకపోతే				// name or number register
    {
	ఒకవేళ (get_yank_రిజిస్టరు(regname, FALSE))
	    literally = TRUE;
	ఒకవేళ (y_current->y_array == ఖాళీ)
	    retval = FAIL;
	లేకపోతే
	{
	    కొరకు (i = 0; i < y_current->y_కొలత; ++i)
	    {
		ఒకవేళ (regname == '-' && y_current->y_type == MCHAR)
		{
		    పూర్ణాంకం dir = BACKWARD;
		    ఒకవేళ ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			ఒకవేళ (u_save_cursor() == FAIL)
			    తిరిగి FAIL;
			del_chars((దీర్ఘ)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			ఒకవేళ (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    Append_Char_గమ్యం_Redobuff(Ctrl_R);
		    Append_Char_గమ్యం_Redobuff(regname);
		    do_put(regname, ఖాళీ, dir, 1L, PUT_CURSEND);
		}
		లేకపోతే
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    ఒకవేళ (y_current->y_type == MLINE || i < y_current->y_కొలత - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    తిరిగి retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    పూర్ణాంకం
get_spec_reg(
    పూర్ణాంకం		regname,
    char_u	**argp,
    పూర్ణాంకం		*allocated,	// return: TRUE when value was allocated
    పూర్ణాంకం		errmsg)		// give error message when failing
{
    పూర్ణాంకం		cnt;

    *argp = ఖాళీ;
    *allocated = FALSE;
    ఎంపిక (regname)
    {
	స్థితి '%':		// file name
	    ఒకవేళ (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    తిరిగి TRUE;

	స్థితి '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    తిరిగి TRUE;

#నిర్వచితమైతే FEAT_EVAL
	స్థితి '=':		// result of expression
	    *argp = get_సమాసం_పంక్తి();
	    *allocated = TRUE;
	    తిరిగి TRUE;
#అయితే_ముగింపు

	స్థితి ':':		// last command line
	    ఒకవేళ (చివరి_cmdline == ఖాళీ && errmsg)
		emsg(_(e_no_మునుపటి_command_పంక్తి));
	    *argp = చివరి_cmdline;
	    తిరిగి TRUE;

	స్థితి '/':		// last search-pattern
	    ఒకవేళ (చివరి_శోధన_pat() == ఖాళీ && errmsg)
		emsg(_(e_no_మునుపటి_regular_వ్యక్తీకరణ));
	    *argp = చివరి_శోధన_pat();
	    తిరిగి TRUE;

	స్థితి '.':		// last inserted text
	    *argp = get_చివరి_చొప్పించు_save();
	    *allocated = TRUE;
	    ఒకవేళ (*argp == ఖాళీ && errmsg)
		emsg(_(e_no_inserted_పాఠ్యం_yet));
	    తిరిగి TRUE;

	స్థితి Ctrl_F:		// Filename under cursor
	స్థితి Ctrl_P:		// Path under cursor, expand via "path"
	    ఒకవేళ (!errmsg)
		తిరిగి FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, ఖాళీ);
	    *allocated = TRUE;
	    తిరిగి TRUE;

	స్థితి Ctrl_W:		// word under cursor
	స్థితి Ctrl_A:		// WORD (mnemonic All) under cursor
	    ఒకవేళ (!errmsg)
		తిరిగి FALSE;
	    cnt = కనుగొను_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (కనుగొను_IDENT|కనుగొను_STRING) : కనుగొను_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : ఖాళీ;
	    *allocated = TRUE;
	    తిరిగి TRUE;

	స్థితి Ctrl_L:		// Line under cursor
	    ఒకవేళ (!errmsg)
		తిరిగి FALSE;

	    *argp = ml_get_buf(curwin->w_బఫరు,
			curwin->w_cursor.lnum, FALSE);
	    తిరిగి TRUE;

	స్థితి '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    తిరిగి TRUE;
    }

    తిరిగి FALSE;
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
    పూర్ణాంకం
cmdline_paste_reg(
    పూర్ణాంకం regname,
    పూర్ణాంకం literally_arg,	// Insert text literally instead of "as typed"
    పూర్ణాంకం remcr)		// don't add CR characters
{
    దీర్ఘ	i;
    పూర్ణాంకం		literally = literally_arg;

    ఒకవేళ (get_yank_రిజిస్టరు(regname, FALSE))
	literally = TRUE;
    ఒకవేళ (y_current->y_array == ఖాళీ)
	తిరిగి FAIL;

    కొరకు (i = 0; i < y_current->y_కొలత; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	ఒకవేళ ((y_current->y_type == MLINE || i < y_current->y_కొలత - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	ఒకవేళ (got_int)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    శూన్య
జరుపు_delete_registers(శూన్య)
{
    పూర్ణాంకం		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    కొరకు (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    ఒకవేళ (!y_append)
	y_మునుపటి = y_current;
    y_regs[1].y_array = ఖాళీ;		// set register one to empty
}

#ఒకవేళ defined(FEAT_EVAL)
    స్థిర శూన్య
కూడు_regtype_గమ్యం_dict(పూర్ణాంకం regname, dict_T *dict, char_u *buf, పూర్ణాంకం bufsize)
{
    size_t  buflen;
    దీర్ఘ    reglen;
    // Register type
    ఎంపిక (get_reg_type(regname, &reglen))
    {
	స్థితి MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ఆపు;
	స్థితి MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ఆపు;
	స్థితి MBLOCK:
	    buflen = vim_snprintf_safelen((అక్షరం *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    ఆపు;
	అప్రమేయ:
	    buf[0] = NUL;
	    buflen = 0;
	    ఆపు;
    }
    (శూన్య)dict_కూడు_string_len(dict, "regtype", buf, (పూర్ణాంకం)buflen);
}

    శూన్య
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    స్థిర పూర్ణాంకం	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    పూర్ణాంకం		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ఒకవేళ (recursive)
	తిరిగి;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ఒకవేళ (list == ఖాళీ)
	తిరిగి;

    // yanked text contents
    కొరకు (n = 0; n < reg->y_కొలత; n++)
	list_append_string(list, reg->y_array[n].string, (పూర్ణాంకం)reg->y_array[n].నిడివి);
    list->lv_తాళం = VAR_FIXED;
    ఒకవేళ (dict_కూడు_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (శూన్య)dict_కూడు_string_len(v_event, "regname", buf, (పూర్ణాంకం)buflen);

    // motion type: inclusive or exclusive
    (శూన్య)dict_కూడు_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (శూన్య)dict_కూడు_string_len(v_event, "operator", buf, (పూర్ణాంకం)buflen);

    కూడు_regtype_గమ్యం_dict(oap->regname, v_event, buf, పరిమాణం(buf));

    // selection type - visual or not
    (శూన్య)dict_కూడు_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_సమితి_వస్తువులు_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, ఖాళీ, ఖాళీ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    స్థిర శూన్య
put_do_autocmd(
	పూర్ణాంకం	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *చొప్పించు,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	పూర్ణాంకం	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    స్థిర bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    పూర్ణాంకం		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ఒకవేళ (recursive || regname == '_')
	తిరిగి;

    ఒకవేళ (regname != '.' && చొప్పించు == ఖాళీ
	    && reg == ఖాళీ)
	// Can happen when pasting text in normal mode in a terminal buffer
	తిరిగి;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ఒకవేళ (list == ఖాళీ)
	తిరిగి;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ఒకవేళ (regname == '.')
    {
	ఒకవేళ (చివరి_చొప్పించు_ga.ga_దత్తాంశం != ఖాళీ)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, చివరి_చొప్పించు_ga.ga_దత్తాంశం,
		    (పూర్ణాంకం)చివరి_చొప్పించు_ga.ga_len);
    }
    లేకపోతే ఒకవేళ (చొప్పించు != ఖాళీ)
    {
	list_append_string(list, చొప్పించు->string, (పూర్ణాంకం)చొప్పించు->నిడివి);
    }
    లేకపోతే
    {
	కొరకు (n = 0; n < reg->y_కొలత; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (పూర్ణాంకం)reg->y_array[n].నిడివి);
    }

    list->lv_తాళం = VAR_FIXED;
    ఒకవేళ (dict_కూడు_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (శూన్య)dict_కూడు_string_len(v_event, "regname", buf, (పూర్ణాంకం)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (శూన్య)dict_కూడు_string_len(v_event, "operator", buf, (పూర్ణాంకం)buflen);

    కూడు_regtype_గమ్యం_dict(regname, v_event, buf, పరిమాణం(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (శూన్య)dict_కూడు_bool(v_event, "visual", VIsual_చురుకైన);

    // Lock the dictionary and its keys
    dict_సమితి_వస్తువులు_ro(v_event);

    recursive = true;
    textlock++;
    ఒకవేళ (post)
	apply_autocmds(EVENT_TEXTPUTPOST, ఖాళీ, ఖాళీ, FALSE, curbuf);
    లేకపోతే
	apply_autocmds(EVENT_TEXTPUTPRE, ఖాళీ, ఖాళీ, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#అయితే_ముగింపు

/*
 * set all the yank registers to empty (called from main())
 */
    శూన్య
init_yank(శూన్య)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < అంకె_REGISTERS; ++i)
	y_regs[i].y_array = ఖాళీ;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
clear_registers(శూన్య)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < అంకె_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	ఒకవేళ (y_current->y_array != ఖాళీ)
	    free_yank_all();
    }
}
#అయితే_ముగింపు

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    స్థిర శూన్య
free_yank(దీర్ఘ n)
{
    ఒకవేళ (y_current->y_array == ఖాళీ)
	తిరిగి;

    దీర్ఘ	    i;

    కొరకు (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    శూన్య
free_yank_all(శూన్య)
{
    free_yank(y_current->y_కొలత);
}

/*
 * Yank the text between "oap->start" and "oap->end" into a yank register.
 * If we are to append (uppercase register), we first yank into a new yank
 * register and then concatenate the old and the new one (so we keep the old
 * one in case of out-of-memory).
 *
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
op_yank(oparg_T *oap, పూర్ణాంకం deleting, పూర్ణాంకం mess)
{
    దీర్ఘ		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    పూర్ణాంకం			yanktype = oap->motion_type;
    దీర్ఘ		yanklines = oap->పంక్తి_లెక్క;
    linenr_T		yankendlnum = oap->చివర.lnum;
    char_u		*pnew;
    నిర్మాణం ఖండం_def	bd;

				    // check for read-only register
    ఒకవేళ (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	తిరిగి FAIL;
    }
    ఒకవేళ (oap->regname == '_')	    // black hole: nothing to do
	తిరిగి OK;

#నిర్వచితమైతే FEAT_CLIPBOARD
    ఒకవేళ ((!clip_నక్షత్రం.అందుబాటు && oap->regname == '*') ||
	(!clip_plus.అందుబాటు && oap->regname == '+'))
    {
	oap->regname = 0;
	సందేశం_warn_missing_clipboard();
    }
#అయితే_ముగింపు

    ఒకవేళ (!deleting)		    // op_delete() already set y_current
	get_yank_రిజిస్టరు(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    ఒకవేళ (y_append && y_current->y_array != ఖాళీ)
	y_current = &newreg;
    లేకపోతే
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    ఒకవేళ (       oap->motion_type == MCHAR
	    && oap->ప్రారంభం.నిలువు == 0
	    && !oap->inclusive
	    && (!oap->is_VIsual || *p_sel == 'o')
	    && !oap->ఖండం_బహుళకం
	    && oap->చివర.నిలువు == 0
	    && yanklines > 1)
    {
	yanktype = MLINE;
	--yankendlnum;
	--yanklines;
    }

    y_current->y_కొలత = yanklines;
    y_current->y_type = yanktype;   // set the yank register type
    y_current->y_వెడల్పు = 0;
    y_current->y_array = lalloc_clear(పరిమాణం(string_T) * yanklines, TRUE);
    ఒకవేళ (y_current->y_array == ఖాళీ)
    {
	y_current = curr;
	తిరిగి FAIL;
    }
#నిర్వచితమైతే FEAT_VIMINFO
    y_current->y_సమయం_సమితి = vim_సమయం();
#అయితే_ముగింపు

    y_idx = 0;
    lnum = oap->ప్రారంభం.lnum;

    ఒకవేళ (oap->ఖండం_బహుళకం)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_వెడల్పు = oap->చివర_vcol - oap->ప్రారంభం_vcol;

	ఒకవేళ (curwin->w_curswant == MAXCOL && y_current->y_వెడల్పు > 0)
	    y_current->y_వెడల్పు--;
    }

    కొరకు ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	ఎంపిక (y_current->y_type)
	{
	    స్థితి MBLOCK:
		ఖండం_prep(oap, &bd, lnum, FALSE);
		ఒకవేళ (yank_copy_పంక్తి(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    వెళ్ళు fail;
		ఆపు;

	    స్థితి MLINE:
		y_current->y_array[y_idx].నిడివి = ml_get_len(lnum);
		ఒకవేళ ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].నిడివి)) == ఖాళీ)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    వెళ్ళు fail;
		}
		ఆపు;

	    స్థితి MCHAR:
		{
		    పూర్ణాంకం tmp;

		    charwise_ఖండం_prep(oap->ప్రారంభం, oap->చివర, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (పూర్ణాంకం)STRLEN(bd.textstart);
		    ఒకవేళ (tmp < bd.textlen)
			bd.textlen = tmp;

		    ఒకవేళ (yank_copy_పంక్తి(&bd, y_idx, FALSE) == FAIL)
			వెళ్ళు fail;
		    ఆపు;
		}
		// NOTREACHED
	}
    }

    ఒకవేళ (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	దీర్ఘ j;

	new_ptr = ALLOC_MULT(string_T, curr->y_కొలత + y_current->y_కొలత);
	ఒకవేళ (new_ptr == ఖాళీ)
	    వెళ్ళు fail;
	కొరకు (j = 0; j < curr->y_కొలత; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#నిర్వచితమైతే FEAT_VIMINFO
	curr->y_సమయం_సమితి = vim_సమయం();
#అయితే_ముగింపు

	ఒకవేళ (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	ఒకవేళ (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == ఖాళీ)
	{
	    pnew = alloc(curr->y_array[curr->y_కొలత - 1].నిడివి + y_current->y_array[0].నిడివి + 1);
	    ఒకవేళ (pnew == ఖాళీ)
	    {
		y_idx = y_current->y_కొలత - 1;
		వెళ్ళు fail;
	    }

	    --j;
	    STRCPY(pnew, curr->y_array[j].string);
	    STRCPY(pnew + curr->y_array[j].నిడివి, y_current->y_array[0].string);
	    vim_free(curr->y_array[j].string);
	    curr->y_array[j].string = pnew;
	    curr->y_array[j].నిడివి = curr->y_array[j].నిడివి + y_current->y_array[0].నిడివి;
	    ++j;
	    VIM_CLEAR_STRING(y_current->y_array[0]);
	    y_idx = 1;
	}
	లేకపోతే
	    y_idx = 0;
	అయితే (y_idx < y_current->y_కొలత)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_కొలత = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    ఒకవేళ (mess)			// Display message about yank?
    {
	ఒకవేళ (yanktype == MCHAR
		&& !oap->ఖండం_బహుళకం
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	ఒకవేళ (yanklines > p_report)
	{
	    అక్షరం namebuf[100];

	    ఒకవేళ (oap->regname == NUL)
		*namebuf = NUL;
	    లేకపోతే
		vim_snprintf(namebuf, పరిమాణం(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    ఒకవేళ (oap->ఖండం_బహుళకం)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    లేకపోతే
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_ప్రారంభం = oap->ప్రారంభం;
	curbuf->b_op_చివర = oap->చివర;
	ఒకవేళ (yanktype == MLINE && !oap->ఖండం_బహుళకం)
	{
	    curbuf->b_op_ప్రారంభం.నిలువు = 0;
	    curbuf->b_op_చివర.నిలువు = MAXCOL;
	}
	ఒకవేళ (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_చివర);
    }

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    ఒకవేళ (curr == &y_regs[REAL_PLUS_రిజిస్టరు] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	ఒకవేళ (curr != &(y_regs[REAL_PLUS_రిజిస్టరు]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_రిజిస్టరు]));
	call_clip_provider_సమితి('+');
    }
    ఒకవేళ (curr == &y_regs[నక్షత్రం_రిజిస్టరు] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	ఒకవేళ (curr != &(y_regs[నక్షత్రం_రిజిస్టరు]))
	    copy_yank_reg(&(y_regs[నక్షత్రం_రిజిస్టరు]));
	call_clip_provider_సమితి('*');
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIPBOARD
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	ఒకవేళ (clip_నక్షత్రం.అందుబాటు
		&& (curr == &(y_regs[నక్షత్రం_రిజిస్టరు])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    ఒకవేళ (curr != &(y_regs[నక్షత్రం_రిజిస్టరు]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[నక్షత్రం_రిజిస్టరు]));

	    clip_own_selection(&clip_నక్షత్రం);
	    clip_gen_సమితి_selection(&clip_నక్షత్రం);
	}

# ఒకవేళ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	ఒకవేళ (clip_plus.అందుబాటు
		&& (curr == &(y_regs[PLUS_రిజిస్టరు])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    ఒకవేళ (curr != &(y_regs[PLUS_రిజిస్టరు]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_రిజిస్టరు]));

	    clip_own_selection(&clip_plus);
	    clip_gen_సమితి_selection(&clip_plus);
	}
# endif
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#అయితే_ముగింపు
    తిరిగి OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    తిరిగి FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    స్థిర పూర్ణాంకం
yank_copy_పంక్తి(నిర్మాణం ఖండం_def *bd, దీర్ఘ y_idx, పూర్ణాంకం exclude_trailing_space)
{
    char_u	*pnew;

    ఒకవేళ (exclude_trailing_space)
	bd->endspaces = 0;
    ఒకవేళ ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == ఖాళీ)
	తిరిగి FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    ఒకవేళ (exclude_trailing_space)
    {
	పూర్ణాంకం s = bd->textlen + bd->endspaces;

	అయితే (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_తల_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].నిడివి = (size_t)(pnew - y_current->y_array[y_idx].string);

    తిరిగి OK;
}

#నిర్వచితమైతే HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    స్థిర శూన్య
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    దీర్ఘ	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    పరిమాణం(string_T) * y_current->y_కొలత, TRUE);
    ఒకవేళ (y_current->y_array == ఖాళీ)
	y_current->y_కొలత = 0;
    లేకపోతే
	కొరకు (j = 0; j < y_current->y_కొలత; ++j)
	{
	    ఒకవేళ ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].నిడివి)) == ఖాళీ)
	    {
		free_yank(j);
		y_current->y_కొలత = 0;
		ఆపు;
	    }
	    y_current->y_array[j].నిడివి = curr->y_array[j].నిడివి;
	}
    y_current = curr;
}
#అయితే_ముగింపు

/*
 * Put contents of register "regname" into the text.
 * Caller must check "regname" to be valid!
 * "flags": PUT_FIXINDENT	make indent look nice
 *	    PUT_CURSEND		leave cursor after end of new text
 *	    PUT_LINE		force linewise put (":put")
 *	    PUT_BLOCK_INNER     in block mode, do not add trailing spaces
 */
    శూన్య
do_put(
    పూర్ణాంకం		regname,
    char_u	*సమాసం_ఫలితం,	// result for regname "=" when compiled
    పూర్ణాంకం		dir,		// BACKWARD for 'P', FORWARD for 'p'
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    పూర్ణాంకం		yanklen;
    పూర్ణాంకం		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	నిలువు;
    దీర్ఘ	i;			// index in y_array[]
    పూర్ణాంకం		y_type;
    దీర్ఘ	y_కొలత;
    పూర్ణాంకం		oldlen;
    దీర్ఘ	y_వెడల్పు = 0;
    colnr_T	vcol;
    string_T	*y_array = ఖాళీ;
    yankreg_T	*y_current_వాడిన = ఖాళీ;
    దీర్ఘ	nr_పంక్తులు = 0;
    string_T	చొప్పించు_string;
    పూర్ణాంకం		allocated = FALSE;
    pos_T	orig_ప్రారంభం = curbuf->b_op_ప్రారంభం;
    pos_T	orig_చివర = curbuf->b_op_చివర;
    గుర్తులేని పూర్ణాంకం cur_ve_flags = get_ve_flags();

#నిర్వచితమైతే HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(శూన్య)may_get_selection(regname);
#అయితే_ముగింపు

    // Remove any preinserted text (issue #19329)
    ఒకవేళ (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_ప్రారంభం = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_చివర = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    ఒకవేళ (regname == '.')
    {
	ఒకవేళ (VIsual_చురుకైన)
	    stuffcharReadbuff(VIsual_బహుళకం);

#నిర్వచితమైతే FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	ఒకవేళ (has_textput_events)
	    కూడు_చివరి_చొప్పించు++;
#అయితే_ముగింపు

	(శూన్య)stuff_inserted((dir == FORWARD ? (లెక్క == -1 ? 'o' : 'a') :
				    (లెక్క == -1 ? 'O' : 'i')), లెక్క, FALSE);

#నిర్వచితమైతే FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	ఒకవేళ (has_textputpre())
	    put_do_autocmd('.', ఖాళీ, ఖాళీ, false, dir);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (has_textputpost())
	    put_do_autocmd('.', ఖాళీ, ఖాళీ, true, dir);

	ఒకవేళ (has_textput_events && --కూడు_చివరి_చొప్పించు == 0)
	    ga_clear(&చివరి_చొప్పించు_ga);
#అయితే_ముగింపు

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	ఒకవేళ ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	తిరిగి;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    చొప్పించు_string.string = ఖాళీ;
    ఒకవేళ (regname == '=' && సమాసం_ఫలితం != ఖాళీ)
	చొప్పించు_string.string = సమాసం_ఫలితం;
    లేకపోతే ఒకవేళ (get_spec_reg(regname, &చొప్పించు_string.string, &allocated, TRUE)
		&& చొప్పించు_string.string == ఖాళీ)
    {
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#అయితే_ముగింపు
	తిరిగి;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    ఒకవేళ (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	వెళ్ళు చివర;

    ఒకవేళ (చొప్పించు_string.string != ఖాళీ)
    {
	చొప్పించు_string.నిడివి = STRLEN(చొప్పించు_string.string);

	y_type = MCHAR;
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    కొరకు (;;)
	    {
		y_కొలత = 0;
		ptr = చొప్పించు_string.string;
		ptrlen = చొప్పించు_string.నిడివి;
		అయితే (ptr != ఖాళీ)
		{
		    ఒకవేళ (y_array != ఖాళీ)
			y_array[y_కొలత].string = ptr;
		    ++y_కొలత;
		    tmp = vim_strchr(ptr, '\n');
		    ఒకవేళ (tmp == ఖాళీ)
		    {
			ఒకవేళ (y_array != ఖాళీ)
			    y_array[y_కొలత - 1].నిడివి = ptrlen;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (y_array != ఖాళీ)
			{
			    *tmp = NUL;
			    y_array[y_కొలత - 1].నిడివి = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_కొలత - 1].నిడివి + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			ఒకవేళ (*tmp == NUL)
			{
			    y_type = MLINE;
			    ఆపు;
			}
		    }
		    ptr = tmp;
		}
		ఒకవేళ (y_array != ఖాళీ)
		    ఆపు;
		y_array = ALLOC_MULT(string_T, y_కొలత);
		ఒకవేళ (y_array == ఖాళీ)
		    వెళ్ళు చివర;
	    }
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    y_కొలత = 1;		// use fake one-line yank register
	    y_array = &చొప్పించు_string;
	}
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (has_textputpre())
	    put_do_autocmd(regname, ఖాళీ, &చొప్పించు_string, false, dir);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_రిజిస్టరు(regname, FALSE);
	    put_do_autocmd(regname, y_current, ఖాళీ, false, dir);
	}
#అయితే_ముగింపు
	get_yank_రిజిస్టరు(regname, FALSE);

	y_type = y_current->y_type;
	y_వెడల్పు = y_current->y_వెడల్పు;
	y_కొలత = y_current->y_కొలత;
	y_array = y_current->y_array;
	y_current_వాడిన = y_current;
    }

    ఒకవేళ (y_type == MLINE)
    {
	ఒకవేళ (flags & PUT_పంక్తి_విభజించు)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    ఒకవేళ (u_save_cursor() == FAIL)
		వెళ్ళు చివర;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    ఒకవేళ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    ఒకవేళ (ptr == ఖాళీ)
		వెళ్ళు చివర;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.నిలువు;
	    ఒకవేళ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    ఒకవేళ (ptr == ఖాళీ)
		వెళ్ళు చివర;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_పంక్తులు;
	    dir = FORWARD;
	}
	ఒకవేళ (flags & PUT_పంక్తి_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_చివర;
	    dir = FORWARD;
	}
	curbuf->b_op_ప్రారంభం = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_చివర = curwin->w_cursor;	// default for '] mark
    }

    ఒకవేళ (flags & PUT_పంక్తి)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    ఒకవేళ (y_కొలత == 0 || y_array == ఖాళీ)
    {
	semsg(_(e_nothing_లోపల_రిజిస్టరు_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	వెళ్ళు చివర;
    }

    ఒకవేళ (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_కొలత + 1;
	ఒకవేళ (lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    lnum = curbuf->b_ml.ml_పంక్తి_లెక్క + 1;
	ఒకవేళ (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    వెళ్ళు చివర;
    }
    లేకపోతే ఒకవేళ (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#నిర్వచితమైతే FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	ఒకవేళ (dir == BACKWARD)
	    (శూన్య)hasFolding(lnum, &lnum, ఖాళీ);
	లేకపోతే
	    (శూన్య)hasFolding(lnum, ఖాళీ, &lnum);
#అయితే_ముగింపు
	ఒకవేళ (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	ఒకవేళ ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    వెళ్ళు చివర;
#నిర్వచితమైతే FEAT_FOLDING
	ఒకవేళ (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	లేకపోతే
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_ప్రారంభం = curwin->w_cursor;	// for mark_adjust()
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (u_save_cursor() == FAIL)
	వెళ్ళు చివర;

    ఒకవేళ (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	ఒకవేళ (gchar_cursor() == TAB)
	{
	    పూర్ణాంకం viscol = getviscol();
	    పూర్ణాంకం ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    ఒకవేళ (dir == FORWARD ?
#నిర్వచితమైతే FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#లేకపోతే
		    ts - (viscol % ts) != 1
#అయితే_ముగింపు
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    లేకపోతే
		curwin->w_cursor.coladd = 0;
	}
	లేకపోతే ఒకవేళ (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    నిలువు = curwin->w_cursor.నిలువు;

    // Block mode
    ఒకవేళ (y_type == MBLOCK)
    {
	పూర్ణాంకం	delcount;
	పూర్ణాంకం	incr = 0;
	నిర్మాణం ఖండం_def bd;
	దీర్ఘ	j;
	పూర్ణాంకం	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	ఒకవేళ (dir == FORWARD && c != NUL)
	{
	    ఒకవేళ (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &నిలువు, ఖాళీ, &endcol2, 0);
	    లేకపోతే
		getvcol(curwin, &curwin->w_cursor, ఖాళీ, ఖాళీ, &నిలువు, 0);

	    ఒకవేళ (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.నిలువు += (*mb_ptr2len)(ml_get_cursor());
	    లేకపోతే
		ఒకవేళ (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.నిలువు;
	    ++నిలువు;
	}
	లేకపోతే
	    getvcol(curwin, &curwin->w_cursor, &నిలువు, ఖాళీ, &endcol2, 0);

	నిలువు += curwin->w_cursor.coladd;
	ఒకవేళ (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.నిలువు))
	{
	    ఒకవేళ (dir == FORWARD && c == NUL)
		++నిలువు;
	    ఒకవేళ (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.నిలువు;
	    ఒకవేళ (c == TAB)
	    {
		ఒకవేళ (dir == BACKWARD && curwin->w_cursor.నిలువు)
		    curwin->w_cursor.నిలువు--;
		ఒకవేళ (dir == FORWARD && నిలువు - 1 == endcol2)
		    curwin->w_cursor.నిలువు++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	కొరకు (i = 0; i < y_కొలత; ++i)
	{
	    పూర్ణాంకం		    spaces = 0;
	    అక్షరం	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    ఒకవేళ (curwin->w_cursor.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    {
		ఒకవేళ (ml_append(curbuf->b_ml.ml_పంక్తి_లెక్క, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    ఆపు;
		++nr_పంక్తులు;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    అయితే (cts.cts_vcol < నిలువు && *cts.cts_ptr != NUL)
	    {
		// Count a tab for what it's worth (if list mode not on)
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    vcol = cts.cts_vcol;
	    ptr = cts.cts_ptr;
	    bd.textcol = (colnr_T)(ptr - oldp);
	    clear_chartabsize_arg(&cts);

	    shortline = (vcol < నిలువు) || (vcol == నిలువు && !*ptr) ;

	    ఒకవేళ (vcol < నిలువు) // line too short, pad with spaces
		bd.startspaces = నిలువు - vcol;
	    లేకపోతే ఒకవేళ (vcol > నిలువు)
	    {
		bd.endspaces = vcol - నిలువు;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		ఒకవేళ (has_mbyte)
		    bd.textcol -= (*mb_తల_off)(oldp, oldp + bd.textcol);
		ఒకవేళ (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (పూర్ణాంకం)y_array[i].నిడివి;

	    ఒకవేళ ((flags & PUT_ఖండం_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		//ఖండం
		spaces = y_వెడల్పు + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		అయితే (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		ఒకవేళ (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    ఒకవేళ (yanklen + spaces != 0
		     && లెక్క > ((గరిష్ఠ_పూర్ణాంకం - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_పాఠ్యం_too_long));
		ఆపు;
	    }

	    totlen = లెక్క * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    ఒకవేళ (newp == ఖాళీ)
		ఆపు;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    కొరకు (j = 0; j < లెక్క; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		ఒకవేళ ((j < లెక్క - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		లేకపోతే
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
	    ఒకవేళ (i == 0)
		curwin->w_cursor.నిలువు += bd.startspaces;
	}

	మారింది_పంక్తులు(lnum, 0, curwin->w_cursor.lnum - nr_పంక్తులు, nr_పంక్తులు);

	// Set '[ mark.
	curbuf->b_op_ప్రారంభం = curwin->w_cursor;
	curbuf->b_op_ప్రారంభం.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_చివర.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_చివర.నిలువు = bd.textcol + totlen - 1;
	ఒకవేళ (curbuf->b_op_చివర.నిలువు < 0)
	    curbuf->b_op_చివర.నిలువు = 0;
	curbuf->b_op_చివర.coladd = 0;
	ఒకవేళ (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_చివర;
	    curwin->w_cursor.నిలువు++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    ఒకవేళ (curwin->w_cursor.నిలువు > len)
		curwin->w_cursor.నిలువు = len;
	}
	లేకపోతే
	    curwin->w_cursor.lnum = lnum;
    }
    లేకపోతే
    {
	pos_T	new_cursor;

	yanklen = (పూర్ణాంకం)y_array[0].నిడివి;

	// Character or Line mode
	ఒకవేళ (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    ఒకవేళ (dir == FORWARD && gchar_cursor() != NUL)
	    {
		ఒకవేళ (has_mbyte)
		{
		    పూర్ణాంకం bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    నిలువు += bytelen;
		    ఒకవేళ (yanklen)
		    {
			curwin->w_cursor.నిలువు += bytelen;
			curbuf->b_op_చివర.నిలువు += bytelen;
		    }
		}
		లేకపోతే
		{
		    ++నిలువు;
		    ఒకవేళ (yanklen)
		    {
			++curwin->w_cursor.నిలువు;
			++curbuf->b_op_చివర.నిలువు;
		    }
		}
	    }
	    curbuf->b_op_ప్రారంభం = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	లేకపోతే ఒకవేళ (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	ఒకవేళ (y_type == MCHAR && y_కొలత == 1)
	{
	    linenr_T	చివర_lnum = 0; // init for gcc
	    linenr_T	ప్రారంభం_lnum = lnum;
	    పూర్ణాంకం		మొదటి_byte_off = 0;

	    ఒకవేళ (VIsual_చురుకైన)
	    {
		చివర_lnum = curbuf->b_visual.vi_చివర.lnum;
		ఒకవేళ (చివర_lnum < curbuf->b_visual.vi_ప్రారంభం.lnum)
		    చివర_lnum = curbuf->b_visual.vi_ప్రారంభం.lnum;
		ఒకవేళ (చివర_lnum > ప్రారంభం_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.నిలువు = నిలువు;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, ఖాళీ, &vcol, ఖాళీ, 0);
		}
	    }

	    ఒకవేళ (లెక్క == 0 || yanklen == 0)
	    {
		ఒకవేళ (VIsual_చురుకైన)
		    lnum = చివర_lnum;
	    }
	    లేకపోతే ఒకవేళ (లెక్క > గరిష్ఠ_పూర్ణాంకం / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_పాఠ్యం_too_long));
	    లేకపోతే
	    {
		totlen = లెక్క * yanklen;
		చేయి
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    ఒకవేళ (lnum > ప్రారంభం_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			ఒకవేళ (getvpos(&pos, vcol) == OK)
			    నిలువు = pos.నిలువు;
			లేకపోతే
			    నిలువు = MAXCOL;
		    }
		    ఒకవేళ (VIsual_చురుకైన && నిలువు > oldlen)
		    {
			lnum++;
			కొనసాగు;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    ఒకవేళ (newp == ఖాళీ)
			వెళ్ళు చివర;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)నిలువు);
		    ptr = newp + నిలువు;
		    కొరకు (i = 0; i < లెక్క; ++i)
		    {
			mch_memmove(ptr, y_array[0].string, (size_t)yanklen);
			ptr += yanklen;
		    }
		    mch_memmove(ptr, oldp + నిలువు, (size_t)(oldlen - నిలువు) + 1);	    // +1 for NUL

		    // compute the byte offset for the last character
		    మొదటి_byte_off = mb_తల_off(newp, ptr - 1);

		    // Note: this may free "newp"
		    ml_replace(lnum, newp, FALSE);

		    inserted_బైట్లు(lnum, నిలువు, totlen);

		    // Place cursor on last putted char.
		    ఒకవేళ (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			మారింది_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.నిలువు += (colnr_T)(totlen - 1);
		    }
		    ఒకవేళ (VIsual_చురుకైన)
			lnum++;
		} అయితే (VIsual_చురుకైన && lnum <= చివర_lnum);

		ఒకవేళ (VIsual_చురుకైన) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_చివర = curwin->w_cursor;
	    curbuf->b_op_చివర.నిలువు -= మొదటి_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    ఒకవేళ (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.నిలువు;
	    లేకపోతే
		curwin->w_cursor.నిలువు -= మొదటి_byte_off;
	}
	లేకపోతే
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    పూర్ణాంకం		indent;
	    పూర్ణాంకం		orig_indent = 0;
	    పూర్ణాంకం		indent_diff = 0;	// init for gcc
	    పూర్ణాంకం		మొదటి_indent = TRUE;
	    పూర్ణాంకం		lendiff = 0;
	    దీర్ఘ	cnt;

	    ఒకవేళ (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    కొరకు (cnt = 1; cnt <= లెక్క; ++cnt)
	    {
		i = 0;
		ఒకవేళ (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + నిలువు;
		    totlen = (పూర్ణాంకం)y_array[y_కొలత - 1].నిడివి;
		    newp = alloc(ml_get_len(lnum) - నిలువు + totlen + 1);
		    ఒకవేళ (newp == ఖాళీ)
			వెళ్ళు error;
		    STRCPY(newp, y_array[y_కొలత - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(నిలువు + yanklen + 1);
		    ఒకవేళ (newp == ఖాళీ)
			వెళ్ళు error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)నిలువు);
					    // append to first line
		    mch_memmove(newp + నిలువు, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		కొరకు (; i < y_కొలత; ++i)
		{
		    ఒకవేళ (y_type != MCHAR || i < y_కొలత - 1)
		    {
			ఒకవేళ (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    వెళ్ళు error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_పంక్తులు;
		    ఒకవేళ (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			ఒకవేళ (cnt == లెక్క && i == y_కొలత - 1)
			    lendiff = ml_get_len(lnum);
			ఒకవేళ (*ptr == '#' && preprocs_ఎడమ())
			    indent = 0;     // Leave # lines at start
			లేకపోతే ఒకవేళ (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			లేకపోతే ఒకవేళ (మొదటి_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    మొదటి_indent = FALSE;
			}
			లేకపోతే ఒకవేళ ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(శూన్య)సమితి_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			ఒకవేళ (cnt == లెక్క && i == y_కొలత - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		ఒకవేళ (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    ఒకవేళ (y_type == MLINE)
	    {
		curbuf->b_op_ప్రారంభం.నిలువు = 0;
		ఒకవేళ (dir == FORWARD)
		    curbuf->b_op_ప్రారంభం.lnum++;
	    }
	    mark_adjust(curbuf->b_op_ప్రారంభం.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_పంక్తులు, 0L);

	    // note changed text for displaying and folding
	    ఒకవేళ (y_type == MCHAR)
		మారింది_పంక్తులు(curwin->w_cursor.lnum, నిలువు,
					 curwin->w_cursor.lnum + 1, nr_పంక్తులు);
	    లేకపోతే
		మారింది_పంక్తులు(curbuf->b_op_ప్రారంభం.lnum, 0,
					   curbuf->b_op_ప్రారంభం.lnum, nr_పంక్తులు);
	    ఒకవేళ (y_current_వాడిన != ఖాళీ && (y_current_వాడిన != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_రిజిస్టరు_మారింది_while_using_it));
		వెళ్ళు చివర;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_చివర.lnum = new_lnum;
	    నిలువు = MAX(0, (colnr_T)y_array[y_కొలత - 1].నిడివి - lendiff);
	    ఒకవేళ (నిలువు > 1)
	    {
		curbuf->b_op_చివర.నిలువు = నిలువు - 1;
		ఒకవేళ (y_array[y_కొలత - 1].నిడివి > 0)
		    curbuf->b_op_చివర.నిలువు -= mb_తల_off(y_array[y_కొలత - 1].string,
						y_array[y_కొలత - 1].string + y_array[y_కొలత - 1].నిడివి - 1);
	    }
	    లేకపోతే
		curbuf->b_op_చివర.నిలువు = 0;

	    ఒకవేళ (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    లేకపోతే ఒకవేళ (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		ఒకవేళ (y_type == MLINE)
		{
		    ఒకవేళ (lnum >= curbuf->b_ml.ml_పంక్తి_లెక్క)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
		    లేకపోతే
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.నిలువు = 0;
		}
		లేకపోతే
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.నిలువు = నిలువు;
		    curbuf->b_op_చివర = curwin->w_cursor;
		    ఒకవేళ (నిలువు > 1)
			curbuf->b_op_చివర.నిలువు = నిలువు - 1;
		}
	    }
	    లేకపోతే ఒకవేళ (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.నిలువు = 0;
		ఒకవేళ (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    లేకపోతే	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_పంక్తులు);
    curwin->w_సమితి_curswant = true;

    // Make sure the cursor is not after the NUL.
    పూర్ణాంకం len = ml_get_curline_len();
    ఒకవేళ (curwin->w_cursor.నిలువు > len)
    {
	ఒకవేళ (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.నిలువు - len;
	curwin->w_cursor.నిలువు = len;
    }

చివర:
    ఒకవేళ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_ప్రారంభం = orig_ప్రారంభం;
	curbuf->b_op_చివర = orig_చివర;
    }

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (has_textputpost())
    {
	ఒకవేళ (చొప్పించు_string.string == ఖాళీ)
	    put_do_autocmd(regname, y_current, ఖాళీ, true, dir);
	లేకపోతే
	    put_do_autocmd(regname, ఖాళీ, &చొప్పించు_string, true, dir);
    }
#అయితే_ముగింపు

    ఒకవేళ (allocated)
	vim_free(చొప్పించు_string.string);
    ఒకవేళ (regname == '=')
	vim_free(y_array);

    VIsual_చురుకైన = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    పూర్ణాంకం
get_రిజిస్టరు_name(పూర్ణాంకం అంకె)
{
    ఒకవేళ (అంకె == -1)
	తిరిగి '"';
    లేకపోతే ఒకవేళ (అంకె < 10)
	తిరిగి అంకె + '0';
    లేకపోతే ఒకవేళ (అంకె == DELETION_రిజిస్టరు)
	తిరిగి '-';
#ఒకవేళ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    లేకపోతే ఒకవేళ (అంకె == నక్షత్రం_రిజిస్టరు)
	తిరిగి '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    లేకపోతే ఒకవేళ (clipmethod == CLIPMETHOD_PROVIDER && అంకె == REAL_PLUS_రిజిస్టరు)
	తిరిగి '+';
    లేకపోతే ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER && అంకె == PLUS_రిజిస్టరు)
	తిరిగి '+';
#అయితే_ముగింపు
    లేకపోతే
	తిరిగి అంకె + 'a' - 10;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    పూర్ణాంకం
get_unname_రిజిస్టరు(శూన్య)
{
    తిరిగి y_మునుపటి == ఖాళీ ? -1 : y_మునుపటి - &y_regs[0];
}
#అయితే_ముగింపు

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    శూన్య
ex_display(exarg_T *eap)
{
    పూర్ణాంకం		i, n;
    దీర్ఘ	j;
    char_u	*p;
    yankreg_T	*yb;
    పూర్ణాంకం		name;
    పూర్ణాంకం		attr;
    char_u	*arg = eap->arg;
    పూర్ణాంకం		clen;
    పూర్ణాంకం		type;
    string_T	చొప్పించు;

    silence_w23_w24_సందేశం++;
    ఒకవేళ (arg != ఖాళీ && *arg == NUL)
	arg = ఖాళీ;
    attr = HL_ATTR(HLF_8);

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#అయితే_ముగింపు

    // Highlight title
    సందేశం_puts_title(_("\nType Name Content"));
    కొరకు (i = -1; i < అంకె_REGISTERS && !got_int; ++i)
    {
	name = get_రిజిస్టరు_name(i);
	ఎంపిక (get_reg_type(name, ఖాళీ))
	{
	    స్థితి MLINE: type = 'l'; ఆపు;
	    స్థితి MCHAR: type = 'c'; ఆపు;
	    అప్రమేయ:	type = 'b'; ఆపు;
	}
	ఒకవేళ (arg != ఖాళీ && vim_strchr(arg, name) == ఖాళీ
#నిర్వచితమైతే ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == ఖాళీ)
#అయితే_ముగింపు
		)
	    కొనసాగు;	    // did not ask for this register

#నిర్వచితమైతే HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
	ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (శూన్య)may_get_selection(name);
#అయితే_ముగింపు

	ఒకవేళ (i == -1)
	{
	    ఒకవేళ (y_మునుపటి != ఖాళీ)
		yb = y_మునుపటి;
	    లేకపోతే
		yb = &(y_regs[0]);
	}
	లేకపోతే
	    yb = &(y_regs[i]);

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != ఖాళీ &&
		    (yb == y_మునుపటి || yb == &y_regs[0])))
	    కొనసాగు;	    // do not list register being written to, the
			    // pointer can be freed
#అయితే_ముగింపు

	ఒకవేళ (yb->y_array != ఖాళీ)
	{
	    పూర్ణాంకం do_show = FALSE;

	    కొరకు (j = 0; !do_show && j < yb->y_కొలత; ++j)
		do_show = !సందేశం_filtered(yb->y_array[j].string);

	    ఒకవేళ (do_show || yb->y_కొలత == 0)
	    {
		సందేశం_putchar('\n');
		సందేశం_puts("  ");
		సందేశం_putchar(type);
		సందేశం_puts("  ");
		సందేశం_putchar('"');
		సందేశం_putchar(name);
		సందేశం_puts("   ");

		n = (పూర్ణాంకం)Columns - 11;
		కొరకు (j = 0; j < yb->y_కొలత && n > 1; ++j)
		{
		    ఒకవేళ (j)
		    {
			సందేశం_puts_attr("^J", attr);
			n -= 2;
		    }
		    కొరకు (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			సందేశం_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		ఒకవేళ (n > 1 && yb->y_type == MLINE)
		    సందేశం_puts_attr("^J", attr);
		బహిర్గతం_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    చొప్పించు = get_చివరి_చొప్పించు();
    ఒకవేళ ((p = చొప్పించు.string) != ఖాళీ
		  && (arg == ఖాళీ || vim_strchr(arg, '.') != ఖాళీ) && !got_int
						      && !సందేశం_filtered(p))
    {
	సందేశం_puts("\n  c  \".   ");
	dis_సందేశం(p, TRUE);
    }

    // display last command line
    ఒకవేళ (చివరి_cmdline != ఖాళీ && (arg == ఖాళీ || vim_strchr(arg, ':') != ఖాళీ)
			       && !got_int && !సందేశం_filtered(చివరి_cmdline))
    {
	సందేశం_puts("\n  c  \":   ");
	dis_సందేశం(చివరి_cmdline, FALSE);
    }

    // display current file name
    ఒకవేళ (curbuf->b_fname != ఖాళీ
	    && (arg == ఖాళీ || vim_strchr(arg, '%') != ఖాళీ) && !got_int
					&& !సందేశం_filtered(curbuf->b_fname))
    {
	సందేశం_puts("\n  c  \"%   ");
	dis_సందేశం(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    ఒకవేళ ((arg == ఖాళీ || vim_strchr(arg, '#') != ఖాళీ) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	ఒకవేళ (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !సందేశం_filtered(fname))
	{
	    సందేశం_puts("\n  c  \"#   ");
	    dis_సందేశం(fname, FALSE);
	}
    }

    // display last search pattern
    ఒకవేళ (చివరి_శోధన_pat() != ఖాళీ
		 && (arg == ఖాళీ || vim_strchr(arg, '/') != ఖాళీ) && !got_int
				      && !సందేశం_filtered(చివరి_శోధన_pat()))
    {
	సందేశం_puts("\n  c  \"/   ");
	dis_సందేశం(చివరి_శోధన_pat(), FALSE);
    }

#నిర్వచితమైతే FEAT_EVAL
    // display last used expression
    ఒకవేళ (సమాసం_పంక్తి != ఖాళీ && (arg == ఖాళీ || vim_strchr(arg, '=') != ఖాళీ)
				  && !got_int && !సందేశం_filtered(సమాసం_పంక్తి))
    {
	సందేశం_puts("\n  c  \"=   ");
	dis_సందేశం(సమాసం_పంక్తి, FALSE);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#అయితే_ముగింపు
    silence_w23_w24_సందేశం--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    స్థిర శూన్య
dis_సందేశం(
    char_u	*p,
    పూర్ణాంకం		skip_esc)	    // if TRUE, ignore trailing ESC
{
    పూర్ణాంకం		n;
    పూర్ణాంకం		l;

    n = (పూర్ణాంకం)Columns - 6;
    అయితే (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    సందేశం_outtrans_len(p, l);
	    p += l;
	}
	లేకపోతే
	    సందేశం_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#ఒకవేళ defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    శూన్య
dnd_yank_drag_దత్తాంశం(char_u *str, దీర్ఘ len)
{
    yankreg_T *curr;

    curr = y_current;
    y_current = &y_regs[TILDE_రిజిస్టరు];
    free_yank_all();
    str_గమ్యం_reg(y_current, MCHAR, str, len, 0L, FALSE);
    y_current = curr;
}
#అయితే_ముగింపు


/*
 * Return the type of a register.
 * Used for getregtype()
 * Returns MAUTO for error.
 */
    char_u
get_reg_type(పూర్ణాంకం regname, దీర్ఘ *reglen)
{
    ఎంపిక (regname)
    {
	స్థితి '%':		// file name
	స్థితి '#':		// alternate file name
	స్థితి '=':		//వ్యక్తీకరణ
	స్థితి ':':		// last command line
	స్థితి '/':		// last search-pattern
	స్థితి '.':		// last inserted text
	స్థితి Ctrl_F:		// Filename under cursor
	స్థితి Ctrl_P:		// Path under cursor, expand via "path"
	స్థితి Ctrl_W:		// word under cursor
	స్థితి Ctrl_A:		// WORD (mnemonic All) under cursor
	స్థితి '_':		// black hole: always empty
	    తిరిగి MCHAR;
    }

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#అయితే_ముగింపు

    ఒకవేళ (regname != NUL && !valid_yank_reg(regname, FALSE))
	తిరిగి MAUTO;

    get_yank_రిజిస్టరు(regname, FALSE);

    ఒకవేళ (y_current->y_array != ఖాళీ)
    {
	ఒకవేళ (reglen != ఖాళీ && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_వెడల్పు;
	తిరిగి y_current->y_type;
    }
    తిరిగి MAUTO;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    స్థిర char_u *
getreg_wrap_one_పంక్తి(char_u *s, పూర్ణాంకం flags)
{
    ఒకవేళ (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	ఒకవేళ (list != ఖాళీ)
	{
	    ఒకవేళ (list_append_string(list, ఖాళీ, -1) == FAIL)
	    {
		list_free(list);
		తిరిగి ఖాళీ;
	    }
	    list->lv_మొదటి->li_tv.vval.v_string = s;
	}
	తిరిగి (char_u *)list;
    }
    తిరిగి s;
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
get_reg_contents(పూర్ణాంకం regname, పూర్ణాంకం flags)
{
    linenr_T	i;
    char_u	*retval;
    పూర్ణాంకం		allocated;
    దీర్ఘ	len;

    // Don't allow using an expression register inside an expression
    ఒకవేళ (regname == '=')
    {
	ఒకవేళ (flags & GREG_NO_సమాసం)
	    తిరిగి ఖాళీ;
	ఒకవేళ (flags & GREG_సమాసం_SRC)
	    తిరిగి getreg_wrap_one_పంక్తి(get_సమాసం_పంక్తి_src(), flags);
	తిరిగి getreg_wrap_one_పంక్తి(get_సమాసం_పంక్తి(), flags);
    }

    ఒకవేళ (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    ఒకవేళ (regname != NUL && !valid_yank_reg(regname, FALSE))
	తిరిగి ఖాళీ;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ఒకవేళ (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	ఒకవేళ (retval == ఖాళీ)
	    తిరిగి ఖాళీ;
	ఒకవేళ (allocated)
	    తిరిగి getreg_wrap_one_పంక్తి(retval, flags);
	తిరిగి getreg_wrap_one_పంక్తి(vim_strsave(retval), flags);
    }

    get_yank_రిజిస్టరు(regname, FALSE);
    ఒకవేళ (y_current->y_array == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	ఒకవేళ (list == ఖాళీ)
	    తిరిగి ఖాళీ;
	కొరకు (i = 0; i < y_current->y_కొలత; ++i)
	{
	    ఒకవేళ (list_append_string(list, y_current->y_array[i].string,
		(పూర్ణాంకం)y_current->y_array[i].నిడివి) == FAIL)
	    {
		list_free(list);
		తిరిగి ఖాళీ;
	    }
	}
	తిరిగి (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    కొరకు (i = 0; i < y_current->y_కొలత; ++i)
    {
	len += (దీర్ఘ)y_current->y_array[i].నిడివి;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ఒకవేళ (y_current->y_type == MLINE || i < y_current->y_కొలత - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    ఒకవేళ (retval == ఖాళీ)
	తిరిగి ఖాళీ;

    // Copy the lines of the yank register into the string.
    len = 0;
    కొరకు (i = 0; i < y_current->y_కొలత; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (దీర్ఘ)y_current->y_array[i].నిడివి;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ఒకవేళ (y_current->y_type == MLINE || i < y_current->y_కొలత - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    తిరిగి retval;
}

    స్థిర పూర్ణాంకం
init_write_reg(
    పూర్ణాంకం		name,
    yankreg_T	**old_y_మునుపటి,
    yankreg_T	**old_y_current,
    పూర్ణాంకం		must_append,
    పూర్ణాంకం		*yank_type వాడని)
{
    ఒకవేళ (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	తిరిగి FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_మునుపటి = y_మునుపటి;
    *old_y_current = y_current;

    get_yank_రిజిస్టరు(name, TRUE);
    ఒకవేళ (!y_append && !must_append)
	free_yank_all();
    తిరిగి OK;
}

    స్థిర శూన్య
ముగింపు_write_reg(
    పూర్ణాంకం		name,
    yankreg_T	*old_y_మునుపటి,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_సమితి_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    ఒకవేళ (name != '"')
	y_మునుపటి = old_y_మునుపటి;
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
    శూన్య
write_reg_contents(
    పూర్ణాంకం		name,
    char_u	*str,
    పూర్ణాంకం		maxlen,
    పూర్ణాంకం		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    శూన్య
write_reg_contents_lst(
    పూర్ణాంకం		name,
    char_u	**అక్షరశ్రేణులు,
    పూర్ణాంకం		maxlen వాడని,
    పూర్ణాంకం		must_append,
    పూర్ణాంకం		yank_type,
    దీర్ఘ	ఖండం_len)
{
    yankreg_T  *old_y_మునుపటి, *old_y_current;

    ఒకవేళ (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	ఒకవేళ (అక్షరశ్రేణులు[0] == ఖాళీ)
	    s = (char_u *)"";
	లేకపోతే ఒకవేళ (అక్షరశ్రేణులు[1] != ఖాళీ)
	{
	    semsg(_(e_రిజిస్టరు_char_cannot_contain_గుణిజం_పంక్తులు), name);
	    తిరిగి;
	}
	లేకపోతే
	    s = అక్షరశ్రేణులు[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, ఖండం_len);
	తిరిగి;
    }

    ఒకవేళ (name == '_')	    // black hole: nothing to do
	తిరిగి;

    ఒకవేళ (init_write_reg(name, &old_y_మునుపటి, &old_y_current, must_append,
		&yank_type) == FAIL)
	తిరిగి;

    str_గమ్యం_reg(y_current, yank_type, (char_u *)అక్షరశ్రేణులు, -1, ఖండం_len, TRUE);

    ముగింపు_write_reg(name, old_y_మునుపటి, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_సమితి(name);
# endif
}

    శూన్య
write_reg_contents_ex(
    పూర్ణాంకం		name,
    char_u	*str,
    పూర్ణాంకం		maxlen,
    పూర్ణాంకం		must_append,
    పూర్ణాంకం		yank_type,
    దీర్ఘ	ఖండం_len)
{
    yankreg_T	*old_y_మునుపటి, *old_y_current;
    దీర్ఘ	len;

    ఒకవేళ (maxlen >= 0)
	len = maxlen;
    లేకపోతే
	len = (దీర్ఘ)STRLEN(str);

    // Special case: '/' search pattern
    ఒకవేళ (name == '/')
    {
	సమితి_చివరి_శోధన_pat(str, RE_శోధన, TRUE, TRUE);
	తిరిగి;
    }

    ఒకవేళ (name == '#')
    {
	ఒకవేళ (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  తిరిగి;
	}

	buf_T	*buf;

	ఒకవేళ (VIM_ISDIGIT(*str))
	{
	    పూర్ణాంకం	అంకె = atoi((అక్షరం *)str);

	    buf = buflist_findnr(అంకె);
	    ఒకవేళ (buf == ఖాళీ)
		semsg(_(e_బఫరు_nr_does_not_exist), (దీర్ఘ)అంకె);
	}
	లేకపోతే
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	ఒకవేళ (buf == ఖాళీ)
	    తిరిగి;
	curwin->w_alt_fnum = buf->b_fnum;
	తిరిగి;
    }

    ఒకవేళ (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి;
	ఒకవేళ (must_append && సమాసం_పంక్తి != ఖాళీ)
	{
	    s = concat_str(సమాసం_పంక్తి, p);
	    vim_free(p);
	    p = s;
	}
	సమితి_సమాసం_పంక్తి(p, ఖాళీ);
	తిరిగి;
    }

    ఒకవేళ (name == '_')	    // black hole: nothing to do
	తిరిగి;

    ఒకవేళ (init_write_reg(name, &old_y_మునుపటి, &old_y_current, must_append,
		&yank_type) == FAIL)
	తిరిగి;

    str_గమ్యం_reg(y_current, yank_type, str, len, ఖండం_len, FALSE);

    ముగింపు_write_reg(name, old_y_మునుపటి, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_సమితి(name);
# endif
}
#అయితే_ముగింపు	// FEAT_EVAL

#ఒకవేళ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    శూన్య
str_గమ్యం_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    పూర్ణాంకం		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    దీర్ఘ	len,		// length of string
    దీర్ఘ	blocklen,	// width of Visual block
    పూర్ణాంకం		str_list)	// TRUE if str is char_u **
{
    పూర్ణాంకం		type;			// MCHAR, MLINE or MBLOCK
    పూర్ణాంకం		lnum;
    దీర్ఘ	ప్రారంభం;
    దీర్ఘ	i;
    పూర్ణాంకం		extra;
    పూర్ణాంకం		newlines;		// number of lines added
    పూర్ణాంకం		extraline = 0;		// extra line at the end
    పూర్ణాంకం		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    దీర్ఘ	maxlen;

    ఒకవేళ (y_ptr->y_array == ఖాళీ)		// NULL means empty register
	y_ptr->y_కొలత = 0;

    ఒకవేళ (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    లేకపోతే
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    ఒకవేళ (str_list)
    {
	కొరకు (ss = (char_u **) str; *ss != ఖాళీ; ++ss)
	    ++newlines;
    }
    లేకపోతే
    {
	కొరకు (i = 0; i < len; i++)
	    ఒకవేళ (str[i] == '\n')
		++newlines;
	ఒకవేళ (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	ఒకవేళ (y_ptr->y_కొలత > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    ఒకవేళ (y_ptr->y_కొలత + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	తిరిగి;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_కొలత + newlines) * పరిమాణం(string_T), TRUE);
    ఒకవేళ (pp == ఖాళీ)	// out of memory
	తిరిగి;
    కొరకు (lnum = 0; lnum < y_ptr->y_కొలత; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    ఒకవేళ (str_list)
    {
	కొరకు (ss = (char_u **) str; *ss != ఖాళీ; ++ss, ++lnum)
	{
	    pp[lnum].నిడివి = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].నిడివి);
	    ఒకవేళ (type == MBLOCK)
	    {
		పూర్ణాంకం charlen = vim_strsize(*ss);

		ఒకవేళ (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    లేకపోతే
    {
	కొరకు (ప్రారంభం = 0; ప్రారంభం < len + extraline; ప్రారంభం += i + 1)
	{
	    పూర్ణాంకం charlen = 0;

	    కొరకు (i = ప్రారంభం; i < len;)	// find the end of the line
	    {
		ఒకవేళ (str[i] == '\n')
		    ఆపు;
		ఒకవేళ (type == MBLOCK)
		{
		    ఒకవేళ (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    లేకపోతే
			charlen += byte2cells(str[i]);
		}

		ఒకవేళ (str[i] == NUL)
		    i++; // registers can have NUL chars
		లేకపోతే
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= ప్రారంభం;			// i is now length of line
	    ఒకవేళ (charlen > maxlen)
		maxlen = charlen;
	    ఒకవేళ (append)
	    {
		--lnum;
		extra = (పూర్ణాంకం)y_ptr->y_array[lnum].నిడివి;
	    }
	    లేకపోతే
		extra = 0;
	    s = alloc(i + extra + 1);
	    ఒకవేళ (s == ఖాళీ)
		ఆపు;
	    ఒకవేళ (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    ఒకవేళ (append)
		vim_free(y_ptr->y_array[lnum].string);
	    ఒకవేళ (i > 0)
		mch_memmove(s + extra, str + ప్రారంభం, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].నిడివి = extra;
	    ++lnum;
	    అయితే (--extra >= 0)
	    {
		ఒకవేళ (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_కొలత = lnum;
    ఒకవేళ (type == MBLOCK)
	y_ptr->y_వెడల్పు = (blocklen < 0 ? maxlen - 1 : blocklen);
    లేకపోతే
	y_ptr->y_వెడల్పు = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_సమయం_సమితి = vim_సమయం();
# endif
}
#అయితే_ముగింపు // FEAT_CLIPBOARD || FEAT_EVAL
