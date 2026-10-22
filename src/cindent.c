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

#ଅନ୍ତର୍ଭାଵ "vim.h"

// values for the "lookfor" state
#define LOOKFOR_INITIAL		0
#define LOOKFOR_IF		1
#define LOOKFOR_DO		2
#define LOOKFOR_CASE		3
#define LOOKFOR_ANY		4
#define LOOKFOR_TERM		5
#define LOOKFOR_UNTERM		6
#define LOOKFOR_SCOPEDECL	7
#define LOOKFOR_NOBREAK		8
#define LOOKFOR_CPP_BASECLASS	9
#define LOOKFOR_ENUM_OR_INIT	10
#define LOOKFOR_JS_ଚାବି		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_is_cinword(char_u *ଧାଡ଼ି)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cinw_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    cinw_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    ଯଦି (cinw_buf == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ଧାଡ଼ି = skipwhite(ଧାଡ଼ି);
    ପାଇଁ (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	ଯଦି (STRNCMP(ଧାଡ଼ି, cinw_buf, len) == 0
		&& (!vim_iswordc(ଧାଡ଼ି[len]) || !vim_iswordc(ଧାଡ଼ି[len - 1])))
	{
	    retval = TRUE;
	    ଭାଙ୍ଗ;
	}
    }
    vim_free(cinw_buf);
    ଫେରନ୍ତୁ retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    ସ୍ଥିର char_u *
