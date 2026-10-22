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

ساكن histentry_T *(history[HIST_شمېر]) = {عدم, عدم, عدم, عدم, عدم};
ساكن صحيح	hisidx[HIST_شمېر] = {-1, -1, -1, -1, -1};  // lastused entry
ساكن صحيح	hisnum[HIST_شمېر] = {0, 0, 0, 0, 0};
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
سېټ_histentry(صحيح hist_type, histentry_T *ثبت)
{
    history[hist_type] = ثبت;
}
#د_که_پای

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
#د_که_پای

/*
 * Translate a history character to the associated type number.
 */
    صحيح
hist_char2type(صحيح c)
{
    إذا (c == ':')
	أرجع HIST_CMD;
    إذا (c == '=')
	أرجع HIST_بیان;
    إذا (c == '@')
	أرجع HIST_ننوت;
    إذا (c == '>')
	أرجع HIST_DEBUG;
    أرجع HIST_پلټنه;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

ساكن حرف *(history_نومونه[]) =
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
get_history_arg(expand_T *xp ناکارول_شوی, صحيح idx)
{
    حرف    *short_نومونه = ":=@>?/";
    صحيح	    short_نومونه_شمېر = (صحيح)STRLEN(short_نومونه);
    صحيح	    history_name_شمېر = ARRAY_اوږدتیا(history_نومونه) - 1;

    إذا (idx < short_نومونه_شمېر)
    {
	xp->xp_buf[0] = (char_u)short_نومونه[idx];
	xp->xp_buf[1] = NUL;
	أرجع xp->xp_buf;
    }
    إذا (idx < short_نومونه_شمېر + history_name_شمېر)
	أرجع (char_u *)history_نومونه[idx - short_نومونه_شمېر];
    إذا (idx == short_نومونه_شمېر + history_name_شمېر)
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
    histentry_T	*لنډمهاله;
    صحيح		type;

    // If size of history table changed, reallocate it
    newlen = (صحيح)p_hi;
    إذا (newlen == hislen)		// history length didn't change
	أرجع;

    // history length changed
    لكل (type = 0; type < HIST_شمېر; ++type)   // adjust the tables
    {
	إذا (newlen > 0)
	{
	    لنډمهاله = ALLOC_MULT(histentry_T, newlen);
	    إذا (لنډمهاله == عدم)   // out of memory!
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
	    لنډمهاله = عدم;

	إذا (hisidx[type] < 0)		// there are no entries yet
	{
	    صحيح	i;

	    لكل (i = 0; i < newlen; ++i)
		clear_hist_ثبت(&لنډمهاله[i]);
	}
	وإلا إذا (newlen > hislen)	// array becomes bigger
	{
	    صحيح	i;
	    صحيح	j;

	    لكل (i = 0; i <= hisidx[type]; ++i)
		لنډمهاله[i] = history[type][i];
	    j = i;
	    لكل ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_ثبت(&لنډمهاله[i]);
	    لكل ( ; j < hislen; ++i, ++j)
		لنډمهاله[i] = history[type][j];
	}
	وإلا				// array becomes smaller or 0
	{
	    صحيح	i;
	    صحيح	j;

	    j = hisidx[type];
	    لكل (i = newlen - 1; ; --i)
	    {
		إذا (i >= 0)		// copy newest entries
		    لنډمهاله[i] = history[type][j];
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
	history[type] = لنډمهاله;
    }
    hislen = newlen;
}

    فراغ
clear_hist_ثبت(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = عدم;
    hisptr->hisstrlen = 0;
    hisptr->وخت_سېټ = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    صحيح
دننه_history(
    صحيح	    type,
    char_u  *str,
    صحيح	    move_منزل_front,	// Move the entry to the front if it exists
    صحيح	    sep,
    صحيح	    writing)		// ignore entries read from viminfo
{
    صحيح	    i;
    صحيح	    وروستی_i = -1;
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
		&& (type != HIST_پلټنه || sep == p[history[type][i].hisstrlen + 1]))
	{
	    إذا (!move_منزل_front)
		أرجع TRUE;
	    وروستی_i = i;
	    اكسر;
	}
	إذا (--i < 0)
	    i = hislen - 1;
    } طالما (i != hisidx[type]);

    إذا (وروستی_i < 0)
	أرجع FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    طالما (i != hisidx[type])
    {
	إذا (++i >= hislen)
	    i = 0;
	history[type][وروستی_i] = history[type][i];
	وروستی_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].وخت_سېټ = vim_وخت();
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

    لكل (i = 0; history_نومونه[i] != عدم; ++i)
	إذا (STRNICMP(name, history_نومونه[i], len) == 0)
	    أرجع i;

    إذا (vim_strchr((char_u *)":=@>?/", name[0]) != عدم && name[1] == NUL)
	أرجع hist_char2type(name[0]);

    أرجع -1;
}

ساكن صحيح	وروستی_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    فراغ
جمع_کړه_منزل_history(
    صحيح		histype,
    char_u	*new_ثبت,
    size_t	new_entrylen,
    صحيح		دننه_map,		// consider maptick when inside a mapping
    صحيح		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    إذا (hislen == 0)		// no history
	أرجع;

    إذا ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_پلټنه)
	أرجع;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    إذا (histype == HIST_پلټنه && دننه_map)
    {
	إذا (maptick == وروستی_maptick && hisidx[HIST_پلټنه] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_پلټنه][hisidx[HIST_پلټنه]];
	    vim_free(hisptr->hisstr);
	    clear_hist_ثبت(hisptr);
	    --hisnum[histype];
	    إذا (--hisidx[HIST_پلټنه] < 0)
		hisidx[HIST_پلټنه] = hislen - 1;
	}
	وروستی_maptick = -1;
    }

    إذا (دننه_history(histype, new_ثبت, TRUE, sep, FALSE))
	أرجع;

    إذا (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_ثبت, new_entrylen + 2);
    إذا (hisptr->hisstr == عدم)
	hisptr->hisstrlen = 0;
    وإلا
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->وخت_سېټ = vim_وخت();
    إذا (histype == HIST_پلټنه && دننه_map)
	وروستی_maptick = maptick;
}

#إذا defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
get_history_idx(صحيح histype)
{
    إذا (hislen == 0 || histype < 0 || histype >= HIST_شمېر
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

    إذا (hislen == 0 || histype < 0 || histype >= HIST_شمېر
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

    إذا (hislen != 0 && histype >= 0 && histype < HIST_شمېر)
    {
	hisptr = history[histype];
	لكل (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_ثبت(hisptr);
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
del_history_ثبت(صحيح histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    صحيح		idx;
    صحيح		i;
    صحيح		وروستی;
    صحيح		وموندل = FALSE;

    إذا (hislen == 0 || histype < 0 || histype >= HIST_شمېر || *str == NUL
		|| hisidx[histype] < 0)
	أرجع FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    إذا (regmatch.regprog == عدم)
	أرجع FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = وروستی = idx;
    افعل
    {
	hisptr = &history[histype][i];
	إذا (hisptr->hisstr == عدم)
	    اكسر;
	إذا (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    وموندل = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_ثبت(hisptr);
	}
	وإلا
	{
	    إذا (i != وروستی)
	    {
		history[histype][وروستی] = *hisptr;
		clear_hist_ثبت(hisptr);
	    }
	    إذا (--وروستی < 0)
		وروستی += hislen;
	}
	إذا (--i < 0)
	    i += hislen;
    } طالما (i != idx);

    إذا (history[histype][idx].hisstr == عدم)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    أرجع وموندل;
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
    إذا (histype == HIST_پلټنه && maptick == وروستی_maptick && i == idx)
	وروستی_maptick = -1;

    طالما (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_ثبت(&history[histype][i]);
    إذا (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    أرجع TRUE;
}

/*
 * "histadd()" function
 */
    فراغ
f_histadd(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    صحيح		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_شمېره = FALSE;
    إذا (check_secure())
	أرجع;

    إذا (دننه_vim9script()
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
    جمع_کړه_منزل_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_شمېره = TRUE;
}

/*
 * "histdel()" function
 */
    فراغ
f_histdel(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    صحيح		n;
    char_u	*str;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    إذا (str == عدم)
	n = 0;
    وإلا إذا (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    وإلا إذا (argvars[1].v_type == VAR_شمېره)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (صحيح)tv_get_شمېره(&argvars[1]));
    وإلا
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_ثبت(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_شمېره = n;
}

/*
 * "histget()" function
 */
    فراغ
f_histget(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    char_u  *str;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_شمېره_arg(argvars, 1) == FAIL))
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
	    idx = (صحيح)tv_get_شمېره_chk(&argvars[1], عدم);
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
f_histnr(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    صحيح		i;
    char_u	*histname;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == عدم ? HIST_CMD - 1 : get_histtype(histname);
    إذا (i >= HIST_CMD && i < HIST_شمېر)
	rettv->vval.v_شمېره = get_history_idx(i);
    وإلا
	rettv->vval.v_شمېره = -1;
}
#د_که_پای // FEAT_EVAL

#إذا defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    فراغ
remove_کلی_from_history(فراغ)
{
    char_u	*p_پیل;
    char_u	*p_پای_ټکی;
    char_u	*p;
    صحيح		i;

    i = hisidx[HIST_CMD];
    إذا (i < 0)
	أرجع;
    p_پیل = history[HIST_CMD][i].hisstr;
    إذا (p_پیل == عدم)
	أرجع;

    p_پای_ټکی = p_پیل + history[HIST_CMD][i].hisstrlen;
    لكل (p = p_پیل; *p; ++p)
    {
	إذا (STRNCMP(p, "key", 3) == 0 && !خوندي_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    إذا (p == عدم)
		اكسر;
	    ++p;
	    لكل (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		إذا (p[i] == '\\' && p[i + 1])
		    ++i;

	    mch_memmove(p, p + i, (p_پای_ټکی - (p + i)) + 1);	    // +1 for the NUL
	    p_پای_ټکی -= i;						    // adjust p_end for shortened string
	    --p;
	}
    }

    history[HIST_CMD][i].hisstrlen = (size_t)(p_پای_ټکی - p_پیل);
}
#د_که_پای

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
    char_u	*پای_ټکی;
    char_u	*arg = eap->arg;

    إذا (hislen == 0)
    {
	پیغام(_("'history' option is zero"));
	أرجع;
    }

    إذا (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	پای_ټکی = arg;
	طالما (ASCII_ISALPHA(*پای_ټکی)
		|| vim_strchr((char_u *)":=@>/?", *پای_ټکی) != عدم)
	    پای_ټکی++;
	i = *پای_ټکی;
	*پای_ټکی = NUL;
	histype1 = get_histtype(arg);
	إذا (histype1 == -1)
	{
	    إذا (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_شمېر-1;
	    }
	    وإلا
	    {
		*پای_ټکی = i;
		semsg(_(e_trailing_characters_str), arg);
		أرجع;
	    }
	}
	وإلا
	    histype2 = histype1;
	*پای_ټکی = i;
    }
    وإلا
	پای_ټکی = arg;
    إذا (!get_list_range(&پای_ټکی, &hisidx1, &hisidx2) || *پای_ټکی != NUL)
    {
	إذا (*پای_ټکی != NUL)
	    semsg(_(e_trailing_characters_str), پای_ټکی);
	وإلا
	    semsg(_(e_val_too_large), arg);
	أرجع;
    }

    لكل (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "\n      #  %s history", history_نومونه[histype1]);
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
			&& !پیغام_filtered(hist[i].hisstr))
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
		    بهر_flush();
		}
		إذا (i == idx)
		    اكسر;
	    }
    }
}
