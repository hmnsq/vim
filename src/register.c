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
ساكن yankreg_T	y_regs[عدد_REGISTERS];

ساكن yankreg_T	*y_current;	    // ptr to current yankreg
ساكن صحيح		y_append;	    // TRUE when appending
ساكن yankreg_T	*y_پچھلا = عدم; // ptr to last written yankreg

ساكن صحيح	stuff_yank(صحيح, char_u *);
ساكن فراغ	put_reedit_وچ_typebuf(صحيح silent);
ساكن صحيح	put_وچ_typebuf(char_u *s, صحيح esc, صحيح colon, صحيح silent);
ساكن صحيح	yank_copy_سطر(بنية بلاک_def *bd, لما y_idx, صحيح exclude_trailing_space);
#جے_تعریف_شدہ HAVE_CLIPMETHOD
ساكن فراغ	copy_yank_reg(yankreg_T *reg);
#جے_انت
ساكن فراغ	dis_پیغام(char_u *p, صحيح skip_esc);

#إذا defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(فراغ)
{
    أرجع y_regs;
}
#جے_انت

#إذا defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_رجسٹر(صحيح reg)
{
    أرجع &y_regs[reg];
}
#جے_انت

    yankreg_T *
get_y_current(فراغ)
{
    أرجع y_current;
}

    yankreg_T *
get_y_پچھلا(فراغ)
{
    أرجع y_پچھلا;
}

    فراغ