skip_string(char_u *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    // We loop, because strings may be concatenated: "date""time".
    ପାଇଁ ( ; ; ++p)
    {
	ଯଦି (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    ଯଦି (p[1] == NUL)		    // ' at end of line
		ଭାଙ୍ଗ;
	    i = 2;
	    ଯଦି (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		ଯେତେବେଳେ (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    ଯଦି (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		ଜାରି;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (p[0] == '"')		    // start of string
	{
	    ପାଇଁ (++p; p[0]; ++p)
	    {
		ଯଦି (p[0] == '\\' && p[1] != NUL)
		    ++p;
		ଅନ୍ୟଥା ଯଦି (p[0] == '"')	    // end of string
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (p[0] == '"')
		ଜାରି; // continue for another string
	}
	ଅନ୍ୟଥା ଯଦି (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    ଯଦି (paren != ଖାଲି)
	    {
		size_t delim_len = paren - delim;

		ପାଇଁ (p += 3; *p; ++p)
		    ଯଦି (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			ଭାଙ୍ଗ;
		    }
		ଯଦି (p[0] == '"')
		    ଜାରି; // continue for another string
	    }
	}
	ଭାଙ୍ଗ;				    // no string found
    }
    ଯଦି (!*p)
	--p;				    // backup from NUL
    ଫେରନ୍ତୁ p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_pos_in_string(char_u *ଧାଡ଼ି, colnr_T col)
{
    char_u *p;

    ପାଇଁ (p = ଧାଡ଼ି; *p && (colnr_T)(p - ଧାଡ଼ି) < col; ++p)
	p = skip_string(p);
    ଫେରନ୍ତୁ !((colnr_T)(p - ଧାଡ଼ି) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_linecomment(char_u *ଧାଡ଼ି)
{
    char_u  *p;

    p = ଧାଡ଼ି;
    // skip Lispish one-line comments
    ଯଦି (curbuf->b_p_lisp)
    {
	ଯଦି (vim_strchr(p, ';') != ଖାଲି) // there may be comments
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_str = FALSE;	// inside of string

	    p = ଧାଡ଼ି;		// scan from start
	    ଯେତେବେଳେ ((p = vim_strpbrk(p, (char_u *)"\";")) != ଖାଲି)
	    {
		ଯଦି (*p == '"')
		{
		    ଯଦି (in_str)
		    {
			ଯଦି (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    ଅନ୍ୟଥା ଯଦି (p == ଧାଡ଼ି || ((p - ଧାଡ଼ି) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		ଅନ୍ୟଥା ଯଦି (!in_str && ((p - ଧାଡ଼ି) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(ଧାଡ଼ି, (colnr_T)(p - ଧାଡ଼ି)))
		    ଭାଙ୍ଗ;	// found!
		++p;
	    }
	}
	ଅନ୍ୟଥା
	    p = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	ପାଇଁ ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    ଯଦି (p[0] == '/' && p[1] == '/'
				 && (p == ଧାଡ଼ି || p[-1] != '*' || p[2] != '*'))
		ଭାଙ୍ଗ;
	}
	ଯଦି (*p == NUL)
	    p = ଖାଲି;
    }

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ MAXCOL;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    ସ୍ଥିର pos_T *
ind_find_start_comment(ଶୂନ୍ୟ)	    // XXX
{
    ଫେରନ୍ତୁ find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_maxcomment = ind_maxcomment;

    ପାଇଁ (;;)
    {
	pos = findmatchlimit(ଖାଲି, '*', FM_BACKWARD, cur_maxcomment);
	ଯଦି (pos == ଖାଲି)
	    ଭାଙ୍ଗ;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ଯଦି (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    ଭାଙ୍ଗ;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ଯଦି (cur_maxcomment <= 0)
	{
	    pos = ଖାଲି;
	    ଭାଙ୍ଗ;
	}
    }
    ଫେରନ୍ତୁ pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    ସ୍ଥିର pos_T *
find_start_rawstring(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_maxcomment = ind_maxcomment;

    ପାଇଁ (;;)
    {
	pos = findmatchlimit(ଖାଲି, 'R', FM_BACKWARD, cur_maxcomment);
	ଯଦି (pos == ଖାଲି)
	    ଭାଙ୍ଗ;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ଯଦି (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    ଭାଙ୍ଗ;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ଯଦି (cur_maxcomment <= 0)
	{
	    pos = ଖାଲି;
	    ଭାଙ୍ଗ;
	}
    }
    ଫେରନ୍ତୁ pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    ସ୍ଥିର pos_T *
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    ସ୍ଥିର pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    ଯଦି (comment_pos != ଖାଲି)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    ଯଦି (comment_pos == ଖାଲି || (rs_pos != ଖାଲି
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	ଯଦି (is_raw != ଖାଲି && rs_pos != ଖାଲି)
	    *is_raw = rs_pos->lnum;
	ଫେରନ୍ତୁ rs_pos;
    }
    ଫେରନ୍ତୁ comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cindent_on(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    ସ୍ଥିର char_u *
cin_skipcomment(char_u *s)
{
    ଯେତେବେଳେ (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	ଯଦି (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (*s != '/')
	    ଭାଙ୍ଗ;
	++s;
	ଯଦି (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (*s != '*')
	    ଭାଙ୍ଗ;
	ପାଇଁ (++s; *s; ++s)	// skip slash-star comment
	    ଯଦି (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		ଭାଙ୍ଗ;
	    }
    }
    ଫେରନ୍ତୁ s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_nocode(char_u *s)
{
    ଫେରନ୍ତୁ *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_iscomment(char_u *p)
{
    ଫେରନ୍ତୁ (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_islinecomment(char_u *p)
{
    ଫେରନ୍ତୁ (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    ସ୍ଥିର pos_T *
find_ଧାଡ଼ି_comment(ଶୂନ୍ୟ) // XXX
{
    ସ୍ଥିର pos_T pos;
    char_u	 *ଧାଡ଼ି;
    char_u	 *p;

    pos = curwin->w_cursor;
    ଯେତେବେଳେ (--pos.lnum > 0)
    {
	ଧାଡ଼ି = ml_get(pos.lnum);
	p = skipwhite(ଧାଡ଼ି);
	ଯଦି (cin_islinecomment(p))
	{
	    pos.col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି);
	    ଫେରନ୍ତୁ &pos;
	}
	ଯଦି (*p != NUL)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_has_js_ଚାବି(char_u *text)
{
    char_u *s = skipwhite(text);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    quote = -1;

    ଯଦି (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    ଯଦି (!vim_isIDc(*s))	    // need at least one ID character
	ଫେରନ୍ତୁ FALSE;

    ଯେତେବେଳେ (vim_isIDc(*s))
	++s;
    ଯଦି (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    ଫେରନ୍ତୁ (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_islabel_skip(char_u **s)
{
    ଯଦି (!vim_isIDc(**s))	    // need at least one ID character
	ଫେରନ୍ତୁ FALSE;

    ଯେତେବେଳେ (vim_isIDc(**s))
    {
	ଯଦି (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	ଅନ୍ୟଥା
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    ଫେରନ୍ତୁ (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    ଯଦି (cinsd_buf == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cinsd_len, ",");
	ଯଦି (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    ଯଦି (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
    }

    vim_free(cinsd_buf);
    ଫେରନ୍ତୁ found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_ispreproc(char_u *s)
{
    ଯଦି (*skipwhite(s) == '#')
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *amount)
{
    char_u	*ଧାଡ଼ି = *pp;
    linenr_T	lnum = *lnump;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		candidate_amount = *amount;

    ଯଦି (*ଧାଡ଼ି != NUL && ଧାଡ଼ି[STRLEN(ଧାଡ଼ି) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    ପାଇଁ (;;)
    {
	ଯଦି (cin_ispreproc(ଧାଡ଼ି))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (lnum == 1)
	    ଭାଙ୍ଗ;
	ଧାଡ଼ି = ml_get(--lnum);
	ଯଦି (*ଧାଡ଼ି == NUL || ଧାଡ଼ି[STRLEN(ଧାଡ଼ି) - 1] != '\\')
	    ଭାଙ୍ଗ;
    }

    ଯଦି (lnum != *lnump)
	*pp = ml_get(*lnump);
    ଯଦି (retval)
	*amount = candidate_amount;
    ଫେରନ୍ତୁ retval;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_iselse(
    char_u  *p)
{
    ଯଦି (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    ଫେରନ୍ତୁ (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_isterminated(
    char_u	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		incl_open,	// include '{' at the end as terminator
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_else = FALSE;

    s = cin_skipcomment(s);

    ଯଦି (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    ଯଦି (!found_start)
	is_else = cin_iselse(s);

    ଯେତେବେଳେ (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	ଯଦି (*s == '}' && n_open > 0)
	    --n_open;
	ଯଦି ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    ଫେରନ୍ତୁ *s;
	ଅନ୍ୟଥା ଯଦି (*s == '{')
	{
	    ଯଦି (incl_open && cin_nocode(s + 1))
		ଫେରନ୍ତୁ *s;
	    ଅନ୍ୟଥା
		++n_open;
	}

	ଯଦି (*s)
	    s++;
    }
    ଫେରନ୍ତୁ found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_starts_with(char_u *s, ଅକ୍ଷର *word)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(word);

    ଫେରନ୍ତୁ (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_isdefault(char_u *s)
{
    ଫେରନ୍ତୁ (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_iscase(
    char_u *s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    ଯଦି (cin_starts_with(s, "case"))
    {
	ପାଇଁ (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    ଯଦି (*s == NUL)
		ଭାଙ୍ଗ;
	    ଯଦି (*s == ':')
	    {
		ଯଦି (s[1] == ':')	// skip over "::" for C++
		    ++s;
		ଅନ୍ୟଥା
		    ଫେରନ୍ତୁ TRUE;
	    }
	    ଯଦି (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    ଅନ୍ୟଥା ଯଦି (*s == '/' && (s[1] == '*' || s[1] == '/'))
		ଫେରନ୍ତୁ FALSE;		// stop at comment
	    ଅନ୍ୟଥା ଯଦି (*s == '"')
	    {
		// JS etc.
		ଯଦି (strict)
		    ଫେରନ୍ତୁ FALSE;		// stop at string
		ଅନ୍ୟଥା
		    ଫେରନ୍ତୁ TRUE;
	    }
	}
	ଫେରନ୍ତୁ FALSE;
    }

    ଯଦି (cin_isdefault(s))
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_islabel(ଶୂନ୍ୟ)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    ଯଦି (cin_isdefault(s))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (cin_isscopedecl(s))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!cin_islabel_skip(&s))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (ind_find_start_CORS(ଖାଲି))
	ଫେରନ୍ତୁ FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*ଧାଡ଼ି;

    cursor_save = curwin->w_cursor;
    ଯେତେବେଳେ (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	ଯଦି ((trypos = ind_find_start_CORS(ଖାଲି)) != ଖାଲି) // XXX
	    curwin->w_cursor = *trypos;

	ଧାଡ଼ି = ml_get_curline();
	ଯଦି (cin_ispreproc(ଧାଡ଼ି))	// ignore #defines, #if, etc.
	    ଜାରି;
	ଯଦି (*(ଧାଡ଼ି = cin_skipcomment(ଧାଡ଼ି)) == NUL)
	    ଜାରି;

	curwin->w_cursor = cursor_save;
	ଯଦି (cin_isterminated(ଧାଡ଼ି, TRUE, FALSE)
		|| cin_isscopedecl(ଧାଡ଼ି)
		|| cin_iscase(ଧାଡ଼ି, TRUE)
		|| (cin_islabel_skip(&ଧାଡ଼ି) && cin_nocode(ଧାଡ଼ି)))
	    ଫେରନ୍ତୁ TRUE;
	ଫେରନ୍ତୁ FALSE;
    }
    curwin->w_cursor = cursor_save;
    ଫେରନ୍ତୁ TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(find);

    ଯେତେବେଳେ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ଯଦି (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    ଯଦି (cin_nocode(r))
		ଫେରନ୍ତୁ TRUE;
	}
	ଯଦି (*p != NUL)
	    ++p;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    ସ୍ଥିର char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = ଖାଲି, *p = s;
    କର
    {
	r = p;
	p = cin_skipcomment(p);
	ଯଦି (*p)
	    p = skip_string(p);
    } ଯେତେବେଳେ (p != r);
    ଫେରନ୍ତୁ p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = ଖାଲି;

    ଯେତେବେଳେ (*p)
    {
	ଯଦି (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	ଅନ୍ୟଥା
	    p = cin_skip_comment_and_string(p + 1);
    }
    ଯଦି (!r)
	ଫେରନ୍ତୁ FALSE;
    p = r; // p points now after '=' or "return"

    ଯଦି (cin_nocode(p))
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (*p == '&')
	p = cin_skipcomment(p + 1);

    ଯଦି (*p == '(') // skip a typecast
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_ଗଣନା = 1;
	କର
	{
	    p = cin_skip_comment_and_string(p + 1);
	    ଯଦି (cin_nocode(p))
		ଫେରନ୍ତୁ TRUE;
	    open_ଗଣନା += (*p == '(') - (*p == ')');
	} ଯେତେବେଳେ (open_ଗଣନା);
	p = cin_skipcomment(p + 1);
	ଯଦି (cin_nocode(p))
	    ଫେରନ୍ତୁ TRUE;
    }

    ଯେତେବେଳେ (*p == '{')
	p = cin_skipcomment(p + 1);
    ଫେରନ୍ତୁ cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_isinit(ଶୂନ୍ୟ)
{
    char_u	*s;
    ସ୍ଥିର ଅକ୍ଷର *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    ଯଦି (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, l;

	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(skip); ++i)
	{
	    l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(skip[i]);
	    ଯଦି (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (l != 0)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (cin_starts_with(s, "enum"))
	ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_name = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    ଯେତେବେଳେ ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    ଯଦି (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	ଯେତେବେଳେ (*p != NUL)
	{
	    ଯଦି (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    ଅନ୍ୟଥା ଯଦି (*p == '{')
	    {
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		ଯଦି (has_name)
		    ଫେରନ୍ତୁ FALSE; // word character after skipping past name
		++p;
	    }
	    ଅନ୍ୟଥା ଯଦି (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		ଯଦି (!has_name_start || has_name)
		    ଫେରନ୍ତୁ FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    ଅନ୍ୟଥା
	    {
		ଫେରନ୍ତୁ FALSE;
	    }
	}
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    ଯଦି (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	ଯେତେବେଳେ (*p != NUL)
	{
	    ଯଦି (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    ଅନ୍ୟଥା ଯଦି (*p == '{')
	    {
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		ଯଦି (has_string_literal)
		    ଫେରନ୍ତୁ FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    ଅନ୍ୟଥା ଯଦି (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		ଯଦି (has_string_literal)
		    ଫେରନ୍ତୁ FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    ଅନ୍ୟଥା
	    {
		ଫେରନ୍ତୁ FALSE;
	    }
	}
	ଫେରନ୍ତୁ has_string_literal ? TRUE : FALSE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    ସ୍ଥିର char_u *
after_label(char_u *l)
{
    ପାଇଁ ( ; *l; ++l)
    {
	ଯଦି (*l == ':')
	{
	    ଯଦି (l[1] == ':')	    // skip over "::" for C++
		++l;
	    ଅନ୍ୟଥା ଯଦି (!cin_iscase(l + 1, FALSE))
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    ଯଦି (*l == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    l = cin_skipcomment(l + 1);
    ଯଦି (*l == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, ଖାଲି, ଖାଲି, 0);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    ଯଦି (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	ଯଦି (l == ଖାଲି)		// just in case
	    l = ml_get_curline();
    }
    ଅନ୍ୟଥା
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    ଫେରନ୍ତୁ amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_first_id_amount(ଶୂନ୍ୟ)
{
    char_u	*ଧାଡ଼ି, *p, *s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    pos_T	fp;
    colnr_T	col;

    ଧାଡ଼ି = ml_get_curline();
    p = skipwhite(ଧାଡ଼ି);
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(skiptowhite(p) - p);
    ଯଦି (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(skiptowhite(p) - p);
    }
    ଯଦି (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    ଅନ୍ୟଥା ଯଦି (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    ଅନ୍ୟଥା ଯଦି ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	ଯଦି ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    ପାଇଁ (len = 0; vim_isIDc(p[len]); ++len)
	;
    ଯଦି (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	ଫେରନ୍ତୁ 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - ଧାଡ଼ି);
    getvcol(curwin, &fp, &col, ଖାଲି, ଖାଲି, 0);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)col;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*ଧାଡ଼ି;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    ଯଦି (lnum > 1)
    {
	ଧାଡ଼ି = ml_get(lnum - 1);
	ଯଦି (*ଧାଡ଼ି != NUL && ଧାଡ଼ି[STRLEN(ଧାଡ଼ି) - 1] == '\\')
	    ଫେରନ୍ତୁ -1;
    }

    ଧାଡ଼ି = s = ml_get(lnum);
    ଯେତେବେଳେ (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == ଖାଲି)
    {
	ଯଦି (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ଅନ୍ୟଥା
	    ++s;
    }
    ଯଦି (*s != '=')
	ଫେରନ୍ତୁ 0;

    s = skipwhite(s + 1);
    ଯଦି (cin_nocode(s))
	ଫେରନ୍ତୁ 0;

    ଯଦି (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - ଧାଡ଼ି);
    getvcol(curwin, &fp, &col, ଖାଲି, ଖାଲି, 0);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_skip2pos(pos_T *trypos)
{
    char_u	*ଧାଡ଼ି;
    char_u	*p;
    char_u	*new_p;

    p = ଧାଡ଼ି = ml_get(trypos->lnum);
    ଯେତେବେଳେ (*p && (colnr_T)(p - ଧାଡ଼ି) < trypos->col)
    {
	ଯଦି (cin_iscomment(p))
	    p = cin_skipcomment(p);
	ଅନ୍ୟଥା
	{
	    new_p = skip_string(p);
	    ଯଦି (new_p == p)
		++p;
	    ଅନ୍ୟଥା
		p = new_p;
	}
    }
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି);
}

    ସ୍ଥିର pos_T *
find_match_char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ସ୍ଥିର pos_T pos_copy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    ଯଦି ((trypos = findmatchlimit(ଖାଲି, c, FM_SKIPCOMM, ind_maxp_wk)) != ଖାଲି)
    {
	// check if the ( is in a // comment
	ଯଦି ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(cursor_save.lnum - trypos->lnum);
	    ଯଦି (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		ଯାଅ retry;
	    }
	    trypos = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    ଯଦି ((trypos_wk = ind_find_start_CORS(ଖାଲି)) != ଖାଲି) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(cursor_save.lnum
			- trypos_wk->lnum);
		ଯଦି (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    ଯାଅ retry;
		}
		trypos = ଖାଲି;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    ଫେରନ୍ତୁ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    ସ୍ଥିର pos_T *
find_match_paren(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ind_maxparen)	// XXX
{
    ଫେରନ୍ତୁ find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_last_paren(char_u *l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		open_ଗଣନା = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    ପାଇଁ (i = 0; l[i] != NUL; i++)
    {
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(cin_skipcomment(l + i) - l); // ignore parens in comments
	ଯଦି (l[i] == NUL)
	    ଭାଙ୍ଗ;
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(skip_string(l + i) - l);    // ignore parens in quotes
	ଯଦି (l[i] == start)
	    ++open_ଗଣନା;
	ଅନ୍ୟଥା ଯଦି (l[i] == end)
	{
	    ଯଦି (open_ଗଣନା > 0)
		--open_ଗଣନା;
	    ଅନ୍ୟଥା
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    ଫେରନ୍ତୁ retval;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;
    pos_T	*trypos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		just_started = TRUE;

    ଯଦି (sp == ଖାଲି)
	s = ml_get(lnum);
    ଅନ୍ୟଥା
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    ଯଦି (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != ଖାଲି)
    {
	lnum = trypos->lnum;
	ଯଦି (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    ଫେରନ୍ତୁ FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    ଯଦି (cin_ispreproc(s))
	ଫେରନ୍ତୁ FALSE;

    ଯେତେବେଳେ (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	ଯଦି (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ଅନ୍ୟଥା ଯଦି (*s == ':')
	{
	    ଯଦି (*(s + 1) == ':')
		s += 2;
	    ଅନ୍ୟଥା
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		ଫେରନ୍ତୁ FALSE;
	}
	ଅନ୍ୟଥା
	    ++s;
    }
    ଯଦି (*s != '(')
	ଫେରନ୍ତୁ FALSE;		// ';', ' or "  before any () or no '('

    ଯେତେବେଳେ (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	ଯଦି (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    ଯଦି (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    ଯାଅ done;
	}
	ଯଦି ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    ପାଇଁ (;;)
	    {
		ଯଦି (lnum >= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		    ଭାଙ୍ଗ;
		s = ml_get(++lnum);
		ଯଦି (!cin_ispreproc(s))
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (lnum >= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ଭାଙ୍ଗ;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    ଯଦି (!just_started && (!comma && *s != ',' && *s != ')'))
		ଭାଙ୍ଗ;
	    just_started = FALSE;
	}
	ଅନ୍ୟଥା ଯଦି (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ଅନ୍ୟଥା
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    ଯଦି (lnum != first_lnum && sp != ଖାଲି)
	*sp = ml_get(first_lnum);

    ଫେରନ୍ତୁ retval;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_isif(char_u *p)
{
    ଫେରନ୍ତୁ (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_isdo(char_u *p)
{
    ଫେରନ୍ତୁ (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;

    p = cin_skipcomment(p);
    ଯଦି (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    ଯଦି (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	ଯେତେବେଳେ (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	ଯଦି ((trypos = findmatchlimit(ଖାଲି, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != ଖାଲି
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    ଫେରନ୍ତୁ retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_is_if_for_while_before_offset(char_u *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *poffset)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = *poffset;

    ଯଦି (offset-- < 2)
	ଫେରନ୍ତୁ 0;
    ଯେତେବେଳେ (offset > 2 && VIM_ISWHITE(ଧାଡ଼ି[offset]))
	--offset;

    offset -= 1;
    ଯଦି (STRNCMP(ଧାଡ଼ି + offset, "if", 2) == 0)
	ଯାଅ probablyFound;

    ଯଦି (offset >= 1)
    {
	offset -= 1;
	ଯଦି (STRNCMP(ଧାଡ଼ି + offset, "for", 3) == 0)
	    ଯାଅ probablyFound;

	ଯଦି (offset >= 2)
	{
	    offset -= 2;
	    ଯଦି (STRNCMP(ଧାଡ଼ି + offset, "while", 5) == 0)
		ଯାଅ probablyFound;
	}
    }
    ଫେରନ୍ତୁ 0;

probablyFound:
    ଯଦି (!offset || !vim_isIDc(ଧାଡ଼ି[offset - 1]))
    {
	*poffset = offset;
	ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_iswhileofdo_end(ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminated)
{
    char_u	*ଧାଡ଼ି;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (terminated != ';')	// there must be a ';' at the end
	ଫେରନ୍ତୁ FALSE;

    p = ଧାଡ଼ି = ml_get_curline();
    ଯେତେବେଳେ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ଯଦି (*p == ')')
	{
	    s = skipwhite(p + 1);
	    ଯଦି (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		ଯଦି (trypos != ଖାଲି)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    ଯଦି (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    ଯଦି (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			ଫେରନ୍ତୁ TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		ଧାଡ଼ି = ml_get_curline();
		p = ଧାଡ଼ି + i;
	    }
	}
	ଯଦି (*p != NUL)
	    ++p;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_isbreak(char_u *p)
{
    ଫେରନ୍ତୁ (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*ଧାଡ଼ି = ml_get_curline();

    ଯଦି (pos->lnum <= lnum)
	ଫେରନ୍ତୁ cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(ଧାଡ଼ି);
    ଯଦି (*s == '#')		// skip #define FOO x ? (x) : x
	ଫେରନ୍ତୁ FALSE;
    s = cin_skipcomment(s);
    ଯଦି (*s == NUL)
	ଫେରନ୍ତୁ FALSE;

    cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;

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
    ଯେତେବେଳେ (lnum > 1)
    {
	ଧାଡ଼ି = ml_get(lnum - 1);
	s = skipwhite(ଧାଡ଼ି);
	ଯଦି (*s == '#' || *s == NUL)
	    ଭାଙ୍ଗ;
	ଯେତେବେଳେ (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    ଯଦି (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		ଭାଙ୍ଗ;
	    ଯଦି (*s != NUL)
		++s;
	}
	ଯଦି (*s != NUL)
	    ଭାଙ୍ଗ;
	--lnum;
    }

    pos->lnum = lnum;
    ଧାଡ଼ି = ml_get(lnum);
    s = ଧାଡ଼ି;
    ପାଇଁ (;;)
    {
	ଯଦି (*s == NUL)
	{
	    ଯଦି (lnum == curwin->w_cursor.lnum)
		ଭାଙ୍ଗ;
	    // Continue in the cursor line.
	    ଧାଡ଼ି = ml_get(++lnum);
	    s = ଧାଡ଼ି;
	}
	ଯଦି (s == ଧାଡ଼ି)
	{
	    // don't recognize "case (foo):" as a baseclass
	    ଯଦି (cin_iscase(s, FALSE))
		ଭାଙ୍ଗ;
	    s = cin_skipcomment(ଧାଡ଼ି);
	    ଯଦି (*s == NUL)
		ଜାରି;
	}

	ଯଦି (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	ଅନ୍ୟଥା ଯଦି (s[0] == ':')
	{
	    ଯଦି (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    ଅନ୍ୟଥା ଯଦି (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    ଅନ୍ୟଥା
		s = cin_skipcomment(s + 1);
	}
	ଅନ୍ୟଥା ଯଦି ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    ଯଦି (*s == 'c')
		s = cin_skipcomment(s + 5);
	    ଅନ୍ୟଥା
		s = cin_skipcomment(s + 6);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    ଅନ୍ୟଥା ଯଦି (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		ଫେରନ୍ତୁ FALSE;
	    }
	    ଅନ୍ୟଥା ଯଦି (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    ଅନ୍ୟଥା ଯଦି (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		ଯଦି (cpp_base_class)
		    pos->col = (colnr_T)(s - ଧାଡ଼ି);
	    }

	    // When the line ends in a comma don't align with it.
	    ଯଦି (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    ଯଦି (cpp_base_class)
	pos->lnum = lnum;
    ଫେରନ୍ତୁ cpp_base_class;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_baseclass_amount(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    ଯଦି (col == 0)
    {
	amount = get_indent();
	ଯଦି (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != ଖାଲି)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	ଯଦି (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    ଅନ୍ୟଥା
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, ଖାଲି, ଖାଲି, 0);
	amount = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vcol;
    }
    ଯଦି (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    ଫେରନ୍ତୁ amount;
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

    ସ୍ଥିର pos_T *
find_start_brace(ଶୂନ୍ୟ)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    ସ୍ଥିର pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    ଯେତେବେଳେ ((trypos = findmatchlimit(ଖାଲି, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != ଖାଲି)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = ଖାଲି;
	// ignore the { if it's in a // or / *  * / comment
	ଯଦି ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(ଖାଲି)) == ଖାଲି) // XXX
	    ଭାଙ୍ଗ;
	ଯଦି (pos != ଖାଲି)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    ଫେରନ୍ତୁ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    ସ୍ଥିର pos_T *
find_match_paren_after_brace (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    ଯଦି (trypos == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    ଯଦି (tryposBrace != ଖାଲି
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = ଖାଲି;
    ଫେରନ୍ତୁ trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    ଯଦି (n > 0 && n < curbuf->b_ind_maxparen / 2)
	ଫେରନ୍ତୁ curbuf->b_ind_maxparen - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n;
    ଫେରନ୍ତୁ curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    ଶୂନ୍ୟ
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		divider;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fraction = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sw;
    long	t = get_sw_ମୂଲ୍ୟ(buf);

    // needed for cino-(, it will be multiplied by 2 again
    ଯଦି (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    ଅନ୍ୟଥା
	sw = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)t;

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
    buf->b_ind_first_open = 0;

    // Spaces from the prevailing indent a leftmost open brace should be
    // located.
    buf->b_ind_open_extra = 0;

    // Spaces from the matching open brace (real location for one at the left
    // edge; imaginary location from one that ends a line) the matching close
    // brace should be located.
    buf->b_ind_close_extra = 0;

    // Spaces from the edge of the line an open brace sitting in the leftmost
    // column is imagined to be.
    buf->b_ind_open_ବାମ_imag = 0;

    // Spaces jump labels should be shifted to the left if N is non-negative,
    // otherwise the jump label will be put to column 1.
    buf->b_ind_jump_label = -1;

    // Spaces from the switch() indent a "case xx" label should be located.
    buf->b_ind_case = sw;

    // Spaces from the "case xx:" code after a switch() should be located.
    buf->b_ind_case_code = sw;

    // Lineup break at end of case in switch() with case label.
    buf->b_ind_case_break = 0;

    // Spaces from the class declaration indent a scope declaration label
    // should be located.
    buf->b_ind_scopedecl = sw;

    // Spaces from the scope declaration label code should be located.
    buf->b_ind_scopedecl_code = sw;

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
    buf->b_ind_in_comment = 3;

    // Boolean: if non-zero, use b_ind_in_comment even if there is something
    // after the comment opener.
    buf->b_ind_in_comment2 = 0;

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

    ପାଇଁ (p = buf->b_p_cino; *p; )
    {
	l = p++;
	ଯଦି (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	ଯଦି (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((ଅକ୍ଷର *)++p);
	    ଯେତେବେଳେ (VIM_ISDIGIT(*p))
	    {
		++p;
		ଯଦି (divider)
		    divider *= 10;
		ଅନ୍ୟଥା
		    divider = 10;
	    }
	}
	ଯଦି (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    ଯଦି (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    ଅନ୍ୟଥା
	    {
		n *= sw;
		ଯଦି (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	ଯଦି (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	ବାଛ (*l)
	{
	    ସ୍ଥିତି '>': buf->b_ind_level = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'e': buf->b_ind_open_imag = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'n': buf->b_ind_no_brace = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'f': buf->b_ind_first_open = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '{': buf->b_ind_open_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '}': buf->b_ind_close_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '^': buf->b_ind_open_ବାମ_imag = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'L': buf->b_ind_jump_label = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି ':': buf->b_ind_case = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '=': buf->b_ind_case_code = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'b': buf->b_ind_case_break = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'p': buf->b_ind_param = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 't': buf->b_ind_func_type = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '/': buf->b_ind_comment = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'c': buf->b_ind_in_comment = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'C': buf->b_ind_in_comment2 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'i': buf->b_ind_cpp_baseclass = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '+': buf->b_ind_continuation = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '(': buf->b_ind_unclosed = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'u': buf->b_ind_unclosed2 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'U': buf->b_ind_unclosed_noignore = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'W': buf->b_ind_unclosed_wrapped = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'w': buf->b_ind_unclosed_whiteok = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'm': buf->b_ind_matching_paren = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'M': buf->b_ind_paren_prev = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି ')': buf->b_ind_maxparen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '*': buf->b_ind_maxcomment = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'g': buf->b_ind_scopedecl = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'h': buf->b_ind_scopedecl_code = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'j': buf->b_ind_java = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'J': buf->b_ind_js = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'l': buf->b_ind_keep_case_label = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '#': buf->b_ind_hash_comment = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'N': buf->b_ind_cpp_namespace = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'k': buf->b_ind_if_for_while = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'E': buf->b_ind_cpp_extern_c = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'P': buf->b_ind_pragma = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଭାଙ୍ଗ;
	}
	ଯଦି (*p == ',')
	    ++p;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_match(ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		elselevel;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		whilelevel;

    ଯଦି (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    ଅନ୍ୟଥା
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    ଯେତେବେଳେ (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	ଯଦି (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    ଯଦି (theirscope == ଖାଲି)
		ଭାଙ୍ଗ;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    ଯଦି (theirscope->lnum < ourscope)
		ଭାଙ୍ଗ;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    ଯଦି (theirscope->lnum > ourscope)
		ଜାରି;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    ଯଦି (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		ଯଦି (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    ଯଦି (!cin_isif(mightbeif))
			++elselevel;
		    ଜାରି;
		}

		// If it's an "if" decrement elselevel
		ଯଦି (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    ଯଦି (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    ଯଦି (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		ଜାରି;
	    }

	    // If it's a "do" decrement whilelevel
	    ଯଦି (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    ଯଦି (elselevel <= 0 && whilelevel <= 0)
		ଫେରନ୍ତୁ OK;
	}
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_c_indent(ଶୂନ୍ୟ)
{
    pos_T	cur_curpos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		amount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scope_amount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = ଖାଲି;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lookfor;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		whilelevel;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		iscase;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lookfor_break;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lookfor_cpp_namespace = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cont_amount = 0;    // amount for continuation line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		original_ଧାଡ଼ି_islabel;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		added_to_amount = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		js_cur_has_ଚାବି = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    ଯଦି (cur_curpos.lnum == 1)
	ଫେରନ୍ତୁ 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    ଯଦି (linecopy == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    ଯଦି ((State & MODE_ଭର୍ତ୍ତି)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_ଧାଡ଼ି_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    ଯଦି (comment_pos != ଖାଲି)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    ଯଦି (trypos != ଖାଲି && (comment_pos == ଖାଲି
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	ଯାଅ laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    ଯଦି (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	ଯଦି (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    ଯାଅ theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    ଯଦି (original_ଧାଡ଼ି_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	ଯାଅ theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    ଯଦି (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_ଧାଡ଼ି_comment(); // XXX
	ଯଦି (trypos == ଖାଲି && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    ଯଦି (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	ଯଦି (trypos  != ଖାଲି)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, ଖାଲି, ଖାଲି, 0);
	    amount = col;
	    ଯାଅ theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    ଯଦି (!cin_iscomment(theline) && comment_pos != ଖାଲି) // XXX
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lead_start_len = 2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lead_ମଧ୍ୟ_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_ମଧ୍ୟ[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lead_end_len;
	char_u	*p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	start_align = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	start_off = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, ଖାଲି, ଖାଲି, 0);
	amount = col;
	*lead_start = NUL;
	*lead_ମଧ୍ୟ = NUL;

	p = curbuf->b_p_com;
	ଯେତେବେଳେ (*p != NUL)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	align = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	off = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା what = 0;

	    ଯେତେବେଳେ (*p != NUL && *p != ':')
	    {
		ଯଦି (*p == COM_START || *p == COM_END || *p == COM_ମଧ୍ୟ)
		    what = *p++;
		ଅନ୍ୟଥା ଯଦି (*p == COM_ବାମ || *p == COM_ଡାହାଣ)
		    align = *p++;
		ଅନ୍ୟଥା ଯଦି (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		ଅନ୍ୟଥା
		    ++p;
	    }

	    ଯଦି (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    ଯଦି (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    ଅନ୍ୟଥା ଯଦି (what == COM_ମଧ୍ୟ)
	    {
		STRCPY(lead_ମଧ୍ୟ, lead_end);
		lead_ମଧ୍ୟ_len = lead_end_len;
	    }
	    ଅନ୍ୟଥା ଯଦି (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		ଯଦି (STRNCMP(theline, lead_ମଧ୍ୟ, lead_ମଧ୍ୟ_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    ଯଦି (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			ଯଦି (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			ଅନ୍ୟଥା ଯଦି (STRNCMP(look, lead_ମଧ୍ୟ,
							lead_ମଧ୍ୟ_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    ଭାଙ୍ଗ;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			ଅନ୍ୟଥା ଯଦି (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    ଜାରି;
		    }
		    ଯଦି (start_off != 0)
			amount += start_off;
		    ଅନ୍ୟଥା ଯଦି (start_align == COM_ଡାହାଣ)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_ମଧ୍ୟ);
		    ଭାଙ୍ଗ;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		ଯଦି (STRNCMP(theline, lead_ମଧ୍ୟ, lead_ମଧ୍ୟ_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    ଯଦି (off != 0)
			amount += off;
		    ଅନ୍ୟଥା ଯଦି (align == COM_ଡାହାଣ)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_ମଧ୍ୟ);
		    done = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	ଯଦି (done)
	    ;
	ଅନ୍ୟଥା ଯଦି (theline[0] == '*')
	    amount += 1;
	ଅନ୍ୟଥା
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    ପାଇଁ (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		ଯଦି (linewhite(lnum))		    // skip blank lines
		    ଜାରି;
		amount = get_indent_lnum(lnum);	    // XXX
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (amount == -1)			    // use the comment opener
	    {
		ଯଦି (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    ଯଦି (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, ଖାଲି, ଖାଲି, 0);
		amount = col;
		ଯଦି (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	ଯାଅ theend;
    }

    // Are we looking at a ']' that has a match?
    ଯଦି (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != ଖାଲି)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	ଯାଅ theend;
    }

    // Are we inside parentheses or braces?  XXX
    ଯଦି (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != ଖାଲି
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != ଖାଲି
	    || trypos != ଖାଲି)
    {
      ଯଦି (trypos != ଖାଲି && tryposBrace != ଖାଲି)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  ଯଦି (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = ଖାଲି;
	  ଅନ୍ୟଥା
	      tryposBrace = ଖାଲି;
      }

      ଯଦି (trypos != ଖାଲି)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	ଯଦି (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	ଅନ୍ୟଥା
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    ପାଇଁ (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		ଯଦି (cin_nocode(l))		// skip comment lines
		    ଜାରି;
		ଯଦି (cin_ispreproc_cont(&l, &lnum, &amount))
		    ଜାରି;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		ଯଦି ((trypos = ind_find_start_CORS(ଖାଲି)) != ଖାଲି)
		{
		    lnum = trypos->lnum + 1;
		    ଜାରି;
		}

		// XXX
		ଯଦି ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != ଖାଲି
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    ଯଦି (theline[0] == ')')
		    {
			ଯଦି (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    ଭାଙ୍ଗ;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	ଯଦି (amount == -1)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ignore_paren_col = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_if_for_while = 0;

	    ଯଦି (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *ଧାଡ଼ି;

		trypos = &our_paren_pos;
		କର
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} ଯେତେବେଳେ (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		ଧାଡ଼ି = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(ଧାଡ଼ି, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    ଯଦି (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *ଧାଡ଼ି;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		ଧାଡ଼ି = ml_get_curline();
		look_col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(look - ଧାଡ଼ି);
		curwin->w_cursor.col = look_col + 1;
		ଯଦି ((trypos = findmatchlimit(ଖାଲି, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != ଖାଲି
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    ଯଦି (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
						      && is_if_for_while == 0)
		    || (!curbuf->b_ind_unclosed_noignore && *look == '('
						    && ignore_paren_col == 0))
	    {
		// If we're looking at a close paren, line up right there;
		// otherwise, line up with the next (non-white) character.
		// When b_ind_unclosed_wrapped is set and the matching paren is
		// the last nonwhite character of the line, use either the
		// indent of the current line or the indentation of the next
		// outer paren and add b_ind_unclosed_wrapped (for very long
		// lines).
		ଯଦି (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    ଯଦି (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			ପାଇଁ (col = 0; col < our_paren_pos.col; ++col)
			{
			    ବାଛ (l[col])
			    {
				ସ୍ଥିତି '(':
				ସ୍ଥିତି '{': ++n;
					  ଭାଙ୍ଗ;

				ସ୍ଥିତି ')':
				ସ୍ଥିତି '}': ଯଦି (n > 1)
					      --n;
					  ଭାଙ୍ଗ;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    ଅନ୍ୟଥା ଯଦି (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    ଅନ୍ୟଥା
		    {
			col = our_paren_pos.col + 1;
			ଯେତେବେଳେ (VIM_ISWHITE(l[col]))
			    col++;
			ଯଦି (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			ଅନ୍ୟଥା
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		ଯଦି (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, ଖାଲି, ଖାଲି, 0);
		    ଯଦି (cur_amount > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)col)
			cur_amount = col;
		}
	    }

	    ଯଦି (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    ଅନ୍ୟଥା ଯଦି ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		ଯଦି (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    ଅନ୍ୟଥା
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		ଯେତେବେଳେ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    ବାଛ (*ml_get_pos(&our_paren_pos))
		    {
			ସ୍ଥିତି '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  ଭାଙ୍ଗ;
			ସ୍ଥିତି ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  ଭାଙ୍ଗ;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		ଯଦି (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		ଅନ୍ୟଥା
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    ଯଦି (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != ଖାଲି)
			amount += curbuf->b_ind_unclosed2;
		    ଅନ୍ୟଥା
		    {
			ଯଦି (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			ଅନ୍ୟଥା
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		ଯଦି (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	ଯଦି (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      ଅନ୍ୟଥା
      {
	// We are inside braces, there is a { before this line at the position
	// stored in tryposBrace.
	// Make a copy of tryposBrace, it may point to pos_copy inside
	// find_start_brace(), which may be changed somewhere.
	tryposCopy = *tryposBrace;
	tryposBrace = &tryposCopy;
	trypos = tryposBrace;
	ourscope = trypos->lnum;
	start = ml_get(ourscope);

	// Now figure out how indented the line is in general.
	// If the brace was at the start of the line, we use that;
	// otherwise, check out the indentation of the line as
	// a whole and then add the "imaginary indent" to that.
	look = skipwhite(start);
	ଯଦି (*look == '{')
	{
	    getvcol(curwin, trypos, &col, ଖାଲି, ଖାଲି, 0);
	    amount = col;
	    ଯଦି (*start == '{')
		start_brace = BRACE_IN_COL0;
	    ଅନ୍ୟଥା
		start_brace = BRACE_AT_START;
	}
	ଅନ୍ୟଥା
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    ଯଦି (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != ଖାଲି)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    ଯଦି ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    ଅନ୍ୟଥା ଯଦି (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    ଅନ୍ୟଥା
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	ଯଦି (curbuf->b_ind_js)
	    js_cur_has_ଚାବି = cin_has_js_ଚାବି(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	ଯଦି (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	ଅନ୍ୟଥା
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    ଯଦି (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    ଅନ୍ୟଥା ଯଦି (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    ଯଦି (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		ଯଦି (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    ଯାଅ theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    ଯଦି (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_ବାମ_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    ଯଦି (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    ଅନ୍ୟଥା ଯଦି (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		ଅନ୍ୟଥା
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    ଯଦି (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    ଯଦି (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    ଅନ୍ୟଥା ଯଦି (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    ପାଇଁ (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		ଯଦି (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    ଯଦି (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ଯଦି (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    ଯଦି (cont_amount > 0)
				amount = cont_amount;
			    ଅନ୍ୟଥା ଯଦି (!curbuf->b_ind_js)
				amount += ind_continuation;
			    ଭାଙ୍ଗ;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(ଖାଲି);
			ଯଦି (trypos != ଖାଲି)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    ଜାରି;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			ଯଦି (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    ଜାରି;

			ଯଦି (cin_nocode(l))
			    ଜାରି;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			ଯଦି (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    ଯଦି (terminated == ',')
				ଭାଙ୍ଗ;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    ଯଦି (terminated != ';' && cin_isinit())
				ଭାଙ୍ଗ;

			    // nothing useful found
			    ଯଦି (terminated == 0 || terminated == '{')
				ଜାରି;
			}

			ଯଦି (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = ଖାଲି;
			    ଯଦି (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    ଯଦି (trypos == ଖାଲି && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    ଯଦି (trypos != ଖାଲି)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				ଜାରି;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			ଯଦି (cont_amount > 0)
			    amount = cont_amount;
			ଅନ୍ୟଥା
			    amount += ind_continuation;
		    }
		    ଅନ୍ୟଥା ଯଦି (lookfor == LOOKFOR_UNTERM)
		    {
			ଯଦି (cont_amount > 0)
			    amount = cont_amount;
			ଅନ୍ୟଥା
			    amount += ind_continuation;
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    ଯଦି (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			ଯଦି (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    ଯଦି (curwin->w_cursor.lnum == ourscope)
				ଜାରି;

			    ଯଦି (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				ଭାଙ୍ଗ;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(ଖାଲି);
			    ଯଦି (trypos != ଖାଲି)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				ଜାରି;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    ଯଦି (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				ଜାରି;

			    // Finally the actual check for "namespace".
			    ଯଦି (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				ଭାଙ୍ଗ;
			    }
			    ଅନ୍ୟଥା ଯଦି (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				ଭାଙ୍ଗ;
			    }

			    ଯଦି (cin_nocode(l))
				ଜାରି;
			}
		    }
		    ଭାଙ୍ଗ;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		ଯଦି ((trypos = ind_find_start_CORS(&raw_string_start)) != ଖାଲି)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    ଜାରି;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		ଯଦି (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    ଯଦି (lookfor == LOOKFOR_CPP_BASECLASS)
			ଭାଙ୍ଗ;

		    // When looking for a "do" we are not interested in
		    // labels.
		    ଯଦି (whilelevel > 0)
			ଜାରି;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    ଯଦି (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ଯଦି (cont_amount > 0)
			    amount = cont_amount;
			ଅନ୍ୟଥା
			    amount += ind_continuation;
			ଭାଙ୍ଗ;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    ଯଦି (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			ଯଦି ((trypos = find_start_brace()) == ଖାଲି
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    ଭାଙ୍ଗ;
			}
			ଜାରି;
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
		    ଯଦି (lookfor == LOOKFOR_TERM)
		    {
			ଯଦି (n)
			    amount = n;

			ଯଦି (!lookfor_break)
			    ଭାଙ୍ଗ;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    ଯଦି (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			ଯଦି (l != ଖାଲି && cin_is_cinword(l))
			{
			    ଯଦି (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    ଅନ୍ୟଥା
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			ଭାଙ୍ଗ;
		    }

		    // Try to get the indent of a statement before the switch
		    // label.  If nothing is found, line up relative to the
		    // switch label.
		    //	    break;		<- may line up with this line
		    //	 case xx:
		    // ->   y = 1;
		    scope_amount = get_indent() + (iscase    // XXX
					? curbuf->b_ind_case_code
					: curbuf->b_ind_scopedecl_code);
		    lookfor = curbuf->b_ind_case_break
					      ? LOOKFOR_NOBREAK : LOOKFOR_ANY;
		    ଜାରି;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		ଯଦି (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    ଯଦି (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != ଖାଲି)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    ଜାରି;
		}

		// Ignore jump labels with nothing after them.
		ଯଦି (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    ଯଦି (l == ଖାଲି || cin_nocode(l))
			ଜାରି;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		ଯଦି (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    ଜାରି;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		ଯଦି (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		ଯଦି (n)
		{
		    ଯଦି (lookfor == LOOKFOR_UNTERM)
		    {
			ଯଦି (cont_amount > 0)
			    amount = cont_amount;
			ଅନ୍ୟଥା
			    amount += ind_continuation;
		    }
		    ଅନ୍ୟଥା ଯଦି (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			ଜାରି;
		    }
		    ଅନ୍ୟଥା
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା ଯଦି (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    ଯଦି (cin_isterminated(l, TRUE, FALSE))
			ଭାଙ୍ଗ;
		    ଅନ୍ୟଥା
			ଜାରି;
		}

		// What happens next depends on the line being terminated.
		// If terminated with a ',' only consider it terminating if
		// there is another unterminated statement behind, eg:
		//   123,
		//   sizeof
		//	  here
		// Otherwise check whether it is an enumeration or structure
		// initialisation (not indented) or a variable declaration
		// (indented).
		terminated = cin_isterminated(l, FALSE, TRUE);

		ଯଦି (js_cur_has_ଚାବି)
		{
		    js_cur_has_ଚାବି = 0; // only check the first line
		    ଯଦି (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_ଚାବି;
		    }
		}
		ଯଦି (lookfor == LOOKFOR_JS_ଚାବି && cin_has_js_ଚାବି(l))
		{
		    amount = get_indent();
		    ଭାଙ୍ଗ;
		}
		ଯଦି (lookfor == LOOKFOR_COMMA)
		{
		    ଯଦି (tryposBrace != ଖାଲି && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			ଭାଙ୍ଗ;
		    ଯଦି (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			ଭାଙ୍ଗ;
		    ଅନ୍ୟଥା
		    {
			amount = get_indent();
			ଯଦି (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    ଭାଙ୍ଗ;
		    }
		}

		ଯଦି (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    ଯଦି (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (ଶୂନ୍ୟ)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    ଯଦି (trypos != ଖାଲି && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = ଖାଲି;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    ଯଦି (trypos == ଖାଲି && terminated == ',')
		    {
			ଯଦି (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    ଯଦି (trypos != ଖାଲି)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			ଯଦି (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    ଜାରି;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    ଯଦି (terminated == ',')
		    {
			ଯେତେବେଳେ (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    ଯଦି (*l == NUL || l[STRLEN(l) - 1] != '\\')
				ଭାଙ୍ଗ;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    ଯଦି (curbuf->b_ind_js)
			cur_amount = get_indent();
		    ଅନ୍ୟଥା
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    ଯଦି (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			ଯଦି (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			ଯଦି (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    ଜାରି;
			}
			ଭାଙ୍ଗ;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    ଯଦି (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			ଯଦି (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    ଯଦି (cont_amount > 0)
				amount = cont_amount;
			    ଅନ୍ୟଥା
				amount += ind_continuation;
			    ଭାଙ୍ଗ;
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
			ଯଦି (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			ଯଦି (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    ଭାଙ୍ଗ;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			ଯଦି (cin_isdo(l))
			{
			    ଯଦି (whilelevel == 0)
				ଭାଙ୍ଗ;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ଯଦି (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    ଯଦି (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    ଯଦି ((trypos = find_start_brace()) == ଖାଲି
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ଭାଙ୍ଗ;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    ଅନ୍ୟଥା
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			ଯଦି (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    ଯଦି (terminated == ',')
				amount += ind_continuation;
			    ଭାଙ୍ଗ;
			}

			ଯଦି (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    ଯଦି (terminated == ',')
			    {
				ଯଦି (curbuf->b_ind_cpp_baseclass == 0)
				    ଭାଙ୍ଗ;

				lookfor = LOOKFOR_CPP_BASECLASS;
				ଜାରି;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    ଯଦି (amount > cur_amount)
				amount = cur_amount;
			}
			ଅନ୍ୟଥା
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(l);
			    ଯଦି (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				ଭାଙ୍ଗ;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    ଯଦି (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				ଯଦି (curbuf->b_ind_js)
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
				    ଯଦି (cin_iscomment(skipwhite(l)))
					ଭାଙ୍ଗ;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    ଯଦି (trypos != ଖାଲି)
				    {
					ଯଦି (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    ଭାଙ୍ଗ;
					}
					ourscope = trypos->lnum;
				    }
				}
				ଅନ୍ୟଥା
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    ଅନ୍ୟଥା
			    {
				ଯଦି (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				ଯଦି (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_ଚାବି
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		ଅନ୍ୟଥା ଯଦି (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    ଯଦି (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ଯଦି (cont_amount > 0)
			    amount = cont_amount;
			ଅନ୍ୟଥା
			    amount += ind_continuation;
			ଭାଙ୍ଗ;
		    }

		    ଯଦି (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			ଯଦି (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		ଅନ୍ୟଥା
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    ଯଦି (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			ଜାରି;
		    }

		    // Handle "do {" line.
		    ଯଦି (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			ଯଦି (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    ଜାରି;
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
		    ଯଦି (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ଯଦି (cont_amount > 0)
			    amount = cont_amount;
			ଅନ୍ୟଥା
			    amount += ind_continuation;
			ଭାଙ୍ଗ;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    ଯଦି (lookfor == LOOKFOR_TERM)
		    {
			ଯଦି (!lookfor_break && whilelevel == 0)
			    ଭାଙ୍ଗ;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    ଅନ୍ୟଥା
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			ଯଦି (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != ଖାଲି)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    ଯଦି (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				ଜାରି;
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

			ଯଦି (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			ଯଦି (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ଯଦି (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    ଯଦି ((trypos = find_start_brace()) == ଖାଲି
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ଭାଙ୍ଗ;
			    ଜାରି;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			ଯଦି (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != ଖାଲି)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    ଯଦି (*l == '}' || !cin_iselse(l))
				ଯାଅ term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      ଯଦି (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      ଯଦି (curbuf->b_ind_jump_label > 0 && original_ଧାଡ଼ି_islabel)
	  amount -= curbuf->b_ind_jump_label;

      ଯାଅ theend;
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

    ଯଦି (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	ଯାଅ theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    ଯଦି (cur_curpos.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == ଖାଲି
	    && vim_strchr(theline, '}') == ଖାଲି
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(ଖାଲି, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	ଯାଅ theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    ଯେତେବେଳେ (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	ଯଦି ((trypos = ind_find_start_CORS(ଖାଲି)) != ଖାଲି)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    ଜାରି;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	ଯଦି (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	ଯଦି (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    ଭାଙ୍ଗ;
	}

	// Skip preprocessor directives and blank lines.
	ଯଦି (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    ଜାରି;

	ଯଦି (cin_nocode(l))
	    ଜାରି;

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
	ଯଦି (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    ଯଦି (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != ଖାଲି)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    ଯେତେବେଳେ (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		ଯଦି (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    ଭାଙ୍ଗ;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    ଯଦି (amount == 0)
		amount = cin_first_id_amount();
	    ଯଦି (amount == 0)
		amount = ind_continuation;
	    ଭାଙ୍ଗ;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	ଯଦି (cin_isfuncdecl(ଖାଲି, cur_curpos.lnum, 0))  // XXX
	    ଭାଙ୍ଗ;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	ଯଦି (*skipwhite(l) == '}')
	    ଭାଙ୍ଗ;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	ଯଦି (cin_ends_in(l, (char_u *)"};"))
	    ଭାଙ୍ଗ;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	ଯଦି (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    ଭାଙ୍ଗ;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	ଯଦି (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    ଯେତେବେଳେ (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		ଯଦି (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		ଭାଙ୍ଗ;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	ଯଦି (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    ଭାଙ୍ଗ;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	ଯଦି (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    ଯଦି (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		ଭାଙ୍ଗ;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	ଯଦି ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != ଖାଲି)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	ଭାଙ୍ଗ;
    }

    // add extra indent for a comment
    ଯଦି (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    ଯଦି (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	ଯଦି (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    ଯଦି (cur_amount > 0)
		amount = cur_amount;
	    ଅନ୍ୟଥା ଯଦି (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    ଯଦି (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    ଫେରନ୍ତୁ amount;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
in_cinkeys(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keytyped,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		when,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_is_empty)
{
    char_u	*look;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		try_match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		try_match_word;
    char_u	*p;
    char_u	*ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		icase;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	ଫେରନ୍ତୁ FALSE;

#ifdef FEAT_EVAL
    ଯଦି (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    ଅନ୍ୟଥା
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    ଯେତେବେଳେ (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	ବାଛ (when)
	{
	    ସ୍ଥିତି '*': try_match = (*look == '*'); ଭାଙ୍ଗ;
	    ସ୍ଥିତି '!': try_match = (*look == '!'); ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: try_match = (*look != '*'); ଭାଙ୍ଗ;
	}
	ଯଦି (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	ଯଦି (*look == '0')
	{
	    try_match_word = try_match;
	    ଯଦି (!ଧାଡ଼ି_is_empty)
		try_match = FALSE;
	    ++look;
	}
	ଅନ୍ୟଥା
	    try_match_word = FALSE;

	// does it look like a control character?
	ଯଦି (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    ଯଦି (try_match && keytyped == Ctrl_chr(look[1]))
		ଫେରନ୍ତୁ TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	ଅନ୍ୟଥା ଯଦି (*look == 'o')
	{
	    ଯଦି (try_match && keytyped == ଚାବି_OPEN_FORW)
		ଫେରନ୍ତୁ TRUE;
	    ++look;
	}
	ଅନ୍ୟଥା ଯଦି (*look == 'O')
	{
	    ଯଦି (try_match && keytyped == ଚାବି_OPEN_BACK)
		ଫେରନ୍ତୁ TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	ଅନ୍ୟଥା ଯଦି (*look == 'e')
	{
	    ଯଦି (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		ଯଦି (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    ଫେରନ୍ତୁ TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	ଅନ୍ୟଥା ଯଦି (*look == ':')
	{
	    ଯଦି (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		ଯଦି (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    ଫେରନ୍ତୁ TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		ଯଦି (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    ଯଦି (i)
			ଫେରନ୍ତୁ TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	ଅନ୍ୟଥା ଯଦି (*look == '<')
	{
	    ଯଦି (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		ଯଦି (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != ଖାଲି
						       && keytyped == look[1])
		    ଫେରନ୍ତୁ TRUE;

		ଯଦି (keytyped == get_special_ଚାବି_code(look + 1))
		    ଫେରନ୍ତୁ TRUE;
	    }
	    ଯେତେବେଳେ (*look && *look != '>')
		look++;
	    ଯେତେବେଳେ (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	ଅନ୍ୟଥା ଯଦି (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    ଯଦି (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    ଅନ୍ୟଥା
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    ଯଦି (p == ଖାଲି)
		p = look + STRLEN(look);
	    ଯଦି ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match = FALSE;

		ଯଦି (keytyped == ଚାବି_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    ଧାଡ଼ି = ml_get_curline();
		    ଯଦି (has_mbyte)
		    {
			char_u	*n;

			ପାଇଁ (s = ଧାଡ଼ି + curwin->w_cursor.col; s > ଧାଡ଼ି; s = n)
			{
			    n = mb_prevptr(ଧାଡ଼ି, s);
			    ଯଦି (!vim_iswordp(n))
				ଭାଙ୍ଗ;
			}
		    }
		    ଅନ୍ୟଥା
			ପାଇଁ (s = ଧାଡ଼ି + curwin->w_cursor.col; s > ଧାଡ଼ି; --s)
			    ଯଦି (!vim_iswordc(s[-1]))
				ଭାଙ୍ଗ;
		    ଯଦି (s + (p - look) <= ଧାଡ଼ି + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		ଅନ୍ୟଥା
		    // TODO: multi-byte
		    ଯଦି (keytyped == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p[-1])))
		{
		    ଧାଡ଼ି = ml_get_cursor();
		    ଯଦି ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(ଧାଡ଼ି[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(ଧାଡ଼ି - (p - look), look, p - look)
				: STRNCMP(ଧାଡ଼ି - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		ଯଦି (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    ଯଦି (getwhitecols_curline() !=
				     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		ଯଦି (match)
		    ଫେରନ୍ତୁ TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	ଅନ୍ୟଥା
	{
	    ଯଦି (try_match && *look == keytyped)
		ଫେରନ୍ତୁ TRUE;
	    ଯଦି (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    ଶୂନ୍ୟ
do_c_ପଦସମଷ୍ଟି_indent(ଶୂନ୍ୟ)
{
#ifdef FEAT_EVAL
    ଯଦି (*curbuf->b_p_inde != NUL)
	fixthisline(get_ପଦସମଷ୍ଟି_indent);
    ଅନ୍ୟଥା
#endif
	fixthisline(get_c_indent);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    ଶୂନ୍ୟ
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ଯଦି (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ଯଦି (lnum >= 1 && lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_ସଂଖ୍ୟା = get_c_indent();
	curwin->w_cursor = pos;
    }
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = -1;
}
#endif
