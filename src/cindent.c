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
    ཧྲིལ
cin_is_cinword(char_u *line)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    ཧྲིལ		cinw_len;
    ཧྲིལ		retval = FALSE;
    ཧྲིལ		len;

    cinw_len = (ཧྲིལ)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    གལ་སྲིད (cinw_buf == NULL)
	ལོག FALSE;

    line = skipwhite(line);
    དོན་ལུ (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	གལ་སྲིད (STRNCMP(line, cinw_buf, len) == 0
		&& (!vim_iswordc(line[len]) || !vim_iswordc(line[len - 1])))
	{
	    retval = TRUE;
	    bha_nga;
	}
    }
    vim_free(cinw_buf);
    ལོག retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    static char_u *
skip_string(char_u *p)
{
    ཧྲིལ	    i;

    // We loop, because strings may be concatenated: "date""time".
    དོན་ལུ ( ; ; ++p)
    {
	གལ་སྲིད (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    གལ་སྲིད (p[1] == NUL)		    // ' at end of line
		bha_nga;
	    i = 2;
	    གལ་སྲིད (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		སྐབས (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    གལ་སྲིད (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		continue;
	    }
	}
	དེ་མིན གལ་སྲིད (p[0] == '"')		    // start of string
	{
	    དོན་ལུ (++p; p[0]; ++p)
	    {
		གལ་སྲིད (p[0] == '\\' && p[1] != NUL)
		    ++p;
		དེ་མིན གལ་སྲིད (p[0] == '"')	    // end of string
		    bha_nga;
	    }
	    གལ་སྲིད (p[0] == '"')
		continue; // continue for another string
	}
	དེ་མིན གལ་སྲིད (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    གལ་སྲིད (paren != NULL)
	    {
		size_t delim_len = paren - delim;

		དོན་ལུ (p += 3; *p; ++p)
		    གལ་སྲིད (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			bha_nga;
		    }
		གལ་སྲིད (p[0] == '"')
		    continue; // continue for another string
	    }
	}
	bha_nga;				    // no string found
    }
    གལ་སྲིད (!*p)
	--p;				    // backup from NUL
    ལོག p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    ཧྲིལ
is_pos_in_string(char_u *line, colnr_T col)
{
    char_u *p;

    དོན་ལུ (p = line; *p && (colnr_T)(p - line) < col; ++p)
	p = skip_string(p);
    ལོག !((colnr_T)(p - line) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    ཧྲིལ
check_linecomment(char_u *line)
{
    char_u  *p;

    p = line;
    // skip Lispish one-line comments
    གལ་སྲིད (curbuf->b_p_lisp)
    {
	གལ་སྲིད (vim_strchr(p, ';') != NULL) // there may be comments
	{
	    ཧྲིལ in_str = FALSE;	// inside of string

	    p = line;		// scan from start
	    སྐབས ((p = vim_strpbrk(p, (char_u *)"\";")) != NULL)
	    {
		གལ་སྲིད (*p == '"')
		{
		    གལ་སྲིད (in_str)
		    {
			གལ་སྲིད (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    དེ་མིན གལ་སྲིད (p == line || ((p - line) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		དེ་མིན གལ་སྲིད (!in_str && ((p - line) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(line, (colnr_T)(p - line)))
		    bha_nga;	// found!
		++p;
	    }
	}
	དེ་མིན
	    p = NULL;
    }
    དེ་མིན
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	དོན་ལུ ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    གལ་སྲིད (*p == NUL)
		bha_nga;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    གལ་སྲིད (p[0] == '/' && p[1] == '/'
				 && (p == line || p[-1] != '*' || p[2] != '*'))
		bha_nga;
	}
	གལ་སྲིད (*p == NUL)
	    p = NULL;
    }

    གལ་སྲིད (p == NULL)
	ལོག MAXCOL;
    ལོག (ཧྲིལ)(p - line);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    static pos_T *
ind_find_start_comment(void)	    // XXX
{
    ལོག find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(ཧྲིལ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ཧྲིལ		cur_maxcomment = ind_maxcomment;

    དོན་ལུ (;;)
    {
	pos = findmatchlimit(NULL, '*', FM_BACKWARD, cur_maxcomment);
	གལ་སྲིད (pos == NULL)
	    bha_nga;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	གལ་སྲིད (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    bha_nga;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	གལ་སྲིད (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    bha_nga;
	}
    }
    ལོག pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    static pos_T *
find_start_rawstring(ཧྲིལ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ཧྲིལ		cur_maxcomment = ind_maxcomment;

    དོན་ལུ (;;)
    {
	pos = findmatchlimit(NULL, 'R', FM_BACKWARD, cur_maxcomment);
	གལ་སྲིད (pos == NULL)
	    bha_nga;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	གལ་སྲིད (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    bha_nga;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	གལ་སྲིད (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    bha_nga;
	}
    }
    ལོག pos;
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
    གལ་སྲིད (comment_pos != NULL)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    གལ་སྲིད (comment_pos == NULL || (rs_pos != NULL
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	གལ་སྲིད (is_raw != NULL && rs_pos != NULL)
	    *is_raw = rs_pos->lnum;
	ལོག rs_pos;
    }
    ལོག comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    ཧྲིལ
cindent_on(void)
{
    ལོག (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
typedef sa_nracanaa {
    ཧྲིལ	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    static char_u *
cin_skipcomment(char_u *s)
{
    སྐབས (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	གལ་སྲིད (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    bha_nga;
	}
	གལ་སྲིད (*s != '/')
	    bha_nga;
	++s;
	གལ་སྲིད (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    bha_nga;
	}
	གལ་སྲིད (*s != '*')
	    bha_nga;
	དོན་ལུ (++s; *s; ++s)	// skip slash-star comment
	    གལ་སྲིད (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		bha_nga;
	    }
    }
    ལོག s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    static ཧྲིལ
cin_nocode(char_u *s)
{
    ལོག *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    static ཧྲིལ
cin_iscomment(char_u *p)
{
    ལོག (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    static ཧྲིལ
cin_islinecomment(char_u *p)
{
    ལོག (p[0] == '/' && p[1] == '/');
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
    སྐབས (--pos.lnum > 0)
    {
	line = ml_get(pos.lnum);
	p = skipwhite(line);
	གལ་སྲིད (cin_islinecomment(p))
	{
	    pos.col = (ཧྲིལ)(p - line);
	    ལོག &pos;
	}
	གལ་སྲིད (*p != NUL)
	    bha_nga;
    }
    ལོག NULL;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    static ཧྲིལ
cin_has_js_key(char_u *text)
{
    char_u *s = skipwhite(text);
    ཧྲིལ	    quote = -1;

    གལ་སྲིད (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    གལ་སྲིད (!vim_isIDc(*s))	    // need at least one ID character
	ལོག FALSE;

    སྐབས (vim_isIDc(*s))
	++s;
    གལ་སྲིད (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    ལོག (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    static ཧྲིལ
cin_islabel_skip(char_u **s)
{
    གལ་སྲིད (!vim_isIDc(**s))	    // need at least one ID character
	ལོག FALSE;

    སྐབས (vim_isIDc(**s))
    {
	གལ་སྲིད (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	དེ་མིན
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    ལོག (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    static ཧྲིལ
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    ཧྲིལ	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    གལ་སྲིད (cinsd_buf == NULL)
	ལོག FALSE;

    དོན་ལུ (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (ཧྲིལ)cinsd_len, ",");
	གལ་སྲིད (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    གལ་སྲིད (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		bha_nga;
	    }
	}
    }

    vim_free(cinsd_buf);
    ལོག found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    static ཧྲིལ
cin_ispreproc(char_u *s)
{
    གལ་སྲིད (*skipwhite(s) == '#')
	ལོག TRUE;
    ལོག FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    static ཧྲིལ
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, ཧྲིལ *amount)
{
    char_u	*line = *pp;
    linenr_T	lnum = *lnump;
    ཧྲིལ		retval = FALSE;
    ཧྲིལ		candidate_amount = *amount;

    གལ་སྲིད (*line != NUL && line[STRLEN(line) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    དོན་ལུ (;;)
    {
	གལ་སྲིད (cin_ispreproc(line))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    bha_nga;
	}
	གལ་སྲིད (lnum == 1)
	    bha_nga;
	line = ml_get(--lnum);
	གལ་སྲིད (*line == NUL || line[STRLEN(line) - 1] != '\\')
	    bha_nga;
    }

    གལ་སྲིད (lnum != *lnump)
	*pp = ml_get(*lnump);
    གལ་སྲིད (retval)
	*amount = candidate_amount;
    ལོག retval;
}

    static ཧྲིལ
cin_iselse(
    char_u  *p)
{
    གལ་སྲིད (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    ལོག (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    static ཧྲིལ
cin_isterminated(
    char_u	*s,
    ཧྲིལ		incl_open,	// include '{' at the end as terminator
    ཧྲིལ		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    ཧྲིལ		is_else = FALSE;

    s = cin_skipcomment(s);

    གལ་སྲིད (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    གལ་སྲིད (!found_start)
	is_else = cin_iselse(s);

    སྐབས (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	གལ་སྲིད (*s == '}' && n_open > 0)
	    --n_open;
	གལ་སྲིད ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    ལོག *s;
	དེ་མིན གལ་སྲིད (*s == '{')
	{
	    གལ་སྲིད (incl_open && cin_nocode(s + 1))
		ལོག *s;
	    དེ་མིན
		++n_open;
	}

	གལ་སྲིད (*s)
	    s++;
    }
    ལོག found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    static ཧྲིལ
cin_starts_with(char_u *s, var_na *word)
{
    ཧྲིལ l = (ཧྲིལ)STRLEN(word);

    ལོག (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    static ཧྲིལ
cin_isdefault(char_u *s)
{
    ལོག (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    static ཧྲིལ
cin_iscase(
    char_u *s,
    ཧྲིལ strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    གལ་སྲིད (cin_starts_with(s, "case"))
    {
	དོན་ལུ (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    གལ་སྲིད (*s == NUL)
		bha_nga;
	    གལ་སྲིད (*s == ':')
	    {
		གལ་སྲིད (s[1] == ':')	// skip over "::" for C++
		    ++s;
		དེ་མིན
		    ལོག TRUE;
	    }
	    གལ་སྲིད (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    དེ་མིན གལ་སྲིད (*s == '/' && (s[1] == '*' || s[1] == '/'))
		ལོག FALSE;		// stop at comment
	    དེ་མིན གལ་སྲིད (*s == '"')
	    {
		// JS etc.
		གལ་སྲིད (strict)
		    ལོག FALSE;		// stop at string
		དེ་མིན
		    ལོག TRUE;
	    }
	}
	ལོག FALSE;
    }

    གལ་སྲིད (cin_isdefault(s))
	ལོག TRUE;
    ལོག FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    static ཧྲིལ
cin_islabel(void)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    གལ་སྲིད (cin_isdefault(s))
	ལོག FALSE;
    གལ་སྲིད (cin_isscopedecl(s))
	ལོག FALSE;

    གལ་སྲིད (!cin_islabel_skip(&s))
	ལོག FALSE;

    གལ་སྲིད (ind_find_start_CORS(NULL))
	ལོག FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*line;

    cursor_save = curwin->w_cursor;
    སྐབས (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	གལ་སྲིད ((trypos = ind_find_start_CORS(NULL)) != NULL) // XXX
	    curwin->w_cursor = *trypos;

	line = ml_get_curline();
	གལ་སྲིད (cin_ispreproc(line))	// ignore #defines, #if, etc.
	    continue;
	གལ་སྲིད (*(line = cin_skipcomment(line)) == NUL)
	    continue;

	curwin->w_cursor = cursor_save;
	གལ་སྲིད (cin_isterminated(line, TRUE, FALSE)
		|| cin_isscopedecl(line)
		|| cin_iscase(line, TRUE)
		|| (cin_islabel_skip(&line) && cin_nocode(line)))
	    ལོག TRUE;
	ལོག FALSE;
    }
    curwin->w_cursor = cursor_save;
    ལོག TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    static ཧྲིལ
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    ཧྲིལ		len = (ཧྲིལ)STRLEN(find);

    སྐབས (*p != NUL)
    {
	p = cin_skipcomment(p);
	གལ་སྲིད (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    གལ་སྲིད (cin_nocode(r))
		ལོག TRUE;
	}
	གལ་སྲིད (*p != NUL)
	    ++p;
    }
    ལོག FALSE;
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
	གལ་སྲིད (*p)
	    p = skip_string(p);
    } སྐབས (p != r);
    ལོག p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    static ཧྲིལ
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = NULL;

    སྐབས (*p)
    {
	གལ་སྲིད (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	དེ་མིན གལ་སྲིད (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	དེ་མིན
	    p = cin_skip_comment_and_string(p + 1);
    }
    གལ་སྲིད (!r)
	ལོག FALSE;
    p = r; // p points now after '=' or "return"

    གལ་སྲིད (cin_nocode(p))
	ལོག TRUE;

    གལ་སྲིད (*p == '&')
	p = cin_skipcomment(p + 1);

    གལ་སྲིད (*p == '(') // skip a typecast
    {
	ཧྲིལ open_count = 1;
	do
	{
	    p = cin_skip_comment_and_string(p + 1);
	    གལ་སྲིད (cin_nocode(p))
		ལོག TRUE;
	    open_count += (*p == '(') - (*p == ')');
	} སྐབས (open_count);
	p = cin_skipcomment(p + 1);
	གལ་སྲིད (cin_nocode(p))
	    ལོག TRUE;
    }

    སྐབས (*p == '{')
	p = cin_skipcomment(p + 1);
    ལོག cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    static ཧྲིལ
cin_isinit(void)
{
    char_u	*s;
    static var_na *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    གལ་སྲིད (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    དོན་ལུ (;;)
    {
	ཧྲིལ i, l;

	དོན་ལུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(skip); ++i)
	{
	    l = (ཧྲིལ)strlen(skip[i]);
	    གལ་སྲིད (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		bha_nga;
	    }
	}
	གལ་སྲིད (l != 0)
	    bha_nga;
    }

    གལ་སྲིད (cin_starts_with(s, "enum"))
	ལོག TRUE;

    ལོག cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    static ཧྲིལ
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    ཧྲིལ		has_name = FALSE;
    ཧྲིལ		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    སྐབས ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    གལ་སྲིད (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	སྐབས (*p != NUL)
	{
	    གལ་སྲིད (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    དེ་མིན གལ་སྲིད (*p == '{')
	    {
		bha_nga;
	    }
	    དེ་མིན གལ་སྲིད (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		གལ་སྲིད (has_name)
		    ལོག FALSE; // word character after skipping past name
		++p;
	    }
	    དེ་མིན གལ་སྲིད (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		གལ་སྲིད (!has_name_start || has_name)
		    ལོག FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    དེ་མིན
	    {
		ལོག FALSE;
	    }
	}
	ལོག TRUE;
    }
    ལོག FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    static ཧྲིལ
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    ཧྲིལ		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    གལ་སྲིད (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	སྐབས (*p != NUL)
	{
	    གལ་སྲིད (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    དེ་མིན གལ་སྲིད (*p == '{')
	    {
		bha_nga;
	    }
	    དེ་མིན གལ་སྲིད (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		གལ་སྲིད (has_string_literal)
		    ལོག FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    དེ་མིན གལ་སྲིད (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		གལ་སྲིད (has_string_literal)
		    ལོག FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    དེ་མིན
	    {
		ལོག FALSE;
	    }
	}
	ལོག has_string_literal ? TRUE : FALSE;
    }
    ལོག FALSE;
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
    དོན་ལུ ( ; *l; ++l)
    {
	གལ་སྲིད (*l == ':')
	{
	    གལ་སྲིད (l[1] == ':')	    // skip over "::" for C++
		++l;
	    དེ་མིན གལ་སྲིད (!cin_iscase(l + 1, FALSE))
		bha_nga;
	}
	དེ་མིན གལ་སྲིད (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    གལ་སྲིད (*l == NUL)
	ལོག NULL;
    l = cin_skipcomment(l + 1);
    གལ་སྲིད (*l == NUL)
	ལོག NULL;
    ལོག l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    static ཧྲིལ
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    གལ་སྲིད (p == NULL)
	ལོག 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    ལོག (ཧྲིལ)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    static ཧྲིལ
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    ཧྲིལ		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    གལ་སྲིད (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	གལ་སྲིད (l == NULL)		// just in case
	    l = ml_get_curline();
    }
    དེ་མིན
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    ལོག amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    static ཧྲིལ
cin_first_id_amount(void)
{
    char_u	*line, *p, *s;
    ཧྲིལ		len;
    pos_T	fp;
    colnr_T	col;

    line = ml_get_curline();
    p = skipwhite(line);
    len = (ཧྲིལ)(skiptowhite(p) - p);
    གལ་སྲིད (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (ཧྲིལ)(skiptowhite(p) - p);
    }
    གལ་སྲིད (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    དེ་མིན གལ་སྲིད (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    དེ་མིན གལ་སྲིད ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	གལ་སྲིད ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    དོན་ལུ (len = 0; vim_isIDc(p[len]); ++len)
	;
    གལ་སྲིད (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	ལོག 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - line);
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    ལོག (ཧྲིལ)col;
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
    static ཧྲིལ
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*line;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    གལ་སྲིད (lnum > 1)
    {
	line = ml_get(lnum - 1);
	གལ་སྲིད (*line != NUL && line[STRLEN(line) - 1] == '\\')
	    ལོག -1;
    }

    line = s = ml_get(lnum);
    སྐབས (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == NULL)
    {
	གལ་སྲིད (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	དེ་མིན
	    ++s;
    }
    གལ་སྲིད (*s != '=')
	ལོག 0;

    s = skipwhite(s + 1);
    གལ་སྲིད (cin_nocode(s))
	ལོག 0;

    གལ་སྲིད (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - line);
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    ལོག (ཧྲིལ)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    static ཧྲིལ
cin_skip2pos(pos_T *trypos)
{
    char_u	*line;
    char_u	*p;
    char_u	*new_p;

    p = line = ml_get(trypos->lnum);
    སྐབས (*p && (colnr_T)(p - line) < trypos->col)
    {
	གལ་སྲིད (cin_iscomment(p))
	    p = cin_skipcomment(p);
	དེ་མིན
	{
	    new_p = skip_string(p);
	    གལ་སྲིད (new_p == p)
		++p;
	    དེ་མིན
		p = new_p;
	}
    }
    ལོག (ཧྲིལ)(p - line);
}

    static pos_T *
find_match_char(ཧྲིལ c, ཧྲིལ ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    static pos_T pos_copy;
    ཧྲིལ		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    གལ་སྲིད ((trypos = findmatchlimit(NULL, c, FM_SKIPCOMM, ind_maxp_wk)) != NULL)
    {
	// check if the ( is in a // comment
	གལ་སྲིད ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (ཧྲིལ)(cursor_save.lnum - trypos->lnum);
	    གལ་སྲིད (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		goto retry;
	    }
	    trypos = NULL;
	}
	དེ་མིན
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    གལ་སྲིད ((trypos_wk = ind_find_start_CORS(NULL)) != NULL) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (ཧྲིལ)(cursor_save.lnum
			- trypos_wk->lnum);
		གལ་སྲིད (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    goto retry;
		}
		trypos = NULL;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    ལོག trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    static pos_T *
find_match_paren(ཧྲིལ ind_maxparen)	// XXX
{
    ལོག find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    static ཧྲིལ
find_last_paren(char_u *l, ཧྲིལ start, ཧྲིལ end)
{
    ཧྲིལ		i;
    ཧྲིལ		retval = FALSE;
    ཧྲིལ		open_count = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    དོན་ལུ (i = 0; l[i] != NUL; i++)
    {
	i = (ཧྲིལ)(cin_skipcomment(l + i) - l); // ignore parens in comments
	གལ་སྲིད (l[i] == NUL)
	    bha_nga;
	i = (ཧྲིལ)(skip_string(l + i) - l);    // ignore parens in quotes
	གལ་སྲིད (l[i] == start)
	    ++open_count;
	དེ་མིན གལ་སྲིད (l[i] == end)
	{
	    གལ་སྲིད (open_count > 0)
		--open_count;
	    དེ་མིན
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    ལོག retval;
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
    static ཧྲིལ
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    ཧྲིལ		retval = FALSE;
    pos_T	*trypos;
    ཧྲིལ		just_started = TRUE;

    གལ་སྲིད (sp == NULL)
	s = ml_get(lnum);
    དེ་མིན
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    གལ་སྲིད (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
    {
	lnum = trypos->lnum;
	གལ་སྲིད (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    ལོག FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    གལ་སྲིད (cin_ispreproc(s))
	ལོག FALSE;

    སྐབས (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	གལ་སྲིད (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	དེ་མིན གལ་སྲིད (*s == ':')
	{
	    གལ་སྲིད (*(s + 1) == ':')
		s += 2;
	    དེ་མིན
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		ལོག FALSE;
	}
	དེ་མིན
	    ++s;
    }
    གལ་སྲིད (*s != '(')
	ལོག FALSE;		// ';', ' or "  before any () or no '('

    སྐབས (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	གལ་སྲིད (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    གལ་སྲིད (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    goto done;
	}
	གལ་སྲིད ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    ཧྲིལ comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    དོན་ལུ (;;)
	    {
		གལ་སྲིད (lnum >= curbuf->b_ml.ml_line_count)
		    bha_nga;
		s = ml_get(++lnum);
		གལ་སྲིད (!cin_ispreproc(s))
		    bha_nga;
	    }
	    གལ་སྲིད (lnum >= curbuf->b_ml.ml_line_count)
		bha_nga;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    གལ་སྲིད (!just_started && (!comma && *s != ',' && *s != ')'))
		bha_nga;
	    just_started = FALSE;
	}
	དེ་མིན གལ་སྲིད (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	དེ་མིན
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    གལ་སྲིད (lnum != first_lnum && sp != NULL)
	*sp = ml_get(first_lnum);

    ལོག retval;
}

    static ཧྲིལ
cin_isif(char_u *p)
{
    ལོག (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    static ཧྲིལ
cin_isdo(char_u *p)
{
    ལོག (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    static ཧྲིལ
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ཧྲིལ		retval = FALSE;

    p = cin_skipcomment(p);
    གལ་སྲིད (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    གལ་སྲིད (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	སྐབས (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	གལ་སྲིད ((trypos = findmatchlimit(NULL, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != NULL
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    ལོག retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    static ཧྲིལ
cin_is_if_for_while_before_offset(char_u *line, ཧྲིལ *poffset)
{
    ཧྲིལ offset = *poffset;

    གལ་སྲིད (offset-- < 2)
	ལོག 0;
    སྐབས (offset > 2 && VIM_ISWHITE(line[offset]))
	--offset;

    offset -= 1;
    གལ་སྲིད (STRNCMP(line + offset, "if", 2) == 0)
	goto probablyFound;

    གལ་སྲིད (offset >= 1)
    {
	offset -= 1;
	གལ་སྲིད (STRNCMP(line + offset, "for", 3) == 0)
	    goto probablyFound;

	གལ་སྲིད (offset >= 2)
	{
	    offset -= 2;
	    གལ་སྲིད (STRNCMP(line + offset, "while", 5) == 0)
		goto probablyFound;
	}
    }
    ལོག 0;

probablyFound:
    གལ་སྲིད (!offset || !vim_isIDc(line[offset - 1]))
    {
	*poffset = offset;
	ལོག 1;
    }
    ལོག 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    static ཧྲིལ
cin_iswhileofdo_end(ཧྲིལ terminated)
{
    char_u	*line;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    ཧྲིལ		i;

    གལ་སྲིད (terminated != ';')	// there must be a ';' at the end
	ལོག FALSE;

    p = line = ml_get_curline();
    སྐབས (*p != NUL)
    {
	p = cin_skipcomment(p);
	གལ་སྲིད (*p == ')')
	{
	    s = skipwhite(p + 1);
	    གལ་སྲིད (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (ཧྲིལ)(p - line);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		གལ་སྲིད (trypos != NULL)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    གལ་སྲིད (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    གལ་སྲིད (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			ལོག TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		line = ml_get_curline();
		p = line + i;
	    }
	}
	གལ་སྲིད (*p != NUL)
	    ++p;
    }
    ལོག FALSE;
}

    static ཧྲིལ
cin_isbreak(char_u *p)
{
    ལོག (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    static ཧྲིལ
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    ཧྲིལ		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*line = ml_get_curline();

    གལ་སྲིད (pos->lnum <= lnum)
	ལོག cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(line);
    གལ་སྲིད (*s == '#')		// skip #define FOO x ? (x) : x
	ལོག FALSE;
    s = cin_skipcomment(s);
    གལ་སྲིད (*s == NUL)
	ལོག FALSE;

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
    སྐབས (lnum > 1)
    {
	line = ml_get(lnum - 1);
	s = skipwhite(line);
	གལ་སྲིད (*s == '#' || *s == NUL)
	    bha_nga;
	སྐབས (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    གལ་སྲིད (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		bha_nga;
	    གལ་སྲིད (*s != NUL)
		++s;
	}
	གལ་སྲིད (*s != NUL)
	    bha_nga;
	--lnum;
    }

    pos->lnum = lnum;
    line = ml_get(lnum);
    s = line;
    དོན་ལུ (;;)
    {
	གལ་སྲིད (*s == NUL)
	{
	    གལ་སྲིད (lnum == curwin->w_cursor.lnum)
		bha_nga;
	    // Continue in the cursor line.
	    line = ml_get(++lnum);
	    s = line;
	}
	གལ་སྲིད (s == line)
	{
	    // don't recognize "case (foo):" as a baseclass
	    གལ་སྲིད (cin_iscase(s, FALSE))
		bha_nga;
	    s = cin_skipcomment(line);
	    གལ་སྲིད (*s == NUL)
		continue;
	}

	གལ་སྲིད (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	དེ་མིན གལ་སྲིད (s[0] == ':')
	{
	    གལ་སྲིད (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    དེ་མིན གལ་སྲིད (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    དེ་མིན
		s = cin_skipcomment(s + 1);
	}
	དེ་མིན གལ་སྲིད ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    གལ་སྲིད (*s == 'c')
		s = cin_skipcomment(s + 5);
	    དེ་མིན
		s = cin_skipcomment(s + 6);
	}
	དེ་མིན
	{
	    གལ་སྲིད (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    དེ་མིན གལ་སྲིད (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    དེ་མིན གལ་སྲིད (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		ལོག FALSE;
	    }
	    དེ་མིན གལ་སྲིད (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    དེ་མིན གལ་སྲིད (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		གལ་སྲིད (cpp_base_class)
		    pos->col = (colnr_T)(s - line);
	    }

	    // When the line ends in a comma don't align with it.
	    གལ་སྲིད (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    གལ་སྲིད (cpp_base_class)
	pos->lnum = lnum;
    ལོག cpp_base_class;
}

    static ཧྲིལ
get_baseclass_amount(ཧྲིལ col)
{
    ཧྲིལ		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    གལ་སྲིད (col == 0)
    {
	amount = get_indent();
	གལ་སྲིད (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	གལ་སྲིད (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    དེ་མིན
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, NULL, NULL, 0);
	amount = (ཧྲིལ)vcol;
    }
    གལ་སྲིད (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    ལོག amount;
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
    སྐབས ((trypos = findmatchlimit(NULL, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != NULL)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = NULL;
	// ignore the { if it's in a // or / *  * / comment
	གལ་སྲིད ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(NULL)) == NULL) // XXX
	    bha_nga;
	གལ་སྲིད (pos != NULL)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    ལོག trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    static pos_T *
find_match_paren_after_brace (ཧྲིལ ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    གལ་སྲིད (trypos == NULL)
	ལོག NULL;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    གལ་སྲིད (tryposBrace != NULL
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = NULL;
    ལོག trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    static ཧྲིལ
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    གལ་སྲིད (n > 0 && n < curbuf->b_ind_maxparen / 2)
	ལོག curbuf->b_ind_maxparen - (ཧྲིལ)n;
    ལོག curbuf->b_ind_maxparen;
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
    ཧྲིལ		divider;
    ཧྲིལ		fraction = 0;
    ཧྲིལ		sw;
    long	t = get_sw_value(buf);

    // needed for cino-(, it will be multiplied by 2 again
    གལ་སྲིད (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    དེ་མིན
	sw = (ཧྲིལ)t;

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

    དོན་ལུ (p = buf->b_p_cino; *p; )
    {
	l = p++;
	གལ་སྲིད (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	གལ་སྲིད (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((var_na *)++p);
	    སྐབས (VIM_ISDIGIT(*p))
	    {
		++p;
		གལ་སྲིད (divider)
		    divider *= 10;
		དེ་མིན
		    divider = 10;
	    }
	}
	གལ་སྲིད (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    གལ་སྲིད (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    དེ་མིན
	    {
		n *= sw;
		གལ་སྲིད (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	གལ་སྲིད (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	switch (*l)
	{
	    pak_sa '>': buf->b_ind_level = (ཧྲིལ)n; bha_nga;
	    pak_sa 'e': buf->b_ind_open_imag = (ཧྲིལ)n; bha_nga;
	    pak_sa 'n': buf->b_ind_no_brace = (ཧྲིལ)n; bha_nga;
	    pak_sa 'f': buf->b_ind_first_open = (ཧྲིལ)n; bha_nga;
	    pak_sa '{': buf->b_ind_open_extra = (ཧྲིལ)n; bha_nga;
	    pak_sa '}': buf->b_ind_close_extra = (ཧྲིལ)n; bha_nga;
	    pak_sa '^': buf->b_ind_open_left_imag = (ཧྲིལ)n; bha_nga;
	    pak_sa 'L': buf->b_ind_jump_label = (ཧྲིལ)n; bha_nga;
	    pak_sa ':': buf->b_ind_case = (ཧྲིལ)n; bha_nga;
	    pak_sa '=': buf->b_ind_case_code = (ཧྲིལ)n; bha_nga;
	    pak_sa 'b': buf->b_ind_case_break = (ཧྲིལ)n; bha_nga;
	    pak_sa 'p': buf->b_ind_param = (ཧྲིལ)n; bha_nga;
	    pak_sa 't': buf->b_ind_func_type = (ཧྲིལ)n; bha_nga;
	    pak_sa '/': buf->b_ind_comment = (ཧྲིལ)n; bha_nga;
	    pak_sa 'c': buf->b_ind_in_comment = (ཧྲིལ)n; bha_nga;
	    pak_sa 'C': buf->b_ind_in_comment2 = (ཧྲིལ)n; bha_nga;
	    pak_sa 'i': buf->b_ind_cpp_baseclass = (ཧྲིལ)n; bha_nga;
	    pak_sa '+': buf->b_ind_continuation = (ཧྲིལ)n; bha_nga;
	    pak_sa '(': buf->b_ind_unclosed = (ཧྲིལ)n; bha_nga;
	    pak_sa 'u': buf->b_ind_unclosed2 = (ཧྲིལ)n; bha_nga;
	    pak_sa 'U': buf->b_ind_unclosed_noignore = (ཧྲིལ)n; bha_nga;
	    pak_sa 'W': buf->b_ind_unclosed_wrapped = (ཧྲིལ)n; bha_nga;
	    pak_sa 'w': buf->b_ind_unclosed_whiteok = (ཧྲིལ)n; bha_nga;
	    pak_sa 'm': buf->b_ind_matching_paren = (ཧྲིལ)n; bha_nga;
	    pak_sa 'M': buf->b_ind_paren_prev = (ཧྲིལ)n; bha_nga;
	    pak_sa ')': buf->b_ind_maxparen = (ཧྲིལ)n; bha_nga;
	    pak_sa '*': buf->b_ind_maxcomment = (ཧྲིལ)n; bha_nga;
	    pak_sa 'g': buf->b_ind_scopedecl = (ཧྲིལ)n; bha_nga;
	    pak_sa 'h': buf->b_ind_scopedecl_code = (ཧྲིལ)n; bha_nga;
	    pak_sa 'j': buf->b_ind_java = (ཧྲིལ)n; bha_nga;
	    pak_sa 'J': buf->b_ind_js = (ཧྲིལ)n; bha_nga;
	    pak_sa 'l': buf->b_ind_keep_case_label = (ཧྲིལ)n; bha_nga;
	    pak_sa '#': buf->b_ind_hash_comment = (ཧྲིལ)n; bha_nga;
	    pak_sa 'N': buf->b_ind_cpp_namespace = (ཧྲིལ)n; bha_nga;
	    pak_sa 'k': buf->b_ind_if_for_while = (ཧྲིལ)n; bha_nga;
	    pak_sa 'E': buf->b_ind_cpp_extern_c = (ཧྲིལ)n; bha_nga;
	    pak_sa 'P': buf->b_ind_pragma = (ཧྲིལ)n; bha_nga;
	}
	གལ་སྲིད (*p == ',')
	    ++p;
    }
}

    static ཧྲིལ
find_match(ཧྲིལ lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    ཧྲིལ		elselevel;
    ཧྲིལ		whilelevel;

    གལ་སྲིད (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    དེ་མིན
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    སྐབས (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	གལ་སྲིད (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    གལ་སྲིད (theirscope == NULL)
		bha_nga;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    གལ་སྲིད (theirscope->lnum < ourscope)
		bha_nga;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    གལ་སྲིད (theirscope->lnum > ourscope)
		continue;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    གལ་སྲིད (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		གལ་སྲིད (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    གལ་སྲིད (!cin_isif(mightbeif))
			++elselevel;
		    continue;
		}

		// If it's an "if" decrement elselevel
		གལ་སྲིད (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    གལ་སྲིད (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    གལ་སྲིད (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		continue;
	    }

	    // If it's a "do" decrement whilelevel
	    གལ་སྲིད (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    གལ་སྲིད (elselevel <= 0 && whilelevel <= 0)
		ལོག OK;
	}
    }
    ལོག FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    ཧྲིལ
get_c_indent(void)
{
    pos_T	cur_curpos;
    ཧྲིལ		amount;
    ཧྲིལ		scope_amount;
    ཧྲིལ		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = NULL;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    ཧྲིལ		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    ཧྲིལ		lookfor;
    ཧྲིལ		whilelevel;
    linenr_T	lnum;
    ཧྲིལ		n;
    ཧྲིལ		iscase;
    ཧྲིལ		lookfor_break;
    ཧྲིལ		lookfor_cpp_namespace = FALSE;
    ཧྲིལ		cont_amount = 0;    // amount for continuation line
    ཧྲིལ		original_line_islabel;
    ཧྲིལ		added_to_amount = 0;
    ཧྲིལ		js_cur_has_key = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    ཧྲིལ		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    གལ་སྲིད (cur_curpos.lnum == 1)
	ལོག 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    གལ་སྲིད (linecopy == NULL)
	ལོག 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    གལ་སྲིད ((State & MODE_INSERT)
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
    གལ་སྲིད (comment_pos != NULL)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    གལ་སྲིད (trypos != NULL && (comment_pos == NULL
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	goto laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    གལ་སྲིད (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	གལ་སྲིད (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    goto theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    གལ་སྲིད (original_line_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	goto theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    གལ་སྲིད (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_line_comment(); // XXX
	གལ་སྲིད (trypos == NULL && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    གལ་སྲིད (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	གལ་སྲིད (trypos  != NULL)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, NULL, NULL, 0);
	    amount = col;
	    goto theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    གལ་སྲིད (!cin_iscomment(theline) && comment_pos != NULL) // XXX
    {
	ཧྲིལ	lead_start_len = 2;
	ཧྲིལ	lead_middle_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_middle[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	ཧྲིལ	lead_end_len;
	char_u	*p;
	ཧྲིལ	start_align = 0;
	ཧྲིལ	start_off = 0;
	ཧྲིལ	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, NULL, NULL, 0);
	amount = col;
	*lead_start = NUL;
	*lead_middle = NUL;

	p = curbuf->b_p_com;
	སྐབས (*p != NUL)
	{
	    ཧྲིལ	align = 0;
	    ཧྲིལ	off = 0;
	    ཧྲིལ what = 0;

	    སྐབས (*p != NUL && *p != ':')
	    {
		གལ་སྲིད (*p == COM_START || *p == COM_END || *p == COM_MIDDLE)
		    what = *p++;
		དེ་མིན གལ་སྲིད (*p == COM_LEFT || *p == COM_RIGHT)
		    align = *p++;
		དེ་མིན གལ་སྲིད (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		དེ་མིན
		    ++p;
	    }

	    གལ་སྲིད (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    གལ་སྲིད (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    དེ་མིན གལ་སྲིད (what == COM_MIDDLE)
	    {
		STRCPY(lead_middle, lead_end);
		lead_middle_len = lead_end_len;
	    }
	    དེ་མིན གལ་སྲིད (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		གལ་སྲིད (STRNCMP(theline, lead_middle, lead_middle_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    གལ་སྲིད (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			གལ་སྲིད (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			དེ་མིན གལ་སྲིད (STRNCMP(look, lead_middle,
							lead_middle_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    bha_nga;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			དེ་མིན གལ་སྲིད (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    continue;
		    }
		    གལ་སྲིད (start_off != 0)
			amount += start_off;
		    དེ་མིན གལ་སྲིད (start_align == COM_RIGHT)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_middle);
		    bha_nga;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		གལ་སྲིད (STRNCMP(theline, lead_middle, lead_middle_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    གལ་སྲིད (off != 0)
			amount += off;
		    དེ་མིན གལ་སྲིད (align == COM_RIGHT)
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
	གལ་སྲིད (done)
	    ;
	དེ་མིན གལ་སྲིད (theline[0] == '*')
	    amount += 1;
	དེ་མིན
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    དོན་ལུ (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		གལ་སྲིད (linewhite(lnum))		    // skip blank lines
		    continue;
		amount = get_indent_lnum(lnum);	    // XXX
		bha_nga;
	    }
	    གལ་སྲིད (amount == -1)			    // use the comment opener
	    {
		གལ་སྲིད (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    གལ་སྲིད (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, NULL, NULL, 0);
		amount = col;
		གལ་སྲིད (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	goto theend;
    }

    // Are we looking at a ']' that has a match?
    གལ་སྲིད (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != NULL)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	goto theend;
    }

    // Are we inside parentheses or braces?  XXX
    གལ་སྲིད (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != NULL
	    || trypos != NULL)
    {
      གལ་སྲིད (trypos != NULL && tryposBrace != NULL)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  གལ་སྲིད (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = NULL;
	  དེ་མིན
	      tryposBrace = NULL;
      }

      གལ་སྲིད (trypos != NULL)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	གལ་སྲིད (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	དེ་མིན
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    དོན་ལུ (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		གལ་སྲིད (cin_nocode(l))		// skip comment lines
		    continue;
		གལ་སྲིད (cin_ispreproc_cont(&l, &lnum, &amount))
		    continue;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		གལ་སྲིད ((trypos = ind_find_start_CORS(NULL)) != NULL)
		{
		    lnum = trypos->lnum + 1;
		    continue;
		}

		// XXX
		གལ་སྲིད ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != NULL
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    གལ་སྲིད (theline[0] == ')')
		    {
			གལ་སྲིད (our_paren_pos.lnum != lnum
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
	གལ་སྲིད (amount == -1)
	{
	    ཧྲིལ	    ignore_paren_col = 0;
	    ཧྲིལ	    is_if_for_while = 0;

	    གལ་སྲིད (curbuf->b_ind_if_for_while)
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
		} སྐབས (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		line = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(line, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    གལ་སྲིད (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *line;
		ཧྲིལ	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		line = ml_get_curline();
		look_col = (ཧྲིལ)(look - line);
		curwin->w_cursor.col = look_col + 1;
		གལ་སྲིད ((trypos = findmatchlimit(NULL, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != NULL
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    གལ་སྲིད (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		གལ་སྲིད (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    གལ་སྲིད (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			དོན་ལུ (col = 0; col < our_paren_pos.col; ++col)
			{
			    switch (l[col])
			    {
				pak_sa '(':
				pak_sa '{': ++n;
					  bha_nga;

				pak_sa ')':
				pak_sa '}': གལ་སྲིད (n > 1)
					      --n;
					  bha_nga;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    དེ་མིན གལ་སྲིད (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    དེ་མིན
		    {
			col = our_paren_pos.col + 1;
			སྐབས (VIM_ISWHITE(l[col]))
			    col++;
			གལ་སྲིད (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			དེ་མིན
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		གལ་སྲིད (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, NULL, NULL, 0);
		    གལ་སྲིད (cur_amount > (ཧྲིལ)col)
			cur_amount = col;
		}
	    }

	    གལ་སྲིད (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    དེ་མིན གལ་སྲིད ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		གལ་སྲིད (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    དེ་མིན
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		སྐབས ((ཧྲིལ)our_paren_pos.col > ignore_paren_col)
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
		གལ་སྲིད (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		དེ་མིན
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    གལ་སྲིད (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != NULL)
			amount += curbuf->b_ind_unclosed2;
		    དེ་མིན
		    {
			གལ་སྲིད (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			དེ་མིན
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		གལ་སྲིད (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	གལ་སྲིད (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      དེ་མིན
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
	གལ་སྲིད (*look == '{')
	{
	    getvcol(curwin, trypos, &col, NULL, NULL, 0);
	    amount = col;
	    གལ་སྲིད (*start == '{')
		start_brace = BRACE_IN_COL0;
	    དེ་མིན
		start_brace = BRACE_AT_START;
	}
	དེ་མིན
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    གལ་སྲིད (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != NULL)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    གལ་སྲིད ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    དེ་མིན གལ་སྲིད (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    དེ་མིན
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	གལ་སྲིད (curbuf->b_ind_js)
	    js_cur_has_key = cin_has_js_key(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	གལ་སྲིད (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	དེ་མིན
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    གལ་སྲིད (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    དེ་མིན གལ་སྲིད (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    གལ་སྲིད (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		གལ་སྲིད (find_match(lookfor, ourscope) == OK)
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

	    གལ་སྲིད (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_left_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    དེ་མིན གལ་སྲིད (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    དེ་མིན
	    {
		གལ་སྲིད (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    གལ་སྲིད (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    དེ་མིན གལ་སྲིད (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		དེ་མིན
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    གལ་སྲིད (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    གལ་སྲིད (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    དེ་མིན གལ་སྲིད (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    དེ་མིན
	    {
		གལ་སྲིད (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    དོན་ལུ (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		གལ་སྲིད (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    གལ་སྲིད (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			གལ་སྲིད (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    གལ་སྲིད (cont_amount > 0)
				amount = cont_amount;
			    དེ་མིན གལ་སྲིད (!curbuf->b_ind_js)
				amount += ind_continuation;
			    bha_nga;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(NULL);
			གལ་སྲིད (trypos != NULL)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    continue;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			གལ་སྲིད (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    continue;

			གལ་སྲིད (cin_nocode(l))
			    continue;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			གལ་སྲིད (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    གལ་སྲིད (terminated == ',')
				bha_nga;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    གལ་སྲིད (terminated != ';' && cin_isinit())
				bha_nga;

			    // nothing useful found
			    གལ་སྲིད (terminated == 0 || terminated == '{')
				continue;
			}

			གལ་སྲིད (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = NULL;
			    གལ་སྲིད (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    གལ་སྲིད (trypos == NULL && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    གལ་སྲིད (trypos != NULL)
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
			གལ་སྲིད (cont_amount > 0)
			    amount = cont_amount;
			དེ་མིན
			    amount += ind_continuation;
		    }
		    དེ་མིན གལ་སྲིད (lookfor == LOOKFOR_UNTERM)
		    {
			གལ་སྲིད (cont_amount > 0)
			    amount = cont_amount;
			དེ་མིན
			    amount += ind_continuation;
		    }
		    དེ་མིན
		    {
			གལ་སྲིད (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    གལ་སྲིད (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			གལ་སྲིད (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    གལ་སྲིད (curwin->w_cursor.lnum == ourscope)
				continue;

			    གལ་སྲིད (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				bha_nga;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(NULL);
			    གལ་སྲིད (trypos != NULL)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				continue;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    གལ་སྲིད (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				continue;

			    // Finally the actual check for "namespace".
			    གལ་སྲིད (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				bha_nga;
			    }
			    དེ་མིན གལ་སྲིད (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				bha_nga;
			    }

			    གལ་སྲིད (cin_nocode(l))
				continue;
			}
		    }
		    bha_nga;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		གལ་སྲིད ((trypos = ind_find_start_CORS(&raw_string_start)) != NULL)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    continue;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		གལ་སྲིད (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    གལ་སྲིད (lookfor == LOOKFOR_CPP_BASECLASS)
			bha_nga;

		    // When looking for a "do" we are not interested in
		    // labels.
		    གལ་སྲིད (whilelevel > 0)
			continue;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    གལ་སྲིད (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			གལ་སྲིད (cont_amount > 0)
			    amount = cont_amount;
			དེ་མིན
			    amount += ind_continuation;
			bha_nga;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    གལ་སྲིད (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			གལ་སྲིད ((trypos = find_start_brace()) == NULL
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
		    གལ་སྲིད (lookfor == LOOKFOR_TERM)
		    {
			གལ་སྲིད (n)
			    amount = n;

			གལ་སྲིད (!lookfor_break)
			    bha_nga;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    གལ་སྲིད (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			གལ་སྲིད (l != NULL && cin_is_cinword(l))
			{
			    གལ་སྲིད (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    དེ་མིན
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
		གལ་སྲིད (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    གལ་སྲིད (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != NULL)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    continue;
		}

		// Ignore jump labels with nothing after them.
		གལ་སྲིད (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    གལ་སྲིད (l == NULL || cin_nocode(l))
			continue;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		གལ་སྲིད (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    continue;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		གལ་སྲིད (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		གལ་སྲིད (n)
		{
		    གལ་སྲིད (lookfor == LOOKFOR_UNTERM)
		    {
			གལ་སྲིད (cont_amount > 0)
			    amount = cont_amount;
			དེ་མིན
			    amount += ind_continuation;
		    }
		    དེ་མིན གལ་སྲིད (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			continue;
		    }
		    དེ་མིན
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    bha_nga;
		}
		དེ་མིན གལ་སྲིད (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    གལ་སྲིད (cin_isterminated(l, TRUE, FALSE))
			bha_nga;
		    དེ་མིན
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

		གལ་སྲིད (js_cur_has_key)
		{
		    js_cur_has_key = 0; // only check the first line
		    གལ་སྲིད (curbuf->b_ind_js && terminated == ',')
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
		གལ་སྲིད (lookfor == LOOKFOR_JS_KEY && cin_has_js_key(l))
		{
		    amount = get_indent();
		    bha_nga;
		}
		གལ་སྲིད (lookfor == LOOKFOR_COMMA)
		{
		    གལ་སྲིད (tryposBrace != NULL && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			bha_nga;
		    གལ་སྲིད (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			bha_nga;
		    དེ་མིན
		    {
			amount = get_indent();
			གལ་སྲིད (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    bha_nga;
		    }
		}

		གལ་སྲིད (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    གལ་སྲིད (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    གལ་སྲིད (trypos != NULL && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = NULL;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    གལ་སྲིད (trypos == NULL && terminated == ',')
		    {
			གལ་སྲིད (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    གལ་སྲིད (trypos != NULL)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			གལ་སྲིད (cin_iscase(l, FALSE) || cin_isscopedecl(l))
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
		    གལ་སྲིད (terminated == ',')
		    {
			སྐབས (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    གལ་སྲིད (*l == NUL || l[STRLEN(l) - 1] != '\\')
				bha_nga;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    གལ་སྲིད (curbuf->b_ind_js)
			cur_amount = get_indent();
		    དེ་མིན
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    གལ་སྲིད (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			གལ་སྲིད (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			གལ་སྲིད (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
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
		    གལ་སྲིད (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			གལ་སྲིད (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    གལ་སྲིད (cont_amount > 0)
				amount = cont_amount;
			    དེ་མིན
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
			གལ་སྲིད (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			གལ་སྲིད (lookfor != LOOKFOR_TERM)
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
			གལ་སྲིད (cin_isdo(l))
			{
			    གལ་སྲིད (whilelevel == 0)
				bha_nga;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			གལ་སྲིད (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    གལ་སྲིད (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    གལ་སྲིད ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				bha_nga;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    དེ་མིན
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			གལ་སྲིད (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    གལ་སྲིད (terminated == ',')
				amount += ind_continuation;
			    bha_nga;
			}

			གལ་སྲིད (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    གལ་སྲིད (terminated == ',')
			    {
				གལ་སྲིད (curbuf->b_ind_cpp_baseclass == 0)
				    bha_nga;

				lookfor = LOOKFOR_CPP_BASECLASS;
				continue;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    གལ་སྲིད (amount > cur_amount)
				amount = cur_amount;
			}
			དེ་མིན
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (ཧྲིལ)STRLEN(l);
			    གལ་སྲིད (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
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
			    གལ་སྲིད (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				གལ་སྲིད (curbuf->b_ind_js)
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
				    གལ་སྲིད (cin_iscomment(skipwhite(l)))
					bha_nga;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    གལ་སྲིད (trypos != NULL)
				    {
					གལ་སྲིད (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    bha_nga;
					}
					ourscope = trypos->lnum;
				    }
				}
				དེ་མིན
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    དེ་མིན
			    {
				གལ་སྲིད (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				གལ་སྲིད (lookfor != LOOKFOR_TERM
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
		དེ་མིན གལ་སྲིད (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    གལ་སྲིད (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			གལ་སྲིད (cont_amount > 0)
			    amount = cont_amount;
			དེ་མིན
			    amount += ind_continuation;
			bha_nga;
		    }

		    གལ་སྲིད (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			གལ་སྲིད (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		དེ་མིན
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    གལ་སྲིད (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			continue;
		    }

		    // Handle "do {" line.
		    གལ་སྲིད (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			གལ་སྲིད (cin_isdo(l))
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
		    གལ་སྲིད (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			གལ་སྲིད (cont_amount > 0)
			    amount = cont_amount;
			དེ་མིན
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
		    གལ་སྲིད (lookfor == LOOKFOR_TERM)
		    {
			གལ་སྲིད (!lookfor_break && whilelevel == 0)
			    bha_nga;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    དེ་མིན
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			གལ་སྲིད (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != NULL)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    གལ་སྲིད (cin_iscase(l, FALSE) || cin_isscopedecl(l))
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

			གལ་སྲིད (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			གལ་སྲིད (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			གལ་སྲིད (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    གལ་སྲིད ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				bha_nga;
			    continue;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			གལ་སྲིད (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != NULL)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    གལ་སྲིད (*l == '}' || !cin_iselse(l))
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
      གལ་སྲིད (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      གལ་སྲིད (curbuf->b_ind_jump_label > 0 && original_line_islabel)
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

    གལ་སྲིད (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	goto theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    གལ་སྲིད (cur_curpos.lnum < curbuf->b_ml.ml_line_count
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
    སྐབས (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	གལ་སྲིད ((trypos = ind_find_start_CORS(NULL)) != NULL)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    continue;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	གལ་སྲིད (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	གལ་སྲིད (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    bha_nga;
	}

	// Skip preprocessor directives and blank lines.
	གལ་སྲིད (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    continue;

	གལ་སྲིད (cin_nocode(l))
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
	གལ་སྲིད (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    གལ་སྲིད (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != NULL)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    སྐབས (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		གལ་སྲིད (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    bha_nga;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    གལ་སྲིད (amount == 0)
		amount = cin_first_id_amount();
	    གལ་སྲིད (amount == 0)
		amount = ind_continuation;
	    bha_nga;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	གལ་སྲིད (cin_isfuncdecl(NULL, cur_curpos.lnum, 0))  // XXX
	    bha_nga;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	གལ་སྲིད (*skipwhite(l) == '}')
	    bha_nga;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	གལ་སྲིད (cin_ends_in(l, (char_u *)"};"))
	    bha_nga;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	གལ་སྲིད (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    bha_nga;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	གལ་སྲིད (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    སྐབས (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		གལ་སྲིད (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    bha_nga;
	    }
	    གལ་སྲིད (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		bha_nga;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	གལ་སྲིད (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    bha_nga;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	གལ་སྲིད (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    གལ་སྲིད (cin_ends_in(l, (char_u *)",")
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

	གལ་སྲིད ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	bha_nga;
    }

    // add extra indent for a comment
    གལ་སྲིད (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    གལ་སྲིད (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	གལ་སྲིད (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    གལ་སྲིད (cur_amount > 0)
		amount = cur_amount;
	    དེ་མིན གལ་སྲིད (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    གལ་སྲིད (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    ལོག amount;
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
    ཧྲིལ
in_cinkeys(
    ཧྲིལ		keytyped,
    ཧྲིལ		when,
    ཧྲིལ		line_is_empty)
{
    char_u	*look;
    ཧྲིལ		try_match;
    ཧྲིལ		try_match_word;
    char_u	*p;
    char_u	*line;
    ཧྲིལ		icase;
    ཧྲིལ		i;

    གལ་སྲིད (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	ལོག FALSE;

#ifdef FEAT_EVAL
    གལ་སྲིད (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    དེ་མིན
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    སྐབས (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	switch (when)
	{
	    pak_sa '*': try_match = (*look == '*'); bha_nga;
	    pak_sa '!': try_match = (*look == '!'); bha_nga;
	    default: try_match = (*look != '*'); bha_nga;
	}
	གལ་སྲིད (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	གལ་སྲིད (*look == '0')
	{
	    try_match_word = try_match;
	    གལ་སྲིད (!line_is_empty)
		try_match = FALSE;
	    ++look;
	}
	དེ་མིན
	    try_match_word = FALSE;

	// does it look like a control character?
	གལ་སྲིད (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    གལ་སྲིད (try_match && keytyped == Ctrl_chr(look[1]))
		ལོག TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	དེ་མིན གལ་སྲིད (*look == 'o')
	{
	    གལ་སྲིད (try_match && keytyped == KEY_OPEN_FORW)
		ལོག TRUE;
	    ++look;
	}
	དེ་མིན གལ་སྲིད (*look == 'O')
	{
	    གལ་སྲིད (try_match && keytyped == KEY_OPEN_BACK)
		ལོག TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	དེ་མིན གལ་སྲིད (*look == 'e')
	{
	    གལ་སྲིད (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		གལ་སྲིད (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    ལོག TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	དེ་མིན གལ་སྲིད (*look == ':')
	{
	    གལ་སྲིད (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		གལ་སྲིད (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    ལོག TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		གལ་སྲིད (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    གལ་སྲིད (i)
			ལོག TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	དེ་མིན གལ་སྲིད (*look == '<')
	{
	    གལ་སྲིད (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		གལ་སྲིད (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != NULL
						       && keytyped == look[1])
		    ལོག TRUE;

		གལ་སྲིད (keytyped == get_special_key_code(look + 1))
		    ལོག TRUE;
	    }
	    སྐབས (*look && *look != '>')
		look++;
	    སྐབས (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	དེ་མིན གལ་སྲིད (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    གལ་སྲིད (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    དེ་མིན
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    གལ་སྲིད (p == NULL)
		p = look + STRLEN(look);
	    གལ་སྲིད ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		ཧྲིལ		match = FALSE;

		གལ་སྲིད (keytyped == KEY_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    line = ml_get_curline();
		    གལ་སྲིད (has_mbyte)
		    {
			char_u	*n;

			དོན་ལུ (s = line + curwin->w_cursor.col; s > line; s = n)
			{
			    n = mb_prevptr(line, s);
			    གལ་སྲིད (!vim_iswordp(n))
				bha_nga;
			}
		    }
		    དེ་མིན
			དོན་ལུ (s = line + curwin->w_cursor.col; s > line; --s)
			    གལ་སྲིད (!vim_iswordc(s[-1]))
				bha_nga;
		    གལ་སྲིད (s + (p - look) <= line + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		དེ་མིན
		    // TODO: multi-byte
		    གལ་སྲིད (keytyped == (ཧྲིལ)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((ཧྲིལ)p[-1])))
		{
		    line = ml_get_cursor();
		    གལ་སྲིད ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(line[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(line - (p - look), look, p - look)
				: STRNCMP(line - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		གལ་སྲིད (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    གལ་སྲིད (getwhitecols_curline() !=
				     (ཧྲིལ)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		གལ་སྲིད (match)
		    ལོག TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	དེ་མིན
	{
	    གལ་སྲིད (try_match && *look == keytyped)
		ལོག TRUE;
	    གལ་སྲིད (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    ལོག FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    void
do_c_expr_indent(void)
{
#ifdef FEAT_EVAL
    གལ་སྲིད (*curbuf->b_p_inde != NUL)
	fixthisline(get_expr_indent);
    དེ་མིན
#endif
	fixthisline(get_c_indent);
}

#གལ་སྲིད defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    void
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    གལ་སྲིད (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ལོག;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    གལ་སྲིད (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_number = get_c_indent();
	curwin->w_cursor = pos;
    }
    དེ་མིན
	rettv->vval.v_number = -1;
}
#endif
