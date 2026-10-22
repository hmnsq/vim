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
static puur_naa_nka		y_append;	    // TRUE when appending
static yankreg_T	*y_previous = NULL; // ptr to last written yankreg

static puur_naa_nka	stuff_yank(puur_naa_nka, char_u *);
static void	put_reedit_in_typebuf(puur_naa_nka silent);
static puur_naa_nka	put_in_typebuf(char_u *s, puur_naa_nka esc, puur_naa_nka colon, puur_naa_nka silent);
static puur_naa_nka	yank_copy_line(sa_nracanaa block_def *bd, long y_idx, puur_naa_nka exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
static void	copy_yank_reg(yankreg_T *reg);
#endif
static void	dis_msg(char_u *p, puur_naa_nka skip_esc);

#ᮚᮦᮔ᮪ defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(void)
{
    ᮝᮜᮤ y_regs;
}
#endif

#ᮚᮦᮔ᮪ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(puur_naa_nka reg)
{
    ᮝᮜᮤ &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(void)
{
    ᮝᮜᮤ y_current;
}

    yankreg_T *
get_y_previous(void)
{
    ᮝᮜᮤ y_previous;
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


#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
static char_u	*expr_line = NULL;
static exarg_T	*expr_eap = NULL;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    puur_naa_nka
get_expr_register(void)
{
    char_u	*new_line;

    new_line = getcmdline('=', 0L, 0, 0);
    ᮚᮦᮔ᮪ (new_line == NULL)
	ᮝᮜᮤ NUL;
    ᮚᮦᮔ᮪ (*new_line == NUL)	// use previous line
	vim_free(new_line);
    ᮜᮤᮚ
	set_expr_line(new_line, NULL);
    ᮝᮜᮤ '=';
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
    static puur_naa_nka	nested = 0;

    ᮚᮦᮔ᮪ (expr_line == NULL)
	ᮝᮜᮤ NULL;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    expr_copy = vim_strsave(expr_line);
    ᮚᮦᮔ᮪ (expr_copy == NULL)
	ᮝᮜᮤ NULL;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    ᮚᮦᮔ᮪ (nested >= 10)
	ᮝᮜᮤ expr_copy;

    ++nested;
    rv = eval_to_string_eap(expr_copy, TRUE, expr_eap, FALSE);
    --nested;
    vim_free(expr_copy);
    ᮝᮜᮤ rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    static char_u *
get_expr_line_src(void)
{
    ᮚᮦᮔ᮪ (expr_line == NULL)
	ᮝᮜᮤ NULL;
    ᮝᮜᮤ vim_strsave(expr_line);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    puur_naa_nka
valid_yank_reg(
    puur_naa_nka	    regname,
    puur_naa_nka	    writing)	    // if TRUE check for writable registers
{
    ᮚᮦᮔ᮪ (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#ᮜᮤᮚ
				    "/#.%:"
#endif
					, regname) != NULL)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#ᮚᮦᮔ᮪ defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
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
	ᮝᮜᮤ TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    ᮜᮤᮚ ᮚᮦᮔ᮪ (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	msg_warn_missing_clipboard();
	ᮝᮜᮤ FALSE;
    }
#endif
    ᮝᮜᮤ FALSE;
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
    puur_naa_nka
get_yank_register(puur_naa_nka regname, puur_naa_nka writing)
{
    puur_naa_nka	    i;
    puur_naa_nka	    ret = FALSE;

    y_append = FALSE;
    ᮚᮦᮔ᮪ ((regname == 0 || regname == '"') && !writing && y_previous != NULL)
    {
	y_current = y_previous;
	ᮝᮜᮤ ret;
    }
    i = regname;
    ᮚᮦᮔ᮪ (VIM_ISDIGIT(i))
	i -= '0';
    ᮜᮤᮚ ᮚᮦᮔ᮪ (ASCII_ISLOWER(i))
	i = CharOrdLow(i) + 10;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    ᮜᮤᮚ ᮚᮦᮔ᮪ (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    ᮜᮤᮚ ᮚᮦᮔ᮪ (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	ᮚᮦᮔ᮪ (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	ᮜᮤᮚ
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#elif defined(FEAT_CLIPBOARD_PROVIDER)
    ᮜᮤᮚ ᮚᮦᮔ᮪ (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    ᮜᮤᮚ ᮚᮦᮔ᮪ (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    ᮜᮤᮚ		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    ᮚᮦᮔ᮪ (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    ᮝᮜᮤ ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    void *
get_register(
    puur_naa_nka		name,
    puur_naa_nka		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    puur_naa_nka		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	ᮚᮦᮔ᮪ (name == '*' && clip_star.available)
	{
	    ᮚᮦᮔ᮪ (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	ᮚᮦᮔ᮪ (name == '+' && clip_plus.available)
	{
	    ᮚᮦᮔ᮪ (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    ᮚᮦᮔ᮪ (reg == NULL)
	ᮝᮜᮤ (void *)NULL;

    *reg = *y_current;
    ᮚᮦᮔ᮪ (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	ᮚᮦᮔ᮪ (reg->y_size == 0 || y_current->y_array == NULL)
	    reg->y_array = NULL;
	ᮜᮤᮚ
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	ᮚᮦᮔ᮪ (reg->y_array != NULL)
	{
	    for (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		ᮚᮦᮔ᮪ (reg->y_array[i].string == NULL)
		{
		    // The allocation failed so clean up and exit
		    ᮞᮨᮓᮨᮜ (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    ᮝᮜᮤ (void *)NULL;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    ᮜᮤᮚ
	y_current->y_array = NULL;
    ᮝᮜᮤ (void *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    void
put_register(puur_naa_nka name, void *reg)
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

#ᮚᮦᮔ᮪ defined(FEAT_CLIPBOARD)
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
    puur_naa_nka
yank_register_mline(puur_naa_nka regname)
{
    ᮚᮦᮔ᮪ (regname != 0 && !valid_yank_reg(regname, FALSE))
	ᮝᮜᮤ FALSE;
    ᮚᮦᮔ᮪ (regname == '_')		// black hole is always empty
	ᮝᮜᮤ FALSE;
    get_yank_register(regname, FALSE);
    ᮝᮜᮤ (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    puur_naa_nka
do_record(puur_naa_nka c)
{
    char_u	    *p;
    static puur_naa_nka	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    puur_naa_nka		    retval;

    ᮚᮦᮔ᮪ (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	ᮚᮦᮔ᮪ (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	ᮜᮤᮚ
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    ᮜᮤᮚ			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	msg("");
	p = get_recorded();
	ᮚᮦᮔ᮪ (p == NULL)
	    retval = FAIL;
	ᮜᮤᮚ
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
    ᮝᮜᮤ retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    static puur_naa_nka
stuff_yank(puur_naa_nka regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    ᮚᮦᮔ᮪ (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	ᮝᮜᮤ FAIL;
    }
    ᮚᮦᮔ᮪ (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	ᮝᮜᮤ OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    ᮚᮦᮔ᮪ (y_append && y_current->y_array != NULL)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	ᮚᮦᮔ᮪ (tmp == NULL)
	{
	    vim_free(p);
	    ᮝᮜᮤ FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    ᮜᮤᮚ
    {
	free_yank_all();
	ᮚᮦᮔ᮪ ((y_current->y_array = ALLOC_ONE(string_T)) == NULL)
	{
	    vim_free(p);
	    ᮝᮜᮤ FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    ᮝᮜᮤ OK;
}

/*
 * Last executed register (@ command)
 */
static puur_naa_nka execreg_lastc = NUL;

#ᮚᮦᮔ᮪ defined(FEAT_VIMINFO)
    puur_naa_nka
get_execreg_lastc(void)
{
    ᮝᮜᮤ execreg_lastc;
}

    void
set_execreg_lastc(puur_naa_nka lastc)
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
    puur_naa_nka		j;
    char_u	*str;

    ga_init2(&ga, parimaa_na(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    ᮞᮨᮓᮨᮜ (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(lines[cmd_start].string);
	ᮚᮦᮔ᮪ (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    bha_nga;
    }

    // join all the lines
    tmp = &lines[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    for (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &lines[j];
	p = skipwhite(tmp->string);
	ᮚᮦᮔ᮪ (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    ᮚᮦᮔ᮪ (ga.ga_len > 400)
	    {
		ᮚᮦᮔ᮪ (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		ᮜᮤᮚ
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
    ᮝᮜᮤ str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    puur_naa_nka
do_execreg(
    puur_naa_nka	    regname,
    puur_naa_nka	    colon,		// insert ':' before each line
    puur_naa_nka	    addcr,		// always add '\n' to end of line
    puur_naa_nka	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    puur_naa_nka		retval = OK;
    puur_naa_nka		remap;

    // repeat previous one
    ᮚᮦᮔ᮪ (regname == '@')
    {
	ᮚᮦᮔ᮪ (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    ᮝᮜᮤ FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    ᮚᮦᮔ᮪ (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	ᮝᮜᮤ FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    ᮚᮦᮔ᮪ (regname == '_')
	ᮝᮜᮤ OK;

    // use last command line
    ᮚᮦᮔ᮪ (regname == ':')
    {
	ᮚᮦᮔ᮪ (last_cmdline == NULL)
	{
	    emsg(_(e_no_previous_command_line));
	    ᮝᮜᮤ FAIL;
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
	ᮚᮦᮔ᮪ (p != NULL)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    ᮚᮦᮔ᮪ (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    ᮜᮤᮚ
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    ᮜᮤᮚ ᮚᮦᮔ᮪ (regname == '=')
    {
	p = get_expr_line();
	ᮚᮦᮔ᮪ (p == NULL)
	    ᮝᮜᮤ FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    ᮜᮤᮚ ᮚᮦᮔ᮪ (regname == '.')		// use last inserted text
    {
	p = get_last_insert_save();
	ᮚᮦᮔ᮪ (p == NULL)
	{
	    emsg(_(e_no_inserted_text_yet));
	    ᮝᮜᮤ FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    ᮜᮤᮚ
    {
	get_yank_register(regname, FALSE);
	ᮚᮦᮔ᮪ (y_current->y_array == NULL)
	    ᮝᮜᮤ FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	for (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    puur_naa_nka	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    ᮚᮦᮔ᮪ (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		ᮚᮦᮔ᮪ (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    ᮝᮜᮤ FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    ᮚᮦᮔ᮪ (colon && i > 0)
	    {
		p = skipwhite(str);
		ᮚᮦᮔ᮪ (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_line_continuation(y_current->y_array, &i);
		    ᮚᮦᮔ᮪ (str == NULL)
			ᮝᮜᮤ FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    ᮚᮦᮔ᮪ (free_str)
		vim_free(str);
	    ᮚᮦᮔ᮪ (escaped == NULL)
		ᮝᮜᮤ FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    ᮚᮦᮔ᮪ (retval == FAIL)
		ᮝᮜᮤ FAIL;
	    ᮚᮦᮔ᮪ (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		ᮝᮜᮤ FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    ᮝᮜᮤ retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    static void
put_reedit_in_typebuf(puur_naa_nka silent)
{
    ᮚᮦᮔ᮪ (restart_edit == NUL)
	ᮝᮜᮤ;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    ᮚᮦᮔ᮪ (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    ᮜᮤᮚ ᮚᮦᮔ᮪ (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    ᮜᮤᮚ ᮚᮦᮔ᮪ (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    ᮚᮦᮔ᮪ (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    static puur_naa_nka
put_in_typebuf(
    char_u	*s,
    puur_naa_nka		esc,
    puur_naa_nka		colon,	    // add ':' before the line
    puur_naa_nka		silent)
{
    puur_naa_nka		retval = OK;

    put_reedit_in_typebuf(silent);
    ᮚᮦᮔ᮪ (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    ᮚᮦᮔ᮪ (retval == OK)
    {
	char_u	*p;

	ᮚᮦᮔ᮪ (esc)
	    p = vim_strsave_escape_csi(s);
	ᮜᮤᮚ
	    p = s;
	ᮚᮦᮔ᮪ (p == NULL)
	    retval = FAIL;
	ᮜᮤᮚ
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	ᮚᮦᮔ᮪ (esc)
	    vim_free(p);
    }
    ᮚᮦᮔ᮪ (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    ᮝᮜᮤ retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    puur_naa_nka
insert_reg(
    puur_naa_nka		regname,
    puur_naa_nka		literally_arg)	// insert literally, not as if typed
{
    long	i;
    puur_naa_nka		retval = OK;
    char_u	*arg;
    puur_naa_nka		allocated;
    puur_naa_nka		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    ᮚᮦᮔ᮪ (got_int)
	ᮝᮜᮤ FAIL;

    // check for valid regname
    ᮚᮦᮔ᮪ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ᮝᮜᮤ FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    ᮚᮦᮔ᮪ (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	ᮚᮦᮔ᮪ (arg == NULL)
	    ᮝᮜᮤ FAIL;
	stuffescaped(arg, literally);
	ᮚᮦᮔ᮪ (allocated)
	    vim_free(arg);
    }
    ᮜᮤᮚ				// name or number register
    {
	ᮚᮦᮔ᮪ (get_yank_register(regname, FALSE))
	    literally = TRUE;
	ᮚᮦᮔ᮪ (y_current->y_array == NULL)
	    retval = FAIL;
	ᮜᮤᮚ
	{
	    for (i = 0; i < y_current->y_size; ++i)
	    {
		ᮚᮦᮔ᮪ (regname == '-' && y_current->y_type == MCHAR)
		{
		    puur_naa_nka dir = BACKWARD;
		    ᮚᮦᮔ᮪ ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			ᮚᮦᮔ᮪ (u_save_cursor() == FAIL)
			    ᮝᮜᮤ FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			ᮚᮦᮔ᮪ (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, NULL, dir, 1L, PUT_CURSEND);
		}
		ᮜᮤᮚ
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    ᮚᮦᮔ᮪ (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    ᮝᮜᮤ retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    puur_naa_nka
get_spec_reg(
    puur_naa_nka		regname,
    char_u	**argp,
    puur_naa_nka		*allocated,	// return: TRUE when value was allocated
    puur_naa_nka		errmsg)		// give error message when failing
{
    puur_naa_nka		cnt;

    *argp = NULL;
    *allocated = FALSE;
    switch (regname)
    {
	pak_sa '%':		// file name
	    ᮚᮦᮔ᮪ (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    ᮝᮜᮤ TRUE;

	pak_sa '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    ᮝᮜᮤ TRUE;

#ifdef FEAT_EVAL
	pak_sa '=':		// result of expression
	    *argp = get_expr_line();
	    *allocated = TRUE;
	    ᮝᮜᮤ TRUE;
#endif

	pak_sa ':':		// last command line
	    ᮚᮦᮔ᮪ (last_cmdline == NULL && errmsg)
		emsg(_(e_no_previous_command_line));
	    *argp = last_cmdline;
	    ᮝᮜᮤ TRUE;

	pak_sa '/':		// last search-pattern
	    ᮚᮦᮔ᮪ (last_search_pat() == NULL && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_search_pat();
	    ᮝᮜᮤ TRUE;

	pak_sa '.':		// last inserted text
	    *argp = get_last_insert_save();
	    *allocated = TRUE;
	    ᮚᮦᮔ᮪ (*argp == NULL && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    ᮝᮜᮤ TRUE;

	pak_sa Ctrl_F:		// Filename under cursor
	pak_sa Ctrl_P:		// Path under cursor, expand via "path"
	    ᮚᮦᮔ᮪ (!errmsg)
		ᮝᮜᮤ FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, NULL);
	    *allocated = TRUE;
	    ᮝᮜᮤ TRUE;

	pak_sa Ctrl_W:		// word under cursor
	pak_sa Ctrl_A:		// WORD (mnemonic All) under cursor
	    ᮚᮦᮔ᮪ (!errmsg)
		ᮝᮜᮤ FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : NULL;
	    *allocated = TRUE;
	    ᮝᮜᮤ TRUE;

	pak_sa Ctrl_L:		// Line under cursor
	    ᮚᮦᮔ᮪ (!errmsg)
		ᮝᮜᮤ FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    ᮝᮜᮤ TRUE;

	pak_sa '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    ᮝᮜᮤ TRUE;
    }

    ᮝᮜᮤ FALSE;
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
    puur_naa_nka
cmdline_paste_reg(
    puur_naa_nka regname,
    puur_naa_nka literally_arg,	// Insert text literally instead of "as typed"
    puur_naa_nka remcr)		// don't add CR characters
{
    long	i;
    puur_naa_nka		literally = literally_arg;

    ᮚᮦᮔ᮪ (get_yank_register(regname, FALSE))
	literally = TRUE;
    ᮚᮦᮔ᮪ (y_current->y_array == NULL)
	ᮝᮜᮤ FAIL;

    for (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	ᮚᮦᮔ᮪ ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	ᮚᮦᮔ᮪ (got_int)
	    ᮝᮜᮤ FAIL;
    }
    ᮝᮜᮤ OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    void
shift_delete_registers(void)
{
    puur_naa_nka		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    for (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    ᮚᮦᮔ᮪ (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = NULL;		// set register one to empty
}

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
    static void
add_regtype_to_dict(puur_naa_nka regname, dict_T *dict, char_u *buf, puur_naa_nka bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    switch (get_reg_type(regname, &reglen))
    {
	pak_sa MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    bha_nga;
	pak_sa MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    bha_nga;
	pak_sa MBLOCK:
	    buflen = vim_snprintf_safelen((var_na *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    bha_nga;
	default:
	    buf[0] = NUL;
	    buflen = 0;
	    bha_nga;
    }
    (void)dict_add_string_len(dict, "regtype", buf, (puur_naa_nka)buflen);
}

    void
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    static puur_naa_nka	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    puur_naa_nka		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ᮚᮦᮔ᮪ (recursive)
	ᮝᮜᮤ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ᮚᮦᮔ᮪ (list == NULL)
	ᮝᮜᮤ;

    // yanked text contents
    for (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (puur_naa_nka)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    ᮚᮦᮔ᮪ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (void)dict_add_string_len(v_event, "regname", buf, (puur_naa_nka)buflen);

    // motion type: inclusive or exclusive
    (void)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (void)dict_add_string_len(v_event, "operator", buf, (puur_naa_nka)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, parimaa_na(buf));

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
	puur_naa_nka	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *insert,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	puur_naa_nka	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    static bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    puur_naa_nka		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ᮚᮦᮔ᮪ (recursive || regname == '_')
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (regname != '.' && insert == NULL
	    && reg == NULL)
	// Can happen when pasting text in normal mode in a terminal buffer
	ᮝᮜᮤ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ᮚᮦᮔ᮪ (list == NULL)
	ᮝᮜᮤ;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ᮚᮦᮔ᮪ (regname == '.')
    {
	ᮚᮦᮔ᮪ (last_insert_ga.ga_data != NULL)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_insert_ga.ga_data,
		    (puur_naa_nka)last_insert_ga.ga_len);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (insert != NULL)
    {
	list_append_string(list, insert->string, (puur_naa_nka)insert->length);
    }
    ᮜᮤᮚ
    {
	for (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (puur_naa_nka)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    ᮚᮦᮔ᮪ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (void)dict_add_string_len(v_event, "regname", buf, (puur_naa_nka)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (void)dict_add_string_len(v_event, "operator", buf, (puur_naa_nka)buflen);

    add_regtype_to_dict(regname, v_event, buf, parimaa_na(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (void)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_items_ro(v_event);

    recursive = true;
    textlock++;
    ᮚᮦᮔ᮪ (post)
	apply_autocmds(EVENT_TEXTPUTPOST, NULL, NULL, FALSE, curbuf);
    ᮜᮤᮚ
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
    puur_naa_nka		i;

    for (i = 0; i < NUM_REGISTERS; ++i)
	y_regs[i].y_array = NULL;
}

#ᮚᮦᮔ᮪ defined(EXITFREE)
    void
clear_registers(void)
{
    puur_naa_nka		i;

    for (i = 0; i < NUM_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	ᮚᮦᮔ᮪ (y_current->y_array != NULL)
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
    ᮚᮦᮔ᮪ (y_current->y_array == NULL)
	ᮝᮜᮤ;

    long	    i;

    for (i = n; --i >= 0; )
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
    puur_naa_nka
op_yank(oparg_T *oap, puur_naa_nka deleting, puur_naa_nka mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    puur_naa_nka			yanktype = oap->motion_type;
    long		yanklines = oap->line_count;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    sa_nracanaa block_def	bd;

				    // check for read-only register
    ᮚᮦᮔ᮪ (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	ᮝᮜᮤ FAIL;
    }
    ᮚᮦᮔ᮪ (oap->regname == '_')	    // black hole: nothing to do
	ᮝᮜᮤ OK;

#ifdef FEAT_CLIPBOARD
    ᮚᮦᮔ᮪ ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	msg_warn_missing_clipboard();
    }
#endif

    ᮚᮦᮔ᮪ (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    ᮚᮦᮔ᮪ (y_append && y_current->y_array != NULL)
	y_current = &newreg;
    ᮜᮤᮚ
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    ᮚᮦᮔ᮪ (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(parimaa_na(string_T) * yanklines, TRUE);
    ᮚᮦᮔ᮪ (y_current->y_array == NULL)
    {
	y_current = curr;
	ᮝᮜᮤ FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    ᮚᮦᮔ᮪ (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	ᮚᮦᮔ᮪ (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    for ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	switch (y_current->y_type)
	{
	    pak_sa MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		ᮚᮦᮔ᮪ (yank_copy_line(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    goto fail;
		bha_nga;

	    pak_sa MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		ᮚᮦᮔ᮪ ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == NULL)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    goto fail;
		}
		bha_nga;

	    pak_sa MCHAR:
		{
		    puur_naa_nka tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (puur_naa_nka)STRLEN(bd.textstart);
		    ᮚᮦᮔ᮪ (tmp < bd.textlen)
			bd.textlen = tmp;

		    ᮚᮦᮔ᮪ (yank_copy_line(&bd, y_idx, FALSE) == FAIL)
			goto fail;
		    bha_nga;
		}
		// NOTREACHED
	}
    }

    ᮚᮦᮔ᮪ (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	ᮚᮦᮔ᮪ (new_ptr == NULL)
	    goto fail;
	for (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	ᮚᮦᮔ᮪ (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	ᮚᮦᮔ᮪ (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == NULL)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    ᮚᮦᮔ᮪ (pnew == NULL)
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
	ᮜᮤᮚ
	    y_idx = 0;
	ᮞᮨᮓᮨᮜ (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    ᮚᮦᮔ᮪ (mess)			// Display message about yank?
    {
	ᮚᮦᮔ᮪ (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	ᮚᮦᮔ᮪ (yanklines > p_report)
	{
	    var_na namebuf[100];

	    ᮚᮦᮔ᮪ (oap->regname == NUL)
		*namebuf = NUL;
	    ᮜᮤᮚ
		vim_snprintf(namebuf, parimaa_na(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    ᮚᮦᮔ᮪ (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    ᮜᮤᮚ
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    ᮚᮦᮔ᮪ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	ᮚᮦᮔ᮪ (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	ᮚᮦᮔ᮪ (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    ᮚᮦᮔ᮪ (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	ᮚᮦᮔ᮪ (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    ᮚᮦᮔ᮪ (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	ᮚᮦᮔ᮪ (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	ᮚᮦᮔ᮪ (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    ᮚᮦᮔ᮪ (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# ᮚᮦᮔ᮪ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	ᮚᮦᮔ᮪ (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    ᮚᮦᮔ᮪ (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
    ᮚᮦᮔ᮪ (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    ᮝᮜᮤ OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    ᮝᮜᮤ FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    static puur_naa_nka
yank_copy_line(sa_nracanaa block_def *bd, long y_idx, puur_naa_nka exclude_trailing_space)
{
    char_u	*pnew;

    ᮚᮦᮔ᮪ (exclude_trailing_space)
	bd->endspaces = 0;
    ᮚᮦᮔ᮪ ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == NULL)
	ᮝᮜᮤ FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    ᮚᮦᮔ᮪ (exclude_trailing_space)
    {
	puur_naa_nka s = bd->textlen + bd->endspaces;

	ᮞᮨᮓᮨᮜ (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    ᮝᮜᮤ OK;
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
				    parimaa_na(string_T) * y_current->y_size, TRUE);
    ᮚᮦᮔ᮪ (y_current->y_array == NULL)
	y_current->y_size = 0;
    ᮜᮤᮚ
	for (j = 0; j < y_current->y_size; ++j)
	{
	    ᮚᮦᮔ᮪ ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == NULL)
	    {
		free_yank(j);
		y_current->y_size = 0;
		bha_nga;
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
    puur_naa_nka		regname,
    char_u	*expr_result,	// result for regname "=" when compiled
    puur_naa_nka		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	count,
    puur_naa_nka		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    puur_naa_nka		yanklen;
    puur_naa_nka		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    puur_naa_nka		y_type;
    long	y_size;
    puur_naa_nka		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = NULL;
    yankreg_T	*y_current_used = NULL;
    long	nr_lines = 0;
    string_T	insert_string;
    puur_naa_nka		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned puur_naa_nka cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(void)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    ᮚᮦᮔ᮪ (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    ᮚᮦᮔ᮪ (regname == '.')
    {
	ᮚᮦᮔ᮪ (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	ᮚᮦᮔ᮪ (has_textput_events)
	    add_last_insert++;
#endif

	(void)stuff_inserted((dir == FORWARD ? (count == -1 ? 'o' : 'a') :
				    (count == -1 ? 'O' : 'i')), count, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	ᮚᮦᮔ᮪ (has_textputpre())
	    put_do_autocmd('.', NULL, NULL, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	ᮚᮦᮔ᮪ (has_textputpost())
	    put_do_autocmd('.', NULL, NULL, true, dir);

	ᮚᮦᮔ᮪ (has_textput_events && --add_last_insert == 0)
	    ga_clear(&last_insert_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	ᮚᮦᮔ᮪ ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	ᮝᮜᮤ;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    insert_string.string = NULL;
    ᮚᮦᮔ᮪ (regname == '=' && expr_result != NULL)
	insert_string.string = expr_result;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (get_spec_reg(regname, &insert_string.string, &allocated, TRUE)
		&& insert_string.string == NULL)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	ᮝᮜᮤ;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    ᮚᮦᮔ᮪ (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	goto end;

    ᮚᮦᮔ᮪ (insert_string.string != NULL)
    {
	insert_string.length = STRLEN(insert_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	ᮚᮦᮔ᮪ (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    for (;;)
	    {
		y_size = 0;
		ptr = insert_string.string;
		ptrlen = insert_string.length;
		ᮞᮨᮓᮨᮜ (ptr != NULL)
		{
		    ᮚᮦᮔ᮪ (y_array != NULL)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    ᮚᮦᮔ᮪ (tmp == NULL)
		    {
			ᮚᮦᮔ᮪ (y_array != NULL)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    ᮜᮤᮚ
		    {
			ᮚᮦᮔ᮪ (y_array != NULL)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			ᮚᮦᮔ᮪ (*tmp == NUL)
			{
			    y_type = MLINE;
			    bha_nga;
			}
		    }
		    ptr = tmp;
		}
		ᮚᮦᮔ᮪ (y_array != NULL)
		    bha_nga;
		y_array = ALLOC_MULT(string_T, y_size);
		ᮚᮦᮔ᮪ (y_array == NULL)
		    goto end;
	    }
	}
	ᮜᮤᮚ
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &insert_string;
	}
#ifdef FEAT_EVAL
	ᮚᮦᮔ᮪ (has_textputpre())
	    put_do_autocmd(regname, NULL, &insert_string, false, dir);
#endif
    }
    ᮜᮤᮚ
    {
#ifdef FEAT_EVAL
	ᮚᮦᮔ᮪ (has_textputpre())
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

    ᮚᮦᮔ᮪ (y_type == MLINE)
    {
	ᮚᮦᮔ᮪ (flags & PUT_LINE_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    ᮚᮦᮔ᮪ (u_save_cursor() == FAIL)
		goto end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    ᮚᮦᮔ᮪ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    ᮚᮦᮔ᮪ (ptr == NULL)
		goto end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    ᮚᮦᮔ᮪ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    ᮚᮦᮔ᮪ (ptr == NULL)
		goto end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_lines;
	    dir = FORWARD;
	}
	ᮚᮦᮔ᮪ (flags & PUT_LINE_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    ᮚᮦᮔ᮪ (flags & PUT_LINE)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    ᮚᮦᮔ᮪ (y_size == 0 || y_array == NULL)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	goto end;
    }

    ᮚᮦᮔ᮪ (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	ᮚᮦᮔ᮪ (lnum > curbuf->b_ml.ml_line_count)
	    lnum = curbuf->b_ml.ml_line_count + 1;
	ᮚᮦᮔ᮪ (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    goto end;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	ᮚᮦᮔ᮪ (dir == BACKWARD)
	    (void)hasFolding(lnum, &lnum, NULL);
	ᮜᮤᮚ
	    (void)hasFolding(lnum, NULL, &lnum);
#endif
	ᮚᮦᮔ᮪ (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	ᮚᮦᮔ᮪ ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    goto end;
#ifdef FEAT_FOLDING
	ᮚᮦᮔ᮪ (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	ᮜᮤᮚ
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (u_save_cursor() == FAIL)
	goto end;

    ᮚᮦᮔ᮪ (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	ᮚᮦᮔ᮪ (gchar_cursor() == TAB)
	{
	    puur_naa_nka viscol = getviscol();
	    puur_naa_nka ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    ᮚᮦᮔ᮪ (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#ᮜᮤᮚ
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    ᮜᮤᮚ
		curwin->w_cursor.coladd = 0;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    ᮚᮦᮔ᮪ (y_type == MBLOCK)
    {
	puur_naa_nka	delcount;
	puur_naa_nka	incr = 0;
	sa_nracanaa block_def bd;
	long	j;
	puur_naa_nka	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	ᮚᮦᮔ᮪ (dir == FORWARD && c != NUL)
	{
	    ᮚᮦᮔ᮪ (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, NULL, &endcol2, 0);
	    ᮜᮤᮚ
		getvcol(curwin, &curwin->w_cursor, NULL, NULL, &col, 0);

	    ᮚᮦᮔ᮪ (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    ᮜᮤᮚ
		ᮚᮦᮔ᮪ (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	ᮜᮤᮚ
	    getvcol(curwin, &curwin->w_cursor, &col, NULL, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	ᮚᮦᮔ᮪ (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    ᮚᮦᮔ᮪ (dir == FORWARD && c == NUL)
		++col;
	    ᮚᮦᮔ᮪ (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    ᮚᮦᮔ᮪ (c == TAB)
	    {
		ᮚᮦᮔ᮪ (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		ᮚᮦᮔ᮪ (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	for (i = 0; i < y_size; ++i)
	{
	    puur_naa_nka		    spaces = 0;
	    var_na	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    ᮚᮦᮔ᮪ (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	    {
		ᮚᮦᮔ᮪ (ml_append(curbuf->b_ml.ml_line_count, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    bha_nga;
		++nr_lines;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    ᮞᮨᮓᮨᮜ (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    ᮚᮦᮔ᮪ (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		ᮚᮦᮔ᮪ (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		ᮚᮦᮔ᮪ (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (puur_naa_nka)y_array[i].length;

	    ᮚᮦᮔ᮪ ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		ᮞᮨᮓᮨᮜ (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		ᮚᮦᮔ᮪ (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    ᮚᮦᮔ᮪ (yanklen + spaces != 0
		     && count > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		bha_nga;
	    }

	    totlen = count * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    ᮚᮦᮔ᮪ (newp == NULL)
		bha_nga;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    for (j = 0; j < count; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		ᮚᮦᮔ᮪ ((j < count - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		ᮜᮤᮚ
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
	    ᮚᮦᮔ᮪ (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_lines(lnum, 0, curwin->w_cursor.lnum - nr_lines, nr_lines);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	ᮚᮦᮔ᮪ (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	ᮚᮦᮔ᮪ (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    ᮚᮦᮔ᮪ (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	ᮜᮤᮚ
	    curwin->w_cursor.lnum = lnum;
    }
    ᮜᮤᮚ
    {
	pos_T	new_cursor;

	yanklen = (puur_naa_nka)y_array[0].length;

	// Character or Line mode
	ᮚᮦᮔ᮪ (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    ᮚᮦᮔ᮪ (dir == FORWARD && gchar_cursor() != NUL)
	    {
		ᮚᮦᮔ᮪ (has_mbyte)
		{
		    puur_naa_nka bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    ᮚᮦᮔ᮪ (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		ᮜᮤᮚ
		{
		    ++col;
		    ᮚᮦᮔ᮪ (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	ᮜᮤᮚ ᮚᮦᮔ᮪ (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	ᮚᮦᮔ᮪ (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    puur_naa_nka		first_byte_off = 0;

	    ᮚᮦᮔ᮪ (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		ᮚᮦᮔ᮪ (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		ᮚᮦᮔ᮪ (end_lnum > start_lnum)
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

	    ᮚᮦᮔ᮪ (count == 0 || yanklen == 0)
	    {
		ᮚᮦᮔ᮪ (VIsual_active)
		    lnum = end_lnum;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (count > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    ᮜᮤᮚ
	    {
		totlen = count * yanklen;
		do
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    ᮚᮦᮔ᮪ (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			ᮚᮦᮔ᮪ (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			ᮜᮤᮚ
			    col = MAXCOL;
		    }
		    ᮚᮦᮔ᮪ (VIsual_active && col > oldlen)
		    {
			lnum++;
			continue;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    ᮚᮦᮔ᮪ (newp == NULL)
			goto end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    for (i = 0; i < count; ++i)
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
		    ᮚᮦᮔ᮪ (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    ᮚᮦᮔ᮪ (VIsual_active)
			lnum++;
		} ᮞᮨᮓᮨᮜ (VIsual_active && lnum <= end_lnum);

		ᮚᮦᮔ᮪ (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    ᮚᮦᮔ᮪ (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    ᮜᮤᮚ
		curwin->w_cursor.col -= first_byte_off;
	}
	ᮜᮤᮚ
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    puur_naa_nka		indent;
	    puur_naa_nka		orig_indent = 0;
	    puur_naa_nka		indent_diff = 0;	// init for gcc
	    puur_naa_nka		first_indent = TRUE;
	    puur_naa_nka		lendiff = 0;
	    long	cnt;

	    ᮚᮦᮔ᮪ (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    for (cnt = 1; cnt <= count; ++cnt)
	    {
		i = 0;
		ᮚᮦᮔ᮪ (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (puur_naa_nka)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    ᮚᮦᮔ᮪ (newp == NULL)
			goto error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    ᮚᮦᮔ᮪ (newp == NULL)
			goto error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		for (; i < y_size; ++i)
		{
		    ᮚᮦᮔ᮪ (y_type != MCHAR || i < y_size - 1)
		    {
			ᮚᮦᮔ᮪ (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    goto error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_lines;
		    ᮚᮦᮔ᮪ (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			ᮚᮦᮔ᮪ (cnt == count && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			ᮚᮦᮔ᮪ (*ptr == '#' && preprocs_left())
			    indent = 0;     // Leave # lines at start
			ᮜᮤᮚ ᮚᮦᮔ᮪ (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			ᮜᮤᮚ ᮚᮦᮔ᮪ (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			ᮜᮤᮚ ᮚᮦᮔ᮪ ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(void)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			ᮚᮦᮔ᮪ (cnt == count && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		ᮚᮦᮔ᮪ (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    ᮚᮦᮔ᮪ (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		ᮚᮦᮔ᮪ (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_lines, 0L);

	    // note changed text for displaying and folding
	    ᮚᮦᮔ᮪ (y_type == MCHAR)
		changed_lines(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_lines);
	    ᮜᮤᮚ
		changed_lines(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_lines);
	    ᮚᮦᮔ᮪ (y_current_used != NULL && (y_current_used != y_current
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
	    ᮚᮦᮔ᮪ (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		ᮚᮦᮔ᮪ (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    ᮜᮤᮚ
		curbuf->b_op_end.col = 0;

	    ᮚᮦᮔ᮪ (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		ᮚᮦᮔ᮪ (y_type == MLINE)
		{
		    ᮚᮦᮔ᮪ (lnum >= curbuf->b_ml.ml_line_count)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		    ᮜᮤᮚ
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		ᮜᮤᮚ
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    ᮚᮦᮔ᮪ (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		ᮚᮦᮔ᮪ (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ᮜᮤᮚ	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_lines);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    puur_naa_nka len = ml_get_curline_len();
    ᮚᮦᮔ᮪ (curwin->w_cursor.col > len)
    {
	ᮚᮦᮔ᮪ (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    ᮚᮦᮔ᮪ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    ᮚᮦᮔ᮪ (has_textputpost())
    {
	ᮚᮦᮔ᮪ (insert_string.string == NULL)
	    put_do_autocmd(regname, y_current, NULL, true, dir);
	ᮜᮤᮚ
	    put_do_autocmd(regname, NULL, &insert_string, true, dir);
    }
#endif

    ᮚᮦᮔ᮪ (allocated)
	vim_free(insert_string.string);
    ᮚᮦᮔ᮪ (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    puur_naa_nka
get_register_name(puur_naa_nka num)
{
    ᮚᮦᮔ᮪ (num == -1)
	ᮝᮜᮤ '"';
    ᮜᮤᮚ ᮚᮦᮔ᮪ (num < 10)
	ᮝᮜᮤ num + '0';
    ᮜᮤᮚ ᮚᮦᮔ᮪ (num == DELETION_REGISTER)
	ᮝᮜᮤ '-';
#ᮚᮦᮔ᮪ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    ᮜᮤᮚ ᮚᮦᮔ᮪ (num == STAR_REGISTER)
	ᮝᮜᮤ '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    ᮜᮤᮚ ᮚᮦᮔ᮪ (clipmethod == CLIPMETHOD_PROVIDER && num == REAL_PLUS_REGISTER)
	ᮝᮜᮤ '+';
    ᮜᮤᮚ ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER && num == PLUS_REGISTER)
	ᮝᮜᮤ '+';
#endif
    ᮜᮤᮚ
	ᮝᮜᮤ num + 'a' - 10;
}

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    puur_naa_nka
get_unname_register(void)
{
    ᮝᮜᮤ y_previous == NULL ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    void
ex_display(exarg_T *eap)
{
    puur_naa_nka		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    puur_naa_nka		name;
    puur_naa_nka		attr;
    char_u	*arg = eap->arg;
    puur_naa_nka		clen;
    puur_naa_nka		type;
    string_T	insert;

    silence_w23_w24_msg++;
    ᮚᮦᮔ᮪ (arg != NULL && *arg == NUL)
	arg = NULL;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    msg_puts_title(_("\nType Name Content"));
    for (i = -1; i < NUM_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	switch (get_reg_type(name, NULL))
	{
	    pak_sa MLINE: type = 'l'; bha_nga;
	    pak_sa MCHAR: type = 'c'; bha_nga;
	    default:	type = 'b'; bha_nga;
	}
	ᮚᮦᮔ᮪ (arg != NULL && vim_strchr(arg, name) == NULL
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
	ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (void)may_get_selection(name);
#endif

	ᮚᮦᮔ᮪ (i == -1)
	{
	    ᮚᮦᮔ᮪ (y_previous != NULL)
		yb = y_previous;
	    ᮜᮤᮚ
		yb = &(y_regs[0]);
	}
	ᮜᮤᮚ
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	ᮚᮦᮔ᮪ (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != NULL &&
		    (yb == y_previous || yb == &y_regs[0])))
	    continue;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	ᮚᮦᮔ᮪ (yb->y_array != NULL)
	{
	    puur_naa_nka do_show = FALSE;

	    for (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    ᮚᮦᮔ᮪ (do_show || yb->y_size == 0)
	    {
		msg_putchar('\n');
		msg_puts("  ");
		msg_putchar(type);
		msg_puts("  ");
		msg_putchar('"');
		msg_putchar(name);
		msg_puts("   ");

		n = (puur_naa_nka)Columns - 11;
		for (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    ᮚᮦᮔ᮪ (j)
		    {
			msg_puts_attr("^J", attr);
			n -= 2;
		    }
		    for (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			msg_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		ᮚᮦᮔ᮪ (n > 1 && yb->y_type == MLINE)
		    msg_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    insert = get_last_insert();
    ᮚᮦᮔ᮪ ((p = insert.string) != NULL
		  && (arg == NULL || vim_strchr(arg, '.') != NULL) && !got_int
						      && !message_filtered(p))
    {
	msg_puts("\n  c  \".   ");
	dis_msg(p, TRUE);
    }

    // display last command line
    ᮚᮦᮔ᮪ (last_cmdline != NULL && (arg == NULL || vim_strchr(arg, ':') != NULL)
			       && !got_int && !message_filtered(last_cmdline))
    {
	msg_puts("\n  c  \":   ");
	dis_msg(last_cmdline, FALSE);
    }

    // display current file name
    ᮚᮦᮔ᮪ (curbuf->b_fname != NULL
	    && (arg == NULL || vim_strchr(arg, '%') != NULL) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	msg_puts("\n  c  \"%   ");
	dis_msg(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    ᮚᮦᮔ᮪ ((arg == NULL || vim_strchr(arg, '#') != NULL) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	ᮚᮦᮔ᮪ (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    msg_puts("\n  c  \"#   ");
	    dis_msg(fname, FALSE);
	}
    }

    // display last search pattern
    ᮚᮦᮔ᮪ (last_search_pat() != NULL
		 && (arg == NULL || vim_strchr(arg, '/') != NULL) && !got_int
				      && !message_filtered(last_search_pat()))
    {
	msg_puts("\n  c  \"/   ");
	dis_msg(last_search_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    ᮚᮦᮔ᮪ (expr_line != NULL && (arg == NULL || vim_strchr(arg, '=') != NULL)
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
    puur_naa_nka		skip_esc)	    // if TRUE, ignore trailing ESC
{
    puur_naa_nka		n;
    puur_naa_nka		l;

    n = (puur_naa_nka)Columns - 6;
    ᮞᮨᮓᮨᮜ (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	ᮚᮦᮔ᮪ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    msg_outtrans_len(p, l);
	    p += l;
	}
	ᮜᮤᮚ
	    msg_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#ᮚᮦᮔ᮪ defined(FEAT_DND)
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
get_reg_type(puur_naa_nka regname, long *reglen)
{
    switch (regname)
    {
	pak_sa '%':		// file name
	pak_sa '#':		// alternate file name
	pak_sa '=':		// expression
	pak_sa ':':		// last command line
	pak_sa '/':		// last search-pattern
	pak_sa '.':		// last inserted text
	pak_sa Ctrl_F:		// Filename under cursor
	pak_sa Ctrl_P:		// Path under cursor, expand via "path"
	pak_sa Ctrl_W:		// word under cursor
	pak_sa Ctrl_A:		// WORD (mnemonic All) under cursor
	pak_sa '_':		// black hole: always empty
	    ᮝᮜᮤ MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    ᮚᮦᮔ᮪ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ᮝᮜᮤ MAUTO;

    get_yank_register(regname, FALSE);

    ᮚᮦᮔ᮪ (y_current->y_array != NULL)
    {
	ᮚᮦᮔ᮪ (reglen != NULL && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	ᮝᮜᮤ y_current->y_type;
    }
    ᮝᮜᮤ MAUTO;
}

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    static char_u *
getreg_wrap_one_line(char_u *s, puur_naa_nka flags)
{
    ᮚᮦᮔ᮪ (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	ᮚᮦᮔ᮪ (list != NULL)
	{
	    ᮚᮦᮔ᮪ (list_append_string(list, NULL, -1) == FAIL)
	    {
		list_free(list);
		ᮝᮜᮤ NULL;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	ᮝᮜᮤ (char_u *)list;
    }
    ᮝᮜᮤ s;
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
get_reg_contents(puur_naa_nka regname, puur_naa_nka flags)
{
    linenr_T	i;
    char_u	*retval;
    puur_naa_nka		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    ᮚᮦᮔ᮪ (regname == '=')
    {
	ᮚᮦᮔ᮪ (flags & GREG_NO_EXPR)
	    ᮝᮜᮤ NULL;
	ᮚᮦᮔ᮪ (flags & GREG_EXPR_SRC)
	    ᮝᮜᮤ getreg_wrap_one_line(get_expr_line_src(), flags);
	ᮝᮜᮤ getreg_wrap_one_line(get_expr_line(), flags);
    }

    ᮚᮦᮔ᮪ (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    ᮚᮦᮔ᮪ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ᮝᮜᮤ NULL;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ᮚᮦᮔ᮪ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ᮚᮦᮔ᮪ (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	ᮚᮦᮔ᮪ (retval == NULL)
	    ᮝᮜᮤ NULL;
	ᮚᮦᮔ᮪ (allocated)
	    ᮝᮜᮤ getreg_wrap_one_line(retval, flags);
	ᮝᮜᮤ getreg_wrap_one_line(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    ᮚᮦᮔ᮪ (y_current->y_array == NULL)
	ᮝᮜᮤ NULL;

    ᮚᮦᮔ᮪ (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	ᮚᮦᮔ᮪ (list == NULL)
	    ᮝᮜᮤ NULL;
	for (i = 0; i < y_current->y_size; ++i)
	{
	    ᮚᮦᮔ᮪ (list_append_string(list, y_current->y_array[i].string,
		(puur_naa_nka)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		ᮝᮜᮤ NULL;
	    }
	}
	ᮝᮜᮤ (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    for (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ᮚᮦᮔ᮪ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    ᮚᮦᮔ᮪ (retval == NULL)
	ᮝᮜᮤ NULL;

    // Copy the lines of the yank register into the string.
    len = 0;
    for (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ᮚᮦᮔ᮪ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    ᮝᮜᮤ retval;
}

    static puur_naa_nka
init_write_reg(
    puur_naa_nka		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    puur_naa_nka		must_append,
    puur_naa_nka		*yank_type UNUSED)
{
    ᮚᮦᮔ᮪ (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	ᮝᮜᮤ FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    ᮚᮦᮔ᮪ (!y_append && !must_append)
	free_yank_all();
    ᮝᮜᮤ OK;
}

    static void
finish_write_reg(
    puur_naa_nka		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    ᮚᮦᮔ᮪ (name != '"')
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
    puur_naa_nka		name,
    char_u	*str,
    puur_naa_nka		maxlen,
    puur_naa_nka		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    void
write_reg_contents_lst(
    puur_naa_nka		name,
    char_u	**strings,
    puur_naa_nka		maxlen UNUSED,
    puur_naa_nka		must_append,
    puur_naa_nka		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    ᮚᮦᮔ᮪ (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	ᮚᮦᮔ᮪ (strings[0] == NULL)
	    s = (char_u *)"";
	ᮜᮤᮚ ᮚᮦᮔ᮪ (strings[1] != NULL)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_lines), name);
	    ᮝᮜᮤ;
	}
	ᮜᮤᮚ
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	ᮝᮜᮤ;
    }

    ᮚᮦᮔ᮪ (name == '_')	    // black hole: nothing to do
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ᮝᮜᮤ;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    void
write_reg_contents_ex(
    puur_naa_nka		name,
    char_u	*str,
    puur_naa_nka		maxlen,
    puur_naa_nka		must_append,
    puur_naa_nka		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    ᮚᮦᮔ᮪ (maxlen >= 0)
	len = maxlen;
    ᮜᮤᮚ
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    ᮚᮦᮔ᮪ (name == '/')
    {
	set_last_search_pat(str, RE_SEARCH, TRUE, TRUE);
	ᮝᮜᮤ;
    }

    ᮚᮦᮔ᮪ (name == '#')
    {
	ᮚᮦᮔ᮪ (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  ᮝᮜᮤ;
	}

	buf_T	*buf;

	ᮚᮦᮔ᮪ (VIM_ISDIGIT(*str))
	{
	    puur_naa_nka	num = atoi((var_na *)str);

	    buf = buflist_findnr(num);
	    ᮚᮦᮔ᮪ (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist), (long)num);
	}
	ᮜᮤᮚ
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	ᮚᮦᮔ᮪ (buf == NULL)
	    ᮝᮜᮤ;
	curwin->w_alt_fnum = buf->b_fnum;
	ᮝᮜᮤ;
    }

    ᮚᮦᮔ᮪ (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	ᮚᮦᮔ᮪ (p == NULL)
	    ᮝᮜᮤ;
	ᮚᮦᮔ᮪ (must_append && expr_line != NULL)
	{
	    s = concat_str(expr_line, p);
	    vim_free(p);
	    p = s;
	}
	set_expr_line(p, NULL);
	ᮝᮜᮤ;
    }

    ᮚᮦᮔ᮪ (name == '_')	    // black hole: nothing to do
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ᮝᮜᮤ;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#ᮚᮦᮔ᮪ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    void
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    puur_naa_nka		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    puur_naa_nka		str_list)	// TRUE if str is char_u **
{
    puur_naa_nka		type;			// MCHAR, MLINE or MBLOCK
    puur_naa_nka		lnum;
    long	start;
    long	i;
    puur_naa_nka		extra;
    puur_naa_nka		newlines;		// number of lines added
    puur_naa_nka		extraline = 0;		// extra line at the end
    puur_naa_nka		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    ᮚᮦᮔ᮪ (y_ptr->y_array == NULL)		// NULL means empty register
	y_ptr->y_size = 0;

    ᮚᮦᮔ᮪ (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    ᮜᮤᮚ
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    ᮚᮦᮔ᮪ (str_list)
    {
	for (ss = (char_u **) str; *ss != NULL; ++ss)
	    ++newlines;
    }
    ᮜᮤᮚ
    {
	for (i = 0; i < len; i++)
	    ᮚᮦᮔ᮪ (str[i] == '\n')
		++newlines;
	ᮚᮦᮔ᮪ (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	ᮚᮦᮔ᮪ (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    ᮚᮦᮔ᮪ (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	ᮝᮜᮤ;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * parimaa_na(string_T), TRUE);
    ᮚᮦᮔ᮪ (pp == NULL)	// out of memory
	ᮝᮜᮤ;
    for (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    ᮚᮦᮔ᮪ (str_list)
    {
	for (ss = (char_u **) str; *ss != NULL; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    ᮚᮦᮔ᮪ (type == MBLOCK)
	    {
		puur_naa_nka charlen = vim_strsize(*ss);

		ᮚᮦᮔ᮪ (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    ᮜᮤᮚ
    {
	for (start = 0; start < len + extraline; start += i + 1)
	{
	    puur_naa_nka charlen = 0;

	    for (i = start; i < len;)	// find the end of the line
	    {
		ᮚᮦᮔ᮪ (str[i] == '\n')
		    bha_nga;
		ᮚᮦᮔ᮪ (type == MBLOCK)
		{
		    ᮚᮦᮔ᮪ (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    ᮜᮤᮚ
			charlen += byte2cells(str[i]);
		}

		ᮚᮦᮔ᮪ (str[i] == NUL)
		    i++; // registers can have NUL chars
		ᮜᮤᮚ
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    ᮚᮦᮔ᮪ (charlen > maxlen)
		maxlen = charlen;
	    ᮚᮦᮔ᮪ (append)
	    {
		--lnum;
		extra = (puur_naa_nka)y_ptr->y_array[lnum].length;
	    }
	    ᮜᮤᮚ
		extra = 0;
	    s = alloc(i + extra + 1);
	    ᮚᮦᮔ᮪ (s == NULL)
		bha_nga;
	    ᮚᮦᮔ᮪ (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    ᮚᮦᮔ᮪ (append)
		vim_free(y_ptr->y_array[lnum].string);
	    ᮚᮦᮔ᮪ (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    ᮞᮨᮓᮨᮜ (--extra >= 0)
	    {
		ᮚᮦᮔ᮪ (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    ᮚᮦᮔ᮪ (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    ᮜᮤᮚ
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
