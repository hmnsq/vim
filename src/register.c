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
ساكن yankreg_T	y_regs[انگ_REGISTERS];

ساكن yankreg_T	*y_current;	    // ptr to current yankreg
ساكن صحيح		y_append;	    // TRUE when appending
ساكن yankreg_T	*y_اڳيون = عدم; // ptr to last written yankreg

ساكن صحيح	stuff_yank(صحيح, char_u *);
ساكن فراغ	put_reedit_اندريون_typebuf(صحيح silent);
ساكن صحيح	put_اندريون_typebuf(char_u *s, صحيح esc, صحيح colon, صحيح silent);
ساكن صحيح	yank_copy_سٽ(بنية بلاڪ_def *bd, ڊگهو y_idx, صحيح exclude_trailing_space);
#جيڪڏهن_وصف_ٿيل HAVE_CLIPMETHOD
ساكن فراغ	copy_yank_reg(yankreg_T *reg);
#جيڪڏهن_پورو
ساكن فراغ	dis_نياپو(char_u *p, صحيح skip_esc);

#إذا defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(فراغ)
{
    أرجع y_regs;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_رجسٽر(صحيح reg)
{
    أرجع &y_regs[reg];
}
#جيڪڏهن_پورو

    yankreg_T *
get_y_current(فراغ)
{
    أرجع y_current;
}

    yankreg_T *
get_y_اڳيون(فراغ)
{
    أرجع y_اڳيون;
}

    فراغ
سيٽ_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    فراغ
سيٽ_y_اڳيون(yankreg_T *yreg)
{
    y_اڳيون = yreg;
}

    فراغ
reset_y_append(فراغ)
{
    y_append = FALSE;
}


#إذا defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
ساكن char_u	*اظهار_سٽ = عدم;
ساكن exarg_T	*اظهار_eap = عدم;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    صحيح
get_اظهار_رجسٽر(فراغ)
{
    char_u	*new_سٽ;

    new_سٽ = getcmdline('=', 0L, 0, 0);
    إذا (new_سٽ == عدم)
	أرجع NUL;
    إذا (*new_سٽ == NUL)	// use previous line
	vim_free(new_سٽ);
    وإلا
	سيٽ_اظهار_سٽ(new_سٽ, عدم);
    أرجع '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    فراغ
سيٽ_اظهار_سٽ(char_u *new_سٽ, exarg_T *eap)
{
    vim_free(اظهار_سٽ);
    اظهار_سٽ = new_سٽ;
    اظهار_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_اظهار_سٽ(فراغ)
{
    char_u	*اظهار_copy;
    char_u	*rv;
    ساكن صحيح	nested = 0;

    إذا (اظهار_سٽ == عدم)
	أرجع عدم;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    اظهار_copy = vim_strsave(اظهار_سٽ);
    إذا (اظهار_copy == عدم)
	أرجع عدم;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    إذا (nested >= 10)
	أرجع اظهار_copy;

    ++nested;
    rv = eval_منزل_string_eap(اظهار_copy, TRUE, اظهار_eap, FALSE);
    --nested;
    vim_free(اظهار_copy);
    أرجع rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ساكن char_u *
get_اظهار_سٽ_src(فراغ)
{
    إذا (اظهار_سٽ == عدم)
	أرجع عدم;
    أرجع vim_strsave(اظهار_سٽ);
}
#جيڪڏهن_پورو // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    صحيح
valid_yank_reg(
    صحيح	    regname,
    صحيح	    writing)	    // if TRUE check for writable registers
{
    إذا (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
				    "/#.%:="
#وإلا
				    "/#.%:"
#جيڪڏهن_پورو
					, regname) != عدم)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#إذا defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
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
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DND
	    || (!writing && regname == '~')
#جيڪڏهن_پورو
							)
	أرجع TRUE;
#جيڪڏهن_وصف_نه_ٿيل FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    وإلا إذا (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	نياپو_warn_missing_clipboard();
	أرجع FALSE;
    }
#جيڪڏهن_پورو
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
get_yank_رجسٽر(صحيح regname, صحيح writing)
{
    صحيح	    i;
    صحيح	    ret = FALSE;

    y_append = FALSE;
    إذا ((regname == 0 || regname == '"') && !writing && y_اڳيون != عدم)
    {
	y_current = y_اڳيون;
	أرجع ret;
    }
    i = regname;
    إذا (VIM_ISDIGIT(i))
	i -= '0';
    وإلا إذا (ASCII_ISLOWER(i))
	i = Char_Ord_هيٺيون(i) + 10;
    وإلا إذا (ASCII_ISUPPER(i))
    {
	i = Char_Ord_مٿي(i) + 10;
	y_append = TRUE;
    }
    وإلا إذا (regname == '-')
	i = DELETION_رجسٽر;
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    وإلا إذا (clip_تارو.موجود && regname == '*')
    {
	i = تارو_رجسٽر;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    وإلا إذا (clip_plus.موجود && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	إذا (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_رجسٽر;
	وإلا
# endif
	    i = PLUS_رجسٽر;
	ret = TRUE;
    }
#وإلاإذا defined(FEAT_CLIPBOARD_PROVIDER)
    وإلا إذا (regname == '*')
    {
	i = تارو_رجسٽر;
	ret = TRUE;
    }
    وإلا إذا (regname == '+')
    {
	i = REAL_PLUS_رجسٽر;
	ret = TRUE;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DND
    وإلا إذا (!writing && regname == '~')
	i = TILDE_رجسٽر;
#جيڪڏهن_پورو
    وإلا		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    إذا (writing)	// remember the register we write into for do_put()
	y_اڳيون = y_current;
    أرجع ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    فراغ *
get_رجسٽر(
    صحيح		name,
    صحيح		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    صحيح		i;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	إذا (name == '*' && clip_تارو.موجود)
	{
	    إذا (clip_isautosel_تارو())
		clip_update_selection(&clip_تارو);
	    may_get_selection(name);
	}
	إذا (name == '+' && clip_plus.موجود)
	{
	    إذا (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#جيڪڏهن_پورو

    get_yank_رجسٽر(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    إذا (reg == عدم)
	أرجع (فراغ *)عدم;

    *reg = *y_current;
    إذا (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	إذا (reg->y_سائيز == 0 || y_current->y_array == عدم)
	    reg->y_array = عدم;
	وإلا
	    reg->y_array = ALLOC_MULT(string_T, reg->y_سائيز);
	إذا (reg->y_array != عدم)
	{
	    لكل (i = 0; i < reg->y_سائيز; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].ڊيگهائي);
		إذا (reg->y_array[i].string == عدم)
		{
		    // The allocation failed so clean up and exit
		    طالما (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    أرجع (فراغ *)عدم;
		}

		reg->y_array[i].ڊيگهائي = y_current->y_array[i].ڊيگهائي;
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
put_رجسٽر(صحيح name, فراغ *reg)
{
    get_yank_رجسٽر(name, 0);
    free_yank_all();
    *y_current = *(yankreg_T *)reg;
    vim_free(reg);

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    // Send text written to clipboard register to the clipboard.
    may_سيٽ_selection();
#جيڪڏهن_پورو
}

#إذا defined(FEAT_CLIPBOARD)
    فراغ
free_رجسٽر(فراغ *reg)
{
    yankreg_T tmp;

    tmp = *y_current;
    *y_current = *(yankreg_T *)reg;
    free_yank_all();
    vim_free(reg);
    *y_current = tmp;
}
#جيڪڏهن_پورو

/*
 * return TRUE if the current yank register has type MLINE
 */
    صحيح
yank_رجسٽر_mline(صحيح regname)
{
    إذا (regname != 0 && !valid_yank_reg(regname, FALSE))
	أرجع FALSE;
    إذا (regname == '_')		// black hole is always empty
	أرجع FALSE;
    get_yank_رجسٽر(regname, FALSE);
    أرجع (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
do_رڪارڊ(صحيح c)
{
    char_u	    *p;
    ساكن صحيح	    regname;
    yankreg_T	    *old_y_اڳيون, *old_y_current;
    صحيح		    retval;

    إذا (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	إذا (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
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
	نياپو("");
	p = get_recorded();
	إذا (p == عدم)
	    retval = FAIL;
	وإلا
	{
	    // Remove escaping for CSI and K_SPECIAL in multi-byte chars.
	    vim_unescape_csi(p);

	    // We don't want to change the default register here, so save and
	    // restore the current register name.
	    old_y_اڳيون = y_اڳيون;
	    old_y_current = y_current;

	    retval = stuff_yank(regname, p);

	    y_اڳيون = old_y_اڳيون;
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
    إذا (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	أرجع FAIL;
    }
    إذا (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	أرجع OK;
    }

    plen = STRLEN(p);
    get_yank_رجسٽر(regname, TRUE);
    إذا (y_append && y_current->y_array != عدم)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_سائيز - 1]);
	tmplen = pp->ڊيگهائي + plen;
	tmp = alloc(tmplen + 1);
	إذا (tmp == عدم)
	{
	    vim_free(p);
	    أرجع FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->ڊيگهائي, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->ڊيگهائي = tmplen;
    }
    وإلا
    {
	free_yank_all();
	إذا ((y_current->y_array = ALLOC_ONE(string_T)) == عدم)
	{
	    vim_free(p);
	    أرجع FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].ڊيگهائي = plen;
	y_current->y_سائيز = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
	y_current->y_وقت_سيٽ = vim_وقت();
#جيڪڏهن_پورو
    }
    أرجع OK;
}

/*
 * Last executed register (@ command)
 */
ساكن صحيح execreg_lastc = NUL;

#إذا defined(FEAT_VIMINFO)
    صحيح
get_execreg_lastc(فراغ)
{
    أرجع execreg_lastc;
}

    فراغ
سيٽ_execreg_lastc(صحيح lastc)
{
    execreg_lastc = lastc;
}
#جيڪڏهن_پورو

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
execreg_سٽ_continuation(string_T *سٽون, ڊگهو *idx)
{
    garray_T	ga;
    ڊگهو	cmd_شروعات = *idx;
    ڊگهو	cmd_پڇاڙي = *idx;
    string_T	*tmp;
    صحيح		j;
    char_u	*str;

    ga_init2(&ga, حجم(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    طالما (--cmd_شروعات > 0)
    {
	char_u	*p;

	p = skipwhite(سٽون[cmd_شروعات].string);
	إذا (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    اكسر;
    }

    // join all the lines
    tmp = &سٽون[cmd_شروعات];
    ga_concat_len(&ga, tmp->string, tmp->ڊيگهائي);
    لكل (j = cmd_شروعات + 1; j <= cmd_پڇاڙي; j++)
    {
	char_u	*p;

	tmp = &سٽون[j];
	p = skipwhite(tmp->string);
	إذا (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    إذا (ga.ga_len > 400)
	    {
		إذا (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		وإلا
		    ga.ga_growsize = ga.ga_len;
	    }
	    p++;
	    ga_concat_len(&ga, p, (tmp->string + tmp->ڊيگهائي) - p);
	}
    }
    ga_append(&ga, NUL);
    str = vim_strnsave(ga.ga_معلومات, ga.ga_len);
    ga_clear(&ga);

    *idx = cmd_شروعات;
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
    ڊگهو	i;
    char_u	*p;
    صحيح		retval = OK;
    صحيح		remap;

    // repeat previous one
    إذا (regname == '@')
    {
	إذا (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_استعمال_ٿيل_رجسٽر));
	    أرجع FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    إذا (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	أرجع FAIL;
    }
    execreg_lastc = regname;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#جيڪڏهن_پورو

    // black hole: don't stuff anything
    إذا (regname == '_')
	أرجع OK;

    // use last command line
    إذا (regname == ':')
    {
	إذا (آخري_cmdline == عدم)
	{
	    emsg(_(e_no_اڳيون_command_سٽ));
	    أرجع FAIL;
	}
	// don't keep the cmdline containing @:
	VIM_CLEAR(new_آخري_cmdline);
	// Escape all control characters with a CTRL-V
	p = vim_strsave_escaped_ext(آخري_cmdline,
		    (char_u *)"\001\002\003\004\005\006\007"
			  "\010\011\012\013\014\015\016\017"
			  "\020\021\022\023\024\025\026\027"
			  "\030\031\032\033\034\035\036\037",
		    Ctrl_V, FALSE);
	إذا (p != عدم)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    إذا (VIsual_سرگرم && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_اندريون_typebuf(p + 5, TRUE, TRUE, silent);
	    وإلا
		retval = put_اندريون_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    وإلا إذا (regname == '=')
    {
	p = get_اظهار_سٽ();
	إذا (p == عدم)
	    أرجع FAIL;
	retval = put_اندريون_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#جيڪڏهن_پورو
    وإلا إذا (regname == '.')		// use last inserted text
    {
	p = get_آخري_وجهو_save();
	إذا (p == عدم)
	{
	    emsg(_(e_no_inserted_متن_yet));
	    أرجع FAIL;
	}
	retval = put_اندريون_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    وإلا
    {
	get_yank_رجسٽر(regname, FALSE);
	إذا (y_current->y_array == عدم)
	    أرجع FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_اندريون_typebuf(silent);
	لكل (i = y_current->y_سائيز; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    صحيح	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    إذا (y_current->y_type == MLINE || i < y_current->y_سائيز - 1
								     || addcr)
	    {
		إذا (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    أرجع FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    إذا (colon && i > 0)
	    {
		p = skipwhite(str);
		إذا (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_سٽ_continuation(y_current->y_array, &i);
		    إذا (str == عدم)
			أرجع FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    إذا (free_str)
		vim_free(str);
	    إذا (escaped == عدم)
		أرجع FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    إذا (retval == FAIL)
		أرجع FAIL;
	    إذا (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		أرجع FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_پڇاڙي_reg_executing = FALSE;
    }
    أرجع retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ساكن فراغ
put_reedit_اندريون_typebuf(صحيح silent)
{
    إذا (restart_edit == NUL)
	أرجع;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    إذا (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    وإلا إذا (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    وإلا إذا (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    إذا (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    ساكن صحيح
put_اندريون_typebuf(
    char_u	*s,
    صحيح		esc,
    صحيح		colon,	    // add ':' before the line
    صحيح		silent)
{
    صحيح		retval = OK;

    put_reedit_اندريون_typebuf(silent);
    إذا (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    إذا (retval == OK)
    {
	char_u	*p;

	إذا (esc)
	    p = vim_strsave_escape_csi(s);
	وإلا
	    p = s;
	إذا (p == عدم)
	    retval = FAIL;
	وإلا
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	إذا (esc)
	    vim_free(p);
    }
    إذا (colon && retval == OK)
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
وجهو_reg(
    صحيح		regname,
    صحيح		literally_arg)	// insert literally, not as if typed
{
    ڊگهو	i;
    صحيح		retval = OK;
    char_u	*arg;
    صحيح		allocated;
    صحيح		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    إذا (got_int)
	أرجع FAIL;

    // check for valid regname
    إذا (regname != NUL && !valid_yank_reg(regname, FALSE))
	أرجع FAIL;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#جيڪڏهن_پورو

    إذا (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    وإلا إذا (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	إذا (arg == عدم)
	    أرجع FAIL;
	stuffescaped(arg, literally);
	إذا (allocated)
	    vim_free(arg);
    }
    وإلا				// name or number register
    {
	إذا (get_yank_رجسٽر(regname, FALSE))
	    literally = TRUE;
	إذا (y_current->y_array == عدم)
	    retval = FAIL;
	وإلا
	{
	    لكل (i = 0; i < y_current->y_سائيز; ++i)
	    {
		إذا (regname == '-' && y_current->y_type == MCHAR)
		{
		    صحيح dir = BACKWARD;
		    إذا ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			إذا (u_save_cursor() == FAIL)
			    أرجع FAIL;
			del_chars((ڊگهو)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			إذا (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    Append_Char_منزل_Redobuff(Ctrl_R);
		    Append_Char_منزل_Redobuff(regname);
		    do_put(regname, عدم, dir, 1L, PUT_CURSEND);
		}
		وإلا
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    إذا (y_current->y_type == MLINE || i < y_current->y_سائيز - 1)
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
	    إذا (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    أرجع TRUE;

	حالة '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    أرجع TRUE;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	حالة '=':		// result of expression
	    *argp = get_اظهار_سٽ();
	    *allocated = TRUE;
	    أرجع TRUE;
#جيڪڏهن_پورو

	حالة ':':		// last command line
	    إذا (آخري_cmdline == عدم && errmsg)
		emsg(_(e_no_اڳيون_command_سٽ));
	    *argp = آخري_cmdline;
	    أرجع TRUE;

	حالة '/':		// last search-pattern
	    إذا (آخري_ڳولا_pat() == عدم && errmsg)
		emsg(_(e_no_اڳيون_regular_اظهار));
	    *argp = آخري_ڳولا_pat();
	    أرجع TRUE;

	حالة '.':		// last inserted text
	    *argp = get_آخري_وجهو_save();
	    *allocated = TRUE;
	    إذا (*argp == عدم && errmsg)
		emsg(_(e_no_inserted_متن_yet));
	    أرجع TRUE;

	حالة Ctrl_F:		// Filename under cursor
	حالة Ctrl_P:		// Path under cursor, expand via "path"
	    إذا (!errmsg)
		أرجع FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, عدم);
	    *allocated = TRUE;
	    أرجع TRUE;

	حالة Ctrl_W:		// word under cursor
	حالة Ctrl_A:		// WORD (mnemonic All) under cursor
	    إذا (!errmsg)
		أرجع FALSE;
	    cnt = ڳوليو_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (ڳوليو_IDENT|ڳوليو_STRING) : ڳوليو_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : عدم;
	    *allocated = TRUE;
	    أرجع TRUE;

	حالة Ctrl_L:		// Line under cursor
	    إذا (!errmsg)
		أرجع FALSE;

	    *argp = ml_get_buf(curwin->w_بفر,
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
    ڊگهو	i;
    صحيح		literally = literally_arg;

    إذا (get_yank_رجسٽر(regname, FALSE))
	literally = TRUE;
    إذا (y_current->y_array == عدم)
	أرجع FAIL;

    لكل (i = 0; i < y_current->y_سائيز; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	إذا ((y_current->y_type == MLINE || i < y_current->y_سائيز - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	إذا (got_int)
	    أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    فراغ
سرڪايو_delete_registers(فراغ)
{
    صحيح		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    لكل (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    إذا (!y_append)
	y_اڳيون = y_current;
    y_regs[1].y_array = عدم;		// set register one to empty
}

#إذا defined(FEAT_EVAL)
    ساكن فراغ
جوڙيو_regtype_منزل_dict(صحيح regname, dict_T *dict, char_u *buf, صحيح bufsize)
{
    size_t  buflen;
    ڊگهو    reglen;
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
    (فراغ)dict_جوڙيو_string_len(dict, "regtype", buf, (صحيح)buflen);
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

    إذا (recursive)
	أرجع;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    إذا (list == عدم)
	أرجع;

    // yanked text contents
    لكل (n = 0; n < reg->y_سائيز; n++)
	list_append_string(list, reg->y_array[n].string, (صحيح)reg->y_array[n].ڊيگهائي);
    list->lv_تالو = VAR_FIXED;
    إذا (dict_جوڙيو_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (فراغ)dict_جوڙيو_string_len(v_event, "regname", buf, (صحيح)buflen);

    // motion type: inclusive or exclusive
    (فراغ)dict_جوڙيو_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (فراغ)dict_جوڙيو_string_len(v_event, "operator", buf, (صحيح)buflen);

    جوڙيو_regtype_منزل_dict(oap->regname, v_event, buf, حجم(buf));

    // selection type - visual or not
    (فراغ)dict_جوڙيو_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_سيٽ_شيون_ro(v_event);

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
	string_T    *وجهو,	// Not NULL if special register, except '.'
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

    إذا (recursive || regname == '_')
	أرجع;

    إذا (regname != '.' && وجهو == عدم
	    && reg == عدم)
	// Can happen when pasting text in normal mode in a terminal buffer
	أرجع;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    إذا (list == عدم)
	أرجع;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    إذا (regname == '.')
    {
	إذا (آخري_وجهو_ga.ga_معلومات != عدم)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, آخري_وجهو_ga.ga_معلومات,
		    (صحيح)آخري_وجهو_ga.ga_len);
    }
    وإلا إذا (وجهو != عدم)
    {
	list_append_string(list, وجهو->string, (صحيح)وجهو->ڊيگهائي);
    }
    وإلا
    {
	لكل (n = 0; n < reg->y_سائيز; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (صحيح)reg->y_array[n].ڊيگهائي);
    }

    list->lv_تالو = VAR_FIXED;
    إذا (dict_جوڙيو_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (فراغ)dict_جوڙيو_string_len(v_event, "regname", buf, (صحيح)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (فراغ)dict_جوڙيو_string_len(v_event, "operator", buf, (صحيح)buflen);

    جوڙيو_regtype_منزل_dict(regname, v_event, buf, حجم(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (فراغ)dict_جوڙيو_bool(v_event, "visual", VIsual_سرگرم);

    // Lock the dictionary and its keys
    dict_سيٽ_شيون_ro(v_event);

    recursive = true;
    textlock++;
    إذا (post)
	apply_autocmds(EVENT_TEXTPUTPOST, عدم, عدم, FALSE, curbuf);
    وإلا
	apply_autocmds(EVENT_TEXTPUTPRE, عدم, عدم, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#جيڪڏهن_پورو

/*
 * set all the yank registers to empty (called from main())
 */
    فراغ
init_yank(فراغ)
{
    صحيح		i;

    لكل (i = 0; i < انگ_REGISTERS; ++i)
	y_regs[i].y_array = عدم;
}

#إذا defined(EXITFREE)
    فراغ
clear_registers(فراغ)
{
    صحيح		i;

    لكل (i = 0; i < انگ_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	إذا (y_current->y_array != عدم)
	    free_yank_all();
    }
}
#جيڪڏهن_پورو

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    ساكن فراغ
free_yank(ڊگهو n)
{
    إذا (y_current->y_array == عدم)
	أرجع;

    ڊگهو	    i;

    لكل (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    فراغ
free_yank_all(فراغ)
{
    free_yank(y_current->y_سائيز);
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
    ڊگهو		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    صحيح			yanktype = oap->motion_type;
    ڊگهو		yanklines = oap->سٽ_ڳڻپ;
    linenr_T		yankendlnum = oap->پڇاڙي.lnum;
    char_u		*pnew;
    بنية بلاڪ_def	bd;

				    // check for read-only register
    إذا (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	أرجع FAIL;
    }
    إذا (oap->regname == '_')	    // black hole: nothing to do
	أرجع OK;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    إذا ((!clip_تارو.موجود && oap->regname == '*') ||
	(!clip_plus.موجود && oap->regname == '+'))
    {
	oap->regname = 0;
	نياپو_warn_missing_clipboard();
    }
#جيڪڏهن_پورو

    إذا (!deleting)		    // op_delete() already set y_current
	get_yank_رجسٽر(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    إذا (y_append && y_current->y_array != عدم)
	y_current = &newreg;
    وإلا
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    إذا (       oap->motion_type == MCHAR
	    && oap->شروعات.کانو == 0
	    && !oap->inclusive
	    && (!oap->is_VIsual || *p_sel == 'o')
	    && !oap->بلاڪ_گهڻائي
	    && oap->پڇاڙي.کانو == 0
	    && yanklines > 1)
    {
	yanktype = MLINE;
	--yankendlnum;
	--yanklines;
    }

    y_current->y_سائيز = yanklines;
    y_current->y_type = yanktype;   // set the yank register type
    y_current->y_ويڪر = 0;
    y_current->y_array = lalloc_clear(حجم(string_T) * yanklines, TRUE);
    إذا (y_current->y_array == عدم)
    {
	y_current = curr;
	أرجع FAIL;
    }
#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
    y_current->y_وقت_سيٽ = vim_وقت();
#جيڪڏهن_پورو

    y_idx = 0;
    lnum = oap->شروعات.lnum;

    إذا (oap->بلاڪ_گهڻائي)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_ويڪر = oap->پڇاڙي_vcol - oap->شروعات_vcol;

	إذا (curwin->w_curswant == MAXCOL && y_current->y_ويڪر > 0)
	    y_current->y_ويڪر--;
    }

    لكل ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	بدل (y_current->y_type)
	{
	    حالة MBLOCK:
		بلاڪ_prep(oap, &bd, lnum, FALSE);
		إذا (yank_copy_سٽ(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    اذهب fail;
		اكسر;

	    حالة MLINE:
		y_current->y_array[y_idx].ڊيگهائي = ml_get_len(lnum);
		إذا ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].ڊيگهائي)) == عدم)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    اذهب fail;
		}
		اكسر;

	    حالة MCHAR:
		{
		    صحيح tmp;

		    charwise_بلاڪ_prep(oap->شروعات, oap->پڇاڙي, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (صحيح)STRLEN(bd.textstart);
		    إذا (tmp < bd.textlen)
			bd.textlen = tmp;

		    إذا (yank_copy_سٽ(&bd, y_idx, FALSE) == FAIL)
			اذهب fail;
		    اكسر;
		}
		// NOTREACHED
	}
    }

    إذا (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	ڊگهو j;

	new_ptr = ALLOC_MULT(string_T, curr->y_سائيز + y_current->y_سائيز);
	إذا (new_ptr == عدم)
	    اذهب fail;
	لكل (j = 0; j < curr->y_سائيز; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
	curr->y_وقت_سيٽ = vim_وقت();
#جيڪڏهن_پورو

	إذا (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	إذا (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == عدم)
	{
	    pnew = alloc(curr->y_array[curr->y_سائيز - 1].ڊيگهائي + y_current->y_array[0].ڊيگهائي + 1);
	    إذا (pnew == عدم)
	    {
		y_idx = y_current->y_سائيز - 1;
		اذهب fail;
	    }

	    --j;
	    STRCPY(pnew, curr->y_array[j].string);
	    STRCPY(pnew + curr->y_array[j].ڊيگهائي, y_current->y_array[0].string);
	    vim_free(curr->y_array[j].string);
	    curr->y_array[j].string = pnew;
	    curr->y_array[j].ڊيگهائي = curr->y_array[j].ڊيگهائي + y_current->y_array[0].ڊيگهائي;
	    ++j;
	    VIM_CLEAR_STRING(y_current->y_array[0]);
	    y_idx = 1;
	}
	وإلا
	    y_idx = 0;
	طالما (y_idx < y_current->y_سائيز)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_سائيز = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    إذا (mess)			// Display message about yank?
    {
	إذا (yanktype == MCHAR
		&& !oap->بلاڪ_گهڻائي
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	إذا (yanklines > p_report)
	{
	    حرف namebuf[100];

	    إذا (oap->regname == NUL)
		*namebuf = NUL;
	    وإلا
		vim_snprintf(namebuf, حجم(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    إذا (oap->بلاڪ_گهڻائي)
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

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_شروعات = oap->شروعات;
	curbuf->b_op_پڇاڙي = oap->پڇاڙي;
	إذا (yanktype == MLINE && !oap->بلاڪ_گهڻائي)
	{
	    curbuf->b_op_شروعات.کانو = 0;
	    curbuf->b_op_پڇاڙي.کانو = MAXCOL;
	}
	إذا (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_پڇاڙي);
    }

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    إذا (curr == &y_regs[REAL_PLUS_رجسٽر] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	إذا (curr != &(y_regs[REAL_PLUS_رجسٽر]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_رجسٽر]));
	call_clip_provider_سيٽ('+');
    }
    إذا (curr == &y_regs[تارو_رجسٽر] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	إذا (curr != &(y_regs[تارو_رجسٽر]))
	    copy_yank_reg(&(y_regs[تارو_رجسٽر]));
	call_clip_provider_سيٽ('*');
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	إذا (clip_تارو.موجود
		&& (curr == &(y_regs[تارو_رجسٽر])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    إذا (curr != &(y_regs[تارو_رجسٽر]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[تارو_رجسٽر]));

	    clip_own_selection(&clip_تارو);
	    clip_gen_سيٽ_selection(&clip_تارو);
	}

# إذا defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	إذا (clip_plus.موجود
		&& (curr == &(y_regs[PLUS_رجسٽر])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    إذا (curr != &(y_regs[PLUS_رجسٽر]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_رجسٽر]));

	    clip_own_selection(&clip_plus);
	    clip_gen_سيٽ_selection(&clip_plus);
	}
# endif
    }
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
    إذا (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#جيڪڏهن_پورو
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
yank_copy_سٽ(بنية بلاڪ_def *bd, ڊگهو y_idx, صحيح exclude_trailing_space)
{
    char_u	*pnew;

    إذا (exclude_trailing_space)
	bd->endspaces = 0;
    إذا ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == عدم)
	أرجع FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    إذا (exclude_trailing_space)
    {
	صحيح s = bd->textlen + bd->endspaces;

	طالما (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_سر_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].ڊيگهائي = (size_t)(pnew - y_current->y_array[y_idx].string);

    أرجع OK;
}

#جيڪڏهن_وصف_ٿيل HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    ساكن فراغ
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    ڊگهو	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    حجم(string_T) * y_current->y_سائيز, TRUE);
    إذا (y_current->y_array == عدم)
	y_current->y_سائيز = 0;
    وإلا
	لكل (j = 0; j < y_current->y_سائيز; ++j)
	{
	    إذا ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].ڊيگهائي)) == عدم)
	    {
		free_yank(j);
		y_current->y_سائيز = 0;
		اكسر;
	    }
	    y_current->y_array[j].ڊيگهائي = curr->y_array[j].ڊيگهائي;
	}
    y_current = curr;
}
#جيڪڏهن_پورو

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
    char_u	*اظهار_نتيجو,	// result for regname "=" when compiled
    صحيح		dir,		// BACKWARD for 'P', FORWARD for 'p'
    ڊگهو	ڳڻپ,
    صحيح		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    صحيح		yanklen;
    صحيح		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	کانو;
    ڊگهو	i;			// index in y_array[]
    صحيح		y_type;
    ڊگهو	y_سائيز;
    صحيح		oldlen;
    ڊگهو	y_ويڪر = 0;
    colnr_T	vcol;
    string_T	*y_array = عدم;
    yankreg_T	*y_current_استعمال_ٿيل = عدم;
    ڊگهو	nr_سٽون = 0;
    string_T	وجهو_string;
    صحيح		allocated = FALSE;
    pos_T	orig_شروعات = curbuf->b_op_شروعات;
    pos_T	orig_پڇاڙي = curbuf->b_op_پڇاڙي;
    بنانشان صحيح cur_ve_flags = get_ve_flags();

#جيڪڏهن_وصف_ٿيل HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(فراغ)may_get_selection(regname);
#جيڪڏهن_پورو

    // Remove any preinserted text (issue #19329)
    إذا (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_شروعات = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_پڇاڙي = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    إذا (regname == '.')
    {
	إذا (VIsual_سرگرم)
	    stuffcharReadbuff(VIsual_گهڻائي);

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	إذا (has_textput_events)
	    جوڙيو_آخري_وجهو++;
#جيڪڏهن_پورو

	(فراغ)stuff_inserted((dir == FORWARD ? (ڳڻپ == -1 ? 'o' : 'a') :
				    (ڳڻپ == -1 ? 'O' : 'i')), ڳڻپ, FALSE);

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	إذا (has_textputpre())
	    put_do_autocmd('.', عدم, عدم, false, dir);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (has_textputpost())
	    put_do_autocmd('.', عدم, عدم, true, dir);

	إذا (has_textput_events && --جوڙيو_آخري_وجهو == 0)
	    ga_clear(&آخري_وجهو_ga);
#جيڪڏهن_پورو

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	إذا ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	أرجع;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    وجهو_string.string = عدم;
    إذا (regname == '=' && اظهار_نتيجو != عدم)
	وجهو_string.string = اظهار_نتيجو;
    وإلا إذا (get_spec_reg(regname, &وجهو_string.string, &allocated, TRUE)
		&& وجهو_string.string == عدم)
    {
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#جيڪڏهن_پورو
	أرجع;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    إذا (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	اذهب پڇاڙي;

    إذا (وجهو_string.string != عدم)
    {
	وجهو_string.ڊيگهائي = STRLEN(وجهو_string.string);

	y_type = MCHAR;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    لكل (;;)
	    {
		y_سائيز = 0;
		ptr = وجهو_string.string;
		ptrlen = وجهو_string.ڊيگهائي;
		طالما (ptr != عدم)
		{
		    إذا (y_array != عدم)
			y_array[y_سائيز].string = ptr;
		    ++y_سائيز;
		    tmp = vim_strchr(ptr, '\n');
		    إذا (tmp == عدم)
		    {
			إذا (y_array != عدم)
			    y_array[y_سائيز - 1].ڊيگهائي = ptrlen;
		    }
		    وإلا
		    {
			إذا (y_array != عدم)
			{
			    *tmp = NUL;
			    y_array[y_سائيز - 1].ڊيگهائي = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_سائيز - 1].ڊيگهائي + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			إذا (*tmp == NUL)
			{
			    y_type = MLINE;
			    اكسر;
			}
		    }
		    ptr = tmp;
		}
		إذا (y_array != عدم)
		    اكسر;
		y_array = ALLOC_MULT(string_T, y_سائيز);
		إذا (y_array == عدم)
		    اذهب پڇاڙي;
	    }
	}
	وإلا
#جيڪڏهن_پورو
	{
	    y_سائيز = 1;		// use fake one-line yank register
	    y_array = &وجهو_string;
	}
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (has_textputpre())
	    put_do_autocmd(regname, عدم, &وجهو_string, false, dir);
#جيڪڏهن_پورو
    }
    وإلا
    {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_رجسٽر(regname, FALSE);
	    put_do_autocmd(regname, y_current, عدم, false, dir);
	}
#جيڪڏهن_پورو
	get_yank_رجسٽر(regname, FALSE);

	y_type = y_current->y_type;
	y_ويڪر = y_current->y_ويڪر;
	y_سائيز = y_current->y_سائيز;
	y_array = y_current->y_array;
	y_current_استعمال_ٿيل = y_current;
    }

    إذا (y_type == MLINE)
    {
	إذا (flags & PUT_سٽ_ورهايو)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    إذا (u_save_cursor() == FAIL)
		اذهب پڇاڙي;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    إذا (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    إذا (ptr == عدم)
		اذهب پڇاڙي;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.کانو;
	    إذا (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    إذا (ptr == عدم)
		اذهب پڇاڙي;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_سٽون;
	    dir = FORWARD;
	}
	إذا (flags & PUT_سٽ_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_پڇاڙي;
	    dir = FORWARD;
	}
	curbuf->b_op_شروعات = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_پڇاڙي = curwin->w_cursor;	// default for '] mark
    }

    إذا (flags & PUT_سٽ)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    إذا (y_سائيز == 0 || y_array == عدم)
    {
	semsg(_(e_nothing_اندريون_رجسٽر_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	اذهب پڇاڙي;
    }

    إذا (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_سائيز + 1;
	إذا (lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
	    lnum = curbuf->b_ml.ml_سٽ_ڳڻپ + 1;
	إذا (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    اذهب پڇاڙي;
    }
    وإلا إذا (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	إذا (dir == BACKWARD)
	    (فراغ)hasFolding(lnum, &lnum, عدم);
	وإلا
	    (فراغ)hasFolding(lnum, عدم, &lnum);
#جيڪڏهن_پورو
	إذا (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	إذا ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    اذهب پڇاڙي;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	إذا (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	وإلا
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_شروعات = curwin->w_cursor;	// for mark_adjust()
#جيڪڏهن_پورو
    }
    وإلا إذا (u_save_cursor() == FAIL)
	اذهب پڇاڙي;

    إذا (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	إذا (gchar_cursor() == TAB)
	{
	    صحيح viscol = getviscol();
	    صحيح ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    إذا (dir == FORWARD ?
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#وإلا
		    ts - (viscol % ts) != 1
#جيڪڏهن_پورو
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    وإلا
		curwin->w_cursor.coladd = 0;
	}
	وإلا إذا (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    کانو = curwin->w_cursor.کانو;

    // Block mode
    إذا (y_type == MBLOCK)
    {
	صحيح	delcount;
	صحيح	incr = 0;
	بنية بلاڪ_def bd;
	ڊگهو	j;
	صحيح	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	إذا (dir == FORWARD && c != NUL)
	{
	    إذا (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &کانو, عدم, &endcol2, 0);
	    وإلا
		getvcol(curwin, &curwin->w_cursor, عدم, عدم, &کانو, 0);

	    إذا (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.کانو += (*mb_ptr2len)(ml_get_cursor());
	    وإلا
		إذا (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.کانو;
	    ++کانو;
	}
	وإلا
	    getvcol(curwin, &curwin->w_cursor, &کانو, عدم, &endcol2, 0);

	کانو += curwin->w_cursor.coladd;
	إذا (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.کانو))
	{
	    إذا (dir == FORWARD && c == NUL)
		++کانو;
	    إذا (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.کانو;
	    إذا (c == TAB)
	    {
		إذا (dir == BACKWARD && curwin->w_cursor.کانو)
		    curwin->w_cursor.کانو--;
		إذا (dir == FORWARD && کانو - 1 == endcol2)
		    curwin->w_cursor.کانو++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	لكل (i = 0; i < y_سائيز; ++i)
	{
	    صحيح		    spaces = 0;
	    حرف	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    إذا (curwin->w_cursor.lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
	    {
		إذا (ml_append(curbuf->b_ml.ml_سٽ_ڳڻپ, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    اكسر;
		++nr_سٽون;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    طالما (cts.cts_vcol < کانو && *cts.cts_ptr != NUL)
	    {
		// Count a tab for what it's worth (if list mode not on)
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    vcol = cts.cts_vcol;
	    ptr = cts.cts_ptr;
	    bd.textcol = (colnr_T)(ptr - oldp);
	    clear_chartabsize_arg(&cts);

	    shortline = (vcol < کانو) || (vcol == کانو && !*ptr) ;

	    إذا (vcol < کانو) // line too short, pad with spaces
		bd.startspaces = کانو - vcol;
	    وإلا إذا (vcol > کانو)
	    {
		bd.endspaces = vcol - کانو;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		إذا (has_mbyte)
		    bd.textcol -= (*mb_سر_off)(oldp, oldp + bd.textcol);
		إذا (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (صحيح)y_array[i].ڊيگهائي;

	    إذا ((flags & PUT_بلاڪ_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		//بلاڪ
		spaces = y_ويڪر + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		طالما (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		إذا (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    إذا (yanklen + spaces != 0
		     && ڳڻپ > ((وڌ_کان_وڌ_عدد - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_متن_too_long));
		اكسر;
	    }

	    totlen = ڳڻپ * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    إذا (newp == عدم)
		اكسر;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    لكل (j = 0; j < ڳڻپ; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		إذا ((j < ڳڻپ - 1 || !shortline) && spaces > 0)
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
	    إذا (i == 0)
		curwin->w_cursor.کانو += bd.startspaces;
	}

	بدليو_سٽون(lnum, 0, curwin->w_cursor.lnum - nr_سٽون, nr_سٽون);

	// Set '[ mark.
	curbuf->b_op_شروعات = curwin->w_cursor;
	curbuf->b_op_شروعات.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_پڇاڙي.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_پڇاڙي.کانو = bd.textcol + totlen - 1;
	إذا (curbuf->b_op_پڇاڙي.کانو < 0)
	    curbuf->b_op_پڇاڙي.کانو = 0;
	curbuf->b_op_پڇاڙي.coladd = 0;
	إذا (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_پڇاڙي;
	    curwin->w_cursor.کانو++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    إذا (curwin->w_cursor.کانو > len)
		curwin->w_cursor.کانو = len;
	}
	وإلا
	    curwin->w_cursor.lnum = lnum;
    }
    وإلا
    {
	pos_T	new_cursor;

	yanklen = (صحيح)y_array[0].ڊيگهائي;

	// Character or Line mode
	إذا (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    إذا (dir == FORWARD && gchar_cursor() != NUL)
	    {
		إذا (has_mbyte)
		{
		    صحيح bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    کانو += bytelen;
		    إذا (yanklen)
		    {
			curwin->w_cursor.کانو += bytelen;
			curbuf->b_op_پڇاڙي.کانو += bytelen;
		    }
		}
		وإلا
		{
		    ++کانو;
		    إذا (yanklen)
		    {
			++curwin->w_cursor.کانو;
			++curbuf->b_op_پڇاڙي.کانو;
		    }
		}
	    }
	    curbuf->b_op_شروعات = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	وإلا إذا (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	إذا (y_type == MCHAR && y_سائيز == 1)
	{
	    linenr_T	پڇاڙي_lnum = 0; // init for gcc
	    linenr_T	شروعات_lnum = lnum;
	    صحيح		پهريون_byte_off = 0;

	    إذا (VIsual_سرگرم)
	    {
		پڇاڙي_lnum = curbuf->b_visual.vi_پڇاڙي.lnum;
		إذا (پڇاڙي_lnum < curbuf->b_visual.vi_شروعات.lnum)
		    پڇاڙي_lnum = curbuf->b_visual.vi_شروعات.lnum;
		إذا (پڇاڙي_lnum > شروعات_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.کانو = کانو;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, عدم, &vcol, عدم, 0);
		}
	    }

	    إذا (ڳڻپ == 0 || yanklen == 0)
	    {
		إذا (VIsual_سرگرم)
		    lnum = پڇاڙي_lnum;
	    }
	    وإلا إذا (ڳڻپ > وڌ_کان_وڌ_عدد / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_متن_too_long));
	    وإلا
	    {
		totlen = ڳڻپ * yanklen;
		افعل
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    إذا (lnum > شروعات_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			إذا (getvpos(&pos, vcol) == OK)
			    کانو = pos.کانو;
			وإلا
			    کانو = MAXCOL;
		    }
		    إذا (VIsual_سرگرم && کانو > oldlen)
		    {
			lnum++;
			استمر;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    إذا (newp == عدم)
			اذهب پڇاڙي;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)کانو);
		    ptr = newp + کانو;
		    لكل (i = 0; i < ڳڻپ; ++i)
		    {
			mch_memmove(ptr, y_array[0].string, (size_t)yanklen);
			ptr += yanklen;
		    }
		    mch_memmove(ptr, oldp + کانو, (size_t)(oldlen - کانو) + 1);	    // +1 for NUL

		    // compute the byte offset for the last character
		    پهريون_byte_off = mb_سر_off(newp, ptr - 1);

		    // Note: this may free "newp"
		    ml_replace(lnum, newp, FALSE);

		    inserted_بائيٽون(lnum, کانو, totlen);

		    // Place cursor on last putted char.
		    إذا (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			بدليو_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.کانو += (colnr_T)(totlen - 1);
		    }
		    إذا (VIsual_سرگرم)
			lnum++;
		} طالما (VIsual_سرگرم && lnum <= پڇاڙي_lnum);

		إذا (VIsual_سرگرم) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_پڇاڙي = curwin->w_cursor;
	    curbuf->b_op_پڇاڙي.کانو -= پهريون_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    إذا (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.کانو;
	    وإلا
		curwin->w_cursor.کانو -= پهريون_byte_off;
	}
	وإلا
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    صحيح		indent;
	    صحيح		orig_indent = 0;
	    صحيح		indent_diff = 0;	// init for gcc
	    صحيح		پهريون_indent = TRUE;
	    صحيح		lendiff = 0;
	    ڊگهو	cnt;

	    إذا (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    لكل (cnt = 1; cnt <= ڳڻپ; ++cnt)
	    {
		i = 0;
		إذا (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + کانو;
		    totlen = (صحيح)y_array[y_سائيز - 1].ڊيگهائي;
		    newp = alloc(ml_get_len(lnum) - کانو + totlen + 1);
		    إذا (newp == عدم)
			اذهب error;
		    STRCPY(newp, y_array[y_سائيز - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(کانو + yanklen + 1);
		    إذا (newp == عدم)
			اذهب error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)کانو);
					    // append to first line
		    mch_memmove(newp + کانو, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		لكل (; i < y_سائيز; ++i)
		{
		    إذا (y_type != MCHAR || i < y_سائيز - 1)
		    {
			إذا (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    اذهب error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_سٽون;
		    إذا (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			إذا (cnt == ڳڻپ && i == y_سائيز - 1)
			    lendiff = ml_get_len(lnum);
			إذا (*ptr == '#' && preprocs_کاٻو())
			    indent = 0;     // Leave # lines at start
			وإلا إذا (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			وإلا إذا (پهريون_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    پهريون_indent = FALSE;
			}
			وإلا إذا ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(فراغ)سيٽ_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			إذا (cnt == ڳڻپ && i == y_سائيز - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		إذا (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    إذا (y_type == MLINE)
	    {
		curbuf->b_op_شروعات.کانو = 0;
		إذا (dir == FORWARD)
		    curbuf->b_op_شروعات.lnum++;
	    }
	    mark_adjust(curbuf->b_op_شروعات.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_سٽون, 0L);

	    // note changed text for displaying and folding
	    إذا (y_type == MCHAR)
		بدليو_سٽون(curwin->w_cursor.lnum, کانو,
					 curwin->w_cursor.lnum + 1, nr_سٽون);
	    وإلا
		بدليو_سٽون(curbuf->b_op_شروعات.lnum, 0,
					   curbuf->b_op_شروعات.lnum, nr_سٽون);
	    إذا (y_current_استعمال_ٿيل != عدم && (y_current_استعمال_ٿيل != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_رجسٽر_بدليو_while_using_it));
		اذهب پڇاڙي;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_پڇاڙي.lnum = new_lnum;
	    کانو = MAX(0, (colnr_T)y_array[y_سائيز - 1].ڊيگهائي - lendiff);
	    إذا (کانو > 1)
	    {
		curbuf->b_op_پڇاڙي.کانو = کانو - 1;
		إذا (y_array[y_سائيز - 1].ڊيگهائي > 0)
		    curbuf->b_op_پڇاڙي.کانو -= mb_سر_off(y_array[y_سائيز - 1].string,
						y_array[y_سائيز - 1].string + y_array[y_سائيز - 1].ڊيگهائي - 1);
	    }
	    وإلا
		curbuf->b_op_پڇاڙي.کانو = 0;

	    إذا (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    وإلا إذا (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		إذا (y_type == MLINE)
		{
		    إذا (lnum >= curbuf->b_ml.ml_سٽ_ڳڻپ)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
		    وإلا
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.کانو = 0;
		}
		وإلا
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.کانو = کانو;
		    curbuf->b_op_پڇاڙي = curwin->w_cursor;
		    إذا (کانو > 1)
			curbuf->b_op_پڇاڙي.کانو = کانو - 1;
		}
	    }
	    وإلا إذا (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.کانو = 0;
		إذا (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    وإلا	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_سٽون);
    curwin->w_سيٽ_curswant = true;

    // Make sure the cursor is not after the NUL.
    صحيح len = ml_get_curline_len();
    إذا (curwin->w_cursor.کانو > len)
    {
	إذا (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.کانو - len;
	curwin->w_cursor.کانو = len;
    }

پڇاڙي:
    إذا (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_شروعات = orig_شروعات;
	curbuf->b_op_پڇاڙي = orig_پڇاڙي;
    }

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (has_textputpost())
    {
	إذا (وجهو_string.string == عدم)
	    put_do_autocmd(regname, y_current, عدم, true, dir);
	وإلا
	    put_do_autocmd(regname, عدم, &وجهو_string, true, dir);
    }
#جيڪڏهن_پورو

    إذا (allocated)
	vim_free(وجهو_string.string);
    إذا (regname == '=')
	vim_free(y_array);

    VIsual_سرگرم = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    صحيح
get_رجسٽر_name(صحيح انگ)
{
    إذا (انگ == -1)
	أرجع '"';
    وإلا إذا (انگ < 10)
	أرجع انگ + '0';
    وإلا إذا (انگ == DELETION_رجسٽر)
	أرجع '-';
#إذا defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    وإلا إذا (انگ == تارو_رجسٽر)
	أرجع '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    وإلا إذا (clipmethod == CLIPMETHOD_PROVIDER && انگ == REAL_PLUS_رجسٽر)
	أرجع '+';
    وإلا إذا (clipmethod != CLIPMETHOD_PROVIDER && انگ == PLUS_رجسٽر)
	أرجع '+';
#جيڪڏهن_پورو
    وإلا
	أرجع انگ + 'a' - 10;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    صحيح
get_unname_رجسٽر(فراغ)
{
    أرجع y_اڳيون == عدم ? -1 : y_اڳيون - &y_regs[0];
}
#جيڪڏهن_پورو

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    فراغ
ex_display(exarg_T *eap)
{
    صحيح		i, n;
    ڊگهو	j;
    char_u	*p;
    yankreg_T	*yb;
    صحيح		name;
    صحيح		attr;
    char_u	*arg = eap->arg;
    صحيح		clen;
    صحيح		type;
    string_T	وجهو;

    silence_w23_w24_نياپو++;
    إذا (arg != عدم && *arg == NUL)
	arg = عدم;
    attr = HL_ATTR(HLF_8);

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#جيڪڏهن_پورو

    // Highlight title
    نياپو_puts_title(_("\nType Name Content"));
    لكل (i = -1; i < انگ_REGISTERS && !got_int; ++i)
    {
	name = get_رجسٽر_name(i);
	بدل (get_reg_type(name, عدم))
	{
	    حالة MLINE: type = 'l'; اكسر;
	    حالة MCHAR: type = 'c'; اكسر;
	    افتراضي:	type = 'b'; اكسر;
	}
	إذا (arg != عدم && vim_strchr(arg, name) == عدم
#جيڪڏهن_وصف_ٿيل ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == عدم)
#جيڪڏهن_پورو
		)
	    استمر;	    // did not ask for this register

#جيڪڏهن_وصف_ٿيل HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
	إذا (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (فراغ)may_get_selection(name);
#جيڪڏهن_پورو

	إذا (i == -1)
	{
	    إذا (y_اڳيون != عدم)
		yb = y_اڳيون;
	    وإلا
		yb = &(y_regs[0]);
	}
	وإلا
	    yb = &(y_regs[i]);

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != عدم &&
		    (yb == y_اڳيون || yb == &y_regs[0])))
	    استمر;	    // do not list register being written to, the
			    // pointer can be freed
#جيڪڏهن_پورو

	إذا (yb->y_array != عدم)
	{
	    صحيح do_show = FALSE;

	    لكل (j = 0; !do_show && j < yb->y_سائيز; ++j)
		do_show = !پيغام_filtered(yb->y_array[j].string);

	    إذا (do_show || yb->y_سائيز == 0)
	    {
		نياپو_putchar('\n');
		نياپو_puts("  ");
		نياپو_putchar(type);
		نياپو_puts("  ");
		نياپو_putchar('"');
		نياپو_putchar(name);
		نياپو_puts("   ");

		n = (صحيح)Columns - 11;
		لكل (j = 0; j < yb->y_سائيز && n > 1; ++j)
		{
		    إذا (j)
		    {
			نياپو_puts_attr("^J", attr);
			n -= 2;
		    }
		    لكل (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			نياپو_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		إذا (n > 1 && yb->y_type == MLINE)
		    نياپو_puts_attr("^J", attr);
		ٻاهريون_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    وجهو = get_آخري_وجهو();
    إذا ((p = وجهو.string) != عدم
		  && (arg == عدم || vim_strchr(arg, '.') != عدم) && !got_int
						      && !پيغام_filtered(p))
    {
	نياپو_puts("\n  c  \".   ");
	dis_نياپو(p, TRUE);
    }

    // display last command line
    إذا (آخري_cmdline != عدم && (arg == عدم || vim_strchr(arg, ':') != عدم)
			       && !got_int && !پيغام_filtered(آخري_cmdline))
    {
	نياپو_puts("\n  c  \":   ");
	dis_نياپو(آخري_cmdline, FALSE);
    }

    // display current file name
    إذا (curbuf->b_fname != عدم
	    && (arg == عدم || vim_strchr(arg, '%') != عدم) && !got_int
					&& !پيغام_filtered(curbuf->b_fname))
    {
	نياپو_puts("\n  c  \"%   ");
	dis_نياپو(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    إذا ((arg == عدم || vim_strchr(arg, '#') != عدم) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	إذا (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !پيغام_filtered(fname))
	{
	    نياپو_puts("\n  c  \"#   ");
	    dis_نياپو(fname, FALSE);
	}
    }

    // display last search pattern
    إذا (آخري_ڳولا_pat() != عدم
		 && (arg == عدم || vim_strchr(arg, '/') != عدم) && !got_int
				      && !پيغام_filtered(آخري_ڳولا_pat()))
    {
	نياپو_puts("\n  c  \"/   ");
	dis_نياپو(آخري_ڳولا_pat(), FALSE);
    }

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    // display last used expression
    إذا (اظهار_سٽ != عدم && (arg == عدم || vim_strchr(arg, '=') != عدم)
				  && !got_int && !پيغام_filtered(اظهار_سٽ))
    {
	نياپو_puts("\n  c  \"=   ");
	dis_نياپو(اظهار_سٽ, FALSE);
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#جيڪڏهن_پورو
    silence_w23_w24_نياپو--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    ساكن فراغ
dis_نياپو(
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
	إذا (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    نياپو_outtrans_len(p, l);
	    p += l;
	}
	وإلا
	    نياپو_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#إذا defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    فراغ
dnd_yank_drag_معلومات(char_u *str, ڊگهو len)
{
    yankreg_T *curr;

    curr = y_current;
    y_current = &y_regs[TILDE_رجسٽر];
    free_yank_all();
    str_منزل_reg(y_current, MCHAR, str, len, 0L, FALSE);
    y_current = curr;
}
#جيڪڏهن_پورو


/*
 * Return the type of a register.
 * Used for getregtype()
 * Returns MAUTO for error.
 */
    char_u
get_reg_type(صحيح regname, ڊگهو *reglen)
{
    بدل (regname)
    {
	حالة '%':		// file name
	حالة '#':		// alternate file name
	حالة '=':		//اظهار
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

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#جيڪڏهن_پورو

    إذا (regname != NUL && !valid_yank_reg(regname, FALSE))
	أرجع MAUTO;

    get_yank_رجسٽر(regname, FALSE);

    إذا (y_current->y_array != عدم)
    {
	إذا (reglen != عدم && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_ويڪر;
	أرجع y_current->y_type;
    }
    أرجع MAUTO;
}

#إذا defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    ساكن char_u *
getreg_wrap_one_سٽ(char_u *s, صحيح flags)
{
    إذا (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	إذا (list != عدم)
	{
	    إذا (list_append_string(list, عدم, -1) == FAIL)
	    {
		list_free(list);
		أرجع عدم;
	    }
	    list->lv_پهريون->li_tv.vval.v_string = s;
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
    ڊگهو	len;

    // Don't allow using an expression register inside an expression
    إذا (regname == '=')
    {
	إذا (flags & GREG_NO_اظهار)
	    أرجع عدم;
	إذا (flags & GREG_اظهار_SRC)
	    أرجع getreg_wrap_one_سٽ(get_اظهار_سٽ_src(), flags);
	أرجع getreg_wrap_one_سٽ(get_اظهار_سٽ(), flags);
    }

    إذا (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    إذا (regname != NUL && !valid_yank_reg(regname, FALSE))
	أرجع عدم;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    إذا (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	إذا (retval == عدم)
	    أرجع عدم;
	إذا (allocated)
	    أرجع getreg_wrap_one_سٽ(retval, flags);
	أرجع getreg_wrap_one_سٽ(vim_strsave(retval), flags);
    }

    get_yank_رجسٽر(regname, FALSE);
    إذا (y_current->y_array == عدم)
	أرجع عدم;

    إذا (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	إذا (list == عدم)
	    أرجع عدم;
	لكل (i = 0; i < y_current->y_سائيز; ++i)
	{
	    إذا (list_append_string(list, y_current->y_array[i].string,
		(صحيح)y_current->y_array[i].ڊيگهائي) == FAIL)
	    {
		list_free(list);
		أرجع عدم;
	    }
	}
	أرجع (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    لكل (i = 0; i < y_current->y_سائيز; ++i)
    {
	len += (ڊگهو)y_current->y_array[i].ڊيگهائي;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	إذا (y_current->y_type == MLINE || i < y_current->y_سائيز - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    إذا (retval == عدم)
	أرجع عدم;

    // Copy the lines of the yank register into the string.
    len = 0;
    لكل (i = 0; i < y_current->y_سائيز; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (ڊگهو)y_current->y_array[i].ڊيگهائي;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	إذا (y_current->y_type == MLINE || i < y_current->y_سائيز - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    أرجع retval;
}

    ساكن صحيح
init_write_reg(
    صحيح		name,
    yankreg_T	**old_y_اڳيون,
    yankreg_T	**old_y_current,
    صحيح		must_append,
    صحيح		*yank_type اڻ_استعمال_ٿيل)
{
    إذا (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	أرجع FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_اڳيون = y_اڳيون;
    *old_y_current = y_current;

    get_yank_رجسٽر(name, TRUE);
    إذا (!y_append && !must_append)
	free_yank_all();
    أرجع OK;
}

    ساكن فراغ
پڄاڻي_write_reg(
    صحيح		name,
    yankreg_T	*old_y_اڳيون,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_سيٽ_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    إذا (name != '"')
	y_اڳيون = old_y_اڳيون;
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
    char_u	**لڙيون,
    صحيح		maxlen اڻ_استعمال_ٿيل,
    صحيح		must_append,
    صحيح		yank_type,
    ڊگهو	بلاڪ_len)
{
    yankreg_T  *old_y_اڳيون, *old_y_current;

    إذا (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	إذا (لڙيون[0] == عدم)
	    s = (char_u *)"";
	وإلا إذا (لڙيون[1] != عدم)
	{
	    semsg(_(e_رجسٽر_char_cannot_contain_گڻو_سٽون), name);
	    أرجع;
	}
	وإلا
	    s = لڙيون[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, بلاڪ_len);
	أرجع;
    }

    إذا (name == '_')	    // black hole: nothing to do
	أرجع;

    إذا (init_write_reg(name, &old_y_اڳيون, &old_y_current, must_append,
		&yank_type) == FAIL)
	أرجع;

    str_منزل_reg(y_current, yank_type, (char_u *)لڙيون, -1, بلاڪ_len, TRUE);

    پڄاڻي_write_reg(name, old_y_اڳيون, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_سيٽ(name);
# endif
}

    فراغ
write_reg_contents_ex(
    صحيح		name,
    char_u	*str,
    صحيح		maxlen,
    صحيح		must_append,
    صحيح		yank_type,
    ڊگهو	بلاڪ_len)
{
    yankreg_T	*old_y_اڳيون, *old_y_current;
    ڊگهو	len;

    إذا (maxlen >= 0)
	len = maxlen;
    وإلا
	len = (ڊگهو)STRLEN(str);

    // Special case: '/' search pattern
    إذا (name == '/')
    {
	سيٽ_آخري_ڳولا_pat(str, RE_ڳولا, TRUE, TRUE);
	أرجع;
    }

    إذا (name == '#')
    {
	إذا (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  أرجع;
	}

	buf_T	*buf;

	إذا (VIM_ISDIGIT(*str))
	{
	    صحيح	انگ = atoi((حرف *)str);

	    buf = buflist_findnr(انگ);
	    إذا (buf == عدم)
		semsg(_(e_بفر_nr_does_not_exist), (ڊگهو)انگ);
	}
	وإلا
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	إذا (buf == عدم)
	    أرجع;
	curwin->w_alt_fnum = buf->b_fnum;
	أرجع;
    }

    إذا (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	إذا (p == عدم)
	    أرجع;
	إذا (must_append && اظهار_سٽ != عدم)
	{
	    s = concat_str(اظهار_سٽ, p);
	    vim_free(p);
	    p = s;
	}
	سيٽ_اظهار_سٽ(p, عدم);
	أرجع;
    }

    إذا (name == '_')	    // black hole: nothing to do
	أرجع;

    إذا (init_write_reg(name, &old_y_اڳيون, &old_y_current, must_append,
		&yank_type) == FAIL)
	أرجع;

    str_منزل_reg(y_current, yank_type, str, len, بلاڪ_len, FALSE);

    پڄاڻي_write_reg(name, old_y_اڳيون, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_سيٽ(name);
# endif
}
#جيڪڏهن_پورو	// FEAT_EVAL

#إذا defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    فراغ
str_منزل_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    صحيح		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    ڊگهو	len,		// length of string
    ڊگهو	blocklen,	// width of Visual block
    صحيح		str_list)	// TRUE if str is char_u **
{
    صحيح		type;			// MCHAR, MLINE or MBLOCK
    صحيح		lnum;
    ڊگهو	شروعات;
    ڊگهو	i;
    صحيح		extra;
    صحيح		newlines;		// number of lines added
    صحيح		extraline = 0;		// extra line at the end
    صحيح		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    ڊگهو	maxlen;

    إذا (y_ptr->y_array == عدم)		// NULL means empty register
	y_ptr->y_سائيز = 0;

    إذا (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    وإلا
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    إذا (str_list)
    {
	لكل (ss = (char_u **) str; *ss != عدم; ++ss)
	    ++newlines;
    }
    وإلا
    {
	لكل (i = 0; i < len; i++)
	    إذا (str[i] == '\n')
		++newlines;
	إذا (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	إذا (y_ptr->y_سائيز > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    إذا (y_ptr->y_سائيز + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	أرجع;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_سائيز + newlines) * حجم(string_T), TRUE);
    إذا (pp == عدم)	// out of memory
	أرجع;
    لكل (lnum = 0; lnum < y_ptr->y_سائيز; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    إذا (str_list)
    {
	لكل (ss = (char_u **) str; *ss != عدم; ++ss, ++lnum)
	{
	    pp[lnum].ڊيگهائي = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].ڊيگهائي);
	    إذا (type == MBLOCK)
	    {
		صحيح charlen = vim_strsize(*ss);

		إذا (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    وإلا
    {
	لكل (شروعات = 0; شروعات < len + extraline; شروعات += i + 1)
	{
	    صحيح charlen = 0;

	    لكل (i = شروعات; i < len;)	// find the end of the line
	    {
		إذا (str[i] == '\n')
		    اكسر;
		إذا (type == MBLOCK)
		{
		    إذا (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    وإلا
			charlen += byte2cells(str[i]);
		}

		إذا (str[i] == NUL)
		    i++; // registers can have NUL chars
		وإلا
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= شروعات;			// i is now length of line
	    إذا (charlen > maxlen)
		maxlen = charlen;
	    إذا (append)
	    {
		--lnum;
		extra = (صحيح)y_ptr->y_array[lnum].ڊيگهائي;
	    }
	    وإلا
		extra = 0;
	    s = alloc(i + extra + 1);
	    إذا (s == عدم)
		اكسر;
	    إذا (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    إذا (append)
		vim_free(y_ptr->y_array[lnum].string);
	    إذا (i > 0)
		mch_memmove(s + extra, str + شروعات, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].ڊيگهائي = extra;
	    ++lnum;
	    طالما (--extra >= 0)
	    {
		إذا (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_سائيز = lnum;
    إذا (type == MBLOCK)
	y_ptr->y_ويڪر = (blocklen < 0 ? maxlen - 1 : blocklen);
    وإلا
	y_ptr->y_ويڪر = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_وقت_سيٽ = vim_وقت();
# endif
}
#جيڪڏهن_پورو // FEAT_CLIPBOARD || FEAT_EVAL
