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
ساکن yankreg_T	y_regs[عدد_REGISTERS];

ساکن yankreg_T	*y_current;	    // ptr to current yankreg
ساکن صحیح		y_append;	    // TRUE when appending
ساکن yankreg_T	*y_پچھلا = تہی; // ptr to last written yankreg

ساکن صحیح	stuff_yank(صحیح, char_u *);
ساکن خالی	put_reedit_اندر_typebuf(صحیح silent);
ساکن صحیح	put_اندر_typebuf(char_u *s, صحیح esc, صحیح colon, صحیح silent);
ساکن صحیح	yank_copy_سطر(ساخت بلاک_def *bd, طویل y_idx, صحیح exclude_trailing_space);
#اگر_تعریف_شدہ HAVE_CLIPMETHOD
ساکن خالی	copy_yank_reg(yankreg_T *reg);
#اگر_ختم
ساکن خالی	dis_پیغام(char_u *p, صحیح skip_esc);

#اگر defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(خالی)
{
    واپس y_regs;
}
#اگر_ختم

#اگر defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_رجسٹر(صحیح reg)
{
    واپس &y_regs[reg];
}
#اگر_ختم

    yankreg_T *
get_y_current(خالی)
{
    واپس y_current;
}

    yankreg_T *
get_y_پچھلا(خالی)
{
    واپس y_پچھلا;
}

    خالی
سیٹ_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    خالی
سیٹ_y_پچھلا(yankreg_T *yreg)
{
    y_پچھلا = yreg;
}

    خالی
reset_y_append(خالی)
{
    y_append = FALSE;
}


#اگر defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
ساکن char_u	*اظہاریہ_سطر = تہی;
ساکن exarg_T	*اظہاریہ_eap = تہی;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    صحیح
get_اظہاریہ_رجسٹر(خالی)
{
    char_u	*new_سطر;

    new_سطر = getcmdline('=', 0L, 0, 0);
    اگر (new_سطر == تہی)
	واپس NUL;
    اگر (*new_سطر == NUL)	// use previous line
	vim_free(new_سطر);
    ورنہ
	سیٹ_اظہاریہ_سطر(new_سطر, تہی);
    واپس '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    خالی
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
get_اظہاریہ_سطر(خالی)
{
    char_u	*اظہاریہ_copy;
    char_u	*rv;
    ساکن صحیح	nested = 0;

    اگر (اظہاریہ_سطر == تہی)
	واپس تہی;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    اظہاریہ_copy = vim_strsave(اظہاریہ_سطر);
    اگر (اظہاریہ_copy == تہی)
	واپس تہی;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    اگر (nested >= 10)
	واپس اظہاریہ_copy;

    ++nested;
    rv = eval_مقصد_string_eap(اظہاریہ_copy, TRUE, اظہاریہ_eap, FALSE);
    --nested;
    vim_free(اظہاریہ_copy);
    واپس rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ساکن char_u *
get_اظہاریہ_سطر_src(خالی)
{
    اگر (اظہاریہ_سطر == تہی)
	واپس تہی;
    واپس vim_strsave(اظہاریہ_سطر);
}
#اگر_ختم // FEAT_EVAL

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
#اگر_تعریف_شدہ FEAT_EVAL
				    "/#.%:="
#ورنہ
				    "/#.%:"
#اگر_ختم
					, regname) != تہی)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#اگر defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#وگرنہ_اگر defined(FEAT_CLIPBOARD_PROVIDER)
	    || ( // If -clipboard, then these registers only exist when
		 // clipmethod is set to provider.
	     clipmethod == CLIPMETHOD_PROVIDER && (
		 regname == '*'
		 || regname == '+'
	    ))
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DND
	    || (!writing && regname == '~')
#اگر_ختم
							)
	واپس TRUE;
#اگر_غیر_تعریف_شدہ FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    ورنہ اگر (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	پیغام_warn_missing_clipboard();
	واپس FALSE;
    }
