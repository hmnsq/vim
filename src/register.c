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
static ကိန်းပြည့်		y_append;	    // TRUE when appending
static yankreg_T	*y_previous = NULL; // ptr to last written yankreg

static ကိန်းပြည့်	stuff_yank(ကိန်းပြည့်, char_u *);
static void	put_reedit_in_typebuf(ကိန်းပြည့် silent);
static ကိန်းပြည့်	put_in_typebuf(char_u *s, ကိန်းပြည့် esc, ကိန်းပြည့် colon, ကိန်းပြည့် silent);
static ကိန်းပြည့်	yank_copy_line(sa_nracanaa block_def *bd, long y_idx, ကိန်းပြည့် exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
static void	copy_yank_reg(yankreg_T *reg);
#endif
static void	dis_msg(char_u *p, ကိန်းပြည့် skip_esc);

#အကယ်၍ defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(void)
{
    ပြန်ပေး y_regs;
}
#endif

#အကယ်၍ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(ကိန်းပြည့် reg)
{
    ပြန်ပေး &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(void)
{
    ပြန်ပေး y_current;
}

    yankreg_T *
get_y_previous(void)
{
    ပြန်ပေး y_previous;
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


#အကယ်၍ defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
static char_u	*expr_line = NULL;
static exarg_T	*expr_eap = NULL;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    ကိန်းပြည့်
get_expr_register(void)
{
    char_u	*new_line;

    new_line = getcmdline('=', 0L, 0, 0);
    အကယ်၍ (new_line == NULL)
	ပြန်ပေး NUL;
    အကယ်၍ (*new_line == NUL)	// use previous line
	vim_free(new_line);
    မဟုတ်ပါက
	set_expr_line(new_line, NULL);
    ပြန်ပေး '=';
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
    static ကိန်းပြည့်	nested = 0;

    အကယ်၍ (expr_line == NULL)
	ပြန်ပေး NULL;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    expr_copy = vim_strsave(expr_line);
    အကယ်၍ (expr_copy == NULL)
	ပြန်ပေး NULL;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    အကယ်၍ (nested >= 10)
	ပြန်ပေး expr_copy;

    ++nested;
    rv = eval_to_string_eap(expr_copy, TRUE, expr_eap, FALSE);
    --nested;
    vim_free(expr_copy);
    ပြန်ပေး rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    static char_u *
get_expr_line_src(void)
{
    အကယ်၍ (expr_line == NULL)
	ပြန်ပေး NULL;
    ပြန်ပေး vim_strsave(expr_line);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    ကိန်းပြည့်
valid_yank_reg(
    ကိန်းပြည့်	    regname,
    ကိန်းပြည့်	    writing)	    // if TRUE check for writable registers
{
    အကယ်၍ (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#မဟုတ်ပါက
				    "/#.%:"
#endif
					, regname) != NULL)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#အကယ်၍ defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
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
	ပြန်ပေး TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    မဟုတ်ပါက အကယ်၍ (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	msg_warn_missing_clipboard();
	ပြန်ပေး FALSE;
    }
#endif
    ပြန်ပေး FALSE;
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
    ကိန်းပြည့်
get_yank_register(ကိန်းပြည့် regname, ကိန်းပြည့် writing)
{
    ကိန်းပြည့်	    i;
    ကိန်းပြည့်	    ret = FALSE;

    y_append = FALSE;
    အကယ်၍ ((regname == 0 || regname == '"') && !writing && y_previous != NULL)
    {
	y_current = y_previous;
	ပြန်ပေး ret;
    }
    i = regname;
    အကယ်၍ (VIM_ISDIGIT(i))
	i -= '0';
    မဟုတ်ပါက အကယ်၍ (ASCII_ISLOWER(i))
	i = CharOrdLow(i) + 10;
    မဟုတ်ပါက အကယ်၍ (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    မဟုတ်ပါက အကယ်၍ (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    မဟုတ်ပါက အကယ်၍ (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    မဟုတ်ပါက အကယ်၍ (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	အကယ်၍ (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	မဟုတ်ပါက
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#elif defined(FEAT_CLIPBOARD_PROVIDER)
    မဟုတ်ပါက အကယ်၍ (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    မဟုတ်ပါက အကယ်၍ (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    မဟုတ်ပါက အကယ်၍ (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    မဟုတ်ပါက		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    အကယ်၍ (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    ပြန်ပေး ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    void *
get_register(
    ကိန်းပြည့်		name,
    ကိန်းပြည့်		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    ကိန်းပြည့်		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	အကယ်၍ (name == '*' && clip_star.available)
	{
	    အကယ်၍ (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	အကယ်၍ (name == '+' && clip_plus.available)
	{
	    အကယ်၍ (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    အကယ်၍ (reg == NULL)
	ပြန်ပေး (void *)NULL;

    *reg = *y_current;
    အကယ်၍ (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	အကယ်၍ (reg->y_size == 0 || y_current->y_array == NULL)
	    reg->y_array = NULL;
	မဟုတ်ပါက
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	အကယ်၍ (reg->y_array != NULL)
	{
	    အတွက် (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		အကယ်၍ (reg->y_array[i].string == NULL)
		{
		    // The allocation failed so clean up and exit
		    နေစဉ် (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    ပြန်ပေး (void *)NULL;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    မဟုတ်ပါက
	y_current->y_array = NULL;
    ပြန်ပေး (void *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    void
put_register(ကိန်းပြည့် name, void *reg)
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

#အကယ်၍ defined(FEAT_CLIPBOARD)
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
    ကိန်းပြည့်
yank_register_mline(ကိန်းပြည့် regname)
{
    အကယ်၍ (regname != 0 && !valid_yank_reg(regname, FALSE))
	ပြန်ပေး FALSE;
    အကယ်၍ (regname == '_')		// black hole is always empty
	ပြန်ပေး FALSE;
    get_yank_register(regname, FALSE);
    ပြန်ပေး (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ကိန်းပြည့်
do_record(ကိန်းပြည့် c)
{
    char_u	    *p;
    static ကိန်းပြည့်	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    ကိန်းပြည့်		    retval;

    အကယ်၍ (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	အကယ်၍ (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	မဟုတ်ပါက
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    မဟုတ်ပါက			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	msg("");
	p = get_recorded();
	အကယ်၍ (p == NULL)
	    retval = FAIL;
	မဟုတ်ပါက
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
    ပြန်ပေး retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    static ကိန်းပြည့်
stuff_yank(ကိန်းပြည့် regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    အကယ်၍ (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	ပြန်ပေး FAIL;
    }
    အကယ်၍ (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	ပြန်ပေး OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    အကယ်၍ (y_append && y_current->y_array != NULL)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	အကယ်၍ (tmp == NULL)
	{
	    vim_free(p);
	    ပြန်ပေး FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    မဟုတ်ပါက
    {
	free_yank_all();
	အကယ်၍ ((y_current->y_array = ALLOC_ONE(string_T)) == NULL)
	{
	    vim_free(p);
	    ပြန်ပေး FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    ပြန်ပေး OK;
}

/*
 * Last executed register (@ command)
 */
static ကိန်းပြည့် execreg_lastc = NUL;

#အကယ်၍ defined(FEAT_VIMINFO)
    ကိန်းပြည့်
get_execreg_lastc(void)
{
    ပြန်ပေး execreg_lastc;
}

    void
set_execreg_lastc(ကိန်းပြည့် lastc)
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
    ကိန်းပြည့်		j;
    char_u	*str;

    ga_init2(&ga, parimaa_na(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    နေစဉ် (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(lines[cmd_start].string);
	အကယ်၍ (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    bha_nga;
    }

    // join all the lines
    tmp = &lines[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    အတွက် (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &lines[j];
	p = skipwhite(tmp->string);
	အကယ်၍ (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    အကယ်၍ (ga.ga_len > 400)
	    {
		အကယ်၍ (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		မဟုတ်ပါက
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
    ပြန်ပေး str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ကိန်းပြည့်
do_execreg(
    ကိန်းပြည့်	    regname,
    ကိန်းပြည့်	    colon,		// insert ':' before each line
    ကိန်းပြည့်	    addcr,		// always add '\n' to end of line
    ကိန်းပြည့်	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    ကိန်းပြည့်		retval = OK;
    ကိန်းပြည့်		remap;

    // repeat previous one
    အကယ်၍ (regname == '@')
    {
	အကယ်၍ (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    ပြန်ပေး FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    အကယ်၍ (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	ပြန်ပေး FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    အကယ်၍ (regname == '_')
	ပြန်ပေး OK;

    // use last command line
    အကယ်၍ (regname == ':')
    {
	အကယ်၍ (last_cmdline == NULL)
	{
	    emsg(_(e_no_previous_command_line));
	    ပြန်ပေး FAIL;
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
	အကယ်၍ (p != NULL)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    အကယ်၍ (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    မဟုတ်ပါက
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    မဟုတ်ပါက အကယ်၍ (regname == '=')
    {
	p = get_expr_line();
	အကယ်၍ (p == NULL)
	    ပြန်ပေး FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    မဟုတ်ပါက အကယ်၍ (regname == '.')		// use last inserted text
    {
	p = get_last_insert_save();
	အကယ်၍ (p == NULL)
	{
	    emsg(_(e_no_inserted_text_yet));
	    ပြန်ပေး FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    မဟုတ်ပါက
    {
	get_yank_register(regname, FALSE);
	အကယ်၍ (y_current->y_array == NULL)
	    ပြန်ပေး FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	အတွက် (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    ကိန်းပြည့်	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    အကယ်၍ (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		အကယ်၍ (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    ပြန်ပေး FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    အကယ်၍ (colon && i > 0)
	    {
		p = skipwhite(str);
		အကယ်၍ (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_line_continuation(y_current->y_array, &i);
		    အကယ်၍ (str == NULL)
			ပြန်ပေး FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    အကယ်၍ (free_str)
		vim_free(str);
	    အကယ်၍ (escaped == NULL)
		ပြန်ပေး FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    အကယ်၍ (retval == FAIL)
		ပြန်ပေး FAIL;
	    အကယ်၍ (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		ပြန်ပေး FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    ပြန်ပေး retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    static void
put_reedit_in_typebuf(ကိန်းပြည့် silent)
{
    အကယ်၍ (restart_edit == NUL)
	ပြန်ပေး;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    အကယ်၍ (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    မဟုတ်ပါက အကယ်၍ (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    မဟုတ်ပါက အကယ်၍ (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    အကယ်၍ (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    static ကိန်းပြည့်
put_in_typebuf(
    char_u	*s,
    ကိန်းပြည့်		esc,
    ကိန်းပြည့်		colon,	    // add ':' before the line
    ကိန်းပြည့်		silent)
{
    ကိန်းပြည့်		retval = OK;

    put_reedit_in_typebuf(silent);
    အကယ်၍ (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    အကယ်၍ (retval == OK)
    {
	char_u	*p;

	အကယ်၍ (esc)
	    p = vim_strsave_escape_csi(s);
	မဟုတ်ပါက
	    p = s;
	အကယ်၍ (p == NULL)
	    retval = FAIL;
	မဟုတ်ပါက
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	အကယ်၍ (esc)
	    vim_free(p);
    }
    အကယ်၍ (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    ပြန်ပေး retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    ကိန်းပြည့်
insert_reg(
    ကိန်းပြည့်		regname,
    ကိန်းပြည့်		literally_arg)	// insert literally, not as if typed
{
    long	i;
    ကိန်းပြည့်		retval = OK;
    char_u	*arg;
    ကိန်းပြည့်		allocated;
    ကိန်းပြည့်		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    အကယ်၍ (got_int)
	ပြန်ပေး FAIL;

    // check for valid regname
    အကယ်၍ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ပြန်ပေး FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    အကယ်၍ (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    မဟုတ်ပါက အကယ်၍ (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	အကယ်၍ (arg == NULL)
	    ပြန်ပေး FAIL;
	stuffescaped(arg, literally);
	အကယ်၍ (allocated)
	    vim_free(arg);
    }
    မဟုတ်ပါက				// name or number register
    {
	အကယ်၍ (get_yank_register(regname, FALSE))
	    literally = TRUE;
	အကယ်၍ (y_current->y_array == NULL)
	    retval = FAIL;
	မဟုတ်ပါက
	{
	    အတွက် (i = 0; i < y_current->y_size; ++i)
	    {
		အကယ်၍ (regname == '-' && y_current->y_type == MCHAR)
		{
		    ကိန်းပြည့် dir = BACKWARD;
		    အကယ်၍ ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			အကယ်၍ (u_save_cursor() == FAIL)
			    ပြန်ပေး FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			အကယ်၍ (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, NULL, dir, 1L, PUT_CURSEND);
		}
		မဟုတ်ပါက
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    အကယ်၍ (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    ပြန်ပေး retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    ကိန်းပြည့်
get_spec_reg(
    ကိန်းပြည့်		regname,
    char_u	**argp,
    ကိန်းပြည့်		*allocated,	// return: TRUE when value was allocated
    ကိန်းပြည့်		errmsg)		// give error message when failing
{
    ကိန်းပြည့်		cnt;

    *argp = NULL;
    *allocated = FALSE;
    switch (regname)
    {
	pak_sa '%':		// file name
	    အကယ်၍ (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    ပြန်ပေး TRUE;

	pak_sa '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    ပြန်ပေး TRUE;

#ifdef FEAT_EVAL
	pak_sa '=':		// result of expression
	    *argp = get_expr_line();
	    *allocated = TRUE;
	    ပြန်ပေး TRUE;
#endif

	pak_sa ':':		// last command line
	    အကယ်၍ (last_cmdline == NULL && errmsg)
		emsg(_(e_no_previous_command_line));
	    *argp = last_cmdline;
	    ပြန်ပေး TRUE;

	pak_sa '/':		// last search-pattern
	    အကယ်၍ (last_search_pat() == NULL && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_search_pat();
	    ပြန်ပေး TRUE;

	pak_sa '.':		// last inserted text
	    *argp = get_last_insert_save();
	    *allocated = TRUE;
	    အကယ်၍ (*argp == NULL && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    ပြန်ပေး TRUE;

	pak_sa Ctrl_F:		// Filename under cursor
	pak_sa Ctrl_P:		// Path under cursor, expand via "path"
	    အကယ်၍ (!errmsg)
		ပြန်ပေး FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, NULL);
	    *allocated = TRUE;
	    ပြန်ပေး TRUE;

	pak_sa Ctrl_W:		// word under cursor
	pak_sa Ctrl_A:		// WORD (mnemonic All) under cursor
	    အကယ်၍ (!errmsg)
		ပြန်ပေး FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : NULL;
	    *allocated = TRUE;
	    ပြန်ပေး TRUE;

	pak_sa Ctrl_L:		// Line under cursor
	    အကယ်၍ (!errmsg)
		ပြန်ပေး FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    ပြန်ပေး TRUE;

	pak_sa '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    ပြန်ပေး TRUE;
    }

    ပြန်ပေး FALSE;
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
    ကိန်းပြည့်
cmdline_paste_reg(
    ကိန်းပြည့် regname,
    ကိန်းပြည့် literally_arg,	// Insert text literally instead of "as typed"
    ကိန်းပြည့် remcr)		// don't add CR characters
{
    long	i;
    ကိန်းပြည့်		literally = literally_arg;

    အကယ်၍ (get_yank_register(regname, FALSE))
	literally = TRUE;
    အကယ်၍ (y_current->y_array == NULL)
	ပြန်ပေး FAIL;

    အတွက် (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	အကယ်၍ ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	အကယ်၍ (got_int)
	    ပြန်ပေး FAIL;
    }
    ပြန်ပေး OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    void
shift_delete_registers(void)
{
    ကိန်းပြည့်		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    အတွက် (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    အကယ်၍ (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = NULL;		// set register one to empty
}

#အကယ်၍ defined(FEAT_EVAL)
    static void
add_regtype_to_dict(ကိန်းပြည့် regname, dict_T *dict, char_u *buf, ကိန်းပြည့် bufsize)
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
    (void)dict_add_string_len(dict, "regtype", buf, (ကိန်းပြည့်)buflen);
}

    void
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    static ကိန်းပြည့်	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    ကိန်းပြည့်		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    အကယ်၍ (recursive)
	ပြန်ပေး;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    အကယ်၍ (list == NULL)
	ပြန်ပေး;

    // yanked text contents
    အတွက် (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (ကိန်းပြည့်)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    အကယ်၍ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (void)dict_add_string_len(v_event, "regname", buf, (ကိန်းပြည့်)buflen);

    // motion type: inclusive or exclusive
    (void)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (void)dict_add_string_len(v_event, "operator", buf, (ကိန်းပြည့်)buflen);

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
	ကိန်းပြည့်	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *insert,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	ကိန်းပြည့်	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    static bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    ကိန်းပြည့်		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    အကယ်၍ (recursive || regname == '_')
	ပြန်ပေး;

    အကယ်၍ (regname != '.' && insert == NULL
	    && reg == NULL)
	// Can happen when pasting text in normal mode in a terminal buffer
	ပြန်ပေး;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    အကယ်၍ (list == NULL)
	ပြန်ပေး;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    အကယ်၍ (regname == '.')
    {
	အကယ်၍ (last_insert_ga.ga_data != NULL)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_insert_ga.ga_data,
		    (ကိန်းပြည့်)last_insert_ga.ga_len);
    }
    မဟုတ်ပါက အကယ်၍ (insert != NULL)
    {
	list_append_string(list, insert->string, (ကိန်းပြည့်)insert->length);
    }
    မဟုတ်ပါက
    {
	အတွက် (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (ကိန်းပြည့်)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    အကယ်၍ (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (void)dict_add_string_len(v_event, "regname", buf, (ကိန်းပြည့်)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (void)dict_add_string_len(v_event, "operator", buf, (ကိန်းပြည့်)buflen);

    add_regtype_to_dict(regname, v_event, buf, parimaa_na(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (void)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_items_ro(v_event);

    recursive = true;
    textlock++;
    အကယ်၍ (post)
	apply_autocmds(EVENT_TEXTPUTPOST, NULL, NULL, FALSE, curbuf);
    မဟုတ်ပါက
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
    ကိန်းပြည့်		i;

    အတွက် (i = 0; i < NUM_REGISTERS; ++i)
	y_regs[i].y_array = NULL;
}

#အကယ်၍ defined(EXITFREE)
    void
clear_registers(void)
{
    ကိန်းပြည့်		i;

    အတွက် (i = 0; i < NUM_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	အကယ်၍ (y_current->y_array != NULL)
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
    အကယ်၍ (y_current->y_array == NULL)
	ပြန်ပေး;

    long	    i;

    အတွက် (i = n; --i >= 0; )
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
    ကိန်းပြည့်
op_yank(oparg_T *oap, ကိန်းပြည့် deleting, ကိန်းပြည့် mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    ကိန်းပြည့်			yanktype = oap->motion_type;
    long		yanklines = oap->line_count;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    sa_nracanaa block_def	bd;

				    // check for read-only register
    အကယ်၍ (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	ပြန်ပေး FAIL;
    }
    အကယ်၍ (oap->regname == '_')	    // black hole: nothing to do
	ပြန်ပေး OK;

#ifdef FEAT_CLIPBOARD
    အကယ်၍ ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	msg_warn_missing_clipboard();
    }
#endif

    အကယ်၍ (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    အကယ်၍ (y_append && y_current->y_array != NULL)
	y_current = &newreg;
    မဟုတ်ပါက
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    အကယ်၍ (       oap->motion_type == MCHAR
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
    အကယ်၍ (y_current->y_array == NULL)
    {
	y_current = curr;
	ပြန်ပေး FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    အကယ်၍ (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	အကယ်၍ (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    အတွက် ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	switch (y_current->y_type)
	{
	    pak_sa MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		အကယ်၍ (yank_copy_line(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    goto fail;
		bha_nga;

	    pak_sa MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		အကယ်၍ ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == NULL)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    goto fail;
		}
		bha_nga;

	    pak_sa MCHAR:
		{
		    ကိန်းပြည့် tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (ကိန်းပြည့်)STRLEN(bd.textstart);
		    အကယ်၍ (tmp < bd.textlen)
			bd.textlen = tmp;

		    အကယ်၍ (yank_copy_line(&bd, y_idx, FALSE) == FAIL)
			goto fail;
		    bha_nga;
		}
		// NOTREACHED
	}
    }

    အကယ်၍ (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	အကယ်၍ (new_ptr == NULL)
	    goto fail;
	အတွက် (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	အကယ်၍ (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	အကယ်၍ (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == NULL)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    အကယ်၍ (pnew == NULL)
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
	မဟုတ်ပါက
	    y_idx = 0;
	နေစဉ် (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    အကယ်၍ (mess)			// Display message about yank?
    {
	အကယ်၍ (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	အကယ်၍ (yanklines > p_report)
	{
	    var_na namebuf[100];

	    အကယ်၍ (oap->regname == NUL)
		*namebuf = NUL;
	    မဟုတ်ပါက
		vim_snprintf(namebuf, parimaa_na(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    အကယ်၍ (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    မဟုတ်ပါက
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    အကယ်၍ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	အကယ်၍ (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	အကယ်၍ (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    အကယ်၍ (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	အကယ်၍ (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    အကယ်၍ (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	အကယ်၍ (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	အကယ်၍ (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    အကယ်၍ (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# အကယ်၍ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	အကယ်၍ (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    အကယ်၍ (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#အကယ်၍ defined(FEAT_EVAL)
    အကယ်၍ (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    ပြန်ပေး OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    ပြန်ပေး FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    static ကိန်းပြည့်
yank_copy_line(sa_nracanaa block_def *bd, long y_idx, ကိန်းပြည့် exclude_trailing_space)
{
    char_u	*pnew;

    အကယ်၍ (exclude_trailing_space)
	bd->endspaces = 0;
    အကယ်၍ ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == NULL)
	ပြန်ပေး FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    အကယ်၍ (exclude_trailing_space)
    {
	ကိန်းပြည့် s = bd->textlen + bd->endspaces;

	နေစဉ် (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    ပြန်ပေး OK;
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
    အကယ်၍ (y_current->y_array == NULL)
	y_current->y_size = 0;
    မဟုတ်ပါက
	အတွက် (j = 0; j < y_current->y_size; ++j)
	{
	    အကယ်၍ ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
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
    ကိန်းပြည့်		regname,
    char_u	*expr_result,	// result for regname "=" when compiled
    ကိန်းပြည့်		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	count,
    ကိန်းပြည့်		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    ကိန်းပြည့်		yanklen;
    ကိန်းပြည့်		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    ကိန်းပြည့်		y_type;
    long	y_size;
    ကိန်းပြည့်		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = NULL;
    yankreg_T	*y_current_used = NULL;
    long	nr_lines = 0;
    string_T	insert_string;
    ကိန်းပြည့်		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned ကိန်းပြည့် cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(void)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    အကယ်၍ (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    အကယ်၍ (regname == '.')
    {
	အကယ်၍ (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	အကယ်၍ (has_textput_events)
	    add_last_insert++;
#endif

	(void)stuff_inserted((dir == FORWARD ? (count == -1 ? 'o' : 'a') :
				    (count == -1 ? 'O' : 'i')), count, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	အကယ်၍ (has_textputpre())
	    put_do_autocmd('.', NULL, NULL, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	အကယ်၍ (has_textputpost())
	    put_do_autocmd('.', NULL, NULL, true, dir);

	အကယ်၍ (has_textput_events && --add_last_insert == 0)
	    ga_clear(&last_insert_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	အကယ်၍ ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	ပြန်ပေး;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    insert_string.string = NULL;
    အကယ်၍ (regname == '=' && expr_result != NULL)
	insert_string.string = expr_result;
    မဟုတ်ပါက အကယ်၍ (get_spec_reg(regname, &insert_string.string, &allocated, TRUE)
		&& insert_string.string == NULL)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	ပြန်ပေး;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    အကယ်၍ (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	goto end;

    အကယ်၍ (insert_string.string != NULL)
    {
	insert_string.length = STRLEN(insert_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	အကယ်၍ (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    အတွက် (;;)
	    {
		y_size = 0;
		ptr = insert_string.string;
		ptrlen = insert_string.length;
		နေစဉ် (ptr != NULL)
		{
		    အကယ်၍ (y_array != NULL)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    အကယ်၍ (tmp == NULL)
		    {
			အကယ်၍ (y_array != NULL)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    မဟုတ်ပါက
		    {
			အကယ်၍ (y_array != NULL)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			အကယ်၍ (*tmp == NUL)
			{
			    y_type = MLINE;
			    bha_nga;
			}
		    }
		    ptr = tmp;
		}
		အကယ်၍ (y_array != NULL)
		    bha_nga;
		y_array = ALLOC_MULT(string_T, y_size);
		အကယ်၍ (y_array == NULL)
		    goto end;
	    }
	}
	မဟုတ်ပါက
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &insert_string;
	}
#ifdef FEAT_EVAL
	အကယ်၍ (has_textputpre())
	    put_do_autocmd(regname, NULL, &insert_string, false, dir);
#endif
    }
    မဟုတ်ပါက
    {
#ifdef FEAT_EVAL
	အကယ်၍ (has_textputpre())
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

    အကယ်၍ (y_type == MLINE)
    {
	အကယ်၍ (flags & PUT_LINE_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    အကယ်၍ (u_save_cursor() == FAIL)
		goto end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    အကယ်၍ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    အကယ်၍ (ptr == NULL)
		goto end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    အကယ်၍ (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    အကယ်၍ (ptr == NULL)
		goto end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_lines;
	    dir = FORWARD;
	}
	အကယ်၍ (flags & PUT_LINE_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    အကယ်၍ (flags & PUT_LINE)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    အကယ်၍ (y_size == 0 || y_array == NULL)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	goto end;
    }

    အကယ်၍ (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	အကယ်၍ (lnum > curbuf->b_ml.ml_line_count)
	    lnum = curbuf->b_ml.ml_line_count + 1;
	အကယ်၍ (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    goto end;
    }
    မဟုတ်ပါက အကယ်၍ (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	အကယ်၍ (dir == BACKWARD)
	    (void)hasFolding(lnum, &lnum, NULL);
	မဟုတ်ပါက
	    (void)hasFolding(lnum, NULL, &lnum);
#endif
	အကယ်၍ (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	အကယ်၍ ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    goto end;
#ifdef FEAT_FOLDING
	အကယ်၍ (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	မဟုတ်ပါက
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    မဟုတ်ပါက အကယ်၍ (u_save_cursor() == FAIL)
	goto end;

    အကယ်၍ (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	အကယ်၍ (gchar_cursor() == TAB)
	{
	    ကိန်းပြည့် viscol = getviscol();
	    ကိန်းပြည့် ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    အကယ်၍ (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#မဟုတ်ပါက
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    မဟုတ်ပါက
		curwin->w_cursor.coladd = 0;
	}
	မဟုတ်ပါက အကယ်၍ (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    အကယ်၍ (y_type == MBLOCK)
    {
	ကိန်းပြည့်	delcount;
	ကိန်းပြည့်	incr = 0;
	sa_nracanaa block_def bd;
	long	j;
	ကိန်းပြည့်	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	အကယ်၍ (dir == FORWARD && c != NUL)
	{
	    အကယ်၍ (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, NULL, &endcol2, 0);
	    မဟုတ်ပါက
		getvcol(curwin, &curwin->w_cursor, NULL, NULL, &col, 0);

	    အကယ်၍ (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    မဟုတ်ပါက
		အကယ်၍ (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	မဟုတ်ပါက
	    getvcol(curwin, &curwin->w_cursor, &col, NULL, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	အကယ်၍ (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    အကယ်၍ (dir == FORWARD && c == NUL)
		++col;
	    အကယ်၍ (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    အကယ်၍ (c == TAB)
	    {
		အကယ်၍ (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		အကယ်၍ (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	အတွက် (i = 0; i < y_size; ++i)
	{
	    ကိန်းပြည့်		    spaces = 0;
	    var_na	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    အကယ်၍ (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	    {
		အကယ်၍ (ml_append(curbuf->b_ml.ml_line_count, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    bha_nga;
		++nr_lines;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    နေစဉ် (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    အကယ်၍ (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    မဟုတ်ပါက အကယ်၍ (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		အကယ်၍ (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		အကယ်၍ (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (ကိန်းပြည့်)y_array[i].length;

	    အကယ်၍ ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		နေစဉ် (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		အကယ်၍ (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    အကယ်၍ (yanklen + spaces != 0
		     && count > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		bha_nga;
	    }

	    totlen = count * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    အကယ်၍ (newp == NULL)
		bha_nga;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    အတွက် (j = 0; j < count; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		အကယ်၍ ((j < count - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		မဟုတ်ပါက
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
	    အကယ်၍ (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_lines(lnum, 0, curwin->w_cursor.lnum - nr_lines, nr_lines);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	အကယ်၍ (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	အကယ်၍ (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    အကယ်၍ (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	မဟုတ်ပါက
	    curwin->w_cursor.lnum = lnum;
    }
    မဟုတ်ပါက
    {
	pos_T	new_cursor;

	yanklen = (ကိန်းပြည့်)y_array[0].length;

	// Character or Line mode
	အကယ်၍ (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    အကယ်၍ (dir == FORWARD && gchar_cursor() != NUL)
	    {
		အကယ်၍ (has_mbyte)
		{
		    ကိန်းပြည့် bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    အကယ်၍ (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		မဟုတ်ပါက
		{
		    ++col;
		    အကယ်၍ (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	မဟုတ်ပါက အကယ်၍ (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	အကယ်၍ (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    ကိန်းပြည့်		first_byte_off = 0;

	    အကယ်၍ (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		အကယ်၍ (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		အကယ်၍ (end_lnum > start_lnum)
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

	    အကယ်၍ (count == 0 || yanklen == 0)
	    {
		အကယ်၍ (VIsual_active)
		    lnum = end_lnum;
	    }
	    မဟုတ်ပါက အကယ်၍ (count > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    မဟုတ်ပါက
	    {
		totlen = count * yanklen;
		do
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    အကယ်၍ (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			အကယ်၍ (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			မဟုတ်ပါက
			    col = MAXCOL;
		    }
		    အကယ်၍ (VIsual_active && col > oldlen)
		    {
			lnum++;
			continue;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    အကယ်၍ (newp == NULL)
			goto end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    အတွက် (i = 0; i < count; ++i)
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
		    အကယ်၍ (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    အကယ်၍ (VIsual_active)
			lnum++;
		} နေစဉ် (VIsual_active && lnum <= end_lnum);

		အကယ်၍ (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    အကယ်၍ (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    မဟုတ်ပါက
		curwin->w_cursor.col -= first_byte_off;
	}
	မဟုတ်ပါက
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    ကိန်းပြည့်		indent;
	    ကိန်းပြည့်		orig_indent = 0;
	    ကိန်းပြည့်		indent_diff = 0;	// init for gcc
	    ကိန်းပြည့်		first_indent = TRUE;
	    ကိန်းပြည့်		lendiff = 0;
	    long	cnt;

	    အကယ်၍ (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    အတွက် (cnt = 1; cnt <= count; ++cnt)
	    {
		i = 0;
		အကယ်၍ (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (ကိန်းပြည့်)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    အကယ်၍ (newp == NULL)
			goto error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    အကယ်၍ (newp == NULL)
			goto error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		အတွက် (; i < y_size; ++i)
		{
		    အကယ်၍ (y_type != MCHAR || i < y_size - 1)
		    {
			အကယ်၍ (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    goto error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_lines;
		    အကယ်၍ (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			အကယ်၍ (cnt == count && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			အကယ်၍ (*ptr == '#' && preprocs_left())
			    indent = 0;     // Leave # lines at start
			မဟုတ်ပါက အကယ်၍ (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			မဟုတ်ပါက အကယ်၍ (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			မဟုတ်ပါက အကယ်၍ ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(void)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			အကယ်၍ (cnt == count && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		အကယ်၍ (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    အကယ်၍ (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		အကယ်၍ (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_lines, 0L);

	    // note changed text for displaying and folding
	    အကယ်၍ (y_type == MCHAR)
		changed_lines(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_lines);
	    မဟုတ်ပါက
		changed_lines(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_lines);
	    အကယ်၍ (y_current_used != NULL && (y_current_used != y_current
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
	    အကယ်၍ (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		အကယ်၍ (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    မဟုတ်ပါက
		curbuf->b_op_end.col = 0;

	    အကယ်၍ (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    မဟုတ်ပါက အကယ်၍ (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		အကယ်၍ (y_type == MLINE)
		{
		    အကယ်၍ (lnum >= curbuf->b_ml.ml_line_count)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		    မဟုတ်ပါက
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		မဟုတ်ပါက
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    အကယ်၍ (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    မဟုတ်ပါက အကယ်၍ (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		အကယ်၍ (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    မဟုတ်ပါက	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_lines);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    ကိန်းပြည့် len = ml_get_curline_len();
    အကယ်၍ (curwin->w_cursor.col > len)
    {
	အကယ်၍ (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    အကယ်၍ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    အကယ်၍ (has_textputpost())
    {
	အကယ်၍ (insert_string.string == NULL)
	    put_do_autocmd(regname, y_current, NULL, true, dir);
	မဟုတ်ပါက
	    put_do_autocmd(regname, NULL, &insert_string, true, dir);
    }
#endif

    အကယ်၍ (allocated)
	vim_free(insert_string.string);
    အကယ်၍ (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    ကိန်းပြည့်
get_register_name(ကိန်းပြည့် num)
{
    အကယ်၍ (num == -1)
	ပြန်ပေး '"';
    မဟုတ်ပါက အကယ်၍ (num < 10)
	ပြန်ပေး num + '0';
    မဟုတ်ပါက အကယ်၍ (num == DELETION_REGISTER)
	ပြန်ပေး '-';
#အကယ်၍ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    မဟုတ်ပါက အကယ်၍ (num == STAR_REGISTER)
	ပြန်ပေး '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    မဟုတ်ပါက အကယ်၍ (clipmethod == CLIPMETHOD_PROVIDER && num == REAL_PLUS_REGISTER)
	ပြန်ပေး '+';
    မဟုတ်ပါက အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER && num == PLUS_REGISTER)
	ပြန်ပေး '+';
#endif
    မဟုတ်ပါက
	ပြန်ပေး num + 'a' - 10;
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    ကိန်းပြည့်
get_unname_register(void)
{
    ပြန်ပေး y_previous == NULL ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    void
ex_display(exarg_T *eap)
{
    ကိန်းပြည့်		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    ကိန်းပြည့်		name;
    ကိန်းပြည့်		attr;
    char_u	*arg = eap->arg;
    ကိန်းပြည့်		clen;
    ကိန်းပြည့်		type;
    string_T	insert;

    silence_w23_w24_msg++;
    အကယ်၍ (arg != NULL && *arg == NUL)
	arg = NULL;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    msg_puts_title(_("\nType Name Content"));
    အတွက် (i = -1; i < NUM_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	switch (get_reg_type(name, NULL))
	{
	    pak_sa MLINE: type = 'l'; bha_nga;
	    pak_sa MCHAR: type = 'c'; bha_nga;
	    default:	type = 'b'; bha_nga;
	}
	အကယ်၍ (arg != NULL && vim_strchr(arg, name) == NULL
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
	အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (void)may_get_selection(name);
#endif

	အကယ်၍ (i == -1)
	{
	    အကယ်၍ (y_previous != NULL)
		yb = y_previous;
	    မဟုတ်ပါက
		yb = &(y_regs[0]);
	}
	မဟုတ်ပါက
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	အကယ်၍ (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != NULL &&
		    (yb == y_previous || yb == &y_regs[0])))
	    continue;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	အကယ်၍ (yb->y_array != NULL)
	{
	    ကိန်းပြည့် do_show = FALSE;

	    အတွက် (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    အကယ်၍ (do_show || yb->y_size == 0)
	    {
		msg_putchar('\n');
		msg_puts("  ");
		msg_putchar(type);
		msg_puts("  ");
		msg_putchar('"');
		msg_putchar(name);
		msg_puts("   ");

		n = (ကိန်းပြည့်)Columns - 11;
		အတွက် (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    အကယ်၍ (j)
		    {
			msg_puts_attr("^J", attr);
			n -= 2;
		    }
		    အတွက် (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			msg_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		အကယ်၍ (n > 1 && yb->y_type == MLINE)
		    msg_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    insert = get_last_insert();
    အကယ်၍ ((p = insert.string) != NULL
		  && (arg == NULL || vim_strchr(arg, '.') != NULL) && !got_int
						      && !message_filtered(p))
    {
	msg_puts("\n  c  \".   ");
	dis_msg(p, TRUE);
    }

    // display last command line
    အကယ်၍ (last_cmdline != NULL && (arg == NULL || vim_strchr(arg, ':') != NULL)
			       && !got_int && !message_filtered(last_cmdline))
    {
	msg_puts("\n  c  \":   ");
	dis_msg(last_cmdline, FALSE);
    }

    // display current file name
    အကယ်၍ (curbuf->b_fname != NULL
	    && (arg == NULL || vim_strchr(arg, '%') != NULL) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	msg_puts("\n  c  \"%   ");
	dis_msg(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    အကယ်၍ ((arg == NULL || vim_strchr(arg, '#') != NULL) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	အကယ်၍ (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    msg_puts("\n  c  \"#   ");
	    dis_msg(fname, FALSE);
	}
    }

    // display last search pattern
    အကယ်၍ (last_search_pat() != NULL
		 && (arg == NULL || vim_strchr(arg, '/') != NULL) && !got_int
				      && !message_filtered(last_search_pat()))
    {
	msg_puts("\n  c  \"/   ");
	dis_msg(last_search_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    အကယ်၍ (expr_line != NULL && (arg == NULL || vim_strchr(arg, '=') != NULL)
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
    ကိန်းပြည့်		skip_esc)	    // if TRUE, ignore trailing ESC
{
    ကိန်းပြည့်		n;
    ကိန်းပြည့်		l;

    n = (ကိန်းပြည့်)Columns - 6;
    နေစဉ် (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	အကယ်၍ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    msg_outtrans_len(p, l);
	    p += l;
	}
	မဟုတ်ပါက
	    msg_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#အကယ်၍ defined(FEAT_DND)
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
get_reg_type(ကိန်းပြည့် regname, long *reglen)
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
	    ပြန်ပေး MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    အကယ်၍ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ပြန်ပေး MAUTO;

    get_yank_register(regname, FALSE);

    အကယ်၍ (y_current->y_array != NULL)
    {
	အကယ်၍ (reglen != NULL && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	ပြန်ပေး y_current->y_type;
    }
    ပြန်ပေး MAUTO;
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    static char_u *
getreg_wrap_one_line(char_u *s, ကိန်းပြည့် flags)
{
    အကယ်၍ (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	အကယ်၍ (list != NULL)
	{
	    အကယ်၍ (list_append_string(list, NULL, -1) == FAIL)
	    {
		list_free(list);
		ပြန်ပေး NULL;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	ပြန်ပေး (char_u *)list;
    }
    ပြန်ပေး s;
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
get_reg_contents(ကိန်းပြည့် regname, ကိန်းပြည့် flags)
{
    linenr_T	i;
    char_u	*retval;
    ကိန်းပြည့်		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    အကယ်၍ (regname == '=')
    {
	အကယ်၍ (flags & GREG_NO_EXPR)
	    ပြန်ပေး NULL;
	အကယ်၍ (flags & GREG_EXPR_SRC)
	    ပြန်ပေး getreg_wrap_one_line(get_expr_line_src(), flags);
	ပြန်ပေး getreg_wrap_one_line(get_expr_line(), flags);
    }

    အကယ်၍ (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    အကယ်၍ (regname != NUL && !valid_yank_reg(regname, FALSE))
	ပြန်ပေး NULL;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    အကယ်၍ (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    အကယ်၍ (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	အကယ်၍ (retval == NULL)
	    ပြန်ပေး NULL;
	အကယ်၍ (allocated)
	    ပြန်ပေး getreg_wrap_one_line(retval, flags);
	ပြန်ပေး getreg_wrap_one_line(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    အကယ်၍ (y_current->y_array == NULL)
	ပြန်ပေး NULL;

    အကယ်၍ (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	အကယ်၍ (list == NULL)
	    ပြန်ပေး NULL;
	အတွက် (i = 0; i < y_current->y_size; ++i)
	{
	    အကယ်၍ (list_append_string(list, y_current->y_array[i].string,
		(ကိန်းပြည့်)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		ပြန်ပေး NULL;
	    }
	}
	ပြန်ပေး (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    အတွက် (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	အကယ်၍ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    အကယ်၍ (retval == NULL)
	ပြန်ပေး NULL;

    // Copy the lines of the yank register into the string.
    len = 0;
    အတွက် (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	အကယ်၍ (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    ပြန်ပေး retval;
}

    static ကိန်းပြည့်
init_write_reg(
    ကိန်းပြည့်		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    ကိန်းပြည့်		must_append,
    ကိန်းပြည့်		*yank_type UNUSED)
{
    အကယ်၍ (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	ပြန်ပေး FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    အကယ်၍ (!y_append && !must_append)
	free_yank_all();
    ပြန်ပေး OK;
}

    static void
finish_write_reg(
    ကိန်းပြည့်		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    အကယ်၍ (name != '"')
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
    ကိန်းပြည့်		name,
    char_u	*str,
    ကိန်းပြည့်		maxlen,
    ကိန်းပြည့်		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    void
write_reg_contents_lst(
    ကိန်းပြည့်		name,
    char_u	**strings,
    ကိန်းပြည့်		maxlen UNUSED,
    ကိန်းပြည့်		must_append,
    ကိန်းပြည့်		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    အကယ်၍ (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	အကယ်၍ (strings[0] == NULL)
	    s = (char_u *)"";
	မဟုတ်ပါက အကယ်၍ (strings[1] != NULL)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_lines), name);
	    ပြန်ပေး;
	}
	မဟုတ်ပါက
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	ပြန်ပေး;
    }

    အကယ်၍ (name == '_')	    // black hole: nothing to do
	ပြန်ပေး;

    အကယ်၍ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ပြန်ပေး;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    void
write_reg_contents_ex(
    ကိန်းပြည့်		name,
    char_u	*str,
    ကိန်းပြည့်		maxlen,
    ကိန်းပြည့်		must_append,
    ကိန်းပြည့်		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    အကယ်၍ (maxlen >= 0)
	len = maxlen;
    မဟုတ်ပါက
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    အကယ်၍ (name == '/')
    {
	set_last_search_pat(str, RE_SEARCH, TRUE, TRUE);
	ပြန်ပေး;
    }

    အကယ်၍ (name == '#')
    {
	အကယ်၍ (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  ပြန်ပေး;
	}

	buf_T	*buf;

	အကယ်၍ (VIM_ISDIGIT(*str))
	{
	    ကိန်းပြည့်	num = atoi((var_na *)str);

	    buf = buflist_findnr(num);
	    အကယ်၍ (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist), (long)num);
	}
	မဟုတ်ပါက
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	အကယ်၍ (buf == NULL)
	    ပြန်ပေး;
	curwin->w_alt_fnum = buf->b_fnum;
	ပြန်ပေး;
    }

    အကယ်၍ (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	အကယ်၍ (p == NULL)
	    ပြန်ပေး;
	အကယ်၍ (must_append && expr_line != NULL)
	{
	    s = concat_str(expr_line, p);
	    vim_free(p);
	    p = s;
	}
	set_expr_line(p, NULL);
	ပြန်ပေး;
    }

    အကယ်၍ (name == '_')	    // black hole: nothing to do
	ပြန်ပေး;

    အကယ်၍ (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ပြန်ပေး;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#အကယ်၍ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    void
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    ကိန်းပြည့်		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    ကိန်းပြည့်		str_list)	// TRUE if str is char_u **
{
    ကိန်းပြည့်		type;			// MCHAR, MLINE or MBLOCK
    ကိန်းပြည့်		lnum;
    long	start;
    long	i;
    ကိန်းပြည့်		extra;
    ကိန်းပြည့်		newlines;		// number of lines added
    ကိန်းပြည့်		extraline = 0;		// extra line at the end
    ကိန်းပြည့်		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    အကယ်၍ (y_ptr->y_array == NULL)		// NULL means empty register
	y_ptr->y_size = 0;

    အကယ်၍ (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    မဟုတ်ပါက
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    အကယ်၍ (str_list)
    {
	အတွက် (ss = (char_u **) str; *ss != NULL; ++ss)
	    ++newlines;
    }
    မဟုတ်ပါက
    {
	အတွက် (i = 0; i < len; i++)
	    အကယ်၍ (str[i] == '\n')
		++newlines;
	အကယ်၍ (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	အကယ်၍ (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    အကယ်၍ (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	ပြန်ပေး;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * parimaa_na(string_T), TRUE);
    အကယ်၍ (pp == NULL)	// out of memory
	ပြန်ပေး;
    အတွက် (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    အကယ်၍ (str_list)
    {
	အတွက် (ss = (char_u **) str; *ss != NULL; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    အကယ်၍ (type == MBLOCK)
	    {
		ကိန်းပြည့် charlen = vim_strsize(*ss);

		အကယ်၍ (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    မဟုတ်ပါက
    {
	အတွက် (start = 0; start < len + extraline; start += i + 1)
	{
	    ကိန်းပြည့် charlen = 0;

	    အတွက် (i = start; i < len;)	// find the end of the line
	    {
		အကယ်၍ (str[i] == '\n')
		    bha_nga;
		အကယ်၍ (type == MBLOCK)
		{
		    အကယ်၍ (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    မဟုတ်ပါက
			charlen += byte2cells(str[i]);
		}

		အကယ်၍ (str[i] == NUL)
		    i++; // registers can have NUL chars
		မဟုတ်ပါက
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    အကယ်၍ (charlen > maxlen)
		maxlen = charlen;
	    အကယ်၍ (append)
	    {
		--lnum;
		extra = (ကိန်းပြည့်)y_ptr->y_array[lnum].length;
	    }
	    မဟုတ်ပါက
		extra = 0;
	    s = alloc(i + extra + 1);
	    အကယ်၍ (s == NULL)
		bha_nga;
	    အကယ်၍ (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    အကယ်၍ (append)
		vim_free(y_ptr->y_array[lnum].string);
	    အကယ်၍ (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    နေစဉ် (--extra >= 0)
	    {
		အကယ်၍ (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    အကယ်၍ (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    မဟုတ်ပါက
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
