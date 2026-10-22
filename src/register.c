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

#অন্তর্ভাৱ "vim.h"

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
স্থির yankreg_T	y_regs[অঙ্ক_REGISTERS];

স্থির yankreg_T	*y_current;	    // ptr to current yankreg
স্থির পূর্ণাঙ্ক		y_append;	    // TRUE when appending
স্থির yankreg_T	*y_previous = রিক্ত; // ptr to last written yankreg

স্থির পূর্ণাঙ্ক	stuff_yank(পূর্ণাঙ্ক, char_u *);
স্থির শূন্য	put_reedit_in_typebuf(পূর্ণাঙ্ক silent);
স্থির পূর্ণাঙ্ক	put_in_typebuf(char_u *s, পূর্ণাঙ্ক esc, পূর্ণাঙ্ক colon, পূর্ণাঙ্ক silent);
স্থির পূর্ণাঙ্ক	yank_copy_সাৰি(সংরচনা block_def *bd, long y_idx, পূর্ণাঙ্ক exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
স্থির শূন্য	copy_yank_reg(yankreg_T *reg);
#endif
স্থির শূন্য	dis_বাৰ্তা(char_u *p, পূর্ণাঙ্ক skip_esc);

#যদি defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(শূন্য)
{
    ঘূৰাই y_regs;
}
#endif

#যদি defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(পূর্ণাঙ্ক reg)
{
    ঘূৰাই &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(শূন্য)
{
    ঘূৰাই y_current;
}

    yankreg_T *
get_y_previous(শূন্য)
{
    ঘূৰাই y_previous;
}

    শূন্য
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    শূন্য
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    শূন্য
reset_y_append(শূন্য)
{
    y_append = FALSE;
}


#যদি defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
স্থির char_u	*ৰাশিমালা_সাৰি = রিক্ত;
স্থির exarg_T	*ৰাশিমালা_eap = রিক্ত;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    পূর্ণাঙ্ক
get_ৰাশিমালা_register(শূন্য)
{
    char_u	*new_সাৰি;

    new_সাৰি = getcmdline('=', 0L, 0, 0);
    যদি (new_সাৰি == রিক্ত)
	ঘূৰাই NUL;
    যদি (*new_সাৰি == NUL)	// use previous line
	vim_free(new_সাৰি);
    অন্যথা
	set_ৰাশিমালা_সাৰি(new_সাৰি, রিক্ত);
    ঘূৰাই '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    শূন্য
set_ৰাশিমালা_সাৰি(char_u *new_সাৰি, exarg_T *eap)
{
    vim_free(ৰাশিমালা_সাৰি);
    ৰাশিমালা_সাৰি = new_সাৰি;
    ৰাশিমালা_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_ৰাশিমালা_সাৰি(শূন্য)
{
    char_u	*ৰাশিমালা_copy;
    char_u	*rv;
    স্থির পূর্ণাঙ্ক	nested = 0;

    যদি (ৰাশিমালা_সাৰি == রিক্ত)
	ঘূৰাই রিক্ত;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    ৰাশিমালা_copy = vim_strsave(ৰাশিমালা_সাৰি);
    যদি (ৰাশিমালা_copy == রিক্ত)
	ঘূৰাই রিক্ত;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    যদি (nested >= 10)
	ঘূৰাই ৰাশিমালা_copy;

    ++nested;
    rv = eval_to_string_eap(ৰাশিমালা_copy, TRUE, ৰাশিমালা_eap, FALSE);
    --nested;
    vim_free(ৰাশিমালা_copy);
    ঘূৰাই rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    স্থির char_u *
get_ৰাশিমালা_সাৰি_src(শূন্য)
{
    যদি (ৰাশিমালা_সাৰি == রিক্ত)
	ঘূৰাই রিক্ত;
    ঘূৰাই vim_strsave(ৰাশিমালা_সাৰি);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    পূর্ণাঙ্ক
valid_yank_reg(
    পূর্ণাঙ্ক	    regname,
    পূর্ণাঙ্ক	    writing)	    // if TRUE check for writable registers
{
    যদি (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#অন্যথা
				    "/#.%:"
#endif
					, regname) != রিক্ত)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#যদি defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#নোচেদ্যদি defined(FEAT_CLIPBOARD_PROVIDER)
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
	ঘূৰাই TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    অন্যথা যদি (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	বাৰ্তা_warn_missing_clipboard();
	ঘূৰাই FALSE;
    }
#endif
    ঘূৰাই FALSE;
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
    পূর্ণাঙ্ক
get_yank_register(পূর্ণাঙ্ক regname, পূর্ণাঙ্ক writing)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    ret = FALSE;

    y_append = FALSE;
    যদি ((regname == 0 || regname == '"') && !writing && y_previous != রিক্ত)
    {
	y_current = y_previous;
	ঘূৰাই ret;
    }
    i = regname;
    যদি (VIM_ISDIGIT(i))
	i -= '0';
    অন্যথা যদি (ASCII_ISLOWER(i))
	i = Char_Ord_নিচেৰ(i) + 10;
    অন্যথা যদি (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    অন্যথা যদি (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    অন্যথা যদি (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    অন্যথা যদি (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	যদি (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	অন্যথা
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#নোচেদ্যদি defined(FEAT_CLIPBOARD_PROVIDER)
    অন্যথা যদি (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    অন্যথা যদি (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    অন্যথা যদি (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    অন্যথা		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    যদি (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    ঘূৰাই ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    শূন্য *
get_register(
    পূর্ণাঙ্ক		name,
    পূর্ণাঙ্ক		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    পূর্ণাঙ্ক		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	যদি (name == '*' && clip_star.available)
	{
	    যদি (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	যদি (name == '+' && clip_plus.available)
	{
	    যদি (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    যদি (reg == রিক্ত)
	ঘূৰাই (শূন্য *)রিক্ত;

    *reg = *y_current;
    যদি (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	যদি (reg->y_size == 0 || y_current->y_array == রিক্ত)
	    reg->y_array = রিক্ত;
	অন্যথা
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	যদি (reg->y_array != রিক্ত)
	{
	    বাবে (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		যদি (reg->y_array[i].string == রিক্ত)
		{
		    // The allocation failed so clean up and exit
		    যেতিয়ালৈ (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    ঘূৰাই (শূন্য *)রিক্ত;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    অন্যথা
	y_current->y_array = রিক্ত;
    ঘূৰাই (শূন্য *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    শূন্য
put_register(পূর্ণাঙ্ক name, শূন্য *reg)
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

#যদি defined(FEAT_CLIPBOARD)
    শূন্য
free_register(শূন্য *reg)
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
    পূর্ণাঙ্ক
yank_register_mline(পূর্ণাঙ্ক regname)
{
    যদি (regname != 0 && !valid_yank_reg(regname, FALSE))
	ঘূৰাই FALSE;
    যদি (regname == '_')		// black hole is always empty
	ঘূৰাই FALSE;
    get_yank_register(regname, FALSE);
    ঘূৰাই (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    পূর্ণাঙ্ক
do_record(পূর্ণাঙ্ক c)
{
    char_u	    *p;
    স্থির পূর্ণাঙ্ক	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    পূর্ণাঙ্ক		    retval;

    যদি (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	যদি (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	অন্যথা
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    অন্যথা			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	বাৰ্তা("");
	p = get_recorded();
	যদি (p == রিক্ত)
	    retval = FAIL;
	অন্যথা
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
    ঘূৰাই retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    স্থির পূর্ণাঙ্ক
stuff_yank(পূর্ণাঙ্ক regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    যদি (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	ঘূৰাই FAIL;
    }
    যদি (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	ঘূৰাই OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    যদি (y_append && y_current->y_array != রিক্ত)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	যদি (tmp == রিক্ত)
	{
	    vim_free(p);
	    ঘূৰাই FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    অন্যথা
    {
	free_yank_all();
	যদি ((y_current->y_array = ALLOC_ONE(string_T)) == রিক্ত)
	{
	    vim_free(p);
	    ঘূৰাই FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    ঘূৰাই OK;
}

/*
 * Last executed register (@ command)
 */
স্থির পূর্ণাঙ্ক execreg_lastc = NUL;

#যদি defined(FEAT_VIMINFO)
    পূর্ণাঙ্ক
get_execreg_lastc(শূন্য)
{
    ঘূৰাই execreg_lastc;
}

    শূন্য
set_execreg_lastc(পূর্ণাঙ্ক lastc)
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
    স্থির char_u *
execreg_সাৰি_continuation(string_T *সাৰিগুলি, long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    পূর্ণাঙ্ক		j;
    char_u	*str;

    ga_init2(&ga, পরিমাণ(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    যেতিয়ালৈ (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(সাৰিগুলি[cmd_start].string);
	যদি (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    ভঙ্গ;
    }

    // join all the lines
    tmp = &সাৰিগুলি[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    বাবে (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &সাৰিগুলি[j];
	p = skipwhite(tmp->string);
	যদি (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    যদি (ga.ga_len > 400)
	    {
		যদি (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		অন্যথা
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
    ঘূৰাই str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    পূর্ণাঙ্ক
do_execreg(
    পূর্ণাঙ্ক	    regname,
    পূর্ণাঙ্ক	    colon,		// insert ':' before each line
    পূর্ণাঙ্ক	    addcr,		// always add '\n' to end of line
    পূর্ণাঙ্ক	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    পূর্ণাঙ্ক		retval = OK;
    পূর্ণাঙ্ক		remap;

    // repeat previous one
    যদি (regname == '@')
    {
	যদি (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    ঘূৰাই FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    যদি (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	ঘূৰাই FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    যদি (regname == '_')
	ঘূৰাই OK;

    // use last command line
    যদি (regname == ':')
    {
	যদি (last_cmdline == রিক্ত)
	{
	    emsg(_(e_no_previous_command_সাৰি));
	    ঘূৰাই FAIL;
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
	যদি (p != রিক্ত)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    যদি (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    অন্যথা
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    অন্যথা যদি (regname == '=')
    {
	p = get_ৰাশিমালা_সাৰি();
	যদি (p == রিক্ত)
	    ঘূৰাই FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    অন্যথা যদি (regname == '.')		// use last inserted text
    {
	p = get_last_ঢোকাও_save();
	যদি (p == রিক্ত)
	{
	    emsg(_(e_no_inserted_text_yet));
	    ঘূৰাই FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    অন্যথা
    {
	get_yank_register(regname, FALSE);
	যদি (y_current->y_array == রিক্ত)
	    ঘূৰাই FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	বাবে (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    পূর্ণাঙ্ক	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    যদি (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		যদি (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    ঘূৰাই FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    যদি (colon && i > 0)
	    {
		p = skipwhite(str);
		যদি (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_সাৰি_continuation(y_current->y_array, &i);
		    যদি (str == রিক্ত)
			ঘূৰাই FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    যদি (free_str)
		vim_free(str);
	    যদি (escaped == রিক্ত)
		ঘূৰাই FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    যদি (retval == FAIL)
		ঘূৰাই FAIL;
	    যদি (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		ঘূৰাই FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    ঘূৰাই retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    স্থির শূন্য
put_reedit_in_typebuf(পূর্ণাঙ্ক silent)
{
    যদি (restart_edit == NUL)
	ঘূৰাই;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    যদি (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    অন্যথা যদি (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    অন্যথা যদি (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    যদি (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    স্থির পূর্ণাঙ্ক
put_in_typebuf(
    char_u	*s,
    পূর্ণাঙ্ক		esc,
    পূর্ণাঙ্ক		colon,	    // add ':' before the line
    পূর্ণাঙ্ক		silent)
{
    পূর্ণাঙ্ক		retval = OK;

    put_reedit_in_typebuf(silent);
    যদি (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    যদি (retval == OK)
    {
	char_u	*p;

	যদি (esc)
	    p = vim_strsave_escape_csi(s);
	অন্যথা
	    p = s;
	যদি (p == রিক্ত)
	    retval = FAIL;
	অন্যথা
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	যদি (esc)
	    vim_free(p);
    }
    যদি (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    ঘূৰাই retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণাঙ্ক
ঢোকাও_reg(
    পূর্ণাঙ্ক		regname,
    পূর্ণাঙ্ক		literally_arg)	// insert literally, not as if typed
{
    long	i;
    পূর্ণাঙ্ক		retval = OK;
    char_u	*arg;
    পূর্ণাঙ্ক		allocated;
    পূর্ণাঙ্ক		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    যদি (got_int)
	ঘূৰাই FAIL;

    // check for valid regname
    যদি (regname != NUL && !valid_yank_reg(regname, FALSE))
	ঘূৰাই FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    যদি (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    অন্যথা যদি (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	যদি (arg == রিক্ত)
	    ঘূৰাই FAIL;
	stuffescaped(arg, literally);
	যদি (allocated)
	    vim_free(arg);
    }
    অন্যথা				// name or number register
    {
	যদি (get_yank_register(regname, FALSE))
	    literally = TRUE;
	যদি (y_current->y_array == রিক্ত)
	    retval = FAIL;
	অন্যথা
	{
	    বাবে (i = 0; i < y_current->y_size; ++i)
	    {
		যদি (regname == '-' && y_current->y_type == MCHAR)
		{
		    পূর্ণাঙ্ক dir = BACKWARD;
		    যদি ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			যদি (u_save_cursor() == FAIL)
			    ঘূৰাই FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			যদি (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, রিক্ত, dir, 1L, PUT_CURSEND);
		}
		অন্যথা
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    যদি (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    ঘূৰাই retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    পূর্ণাঙ্ক
get_spec_reg(
    পূর্ণাঙ্ক		regname,
    char_u	**argp,
    পূর্ণাঙ্ক		*allocated,	// return: TRUE when value was allocated
    পূর্ণাঙ্ক		errmsg)		// give error message when failing
{
    পূর্ণাঙ্ক		cnt;

    *argp = রিক্ত;
    *allocated = FALSE;
    ৱিকল্প (regname)
    {
	পক্ষ '%':		// file name
	    যদি (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    ঘূৰাই TRUE;

	পক্ষ '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    ঘূৰাই TRUE;

#ifdef FEAT_EVAL
	পক্ষ '=':		// result of expression
	    *argp = get_ৰাশিমালা_সাৰি();
	    *allocated = TRUE;
	    ঘূৰাই TRUE;
#endif

	পক্ষ ':':		// last command line
	    যদি (last_cmdline == রিক্ত && errmsg)
		emsg(_(e_no_previous_command_সাৰি));
	    *argp = last_cmdline;
	    ঘূৰাই TRUE;

	পক্ষ '/':		// last search-pattern
	    যদি (last_অনুসন্ধান_pat() == রিক্ত && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_অনুসন্ধান_pat();
	    ঘূৰাই TRUE;

	পক্ষ '.':		// last inserted text
	    *argp = get_last_ঢোকাও_save();
	    *allocated = TRUE;
	    যদি (*argp == রিক্ত && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    ঘূৰাই TRUE;

	পক্ষ Ctrl_F:		// Filename under cursor
	পক্ষ Ctrl_P:		// Path under cursor, expand via "path"
	    যদি (!errmsg)
		ঘূৰাই FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, রিক্ত);
	    *allocated = TRUE;
	    ঘূৰাই TRUE;

	পক্ষ Ctrl_W:		// word under cursor
	পক্ষ Ctrl_A:		// WORD (mnemonic All) under cursor
	    যদি (!errmsg)
		ঘূৰাই FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : রিক্ত;
	    *allocated = TRUE;
	    ঘূৰাই TRUE;

	পক্ষ Ctrl_L:		// Line under cursor
	    যদি (!errmsg)
		ঘূৰাই FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    ঘূৰাই TRUE;

	পক্ষ '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    ঘূৰাই TRUE;
    }

    ঘূৰাই FALSE;
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
    পূর্ণাঙ্ক
cmdline_paste_reg(
    পূর্ণাঙ্ক regname,
    পূর্ণাঙ্ক literally_arg,	// Insert text literally instead of "as typed"
    পূর্ণাঙ্ক remcr)		// don't add CR characters
{
    long	i;
    পূর্ণাঙ্ক		literally = literally_arg;

    যদি (get_yank_register(regname, FALSE))
	literally = TRUE;
    যদি (y_current->y_array == রিক্ত)
	ঘূৰাই FAIL;

    বাবে (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	যদি ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	যদি (got_int)
	    ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    শূন্য
shift_delete_registers(শূন্য)
{
    পূর্ণাঙ্ক		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    বাবে (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    যদি (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = রিক্ত;		// set register one to empty
}

#যদি defined(FEAT_EVAL)
    স্থির শূন্য
add_regtype_to_dict(পূর্ণাঙ্ক regname, dict_T *dict, char_u *buf, পূর্ণাঙ্ক bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    ৱিকল্প (get_reg_type(regname, &reglen))
    {
	পক্ষ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ভঙ্গ;
	পক্ষ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ভঙ্গ;
	পক্ষ MBLOCK:
	    buflen = vim_snprintf_safelen((ৱর্ণ *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    ভঙ্গ;
	স্ৱভাৱ:
	    buf[0] = NUL;
	    buflen = 0;
	    ভঙ্গ;
    }
    (শূন্য)dict_add_string_len(dict, "regtype", buf, (পূর্ণাঙ্ক)buflen);
}

    শূন্য
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    স্থির পূর্ণাঙ্ক	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    পূর্ণাঙ্ক		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    যদি (recursive)
	ঘূৰাই;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    যদি (list == রিক্ত)
	ঘূৰাই;

    // yanked text contents
    বাবে (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (পূর্ণাঙ্ক)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    যদি (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (শূন্য)dict_add_string_len(v_event, "regname", buf, (পূর্ণাঙ্ক)buflen);

    // motion type: inclusive or exclusive
    (শূন্য)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (শূন্য)dict_add_string_len(v_event, "operator", buf, (পূর্ণাঙ্ক)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, পরিমাণ(buf));

    // selection type - visual or not
    (শূন্য)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_বস্তুসমূহ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, রিক্ত, রিক্ত, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    স্থির শূন্য
put_do_autocmd(
	পূর্ণাঙ্ক	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *ঢোকাও,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	পূর্ণাঙ্ক	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    স্থির bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    পূর্ণাঙ্ক		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    যদি (recursive || regname == '_')
	ঘূৰাই;

    যদি (regname != '.' && ঢোকাও == রিক্ত
	    && reg == রিক্ত)
	// Can happen when pasting text in normal mode in a terminal buffer
	ঘূৰাই;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    যদি (list == রিক্ত)
	ঘূৰাই;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    যদি (regname == '.')
    {
	যদি (last_ঢোকাও_ga.ga_data != রিক্ত)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_ঢোকাও_ga.ga_data,
		    (পূর্ণাঙ্ক)last_ঢোকাও_ga.ga_len);
    }
    অন্যথা যদি (ঢোকাও != রিক্ত)
    {
	list_append_string(list, ঢোকাও->string, (পূর্ণাঙ্ক)ঢোকাও->length);
    }
    অন্যথা
    {
	বাবে (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (পূর্ণাঙ্ক)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    যদি (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (শূন্য)dict_add_string_len(v_event, "regname", buf, (পূর্ণাঙ্ক)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (শূন্য)dict_add_string_len(v_event, "operator", buf, (পূর্ণাঙ্ক)buflen);

    add_regtype_to_dict(regname, v_event, buf, পরিমাণ(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (শূন্য)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_বস্তুসমূহ_ro(v_event);

    recursive = true;
    textlock++;
    যদি (post)
	apply_autocmds(EVENT_TEXTPUTPOST, রিক্ত, রিক্ত, FALSE, curbuf);
    অন্যথা
	apply_autocmds(EVENT_TEXTPUTPRE, রিক্ত, রিক্ত, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    শূন্য
init_yank(শূন্য)
{
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < অঙ্ক_REGISTERS; ++i)
	y_regs[i].y_array = রিক্ত;
}

#যদি defined(EXITFREE)
    শূন্য
clear_registers(শূন্য)
{
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < অঙ্ক_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	যদি (y_current->y_array != রিক্ত)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    স্থির শূন্য
free_yank(long n)
{
    যদি (y_current->y_array == রিক্ত)
	ঘূৰাই;

    long	    i;

    বাবে (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    শূন্য
free_yank_all(শূন্য)
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
    পূর্ণাঙ্ক
op_yank(oparg_T *oap, পূর্ণাঙ্ক deleting, পূর্ণাঙ্ক mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    পূর্ণাঙ্ক			yanktype = oap->motion_type;
    long		yanklines = oap->সাৰি_গুনতি;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    সংরচনা block_def	bd;

				    // check for read-only register
    যদি (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	ঘূৰাই FAIL;
    }
    যদি (oap->regname == '_')	    // black hole: nothing to do
	ঘূৰাই OK;

#ifdef FEAT_CLIPBOARD
    যদি ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	বাৰ্তা_warn_missing_clipboard();
    }
#endif

    যদি (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    যদি (y_append && y_current->y_array != রিক্ত)
	y_current = &newreg;
    অন্যথা
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    যদি (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(পরিমাণ(string_T) * yanklines, TRUE);
    যদি (y_current->y_array == রিক্ত)
    {
	y_current = curr;
	ঘূৰাই FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    যদি (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	যদি (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    বাবে ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	ৱিকল্প (y_current->y_type)
	{
	    পক্ষ MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		যদি (yank_copy_সাৰি(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    গচ্ছ fail;
		ভঙ্গ;

	    পক্ষ MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		যদি ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == রিক্ত)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    গচ্ছ fail;
		}
		ভঙ্গ;

	    পক্ষ MCHAR:
		{
		    পূর্ণাঙ্ক tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (পূর্ণাঙ্ক)STRLEN(bd.textstart);
		    যদি (tmp < bd.textlen)
			bd.textlen = tmp;

		    যদি (yank_copy_সাৰি(&bd, y_idx, FALSE) == FAIL)
			গচ্ছ fail;
		    ভঙ্গ;
		}
		// NOTREACHED
	}
    }

    যদি (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	যদি (new_ptr == রিক্ত)
	    গচ্ছ fail;
	বাবে (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	যদি (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	যদি (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == রিক্ত)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    যদি (pnew == রিক্ত)
	    {
		y_idx = y_current->y_size - 1;
		গচ্ছ fail;
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
	অন্যথা
	    y_idx = 0;
	যেতিয়ালৈ (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    যদি (mess)			// Display message about yank?
    {
	যদি (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	যদি (yanklines > p_report)
	{
	    ৱর্ণ namebuf[100];

	    যদি (oap->regname == NUL)
		*namebuf = NUL;
	    অন্যথা
		vim_snprintf(namebuf, পরিমাণ(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    যদি (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    অন্যথা
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	যদি (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	যদি (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    যদি (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	যদি (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    যদি (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	যদি (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	যদি (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    যদি (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# যদি defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	যদি (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    যদি (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#যদি defined(FEAT_EVAL)
    যদি (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    ঘূৰাই OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    ঘূৰাই FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    স্থির পূর্ণাঙ্ক
yank_copy_সাৰি(সংরচনা block_def *bd, long y_idx, পূর্ণাঙ্ক exclude_trailing_space)
{
    char_u	*pnew;

    যদি (exclude_trailing_space)
	bd->endspaces = 0;
    যদি ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == রিক্ত)
	ঘূৰাই FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    যদি (exclude_trailing_space)
    {
	পূর্ণাঙ্ক s = bd->textlen + bd->endspaces;

	যেতিয়ালৈ (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    ঘূৰাই OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    স্থির শূন্য
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    পরিমাণ(string_T) * y_current->y_size, TRUE);
    যদি (y_current->y_array == রিক্ত)
	y_current->y_size = 0;
    অন্যথা
	বাবে (j = 0; j < y_current->y_size; ++j)
	{
	    যদি ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == রিক্ত)
	    {
		free_yank(j);
		y_current->y_size = 0;
		ভঙ্গ;
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
    শূন্য
do_put(
    পূর্ণাঙ্ক		regname,
    char_u	*ৰাশিমালা_result,	// result for regname "=" when compiled
    পূর্ণাঙ্ক		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	গুনতি,
    পূর্ণাঙ্ক		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    পূর্ণাঙ্ক		yanklen;
    পূর্ণাঙ্ক		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    পূর্ণাঙ্ক		y_type;
    long	y_size;
    পূর্ণাঙ্ক		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = রিক্ত;
    yankreg_T	*y_current_used = রিক্ত;
    long	nr_সাৰিগুলি = 0;
    string_T	ঢোকাও_string;
    পূর্ণাঙ্ক		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned পূর্ণাঙ্ক cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(শূন্য)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    যদি (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    যদি (regname == '.')
    {
	যদি (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	যদি (has_textput_events)
	    add_last_ঢোকাও++;
#endif

	(শূন্য)stuff_inserted((dir == FORWARD ? (গুনতি == -1 ? 'o' : 'a') :
				    (গুনতি == -1 ? 'O' : 'i')), গুনতি, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	যদি (has_textputpre())
	    put_do_autocmd('.', রিক্ত, রিক্ত, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	যদি (has_textputpost())
	    put_do_autocmd('.', রিক্ত, রিক্ত, true, dir);

	যদি (has_textput_events && --add_last_ঢোকাও == 0)
	    ga_clear(&last_ঢোকাও_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	যদি ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	ঘূৰাই;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    ঢোকাও_string.string = রিক্ত;
    যদি (regname == '=' && ৰাশিমালা_result != রিক্ত)
	ঢোকাও_string.string = ৰাশিমালা_result;
    অন্যথা যদি (get_spec_reg(regname, &ঢোকাও_string.string, &allocated, TRUE)
		&& ঢোকাও_string.string == রিক্ত)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	ঘূৰাই;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    যদি (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	গচ্ছ end;

    যদি (ঢোকাও_string.string != রিক্ত)
    {
	ঢোকাও_string.length = STRLEN(ঢোকাও_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	যদি (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    বাবে (;;)
	    {
		y_size = 0;
		ptr = ঢোকাও_string.string;
		ptrlen = ঢোকাও_string.length;
		যেতিয়ালৈ (ptr != রিক্ত)
		{
		    যদি (y_array != রিক্ত)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    যদি (tmp == রিক্ত)
		    {
			যদি (y_array != রিক্ত)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    অন্যথা
		    {
			যদি (y_array != রিক্ত)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			যদি (*tmp == NUL)
			{
			    y_type = MLINE;
			    ভঙ্গ;
			}
		    }
		    ptr = tmp;
		}
		যদি (y_array != রিক্ত)
		    ভঙ্গ;
		y_array = ALLOC_MULT(string_T, y_size);
		যদি (y_array == রিক্ত)
		    গচ্ছ end;
	    }
	}
	অন্যথা
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &ঢোকাও_string;
	}
#ifdef FEAT_EVAL
	যদি (has_textputpre())
	    put_do_autocmd(regname, রিক্ত, &ঢোকাও_string, false, dir);
#endif
    }
    অন্যথা
    {
#ifdef FEAT_EVAL
	যদি (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, রিক্ত, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    যদি (y_type == MLINE)
    {
	যদি (flags & PUT_সাৰি_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    যদি (u_save_cursor() == FAIL)
		গচ্ছ end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    যদি (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    যদি (ptr == রিক্ত)
		গচ্ছ end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    যদি (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    যদি (ptr == রিক্ত)
		গচ্ছ end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_সাৰিগুলি;
	    dir = FORWARD;
	}
	যদি (flags & PUT_সাৰি_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    যদি (flags & PUT_সাৰি)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    যদি (y_size == 0 || y_array == রিক্ত)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	গচ্ছ end;
    }

    যদি (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	যদি (lnum > curbuf->b_ml.ml_সাৰি_গুনতি)
	    lnum = curbuf->b_ml.ml_সাৰি_গুনতি + 1;
	যদি (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    গচ্ছ end;
    }
    অন্যথা যদি (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	যদি (dir == BACKWARD)
	    (শূন্য)hasFolding(lnum, &lnum, রিক্ত);
	অন্যথা
	    (শূন্য)hasFolding(lnum, রিক্ত, &lnum);
#endif
	যদি (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	যদি ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    গচ্ছ end;
#ifdef FEAT_FOLDING
	যদি (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	অন্যথা
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    অন্যথা যদি (u_save_cursor() == FAIL)
	গচ্ছ end;

    যদি (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	যদি (gchar_cursor() == TAB)
	{
	    পূর্ণাঙ্ক viscol = getviscol();
	    পূর্ণাঙ্ক ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    যদি (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#অন্যথা
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    অন্যথা
		curwin->w_cursor.coladd = 0;
	}
	অন্যথা যদি (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    যদি (y_type == MBLOCK)
    {
	পূর্ণাঙ্ক	delcount;
	পূর্ণাঙ্ক	incr = 0;
	সংরচনা block_def bd;
	long	j;
	পূর্ণাঙ্ক	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	যদি (dir == FORWARD && c != NUL)
	{
	    যদি (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, রিক্ত, &endcol2, 0);
	    অন্যথা
		getvcol(curwin, &curwin->w_cursor, রিক্ত, রিক্ত, &col, 0);

	    যদি (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    অন্যথা
		যদি (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	অন্যথা
	    getvcol(curwin, &curwin->w_cursor, &col, রিক্ত, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	যদি (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    যদি (dir == FORWARD && c == NUL)
		++col;
	    যদি (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    যদি (c == TAB)
	    {
		যদি (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		যদি (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	বাবে (i = 0; i < y_size; ++i)
	{
	    পূর্ণাঙ্ক		    spaces = 0;
	    ৱর্ণ	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    যদি (curwin->w_cursor.lnum > curbuf->b_ml.ml_সাৰি_গুনতি)
	    {
		যদি (ml_append(curbuf->b_ml.ml_সাৰি_গুনতি, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    ভঙ্গ;
		++nr_সাৰিগুলি;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    যেতিয়ালৈ (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    যদি (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    অন্যথা যদি (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		যদি (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		যদি (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (পূর্ণাঙ্ক)y_array[i].length;

	    যদি ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		যেতিয়ালৈ (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		যদি (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    যদি (yanklen + spaces != 0
		     && গুনতি > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		ভঙ্গ;
	    }

	    totlen = গুনতি * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    যদি (newp == রিক্ত)
		ভঙ্গ;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    বাবে (j = 0; j < গুনতি; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		যদি ((j < গুনতি - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		অন্যথা
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
	    যদি (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_সাৰিগুলি(lnum, 0, curwin->w_cursor.lnum - nr_সাৰিগুলি, nr_সাৰিগুলি);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	যদি (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	যদি (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    যদি (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	অন্যথা
	    curwin->w_cursor.lnum = lnum;
    }
    অন্যথা
    {
	pos_T	new_cursor;

	yanklen = (পূর্ণাঙ্ক)y_array[0].length;

	// Character or Line mode
	যদি (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    যদি (dir == FORWARD && gchar_cursor() != NUL)
	    {
		যদি (has_mbyte)
		{
		    পূর্ণাঙ্ক bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    যদি (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		অন্যথা
		{
		    ++col;
		    যদি (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	অন্যথা যদি (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	যদি (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    পূর্ণাঙ্ক		first_byte_off = 0;

	    যদি (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		যদি (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		যদি (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, রিক্ত, &vcol, রিক্ত, 0);
		}
	    }

	    যদি (গুনতি == 0 || yanklen == 0)
	    {
		যদি (VIsual_active)
		    lnum = end_lnum;
	    }
	    অন্যথা যদি (গুনতি > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    অন্যথা
	    {
		totlen = গুনতি * yanklen;
		কৰা
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    যদি (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			যদি (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			অন্যথা
			    col = MAXCOL;
		    }
		    যদি (VIsual_active && col > oldlen)
		    {
			lnum++;
			অনুৱর্ত;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    যদি (newp == রিক্ত)
			গচ্ছ end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    বাবে (i = 0; i < গুনতি; ++i)
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
		    যদি (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    যদি (VIsual_active)
			lnum++;
		} যেতিয়ালৈ (VIsual_active && lnum <= end_lnum);

		যদি (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    যদি (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    অন্যথা
		curwin->w_cursor.col -= first_byte_off;
	}
	অন্যথা
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    পূর্ণাঙ্ক		indent;
	    পূর্ণাঙ্ক		orig_indent = 0;
	    পূর্ণাঙ্ক		indent_diff = 0;	// init for gcc
	    পূর্ণাঙ্ক		first_indent = TRUE;
	    পূর্ণাঙ্ক		lendiff = 0;
	    long	cnt;

	    যদি (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    বাবে (cnt = 1; cnt <= গুনতি; ++cnt)
	    {
		i = 0;
		যদি (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (পূর্ণাঙ্ক)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    যদি (newp == রিক্ত)
			গচ্ছ error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    যদি (newp == রিক্ত)
			গচ্ছ error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		বাবে (; i < y_size; ++i)
		{
		    যদি (y_type != MCHAR || i < y_size - 1)
		    {
			যদি (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    গচ্ছ error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_সাৰিগুলি;
		    যদি (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			যদি (cnt == গুনতি && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			যদি (*ptr == '#' && preprocs_বাম())
			    indent = 0;     // Leave # lines at start
			অন্যথা যদি (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			অন্যথা যদি (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			অন্যথা যদি ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(শূন্য)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			যদি (cnt == গুনতি && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		যদি (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    যদি (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		যদি (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_সাৰিগুলি, 0L);

	    // note changed text for displaying and folding
	    যদি (y_type == MCHAR)
		changed_সাৰিগুলি(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_সাৰিগুলি);
	    অন্যথা
		changed_সাৰিগুলি(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_সাৰিগুলি);
	    যদি (y_current_used != রিক্ত && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		গচ্ছ end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    যদি (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		যদি (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    অন্যথা
		curbuf->b_op_end.col = 0;

	    যদি (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    অন্যথা যদি (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		যদি (y_type == MLINE)
		{
		    যদি (lnum >= curbuf->b_ml.ml_সাৰি_গুনতি)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_সাৰি_গুনতি;
		    অন্যথা
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		অন্যথা
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    যদি (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    অন্যথা যদি (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		যদি (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    অন্যথা	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_সাৰিগুলি);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    পূর্ণাঙ্ক len = ml_get_curline_len();
    যদি (curwin->w_cursor.col > len)
    {
	যদি (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    যদি (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    যদি (has_textputpost())
    {
	যদি (ঢোকাও_string.string == রিক্ত)
	    put_do_autocmd(regname, y_current, রিক্ত, true, dir);
	অন্যথা
	    put_do_autocmd(regname, রিক্ত, &ঢোকাও_string, true, dir);
    }
#endif

    যদি (allocated)
	vim_free(ঢোকাও_string.string);
    যদি (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    পূর্ণাঙ্ক
get_register_name(পূর্ণাঙ্ক অঙ্ক)
{
    যদি (অঙ্ক == -1)
	ঘূৰাই '"';
    অন্যথা যদি (অঙ্ক < 10)
	ঘূৰাই অঙ্ক + '0';
    অন্যথা যদি (অঙ্ক == DELETION_REGISTER)
	ঘূৰাই '-';
#যদি defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    অন্যথা যদি (অঙ্ক == STAR_REGISTER)
	ঘূৰাই '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    অন্যথা যদি (clipmethod == CLIPMETHOD_PROVIDER && অঙ্ক == REAL_PLUS_REGISTER)
	ঘূৰাই '+';
    অন্যথা যদি (clipmethod != CLIPMETHOD_PROVIDER && অঙ্ক == PLUS_REGISTER)
	ঘূৰাই '+';
#endif
    অন্যথা
	ঘূৰাই অঙ্ক + 'a' - 10;
}

#যদি defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    পূর্ণাঙ্ক
get_unname_register(শূন্য)
{
    ঘূৰাই y_previous == রিক্ত ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    শূন্য
ex_display(exarg_T *eap)
{
    পূর্ণাঙ্ক		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    পূর্ণাঙ্ক		name;
    পূর্ণাঙ্ক		attr;
    char_u	*arg = eap->arg;
    পূর্ণাঙ্ক		clen;
    পূর্ণাঙ্ক		type;
    string_T	ঢোকাও;

    silence_w23_w24_বাৰ্তা++;
    যদি (arg != রিক্ত && *arg == NUL)
	arg = রিক্ত;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    বাৰ্তা_puts_title(_("\nType Name Content"));
    বাবে (i = -1; i < অঙ্ক_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	ৱিকল্প (get_reg_type(name, রিক্ত))
	{
	    পক্ষ MLINE: type = 'l'; ভঙ্গ;
	    পক্ষ MCHAR: type = 'c'; ভঙ্গ;
	    স্ৱভাৱ:	type = 'b'; ভঙ্গ;
	}
	যদি (arg != রিক্ত && vim_strchr(arg, name) == রিক্ত
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == রিক্ত)
#endif
		)
	    অনুৱর্ত;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	যদি (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (শূন্য)may_get_selection(name);
#endif

	যদি (i == -1)
	{
	    যদি (y_previous != রিক্ত)
		yb = y_previous;
	    অন্যথা
		yb = &(y_regs[0]);
	}
	অন্যথা
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	যদি (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != রিক্ত &&
		    (yb == y_previous || yb == &y_regs[0])))
	    অনুৱর্ত;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	যদি (yb->y_array != রিক্ত)
	{
	    পূর্ণাঙ্ক do_show = FALSE;

	    বাবে (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    যদি (do_show || yb->y_size == 0)
	    {
		বাৰ্তা_putchar('\n');
		বাৰ্তা_puts("  ");
		বাৰ্তা_putchar(type);
		বাৰ্তা_puts("  ");
		বাৰ্তা_putchar('"');
		বাৰ্তা_putchar(name);
		বাৰ্তা_puts("   ");

		n = (পূর্ণাঙ্ক)Columns - 11;
		বাবে (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    যদি (j)
		    {
			বাৰ্তা_puts_attr("^J", attr);
			n -= 2;
		    }
		    বাবে (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			বাৰ্তা_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		যদি (n > 1 && yb->y_type == MLINE)
		    বাৰ্তা_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    ঢোকাও = get_last_ঢোকাও();
    যদি ((p = ঢোকাও.string) != রিক্ত
		  && (arg == রিক্ত || vim_strchr(arg, '.') != রিক্ত) && !got_int
						      && !message_filtered(p))
    {
	বাৰ্তা_puts("\n  c  \".   ");
	dis_বাৰ্তা(p, TRUE);
    }

    // display last command line
    যদি (last_cmdline != রিক্ত && (arg == রিক্ত || vim_strchr(arg, ':') != রিক্ত)
			       && !got_int && !message_filtered(last_cmdline))
    {
	বাৰ্তা_puts("\n  c  \":   ");
	dis_বাৰ্তা(last_cmdline, FALSE);
    }

    // display current file name
    যদি (curbuf->b_fname != রিক্ত
	    && (arg == রিক্ত || vim_strchr(arg, '%') != রিক্ত) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	বাৰ্তা_puts("\n  c  \"%   ");
	dis_বাৰ্তা(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    যদি ((arg == রিক্ত || vim_strchr(arg, '#') != রিক্ত) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	যদি (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    বাৰ্তা_puts("\n  c  \"#   ");
	    dis_বাৰ্তা(fname, FALSE);
	}
    }

    // display last search pattern
    যদি (last_অনুসন্ধান_pat() != রিক্ত
		 && (arg == রিক্ত || vim_strchr(arg, '/') != রিক্ত) && !got_int
				      && !message_filtered(last_অনুসন্ধান_pat()))
    {
	বাৰ্তা_puts("\n  c  \"/   ");
	dis_বাৰ্তা(last_অনুসন্ধান_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    যদি (ৰাশিমালা_সাৰি != রিক্ত && (arg == রিক্ত || vim_strchr(arg, '=') != রিক্ত)
				  && !got_int && !message_filtered(ৰাশিমালা_সাৰি))
    {
	বাৰ্তা_puts("\n  c  \"=   ");
	dis_বাৰ্তা(ৰাশিমালা_সাৰি, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_বাৰ্তা--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    স্থির শূন্য
dis_বাৰ্তা(
    char_u	*p,
    পূর্ণাঙ্ক		skip_esc)	    // if TRUE, ignore trailing ESC
{
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		l;

    n = (পূর্ণাঙ্ক)Columns - 6;
    যেতিয়ালৈ (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    বাৰ্তা_outtrans_len(p, l);
	    p += l;
	}
	অন্যথা
	    বাৰ্তা_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#যদি defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    শূন্য
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
get_reg_type(পূর্ণাঙ্ক regname, long *reglen)
{
    ৱিকল্প (regname)
    {
	পক্ষ '%':		// file name
	পক্ষ '#':		// alternate file name
	পক্ষ '=':		// expression
	পক্ষ ':':		// last command line
	পক্ষ '/':		// last search-pattern
	পক্ষ '.':		// last inserted text
	পক্ষ Ctrl_F:		// Filename under cursor
	পক্ষ Ctrl_P:		// Path under cursor, expand via "path"
	পক্ষ Ctrl_W:		// word under cursor
	পক্ষ Ctrl_A:		// WORD (mnemonic All) under cursor
	পক্ষ '_':		// black hole: always empty
	    ঘূৰাই MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    যদি (regname != NUL && !valid_yank_reg(regname, FALSE))
	ঘূৰাই MAUTO;

    get_yank_register(regname, FALSE);

    যদি (y_current->y_array != রিক্ত)
    {
	যদি (reglen != রিক্ত && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	ঘূৰাই y_current->y_type;
    }
    ঘূৰাই MAUTO;
}

#যদি defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    স্থির char_u *
getreg_wrap_one_সাৰি(char_u *s, পূর্ণাঙ্ক flags)
{
    যদি (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	যদি (list != রিক্ত)
	{
	    যদি (list_append_string(list, রিক্ত, -1) == FAIL)
	    {
		list_free(list);
		ঘূৰাই রিক্ত;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	ঘূৰাই (char_u *)list;
    }
    ঘূৰাই s;
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
get_reg_contents(পূর্ণাঙ্ক regname, পূর্ণাঙ্ক flags)
{
    linenr_T	i;
    char_u	*retval;
    পূর্ণাঙ্ক		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    যদি (regname == '=')
    {
	যদি (flags & GREG_NO_ৰাশিমালা)
	    ঘূৰাই রিক্ত;
	যদি (flags & GREG_ৰাশিমালা_SRC)
	    ঘূৰাই getreg_wrap_one_সাৰি(get_ৰাশিমালা_সাৰি_src(), flags);
	ঘূৰাই getreg_wrap_one_সাৰি(get_ৰাশিমালা_সাৰি(), flags);
    }

    যদি (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    যদি (regname != NUL && !valid_yank_reg(regname, FALSE))
	ঘূৰাই রিক্ত;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    যদি (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	যদি (retval == রিক্ত)
	    ঘূৰাই রিক্ত;
	যদি (allocated)
	    ঘূৰাই getreg_wrap_one_সাৰি(retval, flags);
	ঘূৰাই getreg_wrap_one_সাৰি(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    যদি (y_current->y_array == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	যদি (list == রিক্ত)
	    ঘূৰাই রিক্ত;
	বাবে (i = 0; i < y_current->y_size; ++i)
	{
	    যদি (list_append_string(list, y_current->y_array[i].string,
		(পূর্ণাঙ্ক)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		ঘূৰাই রিক্ত;
	    }
	}
	ঘূৰাই (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    বাবে (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	যদি (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    যদি (retval == রিক্ত)
	ঘূৰাই রিক্ত;

    // Copy the lines of the yank register into the string.
    len = 0;
    বাবে (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	যদি (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    ঘূৰাই retval;
}

    স্থির পূর্ণাঙ্ক
init_write_reg(
    পূর্ণাঙ্ক		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    পূর্ণাঙ্ক		must_append,
    পূর্ণাঙ্ক		*yank_type UNUSED)
{
    যদি (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	ঘূৰাই FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    যদি (!y_append && !must_append)
	free_yank_all();
    ঘূৰাই OK;
}

    স্থির শূন্য
finish_write_reg(
    পূর্ণাঙ্ক		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    যদি (name != '"')
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
    শূন্য
write_reg_contents(
    পূর্ণাঙ্ক		name,
    char_u	*str,
    পূর্ণাঙ্ক		maxlen,
    পূর্ণাঙ্ক		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    শূন্য
write_reg_contents_lst(
    পূর্ণাঙ্ক		name,
    char_u	**strings,
    পূর্ণাঙ্ক		maxlen UNUSED,
    পূর্ণাঙ্ক		must_append,
    পূর্ণাঙ্ক		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    যদি (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	যদি (strings[0] == রিক্ত)
	    s = (char_u *)"";
	অন্যথা যদি (strings[1] != রিক্ত)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_সাৰিগুলি), name);
	    ঘূৰাই;
	}
	অন্যথা
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	ঘূৰাই;
    }

    যদি (name == '_')	    // black hole: nothing to do
	ঘূৰাই;

    যদি (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ঘূৰাই;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    শূন্য
write_reg_contents_ex(
    পূর্ণাঙ্ক		name,
    char_u	*str,
    পূর্ণাঙ্ক		maxlen,
    পূর্ণাঙ্ক		must_append,
    পূর্ণাঙ্ক		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    যদি (maxlen >= 0)
	len = maxlen;
    অন্যথা
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    যদি (name == '/')
    {
	set_last_অনুসন্ধান_pat(str, RE_অনুসন্ধান, TRUE, TRUE);
	ঘূৰাই;
    }

    যদি (name == '#')
    {
	যদি (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  ঘূৰাই;
	}

	buf_T	*buf;

	যদি (VIM_ISDIGIT(*str))
	{
	    পূর্ণাঙ্ক	অঙ্ক = atoi((ৱর্ণ *)str);

	    buf = buflist_findnr(অঙ্ক);
	    যদি (buf == রিক্ত)
		semsg(_(e_buffer_nr_does_not_exist), (long)অঙ্ক);
	}
	অন্যথা
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	যদি (buf == রিক্ত)
	    ঘূৰাই;
	curwin->w_alt_fnum = buf->b_fnum;
	ঘূৰাই;
    }

    যদি (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	যদি (p == রিক্ত)
	    ঘূৰাই;
	যদি (must_append && ৰাশিমালা_সাৰি != রিক্ত)
	{
	    s = concat_str(ৰাশিমালা_সাৰি, p);
	    vim_free(p);
	    p = s;
	}
	set_ৰাশিমালা_সাৰি(p, রিক্ত);
	ঘূৰাই;
    }

    যদি (name == '_')	    // black hole: nothing to do
	ঘূৰাই;

    যদি (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ঘূৰাই;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#যদি defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    শূন্য
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    পূর্ণাঙ্ক		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    পূর্ণাঙ্ক		str_list)	// TRUE if str is char_u **
{
    পূর্ণাঙ্ক		type;			// MCHAR, MLINE or MBLOCK
    পূর্ণাঙ্ক		lnum;
    long	start;
    long	i;
    পূর্ণাঙ্ক		extra;
    পূর্ণাঙ্ক		newlines;		// number of lines added
    পূর্ণাঙ্ক		extraline = 0;		// extra line at the end
    পূর্ণাঙ্ক		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    যদি (y_ptr->y_array == রিক্ত)		// NULL means empty register
	y_ptr->y_size = 0;

    যদি (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    অন্যথা
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    যদি (str_list)
    {
	বাবে (ss = (char_u **) str; *ss != রিক্ত; ++ss)
	    ++newlines;
    }
    অন্যথা
    {
	বাবে (i = 0; i < len; i++)
	    যদি (str[i] == '\n')
		++newlines;
	যদি (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	যদি (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    যদি (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	ঘূৰাই;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * পরিমাণ(string_T), TRUE);
    যদি (pp == রিক্ত)	// out of memory
	ঘূৰাই;
    বাবে (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    যদি (str_list)
    {
	বাবে (ss = (char_u **) str; *ss != রিক্ত; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    যদি (type == MBLOCK)
	    {
		পূর্ণাঙ্ক charlen = vim_strsize(*ss);

		যদি (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    অন্যথা
    {
	বাবে (start = 0; start < len + extraline; start += i + 1)
	{
	    পূর্ণাঙ্ক charlen = 0;

	    বাবে (i = start; i < len;)	// find the end of the line
	    {
		যদি (str[i] == '\n')
		    ভঙ্গ;
		যদি (type == MBLOCK)
		{
		    যদি (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    অন্যথা
			charlen += byte2cells(str[i]);
		}

		যদি (str[i] == NUL)
		    i++; // registers can have NUL chars
		অন্যথা
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    যদি (charlen > maxlen)
		maxlen = charlen;
	    যদি (append)
	    {
		--lnum;
		extra = (পূর্ণাঙ্ক)y_ptr->y_array[lnum].length;
	    }
	    অন্যথা
		extra = 0;
	    s = alloc(i + extra + 1);
	    যদি (s == রিক্ত)
		ভঙ্গ;
	    যদি (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    যদি (append)
		vim_free(y_ptr->y_array[lnum].string);
	    যদি (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    যেতিয়ালৈ (--extra >= 0)
	    {
		যদি (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    যদি (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    অন্যথা
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