#اگر_ختم
    واپس FALSE;
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
get_yank_رجسٹر(صحیح regname, صحیح writing)
{
    صحیح	    i;
    صحیح	    ret = FALSE;

    y_append = FALSE;
    اگر ((regname == 0 || regname == '"') && !writing && y_پچھلا != تہی)
    {
	y_current = y_پچھلا;
	واپس ret;
    }
    i = regname;
    اگر (VIM_ISDIGIT(i))
	i -= '0';
    ورنہ اگر (ASCII_ISLOWER(i))
	i = Char_Ord_کم(i) + 10;
    ورنہ اگر (ASCII_ISUPPER(i))
    {
	i = Char_Ord_اوپر(i) + 10;
	y_append = TRUE;
    }
    ورنہ اگر (regname == '-')
	i = DELETION_رجسٹر;
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    ورنہ اگر (clip_ستارہ.دستیاب && regname == '*')
    {
	i = ستارہ_رجسٹر;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    ورنہ اگر (clip_plus.دستیاب && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	اگر (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_رجسٹر;
	ورنہ
# endif
	    i = PLUS_رجسٹر;
	ret = TRUE;
    }
#وگرنہ_اگر defined(FEAT_CLIPBOARD_PROVIDER)
    ورنہ اگر (regname == '*')
    {
	i = ستارہ_رجسٹر;
	ret = TRUE;
    }
    ورنہ اگر (regname == '+')
    {
	i = REAL_PLUS_رجسٹر;
	ret = TRUE;
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DND
    ورنہ اگر (!writing && regname == '~')
	i = TILDE_رجسٹر;
#اگر_ختم
    ورنہ		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    اگر (writing)	// remember the register we write into for do_put()
	y_پچھلا = y_current;
    واپس ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    خالی *
get_رجسٹر(
    صحیح		name,
    صحیح		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    صحیح		i;

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	اگر (name == '*' && clip_ستارہ.دستیاب)
	{
	    اگر (clip_isautosel_ستارہ())
		clip_update_selection(&clip_ستارہ);
	    may_get_selection(name);
	}
	اگر (name == '+' && clip_plus.دستیاب)
	{
	    اگر (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#اگر_ختم

    get_yank_رجسٹر(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    اگر (reg == تہی)
	واپس (خالی *)تہی;

    *reg = *y_current;
    اگر (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	اگر (reg->y_سائز == 0 || y_current->y_array == تہی)
	    reg->y_array = تہی;
	ورنہ
	    reg->y_array = ALLOC_MULT(string_T, reg->y_سائز);
	اگر (reg->y_array != تہی)
	{
	    برائے (i = 0; i < reg->y_سائز; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].لمبائی);
		اگر (reg->y_array[i].string == تہی)
		{
		    // The allocation failed so clean up and exit
		    جب (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    واپس (خالی *)تہی;
		}

		reg->y_array[i].لمبائی = y_current->y_array[i].لمبائی;
	    }
	}
    }
    ورنہ
	y_current->y_array = تہی;
    واپس (خالی *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    خالی
put_رجسٹر(صحیح name, خالی *reg)
{
    get_yank_رجسٹر(name, 0);
    free_yank_all();
    *y_current = *(yankreg_T *)reg;
    vim_free(reg);

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    // Send text written to clipboard register to the clipboard.
    may_سیٹ_selection();
#اگر_ختم
}

#اگر defined(FEAT_CLIPBOARD)
    خالی
free_رجسٹر(خالی *reg)
{
    yankreg_T tmp;

    tmp = *y_current;
    *y_current = *(yankreg_T *)reg;
    free_yank_all();
    vim_free(reg);
    *y_current = tmp;
}
#اگر_ختم

/*
 * return TRUE if the current yank register has type MLINE
 */
    صحیح
yank_رجسٹر_mline(صحیح regname)
{
    اگر (regname != 0 && !valid_yank_reg(regname, FALSE))
	واپس FALSE;
    اگر (regname == '_')		// black hole is always empty
	واپس FALSE;
    get_yank_رجسٹر(regname, FALSE);
    واپس (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
do_ریکارڈ(صحیح c)
{
    char_u	    *p;
    ساکن صحیح	    regname;
    yankreg_T	    *old_y_پچھلا, *old_y_current;
    صحیح		    retval;

    اگر (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	اگر (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	ورنہ
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    ورنہ			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	پیغام("");
	p = get_recorded();
	اگر (p == تہی)
	    retval = FAIL;
	ورنہ
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
    واپس retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    ساکن صحیح
stuff_yank(صحیح regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    اگر (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	واپس FAIL;
    }
    اگر (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	واپس OK;
    }

    plen = STRLEN(p);
    get_yank_رجسٹر(regname, TRUE);
    اگر (y_append && y_current->y_array != تہی)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_سائز - 1]);
	tmplen = pp->لمبائی + plen;
	tmp = alloc(tmplen + 1);
	اگر (tmp == تہی)
	{
	    vim_free(p);
	    واپس FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->لمبائی, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->لمبائی = tmplen;
    }
    ورنہ
    {
	free_yank_all();
	اگر ((y_current->y_array = ALLOC_ONE(string_T)) == تہی)
	{
	    vim_free(p);
	    واپس FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].لمبائی = plen;
	y_current->y_سائز = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#اگر_تعریف_شدہ FEAT_VIMINFO
	y_current->y_وقت_سیٹ = vim_وقت();
#اگر_ختم
    }
    واپس OK;
}

/*
 * Last executed register (@ command)
 */
ساکن صحیح execreg_lastc = NUL;

#اگر defined(FEAT_VIMINFO)
    صحیح
get_execreg_lastc(خالی)
{
    واپس execreg_lastc;
}

    خالی
سیٹ_execreg_lastc(صحیح lastc)
{
    execreg_lastc = lastc;
}
#اگر_ختم

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
    ساکن char_u *
execreg_سطر_continuation(string_T *سطریں, طویل *idx)
{
    garray_T	ga;
    طویل	cmd_آغاز = *idx;
    طویل	cmd_انتہا = *idx;
    string_T	*tmp;
    صحیح		j;
    char_u	*str;

    ga_init2(&ga, حجم(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    جب (--cmd_آغاز > 0)
    {
	char_u	*p;

	p = skipwhite(سطریں[cmd_آغاز].string);
	اگر (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    توڑو;
    }

    // join all the lines
    tmp = &سطریں[cmd_آغاز];
    ga_concat_len(&ga, tmp->string, tmp->لمبائی);
    برائے (j = cmd_آغاز + 1; j <= cmd_انتہا; j++)
    {
	char_u	*p;

	tmp = &سطریں[j];
	p = skipwhite(tmp->string);
	اگر (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    اگر (ga.ga_len > 400)
	    {
		اگر (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		ورنہ
		    ga.ga_growsize = ga.ga_len;
	    }
	    p++;
	    ga_concat_len(&ga, p, (tmp->string + tmp->لمبائی) - p);
	}
    }
    ga_append(&ga, NUL);
    str = vim_strnsave(ga.ga_ڈیٹا, ga.ga_len);
    ga_clear(&ga);

    *idx = cmd_آغاز;
    واپس str;
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
    طویل	i;
    char_u	*p;
    صحیح		retval = OK;
    صحیح		remap;

    // repeat previous one
    اگر (regname == '@')
    {
	اگر (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_مستعمل_رجسٹر));
	    واپس FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    اگر (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	واپس FAIL;
    }
    execreg_lastc = regname;

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#اگر_ختم

    // black hole: don't stuff anything
    اگر (regname == '_')
	واپس OK;

    // use last command line
    اگر (regname == ':')
    {
	اگر (آخری_cmdline == تہی)
	{
	    emsg(_(e_no_پچھلا_command_سطر));
	    واپس FAIL;
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
	اگر (p != تہی)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    اگر (VIsual_فعال && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_اندر_typebuf(p + 5, TRUE, TRUE, silent);
	    ورنہ
		retval = put_اندر_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#اگر_تعریف_شدہ FEAT_EVAL
    ورنہ اگر (regname == '=')
    {
	p = get_اظہاریہ_سطر();
	اگر (p == تہی)
	    واپس FAIL;
	retval = put_اندر_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#اگر_ختم
    ورنہ اگر (regname == '.')		// use last inserted text
    {
	p = get_آخری_داخل_کرو_save();
	اگر (p == تہی)
	{
	    emsg(_(e_no_inserted_متن_yet));
	    واپس FAIL;
	}
	retval = put_اندر_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    ورنہ
    {
	get_yank_رجسٹر(regname, FALSE);
	اگر (y_current->y_array == تہی)
	    واپس FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_اندر_typebuf(silent);
	برائے (i = y_current->y_سائز; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    صحیح	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    اگر (y_current->y_type == MLINE || i < y_current->y_سائز - 1
								     || addcr)
	    {
		اگر (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    واپس FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    اگر (colon && i > 0)
	    {
		p = skipwhite(str);
		اگر (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_سطر_continuation(y_current->y_array, &i);
		    اگر (str == تہی)
			واپس FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    اگر (free_str)
		vim_free(str);
	    اگر (escaped == تہی)
		واپس FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    اگر (retval == FAIL)
		واپس FAIL;
	    اگر (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		واپس FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_انتہا_reg_executing = FALSE;
    }
    واپس retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ساکن خالی
put_reedit_اندر_typebuf(صحیح silent)
{
    اگر (restart_edit == NUL)
	واپس;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    اگر (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    ورنہ اگر (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    ورنہ اگر (restart_edit == 'A')
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
    ساکن صحیح
put_اندر_typebuf(
    char_u	*s,
    صحیح		esc,
    صحیح		colon,	    // add ':' before the line
    صحیح		silent)
{
    صحیح		retval = OK;

    put_reedit_اندر_typebuf(silent);
    اگر (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    اگر (retval == OK)
    {
	char_u	*p;

	اگر (esc)
	    p = vim_strsave_escape_csi(s);
	ورنہ
	    p = s;
	اگر (p == تہی)
	    retval = FAIL;
	ورنہ
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	اگر (esc)
	    vim_free(p);
    }
    اگر (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    واپس retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
داخل_کرو_reg(
    صحیح		regname,
    صحیح		literally_arg)	// insert literally, not as if typed
{
    طویل	i;
    صحیح		retval = OK;
    char_u	*arg;
    صحیح		allocated;
    صحیح		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    اگر (got_int)
	واپس FAIL;

    // check for valid regname
    اگر (regname != NUL && !valid_yank_reg(regname, FALSE))
	واپس FAIL;

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#اگر_ختم

    اگر (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    ورنہ اگر (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	اگر (arg == تہی)
	    واپس FAIL;
	stuffescaped(arg, literally);
	اگر (allocated)
	    vim_free(arg);
    }
    ورنہ				// name or number register
    {
	اگر (get_yank_رجسٹر(regname, FALSE))
	    literally = TRUE;
	اگر (y_current->y_array == تہی)
	    retval = FAIL;
	ورنہ
	{
	    برائے (i = 0; i < y_current->y_سائز; ++i)
	    {
		اگر (regname == '-' && y_current->y_type == MCHAR)
		{
		    صحیح dir = BACKWARD;
		    اگر ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			اگر (u_save_cursor() == FAIL)
			    واپس FAIL;
			del_chars((طویل)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			اگر (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    Append_Char_مقصد_Redobuff(Ctrl_R);
		    Append_Char_مقصد_Redobuff(regname);
		    do_put(regname, تہی, dir, 1L, PUT_CURSEND);
		}
		ورنہ
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    اگر (y_current->y_type == MLINE || i < y_current->y_سائز - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    واپس retval;
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

    *argp = تہی;
    *allocated = FALSE;
    انتخاب (regname)
    {
	حالت '%':		// file name
	    اگر (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    واپس TRUE;

	حالت '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    واپس TRUE;

#اگر_تعریف_شدہ FEAT_EVAL
	حالت '=':		// result of expression
	    *argp = get_اظہاریہ_سطر();
	    *allocated = TRUE;
	    واپس TRUE;
#اگر_ختم

	حالت ':':		// last command line
	    اگر (آخری_cmdline == تہی && errmsg)
		emsg(_(e_no_پچھلا_command_سطر));
	    *argp = آخری_cmdline;
	    واپس TRUE;

	حالت '/':		// last search-pattern
	    اگر (آخری_تلاش_pat() == تہی && errmsg)
		emsg(_(e_no_پچھلا_regular_عبارت));
	    *argp = آخری_تلاش_pat();
	    واپس TRUE;

	حالت '.':		// last inserted text
	    *argp = get_آخری_داخل_کرو_save();
	    *allocated = TRUE;
	    اگر (*argp == تہی && errmsg)
		emsg(_(e_no_inserted_متن_yet));
	    واپس TRUE;

	حالت Ctrl_F:		// Filename under cursor
	حالت Ctrl_P:		// Path under cursor, expand via "path"
	    اگر (!errmsg)
		واپس FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, تہی);
	    *allocated = TRUE;
	    واپس TRUE;

	حالت Ctrl_W:		// word under cursor
	حالت Ctrl_A:		// WORD (mnemonic All) under cursor
	    اگر (!errmsg)
		واپس FALSE;
	    cnt = ڈھونڈو_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (ڈھونڈو_IDENT|ڈھونڈو_STRING) : ڈھونڈو_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : تہی;
	    *allocated = TRUE;
	    واپس TRUE;

	حالت Ctrl_L:		// Line under cursor
	    اگر (!errmsg)
		واپس FALSE;

	    *argp = ml_get_buf(curwin->w_بفر,
			curwin->w_cursor.lnum, FALSE);
	    واپس TRUE;

	حالت '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    واپس TRUE;
    }

    واپس FALSE;
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
    طویل	i;
    صحیح		literally = literally_arg;

    اگر (get_yank_رجسٹر(regname, FALSE))
	literally = TRUE;
    اگر (y_current->y_array == تہی)
	واپس FAIL;

    برائے (i = 0; i < y_current->y_سائز; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	اگر ((y_current->y_type == MLINE || i < y_current->y_سائز - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	اگر (got_int)
	    واپس FAIL;
    }
    واپس OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    خالی
سرکاؤ_delete_registers(خالی)
{
    صحیح		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    برائے (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    اگر (!y_append)
	y_پچھلا = y_current;
    y_regs[1].y_array = تہی;		// set register one to empty
}

#اگر defined(FEAT_EVAL)
    ساکن خالی
جوڑو_regtype_مقصد_dict(صحیح regname, dict_T *dict, char_u *buf, صحیح bufsize)
{
    size_t  buflen;
    طویل    reglen;
    // Register type
    انتخاب (get_reg_type(regname, &reglen))
    {
	حالت MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    توڑو;
	حالت MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    توڑو;
	حالت MBLOCK:
	    buflen = vim_snprintf_safelen((حرف *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    توڑو;
	طےشدہ:
	    buf[0] = NUL;
	    buflen = 0;
	    توڑو;
    }
    (خالی)dict_جوڑو_string_len(dict, "regtype", buf, (صحیح)buflen);
}

    خالی
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    ساکن صحیح	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    صحیح		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    اگر (recursive)
	واپس;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    اگر (list == تہی)
	واپس;

    // yanked text contents
    برائے (n = 0; n < reg->y_سائز; n++)
	list_append_string(list, reg->y_array[n].string, (صحیح)reg->y_array[n].لمبائی);
    list->lv_تالا = VAR_FIXED;
    اگر (dict_جوڑو_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (خالی)dict_جوڑو_string_len(v_event, "regname", buf, (صحیح)buflen);

    // motion type: inclusive or exclusive
    (خالی)dict_جوڑو_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (خالی)dict_جوڑو_string_len(v_event, "operator", buf, (صحیح)buflen);

    جوڑو_regtype_مقصد_dict(oap->regname, v_event, buf, حجم(buf));

    // selection type - visual or not
    (خالی)dict_جوڑو_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_سیٹ_اشیاء_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, تہی, تہی, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    ساکن خالی
put_do_autocmd(
	صحیح	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *داخل_کرو,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	صحیح	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    ساکن bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    صحیح		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    اگر (recursive || regname == '_')
	واپس;

    اگر (regname != '.' && داخل_کرو == تہی
	    && reg == تہی)
	// Can happen when pasting text in normal mode in a terminal buffer
	واپس;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    اگر (list == تہی)
	واپس;

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
	اگر (آخری_داخل_کرو_ga.ga_ڈیٹا != تہی)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, آخری_داخل_کرو_ga.ga_ڈیٹا,
		    (صحیح)آخری_داخل_کرو_ga.ga_len);
    }
    ورنہ اگر (داخل_کرو != تہی)
    {
	list_append_string(list, داخل_کرو->string, (صحیح)داخل_کرو->لمبائی);
    }
    ورنہ
    {
	برائے (n = 0; n < reg->y_سائز; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (صحیح)reg->y_array[n].لمبائی);
    }

    list->lv_تالا = VAR_FIXED;
    اگر (dict_جوڑو_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (خالی)dict_جوڑو_string_len(v_event, "regname", buf, (صحیح)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (خالی)dict_جوڑو_string_len(v_event, "operator", buf, (صحیح)buflen);

    جوڑو_regtype_مقصد_dict(regname, v_event, buf, حجم(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (خالی)dict_جوڑو_bool(v_event, "visual", VIsual_فعال);

    // Lock the dictionary and its keys
    dict_سیٹ_اشیاء_ro(v_event);

    recursive = true;
    textlock++;
    اگر (post)
	apply_autocmds(EVENT_TEXTPUTPOST, تہی, تہی, FALSE, curbuf);
    ورنہ
	apply_autocmds(EVENT_TEXTPUTPRE, تہی, تہی, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#اگر_ختم

/*
 * set all the yank registers to empty (called from main())
 */
    خالی
init_yank(خالی)
{
    صحیح		i;

    برائے (i = 0; i < عدد_REGISTERS; ++i)
	y_regs[i].y_array = تہی;
}

#اگر defined(EXITFREE)
    خالی
clear_registers(خالی)
{
    صحیح		i;

    برائے (i = 0; i < عدد_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	اگر (y_current->y_array != تہی)
	    free_yank_all();
    }
}
#اگر_ختم

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    ساکن خالی
free_yank(طویل n)
{
    اگر (y_current->y_array == تہی)
	واپس;

    طویل	    i;

    برائے (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    خالی
free_yank_all(خالی)
{
    free_yank(y_current->y_سائز);
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
    طویل		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    صحیح			yanktype = oap->motion_type;
    طویل		yanklines = oap->سطر_گنتی;
    linenr_T		yankendlnum = oap->انتہا.lnum;
    char_u		*pnew;
    ساخت بلاک_def	bd;

				    // check for read-only register
    اگر (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	واپس FAIL;
    }
    اگر (oap->regname == '_')	    // black hole: nothing to do
	واپس OK;

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    اگر ((!clip_ستارہ.دستیاب && oap->regname == '*') ||
	(!clip_plus.دستیاب && oap->regname == '+'))
    {
	oap->regname = 0;
	پیغام_warn_missing_clipboard();
    }
#اگر_ختم

    اگر (!deleting)		    // op_delete() already set y_current
	get_yank_رجسٹر(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    اگر (y_append && y_current->y_array != تہی)
	y_current = &newreg;
    ورنہ
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    اگر (       oap->motion_type == MCHAR
	    && oap->آغاز.ستون == 0
	    && !oap->inclusive
	    && (!oap->is_VIsual || *p_sel == 'o')
	    && !oap->بلاک_منوال
	    && oap->انتہا.ستون == 0
	    && yanklines > 1)
    {
	yanktype = MLINE;
	--yankendlnum;
	--yanklines;
    }

    y_current->y_سائز = yanklines;
    y_current->y_type = yanktype;   // set the yank register type
    y_current->y_چوڑائی = 0;
    y_current->y_array = lalloc_clear(حجم(string_T) * yanklines, TRUE);
    اگر (y_current->y_array == تہی)
    {
	y_current = curr;
	واپس FAIL;
    }
#اگر_تعریف_شدہ FEAT_VIMINFO
    y_current->y_وقت_سیٹ = vim_وقت();
#اگر_ختم

    y_idx = 0;
    lnum = oap->آغاز.lnum;

    اگر (oap->بلاک_منوال)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_چوڑائی = oap->انتہا_vcol - oap->آغاز_vcol;

	اگر (curwin->w_curswant == MAXCOL && y_current->y_چوڑائی > 0)
	    y_current->y_چوڑائی--;
    }

    برائے ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	انتخاب (y_current->y_type)
	{
	    حالت MBLOCK:
		بلاک_prep(oap, &bd, lnum, FALSE);
		اگر (yank_copy_سطر(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    جاؤ fail;
		توڑو;

	    حالت MLINE:
		y_current->y_array[y_idx].لمبائی = ml_get_len(lnum);
		اگر ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].لمبائی)) == تہی)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    جاؤ fail;
		}
		توڑو;

	    حالت MCHAR:
		{
		    صحیح tmp;

		    charwise_بلاک_prep(oap->آغاز, oap->انتہا, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (صحیح)STRLEN(bd.textstart);
		    اگر (tmp < bd.textlen)
			bd.textlen = tmp;

		    اگر (yank_copy_سطر(&bd, y_idx, FALSE) == FAIL)
			جاؤ fail;
		    توڑو;
		}
		// NOTREACHED
	}
    }

    اگر (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	طویل j;

	new_ptr = ALLOC_MULT(string_T, curr->y_سائز + y_current->y_سائز);
	اگر (new_ptr == تہی)
	    جاؤ fail;
	برائے (j = 0; j < curr->y_سائز; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#اگر_تعریف_شدہ FEAT_VIMINFO
	curr->y_وقت_سیٹ = vim_وقت();
#اگر_ختم

	اگر (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	اگر (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == تہی)
	{
	    pnew = alloc(curr->y_array[curr->y_سائز - 1].لمبائی + y_current->y_array[0].لمبائی + 1);
	    اگر (pnew == تہی)
	    {
		y_idx = y_current->y_سائز - 1;
		جاؤ fail;
	    }

	    --j;
	    STRCPY(pnew, curr->y_array[j].string);
	    STRCPY(pnew + curr->y_array[j].لمبائی, y_current->y_array[0].string);
	    vim_free(curr->y_array[j].string);
	    curr->y_array[j].string = pnew;
	    curr->y_array[j].لمبائی = curr->y_array[j].لمبائی + y_current->y_array[0].لمبائی;
	    ++j;
	    VIM_CLEAR_STRING(y_current->y_array[0]);
	    y_idx = 1;
	}
	ورنہ
	    y_idx = 0;
	جب (y_idx < y_current->y_سائز)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_سائز = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    اگر (mess)			// Display message about yank?
    {
	اگر (yanktype == MCHAR
		&& !oap->بلاک_منوال
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	اگر (yanklines > p_report)
	{
	    حرف namebuf[100];

	    اگر (oap->regname == NUL)
		*namebuf = NUL;
	    ورنہ
		vim_snprintf(namebuf, حجم(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    اگر (oap->بلاک_منوال)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    ورنہ
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
	curbuf->b_op_آغاز = oap->آغاز;
	curbuf->b_op_انتہا = oap->انتہا;
	اگر (yanktype == MLINE && !oap->بلاک_منوال)
	{
	    curbuf->b_op_آغاز.ستون = 0;
	    curbuf->b_op_انتہا.ستون = MAXCOL;
	}
	اگر (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_انتہا);
    }

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    اگر (curr == &y_regs[REAL_PLUS_رجسٹر] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	اگر (curr != &(y_regs[REAL_PLUS_رجسٹر]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_رجسٹر]));
	call_clip_provider_سیٹ('+');
    }
    اگر (curr == &y_regs[ستارہ_رجسٹر] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	اگر (curr != &(y_regs[ستارہ_رجسٹر]))
	    copy_yank_reg(&(y_regs[ستارہ_رجسٹر]));
	call_clip_provider_سیٹ('*');
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	اگر (clip_ستارہ.دستیاب
		&& (curr == &(y_regs[ستارہ_رجسٹر])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    اگر (curr != &(y_regs[ستارہ_رجسٹر]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[ستارہ_رجسٹر]));

	    clip_own_selection(&clip_ستارہ);
	    clip_gen_سیٹ_selection(&clip_ستارہ);
	}

# اگر defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	اگر (clip_plus.دستیاب
		&& (curr == &(y_regs[PLUS_رجسٹر])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    اگر (curr != &(y_regs[PLUS_رجسٹر]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_رجسٹر]));

	    clip_own_selection(&clip_plus);
	    clip_gen_سیٹ_selection(&clip_plus);
	}
# endif
    }
#اگر_ختم

#اگر defined(FEAT_EVAL)
    اگر (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#اگر_ختم
    واپس OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    واپس FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    ساکن صحیح
yank_copy_سطر(ساخت بلاک_def *bd, طویل y_idx, صحیح exclude_trailing_space)
{
    char_u	*pnew;

    اگر (exclude_trailing_space)
	bd->endspaces = 0;
    اگر ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == تہی)
	واپس FAIL;
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

	جب (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_سر_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].لمبائی = (size_t)(pnew - y_current->y_array[y_idx].string);

    واپس OK;
}

#اگر_تعریف_شدہ HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    ساکن خالی
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    طویل	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    حجم(string_T) * y_current->y_سائز, TRUE);
    اگر (y_current->y_array == تہی)
	y_current->y_سائز = 0;
    ورنہ
	برائے (j = 0; j < y_current->y_سائز; ++j)
	{
	    اگر ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].لمبائی)) == تہی)
	    {
		free_yank(j);
		y_current->y_سائز = 0;
		توڑو;
	    }
	    y_current->y_array[j].لمبائی = curr->y_array[j].لمبائی;
	}
    y_current = curr;
}
#اگر_ختم

/*
 * Put contents of register "regname" into the text.
 * Caller must check "regname" to be valid!
 * "flags": PUT_FIXINDENT	make indent look nice
 *	    PUT_CURSEND		leave cursor after end of new text
 *	    PUT_LINE		force linewise put (":put")
 *	    PUT_BLOCK_INNER     in block mode, do not add trailing spaces
 */
    خالی
do_put(
    صحیح		regname,
    char_u	*اظہاریہ_نتیجہ,	// result for regname "=" when compiled
    صحیح		dir,		// BACKWARD for 'P', FORWARD for 'p'
    طویل	گنتی,
    صحیح		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    صحیح		yanklen;
    صحیح		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	ستون;
    طویل	i;			// index in y_array[]
    صحیح		y_type;
    طویل	y_سائز;
    صحیح		oldlen;
    طویل	y_چوڑائی = 0;
    colnr_T	vcol;
    string_T	*y_array = تہی;
    yankreg_T	*y_current_مستعمل = تہی;
    طویل	nr_سطریں = 0;
    string_T	داخل_کرو_string;
    صحیح		allocated = FALSE;
    pos_T	orig_آغاز = curbuf->b_op_آغاز;
    pos_T	orig_انتہا = curbuf->b_op_انتہا;
    غیرعلامتی صحیح cur_ve_flags = get_ve_flags();

#اگر_تعریف_شدہ HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(خالی)may_get_selection(regname);
#اگر_ختم

    // Remove any preinserted text (issue #19329)
    اگر (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_آغاز = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_انتہا = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    اگر (regname == '.')
    {
	اگر (VIsual_فعال)
	    stuffcharReadbuff(VIsual_منوال);

#اگر_تعریف_شدہ FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	اگر (has_textput_events)
	    جوڑو_آخری_داخل_کرو++;
#اگر_ختم

	(خالی)stuff_inserted((dir == FORWARD ? (گنتی == -1 ? 'o' : 'a') :
				    (گنتی == -1 ? 'O' : 'i')), گنتی, FALSE);

#اگر_تعریف_شدہ FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	اگر (has_textputpre())
	    put_do_autocmd('.', تہی, تہی, false, dir);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (has_textputpost())
	    put_do_autocmd('.', تہی, تہی, true, dir);

	اگر (has_textput_events && --جوڑو_آخری_داخل_کرو == 0)
	    ga_clear(&آخری_داخل_کرو_ga);
#اگر_ختم

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	اگر ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	واپس;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    داخل_کرو_string.string = تہی;
    اگر (regname == '=' && اظہاریہ_نتیجہ != تہی)
	داخل_کرو_string.string = اظہاریہ_نتیجہ;
    ورنہ اگر (get_spec_reg(regname, &داخل_کرو_string.string, &allocated, TRUE)
		&& داخل_کرو_string.string == تہی)
    {
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#اگر_ختم
	واپس;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    اگر (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	جاؤ انتہا;

    اگر (داخل_کرو_string.string != تہی)
    {
	داخل_کرو_string.لمبائی = STRLEN(داخل_کرو_string.string);

	y_type = MCHAR;
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    برائے (;;)
	    {
		y_سائز = 0;
		ptr = داخل_کرو_string.string;
		ptrlen = داخل_کرو_string.لمبائی;
		جب (ptr != تہی)
		{
		    اگر (y_array != تہی)
			y_array[y_سائز].string = ptr;
		    ++y_سائز;
		    tmp = vim_strchr(ptr, '\n');
		    اگر (tmp == تہی)
		    {
			اگر (y_array != تہی)
			    y_array[y_سائز - 1].لمبائی = ptrlen;
		    }
		    ورنہ
		    {
			اگر (y_array != تہی)
			{
			    *tmp = NUL;
			    y_array[y_سائز - 1].لمبائی = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_سائز - 1].لمبائی + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			اگر (*tmp == NUL)
			{
			    y_type = MLINE;
			    توڑو;
			}
		    }
		    ptr = tmp;
		}
		اگر (y_array != تہی)
		    توڑو;
		y_array = ALLOC_MULT(string_T, y_سائز);
		اگر (y_array == تہی)
		    جاؤ انتہا;
	    }
	}
	ورنہ
#اگر_ختم
	{
	    y_سائز = 1;		// use fake one-line yank register
	    y_array = &داخل_کرو_string;
	}
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (has_textputpre())
	    put_do_autocmd(regname, تہی, &داخل_کرو_string, false, dir);
#اگر_ختم
    }
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_رجسٹر(regname, FALSE);
	    put_do_autocmd(regname, y_current, تہی, false, dir);
	}
#اگر_ختم
	get_yank_رجسٹر(regname, FALSE);

	y_type = y_current->y_type;
	y_چوڑائی = y_current->y_چوڑائی;
	y_سائز = y_current->y_سائز;
	y_array = y_current->y_array;
	y_current_مستعمل = y_current;
    }

    اگر (y_type == MLINE)
    {
	اگر (flags & PUT_سطر_بانٹو)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    اگر (u_save_cursor() == FAIL)
		جاؤ انتہا;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    اگر (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    اگر (ptr == تہی)
		جاؤ انتہا;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.ستون;
	    اگر (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    اگر (ptr == تہی)
		جاؤ انتہا;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_سطریں;
	    dir = FORWARD;
	}
	اگر (flags & PUT_سطر_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_انتہا;
	    dir = FORWARD;
	}
	curbuf->b_op_آغاز = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_انتہا = curwin->w_cursor;	// default for '] mark
    }

    اگر (flags & PUT_سطر)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    اگر (y_سائز == 0 || y_array == تہی)
    {
	semsg(_(e_nothing_اندر_رجسٹر_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	جاؤ انتہا;
    }

    اگر (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_سائز + 1;
	اگر (lnum > curbuf->b_ml.ml_سطر_گنتی)
	    lnum = curbuf->b_ml.ml_سطر_گنتی + 1;
	اگر (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    جاؤ انتہا;
    }
    ورنہ اگر (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#اگر_تعریف_شدہ FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	اگر (dir == BACKWARD)
	    (خالی)hasFolding(lnum, &lnum, تہی);
	ورنہ
	    (خالی)hasFolding(lnum, تہی, &lnum);
#اگر_ختم
	اگر (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	اگر ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    جاؤ انتہا;
#اگر_تعریف_شدہ FEAT_FOLDING
	اگر (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	ورنہ
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_آغاز = curwin->w_cursor;	// for mark_adjust()
#اگر_ختم
    }
    ورنہ اگر (u_save_cursor() == FAIL)
	جاؤ انتہا;

    اگر (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	اگر (gchar_cursor() == TAB)
	{
	    صحیح viscol = getviscol();
	    صحیح ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    اگر (dir == FORWARD ?
#اگر_تعریف_شدہ FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#ورنہ
		    ts - (viscol % ts) != 1
#اگر_ختم
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    ورنہ
		curwin->w_cursor.coladd = 0;
	}
	ورنہ اگر (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    ستون = curwin->w_cursor.ستون;

    // Block mode
    اگر (y_type == MBLOCK)
    {
	صحیح	delcount;
	صحیح	incr = 0;
	ساخت بلاک_def bd;
	طویل	j;
	صحیح	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	اگر (dir == FORWARD && c != NUL)
	{
	    اگر (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &ستون, تہی, &endcol2, 0);
	    ورنہ
		getvcol(curwin, &curwin->w_cursor, تہی, تہی, &ستون, 0);

	    اگر (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.ستون += (*mb_ptr2len)(ml_get_cursor());
	    ورنہ
		اگر (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.ستون;
	    ++ستون;
	}
	ورنہ
	    getvcol(curwin, &curwin->w_cursor, &ستون, تہی, &endcol2, 0);

	ستون += curwin->w_cursor.coladd;
	اگر (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.ستون))
	{
	    اگر (dir == FORWARD && c == NUL)
		++ستون;
	    اگر (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.ستون;
	    اگر (c == TAB)
	    {
		اگر (dir == BACKWARD && curwin->w_cursor.ستون)
		    curwin->w_cursor.ستون--;
		اگر (dir == FORWARD && ستون - 1 == endcol2)
		    curwin->w_cursor.ستون++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	برائے (i = 0; i < y_سائز; ++i)
	{
	    صحیح		    spaces = 0;
	    حرف	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
	    {
		اگر (ml_append(curbuf->b_ml.ml_سطر_گنتی, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    توڑو;
		++nr_سطریں;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    جب (cts.cts_vcol < ستون && *cts.cts_ptr != NUL)
	    {
		// Count a tab for what it's worth (if list mode not on)
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    vcol = cts.cts_vcol;
	    ptr = cts.cts_ptr;
	    bd.textcol = (colnr_T)(ptr - oldp);
	    clear_chartabsize_arg(&cts);

	    shortline = (vcol < ستون) || (vcol == ستون && !*ptr) ;

	    اگر (vcol < ستون) // line too short, pad with spaces
		bd.startspaces = ستون - vcol;
	    ورنہ اگر (vcol > ستون)
	    {
		bd.endspaces = vcol - ستون;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		اگر (has_mbyte)
		    bd.textcol -= (*mb_سر_off)(oldp, oldp + bd.textcol);
		اگر (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (صحیح)y_array[i].لمبائی;

	    اگر ((flags & PUT_بلاک_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		//بلاک
		spaces = y_چوڑائی + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		جب (*cts.cts_ptr != NUL)
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
		     && گنتی > ((زیادہ_سے_زیادہ_عدد - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_متن_too_long));
		توڑو;
	    }

	    totlen = گنتی * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    اگر (newp == تہی)
		توڑو;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    برائے (j = 0; j < گنتی; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		اگر ((j < گنتی - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		ورنہ
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
		curwin->w_cursor.ستون += bd.startspaces;
	}

	بدلا_سطریں(lnum, 0, curwin->w_cursor.lnum - nr_سطریں, nr_سطریں);

	// Set '[ mark.
	curbuf->b_op_آغاز = curwin->w_cursor;
	curbuf->b_op_آغاز.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_انتہا.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_انتہا.ستون = bd.textcol + totlen - 1;
	اگر (curbuf->b_op_انتہا.ستون < 0)
	    curbuf->b_op_انتہا.ستون = 0;
	curbuf->b_op_انتہا.coladd = 0;
	اگر (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_انتہا;
	    curwin->w_cursor.ستون++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    اگر (curwin->w_cursor.ستون > len)
		curwin->w_cursor.ستون = len;
	}
	ورنہ
	    curwin->w_cursor.lnum = lnum;
    }
    ورنہ
    {
	pos_T	new_cursor;

	yanklen = (صحیح)y_array[0].لمبائی;

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
		    ستون += bytelen;
		    اگر (yanklen)
		    {
			curwin->w_cursor.ستون += bytelen;
			curbuf->b_op_انتہا.ستون += bytelen;
		    }
		}
		ورنہ
		{
		    ++ستون;
		    اگر (yanklen)
		    {
			++curwin->w_cursor.ستون;
			++curbuf->b_op_انتہا.ستون;
		    }
		}
	    }
	    curbuf->b_op_آغاز = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	ورنہ اگر (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	اگر (y_type == MCHAR && y_سائز == 1)
	{
	    linenr_T	انتہا_lnum = 0; // init for gcc
	    linenr_T	آغاز_lnum = lnum;
	    صحیح		پہلا_byte_off = 0;

	    اگر (VIsual_فعال)
	    {
		انتہا_lnum = curbuf->b_visual.vi_انتہا.lnum;
		اگر (انتہا_lnum < curbuf->b_visual.vi_آغاز.lnum)
		    انتہا_lnum = curbuf->b_visual.vi_آغاز.lnum;
		اگر (انتہا_lnum > آغاز_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.ستون = ستون;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, تہی, &vcol, تہی, 0);
		}
	    }

	    اگر (گنتی == 0 || yanklen == 0)
	    {
		اگر (VIsual_فعال)
		    lnum = انتہا_lnum;
	    }
	    ورنہ اگر (گنتی > زیادہ_سے_زیادہ_عدد / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_متن_too_long));
	    ورنہ
	    {
		totlen = گنتی * yanklen;
		کرو
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    اگر (lnum > آغاز_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			اگر (getvpos(&pos, vcol) == OK)
			    ستون = pos.ستون;
			ورنہ
			    ستون = MAXCOL;
		    }
		    اگر (VIsual_فعال && ستون > oldlen)
		    {
			lnum++;
			جاری;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    اگر (newp == تہی)
			جاؤ انتہا;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)ستون);
		    ptr = newp + ستون;
		    برائے (i = 0; i < گنتی; ++i)
		    {
			mch_memmove(ptr, y_array[0].string, (size_t)yanklen);
			ptr += yanklen;
		    }
		    mch_memmove(ptr, oldp + ستون, (size_t)(oldlen - ستون) + 1);	    // +1 for NUL

		    // compute the byte offset for the last character
		    پہلا_byte_off = mb_سر_off(newp, ptr - 1);

		    // Note: this may free "newp"
		    ml_replace(lnum, newp, FALSE);

		    inserted_بائٹیں(lnum, ستون, totlen);

		    // Place cursor on last putted char.
		    اگر (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			بدلا_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.ستون += (colnr_T)(totlen - 1);
		    }
		    اگر (VIsual_فعال)
			lnum++;
		} جب (VIsual_فعال && lnum <= انتہا_lnum);

		اگر (VIsual_فعال) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_انتہا = curwin->w_cursor;
	    curbuf->b_op_انتہا.ستون -= پہلا_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    اگر (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.ستون;
	    ورنہ
		curwin->w_cursor.ستون -= پہلا_byte_off;
	}
	ورنہ
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    صحیح		indent;
	    صحیح		orig_indent = 0;
	    صحیح		indent_diff = 0;	// init for gcc
	    صحیح		پہلا_indent = TRUE;
	    صحیح		lendiff = 0;
	    طویل	cnt;

	    اگر (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    برائے (cnt = 1; cnt <= گنتی; ++cnt)
	    {
		i = 0;
		اگر (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + ستون;
		    totlen = (صحیح)y_array[y_سائز - 1].لمبائی;
		    newp = alloc(ml_get_len(lnum) - ستون + totlen + 1);
		    اگر (newp == تہی)
			جاؤ error;
		    STRCPY(newp, y_array[y_سائز - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(ستون + yanklen + 1);
		    اگر (newp == تہی)
			جاؤ error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)ستون);
					    // append to first line
		    mch_memmove(newp + ستون, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		برائے (; i < y_سائز; ++i)
		{
		    اگر (y_type != MCHAR || i < y_سائز - 1)
		    {
			اگر (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    جاؤ error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_سطریں;
		    اگر (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			اگر (cnt == گنتی && i == y_سائز - 1)
			    lendiff = ml_get_len(lnum);
			اگر (*ptr == '#' && preprocs_بایاں())
			    indent = 0;     // Leave # lines at start
			ورنہ اگر (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			ورنہ اگر (پہلا_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    پہلا_indent = FALSE;
			}
			ورنہ اگر ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(خالی)سیٹ_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			اگر (cnt == گنتی && i == y_سائز - 1)
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
		curbuf->b_op_آغاز.ستون = 0;
		اگر (dir == FORWARD)
		    curbuf->b_op_آغاز.lnum++;
	    }
	    mark_adjust(curbuf->b_op_آغاز.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_سطریں, 0L);

	    // note changed text for displaying and folding
	    اگر (y_type == MCHAR)
		بدلا_سطریں(curwin->w_cursor.lnum, ستون,
					 curwin->w_cursor.lnum + 1, nr_سطریں);
	    ورنہ
		بدلا_سطریں(curbuf->b_op_آغاز.lnum, 0,
					   curbuf->b_op_آغاز.lnum, nr_سطریں);
	    اگر (y_current_مستعمل != تہی && (y_current_مستعمل != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_رجسٹر_بدلا_while_using_it));
		جاؤ انتہا;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_انتہا.lnum = new_lnum;
	    ستون = MAX(0, (colnr_T)y_array[y_سائز - 1].لمبائی - lendiff);
	    اگر (ستون > 1)
	    {
		curbuf->b_op_انتہا.ستون = ستون - 1;
		اگر (y_array[y_سائز - 1].لمبائی > 0)
		    curbuf->b_op_انتہا.ستون -= mb_سر_off(y_array[y_سائز - 1].string,
						y_array[y_سائز - 1].string + y_array[y_سائز - 1].لمبائی - 1);
	    }
	    ورنہ
		curbuf->b_op_انتہا.ستون = 0;

	    اگر (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ورنہ اگر (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		اگر (y_type == MLINE)
		{
		    اگر (lnum >= curbuf->b_ml.ml_سطر_گنتی)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
		    ورنہ
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.ستون = 0;
		}
		ورنہ
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.ستون = ستون;
		    curbuf->b_op_انتہا = curwin->w_cursor;
		    اگر (ستون > 1)
			curbuf->b_op_انتہا.ستون = ستون - 1;
		}
	    }
	    ورنہ اگر (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.ستون = 0;
		اگر (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ورنہ	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_سطریں);
    curwin->w_سیٹ_curswant = true;

    // Make sure the cursor is not after the NUL.
    صحیح len = ml_get_curline_len();
    اگر (curwin->w_cursor.ستون > len)
    {
	اگر (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.ستون - len;
	curwin->w_cursor.ستون = len;
    }

انتہا:
    اگر (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_آغاز = orig_آغاز;
	curbuf->b_op_انتہا = orig_انتہا;
    }

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (has_textputpost())
    {
	اگر (داخل_کرو_string.string == تہی)
	    put_do_autocmd(regname, y_current, تہی, true, dir);
	ورنہ
	    put_do_autocmd(regname, تہی, &داخل_کرو_string, true, dir);
    }
#اگر_ختم

    اگر (allocated)
	vim_free(داخل_کرو_string.string);
    اگر (regname == '=')
	vim_free(y_array);

    VIsual_فعال = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    صحیح
get_رجسٹر_name(صحیح عدد)
{
    اگر (عدد == -1)
	واپس '"';
    ورنہ اگر (عدد < 10)
	واپس عدد + '0';
    ورنہ اگر (عدد == DELETION_رجسٹر)
	واپس '-';
#اگر defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    ورنہ اگر (عدد == ستارہ_رجسٹر)
	واپس '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    ورنہ اگر (clipmethod == CLIPMETHOD_PROVIDER && عدد == REAL_PLUS_رجسٹر)
	واپس '+';
    ورنہ اگر (clipmethod != CLIPMETHOD_PROVIDER && عدد == PLUS_رجسٹر)
	واپس '+';
#اگر_ختم
    ورنہ
	واپس عدد + 'a' - 10;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    صحیح
get_unname_رجسٹر(خالی)
{
    واپس y_پچھلا == تہی ? -1 : y_پچھلا - &y_regs[0];
}
#اگر_ختم

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    خالی
ex_display(exarg_T *eap)
{
    صحیح		i, n;
    طویل	j;
    char_u	*p;
    yankreg_T	*yb;
    صحیح		name;
    صحیح		attr;
    char_u	*arg = eap->arg;
    صحیح		clen;
    صحیح		type;
    string_T	داخل_کرو;

    silence_w23_w24_پیغام++;
    اگر (arg != تہی && *arg == NUL)
	arg = تہی;
    attr = HL_ATTR(HLF_8);

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#اگر_ختم

    // Highlight title
    پیغام_puts_title(_("\nType Name Content"));
    برائے (i = -1; i < عدد_REGISTERS && !got_int; ++i)
    {
	name = get_رجسٹر_name(i);
	انتخاب (get_reg_type(name, تہی))
	{
	    حالت MLINE: type = 'l'; توڑو;
	    حالت MCHAR: type = 'c'; توڑو;
	    طےشدہ:	type = 'b'; توڑو;
	}
	اگر (arg != تہی && vim_strchr(arg, name) == تہی
#اگر_تعریف_شدہ ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == تہی)
#اگر_ختم
		)
	    جاری;	    // did not ask for this register

#اگر_تعریف_شدہ HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	اگر (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (خالی)may_get_selection(name);
#اگر_ختم

	اگر (i == -1)
	{
	    اگر (y_پچھلا != تہی)
		yb = y_پچھلا;
	    ورنہ
		yb = &(y_regs[0]);
	}
	ورنہ
	    yb = &(y_regs[i]);

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != تہی &&
		    (yb == y_پچھلا || yb == &y_regs[0])))
	    جاری;	    // do not list register being written to, the
			    // pointer can be freed
#اگر_ختم

	اگر (yb->y_array != تہی)
	{
	    صحیح do_show = FALSE;

	    برائے (j = 0; !do_show && j < yb->y_سائز; ++j)
		do_show = !پیغام_filtered(yb->y_array[j].string);

	    اگر (do_show || yb->y_سائز == 0)
	    {
		پیغام_putchar('\n');
		پیغام_puts("  ");
		پیغام_putchar(type);
		پیغام_puts("  ");
		پیغام_putchar('"');
		پیغام_putchar(name);
		پیغام_puts("   ");

		n = (صحیح)Columns - 11;
		برائے (j = 0; j < yb->y_سائز && n > 1; ++j)
		{
		    اگر (j)
		    {
			پیغام_puts_attr("^J", attr);
			n -= 2;
		    }
		    برائے (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			پیغام_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		اگر (n > 1 && yb->y_type == MLINE)
		    پیغام_puts_attr("^J", attr);
		برآمد_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    داخل_کرو = get_آخری_داخل_کرو();
    اگر ((p = داخل_کرو.string) != تہی
		  && (arg == تہی || vim_strchr(arg, '.') != تہی) && !got_int
						      && !پیغام_filtered(p))
    {
	پیغام_puts("\n  c  \".   ");
	dis_پیغام(p, TRUE);
    }

    // display last command line
    اگر (آخری_cmdline != تہی && (arg == تہی || vim_strchr(arg, ':') != تہی)
			       && !got_int && !پیغام_filtered(آخری_cmdline))
    {
	پیغام_puts("\n  c  \":   ");
	dis_پیغام(آخری_cmdline, FALSE);
    }

    // display current file name
    اگر (curbuf->b_fname != تہی
	    && (arg == تہی || vim_strchr(arg, '%') != تہی) && !got_int
					&& !پیغام_filtered(curbuf->b_fname))
    {
	پیغام_puts("\n  c  \"%   ");
	dis_پیغام(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    اگر ((arg == تہی || vim_strchr(arg, '#') != تہی) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	اگر (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !پیغام_filtered(fname))
	{
	    پیغام_puts("\n  c  \"#   ");
	    dis_پیغام(fname, FALSE);
	}
    }

    // display last search pattern
    اگر (آخری_تلاش_pat() != تہی
		 && (arg == تہی || vim_strchr(arg, '/') != تہی) && !got_int
				      && !پیغام_filtered(آخری_تلاش_pat()))
    {
	پیغام_puts("\n  c  \"/   ");
	dis_پیغام(آخری_تلاش_pat(), FALSE);
    }

#اگر_تعریف_شدہ FEAT_EVAL
    // display last used expression
    اگر (اظہاریہ_سطر != تہی && (arg == تہی || vim_strchr(arg, '=') != تہی)
				  && !got_int && !پیغام_filtered(اظہاریہ_سطر))
    {
	پیغام_puts("\n  c  \"=   ");
	dis_پیغام(اظہاریہ_سطر, FALSE);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#اگر_ختم
    silence_w23_w24_پیغام--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    ساکن خالی
dis_پیغام(
    char_u	*p,
    صحیح		skip_esc)	    // if TRUE, ignore trailing ESC
{
    صحیح		n;
    صحیح		l;

    n = (صحیح)Columns - 6;
    جب (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    پیغام_outtrans_len(p, l);
	    p += l;
	}
	ورنہ
	    پیغام_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#اگر defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    خالی
dnd_yank_drag_ڈیٹا(char_u *str, طویل len)
{
    yankreg_T *curr;

    curr = y_current;
    y_current = &y_regs[TILDE_رجسٹر];
    free_yank_all();
    str_مقصد_reg(y_current, MCHAR, str, len, 0L, FALSE);
    y_current = curr;
}
#اگر_ختم


/*
 * Return the type of a register.
 * Used for getregtype()
 * Returns MAUTO for error.
 */
    char_u
get_reg_type(صحیح regname, طویل *reglen)
{
    انتخاب (regname)
    {
	حالت '%':		// file name
	حالت '#':		// alternate file name
	حالت '=':		//عبارت
	حالت ':':		// last command line
	حالت '/':		// last search-pattern
	حالت '.':		// last inserted text
	حالت Ctrl_F:		// Filename under cursor
	حالت Ctrl_P:		// Path under cursor, expand via "path"
	حالت Ctrl_W:		// word under cursor
	حالت Ctrl_A:		// WORD (mnemonic All) under cursor
	حالت '_':		// black hole: always empty
	    واپس MCHAR;
    }

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#اگر_ختم

    اگر (regname != NUL && !valid_yank_reg(regname, FALSE))
	واپس MAUTO;

    get_yank_رجسٹر(regname, FALSE);

    اگر (y_current->y_array != تہی)
    {
	اگر (reglen != تہی && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_چوڑائی;
	واپس y_current->y_type;
    }
    واپس MAUTO;
}

#اگر defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    ساکن char_u *
getreg_wrap_one_سطر(char_u *s, صحیح flags)
{
    اگر (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	اگر (list != تہی)
	{
	    اگر (list_append_string(list, تہی, -1) == FAIL)
	    {
		list_free(list);
		واپس تہی;
	    }
	    list->lv_پہلا->li_tv.vval.v_string = s;
	}
	واپس (char_u *)list;
    }
    واپس s;
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
    طویل	len;

    // Don't allow using an expression register inside an expression
    اگر (regname == '=')
    {
	اگر (flags & GREG_NO_اظہاریہ)
	    واپس تہی;
	اگر (flags & GREG_اظہاریہ_SRC)
	    واپس getreg_wrap_one_سطر(get_اظہاریہ_سطر_src(), flags);
	واپس getreg_wrap_one_سطر(get_اظہاریہ_سطر(), flags);
    }

    اگر (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    اگر (regname != NUL && !valid_yank_reg(regname, FALSE))
	واپس تہی;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    اگر (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	اگر (retval == تہی)
	    واپس تہی;
	اگر (allocated)
	    واپس getreg_wrap_one_سطر(retval, flags);
	واپس getreg_wrap_one_سطر(vim_strsave(retval), flags);
    }

    get_yank_رجسٹر(regname, FALSE);
    اگر (y_current->y_array == تہی)
	واپس تہی;

    اگر (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	اگر (list == تہی)
	    واپس تہی;
	برائے (i = 0; i < y_current->y_سائز; ++i)
	{
	    اگر (list_append_string(list, y_current->y_array[i].string,
		(صحیح)y_current->y_array[i].لمبائی) == FAIL)
	    {
		list_free(list);
		واپس تہی;
	    }
	}
	واپس (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    برائے (i = 0; i < y_current->y_سائز; ++i)
    {
	len += (طویل)y_current->y_array[i].لمبائی;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	اگر (y_current->y_type == MLINE || i < y_current->y_سائز - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    اگر (retval == تہی)
	واپس تہی;

    // Copy the lines of the yank register into the string.
    len = 0;
    برائے (i = 0; i < y_current->y_سائز; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (طویل)y_current->y_array[i].لمبائی;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	اگر (y_current->y_type == MLINE || i < y_current->y_سائز - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    واپس retval;
}

    ساکن صحیح
init_write_reg(
    صحیح		name,
    yankreg_T	**old_y_پچھلا,
    yankreg_T	**old_y_current,
    صحیح		must_append,
    صحیح		*yank_type غیر_مستعمل)
{
    اگر (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	واپس FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_پچھلا = y_پچھلا;
    *old_y_current = y_current;

    get_yank_رجسٹر(name, TRUE);
    اگر (!y_append && !must_append)
	free_yank_all();
    واپس OK;
}

    ساکن خالی
اختتام_write_reg(
    صحیح		name,
    yankreg_T	*old_y_پچھلا,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_سیٹ_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    اگر (name != '"')
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
    خالی
write_reg_contents(
    صحیح		name,
    char_u	*str,
    صحیح		maxlen,
    صحیح		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    خالی
write_reg_contents_lst(
    صحیح		name,
    char_u	**لڑیاں,
    صحیح		maxlen غیر_مستعمل,
    صحیح		must_append,
    صحیح		yank_type,
    طویل	بلاک_len)
{
    yankreg_T  *old_y_پچھلا, *old_y_current;

    اگر (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	اگر (لڑیاں[0] == تہی)
	    s = (char_u *)"";
	ورنہ اگر (لڑیاں[1] != تہی)
	{
	    semsg(_(e_رجسٹر_char_cannot_contain_مضرب_سطریں), name);
	    واپس;
	}
	ورنہ
	    s = لڑیاں[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, بلاک_len);
	واپس;
    }

    اگر (name == '_')	    // black hole: nothing to do
	واپس;

    اگر (init_write_reg(name, &old_y_پچھلا, &old_y_current, must_append,
		&yank_type) == FAIL)
	واپس;

    str_مقصد_reg(y_current, yank_type, (char_u *)لڑیاں, -1, بلاک_len, TRUE);

    اختتام_write_reg(name, old_y_پچھلا, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_سیٹ(name);
# endif
}

    خالی
write_reg_contents_ex(
    صحیح		name,
    char_u	*str,
    صحیح		maxlen,
    صحیح		must_append,
    صحیح		yank_type,
    طویل	بلاک_len)
{
    yankreg_T	*old_y_پچھلا, *old_y_current;
    طویل	len;

    اگر (maxlen >= 0)
	len = maxlen;
    ورنہ
	len = (طویل)STRLEN(str);

    // Special case: '/' search pattern
    اگر (name == '/')
    {
	سیٹ_آخری_تلاش_pat(str, RE_تلاش, TRUE, TRUE);
	واپس;
    }

    اگر (name == '#')
    {
	اگر (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  واپس;
	}

	buf_T	*buf;

	اگر (VIM_ISDIGIT(*str))
	{
	    صحیح	عدد = atoi((حرف *)str);

	    buf = buflist_findnr(عدد);
	    اگر (buf == تہی)
		semsg(_(e_بفر_nr_does_not_exist), (طویل)عدد);
	}
	ورنہ
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	اگر (buf == تہی)
	    واپس;
	curwin->w_alt_fnum = buf->b_fnum;
	واپس;
    }

    اگر (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	اگر (p == تہی)
	    واپس;
	اگر (must_append && اظہاریہ_سطر != تہی)
	{
	    s = concat_str(اظہاریہ_سطر, p);
	    vim_free(p);
	    p = s;
	}
	سیٹ_اظہاریہ_سطر(p, تہی);
	واپس;
    }

    اگر (name == '_')	    // black hole: nothing to do
	واپس;

    اگر (init_write_reg(name, &old_y_پچھلا, &old_y_current, must_append,
		&yank_type) == FAIL)
	واپس;

    str_مقصد_reg(y_current, yank_type, str, len, بلاک_len, FALSE);

    اختتام_write_reg(name, old_y_پچھلا, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_سیٹ(name);
# endif
}
#اگر_ختم	// FEAT_EVAL

#اگر defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    خالی
str_مقصد_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    صحیح		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    طویل	len,		// length of string
    طویل	blocklen,	// width of Visual block
    صحیح		str_list)	// TRUE if str is char_u **
{
    صحیح		type;			// MCHAR, MLINE or MBLOCK
    صحیح		lnum;
    طویل	آغاز;
    طویل	i;
    صحیح		extra;
    صحیح		newlines;		// number of lines added
    صحیح		extraline = 0;		// extra line at the end
    صحیح		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    طویل	maxlen;

    اگر (y_ptr->y_array == تہی)		// NULL means empty register
	y_ptr->y_سائز = 0;

    اگر (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    ورنہ
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    اگر (str_list)
    {
	برائے (ss = (char_u **) str; *ss != تہی; ++ss)
	    ++newlines;
    }
    ورنہ
    {
	برائے (i = 0; i < len; i++)
	    اگر (str[i] == '\n')
		++newlines;
	اگر (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	اگر (y_ptr->y_سائز > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    اگر (y_ptr->y_سائز + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	واپس;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_سائز + newlines) * حجم(string_T), TRUE);
    اگر (pp == تہی)	// out of memory
	واپس;
    برائے (lnum = 0; lnum < y_ptr->y_سائز; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    اگر (str_list)
    {
	برائے (ss = (char_u **) str; *ss != تہی; ++ss, ++lnum)
	{
	    pp[lnum].لمبائی = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].لمبائی);
	    اگر (type == MBLOCK)
	    {
		صحیح charlen = vim_strsize(*ss);

		اگر (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    ورنہ
    {
	برائے (آغاز = 0; آغاز < len + extraline; آغاز += i + 1)
	{
	    صحیح charlen = 0;

	    برائے (i = آغاز; i < len;)	// find the end of the line
	    {
		اگر (str[i] == '\n')
		    توڑو;
		اگر (type == MBLOCK)
		{
		    اگر (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    ورنہ
			charlen += byte2cells(str[i]);
		}

		اگر (str[i] == NUL)
		    i++; // registers can have NUL chars
		ورنہ
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= آغاز;			// i is now length of line
	    اگر (charlen > maxlen)
		maxlen = charlen;
	    اگر (append)
	    {
		--lnum;
		extra = (صحیح)y_ptr->y_array[lnum].لمبائی;
	    }
	    ورنہ
		extra = 0;
	    s = alloc(i + extra + 1);
	    اگر (s == تہی)
		توڑو;
	    اگر (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    اگر (append)
		vim_free(y_ptr->y_array[lnum].string);
	    اگر (i > 0)
		mch_memmove(s + extra, str + آغاز, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].لمبائی = extra;
	    ++lnum;
	    جب (--extra >= 0)
	    {
		اگر (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_سائز = lnum;
    اگر (type == MBLOCK)
	y_ptr->y_چوڑائی = (blocklen < 0 ? maxlen - 1 : blocklen);
    ورنہ
	y_ptr->y_چوڑائی = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_وقت_سیٹ = vim_وقت();
# endif
}
#اگر_ختم // FEAT_CLIPBOARD || FEAT_EVAL
