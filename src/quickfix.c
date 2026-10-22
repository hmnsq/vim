/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * quickfix.c: functions for quickfix mode, using a file with error messages
 */

#include "vim.h"

#නම් defined(FEAT_QUICKFIX)

sa_nracanaa dir_stack_T
{
    sa_nracanaa dir_stack_T	*next;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
typedef sa_nracanaa qfline_S qfline_T;
sa_nracanaa qfline_S
{
    qfline_T	*qf_next;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_end_lnum;	// line number when the error has range or zero
    පූර්ණ_සංඛ්‍යා		qf_fnum;	// file number for the line
    පූර්ණ_සංඛ්‍යා		qf_col;		// column where the error occurred
    පූර්ණ_සංඛ්‍යා		qf_end_col;	// column when the error has range or zero
    පූර්ණ_සංඛ්‍යා		qf_nr;		// error number
    char_u	*qf_module;	// module name for this error
    char_u	*qf_fname;	// different filename if there're hard links
    char_u	*qf_pattern;	// search pattern for the error
    char_u	*qf_text;	// description of the error
    char_u	qf_viscol;	// set to TRUE if qf_col and qf_end_col is
				// screen column
    char_u	qf_cleared;	// set to TRUE if line has been deleted
    char_u	qf_type;	// type of the error (mostly 'E'); 1 for
				// :helpgrep
    typval_T	qf_user_data;	// custom user data associated with this item
    char_u	qf_valid;	// valid error message detected
};

/*
 * There is a stack of error lists.
 */
# define INVALID_QFIDX (-1)
# define INVALID_QFBUFNR (0)

/*
 * Quickfix list type.
 */
typedef ga_nanaa
{
    QFLT_QUICKFIX, // Quickfix list - global list
    QFLT_LOCATION, // Location list - per window list
    QFLT_INTERNAL  // Internal - Temporary list used by getqflist()/getloclist()
} qfltype_T;

/*
 * Quickfix/Location list definition
 * Contains a list of entries (qfline_T). qf_start points to the first entry
 * and qf_last points to the last entry. qf_count contains the list size.
 *
 * Usually the list contains one or more entries. But an empty list can be
 * created using setqflist()/setloclist() with a title and/or user context
 * information and entries can be added later using setqflist()/setloclist().
 */
typedef sa_nracanaa qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_start;	// pointer to the first error
    qfline_T	*qf_last;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    පූර්ණ_සංඛ්‍යා		qf_count;	// number of errors (0 means empty list)
    පූර්ණ_සංඛ්‍යා		qf_index;	// current index in the error list
    පූර්ණ_සංඛ්‍යා		qf_nonevalid;	// TRUE if not a single valid entry found
    පූර්ණ_සංඛ්‍යා		qf_has_user_data; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    sa_nracanaa dir_stack_T	*qf_dir_stack;
    char_u		*qf_directory;
    sa_nracanaa dir_stack_T	*qf_file_stack;
    char_u		*qf_currfile;
    පූර්ණ_සංඛ්‍යා			qf_multiline;
    පූර්ණ_සංඛ්‍යා			qf_multiignore;
    පූර්ණ_සංඛ්‍යා			qf_multiscan;
    long		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
sa_nracanaa qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    පූර්ණ_සංඛ්‍යා		qf_refcount;
    පූර්ණ_සංඛ්‍යා		qf_listcount;	    // current number of lists
    පූර්ණ_සංඛ්‍යා		qf_curlist;	    // current error list
    පූර්ණ_සංඛ්‍යා         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    පූර්ණ_සංඛ්‍යා		qf_bufnr;	    // quickfix window buffer number
};

static qf_info_T ql_info_actual; // global quickfix list
static qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
static int_u last_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
typedef sa_nracanaa efm_S efm_T;
sa_nracanaa efm_S
{
    regprog_T	    *prog;	// pre-formatted part of 'errorformat'
    efm_T	    *next;	// pointer to next (NULL if last)
    char_u	    addr[FMT_PATTERNS]; // indices of used % patterns
    char_u	    prefix;	// prefix of this format line:
				//   'D' enter directory
				//   'X' leave directory
				//   'A' start of multi-line message
				//   'E' error message
				//   'W' warning message
				//   'I' informational message
				//   'N' note message
				//   'C' continuation line
				//   'Z' end of multi-line message
				//   'G' general, unspecific message
				//   'P' push file (partial) message
				//   'Q' pop/quit file (partial) message
				//   'O' overread (partial) message
    char_u	    flags;	// additional flags given in prefix
				//   '-' do not include this line
				//   '+' include whole line in message
    පූර්ණ_සංඛ්‍යා		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
typedef sa_nracanaa qf_delq_S
{
    sa_nracanaa qf_delq_S	*next;
    qf_info_T		*qi;
} qf_delq_T;
static qf_delq_T *qf_delq_head = NULL;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
static පූර්ණ_සංඛ්‍යා	quickfix_busy = 0;

static efm_T	*fmt_start = NULL; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
static callback_T qftf_cb;

static void     qf_pop_stack(qf_info_T *qi, පූර්ණ_සංඛ්‍යා adjust);
static void	qf_new_list(qf_info_T *qi, char_u *qf_title);
static පූර්ණ_සංඛ්‍යා	qf_add_entry(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, පූර්ණ_සංඛ්‍යා bufnum, char_u *mesg, long lnum, long end_lnum, පූර්ණ_සංඛ්‍යා col, පූර්ණ_සංඛ්‍යා end_col, පූර්ණ_සංඛ්‍යා vis_col, char_u *pattern, පූර්ණ_සංඛ්‍යා nr, පූර්ණ_සංඛ්‍යා type, typval_T *user_data, පූර්ණ_සංඛ්‍යා valid);
static පූර්ණ_සංඛ්‍යා      qf_resize_stack_base(qf_info_T *qi, පූර්ණ_සංඛ්‍යා n);
static void     qf_sync_llw_to_win(win_T *llw);
static void     qf_sync_win_to_llw(win_T *pwp);
static qf_info_T *qf_alloc_stack(qfltype_T qfltype, පූර්ණ_සංඛ්‍යා n);
static qf_list_T *qf_alloc_list_stack(පූර්ණ_සංඛ්‍යා n);
static void	qf_free(qf_list_T *qfl);
static char_u	*qf_types(පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා);
static පූර්ණ_සංඛ්‍යා	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
static char_u	*qf_push_dir(char_u *, sa_nracanaa dir_stack_T **, පූර්ණ_සංඛ්‍යා is_file_stack);
static char_u	*qf_pop_dir(sa_nracanaa dir_stack_T **);
static char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
static win_T    *qf_find_win_with_loclist(qf_info_T *ll);
static void	qf_jump_newwin(qf_info_T *qi, පූර්ණ_සංඛ්‍යා dir, පූර්ණ_සංඛ්‍යා errornr, පූර්ණ_සංඛ්‍යා forceit, පූර්ණ_සංඛ්‍යා newwin);
static void	qf_fmt_text(garray_T *gap, char_u *text);
static void	qf_range_text(garray_T *gap, qfline_T *qfp);
static පූර්ණ_සංඛ්‍යා	qf_win_pos_update(qf_info_T *qi, පූර්ණ_සංඛ්‍යා old_qf_index);
static win_T	*qf_find_win(qf_info_T *qi);
static buf_T	*qf_find_buf(qf_info_T *qi);
static void	qf_update_buffer(qf_info_T *qi, qfline_T *old_last);
static void	qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, පූර්ණ_සංඛ්‍යා qf_winid);
static buf_T	*load_dummy_buffer(char_u *fname, char_u *dirname_start, char_u *resulting_dir);
static void	wipe_dummy_buffer(buf_T *buf, char_u *dirname_start);
static void	unload_dummy_buffer(buf_T *buf, char_u *dirname_start);
static qf_info_T *ll_get_or_alloc_list(win_T *);
static පූර්ණ_සංඛ්‍යා	entry_is_closer_to_target(qfline_T *entry, qfline_T *other_entry, පූර්ණ_සංඛ්‍යා target_fnum, පූර්ණ_සංඛ්‍යා target_lnum, පූර්ණ_සංඛ්‍යා target_col);

// Quickfix window check helper macro
# define IS_QF_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref == NULL)
// Location list window check helper macro
# define IS_LL_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref != NULL)

// Quickfix and location list stack check helper macros
# define IS_QF_STACK(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_STACK(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_WINDOW(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_ITEMS(qfl, qfp, i) \
		    සඳහා ((i) = 1, (qfp) = (qfl)->qf_start; \
			    !got_int && (i) <= (qfl)->qf_count && (qfp) != NULL; \
			    ++(i), (qfp) = (qfp)->qf_next)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
static char_u   *qf_last_bufname = NULL;
static bufref_T  qf_last_bufref = {NULL, 0, 0};

static garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    static garray_T *
qfga_get(void)
{
    static පූර්ණ_සංඛ්‍යා initialized = FALSE;

    නම් (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    ආපසු &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    static void
qfga_clear(void)
{
    නම් (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    නැතහොත්
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define LINE_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
static sa_nracanaa fmtpattern
{
    char_u	convchar;
    var_na	*pattern;
} fmt_pat[FMT_PATTERNS] =
    {
	{'f', ".\\+"},	    // only used when at end
	{'b', "\\d\\+"},	// 1
	{'n', "\\d\\+"},	// 2
	{'l', "\\d\\+"},	// 3
	{'e', "\\d\\+"},	// 4
	{'c', "\\d\\+"},	// 5
	{'k', "\\d\\+"},	// 6
	{'t', "."},		// 7
# define FMT_PATTERN_M 8
	{'m', ".\\+"},		// 8
# define FMT_PATTERN_R 9
	{'r', ".*"},		// 9
	{'p', "[-	 .]*"},	// 10
	{'v', "\\d\\+"},	// 11
	{'s', ".\\+"},		// 12
	{'o', ".\\+"}		// 13
    };

/*
 * Convert an errorformat pattern to a regular expression pattern.
 * See fmt_pat definition above for the list of supported patterns.  The
 * pattern specifier is supplied in "efmpat".  The converted pattern is stored
 * in "regpat".  Returns a pointer to the location after the pattern.
 */
    static char_u *
efmpat_to_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	පූර්ණ_සංඛ්‍යා	idx,
	පූර්ණ_සංඛ්‍යා	round)
{
    char_u	*srcptr;

    නම් (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_in_format_string), *efmpat);
	ආපසු NULL;
    }
    නම් ((idx && idx < FMT_PATTERN_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->prefix) != NULL)
	    || (idx == FMT_PATTERN_R
		&& vim_strchr((char_u *)"OPQ", efminfo->prefix) == NULL))
    {
	semsg(_(e_unexpected_chr_in_format_str), *efmpat);
	ආපසු NULL;
    }
    efminfo->addr[idx] = (char_u)++round;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_IN_FILENAME
    නම් (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    නම් (*efmpat == 'f' && efmpat[1] != NUL)
    {
	නම් (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	නැතහොත්
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    නැතහොත්
    {
	srcptr = (char_u *)fmt_pat[idx].pattern;
	අතරතුර ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    ආපසු regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    static char_u *
scanf_fmt_to_regpat(
	char_u	**pefmp,
	char_u	*efm,
	පූර්ණ_සංඛ්‍යා	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    නම් (*efmp == '[' || *efmp == '\\')
    {
	නම් ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    නම් (efmp[1] == '^')
		*regpat++ = *++efmp;
	    නම් (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		අතරතුර (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		නම් (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_in_format_string));
		    ආපසු NULL;
		}
	    }
	}
	නැතහොත් නම් (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    නැතහොත්
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_in_format_string), *efmp);
	ආපසු NULL;
    }

    *pefmp = efmp;

    ආපසු regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    static char_u *
efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    නම් (vim_strchr((char_u *)"+-", *efmp) != NULL)
	efminfo->flags = *efmp++;
    නම් (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != NULL)
	efminfo->prefix = *efmp;
    නැතහොත්
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	ආපසු NULL;
    }

    ආපසු efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
efm_to_regpat(
	char_u	*efm,
	පූර්ණ_සංඛ්‍යා	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    පූර්ණ_සංඛ්‍යා		round;
    පූර්ණ_සංඛ්‍යා		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    සඳහා (efmp = efm; efmp < efm + len; ++efmp)
    {
	නම් (*efmp == '%')
	{
	    ++efmp;
	    සඳහා (idx = 0; idx < FMT_PATTERNS; ++idx)
		නම් (fmt_pat[idx].convchar == *efmp)
		    bha_nga;
	    නම් (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		නම් (ptr == NULL)
		    ආපසු FAIL;
		round++;
	    }
	    නැතහොත් නම් (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		නම් (ptr == NULL)
		    ආපසු FAIL;
	    }
	    නැතහොත් නම් (vim_strchr((char_u *)"%\\.^$~[", *efmp) != NULL)
		*ptr++ = *efmp;		// regexp magic characters
	    නැතහොත් නම් (*efmp == '#')
		*ptr++ = '*';
	    නැතහොත් නම් (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    නැතහොත් නම් (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		නම් (efmp == NULL)
		    ආපසු FAIL;
	    }
	    නැතහොත්
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		ආපසු FAIL;
	    }
	}
	නැතහොත්			// copy normal character
	{
	    නම් (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    නැතහොත් නම් (vim_strchr((char_u *)".*^$~[", *efmp) != NULL)
		*ptr++ = '\\';	// escape regexp atoms
	    නම් (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    ආපසු OK;
}

/*
 * Free the 'errorformat' information list
 */
    static void
free_efm_list(efm_T **efm_first)
{
    efm_T *efm_ptr;

    සඳහා (efm_ptr = *efm_first; efm_ptr != NULL; efm_ptr = *efm_first)
    {
	*efm_first = efm_ptr->next;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_start = NULL;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    static පූර්ණ_සංඛ්‍යා
efm_regpat_bufsz(char_u *efm)
{
    පූර්ණ_සංඛ්‍යා sz;
    පූර්ණ_සංඛ්‍යා i;

    sz = (FMT_PATTERNS * 3) + ((පූර්ණ_සංඛ්‍යා)STRLEN(efm) << 2);
    සඳහා (i = FMT_PATTERNS; i > 0; )
	sz += (පූර්ණ_සංඛ්‍යා)STRLEN(fmt_pat[--i].pattern);
# ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# නැතහොත්
    sz += 2; // "%f" can become two chars longer
# endif

    ආපසු sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    static පූර්ණ_සංඛ්‍යා
efm_option_part_len(char_u *efm)
{
    පූර්ණ_සංඛ්‍යා len;

    සඳහා (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	නම් (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    ආපසු len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    static efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = NULL;
    efm_T	*fmt_first = NULL;
    efm_T	*fmt_last = NULL;
    char_u	*fmtstr = NULL;
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    නම් ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == NULL)
	goto parse_efm_error;

    අතරතුර (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	නම් (fmt_ptr == NULL)
	    goto parse_efm_error;
	නම් (fmt_first == NULL)	    // first one
	    fmt_first = fmt_ptr;
	නැතහොත්
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	නම් (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    goto parse_efm_error;
	නම් ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == NULL)
	    goto parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    නම් (fmt_first == NULL)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    goto parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    ආපසු fmt_first;
}

ga_nanaa {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_END_OF_INPUT = 2,
    QF_NOMEM = 3,
    QF_IGNORE_LINE = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
typedef sa_nracanaa {
    char_u	*linebuf;
    පූර්ණ_සංඛ්‍යා		linelen;
    char_u	*growbuf;
    පූර්ණ_සංඛ්‍යා		growbufsiz;
    FILE	*fd;
    typval_T	*tv;
    char_u	*p_str;
    listitem_T	*p_li;
    buf_T	*buf;
    linenr_T	buflnum;
    linenr_T	lnumlast;
    vimconv_T	vc;
} qfstate_T;

/*
 * Allocate more memory for the line buffer used for parsing lines.
 */
    static char_u *
qf_grow_linebuf(qfstate_T *state, පූර්ණ_සංඛ්‍යා newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > LINE_MAXLEN ? LINE_MAXLEN - 1 : newsz;
    නම් (state->growbuf == NULL)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	නම් (state->growbuf == NULL)
	    ආපසු NULL;
	state->growbufsiz = state->linelen;
    }
    නැතහොත් නම් (state->linelen > state->growbufsiz)
    {
	නම් ((p = vim_realloc(state->growbuf, state->linelen + 1)) == NULL)
	    ආපසු NULL;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    ආපසු state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    static පූර්ණ_සංඛ්‍යා
qf_get_next_str_line(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		len;

    නම් (*p_str == NUL) // Reached the end of the string
	ආපසු QF_END_OF_INPUT;

    p = vim_strchr(p_str, '\n');
    නම් (p != NULL)
	len = (පූර්ණ_සංඛ්‍යා)(p - p_str) + 1;
    නැතහොත්
	len = (පූර්ණ_සංඛ්‍යා)STRLEN(p_str);

    නම් (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	නම් (state->linebuf == NULL)
	    ආපසු QF_NOMEM;
    }
    නැතහොත්
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    ආපසු QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    static පූර්ණ_සංඛ්‍යා
qf_get_next_list_line(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    පූර්ණ_සංඛ්‍යා		len;

    අතරතුර (p_li != NULL
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == NULL))
	p_li = p_li->li_next;	// Skip non-string items

    නම් (p_li == NULL)		// End of the list
    {
	state->p_li = NULL;
	ආපසු QF_END_OF_INPUT;
    }

    len = (පූර්ණ_සංඛ්‍යා)STRLEN(p_li->li_tv.vval.v_string);
    නම් (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	නම් (state->linebuf == NULL)
	    ආපසු QF_NOMEM;
    }
    නැතහොත්
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_next;	// next item
    ආපසු QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    static පූර්ණ_සංඛ්‍යා
qf_get_next_buf_line(qfstate_T *state)
{
    char_u	*p_buf = NULL;
    පූර්ණ_සංඛ්‍යා		len;

    // Get the next line from the supplied buffer
    නම් (state->buflnum > state->lnumlast)
	ආපසු QF_END_OF_INPUT;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    නම් (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	නම් (state->linebuf == NULL)
	    ආපසු QF_NOMEM;
    }
    නැතහොත්
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    ආපසු QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    static පූර්ණ_සංඛ්‍යා
qf_get_next_file_line(qfstate_T *state)
{
    පූර්ණ_සංඛ්‍යා	    discard;
    පූර්ණ_සංඛ්‍යා	    growbuflen;

    නම් (fgets((var_na *)IObuff, IOSIZE, state->fd) == NULL)
	ආපසු QF_END_OF_INPUT;

    discard = FALSE;
    state->linelen = (පූර්ණ_සංඛ්‍යා)STRLEN(IObuff);
    නම් (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	නම් (state->growbuf == NULL)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    නම් (state->growbuf == NULL)
		ආපසු QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	memcpy(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	සඳහා (;;)
	{
	    char_u	*p;

	    නම් (fgets((var_na *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == NULL)
		bha_nga;
	    state->linelen = (පූර්ණ_සංඛ්‍යා)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    නම් ((state->growbuf)[growbuflen - 1] == '\n')
		bha_nga;
	    නම් (state->growbufsiz == LINE_MAXLEN)
	    {
		discard = TRUE;
		bha_nga;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < LINE_MAXLEN
		? 2 * state->growbufsiz : LINE_MAXLEN;
	    නම් ((p = vim_realloc(state->growbuf, state->growbufsiz)) == NULL)
		ආපසු QF_NOMEM;
	    state->growbuf = p;
	}

	අතරතුර (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    නම් (fgets((var_na *)IObuff, IOSIZE, state->fd) == NULL
		    || (පූර්ණ_සංඛ්‍යා)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		bha_nga;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    නැතහොත්
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    නම් (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*line;

	line = string_convert(&state->vc, state->linebuf, &state->linelen);
	නම් (line != NULL)
	{
	    නම් (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, line);
		vim_free(line);
	    }
	    නැතහොත්
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = line;
		state->growbufsiz = state->linelen < LINE_MAXLEN
						? state->linelen : LINE_MAXLEN;
	    }
	}
    }

    ආපසු QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    static පූර්ණ_සංඛ්‍යා
qf_get_nextline(qfstate_T *state)
{
    පූර්ණ_සංඛ්‍යා status = QF_FAIL;

    නම් (state->fd == NULL)
    {
	නම් (state->tv != NULL)
	{
	    නම් (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_line(state);
	    නැතහොත් නම් (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_line(state);
	}
	නැතහොත්
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_line(state);
    }
    නැතහොත්
	// Get the next line from the supplied file
	status = qf_get_next_file_line(state);

    නම් (status != QF_OK)
	ආපසු status;

    // remove newline/CR from the line
    නම් (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	නම් (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    ආපසු QF_OK;
}

typedef sa_nracanaa {
    char_u	*namebuf;
    පූර්ණ_සංඛ්‍යා		bnr;
    char_u	*module;
    char_u	*errmsg;
    පූර්ණ_සංඛ්‍යා		errmsglen;
    long	lnum;
    long	end_lnum;
    පූර්ණ_සංඛ්‍යා		col;
    පූර්ණ_සංඛ්‍යා		end_col;
    char_u	use_viscol;
    char_u	*pattern;
    පූර්ණ_සංඛ්‍යා		enr;
    පූර්ණ_සංඛ්‍යා		type;
    typval_T	*user_data;
    පූර්ණ_සංඛ්‍යා		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_f(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields, පූර්ණ_සංඛ්‍යා prefix)
{
    පූර්ණ_සංඛ්‍යා c;

    නම් (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	ආපසු QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    නම් (vim_strchr((char_u *)"OPQ", prefix) != NULL
	    && mch_getperm(fields->namebuf) == -1)
	ආපසු QF_FAIL;

    ආපසු QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_b(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    නම් (rmp->startp[midx] == NULL)
	ආපසු QF_FAIL;
    පූර්ණ_සංඛ්‍යා bnr = (පූර්ණ_සංඛ්‍යා)atol((var_na *)rmp->startp[midx]);
    නම් (buflist_findnr(bnr) == NULL)
	ආපසු QF_FAIL;
    fields->bnr = bnr;
    ආපසු QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_n(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    නම් (rmp->startp[midx] == NULL)
	ආපසු QF_FAIL;
    fields->enr = (පූර්ණ_සංඛ්‍යා)atol((var_na *)rmp->startp[midx]);
    ආපසු QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_l(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    නම් (rmp->startp[midx] == NULL)
	ආපසු QF_FAIL;
    fields->lnum = atol((var_na *)rmp->startp[midx]);
    ආපසු QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_e(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    නම් (rmp->startp[midx] == NULL)
	ආපසු QF_FAIL;
    fields->end_lnum = atol((var_na *)rmp->startp[midx]);
    ආපසු QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_c(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    නම් (rmp->startp[midx] == NULL)
	ආපසු QF_FAIL;
    fields->col = (පූර්ණ_සංඛ්‍යා)atol((var_na *)rmp->startp[midx]);
    ආපසු QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_k(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    නම් (rmp->startp[midx] == NULL)
	ආපසු QF_FAIL;
    fields->end_col = (පූර්ණ_සංඛ්‍යා)atol((var_na *)rmp->startp[midx]);
    ආපසු QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_t(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    නම් (rmp->startp[midx] == NULL)
	ආපසු QF_FAIL;
    fields->type = *rmp->startp[midx];
    ආපසු QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    static පූර්ණ_සංඛ්‍යා
copy_nonerror_line(char_u *linebuf, පූර්ණ_සංඛ්‍යා linelen, qffields_T *fields)
{
    char_u	*p;

    නම් (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	නම් ((p = vim_realloc(fields->errmsg, linelen + 1)) == NULL)
	    ආපසු QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    ආපසු QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_m(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		len;

    නම් (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	ආපසු QF_FAIL;
    len = (පූර්ණ_සංඛ්‍යා)(rmp->endp[midx] - rmp->startp[midx]);
    නම් (len >= fields->errmsglen)
    {
	// len + null terminator
	නම් ((p = vim_realloc(fields->errmsg, len + 1)) == NULL)
	    ආපසු QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    ආපසු QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_r(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, char_u **tail)
{
    නම් (rmp->startp[midx] == NULL)
	ආපසු QF_FAIL;
    *tail = rmp->startp[midx];
    ආපසු QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_p(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    char_u	*match_ptr;

    නම් (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	ආපසු QF_FAIL;
    fields->col = 0;
    සඳහා (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	නම් (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    ආපසු QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_v(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    නම් (rmp->startp[midx] == NULL)
	ආපසු QF_FAIL;
    fields->col = (පූර්ණ_සංඛ්‍යා)atol((var_na *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    ආපසු QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_s(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    පූර්ණ_සංඛ්‍යා		len;

    නම් (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	ආපසු QF_FAIL;
    len = (පූර්ණ_සංඛ්‍යා)(rmp->endp[midx] - rmp->startp[midx]);
    නම් (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    ආපසු QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_fmt_o(regmatch_T *rmp, පූර්ණ_සංඛ්‍යා midx, qffields_T *fields)
{
    පූර්ණ_සංඛ්‍යා		len;

    නම් (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	ආපසු QF_FAIL;
    len = (පූර්ණ_සංඛ්‍යා)(rmp->endp[midx] - rmp->startp[midx]);
    නම් (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    ආපසු QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
static පූර්ණ_සංඛ්‍යා (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, පූර්ණ_සංඛ්‍යා, qffields_T *) =
{
    NULL, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    NULL, // %r
    qf_parse_fmt_p,
    qf_parse_fmt_v,
    qf_parse_fmt_s,
    qf_parse_fmt_o
};

/*
 * Parse the error format pattern matches in "regmatch" and set the values in
 * "fields".  fmt_ptr contains the 'efm' format specifiers/prefixes that have a
 * match.  Returns QF_OK if all the matches are successfully parsed. On
 * failure, returns QF_FAIL or QF_NOMEM.
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_match(
	char_u		*linebuf,
	පූර්ණ_සංඛ්‍යා		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	පූර්ණ_සංඛ්‍යා		qf_multiline,
	පූර්ණ_සංඛ්‍යා		qf_multiscan,
	char_u		**tail)
{
    පූර්ණ_සංඛ්‍යා		idx = fmt_ptr->prefix;
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		midx;
    පූර්ණ_සංඛ්‍යා		status;

    නම් ((idx == 'C' || idx == 'Z') && !qf_multiline)
	ආපසු QF_FAIL;
    නම් (vim_strchr((char_u *)"EWIN", idx) != NULL)
	fields->type = idx;
    නැතහොත්
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    සඳහා (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (පූර්ණ_සංඛ්‍යා)fmt_ptr->addr[i];
	නම් (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	නැතහොත් නම් (i == FMT_PATTERN_M)
	{
	    නම් (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_line(linebuf, linelen, fields);
	    නැතහොත් නම් (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	නැතහොත් නම් (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	නැතහොත් නම් (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	නම් (status != QF_OK)
	    ආපසු status;
    }

    ආපසු QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_get_fields(
	char_u		*linebuf,
	පූර්ණ_සංඛ්‍යා		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	පූර්ණ_සංඛ්‍යා		qf_multiline,
	පූර්ණ_සංඛ්‍යා		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    පූර්ණ_සංඛ්‍යා		status = QF_FAIL;
    පූර්ණ_සංඛ්‍යා		r;

    නම් (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == NULL)
	ආපසු QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    නම් (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = NULL;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    නම් (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    ආපසු status;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_dir_pfx(පූර්ණ_සංඛ්‍යා idx, qffields_T *fields, qf_list_T *qfl)
{
    නම් (idx == 'D')				// enter directory
    {
	නම් (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    ආපසු QF_FAIL;
	}
	qfl->qf_directory =
	    qf_push_dir(fields->namebuf, &qfl->qf_dir_stack, FALSE);
	නම් (qfl->qf_directory == NULL)
	    ආපසු QF_FAIL;
    }
    නැතහොත් නම් (idx == 'X')			// leave directory
	qfl->qf_directory = qf_pop_dir(&qfl->qf_dir_stack);

    ආපසු QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_file_pfx(
	පූර්ණ_සංඛ්‍යා idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    නම් (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	නම් (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_push_dir(fields->namebuf, &qfl->qf_file_stack, TRUE);
	නැතහොත් නම් (idx == 'Q')
	    qfl->qf_currfile = qf_pop_dir(&qfl->qf_file_stack);
	*fields->namebuf = NUL;
	නම් (tail && *tail)
	{
	    qfl->qf_multiscan = TRUE;
	    ආපසු QF_MULTISCAN;
	}
    }

    ආපසු QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_line_nomatch(char_u *linebuf, පූර්ණ_සංඛ්‍යා linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    ආපසු copy_nonerror_line(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_multiline_pfx(
	පූර්ණ_සංඛ්‍යා idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    පූර්ණ_සංඛ්‍යා			len;

    නම් (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	නම් (qfprev == NULL)
	    ආපසු QF_FAIL;
	නම් (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (පූර්ණ_සංඛ්‍යා)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    නම් (ptr == NULL)
		ආපසු QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	නම් (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	නම් (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	නම් (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	නම් (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	නම් (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	නම් (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	නම් (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != NULL
		    ? fields->namebuf
		    : qfl->qf_currfile != NULL && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    නම් (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    line_breakcheck();

    ආපසු QF_IGNORE_LINE;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    static පූර්ණ_සංඛ්‍යා
qf_parse_line(
	qf_list_T	*qfl,
	char_u		*linebuf,
	පූර්ණ_සංඛ්‍යා		linelen,
	efm_T		*fmt_first,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    පූර්ණ_සංඛ්‍යා			idx = 0;
    char_u		*tail = NULL;
    පූර්ණ_සංඛ්‍යා			status;

restofline:
    // If there was no %> item start at the first pattern
    නම් (fmt_start == NULL)
	fmt_ptr = fmt_first;
    නැතහොත්
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_start;
	fmt_start = NULL;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    සඳහා ( ; fmt_ptr != NULL; fmt_ptr = fmt_ptr->next)
    {
	idx = fmt_ptr->prefix;
	status = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &tail);
	නම් (status == QF_NOMEM)
	    ආපසු status;
	නම් (status == QF_OK)
	    bha_nga;
    }
    qfl->qf_multiscan = FALSE;

    නම් (fmt_ptr == NULL || idx == 'D' || idx == 'X')
    {
	නම් (fmt_ptr != NULL)
	{
	    // 'D' and 'X' directory specifiers
	    status = qf_parse_dir_pfx(idx, fields, qfl);
	    නම් (status != QF_OK)
		ආපසු status;
	}

	status = qf_parse_line_nomatch(linebuf, linelen, fields);
	නම් (status != QF_OK)
	    ආපසු status;

	නම් (fmt_ptr == NULL)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    නැතහොත් නම් (fmt_ptr != NULL)
    {
	// honor %> item
	නම් (fmt_ptr->conthere)
	    fmt_start = fmt_ptr;

	නම් (vim_strchr((char_u *)"AEWIN", idx) != NULL)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	නැතහොත් නම් (vim_strchr((char_u *)"CZ", idx) != NULL)
	{				// continuation of multi-line msg
	    status = qf_parse_multiline_pfx(idx, qfl, fields);
	    නම් (status != QF_OK)
		ආපසු status;
	}
	නැතහොත් නම් (vim_strchr((char_u *)"OPQ", idx) != NULL)
	{				// global file names
	    status = qf_parse_file_pfx(idx, fields, qfl, tail);
	    නම් (status == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(tail);
		පූර්ණ_සංඛ්‍යා new_linelen = (පූර්ණ_සංඛ්‍යා)STRLEN(s);
		නම් (new_linelen >= linelen)
		    ආපසු QF_IGNORE_LINE;
		linebuf = s;
		linelen = new_linelen;
		goto restofline;
	    }
	}
	නම් (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    නම් (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    ආපසු QF_IGNORE_LINE;
	}
    }

    ආපසු QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    static පූර්ණ_සංඛ්‍යා
qf_stack_empty(qf_info_T *qi)
{
    ආපසු qi == NULL || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    static පූර්ණ_සංඛ්‍යා
qf_list_empty(qf_list_T *qfl)
{
    ආපසු qfl == NULL || qfl->qf_count <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    static පූර්ණ_සංඛ්‍යා
qf_list_has_valid_entries(qf_list_T *qfl)
{
    ආපසු !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    static qf_list_T *
qf_get_list(qf_info_T *qi, පූර්ණ_සංඛ්‍යා idx)
{
    ආපසු &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    static පූර්ණ_සංඛ්‍යා
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->pattern = alloc_id(CMDBUFFSIZE + 1, aid_qf_pattern);
    නම් (pfields->namebuf == NULL || pfields->errmsg == NULL
		|| pfields->pattern == NULL || pfields->module == NULL)
	ආපසු FAIL;

    ආපසු OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    static void
qf_free_fields(qffields_T *pfields)
{
    vim_free(pfields->namebuf);
    vim_free(pfields->module);
    vim_free(pfields->errmsg);
    vim_free(pfields->pattern);
}

/*
 * Setup the state information used for parsing lines and populating a
 * quickfix list.
 */
    static පූර්ණ_සංඛ්‍යා
qf_setup_state(
	qfstate_T	*pstate,
	char_u		*enc,
	char_u		*efile,
	typval_T	*tv,
	buf_T		*buf,
	linenr_T	lnumfirst,
	linenr_T	lnumlast)
{
    pstate->vc.vc_type = CONV_NONE;
    නම් (enc != NULL && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    නම් (efile != NULL && (pstate->fd = mch_fopen((var_na *)efile, "r")) == NULL)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	ආපසු FAIL;
    }

    නම් (tv != NULL)
    {
	නම් (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	නැතහොත් නම් (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    ආපසු OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    static void
qf_cleanup_state(qfstate_T *pstate)
{
    නම් (pstate->fd != NULL)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    නම් (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, NULL, NULL);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    static පූර්ණ_සංඛ්‍යා
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    පූර්ණ_සංඛ්‍යා		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    නම් (status != QF_OK)
	ආපසු status;

    status = qf_parse_line(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    නම් (status != QF_OK)
	ආපසු status;

    ආපසු qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != NULL)
		? fields->namebuf
		: ((qfl->qf_currfile != NULL && fields->valid)
		    ? qfl->qf_currfile : (char_u *)NULL),
		fields->module,
		fields->bnr,
		fields->errmsg,
		fields->lnum,
		fields->end_lnum,
		fields->col,
		fields->end_col,
		fields->use_viscol,
		fields->pattern,
		fields->enr,
		fields->type,
		fields->user_data,
		fields->valid);
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list.
 * Alternative: when "efile" is NULL read errors from buffer "buf".
 * Alternative: when "tv" is not NULL get errors from the string or list.
 * Always use 'errorformat' from "buf" if there is a local value.
 * Then "lnumfirst" and "lnumlast" specify the range of lines to use.
 * Set the title of the list to "qf_title".
 * Return -1 for error, number of errors for success.
 */
    static පූර්ණ_සංඛ්‍යා
qf_init_ext(
    qf_info_T	    *qi,
    පූර්ණ_සංඛ්‍යා		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    පූර්ණ_සංඛ්‍යා		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_last = NULL;
    පූර්ණ_සංඛ්‍යා		    adding = FALSE;
    static efm_T    *fmt_first = NULL;
    char_u	    *efm;
    static char_u   *last_efm = NULL;
    පූර්ණ_සංඛ්‍යා		    retval = -1;	// default: return error flag
    පූර්ණ_සංඛ්‍යා		    status;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_last_bufname);

    CLEAR_FIELD(state);
    CLEAR_FIELD(fields);
    නම් ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_setup_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	goto qf_init_end;

    නම් (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    නැතහොත්
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	නම් (!qf_list_empty(qfl))
	    old_last = qfl->qf_last;
    }

    // Use the local value of 'errorformat' if it's set.
    නම් (errorformat == p_efm && tv == NULL && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    නැතහොත්
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    නම් (last_efm == NULL || (STRCMP(last_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(last_efm);
	free_efm_list(&fmt_first);

	// parse the current 'efm'
	fmt_first = parse_efm_option(efm);
	නම් (fmt_first != NULL)
	    last_efm = vim_strsave(efm);
    }

    නම් (fmt_first == NULL)	// nothing found
	goto error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    අතරතුර (!got_int)
    {
	status = qf_init_process_nextline(qfl, fmt_first, &state, &fields);
	නම් (status == QF_NOMEM)		// memory alloc failure
	    goto qf_init_end;
	නම් (status == QF_END_OF_INPUT)	// end of input
	    bha_nga;
	නම් (status == QF_FAIL)
	    goto error2;

	line_breakcheck();
    }
    නම් (state.fd == NULL || !ferror(state.fd))
    {
	නම් (qfl->qf_index == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_start;
	    qfl->qf_index = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	නැතහොත්
	{
	    qfl->qf_nonevalid = FALSE;
	    නම් (qfl->qf_ptr == NULL)
		qfl->qf_ptr = qfl->qf_start;
	}
	// return number of matches
	retval = qfl->qf_count;
	goto qf_init_end;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    නම් (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	නම් (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_end:
    නම් (qf_idx == qi->qf_curlist)
	qf_update_buffer(qi, old_last);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    ආපසු retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    පූර්ණ_සංඛ්‍යා
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	පූර්ණ_සංඛ්‍යා	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    නම් (wp != NULL)
	qi = ll_get_or_alloc_list(wp);
    නම් (qi == NULL)
	ආපසු FAIL;

    ආපසු qf_init_ext(qi, qi->qf_curlist, efile, curbuf, NULL, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    static void
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    නම් (title == NULL)
	ආපසු;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    නම් (p != NULL)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    static char_u *
qf_cmdtitle(char_u *cmd)
{
    static char_u qftitle_str[IOSIZE];

    vim_snprintf((var_na *)qftitle_str, IOSIZE, ":%s", (var_na *)cmd);
    ආපසු qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    static qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    ආපසු qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    static void
qf_pop_stack(qf_info_T *qi, පූර්ණ_සංඛ්‍යා adjust)
{
    පූර්ණ_සංඛ්‍යා i;
    qf_free(&qi->qf_lists[0]);
    සඳහා (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, parimaa_na(*qi->qf_lists));

    නම් (adjust)
    {
	qi->qf_listcount--;
	නම් (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	නැතහොත්
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    static void
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    අතරතුර (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    නම් (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_pop_stack(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    නැතහොත්
	qi->qf_curlist = qi->qf_listcount++;

    qfl = qf_get_curlist(qi);
    CLEAR_POINTER(qfl);
    qf_store_title(qfl, qf_title);
    qfl->qfl_type = qi->qfl_type;
    qfl->qf_id = ++last_qf_id;
    qfl->qf_has_user_data = FALSE;
}

/*
 * Queue location list stack delete request.
 */
    static void
locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    නම් (q == NULL)
	ආපසු;

    q->qi = qi;
    q->next = qf_delq_head;
    qf_delq_head = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    පූර්ණ_සංඛ්‍යා
qf_stack_get_bufnr(void)
{
    නම් (ql_info == NULL)
	ආපසු INVALID_QFBUFNR;
    ආපසු ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    static void
wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    නම් (qi->qf_bufnr == INVALID_QFBUFNR)
	ආපසු;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    නම් (qfbuf != NULL && qfbuf->b_nwindows == 0)
    {
	පූර්ණ_සංඛ්‍යා buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	නම් (curwin->w_buffer == NULL)
	{
	    curwin->w_buffer = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_buffer(NULL, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	නම් (buf_was_null)
	    curwin->w_buffer = NULL;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    static void
qf_free_list_stack_items(qf_info_T *qi)
{
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    static void
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_stack_items(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    static void
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    නම් (qi == NULL)
	ආපසු;
    *pqi = NULL;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    නම් (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	ආපසු;
    }

    qi->qf_refcount--;
    නම් (qi->qf_refcount < 1)
    {
	// No references to this location list.
	// If the quickfix window buffer is loaded, then wipe it
	wipe_qf_buffer(qi);

	qf_free_lists(qi);
    }
}

/*
 * Free all the quickfix/location lists in the stack.
 */
    void
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    නම් (wp != NULL)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    නැතහොත් නම් (qi != NULL)
	qf_free_list_stack_items(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    static void
incr_quickfix_busy(void)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    static void
decr_quickfix_busy(void)
{
    නම් (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	අතරතුර (qf_delq_head != NULL)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    නම් (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# නම් defined(EXITFREE)
    void
check_quickfix_busy(void)
{
    නම් (quickfix_busy != 0)
    {
	semsg("quickfix_busy not zero on exit: %ld", (long)quickfix_busy);
#  ifdef ABORT_ON_INTERNAL_ERROR
	abort();
#  endif
    }
}
# endif

/*
 * Add an entry to the end of the list of errors.
 * Returns QF_OK on success or QF_FAIL on a memory allocation failure.
 */
    static පූර්ණ_සංඛ්‍යා
qf_add_entry(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    පූර්ණ_සංඛ්‍යා		bufnum,		// buffer number or zero
    char_u	*mesg,		// message
    long	lnum,		// line number
    long	end_lnum,	// line number for end
    පූර්ණ_සංඛ්‍යා		col,		// column
    පූර්ණ_සංඛ්‍යා		end_col,	// column for end
    පූර්ණ_සංඛ්‍යා		vis_col,	// using visual column
    char_u	*pattern,	// search pattern
    පූර්ණ_සංඛ්‍යා		nr,		// error number
    පූර්ණ_සංඛ්‍යා		type,		// type character
    typval_T	*user_data,     // custom user data or NULL
    පූර්ණ_සංඛ්‍යා		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = NULL;
    char_u	*p = NULL;

    නම් ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == NULL)
	ආපසු QF_FAIL;
    නම් (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	නම් (buf != NULL)
	    buf->b_has_qf_entry |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    }
    නැතහොත්
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    නම් (fname != NULL)
	fullname = fix_fname(fname);
    qfp->qf_fname = NULL;
    නම් (buf != NULL &&
	buf->b_ffname != NULL && fullname != NULL)
    {
	නම් (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    නම් (p != NULL)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    නම් ((qfp->qf_text = vim_strsave(mesg)) == NULL)
    {
	vim_free(qfp);
	ආපසු QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_end_lnum = end_lnum;
    qfp->qf_col = col;
    qfp->qf_end_col = end_col;
    qfp->qf_viscol = vis_col;
    නම් (user_data == NULL || user_data->v_type == VAR_UNKNOWN)
	qfp->qf_user_data.v_type = VAR_UNKNOWN;
    නැතහොත්
    {
	copy_tv(user_data, &qfp->qf_user_data);
	qfl->qf_has_user_data = TRUE;
    }
    නම් (pattern == NULL || *pattern == NUL)
	qfp->qf_pattern = NULL;
    නැතහොත් නම් ((qfp->qf_pattern = vim_strsave(pattern)) == NULL)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp);
	ආපසු QF_FAIL;
    }
    නම් (module == NULL || *module == NUL)
	qfp->qf_module = NULL;
    නැතහොත් නම් ((qfp->qf_module = vim_strsave(module)) == NULL)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp->qf_pattern);
	vim_free(qfp);
	ආපසු QF_FAIL;
    }
    qfp->qf_nr = nr;
    නම් (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_last;
    නම් (qf_list_empty(qfl))		// first element in the list
    {
	qfl->qf_start = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_index = 0;
	qfp->qf_prev = NULL;
    }
    නැතහොත්
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_next = qfp;
    }
    qfp->qf_next = NULL;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_count;
    නම් (qfl->qf_index == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_index = qfl->qf_count;
	qfl->qf_ptr = qfp;
    }

    ආපසු QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    පූර්ණ_සංඛ්‍යා
qf_resize_stack(පූර්ණ_සංඛ්‍යා n)
{
    නම් (ql_info == NULL)
	ආපසු FAIL;

    නම් (qf_resize_stack_base(ql_info, n) == FAIL)
	ආපසු FAIL;

    ආපසු OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    පූර්ණ_සංඛ්‍යා
ll_resize_stack(win_T *wp, පූර්ණ_සංඛ්‍යා n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    නම් (IS_LL_WINDOW(wp))
	qf_sync_llw_to_win(wp);
    නැතහොත්
	qf_sync_win_to_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    නම් (qi == NULL)
	ආපසු FAIL;

    නම් (qf_resize_stack_base(qi, n) == FAIL)
	ආපසු FAIL;

    ආපසු OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    static පූර්ණ_සංඛ්‍යා
qf_resize_stack_base(qf_info_T *qi, පූර්ණ_සංඛ්‍යා n)
{
    qf_list_T *new;
    පූර්ණ_සංඛ්‍යා amount_to_rm = 0, i;

    නම් (qi == NULL)
	ආපසු FAIL;

    size_t lsz = parimaa_na(*qi->qf_lists);

    නම් (n == qi->qf_maxcount)
	ආපසු OK;
    නැතහොත් නම් (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_to_rm = qi->qf_listcount - n;

	සඳහා (i = 0; i < amount_to_rm; i++)
	    qf_pop_stack(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    නම් (new == NULL)
	ආපසු FAIL;

    // fill with zeroes any newly allocated memory
    නම් (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_buffer(qi, NULL);

    ආපසු OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   void
qf_init_stack(void)
{
    ql_info = qf_alloc_stack(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    static void
qf_sync_llw_to_win(win_T *llw)
{
    win_T *wp = qf_find_win_with_loclist(llw->w_llist_ref);

    නම් (wp != NULL)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    static void
qf_sync_win_to_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    නම් (llw != NULL)
	FOR_ALL_WINDOWS(wp)
	    නම් (wp->w_llist_ref == llw && bt_quickfix(wp->w_buffer))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		ආපසු;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    static qf_info_T *
qf_alloc_stack(qfltype_T qfltype, පූර්ණ_සංඛ්‍යා n)
{
    qf_info_T *qi;

    නම් (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    නැතහොත්
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	නම් (qi == NULL)
	    ආපසු NULL;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_stack(n);
    නම් (qi->qf_lists == NULL)
    {
	නම් (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	ආපසු NULL;
    }
    qi->qf_maxcount = n;

    ආපසු qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    static qf_list_T *
qf_alloc_list_stack(පූර්ණ_සංඛ්‍යා n)
{
    ආපසු ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    static qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    නම් (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	ආපසු wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    නම් (wp->w_llist == NULL)
	// new location list
	wp->w_llist = qf_alloc_stack(QFLT_LOCATION, wp->w_p_lhi);

    ආපසු wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    static qf_info_T *
qf_cmd_get_stack(exarg_T *eap, පූර්ණ_සංඛ්‍යා print_emsg)
{
    qf_info_T	*qi = ql_info;

    නම් (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	නම් (qi == NULL)
	{
	    නම් (print_emsg)
		emsg(_(e_no_location_list));
	    ආපසු NULL;
	}
    }
    නම් (qi == NULL && print_emsg)
	emsg(_(e_no_quickfix_stack));

    ආපසු qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    static qf_info_T *
qf_cmd_get_or_alloc_stack(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    නම් (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	නම් (qi == NULL)
	    ආපසු NULL;
	*pwinp = curwin;
    }

    ආපසු qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    static පූර්ණ_සංඛ්‍යා
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    පූර්ණ_සංඛ්‍යා		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_ITEMS(from_qfl, from_qfp, i)
    {
	නම් (qf_add_entry(to_qfl,
		    NULL,
		    NULL,
		    from_qfp->qf_module,
		    0,
		    from_qfp->qf_text,
		    from_qfp->qf_lnum,
		    from_qfp->qf_end_lnum,
		    from_qfp->qf_col,
		    from_qfp->qf_end_col,
		    from_qfp->qf_viscol,
		    from_qfp->qf_pattern,
		    from_qfp->qf_nr,
		    0,
		    &from_qfp->qf_user_data,
		    from_qfp->qf_valid) == QF_FAIL)
	    ආපසු FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = to_qfl->qf_last;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	නම් (from_qfl->qf_ptr == from_qfp)
	    to_qfl->qf_ptr = prevp;		// current location
    }

    ආපසු OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    static පූර්ණ_සංඛ්‍යා
copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_has_user_data = from_qfl->qf_has_user_data;
    to_qfl->qf_count = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = NULL;
    to_qfl->qf_last = NULL;
    to_qfl->qf_ptr = NULL;
    නම් (from_qfl->qf_title != NULL)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    නැතහොත්
	to_qfl->qf_title = NULL;
    නම් (from_qfl->qf_ctx != NULL)
    {
	to_qfl->qf_ctx = alloc_tv();
	නම් (to_qfl->qf_ctx != NULL)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    නැතහොත්
	to_qfl->qf_ctx = NULL;
    නම් (from_qfl->qf_qftf_cb.cb_name != NULL)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    නැතහොත්
	to_qfl->qf_qftf_cb.cb_name = NULL;

    නම් (from_qfl->qf_count)
	නම් (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    ආපසු FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    නම් (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    ආපසු OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    void
copy_loclist_stack(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    පූර්ණ_සංඛ්‍යා		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    නම් (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    නැතහොත්
	qi = from->w_llist;

    නම් (qi == NULL)		    // no location list to copy
	ආපසු;

    // allocate a new location list, set size of stack to 'from' window value
    නම් ((to->w_llist = qf_alloc_stack(QFLT_LOCATION, from->w_p_lhi)) == NULL)
	ආපසු;
    නැතහොත්
	// set 'to' lhi to reflect new value
	to->w_p_lhi = to->w_llist->qf_maxcount;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    සඳහා (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	නම් (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    ආපසු;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    static පූර්ණ_සංඛ්‍යා
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {NULL, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    නම් (fname == NULL || *fname == NUL)		// no file name
	ආපසු 0;

# ifdef VMS
    vms_remove_version(fname);
# endif
# ifdef BACKSLASH_IN_FILENAME
    නම් (directory != NULL)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    නම් (directory != NULL && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != NULL)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	නම් (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    නම් (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    නැතහොත්
	    {
		ptr.length = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    නම් (ptr.string == NULL)
		ආපසු 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.length = ptr.length;
    }
    නැතහොත්
    {
	bufname.string = fname;
	bufname.length = fname_len;
    }

    නම් (qf_last_bufname != NULL && STRCMP(bufname.string, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr.string);
    }
    නැතහොත්
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname.string, NULL, (linenr_T)0, BLN_NOOPT);
	නම් (bufname.string == ptr.string)
	    qf_last_bufname = bufname.string;
	නැතහොත්
	    qf_last_bufname = vim_strnsave(bufname.string, bufname.length);
	set_bufref(&qf_last_bufref, buf);
    }
    නම් (buf == NULL)
	ආපසු 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    ආපසු buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    static char_u *
qf_push_dir(char_u *dirbuf, sa_nracanaa dir_stack_T **stackptr, පූර්ණ_සංඛ්‍යා is_file_stack)
{
    sa_nracanaa dir_stack_T  *ds_new;
    sa_nracanaa dir_stack_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(sa_nracanaa dir_stack_T, aid_qf_dirstack);
    නම් (ds_new == NULL)
	ආපසු NULL;

    // push the new element onto the stack
    ds_new->next = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    නම් (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == NULL
	    || is_file_stack)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    නැතහොත්
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->next;
	(*stackptr)->dirname = NULL;
	අතරතුර (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    නම් (dirname.string == NULL)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->next;
		vim_free(ds_ptr);

		ආපසු NULL;
	    }
	    නම් (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		bha_nga;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->next;
	}

	// clean up all dirs we already left
	අතරතුර ((*stackptr)->next != ds_new)
	{
	    ds_ptr = (*stackptr)->next;
	    (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	නම් (ds_new == NULL)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    නම් ((*stackptr)->dirname != NULL)
	ආපසු (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->next;
    vim_free(ds_ptr);

    ආපසු NULL;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    static char_u *
qf_pop_dir(sa_nracanaa dir_stack_T **stackptr)
{
    sa_nracanaa dir_stack_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    නම් (*stackptr != NULL)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    ආපසු *stackptr ? (*stackptr)->dirname : NULL;
}

/*
 * clean up directory stack
 */
    static void
qf_clean_dir_stack(sa_nracanaa dir_stack_T **stackptr)
{
    sa_nracanaa dir_stack_T  *ds_ptr;

    අතරතුර ((ds_ptr = *stackptr) != NULL)
    {
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }
}

/*
 * Check in which directory of the directory stack the given file can be
 * found.
 * Returns a pointer to the directory name or NULL if not found.
 * Cleans up intermediate directory entries.
 *
 * TODO: How to solve the following problem?
 * If we have this directory tree:
 *     ./
 *     ./aa
 *     ./aa/bb
 *     ./bb
 *     ./bb/x.c
 * and make says:
 *     making all in aa
 *     making all in bb
 *     x.c:9: Error
 * Then qf_push_dir thinks we are in ./aa/bb, but we are in ./bb.
 * qf_guess_filepath will return NULL.
 */
    static char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    sa_nracanaa dir_stack_T     *ds_ptr;
    sa_nracanaa dir_stack_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    නම් (qfl->qf_dir_stack == NULL)
	ආපසු NULL;

    ds_ptr = qfl->qf_dir_stack->next;
    fullname.string = NULL;
    fullname.length = 0;
    filename_len = STRLEN(filename);
    අතරතුර (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	නම් ((fullname.string != NULL) && (mch_getperm(fullname.string) >= 0))
	    bha_nga;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    අතරතුර (qfl->qf_dir_stack->next != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_stack->next;
	qfl->qf_dir_stack->next = qfl->qf_dir_stack->next->next;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    ආපසු ds_ptr == NULL ? NULL : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    static පූර්ණ_සංඛ්‍යා
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    පූර්ණ_සංඛ්‍යා		i;

    නම් (wp != NULL)
    {
	නම් (!win_valid(wp))
	    ආපසු FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    නම් (qi == NULL)
	ආපසු FALSE;

    සඳහා (i = 0; i < qi->qf_listcount; ++i)
	නම් (qi->qf_lists[i].qf_id == qf_id)
	    ආපසු TRUE;

    ආපසු FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    static පූර්ණ_සංඛ්‍යා
is_qf_entry_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    පූර්ණ_සංඛ්‍යා		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
	නම් (qfp == qf_ptr)
	    bha_nga;

    නම් (i > qfl->qf_count) // Entry is not found
	ආපසු FALSE;

    ආපසු TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    static qfline_T *
get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	පූර්ණ_සංඛ්‍යා		*qf_index,
	පූර්ණ_සංඛ්‍යා		dir)
{
    පූර්ණ_සංඛ්‍යා			idx;
    පූර්ණ_සංඛ්‍යා			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    do
    {
	නම් (idx == qfl->qf_count || qf_ptr->qf_next == NULL)
	    ආපසු NULL;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } අතරතුර ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    ආපසු qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    static qfline_T *
get_prev_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	පූර්ණ_සංඛ්‍යා		*qf_index,
	පූර්ණ_සංඛ්‍යා		dir)
{
    පූර්ණ_සංඛ්‍යා			idx;
    පූර්ණ_සංඛ්‍යා			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    do
    {
	නම් (idx == 1 || qf_ptr->qf_prev == NULL)
	    ආපසු NULL;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } අතරතුර ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    ආපසු qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    static qfline_T *
get_nth_valid_entry(
	qf_list_T	*qfl,
	පූර්ණ_සංඛ්‍යා		errornr,
	පූර්ණ_සංඛ්‍යා		dir,
	පූර්ණ_සංඛ්‍යා		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    පූර්ණ_සංඛ්‍යා			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    පූර්ණ_සංඛ්‍යා			prev_index;
    var_na		*err = e_no_more_items;

    අතරතුර (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	නම් (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	නැතහොත්
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	නම් (qf_ptr == NULL)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    නම් (err != NULL)
	    {
		emsg(_(err));
		ආපසු NULL;
	    }
	    bha_nga;
	}

	err = NULL;
    }

    *new_qfidx = qf_idx;
    ආපසු qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    static qfline_T *
get_nth_entry(qf_list_T *qfl, පූර්ණ_සංඛ්‍යා errornr, පූර්ණ_සංඛ්‍යා *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    පූර්ණ_සංඛ්‍යා		qf_idx = qfl->qf_index;

    // New error number is less than the current error number
    අතරතුර (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != NULL)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    අතරතුර (errornr > qf_idx && qf_idx < qfl->qf_count &&
						qf_ptr->qf_next != NULL)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_next;
    }

    *new_qfidx = qf_idx;
    ආපසු qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    static qfline_T *
qf_get_entry(
	qf_list_T	*qfl,
	පූර්ණ_සංඛ්‍යා		errornr,
	පූර්ණ_සංඛ්‍යා		dir,
	පූර්ණ_සංඛ්‍යා		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    පූර්ණ_සංඛ්‍යා		qfidx = qfl->qf_index;

    නම් (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_entry(qfl, errornr, dir, &qfidx);
    නැතහොත් නම් (errornr != 0)	// go to specified number
	qf_ptr = get_nth_entry(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    ආපසු qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    static win_T *
qf_find_help_win(void)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	නම් (bt_help(wp->w_buffer))
	    ආපසු wp;

    ආපසු NULL;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    static void
win_set_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    static පූර්ණ_සංඛ්‍යා
jump_to_help_window(qf_info_T *qi, පූර්ණ_සංඛ්‍යා newwin, පූර්ණ_සංඛ්‍යා *opened_window)
{
    win_T	*wp;
    පූර්ණ_සංඛ්‍යා		flags;

    නම් (cmdmod.cmod_tab != 0 || newwin)
	wp = NULL;
    නැතහොත්
	wp = qf_find_help_win();
    නම් (wp != NULL && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    නැතහොත්
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	නම් (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_TOP;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	නම් (IS_LL_STACK(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	නම් (win_split(0, flags) == FAIL)
	    ආපසු FAIL;

	*opened_window = TRUE;

	නම් (curwin->w_height < p_hh)
	    win_setheight((පූර්ණ_සංඛ්‍යා)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	නම් (IS_LL_STACK(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    නම් (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    ආපසු OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    static win_T *
qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	නම් (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    ආපසු wp;

    ආපසු NULL;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    static win_T *
qf_find_win_with_normal_buf(void)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	නම් (bt_normal(wp->w_buffer))
	    ආපසු wp;

    ආපසු NULL;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    static පූර්ණ_සංඛ්‍යා
qf_goto_tabwin_with_file(පූර්ණ_සංඛ්‍යා fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	නම් (wp->w_buffer->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    ආපසු TRUE;
	}

    ආපසු FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    static පූර්ණ_සංඛ්‍යා
qf_open_new_file_win(qf_info_T *ll_ref)
{
    පූර්ණ_සංඛ්‍යා		flags;

    flags = WSP_ABOVE;
    නම් (ll_ref != NULL)
	flags |= WSP_NEWLOC;
    නම් (win_split(0, flags) == FAIL)
	ආපසු FAIL;		// not enough room for window
    p_swb = empty_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    නම් (ll_ref != NULL)
	// The new window should use the location list from the
	// location list window
	win_set_loclist(curwin, ll_ref);
    ආපසු OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    static void
qf_goto_win_with_ll_file(win_T *use_win, පූර්ණ_සංඛ්‍යා qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    නම් (win == NULL)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    නම් (win->w_buffer->b_fnum == qf_fnum)
		bha_nga;
	නම් (win == NULL)
	{
	    // Find a previous usable window
	    win = curwin;
	    do
	    {
		නම් (bt_normal(win->w_buffer))
		    bha_nga;
		නම් (win->w_prev == NULL)
		    win = lastwin;	// wrap around the top
		නැතහොත්
		    win = win->w_prev; // go to previous window
	    } අතරතුර (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    නම් (win->w_llist == NULL && ll_ref != NULL)
	win_set_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    static void
qf_goto_win_with_qfl_file(පූර්ණ_සංඛ්‍යා qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = NULL;
    සඳහා (;;)
    {
	නම් (win->w_buffer->b_fnum == qf_fnum)
	    bha_nga;
	නම් (win->w_prev == NULL)
	    win = lastwin;	// wrap around the top
	නැතහොත්
	    win = win->w_prev;	// go to previous window

	නම් (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    නම් ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    නැතහොත් නම් (altwin != NULL)
		win = altwin;
	    නැතහොත් නම් (curwin->w_prev != NULL)
		win = curwin->w_prev;
	    නැතහොත්
		win = curwin->w_next;
	    bha_nga;
	}

	// Remember a usable window.
	නම් (altwin == NULL && !win->w_p_pvw && !win->w_p_wfb &&
		bt_normal(win->w_buffer))
	    altwin = win;
    }

    win_goto(win);
}

/*
 * Find a suitable window for opening a file (qf_fnum) from the
 * quickfix/location list and jump to it.  If the file is already opened in a
 * window, jump to it. Otherwise open a new window to display the file. If
 * 'newwin' is TRUE, then always open a new window. This is called from either
 * a quickfix or a location list window.
 */
    static පූර්ණ_සංඛ්‍යා
qf_jump_to_usable_window(පූර්ණ_සංඛ්‍යා qf_fnum, පූර්ණ_සංඛ්‍යා newwin, පූර්ණ_සංඛ්‍යා *opened_window)
{
    win_T	*usable_wp = NULL;
    පූර්ණ_සංඛ්‍යා		usable_win = FALSE;
    qf_info_T	*ll_ref = NULL;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    නම් (!newwin)
	ll_ref = curwin->w_llist_ref;

    නම් (ll_ref != NULL)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	නම් (usable_wp != NULL)
	    usable_win = TRUE;
    }

    නම් (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	නම් (win != NULL)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    නම් (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    නම් ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	නම් (qf_open_new_file_win(ll_ref) != OK)
	    ආපසු FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    නැතහොත්
    {
	නම් (curwin->w_llist_ref != NULL)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	නැතහොත්					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    ආපසු OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    static පූර්ණ_සංඛ්‍යා
qf_jump_edit_buffer(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	පූර්ණ_සංඛ්‍යා		forceit,
	පූර්ණ_සංඛ්‍යා		prev_winid,
	පූර්ණ_සංඛ්‍යා		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    පූර්ණ_සංඛ්‍යා		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    පූර්ණ_සංඛ්‍යා		retval = OK;
    පූර්ණ_සංඛ්‍යා		old_qf_curlist = qi->qf_curlist;
    පූර්ණ_සංඛ්‍යා		save_qfid = qfl->qf_id;

    නම් (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	නම් (!can_abandon(curbuf, forceit))
	{
	    no_write_message();
	    ආපසු FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, NULL, NULL, NULL, (linenr_T)1,
		ECMD_HIDE + ECMD_SET_HELP,
		prev_winid == curwin->w_id ? curwin : NULL);
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා	fnum = qf_ptr->qf_fnum;

	නම් (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    නම් (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		ආපසු FAIL;
	    }

	    නම් (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_buffer))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    නම් (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		නම් (win_split(0, 0) == OK)
		    *opened_window = TRUE;

		නම් (curwin->w_p_wfb)
		{
		    // Autocommands set 'winfixbuf' or sent us to another window
		    // with it set, or we failed to split the window.  Give up,
		    // but don't return immediately, as they may have messed
		    // with the list.
		    emsg(_(e_winfixbuf_cannot_go_to_buffer));
		    retval = FAIL;
		}
	    }
	}

	නම් (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    නම් (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	නම් (wp == NULL && curwin->w_llist != qi)
	{
	    emsg(_(e_current_window_was_closed));
	    *opened_window = FALSE;
	    ආපසු QF_ABORT;
	}
    }

    නම් (qfl_type == QFLT_QUICKFIX && !qflist_valid(NULL, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_changed));
	ආපසු QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    නම් (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	නම් (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	නැතහොත්
	    emsg(_(e_current_location_list_was_changed));
	ආපසු QF_ABORT;
    }

    ආපසු retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    static පූර්ණ_සංඛ්‍යා
qf_screen_col_to_idx(colnr_T vcol)
{
    char_u	*line = ml_get_curline();
    char_u	*p = line;
    colnr_T	col = 0;

    අතරතුර (*p != NUL && col < vcol)
    {
	නම් (*p == TAB)
	    col += 8 - (col % 8);
	නැතහොත්
	    col += ptr2cells(p);
	නම් (col > vcol)
	    bha_nga;
	MB_PTR_ADV(p);
    }
    ආපසු (පූර්ණ_සංඛ්‍යා)(p - line);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    static void
qf_jump_goto_line(
	linenr_T	qf_lnum,
	පූර්ණ_සංඛ්‍යා		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    නම් (qf_pattern == NULL)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	නම් (i > 0)
	{
	    නම් (i > curbuf->b_ml.ml_line_count)
		i = curbuf->b_ml.ml_line_count;
	    curwin->w_cursor.lnum = i;
	}
	නම් (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    නම් (qf_viscol == TRUE)
		curwin->w_cursor.col = qf_screen_col_to_idx(qf_col - 1);
	    නැතහොත්
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = true;
	    check_cursor();
	}
	නැතහොත්
	    beginline(BL_WHITE | BL_FIX);
    }
    නැතහොත්
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	නම් (!do_search(NULL, '/', '/', qf_pattern, STRLEN(qf_pattern), (long)1, SEARCH_KEEP, NULL))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    static void
qf_jump_print_msg(
	qf_info_T	*qi,
	පූර්ණ_සංඛ්‍යා		qf_index,
	qfline_T	*qf_ptr,
	buf_T		*old_curbuf,
	linenr_T	old_lnum)
{
    linenr_T		i;
    garray_T		*gap;
    size_t		IObufflen;

    gap = qfga_get();

    // Update the screen before showing the message, unless the screen
    // scrolled up.
    නම් (!msg_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_count,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (var_na *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_text(gap, skipwhite(qf_ptr->qf_text));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = msg_scroll;
    නම් (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	msg_scroll = TRUE;
    නැතහොත් නම් (!msg_scrolled && shortmess(SHM_OVERALL))
	msg_scroll = FALSE;
    msg_attr_keep((var_na *)gap->ga_data, 0, TRUE);
    msg_scroll = i;

    qfga_clear();
}

/*
 * Find a usable window for opening a file from the quickfix/location list. If
 * a window is not found then open a new window. If 'newwin' is TRUE, then open
 * a new window.
 * Returns OK if successfully jumped or opened a window. Returns FAIL if not
 * able to jump/open a window.  Returns NOTDONE if a file is not associated
 * with the entry.  Returns QF_ABORT if the quickfix/location list was modified
 * by an autocmd.
 */
    static පූර්ණ_සංඛ්‍යා
qf_jump_open_window(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	පූර්ණ_සංඛ්‍යා		newwin,
	පූර්ණ_සංඛ්‍යා		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    පූර්ණ_සංඛ්‍යා		old_changedtick = qfl->qf_changedtick;
    පූර්ණ_සංඛ්‍යා		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    නම් (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_buffer)
						      || cmdmod.cmod_tab != 0))
	නම් (jump_to_help_window(qi, newwin, opened_window) == FAIL)
	    ආපසු FAIL;
    නම් (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	නම් (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	නැතහොත්
	    emsg(_(e_current_location_list_was_changed));
	ආපසු QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    නම් (bt_quickfix(curbuf) && !*opened_window)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	නම් (qf_ptr->qf_fnum == 0)
	    ආපසු NOTDONE;

	නම් (qf_jump_to_usable_window(qf_ptr->qf_fnum, newwin,
						opened_window) == FAIL)
	    ආපසු FAIL;
    }
    නම් (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	නම් (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	නැතහොත්
	    emsg(_(e_current_location_list_was_changed));
	ආපසු QF_ABORT;
    }

    ආපසු OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    static පූර්ණ_සංඛ්‍යා
qf_jump_to_buffer(
	qf_info_T	*qi,
	පූර්ණ_සංඛ්‍යා		qf_index,
	qfline_T	*qf_ptr,
	පූර්ණ_සංඛ්‍යා		forceit,
	පූර්ණ_සංඛ්‍යා		prev_winid,
	පූර්ණ_සංඛ්‍යා		*opened_window,
	පූර්ණ_සංඛ්‍යා		openfold,
	පූර්ණ_සංඛ්‍යා		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    පූර්ණ_සංඛ්‍යා		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    නම් (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	නම් (retval != OK)
	    ආපසු retval;
    }

    // When not switched to another buffer, still need to set pc mark
    නම් (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_line(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

# ifdef FEAT_FOLDING
    නම් ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    නම් (print_message)
	qf_jump_print_msg(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    ආපසු retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    void
qf_jump(qf_info_T	*qi,
	පූර්ණ_සංඛ්‍යා		dir,
	පූර්ණ_සංඛ්‍යා		errornr,
	පූර්ණ_සංඛ්‍යා		forceit)
{
    qf_jump_newwin(qi, dir, errornr, forceit, FALSE);
}

/*
 * Jump to a quickfix line.
 * If dir == 0 go to entry "errornr".
 * If dir == FORWARD go "errornr" valid entries forward.
 * If dir == BACKWARD go "errornr" valid entries backward.
 * If dir == FORWARD_FILE go "errornr" valid entries files backward.
 * If dir == BACKWARD_FILE go "errornr" valid entries files backward
 * else if "errornr" is zero, redisplay the same line
 * If 'forceit' is TRUE, then can discard changes to the current buffer.
 * If 'newwin' is TRUE, then open the file in a new window.
 */
    static void
qf_jump_newwin(qf_info_T	*qi,
	පූර්ණ_සංඛ්‍යා		dir,
	පූර්ණ_සංඛ්‍යා		errornr,
	පූර්ණ_සංඛ්‍යා		forceit,
	පූර්ණ_සංඛ්‍යා		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    පූර්ණ_සංඛ්‍යා			qf_index;
    පූර්ණ_සංඛ්‍යා			old_qf_index;
    char_u		*old_swb = p_swb;
    unsigned		old_swb_flags = swb_flags;
    පූර්ණ_සංඛ්‍යා			prev_winid;
    පූර්ණ_සංඛ්‍යා			opened_window = FALSE;
    පූර්ණ_සංඛ්‍යා			print_message = TRUE;
    පූර්ණ_සංඛ්‍යා			old_KeyTyped = KeyTyped; // getting file may reset it
    පූර්ණ_සංඛ්‍යා			retval = OK;

    නම් (qi == NULL)
    {
	නම් (ql_info == NULL)
	{
	    emsg(_(e_no_quickfix_stack));
	    ආපසු;
	}
	qi = ql_info;
    }

    නම් (qf_stack_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ආපසු;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qfl->qf_index;
    old_qf_index = qf_index;

    qf_ptr = qf_get_entry(qfl, errornr, dir, &qf_index);
    නම් (qf_ptr == NULL)
    {
	qf_ptr = old_qf_ptr;
	qf_index = old_qf_index;
	goto theend;
    }

    qfl->qf_index = qf_index;
    qfl->qf_ptr = qf_ptr;
    නම් (qf_win_pos_update(qi, old_qf_index))
	// No need to print the error message if it's visible in the error
	// window
	print_message = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_window(qi, qf_ptr, newwin, &opened_window);
    නම් (retval == FAIL)
	goto failed;
    නම් (retval == QF_ABORT)
    {
	qi = NULL;
	qf_ptr = NULL;
	goto theend;
    }
    නම් (retval == NOTDONE)
	goto theend;

    retval = qf_jump_to_buffer(qi, qf_index, qf_ptr, forceit, prev_winid,
				  &opened_window, old_KeyTyped, print_message);
    නම් (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = NULL;
	qf_ptr = NULL;
    }

    නම් (retval != OK)
    {
	නම් (opened_window)
	    win_close(curwin, TRUE);    // Close opened window
	නම් (qf_ptr != NULL && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    නම් (qi != NULL)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_index = qf_index;
    }
    නම් (p_swb != old_swb && p_swb == empty_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
static පූර්ණ_සංඛ්‍යා	qfFileAttr;
static පූර්ණ_සංඛ්‍යා	qfSepAttr;
static පූර්ණ_සංඛ්‍යා	qfLineAttr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    static void
qf_list_entry(qfline_T *qfp, පූර්ණ_සංඛ්‍යා qf_idx, පූර්ණ_සංඛ්‍යා cursel)
{
    char_u	*fname;
    buf_T	*buf;
    පූර්ණ_සංඛ්‍යා		filter_entry;
    garray_T	*gap;

    fname = NULL;
    නම් (qfp->qf_module != NULL && *qfp->qf_module != NUL)
	vim_snprintf((var_na *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(var_na *)qfp->qf_module);
    නැතහොත්
    {
	නම් (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != NULL)
	{
	    නම් (qfp->qf_fname == NULL)
		fname = buf->b_fname;
	    නැතහොත්
		fname = qfp->qf_fname;
	    නම් (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	නම් (fname == NULL)
	    sprintf((var_na *)IObuff, "%2d", qf_idx);
	නැතහොත්
	    vim_snprintf((var_na *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (var_na *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    නම් (qfp->qf_module != NULL && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    නම් (filter_entry && fname != NULL)
	filter_entry &= message_filtered(fname);
    නම් (filter_entry && qfp->qf_pattern != NULL)
	filter_entry &= message_filtered(qfp->qf_pattern);
    නම් (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    නම් (filter_entry)
	ආපසු;

    msg_putchar('\n');
    msg_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    නම් (qfp->qf_lnum != 0)
	msg_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    නම් (qfp->qf_lnum != 0)
	qf_range_text(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    msg_puts_attr((var_na *)gap->ga_data, qfLineAttr);
    msg_puts_attr(":", qfSepAttr);
    නම් (qfp->qf_pattern != NULL)
    {
	gap = qfga_get();
	qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, NUL);
	msg_puts((var_na *)gap->ga_data);
	msg_puts_attr(":", qfSepAttr);
    }
    msg_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_text(gap, (fname != NULL || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_text) : qfp->qf_text);
    ga_append(gap, NUL);
    msg_prt_line((char_u *)gap->ga_data, FALSE);
    out_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    void
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		idx1 = 1;
    පූර්ණ_සංඛ්‍යා		idx2 = -1;
    char_u	*arg = eap->arg;
    පූර්ණ_සංඛ්‍යා		plus = FALSE;
    පූර්ණ_සංඛ්‍යා		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    නම් ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	ආපසු;

    නම් (qf_stack_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ආපසු;
    }
    නම් (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    නම් (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	ආපසු;
    }
    qfl = qf_get_curlist(qi);
    නම් (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    නැතහොත්
    {
	i = qfl->qf_count;
	නම් (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	නම් (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    නම් (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    නම් (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qfLineAttr = syn_name2attr((char_u *)"qfLineNr");
    නම් (qfLineAttr == 0)
	qfLineAttr = HL_ATTR(HLF_N);

    නම් (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	නම් ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    static void
qf_fmt_text(garray_T *gap, char_u *text)
{
    char_u	*p = text;
    අතරතුර (*p != NUL)
    {
	නම් (*p == '\n')
	{
	    ga_append(gap, ' ');
	    අතරතුර (*++p != NUL)
		නම් (!VIM_ISWHITE(*p) && *p != '\n')
		    bha_nga;
	}
	නැතහොත්
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    static void
qf_range_text(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.length = vim_snprintf_safelen((var_na *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    නම් (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	buf.length += vim_snprintf_safelen((var_na *)buf.string + buf.length,
	    IOSIZE - buf.length, "-%ld", qfp->qf_end_lnum);
    }
    නම් (qfp->qf_col > 0)
    {
	buf.length += vim_snprintf_safelen((var_na *)buf.string + buf.length,
	    IOSIZE - buf.length, " col %d", qfp->qf_col);
	නම් (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	    buf.length += vim_snprintf_safelen((var_na *)buf.string + buf.length,
		IOSIZE - buf.length, "-%d", qfp->qf_end_col);
    }

    ga_concat_len(gap, buf.string, buf.length);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    static void
qf_msg(qf_info_T *qi, පූර්ණ_සංඛ්‍යා which, var_na *lead)
{
    var_na   *title = (var_na *)qi->qf_lists[which].qf_title;
    පූර්ණ_සංඛ්‍යා    count = qi->qf_lists[which].qf_count;
    char_u buf[IOSIZE];

    vim_snprintf((var_na *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    count);

    නම් (title != NULL)
    {
	size_t	len = STRLEN(buf);

	නම් (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    msg((var_na *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    void
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    පූර්ණ_සංඛ්‍යා		count;

    නම් ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	ආපසු;

    නම් (eap->addr_count != 0)
	count = eap->line2;
    නැතහොත්
	count = 1;
    අතරතුර (count--)
    {
	නම් (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    නම් (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_stack));
		bha_nga;
	    }
	    --qi->qf_curlist;
	}
	නැතහොත්
	{
	    නම් (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_top_of_quickfix_stack));
		bha_nga;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_msg(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, NULL);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    void
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_stack(eap, FALSE);
    පූර්ණ_සංඛ්‍යා		i;

    නම් (eap->addr_count > 0)
    {
	නම් (qi == NULL)
	{
	    emsg(_(e_no_location_list));
	    ආපසු;
	}

	// Jump to the specified quickfix list
	නම් (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_msg(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, NULL);
	}
	නැතහොත්
	    emsg(_(e_invalid_range));

	ආපසු;
    }

    නම් (qf_stack_empty(qi))
	msg(_("No entries"));
    නැතහොත්
	සඳහා (i = 0; i < qi->qf_listcount; ++i)
	    qf_msg(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    static void
qf_free_items(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    පූර්ණ_සංඛ්‍යා		stop = FALSE;

    අතරතුර (qfl->qf_count && qfl->qf_start != NULL)
    {
	qfp = qfl->qf_start;
	qfpnext = qfp->qf_next;
	නම් (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_text);
	    vim_free(qfp->qf_pattern);
	    clear_tv(&qfp->qf_user_data);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    නම් (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_count = 1;
	    නැතහොත්
		qfl->qf_start = qfpnext;
	}
	--qfl->qf_count;
    }

    qfl->qf_index = 0;
    qfl->qf_start = NULL;
    qfl->qf_last = NULL;
    qfl->qf_ptr = NULL;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_stack(&qfl->qf_dir_stack);
    qfl->qf_directory = NULL;
    qf_clean_dir_stack(&qfl->qf_file_stack);
    qfl->qf_currfile = NULL;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    static void
qf_free(qf_list_T *qfl)
{
    qf_free_items(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = NULL;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   void
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	long	amount,
	long	amount_after)
{
    පූර්ණ_සංඛ්‍යා		i;
    qfline_T	*qfp;
    පූර්ණ_සංඛ්‍යා		idx;
    qf_info_T	*qi = ql_info;
    පූර්ණ_සංඛ්‍යා		found_one = FALSE;
    පූර්ණ_සංඛ්‍යා		buf_has_flag = wp == NULL ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;

    නම් (!(curbuf->b_has_qf_entry & buf_has_flag))
	ආපසු;
    නම් (wp != NULL)
    {
	නම් (wp->w_llist == NULL)
	    ආපසු;
	qi = wp->w_llist;
    }
    නැතහොත් නම් (qi == NULL)
	ආපසු;

    සඳහා (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	නම් (!qf_list_empty(qfl))
	    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
		නම් (qfp->qf_fnum == curbuf->b_fnum)
		{
		    found_one = TRUE;
		    නම් (qfp->qf_cleared)
			continue;

		    නම් (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			නම් (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			නැතහොත්
			    qfp->qf_lnum += amount;
		    }
		    නැතහොත් නම් (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    නම් (!found_one)
	curbuf->b_has_qf_entry &= ~buf_has_flag;
}

/*
 * Make a nice message out of the error character and the error number:
 *  char    number	message
 *  e or E    0		" error"
 *  w or W    0		" warning"
 *  i or I    0		" info"
 *  n or N    0		" note"
 *  0	      0		""
 *  other     0		" c"
 *  e or E    n		" error n"
 *  w or W    n		" warning n"
 *  i or I    n		" info n"
 *  n or N    n		" note n"
 *  0	      n		" error n"
 *  other     n		" c n"
 *  1	      x		""	:helpgrep
 */
    static char_u *
qf_types(පූර්ණ_සංඛ්‍යා c, පූර්ණ_සංඛ්‍යා nr)
{
    static char_u	buf[20];
    static char_u	cc[3];
    char_u		*p;

    නම් (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    නැතහොත් නම් (c == 'I' || c == 'i')
	p = (char_u *)" info";
    නැතහොත් නම් (c == 'N' || c == 'n')
	p = (char_u *)" note";
    නැතහොත් නම් (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    නැතහොත් නම් (c == 0 || c == 1)
	p = (char_u *)"";
    නැතහොත්
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    නම් (nr <= 0)
	ආපසු p;

    sprintf((var_na *)buf, "%s %3d", (var_na *)p, nr);
    ආපසු buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    void
qf_view_result(පූර්ණ_සංඛ්‍යා split)
{
    qf_info_T   *qi = ql_info;

    නම් (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);
    නැතහොත් නම් (qi == NULL)
    {
	emsg(_(e_no_quickfix_stack));
	ආපසු;
    }

    නම් (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ආපසු;
    }

    නම් (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (long)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	ආපසු;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    void
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    නම් ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	ආපසු;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    නම් (qf_stack_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	නම් (win != NULL)
	    ex_cclose(eap);
    }
    නැතහොත් නම් (win == NULL)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    void
ex_cclose(exarg_T *eap)
{
    win_T	*win = NULL;
    qf_info_T	*qi;

    නම් ((qi = qf_cmd_get_stack(eap, FALSE)) == NULL)
	ආපසු;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    නම් (win != NULL)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    static void
qf_set_title_var(qf_list_T *qfl)
{
    නම් (qfl->qf_title != NULL)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    static පූර්ණ_සංඛ්‍යා
qf_goto_cwindow(qf_info_T *qi, පූර්ණ_සංඛ්‍යා resize, පූර්ණ_සංඛ්‍යා sz, පූර්ණ_සංඛ්‍යා vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    නම් (win == NULL)
	ආපසු FAIL;

    win_goto(win);
    නම් (resize)
    {
	නම් (vertsplit)
	{
	    නම් (sz != win->w_width)
		win_setwidth(sz);
	}
	නැතහොත් නම් (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    ආපසු OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    static void
qf_set_cwindow_options(void)
{
    // switch off 'swapfile'
    set_option_value_give_err((char_u *)"swf", 0L, NULL, OPT_LOCAL);
    set_option_value_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_value_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    set_option_value_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    static පූර්ණ_සංඛ්‍යා
qf_open_new_cwindow(qf_info_T *qi, පූර්ණ_සංඛ්‍යා height)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    පූර්ණ_සංඛ්‍යා		flags = 0;
    win_T	*win;

    qf_buf = qf_find_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    නම් (IS_QF_STACK(qi) && cmdmod.cmod_split == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    නම් (cmdmod.cmod_split == 0)
	flags = WSP_BELOW;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    නම් (IS_QF_STACK(qi))
	flags |= WSP_QUICKFIX;

    නම් (win_split(height, flags) == FAIL)
	ආපසු FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    නම් (IS_LL_STACK(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    නම් (oldwin != curwin)
	oldwin = NULL;  // don't store info when in another window
    නම් (qf_buf != NULL)
    {
	// Use the existing quickfix buffer
	නම් (do_ecmd(qf_buf->b_fnum, NULL, NULL, NULL, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    ආපසු FAIL;
    }
    නැතහොත්
    {
	// Create a new quickfix buffer
	නම් (do_ecmd(0, NULL, NULL, NULL, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    ආපසු FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    නම් (!bt_quickfix(curbuf))
	qf_set_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    නම් (curtab == prevtab && curwin->w_width == Columns)
	win_setheight(height);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    නම් (win_valid(win))
	prevwin = win;

    ආපසු OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    void
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    පූර්ණ_සංඛ්‍යා		height;
    පූර්ණ_සංඛ්‍යා		status = FAIL;
    පූර්ණ_සංඛ්‍යා		lnum;

    නම් ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	ආපසු;

    incr_quickfix_busy();

    නම් (eap->addr_count != 0)
	height = eap->line2;
    නැතහොත්
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    නම් (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_count != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    නම් (status == FAIL)
	නම් (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    ආපසු;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, NULL, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    static void
qf_win_goto(win_T *win, linenr_T lnum)
{
    win_T	*old_curwin = curwin;

    curwin = win;
    curbuf = win->w_buffer;
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    update_topline();		// scroll to show the line
    redraw_later(UPD_VALID);
    curwin->w_redr_status = true;	// update ruler
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * :cbottom/:lbottom commands.
 */
    void
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    නම් ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	ආපසු;

    win = qf_find_win(qi);
    නම් (win != NULL && win->w_cursor.lnum != win->w_buffer->b_ml.ml_line_count)
	qf_win_goto(win, win->w_buffer->b_ml.ml_line_count);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_entry(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    නම් (IS_LL_WINDOW(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    නැතහොත් නම් (qi == NULL)
	ආපසු 0;

    ආපසු qf_get_curlist(qi)->qf_index;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    static පූර්ණ_සංඛ්‍යා
qf_win_pos_update(
    qf_info_T	*qi,
    පූර්ණ_සංඛ්‍යා		old_qf_index)	// previous qf_index or zero
{
    win_T	*win;
    පූර්ණ_සංඛ්‍යා		qf_index = qf_get_curlist(qi)->qf_index;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_find_win(qi);
    නම් (win != NULL
	    && qf_index <= win->w_buffer->b_ml.ml_line_count
	    && old_qf_index != qf_index)
    {
	නම් (qf_index > old_qf_index)
	{
	    win->w_redraw_top = old_qf_index;
	    win->w_redraw_bot = qf_index;
	}
	නැතහොත්
	{
	    win->w_redraw_top = qf_index;
	    win->w_redraw_bot = old_qf_index;
	}
	qf_win_goto(win, qf_index);
    }
    ආපසු win != NULL;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    static පූර්ණ_සංඛ්‍යා
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    නම් (buf_valid(win->w_buffer) && bt_quickfix(win->w_buffer))
	නම් ((IS_QF_STACK(qi) && win->w_llist_ref == NULL)
		|| (IS_LL_STACK(qi) && win->w_llist_ref == qi))
	    ආපසු TRUE;

    ආපසු FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    static win_T *
qf_find_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	නම් (is_qf_win(win, qi))
	    ආපසු win;
    ආපසු NULL;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    static buf_T *
qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    නම් (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	නම් (qfbuf != NULL)
	    ආපසු qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	නම් (is_qf_win(win, qi))
	    ආපසු win->w_buffer;

    ආපසු NULL;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    var_na *
did_set_quickfixtextfunc(optset_T *args UNUSED)
{
    නම් (option_set_callback_func(p_qftf, &qftf_cb) == FAIL)
	ආපසු e_invalid_argument;

    ආපසු NULL;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    static void
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	නම් (is_qf_win(win, qi))
	{
	    curwin = win;
	    qf_set_title_var(qfl);
	}
    }
    curwin = save_curwin;
}

/*
 * Find the quickfix buffer.  If it exists, update the contents.
 */
    static void
qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    නම් (buf == NULL)
	ආපසු;

    linenr_T	old_line_count = buf->b_ml.ml_line_count;
    පූර්ණ_සංඛ්‍යා		qf_winid = 0;

    නම් (IS_LL_STACK(qi))
    {
	නම් (curwin->w_llist == qi)
	    win = curwin;
	නැතහොත්
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_find_win_with_loclist(qi);
	    නම් (win == NULL)
		// File window is not found. Find the location list window.
		win = qf_find_win(qi);
	    නම් (win == NULL)
		ආපසු;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    පූර්ණ_සංඛ්‍යා do_fill = TRUE;
    නම් (old_last == NULL)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	නම් (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    නම් (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	නම් (old_last == NULL)
	{
	    (void)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    නම් ((win = qf_find_win(qi)) != NULL && old_line_count < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    static පූර්ණ_සංඛ්‍යා
qf_buf_add_line(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	පූර්ණ_සංඛ්‍යා		first_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    නම් (qftf_str != NULL && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    නැතහොත්
    {
	නම් (qfp->qf_module != NULL)
	    ga_concat(gap, qfp->qf_module);
	නැතහොත් නම් (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != NULL
		&& errbuf->b_fname != NULL)
	{
	    නම් (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    නැතහොත්
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		නම් (first_bufline && (errbuf->b_sfname == NULL
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    නම් (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		නම් (qfp->qf_fname == NULL)
		    ga_concat(gap, errbuf->b_fname);
		නැතහොත්
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	නම් (qfp->qf_lnum > 0)
	{
	    qf_range_text(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	නැතහොත් නම් (qfp->qf_pattern != NULL)
	    qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, '|');
	ga_append(gap, ' ');

	// Remove newlines and leading whitespace from the text.
	// For an unrecognized line keep the indent, the compiler may
	// mark a word with ^^^^.
	qf_fmt_text(gap, gap->ga_len > 3 ? skipwhite(qfp->qf_text)
							       : qfp->qf_text);
    }

    ga_append(gap, NUL);
    නම් (ml_append_buf(buf, lnum, gap->ga_data, gap->ga_len, FALSE) == FAIL)
	ආපසු FAIL;

    ආපසු OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    static list_T *
call_qftf_func(qf_list_T *qfl, පූර්ණ_සංඛ්‍යා qf_winid, long start_idx, long end_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = NULL;
    static පූර්ණ_සංඛ්‍යා	recursive = FALSE;

    නම් (recursive)
	ආපසු NULL;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    නම් (qfl->qf_qftf_cb.cb_name != NULL)
	cb = &qfl->qf_qftf_cb;
    නම් (cb->cb_name != NULL)
    {
	typval_T	args[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	නම් ((d = dict_alloc_lock(VAR_FIXED)) == NULL)
	{
	    recursive = FALSE;
	    ආපසු NULL;
	}
	dict_add_number(d, "quickfix", (long)IS_QF_LIST(qfl));
	dict_add_number(d, "winid", (long)qf_winid);
	dict_add_number(d, "id", (long)qfl->qf_id);
	dict_add_number(d, "start_idx", start_idx);
	dict_add_number(d, "end_idx", end_idx);
	++d->dv_refcount;
	args[0].v_type = VAR_DICT;
	args[0].vval.v_dict = d;

	qftf_list = NULL;
	textlock++;
	නම් (call_callback(cb, 0, &rettv, 1, args) != FAIL)
	{
	    නම් (rettv.v_type == VAR_LIST)
	    {
		qftf_list = rettv.vval.v_list;
		qftf_list->lv_refcount++;
	    }
	    clear_tv(&rettv);
	}
	textlock--;
	dict_unref(d);
    }

    recursive = FALSE;
    ආපසු qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    static void
qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, පූර්ණ_සංඛ්‍යා qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    පූර්ණ_සංඛ්‍යා		old_KeyTyped = KeyTyped;
    list_T	*qftf_list = NULL;
    listitem_T	*qftf_li = NULL;

    නම් (old_last == NULL)
    {
	win_T		*wp;
	tabpage_T	*tp;

	නම් (buf != curbuf)
	{
	    internal_error("qf_fill_buffer()");
	    ආපසු;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	අතරතුර ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	    (void)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    නම් (wp->w_buffer == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    නම් (qfl != NULL && qfl->qf_start != NULL)
    {
	char_u		dirname[MAXPATHL];
	පූර්ණ_සංඛ්‍යා		invalid_val = FALSE;
	පූර්ණ_සංඛ්‍යා		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	නම් (old_last == NULL)
	{
	    qfp = qfl->qf_start;
	    lnum = 0;
	}
	නැතහොත්
	{
	    නම් (old_last->qf_next != NULL)
		qfp = old_last->qf_next;
	    නැතහොත්
		qfp = old_last;
	    lnum = buf->b_ml.ml_line_count;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (long)(lnum + 1),
							(long)qfl->qf_count);
	නම් (qftf_list != NULL)
	    qftf_li = qftf_list->lv_first;

	අතරතුර (lnum < qfl->qf_count)
	{
	    char_u	*qftf_str = NULL;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    නම් (qftf_li != NULL && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		නම් (qftf_str == NULL)
		    invalid_val = TRUE;
	    }

	    නම් (qf_buf_add_line(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		bha_nga;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_next;
	    නම් (qfp == NULL)
		bha_nga;

	    නම් (qftf_li != NULL)
		qftf_li = qftf_li->li_next;
	}

	නම් (old_last == NULL)
	    // Delete the empty line which is now at the end
	    (void)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    නම් (old_last == NULL)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_lock;
	set_option_value_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", NULL,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", NULL,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_lock;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    KeyTyped = old_KeyTyped;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    static void
qf_list_changed(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    static පූර්ණ_සංඛ්‍යා
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    පූර්ණ_සංඛ්‍යා		qf_idx;

    සඳහා (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	නම් (qi->qf_lists[qf_idx].qf_id == qfid)
	    ආපසු qf_idx;
    ආපසු INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    static පූර්ණ_සංඛ්‍යා
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    පූර්ණ_සංඛ්‍යා curlist;

    නම් (qf_get_curlist(qi)->qf_id == save_qfid)
	ආපසු OK;

    curlist = qf_id2nr(qi, save_qfid);
    නම් (curlist < 0)
	// list is not present
	ආපසු FAIL;
    qi->qf_curlist = curlist;
    ආපසු OK;
}

/*
 * Jump to the first entry if there is one.
 */
    static void
qf_jump_first(qf_info_T *qi, int_u save_qfid, පූර්ණ_සංඛ්‍යා forceit)
{
    නම් (qf_restore_list(qi, save_qfid) == FAIL)
	ආපසු;


    නම් (!check_can_set_curbuf_forceit(forceit))
	ආපසු;


    // Autocommands might have cleared the list, check for that.
    නම් (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    පූර්ණ_සංඛ්‍යා
grep_internal(cmdidx_T cmdidx)
{
    ආපසු ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    static char_u *
make_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	pak_sa CMD_make:	    ආපසු (char_u *)"make";
	pak_sa CMD_lmake:	    ආපසු (char_u *)"lmake";
	pak_sa CMD_grep:	    ආපසු (char_u *)"grep";
	pak_sa CMD_lgrep:	    ආපසු (char_u *)"lgrep";
	pak_sa CMD_grepadd:   ආපසු (char_u *)"grepadd";
	pak_sa CMD_lgrepadd:  ආපසු (char_u *)"lgrepadd";
	default: ආපසු NULL;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    static char_u *
get_mef_name(void)
{
    char_u	*p;
    char_u	*name;
    static පූර්ණ_සංඛ්‍යා	start = -1;
    static පූර්ණ_සංඛ්‍යා	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    නම් (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	නම් (name == NULL)
	    emsg(_(e_cant_get_temp_file_name));
	ආපසු name;
    }

    සඳහා (p = p_mef; *p; ++p)
	නම් (p[0] == '#' && p[1] == '#')
	    bha_nga;

    නම් (*p == NUL)
	ආපසු vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    සඳහා (;;)
    {
	නම් (start == -1)
	    start = mch_get_pid();
	නැතහොත්
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	නම් (name == NULL)
	    bha_nga;
	STRCPY(name, p_mef);
	sprintf((var_na *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	නම් (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((var_na *)name, &sb) < 0
# endif
		)
	    bha_nga;
	vim_free(name);
    }
    ආපසු name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    static char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    unsigned	len;

    len = (unsigned)STRLEN(p_shq) * 2 + (unsigned)STRLEN(makecmd) + 1;
    නම් (*p_sp != NUL)
	len += (unsigned)STRLEN(p_sp) + (unsigned)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    නම් (cmd == NULL)
	ආපසු NULL;
    sprintf((var_na *)cmd, "%s%s%s", (var_na *)p_shq, (var_na *)makecmd,
							       (var_na *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    නම් (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    නම් (msg_col == 0)
	msg_didout = FALSE;
    msg_start();
    msg_puts(":!");
    msg_outtrans(cmd);		// show what we are doing

    ආපසු cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    void
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = NULL;
    win_T	*wp = NULL;
    qf_info_T	*qi = ql_info;
    පූර්ණ_සංඛ්‍යා		res;
    char_u	*au_name = NULL;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    පූර්ණ_සංඛ්‍යා		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    නම් (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	ආපසු;
    }

    au_name = make_get_auname(eap->cmdidx);
    නම් (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	නම් (aborting())
	    ආපසු;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    නම් (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    නම් (fname == NULL)
	ආපසු;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    නම් (cmd == NULL)
    {
	vim_free(fname);
	ආපසු;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (void)char_avail();
# endif

    incr_quickfix_busy();

    නම් (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    නම් (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    නම් (wp != NULL)
    {
	qi = GET_LOC_LIST(wp);
	නම් (qi == NULL)
	    goto cleanup;
    }
    නැතහොත් නම් (qi == NULL)
	goto cleanup;

    නම් (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    නම් (au_name != NULL)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    නම් (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, FALSE);

cleanup:
    decr_quickfix_busy();
    mch_remove(fname);
    vim_free(fname);
    vim_free(cmd);
}

/*
 * Returns the number of entries in the current quickfix/location list.
 */
    පූර්ණ_සංඛ්‍යා
qf_get_size(exarg_T *eap)
{
    qf_info_T	*qi;

    නම් ((qi = qf_cmd_get_stack(eap, FALSE)) == NULL)
	ආපසු 0;
    ආපසු qf_get_curlist(qi)->qf_count;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    පූර්ණ_සංඛ්‍යා
qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    පූර්ණ_සංඛ්‍යා		i, sz = 0;
    පූර්ණ_සංඛ්‍යා		prev_fnum = 0;

    නම් ((qi = qf_cmd_get_stack(eap, FALSE)) == NULL)
	ආපසු 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	නම් (qfp->qf_valid)
	{
	    නම් (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    නැතහොත් නම් (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    ආපසු sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    පූර්ණ_සංඛ්‍යා
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    නම් ((qi = qf_cmd_get_stack(eap, FALSE)) == NULL)
	ආපසු 0;

    ආපසු qf_get_curlist(qi)->qf_index;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    පූර්ණ_සංඛ්‍යා
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    පූර්ණ_සංඛ්‍යා		i, eidx = 0;
    පූර්ණ_සංඛ්‍යා		prev_fnum = 0;

    නම් ((qi = qf_cmd_get_stack(eap, FALSE)) == NULL)
	ආපසු 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_start;

    // check if the list has valid errors
    නම් (!qf_list_has_valid_entries(qfl))
	ආපසු 1;

    සඳහා (i = 1; i <= qfl->qf_index && qfp!= NULL; i++, qfp = qfp->qf_next)
    {
	නම් (qfp->qf_valid)
	{
	    නම් (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		නම් (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    නැතහොත්
		eidx++;
	}
    }

    ආපසු eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    static පූර්ණ_සංඛ්‍යා
qf_get_nth_valid_entry(qf_list_T *qfl, පූර්ණ_සංඛ්‍යා n, පූර්ණ_සංඛ්‍යා fdo)
{
    qfline_T	*qfp;
    පූර්ණ_සංඛ්‍යා		i, eidx;
    පූර්ණ_සංඛ්‍යා		prev_fnum = 0;

    // check if the list has valid errors
    නම් (!qf_list_has_valid_entries(qfl))
	ආපසු 1;

    eidx = 0;
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	නම් (qfp->qf_valid)
	{
	    නම් (fdo)
	    {
		නම් (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    නැතහොත්
		eidx++;
	}

	නම් (eidx == n)
	    bha_nga;
    }

    නම් (i <= qfl->qf_count)
	ආපසු i;
    නැතහොත්
	ආපසු 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    void
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    පූර්ණ_සංඛ්‍යා		errornr;

    නම් ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	ආපසු;

    නම් (eap->addr_count > 0)
	errornr = (පූර්ණ_සංඛ්‍යා)eap->line2;
    නැතහොත්
    {
	switch (eap->cmdidx)
	{
	    pak_sa CMD_cc: pak_sa CMD_ll:
		errornr = 0;
		bha_nga;
	    pak_sa CMD_crewind: pak_sa CMD_lrewind: pak_sa CMD_cfirst:
	    pak_sa CMD_lfirst:
		errornr = 1;
		bha_nga;
	    default:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    නම් (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_count > 0 ? (පූර්ණ_සංඛ්‍යා)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    void
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    පූර්ණ_සංඛ්‍යා		errornr;
    පූර්ණ_සංඛ්‍යා		dir;

    නම් ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	ආපසු;

    නම් (eap->addr_count > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (පූර්ණ_සංඛ්‍යා)eap->line2;
    නැතහොත්
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    switch (eap->cmdidx)
    {
	pak_sa CMD_cnext: pak_sa CMD_lnext: pak_sa CMD_cdo: pak_sa CMD_ldo:
	    dir = FORWARD;
	    bha_nga;
	pak_sa CMD_cprevious: pak_sa CMD_lprevious: pak_sa CMD_cNext:
	pak_sa CMD_lNext:
	    dir = BACKWARD;
	    bha_nga;
	pak_sa CMD_cnfile: pak_sa CMD_lnfile: pak_sa CMD_cfdo: pak_sa CMD_lfdo:
	    dir = FORWARD_FILE;
	    bha_nga;
	pak_sa CMD_cpfile: pak_sa CMD_lpfile: pak_sa CMD_cNfile: pak_sa CMD_lNfile:
	    dir = BACKWARD_FILE;
	    bha_nga;
	default:
	    dir = FORWARD;
	    bha_nga;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    static qfline_T *
qf_find_first_entry_in_buf(qf_list_T *qfl, පූර්ණ_සංඛ්‍යා bnr, පූර්ණ_සංඛ්‍යා *errornr)
{
    qfline_T	*qfp = NULL;
    පූර්ණ_සංඛ්‍යා		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_ITEMS(qfl, qfp, idx)
	නම් (qfp->qf_fnum == bnr)
	    bha_nga;

    *errornr = idx;
    ආපසු qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    static qfline_T *
qf_find_first_entry_on_line(qfline_T *entry, පූර්ණ_සංඛ්‍යා *errornr)
{
    අතරතුර (!got_int
	    && entry->qf_prev != NULL
	    && entry->qf_fnum == entry->qf_prev->qf_fnum
	    && entry->qf_lnum == entry->qf_prev->qf_lnum)
    {
	entry = entry->qf_prev;
	--*errornr;
    }

    ආපසු entry;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    static qfline_T *
qf_find_last_entry_on_line(qfline_T *entry, පූර්ණ_සංඛ්‍යා *errornr)
{
    අතරතුර (!got_int &&
	    entry->qf_next != NULL
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    ආපසු entry;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    static පූර්ණ_සංඛ්‍යා
qf_entry_after_pos(qfline_T *qfp, pos_T *pos, පූර්ණ_සංඛ්‍යා linewise)
{
    නම් (linewise)
	ආපසු qfp->qf_lnum > pos->lnum;
    නැතහොත්
	ආපසු (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    static පූර්ණ_සංඛ්‍යා
qf_entry_before_pos(qfline_T *qfp, pos_T *pos, පූර්ණ_සංඛ්‍යා linewise)
{
    නම් (linewise)
	ආපසු qfp->qf_lnum < pos->lnum;
    නැතහොත්
	ආපසු (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col < pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    static පූර්ණ_සංඛ්‍යා
qf_entry_on_or_after_pos(qfline_T *qfp, pos_T *pos, පූර්ණ_සංඛ්‍යා linewise)
{
    නම් (linewise)
	ආපසු qfp->qf_lnum >= pos->lnum;
    නැතහොත්
	ආපසු (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col >= pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    static පූර්ණ_සංඛ්‍යා
qf_entry_on_or_before_pos(qfline_T *qfp, pos_T *pos, පූර්ණ_සංඛ්‍යා linewise)
{
    නම් (linewise)
	ආපසු qfp->qf_lnum <= pos->lnum;
    නැතහොත්
	ආපසු (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col <= pos->col));
}

/*
 * Find the first quickfix entry after position 'pos' in buffer 'bnr'.
 * If 'linewise' is TRUE, returns the entry after the specified line and treats
 * multiple entries on a single line as one. Otherwise returns the entry after
 * the specified line and column.
 * 'qfp' points to the very first entry in the buffer and 'errornr' is the
 * index of the very first entry in the quickfix list.
 * Returns NULL if an entry is not found after 'pos'.
 */
    static qfline_T *
qf_find_entry_after_pos(
	පූර්ණ_සංඛ්‍යා		bnr,
	pos_T		*pos,
	පූර්ණ_සංඛ්‍යා		linewise,
	qfline_T	*qfp,
	පූර්ණ_සංඛ්‍යා		*errornr)
{
    නම් (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	ආපසු qfp;

    // Find the entry just before or at the position 'pos'
    අතරතුර (qfp->qf_next != NULL
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    නම් (qfp->qf_next == NULL || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	ආපසු NULL;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    ආපසු qfp;
}

/*
 * Find the first quickfix entry before position 'pos' in buffer 'bnr'.
 * If 'linewise' is TRUE, returns the entry before the specified line and
 * treats multiple entries on a single line as one. Otherwise returns the entry
 * before the specified line and column.
 * 'qfp' points to the very first entry in the buffer and 'errornr' is the
 * index of the very first entry in the quickfix list.
 * Returns NULL if an entry is not found before 'pos'.
 */
    static qfline_T *
qf_find_entry_before_pos(
	පූර්ණ_සංඛ්‍යා		bnr,
	pos_T		*pos,
	පූර්ණ_සංඛ්‍යා		linewise,
	qfline_T	*qfp,
	පූර්ණ_සංඛ්‍යා		*errornr)
{
    // Find the entry just before the position 'pos'
    අතරතුර (qfp->qf_next != NULL
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    නම් (qf_entry_on_or_after_pos(qfp, pos, linewise))
	ආපසු NULL;

    නම් (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_find_first_entry_on_line(qfp, errornr);

    ආපසු qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    static qfline_T *
qf_find_closest_entry(
	qf_list_T	*qfl,
	පූර්ණ_සංඛ්‍යා		bnr,
	pos_T		*pos,
	පූර්ණ_සංඛ්‍යා		dir,
	පූර්ණ_සංඛ්‍යා		linewise,
	පූර්ණ_සංඛ්‍යා		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    නම් (qfp == NULL)
	ආපසු NULL;		// no entry in this file

    නම් (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    නැතහොත්
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    ආපසු qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    static void
qf_get_nth_below_entry(qfline_T *entry_arg, පූර්ණ_සංඛ්‍යා n, පූර්ණ_සංඛ්‍යා linewise, පූර්ණ_සංඛ්‍යා *errornr)
{
    qfline_T *entry = entry_arg;

    අතරතුර (n-- > 0 && !got_int)
    {
	පූර්ණ_සංඛ්‍යා		first_errornr = *errornr;

	නම් (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_line(entry, errornr);

	නම් (entry->qf_next == NULL
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    නම් (linewise)
		*errornr = first_errornr;
	    bha_nga;
	}

	entry = entry->qf_next;
	++*errornr;
    }
}

/*
 * Get the nth quickfix entry above the specified entry.  Searches backwards in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    static void
qf_get_nth_above_entry(qfline_T *entry, පූර්ණ_සංඛ්‍යා n, පූර්ණ_සංඛ්‍යා linewise, පූර්ණ_සංඛ්‍යා *errornr)
{
    අතරතුර (n-- > 0 && !got_int)
    {
	නම් (entry->qf_prev == NULL
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    bha_nga;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	නම් (linewise)
	    entry = qf_find_first_entry_on_line(entry, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    static පූර්ණ_සංඛ්‍යා
qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	පූර්ණ_සංඛ්‍යා		bnr,
	pos_T		*pos,
	පූර්ණ_සංඛ්‍යා		n,
	පූර්ණ_සංඛ්‍යා		dir,
	පූර්ණ_සංඛ්‍යා		linewise)
{
    qfline_T	*adj_entry;
    පූර්ණ_සංඛ්‍යා		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    නම් (adj_entry == NULL)
	ආපසු 0;

    නම් (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	නම් (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	නැතහොත්
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    ආපසු errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    void
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    පූර්ණ_සංඛ්‍යා		dir;
    පූර්ණ_සංඛ්‍යා		buf_has_flag;
    පූර්ණ_සංඛ්‍යා		errornr = 0;
    pos_T	pos;

    නම් (eap->addr_count > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	ආපසු;
    }

    // Check whether the current buffer has any quickfix entries
    නම් (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    නැතහොත්
	buf_has_flag = BUF_HAS_LL_ENTRY;
    නම් (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	ආපසු;
    }

    නම් ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	ආපසු;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    නම් (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	ආපසු;
    }

    නම් (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    නැතහොත්
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_count > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    නම් (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    නැතහොත්
	emsg(_(e_no_more_items));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    static char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	pak_sa CMD_cfile:	    ආපසු (char_u *)"cfile";
	pak_sa CMD_cgetfile:  ආපසු (char_u *)"cgetfile";
	pak_sa CMD_caddfile:  ආපසු (char_u *)"caddfile";
	pak_sa CMD_lfile:	    ආපසු (char_u *)"lfile";
	pak_sa CMD_lgetfile:  ආපසු (char_u *)"lgetfile";
	pak_sa CMD_laddfile:  ආපසු (char_u *)"laddfile";
	default:	    ආපසු NULL;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    void
ex_cfile(exarg_T *eap)
{
    char_u	*enc = NULL;
    win_T	*wp = NULL;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = NULL;
    int_u	save_qfid = 0;		// init for gcc
    පූර්ණ_සංඛ්‍යා		res;

    au_name = cfile_get_auname(eap->cmdidx);
    නම් (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							NULL, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	නම් (aborting())
	    ආපසු;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    නම් (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   NULL, NULL,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), NULL);
	නම් (browse_file == NULL)
	    ආපසු;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    නැතහොත්
# endif
    නම් (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    නම් (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    incr_quickfix_busy();

    // This function is used by the :cfile, :cgetfile and :caddfile
    // commands.
    // :cfile always creates a new quickfix list and may jump to the
    // first error.
    // :cgetfile creates a new quickfix list but doesn't jump to the
    // first error.
    // :caddfile adds to an existing quickfix list. If there is no
    // quickfix list then a new list is created.
    res = qf_init(wp, p_ef, p_efm, (eap->cmdidx != CMD_caddfile
			&& eap->cmdidx != CMD_laddfile),
			qf_cmdtitle(*eap->cmdlinep), enc);
    නම් (wp != NULL)
    {
	qi = GET_LOC_LIST(wp);
	නම් (qi == NULL)
	{
	    decr_quickfix_busy();
	    ආපසු;
	}
    }
    නැතහොත් නම් (qi == NULL)
    {
	decr_quickfix_busy();
	ආපසු;
    }
    නම් (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    නම් (au_name != NULL)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, NULL, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    නම් (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    static char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	pak_sa CMD_vimgrep:     ආපසු (char_u *)"vimgrep";
	pak_sa CMD_lvimgrep:    ආපසු (char_u *)"lvimgrep";
	pak_sa CMD_vimgrepadd:  ආපසු (char_u *)"vimgrepadd";
	pak_sa CMD_lvimgrepadd: ආපසු (char_u *)"lvimgrepadd";
	pak_sa CMD_grep:	      ආපසු (char_u *)"grep";
	pak_sa CMD_lgrep:	      ආපසු (char_u *)"lgrep";
	pak_sa CMD_grepadd:     ආපසු (char_u *)"grepadd";
	pak_sa CMD_lgrepadd:    ආපසු (char_u *)"lgrepadd";
	default: ආපසු NULL;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    static void
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = NULL;

    නම් (s == NULL || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	නම් (last_search_pat() == NULL)
	{
	    emsg(_(e_no_previous_regular_expression));
	    ආපසු;
	}
	regmatch->regprog = vim_regcomp(last_search_pat(), RE_MAGIC);
    }
    නැතහොත්
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    static void
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    msg_start();
    p = msg_strtrunc(fname, TRUE);
    නම් (p == NULL)
	msg_outtrans(fname);
    නැතහොත්
    {
	msg_outtrans(p);
	vim_free(p);
    }
    msg_clr_eos();
    msg_didout = FALSE;	    // overwrite this message
    msg_nowait = TRUE;	    // don't wait for this message
    msg_col = 0;
    out_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    static buf_T *
vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    පූර්ණ_සංඛ්‍යා		save_mls;
# නම් defined(FEAT_SYN_HL)
    char_u	*save_ei = NULL;
# endif
    buf_T	*buf;

# නම් defined(FEAT_SYN_HL)
    // Don't do Filetype autocommands to avoid loading syntax and
    // indent scripts, a great speed improvement.
    save_ei = au_event_disable(",Filetype");
# endif
    // Don't use modelines here, it's useless.
    save_mls = p_mls;
    p_mls = 0;

    // Load file into a buffer, so that 'fileencoding' is detected,
    // autocommands applied, etc.
    buf = load_dummy_buffer(fname, dirname_start, dirname_now);

    p_mls = save_mls;
# නම් defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    ආපසු buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    static පූර්ණ_සංඛ්‍යා
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    නම් (!qflist_valid(wp, qfid))
    {
	නම් (wp != NULL)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    ආපසු FALSE;
	}
	නැතහොත්
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    ආපසු TRUE;
	}
    }

    නම් (qf_restore_list(qi, qfid) == FAIL)
	ආපසු FALSE;

    ආපසු TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    static පූර්ණ_සංඛ්‍යා
vgr_match_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	long	    *tomatch,
	පූර්ණ_සංඛ්‍යා	    duplicate_name,
	පූර්ණ_සංඛ්‍යා	    flags)
{
    පූර්ණ_සංඛ්‍යා		found_match = FALSE;
    long	lnum;
    colnr_T	col;
    පූර්ණ_සංඛ්‍යා		pat_len = (පූර්ණ_සංඛ්‍යා)STRLEN(spat);
    නම් (pat_len > FUZZY_MATCH_MAX_LEN)
	pat_len = FUZZY_MATCH_MAX_LEN;

    සඳහා (lnum = 1; lnum <= buf->b_ml.ml_line_count && *tomatch > 0; ++lnum)
    {
	col = 0;
	නම් (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    අතරතුර (vim_regexec_multi(regmatch, curwin, buf, lnum,
								col, NULL) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		නම් (qf_add_entry(qfl,
			    NULL,	// dir
			    fname,
			    NULL,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].col + 1,
			    regmatch->endpos[0].col + 1,
			    FALSE,	// vis_col
			    NULL,	// search pattern
			    0,		// nr
			    0,		// type
			    NULL,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    bha_nga;
		}
		found_match = TRUE;
		නම් (--*tomatch == 0)
		    bha_nga;
		නම් ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    bha_nga;
		col = regmatch->endpos[0].col
		    + (col == regmatch->endpos[0].col);
		නම් (col > ml_get_buf_len(buf, lnum))
		    bha_nga;
	    }
	}
	නැතහොත්
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    පූර්ණ_සංඛ්‍යා	    score;
	    int_u   matches[FUZZY_MATCH_MAX_LEN];
	    int_u   sz = ARRAY_LENGTH(matches);

	    // Fuzzy string match
	    CLEAR_FIELD(matches);
	    අතරතුර (fuzzy_match(str + col, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		නම් (qf_add_entry(qfl,
			    NULL,	// dir
			    fname,
			    NULL,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + col + 1,
			    0,
			    FALSE,	// vis_col
			    NULL,	// search pattern
			    0,		// nr
			    0,		// type
			    NULL,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    bha_nga;
		}
		found_match = TRUE;
		නම් (--*tomatch == 0)
		    bha_nga;
		නම් ((flags & VGR_GLOBAL) == 0)
		    bha_nga;
		col = matches[pat_len - 1] + col + 1;
		නම් (col > linelen)
		    bha_nga;
	    }
	}
	line_breakcheck();
	නම් (got_int)
	    bha_nga;
    }

    ආපසු found_match;
}

/*
 * Jump to the first match and update the directory.
 */
    static void
vgr_jump_to_match(
	qf_info_T   *qi,
	පූර්ණ_සංඛ්‍යා	    forceit,
	පූර්ණ_සංඛ්‍යා	    *redraw_for_dummy,
	buf_T	    *first_match_buf,
	char_u	    *target_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    නම් (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    නම් (curbuf == first_match_buf && target_dir != NULL)
    {
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = target_dir;
	ea.cmdidx = CMD_lcd;
	ex_cd(&ea);
    }
}

/*
 * :vimgrep command arguments
 */
typedef sa_nracanaa
{
    long	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    පූර්ණ_සංඛ්‍යා		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    පූර්ණ_සංඛ්‍යා		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_args_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    static පූර්ණ_සංඛ්‍යා
vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    CLEAR_POINTER(args);

    args->regmatch.regprog = NULL;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    නම් (eap->addr_count > 0)
	args->tomatch = eap->line2;
    නැතහොත්
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    නම් (p == NULL)
    {
	emsg(_(e_invalid_search_pattern_or_delimiter));
	ආපසු FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    නම් (args->regmatch.regprog == NULL)
	ආපසු FAIL;

    p = skipwhite(p);
    නම් (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	ආපසු FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    නම් ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	ආපසු FAIL;
    }

    ආපසු OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    static පූර්ණ_සංඛ්‍යා
existing_swapfile(buf_T *buf)
{
    නම් (buf->b_ml.ml_mfp != NULL && buf->b_ml.ml_mfp->mf_fname != NULL)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	ආපසු fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    ආපසු FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    static පූර්ණ_සංඛ්‍යා
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_args_T	*cmd_args,
	පූර්ණ_සංඛ්‍යා		*redraw_for_dummy,
	buf_T		**first_match_buf,
	char_u		**target_dir)
{
    පූර්ණ_සංඛ්‍යා		status = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    පූර්ණ_සංඛ්‍යා		fi;
    buf_T	*buf;
    පූර්ණ_සංඛ්‍යා		duplicate_name = FALSE;
    පූර්ණ_සංඛ්‍යා		using_dummy;
    char_u	*dirname_start = NULL;
    char_u	*dirname_now = NULL;
    පූර්ණ_සංඛ්‍යා		found_match;
    aco_save_T	aco;

    dirname_start = alloc_id(MAXPATHL, aid_qf_dirname_start);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    නම් (dirname_start == NULL || dirname_now == NULL)
	goto theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_start, MAXPATHL);

    seconds = (time_t)0;
    සඳහා (fi = 0; fi < cmd_args->fcount && !got_int && cmd_args->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_args->fnames[fi]);
	නම් (time(NULL) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = time(NULL);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_args->fnames[fi]);
	නම් (buf == NULL || buf->b_ml.ml_mfp == NULL)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != NULL);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_load_dummy_buf(fname, dirname_start, dirname_now);
	}
	නැතහොත්
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	නම් (!vgr_qflist_valid(wp, qi, save_qfid, cmd_args->qf_title))
	    goto theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	නම් (buf == NULL)
	{
	    නම් (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	නැතහොත්
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    found_match = vgr_match_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_args->spat, &cmd_args->regmatch,
		    &cmd_args->tomatch, duplicate_name, cmd_args->flags);

	    නම් (using_dummy)
	    {
		නම් (found_match && *first_match_buf == NULL)
		    *first_match_buf = buf;
		නම් (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_buffer(buf, dirname_start);
		    buf = NULL;
		}
		නැතහොත් නම් ((cmdmod.cmod_flags & CMOD_HIDE) == 0
			    || buf->b_p_bh[0] == 'u'	// "unload"
			    || buf->b_p_bh[0] == 'w'	// "wipe"
			    || buf->b_p_bh[0] == 'd')	// "delete"
		{
		    // When no match was found we don't need to remember the
		    // buffer, wipe it out.  If there was a match and it
		    // wasn't the first one or we won't jump there: only
		    // unload the buffer.
		    // Ignore 'hidden' here, because it may lead to having too
		    // many swap files.
		    නම් (!found_match)
		    {
			wipe_dummy_buffer(buf, dirname_start);
			buf = NULL;
		    }
		    නැතහොත් නම් (buf != *first_match_buf
					|| (cmd_args->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_buffer(buf, dirname_start);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = NULL;
		    }
		}

		නම් (buf != NULL)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    නම් (buf == *first_match_buf
			    && *target_dir == NULL
			    && STRCMP(dirname_start, dirname_now) != 0)
			*target_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    නම් (curbuf == buf)
		    {
# නම් defined(FEAT_SYN_HL)
			apply_autocmds(EVENT_FILETYPE, buf->b_p_ft,
						     buf->b_fname, TRUE, buf);
# endif
			do_modelines(OPT_NOWIN);
			aucmd_restbuf(&aco);
		    }
		}
	    }
	}
    }

    status = OK;

theend:
    vim_free(dirname_now);
    vim_free(dirname_start);
    ආපසු status;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    void
ex_vimgrep(exarg_T *eap)
{
    vgr_args_T	args;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = NULL;
    පූර්ණ_සංඛ්‍යා		redraw_for_dummy = FALSE;
    buf_T	*first_match_buf = NULL;
    char_u	*target_dir = NULL;
    char_u	*au_name =  NULL;
    පූර්ණ_සංඛ්‍යා		status;

    නම් (!check_can_set_curbuf_forceit(eap->forceit))
	ආපසු;

    au_name = vgr_get_auname(eap->cmdidx);
    නම් (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	නම් (aborting())
	    ආපසු;
# endif
    }

    qi = qf_cmd_get_or_alloc_stack(eap, &wp);
    නම් (qi == NULL)
	ආපසු;

    නම් (vgr_process_args(eap, &args) == FAIL)
	goto theend;

    නම් ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_stack_empty(qi))
	// make place for a new list
	qf_new_list(qi, args.qf_title);

    incr_quickfix_busy();

    status = vgr_process_files(wp, qi, &args, &redraw_for_dummy,
						&first_match_buf, &target_dir);
    නම් (status != OK)
    {
	FreeWild(args.fcount, args.fnames);
	decr_quickfix_busy();
	goto theend;
    }

    FreeWild(args.fcount, args.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_start;
    qfl->qf_index = 1;
    qf_list_changed(qfl);

    qf_update_buffer(qi, NULL);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    නම් (au_name != NULL)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    නම් (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	goto theend;
    }

    // Jump to first match.
    නම් (!qf_list_empty(qf_get_curlist(qi)))
    {
	නම් ((args.flags & VGR_NOJUMP) == 0)
	    vgr_jump_to_match(qi, eap->forceit, &redraw_for_dummy,
		    first_match_buf, target_dir);
    }
    නැතහොත්
	semsg(_(e_no_match_str_2), args.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    නම් (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# නැතහොත්
	redraw_later(UPD_NOT_VALID);
# endif
    }

theend:
    vim_free(args.qf_title);
    vim_free(target_dir);
    vim_regfree(args.regmatch.regprog);
}

/*
 * Restore current working directory to "dirname_start" if they differ, taking
 * into account whether it is set locally or globally.
 */
    static void
restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    නම් (dirname_now == NULL)
	ආපසු;

    mch_dirname(dirname_now, MAXPATHL);
    නම් (STRCMP(dirname_start, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = dirname_start;
	ea.cmdidx = (curwin->w_localdir == NULL) ? CMD_cd : CMD_lcd;
	ex_cd(&ea);
    }
    vim_free(dirname_now);
}

/*
 * Load file "fname" into a dummy buffer and return the buffer pointer,
 * placing the directory resulting from the buffer load into the
 * "resulting_dir" pointer. "resulting_dir" must be allocated by the caller
 * prior to calling this function. Restores directory to "dirname_start" prior
 * to returning, if autocmds or the 'autochdir' option have changed it.
 *
 * If creating the dummy buffer does not fail, must call unload_dummy_buffer()
 * or wipe_dummy_buffer() later!
 *
 * Returns NULL if it fails.
 */
    static buf_T *
load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    පූර්ණ_සංඛ්‍යා		failed = TRUE;
    aco_save_T	aco;
    පූර්ණ_සංඛ්‍යා		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(NULL, NULL, (linenr_T)1, BLN_DUMMY);
    නම් (newbuf == NULL)
	ආපසු NULL;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    නම් (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	නම් (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (void)setfname(curbuf, fname, NULL, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_swap(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_to_wipe.br_buf = NULL;
	    readfile_result = readfile(fname, NULL,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			NULL, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    නම් (readfile_result == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		නම් (curbuf != newbuf)
		{
		    // Bloody autocommands changed the buffer!  Can happen when
		    // using netrw and editing a remote file.  Use the current
		    // buffer instead, delete the dummy one after restoring the
		    // window stuff.
		    set_bufref(&newbuf_to_wipe, newbuf);
		    newbuf = curbuf;
		}
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);

	    නම් (newbuf_to_wipe.br_buf != NULL && bufref_valid(&newbuf_to_wipe))
	    {
		block_autocmds();
		wipe_dummy_buffer(newbuf_to_wipe.br_buf, NULL);
		unblock_autocmds();
	    }
	}

	// Add back the "dummy" flag, otherwise buflist_findname_stat() won't
	// skip it.
	newbuf->b_flags |= BF_DUMMY;
    }

    // When autocommands/'autochdir' option changed directory: go back.
    // Let the caller know what the resulting dir was first, in case it is
    // important.
    mch_dirname(resulting_dir, MAXPATHL);
    restore_start_dir(dirname_start);

    නම් (!bufref_valid(&newbufref))
	ආපසු NULL;
    නම් (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	ආපසු NULL;
    }
    ආපසු newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    static void
wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    අතරතුර (buf->b_nwindows > 0)
    {
	පූර්ණ_සංඛ්‍යා	    did_one = FALSE;
	win_T	    *wp;

	නම් (firstwin->w_next != NULL)
	    FOR_ALL_WINDOWS(wp)
		නම් (wp->w_buffer == buf)
		{
		    නම් (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    bha_nga;
		}
	නම් (!did_one)
	    goto fail;
    }

    නම් (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# නම් defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_buffer(buf, TRUE);

# නම් defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	නම් (dirname_start != NULL)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_start_dir(dirname_start);

	ආපසු;
    }

fail:
    // Keeping the buffer, remove the dummy flag.
    buf->b_flags &= ~BF_DUMMY;
}

/*
 * Unload the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" prior to returning, if autocmds or the
 * 'autochdir' option have changed it.
 */
    static void
unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    නම් (curbuf == buf)		// safety check
	ආපසු;

    close_buffer(NULL, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_start_dir(dirname_start);
}

# නම් defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    static පූර්ණ_සංඛ්‍යා
get_qfline_items(qfline_T *qfp, list_T *list)
{
    පූර්ණ_සංඛ්‍යා		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    නම් (bufnum != 0 && (buflist_findnr(bufnum) == NULL))
	bufnum = 0;

    නම් ((dict = dict_alloc()) == NULL)
	ආපසු FAIL;
    නම් (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	ආපසු FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    නම් (dict_add_number(dict, "bufnr", (long)bufnum) == FAIL
	    || dict_add_number(dict, "lnum",     (long)qfp->qf_lnum) == FAIL
	    || dict_add_number(dict, "end_lnum", (long)qfp->qf_end_lnum) == FAIL
	    || dict_add_number(dict, "col",      (long)qfp->qf_col) == FAIL
	    || dict_add_number(dict, "end_col",  (long)qfp->qf_end_col) == FAIL
	    || dict_add_number(dict, "vcol",     (long)qfp->qf_viscol) == FAIL
	    || dict_add_number(dict, "nr",       (long)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string_len(dict, "type", buf, (පූර්ණ_සංඛ්‍යා)buflen) == FAIL
	    || (qfp->qf_user_data.v_type != VAR_UNKNOWN
		&& dict_add_tv(dict, "user_data", &qfp->qf_user_data) == FAIL )
	    || dict_add_number(dict, "valid", (long)qfp->qf_valid) == FAIL)
	ආපසු FAIL;

    ආපසු OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    static පූර්ණ_සංඛ්‍යා
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	පූර්ණ_සංඛ්‍යා		qf_idx,
	පූර්ණ_සංඛ්‍යා		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    පූර්ණ_සංඛ්‍යා		i;

    නම් (qi == NULL)
    {
	qi = ql_info;
	නම් (wp != NULL)
	    qi = GET_LOC_LIST(wp);
	නම් (qi == NULL)
	    ආපසු FAIL;

    }

    නම් (eidx < 0)
	ආපසු OK;

    නම් (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    නම් (qf_idx >= qi->qf_listcount)
	ආපසු FAIL;

    qfl = qf_get_list(qi, qf_idx);
    නම් (qf_list_empty(qfl))
	ආපසු FAIL;

    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	නම් (eidx > 0)
	{
	    නම් (eidx == i)
		ආපසු get_qfline_items(qfp, list);
	}
	නැතහොත් නම් (get_qfline_items(qfp, list) == FAIL)
	    ආපසු FAIL;
    }

    ආපසු OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
ga_nanaa {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_ITEMS	= 0x2,
    QF_GETLIST_NR	= 0x4,
    QF_GETLIST_WINID	= 0x8,
    QF_GETLIST_CONTEXT	= 0x10,
    QF_GETLIST_ID	= 0x20,
    QF_GETLIST_IDX	= 0x40,
    QF_GETLIST_SIZE	= 0x80,
    QF_GETLIST_TICK	= 0x100,
    QF_GETLIST_FILEWINID	= 0x200,
    QF_GETLIST_QFBUFNR	= 0x400,
    QF_GETLIST_QFTF	= 0x800,
    QF_GETLIST_ALL	= 0xFFF,
};

/*
 * Parse text from 'di' and return the quickfix list items.
 * Existing quickfix lists are not modified.
 */
    static පූර්ණ_සංඛ්‍යා
qf_get_list_from_lines(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    පූර්ණ_සංඛ්‍යා		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    නම් (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == NULL)
	ආපසු FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    නම් ((efm_di = dict_find(what, (char_u *)"efm", -1)) != NULL)
    {
	නම් (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == NULL)
	    ආපසු FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    නම් (l == NULL)
	ආපසු FAIL;

    qi = qf_alloc_stack(QFLT_INTERNAL, 1);
    නම් (qi != NULL)
    {
	නම් (qf_init_ext(qi, 0, NULL, NULL, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, NULL, NULL) > 0)
	{
	    (void)get_errorlist(qi, NULL, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_add_list(retdict, "items", l);
    status = OK;

    ආපසු status;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    static පූර්ණ_සංඛ්‍යා
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    නම් (qi == NULL)
	ආපසු 0;
    win = qf_find_win(qi);
    නම් (win != NULL)
	ආපසු win->w_id;
    ආපසු 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    පූර්ණ_සංඛ්‍යා	bufnum = 0;

    නම් (qi != NULL && buflist_findnr(qi->qf_bufnr) != NULL)
	bufnum = qi->qf_bufnr;

    ආපසු dict_add_number(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_keys2flags(dict_T *what, පූර්ණ_සංඛ්‍යා loclist)
{
    පූර්ණ_සංඛ්‍යා		flags = QF_GETLIST_NONE;

    නම් (dict_has_key(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	නම් (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    නම් (dict_has_key(what, "title"))
	flags |= QF_GETLIST_TITLE;

    නම් (dict_has_key(what, "nr"))
	flags |= QF_GETLIST_NR;

    නම් (dict_has_key(what, "winid"))
	flags |= QF_GETLIST_WINID;

    නම් (dict_has_key(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    නම් (dict_has_key(what, "id"))
	flags |= QF_GETLIST_ID;

    නම් (dict_has_key(what, "items"))
	flags |= QF_GETLIST_ITEMS;

    නම් (dict_has_key(what, "idx"))
	flags |= QF_GETLIST_IDX;

    නම් (dict_has_key(what, "size"))
	flags |= QF_GETLIST_SIZE;

    නම් (dict_has_key(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    නම් (loclist && dict_has_key(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    නම් (dict_has_key(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    නම් (dict_has_key(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    ආපසු flags;
}

/*
 * Return the quickfix list index based on 'nr' or 'id' in 'what'.
 * If 'nr' and 'id' are not present in 'what' then return the current
 * quickfix list index.
 * If 'nr' is zero then return the current quickfix list index.
 * If 'nr' is '$' then return the last quickfix list index.
 * If 'id' is present then return the index of the quickfix list with that id.
 * If 'id' is zero then return the quickfix list index specified by 'nr'.
 * Return -1, if quickfix list is not present or if the stack is empty.
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    පූර්ණ_සංඛ්‍යා		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    නම් ((di = dict_find(what, (char_u *)"nr", -1)) != NULL)
    {
	// Use the specified quickfix/location list
	නම් (di->di_tv.v_type == VAR_NUMBER)
	{
	    // for zero use the current list
	    නම් (di->di_tv.vval.v_number != 0)
	    {
		qf_idx = di->di_tv.vval.v_number - 1;
		නම් (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	නැතහොත් නම් (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != NULL
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	නැතහොත්
	    qf_idx = INVALID_QFIDX;
    }

    නම් ((di = dict_find(what, (char_u *)"id", -1)) != NULL)
    {
	// Look for a list with the specified id
	නම් (di->di_tv.v_type == VAR_NUMBER)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    නම් (di->di_tv.vval.v_number != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_number);
	}
	නැතහොත්
	    qf_idx = INVALID_QFIDX;
    }

    ආපසු qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_defaults(qf_info_T *qi, පූර්ණ_සංඛ්‍යා flags, පූර්ණ_සංඛ්‍යා locstack, dict_T *retdict)
{
    පූර්ණ_සංඛ්‍යා		status = OK;

    නම් (flags & QF_GETLIST_TITLE)
	status = dict_add_string_len(retdict, "title", (char_u *)"", 0);
    නම් ((status == OK) && (flags & QF_GETLIST_ITEMS))
    {
	list_T	*l = list_alloc();
	නම් (l != NULL)
	    status = dict_add_list(retdict, "items", l);
	නැතහොත්
	    status = FAIL;
    }
    නම් ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_number(retdict, "nr", 0);
    නම් ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_number(retdict, "winid", qf_winid(qi));
    නම් ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);
    නම් ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_number(retdict, "id", 0);
    නම් ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_number(retdict, "idx", 0);
    නම් ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_number(retdict, "size", 0);
    නම් ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_number(retdict, "changedtick", 0);
    නම් ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_number(retdict, "filewinid", 0);
    නම් ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    නම් ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ආපසු status;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    ආපසු dict_add_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    පූර්ණ_සංඛ්‍යා winid = 0;

    නම් (wp != NULL && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	නම් (ll_wp != NULL)
	    winid = ll_wp->w_id;
    }

    ආපසු dict_add_number(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_items(qf_info_T *qi, පූර්ණ_සංඛ්‍යා qf_idx, පූර්ණ_සංඛ්‍යා eidx, dict_T *retdict)
{
    පූර්ණ_සංඛ්‍යා		status = OK;
    list_T	*l = list_alloc();
    නම් (l != NULL)
    {
	(void)get_errorlist(qi, NULL, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    නැතහොත්
	status = FAIL;

    ආපසු status;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    පූර්ණ_සංඛ්‍යා		status;
    dictitem_T	*di;

    නම් (qfl->qf_ctx != NULL)
    {
	di = dictitem_alloc((char_u *)"context");
	නම් (di != NULL)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    නම් (status == FAIL)
		dictitem_free(di);
	}
	නැතහොත්
	    status = FAIL;
    }
    නැතහොත්
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);

    ආපසු status;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_idx(qf_list_T *qfl, පූර්ණ_සංඛ්‍යා eidx, dict_T *retdict)
{
    නම් (eidx == 0)
    {
	eidx = qfl->qf_index;
	නම් (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    ආපසු dict_add_number(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    static පූර්ණ_සංඛ්‍යා
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    පූර්ණ_සංඛ්‍යා		status;

    නම් (qfl->qf_qftf_cb.cb_name != NULL)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    නැතහොත්
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ආපසු status;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    static පූර්ණ_සංඛ්‍යා
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    පූර්ණ_සංඛ්‍යා		status = OK;
    පූර්ණ_සංඛ්‍යා		qf_idx = INVALID_QFIDX;
    පූර්ණ_සංඛ්‍යා		eidx = 0;
    dictitem_T	*di;
    පූර්ණ_සංඛ්‍යා		flags = QF_GETLIST_NONE;

    නම් ((di = dict_find(what, (char_u *)"lines", -1)) != NULL)
	ආපසු qf_get_list_from_lines(what, di, retdict);

    නම් (wp != NULL)
	qi = GET_LOC_LIST(wp);
    නැතහොත් නම් (qi == NULL)
	ආපසු FAIL;

    flags = qf_getprop_keys2flags(what, (wp != NULL));

    නම් (!qf_stack_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    නම් (qf_stack_empty(qi) || qf_idx == INVALID_QFIDX)
	ආපසු qf_getprop_defaults(qi, flags, wp != NULL, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    නම් ((di = dict_find(what, (char_u *)"idx", -1)) != NULL)
    {
	නම් (di->di_tv.v_type != VAR_NUMBER)
	    ආපසු FAIL;
	eidx = di->di_tv.vval.v_number;
    }

    නම් (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    නම් ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_number(retdict, "nr", qf_idx + 1);
    නම් ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_number(retdict, "winid", qf_winid(qi));
    නම් ((status == OK) && (flags & QF_GETLIST_ITEMS))
	status = qf_getprop_items(qi, qf_idx, eidx, retdict);
    නම් ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    නම් ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_number(retdict, "id", qfl->qf_id);
    නම් ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    නම් ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_number(retdict, "size", qfl->qf_count);
    නම් ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_number(retdict, "changedtick", qfl->qf_changedtick);
    නම් ((status == OK) && (wp != NULL) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    නම් ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    නම් ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    ආපසු status;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    static පූර්ණ_සංඛ්‍යා
qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	පූර්ණ_සංඛ්‍යා		first_entry,
	පූර්ණ_සංඛ්‍යා		*valid_entry)
{
    static පූර්ණ_සංඛ්‍යා	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    පූර්ණ_සංඛ්‍යා		bufnum, valid, status, col, end_col, vcol, nr;
    long	lnum, end_lnum;

    නම් (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (පූර්ණ_සංඛ්‍යා)dict_get_number(d, "bufnr");
    lnum = (පූර්ණ_සංඛ්‍යා)dict_get_number(d, "lnum");
    end_lnum = (පූර්ණ_සංඛ්‍යා)dict_get_number(d, "end_lnum");
    col = (පූර්ණ_සංඛ්‍යා)dict_get_number(d, "col");
    end_col = (පූර්ණ_සංඛ්‍යා)dict_get_number(d, "end_col");
    vcol = (පූර්ණ_සංඛ්‍යා)dict_get_number(d, "vcol");
    nr = (පූර්ණ_සංඛ්‍යා)dict_get_number(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    නම් (text == NULL)
	text = vim_strsave((char_u *)"");
    typval_T user_data;
    user_data.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_data);

    valid = TRUE;
    නම් ((filename == NULL && bufnum == 0) || (lnum == 0 && pattern == NULL))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    නම් (bufnum != 0 && (buflist_findnr(bufnum) == NULL))
    {
	නම් (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    නම් (dict_has_key(d, "valid"))
	valid = (පූර්ණ_සංඛ්‍යා)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			NULL,		// dir
			filename,
			module,
			bufnum,
			text,
			lnum,
			end_lnum,
			col,
			end_col,
			vcol,		// vis_col
			pattern,	// search pattern
			nr,
			type == NULL ? NUL : *type,
			&user_data,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);
    clear_tv(&user_data);

    නම් (valid)
	*valid_entry = TRUE;

    ආපසු status;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    static පූර්ණ_සංඛ්‍යා
entry_is_closer_to_target(
	qfline_T	*entry,
	qfline_T	*other_entry,
	පූර්ණ_සංඛ්‍යා		target_fnum,
	පූර්ණ_සංඛ්‍යා		target_lnum,
	පූර්ණ_සංඛ්‍යා		target_col)
{
    // First, compare entries to target file.
    නම් (!target_fnum)
	// Without a target file, we can't know which is closer.
	ආපසු FALSE;

    පූර්ණ_සංඛ්‍යා is_target_file = entry->qf_fnum && entry->qf_fnum == target_fnum;
    පූර්ණ_සංඛ්‍යා other_is_target_file = other_entry->qf_fnum && other_entry->qf_fnum == target_fnum;
    නම් (!is_target_file && other_is_target_file)
	ආපසු FALSE;
    නැතහොත් නම් (is_target_file && !other_is_target_file)
	ආපසු TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    නම් (!target_lnum)
	// Without a target line number, we can't know which is closer.
	ආපසු FALSE;

    පූර්ණ_සංඛ්‍යා line_distance = entry->qf_lnum ? labs(entry->qf_lnum - target_lnum) : INT_MAX;
    පූර්ණ_සංඛ්‍යා other_line_distance = other_entry->qf_lnum ? labs(other_entry->qf_lnum - target_lnum) : INT_MAX;
    නම් (line_distance > other_line_distance)
	ආපසු FALSE;
    නැතහොත් නම් (line_distance < other_line_distance)
	ආපසු TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    නම් (!target_col)
	// Without a target column, we can't know which is closer.
	ආපසු FALSE;

    පූර්ණ_සංඛ්‍යා column_distance = entry->qf_col ? abs(entry->qf_col - target_col) : INT_MAX;
    පූර්ණ_සංඛ්‍යා other_column_distance = other_entry->qf_col ? abs(other_entry->qf_col - target_col): INT_MAX;
    නම් (column_distance > other_column_distance)
	ආපසු FALSE;
    නැතහොත් නම් (column_distance < other_column_distance)
	ආපසු TRUE;

    // It's a complete tie! The exact same file, line, and column.
    ආපසු FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    static පූර්ණ_සංඛ්‍යා
qf_add_entries(
	qf_info_T	*qi,
	පූර්ණ_සංඛ්‍යා		qf_idx,
	list_T		*list,
	char_u		*title,
	පූර්ණ_සංඛ්‍යා		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_last = NULL;
    පූර්ණ_සංඛ්‍යා		retval = OK;
    පූර්ණ_සංඛ්‍යා		valid_entry = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    පූර්ණ_සංඛ්‍යා prev_fnum = 0;
    පූර්ණ_සංඛ්‍යා prev_lnum = 0;
    පූර්ණ_සංඛ්‍යා prev_col = 0;
    නම් (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_col = qfl->qf_ptr->qf_col;
    }

    පූර්ණ_සංඛ්‍යා select_first_entry = FALSE;
    පූර්ණ_සංඛ්‍යා select_nearest_entry = FALSE;

    නම් (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_first_entry = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    නැතහොත් නම් (action == 'a')
    {
	නම් (qf_list_empty(qfl))
	    // Appending to empty list, select first entry.
	    select_first_entry = TRUE;
	නැතහොත්
	    // Adding to existing list, use last entry.
	    old_last = qfl->qf_last;
    }
    නැතහොත් නම් (action == 'r')
    {
	select_first_entry = TRUE;
	qf_free_items(qfl);
	qf_store_title(qfl, title);
    }
    නැතහොත් නම් (action == 'u')
    {
	select_nearest_entry = TRUE;
	qf_free_items(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *entry_to_select = NULL;
    පූර්ණ_සංඛ්‍යා entry_to_select_index = 0;

    FOR_ALL_LIST_ITEMS(list, li)
    {
	නම් (li->li_tv.v_type != VAR_DICT)
	    continue; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	නම් (d == NULL)
	    continue;

	retval = qf_add_entry_from_dict(qfl, d, li == list->lv_first,
								&valid_entry);
	නම් (retval == QF_FAIL)
	    bha_nga;

	qfline_T *entry = qfl->qf_last;
	නම් (
	    (select_first_entry && entry_to_select == NULL) ||
	    (select_nearest_entry &&
		(entry_to_select == NULL ||
		 entry_is_closer_to_target(entry, entry_to_select, prev_fnum,
					   prev_lnum, prev_col))))
	{
	    entry_to_select = entry;
	    entry_to_select_index = qfl->qf_count;
	}
    }

    // Check if any valid error entries are added to the list.
    නම් (valid_entry)
	qfl->qf_nonevalid = FALSE;
    නැතහොත් නම් (qfl->qf_index == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    නම් (entry_to_select)
    {
	qfl->qf_ptr = entry_to_select;
	qfl->qf_index = entry_to_select_index;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_buffer(qi, old_last);

    ආපසු retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    static පූර්ණ_සංඛ්‍යා
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	පූර්ණ_සංඛ්‍යා		action,
	පූර්ණ_සංඛ්‍යා		*newlist)
{
    dictitem_T	*di;
    පූර්ණ_සංඛ්‍යා		qf_idx = qi->qf_curlist;    // default is the current list

    නම් ((di = dict_find(what, (char_u *)"nr", -1)) != NULL)
    {
	// Use the specified quickfix/location list
	නම් (di->di_tv.v_type == VAR_NUMBER)
	{
	    // for zero use the current list
	    නම් (di->di_tv.vval.v_number != 0)
		qf_idx = di->di_tv.vval.v_number - 1;

	    නම් ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_stack_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    නැතහොත් නම් (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		ආපසු INVALID_QFIDX;
	    නැතහොත් නම් (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	නැතහොත් නම් (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != NULL
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    නම් (!qf_stack_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    නැතහොත් නම් (*newlist)
		qf_idx = 0;
	    නැතහොත්
		ආපසු INVALID_QFIDX;
	}
	නැතහොත්
	    ආපසු INVALID_QFIDX;
    }

    නම් (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != NULL)
    {
	// Use the quickfix/location list with the specified id
	නම් (di->di_tv.v_type != VAR_NUMBER)
	    ආපසු INVALID_QFIDX;

	ආපසු qf_id2nr(qi, di->di_tv.vval.v_number);
    }

    ආපසු qf_idx;
}

/*
 * Set the quickfix list title.
 */
    static පූර්ණ_සංඛ්‍යා
qf_setprop_title(qf_info_T *qi, පූර්ණ_සංඛ්‍යා qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    නම් (di->di_tv.v_type != VAR_STRING)
	ආපසු FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    නම් (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    ආපසු OK;
}

/*
 * Set quickfix list items/entries.
 */
    static පූර්ණ_සංඛ්‍යා
qf_setprop_items(qf_info_T *qi, පූර්ණ_සංඛ්‍යා qf_idx, dictitem_T *di, පූර්ණ_සංඛ්‍යා action)
{
    පූර්ණ_සංඛ්‍යා		retval = FAIL;
    char_u	*title_save;

    නම් (di->di_tv.v_type != VAR_LIST)
	ආපසු FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    ආපසු retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    static පූර්ණ_සංඛ්‍යා
qf_setprop_items_from_lines(
	qf_info_T	*qi,
	පූර්ණ_සංඛ්‍යා		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	පූර්ණ_සංඛ්‍යා		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    පූර්ණ_සංඛ්‍යා		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    නම් ((efm_di = dict_find(what, (char_u *)"efm", -1)) != NULL)
    {
	නම් (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == NULL)
	    ආපසු FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    නම් (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == NULL)
	ආපසු FAIL;

    නම් (action == 'r' || action == 'u')
	qf_free_items(&qi->qf_lists[qf_idx]);
    නම් (qf_init_ext(qi, qf_idx, NULL, NULL, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, NULL, NULL) >= 0)
	retval = OK;

    ආපසු retval;
}

/*
 * Set quickfix list context.
 */
    static පූර්ණ_සංඛ්‍යා
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    නම් (ctx != NULL)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    ආපසු OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    static පූර්ණ_සංඛ්‍යා
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    පූර්ණ_සංඛ්‍යා		denote = FALSE;
    පූර්ණ_සංඛ්‍යා		newidx;
    පූර්ණ_සංඛ්‍යා		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    නම් (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != NULL
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_count;
    නැතහොත්
    {
	// Otherwise use the specified index
	newidx = tv_get_number_chk(&di->di_tv, &denote);
	නම් (denote)
	    ආපසු FAIL;
    }

    නම් (newidx < 1)		// sanity check
	ආපසු FAIL;
    නම් (newidx > qfl->qf_count)
	newidx = qfl->qf_count;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    නම් (qf_ptr == NULL)
	ආපසු FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    නම් (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    ආපසු OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    static පූර්ණ_සංඛ්‍යා
qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    නම් (check_restricted() || check_secure())
	ආපසු FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    නම් (cb.cb_name == NULL || *cb.cb_name == NUL)
	ආපසු OK;
    set_callback(&qfl->qf_qftf_cb, &cb);
    ආපසු OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    static පූර්ණ_සංඛ්‍යා
qf_set_properties(qf_info_T *qi, dict_T *what, පූර්ණ_සංඛ්‍යා action, char_u *title)
{
    dictitem_T	*di;
    පූර්ණ_සංඛ්‍යා		retval = FAIL;
    පූර්ණ_සංඛ්‍යා		qf_idx;
    පූර්ණ_සංඛ්‍යා		newlist = FALSE;
    qf_list_T	*qfl;

    නම් (action == ' ' || qf_stack_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    නම් (qf_idx == INVALID_QFIDX)	// List not found
	ආපසු FAIL;

    නම් (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    නම් ((di = dict_find(what, (char_u *)"title", -1)) != NULL)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    නම් ((di = dict_find(what, (char_u *)"items", -1)) != NULL)
	retval = qf_setprop_items(qi, qf_idx, di, action);
    නම් ((di = dict_find(what, (char_u *)"lines", -1)) != NULL)
	retval = qf_setprop_items_from_lines(qi, qf_idx, what, di, action);
    නම් ((di = dict_find(what, (char_u *)"context", -1)) != NULL)
	retval = qf_setprop_context(qfl, di);
    නම් ((di = dict_find(what, (char_u *)"idx", -1)) != NULL)
	retval = qf_setprop_curidx(qi, qfl, di);
    නම් ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != NULL)
	retval = qf_setprop_qftf(qi, qfl, di);

    නම් (newlist || retval == OK)
	qf_list_changed(qfl);
    නම් (newlist)
	qf_update_buffer(qi, NULL);

    ආපසු retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    static void
qf_free_stack(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_find_win(qi);
    win_T	*llwin = NULL;

    නම් (qfwin != NULL)
    {
	// If the quickfix/location list window is open, then clear it
	නම් (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_buffer(qi, NULL);
    }

    නම් (wp != NULL && IS_LL_WINDOW(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_find_win_with_loclist(qi);
	නම් (llwin != NULL)
	    wp = llwin;
    }

    qf_free_all(wp);
    නම් (wp == NULL)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    නැතහොත් නම් (qfwin != NULL)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_stack(QFLT_LOCATION, wp->w_p_lhi);

	නම් (new_ll != NULL)
	{
	    new_ll->qf_bufnr = qfwin->w_buffer->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    නම් (wp != qfwin)
		win_set_loclist(wp, new_ll);
	}
    }
}

/*
 * Populate the quickfix list with the items supplied in the list
 * of dictionaries. "title" will be copied to w:quickfix_title.
 * "action" is 'a' for add, 'r' for replace, 'u' for update.  Otherwise
 * create a new list. When "what" is not NULL then only set some properties.
 */
    පූර්ණ_සංඛ්‍යා
set_errorlist(
	win_T	*wp,
	list_T	*list,
	පූර්ණ_සංඛ්‍යා	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    පූර්ණ_සංඛ්‍යා		retval = OK;

    නම් (wp != NULL)
	qi = ll_get_or_alloc_list(wp);
    නැතහොත්
	qi = ql_info;
    නම් (qi == NULL)
	ආපසු FAIL;

    නම් (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_stack(wp, qi);
	ආපසු OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    නම් (list->lv_len != 0 && what != NULL)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	ආපසු FAIL;
    }

    incr_quickfix_busy();

    නම් (what != NULL)
	retval = qf_set_properties(qi, what, action, title);
    නැතහොත්
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	නම් (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    ආපසු retval;
}

static පූර්ණ_සංඛ්‍යා mark_quickfix_user_data(qf_info_T *qi, පූර්ණ_සංඛ්‍යා copyID)
{
    පූර්ණ_සංඛ්‍යා abort = FALSE;
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	නම් (!qfl->qf_has_user_data)
	    continue;
	qfline_T *qfp;
	පූර්ණ_සංඛ්‍යා j;
	FOR_ALL_QFL_ITEMS(qfl, qfp, j)
	{
	    typval_T* user_data = &qfp->qf_user_data;
	    නම් (user_data != NULL && user_data->v_type != VAR_NUMBER
		&& user_data->v_type != VAR_STRING && user_data->v_type != VAR_FLOAT)
		abort = abort || set_ref_in_item(user_data, copyID, NULL, NULL, NULL);
	}
    }
    ආපසු abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    static පූර්ණ_සංඛ්‍යා
mark_quickfix_ctx(qf_info_T *qi, පූර්ණ_සංඛ්‍යා copyID)
{
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    සඳහා (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	නම් (ctx != NULL && ctx->v_type != VAR_NUMBER
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || set_ref_in_item(ctx, copyID, NULL, NULL, NULL);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || set_ref_in_callback(cb, copyID);
    }

    ආපසු abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    පූර්ණ_සංඛ්‍යා
set_ref_in_quickfix(පූර්ණ_සංඛ්‍යා copyID)
{
    පූර්ණ_සංඛ්‍යා		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    නම් (ql_info == NULL)
	ආපසු TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    නම් (abort)
	ආපසු abort;

    abort = mark_quickfix_user_data(ql_info, copyID);
    නම් (abort)
	ආපසු abort;

    abort = set_ref_in_callback(&qftf_cb, copyID);
    නම් (abort)
	ආපසු abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	නම් (win->w_llist != NULL)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    නම් (abort)
		ආපසු abort;

	    abort = mark_quickfix_user_data(win->w_llist, copyID);
	    නම් (abort)
		ආපසු abort;
	}
	නම් (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    නම් (abort)
		ආපසු abort;

	    abort = mark_quickfix_user_data(win->w_llist_ref, copyID);
	    නම් (abort)
		ආපසු abort;
	}
    }

    ආපසු abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    static char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	pak_sa CMD_cbuffer:	ආපසු (char_u *)"cbuffer";
	pak_sa CMD_cgetbuffer:	ආපසු (char_u *)"cgetbuffer";
	pak_sa CMD_caddbuffer:	ආපසු (char_u *)"caddbuffer";
	pak_sa CMD_lbuffer:	ආපසු (char_u *)"lbuffer";
	pak_sa CMD_lgetbuffer:	ආපසු (char_u *)"lgetbuffer";
	pak_sa CMD_laddbuffer:	ආපසු (char_u *)"laddbuffer";
	default:		ආපසු NULL;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    static පූර්ණ_සංඛ්‍යා
cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = NULL;

    නම් (*eap->arg == NUL)
	buf = curbuf;
    නැතහොත් නම් (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((var_na *)eap->arg));

    නම් (buf == NULL)
    {
	emsg(_(e_invalid_argument));
	ආපසු FAIL;
    }

    නම් (buf->b_ml.ml_mfp == NULL)
    {
	emsg(_(e_buffer_is_not_loaded));
	ආපසු FAIL;
    }

    නම් (eap->addr_count == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_line_count;
    }

    නම් (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_line_count
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_line_count)
    {
	emsg(_(e_invalid_range));
	ආපසු FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    ආපසු OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    void
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = NULL;
    qf_info_T	*qi;
    char_u	*au_name = NULL;
    පූර්ණ_සංඛ්‍යා		res;
    int_u	save_qfid;
    win_T	*wp = NULL;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    නම් (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	නම් (aborting())
	    ආපසු;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_stack(eap, &wp);
    නම් (qi == NULL)
	ආපසු;

    නම් (cbuffer_process_args(eap, &buf, &line1, &line2) == FAIL)
	ආපසු;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    නම් (buf->b_sfname)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "%s (%s)",
		(var_na *)qf_title, (var_na *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, NULL, buf, NULL, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, NULL);
    නම් (qf_stack_empty(qi))
    {
	decr_quickfix_busy();
	ආපසු;
    }
    නම් (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    නම් (au_name != NULL)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	නම් (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    නම් (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# නම් defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	pak_sa CMD_cexpr:	    ආපසු (char_u *)"cexpr";
	pak_sa CMD_cgetexpr:  ආපසු (char_u *)"cgetexpr";
	pak_sa CMD_caddexpr:  ආපසු (char_u *)"caddexpr";
	pak_sa CMD_lexpr:	    ආපසු (char_u *)"lexpr";
	pak_sa CMD_lgetexpr:  ආපසු (char_u *)"lgetexpr";
	pak_sa CMD_laddexpr:  ආපසු (char_u *)"laddexpr";
	default:	    ආපසු NULL;
    }
}

    පූර්ණ_සංඛ්‍යා
trigger_cexpr_autocmd(පූර්ණ_සංඛ්‍යා cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    නම් (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	නම් (aborting())
	    ආපසු FAIL;
    }
    ආපසු OK;
}

    පූර්ණ_සංඛ්‍යා
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = NULL;

    qi = qf_cmd_get_or_alloc_stack(eap, &wp);
    නම් (qi == NULL)
	ආපසු FAIL;

    නම් ((tv->v_type == VAR_STRING && tv->vval.v_string != NULL)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != NULL))
    {
	පූර්ණ_සංඛ්‍යා	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, NULL, NULL, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), NULL);
	නම් (qf_stack_empty(qi))
	{
	    decr_quickfix_busy();
	    ආපසු FAIL;
	}
	නම් (res >= 0)
	    qf_list_changed(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	නම් (au_name != NULL)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	නම් (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_first(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	ආපසු OK;
    }

    emsg(_(e_string_or_list_expected));
    ආපසු FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    void
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    නම් (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	ආපසු;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_expr(eap->arg, eap);
    නම් (tv == NULL)
	ආපසු;

    (void)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    static qf_info_T *
hgr_get_ll(පූර්ණ_සංඛ්‍යා *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    නම් (bt_help(curwin->w_buffer))
	wp = curwin;
    නැතහොත්
	// Find an existing help window
	wp = qf_find_help_win();

    නම් (wp == NULL)	    // Help window not found
	qi = NULL;
    නැතහොත්
	qi = wp->w_llist;

    නම් (qi == NULL)
    {
	// Allocate a new location list for help text matches
	නම් ((qi = qf_alloc_stack(QFLT_LOCATION, 1)) == NULL)
	    ආපසු NULL;
	*new_ll = TRUE;
    }

    ආපසු qi;
}

/*
 * Search for a pattern in a help file.
 */
    static void
hgr_search_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    long	lnum;

    fd = mch_fopen((var_na *)fname, "r");
    නම් (fd == NULL)
	ආපසු;

    lnum = 1;
    අතරතුර (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *line = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	නම් (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    line = string_convert(p_vc, IObuff, NULL);
	    නම් (line == NULL)
		line = IObuff;
	}

	නම් (vim_regexec(p_regmatch, line, (colnr_T)0))
	{
	    පූර්ණ_සංඛ්‍යා	l = (පූර්ණ_සංඛ්‍යා)STRLEN(line);

	    // remove trailing CR, LF, spaces, etc.
	    අතරතුර (l > 0 && line[l - 1] <= ' ')
		line[--l] = NUL;

	    නම් (qf_add_entry(qfl,
			NULL,	// dir
			fname,
			NULL,
			0,
			line,
			lnum,
			0,
			(පූර්ණ_සංඛ්‍යා)(p_regmatch->startp[0] - line)
			+ 1,	// col
			(පූර්ණ_සංඛ්‍යා)(p_regmatch->endp[0] - line)
			+ 1,	// end_col
			FALSE,	// vis_col
			NULL,	// search pattern
			0,	// nr
			1,	// type
			NULL,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		නම් (line != IObuff)
		    vim_free(line);
		bha_nga;
	    }
	}
	නම් (line != IObuff)
	    vim_free(line);
	++lnum;
	line_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    static void
hgr_search_files_in_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    පූර්ණ_සංඛ්‍යා		fcount;
    char_u	**fnames;
    පූර්ණ_සංඛ්‍යා		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    add_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    නම් (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	සඳහා (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    නම් (lang != NULL
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		continue;
# endif

	    hgr_search_file(qfl, fnames[fi], p_vc, p_regmatch);
	}
	FreeWild(fcount, fnames);
    }
}

/*
 * Search for a pattern in all the help files in the 'runtimepath'
 * and add the matches to a quickfix list.
 * 'lang' is the language specifier.  If supplied, then only matches in the
 * specified language are found.
 */
    static void
hgr_search_in_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    නම් (!enc_utf8)
	convert_setup(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    අතරතුර (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_search_files_in_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    නම් (vc.vc_type != CONV_NONE)
	convert_setup(&vc, NULL, NULL);
}

/*
 * ":helpgrep {pattern}"
 */
    void
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    පූර්ණ_සංඛ්‍යා		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    පූර්ණ_සංඛ්‍යා		new_qi = FALSE;
    char_u	*au_name =  NULL;
    char_u	*lang = NULL;
    පූර්ණ_සංඛ්‍යා		updated = FALSE;

    switch (eap->cmdidx)
    {
	pak_sa CMD_helpgrep:  au_name = (char_u *)"helpgrep"; bha_nga;
	pak_sa CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; bha_nga;
	default: bha_nga;
    }
    නම් (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	නම් (aborting())
	    ආපසු;
# endif
    }

    නම් (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	නම් (qi == NULL)
	    ආපසු;
    }
    නැතහොත් නම් (qi == NULL)
    {
	emsg(_(e_no_quickfix_stack));
	ආපසු;
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    save_cpo_allocated = is_option_allocated("cpo");
    p_cpo = empty_option;

    incr_quickfix_busy();

# ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(eap->arg);
# endif
    regmatch.regprog = vim_regcomp(eap->arg, RE_MAGIC + RE_STRING);
    regmatch.rm_ic = FALSE;
    නම් (regmatch.regprog != NULL)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_search_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    නම් (p_cpo == empty_option)
	p_cpo = save_cpo;
    නැතහොත්
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	නම් (*p_cpo == NUL)
	    set_option_value_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	නම් (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    නම් (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, NULL);

    නම් (au_name != NULL)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	නම් (!new_qi && IS_LL_STACK(qi) && qf_find_win_with_loclist(qi) == NULL)
	{
	    decr_quickfix_busy();
	    ආපසු;
	}
    }

    // Jump to first match.
    නම් (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    නැතහොත්
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    නම් (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	නම් (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    නම් (new_qi)
		ll_free_all(&qi);
	}
	නැතහොත් නම් (curwin->w_llist == NULL && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# නම් defined(EXITFREE)
    void
free_quickfix(void)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(NULL);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#endif // FEAT_QUICKFIX

#නම් defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    static void
get_qf_loc_list(පූර්ණ_සංඛ්‍යා is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    නම් (what_arg->v_type == VAR_UNKNOWN)
    {
	නම් (rettv_list_alloc(rettv) == OK)
	    නම් (is_qf || wp != NULL)
		(void)get_errorlist(NULL, wp, -1, 0, rettv->vval.v_list);
    }
    නැතහොත්
    {
	නම් (rettv_dict_alloc(rettv) == OK)
	    නම් (is_qf || (wp != NULL))
	    {
		නම් (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    නම් (d != NULL)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		නැතහොත්
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    void
f_getloclist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    නම් (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ආපසු;

    wp = find_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    void
f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    නම් (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ආපසු;

    get_qf_loc_list(TRUE, NULL, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    static void
set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    පූර්ණ_සංඛ්‍යා		action = 0;
    static පූර්ණ_සංඛ්‍යා	recursive = 0;
# endif

    rettv->vval.v_number = -1;

# ifdef FEAT_QUICKFIX
    නම් (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    නැතහොත් නම් (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    නැතහොත්
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = NULL;
	පූර්ණ_සංඛ්‍යා	valid_dict = TRUE;

	නම් (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    නම් (act == NULL)
		ආපසු;		// type error; errmsg already given
	    නම් ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    නැතහොත්
		semsg(_(e_invalid_action_str_1), act);
	}
	නැතහොත් නම් (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	නැතහොත්
	    emsg(_(e_string_required));

	නම් (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    නම් (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != NULL)
		what = what_arg->vval.v_dict;
	    නැතහොත්
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	නම් (l != NULL && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == NULL ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_number = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    void
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_number = -1;

    නම් (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	ආපසු;

    win = find_win_by_nr_or_id(&argvars[0]);
    නම් (win != NULL)
	set_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    void
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    නම් (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ආපසු;

    set_qf_ll_list(NULL, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#endif
