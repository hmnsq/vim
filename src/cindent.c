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
    정수
cin_is_cinword(char_u *line)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    정수		cinw_len;
    정수		retval = FALSE;
    정수		len;

    cinw_len = (정수)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    만약 (cinw_buf == NULL)
	반환 FALSE;

    line = skipwhite(line);
    위해 (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	만약 (STRNCMP(line, cinw_buf, len) == 0
		&& (!vim_iswordc(line[len]) || !vim_iswordc(line[len - 1])))
	{
	    retval = TRUE;
	    break;
	}
    }
    vim_free(cinw_buf);
    반환 retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    static char_u *
skip_string(char_u *p)
{
    정수	    i;

    // We loop, because strings may be concatenated: "date""time".
    위해 ( ; ; ++p)
    {
	만약 (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    만약 (p[1] == NUL)		    // ' at end of line
		break;
	    i = 2;
	    만약 (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		동안 (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    만약 (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		continue;
	    }
	}
	아니면 만약 (p[0] == '"')		    // start of string
	{
	    위해 (++p; p[0]; ++p)
	    {
		만약 (p[0] == '\\' && p[1] != NUL)
		    ++p;
		아니면 만약 (p[0] == '"')	    // end of string
		    break;
	    }
	    만약 (p[0] == '"')
		continue; // continue for another string
	}
	아니면 만약 (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    만약 (paren != NULL)
	    {
		size_t delim_len = paren - delim;

		위해 (p += 3; *p; ++p)
		    만약 (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			break;
		    }
		만약 (p[0] == '"')
		    continue; // continue for another string
	    }
	}
	break;				    // no string found
    }
    만약 (!*p)
	--p;				    // backup from NUL
    반환 p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    정수
is_pos_in_string(char_u *line, colnr_T col)
{
    char_u *p;

    위해 (p = line; *p && (colnr_T)(p - line) < col; ++p)
	p = skip_string(p);
    반환 !((colnr_T)(p - line) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    정수
check_linecomment(char_u *line)
{
    char_u  *p;

    p = line;
    // skip Lispish one-line comments
    만약 (curbuf->b_p_lisp)
    {
	만약 (vim_strchr(p, ';') != NULL) // there may be comments
	{
	    정수 in_str = FALSE;	// inside of string

	    p = line;		// scan from start
	    동안 ((p = vim_strpbrk(p, (char_u *)"\";")) != NULL)
	    {
		만약 (*p == '"')
		{
		    만약 (in_str)
		    {
			만약 (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    아니면 만약 (p == line || ((p - line) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		아니면 만약 (!in_str && ((p - line) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(line, (colnr_T)(p - line)))
		    break;	// found!
		++p;
	    }
	}
	아니면
	    p = NULL;
    }
    아니면
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	위해 ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    만약 (*p == NUL)
		break;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    만약 (p[0] == '/' && p[1] == '/'
				 && (p == line || p[-1] != '*' || p[2] != '*'))
		break;
	}
	만약 (*p == NUL)
	    p = NULL;
    }

    만약 (p == NULL)
	반환 MAXCOL;
    반환 (정수)(p - line);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    static pos_T *
ind_find_start_comment(void)	    // XXX
{
    반환 find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(정수 ind_maxcomment)	// XXX
{
    pos_T	*pos;
    정수		cur_maxcomment = ind_maxcomment;

    위해 (;;)
    {
	pos = findmatchlimit(NULL, '*', FM_BACKWARD, cur_maxcomment);
	만약 (pos == NULL)
	    break;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	만약 (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    break;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	만약 (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    break;
	}
    }
    반환 pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    static pos_T *
find_start_rawstring(정수 ind_maxcomment)	// XXX
{
    pos_T	*pos;
    정수		cur_maxcomment = ind_maxcomment;

    위해 (;;)
    {
	pos = findmatchlimit(NULL, 'R', FM_BACKWARD, cur_maxcomment);
	만약 (pos == NULL)
	    break;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	만약 (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    break;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	만약 (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    break;
	}
    }
    반환 pos;
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
    만약 (comment_pos != NULL)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    만약 (comment_pos == NULL || (rs_pos != NULL
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	만약 (is_raw != NULL && rs_pos != NULL)
	    *is_raw = rs_pos->lnum;
	반환 rs_pos;
    }
    반환 comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    정수
cindent_on(void)
{
    반환 (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
typedef struct {
    정수	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    static char_u *
cin_skipcomment(char_u *s)
{
    동안 (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	만약 (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    break;
	}
	만약 (*s != '/')
	    break;
	++s;
	만약 (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    break;
	}
	만약 (*s != '*')
	    break;
	위해 (++s; *s; ++s)	// skip slash-star comment
	    만약 (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		break;
	    }
    }
    반환 s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    static 정수
cin_nocode(char_u *s)
{
    반환 *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    static 정수
cin_iscomment(char_u *p)
{
    반환 (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    static 정수
cin_islinecomment(char_u *p)
{
    반환 (p[0] == '/' && p[1] == '/');
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
    동안 (--pos.lnum > 0)
    {
	line = ml_get(pos.lnum);
	p = skipwhite(line);
	만약 (cin_islinecomment(p))
	{
	    pos.col = (정수)(p - line);
	    반환 &pos;
	}
	만약 (*p != NUL)
	    break;
    }
    반환 NULL;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    static 정수
cin_has_js_key(char_u *text)
{
    char_u *s = skipwhite(text);
    정수	    quote = -1;

    만약 (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    만약 (!vim_isIDc(*s))	    // need at least one ID character
	반환 FALSE;

    동안 (vim_isIDc(*s))
	++s;
    만약 (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    반환 (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    static 정수
cin_islabel_skip(char_u **s)
{
    만약 (!vim_isIDc(**s))	    // need at least one ID character
	반환 FALSE;

    동안 (vim_isIDc(**s))
    {
	만약 (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	아니면
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    반환 (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    static 정수
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    정수	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    만약 (cinsd_buf == NULL)
	반환 FALSE;

    위해 (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (정수)cinsd_len, ",");
	만약 (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    만약 (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		break;
	    }
	}
    }

    vim_free(cinsd_buf);
    반환 found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    static 정수
cin_ispreproc(char_u *s)
{
    만약 (*skipwhite(s) == '#')
	반환 TRUE;
    반환 FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    static 정수
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, 정수 *amount)
{
    char_u	*line = *pp;
    linenr_T	lnum = *lnump;
    정수		retval = FALSE;
    정수		candidate_amount = *amount;

    만약 (*line != NUL && line[STRLEN(line) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    위해 (;;)
    {
	만약 (cin_ispreproc(line))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    break;
	}
	만약 (lnum == 1)
	    break;
	line = ml_get(--lnum);
	만약 (*line == NUL || line[STRLEN(line) - 1] != '\\')
	    break;
    }

    만약 (lnum != *lnump)
	*pp = ml_get(*lnump);
    만약 (retval)
	*amount = candidate_amount;
    반환 retval;
}

    static 정수
cin_iselse(
    char_u  *p)
{
    만약 (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    반환 (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    static 정수
cin_isterminated(
    char_u	*s,
    정수		incl_open,	// include '{' at the end as terminator
    정수		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    정수		is_else = FALSE;

    s = cin_skipcomment(s);

    만약 (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    만약 (!found_start)
	is_else = cin_iselse(s);

    동안 (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	만약 (*s == '}' && n_open > 0)
	    --n_open;
	만약 ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    반환 *s;
	아니면 만약 (*s == '{')
	{
	    만약 (incl_open && cin_nocode(s + 1))
		반환 *s;
	    아니면
		++n_open;
	}

	만약 (*s)
	    s++;
    }
    반환 found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    static 정수
cin_starts_with(char_u *s, char *word)
{
    정수 l = (정수)STRLEN(word);

    반환 (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    static 정수
cin_isdefault(char_u *s)
{
    반환 (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    static 정수
cin_iscase(
    char_u *s,
    정수 strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    만약 (cin_starts_with(s, "case"))
    {
	위해 (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    만약 (*s == NUL)
		break;
	    만약 (*s == ':')
	    {
		만약 (s[1] == ':')	// skip over "::" for C++
		    ++s;
		아니면
		    반환 TRUE;
	    }
	    만약 (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    아니면 만약 (*s == '/' && (s[1] == '*' || s[1] == '/'))
		반환 FALSE;		// stop at comment
	    아니면 만약 (*s == '"')
	    {
		// JS etc.
		만약 (strict)
		    반환 FALSE;		// stop at string
		아니면
		    반환 TRUE;
	    }
	}
	반환 FALSE;
    }

    만약 (cin_isdefault(s))
	반환 TRUE;
    반환 FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    static 정수
cin_islabel(void)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    만약 (cin_isdefault(s))
	반환 FALSE;
    만약 (cin_isscopedecl(s))
	반환 FALSE;

    만약 (!cin_islabel_skip(&s))
	반환 FALSE;

    만약 (ind_find_start_CORS(NULL))
	반환 FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*line;

    cursor_save = curwin->w_cursor;
    동안 (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	만약 ((trypos = ind_find_start_CORS(NULL)) != NULL) // XXX
	    curwin->w_cursor = *trypos;

	line = ml_get_curline();
	만약 (cin_ispreproc(line))	// ignore #defines, #if, etc.
	    continue;
	만약 (*(line = cin_skipcomment(line)) == NUL)
	    continue;

	curwin->w_cursor = cursor_save;
	만약 (cin_isterminated(line, TRUE, FALSE)
		|| cin_isscopedecl(line)
		|| cin_iscase(line, TRUE)
		|| (cin_islabel_skip(&line) && cin_nocode(line)))
	    반환 TRUE;
	반환 FALSE;
    }
    curwin->w_cursor = cursor_save;
    반환 TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    static 정수
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    정수		len = (정수)STRLEN(find);

    동안 (*p != NUL)
    {
	p = cin_skipcomment(p);
	만약 (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    만약 (cin_nocode(r))
		반환 TRUE;
	}
	만약 (*p != NUL)
	    ++p;
    }
    반환 FALSE;
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
	만약 (*p)
	    p = skip_string(p);
    } 동안 (p != r);
    반환 p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    static 정수
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = NULL;

    동안 (*p)
    {
	만약 (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	아니면 만약 (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	아니면
	    p = cin_skip_comment_and_string(p + 1);
    }
    만약 (!r)
	반환 FALSE;
    p = r; // p points now after '=' or "return"

    만약 (cin_nocode(p))
	반환 TRUE;

    만약 (*p == '&')
	p = cin_skipcomment(p + 1);

    만약 (*p == '(') // skip a typecast
    {
	정수 open_count = 1;
	do
	{
	    p = cin_skip_comment_and_string(p + 1);
	    만약 (cin_nocode(p))
		반환 TRUE;
	    open_count += (*p == '(') - (*p == ')');
	} 동안 (open_count);
	p = cin_skipcomment(p + 1);
	만약 (cin_nocode(p))
	    반환 TRUE;
    }

    동안 (*p == '{')
	p = cin_skipcomment(p + 1);
    반환 cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    static 정수
cin_isinit(void)
{
    char_u	*s;
    static char *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    만약 (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    위해 (;;)
    {
	정수 i, l;

	위해 (i = 0; i < (정수)ARRAY_LENGTH(skip); ++i)
	{
	    l = (정수)strlen(skip[i]);
	    만약 (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		break;
	    }
	}
	만약 (l != 0)
	    break;
    }

    만약 (cin_starts_with(s, "enum"))
	반환 TRUE;

    반환 cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    static 정수
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    정수		has_name = FALSE;
    정수		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    동안 ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    만약 (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	동안 (*p != NUL)
	{
	    만약 (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    아니면 만약 (*p == '{')
	    {
		break;
	    }
	    아니면 만약 (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		만약 (has_name)
		    반환 FALSE; // word character after skipping past name
		++p;
	    }
	    아니면 만약 (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		만약 (!has_name_start || has_name)
		    반환 FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    아니면
	    {
		반환 FALSE;
	    }
	}
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    static 정수
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    정수		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    만약 (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	동안 (*p != NUL)
	{
	    만약 (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    아니면 만약 (*p == '{')
	    {
		break;
	    }
	    아니면 만약 (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		만약 (has_string_literal)
		    반환 FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    아니면 만약 (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		만약 (has_string_literal)
		    반환 FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    아니면
	    {
		반환 FALSE;
	    }
	}
	반환 has_string_literal ? TRUE : FALSE;
    }
    반환 FALSE;
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
    위해 ( ; *l; ++l)
    {
	만약 (*l == ':')
	{
	    만약 (l[1] == ':')	    // skip over "::" for C++
		++l;
	    아니면 만약 (!cin_iscase(l + 1, FALSE))
		break;
	}
	아니면 만약 (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    만약 (*l == NUL)
	반환 NULL;
    l = cin_skipcomment(l + 1);
    만약 (*l == NUL)
	반환 NULL;
    반환 l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    static 정수
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    만약 (p == NULL)
	반환 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    반환 (정수)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    static 정수
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    정수		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    만약 (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	만약 (l == NULL)		// just in case
	    l = ml_get_curline();
    }
    아니면
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    반환 amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    static 정수
cin_first_id_amount(void)
{
    char_u	*line, *p, *s;
    정수		len;
    pos_T	fp;
    colnr_T	col;

    line = ml_get_curline();
    p = skipwhite(line);
    len = (정수)(skiptowhite(p) - p);
    만약 (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (정수)(skiptowhite(p) - p);
    }
    만약 (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    아니면 만약 (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    아니면 만약 ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	만약 ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    위해 (len = 0; vim_isIDc(p[len]); ++len)
	;
    만약 (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	반환 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - line);
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    반환 (정수)col;
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
    static 정수
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*line;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    만약 (lnum > 1)
    {
	line = ml_get(lnum - 1);
	만약 (*line != NUL && line[STRLEN(line) - 1] == '\\')
	    반환 -1;
    }

    line = s = ml_get(lnum);
    동안 (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == NULL)
    {
	만약 (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	아니면
	    ++s;
    }
    만약 (*s != '=')
	반환 0;

    s = skipwhite(s + 1);
    만약 (cin_nocode(s))
	반환 0;

    만약 (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - line);
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    반환 (정수)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    static 정수
cin_skip2pos(pos_T *trypos)
{
    char_u	*line;
    char_u	*p;
    char_u	*new_p;

    p = line = ml_get(trypos->lnum);
    동안 (*p && (colnr_T)(p - line) < trypos->col)
    {
	만약 (cin_iscomment(p))
	    p = cin_skipcomment(p);
	아니면
	{
	    new_p = skip_string(p);
	    만약 (new_p == p)
		++p;
	    아니면
		p = new_p;
	}
    }
    반환 (정수)(p - line);
}

    static pos_T *
find_match_char(정수 c, 정수 ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    static pos_T pos_copy;
    정수		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    만약 ((trypos = findmatchlimit(NULL, c, FM_SKIPCOMM, ind_maxp_wk)) != NULL)
    {
	// check if the ( is in a // comment
	만약 ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (정수)(cursor_save.lnum - trypos->lnum);
	    만약 (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		goto retry;
	    }
	    trypos = NULL;
	}
	아니면
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    만약 ((trypos_wk = ind_find_start_CORS(NULL)) != NULL) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (정수)(cursor_save.lnum
			- trypos_wk->lnum);
		만약 (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    goto retry;
		}
		trypos = NULL;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    반환 trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    static pos_T *
find_match_paren(정수 ind_maxparen)	// XXX
{
    반환 find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    static 정수
find_last_paren(char_u *l, 정수 start, 정수 end)
{
    정수		i;
    정수		retval = FALSE;
    정수		open_count = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    위해 (i = 0; l[i] != NUL; i++)
    {
	i = (정수)(cin_skipcomment(l + i) - l); // ignore parens in comments
	만약 (l[i] == NUL)
	    break;
	i = (정수)(skip_string(l + i) - l);    // ignore parens in quotes
	만약 (l[i] == start)
	    ++open_count;
	아니면 만약 (l[i] == end)
	{
	    만약 (open_count > 0)
		--open_count;
	    아니면
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    반환 retval;
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
    static 정수
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    정수		retval = FALSE;
    pos_T	*trypos;
    정수		just_started = TRUE;

    만약 (sp == NULL)
	s = ml_get(lnum);
    아니면
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    만약 (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
    {
	lnum = trypos->lnum;
	만약 (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    반환 FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    만약 (cin_ispreproc(s))
	반환 FALSE;

    동안 (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	만약 (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	아니면 만약 (*s == ':')
	{
	    만약 (*(s + 1) == ':')
		s += 2;
	    아니면
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		반환 FALSE;
	}
	아니면
	    ++s;
    }
    만약 (*s != '(')
	반환 FALSE;		// ';', ' or "  before any () or no '('

    동안 (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	만약 (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    만약 (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    goto done;
	}
	만약 ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    정수 comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    위해 (;;)
	    {
		만약 (lnum >= curbuf->b_ml.ml_line_count)
		    break;
		s = ml_get(++lnum);
		만약 (!cin_ispreproc(s))
		    break;
	    }
	    만약 (lnum >= curbuf->b_ml.ml_line_count)
		break;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    만약 (!just_started && (!comma && *s != ',' && *s != ')'))
		break;
	    just_started = FALSE;
	}
	아니면 만약 (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	아니면
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    만약 (lnum != first_lnum && sp != NULL)
	*sp = ml_get(first_lnum);

    반환 retval;
}

    static 정수
cin_isif(char_u *p)
{
    반환 (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    static 정수
cin_isdo(char_u *p)
{
    반환 (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    static 정수
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    정수		retval = FALSE;

    p = cin_skipcomment(p);
    만약 (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    만약 (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	동안 (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	만약 ((trypos = findmatchlimit(NULL, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != NULL
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    반환 retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    static 정수
cin_is_if_for_while_before_offset(char_u *line, 정수 *poffset)
{
    정수 offset = *poffset;

    만약 (offset-- < 2)
	반환 0;
    동안 (offset > 2 && VIM_ISWHITE(line[offset]))
	--offset;

    offset -= 1;
    만약 (STRNCMP(line + offset, "if", 2) == 0)
	goto probablyFound;

    만약 (offset >= 1)
    {
	offset -= 1;
	만약 (STRNCMP(line + offset, "for", 3) == 0)
	    goto probablyFound;

	만약 (offset >= 2)
	{
	    offset -= 2;
	    만약 (STRNCMP(line + offset, "while", 5) == 0)
		goto probablyFound;
	}
    }
    반환 0;

probablyFound:
    만약 (!offset || !vim_isIDc(line[offset - 1]))
    {
	*poffset = offset;
	반환 1;
    }
    반환 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    static 정수
cin_iswhileofdo_end(정수 terminated)
{
    char_u	*line;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    정수		i;

    만약 (terminated != ';')	// there must be a ';' at the end
	반환 FALSE;

    p = line = ml_get_curline();
    동안 (*p != NUL)
    {
	p = cin_skipcomment(p);
	만약 (*p == ')')
	{
	    s = skipwhite(p + 1);
	    만약 (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (정수)(p - line);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		만약 (trypos != NULL)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    만약 (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    만약 (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			반환 TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		line = ml_get_curline();
		p = line + i;
	    }
	}
	만약 (*p != NUL)
	    ++p;
    }
    반환 FALSE;
}

    static 정수
cin_isbreak(char_u *p)
{
    반환 (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    static 정수
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    정수		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*line = ml_get_curline();

    만약 (pos->lnum <= lnum)
	반환 cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(line);
    만약 (*s == '#')		// skip #define FOO x ? (x) : x
	반환 FALSE;
    s = cin_skipcomment(s);
    만약 (*s == NUL)
	반환 FALSE;

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
    동안 (lnum > 1)
    {
	line = ml_get(lnum - 1);
	s = skipwhite(line);
	만약 (*s == '#' || *s == NUL)
	    break;
	동안 (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    만약 (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		break;
	    만약 (*s != NUL)
		++s;
	}
	만약 (*s != NUL)
	    break;
	--lnum;
    }

    pos->lnum = lnum;
    line = ml_get(lnum);
    s = line;
    위해 (;;)
    {
	만약 (*s == NUL)
	{
	    만약 (lnum == curwin->w_cursor.lnum)
		break;
	    // Continue in the cursor line.
	    line = ml_get(++lnum);
	    s = line;
	}
	만약 (s == line)
	{
	    // don't recognize "case (foo):" as a baseclass
	    만약 (cin_iscase(s, FALSE))
		break;
	    s = cin_skipcomment(line);
	    만약 (*s == NUL)
		continue;
	}

	만약 (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	아니면 만약 (s[0] == ':')
	{
	    만약 (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    아니면 만약 (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    아니면
		s = cin_skipcomment(s + 1);
	}
	아니면 만약 ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    만약 (*s == 'c')
		s = cin_skipcomment(s + 5);
	    아니면
		s = cin_skipcomment(s + 6);
	}
	아니면
	{
	    만약 (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    아니면 만약 (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    아니면 만약 (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		반환 FALSE;
	    }
	    아니면 만약 (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    아니면 만약 (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		만약 (cpp_base_class)
		    pos->col = (colnr_T)(s - line);
	    }

	    // When the line ends in a comma don't align with it.
	    만약 (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    만약 (cpp_base_class)
	pos->lnum = lnum;
    반환 cpp_base_class;
}

    static 정수
get_baseclass_amount(정수 col)
{
    정수		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    만약 (col == 0)
    {
	amount = get_indent();
	만약 (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	만약 (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    아니면
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, NULL, NULL, 0);
	amount = (정수)vcol;
    }
    만약 (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    반환 amount;
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
    동안 ((trypos = findmatchlimit(NULL, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != NULL)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = NULL;
	// ignore the { if it's in a // or / *  * / comment
	만약 ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(NULL)) == NULL) // XXX
	    break;
	만약 (pos != NULL)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    반환 trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    static pos_T *
find_match_paren_after_brace (정수 ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    만약 (trypos == NULL)
	반환 NULL;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    만약 (tryposBrace != NULL
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = NULL;
    반환 trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    static 정수
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    만약 (n > 0 && n < curbuf->b_ind_maxparen / 2)
	반환 curbuf->b_ind_maxparen - (정수)n;
    반환 curbuf->b_ind_maxparen;
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
    정수		divider;
    정수		fraction = 0;
    정수		sw;
    long	t = get_sw_value(buf);

    // needed for cino-(, it will be multiplied by 2 again
    만약 (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    아니면
	sw = (정수)t;

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

    위해 (p = buf->b_p_cino; *p; )
    {
	l = p++;
	만약 (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	만약 (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((char *)++p);
	    동안 (VIM_ISDIGIT(*p))
	    {
		++p;
		만약 (divider)
		    divider *= 10;
		아니면
		    divider = 10;
	    }
	}
	만약 (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    만약 (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    아니면
	    {
		n *= sw;
		만약 (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	만약 (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	switch (*l)
	{
	    case '>': buf->b_ind_level = (정수)n; break;
	    case 'e': buf->b_ind_open_imag = (정수)n; break;
	    case 'n': buf->b_ind_no_brace = (정수)n; break;
	    case 'f': buf->b_ind_first_open = (정수)n; break;
	    case '{': buf->b_ind_open_extra = (정수)n; break;
	    case '}': buf->b_ind_close_extra = (정수)n; break;
	    case '^': buf->b_ind_open_left_imag = (정수)n; break;
	    case 'L': buf->b_ind_jump_label = (정수)n; break;
	    case ':': buf->b_ind_case = (정수)n; break;
	    case '=': buf->b_ind_case_code = (정수)n; break;
	    case 'b': buf->b_ind_case_break = (정수)n; break;
	    case 'p': buf->b_ind_param = (정수)n; break;
	    case 't': buf->b_ind_func_type = (정수)n; break;
	    case '/': buf->b_ind_comment = (정수)n; break;
	    case 'c': buf->b_ind_in_comment = (정수)n; break;
	    case 'C': buf->b_ind_in_comment2 = (정수)n; break;
	    case 'i': buf->b_ind_cpp_baseclass = (정수)n; break;
	    case '+': buf->b_ind_continuation = (정수)n; break;
	    case '(': buf->b_ind_unclosed = (정수)n; break;
	    case 'u': buf->b_ind_unclosed2 = (정수)n; break;
	    case 'U': buf->b_ind_unclosed_noignore = (정수)n; break;
	    case 'W': buf->b_ind_unclosed_wrapped = (정수)n; break;
	    case 'w': buf->b_ind_unclosed_whiteok = (정수)n; break;
	    case 'm': buf->b_ind_matching_paren = (정수)n; break;
	    case 'M': buf->b_ind_paren_prev = (정수)n; break;
	    case ')': buf->b_ind_maxparen = (정수)n; break;
	    case '*': buf->b_ind_maxcomment = (정수)n; break;
	    case 'g': buf->b_ind_scopedecl = (정수)n; break;
	    case 'h': buf->b_ind_scopedecl_code = (정수)n; break;
	    case 'j': buf->b_ind_java = (정수)n; break;
	    case 'J': buf->b_ind_js = (정수)n; break;
	    case 'l': buf->b_ind_keep_case_label = (정수)n; break;
	    case '#': buf->b_ind_hash_comment = (정수)n; break;
	    case 'N': buf->b_ind_cpp_namespace = (정수)n; break;
	    case 'k': buf->b_ind_if_for_while = (정수)n; break;
	    case 'E': buf->b_ind_cpp_extern_c = (정수)n; break;
	    case 'P': buf->b_ind_pragma = (정수)n; break;
	}
	만약 (*p == ',')
	    ++p;
    }
}

    static 정수
find_match(정수 lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    정수		elselevel;
    정수		whilelevel;

    만약 (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    아니면
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    동안 (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	만약 (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    만약 (theirscope == NULL)
		break;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    만약 (theirscope->lnum < ourscope)
		break;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    만약 (theirscope->lnum > ourscope)
		continue;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    만약 (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		만약 (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    만약 (!cin_isif(mightbeif))
			++elselevel;
		    continue;
		}

		// If it's an "if" decrement elselevel
		만약 (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    만약 (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    만약 (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		continue;
	    }

	    // If it's a "do" decrement whilelevel
	    만약 (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    만약 (elselevel <= 0 && whilelevel <= 0)
		반환 OK;
	}
    }
    반환 FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    정수
get_c_indent(void)
{
    pos_T	cur_curpos;
    정수		amount;
    정수		scope_amount;
    정수		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = NULL;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    정수		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    정수		lookfor;
    정수		whilelevel;
    linenr_T	lnum;
    정수		n;
    정수		iscase;
    정수		lookfor_break;
    정수		lookfor_cpp_namespace = FALSE;
    정수		cont_amount = 0;    // amount for continuation line
    정수		original_line_islabel;
    정수		added_to_amount = 0;
    정수		js_cur_has_key = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    정수		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    만약 (cur_curpos.lnum == 1)
	반환 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    만약 (linecopy == NULL)
	반환 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    만약 ((State & MODE_INSERT)
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
    만약 (comment_pos != NULL)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    만약 (trypos != NULL && (comment_pos == NULL
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	goto laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    만약 (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	만약 (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    goto theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    만약 (original_line_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	goto theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    만약 (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_line_comment(); // XXX
	만약 (trypos == NULL && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    만약 (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	만약 (trypos  != NULL)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, NULL, NULL, 0);
	    amount = col;
	    goto theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    만약 (!cin_iscomment(theline) && comment_pos != NULL) // XXX
    {
	정수	lead_start_len = 2;
	정수	lead_middle_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_middle[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	정수	lead_end_len;
	char_u	*p;
	정수	start_align = 0;
	정수	start_off = 0;
	정수	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, NULL, NULL, 0);
	amount = col;
	*lead_start = NUL;
	*lead_middle = NUL;

	p = curbuf->b_p_com;
	동안 (*p != NUL)
	{
	    정수	align = 0;
	    정수	off = 0;
	    정수 what = 0;

	    동안 (*p != NUL && *p != ':')
	    {
		만약 (*p == COM_START || *p == COM_END || *p == COM_MIDDLE)
		    what = *p++;
		아니면 만약 (*p == COM_LEFT || *p == COM_RIGHT)
		    align = *p++;
		아니면 만약 (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		아니면
		    ++p;
	    }

	    만약 (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    만약 (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    아니면 만약 (what == COM_MIDDLE)
	    {
		STRCPY(lead_middle, lead_end);
		lead_middle_len = lead_end_len;
	    }
	    아니면 만약 (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		만약 (STRNCMP(theline, lead_middle, lead_middle_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    만약 (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			만약 (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			아니면 만약 (STRNCMP(look, lead_middle,
							lead_middle_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    break;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			아니면 만약 (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    continue;
		    }
		    만약 (start_off != 0)
			amount += start_off;
		    아니면 만약 (start_align == COM_RIGHT)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_middle);
		    break;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		만약 (STRNCMP(theline, lead_middle, lead_middle_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    만약 (off != 0)
			amount += off;
		    아니면 만약 (align == COM_RIGHT)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_middle);
		    done = TRUE;
		    break;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	만약 (done)
	    ;
	아니면 만약 (theline[0] == '*')
	    amount += 1;
	아니면
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    위해 (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		만약 (linewhite(lnum))		    // skip blank lines
		    continue;
		amount = get_indent_lnum(lnum);	    // XXX
		break;
	    }
	    만약 (amount == -1)			    // use the comment opener
	    {
		만약 (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    만약 (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, NULL, NULL, 0);
		amount = col;
		만약 (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	goto theend;
    }

    // Are we looking at a ']' that has a match?
    만약 (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != NULL)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	goto theend;
    }

    // Are we inside parentheses or braces?  XXX
    만약 (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != NULL
	    || trypos != NULL)
    {
      만약 (trypos != NULL && tryposBrace != NULL)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  만약 (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = NULL;
	  아니면
	      tryposBrace = NULL;
      }

      만약 (trypos != NULL)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	만약 (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	아니면
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    위해 (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		만약 (cin_nocode(l))		// skip comment lines
		    continue;
		만약 (cin_ispreproc_cont(&l, &lnum, &amount))
		    continue;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		만약 ((trypos = ind_find_start_CORS(NULL)) != NULL)
		{
		    lnum = trypos->lnum + 1;
		    continue;
		}

		// XXX
		만약 ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != NULL
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    만약 (theline[0] == ')')
		    {
			만약 (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    break;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	만약 (amount == -1)
	{
	    정수	    ignore_paren_col = 0;
	    정수	    is_if_for_while = 0;

	    만약 (curbuf->b_ind_if_for_while)
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
		} 동안 (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		line = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(line, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    만약 (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *line;
		정수	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		line = ml_get_curline();
		look_col = (정수)(look - line);
		curwin->w_cursor.col = look_col + 1;
		만약 ((trypos = findmatchlimit(NULL, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != NULL
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    만약 (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		만약 (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    만약 (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			위해 (col = 0; col < our_paren_pos.col; ++col)
			{
			    switch (l[col])
			    {
				case '(':
				case '{': ++n;
					  break;

				case ')':
				case '}': 만약 (n > 1)
					      --n;
					  break;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    아니면 만약 (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    아니면
		    {
			col = our_paren_pos.col + 1;
			동안 (VIM_ISWHITE(l[col]))
			    col++;
			만약 (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			아니면
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		만약 (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, NULL, NULL, 0);
		    만약 (cur_amount > (정수)col)
			cur_amount = col;
		}
	    }

	    만약 (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    아니면 만약 ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		만약 (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    아니면
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		동안 ((정수)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    switch (*ml_get_pos(&our_paren_pos))
		    {
			case '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  break;
			case ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  break;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		만약 (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		아니면
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    만약 (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != NULL)
			amount += curbuf->b_ind_unclosed2;
		    아니면
		    {
			만약 (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			아니면
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		만약 (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	만약 (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      아니면
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
	만약 (*look == '{')
	{
	    getvcol(curwin, trypos, &col, NULL, NULL, 0);
	    amount = col;
	    만약 (*start == '{')
		start_brace = BRACE_IN_COL0;
	    아니면
		start_brace = BRACE_AT_START;
	}
	아니면
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    만약 (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != NULL)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    만약 ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    아니면 만약 (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    아니면
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	만약 (curbuf->b_ind_js)
	    js_cur_has_key = cin_has_js_key(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	만약 (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	아니면
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    만약 (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    아니면 만약 (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    만약 (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		만약 (find_match(lookfor, ourscope) == OK)
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

	    만약 (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_left_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    아니면 만약 (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    아니면
	    {
		만약 (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    만약 (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    아니면 만약 (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		아니면
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    만약 (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    만약 (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    아니면 만약 (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    아니면
	    {
		만약 (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    위해 (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		만약 (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    만약 (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			만약 (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    만약 (cont_amount > 0)
				amount = cont_amount;
			    아니면 만약 (!curbuf->b_ind_js)
				amount += ind_continuation;
			    break;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(NULL);
			만약 (trypos != NULL)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    continue;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			만약 (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    continue;

			만약 (cin_nocode(l))
			    continue;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			만약 (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    만약 (terminated == ',')
				break;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    만약 (terminated != ';' && cin_isinit())
				break;

			    // nothing useful found
			    만약 (terminated == 0 || terminated == '{')
				continue;
			}

			만약 (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = NULL;
			    만약 (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    만약 (trypos == NULL && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    만약 (trypos != NULL)
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
			만약 (cont_amount > 0)
			    amount = cont_amount;
			아니면
			    amount += ind_continuation;
		    }
		    아니면 만약 (lookfor == LOOKFOR_UNTERM)
		    {
			만약 (cont_amount > 0)
			    amount = cont_amount;
			아니면
			    amount += ind_continuation;
		    }
		    아니면
		    {
			만약 (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    만약 (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			만약 (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    만약 (curwin->w_cursor.lnum == ourscope)
				continue;

			    만약 (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				break;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(NULL);
			    만약 (trypos != NULL)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				continue;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    만약 (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				continue;

			    // Finally the actual check for "namespace".
			    만약 (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				break;
			    }
			    아니면 만약 (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				break;
			    }

			    만약 (cin_nocode(l))
				continue;
			}
		    }
		    break;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		만약 ((trypos = ind_find_start_CORS(&raw_string_start)) != NULL)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    continue;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		만약 (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    만약 (lookfor == LOOKFOR_CPP_BASECLASS)
			break;

		    // When looking for a "do" we are not interested in
		    // labels.
		    만약 (whilelevel > 0)
			continue;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    만약 (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			만약 (cont_amount > 0)
			    amount = cont_amount;
			아니면
			    amount += ind_continuation;
			break;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    만약 (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			만약 ((trypos = find_start_brace()) == NULL
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    break;
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
		    만약 (lookfor == LOOKFOR_TERM)
		    {
			만약 (n)
			    amount = n;

			만약 (!lookfor_break)
			    break;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    만약 (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			만약 (l != NULL && cin_is_cinword(l))
			{
			    만약 (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    아니면
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			break;
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
		만약 (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    만약 (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != NULL)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    continue;
		}

		// Ignore jump labels with nothing after them.
		만약 (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    만약 (l == NULL || cin_nocode(l))
			continue;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		만약 (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    continue;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		만약 (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		만약 (n)
		{
		    만약 (lookfor == LOOKFOR_UNTERM)
		    {
			만약 (cont_amount > 0)
			    amount = cont_amount;
			아니면
			    amount += ind_continuation;
		    }
		    아니면 만약 (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			continue;
		    }
		    아니면
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    break;
		}
		아니면 만약 (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    만약 (cin_isterminated(l, TRUE, FALSE))
			break;
		    아니면
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

		만약 (js_cur_has_key)
		{
		    js_cur_has_key = 0; // only check the first line
		    만약 (curbuf->b_ind_js && terminated == ',')
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
		만약 (lookfor == LOOKFOR_JS_KEY && cin_has_js_key(l))
		{
		    amount = get_indent();
		    break;
		}
		만약 (lookfor == LOOKFOR_COMMA)
		{
		    만약 (tryposBrace != NULL && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			break;
		    만약 (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			break;
		    아니면
		    {
			amount = get_indent();
			만약 (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    break;
		    }
		}

		만약 (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    만약 (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    만약 (trypos != NULL && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = NULL;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    만약 (trypos == NULL && terminated == ',')
		    {
			만약 (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    만약 (trypos != NULL)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			만약 (cin_iscase(l, FALSE) || cin_isscopedecl(l))
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
		    만약 (terminated == ',')
		    {
			동안 (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    만약 (*l == NUL || l[STRLEN(l) - 1] != '\\')
				break;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    만약 (curbuf->b_ind_js)
			cur_amount = get_indent();
		    아니면
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    만약 (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			만약 (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			만약 (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    continue;
			}
			break;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    만약 (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			만약 (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    만약 (cont_amount > 0)
				amount = cont_amount;
			    아니면
				amount += ind_continuation;
			    break;
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
			만약 (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			만약 (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    break;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			만약 (cin_isdo(l))
			{
			    만약 (whilelevel == 0)
				break;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			만약 (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    만약 (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    만약 ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				break;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    아니면
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			만약 (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    만약 (terminated == ',')
				amount += ind_continuation;
			    break;
			}

			만약 (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    만약 (terminated == ',')
			    {
				만약 (curbuf->b_ind_cpp_baseclass == 0)
				    break;

				lookfor = LOOKFOR_CPP_BASECLASS;
				continue;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    만약 (amount > cur_amount)
				amount = cur_amount;
			}
			아니면
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (정수)STRLEN(l);
			    만약 (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				break;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    만약 (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				만약 (curbuf->b_ind_js)
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
				    만약 (cin_iscomment(skipwhite(l)))
					break;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    만약 (trypos != NULL)
				    {
					만약 (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    break;
					}
					ourscope = trypos->lnum;
				    }
				}
				아니면
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    아니면
			    {
				만약 (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				만약 (lookfor != LOOKFOR_TERM
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
		아니면 만약 (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    만약 (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			만약 (cont_amount > 0)
			    amount = cont_amount;
			아니면
			    amount += ind_continuation;
			break;
		    }

		    만약 (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			만약 (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		아니면
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    만약 (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			continue;
		    }

		    // Handle "do {" line.
		    만약 (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			만약 (cin_isdo(l))
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
		    만약 (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			만약 (cont_amount > 0)
			    amount = cont_amount;
			아니면
			    amount += ind_continuation;
			break;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    만약 (lookfor == LOOKFOR_TERM)
		    {
			만약 (!lookfor_break && whilelevel == 0)
			    break;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    아니면
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			만약 (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != NULL)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    만약 (cin_iscase(l, FALSE) || cin_isscopedecl(l))
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

			만약 (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			만약 (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			만약 (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    만약 ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				break;
			    continue;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			만약 (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != NULL)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    만약 (*l == '}' || !cin_iselse(l))
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
      만약 (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      만약 (curbuf->b_ind_jump_label > 0 && original_line_islabel)
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

    만약 (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	goto theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    만약 (cur_curpos.lnum < curbuf->b_ml.ml_line_count
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
    동안 (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	만약 ((trypos = ind_find_start_CORS(NULL)) != NULL)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    continue;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	만약 (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	만약 (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    break;
	}

	// Skip preprocessor directives and blank lines.
	만약 (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    continue;

	만약 (cin_nocode(l))
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
	만약 (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    만약 (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != NULL)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    동안 (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		만약 (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    break;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    만약 (amount == 0)
		amount = cin_first_id_amount();
	    만약 (amount == 0)
		amount = ind_continuation;
	    break;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	만약 (cin_isfuncdecl(NULL, cur_curpos.lnum, 0))  // XXX
	    break;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	만약 (*skipwhite(l) == '}')
	    break;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	만약 (cin_ends_in(l, (char_u *)"};"))
	    break;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	만약 (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    break;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	만약 (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    동안 (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		만약 (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    break;
	    }
	    만약 (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		break;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	만약 (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    break;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	만약 (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    만약 (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		break;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	만약 ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	break;
    }

    // add extra indent for a comment
    만약 (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    만약 (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	만약 (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    만약 (cur_amount > 0)
		amount = cur_amount;
	    아니면 만약 (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    만약 (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    반환 amount;
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
    정수
in_cinkeys(
    정수		keytyped,
    정수		when,
    정수		line_is_empty)
{
    char_u	*look;
    정수		try_match;
    정수		try_match_word;
    char_u	*p;
    char_u	*line;
    정수		icase;
    정수		i;

    만약 (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	반환 FALSE;

#ifdef FEAT_EVAL
    만약 (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    아니면
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    동안 (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	switch (when)
	{
	    case '*': try_match = (*look == '*'); break;
	    case '!': try_match = (*look == '!'); break;
	    default: try_match = (*look != '*'); break;
	}
	만약 (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	만약 (*look == '0')
	{
	    try_match_word = try_match;
	    만약 (!line_is_empty)
		try_match = FALSE;
	    ++look;
	}
	아니면
	    try_match_word = FALSE;

	// does it look like a control character?
	만약 (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    만약 (try_match && keytyped == Ctrl_chr(look[1]))
		반환 TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	아니면 만약 (*look == 'o')
	{
	    만약 (try_match && keytyped == KEY_OPEN_FORW)
		반환 TRUE;
	    ++look;
	}
	아니면 만약 (*look == 'O')
	{
	    만약 (try_match && keytyped == KEY_OPEN_BACK)
		반환 TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	아니면 만약 (*look == 'e')
	{
	    만약 (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		만약 (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    반환 TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	아니면 만약 (*look == ':')
	{
	    만약 (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		만약 (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    반환 TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		만약 (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    만약 (i)
			반환 TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	아니면 만약 (*look == '<')
	{
	    만약 (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		만약 (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != NULL
						       && keytyped == look[1])
		    반환 TRUE;

		만약 (keytyped == get_special_key_code(look + 1))
		    반환 TRUE;
	    }
	    동안 (*look && *look != '>')
		look++;
	    동안 (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	아니면 만약 (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    만약 (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    아니면
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    만약 (p == NULL)
		p = look + STRLEN(look);
	    만약 ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		정수		match = FALSE;

		만약 (keytyped == KEY_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    line = ml_get_curline();
		    만약 (has_mbyte)
		    {
			char_u	*n;

			위해 (s = line + curwin->w_cursor.col; s > line; s = n)
			{
			    n = mb_prevptr(line, s);
			    만약 (!vim_iswordp(n))
				break;
			}
		    }
		    아니면
			위해 (s = line + curwin->w_cursor.col; s > line; --s)
			    만약 (!vim_iswordc(s[-1]))
				break;
		    만약 (s + (p - look) <= line + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		아니면
		    // TODO: multi-byte
		    만약 (keytyped == (정수)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((정수)p[-1])))
		{
		    line = ml_get_cursor();
		    만약 ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(line[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(line - (p - look), look, p - look)
				: STRNCMP(line - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		만약 (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    만약 (getwhitecols_curline() !=
				     (정수)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		만약 (match)
		    반환 TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	아니면
	{
	    만약 (try_match && *look == keytyped)
		반환 TRUE;
	    만약 (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    반환 FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    void
do_c_expr_indent(void)
{
#ifdef FEAT_EVAL
    만약 (*curbuf->b_p_inde != NUL)
	fixthisline(get_expr_indent);
    아니면
#endif
	fixthisline(get_c_indent);
}

#만약 defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    void
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    만약 (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	반환;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    만약 (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_number = get_c_indent();
	curwin->w_cursor = pos;
    }
    아니면
	rettv->vval.v_number = -1;
}
#endif
