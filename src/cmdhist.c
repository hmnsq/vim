/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * cmdhist.c: Functions for the history of the command-line.
 */

#شامل "vim.h"

ساکن histentry_T *(history[HIST_گنتی]) = {تہی, تہی, تہی, تہی, تہی};
ساکن صحیح	hisidx[HIST_گنتی] = {-1, -1, -1, -1, -1};  // lastused entry
ساکن صحیح	hisnum[HIST_گنتی] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
ساکن صحیح	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    صحیح
get_hislen(خالی)
{
    واپس hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(صحیح hist_type)
{
    واپس history[hist_type];
}

#اگر defined(FEAT_VIMINFO)
    خالی
سیٹ_histentry(صحیح hist_type, histentry_T *اندراج_2)
{
    history[hist_type] = اندراج_2;
}
#اگر_ختم

    صحیح *
get_hisidx(صحیح hist_type)
{
    واپس &hisidx[hist_type];
}

#اگر defined(FEAT_VIMINFO)
    صحیح *
get_hisnum(صحیح hist_type)
{
    واپس &hisnum[hist_type];
}
#اگر_ختم

/*
 * Translate a history character to the associated type number.
 */
    صحیح
hist_char2type(صحیح c)
{
    اگر (c == ':')
	واپس HIST_CMD;
    اگر (c == '=')
	واپس HIST_اظہاریہ;
    اگر (c == '@')
	واپس HIST_داخلہ;
    اگر (c == '>')
	واپس HIST_DEBUG;
    واپس HIST_تلاش;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

ساکن حرف *(history_نام[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    تہی
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    حرف    *short_نام = ":=@>?/";
    صحیح	    short_نام_گنتی = (صحیح)STRLEN(short_نام);
    صحیح	    history_name_گنتی = ARRAY_لمبائی(history_نام) - 1;

    اگر (idx < short_نام_گنتی)
    {
	xp->xp_buf[0] = (char_u)short_نام[idx];
	xp->xp_buf[1] = NUL;
	واپس xp->xp_buf;
    }
    اگر (idx < short_نام_گنتی + history_name_گنتی)
	واپس (char_u *)history_نام[idx - short_نام_گنتی];
    اگر (idx == short_نام_گنتی + history_name_گنتی)
	واپس (char_u *)"all";
    واپس تہی;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    خالی
init_history(خالی)
{
    صحیح		newlen;	    // new length of history table
    histentry_T	*عارضی;
    صحیح		type;

    // If size of history table changed, reallocate it
    newlen = (صحیح)p_hi;
    اگر (newlen == hislen)		// history length didn't change
	واپس;

    // history length changed
    برائے (type = 0; type < HIST_گنتی; ++type)   // adjust the tables
    {
	اگر (newlen > 0)
	{
	    عارضی = ALLOC_MULT(histentry_T, newlen);
	    اگر (عارضی == تہی)   // out of memory!
	    {
		اگر (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    توڑو;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		جاری;
	    }
	}
	ورنہ
	    عارضی = تہی;

	اگر (hisidx[type] < 0)		// there are no entries yet
	{
	    صحیح	i;

	    برائے (i = 0; i < newlen; ++i)
		clear_hist_اندراج(&عارضی[i]);
	}
	ورنہ اگر (newlen > hislen)	// array becomes bigger
	{
	    صحیح	i;
	    صحیح	j;

	    برائے (i = 0; i <= hisidx[type]; ++i)
		عارضی[i] = history[type][i];
	    j = i;
	    برائے ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_اندراج(&عارضی[i]);
	    برائے ( ; j < hislen; ++i, ++j)
		عارضی[i] = history[type][j];
	}
	ورنہ				// array becomes smaller or 0
	{
	    صحیح	i;
	    صحیح	j;

	    j = hisidx[type];
	    برائے (i = newlen - 1; ; --i)
	    {
		اگر (i >= 0)		// copy newest entries
		    عارضی[i] = history[type][j];
		ورنہ			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		اگر (--j < 0)
		    j = hislen - 1;
		اگر (j == hisidx[type])
		    توڑو;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = عارضی;
    }
    hislen = newlen;
}

    خالی
clear_hist_اندراج(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = تہی;
    hisptr->hisstrlen = 0;
    hisptr->وقت_سیٹ = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    صحیح
اندر_history(
    صحیح	    type,
    char_u  *str,
    صحیح	    move_مقصد_front,	// Move the entry to the front if it exists
    صحیح	    sep,
    صحیح	    writing)		// ignore entries read from viminfo
{
    صحیح	    i;
    صحیح	    آخری_i = -1;
    char_u  *p;
    size_t  len;

    اگر (hisidx[type] < 0)
	واپس FALSE;
    i = hisidx[type];
    کرو
    {
	اگر (history[type][i].hisstr == تہی)
	    واپس FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	اگر (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_تلاش || sep == p[history[type][i].hisstrlen + 1]))
	{
	    اگر (!move_مقصد_front)
		واپس TRUE;
	    آخری_i = i;
	    توڑو;
	}
	اگر (--i < 0)
	    i = hislen - 1;
    } جب (i != hisidx[type]);

    اگر (آخری_i < 0)
	واپس FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    جب (i != hisidx[type])
    {
	اگر (++i >= hislen)
	    i = 0;
	history[type][آخری_i] = history[type][i];
	آخری_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].وقت_سیٹ = vim_وقت();
    واپس TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    ساکن صحیح
get_histtype(char_u *name)
{
    صحیح		i;
    صحیح		len = (صحیح)STRLEN(name);

    // No argument: use current history.
    اگر (len == 0)
	واپس hist_char2type(get_cmdline_firstc());

    برائے (i = 0; history_نام[i] != تہی; ++i)
	اگر (STRNICMP(name, history_نام[i], len) == 0)
	    واپس i;

    اگر (vim_strchr((char_u *)":=@>?/", name[0]) != تہی && name[1] == NUL)
	واپس hist_char2type(name[0]);

    واپس -1;
}

ساکن صحیح	آخری_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    خالی
جوڑو_مقصد_history(
    صحیح		histype,
    char_u	*new_اندراج,
    size_t	new_entrylen,
    صحیح		اندر_map,		// consider maptick when inside a mapping
    صحیح		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    اگر (hislen == 0)		// no history
	واپس;

    اگر ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_تلاش)
	واپس;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    اگر (histype == HIST_تلاش && اندر_map)
    {
	اگر (maptick == آخری_maptick && hisidx[HIST_تلاش] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_تلاش][hisidx[HIST_تلاش]];
	    vim_free(hisptr->hisstr);
	    clear_hist_اندراج(hisptr);
	    --hisnum[histype];
	    اگر (--hisidx[HIST_تلاش] < 0)
		hisidx[HIST_تلاش] = hislen - 1;
	}
	آخری_maptick = -1;
    }

    اگر (اندر_history(histype, new_اندراج, TRUE, sep, FALSE))
	واپس;

    اگر (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_اندراج, new_entrylen + 2);
    اگر (hisptr->hisstr == تہی)
	hisptr->hisstrlen = 0;
    ورنہ
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->وقت_سیٹ = vim_وقت();
    اگر (histype == HIST_تلاش && اندر_map)
	آخری_maptick = maptick;
}

#اگر defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ساکن صحیح
get_history_idx(صحیح histype)
{
    اگر (hislen == 0 || histype < 0 || histype >= HIST_گنتی
		    || hisidx[histype] < 0)
	واپس -1;

    واپس history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    ساکن صحیح
calc_hist_idx(صحیح histype, صحیح عدد)
{
    صحیح		i;
    histentry_T	*hist;
    صحیح		wrapped = FALSE;

    اگر (hislen == 0 || histype < 0 || histype >= HIST_گنتی
		    || (i = hisidx[histype]) < 0 || عدد == 0)
	واپس -1;

    hist = history[histype];
    اگر (عدد > 0)
    {
	جب (hist[i].hisnum > عدد)
	    اگر (--i < 0)
	    {
		اگر (wrapped)
		    توڑو;
		i += hislen;
		wrapped = TRUE;
	    }
	اگر (i >= 0 && hist[i].hisnum == عدد && hist[i].hisstr != تہی)
	    واپس i;
    }
    ورنہ اگر (-عدد <= hislen)
    {
	i += عدد + 1;
	اگر (i < 0)
	    i += hislen;
	اگر (hist[i].hisstr != تہی)
	    واپس i;
    }
    واپس -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    ساکن صحیح
clr_history(صحیح histype)
{
    صحیح		i;
    histentry_T	*hisptr;

    اگر (hislen != 0 && histype >= 0 && histype < HIST_گنتی)
    {
	hisptr = history[histype];
	برائے (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_اندراج(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	واپس OK;
    }
    واپس FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    ساکن صحیح
del_history_اندراج(صحیح histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    صحیح		idx;
    صحیح		i;
    صحیح		آخری;
    صحیح		ملا = FALSE;

    اگر (hislen == 0 || histype < 0 || histype >= HIST_گنتی || *str == NUL
		|| hisidx[histype] < 0)
	واپس FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    اگر (regmatch.regprog == تہی)
	واپس FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = آخری = idx;
    کرو
    {
	hisptr = &history[histype][i];
	اگر (hisptr->hisstr == تہی)
	    توڑو;
	اگر (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    ملا = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_اندراج(hisptr);
	}
	ورنہ
	{
	    اگر (i != آخری)
	    {
		history[histype][آخری] = *hisptr;
		clear_hist_اندراج(hisptr);
	    }
	    اگر (--آخری < 0)
		آخری += hislen;
	}
	اگر (--i < 0)
	    i += hislen;
    } جب (i != idx);

    اگر (history[histype][idx].hisstr == تہی)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    واپس ملا;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    ساکن صحیح
del_history_idx(صحیح histype, صحیح idx)
{
    صحیح	    i, j;

    i = calc_hist_idx(histype, idx);
    اگر (i < 0)
	واپس FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    اگر (histype == HIST_تلاش && maptick == آخری_maptick && i == idx)
	آخری_maptick = -1;

    جب (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_اندراج(&history[histype][i]);
    اگر (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    واپس TRUE;
}

/*
 * "histadd()" function
 */
    خالی
f_histadd(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_نمبر = FALSE;
    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != تہی ? get_histtype(str) : -1;
    اگر (histype < 0)
	واپس;

    str = tv_get_string_buf(&argvars[1], buf);
    اگر (*str == NUL)
	واپس;

    init_history();
    جوڑو_مقصد_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_نمبر = TRUE;
}

/*
 * "histdel()" function
 */
    خالی
f_histdel(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    صحیح		n;
    char_u	*str;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    اگر (str == تہی)
	n = 0;
    ورنہ اگر (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    ورنہ اگر (argvars[1].v_type == VAR_نمبر)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (صحیح)tv_get_نمبر(&argvars[1]));
    ورنہ
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_اندراج(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_نمبر = n;
}

/*
 * "histget()" function
 */
    خالی
f_histget(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    char_u  *str;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    اگر (str == تہی)
	rettv->vval.v_string = تہی;
    ورنہ
    {
	صحیح type;
	صحیح idx;

	type = get_histtype(str);
	اگر (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	ورنہ
	    idx = (صحیح)tv_get_نمبر_chk(&argvars[1], تہی);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	اگر (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	ورنہ
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    خالی
f_histnr(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح		i;
    char_u	*histname;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == تہی ? HIST_CMD - 1 : get_histtype(histname);
    اگر (i >= HIST_CMD && i < HIST_گنتی)
	rettv->vval.v_نمبر = get_history_idx(i);
    ورنہ
	rettv->vval.v_نمبر = -1;
}
#اگر_ختم // FEAT_EVAL

#اگر defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    خالی
remove_کلید_from_history(خالی)
{
    char_u	*p_آغاز;
    char_u	*p_انتہا;
    char_u	*p;
    صحیح		i;

    i = hisidx[HIST_CMD];
    اگر (i < 0)
	واپس;
    p_آغاز = history[HIST_CMD][i].hisstr;
    اگر (p_آغاز == تہی)
	واپس;

    p_انتہا = p_آغاز + history[HIST_CMD][i].hisstrlen;
    برائے (p = p_آغاز; *p; ++p)
    {
	اگر (STRNCMP(p, "key", 3) == 0 && !محفوظ_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    اگر (p == تہی)
		توڑو;
	    ++p;
	    برائے (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		اگر (p[i] == '\\' && p[i + 1])
		    ++i;

	    mch_memmove(p, p + i, (p_انتہا - (p + i)) + 1);	    // +1 for the NUL
	    p_انتہا -= i;						    // adjust p_end for shortened string
	    --p;
	}
    }

    history[HIST_CMD][i].hisstrlen = (size_t)(p_انتہا - p_آغاز);
}
#اگر_ختم

/*
 * :history command - print a history
 */
    خالی
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    صحیح		histype1 = HIST_CMD;
    صحیح		histype2 = HIST_CMD;
    صحیح		hisidx1 = 1;
    صحیح		hisidx2 = -1;
    صحیح		idx;
    صحیح		i, j, k;
    char_u	*انتہا;
    char_u	*arg = eap->arg;

    اگر (hislen == 0)
    {
	پیغام(_("'history' option is zero"));
	واپس;
    }

    اگر (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	انتہا = arg;
	جب (ASCII_ISALPHA(*انتہا)
		|| vim_strchr((char_u *)":=@>/?", *انتہا) != تہی)
	    انتہا++;
	i = *انتہا;
	*انتہا = NUL;
	histype1 = get_histtype(arg);
	اگر (histype1 == -1)
	{
	    اگر (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_گنتی-1;
	    }
	    ورنہ
	    {
		*انتہا = i;
		semsg(_(e_trailing_characters_str), arg);
		واپس;
	    }
	}
	ورنہ
	    histype2 = histype1;
	*انتہا = i;
    }
    ورنہ
	انتہا = arg;
    اگر (!get_list_range(&انتہا, &hisidx1, &hisidx2) || *انتہا != NUL)
    {
	اگر (*انتہا != NUL)
	    semsg(_(e_trailing_characters_str), انتہا);
	ورنہ
	    semsg(_(e_val_too_large), arg);
	واپس;
    }

    برائے (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "\n      #  %s history", history_نام[histype1]);
	پیغام_puts_title((حرف *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	اگر (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	اگر (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	اگر (idx >= 0 && j <= k)
	    برائے (i = idx + 1; !got_int; ++i)
	    {
		اگر (i == hislen)
		    i = 0;
		اگر (hist[i].hisstr != تہی
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !پیغام_filtered(hist[i].hisstr))
		{
		    صحیح  len;

		    پیغام_putchar('\n');
		    len = vim_snprintf((حرف *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    اگر (vim_strsize(hist[i].hisstr) > (صحیح)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (صحیح)Columns - 10, IOSIZE - (صحیح)len);
		    ورنہ
			STRCPY(IObuff + len, hist[i].hisstr);
		    پیغام_outtrans(IObuff);
		    برآمد_flush();
		}
		اگر (i == idx)
		    توڑو;
	    }
    }
}
