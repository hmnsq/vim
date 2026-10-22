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

#अन्तर्भाव "vim.h"

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
स्थिर yankreg_T	y_regs[अङ्कः_REGISTERS];

स्थिर yankreg_T	*y_current;	    // ptr to current yankreg
स्थिर पूर्णाङ्क		y_append;	    // TRUE when appending
स्थिर yankreg_T	*y_previous = रिक्त; // ptr to last written yankreg

स्थिर पूर्णाङ्क	stuff_yank(पूर्णाङ्क, char_u *);
स्थिर शून्य	put_reedit_in_typebuf(पूर्णाङ्क silent);
स्थिर पूर्णाङ्क	put_in_typebuf(char_u *s, पूर्णाङ्क esc, पूर्णाङ्क colon, पूर्णाङ्क silent);
स्थिर पूर्णाङ्क	yank_copy_पङ्क्तिः(संरचना block_def *bd, long y_idx, पूर्णाङ्क exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
स्थिर शून्य	copy_yank_reg(yankreg_T *reg);
#endif
स्थिर शून्य	dis_सन्देशः(char_u *p, पूर्णाङ्क skip_esc);

#जइ defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(शून्य)
{
    प्रत्यागम y_regs;
}
#endif

#जइ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(पूर्णाङ्क reg)
{
    प्रत्यागम &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(शून्य)
{
    प्रत्यागम y_current;
}

    yankreg_T *
get_y_previous(शून्य)
{
    प्रत्यागम y_previous;
}

    शून्य
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    शून्य
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    शून्य
reset_y_append(शून्य)
{
    y_append = FALSE;
}


#जइ defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
स्थिर char_u	*व्यञ्जकम्_पङ्क्तिः = रिक्त;
स्थिर exarg_T	*व्यञ्जकम्_eap = रिक्त;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    पूर्णाङ्क
get_व्यञ्जकम्_register(शून्य)
{
    char_u	*new_पङ्क्तिः;

    new_पङ्क्तिः = getcmdline('=', 0L, 0, 0);
    जइ (new_पङ्क्तिः == रिक्त)
	प्रत्यागम NUL;
    जइ (*new_पङ्क्तिः == NUL)	// use previous line
	vim_free(new_पङ्क्तिः);
    अण्णहा
	set_व्यञ्जकम्_पङ्क्तिः(new_पङ्क्तिः, रिक्त);
    प्रत्यागम '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    शून्य
set_व्यञ्जकम्_पङ्क्तिः(char_u *new_पङ्क्तिः, exarg_T *eap)
{
    vim_free(व्यञ्जकम्_पङ्क्तिः);
    व्यञ्जकम्_पङ्क्तिः = new_पङ्क्तिः;
    व्यञ्जकम्_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_व्यञ्जकम्_पङ्क्तिः(शून्य)
{
    char_u	*व्यञ्जकम्_copy;
    char_u	*rv;
    स्थिर पूर्णाङ्क	nested = 0;

    जइ (व्यञ्जकम्_पङ्क्तिः == रिक्त)
	प्रत्यागम रिक्त;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    व्यञ्जकम्_copy = vim_strsave(व्यञ्जकम्_पङ्क्तिः);
    जइ (व्यञ्जकम्_copy == रिक्त)
	प्रत्यागम रिक्त;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    जइ (nested >= 10)
	प्रत्यागम व्यञ्जकम्_copy;

    ++nested;
    rv = eval_to_string_eap(व्यञ्जकम्_copy, TRUE, व्यञ्जकम्_eap, FALSE);
    --nested;
    vim_free(व्यञ्जकम्_copy);
    प्रत्यागम rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    स्थिर char_u *
get_व्यञ्जकम्_पङ्क्तिः_src(शून्य)
{
    जइ (व्यञ्जकम्_पङ्क्तिः == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम vim_strsave(व्यञ्जकम्_पङ्क्तिः);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    पूर्णाङ्क
valid_yank_reg(
    पूर्णाङ्क	    regname,
    पूर्णाङ्क	    writing)	    // if TRUE check for writable registers
{
    जइ (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#अण्णहा
				    "/#.%:"
#endif
					, regname) != रिक्त)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#जइ defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#नोचेद्यदि defined(FEAT_CLIPBOARD_PROVIDER)
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
	प्रत्यागम TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    अण्णहा जइ (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	सन्देशः_warn_missing_clipboard();
	प्रत्यागम FALSE;
    }
#endif
    प्रत्यागम FALSE;
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
    पूर्णाङ्क
get_yank_register(पूर्णाङ्क regname, पूर्णाङ्क writing)
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    ret = FALSE;

    y_append = FALSE;
    जइ ((regname == 0 || regname == '"') && !writing && y_previous != रिक्त)
    {
	y_current = y_previous;
	प्रत्यागम ret;
    }
    i = regname;
    जइ (VIM_ISDIGIT(i))
	i -= '0';
    अण्णहा जइ (ASCII_ISLOWER(i))
	i = Char_Ord_नीचः(i) + 10;
    अण्णहा जइ (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    अण्णहा जइ (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    अण्णहा जइ (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    अण्णहा जइ (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	जइ (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	अण्णहा
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#नोचेद्यदि defined(FEAT_CLIPBOARD_PROVIDER)
    अण्णहा जइ (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    अण्णहा जइ (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    अण्णहा जइ (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    अण्णहा		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    जइ (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    प्रत्यागम ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    शून्य *
get_register(
    पूर्णाङ्क		name,
    पूर्णाङ्क		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    पूर्णाङ्क		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	जइ (name == '*' && clip_star.available)
	{
	    जइ (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	जइ (name == '+' && clip_plus.available)
	{
	    जइ (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    जइ (reg == रिक्त)
	प्रत्यागम (शून्य *)रिक्त;

    *reg = *y_current;
    जइ (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	जइ (reg->y_size == 0 || y_current->y_array == रिक्त)
	    reg->y_array = रिक्त;
	अण्णहा
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	जइ (reg->y_array != रिक्त)
	{
	    प्रति (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		जइ (reg->y_array[i].string == रिक्त)
		{
		    // The allocation failed so clean up and exit
		    जाव (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    प्रत्यागम (शून्य *)रिक्त;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    अण्णहा
	y_current->y_array = रिक्त;
    प्रत्यागम (शून्य *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    शून्य
put_register(पूर्णाङ्क name, शून्य *reg)
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

#जइ defined(FEAT_CLIPBOARD)
    शून्य
free_register(शून्य *reg)
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
    पूर्णाङ्क
yank_register_mline(पूर्णाङ्क regname)
{
    जइ (regname != 0 && !valid_yank_reg(regname, FALSE))
	प्रत्यागम FALSE;
    जइ (regname == '_')		// black hole is always empty
	प्रत्यागम FALSE;
    get_yank_register(regname, FALSE);
    प्रत्यागम (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
do_record(पूर्णाङ्क c)
{
    char_u	    *p;
    स्थिर पूर्णाङ्क	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    पूर्णाङ्क		    retval;

    जइ (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	जइ (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	अण्णहा
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    अण्णहा			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	सन्देशः("");
	p = get_recorded();
	जइ (p == रिक्त)
	    retval = FAIL;
	अण्णहा
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
    प्रत्यागम retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    स्थिर पूर्णाङ्क
stuff_yank(पूर्णाङ्क regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    जइ (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	प्रत्यागम FAIL;
    }
    जइ (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	प्रत्यागम OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    जइ (y_append && y_current->y_array != रिक्त)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	जइ (tmp == रिक्त)
	{
	    vim_free(p);
	    प्रत्यागम FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    अण्णहा
    {
	free_yank_all();
	जइ ((y_current->y_array = ALLOC_ONE(string_T)) == रिक्त)
	{
	    vim_free(p);
	    प्रत्यागम FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    प्रत्यागम OK;
}

/*
 * Last executed register (@ command)
 */
स्थिर पूर्णाङ्क execreg_lastc = NUL;

#जइ defined(FEAT_VIMINFO)
    पूर्णाङ्क
get_execreg_lastc(शून्य)
{
    प्रत्यागम execreg_lastc;
}

    शून्य
set_execreg_lastc(पूर्णाङ्क lastc)
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
    स्थिर char_u *
execreg_पङ्क्तिः_continuation(string_T *पङ्क्तयः, long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    पूर्णाङ्क		j;
    char_u	*str;

    ga_init2(&ga, परिमाण(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    जाव (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(पङ्क्तयः[cmd_start].string);
	जइ (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    भङ्ग;
    }

    // join all the lines
    tmp = &पङ्क्तयः[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    प्रति (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &पङ्क्तयः[j];
	p = skipwhite(tmp->string);
	जइ (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    जइ (ga.ga_len > 400)
	    {
		जइ (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		अण्णहा
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
    प्रत्यागम str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
do_execreg(
    पूर्णाङ्क	    regname,
    पूर्णाङ्क	    colon,		// insert ':' before each line
    पूर्णाङ्क	    addcr,		// always add '\n' to end of line
    पूर्णाङ्क	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    पूर्णाङ्क		retval = OK;
    पूर्णाङ्क		remap;

    // repeat previous one
    जइ (regname == '@')
    {
	जइ (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    प्रत्यागम FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    जइ (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	प्रत्यागम FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    जइ (regname == '_')
	प्रत्यागम OK;

    // use last command line
    जइ (regname == ':')
    {
	जइ (last_cmdline == रिक्त)
	{
	    emsg(_(e_no_previous_command_पङ्क्तिः));
	    प्रत्यागम FAIL;
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
	जइ (p != रिक्त)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    जइ (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    अण्णहा
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    अण्णहा जइ (regname == '=')
    {
	p = get_व्यञ्जकम्_पङ्क्तिः();
	जइ (p == रिक्त)
	    प्रत्यागम FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    अण्णहा जइ (regname == '.')		// use last inserted text
    {
	p = get_last_निवेशय_save();
	जइ (p == रिक्त)
	{
	    emsg(_(e_no_inserted_text_yet));
	    प्रत्यागम FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    अण्णहा
    {
	get_yank_register(regname, FALSE);
	जइ (y_current->y_array == रिक्त)
	    प्रत्यागम FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	प्रति (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    पूर्णाङ्क	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    जइ (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		जइ (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    प्रत्यागम FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    जइ (colon && i > 0)
	    {
		p = skipwhite(str);
		जइ (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_पङ्क्तिः_continuation(y_current->y_array, &i);
		    जइ (str == रिक्त)
			प्रत्यागम FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    जइ (free_str)
		vim_free(str);
	    जइ (escaped == रिक्त)
		प्रत्यागम FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    जइ (retval == FAIL)
		प्रत्यागम FAIL;
	    जइ (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		प्रत्यागम FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    प्रत्यागम retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    स्थिर शून्य
put_reedit_in_typebuf(पूर्णाङ्क silent)
{
    जइ (restart_edit == NUL)
	प्रत्यागम;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    जइ (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    अण्णहा जइ (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    अण्णहा जइ (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    जइ (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    स्थिर पूर्णाङ्क
put_in_typebuf(
    char_u	*s,
    पूर्णाङ्क		esc,
    पूर्णाङ्क		colon,	    // add ':' before the line
    पूर्णाङ्क		silent)
{
    पूर्णाङ्क		retval = OK;

    put_reedit_in_typebuf(silent);
    जइ (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    जइ (retval == OK)
    {
	char_u	*p;

	जइ (esc)
	    p = vim_strsave_escape_csi(s);
	अण्णहा
	    p = s;
	जइ (p == रिक्त)
	    retval = FAIL;
	अण्णहा
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	जइ (esc)
	    vim_free(p);
    }
    जइ (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    प्रत्यागम retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
निवेशय_reg(
    पूर्णाङ्क		regname,
    पूर्णाङ्क		literally_arg)	// insert literally, not as if typed
{
    long	i;
    पूर्णाङ्क		retval = OK;
    char_u	*arg;
    पूर्णाङ्क		allocated;
    पूर्णाङ्क		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    जइ (got_int)
	प्रत्यागम FAIL;

    // check for valid regname
    जइ (regname != NUL && !valid_yank_reg(regname, FALSE))
	प्रत्यागम FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    जइ (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    अण्णहा जइ (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	जइ (arg == रिक्त)
	    प्रत्यागम FAIL;
	stuffescaped(arg, literally);
	जइ (allocated)
	    vim_free(arg);
    }
    अण्णहा				// name or number register
    {
	जइ (get_yank_register(regname, FALSE))
	    literally = TRUE;
	जइ (y_current->y_array == रिक्त)
	    retval = FAIL;
	अण्णहा
	{
	    प्रति (i = 0; i < y_current->y_size; ++i)
	    {
		जइ (regname == '-' && y_current->y_type == MCHAR)
		{
		    पूर्णाङ्क dir = BACKWARD;
		    जइ ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			जइ (u_save_cursor() == FAIL)
			    प्रत्यागम FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			जइ (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, रिक्त, dir, 1L, PUT_CURSEND);
		}
		अण्णहा
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    जइ (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    प्रत्यागम retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    पूर्णाङ्क
get_spec_reg(
    पूर्णाङ्क		regname,
    char_u	**argp,
    पूर्णाङ्क		*allocated,	// return: TRUE when value was allocated
    पूर्णाङ्क		errmsg)		// give error message when failing
{
    पूर्णाङ्क		cnt;

    *argp = रिक्त;
    *allocated = FALSE;
    विकल्प (regname)
    {
	पक्ष '%':		// file name
	    जइ (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    प्रत्यागम TRUE;

	पक्ष '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    प्रत्यागम TRUE;

#ifdef FEAT_EVAL
	पक्ष '=':		// result of expression
	    *argp = get_व्यञ्जकम्_पङ्क्तिः();
	    *allocated = TRUE;
	    प्रत्यागम TRUE;
#endif

	पक्ष ':':		// last command line
	    जइ (last_cmdline == रिक्त && errmsg)
		emsg(_(e_no_previous_command_पङ्क्तिः));
	    *argp = last_cmdline;
	    प्रत्यागम TRUE;

	पक्ष '/':		// last search-pattern
	    जइ (last_अन्वेषणम्_pat() == रिक्त && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_अन्वेषणम्_pat();
	    प्रत्यागम TRUE;

	पक्ष '.':		// last inserted text
	    *argp = get_last_निवेशय_save();
	    *allocated = TRUE;
	    जइ (*argp == रिक्त && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    प्रत्यागम TRUE;

	पक्ष Ctrl_F:		// Filename under cursor
	पक्ष Ctrl_P:		// Path under cursor, expand via "path"
	    जइ (!errmsg)
		प्रत्यागम FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, रिक्त);
	    *allocated = TRUE;
	    प्रत्यागम TRUE;

	पक्ष Ctrl_W:		// word under cursor
	पक्ष Ctrl_A:		// WORD (mnemonic All) under cursor
	    जइ (!errmsg)
		प्रत्यागम FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : रिक्त;
	    *allocated = TRUE;
	    प्रत्यागम TRUE;

	पक्ष Ctrl_L:		// Line under cursor
	    जइ (!errmsg)
		प्रत्यागम FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    प्रत्यागम TRUE;

	पक्ष '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    प्रत्यागम TRUE;
    }

    प्रत्यागम FALSE;
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
    पूर्णाङ्क
cmdline_paste_reg(
    पूर्णाङ्क regname,
    पूर्णाङ्क literally_arg,	// Insert text literally instead of "as typed"
    पूर्णाङ्क remcr)		// don't add CR characters
{
    long	i;
    पूर्णाङ्क		literally = literally_arg;

    जइ (get_yank_register(regname, FALSE))
	literally = TRUE;
    जइ (y_current->y_array == रिक्त)
	प्रत्यागम FAIL;

    प्रति (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	जइ ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	जइ (got_int)
	    प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    शून्य
shift_delete_registers(शून्य)
{
    पूर्णाङ्क		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    प्रति (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    जइ (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = रिक्त;		// set register one to empty
}

#जइ defined(FEAT_EVAL)
    स्थिर शून्य
add_regtype_to_dict(पूर्णाङ्क regname, dict_T *dict, char_u *buf, पूर्णाङ्क bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    विकल्प (get_reg_type(regname, &reglen))
    {
	पक्ष MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    भङ्ग;
	पक्ष MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    भङ्ग;
	पक्ष MBLOCK:
	    buflen = vim_snprintf_safelen((वर्ण *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    भङ्ग;
	स्वभाव:
	    buf[0] = NUL;
	    buflen = 0;
	    भङ्ग;
    }
    (शून्य)dict_add_string_len(dict, "regtype", buf, (पूर्णाङ्क)buflen);
}

    शून्य
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    स्थिर पूर्णाङ्क	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    पूर्णाङ्क		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    जइ (recursive)
	प्रत्यागम;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    जइ (list == रिक्त)
	प्रत्यागम;

    // yanked text contents
    प्रति (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (पूर्णाङ्क)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    जइ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (शून्य)dict_add_string_len(v_event, "regname", buf, (पूर्णाङ्क)buflen);

    // motion type: inclusive or exclusive
    (शून्य)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (शून्य)dict_add_string_len(v_event, "operator", buf, (पूर्णाङ्क)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, परिमाण(buf));

    // selection type - visual or not
    (शून्य)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_वस्तूनि_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, रिक्त, रिक्त, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    स्थिर शून्य
put_do_autocmd(
	पूर्णाङ्क	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *निवेशय,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	पूर्णाङ्क	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    स्थिर bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    पूर्णाङ्क		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    जइ (recursive || regname == '_')
	प्रत्यागम;

    जइ (regname != '.' && निवेशय == रिक्त
	    && reg == रिक्त)
	// Can happen when pasting text in normal mode in a terminal buffer
	प्रत्यागम;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    जइ (list == रिक्त)
	प्रत्यागम;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    जइ (regname == '.')
    {
	जइ (last_निवेशय_ga.ga_data != रिक्त)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_निवेशय_ga.ga_data,
		    (पूर्णाङ्क)last_निवेशय_ga.ga_len);
    }
    अण्णहा जइ (निवेशय != रिक्त)
    {
	list_append_string(list, निवेशय->string, (पूर्णाङ्क)निवेशय->length);
    }
    अण्णहा
    {
	प्रति (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (पूर्णाङ्क)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    जइ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (शून्य)dict_add_string_len(v_event, "regname", buf, (पूर्णाङ्क)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (शून्य)dict_add_string_len(v_event, "operator", buf, (पूर्णाङ्क)buflen);

    add_regtype_to_dict(regname, v_event, buf, परिमाण(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (शून्य)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_वस्तूनि_ro(v_event);

    recursive = true;
    textlock++;
    जइ (post)
	apply_autocmds(EVENT_TEXTPUTPOST, रिक्त, रिक्त, FALSE, curbuf);
    अण्णहा
	apply_autocmds(EVENT_TEXTPUTPRE, रिक्त, रिक्त, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    शून्य
init_yank(शून्य)
{
    पूर्णाङ्क		i;

    प्रति (i = 0; i < अङ्कः_REGISTERS; ++i)
	y_regs[i].y_array = रिक्त;
}

#जइ defined(EXITFREE)
    शून्य
clear_registers(शून्य)
{
    पूर्णाङ्क		i;

    प्रति (i = 0; i < अङ्कः_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	जइ (y_current->y_array != रिक्त)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    स्थिर शून्य
free_yank(long n)
{
    जइ (y_current->y_array == रिक्त)
	प्रत्यागम;

    long	    i;

    प्रति (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    शून्य
free_yank_all(शून्य)
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
    पूर्णाङ्क
op_yank(oparg_T *oap, पूर्णाङ्क deleting, पूर्णाङ्क mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    पूर्णाङ्क			yanktype = oap->motion_type;
    long		yanklines = oap->पङ्क्तिः_गणनम्;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    संरचना block_def	bd;

				    // check for read-only register
    जइ (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	प्रत्यागम FAIL;
    }
    जइ (oap->regname == '_')	    // black hole: nothing to do
	प्रत्यागम OK;

#ifdef FEAT_CLIPBOARD
    जइ ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	सन्देशः_warn_missing_clipboard();
    }
#endif

    जइ (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    जइ (y_append && y_current->y_array != रिक्त)
	y_current = &newreg;
    अण्णहा
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    जइ (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(परिमाण(string_T) * yanklines, TRUE);
    जइ (y_current->y_array == रिक्त)
    {
	y_current = curr;
	प्रत्यागम FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    जइ (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	जइ (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    प्रति ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	विकल्प (y_current->y_type)
	{
	    पक्ष MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		जइ (yank_copy_पङ्क्तिः(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    गच्छ fail;
		भङ्ग;

	    पक्ष MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		जइ ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == रिक्त)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    गच्छ fail;
		}
		भङ्ग;

	    पक्ष MCHAR:
		{
		    पूर्णाङ्क tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (पूर्णाङ्क)STRLEN(bd.textstart);
		    जइ (tmp < bd.textlen)
			bd.textlen = tmp;

		    जइ (yank_copy_पङ्क्तिः(&bd, y_idx, FALSE) == FAIL)
			गच्छ fail;
		    भङ्ग;
		}
		// NOTREACHED
	}
    }

    जइ (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	जइ (new_ptr == रिक्त)
	    गच्छ fail;
	प्रति (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	जइ (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	जइ (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == रिक्त)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    जइ (pnew == रिक्त)
	    {
		y_idx = y_current->y_size - 1;
		गच्छ fail;
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
	अण्णहा
	    y_idx = 0;
	जाव (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    जइ (mess)			// Display message about yank?
    {
	जइ (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	जइ (yanklines > p_report)
	{
	    वर्ण namebuf[100];

	    जइ (oap->regname == NUL)
		*namebuf = NUL;
	    अण्णहा
		vim_snprintf(namebuf, परिमाण(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    जइ (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    अण्णहा
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    जइ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	जइ (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	जइ (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    जइ (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	जइ (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    जइ (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	जइ (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	जइ (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    जइ (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# जइ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	जइ (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    जइ (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#जइ defined(FEAT_EVAL)
    जइ (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    प्रत्यागम OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    प्रत्यागम FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    स्थिर पूर्णाङ्क
yank_copy_पङ्क्तिः(संरचना block_def *bd, long y_idx, पूर्णाङ्क exclude_trailing_space)
{
    char_u	*pnew;

    जइ (exclude_trailing_space)
	bd->endspaces = 0;
    जइ ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == रिक्त)
	प्रत्यागम FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    जइ (exclude_trailing_space)
    {
	पूर्णाङ्क s = bd->textlen + bd->endspaces;

	जाव (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    प्रत्यागम OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    स्थिर शून्य
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    परिमाण(string_T) * y_current->y_size, TRUE);
    जइ (y_current->y_array == रिक्त)
	y_current->y_size = 0;
    अण्णहा
	प्रति (j = 0; j < y_current->y_size; ++j)
	{
	    जइ ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == रिक्त)
	    {
		free_yank(j);
		y_current->y_size = 0;
		भङ्ग;
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
    शून्य
do_put(
    पूर्णाङ्क		regname,
    char_u	*व्यञ्जकम्_result,	// result for regname "=" when compiled
    पूर्णाङ्क		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	गणनम्,
    पूर्णाङ्क		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    पूर्णाङ्क		yanklen;
    पूर्णाङ्क		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    पूर्णाङ्क		y_type;
    long	y_size;
    पूर्णाङ्क		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = रिक्त;
    yankreg_T	*y_current_used = रिक्त;
    long	nr_पङ्क्तयः = 0;
    string_T	निवेशय_string;
    पूर्णाङ्क		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned पूर्णाङ्क cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(शून्य)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    जइ (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    जइ (regname == '.')
    {
	जइ (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	जइ (has_textput_events)
	    add_last_निवेशय++;
#endif

	(शून्य)stuff_inserted((dir == FORWARD ? (गणनम् == -1 ? 'o' : 'a') :
				    (गणनम् == -1 ? 'O' : 'i')), गणनम्, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	जइ (has_textputpre())
	    put_do_autocmd('.', रिक्त, रिक्त, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	जइ (has_textputpost())
	    put_do_autocmd('.', रिक्त, रिक्त, true, dir);

	जइ (has_textput_events && --add_last_निवेशय == 0)
	    ga_clear(&last_निवेशय_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	जइ ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	प्रत्यागम;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    निवेशय_string.string = रिक्त;
    जइ (regname == '=' && व्यञ्जकम्_result != रिक्त)
	निवेशय_string.string = व्यञ्जकम्_result;
    अण्णहा जइ (get_spec_reg(regname, &निवेशय_string.string, &allocated, TRUE)
		&& निवेशय_string.string == रिक्त)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	प्रत्यागम;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    जइ (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	गच्छ end;

    जइ (निवेशय_string.string != रिक्त)
    {
	निवेशय_string.length = STRLEN(निवेशय_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	जइ (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    प्रति (;;)
	    {
		y_size = 0;
		ptr = निवेशय_string.string;
		ptrlen = निवेशय_string.length;
		जाव (ptr != रिक्त)
		{
		    जइ (y_array != रिक्त)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    जइ (tmp == रिक्त)
		    {
			जइ (y_array != रिक्त)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    अण्णहा
		    {
			जइ (y_array != रिक्त)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			जइ (*tmp == NUL)
			{
			    y_type = MLINE;
			    भङ्ग;
			}
		    }
		    ptr = tmp;
		}
		जइ (y_array != रिक्त)
		    भङ्ग;
		y_array = ALLOC_MULT(string_T, y_size);
		जइ (y_array == रिक्त)
		    गच्छ end;
	    }
	}
	अण्णहा
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &निवेशय_string;
	}
#ifdef FEAT_EVAL
	जइ (has_textputpre())
	    put_do_autocmd(regname, रिक्त, &निवेशय_string, false, dir);
#endif
    }
    अण्णहा
    {
#ifdef FEAT_EVAL
	जइ (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, रिक्त, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    जइ (y_type == MLINE)
    {
	जइ (flags & PUT_पङ्क्तिः_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    जइ (u_save_cursor() == FAIL)
		गच्छ end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    जइ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    जइ (ptr == रिक्त)
		गच्छ end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    जइ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    जइ (ptr == रिक्त)
		गच्छ end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_पङ्क्तयः;
	    dir = FORWARD;
	}
	जइ (flags & PUT_पङ्क्तिः_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    जइ (flags & PUT_पङ्क्तिः)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    जइ (y_size == 0 || y_array == रिक्त)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	गच्छ end;
    }

    जइ (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	जइ (lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम् + 1;
	जइ (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    गच्छ end;
    }
    अण्णहा जइ (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	जइ (dir == BACKWARD)
	    (शून्य)hasFolding(lnum, &lnum, रिक्त);
	अण्णहा
	    (शून्य)hasFolding(lnum, रिक्त, &lnum);
#endif
	जइ (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	जइ ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    गच्छ end;
#ifdef FEAT_FOLDING
	जइ (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	अण्णहा
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    अण्णहा जइ (u_save_cursor() == FAIL)
	गच्छ end;

    जइ (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	जइ (gchar_cursor() == TAB)
	{
	    पूर्णाङ्क viscol = getviscol();
	    पूर्णाङ्क ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    जइ (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#अण्णहा
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    अण्णहा
		curwin->w_cursor.coladd = 0;
	}
	अण्णहा जइ (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    जइ (y_type == MBLOCK)
    {
	पूर्णाङ्क	delcount;
	पूर्णाङ्क	incr = 0;
	संरचना block_def bd;
	long	j;
	पूर्णाङ्क	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	जइ (dir == FORWARD && c != NUL)
	{
	    जइ (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, रिक्त, &endcol2, 0);
	    अण्णहा
		getvcol(curwin, &curwin->w_cursor, रिक्त, रिक्त, &col, 0);

	    जइ (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    अण्णहा
		जइ (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	अण्णहा
	    getvcol(curwin, &curwin->w_cursor, &col, रिक्त, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	जइ (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    जइ (dir == FORWARD && c == NUL)
		++col;
	    जइ (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    जइ (c == TAB)
	    {
		जइ (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		जइ (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	प्रति (i = 0; i < y_size; ++i)
	{
	    पूर्णाङ्क		    spaces = 0;
	    वर्ण	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    जइ (curwin->w_cursor.lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    {
		जइ (ml_append(curbuf->b_ml.ml_पङ्क्तिः_गणनम्, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    भङ्ग;
		++nr_पङ्क्तयः;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    जाव (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    जइ (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    अण्णहा जइ (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		जइ (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		जइ (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (पूर्णाङ्क)y_array[i].length;

	    जइ ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		जाव (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		जइ (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    जइ (yanklen + spaces != 0
		     && गणनम् > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		भङ्ग;
	    }

	    totlen = गणनम् * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    जइ (newp == रिक्त)
		भङ्ग;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    प्रति (j = 0; j < गणनम्; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		जइ ((j < गणनम् - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		अण्णहा
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
	    जइ (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_पङ्क्तयः(lnum, 0, curwin->w_cursor.lnum - nr_पङ्क्तयः, nr_पङ्क्तयः);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	जइ (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	जइ (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    जइ (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	अण्णहा
	    curwin->w_cursor.lnum = lnum;
    }
    अण्णहा
    {
	pos_T	new_cursor;

	yanklen = (पूर्णाङ्क)y_array[0].length;

	// Character or Line mode
	जइ (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    जइ (dir == FORWARD && gchar_cursor() != NUL)
	    {
		जइ (has_mbyte)
		{
		    पूर्णाङ्क bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    जइ (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		अण्णहा
		{
		    ++col;
		    जइ (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	अण्णहा जइ (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	जइ (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    पूर्णाङ्क		first_byte_off = 0;

	    जइ (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		जइ (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		जइ (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, रिक्त, &vcol, रिक्त, 0);
		}
	    }

	    जइ (गणनम् == 0 || yanklen == 0)
	    {
		जइ (VIsual_active)
		    lnum = end_lnum;
	    }
	    अण्णहा जइ (गणनम् > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    अण्णहा
	    {
		totlen = गणनम् * yanklen;
		करेहि
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    जइ (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			जइ (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			अण्णहा
			    col = MAXCOL;
		    }
		    जइ (VIsual_active && col > oldlen)
		    {
			lnum++;
			अनुवर्त;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    जइ (newp == रिक्त)
			गच्छ end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    प्रति (i = 0; i < गणनम्; ++i)
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
		    जइ (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    जइ (VIsual_active)
			lnum++;
		} जाव (VIsual_active && lnum <= end_lnum);

		जइ (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    जइ (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    अण्णहा
		curwin->w_cursor.col -= first_byte_off;
	}
	अण्णहा
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    पूर्णाङ्क		indent;
	    पूर्णाङ्क		orig_indent = 0;
	    पूर्णाङ्क		indent_diff = 0;	// init for gcc
	    पूर्णाङ्क		first_indent = TRUE;
	    पूर्णाङ्क		lendiff = 0;
	    long	cnt;

	    जइ (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    प्रति (cnt = 1; cnt <= गणनम्; ++cnt)
	    {
		i = 0;
		जइ (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (पूर्णाङ्क)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    जइ (newp == रिक्त)
			गच्छ error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    जइ (newp == रिक्त)
			गच्छ error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		प्रति (; i < y_size; ++i)
		{
		    जइ (y_type != MCHAR || i < y_size - 1)
		    {
			जइ (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    गच्छ error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_पङ्क्तयः;
		    जइ (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			जइ (cnt == गणनम् && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			जइ (*ptr == '#' && preprocs_वामः())
			    indent = 0;     // Leave # lines at start
			अण्णहा जइ (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			अण्णहा जइ (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			अण्णहा जइ ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(शून्य)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			जइ (cnt == गणनम् && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		जइ (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    जइ (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		जइ (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_पङ्क्तयः, 0L);

	    // note changed text for displaying and folding
	    जइ (y_type == MCHAR)
		changed_पङ्क्तयः(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_पङ्क्तयः);
	    अण्णहा
		changed_पङ्क्तयः(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_पङ्क्तयः);
	    जइ (y_current_used != रिक्त && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		गच्छ end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    जइ (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		जइ (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    अण्णहा
		curbuf->b_op_end.col = 0;

	    जइ (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    अण्णहा जइ (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		जइ (y_type == MLINE)
		{
		    जइ (lnum >= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
		    अण्णहा
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		अण्णहा
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    जइ (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    अण्णहा जइ (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		जइ (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    अण्णहा	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_पङ्क्तयः);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    पूर्णाङ्क len = ml_get_curline_len();
    जइ (curwin->w_cursor.col > len)
    {
	जइ (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    जइ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    जइ (has_textputpost())
    {
	जइ (निवेशय_string.string == रिक्त)
	    put_do_autocmd(regname, y_current, रिक्त, true, dir);
	अण्णहा
	    put_do_autocmd(regname, रिक्त, &निवेशय_string, true, dir);
    }
#endif

    जइ (allocated)
	vim_free(निवेशय_string.string);
    जइ (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    पूर्णाङ्क
get_register_name(पूर्णाङ्क अङ्कः)
{
    जइ (अङ्कः == -1)
	प्रत्यागम '"';
    अण्णहा जइ (अङ्कः < 10)
	प्रत्यागम अङ्कः + '0';
    अण्णहा जइ (अङ्कः == DELETION_REGISTER)
	प्रत्यागम '-';
#जइ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    अण्णहा जइ (अङ्कः == STAR_REGISTER)
	प्रत्यागम '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    अण्णहा जइ (clipmethod == CLIPMETHOD_PROVIDER && अङ्कः == REAL_PLUS_REGISTER)
	प्रत्यागम '+';
    अण्णहा जइ (clipmethod != CLIPMETHOD_PROVIDER && अङ्कः == PLUS_REGISTER)
	प्रत्यागम '+';
#endif
    अण्णहा
	प्रत्यागम अङ्कः + 'a' - 10;
}

#जइ defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    पूर्णाङ्क
get_unname_register(शून्य)
{
    प्रत्यागम y_previous == रिक्त ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    शून्य
ex_display(exarg_T *eap)
{
    पूर्णाङ्क		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    पूर्णाङ्क		name;
    पूर्णाङ्क		attr;
    char_u	*arg = eap->arg;
    पूर्णाङ्क		clen;
    पूर्णाङ्क		type;
    string_T	निवेशय;

    silence_w23_w24_सन्देशः++;
    जइ (arg != रिक्त && *arg == NUL)
	arg = रिक्त;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    सन्देशः_puts_title(_("\nType Name Content"));
    प्रति (i = -1; i < अङ्कः_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	विकल्प (get_reg_type(name, रिक्त))
	{
	    पक्ष MLINE: type = 'l'; भङ्ग;
	    पक्ष MCHAR: type = 'c'; भङ्ग;
	    स्वभाव:	type = 'b'; भङ्ग;
	}
	जइ (arg != रिक्त && vim_strchr(arg, name) == रिक्त
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == रिक्त)
#endif
		)
	    अनुवर्त;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	जइ (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (शून्य)may_get_selection(name);
#endif

	जइ (i == -1)
	{
	    जइ (y_previous != रिक्त)
		yb = y_previous;
	    अण्णहा
		yb = &(y_regs[0]);
	}
	अण्णहा
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	जइ (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != रिक्त &&
		    (yb == y_previous || yb == &y_regs[0])))
	    अनुवर्त;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	जइ (yb->y_array != रिक्त)
	{
	    पूर्णाङ्क do_show = FALSE;

	    प्रति (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    जइ (do_show || yb->y_size == 0)
	    {
		सन्देशः_putchar('\n');
		सन्देशः_puts("  ");
		सन्देशः_putchar(type);
		सन्देशः_puts("  ");
		सन्देशः_putchar('"');
		सन्देशः_putchar(name);
		सन्देशः_puts("   ");

		n = (पूर्णाङ्क)Columns - 11;
		प्रति (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    जइ (j)
		    {
			सन्देशः_puts_attr("^J", attr);
			n -= 2;
		    }
		    प्रति (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			सन्देशः_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		जइ (n > 1 && yb->y_type == MLINE)
		    सन्देशः_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    निवेशय = get_last_निवेशय();
    जइ ((p = निवेशय.string) != रिक्त
		  && (arg == रिक्त || vim_strchr(arg, '.') != रिक्त) && !got_int
						      && !message_filtered(p))
    {
	सन्देशः_puts("\n  c  \".   ");
	dis_सन्देशः(p, TRUE);
    }

    // display last command line
    जइ (last_cmdline != रिक्त && (arg == रिक्त || vim_strchr(arg, ':') != रिक्त)
			       && !got_int && !message_filtered(last_cmdline))
    {
	सन्देशः_puts("\n  c  \":   ");
	dis_सन्देशः(last_cmdline, FALSE);
    }

    // display current file name
    जइ (curbuf->b_fname != रिक्त
	    && (arg == रिक्त || vim_strchr(arg, '%') != रिक्त) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	सन्देशः_puts("\n  c  \"%   ");
	dis_सन्देशः(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    जइ ((arg == रिक्त || vim_strchr(arg, '#') != रिक्त) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	जइ (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    सन्देशः_puts("\n  c  \"#   ");
	    dis_सन्देशः(fname, FALSE);
	}
    }

    // display last search pattern
    जइ (last_अन्वेषणम्_pat() != रिक्त
		 && (arg == रिक्त || vim_strchr(arg, '/') != रिक्त) && !got_int
				      && !message_filtered(last_अन्वेषणम्_pat()))
    {
	सन्देशः_puts("\n  c  \"/   ");
	dis_सन्देशः(last_अन्वेषणम्_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    जइ (व्यञ्जकम्_पङ्क्तिः != रिक्त && (arg == रिक्त || vim_strchr(arg, '=') != रिक्त)
				  && !got_int && !message_filtered(व्यञ्जकम्_पङ्क्तिः))
    {
	सन्देशः_puts("\n  c  \"=   ");
	dis_सन्देशः(व्यञ्जकम्_पङ्क्तिः, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_सन्देशः--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    स्थिर शून्य
dis_सन्देशः(
    char_u	*p,
    पूर्णाङ्क		skip_esc)	    // if TRUE, ignore trailing ESC
{
    पूर्णाङ्क		n;
    पूर्णाङ्क		l;

    n = (पूर्णाङ्क)Columns - 6;
    जाव (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	जइ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    सन्देशः_outtrans_len(p, l);
	    p += l;
	}
	अण्णहा
	    सन्देशः_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#जइ defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    शून्य
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
get_reg_type(पूर्णाङ्क regname, long *reglen)
{
    विकल्प (regname)
    {
	पक्ष '%':		// file name
	पक्ष '#':		// alternate file name
	पक्ष '=':		// expression
	पक्ष ':':		// last command line
	पक्ष '/':		// last search-pattern
	पक्ष '.':		// last inserted text
	पक्ष Ctrl_F:		// Filename under cursor
	पक्ष Ctrl_P:		// Path under cursor, expand via "path"
	पक्ष Ctrl_W:		// word under cursor
	पक्ष Ctrl_A:		// WORD (mnemonic All) under cursor
	पक्ष '_':		// black hole: always empty
	    प्रत्यागम MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    जइ (regname != NUL && !valid_yank_reg(regname, FALSE))
	प्रत्यागम MAUTO;

    get_yank_register(regname, FALSE);

    जइ (y_current->y_array != रिक्त)
    {
	जइ (reglen != रिक्त && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	प्रत्यागम y_current->y_type;
    }
    प्रत्यागम MAUTO;
}

#जइ defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    स्थिर char_u *
getreg_wrap_one_पङ्क्तिः(char_u *s, पूर्णाङ्क flags)
{
    जइ (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	जइ (list != रिक्त)
	{
	    जइ (list_append_string(list, रिक्त, -1) == FAIL)
	    {
		list_free(list);
		प्रत्यागम रिक्त;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	प्रत्यागम (char_u *)list;
    }
    प्रत्यागम s;
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
get_reg_contents(पूर्णाङ्क regname, पूर्णाङ्क flags)
{
    linenr_T	i;
    char_u	*retval;
    पूर्णाङ्क		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    जइ (regname == '=')
    {
	जइ (flags & GREG_NO_व्यञ्जकम्)
	    प्रत्यागम रिक्त;
	जइ (flags & GREG_व्यञ्जकम्_SRC)
	    प्रत्यागम getreg_wrap_one_पङ्क्तिः(get_व्यञ्जकम्_पङ्क्तिः_src(), flags);
	प्रत्यागम getreg_wrap_one_पङ्क्तिः(get_व्यञ्जकम्_पङ्क्तिः(), flags);
    }

    जइ (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    जइ (regname != NUL && !valid_yank_reg(regname, FALSE))
	प्रत्यागम रिक्त;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    जइ (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	जइ (retval == रिक्त)
	    प्रत्यागम रिक्त;
	जइ (allocated)
	    प्रत्यागम getreg_wrap_one_पङ्क्तिः(retval, flags);
	प्रत्यागम getreg_wrap_one_पङ्क्तिः(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    जइ (y_current->y_array == रिक्त)
	प्रत्यागम रिक्त;

    जइ (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	जइ (list == रिक्त)
	    प्रत्यागम रिक्त;
	प्रति (i = 0; i < y_current->y_size; ++i)
	{
	    जइ (list_append_string(list, y_current->y_array[i].string,
		(पूर्णाङ्क)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		प्रत्यागम रिक्त;
	    }
	}
	प्रत्यागम (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    प्रति (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	जइ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    जइ (retval == रिक्त)
	प्रत्यागम रिक्त;

    // Copy the lines of the yank register into the string.
    len = 0;
    प्रति (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	जइ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    प्रत्यागम retval;
}

    स्थिर पूर्णाङ्क
init_write_reg(
    पूर्णाङ्क		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    पूर्णाङ्क		must_append,
    पूर्णाङ्क		*yank_type UNUSED)
{
    जइ (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	प्रत्यागम FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    जइ (!y_append && !must_append)
	free_yank_all();
    प्रत्यागम OK;
}

    स्थिर शून्य
finish_write_reg(
    पूर्णाङ्क		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    जइ (name != '"')
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
    शून्य
write_reg_contents(
    पूर्णाङ्क		name,
    char_u	*str,
    पूर्णाङ्क		maxlen,
    पूर्णाङ्क		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    शून्य
write_reg_contents_lst(
    पूर्णाङ्क		name,
    char_u	**strings,
    पूर्णाङ्क		maxlen UNUSED,
    पूर्णाङ्क		must_append,
    पूर्णाङ्क		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    जइ (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	जइ (strings[0] == रिक्त)
	    s = (char_u *)"";
	अण्णहा जइ (strings[1] != रिक्त)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_पङ्क्तयः), name);
	    प्रत्यागम;
	}
	अण्णहा
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	प्रत्यागम;
    }

    जइ (name == '_')	    // black hole: nothing to do
	प्रत्यागम;

    जइ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	प्रत्यागम;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    शून्य
write_reg_contents_ex(
    पूर्णाङ्क		name,
    char_u	*str,
    पूर्णाङ्क		maxlen,
    पूर्णाङ्क		must_append,
    पूर्णाङ्क		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    जइ (maxlen >= 0)
	len = maxlen;
    अण्णहा
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    जइ (name == '/')
    {
	set_last_अन्वेषणम्_pat(str, RE_अन्वेषणम्, TRUE, TRUE);
	प्रत्यागम;
    }

    जइ (name == '#')
    {
	जइ (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  प्रत्यागम;
	}

	buf_T	*buf;

	जइ (VIM_ISDIGIT(*str))
	{
	    पूर्णाङ्क	अङ्कः = atoi((वर्ण *)str);

	    buf = buflist_findnr(अङ्कः);
	    जइ (buf == रिक्त)
		semsg(_(e_buffer_nr_does_not_exist), (long)अङ्कः);
	}
	अण्णहा
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	जइ (buf == रिक्त)
	    प्रत्यागम;
	curwin->w_alt_fnum = buf->b_fnum;
	प्रत्यागम;
    }

    जइ (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	जइ (p == रिक्त)
	    प्रत्यागम;
	जइ (must_append && व्यञ्जकम्_पङ्क्तिः != रिक्त)
	{
	    s = concat_str(व्यञ्जकम्_पङ्क्तिः, p);
	    vim_free(p);
	    p = s;
	}
	set_व्यञ्जकम्_पङ्क्तिः(p, रिक्त);
	प्रत्यागम;
    }

    जइ (name == '_')	    // black hole: nothing to do
	प्रत्यागम;

    जइ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	प्रत्यागम;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#जइ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    शून्य
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    पूर्णाङ्क		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    पूर्णाङ्क		str_list)	// TRUE if str is char_u **
{
    पूर्णाङ्क		type;			// MCHAR, MLINE or MBLOCK
    पूर्णाङ्क		lnum;
    long	start;
    long	i;
    पूर्णाङ्क		extra;
    पूर्णाङ्क		newlines;		// number of lines added
    पूर्णाङ्क		extraline = 0;		// extra line at the end
    पूर्णाङ्क		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    जइ (y_ptr->y_array == रिक्त)		// NULL means empty register
	y_ptr->y_size = 0;

    जइ (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    अण्णहा
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    जइ (str_list)
    {
	प्रति (ss = (char_u **) str; *ss != रिक्त; ++ss)
	    ++newlines;
    }
    अण्णहा
    {
	प्रति (i = 0; i < len; i++)
	    जइ (str[i] == '\n')
		++newlines;
	जइ (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	जइ (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    जइ (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	प्रत्यागम;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * परिमाण(string_T), TRUE);
    जइ (pp == रिक्त)	// out of memory
	प्रत्यागम;
    प्रति (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    जइ (str_list)
    {
	प्रति (ss = (char_u **) str; *ss != रिक्त; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    जइ (type == MBLOCK)
	    {
		पूर्णाङ्क charlen = vim_strsize(*ss);

		जइ (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    अण्णहा
    {
	प्रति (start = 0; start < len + extraline; start += i + 1)
	{
	    पूर्णाङ्क charlen = 0;

	    प्रति (i = start; i < len;)	// find the end of the line
	    {
		जइ (str[i] == '\n')
		    भङ्ग;
		जइ (type == MBLOCK)
		{
		    जइ (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    अण्णहा
			charlen += byte2cells(str[i]);
		}

		जइ (str[i] == NUL)
		    i++; // registers can have NUL chars
		अण्णहा
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    जइ (charlen > maxlen)
		maxlen = charlen;
	    जइ (append)
	    {
		--lnum;
		extra = (पूर्णाङ्क)y_ptr->y_array[lnum].length;
	    }
	    अण्णहा
		extra = 0;
	    s = alloc(i + extra + 1);
	    जइ (s == रिक्त)
		भङ्ग;
	    जइ (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    जइ (append)
		vim_free(y_ptr->y_array[lnum].string);
	    जइ (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    जाव (--extra >= 0)
	    {
		जइ (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    जइ (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    अण्णहा
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
