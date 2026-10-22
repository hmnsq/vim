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

#అన్తర్భావ "vim.h"

// values for the "lookfor" state
#నిర్వచించు LOOKFOR_INITIAL		0
#నిర్వచించు LOOKFOR_IF		1
#నిర్వచించు LOOKFOR_DO		2
#నిర్వచించు LOOKFOR_CASE		3
#నిర్వచించు LOOKFOR_ANY		4
#నిర్వచించు LOOKFOR_పదం		5
#నిర్వచించు LOOKFOR_UNTERM		6
#నిర్వచించు LOOKFOR_SCOPEDECL	7
#నిర్వచించు LOOKFOR_NOBREAK		8
#నిర్వచించు LOOKFOR_CPP_BASECLASS	9
#నిర్వచించు LOOKFOR_ENUM_OR_INIT	10
#నిర్వచించు LOOKFOR_JS_కీలకం		11
#నిర్వచించు LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    పూర్ణాంకం
cin_is_cinword(char_u *పంక్తి)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    పూర్ణాంకం		cinw_len;
    పూర్ణాంకం		retval = FALSE;
    పూర్ణాంకం		len;

    cinw_len = (పూర్ణాంకం)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    ఒకవేళ (cinw_buf == ఖాళీ)
	తిరిగి FALSE;

    పంక్తి = skipwhite(పంక్తి);
    కొరకు (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	ఒకవేళ (STRNCMP(పంక్తి, cinw_buf, len) == 0
		&& (!vim_iswordc(పంక్తి[len]) || !vim_iswordc(పంక్తి[len - 1])))
	{
	    retval = TRUE;
	    ఆపు;
	}
    }
    vim_free(cinw_buf);
    తిరిగి retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    స్థిర char_u *
