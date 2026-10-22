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

#include "vim.h"

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
#define LOOKFOR_JS_KEY		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    ကိန်းပြည့်
cin_is_cinword(char_u *line)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    ကိန်းပြည့်		cinw_len;
    ကိန်းပြည့်		retval = FALSE;
    ကိန်းပြည့်		len;

    cinw_len = (ကိန်းပြည့်)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    အကယ်၍ (cinw_buf == NULL)
	ပြန်ပေး FALSE;

    line = skipwhite(line);
    အတွက် (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	အကယ်၍ (STRNCMP(line, cinw_buf, len) == 0
		&& (!vim_iswordc(line[len]) || !vim_iswordc(line[len - 1])))
	{
	    retval = TRUE;
	    bha_nga;
	}
    }
    vim_free(cinw_buf);
    ပြန်ပေး retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    static char_u *
skip_string(char_u *p)
{
    ကိန်းပြည့်	    i;

    // We loop, because strings may be concatenated: "date""time".
    အတွက် ( ; ; ++p)
    {
	အကယ်၍ (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    အကယ်၍ (p[1] == NUL)		    // ' at end of line
		bha_nga;
	    i = 2;
	    အကယ်၍ (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		နေစဉ် (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    အကယ်၍ (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		continue;
	    }
	}
	မဟုတ်ပါက အကယ်၍ (p[0] == '"')		    // start of string
	{
	    အတွက် (++p; p[0]; ++p)
	    {
		အကယ်၍ (p[0] == '\\' && p[1] != NUL)
		    ++p;
		မဟုတ်ပါက အကယ်၍ (p[0] == '"')	    // end of string
		    bha_nga;
	    }
	    အကယ်၍ (p[0] == '"')
		continue; // continue for another string
	}
	မဟုတ်ပါက အကယ်၍ (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    အကယ်၍ (paren != NULL)
	    {
		size_t delim_len = paren - delim;

		အတွက် (p += 3; *p; ++p)
		    အကယ်၍ (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			bha_nga;
		    }
		အကယ်၍ (p[0] == '"')
		    continue; // continue for another string
	    }
	}
	bha_nga;				    // no string found
    }
    အကယ်၍ (!*p)
	--p;				    // backup from NUL
    ပြန်ပေး p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    ကိန်းပြည့်
is_pos_in_string(char_u *line, colnr_T col)
{
    char_u *p;

    အတွက် (p = line; *p && (colnr_T)(p - line) < col; ++p)
	p = skip_string(p);
    ပြန်ပေး !((colnr_T)(p - line) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    ကိန်းပြည့်
check_linecomment(char_u *line)
{
    char_u  *p;

    p = line;
    // skip Lispish one-line comments
    အကယ်၍ (curbuf->b_p_lisp)
    {
	အကယ်၍ (vim_strchr(p, ';') != NULL) // there may be comments
	{
	    ကိန်းပြည့် in_str = FALSE;	// inside of string

	    p = line;		// scan from start
	    နေစဉ် ((p = vim_strpbrk(p, (char_u *)"\";")) != NULL)
	    {
		အကယ်၍ (*p == '"')
		{
		    အကယ်၍ (in_str)
		    {
			အကယ်၍ (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    မဟုတ်ပါက အကယ်၍ (p == line || ((p - line) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		မဟုတ်ပါက အကယ်၍ (!in_str && ((p - line) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(line, (colnr_T)(p - line)))
		    bha_nga;	// found!
		++p;
	    }
	}
	မဟုတ်ပါက
	    p = NULL;
    }
    မဟုတ်ပါက
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	အတွက် ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    အကယ်၍ (*p == NUL)
		bha_nga;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    အကယ်၍ (p[0] == '/' && p[1] == '/'
				 && (p == line || p[-1] != '*' || p[2] != '*'))
		bha_nga;
	}
	အကယ်၍ (*p == NUL)
	    p = NULL;
    }

    အကယ်၍ (p == NULL)
	ပြန်ပေး MAXCOL;
    ပြန်ပေး (ကိန်းပြည့်)(p - line);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    static pos_T *
ind_find_start_comment(void)	    // XXX
{
    ပြန်ပေး find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(ကိန်းပြည့် ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ကိန်းပြည့်		cur_maxcomment = ind_maxcomment;

    အတွက် (;;)
    {
	pos = findmatchlimit(NULL, '*', FM_BACKWARD, cur_maxcomment);
	အကယ်၍ (pos == NULL)
	    bha_nga;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	အကယ်၍ (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    bha_nga;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	အကယ်၍ (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    bha_nga;
	}
    }
    ပြန်ပေး pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    static pos_T *
find_start_rawstring(ကိန်းပြည့် ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ကိန်းပြည့်		cur_maxcomment = ind_maxcomment;

    အတွက် (;;)
    {
	pos = findmatchlimit(NULL, 'R', FM_BACKWARD, cur_maxcomment);
	အကယ်၍ (pos == NULL)
	    bha_nga;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	အကယ်၍ (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    bha_nga;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	အကယ်၍ (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    bha_nga;
	}
    }
    ပြန်ပေး pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    static pos_T *
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    static pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    အကယ်၍ (comment_pos != NULL)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    အကယ်၍ (comment_pos == NULL || (rs_pos != NULL
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	အကယ်၍ (is_raw != NULL && rs_pos != NULL)
	    *is_raw = rs_pos->lnum;
	ပြန်ပေး rs_pos;
    }
    ပြန်ပေး comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    ကိန်းပြည့်
cindent_on(void)
{
    ပြန်ပေး (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
typedef sa_nracanaa {
    ကိန်းပြည့်	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    static char_u *
cin_skipcomment(char_u *s)
{
    နေစဉ် (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	အကယ်၍ (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    bha_nga;
	}
	အကယ်၍ (*s != '/')
	    bha_nga;
	++s;
	အကယ်၍ (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    bha_nga;
	}
	အကယ်၍ (*s != '*')
	    bha_nga;
	အတွက် (++s; *s; ++s)	// skip slash-star comment
	    အကယ်၍ (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		bha_nga;
	    }
    }
    ပြန်ပေး s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    static ကိန်းပြည့်
cin_nocode(char_u *s)
{
    ပြန်ပေး *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    static ကိန်းပြည့်
cin_iscomment(char_u *p)
{
    ပြန်ပေး (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    static ကိန်းပြည့်
cin_islinecomment(char_u *p)
{
    ပြန်ပေး (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    static pos_T *
find_line_comment(void) // XXX
{
    static pos_T pos;
    char_u	 *line;
    char_u	 *p;

    pos = curwin->w_cursor;
    နေစဉ် (--pos.lnum > 0)
    {
	line = ml_get(pos.lnum);
	p = skipwhite(line);
	အကယ်၍ (cin_islinecomment(p))
	{
	    pos.col = (ကိန်းပြည့်)(p - line);
	    ပြန်ပေး &pos;
	}
	အကယ်၍ (*p != NUL)
	    bha_nga;
    }
    ပြန်ပေး NULL;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    static ကိန်းပြည့်
cin_has_js_key(char_u *text)
{
    char_u *s = skipwhite(text);
    ကိန်းပြည့်	    quote = -1;

    အကယ်၍ (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    အကယ်၍ (!vim_isIDc(*s))	    // need at least one ID character
	ပြန်ပေး FALSE;

    နေစဉ် (vim_isIDc(*s))
	++s;
    အကယ်၍ (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    ပြန်ပေး (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    static ကိန်းပြည့်
cin_islabel_skip(char_u **s)
{
    အကယ်၍ (!vim_isIDc(**s))	    // need at least one ID character
	ပြန်ပေး FALSE;

    နေစဉ် (vim_isIDc(**s))
    {
	အကယ်၍ (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	မဟုတ်ပါက
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    ပြန်ပေး (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    static ကိန်းပြည့်
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    ကိန်းပြည့်	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    အကယ်၍ (cinsd_buf == NULL)
	ပြန်ပေး FALSE;

    အတွက် (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (ကိန်းပြည့်)cinsd_len, ",");
	အကယ်၍ (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    အကယ်၍ (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		bha_nga;
	    }
	}
    }

    vim_free(cinsd_buf);
    ပြန်ပေး found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    static ကိန်းပြည့်
cin_ispreproc(char_u *s)
{
    အကယ်၍ (*skipwhite(s) == '#')
	ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    static ကိန်းပြည့်
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, ကိန်းပြည့် *amount)
{
    char_u	*line = *pp;
    linenr_T	lnum = *lnump;
    ကိန်းပြည့်		retval = FALSE;
    ကိန်းပြည့်		candidate_amount = *amount;

    အကယ်၍ (*line != NUL && line[STRLEN(line) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    အတွက် (;;)
    {
	အကယ်၍ (cin_ispreproc(line))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    bha_nga;
	}
	အကယ်၍ (lnum == 1)
	    bha_nga;
	line = ml_get(--lnum);
	အကယ်၍ (*line == NUL || line[STRLEN(line) - 1] != '\\')
	    bha_nga;
    }

    အကယ်၍ (lnum != *lnump)
	*pp = ml_get(*lnump);
    အကယ်၍ (retval)
	*amount = candidate_amount;
    ပြန်ပေး retval;
}

    static ကိန်းပြည့်
cin_iselse(
    char_u  *p)
{
    အကယ်၍ (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    ပြန်ပေး (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    static ကိန်းပြည့်
cin_isterminated(
    char_u	*s,
    ကိန်းပြည့်		incl_open,	// include '{' at the end as terminator
    ကိန်းပြည့်		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    ကိန်းပြည့်		is_else = FALSE;

    s = cin_skipcomment(s);

    အကယ်၍ (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    အကယ်၍ (!found_start)
	is_else = cin_iselse(s);

    နေစဉ် (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	အကယ်၍ (*s == '}' && n_open > 0)
	    --n_open;
	အကယ်၍ ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    ပြန်ပေး *s;
	မဟုတ်ပါက အကယ်၍ (*s == '{')
	{
	    အကယ်၍ (incl_open && cin_nocode(s + 1))
		ပြန်ပေး *s;
	    မဟုတ်ပါက
		++n_open;
	}

	အကယ်၍ (*s)
	    s++;
    }
    ပြန်ပေး found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    static ကိန်းပြည့်
cin_starts_with(char_u *s, var_na *word)
{
    ကိန်းပြည့် l = (ကိန်းပြည့်)STRLEN(word);

    ပြန်ပေး (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    static ကိန်းပြည့်
cin_isdefault(char_u *s)
{
    ပြန်ပေး (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    static ကိန်းပြည့်
cin_iscase(
    char_u *s,
    ကိန်းပြည့် strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    အကယ်၍ (cin_starts_with(s, "case"))
    {
	အတွက် (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    အကယ်၍ (*s == NUL)
		bha_nga;
	    အကယ်၍ (*s == ':')
	    {
		အကယ်၍ (s[1] == ':')	// skip over "::" for C++
		    ++s;
		မဟုတ်ပါက
		    ပြန်ပေး TRUE;
	    }
	    အကယ်၍ (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    မဟုတ်ပါက အကယ်၍ (*s == '/' && (s[1] == '*' || s[1] == '/'))
		ပြန်ပေး FALSE;		// stop at comment
	    မဟုတ်ပါက အကယ်၍ (*s == '"')
	    {
		// JS etc.
		အကယ်၍ (strict)
		    ပြန်ပေး FALSE;		// stop at string
		မဟုတ်ပါက
		    ပြန်ပေး TRUE;
	    }
	}
	ပြန်ပေး FALSE;
    }

    အကယ်၍ (cin_isdefault(s))
	ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    static ကိန်းပြည့်
cin_islabel(void)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    အကယ်၍ (cin_isdefault(s))
	ပြန်ပေး FALSE;
    အကယ်၍ (cin_isscopedecl(s))
	ပြန်ပေး FALSE;

    အကယ်၍ (!cin_islabel_skip(&s))
	ပြန်ပေး FALSE;

    အကယ်၍ (ind_find_start_CORS(NULL))
	ပြန်ပေး FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*line;

    cursor_save = curwin->w_cursor;
    နေစဉ် (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	အကယ်၍ ((trypos = ind_find_start_CORS(NULL)) != NULL) // XXX
	    curwin->w_cursor = *trypos;

	line = ml_get_curline();
	အကယ်၍ (cin_ispreproc(line))	// ignore #defines, #if, etc.
	    continue;
	အကယ်၍ (*(line = cin_skipcomment(line)) == NUL)
	    continue;

	curwin->w_cursor = cursor_save;
	အကယ်၍ (cin_isterminated(line, TRUE, FALSE)
		|| cin_isscopedecl(line)
		|| cin_iscase(line, TRUE)
		|| (cin_islabel_skip(&line) && cin_nocode(line)))
	    ပြန်ပေး TRUE;
	ပြန်ပေး FALSE;
    }
    curwin->w_cursor = cursor_save;
    ပြန်ပေး TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    static ကိန်းပြည့်
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    ကိန်းပြည့်		len = (ကိန်းပြည့်)STRLEN(find);

    နေစဉ် (*p != NUL)
    {
	p = cin_skipcomment(p);
	အကယ်၍ (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    အကယ်၍ (cin_nocode(r))
		ပြန်ပေး TRUE;
	}
	အကယ်၍ (*p != NUL)
	    ++p;
    }
    ပြန်ပေး FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    static char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = NULL, *p = s;
    do
    {
	r = p;
	p = cin_skipcomment(p);
	အကယ်၍ (*p)
	    p = skip_string(p);
    } နေစဉ် (p != r);
    ပြန်ပေး p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    static ကိန်းပြည့်
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = NULL;

    နေစဉ် (*p)
    {
	အကယ်၍ (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	မဟုတ်ပါက
	    p = cin_skip_comment_and_string(p + 1);
    }
    အကယ်၍ (!r)
	ပြန်ပေး FALSE;
    p = r; // p points now after '=' or "return"

    အကယ်၍ (cin_nocode(p))
	ပြန်ပေး TRUE;

    အကယ်၍ (*p == '&')
	p = cin_skipcomment(p + 1);

    အကယ်၍ (*p == '(') // skip a typecast
    {
	ကိန်းပြည့် open_count = 1;
	do
	{
	    p = cin_skip_comment_and_string(p + 1);
	    အကယ်၍ (cin_nocode(p))
		ပြန်ပေး TRUE;
	    open_count += (*p == '(') - (*p == ')');
	} နေစဉ် (open_count);
	p = cin_skipcomment(p + 1);
	အကယ်၍ (cin_nocode(p))
	    ပြန်ပေး TRUE;
    }

    နေစဉ် (*p == '{')
	p = cin_skipcomment(p + 1);
    ပြန်ပေး cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    static ကိန်းပြည့်
cin_isinit(void)
{
    char_u	*s;
    static var_na *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    အကယ်၍ (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    အတွက် (;;)
    {
	ကိန်းပြည့် i, l;

	အတွက် (i = 0; i < (ကိန်းပြည့်)ARRAY_LENGTH(skip); ++i)
	{
	    l = (ကိန်းပြည့်)strlen(skip[i]);
	    အကယ်၍ (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		bha_nga;
	    }
	}
	အကယ်၍ (l != 0)
	    bha_nga;
    }

    အကယ်၍ (cin_starts_with(s, "enum"))
	ပြန်ပေး TRUE;

    ပြန်ပေး cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    static ကိန်းပြည့်
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    ကိန်းပြည့်		has_name = FALSE;
    ကိန်းပြည့်		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    နေစဉ် ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    အကယ်၍ (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	နေစဉ် (*p != NUL)
	{
	    အကယ်၍ (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    မဟုတ်ပါက အကယ်၍ (*p == '{')
	    {
		bha_nga;
	    }
	    မဟုတ်ပါက အကယ်၍ (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		အကယ်၍ (has_name)
		    ပြန်ပေး FALSE; // word character after skipping past name
		++p;
	    }
	    မဟုတ်ပါက အကယ်၍ (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		အကယ်၍ (!has_name_start || has_name)
		    ပြန်ပေး FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    မဟုတ်ပါက
	    {
		ပြန်ပေး FALSE;
	    }
	}
	ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    static ကိန်းပြည့်
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    ကိန်းပြည့်		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    အကယ်၍ (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	နေစဉ် (*p != NUL)
	{
	    အကယ်၍ (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    မဟုတ်ပါက အကယ်၍ (*p == '{')
	    {
		bha_nga;
	    }
	    မဟုတ်ပါက အကယ်၍ (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		အကယ်၍ (has_string_literal)
		    ပြန်ပေး FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    မဟုတ်ပါက အကယ်၍ (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		အကယ်၍ (has_string_literal)
		    ပြန်ပေး FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    မဟုတ်ပါက
	    {
		ပြန်ပေး FALSE;
	    }
	}
	ပြန်ပေး has_string_literal ? TRUE : FALSE;
    }
    ပြန်ပေး FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    static char_u *
after_label(char_u *l)
{
    အတွက် ( ; *l; ++l)
    {
	အကယ်၍ (*l == ':')
	{
	    အကယ်၍ (l[1] == ':')	    // skip over "::" for C++
		++l;
	    မဟုတ်ပါက အကယ်၍ (!cin_iscase(l + 1, FALSE))
		bha_nga;
	}
	မဟုတ်ပါက အကယ်၍ (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    အကယ်၍ (*l == NUL)
	ပြန်ပေး NULL;
    l = cin_skipcomment(l + 1);
    အကယ်၍ (*l == NUL)
	ပြန်ပေး NULL;
    ပြန်ပေး l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    static ကိန်းပြည့်
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    အကယ်၍ (p == NULL)
	ပြန်ပေး 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    ပြန်ပေး (ကိန်းပြည့်)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    static ကိန်းပြည့်
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    ကိန်းပြည့်		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    အကယ်၍ (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	အကယ်၍ (l == NULL)		// just in case
	    l = ml_get_curline();
    }
    မဟုတ်ပါက
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    ပြန်ပေး amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    static ကိန်းပြည့်
cin_first_id_amount(void)
{
    char_u	*line, *p, *s;
    ကိန်းပြည့်		len;
    pos_T	fp;
    colnr_T	col;

    line = ml_get_curline();
    p = skipwhite(line);
    len = (ကိန်းပြည့်)(skiptowhite(p) - p);
    အကယ်၍ (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (ကိန်းပြည့်)(skiptowhite(p) - p);
    }
    အကယ်၍ (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    မဟုတ်ပါက အကယ်၍ (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    မဟုတ်ပါက အကယ်၍ ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	အကယ်၍ ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    အတွက် (len = 0; vim_isIDc(p[len]); ++len)
	;
    အကယ်၍ (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	ပြန်ပေး 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - line);
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    ပြန်ပေး (ကိန်းပြည့်)col;
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
    static ကိန်းပြည့်
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*line;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    အကယ်၍ (lnum > 1)
    {
	line = ml_get(lnum - 1);
	အကယ်၍ (*line != NUL && line[STRLEN(line) - 1] == '\\')
	    ပြန်ပေး -1;
    }

    line = s = ml_get(lnum);
    နေစဉ် (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == NULL)
    {
	အကယ်၍ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	မဟုတ်ပါက
	    ++s;
    }
    အကယ်၍ (*s != '=')
	ပြန်ပေး 0;

    s = skipwhite(s + 1);
    အကယ်၍ (cin_nocode(s))
	ပြန်ပေး 0;

    အကယ်၍ (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - line);
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    ပြန်ပေး (ကိန်းပြည့်)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    static ကိန်းပြည့်
cin_skip2pos(pos_T *trypos)
{
    char_u	*line;
    char_u	*p;
    char_u	*new_p;

    p = line = ml_get(trypos->lnum);
    နေစဉ် (*p && (colnr_T)(p - line) < trypos->col)
    {
	အကယ်၍ (cin_iscomment(p))
	    p = cin_skipcomment(p);
	မဟုတ်ပါက
	{
	    new_p = skip_string(p);
	    အကယ်၍ (new_p == p)
		++p;
	    မဟုတ်ပါက
		p = new_p;
	}
    }
    ပြန်ပေး (ကိန်းပြည့်)(p - line);
}

    static pos_T *
find_match_char(ကိန်းပြည့် c, ကိန်းပြည့် ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    static pos_T pos_copy;
    ကိန်းပြည့်		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    အကယ်၍ ((trypos = findmatchlimit(NULL, c, FM_SKIPCOMM, ind_maxp_wk)) != NULL)
    {
	// check if the ( is in a // comment
	အကယ်၍ ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (ကိန်းပြည့်)(cursor_save.lnum - trypos->lnum);
	    အကယ်၍ (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		goto retry;
	    }
	    trypos = NULL;
	}
	မဟုတ်ပါက
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    အကယ်၍ ((trypos_wk = ind_find_start_CORS(NULL)) != NULL) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (ကိန်းပြည့်)(cursor_save.lnum
			- trypos_wk->lnum);
		အကယ်၍ (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    goto retry;
		}
		trypos = NULL;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    ပြန်ပေး trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    static pos_T *
find_match_paren(ကိန်းပြည့် ind_maxparen)	// XXX
{
    ပြန်ပေး find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    static ကိန်းပြည့်
find_last_paren(char_u *l, ကိန်းပြည့် start, ကိန်းပြည့် end)
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		retval = FALSE;
    ကိန်းပြည့်		open_count = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    အတွက် (i = 0; l[i] != NUL; i++)
    {
	i = (ကိန်းပြည့်)(cin_skipcomment(l + i) - l); // ignore parens in comments
	အကယ်၍ (l[i] == NUL)
	    bha_nga;
	i = (ကိန်းပြည့်)(skip_string(l + i) - l);    // ignore parens in quotes
	အကယ်၍ (l[i] == start)
	    ++open_count;
	မဟုတ်ပါက အကယ်၍ (l[i] == end)
	{
	    အကယ်၍ (open_count > 0)
		--open_count;
	    မဟုတ်ပါက
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    ပြန်ပေး retval;
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
    static ကိန်းပြည့်
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    ကိန်းပြည့်		retval = FALSE;
    pos_T	*trypos;
    ကိန်းပြည့်		just_started = TRUE;

    အကယ်၍ (sp == NULL)
	s = ml_get(lnum);
    မဟုတ်ပါက
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    အကယ်၍ (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
    {
	lnum = trypos->lnum;
	အကယ်၍ (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    ပြန်ပေး FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    အကယ်၍ (cin_ispreproc(s))
	ပြန်ပေး FALSE;

    နေစဉ် (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	အကယ်၍ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	မဟုတ်ပါက အကယ်၍ (*s == ':')
	{
	    အကယ်၍ (*(s + 1) == ':')
		s += 2;
	    မဟုတ်ပါက
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		ပြန်ပေး FALSE;
	}
	မဟုတ်ပါက
	    ++s;
    }
    အကယ်၍ (*s != '(')
	ပြန်ပေး FALSE;		// ';', ' or "  before any () or no '('

    နေစဉ် (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	အကယ်၍ (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    အကယ်၍ (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    goto done;
	}
	အကယ်၍ ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    ကိန်းပြည့် comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    အတွက် (;;)
	    {
		အကယ်၍ (lnum >= curbuf->b_ml.ml_line_count)
		    bha_nga;
		s = ml_get(++lnum);
		အကယ်၍ (!cin_ispreproc(s))
		    bha_nga;
	    }
	    အကယ်၍ (lnum >= curbuf->b_ml.ml_line_count)
		bha_nga;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    အကယ်၍ (!just_started && (!comma && *s != ',' && *s != ')'))
		bha_nga;
	    just_started = FALSE;
	}
	မဟုတ်ပါက အကယ်၍ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	မဟုတ်ပါက
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    အကယ်၍ (lnum != first_lnum && sp != NULL)
	*sp = ml_get(first_lnum);

    ပြန်ပေး retval;
}

    static ကိန်းပြည့်
cin_isif(char_u *p)
{
    ပြန်ပေး (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    static ကိန်းပြည့်
cin_isdo(char_u *p)
{
    ပြန်ပေး (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    static ကိန်းပြည့်
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ကိန်းပြည့်		retval = FALSE;

    p = cin_skipcomment(p);
    အကယ်၍ (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    အကယ်၍ (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	နေစဉ် (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	အကယ်၍ ((trypos = findmatchlimit(NULL, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != NULL
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    ပြန်ပေး retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    static ကိန်းပြည့်
cin_is_if_for_while_before_offset(char_u *line, ကိန်းပြည့် *poffset)
{
    ကိန်းပြည့် offset = *poffset;

    အကယ်၍ (offset-- < 2)
	ပြန်ပေး 0;
    နေစဉ် (offset > 2 && VIM_ISWHITE(line[offset]))
	--offset;

    offset -= 1;
    အကယ်၍ (STRNCMP(line + offset, "if", 2) == 0)
	goto probablyFound;

    အကယ်၍ (offset >= 1)
    {
	offset -= 1;
	အကယ်၍ (STRNCMP(line + offset, "for", 3) == 0)
	    goto probablyFound;

	အကယ်၍ (offset >= 2)
	{
	    offset -= 2;
	    အကယ်၍ (STRNCMP(line + offset, "while", 5) == 0)
		goto probablyFound;
	}
    }
    ပြန်ပေး 0;

probablyFound:
    အကယ်၍ (!offset || !vim_isIDc(line[offset - 1]))
    {
	*poffset = offset;
	ပြန်ပေး 1;
    }
    ပြန်ပေး 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    static ကိန်းပြည့်
cin_iswhileofdo_end(ကိန်းပြည့် terminated)
{
    char_u	*line;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    ကိန်းပြည့်		i;

    အကယ်၍ (terminated != ';')	// there must be a ';' at the end
	ပြန်ပေး FALSE;

    p = line = ml_get_curline();
    နေစဉ် (*p != NUL)
    {
	p = cin_skipcomment(p);
	အကယ်၍ (*p == ')')
	{
	    s = skipwhite(p + 1);
	    အကယ်၍ (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (ကိန်းပြည့်)(p - line);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		အကယ်၍ (trypos != NULL)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    အကယ်၍ (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    အကယ်၍ (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			ပြန်ပေး TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		line = ml_get_curline();
		p = line + i;
	    }
	}
	အကယ်၍ (*p != NUL)
	    ++p;
    }
    ပြန်ပေး FALSE;
}

    static ကိန်းပြည့်
cin_isbreak(char_u *p)
{
    ပြန်ပေး (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    static ကိန်းပြည့်
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    ကိန်းပြည့်		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*line = ml_get_curline();

    အကယ်၍ (pos->lnum <= lnum)
	ပြန်ပေး cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(line);
    အကယ်၍ (*s == '#')		// skip #define FOO x ? (x) : x
	ပြန်ပေး FALSE;
    s = cin_skipcomment(s);
    အကယ်၍ (*s == NUL)
	ပြန်ပေး FALSE;

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
    နေစဉ် (lnum > 1)
    {
	line = ml_get(lnum - 1);
	s = skipwhite(line);
	အကယ်၍ (*s == '#' || *s == NUL)
	    bha_nga;
	နေစဉ် (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    အကယ်၍ (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		bha_nga;
	    အကယ်၍ (*s != NUL)
		++s;
	}
	အကယ်၍ (*s != NUL)
	    bha_nga;
	--lnum;
    }

    pos->lnum = lnum;
    line = ml_get(lnum);
    s = line;
    အတွက် (;;)
    {
	အကယ်၍ (*s == NUL)
	{
	    အကယ်၍ (lnum == curwin->w_cursor.lnum)
		bha_nga;
	    // Continue in the cursor line.
	    line = ml_get(++lnum);
	    s = line;
	}
	အကယ်၍ (s == line)
	{
	    // don't recognize "case (foo):" as a baseclass
	    အကယ်၍ (cin_iscase(s, FALSE))
		bha_nga;
	    s = cin_skipcomment(line);
	    အကယ်၍ (*s == NUL)
		continue;
	}

	အကယ်၍ (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	မဟုတ်ပါက အကယ်၍ (s[0] == ':')
	{
	    အကယ်၍ (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    မဟုတ်ပါက အကယ်၍ (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    မဟုတ်ပါက
		s = cin_skipcomment(s + 1);
	}
	မဟုတ်ပါက အကယ်၍ ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    အကယ်၍ (*s == 'c')
		s = cin_skipcomment(s + 5);
	    မဟုတ်ပါက
		s = cin_skipcomment(s + 6);
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    မဟုတ်ပါက အကယ်၍ (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    မဟုတ်ပါက အကယ်၍ (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		ပြန်ပေး FALSE;
	    }
	    မဟုတ်ပါက အကယ်၍ (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    မဟုတ်ပါက အကယ်၍ (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		အကယ်၍ (cpp_base_class)
		    pos->col = (colnr_T)(s - line);
	    }

	    // When the line ends in a comma don't align with it.
	    အကယ်၍ (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    အကယ်၍ (cpp_base_class)
	pos->lnum = lnum;
    ပြန်ပေး cpp_base_class;
}

    static ကိန်းပြည့်
get_baseclass_amount(ကိန်းပြည့် col)
{
    ကိန်းပြည့်		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    အကယ်၍ (col == 0)
    {
	amount = get_indent();
	အကယ်၍ (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	အကယ်၍ (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    မဟုတ်ပါက
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, NULL, NULL, 0);
	amount = (ကိန်းပြည့်)vcol;
    }
    အကယ်၍ (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    ပြန်ပေး amount;
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

    static pos_T *
find_start_brace(void)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    static pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    နေစဉ် ((trypos = findmatchlimit(NULL, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != NULL)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = NULL;
	// ignore the { if it's in a // or / *  * / comment
	အကယ်၍ ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(NULL)) == NULL) // XXX
	    bha_nga;
	အကယ်၍ (pos != NULL)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    ပြန်ပေး trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    static pos_T *
find_match_paren_after_brace (ကိန်းပြည့် ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    အကယ်၍ (trypos == NULL)
	ပြန်ပေး NULL;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    အကယ်၍ (tryposBrace != NULL
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = NULL;
    ပြန်ပေး trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    static ကိန်းပြည့်
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    အကယ်၍ (n > 0 && n < curbuf->b_ind_maxparen / 2)
	ပြန်ပေး curbuf->b_ind_maxparen - (ကိန်းပြည့်)n;
    ပြန်ပေး curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    void
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    ကိန်းပြည့်		divider;
    ကိန်းပြည့်		fraction = 0;
    ကိန်းပြည့်		sw;
    long	t = get_sw_value(buf);

    // needed for cino-(, it will be multiplied by 2 again
    အကယ်၍ (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    မဟုတ်ပါက
	sw = (ကိန်းပြည့်)t;

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
    buf->b_ind_open_left_imag = 0;

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

    အတွက် (p = buf->b_p_cino; *p; )
    {
	l = p++;
	အကယ်၍ (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	အကယ်၍ (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((var_na *)++p);
	    နေစဉ် (VIM_ISDIGIT(*p))
	    {
		++p;
		အကယ်၍ (divider)
		    divider *= 10;
		မဟုတ်ပါက
		    divider = 10;
	    }
	}
	အကယ်၍ (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    အကယ်၍ (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    မဟုတ်ပါက
	    {
		n *= sw;
		အကယ်၍ (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	အကယ်၍ (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	switch (*l)
	{
	    pak_sa '>': buf->b_ind_level = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'e': buf->b_ind_open_imag = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'n': buf->b_ind_no_brace = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'f': buf->b_ind_first_open = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa '{': buf->b_ind_open_extra = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa '}': buf->b_ind_close_extra = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa '^': buf->b_ind_open_left_imag = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'L': buf->b_ind_jump_label = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa ':': buf->b_ind_case = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa '=': buf->b_ind_case_code = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'b': buf->b_ind_case_break = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'p': buf->b_ind_param = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 't': buf->b_ind_func_type = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa '/': buf->b_ind_comment = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'c': buf->b_ind_in_comment = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'C': buf->b_ind_in_comment2 = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'i': buf->b_ind_cpp_baseclass = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa '+': buf->b_ind_continuation = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa '(': buf->b_ind_unclosed = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'u': buf->b_ind_unclosed2 = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'U': buf->b_ind_unclosed_noignore = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'W': buf->b_ind_unclosed_wrapped = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'w': buf->b_ind_unclosed_whiteok = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'm': buf->b_ind_matching_paren = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'M': buf->b_ind_paren_prev = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa ')': buf->b_ind_maxparen = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa '*': buf->b_ind_maxcomment = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'g': buf->b_ind_scopedecl = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'h': buf->b_ind_scopedecl_code = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'j': buf->b_ind_java = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'J': buf->b_ind_js = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'l': buf->b_ind_keep_case_label = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa '#': buf->b_ind_hash_comment = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'N': buf->b_ind_cpp_namespace = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'k': buf->b_ind_if_for_while = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'E': buf->b_ind_cpp_extern_c = (ကိန်းပြည့်)n; bha_nga;
	    pak_sa 'P': buf->b_ind_pragma = (ကိန်းပြည့်)n; bha_nga;
	}
	အကယ်၍ (*p == ',')
	    ++p;
    }
}

    static ကိန်းပြည့်
find_match(ကိန်းပြည့် lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    ကိန်းပြည့်		elselevel;
    ကိန်းပြည့်		whilelevel;

    အကယ်၍ (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    မဟုတ်ပါက
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    နေစဉ် (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	အကယ်၍ (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    အကယ်၍ (theirscope == NULL)
		bha_nga;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    အကယ်၍ (theirscope->lnum < ourscope)
		bha_nga;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    အကယ်၍ (theirscope->lnum > ourscope)
		continue;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    အကယ်၍ (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		အကယ်၍ (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    အကယ်၍ (!cin_isif(mightbeif))
			++elselevel;
		    continue;
		}

		// If it's an "if" decrement elselevel
		အကယ်၍ (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    အကယ်၍ (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    အကယ်၍ (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		continue;
	    }

	    // If it's a "do" decrement whilelevel
	    အကယ်၍ (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    အကယ်၍ (elselevel <= 0 && whilelevel <= 0)
		ပြန်ပေး OK;
	}
    }
    ပြန်ပေး FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    ကိန်းပြည့်
get_c_indent(void)
{
    pos_T	cur_curpos;
    ကိန်းပြည့်		amount;
    ကိန်းပြည့်		scope_amount;
    ကိန်းပြည့်		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = NULL;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    ကိန်းပြည့်		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    ကိန်းပြည့်		lookfor;
    ကိန်းပြည့်		whilelevel;
    linenr_T	lnum;
    ကိန်းပြည့်		n;
    ကိန်းပြည့်		iscase;
    ကိန်းပြည့်		lookfor_break;
    ကိန်းပြည့်		lookfor_cpp_namespace = FALSE;
    ကိန်းပြည့်		cont_amount = 0;    // amount for continuation line
    ကိန်းပြည့်		original_line_islabel;
    ကိန်းပြည့်		added_to_amount = 0;
    ကိန်းပြည့်		js_cur_has_key = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    ကိန်းပြည့်		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    အကယ်၍ (cur_curpos.lnum == 1)
	ပြန်ပေး 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    အကယ်၍ (linecopy == NULL)
	ပြန်ပေး 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    အကယ်၍ ((State & MODE_INSERT)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_line_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    အကယ်၍ (comment_pos != NULL)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    အကယ်၍ (trypos != NULL && (comment_pos == NULL
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	goto laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    အကယ်၍ (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	အကယ်၍ (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    goto theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    အကယ်၍ (original_line_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	goto theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    အကယ်၍ (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_line_comment(); // XXX
	အကယ်၍ (trypos == NULL && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    အကယ်၍ (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	အကယ်၍ (trypos  != NULL)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, NULL, NULL, 0);
	    amount = col;
	    goto theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    အကယ်၍ (!cin_iscomment(theline) && comment_pos != NULL) // XXX
    {
	ကိန်းပြည့်	lead_start_len = 2;
	ကိန်းပြည့်	lead_middle_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_middle[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	ကိန်းပြည့်	lead_end_len;
	char_u	*p;
	ကိန်းပြည့်	start_align = 0;
	ကိန်းပြည့်	start_off = 0;
	ကိန်းပြည့်	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, NULL, NULL, 0);
	amount = col;
	*lead_start = NUL;
	*lead_middle = NUL;

	p = curbuf->b_p_com;
	နေစဉ် (*p != NUL)
	{
	    ကိန်းပြည့်	align = 0;
	    ကိန်းပြည့်	off = 0;
	    ကိန်းပြည့် what = 0;

	    နေစဉ် (*p != NUL && *p != ':')
	    {
		အကယ်၍ (*p == COM_START || *p == COM_END || *p == COM_MIDDLE)
		    what = *p++;
		မဟုတ်ပါက အကယ်၍ (*p == COM_LEFT || *p == COM_RIGHT)
		    align = *p++;
		မဟုတ်ပါက အကယ်၍ (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		မဟုတ်ပါက
		    ++p;
	    }

	    အကယ်၍ (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    အကယ်၍ (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    မဟုတ်ပါက အကယ်၍ (what == COM_MIDDLE)
	    {
		STRCPY(lead_middle, lead_end);
		lead_middle_len = lead_end_len;
	    }
	    မဟုတ်ပါက အကယ်၍ (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		အကယ်၍ (STRNCMP(theline, lead_middle, lead_middle_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    အကယ်၍ (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			အကယ်၍ (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			မဟုတ်ပါက အကယ်၍ (STRNCMP(look, lead_middle,
							lead_middle_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    bha_nga;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			မဟုတ်ပါက အကယ်၍ (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    continue;
		    }
		    အကယ်၍ (start_off != 0)
			amount += start_off;
		    မဟုတ်ပါက အကယ်၍ (start_align == COM_RIGHT)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_middle);
		    bha_nga;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		အကယ်၍ (STRNCMP(theline, lead_middle, lead_middle_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    အကယ်၍ (off != 0)
			amount += off;
		    မဟုတ်ပါက အကယ်၍ (align == COM_RIGHT)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_middle);
		    done = TRUE;
		    bha_nga;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	အကယ်၍ (done)
	    ;
	မဟုတ်ပါက အကယ်၍ (theline[0] == '*')
	    amount += 1;
	မဟုတ်ပါက
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    အတွက် (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		အကယ်၍ (linewhite(lnum))		    // skip blank lines
		    continue;
		amount = get_indent_lnum(lnum);	    // XXX
		bha_nga;
	    }
	    အကယ်၍ (amount == -1)			    // use the comment opener
	    {
		အကယ်၍ (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    အကယ်၍ (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, NULL, NULL, 0);
		amount = col;
		အကယ်၍ (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	goto theend;
    }

    // Are we looking at a ']' that has a match?
    အကယ်၍ (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != NULL)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	goto theend;
    }

    // Are we inside parentheses or braces?  XXX
    အကယ်၍ (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != NULL
	    || trypos != NULL)
    {
      အကယ်၍ (trypos != NULL && tryposBrace != NULL)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  အကယ်၍ (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = NULL;
	  မဟုတ်ပါက
	      tryposBrace = NULL;
      }

      အကယ်၍ (trypos != NULL)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	အကယ်၍ (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	မဟုတ်ပါက
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    အတွက် (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		အကယ်၍ (cin_nocode(l))		// skip comment lines
		    continue;
		အကယ်၍ (cin_ispreproc_cont(&l, &lnum, &amount))
		    continue;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		အကယ်၍ ((trypos = ind_find_start_CORS(NULL)) != NULL)
		{
		    lnum = trypos->lnum + 1;
		    continue;
		}

		// XXX
		အကယ်၍ ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != NULL
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    အကယ်၍ (theline[0] == ')')
		    {
			အကယ်၍ (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    bha_nga;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	အကယ်၍ (amount == -1)
	{
	    ကိန်းပြည့်	    ignore_paren_col = 0;
	    ကိန်းပြည့်	    is_if_for_while = 0;

	    အကယ်၍ (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *line;

		trypos = &our_paren_pos;
		do
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} နေစဉ် (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		line = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(line, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    အကယ်၍ (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *line;
		ကိန်းပြည့်	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		line = ml_get_curline();
		look_col = (ကိန်းပြည့်)(look - line);
		curwin->w_cursor.col = look_col + 1;
		အကယ်၍ ((trypos = findmatchlimit(NULL, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != NULL
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    အကယ်၍ (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		အကယ်၍ (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    အကယ်၍ (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			အတွက် (col = 0; col < our_paren_pos.col; ++col)
			{
			    switch (l[col])
			    {
				pak_sa '(':
				pak_sa '{': ++n;
					  bha_nga;

				pak_sa ')':
				pak_sa '}': အကယ်၍ (n > 1)
					      --n;
					  bha_nga;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    မဟုတ်ပါက အကယ်၍ (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    မဟုတ်ပါက
		    {
			col = our_paren_pos.col + 1;
			နေစဉ် (VIM_ISWHITE(l[col]))
			    col++;
			အကယ်၍ (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			မဟုတ်ပါက
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		အကယ်၍ (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, NULL, NULL, 0);
		    အကယ်၍ (cur_amount > (ကိန်းပြည့်)col)
			cur_amount = col;
		}
	    }

	    အကယ်၍ (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    မဟုတ်ပါက အကယ်၍ ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		အကယ်၍ (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    မဟုတ်ပါက
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		နေစဉ် ((ကိန်းပြည့်)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    switch (*ml_get_pos(&our_paren_pos))
		    {
			pak_sa '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  bha_nga;
			pak_sa ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  bha_nga;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		အကယ်၍ (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		မဟုတ်ပါက
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    အကယ်၍ (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != NULL)
			amount += curbuf->b_ind_unclosed2;
		    မဟုတ်ပါက
		    {
			အကယ်၍ (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			မဟုတ်ပါက
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		အကယ်၍ (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	အကယ်၍ (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      မဟုတ်ပါက
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
	အကယ်၍ (*look == '{')
	{
	    getvcol(curwin, trypos, &col, NULL, NULL, 0);
	    amount = col;
	    အကယ်၍ (*start == '{')
		start_brace = BRACE_IN_COL0;
	    မဟုတ်ပါက
		start_brace = BRACE_AT_START;
	}
	မဟုတ်ပါက
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    အကယ်၍ (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != NULL)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    အကယ်၍ ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    မဟုတ်ပါက အကယ်၍ (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    မဟုတ်ပါက
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	အကယ်၍ (curbuf->b_ind_js)
	    js_cur_has_key = cin_has_js_key(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	အကယ်၍ (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	မဟုတ်ပါက
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    အကယ်၍ (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    မဟုတ်ပါက အကယ်၍ (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    အကယ်၍ (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		အကယ်၍ (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    goto theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    အကယ်၍ (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_left_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    မဟုတ်ပါက အကယ်၍ (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    မဟုတ်ပါက
	    {
		အကယ်၍ (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    အကယ်၍ (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    မဟုတ်ပါက အကယ်၍ (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		မဟုတ်ပါက
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    အကယ်၍ (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    အကယ်၍ (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    မဟုတ်ပါက အကယ်၍ (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    မဟုတ်ပါက
	    {
		အကယ်၍ (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    အတွက် (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		အကယ်၍ (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    အကယ်၍ (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			အကယ်၍ (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    အကယ်၍ (cont_amount > 0)
				amount = cont_amount;
			    မဟုတ်ပါက အကယ်၍ (!curbuf->b_ind_js)
				amount += ind_continuation;
			    bha_nga;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(NULL);
			အကယ်၍ (trypos != NULL)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    continue;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			အကယ်၍ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    continue;

			အကယ်၍ (cin_nocode(l))
			    continue;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			အကယ်၍ (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    အကယ်၍ (terminated == ',')
				bha_nga;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    အကယ်၍ (terminated != ';' && cin_isinit())
				bha_nga;

			    // nothing useful found
			    အကယ်၍ (terminated == 0 || terminated == '{')
				continue;
			}

			အကယ်၍ (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = NULL;
			    အကယ်၍ (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    အကယ်၍ (trypos == NULL && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    အကယ်၍ (trypos != NULL)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				continue;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			အကယ်၍ (cont_amount > 0)
			    amount = cont_amount;
			မဟုတ်ပါက
			    amount += ind_continuation;
		    }
		    မဟုတ်ပါက အကယ်၍ (lookfor == LOOKFOR_UNTERM)
		    {
			အကယ်၍ (cont_amount > 0)
			    amount = cont_amount;
			မဟုတ်ပါက
			    amount += ind_continuation;
		    }
		    မဟုတ်ပါက
		    {
			အကယ်၍ (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    အကယ်၍ (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			အကယ်၍ (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    အကယ်၍ (curwin->w_cursor.lnum == ourscope)
				continue;

			    အကယ်၍ (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				bha_nga;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(NULL);
			    အကယ်၍ (trypos != NULL)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				continue;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    အကယ်၍ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				continue;

			    // Finally the actual check for "namespace".
			    အကယ်၍ (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				bha_nga;
			    }
			    မဟုတ်ပါက အကယ်၍ (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				bha_nga;
			    }

			    အကယ်၍ (cin_nocode(l))
				continue;
			}
		    }
		    bha_nga;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		အကယ်၍ ((trypos = ind_find_start_CORS(&raw_string_start)) != NULL)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    continue;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		အကယ်၍ (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    အကယ်၍ (lookfor == LOOKFOR_CPP_BASECLASS)
			bha_nga;

		    // When looking for a "do" we are not interested in
		    // labels.
		    အကယ်၍ (whilelevel > 0)
			continue;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    အကယ်၍ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			အကယ်၍ (cont_amount > 0)
			    amount = cont_amount;
			မဟုတ်ပါက
			    amount += ind_continuation;
			bha_nga;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    အကယ်၍ (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			အကယ်၍ ((trypos = find_start_brace()) == NULL
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    bha_nga;
			}
			continue;
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
		    အကယ်၍ (lookfor == LOOKFOR_TERM)
		    {
			အကယ်၍ (n)
			    amount = n;

			အကယ်၍ (!lookfor_break)
			    bha_nga;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    အကယ်၍ (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			အကယ်၍ (l != NULL && cin_is_cinword(l))
			{
			    အကယ်၍ (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    မဟုတ်ပါက
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			bha_nga;
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
		    continue;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		အကယ်၍ (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    အကယ်၍ (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != NULL)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    continue;
		}

		// Ignore jump labels with nothing after them.
		အကယ်၍ (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    အကယ်၍ (l == NULL || cin_nocode(l))
			continue;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		အကယ်၍ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    continue;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		အကယ်၍ (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		အကယ်၍ (n)
		{
		    အကယ်၍ (lookfor == LOOKFOR_UNTERM)
		    {
			အကယ်၍ (cont_amount > 0)
			    amount = cont_amount;
			မဟုတ်ပါက
			    amount += ind_continuation;
		    }
		    မဟုတ်ပါက အကယ်၍ (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			continue;
		    }
		    မဟုတ်ပါက
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    bha_nga;
		}
		မဟုတ်ပါက အကယ်၍ (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    အကယ်၍ (cin_isterminated(l, TRUE, FALSE))
			bha_nga;
		    မဟုတ်ပါက
			continue;
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

		အကယ်၍ (js_cur_has_key)
		{
		    js_cur_has_key = 0; // only check the first line
		    အကယ်၍ (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_KEY;
		    }
		}
		အကယ်၍ (lookfor == LOOKFOR_JS_KEY && cin_has_js_key(l))
		{
		    amount = get_indent();
		    bha_nga;
		}
		အကယ်၍ (lookfor == LOOKFOR_COMMA)
		{
		    အကယ်၍ (tryposBrace != NULL && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			bha_nga;
		    အကယ်၍ (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			bha_nga;
		    မဟုတ်ပါက
		    {
			amount = get_indent();
			အကယ်၍ (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    bha_nga;
		    }
		}

		အကယ်၍ (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    အကယ်၍ (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (void)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    အကယ်၍ (trypos != NULL && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = NULL;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    အကယ်၍ (trypos == NULL && terminated == ',')
		    {
			အကယ်၍ (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    အကယ်၍ (trypos != NULL)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			အကယ်၍ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    continue;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    အကယ်၍ (terminated == ',')
		    {
			နေစဉ် (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    အကယ်၍ (*l == NUL || l[STRLEN(l) - 1] != '\\')
				bha_nga;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    အကယ်၍ (curbuf->b_ind_js)
			cur_amount = get_indent();
		    မဟုတ်ပါက
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    အကယ်၍ (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			အကယ်၍ (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			အကယ်၍ (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    continue;
			}
			bha_nga;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    အကယ်၍ (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			အကယ်၍ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    အကယ်၍ (cont_amount > 0)
				amount = cont_amount;
			    မဟုတ်ပါက
				amount += ind_continuation;
			    bha_nga;
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
			အကယ်၍ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			အကယ်၍ (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    bha_nga;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			အကယ်၍ (cin_isdo(l))
			{
			    အကယ်၍ (whilelevel == 0)
				bha_nga;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			အကယ်၍ (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    အကယ်၍ (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    အကယ်၍ ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				bha_nga;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    မဟုတ်ပါက
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			အကယ်၍ (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    အကယ်၍ (terminated == ',')
				amount += ind_continuation;
			    bha_nga;
			}

			အကယ်၍ (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    အကယ်၍ (terminated == ',')
			    {
				အကယ်၍ (curbuf->b_ind_cpp_baseclass == 0)
				    bha_nga;

				lookfor = LOOKFOR_CPP_BASECLASS;
				continue;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    အကယ်၍ (amount > cur_amount)
				amount = cur_amount;
			}
			မဟုတ်ပါက
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (ကိန်းပြည့်)STRLEN(l);
			    အကယ်၍ (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				bha_nga;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    အကယ်၍ (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				အကယ်၍ (curbuf->b_ind_js)
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
				    အကယ်၍ (cin_iscomment(skipwhite(l)))
					bha_nga;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    အကယ်၍ (trypos != NULL)
				    {
					အကယ်၍ (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    bha_nga;
					}
					ourscope = trypos->lnum;
				    }
				}
				မဟုတ်ပါက
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    မဟုတ်ပါက
			    {
				အကယ်၍ (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				အကယ်၍ (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_KEY
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		မဟုတ်ပါက အကယ်၍ (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    အကယ်၍ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			အကယ်၍ (cont_amount > 0)
			    amount = cont_amount;
			မဟုတ်ပါက
			    amount += ind_continuation;
			bha_nga;
		    }

		    အကယ်၍ (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			အကယ်၍ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		မဟုတ်ပါက
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    အကယ်၍ (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			continue;
		    }

		    // Handle "do {" line.
		    အကယ်၍ (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			အကယ်၍ (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    continue;
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
		    အကယ်၍ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			အကယ်၍ (cont_amount > 0)
			    amount = cont_amount;
			မဟုတ်ပါက
			    amount += ind_continuation;
			bha_nga;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    အကယ်၍ (lookfor == LOOKFOR_TERM)
		    {
			အကယ်၍ (!lookfor_break && whilelevel == 0)
			    bha_nga;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    မဟုတ်ပါက
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			အကယ်၍ (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != NULL)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    အကယ်၍ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				continue;
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

			အကယ်၍ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			အကယ်၍ (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			အကယ်၍ (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    အကယ်၍ ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				bha_nga;
			    continue;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			အကယ်၍ (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != NULL)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    အကယ်၍ (*l == '}' || !cin_iselse(l))
				goto term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      အကယ်၍ (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      အကယ်၍ (curbuf->b_ind_jump_label > 0 && original_line_islabel)
	  amount -= curbuf->b_ind_jump_label;

      goto theend;
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

    အကယ်၍ (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	goto theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    အကယ်၍ (cur_curpos.lnum < curbuf->b_ml.ml_line_count
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == NULL
	    && vim_strchr(theline, '}') == NULL
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(NULL, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	goto theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    နေစဉ် (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	အကယ်၍ ((trypos = ind_find_start_CORS(NULL)) != NULL)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    continue;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	အကယ်၍ (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	အကယ်၍ (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    bha_nga;
	}

	// Skip preprocessor directives and blank lines.
	အကယ်၍ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    continue;

	အကယ်၍ (cin_nocode(l))
	    continue;

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
	အကယ်၍ (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    အကယ်၍ (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != NULL)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    နေစဉ် (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		အကယ်၍ (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    bha_nga;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    အကယ်၍ (amount == 0)
		amount = cin_first_id_amount();
	    အကယ်၍ (amount == 0)
		amount = ind_continuation;
	    bha_nga;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	အကယ်၍ (cin_isfuncdecl(NULL, cur_curpos.lnum, 0))  // XXX
	    bha_nga;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	အကယ်၍ (*skipwhite(l) == '}')
	    bha_nga;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	အကယ်၍ (cin_ends_in(l, (char_u *)"};"))
	    bha_nga;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	အကယ်၍ (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    bha_nga;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	အကယ်၍ (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    နေစဉ် (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		အကယ်၍ (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    bha_nga;
	    }
	    အကယ်၍ (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		bha_nga;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	အကယ်၍ (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    bha_nga;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	အကယ်၍ (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    အကယ်၍ (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		bha_nga;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	အကယ်၍ ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	bha_nga;
    }

    // add extra indent for a comment
    အကယ်၍ (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    အကယ်၍ (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	အကယ်၍ (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    အကယ်၍ (cur_amount > 0)
		amount = cur_amount;
	    မဟုတ်ပါက အကယ်၍ (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    အကယ်၍ (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    ပြန်ပေး amount;
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
    ကိန်းပြည့်
in_cinkeys(
    ကိန်းပြည့်		keytyped,
    ကိန်းပြည့်		when,
    ကိန်းပြည့်		line_is_empty)
{
    char_u	*look;
    ကိန်းပြည့်		try_match;
    ကိန်းပြည့်		try_match_word;
    char_u	*p;
    char_u	*line;
    ကိန်းပြည့်		icase;
    ကိန်းပြည့်		i;

    အကယ်၍ (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	ပြန်ပေး FALSE;

#ifdef FEAT_EVAL
    အကယ်၍ (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    မဟုတ်ပါက
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    နေစဉ် (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	switch (when)
	{
	    pak_sa '*': try_match = (*look == '*'); bha_nga;
	    pak_sa '!': try_match = (*look == '!'); bha_nga;
	    default: try_match = (*look != '*'); bha_nga;
	}
	အကယ်၍ (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	အကယ်၍ (*look == '0')
	{
	    try_match_word = try_match;
	    အကယ်၍ (!line_is_empty)
		try_match = FALSE;
	    ++look;
	}
	မဟုတ်ပါက
	    try_match_word = FALSE;

	// does it look like a control character?
	အကယ်၍ (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    အကယ်၍ (try_match && keytyped == Ctrl_chr(look[1]))
		ပြန်ပေး TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	မဟုတ်ပါက အကယ်၍ (*look == 'o')
	{
	    အကယ်၍ (try_match && keytyped == KEY_OPEN_FORW)
		ပြန်ပေး TRUE;
	    ++look;
	}
	မဟုတ်ပါက အကယ်၍ (*look == 'O')
	{
	    အကယ်၍ (try_match && keytyped == KEY_OPEN_BACK)
		ပြန်ပေး TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	မဟုတ်ပါက အကယ်၍ (*look == 'e')
	{
	    အကယ်၍ (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		အကယ်၍ (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    ပြန်ပေး TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	မဟုတ်ပါက အကယ်၍ (*look == ':')
	{
	    အကယ်၍ (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		အကယ်၍ (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    ပြန်ပေး TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		အကယ်၍ (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    အကယ်၍ (i)
			ပြန်ပေး TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	မဟုတ်ပါက အကယ်၍ (*look == '<')
	{
	    အကယ်၍ (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		အကယ်၍ (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != NULL
						       && keytyped == look[1])
		    ပြန်ပေး TRUE;

		အကယ်၍ (keytyped == get_special_key_code(look + 1))
		    ပြန်ပေး TRUE;
	    }
	    နေစဉ် (*look && *look != '>')
		look++;
	    နေစဉ် (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	မဟုတ်ပါက အကယ်၍ (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    အကယ်၍ (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    မဟုတ်ပါက
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    အကယ်၍ (p == NULL)
		p = look + STRLEN(look);
	    အကယ်၍ ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		ကိန်းပြည့်		match = FALSE;

		အကယ်၍ (keytyped == KEY_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    line = ml_get_curline();
		    အကယ်၍ (has_mbyte)
		    {
			char_u	*n;

			အတွက် (s = line + curwin->w_cursor.col; s > line; s = n)
			{
			    n = mb_prevptr(line, s);
			    အကယ်၍ (!vim_iswordp(n))
				bha_nga;
			}
		    }
		    မဟုတ်ပါက
			အတွက် (s = line + curwin->w_cursor.col; s > line; --s)
			    အကယ်၍ (!vim_iswordc(s[-1]))
				bha_nga;
		    အကယ်၍ (s + (p - look) <= line + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		မဟုတ်ပါက
		    // TODO: multi-byte
		    အကယ်၍ (keytyped == (ကိန်းပြည့်)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((ကိန်းပြည့်)p[-1])))
		{
		    line = ml_get_cursor();
		    အကယ်၍ ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(line[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(line - (p - look), look, p - look)
				: STRNCMP(line - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		အကယ်၍ (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    အကယ်၍ (getwhitecols_curline() !=
				     (ကိန်းပြည့်)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		အကယ်၍ (match)
		    ပြန်ပေး TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	မဟုတ်ပါက
	{
	    အကယ်၍ (try_match && *look == keytyped)
		ပြန်ပေး TRUE;
	    အကယ်၍ (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    ပြန်ပေး FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    void
do_c_expr_indent(void)
{
#ifdef FEAT_EVAL
    အကယ်၍ (*curbuf->b_p_inde != NUL)
	fixthisline(get_expr_indent);
    မဟုတ်ပါက
#endif
	fixthisline(get_c_indent);
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    void
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    အကယ်၍ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    အကယ်၍ (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_number = get_c_indent();
	curwin->w_cursor = pos;
    }
    မဟုတ်ပါက
	rettv->vval.v_number = -1;
}
#endif