سیٹ_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    فراغ
سیٹ_y_پچھلا(yankreg_T *yreg)
{
    y_پچھلا = yreg;
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
ساكن char_u	*اظہاریہ_سطر = عدم;
ساكن exarg_T	*اظہاریہ_eap = عدم;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    صحيح
get_اظہاریہ_رجسٹر(فراغ)
{
    char_u	*new_سطر;

    new_سطر = getcmdline('=', 0L, 0, 0);
    إذا (new_سطر == عدم)
	أرجع NUL;
    إذا (*new_سطر == NUL)	// use previous line
	vim_free(new_سطر);
    وإلا
	سیٹ_اظہاریہ_سطر(new_سطر, عدم);
    أرجع '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    فراغ
سیٹ_اظہاریہ_سطر(char_u *new_سطر, exarg_T *eap)
{
    vim_free(اظہاریہ_سطر);
    اظہاریہ_سطر = new_سطر;
    اظہاریہ_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_اظہاریہ_سطر(فراغ)
{
    char_u	*اظہاریہ_copy;
    char_u	*rv;
    ساكن صحيح	nested = 0;

    إذا (اظہاریہ_سطر == عدم)
	أرجع عدم;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    اظہاریہ_copy = vim_strsave(اظہاریہ_سطر);
    إذا (اظہاریہ_copy == عدم)
	أرجع عدم;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    إذا (nested >= 10)
	أرجع اظہاریہ_copy;

    ++nested;
    rv = eval_منزل_string_eap(اظہاریہ_copy, TRUE, اظہاریہ_eap, FALSE);
    --nested;
    vim_free(اظہاریہ_copy);
    أرجع rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ساكن char_u *
get_اظہاریہ_سطر_src(فراغ)
{
    إذا (اظہاریہ_سطر == عدم)
	أرجع عدم;
    أرجع vim_strsave(اظہاریہ_سطر);
}
#جے_انت // FEAT_EVAL

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
#جے_تعریف_شدہ FEAT_EVAL
				    "/#.%:="
#وإلا
				    "/#.%:"
#جے_انت
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
#جے_انت
#جے_تعریف_شدہ FEAT_DND
	    || (!writing && regname == '~')
#جے_انت
							)
	أرجع TRUE;
#جے_بنا_تعریف FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    وإلا إذا (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	پیغام_warn_missing_clipboard();
	أرجع FALSE;
    }
#جے_انت
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
get_yank_رجسٹر(صحيح regname, صحيح writing)
{
    صحيح	    i;
    صحيح	    ret = FALSE;

    y_append = FALSE;
    إذا ((regname == 0 || regname == '"') && !writing && y_پچھلا != عدم)
    {
	y_current = y_پچھلا;
	أرجع ret;
    }
    i = regname;
    إذا (VIM_ISDIGIT(i))
	i -= '0';
    وإلا إذا (ASCII_ISLOWER(i))
	i = Char_Ord_کم(i) + 10;
    وإلا إذا (ASCII_ISUPPER(i))
    {
	i = Char_Ord_اتے(i) + 10;
	y_append = TRUE;
    }
    وإلا إذا (regname == '-')
	i = DELETION_رجسٹر;
#جے_تعریف_شدہ FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    وإلا إذا (clip_تارا.دستیاب && regname == '*')
    {
	i = تارا_رجسٹر;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    وإلا إذا (clip_plus.دستیاب && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	إذا (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_رجسٹر;
	وإلا
# endif
	    i = PLUS_رجسٹر;
	ret = TRUE;
    }
#وإلاإذا defined(FEAT_CLIPBOARD_PROVIDER)
    وإلا إذا (regname == '*')
    {
	i = تارا_رجسٹر;
	ret = TRUE;
    }
    وإلا إذا (regname == '+')
    {
	i = REAL_PLUS_رجسٹر;
	ret = TRUE;
    }
#جے_انت
#جے_تعریف_شدہ FEAT_DND
    وإلا إذا (!writing && regname == '~')
	i = TILDE_رجسٹر;
#جے_انت
    وإلا		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    إذا (writing)	// remember the register we write into for do_put()
	y_پچھلا = y_current;
    أرجع ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    فراغ *
get_رجسٹر(
    صحيح		name,
    صحيح		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    صحيح		i;

#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	إذا (name == '*' && clip_تارا.دستیاب)
	{
	    إذا (clip_isautosel_تارا())
		clip_update_selection(&clip_تارا);
	    may_get_selection(name);
	}
	إذا (name == '+' && clip_plus.دستیاب)
	{
	    إذا (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#جے_انت

    get_yank_رجسٹر(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    إذا (reg == عدم)
	أرجع (فراغ *)عدم;

    *reg = *y_current;
    إذا (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	إذا (reg->y_ناپ == 0 || y_current->y_array == عدم)
	    reg->y_array = عدم;
	وإلا
	    reg->y_array = ALLOC_MULT(string_T, reg->y_ناپ);
	إذا (reg->y_array != عدم)
	{
	    لكل (i = 0; i < reg->y_ناپ; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].لمائی);
		إذا (reg->y_array[i].string == عدم)
		{
		    // The allocation failed so clean up and exit
		    طالما (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    أرجع (فراغ *)عدم;
		}

		reg->y_array[i].لمائی = y_current->y_array[i].لمائی;
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
put_رجسٹر(صحيح name, فراغ *reg)
{
    get_yank_رجسٹر(name, 0);
    free_yank_all();
    *y_current = *(yankreg_T *)reg;
    vim_free(reg);

#جے_تعریف_شدہ FEAT_CLIPBOARD
    // Send text written to clipboard register to the clipboard.
    may_سیٹ_selection();
#جے_انت
}

#إذا defined(FEAT_CLIPBOARD)
    فراغ
free_رجسٹر(فراغ *reg)
{
    yankreg_T tmp;

    tmp = *y_current;
    *y_current = *(yankreg_T *)reg;
    free_yank_all();
    vim_free(reg);
    *y_current = tmp;
}
#جے_انت

/*
 * return TRUE if the current yank register has type MLINE
 */
    صحيح
yank_رجسٹر_mline(صحيح regname)
{
    إذا (regname != 0 && !valid_yank_reg(regname, FALSE))
	أرجع FALSE;
    إذا (regname == '_')		// black hole is always empty
	أرجع FALSE;
    get_yank_رجسٹر(regname, FALSE);
    أرجع (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
do_ریکارڈ(صحيح c)
{
    char_u	    *p;
    ساكن صحيح	    regname;
    yankreg_T	    *old_y_پچھلا, *old_y_current;
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
	پیغام("");
	p = get_recorded();
	إذا (p == عدم)
	    retval = FAIL;
	وإلا
	{
	    // Remove escaping for CSI and K_SPECIAL in multi-byte chars.
	    vim_unescape_csi(p);

	    // We don't want to change the default register here, so save and
	    // restore the current register name.
	    old_y_پچھلا = y_پچھلا;
	    old_y_current = y_current;

	    retval = stuff_yank(regname, p);

	    y_پچھلا = old_y_پچھلا;
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
    get_yank_رجسٹر(regname, TRUE);
    إذا (y_append && y_current->y_array != عدم)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_ناپ - 1]);
	tmplen = pp->لمائی + plen;
	tmp = alloc(tmplen + 1);
	إذا (tmp == عدم)
	{
	    vim_free(p);
	    أرجع FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->لمائی, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->لمائی = tmplen;
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
	y_current->y_array[0].لمائی = plen;
	y_current->y_ناپ = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#جے_تعریف_شدہ FEAT_VIMINFO
	y_current->y_ویلا_سیٹ = vim_ویلا();
#جے_انت
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
سیٹ_execreg_lastc(صحيح lastc)
{
    execreg_lastc = lastc;
}
#جے_انت

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
execreg_سطر_continuation(string_T *سطریں, لما *idx)
{
    garray_T	ga;
    لما	cmd_شروع = *idx;
    لما	cmd_سرا = *idx;
    string_T	*tmp;
    صحيح		j;
    char_u	*str;

    ga_init2(&ga, حجم(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    طالما (--cmd_شروع > 0)
    {
	char_u	*p;

	p = skipwhite(سطریں[cmd_شروع].string);
	إذا (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    اكسر;
    }

    // join all the lines
    tmp = &سطریں[cmd_شروع];
    ga_concat_len(&ga, tmp->string, tmp->لمائی);
    لكل (j = cmd_شروع + 1; j <= cmd_سرا; j++)
    {
	char_u	*p;

	tmp = &سطریں[j];
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
	    ga_concat_len(&ga, p, (tmp->string + tmp->لمائی) - p);
	}
    }
    ga_append(&ga, NUL);
    str = vim_strnsave(ga.ga_انکڑے, ga.ga_len);
    ga_clear(&ga);

    *idx = cmd_شروع;
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
    لما	i;
    char_u	*p;
    صحيح		retval = OK;
    صحيح		remap;

    // repeat previous one
    إذا (regname == '@')
    {
	إذا (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_ورتیا_رجسٹر));
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

#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#جے_انت

    // black hole: don't stuff anything
    إذا (regname == '_')
	أرجع OK;

    // use last command line
    إذا (regname == ':')
    {
	إذا (آخری_cmdline == عدم)
	{
	    emsg(_(e_no_پچھلا_command_سطر));
	    أرجع FAIL;
	}
	// don't keep the cmdline containing @:
	VIM_CLEAR(new_آخری_cmdline);
	// Escape all control characters with a CTRL-V
	p = vim_strsave_escaped_ext(آخری_cmdline,
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
		retval = put_وچ_typebuf(p + 5, TRUE, TRUE, silent);
	    وإلا
		retval = put_وچ_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#جے_تعریف_شدہ FEAT_EVAL
    وإلا إذا (regname == '=')
    {
	p = get_اظہاریہ_سطر();
	إذا (p == عدم)
	    أرجع FAIL;
	retval = put_وچ_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#جے_انت
    وإلا إذا (regname == '.')		// use last inserted text
    {
	p = get_آخری_داخل_کرو_save();
	إذا (p == عدم)
	{
	    emsg(_(e_no_inserted_متن_yet));
	    أرجع FAIL;
	}
	retval = put_وچ_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    وإلا
    {
	get_yank_رجسٹر(regname, FALSE);
	إذا (y_current->y_array == عدم)
	    أرجع FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_وچ_typebuf(silent);
	لكل (i = y_current->y_ناپ; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    صحيح	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    إذا (y_current->y_type == MLINE || i < y_current->y_ناپ - 1
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
		    str = execreg_سطر_continuation(y_current->y_array, &i);
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
	pending_سرا_reg_executing = FALSE;
    }
    أرجع retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ساكن فراغ
put_reedit_وچ_typebuf(صحيح silent)
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
put_وچ_typebuf(
    char_u	*s,
    صحيح		esc,
    صحيح		colon,	    // add ':' before the line
    صحيح		silent)
{
    صحيح		retval = OK;

    put_reedit_وچ_typebuf(silent);
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
داخل_کرو_reg(
    صحيح		regname,
    صحيح		literally_arg)	// insert literally, not as if typed
{
    لما	i;
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

#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#جے_انت

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
	إذا (get_yank_رجسٹر(regname, FALSE))
	    literally = TRUE;
	إذا (y_current->y_array == عدم)
	    retval = FAIL;
	وإلا
	{
	    لكل (i = 0; i < y_current->y_ناپ; ++i)
	    {
		إذا (regname == '-' && y_current->y_type == MCHAR)
		{
		    صحيح dir = BACKWARD;
		    إذا ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			إذا (u_save_cursor() == FAIL)
			    أرجع FAIL;
			del_chars((لما)mb_charlen(y_current->y_array[0].string), TRUE);
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
		    إذا (y_current->y_type == MLINE || i < y_current->y_ناپ - 1)
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

#جے_تعریف_شدہ FEAT_EVAL
	حالة '=':		// result of expression
	    *argp = get_اظہاریہ_سطر();
	    *allocated = TRUE;
	    أرجع TRUE;
#جے_انت

	حالة ':':		// last command line
	    إذا (آخری_cmdline == عدم && errmsg)
		emsg(_(e_no_پچھلا_command_سطر));
	    *argp = آخری_cmdline;
	    أرجع TRUE;

	حالة '/':		// last search-pattern
	    إذا (آخری_تلاش_pat() == عدم && errmsg)
		emsg(_(e_no_پچھلا_regular_عبارت));
	    *argp = آخری_تلاش_pat();
	    أرجع TRUE;

	حالة '.':		// last inserted text
	    *argp = get_آخری_داخل_کرو_save();
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
	    cnt = لبھو_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (لبھو_IDENT|لبھو_STRING) : لبھو_STRING);
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
    لما	i;
    صحيح		literally = literally_arg;

    إذا (get_yank_رجسٹر(regname, FALSE))
	literally = TRUE;
    إذا (y_current->y_array == عدم)
	أرجع FAIL;

    لكل (i = 0; i < y_current->y_ناپ; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	إذا ((y_current->y_type == MLINE || i < y_current->y_ناپ - 1) && !remcr)
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
کھسکاؤ_delete_registers(فراغ)
{
    صحيح		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    لكل (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    إذا (!y_append)
	y_پچھلا = y_current;
    y_regs[1].y_array = عدم;		// set register one to empty
}

#إذا defined(FEAT_EVAL)
    ساكن فراغ
جوڑو_regtype_منزل_dict(صحيح regname, dict_T *dict, char_u *buf, صحيح bufsize)
{
    size_t  buflen;
    لما    reglen;
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
    (فراغ)dict_جوڑو_string_len(dict, "regtype", buf, (صحيح)buflen);
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
    لكل (n = 0; n < reg->y_ناپ; n++)
	list_append_string(list, reg->y_array[n].string, (صحيح)reg->y_array[n].لمائی);
    list->lv_جندرا = VAR_FIXED;
    إذا (dict_جوڑو_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (فراغ)dict_جوڑو_string_len(v_event, "regname", buf, (صحيح)buflen);

    // motion type: inclusive or exclusive
    (فراغ)dict_جوڑو_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (فراغ)dict_جوڑو_string_len(v_event, "operator", buf, (صحيح)buflen);

    جوڑو_regtype_منزل_dict(oap->regname, v_event, buf, حجم(buf));

    // selection type - visual or not
    (فراغ)dict_جوڑو_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_سیٹ_اشیاء_ro(v_event);

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
	string_T    *داخل_کرو,	// Not NULL if special register, except '.'
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

    إذا (regname != '.' && داخل_کرو == عدم
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
	إذا (آخری_داخل_کرو_ga.ga_انکڑے != عدم)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, آخری_داخل_کرو_ga.ga_انکڑے,
		    (صحيح)آخری_داخل_کرو_ga.ga_len);
    }
    وإلا إذا (داخل_کرو != عدم)
    {
	list_append_string(list, داخل_کرو->string, (صحيح)داخل_کرو->لمائی);
    }
    وإلا
    {
	لكل (n = 0; n < reg->y_ناپ; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (صحيح)reg->y_array[n].لمائی);
    }

    list->lv_جندرا = VAR_FIXED;
    إذا (dict_جوڑو_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (فراغ)dict_جوڑو_string_len(v_event, "regname", buf, (صحيح)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (فراغ)dict_جوڑو_string_len(v_event, "operator", buf, (صحيح)buflen);

    جوڑو_regtype_منزل_dict(regname, v_event, buf, حجم(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (فراغ)dict_جوڑو_bool(v_event, "visual", VIsual_سرگرم);

    // Lock the dictionary and its keys
    dict_سیٹ_اشیاء_ro(v_event);

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
#جے_انت

/*
 * set all the yank registers to empty (called from main())
 */
    فراغ
init_yank(فراغ)
{
    صحيح		i;

    لكل (i = 0; i < عدد_REGISTERS; ++i)
	y_regs[i].y_array = عدم;
}

#إذا defined(EXITFREE)
    فراغ
clear_registers(فراغ)
{
    صحيح		i;

    لكل (i = 0; i < عدد_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	إذا (y_current->y_array != عدم)
	    free_yank_all();
    }
}
#جے_انت

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    ساكن فراغ
free_yank(لما n)
{
    إذا (y_current->y_array == عدم)
	أرجع;

    لما	    i;

    لكل (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    فراغ
free_yank_all(فراغ)
{
    free_yank(y_current->y_ناپ);
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
    لما		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    صحيح			yanktype = oap->motion_type;
    لما		yanklines = oap->سطر_گنتی;
    linenr_T		yankendlnum = oap->سرا.lnum;
    char_u		*pnew;
    بنية بلاک_def	bd;

				    // check for read-only register
    إذا (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	أرجع FAIL;
    }
    إذا (oap->regname == '_')	    // black hole: nothing to do
	أرجع OK;

#جے_تعریف_شدہ FEAT_CLIPBOARD
    إذا ((!clip_تارا.دستیاب && oap->regname == '*') ||
	(!clip_plus.دستیاب && oap->regname == '+'))
    {
	oap->regname = 0;
	پیغام_warn_missing_clipboard();
    }
#جے_انت

    إذا (!deleting)		    // op_delete() already set y_current
	get_yank_رجسٹر(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    إذا (y_append && y_current->y_array != عدم)
	y_current = &newreg;
    وإلا
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    إذا (       oap->motion_type == MCHAR
	    && oap->شروع.خانہ == 0
	    && !oap->inclusive
	    && (!oap->is_VIsual || *p_sel == 'o')
	    && !oap->بلاک_منوال
	    && oap->سرا.خانہ == 0
	    && yanklines > 1)
    {
	yanktype = MLINE;
	--yankendlnum;
	--yanklines;
    }

    y_current->y_ناپ = yanklines;
    y_current->y_type = yanktype;   // set the yank register type
    y_current->y_چوڑائی = 0;
    y_current->y_array = lalloc_clear(حجم(string_T) * yanklines, TRUE);
    إذا (y_current->y_array == عدم)
    {
	y_current = curr;
	أرجع FAIL;
    }
#جے_تعریف_شدہ FEAT_VIMINFO
    y_current->y_ویلا_سیٹ = vim_ویلا();
#جے_انت

    y_idx = 0;
    lnum = oap->شروع.lnum;

    إذا (oap->بلاک_منوال)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_چوڑائی = oap->سرا_vcol - oap->شروع_vcol;

	إذا (curwin->w_curswant == MAXCOL && y_current->y_چوڑائی > 0)
	    y_current->y_چوڑائی--;
    }

    لكل ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	بدل (y_current->y_type)
	{
	    حالة MBLOCK:
		بلاک_prep(oap, &bd, lnum, FALSE);
		إذا (yank_copy_سطر(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    اذهب fail;
		اكسر;

	    حالة MLINE:
		y_current->y_array[y_idx].لمائی = ml_get_len(lnum);
		إذا ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].لمائی)) == عدم)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    اذهب fail;
		}
		اكسر;

	    حالة MCHAR:
		{
		    صحيح tmp;

		    charwise_بلاک_prep(oap->شروع, oap->سرا, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (صحيح)STRLEN(bd.textstart);
		    إذا (tmp < bd.textlen)
			bd.textlen = tmp;

		    إذا (yank_copy_سطر(&bd, y_idx, FALSE) == FAIL)
			اذهب fail;
		    اكسر;
		}
		// NOTREACHED
	}
    }

    إذا (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	لما j;

	new_ptr = ALLOC_MULT(string_T, curr->y_ناپ + y_current->y_ناپ);
	إذا (new_ptr == عدم)
	    اذهب fail;
	لكل (j = 0; j < curr->y_ناپ; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#جے_تعریف_شدہ FEAT_VIMINFO
	curr->y_ویلا_سیٹ = vim_ویلا();
#جے_انت

	إذا (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	إذا (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == عدم)
	{
	    pnew = alloc(curr->y_array[curr->y_ناپ - 1].لمائی + y_current->y_array[0].لمائی + 1);
	    إذا (pnew == عدم)
	    {
		y_idx = y_current->y_ناپ - 1;
		اذهب fail;
	    }

	    --j;
	    STRCPY(pnew, curr->y_array[j].string);
	    STRCPY(pnew + curr->y_array[j].لمائی, y_current->y_array[0].string);
	    vim_free(curr->y_array[j].string);
	    curr->y_array[j].string = pnew;
	    curr->y_array[j].لمائی = curr->y_array[j].لمائی + y_current->y_array[0].لمائی;
	    ++j;
	    VIM_CLEAR_STRING(y_current->y_array[0]);
	    y_idx = 1;
	}
	وإلا
	    y_idx = 0;
	طالما (y_idx < y_current->y_ناپ)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_ناپ = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    إذا (mess)			// Display message about yank?
    {
	إذا (yanktype == MCHAR
		&& !oap->بلاک_منوال
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
	    إذا (oap->بلاک_منوال)
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
	curbuf->b_op_شروع = oap->شروع;
	curbuf->b_op_سرا = oap->سرا;
	إذا (yanktype == MLINE && !oap->بلاک_منوال)
	{
	    curbuf->b_op_شروع.خانہ = 0;
	    curbuf->b_op_سرا.خانہ = MAXCOL;
	}
	إذا (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_سرا);
    }

#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    إذا (curr == &y_regs[REAL_PLUS_رجسٹر] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	إذا (curr != &(y_regs[REAL_PLUS_رجسٹر]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_رجسٹر]));
	call_clip_provider_سیٹ('+');
    }
    إذا (curr == &y_regs[تارا_رجسٹر] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	إذا (curr != &(y_regs[تارا_رجسٹر]))
	    copy_yank_reg(&(y_regs[تارا_رجسٹر]));
	call_clip_provider_سیٹ('*');
    }
#جے_انت

#جے_تعریف_شدہ FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	إذا (clip_تارا.دستیاب
		&& (curr == &(y_regs[تارا_رجسٹر])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    إذا (curr != &(y_regs[تارا_رجسٹر]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[تارا_رجسٹر]));

	    clip_own_selection(&clip_تارا);
	    clip_gen_سیٹ_selection(&clip_تارا);
	}

# إذا defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	إذا (clip_plus.دستیاب
		&& (curr == &(y_regs[PLUS_رجسٹر])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    إذا (curr != &(y_regs[PLUS_رجسٹر]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_رجسٹر]));

	    clip_own_selection(&clip_plus);
	    clip_gen_سیٹ_selection(&clip_plus);
	}
# endif
    }
#جے_انت

#إذا defined(FEAT_EVAL)
    إذا (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#جے_انت
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
yank_copy_سطر(بنية بلاک_def *bd, لما y_idx, صحيح exclude_trailing_space)
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

    y_current->y_array[y_idx].لمائی = (size_t)(pnew - y_current->y_array[y_idx].string);

    أرجع OK;
}

#جے_تعریف_شدہ HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    ساكن فراغ
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    لما	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    حجم(string_T) * y_current->y_ناپ, TRUE);
    إذا (y_current->y_array == عدم)
	y_current->y_ناپ = 0;
    وإلا
	لكل (j = 0; j < y_current->y_ناپ; ++j)
	{
	    إذا ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].لمائی)) == عدم)
	    {
		free_yank(j);
		y_current->y_ناپ = 0;
		اكسر;
	    }
	    y_current->y_array[j].لمائی = curr->y_array[j].لمائی;
	}
    y_current = curr;
}
#جے_انت

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
    char_u	*اظہاریہ_نتیجہ,	// result for regname "=" when compiled
    صحيح		dir,		// BACKWARD for 'P', FORWARD for 'p'
    لما	گنتی,
    صحيح		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    صحيح		yanklen;
    صحيح		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	خانہ;
    لما	i;			// index in y_array[]
    صحيح		y_type;
    لما	y_ناپ;
    صحيح		oldlen;
    لما	y_چوڑائی = 0;
    colnr_T	vcol;
    string_T	*y_array = عدم;
    yankreg_T	*y_current_ورتیا = عدم;
    لما	nr_سطریں = 0;
    string_T	داخل_کرو_string;
    صحيح		allocated = FALSE;
    pos_T	orig_شروع = curbuf->b_op_شروع;
    pos_T	orig_سرا = curbuf->b_op_سرا;
    بنانشانی صحيح cur_ve_flags = get_ve_flags();

#جے_تعریف_شدہ HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(فراغ)may_get_selection(regname);
#جے_انت

    // Remove any preinserted text (issue #19329)
    إذا (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_شروع = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_سرا = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    إذا (regname == '.')
    {
	إذا (VIsual_سرگرم)
	    stuffcharReadbuff(VIsual_منوال);

#جے_تعریف_شدہ FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	إذا (has_textput_events)
	    جوڑو_آخری_داخل_کرو++;
#جے_انت

	(فراغ)stuff_inserted((dir == FORWARD ? (گنتی == -1 ? 'o' : 'a') :
				    (گنتی == -1 ? 'O' : 'i')), گنتی, FALSE);

#جے_تعریف_شدہ FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	إذا (has_textputpre())
	    put_do_autocmd('.', عدم, عدم, false, dir);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#جے_انت
#جے_تعریف_شدہ FEAT_EVAL
	إذا (has_textputpost())
	    put_do_autocmd('.', عدم, عدم, true, dir);

	إذا (has_textput_events && --جوڑو_آخری_داخل_کرو == 0)
	    ga_clear(&آخری_داخل_کرو_ga);
#جے_انت

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	إذا ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	أرجع;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    داخل_کرو_string.string = عدم;
    إذا (regname == '=' && اظہاریہ_نتیجہ != عدم)
	داخل_کرو_string.string = اظہاریہ_نتیجہ;
    وإلا إذا (get_spec_reg(regname, &داخل_کرو_string.string, &allocated, TRUE)
		&& داخل_کرو_string.string == عدم)
    {
#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#جے_انت
	أرجع;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    إذا (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	اذهب سرا;

    إذا (داخل_کرو_string.string != عدم)
    {
	داخل_کرو_string.لمائی = STRLEN(داخل_کرو_string.string);

	y_type = MCHAR;
#جے_تعریف_شدہ FEAT_EVAL
	إذا (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    لكل (;;)
	    {
		y_ناپ = 0;
		ptr = داخل_کرو_string.string;
		ptrlen = داخل_کرو_string.لمائی;
		طالما (ptr != عدم)
		{
		    إذا (y_array != عدم)
			y_array[y_ناپ].string = ptr;
		    ++y_ناپ;
		    tmp = vim_strchr(ptr, '\n');
		    إذا (tmp == عدم)
		    {
			إذا (y_array != عدم)
			    y_array[y_ناپ - 1].لمائی = ptrlen;
		    }
		    وإلا
		    {
			إذا (y_array != عدم)
			{
			    *tmp = NUL;
			    y_array[y_ناپ - 1].لمائی = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_ناپ - 1].لمائی + 1;
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
		y_array = ALLOC_MULT(string_T, y_ناپ);
		إذا (y_array == عدم)
		    اذهب سرا;
	    }
	}
	وإلا
#جے_انت
	{
	    y_ناپ = 1;		// use fake one-line yank register
	    y_array = &داخل_کرو_string;
	}
#جے_تعریف_شدہ FEAT_EVAL
	إذا (has_textputpre())
	    put_do_autocmd(regname, عدم, &داخل_کرو_string, false, dir);
#جے_انت
    }
    وإلا
    {
#جے_تعریف_شدہ FEAT_EVAL
	إذا (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_رجسٹر(regname, FALSE);
	    put_do_autocmd(regname, y_current, عدم, false, dir);
	}
#جے_انت
	get_yank_رجسٹر(regname, FALSE);

	y_type = y_current->y_type;
	y_چوڑائی = y_current->y_چوڑائی;
	y_ناپ = y_current->y_ناپ;
	y_array = y_current->y_array;
	y_current_ورتیا = y_current;
    }

    إذا (y_type == MLINE)
    {
	إذا (flags & PUT_سطر_ونڈو)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    إذا (u_save_cursor() == FAIL)
		اذهب سرا;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    إذا (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    إذا (ptr == عدم)
		اذهب سرا;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.خانہ;
	    إذا (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    إذا (ptr == عدم)
		اذهب سرا;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_سطریں;
	    dir = FORWARD;
	}
	إذا (flags & PUT_سطر_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_سرا;
	    dir = FORWARD;
	}
	curbuf->b_op_شروع = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_سرا = curwin->w_cursor;	// default for '] mark
    }

    إذا (flags & PUT_سطر)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    إذا (y_ناپ == 0 || y_array == عدم)
    {
	semsg(_(e_nothing_وچ_رجسٹر_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	اذهب سرا;
    }

    إذا (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_ناپ + 1;
	إذا (lnum > curbuf->b_ml.ml_سطر_گنتی)
	    lnum = curbuf->b_ml.ml_سطر_گنتی + 1;
	إذا (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    اذهب سرا;
    }
    وإلا إذا (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#جے_تعریف_شدہ FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	إذا (dir == BACKWARD)
	    (فراغ)hasFolding(lnum, &lnum, عدم);
	وإلا
	    (فراغ)hasFolding(lnum, عدم, &lnum);
#جے_انت
	إذا (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	إذا ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    اذهب سرا;
#جے_تعریف_شدہ FEAT_FOLDING
	إذا (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	وإلا
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_شروع = curwin->w_cursor;	// for mark_adjust()
#جے_انت
    }
    وإلا إذا (u_save_cursor() == FAIL)
	اذهب سرا;

    إذا (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	إذا (gchar_cursor() == TAB)
	{
	    صحيح viscol = getviscol();
	    صحيح ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    إذا (dir == FORWARD ?
#جے_تعریف_شدہ FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#وإلا
		    ts - (viscol % ts) != 1
#جے_انت
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    وإلا
		curwin->w_cursor.coladd = 0;
	}
	وإلا إذا (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    خانہ = curwin->w_cursor.خانہ;

    // Block mode
    إذا (y_type == MBLOCK)
    {
	صحيح	delcount;
	صحيح	incr = 0;
	بنية بلاک_def bd;
	لما	j;
	صحيح	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	إذا (dir == FORWARD && c != NUL)
	{
	    إذا (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &خانہ, عدم, &endcol2, 0);
	    وإلا
		getvcol(curwin, &curwin->w_cursor, عدم, عدم, &خانہ, 0);

	    إذا (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.خانہ += (*mb_ptr2len)(ml_get_cursor());
	    وإلا
		إذا (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.خانہ;
	    ++خانہ;
	}
	وإلا
	    getvcol(curwin, &curwin->w_cursor, &خانہ, عدم, &endcol2, 0);

	خانہ += curwin->w_cursor.coladd;
	إذا (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.خانہ))
	{
	    إذا (dir == FORWARD && c == NUL)
		++خانہ;
	    إذا (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.خانہ;
	    إذا (c == TAB)
	    {
		إذا (dir == BACKWARD && curwin->w_cursor.خانہ)
		    curwin->w_cursor.خانہ--;
		إذا (dir == FORWARD && خانہ - 1 == endcol2)
		    curwin->w_cursor.خانہ++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	لكل (i = 0; i < y_ناپ; ++i)
	{
	    صحيح		    spaces = 0;
	    حرف	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    إذا (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
	    {
		إذا (ml_append(curbuf->b_ml.ml_سطر_گنتی, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    اكسر;
		++nr_سطریں;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    طالما (cts.cts_vcol < خانہ && *cts.cts_ptr != NUL)
	    {
		// Count a tab for what it's worth (if list mode not on)
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    vcol = cts.cts_vcol;
	    ptr = cts.cts_ptr;
	    bd.textcol = (colnr_T)(ptr - oldp);
	    clear_chartabsize_arg(&cts);

	    shortline = (vcol < خانہ) || (vcol == خانہ && !*ptr) ;

	    إذا (vcol < خانہ) // line too short, pad with spaces
		bd.startspaces = خانہ - vcol;
	    وإلا إذا (vcol > خانہ)
	    {
		bd.endspaces = vcol - خانہ;
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

	    yanklen = (صحيح)y_array[i].لمائی;

	    إذا ((flags & PUT_بلاک_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		//بلاک
		spaces = y_چوڑائی + 1;
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
		     && گنتی > ((ودھ_توں_ودھ_عدد - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_متن_too_long));
		اكسر;
	    }

	    totlen = گنتی * (yanklen + spaces) + bd.startspaces + bd.endspaces;
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
	    لكل (j = 0; j < گنتی; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		إذا ((j < گنتی - 1 || !shortline) && spaces > 0)
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
		curwin->w_cursor.خانہ += bd.startspaces;
	}

	بدلیا_سطریں(lnum, 0, curwin->w_cursor.lnum - nr_سطریں, nr_سطریں);

	// Set '[ mark.
	curbuf->b_op_شروع = curwin->w_cursor;
	curbuf->b_op_شروع.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_سرا.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_سرا.خانہ = bd.textcol + totlen - 1;
	إذا (curbuf->b_op_سرا.خانہ < 0)
	    curbuf->b_op_سرا.خانہ = 0;
	curbuf->b_op_سرا.coladd = 0;
	إذا (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_سرا;
	    curwin->w_cursor.خانہ++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    إذا (curwin->w_cursor.خانہ > len)
		curwin->w_cursor.خانہ = len;
	}
	وإلا
	    curwin->w_cursor.lnum = lnum;
    }
    وإلا
    {
	pos_T	new_cursor;

	yanklen = (صحيح)y_array[0].لمائی;

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
		    خانہ += bytelen;
		    إذا (yanklen)
		    {
			curwin->w_cursor.خانہ += bytelen;
			curbuf->b_op_سرا.خانہ += bytelen;
		    }
		}
		وإلا
		{
		    ++خانہ;
		    إذا (yanklen)
		    {
			++curwin->w_cursor.خانہ;
			++curbuf->b_op_سرا.خانہ;
		    }
		}
	    }
	    curbuf->b_op_شروع = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	وإلا إذا (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	إذا (y_type == MCHAR && y_ناپ == 1)
	{
	    linenr_T	سرا_lnum = 0; // init for gcc
	    linenr_T	شروع_lnum = lnum;
	    صحيح		پہلا_byte_off = 0;

	    إذا (VIsual_سرگرم)
	    {
		سرا_lnum = curbuf->b_visual.vi_سرا.lnum;
		إذا (سرا_lnum < curbuf->b_visual.vi_شروع.lnum)
		    سرا_lnum = curbuf->b_visual.vi_شروع.lnum;
		إذا (سرا_lnum > شروع_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.خانہ = خانہ;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, عدم, &vcol, عدم, 0);
		}
	    }

	    إذا (گنتی == 0 || yanklen == 0)
	    {
		إذا (VIsual_سرگرم)
		    lnum = سرا_lnum;
	    }
	    وإلا إذا (گنتی > ودھ_توں_ودھ_عدد / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_متن_too_long));
	    وإلا
	    {
		totlen = گنتی * yanklen;
		افعل
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    إذا (lnum > شروع_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			إذا (getvpos(&pos, vcol) == OK)
			    خانہ = pos.خانہ;
			وإلا
			    خانہ = MAXCOL;
		    }
		    إذا (VIsual_سرگرم && خانہ > oldlen)
		    {
			lnum++;
			استمر;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    إذا (newp == عدم)
			اذهب سرا;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)خانہ);
		    ptr = newp + خانہ;
		    لكل (i = 0; i < گنتی; ++i)
		    {
			mch_memmove(ptr, y_array[0].string, (size_t)yanklen);
			ptr += yanklen;
		    }
		    mch_memmove(ptr, oldp + خانہ, (size_t)(oldlen - خانہ) + 1);	    // +1 for NUL

		    // compute the byte offset for the last character
		    پہلا_byte_off = mb_سر_off(newp, ptr - 1);

		    // Note: this may free "newp"
		    ml_replace(lnum, newp, FALSE);

		    inserted_بائٹاں(lnum, خانہ, totlen);

		    // Place cursor on last putted char.
		    إذا (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			بدلیا_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.خانہ += (colnr_T)(totlen - 1);
		    }
		    إذا (VIsual_سرگرم)
			lnum++;
		} طالما (VIsual_سرگرم && lnum <= سرا_lnum);

		إذا (VIsual_سرگرم) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_سرا = curwin->w_cursor;
	    curbuf->b_op_سرا.خانہ -= پہلا_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    إذا (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.خانہ;
	    وإلا
		curwin->w_cursor.خانہ -= پہلا_byte_off;
	}
	وإلا
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    صحيح		indent;
	    صحيح		orig_indent = 0;
	    صحيح		indent_diff = 0;	// init for gcc
	    صحيح		پہلا_indent = TRUE;
	    صحيح		lendiff = 0;
	    لما	cnt;

	    إذا (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    لكل (cnt = 1; cnt <= گنتی; ++cnt)
	    {
		i = 0;
		إذا (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + خانہ;
		    totlen = (صحيح)y_array[y_ناپ - 1].لمائی;
		    newp = alloc(ml_get_len(lnum) - خانہ + totlen + 1);
		    إذا (newp == عدم)
			اذهب error;
		    STRCPY(newp, y_array[y_ناپ - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(خانہ + yanklen + 1);
		    إذا (newp == عدم)
			اذهب error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)خانہ);
					    // append to first line
		    mch_memmove(newp + خانہ, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		لكل (; i < y_ناپ; ++i)
		{
		    إذا (y_type != MCHAR || i < y_ناپ - 1)
		    {
			إذا (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    اذهب error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_سطریں;
		    إذا (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			إذا (cnt == گنتی && i == y_ناپ - 1)
			    lendiff = ml_get_len(lnum);
			إذا (*ptr == '#' && preprocs_بایاں())
			    indent = 0;     // Leave # lines at start
			وإلا إذا (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			وإلا إذا (پہلا_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    پہلا_indent = FALSE;
			}
			وإلا إذا ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(فراغ)سیٹ_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			إذا (cnt == گنتی && i == y_ناپ - 1)
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
		curbuf->b_op_شروع.خانہ = 0;
		إذا (dir == FORWARD)
		    curbuf->b_op_شروع.lnum++;
	    }
	    mark_adjust(curbuf->b_op_شروع.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_سطریں, 0L);

	    // note changed text for displaying and folding
	    إذا (y_type == MCHAR)
		بدلیا_سطریں(curwin->w_cursor.lnum, خانہ,
					 curwin->w_cursor.lnum + 1, nr_سطریں);
	    وإلا
		بدلیا_سطریں(curbuf->b_op_شروع.lnum, 0,
					   curbuf->b_op_شروع.lnum, nr_سطریں);
	    إذا (y_current_ورتیا != عدم && (y_current_ورتیا != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_رجسٹر_بدلیا_while_using_it));
		اذهب سرا;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_سرا.lnum = new_lnum;
	    خانہ = MAX(0, (colnr_T)y_array[y_ناپ - 1].لمائی - lendiff);
	    إذا (خانہ > 1)
	    {
		curbuf->b_op_سرا.خانہ = خانہ - 1;
		إذا (y_array[y_ناپ - 1].لمائی > 0)
		    curbuf->b_op_سرا.خانہ -= mb_سر_off(y_array[y_ناپ - 1].string,
						y_array[y_ناپ - 1].string + y_array[y_ناپ - 1].لمائی - 1);
	    }
	    وإلا
		curbuf->b_op_سرا.خانہ = 0;

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
		    إذا (lnum >= curbuf->b_ml.ml_سطر_گنتی)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
		    وإلا
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.خانہ = 0;
		}
		وإلا
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.خانہ = خانہ;
		    curbuf->b_op_سرا = curwin->w_cursor;
		    إذا (خانہ > 1)
			curbuf->b_op_سرا.خانہ = خانہ - 1;
		}
	    }
	    وإلا إذا (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.خانہ = 0;
		إذا (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    وإلا	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_سطریں);
    curwin->w_سیٹ_curswant = true;

    // Make sure the cursor is not after the NUL.
    صحيح len = ml_get_curline_len();
    إذا (curwin->w_cursor.خانہ > len)
    {
	إذا (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.خانہ - len;
	curwin->w_cursor.خانہ = len;
    }

سرا:
    إذا (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_شروع = orig_شروع;
	curbuf->b_op_سرا = orig_سرا;
    }

#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#جے_انت

#جے_تعریف_شدہ FEAT_EVAL
    إذا (has_textputpost())
    {
	إذا (داخل_کرو_string.string == عدم)
	    put_do_autocmd(regname, y_current, عدم, true, dir);
	وإلا
	    put_do_autocmd(regname, عدم, &داخل_کرو_string, true, dir);
    }
#جے_انت

    إذا (allocated)
	vim_free(داخل_کرو_string.string);
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
get_رجسٹر_name(صحيح عدد)
{
    إذا (عدد == -1)
	أرجع '"';
    وإلا إذا (عدد < 10)
	أرجع عدد + '0';
    وإلا إذا (عدد == DELETION_رجسٹر)
	أرجع '-';
#إذا defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    وإلا إذا (عدد == تارا_رجسٹر)
	أرجع '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    وإلا إذا (clipmethod == CLIPMETHOD_PROVIDER && عدد == REAL_PLUS_رجسٹر)
	أرجع '+';
    وإلا إذا (clipmethod != CLIPMETHOD_PROVIDER && عدد == PLUS_رجسٹر)
	أرجع '+';
#جے_انت
    وإلا
	أرجع عدد + 'a' - 10;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    صحيح
get_unname_رجسٹر(فراغ)
{
    أرجع y_پچھلا == عدم ? -1 : y_پچھلا - &y_regs[0];
}
#جے_انت

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    فراغ
ex_display(exarg_T *eap)
{
    صحيح		i, n;
    لما	j;
    char_u	*p;
    yankreg_T	*yb;
    صحيح		name;
    صحيح		attr;
    char_u	*arg = eap->arg;
    صحيح		clen;
    صحيح		type;
    string_T	داخل_کرو;

    silence_w23_w24_پیغام++;
    إذا (arg != عدم && *arg == NUL)
	arg = عدم;
    attr = HL_ATTR(HLF_8);

#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#جے_انت

    // Highlight title
    پیغام_puts_title(_("\nType Name Content"));
    لكل (i = -1; i < عدد_REGISTERS && !got_int; ++i)
    {
	name = get_رجسٹر_name(i);
	بدل (get_reg_type(name, عدم))
	{
	    حالة MLINE: type = 'l'; اكسر;
	    حالة MCHAR: type = 'c'; اكسر;
	    افتراضي:	type = 'b'; اكسر;
	}
	إذا (arg != عدم && vim_strchr(arg, name) == عدم
#جے_تعریف_شدہ ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == عدم)
#جے_انت
		)
	    استمر;	    // did not ask for this register

#جے_تعریف_شدہ HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD
	إذا (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (فراغ)may_get_selection(name);
#جے_انت

	إذا (i == -1)
	{
	    إذا (y_پچھلا != عدم)
		yb = y_پچھلا;
	    وإلا
		yb = &(y_regs[0]);
	}
	وإلا
	    yb = &(y_regs[i]);

#جے_تعریف_شدہ FEAT_EVAL
	إذا (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != عدم &&
		    (yb == y_پچھلا || yb == &y_regs[0])))
	    استمر;	    // do not list register being written to, the
			    // pointer can be freed
#جے_انت

	إذا (yb->y_array != عدم)
	{
	    صحيح do_show = FALSE;

	    لكل (j = 0; !do_show && j < yb->y_ناپ; ++j)
		do_show = !سنیہا_filtered(yb->y_array[j].string);

	    إذا (do_show || yb->y_ناپ == 0)
	    {
		پیغام_putchar('\n');
		پیغام_puts("  ");
		پیغام_putchar(type);
		پیغام_puts("  ");
		پیغام_putchar('"');
		پیغام_putchar(name);
		پیغام_puts("   ");

		n = (صحيح)Columns - 11;
		لكل (j = 0; j < yb->y_ناپ && n > 1; ++j)
		{
		    إذا (j)
		    {
			پیغام_puts_attr("^J", attr);
			n -= 2;
		    }
		    لكل (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			پیغام_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		إذا (n > 1 && yb->y_type == MLINE)
		    پیغام_puts_attr("^J", attr);
		باہرلا_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    داخل_کرو = get_آخری_داخل_کرو();
    إذا ((p = داخل_کرو.string) != عدم
		  && (arg == عدم || vim_strchr(arg, '.') != عدم) && !got_int
						      && !سنیہا_filtered(p))
    {
	پیغام_puts("\n  c  \".   ");
	dis_پیغام(p, TRUE);
    }

    // display last command line
    إذا (آخری_cmdline != عدم && (arg == عدم || vim_strchr(arg, ':') != عدم)
			       && !got_int && !سنیہا_filtered(آخری_cmdline))
    {
	پیغام_puts("\n  c  \":   ");
	dis_پیغام(آخری_cmdline, FALSE);
    }

    // display current file name
    إذا (curbuf->b_fname != عدم
	    && (arg == عدم || vim_strchr(arg, '%') != عدم) && !got_int
					&& !سنیہا_filtered(curbuf->b_fname))
    {
	پیغام_puts("\n  c  \"%   ");
	dis_پیغام(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    إذا ((arg == عدم || vim_strchr(arg, '#') != عدم) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	إذا (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !سنیہا_filtered(fname))
	{
	    پیغام_puts("\n  c  \"#   ");
	    dis_پیغام(fname, FALSE);
	}
    }

    // display last search pattern
    إذا (آخری_تلاش_pat() != عدم
		 && (arg == عدم || vim_strchr(arg, '/') != عدم) && !got_int
				      && !سنیہا_filtered(آخری_تلاش_pat()))
    {
	پیغام_puts("\n  c  \"/   ");
	dis_پیغام(آخری_تلاش_pat(), FALSE);
    }

#جے_تعریف_شدہ FEAT_EVAL
    // display last used expression
    إذا (اظہاریہ_سطر != عدم && (arg == عدم || vim_strchr(arg, '=') != عدم)
				  && !got_int && !سنیہا_filtered(اظہاریہ_سطر))
    {
	پیغام_puts("\n  c  \"=   ");
	dis_پیغام(اظہاریہ_سطر, FALSE);
    }
#جے_انت

#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#جے_انت
    silence_w23_w24_پیغام--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    ساكن فراغ
dis_پیغام(
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
	    پیغام_outtrans_len(p, l);
	    p += l;
	}
	وإلا
	    پیغام_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#إذا defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    فراغ
dnd_yank_drag_انکڑے(char_u *str, لما len)
{
    yankreg_T *curr;

    curr = y_current;
    y_current = &y_regs[TILDE_رجسٹر];
    free_yank_all();
    str_منزل_reg(y_current, MCHAR, str, len, 0L, FALSE);
    y_current = curr;
}
#جے_انت


/*
 * Return the type of a register.
 * Used for getregtype()
 * Returns MAUTO for error.
 */
    char_u
get_reg_type(صحيح regname, لما *reglen)
{
    بدل (regname)
    {
	حالة '%':		// file name
	حالة '#':		// alternate file name
	حالة '=':		//عبارت
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

#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#جے_انت

    إذا (regname != NUL && !valid_yank_reg(regname, FALSE))
	أرجع MAUTO;

    get_yank_رجسٹر(regname, FALSE);

    إذا (y_current->y_array != عدم)
    {
	إذا (reglen != عدم && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_چوڑائی;
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
getreg_wrap_one_سطر(char_u *s, صحيح flags)
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
	    list->lv_پہلا->li_tv.vval.v_string = s;
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
    لما	len;

    // Don't allow using an expression register inside an expression
    إذا (regname == '=')
    {
	إذا (flags & GREG_NO_اظہاریہ)
	    أرجع عدم;
	إذا (flags & GREG_اظہاریہ_SRC)
	    أرجع getreg_wrap_one_سطر(get_اظہاریہ_سطر_src(), flags);
	أرجع getreg_wrap_one_سطر(get_اظہاریہ_سطر(), flags);
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
	    أرجع getreg_wrap_one_سطر(retval, flags);
	أرجع getreg_wrap_one_سطر(vim_strsave(retval), flags);
    }

    get_yank_رجسٹر(regname, FALSE);
    إذا (y_current->y_array == عدم)
	أرجع عدم;

    إذا (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	إذا (list == عدم)
	    أرجع عدم;
	لكل (i = 0; i < y_current->y_ناپ; ++i)
	{
	    إذا (list_append_string(list, y_current->y_array[i].string,
		(صحيح)y_current->y_array[i].لمائی) == FAIL)
	    {
		list_free(list);
		أرجع عدم;
	    }
	}
	أرجع (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    لكل (i = 0; i < y_current->y_ناپ; ++i)
    {
	len += (لما)y_current->y_array[i].لمائی;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	إذا (y_current->y_type == MLINE || i < y_current->y_ناپ - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    إذا (retval == عدم)
	أرجع عدم;

    // Copy the lines of the yank register into the string.
    len = 0;
    لكل (i = 0; i < y_current->y_ناپ; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (لما)y_current->y_array[i].لمائی;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	إذا (y_current->y_type == MLINE || i < y_current->y_ناپ - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    أرجع retval;
}

    ساكن صحيح
init_write_reg(
    صحيح		name,
    yankreg_T	**old_y_پچھلا,
    yankreg_T	**old_y_current,
    صحيح		must_append,
    صحيح		*yank_type بن_ورتیا)
{
    إذا (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	أرجع FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_پچھلا = y_پچھلا;
    *old_y_current = y_current;

    get_yank_رجسٹر(name, TRUE);
    إذا (!y_append && !must_append)
	free_yank_all();
    أرجع OK;
}

    ساكن فراغ
انت_write_reg(
    صحيح		name,
    yankreg_T	*old_y_پچھلا,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_سیٹ_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    إذا (name != '"')
	y_پچھلا = old_y_پچھلا;
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
    char_u	**لڑیاں,
    صحيح		maxlen بن_ورتیا,
    صحيح		must_append,
    صحيح		yank_type,
    لما	بلاک_len)
{
    yankreg_T  *old_y_پچھلا, *old_y_current;

    إذا (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	إذا (لڑیاں[0] == عدم)
	    s = (char_u *)"";
	وإلا إذا (لڑیاں[1] != عدم)
	{
	    semsg(_(e_رجسٹر_char_cannot_contain_مضرب_سطریں), name);
	    أرجع;
	}
	وإلا
	    s = لڑیاں[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, بلاک_len);
	أرجع;
    }

    إذا (name == '_')	    // black hole: nothing to do
	أرجع;

    إذا (init_write_reg(name, &old_y_پچھلا, &old_y_current, must_append,
		&yank_type) == FAIL)
	أرجع;

    str_منزل_reg(y_current, yank_type, (char_u *)لڑیاں, -1, بلاک_len, TRUE);

    انت_write_reg(name, old_y_پچھلا, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_سیٹ(name);
# endif
}

    فراغ
write_reg_contents_ex(
    صحيح		name,
    char_u	*str,
    صحيح		maxlen,
    صحيح		must_append,
    صحيح		yank_type,
    لما	بلاک_len)
{
    yankreg_T	*old_y_پچھلا, *old_y_current;
    لما	len;

    إذا (maxlen >= 0)
	len = maxlen;
    وإلا
	len = (لما)STRLEN(str);

    // Special case: '/' search pattern
    إذا (name == '/')
    {
	سیٹ_آخری_تلاش_pat(str, RE_تلاش, TRUE, TRUE);
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
	    صحيح	عدد = atoi((حرف *)str);

	    buf = buflist_findnr(عدد);
	    إذا (buf == عدم)
		semsg(_(e_بفر_nr_does_not_exist), (لما)عدد);
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
	إذا (must_append && اظہاریہ_سطر != عدم)
	{
	    s = concat_str(اظہاریہ_سطر, p);
	    vim_free(p);
	    p = s;
	}
	سیٹ_اظہاریہ_سطر(p, عدم);
	أرجع;
    }

    إذا (name == '_')	    // black hole: nothing to do
	أرجع;

    إذا (init_write_reg(name, &old_y_پچھلا, &old_y_current, must_append,
		&yank_type) == FAIL)
	أرجع;

    str_منزل_reg(y_current, yank_type, str, len, بلاک_len, FALSE);

    انت_write_reg(name, old_y_پچھلا, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_سیٹ(name);
# endif
}
#جے_انت	// FEAT_EVAL

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
    لما	len,		// length of string
    لما	blocklen,	// width of Visual block
    صحيح		str_list)	// TRUE if str is char_u **
{
    صحيح		type;			// MCHAR, MLINE or MBLOCK
    صحيح		lnum;
    لما	شروع;
    لما	i;
    صحيح		extra;
    صحيح		newlines;		// number of lines added
    صحيح		extraline = 0;		// extra line at the end
    صحيح		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    لما	maxlen;

    إذا (y_ptr->y_array == عدم)		// NULL means empty register
	y_ptr->y_ناپ = 0;

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
	إذا (y_ptr->y_ناپ > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    إذا (y_ptr->y_ناپ + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	أرجع;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_ناپ + newlines) * حجم(string_T), TRUE);
    إذا (pp == عدم)	// out of memory
	أرجع;
    لكل (lnum = 0; lnum < y_ptr->y_ناپ; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    إذا (str_list)
    {
	لكل (ss = (char_u **) str; *ss != عدم; ++ss, ++lnum)
	{
	    pp[lnum].لمائی = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].لمائی);
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
	لكل (شروع = 0; شروع < len + extraline; شروع += i + 1)
	{
	    صحيح charlen = 0;

	    لكل (i = شروع; i < len;)	// find the end of the line
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
	    i -= شروع;			// i is now length of line
	    إذا (charlen > maxlen)
		maxlen = charlen;
	    إذا (append)
	    {
		--lnum;
		extra = (صحيح)y_ptr->y_array[lnum].لمائی;
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
		mch_memmove(s + extra, str + شروع, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].لمائی = extra;
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
    y_ptr->y_ناپ = lnum;
    إذا (type == MBLOCK)
	y_ptr->y_چوڑائی = (blocklen < 0 ? maxlen - 1 : blocklen);
    وإلا
	y_ptr->y_چوڑائی = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_ویلا_سیٹ = vim_ویلا();
# endif
}
#جے_انت // FEAT_CLIPBOARD || FEAT_EVAL
