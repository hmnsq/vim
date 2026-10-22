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

#અન્તર્ભાવ "vim.h"

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
સ્થિર yankreg_T	y_regs[અંક_REGISTERS];

સ્થિર yankreg_T	*y_current;	    // ptr to current yankreg
સ્થિર પૂર્ણાંક		y_append;	    // TRUE when appending
સ્થિર yankreg_T	*y_previous = ખાલી; // ptr to last written yankreg

સ્થિર પૂર્ણાંક	stuff_yank(પૂર્ણાંક, char_u *);
સ્થિર શૂન્ય	put_reedit_in_typebuf(પૂર્ણાંક silent);
સ્થિર પૂર્ણાંક	put_in_typebuf(char_u *s, પૂર્ણાંક esc, પૂર્ણાંક colon, પૂર્ણાંક silent);
સ્થિર પૂર્ણાંક	yank_copy_લીટી(સંરચના block_def *bd, long y_idx, પૂર્ણાંક exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
સ્થિર શૂન્ય	copy_yank_reg(yankreg_T *reg);
#endif
સ્થિર શૂન્ય	dis_સંદેશ(char_u *p, પૂર્ણાંક skip_esc);

#જો defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(શૂન્ય)
{
    પાછા y_regs;
}
#endif

#જો defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(પૂર્ણાંક reg)
{
    પાછા &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(શૂન્ય)
{
    પાછા y_current;
}

    yankreg_T *
get_y_previous(શૂન્ય)
{
    પાછા y_previous;
}

    શૂન્ય
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    શૂન્ય
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    શૂન્ય
reset_y_append(શૂન્ય)
{
    y_append = FALSE;
}


#જો defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
સ્થિર char_u	*પદાવલિ_લીટી = ખાલી;
સ્થિર exarg_T	*પદાવલિ_eap = ખાલી;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    પૂર્ણાંક
get_પદાવલિ_register(શૂન્ય)
{
    char_u	*new_લીટી;

    new_લીટી = getcmdline('=', 0L, 0, 0);
    જો (new_લીટી == ખાલી)
	પાછા NUL;
    જો (*new_લીટી == NUL)	// use previous line
	vim_free(new_લીટી);
    નહીંતર
	set_પદાવલિ_લીટી(new_લીટી, ખાલી);
    પાછા '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    શૂન્ય
set_પદાવલિ_લીટી(char_u *new_લીટી, exarg_T *eap)
{
    vim_free(પદાવલિ_લીટી);
    પદાવલિ_લીટી = new_લીટી;
    પદાવલિ_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_પદાવલિ_લીટી(શૂન્ય)
{
    char_u	*પદાવલિ_copy;
    char_u	*rv;
    સ્થિર પૂર્ણાંક	nested = 0;

    જો (પદાવલિ_લીટી == ખાલી)
	પાછા ખાલી;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    પદાવલિ_copy = vim_strsave(પદાવલિ_લીટી);
    જો (પદાવલિ_copy == ખાલી)
	પાછા ખાલી;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    જો (nested >= 10)
	પાછા પદાવલિ_copy;

    ++nested;
    rv = eval_to_string_eap(પદાવલિ_copy, TRUE, પદાવલિ_eap, FALSE);
    --nested;
    vim_free(પદાવલિ_copy);
    પાછા rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    સ્થિર char_u *
get_પદાવલિ_લીટી_src(શૂન્ય)
{
    જો (પદાવલિ_લીટી == ખાલી)
	પાછા ખાલી;
    પાછા vim_strsave(પદાવલિ_લીટી);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    પૂર્ણાંક
valid_yank_reg(
    પૂર્ણાંક	    regname,
    પૂર્ણાંક	    writing)	    // if TRUE check for writable registers
{
    જો (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#નહીંતર
				    "/#.%:"
#endif
					, regname) != ખાલી)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#જો defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#અથવાજો defined(FEAT_CLIPBOARD_PROVIDER)
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
	પાછા TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    નહીંતર જો (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	સંદેશ_warn_missing_clipboard();
	પાછા FALSE;
    }
#endif
    પાછા FALSE;
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
    પૂર્ણાંક
get_yank_register(પૂર્ણાંક regname, પૂર્ણાંક writing)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    ret = FALSE;

    y_append = FALSE;
    જો ((regname == 0 || regname == '"') && !writing && y_previous != ખાલી)
    {
	y_current = y_previous;
	પાછા ret;
    }
    i = regname;
    જો (VIM_ISDIGIT(i))
	i -= '0';
    નહીંતર જો (ASCII_ISLOWER(i))
	i = Char_Ord_નીચલું(i) + 10;
    નહીંતર જો (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    નહીંતર જો (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    નહીંતર જો (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    નહીંતર જો (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	જો (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	નહીંતર
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#અથવાજો defined(FEAT_CLIPBOARD_PROVIDER)
    નહીંતર જો (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    નહીંતર જો (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    નહીંતર જો (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    નહીંતર		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    જો (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    પાછા ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    શૂન્ય *
get_register(
    પૂર્ણાંક		name,
    પૂર્ણાંક		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    પૂર્ણાંક		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    જો (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	જો (name == '*' && clip_star.available)
	{
	    જો (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	જો (name == '+' && clip_plus.available)
	{
	    જો (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    જો (reg == ખાલી)
	પાછા (શૂન્ય *)ખાલી;

    *reg = *y_current;
    જો (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	જો (reg->y_size == 0 || y_current->y_array == ખાલી)
	    reg->y_array = ખાલી;
	નહીંતર
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	જો (reg->y_array != ખાલી)
	{
	    માટે (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		જો (reg->y_array[i].string == ખાલી)
		{
		    // The allocation failed so clean up and exit
		    જ્યારે (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    પાછા (શૂન્ય *)ખાલી;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    નહીંતર
	y_current->y_array = ખાલી;
    પાછા (શૂન્ય *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    શૂન્ય
put_register(પૂર્ણાંક name, શૂન્ય *reg)
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

#જો defined(FEAT_CLIPBOARD)
    શૂન્ય
free_register(શૂન્ય *reg)
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
    પૂર્ણાંક
yank_register_mline(પૂર્ણાંક regname)
{
    જો (regname != 0 && !valid_yank_reg(regname, FALSE))
	પાછા FALSE;
    જો (regname == '_')		// black hole is always empty
	પાછા FALSE;
    get_yank_register(regname, FALSE);
    પાછા (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
do_record(પૂર્ણાંક c)
{
    char_u	    *p;
    સ્થિર પૂર્ણાંક	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    પૂર્ણાંક		    retval;

    જો (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	જો (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	નહીંતર
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    નહીંતર			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	સંદેશ("");
	p = get_recorded();
	જો (p == ખાલી)
	    retval = FAIL;
	નહીંતર
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
    પાછા retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    સ્થિર પૂર્ણાંક
stuff_yank(પૂર્ણાંક regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    જો (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	પાછા FAIL;
    }
    જો (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	પાછા OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    જો (y_append && y_current->y_array != ખાલી)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	જો (tmp == ખાલી)
	{
	    vim_free(p);
	    પાછા FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    નહીંતર
    {
	free_yank_all();
	જો ((y_current->y_array = ALLOC_ONE(string_T)) == ખાલી)
	{
	    vim_free(p);
	    પાછા FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    પાછા OK;
}

/*
 * Last executed register (@ command)
 */
સ્થિર પૂર્ણાંક execreg_lastc = NUL;

#જો defined(FEAT_VIMINFO)
    પૂર્ણાંક
get_execreg_lastc(શૂન્ય)
{
    પાછા execreg_lastc;
}

    શૂન્ય
set_execreg_lastc(પૂર્ણાંક lastc)
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
    સ્થિર char_u *
execreg_લીટી_continuation(string_T *લીટીઓ, long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    પૂર્ણાંક		j;
    char_u	*str;

    ga_init2(&ga, કદ(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    જ્યારે (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(લીટીઓ[cmd_start].string);
	જો (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    તોડો;
    }

    // join all the lines
    tmp = &લીટીઓ[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    માટે (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &લીટીઓ[j];
	p = skipwhite(tmp->string);
	જો (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    જો (ga.ga_len > 400)
	    {
		જો (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		નહીંતર
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
    પાછા str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
do_execreg(
    પૂર્ણાંક	    regname,
    પૂર્ણાંક	    colon,		// insert ':' before each line
    પૂર્ણાંક	    addcr,		// always add '\n' to end of line
    પૂર્ણાંક	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    પૂર્ણાંક		retval = OK;
    પૂર્ણાંક		remap;

    // repeat previous one
    જો (regname == '@')
    {
	જો (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    પાછા FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    જો (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	પાછા FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    જો (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    જો (regname == '_')
	પાછા OK;

    // use last command line
    જો (regname == ':')
    {
	જો (last_cmdline == ખાલી)
	{
	    emsg(_(e_no_previous_command_લીટી));
	    પાછા FAIL;
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
	જો (p != ખાલી)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    જો (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    નહીંતર
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    નહીંતર જો (regname == '=')
    {
	p = get_પદાવલિ_લીટી();
	જો (p == ખાલી)
	    પાછા FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    નહીંતર જો (regname == '.')		// use last inserted text
    {
	p = get_last_ઉમેરો_save();
	જો (p == ખાલી)
	{
	    emsg(_(e_no_inserted_text_yet));
	    પાછા FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    નહીંતર
    {
	get_yank_register(regname, FALSE);
	જો (y_current->y_array == ખાલી)
	    પાછા FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	માટે (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    પૂર્ણાંક	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    જો (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		જો (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    પાછા FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    જો (colon && i > 0)
	    {
		p = skipwhite(str);
		જો (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_લીટી_continuation(y_current->y_array, &i);
		    જો (str == ખાલી)
			પાછા FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    જો (free_str)
		vim_free(str);
	    જો (escaped == ખાલી)
		પાછા FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    જો (retval == FAIL)
		પાછા FAIL;
	    જો (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		પાછા FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    પાછા retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    સ્થિર શૂન્ય
put_reedit_in_typebuf(પૂર્ણાંક silent)
{
    જો (restart_edit == NUL)
	પાછા;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    જો (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    નહીંતર જો (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    નહીંતર જો (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    જો (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    સ્થિર પૂર્ણાંક
put_in_typebuf(
    char_u	*s,
    પૂર્ણાંક		esc,
    પૂર્ણાંક		colon,	    // add ':' before the line
    પૂર્ણાંક		silent)
{
    પૂર્ણાંક		retval = OK;

    put_reedit_in_typebuf(silent);
    જો (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    જો (retval == OK)
    {
	char_u	*p;

	જો (esc)
	    p = vim_strsave_escape_csi(s);
	નહીંતર
	    p = s;
	જો (p == ખાલી)
	    retval = FAIL;
	નહીંતર
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	જો (esc)
	    vim_free(p);
    }
    જો (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    પાછા retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
ઉમેરો_reg(
    પૂર્ણાંક		regname,
    પૂર્ણાંક		literally_arg)	// insert literally, not as if typed
{
    long	i;
    પૂર્ણાંક		retval = OK;
    char_u	*arg;
    પૂર્ણાંક		allocated;
    પૂર્ણાંક		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    જો (got_int)
	પાછા FAIL;

    // check for valid regname
    જો (regname != NUL && !valid_yank_reg(regname, FALSE))
	પાછા FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    જો (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    જો (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    નહીંતર જો (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	જો (arg == ખાલી)
	    પાછા FAIL;
	stuffescaped(arg, literally);
	જો (allocated)
	    vim_free(arg);
    }
    નહીંતર				// name or number register
    {
	જો (get_yank_register(regname, FALSE))
	    literally = TRUE;
	જો (y_current->y_array == ખાલી)
	    retval = FAIL;
	નહીંતર
	{
	    માટે (i = 0; i < y_current->y_size; ++i)
	    {
		જો (regname == '-' && y_current->y_type == MCHAR)
		{
		    પૂર્ણાંક dir = BACKWARD;
		    જો ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			જો (u_save_cursor() == FAIL)
			    પાછા FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			જો (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, ખાલી, dir, 1L, PUT_CURSEND);
		}
		નહીંતર
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    જો (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    પાછા retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    પૂર્ણાંક
get_spec_reg(
    પૂર્ણાંક		regname,
    char_u	**argp,
    પૂર્ણાંક		*allocated,	// return: TRUE when value was allocated
    પૂર્ણાંક		errmsg)		// give error message when failing
{
    પૂર્ણાંક		cnt;

    *argp = ખાલી;
    *allocated = FALSE;
    પસંદગી (regname)
    {
	સ્થિતિ '%':		// file name
	    જો (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    પાછા TRUE;

	સ્થિતિ '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    પાછા TRUE;

#ifdef FEAT_EVAL
	સ્થિતિ '=':		// result of expression
	    *argp = get_પદાવલિ_લીટી();
	    *allocated = TRUE;
	    પાછા TRUE;
#endif

	સ્થિતિ ':':		// last command line
	    જો (last_cmdline == ખાલી && errmsg)
		emsg(_(e_no_previous_command_લીટી));
	    *argp = last_cmdline;
	    પાછા TRUE;

	સ્થિતિ '/':		// last search-pattern
	    જો (last_શોધ_pat() == ખાલી && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_શોધ_pat();
	    પાછા TRUE;

	સ્થિતિ '.':		// last inserted text
	    *argp = get_last_ઉમેરો_save();
	    *allocated = TRUE;
	    જો (*argp == ખાલી && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    પાછા TRUE;

	સ્થિતિ Ctrl_F:		// Filename under cursor
	સ્થિતિ Ctrl_P:		// Path under cursor, expand via "path"
	    જો (!errmsg)
		પાછા FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, ખાલી);
	    *allocated = TRUE;
	    પાછા TRUE;

	સ્થિતિ Ctrl_W:		// word under cursor
	સ્થિતિ Ctrl_A:		// WORD (mnemonic All) under cursor
	    જો (!errmsg)
		પાછા FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : ખાલી;
	    *allocated = TRUE;
	    પાછા TRUE;

	સ્થિતિ Ctrl_L:		// Line under cursor
	    જો (!errmsg)
		પાછા FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    પાછા TRUE;

	સ્થિતિ '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    પાછા TRUE;
    }

    પાછા FALSE;
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
    પૂર્ણાંક
cmdline_paste_reg(
    પૂર્ણાંક regname,
    પૂર્ણાંક literally_arg,	// Insert text literally instead of "as typed"
    પૂર્ણાંક remcr)		// don't add CR characters
{
    long	i;
    પૂર્ણાંક		literally = literally_arg;

    જો (get_yank_register(regname, FALSE))
	literally = TRUE;
    જો (y_current->y_array == ખાલી)
	પાછા FAIL;

    માટે (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	જો ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	જો (got_int)
	    પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    શૂન્ય
shift_delete_registers(શૂન્ય)
{
    પૂર્ણાંક		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    માટે (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    જો (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = ખાલી;		// set register one to empty
}

#જો defined(FEAT_EVAL)
    સ્થિર શૂન્ય
add_regtype_to_dict(પૂર્ણાંક regname, dict_T *dict, char_u *buf, પૂર્ણાંક bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    પસંદગી (get_reg_type(regname, &reglen))
    {
	સ્થિતિ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    તોડો;
	સ્થિતિ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    તોડો;
	સ્થિતિ MBLOCK:
	    buflen = vim_snprintf_safelen((અક્ષર *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    તોડો;
	મૂળભૂત:
	    buf[0] = NUL;
	    buflen = 0;
	    તોડો;
    }
    (શૂન્ય)dict_add_string_len(dict, "regtype", buf, (પૂર્ણાંક)buflen);
}

    શૂન્ય
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    સ્થિર પૂર્ણાંક	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    પૂર્ણાંક		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    જો (recursive)
	પાછા;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    જો (list == ખાલી)
	પાછા;

    // yanked text contents
    માટે (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (પૂર્ણાંક)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    જો (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (શૂન્ય)dict_add_string_len(v_event, "regname", buf, (પૂર્ણાંક)buflen);

    // motion type: inclusive or exclusive
    (શૂન્ય)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (શૂન્ય)dict_add_string_len(v_event, "operator", buf, (પૂર્ણાંક)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, કદ(buf));

    // selection type - visual or not
    (શૂન્ય)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_વસ્તુઓ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, ખાલી, ખાલી, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    સ્થિર શૂન્ય
put_do_autocmd(
	પૂર્ણાંક	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *ઉમેરો,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	પૂર્ણાંક	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    સ્થિર bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    પૂર્ણાંક		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    જો (recursive || regname == '_')
	પાછા;

    જો (regname != '.' && ઉમેરો == ખાલી
	    && reg == ખાલી)
	// Can happen when pasting text in normal mode in a terminal buffer
	પાછા;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    જો (list == ખાલી)
	પાછા;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    જો (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    જો (regname == '.')
    {
	જો (last_ઉમેરો_ga.ga_data != ખાલી)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_ઉમેરો_ga.ga_data,
		    (પૂર્ણાંક)last_ઉમેરો_ga.ga_len);
    }
    નહીંતર જો (ઉમેરો != ખાલી)
    {
	list_append_string(list, ઉમેરો->string, (પૂર્ણાંક)ઉમેરો->length);
    }
    નહીંતર
    {
	માટે (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (પૂર્ણાંક)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    જો (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (શૂન્ય)dict_add_string_len(v_event, "regname", buf, (પૂર્ણાંક)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (શૂન્ય)dict_add_string_len(v_event, "operator", buf, (પૂર્ણાંક)buflen);

    add_regtype_to_dict(regname, v_event, buf, કદ(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (શૂન્ય)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_વસ્તુઓ_ro(v_event);

    recursive = true;
    textlock++;
    જો (post)
	apply_autocmds(EVENT_TEXTPUTPOST, ખાલી, ખાલી, FALSE, curbuf);
    નહીંતર
	apply_autocmds(EVENT_TEXTPUTPRE, ખાલી, ખાલી, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    શૂન્ય
init_yank(શૂન્ય)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < અંક_REGISTERS; ++i)
	y_regs[i].y_array = ખાલી;
}

#જો defined(EXITFREE)
    શૂન્ય
clear_registers(શૂન્ય)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < અંક_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	જો (y_current->y_array != ખાલી)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    સ્થિર શૂન્ય
free_yank(long n)
{
    જો (y_current->y_array == ખાલી)
	પાછા;

    long	    i;

    માટે (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    શૂન્ય
free_yank_all(શૂન્ય)
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
    પૂર્ણાંક
op_yank(oparg_T *oap, પૂર્ણાંક deleting, પૂર્ણાંક mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    પૂર્ણાંક			yanktype = oap->motion_type;
    long		yanklines = oap->લીટી_ગણતરી;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    સંરચના block_def	bd;

				    // check for read-only register
    જો (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	પાછા FAIL;
    }
    જો (oap->regname == '_')	    // black hole: nothing to do
	પાછા OK;

#ifdef FEAT_CLIPBOARD
    જો ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	સંદેશ_warn_missing_clipboard();
    }
#endif

    જો (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    જો (y_append && y_current->y_array != ખાલી)
	y_current = &newreg;
    નહીંતર
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    જો (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(કદ(string_T) * yanklines, TRUE);
    જો (y_current->y_array == ખાલી)
    {
	y_current = curr;
	પાછા FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    જો (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	જો (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    માટે ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	પસંદગી (y_current->y_type)
	{
	    સ્થિતિ MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		જો (yank_copy_લીટી(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    જાઓ fail;
		તોડો;

	    સ્થિતિ MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		જો ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == ખાલી)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    જાઓ fail;
		}
		તોડો;

	    સ્થિતિ MCHAR:
		{
		    પૂર્ણાંક tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (પૂર્ણાંક)STRLEN(bd.textstart);
		    જો (tmp < bd.textlen)
			bd.textlen = tmp;

		    જો (yank_copy_લીટી(&bd, y_idx, FALSE) == FAIL)
			જાઓ fail;
		    તોડો;
		}
		// NOTREACHED
	}
    }

    જો (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	જો (new_ptr == ખાલી)
	    જાઓ fail;
	માટે (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	જો (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	જો (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == ખાલી)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    જો (pnew == ખાલી)
	    {
		y_idx = y_current->y_size - 1;
		જાઓ fail;
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
	નહીંતર
	    y_idx = 0;
	જ્યારે (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    જો (mess)			// Display message about yank?
    {
	જો (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	જો (yanklines > p_report)
	{
	    અક્ષર namebuf[100];

	    જો (oap->regname == NUL)
		*namebuf = NUL;
	    નહીંતર
		vim_snprintf(namebuf, કદ(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    જો (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    નહીંતર
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	જો (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	જો (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    જો (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	જો (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    જો (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	જો (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    જો (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	જો (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    જો (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# જો defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	જો (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    જો (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#જો defined(FEAT_EVAL)
    જો (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    પાછા OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    પાછા FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    સ્થિર પૂર્ણાંક
yank_copy_લીટી(સંરચના block_def *bd, long y_idx, પૂર્ણાંક exclude_trailing_space)
{
    char_u	*pnew;

    જો (exclude_trailing_space)
	bd->endspaces = 0;
    જો ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == ખાલી)
	પાછા FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    જો (exclude_trailing_space)
    {
	પૂર્ણાંક s = bd->textlen + bd->endspaces;

	જ્યારે (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    પાછા OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    સ્થિર શૂન્ય
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    કદ(string_T) * y_current->y_size, TRUE);
    જો (y_current->y_array == ખાલી)
	y_current->y_size = 0;
    નહીંતર
	માટે (j = 0; j < y_current->y_size; ++j)
	{
	    જો ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == ખાલી)
	    {
		free_yank(j);
		y_current->y_size = 0;
		તોડો;
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
    શૂન્ય
do_put(
    પૂર્ણાંક		regname,
    char_u	*પદાવલિ_result,	// result for regname "=" when compiled
    પૂર્ણાંક		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	ગણતરી,
    પૂર્ણાંક		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    પૂર્ણાંક		yanklen;
    પૂર્ણાંક		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    પૂર્ણાંક		y_type;
    long	y_size;
    પૂર્ણાંક		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = ખાલી;
    yankreg_T	*y_current_used = ખાલી;
    long	nr_લીટીઓ = 0;
    string_T	ઉમેરો_string;
    પૂર્ણાંક		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned પૂર્ણાંક cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    જો (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(શૂન્ય)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    જો (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    જો (regname == '.')
    {
	જો (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	જો (has_textput_events)
	    add_last_ઉમેરો++;
#endif

	(શૂન્ય)stuff_inserted((dir == FORWARD ? (ગણતરી == -1 ? 'o' : 'a') :
				    (ગણતરી == -1 ? 'O' : 'i')), ગણતરી, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	જો (has_textputpre())
	    put_do_autocmd('.', ખાલી, ખાલી, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	જો (has_textputpost())
	    put_do_autocmd('.', ખાલી, ખાલી, true, dir);

	જો (has_textput_events && --add_last_ઉમેરો == 0)
	    ga_clear(&last_ઉમેરો_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	જો ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	પાછા;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    ઉમેરો_string.string = ખાલી;
    જો (regname == '=' && પદાવલિ_result != ખાલી)
	ઉમેરો_string.string = પદાવલિ_result;
    નહીંતર જો (get_spec_reg(regname, &ઉમેરો_string.string, &allocated, TRUE)
		&& ઉમેરો_string.string == ખાલી)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	પાછા;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    જો (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	જાઓ end;

    જો (ઉમેરો_string.string != ખાલી)
    {
	ઉમેરો_string.length = STRLEN(ઉમેરો_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	જો (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    માટે (;;)
	    {
		y_size = 0;
		ptr = ઉમેરો_string.string;
		ptrlen = ઉમેરો_string.length;
		જ્યારે (ptr != ખાલી)
		{
		    જો (y_array != ખાલી)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    જો (tmp == ખાલી)
		    {
			જો (y_array != ખાલી)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    નહીંતર
		    {
			જો (y_array != ખાલી)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			જો (*tmp == NUL)
			{
			    y_type = MLINE;
			    તોડો;
			}
		    }
		    ptr = tmp;
		}
		જો (y_array != ખાલી)
		    તોડો;
		y_array = ALLOC_MULT(string_T, y_size);
		જો (y_array == ખાલી)
		    જાઓ end;
	    }
	}
	નહીંતર
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &ઉમેરો_string;
	}
#ifdef FEAT_EVAL
	જો (has_textputpre())
	    put_do_autocmd(regname, ખાલી, &ઉમેરો_string, false, dir);
#endif
    }
    નહીંતર
    {
#ifdef FEAT_EVAL
	જો (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, ખાલી, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    જો (y_type == MLINE)
    {
	જો (flags & PUT_લીટી_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    જો (u_save_cursor() == FAIL)
		જાઓ end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    જો (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    જો (ptr == ખાલી)
		જાઓ end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    જો (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    જો (ptr == ખાલી)
		જાઓ end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_લીટીઓ;
	    dir = FORWARD;
	}
	જો (flags & PUT_લીટી_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    જો (flags & PUT_લીટી)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    જો (y_size == 0 || y_array == ખાલી)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	જાઓ end;
    }

    જો (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	જો (lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	    lnum = curbuf->b_ml.ml_લીટી_ગણતરી + 1;
	જો (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    જાઓ end;
    }
    નહીંતર જો (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	જો (dir == BACKWARD)
	    (શૂન્ય)hasFolding(lnum, &lnum, ખાલી);
	નહીંતર
	    (શૂન્ય)hasFolding(lnum, ખાલી, &lnum);
#endif
	જો (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	જો ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    જાઓ end;
#ifdef FEAT_FOLDING
	જો (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	નહીંતર
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    નહીંતર જો (u_save_cursor() == FAIL)
	જાઓ end;

    જો (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	જો (gchar_cursor() == TAB)
	{
	    પૂર્ણાંક viscol = getviscol();
	    પૂર્ણાંક ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    જો (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#નહીંતર
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    નહીંતર
		curwin->w_cursor.coladd = 0;
	}
	નહીંતર જો (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    જો (y_type == MBLOCK)
    {
	પૂર્ણાંક	delcount;
	પૂર્ણાંક	incr = 0;
	સંરચના block_def bd;
	long	j;
	પૂર્ણાંક	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	જો (dir == FORWARD && c != NUL)
	{
	    જો (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, ખાલી, &endcol2, 0);
	    નહીંતર
		getvcol(curwin, &curwin->w_cursor, ખાલી, ખાલી, &col, 0);

	    જો (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    નહીંતર
		જો (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	નહીંતર
	    getvcol(curwin, &curwin->w_cursor, &col, ખાલી, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	જો (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    જો (dir == FORWARD && c == NUL)
		++col;
	    જો (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    જો (c == TAB)
	    {
		જો (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		જો (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	માટે (i = 0; i < y_size; ++i)
	{
	    પૂર્ણાંક		    spaces = 0;
	    અક્ષર	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    જો (curwin->w_cursor.lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	    {
		જો (ml_append(curbuf->b_ml.ml_લીટી_ગણતરી, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    તોડો;
		++nr_લીટીઓ;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    જ્યારે (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    જો (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    નહીંતર જો (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		જો (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		જો (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (પૂર્ણાંક)y_array[i].length;

	    જો ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		જ્યારે (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		જો (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    જો (yanklen + spaces != 0
		     && ગણતરી > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		તોડો;
	    }

	    totlen = ગણતરી * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    જો (newp == ખાલી)
		તોડો;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    માટે (j = 0; j < ગણતરી; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		જો ((j < ગણતરી - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		નહીંતર
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
	    જો (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_લીટીઓ(lnum, 0, curwin->w_cursor.lnum - nr_લીટીઓ, nr_લીટીઓ);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	જો (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	જો (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    જો (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	નહીંતર
	    curwin->w_cursor.lnum = lnum;
    }
    નહીંતર
    {
	pos_T	new_cursor;

	yanklen = (પૂર્ણાંક)y_array[0].length;

	// Character or Line mode
	જો (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    જો (dir == FORWARD && gchar_cursor() != NUL)
	    {
		જો (has_mbyte)
		{
		    પૂર્ણાંક bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    જો (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		નહીંતર
		{
		    ++col;
		    જો (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	નહીંતર જો (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	જો (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    પૂર્ણાંક		first_byte_off = 0;

	    જો (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		જો (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		જો (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, ખાલી, &vcol, ખાલી, 0);
		}
	    }

	    જો (ગણતરી == 0 || yanklen == 0)
	    {
		જો (VIsual_active)
		    lnum = end_lnum;
	    }
	    નહીંતર જો (ગણતરી > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    નહીંતર
	    {
		totlen = ગણતરી * yanklen;
		કરો
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    જો (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			જો (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			નહીંતર
			    col = MAXCOL;
		    }
		    જો (VIsual_active && col > oldlen)
		    {
			lnum++;
			ચાલુ;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    જો (newp == ખાલી)
			જાઓ end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    માટે (i = 0; i < ગણતરી; ++i)
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
		    જો (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    જો (VIsual_active)
			lnum++;
		} જ્યારે (VIsual_active && lnum <= end_lnum);

		જો (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    જો (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    નહીંતર
		curwin->w_cursor.col -= first_byte_off;
	}
	નહીંતર
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    પૂર્ણાંક		indent;
	    પૂર્ણાંક		orig_indent = 0;
	    પૂર્ણાંક		indent_diff = 0;	// init for gcc
	    પૂર્ણાંક		first_indent = TRUE;
	    પૂર્ણાંક		lendiff = 0;
	    long	cnt;

	    જો (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    માટે (cnt = 1; cnt <= ગણતરી; ++cnt)
	    {
		i = 0;
		જો (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (પૂર્ણાંક)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    જો (newp == ખાલી)
			જાઓ error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    જો (newp == ખાલી)
			જાઓ error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		માટે (; i < y_size; ++i)
		{
		    જો (y_type != MCHAR || i < y_size - 1)
		    {
			જો (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    જાઓ error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_લીટીઓ;
		    જો (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			જો (cnt == ગણતરી && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			જો (*ptr == '#' && preprocs_ડાબું())
			    indent = 0;     // Leave # lines at start
			નહીંતર જો (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			નહીંતર જો (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			નહીંતર જો ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(શૂન્ય)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			જો (cnt == ગણતરી && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		જો (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    જો (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		જો (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_લીટીઓ, 0L);

	    // note changed text for displaying and folding
	    જો (y_type == MCHAR)
		changed_લીટીઓ(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_લીટીઓ);
	    નહીંતર
		changed_લીટીઓ(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_લીટીઓ);
	    જો (y_current_used != ખાલી && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		જાઓ end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    જો (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		જો (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    નહીંતર
		curbuf->b_op_end.col = 0;

	    જો (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    નહીંતર જો (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		જો (y_type == MLINE)
		{
		    જો (lnum >= curbuf->b_ml.ml_લીટી_ગણતરી)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
		    નહીંતર
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		નહીંતર
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    જો (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    નહીંતર જો (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		જો (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    નહીંતર	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_લીટીઓ);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    પૂર્ણાંક len = ml_get_curline_len();
    જો (curwin->w_cursor.col > len)
    {
	જો (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    જો (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    જો (has_textputpost())
    {
	જો (ઉમેરો_string.string == ખાલી)
	    put_do_autocmd(regname, y_current, ખાલી, true, dir);
	નહીંતર
	    put_do_autocmd(regname, ખાલી, &ઉમેરો_string, true, dir);
    }
#endif

    જો (allocated)
	vim_free(ઉમેરો_string.string);
    જો (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    પૂર્ણાંક
get_register_name(પૂર્ણાંક અંક)
{
    જો (અંક == -1)
	પાછા '"';
    નહીંતર જો (અંક < 10)
	પાછા અંક + '0';
    નહીંતર જો (અંક == DELETION_REGISTER)
	પાછા '-';
#જો defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    નહીંતર જો (અંક == STAR_REGISTER)
	પાછા '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    નહીંતર જો (clipmethod == CLIPMETHOD_PROVIDER && અંક == REAL_PLUS_REGISTER)
	પાછા '+';
    નહીંતર જો (clipmethod != CLIPMETHOD_PROVIDER && અંક == PLUS_REGISTER)
	પાછા '+';
#endif
    નહીંતર
	પાછા અંક + 'a' - 10;
}

#જો defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    પૂર્ણાંક
get_unname_register(શૂન્ય)
{
    પાછા y_previous == ખાલી ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    શૂન્ય
ex_display(exarg_T *eap)
{
    પૂર્ણાંક		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    પૂર્ણાંક		name;
    પૂર્ણાંક		attr;
    char_u	*arg = eap->arg;
    પૂર્ણાંક		clen;
    પૂર્ણાંક		type;
    string_T	ઉમેરો;

    silence_w23_w24_સંદેશ++;
    જો (arg != ખાલી && *arg == NUL)
	arg = ખાલી;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    સંદેશ_puts_title(_("\nType Name Content"));
    માટે (i = -1; i < અંક_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	પસંદગી (get_reg_type(name, ખાલી))
	{
	    સ્થિતિ MLINE: type = 'l'; તોડો;
	    સ્થિતિ MCHAR: type = 'c'; તોડો;
	    મૂળભૂત:	type = 'b'; તોડો;
	}
	જો (arg != ખાલી && vim_strchr(arg, name) == ખાલી
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == ખાલી)
#endif
		)
	    ચાલુ;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	જો (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (શૂન્ય)may_get_selection(name);
#endif

	જો (i == -1)
	{
	    જો (y_previous != ખાલી)
		yb = y_previous;
	    નહીંતર
		yb = &(y_regs[0]);
	}
	નહીંતર
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	જો (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != ખાલી &&
		    (yb == y_previous || yb == &y_regs[0])))
	    ચાલુ;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	જો (yb->y_array != ખાલી)
	{
	    પૂર્ણાંક do_show = FALSE;

	    માટે (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    જો (do_show || yb->y_size == 0)
	    {
		સંદેશ_putchar('\n');
		સંદેશ_puts("  ");
		સંદેશ_putchar(type);
		સંદેશ_puts("  ");
		સંદેશ_putchar('"');
		સંદેશ_putchar(name);
		સંદેશ_puts("   ");

		n = (પૂર્ણાંક)Columns - 11;
		માટે (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    જો (j)
		    {
			સંદેશ_puts_attr("^J", attr);
			n -= 2;
		    }
		    માટે (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			સંદેશ_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		જો (n > 1 && yb->y_type == MLINE)
		    સંદેશ_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    ઉમેરો = get_last_ઉમેરો();
    જો ((p = ઉમેરો.string) != ખાલી
		  && (arg == ખાલી || vim_strchr(arg, '.') != ખાલી) && !got_int
						      && !message_filtered(p))
    {
	સંદેશ_puts("\n  c  \".   ");
	dis_સંદેશ(p, TRUE);
    }

    // display last command line
    જો (last_cmdline != ખાલી && (arg == ખાલી || vim_strchr(arg, ':') != ખાલી)
			       && !got_int && !message_filtered(last_cmdline))
    {
	સંદેશ_puts("\n  c  \":   ");
	dis_સંદેશ(last_cmdline, FALSE);
    }

    // display current file name
    જો (curbuf->b_fname != ખાલી
	    && (arg == ખાલી || vim_strchr(arg, '%') != ખાલી) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	સંદેશ_puts("\n  c  \"%   ");
	dis_સંદેશ(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    જો ((arg == ખાલી || vim_strchr(arg, '#') != ખાલી) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	જો (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    સંદેશ_puts("\n  c  \"#   ");
	    dis_સંદેશ(fname, FALSE);
	}
    }

    // display last search pattern
    જો (last_શોધ_pat() != ખાલી
		 && (arg == ખાલી || vim_strchr(arg, '/') != ખાલી) && !got_int
				      && !message_filtered(last_શોધ_pat()))
    {
	સંદેશ_puts("\n  c  \"/   ");
	dis_સંદેશ(last_શોધ_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    જો (પદાવલિ_લીટી != ખાલી && (arg == ખાલી || vim_strchr(arg, '=') != ખાલી)
				  && !got_int && !message_filtered(પદાવલિ_લીટી))
    {
	સંદેશ_puts("\n  c  \"=   ");
	dis_સંદેશ(પદાવલિ_લીટી, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_સંદેશ--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    સ્થિર શૂન્ય
dis_સંદેશ(
    char_u	*p,
    પૂર્ણાંક		skip_esc)	    // if TRUE, ignore trailing ESC
{
    પૂર્ણાંક		n;
    પૂર્ણાંક		l;

    n = (પૂર્ણાંક)Columns - 6;
    જ્યારે (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	જો (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    સંદેશ_outtrans_len(p, l);
	    p += l;
	}
	નહીંતર
	    સંદેશ_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#જો defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    શૂન્ય
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
get_reg_type(પૂર્ણાંક regname, long *reglen)
{
    પસંદગી (regname)
    {
	સ્થિતિ '%':		// file name
	સ્થિતિ '#':		// alternate file name
	સ્થિતિ '=':		// expression
	સ્થિતિ ':':		// last command line
	સ્થિતિ '/':		// last search-pattern
	સ્થિતિ '.':		// last inserted text
	સ્થિતિ Ctrl_F:		// Filename under cursor
	સ્થિતિ Ctrl_P:		// Path under cursor, expand via "path"
	સ્થિતિ Ctrl_W:		// word under cursor
	સ્થિતિ Ctrl_A:		// WORD (mnemonic All) under cursor
	સ્થિતિ '_':		// black hole: always empty
	    પાછા MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    જો (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    જો (regname != NUL && !valid_yank_reg(regname, FALSE))
	પાછા MAUTO;

    get_yank_register(regname, FALSE);

    જો (y_current->y_array != ખાલી)
    {
	જો (reglen != ખાલી && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	પાછા y_current->y_type;
    }
    પાછા MAUTO;
}

#જો defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    સ્થિર char_u *
getreg_wrap_one_લીટી(char_u *s, પૂર્ણાંક flags)
{
    જો (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	જો (list != ખાલી)
	{
	    જો (list_append_string(list, ખાલી, -1) == FAIL)
	    {
		list_free(list);
		પાછા ખાલી;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	પાછા (char_u *)list;
    }
    પાછા s;
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
get_reg_contents(પૂર્ણાંક regname, પૂર્ણાંક flags)
{
    linenr_T	i;
    char_u	*retval;
    પૂર્ણાંક		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    જો (regname == '=')
    {
	જો (flags & GREG_NO_પદાવલિ)
	    પાછા ખાલી;
	જો (flags & GREG_પદાવલિ_SRC)
	    પાછા getreg_wrap_one_લીટી(get_પદાવલિ_લીટી_src(), flags);
	પાછા getreg_wrap_one_લીટી(get_પદાવલિ_લીટી(), flags);
    }

    જો (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    જો (regname != NUL && !valid_yank_reg(regname, FALSE))
	પાછા ખાલી;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    જો (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    જો (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	જો (retval == ખાલી)
	    પાછા ખાલી;
	જો (allocated)
	    પાછા getreg_wrap_one_લીટી(retval, flags);
	પાછા getreg_wrap_one_લીટી(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    જો (y_current->y_array == ખાલી)
	પાછા ખાલી;

    જો (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	જો (list == ખાલી)
	    પાછા ખાલી;
	માટે (i = 0; i < y_current->y_size; ++i)
	{
	    જો (list_append_string(list, y_current->y_array[i].string,
		(પૂર્ણાંક)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		પાછા ખાલી;
	    }
	}
	પાછા (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    માટે (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	જો (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    જો (retval == ખાલી)
	પાછા ખાલી;

    // Copy the lines of the yank register into the string.
    len = 0;
    માટે (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	જો (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    પાછા retval;
}

    સ્થિર પૂર્ણાંક
init_write_reg(
    પૂર્ણાંક		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    પૂર્ણાંક		must_append,
    પૂર્ણાંક		*yank_type UNUSED)
{
    જો (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	પાછા FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    જો (!y_append && !must_append)
	free_yank_all();
    પાછા OK;
}

    સ્થિર શૂન્ય
finish_write_reg(
    પૂર્ણાંક		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    જો (name != '"')
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
    શૂન્ય
write_reg_contents(
    પૂર્ણાંક		name,
    char_u	*str,
    પૂર્ણાંક		maxlen,
    પૂર્ણાંક		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    શૂન્ય
write_reg_contents_lst(
    પૂર્ણાંક		name,
    char_u	**strings,
    પૂર્ણાંક		maxlen UNUSED,
    પૂર્ણાંક		must_append,
    પૂર્ણાંક		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    જો (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	જો (strings[0] == ખાલી)
	    s = (char_u *)"";
	નહીંતર જો (strings[1] != ખાલી)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_લીટીઓ), name);
	    પાછા;
	}
	નહીંતર
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	પાછા;
    }

    જો (name == '_')	    // black hole: nothing to do
	પાછા;

    જો (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	પાછા;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    શૂન્ય
write_reg_contents_ex(
    પૂર્ણાંક		name,
    char_u	*str,
    પૂર્ણાંક		maxlen,
    પૂર્ણાંક		must_append,
    પૂર્ણાંક		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    જો (maxlen >= 0)
	len = maxlen;
    નહીંતર
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    જો (name == '/')
    {
	set_last_શોધ_pat(str, RE_શોધ, TRUE, TRUE);
	પાછા;
    }

    જો (name == '#')
    {
	જો (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  પાછા;
	}

	buf_T	*buf;

	જો (VIM_ISDIGIT(*str))
	{
	    પૂર્ણાંક	અંક = atoi((અક્ષર *)str);

	    buf = buflist_findnr(અંક);
	    જો (buf == ખાલી)
		semsg(_(e_buffer_nr_does_not_exist), (long)અંક);
	}
	નહીંતર
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	જો (buf == ખાલી)
	    પાછા;
	curwin->w_alt_fnum = buf->b_fnum;
	પાછા;
    }

    જો (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	જો (p == ખાલી)
	    પાછા;
	જો (must_append && પદાવલિ_લીટી != ખાલી)
	{
	    s = concat_str(પદાવલિ_લીટી, p);
	    vim_free(p);
	    p = s;
	}
	set_પદાવલિ_લીટી(p, ખાલી);
	પાછા;
    }

    જો (name == '_')	    // black hole: nothing to do
	પાછા;

    જો (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	પાછા;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#જો defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    શૂન્ય
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    પૂર્ણાંક		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    પૂર્ણાંક		str_list)	// TRUE if str is char_u **
{
    પૂર્ણાંક		type;			// MCHAR, MLINE or MBLOCK
    પૂર્ણાંક		lnum;
    long	start;
    long	i;
    પૂર્ણાંક		extra;
    પૂર્ણાંક		newlines;		// number of lines added
    પૂર્ણાંક		extraline = 0;		// extra line at the end
    પૂર્ણાંક		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    જો (y_ptr->y_array == ખાલી)		// NULL means empty register
	y_ptr->y_size = 0;

    જો (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    નહીંતર
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    જો (str_list)
    {
	માટે (ss = (char_u **) str; *ss != ખાલી; ++ss)
	    ++newlines;
    }
    નહીંતર
    {
	માટે (i = 0; i < len; i++)
	    જો (str[i] == '\n')
		++newlines;
	જો (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	જો (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    જો (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	પાછા;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * કદ(string_T), TRUE);
    જો (pp == ખાલી)	// out of memory
	પાછા;
    માટે (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    જો (str_list)
    {
	માટે (ss = (char_u **) str; *ss != ખાલી; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    જો (type == MBLOCK)
	    {
		પૂર્ણાંક charlen = vim_strsize(*ss);

		જો (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    નહીંતર
    {
	માટે (start = 0; start < len + extraline; start += i + 1)
	{
	    પૂર્ણાંક charlen = 0;

	    માટે (i = start; i < len;)	// find the end of the line
	    {
		જો (str[i] == '\n')
		    તોડો;
		જો (type == MBLOCK)
		{
		    જો (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    નહીંતર
			charlen += byte2cells(str[i]);
		}

		જો (str[i] == NUL)
		    i++; // registers can have NUL chars
		નહીંતર
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    જો (charlen > maxlen)
		maxlen = charlen;
	    જો (append)
	    {
		--lnum;
		extra = (પૂર્ણાંક)y_ptr->y_array[lnum].length;
	    }
	    નહીંતર
		extra = 0;
	    s = alloc(i + extra + 1);
	    જો (s == ખાલી)
		તોડો;
	    જો (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    જો (append)
		vim_free(y_ptr->y_array[lnum].string);
	    જો (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    જ્યારે (--extra >= 0)
	    {
		જો (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    જો (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    નહીંતર
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
