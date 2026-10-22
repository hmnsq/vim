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

#ضمن "vim.h"

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
ساكن yankreg_T	y_regs[NUM_REGISTERS];

ساكن yankreg_T	*y_current;	    // ptr to current yankreg
ساكن صحيح		y_append;	    // TRUE when appending
ساكن yankreg_T	*y_previous = عدم; // ptr to last written yankreg

ساكن صحيح	stuff_yank(صحيح, char_u *);
ساكن فراغ	put_reedit_in_typebuf(صحيح silent);
ساكن صحيح	put_in_typebuf(char_u *s, صحيح esc, صحيح colon, صحيح silent);
ساكن صحيح	yank_copy_line(بنية block_def *bd, long y_idx, صحيح exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
ساكن فراغ	copy_yank_reg(yankreg_T *reg);
#endif
ساكن فراغ	dis_msg(char_u *p, صحيح skip_esc);

#ܐܢ defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(فراغ)
{
    ܦܢܐ y_regs;
}
#endif

#ܐܢ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(صحيح reg)
{
    ܦܢܐ &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(فراغ)
{
    ܦܢܐ y_current;
}

    yankreg_T *
get_y_previous(فراغ)
{
    ܦܢܐ y_previous;
}

    فراغ
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    فراغ
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    فراغ
reset_y_append(فراغ)
{
    y_append = FALSE;
}


#ܐܢ defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
ساكن char_u	*expr_line = عدم;
ساكن exarg_T	*expr_eap = عدم;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    صحيح
get_expr_register(فراغ)
{
    char_u	*new_line;

    new_line = getcmdline('=', 0L, 0, 0);
    ܐܢ (new_line == عدم)
	ܦܢܐ NUL;
    ܐܢ (*new_line == NUL)	// use previous line
	vim_free(new_line);
    ܐܚܪܢܐ
	set_expr_line(new_line, عدم);
    ܦܢܐ '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    فراغ
set_expr_line(char_u *new_line, exarg_T *eap)
{
    vim_free(expr_line);
    expr_line = new_line;
    expr_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_expr_line(فراغ)
{
    char_u	*expr_copy;
    char_u	*rv;
    ساكن صحيح	nested = 0;

    ܐܢ (expr_line == عدم)
	ܦܢܐ عدم;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    expr_copy = vim_strsave(expr_line);
    ܐܢ (expr_copy == عدم)
	ܦܢܐ عدم;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    ܐܢ (nested >= 10)
	ܦܢܐ expr_copy;

    ++nested;
    rv = eval_to_string_eap(expr_copy, TRUE, expr_eap, FALSE);
    --nested;
    vim_free(expr_copy);
    ܦܢܐ rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ساكن char_u *
get_expr_line_src(فراغ)
{
    ܐܢ (expr_line == عدم)
	ܦܢܐ عدم;
    ܦܢܐ vim_strsave(expr_line);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    صحيح
valid_yank_reg(
    صحيح	    regname,
    صحيح	    writing)	    // if TRUE check for writable registers
{
    ܐܢ (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#ܐܚܪܢܐ
				    "/#.%:"
#endif
					, regname) != عدم)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#ܐܢ defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#وإلاإذا defined(FEAT_CLIPBOARD_PROVIDER)
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
	ܦܢܐ TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    ܐܚܪܢܐ ܐܢ (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	msg_warn_missing_clipboard();
	ܦܢܐ FALSE;
    }
#endif
    ܦܢܐ FALSE;
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
    صحيح
get_yank_register(صحيح regname, صحيح writing)
{
    صحيح	    i;
    صحيح	    ret = FALSE;

    y_append = FALSE;
    ܐܢ ((regname == 0 || regname == '"') && !writing && y_previous != عدم)
    {
	y_current = y_previous;
	ܦܢܐ ret;
    }
    i = regname;
    ܐܢ (VIM_ISDIGIT(i))
	i -= '0';
    ܐܚܪܢܐ ܐܢ (ASCII_ISLOWER(i))
	i = CharOrdLow(i) + 10;
    ܐܚܪܢܐ ܐܢ (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    ܐܚܪܢܐ ܐܢ (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    ܐܚܪܢܐ ܐܢ (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    ܐܚܪܢܐ ܐܢ (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	ܐܢ (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	ܐܚܪܢܐ
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#وإلاإذا defined(FEAT_CLIPBOARD_PROVIDER)
    ܐܚܪܢܐ ܐܢ (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    ܐܚܪܢܐ ܐܢ (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    ܐܚܪܢܐ ܐܢ (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    ܐܚܪܢܐ		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    ܐܢ (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    ܦܢܐ ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    فراغ *
get_register(
    صحيح		name,
    صحيح		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    صحيح		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    ܐܢ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	ܐܢ (name == '*' && clip_star.available)
	{
	    ܐܢ (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	ܐܢ (name == '+' && clip_plus.available)
	{
	    ܐܢ (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    ܐܢ (reg == عدم)
	ܦܢܐ (فراغ *)عدم;

    *reg = *y_current;
    ܐܢ (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	ܐܢ (reg->y_size == 0 || y_current->y_array == عدم)
	    reg->y_array = عدم;
	ܐܚܪܢܐ
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	ܐܢ (reg->y_array != عدم)
	{
	    لكل (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		ܐܢ (reg->y_array[i].string == عدم)
		{
		    // The allocation failed so clean up and exit
		    ܟܡܐ (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    ܦܢܐ (فراغ *)عدم;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    ܐܚܪܢܐ
	y_current->y_array = عدم;
    ܦܢܐ (فراغ *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    فراغ
put_register(صحيح name, فراغ *reg)
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

#ܐܢ defined(FEAT_CLIPBOARD)
    فراغ
free_register(فراغ *reg)
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
    صحيح
yank_register_mline(صحيح regname)
{
    ܐܢ (regname != 0 && !valid_yank_reg(regname, FALSE))
	ܦܢܐ FALSE;
    ܐܢ (regname == '_')		// black hole is always empty
	ܦܢܐ FALSE;
    get_yank_register(regname, FALSE);
    ܦܢܐ (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
do_record(صحيح c)
{
    char_u	    *p;
    ساكن صحيح	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    صحيح		    retval;

    ܐܢ (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	ܐܢ (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	ܐܚܪܢܐ
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    ܐܚܪܢܐ			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	msg("");
	p = get_recorded();
	ܐܢ (p == عدم)
	    retval = FAIL;
	ܐܚܪܢܐ
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
    ܦܢܐ retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    ساكن صحيح
stuff_yank(صحيح regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    ܐܢ (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	ܦܢܐ FAIL;
    }
    ܐܢ (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	ܦܢܐ OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    ܐܢ (y_append && y_current->y_array != عدم)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	ܐܢ (tmp == عدم)
	{
	    vim_free(p);
	    ܦܢܐ FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    ܐܚܪܢܐ
    {
	free_yank_all();
	ܐܢ ((y_current->y_array = ALLOC_ONE(string_T)) == عدم)
	{
	    vim_free(p);
	    ܦܢܐ FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    ܦܢܐ OK;
}

/*
 * Last executed register (@ command)
 */
ساكن صحيح execreg_lastc = NUL;

#ܐܢ defined(FEAT_VIMINFO)
    صحيح
get_execreg_lastc(فراغ)
{
    ܦܢܐ execreg_lastc;
}

    فراغ
set_execreg_lastc(صحيح lastc)
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
    ساكن char_u *
execreg_line_continuation(string_T *lines, long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    صحيح		j;
    char_u	*str;

    ga_init2(&ga, حجم(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    ܟܡܐ (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(lines[cmd_start].string);
	ܐܢ (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    اكسر;
    }

    // join all the lines
    tmp = &lines[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    لكل (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &lines[j];
	p = skipwhite(tmp->string);
	ܐܢ (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    ܐܢ (ga.ga_len > 400)
	    {
		ܐܢ (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		ܐܚܪܢܐ
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
    ܦܢܐ str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
do_execreg(
    صحيح	    regname,
    صحيح	    colon,		// insert ':' before each line
    صحيح	    addcr,		// always add '\n' to end of line
    صحيح	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    صحيح		retval = OK;
    صحيح		remap;

    // repeat previous one
    ܐܢ (regname == '@')
    {
	ܐܢ (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    ܦܢܐ FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    ܐܢ (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	ܦܢܐ FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ܐܢ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    ܐܢ (regname == '_')
	ܦܢܐ OK;

    // use last command line
    ܐܢ (regname == ':')
    {
	ܐܢ (last_cmdline == عدم)
	{
	    emsg(_(e_no_previous_command_line));
	    ܦܢܐ FAIL;
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
	ܐܢ (p != عدم)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    ܐܢ (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    ܐܚܪܢܐ
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    ܐܚܪܢܐ ܐܢ (regname == '=')
    {
	p = get_expr_line();
	ܐܢ (p == عدم)
	    ܦܢܐ FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    ܐܚܪܢܐ ܐܢ (regname == '.')		// use last inserted text
    {
	p = get_last_insert_save();
	ܐܢ (p == عدم)
	{
	    emsg(_(e_no_inserted_text_yet));
	    ܦܢܐ FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    ܐܚܪܢܐ
    {
	get_yank_register(regname, FALSE);
	ܐܢ (y_current->y_array == عدم)
	    ܦܢܐ FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	لكل (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    صحيح	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    ܐܢ (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		ܐܢ (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    ܦܢܐ FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    ܐܢ (colon && i > 0)
	    {
		p = skipwhite(str);
		ܐܢ (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_line_continuation(y_current->y_array, &i);
		    ܐܢ (str == عدم)
			ܦܢܐ FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    ܐܢ (free_str)
		vim_free(str);
	    ܐܢ (escaped == عدم)
		ܦܢܐ FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    ܐܢ (retval == FAIL)
		ܦܢܐ FAIL;
	    ܐܢ (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		ܦܢܐ FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    ܦܢܐ retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ساكن فراغ
put_reedit_in_typebuf(صحيح silent)
{
    ܐܢ (restart_edit == NUL)
	ܦܢܐ;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    ܐܢ (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    ܐܚܪܢܐ ܐܢ (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    ܐܚܪܢܐ ܐܢ (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    ܐܢ (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    ساكن صحيح
put_in_typebuf(
    char_u	*s,
    صحيح		esc,
    صحيح		colon,	    // add ':' before the line
    صحيح		silent)
{
    صحيح		retval = OK;

    put_reedit_in_typebuf(silent);
    ܐܢ (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    ܐܢ (retval == OK)
    {
	char_u	*p;

	ܐܢ (esc)
	    p = vim_strsave_escape_csi(s);
	ܐܚܪܢܐ
	    p = s;
	ܐܢ (p == عدم)
	    retval = FAIL;
	ܐܚܪܢܐ
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	ܐܢ (esc)
	    vim_free(p);
    }
    ܐܢ (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    ܦܢܐ retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
insert_reg(
    صحيح		regname,
    صحيح		literally_arg)	// insert literally, not as if typed
{
    long	i;
    صحيح		retval = OK;
    char_u	*arg;
    صحيح		allocated;
    صحيح		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    ܐܢ (got_int)
	ܦܢܐ FAIL;

    // check for valid regname
    ܐܢ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ܦܢܐ FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ܐܢ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    ܐܢ (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    ܐܚܪܢܐ ܐܢ (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	ܐܢ (arg == عدم)
	    ܦܢܐ FAIL;
	stuffescaped(arg, literally);
	ܐܢ (allocated)
	    vim_free(arg);
    }
    ܐܚܪܢܐ				// name or number register
    {
	ܐܢ (get_yank_register(regname, FALSE))
	    literally = TRUE;
	ܐܢ (y_current->y_array == عدم)
	    retval = FAIL;
	ܐܚܪܢܐ
	{
	    لكل (i = 0; i < y_current->y_size; ++i)
	    {
		ܐܢ (regname == '-' && y_current->y_type == MCHAR)
		{
		    صحيح dir = BACKWARD;
		    ܐܢ ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			ܐܢ (u_save_cursor() == FAIL)
			    ܦܢܐ FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			ܐܢ (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, عدم, dir, 1L, PUT_CURSEND);
		}
		ܐܚܪܢܐ
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    ܐܢ (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    ܦܢܐ retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    صحيح
get_spec_reg(
    صحيح		regname,
    char_u	**argp,
    صحيح		*allocated,	// return: TRUE when value was allocated
    صحيح		errmsg)		// give error message when failing
{
    صحيح		cnt;

    *argp = عدم;
    *allocated = FALSE;
    بدل (regname)
    {
	حالة '%':		// file name
	    ܐܢ (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    ܦܢܐ TRUE;

	حالة '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    ܦܢܐ TRUE;

#ifdef FEAT_EVAL
	حالة '=':		// result of expression
	    *argp = get_expr_line();
	    *allocated = TRUE;
	    ܦܢܐ TRUE;
#endif

	حالة ':':		// last command line
	    ܐܢ (last_cmdline == عدم && errmsg)
		emsg(_(e_no_previous_command_line));
	    *argp = last_cmdline;
	    ܦܢܐ TRUE;

	حالة '/':		// last search-pattern
	    ܐܢ (last_search_pat() == عدم && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_search_pat();
	    ܦܢܐ TRUE;

	حالة '.':		// last inserted text
	    *argp = get_last_insert_save();
	    *allocated = TRUE;
	    ܐܢ (*argp == عدم && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    ܦܢܐ TRUE;

	حالة Ctrl_F:		// Filename under cursor
	حالة Ctrl_P:		// Path under cursor, expand via "path"
	    ܐܢ (!errmsg)
		ܦܢܐ FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, عدم);
	    *allocated = TRUE;
	    ܦܢܐ TRUE;

	حالة Ctrl_W:		// word under cursor
	حالة Ctrl_A:		// WORD (mnemonic All) under cursor
	    ܐܢ (!errmsg)
		ܦܢܐ FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : عدم;
	    *allocated = TRUE;
	    ܦܢܐ TRUE;

	حالة Ctrl_L:		// Line under cursor
	    ܐܢ (!errmsg)
		ܦܢܐ FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    ܦܢܐ TRUE;

	حالة '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    ܦܢܐ TRUE;
    }

    ܦܢܐ FALSE;
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
    صحيح
cmdline_paste_reg(
    صحيح regname,
    صحيح literally_arg,	// Insert text literally instead of "as typed"
    صحيح remcr)		// don't add CR characters
{
    long	i;
    صحيح		literally = literally_arg;

    ܐܢ (get_yank_register(regname, FALSE))
	literally = TRUE;
    ܐܢ (y_current->y_array == عدم)
	ܦܢܐ FAIL;

    لكل (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	ܐܢ ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	ܐܢ (got_int)
	    ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    فراغ
shift_delete_registers(فراغ)
{
    صحيح		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    لكل (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    ܐܢ (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = عدم;		// set register one to empty
}

#ܐܢ defined(FEAT_EVAL)
    ساكن فراغ
add_regtype_to_dict(صحيح regname, dict_T *dict, char_u *buf, صحيح bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    بدل (get_reg_type(regname, &reglen))
    {
	حالة MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    اكسر;
	حالة MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    اكسر;
	حالة MBLOCK:
	    buflen = vim_snprintf_safelen((حرف *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    اكسر;
	افتراضي:
	    buf[0] = NUL;
	    buflen = 0;
	    اكسر;
    }
    (فراغ)dict_add_string_len(dict, "regtype", buf, (صحيح)buflen);
}

    فراغ
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    ساكن صحيح	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    صحيح		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ܐܢ (recursive)
	ܦܢܐ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ܐܢ (list == عدم)
	ܦܢܐ;

    // yanked text contents
    لكل (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (صحيح)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    ܐܢ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (فراغ)dict_add_string_len(v_event, "regname", buf, (صحيح)buflen);

    // motion type: inclusive or exclusive
    (فراغ)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (فراغ)dict_add_string_len(v_event, "operator", buf, (صحيح)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, حجم(buf));

    // selection type - visual or not
    (فراغ)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_items_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, عدم, عدم, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    ساكن فراغ
put_do_autocmd(
	صحيح	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *insert,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	صحيح	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    ساكن bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    صحيح		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ܐܢ (recursive || regname == '_')
	ܦܢܐ;

    ܐܢ (regname != '.' && insert == عدم
	    && reg == عدم)
	// Can happen when pasting text in normal mode in a terminal buffer
	ܦܢܐ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ܐܢ (list == عدم)
	ܦܢܐ;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ܐܢ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ܐܢ (regname == '.')
    {
	ܐܢ (last_insert_ga.ga_data != عدم)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_insert_ga.ga_data,
		    (صحيح)last_insert_ga.ga_len);
    }
    ܐܚܪܢܐ ܐܢ (insert != عدم)
    {
	list_append_string(list, insert->string, (صحيح)insert->length);
    }
    ܐܚܪܢܐ
    {
	لكل (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (صحيح)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    ܐܢ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (فراغ)dict_add_string_len(v_event, "regname", buf, (صحيح)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (فراغ)dict_add_string_len(v_event, "operator", buf, (صحيح)buflen);

    add_regtype_to_dict(regname, v_event, buf, حجم(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (فراغ)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_items_ro(v_event);

    recursive = true;
    textlock++;
    ܐܢ (post)
	apply_autocmds(EVENT_TEXTPUTPOST, عدم, عدم, FALSE, curbuf);
    ܐܚܪܢܐ
	apply_autocmds(EVENT_TEXTPUTPRE, عدم, عدم, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    فراغ
init_yank(فراغ)
{
    صحيح		i;

    لكل (i = 0; i < NUM_REGISTERS; ++i)
	y_regs[i].y_array = عدم;
}

#ܐܢ defined(EXITFREE)
    فراغ
clear_registers(فراغ)
{
    صحيح		i;

    لكل (i = 0; i < NUM_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	ܐܢ (y_current->y_array != عدم)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    ساكن فراغ
free_yank(long n)
{
    ܐܢ (y_current->y_array == عدم)
	ܦܢܐ;

    long	    i;

    لكل (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    فراغ
free_yank_all(فراغ)
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
    صحيح
op_yank(oparg_T *oap, صحيح deleting, صحيح mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    صحيح			yanktype = oap->motion_type;
    long		yanklines = oap->line_count;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    بنية block_def	bd;

				    // check for read-only register
    ܐܢ (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	ܦܢܐ FAIL;
    }
    ܐܢ (oap->regname == '_')	    // black hole: nothing to do
	ܦܢܐ OK;

#ifdef FEAT_CLIPBOARD
    ܐܢ ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	msg_warn_missing_clipboard();
    }
#endif

    ܐܢ (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    ܐܢ (y_append && y_current->y_array != عدم)
	y_current = &newreg;
    ܐܚܪܢܐ
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    ܐܢ (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(حجم(string_T) * yanklines, TRUE);
    ܐܢ (y_current->y_array == عدم)
    {
	y_current = curr;
	ܦܢܐ FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    ܐܢ (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	ܐܢ (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    لكل ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	بدل (y_current->y_type)
	{
	    حالة MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		ܐܢ (yank_copy_line(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    اذهب fail;
		اكسر;

	    حالة MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		ܐܢ ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == عدم)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    اذهب fail;
		}
		اكسر;

	    حالة MCHAR:
		{
		    صحيح tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (صحيح)STRLEN(bd.textstart);
		    ܐܢ (tmp < bd.textlen)
			bd.textlen = tmp;

		    ܐܢ (yank_copy_line(&bd, y_idx, FALSE) == FAIL)
			اذهب fail;
		    اكسر;
		}
		// NOTREACHED
	}
    }

    ܐܢ (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	ܐܢ (new_ptr == عدم)
	    اذهب fail;
	لكل (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	ܐܢ (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	ܐܢ (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == عدم)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    ܐܢ (pnew == عدم)
	    {
		y_idx = y_current->y_size - 1;
		اذهب fail;
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
	ܐܚܪܢܐ
	    y_idx = 0;
	ܟܡܐ (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    ܐܢ (mess)			// Display message about yank?
    {
	ܐܢ (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	ܐܢ (yanklines > p_report)
	{
	    حرف namebuf[100];

	    ܐܢ (oap->regname == NUL)
		*namebuf = NUL;
	    ܐܚܪܢܐ
		vim_snprintf(namebuf, حجم(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    ܐܢ (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    ܐܚܪܢܐ
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    ܐܢ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	ܐܢ (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	ܐܢ (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    ܐܢ (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	ܐܢ (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    ܐܢ (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	ܐܢ (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    ܐܢ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	ܐܢ (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    ܐܢ (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# ܐܢ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	ܐܢ (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    ܐܢ (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#ܐܢ defined(FEAT_EVAL)
    ܐܢ (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    ܦܢܐ OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    ܦܢܐ FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    ساكن صحيح
yank_copy_line(بنية block_def *bd, long y_idx, صحيح exclude_trailing_space)
{
    char_u	*pnew;

    ܐܢ (exclude_trailing_space)
	bd->endspaces = 0;
    ܐܢ ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == عدم)
	ܦܢܐ FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    ܐܢ (exclude_trailing_space)
    {
	صحيح s = bd->textlen + bd->endspaces;

	ܟܡܐ (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    ܦܢܐ OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    ساكن فراغ
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    حجم(string_T) * y_current->y_size, TRUE);
    ܐܢ (y_current->y_array == عدم)
	y_current->y_size = 0;
    ܐܚܪܢܐ
	لكل (j = 0; j < y_current->y_size; ++j)
	{
	    ܐܢ ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == عدم)
	    {
		free_yank(j);
		y_current->y_size = 0;
		اكسر;
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
    فراغ
do_put(
    صحيح		regname,
    char_u	*expr_result,	// result for regname "=" when compiled
    صحيح		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	count,
    صحيح		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    صحيح		yanklen;
    صحيح		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    صحيح		y_type;
    long	y_size;
    صحيح		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = عدم;
    yankreg_T	*y_current_used = عدم;
    long	nr_lines = 0;
    string_T	insert_string;
    صحيح		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned صحيح cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ܐܢ (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(فراغ)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    ܐܢ (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    ܐܢ (regname == '.')
    {
	ܐܢ (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	ܐܢ (has_textput_events)
	    add_last_insert++;
#endif

	(فراغ)stuff_inserted((dir == FORWARD ? (count == -1 ? 'o' : 'a') :
				    (count == -1 ? 'O' : 'i')), count, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	ܐܢ (has_textputpre())
	    put_do_autocmd('.', عدم, عدم, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	ܐܢ (has_textputpost())
	    put_do_autocmd('.', عدم, عدم, true, dir);

	ܐܢ (has_textput_events && --add_last_insert == 0)
	    ga_clear(&last_insert_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	ܐܢ ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	ܦܢܐ;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    insert_string.string = عدم;
    ܐܢ (regname == '=' && expr_result != عدم)
	insert_string.string = expr_result;
    ܐܚܪܢܐ ܐܢ (get_spec_reg(regname, &insert_string.string, &allocated, TRUE)
		&& insert_string.string == عدم)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	ܦܢܐ;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    ܐܢ (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	اذهب end;

    ܐܢ (insert_string.string != عدم)
    {
	insert_string.length = STRLEN(insert_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	ܐܢ (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    لكل (;;)
	    {
		y_size = 0;
		ptr = insert_string.string;
		ptrlen = insert_string.length;
		ܟܡܐ (ptr != عدم)
		{
		    ܐܢ (y_array != عدم)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    ܐܢ (tmp == عدم)
		    {
			ܐܢ (y_array != عدم)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    ܐܚܪܢܐ
		    {
			ܐܢ (y_array != عدم)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			ܐܢ (*tmp == NUL)
			{
			    y_type = MLINE;
			    اكسر;
			}
		    }
		    ptr = tmp;
		}
		ܐܢ (y_array != عدم)
		    اكسر;
		y_array = ALLOC_MULT(string_T, y_size);
		ܐܢ (y_array == عدم)
		    اذهب end;
	    }
	}
	ܐܚܪܢܐ
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &insert_string;
	}
#ifdef FEAT_EVAL
	ܐܢ (has_textputpre())
	    put_do_autocmd(regname, عدم, &insert_string, false, dir);
#endif
    }
    ܐܚܪܢܐ
    {
#ifdef FEAT_EVAL
	ܐܢ (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, عدم, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    ܐܢ (y_type == MLINE)
    {
	ܐܢ (flags & PUT_LINE_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    ܐܢ (u_save_cursor() == FAIL)
		اذهب end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    ܐܢ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    ܐܢ (ptr == عدم)
		اذهب end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    ܐܢ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    ܐܢ (ptr == عدم)
		اذهب end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_lines;
	    dir = FORWARD;
	}
	ܐܢ (flags & PUT_LINE_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    ܐܢ (flags & PUT_LINE)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    ܐܢ (y_size == 0 || y_array == عدم)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	اذهب end;
    }

    ܐܢ (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	ܐܢ (lnum > curbuf->b_ml.ml_line_count)
	    lnum = curbuf->b_ml.ml_line_count + 1;
	ܐܢ (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    اذهب end;
    }
    ܐܚܪܢܐ ܐܢ (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	ܐܢ (dir == BACKWARD)
	    (فراغ)hasFolding(lnum, &lnum, عدم);
	ܐܚܪܢܐ
	    (فراغ)hasFolding(lnum, عدم, &lnum);
#endif
	ܐܢ (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	ܐܢ ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    اذهب end;
#ifdef FEAT_FOLDING
	ܐܢ (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	ܐܚܪܢܐ
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    ܐܚܪܢܐ ܐܢ (u_save_cursor() == FAIL)
	اذهب end;

    ܐܢ (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	ܐܢ (gchar_cursor() == TAB)
	{
	    صحيح viscol = getviscol();
	    صحيح ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    ܐܢ (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#ܐܚܪܢܐ
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    ܐܚܪܢܐ
		curwin->w_cursor.coladd = 0;
	}
	ܐܚܪܢܐ ܐܢ (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    ܐܢ (y_type == MBLOCK)
    {
	صحيح	delcount;
	صحيح	incr = 0;
	بنية block_def bd;
	long	j;
	صحيح	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	ܐܢ (dir == FORWARD && c != NUL)
	{
	    ܐܢ (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, عدم, &endcol2, 0);
	    ܐܚܪܢܐ
		getvcol(curwin, &curwin->w_cursor, عدم, عدم, &col, 0);

	    ܐܢ (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    ܐܚܪܢܐ
		ܐܢ (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	ܐܚܪܢܐ
	    getvcol(curwin, &curwin->w_cursor, &col, عدم, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	ܐܢ (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    ܐܢ (dir == FORWARD && c == NUL)
		++col;
	    ܐܢ (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    ܐܢ (c == TAB)
	    {
		ܐܢ (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		ܐܢ (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	لكل (i = 0; i < y_size; ++i)
	{
	    صحيح		    spaces = 0;
	    حرف	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    ܐܢ (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	    {
		ܐܢ (ml_append(curbuf->b_ml.ml_line_count, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    اكسر;
		++nr_lines;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    ܟܡܐ (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    ܐܢ (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    ܐܚܪܢܐ ܐܢ (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		ܐܢ (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		ܐܢ (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (صحيح)y_array[i].length;

	    ܐܢ ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		ܟܡܐ (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		ܐܢ (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    ܐܢ (yanklen + spaces != 0
		     && count > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		اكسر;
	    }

	    totlen = count * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    ܐܢ (newp == عدم)
		اكسر;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    لكل (j = 0; j < count; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		ܐܢ ((j < count - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		ܐܚܪܢܐ
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
	    ܐܢ (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_lines(lnum, 0, curwin->w_cursor.lnum - nr_lines, nr_lines);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	ܐܢ (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	ܐܢ (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    ܐܢ (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	ܐܚܪܢܐ
	    curwin->w_cursor.lnum = lnum;
    }
    ܐܚܪܢܐ
    {
	pos_T	new_cursor;

	yanklen = (صحيح)y_array[0].length;

	// Character or Line mode
	ܐܢ (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    ܐܢ (dir == FORWARD && gchar_cursor() != NUL)
	    {
		ܐܢ (has_mbyte)
		{
		    صحيح bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    ܐܢ (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		ܐܚܪܢܐ
		{
		    ++col;
		    ܐܢ (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	ܐܚܪܢܐ ܐܢ (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	ܐܢ (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    صحيح		first_byte_off = 0;

	    ܐܢ (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		ܐܢ (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		ܐܢ (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, عدم, &vcol, عدم, 0);
		}
	    }

	    ܐܢ (count == 0 || yanklen == 0)
	    {
		ܐܢ (VIsual_active)
		    lnum = end_lnum;
	    }
	    ܐܚܪܢܐ ܐܢ (count > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    ܐܚܪܢܐ
	    {
		totlen = count * yanklen;
		ܥܒܕ
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    ܐܢ (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			ܐܢ (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			ܐܚܪܢܐ
			    col = MAXCOL;
		    }
		    ܐܢ (VIsual_active && col > oldlen)
		    {
			lnum++;
			استمر;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    ܐܢ (newp == عدم)
			اذهب end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    لكل (i = 0; i < count; ++i)
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
		    ܐܢ (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    ܐܢ (VIsual_active)
			lnum++;
		} ܟܡܐ (VIsual_active && lnum <= end_lnum);

		ܐܢ (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    ܐܢ (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    ܐܚܪܢܐ
		curwin->w_cursor.col -= first_byte_off;
	}
	ܐܚܪܢܐ
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    صحيح		indent;
	    صحيح		orig_indent = 0;
	    صحيح		indent_diff = 0;	// init for gcc
	    صحيح		first_indent = TRUE;
	    صحيح		lendiff = 0;
	    long	cnt;

	    ܐܢ (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    لكل (cnt = 1; cnt <= count; ++cnt)
	    {
		i = 0;
		ܐܢ (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (صحيح)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    ܐܢ (newp == عدم)
			اذهب error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    ܐܢ (newp == عدم)
			اذهب error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		لكل (; i < y_size; ++i)
		{
		    ܐܢ (y_type != MCHAR || i < y_size - 1)
		    {
			ܐܢ (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    اذهب error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_lines;
		    ܐܢ (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			ܐܢ (cnt == count && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			ܐܢ (*ptr == '#' && preprocs_left())
			    indent = 0;     // Leave # lines at start
			ܐܚܪܢܐ ܐܢ (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			ܐܚܪܢܐ ܐܢ (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			ܐܚܪܢܐ ܐܢ ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(فراغ)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			ܐܢ (cnt == count && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		ܐܢ (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    ܐܢ (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		ܐܢ (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_lines, 0L);

	    // note changed text for displaying and folding
	    ܐܢ (y_type == MCHAR)
		changed_lines(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_lines);
	    ܐܚܪܢܐ
		changed_lines(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_lines);
	    ܐܢ (y_current_used != عدم && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		اذهب end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    ܐܢ (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		ܐܢ (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    ܐܚܪܢܐ
		curbuf->b_op_end.col = 0;

	    ܐܢ (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ܐܚܪܢܐ ܐܢ (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		ܐܢ (y_type == MLINE)
		{
		    ܐܢ (lnum >= curbuf->b_ml.ml_line_count)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		    ܐܚܪܢܐ
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		ܐܚܪܢܐ
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    ܐܢ (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    ܐܚܪܢܐ ܐܢ (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		ܐܢ (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ܐܚܪܢܐ	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_lines);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    صحيح len = ml_get_curline_len();
    ܐܢ (curwin->w_cursor.col > len)
    {
	ܐܢ (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    ܐܢ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    ܐܢ (has_textputpost())
    {
	ܐܢ (insert_string.string == عدم)
	    put_do_autocmd(regname, y_current, عدم, true, dir);
	ܐܚܪܢܐ
	    put_do_autocmd(regname, عدم, &insert_string, true, dir);
    }
#endif

    ܐܢ (allocated)
	vim_free(insert_string.string);
    ܐܢ (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    صحيح
get_register_name(صحيح num)
{
    ܐܢ (num == -1)
	ܦܢܐ '"';
    ܐܚܪܢܐ ܐܢ (num < 10)
	ܦܢܐ num + '0';
    ܐܚܪܢܐ ܐܢ (num == DELETION_REGISTER)
	ܦܢܐ '-';
#ܐܢ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    ܐܚܪܢܐ ܐܢ (num == STAR_REGISTER)
	ܦܢܐ '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    ܐܚܪܢܐ ܐܢ (clipmethod == CLIPMETHOD_PROVIDER && num == REAL_PLUS_REGISTER)
	ܦܢܐ '+';
    ܐܚܪܢܐ ܐܢ (clipmethod != CLIPMETHOD_PROVIDER && num == PLUS_REGISTER)
	ܦܢܐ '+';
#endif
    ܐܚܪܢܐ
	ܦܢܐ num + 'a' - 10;
}

#ܐܢ defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    صحيح
get_unname_register(فراغ)
{
    ܦܢܐ y_previous == عدم ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    فراغ
ex_display(exarg_T *eap)
{
    صحيح		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    صحيح		name;
    صحيح		attr;
    char_u	*arg = eap->arg;
    صحيح		clen;
    صحيح		type;
    string_T	insert;

    silence_w23_w24_msg++;
    ܐܢ (arg != عدم && *arg == NUL)
	arg = عدم;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    msg_puts_title(_("\nType Name Content"));
    لكل (i = -1; i < NUM_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	بدل (get_reg_type(name, عدم))
	{
	    حالة MLINE: type = 'l'; اكسر;
	    حالة MCHAR: type = 'c'; اكسر;
	    افتراضي:	type = 'b'; اكسر;
	}
	ܐܢ (arg != عدم && vim_strchr(arg, name) == عدم
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == عدم)
#endif
		)
	    استمر;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	ܐܢ (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (فراغ)may_get_selection(name);
#endif

	ܐܢ (i == -1)
	{
	    ܐܢ (y_previous != عدم)
		yb = y_previous;
	    ܐܚܪܢܐ
		yb = &(y_regs[0]);
	}
	ܐܚܪܢܐ
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	ܐܢ (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != عدم &&
		    (yb == y_previous || yb == &y_regs[0])))
	    استمر;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	ܐܢ (yb->y_array != عدم)
	{
	    صحيح do_show = FALSE;

	    لكل (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    ܐܢ (do_show || yb->y_size == 0)
	    {
		msg_putchar('\n');
		msg_puts("  ");
		msg_putchar(type);
		msg_puts("  ");
		msg_putchar('"');
		msg_putchar(name);
		msg_puts("   ");

		n = (صحيح)Columns - 11;
		لكل (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    ܐܢ (j)
		    {
			msg_puts_attr("^J", attr);
			n -= 2;
		    }
		    لكل (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			msg_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		ܐܢ (n > 1 && yb->y_type == MLINE)
		    msg_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    insert = get_last_insert();
    ܐܢ ((p = insert.string) != عدم
		  && (arg == عدم || vim_strchr(arg, '.') != عدم) && !got_int
						      && !message_filtered(p))
    {
	msg_puts("\n  c  \".   ");
	dis_msg(p, TRUE);
    }

    // display last command line
    ܐܢ (last_cmdline != عدم && (arg == عدم || vim_strchr(arg, ':') != عدم)
			       && !got_int && !message_filtered(last_cmdline))
    {
	msg_puts("\n  c  \":   ");
	dis_msg(last_cmdline, FALSE);
    }

    // display current file name
    ܐܢ (curbuf->b_fname != عدم
	    && (arg == عدم || vim_strchr(arg, '%') != عدم) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	msg_puts("\n  c  \"%   ");
	dis_msg(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    ܐܢ ((arg == عدم || vim_strchr(arg, '#') != عدم) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	ܐܢ (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    msg_puts("\n  c  \"#   ");
	    dis_msg(fname, FALSE);
	}
    }

    // display last search pattern
    ܐܢ (last_search_pat() != عدم
		 && (arg == عدم || vim_strchr(arg, '/') != عدم) && !got_int
				      && !message_filtered(last_search_pat()))
    {
	msg_puts("\n  c  \"/   ");
	dis_msg(last_search_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    ܐܢ (expr_line != عدم && (arg == عدم || vim_strchr(arg, '=') != عدم)
				  && !got_int && !message_filtered(expr_line))
    {
	msg_puts("\n  c  \"=   ");
	dis_msg(expr_line, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_msg--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    ساكن فراغ
dis_msg(
    char_u	*p,
    صحيح		skip_esc)	    // if TRUE, ignore trailing ESC
{
    صحيح		n;
    صحيح		l;

    n = (صحيح)Columns - 6;
    ܟܡܐ (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	ܐܢ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    msg_outtrans_len(p, l);
	    p += l;
	}
	ܐܚܪܢܐ
	    msg_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#ܐܢ defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    فراغ
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
get_reg_type(صحيح regname, long *reglen)
{
    بدل (regname)
    {
	حالة '%':		// file name
	حالة '#':		// alternate file name
	حالة '=':		// expression
	حالة ':':		// last command line
	حالة '/':		// last search-pattern
	حالة '.':		// last inserted text
	حالة Ctrl_F:		// Filename under cursor
	حالة Ctrl_P:		// Path under cursor, expand via "path"
	حالة Ctrl_W:		// word under cursor
	حالة Ctrl_A:		// WORD (mnemonic All) under cursor
	حالة '_':		// black hole: always empty
	    ܦܢܐ MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ܐܢ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    ܐܢ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ܦܢܐ MAUTO;

    get_yank_register(regname, FALSE);

    ܐܢ (y_current->y_array != عدم)
    {
	ܐܢ (reglen != عدم && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	ܦܢܐ y_current->y_type;
    }
    ܦܢܐ MAUTO;
}

#ܐܢ defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    ساكن char_u *
getreg_wrap_one_line(char_u *s, صحيح flags)
{
    ܐܢ (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	ܐܢ (list != عدم)
	{
	    ܐܢ (list_append_string(list, عدم, -1) == FAIL)
	    {
		list_free(list);
		ܦܢܐ عدم;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	ܦܢܐ (char_u *)list;
    }
    ܦܢܐ s;
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
get_reg_contents(صحيح regname, صحيح flags)
{
    linenr_T	i;
    char_u	*retval;
    صحيح		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    ܐܢ (regname == '=')
    {
	ܐܢ (flags & GREG_NO_EXPR)
	    ܦܢܐ عدم;
	ܐܢ (flags & GREG_EXPR_SRC)
	    ܦܢܐ getreg_wrap_one_line(get_expr_line_src(), flags);
	ܦܢܐ getreg_wrap_one_line(get_expr_line(), flags);
    }

    ܐܢ (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    ܐܢ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ܦܢܐ عدم;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ܐܢ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ܐܢ (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	ܐܢ (retval == عدم)
	    ܦܢܐ عدم;
	ܐܢ (allocated)
	    ܦܢܐ getreg_wrap_one_line(retval, flags);
	ܦܢܐ getreg_wrap_one_line(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    ܐܢ (y_current->y_array == عدم)
	ܦܢܐ عدم;

    ܐܢ (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	ܐܢ (list == عدم)
	    ܦܢܐ عدم;
	لكل (i = 0; i < y_current->y_size; ++i)
	{
	    ܐܢ (list_append_string(list, y_current->y_array[i].string,
		(صحيح)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		ܦܢܐ عدم;
	    }
	}
	ܦܢܐ (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    لكل (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ܐܢ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    ܐܢ (retval == عدم)
	ܦܢܐ عدم;

    // Copy the lines of the yank register into the string.
    len = 0;
    لكل (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ܐܢ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    ܦܢܐ retval;
}

    ساكن صحيح
init_write_reg(
    صحيح		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    صحيح		must_append,
    صحيح		*yank_type UNUSED)
{
    ܐܢ (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	ܦܢܐ FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    ܐܢ (!y_append && !must_append)
	free_yank_all();
    ܦܢܐ OK;
}

    ساكن فراغ
finish_write_reg(
    صحيح		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    ܐܢ (name != '"')
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
    فراغ
write_reg_contents(
    صحيح		name,
    char_u	*str,
    صحيح		maxlen,
    صحيح		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    فراغ
write_reg_contents_lst(
    صحيح		name,
    char_u	**strings,
    صحيح		maxlen UNUSED,
    صحيح		must_append,
    صحيح		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    ܐܢ (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	ܐܢ (strings[0] == عدم)
	    s = (char_u *)"";
	ܐܚܪܢܐ ܐܢ (strings[1] != عدم)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_lines), name);
	    ܦܢܐ;
	}
	ܐܚܪܢܐ
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	ܦܢܐ;
    }

    ܐܢ (name == '_')	    // black hole: nothing to do
	ܦܢܐ;

    ܐܢ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ܦܢܐ;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    فراغ
write_reg_contents_ex(
    صحيح		name,
    char_u	*str,
    صحيح		maxlen,
    صحيح		must_append,
    صحيح		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    ܐܢ (maxlen >= 0)
	len = maxlen;
    ܐܚܪܢܐ
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    ܐܢ (name == '/')
    {
	set_last_search_pat(str, RE_SEARCH, TRUE, TRUE);
	ܦܢܐ;
    }

    ܐܢ (name == '#')
    {
	ܐܢ (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  ܦܢܐ;
	}

	buf_T	*buf;

	ܐܢ (VIM_ISDIGIT(*str))
	{
	    صحيح	num = atoi((حرف *)str);

	    buf = buflist_findnr(num);
	    ܐܢ (buf == عدم)
		semsg(_(e_buffer_nr_does_not_exist), (long)num);
	}
	ܐܚܪܢܐ
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	ܐܢ (buf == عدم)
	    ܦܢܐ;
	curwin->w_alt_fnum = buf->b_fnum;
	ܦܢܐ;
    }

    ܐܢ (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	ܐܢ (p == عدم)
	    ܦܢܐ;
	ܐܢ (must_append && expr_line != عدم)
	{
	    s = concat_str(expr_line, p);
	    vim_free(p);
	    p = s;
	}
	set_expr_line(p, عدم);
	ܦܢܐ;
    }

    ܐܢ (name == '_')	    // black hole: nothing to do
	ܦܢܐ;

    ܐܢ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ܦܢܐ;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#ܐܢ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    فراغ
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    صحيح		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    صحيح		str_list)	// TRUE if str is char_u **
{
    صحيح		type;			// MCHAR, MLINE or MBLOCK
    صحيح		lnum;
    long	start;
    long	i;
    صحيح		extra;
    صحيح		newlines;		// number of lines added
    صحيح		extraline = 0;		// extra line at the end
    صحيح		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    ܐܢ (y_ptr->y_array == عدم)		// NULL means empty register
	y_ptr->y_size = 0;

    ܐܢ (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    ܐܚܪܢܐ
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    ܐܢ (str_list)
    {
	لكل (ss = (char_u **) str; *ss != عدم; ++ss)
	    ++newlines;
    }
    ܐܚܪܢܐ
    {
	لكل (i = 0; i < len; i++)
	    ܐܢ (str[i] == '\n')
		++newlines;
	ܐܢ (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	ܐܢ (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    ܐܢ (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	ܦܢܐ;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * حجم(string_T), TRUE);
    ܐܢ (pp == عدم)	// out of memory
	ܦܢܐ;
    لكل (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    ܐܢ (str_list)
    {
	لكل (ss = (char_u **) str; *ss != عدم; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    ܐܢ (type == MBLOCK)
	    {
		صحيح charlen = vim_strsize(*ss);

		ܐܢ (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    ܐܚܪܢܐ
    {
	لكل (start = 0; start < len + extraline; start += i + 1)
	{
	    صحيح charlen = 0;

	    لكل (i = start; i < len;)	// find the end of the line
	    {
		ܐܢ (str[i] == '\n')
		    اكسر;
		ܐܢ (type == MBLOCK)
		{
		    ܐܢ (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    ܐܚܪܢܐ
			charlen += byte2cells(str[i]);
		}

		ܐܢ (str[i] == NUL)
		    i++; // registers can have NUL chars
		ܐܚܪܢܐ
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    ܐܢ (charlen > maxlen)
		maxlen = charlen;
	    ܐܢ (append)
	    {
		--lnum;
		extra = (صحيح)y_ptr->y_array[lnum].length;
	    }
	    ܐܚܪܢܐ
		extra = 0;
	    s = alloc(i + extra + 1);
	    ܐܢ (s == عدم)
		اكسر;
	    ܐܢ (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    ܐܢ (append)
		vim_free(y_ptr->y_array[lnum].string);
	    ܐܢ (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    ܟܡܐ (--extra >= 0)
	    {
		ܐܢ (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    ܐܢ (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    ܐܚܪܢܐ
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