skip_string(char_u *p)
{
    పూర్ణాంకం	    i;

    // We loop, because strings may be concatenated: "date""time".
    కొరకు ( ; ; ++p)
    {
	ఒకవేళ (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    ఒకవేళ (p[1] == NUL)		    // ' at end of line
		ఆపు;
	    i = 2;
	    ఒకవేళ (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		అయితే (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    ఒకవేళ (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		కొనసాగు;
	    }
	}
	లేకపోతే ఒకవేళ (p[0] == '"')		    // start of string
	{
	    కొరకు (++p; p[0]; ++p)
	    {
		ఒకవేళ (p[0] == '\\' && p[1] != NUL)
		    ++p;
		లేకపోతే ఒకవేళ (p[0] == '"')	    // end of string
		    ఆపు;
	    }
	    ఒకవేళ (p[0] == '"')
		కొనసాగు; // continue for another string
	}
	లేకపోతే ఒకవేళ (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    ఒకవేళ (paren != ఖాళీ)
	    {
		size_t delim_len = paren - delim;

		కొరకు (p += 3; *p; ++p)
		    ఒకవేళ (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			ఆపు;
		    }
		ఒకవేళ (p[0] == '"')
		    కొనసాగు; // continue for another string
	    }
	}
	ఆపు;				    // no string found
    }
    ఒకవేళ (!*p)
	--p;				    // backup from NUL
    తిరిగి p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    పూర్ణాంకం
is_pos_లోపల_string(char_u *పంక్తి, colnr_T నిలువు)
{
    char_u *p;

    కొరకు (p = పంక్తి; *p && (colnr_T)(p - పంక్తి) < నిలువు; ++p)
	p = skip_string(p);
    తిరిగి !((colnr_T)(p - పంక్తి) <= నిలువు);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    పూర్ణాంకం
check_linecomment(char_u *పంక్తి)
{
    char_u  *p;

    p = పంక్తి;
    // skip Lispish one-line comments
    ఒకవేళ (curbuf->b_p_lisp)
    {
	ఒకవేళ (vim_strchr(p, ';') != ఖాళీ) // there may be comments
	{
	    పూర్ణాంకం లోపల_str = FALSE;	// inside of string

	    p = పంక్తి;		// scan from start
	    అయితే ((p = vim_strpbrk(p, (char_u *)"\";")) != ఖాళీ)
	    {
		ఒకవేళ (*p == '"')
		{
		    ఒకవేళ (లోపల_str)
		    {
			ఒకవేళ (*(p - 1) != '\\') // skip escaped quote
			    లోపల_str = FALSE;
		    }
		    లేకపోతే ఒకవేళ (p == పంక్తి || ((p - పంక్తి) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			లోపల_str = TRUE;
		}
		లేకపోతే ఒకవేళ (!లోపల_str && ((p - పంక్తి) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_లోపల_string(పంక్తి, (colnr_T)(p - పంక్తి)))
		    ఆపు;	// found!
		++p;
	    }
	}
	లేకపోతే
	    p = ఖాళీ;
    }
    లేకపోతే
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	కొరకు ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    ఒకవేళ (*p == NUL)
		ఆపు;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    ఒకవేళ (p[0] == '/' && p[1] == '/'
				 && (p == పంక్తి || p[-1] != '*' || p[2] != '*'))
		ఆపు;
	}
	ఒకవేళ (*p == NUL)
	    p = ఖాళీ;
    }

    ఒకవేళ (p == ఖాళీ)
	తిరిగి MAXCOL;
    తిరిగి (పూర్ణాంకం)(p - పంక్తి);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    స్థిర pos_T *
ind_కనుగొను_ప్రారంభం_comment(శూన్య)	    // XXX
{
    తిరిగి కనుగొను_ప్రారంభం_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
కనుగొను_ప్రారంభం_comment(పూర్ణాంకం ind_maxcomment)	// XXX
{
    pos_T	*pos;
    పూర్ణాంకం		cur_maxcomment = ind_maxcomment;

    కొరకు (;;)
    {
	pos = findmatchlimit(ఖాళీ, '*', FM_BACKWARD, cur_maxcomment);
	ఒకవేళ (pos == ఖాళీ)
	    ఆపు;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ఒకవేళ (!is_pos_లోపల_string(ml_get(pos->lnum), pos->నిలువు))
	    ఆపు;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ఒకవేళ (cur_maxcomment <= 0)
	{
	    pos = ఖాళీ;
	    ఆపు;
	}
    }
    తిరిగి pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    స్థిర pos_T *
కనుగొను_ప్రారంభం_rawstring(పూర్ణాంకం ind_maxcomment)	// XXX
{
    pos_T	*pos;
    పూర్ణాంకం		cur_maxcomment = ind_maxcomment;

    కొరకు (;;)
    {
	pos = findmatchlimit(ఖాళీ, 'R', FM_BACKWARD, cur_maxcomment);
	ఒకవేళ (pos == ఖాళీ)
	    ఆపు;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ఒకవేళ (!is_pos_లోపల_string(ml_get(pos->lnum), pos->నిలువు))
	    ఆపు;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ఒకవేళ (cur_maxcomment <= 0)
	{
	    pos = ఖాళీ;
	    ఆపు;
	}
    }
    తిరిగి pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    స్థిర pos_T *
ind_కనుగొను_ప్రారంభం_CORS(linenr_T *is_raw)	    // XXX
{
    స్థిర pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = కనుగొను_ప్రారంభం_comment(curbuf->b_ind_maxcomment);
    ఒకవేళ (comment_pos != ఖాళీ)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = కనుగొను_ప్రారంభం_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    ఒకవేళ (comment_pos == ఖాళీ || (rs_pos != ఖాళీ
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	ఒకవేళ (is_raw != ఖాళీ && rs_pos != ఖాళీ)
	    *is_raw = rs_pos->lnum;
	తిరిగి rs_pos;
    }
    తిరిగి comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    పూర్ణాంకం
cindent_on(శూన్య)
{
    తిరిగి (!p_paste && (curbuf->b_p_cin
#నిర్వచితమైతే FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#అయితే_ముగింపు
		    ));
}

// Find result cache for cpp_baseclass
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం	    దొరికింది;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    స్థిర char_u *
cin_skipcomment(char_u *s)
{
    అయితే (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	ఒకవేళ (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    ఆపు;
	}
	ఒకవేళ (*s != '/')
	    ఆపు;
	++s;
	ఒకవేళ (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    ఆపు;
	}
	ఒకవేళ (*s != '*')
	    ఆపు;
	కొరకు (++s; *s; ++s)	// skip slash-star comment
	    ఒకవేళ (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		ఆపు;
	    }
    }
    తిరిగి s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    స్థిర పూర్ణాంకం
cin_nocode(char_u *s)
{
    తిరిగి *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    స్థిర పూర్ణాంకం
cin_iscomment(char_u *p)
{
    తిరిగి (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    స్థిర పూర్ణాంకం
cin_islinecomment(char_u *p)
{
    తిరిగి (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    స్థిర pos_T *
కనుగొను_పంక్తి_comment(శూన్య) // XXX
{
    స్థిర pos_T pos;
    char_u	 *పంక్తి;
    char_u	 *p;

    pos = curwin->w_cursor;
    అయితే (--pos.lnum > 0)
    {
	పంక్తి = ml_get(pos.lnum);
	p = skipwhite(పంక్తి);
	ఒకవేళ (cin_islinecomment(p))
	{
	    pos.నిలువు = (పూర్ణాంకం)(p - పంక్తి);
	    తిరిగి &pos;
	}
	ఒకవేళ (*p != NUL)
	    ఆపు;
    }
    తిరిగి ఖాళీ;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    స్థిర పూర్ణాంకం
cin_has_js_కీలకం(char_u *పాఠ్యం)
{
    char_u *s = skipwhite(పాఠ్యం);
    పూర్ణాంకం	    quote = -1;

    ఒకవేళ (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    ఒకవేళ (!vim_isIDc(*s))	    // need at least one ID character
	తిరిగి FALSE;

    అయితే (vim_isIDc(*s))
	++s;
    ఒకవేళ (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    తిరిగి (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    స్థిర పూర్ణాంకం
cin_islabel_skip(char_u **s)
{
    ఒకవేళ (!vim_isIDc(**s))	    // need at least one ID character
	తిరిగి FALSE;

    అయితే (vim_isIDc(**s))
    {
	ఒకవేళ (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	లేకపోతే
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    తిరిగి (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    స్థిర పూర్ణాంకం
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    పూర్ణాంకం	    దొరికింది = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    ఒకవేళ (cinsd_buf == ఖాళీ)
	తిరిగి FALSE;

    కొరకు (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (పూర్ణాంకం)cinsd_len, ",");
	ఒకవేళ (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    ఒకవేళ (*skip == ':' && skip[1] != ':')
	    {
		దొరికింది = TRUE;
		ఆపు;
	    }
	}
    }

    vim_free(cinsd_buf);
    తిరిగి దొరికింది;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    స్థిర పూర్ణాంకం
cin_ispreproc(char_u *s)
{
    ఒకవేళ (*skipwhite(s) == '#')
	తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    స్థిర పూర్ణాంకం
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, పూర్ణాంకం *amount)
{
    char_u	*పంక్తి = *pp;
    linenr_T	lnum = *lnump;
    పూర్ణాంకం		retval = FALSE;
    పూర్ణాంకం		candidate_amount = *amount;

    ఒకవేళ (*పంక్తి != NUL && పంక్తి[STRLEN(పంక్తి) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    కొరకు (;;)
    {
	ఒకవేళ (cin_ispreproc(పంక్తి))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    ఆపు;
	}
	ఒకవేళ (lnum == 1)
	    ఆపు;
	పంక్తి = ml_get(--lnum);
	ఒకవేళ (*పంక్తి == NUL || పంక్తి[STRLEN(పంక్తి) - 1] != '\\')
	    ఆపు;
    }

    ఒకవేళ (lnum != *lnump)
	*pp = ml_get(*lnump);
    ఒకవేళ (retval)
	*amount = candidate_amount;
    తిరిగి retval;
}

    స్థిర పూర్ణాంకం
cin_iselse(
    char_u  *p)
{
    ఒకవేళ (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    తిరిగి (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    స్థిర పూర్ణాంకం
cin_isterminated(
    char_u	*s,
    పూర్ణాంకం		incl_open,	// include '{' at the end as terminator
    పూర్ణాంకం		incl_comma)	// recognize a trailing comma
{
    char_u	దొరికింది_ప్రారంభం = 0;
    గుర్తులేని	n_open = 0;
    పూర్ణాంకం		is_else = FALSE;

    s = cin_skipcomment(s);

    ఒకవేళ (*s == '{' || (*s == '}' && !cin_iselse(s)))
	దొరికింది_ప్రారంభం = *s;

    ఒకవేళ (!దొరికింది_ప్రారంభం)
	is_else = cin_iselse(s);

    అయితే (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	ఒకవేళ (*s == '}' && n_open > 0)
	    --n_open;
	ఒకవేళ ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    తిరిగి *s;
	లేకపోతే ఒకవేళ (*s == '{')
	{
	    ఒకవేళ (incl_open && cin_nocode(s + 1))
		తిరిగి *s;
	    లేకపోతే
		++n_open;
	}

	ఒకవేళ (*s)
	    s++;
    }
    తిరిగి దొరికింది_ప్రారంభం;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    స్థిర పూర్ణాంకం
cin_starts_with(char_u *s, అక్షరం *మాట)
{
    పూర్ణాంకం l = (పూర్ణాంకం)STRLEN(మాట);

    తిరిగి (STRNCMP(s, మాట, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    స్థిర పూర్ణాంకం
cin_isdefault(char_u *s)
{
    తిరిగి (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    స్థిర పూర్ణాంకం
cin_iscase(
    char_u *s,
    పూర్ణాంకం strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    ఒకవేళ (cin_starts_with(s, "case"))
    {
	కొరకు (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    ఒకవేళ (*s == NUL)
		ఆపు;
	    ఒకవేళ (*s == ':')
	    {
		ఒకవేళ (s[1] == ':')	// skip over "::" for C++
		    ++s;
		లేకపోతే
		    తిరిగి TRUE;
	    }
	    ఒకవేళ (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    లేకపోతే ఒకవేళ (*s == '/' && (s[1] == '*' || s[1] == '/'))
		తిరిగి FALSE;		// stop at comment
	    లేకపోతే ఒకవేళ (*s == '"')
	    {
		// JS etc.
		ఒకవేళ (strict)
		    తిరిగి FALSE;		// stop at string
		లేకపోతే
		    తిరిగి TRUE;
	    }
	}
	తిరిగి FALSE;
    }

    ఒకవేళ (cin_isdefault(s))
	తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    స్థిర పూర్ణాంకం
cin_islabel(శూన్య)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    ఒకవేళ (cin_isdefault(s))
	తిరిగి FALSE;
    ఒకవేళ (cin_isscopedecl(s))
	తిరిగి FALSE;

    ఒకవేళ (!cin_islabel_skip(&s))
	తిరిగి FALSE;

    ఒకవేళ (ind_కనుగొను_ప్రారంభం_CORS(ఖాళీ))
	తిరిగి FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*పంక్తి;

    cursor_save = curwin->w_cursor;
    అయితే (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.నిలువు = 0;
	ఒకవేళ ((trypos = ind_కనుగొను_ప్రారంభం_CORS(ఖాళీ)) != ఖాళీ) // XXX
	    curwin->w_cursor = *trypos;

	పంక్తి = ml_get_curline();
	ఒకవేళ (cin_ispreproc(పంక్తి))	// ignore #defines, #if, etc.
	    కొనసాగు;
	ఒకవేళ (*(పంక్తి = cin_skipcomment(పంక్తి)) == NUL)
	    కొనసాగు;

	curwin->w_cursor = cursor_save;
	ఒకవేళ (cin_isterminated(పంక్తి, TRUE, FALSE)
		|| cin_isscopedecl(పంక్తి)
		|| cin_iscase(పంక్తి, TRUE)
		|| (cin_islabel_skip(&పంక్తి) && cin_nocode(పంక్తి)))
	    తిరిగి TRUE;
	తిరిగి FALSE;
    }
    curwin->w_cursor = cursor_save;
    తిరిగి TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    స్థిర పూర్ణాంకం
cin_చివర్లు_లోపల(char_u *s, char_u *కనుగొను)
{
    char_u	*p = s;
    char_u	*r;
    పూర్ణాంకం		len = (పూర్ణాంకం)STRLEN(కనుగొను);

    అయితే (*p != NUL)
    {
	p = cin_skipcomment(p);
	ఒకవేళ (STRNCMP(p, కనుగొను, len) == 0)
	{
	    r = skipwhite(p + len);
	    ఒకవేళ (cin_nocode(r))
		తిరిగి TRUE;
	}
	ఒకవేళ (*p != NUL)
	    ++p;
    }
    తిరిగి FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    స్థిర char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = ఖాళీ, *p = s;
    చేయి
    {
	r = p;
	p = cin_skipcomment(p);
	ఒకవేళ (*p)
	    p = skip_string(p);
    } అయితే (p != r);
    తిరిగి p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    స్థిర పూర్ణాంకం
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = ఖాళీ;

    అయితే (*p)
    {
	ఒకవేళ (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	లేకపోతే ఒకవేళ (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	లేకపోతే
	    p = cin_skip_comment_and_string(p + 1);
    }
    ఒకవేళ (!r)
	తిరిగి FALSE;
    p = r; // p points now after '=' or "return"

    ఒకవేళ (cin_nocode(p))
	తిరిగి TRUE;

    ఒకవేళ (*p == '&')
	p = cin_skipcomment(p + 1);

    ఒకవేళ (*p == '(') // skip a typecast
    {
	పూర్ణాంకం open_లెక్క = 1;
	చేయి
	{
	    p = cin_skip_comment_and_string(p + 1);
	    ఒకవేళ (cin_nocode(p))
		తిరిగి TRUE;
	    open_లెక్క += (*p == '(') - (*p == ')');
	} అయితే (open_లెక్క);
	p = cin_skipcomment(p + 1);
	ఒకవేళ (cin_nocode(p))
	    తిరిగి TRUE;
    }

    అయితే (*p == '{')
	p = cin_skipcomment(p + 1);
    తిరిగి cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    స్థిర పూర్ణాంకం
cin_isinit(శూన్య)
{
    char_u	*s;
    స్థిర అక్షరం *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    ఒకవేళ (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    కొరకు (;;)
    {
	పూర్ణాంకం i, l;

	కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(skip); ++i)
	{
	    l = (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(skip[i]);
	    ఒకవేళ (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		ఆపు;
	    }
	}
	ఒకవేళ (l != 0)
	    ఆపు;
    }

    ఒకవేళ (cin_starts_with(s, "enum"))
	తిరిగి TRUE;

    తిరిగి cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#నిర్వచించు కనుగొను_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    స్థిర పూర్ణాంకం
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    పూర్ణాంకం		has_name = FALSE;
    పూర్ణాంకం		has_name_ప్రారంభం = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    అయితే ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    ఒకవేళ (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	అయితే (*p != NUL)
	{
	    ఒకవేళ (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    లేకపోతే ఒకవేళ (*p == '{')
	    {
		ఆపు;
	    }
	    లేకపోతే ఒకవేళ (vim_iswordc(*p))
	    {
		has_name_ప్రారంభం = TRUE;
		ఒకవేళ (has_name)
		    తిరిగి FALSE; // word character after skipping past name
		++p;
	    }
	    లేకపోతే ఒకవేళ (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		ఒకవేళ (!has_name_ప్రారంభం || has_name)
		    తిరిగి FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    లేకపోతే
	    {
		తిరిగి FALSE;
	    }
	}
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    స్థిర పూర్ణాంకం
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    పూర్ణాంకం		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    ఒకవేళ (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	అయితే (*p != NUL)
	{
	    ఒకవేళ (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    లేకపోతే ఒకవేళ (*p == '{')
	    {
		ఆపు;
	    }
	    లేకపోతే ఒకవేళ (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		ఒకవేళ (has_string_literal)
		    తిరిగి FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    లేకపోతే ఒకవేళ (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		ఒకవేళ (has_string_literal)
		    తిరిగి FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    లేకపోతే
	    {
		తిరిగి FALSE;
	    }
	}
	తిరిగి has_string_literal ? TRUE : FALSE;
    }
    తిరిగి FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    స్థిర char_u *
after_label(char_u *l)
{
    కొరకు ( ; *l; ++l)
    {
	ఒకవేళ (*l == ':')
	{
	    ఒకవేళ (l[1] == ':')	    // skip over "::" for C++
		++l;
	    లేకపోతే ఒకవేళ (!cin_iscase(l + 1, FALSE))
		ఆపు;
	}
	లేకపోతే ఒకవేళ (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    ఒకవేళ (*l == NUL)
	తిరిగి ఖాళీ;
    l = cin_skipcomment(l + 1);
    ఒకవేళ (*l == NUL)
	తిరిగి ఖాళీ;
    తిరిగి l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    స్థిర పూర్ణాంకం
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	నిలువు;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి 0;

    fp.నిలువు = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &నిలువు, ఖాళీ, ఖాళీ, 0);
    తిరిగి (పూర్ణాంకం)నిలువు;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    స్థిర పూర్ణాంకం
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    పూర్ణాంకం		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    ఒకవేళ (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	ఒకవేళ (l == ఖాళీ)		// just in case
	    l = ml_get_curline();
    }
    లేకపోతే
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    తిరిగి amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    స్థిర పూర్ణాంకం
cin_మొదటి_id_amount(శూన్య)
{
    char_u	*పంక్తి, *p, *s;
    పూర్ణాంకం		len;
    pos_T	fp;
    colnr_T	నిలువు;

    పంక్తి = ml_get_curline();
    p = skipwhite(పంక్తి);
    len = (పూర్ణాంకం)(skiptowhite(p) - p);
    ఒకవేళ (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (పూర్ణాంకం)(skiptowhite(p) - p);
    }
    ఒకవేళ (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    లేకపోతే ఒకవేళ (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    లేకపోతే ఒకవేళ ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	ఒకవేళ ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    కొరకు (len = 0; vim_isIDc(p[len]); ++len)
	;
    ఒకవేళ (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	తిరిగి 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.నిలువు = (colnr_T)(p - పంక్తి);
    getvcol(curwin, &fp, &నిలువు, ఖాళీ, ఖాళీ, 0);
    తిరిగి (పూర్ణాంకం)నిలువు;
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
    స్థిర పూర్ణాంకం
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*పంక్తి;
    char_u	*s;
    colnr_T	నిలువు;
    pos_T	fp;

    ఒకవేళ (lnum > 1)
    {
	పంక్తి = ml_get(lnum - 1);
	ఒకవేళ (*పంక్తి != NUL && పంక్తి[STRLEN(పంక్తి) - 1] == '\\')
	    తిరిగి -1;
    }

    పంక్తి = s = ml_get(lnum);
    అయితే (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == ఖాళీ)
    {
	ఒకవేళ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	లేకపోతే
	    ++s;
    }
    ఒకవేళ (*s != '=')
	తిరిగి 0;

    s = skipwhite(s + 1);
    ఒకవేళ (cin_nocode(s))
	తిరిగి 0;

    ఒకవేళ (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.నిలువు = (colnr_T)(s - పంక్తి);
    getvcol(curwin, &fp, &నిలువు, ఖాళీ, ఖాళీ, 0);
    తిరిగి (పూర్ణాంకం)నిలువు;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    స్థిర పూర్ణాంకం
cin_skip2pos(pos_T *trypos)
{
    char_u	*పంక్తి;
    char_u	*p;
    char_u	*new_p;

    p = పంక్తి = ml_get(trypos->lnum);
    అయితే (*p && (colnr_T)(p - పంక్తి) < trypos->నిలువు)
    {
	ఒకవేళ (cin_iscomment(p))
	    p = cin_skipcomment(p);
	లేకపోతే
	{
	    new_p = skip_string(p);
	    ఒకవేళ (new_p == p)
		++p;
	    లేకపోతే
		p = new_p;
	}
    }
    తిరిగి (పూర్ణాంకం)(p - పంక్తి);
}

    స్థిర pos_T *
కనుగొను_సరిపోలిక_char(పూర్ణాంకం c, పూర్ణాంకం ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    స్థిర pos_T pos_copy;
    పూర్ణాంకం		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    ఒకవేళ ((trypos = findmatchlimit(ఖాళీ, c, FM_SKIPCOMM, ind_maxp_wk)) != ఖాళీ)
    {
	// check if the ( is in a // comment
	ఒకవేళ ((colnr_T)cin_skip2pos(trypos) > trypos->నిలువు)
	{
	    ind_maxp_wk = ind_maxparen - (పూర్ణాంకం)(cursor_save.lnum - trypos->lnum);
	    ఒకవేళ (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.నిలువు = 0;	// XXX
		వెళ్ళు retry;
	    }
	    trypos = ఖాళీ;
	}
	లేకపోతే
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    ఒకవేళ ((trypos_wk = ind_కనుగొను_ప్రారంభం_CORS(ఖాళీ)) != ఖాళీ) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (పూర్ణాంకం)(cursor_save.lnum
			- trypos_wk->lnum);
		ఒకవేళ (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    వెళ్ళు retry;
		}
		trypos = ఖాళీ;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    తిరిగి trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    స్థిర pos_T *
కనుగొను_సరిపోలిక_paren(పూర్ణాంకం ind_maxparen)	// XXX
{
    తిరిగి కనుగొను_సరిపోలిక_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    స్థిర పూర్ణాంకం
కనుగొను_చివరి_paren(char_u *l, పూర్ణాంకం ప్రారంభం, పూర్ణాంకం చివర)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		retval = FALSE;
    పూర్ణాంకం		open_లెక్క = 0;

    curwin->w_cursor.నిలువు = 0;		    // default is start of line

    కొరకు (i = 0; l[i] != NUL; i++)
    {
	i = (పూర్ణాంకం)(cin_skipcomment(l + i) - l); // ignore parens in comments
	ఒకవేళ (l[i] == NUL)
	    ఆపు;
	i = (పూర్ణాంకం)(skip_string(l + i) - l);    // ignore parens in quotes
	ఒకవేళ (l[i] == ప్రారంభం)
	    ++open_లెక్క;
	లేకపోతే ఒకవేళ (l[i] == చివర)
	{
	    ఒకవేళ (open_లెక్క > 0)
		--open_లెక్క;
	    లేకపోతే
	    {
		curwin->w_cursor.నిలువు = i;
		retval = TRUE;
	    }
	}
    }
    తిరిగి retval;
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
    స్థిర పూర్ణాంకం
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	మొదటి_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = మొదటి_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    పూర్ణాంకం		retval = FALSE;
    pos_T	*trypos;
    పూర్ణాంకం		just_started = TRUE;

    ఒకవేళ (sp == ఖాళీ)
	s = ml_get(lnum);
    లేకపోతే
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    ఒకవేళ (కనుగొను_చివరి_paren(s, '(', ')')
	&& (trypos = కనుగొను_సరిపోలిక_paren(curbuf->b_ind_maxparen)) != ఖాళీ)
    {
	lnum = trypos->lnum;
	ఒకవేళ (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    తిరిగి FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    ఒకవేళ (cin_ispreproc(s))
	తిరిగి FALSE;

    అయితే (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	ఒకవేళ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	లేకపోతే ఒకవేళ (*s == ':')
	{
	    ఒకవేళ (*(s + 1) == ':')
		s += 2;
	    లేకపోతే
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		తిరిగి FALSE;
	}
	లేకపోతే
	    ++s;
    }
    ఒకవేళ (*s != '(')
	తిరిగి FALSE;		// ';', ' or "  before any () or no '('

    అయితే (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	ఒకవేళ (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = మొదటి_lnum - 1;
	    s = ml_get(lnum);
	    ఒకవేళ (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    వెళ్ళు పూర్తి;
	}
	ఒకవేళ ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    పూర్ణాంకం comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    కొరకు (;;)
	    {
		ఒకవేళ (lnum >= curbuf->b_ml.ml_పంక్తి_లెక్క)
		    ఆపు;
		s = ml_get(++lnum);
		ఒకవేళ (!cin_ispreproc(s))
		    ఆపు;
	    }
	    ఒకవేళ (lnum >= curbuf->b_ml.ml_పంక్తి_లెక్క)
		ఆపు;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    ఒకవేళ (!just_started && (!comma && *s != ',' && *s != ')'))
		ఆపు;
	    just_started = FALSE;
	}
	లేకపోతే ఒకవేళ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	లేకపోతే
	{
	    ++s;
	    just_started = FALSE;
	}
    }

పూర్తి:
    ఒకవేళ (lnum != మొదటి_lnum && sp != ఖాళీ)
	*sp = ml_get(మొదటి_lnum);

    తిరిగి retval;
}

    స్థిర పూర్ణాంకం
cin_isif(char_u *p)
{
    తిరిగి (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    స్థిర పూర్ణాంకం
cin_isdo(char_u *p)
{
    తిరిగి (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    స్థిర పూర్ణాంకం
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    పూర్ణాంకం		retval = FALSE;

    p = cin_skipcomment(p);
    ఒకవేళ (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    ఒకవేళ (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.నిలువు = 0;
	p = ml_get_curline();
	అయితే (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.నిలువు;
	}
	ఒకవేళ ((trypos = findmatchlimit(ఖాళీ, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != ఖాళీ
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    తిరిగి retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    స్థిర పూర్ణాంకం
cin_is_if_for_while_before_స్థానభ్రంశం(char_u *పంక్తి, పూర్ణాంకం *poffset)
{
    పూర్ణాంకం స్థానభ్రంశం = *poffset;

    ఒకవేళ (స్థానభ్రంశం-- < 2)
	తిరిగి 0;
    అయితే (స్థానభ్రంశం > 2 && VIM_ISWHITE(పంక్తి[స్థానభ్రంశం]))
	--స్థానభ్రంశం;

    స్థానభ్రంశం -= 1;
    ఒకవేళ (STRNCMP(పంక్తి + స్థానభ్రంశం, "if", 2) == 0)
	వెళ్ళు probably_దొరికింది;

    ఒకవేళ (స్థానభ్రంశం >= 1)
    {
	స్థానభ్రంశం -= 1;
	ఒకవేళ (STRNCMP(పంక్తి + స్థానభ్రంశం, "for", 3) == 0)
	    వెళ్ళు probably_దొరికింది;

	ఒకవేళ (స్థానభ్రంశం >= 2)
	{
	    స్థానభ్రంశం -= 2;
	    ఒకవేళ (STRNCMP(పంక్తి + స్థానభ్రంశం, "while", 5) == 0)
		వెళ్ళు probably_దొరికింది;
	}
    }
    తిరిగి 0;

probably_దొరికింది:
    ఒకవేళ (!స్థానభ్రంశం || !vim_isIDc(పంక్తి[స్థానభ్రంశం - 1]))
    {
	*poffset = స్థానభ్రంశం;
	తిరిగి 1;
    }
    తిరిగి 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    స్థిర పూర్ణాంకం
cin_iswhileofdo_చివర(పూర్ణాంకం terminated)
{
    char_u	*పంక్తి;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    పూర్ణాంకం		i;

    ఒకవేళ (terminated != ';')	// there must be a ';' at the end
	తిరిగి FALSE;

    p = పంక్తి = ml_get_curline();
    అయితే (*p != NUL)
    {
	p = cin_skipcomment(p);
	ఒకవేళ (*p == ')')
	{
	    s = skipwhite(p + 1);
	    ఒకవేళ (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (పూర్ణాంకం)(p - పంక్తి);
		curwin->w_cursor.నిలువు = i;
		trypos = కనుగొను_సరిపోలిక_paren(curbuf->b_ind_maxparen);
		ఒకవేళ (trypos != ఖాళీ)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    ఒకవేళ (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    ఒకవేళ (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			తిరిగి TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		పంక్తి = ml_get_curline();
		p = పంక్తి + i;
	    }
	}
	ఒకవేళ (*p != NUL)
	    ++p;
    }
    తిరిగి FALSE;
}

    స్థిర పూర్ణాంకం
cin_isbreak(char_u *p)
{
    తిరిగి (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    స్థిర పూర్ణాంకం
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    పూర్ణాంకం		class_or_struct, lookfor_ctor_init, cpp_పునాది_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*పంక్తి = ml_get_curline();

    ఒకవేళ (pos->lnum <= lnum)
	తిరిగి cached->దొరికింది;	// Use the cached result

    pos->నిలువు = 0;

    s = skipwhite(పంక్తి);
    ఒకవేళ (*s == '#')		// skip #define FOO x ? (x) : x
	తిరిగి FALSE;
    s = cin_skipcomment(s);
    ఒకవేళ (*s == NUL)
	తిరిగి FALSE;

    cpp_పునాది_class = lookfor_ctor_init = class_or_struct = FALSE;

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
    అయితే (lnum > 1)
    {
	పంక్తి = ml_get(lnum - 1);
	s = skipwhite(పంక్తి);
	ఒకవేళ (*s == '#' || *s == NUL)
	    ఆపు;
	అయితే (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    ఒకవేళ (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		ఆపు;
	    ఒకవేళ (*s != NUL)
		++s;
	}
	ఒకవేళ (*s != NUL)
	    ఆపు;
	--lnum;
    }

    pos->lnum = lnum;
    పంక్తి = ml_get(lnum);
    s = పంక్తి;
    కొరకు (;;)
    {
	ఒకవేళ (*s == NUL)
	{
	    ఒకవేళ (lnum == curwin->w_cursor.lnum)
		ఆపు;
	    // Continue in the cursor line.
	    పంక్తి = ml_get(++lnum);
	    s = పంక్తి;
	}
	ఒకవేళ (s == పంక్తి)
	{
	    // don't recognize "case (foo):" as a baseclass
	    ఒకవేళ (cin_iscase(s, FALSE))
		ఆపు;
	    s = cin_skipcomment(పంక్తి);
	    ఒకవేళ (*s == NUL)
		కొనసాగు;
	}

	ఒకవేళ (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	లేకపోతే ఒకవేళ (s[0] == ':')
	{
	    ఒకవేళ (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    లేకపోతే ఒకవేళ (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_పునాది_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->నిలువు = 0;
		s = cin_skipcomment(s + 1);
	    }
	    లేకపోతే
		s = cin_skipcomment(s + 1);
	}
	లేకపోతే ఒకవేళ ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    ఒకవేళ (*s == 'c')
		s = cin_skipcomment(s + 5);
	    లేకపోతే
		s = cin_skipcomment(s + 6);
	}
	లేకపోతే
	{
	    ఒకవేళ (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_పునాది_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    లేకపోతే ఒకవేళ (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    లేకపోతే ఒకవేళ (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		తిరిగి FALSE;
	    }
	    లేకపోతే ఒకవేళ (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    లేకపోతే ఒకవేళ (pos->నిలువు == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		ఒకవేళ (cpp_పునాది_class)
		    pos->నిలువు = (colnr_T)(s - పంక్తి);
	    }

	    // When the line ends in a comma don't align with it.
	    ఒకవేళ (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->నిలువు = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->దొరికింది = cpp_పునాది_class;
    ఒకవేళ (cpp_పునాది_class)
	pos->lnum = lnum;
    తిరిగి cpp_పునాది_class;
}

    స్థిర పూర్ణాంకం
get_baseclass_amount(పూర్ణాంకం నిలువు)
{
    పూర్ణాంకం		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    ఒకవేళ (నిలువు == 0)
    {
	amount = get_indent();
	ఒకవేళ (కనుగొను_చివరి_paren(ml_get_curline(), '(', ')')
		&& (trypos = కనుగొను_సరిపోలిక_paren(curbuf->b_ind_maxparen)) != ఖాళీ)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	ఒకవేళ (!cin_చివర్లు_లోపల(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    లేకపోతే
    {
	curwin->w_cursor.నిలువు = నిలువు;
	getvcol(curwin, &curwin->w_cursor, &vcol, ఖాళీ, ఖాళీ, 0);
	amount = (పూర్ణాంకం)vcol;
    }
    ఒకవేళ (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    తిరిగి amount;
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

    స్థిర pos_T *
కనుగొను_ప్రారంభం_brace(శూన్య)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    స్థిర pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    అయితే ((trypos = findmatchlimit(ఖాళీ, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != ఖాళీ)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = ఖాళీ;
	// ignore the { if it's in a // or / *  * / comment
	ఒకవేళ ((colnr_T)cin_skip2pos(trypos) == trypos->నిలువు
		       && (pos = ind_కనుగొను_ప్రారంభం_CORS(ఖాళీ)) == ఖాళీ) // XXX
	    ఆపు;
	ఒకవేళ (pos != ఖాళీ)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    తిరిగి trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    స్థిర pos_T *
కనుగొను_సరిపోలిక_paren_after_brace (పూర్ణాంకం ind_maxparen)	    // XXX
{
    pos_T	*trypos = కనుగొను_సరిపోలిక_paren(ind_maxparen);

    ఒకవేళ (trypos == ఖాళీ)
	తిరిగి ఖాళీ;

    pos_T	*tryposBrace = కనుగొను_ప్రారంభం_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    ఒకవేళ (tryposBrace != ఖాళీ
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->నిలువు < tryposBrace->నిలువు))
	trypos = ఖాళీ;
    తిరిగి trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    స్థిర పూర్ణాంకం
corr_ind_maxparen(pos_T *startpos)
{
    దీర్ఘ	n = (దీర్ఘ)startpos->lnum - (దీర్ఘ)curwin->w_cursor.lnum;

    ఒకవేళ (n > 0 && n < curbuf->b_ind_maxparen / 2)
	తిరిగి curbuf->b_ind_maxparen - (పూర్ణాంకం)n;
    తిరిగి curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    శూన్య
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    పూర్ణాంకం		divider;
    పూర్ణాంకం		fraction = 0;
    పూర్ణాంకం		sw;
    దీర్ఘ	t = get_sw_విలువ(buf);

    // needed for cino-(, it will be multiplied by 2 again
    ఒకవేళ (t > గరిష్ఠ_పూర్ణాంకం / 2)
	sw = గరిష్ఠ_పూర్ణాంకం / 2;
    లేకపోతే
	sw = (పూర్ణాంకం)t;

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
    buf->b_ind_మొదటి_open = 0;

    // Spaces from the prevailing indent a leftmost open brace should be
    // located.
    buf->b_ind_open_extra = 0;

    // Spaces from the matching open brace (real location for one at the left
    // edge; imaginary location from one that ends a line) the matching close
    // brace should be located.
    buf->b_ind_close_extra = 0;

    // Spaces from the edge of the line an open brace sitting in the leftmost
    // column is imagined to be.
    buf->b_ind_open_ఎడమ_imag = 0;

    // Spaces jump labels should be shifted to the left if N is non-negative,
    // otherwise the jump label will be put to column 1.
    buf->b_ind_jump_label = -1;

    // Spaces from the switch() indent a "case xx" label should be located.
    buf->b_ind_case = sw;

    // Spaces from the "case xx:" code after a switch() should be located.
    buf->b_ind_case_సంకేతం = sw;

    // Lineup break at end of case in switch() with case label.
    buf->b_ind_case_break = 0;

    // Spaces from the class declaration indent a scope declaration label
    // should be located.
    buf->b_ind_scopedecl = sw;

    // Spaces from the scope declaration label code should be located.
    buf->b_ind_scopedecl_సంకేతం = sw;

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
    buf->b_ind_లోపల_comment = 3;

    // Boolean: if non-zero, use b_ind_in_comment even if there is something
    // after the comment opener.
    buf->b_ind_లోపల_comment2 = 0;

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

    కొరకు (p = buf->b_p_cino; *p; )
    {
	l = p++;
	ఒకవేళ (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	ఒకవేళ (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((అక్షరం *)++p);
	    అయితే (VIM_ISDIGIT(*p))
	    {
		++p;
		ఒకవేళ (divider)
		    divider *= 10;
		లేకపోతే
		    divider = 10;
	    }
	}
	ఒకవేళ (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    ఒకవేళ (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    లేకపోతే
	    {
		n *= sw;
		ఒకవేళ (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	ఒకవేళ (l[1] == '-')
	    n = -n;

	n = trim_గమ్యం_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	ఎంపిక (*l)
	{
	    స్థితి '>': buf->b_ind_level = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'e': buf->b_ind_open_imag = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'n': buf->b_ind_no_brace = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'f': buf->b_ind_మొదటి_open = (పూర్ణాంకం)n; ఆపు;
	    స్థితి '{': buf->b_ind_open_extra = (పూర్ణాంకం)n; ఆపు;
	    స్థితి '}': buf->b_ind_close_extra = (పూర్ణాంకం)n; ఆపు;
	    స్థితి '^': buf->b_ind_open_ఎడమ_imag = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'L': buf->b_ind_jump_label = (పూర్ణాంకం)n; ఆపు;
	    స్థితి ':': buf->b_ind_case = (పూర్ణాంకం)n; ఆపు;
	    స్థితి '=': buf->b_ind_case_సంకేతం = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'b': buf->b_ind_case_break = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'p': buf->b_ind_param = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 't': buf->b_ind_func_type = (పూర్ణాంకం)n; ఆపు;
	    స్థితి '/': buf->b_ind_comment = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'c': buf->b_ind_లోపల_comment = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'C': buf->b_ind_లోపల_comment2 = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'i': buf->b_ind_cpp_baseclass = (పూర్ణాంకం)n; ఆపు;
	    స్థితి '+': buf->b_ind_continuation = (పూర్ణాంకం)n; ఆపు;
	    స్థితి '(': buf->b_ind_unclosed = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'u': buf->b_ind_unclosed2 = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'U': buf->b_ind_unclosed_noignore = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'W': buf->b_ind_unclosed_wrapped = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'w': buf->b_ind_unclosed_whiteok = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'm': buf->b_ind_matching_paren = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'M': buf->b_ind_paren_prev = (పూర్ణాంకం)n; ఆపు;
	    స్థితి ')': buf->b_ind_maxparen = (పూర్ణాంకం)n; ఆపు;
	    స్థితి '*': buf->b_ind_maxcomment = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'g': buf->b_ind_scopedecl = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'h': buf->b_ind_scopedecl_సంకేతం = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'j': buf->b_ind_java = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'J': buf->b_ind_js = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'l': buf->b_ind_keep_case_label = (పూర్ణాంకం)n; ఆపు;
	    స్థితి '#': buf->b_ind_hash_comment = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'N': buf->b_ind_cpp_namespace = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'k': buf->b_ind_if_for_while = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'E': buf->b_ind_cpp_extern_c = (పూర్ణాంకం)n; ఆపు;
	    స్థితి 'P': buf->b_ind_pragma = (పూర్ణాంకం)n; ఆపు;
	}
	ఒకవేళ (*p == ',')
	    ++p;
    }
}

    స్థిర పూర్ణాంకం
కనుగొను_సరిపోలిక(పూర్ణాంకం lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    పూర్ణాంకం		elselevel;
    పూర్ణాంకం		whilelevel;

    ఒకవేళ (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    లేకపోతే
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.నిలువు = 0;

    అయితే (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.నిలువు = 0;

	look = cin_skipcomment(ml_get_curline());
	ఒకవేళ (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = కనుగొను_ప్రారంభం_brace();  // XXX
	    ఒకవేళ (theirscope == ఖాళీ)
		ఆపు;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    ఒకవేళ (theirscope->lnum < ourscope)
		ఆపు;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    ఒకవేళ (theirscope->lnum > ourscope)
		కొనసాగు;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    ఒకవేళ (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		ఒకవేళ (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    ఒకవేళ (!cin_isif(mightbeif))
			++elselevel;
		    కొనసాగు;
		}

		// If it's an "if" decrement elselevel
		ఒకవేళ (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    ఒకవేళ (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    ఒకవేళ (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		కొనసాగు;
	    }

	    // If it's a "do" decrement whilelevel
	    ఒకవేళ (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    ఒకవేళ (elselevel <= 0 && whilelevel <= 0)
		తిరిగి OK;
	}
    }
    తిరిగి FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    పూర్ణాంకం
get_c_indent(శూన్య)
{
    pos_T	cur_curpos;
    పూర్ణాంకం		amount;
    పూర్ణాంకం		scope_amount;
    పూర్ణాంకం		cur_amount = MAXCOL;
    colnr_T	నిలువు;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = ఖాళీ;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*ప్రారంభం;
    పూర్ణాంకం		ప్రారంభం_brace;
#నిర్వచించు BRACE_లోపల_COL0		1	    // '{' is in column 0
#నిర్వచించు BRACE_AT_ప్రారంభం		2	    // '{' is at start of line
#నిర్వచించు BRACE_AT_చివర		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    పూర్ణాంకం		lookfor;
    పూర్ణాంకం		whilelevel;
    linenr_T	lnum;
    పూర్ణాంకం		n;
    పూర్ణాంకం		iscase;
    పూర్ణాంకం		lookfor_break;
    పూర్ణాంకం		lookfor_cpp_namespace = FALSE;
    పూర్ణాంకం		cont_amount = 0;    // amount for continuation line
    పూర్ణాంకం		original_పంక్తి_islabel;
    పూర్ణాంకం		added_గమ్యం_amount = 0;
    పూర్ణాంకం		js_cur_has_కీలకం = 0;
    linenr_T	raw_string_ప్రారంభం = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    పూర్ణాంకం		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    ఒకవేళ (cur_curpos.lnum == 1)
	తిరిగి 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    ఒకవేళ (linecopy == ఖాళీ)
	తిరిగి 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    ఒకవేళ ((State & బహుళకం_చొప్పించు)
	    && curwin->w_cursor.నిలువు < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.నిలువు] == ')')
	linecopy[curwin->w_cursor.నిలువు] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.నిలువు = 0;

    original_పంక్తి_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_కనుగొను_ప్రారంభం_comment();
    ఒకవేళ (comment_pos != ఖాళీ)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = కనుగొను_ప్రారంభం_rawstring(curbuf->b_ind_maxcomment);
    ఒకవేళ (trypos != ఖాళీ && (comment_pos == ఖాళీ
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	వెళ్ళు laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    ఒకవేళ (*theline == '#' && (*linecopy == '#' || లోపల_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	ఒకవేళ (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    వెళ్ళు theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    ఒకవేళ (original_పంక్తి_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	వెళ్ళు theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    ఒకవేళ (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = కనుగొను_పంక్తి_comment(); // XXX
	ఒకవేళ (trypos == ఖాళీ && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.నిలువు =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    ఒకవేళ (linecomment_pos.నిలువు != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	ఒకవేళ (trypos  != ఖాళీ)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &నిలువు, ఖాళీ, ఖాళీ, 0);
	    amount = నిలువు;
	    వెళ్ళు theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    ఒకవేళ (!cin_iscomment(theline) && comment_pos != ఖాళీ) // XXX
    {
	పూర్ణాంకం	lead_ప్రారంభం_len = 2;
	పూర్ణాంకం	lead_మధ్య_len = 1;
	char_u	lead_ప్రారంభం[COM_MAX_LEN];	// start-comment string
	char_u	lead_మధ్య[COM_MAX_LEN];	// middle-comment string
	char_u	lead_చివర[COM_MAX_LEN];		// end-comment string
	పూర్ణాంకం	lead_చివర_len;
	char_u	*p;
	పూర్ణాంకం	ప్రారంభం_align = 0;
	పూర్ణాంకం	ప్రారంభం_off = 0;
	పూర్ణాంకం	పూర్తి = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &నిలువు, ఖాళీ, ఖాళీ, 0);
	amount = నిలువు;
	*lead_ప్రారంభం = NUL;
	*lead_మధ్య = NUL;

	p = curbuf->b_p_com;
	అయితే (*p != NUL)
	{
	    పూర్ణాంకం	align = 0;
	    పూర్ణాంకం	off = 0;
	    పూర్ణాంకం what = 0;

	    అయితే (*p != NUL && *p != ':')
	    {
		ఒకవేళ (*p == COM_ప్రారంభం || *p == COM_చివర || *p == COM_మధ్య)
		    what = *p++;
		లేకపోతే ఒకవేళ (*p == COM_ఎడమ || *p == COM_కుడి)
		    align = *p++;
		లేకపోతే ఒకవేళ (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		లేకపోతే
		    ++p;
	    }

	    ఒకవేళ (*p == ':')
		++p;
	    lead_చివర_len = copy_option_part(&p, lead_చివర, COM_MAX_LEN, ",");
	    ఒకవేళ (what == COM_ప్రారంభం)
	    {
		STRCPY(lead_ప్రారంభం, lead_చివర);
		lead_ప్రారంభం_len = lead_చివర_len;
		ప్రారంభం_off = off;
		ప్రారంభం_align = align;
	    }
	    లేకపోతే ఒకవేళ (what == COM_మధ్య)
	    {
		STRCPY(lead_మధ్య, lead_చివర);
		lead_మధ్య_len = lead_చివర_len;
	    }
	    లేకపోతే ఒకవేళ (what == COM_చివర)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		ఒకవేళ (STRNCMP(theline, lead_మధ్య, lead_మధ్య_len) == 0
			&& STRNCMP(theline, lead_చివర, lead_చివర_len) != 0)
		{
		    పూర్తి = TRUE;
		    ఒకవేళ (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			ఒకవేళ (STRNCMP(look, lead_ప్రారంభం, lead_ప్రారంభం_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			లేకపోతే ఒకవేళ (STRNCMP(look, lead_మధ్య,
							lead_మధ్య_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    ఆపు;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			లేకపోతే ఒకవేళ (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->నిలువు,
					     lead_ప్రారంభం, lead_ప్రారంభం_len) != 0)
			    కొనసాగు;
		    }
		    ఒకవేళ (ప్రారంభం_off != 0)
			amount += ప్రారంభం_off;
		    లేకపోతే ఒకవేళ (ప్రారంభం_align == COM_కుడి)
			amount += vim_strsize(lead_ప్రారంభం)
						   - vim_strsize(lead_మధ్య);
		    ఆపు;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		ఒకవేళ (STRNCMP(theline, lead_మధ్య, lead_మధ్య_len) != 0
			&& STRNCMP(theline, lead_చివర, lead_చివర_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    ఒకవేళ (off != 0)
			amount += off;
		    లేకపోతే ఒకవేళ (align == COM_కుడి)
			amount += vim_strsize(lead_ప్రారంభం)
						   - vim_strsize(lead_మధ్య);
		    పూర్తి = TRUE;
		    ఆపు;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	ఒకవేళ (పూర్తి)
	    ;
	లేకపోతే ఒకవేళ (theline[0] == '*')
	    amount += 1;
	లేకపోతే
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    కొరకు (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		ఒకవేళ (linewhite(lnum))		    // skip blank lines
		    కొనసాగు;
		amount = get_indent_lnum(lnum);	    // XXX
		ఆపు;
	    }
	    ఒకవేళ (amount == -1)			    // use the comment opener
	    {
		ఒకవేళ (!curbuf->b_ind_లోపల_comment2)
		{
		    ప్రారంభం = ml_get(comment_pos->lnum);
		    look = ప్రారంభం + comment_pos->నిలువు + 2; // skip / and *
		    ఒకవేళ (*look != NUL)		    // if something after it
			comment_pos->నిలువు = (colnr_T)(skipwhite(look) - ప్రారంభం);
		}
		getvcol(curwin, comment_pos, &నిలువు, ఖాళీ, ఖాళీ, 0);
		amount = నిలువు;
		ఒకవేళ (curbuf->b_ind_లోపల_comment2 || *look == NUL)
		    amount += curbuf->b_ind_లోపల_comment;
	    }
	}
	వెళ్ళు theend;
    }

    // Are we looking at a ']' that has a match?
    ఒకవేళ (*skipwhite(theline) == ']'
	    && (trypos = కనుగొను_సరిపోలిక_char('[', curbuf->b_ind_maxparen)) != ఖాళీ)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	వెళ్ళు theend;
    }

    // Are we inside parentheses or braces?  XXX
    ఒకవేళ (((trypos = కనుగొను_సరిపోలిక_paren(curbuf->b_ind_maxparen)) != ఖాళీ
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = కనుగొను_ప్రారంభం_brace()) != ఖాళీ
	    || trypos != ఖాళీ)
    {
      ఒకవేళ (trypos != ఖాళీ && tryposBrace != ఖాళీ)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  ఒకవేళ (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->నిలువు < tryposBrace->నిలువు)
	      trypos = ఖాళీ;
	  లేకపోతే
	      tryposBrace = ఖాళీ;
      }

      ఒకవేళ (trypos != ఖాళీ)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	ఒకవేళ (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	లేకపోతే
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    కొరకు (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		ఒకవేళ (cin_nocode(l))		// skip comment lines
		    కొనసాగు;
		ఒకవేళ (cin_ispreproc_cont(&l, &lnum, &amount))
		    కొనసాగు;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		ఒకవేళ ((trypos = ind_కనుగొను_ప్రారంభం_CORS(ఖాళీ)) != ఖాళీ)
		{
		    lnum = trypos->lnum + 1;
		    కొనసాగు;
		}

		// XXX
		ఒకవేళ ((trypos = కనుగొను_సరిపోలిక_paren(
			corr_ind_maxparen(&cur_curpos))) != ఖాళీ
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->నిలువు == our_paren_pos.నిలువు)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    ఒకవేళ (theline[0] == ')')
		    {
			ఒకవేళ (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    ఆపు;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	ఒకవేళ (amount == -1)
	{
	    పూర్ణాంకం	    ignore_paren_నిలువు = 0;
	    పూర్ణాంకం	    is_if_for_while = 0;

	    ఒకవేళ (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *పంక్తి;

		trypos = &our_paren_pos;
		చేయి
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.నిలువు = outermost.నిలువు;

		    trypos = కనుగొను_సరిపోలిక_paren(curbuf->b_ind_maxparen);
		} అయితే (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		పంక్తి = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_స్థానభ్రంశం(పంక్తి, &outermost.నిలువు);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    ఒకవేళ (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *పంక్తి;
		పూర్ణాంకం	    look_నిలువు;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		పంక్తి = ml_get_curline();
		look_నిలువు = (పూర్ణాంకం)(look - పంక్తి);
		curwin->w_cursor.నిలువు = look_నిలువు + 1;
		ఒకవేళ ((trypos = findmatchlimit(ఖాళీ, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != ఖాళీ
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->నిలువు < our_paren_pos.నిలువు)
		    ignore_paren_నిలువు = trypos->నిలువు + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_నిలువు;
	    }
	    ఒకవేళ (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
						      && is_if_for_while == 0)
		    || (!curbuf->b_ind_unclosed_noignore && *look == '('
						    && ignore_paren_నిలువు == 0))
	    {
		// If we're looking at a close paren, line up right there;
		// otherwise, line up with the next (non-white) character.
		// When b_ind_unclosed_wrapped is set and the matching paren is
		// the last nonwhite character of the line, use either the
		// indent of the current line or the indentation of the next
		// outer paren and add b_ind_unclosed_wrapped (for very long
		// lines).
		ఒకవేళ (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    ఒకవేళ (curbuf->b_ind_unclosed_wrapped
				       && cin_చివర్లు_లోపల(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			కొరకు (నిలువు = 0; నిలువు < our_paren_pos.నిలువు; ++నిలువు)
			{
			    ఎంపిక (l[నిలువు])
			    {
				స్థితి '(':
				స్థితి '{': ++n;
					  ఆపు;

				స్థితి ')':
				స్థితి '}': ఒకవేళ (n > 1)
					      --n;
					  ఆపు;
			    }
			}

			our_paren_pos.నిలువు = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    లేకపోతే ఒకవేళ (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.నిలువు++;
		    లేకపోతే
		    {
			నిలువు = our_paren_pos.నిలువు + 1;
			అయితే (VIM_ISWHITE(l[నిలువు]))
			    నిలువు++;
			ఒకవేళ (l[నిలువు] != NUL)	// In case of trailing space
			    our_paren_pos.నిలువు = నిలువు;
			లేకపోతే
			    our_paren_pos.నిలువు++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		ఒకవేళ (our_paren_pos.నిలువు > 0)
		{
		    getvcol(curwin, &our_paren_pos, &నిలువు, ఖాళీ, ఖాళీ, 0);
		    ఒకవేళ (cur_amount > (పూర్ణాంకం)నిలువు)
			cur_amount = నిలువు;
		}
	    }

	    ఒకవేళ (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    లేకపోతే ఒకవేళ ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_నిలువు == 0))
	    {
		ఒకవేళ (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    లేకపోతే
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		నిలువు = our_paren_pos.నిలువు;
		అయితే ((పూర్ణాంకం)our_paren_pos.నిలువు > ignore_paren_నిలువు)
		{
		    --our_paren_pos.నిలువు;
		    ఎంపిక (*ml_get_pos(&our_paren_pos))
		    {
			స్థితి '(': amount += curbuf->b_ind_unclosed2;
				  నిలువు = our_paren_pos.నిలువు;
				  ఆపు;
			స్థితి ')': amount -= curbuf->b_ind_unclosed2;
				  నిలువు = MAXCOL;
				  ఆపు;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		ఒకవేళ (నిలువు == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		లేకపోతే
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.నిలువు = నిలువు;
		    ఒకవేళ (కనుగొను_సరిపోలిక_paren_after_brace(curbuf->b_ind_maxparen)
								      != ఖాళీ)
			amount += curbuf->b_ind_unclosed2;
		    లేకపోతే
		    {
			ఒకవేళ (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			లేకపోతే
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		ఒకవేళ (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	ఒకవేళ (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      లేకపోతే
      {
	// We are inside braces, there is a { before this line at the position
	// stored in tryposBrace.
	// Make a copy of tryposBrace, it may point to pos_copy inside
	// find_start_brace(), which may be changed somewhere.
	tryposCopy = *tryposBrace;
	tryposBrace = &tryposCopy;
	trypos = tryposBrace;
	ourscope = trypos->lnum;
	ప్రారంభం = ml_get(ourscope);

	// Now figure out how indented the line is in general.
	// If the brace was at the start of the line, we use that;
	// otherwise, check out the indentation of the line as
	// a whole and then add the "imaginary indent" to that.
	look = skipwhite(ప్రారంభం);
	ఒకవేళ (*look == '{')
	{
	    getvcol(curwin, trypos, &నిలువు, ఖాళీ, ఖాళీ, 0);
	    amount = నిలువు;
	    ఒకవేళ (*ప్రారంభం == '{')
		ప్రారంభం_brace = BRACE_లోపల_COL0;
	    లేకపోతే
		ప్రారంభం_brace = BRACE_AT_ప్రారంభం;
	}
	లేకపోతే
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    ఒకవేళ (కనుగొను_చివరి_paren(ప్రారంభం, '(', ')')
			&& (trypos = కనుగొను_సరిపోలిక_paren(curbuf->b_ind_maxparen))
								      != ఖాళీ)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    ఒకవేళ ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    లేకపోతే ఒకవేళ (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    లేకపోతే
		amount = skip_label(lnum, &l);

	    ప్రారంభం_brace = BRACE_AT_చివర;
	}

	// For Javascript check if the line starts with "key:".
	ఒకవేళ (curbuf->b_ind_js)
	    js_cur_has_కీలకం = cin_has_js_కీలకం(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	ఒకవేళ (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	లేకపోతే
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    ఒకవేళ (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    లేకపోతే ఒకవేళ (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    ఒకవేళ (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		ఒకవేళ (కనుగొను_సరిపోలిక(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    వెళ్ళు theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    ఒకవేళ (ప్రారంభం_brace == BRACE_లోపల_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_ఎడమ_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    లేకపోతే ఒకవేళ (ప్రారంభం_brace == BRACE_AT_ప్రారంభం &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (ప్రారంభం_brace == BRACE_AT_చివర)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    ఒకవేళ (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    లేకపోతే ఒకవేళ (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		లేకపోతే
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    ఒకవేళ (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    ఒకవేళ (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    లేకపోతే ఒకవేళ (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    కొరకు (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.నిలువు = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		ఒకవేళ (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    ఒకవేళ (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ఒకవేళ (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    ఒకవేళ (cont_amount > 0)
				amount = cont_amount;
			    లేకపోతే ఒకవేళ (!curbuf->b_ind_js)
				amount += ind_continuation;
			    ఆపు;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_కనుగొను_ప్రారంభం_CORS(ఖాళీ);
			ఒకవేళ (trypos != ఖాళీ)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.నిలువు = 0;
			    కొనసాగు;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			ఒకవేళ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    కొనసాగు;

			ఒకవేళ (cin_nocode(l))
			    కొనసాగు;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			ఒకవేళ (ప్రారంభం_brace != BRACE_లోపల_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    ఒకవేళ (terminated == ',')
				ఆపు;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    ఒకవేళ (terminated != ';' && cin_isinit())
				ఆపు;

			    // nothing useful found
			    ఒకవేళ (terminated == 0 || terminated == '{')
				కొనసాగు;
			}

			ఒకవేళ (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = ఖాళీ;
			    ఒకవేళ (కనుగొను_చివరి_paren(l, '(', ')'))
				trypos = కనుగొను_సరిపోలిక_paren(
						      curbuf->b_ind_maxparen);

			    ఒకవేళ (trypos == ఖాళీ && కనుగొను_చివరి_paren(l, '{', '}'))
				trypos = కనుగొను_ప్రారంభం_brace();

			    ఒకవేళ (trypos != ఖాళీ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.నిలువు = 0;
				కొనసాగు;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			ఒకవేళ (cont_amount > 0)
			    amount = cont_amount;
			లేకపోతే
			    amount += ind_continuation;
		    }
		    లేకపోతే ఒకవేళ (lookfor == LOOKFOR_UNTERM)
		    {
			ఒకవేళ (cont_amount > 0)
			    amount = cont_amount;
			లేకపోతే
			    amount += ind_continuation;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (lookfor != LOOKFOR_పదం
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    ఒకవేళ (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_గమ్యం_amount = curbuf->b_ind_open_extra;
			    }
			}

			ఒకవేళ (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    ఒకవేళ (curwin->w_cursor.lnum == ourscope)
				కొనసాగు;

			    ఒకవేళ (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - కనుగొను_NAMESPACE_LIM)
				ఆపు;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_కనుగొను_ప్రారంభం_CORS(ఖాళీ);
			    ఒకవేళ (trypos != ఖాళీ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.నిలువు = 0;
				కొనసాగు;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    ఒకవేళ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				కొనసాగు;

			    // Finally the actual check for "namespace".
			    ఒకవేళ (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_గమ్యం_amount;
				ఆపు;
			    }
			    లేకపోతే ఒకవేళ (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_గమ్యం_amount;
				ఆపు;
			    }

			    ఒకవేళ (cin_nocode(l))
				కొనసాగు;
			}
		    }
		    ఆపు;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		ఒకవేళ ((trypos = ind_కనుగొను_ప్రారంభం_CORS(&raw_string_ప్రారంభం)) != ఖాళీ)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.నిలువు = 0;
		    కొనసాగు;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		ఒకవేళ (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    ఒకవేళ (lookfor == LOOKFOR_CPP_BASECLASS)
			ఆపు;

		    // When looking for a "do" we are not interested in
		    // labels.
		    ఒకవేళ (whilelevel > 0)
			కొనసాగు;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    ఒకవేళ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ఒకవేళ (cont_amount > 0)
			    amount = cont_amount;
			లేకపోతే
			    amount += ind_continuation;
			ఆపు;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    ఒకవేళ (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			ఒకవేళ ((trypos = కనుగొను_ప్రారంభం_brace()) == ఖాళీ
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    ఆపు;
			}
			కొనసాగు;
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
		    ఒకవేళ (lookfor == LOOKFOR_పదం)
		    {
			ఒకవేళ (n)
			    amount = n;

			ఒకవేళ (!lookfor_break)
			    ఆపు;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    ఒకవేళ (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			ఒకవేళ (l != ఖాళీ && cin_is_cinword(l))
			{
			    ఒకవేళ (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    లేకపోతే
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			ఆపు;
		    }

		    // Try to get the indent of a statement before the switch
		    // label.  If nothing is found, line up relative to the
		    // switch label.
		    //	    break;		<- may line up with this line
		    //	 case xx:
		    // ->   y = 1;
		    scope_amount = get_indent() + (iscase    // XXX
					? curbuf->b_ind_case_సంకేతం
					: curbuf->b_ind_scopedecl_సంకేతం);
		    lookfor = curbuf->b_ind_case_break
					      ? LOOKFOR_NOBREAK : LOOKFOR_ANY;
		    కొనసాగు;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		ఒకవేళ (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    ఒకవేళ (కనుగొను_చివరి_paren(l, '{', '}')
				     && (trypos = కనుగొను_ప్రారంభం_brace()) != ఖాళీ)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.నిలువు = 0;
		    }
		    కొనసాగు;
		}

		// Ignore jump labels with nothing after them.
		ఒకవేళ (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    ఒకవేళ (l == ఖాళీ || cin_nocode(l))
			కొనసాగు;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		ఒకవేళ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    కొనసాగు;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		ఒకవేళ (lookfor != LOOKFOR_పదం && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		ఒకవేళ (n)
		{
		    ఒకవేళ (lookfor == LOOKFOR_UNTERM)
		    {
			ఒకవేళ (cont_amount > 0)
			    amount = cont_amount;
			లేకపోతే
			    amount += ind_continuation;
		    }
		    లేకపోతే ఒకవేళ (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			కొనసాగు;
		    }
		    లేకపోతే
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.నిలువు);
		    ఆపు;
		}
		లేకపోతే ఒకవేళ (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    ఒకవేళ (cin_isterminated(l, TRUE, FALSE))
			ఆపు;
		    లేకపోతే
			కొనసాగు;
		}

		// What happens next depends on the line being terminated.
		// If terminated with a ',' only consider it terminating if
		// there is another unterminated statement behind, eg:
		//   123,
		//   sizeof
		//ఇక్కడ
		// Otherwise check whether it is an enumeration or structure
		// initialisation (not indented) or a variable declaration
		// (indented).
		terminated = cin_isterminated(l, FALSE, TRUE);

		ఒకవేళ (js_cur_has_కీలకం)
		{
		    js_cur_has_కీలకం = 0; // only check the first line
		    ఒకవేళ (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_కీలకం;
		    }
		}
		ఒకవేళ (lookfor == LOOKFOR_JS_కీలకం && cin_has_js_కీలకం(l))
		{
		    amount = get_indent();
		    ఆపు;
		}
		ఒకవేళ (lookfor == LOOKFOR_COMMA)
		{
		    ఒకవేళ (tryposBrace != ఖాళీ && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			ఆపు;
		    ఒకవేళ (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			ఆపు;
		    లేకపోతే
		    {
			amount = get_indent();
			ఒకవేళ (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    ఆపు;
		    }
		}

		ఒకవేళ (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    ఒకవేళ (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (శూన్య)కనుగొను_చివరి_paren(l, '(', ')');
		    trypos = కనుగొను_సరిపోలిక_paren(corr_ind_maxparen(&cur_curpos));
		    ఒకవేళ (trypos != ఖాళీ && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->నిలువు < tryposBrace->నిలువు)))
			trypos = ఖాళీ;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    ఒకవేళ (trypos == ఖాళీ && terminated == ',')
		    {
			ఒకవేళ (కనుగొను_చివరి_paren(l, '{', '}'))
			    trypos = కనుగొను_ప్రారంభం_brace();
			l = ml_get_curline();
		    }

		    ఒకవేళ (trypos != ఖాళీ)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			ఒకవేళ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.నిలువు = 0;
			    కొనసాగు;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    ఒకవేళ (terminated == ',')
		    {
			అయితే (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    ఒకవేళ (*l == NUL || l[STRLEN(l) - 1] != '\\')
				ఆపు;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.నిలువు = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    ఒకవేళ (curbuf->b_ind_js)
			cur_amount = get_indent();
		    లేకపోతే
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    ఒకవేళ (terminated != ',' && lookfor != LOOKFOR_పదం
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			ఒకవేళ (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			ఒకవేళ (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    కొనసాగు;
			}
			ఆపు;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    ఒకవేళ (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			ఒకవేళ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    ఒకవేళ (cont_amount > 0)
				amount = cont_amount;
			    లేకపోతే
				amount += ind_continuation;
			    ఆపు;
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
			ఒకవేళ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			ఒకవేళ (lookfor != LOOKFOR_పదం)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    ఆపు;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			ఒకవేళ (cin_isdo(l))
			{
			    ఒకవేళ (whilelevel == 0)
				ఆపు;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ఒకవేళ (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    ఒకవేళ (*l == '}')
				curwin->w_cursor.నిలువు =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    ఒకవేళ ((trypos = కనుగొను_ప్రారంభం_brace()) == ఖాళీ
				       || కనుగొను_సరిపోలిక(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ఆపు;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    లేకపోతే
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			ఒకవేళ (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    ఒకవేళ (terminated == ',')
				amount += ind_continuation;
			    ఆపు;
			}

			ఒకవేళ (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    ఒకవేళ (terminated == ',')
			    {
				ఒకవేళ (curbuf->b_ind_cpp_baseclass == 0)
				    ఆపు;

				lookfor = LOOKFOR_CPP_BASECLASS;
				కొనసాగు;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    ఒకవేళ (amount > cur_amount)
				amount = cur_amount;
			}
			లేకపోతే
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (పూర్ణాంకం)STRLEN(l);
			    ఒకవేళ (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				ఆపు;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    ఒకవేళ (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				ఒకవేళ (curbuf->b_ind_js)
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
				    ఒకవేళ (cin_iscomment(skipwhite(l)))
					ఆపు;
				    lookfor = LOOKFOR_COMMA;
				    trypos = కనుగొను_సరిపోలిక_char('[',
						      curbuf->b_ind_maxparen);
				    ఒకవేళ (trypos != ఖాళీ)
				    {
					ఒకవేళ (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    ఆపు;
					}
					ourscope = trypos->lnum;
				    }
				}
				లేకపోతే
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_మొదటి_id_amount();
				}
			    }
			    లేకపోతే
			    {
				ఒకవేళ (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				ఒకవేళ (lookfor != LOOKFOR_పదం
						&& lookfor != LOOKFOR_JS_కీలకం
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_ప్రారంభం != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		లేకపోతే ఒకవేళ (cin_iswhileofdo_చివర(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    ఒకవేళ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ఒకవేళ (cont_amount > 0)
			    amount = cont_amount;
			లేకపోతే
			    amount += ind_continuation;
			ఆపు;
		    }

		    ఒకవేళ (whilelevel == 0)
		    {
			lookfor = LOOKFOR_పదం;
			amount = get_indent();	    // XXX
			ఒకవేళ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		లేకపోతే
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    ఒకవేళ (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			కొనసాగు;
		    }

		    // Handle "do {" line.
		    ఒకవేళ (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			ఒకవేళ (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    కొనసాగు;
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
		    ఒకవేళ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ఒకవేళ (cont_amount > 0)
			    amount = cont_amount;
			లేకపోతే
			    amount += ind_continuation;
			ఆపు;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    ఒకవేళ (lookfor == LOOKFOR_పదం)
		    {
			ఒకవేళ (!lookfor_break && whilelevel == 0)
			    ఆపు;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    లేకపోతే
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
పదం_again:
			l = ml_get_curline();
			ఒకవేళ (కనుగొను_చివరి_paren(l, '(', ')')
				&& (trypos = కనుగొను_సరిపోలిక_paren(
					   curbuf->b_ind_maxparen)) != ఖాళీ)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    ఒకవేళ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.నిలువు = 0;
				కొనసాగు;
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

			ఒకవేళ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			ఒకవేళ (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_పదం;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ఒకవేళ (lookfor == LOOKFOR_పదం
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    ఒకవేళ ((trypos = కనుగొను_ప్రారంభం_brace()) == ఖాళీ
				       || కనుగొను_సరిపోలిక(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ఆపు;
			    కొనసాగు;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			ఒకవేళ (కనుగొను_చివరి_paren(l, '{', '}') // XXX
				     && (trypos = కనుగొను_ప్రారంభం_brace()) != ఖాళీ)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    ఒకవేళ (*l == '}' || !cin_iselse(l))
				వెళ్ళు పదం_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.నిలువు = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      ఒకవేళ (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      ఒకవేళ (curbuf->b_ind_jump_label > 0 && original_పంక్తి_islabel)
	  amount -= curbuf->b_ind_jump_label;

      వెళ్ళు theend;
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

    ఒకవేళ (theline[0] == '{')
    {
	amount = curbuf->b_ind_మొదటి_open;
	వెళ్ళు theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    ఒకవేళ (cur_curpos.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == ఖాళీ
	    && vim_strchr(theline, '}') == ఖాళీ
	    && !cin_చివర్లు_లోపల(theline, (char_u *)":")
	    && !cin_చివర్లు_లోపల(theline, (char_u *)",")
	    && cin_isfuncdecl(ఖాళీ, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	వెళ్ళు theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    అయితే (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.నిలువు = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	ఒకవేళ ((trypos = ind_కనుగొను_ప్రారంభం_CORS(ఖాళీ)) != ఖాళీ)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.నిలువు = 0;
	    కొనసాగు;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	ఒకవేళ (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	ఒకవేళ (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.నిలువు);
	    ఆపు;
	}

	// Skip preprocessor directives and blank lines.
	ఒకవేళ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    కొనసాగు;

	ఒకవేళ (cin_nocode(l))
	    కొనసాగు;

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
	ఒకవేళ (cin_చివర్లు_లోపల(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    ఒకవేళ (కనుగొను_చివరి_paren(l, '(', ')')
		    && (trypos = కనుగొను_సరిపోలిక_paren(
				     curbuf->b_ind_maxparen)) != ఖాళీ)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    అయితే (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		ఒకవేళ (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    ఆపు;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.నిలువు = 0;
	    }

	    amount = get_indent();	    // XXX

	    ఒకవేళ (amount == 0)
		amount = cin_మొదటి_id_amount();
	    ఒకవేళ (amount == 0)
		amount = ind_continuation;
	    ఆపు;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	ఒకవేళ (cin_isfuncdecl(ఖాళీ, cur_curpos.lnum, 0))  // XXX
	    ఆపు;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	ఒకవేళ (*skipwhite(l) == '}')
	    ఆపు;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	ఒకవేళ (cin_చివర్లు_లోపల(l, (char_u *)"};"))
	    ఆపు;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	ఒకవేళ (cin_చివర్లు_లోపల(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    ఆపు;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	ఒకవేళ (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    అయితే (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		ఒకవేళ (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    ఆపు;
	    }
	    ఒకవేళ (curwin->w_cursor.lnum > 0
			    && cin_చివర్లు_లోపల(look, (char_u *)"}"))
		ఆపు;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	ఒకవేళ (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    ఆపు;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	ఒకవేళ (cin_చివర్లు_లోపల(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    ఒకవేళ (cin_చివర్లు_లోపల(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		ఆపు;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	కనుగొను_చివరి_paren(l, '(', ')');

	ఒకవేళ ((trypos = కనుగొను_సరిపోలిక_paren(curbuf->b_ind_maxparen)) != ఖాళీ)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	ఆపు;
    }

    // add extra indent for a comment
    ఒకవేళ (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    ఒకవేళ (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	ఒకవేళ (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    ఒకవేళ (cur_amount > 0)
		amount = cur_amount;
	    లేకపోతే ఒకవేళ (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    ఒకవేళ (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    తిరిగి amount;
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
    పూర్ణాంకం
లోపల_cinkeys(
    పూర్ణాంకం		keytyped,
    పూర్ణాంకం		when,
    పూర్ణాంకం		పంక్తి_is_ఖాళీలు)
{
    char_u	*look;
    పూర్ణాంకం		try_సరిపోలిక;
    పూర్ణాంకం		try_సరిపోలిక_మాట;
    char_u	*p;
    char_u	*పంక్తి;
    పూర్ణాంకం		icase;
    పూర్ణాంకం		i;

    ఒకవేళ (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	తిరిగి FALSE;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    లేకపోతే
#అయితే_ముగింపు
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    అయితే (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	ఎంపిక (when)
	{
	    స్థితి '*': try_సరిపోలిక = (*look == '*'); ఆపు;
	    స్థితి '!': try_సరిపోలిక = (*look == '!'); ఆపు;
	    అప్రమేయ: try_సరిపోలిక = (*look != '*'); ఆపు;
	}
	ఒకవేళ (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	ఒకవేళ (*look == '0')
	{
	    try_సరిపోలిక_మాట = try_సరిపోలిక;
	    ఒకవేళ (!పంక్తి_is_ఖాళీలు)
		try_సరిపోలిక = FALSE;
	    ++look;
	}
	లేకపోతే
	    try_సరిపోలిక_మాట = FALSE;

	// does it look like a control character?
	ఒకవేళ (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    ఒకవేళ (try_సరిపోలిక && keytyped == Ctrl_chr(look[1]))
		తిరిగి TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	లేకపోతే ఒకవేళ (*look == 'o')
	{
	    ఒకవేళ (try_సరిపోలిక && keytyped == కీలకం_OPEN_FORW)
		తిరిగి TRUE;
	    ++look;
	}
	లేకపోతే ఒకవేళ (*look == 'O')
	{
	    ఒకవేళ (try_సరిపోలిక && keytyped == కీలకం_OPEN_వెనుక)
		తిరిగి TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	లేకపోతే ఒకవేళ (*look == 'e')
	{
	    ఒకవేళ (try_సరిపోలిక && keytyped == 'e' && curwin->w_cursor.నిలువు >= 4)
	    {
		p = ml_get_curline();
		ఒకవేళ (skipwhite(p) == p + curwin->w_cursor.నిలువు - 4 &&
			STRNCMP(p + curwin->w_cursor.నిలువు - 4, "else", 4) == 0)
		    తిరిగి TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	లేకపోతే ఒకవేళ (*look == ':')
	{
	    ఒకవేళ (try_సరిపోలిక && keytyped == ':')
	    {
		p = ml_get_curline();
		ఒకవేళ (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    తిరిగి TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		ఒకవేళ (curwin->w_cursor.నిలువు > 2
			&& p[curwin->w_cursor.నిలువు - 1] == ':'
			&& p[curwin->w_cursor.నిలువు - 2] == ':')
		{
		    p[curwin->w_cursor.నిలువు - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.నిలువు - 1] = ':';
		    ఒకవేళ (i)
			తిరిగి TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	లేకపోతే ఒకవేళ (*look == '<')
	{
	    ఒకవేళ (try_సరిపోలిక)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		ఒకవేళ (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != ఖాళీ
						       && keytyped == look[1])
		    తిరిగి TRUE;

		ఒకవేళ (keytyped == get_special_కీలకం_సంకేతం(look + 1))
		    తిరిగి TRUE;
	    }
	    అయితే (*look && *look != '>')
		look++;
	    అయితే (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	లేకపోతే ఒకవేళ (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    ఒకవేళ (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    లేకపోతే
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    ఒకవేళ (p == ఖాళీ)
		p = look + STRLEN(look);
	    ఒకవేళ ((try_సరిపోలిక || try_సరిపోలిక_మాట)
		    && curwin->w_cursor.నిలువు >= (colnr_T)(p - look))
	    {
		పూర్ణాంకం		సరిపోలిక = FALSE;

		ఒకవేళ (keytyped == కీలకం_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    పంక్తి = ml_get_curline();
		    ఒకవేళ (has_mbyte)
		    {
			char_u	*n;

			కొరకు (s = పంక్తి + curwin->w_cursor.నిలువు; s > పంక్తి; s = n)
			{
			    n = mb_prevptr(పంక్తి, s);
			    ఒకవేళ (!vim_iswordp(n))
				ఆపు;
			}
		    }
		    లేకపోతే
			కొరకు (s = పంక్తి + curwin->w_cursor.నిలువు; s > పంక్తి; --s)
			    ఒకవేళ (!vim_iswordc(s[-1]))
				ఆపు;
		    ఒకవేళ (s + (p - look) <= పంక్తి + curwin->w_cursor.నిలువు
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			సరిపోలిక = TRUE;
		}
		లేకపోతే
		    // TODO: multi-byte
		    ఒకవేళ (keytyped == (పూర్ణాంకం)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((పూర్ణాంకం)p[-1])))
		{
		    పంక్తి = ml_get_cursor();
		    ఒకవేళ ((curwin->w_cursor.నిలువు == (colnr_T)(p - look)
				|| !vim_iswordc(పంక్తి[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(పంక్తి - (p - look), look, p - look)
				: STRNCMP(పంక్తి - (p - look), look, p - look))
									 == 0)
			సరిపోలిక = TRUE;
		}
		ఒకవేళ (సరిపోలిక && try_సరిపోలిక_మాట && !try_సరిపోలిక)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    ఒకవేళ (getwhitecols_curline() !=
				     (పూర్ణాంకం)(curwin->w_cursor.నిలువు - (p - look)))
			సరిపోలిక = FALSE;
		}
		ఒకవేళ (సరిపోలిక)
		    తిరిగి TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	లేకపోతే
	{
	    ఒకవేళ (try_సరిపోలిక && *look == keytyped)
		తిరిగి TRUE;
	    ఒకవేళ (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_గమ్యం_option_part(look);
    }
    తిరిగి FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    శూన్య
do_c_సమాసం_indent(శూన్య)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (*curbuf->b_p_inde != NUL)
	fixthisline(get_సమాసం_indent);
    లేకపోతే
#అయితే_ముగింపు
	fixthisline(get_c_indent);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    శూన్య
f_cindent(typval_T *argvars వాడని, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ఒకవేళ (లోపల_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	తిరిగి;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ఒకవేళ (lnum >= 1 && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_సంఖ్య = get_c_indent();
	curwin->w_cursor = pos;
    }
    లేకపోతే
	rettv->vval.v_సంఖ్య = -1;
}
#అయితే_ముగింపు
