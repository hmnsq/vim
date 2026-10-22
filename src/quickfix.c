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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_QUICKFIX)

ସଂରଚନା dir_ଗଦା_T
{
    ସଂରଚନା dir_ଗଦା_T	*next;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା qfline_S qfline_T;
ସଂରଚନା qfline_S
{
    qfline_T	*qf_next;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_end_lnum;	// line number when the error has range or zero
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_fnum;	// file number for the line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_col;		// column where the error occurred
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_end_col;	// column when the error has range or zero
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_nr;		// error number
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_start;	// pointer to the first error
    qfline_T	*qf_last;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_ଗଣନା;	// number of errors (0 means empty list)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_index;	// current index in the error list
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_nonevalid;	// TRUE if not a single valid entry found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_has_user_data; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    ସଂରଚନା dir_ଗଦା_T	*qf_dir_ଗଦା;
    char_u		*qf_directory;
    ସଂରଚନା dir_ଗଦା_T	*qf_file_ଗଦା;
    char_u		*qf_currfile;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			qf_multiline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			qf_multiignore;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			qf_multiscan;
    long		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
ସଂରଚନା qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_refcount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_listcount;	    // current number of lists
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_curlist;	    // current error list
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_bufnr;	    // quickfix window buffer number
};

ସ୍ଥିର qf_info_T ql_info_actual; // global quickfix list
ସ୍ଥିର qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
ସ୍ଥିର int_u last_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା efm_S efm_T;
ସଂରଚନା efm_S
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା qf_delq_S
{
    ସଂରଚନା qf_delq_S	*next;
    qf_info_T		*qi;
} qf_delq_T;
ସ୍ଥିର qf_delq_T *qf_delq_head = ଖାଲି;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	quickfix_busy = 0;

ସ୍ଥିର efm_T	*fmt_start = ଖାଲି; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
ସ୍ଥିର callback_T qftf_cb;

ସ୍ଥିର ଶୂନ୍ୟ     qf_କାଢ଼_ଗଦା(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା adjust);
ସ୍ଥିର ଶୂନ୍ୟ	qf_new_list(qf_info_T *qi, char_u *qf_title);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	qf_add_entry(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufnum, char_u *mesg, long lnum, long end_lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vis_col, char_u *pattern, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, typval_T *user_data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା valid);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା      qf_resize_ଗଦା_base(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
ସ୍ଥିର ଶୂନ୍ୟ     qf_sync_llw_to_win(win_T *llw);
ସ୍ଥିର ଶୂନ୍ୟ     qf_sync_win_to_llw(win_T *pwp);
ସ୍ଥିର qf_info_T *qf_alloc_ଗଦା(qfltype_T qfltype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
ସ୍ଥିର qf_list_T *qf_alloc_list_ଗଦା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
ସ୍ଥିର ଶୂନ୍ୟ	qf_free(qf_list_T *qfl);
ସ୍ଥିର char_u	*qf_types(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
ସ୍ଥିର char_u	*qf_ଠେଲ_dir(char_u *, ସଂରଚନା dir_ଗଦା_T **, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_file_ଗଦା);
ସ୍ଥିର char_u	*qf_କାଢ଼_dir(ସଂରଚନା dir_ଗଦା_T **);
ସ୍ଥିର char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
ସ୍ଥିର win_T    *qf_find_win_with_loclist(qf_info_T *ll);
ସ୍ଥିର ଶୂନ୍ୟ	qf_jump_newwin(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା errornr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newwin);
ସ୍ଥିର ଶୂନ୍ୟ	qf_fmt_text(garray_T *gap, char_u *text);
ସ୍ଥିର ଶୂନ୍ୟ	qf_range_text(garray_T *gap, qfline_T *qfp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	qf_win_pos_update(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_qf_index);
ସ୍ଥିର win_T	*qf_find_win(qf_info_T *qi);
ସ୍ଥିର buf_T	*qf_find_buf(qf_info_T *qi);
ସ୍ଥିର ଶୂନ୍ୟ	qf_update_buffer(qf_info_T *qi, qfline_T *old_last);
ସ୍ଥିର ଶୂନ୍ୟ	qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_winid);
ସ୍ଥିର buf_T	*load_dummy_buffer(char_u *fname, char_u *dirname_start, char_u *resulting_dir);
ସ୍ଥିର ଶୂନ୍ୟ	wipe_dummy_buffer(buf_T *buf, char_u *dirname_start);
ସ୍ଥିର ଶୂନ୍ୟ	unload_dummy_buffer(buf_T *buf, char_u *dirname_start);
ସ୍ଥିର qf_info_T *ll_get_or_alloc_list(win_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	entry_is_closer_to_target(qfline_T *entry, qfline_T *other_entry, ପୂର୍ଣ୍ଣସଂଖ୍ୟା target_fnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା target_lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା target_col);

// Quickfix window check helper macro
# define IS_QF_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref == ଖାଲି)
// Location list window check helper macro
# define IS_LL_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref != ଖାଲି)

// Quickfix and location list stack check helper macros
# define IS_QF_ଗଦା(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_ଗଦା(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_WINDOW(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(qfl, qfp, i) \
		    ପାଇଁ ((i) = 1, (qfp) = (qfl)->qf_start; \
			    !got_int && (i) <= (qfl)->qf_ଗଣନା && (qfp) != ଖାଲି; \
			    ++(i), (qfp) = (qfp)->qf_next)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
ସ୍ଥିର char_u   *qf_last_bufname = ଖାଲି;
ସ୍ଥିର bufref_T  qf_last_bufref = {ଖାଲି, 0, 0};

ସ୍ଥିର garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    ସ୍ଥିର garray_T *
qfga_get(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା initialized = FALSE;

    ଯଦି (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    ଫେରନ୍ତୁ &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qfga_clear(ଶୂନ୍ୟ)
{
    ଯଦି (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    ଅନ୍ୟଥା
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define ଧାଡ଼ି_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
ସ୍ଥିର ସଂରଚନା fmtpattern
{
    char_u	convchar;
    ଅକ୍ଷର	*pattern;
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
    ସ୍ଥିର char_u *
efmpat_to_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	round)
{
    char_u	*srcptr;

    ଯଦି (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_in_format_string), *efmpat);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି ((idx && idx < FMT_PATTERN_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->prefix) != ଖାଲି)
	    || (idx == FMT_PATTERN_R
		&& vim_strchr((char_u *)"OPQ", efminfo->prefix) == ଖାଲି))
    {
	semsg(_(e_unexpected_chr_in_format_str), *efmpat);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    efminfo->addr[idx] = (char_u)++round;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_IN_FILENAME
    ଯଦି (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    ଯଦି (*efmpat == 'f' && efmpat[1] != NUL)
    {
	ଯଦି (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	ଅନ୍ୟଥା
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    ଅନ୍ୟଥା
    {
	srcptr = (char_u *)fmt_pat[idx].pattern;
	ଯେତେବେଳେ ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    ଫେରନ୍ତୁ regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    ସ୍ଥିର char_u *
scanf_fmt_to_regpat(
	char_u	**pefmp,
	char_u	*efm,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    ଯଦି (*efmp == '[' || *efmp == '\\')
    {
	ଯଦି ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    ଯଦି (efmp[1] == '^')
		*regpat++ = *++efmp;
	    ଯଦି (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		ଯେତେବେଳେ (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		ଯଦି (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_in_format_string));
		    ଫେରନ୍ତୁ ଖାଲି;
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    ଅନ୍ୟଥା
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_in_format_string), *efmp);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    *pefmp = efmp;

    ଫେରନ୍ତୁ regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    ସ୍ଥିର char_u *
efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    ଯଦି (vim_strchr((char_u *)"+-", *efmp) != ଖାଲି)
	efminfo->flags = *efmp++;
    ଯଦି (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != ଖାଲି)
	efminfo->prefix = *efmp;
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
efm_to_regpat(
	char_u	*efm,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    ପାଇଁ (efmp = efm; efmp < efm + len; ++efmp)
    {
	ଯଦି (*efmp == '%')
	{
	    ++efmp;
	    ପାଇଁ (idx = 0; idx < FMT_PATTERNS; ++idx)
		ଯଦି (fmt_pat[idx].convchar == *efmp)
		    ଭାଙ୍ଗ;
	    ଯଦି (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		ଯଦି (ptr == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		round++;
	    }
	    ଅନ୍ୟଥା ଯଦି (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		ଯଦି (ptr == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)"%\\.^$~[", *efmp) != ଖାଲି)
		*ptr++ = *efmp;		// regexp magic characters
	    ଅନ୍ୟଥା ଯଦି (*efmp == '#')
		*ptr++ = '*';
	    ଅନ୍ୟଥା ଯଦି (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    ଅନ୍ୟଥା ଯଦି (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		ଯଦି (efmp == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା			// copy normal character
	{
	    ଯଦି (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)".*^$~[", *efmp) != ଖାଲି)
		*ptr++ = '\\';	// escape regexp atoms
	    ଯଦି (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Free the 'errorformat' information list
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_efm_list(efm_T **efm_first)
{
    efm_T *efm_ptr;

    ପାଇଁ (efm_ptr = *efm_first; efm_ptr != ଖାଲି; efm_ptr = *efm_first)
    {
	*efm_first = efm_ptr->next;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_start = ଖାଲି;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
efm_regpat_bufsz(char_u *efm)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    sz = (FMT_PATTERNS * 3) + ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(efm) << 2);
    ପାଇଁ (i = FMT_PATTERNS; i > 0; )
	sz += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(fmt_pat[--i].pattern);
# ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# ଅନ୍ୟଥା
    sz += 2; // "%f" can become two chars longer
# endif

    ଫେରନ୍ତୁ sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
efm_option_part_len(char_u *efm)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

    ପାଇଁ (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	ଯଦି (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    ଫେରନ୍ତୁ len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    ସ୍ଥିର efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = ଖାଲି;
    efm_T	*fmt_first = ଖାଲି;
    efm_T	*fmt_last = ଖାଲି;
    char_u	*fmtstr = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    ଯଦି ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == ଖାଲି)
	ଯାଅ parse_efm_error;

    ଯେତେବେଳେ (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	ଯଦି (fmt_ptr == ଖାଲି)
	    ଯାଅ parse_efm_error;
	ଯଦି (fmt_first == ଖାଲି)	    // first one
	    fmt_first = fmt_ptr;
	ଅନ୍ୟଥା
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	ଯଦି (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    ଯାଅ parse_efm_error;
	ଯଦି ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == ଖାଲି)
	    ଯାଅ parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    ଯଦି (fmt_first == ଖାଲି)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    ଯାଅ parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    ଫେରନ୍ତୁ fmt_first;
}

ଗଣନା {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_END_OF_ନିବେଶ = 2,
    QF_NOMEM = 3,
    QF_IGNORE_ଧାଡ଼ି = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    char_u	*linebuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linelen;
    char_u	*growbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		growbufsiz;
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
    ସ୍ଥିର char_u *
qf_grow_linebuf(qfstate_T *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > ଧାଡ଼ି_MAXLEN ? ଧାଡ଼ି_MAXLEN - 1 : newsz;
    ଯଦି (state->growbuf == ଖାଲି)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	ଯଦି (state->growbuf == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	state->growbufsiz = state->linelen;
    }
    ଅନ୍ୟଥା ଯଦି (state->linelen > state->growbufsiz)
    {
	ଯଦି ((p = vim_realloc(state->growbuf, state->linelen + 1)) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    ଫେରନ୍ତୁ state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_next_str_ଧାଡ଼ି(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (*p_str == NUL) // Reached the end of the string
	ଫେରନ୍ତୁ QF_END_OF_ନିବେଶ;

    p = vim_strchr(p_str, '\n');
    ଯଦି (p != ଖାଲି)
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - p_str) + 1;
    ଅନ୍ୟଥା
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p_str);

    ଯଦି (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ଯଦି (state->linebuf == ଖାଲି)
	    ଫେରନ୍ତୁ QF_NOMEM;
    }
    ଅନ୍ୟଥା
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_next_list_ଧାଡ଼ି(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯେତେବେଳେ (p_li != ଖାଲି
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == ଖାଲି))
	p_li = p_li->li_next;	// Skip non-string items

    ଯଦି (p_li == ଖାଲି)		// End of the list
    {
	state->p_li = ଖାଲି;
	ଫେରନ୍ତୁ QF_END_OF_ନିବେଶ;
    }

    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p_li->li_tv.vval.v_string);
    ଯଦି (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ଯଦି (state->linebuf == ଖାଲି)
	    ଫେରନ୍ତୁ QF_NOMEM;
    }
    ଅନ୍ୟଥା
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_next;	// next item
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_next_buf_ଧାଡ଼ି(qfstate_T *state)
{
    char_u	*p_buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    // Get the next line from the supplied buffer
    ଯଦି (state->buflnum > state->lnumlast)
	ଫେରନ୍ତୁ QF_END_OF_ନିବେଶ;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    ଯଦି (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ଯଦି (state->linebuf == ଖାଲି)
	    ଫେରନ୍ତୁ QF_NOMEM;
    }
    ଅନ୍ୟଥା
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_next_file_ଧାଡ଼ି(qfstate_T *state)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    discard;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    growbuflen;

    ଯଦି (fgets((ଅକ୍ଷର *)IObuff, IOSIZE, state->fd) == ଖାଲି)
	ଫେରନ୍ତୁ QF_END_OF_ନିବେଶ;

    discard = FALSE;
    state->linelen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(IObuff);
    ଯଦି (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	ଯଦି (state->growbuf == ଖାଲି)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    ଯଦି (state->growbuf == ଖାଲି)
		ଫେରନ୍ତୁ QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	memcpy(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	ପାଇଁ (;;)
	{
	    char_u	*p;

	    ଯଦି (fgets((ଅକ୍ଷର *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == ଖାଲି)
		ଭାଙ୍ଗ;
	    state->linelen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    ଯଦି ((state->growbuf)[growbuflen - 1] == '\n')
		ଭାଙ୍ଗ;
	    ଯଦି (state->growbufsiz == ଧାଡ଼ି_MAXLEN)
	    {
		discard = TRUE;
		ଭାଙ୍ଗ;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < ଧାଡ଼ି_MAXLEN
		? 2 * state->growbufsiz : ଧାଡ଼ି_MAXLEN;
	    ଯଦି ((p = vim_realloc(state->growbuf, state->growbufsiz)) == ଖାଲି)
		ଫେରନ୍ତୁ QF_NOMEM;
	    state->growbuf = p;
	}

	ଯେତେବେଳେ (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    ଯଦି (fgets((ଅକ୍ଷର *)IObuff, IOSIZE, state->fd) == ଖାଲି
		    || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		ଭାଙ୍ଗ;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    ଅନ୍ୟଥା
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    ଯଦି (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*ଧାଡ଼ି;

	ଧାଡ଼ି = string_convert(&state->vc, state->linebuf, &state->linelen);
	ଯଦି (ଧାଡ଼ି != ଖାଲି)
	{
	    ଯଦି (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, ଧାଡ଼ି);
		vim_free(ଧାଡ଼ି);
	    }
	    ଅନ୍ୟଥା
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = ଧାଡ଼ି;
		state->growbufsiz = state->linelen < ଧାଡ଼ି_MAXLEN
						? state->linelen : ଧାଡ଼ି_MAXLEN;
	    }
	}
    }

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_nextline(qfstate_T *state)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status = QF_FAIL;

    ଯଦି (state->fd == ଖାଲି)
    {
	ଯଦି (state->tv != ଖାଲି)
	{
	    ଯଦି (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_ଧାଡ଼ି(state);
	    ଅନ୍ୟଥା ଯଦି (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_ଧାଡ଼ି(state);
	}
	ଅନ୍ୟଥା
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_ଧାଡ଼ି(state);
    }
    ଅନ୍ୟଥା
	// Get the next line from the supplied file
	status = qf_get_next_file_ଧାଡ଼ି(state);

    ଯଦି (status != QF_OK)
	ଫେରନ୍ତୁ status;

    // remove newline/CR from the line
    ଯଦି (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	ଯଦି (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    ଫେରନ୍ତୁ QF_OK;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    char_u	*namebuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bnr;
    char_u	*module;
    char_u	*errmsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errmsglen;
    long	lnum;
    long	end_lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_col;
    char_u	use_viscol;
    char_u	*pattern;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		enr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;
    typval_T	*user_data;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_f(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prefix)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

    ଯଦି (rmp->startp[midx] == ଖାଲି || rmp->endp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    ଯଦି (vim_strchr((char_u *)"OPQ", prefix) != ଖାଲି
	    && mch_getperm(fields->namebuf) == -1)
	ଫେରନ୍ତୁ QF_FAIL;

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_b(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ଯଦି (rmp->startp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bnr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)atol((ଅକ୍ଷର *)rmp->startp[midx]);
    ଯଦି (buflist_findnr(bnr) == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    fields->bnr = bnr;
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_n(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ଯଦି (rmp->startp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    fields->enr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)atol((ଅକ୍ଷର *)rmp->startp[midx]);
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_l(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ଯଦି (rmp->startp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    fields->lnum = atol((ଅକ୍ଷର *)rmp->startp[midx]);
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_e(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ଯଦି (rmp->startp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    fields->end_lnum = atol((ଅକ୍ଷର *)rmp->startp[midx]);
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_c(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ଯଦି (rmp->startp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    fields->col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)atol((ଅକ୍ଷର *)rmp->startp[midx]);
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_k(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ଯଦି (rmp->startp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    fields->end_col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)atol((ଅକ୍ଷର *)rmp->startp[midx]);
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_t(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ଯଦି (rmp->startp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    fields->type = *rmp->startp[midx];
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_nonerror_ଧାଡ଼ି(char_u *linebuf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା linelen, qffields_T *fields)
{
    char_u	*p;

    ଯଦି (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	ଯଦି ((p = vim_realloc(fields->errmsg, linelen + 1)) == ଖାଲି)
	    ଫେରନ୍ତୁ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_m(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (rmp->startp[midx] == ଖାଲି || rmp->endp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rmp->endp[midx] - rmp->startp[midx]);
    ଯଦି (len >= fields->errmsglen)
    {
	// len + null terminator
	ଯଦି ((p = vim_realloc(fields->errmsg, len + 1)) == ଖାଲି)
	    ଫେରନ୍ତୁ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_r(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, char_u **tail)
{
    ଯଦି (rmp->startp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    *tail = rmp->startp[midx];
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_p(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    char_u	*match_ptr;

    ଯଦି (rmp->startp[midx] == ଖାଲି || rmp->endp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    fields->col = 0;
    ପାଇଁ (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	ଯଦି (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_v(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ଯଦି (rmp->startp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    fields->col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)atol((ଅକ୍ଷର *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_s(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (rmp->startp[midx] == ଖାଲି || rmp->endp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rmp->endp[midx] - rmp->startp[midx]);
    ଯଦି (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_fmt_o(regmatch_T *rmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା midx, qffields_T *fields)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (rmp->startp[midx] == ଖାଲି || rmp->endp[midx] == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rmp->endp[midx] - rmp->startp[midx]);
    ଯଦି (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    ଫେରନ୍ତୁ QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, qffields_T *) =
{
    ଖାଲି, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    ଖାଲି, // %r
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_match(
	char_u		*linebuf,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_multiline,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_multiscan,
	char_u		**tail)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = fmt_ptr->prefix;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		midx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;

    ଯଦି ((idx == 'C' || idx == 'Z') && !qf_multiline)
	ଫେରନ୍ତୁ QF_FAIL;
    ଯଦି (vim_strchr((char_u *)"EWIN", idx) != ଖାଲି)
	fields->type = idx;
    ଅନ୍ୟଥା
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    ପାଇଁ (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fmt_ptr->addr[i];
	ଯଦି (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	ଅନ୍ୟଥା ଯଦି (i == FMT_PATTERN_M)
	{
	    ଯଦି (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_ଧାଡ଼ି(linebuf, linelen, fields);
	    ଅନ୍ୟଥା ଯଦି (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	ଅନ୍ୟଥା ଯଦି (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	ଅନ୍ୟଥା ଯଦି (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	ଯଦି (status != QF_OK)
	    ଫେରନ୍ତୁ status;
    }

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_get_fields(
	char_u		*linebuf,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_multiline,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = QF_FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    ଯଦି (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    ଯଦି (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = ଖାଲି;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    ଯଦି (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    ଫେରନ୍ତୁ status;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_dir_pfx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, qffields_T *fields, qf_list_T *qfl)
{
    ଯଦି (idx == 'D')				// enter directory
    {
	ଯଦି (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    ଫେରନ୍ତୁ QF_FAIL;
	}
	qfl->qf_directory =
	    qf_ଠେଲ_dir(fields->namebuf, &qfl->qf_dir_ଗଦା, FALSE);
	ଯଦି (qfl->qf_directory == ଖାଲି)
	    ଫେରନ୍ତୁ QF_FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (idx == 'X')			// leave directory
	qfl->qf_directory = qf_କାଢ଼_dir(&qfl->qf_dir_ଗଦା);

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_file_pfx(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    ଯଦି (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	ଯଦି (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_ଠେଲ_dir(fields->namebuf, &qfl->qf_file_ଗଦା, TRUE);
	ଅନ୍ୟଥା ଯଦି (idx == 'Q')
	    qfl->qf_currfile = qf_କାଢ଼_dir(&qfl->qf_file_ଗଦା);
	*fields->namebuf = NUL;
	ଯଦି (tail && *tail)
	{
	    qfl->qf_multiscan = TRUE;
	    ଫେରନ୍ତୁ QF_MULTISCAN;
	}
    }

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_ଧାଡ଼ି_nomatch(char_u *linebuf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    ଫେରନ୍ତୁ copy_nonerror_ଧାଡ଼ି(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_multiline_pfx(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len;

    ଯଦି (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	ଯଦି (qfprev == ଖାଲି)
	    ଫେରନ୍ତୁ QF_FAIL;
	ଯଦି (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    ଯଦି (ptr == ଖାଲି)
		ଫେରନ୍ତୁ QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	ଯଦି (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	ଯଦି (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	ଯଦି (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	ଯଦି (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	ଯଦି (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	ଯଦି (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	ଯଦି (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != ଖାଲି
		    ? fields->namebuf
		    : qfl->qf_currfile != ଖାଲି && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    ଯଦି (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    ଧାଡ଼ି_breakcheck();

    ଫେରନ୍ତୁ QF_IGNORE_ଧାଡ଼ି;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_parse_ଧାଡ଼ି(
	qf_list_T	*qfl,
	char_u		*linebuf,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linelen,
	efm_T		*fmt_first,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			idx = 0;
    char_u		*tail = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			status;

restofline:
    // If there was no %> item start at the first pattern
    ଯଦି (fmt_start == ଖାଲି)
	fmt_ptr = fmt_first;
    ଅନ୍ୟଥା
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_start;
	fmt_start = ଖାଲି;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    ପାଇଁ ( ; fmt_ptr != ଖାଲି; fmt_ptr = fmt_ptr->next)
    {
	idx = fmt_ptr->prefix;
	status = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &tail);
	ଯଦି (status == QF_NOMEM)
	    ଫେରନ୍ତୁ status;
	ଯଦି (status == QF_OK)
	    ଭାଙ୍ଗ;
    }
    qfl->qf_multiscan = FALSE;

    ଯଦି (fmt_ptr == ଖାଲି || idx == 'D' || idx == 'X')
    {
	ଯଦି (fmt_ptr != ଖାଲି)
	{
	    // 'D' and 'X' directory specifiers
	    status = qf_parse_dir_pfx(idx, fields, qfl);
	    ଯଦି (status != QF_OK)
		ଫେରନ୍ତୁ status;
	}

	status = qf_parse_ଧାଡ଼ି_nomatch(linebuf, linelen, fields);
	ଯଦି (status != QF_OK)
	    ଫେରନ୍ତୁ status;

	ଯଦି (fmt_ptr == ଖାଲି)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (fmt_ptr != ଖାଲି)
    {
	// honor %> item
	ଯଦି (fmt_ptr->conthere)
	    fmt_start = fmt_ptr;

	ଯଦି (vim_strchr((char_u *)"AEWIN", idx) != ଖାଲି)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)"CZ", idx) != ଖାଲି)
	{				// continuation of multi-line msg
	    status = qf_parse_multiline_pfx(idx, qfl, fields);
	    ଯଦି (status != QF_OK)
		ଫେରନ୍ତୁ status;
	}
	ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)"OPQ", idx) != ଖାଲି)
	{				// global file names
	    status = qf_parse_file_pfx(idx, fields, qfl, tail);
	    ଯଦି (status == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(tail);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_linelen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
		ଯଦି (new_linelen >= linelen)
		    ଫେରନ୍ତୁ QF_IGNORE_ଧାଡ଼ି;
		linebuf = s;
		linelen = new_linelen;
		ଯାଅ restofline;
	    }
	}
	ଯଦି (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    ଯଦି (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    ଫେରନ୍ତୁ QF_IGNORE_ଧାଡ଼ି;
	}
    }

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_ଗଦା_empty(qf_info_T *qi)
{
    ଫେରନ୍ତୁ qi == ଖାଲି || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_list_empty(qf_list_T *qfl)
{
    ଫେରନ୍ତୁ qfl == ଖାଲି || qfl->qf_ଗଣନା <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_list_has_valid_entries(qf_list_T *qfl)
{
    ଫେରନ୍ତୁ !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    ସ୍ଥିର qf_list_T *
qf_get_list(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->pattern = alloc_id(CMDBUFFSIZE + 1, aid_qf_pattern);
    ଯଦି (pfields->namebuf == ଖାଲି || pfields->errmsg == ଖାଲି
		|| pfields->pattern == ଖାଲି || pfields->module == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    ଯଦି (enc != ଖାଲି && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    ଯଦି (efile != ଖାଲି && (pstate->fd = mch_fopen((ଅକ୍ଷର *)efile, "r")) == ଖାଲି)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (tv != ଖାଲି)
    {
	ଯଦି (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    ଫେରନ୍ତୁ OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_cleanup_state(qfstate_T *pstate)
{
    ଯଦି (pstate->fd != ଖାଲି)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    ଯଦି (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, ଖାଲି, ଖାଲି);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    ଯଦି (status != QF_OK)
	ଫେରନ୍ତୁ status;

    status = qf_parse_ଧାଡ଼ି(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    ଯଦି (status != QF_OK)
	ଫେରନ୍ତୁ status;

    ଫେରନ୍ତୁ qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != ଖାଲି)
		? fields->namebuf
		: ((qfl->qf_currfile != ଖାଲି && fields->valid)
		    ? qfl->qf_currfile : (char_u *)ଖାଲି),
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_init_ext(
    qf_info_T	    *qi,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_last = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    adding = FALSE;
    ସ୍ଥିର efm_T    *fmt_first = ଖାଲି;
    char_u	    *efm;
    ସ୍ଥିର char_u   *last_efm = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    retval = -1;	// default: return error flag
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    status;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_last_bufname);

    CLEAR_FIELD(state);
    CLEAR_FIELD(fields);
    ଯଦି ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_setup_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	ଯାଅ qf_init_end;

    ଯଦି (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ଅନ୍ୟଥା
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	ଯଦି (!qf_list_empty(qfl))
	    old_last = qfl->qf_last;
    }

    // Use the local value of 'errorformat' if it's set.
    ଯଦି (errorformat == p_efm && tv == ଖାଲି && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    ଅନ୍ୟଥା
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    ଯଦି (last_efm == ଖାଲି || (STRCMP(last_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(last_efm);
	free_efm_list(&fmt_first);

	// parse the current 'efm'
	fmt_first = parse_efm_option(efm);
	ଯଦି (fmt_first != ଖାଲି)
	    last_efm = vim_strsave(efm);
    }

    ଯଦି (fmt_first == ଖାଲି)	// nothing found
	ଯାଅ error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    ଯେତେବେଳେ (!got_int)
    {
	status = qf_init_process_nextline(qfl, fmt_first, &state, &fields);
	ଯଦି (status == QF_NOMEM)		// memory alloc failure
	    ଯାଅ qf_init_end;
	ଯଦି (status == QF_END_OF_ନିବେଶ)	// end of input
	    ଭାଙ୍ଗ;
	ଯଦି (status == QF_FAIL)
	    ଯାଅ error2;

	ଧାଡ଼ି_breakcheck();
    }
    ଯଦି (state.fd == ଖାଲି || !ferror(state.fd))
    {
	ଯଦି (qfl->qf_index == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_start;
	    qfl->qf_index = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    qfl->qf_nonevalid = FALSE;
	    ଯଦି (qfl->qf_ptr == ଖାଲି)
		qfl->qf_ptr = qfl->qf_start;
	}
	// return number of matches
	retval = qfl->qf_ଗଣନା;
	ଯାଅ qf_init_end;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    ଯଦି (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	ଯଦି (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_end:
    ଯଦି (qf_idx == qi->qf_curlist)
	qf_update_buffer(qi, old_last);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    ଫେରନ୍ତୁ retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    ଯଦି (wp != ଖାଲି)
	qi = ll_get_or_alloc_list(wp);
    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ qf_init_ext(qi, qi->qf_curlist, efile, curbuf, ଖାଲି, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    ଯଦି (title == ଖାଲି)
	ଫେରନ୍ତୁ;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    ଯଦି (p != ଖାଲି)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    ସ୍ଥିର char_u *
qf_cmdtitle(char_u *cmd)
{
    ସ୍ଥିର char_u qftitle_str[IOSIZE];

    vim_snprintf((ଅକ୍ଷର *)qftitle_str, IOSIZE, ":%s", (ଅକ୍ଷର *)cmd);
    ଫେରନ୍ତୁ qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    ସ୍ଥିର qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    ଫେରନ୍ତୁ qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_କାଢ଼_ଗଦା(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା adjust)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    qf_free(&qi->qf_lists[0]);
    ପାଇଁ (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, ଆକାର(*qi->qf_lists));

    ଯଦି (adjust)
    {
	qi->qf_listcount--;
	ଯଦି (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	ଅନ୍ୟଥା
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    ଯେତେବେଳେ (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    ଯଦି (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_କାଢ଼_ଗଦା(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    ଯଦି (q == ଖାଲି)
	ଫେରନ୍ତୁ;

    q->qi = qi;
    q->next = qf_delq_head;
    qf_delq_head = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_ଗଦା_get_bufnr(ଶୂନ୍ୟ)
{
    ଯଦି (ql_info == ଖାଲି)
	ଫେରନ୍ତୁ INVALID_QFBUFNR;
    ଫେରନ୍ତୁ ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    ଯଦି (qi->qf_bufnr == INVALID_QFBUFNR)
	ଫେରନ୍ତୁ;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    ଯଦି (qfbuf != ଖାଲି && qfbuf->b_nwindows == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	ଯଦି (curwin->w_buffer == ଖାଲି)
	{
	    curwin->w_buffer = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_buffer(ଖାଲି, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	ଯଦି (buf_was_null)
	    curwin->w_buffer = ଖାଲି;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_free_list_ଗଦା_ବସ୍ତୁଗୁଡ଼ିକ(qf_info_T *qi)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_ଗଦା_ବସ୍ତୁଗୁଡ଼ିକ(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ;
    *pqi = ଖାଲି;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    ଯଦି (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	ଫେରନ୍ତୁ;
    }

    qi->qf_refcount--;
    ଯଦି (qi->qf_refcount < 1)
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
    ଶୂନ୍ୟ
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    ଯଦି (wp != ଖାଲି)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    ଅନ୍ୟଥା ଯଦି (qi != ଖାଲି)
	qf_free_list_ଗଦା_ବସ୍ତୁଗୁଡ଼ିକ(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
incr_quickfix_busy(ଶୂନ୍ୟ)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
decr_quickfix_busy(ଶୂନ୍ୟ)
{
    ଯଦି (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	ଯେତେବେଳେ (qf_delq_head != ଖାଲି)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    ଯଦି (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
check_quickfix_busy(ଶୂନ୍ୟ)
{
    ଯଦି (quickfix_busy != 0)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_add_entry(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufnum,		// buffer number or zero
    char_u	*mesg,		// message
    long	lnum,		// line number
    long	end_lnum,	// line number for end
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,		// column
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_col,	// column for end
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vis_col,	// using visual column
    char_u	*pattern,	// search pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr,		// error number
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,		// type character
    typval_T	*user_data,     // custom user data or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = ଖାଲି;
    char_u	*p = ଖାଲି;

    ଯଦି ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == ଖାଲି)
	ଫେରନ୍ତୁ QF_FAIL;
    ଯଦି (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	ଯଦି (buf != ଖାଲି)
	    buf->b_has_qf_entry |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    }
    ଅନ୍ୟଥା
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    ଯଦି (fname != ଖାଲି)
	fullname = fix_fname(fname);
    qfp->qf_fname = ଖାଲି;
    ଯଦି (buf != ଖାଲି &&
	buf->b_ffname != ଖାଲି && fullname != ଖାଲି)
    {
	ଯଦି (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    ଯଦି (p != ଖାଲି)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    ଯଦି ((qfp->qf_text = vim_strsave(mesg)) == ଖାଲି)
    {
	vim_free(qfp);
	ଫେରନ୍ତୁ QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_end_lnum = end_lnum;
    qfp->qf_col = col;
    qfp->qf_end_col = end_col;
    qfp->qf_viscol = vis_col;
    ଯଦି (user_data == ଖାଲି || user_data->v_type == VAR_UNKNOWN)
	qfp->qf_user_data.v_type = VAR_UNKNOWN;
    ଅନ୍ୟଥା
    {
	copy_tv(user_data, &qfp->qf_user_data);
	qfl->qf_has_user_data = TRUE;
    }
    ଯଦି (pattern == ଖାଲି || *pattern == NUL)
	qfp->qf_pattern = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି ((qfp->qf_pattern = vim_strsave(pattern)) == ଖାଲି)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp);
	ଫେରନ୍ତୁ QF_FAIL;
    }
    ଯଦି (module == ଖାଲି || *module == NUL)
	qfp->qf_module = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି ((qfp->qf_module = vim_strsave(module)) == ଖାଲି)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp->qf_pattern);
	vim_free(qfp);
	ଫେରନ୍ତୁ QF_FAIL;
    }
    qfp->qf_nr = nr;
    ଯଦି (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_last;
    ଯଦି (qf_list_empty(qfl))		// first element in the list
    {
	qfl->qf_start = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_index = 0;
	qfp->qf_prev = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_next = qfp;
    }
    qfp->qf_next = ଖାଲି;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_ଗଣନା;
    ଯଦି (qfl->qf_index == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_index = qfl->qf_ଗଣନା;
	qfl->qf_ptr = qfp;
    }

    ଫେରନ୍ତୁ QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_resize_ଗଦା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    ଯଦି (ql_info == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (qf_resize_ଗଦା_base(ql_info, n) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ll_resize_ଗଦା(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    ଯଦି (IS_LL_WINDOW(wp))
	qf_sync_llw_to_win(wp);
    ଅନ୍ୟଥା
	qf_sync_win_to_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (qf_resize_ଗଦା_base(qi, n) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_resize_ଗଦା_base(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    qf_list_T *new;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା amount_to_rm = 0, i;

    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    size_t lsz = ଆକାର(*qi->qf_lists);

    ଯଦି (n == qi->qf_maxcount)
	ଫେରନ୍ତୁ OK;
    ଅନ୍ୟଥା ଯଦି (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_to_rm = qi->qf_listcount - n;

	ପାଇଁ (i = 0; i < amount_to_rm; i++)
	    qf_କାଢ଼_ଗଦା(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    ଯଦି (new == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // fill with zeroes any newly allocated memory
    ଯଦି (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_buffer(qi, ଖାଲି);

    ଫେରନ୍ତୁ OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   ଶୂନ୍ୟ
qf_init_ଗଦା(ଶୂନ୍ୟ)
{
    ql_info = qf_alloc_ଗଦା(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_sync_llw_to_win(win_T *llw)
{
    win_T *wp = qf_find_win_with_loclist(llw->w_llist_ref);

    ଯଦି (wp != ଖାଲି)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_sync_win_to_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    ଯଦି (llw != ଖାଲି)
	FOR_ALL_WINDOWS(wp)
	    ଯଦି (wp->w_llist_ref == llw && bt_quickfix(wp->w_buffer))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		ଫେରନ୍ତୁ;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    ସ୍ଥିର qf_info_T *
qf_alloc_ଗଦା(qfltype_T qfltype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    qf_info_T *qi;

    ଯଦି (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    ଅନ୍ୟଥା
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	ଯଦି (qi == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_ଗଦା(n);
    ଯଦି (qi->qf_lists == ଖାଲି)
    {
	ଯଦି (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    qi->qf_maxcount = n;

    ଫେରନ୍ତୁ qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    ସ୍ଥିର qf_list_T *
qf_alloc_list_ଗଦା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    ଫେରନ୍ତୁ ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    ସ୍ଥିର qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    ଯଦି (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	ଫେରନ୍ତୁ wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    ଯଦି (wp->w_llist == ଖାଲି)
	// new location list
	wp->w_llist = qf_alloc_ଗଦା(QFLT_LOCATION, wp->w_p_lhi);

    ଫେରନ୍ତୁ wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    ସ୍ଥିର qf_info_T *
qf_cmd_get_ଗଦା(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା print_emsg)
{
    qf_info_T	*qi = ql_info;

    ଯଦି (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	ଯଦି (qi == ଖାଲି)
	{
	    ଯଦି (print_emsg)
		emsg(_(e_no_location_list));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଯଦି (qi == ଖାଲି && print_emsg)
	emsg(_(e_no_quickfix_ଗଦା));

    ଫେରନ୍ତୁ qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    ସ୍ଥିର qf_info_T *
qf_cmd_get_or_alloc_ଗଦା(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    ଯଦି (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	ଯଦି (qi == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	*pwinp = curwin;
    }

    ଫେରନ୍ତୁ qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(from_qfl, from_qfp, i)
    {
	ଯଦି (qf_add_entry(to_qfl,
		    ଖାଲି,
		    ଖାଲି,
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
	    ଫେରନ୍ତୁ FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = to_qfl->qf_last;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	ଯଦି (from_qfl->qf_ptr == from_qfp)
	    to_qfl->qf_ptr = prevp;		// current location
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_has_user_data = from_qfl->qf_has_user_data;
    to_qfl->qf_ଗଣନା = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = ଖାଲି;
    to_qfl->qf_last = ଖାଲି;
    to_qfl->qf_ptr = ଖାଲି;
    ଯଦି (from_qfl->qf_title != ଖାଲି)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    ଅନ୍ୟଥା
	to_qfl->qf_title = ଖାଲି;
    ଯଦି (from_qfl->qf_ctx != ଖାଲି)
    {
	to_qfl->qf_ctx = alloc_tv();
	ଯଦି (to_qfl->qf_ctx != ଖାଲି)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    ଅନ୍ୟଥା
	to_qfl->qf_ctx = ଖାଲି;
    ଯଦି (from_qfl->qf_qftf_cb.cb_name != ଖାଲି)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    ଅନ୍ୟଥା
	to_qfl->qf_qftf_cb.cb_name = ଖାଲି;

    ଯଦି (from_qfl->qf_ଗଣନା)
	ଯଦି (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    ଯଦି (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    ଶୂନ୍ୟ
copy_loclist_ଗଦା(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    ଯଦି (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    ଅନ୍ୟଥା
	qi = from->w_llist;

    ଯଦି (qi == ଖାଲି)		    // no location list to copy
	ଫେରନ୍ତୁ;

    // allocate a new location list, set size of stack to 'from' window value
    ଯଦି ((to->w_llist = qf_alloc_ଗଦା(QFLT_LOCATION, from->w_p_lhi)) == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଅନ୍ୟଥା
	// set 'to' lhi to reflect new value
	to->w_p_lhi = to->w_llist->qf_maxcount;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    ପାଇଁ (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	ଯଦି (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    ଫେରନ୍ତୁ;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {ଖାଲି, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    ଯଦି (fname == ଖାଲି || *fname == NUL)		// no file name
	ଫେରନ୍ତୁ 0;

# ifdef VMS
    vms_remove_version(fname);
# endif
# ifdef BACKSLASH_IN_FILENAME
    ଯଦି (directory != ଖାଲି)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    ଯଦି (directory != ଖାଲି && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != ଖାଲି)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	ଯଦି (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    ଯଦି (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    ଅନ୍ୟଥା
	    {
		ptr.length = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    ଯଦି (ptr.string == ଖାଲି)
		ଫେରନ୍ତୁ 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.length = ptr.length;
    }
    ଅନ୍ୟଥା
    {
	bufname.string = fname;
	bufname.length = fname_len;
    }

    ଯଦି (qf_last_bufname != ଖାଲି && STRCMP(bufname.string, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr.string);
    }
    ଅନ୍ୟଥା
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname.string, ଖାଲି, (linenr_T)0, BLN_NOOPT);
	ଯଦି (bufname.string == ptr.string)
	    qf_last_bufname = bufname.string;
	ଅନ୍ୟଥା
	    qf_last_bufname = vim_strnsave(bufname.string, bufname.length);
	set_bufref(&qf_last_bufref, buf);
    }
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    ଫେରନ୍ତୁ buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    ସ୍ଥିର char_u *
qf_ଠେଲ_dir(char_u *dirbuf, ସଂରଚନା dir_ଗଦା_T **stackptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_file_ଗଦା)
{
    ସଂରଚନା dir_ଗଦା_T  *ds_new;
    ସଂରଚନା dir_ଗଦା_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(ସଂରଚନା dir_ଗଦା_T, aid_qf_dirstack);
    ଯଦି (ds_new == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // push the new element onto the stack
    ds_new->next = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    ଯଦି (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == ଖାଲି
	    || is_file_ଗଦା)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    ଅନ୍ୟଥା
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->next;
	(*stackptr)->dirname = ଖାଲି;
	ଯେତେବେଳେ (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    ଯଦି (dirname.string == ଖାଲି)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->next;
		vim_free(ds_ptr);

		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ଯଦି (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		ଭାଙ୍ଗ;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->next;
	}

	// clean up all dirs we already left
	ଯେତେବେଳେ ((*stackptr)->next != ds_new)
	{
	    ds_ptr = (*stackptr)->next;
	    (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	ଯଦି (ds_new == ଖାଲି)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    ଯଦି ((*stackptr)->dirname != ଖାଲି)
	ଫେରନ୍ତୁ (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->next;
    vim_free(ds_ptr);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    ସ୍ଥିର char_u *
qf_କାଢ଼_dir(ସଂରଚନା dir_ଗଦା_T **stackptr)
{
    ସଂରଚନା dir_ଗଦା_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    ଯଦି (*stackptr != ଖାଲି)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    ଫେରନ୍ତୁ *stackptr ? (*stackptr)->dirname : ଖାଲି;
}

/*
 * clean up directory stack
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_clean_dir_ଗଦା(ସଂରଚନା dir_ଗଦା_T **stackptr)
{
    ସଂରଚନା dir_ଗଦା_T  *ds_ptr;

    ଯେତେବେଳେ ((ds_ptr = *stackptr) != ଖାଲି)
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
    ସ୍ଥିର char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    ସଂରଚନା dir_ଗଦା_T     *ds_ptr;
    ସଂରଚନା dir_ଗଦା_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    ଯଦି (qfl->qf_dir_ଗଦା == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ds_ptr = qfl->qf_dir_ଗଦା->next;
    fullname.string = ଖାଲି;
    fullname.length = 0;
    filename_len = STRLEN(filename);
    ଯେତେବେଳେ (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	ଯଦି ((fullname.string != ଖାଲି) && (mch_getperm(fullname.string) >= 0))
	    ଭାଙ୍ଗ;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    ଯେତେବେଳେ (qfl->qf_dir_ଗଦା->next != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_ଗଦା->next;
	qfl->qf_dir_ଗଦା->next = qfl->qf_dir_ଗଦା->next->next;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    ଫେରନ୍ତୁ ds_ptr == ଖାଲି ? ଖାଲି : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (wp != ଖାଲି)
    {
	ଯଦି (!win_valid(wp))
	    ଫେରନ୍ତୁ FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (i = 0; i < qi->qf_listcount; ++i)
	ଯଦି (qi->qf_lists[i].qf_id == qf_id)
	    ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_qf_entry_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(qfl, qfp, i)
	ଯଦି (qfp == qf_ptr)
	    ଭାଙ୍ଗ;

    ଯଦି (i > qfl->qf_ଗଣନା) // Entry is not found
	ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    ସ୍ଥିର qfline_T *
get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*qf_index,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    କର
    {
	ଯଦି (idx == qfl->qf_ଗଣନା || qf_ptr->qf_next == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } ଯେତେବେଳେ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    ଫେରନ୍ତୁ qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    ସ୍ଥିର qfline_T *
get_prev_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*qf_index,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    କର
    {
	ଯଦି (idx == 1 || qf_ptr->qf_prev == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } ଯେତେବେଳେ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    ଫେରନ୍ତୁ qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    ସ୍ଥିର qfline_T *
get_nth_valid_entry(
	qf_list_T	*qfl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errornr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			prev_index;
    ଅକ୍ଷର		*err = e_no_more_ବସ୍ତୁଗୁଡ଼ିକ;

    ଯେତେବେଳେ (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	ଯଦି (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	ଅନ୍ୟଥା
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	ଯଦି (qf_ptr == ଖାଲି)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    ଯଦି (err != ଖାଲି)
	    {
		emsg(_(err));
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ଭାଙ୍ଗ;
	}

	err = ଖାଲି;
    }

    *new_qfidx = qf_idx;
    ଫେରନ୍ତୁ qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    ସ୍ଥିର qfline_T *
get_nth_entry(qf_list_T *qfl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା errornr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx = qfl->qf_index;

    // New error number is less than the current error number
    ଯେତେବେଳେ (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != ଖାଲି)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    ଯେତେବେଳେ (errornr > qf_idx && qf_idx < qfl->qf_ଗଣନା &&
						qf_ptr->qf_next != ଖାଲି)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_next;
    }

    *new_qfidx = qf_idx;
    ଫେରନ୍ତୁ qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    ସ୍ଥିର qfline_T *
qf_get_entry(
	qf_list_T	*qfl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errornr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qfidx = qfl->qf_index;

    ଯଦି (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_entry(qfl, errornr, dir, &qfidx);
    ଅନ୍ୟଥା ଯଦି (errornr != 0)	// go to specified number
	qf_ptr = get_nth_entry(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    ଫେରନ୍ତୁ qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    ସ୍ଥିର win_T *
qf_find_help_win(ଶୂନ୍ୟ)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ଯଦି (bt_help(wp->w_buffer))
	    ଫେରନ୍ତୁ wp;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_set_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
jump_to_help_window(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newwin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *opened_window)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;

    ଯଦି (cmdmod.cmod_tab != 0 || newwin)
	wp = ଖାଲି;
    ଅନ୍ୟଥା
	wp = qf_find_help_win();
    ଯଦି (wp != ଖାଲି && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    ଅନ୍ୟଥା
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	ଯଦି (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_ଶୀର୍ଷ;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	ଯଦି (IS_LL_ଗଦା(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	ଯଦି (win_split(0, flags) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	*opened_window = TRUE;

	ଯଦି (curwin->w_height < p_hh)
	    win_setheight((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	ଯଦି (IS_LL_ଗଦା(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    ଯଦି (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    ଫେରନ୍ତୁ OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    ସ୍ଥିର win_T *
qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    ଫେରନ୍ତୁ wp;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    ସ୍ଥିର win_T *
qf_find_win_with_normal_buf(ଶୂନ୍ୟ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ଯଦି (bt_normal(wp->w_buffer))
	    ଫେରନ୍ତୁ wp;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_goto_tabwin_with_file(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ଯଦି (wp->w_buffer->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    ଫେରନ୍ତୁ TRUE;
	}

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_open_new_file_win(qf_info_T *ll_ref)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;

    flags = WSP_ABOVE;
    ଯଦି (ll_ref != ଖାଲି)
	flags |= WSP_NEWLOC;
    ଯଦି (win_split(0, flags) == FAIL)
	ଫେରନ୍ତୁ FAIL;		// not enough room for window
    p_swb = empty_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    ଯଦି (ll_ref != ଖାଲି)
	// The new window should use the location list from the
	// location list window
	win_set_loclist(curwin, ll_ref);
    ଫେରନ୍ତୁ OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_goto_win_with_ll_file(win_T *use_win, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    ଯଦି (win == ଖାଲି)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    ଯଦି (win->w_buffer->b_fnum == qf_fnum)
		ଭାଙ୍ଗ;
	ଯଦି (win == ଖାଲି)
	{
	    // Find a previous usable window
	    win = curwin;
	    କର
	    {
		ଯଦି (bt_normal(win->w_buffer))
		    ଭାଙ୍ଗ;
		ଯଦି (win->w_prev == ଖାଲି)
		    win = lastwin;	// wrap around the top
		ଅନ୍ୟଥା
		    win = win->w_prev; // go to previous window
	    } ଯେତେବେଳେ (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    ଯଦି (win->w_llist == ଖାଲି && ll_ref != ଖାଲି)
	win_set_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_goto_win_with_qfl_file(ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = ଖାଲି;
    ପାଇଁ (;;)
    {
	ଯଦି (win->w_buffer->b_fnum == qf_fnum)
	    ଭାଙ୍ଗ;
	ଯଦି (win->w_prev == ଖାଲି)
	    win = lastwin;	// wrap around the top
	ଅନ୍ୟଥା
	    win = win->w_prev;	// go to previous window

	ଯଦି (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    ଯଦି ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    ଅନ୍ୟଥା ଯଦି (altwin != ଖାଲି)
		win = altwin;
	    ଅନ୍ୟଥା ଯଦି (curwin->w_prev != ଖାଲି)
		win = curwin->w_prev;
	    ଅନ୍ୟଥା
		win = curwin->w_next;
	    ଭାଙ୍ଗ;
	}

	// Remember a usable window.
	ଯଦି (altwin == ଖାଲି && !win->w_p_pvw && !win->w_p_wfb &&
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_jump_to_usable_window(ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_fnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newwin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *opened_window)
{
    win_T	*usable_wp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		usable_win = FALSE;
    qf_info_T	*ll_ref = ଖାଲି;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    ଯଦି (!newwin)
	ll_ref = curwin->w_llist_ref;

    ଯଦି (ll_ref != ଖାଲି)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	ଯଦି (usable_wp != ଖାଲି)
	    usable_win = TRUE;
    }

    ଯଦି (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	ଯଦି (win != ଖାଲି)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    ଯଦି (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    ଯଦି ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	ଯଦି (qf_open_new_file_win(ll_ref) != OK)
	    ଫେରନ୍ତୁ FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (curwin->w_llist_ref != ଖାଲି)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	ଅନ୍ୟଥା					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_jump_edit_buffer(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_winid,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_qf_curlist = qi->qf_curlist;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_qfid = qfl->qf_id;

    ଯଦି (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	ଯଦି (!can_abandon(curbuf, forceit))
	{
	    no_write_message();
	    ଫେରନ୍ତୁ FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, ଖାଲି, ଖାଲି, ଖାଲି, (linenr_T)1,
		ECMD_HIDE + ECMD_SET_HELP,
		prev_winid == curwin->w_id ? curwin : ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fnum = qf_ptr->qf_fnum;

	ଯଦି (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    ଯଦି (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		ଫେରନ୍ତୁ FAIL;
	    }

	    ଯଦି (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_buffer))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    ଯଦି (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		ଯଦି (win_split(0, 0) == OK)
		    *opened_window = TRUE;

		ଯଦି (curwin->w_p_wfb)
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

	ଯଦି (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    ଯଦି (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	ଯଦି (wp == ଖାଲି && curwin->w_llist != qi)
	{
	    emsg(_(e_current_window_was_closed));
	    *opened_window = FALSE;
	    ଫେରନ୍ତୁ QF_ABORT;
	}
    }

    ଯଦି (qfl_type == QFLT_QUICKFIX && !qflist_valid(ଖାଲି, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_changed));
	ଫେରନ୍ତୁ QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    ଯଦି (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	ଯଦି (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	ଅନ୍ୟଥା
	    emsg(_(e_current_location_list_was_changed));
	ଫେରନ୍ତୁ QF_ABORT;
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_screen_col_to_idx(colnr_T vcol)
{
    char_u	*ଧାଡ଼ି = ml_get_curline();
    char_u	*p = ଧାଡ଼ି;
    colnr_T	col = 0;

    ଯେତେବେଳେ (*p != NUL && col < vcol)
    {
	ଯଦି (*p == TAB)
	    col += 8 - (col % 8);
	ଅନ୍ୟଥା
	    col += ptr2cells(p);
	ଯଦି (col > vcol)
	    ଭାଙ୍ଗ;
	MB_PTR_ADV(p);
    }
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_jump_goto_ଧାଡ଼ି(
	linenr_T	qf_lnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    ଯଦି (qf_pattern == ଖାଲି)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	ଯଦି (i > 0)
	{
	    ଯଦି (i > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		i = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    curwin->w_cursor.lnum = i;
	}
	ଯଦି (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    ଯଦି (qf_viscol == TRUE)
		curwin->w_cursor.col = qf_screen_col_to_idx(qf_col - 1);
	    ଅନ୍ୟଥା
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = true;
	    check_cursor();
	}
	ଅନ୍ୟଥା
	    beginline(BL_WHITE | BL_FIX);
    }
    ଅନ୍ୟଥା
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	ଯଦି (!do_ସନ୍ଧାନ(ଖାଲି, '/', '/', qf_pattern, STRLEN(qf_pattern), (long)1, ସନ୍ଧାନ_KEEP, ଖାଲି))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_jump_print_ସନ୍ଦେଶ(
	qf_info_T	*qi,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_index,
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
    ଯଦି (!ସନ୍ଦେଶ_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((ଅକ୍ଷର *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_ଗଣନା,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (ଅକ୍ଷର *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_text(gap, skipwhite(qf_ptr->qf_text));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = ସନ୍ଦେଶ_scroll;
    ଯଦି (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	ସନ୍ଦେଶ_scroll = TRUE;
    ଅନ୍ୟଥା ଯଦି (!ସନ୍ଦେଶ_scrolled && shortmess(SHM_OVERALL))
	ସନ୍ଦେଶ_scroll = FALSE;
    ସନ୍ଦେଶ_attr_keep((ଅକ୍ଷର *)gap->ga_data, 0, TRUE);
    ସନ୍ଦେଶ_scroll = i;

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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_jump_open_window(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newwin,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_changedtick = qfl->qf_changedtick;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    ଯଦି (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_buffer)
						      || cmdmod.cmod_tab != 0))
	ଯଦି (jump_to_help_window(qi, newwin, opened_window) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    ଯଦି (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	ଯଦି (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	ଅନ୍ୟଥା
	    emsg(_(e_current_location_list_was_changed));
	ଫେରନ୍ତୁ QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    ଯଦି (bt_quickfix(curbuf) && !*opened_window)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	ଯଦି (qf_ptr->qf_fnum == 0)
	    ଫେରନ୍ତୁ NOTDONE;

	ଯଦି (qf_jump_to_usable_window(qf_ptr->qf_fnum, newwin,
						opened_window) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	ଯଦି (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	ଅନ୍ୟଥା
	    emsg(_(e_current_location_list_was_changed));
	ଫେରନ୍ତୁ QF_ABORT;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_jump_to_buffer(
	qf_info_T	*qi,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_index,
	qfline_T	*qf_ptr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_winid,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*opened_window,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		openfold,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    ଯଦି (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	ଯଦି (retval != OK)
	    ଫେରନ୍ତୁ retval;
    }

    // When not switched to another buffer, still need to set pc mark
    ଯଦି (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_ଧାଡ଼ି(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

# ifdef FEAT_FOLDING
    ଯଦି ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    ଯଦି (print_message)
	qf_jump_print_ସନ୍ଦେଶ(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    ଫେରନ୍ତୁ retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    ଶୂନ୍ୟ
qf_jump(qf_info_T	*qi,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errornr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit)
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
    ସ୍ଥିର ଶୂନ୍ୟ
qf_jump_newwin(qf_info_T	*qi,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errornr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			qf_index;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			old_qf_index;
    char_u		*old_swb = p_swb;
    unsigned		old_swb_flags = swb_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			prev_winid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			opened_window = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			print_message = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			old_ଚାବି_Typed = ଚାବି_Typed; // getting file may reset it
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			retval = OK;

    ଯଦି (qi == ଖାଲି)
    {
	ଯଦି (ql_info == ଖାଲି)
	{
	    emsg(_(e_no_quickfix_ଗଦା));
	    ଫେରନ୍ତୁ;
	}
	qi = ql_info;
    }

    ଯଦି (qf_ଗଦା_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ଫେରନ୍ତୁ;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qfl->qf_index;
    old_qf_index = qf_index;

    qf_ptr = qf_get_entry(qfl, errornr, dir, &qf_index);
    ଯଦି (qf_ptr == ଖାଲି)
    {
	qf_ptr = old_qf_ptr;
	qf_index = old_qf_index;
	ଯାଅ theend;
    }

    qfl->qf_index = qf_index;
    qfl->qf_ptr = qf_ptr;
    ଯଦି (qf_win_pos_update(qi, old_qf_index))
	// No need to print the error message if it's visible in the error
	// window
	print_message = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_window(qi, qf_ptr, newwin, &opened_window);
    ଯଦି (retval == FAIL)
	ଯାଅ failed;
    ଯଦି (retval == QF_ABORT)
    {
	qi = ଖାଲି;
	qf_ptr = ଖାଲି;
	ଯାଅ theend;
    }
    ଯଦି (retval == NOTDONE)
	ଯାଅ theend;

    retval = qf_jump_to_buffer(qi, qf_index, qf_ptr, forceit, prev_winid,
				  &opened_window, old_ଚାବି_Typed, print_message);
    ଯଦି (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = ଖାଲି;
	qf_ptr = ଖାଲି;
    }

    ଯଦି (retval != OK)
    {
	ଯଦି (opened_window)
	    win_close(curwin, TRUE);    // Close opened window
	ଯଦି (qf_ptr != ଖାଲି && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    ଯଦି (qi != ଖାଲି)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_index = qf_index;
    }
    ଯଦି (p_swb != old_swb && p_swb == empty_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	qfFileAttr;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	qfSepAttr;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	qf_ଧାଡ଼ି_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_list_entry(qfline_T *qfp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cursel)
{
    char_u	*fname;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filter_entry;
    garray_T	*gap;

    fname = ଖାଲି;
    ଯଦି (qfp->qf_module != ଖାଲି && *qfp->qf_module != NUL)
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(ଅକ୍ଷର *)qfp->qf_module);
    ଅନ୍ୟଥା
    {
	ଯଦି (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != ଖାଲି)
	{
	    ଯଦି (qfp->qf_fname == ଖାଲି)
		fname = buf->b_fname;
	    ଅନ୍ୟଥା
		fname = qfp->qf_fname;
	    ଯଦି (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	ଯଦି (fname == ଖାଲି)
	    sprintf((ଅକ୍ଷର *)IObuff, "%2d", qf_idx);
	ଅନ୍ୟଥା
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (ଅକ୍ଷର *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    ଯଦି (qfp->qf_module != ଖାଲି && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    ଯଦି (filter_entry && fname != ଖାଲି)
	filter_entry &= message_filtered(fname);
    ଯଦି (filter_entry && qfp->qf_pattern != ଖାଲି)
	filter_entry &= message_filtered(qfp->qf_pattern);
    ଯଦି (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    ଯଦି (filter_entry)
	ଫେରନ୍ତୁ;

    ସନ୍ଦେଶ_putchar('\n');
    ସନ୍ଦେଶ_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    ଯଦି (qfp->qf_lnum != 0)
	ସନ୍ଦେଶ_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    ଯଦି (qfp->qf_lnum != 0)
	qf_range_text(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)gap->ga_data, qf_ଧାଡ଼ି_Attr);
    ସନ୍ଦେଶ_puts_attr(":", qfSepAttr);
    ଯଦି (qfp->qf_pattern != ଖାଲି)
    {
	gap = qfga_get();
	qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, NUL);
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)gap->ga_data);
	ସନ୍ଦେଶ_puts_attr(":", qfSepAttr);
    }
    ସନ୍ଦେଶ_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_text(gap, (fname != ଖାଲି || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_text) : qfp->qf_text);
    ga_append(gap, NUL);
    ସନ୍ଦେଶ_prt_ଧାଡ଼ି((char_u *)gap->ga_data, FALSE);
    out_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    ଶୂନ୍ୟ
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx1 = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx2 = -1;
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		plus = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, TRUE)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (qf_ଗଦା_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    ଯଦି (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	ଫେରନ୍ତୁ;
    }
    qfl = qf_get_curlist(qi);
    ଯଦି (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    ଅନ୍ୟଥା
    {
	i = qfl->qf_ଗଣନା;
	ଯଦି (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	ଯଦି (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    ଯଦି (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    ଯଦି (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_ଧାଡ଼ି_Attr = syn_name2attr((char_u *)"qfLineNr");
    ଯଦି (qf_ଧାଡ଼ି_Attr == 0)
	qf_ଧାଡ଼ି_Attr = HL_ATTR(HLF_N);

    ଯଦି (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(qfl, qfp, i)
    {
	ଯଦି ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_fmt_text(garray_T *gap, char_u *text)
{
    char_u	*p = text;
    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (*p == '\n')
	{
	    ga_append(gap, ' ');
	    ଯେତେବେଳେ (*++p != NUL)
		ଯଦି (!VIM_ISWHITE(*p) && *p != '\n')
		    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_range_text(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.length = vim_snprintf_safelen((ଅକ୍ଷର *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    ଯଦି (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	buf.length += vim_snprintf_safelen((ଅକ୍ଷର *)buf.string + buf.length,
	    IOSIZE - buf.length, "-%ld", qfp->qf_end_lnum);
    }
    ଯଦି (qfp->qf_col > 0)
    {
	buf.length += vim_snprintf_safelen((ଅକ୍ଷର *)buf.string + buf.length,
	    IOSIZE - buf.length, " col %d", qfp->qf_col);
	ଯଦି (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	    buf.length += vim_snprintf_safelen((ଅକ୍ଷର *)buf.string + buf.length,
		IOSIZE - buf.length, "-%d", qfp->qf_end_col);
    }

    ga_concat_len(gap, buf.string, buf.length);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_ସନ୍ଦେଶ(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା which, ଅକ୍ଷର *lead)
{
    ଅକ୍ଷର   *title = (ଅକ୍ଷର *)qi->qf_lists[which].qf_title;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା    ଗଣନା_2 = qi->qf_lists[which].qf_ଗଣନା;
    char_u buf[IOSIZE];

    vim_snprintf((ଅକ୍ଷର *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    ଗଣନା_2);

    ଯଦି (title != ଖାଲି)
    {
	size_t	len = STRLEN(buf);

	ଯଦି (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    ସନ୍ଦେଶ((ଅକ୍ଷର *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    ଶୂନ୍ୟ
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, TRUE)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (eap->addr_ଗଣନା != 0)
	ଗଣନା_2 = eap->line2;
    ଅନ୍ୟଥା
	ଗଣନା_2 = 1;
    ଯେତେବେଳେ (ଗଣନା_2--)
    {
	ଯଦି (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    ଯଦି (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_ଗଦା));
		ଭାଙ୍ଗ;
	    }
	    --qi->qf_curlist;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_ଶୀର୍ଷ_of_quickfix_ଗଦା));
		ଭାଙ୍ଗ;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_ସନ୍ଦେଶ(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, ଖାଲି);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    ଶୂନ୍ୟ
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_ଗଦା(eap, FALSE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (eap->addr_ଗଣନା > 0)
    {
	ଯଦି (qi == ଖାଲି)
	{
	    emsg(_(e_no_location_list));
	    ଫେରନ୍ତୁ;
	}

	// Jump to the specified quickfix list
	ଯଦି (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_ସନ୍ଦେଶ(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, ଖାଲି);
	}
	ଅନ୍ୟଥା
	    emsg(_(e_invalid_range));

	ଫେରନ୍ତୁ;
    }

    ଯଦି (qf_ଗଦା_empty(qi))
	ସନ୍ଦେଶ(_("No entries"));
    ଅନ୍ୟଥା
	ପାଇଁ (i = 0; i < qi->qf_listcount; ++i)
	    qf_ସନ୍ଦେଶ(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_free_ବସ୍ତୁଗୁଡ଼ିକ(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stop = FALSE;

    ଯେତେବେଳେ (qfl->qf_ଗଣନା && qfl->qf_start != ଖାଲି)
    {
	qfp = qfl->qf_start;
	qfpnext = qfp->qf_next;
	ଯଦି (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_text);
	    vim_free(qfp->qf_pattern);
	    clear_tv(&qfp->qf_user_data);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    ଯଦି (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_ଗଣନା = 1;
	    ଅନ୍ୟଥା
		qfl->qf_start = qfpnext;
	}
	--qfl->qf_ଗଣନା;
    }

    qfl->qf_index = 0;
    qfl->qf_start = ଖାଲି;
    qfl->qf_last = ଖାଲି;
    qfl->qf_ptr = ଖାଲି;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_ଗଦା(&qfl->qf_dir_ଗଦା);
    qfl->qf_directory = ଖାଲି;
    qf_clean_dir_ଗଦା(&qfl->qf_file_ଗଦା);
    qfl->qf_currfile = ଖାଲି;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_free(qf_list_T *qfl)
{
    qf_free_ବସ୍ତୁଗୁଡ଼ିକ(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = ଖାଲି;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   ଶୂନ୍ୟ
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	long	amount,
	long	amount_after)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    qfline_T	*qfp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    qf_info_T	*qi = ql_info;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_one = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buf_has_flag = wp == ଖାଲି ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;

    ଯଦି (!(curbuf->b_has_qf_entry & buf_has_flag))
	ଫେରନ୍ତୁ;
    ଯଦି (wp != ଖାଲି)
    {
	ଯଦି (wp->w_llist == ଖାଲି)
	    ଫେରନ୍ତୁ;
	qi = wp->w_llist;
    }
    ଅନ୍ୟଥା ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	ଯଦି (!qf_list_empty(qfl))
	    FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(qfl, qfp, i)
		ଯଦି (qfp->qf_fnum == curbuf->b_fnum)
		{
		    found_one = TRUE;
		    ଯଦି (qfp->qf_cleared)
			ଜାରି;

		    ଯଦି (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			ଯଦି (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			ଅନ୍ୟଥା
			    qfp->qf_lnum += amount;
		    }
		    ଅନ୍ୟଥା ଯଦି (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    ଯଦି (!found_one)
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
    ସ୍ଥିର char_u *
qf_types(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr)
{
    ସ୍ଥିର char_u	buf[20];
    ସ୍ଥିର char_u	cc[3];
    char_u		*p;

    ଯଦି (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    ଅନ୍ୟଥା ଯଦି (c == 'I' || c == 'i')
	p = (char_u *)" info";
    ଅନ୍ୟଥା ଯଦି (c == 'N' || c == 'n')
	p = (char_u *)" note";
    ଅନ୍ୟଥା ଯଦି (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    ଅନ୍ୟଥା ଯଦି (c == 0 || c == 1)
	p = (char_u *)"";
    ଅନ୍ୟଥା
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    ଯଦି (nr <= 0)
	ଫେରନ୍ତୁ p;

    sprintf((ଅକ୍ଷର *)buf, "%s %3d", (ଅକ୍ଷର *)p, nr);
    ଫେରନ୍ତୁ buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    ଶୂନ୍ୟ
qf_view_result(ପୂର୍ଣ୍ଣସଂଖ୍ୟା split)
{
    qf_info_T   *qi = ql_info;

    ଯଦି (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);
    ଅନ୍ୟଥା ଯଦି (qi == ଖାଲି)
    {
	emsg(_(e_no_quickfix_ଗଦା));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (long)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	ଫେରନ୍ତୁ;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    ଶୂନ୍ୟ
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, TRUE)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    ଯଦି (qf_ଗଦା_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	ଯଦି (win != ଖାଲି)
	    ex_cclose(eap);
    }
    ଅନ୍ୟଥା ଯଦି (win == ଖାଲି)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    ଶୂନ୍ୟ
ex_cclose(exarg_T *eap)
{
    win_T	*win = ଖାଲି;
    qf_info_T	*qi;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, FALSE)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    ଯଦି (win != ଖାଲି)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_set_title_var(qf_list_T *qfl)
{
    ଯଦି (qfl->qf_title != ଖାଲି)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_goto_cwindow(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା resize, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    ଯଦି (win == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    win_goto(win);
    ଯଦି (resize)
    {
	ଯଦି (vertsplit)
	{
	    ଯଦି (sz != win->w_width)
		win_setwidth(sz);
	}
	ଅନ୍ୟଥା ଯଦି (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_set_cwindow_options(ଶୂନ୍ୟ)
{
    // switch off 'swapfile'
    set_option_ମୂଲ୍ୟ_give_err((char_u *)"swf", 0L, ଖାଲି, OPT_LOCAL);
    set_option_ମୂଲ୍ୟ_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_ମୂଲ୍ୟ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    set_option_ମୂଲ୍ୟ_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_open_new_cwindow(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;
    win_T	*win;

    qf_buf = qf_find_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    ଯଦି (IS_QF_ଗଦା(qi) && cmdmod.cmod_split == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    ଯଦି (cmdmod.cmod_split == 0)
	flags = WSP_BELOW;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    ଯଦି (IS_QF_ଗଦା(qi))
	flags |= WSP_QUICKFIX;

    ଯଦି (win_split(height, flags) == FAIL)
	ଫେରନ୍ତୁ FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    ଯଦି (IS_LL_ଗଦା(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    ଯଦି (oldwin != curwin)
	oldwin = ଖାଲି;  // don't store info when in another window
    ଯଦି (qf_buf != ଖାଲି)
    {
	// Use the existing quickfix buffer
	ଯଦି (do_ecmd(qf_buf->b_fnum, ଖାଲି, ଖାଲି, ଖାଲି, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	// Create a new quickfix buffer
	ଯଦି (do_ecmd(0, ଖାଲି, ଖାଲି, ଖାଲି, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    ଯଦି (!bt_quickfix(curbuf))
	qf_set_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    ଯଦି (curtab == prevtab && curwin->w_width == Columns)
	win_setheight(height);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    ଯଦି (win_valid(win))
	prevwin = win;

    ଫେରନ୍ତୁ OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    ଶୂନ୍ୟ
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, TRUE)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    incr_quickfix_busy();

    ଯଦି (eap->addr_ଗଣନା != 0)
	height = eap->line2;
    ଅନ୍ୟଥା
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    ଯଦି (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_ଗଣନା != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    ଯଦି (status == FAIL)
	ଯଦି (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    ଫେରନ୍ତୁ;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, ଖାଲି, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, TRUE)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    win = qf_find_win(qi);
    ଯଦି (win != ଖାଲି && win->w_cursor.lnum != win->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	qf_win_goto(win, win->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_entry(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    ଯଦି (IS_LL_WINDOW(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    ଅନ୍ୟଥା ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    ଫେରନ୍ତୁ qf_get_curlist(qi)->qf_index;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_win_pos_update(
    qf_info_T	*qi,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_qf_index)	// previous qf_index or zero
{
    win_T	*win;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_index = qf_get_curlist(qi)->qf_index;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_find_win(qi);
    ଯଦି (win != ଖାଲି
	    && qf_index <= win->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା
	    && old_qf_index != qf_index)
    {
	ଯଦି (qf_index > old_qf_index)
	{
	    win->w_redraw_ଶୀର୍ଷ = old_qf_index;
	    win->w_redraw_bot = qf_index;
	}
	ଅନ୍ୟଥା
	{
	    win->w_redraw_ଶୀର୍ଷ = qf_index;
	    win->w_redraw_bot = old_qf_index;
	}
	qf_win_goto(win, qf_index);
    }
    ଫେରନ୍ତୁ win != ଖାଲି;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    ଯଦି (buf_valid(win->w_buffer) && bt_quickfix(win->w_buffer))
	ଯଦି ((IS_QF_ଗଦା(qi) && win->w_llist_ref == ଖାଲି)
		|| (IS_LL_ଗଦା(qi) && win->w_llist_ref == qi))
	    ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    ସ୍ଥିର win_T *
qf_find_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ଯଦି (is_qf_win(win, qi))
	    ଫେରନ୍ତୁ win;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    ସ୍ଥିର buf_T *
qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    ଯଦି (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	ଯଦି (qfbuf != ଖାଲି)
	    ଫେରନ୍ତୁ qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	ଯଦି (is_qf_win(win, qi))
	    ଫେରନ୍ତୁ win->w_buffer;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    ଅକ୍ଷର *
did_set_quickfixtextfunc(optset_T *args UNUSED)
{
    ଯଦି (option_set_callback_func(p_qftf, &qftf_cb) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	ଯଦି (is_qf_win(win, qi))
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
    ସ୍ଥିର ଶୂନ୍ୟ
qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    linenr_T	old_ଧାଡ଼ି_ଗଣନା = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_winid = 0;

    ଯଦି (IS_LL_ଗଦା(qi))
    {
	ଯଦି (curwin->w_llist == qi)
	    win = curwin;
	ଅନ୍ୟଥା
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_find_win_with_loclist(qi);
	    ଯଦି (win == ଖାଲି)
		// File window is not found. Find the location list window.
		win = qf_find_win(qi);
	    ଯଦି (win == ଖାଲି)
		ଫେରନ୍ତୁ;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_fill = TRUE;
    ଯଦି (old_last == ଖାଲି)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	ଯଦି (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    ଯଦି (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	ଯଦି (old_last == ଖାଲି)
	{
	    (ଶୂନ୍ୟ)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    ଯଦି ((win = qf_find_win(qi)) != ଖାଲି && old_ଧାଡ଼ି_ଗଣନା < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_buf_add_ଧାଡ଼ି(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    ଯଦି (qftf_str != ଖାଲି && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (qfp->qf_module != ଖାଲି)
	    ga_concat(gap, qfp->qf_module);
	ଅନ୍ୟଥା ଯଦି (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != ଖାଲି
		&& errbuf->b_fname != ଖାଲି)
	{
	    ଯଦି (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    ଅନ୍ୟଥା
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		ଯଦି (first_bufline && (errbuf->b_sfname == ଖାଲି
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    ଯଦି (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		ଯଦି (qfp->qf_fname == ଖାଲି)
		    ga_concat(gap, errbuf->b_fname);
		ଅନ୍ୟଥା
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	ଯଦି (qfp->qf_lnum > 0)
	{
	    qf_range_text(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	ଅନ୍ୟଥା ଯଦି (qfp->qf_pattern != ଖାଲି)
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
    ଯଦି (ml_append_buf(buf, lnum, gap->ga_data, gap->ga_len, FALSE) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    ସ୍ଥିର list_T *
call_qftf_func(qf_list_T *qfl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_winid, long start_idx, long end_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;

    ଯଦି (recursive)
	ଫେରନ୍ତୁ ଖାଲି;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    ଯଦି (qfl->qf_qftf_cb.cb_name != ଖାଲି)
	cb = &qfl->qf_qftf_cb;
    ଯଦି (cb->cb_name != ଖାଲି)
    {
	typval_T	args[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	ଯଦି ((d = dict_alloc_lock(VAR_FIXED)) == ଖାଲି)
	{
	    recursive = FALSE;
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	dict_add_ସଂଖ୍ୟା(d, "quickfix", (long)IS_QF_LIST(qfl));
	dict_add_ସଂଖ୍ୟା(d, "winid", (long)qf_winid);
	dict_add_ସଂଖ୍ୟା(d, "id", (long)qfl->qf_id);
	dict_add_ସଂଖ୍ୟା(d, "start_idx", start_idx);
	dict_add_ସଂଖ୍ୟା(d, "end_idx", end_idx);
	++d->dv_refcount;
	args[0].v_type = VAR_DICT;
	args[0].vval.v_dict = d;

	qftf_list = ଖାଲି;
	textlock++;
	ଯଦି (call_callback(cb, 0, &rettv, 1, args) != FAIL)
	{
	    ଯଦି (rettv.v_type == VAR_LIST)
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
    ଫେରନ୍ତୁ qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_ଚାବି_Typed = ଚାବି_Typed;
    list_T	*qftf_list = ଖାଲି;
    listitem_T	*qftf_li = ଖାଲି;

    ଯଦି (old_last == ଖାଲି)
    {
	win_T		*wp;
	tabpage_T	*tp;

	ଯଦି (buf != curbuf)
	{
	    internal_error("qf_fill_buffer()");
	    ଫେରନ୍ତୁ;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	ଯେତେବେଳେ ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	    (ଶୂନ୍ୟ)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ଯଦି (wp->w_buffer == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    ଯଦି (qfl != ଖାଲି && qfl->qf_start != ଖାଲି)
    {
	char_u		dirname[MAXPATHL];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		invalid_val = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	ଯଦି (old_last == ଖାଲି)
	{
	    qfp = qfl->qf_start;
	    lnum = 0;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (old_last->qf_next != ଖାଲି)
		qfp = old_last->qf_next;
	    ଅନ୍ୟଥା
		qfp = old_last;
	    lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (long)(lnum + 1),
							(long)qfl->qf_ଗଣନା);
	ଯଦି (qftf_list != ଖାଲି)
	    qftf_li = qftf_list->lv_first;

	ଯେତେବେଳେ (lnum < qfl->qf_ଗଣନା)
	{
	    char_u	*qftf_str = ଖାଲି;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    ଯଦି (qftf_li != ଖାଲି && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		ଯଦି (qftf_str == ଖାଲି)
		    invalid_val = TRUE;
	    }

	    ଯଦି (qf_buf_add_ଧାଡ଼ି(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		ଭାଙ୍ଗ;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_next;
	    ଯଦି (qfp == ଖାଲି)
		ଭାଙ୍ଗ;

	    ଯଦି (qftf_li != ଖାଲି)
		qftf_li = qftf_li->li_next;
	}

	ଯଦି (old_last == ଖାଲି)
	    // Delete the empty line which is now at the end
	    (ଶୂନ୍ୟ)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    ଯଦି (old_last == ଖାଲି)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_lock;
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", ଖାଲି,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", ଖାଲି,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_lock;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    ଚାବି_Typed = old_ଚାବି_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_list_changed(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx;

    ପାଇଁ (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	ଯଦି (qi->qf_lists[qf_idx].qf_id == qfid)
	    ଫେରନ୍ତୁ qf_idx;
    ଫେରନ୍ତୁ INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlist;

    ଯଦି (qf_get_curlist(qi)->qf_id == save_qfid)
	ଫେରନ୍ତୁ OK;

    curlist = qf_id2nr(qi, save_qfid);
    ଯଦି (curlist < 0)
	// list is not present
	ଫେରନ୍ତୁ FAIL;
    qi->qf_curlist = curlist;
    ଫେରନ୍ତୁ OK;
}

/*
 * Jump to the first entry if there is one.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_jump_first(qf_info_T *qi, int_u save_qfid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    ଯଦି (qf_restore_list(qi, save_qfid) == FAIL)
	ଫେରନ୍ତୁ;


    ଯଦି (!check_can_set_curbuf_forceit(forceit))
	ଫେରନ୍ତୁ;


    // Autocommands might have cleared the list, check for that.
    ଯଦି (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
grep_internal(cmdidx_T cmdidx)
{
    ଫେରନ୍ତୁ ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    ସ୍ଥିର char_u *
make_get_auname(cmdidx_T cmdidx)
{
    ବାଛ (cmdidx)
    {
	ସ୍ଥିତି CMD_make:	    ଫେରନ୍ତୁ (char_u *)"make";
	ସ୍ଥିତି CMD_lmake:	    ଫେରନ୍ତୁ (char_u *)"lmake";
	ସ୍ଥିତି CMD_grep:	    ଫେରନ୍ତୁ (char_u *)"grep";
	ସ୍ଥିତି CMD_lgrep:	    ଫେରନ୍ତୁ (char_u *)"lgrep";
	ସ୍ଥିତି CMD_grepadd:   ଫେରନ୍ତୁ (char_u *)"grepadd";
	ସ୍ଥିତି CMD_lgrepadd:  ଫେରନ୍ତୁ (char_u *)"lgrepadd";
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ ଖାଲି;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    ସ୍ଥିର char_u *
get_mef_name(ଶୂନ୍ୟ)
{
    char_u	*p;
    char_u	*name;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	start = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    ଯଦି (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	ଯଦି (name == ଖାଲି)
	    emsg(_(e_cant_get_temp_file_name));
	ଫେରନ୍ତୁ name;
    }

    ପାଇଁ (p = p_mef; *p; ++p)
	ଯଦି (p[0] == '#' && p[1] == '#')
	    ଭାଙ୍ଗ;

    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    ପାଇଁ (;;)
    {
	ଯଦି (start == -1)
	    start = mch_get_pid();
	ଅନ୍ୟଥା
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	ଯଦି (name == ଖାଲି)
	    ଭାଙ୍ଗ;
	STRCPY(name, p_mef);
	sprintf((ଅକ୍ଷର *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	ଯଦି (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((ଅକ୍ଷର *)name, &sb) < 0
# endif
		)
	    ଭାଙ୍ଗ;
	vim_free(name);
    }
    ଫେରନ୍ତୁ name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    ସ୍ଥିର char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    unsigned	len;

    len = (unsigned)STRLEN(p_shq) * 2 + (unsigned)STRLEN(makecmd) + 1;
    ଯଦି (*p_sp != NUL)
	len += (unsigned)STRLEN(p_sp) + (unsigned)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    ଯଦି (cmd == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    sprintf((ଅକ୍ଷର *)cmd, "%s%s%s", (ଅକ୍ଷର *)p_shq, (ଅକ୍ଷର *)makecmd,
							       (ଅକ୍ଷର *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    ଯଦି (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    ଯଦି (ସନ୍ଦେଶ_col == 0)
	ସନ୍ଦେଶ_didout = FALSE;
    ସନ୍ଦେଶ_start();
    ସନ୍ଦେଶ_puts(":!");
    ସନ୍ଦେଶ_outtrans(cmd);		// show what we are doing

    ଫେରନ୍ତୁ cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    ଶୂନ୍ୟ
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = ଖାଲି;
    win_T	*wp = ଖାଲି;
    qf_info_T	*qi = ql_info;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    char_u	*au_name = ଖାଲି;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    ଯଦି (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	ଫେରନ୍ତୁ;
    }

    au_name = make_get_auname(eap->cmdidx);
    ଯଦି (au_name != ଖାଲି && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ଯଦି (aborting())
	    ଫେରନ୍ତୁ;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    ଯଦି (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    ଯଦି (cmd == ଖାଲି)
    {
	vim_free(fname);
	ଫେରନ୍ତୁ;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (ଶୂନ୍ୟ)char_avail();
# endif

    incr_quickfix_busy();

    ଯଦି (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    ଯଦି (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    ଯଦି (wp != ଖାଲି)
    {
	qi = GET_LOC_LIST(wp);
	ଯଦି (qi == ଖାଲି)
	    ଯାଅ cleanup;
    }
    ଅନ୍ୟଥା ଯଦି (qi == ଖାଲି)
	ଯାଅ cleanup;

    ଯଦି (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ଯଦି (au_name != ଖାଲି)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    ଯଦି (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_size(exarg_T *eap)
{
    qf_info_T	*qi;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, FALSE)) == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ qf_get_curlist(qi)->qf_ଗଣନା;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, sz = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_fnum = 0;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, FALSE)) == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(qfl, qfp, i)
    {
	ଯଦି (qfp->qf_valid)
	{
	    ଯଦି (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    ଅନ୍ୟଥା ଯଦି (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    ଫେରନ୍ତୁ sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, FALSE)) == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    ଫେରନ୍ତୁ qf_get_curlist(qi)->qf_index;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, eidx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_fnum = 0;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, FALSE)) == ଖାଲି)
	ଫେରନ୍ତୁ 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_start;

    // check if the list has valid errors
    ଯଦି (!qf_list_has_valid_entries(qfl))
	ଫେରନ୍ତୁ 1;

    ପାଇଁ (i = 1; i <= qfl->qf_index && qfp!= ଖାଲି; i++, qfp = qfp->qf_next)
    {
	ଯଦି (qfp->qf_valid)
	{
	    ଯଦି (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ଯଦି (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    ଅନ୍ୟଥା
		eidx++;
	}
    }

    ଫେରନ୍ତୁ eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_nth_valid_entry(qf_list_T *qfl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdo)
{
    qfline_T	*qfp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, eidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_fnum = 0;

    // check if the list has valid errors
    ଯଦି (!qf_list_has_valid_entries(qfl))
	ଫେରନ୍ତୁ 1;

    eidx = 0;
    FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(qfl, qfp, i)
    {
	ଯଦି (qfp->qf_valid)
	{
	    ଯଦି (fdo)
	    {
		ଯଦି (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    ଅନ୍ୟଥା
		eidx++;
	}

	ଯଦି (eidx == n)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (i <= qfl->qf_ଗଣନା)
	ଫେରନ୍ତୁ i;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    ଶୂନ୍ୟ
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errornr;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, TRUE)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (eap->addr_ଗଣନା > 0)
	errornr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2;
    ଅନ୍ୟଥା
    {
	ବାଛ (eap->cmdidx)
	{
	    ସ୍ଥିତି CMD_cc: ସ୍ଥିତି CMD_ll:
		errornr = 0;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_crewind: ସ୍ଥିତି CMD_lrewind: ସ୍ଥିତି CMD_cfirst:
	    ସ୍ଥିତି CMD_lfirst:
		errornr = 1;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    ଯଦି (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_ଗଣନା > 0 ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    ଶୂନ୍ୟ
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errornr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, TRUE)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (eap->addr_ଗଣନା > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2;
    ଅନ୍ୟଥା
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    ବାଛ (eap->cmdidx)
    {
	ସ୍ଥିତି CMD_cnext: ସ୍ଥିତି CMD_lnext: ସ୍ଥିତି CMD_cdo: ସ୍ଥିତି CMD_ldo:
	    dir = FORWARD;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_cprevious: ସ୍ଥିତି CMD_lprevious: ସ୍ଥିତି CMD_cNext:
	ସ୍ଥିତି CMD_lNext:
	    dir = BACKWARD;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_cnfile: ସ୍ଥିତି CMD_lnfile: ସ୍ଥିତି CMD_cfdo: ସ୍ଥିତି CMD_lfdo:
	    dir = FORWARD_FILE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_cpfile: ସ୍ଥିତି CMD_lpfile: ସ୍ଥିତି CMD_cNfile: ସ୍ଥିତି CMD_lNfile:
	    dir = BACKWARD_FILE;
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    dir = FORWARD;
	    ଭାଙ୍ଗ;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    ସ୍ଥିର qfline_T *
qf_find_first_entry_in_buf(qf_list_T *qfl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bnr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errornr)
{
    qfline_T	*qfp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(qfl, qfp, idx)
	ଯଦି (qfp->qf_fnum == bnr)
	    ଭାଙ୍ଗ;

    *errornr = idx;
    ଫେରନ୍ତୁ qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ସ୍ଥିର qfline_T *
qf_find_first_entry_on_ଧାଡ଼ି(qfline_T *entry, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errornr)
{
    ଯେତେବେଳେ (!got_int
	    && entry->qf_prev != ଖାଲି
	    && entry->qf_fnum == entry->qf_prev->qf_fnum
	    && entry->qf_lnum == entry->qf_prev->qf_lnum)
    {
	entry = entry->qf_prev;
	--*errornr;
    }

    ଫେରନ୍ତୁ entry;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ସ୍ଥିର qfline_T *
qf_find_last_entry_on_ଧାଡ଼ି(qfline_T *entry, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errornr)
{
    ଯେତେବେଳେ (!got_int &&
	    entry->qf_next != ଖାଲି
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    ଫେରନ୍ତୁ entry;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_entry_after_pos(qfline_T *qfp, pos_T *pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା linewise)
{
    ଯଦି (linewise)
	ଫେରନ୍ତୁ qfp->qf_lnum > pos->lnum;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_entry_before_pos(qfline_T *qfp, pos_T *pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା linewise)
{
    ଯଦି (linewise)
	ଫେରନ୍ତୁ qfp->qf_lnum < pos->lnum;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col < pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_entry_on_or_after_pos(qfline_T *qfp, pos_T *pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା linewise)
{
    ଯଦି (linewise)
	ଫେରନ୍ତୁ qfp->qf_lnum >= pos->lnum;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col >= pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_entry_on_or_before_pos(qfline_T *qfp, pos_T *pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା linewise)
{
    ଯଦି (linewise)
	ଫେରନ୍ତୁ qfp->qf_lnum <= pos->lnum;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ (qfp->qf_lnum < pos->lnum ||
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
    ସ୍ଥିର qfline_T *
qf_find_entry_after_pos(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bnr,
	pos_T		*pos,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linewise,
	qfline_T	*qfp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*errornr)
{
    ଯଦି (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	ଫେରନ୍ତୁ qfp;

    // Find the entry just before or at the position 'pos'
    ଯେତେବେଳେ (qfp->qf_next != ଖାଲି
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    ଯଦି (qfp->qf_next == ଖାଲି || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	ଫେରନ୍ତୁ ଖାଲି;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    ଫେରନ୍ତୁ qfp;
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
    ସ୍ଥିର qfline_T *
qf_find_entry_before_pos(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bnr,
	pos_T		*pos,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linewise,
	qfline_T	*qfp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*errornr)
{
    // Find the entry just before the position 'pos'
    ଯେତେବେଳେ (qfp->qf_next != ଖାଲି
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    ଯଦି (qf_entry_on_or_after_pos(qfp, pos, linewise))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_find_first_entry_on_ଧାଡ଼ି(qfp, errornr);

    ଫେରନ୍ତୁ qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    ସ୍ଥିର qfline_T *
qf_find_closest_entry(
	qf_list_T	*qfl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bnr,
	pos_T		*pos,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linewise,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    ଯଦି (qfp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;		// no entry in this file

    ଯଦି (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    ଅନ୍ୟଥା
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    ଫେରନ୍ତୁ qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_get_nth_below_entry(qfline_T *entry_arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା linewise, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errornr)
{
    qfline_T *entry = entry_arg;

    ଯେତେବେଳେ (n-- > 0 && !got_int)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_errornr = *errornr;

	ଯଦି (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_ଧାଡ଼ି(entry, errornr);

	ଯଦି (entry->qf_next == ଖାଲି
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    ଯଦି (linewise)
		*errornr = first_errornr;
	    ଭାଙ୍ଗ;
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
    ସ୍ଥିର ଶୂନ୍ୟ
qf_get_nth_above_entry(qfline_T *entry, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା linewise, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errornr)
{
    ଯେତେବେଳେ (n-- > 0 && !got_int)
    {
	ଯଦି (entry->qf_prev == ଖାଲି
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    ଭାଙ୍ଗ;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	ଯଦି (linewise)
	    entry = qf_find_first_entry_on_ଧାଡ଼ି(entry, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bnr,
	pos_T		*pos,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linewise)
{
    qfline_T	*adj_entry;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    ଯଦି (adj_entry == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    ଯଦି (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	ଯଦି (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	ଅନ୍ୟଥା
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    ଫେରନ୍ତୁ errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    ଶୂନ୍ୟ
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buf_has_flag;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errornr = 0;
    pos_T	pos;

    ଯଦି (eap->addr_ଗଣନା > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	ଫେରନ୍ତୁ;
    }

    // Check whether the current buffer has any quickfix entries
    ଯଦି (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    ଅନ୍ୟଥା
	buf_has_flag = BUF_HAS_LL_ENTRY;
    ଯଦି (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	ଫେରନ୍ତୁ;
    }

    ଯଦି ((qi = qf_cmd_get_ଗଦା(eap, TRUE)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    ଯଦି (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    ଅନ୍ୟଥା
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_ଗଣନା > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    ଯଦି (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    ଅନ୍ୟଥା
	emsg(_(e_no_more_ବସ୍ତୁଗୁଡ଼ିକ));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    ସ୍ଥିର char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    ବାଛ (cmdidx)
    {
	ସ୍ଥିତି CMD_cfile:	    ଫେରନ୍ତୁ (char_u *)"cfile";
	ସ୍ଥିତି CMD_cgetfile:  ଫେରନ୍ତୁ (char_u *)"cgetfile";
	ସ୍ଥିତି CMD_caddfile:  ଫେରନ୍ତୁ (char_u *)"caddfile";
	ସ୍ଥିତି CMD_lfile:	    ଫେରନ୍ତୁ (char_u *)"lfile";
	ସ୍ଥିତି CMD_lgetfile:  ଫେରନ୍ତୁ (char_u *)"lgetfile";
	ସ୍ଥିତି CMD_laddfile:  ଫେରନ୍ତୁ (char_u *)"laddfile";
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	    ଫେରନ୍ତୁ ଖାଲି;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    ଶୂନ୍ୟ
ex_cfile(exarg_T *eap)
{
    char_u	*enc = ଖାଲି;
    win_T	*wp = ଖାଲି;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = ଖାଲି;
    int_u	save_qfid = 0;		// init for gcc
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

    au_name = cfile_get_auname(eap->cmdidx);
    ଯଦି (au_name != ଖାଲି && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							ଖାଲି, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	ଯଦି (aborting())
	    ଫେରନ୍ତୁ;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    ଯଦି (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   ଖାଲି, ଖାଲି,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), ଖାଲି);
	ଯଦି (browse_file == ଖାଲି)
	    ଫେରନ୍ତୁ;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    ଅନ୍ୟଥା
# endif
    ଯଦି (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    ଯଦି (is_loclist_cmd(eap->cmdidx))
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
    ଯଦି (wp != ଖାଲି)
    {
	qi = GET_LOC_LIST(wp);
	ଯଦି (qi == ଖାଲି)
	{
	    decr_quickfix_busy();
	    ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା ଯଦି (qi == ଖାଲି)
    {
	decr_quickfix_busy();
	ଫେରନ୍ତୁ;
    }
    ଯଦି (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    ଯଦି (au_name != ଖାଲି)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, ଖାଲି, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ଯଦି (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    ସ୍ଥିର char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    ବାଛ (cmdidx)
    {
	ସ୍ଥିତି CMD_vimgrep:     ଫେରନ୍ତୁ (char_u *)"vimgrep";
	ସ୍ଥିତି CMD_lvimgrep:    ଫେରନ୍ତୁ (char_u *)"lvimgrep";
	ସ୍ଥିତି CMD_vimgrepadd:  ଫେରନ୍ତୁ (char_u *)"vimgrepadd";
	ସ୍ଥିତି CMD_lvimgrepadd: ଫେରନ୍ତୁ (char_u *)"lvimgrepadd";
	ସ୍ଥିତି CMD_grep:	      ଫେରନ୍ତୁ (char_u *)"grep";
	ସ୍ଥିତି CMD_lgrep:	      ଫେରନ୍ତୁ (char_u *)"lgrep";
	ସ୍ଥିତି CMD_grepadd:     ଫେରନ୍ତୁ (char_u *)"grepadd";
	ସ୍ଥିତି CMD_lgrepadd:    ଫେରନ୍ତୁ (char_u *)"lgrepadd";
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ ଖାଲି;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = ଖାଲି;

    ଯଦି (s == ଖାଲି || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	ଯଦି (last_ସନ୍ଧାନ_pat() == ଖାଲି)
	{
	    emsg(_(e_no_previous_regular_expression));
	    ଫେରନ୍ତୁ;
	}
	regmatch->regprog = vim_regcomp(last_ସନ୍ଧାନ_pat(), RE_MAGIC);
    }
    ଅନ୍ୟଥା
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    ସନ୍ଦେଶ_start();
    p = ସନ୍ଦେଶ_strtrunc(fname, TRUE);
    ଯଦି (p == ଖାଲି)
	ସନ୍ଦେଶ_outtrans(fname);
    ଅନ୍ୟଥା
    {
	ସନ୍ଦେଶ_outtrans(p);
	vim_free(p);
    }
    ସନ୍ଦେଶ_clr_eos();
    ସନ୍ଦେଶ_didout = FALSE;	    // overwrite this message
    ସନ୍ଦେଶ_nowait = TRUE;	    // don't wait for this message
    ସନ୍ଦେଶ_col = 0;
    out_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    ସ୍ଥିର buf_T *
vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_mls;
# ଯଦି defined(FEAT_SYN_HL)
    char_u	*save_ei = ଖାଲି;
# endif
    buf_T	*buf;

# ଯଦି defined(FEAT_SYN_HL)
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
# ଯଦି defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    ଫେରନ୍ତୁ buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    ଯଦି (!qflist_valid(wp, qfid))
    {
	ଯଦି (wp != ଖାଲି)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    ଫେରନ୍ତୁ FALSE;
	}
	ଅନ୍ୟଥା
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    ଫେରନ୍ତୁ TRUE;
	}
    }

    ଯଦି (qf_restore_list(qi, qfid) == FAIL)
	ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vgr_match_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	long	    *tomatch,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    duplicate_name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_match = FALSE;
    long	lnum;
    colnr_T	col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pat_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(spat);
    ଯଦି (pat_len > FUZZY_MATCH_MAX_LEN)
	pat_len = FUZZY_MATCH_MAX_LEN;

    ପାଇଁ (lnum = 1; lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା && *tomatch > 0; ++lnum)
    {
	col = 0;
	ଯଦି (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    ଯେତେବେଳେ (vim_regexec_multi(regmatch, curwin, buf, lnum,
								col, ଖାଲି) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ଯଦି (qf_add_entry(qfl,
			    ଖାଲି,	// dir
			    fname,
			    ଖାଲି,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].col + 1,
			    regmatch->endpos[0].col + 1,
			    FALSE,	// vis_col
			    ଖାଲି,	// search pattern
			    0,		// nr
			    0,		// type
			    ଖାଲି,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ଭାଙ୍ଗ;
		}
		found_match = TRUE;
		ଯଦି (--*tomatch == 0)
		    ଭାଙ୍ଗ;
		ଯଦି ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    ଭାଙ୍ଗ;
		col = regmatch->endpos[0].col
		    + (col == regmatch->endpos[0].col);
		ଯଦି (col > ml_get_buf_len(buf, lnum))
		    ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    score;
	    int_u   matches[FUZZY_MATCH_MAX_LEN];
	    int_u   sz = ARRAY_LENGTH(matches);

	    // Fuzzy string match
	    CLEAR_FIELD(matches);
	    ଯେତେବେଳେ (fuzzy_match(str + col, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ଯଦି (qf_add_entry(qfl,
			    ଖାଲି,	// dir
			    fname,
			    ଖାଲି,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + col + 1,
			    0,
			    FALSE,	// vis_col
			    ଖାଲି,	// search pattern
			    0,		// nr
			    0,		// type
			    ଖାଲି,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ଭାଙ୍ଗ;
		}
		found_match = TRUE;
		ଯଦି (--*tomatch == 0)
		    ଭାଙ୍ଗ;
		ଯଦି ((flags & VGR_GLOBAL) == 0)
		    ଭାଙ୍ଗ;
		col = matches[pat_len - 1] + col + 1;
		ଯଦି (col > linelen)
		    ଭାଙ୍ଗ;
	    }
	}
	ଧାଡ଼ି_breakcheck();
	ଯଦି (got_int)
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ found_match;
}

/*
 * Jump to the first match and update the directory.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vgr_jump_to_match(
	qf_info_T   *qi,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    forceit,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *redraw_for_dummy,
	buf_T	    *first_match_buf,
	char_u	    *target_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    ଯଦି (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    ଯଦି (curbuf == first_match_buf && target_dir != ଖାଲି)
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    long	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_args_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    CLEAR_POINTER(args);

    args->regmatch.regprog = ଖାଲି;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    ଯଦି (eap->addr_ଗଣନା > 0)
	args->tomatch = eap->line2;
    ଅନ୍ୟଥା
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    ଯଦି (p == ଖାଲି)
    {
	emsg(_(e_invalid_ସନ୍ଧାନ_pattern_or_delimiter));
	ଫେରନ୍ତୁ FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    ଯଦି (args->regmatch.regprog == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    p = skipwhite(p);
    ଯଦି (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	ଫେରନ୍ତୁ FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    ଯଦି ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
existing_swapfile(buf_T *buf)
{
    ଯଦି (buf->b_ml.ml_mfp != ଖାଲି && buf->b_ml.ml_mfp->mf_fname != ଖାଲି)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	ଫେରନ୍ତୁ fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_args_T	*cmd_args,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*redraw_for_dummy,
	buf_T		**first_match_buf,
	char_u		**target_dir)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fi;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		duplicate_name = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		using_dummy;
    char_u	*dirname_start = ଖାଲି;
    char_u	*dirname_now = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_match;
    aco_save_T	aco;

    dirname_start = alloc_id(MAXPATHL, aid_qf_dirname_start);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    ଯଦି (dirname_start == ଖାଲି || dirname_now == ଖାଲି)
	ଯାଅ theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_start, MAXPATHL);

    seconds = (time_t)0;
    ପାଇଁ (fi = 0; fi < cmd_args->fcount && !got_int && cmd_args->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_args->fnames[fi]);
	ଯଦି (time(ଖାଲି) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = time(ଖାଲି);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_args->fnames[fi]);
	ଯଦି (buf == ଖାଲି || buf->b_ml.ml_mfp == ଖାଲି)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != ଖାଲି);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_load_dummy_buf(fname, dirname_start, dirname_now);
	}
	ଅନ୍ୟଥା
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	ଯଦି (!vgr_qflist_valid(wp, qi, save_qfid, cmd_args->qf_title))
	    ଯାଅ theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	ଯଦି (buf == ଖାଲି)
	{
	    ଯଦି (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	ଅନ୍ୟଥା
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    found_match = vgr_match_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_args->spat, &cmd_args->regmatch,
		    &cmd_args->tomatch, duplicate_name, cmd_args->flags);

	    ଯଦି (using_dummy)
	    {
		ଯଦି (found_match && *first_match_buf == ଖାଲି)
		    *first_match_buf = buf;
		ଯଦି (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_buffer(buf, dirname_start);
		    buf = ଖାଲି;
		}
		ଅନ୍ୟଥା ଯଦି ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    ଯଦି (!found_match)
		    {
			wipe_dummy_buffer(buf, dirname_start);
			buf = ଖାଲି;
		    }
		    ଅନ୍ୟଥା ଯଦି (buf != *first_match_buf
					|| (cmd_args->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_buffer(buf, dirname_start);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = ଖାଲି;
		    }
		}

		ଯଦି (buf != ଖାଲି)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    ଯଦି (buf == *first_match_buf
			    && *target_dir == ଖାଲି
			    && STRCMP(dirname_start, dirname_now) != 0)
			*target_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    ଯଦି (curbuf == buf)
		    {
# ଯଦି defined(FEAT_SYN_HL)
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
    ଫେରନ୍ତୁ status;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    ଶୂନ୍ୟ
ex_vimgrep(exarg_T *eap)
{
    vgr_args_T	args;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		redraw_for_dummy = FALSE;
    buf_T	*first_match_buf = ଖାଲି;
    char_u	*target_dir = ଖାଲି;
    char_u	*au_name =  ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;

    ଯଦି (!check_can_set_curbuf_forceit(eap->forceit))
	ଫେରନ୍ତୁ;

    au_name = vgr_get_auname(eap->cmdidx);
    ଯଦି (au_name != ଖାଲି && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ଯଦି (aborting())
	    ଫେରନ୍ତୁ;
# endif
    }

    qi = qf_cmd_get_or_alloc_ଗଦା(eap, &wp);
    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (vgr_process_args(eap, &args) == FAIL)
	ଯାଅ theend;

    ଯଦି ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_ଗଦା_empty(qi))
	// make place for a new list
	qf_new_list(qi, args.qf_title);

    incr_quickfix_busy();

    status = vgr_process_files(wp, qi, &args, &redraw_for_dummy,
						&first_match_buf, &target_dir);
    ଯଦି (status != OK)
    {
	FreeWild(args.fcount, args.fnames);
	decr_quickfix_busy();
	ଯାଅ theend;
    }

    FreeWild(args.fcount, args.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_start;
    qfl->qf_index = 1;
    qf_list_changed(qfl);

    qf_update_buffer(qi, ଖାଲି);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    ଯଦି (au_name != ଖାଲି)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    ଯଦି (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	ଯାଅ theend;
    }

    // Jump to first match.
    ଯଦି (!qf_list_empty(qf_get_curlist(qi)))
    {
	ଯଦି ((args.flags & VGR_NOJUMP) == 0)
	    vgr_jump_to_match(qi, eap->forceit, &redraw_for_dummy,
		    first_match_buf, target_dir);
    }
    ଅନ୍ୟଥା
	semsg(_(e_no_match_str_2), args.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    ଯଦି (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    ଯଦି (dirname_now == ଖାଲି)
	ଫେରନ୍ତୁ;

    mch_dirname(dirname_now, MAXPATHL);
    ଯଦି (STRCMP(dirname_start, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = dirname_start;
	ea.cmdidx = (curwin->w_localdir == ଖାଲି) ? CMD_cd : CMD_lcd;
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
    ସ୍ଥିର buf_T *
load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = TRUE;
    aco_save_T	aco;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ଖାଲି, ଖାଲି, (linenr_T)1, BLN_DUMMY);
    ଯଦି (newbuf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    ଯଦି (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	ଯଦି (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (ଶୂନ୍ୟ)setfname(curbuf, fname, ଖାଲି, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_swap(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_to_wipe.br_buf = ଖାଲି;
	    readfile_result = readfile(fname, ଖାଲି,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			ଖାଲି, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    ଯଦି (readfile_result == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		ଯଦି (curbuf != newbuf)
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

	    ଯଦି (newbuf_to_wipe.br_buf != ଖାଲି && bufref_valid(&newbuf_to_wipe))
	    {
		block_autocmds();
		wipe_dummy_buffer(newbuf_to_wipe.br_buf, ଖାଲି);
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

    ଯଦି (!bufref_valid(&newbufref))
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    ଯେତେବେଳେ (buf->b_nwindows > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_one = FALSE;
	win_T	    *wp;

	ଯଦି (firstwin->w_next != ଖାଲି)
	    FOR_ALL_WINDOWS(wp)
		ଯଦି (wp->w_buffer == buf)
		{
		    ଯଦି (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    ଭାଙ୍ଗ;
		}
	ଯଦି (!did_one)
	    ଯାଅ fail;
    }

    ଯଦି (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# ଯଦି defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_buffer(buf, TRUE);

# ଯଦି defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	ଯଦି (dirname_start != ଖାଲି)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_start_dir(dirname_start);

	ଫେରନ୍ତୁ;
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
    ସ୍ଥିର ଶୂନ୍ୟ
unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    ଯଦି (curbuf == buf)		// safety check
	ଫେରନ୍ତୁ;

    close_buffer(ଖାଲି, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_start_dir(dirname_start);
}

# ଯଦି defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_qfline_ବସ୍ତୁଗୁଡ଼ିକ(qfline_T *qfp, list_T *list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    ଯଦି (bufnum != 0 && (buflist_findnr(bufnum) == ଖାଲି))
	bufnum = 0;

    ଯଦି ((dict = dict_alloc()) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	ଫେରନ୍ତୁ FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ଯଦି (dict_add_ସଂଖ୍ୟା(dict, "bufnr", (long)bufnum) == FAIL
	    || dict_add_ସଂଖ୍ୟା(dict, "lnum",     (long)qfp->qf_lnum) == FAIL
	    || dict_add_ସଂଖ୍ୟା(dict, "end_lnum", (long)qfp->qf_end_lnum) == FAIL
	    || dict_add_ସଂଖ୍ୟା(dict, "col",      (long)qfp->qf_col) == FAIL
	    || dict_add_ସଂଖ୍ୟା(dict, "end_col",  (long)qfp->qf_end_col) == FAIL
	    || dict_add_ସଂଖ୍ୟା(dict, "vcol",     (long)qfp->qf_viscol) == FAIL
	    || dict_add_ସଂଖ୍ୟା(dict, "nr",       (long)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string_len(dict, "type", buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen) == FAIL
	    || (qfp->qf_user_data.v_type != VAR_UNKNOWN
		&& dict_add_tv(dict, "user_data", &qfp->qf_user_data) == FAIL )
	    || dict_add_ସଂଖ୍ୟା(dict, "valid", (long)qfp->qf_valid) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (qi == ଖାଲି)
    {
	qi = ql_info;
	ଯଦି (wp != ଖାଲି)
	    qi = GET_LOC_LIST(wp);
	ଯଦି (qi == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

    }

    ଯଦି (eidx < 0)
	ଫେରନ୍ତୁ OK;

    ଯଦି (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    ଯଦି (qf_idx >= qi->qf_listcount)
	ଫେରନ୍ତୁ FAIL;

    qfl = qf_get_list(qi, qf_idx);
    ଯଦି (qf_list_empty(qfl))
	ଫେରନ୍ତୁ FAIL;

    FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(qfl, qfp, i)
    {
	ଯଦି (eidx > 0)
	{
	    ଯଦି (eidx == i)
		ଫେରନ୍ତୁ get_qfline_ବସ୍ତୁଗୁଡ଼ିକ(qfp, list);
	}
	ଅନ୍ୟଥା ଯଦି (get_qfline_ବସ୍ତୁଗୁଡ଼ିକ(qfp, list) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
ଗଣନା {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_ବସ୍ତୁଗୁଡ଼ିକ	= 0x2,
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_list_from_ଧାଡ଼ିଗୁଡ଼ିକ(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    ଯଦି (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    ଯଦି ((efm_di = dict_find(what, (char_u *)"efm", -1)) != ଖାଲି)
    {
	ଯଦି (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    qi = qf_alloc_ଗଦା(QFLT_INTERNAL, 1);
    ଯଦି (qi != ଖାଲି)
    {
	ଯଦି (qf_init_ext(qi, 0, ଖାଲି, ଖାଲି, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, ଖାଲି, ଖାଲି) > 0)
	{
	    (ଶୂନ୍ୟ)get_errorlist(qi, ଖାଲି, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_add_list(retdict, "items", l);
    status = OK;

    ଫେରନ୍ତୁ status;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    win = qf_find_win(qi);
    ଯଦି (win != ଖାଲି)
	ଫେରନ୍ତୁ win->w_id;
    ଫେରନ୍ତୁ 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	bufnum = 0;

    ଯଦି (qi != ଖାଲି && buflist_findnr(qi->qf_bufnr) != ଖାଲି)
	bufnum = qi->qf_bufnr;

    ଫେରନ୍ତୁ dict_add_ସଂଖ୍ୟା(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_keys2flags(dict_T *what, ପୂର୍ଣ୍ଣସଂଖ୍ୟା loclist)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = QF_GETLIST_NONE;

    ଯଦି (dict_has_ଚାବି(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	ଯଦି (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    ଯଦି (dict_has_ଚାବି(what, "title"))
	flags |= QF_GETLIST_TITLE;

    ଯଦି (dict_has_ଚାବି(what, "nr"))
	flags |= QF_GETLIST_NR;

    ଯଦି (dict_has_ଚାବି(what, "winid"))
	flags |= QF_GETLIST_WINID;

    ଯଦି (dict_has_ଚାବି(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    ଯଦି (dict_has_ଚାବି(what, "id"))
	flags |= QF_GETLIST_ID;

    ଯଦି (dict_has_ଚାବି(what, "items"))
	flags |= QF_GETLIST_ବସ୍ତୁଗୁଡ଼ିକ;

    ଯଦି (dict_has_ଚାବି(what, "idx"))
	flags |= QF_GETLIST_IDX;

    ଯଦି (dict_has_ଚାବି(what, "size"))
	flags |= QF_GETLIST_SIZE;

    ଯଦି (dict_has_ଚାବି(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    ଯଦି (loclist && dict_has_ଚାବି(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    ଯଦି (dict_has_ଚାବି(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    ଯଦି (dict_has_ଚାବି(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    ଫେରନ୍ତୁ flags;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    ଯଦି ((di = dict_find(what, (char_u *)"nr", -1)) != ଖାଲି)
    {
	// Use the specified quickfix/location list
	ଯଦି (di->di_tv.v_type == VAR_ସଂଖ୍ୟା)
	{
	    // for zero use the current list
	    ଯଦି (di->di_tv.vval.v_ସଂଖ୍ୟା != 0)
	    {
		qf_idx = di->di_tv.vval.v_ସଂଖ୍ୟା - 1;
		ଯଦି (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ଖାଲି
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	ଅନ୍ୟଥା
	    qf_idx = INVALID_QFIDX;
    }

    ଯଦି ((di = dict_find(what, (char_u *)"id", -1)) != ଖାଲି)
    {
	// Look for a list with the specified id
	ଯଦି (di->di_tv.v_type == VAR_ସଂଖ୍ୟା)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    ଯଦି (di->di_tv.vval.v_ସଂଖ୍ୟା != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_ସଂଖ୍ୟା);
	}
	ଅନ୍ୟଥା
	    qf_idx = INVALID_QFIDX;
    }

    ଫେରନ୍ତୁ qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_defaults(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା locstack, dict_T *retdict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;

    ଯଦି (flags & QF_GETLIST_TITLE)
	status = dict_add_string_len(retdict, "title", (char_u *)"", 0);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_ବସ୍ତୁଗୁଡ଼ିକ))
    {
	list_T	*l = list_alloc();
	ଯଦି (l != ଖାଲି)
	    status = dict_add_list(retdict, "items", l);
	ଅନ୍ୟଥା
	    status = FAIL;
    }
    ଯଦି ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_ସଂଖ୍ୟା(retdict, "nr", 0);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_ସଂଖ୍ୟା(retdict, "winid", qf_winid(qi));
    ଯଦି ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_ସଂଖ୍ୟା(retdict, "id", 0);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_ସଂଖ୍ୟା(retdict, "idx", 0);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_ସଂଖ୍ୟା(retdict, "size", 0);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_ସଂଖ୍ୟା(retdict, "changedtick", 0);
    ଯଦି ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_ସଂଖ୍ୟା(retdict, "filewinid", 0);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ଫେରନ୍ତୁ status;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    ଫେରନ୍ତୁ dict_add_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା winid = 0;

    ଯଦି (wp != ଖାଲି && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	ଯଦି (ll_wp != ଖାଲି)
	    winid = ll_wp->w_id;
    }

    ଫେରନ୍ତୁ dict_add_ସଂଖ୍ୟା(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_ବସ୍ତୁଗୁଡ଼ିକ(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା eidx, dict_T *retdict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;
    list_T	*l = list_alloc();
    ଯଦି (l != ଖାଲି)
    {
	(ଶୂନ୍ୟ)get_errorlist(qi, ଖାଲି, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    ଅନ୍ୟଥା
	status = FAIL;

    ଫେରନ୍ତୁ status;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;
    dictitem_T	*di;

    ଯଦି (qfl->qf_ctx != ଖାଲି)
    {
	di = dictitem_alloc((char_u *)"context");
	ଯଦି (di != ଖାଲି)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    ଯଦି (status == FAIL)
		dictitem_free(di);
	}
	ଅନ୍ୟଥା
	    status = FAIL;
    }
    ଅନ୍ୟଥା
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);

    ଫେରନ୍ତୁ status;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_idx(qf_list_T *qfl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା eidx, dict_T *retdict)
{
    ଯଦି (eidx == 0)
    {
	eidx = qfl->qf_index;
	ଯଦି (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    ଫେରନ୍ତୁ dict_add_ସଂଖ୍ୟା(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;

    ଯଦି (qfl->qf_qftf_cb.cb_name != ଖାଲି)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    ଅନ୍ୟଥା
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ଫେରନ୍ତୁ status;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx = INVALID_QFIDX;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eidx = 0;
    dictitem_T	*di;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = QF_GETLIST_NONE;

    ଯଦି ((di = dict_find(what, (char_u *)"lines", -1)) != ଖାଲି)
	ଫେରନ୍ତୁ qf_get_list_from_ଧାଡ଼ିଗୁଡ଼ିକ(what, di, retdict);

    ଯଦି (wp != ଖାଲି)
	qi = GET_LOC_LIST(wp);
    ଅନ୍ୟଥା ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    flags = qf_getprop_keys2flags(what, (wp != ଖାଲି));

    ଯଦି (!qf_ଗଦା_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    ଯଦି (qf_ଗଦା_empty(qi) || qf_idx == INVALID_QFIDX)
	ଫେରନ୍ତୁ qf_getprop_defaults(qi, flags, wp != ଖାଲି, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    ଯଦି ((di = dict_find(what, (char_u *)"idx", -1)) != ଖାଲି)
    {
	ଯଦି (di->di_tv.v_type != VAR_ସଂଖ୍ୟା)
	    ଫେରନ୍ତୁ FAIL;
	eidx = di->di_tv.vval.v_ସଂଖ୍ୟା;
    }

    ଯଦି (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_ସଂଖ୍ୟା(retdict, "nr", qf_idx + 1);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_ସଂଖ୍ୟା(retdict, "winid", qf_winid(qi));
    ଯଦି ((status == OK) && (flags & QF_GETLIST_ବସ୍ତୁଗୁଡ଼ିକ))
	status = qf_getprop_ବସ୍ତୁଗୁଡ଼ିକ(qi, qf_idx, eidx, retdict);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_ସଂଖ୍ୟା(retdict, "id", qfl->qf_id);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_ସଂଖ୍ୟା(retdict, "size", qfl->qf_ଗଣନା);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_ସଂଖ୍ୟା(retdict, "changedtick", qfl->qf_changedtick);
    ଯଦି ((status == OK) && (wp != ଖାଲି) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    ଯଦି ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    ଫେରନ୍ତୁ status;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_entry,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*valid_entry)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufnum, valid, status, col, end_col, vcol, nr;
    long	lnum, end_lnum;

    ଯଦି (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "bufnr");
    lnum = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "lnum");
    end_lnum = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "end_lnum");
    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "col");
    end_col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "end_col");
    vcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "vcol");
    nr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    ଯଦି (text == ଖାଲି)
	text = vim_strsave((char_u *)"");
    typval_T user_data;
    user_data.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_data);

    valid = TRUE;
    ଯଦି ((filename == ଖାଲି && bufnum == 0) || (lnum == 0 && pattern == ଖାଲି))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    ଯଦି (bufnum != 0 && (buflist_findnr(bufnum) == ଖାଲି))
    {
	ଯଦି (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    ଯଦି (dict_has_ଚାବି(d, "valid"))
	valid = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			ଖାଲି,		// dir
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
			type == ଖାଲି ? NUL : *type,
			&user_data,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);
    clear_tv(&user_data);

    ଯଦି (valid)
	*valid_entry = TRUE;

    ଫେରନ୍ତୁ status;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
entry_is_closer_to_target(
	qfline_T	*entry,
	qfline_T	*other_entry,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		target_fnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		target_lnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		target_col)
{
    // First, compare entries to target file.
    ଯଦି (!target_fnum)
	// Without a target file, we can't know which is closer.
	ଫେରନ୍ତୁ FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_target_file = entry->qf_fnum && entry->qf_fnum == target_fnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା other_is_target_file = other_entry->qf_fnum && other_entry->qf_fnum == target_fnum;
    ଯଦି (!is_target_file && other_is_target_file)
	ଫେରନ୍ତୁ FALSE;
    ଅନ୍ୟଥା ଯଦି (is_target_file && !other_is_target_file)
	ଫେରନ୍ତୁ TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    ଯଦି (!target_lnum)
	// Without a target line number, we can't know which is closer.
	ଫେରନ୍ତୁ FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି_distance = entry->qf_lnum ? labs(entry->qf_lnum - target_lnum) : INT_MAX;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା other_ଧାଡ଼ି_distance = other_entry->qf_lnum ? labs(other_entry->qf_lnum - target_lnum) : INT_MAX;
    ଯଦି (ଧାଡ଼ି_distance > other_ଧାଡ଼ି_distance)
	ଫେରନ୍ତୁ FALSE;
    ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି_distance < other_ଧାଡ଼ି_distance)
	ଫେରନ୍ତୁ TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    ଯଦି (!target_col)
	// Without a target column, we can't know which is closer.
	ଫେରନ୍ତୁ FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା column_distance = entry->qf_col ? abs(entry->qf_col - target_col) : INT_MAX;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା other_column_distance = other_entry->qf_col ? abs(other_entry->qf_col - target_col): INT_MAX;
    ଯଦି (column_distance > other_column_distance)
	ଫେରନ୍ତୁ FALSE;
    ଅନ୍ୟଥା ଯଦି (column_distance < other_column_distance)
	ଫେରନ୍ତୁ TRUE;

    // It's a complete tie! The exact same file, line, and column.
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_add_entries(
	qf_info_T	*qi,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx,
	list_T		*list,
	char_u		*title,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_last = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		valid_entry = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_fnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_lnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_col = 0;
    ଯଦି (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_col = qfl->qf_ptr->qf_col;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା select_first_entry = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା select_nearest_entry = FALSE;

    ଯଦି (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_first_entry = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ଅନ୍ୟଥା ଯଦି (action == 'a')
    {
	ଯଦି (qf_list_empty(qfl))
	    // Appending to empty list, select first entry.
	    select_first_entry = TRUE;
	ଅନ୍ୟଥା
	    // Adding to existing list, use last entry.
	    old_last = qfl->qf_last;
    }
    ଅନ୍ୟଥା ଯଦି (action == 'r')
    {
	select_first_entry = TRUE;
	qf_free_ବସ୍ତୁଗୁଡ଼ିକ(qfl);
	qf_store_title(qfl, title);
    }
    ଅନ୍ୟଥା ଯଦି (action == 'u')
    {
	select_nearest_entry = TRUE;
	qf_free_ବସ୍ତୁଗୁଡ଼ିକ(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *entry_to_select = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା entry_to_select_index = 0;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(list, li)
    {
	ଯଦି (li->li_tv.v_type != VAR_DICT)
	    ଜାରି; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	ଯଦି (d == ଖାଲି)
	    ଜାରି;

	retval = qf_add_entry_from_dict(qfl, d, li == list->lv_first,
								&valid_entry);
	ଯଦି (retval == QF_FAIL)
	    ଭାଙ୍ଗ;

	qfline_T *entry = qfl->qf_last;
	ଯଦି (
	    (select_first_entry && entry_to_select == ଖାଲି) ||
	    (select_nearest_entry &&
		(entry_to_select == ଖାଲି ||
		 entry_is_closer_to_target(entry, entry_to_select, prev_fnum,
					   prev_lnum, prev_col))))
	{
	    entry_to_select = entry;
	    entry_to_select_index = qfl->qf_ଗଣନା;
	}
    }

    // Check if any valid error entries are added to the list.
    ଯଦି (valid_entry)
	qfl->qf_nonevalid = FALSE;
    ଅନ୍ୟଥା ଯଦି (qfl->qf_index == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    ଯଦି (entry_to_select)
    {
	qfl->qf_ptr = entry_to_select;
	qfl->qf_index = entry_to_select_index;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_buffer(qi, old_last);

    ଫେରନ୍ତୁ retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*newlist)
{
    dictitem_T	*di;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx = qi->qf_curlist;    // default is the current list

    ଯଦି ((di = dict_find(what, (char_u *)"nr", -1)) != ଖାଲି)
    {
	// Use the specified quickfix/location list
	ଯଦି (di->di_tv.v_type == VAR_ସଂଖ୍ୟା)
	{
	    // for zero use the current list
	    ଯଦି (di->di_tv.vval.v_ସଂଖ୍ୟା != 0)
		qf_idx = di->di_tv.vval.v_ସଂଖ୍ୟା - 1;

	    ଯଦି ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_ଗଦା_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    ଅନ୍ୟଥା ଯଦି (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		ଫେରନ୍ତୁ INVALID_QFIDX;
	    ଅନ୍ୟଥା ଯଦି (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	ଅନ୍ୟଥା ଯଦି (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ଖାଲି
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    ଯଦି (!qf_ଗଦା_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    ଅନ୍ୟଥା ଯଦି (*newlist)
		qf_idx = 0;
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ INVALID_QFIDX;
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ INVALID_QFIDX;
    }

    ଯଦି (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != ଖାଲି)
    {
	// Use the quickfix/location list with the specified id
	ଯଦି (di->di_tv.v_type != VAR_ସଂଖ୍ୟା)
	    ଫେରନ୍ତୁ INVALID_QFIDX;

	ଫେରନ୍ତୁ qf_id2nr(qi, di->di_tv.vval.v_ସଂଖ୍ୟା);
    }

    ଫେରନ୍ତୁ qf_idx;
}

/*
 * Set the quickfix list title.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_setprop_title(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    ଯଦି (di->di_tv.v_type != VAR_STRING)
	ଫେରନ୍ତୁ FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    ଯଦି (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    ଫେରନ୍ତୁ OK;
}

/*
 * Set quickfix list items/entries.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_setprop_ବସ୍ତୁଗୁଡ଼ିକ(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qf_idx, dictitem_T *di, ପୂର୍ଣ୍ଣସଂଖ୍ୟା action)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
    char_u	*title_save;

    ଯଦି (di->di_tv.v_type != VAR_LIST)
	ଫେରନ୍ତୁ FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    ଫେରନ୍ତୁ retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_setprop_ବସ୍ତୁଗୁଡ଼ିକ_from_ଧାଡ଼ିଗୁଡ଼ିକ(
	qf_info_T	*qi,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    ଯଦି ((efm_di = dict_find(what, (char_u *)"efm", -1)) != ଖାଲି)
    {
	ଯଦି (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    ଯଦି (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (action == 'r' || action == 'u')
	qf_free_ବସ୍ତୁଗୁଡ଼ିକ(&qi->qf_lists[qf_idx]);
    ଯଦି (qf_init_ext(qi, qf_idx, ଖାଲି, ଖାଲି, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, ଖାଲି, ଖାଲି) >= 0)
	retval = OK;

    ଫେରନ୍ତୁ retval;
}

/*
 * Set quickfix list context.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    ଯଦି (ctx != ଖାଲି)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    ଫେରନ୍ତୁ OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		denote = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    ଯଦି (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != ଖାଲି
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_ଗଣନା;
    ଅନ୍ୟଥା
    {
	// Otherwise use the specified index
	newidx = tv_get_ସଂଖ୍ୟା_chk(&di->di_tv, &denote);
	ଯଦି (denote)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (newidx < 1)		// sanity check
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (newidx > qfl->qf_ଗଣନା)
	newidx = qfl->qf_ଗଣନା;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    ଯଦି (qf_ptr == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    ଯଦି (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    ଫେରନ୍ତୁ OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    ଯଦି (cb.cb_name == ଖାଲି || *cb.cb_name == NUL)
	ଫେରନ୍ତୁ OK;
    set_callback(&qfl->qf_qftf_cb, &cb);
    ଫେରନ୍ତୁ OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
qf_set_properties(qf_info_T *qi, dict_T *what, ପୂର୍ଣ୍ଣସଂଖ୍ୟା action, char_u *title)
{
    dictitem_T	*di;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newlist = FALSE;
    qf_list_T	*qfl;

    ଯଦି (action == ' ' || qf_ଗଦା_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    ଯଦି (qf_idx == INVALID_QFIDX)	// List not found
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    ଯଦି ((di = dict_find(what, (char_u *)"title", -1)) != ଖାଲି)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    ଯଦି ((di = dict_find(what, (char_u *)"items", -1)) != ଖାଲି)
	retval = qf_setprop_ବସ୍ତୁଗୁଡ଼ିକ(qi, qf_idx, di, action);
    ଯଦି ((di = dict_find(what, (char_u *)"lines", -1)) != ଖାଲି)
	retval = qf_setprop_ବସ୍ତୁଗୁଡ଼ିକ_from_ଧାଡ଼ିଗୁଡ଼ିକ(qi, qf_idx, what, di, action);
    ଯଦି ((di = dict_find(what, (char_u *)"context", -1)) != ଖାଲି)
	retval = qf_setprop_context(qfl, di);
    ଯଦି ((di = dict_find(what, (char_u *)"idx", -1)) != ଖାଲି)
	retval = qf_setprop_curidx(qi, qfl, di);
    ଯଦି ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != ଖାଲି)
	retval = qf_setprop_qftf(qi, qfl, di);

    ଯଦି (newlist || retval == OK)
	qf_list_changed(qfl);
    ଯଦି (newlist)
	qf_update_buffer(qi, ଖାଲି);

    ଫେରନ୍ତୁ retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
qf_free_ଗଦା(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_find_win(qi);
    win_T	*llwin = ଖାଲି;

    ଯଦି (qfwin != ଖାଲି)
    {
	// If the quickfix/location list window is open, then clear it
	ଯଦି (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_buffer(qi, ଖାଲି);
    }

    ଯଦି (wp != ଖାଲି && IS_LL_WINDOW(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_find_win_with_loclist(qi);
	ଯଦି (llwin != ଖାଲି)
	    wp = llwin;
    }

    qf_free_all(wp);
    ଯଦି (wp == ଖାଲି)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    ଅନ୍ୟଥା ଯଦି (qfwin != ଖାଲି)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_ଗଦା(QFLT_LOCATION, wp->w_p_lhi);

	ଯଦି (new_ll != ଖାଲି)
	{
	    new_ll->qf_bufnr = qfwin->w_buffer->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    ଯଦି (wp != qfwin)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_errorlist(
	win_T	*wp,
	list_T	*list,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;

    ଯଦି (wp != ଖାଲି)
	qi = ll_get_or_alloc_list(wp);
    ଅନ୍ୟଥା
	qi = ql_info;
    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_ଗଦା(wp, qi);
	ଫେରନ୍ତୁ OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    ଯଦି (list->lv_len != 0 && what != ଖାଲି)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	ଫେରନ୍ତୁ FAIL;
    }

    incr_quickfix_busy();

    ଯଦି (what != ଖାଲି)
	retval = qf_set_properties(qi, what, action, title);
    ଅନ୍ୟଥା
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	ଯଦି (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    ଫେରନ୍ତୁ retval;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark_quickfix_user_data(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା abort = FALSE;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	ଯଦି (!qfl->qf_has_user_data)
	    ଜାରି;
	qfline_T *qfp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;
	FOR_ALL_QFL_ବସ୍ତୁଗୁଡ଼ିକ(qfl, qfp, j)
	{
	    typval_T* user_data = &qfp->qf_user_data;
	    ଯଦି (user_data != ଖାଲି && user_data->v_type != VAR_ସଂଖ୍ୟା
		&& user_data->v_type != VAR_STRING && user_data->v_type != VAR_FLOAT)
		abort = abort || set_ref_in_item(user_data, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
	}
    }
    ଫେରନ୍ତୁ abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mark_quickfix_ctx(qf_info_T *qi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    ପାଇଁ (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	ଯଦି (ctx != ଖାଲି && ctx->v_type != VAR_ସଂଖ୍ୟା
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || set_ref_in_item(ctx, copyID, ଖାଲି, ଖାଲି, ଖାଲି);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || set_ref_in_callback(cb, copyID);
    }

    ଫେରନ୍ତୁ abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_quickfix(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    ଯଦି (ql_info == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    ଯଦି (abort)
	ଫେରନ୍ତୁ abort;

    abort = mark_quickfix_user_data(ql_info, copyID);
    ଯଦି (abort)
	ଫେରନ୍ତୁ abort;

    abort = set_ref_in_callback(&qftf_cb, copyID);
    ଯଦି (abort)
	ଫେରନ୍ତୁ abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	ଯଦି (win->w_llist != ଖାଲି)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    ଯଦି (abort)
		ଫେରନ୍ତୁ abort;

	    abort = mark_quickfix_user_data(win->w_llist, copyID);
	    ଯଦି (abort)
		ଫେରନ୍ତୁ abort;
	}
	ଯଦି (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    ଯଦି (abort)
		ଫେରନ୍ତୁ abort;

	    abort = mark_quickfix_user_data(win->w_llist_ref, copyID);
	    ଯଦି (abort)
		ଫେରନ୍ତୁ abort;
	}
    }

    ଫେରନ୍ତୁ abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    ସ୍ଥିର char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    ବାଛ (cmdidx)
    {
	ସ୍ଥିତି CMD_cbuffer:	ଫେରନ୍ତୁ (char_u *)"cbuffer";
	ସ୍ଥିତି CMD_cgetbuffer:	ଫେରନ୍ତୁ (char_u *)"cgetbuffer";
	ସ୍ଥିତି CMD_caddbuffer:	ଫେରନ୍ତୁ (char_u *)"caddbuffer";
	ସ୍ଥିତି CMD_lbuffer:	ଫେରନ୍ତୁ (char_u *)"lbuffer";
	ସ୍ଥିତି CMD_lgetbuffer:	ଫେରନ୍ତୁ (char_u *)"lgetbuffer";
	ସ୍ଥିତି CMD_laddbuffer:	ଫେରନ୍ତୁ (char_u *)"laddbuffer";
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:		ଫେରନ୍ତୁ ଖାଲି;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = ଖାଲି;

    ଯଦି (*eap->arg == NUL)
	buf = curbuf;
    ଅନ୍ୟଥା ଯଦି (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((ଅକ୍ଷର *)eap->arg));

    ଯଦି (buf == ଖାଲି)
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)
    {
	emsg(_(e_buffer_is_not_loaded));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (eap->addr_ଗଣନା == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    }

    ଯଦି (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	emsg(_(e_invalid_range));
	ଫେରନ୍ତୁ FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    ଫେରନ୍ତୁ OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    ଶୂନ୍ୟ
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = ଖାଲି;
    qf_info_T	*qi;
    char_u	*au_name = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    int_u	save_qfid;
    win_T	*wp = ଖାଲି;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    ଯଦି (au_name != ଖାଲି && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ଯଦି (aborting())
	    ଫେରନ୍ତୁ;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_ଗଦା(eap, &wp);
    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (cbuffer_process_args(eap, &buf, &line1, &line2) == FAIL)
	ଫେରନ୍ତୁ;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    ଯଦି (buf->b_sfname)
    {
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "%s (%s)",
		(ଅକ୍ଷର *)qf_title, (ଅକ୍ଷର *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, ଖାଲି, buf, ଖାଲି, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, ଖାଲି);
    ଯଦି (qf_ଗଦା_empty(qi))
    {
	decr_quickfix_busy();
	ଫେରନ୍ତୁ;
    }
    ଯଦି (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ଯଦି (au_name != ଖାଲି)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	ଯଦି (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ଯଦି (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# ଯଦି defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    ବାଛ (cmdidx)
    {
	ସ୍ଥିତି CMD_cexpr:	    ଫେରନ୍ତୁ (char_u *)"cexpr";
	ସ୍ଥିତି CMD_cgetexpr:  ଫେରନ୍ତୁ (char_u *)"cgetexpr";
	ସ୍ଥିତି CMD_caddexpr:  ଫେରନ୍ତୁ (char_u *)"caddexpr";
	ସ୍ଥିତି CMD_lexpr:	    ଫେରନ୍ତୁ (char_u *)"lexpr";
	ସ୍ଥିତି CMD_lgetexpr:  ଫେରନ୍ତୁ (char_u *)"lgetexpr";
	ସ୍ଥିତି CMD_laddexpr:  ଫେରନ୍ତୁ (char_u *)"laddexpr";
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	    ଫେରନ୍ତୁ ଖାଲି;
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
trigger_cexpr_autocmd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    ଯଦି (au_name != ଖାଲି && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	ଯଦି (aborting())
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = ଖାଲି;

    qi = qf_cmd_get_or_alloc_ଗଦା(eap, &wp);
    ଯଦି (qi == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି ((tv->v_type == VAR_STRING && tv->vval.v_string != ଖାଲି)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != ଖାଲି))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, ଖାଲି, ଖାଲି, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), ଖାଲି);
	ଯଦି (qf_ଗଦା_empty(qi))
	{
	    decr_quickfix_busy();
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (res >= 0)
	    qf_list_changed(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	ଯଦି (au_name != ଖାଲି)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	ଯଦି (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_first(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	ଫେରନ୍ତୁ OK;
    }

    emsg(_(e_string_or_list_expected));
    ଫେରନ୍ତୁ FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    ଶୂନ୍ୟ
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    ଯଦି (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	ଫେରନ୍ତୁ;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_ପଦସମଷ୍ଟି(eap->arg, eap);
    ଯଦି (tv == ଖାଲି)
	ଫେରନ୍ତୁ;

    (ଶୂନ୍ୟ)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    ସ୍ଥିର qf_info_T *
hgr_get_ll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    ଯଦି (bt_help(curwin->w_buffer))
	wp = curwin;
    ଅନ୍ୟଥା
	// Find an existing help window
	wp = qf_find_help_win();

    ଯଦି (wp == ଖାଲି)	    // Help window not found
	qi = ଖାଲି;
    ଅନ୍ୟଥା
	qi = wp->w_llist;

    ଯଦି (qi == ଖାଲି)
    {
	// Allocate a new location list for help text matches
	ଯଦି ((qi = qf_alloc_ଗଦା(QFLT_LOCATION, 1)) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	*new_ll = TRUE;
    }

    ଫେରନ୍ତୁ qi;
}

/*
 * Search for a pattern in a help file.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hgr_ସନ୍ଧାନ_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    long	lnum;

    fd = mch_fopen((ଅକ୍ଷର *)fname, "r");
    ଯଦି (fd == ଖାଲି)
	ଫେରନ୍ତୁ;

    lnum = 1;
    ଯେତେବେଳେ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *ଧାଡ଼ି = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	ଯଦି (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    ଧାଡ଼ି = string_convert(p_vc, IObuff, ଖାଲି);
	    ଯଦି (ଧାଡ଼ି == ଖାଲି)
		ଧାଡ଼ି = IObuff;
	}

	ଯଦି (vim_regexec(p_regmatch, ଧାଡ଼ି, (colnr_T)0))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଧାଡ଼ି);

	    // remove trailing CR, LF, spaces, etc.
	    ଯେତେବେଳେ (l > 0 && ଧାଡ଼ି[l - 1] <= ' ')
		ଧାଡ଼ି[--l] = NUL;

	    ଯଦି (qf_add_entry(qfl,
			ଖାଲି,	// dir
			fname,
			ଖାଲି,
			0,
			ଧାଡ଼ି,
			lnum,
			0,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p_regmatch->startp[0] - ଧାଡ଼ି)
			+ 1,	// col
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p_regmatch->endp[0] - ଧାଡ଼ି)
			+ 1,	// end_col
			FALSE,	// vis_col
			ଖାଲି,	// search pattern
			0,	// nr
			1,	// type
			ଖାଲି,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		ଯଦି (ଧାଡ଼ି != IObuff)
		    vim_free(ଧାଡ଼ି);
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (ଧାଡ଼ି != IObuff)
	    vim_free(ଧାଡ଼ି);
	++lnum;
	ଧାଡ଼ି_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hgr_ସନ୍ଧାନ_files_in_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fcount;
    char_u	**fnames;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    add_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    ଯଦି (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	ପାଇଁ (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    ଯଦି (lang != ଖାଲି
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		ଜାରି;
# endif

	    hgr_ସନ୍ଧାନ_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    ସ୍ଥିର ଶୂନ୍ୟ
hgr_ସନ୍ଧାନ_in_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    ଯଦି (!enc_utf8)
	convert_setup(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    ଯେତେବେଳେ (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_ସନ୍ଧାନ_files_in_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    ଯଦି (vc.vc_type != CONV_NONE)
	convert_setup(&vc, ଖାଲି, ଖାଲି);
}

/*
 * ":helpgrep {pattern}"
 */
    ଶୂନ୍ୟ
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_qi = FALSE;
    char_u	*au_name =  ଖାଲି;
    char_u	*lang = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		updated = FALSE;

    ବାଛ (eap->cmdidx)
    {
	ସ୍ଥିତି CMD_helpgrep:  au_name = (char_u *)"helpgrep"; ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;
    }
    ଯଦି (au_name != ଖାଲି && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ଯଦି (aborting())
	    ଫେରନ୍ତୁ;
# endif
    }

    ଯଦି (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	ଯଦି (qi == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା ଯଦି (qi == ଖାଲି)
    {
	emsg(_(e_no_quickfix_ଗଦା));
	ଫେରନ୍ତୁ;
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
    ଯଦି (regmatch.regprog != ଖାଲି)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_ସନ୍ଧାନ_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    ଯଦି (p_cpo == empty_option)
	p_cpo = save_cpo;
    ଅନ୍ୟଥା
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	ଯଦି (*p_cpo == NUL)
	    set_option_ମୂଲ୍ୟ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	ଯଦି (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    ଯଦି (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, ଖାଲି);

    ଯଦି (au_name != ଖାଲି)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	ଯଦି (!new_qi && IS_LL_ଗଦା(qi) && qf_find_win_with_loclist(qi) == ଖାଲି)
	{
	    decr_quickfix_busy();
	    ଫେରନ୍ତୁ;
	}
    }

    // Jump to first match.
    ଯଦି (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    ଅନ୍ୟଥା
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    ଯଦି (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	ଯଦି (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    ଯଦି (new_qi)
		ll_free_all(&qi);
	}
	ଅନ୍ୟଥା ଯଦି (curwin->w_llist == ଖାଲି && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_quickfix(ଶୂନ୍ୟ)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(ଖାଲି);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#endif // FEAT_QUICKFIX

#ଯଦି defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    ସ୍ଥିର ଶୂନ୍ୟ
get_qf_loc_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    ଯଦି (what_arg->v_type == VAR_UNKNOWN)
    {
	ଯଦି (rettv_list_alloc(rettv) == OK)
	    ଯଦି (is_qf || wp != ଖାଲି)
		(ଶୂନ୍ୟ)get_errorlist(ଖାଲି, wp, -1, 0, rettv->vval.v_list);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (rettv_dict_alloc(rettv) == OK)
	    ଯଦି (is_qf || (wp != ଖାଲି))
	    {
		ଯଦି (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    ଯଦି (d != ଖାଲି)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		ଅନ୍ୟଥା
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    ଶୂନ୍ୟ
f_getloclist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    ଶୂନ୍ୟ
f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    ଯଦି (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    get_qf_loc_list(TRUE, ଖାଲି, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = 0;
# endif

    rettv->vval.v_ସଂଖ୍ୟା = -1;

# ifdef FEAT_QUICKFIX
    ଯଦି (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    ଅନ୍ୟଥା ଯଦି (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    ଅନ୍ୟଥା
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	valid_dict = TRUE;

	ଯଦି (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    ଯଦି (act == ଖାଲି)
		ଫେରନ୍ତୁ;		// type error; errmsg already given
	    ଯଦି ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    ଅନ୍ୟଥା
		semsg(_(e_invalid_action_str_1), act);
	}
	ଅନ୍ୟଥା ଯଦି (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	ଅନ୍ୟଥା
	    emsg(_(e_string_required));

	ଯଦି (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    ଯଦି (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != ଖାଲି)
		what = what_arg->vval.v_dict;
	    ଅନ୍ୟଥା
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	ଯଦି (l != ଖାଲି && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == ଖାଲି ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_ସଂଖ୍ୟା = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    ଶୂନ୍ୟ
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	ଫେରନ୍ତୁ;

    win = find_win_by_nr_or_id(&argvars[0]);
    ଯଦି (win != ଖାଲି)
	set_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    ଶୂନ୍ୟ
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    set_qf_ll_list(ଖାଲି, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#endif
