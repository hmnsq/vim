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

#𐤀𐤌 defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(فراغ)
{
    أرجع y_regs;
}
#endif

#𐤀𐤌 defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(صحيح reg)
{
    أرجع &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(فراغ)
{
    أرجع y_current;
}

    yankreg_T *
get_y_previous(فراغ)
{
    أرجع y_previous;
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


#𐤀𐤌 defined(FEAT_EVAL)
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
    𐤀𐤌 (new_line == عدم)
	أرجع NUL;
    𐤀𐤌 (*new_line == NUL)	// use previous line
	vim_free(new_line);
    وإلا
	set_expr_line(new_line, عدم);
    أرجع '=';
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

    𐤀𐤌 (expr_line == عدم)
	أرجع عدم;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    expr_copy = vim_strsave(expr_line);
    𐤀𐤌 (expr_copy == عدم)
	أرجع عدم;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    𐤀𐤌 (nested >= 10)
	أرجع expr_copy;

    ++nested;
    rv = eval_to_string_eap(expr_copy, TRUE, expr_eap, FALSE);
    --nested;
    vim_free(expr_copy);
    أرجع rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ساكن char_u *
get_expr_line_src(فراغ)
{
    𐤀𐤌 (expr_line == عدم)
	أرجع عدم;
    أرجع vim_strsave(expr_line);
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
    𐤀𐤌 (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#وإلا
				    "/#.%:"
#endif
					, regname) != عدم)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#𐤀𐤌 defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
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
	أرجع TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    وإلا 𐤀𐤌 (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	msg_warn_missing_clipboard();
	أرجع FALSE;
    }
#endif
    أرجع FALSE;
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
    𐤀𐤌 ((regname == 0 || regname == '"') && !writing && y_previous != عدم)
    {
	y_current = y_previous;
	أرجع ret;
    }
    i = regname;
    𐤀𐤌 (VIM_ISDIGIT(i))
	i -= '0';
    وإلا 𐤀𐤌 (ASCII_ISLOWER(i))
	i = CharOrdLow(i) + 10;
    وإلا 𐤀𐤌 (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    وإلا 𐤀𐤌 (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    وإلا 𐤀𐤌 (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    وإلا 𐤀𐤌 (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	𐤀𐤌 (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	وإلا
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#وإلاإذا defined(FEAT_CLIPBOARD_PROVIDER)
    وإلا 𐤀𐤌 (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    وإلا 𐤀𐤌 (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    وإلا 𐤀𐤌 (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    وإلا		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    𐤀𐤌 (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    أرجع ret;
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
    𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	𐤀𐤌 (name == '*' && clip_star.available)
	{
	    𐤀𐤌 (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	𐤀𐤌 (name == '+' && clip_plus.available)
	{
	    𐤀𐤌 (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    𐤀𐤌 (reg == عدم)
	أرجع (فراغ *)عدم;

    *reg = *y_current;
    𐤀𐤌 (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	𐤀𐤌 (reg->y_size == 0 || y_current->y_array == عدم)
	    reg->y_array = عدم;
	وإلا
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	𐤀𐤌 (reg->y_array != عدم)
	{
	    لكل (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		𐤀𐤌 (reg->y_array[i].string == عدم)
		{
		    // The allocation failed so clean up and exit
		    طالما (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    أرجع (فراغ *)عدم;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    وإلا
	y_current->y_array = عدم;
    أرجع (فراغ *)reg;
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

#𐤀𐤌 defined(FEAT_CLIPBOARD)
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
    𐤀𐤌 (regname != 0 && !valid_yank_reg(regname, FALSE))
	أرجع FALSE;
    𐤀𐤌 (regname == '_')		// black hole is always empty
	أرجع FALSE;
    get_yank_register(regname, FALSE);
    أرجع (y_current->y_type == MLINE);
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

    𐤀𐤌 (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	𐤀𐤌 (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	وإلا
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    وإلا			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	msg("");
	p = get_recorded();
	𐤀𐤌 (p == عدم)
	    retval = FAIL;
	وإلا
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
    أرجع retval;
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
    𐤀𐤌 (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	أرجع FAIL;
    }
    𐤀𐤌 (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	أرجع OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    𐤀𐤌 (y_append && y_current->y_array != عدم)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	𐤀𐤌 (tmp == عدم)
	{
	    vim_free(p);
	    أرجع FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    وإلا
    {
	free_yank_all();
	𐤀𐤌 ((y_current->y_array = ALLOC_ONE(string_T)) == عدم)
	{
	    vim_free(p);
	    أرجع FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    أرجع OK;
}

/*
 * Last executed register (@ command)
 */
ساكن صحيح execreg_lastc = NUL;

#𐤀𐤌 defined(FEAT_VIMINFO)
    صحيح
get_execreg_lastc(فراغ)
{
    أرجع execreg_lastc;
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
    طالما (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(lines[cmd_start].string);
	𐤀𐤌 (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
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
	𐤀𐤌 (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    𐤀𐤌 (ga.ga_len > 400)
	    {
		𐤀𐤌 (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		وإلا
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
    أرجع str;
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
    𐤀𐤌 (regname == '@')
    {
	𐤀𐤌 (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    أرجع FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    𐤀𐤌 (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	أرجع FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    𐤀𐤌 (regname == '_')
	أرجع OK;

    // use last command line
    𐤀𐤌 (regname == ':')
    {
	𐤀𐤌 (last_cmdline == عدم)
	{
	    emsg(_(e_no_previous_command_line));
	    أرجع FAIL;
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
	𐤀𐤌 (p != عدم)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    𐤀𐤌 (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    وإلا
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    وإلا 𐤀𐤌 (regname == '=')
    {
	p = get_expr_line();
	𐤀𐤌 (p == عدم)
	    أرجع FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    وإلا 𐤀𐤌 (regname == '.')		// use last inserted text
    {
	p = get_last_insert_save();
	𐤀𐤌 (p == عدم)
	{
	    emsg(_(e_no_inserted_text_yet));
	    أرجع FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    وإلا
    {
	get_yank_register(regname, FALSE);
	𐤀𐤌 (y_current->y_array == عدم)
	    أرجع FAIL;

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
	    𐤀𐤌 (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		𐤀𐤌 (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    أرجع FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    𐤀𐤌 (colon && i > 0)
	    {
		p = skipwhite(str);
		𐤀𐤌 (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_line_continuation(y_current->y_array, &i);
		    𐤀𐤌 (str == عدم)
			أرجع FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    𐤀𐤌 (free_str)
		vim_free(str);
	    𐤀𐤌 (escaped == عدم)
		أرجع FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    𐤀𐤌 (retval == FAIL)
		أرجع FAIL;
	    𐤀𐤌 (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		أرجع FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    أرجع retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ساكن فراغ
put_reedit_in_typebuf(صحيح silent)
{
    𐤀𐤌 (restart_edit == NUL)
	أرجع;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    𐤀𐤌 (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    وإلا 𐤀𐤌 (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    وإلا 𐤀𐤌 (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    𐤀𐤌 (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
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
    𐤀𐤌 (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    𐤀𐤌 (retval == OK)
    {
	char_u	*p;

	𐤀𐤌 (esc)
	    p = vim_strsave_escape_csi(s);
	وإلا
	    p = s;
	𐤀𐤌 (p == عدم)
	    retval = FAIL;
	وإلا
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	𐤀𐤌 (esc)
	    vim_free(p);
    }
    𐤀𐤌 (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    أرجع retval;
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
    𐤀𐤌 (got_int)
	أرجع FAIL;

    // check for valid regname
    𐤀𐤌 (regname != NUL && !valid_yank_reg(regname, FALSE))
	أرجع FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    𐤀𐤌 (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    وإلا 𐤀𐤌 (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	𐤀𐤌 (arg == عدم)
	    أرجع FAIL;
	stuffescaped(arg, literally);
	𐤀𐤌 (allocated)
	    vim_free(arg);
    }
    وإلا				// name or number register
    {
	𐤀𐤌 (get_yank_register(regname, FALSE))
	    literally = TRUE;
	𐤀𐤌 (y_current->y_array == عدم)
	    retval = FAIL;
	وإلا
	{
	    لكل (i = 0; i < y_current->y_size; ++i)
	    {
		𐤀𐤌 (regname == '-' && y_current->y_type == MCHAR)
		{
		    صحيح dir = BACKWARD;
		    𐤀𐤌 ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			𐤀𐤌 (u_save_cursor() == FAIL)
			    أرجع FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			𐤀𐤌 (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, عدم, dir, 1L, PUT_CURSEND);
		}
		وإلا
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    𐤀𐤌 (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    أرجع retval;
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
	    𐤀𐤌 (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    أرجع TRUE;

	حالة '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    أرجع TRUE;

#ifdef FEAT_EVAL
	حالة '=':		// result of expression
	    *argp = get_expr_line();
	    *allocated = TRUE;
	    أرجع TRUE;
#endif

	حالة ':':		// last command line
	    𐤀𐤌 (last_cmdline == عدم && errmsg)
		emsg(_(e_no_previous_command_line));
	    *argp = last_cmdline;
	    أرجع TRUE;

	حالة '/':		// last search-pattern
	    𐤀𐤌 (last_search_pat() == عدم && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_search_pat();
	    أرجع TRUE;

	حالة '.':		// last inserted text
	    *argp = get_last_insert_save();
	    *allocated = TRUE;
	    𐤀𐤌 (*argp == عدم && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    أرجع TRUE;

	حالة Ctrl_F:		// Filename under cursor
	حالة Ctrl_P:		// Path under cursor, expand via "path"
	    𐤀𐤌 (!errmsg)
		أرجع FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, عدم);
	    *allocated = TRUE;
	    أرجع TRUE;

	حالة Ctrl_W:		// word under cursor
	حالة Ctrl_A:		// WORD (mnemonic All) under cursor
	    𐤀𐤌 (!errmsg)
		أرجع FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : عدم;
	    *allocated = TRUE;
	    أرجع TRUE;

	حالة Ctrl_L:		// Line under cursor
	    𐤀𐤌 (!errmsg)
		أرجع FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    أرجع TRUE;

	حالة '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    أرجع TRUE;
    }

    أرجع FALSE;
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

    𐤀𐤌 (get_yank_register(regname, FALSE))
	literally = TRUE;
    𐤀𐤌 (y_current->y_array == عدم)
	أرجع FAIL;

    لكل (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	𐤀𐤌 ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	𐤀𐤌 (got_int)
	    أرجع FAIL;
    }
    أرجع OK;
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
    𐤀𐤌 (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = عدم;		// set register one to empty
}

#𐤀𐤌 defined(FEAT_EVAL)
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

    𐤀𐤌 (recursive)
	أرجع;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    𐤀𐤌 (list == عدم)
	أرجع;

    // yanked text contents
    لكل (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (صحيح)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    𐤀𐤌 (dict_add_list(v_event, "regcontents", list) == FAIL)
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

    𐤀𐤌 (recursive || regname == '_')
	أرجع;

    𐤀𐤌 (regname != '.' && insert == عدم
	    && reg == عدم)
	// Can happen when pasting text in normal mode in a terminal buffer
	أرجع;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    𐤀𐤌 (list == عدم)
	أرجع;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    𐤀𐤌 (regname == '.')
    {
	𐤀𐤌 (last_insert_ga.ga_data != عدم)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_insert_ga.ga_data,
		    (صحيح)last_insert_ga.ga_len);
    }
    وإلا 𐤀𐤌 (insert != عدم)
    {
	list_append_string(list, insert->string, (صحيح)insert->length);
    }
    وإلا
    {
	لكل (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (صحيح)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    𐤀𐤌 (dict_add_list(v_event, "regcontents", list) == FAIL)
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
    𐤀𐤌 (post)
	apply_autocmds(EVENT_TEXTPUTPOST, عدم, عدم, FALSE, curbuf);
    وإلا
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

#𐤀𐤌 defined(EXITFREE)
    فراغ
clear_registers(فراغ)
{
    صحيح		i;

    لكل (i = 0; i < NUM_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	𐤀𐤌 (y_current->y_array != عدم)
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
    𐤀𐤌 (y_current->y_array == عدم)
	أرجع;

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
    𐤀𐤌 (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	أرجع FAIL;
    }
    𐤀𐤌 (oap->regname == '_')	    // black hole: nothing to do
	أرجع OK;

#ifdef FEAT_CLIPBOARD
    𐤀𐤌 ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	msg_warn_missing_clipboard();
    }
#endif

    𐤀𐤌 (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    𐤀𐤌 (y_append && y_current->y_array != عدم)
	y_current = &newreg;
    وإلا
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    𐤀𐤌 (       oap->motion_type == MCHAR
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
    𐤀𐤌 (y_current->y_array == عدم)
    {
	y_current = curr;
	أرجع FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    𐤀𐤌 (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	𐤀𐤌 (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    لكل ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	بدل (y_current->y_type)
	{
	    حالة MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		𐤀𐤌 (yank_copy_line(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    اذهب fail;
		اكسر;

	    حالة MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		𐤀𐤌 ((y_current->y_array[y_idx].string =
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
		    𐤀𐤌 (tmp < bd.textlen)
			bd.textlen = tmp;

		    𐤀𐤌 (yank_copy_line(&bd, y_idx, FALSE) == FAIL)
			اذهب fail;
		    اكسر;
		}
		// NOTREACHED
	}
    }

    𐤀𐤌 (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	𐤀𐤌 (new_ptr == عدم)
	    اذهب fail;
	لكل (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	𐤀𐤌 (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	𐤀𐤌 (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == عدم)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    𐤀𐤌 (pnew == عدم)
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
	وإلا
	    y_idx = 0;
	طالما (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    𐤀𐤌 (mess)			// Display message about yank?
    {
	𐤀𐤌 (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	𐤀𐤌 (yanklines > p_report)
	{
	    حرف namebuf[100];

	    𐤀𐤌 (oap->regname == NUL)
		*namebuf = NUL;
	    وإلا
		vim_snprintf(namebuf, حجم(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    𐤀𐤌 (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    وإلا
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    𐤀𐤌 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	𐤀𐤌 (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	𐤀𐤌 (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    𐤀𐤌 (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	𐤀𐤌 (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    𐤀𐤌 (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	𐤀𐤌 (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	𐤀𐤌 (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    𐤀𐤌 (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# 𐤀𐤌 defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	𐤀𐤌 (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    𐤀𐤌 (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#𐤀𐤌 defined(FEAT_EVAL)
    𐤀𐤌 (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    أرجع OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    أرجع FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    ساكن صحيح
yank_copy_line(بنية block_def *bd, long y_idx, صحيح exclude_trailing_space)
{
    char_u	*pnew;

    𐤀𐤌 (exclude_trailing_space)
	bd->endspaces = 0;
    𐤀𐤌 ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == عدم)
	أرجع FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    𐤀𐤌 (exclude_trailing_space)
    {
	صحيح s = bd->textlen + bd->endspaces;

	طالما (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    أرجع OK;
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
    𐤀𐤌 (y_current->y_array == عدم)
	y_current->y_size = 0;
    وإلا
	لكل (j = 0; j < y_current->y_size; ++j)
	{
	    𐤀𐤌 ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
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
    𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(فراغ)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    𐤀𐤌 (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    𐤀𐤌 (regname == '.')
    {
	𐤀𐤌 (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	𐤀𐤌 (has_textput_events)
	    add_last_insert++;
#endif

	(فراغ)stuff_inserted((dir == FORWARD ? (count == -1 ? 'o' : 'a') :
				    (count == -1 ? 'O' : 'i')), count, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	𐤀𐤌 (has_textputpre())
	    put_do_autocmd('.', عدم, عدم, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	𐤀𐤌 (has_textputpost())
	    put_do_autocmd('.', عدم, عدم, true, dir);

	𐤀𐤌 (has_textput_events && --add_last_insert == 0)
	    ga_clear(&last_insert_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	𐤀𐤌 ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	أرجع;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    insert_string.string = عدم;
    𐤀𐤌 (regname == '=' && expr_result != عدم)
	insert_string.string = expr_result;
    وإلا 𐤀𐤌 (get_spec_reg(regname, &insert_string.string, &allocated, TRUE)
		&& insert_string.string == عدم)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	أرجع;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    𐤀𐤌 (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	اذهب end;

    𐤀𐤌 (insert_string.string != عدم)
    {
	insert_string.length = STRLEN(insert_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	𐤀𐤌 (regname == '=')
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
		طالما (ptr != عدم)
		{
		    𐤀𐤌 (y_array != عدم)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    𐤀𐤌 (tmp == عدم)
		    {
			𐤀𐤌 (y_array != عدم)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    وإلا
		    {
			𐤀𐤌 (y_array != عدم)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			𐤀𐤌 (*tmp == NUL)
			{
			    y_type = MLINE;
			    اكسر;
			}
		    }
		    ptr = tmp;
		}
		𐤀𐤌 (y_array != عدم)
		    اكسر;
		y_array = ALLOC_MULT(string_T, y_size);
		𐤀𐤌 (y_array == عدم)
		    اذهب end;
	    }
	}
	وإلا
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &insert_string;
	}
#ifdef FEAT_EVAL
	𐤀𐤌 (has_textputpre())
	    put_do_autocmd(regname, عدم, &insert_string, false, dir);
#endif
    }
    وإلا
    {
#ifdef FEAT_EVAL
	𐤀𐤌 (has_textputpre())
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

    𐤀𐤌 (y_type == MLINE)
    {
	𐤀𐤌 (flags & PUT_LINE_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    𐤀𐤌 (u_save_cursor() == FAIL)
		اذهب end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    𐤀𐤌 (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    𐤀𐤌 (ptr == عدم)
		اذهب end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    𐤀𐤌 (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    𐤀𐤌 (ptr == عدم)
		اذهب end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_lines;
	    dir = FORWARD;
	}
	𐤀𐤌 (flags & PUT_LINE_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    𐤀𐤌 (flags & PUT_LINE)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    𐤀𐤌 (y_size == 0 || y_array == عدم)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	اذهب end;
    }

    𐤀𐤌 (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	𐤀𐤌 (lnum > curbuf->b_ml.ml_line_count)
	    lnum = curbuf->b_ml.ml_line_count + 1;
	𐤀𐤌 (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    اذهب end;
    }
    وإلا 𐤀𐤌 (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	𐤀𐤌 (dir == BACKWARD)
	    (فراغ)hasFolding(lnum, &lnum, عدم);
	وإلا
	    (فراغ)hasFolding(lnum, عدم, &lnum);
#endif
	𐤀𐤌 (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	𐤀𐤌 ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    اذهب end;
#ifdef FEAT_FOLDING
	𐤀𐤌 (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	وإلا
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    وإلا 𐤀𐤌 (u_save_cursor() == FAIL)
	اذهب end;

    𐤀𐤌 (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	𐤀𐤌 (gchar_cursor() == TAB)
	{
	    صحيح viscol = getviscol();
	    صحيح ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    𐤀𐤌 (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#وإلا
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    وإلا
		curwin->w_cursor.coladd = 0;
	}
	وإلا 𐤀𐤌 (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    𐤀𐤌 (y_type == MBLOCK)
    {
	صحيح	delcount;
	صحيح	incr = 0;
	بنية block_def bd;
	long	j;
	صحيح	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	𐤀𐤌 (dir == FORWARD && c != NUL)
	{
	    𐤀𐤌 (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, عدم, &endcol2, 0);
	    وإلا
		getvcol(curwin, &curwin->w_cursor, عدم, عدم, &col, 0);

	    𐤀𐤌 (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    وإلا
		𐤀𐤌 (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	وإلا
	    getvcol(curwin, &curwin->w_cursor, &col, عدم, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	𐤀𐤌 (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    𐤀𐤌 (dir == FORWARD && c == NUL)
		++col;
	    𐤀𐤌 (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    𐤀𐤌 (c == TAB)
	    {
		𐤀𐤌 (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		𐤀𐤌 (dir == FORWARD && col - 1 == endcol2)
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
	    𐤀𐤌 (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	    {
		𐤀𐤌 (ml_append(curbuf->b_ml.ml_line_count, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    اكسر;
		++nr_lines;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    طالما (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    𐤀𐤌 (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    وإلا 𐤀𐤌 (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		𐤀𐤌 (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		𐤀𐤌 (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (صحيح)y_array[i].length;

	    𐤀𐤌 ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		طالما (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		𐤀𐤌 (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    𐤀𐤌 (yanklen + spaces != 0
		     && count > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		اكسر;
	    }

	    totlen = count * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    𐤀𐤌 (newp == عدم)
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
		𐤀𐤌 ((j < count - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		وإلا
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
	    𐤀𐤌 (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_lines(lnum, 0, curwin->w_cursor.lnum - nr_lines, nr_lines);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	𐤀𐤌 (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	𐤀𐤌 (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    𐤀𐤌 (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	وإلا
	    curwin->w_cursor.lnum = lnum;
    }
    وإلا
    {
	pos_T	new_cursor;

	yanklen = (صحيح)y_array[0].length;

	// Character or Line mode
	𐤀𐤌 (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    𐤀𐤌 (dir == FORWARD && gchar_cursor() != NUL)
	    {
		𐤀𐤌 (has_mbyte)
		{
		    صحيح bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    𐤀𐤌 (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		وإلا
		{
		    ++col;
		    𐤀𐤌 (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	وإلا 𐤀𐤌 (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	𐤀𐤌 (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    صحيح		first_byte_off = 0;

	    𐤀𐤌 (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		𐤀𐤌 (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		𐤀𐤌 (end_lnum > start_lnum)
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

	    𐤀𐤌 (count == 0 || yanklen == 0)
	    {
		𐤀𐤌 (VIsual_active)
		    lnum = end_lnum;
	    }
	    وإلا 𐤀𐤌 (count > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    وإلا
	    {
		totlen = count * yanklen;
		𐤐𐤏𐤋
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    𐤀𐤌 (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			𐤀𐤌 (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			وإلا
			    col = MAXCOL;
		    }
		    𐤀𐤌 (VIsual_active && col > oldlen)
		    {
			lnum++;
			استمر;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    𐤀𐤌 (newp == عدم)
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
		    𐤀𐤌 (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    𐤀𐤌 (VIsual_active)
			lnum++;
		} طالما (VIsual_active && lnum <= end_lnum);

		𐤀𐤌 (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    𐤀𐤌 (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    وإلا
		curwin->w_cursor.col -= first_byte_off;
	}
	وإلا
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    صحيح		indent;
	    صحيح		orig_indent = 0;
	    صحيح		indent_diff = 0;	// init for gcc
	    صحيح		first_indent = TRUE;
	    صحيح		lendiff = 0;
	    long	cnt;

	    𐤀𐤌 (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    لكل (cnt = 1; cnt <= count; ++cnt)
	    {
		i = 0;
		𐤀𐤌 (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (صحيح)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    𐤀𐤌 (newp == عدم)
			اذهب error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    𐤀𐤌 (newp == عدم)
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
		    𐤀𐤌 (y_type != MCHAR || i < y_size - 1)
		    {
			𐤀𐤌 (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    اذهب error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_lines;
		    𐤀𐤌 (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			𐤀𐤌 (cnt == count && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			𐤀𐤌 (*ptr == '#' && preprocs_left())
			    indent = 0;     // Leave # lines at start
			وإلا 𐤀𐤌 (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			وإلا 𐤀𐤌 (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			وإلا 𐤀𐤌 ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(فراغ)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			𐤀𐤌 (cnt == count && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		𐤀𐤌 (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    𐤀𐤌 (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		𐤀𐤌 (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_lines, 0L);

	    // note changed text for displaying and folding
	    𐤀𐤌 (y_type == MCHAR)
		changed_lines(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_lines);
	    وإلا
		changed_lines(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_lines);
	    𐤀𐤌 (y_current_used != عدم && (y_current_used != y_current
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
	    𐤀𐤌 (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		𐤀𐤌 (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    وإلا
		curbuf->b_op_end.col = 0;

	    𐤀𐤌 (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    وإلا 𐤀𐤌 (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		𐤀𐤌 (y_type == MLINE)
		{
		    𐤀𐤌 (lnum >= curbuf->b_ml.ml_line_count)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		    وإلا
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		وإلا
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    𐤀𐤌 (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    وإلا 𐤀𐤌 (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		𐤀𐤌 (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    وإلا	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_lines);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    صحيح len = ml_get_curline_len();
    𐤀𐤌 (curwin->w_cursor.col > len)
    {
	𐤀𐤌 (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    𐤀𐤌 (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    𐤀𐤌 (has_textputpost())
    {
	𐤀𐤌 (insert_string.string == عدم)
	    put_do_autocmd(regname, y_current, عدم, true, dir);
	وإلا
	    put_do_autocmd(regname, عدم, &insert_string, true, dir);
    }
#endif

    𐤀𐤌 (allocated)
	vim_free(insert_string.string);
    𐤀𐤌 (regname == '=')
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
    𐤀𐤌 (num == -1)
	أرجع '"';
    وإلا 𐤀𐤌 (num < 10)
	أرجع num + '0';
    وإلا 𐤀𐤌 (num == DELETION_REGISTER)
	أرجع '-';
#𐤀𐤌 defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    وإلا 𐤀𐤌 (num == STAR_REGISTER)
	أرجع '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    وإلا 𐤀𐤌 (clipmethod == CLIPMETHOD_PROVIDER && num == REAL_PLUS_REGISTER)
	أرجع '+';
    وإلا 𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER && num == PLUS_REGISTER)
	أرجع '+';
#endif
    وإلا
	أرجع num + 'a' - 10;
}

#𐤀𐤌 defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    صحيح
get_unname_register(فراغ)
{
    أرجع y_previous == عدم ? -1 : y_previous - &y_regs[0];
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
    𐤀𐤌 (arg != عدم && *arg == NUL)
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
	𐤀𐤌 (arg != عدم && vim_strchr(arg, name) == عدم
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
	𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (فراغ)may_get_selection(name);
#endif

	𐤀𐤌 (i == -1)
	{
	    𐤀𐤌 (y_previous != عدم)
		yb = y_previous;
	    وإلا
		yb = &(y_regs[0]);
	}
	وإلا
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	𐤀𐤌 (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != عدم &&
		    (yb == y_previous || yb == &y_regs[0])))
	    استمر;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	𐤀𐤌 (yb->y_array != عدم)
	{
	    صحيح do_show = FALSE;

	    لكل (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    𐤀𐤌 (do_show || yb->y_size == 0)
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
		    𐤀𐤌 (j)
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
		𐤀𐤌 (n > 1 && yb->y_type == MLINE)
		    msg_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    insert = get_last_insert();
    𐤀𐤌 ((p = insert.string) != عدم
		  && (arg == عدم || vim_strchr(arg, '.') != عدم) && !got_int
						      && !message_filtered(p))
    {
	msg_puts("\n  c  \".   ");
	dis_msg(p, TRUE);
    }

    // display last command line
    𐤀𐤌 (last_cmdline != عدم && (arg == عدم || vim_strchr(arg, ':') != عدم)
			       && !got_int && !message_filtered(last_cmdline))
    {
	msg_puts("\n  c  \":   ");
	dis_msg(last_cmdline, FALSE);
    }

    // display current file name
    𐤀𐤌 (curbuf->b_fname != عدم
	    && (arg == عدم || vim_strchr(arg, '%') != عدم) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	msg_puts("\n  c  \"%   ");
	dis_msg(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    𐤀𐤌 ((arg == عدم || vim_strchr(arg, '#') != عدم) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	𐤀𐤌 (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    msg_puts("\n  c  \"#   ");
	    dis_msg(fname, FALSE);
	}
    }

    // display last search pattern
    𐤀𐤌 (last_search_pat() != عدم
		 && (arg == عدم || vim_strchr(arg, '/') != عدم) && !got_int
				      && !message_filtered(last_search_pat()))
    {
	msg_puts("\n  c  \"/   ");
	dis_msg(last_search_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    𐤀𐤌 (expr_line != عدم && (arg == عدم || vim_strchr(arg, '=') != عدم)
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
    طالما (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	𐤀𐤌 (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    msg_outtrans_len(p, l);
	    p += l;
	}
	وإلا
	    msg_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#𐤀𐤌 defined(FEAT_DND)
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
	    أرجع MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    𐤀𐤌 (regname != NUL && !valid_yank_reg(regname, FALSE))
	أرجع MAUTO;

    get_yank_register(regname, FALSE);

    𐤀𐤌 (y_current->y_array != عدم)
    {
	𐤀𐤌 (reglen != عدم && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	أرجع y_current->y_type;
    }
    أرجع MAUTO;
}

#𐤀𐤌 defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    ساكن char_u *
getreg_wrap_one_line(char_u *s, صحيح flags)
{
    𐤀𐤌 (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	𐤀𐤌 (list != عدم)
	{
	    𐤀𐤌 (list_append_string(list, عدم, -1) == FAIL)
	    {
		list_free(list);
		أرجع عدم;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	أرجع (char_u *)list;
    }
    أرجع s;
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
    𐤀𐤌 (regname == '=')
    {
	𐤀𐤌 (flags & GREG_NO_EXPR)
	    أرجع عدم;
	𐤀𐤌 (flags & GREG_EXPR_SRC)
	    أرجع getreg_wrap_one_line(get_expr_line_src(), flags);
	أرجع getreg_wrap_one_line(get_expr_line(), flags);
    }

    𐤀𐤌 (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    𐤀𐤌 (regname != NUL && !valid_yank_reg(regname, FALSE))
	أرجع عدم;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    𐤀𐤌 (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    𐤀𐤌 (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	𐤀𐤌 (retval == عدم)
	    أرجع عدم;
	𐤀𐤌 (allocated)
	    أرجع getreg_wrap_one_line(retval, flags);
	أرجع getreg_wrap_one_line(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    𐤀𐤌 (y_current->y_array == عدم)
	أرجع عدم;

    𐤀𐤌 (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	𐤀𐤌 (list == عدم)
	    أرجع عدم;
	لكل (i = 0; i < y_current->y_size; ++i)
	{
	    𐤀𐤌 (list_append_string(list, y_current->y_array[i].string,
		(صحيح)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		أرجع عدم;
	    }
	}
	أرجع (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    لكل (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	𐤀𐤌 (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    𐤀𐤌 (retval == عدم)
	أرجع عدم;

    // Copy the lines of the yank register into the string.
    len = 0;
    لكل (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	𐤀𐤌 (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    أرجع retval;
}

    ساكن صحيح
init_write_reg(
    صحيح		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    صحيح		must_append,
    صحيح		*yank_type UNUSED)
{
    𐤀𐤌 (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	أرجع FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    𐤀𐤌 (!y_append && !must_append)
	free_yank_all();
    أرجع OK;
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
    𐤀𐤌 (name != '"')
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

    𐤀𐤌 (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	𐤀𐤌 (strings[0] == عدم)
	    s = (char_u *)"";
	وإلا 𐤀𐤌 (strings[1] != عدم)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_lines), name);
	    أرجع;
	}
	وإلا
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	أرجع;
    }

    𐤀𐤌 (name == '_')	    // black hole: nothing to do
	أرجع;

    𐤀𐤌 (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	أرجع;

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

    𐤀𐤌 (maxlen >= 0)
	len = maxlen;
    وإلا
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    𐤀𐤌 (name == '/')
    {
	set_last_search_pat(str, RE_SEARCH, TRUE, TRUE);
	أرجع;
    }

    𐤀𐤌 (name == '#')
    {
	𐤀𐤌 (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  أرجع;
	}

	buf_T	*buf;

	𐤀𐤌 (VIM_ISDIGIT(*str))
	{
	    صحيح	num = atoi((حرف *)str);

	    buf = buflist_findnr(num);
	    𐤀𐤌 (buf == عدم)
		semsg(_(e_buffer_nr_does_not_exist), (long)num);
	}
	وإلا
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	𐤀𐤌 (buf == عدم)
	    أرجع;
	curwin->w_alt_fnum = buf->b_fnum;
	أرجع;
    }

    𐤀𐤌 (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	𐤀𐤌 (p == عدم)
	    أرجع;
	𐤀𐤌 (must_append && expr_line != عدم)
	{
	    s = concat_str(expr_line, p);
	    vim_free(p);
	    p = s;
	}
	set_expr_line(p, عدم);
	أرجع;
    }

    𐤀𐤌 (name == '_')	    // black hole: nothing to do
	أرجع;

    𐤀𐤌 (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	أرجع;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#𐤀𐤌 defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
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

    𐤀𐤌 (y_ptr->y_array == عدم)		// NULL means empty register
	y_ptr->y_size = 0;

    𐤀𐤌 (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    وإلا
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    𐤀𐤌 (str_list)
    {
	لكل (ss = (char_u **) str; *ss != عدم; ++ss)
	    ++newlines;
    }
    وإلا
    {
	لكل (i = 0; i < len; i++)
	    𐤀𐤌 (str[i] == '\n')
		++newlines;
	𐤀𐤌 (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	𐤀𐤌 (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    𐤀𐤌 (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	أرجع;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * حجم(string_T), TRUE);
    𐤀𐤌 (pp == عدم)	// out of memory
	أرجع;
    لكل (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    𐤀𐤌 (str_list)
    {
	لكل (ss = (char_u **) str; *ss != عدم; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    𐤀𐤌 (type == MBLOCK)
	    {
		صحيح charlen = vim_strsize(*ss);

		𐤀𐤌 (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    وإلا
    {
	لكل (start = 0; start < len + extraline; start += i + 1)
	{
	    صحيح charlen = 0;

	    لكل (i = start; i < len;)	// find the end of the line
	    {
		𐤀𐤌 (str[i] == '\n')
		    اكسر;
		𐤀𐤌 (type == MBLOCK)
		{
		    𐤀𐤌 (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    وإلا
			charlen += byte2cells(str[i]);
		}

		𐤀𐤌 (str[i] == NUL)
		    i++; // registers can have NUL chars
		وإلا
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    𐤀𐤌 (charlen > maxlen)
		maxlen = charlen;
	    𐤀𐤌 (append)
	    {
		--lnum;
		extra = (صحيح)y_ptr->y_array[lnum].length;
	    }
	    وإلا
		extra = 0;
	    s = alloc(i + extra + 1);
	    𐤀𐤌 (s == عدم)
		اكسر;
	    𐤀𐤌 (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    𐤀𐤌 (append)
		vim_free(y_ptr->y_array[lnum].string);
	    𐤀𐤌 (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    طالما (--extra >= 0)
	    {
		𐤀𐤌 (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    𐤀𐤌 (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    وإلا
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
