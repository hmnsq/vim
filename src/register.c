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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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
ସ୍ଥିର yankreg_T	y_regs[ଅଙ୍କ_REGISTERS];

ସ୍ଥିର yankreg_T	*y_current;	    // ptr to current yankreg
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y_append;	    // TRUE when appending
ସ୍ଥିର yankreg_T	*y_previous = ଖାଲି; // ptr to last written yankreg

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	stuff_yank(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u *);
ସ୍ଥିର ଶୂନ୍ୟ	put_reedit_in_typebuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା silent);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	put_in_typebuf(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା esc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା colon, ପୂର୍ଣ୍ଣସଂଖ୍ୟା silent);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	yank_copy_ଧାଡ଼ି(ସଂରଚନା block_def *bd, long y_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exclude_trailing_space);
#ifdef HAVE_CLIPMETHOD
ସ୍ଥିର ଶୂନ୍ୟ	copy_yank_reg(yankreg_T *reg);
#endif
ସ୍ଥିର ଶୂନ୍ୟ	dis_ସନ୍ଦେଶ(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip_esc);

#ଯଦି defined(FEAT_VIMINFO)
    yankreg_T *
get_y_regs(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ y_regs;
}
#endif

#ଯଦି defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    yankreg_T *
get_y_register(ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg)
{
    ଫେରନ୍ତୁ &y_regs[reg];
}
#endif

    yankreg_T *
get_y_current(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ y_current;
}

    yankreg_T *
get_y_previous(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ y_previous;
}

    ଶୂନ୍ୟ
set_y_current(yankreg_T *yreg)
{
    y_current = yreg;
}

    ଶୂନ୍ୟ
set_y_previous(yankreg_T *yreg)
{
    y_previous = yreg;
}

    ଶୂନ୍ୟ
reset_y_append(ଶୂନ୍ୟ)
{
    y_append = FALSE;
}


#ଯଦି defined(FEAT_EVAL)
/*
 * Keep the last expression line here, for repeating.
 */
