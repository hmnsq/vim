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

#ضمن "vim.h"

ساكن histentry_T *(history[HIST_گنتی]) = {عدم, عدم, عدم, عدم, عدم};
ساكن صحيح	hisidx[HIST_گنتی] = {-1, -1, -1, -1, -1};  // lastused entry
ساكن صحيح	hisnum[HIST_گنتی] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
ساكن صحيح	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    صحيح
get_hislen(فراغ)
{
    أرجع hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(صحيح hist_type)
{
    أرجع history[hist_type];
}

#إذا defined(FEAT_VIMINFO)
    فراغ
سیٹ_histentry(صحيح hist_type, histentry_T *اندراج)
{
    history[hist_type] = اندراج;
}
#جے_انت

    صحيح *
get_hisidx(صحيح hist_type)
{
    أرجع &hisidx[hist_type];
}

#إذا defined(FEAT_VIMINFO)
    صحيح *
get_hisnum(صحيح hist_type)
{
    أرجع &hisnum[hist_type];
}
#جے_انت

/*
 * Translate a history character to the associated type number.
 */
    صحيح
hist_char2type(صحيح c)
{
    إذا (c == ':')
	أرجع HIST_CMD;
    إذا (c == '=')
	أرجع HIST_اظہاریہ;
    إذا (c == '@')
	أرجع HIST_داخلہ;
    إذا (c == '>')
	أرجع HIST_DEBUG;
    أرجع HIST_تلاش;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

ساكن حرف *(history_ناں[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    عدم
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp بن_ورتیا, صحيح idx)
{
    حرف    *short_ناں = ":=@>?/";
    صحيح	    short_ناں_گنتی = (صحيح)STRLEN(short_ناں);
    صحيح	    history_name_گنتی = ARRAY_لمائی(history_ناں) - 1;

    إذا (idx < short_ناں_گنتی)
    {
	xp->xp_buf[0] = (char_u)short_ناں[idx];
	xp->xp_buf[1] = NUL;
	أرجع xp->xp_buf;
    }
    إذا (idx < short_ناں_گنتی + history_name_گنتی)
	أرجع (char_u *)history_ناں[idx - short_ناں_گنتی];
    إذا (idx == short_ناں_گنتی + history_name_گنتی)
	أرجع (char_u *)"all";
    أرجع عدم;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    فراغ
init_history(فراغ)
{
    صحيح		newlen;	    // new length of history table
    histentry_T	*عارضی;
    صحيح		type;

    // If size of history table changed, reallocate it
    newlen = (صحيح)p_hi;
    إذا (newlen == hislen)		// history length didn't change
	أرجع;

    // history length changed
    لكل (type = 0; type < HIST_گنتی; ++type)   // adjust the tables
    {
	إذا (newlen > 0)
	{
	    عارضی = ALLOC_MULT(histentry_T, newlen);
	    إذا (عارضی == عدم)   // out of memory!
	    {
		إذا (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    اكسر;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		استمر;
	    }
	}
	وإلا
	    عارضی = عدم;

	إذا (hisidx[type] < 0)		// there are no entries yet
	{
	    صحيح	i;

	    لكل (i = 0; i < newlen; ++i)
		clear_hist_اندراج(&عارضی[i]);
	}
	وإلا إذا (newlen > hislen)	// array becomes bigger
	{
	    صحيح	i;
	    صحيح	j;

	    لكل (i = 0; i <= hisidx[type]; ++i)
		عارضی[i] = history[type][i];
	    j = i;
	    لكل ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_اندراج(&عارضی[i]);
	    لكل ( ; j < hislen; ++i, ++j)
		عارضی[i] = history[type][j];
	}
	وإلا				// array becomes smaller or 0
	{
	    صحيح	i;
	    صحيح	j;

	    j = hisidx[type];
	    لكل (i = newlen - 1; ; --i)
	    {
		إذا (i >= 0)		// copy newest entries
		    عارضی[i] = history[type][j];
		وإلا			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		إذا (--j < 0)
		    j = hislen - 1;
		إذا (j == hisidx[type])
		    اكسر;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = عارضی;
    }
    hislen = newlen;
}

    فراغ
clear_hist_اندراج(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = عدم;
    hisptr->hisstrlen = 0;
    hisptr->ویلا_سیٹ = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    صحيح
وچ_history(
    صحيح	    type,
    char_u  *str,
    صحيح	    move_منزل_front,	// Move the entry to the front if it exists
    صحيح	    sep,
    صحيح	    writing)		// ignore entries read from viminfo
{
    صحيح	    i;
    صحيح	    آخری_i = -1;
    char_u  *p;
    size_t  len;

    إذا (hisidx[type] < 0)
	أرجع FALSE;
    i = hisidx[type];
    افعل
    {
	إذا (history[type][i].hisstr == عدم)
	    أرجع FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	إذا (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_تلاش || sep == p[history[type][i].hisstrlen + 1]))
	{
	    إذا (!move_منزل_front)
		أرجع TRUE;
	    آخری_i = i;
	    اكسر;
	}
	إذا (--i < 0)
	    i = hislen - 1;
    } طالما (i != hisidx[type]);

    إذا (آخری_i < 0)
	أرجع FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    طالما (i != hisidx[type])
    {
	إذا (++i >= hislen)
	    i = 0;
	history[type][آخری_i] = history[type][i];
	آخری_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].ویلا_سیٹ = vim_ویلا();
    أرجع TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    ساكن صحيح
get_histtype(char_u *name)
{
    صحيح		i;
    صحيح		len = (صحيح)STRLEN(name);

    // No argument: use current history.
    إذا (len == 0)
	أرجع hist_char2type(get_cmdline_firstc());

    لكل (i = 0; history_ناں[i] != عدم; ++i)
	إذا (STRNICMP(name, history_ناں[i], len) == 0)
	    أرجع i;

    إذا (vim_strchr((char_u *)":=@>?/", name[0]) != عدم && name[1] == NUL)
	أرجع hist_char2type(name[0]);

    أرجع -1;
}

ساكن صحيح	آخری_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    فراغ
جوڑو_منزل_history(
    صحيح		histype,
    char_u	*new_اندراج,
    size_t	new_entrylen,
    صحيح		وچ_map,		// consider maptick when inside a mapping
    صحيح		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    إذا (hislen == 0)		// no history
	أرجع;

    إذا ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_تلاش)
	أرجع;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    إذا (histype == HIST_تلاش && وچ_map)
    {
	إذا (maptick == آخری_maptick && hisidx[HIST_تلاش] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_تلاش][hisidx[HIST_تلاش]];
	    vim_free(hisptr->hisstr);
	    clear_hist_اندراج(hisptr);
	    --hisnum[histype];
	    إذا (--hisidx[HIST_تلاش] < 0)
		hisidx[HIST_تلاش] = hislen - 1;
	}
	آخری_maptick = -1;
    }

    إذا (وچ_history(histype, new_اندراج, TRUE, sep, FALSE))
	أرجع;

    إذا (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_اندراج, new_entrylen + 2);
    إذا (hisptr->hisstr == عدم)
	hisptr->hisstrlen = 0;
    وإلا
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->ویلا_سیٹ = vim_ویلا();
    إذا (histype == HIST_تلاش && وچ_map)
	آخری_maptick = maptick;
}

#إذا defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
get_history_idx(صحيح histype)
{
    إذا (hislen == 0 || histype < 0 || histype >= HIST_گنتی
		    || hisidx[histype] < 0)
	أرجع -1;

    أرجع history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
calc_hist_idx(صحيح histype, صحيح عدد)
{
    صحيح		i;
    histentry_T	*hist;
    صحيح		wrapped = FALSE;

    إذا (hislen == 0 || histype < 0 || histype >= HIST_گنتی
		    || (i = hisidx[histype]) < 0 || عدد == 0)
	أرجع -1;

    hist = history[histype];
    إذا (عدد > 0)
    {
	طالما (hist[i].hisnum > عدد)
	    إذا (--i < 0)
	    {
		إذا (wrapped)
		    اكسر;
		i += hislen;
		wrapped = TRUE;
	    }
	إذا (i >= 0 && hist[i].hisnum == عدد && hist[i].hisstr != عدم)
	    أرجع i;
    }
    وإلا إذا (-عدد <= hislen)
    {
	i += عدد + 1;
	إذا (i < 0)
	    i += hislen;
	إذا (hist[i].hisstr != عدم)
	    أرجع i;
    }
    أرجع -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
clr_history(صحيح histype)
{
    صحيح		i;
    histentry_T	*hisptr;

    إذا (hislen != 0 && histype >= 0 && histype < HIST_گنتی)
    {
	hisptr = history[histype];
	لكل (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_اندراج(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	أرجع OK;
    }
    أرجع FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
del_history_اندراج(صحيح histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    صحيح		idx;
    صحيح		i;
    صحيح		آخری;
    صحيح		لبھیا = FALSE;

    إذا (hislen == 0 || histype < 0 || histype >= HIST_گنتی || *str == NUL
		|| hisidx[histype] < 0)
	أرجع FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    إذا (regmatch.regprog == عدم)
	أرجع FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = آخری = idx;
    افعل
    {
	hisptr = &history[histype][i];
	إذا (hisptr->hisstr == عدم)
	    اكسر;
	إذا (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    لبھیا = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_اندراج(hisptr);
	}
	وإلا
	{
	    إذا (i != آخری)
	    {
		history[histype][آخری] = *hisptr;
		clear_hist_اندراج(hisptr);
	    }
	    إذا (--آخری < 0)
		آخری += hislen;
	}
	إذا (--i < 0)
	    i += hislen;
    } طالما (i != idx);

    إذا (history[histype][idx].hisstr == عدم)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    أرجع لبھیا;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
del_history_idx(صحيح histype, صحيح idx)
{
    صحيح	    i, j;

    i = calc_hist_idx(histype, idx);
    إذا (i < 0)
	أرجع FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    إذا (histype == HIST_تلاش && maptick == آخری_maptick && i == idx)
	آخری_maptick = -1;

    طالما (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_اندراج(&history[histype][i]);
    إذا (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    أرجع TRUE;
}

/*
 * "histadd()" function
 */
    فراغ
f_histadd(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    صحيح		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_نمبر = FALSE;
    إذا (check_secure())
	أرجع;

    إذا (وچ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != عدم ? get_histtype(str) : -1;
    إذا (histype < 0)
	أرجع;

    str = tv_get_string_buf(&argvars[1], buf);
    إذا (*str == NUL)
	أرجع;

    init_history();
    جوڑو_منزل_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_نمبر = TRUE;
}

/*
 * "histdel()" function
 */
    فراغ
f_histdel(typval_T *argvars بن_ورتیا, typval_T *rettv بن_ورتیا)
{
    صحيح		n;
    char_u	*str;

    إذا (وچ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_نمبر_arg(argvars, 1) == FAIL))
	أرجع;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    إذا (str == عدم)
	n = 0;
    وإلا إذا (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    وإلا إذا (argvars[1].v_type == VAR_نمبر)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (صحيح)tv_get_نمبر(&argvars[1]));
    وإلا
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
    فراغ
f_histget(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    char_u  *str;

    إذا (وچ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	أرجع;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    إذا (str == عدم)
	rettv->vval.v_string = عدم;
    وإلا
    {
	صحيح type;
	صحيح idx;

	type = get_histtype(str);
	إذا (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	وإلا
	    idx = (صحيح)tv_get_نمبر_chk(&argvars[1], عدم);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	إذا (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	وإلا
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    فراغ
f_histnr(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    صحيح		i;
    char_u	*histname;

    إذا (وچ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == عدم ? HIST_CMD - 1 : get_histtype(histname);
    إذا (i >= HIST_CMD && i < HIST_گنتی)
	rettv->vval.v_نمبر = get_history_idx(i);
    وإلا
	rettv->vval.v_نمبر = -1;
}
#جے_انت // FEAT_EVAL

#إذا defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    فراغ
remove_کلید_from_history(فراغ)
{
    char_u	*p_شروع;
    char_u	*p_سرا;
    char_u	*p;
    صحيح		i;

    i = hisidx[HIST_CMD];
    إذا (i < 0)
	أرجع;
    p_شروع = history[HIST_CMD][i].hisstr;
    إذا (p_شروع == عدم)
	أرجع;

    p_سرا = p_شروع + history[HIST_CMD][i].hisstrlen;
    لكل (p = p_شروع; *p; ++p)
    {
	إذا (STRNCMP(p, "key", 3) == 0 && !محفوظ_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    إذا (p == عدم)
		اكسر;
	    ++p;
	    لكل (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		إذا (p[i] == '\\' && p[i + 1])
		    ++i;

	    mch_memmove(p, p + i, (p_سرا - (p + i)) + 1);	    // +1 for the NUL
	    p_سرا -= i;						    // adjust p_end for shortened string
	    --p;
	}
    }

    history[HIST_CMD][i].hisstrlen = (size_t)(p_سرا - p_شروع);
}
#جے_انت

/*
 * :history command - print a history
 */
    فراغ
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    صحيح		histype1 = HIST_CMD;
    صحيح		histype2 = HIST_CMD;
    صحيح		hisidx1 = 1;
    صحيح		hisidx2 = -1;
    صحيح		idx;
    صحيح		i, j, k;
    char_u	*سرا;
    char_u	*arg = eap->arg;

    إذا (hislen == 0)
    {
	پیغام(_("'history' option is zero"));
	أرجع;
    }

    إذا (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	سرا = arg;
	طالما (ASCII_ISALPHA(*سرا)
		|| vim_strchr((char_u *)":=@>/?", *سرا) != عدم)
	    سرا++;
	i = *سرا;
	*سرا = NUL;
	histype1 = get_histtype(arg);
	إذا (histype1 == -1)
	{
	    إذا (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_گنتی-1;
	    }
	    وإلا
	    {
		*سرا = i;
		semsg(_(e_trailing_characters_str), arg);
		أرجع;
	    }
	}
	وإلا
	    histype2 = histype1;
	*سرا = i;
    }
    وإلا
	سرا = arg;
    إذا (!get_list_range(&سرا, &hisidx1, &hisidx2) || *سرا != NUL)
    {
	إذا (*سرا != NUL)
	    semsg(_(e_trailing_characters_str), سرا);
	وإلا
	    semsg(_(e_val_too_large), arg);
	أرجع;
    }

    لكل (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "\n      #  %s history", history_ناں[histype1]);
	پیغام_puts_title((حرف *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	إذا (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	إذا (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	إذا (idx >= 0 && j <= k)
	    لكل (i = idx + 1; !got_int; ++i)
	    {
		إذا (i == hislen)
		    i = 0;
		إذا (hist[i].hisstr != عدم
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !سنیہا_filtered(hist[i].hisstr))
		{
		    صحيح  len;

		    پیغام_putchar('\n');
		    len = vim_snprintf((حرف *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    إذا (vim_strsize(hist[i].hisstr) > (صحيح)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (صحيح)Columns - 10, IOSIZE - (صحيح)len);
		    وإلا
			STRCPY(IObuff + len, hist[i].hisstr);
		    پیغام_outtrans(IObuff);
		    باہرلا_flush();
		}
		إذا (i == idx)
		    اكسر;
	    }
    }
}
