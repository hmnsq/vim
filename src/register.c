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

#شامل "vim.h"

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
ایستا yankreg_T	y_regs[رقم_REGISTERS];

ایستا yankreg_T	*y_current;	    // ptr to current yankreg
ایستا صحیح		y_append;	    // TRUE when appending
ایستا yankreg_T	*y_previous = پوچ; // ptr to last written yankreg

ایستا صحیح	stuff_yank(صحیح, char_u *);
ایستا تهی	put_reedit_in_typebuf(صحیح silent);
ایستا صحیح	put_in_typebuf(char_u *s, صحیح esc, صحیح colon, صحیح silent);
ایستا صحیح	yank_copy_سطر(ساختار block_def *bd, long y_idx, صحیح exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
ایستا تهی	copy_yank_reg(yankreg_T *reg);
#endif
ایستا تهی	dis_پیام(char_u *p, صحیح skip_esc);

#اگر defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(تهی)
{
    بازگشت y_regs;
}
#endif

#اگر defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(صحیح reg)
{
    بازگشت &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(تهی)
{
    بازگشت y_current;
}

    yankreg_T *
get_y_previous(تهی)
{
    بازگشت y_previous;
}

    تهی
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    تهی
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    تهی
reset_y_append(تهی)
{
    y_append = FALSE;
}


#اگر defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
ایستا char_u	*عبارت_سطر = پوچ;
ایستا exarg_T	*عبارت_eap = پوچ;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    صحیح
get_عبارت_register(تهی)
{
    char_u	*new_سطر;

    new_سطر = getcmdline('=', 0L, 0, 0);
    اگر (new_سطر == پوچ)
	بازگشت NUL;
    اگر (*new_سطر == NUL)	// use previous line
	vim_free(new_سطر);
    وگرنه
	set_عبارت_سطر(new_سطر, پوچ);
    بازگشت '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    تهی
set_عبارت_سطر(char_u *new_سطر, exarg_T *eap)
{
    vim_free(عبارت_سطر);
    عبارت_سطر = new_سطر;
    عبارت_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_عبارت_سطر(تهی)
{
    char_u	*عبارت_copy;
    char_u	*rv;
    ایستا صحیح	nested = 0;

    اگر (عبارت_سطر == پوچ)
	بازگشت پوچ;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    عبارت_copy = vim_strsave(عبارت_سطر);
    اگر (عبارت_copy == پوچ)
	بازگشت پوچ;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    اگر (nested >= 10)
	بازگشت عبارت_copy;

    ++nested;
    rv = eval_to_string_eap(عبارت_copy, TRUE, عبارت_eap, FALSE);
    --nested;
    vim_free(عبارت_copy);
    بازگشت rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ایستا char_u *
get_عبارت_سطر_src(تهی)
{
    اگر (عبارت_سطر == پوچ)
	بازگشت پوچ;
    بازگشت vim_strsave(عبارت_سطر);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    صحیح
valid_yank_reg(
    صحیح	    regname,
    صحیح	    writing)	    // if TRUE check for writable registers
{
    اگر (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#وگرنه
				    "/#.%:"
#endif
					, regname) != پوچ)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#اگر defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#وگرنهاگر defined(FEAT_CLIPBOARD_PROVIDER)
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
	بازگشت TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    وگرنه اگر (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	پیام_warn_missing_clipboard();
	بازگشت FALSE;
    }
#endif
    بازگشت FALSE;
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
    صحیح
get_yank_register(صحیح regname, صحیح writing)
{
    صحیح	    i;
    صحیح	    ret = FALSE;

    y_append = FALSE;
    اگر ((regname == 0 || regname == '"') && !writing && y_previous != پوچ)
    {
	y_current = y_previous;
	بازگشت ret;
    }
    i = regname;
    اگر (VIM_ISDIGIT(i))
	i -= '0';
    وگرنه اگر (ASCII_ISLOWER(i))
	i = Char_Ord_کم(i) + 10;
    وگرنه اگر (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    وگرنه اگر (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    وگرنه اگر (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    وگرنه اگر (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	اگر (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	وگرنه
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#وگرنهاگر defined(FEAT_CLIPBOARD_PROVIDER)
    وگرنه اگر (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    وگرنه اگر (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    وگرنه اگر (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    وگرنه		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    اگر (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    بازگشت ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    تهی *
get_register(
    صحیح		name,
    صحیح		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    صحیح		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	اگر (name == '*' && clip_star.available)
	{
	    اگر (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	اگر (name == '+' && clip_plus.available)
	{
	    اگر (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    اگر (reg == پوچ)
	بازگشت (تهی *)پوچ;

    *reg = *y_current;
    اگر (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	اگر (reg->y_size == 0 || y_current->y_array == پوچ)
	    reg->y_array = پوچ;
	وگرنه
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	اگر (reg->y_array != پوچ)
	{
	    برای (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		اگر (reg->y_array[i].string == پوچ)
		{
		    // The allocation failed so clean up and exit
		    وقتی (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    بازگشت (تهی *)پوچ;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    وگرنه
	y_current->y_array = پوچ;
    بازگشت (تهی *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    تهی
put_register(صحیح name, تهی *reg)
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

#اگر defined(FEAT_CLIPBOARD)
    تهی
free_register(تهی *reg)
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
    صحیح
yank_register_mline(صحیح regname)
{
    اگر (regname != 0 && !valid_yank_reg(regname, FALSE))
	بازگشت FALSE;
    اگر (regname == '_')		// black hole is always empty
	بازگشت FALSE;
    get_yank_register(regname, FALSE);
    بازگشت (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
do_record(صحیح c)
{
    char_u	    *p;
    ایستا صحیح	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    صحیح		    retval;

    اگر (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	اگر (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	وگرنه
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    وگرنه			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	پیام("");
	p = get_recorded();
	اگر (p == پوچ)
	    retval = FAIL;
	وگرنه
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
    بازگشت retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    ایستا صحیح
stuff_yank(صحیح regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    اگر (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	بازگشت FAIL;
    }
    اگر (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	بازگشت OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    اگر (y_append && y_current->y_array != پوچ)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	اگر (tmp == پوچ)
	{
	    vim_free(p);
	    بازگشت FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    وگرنه
    {
	free_yank_all();
	اگر ((y_current->y_array = ALLOC_ONE(string_T)) == پوچ)
	{
	    vim_free(p);
	    بازگشت FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    بازگشت OK;
}

/*
 * Last executed register (@ command)
 */
ایستا صحیح execreg_lastc = NUL;

#اگر defined(FEAT_VIMINFO)
    صحیح
get_execreg_lastc(تهی)
{
    بازگشت execreg_lastc;
}

    تهی
set_execreg_lastc(صحیح lastc)
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
    ایستا char_u *
execreg_سطر_continuation(string_T *سطرها, long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    صحیح		j;
    char_u	*str;

    ga_init2(&ga, اندازه(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    وقتی (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(سطرها[cmd_start].string);
	اگر (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    بشکن;
    }

    // join all the lines
    tmp = &سطرها[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    برای (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &سطرها[j];
	p = skipwhite(tmp->string);
	اگر (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    اگر (ga.ga_len > 400)
	    {
		اگر (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		وگرنه
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
    بازگشت str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
do_execreg(
    صحیح	    regname,
    صحیح	    colon,		// insert ':' before each line
    صحیح	    addcr,		// always add '\n' to end of line
    صحیح	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    صحیح		retval = OK;
    صحیح		remap;

    // repeat previous one
    اگر (regname == '@')
    {
	اگر (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    بازگشت FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    اگر (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	بازگشت FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    اگر (regname == '_')
	بازگشت OK;

    // use last command line
    اگر (regname == ':')
    {
	اگر (last_cmdline == پوچ)
	{
	    emsg(_(e_no_previous_command_سطر));
	    بازگشت FAIL;
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
	اگر (p != پوچ)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    اگر (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    وگرنه
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    وگرنه اگر (regname == '=')
    {
	p = get_عبارت_سطر();
	اگر (p == پوچ)
	    بازگشت FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    وگرنه اگر (regname == '.')		// use last inserted text
    {
	p = get_last_درج_save();
	اگر (p == پوچ)
	{
	    emsg(_(e_no_inserted_text_yet));
	    بازگشت FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    وگرنه
    {
	get_yank_register(regname, FALSE);
	اگر (y_current->y_array == پوچ)
	    بازگشت FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	برای (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    صحیح	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    اگر (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		اگر (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    بازگشت FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    اگر (colon && i > 0)
	    {
		p = skipwhite(str);
		اگر (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_سطر_continuation(y_current->y_array, &i);
		    اگر (str == پوچ)
			بازگشت FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    اگر (free_str)
		vim_free(str);
	    اگر (escaped == پوچ)
		بازگشت FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    اگر (retval == FAIL)
		بازگشت FAIL;
	    اگر (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		بازگشت FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    بازگشت retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ایستا تهی
put_reedit_in_typebuf(صحیح silent)
{
    اگر (restart_edit == NUL)
	بازگشت;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    اگر (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    وگرنه اگر (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    وگرنه اگر (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    اگر (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    ایستا صحیح
put_in_typebuf(
    char_u	*s,
    صحیح		esc,
    صحیح		colon,	    // add ':' before the line
    صحیح		silent)
{
    صحیح		retval = OK;

    put_reedit_in_typebuf(silent);
    اگر (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    اگر (retval == OK)
    {
	char_u	*p;

	اگر (esc)
	    p = vim_strsave_escape_csi(s);
	وگرنه
	    p = s;
	اگر (p == پوچ)
	    retval = FAIL;
	وگرنه
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	اگر (esc)
	    vim_free(p);
    }
    اگر (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    بازگشت retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
درج_reg(
    صحیح		regname,
    صحیح		literally_arg)	// insert literally, not as if typed
{
    long	i;
    صحیح		retval = OK;
    char_u	*arg;
    صحیح		allocated;
    صحیح		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    اگر (got_int)
	بازگشت FAIL;

    // check for valid regname
    اگر (regname != NUL && !valid_yank_reg(regname, FALSE))
	بازگشت FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    اگر (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    وگرنه اگر (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	اگر (arg == پوچ)
	    بازگشت FAIL;
	stuffescaped(arg, literally);
	اگر (allocated)
	    vim_free(arg);
    }
    وگرنه				// name or number register
    {
	اگر (get_yank_register(regname, FALSE))
	    literally = TRUE;
	اگر (y_current->y_array == پوچ)
	    retval = FAIL;
	وگرنه
	{
	    برای (i = 0; i < y_current->y_size; ++i)
	    {
		اگر (regname == '-' && y_current->y_type == MCHAR)
		{
		    صحیح dir = BACKWARD;
		    اگر ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			اگر (u_save_cursor() == FAIL)
			    بازگشت FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			اگر (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, پوچ, dir, 1L, PUT_CURSEND);
		}
		وگرنه
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    اگر (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    بازگشت retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    صحیح
get_spec_reg(
    صحیح		regname,
    char_u	**argp,
    صحیح		*allocated,	// return: TRUE when value was allocated
    صحیح		errmsg)		// give error message when failing
{
    صحیح		cnt;

    *argp = پوچ;
    *allocated = FALSE;
    انتخاب (regname)
    {
	حالت '%':		// file name
	    اگر (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    بازگشت TRUE;

	حالت '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    بازگشت TRUE;

#ifdef FEAT_EVAL
	حالت '=':		// result of expression
	    *argp = get_عبارت_سطر();
	    *allocated = TRUE;
	    بازگشت TRUE;
#endif

	حالت ':':		// last command line
	    اگر (last_cmdline == پوچ && errmsg)
		emsg(_(e_no_previous_command_سطر));
	    *argp = last_cmdline;
	    بازگشت TRUE;

	حالت '/':		// last search-pattern
	    اگر (last_جستجو_pat() == پوچ && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_جستجو_pat();
	    بازگشت TRUE;

	حالت '.':		// last inserted text
	    *argp = get_last_درج_save();
	    *allocated = TRUE;
	    اگر (*argp == پوچ && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    بازگشت TRUE;

	حالت Ctrl_F:		// Filename under cursor
	حالت Ctrl_P:		// Path under cursor, expand via "path"
	    اگر (!errmsg)
		بازگشت FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, پوچ);
	    *allocated = TRUE;
	    بازگشت TRUE;

	حالت Ctrl_W:		// word under cursor
	حالت Ctrl_A:		// WORD (mnemonic All) under cursor
	    اگر (!errmsg)
		بازگشت FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : پوچ;
	    *allocated = TRUE;
	    بازگشت TRUE;

	حالت Ctrl_L:		// Line under cursor
	    اگر (!errmsg)
		بازگشت FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    بازگشت TRUE;

	حالت '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    بازگشت TRUE;
    }

    بازگشت FALSE;
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
    صحیح
cmdline_paste_reg(
    صحیح regname,
    صحیح literally_arg,	// Insert text literally instead of "as typed"
    صحیح remcr)		// don't add CR characters
{
    long	i;
    صحیح		literally = literally_arg;

    اگر (get_yank_register(regname, FALSE))
	literally = TRUE;
    اگر (y_current->y_array == پوچ)
	بازگشت FAIL;

    برای (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	اگر ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	اگر (got_int)
	    بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    تهی
shift_delete_registers(تهی)
{
    صحیح		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    برای (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    اگر (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = پوچ;		// set register one to empty
}

#اگر defined(FEAT_EVAL)
    ایستا تهی
add_regtype_to_dict(صحیح regname, dict_T *dict, char_u *buf, صحیح bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    انتخاب (get_reg_type(regname, &reglen))
    {
	حالت MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    بشکن;
	حالت MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    بشکن;
	حالت MBLOCK:
	    buflen = vim_snprintf_safelen((نویسه *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    بشکن;
	پیشفرض:
	    buf[0] = NUL;
	    buflen = 0;
	    بشکن;
    }
    (تهی)dict_add_string_len(dict, "regtype", buf, (صحیح)buflen);
}

    تهی
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    ایستا صحیح	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    صحیح		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    اگر (recursive)
	بازگشت;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    اگر (list == پوچ)
	بازگشت;

    // yanked text contents
    برای (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (صحیح)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    اگر (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (تهی)dict_add_string_len(v_event, "regname", buf, (صحیح)buflen);

    // motion type: inclusive or exclusive
    (تهی)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (تهی)dict_add_string_len(v_event, "operator", buf, (صحیح)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, اندازه(buf));

    // selection type - visual or not
    (تهی)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_اقلام_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, پوچ, پوچ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    ایستا تهی
put_do_autocmd(
	صحیح	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *درج,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	صحیح	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    ایستا bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    صحیح		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    اگر (recursive || regname == '_')
	بازگشت;

    اگر (regname != '.' && درج == پوچ
	    && reg == پوچ)
	// Can happen when pasting text in normal mode in a terminal buffer
	بازگشت;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    اگر (list == پوچ)
	بازگشت;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    اگر (regname == '.')
    {
	اگر (last_درج_ga.ga_data != پوچ)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_درج_ga.ga_data,
		    (صحیح)last_درج_ga.ga_len);
    }
    وگرنه اگر (درج != پوچ)
    {
	list_append_string(list, درج->string, (صحیح)درج->length);
    }
    وگرنه
    {
	برای (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (صحیح)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    اگر (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (تهی)dict_add_string_len(v_event, "regname", buf, (صحیح)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (تهی)dict_add_string_len(v_event, "operator", buf, (صحیح)buflen);

    add_regtype_to_dict(regname, v_event, buf, اندازه(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (تهی)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_اقلام_ro(v_event);

    recursive = true;
    textlock++;
    اگر (post)
	apply_autocmds(EVENT_TEXTPUTPOST, پوچ, پوچ, FALSE, curbuf);
    وگرنه
	apply_autocmds(EVENT_TEXTPUTPRE, پوچ, پوچ, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    تهی
init_yank(تهی)
{
    صحیح		i;

    برای (i = 0; i < رقم_REGISTERS; ++i)
	y_regs[i].y_array = پوچ;
}

#اگر defined(EXITFREE)
    تهی
clear_registers(تهی)
{
    صحیح		i;

    برای (i = 0; i < رقم_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	اگر (y_current->y_array != پوچ)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    ایستا تهی
free_yank(long n)
{
    اگر (y_current->y_array == پوچ)
	بازگشت;

    long	    i;

    برای (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    تهی
free_yank_all(تهی)
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
    صحیح
op_yank(oparg_T *oap, صحیح deleting, صحیح mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    صحیح			yanktype = oap->motion_type;
    long		yanklines = oap->سطر_شمار;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    ساختار block_def	bd;

				    // check for read-only register
    اگر (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	بازگشت FAIL;
    }
    اگر (oap->regname == '_')	    // black hole: nothing to do
	بازگشت OK;

#ifdef FEAT_CLIPBOARD
    اگر ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	پیام_warn_missing_clipboard();
    }
#endif

    اگر (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    اگر (y_append && y_current->y_array != پوچ)
	y_current = &newreg;
    وگرنه
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    اگر (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(اندازه(string_T) * yanklines, TRUE);
    اگر (y_current->y_array == پوچ)
    {
	y_current = curr;
	بازگشت FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    اگر (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	اگر (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    برای ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	انتخاب (y_current->y_type)
	{
	    حالت MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		اگر (yank_copy_سطر(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    برو fail;
		بشکن;

	    حالت MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		اگر ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == پوچ)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    برو fail;
		}
		بشکن;

	    حالت MCHAR:
		{
		    صحیح tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (صحیح)STRLEN(bd.textstart);
		    اگر (tmp < bd.textlen)
			bd.textlen = tmp;

		    اگر (yank_copy_سطر(&bd, y_idx, FALSE) == FAIL)
			برو fail;
		    بشکن;
		}
		// NOTREACHED
	}
    }

    اگر (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	اگر (new_ptr == پوچ)
	    برو fail;
	برای (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	اگر (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	اگر (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == پوچ)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    اگر (pnew == پوچ)
	    {
		y_idx = y_current->y_size - 1;
		برو fail;
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
	وگرنه
	    y_idx = 0;
	وقتی (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    اگر (mess)			// Display message about yank?
    {
	اگر (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	اگر (yanklines > p_report)
	{
	    نویسه namebuf[100];

	    اگر (oap->regname == NUL)
		*namebuf = NUL;
	    وگرنه
		vim_snprintf(namebuf, اندازه(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    اگر (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    وگرنه
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	اگر (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	اگر (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    اگر (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	اگر (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    اگر (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	اگر (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	اگر (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    اگر (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# اگر defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	اگر (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    اگر (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#اگر defined(FEAT_EVAL)
    اگر (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    بازگشت OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    بازگشت FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    ایستا صحیح
yank_copy_سطر(ساختار block_def *bd, long y_idx, صحیح exclude_trailing_space)
{
    char_u	*pnew;

    اگر (exclude_trailing_space)
	bd->endspaces = 0;
    اگر ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == پوچ)
	بازگشت FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    اگر (exclude_trailing_space)
    {
	صحیح s = bd->textlen + bd->endspaces;

	وقتی (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    بازگشت OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    ایستا تهی
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    اندازه(string_T) * y_current->y_size, TRUE);
    اگر (y_current->y_array == پوچ)
	y_current->y_size = 0;
    وگرنه
	برای (j = 0; j < y_current->y_size; ++j)
	{
	    اگر ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == پوچ)
	    {
		free_yank(j);
		y_current->y_size = 0;
		بشکن;
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
    تهی
do_put(
    صحیح		regname,
    char_u	*عبارت_result,	// result for regname "=" when compiled
    صحیح		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	شمار,
    صحیح		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    صحیح		yanklen;
    صحیح		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    صحیح		y_type;
    long	y_size;
    صحیح		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = پوچ;
    yankreg_T	*y_current_used = پوچ;
    long	nr_سطرها = 0;
    string_T	درج_string;
    صحیح		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned صحیح cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(تهی)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    اگر (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    اگر (regname == '.')
    {
	اگر (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	اگر (has_textput_events)
	    add_last_درج++;
#endif

	(تهی)stuff_inserted((dir == FORWARD ? (شمار == -1 ? 'o' : 'a') :
				    (شمار == -1 ? 'O' : 'i')), شمار, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	اگر (has_textputpre())
	    put_do_autocmd('.', پوچ, پوچ, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	اگر (has_textputpost())
	    put_do_autocmd('.', پوچ, پوچ, true, dir);

	اگر (has_textput_events && --add_last_درج == 0)
	    ga_clear(&last_درج_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	اگر ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	بازگشت;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    درج_string.string = پوچ;
    اگر (regname == '=' && عبارت_result != پوچ)
	درج_string.string = عبارت_result;
    وگرنه اگر (get_spec_reg(regname, &درج_string.string, &allocated, TRUE)
		&& درج_string.string == پوچ)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	بازگشت;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    اگر (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	برو end;

    اگر (درج_string.string != پوچ)
    {
	درج_string.length = STRLEN(درج_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	اگر (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    برای (;;)
	    {
		y_size = 0;
		ptr = درج_string.string;
		ptrlen = درج_string.length;
		وقتی (ptr != پوچ)
		{
		    اگر (y_array != پوچ)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    اگر (tmp == پوچ)
		    {
			اگر (y_array != پوچ)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    وگرنه
		    {
			اگر (y_array != پوچ)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			اگر (*tmp == NUL)
			{
			    y_type = MLINE;
			    بشکن;
			}
		    }
		    ptr = tmp;
		}
		اگر (y_array != پوچ)
		    بشکن;
		y_array = ALLOC_MULT(string_T, y_size);
		اگر (y_array == پوچ)
		    برو end;
	    }
	}
	وگرنه
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &درج_string;
	}
#ifdef FEAT_EVAL
	اگر (has_textputpre())
	    put_do_autocmd(regname, پوچ, &درج_string, false, dir);
#endif
    }
    وگرنه
    {
#ifdef FEAT_EVAL
	اگر (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, پوچ, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    اگر (y_type == MLINE)
    {
	اگر (flags & PUT_سطر_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    اگر (u_save_cursor() == FAIL)
		برو end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    اگر (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    اگر (ptr == پوچ)
		برو end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    اگر (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    اگر (ptr == پوچ)
		برو end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_سطرها;
	    dir = FORWARD;
	}
	اگر (flags & PUT_سطر_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    اگر (flags & PUT_سطر)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    اگر (y_size == 0 || y_array == پوچ)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	برو end;
    }

    اگر (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	اگر (lnum > curbuf->b_ml.ml_سطر_شمار)
	    lnum = curbuf->b_ml.ml_سطر_شمار + 1;
	اگر (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    برو end;
    }
    وگرنه اگر (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	اگر (dir == BACKWARD)
	    (تهی)hasFolding(lnum, &lnum, پوچ);
	وگرنه
	    (تهی)hasFolding(lnum, پوچ, &lnum);
#endif
	اگر (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	اگر ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    برو end;
#ifdef FEAT_FOLDING
	اگر (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	وگرنه
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    وگرنه اگر (u_save_cursor() == FAIL)
	برو end;

    اگر (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	اگر (gchar_cursor() == TAB)
	{
	    صحیح viscol = getviscol();
	    صحیح ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    اگر (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#وگرنه
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    وگرنه
		curwin->w_cursor.coladd = 0;
	}
	وگرنه اگر (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    اگر (y_type == MBLOCK)
    {
	صحیح	delcount;
	صحیح	incr = 0;
	ساختار block_def bd;
	long	j;
	صحیح	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	اگر (dir == FORWARD && c != NUL)
	{
	    اگر (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, پوچ, &endcol2, 0);
	    وگرنه
		getvcol(curwin, &curwin->w_cursor, پوچ, پوچ, &col, 0);

	    اگر (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    وگرنه
		اگر (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	وگرنه
	    getvcol(curwin, &curwin->w_cursor, &col, پوچ, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	اگر (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    اگر (dir == FORWARD && c == NUL)
		++col;
	    اگر (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    اگر (c == TAB)
	    {
		اگر (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		اگر (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	برای (i = 0; i < y_size; ++i)
	{
	    صحیح		    spaces = 0;
	    نویسه	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_شمار)
	    {
		اگر (ml_append(curbuf->b_ml.ml_سطر_شمار, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    بشکن;
		++nr_سطرها;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    وقتی (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    اگر (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    وگرنه اگر (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		اگر (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		اگر (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (صحیح)y_array[i].length;

	    اگر ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		وقتی (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		اگر (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    اگر (yanklen + spaces != 0
		     && شمار > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		بشکن;
	    }

	    totlen = شمار * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    اگر (newp == پوچ)
		بشکن;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    برای (j = 0; j < شمار; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		اگر ((j < شمار - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		وگرنه
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
	    اگر (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_سطرها(lnum, 0, curwin->w_cursor.lnum - nr_سطرها, nr_سطرها);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	اگر (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	اگر (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    اگر (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	وگرنه
	    curwin->w_cursor.lnum = lnum;
    }
    وگرنه
    {
	pos_T	new_cursor;

	yanklen = (صحیح)y_array[0].length;

	// Character or Line mode
	اگر (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    اگر (dir == FORWARD && gchar_cursor() != NUL)
	    {
		اگر (has_mbyte)
		{
		    صحیح bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    اگر (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		وگرنه
		{
		    ++col;
		    اگر (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	وگرنه اگر (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	اگر (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    صحیح		first_byte_off = 0;

	    اگر (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		اگر (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		اگر (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, پوچ, &vcol, پوچ, 0);
		}
	    }

	    اگر (شمار == 0 || yanklen == 0)
	    {
		اگر (VIsual_active)
		    lnum = end_lnum;
	    }
	    وگرنه اگر (شمار > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    وگرنه
	    {
		totlen = شمار * yanklen;
		انجامبده
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    اگر (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			اگر (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			وگرنه
			    col = MAXCOL;
		    }
		    اگر (VIsual_active && col > oldlen)
		    {
			lnum++;
			ادامه;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    اگر (newp == پوچ)
			برو end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    برای (i = 0; i < شمار; ++i)
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
		    اگر (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    اگر (VIsual_active)
			lnum++;
		} وقتی (VIsual_active && lnum <= end_lnum);

		اگر (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    اگر (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    وگرنه
		curwin->w_cursor.col -= first_byte_off;
	}
	وگرنه
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    صحیح		indent;
	    صحیح		orig_indent = 0;
	    صحیح		indent_diff = 0;	// init for gcc
	    صحیح		first_indent = TRUE;
	    صحیح		lendiff = 0;
	    long	cnt;

	    اگر (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    برای (cnt = 1; cnt <= شمار; ++cnt)
	    {
		i = 0;
		اگر (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (صحیح)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    اگر (newp == پوچ)
			برو error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    اگر (newp == پوچ)
			برو error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		برای (; i < y_size; ++i)
		{
		    اگر (y_type != MCHAR || i < y_size - 1)
		    {
			اگر (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    برو error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_سطرها;
		    اگر (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			اگر (cnt == شمار && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			اگر (*ptr == '#' && preprocs_چپ())
			    indent = 0;     // Leave # lines at start
			وگرنه اگر (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			وگرنه اگر (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			وگرنه اگر ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(تهی)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			اگر (cnt == شمار && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		اگر (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    اگر (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		اگر (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_سطرها, 0L);

	    // note changed text for displaying and folding
	    اگر (y_type == MCHAR)
		changed_سطرها(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_سطرها);
	    وگرنه
		changed_سطرها(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_سطرها);
	    اگر (y_current_used != پوچ && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		برو end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    اگر (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		اگر (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    وگرنه
		curbuf->b_op_end.col = 0;

	    اگر (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    وگرنه اگر (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		اگر (y_type == MLINE)
		{
		    اگر (lnum >= curbuf->b_ml.ml_سطر_شمار)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
		    وگرنه
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		وگرنه
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    اگر (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    وگرنه اگر (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		اگر (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    وگرنه	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_سطرها);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    صحیح len = ml_get_curline_len();
    اگر (curwin->w_cursor.col > len)
    {
	اگر (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    اگر (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    اگر (has_textputpost())
    {
	اگر (درج_string.string == پوچ)
	    put_do_autocmd(regname, y_current, پوچ, true, dir);
	وگرنه
	    put_do_autocmd(regname, پوچ, &درج_string, true, dir);
    }
#endif

    اگر (allocated)
	vim_free(درج_string.string);
    اگر (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    صحیح
get_register_name(صحیح رقم)
{
    اگر (رقم == -1)
	بازگشت '"';
    وگرنه اگر (رقم < 10)
	بازگشت رقم + '0';
    وگرنه اگر (رقم == DELETION_REGISTER)
	بازگشت '-';
#اگر defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    وگرنه اگر (رقم == STAR_REGISTER)
	بازگشت '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    وگرنه اگر (clipmethod == CLIPMETHOD_PROVIDER && رقم == REAL_PLUS_REGISTER)
	بازگشت '+';
    وگرنه اگر (clipmethod != CLIPMETHOD_PROVIDER && رقم == PLUS_REGISTER)
	بازگشت '+';
#endif
    وگرنه
	بازگشت رقم + 'a' - 10;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    صحیح
get_unname_register(تهی)
{
    بازگشت y_previous == پوچ ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    تهی
ex_display(exarg_T *eap)
{
    صحیح		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    صحیح		name;
    صحیح		attr;
    char_u	*arg = eap->arg;
    صحیح		clen;
    صحیح		type;
    string_T	درج;

    silence_w23_w24_پیام++;
    اگر (arg != پوچ && *arg == NUL)
	arg = پوچ;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    پیام_puts_title(_("\nType Name Content"));
    برای (i = -1; i < رقم_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	انتخاب (get_reg_type(name, پوچ))
	{
	    حالت MLINE: type = 'l'; بشکن;
	    حالت MCHAR: type = 'c'; بشکن;
	    پیشفرض:	type = 'b'; بشکن;
	}
	اگر (arg != پوچ && vim_strchr(arg, name) == پوچ
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == پوچ)
#endif
		)
	    ادامه;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	اگر (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (تهی)may_get_selection(name);
#endif

	اگر (i == -1)
	{
	    اگر (y_previous != پوچ)
		yb = y_previous;
	    وگرنه
		yb = &(y_regs[0]);
	}
	وگرنه
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	اگر (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != پوچ &&
		    (yb == y_previous || yb == &y_regs[0])))
	    ادامه;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	اگر (yb->y_array != پوچ)
	{
	    صحیح do_show = FALSE;

	    برای (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    اگر (do_show || yb->y_size == 0)
	    {
		پیام_putchar('\n');
		پیام_puts("  ");
		پیام_putchar(type);
		پیام_puts("  ");
		پیام_putchar('"');
		پیام_putchar(name);
		پیام_puts("   ");

		n = (صحیح)Columns - 11;
		برای (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    اگر (j)
		    {
			پیام_puts_attr("^J", attr);
			n -= 2;
		    }
		    برای (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			پیام_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		اگر (n > 1 && yb->y_type == MLINE)
		    پیام_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    درج = get_last_درج();
    اگر ((p = درج.string) != پوچ
		  && (arg == پوچ || vim_strchr(arg, '.') != پوچ) && !got_int
						      && !message_filtered(p))
    {
	پیام_puts("\n  c  \".   ");
	dis_پیام(p, TRUE);
    }

    // display last command line
    اگر (last_cmdline != پوچ && (arg == پوچ || vim_strchr(arg, ':') != پوچ)
			       && !got_int && !message_filtered(last_cmdline))
    {
	پیام_puts("\n  c  \":   ");
	dis_پیام(last_cmdline, FALSE);
    }

    // display current file name
    اگر (curbuf->b_fname != پوچ
	    && (arg == پوچ || vim_strchr(arg, '%') != پوچ) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	پیام_puts("\n  c  \"%   ");
	dis_پیام(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    اگر ((arg == پوچ || vim_strchr(arg, '#') != پوچ) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	اگر (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    پیام_puts("\n  c  \"#   ");
	    dis_پیام(fname, FALSE);
	}
    }

    // display last search pattern
    اگر (last_جستجو_pat() != پوچ
		 && (arg == پوچ || vim_strchr(arg, '/') != پوچ) && !got_int
				      && !message_filtered(last_جستجو_pat()))
    {
	پیام_puts("\n  c  \"/   ");
	dis_پیام(last_جستجو_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    اگر (عبارت_سطر != پوچ && (arg == پوچ || vim_strchr(arg, '=') != پوچ)
				  && !got_int && !message_filtered(عبارت_سطر))
    {
	پیام_puts("\n  c  \"=   ");
	dis_پیام(عبارت_سطر, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_پیام--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    ایستا تهی
dis_پیام(
    char_u	*p,
    صحیح		skip_esc)	    // if TRUE, ignore trailing ESC
{
    صحیح		n;
    صحیح		l;

    n = (صحیح)Columns - 6;
    وقتی (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    پیام_outtrans_len(p, l);
	    p += l;
	}
	وگرنه
	    پیام_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#اگر defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    تهی
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
get_reg_type(صحیح regname, long *reglen)
{
    انتخاب (regname)
    {
	حالت '%':		// file name
	حالت '#':		// alternate file name
	حالت '=':		// expression
	حالت ':':		// last command line
	حالت '/':		// last search-pattern
	حالت '.':		// last inserted text
	حالت Ctrl_F:		// Filename under cursor
	حالت Ctrl_P:		// Path under cursor, expand via "path"
	حالت Ctrl_W:		// word under cursor
	حالت Ctrl_A:		// WORD (mnemonic All) under cursor
	حالت '_':		// black hole: always empty
	    بازگشت MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    اگر (regname != NUL && !valid_yank_reg(regname, FALSE))
	بازگشت MAUTO;

    get_yank_register(regname, FALSE);

    اگر (y_current->y_array != پوچ)
    {
	اگر (reglen != پوچ && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	بازگشت y_current->y_type;
    }
    بازگشت MAUTO;
}

#اگر defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    ایستا char_u *
getreg_wrap_one_سطر(char_u *s, صحیح flags)
{
    اگر (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	اگر (list != پوچ)
	{
	    اگر (list_append_string(list, پوچ, -1) == FAIL)
	    {
		list_free(list);
		بازگشت پوچ;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	بازگشت (char_u *)list;
    }
    بازگشت s;
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
get_reg_contents(صحیح regname, صحیح flags)
{
    linenr_T	i;
    char_u	*retval;
    صحیح		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    اگر (regname == '=')
    {
	اگر (flags & GREG_NO_عبارت)
	    بازگشت پوچ;
	اگر (flags & GREG_عبارت_SRC)
	    بازگشت getreg_wrap_one_سطر(get_عبارت_سطر_src(), flags);
	بازگشت getreg_wrap_one_سطر(get_عبارت_سطر(), flags);
    }

    اگر (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    اگر (regname != NUL && !valid_yank_reg(regname, FALSE))
	بازگشت پوچ;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    اگر (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	اگر (retval == پوچ)
	    بازگشت پوچ;
	اگر (allocated)
	    بازگشت getreg_wrap_one_سطر(retval, flags);
	بازگشت getreg_wrap_one_سطر(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    اگر (y_current->y_array == پوچ)
	بازگشت پوچ;

    اگر (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	اگر (list == پوچ)
	    بازگشت پوچ;
	برای (i = 0; i < y_current->y_size; ++i)
	{
	    اگر (list_append_string(list, y_current->y_array[i].string,
		(صحیح)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		بازگشت پوچ;
	    }
	}
	بازگشت (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    برای (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	اگر (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    اگر (retval == پوچ)
	بازگشت پوچ;

    // Copy the lines of the yank register into the string.
    len = 0;
    برای (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	اگر (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    بازگشت retval;
}

    ایستا صحیح
init_write_reg(
    صحیح		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    صحیح		must_append,
    صحیح		*yank_type UNUSED)
{
    اگر (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	بازگشت FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    اگر (!y_append && !must_append)
	free_yank_all();
    بازگشت OK;
}

    ایستا تهی
finish_write_reg(
    صحیح		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    اگر (name != '"')
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
    تهی
write_reg_contents(
    صحیح		name,
    char_u	*str,
    صحیح		maxlen,
    صحیح		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    تهی
write_reg_contents_lst(
    صحیح		name,
    char_u	**strings,
    صحیح		maxlen UNUSED,
    صحیح		must_append,
    صحیح		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    اگر (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	اگر (strings[0] == پوچ)
	    s = (char_u *)"";
	وگرنه اگر (strings[1] != پوچ)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_سطرها), name);
	    بازگشت;
	}
	وگرنه
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	بازگشت;
    }

    اگر (name == '_')	    // black hole: nothing to do
	بازگشت;

    اگر (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	بازگشت;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    تهی
write_reg_contents_ex(
    صحیح		name,
    char_u	*str,
    صحیح		maxlen,
    صحیح		must_append,
    صحیح		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    اگر (maxlen >= 0)
	len = maxlen;
    وگرنه
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    اگر (name == '/')
    {
	set_last_جستجو_pat(str, RE_جستجو, TRUE, TRUE);
	بازگشت;
    }

    اگر (name == '#')
    {
	اگر (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  بازگشت;
	}

	buf_T	*buf;

	اگر (VIM_ISDIGIT(*str))
	{
	    صحیح	رقم = atoi((نویسه *)str);

	    buf = buflist_findnr(رقم);
	    اگر (buf == پوچ)
		semsg(_(e_buffer_nr_does_not_exist), (long)رقم);
	}
	وگرنه
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	اگر (buf == پوچ)
	    بازگشت;
	curwin->w_alt_fnum = buf->b_fnum;
	بازگشت;
    }

    اگر (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	اگر (p == پوچ)
	    بازگشت;
	اگر (must_append && عبارت_سطر != پوچ)
	{
	    s = concat_str(عبارت_سطر, p);
	    vim_free(p);
	    p = s;
	}
	set_عبارت_سطر(p, پوچ);
	بازگشت;
    }

    اگر (name == '_')	    // black hole: nothing to do
	بازگشت;

    اگر (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	بازگشت;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#اگر defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    تهی
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    صحیح		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    صحیح		str_list)	// TRUE if str is char_u **
{
    صحیح		type;			// MCHAR, MLINE or MBLOCK
    صحیح		lnum;
    long	start;
    long	i;
    صحیح		extra;
    صحیح		newlines;		// number of lines added
    صحیح		extraline = 0;		// extra line at the end
    صحیح		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    اگر (y_ptr->y_array == پوچ)		// NULL means empty register
	y_ptr->y_size = 0;

    اگر (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    وگرنه
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    اگر (str_list)
    {
	برای (ss = (char_u **) str; *ss != پوچ; ++ss)
	    ++newlines;
    }
    وگرنه
    {
	برای (i = 0; i < len; i++)
	    اگر (str[i] == '\n')
		++newlines;
	اگر (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	اگر (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    اگر (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	بازگشت;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * اندازه(string_T), TRUE);
    اگر (pp == پوچ)	// out of memory
	بازگشت;
    برای (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    اگر (str_list)
    {
	برای (ss = (char_u **) str; *ss != پوچ; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    اگر (type == MBLOCK)
	    {
		صحیح charlen = vim_strsize(*ss);

		اگر (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    وگرنه
    {
	برای (start = 0; start < len + extraline; start += i + 1)
	{
	    صحیح charlen = 0;

	    برای (i = start; i < len;)	// find the end of the line
	    {
		اگر (str[i] == '\n')
		    بشکن;
		اگر (type == MBLOCK)
		{
		    اگر (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    وگرنه
			charlen += byte2cells(str[i]);
		}

		اگر (str[i] == NUL)
		    i++; // registers can have NUL chars
		وگرنه
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    اگر (charlen > maxlen)
		maxlen = charlen;
	    اگر (append)
	    {
		--lnum;
		extra = (صحیح)y_ptr->y_array[lnum].length;
	    }
	    وگرنه
		extra = 0;
	    s = alloc(i + extra + 1);
	    اگر (s == پوچ)
		بشکن;
	    اگر (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    اگر (append)
		vim_free(y_ptr->y_array[lnum].string);
	    اگر (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    وقتی (--extra >= 0)
	    {
		اگر (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    اگر (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    وگرنه
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
