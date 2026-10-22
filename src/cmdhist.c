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

#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର histentry_T *(history[HIST_ଗଣନା]) = {ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି};
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	hisidx[HIST_ଗଣନା] = {-1, -1, -1, -1, -1};  // lastused entry
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	hisnum[HIST_ଗଣନା] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_hislen(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hist_type)
{
    ଫେରନ୍ତୁ history[hist_type];
}

#ଯଦି defined(FEAT_VIMINFO)
    ଶୂନ୍ୟ
set_histentry(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
get_hisidx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hist_type)
{
    ଫେରନ୍ତୁ &hisidx[hist_type];
}

#ଯଦି defined(FEAT_VIMINFO)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
get_hisnum(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hist_type)
{
    ଫେରନ୍ତୁ &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hist_char2type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c == ':')
	ଫେରନ୍ତୁ HIST_CMD;
    ଯଦି (c == '=')
	ଫେରନ୍ତୁ HIST_ପଦସମଷ୍ଟି;
    ଯଦି (c == '@')
	ଫେରନ୍ତୁ HIST_ନିବେଶ;
    ଯଦି (c == '>')
	ଫେରନ୍ତୁ HIST_DEBUG;
    ଫେରନ୍ତୁ HIST_ସନ୍ଧାନ;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

ସ୍ଥିର ଅକ୍ଷର *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    ଖାଲି
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଅକ୍ଷର    *short_names = ":=@>?/";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    short_names_ଗଣନା = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(short_names);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    history_name_ଗଣନା = ARRAY_LENGTH(history_names) - 1;

    ଯଦି (idx < short_names_ଗଣନା)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ଫେରନ୍ତୁ xp->xp_buf;
    }
    ଯଦି (idx < short_names_ଗଣନା + history_name_ଗଣନା)
	ଫେରନ୍ତୁ (char_u *)history_names[idx - short_names_ଗଣନା];
    ଯଦି (idx == short_names_ଗଣନା + history_name_ଗଣନା)
	ଫେରନ୍ତୁ (char_u *)"all";
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    ଶୂନ୍ୟ
init_history(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newlen;	    // new length of history table
    histentry_T	*temp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;

    // If size of history table changed, reallocate it
    newlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_hi;
    ଯଦି (newlen == hislen)		// history length didn't change
	ଫେରନ୍ତୁ;

    // history length changed
    ପାଇଁ (type = 0; type < HIST_ଗଣନା; ++type)   // adjust the tables
    {
	ଯଦି (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    ଯଦି (temp == ଖାଲି)   // out of memory!
	    {
		ଯଦି (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    ଭାଙ୍ଗ;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		ଜାରି;
	    }
	}
	ଅନ୍ୟଥା
	    temp = ଖାଲି;

	ଯଦି (hisidx[type] < 0)		// there are no entries yet
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	    ପାଇଁ (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	ଅନ୍ୟଥା ଯଦି (newlen > hislen)	// array becomes bigger
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	j;

	    ପାଇଁ (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    ପାଇଁ ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    ପାଇଁ ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	ଅନ୍ୟଥା				// array becomes smaller or 0
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	j;

	    j = hisidx[type];
	    ପାଇଁ (i = newlen - 1; ; --i)
	    {
		ଯଦି (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		ଅନ୍ୟଥା			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ଯଦି (--j < 0)
		    j = hislen - 1;
		ଯଦି (j == hisidx[type])
		    ଭାଙ୍ଗ;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    ଶୂନ୍ୟ
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = ଖାଲି;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
in_history(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    type,
    char_u  *str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    move_to_front,	// Move the entry to the front if it exists
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    writing)		// ignore entries read from viminfo
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    last_i = -1;
    char_u  *p;
    size_t  len;

    ଯଦି (hisidx[type] < 0)
	ଫେରନ୍ତୁ FALSE;
    i = hisidx[type];
    କର
    {
	ଯଦି (history[type][i].hisstr == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ଯଦି (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_ସନ୍ଧାନ || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ଯଦି (!move_to_front)
		ଫେରନ୍ତୁ TRUE;
	    last_i = i;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (--i < 0)
	    i = hislen - 1;
    } ଯେତେବେଳେ (i != hisidx[type]);

    ଯଦି (last_i < 0)
	ଫେରନ୍ତୁ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ଯେତେବେଳେ (i != hisidx[type])
    {
	ଯଦି (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_histtype(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name);

    // No argument: use current history.
    ଯଦି (len == 0)
	ଫେରନ୍ତୁ hist_char2type(get_cmdline_firstc());

    ପାଇଁ (i = 0; history_names[i] != ଖାଲି; ++i)
	ଯଦି (STRNICMP(name, history_names[i], len) == 0)
	    ଫେରନ୍ତୁ i;

    ଯଦି (vim_strchr((char_u *)":=@>?/", name[0]) != ଖାଲି && name[1] == NUL)
	ଫେରନ୍ତୁ hist_char2type(name[0]);

    ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    ଶୂନ୍ୟ
add_to_history(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_map,		// consider maptick when inside a mapping
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ଯଦି (hislen == 0)		// no history
	ଫେରନ୍ତୁ;

    ଯଦି ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_ସନ୍ଧାନ)
	ଫେରନ୍ତୁ;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ଯଦି (histype == HIST_ସନ୍ଧାନ && in_map)
    {
	ଯଦି (maptick == last_maptick && hisidx[HIST_ସନ୍ଧାନ] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_ସନ୍ଧାନ][hisidx[HIST_ସନ୍ଧାନ]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    ଯଦି (--hisidx[HIST_ସନ୍ଧାନ] < 0)
		hisidx[HIST_ସନ୍ଧାନ] = hislen - 1;
	}
	last_maptick = -1;
    }

    ଯଦି (in_history(histype, new_entry, TRUE, sep, FALSE))
	ଫେରନ୍ତୁ;

    ଯଦି (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    ଯଦି (hisptr->hisstr == ଖାଲି)
	hisptr->hisstrlen = 0;
    ଅନ୍ୟଥା
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    ଯଦି (histype == HIST_ସନ୍ଧାନ && in_map)
	last_maptick = maptick;
}

#ଯଦି defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_history_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା histype)
{
    ଯଦି (hislen == 0 || histype < 0 || histype >= HIST_ଗଣନା
		    || hisidx[histype] < 0)
	ଫେରନ୍ତୁ -1;

    ଫେରନ୍ତୁ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
calc_hist_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା histype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    histentry_T	*hist;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wrapped = FALSE;

    ଯଦି (hislen == 0 || histype < 0 || histype >= HIST_ଗଣନା
		    || (i = hisidx[histype]) < 0 || ଅଙ୍କ == 0)
	ଫେରନ୍ତୁ -1;

    hist = history[histype];
    ଯଦି (ଅଙ୍କ > 0)
    {
	ଯେତେବେଳେ (hist[i].hisnum > ଅଙ୍କ)
	    ଯଦି (--i < 0)
	    {
		ଯଦି (wrapped)
		    ଭାଙ୍ଗ;
		i += hislen;
		wrapped = TRUE;
	    }
	ଯଦି (i >= 0 && hist[i].hisnum == ଅଙ୍କ && hist[i].hisstr != ଖାଲି)
	    ଫେରନ୍ତୁ i;
    }
    ଅନ୍ୟଥା ଯଦି (-ଅଙ୍କ <= hislen)
    {
	i += ଅଙ୍କ + 1;
	ଯଦି (i < 0)
	    i += hislen;
	ଯଦି (hist[i].hisstr != ଖାଲି)
	    ଫେରନ୍ତୁ i;
    }
    ଫେରନ୍ତୁ -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clr_history(ପୂର୍ଣ୍ଣସଂଖ୍ୟା histype)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    histentry_T	*hisptr;

    ଯଦି (hislen != 0 && histype >= 0 && histype < HIST_ଗଣନା)
    {
	hisptr = history[histype];
	ପାଇଁ (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ଫେରନ୍ତୁ OK;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
del_history_entry(ପୂର୍ଣ୍ଣସଂଖ୍ୟା histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found = FALSE;

    ଯଦି (hislen == 0 || histype < 0 || histype >= HIST_ଗଣନା || *str == NUL
		|| hisidx[histype] < 0)
	ଫେରନ୍ତୁ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ଯଦି (regmatch.regprog == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    କର
    {
	hisptr = &history[histype][i];
	ଯଦି (hisptr->hisstr == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    ଯଦି (--last < 0)
		last += hislen;
	}
	ଯଦି (--i < 0)
	    i += hislen;
    } ଯେତେବେଳେ (i != idx);

    ଯଦି (history[histype][idx].hisstr == ଖାଲି)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ଫେରନ୍ତୁ found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
del_history_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା histype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i, j;

    i = calc_hist_idx(histype, idx);
    ଯଦି (i < 0)
	ଫେରନ୍ତୁ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ଯଦି (histype == HIST_ସନ୍ଧାନ && maptick == last_maptick && i == idx)
	last_maptick = -1;

    ଯେତେବେଳେ (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    ଯଦି (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * "histadd()" function
 */
    ଶୂନ୍ୟ
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_ସଂଖ୍ୟା = FALSE;
    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != ଖାଲି ? get_histtype(str) : -1;
    ଯଦି (histype < 0)
	ଫେରନ୍ତୁ;

    str = tv_get_string_buf(&argvars[1], buf);
    ଯଦି (*str == NUL)
	ଫେରନ୍ତୁ;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_ସଂଖ୍ୟା = TRUE;
}

/*
 * "histdel()" function
 */
    ଶୂନ୍ୟ
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    char_u	*str;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ଯଦି (str == ଖାଲି)
	n = 0;
    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type == VAR_ସଂଖ୍ୟା)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[1]));
    ଅନ୍ୟଥା
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_ସଂଖ୍ୟା = n;
}

/*
 * "histget()" function
 */
    ଶୂନ୍ୟ
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ଯଦି (str == ଖାଲି)
	rettv->vval.v_string = ଖାଲି;
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

	type = get_histtype(str);
	ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	ଅନ୍ୟଥା
	    idx = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ଯଦି (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	ଅନ୍ୟଥା
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    ଶୂନ୍ୟ
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*histname;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == ଖାଲି ? HIST_CMD - 1 : get_histtype(histname);
    ଯଦି (i >= HIST_CMD && i < HIST_ଗଣନା)
	rettv->vval.v_ସଂଖ୍ୟା = get_history_idx(i);
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = -1;
}
#endif // FEAT_EVAL

#ଯଦି defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    ଶୂନ୍ୟ
remove_ଚାବି_from_history(ଶୂନ୍ୟ)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    i = hisidx[HIST_CMD];
    ଯଦି (i < 0)
	ଫେରନ୍ତୁ;
    p_start = history[HIST_CMD][i].hisstr;
    ଯଦି (p_start == ଖାଲି)
	ଫେରନ୍ତୁ;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    ପାଇଁ (p = p_start; *p; ++p)
    {
	ଯଦି (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ଯଦି (p == ଖାଲି)
		ଭାଙ୍ଗ;
	    ++p;
	    ପାଇଁ (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ଯଦି (p[i] == '\\' && p[i + 1])
		    ++i;

	    mch_memmove(p, p + i, (p_end - (p + i)) + 1);	    // +1 for the NUL
	    p_end -= i;						    // adjust p_end for shortened string
	    --p;
	}
    }

    history[HIST_CMD][i].hisstrlen = (size_t)(p_end - p_start);
}
#endif

/*
 * :history command - print a history
 */
    ଶୂନ୍ୟ
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		histype1 = HIST_CMD;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		histype2 = HIST_CMD;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hisidx1 = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hisidx2 = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    ଯଦି (hislen == 0)
    {
	ସନ୍ଦେଶ(_("'history' option is zero"));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	ଯେତେବେଳେ (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != ଖାଲି)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	ଯଦି (histype1 == -1)
	{
	    ଯଦି (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_ଗଣନା-1;
	    }
	    ଅନ୍ୟଥା
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ଫେରନ୍ତୁ;
	    }
	}
	ଅନ୍ୟଥା
	    histype2 = histype1;
	*end = i;
    }
    ଅନ୍ୟଥା
	end = arg;
    ଯଦି (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	ଯଦି (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	ଅନ୍ୟଥା
	    semsg(_(e_val_too_large), arg);
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	ସନ୍ଦେଶ_puts_title((ଅକ୍ଷର *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ଯଦି (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ଯଦି (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ଯଦି (idx >= 0 && j <= k)
	    ପାଇଁ (i = idx + 1; !got_int; ++i)
	    {
		ଯଦି (i == hislen)
		    i = 0;
		ଯଦି (hist[i].hisstr != ଖାଲି
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  len;

		    ସନ୍ଦେଶ_putchar('\n');
		    len = vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ଯଦି (vim_strsize(hist[i].hisstr) > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns - 10, IOSIZE - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);
		    ଅନ୍ୟଥା
			STRCPY(IObuff + len, hist[i].hisstr);
		    ସନ୍ଦେଶ_outtrans(IObuff);
		    out_flush();
		}
		ଯଦି (i == idx)
		    ଭାଙ୍ଗ;
	    }
    }
}
