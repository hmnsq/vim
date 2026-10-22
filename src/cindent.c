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

#அந்தர்பாவ "vim.h"

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
#define LOOKFOR_JS_சாவி		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    முழு
cin_is_cinword(char_u *வரி)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    முழு		cinw_len;
    முழு		retval = FALSE;
    முழு		len;

    cinw_len = (முழு)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    எனில் (cinw_buf == வெற்று)
	திரும்பு FALSE;

    வரி = skipwhite(வரி);
    க்கு (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	எனில் (STRNCMP(வரி, cinw_buf, len) == 0
		&& (!vim_iswordc(வரி[len]) || !vim_iswordc(வரி[len - 1])))
	{
	    retval = TRUE;
	    நிறுத்து;
	}
    }
    vim_free(cinw_buf);
    திரும்பு retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    நிலையான char_u *
skip_string(char_u *p)
{
    முழு	    i;

    // We loop, because strings may be concatenated: "date""time".
    க்கு ( ; ; ++p)
    {
	எனில் (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    எனில் (p[1] == NUL)		    // ' at end of line
		நிறுத்து;
	    i = 2;
	    எனில் (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		போது (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    எனில் (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		தொடர்;
	    }
	}
	இல்லையெனில் எனில் (p[0] == '"')		    // start of string
	{
	    க்கு (++p; p[0]; ++p)
	    {
		எனில் (p[0] == '\\' && p[1] != NUL)
		    ++p;
		இல்லையெனில் எனில் (p[0] == '"')	    // end of string
		    நிறுத்து;
	    }
	    எனில் (p[0] == '"')
		தொடர்; // continue for another string
	}
	இல்லையெனில் எனில் (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    எனில் (paren != வெற்று)
	    {
		size_t delim_len = paren - delim;

		க்கு (p += 3; *p; ++p)
		    எனில் (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			நிறுத்து;
		    }
		எனில் (p[0] == '"')
		    தொடர்; // continue for another string
	    }
	}
	நிறுத்து;				    // no string found
    }
    எனில் (!*p)
	--p;				    // backup from NUL
    திரும்பு p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    முழு
is_pos_in_string(char_u *வரி, colnr_T col)
{
    char_u *p;

    க்கு (p = வரி; *p && (colnr_T)(p - வரி) < col; ++p)
	p = skip_string(p);
    திரும்பு !((colnr_T)(p - வரி) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    முழு
check_linecomment(char_u *வரி)
{
    char_u  *p;

    p = வரி;
    // skip Lispish one-line comments
    எனில் (curbuf->b_p_lisp)
    {
	எனில் (vim_strchr(p, ';') != வெற்று) // there may be comments
	{
	    முழு in_str = FALSE;	// inside of string

	    p = வரி;		// scan from start
	    போது ((p = vim_strpbrk(p, (char_u *)"\";")) != வெற்று)
	    {
		எனில் (*p == '"')
		{
		    எனில் (in_str)
		    {
			எனில் (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    இல்லையெனில் எனில் (p == வரி || ((p - வரி) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		இல்லையெனில் எனில் (!in_str && ((p - வரி) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(வரி, (colnr_T)(p - வரி)))
		    நிறுத்து;	// found!
		++p;
	    }
	}
	இல்லையெனில்
	    p = வெற்று;
    }
    இல்லையெனில்
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	க்கு ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    எனில் (*p == NUL)
		நிறுத்து;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    எனில் (p[0] == '/' && p[1] == '/'
				 && (p == வரி || p[-1] != '*' || p[2] != '*'))
		நிறுத்து;
	}
	எனில் (*p == NUL)
	    p = வெற்று;
    }

    எனில் (p == வெற்று)
	திரும்பு MAXCOL;
    திரும்பு (முழு)(p - வரி);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    நிலையான pos_T *
ind_find_start_comment(வெறுமை)	    // XXX
{
    திரும்பு find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(முழு ind_maxcomment)	// XXX
{
    pos_T	*pos;
    முழு		cur_maxcomment = ind_maxcomment;

    க்கு (;;)
    {
	pos = findmatchlimit(வெற்று, '*', FM_BACKWARD, cur_maxcomment);
	எனில் (pos == வெற்று)
	    நிறுத்து;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	எனில் (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    நிறுத்து;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	எனில் (cur_maxcomment <= 0)
	{
	    pos = வெற்று;
	    நிறுத்து;
	}
    }
    திரும்பு pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    நிலையான pos_T *
find_start_rawstring(முழு ind_maxcomment)	// XXX
{
    pos_T	*pos;
    முழு		cur_maxcomment = ind_maxcomment;

    க்கு (;;)
    {
	pos = findmatchlimit(வெற்று, 'R', FM_BACKWARD, cur_maxcomment);
	எனில் (pos == வெற்று)
	    நிறுத்து;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	எனில் (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    நிறுத்து;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	எனில் (cur_maxcomment <= 0)
	{
	    pos = வெற்று;
	    நிறுத்து;
	}
    }
    திரும்பு pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    நிலையான pos_T *
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    நிலையான pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    எனில் (comment_pos != வெற்று)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    எனில் (comment_pos == வெற்று || (rs_pos != வெற்று
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	எனில் (is_raw != வெற்று && rs_pos != வெற்று)
	    *is_raw = rs_pos->lnum;
	திரும்பு rs_pos;
    }
    திரும்பு comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    முழு
cindent_on(வெறுமை)
{
    திரும்பு (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
வகைவரை கட்டமைப்பு {
    முழு	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    நிலையான char_u *
cin_skipcomment(char_u *s)
{
    போது (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	எனில் (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    நிறுத்து;
	}
	எனில் (*s != '/')
	    நிறுத்து;
	++s;
	எனில் (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    நிறுத்து;
	}
	எனில் (*s != '*')
	    நிறுத்து;
	க்கு (++s; *s; ++s)	// skip slash-star comment
	    எனில் (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		நிறுத்து;
	    }
    }
    திரும்பு s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    நிலையான முழு
cin_nocode(char_u *s)
{
    திரும்பு *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    நிலையான முழு
cin_iscomment(char_u *p)
{
    திரும்பு (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    நிலையான முழு
cin_islinecomment(char_u *p)
{
    திரும்பு (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    நிலையான pos_T *
find_வரி_comment(வெறுமை) // XXX
{
    நிலையான pos_T pos;
    char_u	 *வரி;
    char_u	 *p;

    pos = curwin->w_cursor;
    போது (--pos.lnum > 0)
    {
	வரி = ml_get(pos.lnum);
	p = skipwhite(வரி);
	எனில் (cin_islinecomment(p))
	{
	    pos.col = (முழு)(p - வரி);
	    திரும்பு &pos;
	}
	எனில் (*p != NUL)
	    நிறுத்து;
    }
    திரும்பு வெற்று;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    நிலையான முழு
cin_has_js_சாவி(char_u *text)
{
    char_u *s = skipwhite(text);
    முழு	    quote = -1;

    எனில் (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    எனில் (!vim_isIDc(*s))	    // need at least one ID character
	திரும்பு FALSE;

    போது (vim_isIDc(*s))
	++s;
    எனில் (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    திரும்பு (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    நிலையான முழு
cin_islabel_skip(char_u **s)
{
    எனில் (!vim_isIDc(**s))	    // need at least one ID character
	திரும்பு FALSE;

    போது (vim_isIDc(**s))
    {
	எனில் (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	இல்லையெனில்
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    திரும்பு (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    நிலையான முழு
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    முழு	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    எனில் (cinsd_buf == வெற்று)
	திரும்பு FALSE;

    க்கு (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (முழு)cinsd_len, ",");
	எனில் (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    எனில் (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		நிறுத்து;
	    }
	}
    }

    vim_free(cinsd_buf);
    திரும்பு found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    நிலையான முழு
cin_ispreproc(char_u *s)
{
    எனில் (*skipwhite(s) == '#')
	திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    நிலையான முழு
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, முழு *amount)
{
    char_u	*வரி = *pp;
    linenr_T	lnum = *lnump;
    முழு		retval = FALSE;
    முழு		candidate_amount = *amount;

    எனில் (*வரி != NUL && வரி[STRLEN(வரி) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    க்கு (;;)
    {
	எனில் (cin_ispreproc(வரி))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    நிறுத்து;
	}
	எனில் (lnum == 1)
	    நிறுத்து;
	வரி = ml_get(--lnum);
	எனில் (*வரி == NUL || வரி[STRLEN(வரி) - 1] != '\\')
	    நிறுத்து;
    }

    எனில் (lnum != *lnump)
	*pp = ml_get(*lnump);
    எனில் (retval)
	*amount = candidate_amount;
    திரும்பு retval;
}

    நிலையான முழு
cin_iselse(
    char_u  *p)
{
    எனில் (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    திரும்பு (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    நிலையான முழு
cin_isterminated(
    char_u	*s,
    முழு		incl_open,	// include '{' at the end as terminator
    முழு		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    முழு		is_else = FALSE;

    s = cin_skipcomment(s);

    எனில் (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    எனில் (!found_start)
	is_else = cin_iselse(s);

    போது (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	எனில் (*s == '}' && n_open > 0)
	    --n_open;
	எனில் ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    திரும்பு *s;
	இல்லையெனில் எனில் (*s == '{')
	{
	    எனில் (incl_open && cin_nocode(s + 1))
		திரும்பு *s;
	    இல்லையெனில்
		++n_open;
	}

	எனில் (*s)
	    s++;
    }
    திரும்பு found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    நிலையான முழு
cin_starts_with(char_u *s, எழுத்து *word)
{
    முழு l = (முழு)STRLEN(word);

    திரும்பு (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    நிலையான முழு
cin_isdefault(char_u *s)
{
    திரும்பு (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    நிலையான முழு
cin_iscase(
    char_u *s,
    முழு strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    எனில் (cin_starts_with(s, "case"))
    {
	க்கு (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    எனில் (*s == NUL)
		நிறுத்து;
	    எனில் (*s == ':')
	    {
		எனில் (s[1] == ':')	// skip over "::" for C++
		    ++s;
		இல்லையெனில்
		    திரும்பு TRUE;
	    }
	    எனில் (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    இல்லையெனில் எனில் (*s == '/' && (s[1] == '*' || s[1] == '/'))
		திரும்பு FALSE;		// stop at comment
	    இல்லையெனில் எனில் (*s == '"')
	    {
		// JS etc.
		எனில் (strict)
		    திரும்பு FALSE;		// stop at string
		இல்லையெனில்
		    திரும்பு TRUE;
	    }
	}
	திரும்பு FALSE;
    }

    எனில் (cin_isdefault(s))
	திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    நிலையான முழு
cin_islabel(வெறுமை)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    எனில் (cin_isdefault(s))
	திரும்பு FALSE;
    எனில் (cin_isscopedecl(s))
	திரும்பு FALSE;

    எனில் (!cin_islabel_skip(&s))
	திரும்பு FALSE;

    எனில் (ind_find_start_CORS(வெற்று))
	திரும்பு FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*வரி;

    cursor_save = curwin->w_cursor;
    போது (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	எனில் ((trypos = ind_find_start_CORS(வெற்று)) != வெற்று) // XXX
	    curwin->w_cursor = *trypos;

	வரி = ml_get_curline();
	எனில் (cin_ispreproc(வரி))	// ignore #defines, #if, etc.
	    தொடர்;
	எனில் (*(வரி = cin_skipcomment(வரி)) == NUL)
	    தொடர்;

	curwin->w_cursor = cursor_save;
	எனில் (cin_isterminated(வரி, TRUE, FALSE)
		|| cin_isscopedecl(வரி)
		|| cin_iscase(வரி, TRUE)
		|| (cin_islabel_skip(&வரி) && cin_nocode(வரி)))
	    திரும்பு TRUE;
	திரும்பு FALSE;
    }
    curwin->w_cursor = cursor_save;
    திரும்பு TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    நிலையான முழு
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    முழு		len = (முழு)STRLEN(find);

    போது (*p != NUL)
    {
	p = cin_skipcomment(p);
	எனில் (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    எனில் (cin_nocode(r))
		திரும்பு TRUE;
	}
	எனில் (*p != NUL)
	    ++p;
    }
    திரும்பு FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    நிலையான char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = வெற்று, *p = s;
    செய்
    {
	r = p;
	p = cin_skipcomment(p);
	எனில் (*p)
	    p = skip_string(p);
    } போது (p != r);
    திரும்பு p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    நிலையான முழு
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = வெற்று;

    போது (*p)
    {
	எனில் (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	இல்லையெனில் எனில் (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	இல்லையெனில்
	    p = cin_skip_comment_and_string(p + 1);
    }
    எனில் (!r)
	திரும்பு FALSE;
    p = r; // p points now after '=' or "return"

    எனில் (cin_nocode(p))
	திரும்பு TRUE;

    எனில் (*p == '&')
	p = cin_skipcomment(p + 1);

    எனில் (*p == '(') // skip a typecast
    {
	முழு open_மொத்தம் = 1;
	செய்
	{
	    p = cin_skip_comment_and_string(p + 1);
	    எனில் (cin_nocode(p))
		திரும்பு TRUE;
	    open_மொத்தம் += (*p == '(') - (*p == ')');
	} போது (open_மொத்தம்);
	p = cin_skipcomment(p + 1);
	எனில் (cin_nocode(p))
	    திரும்பு TRUE;
    }

    போது (*p == '{')
	p = cin_skipcomment(p + 1);
    திரும்பு cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    நிலையான முழு
cin_isinit(வெறுமை)
{
    char_u	*s;
    நிலையான எழுத்து *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    எனில் (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    க்கு (;;)
    {
	முழு i, l;

	க்கு (i = 0; i < (முழு)ARRAY_LENGTH(skip); ++i)
	{
	    l = (முழு)strlen(skip[i]);
	    எனில் (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		நிறுத்து;
	    }
	}
	எனில் (l != 0)
	    நிறுத்து;
    }

    எனில் (cin_starts_with(s, "enum"))
	திரும்பு TRUE;

    திரும்பு cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    நிலையான முழு
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    முழு		has_name = FALSE;
    முழு		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    போது ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    எனில் (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	போது (*p != NUL)
	{
	    எனில் (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    இல்லையெனில் எனில் (*p == '{')
	    {
		நிறுத்து;
	    }
	    இல்லையெனில் எனில் (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		எனில் (has_name)
		    திரும்பு FALSE; // word character after skipping past name
		++p;
	    }
	    இல்லையெனில் எனில் (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		எனில் (!has_name_start || has_name)
		    திரும்பு FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    இல்லையெனில்
	    {
		திரும்பு FALSE;
	    }
	}
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    நிலையான முழு
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    முழு		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    எனில் (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	போது (*p != NUL)
	{
	    எனில் (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    இல்லையெனில் எனில் (*p == '{')
	    {
		நிறுத்து;
	    }
	    இல்லையெனில் எனில் (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		எனில் (has_string_literal)
		    திரும்பு FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    இல்லையெனில் எனில் (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		எனில் (has_string_literal)
		    திரும்பு FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    இல்லையெனில்
	    {
		திரும்பு FALSE;
	    }
	}
	திரும்பு has_string_literal ? TRUE : FALSE;
    }
    திரும்பு FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    நிலையான char_u *
after_label(char_u *l)
{
    க்கு ( ; *l; ++l)
    {
	எனில் (*l == ':')
	{
	    எனில் (l[1] == ':')	    // skip over "::" for C++
		++l;
	    இல்லையெனில் எனில் (!cin_iscase(l + 1, FALSE))
		நிறுத்து;
	}
	இல்லையெனில் எனில் (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    எனில் (*l == NUL)
	திரும்பு வெற்று;
    l = cin_skipcomment(l + 1);
    எனில் (*l == NUL)
	திரும்பு வெற்று;
    திரும்பு l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    நிலையான முழு
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    எனில் (p == வெற்று)
	திரும்பு 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, வெற்று, வெற்று, 0);
    திரும்பு (முழு)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    நிலையான முழு
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    முழு		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    எனில் (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	எனில் (l == வெற்று)		// just in case
	    l = ml_get_curline();
    }
    இல்லையெனில்
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    திரும்பு amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    நிலையான முழு
cin_first_id_amount(வெறுமை)
{
    char_u	*வரி, *p, *s;
    முழு		len;
    pos_T	fp;
    colnr_T	col;

    வரி = ml_get_curline();
    p = skipwhite(வரி);
    len = (முழு)(skiptowhite(p) - p);
    எனில் (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (முழு)(skiptowhite(p) - p);
    }
    எனில் (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    இல்லையெனில் எனில் (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    இல்லையெனில் எனில் ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	எனில் ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    க்கு (len = 0; vim_isIDc(p[len]); ++len)
	;
    எனில் (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	திரும்பு 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - வரி);
    getvcol(curwin, &fp, &col, வெற்று, வெற்று, 0);
    திரும்பு (முழு)col;
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
    நிலையான முழு
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*வரி;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    எனில் (lnum > 1)
    {
	வரி = ml_get(lnum - 1);
	எனில் (*வரி != NUL && வரி[STRLEN(வரி) - 1] == '\\')
	    திரும்பு -1;
    }

    வரி = s = ml_get(lnum);
    போது (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == வெற்று)
    {
	எனில் (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	இல்லையெனில்
	    ++s;
    }
    எனில் (*s != '=')
	திரும்பு 0;

    s = skipwhite(s + 1);
    எனில் (cin_nocode(s))
	திரும்பு 0;

    எனில் (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - வரி);
    getvcol(curwin, &fp, &col, வெற்று, வெற்று, 0);
    திரும்பு (முழு)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    நிலையான முழு
cin_skip2pos(pos_T *trypos)
{
    char_u	*வரி;
    char_u	*p;
    char_u	*new_p;

    p = வரி = ml_get(trypos->lnum);
    போது (*p && (colnr_T)(p - வரி) < trypos->col)
    {
	எனில் (cin_iscomment(p))
	    p = cin_skipcomment(p);
	இல்லையெனில்
	{
	    new_p = skip_string(p);
	    எனில் (new_p == p)
		++p;
	    இல்லையெனில்
		p = new_p;
	}
    }
    திரும்பு (முழு)(p - வரி);
}

    நிலையான pos_T *
find_match_char(முழு c, முழு ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    நிலையான pos_T pos_copy;
    முழு		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    எனில் ((trypos = findmatchlimit(வெற்று, c, FM_SKIPCOMM, ind_maxp_wk)) != வெற்று)
    {
	// check if the ( is in a // comment
	எனில் ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (முழு)(cursor_save.lnum - trypos->lnum);
	    எனில் (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		செல் retry;
	    }
	    trypos = வெற்று;
	}
	இல்லையெனில்
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    எனில் ((trypos_wk = ind_find_start_CORS(வெற்று)) != வெற்று) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (முழு)(cursor_save.lnum
			- trypos_wk->lnum);
		எனில் (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    செல் retry;
		}
		trypos = வெற்று;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    திரும்பு trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    நிலையான pos_T *
find_match_paren(முழு ind_maxparen)	// XXX
{
    திரும்பு find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    நிலையான முழு
find_last_paren(char_u *l, முழு start, முழு end)
{
    முழு		i;
    முழு		retval = FALSE;
    முழு		open_மொத்தம் = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    க்கு (i = 0; l[i] != NUL; i++)
    {
	i = (முழு)(cin_skipcomment(l + i) - l); // ignore parens in comments
	எனில் (l[i] == NUL)
	    நிறுத்து;
	i = (முழு)(skip_string(l + i) - l);    // ignore parens in quotes
	எனில் (l[i] == start)
	    ++open_மொத்தம்;
	இல்லையெனில் எனில் (l[i] == end)
	{
	    எனில் (open_மொத்தம் > 0)
		--open_மொத்தம்;
	    இல்லையெனில்
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    திரும்பு retval;
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
    நிலையான முழு
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    முழு		retval = FALSE;
    pos_T	*trypos;
    முழு		just_started = TRUE;

    எனில் (sp == வெற்று)
	s = ml_get(lnum);
    இல்லையெனில்
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    எனில் (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != வெற்று)
    {
	lnum = trypos->lnum;
	எனில் (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    திரும்பு FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    எனில் (cin_ispreproc(s))
	திரும்பு FALSE;

    போது (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	எனில் (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	இல்லையெனில் எனில் (*s == ':')
	{
	    எனில் (*(s + 1) == ':')
		s += 2;
	    இல்லையெனில்
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		திரும்பு FALSE;
	}
	இல்லையெனில்
	    ++s;
    }
    எனில் (*s != '(')
	திரும்பு FALSE;		// ';', ' or "  before any () or no '('

    போது (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	எனில் (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    எனில் (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    செல் done;
	}
	எனில் ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    முழு comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    க்கு (;;)
	    {
		எனில் (lnum >= curbuf->b_ml.ml_வரி_மொத்தம்)
		    நிறுத்து;
		s = ml_get(++lnum);
		எனில் (!cin_ispreproc(s))
		    நிறுத்து;
	    }
	    எனில் (lnum >= curbuf->b_ml.ml_வரி_மொத்தம்)
		நிறுத்து;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    எனில் (!just_started && (!comma && *s != ',' && *s != ')'))
		நிறுத்து;
	    just_started = FALSE;
	}
	இல்லையெனில் எனில் (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	இல்லையெனில்
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    எனில் (lnum != first_lnum && sp != வெற்று)
	*sp = ml_get(first_lnum);

    திரும்பு retval;
}

    நிலையான முழு
cin_isif(char_u *p)
{
    திரும்பு (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    நிலையான முழு
cin_isdo(char_u *p)
{
    திரும்பு (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    நிலையான முழு
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    முழு		retval = FALSE;

    p = cin_skipcomment(p);
    எனில் (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    எனில் (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	போது (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	எனில் ((trypos = findmatchlimit(வெற்று, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != வெற்று
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    திரும்பு retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    நிலையான முழு
cin_is_if_for_while_before_offset(char_u *வரி, முழு *poffset)
{
    முழு offset = *poffset;

    எனில் (offset-- < 2)
	திரும்பு 0;
    போது (offset > 2 && VIM_ISWHITE(வரி[offset]))
	--offset;

    offset -= 1;
    எனில் (STRNCMP(வரி + offset, "if", 2) == 0)
	செல் probablyFound;

    எனில் (offset >= 1)
    {
	offset -= 1;
	எனில் (STRNCMP(வரி + offset, "for", 3) == 0)
	    செல் probablyFound;

	எனில் (offset >= 2)
	{
	    offset -= 2;
	    எனில் (STRNCMP(வரி + offset, "while", 5) == 0)
		செல் probablyFound;
	}
    }
    திரும்பு 0;

probablyFound:
    எனில் (!offset || !vim_isIDc(வரி[offset - 1]))
    {
	*poffset = offset;
	திரும்பு 1;
    }
    திரும்பு 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    நிலையான முழு
cin_iswhileofdo_end(முழு terminated)
{
    char_u	*வரி;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    முழு		i;

    எனில் (terminated != ';')	// there must be a ';' at the end
	திரும்பு FALSE;

    p = வரி = ml_get_curline();
    போது (*p != NUL)
    {
	p = cin_skipcomment(p);
	எனில் (*p == ')')
	{
	    s = skipwhite(p + 1);
	    எனில் (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (முழு)(p - வரி);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		எனில் (trypos != வெற்று)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    எனில் (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    எனில் (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			திரும்பு TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		வரி = ml_get_curline();
		p = வரி + i;
	    }
	}
	எனில் (*p != NUL)
	    ++p;
    }
    திரும்பு FALSE;
}

    நிலையான முழு
cin_isbreak(char_u *p)
{
    திரும்பு (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    நிலையான முழு
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    முழு		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*வரி = ml_get_curline();

    எனில் (pos->lnum <= lnum)
	திரும்பு cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(வரி);
    எனில் (*s == '#')		// skip #define FOO x ? (x) : x
	திரும்பு FALSE;
    s = cin_skipcomment(s);
    எனில் (*s == NUL)
	திரும்பு FALSE;

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
    போது (lnum > 1)
    {
	வரி = ml_get(lnum - 1);
	s = skipwhite(வரி);
	எனில் (*s == '#' || *s == NUL)
	    நிறுத்து;
	போது (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    எனில் (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		நிறுத்து;
	    எனில் (*s != NUL)
		++s;
	}
	எனில் (*s != NUL)
	    நிறுத்து;
	--lnum;
    }

    pos->lnum = lnum;
    வரி = ml_get(lnum);
    s = வரி;
    க்கு (;;)
    {
	எனில் (*s == NUL)
	{
	    எனில் (lnum == curwin->w_cursor.lnum)
		நிறுத்து;
	    // Continue in the cursor line.
	    வரி = ml_get(++lnum);
	    s = வரி;
	}
	எனில் (s == வரி)
	{
	    // don't recognize "case (foo):" as a baseclass
	    எனில் (cin_iscase(s, FALSE))
		நிறுத்து;
	    s = cin_skipcomment(வரி);
	    எனில் (*s == NUL)
		தொடர்;
	}

	எனில் (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	இல்லையெனில் எனில் (s[0] == ':')
	{
	    எனில் (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    இல்லையெனில் எனில் (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    இல்லையெனில்
		s = cin_skipcomment(s + 1);
	}
	இல்லையெனில் எனில் ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    எனில் (*s == 'c')
		s = cin_skipcomment(s + 5);
	    இல்லையெனில்
		s = cin_skipcomment(s + 6);
	}
	இல்லையெனில்
	{
	    எனில் (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    இல்லையெனில் எனில் (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    இல்லையெனில் எனில் (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		திரும்பு FALSE;
	    }
	    இல்லையெனில் எனில் (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    இல்லையெனில் எனில் (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		எனில் (cpp_base_class)
		    pos->col = (colnr_T)(s - வரி);
	    }

	    // When the line ends in a comma don't align with it.
	    எனில் (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    எனில் (cpp_base_class)
	pos->lnum = lnum;
    திரும்பு cpp_base_class;
}

    நிலையான முழு
get_baseclass_amount(முழு col)
{
    முழு		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    எனில் (col == 0)
    {
	amount = get_indent();
	எனில் (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != வெற்று)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	எனில் (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    இல்லையெனில்
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, வெற்று, வெற்று, 0);
	amount = (முழு)vcol;
    }
    எனில் (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    திரும்பு amount;
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

    நிலையான pos_T *
find_start_brace(வெறுமை)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    நிலையான pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    போது ((trypos = findmatchlimit(வெற்று, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != வெற்று)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = வெற்று;
	// ignore the { if it's in a // or / *  * / comment
	எனில் ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(வெற்று)) == வெற்று) // XXX
	    நிறுத்து;
	எனில் (pos != வெற்று)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    திரும்பு trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    நிலையான pos_T *
find_match_paren_after_brace (முழு ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    எனில் (trypos == வெற்று)
	திரும்பு வெற்று;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    எனில் (tryposBrace != வெற்று
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = வெற்று;
    திரும்பு trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    நிலையான முழு
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    எனில் (n > 0 && n < curbuf->b_ind_maxparen / 2)
	திரும்பு curbuf->b_ind_maxparen - (முழு)n;
    திரும்பு curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    வெறுமை
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    முழு		divider;
    முழு		fraction = 0;
    முழு		sw;
    long	t = get_sw_மதிப்பு(buf);

    // needed for cino-(, it will be multiplied by 2 again
    எனில் (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    இல்லையெனில்
	sw = (முழு)t;

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
    buf->b_ind_open_இடது_imag = 0;

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

    க்கு (p = buf->b_p_cino; *p; )
    {
	l = p++;
	எனில் (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	எனில் (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((எழுத்து *)++p);
	    போது (VIM_ISDIGIT(*p))
	    {
		++p;
		எனில் (divider)
		    divider *= 10;
		இல்லையெனில்
		    divider = 10;
	    }
	}
	எனில் (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    எனில் (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    இல்லையெனில்
	    {
		n *= sw;
		எனில் (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	எனில் (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	தேர்வு (*l)
	{
	    நிலை '>': buf->b_ind_level = (முழு)n; நிறுத்து;
	    நிலை 'e': buf->b_ind_open_imag = (முழு)n; நிறுத்து;
	    நிலை 'n': buf->b_ind_no_brace = (முழு)n; நிறுத்து;
	    நிலை 'f': buf->b_ind_first_open = (முழு)n; நிறுத்து;
	    நிலை '{': buf->b_ind_open_extra = (முழு)n; நிறுத்து;
	    நிலை '}': buf->b_ind_close_extra = (முழு)n; நிறுத்து;
	    நிலை '^': buf->b_ind_open_இடது_imag = (முழு)n; நிறுத்து;
	    நிலை 'L': buf->b_ind_jump_label = (முழு)n; நிறுத்து;
	    நிலை ':': buf->b_ind_case = (முழு)n; நிறுத்து;
	    நிலை '=': buf->b_ind_case_code = (முழு)n; நிறுத்து;
	    நிலை 'b': buf->b_ind_case_break = (முழு)n; நிறுத்து;
	    நிலை 'p': buf->b_ind_param = (முழு)n; நிறுத்து;
	    நிலை 't': buf->b_ind_func_type = (முழு)n; நிறுத்து;
	    நிலை '/': buf->b_ind_comment = (முழு)n; நிறுத்து;
	    நிலை 'c': buf->b_ind_in_comment = (முழு)n; நிறுத்து;
	    நிலை 'C': buf->b_ind_in_comment2 = (முழு)n; நிறுத்து;
	    நிலை 'i': buf->b_ind_cpp_baseclass = (முழு)n; நிறுத்து;
	    நிலை '+': buf->b_ind_continuation = (முழு)n; நிறுத்து;
	    நிலை '(': buf->b_ind_unclosed = (முழு)n; நிறுத்து;
	    நிலை 'u': buf->b_ind_unclosed2 = (முழு)n; நிறுத்து;
	    நிலை 'U': buf->b_ind_unclosed_noignore = (முழு)n; நிறுத்து;
	    நிலை 'W': buf->b_ind_unclosed_wrapped = (முழு)n; நிறுத்து;
	    நிலை 'w': buf->b_ind_unclosed_whiteok = (முழு)n; நிறுத்து;
	    நிலை 'm': buf->b_ind_matching_paren = (முழு)n; நிறுத்து;
	    நிலை 'M': buf->b_ind_paren_prev = (முழு)n; நிறுத்து;
	    நிலை ')': buf->b_ind_maxparen = (முழு)n; நிறுத்து;
	    நிலை '*': buf->b_ind_maxcomment = (முழு)n; நிறுத்து;
	    நிலை 'g': buf->b_ind_scopedecl = (முழு)n; நிறுத்து;
	    நிலை 'h': buf->b_ind_scopedecl_code = (முழு)n; நிறுத்து;
	    நிலை 'j': buf->b_ind_java = (முழு)n; நிறுத்து;
	    நிலை 'J': buf->b_ind_js = (முழு)n; நிறுத்து;
	    நிலை 'l': buf->b_ind_keep_case_label = (முழு)n; நிறுத்து;
	    நிலை '#': buf->b_ind_hash_comment = (முழு)n; நிறுத்து;
	    நிலை 'N': buf->b_ind_cpp_namespace = (முழு)n; நிறுத்து;
	    நிலை 'k': buf->b_ind_if_for_while = (முழு)n; நிறுத்து;
	    நிலை 'E': buf->b_ind_cpp_extern_c = (முழு)n; நிறுத்து;
	    நிலை 'P': buf->b_ind_pragma = (முழு)n; நிறுத்து;
	}
	எனில் (*p == ',')
	    ++p;
    }
}

    நிலையான முழு
find_match(முழு lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    முழு		elselevel;
    முழு		whilelevel;

    எனில் (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    இல்லையெனில்
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    போது (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	எனில் (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    எனில் (theirscope == வெற்று)
		நிறுத்து;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    எனில் (theirscope->lnum < ourscope)
		நிறுத்து;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    எனில் (theirscope->lnum > ourscope)
		தொடர்;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    எனில் (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		எனில் (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    எனில் (!cin_isif(mightbeif))
			++elselevel;
		    தொடர்;
		}

		// If it's an "if" decrement elselevel
		எனில் (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    எனில் (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    எனில் (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		தொடர்;
	    }

	    // If it's a "do" decrement whilelevel
	    எனில் (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    எனில் (elselevel <= 0 && whilelevel <= 0)
		திரும்பு OK;
	}
    }
    திரும்பு FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    முழு
get_c_indent(வெறுமை)
{
    pos_T	cur_curpos;
    முழு		amount;
    முழு		scope_amount;
    முழு		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = வெற்று;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    முழு		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    முழு		lookfor;
    முழு		whilelevel;
    linenr_T	lnum;
    முழு		n;
    முழு		iscase;
    முழு		lookfor_break;
    முழு		lookfor_cpp_namespace = FALSE;
    முழு		cont_amount = 0;    // amount for continuation line
    முழு		original_வரி_islabel;
    முழு		added_to_amount = 0;
    முழு		js_cur_has_சாவி = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    முழு		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    எனில் (cur_curpos.lnum == 1)
	திரும்பு 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    எனில் (linecopy == வெற்று)
	திரும்பு 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    எனில் ((State & MODE_செருகு)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_வரி_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    எனில் (comment_pos != வெற்று)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    எனில் (trypos != வெற்று && (comment_pos == வெற்று
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	செல் laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    எனில் (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	எனில் (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    செல் theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    எனில் (original_வரி_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	செல் theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    எனில் (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_வரி_comment(); // XXX
	எனில் (trypos == வெற்று && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    எனில் (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	எனில் (trypos  != வெற்று)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, வெற்று, வெற்று, 0);
	    amount = col;
	    செல் theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    எனில் (!cin_iscomment(theline) && comment_pos != வெற்று) // XXX
    {
	முழு	lead_start_len = 2;
	முழு	lead_நடு_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_நடு[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	முழு	lead_end_len;
	char_u	*p;
	முழு	start_align = 0;
	முழு	start_off = 0;
	முழு	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, வெற்று, வெற்று, 0);
	amount = col;
	*lead_start = NUL;
	*lead_நடு = NUL;

	p = curbuf->b_p_com;
	போது (*p != NUL)
	{
	    முழு	align = 0;
	    முழு	off = 0;
	    முழு what = 0;

	    போது (*p != NUL && *p != ':')
	    {
		எனில் (*p == COM_START || *p == COM_END || *p == COM_நடு)
		    what = *p++;
		இல்லையெனில் எனில் (*p == COM_இடது || *p == COM_வலது)
		    align = *p++;
		இல்லையெனில் எனில் (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		இல்லையெனில்
		    ++p;
	    }

	    எனில் (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    எனில் (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    இல்லையெனில் எனில் (what == COM_நடு)
	    {
		STRCPY(lead_நடு, lead_end);
		lead_நடு_len = lead_end_len;
	    }
	    இல்லையெனில் எனில் (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		எனில் (STRNCMP(theline, lead_நடு, lead_நடு_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    எனில் (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			எனில் (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			இல்லையெனில் எனில் (STRNCMP(look, lead_நடு,
							lead_நடு_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    நிறுத்து;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			இல்லையெனில் எனில் (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    தொடர்;
		    }
		    எனில் (start_off != 0)
			amount += start_off;
		    இல்லையெனில் எனில் (start_align == COM_வலது)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_நடு);
		    நிறுத்து;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		எனில் (STRNCMP(theline, lead_நடு, lead_நடு_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    எனில் (off != 0)
			amount += off;
		    இல்லையெனில் எனில் (align == COM_வலது)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_நடு);
		    done = TRUE;
		    நிறுத்து;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	எனில் (done)
	    ;
	இல்லையெனில் எனில் (theline[0] == '*')
	    amount += 1;
	இல்லையெனில்
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    க்கு (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		எனில் (linewhite(lnum))		    // skip blank lines
		    தொடர்;
		amount = get_indent_lnum(lnum);	    // XXX
		நிறுத்து;
	    }
	    எனில் (amount == -1)			    // use the comment opener
	    {
		எனில் (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    எனில் (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, வெற்று, வெற்று, 0);
		amount = col;
		எனில் (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	செல் theend;
    }

    // Are we looking at a ']' that has a match?
    எனில் (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != வெற்று)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	செல் theend;
    }

    // Are we inside parentheses or braces?  XXX
    எனில் (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != வெற்று
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != வெற்று
	    || trypos != வெற்று)
    {
      எனில் (trypos != வெற்று && tryposBrace != வெற்று)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  எனில் (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = வெற்று;
	  இல்லையெனில்
	      tryposBrace = வெற்று;
      }

      எனில் (trypos != வெற்று)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	எனில் (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	இல்லையெனில்
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    க்கு (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		எனில் (cin_nocode(l))		// skip comment lines
		    தொடர்;
		எனில் (cin_ispreproc_cont(&l, &lnum, &amount))
		    தொடர்;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		எனில் ((trypos = ind_find_start_CORS(வெற்று)) != வெற்று)
		{
		    lnum = trypos->lnum + 1;
		    தொடர்;
		}

		// XXX
		எனில் ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != வெற்று
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    எனில் (theline[0] == ')')
		    {
			எனில் (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    நிறுத்து;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	எனில் (amount == -1)
	{
	    முழு	    ignore_paren_col = 0;
	    முழு	    is_if_for_while = 0;

	    எனில் (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *வரி;

		trypos = &our_paren_pos;
		செய்
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} போது (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		வரி = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(வரி, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    எனில் (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *வரி;
		முழு	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		வரி = ml_get_curline();
		look_col = (முழு)(look - வரி);
		curwin->w_cursor.col = look_col + 1;
		எனில் ((trypos = findmatchlimit(வெற்று, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != வெற்று
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    எனில் (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		எனில் (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    எனில் (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			க்கு (col = 0; col < our_paren_pos.col; ++col)
			{
			    தேர்வு (l[col])
			    {
				நிலை '(':
				நிலை '{': ++n;
					  நிறுத்து;

				நிலை ')':
				நிலை '}': எனில் (n > 1)
					      --n;
					  நிறுத்து;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    இல்லையெனில் எனில் (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    இல்லையெனில்
		    {
			col = our_paren_pos.col + 1;
			போது (VIM_ISWHITE(l[col]))
			    col++;
			எனில் (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			இல்லையெனில்
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		எனில் (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, வெற்று, வெற்று, 0);
		    எனில் (cur_amount > (முழு)col)
			cur_amount = col;
		}
	    }

	    எனில் (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    இல்லையெனில் எனில் ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		எனில் (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    இல்லையெனில்
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		போது ((முழு)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    தேர்வு (*ml_get_pos(&our_paren_pos))
		    {
			நிலை '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  நிறுத்து;
			நிலை ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  நிறுத்து;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		எனில் (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		இல்லையெனில்
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    எனில் (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != வெற்று)
			amount += curbuf->b_ind_unclosed2;
		    இல்லையெனில்
		    {
			எனில் (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			இல்லையெனில்
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		எனில் (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	எனில் (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      இல்லையெனில்
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
	எனில் (*look == '{')
	{
	    getvcol(curwin, trypos, &col, வெற்று, வெற்று, 0);
	    amount = col;
	    எனில் (*start == '{')
		start_brace = BRACE_IN_COL0;
	    இல்லையெனில்
		start_brace = BRACE_AT_START;
	}
	இல்லையெனில்
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    எனில் (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != வெற்று)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    எனில் ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    இல்லையெனில் எனில் (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    இல்லையெனில்
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	எனில் (curbuf->b_ind_js)
	    js_cur_has_சாவி = cin_has_js_சாவி(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	எனில் (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	இல்லையெனில்
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    எனில் (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    இல்லையெனில் எனில் (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    எனில் (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		எனில் (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    செல் theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    எனில் (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_இடது_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    இல்லையெனில் எனில் (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    இல்லையெனில்
	    {
		எனில் (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    எனில் (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    இல்லையெனில் எனில் (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		இல்லையெனில்
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    எனில் (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    எனில் (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    இல்லையெனில் எனில் (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    இல்லையெனில்
	    {
		எனில் (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    க்கு (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		எனில் (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    எனில் (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			எனில் (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    எனில் (cont_amount > 0)
				amount = cont_amount;
			    இல்லையெனில் எனில் (!curbuf->b_ind_js)
				amount += ind_continuation;
			    நிறுத்து;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(வெற்று);
			எனில் (trypos != வெற்று)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    தொடர்;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			எனில் (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    தொடர்;

			எனில் (cin_nocode(l))
			    தொடர்;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			எனில் (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    எனில் (terminated == ',')
				நிறுத்து;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    எனில் (terminated != ';' && cin_isinit())
				நிறுத்து;

			    // nothing useful found
			    எனில் (terminated == 0 || terminated == '{')
				தொடர்;
			}

			எனில் (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = வெற்று;
			    எனில் (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    எனில் (trypos == வெற்று && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    எனில் (trypos != வெற்று)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				தொடர்;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			எனில் (cont_amount > 0)
			    amount = cont_amount;
			இல்லையெனில்
			    amount += ind_continuation;
		    }
		    இல்லையெனில் எனில் (lookfor == LOOKFOR_UNTERM)
		    {
			எனில் (cont_amount > 0)
			    amount = cont_amount;
			இல்லையெனில்
			    amount += ind_continuation;
		    }
		    இல்லையெனில்
		    {
			எனில் (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    எனில் (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			எனில் (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    எனில் (curwin->w_cursor.lnum == ourscope)
				தொடர்;

			    எனில் (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				நிறுத்து;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(வெற்று);
			    எனில் (trypos != வெற்று)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				தொடர்;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    எனில் (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				தொடர்;

			    // Finally the actual check for "namespace".
			    எனில் (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				நிறுத்து;
			    }
			    இல்லையெனில் எனில் (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				நிறுத்து;
			    }

			    எனில் (cin_nocode(l))
				தொடர்;
			}
		    }
		    நிறுத்து;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		எனில் ((trypos = ind_find_start_CORS(&raw_string_start)) != வெற்று)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    தொடர்;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		எனில் (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    எனில் (lookfor == LOOKFOR_CPP_BASECLASS)
			நிறுத்து;

		    // When looking for a "do" we are not interested in
		    // labels.
		    எனில் (whilelevel > 0)
			தொடர்;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    எனில் (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			எனில் (cont_amount > 0)
			    amount = cont_amount;
			இல்லையெனில்
			    amount += ind_continuation;
			நிறுத்து;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    எனில் (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			எனில் ((trypos = find_start_brace()) == வெற்று
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    நிறுத்து;
			}
			தொடர்;
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
		    எனில் (lookfor == LOOKFOR_TERM)
		    {
			எனில் (n)
			    amount = n;

			எனில் (!lookfor_break)
			    நிறுத்து;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    எனில் (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			எனில் (l != வெற்று && cin_is_cinword(l))
			{
			    எனில் (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    இல்லையெனில்
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			நிறுத்து;
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
		    தொடர்;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		எனில் (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    எனில் (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != வெற்று)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    தொடர்;
		}

		// Ignore jump labels with nothing after them.
		எனில் (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    எனில் (l == வெற்று || cin_nocode(l))
			தொடர்;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		எனில் (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    தொடர்;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		எனில் (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		எனில் (n)
		{
		    எனில் (lookfor == LOOKFOR_UNTERM)
		    {
			எனில் (cont_amount > 0)
			    amount = cont_amount;
			இல்லையெனில்
			    amount += ind_continuation;
		    }
		    இல்லையெனில் எனில் (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			தொடர்;
		    }
		    இல்லையெனில்
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    நிறுத்து;
		}
		இல்லையெனில் எனில் (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    எனில் (cin_isterminated(l, TRUE, FALSE))
			நிறுத்து;
		    இல்லையெனில்
			தொடர்;
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

		எனில் (js_cur_has_சாவி)
		{
		    js_cur_has_சாவி = 0; // only check the first line
		    எனில் (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_சாவி;
		    }
		}
		எனில் (lookfor == LOOKFOR_JS_சாவி && cin_has_js_சாவி(l))
		{
		    amount = get_indent();
		    நிறுத்து;
		}
		எனில் (lookfor == LOOKFOR_COMMA)
		{
		    எனில் (tryposBrace != வெற்று && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			நிறுத்து;
		    எனில் (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			நிறுத்து;
		    இல்லையெனில்
		    {
			amount = get_indent();
			எனில் (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    நிறுத்து;
		    }
		}

		எனில் (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    எனில் (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (வெறுமை)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    எனில் (trypos != வெற்று && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = வெற்று;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    எனில் (trypos == வெற்று && terminated == ',')
		    {
			எனில் (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    எனில் (trypos != வெற்று)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			எனில் (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    தொடர்;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    எனில் (terminated == ',')
		    {
			போது (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    எனில் (*l == NUL || l[STRLEN(l) - 1] != '\\')
				நிறுத்து;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    எனில் (curbuf->b_ind_js)
			cur_amount = get_indent();
		    இல்லையெனில்
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    எனில் (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			எனில் (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			எனில் (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    தொடர்;
			}
			நிறுத்து;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    எனில் (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			எனில் (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    எனில் (cont_amount > 0)
				amount = cont_amount;
			    இல்லையெனில்
				amount += ind_continuation;
			    நிறுத்து;
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
			எனில் (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			எனில் (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    நிறுத்து;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			எனில் (cin_isdo(l))
			{
			    எனில் (whilelevel == 0)
				நிறுத்து;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			எனில் (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    எனில் (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    எனில் ((trypos = find_start_brace()) == வெற்று
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				நிறுத்து;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    இல்லையெனில்
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			எனில் (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    எனில் (terminated == ',')
				amount += ind_continuation;
			    நிறுத்து;
			}

			எனில் (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    எனில் (terminated == ',')
			    {
				எனில் (curbuf->b_ind_cpp_baseclass == 0)
				    நிறுத்து;

				lookfor = LOOKFOR_CPP_BASECLASS;
				தொடர்;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    எனில் (amount > cur_amount)
				amount = cur_amount;
			}
			இல்லையெனில்
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (முழு)STRLEN(l);
			    எனில் (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				நிறுத்து;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    எனில் (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				எனில் (curbuf->b_ind_js)
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
				    எனில் (cin_iscomment(skipwhite(l)))
					நிறுத்து;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    எனில் (trypos != வெற்று)
				    {
					எனில் (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    நிறுத்து;
					}
					ourscope = trypos->lnum;
				    }
				}
				இல்லையெனில்
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    இல்லையெனில்
			    {
				எனில் (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				எனில் (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_சாவி
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		இல்லையெனில் எனில் (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    எனில் (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			எனில் (cont_amount > 0)
			    amount = cont_amount;
			இல்லையெனில்
			    amount += ind_continuation;
			நிறுத்து;
		    }

		    எனில் (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			எனில் (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		இல்லையெனில்
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    எனில் (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			தொடர்;
		    }

		    // Handle "do {" line.
		    எனில் (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			எனில் (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    தொடர்;
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
		    எனில் (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			எனில் (cont_amount > 0)
			    amount = cont_amount;
			இல்லையெனில்
			    amount += ind_continuation;
			நிறுத்து;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    எனில் (lookfor == LOOKFOR_TERM)
		    {
			எனில் (!lookfor_break && whilelevel == 0)
			    நிறுத்து;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    இல்லையெனில்
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			எனில் (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != வெற்று)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    எனில் (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				தொடர்;
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

			எனில் (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			எனில் (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			எனில் (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    எனில் ((trypos = find_start_brace()) == வெற்று
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				நிறுத்து;
			    தொடர்;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			எனில் (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != வெற்று)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    எனில் (*l == '}' || !cin_iselse(l))
				செல் term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      எனில் (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      எனில் (curbuf->b_ind_jump_label > 0 && original_வரி_islabel)
	  amount -= curbuf->b_ind_jump_label;

      செல் theend;
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

    எனில் (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	செல் theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    எனில் (cur_curpos.lnum < curbuf->b_ml.ml_வரி_மொத்தம்
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == வெற்று
	    && vim_strchr(theline, '}') == வெற்று
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(வெற்று, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	செல் theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    போது (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	எனில் ((trypos = ind_find_start_CORS(வெற்று)) != வெற்று)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    தொடர்;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	எனில் (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	எனில் (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    நிறுத்து;
	}

	// Skip preprocessor directives and blank lines.
	எனில் (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    தொடர்;

	எனில் (cin_nocode(l))
	    தொடர்;

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
	எனில் (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    எனில் (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != வெற்று)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    போது (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		எனில் (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    நிறுத்து;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    எனில் (amount == 0)
		amount = cin_first_id_amount();
	    எனில் (amount == 0)
		amount = ind_continuation;
	    நிறுத்து;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	எனில் (cin_isfuncdecl(வெற்று, cur_curpos.lnum, 0))  // XXX
	    நிறுத்து;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	எனில் (*skipwhite(l) == '}')
	    நிறுத்து;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	எனில் (cin_ends_in(l, (char_u *)"};"))
	    நிறுத்து;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	எனில் (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    நிறுத்து;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	எனில் (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    போது (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		எனில் (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    நிறுத்து;
	    }
	    எனில் (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		நிறுத்து;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	எனில் (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    நிறுத்து;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	எனில் (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    எனில் (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		நிறுத்து;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	எனில் ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != வெற்று)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	நிறுத்து;
    }

    // add extra indent for a comment
    எனில் (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    எனில் (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	எனில் (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    எனில் (cur_amount > 0)
		amount = cur_amount;
	    இல்லையெனில் எனில் (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    எனில் (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    திரும்பு amount;
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
    முழு
in_cinkeys(
    முழு		keytyped,
    முழு		when,
    முழு		வரி_is_empty)
{
    char_u	*look;
    முழு		try_match;
    முழு		try_match_word;
    char_u	*p;
    char_u	*வரி;
    முழு		icase;
    முழு		i;

    எனில் (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	திரும்பு FALSE;

#ifdef FEAT_EVAL
    எனில் (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    இல்லையெனில்
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    போது (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	தேர்வு (when)
	{
	    நிலை '*': try_match = (*look == '*'); நிறுத்து;
	    நிலை '!': try_match = (*look == '!'); நிறுத்து;
	    இயல்பு: try_match = (*look != '*'); நிறுத்து;
	}
	எனில் (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	எனில் (*look == '0')
	{
	    try_match_word = try_match;
	    எனில் (!வரி_is_empty)
		try_match = FALSE;
	    ++look;
	}
	இல்லையெனில்
	    try_match_word = FALSE;

	// does it look like a control character?
	எனில் (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    எனில் (try_match && keytyped == Ctrl_chr(look[1]))
		திரும்பு TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	இல்லையெனில் எனில் (*look == 'o')
	{
	    எனில் (try_match && keytyped == சாவி_OPEN_FORW)
		திரும்பு TRUE;
	    ++look;
	}
	இல்லையெனில் எனில் (*look == 'O')
	{
	    எனில் (try_match && keytyped == சாவி_OPEN_BACK)
		திரும்பு TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	இல்லையெனில் எனில் (*look == 'e')
	{
	    எனில் (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		எனில் (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    திரும்பு TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	இல்லையெனில் எனில் (*look == ':')
	{
	    எனில் (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		எனில் (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    திரும்பு TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		எனில் (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    எனில் (i)
			திரும்பு TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	இல்லையெனில் எனில் (*look == '<')
	{
	    எனில் (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		எனில் (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != வெற்று
						       && keytyped == look[1])
		    திரும்பு TRUE;

		எனில் (keytyped == get_special_சாவி_code(look + 1))
		    திரும்பு TRUE;
	    }
	    போது (*look && *look != '>')
		look++;
	    போது (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	இல்லையெனில் எனில் (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    எனில் (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    இல்லையெனில்
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    எனில் (p == வெற்று)
		p = look + STRLEN(look);
	    எனில் ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		முழு		match = FALSE;

		எனில் (keytyped == சாவி_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    வரி = ml_get_curline();
		    எனில் (has_mbyte)
		    {
			char_u	*n;

			க்கு (s = வரி + curwin->w_cursor.col; s > வரி; s = n)
			{
			    n = mb_prevptr(வரி, s);
			    எனில் (!vim_iswordp(n))
				நிறுத்து;
			}
		    }
		    இல்லையெனில்
			க்கு (s = வரி + curwin->w_cursor.col; s > வரி; --s)
			    எனில் (!vim_iswordc(s[-1]))
				நிறுத்து;
		    எனில் (s + (p - look) <= வரி + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		இல்லையெனில்
		    // TODO: multi-byte
		    எனில் (keytyped == (முழு)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((முழு)p[-1])))
		{
		    வரி = ml_get_cursor();
		    எனில் ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(வரி[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(வரி - (p - look), look, p - look)
				: STRNCMP(வரி - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		எனில் (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    எனில் (getwhitecols_curline() !=
				     (முழு)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		எனில் (match)
		    திரும்பு TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	இல்லையெனில்
	{
	    எனில் (try_match && *look == keytyped)
		திரும்பு TRUE;
	    எனில் (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    திரும்பு FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    வெறுமை
do_c_கோவை_indent(வெறுமை)
{
#ifdef FEAT_EVAL
    எனில் (*curbuf->b_p_inde != NUL)
	fixthisline(get_கோவை_indent);
    இல்லையெனில்
#endif
	fixthisline(get_c_indent);
}

#எனில் defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    வெறுமை
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    எனில் (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	திரும்பு;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    எனில் (lnum >= 1 && lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_எண் = get_c_indent();
	curwin->w_cursor = pos;
    }
    இல்லையெனில்
	rettv->vval.v_எண் = -1;
}
#endif
