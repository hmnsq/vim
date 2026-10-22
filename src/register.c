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

#include "vim.h"

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
static yankreg_T	y_regs[NUM_REGISTERS];

static yankreg_T	*y_current;	    // ptr to current yankreg
static số_nguyên		y_append;	    // TRUE when appending
static yankreg_T	*y_previous = NULL; // ptr to last written yankreg

static số_nguyên	stuff_yank(số_nguyên, char_u *);
static void	put_reedit_in_typebuf(số_nguyên silent);
static số_nguyên	put_in_typebuf(char_u *s, số_nguyên esc, số_nguyên colon, số_nguyên silent);
static số_nguyên	yank_copy_line(struct block_def *bd, long y_idx, số_nguyên exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
static void	copy_yank_reg(yankreg_T *reg);
#endif
static void	dis_msg(char_u *p, số_nguyên skip_esc);

#nếu defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(void)
{
    trả_về y_regs;
}
#endif

#nếu defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(số_nguyên reg)
{
    trả_về &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(void)
{
    trả_về y_current;
}

    yankreg_T *
get_y_previous(void)
{
    trả_về y_previous;
}

    void
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    void
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    void
reset_y_append(void)
{
    y_append = FALSE;
}


#nếu defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
static char_u	*expr_line = NULL;
static exarg_T	*expr_eap = NULL;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    số_nguyên
get_expr_register(void)
{
    char_u	*new_line;

    new_line = getcmdline('=', 0L, 0, 0);
    nếu (new_line == NULL)
	trả_về NUL;
    nếu (*new_line == NUL)	// use previous line
	vim_free(new_line);
    không_thì
	set_expr_line(new_line, NULL);
    trả_về '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    void
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
get_expr_line(void)
{
    char_u	*expr_copy;
    char_u	*rv;
    static số_nguyên	nested = 0;

    nếu (expr_line == NULL)
	trả_về NULL;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    expr_copy = vim_strsave(expr_line);
    nếu (expr_copy == NULL)
	trả_về NULL;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    nếu (nested >= 10)
	trả_về expr_copy;

    ++nested;
    rv = eval_to_string_eap(expr_copy, TRUE, expr_eap, FALSE);
    --nested;
    vim_free(expr_copy);
    trả_về rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    static char_u *
get_expr_line_src(void)
{
    nếu (expr_line == NULL)
	trả_về NULL;
    trả_về vim_strsave(expr_line);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    số_nguyên
valid_yank_reg(
    số_nguyên	    regname,
    số_nguyên	    writing)	    // if TRUE check for writable registers
{
    nếu (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#không_thì
				    "/#.%:"
#endif
					, regname) != NULL)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#nếu defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#elif defined(FEAT_CLIPBOARD_PROVIDER)
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
	trả_về TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    không_thì nếu (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	msg_warn_missing_clipboard();
	trả_về FALSE;
    }
#endif
    trả_về FALSE;
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
    số_nguyên
get_yank_register(số_nguyên regname, số_nguyên writing)
{
    số_nguyên	    i;
    số_nguyên	    ret = FALSE;

    y_append = FALSE;
    nếu ((regname == 0 || regname == '"') && !writing && y_previous != NULL)
    {
	y_current = y_previous;
	trả_về ret;
    }
    i = regname;
    nếu (VIM_ISDIGIT(i))
	i -= '0';
    không_thì nếu (ASCII_ISLOWER(i))
	i = CharOrdLow(i) + 10;
    không_thì nếu (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    không_thì nếu (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    không_thì nếu (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    không_thì nếu (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	nếu (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	không_thì
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#elif defined(FEAT_CLIPBOARD_PROVIDER)
    không_thì nếu (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    không_thì nếu (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    không_thì nếu (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    không_thì		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    nếu (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    trả_về ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    void *
get_register(
    số_nguyên		name,
    số_nguyên		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    số_nguyên		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    nếu (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	nếu (name == '*' && clip_star.available)
	{
	    nếu (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	nếu (name == '+' && clip_plus.available)
	{
	    nếu (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    nếu (reg == NULL)
	trả_về (void *)NULL;

    *reg = *y_current;
    nếu (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	nếu (reg->y_size == 0 || y_current->y_array == NULL)
	    reg->y_array = NULL;
	không_thì
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	nếu (reg->y_array != NULL)
	{
	    cho (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		nếu (reg->y_array[i].string == NULL)
		{
		    // The allocation failed so clean up and exit
		    trong_khi (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    trả_về (void *)NULL;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    không_thì
	y_current->y_array = NULL;
    trả_về (void *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    void
put_register(số_nguyên name, void *reg)
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

#nếu defined(FEAT_CLIPBOARD)
    void
free_register(void *reg)
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
    số_nguyên
yank_register_mline(số_nguyên regname)
{
    nếu (regname != 0 && !valid_yank_reg(regname, FALSE))
	trả_về FALSE;
    nếu (regname == '_')		// black hole is always empty
	trả_về FALSE;
    get_yank_register(regname, FALSE);
    trả_về (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    số_nguyên
do_record(số_nguyên c)
{
    char_u	    *p;
    static số_nguyên	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    số_nguyên		    retval;

    nếu (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	nếu (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	không_thì
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    không_thì			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	msg("");
	p = get_recorded();
	nếu (p == NULL)
	    retval = FAIL;
	không_thì
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
    trả_về retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    static số_nguyên
stuff_yank(số_nguyên regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    nếu (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	trả_về FAIL;
    }
    nếu (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	trả_về OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    nếu (y_append && y_current->y_array != NULL)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	nếu (tmp == NULL)
	{
	    vim_free(p);
	    trả_về FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    không_thì
    {
	free_yank_all();
	nếu ((y_current->y_array = ALLOC_ONE(string_T)) == NULL)
	{
	    vim_free(p);
	    trả_về FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    trả_về OK;
}

/*
 * Last executed register (@ command)
 */
static số_nguyên execreg_lastc = NUL;

#nếu defined(FEAT_VIMINFO)
    số_nguyên
get_execreg_lastc(void)
{
    trả_về execreg_lastc;
}

    void
set_execreg_lastc(số_nguyên lastc)
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
    static char_u *
execreg_line_continuation(string_T *lines, long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    số_nguyên		j;
    char_u	*str;

    ga_init2(&ga, sizeof(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    trong_khi (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(lines[cmd_start].string);
	nếu (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    break;
    }

    // join all the lines
    tmp = &lines[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    cho (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &lines[j];
	p = skipwhite(tmp->string);
	nếu (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    nếu (ga.ga_len > 400)
	    {
		nếu (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		không_thì
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
    trả_về str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    số_nguyên
do_execreg(
    số_nguyên	    regname,
    số_nguyên	    colon,		// insert ':' before each line
    số_nguyên	    addcr,		// always add '\n' to end of line
    số_nguyên	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    số_nguyên		retval = OK;
    số_nguyên		remap;

    // repeat previous one
    nếu (regname == '@')
    {
	nếu (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    trả_về FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    nếu (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	trả_về FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    nếu (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    nếu (regname == '_')
	trả_về OK;

    // use last command line
    nếu (regname == ':')
    {
	nếu (last_cmdline == NULL)
	{
	    emsg(_(e_no_previous_command_line));
	    trả_về FAIL;
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
	nếu (p != NULL)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    nếu (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    không_thì
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    không_thì nếu (regname == '=')
    {
	p = get_expr_line();
	nếu (p == NULL)
	    trả_về FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    không_thì nếu (regname == '.')		// use last inserted text
    {
	p = get_last_insert_save();
	nếu (p == NULL)
	{
	    emsg(_(e_no_inserted_text_yet));
	    trả_về FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    không_thì
    {
	get_yank_register(regname, FALSE);
	nếu (y_current->y_array == NULL)
	    trả_về FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	cho (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    số_nguyên	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    nếu (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		nếu (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    trả_về FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    nếu (colon && i > 0)
	    {
		p = skipwhite(str);
		nếu (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_line_continuation(y_current->y_array, &i);
		    nếu (str == NULL)
			trả_về FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    nếu (free_str)
		vim_free(str);
	    nếu (escaped == NULL)
		trả_về FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    nếu (retval == FAIL)
		trả_về FAIL;
	    nếu (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		trả_về FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    trả_về retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    static void
put_reedit_in_typebuf(số_nguyên silent)
{
    nếu (restart_edit == NUL)
	trả_về;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    nếu (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    không_thì nếu (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    không_thì nếu (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    nếu (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    static số_nguyên
put_in_typebuf(
    char_u	*s,
    số_nguyên		esc,
    số_nguyên		colon,	    // add ':' before the line
    số_nguyên		silent)
{
    số_nguyên		retval = OK;

    put_reedit_in_typebuf(silent);
    nếu (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    nếu (retval == OK)
    {
	char_u	*p;

	nếu (esc)
	    p = vim_strsave_escape_csi(s);
	không_thì
	    p = s;
	nếu (p == NULL)
	    retval = FAIL;
	không_thì
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	nếu (esc)
	    vim_free(p);
    }
    nếu (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    trả_về retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    số_nguyên
insert_reg(
    số_nguyên		regname,
    số_nguyên		literally_arg)	// insert literally, not as if typed
{
    long	i;
    số_nguyên		retval = OK;
    char_u	*arg;
    số_nguyên		allocated;
    số_nguyên		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    nếu (got_int)
	trả_về FAIL;

    // check for valid regname
    nếu (regname != NUL && !valid_yank_reg(regname, FALSE))
	trả_về FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    nếu (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    nếu (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    không_thì nếu (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	nếu (arg == NULL)
	    trả_về FAIL;
	stuffescaped(arg, literally);
	nếu (allocated)
	    vim_free(arg);
    }
    không_thì				// name or number register
    {
	nếu (get_yank_register(regname, FALSE))
	    literally = TRUE;
	nếu (y_current->y_array == NULL)
	    retval = FAIL;
	không_thì
	{
	    cho (i = 0; i < y_current->y_size; ++i)
	    {
		nếu (regname == '-' && y_current->y_type == MCHAR)
		{
		    số_nguyên dir = BACKWARD;
		    nếu ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			nếu (u_save_cursor() == FAIL)
			    trả_về FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			nếu (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, NULL, dir, 1L, PUT_CURSEND);
		}
		không_thì
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    nếu (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    trả_về retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    số_nguyên
get_spec_reg(
    số_nguyên		regname,
    char_u	**argp,
    số_nguyên		*allocated,	// return: TRUE when value was allocated
    số_nguyên		errmsg)		// give error message when failing
{
    số_nguyên		cnt;

    *argp = NULL;
    *allocated = FALSE;
    switch (regname)
    {
	case '%':		// file name
	    nếu (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    trả_về TRUE;

	case '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    trả_về TRUE;

#ifdef FEAT_EVAL
	case '=':		// result of expression
	    *argp = get_expr_line();
	    *allocated = TRUE;
	    trả_về TRUE;
#endif

	case ':':		// last command line
	    nếu (last_cmdline == NULL && errmsg)
		emsg(_(e_no_previous_command_line));
	    *argp = last_cmdline;
	    trả_về TRUE;

	case '/':		// last search-pattern
	    nếu (last_search_pat() == NULL && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_search_pat();
	    trả_về TRUE;

	case '.':		// last inserted text
	    *argp = get_last_insert_save();
	    *allocated = TRUE;
	    nếu (*argp == NULL && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    trả_về TRUE;

	case Ctrl_F:		// Filename under cursor
	case Ctrl_P:		// Path under cursor, expand via "path"
	    nếu (!errmsg)
		trả_về FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, NULL);
	    *allocated = TRUE;
	    trả_về TRUE;

	case Ctrl_W:		// word under cursor
	case Ctrl_A:		// WORD (mnemonic All) under cursor
	    nếu (!errmsg)
		trả_về FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : NULL;
	    *allocated = TRUE;
	    trả_về TRUE;

	case Ctrl_L:		// Line under cursor
	    nếu (!errmsg)
		trả_về FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    trả_về TRUE;

	case '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    trả_về TRUE;
    }

    trả_về FALSE;
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
    số_nguyên
cmdline_paste_reg(
    số_nguyên regname,
    số_nguyên literally_arg,	// Insert text literally instead of "as typed"
    số_nguyên remcr)		// don't add CR characters
{
    long	i;
    số_nguyên		literally = literally_arg;

    nếu (get_yank_register(regname, FALSE))
	literally = TRUE;
    nếu (y_current->y_array == NULL)
	trả_về FAIL;

    cho (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	nếu ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	nếu (got_int)
	    trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    void
shift_delete_registers(void)
{
    số_nguyên		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    cho (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    nếu (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = NULL;		// set register one to empty
}

#nếu defined(FEAT_EVAL)
    static void
add_regtype_to_dict(số_nguyên regname, dict_T *dict, char_u *buf, số_nguyên bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    switch (get_reg_type(regname, &reglen))
    {
	case MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    break;
	case MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    break;
	case MBLOCK:
	    buflen = vim_snprintf_safelen((char *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    break;
	default:
	    buf[0] = NUL;
	    buflen = 0;
	    break;
    }
    (void)dict_add_string_len(dict, "regtype", buf, (số_nguyên)buflen);
}

    void
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    static số_nguyên	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    số_nguyên		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    nếu (recursive)
	trả_về;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    nếu (list == NULL)
	trả_về;

    // yanked text contents
    cho (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (số_nguyên)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    nếu (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (void)dict_add_string_len(v_event, "regname", buf, (số_nguyên)buflen);

    // motion type: inclusive or exclusive
    (void)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (void)dict_add_string_len(v_event, "operator", buf, (số_nguyên)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, sizeof(buf));

    // selection type - visual or not
    (void)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_items_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, NULL, NULL, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    static void
put_do_autocmd(
	số_nguyên	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *insert,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	số_nguyên	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    static bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    số_nguyên		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    nếu (recursive || regname == '_')
	trả_về;

    nếu (regname != '.' && insert == NULL
	    && reg == NULL)
	// Can happen when pasting text in normal mode in a terminal buffer
	trả_về;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    nếu (list == NULL)
	trả_về;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    nếu (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    nếu (regname == '.')
    {
	nếu (last_insert_ga.ga_data != NULL)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_insert_ga.ga_data,
		    (số_nguyên)last_insert_ga.ga_len);
    }
    không_thì nếu (insert != NULL)
    {
	list_append_string(list, insert->string, (số_nguyên)insert->length);
    }
    không_thì
    {
	cho (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (số_nguyên)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    nếu (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (void)dict_add_string_len(v_event, "regname", buf, (số_nguyên)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (void)dict_add_string_len(v_event, "operator", buf, (số_nguyên)buflen);

    add_regtype_to_dict(regname, v_event, buf, sizeof(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (void)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_items_ro(v_event);

    recursive = true;
    textlock++;
    nếu (post)
	apply_autocmds(EVENT_TEXTPUTPOST, NULL, NULL, FALSE, curbuf);
    không_thì
	apply_autocmds(EVENT_TEXTPUTPRE, NULL, NULL, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    void
init_yank(void)
{
    số_nguyên		i;

    cho (i = 0; i < NUM_REGISTERS; ++i)
	y_regs[i].y_array = NULL;
}

#nếu defined(EXITFREE)
    void
clear_registers(void)
{
    số_nguyên		i;

    cho (i = 0; i < NUM_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	nếu (y_current->y_array != NULL)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    static void
free_yank(long n)
{
    nếu (y_current->y_array == NULL)
	trả_về;

    long	    i;

    cho (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    void
free_yank_all(void)
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
    số_nguyên
op_yank(oparg_T *oap, số_nguyên deleting, số_nguyên mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    số_nguyên			yanktype = oap->motion_type;
    long		yanklines = oap->line_count;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    struct block_def	bd;

				    // check for read-only register
    nếu (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	trả_về FAIL;
    }
    nếu (oap->regname == '_')	    // black hole: nothing to do
	trả_về OK;

#ifdef FEAT_CLIPBOARD
    nếu ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	msg_warn_missing_clipboard();
    }
#endif

    nếu (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    nếu (y_append && y_current->y_array != NULL)
	y_current = &newreg;
    không_thì
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    nếu (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(sizeof(string_T) * yanklines, TRUE);
    nếu (y_current->y_array == NULL)
    {
	y_current = curr;
	trả_về FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    nếu (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	nếu (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    cho ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	switch (y_current->y_type)
	{
	    case MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		nếu (yank_copy_line(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    goto fail;
		break;

	    case MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		nếu ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == NULL)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    goto fail;
		}
		break;

	    case MCHAR:
		{
		    số_nguyên tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (số_nguyên)STRLEN(bd.textstart);
		    nếu (tmp < bd.textlen)
			bd.textlen = tmp;

		    nếu (yank_copy_line(&bd, y_idx, FALSE) == FAIL)
			goto fail;
		    break;
		}
		// NOTREACHED
	}
    }

    nếu (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	nếu (new_ptr == NULL)
	    goto fail;
	cho (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	nếu (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	nếu (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == NULL)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    nếu (pnew == NULL)
	    {
		y_idx = y_current->y_size - 1;
		goto fail;
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
	không_thì
	    y_idx = 0;
	trong_khi (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    nếu (mess)			// Display message about yank?
    {
	nếu (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	nếu (yanklines > p_report)
	{
	    char namebuf[100];

	    nếu (oap->regname == NUL)
		*namebuf = NUL;
	    không_thì
		vim_snprintf(namebuf, sizeof(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    nếu (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    không_thì
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    nếu ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	nếu (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	nếu (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    nếu (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	nếu (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    nếu (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	nếu (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    nếu (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	nếu (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    nếu (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# nếu defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	nếu (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    nếu (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#nếu defined(FEAT_EVAL)
    nếu (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    trả_về OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    trả_về FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    static số_nguyên
yank_copy_line(struct block_def *bd, long y_idx, số_nguyên exclude_trailing_space)
{
    char_u	*pnew;

    nếu (exclude_trailing_space)
	bd->endspaces = 0;
    nếu ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == NULL)
	trả_về FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    nếu (exclude_trailing_space)
    {
	số_nguyên s = bd->textlen + bd->endspaces;

	trong_khi (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    trả_về OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    static void
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    sizeof(string_T) * y_current->y_size, TRUE);
    nếu (y_current->y_array == NULL)
	y_current->y_size = 0;
    không_thì
	cho (j = 0; j < y_current->y_size; ++j)
	{
	    nếu ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == NULL)
	    {
		free_yank(j);
		y_current->y_size = 0;
		break;
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
    void
do_put(
    số_nguyên		regname,
    char_u	*expr_result,	// result for regname "=" when compiled
    số_nguyên		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	count,
    số_nguyên		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    số_nguyên		yanklen;
    số_nguyên		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    số_nguyên		y_type;
    long	y_size;
    số_nguyên		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = NULL;
    yankreg_T	*y_current_used = NULL;
    long	nr_lines = 0;
    string_T	insert_string;
    số_nguyên		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned số_nguyên cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    nếu (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(void)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    nếu (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    nếu (regname == '.')
    {
	nếu (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	nếu (has_textput_events)
	    add_last_insert++;
#endif

	(void)stuff_inserted((dir == FORWARD ? (count == -1 ? 'o' : 'a') :
				    (count == -1 ? 'O' : 'i')), count, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	nếu (has_textputpre())
	    put_do_autocmd('.', NULL, NULL, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	nếu (has_textputpost())
	    put_do_autocmd('.', NULL, NULL, true, dir);

	nếu (has_textput_events && --add_last_insert == 0)
	    ga_clear(&last_insert_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	nếu ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	trả_về;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    insert_string.string = NULL;
    nếu (regname == '=' && expr_result != NULL)
	insert_string.string = expr_result;
    không_thì nếu (get_spec_reg(regname, &insert_string.string, &allocated, TRUE)
		&& insert_string.string == NULL)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	trả_về;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    nếu (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	goto end;

    nếu (insert_string.string != NULL)
    {
	insert_string.length = STRLEN(insert_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	nếu (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    cho (;;)
	    {
		y_size = 0;
		ptr = insert_string.string;
		ptrlen = insert_string.length;
		trong_khi (ptr != NULL)
		{
		    nếu (y_array != NULL)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    nếu (tmp == NULL)
		    {
			nếu (y_array != NULL)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    không_thì
		    {
			nếu (y_array != NULL)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			nếu (*tmp == NUL)
			{
			    y_type = MLINE;
			    break;
			}
		    }
		    ptr = tmp;
		}
		nếu (y_array != NULL)
		    break;
		y_array = ALLOC_MULT(string_T, y_size);
		nếu (y_array == NULL)
		    goto end;
	    }
	}
	không_thì
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &insert_string;
	}
#ifdef FEAT_EVAL
	nếu (has_textputpre())
	    put_do_autocmd(regname, NULL, &insert_string, false, dir);
#endif
    }
    không_thì
    {
#ifdef FEAT_EVAL
	nếu (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, NULL, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    nếu (y_type == MLINE)
    {
	nếu (flags & PUT_LINE_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    nếu (u_save_cursor() == FAIL)
		goto end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    nếu (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    nếu (ptr == NULL)
		goto end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    nếu (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    nếu (ptr == NULL)
		goto end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_lines;
	    dir = FORWARD;
	}
	nếu (flags & PUT_LINE_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    nếu (flags & PUT_LINE)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    nếu (y_size == 0 || y_array == NULL)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	goto end;
    }

    nếu (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	nếu (lnum > curbuf->b_ml.ml_line_count)
	    lnum = curbuf->b_ml.ml_line_count + 1;
	nếu (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    goto end;
    }
    không_thì nếu (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	nếu (dir == BACKWARD)
	    (void)hasFolding(lnum, &lnum, NULL);
	không_thì
	    (void)hasFolding(lnum, NULL, &lnum);
#endif
	nếu (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	nếu ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    goto end;
#ifdef FEAT_FOLDING
	nếu (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	không_thì
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    không_thì nếu (u_save_cursor() == FAIL)
	goto end;

    nếu (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	nếu (gchar_cursor() == TAB)
	{
	    số_nguyên viscol = getviscol();
	    số_nguyên ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    nếu (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#không_thì
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    không_thì
		curwin->w_cursor.coladd = 0;
	}
	không_thì nếu (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    nếu (y_type == MBLOCK)
    {
	số_nguyên	delcount;
	số_nguyên	incr = 0;
	struct block_def bd;
	long	j;
	số_nguyên	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	nếu (dir == FORWARD && c != NUL)
	{
	    nếu (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, NULL, &endcol2, 0);
	    không_thì
		getvcol(curwin, &curwin->w_cursor, NULL, NULL, &col, 0);

	    nếu (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    không_thì
		nếu (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	không_thì
	    getvcol(curwin, &curwin->w_cursor, &col, NULL, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	nếu (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    nếu (dir == FORWARD && c == NUL)
		++col;
	    nếu (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    nếu (c == TAB)
	    {
		nếu (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		nếu (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	cho (i = 0; i < y_size; ++i)
	{
	    số_nguyên		    spaces = 0;
	    char	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    nếu (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	    {
		nếu (ml_append(curbuf->b_ml.ml_line_count, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    break;
		++nr_lines;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    trong_khi (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    nếu (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    không_thì nếu (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		nếu (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		nếu (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (số_nguyên)y_array[i].length;

	    nếu ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		trong_khi (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		nếu (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    nếu (yanklen + spaces != 0
		     && count > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		break;
	    }

	    totlen = count * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    nếu (newp == NULL)
		break;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    cho (j = 0; j < count; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		nếu ((j < count - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		không_thì
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
	    nếu (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_lines(lnum, 0, curwin->w_cursor.lnum - nr_lines, nr_lines);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	nếu (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	nếu (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    nếu (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	không_thì
	    curwin->w_cursor.lnum = lnum;
    }
    không_thì
    {
	pos_T	new_cursor;

	yanklen = (số_nguyên)y_array[0].length;

	// Character or Line mode
	nếu (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    nếu (dir == FORWARD && gchar_cursor() != NUL)
	    {
		nếu (has_mbyte)
		{
		    số_nguyên bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    nếu (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		không_thì
		{
		    ++col;
		    nếu (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	không_thì nếu (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	nếu (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    số_nguyên		first_byte_off = 0;

	    nếu (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		nếu (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		nếu (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, NULL, &vcol, NULL, 0);
		}
	    }

	    nếu (count == 0 || yanklen == 0)
	    {
		nếu (VIsual_active)
		    lnum = end_lnum;
	    }
	    không_thì nếu (count > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    không_thì
	    {
		totlen = count * yanklen;
		do
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    nếu (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			nếu (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			không_thì
			    col = MAXCOL;
		    }
		    nếu (VIsual_active && col > oldlen)
		    {
			lnum++;
			continue;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    nếu (newp == NULL)
			goto end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    cho (i = 0; i < count; ++i)
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
		    nếu (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    nếu (VIsual_active)
			lnum++;
		} trong_khi (VIsual_active && lnum <= end_lnum);

		nếu (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    nếu (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    không_thì
		curwin->w_cursor.col -= first_byte_off;
	}
	không_thì
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    số_nguyên		indent;
	    số_nguyên		orig_indent = 0;
	    số_nguyên		indent_diff = 0;	// init for gcc
	    số_nguyên		first_indent = TRUE;
	    số_nguyên		lendiff = 0;
	    long	cnt;

	    nếu (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    cho (cnt = 1; cnt <= count; ++cnt)
	    {
		i = 0;
		nếu (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (số_nguyên)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    nếu (newp == NULL)
			goto error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    nếu (newp == NULL)
			goto error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		cho (; i < y_size; ++i)
		{
		    nếu (y_type != MCHAR || i < y_size - 1)
		    {
			nếu (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    goto error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_lines;
		    nếu (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			nếu (cnt == count && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			nếu (*ptr == '#' && preprocs_left())
			    indent = 0;     // Leave # lines at start
			không_thì nếu (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			không_thì nếu (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			không_thì nếu ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(void)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			nếu (cnt == count && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		nếu (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    nếu (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		nếu (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_lines, 0L);

	    // note changed text for displaying and folding
	    nếu (y_type == MCHAR)
		changed_lines(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_lines);
	    không_thì
		changed_lines(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_lines);
	    nếu (y_current_used != NULL && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		goto end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    nếu (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		nếu (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    không_thì
		curbuf->b_op_end.col = 0;

	    nếu (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    không_thì nếu (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		nếu (y_type == MLINE)
		{
		    nếu (lnum >= curbuf->b_ml.ml_line_count)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		    không_thì
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		không_thì
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    nếu (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    không_thì nếu (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		nếu (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    không_thì	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_lines);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    số_nguyên len = ml_get_curline_len();
    nếu (curwin->w_cursor.col > len)
    {
	nếu (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    nếu (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    nếu (has_textputpost())
    {
	nếu (insert_string.string == NULL)
	    put_do_autocmd(regname, y_current, NULL, true, dir);
	không_thì
	    put_do_autocmd(regname, NULL, &insert_string, true, dir);
    }
#endif

    nếu (allocated)
	vim_free(insert_string.string);
    nếu (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    số_nguyên
get_register_name(số_nguyên num)
{
    nếu (num == -1)
	trả_về '"';
    không_thì nếu (num < 10)
	trả_về num + '0';
    không_thì nếu (num == DELETION_REGISTER)
	trả_về '-';
#nếu defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    không_thì nếu (num == STAR_REGISTER)
	trả_về '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    không_thì nếu (clipmethod == CLIPMETHOD_PROVIDER && num == REAL_PLUS_REGISTER)
	trả_về '+';
    không_thì nếu (clipmethod != CLIPMETHOD_PROVIDER && num == PLUS_REGISTER)
	trả_về '+';
#endif
    không_thì
	trả_về num + 'a' - 10;
}

#nếu defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    số_nguyên
get_unname_register(void)
{
    trả_về y_previous == NULL ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    void
ex_display(exarg_T *eap)
{
    số_nguyên		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    số_nguyên		name;
    số_nguyên		attr;
    char_u	*arg = eap->arg;
    số_nguyên		clen;
    số_nguyên		type;
    string_T	insert;

    silence_w23_w24_msg++;
    nếu (arg != NULL && *arg == NUL)
	arg = NULL;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    msg_puts_title(_("\nType Name Content"));
    cho (i = -1; i < NUM_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	switch (get_reg_type(name, NULL))
	{
	    case MLINE: type = 'l'; break;
	    case MCHAR: type = 'c'; break;
	    default:	type = 'b'; break;
	}
	nếu (arg != NULL && vim_strchr(arg, name) == NULL
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == NULL)
#endif
		)
	    continue;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	nếu (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (void)may_get_selection(name);
#endif

	nếu (i == -1)
	{
	    nếu (y_previous != NULL)
		yb = y_previous;
	    không_thì
		yb = &(y_regs[0]);
	}
	không_thì
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	nếu (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != NULL &&
		    (yb == y_previous || yb == &y_regs[0])))
	    continue;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	nếu (yb->y_array != NULL)
	{
	    số_nguyên do_show = FALSE;

	    cho (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    nếu (do_show || yb->y_size == 0)
	    {
		msg_putchar('\n');
		msg_puts("  ");
		msg_putchar(type);
		msg_puts("  ");
		msg_putchar('"');
		msg_putchar(name);
		msg_puts("   ");

		n = (số_nguyên)Columns - 11;
		cho (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    nếu (j)
		    {
			msg_puts_attr("^J", attr);
			n -= 2;
		    }
		    cho (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			msg_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		nếu (n > 1 && yb->y_type == MLINE)
		    msg_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    insert = get_last_insert();
    nếu ((p = insert.string) != NULL
		  && (arg == NULL || vim_strchr(arg, '.') != NULL) && !got_int
						      && !message_filtered(p))
    {
	msg_puts("\n  c  \".   ");
	dis_msg(p, TRUE);
    }

    // display last command line
    nếu (last_cmdline != NULL && (arg == NULL || vim_strchr(arg, ':') != NULL)
			       && !got_int && !message_filtered(last_cmdline))
    {
	msg_puts("\n  c  \":   ");
	dis_msg(last_cmdline, FALSE);
    }

    // display current file name
    nếu (curbuf->b_fname != NULL
	    && (arg == NULL || vim_strchr(arg, '%') != NULL) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	msg_puts("\n  c  \"%   ");
	dis_msg(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    nếu ((arg == NULL || vim_strchr(arg, '#') != NULL) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	nếu (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    msg_puts("\n  c  \"#   ");
	    dis_msg(fname, FALSE);
	}
    }

    // display last search pattern
    nếu (last_search_pat() != NULL
		 && (arg == NULL || vim_strchr(arg, '/') != NULL) && !got_int
				      && !message_filtered(last_search_pat()))
    {
	msg_puts("\n  c  \"/   ");
	dis_msg(last_search_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    nếu (expr_line != NULL && (arg == NULL || vim_strchr(arg, '=') != NULL)
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
    static void
dis_msg(
    char_u	*p,
    số_nguyên		skip_esc)	    // if TRUE, ignore trailing ESC
{
    số_nguyên		n;
    số_nguyên		l;

    n = (số_nguyên)Columns - 6;
    trong_khi (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	nếu (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    msg_outtrans_len(p, l);
	    p += l;
	}
	không_thì
	    msg_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#nếu defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    void
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
get_reg_type(số_nguyên regname, long *reglen)
{
    switch (regname)
    {
	case '%':		// file name
	case '#':		// alternate file name
	case '=':		// expression
	case ':':		// last command line
	case '/':		// last search-pattern
	case '.':		// last inserted text
	case Ctrl_F:		// Filename under cursor
	case Ctrl_P:		// Path under cursor, expand via "path"
	case Ctrl_W:		// word under cursor
	case Ctrl_A:		// WORD (mnemonic All) under cursor
	case '_':		// black hole: always empty
	    trả_về MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    nếu (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    nếu (regname != NUL && !valid_yank_reg(regname, FALSE))
	trả_về MAUTO;

    get_yank_register(regname, FALSE);

    nếu (y_current->y_array != NULL)
    {
	nếu (reglen != NULL && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	trả_về y_current->y_type;
    }
    trả_về MAUTO;
}

#nếu defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    static char_u *
getreg_wrap_one_line(char_u *s, số_nguyên flags)
{
    nếu (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	nếu (list != NULL)
	{
	    nếu (list_append_string(list, NULL, -1) == FAIL)
	    {
		list_free(list);
		trả_về NULL;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	trả_về (char_u *)list;
    }
    trả_về s;
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
get_reg_contents(số_nguyên regname, số_nguyên flags)
{
    linenr_T	i;
    char_u	*retval;
    số_nguyên		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    nếu (regname == '=')
    {
	nếu (flags & GREG_NO_EXPR)
	    trả_về NULL;
	nếu (flags & GREG_EXPR_SRC)
	    trả_về getreg_wrap_one_line(get_expr_line_src(), flags);
	trả_về getreg_wrap_one_line(get_expr_line(), flags);
    }

    nếu (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    nếu (regname != NUL && !valid_yank_reg(regname, FALSE))
	trả_về NULL;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    nếu (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    nếu (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	nếu (retval == NULL)
	    trả_về NULL;
	nếu (allocated)
	    trả_về getreg_wrap_one_line(retval, flags);
	trả_về getreg_wrap_one_line(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    nếu (y_current->y_array == NULL)
	trả_về NULL;

    nếu (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	nếu (list == NULL)
	    trả_về NULL;
	cho (i = 0; i < y_current->y_size; ++i)
	{
	    nếu (list_append_string(list, y_current->y_array[i].string,
		(số_nguyên)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		trả_về NULL;
	    }
	}
	trả_về (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    cho (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	nếu (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    nếu (retval == NULL)
	trả_về NULL;

    // Copy the lines of the yank register into the string.
    len = 0;
    cho (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	nếu (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    trả_về retval;
}

    static số_nguyên
init_write_reg(
    số_nguyên		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    số_nguyên		must_append,
    số_nguyên		*yank_type UNUSED)
{
    nếu (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	trả_về FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    nếu (!y_append && !must_append)
	free_yank_all();
    trả_về OK;
}

    static void
finish_write_reg(
    số_nguyên		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    nếu (name != '"')
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
    void
write_reg_contents(
    số_nguyên		name,
    char_u	*str,
    số_nguyên		maxlen,
    số_nguyên		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    void
write_reg_contents_lst(
    số_nguyên		name,
    char_u	**strings,
    số_nguyên		maxlen UNUSED,
    số_nguyên		must_append,
    số_nguyên		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    nếu (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	nếu (strings[0] == NULL)
	    s = (char_u *)"";
	không_thì nếu (strings[1] != NULL)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_lines), name);
	    trả_về;
	}
	không_thì
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	trả_về;
    }

    nếu (name == '_')	    // black hole: nothing to do
	trả_về;

    nếu (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	trả_về;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    void
write_reg_contents_ex(
    số_nguyên		name,
    char_u	*str,
    số_nguyên		maxlen,
    số_nguyên		must_append,
    số_nguyên		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    nếu (maxlen >= 0)
	len = maxlen;
    không_thì
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    nếu (name == '/')
    {
	set_last_search_pat(str, RE_SEARCH, TRUE, TRUE);
	trả_về;
    }

    nếu (name == '#')
    {
	nếu (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  trả_về;
	}

	buf_T	*buf;

	nếu (VIM_ISDIGIT(*str))
	{
	    số_nguyên	num = atoi((char *)str);

	    buf = buflist_findnr(num);
	    nếu (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist), (long)num);
	}
	không_thì
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	nếu (buf == NULL)
	    trả_về;
	curwin->w_alt_fnum = buf->b_fnum;
	trả_về;
    }

    nếu (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	nếu (p == NULL)
	    trả_về;
	nếu (must_append && expr_line != NULL)
	{
	    s = concat_str(expr_line, p);
	    vim_free(p);
	    p = s;
	}
	set_expr_line(p, NULL);
	trả_về;
    }

    nếu (name == '_')	    // black hole: nothing to do
	trả_về;

    nếu (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	trả_về;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#nếu defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    void
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    số_nguyên		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    số_nguyên		str_list)	// TRUE if str is char_u **
{
    số_nguyên		type;			// MCHAR, MLINE or MBLOCK
    số_nguyên		lnum;
    long	start;
    long	i;
    số_nguyên		extra;
    số_nguyên		newlines;		// number of lines added
    số_nguyên		extraline = 0;		// extra line at the end
    số_nguyên		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    nếu (y_ptr->y_array == NULL)		// NULL means empty register
	y_ptr->y_size = 0;

    nếu (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    không_thì
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    nếu (str_list)
    {
	cho (ss = (char_u **) str; *ss != NULL; ++ss)
	    ++newlines;
    }
    không_thì
    {
	cho (i = 0; i < len; i++)
	    nếu (str[i] == '\n')
		++newlines;
	nếu (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	nếu (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    nếu (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	trả_về;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * sizeof(string_T), TRUE);
    nếu (pp == NULL)	// out of memory
	trả_về;
    cho (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    nếu (str_list)
    {
	cho (ss = (char_u **) str; *ss != NULL; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    nếu (type == MBLOCK)
	    {
		số_nguyên charlen = vim_strsize(*ss);

		nếu (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    không_thì
    {
	cho (start = 0; start < len + extraline; start += i + 1)
	{
	    số_nguyên charlen = 0;

	    cho (i = start; i < len;)	// find the end of the line
	    {
		nếu (str[i] == '\n')
		    break;
		nếu (type == MBLOCK)
		{
		    nếu (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    không_thì
			charlen += byte2cells(str[i]);
		}

		nếu (str[i] == NUL)
		    i++; // registers can have NUL chars
		không_thì
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    nếu (charlen > maxlen)
		maxlen = charlen;
	    nếu (append)
	    {
		--lnum;
		extra = (số_nguyên)y_ptr->y_array[lnum].length;
	    }
	    không_thì
		extra = 0;
	    s = alloc(i + extra + 1);
	    nếu (s == NULL)
		break;
	    nếu (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    nếu (append)
		vim_free(y_ptr->y_array[lnum].string);
	    nếu (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    trong_khi (--extra >= 0)
	    {
		nếu (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    nếu (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    không_thì
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
