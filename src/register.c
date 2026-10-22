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
स्थिर yankreg_T	y_regs[अंक_REGISTERS];

स्थिर yankreg_T	*y_current;	    // ptr to current yankreg
स्थिर पूर्णांक		y_append;	    // TRUE when appending
स्थिर yankreg_T	*y_previous = रिक्त; // ptr to last written yankreg

स्थिर पूर्णांक	stuff_yank(पूर्णांक, char_u *);
स्थिर शून्य	put_reedit_in_typebuf(पूर्णांक silent);
स्थिर पूर्णांक	put_in_typebuf(char_u *s, पूर्णांक esc, पूर्णांक colon, पूर्णांक silent);
स्थिर पूर्णांक	yank_copy_ओळ(संरचना block_def *bd, दीर्घ y_idx, पूर्णांक exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
स्थिर शून्य	copy_yank_reg(yankreg_T *reg);
#endif
स्थिर शून्य	dis_संदेश(char_u *p, पूर्णांक skip_esc);

#जर defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(शून्य)
{
    परत y_regs;
}
#endif

#जर defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(पूर्णांक reg)
{
    परत &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(शून्य)
{
    परत y_current;
}

    yankreg_T *
get_y_previous(शून्य)
{
    परत y_previous;
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


#जर defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
स्थिर char_u	*अभिव्यक्ती_ओळ = रिक्त;
स्थिर exarg_T	*अभिव्यक्ती_eap = रिक्त;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    पूर्णांक
get_अभिव्यक्ती_register(शून्य)
{
    char_u	*new_ओळ;

    new_ओळ = getcmdline('=', 0L, 0, 0);
    जर (new_ओळ == रिक्त)
	परत NUL;
    जर (*new_ओळ == NUL)	// use previous line
	vim_free(new_ओळ);
    अन्यथा
	set_अभिव्यक्ती_ओळ(new_ओळ, रिक्त);
    परत '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    शून्य
set_अभिव्यक्ती_ओळ(char_u *new_ओळ, exarg_T *eap)
{
    vim_free(अभिव्यक्ती_ओळ);
    अभिव्यक्ती_ओळ = new_ओळ;
    अभिव्यक्ती_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_अभिव्यक्ती_ओळ(शून्य)
{
    char_u	*अभिव्यक्ती_copy;
    char_u	*rv;
    स्थिर पूर्णांक	nested = 0;

    जर (अभिव्यक्ती_ओळ == रिक्त)
	परत रिक्त;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    अभिव्यक्ती_copy = vim_strsave(अभिव्यक्ती_ओळ);
    जर (अभिव्यक्ती_copy == रिक्त)
	परत रिक्त;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    जर (nested >= 10)
	परत अभिव्यक्ती_copy;

    ++nested;
    rv = eval_to_string_eap(अभिव्यक्ती_copy, TRUE, अभिव्यक्ती_eap, FALSE);
    --nested;
    vim_free(अभिव्यक्ती_copy);
    परत rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    स्थिर char_u *
get_अभिव्यक्ती_ओळ_src(शून्य)
{
    जर (अभिव्यक्ती_ओळ == रिक्त)
	परत रिक्त;
    परत vim_strsave(अभिव्यक्ती_ओळ);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    पूर्णांक
valid_yank_reg(
    पूर्णांक	    regname,
    पूर्णांक	    writing)	    // if TRUE check for writable registers
{
    जर (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#अन्यथा
				    "/#.%:"
#endif
					, regname) != रिक्त)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#जर defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#किंवाजर defined(FEAT_CLIPBOARD_PROVIDER)
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
	परत TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    अन्यथा जर (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	संदेश_warn_missing_clipboard();
	परत FALSE;
    }
#endif
    परत FALSE;
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
    पूर्णांक
get_yank_register(पूर्णांक regname, पूर्णांक writing)
{
    पूर्णांक	    i;
    पूर्णांक	    ret = FALSE;

    y_append = FALSE;
    जर ((regname == 0 || regname == '"') && !writing && y_previous != रिक्त)
    {
	y_current = y_previous;
	परत ret;
    }
    i = regname;
    जर (VIM_ISDIGIT(i))
	i -= '0';
    अन्यथा जर (ASCII_ISLOWER(i))
	i = Char_Ord_खालचा(i) + 10;
    अन्यथा जर (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    अन्यथा जर (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    अन्यथा जर (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    अन्यथा जर (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	जर (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	अन्यथा
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#किंवाजर defined(FEAT_CLIPBOARD_PROVIDER)
    अन्यथा जर (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    अन्यथा जर (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    अन्यथा जर (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    अन्यथा		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    जर (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    परत ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    शून्य *
get_register(
    पूर्णांक		name,
    पूर्णांक		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    पूर्णांक		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    जर (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	जर (name == '*' && clip_star.available)
	{
	    जर (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	जर (name == '+' && clip_plus.available)
	{
	    जर (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    जर (reg == रिक्त)
	परत (शून्य *)रिक्त;

    *reg = *y_current;
    जर (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	जर (reg->y_size == 0 || y_current->y_array == रिक्त)
	    reg->y_array = रिक्त;
	अन्यथा
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	जर (reg->y_array != रिक्त)
	{
	    साठी (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		जर (reg->y_array[i].string == रिक्त)
		{
		    // The allocation failed so clean up and exit
		    पर्यंत (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    परत (शून्य *)रिक्त;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    अन्यथा
	y_current->y_array = रिक्त;
    परत (शून्य *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    शून्य
put_register(पूर्णांक name, शून्य *reg)
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

#जर defined(FEAT_CLIPBOARD)
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
    पूर्णांक
yank_register_mline(पूर्णांक regname)
{
    जर (regname != 0 && !valid_yank_reg(regname, FALSE))
	परत FALSE;
    जर (regname == '_')		// black hole is always empty
	परत FALSE;
    get_yank_register(regname, FALSE);
    परत (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णांक
do_record(पूर्णांक c)
{
    char_u	    *p;
    स्थिर पूर्णांक	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    पूर्णांक		    retval;

    जर (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	जर (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	अन्यथा
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    अन्यथा			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	संदेश("");
	p = get_recorded();
	जर (p == रिक्त)
	    retval = FAIL;
	अन्यथा
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
    परत retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    स्थिर पूर्णांक
stuff_yank(पूर्णांक regname, char_u *p)
{
    माप_प्रकार  plen;

    // check for read-only register
    जर (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	परत FAIL;
    }
    जर (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	परत OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    जर (y_append && y_current->y_array != रिक्त)
    {
	string_T    *pp;
	char_u	    *tmp;
	माप_प्रकार	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	जर (tmp == रिक्त)
	{
	    vim_free(p);
	    परत FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    अन्यथा
    {
	free_yank_all();
	जर ((y_current->y_array = ALLOC_ONE(string_T)) == रिक्त)
	{
	    vim_free(p);
	    परत FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    परत OK;
}

/*
 * Last executed register (@ command)
 */
स्थिर पूर्णांक execreg_lastc = NUL;

#जर defined(FEAT_VIMINFO)
    पूर्णांक
get_execreg_lastc(शून्य)
{
    परत execreg_lastc;
}

    शून्य
set_execreg_lastc(पूर्णांक lastc)
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
execreg_ओळ_continuation(string_T *ओळी, दीर्घ *idx)
{
    garray_T	ga;
    दीर्घ	cmd_start = *idx;
    दीर्घ	cmd_end = *idx;
    string_T	*tmp;
    पूर्णांक		j;
    char_u	*str;

    ga_init2(&ga, आकार(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    पर्यंत (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(ओळी[cmd_start].string);
	जर (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    थांबा;
    }

    // join all the lines
    tmp = &ओळी[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    साठी (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &ओळी[j];
	p = skipwhite(tmp->string);
	जर (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    जर (ga.ga_len > 400)
	    {
		जर (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		अन्यथा
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
    परत str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णांक
do_execreg(
    पूर्णांक	    regname,
    पूर्णांक	    colon,		// insert ':' before each line
    पूर्णांक	    addcr,		// always add '\n' to end of line
    पूर्णांक	    silent)		// set "silent" flag in typeahead buffer
{
    दीर्घ	i;
    char_u	*p;
    पूर्णांक		retval = OK;
    पूर्णांक		remap;

    // repeat previous one
    जर (regname == '@')
    {
	जर (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    परत FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    जर (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	परत FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    जर (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    जर (regname == '_')
	परत OK;

    // use last command line
    जर (regname == ':')
    {
	जर (last_cmdline == रिक्त)
	{
	    emsg(_(e_no_previous_command_ओळ));
	    परत FAIL;
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
	जर (p != रिक्त)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    जर (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    अन्यथा
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    अन्यथा जर (regname == '=')
    {
	p = get_अभिव्यक्ती_ओळ();
	जर (p == रिक्त)
	    परत FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    अन्यथा जर (regname == '.')		// use last inserted text
    {
	p = get_last_घाला_save();
	जर (p == रिक्त)
	{
	    emsg(_(e_no_inserted_text_yet));
	    परत FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    अन्यथा
    {
	get_yank_register(regname, FALSE);
	जर (y_current->y_array == रिक्त)
	    परत FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	साठी (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    पूर्णांक	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    जर (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		जर (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    परत FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    जर (colon && i > 0)
	    {
		p = skipwhite(str);
		जर (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_ओळ_continuation(y_current->y_array, &i);
		    जर (str == रिक्त)
			परत FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    जर (free_str)
		vim_free(str);
	    जर (escaped == रिक्त)
		परत FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    जर (retval == FAIL)
		परत FAIL;
	    जर (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		परत FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    परत retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    स्थिर शून्य
put_reedit_in_typebuf(पूर्णांक silent)
{
    जर (restart_edit == NUL)
	परत;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    जर (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    अन्यथा जर (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    अन्यथा जर (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    जर (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    स्थिर पूर्णांक
put_in_typebuf(
    char_u	*s,
    पूर्णांक		esc,
    पूर्णांक		colon,	    // add ':' before the line
    पूर्णांक		silent)
{
    पूर्णांक		retval = OK;

    put_reedit_in_typebuf(silent);
    जर (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    जर (retval == OK)
    {
	char_u	*p;

	जर (esc)
	    p = vim_strsave_escape_csi(s);
	अन्यथा
	    p = s;
	जर (p == रिक्त)
	    retval = FAIL;
	अन्यथा
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	जर (esc)
	    vim_free(p);
    }
    जर (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    परत retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णांक
घाला_reg(
    पूर्णांक		regname,
    पूर्णांक		literally_arg)	// insert literally, not as if typed
{
    दीर्घ	i;
    पूर्णांक		retval = OK;
    char_u	*arg;
    पूर्णांक		allocated;
    पूर्णांक		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    जर (got_int)
	परत FAIL;

    // check for valid regname
    जर (regname != NUL && !valid_yank_reg(regname, FALSE))
	परत FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    जर (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    जर (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    अन्यथा जर (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	जर (arg == रिक्त)
	    परत FAIL;
	stuffescaped(arg, literally);
	जर (allocated)
	    vim_free(arg);
    }
    अन्यथा				// name or number register
    {
	जर (get_yank_register(regname, FALSE))
	    literally = TRUE;
	जर (y_current->y_array == रिक्त)
	    retval = FAIL;
	अन्यथा
	{
	    साठी (i = 0; i < y_current->y_size; ++i)
	    {
		जर (regname == '-' && y_current->y_type == MCHAR)
		{
		    पूर्णांक dir = BACKWARD;
		    जर ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			जर (u_save_cursor() == FAIL)
			    परत FAIL;
			del_chars((दीर्घ)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			जर (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, रिक्त, dir, 1L, PUT_CURSEND);
		}
		अन्यथा
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    जर (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    परत retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    पूर्णांक
get_spec_reg(
    पूर्णांक		regname,
    char_u	**argp,
    पूर्णांक		*allocated,	// return: TRUE when value was allocated
    पूर्णांक		errmsg)		// give error message when failing
{
    पूर्णांक		cnt;

    *argp = रिक्त;
    *allocated = FALSE;
    निवड (regname)
    {
	स्थिती '%':		// file name
	    जर (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    परत TRUE;

	स्थिती '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    परत TRUE;

#ifdef FEAT_EVAL
	स्थिती '=':		// result of expression
	    *argp = get_अभिव्यक्ती_ओळ();
	    *allocated = TRUE;
	    परत TRUE;
#endif

	स्थिती ':':		// last command line
	    जर (last_cmdline == रिक्त && errmsg)
		emsg(_(e_no_previous_command_ओळ));
	    *argp = last_cmdline;
	    परत TRUE;

	स्थिती '/':		// last search-pattern
	    जर (last_शोध_pat() == रिक्त && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_शोध_pat();
	    परत TRUE;

	स्थिती '.':		// last inserted text
	    *argp = get_last_घाला_save();
	    *allocated = TRUE;
	    जर (*argp == रिक्त && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    परत TRUE;

	स्थिती Ctrl_F:		// Filename under cursor
	स्थिती Ctrl_P:		// Path under cursor, expand via "path"
	    जर (!errmsg)
		परत FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, रिक्त);
	    *allocated = TRUE;
	    परत TRUE;

	स्थिती Ctrl_W:		// word under cursor
	स्थिती Ctrl_A:		// WORD (mnemonic All) under cursor
	    जर (!errmsg)
		परत FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : रिक्त;
	    *allocated = TRUE;
	    परत TRUE;

	स्थिती Ctrl_L:		// Line under cursor
	    जर (!errmsg)
		परत FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    परत TRUE;

	स्थिती '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    परत TRUE;
    }

    परत FALSE;
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
    पूर्णांक
cmdline_paste_reg(
    पूर्णांक regname,
    पूर्णांक literally_arg,	// Insert text literally instead of "as typed"
    पूर्णांक remcr)		// don't add CR characters
{
    दीर्घ	i;
    पूर्णांक		literally = literally_arg;

    जर (get_yank_register(regname, FALSE))
	literally = TRUE;
    जर (y_current->y_array == रिक्त)
	परत FAIL;

    साठी (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	जर ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	जर (got_int)
	    परत FAIL;
    }
    परत OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    शून्य
shift_delete_registers(शून्य)
{
    पूर्णांक		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    साठी (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    जर (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = रिक्त;		// set register one to empty
}

#जर defined(FEAT_EVAL)
    स्थिर शून्य
add_regtype_to_dict(पूर्णांक regname, dict_T *dict, char_u *buf, पूर्णांक bufsize)
{
    माप_प्रकार  buflen;
    दीर्घ    reglen;
    // Register type
    निवड (get_reg_type(regname, &reglen))
    {
	स्थिती MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    थांबा;
	स्थिती MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    थांबा;
	स्थिती MBLOCK:
	    buflen = vim_snprintf_safelen((अक्षर *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    थांबा;
	पूर्वनिर्धारित:
	    buf[0] = NUL;
	    buflen = 0;
	    थांबा;
    }
    (शून्य)dict_add_string_len(dict, "regtype", buf, (पूर्णांक)buflen);
}

    शून्य
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    स्थिर पूर्णांक	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    पूर्णांक		    n;
    char_u	    buf[NUMBUFLEN + 2];
    माप_प्रकार	    buflen;
    save_v_event_T  save_v_event;

    जर (recursive)
	परत;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    जर (list == रिक्त)
	परत;

    // yanked text contents
    साठी (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (पूर्णांक)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    जर (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (शून्य)dict_add_string_len(v_event, "regname", buf, (पूर्णांक)buflen);

    // motion type: inclusive or exclusive
    (शून्य)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (शून्य)dict_add_string_len(v_event, "operator", buf, (पूर्णांक)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, आकार(buf));

    // selection type - visual or not
    (शून्य)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_वस्तुसंच_ro(v_event);

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
	पूर्णांक	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *घाला,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	पूर्णांक	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    स्थिर bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    पूर्णांक		    n;
    char_u	    buf[NUMBUFLEN + 2];
    माप_प्रकार	    buflen;
    save_v_event_T  save_v_event;

    जर (recursive || regname == '_')
	परत;

    जर (regname != '.' && घाला == रिक्त
	    && reg == रिक्त)
	// Can happen when pasting text in normal mode in a terminal buffer
	परत;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    जर (list == रिक्त)
	परत;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    जर (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    जर (regname == '.')
    {
	जर (last_घाला_ga.ga_data != रिक्त)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_घाला_ga.ga_data,
		    (पूर्णांक)last_घाला_ga.ga_len);
    }
    अन्यथा जर (घाला != रिक्त)
    {
	list_append_string(list, घाला->string, (पूर्णांक)घाला->length);
    }
    अन्यथा
    {
	साठी (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (पूर्णांक)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    जर (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (शून्य)dict_add_string_len(v_event, "regname", buf, (पूर्णांक)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (शून्य)dict_add_string_len(v_event, "operator", buf, (पूर्णांक)buflen);

    add_regtype_to_dict(regname, v_event, buf, आकार(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (शून्य)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_वस्तुसंच_ro(v_event);

    recursive = true;
    textlock++;
    जर (post)
	apply_autocmds(EVENT_TEXTPUTPOST, रिक्त, रिक्त, FALSE, curbuf);
    अन्यथा
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
    पूर्णांक		i;

    साठी (i = 0; i < अंक_REGISTERS; ++i)
	y_regs[i].y_array = रिक्त;
}

#जर defined(EXITFREE)
    शून्य
clear_registers(शून्य)
{
    पूर्णांक		i;

    साठी (i = 0; i < अंक_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	जर (y_current->y_array != रिक्त)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    स्थिर शून्य
free_yank(दीर्घ n)
{
    जर (y_current->y_array == रिक्त)
	परत;

    दीर्घ	    i;

    साठी (i = n; --i >= 0; )
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
    पूर्णांक
op_yank(oparg_T *oap, पूर्णांक deleting, पूर्णांक mess)
{
    दीर्घ		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    पूर्णांक			yanktype = oap->motion_type;
    दीर्घ		yanklines = oap->ओळ_मोजणी;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    संरचना block_def	bd;

				    // check for read-only register
    जर (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	परत FAIL;
    }
    जर (oap->regname == '_')	    // black hole: nothing to do
	परत OK;

#ifdef FEAT_CLIPBOARD
    जर ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	संदेश_warn_missing_clipboard();
    }
#endif

    जर (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    जर (y_append && y_current->y_array != रिक्त)
	y_current = &newreg;
    अन्यथा
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    जर (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(आकार(string_T) * yanklines, TRUE);
    जर (y_current->y_array == रिक्त)
    {
	y_current = curr;
	परत FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    जर (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	जर (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    साठी ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	निवड (y_current->y_type)
	{
	    स्थिती MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		जर (yank_copy_ओळ(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    जा fail;
		थांबा;

	    स्थिती MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		जर ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == रिक्त)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    जा fail;
		}
		थांबा;

	    स्थिती MCHAR:
		{
		    पूर्णांक tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (पूर्णांक)STRLEN(bd.textstart);
		    जर (tmp < bd.textlen)
			bd.textlen = tmp;

		    जर (yank_copy_ओळ(&bd, y_idx, FALSE) == FAIL)
			जा fail;
		    थांबा;
		}
		// NOTREACHED
	}
    }

    जर (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	दीर्घ j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	जर (new_ptr == रिक्त)
	    जा fail;
	साठी (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	जर (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	जर (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == रिक्त)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    जर (pnew == रिक्त)
	    {
		y_idx = y_current->y_size - 1;
		जा fail;
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
	अन्यथा
	    y_idx = 0;
	पर्यंत (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    जर (mess)			// Display message about yank?
    {
	जर (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	जर (yanklines > p_report)
	{
	    अक्षर namebuf[100];

	    जर (oap->regname == NUL)
		*namebuf = NUL;
	    अन्यथा
		vim_snprintf(namebuf, आकार(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    जर (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    अन्यथा
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	जर (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	जर (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    जर (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	जर (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    जर (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	जर (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    जर (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	जर (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    जर (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# जर defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	जर (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    जर (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#जर defined(FEAT_EVAL)
    जर (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    परत OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    परत FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    स्थिर पूर्णांक
yank_copy_ओळ(संरचना block_def *bd, दीर्घ y_idx, पूर्णांक exclude_trailing_space)
{
    char_u	*pnew;

    जर (exclude_trailing_space)
	bd->endspaces = 0;
    जर ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == रिक्त)
	परत FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (माप_प्रकार)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (माप_प्रकार)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (माप_प्रकार)bd->endspaces);
    pnew += bd->endspaces;
    जर (exclude_trailing_space)
    {
	पूर्णांक s = bd->textlen + bd->endspaces;

	पर्यंत (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (माप_प्रकार)(pnew - y_current->y_array[y_idx].string);

    परत OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    स्थिर शून्य
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    दीर्घ	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    आकार(string_T) * y_current->y_size, TRUE);
    जर (y_current->y_array == रिक्त)
	y_current->y_size = 0;
    अन्यथा
	साठी (j = 0; j < y_current->y_size; ++j)
	{
	    जर ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == रिक्त)
	    {
		free_yank(j);
		y_current->y_size = 0;
		थांबा;
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
    पूर्णांक		regname,
    char_u	*अभिव्यक्ती_result,	// result for regname "=" when compiled
    पूर्णांक		dir,		// BACKWARD for 'P', FORWARD for 'p'
    दीर्घ	मोजणी,
    पूर्णांक		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    पूर्णांक		yanklen;
    पूर्णांक		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    दीर्घ	i;			// index in y_array[]
    पूर्णांक		y_type;
    दीर्घ	y_size;
    पूर्णांक		oldlen;
    दीर्घ	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = रिक्त;
    yankreg_T	*y_current_used = रिक्त;
    दीर्घ	nr_ओळी = 0;
    string_T	घाला_string;
    पूर्णांक		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    अचिन्हित पूर्णांक cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    जर (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(शून्य)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    जर (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    जर (regname == '.')
    {
	जर (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	जर (has_textput_events)
	    add_last_घाला++;
#endif

	(शून्य)stuff_inserted((dir == FORWARD ? (मोजणी == -1 ? 'o' : 'a') :
				    (मोजणी == -1 ? 'O' : 'i')), मोजणी, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	जर (has_textputpre())
	    put_do_autocmd('.', रिक्त, रिक्त, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	जर (has_textputpost())
	    put_do_autocmd('.', रिक्त, रिक्त, true, dir);

	जर (has_textput_events && --add_last_घाला == 0)
	    ga_clear(&last_घाला_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	जर ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	परत;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    घाला_string.string = रिक्त;
    जर (regname == '=' && अभिव्यक्ती_result != रिक्त)
	घाला_string.string = अभिव्यक्ती_result;
    अन्यथा जर (get_spec_reg(regname, &घाला_string.string, &allocated, TRUE)
		&& घाला_string.string == रिक्त)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	परत;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    जर (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	जा end;

    जर (घाला_string.string != रिक्त)
    {
	घाला_string.length = STRLEN(घाला_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	जर (regname == '=')
	{
	    माप_प्रकार  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    साठी (;;)
	    {
		y_size = 0;
		ptr = घाला_string.string;
		ptrlen = घाला_string.length;
		पर्यंत (ptr != रिक्त)
		{
		    जर (y_array != रिक्त)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    जर (tmp == रिक्त)
		    {
			जर (y_array != रिक्त)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    अन्यथा
		    {
			जर (y_array != रिक्त)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (माप_प्रकार)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			जर (*tmp == NUL)
			{
			    y_type = MLINE;
			    थांबा;
			}
		    }
		    ptr = tmp;
		}
		जर (y_array != रिक्त)
		    थांबा;
		y_array = ALLOC_MULT(string_T, y_size);
		जर (y_array == रिक्त)
		    जा end;
	    }
	}
	अन्यथा
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &घाला_string;
	}
#ifdef FEAT_EVAL
	जर (has_textputpre())
	    put_do_autocmd(regname, रिक्त, &घाला_string, false, dir);
#endif
    }
    अन्यथा
    {
#ifdef FEAT_EVAL
	जर (has_textputpre())
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

    जर (y_type == MLINE)
    {
	जर (flags & PUT_ओळ_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    माप_प्रकार plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    जर (u_save_cursor() == FAIL)
		जा end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    जर (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (माप_प्रकार)(p - p_orig));
	    जर (ptr == रिक्त)
		जा end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    जर (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (माप_प्रकार)(p - oldp));
	    जर (ptr == रिक्त)
		जा end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_ओळी;
	    dir = FORWARD;
	}
	जर (flags & PUT_ओळ_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    जर (flags & PUT_ओळ)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    जर (y_size == 0 || y_array == रिक्त)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	जा end;
    }

    जर (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	जर (lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	    lnum = curbuf->b_ml.ml_ओळ_मोजणी + 1;
	जर (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    जा end;
    }
    अन्यथा जर (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	जर (dir == BACKWARD)
	    (शून्य)hasFolding(lnum, &lnum, रिक्त);
	अन्यथा
	    (शून्य)hasFolding(lnum, रिक्त, &lnum);
#endif
	जर (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	जर ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    जा end;
#ifdef FEAT_FOLDING
	जर (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	अन्यथा
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    अन्यथा जर (u_save_cursor() == FAIL)
	जा end;

    जर (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	जर (gchar_cursor() == TAB)
	{
	    पूर्णांक viscol = getviscol();
	    पूर्णांक ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    जर (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#अन्यथा
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    अन्यथा
		curwin->w_cursor.coladd = 0;
	}
	अन्यथा जर (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    जर (y_type == MBLOCK)
    {
	पूर्णांक	delcount;
	पूर्णांक	incr = 0;
	संरचना block_def bd;
	दीर्घ	j;
	पूर्णांक	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	जर (dir == FORWARD && c != NUL)
	{
	    जर (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, रिक्त, &endcol2, 0);
	    अन्यथा
		getvcol(curwin, &curwin->w_cursor, रिक्त, रिक्त, &col, 0);

	    जर (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    अन्यथा
		जर (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	अन्यथा
	    getvcol(curwin, &curwin->w_cursor, &col, रिक्त, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	जर (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    जर (dir == FORWARD && c == NUL)
		++col;
	    जर (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    जर (c == TAB)
	    {
		जर (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		जर (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	साठी (i = 0; i < y_size; ++i)
	{
	    पूर्णांक		    spaces = 0;
	    अक्षर	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    जर (curwin->w_cursor.lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	    {
		जर (ml_append(curbuf->b_ml.ml_ओळ_मोजणी, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    थांबा;
		++nr_ओळी;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    पर्यंत (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    जर (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    अन्यथा जर (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		जर (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		जर (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (पूर्णांक)y_array[i].length;

	    जर ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		पर्यंत (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		जर (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    जर (yanklen + spaces != 0
		     && मोजणी > ((पूर्णांक_उच्च - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		थांबा;
	    }

	    totlen = मोजणी * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    जर (newp == रिक्त)
		थांबा;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (माप_प्रकार)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (माप_प्रकार)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    साठी (j = 0; j < मोजणी; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (माप_प्रकार)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		जर ((j < मोजणी - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (माप_प्रकार)spaces);
		    ptr += spaces;
		}
		अन्यथा
		    totlen -= spaces;  // didn't use these spaces
	    }

	    // may insert some spaces after the new text
	    vim_memset(ptr, ' ', (माप_प्रकार)bd.endspaces);
	    ptr += bd.endspaces;

	    // move the text after the cursor to the end of the line.
	    mch_memmove(ptr, oldp + bd.textcol + delcount,
				(माप_प्रकार)(oldlen - bd.textcol - delcount + 1));
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);

	    ++curwin->w_cursor.lnum;
	    जर (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_ओळी(lnum, 0, curwin->w_cursor.lnum - nr_ओळी, nr_ओळी);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	जर (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	जर (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    जर (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	अन्यथा
	    curwin->w_cursor.lnum = lnum;
    }
    अन्यथा
    {
	pos_T	new_cursor;

	yanklen = (पूर्णांक)y_array[0].length;

	// Character or Line mode
	जर (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    जर (dir == FORWARD && gchar_cursor() != NUL)
	    {
		जर (has_mbyte)
		{
		    पूर्णांक bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    जर (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		अन्यथा
		{
		    ++col;
		    जर (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	अन्यथा जर (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	जर (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    पूर्णांक		first_byte_off = 0;

	    जर (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		जर (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		जर (end_lnum > start_lnum)
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

	    जर (मोजणी == 0 || yanklen == 0)
	    {
		जर (VIsual_active)
		    lnum = end_lnum;
	    }
	    अन्यथा जर (मोजणी > पूर्णांक_उच्च / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    अन्यथा
	    {
		totlen = मोजणी * yanklen;
		करा
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    जर (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			जर (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			अन्यथा
			    col = MAXCOL;
		    }
		    जर (VIsual_active && col > oldlen)
		    {
			lnum++;
			सुरूठेवा;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    जर (newp == रिक्त)
			जा end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (माप_प्रकार)col);
		    ptr = newp + col;
		    साठी (i = 0; i < मोजणी; ++i)
		    {
			mch_memmove(ptr, y_array[0].string, (माप_प्रकार)yanklen);
			ptr += yanklen;
		    }
		    mch_memmove(ptr, oldp + col, (माप_प्रकार)(oldlen - col) + 1);	    // +1 for NUL

		    // compute the byte offset for the last character
		    first_byte_off = mb_head_off(newp, ptr - 1);

		    // Note: this may free "newp"
		    ml_replace(lnum, newp, FALSE);

		    inserted_bytes(lnum, col, totlen);

		    // Place cursor on last putted char.
		    जर (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    जर (VIsual_active)
			lnum++;
		} पर्यंत (VIsual_active && lnum <= end_lnum);

		जर (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    जर (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    अन्यथा
		curwin->w_cursor.col -= first_byte_off;
	}
	अन्यथा
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    पूर्णांक		indent;
	    पूर्णांक		orig_indent = 0;
	    पूर्णांक		indent_diff = 0;	// init for gcc
	    पूर्णांक		first_indent = TRUE;
	    पूर्णांक		lendiff = 0;
	    दीर्घ	cnt;

	    जर (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    साठी (cnt = 1; cnt <= मोजणी; ++cnt)
	    {
		i = 0;
		जर (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (पूर्णांक)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    जर (newp == रिक्त)
			जा error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    जर (newp == रिक्त)
			जा error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (माप_प्रकार)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (माप_प्रकार)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		साठी (; i < y_size; ++i)
		{
		    जर (y_type != MCHAR || i < y_size - 1)
		    {
			जर (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    जा error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_ओळी;
		    जर (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			जर (cnt == मोजणी && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			जर (*ptr == '#' && preprocs_डावा())
			    indent = 0;     // Leave # lines at start
			अन्यथा जर (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			अन्यथा जर (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			अन्यथा जर ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(शून्य)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			जर (cnt == मोजणी && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		जर (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    जर (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		जर (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_ओळी, 0L);

	    // note changed text for displaying and folding
	    जर (y_type == MCHAR)
		changed_ओळी(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_ओळी);
	    अन्यथा
		changed_ओळी(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_ओळी);
	    जर (y_current_used != रिक्त && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		जा end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    जर (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		जर (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    अन्यथा
		curbuf->b_op_end.col = 0;

	    जर (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    अन्यथा जर (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		जर (y_type == MLINE)
		{
		    जर (lnum >= curbuf->b_ml.ml_ओळ_मोजणी)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
		    अन्यथा
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		अन्यथा
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    जर (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    अन्यथा जर (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		जर (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    अन्यथा	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_ओळी);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    पूर्णांक len = ml_get_curline_len();
    जर (curwin->w_cursor.col > len)
    {
	जर (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    जर (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    जर (has_textputpost())
    {
	जर (घाला_string.string == रिक्त)
	    put_do_autocmd(regname, y_current, रिक्त, true, dir);
	अन्यथा
	    put_do_autocmd(regname, रिक्त, &घाला_string, true, dir);
    }
#endif

    जर (allocated)
	vim_free(घाला_string.string);
    जर (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    पूर्णांक
get_register_name(पूर्णांक अंक)
{
    जर (अंक == -1)
	परत '"';
    अन्यथा जर (अंक < 10)
	परत अंक + '0';
    अन्यथा जर (अंक == DELETION_REGISTER)
	परत '-';
#जर defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    अन्यथा जर (अंक == STAR_REGISTER)
	परत '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    अन्यथा जर (clipmethod == CLIPMETHOD_PROVIDER && अंक == REAL_PLUS_REGISTER)
	परत '+';
    अन्यथा जर (clipmethod != CLIPMETHOD_PROVIDER && अंक == PLUS_REGISTER)
	परत '+';
#endif
    अन्यथा
	परत अंक + 'a' - 10;
}

#जर defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    पूर्णांक
get_unname_register(शून्य)
{
    परत y_previous == रिक्त ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    शून्य
ex_display(exarg_T *eap)
{
    पूर्णांक		i, n;
    दीर्घ	j;
    char_u	*p;
    yankreg_T	*yb;
    पूर्णांक		name;
    पूर्णांक		attr;
    char_u	*arg = eap->arg;
    पूर्णांक		clen;
    पूर्णांक		type;
    string_T	घाला;

    silence_w23_w24_संदेश++;
    जर (arg != रिक्त && *arg == NUL)
	arg = रिक्त;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    संदेश_puts_title(_("\nType Name Content"));
    साठी (i = -1; i < अंक_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	निवड (get_reg_type(name, रिक्त))
	{
	    स्थिती MLINE: type = 'l'; थांबा;
	    स्थिती MCHAR: type = 'c'; थांबा;
	    पूर्वनिर्धारित:	type = 'b'; थांबा;
	}
	जर (arg != रिक्त && vim_strchr(arg, name) == रिक्त
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == रिक्त)
#endif
		)
	    सुरूठेवा;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	जर (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (शून्य)may_get_selection(name);
#endif

	जर (i == -1)
	{
	    जर (y_previous != रिक्त)
		yb = y_previous;
	    अन्यथा
		yb = &(y_regs[0]);
	}
	अन्यथा
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	जर (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != रिक्त &&
		    (yb == y_previous || yb == &y_regs[0])))
	    सुरूठेवा;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	जर (yb->y_array != रिक्त)
	{
	    पूर्णांक do_show = FALSE;

	    साठी (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    जर (do_show || yb->y_size == 0)
	    {
		संदेश_putchar('\n');
		संदेश_puts("  ");
		संदेश_putchar(type);
		संदेश_puts("  ");
		संदेश_putchar('"');
		संदेश_putchar(name);
		संदेश_puts("   ");

		n = (पूर्णांक)Columns - 11;
		साठी (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    जर (j)
		    {
			संदेश_puts_attr("^J", attr);
			n -= 2;
		    }
		    साठी (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			संदेश_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		जर (n > 1 && yb->y_type == MLINE)
		    संदेश_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    घाला = get_last_घाला();
    जर ((p = घाला.string) != रिक्त
		  && (arg == रिक्त || vim_strchr(arg, '.') != रिक्त) && !got_int
						      && !message_filtered(p))
    {
	संदेश_puts("\n  c  \".   ");
	dis_संदेश(p, TRUE);
    }

    // display last command line
    जर (last_cmdline != रिक्त && (arg == रिक्त || vim_strchr(arg, ':') != रिक्त)
			       && !got_int && !message_filtered(last_cmdline))
    {
	संदेश_puts("\n  c  \":   ");
	dis_संदेश(last_cmdline, FALSE);
    }

    // display current file name
    जर (curbuf->b_fname != रिक्त
	    && (arg == रिक्त || vim_strchr(arg, '%') != रिक्त) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	संदेश_puts("\n  c  \"%   ");
	dis_संदेश(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    जर ((arg == रिक्त || vim_strchr(arg, '#') != रिक्त) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	जर (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    संदेश_puts("\n  c  \"#   ");
	    dis_संदेश(fname, FALSE);
	}
    }

    // display last search pattern
    जर (last_शोध_pat() != रिक्त
		 && (arg == रिक्त || vim_strchr(arg, '/') != रिक्त) && !got_int
				      && !message_filtered(last_शोध_pat()))
    {
	संदेश_puts("\n  c  \"/   ");
	dis_संदेश(last_शोध_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    जर (अभिव्यक्ती_ओळ != रिक्त && (arg == रिक्त || vim_strchr(arg, '=') != रिक्त)
				  && !got_int && !message_filtered(अभिव्यक्ती_ओळ))
    {
	संदेश_puts("\n  c  \"=   ");
	dis_संदेश(अभिव्यक्ती_ओळ, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_संदेश--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    स्थिर शून्य
dis_संदेश(
    char_u	*p,
    पूर्णांक		skip_esc)	    // if TRUE, ignore trailing ESC
{
    पूर्णांक		n;
    पूर्णांक		l;

    n = (पूर्णांक)Columns - 6;
    पर्यंत (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	जर (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    संदेश_outtrans_len(p, l);
	    p += l;
	}
	अन्यथा
	    संदेश_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#जर defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    शून्य
dnd_yank_drag_data(char_u *str, दीर्घ len)
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
get_reg_type(पूर्णांक regname, दीर्घ *reglen)
{
    निवड (regname)
    {
	स्थिती '%':		// file name
	स्थिती '#':		// alternate file name
	स्थिती '=':		// expression
	स्थिती ':':		// last command line
	स्थिती '/':		// last search-pattern
	स्थिती '.':		// last inserted text
	स्थिती Ctrl_F:		// Filename under cursor
	स्थिती Ctrl_P:		// Path under cursor, expand via "path"
	स्थिती Ctrl_W:		// word under cursor
	स्थिती Ctrl_A:		// WORD (mnemonic All) under cursor
	स्थिती '_':		// black hole: always empty
	    परत MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    जर (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    जर (regname != NUL && !valid_yank_reg(regname, FALSE))
	परत MAUTO;

    get_yank_register(regname, FALSE);

    जर (y_current->y_array != रिक्त)
    {
	जर (reglen != रिक्त && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	परत y_current->y_type;
    }
    परत MAUTO;
}

#जर defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    स्थिर char_u *
getreg_wrap_one_ओळ(char_u *s, पूर्णांक flags)
{
    जर (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	जर (list != रिक्त)
	{
	    जर (list_append_string(list, रिक्त, -1) == FAIL)
	    {
		list_free(list);
		परत रिक्त;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	परत (char_u *)list;
    }
    परत s;
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
get_reg_contents(पूर्णांक regname, पूर्णांक flags)
{
    linenr_T	i;
    char_u	*retval;
    पूर्णांक		allocated;
    दीर्घ	len;

    // Don't allow using an expression register inside an expression
    जर (regname == '=')
    {
	जर (flags & GREG_NO_अभिव्यक्ती)
	    परत रिक्त;
	जर (flags & GREG_अभिव्यक्ती_SRC)
	    परत getreg_wrap_one_ओळ(get_अभिव्यक्ती_ओळ_src(), flags);
	परत getreg_wrap_one_ओळ(get_अभिव्यक्ती_ओळ(), flags);
    }

    जर (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    जर (regname != NUL && !valid_yank_reg(regname, FALSE))
	परत रिक्त;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    जर (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    जर (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	जर (retval == रिक्त)
	    परत रिक्त;
	जर (allocated)
	    परत getreg_wrap_one_ओळ(retval, flags);
	परत getreg_wrap_one_ओळ(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    जर (y_current->y_array == रिक्त)
	परत रिक्त;

    जर (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	जर (list == रिक्त)
	    परत रिक्त;
	साठी (i = 0; i < y_current->y_size; ++i)
	{
	    जर (list_append_string(list, y_current->y_array[i].string,
		(पूर्णांक)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		परत रिक्त;
	    }
	}
	परत (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    साठी (i = 0; i < y_current->y_size; ++i)
    {
	len += (दीर्घ)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	जर (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    जर (retval == रिक्त)
	परत रिक्त;

    // Copy the lines of the yank register into the string.
    len = 0;
    साठी (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (दीर्घ)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	जर (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    परत retval;
}

    स्थिर पूर्णांक
init_write_reg(
    पूर्णांक		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    पूर्णांक		must_append,
    पूर्णांक		*yank_type UNUSED)
{
    जर (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	परत FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    जर (!y_append && !must_append)
	free_yank_all();
    परत OK;
}

    स्थिर शून्य
finish_write_reg(
    पूर्णांक		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    जर (name != '"')
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
    पूर्णांक		name,
    char_u	*str,
    पूर्णांक		maxlen,
    पूर्णांक		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    शून्य
write_reg_contents_lst(
    पूर्णांक		name,
    char_u	**strings,
    पूर्णांक		maxlen UNUSED,
    पूर्णांक		must_append,
    पूर्णांक		yank_type,
    दीर्घ	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    जर (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	जर (strings[0] == रिक्त)
	    s = (char_u *)"";
	अन्यथा जर (strings[1] != रिक्त)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_ओळी), name);
	    परत;
	}
	अन्यथा
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	परत;
    }

    जर (name == '_')	    // black hole: nothing to do
	परत;

    जर (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	परत;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    शून्य
write_reg_contents_ex(
    पूर्णांक		name,
    char_u	*str,
    पूर्णांक		maxlen,
    पूर्णांक		must_append,
    पूर्णांक		yank_type,
    दीर्घ	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    दीर्घ	len;

    जर (maxlen >= 0)
	len = maxlen;
    अन्यथा
	len = (दीर्घ)STRLEN(str);

    // Special case: '/' search pattern
    जर (name == '/')
    {
	set_last_शोध_pat(str, RE_शोध, TRUE, TRUE);
	परत;
    }

    जर (name == '#')
    {
	जर (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  परत;
	}

	buf_T	*buf;

	जर (VIM_ISDIGIT(*str))
	{
	    पूर्णांक	अंक = म_से_प((अक्षर *)str);

	    buf = buflist_findnr(अंक);
	    जर (buf == रिक्त)
		semsg(_(e_buffer_nr_does_not_exist), (दीर्घ)अंक);
	}
	अन्यथा
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	जर (buf == रिक्त)
	    परत;
	curwin->w_alt_fnum = buf->b_fnum;
	परत;
    }

    जर (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (माप_प्रकार)len);
	जर (p == रिक्त)
	    परत;
	जर (must_append && अभिव्यक्ती_ओळ != रिक्त)
	{
	    s = concat_str(अभिव्यक्ती_ओळ, p);
	    vim_free(p);
	    p = s;
	}
	set_अभिव्यक्ती_ओळ(p, रिक्त);
	परत;
    }

    जर (name == '_')	    // black hole: nothing to do
	परत;

    जर (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	परत;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#जर defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    शून्य
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    पूर्णांक		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    दीर्घ	len,		// length of string
    दीर्घ	blocklen,	// width of Visual block
    पूर्णांक		str_list)	// TRUE if str is char_u **
{
    पूर्णांक		type;			// MCHAR, MLINE or MBLOCK
    पूर्णांक		lnum;
    दीर्घ	start;
    दीर्घ	i;
    पूर्णांक		extra;
    पूर्णांक		newlines;		// number of lines added
    पूर्णांक		extraline = 0;		// extra line at the end
    पूर्णांक		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    दीर्घ	maxlen;

    जर (y_ptr->y_array == रिक्त)		// NULL means empty register
	y_ptr->y_size = 0;

    जर (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    अन्यथा
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    जर (str_list)
    {
	साठी (ss = (char_u **) str; *ss != रिक्त; ++ss)
	    ++newlines;
    }
    अन्यथा
    {
	साठी (i = 0; i < len; i++)
	    जर (str[i] == '\n')
		++newlines;
	जर (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	जर (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    जर (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	परत;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * आकार(string_T), TRUE);
    जर (pp == रिक्त)	// out of memory
	परत;
    साठी (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    जर (str_list)
    {
	साठी (ss = (char_u **) str; *ss != रिक्त; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    जर (type == MBLOCK)
	    {
		पूर्णांक charlen = vim_strsize(*ss);

		जर (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    अन्यथा
    {
	साठी (start = 0; start < len + extraline; start += i + 1)
	{
	    पूर्णांक charlen = 0;

	    साठी (i = start; i < len;)	// find the end of the line
	    {
		जर (str[i] == '\n')
		    थांबा;
		जर (type == MBLOCK)
		{
		    जर (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    अन्यथा
			charlen += byte2cells(str[i]);
		}

		जर (str[i] == NUL)
		    i++; // registers can have NUL chars
		अन्यथा
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    जर (charlen > maxlen)
		maxlen = charlen;
	    जर (append)
	    {
		--lnum;
		extra = (पूर्णांक)y_ptr->y_array[lnum].length;
	    }
	    अन्यथा
		extra = 0;
	    s = alloc(i + extra + 1);
	    जर (s == रिक्त)
		थांबा;
	    जर (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (माप_प्रकार)extra);
	    जर (append)
		vim_free(y_ptr->y_array[lnum].string);
	    जर (i > 0)
		mch_memmove(s + extra, str + start, (माप_प्रकार)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    पर्यंत (--extra >= 0)
	    {
		जर (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    जर (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    अन्यथा
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
