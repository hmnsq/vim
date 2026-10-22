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

#అన్తర్భావ "vim.h"

స్థిర histentry_T *(history[HIST_లెక్క]) = {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};
స్థిర పూర్ణాంకం	hisidx[HIST_లెక్క] = {-1, -1, -1, -1, -1};  // lastused entry
స్థిర పూర్ణాంకం	hisnum[HIST_లెక్క] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
స్థిర పూర్ణాంకం	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    పూర్ణాంకం
get_hislen(శూన్య)
{
    తిరిగి hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(పూర్ణాంకం hist_type)
{
    తిరిగి history[hist_type];
}

#ఒకవేళ defined(FEAT_VIMINFO)
    శూన్య
సమితి_histentry(పూర్ణాంకం hist_type, histentry_T *నమోదు_2)
{
    history[hist_type] = నమోదు_2;
}
#అయితే_ముగింపు

    పూర్ణాంకం *
get_hisidx(పూర్ణాంకం hist_type)
{
    తిరిగి &hisidx[hist_type];
}

#ఒకవేళ defined(FEAT_VIMINFO)
    పూర్ణాంకం *
get_hisnum(పూర్ణాంకం hist_type)
{
    తిరిగి &hisnum[hist_type];
}
#అయితే_ముగింపు

/*
 * Translate a history character to the associated type number.
 */
    పూర్ణాంకం
hist_char2type(పూర్ణాంకం c)
{
    ఒకవేళ (c == ':')
	తిరిగి HIST_CMD;
    ఒకవేళ (c == '=')
	తిరిగి HIST_సమాసం;
    ఒకవేళ (c == '@')
	తిరిగి HIST_ఆదానం;
    ఒకవేళ (c == '>')
	తిరిగి HIST_DEBUG;
    తిరిగి HIST_శోధన;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

స్థిర అక్షరం *(history_పేర్లు[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    ఖాళీ
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    అక్షరం    *short_పేర్లు = ":=@>?/";
    పూర్ణాంకం	    short_పేర్లు_లెక్క = (పూర్ణాంకం)STRLEN(short_పేర్లు);
    పూర్ణాంకం	    history_name_లెక్క = ARRAY_నిడివి(history_పేర్లు) - 1;

    ఒకవేళ (idx < short_పేర్లు_లెక్క)
    {
	xp->xp_buf[0] = (char_u)short_పేర్లు[idx];
	xp->xp_buf[1] = NUL;
	తిరిగి xp->xp_buf;
    }
    ఒకవేళ (idx < short_పేర్లు_లెక్క + history_name_లెక్క)
	తిరిగి (char_u *)history_పేర్లు[idx - short_పేర్లు_లెక్క];
    ఒకవేళ (idx == short_పేర్లు_లెక్క + history_name_లెక్క)
	తిరిగి (char_u *)"all";
    తిరిగి ఖాళీ;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    శూన్య
init_history(శూన్య)
{
    పూర్ణాంకం		newlen;	    // new length of history table
    histentry_T	*తాత్కాలిక;
    పూర్ణాంకం		type;

    // If size of history table changed, reallocate it
    newlen = (పూర్ణాంకం)p_hi;
    ఒకవేళ (newlen == hislen)		// history length didn't change
	తిరిగి;

    // history length changed
    కొరకు (type = 0; type < HIST_లెక్క; ++type)   // adjust the tables
    {
	ఒకవేళ (newlen > 0)
	{
	    తాత్కాలిక = ALLOC_MULT(histentry_T, newlen);
	    ఒకవేళ (తాత్కాలిక == ఖాళీ)   // out of memory!
	    {
		ఒకవేళ (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    ఆపు;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		కొనసాగు;
	    }
	}
	లేకపోతే
	    తాత్కాలిక = ఖాళీ;

	ఒకవేళ (hisidx[type] < 0)		// there are no entries yet
	{
	    పూర్ణాంకం	i;

	    కొరకు (i = 0; i < newlen; ++i)
		clear_hist_నమోదు(&తాత్కాలిక[i]);
	}
	లేకపోతే ఒకవేళ (newlen > hislen)	// array becomes bigger
	{
	    పూర్ణాంకం	i;
	    పూర్ణాంకం	j;

	    కొరకు (i = 0; i <= hisidx[type]; ++i)
		తాత్కాలిక[i] = history[type][i];
	    j = i;
	    కొరకు ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_నమోదు(&తాత్కాలిక[i]);
	    కొరకు ( ; j < hislen; ++i, ++j)
		తాత్కాలిక[i] = history[type][j];
	}
	లేకపోతే				// array becomes smaller or 0
	{
	    పూర్ణాంకం	i;
	    పూర్ణాంకం	j;

	    j = hisidx[type];
	    కొరకు (i = newlen - 1; ; --i)
	    {
		ఒకవేళ (i >= 0)		// copy newest entries
		    తాత్కాలిక[i] = history[type][j];
		లేకపోతే			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ఒకవేళ (--j < 0)
		    j = hislen - 1;
		ఒకవేళ (j == hisidx[type])
		    ఆపు;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = తాత్కాలిక;
    }
    hislen = newlen;
}

    శూన్య
clear_hist_నమోదు(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = ఖాళీ;
    hisptr->hisstrlen = 0;
    hisptr->సమయం_సమితి = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    పూర్ణాంకం
లోపల_history(
    పూర్ణాంకం	    type,
    char_u  *str,
    పూర్ణాంకం	    move_గమ్యం_front,	// Move the entry to the front if it exists
    పూర్ణాంకం	    sep,
    పూర్ణాంకం	    writing)		// ignore entries read from viminfo
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    చివరి_i = -1;
    char_u  *p;
    size_t  len;

    ఒకవేళ (hisidx[type] < 0)
	తిరిగి FALSE;
    i = hisidx[type];
    చేయి
    {
	ఒకవేళ (history[type][i].hisstr == ఖాళీ)
	    తిరిగి FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ఒకవేళ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_శోధన || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ఒకవేళ (!move_గమ్యం_front)
		తిరిగి TRUE;
	    చివరి_i = i;
	    ఆపు;
	}
	ఒకవేళ (--i < 0)
	    i = hislen - 1;
    } అయితే (i != hisidx[type]);

    ఒకవేళ (చివరి_i < 0)
	తిరిగి FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    అయితే (i != hisidx[type])
    {
	ఒకవేళ (++i >= hislen)
	    i = 0;
	history[type][చివరి_i] = history[type][i];
	చివరి_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].సమయం_సమితి = vim_సమయం();
    తిరిగి TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    స్థిర పూర్ణాంకం
get_histtype(char_u *name)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		len = (పూర్ణాంకం)STRLEN(name);

    // No argument: use current history.
    ఒకవేళ (len == 0)
	తిరిగి hist_char2type(get_cmdline_firstc());

    కొరకు (i = 0; history_పేర్లు[i] != ఖాళీ; ++i)
	ఒకవేళ (STRNICMP(name, history_పేర్లు[i], len) == 0)
	    తిరిగి i;

    ఒకవేళ (vim_strchr((char_u *)":=@>?/", name[0]) != ఖాళీ && name[1] == NUL)
	తిరిగి hist_char2type(name[0]);

    తిరిగి -1;
}

స్థిర పూర్ణాంకం	చివరి_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    శూన్య
కూడు_గమ్యం_history(
    పూర్ణాంకం		histype,
    char_u	*new_నమోదు,
    size_t	new_entrylen,
    పూర్ణాంకం		లోపల_map,		// consider maptick when inside a mapping
    పూర్ణాంకం		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ఒకవేళ (hislen == 0)		// no history
	తిరిగి;

    ఒకవేళ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_శోధన)
	తిరిగి;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ఒకవేళ (histype == HIST_శోధన && లోపల_map)
    {
	ఒకవేళ (maptick == చివరి_maptick && hisidx[HIST_శోధన] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_శోధన][hisidx[HIST_శోధన]];
	    vim_free(hisptr->hisstr);
	    clear_hist_నమోదు(hisptr);
	    --hisnum[histype];
	    ఒకవేళ (--hisidx[HIST_శోధన] < 0)
		hisidx[HIST_శోధన] = hislen - 1;
	}
	చివరి_maptick = -1;
    }

    ఒకవేళ (లోపల_history(histype, new_నమోదు, TRUE, sep, FALSE))
	తిరిగి;

    ఒకవేళ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_నమోదు, new_entrylen + 2);
    ఒకవేళ (hisptr->hisstr == ఖాళీ)
	hisptr->hisstrlen = 0;
    లేకపోతే
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->సమయం_సమితి = vim_సమయం();
    ఒకవేళ (histype == HIST_శోధన && లోపల_map)
	చివరి_maptick = maptick;
}

#ఒకవేళ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    స్థిర పూర్ణాంకం
get_history_idx(పూర్ణాంకం histype)
{
    ఒకవేళ (hislen == 0 || histype < 0 || histype >= HIST_లెక్క
		    || hisidx[histype] < 0)
	తిరిగి -1;

    తిరిగి history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    స్థిర పూర్ణాంకం
calc_hist_idx(పూర్ణాంకం histype, పూర్ణాంకం అంకె)
{
    పూర్ణాంకం		i;
    histentry_T	*hist;
    పూర్ణాంకం		wrapped = FALSE;

    ఒకవేళ (hislen == 0 || histype < 0 || histype >= HIST_లెక్క
		    || (i = hisidx[histype]) < 0 || అంకె == 0)
	తిరిగి -1;

    hist = history[histype];
    ఒకవేళ (అంకె > 0)
    {
	అయితే (hist[i].hisnum > అంకె)
	    ఒకవేళ (--i < 0)
	    {
		ఒకవేళ (wrapped)
		    ఆపు;
		i += hislen;
		wrapped = TRUE;
	    }
	ఒకవేళ (i >= 0 && hist[i].hisnum == అంకె && hist[i].hisstr != ఖాళీ)
	    తిరిగి i;
    }
    లేకపోతే ఒకవేళ (-అంకె <= hislen)
    {
	i += అంకె + 1;
	ఒకవేళ (i < 0)
	    i += hislen;
	ఒకవేళ (hist[i].hisstr != ఖాళీ)
	    తిరిగి i;
    }
    తిరిగి -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    స్థిర పూర్ణాంకం
clr_history(పూర్ణాంకం histype)
{
    పూర్ణాంకం		i;
    histentry_T	*hisptr;

    ఒకవేళ (hislen != 0 && histype >= 0 && histype < HIST_లెక్క)
    {
	hisptr = history[histype];
	కొరకు (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_నమోదు(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	తిరిగి OK;
    }
    తిరిగి FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    స్థిర పూర్ణాంకం
del_history_నమోదు(పూర్ణాంకం histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    పూర్ణాంకం		idx;
    పూర్ణాంకం		i;
    పూర్ణాంకం		చివరి;
    పూర్ణాంకం		దొరికింది = FALSE;

    ఒకవేళ (hislen == 0 || histype < 0 || histype >= HIST_లెక్క || *str == NUL
		|| hisidx[histype] < 0)
	తిరిగి FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ఒకవేళ (regmatch.regprog == ఖాళీ)
	తిరిగి FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = చివరి = idx;
    చేయి
    {
	hisptr = &history[histype][i];
	ఒకవేళ (hisptr->hisstr == ఖాళీ)
	    ఆపు;
	ఒకవేళ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    దొరికింది = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_నమోదు(hisptr);
	}
	లేకపోతే
	{
	    ఒకవేళ (i != చివరి)
	    {
		history[histype][చివరి] = *hisptr;
		clear_hist_నమోదు(hisptr);
	    }
	    ఒకవేళ (--చివరి < 0)
		చివరి += hislen;
	}
	ఒకవేళ (--i < 0)
	    i += hislen;
    } అయితే (i != idx);

    ఒకవేళ (history[histype][idx].hisstr == ఖాళీ)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    తిరిగి దొరికింది;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    స్థిర పూర్ణాంకం
del_history_idx(పూర్ణాంకం histype, పూర్ణాంకం idx)
{
    పూర్ణాంకం	    i, j;

    i = calc_hist_idx(histype, idx);
    ఒకవేళ (i < 0)
	తిరిగి FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ఒకవేళ (histype == HIST_శోధన && maptick == చివరి_maptick && i == idx)
	చివరి_maptick = -1;

    అయితే (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_నమోదు(&history[histype][i]);
    ఒకవేళ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    తిరిగి TRUE;
}

/*
 * "histadd()" function
 */
    శూన్య
f_histadd(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_సంఖ్య = FALSE;
    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != ఖాళీ ? get_histtype(str) : -1;
    ఒకవేళ (histype < 0)
	తిరిగి;

    str = tv_get_string_buf(&argvars[1], buf);
    ఒకవేళ (*str == NUL)
	తిరిగి;

    init_history();
    కూడు_గమ్యం_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_సంఖ్య = TRUE;
}

/*
 * "histdel()" function
 */
    శూన్య
f_histdel(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    పూర్ణాంకం		n;
    char_u	*str;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ఒకవేళ (str == ఖాళీ)
	n = 0;
    లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_సంఖ్య)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[1]));
    లేకపోతే
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_నమోదు(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_సంఖ్య = n;
}

/*
 * "histget()" function
 */
    శూన్య
f_histget(typval_T *argvars వాడని, typval_T *rettv)
{
    char_u  *str;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ఒకవేళ (str == ఖాళీ)
	rettv->vval.v_string = ఖాళీ;
    లేకపోతే
    {
	పూర్ణాంకం type;
	పూర్ణాంకం idx;

	type = get_histtype(str);
	ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	లేకపోతే
	    idx = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ఒకవేళ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	లేకపోతే
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    శూన్య
f_histnr(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం		i;
    char_u	*histname;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == ఖాళీ ? HIST_CMD - 1 : get_histtype(histname);
    ఒకవేళ (i >= HIST_CMD && i < HIST_లెక్క)
	rettv->vval.v_సంఖ్య = get_history_idx(i);
    లేకపోతే
	rettv->vval.v_సంఖ్య = -1;
}
#అయితే_ముగింపు // FEAT_EVAL

#ఒకవేళ defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    శూన్య
remove_కీలకం_from_history(శూన్య)
{
    char_u	*p_ప్రారంభం;
    char_u	*p_చివర;
    char_u	*p;
    పూర్ణాంకం		i;

    i = hisidx[HIST_CMD];
    ఒకవేళ (i < 0)
	తిరిగి;
    p_ప్రారంభం = history[HIST_CMD][i].hisstr;
    ఒకవేళ (p_ప్రారంభం == ఖాళీ)
	తిరిగి;

    p_చివర = p_ప్రారంభం + history[HIST_CMD][i].hisstrlen;
    కొరకు (p = p_ప్రారంభం; *p; ++p)
    {
	ఒకవేళ (STRNCMP(p, "key", 3) == 0 && !సురక్షిత_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ఒకవేళ (p == ఖాళీ)
		ఆపు;
	    ++p;
	    కొరకు (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ఒకవేళ (p[i] == '\\' && p[i + 1])
		    ++i;

	    mch_memmove(p, p + i, (p_చివర - (p + i)) + 1);	    // +1 for the NUL
	    p_చివర -= i;						    // adjust p_end for shortened string
	    --p;
	}
    }

    history[HIST_CMD][i].hisstrlen = (size_t)(p_చివర - p_ప్రారంభం);
}
#అయితే_ముగింపు

/*
 * :history command - print a history
 */
    శూన్య
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    పూర్ణాంకం		histype1 = HIST_CMD;
    పూర్ణాంకం		histype2 = HIST_CMD;
    పూర్ణాంకం		hisidx1 = 1;
    పూర్ణాంకం		hisidx2 = -1;
    పూర్ణాంకం		idx;
    పూర్ణాంకం		i, j, k;
    char_u	*చివర;
    char_u	*arg = eap->arg;

    ఒకవేళ (hislen == 0)
    {
	సందేశం(_("'history' option is zero"));
	తిరిగి;
    }

    ఒకవేళ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	చివర = arg;
	అయితే (ASCII_ISALPHA(*చివర)
		|| vim_strchr((char_u *)":=@>/?", *చివర) != ఖాళీ)
	    చివర++;
	i = *చివర;
	*చివర = NUL;
	histype1 = get_histtype(arg);
	ఒకవేళ (histype1 == -1)
	{
	    ఒకవేళ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_లెక్క-1;
	    }
	    లేకపోతే
	    {
		*చివర = i;
		semsg(_(e_trailing_characters_str), arg);
		తిరిగి;
	    }
	}
	లేకపోతే
	    histype2 = histype1;
	*చివర = i;
    }
    లేకపోతే
	చివర = arg;
    ఒకవేళ (!get_list_range(&చివర, &hisidx1, &hisidx2) || *చివర != NUL)
    {
	ఒకవేళ (*చివర != NUL)
	    semsg(_(e_trailing_characters_str), చివర);
	లేకపోతే
	    semsg(_(e_val_too_large), arg);
	తిరిగి;
    }

    కొరకు (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, "\n      #  %s history", history_పేర్లు[histype1]);
	సందేశం_puts_title((అక్షరం *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ఒకవేళ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ఒకవేళ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ఒకవేళ (idx >= 0 && j <= k)
	    కొరకు (i = idx + 1; !got_int; ++i)
	    {
		ఒకవేళ (i == hislen)
		    i = 0;
		ఒకవేళ (hist[i].hisstr != ఖాళీ
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !సందేశం_filtered(hist[i].hisstr))
		{
		    పూర్ణాంకం  len;

		    సందేశం_putchar('\n');
		    len = vim_snprintf((అక్షరం *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ఒకవేళ (vim_strsize(hist[i].hisstr) > (పూర్ణాంకం)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (పూర్ణాంకం)Columns - 10, IOSIZE - (పూర్ణాంకం)len);
		    లేకపోతే
			STRCPY(IObuff + len, hist[i].hisstr);
		    సందేశం_outtrans(IObuff);
		    బహిర్గతం_flush();
		}
		ఒకవేళ (i == idx)
		    ఆపు;
	    }
    }
}