ସ୍ଥିର char_u	*ପଦସମଷ୍ଟି_ଧାଡ଼ି = ଖାଲି;
ସ୍ଥିର exarg_T	*ପଦସମଷ୍ଟି_eap = ଖାଲି;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_ପଦସମଷ୍ଟି_register(ଶୂନ୍ୟ)
{
    char_u	*new_ଧାଡ଼ି;

    new_ଧାଡ଼ି = getcmdline('=', 0L, 0, 0);
    ଯଦି (new_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ NUL;
    ଯଦି (*new_ଧାଡ଼ି == NUL)	// use previous line
	vim_free(new_ଧାଡ଼ି);
    ଅନ୍ୟଥା
	set_ପଦସମଷ୍ଟି_ଧାଡ଼ି(new_ଧାଡ଼ି, ଖାଲି);
    ଫେରନ୍ତୁ '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 * "eap" may be used if the next line needs to be checked when evaluating the
 * expression.
 */
    ଶୂନ୍ୟ
set_ପଦସମଷ୍ଟି_ଧାଡ଼ି(char_u *new_ଧାଡ଼ି, exarg_T *eap)
{
    vim_free(ପଦସମଷ୍ଟି_ଧାଡ଼ି);
    ପଦସମଷ୍ଟି_ଧାଡ଼ି = new_ଧାଡ଼ି;
    ପଦସମଷ୍ଟି_eap = eap;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    char_u *
get_ପଦସମଷ୍ଟି_ଧାଡ଼ି(ଶୂନ୍ୟ)
{
    char_u	*ପଦସମଷ୍ଟି_copy;
    char_u	*rv;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nested = 0;

    ଯଦି (ପଦସମଷ୍ଟି_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    ପଦସମଷ୍ଟି_copy = vim_strsave(ପଦସମଷ୍ଟି_ଧାଡ଼ି);
    ଯଦି (ପଦସମଷ୍ଟି_copy == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    ଯଦି (nested >= 10)
	ଫେରନ୍ତୁ ପଦସମଷ୍ଟି_copy;

    ++nested;
    rv = eval_to_string_eap(ପଦସମଷ୍ଟି_copy, TRUE, ପଦସମଷ୍ଟି_eap, FALSE);
    --nested;
    vim_free(ପଦସମଷ୍ଟି_copy);
    ଫେରନ୍ତୁ rv;
}

/*
 * Get the '=' register expression itself, without evaluating it.
 */
    ସ୍ଥିର char_u *
get_ପଦସମଷ୍ଟି_ଧାଡ଼ି_src(ଶୂନ୍ୟ)
{
    ଯଦି (ପଦସମଷ୍ଟି_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ vim_strsave(ପଦସମଷ୍ଟି_ଧାଡ଼ି);
}
#endif // FEAT_EVAL

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_yank_reg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    regname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    writing)	    // if TRUE check for writable registers
{
    ଯଦି (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/#.%:="
#ଅନ୍ୟଥା
				    "/#.%:"
#endif
					, regname) != ଖାଲି)
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#ଯଦି defined(FEAT_CLIPBOARD) // If +clipboard is enabled, then these registers
			    // always exist.
	    || regname == '*'
	    || regname == '+'
#ନଚେତଯଦି defined(FEAT_CLIPBOARD_PROVIDER)
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
	ଫେରନ୍ତୁ TRUE;
#ifndef FEAT_CLIPBOARD
    // clipboard support not enabled in this build
    ଅନ୍ୟଥା ଯଦି (regname == '*' || regname == '+')
    {
	// Warn about missing clipboard support once
	ସନ୍ଦେଶ_warn_missing_clipboard();
	ଫେରନ୍ତୁ FALSE;
    }
#endif
    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_yank_register(ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writing)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ret = FALSE;

    y_append = FALSE;
    ଯଦି ((regname == 0 || regname == '"') && !writing && y_previous != ଖାଲି)
    {
	y_current = y_previous;
	ଫେରନ୍ତୁ ret;
    }
    i = regname;
    ଯଦି (VIM_ISDIGIT(i))
	i -= '0';
    ଅନ୍ୟଥା ଯଦି (ASCII_ISLOWER(i))
	i = Char_Ord_ତଳ(i) + 10;
    ଅନ୍ୟଥା ଯଦି (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    ଅନ୍ୟଥା ଯଦି (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    ଅନ୍ୟଥା ଯଦି (clip_plus.available && regname == '+')
    {
# ifdef FEAT_CLIPBOARD_PROVIDER
	// We want to use the actual + register, since PLUS_REGISTER may be
	// pointing to STAR_REGISTER.
	ଯଦି (clipmethod == CLIPMETHOD_PROVIDER)
	    i = REAL_PLUS_REGISTER;
	ଅନ୍ୟଥା
# endif
	    i = PLUS_REGISTER;
	ret = TRUE;
    }
#ନଚେତଯଦି defined(FEAT_CLIPBOARD_PROVIDER)
    ଅନ୍ୟଥା ଯଦି (regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (regname == '+')
    {
	i = REAL_PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    ଅନ୍ୟଥା ଯଦି (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    ଅନ୍ୟଥା		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    ଯଦି (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    ଫେରନ୍ତୁ ret;
}

/*
 * Obtain the contents of a "normal" register. The register is made empty.
 * The returned pointer has allocated memory, use put_register() later.
 */
    ଶୂନ୍ୟ *
get_register(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// When Visual area changed, may have to update selection.  Obtain the
	// selection too.
	ଯଦି (name == '*' && clip_star.available)
	{
	    ଯଦି (clip_isautosel_star())
		clip_update_selection(&clip_star);
	    may_get_selection(name);
	}
	ଯଦି (name == '+' && clip_plus.available)
	{
	    ଯଦି (clip_isautosel_plus())
		clip_update_selection(&clip_plus);
	    may_get_selection(name);
	}
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    ଯଦି (reg == ଖାଲି)
	ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)ଖାଲି;

    *reg = *y_current;
    ଯଦି (copy)
    {
	// If we run out of memory some or all of the lines are empty.
	ଯଦି (reg->y_size == 0 || y_current->y_array == ଖାଲି)
	    reg->y_array = ଖାଲି;
	ଅନ୍ୟଥା
	    reg->y_array = ALLOC_MULT(string_T, reg->y_size);
	ଯଦି (reg->y_array != ଖାଲି)
	{
	    ପାଇଁ (i = 0; i < reg->y_size; ++i)
	    {
		reg->y_array[i].string = vim_strnsave(y_current->y_array[i].string,
					    y_current->y_array[i].length);
		ଯଦି (reg->y_array[i].string == ଖାଲି)
		{
		    // The allocation failed so clean up and exit
		    ଯେତେବେଳେ (--i >= 0)
			vim_free(reg->y_array[i].string);
		    vim_free(reg->y_array);
		    vim_free(reg);
		    ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)ଖାଲି;
		}

		reg->y_array[i].length = y_current->y_array[i].length;
	    }
	}
    }
    ଅନ୍ୟଥା
	y_current->y_array = ଖାଲି;
    ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)reg;
}

/*
 * Put "reg" into register "name".  Free any previous contents and "reg".
 */
    ଶୂନ୍ୟ
put_register(ପୂର୍ଣ୍ଣସଂଖ୍ୟା name, ଶୂନ୍ୟ *reg)
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

#ଯଦି defined(FEAT_CLIPBOARD)
    ଶୂନ୍ୟ
free_register(ଶୂନ୍ୟ *reg)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
yank_register_mline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname)
{
    ଯଦି (regname != 0 && !valid_yank_reg(regname, FALSE))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (regname == '_')		// black hole is always empty
	ଫେରନ୍ତୁ FALSE;
    get_yank_register(regname, FALSE);
    ଫେରନ୍ତୁ (y_current->y_type == MLINE);
}

/*
 * Start or stop recording into a yank register.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_record(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u	    *p;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    retval;

    ଯଦି (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	ଯଦି (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	ଅନ୍ୟଥା
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    ଅନ୍ୟଥା			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	ସନ୍ଦେଶ("");
	p = get_recorded();
	ଯଦି (p == ଖାଲି)
	    retval = FAIL;
	ଅନ୍ୟଥା
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
    ଫେରନ୍ତୁ retval;
}

/*
 * Stuff string "p" into yank register "regname" as a single line (append if
 * uppercase).	"p" must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
stuff_yank(ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname, char_u *p)
{
    size_t  plen;

    // check for read-only register
    ଯଦି (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	ଫେରନ୍ତୁ OK;
    }

    plen = STRLEN(p);
    get_yank_register(regname, TRUE);
    ଯଦି (y_append && y_current->y_array != ଖାଲି)
    {
	string_T    *pp;
	char_u	    *tmp;
	size_t	    tmplen;

	pp = &(y_current->y_array[y_current->y_size - 1]);
	tmplen = pp->length + plen;
	tmp = alloc(tmplen + 1);
	ଯଦି (tmp == ଖାଲି)
	{
	    vim_free(p);
	    ଫେରନ୍ତୁ FAIL;
	}
	STRCPY(tmp, pp->string);
	STRCPY(tmp + pp->length, p);
	vim_free(p);
	vim_free(pp->string);
	pp->string = tmp;
	pp->length = tmplen;
    }
    ଅନ୍ୟଥା
    {
	free_yank_all();
	ଯଦି ((y_current->y_array = ALLOC_ONE(string_T)) == ଖାଲି)
	{
	    vim_free(p);
	    ଫେରନ୍ତୁ FAIL;
	}
	y_current->y_array[0].string = p;
	y_current->y_array[0].length = plen;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Last executed register (@ command)
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା execreg_lastc = NUL;

#ଯଦି defined(FEAT_VIMINFO)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_execreg_lastc(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ execreg_lastc;
}

    ଶୂନ୍ୟ
set_execreg_lastc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା lastc)
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
    ସ୍ଥିର char_u *
execreg_ଧାଡ଼ି_continuation(string_T *ଧାଡ଼ିଗୁଡ଼ିକ, long *idx)
{
    garray_T	ga;
    long	cmd_start = *idx;
    long	cmd_end = *idx;
    string_T	*tmp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    char_u	*str;

    ga_init2(&ga, ଆକାର(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    ଯେତେବେଳେ (--cmd_start > 0)
    {
	char_u	*p;

	p = skipwhite(ଧାଡ଼ିଗୁଡ଼ିକ[cmd_start].string);
	ଯଦି (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    ଭାଙ୍ଗ;
    }

    // join all the lines
    tmp = &ଧାଡ଼ିଗୁଡ଼ିକ[cmd_start];
    ga_concat_len(&ga, tmp->string, tmp->length);
    ପାଇଁ (j = cmd_start + 1; j <= cmd_end; j++)
    {
	char_u	*p;

	tmp = &ଧାଡ଼ିଗୁଡ଼ିକ[j];
	p = skipwhite(tmp->string);
	ଯଦି (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    ଯଦି (ga.ga_len > 400)
	    {
		ଯଦି (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		ଅନ୍ୟଥା
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
    ଫେରନ୍ତୁ str;
}

/*
 * Execute a yank register: copy it into the stuff buffer.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_execreg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    regname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    colon,		// insert ':' before each line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    addcr,		// always add '\n' to end of line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		remap;

    // repeat previous one
    ଯଦି (regname == '@')
    {
	ଯଦି (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    ଫେରନ୍ତୁ FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    ଯଦି (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	ଫେରନ୍ତୁ FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    ଯଦି (regname == '_')
	ଫେରନ୍ତୁ OK;

    // use last command line
    ଯଦି (regname == ':')
    {
	ଯଦି (last_cmdline == ଖାଲି)
	{
	    emsg(_(e_no_previous_command_ଧାଡ଼ି));
	    ଫେରନ୍ତୁ FAIL;
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
	ଯଦି (p != ଖାଲି)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    ଯଦି (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    ଅନ୍ୟଥା
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    ଅନ୍ୟଥା ଯଦି (regname == '=')
    {
	p = get_ପଦସମଷ୍ଟି_ଧାଡ଼ି();
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    ଅନ୍ୟଥା ଯଦି (regname == '.')		// use last inserted text
    {
	p = get_last_ଭର୍ତ୍ତି_save();
	ଯଦି (p == ଖାଲି)
	{
	    emsg(_(e_no_inserted_text_yet));
	    ଫେରନ୍ତୁ FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    ଅନ୍ୟଥା
    {
	get_yank_register(regname, FALSE);
	ଯଦି (y_current->y_array == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	ପାଇଁ (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    ଯଦି (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		ଯଦି (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i].string;
	    ଯଦି (colon && i > 0)
	    {
		p = skipwhite(str);
		ଯଦି (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_ଧାଡ଼ି_continuation(y_current->y_array, &i);
		    ଯଦି (str == ଖାଲି)
			ଫେରନ୍ତୁ FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    ଯଦି (free_str)
		vim_free(str);
	    ଯଦି (escaped == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    ଯଦି (retval == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
	pending_end_reg_executing = FALSE;
    }
    ଫେରନ୍ତୁ retval;
}

/*
 * If "restart_edit" is not zero, put it in the typeahead buffer, so that it's
 * used only after other typeahead has been processed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
put_reedit_in_typebuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା silent)
{
    ଯଦି (restart_edit == NUL)
	ଫେରନ୍ତୁ;

    char_u	buf[] = { K_SPECIAL, KS_EXTRA, KE_COMMAND,
	// :startinsert
	's', 't', 'a', 'r', 't', 'i', CAR, NUL };
    ଯଦି (restart_edit == 'R')
	buf[8] = 'r';  // :startreplace
    ଅନ୍ୟଥା ଯଦି (restart_edit == 'V')
	buf[8] = 'g';  // :startgreplace
    ଅନ୍ୟଥା ଯଦି (restart_edit == 'A')
	buf[8] = '!';  // :startinsert!
    ଯଦି (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	restart_edit = NUL;
}

/*
 * Insert register contents "s" into the typeahead buffer, so that it will be
 * executed again.
 * When "esc" is TRUE it is to be taken literally: Escape CSI characters and
 * no remapping.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_in_typebuf(
    char_u	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		esc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		colon,	    // add ':' before the line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;

    put_reedit_in_typebuf(silent);
    ଯଦି (colon)
	retval = ins_typebuf((char_u *)"\n", REMAP_NONE, 0, TRUE, silent);
    ଯଦି (retval == OK)
    {
	char_u	*p;

	ଯଦି (esc)
	    p = vim_strsave_escape_csi(s);
	ଅନ୍ୟଥା
	    p = s;
	ଯଦି (p == ଖାଲି)
	    retval = FAIL;
	ଅନ୍ୟଥା
	    retval = ins_typebuf(p, esc ? REMAP_NONE : REMAP_YES,
							     0, TRUE, silent);
	ଯଦି (esc)
	    vim_free(p);
    }
    ଯଦି (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", REMAP_NONE, 0, TRUE, silent);
    ଫେରନ୍ତୁ retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଭର୍ତ୍ତି_reg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		literally_arg)	// insert literally, not as if typed
{
    long	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    char_u	*arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		allocated;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    ଯଦି (got_int)
	ଫେରନ୍ତୁ FAIL;

    // check for valid regname
    ଯଦି (regname != NUL && !valid_yank_reg(regname, FALSE))
	ଫେରନ୍ତୁ FAIL;

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    ଯଦି (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    ଅନ୍ୟଥା ଯଦି (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	ଯଦି (arg == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	stuffescaped(arg, literally);
	ଯଦି (allocated)
	    vim_free(arg);
    }
    ଅନ୍ୟଥା				// name or number register
    {
	ଯଦି (get_yank_register(regname, FALSE))
	    literally = TRUE;
	ଯଦି (y_current->y_array == ଖାଲି)
	    retval = FAIL;
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (i = 0; i < y_current->y_size; ++i)
	    {
		ଯଦି (regname == '-' && y_current->y_type == MCHAR)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir = BACKWARD;
		    ଯଦି ((State & REPLACE_FLAG) != 0)
		    {
			pos_T curpos;
			ଯଦି (u_save_cursor() == FAIL)
			    ଫେରନ୍ତୁ FAIL;
			del_chars((long)mb_charlen(y_current->y_array[0].string), TRUE);
			curpos = curwin->w_cursor;
			ଯଦି (oneright() == FAIL)
			    // hit end of line, need to put forward (after the current position)
			    dir = FORWARD;
			curwin->w_cursor = curpos;
		    }

		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, ଖାଲି, dir, 1L, PUT_CURSEND);
		}
		ଅନ୍ୟଥା
		{
		    stuffescaped(y_current->y_array[i].string, literally);
		    // Insert a newline between lines and after last line if
		    // y_type is MLINE.
		    ଯଦି (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffcharReadbuff('\n');
		}
	    }
	}
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * If "regname" is a special register, return TRUE and store a pointer to its
 * value in "argp".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_spec_reg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname,
    char_u	**argp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*allocated,	// return: TRUE when value was allocated
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errmsg)		// give error message when failing
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt;

    *argp = ଖାଲି;
    *allocated = FALSE;
    ବାଛ (regname)
    {
	ସ୍ଥିତି '%':		// file name
	    ଯଦି (errmsg)
		check_fname();	// will give emsg if not set
	    *argp = curbuf->b_fname;
	    ଫେରନ୍ତୁ TRUE;

	ସ୍ଥିତି '#':		// alternate file name
	    *argp = getaltfname(errmsg);	// may give emsg if not set
	    ଫେରନ୍ତୁ TRUE;

#ifdef FEAT_EVAL
	ସ୍ଥିତି '=':		// result of expression
	    *argp = get_ପଦସମଷ୍ଟି_ଧାଡ଼ି();
	    *allocated = TRUE;
	    ଫେରନ୍ତୁ TRUE;
#endif

	ସ୍ଥିତି ':':		// last command line
	    ଯଦି (last_cmdline == ଖାଲି && errmsg)
		emsg(_(e_no_previous_command_ଧାଡ଼ି));
	    *argp = last_cmdline;
	    ଫେରନ୍ତୁ TRUE;

	ସ୍ଥିତି '/':		// last search-pattern
	    ଯଦି (last_ସନ୍ଧାନ_pat() == ଖାଲି && errmsg)
		emsg(_(e_no_previous_regular_expression));
	    *argp = last_ସନ୍ଧାନ_pat();
	    ଫେରନ୍ତୁ TRUE;

	ସ୍ଥିତି '.':		// last inserted text
	    *argp = get_last_ଭର୍ତ୍ତି_save();
	    *allocated = TRUE;
	    ଯଦି (*argp == ଖାଲି && errmsg)
		emsg(_(e_no_inserted_text_yet));
	    ଫେରନ୍ତୁ TRUE;

	ସ୍ଥିତି Ctrl_F:		// Filename under cursor
	ସ୍ଥିତି Ctrl_P:		// Path under cursor, expand via "path"
	    ଯଦି (!errmsg)
		ଫେରନ୍ତୁ FALSE;
	    *argp = file_name_at_cursor(FNAME_MESS | FNAME_HYP
			    | (regname == Ctrl_P ? FNAME_EXP : 0), 1L, ଖାଲି);
	    *allocated = TRUE;
	    ଫେରନ୍ତୁ TRUE;

	ସ୍ଥିତି Ctrl_W:		// word under cursor
	ସ୍ଥିତି Ctrl_A:		// WORD (mnemonic All) under cursor
	    ଯଦି (!errmsg)
		ଫେରନ୍ତୁ FALSE;
	    cnt = find_ident_under_cursor(argp, regname == Ctrl_W
				   ?  (FIND_IDENT|FIND_STRING) : FIND_STRING);
	    *argp = cnt ? vim_strnsave(*argp, cnt) : ଖାଲି;
	    *allocated = TRUE;
	    ଫେରନ୍ତୁ TRUE;

	ସ୍ଥିତି Ctrl_L:		// Line under cursor
	    ଯଦି (!errmsg)
		ଫେରନ୍ତୁ FALSE;

	    *argp = ml_get_buf(curwin->w_buffer,
			curwin->w_cursor.lnum, FALSE);
	    ଫେରନ୍ତୁ TRUE;

	ସ୍ଥିତି '_':		// black hole: always empty
	    *argp = (char_u *)"";
	    ଫେରନ୍ତୁ TRUE;
    }

    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_paste_reg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା literally_arg,	// Insert text literally instead of "as typed"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା remcr)		// don't add CR characters
{
    long	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		literally = literally_arg;

    ଯଦି (get_yank_register(regname, FALSE))
	literally = TRUE;
    ଯଦି (y_current->y_array == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ପାଇଁ (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i].string, literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	ଯଦି ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	ଯଦି (got_int)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Shift the delete registers: "9 is cleared, "8 becomes "9, etc.
 */
    ଶୂନ୍ୟ
shift_delete_registers(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    ପାଇଁ (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    ଯଦି (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = ଖାଲି;		// set register one to empty
}

#ଯଦି defined(FEAT_EVAL)
    ସ୍ଥିର ଶୂନ୍ୟ
add_regtype_to_dict(ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname, dict_T *dict, char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize)
{
    size_t  buflen;
    long    reglen;
    // Register type
    ବାଛ (get_reg_type(regname, &reglen))
    {
	ସ୍ଥିତି MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି MBLOCK:
	    buflen = vim_snprintf_safelen((ଅକ୍ଷର *)buf, bufsize,
		"%c%ld", Ctrl_V, reglen + 1);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    buf[0] = NUL;
	    buflen = 0;
	    ଭାଙ୍ଗ;
    }
    (ଶୂନ୍ୟ)dict_add_string_len(dict, "regtype", buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);
}

    ଶୂନ୍ୟ
yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ଯଦି (recursive)
	ଫେରନ୍ତୁ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ;

    // yanked text contents
    ପାଇଁ (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n].string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)reg->y_array[n].length);
    list->lv_lock = VAR_FIXED;
    ଯଦି (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ଶୂନ୍ୟ)dict_add_string_len(v_event, "regname", buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);

    // motion type: inclusive or exclusive
    (ଶୂନ୍ୟ)dict_add_bool(v_event, "inclusive", oap->inclusive);

    // kind of operation (yank, delete, change)
    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    buflen = (buf[0] == NUL) ? 0 : (buf[1] == NUL) ? 1 : 2;
    (ଶୂନ୍ୟ)dict_add_string_len(v_event, "operator", buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);

    add_regtype_to_dict(oap->regname, v_event, buf, ଆକାର(buf));

    // selection type - visual or not
    (ଶୂନ୍ୟ)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, ଖାଲି, ଖାଲି, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}

    ସ୍ଥିର ଶୂନ୍ୟ
put_do_autocmd(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    regname,
	yankreg_T   *reg,	// May be NULL, if special register
	string_T    *ଭର୍ତ୍ତି,	// Not NULL if special register, except '.'
	bool	    post,	// If Post or Pre
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    dir)	// BACKWARD for 'P', FORWARD for 'p'
{
    ସ୍ଥିର bool	    recursive = false;
    dict_T	    *v_event;
    list_T	    *list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    n;
    char_u	    buf[NUMBUFLEN + 2];
    size_t	    buflen;
    save_v_event_T  save_v_event;

    ଯଦି (recursive || regname == '_')
	ଫେରନ୍ତୁ;

    ଯଦି (regname != '.' && ଭର୍ତ୍ତି == ଖାଲି
	    && reg == ଖାଲି)
	// Can happen when pasting text in normal mode in a terminal buffer
	ଫେରନ୍ତୁ;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Make sure regcontents will be up to date
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ଯଦି (regname == '.')
    {
	ଯଦି (last_ଭର୍ତ୍ତି_ga.ga_data != ଖାଲି)
	    // Get the last inserted text to place in "regcontents"
	    list_append_string(list, last_ଭର୍ତ୍ତି_ga.ga_data,
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)last_ଭର୍ତ୍ତି_ga.ga_len);
    }
    ଅନ୍ୟଥା ଯଦି (ଭର୍ତ୍ତି != ଖାଲି)
    {
	list_append_string(list, ଭର୍ତ୍ତି->string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଭର୍ତ୍ତି->length);
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (n = 0; n < reg->y_size; n++)
	    list_append_string(list, reg->y_array[n].string,
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)reg->y_array[n].length);
    }

    list->lv_lock = VAR_FIXED;
    ଯଦି (dict_add_list(v_event, "regcontents", list) == FAIL)
	list_unref(list);

    // register name or empty string for unnamed operation
    buf[0] = (char_u)regname;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    (ଶୂନ୍ୟ)dict_add_string_len(v_event, "regname", buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);

    // kind of operation (P, p)
    buf[0] = dir == BACKWARD ? 'P' : 'p';
    buf[1] = NUL;
    buflen = 1;
    (ଶୂନ୍ୟ)dict_add_string_len(v_event, "operator", buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);

    add_regtype_to_dict(regname, v_event, buf, ଆକାର(buf));
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif

    (ଶୂନ୍ୟ)dict_add_bool(v_event, "visual", VIsual_active);

    // Lock the dictionary and its keys
    dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(v_event);

    recursive = true;
    textlock++;
    ଯଦି (post)
	apply_autocmds(EVENT_TEXTPUTPOST, ଖାଲି, ଖାଲି, FALSE, curbuf);
    ଅନ୍ୟଥା
	apply_autocmds(EVENT_TEXTPUTPRE, ଖାଲି, ଖାଲି, FALSE, curbuf);
    textlock--;
    recursive = false;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * set all the yank registers to empty (called from main())
 */
    ଶୂନ୍ୟ
init_yank(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_REGISTERS; ++i)
	y_regs[i].y_array = ଖାଲି;
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
clear_registers(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	ଯଦି (y_current->y_array != ଖାଲି)
	    free_yank_all();
    }
}
#endif

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_yank(long n)
{
    ଯଦି (y_current->y_array == ଖାଲି)
	ଫେରନ୍ତୁ;

    long	    i;

    ପାଇଁ (i = n; --i >= 0; )
	VIM_CLEAR_STRING(y_current->y_array[i]);
    VIM_CLEAR(y_current->y_array);
}

    ଶୂନ୍ୟ
free_yank_all(ଶୂନ୍ୟ)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
op_yank(oparg_T *oap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା deleting, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mess)
{
    long		y_idx;		// index in y_array[]
    yankreg_T		*curr;		// copy of y_current
    yankreg_T		newreg;		// new yank register when appending
    linenr_T		lnum;		// current line number
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			yanktype = oap->motion_type;
    long		yanklines = oap->ଧାଡ଼ି_ଗଣନା;
    linenr_T		yankendlnum = oap->end.lnum;
    char_u		*pnew;
    ସଂରଚନା block_def	bd;

				    // check for read-only register
    ଯଦି (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (oap->regname == '_')	    // black hole: nothing to do
	ଫେରନ୍ତୁ OK;

#ifdef FEAT_CLIPBOARD
    ଯଦି ((!clip_star.available && oap->regname == '*') ||
	(!clip_plus.available && oap->regname == '+'))
    {
	oap->regname = 0;
	ସନ୍ଦେଶ_warn_missing_clipboard();
    }
#endif

    ଯଦି (!deleting)		    // op_delete() already set y_current
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    // append to existing contents
    ଯଦି (y_append && y_current->y_array != ଖାଲି)
	y_current = &newreg;
    ଅନ୍ୟଥା
	free_yank_all();	    // free previously yanked lines

    // If the cursor was in column 1 before and after the movement, and the
    // operator is not inclusive, the yank is always linewise.
    ଯଦି (       oap->motion_type == MCHAR
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
    y_current->y_array = lalloc_clear(ଆକାର(string_T) * yanklines, TRUE);
    ଯଦି (y_current->y_array == ଖାଲି)
    {
	y_current = curr;
	ଫେରନ୍ତୁ FAIL;
    }
#ifdef FEAT_VIMINFO
    y_current->y_time_set = vim_time();
#endif

    y_idx = 0;
    lnum = oap->start.lnum;

    ଯଦି (oap->block_mode)
    {
	// Visual block mode
	y_current->y_type = MBLOCK;	    // set the yank register type
	y_current->y_width = oap->end_vcol - oap->start_vcol;

	ଯଦି (curwin->w_curswant == MAXCOL && y_current->y_width > 0)
	    y_current->y_width--;
    }

    ପାଇଁ ( ; lnum <= yankendlnum; lnum++, y_idx++)
    {
	ବାଛ (y_current->y_type)
	{
	    ସ୍ଥିତି MBLOCK:
		block_prep(oap, &bd, lnum, FALSE);
		ଯଦି (yank_copy_ଧାଡ଼ି(&bd, y_idx, oap->excl_tr_ws) == FAIL)
		    ଯାଅ fail;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି MLINE:
		y_current->y_array[y_idx].length = ml_get_len(lnum);
		ଯଦି ((y_current->y_array[y_idx].string =
					vim_strnsave(ml_get(lnum),
					y_current->y_array[y_idx].length)) == ଖାଲି)
		{
		    VIM_CLEAR_STRING(y_current->y_array[y_idx]);
		    ଯାଅ fail;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି MCHAR:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp;

		    charwise_block_prep(oap->start, oap->end, &bd, lnum, oap->inclusive);

		    // make sure bd.textlen is not longer than the text
		    tmp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(bd.textstart);
		    ଯଦି (tmp < bd.textlen)
			bd.textlen = tmp;

		    ଯଦି (yank_copy_ଧାଡ଼ି(&bd, y_idx, FALSE) == FAIL)
			ଯାଅ fail;
		    ଭାଙ୍ଗ;
		}
		// NOTREACHED
	}
    }

    ଯଦି (curr != y_current)	// append the new block to the old block
    {
	string_T *new_ptr;
	long j;

	new_ptr = ALLOC_MULT(string_T, curr->y_size + y_current->y_size);
	ଯଦି (new_ptr == ଖାଲି)
	    ଯାଅ fail;
	ପାଇଁ (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;
#ifdef FEAT_VIMINFO
	curr->y_time_set = vim_time();
#endif

	ଯଦି (yanktype == MLINE)	// MLINE overrides MCHAR and MBLOCK
	    curr->y_type = MLINE;

	// Concatenate the last line of the old block with the first line of
	// the new block, unless being Vi compatible.
	ଯଦି (curr->y_type == MCHAR && vim_strchr(p_cpo, CPO_REGAPPEND) == ଖାଲି)
	{
	    pnew = alloc(curr->y_array[curr->y_size - 1].length + y_current->y_array[0].length + 1);
	    ଯଦି (pnew == ଖାଲି)
	    {
		y_idx = y_current->y_size - 1;
		ଯାଅ fail;
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
	ଅନ୍ୟଥା
	    y_idx = 0;
	ଯେତେବେଳେ (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }

    ଯଦି (mess)			// Display message about yank?
    {
	ଯଦି (yanktype == MCHAR
		&& !oap->block_mode
		&& yanklines == 1)
	    yanklines = 0;
	// Some versions of Vi use ">=" here, some don't...
	ଯଦି (yanklines > p_report)
	{
	    ଅକ୍ଷର namebuf[100];

	    ଯଦି (oap->regname == NUL)
		*namebuf = NUL;
	    ଅନ୍ୟଥା
		vim_snprintf(namebuf, ଆକାର(namebuf),
						_(" into \"%c"), oap->regname);

	    // redisplay now, so message is not deleted
	    update_topline_redraw();
	    ଯଦି (oap->block_mode)
	    {
		smsg(NGETTEXT("block of %ld line yanked%s",
				     "block of %ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	    ଅନ୍ୟଥା
	    {
		smsg(NGETTEXT("%ld line yanked%s",
					      "%ld lines yanked%s", yanklines),
			yanklines, namebuf);
	    }
	}
    }

    ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	ଯଦି (yanktype == MLINE && !oap->block_mode)
	{
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.col = MAXCOL;
	}
	ଯଦି (yanktype != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    ଯଦି (curr == &y_regs[REAL_PLUS_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED_PLUS)))
    {
	ଯଦି (curr != &(y_regs[REAL_PLUS_REGISTER]))
	    copy_yank_reg(&(y_regs[REAL_PLUS_REGISTER]));
	call_clip_provider_set('+');
    }
    ଯଦି (curr == &y_regs[STAR_REGISTER] || (!deleting && oap->regname == 0
		&& (clip_unnamed & CLIP_UNNAMED)))
    {
	ଯଦି (curr != &(y_regs[STAR_REGISTER]))
	    copy_yank_reg(&(y_regs[STAR_REGISTER]));
	call_clip_provider_set('*');
    }
#endif

#ifdef FEAT_CLIPBOARD
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
    {
	// If we were yanking to the '*' register, send result to clipboard. If
	// no register was specified, and "unnamed" in 'clipboard', make a copy
	// to the '*' register.
	ଯଦି (clip_star.available
		&& (curr == &(y_regs[STAR_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) & CLIP_UNNAMED))))
	{
	    ଯଦି (curr != &(y_regs[STAR_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[STAR_REGISTER]));

	    clip_own_selection(&clip_star);
	    clip_gen_set_selection(&clip_star);
	}

# ଯଦି defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	// If we were yanking to the '+' register, send result to selection.
	ଯଦି (clip_plus.available
		&& (curr == &(y_regs[PLUS_REGISTER])
		    || (!deleting && oap->regname == 0
			&& ((clip_unnamed | clip_unnamed_saved) &
			    CLIP_UNNAMED_PLUS))))
	{
	    ଯଦି (curr != &(y_regs[PLUS_REGISTER]))
		// Copy the text from register 0 to the clipboard register.
		copy_yank_reg(&(y_regs[PLUS_REGISTER]));

	    clip_own_selection(&clip_plus);
	    clip_gen_set_selection(&clip_plus);
	}
# endif
    }
#endif

#ଯଦି defined(FEAT_EVAL)
    ଯଦି (!deleting && has_textyankpost())
	yank_do_autocmd(oap, y_current);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    ଫେରନ୍ତୁ OK;

fail:		// free the allocated lines
    free_yank(y_idx + 1);
    y_current = curr;
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Copy a block range into a register.
 * If "exclude_trailing_space" is set, do not copy trailing whitespaces.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
yank_copy_ଧାଡ଼ି(ସଂରଚନା block_def *bd, long y_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exclude_trailing_space)
{
    char_u	*pnew;

    ଯଦି (exclude_trailing_space)
	bd->endspaces = 0;
    ଯଦି ((pnew = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1))
								      == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    y_current->y_array[y_idx].string = pnew;
    vim_memset(pnew, ' ', (size_t)bd->startspaces);
    pnew += bd->startspaces;
    mch_memmove(pnew, bd->textstart, (size_t)bd->textlen);
    pnew += bd->textlen;
    vim_memset(pnew, ' ', (size_t)bd->endspaces);
    pnew += bd->endspaces;
    ଯଦି (exclude_trailing_space)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା s = bd->textlen + bd->endspaces;

	ଯେତେବେଳେ (s > 0 && VIM_ISWHITE(*(bd->textstart + s - 1)))
	{
	    s = s - (*mb_head_off)(bd->textstart, bd->textstart + s - 1) - 1;
	    pnew--;
	}
    }
    *pnew = NUL;

    y_current->y_array[y_idx].length = (size_t)(pnew - y_current->y_array[y_idx].string);

    ଫେରନ୍ତୁ OK;
}

#ifdef HAVE_CLIPMETHOD
/*
 * Make a copy of the y_current register to register "reg".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    ଆକାର(string_T) * y_current->y_size, TRUE);
    ଯଦି (y_current->y_array == ଖାଲି)
	y_current->y_size = 0;
    ଅନ୍ୟଥା
	ପାଇଁ (j = 0; j < y_current->y_size; ++j)
	{
	    ଯଦି ((y_current->y_array[j].string = vim_strnsave(curr->y_array[j].string,
						curr->y_array[j].length)) == ଖାଲି)
	    {
		free_yank(j);
		y_current->y_size = 0;
		ଭାଙ୍ଗ;
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
    ଶୂନ୍ୟ
do_put(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname,
    char_u	*ପଦସମଷ୍ଟି_result,	// result for regname "=" when compiled
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,		// BACKWARD for 'P', FORWARD for 'p'
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    char_u	*ptr;
    char_u	*newp;
    char_u	*oldp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		yanklen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		totlen = 0;		// init for gcc
    linenr_T	lnum;
    colnr_T	col;
    long	i;			// index in y_array[]
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y_type;
    long	y_size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldlen;
    long	y_width = 0;
    colnr_T	vcol;
    string_T	*y_array = ଖାଲି;
    yankreg_T	*y_current_used = ଖାଲି;
    long	nr_ଧାଡ଼ିଗୁଡ଼ିକ = 0;
    string_T	ଭର୍ତ୍ତି_string;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		allocated = FALSE;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_ve_flags = get_ve_flags();

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
	// Adjust register name for "unnamed" in 'clipboard'.
	(ଶୂନ୍ୟ)may_get_selection(regname);
#endif

    // Remove any preinserted text (issue #19329)
    ଯଦି (ins_compl_preinsert_effect())
	ins_compl_delete();

    curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
    curbuf->b_op_end = curwin->w_cursor;	// default for '] mark

    // Using inserted text works differently, because the register includes
    // special characters (newlines, etc.).
    ଯଦି (regname == '.')
    {
	ଯଦି (VIsual_active)
	    stuffcharReadbuff(VIsual_mode);

#ifdef FEAT_EVAL
	bool has_textput_events = has_textputpre() || has_textputpost();
	ଯଦି (has_textput_events)
	    add_last_ଭର୍ତ୍ତି++;
#endif

	(ଶୂନ୍ୟ)stuff_inserted((dir == FORWARD ? (ଗଣନା_2 == -1 ? 'o' : 'a') :
				    (ଗଣନା_2 == -1 ? 'O' : 'i')), ଗଣନା_2, FALSE);

#ifdef FEAT_EVAL
	// Since the text is not inserted into the buffer immediately, just call
	// TextPutPost after TextPutPre.
	ଯଦି (has_textputpre())
	    put_do_autocmd('.', ଖାଲି, ଖାଲି, false, dir);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
#ifdef FEAT_EVAL
	ଯଦି (has_textputpost())
	    put_do_autocmd('.', ଖାଲି, ଖାଲି, true, dir);

	ଯଦି (has_textput_events && --add_last_ଭର୍ତ୍ତି == 0)
	    ga_clear(&last_ଭର୍ତ୍ତି_ga);
#endif

	// Putting the text is done later, so can't really move the cursor to
	// the next character.  Use "l" to simulate it.
	ଯଦି ((flags & PUT_CURSEND) && gchar_cursor() != NUL)
	    stuffcharReadbuff('l');
	ଫେରନ୍ତୁ;
    }

    // For special registers '%' (file name), '#' (alternate file name) and
    // ':' (last command line), etc. we have to create a fake yank register.
    // For compiled code "expr_result" holds the expression result.
    ଭର୍ତ୍ତି_string.string = ଖାଲି;
    ଯଦି (regname == '=' && ପଦସମଷ୍ଟି_result != ଖାଲି)
	ଭର୍ତ୍ତି_string.string = ପଦସମଷ୍ଟି_result;
    ଅନ୍ୟଥା ଯଦି (get_spec_reg(regname, &ଭର୍ତ୍ତି_string.string, &allocated, TRUE)
		&& ଭର୍ତ୍ତି_string.string == ଖାଲି)
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
	ଫେରନ୍ତୁ;
    }

    // Autocommands may be executed when saving lines for undo.  This might
    // make "y_array" invalid, so we start undo now to avoid that.
    ଯଦି (u_save(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1) == FAIL)
	ଯାଅ end;

    ଯଦି (ଭର୍ତ୍ତି_string.string != ଖାଲି)
    {
	ଭର୍ତ୍ତି_string.length = STRLEN(ଭର୍ତ୍ତି_string.string);

	y_type = MCHAR;
#ifdef FEAT_EVAL
	ଯଦି (regname == '=')
	{
	    size_t  ptrlen;
	    char_u  *tmp;

	    // For the = register we need to split the string at NL
	    // characters.
	    // Loop twice: count the number of lines and save them.
	    ପାଇଁ (;;)
	    {
		y_size = 0;
		ptr = ଭର୍ତ୍ତି_string.string;
		ptrlen = ଭର୍ତ୍ତି_string.length;
		ଯେତେବେଳେ (ptr != ଖାଲି)
		{
		    ଯଦି (y_array != ଖାଲି)
			y_array[y_size].string = ptr;
		    ++y_size;
		    tmp = vim_strchr(ptr, '\n');
		    ଯଦି (tmp == ଖାଲି)
		    {
			ଯଦି (y_array != ଖାଲି)
			    y_array[y_size - 1].length = ptrlen;
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (y_array != ଖାଲି)
			{
			    *tmp = NUL;
			    y_array[y_size - 1].length = (size_t)(tmp - ptr);
			    ptrlen -= y_array[y_size - 1].length + 1;
			}
			++tmp;
			// A trailing '\n' makes the register linewise.
			ଯଦି (*tmp == NUL)
			{
			    y_type = MLINE;
			    ଭାଙ୍ଗ;
			}
		    }
		    ptr = tmp;
		}
		ଯଦି (y_array != ଖାଲି)
		    ଭାଙ୍ଗ;
		y_array = ALLOC_MULT(string_T, y_size);
		ଯଦି (y_array == ଖାଲି)
		    ଯାଅ end;
	    }
	}
	ଅନ୍ୟଥା
#endif
	{
	    y_size = 1;		// use fake one-line yank register
	    y_array = &ଭର୍ତ୍ତି_string;
	}
#ifdef FEAT_EVAL
	ଯଦି (has_textputpre())
	    put_do_autocmd(regname, ଖାଲି, &ଭର୍ତ୍ତି_string, false, dir);
#endif
    }
    ଅନ୍ୟଥା
    {
#ifdef FEAT_EVAL
	ଯଦି (has_textputpre())
	{
	    // Make sure to call this before we set the variables, as setreg()
	    // may be called and invalidate them.
	    get_yank_register(regname, FALSE);
	    put_do_autocmd(regname, y_current, ଖାଲି, false, dir);
	}
#endif
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_width = y_current->y_width;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
	y_current_used = y_current;
    }

    ଯଦି (y_type == MLINE)
    {
	ଯଦି (flags & PUT_ଧାଡ଼ି_SPLIT)
	{
	    char_u *p_orig;
	    char_u *p;
	    size_t plen;

	    // "p" or "P" in Visual mode: split the lines to put the text in
	    // between.
	    ଯଦି (u_save_cursor() == FAIL)
		ଯାଅ end;
	    p_orig = p = ml_get_cursor();
	    plen = ml_get_cursor_len();
	    ଯଦି (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(p, plen - (size_t)(p - p_orig));
	    ଯଦି (ptr == ଖାଲି)
		ଯାଅ end;
	    ml_append(curwin->w_cursor.lnum, ptr, (colnr_T)0, FALSE);
	    vim_free(ptr);

	    oldp = ml_get_curline();
	    p = oldp + curwin->w_cursor.col;
	    ଯଦି (dir == FORWARD && *p != NUL)
		MB_PTR_ADV(p);
	    ptr = vim_strnsave(oldp, (size_t)(p - oldp));
	    ଯଦି (ptr == ଖାଲି)
		ଯାଅ end;
	    ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
	    ++nr_ଧାଡ଼ିଗୁଡ଼ିକ;
	    dir = FORWARD;
	}
	ଯଦି (flags & PUT_ଧାଡ଼ି_FORWARD)
	{
	    // Must be "p" for a Visual block, put lines below the block.
	    curwin->w_cursor = curbuf->b_visual.vi_end;
	    dir = FORWARD;
	}
	curbuf->b_op_start = curwin->w_cursor;	// default for '[ mark
	curbuf->b_op_end = curwin->w_cursor;	// default for '] mark
    }

    ଯଦି (flags & PUT_ଧାଡ଼ି)	// :put command or "p" in Visual line mode.
	y_type = MLINE;

    ଯଦି (y_size == 0 || y_array == ଖାଲି)
    {
	semsg(_(e_nothing_in_register_str),
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	ଯାଅ end;
    }

    ଯଦି (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	ଯଦି (lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1;
	ଯଦି (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    ଯାଅ end;
    }
    ଅନ୍ୟଥା ଯଦି (y_type == MLINE)
    {
	lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
	// Correct line number for closed fold.  Don't move the cursor yet,
	// u_save() uses it.
	ଯଦି (dir == BACKWARD)
	    (ଶୂନ୍ୟ)hasFolding(lnum, &lnum, ଖାଲି);
	ଅନ୍ୟଥା
	    (ଶୂନ୍ୟ)hasFolding(lnum, ଖାଲି, &lnum);
#endif
	ଯଦି (dir == FORWARD)
	    ++lnum;
	// In an empty buffer the empty line is going to be replaced, include
	// it in the saved lines.
	ଯଦି ((BUFEMPTY() ? u_save(0, 2) : u_save(lnum - 1, lnum)) == FAIL)
	    ଯାଅ end;
#ifdef FEAT_FOLDING
	ଯଦି (dir == FORWARD)
	    curwin->w_cursor.lnum = lnum - 1;
	ଅନ୍ୟଥା
	    curwin->w_cursor.lnum = lnum;
	curbuf->b_op_start = curwin->w_cursor;	// for mark_adjust()
#endif
    }
    ଅନ୍ୟଥା ଯଦି (u_save_cursor() == FAIL)
	ଯାଅ end;

    ଯଦି (cur_ve_flags == VE_ALL && y_type == MCHAR)
    {
	ଯଦି (gchar_cursor() == TAB)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା viscol = getviscol();
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ts = curbuf->b_p_ts;

	    // Don't need to insert spaces when "p" on the last position of a
	    // tab or "P" on the first position.
	    ଯଦି (dir == FORWARD ?
#ifdef FEAT_VARTABS
		    tabstop_padding(viscol, ts, curbuf->b_p_vts_array) != 1
#ଅନ୍ୟଥା
		    ts - (viscol % ts) != 1
#endif
		    : curwin->w_cursor.coladd > 0)
		coladvance_force(viscol);
	    ଅନ୍ୟଥା
		curwin->w_cursor.coladd = 0;
	}
	ଅନ୍ୟଥା ଯଦି (curwin->w_cursor.coladd > 0 || gchar_cursor() == NUL)
	    coladvance_force(getviscol() + (dir == FORWARD));
    }

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;

    // Block mode
    ଯଦି (y_type == MBLOCK)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	delcount;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	incr = 0;
	ସଂରଚନା block_def bd;
	long	j;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c = gchar_cursor();
	colnr_T	endcol2 = 0;

	ଯଦି (dir == FORWARD && c != NUL)
	{
	    ଯଦି (cur_ve_flags == VE_ALL)
		getvcol(curwin, &curwin->w_cursor, &col, ଖାଲି, &endcol2, 0);
	    ଅନ୍ୟଥା
		getvcol(curwin, &curwin->w_cursor, ଖାଲି, ଖାଲି, &col, 0);

	    ଯଦି (has_mbyte)
		// move to start of next multi-byte character
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    ଅନ୍ୟଥା
		ଯଦି (c != TAB || cur_ve_flags != VE_ALL)
		    ++curwin->w_cursor.col;
	    ++col;
	}
	ଅନ୍ୟଥା
	    getvcol(curwin, &curwin->w_cursor, &col, ଖାଲି, &endcol2, 0);

	col += curwin->w_cursor.coladd;
	ଯଦି (cur_ve_flags == VE_ALL
		&& (curwin->w_cursor.coladd > 0
		    || endcol2 == curwin->w_cursor.col))
	{
	    ଯଦି (dir == FORWARD && c == NUL)
		++col;
	    ଯଦି (dir != FORWARD && c != NUL && curwin->w_cursor.coladd > 0)
		++curwin->w_cursor.col;
	    ଯଦି (c == TAB)
	    {
		ଯଦି (dir == BACKWARD && curwin->w_cursor.col)
		    curwin->w_cursor.col--;
		ଯଦି (dir == FORWARD && col - 1 == endcol2)
		    curwin->w_cursor.col++;
	    }
	}
	curwin->w_cursor.coladd = 0;
	bd.textcol = 0;
	ପାଇଁ (i = 0; i < y_size; ++i)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    spaces = 0;
	    ଅକ୍ଷର	    shortline;
	    chartabsize_T   cts;

	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    vcol = 0;
	    delcount = 0;

	    // add a new line
	    ଯଦି (curwin->w_cursor.lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    {
		ଯଦି (ml_append(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, (char_u *)"",
						   (colnr_T)1, FALSE) == FAIL)
		    ଭାଙ୍ଗ;
		++nr_ଧାଡ଼ିଗୁଡ଼ିକ;
	    }
	    // get the old line and advance to the position to insert at
	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum, 0,
								  oldp, oldp);

	    ଯେତେବେଳେ (cts.cts_vcol < col && *cts.cts_ptr != NUL)
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

	    ଯଦି (vcol < col) // line too short, pad with spaces
		bd.startspaces = col - vcol;
	    ଅନ୍ୟଥା ଯଦି (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
		ଯଦି (has_mbyte)
		    bd.textcol -= (*mb_head_off)(oldp, oldp + bd.textcol);
		ଯଦି (oldp[bd.textcol] != TAB)
		{
		    // Only a Tab can be split into spaces.  Other
		    // characters will have to be moved to after the
		    // block, causing misalignment.
		    delcount = 0;
		    bd.endspaces = 0;
		}
	    }

	    yanklen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y_array[i].length;

	    ଯଦି ((flags & PUT_BLOCK_INNER) == 0)
	    {
		// calculate number of spaces required to fill right side of
		// block
		spaces = y_width + 1;
		init_chartabsize_arg(&cts, curwin, 0, 0,
						      y_array[i].string, y_array[i].string);

		ଯେତେବେଳେ (*cts.cts_ptr != NUL)
		{
		    spaces -= lbr_chartabsize_adv(&cts);
		    cts.cts_vcol = 0;
		}
		clear_chartabsize_arg(&cts);
		ଯଦି (spaces < 0)
		    spaces = 0;
	    }

	    // Insert the new text.
	    // First check for multiplication overflow.
	    ଯଦି (yanklen + spaces != 0
		     && ଗଣନା_2 > ((INT_MAX - (bd.startspaces + bd.endspaces))
							/ (yanklen + spaces)))
	    {
		emsg(_(e_resulting_text_too_long));
		ଭାଙ୍ଗ;
	    }

	    totlen = ଗଣନା_2 * (yanklen + spaces) + bd.startspaces + bd.endspaces;
	    newp = alloc(totlen + oldlen + 1);
	    ଯଦି (newp == ଖାଲି)
		ଭାଙ୍ଗ;

	    // copy part up to cursor to new line
	    ptr = newp;
	    mch_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;

	    // may insert some spaces before the new text
	    vim_memset(ptr, ' ', (size_t)bd.startspaces);
	    ptr += bd.startspaces;

	    // insert the new text
	    ପାଇଁ (j = 0; j < ଗଣନା_2; ++j)
	    {
		mch_memmove(ptr, y_array[i].string, (size_t)yanklen);
		ptr += yanklen;

		// insert block's trailing spaces only if there's text behind
		ଯଦି ((j < ଗଣନା_2 - 1 || !shortline) && spaces > 0)
		{
		    vim_memset(ptr, ' ', (size_t)spaces);
		    ptr += spaces;
		}
		ଅନ୍ୟଥା
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
	    ଯଦି (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}

	changed_ଧାଡ଼ିଗୁଡ଼ିକ(lnum, 0, curwin->w_cursor.lnum - nr_ଧାଡ଼ିଗୁଡ଼ିକ, nr_ଧାଡ଼ିଗୁଡ଼ିକ);

	// Set '[ mark.
	curbuf->b_op_start = curwin->w_cursor;
	curbuf->b_op_start.lnum = lnum;

	// adjust '] mark
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	ଯଦି (curbuf->b_op_end.col < 0)
	    curbuf->b_op_end.col = 0;
	curbuf->b_op_end.coladd = 0;
	ଯଦି (flags & PUT_CURSEND)
	{
	    colnr_T len;

	    curwin->w_cursor = curbuf->b_op_end;
	    curwin->w_cursor.col++;

	    // in Insert mode we might be after the NUL, correct for that
	    len = ml_get_curline_len();
	    ଯଦି (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}
	ଅନ୍ୟଥା
	    curwin->w_cursor.lnum = lnum;
    }
    ଅନ୍ୟଥା
    {
	pos_T	new_cursor;

	yanklen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y_array[0].length;

	// Character or Line mode
	ଯଦି (y_type == MCHAR)
	{
	    // if type is MCHAR, FORWARD is the same as BACKWARD on the next
	    // char
	    ଯଦି (dir == FORWARD && gchar_cursor() != NUL)
	    {
		ଯଦି (has_mbyte)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytelen = (*mb_ptr2len)(ml_get_cursor());

		    // put it on the next of the multi-byte character.
		    col += bytelen;
		    ଯଦି (yanklen)
		    {
			curwin->w_cursor.col += bytelen;
			curbuf->b_op_end.col += bytelen;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ++col;
		    ଯଦି (yanklen)
		    {
			++curwin->w_cursor.col;
			++curbuf->b_op_end.col;
		    }
		}
	    }
	    curbuf->b_op_start = curwin->w_cursor;
	}
	// Line mode: BACKWARD is the same as FORWARD on the previous line
	ଅନ୍ୟଥା ଯଦି (dir == BACKWARD)
	    --lnum;
	new_cursor = curwin->w_cursor;

	// simple case: insert into one line at a time
	ଯଦି (y_type == MCHAR && y_size == 1)
	{
	    linenr_T	end_lnum = 0; // init for gcc
	    linenr_T	start_lnum = lnum;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_byte_off = 0;

	    ଯଦି (VIsual_active)
	    {
		end_lnum = curbuf->b_visual.vi_end.lnum;
		ଯଦି (end_lnum < curbuf->b_visual.vi_start.lnum)
		    end_lnum = curbuf->b_visual.vi_start.lnum;
		ଯଦି (end_lnum > start_lnum)
		{
		    pos_T   pos;

		    // "col" is valid for the first line, in following lines
		    // the virtual column needs to be used.  Matters for
		    // multi-byte characters.
		    pos.lnum = lnum;
		    pos.col = col;
		    pos.coladd = 0;
		    getvcol(curwin, &pos, ଖାଲି, &vcol, ଖାଲି, 0);
		}
	    }

	    ଯଦି (ଗଣନା_2 == 0 || yanklen == 0)
	    {
		ଯଦି (VIsual_active)
		    lnum = end_lnum;
	    }
	    ଅନ୍ୟଥା ଯଦି (ଗଣନା_2 > INT_MAX / yanklen)
		// multiplication overflow
		emsg(_(e_resulting_text_too_long));
	    ଅନ୍ୟଥା
	    {
		totlen = ଗଣନା_2 * yanklen;
		କର
		{
		    oldp = ml_get(lnum);
		    oldlen = ml_get_len(lnum);
		    ଯଦି (lnum > start_lnum)
		    {
			pos_T   pos;

			pos.lnum = lnum;
			ଯଦି (getvpos(&pos, vcol) == OK)
			    col = pos.col;
			ଅନ୍ୟଥା
			    col = MAXCOL;
		    }
		    ଯଦି (VIsual_active && col > oldlen)
		    {
			lnum++;
			ଜାରି;
		    }
		    newp = alloc(totlen + oldlen + 1);
		    ଯଦି (newp == ଖାଲି)
			ଯାଅ end;	// alloc() gave an error message
		    mch_memmove(newp, oldp, (size_t)col);
		    ptr = newp + col;
		    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
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
		    ଯଦି (lnum == curwin->w_cursor.lnum)
		    {
			// make sure curwin->w_virtcol is updated
			changed_cline_bef_curs();
			invalidate_botline();
			curwin->w_cursor.col += (colnr_T)(totlen - 1);
		    }
		    ଯଦି (VIsual_active)
			lnum++;
		} ଯେତେବେଳେ (VIsual_active && lnum <= end_lnum);

		ଯଦି (VIsual_active) // reset lnum to the last visual line
		    lnum--;
	    }

	    // put '] at the first byte of the last character
	    curbuf->b_op_end = curwin->w_cursor;
	    curbuf->b_op_end.col -= first_byte_off;

	    // For "CTRL-O p" in Insert mode, put cursor after last char
	    ଯଦି (totlen && (restart_edit != 0 || (flags & PUT_CURSEND)))
		++curwin->w_cursor.col;
	    ଅନ୍ୟଥା
		curwin->w_cursor.col -= first_byte_off;
	}
	ଅନ୍ୟଥା
	{
	    linenr_T	new_lnum = new_cursor.lnum;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_indent = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent_diff = 0;	// init for gcc
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_indent = TRUE;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lendiff = 0;
	    long	cnt;

	    ଯଦି (flags & PUT_FIXINDENT)
		orig_indent = get_indent();

	    // Insert at least one line.  When y_type is MCHAR, break the first
	    // line in two.
	    ପାଇଁ (cnt = 1; cnt <= ଗଣନା_2; ++cnt)
	    {
		i = 0;
		ଯଦି (y_type == MCHAR)
		{
		    // Split the current line in two at the insert position.
		    // First insert y_array[size - 1] in front of second line.
		    // Then append y_array[0] to first line.
		    lnum = new_cursor.lnum;
		    ptr = ml_get(lnum) + col;
		    totlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y_array[y_size - 1].length;
		    newp = alloc(ml_get_len(lnum) - col + totlen + 1);
		    ଯଦି (newp == ଖାଲି)
			ଯାଅ error;
		    STRCPY(newp, y_array[y_size - 1].string);
		    STRCPY(newp + totlen, ptr);
		    // insert second line
		    ml_append(lnum, newp, (colnr_T)0, FALSE);
		    ++new_lnum;
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc(col + yanklen + 1);
		    ଯଦି (newp == ଖାଲି)
			ଯାଅ error;
					    // copy first part of line
		    mch_memmove(newp, oldp, (size_t)col);
					    // append to first line
		    mch_memmove(newp + col, y_array[0].string, (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		ପାଇଁ (; i < y_size; ++i)
		{
		    ଯଦି (y_type != MCHAR || i < y_size - 1)
		    {
			ଯଦି (ml_append(lnum, y_array[i].string, (colnr_T)0, FALSE)
								      == FAIL)
			    ଯାଅ error;
			new_lnum++;
		    }
		    lnum++;
		    ++nr_ଧାଡ଼ିଗୁଡ଼ିକ;
		    ଯଦି (flags & PUT_FIXINDENT)
		    {
			pos_T	old_pos = curwin->w_cursor;

			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
			ଯଦି (cnt == ଗଣନା_2 && i == y_size - 1)
			    lendiff = ml_get_len(lnum);
			ଯଦି (*ptr == '#' && preprocs_ବାମ())
			    indent = 0;     // Leave # lines at start
			ଅନ୍ୟଥା ଯଦି (*ptr == NUL)
			    indent = 0;     // Ignore empty lines
			ଅନ୍ୟଥା ଯଦି (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			ଅନ୍ୟଥା ଯଦି ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			(ଶୂନ୍ୟ)set_indent(indent, 0);
			curwin->w_cursor = old_pos;
			// remember how many chars were removed
			ଯଦି (cnt == ଗଣନା_2 && i == y_size - 1)
			    lendiff -= ml_get_len(lnum);
		    }
		}
		ଯଦି (cnt == 1)
		    new_lnum = lnum;
	    }

error:
	    // Adjust marks.
	    ଯଦି (y_type == MLINE)
	    {
		curbuf->b_op_start.col = 0;
		ଯଦି (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_T)MAXLNUM, nr_ଧାଡ଼ିଗୁଡ଼ିକ, 0L);

	    // note changed text for displaying and folding
	    ଯଦି (y_type == MCHAR)
		changed_ଧାଡ଼ିଗୁଡ଼ିକ(curwin->w_cursor.lnum, col,
					 curwin->w_cursor.lnum + 1, nr_ଧାଡ଼ିଗୁଡ଼ିକ);
	    ଅନ୍ୟଥା
		changed_ଧାଡ଼ିଗୁଡ଼ିକ(curbuf->b_op_start.lnum, 0,
					   curbuf->b_op_start.lnum, nr_ଧାଡ଼ିଗୁଡ଼ିକ);
	    ଯଦି (y_current_used != ଖାଲି && (y_current_used != y_current
					     || y_current->y_array != y_array))
	    {
		// Something invoked through changed_lines() has changed the
		// yank buffer, e.g. a GUI clipboard callback.
		emsg(_(e_yank_register_changed_while_using_it));
		ଯାଅ end;
	    }

	    // Put the '] mark on the first byte of the last inserted character.
	    // Correct the length for change in indent.
	    curbuf->b_op_end.lnum = new_lnum;
	    col = MAX(0, (colnr_T)y_array[y_size - 1].length - lendiff);
	    ଯଦି (col > 1)
	    {
		curbuf->b_op_end.col = col - 1;
		ଯଦି (y_array[y_size - 1].length > 0)
		    curbuf->b_op_end.col -= mb_head_off(y_array[y_size - 1].string,
						y_array[y_size - 1].string + y_array[y_size - 1].length - 1);
	    }
	    ଅନ୍ୟଥା
		curbuf->b_op_end.col = 0;

	    ଯଦି (flags & PUT_CURSLINE)
	    {
		// ":put": put cursor on last inserted line
		curwin->w_cursor.lnum = lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ଅନ୍ୟଥା ଯଦି (flags & PUT_CURSEND)
	    {
		// put cursor after inserted text
		ଯଦି (y_type == MLINE)
		{
		    ଯଦି (lnum >= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		    ଅନ୍ୟଥା
			curwin->w_cursor.lnum = lnum + 1;
		    curwin->w_cursor.col = 0;
		}
		ଅନ୍ୟଥା
		{
		    curwin->w_cursor.lnum = new_lnum;
		    curwin->w_cursor.col = col;
		    curbuf->b_op_end = curwin->w_cursor;
		    ଯଦି (col > 1)
			curbuf->b_op_end.col = col - 1;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (y_type == MLINE)
	    {
		// put cursor on first non-blank in first inserted line
		curwin->w_cursor.col = 0;
		ଯଦି (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    ଅନ୍ୟଥା	// put cursor on first inserted character
		curwin->w_cursor = new_cursor;
	}
    }

    msgmore(nr_ଧାଡ଼ିଗୁଡ଼ିକ);
    curwin->w_set_curswant = true;

    // Make sure the cursor is not after the NUL.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = ml_get_curline_len();
    ଯଦି (curwin->w_cursor.col > len)
    {
	ଯଦି (cur_ve_flags == VE_ALL)
	    curwin->w_cursor.coladd = curwin->w_cursor.col - len;
	curwin->w_cursor.col = len;
    }

end:
    ଯଦି (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif

#ifdef FEAT_EVAL
    ଯଦି (has_textputpost())
    {
	ଯଦି (ଭର୍ତ୍ତି_string.string == ଖାଲି)
	    put_do_autocmd(regname, y_current, ଖାଲି, true, dir);
	ଅନ୍ୟଥା
	    put_do_autocmd(regname, ଖାଲି, &ଭର୍ତ୍ତି_string, true, dir);
    }
#endif

    ଯଦି (allocated)
	vim_free(ଭର୍ତ୍ତି_string.string);
    ଯଦି (regname == '=')
	vim_free(y_array);

    VIsual_active = FALSE;

    // If the cursor is past the end of the line put it at the end.
    adjust_cursor_eol();
}

/*
 * Return the character name of the register with the given number.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_register_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ)
{
    ଯଦି (ଅଙ୍କ == -1)
	ଫେରନ୍ତୁ '"';
    ଅନ୍ୟଥା ଯଦି (ଅଙ୍କ < 10)
	ଫେରନ୍ତୁ ଅଙ୍କ + '0';
    ଅନ୍ୟଥା ଯଦି (ଅଙ୍କ == DELETION_REGISTER)
	ଫେରନ୍ତୁ '-';
#ଯଦି defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
    ଅନ୍ୟଥା ଯଦି (ଅଙ୍କ == STAR_REGISTER)
	ଫେରନ୍ତୁ '*';
    // If there is only one clipboard, we only want the plus register to point
    // to the star register if the clipboard provider is not being used. If the
    // clipboard provider is being used, then both registers should be available
    // no matter the platform
    ଅନ୍ୟଥା ଯଦି (clipmethod == CLIPMETHOD_PROVIDER && ଅଙ୍କ == REAL_PLUS_REGISTER)
	ଫେରନ୍ତୁ '+';
    ଅନ୍ୟଥା ଯଦି (clipmethod != CLIPMETHOD_PROVIDER && ଅଙ୍କ == PLUS_REGISTER)
	ଫେରନ୍ତୁ '+';
#endif
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଅଙ୍କ + 'a' - 10;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return the index of the register "" points to.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_unname_register(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ y_previous == ଖାଲି ? -1 : y_previous - &y_regs[0];
}
#endif

/*
 * ":dis" and ":registers": Display the contents of the yank registers.
 */
    ଶୂନ୍ୟ
ex_display(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		clen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;
    string_T	ଭର୍ତ୍ତି;

    silence_w23_w24_ସନ୍ଦେଶ++;
    ଯଦି (arg != ଖାଲି && *arg == NUL)
	arg = ଖାଲି;
    attr = HL_ATTR(HLF_8);

#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    // Highlight title
    ସନ୍ଦେଶ_puts_title(_("\nType Name Content"));
    ପାଇଁ (i = -1; i < ଅଙ୍କ_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	ବାଛ (get_reg_type(name, ଖାଲି))
	{
	    ସ୍ଥିତି MLINE: type = 'l'; ଭାଙ୍ଗ;
	    ସ୍ଥିତି MCHAR: type = 'c'; ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	type = 'b'; ଭାଙ୍ଗ;
	}
	ଯଦି (arg != ଖାଲି && vim_strchr(arg, name) == ଖାଲି
#ifdef ONE_CLIPBOARD
		// Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == ଖାଲି)
#endif
		)
	    ଜାରି;	    // did not ask for this register

#ifdef HAVE_CLIPMETHOD
	adjust_clip_reg(&name);
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	call_clip_provider_request(name);
#endif
#ifdef FEAT_CLIPBOARD
	ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
	    // Adjust register name for "unnamed" in 'clipboard'.
	    // When it's a clipboard register, fill it with the current contents
	    // of the clipboard.
	    (ଶୂନ୍ୟ)may_get_selection(name);
#endif

	ଯଦି (i == -1)
	{
	    ଯଦି (y_previous != ଖାଲି)
		yb = y_previous;
	    ଅନ୍ୟଥା
		yb = &(y_regs[0]);
	}
	ଅନ୍ୟଥା
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	ଯଦି (name == MB_TOLOWER(redir_reg)
		|| (vim_strchr((char_u *)"\"*+", redir_reg) != ଖାଲି &&
		    (yb == y_previous || yb == &y_regs[0])))
	    ଜାରି;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	ଯଦି (yb->y_array != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_show = FALSE;

	    ପାଇଁ (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j].string);

	    ଯଦି (do_show || yb->y_size == 0)
	    {
		ସନ୍ଦେଶ_putchar('\n');
		ସନ୍ଦେଶ_puts("  ");
		ସନ୍ଦେଶ_putchar(type);
		ସନ୍ଦେଶ_puts("  ");
		ସନ୍ଦେଶ_putchar('"');
		ସନ୍ଦେଶ_putchar(name);
		ସନ୍ଦେଶ_puts("   ");

		n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns - 11;
		ପାଇଁ (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    ଯଦି (j)
		    {
			ସନ୍ଦେଶ_puts_attr("^J", attr);
			n -= 2;
		    }
		    ପାଇଁ (p = yb->y_array[j].string;
				    *p != NUL && (n -= ptr2cells(p)) >= 0; ++p)
		    {
			clen = (*mb_ptr2len)(p);
			ସନ୍ଦେଶ_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		ଯଦି (n > 1 && yb->y_type == MLINE)
		    ସନ୍ଦେଶ_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    ଭର୍ତ୍ତି = get_last_ଭର୍ତ୍ତି();
    ଯଦି ((p = ଭର୍ତ୍ତି.string) != ଖାଲି
		  && (arg == ଖାଲି || vim_strchr(arg, '.') != ଖାଲି) && !got_int
						      && !message_filtered(p))
    {
	ସନ୍ଦେଶ_puts("\n  c  \".   ");
	dis_ସନ୍ଦେଶ(p, TRUE);
    }

    // display last command line
    ଯଦି (last_cmdline != ଖାଲି && (arg == ଖାଲି || vim_strchr(arg, ':') != ଖାଲି)
			       && !got_int && !message_filtered(last_cmdline))
    {
	ସନ୍ଦେଶ_puts("\n  c  \":   ");
	dis_ସନ୍ଦେଶ(last_cmdline, FALSE);
    }

    // display current file name
    ଯଦି (curbuf->b_fname != ଖାଲି
	    && (arg == ଖାଲି || vim_strchr(arg, '%') != ଖାଲି) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	ସନ୍ଦେଶ_puts("\n  c  \"%   ");
	dis_ସନ୍ଦେଶ(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    ଯଦି ((arg == ଖାଲି || vim_strchr(arg, '#') != ଖାଲି) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	ଯଦି (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    ସନ୍ଦେଶ_puts("\n  c  \"#   ");
	    dis_ସନ୍ଦେଶ(fname, FALSE);
	}
    }

    // display last search pattern
    ଯଦି (last_ସନ୍ଧାନ_pat() != ଖାଲି
		 && (arg == ଖାଲି || vim_strchr(arg, '/') != ଖାଲି) && !got_int
				      && !message_filtered(last_ସନ୍ଧାନ_pat()))
    {
	ସନ୍ଦେଶ_puts("\n  c  \"/   ");
	dis_ସନ୍ଦେଶ(last_ସନ୍ଧାନ_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    ଯଦି (ପଦସମଷ୍ଟି_ଧାଡ଼ି != ଖାଲି && (arg == ଖାଲି || vim_strchr(arg, '=') != ଖାଲି)
				  && !got_int && !message_filtered(ପଦସମଷ୍ଟି_ଧାଡ଼ି))
    {
	ସନ୍ଦେଶ_puts("\n  c  \"=   ");
	dis_ସନ୍ଦେଶ(ପଦସମଷ୍ଟି_ଧାଡ଼ି, FALSE);
    }
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
    silence_w23_w24_ସନ୍ଦେଶ--;
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    ସ୍ଥିର ଶୂନ୍ୟ
dis_ସନ୍ଦେଶ(
    char_u	*p,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_esc)	    // if TRUE, ignore trailing ESC
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns - 6;
    ଯେତେବେଳେ (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	ଯଦି (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ସନ୍ଦେଶ_outtrans_len(p, l);
	    p += l;
	}
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}

#ଯଦି defined(FEAT_DND)
/*
 * Replace the contents of the '~' register with str.
 */
    ଶୂନ୍ୟ
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
get_reg_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname, long *reglen)
{
    ବାଛ (regname)
    {
	ସ୍ଥିତି '%':		// file name
	ସ୍ଥିତି '#':		// alternate file name
	ସ୍ଥିତି '=':		// expression
	ସ୍ଥିତି ':':		// last command line
	ସ୍ଥିତି '/':		// last search-pattern
	ସ୍ଥିତି '.':		// last inserted text
	ସ୍ଥିତି Ctrl_F:		// Filename under cursor
	ସ୍ଥିତି Ctrl_P:		// Path under cursor, expand via "path"
	ସ୍ଥିତି Ctrl_W:		// word under cursor
	ସ୍ଥିତି Ctrl_A:		// WORD (mnemonic All) under cursor
	ସ୍ଥିତି '_':		// black hole: always empty
	    ଫେରନ୍ତୁ MCHAR;
    }

#ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
#endif
#ifdef FEAT_CLIPBOARD
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
#endif

    ଯଦି (regname != NUL && !valid_yank_reg(regname, FALSE))
	ଫେରନ୍ତୁ MAUTO;

    get_yank_register(regname, FALSE);

    ଯଦି (y_current->y_array != ଖାଲି)
    {
	ଯଦି (reglen != ଖାଲି && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	ଫେରନ୍ତୁ y_current->y_type;
    }
    ଫେରନ୍ତୁ MAUTO;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * When "flags" has GREG_LIST return a list with text "s".
 * Otherwise just return "s".
 */
    ସ୍ଥିର char_u *
getreg_wrap_one_ଧାଡ଼ି(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ଯଦି (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	ଯଦି (list != ଖାଲି)
	{
	    ଯଦି (list_append_string(list, ଖାଲି, -1) == FAIL)
	    {
		list_free(list);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	ଫେରନ୍ତୁ (char_u *)list;
    }
    ଫେରନ୍ତୁ s;
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
get_reg_contents(ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    linenr_T	i;
    char_u	*retval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    ଯଦି (regname == '=')
    {
	ଯଦି (flags & GREG_NO_ପଦସମଷ୍ଟି)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (flags & GREG_ପଦସମଷ୍ଟି_SRC)
	    ଫେରନ୍ତୁ getreg_wrap_one_ଧାଡ଼ି(get_ପଦସମଷ୍ଟି_ଧାଡ଼ି_src(), flags);
	ଫେରନ୍ତୁ getreg_wrap_one_ଧାଡ଼ି(get_ପଦସମଷ୍ଟି_ଧାଡ଼ି(), flags);
    }

    ଯଦି (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    ଯଦି (regname != NUL && !valid_yank_reg(regname, FALSE))
	ଫେରନ୍ତୁ ଖାଲି;

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_request(regname);
# endif
# ifdef FEAT_CLIPBOARD
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
	regname = may_get_selection(regname);
# endif

    ଯଦି (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	ଯଦି (retval == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (allocated)
	    ଫେରନ୍ତୁ getreg_wrap_one_ଧାଡ଼ି(retval, flags);
	ଫେରନ୍ତୁ getreg_wrap_one_ଧାଡ଼ି(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    ଯଦି (y_current->y_array == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();

	ଯଦି (list == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	ପାଇଁ (i = 0; i < y_current->y_size; ++i)
	{
	    ଯଦି (list_append_string(list, y_current->y_array[i].string,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y_current->y_array[i].length) == FAIL)
	    {
		list_free(list);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
	ଫେରନ୍ତୁ (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    ପାଇଁ (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ଯଦି (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);
    ଯଦି (retval == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Copy the lines of the yank register into the string.
    len = 0;
    ପାଇଁ (i = 0; i < y_current->y_size; ++i)
    {
	STRCPY(retval + len, y_current->y_array[i].string);
	len += (long)y_current->y_array[i].length;

	// Insert a newline between lines and after the last line if y_type is
	// MLINE.
	ଯଦି (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    retval[len++] = '\n';
    }
    retval[len] = NUL;

    ଫେରନ୍ତୁ retval;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
init_write_reg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		must_append,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*yank_type UNUSED)
{
    ଯଦି (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	ଫେରନ୍ତୁ FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    ଯଦି (!y_append && !must_append)
	free_yank_all();
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
finish_write_reg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name,
    yankreg_T	*old_y_previous,
    yankreg_T	*old_y_current)
{
# ifdef FEAT_CLIPBOARD
    // Send text of clipboard register to the clipboard.
    may_set_selection();
# endif

    // ':let @" = "val"' should change the meaning of the "" register
    ଯଦି (name != '"')
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
    ଶୂନ୍ୟ
write_reg_contents(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name,
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}

    ଶୂନ୍ୟ
write_reg_contents_lst(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name,
    char_u	**strings,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		must_append,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    ଯଦି (name == '/' || name == '=' || name == '#')
    {
	char_u	*s;

	ଯଦି (strings[0] == ଖାଲି)
	    s = (char_u *)"";
	ଅନ୍ୟଥା ଯଦି (strings[1] != ଖାଲି)
	{
	    semsg(_(e_register_char_cannot_contain_multiple_ଧାଡ଼ିଗୁଡ଼ିକ), name);
	    ଫେରନ୍ତୁ;
	}
	ଅନ୍ୟଥା
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (name == '_')	    // black hole: nothing to do
	ଫେରନ୍ତୁ;

    ଯଦି (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ଫେରନ୍ତୁ;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}

    ଶୂନ୍ୟ
write_reg_contents_ex(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name,
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		must_append,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    ଯଦି (maxlen >= 0)
	len = maxlen;
    ଅନ୍ୟଥା
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    ଯଦି (name == '/')
    {
	set_last_ସନ୍ଧାନ_pat(str, RE_ସନ୍ଧାନ, TRUE, TRUE);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (name == '#')
    {
	ଯଦି (len == 0)
	{
	  curwin->w_alt_fnum = 0; // clear altfile
	  ଫେରନ୍ତୁ;
	}

	buf_T	*buf;

	ଯଦି (VIM_ISDIGIT(*str))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଅଙ୍କ = atoi((ଅକ୍ଷର *)str);

	    buf = buflist_findnr(ଅଙ୍କ);
	    ଯଦି (buf == ଖାଲି)
		semsg(_(e_buffer_nr_does_not_exist), (long)ଅଙ୍କ);
	}
	ଅନ୍ୟଥା
	    buf = buflist_findnr(buflist_findpat(str, str + len,
							 TRUE, FALSE, FALSE));
	ଯଦି (buf == ଖାଲି)
	    ଫେରନ୍ତୁ;
	curwin->w_alt_fnum = buf->b_fnum;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, (size_t)len);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି (must_append && ପଦସମଷ୍ଟି_ଧାଡ଼ି != ଖାଲି)
	{
	    s = concat_str(ପଦସମଷ୍ଟି_ଧାଡ଼ି, p);
	    vim_free(p);
	    p = s;
	}
	set_ପଦସମଷ୍ଟି_ଧାଡ଼ି(p, ଖାଲି);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (name == '_')	    // black hole: nothing to do
	ଫେରନ୍ତୁ;

    ଯଦି (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	ଫେରନ୍ତୁ;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);

# ifdef FEAT_CLIPBOARD_PROVIDER
    call_clip_provider_set(name);
# endif
}
#endif	// FEAT_EVAL

#ଯଦି defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    ଶୂନ୍ୟ
str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		str_list)	// TRUE if str is char_u **
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;			// MCHAR, MLINE or MBLOCK
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum;
    long	start;
    long	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newlines;		// number of lines added
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extraline = 0;		// extra line at the end
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    string_T	*pp;
    long	maxlen;

    ଯଦି (y_ptr->y_array == ଖାଲି)		// NULL means empty register
	y_ptr->y_size = 0;

    ଯଦି (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    ଅନ୍ୟଥା
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    ଯଦି (str_list)
    {
	ପାଇଁ (ss = (char_u **) str; *ss != ଖାଲି; ++ss)
	    ++newlines;
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (i = 0; i < len; i++)
	    ଯଦି (str[i] == '\n')
		++newlines;
	ଯଦି (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	ଯଦି (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    ଯଦି (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	ଫେରନ୍ତୁ;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * ଆକାର(string_T), TRUE);
    ଯଦି (pp == ଖାଲି)	// out of memory
	ଫେରନ୍ତୁ;
    ପାଇଁ (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    ଯଦି (str_list)
    {
	ପାଇଁ (ss = (char_u **) str; *ss != ଖାଲି; ++ss, ++lnum)
	{
	    pp[lnum].length = STRLEN(*ss);
	    pp[lnum].string = vim_strnsave(*ss, pp[lnum].length);
	    ଯଦି (type == MBLOCK)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା charlen = vim_strsize(*ss);

		ଯଦି (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (start = 0; start < len + extraline; start += i + 1)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା charlen = 0;

	    ପାଇଁ (i = start; i < len;)	// find the end of the line
	    {
		ଯଦି (str[i] == '\n')
		    ଭାଙ୍ଗ;
		ଯଦି (type == MBLOCK)
		{
		    ଯଦି (str[i] >= 0x80)
			charlen += mb_ptr2cells_len(str + i, len - i);
		    ଅନ୍ୟଥା
			charlen += byte2cells(str[i]);
		}

		ଯଦି (str[i] == NUL)
		    i++; // registers can have NUL chars
		ଅନ୍ୟଥା
		    i += mb_ptr2len_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    ଯଦି (charlen > maxlen)
		maxlen = charlen;
	    ଯଦି (append)
	    {
		--lnum;
		extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y_ptr->y_array[lnum].length;
	    }
	    ଅନ୍ୟଥା
		extra = 0;
	    s = alloc(i + extra + 1);
	    ଯଦି (s == ଖାଲି)
		ଭାଙ୍ଗ;
	    ଯଦି (extra)
		mch_memmove(s, y_ptr->y_array[lnum].string, (size_t)extra);
	    ଯଦି (append)
		vim_free(y_ptr->y_array[lnum].string);
	    ଯଦି (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum].string = s;
	    y_ptr->y_array[lnum].length = extra;
	    ++lnum;
	    ଯେତେବେଳେ (--extra >= 0)
	    {
		ଯଦି (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    ଯଦି (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    ଅନ୍ୟଥା
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}
#endif // FEAT_CLIPBOARD || FEAT_EVAL
