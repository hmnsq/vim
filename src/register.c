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

#அந்தர்பாவ "vim.h"

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
நிலையான yankreg_T	y_regs[இலக்கம்_REGISTERS];

நிலையான yankreg_T	*y_current;	    // ptr to current yankreg
நிலையான முழு		y_append;	    // TRUE when appending
நிலையான yankreg_T	*y_previous = வெற்று; // ptr to last written yankreg

நிலையான முழு	stuff_yank(முழு, char_u *);
நிலையான வெறுமை	put_reedit_in_typebuf(முழு silent);
நிலையான முழு	put_in_typebuf(char_u *s, முழு esc, முழு colon, முழு silent);
நிலையான முழு	yank_copy_வரி(கட்டமைப்பு block_def *bd, long y_idx, முழு exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
நிலையான வெறுமை	copy_yank_reg(yankreg_T *reg);
#endif
நிலையான வெறுமை	dis_செய்தி(char_u *p, முழு skip_esc);

#எனில் defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(வெறுமை)
{
    திரும்பு y_regs;
}
#endif

#எனில் defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(முழு reg)
{
    திரும்பு &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(வெறுமை)
{
    திரும்பு y_current;
}

    yankreg_T *
get_y_previous(வெறுமை)
{
    திரும்பு y_previous;
}

    வெறுமை
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    வெறுமை
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    வெறுமை
reset_y_append(வெறுமை)
{
    y_append = FALSE;
}


#எனில் defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
நிலையான char_u	*கோவை_வரி = வெற்று;
நிலையான exarg_T	*கோவை_eap = வெற்று;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    முழு
get_கோவை_register(வெறுமை)
{
    char_u	*new_வரி;

    new_வரி = getcmdline('=', 0L, 0, 0);
    எனில் (new_வரி == வெற்று)
	திரும்பு NUL;
    எனில் (*new_வரி == NUL)	// use previous line
	vim_free(new_வரி);
    இல்லையெனில்
	set_கோவை_வரி(new_வரி, வெற்று);
    திரும்பு '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    வெறுமை
set_கோவை_வரி(char_u *new_வரி, exarg_T *eap)
{
    vim_free(கோவை_வரி);
    கோவை_வரி = new_வரி;
    கோவை_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_கோவை_வரி(வெறுமை)
{
    char_u	*கோவை_copy;
    char_u	*rv;
    நிலையான முழு	nested = 0;

    எனில் (கோவை_வரி == வெற்று)
	திரும்பு வெற்று;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    கோவை_copy = vim_strsave(கோவை_வரி);
    எனில் (கோவை_copy == வெற்று)
	திரும்பு வெற்று;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    எனில் (nested >= 10)
	திரும்பு கோவை_copy;

    ++nested;
    rv = eval_to_string_eap(கோவை_copy, TRUE, கோவை_eap, FALSE);
    --nested;
    vim_free(கோவை_copy);
    திரும்பு rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    நிலையான char_u *
get_கோவை_வரி_src(வெறுமை)
{
    எனில் (கோவை_வரி == வெற்று)
	திரும்பு வெற்று;
    திரும்பு vim_strsave(கோவை_வரி);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    முழு
valid_yank_reg(
    முழு	    regname,
    முழு	    writing)	    // if TRUE check for writable registers
{
    எனில் (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#இல்லையெனில்
				    "/#.%:"
#endif
					, regname) != வெற்று)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#எனில் defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#அல்லதெனில் defined(FEAT_CLIPBOARD_PROVIDER)
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
	திரும்பு TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    இல்லையெனில் எனில் (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	செய்தி_warn_missing_clipboard();
	திரும்பு FALSE;
    }
#endif
    திரும்பு FALSE;
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
    முழு
get_yank_register(முழு regname, முழு writing)
{
    முழு	    i;
    முழு	    ret = FALSE;

    y_append = FALSE;
    எனில் ((regname == 0 || regname == '"') && !writing && y_previous != வெற்று)
    {
	y_current = y_previous;
	திரும்பு ret;
    }
    i = regname;
    எனில் (VIM_ISDIGIT(i))
	i -= '0';
    இல்லையெனில் எனில் (ASCII_ISLOWER(i))
	i = Char_Ord_கீழ்(i) + 10;
    இல்லையெனில் எனில் (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    இல்லையெனில் எனில் (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    இல்லையெனில் எனில் (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    இல்லையெனில் எனில் (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	எனில் (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	இல்லையெனில்
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#அல்லதெனில் defined(FEAT_CLIPBOARD_PROVIDER)
    இல்லையெனில் எனில் (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    இல்லையெனில் எனில் (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    இல்லையெனில் எனில் (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    இல்லையெனில்		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    எனில் (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    திரும்பு ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    வெறுமை *
get_register(
    முழு		name,
    முழு		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    முழு		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    எனில் (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	எனில் (name == '*' && clip_star.available)
	{
	    எனில் (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	எனில் (name == '+' && clip_plus.available)
	{
	    எனில் (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    எனில் (reg == வெற்று)
	திரும்பு (வெறுமை *)வெற்று;

    *reg = *y_current;
    எனில் (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	எனில் (reg->y_size == 0 || y_current->y_array == வெற்று)
	    reg->y_array = வெற்று;
	இல்லையெனில்
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	எனில் (reg->y_array != வெற்று)
	{
	    க்கு (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		எனில் (reg->y_array[i].string == வெற்று)
		{
		    // The allocation failed so clean up and exit
		    போது (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    திரும்பு (வெறுமை *)வெற்று;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    இல்லையெனில்
	y_current->y_array = வெற்று;
    திரும்பு (வெறுமை *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    வெறுமை
put_register(முழு name, வெறுமை *reg)
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

#எனில் defined(FEAT_CLIPBOARD)
    வெறுமை
free_register(வெறுமை *reg)
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
    முழு
yank_register_mline(முழு regname)
{
    எனில் (regname != 0 && !valid_yank_reg(regname, FALSE))
	திரும்பு FALSE;
    எனில் (regname == '_')		// black hole is always empty
	திரும்பு FALSE;
    get_yank_register(regname, FALSE);
    திரும்பு (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    முழு
do_record(முழு c)
{
    char_u	    *p;
    நிலையான முழு	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    முழு		    retval;

    எனில் (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	எனில் (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	இல்லையெனில்
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    இல்லையெனில்			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	செய்தி("");
	p = get_recorded();
	எனில் (p == வெற்று)
	    retval = FAIL;
	இல்லையெனில்
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
    திரும்பு retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    நிலையான முழு
stuff_yank(முழு regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    எனில் (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	திரும்பு FAIL;
    }
    எனில் (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	திரும்பு OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    எனில் (y_append && y_current->y_array != வெற்று)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	எனில் (tmp == வெற்று)
	{
	    vim_free(p);
	    திரும்பு FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    இல்லையெனில்
    {
	free_yank_all();
	எனில் ((y_current->y_array = ALLOC_ONE(string_T)) == வெற்று)
	{
	    vim_free(p);
	    திரும்பு FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    திரும்பு OK;
}

/*
 * Last executed register (@ command)
 */
நிலையான முழு execreg_lastc = NUL;

#எனில் defined(FEAT_VIMINFO)
    முழு
get_execreg_lastc(வெறுமை)
{
    திரும்பு execreg_lastc;
}

    வெறுமை
set_execreg_lastc(முழு lastc)
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
    நிலையான char_u *
execreg_வரி_continuation(string_T *வரிகள், long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    முழு		j;
    char_u	*str;

    ga_init2(&ga, அளவு(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    போது (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(வரிகள்[cmd_start].string);
	எனில் (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    நிறுத்து;
    }

    // join all the lines
    tmp = &வரிகள்[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    க்கு (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &வரிகள்[j];
	p = skipwhite(tmp->string);
	எனில் (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    எனில் (ga.ga_len > 400)
	    {
		எனில் (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		இல்லையெனில்
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
    திரும்பு str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    முழு
do_execreg(
    முழு	    regname,
    முழு	    colon,		// insert ':' before each line
    முழு	    addcr,		// always add '\n' to end of line
    முழு	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    முழு		retval = OK;
    முழு		remap;

    // repeat previous one
    எனில் (regname == '@')
    {
	எனில் (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    திரும்பு FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    எனில் (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	திரும்பு FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    எனில் (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    எனில் (regname == '_')
	திரும்பு OK;

    // use last command line
    எனில் (regname == ':')
    {
	எனில் (last_cmdline == வெற்று)
	{
	    emsg(_(e_no_previous_command_வரி));
	    திரும்பு FAIL;
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
	எனில் (p != வெற்று)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    எனில் (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    இல்லையெனில்
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    இல்லையெனில் எனில் (regname == '=')
    {
	p = get_கோவை_வரி();
	எனில் (p == வெற்று)
	    திரும்பு FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    இல்லையெனில் எனில் (regname == '.')		// use last inserted text
    {
	p = get_last_செருகு_save();
	எனில் (p == வெற்று)
	{
	    emsg(_(e_no_inserted_text_yet));
	    திரும்பு FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    இல்லையெனில்
    {
	get_yank_register(regname, FALSE);
	எனில் (y_current->y_array == வெற்று)
	    திரும்பு FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	க்கு (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    முழு	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    எனில் (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		எனில் (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    திரும்பு FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    எனில் (colon && i > 0)
	    {
		p = skipwhite(str);
		எனில் (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_வரி_continuation(y_current->y_array, &i);
		    எனில் (str == வெற்று)
			திரும்பு FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    எனில் (free_str)
		vim_free(str);
	    எனில் (escaped == வெற்று)
		திரும்பு FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    எனில் (retval == FAIL)
		திரும்பு FAIL;
	    எனில் (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		திரும்பு FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    திரும்பு retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    நிலையான வெறுமை
put_reedit_in_typebuf(முழு silent)
{
    எனில் (restart_edit == NUL)
	திரும்பு;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    எனில் (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    இல்லையெனில் எனில் (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    இல்லையெனில் எனில் (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    எனில் (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    நிலையான முழு
put_in_typebuf(
    char_u	*s,
    முழு		esc,
    முழு		colon,	    // add ':' before the line
    முழு		silent)
{
    முழு		retval = OK;

    put_reedit_in_typebuf(silent);
    எனில் (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    எனில் (retval == OK)
    {
	char_u	*p;

	எனில் (esc)
	    p = vim_strsave_escape_csi(s);
	இல்லையெனில்
	    p = s;
	எனில் (p == வெற்று)
	    retval = FAIL;
	இல்லையெனில்
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	எனில் (esc)
	    vim_free(p);
    }
    எனில் (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    திரும்பு retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
செருகு_reg(
    முழு		regname,
    முழு		literally_arg)	// insert literally, not as if typed
{
    long	i;
    முழு		retval = OK;
    char_u	*arg;
    முழு		allocated;
    முழு		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    எனில் (got_int)
	திரும்பு FAIL;

    // check for valid regname
    எனில் (regname != NUL && !valid_yank_reg(regname, FALSE))
	திரும்பு FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    எனில் (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    எனில் (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    இல்லையெனில் எனில் (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	எனில் (arg == வெற்று)
	    திரும்பு FAIL;
	stuffescaped(arg, literally);
	எனில் (allocated)
	    vim_free(arg);
    }
    இல்லையெனில்				// name or number register
    {
	எனில் (get_yank_register(regname, FALSE))
	    literally = TRUE;
	எனில் (y_current->y_array == வெற்று)
	    retval = FAIL;
	இல்லையெனில்
	{
	    க்கு (i = 0; i < y_current->y_size; ++i)
	    {
		எனில் (regname == '-' && y_current->y_type == MCHAR)
		{
		    முழு dir = BACKWARD;
		    எனில் ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			எனில் (u_save_cursor() == FAIL)
			    திரும்பு FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			எனில் (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, வெற்று, dir, 1L, PUT_CURSEND);
		}
		இல்லையெனில்
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    எனில் (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    திரும்பு retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    முழு
get_spec_reg(
    முழு		regname,
    char_u	**argp,
    முழு		*allocated,	// return: TRUE when value was allocated
    முழு		errmsg)		// give error message when failing
{
    முழு		cnt;

    *argp = வெற்று;
    *allocated = FALSE;
    தேர்வு (regname)
    {
	நிலை '%':		// file name
	    எனில் (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    திரும்பு TRUE;

	நிலை '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    திரும்பு TRUE;

#ifdef FEAT_EVAL
	நிலை '=':		// result of expression
	    *argp = get_கோவை_வரி();
	    *allocated = TRUE;
	    திரும்பு TRUE;
#endif

	நிலை ':':		// last command line
	    எனில் (last_cmdline == வெற்று && errmsg)
		emsg(_(e_no_previous_command_வரி));
	    *argp = last_cmdline;
	    திரும்பு TRUE;

	நிலை '/':		// last search-pattern
	    எனில் (last_தேடல்_pat() == வெற்று && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_தேடல்_pat();
	    திரும்பு TRUE;

	நிலை '.':		// last inserted text
	    *argp = get_last_செருகு_save();
	    *allocated = TRUE;
	    எனில் (*argp == வெற்று && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    திரும்பு TRUE;

	நிலை Ctrl_F:		// Filename under cursor
	நிலை Ctrl_P:		// Path under cursor, expand via "path"
	    எனில் (!errmsg)
		திரும்பு FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, வெற்று);
	    *allocated = TRUE;
	    திரும்பு TRUE;

	நிலை Ctrl_W:		// word under cursor
	நிலை Ctrl_A:		// WORD (mnemonic All) under cursor
	    எனில் (!errmsg)
		திரும்பு FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : வெற்று;
	    *allocated = TRUE;
	    திரும்பு TRUE;

	நிலை Ctrl_L:		// Line under cursor
	    எனில் (!errmsg)
		திரும்பு FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    திரும்பு TRUE;

	நிலை '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    திரும்பு TRUE;
    }

    திரும்பு FALSE;
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
    முழு
cmdline_paste_reg(
    முழு regname,
    முழு literally_arg,	// Insert text literally instead of "as typed"
    முழு remcr)		// don't add CR characters
{
    long	i;
    முழு		literally = literally_arg;

    எனில் (get_yank_register(regname, FALSE))
	literally = TRUE;
    எனில் (y_current->y_array == வெற்று)
	திரும்பு FAIL;

    க்கு (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	எனில் ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	எனில் (got_int)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    வெறுமை
shift_delete_registers(வெறுமை)
{
    முழு		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    க்கு (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    எனில் (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = வெற்று;		// set register one to empty
}

#எனில் defined(FEAT_EVAL)
    நிலையான வெறுமை
add_regtype_to_dict(முழு regname, dict_T *dict, char_u *buf, முழு bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    தேர்வு (get_reg_type(regname, &reglen))
    {
	நிலை MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    நிறுத்து;
	நிலை MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    நிறுத்து;
	நிலை MBLOCK:
	    buflen = vim_snprintf_safelen((எழுத்து *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    நிறுத்து;
	இயல்பு:
	    buf[0] = NUL;
	    buflen = 0;
	    நிறுத்து;
    }
    (வெறுமை)dict_add_string_len(dict, "regtype", buf, (முழு)buflen);
}

    வெறுமை
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    நிலையான முழு	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    முழு		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    எனில் (recursive)
	திரும்பு;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    எனில் (list == வெற்று)
	திரும்பு;

    // yanked text contents
    க்கு (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (முழு)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    எனில் (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (வெறுமை)dict_add_string_len(v_event, "regname", buf, (முழு)buflen);

    // motion type: inclusive or exclusive
    (வெறுமை)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (வெறுமை)dict_add_string_len(v_event, "operator", buf, (முழு)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, அளவு(buf));

    // selection type - visual or not
    (வெறுமை)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_பொருட்கள்_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, வெற்று, வெற்று, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    நிலையான வெறுமை
put_do_autocmd(
	முழு	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *செருகு,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	முழு	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    நிலையான bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    முழு		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    எனில் (recursive || regname == '_')
	திரும்பு;

    எனில் (regname != '.' && செருகு == வெற்று
	    && reg == வெற்று)
	// Can happen when pasting text in normal mode in a terminal buffer
	திரும்பு;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    எனில் (list == வெற்று)
	திரும்பு;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    எனில் (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    எனில் (regname == '.')
    {
	எனில் (last_செருகு_ga.ga_data != வெற்று)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_செருகு_ga.ga_data,
		    (முழு)last_செருகு_ga.ga_len);
    }
    இல்லையெனில் எனில் (செருகு != வெற்று)
    {
	list_append_string(list, செருகு->string, (முழு)செருகு->length);
    }
    இல்லையெனில்
    {
	க்கு (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (முழு)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    எனில் (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (வெறுமை)dict_add_string_len(v_event, "regname", buf, (முழு)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (வெறுமை)dict_add_string_len(v_event, "operator", buf, (முழு)buflen);

    add_regtype_to_dict(regname, v_event, buf, அளவு(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (வெறுமை)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_பொருட்கள்_ro(v_event);

    recursive = true;
    textlock++;
    எனில் (post)
	apply_autocmds(EVENT_TEXTPUTPOST, வெற்று, வெற்று, FALSE, curbuf);
    இல்லையெனில்
	apply_autocmds(EVENT_TEXTPUTPRE, வெற்று, வெற்று, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    வெறுமை
init_yank(வெறுமை)
{
    முழு		i;

    க்கு (i = 0; i < இலக்கம்_REGISTERS; ++i)
	y_regs[i].y_array = வெற்று;
}

#எனில் defined(EXITFREE)
    வெறுமை
clear_registers(வெறுமை)
{
    முழு		i;

    க்கு (i = 0; i < இலக்கம்_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	எனில் (y_current->y_array != வெற்று)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    நிலையான வெறுமை
free_yank(long n)
{
    எனில் (y_current->y_array == வெற்று)
	திரும்பு;

    long	    i;

    க்கு (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    வெறுமை
free_yank_all(வெறுமை)
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
    முழு
op_yank(oparg_T *oap, முழு deleting, முழு mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    முழு			yanktype = oap->motion_type;
    long		yanklines = oap->வரி_மொத்தம்;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    கட்டமைப்பு block_def	bd;

				    // check for read-only register
    எனில் (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	திரும்பு FAIL;
    }
    எனில் (oap->regname == '_')	    // black hole: nothing to do
	திரும்பு OK;

#ifdef FEAT_CLIPBOARD
    எனில் ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	செய்தி_warn_missing_clipboard();
    }
#endif

    எனில் (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    எனில் (y_append && y_current->y_array != வெற்று)
	y_current = &newreg;
    இல்லையெனில்
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    எனில் (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(அளவு(string_T) * yanklines, TRUE);
    எனில் (y_current->y_array == வெற்று)
    {
	y_current = curr;
	திரும்பு FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    எனில் (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	எனில் (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    க்கு ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	தேர்வு (y_current->y_type)
	{
	    நிலை MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		எனில் (yank_copy_வரி(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    செல் fail;
		நிறுத்து;

	    நிலை MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		எனில் ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == வெற்று)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    செல் fail;
		}
		நிறுத்து;

	    நிலை MCHAR:
		{
		    முழு tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (முழு)STRLEN(bd.textstart);
		    எனில் (tmp < bd.textlen)
			bd.textlen = tmp;

		    எனில் (yank_copy_வரி(&bd, y_idx, FALSE) == FAIL)
			செல் fail;
		    நிறுத்து;
		}
		// NOTREACHED
	}
    }

    எனில் (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	எனில் (new_ptr == வெற்று)
	    செல் fail;
	க்கு (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	எனில் (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	எனில் (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == வெற்று)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    எனில் (pnew == வெற்று)
	    {
		y_idx = y_current->y_size - 1;
		செல் fail;
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
	இல்லையெனில்
	    y_idx = 0;
	போது (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    எனில் (mess)			// Display message about yank?
    {
	எனில் (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	எனில் (yanklines > p_report)
	{
	    எழுத்து namebuf[100];

	    எனில் (oap->regname == NUL)
		*namebuf = NUL;
	    இல்லையெனில்
		vim_snprintf(namebuf, அளவு(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    எனில் (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    இல்லையெனில்
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	எனில் (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	எனில் (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    எனில் (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	எனில் (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    எனில் (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	எனில் (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    எனில் (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	எனில் (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    எனில் (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# எனில் defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	எனில் (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    எனில் (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#எனில் defined(FEAT_EVAL)
    எனில் (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    திரும்பு OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    திரும்பு FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    நிலையான முழு
yank_copy_வரி(கட்டமைப்பு block_def *bd, long y_idx, முழு exclude_trailing_space)
{
    char_u	*pnew;

    எனில் (exclude_trailing_space)
	bd->endspaces = 0;
    எனில் ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == வெற்று)
	திரும்பு FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    எனில் (exclude_trailing_space)
    {
	முழு s = bd->textlen + bd->endspaces;

	போது (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    திரும்பு OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    நிலையான வெறுமை
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    அளவு(string_T) * y_current->y_size, TRUE);
    எனில் (y_current->y_array == வெற்று)
	y_current->y_size = 0;
    இல்லையெனில்
	க்கு (j = 0; j < y_current->y_size; ++j)
	{
	    எனில் ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == வெற்று)
	    {
		free_yank(j);
		y_current->y_size = 0;
		நிறுத்து;
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
    வெறுமை
do_put(
    முழு		regname,
    char_u	*கோவை_result,	// result for regname "=" when compiled
    முழு		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	மொத்தம்,
    முழு		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    முழு		yanklen;
    முழு		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    முழு		y_type;
    long	y_size;
    முழு		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = வெற்று;
    yankreg_T	*y_current_used = வெற்று;
    long	nr_வரிகள் = 0;
    string_T	செருகு_string;
    முழு		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned முழு cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    எனில் (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(வெறுமை)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    எனில் (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    எனில் (regname == '.')
    {
	எனில் (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	எனில் (has_textput_events)
	    add_last_செருகு++;
#endif

	(வெறுமை)stuff_inserted((dir == FORWARD ? (மொத்தம் == -1 ? 'o' : 'a') :
				    (மொத்தம் == -1 ? 'O' : 'i')), மொத்தம், FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	எனில் (has_textputpre())
	    put_do_autocmd('.', வெற்று, வெற்று, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	எனில் (has_textputpost())
	    put_do_autocmd('.', வெற்று, வெற்று, true, dir);

	எனில் (has_textput_events && --add_last_செருகு == 0)
	    ga_clear(&last_செருகு_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	எனில் ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	திரும்பு;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    செருகு_string.string = வெற்று;
    எனில் (regname == '=' && கோவை_result != வெற்று)
	செருகு_string.string = கோவை_result;
    இல்லையெனில் எனில் (get_spec_reg(regname, &செருகு_string.string, &allocated, TRUE)
		&& செருகு_string.string == வெற்று)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	திரும்பு;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    எனில் (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	செல் end;

    எனில் (செருகு_string.string != வெற்று)
    {
	செருகு_string.length = STRLEN(செருகு_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	எனில் (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    க்கு (;;)
	    {
		y_size = 0;
		ptr = செருகு_string.string;
		ptrlen = செருகு_string.length;
		போது (ptr != வெற்று)
		{
		    எனில் (y_array != வெற்று)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    எனில் (tmp == வெற்று)
		    {
			எனில் (y_array != வெற்று)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    இல்லையெனில்
		    {
			எனில் (y_array != வெற்று)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			எனில் (*tmp == NUL)
			{
			    y_type = MLINE;
			    நிறுத்து;
			}
		    }
		    ptr = tmp;
		}
		எனில் (y_array != வெற்று)
		    நிறுத்து;
		y_array = ALLOC_MULT(string_T, y_size);
		எனில் (y_array == வெற்று)
		    செல் end;
	    }
	}
	இல்லையெனில்
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &செருகு_string;
	}
#ifdef FEAT_EVAL
	எனில் (has_textputpre())
	    put_do_autocmd(regname, வெற்று, &செருகு_string, false, dir);
#endif
    }
    இல்லையெனில்
    {
#ifdef FEAT_EVAL
	எனில் (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, வெற்று, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    எனில் (y_type == MLINE)
    {
	எனில் (flags & PUT_வரி_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    எனில் (u_save_cursor() == FAIL)
		செல் end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    எனில் (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    எனில் (ptr == வெற்று)
		செல் end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    எனில் (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    எனில் (ptr == வெற்று)
		செல் end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_வரிகள்;
	    dir = FORWARD;
	}
	எனில் (flags & PUT_வரி_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    எனில் (flags & PUT_வரி)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    எனில் (y_size == 0 || y_array == வெற்று)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	செல் end;
    }

    எனில் (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	எனில் (lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	    lnum = curbuf->b_ml.ml_வரி_மொத்தம் + 1;
	எனில் (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    செல் end;
    }
    இல்லையெனில் எனில் (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	எனில் (dir == BACKWARD)
	    (வெறுமை)hasFolding(lnum, &lnum, வெற்று);
	இல்லையெனில்
	    (வெறுமை)hasFolding(lnum, வெற்று, &lnum);
#endif
	எனில் (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	எனில் ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    செல் end;
#ifdef FEAT_FOLDING
	எனில் (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	இல்லையெனில்
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    இல்லையெனில் எனில் (u_save_cursor() == FAIL)
	செல் end;

    எனில் (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	எனில் (gchar_cursor() == TAB)
	{
	    முழு viscol = getviscol();
	    முழு ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    எனில் (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#இல்லையெனில்
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    இல்லையெனில்
		curwin->w_cursor.coladd = 0;
	}
	இல்லையெனில் எனில் (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    எனில் (y_type == MBLOCK)
    {
	முழு	delcount;
	முழு	incr = 0;
	கட்டமைப்பு block_def bd;
	long	j;
	முழு	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	எனில் (dir == FORWARD && c != NUL)
	{
	    எனில் (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, வெற்று, &endcol2, 0);
	    இல்லையெனில்
		getvcol(curwin, &curwin->w_cursor, வெற்று, வெற்று, &col, 0);

	    எனில் (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    இல்லையெனில்
		எனில் (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	இல்லையெனில்
	    getvcol(curwin, &curwin->w_cursor, &col, வெற்று, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	எனில் (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    எனில் (dir == FORWARD && c == NUL)
		++col;
	    எனில் (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    எனில் (c == TAB)
	    {
		எனில் (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		எனில் (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	க்கு (i = 0; i < y_size; ++i)
	{
	    முழு		    spaces = 0;
	    எழுத்து	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    எனில் (curwin->w_cursor.lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	    {
		எனில் (ml_append(curbuf->b_ml.ml_வரி_மொத்தம், (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    நிறுத்து;
		++nr_வரிகள்;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    போது (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    எனில் (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    இல்லையெனில் எனில் (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		எனில் (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		எனில் (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (முழு)y_array[i].length;

	    எனில் ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		போது (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		எனில் (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    எனில் (yanklen + spaces != 0
		     && மொத்தம் > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		நிறுத்து;
	    }

	    totlen = மொத்தம் * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    எனில் (newp == வெற்று)
		நிறுத்து;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    க்கு (j = 0; j < மொத்தம்; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		எனில் ((j < மொத்தம் - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		இல்லையெனில்
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
	    எனில் (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_வரிகள்(lnum, 0, curwin->w_cursor.lnum - nr_வரிகள், nr_வரிகள்);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	எனில் (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	எனில் (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    எனில் (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	இல்லையெனில்
	    curwin->w_cursor.lnum = lnum;
    }
    இல்லையெனில்
    {
	pos_T	new_cursor;

	yanklen = (முழு)y_array[0].length;

	// Character or Line mode
	எனில் (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    எனில் (dir == FORWARD && gchar_cursor() != NUL)
	    {
		எனில் (has_mbyte)
		{
		    முழு bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    எனில் (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		இல்லையெனில்
		{
		    ++col;
		    எனில் (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	இல்லையெனில் எனில் (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	எனில் (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    முழு		first_byte_off = 0;

	    எனில் (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		எனில் (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		எனில் (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, வெற்று, &vcol, வெற்று, 0);
		}
	    }

	    எனில் (மொத்தம் == 0 || yanklen == 0)
	    {
		எனில் (VIsual_active)
		    lnum = end_lnum;
	    }
	    இல்லையெனில் எனில் (மொத்தம் > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    இல்லையெனில்
	    {
		totlen = மொத்தம் * yanklen;
		செய்
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    எனில் (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			எனில் (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			இல்லையெனில்
			    col = MAXCOL;
		    }
		    எனில் (VIsual_active && col > oldlen)
		    {
			lnum++;
			தொடர்;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    எனில் (newp == வெற்று)
			செல் end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    க்கு (i = 0; i < மொத்தம்; ++i)
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
		    எனில் (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    எனில் (VIsual_active)
			lnum++;
		} போது (VIsual_active && lnum <= end_lnum);

		எனில் (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    எனில் (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    இல்லையெனில்
		curwin->w_cursor.col -= first_byte_off;
	}
	இல்லையெனில்
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    முழு		indent;
	    முழு		orig_indent = 0;
	    முழு		indent_diff = 0;	// init for gcc
	    முழு		first_indent = TRUE;
	    முழு		lendiff = 0;
	    long	cnt;

	    எனில் (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    க்கு (cnt = 1; cnt <= மொத்தம்; ++cnt)
	    {
		i = 0;
		எனில் (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (முழு)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    எனில் (newp == வெற்று)
			செல் error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    எனில் (newp == வெற்று)
			செல் error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		க்கு (; i < y_size; ++i)
		{
		    எனில் (y_type != MCHAR || i < y_size - 1)
		    {
			எனில் (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    செல் error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_வரிகள்;
		    எனில் (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			எனில் (cnt == மொத்தம் && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			எனில் (*ptr == '#' && preprocs_இடது())
			    indent = 0;     // Leave # lines at start
			இல்லையெனில் எனில் (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			இல்லையெனில் எனில் (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			இல்லையெனில் எனில் ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(வெறுமை)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			எனில் (cnt == மொத்தம் && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		எனில் (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    எனில் (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		எனில் (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_வரிகள், 0L);

	    // note changed text for displaying and folding
	    எனில் (y_type == MCHAR)
		changed_வரிகள்(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_வரிகள்);
	    இல்லையெனில்
		changed_வரிகள்(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_வரிகள்);
	    எனில் (y_current_used != வெற்று && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		செல் end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    எனில் (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		எனில் (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    இல்லையெனில்
		curbuf->b_op_end.col = 0;

	    எனில் (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    இல்லையெனில் எனில் (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		எனில் (y_type == MLINE)
		{
		    எனில் (lnum >= curbuf->b_ml.ml_வரி_மொத்தம்)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
		    இல்லையெனில்
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		இல்லையெனில்
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    எனில் (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    இல்லையெனில் எனில் (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		எனில் (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    இல்லையெனில்	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_வரிகள்);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    முழு len = ml_get_curline_len();
    எனில் (curwin->w_cursor.col > len)
    {
	எனில் (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    எனில் (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    எனில் (has_textputpost())
    {
	எனில் (செருகு_string.string == வெற்று)
	    put_do_autocmd(regname, y_current, வெற்று, true, dir);
	இல்லையெனில்
	    put_do_autocmd(regname, வெற்று, &செருகு_string, true, dir);
    }
#endif

    எனில் (allocated)
	vim_free(செருகு_string.string);
    எனில் (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    முழு
get_register_name(முழு இலக்கம்)
{
    எனில் (இலக்கம் == -1)
	திரும்பு '"';
    இல்லையெனில் எனில் (இலக்கம் < 10)
	திரும்பு இலக்கம் + '0';
    இல்லையெனில் எனில் (இலக்கம் == DELETION_REGISTER)
	திரும்பு '-';
#எனில் defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    இல்லையெனில் எனில் (இலக்கம் == STAR_REGISTER)
	திரும்பு '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    இல்லையெனில் எனில் (clipmethod == CLIPMETHOD_PROVIDER && இலக்கம் == REAL_PLUS_REGISTER)
	திரும்பு '+';
    இல்லையெனில் எனில் (clipmethod != CLIPMETHOD_PROVIDER && இலக்கம் == PLUS_REGISTER)
	திரும்பு '+';
#endif
    இல்லையெனில்
	திரும்பு இலக்கம் + 'a' - 10;
}

#எனில் defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    முழு
get_unname_register(வெறுமை)
{
    திரும்பு y_previous == வெற்று ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    வெறுமை
ex_display(exarg_T *eap)
{
    முழு		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    முழு		name;
    முழு		attr;
    char_u	*arg = eap->arg;
    முழு		clen;
    முழு		type;
    string_T	செருகு;

    silence_w23_w24_செய்தி++;
    எனில் (arg != வெற்று && *arg == NUL)
	arg = வெற்று;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    செய்தி_puts_title(_("\nType Name Content"));
    க்கு (i = -1; i < இலக்கம்_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	தேர்வு (get_reg_type(name, வெற்று))
	{
	    நிலை MLINE: type = 'l'; நிறுத்து;
	    நிலை MCHAR: type = 'c'; நிறுத்து;
	    இயல்பு:	type = 'b'; நிறுத்து;
	}
	எனில் (arg != வெற்று && vim_strchr(arg, name) == வெற்று
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == வெற்று)
#endif
		)
	    தொடர்;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	எனில் (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (வெறுமை)may_get_selection(name);
#endif

	எனில் (i == -1)
	{
	    எனில் (y_previous != வெற்று)
		yb = y_previous;
	    இல்லையெனில்
		yb = &(y_regs[0]);
	}
	இல்லையெனில்
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	எனில் (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != வெற்று &&
		    (yb == y_previous || yb == &y_regs[0])))
	    தொடர்;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	எனில் (yb->y_array != வெற்று)
	{
	    முழு do_show = FALSE;

	    க்கு (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    எனில் (do_show || yb->y_size == 0)
	    {
		செய்தி_putchar('\n');
		செய்தி_puts("  ");
		செய்தி_putchar(type);
		செய்தி_puts("  ");
		செய்தி_putchar('"');
		செய்தி_putchar(name);
		செய்தி_puts("   ");

		n = (முழு)Columns - 11;
		க்கு (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    எனில் (j)
		    {
			செய்தி_puts_attr("^J", attr);
			n -= 2;
		    }
		    க்கு (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			செய்தி_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		எனில் (n > 1 && yb->y_type == MLINE)
		    செய்தி_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    செருகு = get_last_செருகு();
    எனில் ((p = செருகு.string) != வெற்று
		  && (arg == வெற்று || vim_strchr(arg, '.') != வெற்று) && !got_int
						      && !message_filtered(p))
    {
	செய்தி_puts("\n  c  \".   ");
	dis_செய்தி(p, TRUE);
    }

    // display last command line
    எனில் (last_cmdline != வெற்று && (arg == வெற்று || vim_strchr(arg, ':') != வெற்று)
			       && !got_int && !message_filtered(last_cmdline))
    {
	செய்தி_puts("\n  c  \":   ");
	dis_செய்தி(last_cmdline, FALSE);
    }

    // display current file name
    எனில் (curbuf->b_fname != வெற்று
	    && (arg == வெற்று || vim_strchr(arg, '%') != வெற்று) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	செய்தி_puts("\n  c  \"%   ");
	dis_செய்தி(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    எனில் ((arg == வெற்று || vim_strchr(arg, '#') != வெற்று) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	எனில் (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    செய்தி_puts("\n  c  \"#   ");
	    dis_செய்தி(fname, FALSE);
	}
    }

    // display last search pattern
    எனில் (last_தேடல்_pat() != வெற்று
		 && (arg == வெற்று || vim_strchr(arg, '/') != வெற்று) && !got_int
				      && !message_filtered(last_தேடல்_pat()))
    {
	செய்தி_puts("\n  c  \"/   ");
	dis_செய்தி(last_தேடல்_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    எனில் (கோவை_வரி != வெற்று && (arg == வெற்று || vim_strchr(arg, '=') != வெற்று)
				  && !got_int && !message_filtered(கோவை_வரி))
    {
	செய்தி_puts("\n  c  \"=   ");
	dis_செய்தி(கோவை_வரி, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_செய்தி--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    நிலையான வெறுமை
dis_செய்தி(
    char_u	*p,
    முழு		skip_esc)	    // if TRUE, ignore trailing ESC
{
    முழு		n;
    முழு		l;

    n = (முழு)Columns - 6;
    போது (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	எனில் (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    செய்தி_outtrans_len(p, l);
	    p += l;
	}
	இல்லையெனில்
	    செய்தி_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#எனில் defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    வெறுமை
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
get_reg_type(முழு regname, long *reglen)
{
    தேர்வு (regname)
    {
	நிலை '%':		// file name
	நிலை '#':		// alternate file name
	நிலை '=':		// expression
	நிலை ':':		// last command line
	நிலை '/':		// last search-pattern
	நிலை '.':		// last inserted text
	நிலை Ctrl_F:		// Filename under cursor
	நிலை Ctrl_P:		// Path under cursor, expand via "path"
	நிலை Ctrl_W:		// word under cursor
	நிலை Ctrl_A:		// WORD (mnemonic All) under cursor
	நிலை '_':		// black hole: always empty
	    திரும்பு MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    எனில் (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    எனில் (regname != NUL && !valid_yank_reg(regname, FALSE))
	திரும்பு MAUTO;

    get_yank_register(regname, FALSE);

    எனில் (y_current->y_array != வெற்று)
    {
	எனில் (reglen != வெற்று && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	திரும்பு y_current->y_type;
    }
    திரும்பு MAUTO;
}

#எனில் defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    நிலையான char_u *
getreg_wrap_one_வரி(char_u *s, முழு flags)
{
    எனில் (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	எனில் (list != வெற்று)
	{
	    எனில் (list_append_string(list, வெற்று, -1) == FAIL)
	    {
		list_free(list);
		திரும்பு வெற்று;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	திரும்பு (char_u *)list;
    }
    திரும்பு s;
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
get_reg_contents(முழு regname, முழு flags)
{
    linenr_T	i;
    char_u	*retval;
    முழு		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    எனில் (regname == '=')
    {
	எனில் (flags & GREG_NO_கோவை)
	    திரும்பு வெற்று;
	எனில் (flags & GREG_கோவை_SRC)
	    திரும்பு getreg_wrap_one_வரி(get_கோவை_வரி_src(), flags);
	திரும்பு getreg_wrap_one_வரி(get_கோவை_வரி(), flags);
    }

    எனில் (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    எனில் (regname != NUL && !valid_yank_reg(regname, FALSE))
	திரும்பு வெற்று;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    எனில் (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    எனில் (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	எனில் (retval == வெற்று)
	    திரும்பு வெற்று;
	எனில் (allocated)
	    திரும்பு getreg_wrap_one_வரி(retval, flags);
	திரும்பு getreg_wrap_one_வரி(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    எனில் (y_current->y_array == வெற்று)
	திரும்பு வெற்று;

    எனில் (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	எனில் (list == வெற்று)
	    திரும்பு வெற்று;
	க்கு (i = 0; i < y_current->y_size; ++i)
	{
	    எனில் (list_append_string(list, y_current->y_array[i].string,
		(முழு)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		திரும்பு வெற்று;
	    }
	}
	திரும்பு (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    க்கு (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	எனில் (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    எனில் (retval == வெற்று)
	திரும்பு வெற்று;

    // Copy the lines of the yank register into the string.
    len = 0;
    க்கு (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	எனில் (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    திரும்பு retval;
}

    நிலையான முழு
init_write_reg(
    முழு		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    முழு		must_append,
    முழு		*yank_type UNUSED)
{
    எனில் (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	திரும்பு FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    எனில் (!y_append && !must_append)
	free_yank_all();
    திரும்பு OK;
}

    நிலையான வெறுமை
finish_write_reg(
    முழு		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    எனில் (name != '"')
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
    வெறுமை
write_reg_contents(
    முழு		name,
    char_u	*str,
    முழு		maxlen,
    முழு		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    வெறுமை
write_reg_contents_lst(
    முழு		name,
    char_u	**strings,
    முழு		maxlen UNUSED,
    முழு		must_append,
    முழு		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    எனில் (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	எனில் (strings[0] == வெற்று)
	    s = (char_u *)"";
	இல்லையெனில் எனில் (strings[1] != வெற்று)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_வரிகள்), name);
	    திரும்பு;
	}
	இல்லையெனில்
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	திரும்பு;
    }

    எனில் (name == '_')	    // black hole: nothing to do
	திரும்பு;

    எனில் (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	திரும்பு;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    வெறுமை
write_reg_contents_ex(
    முழு		name,
    char_u	*str,
    முழு		maxlen,
    முழு		must_append,
    முழு		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    எனில் (maxlen >= 0)
	len = maxlen;
    இல்லையெனில்
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    எனில் (name == '/')
    {
	set_last_தேடல்_pat(str, RE_தேடல், TRUE, TRUE);
	திரும்பு;
    }

    எனில் (name == '#')
    {
	எனில் (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  திரும்பு;
	}

	buf_T	*buf;

	எனில் (VIM_ISDIGIT(*str))
	{
	    முழு	இலக்கம் = atoi((எழுத்து *)str);

	    buf = buflist_findnr(இலக்கம்);
	    எனில் (buf == வெற்று)
		semsg(_(e_buffer_nr_does_not_exist), (long)இலக்கம்);
	}
	இல்லையெனில்
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	எனில் (buf == வெற்று)
	    திரும்பு;
	curwin->w_alt_fnum = buf->b_fnum;
	திரும்பு;
    }

    எனில் (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	எனில் (p == வெற்று)
	    திரும்பு;
	எனில் (must_append && கோவை_வரி != வெற்று)
	{
	    s = concat_str(கோவை_வரி, p);
	    vim_free(p);
	    p = s;
	}
	set_கோவை_வரி(p, வெற்று);
	திரும்பு;
    }

    எனில் (name == '_')	    // black hole: nothing to do
	திரும்பு;

    எனில் (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	திரும்பு;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#எனில் defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    வெறுமை
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    முழு		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    முழு		str_list)	// TRUE if str is char_u **
{
    முழு		type;			// MCHAR, MLINE or MBLOCK
    முழு		lnum;
    long	start;
    long	i;
    முழு		extra;
    முழு		newlines;		// number of lines added
    முழு		extraline = 0;		// extra line at the end
    முழு		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    எனில் (y_ptr->y_array == வெற்று)		// NULL means empty register
	y_ptr->y_size = 0;

    எனில் (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    இல்லையெனில்
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    எனில் (str_list)
    {
	க்கு (ss = (char_u **) str; *ss != வெற்று; ++ss)
	    ++newlines;
    }
    இல்லையெனில்
    {
	க்கு (i = 0; i < len; i++)
	    எனில் (str[i] == '\n')
		++newlines;
	எனில் (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	எனில் (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    எனில் (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	திரும்பு;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * அளவு(string_T), TRUE);
    எனில் (pp == வெற்று)	// out of memory
	திரும்பு;
    க்கு (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    எனில் (str_list)
    {
	க்கு (ss = (char_u **) str; *ss != வெற்று; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    எனில் (type == MBLOCK)
	    {
		முழு charlen = vim_strsize(*ss);

		எனில் (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    இல்லையெனில்
    {
	க்கு (start = 0; start < len + extraline; start += i + 1)
	{
	    முழு charlen = 0;

	    க்கு (i = start; i < len;)	// find the end of the line
	    {
		எனில் (str[i] == '\n')
		    நிறுத்து;
		எனில் (type == MBLOCK)
		{
		    எனில் (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    இல்லையெனில்
			charlen += byte2cells(str[i]);
		}

		எனில் (str[i] == NUL)
		    i++; // registers can have NUL chars
		இல்லையெனில்
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    எனில் (charlen > maxlen)
		maxlen = charlen;
	    எனில் (append)
	    {
		--lnum;
		extra = (முழு)y_ptr->y_array[lnum].length;
	    }
	    இல்லையெனில்
		extra = 0;
	    s = alloc(i + extra + 1);
	    எனில் (s == வெற்று)
		நிறுத்து;
	    எனில் (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    எனில் (append)
		vim_free(y_ptr->y_array[lnum].string);
	    எனில் (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    போது (--extra >= 0)
	    {
		எனில் (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    எனில் (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    இல்லையெனில்
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
