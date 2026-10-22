/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * cindent.c: C indentation related functions
 *
 * Many of C-indenting functions originally come from Eric Fischer.
 *
 * Below "XXX" means that this function may unlock the current line.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

// values for the "lookfor" state
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_INITIAL		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_IF		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_DO		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_CASE		3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_ANY		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_ਪਦ		5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_UNTERM		6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_SCOPEDECL	7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_NOBREAK		8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_CPP_BASECLASS	9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_ENUM_OR_INIT	10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_JS_ਕੁੰਜੀ		11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    ਪੂਰਨ
cin_is_cinword(char_u *ਸਤਰ)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    ਪੂਰਨ		cinw_len;
    ਪੂਰਨ		retval = FALSE;
    ਪੂਰਨ		len;

    cinw_len = (ਪੂਰਨ)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    ਜੇ (cinw_buf == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਸਤਰ = skipwhite(ਸਤਰ);
    ਲਈ (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	ਜੇ (STRNCMP(ਸਤਰ, cinw_buf, len) == 0
		&& (!vim_iswordc(ਸਤਰ[len]) || !vim_iswordc(ਸਤਰ[len - 1])))
	{
	    retval = TRUE;
	    ਰੋਕੋ;
	}
    }
    vim_free(cinw_buf);
    ਵਾਪਸ retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    ਸਥਿਰ char_u *
skip_string(char_u *p)
{
    ਪੂਰਨ	    i;

    // We loop, because strings may be concatenated: "date""time".
    ਲਈ ( ; ; ++p)
    {
	ਜੇ (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    ਜੇ (p[1] == NUL)		    // ' at end of line
		ਰੋਕੋ;
	    i = 2;
	    ਜੇ (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		ਜਦੋਂ (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    ਜੇ (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		ਜਾਰੀ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '"')		    // start of string
	{
	    ਲਈ (++p; p[0]; ++p)
	    {
		ਜੇ (p[0] == '\\' && p[1] != NUL)
		    ++p;
		ਨਹੀਂਤਾਂ ਜੇ (p[0] == '"')	    // end of string
		    ਰੋਕੋ;
	    }
	    ਜੇ (p[0] == '"')
		ਜਾਰੀ; // continue for another string
	}
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    ਜੇ (paren != ਨਿਰਾ)
	    {
		size_t delim_len = paren - delim;

		ਲਈ (p += 3; *p; ++p)
		    ਜੇ (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			ਰੋਕੋ;
		    }
		ਜੇ (p[0] == '"')
		    ਜਾਰੀ; // continue for another string
	    }
	}
	ਰੋਕੋ;				    // no string found
    }
    ਜੇ (!*p)
	--p;				    // backup from NUL
    ਵਾਪਸ p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    ਪੂਰਨ
is_pos_ਵਿੱਚ_string(char_u *ਸਤਰ, colnr_T ਖਾਨਾ)
{
    char_u *p;

    ਲਈ (p = ਸਤਰ; *p && (colnr_T)(p - ਸਤਰ) < ਖਾਨਾ; ++p)
	p = skip_string(p);
    ਵਾਪਸ !((colnr_T)(p - ਸਤਰ) <= ਖਾਨਾ);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    ਪੂਰਨ
check_linecomment(char_u *ਸਤਰ)
{
    char_u  *p;

    p = ਸਤਰ;
    // skip Lispish one-line comments
    ਜੇ (curbuf->b_p_lisp)
    {
	ਜੇ (vim_strchr(p, ';') != ਨਿਰਾ) // there may be comments
	{
	    ਪੂਰਨ ਵਿੱਚ_str = FALSE;	// inside of string

	    p = ਸਤਰ;		// scan from start
	    ਜਦੋਂ ((p = vim_strpbrk(p, (char_u *)"\";")) != ਨਿਰਾ)
	    {
		ਜੇ (*p == '"')
		{
		    ਜੇ (ਵਿੱਚ_str)
		    {
			ਜੇ (*(p - 1) != '\\') // skip escaped quote
			    ਵਿੱਚ_str = FALSE;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (p == ਸਤਰ || ((p - ਸਤਰ) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			ਵਿੱਚ_str = TRUE;
		}
		ਨਹੀਂਤਾਂ ਜੇ (!ਵਿੱਚ_str && ((p - ਸਤਰ) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_ਵਿੱਚ_string(ਸਤਰ, (colnr_T)(p - ਸਤਰ)))
		    ਰੋਕੋ;	// found!
		++p;
	    }
	}
	ਨਹੀਂਤਾਂ
	    p = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	ਲਈ ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    ਜੇ (p[0] == '/' && p[1] == '/'
				 && (p == ਸਤਰ || p[-1] != '*' || p[2] != '*'))
		ਰੋਕੋ;
	}
	ਜੇ (*p == NUL)
	    p = ਨਿਰਾ;
    }

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ MAXCOL;
    ਵਾਪਸ (ਪੂਰਨ)(p - ਸਤਰ);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    ਸਥਿਰ pos_T *
ind_ਲੱਭੋ_ਸ਼ੁਰੂ_comment(ਖਾਲੀ)	    // XXX
{
    ਵਾਪਸ ਲੱਭੋ_ਸ਼ੁਰੂ_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
ਲੱਭੋ_ਸ਼ੁਰੂ_comment(ਪੂਰਨ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ਪੂਰਨ		cur_maxcomment = ind_maxcomment;

    ਲਈ (;;)
    {
	pos = findmatchlimit(ਨਿਰਾ, '*', FM_BACKWARD, cur_maxcomment);
	ਜੇ (pos == ਨਿਰਾ)
	    ਰੋਕੋ;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ਜੇ (!is_pos_ਵਿੱਚ_string(ml_get(pos->lnum), pos->ਖਾਨਾ))
	    ਰੋਕੋ;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ਜੇ (cur_maxcomment <= 0)
	{
	    pos = ਨਿਰਾ;
	    ਰੋਕੋ;
	}
    }
    ਵਾਪਸ pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    ਸਥਿਰ pos_T *
ਲੱਭੋ_ਸ਼ੁਰੂ_rawstring(ਪੂਰਨ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ਪੂਰਨ		cur_maxcomment = ind_maxcomment;

    ਲਈ (;;)
    {
	pos = findmatchlimit(ਨਿਰਾ, 'R', FM_BACKWARD, cur_maxcomment);
	ਜੇ (pos == ਨਿਰਾ)
	    ਰੋਕੋ;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ਜੇ (!is_pos_ਵਿੱਚ_string(ml_get(pos->lnum), pos->ਖਾਨਾ))
	    ਰੋਕੋ;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ਜੇ (cur_maxcomment <= 0)
	{
	    pos = ਨਿਰਾ;
	    ਰੋਕੋ;
	}
    }
    ਵਾਪਸ pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    ਸਥਿਰ pos_T *
ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(linenr_T *is_raw)	    // XXX
{
    ਸਥਿਰ pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = ਲੱਭੋ_ਸ਼ੁਰੂ_comment(curbuf->b_ind_maxcomment);
    ਜੇ (comment_pos != ਨਿਰਾ)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = ਲੱਭੋ_ਸ਼ੁਰੂ_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    ਜੇ (comment_pos == ਨਿਰਾ || (rs_pos != ਨਿਰਾ
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	ਜੇ (is_raw != ਨਿਰਾ && rs_pos != ਨਿਰਾ)
	    *is_raw = rs_pos->lnum;
	ਵਾਪਸ rs_pos;
    }
    ਵਾਪਸ comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    ਪੂਰਨ
cindent_on(ਖਾਲੀ)
{
    ਵਾਪਸ (!p_paste && (curbuf->b_p_cin
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#ਜੇ_ਅੰਤ
		    ));
}

// Find result cache for cpp_baseclass
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ	    ਮਿਲਿਆ;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    ਸਥਿਰ char_u *
cin_skipcomment(char_u *s)
{
    ਜਦੋਂ (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	ਜੇ (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    ਰੋਕੋ;
	}
	ਜੇ (*s != '/')
	    ਰੋਕੋ;
	++s;
	ਜੇ (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    ਰੋਕੋ;
	}
	ਜੇ (*s != '*')
	    ਰੋਕੋ;
	ਲਈ (++s; *s; ++s)	// skip slash-star comment
	    ਜੇ (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		ਰੋਕੋ;
	    }
    }
    ਵਾਪਸ s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_nocode(char_u *s)
{
    ਵਾਪਸ *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_iscomment(char_u *p)
{
    ਵਾਪਸ (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_islinecomment(char_u *p)
{
    ਵਾਪਸ (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    ਸਥਿਰ pos_T *
ਲੱਭੋ_ਸਤਰ_comment(ਖਾਲੀ) // XXX
{
    ਸਥਿਰ pos_T pos;
    char_u	 *ਸਤਰ;
    char_u	 *p;

    pos = curwin->w_cursor;
    ਜਦੋਂ (--pos.lnum > 0)
    {
	ਸਤਰ = ml_get(pos.lnum);
	p = skipwhite(ਸਤਰ);
	ਜੇ (cin_islinecomment(p))
	{
	    pos.ਖਾਨਾ = (ਪੂਰਨ)(p - ਸਤਰ);
	    ਵਾਪਸ &pos;
	}
	ਜੇ (*p != NUL)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    ਸਥਿਰ ਪੂਰਨ
cin_has_js_ਕੁੰਜੀ(char_u *ਪਾਠ)
{
    char_u *s = skipwhite(ਪਾਠ);
    ਪੂਰਨ	    quote = -1;

    ਜੇ (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    ਜੇ (!vim_isIDc(*s))	    // need at least one ID character
	ਵਾਪਸ FALSE;

    ਜਦੋਂ (vim_isIDc(*s))
	++s;
    ਜੇ (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    ਵਾਪਸ (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_islabel_skip(char_u **s)
{
    ਜੇ (!vim_isIDc(**s))	    // need at least one ID character
	ਵਾਪਸ FALSE;

    ਜਦੋਂ (vim_isIDc(**s))
    {
	ਜੇ (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	ਨਹੀਂਤਾਂ
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    ਵਾਪਸ (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    ਪੂਰਨ	    ਮਿਲਿਆ = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    ਜੇ (cinsd_buf == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਲਈ (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (ਪੂਰਨ)cinsd_len, ",");
	ਜੇ (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    ਜੇ (*skip == ':' && skip[1] != ':')
	    {
		ਮਿਲਿਆ = TRUE;
		ਰੋਕੋ;
	    }
	}
    }

    vim_free(cinsd_buf);
    ਵਾਪਸ ਮਿਲਿਆ;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_ispreproc(char_u *s)
{
    ਜੇ (*skipwhite(s) == '#')
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    ਸਥਿਰ ਪੂਰਨ
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, ਪੂਰਨ *amount)
{
    char_u	*ਸਤਰ = *pp;
    linenr_T	lnum = *lnump;
    ਪੂਰਨ		retval = FALSE;
    ਪੂਰਨ		candidate_amount = *amount;

    ਜੇ (*ਸਤਰ != NUL && ਸਤਰ[STRLEN(ਸਤਰ) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    ਲਈ (;;)
    {
	ਜੇ (cin_ispreproc(ਸਤਰ))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    ਰੋਕੋ;
	}
	ਜੇ (lnum == 1)
	    ਰੋਕੋ;
	ਸਤਰ = ml_get(--lnum);
	ਜੇ (*ਸਤਰ == NUL || ਸਤਰ[STRLEN(ਸਤਰ) - 1] != '\\')
	    ਰੋਕੋ;
    }

    ਜੇ (lnum != *lnump)
	*pp = ml_get(*lnump);
    ਜੇ (retval)
	*amount = candidate_amount;
    ਵਾਪਸ retval;
}

    ਸਥਿਰ ਪੂਰਨ
cin_iselse(
    char_u  *p)
{
    ਜੇ (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    ਵਾਪਸ (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
}

/*
 * Recognize a line that starts with '{' or '}', or ends with ';', ',', '{' or
 * '}'.
 * Don't consider "} else" a terminated line.
 * If a line begins with an "else", only consider it terminated if no unmatched
 * opening braces follow (handle "else { foo();" correctly).
 * Return the character terminating the line (ending char's have precedence if
 * both apply in order to determine initializations).
 */
    ਸਥਿਰ ਪੂਰਨ
cin_isterminated(
    char_u	*s,
    ਪੂਰਨ		incl_open,	// include '{' at the end as terminator
    ਪੂਰਨ		incl_comma)	// recognize a trailing comma
{
    char_u	ਮਿਲਿਆ_ਸ਼ੁਰੂ = 0;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	n_open = 0;
    ਪੂਰਨ		is_else = FALSE;

    s = cin_skipcomment(s);

    ਜੇ (*s == '{' || (*s == '}' && !cin_iselse(s)))
	ਮਿਲਿਆ_ਸ਼ੁਰੂ = *s;

    ਜੇ (!ਮਿਲਿਆ_ਸ਼ੁਰੂ)
	is_else = cin_iselse(s);

    ਜਦੋਂ (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	ਜੇ (*s == '}' && n_open > 0)
	    --n_open;
	ਜੇ ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    ਵਾਪਸ *s;
	ਨਹੀਂਤਾਂ ਜੇ (*s == '{')
	{
	    ਜੇ (incl_open && cin_nocode(s + 1))
		ਵਾਪਸ *s;
	    ਨਹੀਂਤਾਂ
		++n_open;
	}

	ਜੇ (*s)
	    s++;
    }
    ਵਾਪਸ ਮਿਲਿਆ_ਸ਼ੁਰੂ;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_starts_with(char_u *s, ਅੱਖਰ *ਸ਼ਬਦ)
{
    ਪੂਰਨ l = (ਪੂਰਨ)STRLEN(ਸ਼ਬਦ);

    ਵਾਪਸ (STRNCMP(s, ਸ਼ਬਦ, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_isdefault(char_u *s)
{
    ਵਾਪਸ (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    ਸਥਿਰ ਪੂਰਨ
cin_iscase(
    char_u *s,
    ਪੂਰਨ strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    ਜੇ (cin_starts_with(s, "case"))
    {
	ਲਈ (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    ਜੇ (*s == NUL)
		ਰੋਕੋ;
	    ਜੇ (*s == ':')
	    {
		ਜੇ (s[1] == ':')	// skip over "::" for C++
		    ++s;
		ਨਹੀਂਤਾਂ
		    ਵਾਪਸ TRUE;
	    }
	    ਜੇ (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    ਨਹੀਂਤਾਂ ਜੇ (*s == '/' && (s[1] == '*' || s[1] == '/'))
		ਵਾਪਸ FALSE;		// stop at comment
	    ਨਹੀਂਤਾਂ ਜੇ (*s == '"')
	    {
		// JS etc.
		ਜੇ (strict)
		    ਵਾਪਸ FALSE;		// stop at string
		ਨਹੀਂਤਾਂ
		    ਵਾਪਸ TRUE;
	    }
	}
	ਵਾਪਸ FALSE;
    }

    ਜੇ (cin_isdefault(s))
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_islabel(ਖਾਲੀ)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    ਜੇ (cin_isdefault(s))
	ਵਾਪਸ FALSE;
    ਜੇ (cin_isscopedecl(s))
	ਵਾਪਸ FALSE;

    ਜੇ (!cin_islabel_skip(&s))
	ਵਾਪਸ FALSE;

    ਜੇ (ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(ਨਿਰਾ))
	ਵਾਪਸ FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*ਸਤਰ;

    cursor_save = curwin->w_cursor;
    ਜਦੋਂ (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.ਖਾਨਾ = 0;
	ਜੇ ((trypos = ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(ਨਿਰਾ)) != ਨਿਰਾ) // XXX
	    curwin->w_cursor = *trypos;

	ਸਤਰ = ml_get_curline();
	ਜੇ (cin_ispreproc(ਸਤਰ))	// ignore #defines, #if, etc.
	    ਜਾਰੀ;
	ਜੇ (*(ਸਤਰ = cin_skipcomment(ਸਤਰ)) == NUL)
	    ਜਾਰੀ;

	curwin->w_cursor = cursor_save;
	ਜੇ (cin_isterminated(ਸਤਰ, TRUE, FALSE)
		|| cin_isscopedecl(ਸਤਰ)
		|| cin_iscase(ਸਤਰ, TRUE)
		|| (cin_islabel_skip(&ਸਤਰ) && cin_nocode(ਸਤਰ)))
	    ਵਾਪਸ TRUE;
	ਵਾਪਸ FALSE;
    }
    curwin->w_cursor = cursor_save;
    ਵਾਪਸ TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_ਸਿਰੇ_ਵਿੱਚ(char_u *s, char_u *ਲੱਭੋ)
{
    char_u	*p = s;
    char_u	*r;
    ਪੂਰਨ		len = (ਪੂਰਨ)STRLEN(ਲੱਭੋ);

    ਜਦੋਂ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ਜੇ (STRNCMP(p, ਲੱਭੋ, len) == 0)
	{
	    r = skipwhite(p + len);
	    ਜੇ (cin_nocode(r))
		ਵਾਪਸ TRUE;
	}
	ਜੇ (*p != NUL)
	    ++p;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    ਸਥਿਰ char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = ਨਿਰਾ, *p = s;
    ਕਰੋ
    {
	r = p;
	p = cin_skipcomment(p);
	ਜੇ (*p)
	    p = skip_string(p);
    } ਜਦੋਂ (p != r);
    ਵਾਪਸ p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = ਨਿਰਾ;

    ਜਦੋਂ (*p)
    {
	ਜੇ (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	ਨਹੀਂਤਾਂ
	    p = cin_skip_comment_and_string(p + 1);
    }
    ਜੇ (!r)
	ਵਾਪਸ FALSE;
    p = r; // p points now after '=' or "return"

    ਜੇ (cin_nocode(p))
	ਵਾਪਸ TRUE;

    ਜੇ (*p == '&')
	p = cin_skipcomment(p + 1);

    ਜੇ (*p == '(') // skip a typecast
    {
	ਪੂਰਨ open_ਗਿਣਤੀ = 1;
	ਕਰੋ
	{
	    p = cin_skip_comment_and_string(p + 1);
	    ਜੇ (cin_nocode(p))
		ਵਾਪਸ TRUE;
	    open_ਗਿਣਤੀ += (*p == '(') - (*p == ')');
	} ਜਦੋਂ (open_ਗਿਣਤੀ);
	p = cin_skipcomment(p + 1);
	ਜੇ (cin_nocode(p))
	    ਵਾਪਸ TRUE;
    }

    ਜਦੋਂ (*p == '{')
	p = cin_skipcomment(p + 1);
    ਵਾਪਸ cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_isinit(ਖਾਲੀ)
{
    char_u	*s;
    ਸਥਿਰ ਅੱਖਰ *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    ਜੇ (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    ਲਈ (;;)
    {
	ਪੂਰਨ i, l;

	ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(skip); ++i)
	{
	    l = (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(skip[i]);
	    ਜੇ (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (l != 0)
	    ਰੋਕੋ;
    }

    ਜੇ (cin_starts_with(s, "enum"))
	ਵਾਪਸ TRUE;

    ਵਾਪਸ cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    ਪੂਰਨ		has_name = FALSE;
    ਪੂਰਨ		has_name_ਸ਼ੁਰੂ = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    ਜਦੋਂ ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    ਜੇ (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	ਜਦੋਂ (*p != NUL)
	{
	    ਜੇ (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '{')
	    {
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (vim_iswordc(*p))
	    {
		has_name_ਸ਼ੁਰੂ = TRUE;
		ਜੇ (has_name)
		    ਵਾਪਸ FALSE; // word character after skipping past name
		++p;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		ਜੇ (!has_name_ਸ਼ੁਰੂ || has_name)
		    ਵਾਪਸ FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਵਾਪਸ FALSE;
	    }
	}
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    ਪੂਰਨ		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    ਜੇ (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	ਜਦੋਂ (*p != NUL)
	{
	    ਜੇ (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '{')
	    {
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		ਜੇ (has_string_literal)
		    ਵਾਪਸ FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		ਜੇ (has_string_literal)
		    ਵਾਪਸ FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਵਾਪਸ FALSE;
	    }
	}
	ਵਾਪਸ has_string_literal ? TRUE : FALSE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    ਸਥਿਰ char_u *
after_label(char_u *l)
{
    ਲਈ ( ; *l; ++l)
    {
	ਜੇ (*l == ':')
	{
	    ਜੇ (l[1] == ':')	    // skip over "::" for C++
		++l;
	    ਨਹੀਂਤਾਂ ਜੇ (!cin_iscase(l + 1, FALSE))
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    ਜੇ (*l == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    l = cin_skipcomment(l + 1);
    ਜੇ (*l == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    ਸਥਿਰ ਪੂਰਨ
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	ਖਾਨਾ;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ 0;

    fp.ਖਾਨਾ = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
    ਵਾਪਸ (ਪੂਰਨ)ਖਾਨਾ;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    ਸਥਿਰ ਪੂਰਨ
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    ਪੂਰਨ		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    ਜੇ (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	ਜੇ (l == ਨਿਰਾ)		// just in case
	    l = ml_get_curline();
    }
    ਨਹੀਂਤਾਂ
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    ਵਾਪਸ amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_ਪਹਿਲਾ_id_amount(ਖਾਲੀ)
{
    char_u	*ਸਤਰ, *p, *s;
    ਪੂਰਨ		len;
    pos_T	fp;
    colnr_T	ਖਾਨਾ;

    ਸਤਰ = ml_get_curline();
    p = skipwhite(ਸਤਰ);
    len = (ਪੂਰਨ)(skiptowhite(p) - p);
    ਜੇ (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (ਪੂਰਨ)(skiptowhite(p) - p);
    }
    ਜੇ (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    ਨਹੀਂਤਾਂ ਜੇ (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    ਨਹੀਂਤਾਂ ਜੇ ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	ਜੇ ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    ਲਈ (len = 0; vim_isIDc(p[len]); ++len)
	;
    ਜੇ (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	ਵਾਪਸ 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.ਖਾਨਾ = (colnr_T)(p - ਸਤਰ);
    getvcol(curwin, &fp, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
    ਵਾਪਸ (ਪੂਰਨ)ਖਾਨਾ;
}

/*
 * Return the indent of the first non-blank after an equal sign.
 *       char *foo = "here";
 * Return zero if no (useful) equal sign found.
 * Return -1 if the line above "lnum" ends in a backslash.
 *      foo = "asdf\
 *	       asdf\
 *	       here";
 */
    ਸਥਿਰ ਪੂਰਨ
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*ਸਤਰ;
    char_u	*s;
    colnr_T	ਖਾਨਾ;
    pos_T	fp;

    ਜੇ (lnum > 1)
    {
	ਸਤਰ = ml_get(lnum - 1);
	ਜੇ (*ਸਤਰ != NUL && ਸਤਰ[STRLEN(ਸਤਰ) - 1] == '\\')
	    ਵਾਪਸ -1;
    }

    ਸਤਰ = s = ml_get(lnum);
    ਜਦੋਂ (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == ਨਿਰਾ)
    {
	ਜੇ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ਨਹੀਂਤਾਂ
	    ++s;
    }
    ਜੇ (*s != '=')
	ਵਾਪਸ 0;

    s = skipwhite(s + 1);
    ਜੇ (cin_nocode(s))
	ਵਾਪਸ 0;

    ਜੇ (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.ਖਾਨਾ = (colnr_T)(s - ਸਤਰ);
    getvcol(curwin, &fp, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
    ਵਾਪਸ (ਪੂਰਨ)ਖਾਨਾ;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_skip2pos(pos_T *trypos)
{
    char_u	*ਸਤਰ;
    char_u	*p;
    char_u	*new_p;

    p = ਸਤਰ = ml_get(trypos->lnum);
    ਜਦੋਂ (*p && (colnr_T)(p - ਸਤਰ) < trypos->ਖਾਨਾ)
    {
	ਜੇ (cin_iscomment(p))
	    p = cin_skipcomment(p);
	ਨਹੀਂਤਾਂ
	{
	    new_p = skip_string(p);
	    ਜੇ (new_p == p)
		++p;
	    ਨਹੀਂਤਾਂ
		p = new_p;
	}
    }
    ਵਾਪਸ (ਪੂਰਨ)(p - ਸਤਰ);
}

    ਸਥਿਰ pos_T *
ਲੱਭੋ_ਮੇਲ_char(ਪੂਰਨ c, ਪੂਰਨ ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ਸਥਿਰ pos_T pos_copy;
    ਪੂਰਨ		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    ਜੇ ((trypos = findmatchlimit(ਨਿਰਾ, c, FM_SKIPCOMM, ind_maxp_wk)) != ਨਿਰਾ)
    {
	// check if the ( is in a // comment
	ਜੇ ((colnr_T)cin_skip2pos(trypos) > trypos->ਖਾਨਾ)
	{
	    ind_maxp_wk = ind_maxparen - (ਪੂਰਨ)(cursor_save.lnum - trypos->lnum);
	    ਜੇ (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.ਖਾਨਾ = 0;	// XXX
		ਜਾਓ retry;
	    }
	    trypos = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    ਜੇ ((trypos_wk = ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(ਨਿਰਾ)) != ਨਿਰਾ) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (ਪੂਰਨ)(cursor_save.lnum
			- trypos_wk->lnum);
		ਜੇ (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    ਜਾਓ retry;
		}
		trypos = ਨਿਰਾ;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    ਵਾਪਸ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    ਸਥਿਰ pos_T *
ਲੱਭੋ_ਮੇਲ_paren(ਪੂਰਨ ind_maxparen)	// XXX
{
    ਵਾਪਸ ਲੱਭੋ_ਮੇਲ_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_ਆਖਰੀ_paren(char_u *l, ਪੂਰਨ ਸ਼ੁਰੂ, ਪੂਰਨ ਸਿਰਾ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		retval = FALSE;
    ਪੂਰਨ		open_ਗਿਣਤੀ = 0;

    curwin->w_cursor.ਖਾਨਾ = 0;		    // default is start of line

    ਲਈ (i = 0; l[i] != NUL; i++)
    {
	i = (ਪੂਰਨ)(cin_skipcomment(l + i) - l); // ignore parens in comments
	ਜੇ (l[i] == NUL)
	    ਰੋਕੋ;
	i = (ਪੂਰਨ)(skip_string(l + i) - l);    // ignore parens in quotes
	ਜੇ (l[i] == ਸ਼ੁਰੂ)
	    ++open_ਗਿਣਤੀ;
	ਨਹੀਂਤਾਂ ਜੇ (l[i] == ਸਿਰਾ)
	{
	    ਜੇ (open_ਗਿਣਤੀ > 0)
		--open_ਗਿਣਤੀ;
	    ਨਹੀਂਤਾਂ
	    {
		curwin->w_cursor.ਖਾਨਾ = i;
		retval = TRUE;
	    }
	}
    }
    ਵਾਪਸ retval;
}

/*
 * Recognize the basic picture of a function declaration -- it needs to
 * have an open paren somewhere and a close paren at the end of the line and
 * no semicolons anywhere.
 * When a line ends in a comma we continue looking in the next line.
 * "sp" points to a string with the line.  When looking at other lines it must
 * be restored to the line.  When it's NULL fetch lines here.
 * "first_lnum" is where we start looking.
 * "min_lnum" is the line before which we will not be looking.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	ਪਹਿਲਾ_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = ਪਹਿਲਾ_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    ਪੂਰਨ		retval = FALSE;
    pos_T	*trypos;
    ਪੂਰਨ		just_started = TRUE;

    ਜੇ (sp == ਨਿਰਾ)
	s = ml_get(lnum);
    ਨਹੀਂਤਾਂ
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    ਜੇ (ਲੱਭੋ_ਆਖਰੀ_paren(s, '(', ')')
	&& (trypos = ਲੱਭੋ_ਮੇਲ_paren(curbuf->b_ind_maxparen)) != ਨਿਰਾ)
    {
	lnum = trypos->lnum;
	ਜੇ (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    ਵਾਪਸ FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    ਜੇ (cin_ispreproc(s))
	ਵਾਪਸ FALSE;

    ਜਦੋਂ (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	ਜੇ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ਨਹੀਂਤਾਂ ਜੇ (*s == ':')
	{
	    ਜੇ (*(s + 1) == ':')
		s += 2;
	    ਨਹੀਂਤਾਂ
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		ਵਾਪਸ FALSE;
	}
	ਨਹੀਂਤਾਂ
	    ++s;
    }
    ਜੇ (*s != '(')
	ਵਾਪਸ FALSE;		// ';', ' or "  before any () or no '('

    ਜਦੋਂ (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	ਜੇ (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = ਪਹਿਲਾ_lnum - 1;
	    s = ml_get(lnum);
	    ਜੇ (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    ਜਾਓ ਪੂਰਾ;
	}
	ਜੇ ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    ਪੂਰਨ comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    ਲਈ (;;)
	    {
		ਜੇ (lnum >= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		    ਰੋਕੋ;
		s = ml_get(++lnum);
		ਜੇ (!cin_ispreproc(s))
		    ਰੋਕੋ;
	    }
	    ਜੇ (lnum >= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਰੋਕੋ;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    ਜੇ (!just_started && (!comma && *s != ',' && *s != ')'))
		ਰੋਕੋ;
	    just_started = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ਨਹੀਂਤਾਂ
	{
	    ++s;
	    just_started = FALSE;
	}
    }

ਪੂਰਾ:
    ਜੇ (lnum != ਪਹਿਲਾ_lnum && sp != ਨਿਰਾ)
	*sp = ml_get(ਪਹਿਲਾ_lnum);

    ਵਾਪਸ retval;
}

    ਸਥਿਰ ਪੂਰਨ
cin_isif(char_u *p)
{
    ਵਾਪਸ (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    ਸਥਿਰ ਪੂਰਨ
cin_isdo(char_u *p)
{
    ਵਾਪਸ (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ਪੂਰਨ		retval = FALSE;

    p = cin_skipcomment(p);
    ਜੇ (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    ਜੇ (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.ਖਾਨਾ = 0;
	p = ml_get_curline();
	ਜਦੋਂ (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.ਖਾਨਾ;
	}
	ਜੇ ((trypos = findmatchlimit(ਨਿਰਾ, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != ਨਿਰਾ
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    ਵਾਪਸ retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    ਸਥਿਰ ਪੂਰਨ
cin_is_if_for_while_before_ਵਿੱਥ(char_u *ਸਤਰ, ਪੂਰਨ *poffset)
{
    ਪੂਰਨ ਵਿੱਥ = *poffset;

    ਜੇ (ਵਿੱਥ-- < 2)
	ਵਾਪਸ 0;
    ਜਦੋਂ (ਵਿੱਥ > 2 && VIM_ISWHITE(ਸਤਰ[ਵਿੱਥ]))
	--ਵਿੱਥ;

    ਵਿੱਥ -= 1;
    ਜੇ (STRNCMP(ਸਤਰ + ਵਿੱਥ, "if", 2) == 0)
	ਜਾਓ probably_ਮਿਲਿਆ;

    ਜੇ (ਵਿੱਥ >= 1)
    {
	ਵਿੱਥ -= 1;
	ਜੇ (STRNCMP(ਸਤਰ + ਵਿੱਥ, "for", 3) == 0)
	    ਜਾਓ probably_ਮਿਲਿਆ;

	ਜੇ (ਵਿੱਥ >= 2)
	{
	    ਵਿੱਥ -= 2;
	    ਜੇ (STRNCMP(ਸਤਰ + ਵਿੱਥ, "while", 5) == 0)
		ਜਾਓ probably_ਮਿਲਿਆ;
	}
    }
    ਵਾਪਸ 0;

probably_ਮਿਲਿਆ:
    ਜੇ (!ਵਿੱਥ || !vim_isIDc(ਸਤਰ[ਵਿੱਥ - 1]))
    {
	*poffset = ਵਿੱਥ;
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    ਸਥਿਰ ਪੂਰਨ
cin_iswhileofdo_ਸਿਰਾ(ਪੂਰਨ terminated)
{
    char_u	*ਸਤਰ;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    ਪੂਰਨ		i;

    ਜੇ (terminated != ';')	// there must be a ';' at the end
	ਵਾਪਸ FALSE;

    p = ਸਤਰ = ml_get_curline();
    ਜਦੋਂ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ਜੇ (*p == ')')
	{
	    s = skipwhite(p + 1);
	    ਜੇ (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (ਪੂਰਨ)(p - ਸਤਰ);
		curwin->w_cursor.ਖਾਨਾ = i;
		trypos = ਲੱਭੋ_ਮੇਲ_paren(curbuf->b_ind_maxparen);
		ਜੇ (trypos != ਨਿਰਾ)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    ਜੇ (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    ਜੇ (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			ਵਾਪਸ TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		ਸਤਰ = ml_get_curline();
		p = ਸਤਰ + i;
	    }
	}
	ਜੇ (*p != NUL)
	    ++p;
    }
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਪੂਰਨ
cin_isbreak(char_u *p)
{
    ਵਾਪਸ (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
}

/*
 * Find the position of a C++ base-class declaration or
 * constructor-initialization. eg:
 *
 * class MyClass :
 *	baseClass		<-- here
 * class MyClass : public baseClass,
 *	anotherBaseClass	<-- here (should probably lineup ??)
 * MyClass::MyClass(...) :
 *	baseClass(...)		<-- here (constructor-initialization)
 *
 * This is a lot of guessing.  Watch out for "cond ? func() : foo".
 */
    ਸਥਿਰ ਪੂਰਨ
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    ਪੂਰਨ		class_or_struct, lookfor_ctor_init, cpp_ਅਧਾਰ_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*ਸਤਰ = ml_get_curline();

    ਜੇ (pos->lnum <= lnum)
	ਵਾਪਸ cached->ਮਿਲਿਆ;	// Use the cached result

    pos->ਖਾਨਾ = 0;

    s = skipwhite(ਸਤਰ);
    ਜੇ (*s == '#')		// skip #define FOO x ? (x) : x
	ਵਾਪਸ FALSE;
    s = cin_skipcomment(s);
    ਜੇ (*s == NUL)
	ਵਾਪਸ FALSE;

    cpp_ਅਧਾਰ_class = lookfor_ctor_init = class_or_struct = FALSE;

    // Search for a line starting with '#', empty, ending in ';' or containing
    // '{' or '}' and start below it.  This handles the following situations:
    //	a = cond ?
    //	      func() :
    //		   asdf;
    //	func::foo()
    //	      : something
    //	{}
    //	Foo::Foo (int one, int two)
    //		: something(4),
    //		somethingelse(3)
    //	{}
    ਜਦੋਂ (lnum > 1)
    {
	ਸਤਰ = ml_get(lnum - 1);
	s = skipwhite(ਸਤਰ);
	ਜੇ (*s == '#' || *s == NUL)
	    ਰੋਕੋ;
	ਜਦੋਂ (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    ਜੇ (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		ਰੋਕੋ;
	    ਜੇ (*s != NUL)
		++s;
	}
	ਜੇ (*s != NUL)
	    ਰੋਕੋ;
	--lnum;
    }

    pos->lnum = lnum;
    ਸਤਰ = ml_get(lnum);
    s = ਸਤਰ;
    ਲਈ (;;)
    {
	ਜੇ (*s == NUL)
	{
	    ਜੇ (lnum == curwin->w_cursor.lnum)
		ਰੋਕੋ;
	    // Continue in the cursor line.
	    ਸਤਰ = ml_get(++lnum);
	    s = ਸਤਰ;
	}
	ਜੇ (s == ਸਤਰ)
	{
	    // don't recognize "case (foo):" as a baseclass
	    ਜੇ (cin_iscase(s, FALSE))
		ਰੋਕੋ;
	    s = cin_skipcomment(ਸਤਰ);
	    ਜੇ (*s == NUL)
		ਜਾਰੀ;
	}

	ਜੇ (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	ਨਹੀਂਤਾਂ ਜੇ (s[0] == ':')
	{
	    ਜੇ (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_ਅਧਾਰ_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->ਖਾਨਾ = 0;
		s = cin_skipcomment(s + 1);
	    }
	    ਨਹੀਂਤਾਂ
		s = cin_skipcomment(s + 1);
	}
	ਨਹੀਂਤਾਂ ਜੇ ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    ਜੇ (*s == 'c')
		s = cin_skipcomment(s + 5);
	    ਨਹੀਂਤਾਂ
		s = cin_skipcomment(s + 6);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_ਅਧਾਰ_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		ਵਾਪਸ FALSE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (pos->ਖਾਨਾ == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		ਜੇ (cpp_ਅਧਾਰ_class)
		    pos->ਖਾਨਾ = (colnr_T)(s - ਸਤਰ);
	    }

	    // When the line ends in a comma don't align with it.
	    ਜੇ (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->ਖਾਨਾ = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->ਮਿਲਿਆ = cpp_ਅਧਾਰ_class;
    ਜੇ (cpp_ਅਧਾਰ_class)
	pos->lnum = lnum;
    ਵਾਪਸ cpp_ਅਧਾਰ_class;
}

    ਸਥਿਰ ਪੂਰਨ
get_baseclass_amount(ਪੂਰਨ ਖਾਨਾ)
{
    ਪੂਰਨ		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    ਜੇ (ਖਾਨਾ == 0)
    {
	amount = get_indent();
	ਜੇ (ਲੱਭੋ_ਆਖਰੀ_paren(ml_get_curline(), '(', ')')
		&& (trypos = ਲੱਭੋ_ਮੇਲ_paren(curbuf->b_ind_maxparen)) != ਨਿਰਾ)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	ਜੇ (!cin_ਸਿਰੇ_ਵਿੱਚ(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    ਨਹੀਂਤਾਂ
    {
	curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
	getvcol(curwin, &curwin->w_cursor, &vcol, ਨਿਰਾ, ਨਿਰਾ, 0);
	amount = (ਪੂਰਨ)vcol;
    }
    ਜੇ (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    ਵਾਪਸ amount;
}

/*
 * Find the '{' at the start of the block we are in.
 * Return NULL if no match found.
 * Ignore a '{' that is in a comment, makes indenting the next three lines
 * work.
 */
// foo()
// {
// }

    ਸਥਿਰ pos_T *
ਲੱਭੋ_ਸ਼ੁਰੂ_brace(ਖਾਲੀ)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    ਸਥਿਰ pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    ਜਦੋਂ ((trypos = findmatchlimit(ਨਿਰਾ, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != ਨਿਰਾ)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = ਨਿਰਾ;
	// ignore the { if it's in a // or / *  * / comment
	ਜੇ ((colnr_T)cin_skip2pos(trypos) == trypos->ਖਾਨਾ
		       && (pos = ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(ਨਿਰਾ)) == ਨਿਰਾ) // XXX
	    ਰੋਕੋ;
	ਜੇ (pos != ਨਿਰਾ)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    ਵਾਪਸ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    ਸਥਿਰ pos_T *
ਲੱਭੋ_ਮੇਲ_paren_after_brace (ਪੂਰਨ ind_maxparen)	    // XXX
{
    pos_T	*trypos = ਲੱਭੋ_ਮੇਲ_paren(ind_maxparen);

    ਜੇ (trypos == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    pos_T	*tryposBrace = ਲੱਭੋ_ਸ਼ੁਰੂ_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    ਜੇ (tryposBrace != ਨਿਰਾ
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->ਖਾਨਾ < tryposBrace->ਖਾਨਾ))
	trypos = ਨਿਰਾ;
    ਵਾਪਸ trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    ਸਥਿਰ ਪੂਰਨ
corr_ind_maxparen(pos_T *startpos)
{
    ਲੰਮਾ	n = (ਲੰਮਾ)startpos->lnum - (ਲੰਮਾ)curwin->w_cursor.lnum;

    ਜੇ (n > 0 && n < curbuf->b_ind_maxparen / 2)
	ਵਾਪਸ curbuf->b_ind_maxparen - (ਪੂਰਨ)n;
    ਵਾਪਸ curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    ਖਾਲੀ
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    ਪੂਰਨ		divider;
    ਪੂਰਨ		fraction = 0;
    ਪੂਰਨ		sw;
    ਲੰਮਾ	t = get_sw_ਮੁੱਲ(buf);

    // needed for cino-(, it will be multiplied by 2 again
    ਜੇ (t > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ / 2)
	sw = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ / 2;
    ਨਹੀਂਤਾਂ
	sw = (ਪੂਰਨ)t;

    // Set the default values.

    // Spaces from a block's opening brace the prevailing indent for that
    // block should be.
    buf->b_ind_level = sw;

    // Spaces from the edge of the line an open brace that's at the end of a
    // line is imagined to be.
    buf->b_ind_open_imag = 0;

    // Spaces from the prevailing indent for a line that is not preceded by
    // an opening brace.
    buf->b_ind_no_brace = 0;

    // Column where the first { of a function should be located }.
    buf->b_ind_ਪਹਿਲਾ_open = 0;

    // Spaces from the prevailing indent a leftmost open brace should be
    // located.
    buf->b_ind_open_extra = 0;

    // Spaces from the matching open brace (real location for one at the left
    // edge; imaginary location from one that ends a line) the matching close
    // brace should be located.
    buf->b_ind_close_extra = 0;

    // Spaces from the edge of the line an open brace sitting in the leftmost
    // column is imagined to be.
    buf->b_ind_open_ਖੱਬਾ_imag = 0;

    // Spaces jump labels should be shifted to the left if N is non-negative,
    // otherwise the jump label will be put to column 1.
    buf->b_ind_jump_label = -1;

    // Spaces from the switch() indent a "case xx" label should be located.
    buf->b_ind_case = sw;

    // Spaces from the "case xx:" code after a switch() should be located.
    buf->b_ind_case_ਕੋਡ = sw;

    // Lineup break at end of case in switch() with case label.
    buf->b_ind_case_break = 0;

    // Spaces from the class declaration indent a scope declaration label
    // should be located.
    buf->b_ind_scopedecl = sw;

    // Spaces from the scope declaration label code should be located.
    buf->b_ind_scopedecl_ਕੋਡ = sw;

    // Amount K&R-style parameters should be indented.
    buf->b_ind_param = sw;

    // Amount a function type spec should be indented.
    buf->b_ind_func_type = sw;

    // Amount a cpp base class declaration or constructor initialization
    // should be indented.
    buf->b_ind_cpp_baseclass = sw;

    // additional spaces beyond the prevailing indent a continuation line
    // should be located.
    buf->b_ind_continuation = sw;

    // Spaces from the indent of the line with an unclosed parenthesis.
    buf->b_ind_unclosed = sw * 2;

    // Spaces from the indent of the line with an unclosed parenthesis, which
    // itself is also unclosed.
    buf->b_ind_unclosed2 = sw;

    // Suppress ignoring spaces from the indent of a line starting with an
    // unclosed parenthesis.
    buf->b_ind_unclosed_noignore = 0;

    // If the opening paren is the last nonwhite character on the line, and
    // b_ind_unclosed_wrapped is nonzero, use this indent relative to the outer
    // context (for very long lines).
    buf->b_ind_unclosed_wrapped = 0;

    // Suppress ignoring white space when lining up with the character after
    // an unclosed parenthesis.
    buf->b_ind_unclosed_whiteok = 0;

    // Indent a closing parenthesis under the line start of the matching
    // opening parenthesis.
    buf->b_ind_matching_paren = 0;

    // Indent a closing parenthesis under the previous line.
    buf->b_ind_paren_prev = 0;

    // Extra indent for comments.
    buf->b_ind_comment = 0;

    // Spaces from the comment opener when there is nothing after it.
    buf->b_ind_ਵਿੱਚ_comment = 3;

    // Boolean: if non-zero, use b_ind_in_comment even if there is something
    // after the comment opener.
    buf->b_ind_ਵਿੱਚ_comment2 = 0;

    // Max lines to search for an open paren.
    buf->b_ind_maxparen = 20;

    // Max lines to search for an open comment.
    buf->b_ind_maxcomment = 70;

    // Handle braces for java code.
    buf->b_ind_java = 0;

    // Not to confuse JS object properties with labels.
    buf->b_ind_js = 0;

    // Handle blocked cases correctly.
    buf->b_ind_keep_case_label = 0;

    // Handle C++ namespace.
    buf->b_ind_cpp_namespace = 0;

    // Handle continuation lines containing conditions of if (), for () and
    // while ().
    buf->b_ind_if_for_while = 0;

    // indentation for # comments
    buf->b_ind_hash_comment = 0;

    // Handle C++ extern "C" or "C++"
    buf->b_ind_cpp_extern_c = 0;

    // Handle C #pragma directives
    buf->b_ind_pragma = 0;

    ਲਈ (p = buf->b_p_cino; *p; )
    {
	l = p++;
	ਜੇ (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	ਜੇ (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((ਅੱਖਰ *)++p);
	    ਜਦੋਂ (VIM_ISDIGIT(*p))
	    {
		++p;
		ਜੇ (divider)
		    divider *= 10;
		ਨਹੀਂਤਾਂ
		    divider = 10;
	    }
	}
	ਜੇ (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    ਜੇ (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    ਨਹੀਂਤਾਂ
	    {
		n *= sw;
		ਜੇ (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	ਜੇ (l[1] == '-')
	    n = -n;

	n = trim_ਮੰਜ਼ਿਲ_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	ਚੋਣ (*l)
	{
	    ਹਾਲਤ '>': buf->b_ind_level = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'e': buf->b_ind_open_imag = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'n': buf->b_ind_no_brace = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'f': buf->b_ind_ਪਹਿਲਾ_open = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ '{': buf->b_ind_open_extra = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ '}': buf->b_ind_close_extra = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ '^': buf->b_ind_open_ਖੱਬਾ_imag = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'L': buf->b_ind_jump_label = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ ':': buf->b_ind_case = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ '=': buf->b_ind_case_ਕੋਡ = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'b': buf->b_ind_case_break = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'p': buf->b_ind_param = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 't': buf->b_ind_func_type = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ '/': buf->b_ind_comment = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'c': buf->b_ind_ਵਿੱਚ_comment = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'C': buf->b_ind_ਵਿੱਚ_comment2 = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'i': buf->b_ind_cpp_baseclass = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ '+': buf->b_ind_continuation = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ '(': buf->b_ind_unclosed = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'u': buf->b_ind_unclosed2 = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'U': buf->b_ind_unclosed_noignore = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'W': buf->b_ind_unclosed_wrapped = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'w': buf->b_ind_unclosed_whiteok = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'm': buf->b_ind_matching_paren = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'M': buf->b_ind_paren_prev = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ ')': buf->b_ind_maxparen = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ '*': buf->b_ind_maxcomment = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'g': buf->b_ind_scopedecl = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'h': buf->b_ind_scopedecl_ਕੋਡ = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'j': buf->b_ind_java = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'J': buf->b_ind_js = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'l': buf->b_ind_keep_case_label = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ '#': buf->b_ind_hash_comment = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'N': buf->b_ind_cpp_namespace = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'k': buf->b_ind_if_for_while = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'E': buf->b_ind_cpp_extern_c = (ਪੂਰਨ)n; ਰੋਕੋ;
	    ਹਾਲਤ 'P': buf->b_ind_pragma = (ਪੂਰਨ)n; ਰੋਕੋ;
	}
	ਜੇ (*p == ',')
	    ++p;
    }
}

    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_ਮੇਲ(ਪੂਰਨ lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    ਪੂਰਨ		elselevel;
    ਪੂਰਨ		whilelevel;

    ਜੇ (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.ਖਾਨਾ = 0;

    ਜਦੋਂ (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.ਖਾਨਾ = 0;

	look = cin_skipcomment(ml_get_curline());
	ਜੇ (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = ਲੱਭੋ_ਸ਼ੁਰੂ_brace();  // XXX
	    ਜੇ (theirscope == ਨਿਰਾ)
		ਰੋਕੋ;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    ਜੇ (theirscope->lnum < ourscope)
		ਰੋਕੋ;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    ਜੇ (theirscope->lnum > ourscope)
		ਜਾਰੀ;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    ਜੇ (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		ਜੇ (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    ਜੇ (!cin_isif(mightbeif))
			++elselevel;
		    ਜਾਰੀ;
		}

		// If it's an "if" decrement elselevel
		ਜੇ (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    ਜੇ (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    ਜੇ (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		ਜਾਰੀ;
	    }

	    // If it's a "do" decrement whilelevel
	    ਜੇ (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    ਜੇ (elselevel <= 0 && whilelevel <= 0)
		ਵਾਪਸ OK;
	}
    }
    ਵਾਪਸ FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    ਪੂਰਨ
get_c_indent(ਖਾਲੀ)
{
    pos_T	cur_curpos;
    ਪੂਰਨ		amount;
    ਪੂਰਨ		scope_amount;
    ਪੂਰਨ		cur_amount = MAXCOL;
    colnr_T	ਖਾਨਾ;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = ਨਿਰਾ;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*ਸ਼ੁਰੂ;
    ਪੂਰਨ		ਸ਼ੁਰੂ_brace;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BRACE_ਵਿੱਚ_COL0		1	    // '{' is in column 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BRACE_AT_ਸ਼ੁਰੂ		2	    // '{' is at start of line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BRACE_AT_ਸਿਰਾ		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    ਪੂਰਨ		lookfor;
    ਪੂਰਨ		whilelevel;
    linenr_T	lnum;
    ਪੂਰਨ		n;
    ਪੂਰਨ		iscase;
    ਪੂਰਨ		lookfor_break;
    ਪੂਰਨ		lookfor_cpp_namespace = FALSE;
    ਪੂਰਨ		cont_amount = 0;    // amount for continuation line
    ਪੂਰਨ		original_ਸਤਰ_islabel;
    ਪੂਰਨ		added_ਮੰਜ਼ਿਲ_amount = 0;
    ਪੂਰਨ		js_cur_has_ਕੁੰਜੀ = 0;
    linenr_T	raw_string_ਸ਼ੁਰੂ = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    ਪੂਰਨ		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    ਜੇ (cur_curpos.lnum == 1)
	ਵਾਪਸ 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    ਜੇ (linecopy == ਨਿਰਾ)
	ਵਾਪਸ 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ)
	    && curwin->w_cursor.ਖਾਨਾ < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.ਖਾਨਾ] == ')')
	linecopy[curwin->w_cursor.ਖਾਨਾ] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.ਖਾਨਾ = 0;

    original_ਸਤਰ_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_ਲੱਭੋ_ਸ਼ੁਰੂ_comment();
    ਜੇ (comment_pos != ਨਿਰਾ)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = ਲੱਭੋ_ਸ਼ੁਰੂ_rawstring(curbuf->b_ind_maxcomment);
    ਜੇ (trypos != ਨਿਰਾ && (comment_pos == ਨਿਰਾ
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	ਜਾਓ laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    ਜੇ (*theline == '#' && (*linecopy == '#' || ਵਿੱਚ_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	ਜੇ (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    ਜਾਓ theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    ਜੇ (original_ਸਤਰ_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	ਜਾਓ theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    ਜੇ (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = ਲੱਭੋ_ਸਤਰ_comment(); // XXX
	ਜੇ (trypos == ਨਿਰਾ && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.ਖਾਨਾ =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    ਜੇ (linecomment_pos.ਖਾਨਾ != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	ਜੇ (trypos  != ਨਿਰਾ)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
	    amount = ਖਾਨਾ;
	    ਜਾਓ theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    ਜੇ (!cin_iscomment(theline) && comment_pos != ਨਿਰਾ) // XXX
    {
	ਪੂਰਨ	lead_ਸ਼ੁਰੂ_len = 2;
	ਪੂਰਨ	lead_ਵਿਚਕਾਰ_len = 1;
	char_u	lead_ਸ਼ੁਰੂ[COM_MAX_LEN];	// start-comment string
	char_u	lead_ਵਿਚਕਾਰ[COM_MAX_LEN];	// middle-comment string
	char_u	lead_ਸਿਰਾ[COM_MAX_LEN];		// end-comment string
	ਪੂਰਨ	lead_ਸਿਰਾ_len;
	char_u	*p;
	ਪੂਰਨ	ਸ਼ੁਰੂ_align = 0;
	ਪੂਰਨ	ਸ਼ੁਰੂ_off = 0;
	ਪੂਰਨ	ਪੂਰਾ = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
	amount = ਖਾਨਾ;
	*lead_ਸ਼ੁਰੂ = NUL;
	*lead_ਵਿਚਕਾਰ = NUL;

	p = curbuf->b_p_com;
	ਜਦੋਂ (*p != NUL)
	{
	    ਪੂਰਨ	align = 0;
	    ਪੂਰਨ	off = 0;
	    ਪੂਰਨ what = 0;

	    ਜਦੋਂ (*p != NUL && *p != ':')
	    {
		ਜੇ (*p == COM_ਸ਼ੁਰੂ || *p == COM_ਸਿਰਾ || *p == COM_ਵਿਚਕਾਰ)
		    what = *p++;
		ਨਹੀਂਤਾਂ ਜੇ (*p == COM_ਖੱਬਾ || *p == COM_ਸੱਜਾ)
		    align = *p++;
		ਨਹੀਂਤਾਂ ਜੇ (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		ਨਹੀਂਤਾਂ
		    ++p;
	    }

	    ਜੇ (*p == ':')
		++p;
	    lead_ਸਿਰਾ_len = copy_option_part(&p, lead_ਸਿਰਾ, COM_MAX_LEN, ",");
	    ਜੇ (what == COM_ਸ਼ੁਰੂ)
	    {
		STRCPY(lead_ਸ਼ੁਰੂ, lead_ਸਿਰਾ);
		lead_ਸ਼ੁਰੂ_len = lead_ਸਿਰਾ_len;
		ਸ਼ੁਰੂ_off = off;
		ਸ਼ੁਰੂ_align = align;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (what == COM_ਵਿਚਕਾਰ)
	    {
		STRCPY(lead_ਵਿਚਕਾਰ, lead_ਸਿਰਾ);
		lead_ਵਿਚਕਾਰ_len = lead_ਸਿਰਾ_len;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (what == COM_ਸਿਰਾ)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		ਜੇ (STRNCMP(theline, lead_ਵਿਚਕਾਰ, lead_ਵਿਚਕਾਰ_len) == 0
			&& STRNCMP(theline, lead_ਸਿਰਾ, lead_ਸਿਰਾ_len) != 0)
		{
		    ਪੂਰਾ = TRUE;
		    ਜੇ (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			ਜੇ (STRNCMP(look, lead_ਸ਼ੁਰੂ, lead_ਸ਼ੁਰੂ_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(look, lead_ਵਿਚਕਾਰ,
							lead_ਵਿਚਕਾਰ_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    ਰੋਕੋ;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->ਖਾਨਾ,
					     lead_ਸ਼ੁਰੂ, lead_ਸ਼ੁਰੂ_len) != 0)
			    ਜਾਰੀ;
		    }
		    ਜੇ (ਸ਼ੁਰੂ_off != 0)
			amount += ਸ਼ੁਰੂ_off;
		    ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ_align == COM_ਸੱਜਾ)
			amount += vim_strsize(lead_ਸ਼ੁਰੂ)
						   - vim_strsize(lead_ਵਿਚਕਾਰ);
		    ਰੋਕੋ;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		ਜੇ (STRNCMP(theline, lead_ਵਿਚਕਾਰ, lead_ਵਿਚਕਾਰ_len) != 0
			&& STRNCMP(theline, lead_ਸਿਰਾ, lead_ਸਿਰਾ_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    ਜੇ (off != 0)
			amount += off;
		    ਨਹੀਂਤਾਂ ਜੇ (align == COM_ਸੱਜਾ)
			amount += vim_strsize(lead_ਸ਼ੁਰੂ)
						   - vim_strsize(lead_ਵਿਚਕਾਰ);
		    ਪੂਰਾ = TRUE;
		    ਰੋਕੋ;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	ਜੇ (ਪੂਰਾ)
	    ;
	ਨਹੀਂਤਾਂ ਜੇ (theline[0] == '*')
	    amount += 1;
	ਨਹੀਂਤਾਂ
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    ਲਈ (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		ਜੇ (linewhite(lnum))		    // skip blank lines
		    ਜਾਰੀ;
		amount = get_indent_lnum(lnum);	    // XXX
		ਰੋਕੋ;
	    }
	    ਜੇ (amount == -1)			    // use the comment opener
	    {
		ਜੇ (!curbuf->b_ind_ਵਿੱਚ_comment2)
		{
		    ਸ਼ੁਰੂ = ml_get(comment_pos->lnum);
		    look = ਸ਼ੁਰੂ + comment_pos->ਖਾਨਾ + 2; // skip / and *
		    ਜੇ (*look != NUL)		    // if something after it
			comment_pos->ਖਾਨਾ = (colnr_T)(skipwhite(look) - ਸ਼ੁਰੂ);
		}
		getvcol(curwin, comment_pos, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
		amount = ਖਾਨਾ;
		ਜੇ (curbuf->b_ind_ਵਿੱਚ_comment2 || *look == NUL)
		    amount += curbuf->b_ind_ਵਿੱਚ_comment;
	    }
	}
	ਜਾਓ theend;
    }

    // Are we looking at a ']' that has a match?
    ਜੇ (*skipwhite(theline) == ']'
	    && (trypos = ਲੱਭੋ_ਮੇਲ_char('[', curbuf->b_ind_maxparen)) != ਨਿਰਾ)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	ਜਾਓ theend;
    }

    // Are we inside parentheses or braces?  XXX
    ਜੇ (((trypos = ਲੱਭੋ_ਮੇਲ_paren(curbuf->b_ind_maxparen)) != ਨਿਰਾ
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = ਲੱਭੋ_ਸ਼ੁਰੂ_brace()) != ਨਿਰਾ
	    || trypos != ਨਿਰਾ)
    {
      ਜੇ (trypos != ਨਿਰਾ && tryposBrace != ਨਿਰਾ)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  ਜੇ (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->ਖਾਨਾ < tryposBrace->ਖਾਨਾ)
	      trypos = ਨਿਰਾ;
	  ਨਹੀਂਤਾਂ
	      tryposBrace = ਨਿਰਾ;
      }

      ਜੇ (trypos != ਨਿਰਾ)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	ਜੇ (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	ਨਹੀਂਤਾਂ
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    ਲਈ (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		ਜੇ (cin_nocode(l))		// skip comment lines
		    ਜਾਰੀ;
		ਜੇ (cin_ispreproc_cont(&l, &lnum, &amount))
		    ਜਾਰੀ;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		ਜੇ ((trypos = ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(ਨਿਰਾ)) != ਨਿਰਾ)
		{
		    lnum = trypos->lnum + 1;
		    ਜਾਰੀ;
		}

		// XXX
		ਜੇ ((trypos = ਲੱਭੋ_ਮੇਲ_paren(
			corr_ind_maxparen(&cur_curpos))) != ਨਿਰਾ
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->ਖਾਨਾ == our_paren_pos.ਖਾਨਾ)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    ਜੇ (theline[0] == ')')
		    {
			ਜੇ (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    ਰੋਕੋ;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	ਜੇ (amount == -1)
	{
	    ਪੂਰਨ	    ignore_paren_ਖਾਨਾ = 0;
	    ਪੂਰਨ	    is_if_for_while = 0;

	    ਜੇ (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *ਸਤਰ;

		trypos = &our_paren_pos;
		ਕਰੋ
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.ਖਾਨਾ = outermost.ਖਾਨਾ;

		    trypos = ਲੱਭੋ_ਮੇਲ_paren(curbuf->b_ind_maxparen);
		} ਜਦੋਂ (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		ਸਤਰ = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_ਵਿੱਥ(ਸਤਰ, &outermost.ਖਾਨਾ);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    ਜੇ (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *ਸਤਰ;
		ਪੂਰਨ	    look_ਖਾਨਾ;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		ਸਤਰ = ml_get_curline();
		look_ਖਾਨਾ = (ਪੂਰਨ)(look - ਸਤਰ);
		curwin->w_cursor.ਖਾਨਾ = look_ਖਾਨਾ + 1;
		ਜੇ ((trypos = findmatchlimit(ਨਿਰਾ, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != ਨਿਰਾ
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->ਖਾਨਾ < our_paren_pos.ਖਾਨਾ)
		    ignore_paren_ਖਾਨਾ = trypos->ਖਾਨਾ + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_ਖਾਨਾ;
	    }
	    ਜੇ (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
						      && is_if_for_while == 0)
		    || (!curbuf->b_ind_unclosed_noignore && *look == '('
						    && ignore_paren_ਖਾਨਾ == 0))
	    {
		// If we're looking at a close paren, line up right there;
		// otherwise, line up with the next (non-white) character.
		// When b_ind_unclosed_wrapped is set and the matching paren is
		// the last nonwhite character of the line, use either the
		// indent of the current line or the indentation of the next
		// outer paren and add b_ind_unclosed_wrapped (for very long
		// lines).
		ਜੇ (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    ਜੇ (curbuf->b_ind_unclosed_wrapped
				       && cin_ਸਿਰੇ_ਵਿੱਚ(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			ਲਈ (ਖਾਨਾ = 0; ਖਾਨਾ < our_paren_pos.ਖਾਨਾ; ++ਖਾਨਾ)
			{
			    ਚੋਣ (l[ਖਾਨਾ])
			    {
				ਹਾਲਤ '(':
				ਹਾਲਤ '{': ++n;
					  ਰੋਕੋ;

				ਹਾਲਤ ')':
				ਹਾਲਤ '}': ਜੇ (n > 1)
					      --n;
					  ਰੋਕੋ;
			    }
			}

			our_paren_pos.ਖਾਨਾ = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.ਖਾਨਾ++;
		    ਨਹੀਂਤਾਂ
		    {
			ਖਾਨਾ = our_paren_pos.ਖਾਨਾ + 1;
			ਜਦੋਂ (VIM_ISWHITE(l[ਖਾਨਾ]))
			    ਖਾਨਾ++;
			ਜੇ (l[ਖਾਨਾ] != NUL)	// In case of trailing space
			    our_paren_pos.ਖਾਨਾ = ਖਾਨਾ;
			ਨਹੀਂਤਾਂ
			    our_paren_pos.ਖਾਨਾ++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		ਜੇ (our_paren_pos.ਖਾਨਾ > 0)
		{
		    getvcol(curwin, &our_paren_pos, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
		    ਜੇ (cur_amount > (ਪੂਰਨ)ਖਾਨਾ)
			cur_amount = ਖਾਨਾ;
		}
	    }

	    ਜੇ (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_ਖਾਨਾ == 0))
	    {
		ਜੇ (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		ਖਾਨਾ = our_paren_pos.ਖਾਨਾ;
		ਜਦੋਂ ((ਪੂਰਨ)our_paren_pos.ਖਾਨਾ > ignore_paren_ਖਾਨਾ)
		{
		    --our_paren_pos.ਖਾਨਾ;
		    ਚੋਣ (*ml_get_pos(&our_paren_pos))
		    {
			ਹਾਲਤ '(': amount += curbuf->b_ind_unclosed2;
				  ਖਾਨਾ = our_paren_pos.ਖਾਨਾ;
				  ਰੋਕੋ;
			ਹਾਲਤ ')': amount -= curbuf->b_ind_unclosed2;
				  ਖਾਨਾ = MAXCOL;
				  ਰੋਕੋ;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		ਜੇ (ਖਾਨਾ == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		ਨਹੀਂਤਾਂ
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
		    ਜੇ (ਲੱਭੋ_ਮੇਲ_paren_after_brace(curbuf->b_ind_maxparen)
								      != ਨਿਰਾ)
			amount += curbuf->b_ind_unclosed2;
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			ਨਹੀਂਤਾਂ
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		ਜੇ (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	ਜੇ (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      ਨਹੀਂਤਾਂ
      {
	// We are inside braces, there is a { before this line at the position
	// stored in tryposBrace.
	// Make a copy of tryposBrace, it may point to pos_copy inside
	// find_start_brace(), which may be changed somewhere.
	tryposCopy = *tryposBrace;
	tryposBrace = &tryposCopy;
	trypos = tryposBrace;
	ourscope = trypos->lnum;
	ਸ਼ੁਰੂ = ml_get(ourscope);

	// Now figure out how indented the line is in general.
	// If the brace was at the start of the line, we use that;
	// otherwise, check out the indentation of the line as
	// a whole and then add the "imaginary indent" to that.
	look = skipwhite(ਸ਼ੁਰੂ);
	ਜੇ (*look == '{')
	{
	    getvcol(curwin, trypos, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
	    amount = ਖਾਨਾ;
	    ਜੇ (*ਸ਼ੁਰੂ == '{')
		ਸ਼ੁਰੂ_brace = BRACE_ਵਿੱਚ_COL0;
	    ਨਹੀਂਤਾਂ
		ਸ਼ੁਰੂ_brace = BRACE_AT_ਸ਼ੁਰੂ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    ਜੇ (ਲੱਭੋ_ਆਖਰੀ_paren(ਸ਼ੁਰੂ, '(', ')')
			&& (trypos = ਲੱਭੋ_ਮੇਲ_paren(curbuf->b_ind_maxparen))
								      != ਨਿਰਾ)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    ਜੇ ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    ਨਹੀਂਤਾਂ
		amount = skip_label(lnum, &l);

	    ਸ਼ੁਰੂ_brace = BRACE_AT_ਸਿਰਾ;
	}

	// For Javascript check if the line starts with "key:".
	ਜੇ (curbuf->b_ind_js)
	    js_cur_has_ਕੁੰਜੀ = cin_has_js_ਕੁੰਜੀ(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	ਜੇ (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	ਨਹੀਂਤਾਂ
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    ਜੇ (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    ਨਹੀਂਤਾਂ ਜੇ (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    ਜੇ (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		ਜੇ (ਲੱਭੋ_ਮੇਲ(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    ਜਾਓ theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    ਜੇ (ਸ਼ੁਰੂ_brace == BRACE_ਵਿੱਚ_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_ਖੱਬਾ_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ_brace == BRACE_AT_ਸ਼ੁਰੂ &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (ਸ਼ੁਰੂ_brace == BRACE_AT_ਸਿਰਾ)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    ਜੇ (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    ਨਹੀਂਤਾਂ ਜੇ (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		ਨਹੀਂਤਾਂ
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    ਜੇ (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    ਜੇ (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (curbuf->b_ind_case_break && cin_isbreak(theline))
		    // break; ...
		    lookfor_break = TRUE;

		lookfor = LOOKFOR_INITIAL;
		// b_ind_level from start of block
		amount += curbuf->b_ind_level;
	    }
	    scope_amount = amount;
	    whilelevel = 0;

	    // Search backwards.  If we find something we recognize, line up
	    // with that.
	    //
	    // If we're looking at an open brace, indent
	    // the usual amount relative to the conditional
	    // that opens the block.
	    curwin->w_cursor = cur_curpos;
	    ਲਈ (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.ਖਾਨਾ = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		ਜੇ (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    ਜੇ (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ਜੇ (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    ਜੇ (cont_amount > 0)
				amount = cont_amount;
			    ਨਹੀਂਤਾਂ ਜੇ (!curbuf->b_ind_js)
				amount += ind_continuation;
			    ਰੋਕੋ;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(ਨਿਰਾ);
			ਜੇ (trypos != ਨਿਰਾ)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.ਖਾਨਾ = 0;
			    ਜਾਰੀ;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			ਜੇ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    ਜਾਰੀ;

			ਜੇ (cin_nocode(l))
			    ਜਾਰੀ;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			ਜੇ (ਸ਼ੁਰੂ_brace != BRACE_ਵਿੱਚ_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    ਜੇ (terminated == ',')
				ਰੋਕੋ;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    ਜੇ (terminated != ';' && cin_isinit())
				ਰੋਕੋ;

			    // nothing useful found
			    ਜੇ (terminated == 0 || terminated == '{')
				ਜਾਰੀ;
			}

			ਜੇ (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = ਨਿਰਾ;
			    ਜੇ (ਲੱਭੋ_ਆਖਰੀ_paren(l, '(', ')'))
				trypos = ਲੱਭੋ_ਮੇਲ_paren(
						      curbuf->b_ind_maxparen);

			    ਜੇ (trypos == ਨਿਰਾ && ਲੱਭੋ_ਆਖਰੀ_paren(l, '{', '}'))
				trypos = ਲੱਭੋ_ਸ਼ੁਰੂ_brace();

			    ਜੇ (trypos != ਨਿਰਾ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.ਖਾਨਾ = 0;
				ਜਾਰੀ;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			ਜੇ (cont_amount > 0)
			    amount = cont_amount;
			ਨਹੀਂਤਾਂ
			    amount += ind_continuation;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (lookfor == LOOKFOR_UNTERM)
		    {
			ਜੇ (cont_amount > 0)
			    amount = cont_amount;
			ਨਹੀਂਤਾਂ
			    amount += ind_continuation;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (lookfor != LOOKFOR_ਪਦ
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    ਜੇ (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_ਮੰਜ਼ਿਲ_amount = curbuf->b_ind_open_extra;
			    }
			}

			ਜੇ (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    ਜੇ (curwin->w_cursor.lnum == ourscope)
				ਜਾਰੀ;

			    ਜੇ (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - ਲੱਭੋ_NAMESPACE_LIM)
				ਰੋਕੋ;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(ਨਿਰਾ);
			    ਜੇ (trypos != ਨਿਰਾ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.ਖਾਨਾ = 0;
				ਜਾਰੀ;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    ਜੇ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				ਜਾਰੀ;

			    // Finally the actual check for "namespace".
			    ਜੇ (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_ਮੰਜ਼ਿਲ_amount;
				ਰੋਕੋ;
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_ਮੰਜ਼ਿਲ_amount;
				ਰੋਕੋ;
			    }

			    ਜੇ (cin_nocode(l))
				ਜਾਰੀ;
			}
		    }
		    ਰੋਕੋ;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		ਜੇ ((trypos = ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(&raw_string_ਸ਼ੁਰੂ)) != ਨਿਰਾ)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.ਖਾਨਾ = 0;
		    ਜਾਰੀ;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		ਜੇ (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    ਜੇ (lookfor == LOOKFOR_CPP_BASECLASS)
			ਰੋਕੋ;

		    // When looking for a "do" we are not interested in
		    // labels.
		    ਜੇ (whilelevel > 0)
			ਜਾਰੀ;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    ਜੇ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ਜੇ (cont_amount > 0)
			    amount = cont_amount;
			ਨਹੀਂਤਾਂ
			    amount += ind_continuation;
			ਰੋਕੋ;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    ਜੇ (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			ਜੇ ((trypos = ਲੱਭੋ_ਸ਼ੁਰੂ_brace()) == ਨਿਰਾ
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    ਰੋਕੋ;
			}
			ਜਾਰੀ;
		    }

		    n = get_indent_nolabel(curwin->w_cursor.lnum);  // XXX

		    //	 case xx: if (cond)	    <- line up with this if
		    //		      y = y + 1;
		    // ->	  s = 99;
		    //
		    //	 case xx:
		    //	     if (cond)		<- line up with this line
		    //		 y = y + 1;
		    // ->    s = 99;
		    ਜੇ (lookfor == LOOKFOR_ਪਦ)
		    {
			ਜੇ (n)
			    amount = n;

			ਜੇ (!lookfor_break)
			    ਰੋਕੋ;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    ਜੇ (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			ਜੇ (l != ਨਿਰਾ && cin_is_cinword(l))
			{
			    ਜੇ (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    ਨਹੀਂਤਾਂ
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			ਰੋਕੋ;
		    }

		    // Try to get the indent of a statement before the switch
		    // label.  If nothing is found, line up relative to the
		    // switch label.
		    //	    break;		<- may line up with this line
		    //	 case xx:
		    // ->   y = 1;
		    scope_amount = get_indent() + (iscase    // XXX
					? curbuf->b_ind_case_ਕੋਡ
					: curbuf->b_ind_scopedecl_ਕੋਡ);
		    lookfor = curbuf->b_ind_case_break
					      ? LOOKFOR_NOBREAK : LOOKFOR_ANY;
		    ਜਾਰੀ;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		ਜੇ (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    ਜੇ (ਲੱਭੋ_ਆਖਰੀ_paren(l, '{', '}')
				     && (trypos = ਲੱਭੋ_ਸ਼ੁਰੂ_brace()) != ਨਿਰਾ)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.ਖਾਨਾ = 0;
		    }
		    ਜਾਰੀ;
		}

		// Ignore jump labels with nothing after them.
		ਜੇ (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    ਜੇ (l == ਨਿਰਾ || cin_nocode(l))
			ਜਾਰੀ;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		ਜੇ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    ਜਾਰੀ;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		ਜੇ (lookfor != LOOKFOR_ਪਦ && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		ਜੇ (n)
		{
		    ਜੇ (lookfor == LOOKFOR_UNTERM)
		    {
			ਜੇ (cont_amount > 0)
			    amount = cont_amount;
			ਨਹੀਂਤਾਂ
			    amount += ind_continuation;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			ਜਾਰੀ;
		    }
		    ਨਹੀਂਤਾਂ
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.ਖਾਨਾ);
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    ਜੇ (cin_isterminated(l, TRUE, FALSE))
			ਰੋਕੋ;
		    ਨਹੀਂਤਾਂ
			ਜਾਰੀ;
		}

		// What happens next depends on the line being terminated.
		// If terminated with a ',' only consider it terminating if
		// there is another unterminated statement behind, eg:
		//   123,
		//   sizeof
		//ਇੱਥੇ
		// Otherwise check whether it is an enumeration or structure
		// initialisation (not indented) or a variable declaration
		// (indented).
		terminated = cin_isterminated(l, FALSE, TRUE);

		ਜੇ (js_cur_has_ਕੁੰਜੀ)
		{
		    js_cur_has_ਕੁੰਜੀ = 0; // only check the first line
		    ਜੇ (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_ਕੁੰਜੀ;
		    }
		}
		ਜੇ (lookfor == LOOKFOR_JS_ਕੁੰਜੀ && cin_has_js_ਕੁੰਜੀ(l))
		{
		    amount = get_indent();
		    ਰੋਕੋ;
		}
		ਜੇ (lookfor == LOOKFOR_COMMA)
		{
		    ਜੇ (tryposBrace != ਨਿਰਾ && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			ਰੋਕੋ;
		    ਜੇ (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			ਰੋਕੋ;
		    ਨਹੀਂਤਾਂ
		    {
			amount = get_indent();
			ਜੇ (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    ਰੋਕੋ;
		    }
		}

		ਜੇ (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    ਜੇ (lookfor != LOOKFOR_ENUM_OR_INIT &&
			    (*skipwhite(l) == '[' || l[STRLEN(l) - 1] == '['))
			amount += ind_continuation;
		    // if we're in the middle of a paren thing,
		    // go back to the line that starts it so
		    // we can get the right prevailing indent
		    //	   if ( foo &&
		    //		    bar )

		    // Position the cursor over the rightmost paren, so that
		    // matching it will take us back to the start of the line.
		    // Ignore a match before the start of the block.
		    (ਖਾਲੀ)ਲੱਭੋ_ਆਖਰੀ_paren(l, '(', ')');
		    trypos = ਲੱਭੋ_ਮੇਲ_paren(corr_ind_maxparen(&cur_curpos));
		    ਜੇ (trypos != ਨਿਰਾ && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->ਖਾਨਾ < tryposBrace->ਖਾਨਾ)))
			trypos = ਨਿਰਾ;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    ਜੇ (trypos == ਨਿਰਾ && terminated == ',')
		    {
			ਜੇ (ਲੱਭੋ_ਆਖਰੀ_paren(l, '{', '}'))
			    trypos = ਲੱਭੋ_ਸ਼ੁਰੂ_brace();
			l = ml_get_curline();
		    }

		    ਜੇ (trypos != ਨਿਰਾ)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			ਜੇ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.ਖਾਨਾ = 0;
			    ਜਾਰੀ;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    ਜੇ (terminated == ',')
		    {
			ਜਦੋਂ (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    ਜੇ (*l == NUL || l[STRLEN(l) - 1] != '\\')
				ਰੋਕੋ;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.ਖਾਨਾ = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    ਜੇ (curbuf->b_ind_js)
			cur_amount = get_indent();
		    ਨਹੀਂਤਾਂ
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    ਜੇ (terminated != ',' && lookfor != LOOKFOR_ਪਦ
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			ਜੇ (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			ਜੇ (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    ਜਾਰੀ;
			}
			ਰੋਕੋ;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    ਜੇ (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			ਜੇ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    ਜੇ (cont_amount > 0)
				amount = cont_amount;
			    ਨਹੀਂਤਾਂ
				amount += ind_continuation;
			    ਰੋਕੋ;
			}

			// If this is just above the line we are indenting, we
			// are finished.
			//	    while (not)
			// ->		here;
			// Otherwise this indent can be used when the line
			// before this is terminated.
			//	yyy;
			//	if (stat)
			//	    while (not)
			//		xxx;
			// ->	here;
			amount = cur_amount;
			ਜੇ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			ਜੇ (lookfor != LOOKFOR_ਪਦ)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    ਰੋਕੋ;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			ਜੇ (cin_isdo(l))
			{
			    ਜੇ (whilelevel == 0)
				ਰੋਕੋ;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ਜੇ (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    ਜੇ (*l == '}')
				curwin->w_cursor.ਖਾਨਾ =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    ਜੇ ((trypos = ਲੱਭੋ_ਸ਼ੁਰੂ_brace()) == ਨਿਰਾ
				       || ਲੱਭੋ_ਮੇਲ(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ਰੋਕੋ;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    ਨਹੀਂਤਾਂ
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			ਜੇ (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    ਜੇ (terminated == ',')
				amount += ind_continuation;
			    ਰੋਕੋ;
			}

			ਜੇ (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    ਜੇ (terminated == ',')
			    {
				ਜੇ (curbuf->b_ind_cpp_baseclass == 0)
				    ਰੋਕੋ;

				lookfor = LOOKFOR_CPP_BASECLASS;
				ਜਾਰੀ;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    ਜੇ (amount > cur_amount)
				amount = cur_amount;
			}
			ਨਹੀਂਤਾਂ
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (ਪੂਰਨ)STRLEN(l);
			    ਜੇ (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				ਰੋਕੋ;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    ਜੇ (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				ਜੇ (curbuf->b_ind_js)
				{
				    // Search for a line ending in a comma
				    // and line up with the line below it
				    // (could be the current line).
				    // some = [
				    //     1,     <- line up here
				    //     2,
				    // some = [
				    //     3 +    <- line up here
				    //       4 *
				    //        5,
				    //     6,
				    ਜੇ (cin_iscomment(skipwhite(l)))
					ਰੋਕੋ;
				    lookfor = LOOKFOR_COMMA;
				    trypos = ਲੱਭੋ_ਮੇਲ_char('[',
						      curbuf->b_ind_maxparen);
				    ਜੇ (trypos != ਨਿਰਾ)
				    {
					ਜੇ (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    ਰੋਕੋ;
					}
					ourscope = trypos->lnum;
				    }
				}
				ਨਹੀਂਤਾਂ
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_ਪਹਿਲਾ_id_amount();
				}
			    }
			    ਨਹੀਂਤਾਂ
			    {
				ਜੇ (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				ਜੇ (lookfor != LOOKFOR_ਪਦ
						&& lookfor != LOOKFOR_JS_ਕੁੰਜੀ
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_ਸ਼ੁਰੂ != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		ਨਹੀਂਤਾਂ ਜੇ (cin_iswhileofdo_ਸਿਰਾ(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    ਜੇ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ਜੇ (cont_amount > 0)
			    amount = cont_amount;
			ਨਹੀਂਤਾਂ
			    amount += ind_continuation;
			ਰੋਕੋ;
		    }

		    ਜੇ (whilelevel == 0)
		    {
			lookfor = LOOKFOR_ਪਦ;
			amount = get_indent();	    // XXX
			ਜੇ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		ਨਹੀਂਤਾਂ
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    ਜੇ (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			ਜਾਰੀ;
		    }

		    // Handle "do {" line.
		    ਜੇ (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			ਜੇ (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    ਜਾਰੀ;
			}
		    }

		    // Found a terminated line above an unterminated line. Add
		    // the amount for a continuation line.
		    //	 x = 1;
		    //	 y = foo +
		    // ->	here;
		    // or
		    //	 int x = 1;
		    //	 int foo,
		    // ->	here;
		    ਜੇ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ਜੇ (cont_amount > 0)
			    amount = cont_amount;
			ਨਹੀਂਤਾਂ
			    amount += ind_continuation;
			ਰੋਕੋ;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    ਜੇ (lookfor == LOOKFOR_ਪਦ)
		    {
			ਜੇ (!lookfor_break && whilelevel == 0)
			    ਰੋਕੋ;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    ਨਹੀਂਤਾਂ
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
ਪਦ_again:
			l = ml_get_curline();
			ਜੇ (ਲੱਭੋ_ਆਖਰੀ_paren(l, '(', ')')
				&& (trypos = ਲੱਭੋ_ਮੇਲ_paren(
					   curbuf->b_ind_maxparen)) != ਨਿਰਾ)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    ਜੇ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.ਖਾਨਾ = 0;
				ਜਾਰੀ;
			    }
			}

			// When aligning with the case statement, don't align
			// with a statement after it.
			//  case 1: {   <-- don't use this { position
			//	stat;
			//  }
			//  case 2:
			//	stat;
			// }
			iscase = (curbuf->b_ind_keep_case_label
						     && cin_iscase(l, FALSE));

			// Get indent and pointer to text for current line,
			// ignoring any jump label.
			amount = skip_label(curwin->w_cursor.lnum, &l);

			ਜੇ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			ਜੇ (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_ਪਦ;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ਜੇ (lookfor == LOOKFOR_ਪਦ
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    ਜੇ ((trypos = ਲੱਭੋ_ਸ਼ੁਰੂ_brace()) == ਨਿਰਾ
				       || ਲੱਭੋ_ਮੇਲ(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ਰੋਕੋ;
			    ਜਾਰੀ;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			ਜੇ (ਲੱਭੋ_ਆਖਰੀ_paren(l, '{', '}') // XXX
				     && (trypos = ਲੱਭੋ_ਸ਼ੁਰੂ_brace()) != ਨਿਰਾ)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    ਜੇ (*l == '}' || !cin_iselse(l))
				ਜਾਓ ਪਦ_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.ਖਾਨਾ = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      ਜੇ (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      ਜੇ (curbuf->b_ind_jump_label > 0 && original_ਸਤਰ_islabel)
	  amount -= curbuf->b_ind_jump_label;

      ਜਾਓ theend;
    }

    // ok -- we're not inside any sort of structure at all!
    //
    // This means we're at the top level, and everything should
    // basically just match where the previous line is, except
    // for the lines immediately following a function declaration,
    // which are K&R-style parameters and need to be indented.
    //
    // if our line starts with an open brace, forget about any
    // prevailing indent and make sure it looks like the start
    // of a function

    ਜੇ (theline[0] == '{')
    {
	amount = curbuf->b_ind_ਪਹਿਲਾ_open;
	ਜਾਓ theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    ਜੇ (cur_curpos.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == ਨਿਰਾ
	    && vim_strchr(theline, '}') == ਨਿਰਾ
	    && !cin_ਸਿਰੇ_ਵਿੱਚ(theline, (char_u *)":")
	    && !cin_ਸਿਰੇ_ਵਿੱਚ(theline, (char_u *)",")
	    && cin_isfuncdecl(ਨਿਰਾ, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	ਜਾਓ theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    ਜਦੋਂ (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.ਖਾਨਾ = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	ਜੇ ((trypos = ind_ਲੱਭੋ_ਸ਼ੁਰੂ_CORS(ਨਿਰਾ)) != ਨਿਰਾ)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.ਖਾਨਾ = 0;
	    ਜਾਰੀ;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	ਜੇ (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	ਜੇ (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.ਖਾਨਾ);
	    ਰੋਕੋ;
	}

	// Skip preprocessor directives and blank lines.
	ਜੇ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    ਜਾਰੀ;

	ਜੇ (cin_nocode(l))
	    ਜਾਰੀ;

	// If the previous line ends in ',', use one level of
	// indentation:
	// int foo,
	//     bar;
	// do this before checking for '}' in case of eg.
	// enum foobar
	// {
	//   ...
	// } foo,
	//   bar;
	n = 0;
	ਜੇ (cin_ਸਿਰੇ_ਵਿੱਚ(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    ਜੇ (ਲੱਭੋ_ਆਖਰੀ_paren(l, '(', ')')
		    && (trypos = ਲੱਭੋ_ਮੇਲ_paren(
				     curbuf->b_ind_maxparen)) != ਨਿਰਾ)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    ਜਦੋਂ (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		ਜੇ (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    ਰੋਕੋ;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.ਖਾਨਾ = 0;
	    }

	    amount = get_indent();	    // XXX

	    ਜੇ (amount == 0)
		amount = cin_ਪਹਿਲਾ_id_amount();
	    ਜੇ (amount == 0)
		amount = ind_continuation;
	    ਰੋਕੋ;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	ਜੇ (cin_isfuncdecl(ਨਿਰਾ, cur_curpos.lnum, 0))  // XXX
	    ਰੋਕੋ;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	ਜੇ (*skipwhite(l) == '}')
	    ਰੋਕੋ;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	ਜੇ (cin_ਸਿਰੇ_ਵਿੱਚ(l, (char_u *)"};"))
	    ਰੋਕੋ;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	ਜੇ (cin_ਸਿਰੇ_ਵਿੱਚ(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    ਰੋਕੋ;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	ਜੇ (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    ਜਦੋਂ (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		ਜੇ (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    ਰੋਕੋ;
	    }
	    ਜੇ (curwin->w_cursor.lnum > 0
			    && cin_ਸਿਰੇ_ਵਿੱਚ(look, (char_u *)"}"))
		ਰੋਕੋ;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	ਜੇ (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    ਰੋਕੋ;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	ਜੇ (cin_ਸਿਰੇ_ਵਿੱਚ(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    ਜੇ (cin_ਸਿਰੇ_ਵਿੱਚ(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		ਰੋਕੋ;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	ਲੱਭੋ_ਆਖਰੀ_paren(l, '(', ')');

	ਜੇ ((trypos = ਲੱਭੋ_ਮੇਲ_paren(curbuf->b_ind_maxparen)) != ਨਿਰਾ)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	ਰੋਕੋ;
    }

    // add extra indent for a comment
    ਜੇ (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    ਜੇ (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	ਜੇ (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    ਜੇ (cur_amount > 0)
		amount = cur_amount;
	    ਨਹੀਂਤਾਂ ਜੇ (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    ਜੇ (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    ਵਾਪਸ amount;
}

/*
 * return TRUE if 'cinkeys' contains the key "keytyped",
 * when == '*':	    Only if key is preceded with '*'	(indent before insert)
 * when == '!':	    Only if key is preceded with '!'	(don't insert)
 * when == ' ':	    Only if key is not preceded with '*'(indent afterwards)
 *
 * "keytyped" can have a few special values:
 * KEY_OPEN_FORW
 * KEY_OPEN_BACK
 * KEY_COMPLETE	    just finished completion.
 *
 * If line_is_empty is TRUE accept keys with '0' before them.
 */
    ਪੂਰਨ
ਵਿੱਚ_cinkeys(
    ਪੂਰਨ		keytyped,
    ਪੂਰਨ		when,
    ਪੂਰਨ		ਸਤਰ_is_ਖਾਲੀ_ਥਾਵਾਂ)
{
    char_u	*look;
    ਪੂਰਨ		try_ਮੇਲ;
    ਪੂਰਨ		try_ਮੇਲ_ਸ਼ਬਦ;
    char_u	*p;
    char_u	*ਸਤਰ;
    ਪੂਰਨ		icase;
    ਪੂਰਨ		i;

    ਜੇ (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	ਵਾਪਸ FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    ਜਦੋਂ (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	ਚੋਣ (when)
	{
	    ਹਾਲਤ '*': try_ਮੇਲ = (*look == '*'); ਰੋਕੋ;
	    ਹਾਲਤ '!': try_ਮੇਲ = (*look == '!'); ਰੋਕੋ;
	    ਮੂਲ: try_ਮੇਲ = (*look != '*'); ਰੋਕੋ;
	}
	ਜੇ (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	ਜੇ (*look == '0')
	{
	    try_ਮੇਲ_ਸ਼ਬਦ = try_ਮੇਲ;
	    ਜੇ (!ਸਤਰ_is_ਖਾਲੀ_ਥਾਵਾਂ)
		try_ਮੇਲ = FALSE;
	    ++look;
	}
	ਨਹੀਂਤਾਂ
	    try_ਮੇਲ_ਸ਼ਬਦ = FALSE;

	// does it look like a control character?
	ਜੇ (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    ਜੇ (try_ਮੇਲ && keytyped == Ctrl_chr(look[1]))
		ਵਾਪਸ TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	ਨਹੀਂਤਾਂ ਜੇ (*look == 'o')
	{
	    ਜੇ (try_ਮੇਲ && keytyped == ਕੁੰਜੀ_OPEN_FORW)
		ਵਾਪਸ TRUE;
	    ++look;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*look == 'O')
	{
	    ਜੇ (try_ਮੇਲ && keytyped == ਕੁੰਜੀ_OPEN_ਪਿੱਠ)
		ਵਾਪਸ TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	ਨਹੀਂਤਾਂ ਜੇ (*look == 'e')
	{
	    ਜੇ (try_ਮੇਲ && keytyped == 'e' && curwin->w_cursor.ਖਾਨਾ >= 4)
	    {
		p = ml_get_curline();
		ਜੇ (skipwhite(p) == p + curwin->w_cursor.ਖਾਨਾ - 4 &&
			STRNCMP(p + curwin->w_cursor.ਖਾਨਾ - 4, "else", 4) == 0)
		    ਵਾਪਸ TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	ਨਹੀਂਤਾਂ ਜੇ (*look == ':')
	{
	    ਜੇ (try_ਮੇਲ && keytyped == ':')
	    {
		p = ml_get_curline();
		ਜੇ (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    ਵਾਪਸ TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		ਜੇ (curwin->w_cursor.ਖਾਨਾ > 2
			&& p[curwin->w_cursor.ਖਾਨਾ - 1] == ':'
			&& p[curwin->w_cursor.ਖਾਨਾ - 2] == ':')
		{
		    p[curwin->w_cursor.ਖਾਨਾ - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.ਖਾਨਾ - 1] = ':';
		    ਜੇ (i)
			ਵਾਪਸ TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	ਨਹੀਂਤਾਂ ਜੇ (*look == '<')
	{
	    ਜੇ (try_ਮੇਲ)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		ਜੇ (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != ਨਿਰਾ
						       && keytyped == look[1])
		    ਵਾਪਸ TRUE;

		ਜੇ (keytyped == get_special_ਕੁੰਜੀ_ਕੋਡ(look + 1))
		    ਵਾਪਸ TRUE;
	    }
	    ਜਦੋਂ (*look && *look != '>')
		look++;
	    ਜਦੋਂ (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	ਨਹੀਂਤਾਂ ਜੇ (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    ਜੇ (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    ਨਹੀਂਤਾਂ
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    ਜੇ (p == ਨਿਰਾ)
		p = look + STRLEN(look);
	    ਜੇ ((try_ਮੇਲ || try_ਮੇਲ_ਸ਼ਬਦ)
		    && curwin->w_cursor.ਖਾਨਾ >= (colnr_T)(p - look))
	    {
		ਪੂਰਨ		ਮੇਲ = FALSE;

		ਜੇ (keytyped == ਕੁੰਜੀ_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    ਸਤਰ = ml_get_curline();
		    ਜੇ (has_mbyte)
		    {
			char_u	*n;

			ਲਈ (s = ਸਤਰ + curwin->w_cursor.ਖਾਨਾ; s > ਸਤਰ; s = n)
			{
			    n = mb_prevptr(ਸਤਰ, s);
			    ਜੇ (!vim_iswordp(n))
				ਰੋਕੋ;
			}
		    }
		    ਨਹੀਂਤਾਂ
			ਲਈ (s = ਸਤਰ + curwin->w_cursor.ਖਾਨਾ; s > ਸਤਰ; --s)
			    ਜੇ (!vim_iswordc(s[-1]))
				ਰੋਕੋ;
		    ਜੇ (s + (p - look) <= ਸਤਰ + curwin->w_cursor.ਖਾਨਾ
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			ਮੇਲ = TRUE;
		}
		ਨਹੀਂਤਾਂ
		    // TODO: multi-byte
		    ਜੇ (keytyped == (ਪੂਰਨ)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((ਪੂਰਨ)p[-1])))
		{
		    ਸਤਰ = ml_get_cursor();
		    ਜੇ ((curwin->w_cursor.ਖਾਨਾ == (colnr_T)(p - look)
				|| !vim_iswordc(ਸਤਰ[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(ਸਤਰ - (p - look), look, p - look)
				: STRNCMP(ਸਤਰ - (p - look), look, p - look))
									 == 0)
			ਮੇਲ = TRUE;
		}
		ਜੇ (ਮੇਲ && try_ਮੇਲ_ਸ਼ਬਦ && !try_ਮੇਲ)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    ਜੇ (getwhitecols_curline() !=
				     (ਪੂਰਨ)(curwin->w_cursor.ਖਾਨਾ - (p - look)))
			ਮੇਲ = FALSE;
		}
		ਜੇ (ਮੇਲ)
		    ਵਾਪਸ TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (try_ਮੇਲ && *look == keytyped)
		ਵਾਪਸ TRUE;
	    ਜੇ (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_ਮੰਜ਼ਿਲ_option_part(look);
    }
    ਵਾਪਸ FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    ਖਾਲੀ
do_c_ਵਿਅੰਜਕ_indent(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (*curbuf->b_p_inde != NUL)
	fixthisline(get_ਵਿਅੰਜਕ_indent);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	fixthisline(get_c_indent);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    ਖਾਲੀ
f_cindent(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ਜੇ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_ਸੰਖਿਆ = get_c_indent();
	curwin->w_cursor = pos;
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = -1;
}
#ਜੇ_ਅੰਤ
