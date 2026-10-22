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

#شامل "vim.h"

#اگر defined(FEAT_QUICKFIX)

ساخت dir_ڈھیر_T
{
    ساخت dir_ڈھیر_T	*اگلا;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
قسمتعریف ساخت qfline_S qfline_T;
ساخت qfline_S
{
    qfline_T	*qf_اگلا;	// pointer to next error in the list
    qfline_T	*qf_پچھلا;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_انتہا_lnum;	// line number when the error has range or zero
    صحیح		qf_fnum;	// file number for the line
    صحیح		qf_ستون;		// column where the error occurred
    صحیح		qf_انتہا_ستون;	// column when the error has range or zero
    صحیح		qf_nr;		// error number
    char_u	*qf_module;	// module name for this error
    char_u	*qf_fname;	// different filename if there're hard links
    char_u	*qf_نمونہ;	// search pattern for the error
    char_u	*qf_متن;	// description of the error
    char_u	qf_viscol;	// set to TRUE if qf_col and qf_end_col is
				// screen column
    char_u	qf_cleared;	// set to TRUE if line has been deleted
    char_u	qf_type;	// type of the error (mostly 'E'); 1 for
				// :helpgrep
    typval_T	qf_user_ڈیٹا;	// custom user data associated with this item
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
قسمتعریف شمار
{
    QFLT_QUICKFIX, // Quickfix list - global list
    QFLT_LOCATION, // Location list - per window list
    QFLT_اندرونی  // Internal - Temporary list used by getqflist()/getloclist()
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
قسمتعریف ساخت qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_آغاز;	// pointer to the first error
    qfline_T	*qf_آخری;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    صحیح		qf_گنتی;	// number of errors (0 means empty list)
    صحیح		qf_اشاریہ;	// current index in the error list
    صحیح		qf_nonevalid;	// TRUE if not a single valid entry found
    صحیح		qf_has_user_ڈیٹا; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    ساخت dir_ڈھیر_T	*qf_dir_ڈھیر;
    char_u		*qf_directory;
    ساخت dir_ڈھیر_T	*qf_file_ڈھیر;
    char_u		*qf_currfile;
    صحیح			qf_multiline;
    صحیح			qf_multiignore;
    صحیح			qf_multiscan;
    طویل		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
ساخت qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    صحیح		qf_refcount;
    صحیح		qf_listcount;	    // current number of lists
    صحیح		qf_curlist;	    // current error list
    صحیح         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    صحیح		qf_bufnr;	    // quickfix window buffer number
};

ساکن qf_info_T ql_info_actual; // global quickfix list
ساکن qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
ساکن int_u آخری_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
قسمتعریف ساخت efm_S efm_T;
ساخت efm_S
{
    regprog_T	    *prog;	// pre-formatted part of 'errorformat'
    efm_T	    *اگلا;	// pointer to next (NULL if last)
    char_u	    addr[FMT_PATTERNS]; // indices of used % patterns
    char_u	    سابقہ;	// prefix of this format line:
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
    صحیح		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
قسمتعریف ساخت qf_delq_S
{
    ساخت qf_delq_S	*اگلا;
    qf_info_T		*qi;
} qf_delq_T;
ساکن qf_delq_T *qf_delq_سر = تہی;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
ساکن صحیح	quickfix_busy = 0;

ساکن efm_T	*fmt_آغاز = تہی; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
ساکن callback_T qftf_cb;

ساکن خالی     qf_نکالو_ڈھیر(qf_info_T *qi, صحیح adjust);
ساکن خالی	qf_new_list(qf_info_T *qi, char_u *qf_title);
ساکن صحیح	qf_جوڑو_اندراج(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, صحیح bufnum, char_u *mesg, طویل lnum, طویل انتہا_lnum, صحیح ستون, صحیح انتہا_ستون, صحیح vis_ستون, char_u *نمونہ, صحیح nr, صحیح type, typval_T *user_ڈیٹا, صحیح valid);
ساکن صحیح      qf_resize_ڈھیر_بنیاد(qf_info_T *qi, صحیح n);
ساکن خالی     qf_sync_llw_مقصد_win(win_T *llw);
ساکن خالی     qf_sync_win_مقصد_llw(win_T *pwp);
ساکن qf_info_T *qf_alloc_ڈھیر(qfltype_T qfltype, صحیح n);
ساکن qf_list_T *qf_alloc_list_ڈھیر(صحیح n);
ساکن خالی	qf_free(qf_list_T *qfl);
ساکن char_u	*qf_types(صحیح, صحیح);
ساکن صحیح	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
ساکن char_u	*qf_ڈالو_dir(char_u *, ساخت dir_ڈھیر_T **, صحیح is_file_ڈھیر);
ساکن char_u	*qf_نکالو_dir(ساخت dir_ڈھیر_T **);
ساکن char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
ساکن win_T    *qf_ڈھونڈو_win_with_loclist(qf_info_T *ll);
ساکن خالی	qf_jump_newwin(qf_info_T *qi, صحیح dir, صحیح errornr, صحیح forceit, صحیح newwin);
ساکن خالی	qf_fmt_متن(garray_T *gap, char_u *متن);
ساکن خالی	qf_range_متن(garray_T *gap, qfline_T *qfp);
ساکن صحیح	qf_win_pos_update(qf_info_T *qi, صحیح old_qf_اشاریہ);
ساکن win_T	*qf_ڈھونڈو_win(qf_info_T *qi);
ساکن buf_T	*qf_ڈھونڈو_buf(qf_info_T *qi);
ساکن خالی	qf_update_بفر(qf_info_T *qi, qfline_T *old_آخری);
ساکن خالی	qf_fill_بفر(qf_list_T *qfl, buf_T *buf, qfline_T *old_آخری, صحیح qf_winid);
ساکن buf_T	*بھرو_dummy_بفر(char_u *fname, char_u *dirname_آغاز, char_u *resulting_dir);
ساکن خالی	wipe_dummy_بفر(buf_T *buf, char_u *dirname_آغاز);
ساکن خالی	unload_dummy_بفر(buf_T *buf, char_u *dirname_آغاز);
ساکن qf_info_T *ll_get_or_alloc_list(win_T *);
ساکن صحیح	اندراج_is_closer_مقصد_ہدف(qfline_T *اندراج_2, qfline_T *other_اندراج, صحیح ہدف_fnum, صحیح ہدف_lnum, صحیح ہدف_ستون);

// Quickfix window check helper macro
# define IS_QF_کھڑکی(wp) (bt_quickfix((wp)->w_بفر) && (wp)->w_llist_ref == تہی)
// Location list window check helper macro
# define IS_LL_کھڑکی(wp) (bt_quickfix((wp)->w_بفر) && (wp)->w_llist_ref != تہی)

// Quickfix and location list stack check helper macros
# define IS_QF_ڈھیر(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_ڈھیر(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_کھڑکی(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_اشیاء(qfl, qfp, i) \
		    برائے ((i) = 1, (qfp) = (qfl)->qf_آغاز; \
			    !got_int && (i) <= (qfl)->qf_گنتی && (qfp) != تہی; \
			    ++(i), (qfp) = (qfp)->qf_اگلا)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
ساکن char_u   *qf_آخری_bufname = تہی;
ساکن bufref_T  qf_آخری_bufref = {تہی, 0, 0};

ساکن garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    ساکن garray_T *
qfga_get(خالی)
{
    ساکن صحیح initialized = FALSE;

    اگر (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    واپس &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    ساکن خالی
qfga_clear(خالی)
{
    اگر (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    ورنہ
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define سطر_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
ساکن ساخت fmtpattern
{
    char_u	convchar;
    حرف	*نمونہ;
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
# define FMT_نمونہ_M 8
	{'m', ".\\+"},		// 8
# define FMT_نمونہ_R 9
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
    ساکن char_u *
efmpat_مقصد_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	صحیح	idx,
	صحیح	باری)
{
    char_u	*srcptr;

    اگر (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_اندر_format_string), *efmpat);
	واپس تہی;
    }
    اگر ((idx && idx < FMT_نمونہ_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->سابقہ) != تہی)
	    || (idx == FMT_نمونہ_R
		&& vim_strchr((char_u *)"OPQ", efminfo->سابقہ) == تہی))
    {
	semsg(_(e_unexpected_chr_اندر_format_str), *efmpat);
	واپس تہی;
    }
    efminfo->addr[idx] = (char_u)++باری;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_اندر_FILENAME
    اگر (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    اگر (*efmpat == 'f' && efmpat[1] != NUL)
    {
	اگر (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	ورنہ
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    ورنہ
    {
	srcptr = (char_u *)fmt_pat[idx].نمونہ;
	جب ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    واپس regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    ساکن char_u *
scanf_fmt_مقصد_regpat(
	char_u	**pefmp,
	char_u	*efm,
	صحیح	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    اگر (*efmp == '[' || *efmp == '\\')
    {
	اگر ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    اگر (efmp[1] == '^')
		*regpat++ = *++efmp;
	    اگر (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		جب (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		اگر (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_اندر_format_string));
		    واپس تہی;
		}
	    }
	}
	ورنہ اگر (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    ورنہ
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_اندر_format_string), *efmp);
	واپس تہی;
    }

    *pefmp = efmp;

    واپس regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    ساکن char_u *
efm_analyze_سابقہ(char_u *efmp, efm_T *efminfo)
{
    اگر (vim_strchr((char_u *)"+-", *efmp) != تہی)
	efminfo->flags = *efmp++;
    اگر (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != تہی)
	efminfo->سابقہ = *efmp;
    ورنہ
    {
	semsg(_(e_invalid_chr_اندر_format_string_سابقہ), *efmp);
	واپس تہی;
    }

    واپس efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    ساکن صحیح
efm_مقصد_regpat(
	char_u	*efm,
	صحیح	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    صحیح		باری;
    صحیح		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    باری = 0;
    برائے (efmp = efm; efmp < efm + len; ++efmp)
    {
	اگر (*efmp == '%')
	{
	    ++efmp;
	    برائے (idx = 0; idx < FMT_PATTERNS; ++idx)
		اگر (fmt_pat[idx].convchar == *efmp)
		    توڑو;
	    اگر (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_مقصد_regpat(efmp, ptr, fmt_ptr, idx, باری);
		اگر (ptr == تہی)
		    واپس FAIL;
		باری++;
	    }
	    ورنہ اگر (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_مقصد_regpat(&efmp, efm, len, ptr);
		اگر (ptr == تہی)
		    واپس FAIL;
	    }
	    ورنہ اگر (vim_strchr((char_u *)"%\\.^$~[", *efmp) != تہی)
		*ptr++ = *efmp;		// regexp magic characters
	    ورنہ اگر (*efmp == '#')
		*ptr++ = '*';
	    ورنہ اگر (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    ورنہ اگر (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_سابقہ(efmp, fmt_ptr);
		اگر (efmp == تہی)
		    واپس FAIL;
	    }
	    ورنہ
	    {
		semsg(_(e_invalid_chr_اندر_format_string), *efmp);
		واپس FAIL;
	    }
	}
	ورنہ			// copy normal character
	{
	    اگر (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    ورنہ اگر (vim_strchr((char_u *)".*^$~[", *efmp) != تہی)
		*ptr++ = '\\';	// escape regexp atoms
	    اگر (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    واپس OK;
}

/*
 * Free the 'errorformat' information list
 */
    ساکن خالی
free_efm_list(efm_T **efm_پہلا)
{
    efm_T *efm_ptr;

    برائے (efm_ptr = *efm_پہلا; efm_ptr != تہی; efm_ptr = *efm_پہلا)
    {
	*efm_پہلا = efm_ptr->اگلا;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_آغاز = تہی;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    ساکن صحیح
efm_regpat_bufsz(char_u *efm)
{
    صحیح sz;
    صحیح i;

    sz = (FMT_PATTERNS * 3) + ((صحیح)STRLEN(efm) << 2);
    برائے (i = FMT_PATTERNS; i > 0; )
	sz += (صحیح)STRLEN(fmt_pat[--i].نمونہ);
# ifdef BACKSLASH_اندر_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# ورنہ
    sz += 2; // "%f" can become two chars longer
# endif

    واپس sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    ساکن صحیح
efm_option_part_len(char_u *efm)
{
    صحیح len;

    برائے (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	اگر (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    واپس len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    ساکن efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = تہی;
    efm_T	*fmt_پہلا = تہی;
    efm_T	*fmt_آخری = تہی;
    char_u	*fmtstr = تہی;
    صحیح		len;
    صحیح		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    اگر ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == تہی)
	جاؤ parse_efm_error;

    جب (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	اگر (fmt_ptr == تہی)
	    جاؤ parse_efm_error;
	اگر (fmt_پہلا == تہی)	    // first one
	    fmt_پہلا = fmt_ptr;
	ورنہ
	    fmt_آخری->اگلا = fmt_ptr;
	fmt_آخری = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	اگر (efm_مقصد_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    جاؤ parse_efm_error;
	اگر ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == تہی)
	    جاؤ parse_efm_error;
	// Advance to next part
	efm = skip_مقصد_option_part(efm + len);	// skip comma and spaces
    }

    اگر (fmt_پہلا == تہی)	// nothing found
	emsg(_(e_errorformat_شامل_ہے_no_نمونہ));

    جاؤ parse_efm_انتہا;

parse_efm_error:
    free_efm_list(&fmt_پہلا);

parse_efm_انتہا:
    vim_free(fmtstr);

    واپس fmt_پہلا;
}

شمار {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_انتہا_کا_داخلہ = 2,
    QF_NOMEM = 3,
    QF_IGNORE_سطر = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
قسمتعریف ساخت {
    char_u	*linebuf;
    صحیح		linelen;
    char_u	*growbuf;
    صحیح		growbufsiz;
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
    ساکن char_u *
qf_grow_linebuf(qfstate_T *state, صحیح newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > سطر_MAXLEN ? سطر_MAXLEN - 1 : newsz;
    اگر (state->growbuf == تہی)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	اگر (state->growbuf == تہی)
	    واپس تہی;
	state->growbufsiz = state->linelen;
    }
    ورنہ اگر (state->linelen > state->growbufsiz)
    {
	اگر ((p = vim_realloc(state->growbuf, state->linelen + 1)) == تہی)
	    واپس تہی;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    واپس state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    ساکن صحیح
qf_get_اگلا_str_سطر(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    صحیح		len;

    اگر (*p_str == NUL) // Reached the end of the string
	واپس QF_انتہا_کا_داخلہ;

    p = vim_strchr(p_str, '\n');
    اگر (p != تہی)
	len = (صحیح)(p - p_str) + 1;
    ورنہ
	len = (صحیح)STRLEN(p_str);

    اگر (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	اگر (state->linebuf == تہی)
	    واپس QF_NOMEM;
    }
    ورنہ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    واپس QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    ساکن صحیح
qf_get_اگلا_list_سطر(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    صحیح		len;

    جب (p_li != تہی
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == تہی))
	p_li = p_li->li_اگلا;	// Skip non-string items

    اگر (p_li == تہی)		// End of the list
    {
	state->p_li = تہی;
	واپس QF_انتہا_کا_داخلہ;
    }

    len = (صحیح)STRLEN(p_li->li_tv.vval.v_string);
    اگر (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	اگر (state->linebuf == تہی)
	    واپس QF_NOMEM;
    }
    ورنہ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_اگلا;	// next item
    واپس QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    ساکن صحیح
qf_get_اگلا_buf_سطر(qfstate_T *state)
{
    char_u	*p_buf = تہی;
    صحیح		len;

    // Get the next line from the supplied buffer
    اگر (state->buflnum > state->lnumlast)
	واپس QF_انتہا_کا_داخلہ;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    اگر (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	اگر (state->linebuf == تہی)
	    واپس QF_NOMEM;
    }
    ورنہ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    واپس QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    ساکن صحیح
qf_get_اگلا_file_سطر(qfstate_T *state)
{
    صحیح	    discard;
    صحیح	    growbuflen;

    اگر (fgets((حرف *)IObuff, IOSIZE, state->fd) == تہی)
	واپس QF_انتہا_کا_داخلہ;

    discard = FALSE;
    state->linelen = (صحیح)STRLEN(IObuff);
    اگر (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	اگر (state->growbuf == تہی)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    اگر (state->growbuf == تہی)
		واپس QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	حافظہ_نقل(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	برائے (;;)
	{
	    char_u	*p;

	    اگر (fgets((حرف *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == تہی)
		توڑو;
	    state->linelen = (صحیح)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    اگر ((state->growbuf)[growbuflen - 1] == '\n')
		توڑو;
	    اگر (state->growbufsiz == سطر_MAXLEN)
	    {
		discard = TRUE;
		توڑو;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < سطر_MAXLEN
		? 2 * state->growbufsiz : سطر_MAXLEN;
	    اگر ((p = vim_realloc(state->growbuf, state->growbufsiz)) == تہی)
		واپس QF_NOMEM;
	    state->growbuf = p;
	}

	جب (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    اگر (fgets((حرف *)IObuff, IOSIZE, state->fd) == تہی
		    || (صحیح)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		توڑو;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    ورنہ
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    اگر (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*سطر;

	سطر = string_convert(&state->vc, state->linebuf, &state->linelen);
	اگر (سطر != تہی)
	{
	    اگر (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, سطر);
		vim_free(سطر);
	    }
	    ورنہ
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = سطر;
		state->growbufsiz = state->linelen < سطر_MAXLEN
						? state->linelen : سطر_MAXLEN;
	    }
	}
    }

    واپس QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    ساکن صحیح
qf_get_nextline(qfstate_T *state)
{
    صحیح حالت_2 = QF_FAIL;

    اگر (state->fd == تہی)
    {
	اگر (state->tv != تہی)
	{
	    اگر (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		حالت_2 = qf_get_اگلا_str_سطر(state);
	    ورنہ اگر (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		حالت_2 = qf_get_اگلا_list_سطر(state);
	}
	ورنہ
	    // Get the next line from the supplied buffer
	    حالت_2 = qf_get_اگلا_buf_سطر(state);
    }
    ورنہ
	// Get the next line from the supplied file
	حالت_2 = qf_get_اگلا_file_سطر(state);

    اگر (حالت_2 != QF_OK)
	واپس حالت_2;

    // remove newline/CR from the line
    اگر (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	اگر (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    واپس QF_OK;
}

قسمتعریف ساخت {
    char_u	*namebuf;
    صحیح		bnr;
    char_u	*module;
    char_u	*errmsg;
    صحیح		errmsglen;
    طویل	lnum;
    طویل	انتہا_lnum;
    صحیح		ستون;
    صحیح		انتہا_ستون;
    char_u	use_viscol;
    char_u	*نمونہ;
    صحیح		enr;
    صحیح		type;
    typval_T	*user_ڈیٹا;
    صحیح		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    ساکن صحیح
qf_parse_fmt_f(regmatch_T *rmp, صحیح midx, qffields_T *fields, صحیح سابقہ)
{
    صحیح c;

    اگر (rmp->startp[midx] == تہی || rmp->endp[midx] == تہی)
	واپس QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    اگر (vim_strchr((char_u *)"OPQ", سابقہ) != تہی
	    && mch_getperm(fields->namebuf) == -1)
	واپس QF_FAIL;

    واپس QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    ساکن صحیح
qf_parse_fmt_b(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == تہی)
	واپس QF_FAIL;
    صحیح bnr = (صحیح)atol((حرف *)rmp->startp[midx]);
    اگر (buflist_findnr(bnr) == تہی)
	واپس QF_FAIL;
    fields->bnr = bnr;
    واپس QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    ساکن صحیح
qf_parse_fmt_n(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == تہی)
	واپس QF_FAIL;
    fields->enr = (صحیح)atol((حرف *)rmp->startp[midx]);
    واپس QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    ساکن صحیح
qf_parse_fmt_l(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == تہی)
	واپس QF_FAIL;
    fields->lnum = atol((حرف *)rmp->startp[midx]);
    واپس QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    ساکن صحیح
qf_parse_fmt_e(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == تہی)
	واپس QF_FAIL;
    fields->انتہا_lnum = atol((حرف *)rmp->startp[midx]);
    واپس QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ساکن صحیح
qf_parse_fmt_c(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == تہی)
	واپس QF_FAIL;
    fields->ستون = (صحیح)atol((حرف *)rmp->startp[midx]);
    واپس QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    ساکن صحیح
qf_parse_fmt_k(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == تہی)
	واپس QF_FAIL;
    fields->انتہا_ستون = (صحیح)atol((حرف *)rmp->startp[midx]);
    واپس QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    ساکن صحیح
qf_parse_fmt_t(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == تہی)
	واپس QF_FAIL;
    fields->type = *rmp->startp[midx];
    واپس QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    ساکن صحیح
copy_nonerror_سطر(char_u *linebuf, صحیح linelen, qffields_T *fields)
{
    char_u	*p;

    اگر (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	اگر ((p = vim_realloc(fields->errmsg, linelen + 1)) == تہی)
	    واپس QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    واپس QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    ساکن صحیح
qf_parse_fmt_m(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    char_u	*p;
    صحیح		len;

    اگر (rmp->startp[midx] == تہی || rmp->endp[midx] == تہی)
	واپس QF_FAIL;
    len = (صحیح)(rmp->endp[midx] - rmp->startp[midx]);
    اگر (len >= fields->errmsglen)
    {
	// len + null terminator
	اگر ((p = vim_realloc(fields->errmsg, len + 1)) == تہی)
	    واپس QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    واپس QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    ساکن صحیح
qf_parse_fmt_r(regmatch_T *rmp, صحیح midx, char_u **دم)
{
    اگر (rmp->startp[midx] == تہی)
	واپس QF_FAIL;
    *دم = rmp->startp[midx];
    واپس QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ساکن صحیح
qf_parse_fmt_p(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    char_u	*مطابقت_ptr;

    اگر (rmp->startp[midx] == تہی || rmp->endp[midx] == تہی)
	واپس QF_FAIL;
    fields->ستون = 0;
    برائے (مطابقت_ptr = rmp->startp[midx]; مطابقت_ptr != rmp->endp[midx];
								++مطابقت_ptr)
    {
	++fields->ستون;
	اگر (*مطابقت_ptr == TAB)
	{
	    fields->ستون += 7;
	    fields->ستون -= fields->ستون % 8;
	}
    }
    ++fields->ستون;
    fields->use_viscol = TRUE;
    واپس QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ساکن صحیح
qf_parse_fmt_v(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == تہی)
	واپس QF_FAIL;
    fields->ستون = (صحیح)atol((حرف *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    واپس QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    ساکن صحیح
qf_parse_fmt_s(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    صحیح		len;

    اگر (rmp->startp[midx] == تہی || rmp->endp[midx] == تہی)
	واپس QF_FAIL;
    len = (صحیح)(rmp->endp[midx] - rmp->startp[midx]);
    اگر (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->نمونہ, "^\\V");
    STRNCAT(fields->نمونہ, rmp->startp[midx], len);
    fields->نمونہ[len + 3] = '\\';
    fields->نمونہ[len + 4] = '$';
    fields->نمونہ[len + 5] = NUL;
    واپس QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    ساکن صحیح
qf_parse_fmt_o(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    صحیح		len;

    اگر (rmp->startp[midx] == تہی || rmp->endp[midx] == تہی)
	واپس QF_FAIL;
    len = (صحیح)(rmp->endp[midx] - rmp->startp[midx]);
    اگر (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    واپس QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
ساکن صحیح (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, صحیح, qffields_T *) =
{
    تہی, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    تہی, // %r
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
    ساکن صحیح
qf_parse_مطابقت(
	char_u		*linebuf,
	صحیح		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	صحیح		qf_multiline,
	صحیح		qf_multiscan,
	char_u		**دم)
{
    صحیح		idx = fmt_ptr->سابقہ;
    صحیح		i;
    صحیح		midx;
    صحیح		حالت_2;

    اگر ((idx == 'C' || idx == 'Z') && !qf_multiline)
	واپس QF_FAIL;
    اگر (vim_strchr((char_u *)"EWIN", idx) != تہی)
	fields->type = idx;
    ورنہ
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    برائے (i = 0; i < FMT_PATTERNS; i++)
    {
	حالت_2 = QF_OK;
	midx = (صحیح)fmt_ptr->addr[i];
	اگر (i == 0 && midx > 0)				// %f
	    حالت_2 = qf_parse_fmt_f(regmatch, midx, fields, idx);
	ورنہ اگر (i == FMT_نمونہ_M)
	{
	    اگر (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		حالت_2 = copy_nonerror_سطر(linebuf, linelen, fields);
	    ورنہ اگر (midx > 0)				// %m
		حالت_2 = qf_parse_fmt_m(regmatch, midx, fields);
	}
	ورنہ اگر (i == FMT_نمونہ_R && midx > 0)	// %r
	    حالت_2 = qf_parse_fmt_r(regmatch, midx, دم);
	ورنہ اگر (midx > 0)				// others
	    حالت_2 = (qf_parse_fmt[i])(regmatch, midx, fields);

	اگر (حالت_2 != QF_OK)
	    واپس حالت_2;
    }

    واپس QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    ساکن صحیح
qf_parse_get_fields(
	char_u		*linebuf,
	صحیح		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	صحیح		qf_multiline,
	صحیح		qf_multiscan,
	char_u		**دم)
{
    regmatch_T	regmatch;
    صحیح		حالت_2 = QF_FAIL;
    صحیح		r;

    اگر (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->سابقہ) == تہی)
	واپس QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->نمونہ[0] = NUL;
    اگر (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->انتہا_lnum = 0;
    fields->ستون = 0;
    fields->انتہا_ستون = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *دم = تہی;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    اگر (r)
	حالت_2 = qf_parse_مطابقت(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, دم);

    واپس حالت_2;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    ساکن صحیح
qf_parse_dir_pfx(صحیح idx, qffields_T *fields, qf_list_T *qfl)
{
    اگر (idx == 'D')				// enter directory
    {
	اگر (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_خالی_خانے_directory_name));
	    واپس QF_FAIL;
	}
	qfl->qf_directory =
	    qf_ڈالو_dir(fields->namebuf, &qfl->qf_dir_ڈھیر, FALSE);
	اگر (qfl->qf_directory == تہی)
	    واپس QF_FAIL;
    }
    ورنہ اگر (idx == 'X')			// leave directory
	qfl->qf_directory = qf_نکالو_dir(&qfl->qf_dir_ڈھیر);

    واپس QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    ساکن صحیح
qf_parse_file_pfx(
	صحیح idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *دم)
{
    fields->valid = FALSE;
    اگر (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	اگر (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_ڈالو_dir(fields->namebuf, &qfl->qf_file_ڈھیر, TRUE);
	ورنہ اگر (idx == 'Q')
	    qfl->qf_currfile = qf_نکالو_dir(&qfl->qf_file_ڈھیر);
	*fields->namebuf = NUL;
	اگر (دم && *دم)
	{
	    qfl->qf_multiscan = TRUE;
	    واپس QF_MULTISCAN;
	}
    }

    واپس QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    ساکن صحیح
qf_parse_سطر_nomatch(char_u *linebuf, صحیح linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    واپس copy_nonerror_سطر(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    ساکن صحیح
qf_parse_multiline_pfx(
	صحیح idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    صحیح			len;

    اگر (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_آخری;

	اگر (qfprev == تہی)
	    واپس QF_FAIL;
	اگر (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (صحیح)STRLEN(qfprev->qf_متن);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    اگر (ptr == تہی)
		واپس QF_FAIL;
	    STRCPY(ptr, qfprev->qf_متن);
	    vim_free(qfprev->qf_متن);
	    qfprev->qf_متن = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	اگر (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	اگر (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	اگر (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	اگر (!qfprev->qf_انتہا_lnum)
	    qfprev->qf_انتہا_lnum = fields->انتہا_lnum;
	اگر (!qfprev->qf_ستون)
	{
	    qfprev->qf_ستون = fields->ستون;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	اگر (!qfprev->qf_انتہا_ستون)
	    qfprev->qf_انتہا_ستون = fields->انتہا_ستون;
	اگر (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != تہی
		    ? fields->namebuf
		    : qfl->qf_currfile != تہی && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    اگر (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    سطر_breakcheck();

    واپس QF_IGNORE_سطر;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    ساکن صحیح
qf_parse_سطر(
	qf_list_T	*qfl,
	char_u		*linebuf,
	صحیح		linelen,
	efm_T		*fmt_پہلا,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    صحیح			idx = 0;
    char_u		*دم = تہی;
    صحیح			حالت_2;

restofline:
    // If there was no %> item start at the first pattern
    اگر (fmt_آغاز == تہی)
	fmt_ptr = fmt_پہلا;
    ورنہ
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_آغاز;
	fmt_آغاز = تہی;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    برائے ( ; fmt_ptr != تہی; fmt_ptr = fmt_ptr->اگلا)
    {
	idx = fmt_ptr->سابقہ;
	حالت_2 = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &دم);
	اگر (حالت_2 == QF_NOMEM)
	    واپس حالت_2;
	اگر (حالت_2 == QF_OK)
	    توڑو;
    }
    qfl->qf_multiscan = FALSE;

    اگر (fmt_ptr == تہی || idx == 'D' || idx == 'X')
    {
	اگر (fmt_ptr != تہی)
	{
	    // 'D' and 'X' directory specifiers
	    حالت_2 = qf_parse_dir_pfx(idx, fields, qfl);
	    اگر (حالت_2 != QF_OK)
		واپس حالت_2;
	}

	حالت_2 = qf_parse_سطر_nomatch(linebuf, linelen, fields);
	اگر (حالت_2 != QF_OK)
	    واپس حالت_2;

	اگر (fmt_ptr == تہی)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    ورنہ اگر (fmt_ptr != تہی)
    {
	// honor %> item
	اگر (fmt_ptr->conthere)
	    fmt_آغاز = fmt_ptr;

	اگر (vim_strchr((char_u *)"AEWIN", idx) != تہی)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	ورنہ اگر (vim_strchr((char_u *)"CZ", idx) != تہی)
	{				// continuation of multi-line msg
	    حالت_2 = qf_parse_multiline_pfx(idx, qfl, fields);
	    اگر (حالت_2 != QF_OK)
		واپس حالت_2;
	}
	ورنہ اگر (vim_strchr((char_u *)"OPQ", idx) != تہی)
	{				// global file names
	    حالت_2 = qf_parse_file_pfx(idx, fields, qfl, دم);
	    اگر (حالت_2 == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(دم);
		صحیح new_linelen = (صحیح)STRLEN(s);
		اگر (new_linelen >= linelen)
		    واپس QF_IGNORE_سطر;
		linebuf = s;
		linelen = new_linelen;
		جاؤ restofline;
	    }
	}
	اگر (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    اگر (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    واپس QF_IGNORE_سطر;
	}
    }

    واپس QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    ساکن صحیح
qf_ڈھیر_خالی_خانے(qf_info_T *qi)
{
    واپس qi == تہی || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    ساکن صحیح
qf_list_خالی_خانے(qf_list_T *qfl)
{
    واپس qfl == تہی || qfl->qf_گنتی <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    ساکن صحیح
qf_list_has_valid_entries(qf_list_T *qfl)
{
    واپس !qf_list_خالی_خانے(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    ساکن qf_list_T *
qf_get_list(qf_info_T *qi, صحیح idx)
{
    واپس &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    ساکن صحیح
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->نمونہ = alloc_id(CMDBUFFSIZE + 1, aid_qf_نمونہ);
    اگر (pfields->namebuf == تہی || pfields->errmsg == تہی
		|| pfields->نمونہ == تہی || pfields->module == تہی)
	واپس FAIL;

    واپس OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    ساکن خالی
qf_free_fields(qffields_T *pfields)
{
    vim_free(pfields->namebuf);
    vim_free(pfields->module);
    vim_free(pfields->errmsg);
    vim_free(pfields->نمونہ);
}

/*
 * Setup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ساکن صحیح
qf_تیاری_state(
	qfstate_T	*pstate,
	char_u		*enc,
	char_u		*efile,
	typval_T	*tv,
	buf_T		*buf,
	linenr_T	lnumfirst,
	linenr_T	lnumlast)
{
    pstate->vc.vc_type = CONV_NONE;
    اگر (enc != تہی && *enc != NUL)
	convert_تیاری(&pstate->vc, enc, p_enc);

    اگر (efile != تہی && (pstate->fd = mch_fopen((حرف *)efile, "r")) == تہی)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	واپس FAIL;
    }

    اگر (tv != تہی)
    {
	اگر (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	ورنہ اگر (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_پہلا;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    واپس OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ساکن خالی
qf_cleanup_state(qfstate_T *pstate)
{
    اگر (pstate->fd != تہی)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    اگر (pstate->vc.vc_type != CONV_NONE)
	convert_تیاری(&pstate->vc, تہی, تہی);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    ساکن صحیح
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_پہلا,
	qfstate_T	*state,
	qffields_T	*fields)
{
    صحیح		    حالت_2;

    // Get the next line from a file/buffer/list/string
    حالت_2 = qf_get_nextline(state);
    اگر (حالت_2 != QF_OK)
	واپس حالت_2;

    حالت_2 = qf_parse_سطر(qfl, state->linebuf, state->linelen,
	    fmt_پہلا, fields);
    اگر (حالت_2 != QF_OK)
	واپس حالت_2;

    واپس qf_جوڑو_اندراج(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != تہی)
		? fields->namebuf
		: ((qfl->qf_currfile != تہی && fields->valid)
		    ? qfl->qf_currfile : (char_u *)تہی),
		fields->module,
		fields->bnr,
		fields->errmsg,
		fields->lnum,
		fields->انتہا_lnum,
		fields->ستون,
		fields->انتہا_ستون,
		fields->use_viscol,
		fields->نمونہ,
		fields->enr,
		fields->type,
		fields->user_ڈیٹا,
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
    ساکن صحیح
qf_init_ext(
    qf_info_T	    *qi,
    صحیح		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    صحیح		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_آخری = تہی;
    صحیح		    adding = FALSE;
    ساکن efm_T    *fmt_پہلا = تہی;
    char_u	    *efm;
    ساکن char_u   *آخری_efm = تہی;
    صحیح		    retval = -1;	// default: return error flag
    صحیح		    حالت_2;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_آخری_bufname);

    CLEAR_میدان(state);
    CLEAR_میدان(fields);
    اگر ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_تیاری_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	جاؤ qf_init_انتہا;

    اگر (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ورنہ
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	اگر (!qf_list_خالی_خانے(qfl))
	    old_آخری = qfl->qf_آخری;
    }

    // Use the local value of 'errorformat' if it's set.
    اگر (errorformat == p_efm && tv == تہی && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    ورنہ
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    اگر (آخری_efm == تہی || (STRCMP(آخری_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(آخری_efm);
	free_efm_list(&fmt_پہلا);

	// parse the current 'efm'
	fmt_پہلا = parse_efm_option(efm);
	اگر (fmt_پہلا != تہی)
	    آخری_efm = vim_strsave(efm);
    }

    اگر (fmt_پہلا == تہی)	// nothing found
	جاؤ error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    جب (!got_int)
    {
	حالت_2 = qf_init_process_nextline(qfl, fmt_پہلا, &state, &fields);
	اگر (حالت_2 == QF_NOMEM)		// memory alloc failure
	    جاؤ qf_init_انتہا;
	اگر (حالت_2 == QF_انتہا_کا_داخلہ)	// end of input
	    توڑو;
	اگر (حالت_2 == QF_FAIL)
	    جاؤ error2;

	سطر_breakcheck();
    }
    اگر (state.fd == تہی || !ferror(state.fd))
    {
	اگر (qfl->qf_اشاریہ == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_آغاز;
	    qfl->qf_اشاریہ = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	ورنہ
	{
	    qfl->qf_nonevalid = FALSE;
	    اگر (qfl->qf_ptr == تہی)
		qfl->qf_ptr = qfl->qf_آغاز;
	}
	// return number of matches
	retval = qfl->qf_گنتی;
	جاؤ qf_init_انتہا;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    اگر (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	اگر (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_انتہا:
    اگر (qf_idx == qi->qf_curlist)
	qf_update_بفر(qi, old_آخری);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    واپس retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    صحیح
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	صحیح	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    اگر (wp != تہی)
	qi = ll_get_or_alloc_list(wp);
    اگر (qi == تہی)
	واپس FAIL;

    واپس qf_init_ext(qi, qi->qf_curlist, efile, curbuf, تہی, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    ساکن خالی
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    اگر (title == تہی)
	واپس;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    اگر (p != تہی)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    ساکن char_u *
qf_cmdtitle(char_u *cmd)
{
    ساکن char_u qftitle_str[IOSIZE];

    vim_snprintf((حرف *)qftitle_str, IOSIZE, ":%s", (حرف *)cmd);
    واپس qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    ساکن qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    واپس qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    ساکن خالی
qf_نکالو_ڈھیر(qf_info_T *qi, صحیح adjust)
{
    صحیح i;
    qf_free(&qi->qf_lists[0]);
    برائے (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, حجم(*qi->qf_lists));

    اگر (adjust)
    {
	qi->qf_listcount--;
	اگر (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	ورنہ
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    ساکن خالی
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    جب (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    اگر (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_نکالو_ڈھیر(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    ورنہ
	qi->qf_curlist = qi->qf_listcount++;

    qfl = qf_get_curlist(qi);
    CLEAR_اشارہ_گر(qfl);
    qf_store_title(qfl, qf_title);
    qfl->qfl_type = qi->qfl_type;
    qfl->qf_id = ++آخری_qf_id;
    qfl->qf_has_user_ڈیٹا = FALSE;
}

/*
 * Queue location list stack delete request.
 */
    ساکن خالی
locstack_قطار_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    اگر (q == تہی)
	واپس;

    q->qi = qi;
    q->اگلا = qf_delq_سر;
    qf_delq_سر = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    صحیح
qf_ڈھیر_get_bufnr(خالی)
{
    اگر (ql_info == تہی)
	واپس INVALID_QFBUFNR;
    واپس ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    ساکن خالی
wipe_qf_بفر(qf_info_T *qi)
{
    buf_T	*qfbuf;

    اگر (qi->qf_bufnr == INVALID_QFBUFNR)
	واپس;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    اگر (qfbuf != تہی && qfbuf->b_nwindows == 0)
    {
	صحیح buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	اگر (curwin->w_بفر == تہی)
	{
	    curwin->w_بفر = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_بفر(تہی, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	اگر (buf_was_null)
	    curwin->w_بفر = تہی;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    ساکن خالی
qf_free_list_ڈھیر_اشیاء(qf_info_T *qi)
{
    برائے (صحیح i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    ساکن خالی
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_ڈھیر_اشیاء(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    ساکن خالی
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    اگر (qi == تہی)
	واپس;
    *pqi = تہی;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    اگر (quickfix_busy > 0)
    {
	locstack_قطار_delreq(qi);
	واپس;
    }

    qi->qf_refcount--;
    اگر (qi->qf_refcount < 1)
    {
	// No references to this location list.
	// If the quickfix window buffer is loaded, then wipe it
	wipe_qf_بفر(qi);

	qf_free_lists(qi);
    }
}

/*
 * Free all the quickfix/location lists in the stack.
 */
    خالی
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    اگر (wp != تہی)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    ورنہ اگر (qi != تہی)
	qf_free_list_ڈھیر_اشیاء(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    ساکن خالی
incr_quickfix_busy(خالی)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    ساکن خالی
decr_quickfix_busy(خالی)
{
    اگر (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	جب (qf_delq_سر != تہی)
	{
	    qf_delq_T	*q = qf_delq_سر;

	    qf_delq_سر = q->اگلا;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_اندرونی_ERROR
    اگر (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# اگر defined(EXITFREE)
    خالی
check_quickfix_busy(خالی)
{
    اگر (quickfix_busy != 0)
    {
	semsg("quickfix_busy not zero on exit: %ld", (طویل)quickfix_busy);
#  ifdef ABORT_ON_اندرونی_ERROR
	abort();
#  endif
    }
}
# endif

/*
 * Add an entry to the end of the list of errors.
 * Returns QF_OK on success or QF_FAIL on a memory allocation failure.
 */
    ساکن صحیح
qf_جوڑو_اندراج(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    صحیح		bufnum,		// buffer number or zero
    char_u	*mesg,		//پیغام
    طویل	lnum,		// line number
    طویل	انتہا_lnum,	// line number for end
    صحیح		ستون,		//عمود
    صحیح		انتہا_ستون,	// column for end
    صحیح		vis_ستون,	// using visual column
    char_u	*نمونہ,	// search pattern
    صحیح		nr,		// error number
    صحیح		type,		// type character
    typval_T	*user_ڈیٹا,     // custom user data or NULL
    صحیح		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = تہی;
    char_u	*p = تہی;

    اگر ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == تہی)
	واپس QF_FAIL;
    اگر (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	اگر (buf != تہی)
	    buf->b_has_qf_اندراج |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_اندراج : BUF_HAS_LL_اندراج;
    }
    ورنہ
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    اگر (fname != تہی)
	fullname = fix_fname(fname);
    qfp->qf_fname = تہی;
    اگر (buf != تہی &&
	buf->b_ffname != تہی && fullname != تہی)
    {
	اگر (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    اگر (p != تہی)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    اگر ((qfp->qf_متن = vim_strsave(mesg)) == تہی)
    {
	vim_free(qfp);
	واپس QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_انتہا_lnum = انتہا_lnum;
    qfp->qf_ستون = ستون;
    qfp->qf_انتہا_ستون = انتہا_ستون;
    qfp->qf_viscol = vis_ستون;
    اگر (user_ڈیٹا == تہی || user_ڈیٹا->v_type == VAR_UNKNOWN)
	qfp->qf_user_ڈیٹا.v_type = VAR_UNKNOWN;
    ورنہ
    {
	copy_tv(user_ڈیٹا, &qfp->qf_user_ڈیٹا);
	qfl->qf_has_user_ڈیٹا = TRUE;
    }
    اگر (نمونہ == تہی || *نمونہ == NUL)
	qfp->qf_نمونہ = تہی;
    ورنہ اگر ((qfp->qf_نمونہ = vim_strsave(نمونہ)) == تہی)
    {
	vim_free(qfp->qf_متن);
	vim_free(qfp);
	واپس QF_FAIL;
    }
    اگر (module == تہی || *module == NUL)
	qfp->qf_module = تہی;
    ورنہ اگر ((qfp->qf_module = vim_strsave(module)) == تہی)
    {
	vim_free(qfp->qf_متن);
	vim_free(qfp->qf_نمونہ);
	vim_free(qfp);
	واپس QF_FAIL;
    }
    qfp->qf_nr = nr;
    اگر (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_آخری;
    اگر (qf_list_خالی_خانے(qfl))		// first element in the list
    {
	qfl->qf_آغاز = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_اشاریہ = 0;
	qfp->qf_پچھلا = تہی;
    }
    ورنہ
    {
	qfp->qf_پچھلا = *lastp;
	(*lastp)->qf_اگلا = qfp;
    }
    qfp->qf_اگلا = تہی;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_گنتی;
    اگر (qfl->qf_اشاریہ == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_اشاریہ = qfl->qf_گنتی;
	qfl->qf_ptr = qfp;
    }

    واپس QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    صحیح
qf_resize_ڈھیر(صحیح n)
{
    اگر (ql_info == تہی)
	واپس FAIL;

    اگر (qf_resize_ڈھیر_بنیاد(ql_info, n) == FAIL)
	واپس FAIL;

    واپس OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    صحیح
ll_resize_ڈھیر(win_T *wp, صحیح n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    اگر (IS_LL_کھڑکی(wp))
	qf_sync_llw_مقصد_win(wp);
    ورنہ
	qf_sync_win_مقصد_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    اگر (qi == تہی)
	واپس FAIL;

    اگر (qf_resize_ڈھیر_بنیاد(qi, n) == FAIL)
	واپس FAIL;

    واپس OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    ساکن صحیح
qf_resize_ڈھیر_بنیاد(qf_info_T *qi, صحیح n)
{
    qf_list_T *new;
    صحیح amount_مقصد_rm = 0, i;

    اگر (qi == تہی)
	واپس FAIL;

    size_t lsz = حجم(*qi->qf_lists);

    اگر (n == qi->qf_maxcount)
	واپس OK;
    ورنہ اگر (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_مقصد_rm = qi->qf_listcount - n;

	برائے (i = 0; i < amount_مقصد_rm; i++)
	    qf_نکالو_ڈھیر(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    اگر (new == تہی)
	واپس FAIL;

    // fill with zeroes any newly allocated memory
    اگر (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_بفر(qi, تہی);

    واپس OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   خالی
qf_init_ڈھیر(خالی)
{
    ql_info = qf_alloc_ڈھیر(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    ساکن خالی
qf_sync_llw_مقصد_win(win_T *llw)
{
    win_T *wp = qf_ڈھونڈو_win_with_loclist(llw->w_llist_ref);

    اگر (wp != تہی)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    ساکن خالی
qf_sync_win_مقصد_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    اگر (llw != تہی)
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_llist_ref == llw && bt_quickfix(wp->w_بفر))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		واپس;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    ساکن qf_info_T *
qf_alloc_ڈھیر(qfltype_T qfltype, صحیح n)
{
    qf_info_T *qi;

    اگر (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    ورنہ
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	اگر (qi == تہی)
	    واپس تہی;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_ڈھیر(n);
    اگر (qi->qf_lists == تہی)
    {
	اگر (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	واپس تہی;
    }
    qi->qf_maxcount = n;

    واپس qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    ساکن qf_list_T *
qf_alloc_list_ڈھیر(صحیح n)
{
    واپس ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    ساکن qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    اگر (IS_LL_کھڑکی(wp))
	// For a location list window, use the referenced location list
	واپس wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    اگر (wp->w_llist == تہی)
	// new location list
	wp->w_llist = qf_alloc_ڈھیر(QFLT_LOCATION, wp->w_p_lhi);

    واپس wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    ساکن qf_info_T *
qf_cmd_get_ڈھیر(exarg_T *eap, صحیح print_emsg)
{
    qf_info_T	*qi = ql_info;

    اگر (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	اگر (qi == تہی)
	{
	    اگر (print_emsg)
		emsg(_(e_no_location_list));
	    واپس تہی;
	}
    }
    اگر (qi == تہی && print_emsg)
	emsg(_(e_no_quickfix_ڈھیر));

    واپس qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    ساکن qf_info_T *
qf_cmd_get_or_alloc_ڈھیر(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    اگر (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	اگر (qi == تہی)
	    واپس تہی;
	*pwinp = curwin;
    }

    واپس qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    ساکن صحیح
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *مقصد_qfl)
{
    صحیح		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_اشیاء(from_qfl, from_qfp, i)
    {
	اگر (qf_جوڑو_اندراج(مقصد_qfl,
		    تہی,
		    تہی,
		    from_qfp->qf_module,
		    0,
		    from_qfp->qf_متن,
		    from_qfp->qf_lnum,
		    from_qfp->qf_انتہا_lnum,
		    from_qfp->qf_ستون,
		    from_qfp->qf_انتہا_ستون,
		    from_qfp->qf_viscol,
		    from_qfp->qf_نمونہ,
		    from_qfp->qf_nr,
		    0,
		    &from_qfp->qf_user_ڈیٹا,
		    from_qfp->qf_valid) == QF_FAIL)
	    واپس FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = مقصد_qfl->qf_آخری;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	اگر (from_qfl->qf_ptr == from_qfp)
	    مقصد_qfl->qf_ptr = prevp;		// current location
    }

    واپس OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    ساکن صحیح
copy_loclist(qf_list_T *from_qfl, qf_list_T *مقصد_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    مقصد_qfl->qfl_type = from_qfl->qfl_type;
    مقصد_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    مقصد_qfl->qf_has_user_ڈیٹا = from_qfl->qf_has_user_ڈیٹا;
    مقصد_qfl->qf_گنتی = 0;
    مقصد_qfl->qf_اشاریہ = 0;
    مقصد_qfl->qf_آغاز = تہی;
    مقصد_qfl->qf_آخری = تہی;
    مقصد_qfl->qf_ptr = تہی;
    اگر (from_qfl->qf_title != تہی)
	مقصد_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    ورنہ
	مقصد_qfl->qf_title = تہی;
    اگر (from_qfl->qf_ctx != تہی)
    {
	مقصد_qfl->qf_ctx = alloc_tv();
	اگر (مقصد_qfl->qf_ctx != تہی)
	    copy_tv(from_qfl->qf_ctx, مقصد_qfl->qf_ctx);
    }
    ورنہ
	مقصد_qfl->qf_ctx = تہی;
    اگر (from_qfl->qf_qftf_cb.cb_name != تہی)
	copy_callback(&مقصد_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    ورنہ
	مقصد_qfl->qf_qftf_cb.cb_name = تہی;

    اگر (from_qfl->qf_گنتی)
	اگر (copy_loclist_entries(from_qfl, مقصد_qfl) == FAIL)
	    واپس FAIL;

    مقصد_qfl->qf_اشاریہ = from_qfl->qf_اشاریہ;	// current index in the list

    // Assign a new ID for the location list
    مقصد_qfl->qf_id = ++آخری_qf_id;
    مقصد_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    اگر (مقصد_qfl->qf_nonevalid)
    {
	مقصد_qfl->qf_ptr = مقصد_qfl->qf_آغاز;
	مقصد_qfl->qf_اشاریہ = 1;
    }

    واپس OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    خالی
copy_loclist_ڈھیر(win_T *from, win_T *مقصد)
{
    qf_info_T	*qi;
    صحیح		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    اگر (IS_LL_کھڑکی(from))
	qi = from->w_llist_ref;
    ورنہ
	qi = from->w_llist;

    اگر (qi == تہی)		    // no location list to copy
	واپس;

    // allocate a new location list, set size of stack to 'from' window value
    اگر ((مقصد->w_llist = qf_alloc_ڈھیر(QFLT_LOCATION, from->w_p_lhi)) == تہی)
	واپس;
    ورنہ
	// set 'to' lhi to reflect new value
	مقصد->w_p_lhi = مقصد->w_llist->qf_maxcount;

    مقصد->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    برائے (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	مقصد->w_llist->qf_curlist = idx;

	اگر (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(مقصد->w_llist, idx)) == FAIL)
	{
	    qf_free_all(مقصد);
	    واپس;
	}
    }

    مقصد->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    ساکن صحیح
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {تہی, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    اگر (fname == تہی || *fname == NUL)		// no file name
	واپس 0;

# ifdef VMS
    vms_remove_نسخہ(fname);
# endif
# ifdef BACKSLASH_اندر_FILENAME
    اگر (directory != تہی)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    اگر (directory != تہی && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != تہی)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	اگر (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    اگر (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    ورنہ
	    {
		ptr.لمبائی = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    اگر (ptr.string == تہی)
		واپس 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.لمبائی = ptr.لمبائی;
    }
    ورنہ
    {
	bufname.string = fname;
	bufname.لمبائی = fname_len;
    }

    اگر (qf_آخری_bufname != تہی && STRCMP(bufname.string, qf_آخری_bufname) == 0
	    && bufref_valid(&qf_آخری_bufref))
    {
	buf = qf_آخری_bufref.br_buf;
	vim_free(ptr.string);
    }
    ورنہ
    {
	vim_free(qf_آخری_bufname);
	buf = buflist_new(bufname.string, تہی, (linenr_T)0, BLN_NOOPT);
	اگر (bufname.string == ptr.string)
	    qf_آخری_bufname = bufname.string;
	ورنہ
	    qf_آخری_bufname = vim_strnsave(bufname.string, bufname.لمبائی);
	سیٹ_bufref(&qf_آخری_bufref, buf);
    }
    اگر (buf == تہی)
	واپس 0;

    buf->b_has_qf_اندراج =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_اندراج : BUF_HAS_LL_اندراج;
    واپس buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    ساکن char_u *
qf_ڈالو_dir(char_u *dirbuf, ساخت dir_ڈھیر_T **stackptr, صحیح is_file_ڈھیر)
{
    ساخت dir_ڈھیر_T  *ds_new;
    ساخت dir_ڈھیر_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(ساخت dir_ڈھیر_T, aid_qf_dirstack);
    اگر (ds_new == تہی)
	واپس تہی;

    // push the new element onto the stack
    ds_new->اگلا = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    اگر (vim_isAbsName(dirbuf)
	    || (*stackptr)->اگلا == تہی
	    || is_file_ڈھیر)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    ورنہ
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->اگلا;
	(*stackptr)->dirname = تہی;
	جب (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    اگر (dirname.string == تہی)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->اگلا;
		vim_free(ds_ptr);

		واپس تہی;
	    }
	    اگر (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		توڑو;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->اگلا;
	}

	// clean up all dirs we already left
	جب ((*stackptr)->اگلا != ds_new)
	{
	    ds_ptr = (*stackptr)->اگلا;
	    (*stackptr)->اگلا = (*stackptr)->اگلا->اگلا;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	اگر (ds_new == تہی)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    اگر ((*stackptr)->dirname != تہی)
	واپس (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->اگلا;
    vim_free(ds_ptr);

    واپس تہی;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    ساکن char_u *
qf_نکالو_dir(ساخت dir_ڈھیر_T **stackptr)
{
    ساخت dir_ڈھیر_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    اگر (*stackptr != تہی)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->اگلا;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    واپس *stackptr ? (*stackptr)->dirname : تہی;
}

/*
 * clean up directory stack
 */
    ساکن خالی
qf_clean_dir_ڈھیر(ساخت dir_ڈھیر_T **stackptr)
{
    ساخت dir_ڈھیر_T  *ds_ptr;

    جب ((ds_ptr = *stackptr) != تہی)
    {
	*stackptr = (*stackptr)->اگلا;
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
    ساکن char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    ساخت dir_ڈھیر_T     *ds_ptr;
    ساخت dir_ڈھیر_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    اگر (qfl->qf_dir_ڈھیر == تہی)
	واپس تہی;

    ds_ptr = qfl->qf_dir_ڈھیر->اگلا;
    fullname.string = تہی;
    fullname.لمبائی = 0;
    filename_len = STRLEN(filename);
    جب (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	اگر ((fullname.string != تہی) && (mch_getperm(fullname.string) >= 0))
	    توڑو;

	ds_ptr = ds_ptr->اگلا;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    جب (qfl->qf_dir_ڈھیر->اگلا != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_ڈھیر->اگلا;
	qfl->qf_dir_ڈھیر->اگلا = qfl->qf_dir_ڈھیر->اگلا->اگلا;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    واپس ds_ptr == تہی ? تہی : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    ساکن صحیح
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    صحیح		i;

    اگر (wp != تہی)
    {
	اگر (!win_valid(wp))
	    واپس FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    اگر (qi == تہی)
	واپس FALSE;

    برائے (i = 0; i < qi->qf_listcount; ++i)
	اگر (qi->qf_lists[i].qf_id == qf_id)
	    واپس TRUE;

    واپس FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    ساکن صحیح
is_qf_اندراج_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    صحیح		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_اشیاء(qfl, qfp, i)
	اگر (qfp == qf_ptr)
	    توڑو;

    اگر (i > qfl->qf_گنتی) // Entry is not found
	واپس FALSE;

    واپس TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    ساکن qfline_T *
get_اگلا_valid_اندراج(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	صحیح		*qf_اشاریہ,
	صحیح		dir)
{
    صحیح			idx;
    صحیح			old_qf_fnum;

    idx = *qf_اشاریہ;
    old_qf_fnum = qf_ptr->qf_fnum;

    کرو
    {
	اگر (idx == qfl->qf_گنتی || qf_ptr->qf_اگلا == تہی)
	    واپس تہی;
	++idx;
	qf_ptr = qf_ptr->qf_اگلا;
    } جب ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_اشاریہ = idx;
    واپس qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    ساکن qfline_T *
get_پچھلا_valid_اندراج(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	صحیح		*qf_اشاریہ,
	صحیح		dir)
{
    صحیح			idx;
    صحیح			old_qf_fnum;

    idx = *qf_اشاریہ;
    old_qf_fnum = qf_ptr->qf_fnum;

    کرو
    {
	اگر (idx == 1 || qf_ptr->qf_پچھلا == تہی)
	    واپس تہی;
	--idx;
	qf_ptr = qf_ptr->qf_پچھلا;
    } جب ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_اشاریہ = idx;
    واپس qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    ساکن qfline_T *
get_nth_valid_اندراج(
	qf_list_T	*qfl,
	صحیح		errornr,
	صحیح		dir,
	صحیح		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    صحیح			qf_idx = qfl->qf_اشاریہ;
    qfline_T		*پچھلا_qf_ptr;
    صحیح			پچھلا_اشاریہ;
    حرف		*err = e_no_مزید_اشیاء;

    جب (errornr--)
    {
	پچھلا_qf_ptr = qf_ptr;
	پچھلا_اشاریہ = qf_idx;

	اگر (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_اگلا_valid_اندراج(qfl, qf_ptr, &qf_idx, dir);
	ورنہ
	    qf_ptr = get_پچھلا_valid_اندراج(qfl, qf_ptr, &qf_idx, dir);
	اگر (qf_ptr == تہی)
	{
	    qf_ptr = پچھلا_qf_ptr;
	    qf_idx = پچھلا_اشاریہ;
	    اگر (err != تہی)
	    {
		emsg(_(err));
		واپس تہی;
	    }
	    توڑو;
	}

	err = تہی;
    }

    *new_qfidx = qf_idx;
    واپس qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    ساکن qfline_T *
get_nth_اندراج(qf_list_T *qfl, صحیح errornr, صحیح *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    صحیح		qf_idx = qfl->qf_اشاریہ;

    // New error number is less than the current error number
    جب (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_پچھلا != تہی)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_پچھلا;
    }
    // New error number is greater than the current error number
    جب (errornr > qf_idx && qf_idx < qfl->qf_گنتی &&
						qf_ptr->qf_اگلا != تہی)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_اگلا;
    }

    *new_qfidx = qf_idx;
    واپس qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    ساکن qfline_T *
qf_get_اندراج(
	qf_list_T	*qfl,
	صحیح		errornr,
	صحیح		dir,
	صحیح		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    صحیح		qfidx = qfl->qf_اشاریہ;

    اگر (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_اندراج(qfl, errornr, dir, &qfidx);
    ورنہ اگر (errornr != 0)	// go to specified number
	qf_ptr = get_nth_اندراج(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    واپس qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    ساکن win_T *
qf_ڈھونڈو_help_win(خالی)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	اگر (bt_help(wp->w_بفر))
	    واپس wp;

    واپس تہی;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    ساکن خالی
win_سیٹ_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    ساکن صحیح
jump_مقصد_help_کھڑکی(qf_info_T *qi, صحیح newwin, صحیح *opened_کھڑکی)
{
    win_T	*wp;
    صحیح		flags;

    اگر (cmdmod.cmod_tab != 0 || newwin)
	wp = تہی;
    ورنہ
	wp = qf_ڈھونڈو_help_win();
    اگر (wp != تہی && wp->w_بفر->b_nwindows > 0)
	win_enter(wp, TRUE);
    ورنہ
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	اگر (cmdmod.cmod_بانٹو == 0 && curwin->w_چوڑائی != Columns
		&& curwin->w_چوڑائی < 80)
	    flags |= WSP_چوٹی;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	اگر (IS_LL_ڈھیر(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	اگر (win_بانٹو(0, flags) == FAIL)
	    واپس FAIL;

	*opened_کھڑکی = TRUE;

	اگر (curwin->w_اونچائی < p_hh)
	    win_setheight((صحیح)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	اگر (IS_LL_ڈھیر(qi) && !newwin)
	    win_سیٹ_loclist(curwin, qi);
    }

    اگر (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    واپس OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    ساکن win_T *
qf_ڈھونڈو_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_llist == ll && !bt_quickfix(wp->w_بفر))
	    واپس wp;

    واپس تہی;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    ساکن win_T *
qf_ڈھونڈو_win_with_معمول_buf(خالی)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (bt_معمول(wp->w_بفر))
	    واپس wp;

    واپس تہی;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    ساکن صحیح
qf_goto_tabwin_with_file(صحیح fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_بفر->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    واپس TRUE;
	}

    واپس FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    ساکن صحیح
qf_open_new_file_win(qf_info_T *ll_ref)
{
    صحیح		flags;

    flags = WSP_ABOVE;
    اگر (ll_ref != تہی)
	flags |= WSP_NEWLOC;
    اگر (win_بانٹو(0, flags) == FAIL)
	واپس FAIL;		// not enough room for window
    p_swb = خالی_خانے_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    اگر (ll_ref != تہی)
	// The new window should use the location list from the
	// location list window
	win_سیٹ_loclist(curwin, ll_ref);
    واپس OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    ساکن خالی
qf_goto_win_with_ll_file(win_T *use_win, صحیح qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    اگر (win == تہی)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    اگر (win->w_بفر->b_fnum == qf_fnum)
		توڑو;
	اگر (win == تہی)
	{
	    // Find a previous usable window
	    win = curwin;
	    کرو
	    {
		اگر (bt_معمول(win->w_بفر))
		    توڑو;
		اگر (win->w_پچھلا == تہی)
		    win = lastwin;	// wrap around the top
		ورنہ
		    win = win->w_پچھلا; // go to previous window
	    } جب (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    اگر (win->w_llist == تہی && ll_ref != تہی)
	win_سیٹ_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    ساکن خالی
qf_goto_win_with_qfl_file(صحیح qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = تہی;
    برائے (;;)
    {
	اگر (win->w_بفر->b_fnum == qf_fnum)
	    توڑو;
	اگر (win->w_پچھلا == تہی)
	    win = lastwin;	// wrap around the top
	ورنہ
	    win = win->w_پچھلا;	// go to previous window

	اگر (IS_QF_کھڑکی(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    اگر ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    ورنہ اگر (altwin != تہی)
		win = altwin;
	    ورنہ اگر (curwin->w_پچھلا != تہی)
		win = curwin->w_پچھلا;
	    ورنہ
		win = curwin->w_اگلا;
	    توڑو;
	}

	// Remember a usable window.
	اگر (altwin == تہی && !win->w_p_pvw && !win->w_p_wfb &&
		bt_معمول(win->w_بفر))
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
    ساکن صحیح
qf_jump_مقصد_usable_کھڑکی(صحیح qf_fnum, صحیح newwin, صحیح *opened_کھڑکی)
{
    win_T	*usable_wp = تہی;
    صحیح		usable_win = FALSE;
    qf_info_T	*ll_ref = تہی;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    اگر (!newwin)
	ll_ref = curwin->w_llist_ref;

    اگر (ll_ref != تہی)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_ڈھونڈو_win_with_loclist(ll_ref);
	اگر (usable_wp != تہی)
	    usable_win = TRUE;
    }

    اگر (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_ڈھونڈو_win_with_معمول_buf();
	اگر (win != تہی)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    اگر (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    اگر ((ONE_کھڑکی && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	اگر (qf_open_new_file_win(ll_ref) != OK)
	    واپس FAIL;
	*opened_کھڑکی = TRUE;	// close it when fail
    }
    ورنہ
    {
	اگر (curwin->w_llist_ref != تہی)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	ورنہ					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    واپس OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    ساکن صحیح
qf_jump_edit_بفر(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	صحیح		forceit,
	صحیح		پچھلا_winid,
	صحیح		*opened_کھڑکی)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    صحیح		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    صحیح		retval = OK;
    صحیح		old_qf_curlist = qi->qf_curlist;
    صحیح		save_qfid = qfl->qf_id;

    اگر (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	اگر (!can_abandon(curbuf, forceit))
	{
	    no_write_پیغام();
	    واپس FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, تہی, تہی, تہی, (linenr_T)1,
		ECMD_HIDE + ECMD_سیٹ_HELP,
		پچھلا_winid == curwin->w_id ? curwin : تہی);
    }
    ورنہ
    {
	صحیح	fnum = qf_ptr->qf_fnum;

	اگر (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    اگر (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_مقصد_بفر));
		واپس FAIL;
	    }

	    اگر (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_بفر))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    اگر (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		اگر (win_بانٹو(0, 0) == OK)
		    *opened_کھڑکی = TRUE;

		اگر (curwin->w_p_wfb)
		{
		    // Autocommands set 'winfixbuf' or sent us to another window
		    // with it set, or we failed to split the window.  Give up,
		    // but don't return immediately, as they may have messed
		    // with the list.
		    emsg(_(e_winfixbuf_cannot_go_مقصد_بفر));
		    retval = FAIL;
		}
	    }
	}

	اگر (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    اگر (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(پچھلا_winid);

	اگر (wp == تہی && curwin->w_llist != qi)
	{
	    emsg(_(e_current_کھڑکی_was_closed));
	    *opened_کھڑکی = FALSE;
	    واپس QF_ABORT;
	}
    }

    اگر (qfl_type == QFLT_QUICKFIX && !qflist_valid(تہی, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_بدلا));
	واپس QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    اگر (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_اندراج_present(qfl, qf_ptr))
    {
	اگر (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_بدلا));
	ورنہ
	    emsg(_(e_current_location_list_was_بدلا));
	واپس QF_ABORT;
    }

    واپس retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    ساکن صحیح
qf_screen_ستون_مقصد_idx(colnr_T vcol)
{
    char_u	*سطر = ml_get_curline();
    char_u	*p = سطر;
    colnr_T	ستون = 0;

    جب (*p != NUL && ستون < vcol)
    {
	اگر (*p == TAB)
	    ستون += 8 - (ستون % 8);
	ورنہ
	    ستون += ptr2cells(p);
	اگر (ستون > vcol)
	    توڑو;
	MB_PTR_ADV(p);
    }
    واپس (صحیح)(p - سطر);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    ساکن خالی
qf_jump_goto_سطر(
	linenr_T	qf_lnum,
	صحیح		qf_ستون,
	char_u		qf_viscol,
	char_u		*qf_نمونہ)
{
    linenr_T		i;

    اگر (qf_نمونہ == تہی)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	اگر (i > 0)
	{
	    اگر (i > curbuf->b_ml.ml_سطر_گنتی)
		i = curbuf->b_ml.ml_سطر_گنتی;
	    curwin->w_cursor.lnum = i;
	}
	اگر (qf_ستون > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    اگر (qf_viscol == TRUE)
		curwin->w_cursor.ستون = qf_screen_ستون_مقصد_idx(qf_ستون - 1);
	    ورنہ
		curwin->w_cursor.ستون = qf_ستون - 1;
	    curwin->w_سیٹ_curswant = true;
	    check_cursor();
	}
	ورنہ
	    beginline(BL_WHITE | BL_FIX);
    }
    ورنہ
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	اگر (!do_تلاش(تہی, '/', '/', qf_نمونہ, STRLEN(qf_نمونہ), (طویل)1, تلاش_KEEP, تہی))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    ساکن خالی
qf_jump_print_پیغام(
	qf_info_T	*qi,
	صحیح		qf_اشاریہ,
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
    اگر (!پیغام_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_اشاریہ,
	    qf_get_curlist(qi)->qf_گنتی,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (حرف *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_متن(gap, skipwhite(qf_ptr->qf_متن));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = پیغام_scroll;
    اگر (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	پیغام_scroll = TRUE;
    ورنہ اگر (!پیغام_scrolled && shortmess(SHM_OVERALL))
	پیغام_scroll = FALSE;
    پیغام_attr_keep((حرف *)gap->ga_ڈیٹا, 0, TRUE);
    پیغام_scroll = i;

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
    ساکن صحیح
qf_jump_open_کھڑکی(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	صحیح		newwin,
	صحیح		*opened_کھڑکی)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    صحیح		old_changedtick = qfl->qf_changedtick;
    صحیح		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    اگر (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_بفر)
						      || cmdmod.cmod_tab != 0))
	اگر (jump_مقصد_help_کھڑکی(qi, newwin, opened_کھڑکی) == FAIL)
	    واپس FAIL;
    اگر (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_اندراج_present(qfl, qf_ptr))
    {
	اگر (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_بدلا));
	ورنہ
	    emsg(_(e_current_location_list_was_بدلا));
	واپس QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    اگر (bt_quickfix(curbuf) && !*opened_کھڑکی)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	اگر (qf_ptr->qf_fnum == 0)
	    واپس NOTDONE;

	اگر (qf_jump_مقصد_usable_کھڑکی(qf_ptr->qf_fnum, newwin,
						opened_کھڑکی) == FAIL)
	    واپس FAIL;
    }
    اگر (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_اندراج_present(qfl, qf_ptr))
    {
	اگر (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_بدلا));
	ورنہ
	    emsg(_(e_current_location_list_was_بدلا));
	واپس QF_ABORT;
    }

    واپس OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    ساکن صحیح
qf_jump_مقصد_بفر(
	qf_info_T	*qi,
	صحیح		qf_اشاریہ,
	qfline_T	*qf_ptr,
	صحیح		forceit,
	صحیح		پچھلا_winid,
	صحیح		*opened_کھڑکی,
	صحیح		openfold,
	صحیح		print_پیغام)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    صحیح		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    اگر (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_بفر(qi, qf_ptr, forceit, پچھلا_winid,
						opened_کھڑکی);
	اگر (retval != OK)
	    واپس retval;
    }

    // When not switched to another buffer, still need to set pc mark
    اگر (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_سطر(qf_ptr->qf_lnum, qf_ptr->qf_ستون, qf_ptr->qf_viscol,
	    qf_ptr->qf_نمونہ);

# ifdef FEAT_FOLDING
    اگر ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    اگر (print_پیغام)
	qf_jump_print_پیغام(qi, qf_اشاریہ, qf_ptr, old_curbuf, old_lnum);

    واپس retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    خالی
qf_jump(qf_info_T	*qi,
	صحیح		dir,
	صحیح		errornr,
	صحیح		forceit)
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
    ساکن خالی
qf_jump_newwin(qf_info_T	*qi,
	صحیح		dir,
	صحیح		errornr,
	صحیح		forceit,
	صحیح		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    صحیح			qf_اشاریہ;
    صحیح			old_qf_اشاریہ;
    char_u		*old_swb = p_swb;
    غیرعلامتی		old_swb_flags = swb_flags;
    صحیح			پچھلا_winid;
    صحیح			opened_کھڑکی = FALSE;
    صحیح			print_پیغام = TRUE;
    صحیح			old_کلید_Typed = کلید_Typed; // getting file may reset it
    صحیح			retval = OK;

    اگر (qi == تہی)
    {
	اگر (ql_info == تہی)
	{
	    emsg(_(e_no_quickfix_ڈھیر));
	    واپس;
	}
	qi = ql_info;
    }

    اگر (qf_ڈھیر_خالی_خانے(qi) || qf_list_خالی_خانے(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	واپس;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_اشاریہ = qfl->qf_اشاریہ;
    old_qf_اشاریہ = qf_اشاریہ;

    qf_ptr = qf_get_اندراج(qfl, errornr, dir, &qf_اشاریہ);
    اگر (qf_ptr == تہی)
    {
	qf_ptr = old_qf_ptr;
	qf_اشاریہ = old_qf_اشاریہ;
	جاؤ theend;
    }

    qfl->qf_اشاریہ = qf_اشاریہ;
    qfl->qf_ptr = qf_ptr;
    اگر (qf_win_pos_update(qi, old_qf_اشاریہ))
	// No need to print the error message if it's visible in the error
	//کھڑکی
	print_پیغام = FALSE;

    پچھلا_winid = curwin->w_id;

    retval = qf_jump_open_کھڑکی(qi, qf_ptr, newwin, &opened_کھڑکی);
    اگر (retval == FAIL)
	جاؤ failed;
    اگر (retval == QF_ABORT)
    {
	qi = تہی;
	qf_ptr = تہی;
	جاؤ theend;
    }
    اگر (retval == NOTDONE)
	جاؤ theend;

    retval = qf_jump_مقصد_بفر(qi, qf_اشاریہ, qf_ptr, forceit, پچھلا_winid,
				  &opened_کھڑکی, old_کلید_Typed, print_پیغام);
    اگر (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = تہی;
	qf_ptr = تہی;
    }

    اگر (retval != OK)
    {
	اگر (opened_کھڑکی)
	    win_close(curwin, TRUE);    // Close opened window
	اگر (qf_ptr != تہی && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_اشاریہ = old_qf_اشاریہ;
	}
    }
theend:
    اگر (qi != تہی)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_اشاریہ = qf_اشاریہ;
    }
    اگر (p_swb != old_swb && p_swb == خالی_خانے_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
ساکن صحیح	qfFileAttr;
ساکن صحیح	qfSepAttr;
ساکن صحیح	qf_سطر_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    ساکن خالی
qf_list_اندراج(qfline_T *qfp, صحیح qf_idx, صحیح cursel)
{
    char_u	*fname;
    buf_T	*buf;
    صحیح		filter_اندراج;
    garray_T	*gap;

    fname = تہی;
    اگر (qfp->qf_module != تہی && *qfp->qf_module != NUL)
	vim_snprintf((حرف *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(حرف *)qfp->qf_module);
    ورنہ
    {
	اگر (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != تہی)
	{
	    اگر (qfp->qf_fname == تہی)
		fname = buf->b_fname;
	    ورنہ
		fname = qfp->qf_fname;
	    اگر (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	اگر (fname == تہی)
	    sprintf((حرف *)IObuff, "%2d", qf_idx);
	ورنہ
	    vim_snprintf((حرف *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (حرف *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_اندراج = TRUE;
    اگر (qfp->qf_module != تہی && *qfp->qf_module != NUL)
	filter_اندراج &= پیغام_filtered(qfp->qf_module);
    اگر (filter_اندراج && fname != تہی)
	filter_اندراج &= پیغام_filtered(fname);
    اگر (filter_اندراج && qfp->qf_نمونہ != تہی)
	filter_اندراج &= پیغام_filtered(qfp->qf_نمونہ);
    اگر (filter_اندراج)
	filter_اندراج &= پیغام_filtered(qfp->qf_متن);
    اگر (filter_اندراج)
	واپس;

    پیغام_putchar('\n');
    پیغام_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    اگر (qfp->qf_lnum != 0)
	پیغام_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    اگر (qfp->qf_lnum != 0)
	qf_range_متن(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    پیغام_puts_attr((حرف *)gap->ga_ڈیٹا, qf_سطر_Attr);
    پیغام_puts_attr(":", qfSepAttr);
    اگر (qfp->qf_نمونہ != تہی)
    {
	gap = qfga_get();
	qf_fmt_متن(gap, qfp->qf_نمونہ);
	ga_append(gap, NUL);
	پیغام_puts((حرف *)gap->ga_ڈیٹا);
	پیغام_puts_attr(":", qfSepAttr);
    }
    پیغام_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_متن(gap, (fname != تہی || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_متن) : qfp->qf_متن);
    ga_append(gap, NUL);
    پیغام_prt_سطر((char_u *)gap->ga_ڈیٹا, FALSE);
    برآمد_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    خالی
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحیح		i;
    صحیح		idx1 = 1;
    صحیح		idx2 = -1;
    char_u	*arg = eap->arg;
    صحیح		plus = FALSE;
    صحیح		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, TRUE)) == تہی)
	واپس;

    اگر (qf_ڈھیر_خالی_خانے(qi) || qf_list_خالی_خانے(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	واپس;
    }
    اگر (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    اگر (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	واپس;
    }
    qfl = qf_get_curlist(qi);
    اگر (plus)
    {
	i = qfl->qf_اشاریہ;
	idx2 = i + idx1;
	idx1 = i;
    }
    ورنہ
    {
	i = qfl->qf_گنتی;
	اگر (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	اگر (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    اگر (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    اگر (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_سطر_Attr = syn_name2attr((char_u *)"qfLineNr");
    اگر (qf_سطر_Attr == 0)
	qf_سطر_Attr = HL_ATTR(HLF_N);

    اگر (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_اشیاء(qfl, qfp, i)
    {
	اگر ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_اندراج(qfp, i, i == qfl->qf_اشاریہ);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    ساکن خالی
qf_fmt_متن(garray_T *gap, char_u *متن)
{
    char_u	*p = متن;
    جب (*p != NUL)
    {
	اگر (*p == '\n')
	{
	    ga_append(gap, ' ');
	    جب (*++p != NUL)
		اگر (!VIM_ISWHITE(*p) && *p != '\n')
		    توڑو;
	}
	ورنہ
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    ساکن خالی
qf_range_متن(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.لمبائی = vim_snprintf_safelen((حرف *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    اگر (qfp->qf_انتہا_lnum > 0 && qfp->qf_lnum != qfp->qf_انتہا_lnum)
    {
	buf.لمبائی += vim_snprintf_safelen((حرف *)buf.string + buf.لمبائی,
	    IOSIZE - buf.لمبائی, "-%ld", qfp->qf_انتہا_lnum);
    }
    اگر (qfp->qf_ستون > 0)
    {
	buf.لمبائی += vim_snprintf_safelen((حرف *)buf.string + buf.لمبائی,
	    IOSIZE - buf.لمبائی, " col %d", qfp->qf_ستون);
	اگر (qfp->qf_انتہا_ستون > 0 && qfp->qf_ستون != qfp->qf_انتہا_ستون)
	    buf.لمبائی += vim_snprintf_safelen((حرف *)buf.string + buf.لمبائی,
		IOSIZE - buf.لمبائی, "-%d", qfp->qf_انتہا_ستون);
    }

    ga_concat_len(gap, buf.string, buf.لمبائی);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    ساکن خالی
qf_پیغام(qf_info_T *qi, صحیح which, حرف *lead)
{
    حرف   *title = (حرف *)qi->qf_lists[which].qf_title;
    صحیح    گنتی = qi->qf_lists[which].qf_گنتی;
    char_u buf[IOSIZE];

    vim_snprintf((حرف *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    گنتی);

    اگر (title != تہی)
    {
	size_t	len = STRLEN(buf);

	اگر (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    پیغام((حرف *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    خالی
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    صحیح		گنتی;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, TRUE)) == تہی)
	واپس;

    اگر (eap->addr_گنتی != 0)
	گنتی = eap->line2;
    ورنہ
	گنتی = 1;
    جب (گنتی--)
    {
	اگر (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    اگر (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_کا_quickfix_ڈھیر));
		توڑو;
	    }
	    --qi->qf_curlist;
	}
	ورنہ
	{
	    اگر (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_چوٹی_کا_quickfix_ڈھیر));
		توڑو;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_پیغام(qi, qi->qf_curlist, "");
    qf_update_بفر(qi, تہی);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    خالی
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_ڈھیر(eap, FALSE);
    صحیح		i;

    اگر (eap->addr_گنتی > 0)
    {
	اگر (qi == تہی)
	{
	    emsg(_(e_no_location_list));
	    واپس;
	}

	// Jump to the specified quickfix list
	اگر (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_پیغام(qi, qi->qf_curlist, "");
	    qf_update_بفر(qi, تہی);
	}
	ورنہ
	    emsg(_(e_invalid_range));

	واپس;
    }

    اگر (qf_ڈھیر_خالی_خانے(qi))
	پیغام(_("No entries"));
    ورنہ
	برائے (i = 0; i < qi->qf_listcount; ++i)
	    qf_پیغام(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    ساکن خالی
qf_free_اشیاء(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    صحیح		stop = FALSE;

    جب (qfl->qf_گنتی && qfl->qf_آغاز != تہی)
    {
	qfp = qfl->qf_آغاز;
	qfpnext = qfp->qf_اگلا;
	اگر (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_متن);
	    vim_free(qfp->qf_نمونہ);
	    clear_tv(&qfp->qf_user_ڈیٹا);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    اگر (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_گنتی = 1;
	    ورنہ
		qfl->qf_آغاز = qfpnext;
	}
	--qfl->qf_گنتی;
    }

    qfl->qf_اشاریہ = 0;
    qfl->qf_آغاز = تہی;
    qfl->qf_آخری = تہی;
    qfl->qf_ptr = تہی;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_ڈھیر(&qfl->qf_dir_ڈھیر);
    qfl->qf_directory = تہی;
    qf_clean_dir_ڈھیر(&qfl->qf_file_ڈھیر);
    qfl->qf_currfile = تہی;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    ساکن خالی
qf_free(qf_list_T *qfl)
{
    qf_free_اشیاء(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = تہی;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   خالی
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	طویل	amount,
	طویل	amount_after)
{
    صحیح		i;
    qfline_T	*qfp;
    صحیح		idx;
    qf_info_T	*qi = ql_info;
    صحیح		ملا_one = FALSE;
    صحیح		buf_has_flag = wp == تہی ? BUF_HAS_QF_اندراج : BUF_HAS_LL_اندراج;

    اگر (!(curbuf->b_has_qf_اندراج & buf_has_flag))
	واپس;
    اگر (wp != تہی)
    {
	اگر (wp->w_llist == تہی)
	    واپس;
	qi = wp->w_llist;
    }
    ورنہ اگر (qi == تہی)
	واپس;

    برائے (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	اگر (!qf_list_خالی_خانے(qfl))
	    FOR_ALL_QFL_اشیاء(qfl, qfp, i)
		اگر (qfp->qf_fnum == curbuf->b_fnum)
		{
		    ملا_one = TRUE;
		    اگر (qfp->qf_cleared)
			جاری;

		    اگر (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			اگر (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			ورنہ
			    qfp->qf_lnum += amount;
		    }
		    ورنہ اگر (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    اگر (!ملا_one)
	curbuf->b_has_qf_اندراج &= ~buf_has_flag;
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
    ساکن char_u *
qf_types(صحیح c, صحیح nr)
{
    ساکن char_u	buf[20];
    ساکن char_u	cc[3];
    char_u		*p;

    اگر (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    ورنہ اگر (c == 'I' || c == 'i')
	p = (char_u *)" info";
    ورنہ اگر (c == 'N' || c == 'n')
	p = (char_u *)" note";
    ورنہ اگر (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    ورنہ اگر (c == 0 || c == 1)
	p = (char_u *)"";
    ورنہ
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    اگر (nr <= 0)
	واپس p;

    sprintf((حرف *)buf, "%s %3d", (حرف *)p, nr);
    واپس buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    خالی
qf_view_نتیجہ(صحیح بانٹو)
{
    qf_info_T   *qi = ql_info;

    اگر (IS_LL_کھڑکی(curwin))
	qi = GET_LOC_LIST(curwin);
    ورنہ اگر (qi == تہی)
    {
	emsg(_(e_no_quickfix_ڈھیر));
	واپس;
    }

    اگر (qf_list_خالی_خانے(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	واپس;
    }

    اگر (بانٹو)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (طویل)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	واپس;
    }

    do_cmdline_cmd((char_u *)(IS_LL_کھڑکی(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    خالی
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, TRUE)) == تہی)
	واپس;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_ڈھونڈو_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    اگر (qf_ڈھیر_خالی_خانے(qi)
	    || qfl->qf_nonevalid
	    || qf_list_خالی_خانے(qfl))
    {
	اگر (win != تہی)
	    ex_cclose(eap);
    }
    ورنہ اگر (win == تہی)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    خالی
ex_cclose(exarg_T *eap)
{
    win_T	*win = تہی;
    qf_info_T	*qi;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, FALSE)) == تہی)
	واپس;

    // Find existing quickfix window and close it.
    win = qf_ڈھونڈو_win(qi);
    اگر (win != تہی)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    ساکن خالی
qf_سیٹ_title_var(qf_list_T *qfl)
{
    اگر (qfl->qf_title != تہی)
	سیٹ_اندرونی_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    ساکن صحیح
qf_goto_cwindow(qf_info_T *qi, صحیح resize, صحیح sz, صحیح vertsplit)
{
    win_T	*win;

    win = qf_ڈھونڈو_win(qi);
    اگر (win == تہی)
	واپس FAIL;

    win_goto(win);
    اگر (resize)
    {
	اگر (vertsplit)
	{
	    اگر (sz != win->w_چوڑائی)
		win_setwidth(sz);
	}
	ورنہ اگر (sz != win->w_اونچائی && win->w_اونچائی
		       + win->w_حالت_اونچائی + tabline_اونچائی() < cmdline_صف)
	    win_setheight(sz);
    }

    واپس OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    ساکن خالی
qf_سیٹ_cwindow_options(خالی)
{
    // switch off 'swapfile'
    سیٹ_option_قدر_give_err((char_u *)"swf", 0L, تہی, OPT_LOCAL);
    سیٹ_option_قدر_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    سیٹ_option_قدر_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    سیٹ_option_قدر_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    ساکن صحیح
qf_open_new_cwindow(qf_info_T *qi, صحیح اونچائی)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    صحیح		flags = 0;
    win_T	*win;

    qf_buf = qf_ڈھونڈو_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    اگر (IS_QF_ڈھیر(qi) && cmdmod.cmod_بانٹو == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    اگر (cmdmod.cmod_بانٹو == 0)
	flags = WSP_نیچے;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    اگر (IS_QF_ڈھیر(qi))
	flags |= WSP_QUICKFIX;

    اگر (win_بانٹو(اونچائی, flags) == FAIL)
	واپس FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    اگر (IS_LL_ڈھیر(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    اگر (oldwin != curwin)
	oldwin = تہی;  // don't store info when in another window
    اگر (qf_buf != تہی)
    {
	// Use the existing quickfix buffer
	اگر (do_ecmd(qf_buf->b_fnum, تہی, تہی, تہی, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    واپس FAIL;
    }
    ورنہ
    {
	// Create a new quickfix buffer
	اگر (do_ecmd(0, تہی, تہی, تہی, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    واپس FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    اگر (!bt_quickfix(curbuf))
	qf_سیٹ_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    اگر (curtab == prevtab && curwin->w_چوڑائی == Columns)
	win_setheight(اونچائی);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    اگر (win_valid(win))
	prevwin = win;

    واپس OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    خالی
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    صحیح		اونچائی;
    صحیح		حالت_2 = FAIL;
    صحیح		lnum;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, TRUE)) == تہی)
	واپس;

    incr_quickfix_busy();

    اگر (eap->addr_گنتی != 0)
	اونچائی = eap->line2;
    ورنہ
	اونچائی = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    اگر (cmdmod.cmod_tab == 0)
	حالت_2 = qf_goto_cwindow(qi, eap->addr_گنتی != 0, اونچائی,
						cmdmod.cmod_بانٹو & WSP_VERT);
    اگر (حالت_2 == FAIL)
	اگر (qf_open_new_cwindow(qi, اونچائی) == FAIL)
	{
	    decr_quickfix_busy();
	    واپس;
	}

    qfl = qf_get_curlist(qi);
    qf_سیٹ_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_اشاریہ;

    // Fill the buffer with the quickfix list.
    qf_fill_بفر(qfl, curbuf, تہی, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ستون = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    ساکن خالی
qf_win_goto(win_T *win, linenr_T lnum)
{
    win_T	*old_curwin = curwin;

    curwin = win;
    curbuf = win->w_بفر;
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ستون = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    update_topline();		// scroll to show the line
    redraw_later(UPD_VALID);
    curwin->w_redr_حالت = true;	// update ruler
    curwin = old_curwin;
    curbuf = curwin->w_بفر;
}

/*
 * :cbottom/:lbottom commands.
 */
    خالی
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, TRUE)) == تہی)
	واپس;

    win = qf_ڈھونڈو_win(qi);
    اگر (win != تہی && win->w_cursor.lnum != win->w_بفر->b_ml.ml_سطر_گنتی)
	qf_win_goto(win, win->w_بفر->b_ml.ml_سطر_گنتی);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_اندراج(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    اگر (IS_LL_کھڑکی(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    ورنہ اگر (qi == تہی)
	واپس 0;

    واپس qf_get_curlist(qi)->qf_اشاریہ;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    ساکن صحیح
qf_win_pos_update(
    qf_info_T	*qi,
    صحیح		old_qf_اشاریہ)	// previous qf_index or zero
{
    win_T	*win;
    صحیح		qf_اشاریہ = qf_get_curlist(qi)->qf_اشاریہ;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_ڈھونڈو_win(qi);
    اگر (win != تہی
	    && qf_اشاریہ <= win->w_بفر->b_ml.ml_سطر_گنتی
	    && old_qf_اشاریہ != qf_اشاریہ)
    {
	اگر (qf_اشاریہ > old_qf_اشاریہ)
	{
	    win->w_redraw_چوٹی = old_qf_اشاریہ;
	    win->w_redraw_bot = qf_اشاریہ;
	}
	ورنہ
	{
	    win->w_redraw_چوٹی = qf_اشاریہ;
	    win->w_redraw_bot = old_qf_اشاریہ;
	}
	qf_win_goto(win, qf_اشاریہ);
    }
    واپس win != تہی;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    ساکن صحیح
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    اگر (buf_valid(win->w_بفر) && bt_quickfix(win->w_بفر))
	اگر ((IS_QF_ڈھیر(qi) && win->w_llist_ref == تہی)
		|| (IS_LL_ڈھیر(qi) && win->w_llist_ref == qi))
	    واپس TRUE;

    واپس FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    ساکن win_T *
qf_ڈھونڈو_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	اگر (is_qf_win(win, qi))
	    واپس win;
    واپس تہی;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    ساکن buf_T *
qf_ڈھونڈو_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    اگر (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	اگر (qfbuf != تہی)
	    واپس qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	اگر (is_qf_win(win, qi))
	    واپس win->w_بفر;

    واپس تہی;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    حرف *
did_سیٹ_quickfixtextfunc(optset_T *دلائل غیر_مستعمل)
{
    اگر (option_سیٹ_callback_func(p_qftf, &qftf_cb) == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    ساکن خالی
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	اگر (is_qf_win(win, qi))
	{
	    curwin = win;
	    qf_سیٹ_title_var(qfl);
	}
    }
    curwin = save_curwin;
}

/*
 * Find the quickfix buffer.  If it exists, update the contents.
 */
    ساکن خالی
qf_update_بفر(qf_info_T *qi, qfline_T *old_آخری)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_ڈھونڈو_buf(qi);
    اگر (buf == تہی)
	واپس;

    linenr_T	old_سطر_گنتی = buf->b_ml.ml_سطر_گنتی;
    صحیح		qf_winid = 0;

    اگر (IS_LL_ڈھیر(qi))
    {
	اگر (curwin->w_llist == qi)
	    win = curwin;
	ورنہ
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_ڈھونڈو_win_with_loclist(qi);
	    اگر (win == تہی)
		// File window is not found. Find the location list window.
		win = qf_ڈھونڈو_win(qi);
	    اگر (win == تہی)
		واپس;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    صحیح do_fill = TRUE;
    اگر (old_آخری == تہی)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    اگر (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_بفر(qf_get_curlist(qi), buf, old_آخری, qf_winid);
	++CHANGEDTICK(buf);

	اگر (old_آخری == تہی)
	{
	    (خالی)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    اگر ((win = qf_ڈھونڈو_win(qi)) != تہی && old_سطر_گنتی < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    ساکن صحیح
qf_buf_جوڑو_سطر(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	صحیح		پہلا_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    اگر (qftf_str != تہی && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    ورنہ
    {
	اگر (qfp->qf_module != تہی)
	    ga_concat(gap, qfp->qf_module);
	ورنہ اگر (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != تہی
		&& errbuf->b_fname != تہی)
	{
	    اگر (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    ورنہ
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		اگر (پہلا_bufline && (errbuf->b_sfname == تہی
				|| mch_is_بھرا_Name(errbuf->b_sfname)))
		{
		    اگر (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		اگر (qfp->qf_fname == تہی)
		    ga_concat(gap, errbuf->b_fname);
		ورنہ
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	اگر (qfp->qf_lnum > 0)
	{
	    qf_range_متن(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	ورنہ اگر (qfp->qf_نمونہ != تہی)
	    qf_fmt_متن(gap, qfp->qf_نمونہ);
	ga_append(gap, '|');
	ga_append(gap, ' ');

	// Remove newlines and leading whitespace from the text.
	// For an unrecognized line keep the indent, the compiler may
	// mark a word with ^^^^.
	qf_fmt_متن(gap, gap->ga_len > 3 ? skipwhite(qfp->qf_متن)
							       : qfp->qf_متن);
    }

    ga_append(gap, NUL);
    اگر (ml_append_buf(buf, lnum, gap->ga_ڈیٹا, gap->ga_len, FALSE) == FAIL)
	واپس FAIL;

    واپس OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    ساکن list_T *
call_qftf_func(qf_list_T *qfl, صحیح qf_winid, طویل آغاز_idx, طویل انتہا_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = تہی;
    ساکن صحیح	recursive = FALSE;

    اگر (recursive)
	واپس تہی;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    اگر (qfl->qf_qftf_cb.cb_name != تہی)
	cb = &qfl->qf_qftf_cb;
    اگر (cb->cb_name != تہی)
    {
	typval_T	دلائل[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	اگر ((d = dict_alloc_تالا(VAR_FIXED)) == تہی)
	{
	    recursive = FALSE;
	    واپس تہی;
	}
	dict_جوڑو_نمبر(d, "quickfix", (طویل)IS_QF_LIST(qfl));
	dict_جوڑو_نمبر(d, "winid", (طویل)qf_winid);
	dict_جوڑو_نمبر(d, "id", (طویل)qfl->qf_id);
	dict_جوڑو_نمبر(d, "start_idx", آغاز_idx);
	dict_جوڑو_نمبر(d, "end_idx", انتہا_idx);
	++d->dv_refcount;
	دلائل[0].v_type = VAR_DICT;
	دلائل[0].vval.v_dict = d;

	qftf_list = تہی;
	textlock++;
	اگر (call_callback(cb, 0, &rettv, 1, دلائل) != FAIL)
	{
	    اگر (rettv.v_type == VAR_LIST)
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
    واپس qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    ساکن خالی
qf_fill_بفر(qf_list_T *qfl, buf_T *buf, qfline_T *old_آخری, صحیح qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    صحیح		old_کلید_Typed = کلید_Typed;
    list_T	*qftf_list = تہی;
    listitem_T	*qftf_li = تہی;

    اگر (old_آخری == تہی)
    {
	win_T		*wp;
	tabpage_T	*tp;

	اگر (buf != curbuf)
	{
	    اندرونی_error("qf_fill_buffer()");
	    واپس;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	جب ((curbuf->b_ml.ml_flags & ML_خالی_خانے) == 0)
	    (خالی)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp->w_بفر == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    اگر (qfl != تہی && qfl->qf_آغاز != تہی)
    {
	char_u		dirname[MAXPATHL];
	صحیح		invalid_val = FALSE;
	صحیح		پچھلا_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	اگر (old_آخری == تہی)
	{
	    qfp = qfl->qf_آغاز;
	    lnum = 0;
	}
	ورنہ
	{
	    اگر (old_آخری->qf_اگلا != تہی)
		qfp = old_آخری->qf_اگلا;
	    ورنہ
		qfp = old_آخری;
	    lnum = buf->b_ml.ml_سطر_گنتی;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (طویل)(lnum + 1),
							(طویل)qfl->qf_گنتی);
	اگر (qftf_list != تہی)
	    qftf_li = qftf_list->lv_پہلا;

	جب (lnum < qfl->qf_گنتی)
	{
	    char_u	*qftf_str = تہی;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    اگر (qftf_li != تہی && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		اگر (qftf_str == تہی)
		    invalid_val = TRUE;
	    }

	    اگر (qf_buf_جوڑو_سطر(buf, lnum, qfp, dirname,
			پچھلا_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		توڑو;

	    پچھلا_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_اگلا;
	    اگر (qfp == تہی)
		توڑو;

	    اگر (qftf_li != تہی)
		qftf_li = qftf_li->li_اگلا;
	}

	اگر (old_آخری == تہی)
	    // Delete the empty line which is now at the end
	    (خالی)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    اگر (old_آخری == تہی)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_تالا;
	سیٹ_option_قدر_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", تہی,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", تہی,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_تالا;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    کلید_Typed = old_کلید_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    ساکن خالی
qf_list_بدلا(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    ساکن صحیح
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    صحیح		qf_idx;

    برائے (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	اگر (qi->qf_lists[qf_idx].qf_id == qfid)
	    واپس qf_idx;
    واپس INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    ساکن صحیح
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    صحیح curlist;

    اگر (qf_get_curlist(qi)->qf_id == save_qfid)
	واپس OK;

    curlist = qf_id2nr(qi, save_qfid);
    اگر (curlist < 0)
	// list is not present
	واپس FAIL;
    qi->qf_curlist = curlist;
    واپس OK;
}

/*
 * Jump to the first entry if there is one.
 */
    ساکن خالی
qf_jump_پہلا(qf_info_T *qi, int_u save_qfid, صحیح forceit)
{
    اگر (qf_restore_list(qi, save_qfid) == FAIL)
	واپس;


    اگر (!check_can_سیٹ_curbuf_forceit(forceit))
	واپس;


    // Autocommands might have cleared the list, check for that.
    اگر (!qf_list_خالی_خانے(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    صحیح
grep_اندرونی(cmdidx_T cmdidx)
{
    واپس ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    ساکن char_u *
make_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_make:	    واپس (char_u *)"make";
	حالت CMD_lmake:	    واپس (char_u *)"lmake";
	حالت CMD_grep:	    واپس (char_u *)"grep";
	حالت CMD_lgrep:	    واپس (char_u *)"lgrep";
	حالت CMD_grepadd:   واپس (char_u *)"grepadd";
	حالت CMD_lgrepadd:  واپس (char_u *)"lgrepadd";
	طےشدہ: واپس تہی;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    ساکن char_u *
get_mef_name(خالی)
{
    char_u	*p;
    char_u	*name;
    ساکن صحیح	آغاز = -1;
    ساکن صحیح	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    اگر (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	اگر (name == تہی)
	    emsg(_(e_cant_get_عارضی_file_name));
	واپس name;
    }

    برائے (p = p_mef; *p; ++p)
	اگر (p[0] == '#' && p[1] == '#')
	    توڑو;

    اگر (*p == NUL)
	واپس vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    برائے (;;)
    {
	اگر (آغاز == -1)
	    آغاز = mch_get_pid();
	ورنہ
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	اگر (name == تہی)
	    توڑو;
	STRCPY(name, p_mef);
	sprintf((حرف *)name + (p - p_mef), "%d%d", آغاز, off);
	STRCAT(name, p + 2);
	اگر (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((حرف *)name, &sb) < 0
# endif
		)
	    توڑو;
	vim_free(name);
    }
    واپس name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    ساکن char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    غیرعلامتی	len;

    len = (غیرعلامتی)STRLEN(p_shq) * 2 + (غیرعلامتی)STRLEN(makecmd) + 1;
    اگر (*p_sp != NUL)
	len += (غیرعلامتی)STRLEN(p_sp) + (غیرعلامتی)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    اگر (cmd == تہی)
	واپس تہی;
    sprintf((حرف *)cmd, "%s%s%s", (حرف *)p_shq, (حرف *)makecmd,
							       (حرف *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    اگر (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    اگر (پیغام_ستون == 0)
	پیغام_didout = FALSE;
    پیغام_آغاز();
    پیغام_puts(":!");
    پیغام_outtrans(cmd);		// show what we are doing

    واپس cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    خالی
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = تہی;
    win_T	*wp = تہی;
    qf_info_T	*qi = ql_info;
    صحیح		res;
    char_u	*au_name = تہی;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    صحیح		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    اگر (grep_اندرونی(eap->cmdidx))
    {
	ex_vimgrep(eap);
	واپس;
    }

    au_name = make_get_auname(eap->cmdidx);
    اگر (au_name != تہی && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    واپس;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    اگر (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    اگر (fname == تہی)
	واپس;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    اگر (cmd == تہی)
    {
	vim_free(fname);
	واپس;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    برآمد_flush();
		// read window status report and redraw before message
    (خالی)char_avail();
# endif

    incr_quickfix_busy();

    اگر (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    اگر (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    اگر (wp != تہی)
    {
	qi = GET_LOC_LIST(wp);
	اگر (qi == تہی)
	    جاؤ cleanup;
    }
    ورنہ اگر (qi == تہی)
	جاؤ cleanup;

    اگر (res >= 0)
	qf_list_بدلا(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    اگر (au_name != تہی)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    اگر (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_پہلا(qi, save_qfid, FALSE);

cleanup:
    decr_quickfix_busy();
    mch_remove(fname);
    vim_free(fname);
    vim_free(cmd);
}

/*
 * Returns the number of entries in the current quickfix/location list.
 */
    صحیح
qf_get_سائز(exarg_T *eap)
{
    qf_info_T	*qi;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, FALSE)) == تہی)
	واپس 0;
    واپس qf_get_curlist(qi)->qf_گنتی;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    صحیح
qf_get_valid_سائز(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحیح		i, sz = 0;
    صحیح		پچھلا_fnum = 0;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, FALSE)) == تہی)
	واپس 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_اشیاء(qfl, qfp, i)
    {
	اگر (qfp->qf_valid)
	{
	    اگر (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    ورنہ اگر (qfp->qf_fnum > 0 && qfp->qf_fnum != پچھلا_fnum)
	    {
		// Count the number of files
		sz++;
		پچھلا_fnum = qfp->qf_fnum;
	    }
	}
    }

    واپس sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    صحیح
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, FALSE)) == تہی)
	واپس 0;

    واپس qf_get_curlist(qi)->qf_اشاریہ;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    صحیح
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحیح		i, eidx = 0;
    صحیح		پچھلا_fnum = 0;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, FALSE)) == تہی)
	واپس 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_آغاز;

    // check if the list has valid errors
    اگر (!qf_list_has_valid_entries(qfl))
	واپس 1;

    برائے (i = 1; i <= qfl->qf_اشاریہ && qfp!= تہی; i++, qfp = qfp->qf_اگلا)
    {
	اگر (qfp->qf_valid)
	{
	    اگر (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		اگر (qfp->qf_fnum > 0 && qfp->qf_fnum != پچھلا_fnum)
		{
		    // Count the number of files
		    eidx++;
		    پچھلا_fnum = qfp->qf_fnum;
		}
	    }
	    ورنہ
		eidx++;
	}
    }

    واپس eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    ساکن صحیح
qf_get_nth_valid_اندراج(qf_list_T *qfl, صحیح n, صحیح fdo)
{
    qfline_T	*qfp;
    صحیح		i, eidx;
    صحیح		پچھلا_fnum = 0;

    // check if the list has valid errors
    اگر (!qf_list_has_valid_entries(qfl))
	واپس 1;

    eidx = 0;
    FOR_ALL_QFL_اشیاء(qfl, qfp, i)
    {
	اگر (qfp->qf_valid)
	{
	    اگر (fdo)
	    {
		اگر (qfp->qf_fnum > 0 && qfp->qf_fnum != پچھلا_fnum)
		{
		    // Count the number of files
		    eidx++;
		    پچھلا_fnum = qfp->qf_fnum;
		}
	    }
	    ورنہ
		eidx++;
	}

	اگر (eidx == n)
	    توڑو;
    }

    اگر (i <= qfl->qf_گنتی)
	واپس i;
    ورنہ
	واپس 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    خالی
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    صحیح		errornr;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, TRUE)) == تہی)
	واپس;

    اگر (eap->addr_گنتی > 0)
	errornr = (صحیح)eap->line2;
    ورنہ
    {
	انتخاب (eap->cmdidx)
	{
	    حالت CMD_cc: حالت CMD_ll:
		errornr = 0;
		توڑو;
	    حالت CMD_crewind: حالت CMD_lrewind: حالت CMD_cfirst:
	    حالت CMD_lfirst:
		errornr = 1;
		توڑو;
	    طےشدہ:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    اگر (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_اندراج(qf_get_curlist(qi),
		eap->addr_گنتی > 0 ? (صحیح)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    خالی
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    صحیح		errornr;
    صحیح		dir;

    اگر ((qi = qf_cmd_get_ڈھیر(eap, TRUE)) == تہی)
	واپس;

    اگر (eap->addr_گنتی > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (صحیح)eap->line2;
    ورنہ
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    انتخاب (eap->cmdidx)
    {
	حالت CMD_cnext: حالت CMD_lnext: حالت CMD_cdo: حالت CMD_ldo:
	    dir = FORWARD;
	    توڑو;
	حالت CMD_cprevious: حالت CMD_lprevious: حالت CMD_c_اگلا:
	حالت CMD_l_اگلا:
	    dir = BACKWARD;
	    توڑو;
	حالت CMD_cnfile: حالت CMD_lnfile: حالت CMD_cfdo: حالت CMD_lfdo:
	    dir = FORWARD_FILE;
	    توڑو;
	حالت CMD_cpfile: حالت CMD_lpfile: حالت CMD_cNfile: حالت CMD_lNfile:
	    dir = BACKWARD_FILE;
	    توڑو;
	طےشدہ:
	    dir = FORWARD;
	    توڑو;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    ساکن qfline_T *
qf_ڈھونڈو_پہلا_اندراج_اندر_buf(qf_list_T *qfl, صحیح bnr, صحیح *errornr)
{
    qfline_T	*qfp = تہی;
    صحیح		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_اشیاء(qfl, qfp, idx)
	اگر (qfp->qf_fnum == bnr)
	    توڑو;

    *errornr = idx;
    واپس qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ساکن qfline_T *
qf_ڈھونڈو_پہلا_اندراج_on_سطر(qfline_T *اندراج_2, صحیح *errornr)
{
    جب (!got_int
	    && اندراج_2->qf_پچھلا != تہی
	    && اندراج_2->qf_fnum == اندراج_2->qf_پچھلا->qf_fnum
	    && اندراج_2->qf_lnum == اندراج_2->qf_پچھلا->qf_lnum)
    {
	اندراج_2 = اندراج_2->qf_پچھلا;
	--*errornr;
    }

    واپس اندراج_2;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ساکن qfline_T *
qf_ڈھونڈو_آخری_اندراج_on_سطر(qfline_T *اندراج_2, صحیح *errornr)
{
    جب (!got_int &&
	    اندراج_2->qf_اگلا != تہی
	    && اندراج_2->qf_fnum == اندراج_2->qf_اگلا->qf_fnum
	    && اندراج_2->qf_lnum == اندراج_2->qf_اگلا->qf_lnum)
    {
	اندراج_2 = اندراج_2->qf_اگلا;
	++*errornr;
    }

    واپس اندراج_2;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    ساکن صحیح
qf_اندراج_after_pos(qfline_T *qfp, pos_T *pos, صحیح linewise)
{
    اگر (linewise)
	واپس qfp->qf_lnum > pos->lnum;
    ورنہ
	واپس (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ستون > pos->ستون));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    ساکن صحیح
qf_اندراج_before_pos(qfline_T *qfp, pos_T *pos, صحیح linewise)
{
    اگر (linewise)
	واپس qfp->qf_lnum < pos->lnum;
    ورنہ
	واپس (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ستون < pos->ستون));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    ساکن صحیح
qf_اندراج_on_or_after_pos(qfline_T *qfp, pos_T *pos, صحیح linewise)
{
    اگر (linewise)
	واپس qfp->qf_lnum >= pos->lnum;
    ورنہ
	واپس (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ستون >= pos->ستون));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    ساکن صحیح
qf_اندراج_on_or_before_pos(qfline_T *qfp, pos_T *pos, صحیح linewise)
{
    اگر (linewise)
	واپس qfp->qf_lnum <= pos->lnum;
    ورنہ
	واپس (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ستون <= pos->ستون));
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
    ساکن qfline_T *
qf_ڈھونڈو_اندراج_after_pos(
	صحیح		bnr,
	pos_T		*pos,
	صحیح		linewise,
	qfline_T	*qfp,
	صحیح		*errornr)
{
    اگر (qf_اندراج_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	واپس qfp;

    // Find the entry just before or at the position 'pos'
    جب (qfp->qf_اگلا != تہی
	    && qfp->qf_اگلا->qf_fnum == bnr
	    && qf_اندراج_on_or_before_pos(qfp->qf_اگلا, pos, linewise))
    {
	qfp = qfp->qf_اگلا;
	++*errornr;
    }

    اگر (qfp->qf_اگلا == تہی || qfp->qf_اگلا->qf_fnum != bnr)
	// No entries found after position 'pos'
	واپس تہی;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_اگلا;
    ++*errornr;

    واپس qfp;
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
    ساکن qfline_T *
qf_ڈھونڈو_اندراج_before_pos(
	صحیح		bnr,
	pos_T		*pos,
	صحیح		linewise,
	qfline_T	*qfp,
	صحیح		*errornr)
{
    // Find the entry just before the position 'pos'
    جب (qfp->qf_اگلا != تہی
	    && qfp->qf_اگلا->qf_fnum == bnr
	    && qf_اندراج_before_pos(qfp->qf_اگلا, pos, linewise))
    {
	qfp = qfp->qf_اگلا;
	++*errornr;
    }

    اگر (qf_اندراج_on_or_after_pos(qfp, pos, linewise))
	واپس تہی;

    اگر (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_ڈھونڈو_پہلا_اندراج_on_سطر(qfp, errornr);

    واپس qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    ساکن qfline_T *
qf_ڈھونڈو_closest_اندراج(
	qf_list_T	*qfl,
	صحیح		bnr,
	pos_T		*pos,
	صحیح		dir,
	صحیح		linewise,
	صحیح		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_ڈھونڈو_پہلا_اندراج_اندر_buf(qfl, bnr, errornr);
    اگر (qfp == تہی)
	واپس تہی;		// no entry in this file

    اگر (dir == FORWARD)
	qfp = qf_ڈھونڈو_اندراج_after_pos(bnr, pos, linewise, qfp, errornr);
    ورنہ
	qfp = qf_ڈھونڈو_اندراج_before_pos(bnr, pos, linewise, qfp, errornr);

    واپس qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ساکن خالی
qf_get_nth_نیچے_اندراج(qfline_T *اندراج_arg, صحیح n, صحیح linewise, صحیح *errornr)
{
    qfline_T *اندراج_2 = اندراج_arg;

    جب (n-- > 0 && !got_int)
    {
	صحیح		پہلا_errornr = *errornr;

	اگر (linewise)
	    // Treat all the entries on the same line in this file as one
	    اندراج_2 = qf_ڈھونڈو_آخری_اندراج_on_سطر(اندراج_2, errornr);

	اگر (اندراج_2->qf_اگلا == تہی
		|| اندراج_2->qf_اگلا->qf_fnum != اندراج_2->qf_fnum)
	{
	    اگر (linewise)
		*errornr = پہلا_errornr;
	    توڑو;
	}

	اندراج_2 = اندراج_2->qf_اگلا;
	++*errornr;
    }
}

/*
 * Get the nth quickfix entry above the specified entry.  Searches backwards in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ساکن خالی
qf_get_nth_above_اندراج(qfline_T *اندراج_2, صحیح n, صحیح linewise, صحیح *errornr)
{
    جب (n-- > 0 && !got_int)
    {
	اگر (اندراج_2->qf_پچھلا == تہی
		|| اندراج_2->qf_پچھلا->qf_fnum != اندراج_2->qf_fnum)
	    توڑو;

	اندراج_2 = اندراج_2->qf_پچھلا;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	اگر (linewise)
	    اندراج_2 = qf_ڈھونڈو_پہلا_اندراج_on_سطر(اندراج_2, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    ساکن صحیح
qf_ڈھونڈو_nth_adj_اندراج(
	qf_list_T	*qfl,
	صحیح		bnr,
	pos_T		*pos,
	صحیح		n,
	صحیح		dir,
	صحیح		linewise)
{
    qfline_T	*adj_اندراج;
    صحیح		errornr;

    // Find an entry closest to the specified position
    adj_اندراج = qf_ڈھونڈو_closest_اندراج(qfl, bnr, pos, dir, linewise, &errornr);
    اگر (adj_اندراج == تہی)
	واپس 0;

    اگر (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	اگر (dir == FORWARD)
	    qf_get_nth_نیچے_اندراج(adj_اندراج, n, linewise, &errornr);
	ورنہ
	    qf_get_nth_above_اندراج(adj_اندراج, n, linewise, &errornr);
    }

    واپس errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    خالی
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    صحیح		dir;
    صحیح		buf_has_flag;
    صحیح		errornr = 0;
    pos_T	pos;

    اگر (eap->addr_گنتی > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	واپس;
    }

    // Check whether the current buffer has any quickfix entries
    اگر (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_اندراج;
    ورنہ
	buf_has_flag = BUF_HAS_LL_اندراج;
    اگر (!(curbuf->b_has_qf_اندراج & buf_has_flag))
    {
	emsg(_(e_no_errors));
	واپس;
    }

    اگر ((qi = qf_cmd_get_ڈھیر(eap, TRUE)) == تہی)
	واپس;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    اگر (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	واپس;
    }

    اگر (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    ورنہ
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.ستون++;
    errornr = qf_ڈھونڈو_nth_adj_اندراج(qfl, curbuf->b_fnum, &pos,
				eap->addr_گنتی > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    اگر (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    ورنہ
	emsg(_(e_no_مزید_اشیاء));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    ساکن char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_cfile:	    واپس (char_u *)"cfile";
	حالت CMD_cgetfile:  واپس (char_u *)"cgetfile";
	حالت CMD_caddfile:  واپس (char_u *)"caddfile";
	حالت CMD_lfile:	    واپس (char_u *)"lfile";
	حالت CMD_lgetfile:  واپس (char_u *)"lgetfile";
	حالت CMD_laddfile:  واپس (char_u *)"laddfile";
	طےشدہ:	    واپس تہی;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    خالی
ex_cfile(exarg_T *eap)
{
    char_u	*enc = تہی;
    win_T	*wp = تہی;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = تہی;
    int_u	save_qfid = 0;		// init for gcc
    صحیح		res;

    au_name = cfile_get_auname(eap->cmdidx);
    اگر (au_name != تہی && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							تہی, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    واپس;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   تہی, تہی,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), تہی);
	اگر (browse_file == تہی)
	    واپس;
	سیٹ_string_option_براہ_راست((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    ورنہ
# endif
    اگر (*eap->arg != NUL)
	سیٹ_string_option_براہ_راست((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    اگر (is_loclist_cmd(eap->cmdidx))
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
    اگر (wp != تہی)
    {
	qi = GET_LOC_LIST(wp);
	اگر (qi == تہی)
	{
	    decr_quickfix_busy();
	    واپس;
	}
    }
    ورنہ اگر (qi == تہی)
    {
	decr_quickfix_busy();
	واپس;
    }
    اگر (res >= 0)
	qf_list_بدلا(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    اگر (au_name != تہی)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, تہی, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    اگر (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_پہلا(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    ساکن char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_vimgrep:     واپس (char_u *)"vimgrep";
	حالت CMD_lvimgrep:    واپس (char_u *)"lvimgrep";
	حالت CMD_vimgrepadd:  واپس (char_u *)"vimgrepadd";
	حالت CMD_lvimgrepadd: واپس (char_u *)"lvimgrepadd";
	حالت CMD_grep:	      واپس (char_u *)"grep";
	حالت CMD_lgrep:	      واپس (char_u *)"lgrep";
	حالت CMD_grepadd:     واپس (char_u *)"grepadd";
	حالت CMD_lgrepadd:    واپس (char_u *)"lgrepadd";
	طےشدہ: واپس تہی;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    ساکن خالی
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = تہی;

    اگر (s == تہی || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	اگر (آخری_تلاش_pat() == تہی)
	{
	    emsg(_(e_no_پچھلا_regular_عبارت));
	    واپس;
	}
	regmatch->regprog = vim_regcomp(آخری_تلاش_pat(), RE_MAGIC);
    }
    ورنہ
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    ساکن خالی
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    پیغام_آغاز();
    p = پیغام_strtrunc(fname, TRUE);
    اگر (p == تہی)
	پیغام_outtrans(fname);
    ورنہ
    {
	پیغام_outtrans(p);
	vim_free(p);
    }
    پیغام_clr_eos();
    پیغام_didout = FALSE;	    // overwrite this message
    پیغام_nowait = TRUE;	    // don't wait for this message
    پیغام_ستون = 0;
    برآمد_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    ساکن buf_T *
vgr_بھرو_dummy_buf(
	char_u *fname,
	char_u *dirname_آغاز,
	char_u *dirname_now)
{
    صحیح		save_mls;
# اگر defined(FEAT_SYN_HL)
    char_u	*save_ei = تہی;
# endif
    buf_T	*buf;

# اگر defined(FEAT_SYN_HL)
    // Don't do Filetype autocommands to avoid loading syntax and
    // indent scripts, a great speed improvement.
    save_ei = au_event_disable(",Filetype");
# endif
    // Don't use modelines here, it's useless.
    save_mls = p_mls;
    p_mls = 0;

    // Load file into a buffer, so that 'fileencoding' is detected,
    // autocommands applied, etc.
    buf = بھرو_dummy_بفر(fname, dirname_آغاز, dirname_now);

    p_mls = save_mls;
# اگر defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    واپس buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    ساکن صحیح
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    اگر (!qflist_valid(wp, qfid))
    {
	اگر (wp != تہی)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_بدلا));
	    واپس FALSE;
	}
	ورنہ
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    واپس TRUE;
	}
    }

    اگر (qf_restore_list(qi, qfid) == FAIL)
	واپس FALSE;

    واپس TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    ساکن صحیح
vgr_مطابقت_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	طویل	    *tomatch,
	صحیح	    duplicate_name,
	صحیح	    flags)
{
    صحیح		ملا_مطابقت = FALSE;
    طویل	lnum;
    colnr_T	ستون;
    صحیح		pat_len = (صحیح)STRLEN(spat);
    اگر (pat_len > FUZZY_مطابقت_MAX_LEN)
	pat_len = FUZZY_مطابقت_MAX_LEN;

    برائے (lnum = 1; lnum <= buf->b_ml.ml_سطر_گنتی && *tomatch > 0; ++lnum)
    {
	ستون = 0;
	اگر (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    جب (vim_regexec_multi(regmatch, curwin, buf, lnum,
								ستون, تہی) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		اگر (qf_جوڑو_اندراج(qfl,
			    تہی,	// dir
			    fname,
			    تہی,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].ستون + 1,
			    regmatch->endpos[0].ستون + 1,
			    FALSE,	// vis_col
			    تہی,	// search pattern
			    0,		// nr
			    0,		// type
			    تہی,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    توڑو;
		}
		ملا_مطابقت = TRUE;
		اگر (--*tomatch == 0)
		    توڑو;
		اگر ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    توڑو;
		ستون = regmatch->endpos[0].ستون
		    + (ستون == regmatch->endpos[0].ستون);
		اگر (ستون > ml_get_buf_len(buf, lnum))
		    توڑو;
	    }
	}
	ورنہ
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    صحیح	    score;
	    int_u   matches[FUZZY_مطابقت_MAX_LEN];
	    int_u   sz = ARRAY_لمبائی(matches);

	    // Fuzzy string match
	    CLEAR_میدان(matches);
	    جب (fuzzy_مطابقت(str + ستون, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		اگر (qf_جوڑو_اندراج(qfl,
			    تہی,	// dir
			    fname,
			    تہی,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + ستون + 1,
			    0,
			    FALSE,	// vis_col
			    تہی,	// search pattern
			    0,		// nr
			    0,		// type
			    تہی,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    توڑو;
		}
		ملا_مطابقت = TRUE;
		اگر (--*tomatch == 0)
		    توڑو;
		اگر ((flags & VGR_GLOBAL) == 0)
		    توڑو;
		ستون = matches[pat_len - 1] + ستون + 1;
		اگر (ستون > linelen)
		    توڑو;
	    }
	}
	سطر_breakcheck();
	اگر (got_int)
	    توڑو;
    }

    واپس ملا_مطابقت;
}

/*
 * Jump to the first match and update the directory.
 */
    ساکن خالی
vgr_jump_مقصد_مطابقت(
	qf_info_T   *qi,
	صحیح	    forceit,
	صحیح	    *redraw_for_dummy,
	buf_T	    *پہلا_مطابقت_buf,
	char_u	    *ہدف_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    اگر (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    اگر (curbuf == پہلا_مطابقت_buf && ہدف_dir != تہی)
    {
	exarg_T ea;

	CLEAR_میدان(ea);
	ea.arg = ہدف_dir;
	ea.cmdidx = CMD_lcd;
	ex_cd(&ea);
    }
}

/*
 * :vimgrep command arguments
 */
قسمتعریف ساخت
{
    طویل	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    صحیح		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    صحیح		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_دلائل_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    ساکن صحیح
vgr_process_دلائل(
	exarg_T		*eap,
	vgr_دلائل_T	*دلائل)
{
    char_u	*p;

    CLEAR_اشارہ_گر(دلائل);

    دلائل->regmatch.regprog = تہی;
    دلائل->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    اگر (eap->addr_گنتی > 0)
	دلائل->tomatch = eap->line2;
    ورنہ
	دلائل->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &دلائل->spat, &دلائل->flags);
    اگر (p == تہی)
    {
	emsg(_(e_invalid_تلاش_نمونہ_or_delimiter));
	واپس FAIL;
    }

    vgr_init_regmatch(&دلائل->regmatch, دلائل->spat);
    اگر (دلائل->regmatch.regprog == تہی)
	واپس FAIL;

    p = skipwhite(p);
    اگر (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_نمونہ));
	واپس FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    اگر ((get_arglist_exp(p, &دلائل->fcount, &دلائل->fnames, TRUE) == FAIL) ||
	دلائل->fcount == 0)
    {
	emsg(_(e_no_مطابقت));
	واپس FAIL;
    }

    واپس OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    ساکن صحیح
existing_swapfile(buf_T *buf)
{
    اگر (buf->b_ml.ml_mfp != تہی && buf->b_ml.ml_mfp->mf_fname != تہی)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	واپس fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    واپس FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    ساکن صحیح
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_دلائل_T	*cmd_دلائل,
	صحیح		*redraw_for_dummy,
	buf_T		**پہلا_مطابقت_buf,
	char_u		**ہدف_dir)
{
    صحیح		حالت_2 = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    صحیح		fi;
    buf_T	*buf;
    صحیح		duplicate_name = FALSE;
    صحیح		using_dummy;
    char_u	*dirname_آغاز = تہی;
    char_u	*dirname_now = تہی;
    صحیح		ملا_مطابقت;
    aco_save_T	aco;

    dirname_آغاز = alloc_id(MAXPATHL, aid_qf_dirname_آغاز);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    اگر (dirname_آغاز == تہی || dirname_now == تہی)
	جاؤ theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_آغاز, MAXPATHL);

    seconds = (time_t)0;
    برائے (fi = 0; fi < cmd_دلائل->fcount && !got_int && cmd_دلائل->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_دلائل->fnames[fi]);
	اگر (وقت(تہی) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = وقت(تہی);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_دلائل->fnames[fi]);
	اگر (buf == تہی || buf->b_ml.ml_mfp == تہی)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != تہی);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_بھرو_dummy_buf(fname, dirname_آغاز, dirname_now);
	}
	ورنہ
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	اگر (!vgr_qflist_valid(wp, qi, save_qfid, cmd_دلائل->qf_title))
	    جاؤ theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	اگر (buf == تہی)
	{
	    اگر (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	ورنہ
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    ملا_مطابقت = vgr_مطابقت_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_دلائل->spat, &cmd_دلائل->regmatch,
		    &cmd_دلائل->tomatch, duplicate_name, cmd_دلائل->flags);

	    اگر (using_dummy)
	    {
		اگر (ملا_مطابقت && *پہلا_مطابقت_buf == تہی)
		    *پہلا_مطابقت_buf = buf;
		اگر (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_بفر(buf, dirname_آغاز);
		    buf = تہی;
		}
		ورنہ اگر ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    اگر (!ملا_مطابقت)
		    {
			wipe_dummy_بفر(buf, dirname_آغاز);
			buf = تہی;
		    }
		    ورنہ اگر (buf != *پہلا_مطابقت_buf
					|| (cmd_دلائل->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_بفر(buf, dirname_آغاز);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = تہی;
		    }
		}

		اگر (buf != تہی)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    اگر (buf == *پہلا_مطابقت_buf
			    && *ہدف_dir == تہی
			    && STRCMP(dirname_آغاز, dirname_now) != 0)
			*ہدف_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    اگر (curbuf == buf)
		    {
# اگر defined(FEAT_SYN_HL)
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

    حالت_2 = OK;

theend:
    vim_free(dirname_now);
    vim_free(dirname_آغاز);
    واپس حالت_2;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    خالی
ex_vimgrep(exarg_T *eap)
{
    vgr_دلائل_T	دلائل;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = تہی;
    صحیح		redraw_for_dummy = FALSE;
    buf_T	*پہلا_مطابقت_buf = تہی;
    char_u	*ہدف_dir = تہی;
    char_u	*au_name =  تہی;
    صحیح		حالت_2;

    اگر (!check_can_سیٹ_curbuf_forceit(eap->forceit))
	واپس;

    au_name = vgr_get_auname(eap->cmdidx);
    اگر (au_name != تہی && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    واپس;
# endif
    }

    qi = qf_cmd_get_or_alloc_ڈھیر(eap, &wp);
    اگر (qi == تہی)
	واپس;

    اگر (vgr_process_دلائل(eap, &دلائل) == FAIL)
	جاؤ theend;

    اگر ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_ڈھیر_خالی_خانے(qi))
	// make place for a new list
	qf_new_list(qi, دلائل.qf_title);

    incr_quickfix_busy();

    حالت_2 = vgr_process_files(wp, qi, &دلائل, &redraw_for_dummy,
						&پہلا_مطابقت_buf, &ہدف_dir);
    اگر (حالت_2 != OK)
    {
	FreeWild(دلائل.fcount, دلائل.fnames);
	decr_quickfix_busy();
	جاؤ theend;
    }

    FreeWild(دلائل.fcount, دلائل.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_آغاز;
    qfl->qf_اشاریہ = 1;
    qf_list_بدلا(qfl);

    qf_update_بفر(qi, تہی);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    اگر (au_name != تہی)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    اگر (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	جاؤ theend;
    }

    // Jump to first match.
    اگر (!qf_list_خالی_خانے(qf_get_curlist(qi)))
    {
	اگر ((دلائل.flags & VGR_NOJUMP) == 0)
	    vgr_jump_مقصد_مطابقت(qi, eap->forceit, &redraw_for_dummy,
		    پہلا_مطابقت_buf, ہدف_dir);
    }
    ورنہ
	semsg(_(e_no_مطابقت_str_2), دلائل.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    اگر (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# ورنہ
	redraw_later(UPD_NOT_VALID);
# endif
    }

theend:
    vim_free(دلائل.qf_title);
    vim_free(ہدف_dir);
    vim_regfree(دلائل.regmatch.regprog);
}

/*
 * Restore current working directory to "dirname_start" if they differ, taking
 * into account whether it is set locally or globally.
 */
    ساکن خالی
restore_آغاز_dir(char_u *dirname_آغاز)
{
    char_u *dirname_now = alloc(MAXPATHL);

    اگر (dirname_now == تہی)
	واپس;

    mch_dirname(dirname_now, MAXPATHL);
    اگر (STRCMP(dirname_آغاز, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_میدان(ea);
	ea.arg = dirname_آغاز;
	ea.cmdidx = (curwin->w_localdir == تہی) ? CMD_cd : CMD_lcd;
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
    ساکن buf_T *
بھرو_dummy_بفر(
    char_u	*fname,
    char_u	*dirname_آغاز,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_مقصد_wipe;
    صحیح		failed = TRUE;
    aco_save_T	aco;
    صحیح		readfile_نتیجہ;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(تہی, تہی, (linenr_T)1, BLN_DUMMY);
    اگر (newbuf == تہی)
	واپس تہی;
    سیٹ_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    اگر (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	اگر (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (خالی)setfname(curbuf, fname, تہی, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_تبادلہ(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_مقصد_wipe.br_buf = تہی;
	    readfile_نتیجہ = readfile(fname, تہی,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			تہی, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    اگر (readfile_نتیجہ == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		اگر (curbuf != newbuf)
		{
		    // Bloody autocommands changed the buffer!  Can happen when
		    // using netrw and editing a remote file.  Use the current
		    // buffer instead, delete the dummy one after restoring the
		    // window stuff.
		    سیٹ_bufref(&newbuf_مقصد_wipe, newbuf);
		    newbuf = curbuf;
		}
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);

	    اگر (newbuf_مقصد_wipe.br_buf != تہی && bufref_valid(&newbuf_مقصد_wipe))
	    {
		بلاک_autocmds();
		wipe_dummy_بفر(newbuf_مقصد_wipe.br_buf, تہی);
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
    restore_آغاز_dir(dirname_آغاز);

    اگر (!bufref_valid(&newbufref))
	واپس تہی;
    اگر (failed)
    {
	wipe_dummy_بفر(newbuf, dirname_آغاز);
	واپس تہی;
    }
    واپس newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    ساکن خالی
wipe_dummy_بفر(buf_T *buf, char_u *dirname_آغاز)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    جب (buf->b_nwindows > 0)
    {
	صحیح	    did_one = FALSE;
	win_T	    *wp;

	اگر (firstwin->w_اگلا != تہی)
	    FOR_ALL_WINDOWS(wp)
		اگر (wp->w_بفر == buf)
		{
		    اگر (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    توڑو;
		}
	اگر (!did_one)
	    جاؤ fail;
    }

    اگر (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# اگر defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_بفر(buf, TRUE);

# اگر defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	اگر (dirname_آغاز != تہی)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_آغاز_dir(dirname_آغاز);

	واپس;
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
    ساکن خالی
unload_dummy_بفر(buf_T *buf, char_u *dirname_آغاز)
{
    اگر (curbuf == buf)		// safety check
	واپس;

    close_بفر(تہی, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_آغاز_dir(dirname_آغاز);
}

# اگر defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    ساکن صحیح
get_qfline_اشیاء(qfline_T *qfp, list_T *list)
{
    صحیح		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    اگر (bufnum != 0 && (buflist_findnr(bufnum) == تہی))
	bufnum = 0;

    اگر ((dict = dict_alloc()) == تہی)
	واپس FAIL;
    اگر (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	واپس FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    اگر (dict_جوڑو_نمبر(dict, "bufnr", (طویل)bufnum) == FAIL
	    || dict_جوڑو_نمبر(dict, "lnum",     (طویل)qfp->qf_lnum) == FAIL
	    || dict_جوڑو_نمبر(dict, "end_lnum", (طویل)qfp->qf_انتہا_lnum) == FAIL
	    || dict_جوڑو_نمبر(dict, "col",      (طویل)qfp->qf_ستون) == FAIL
	    || dict_جوڑو_نمبر(dict, "end_col",  (طویل)qfp->qf_انتہا_ستون) == FAIL
	    || dict_جوڑو_نمبر(dict, "vcol",     (طویل)qfp->qf_viscol) == FAIL
	    || dict_جوڑو_نمبر(dict, "nr",       (طویل)qfp->qf_nr) == FAIL
	    || dict_جوڑو_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_جوڑو_string(dict, "pattern", qfp->qf_نمونہ) == FAIL
	    || dict_جوڑو_string(dict, "text", qfp->qf_متن) == FAIL
	    || dict_جوڑو_string_len(dict, "type", buf, (صحیح)buflen) == FAIL
	    || (qfp->qf_user_ڈیٹا.v_type != VAR_UNKNOWN
		&& dict_جوڑو_tv(dict, "user_data", &qfp->qf_user_ڈیٹا) == FAIL )
	    || dict_جوڑو_نمبر(dict, "valid", (طویل)qfp->qf_valid) == FAIL)
	واپس FAIL;

    واپس OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    ساکن صحیح
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	صحیح		qf_idx,
	صحیح		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحیح		i;

    اگر (qi == تہی)
    {
	qi = ql_info;
	اگر (wp != تہی)
	    qi = GET_LOC_LIST(wp);
	اگر (qi == تہی)
	    واپس FAIL;

    }

    اگر (eidx < 0)
	واپس OK;

    اگر (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    اگر (qf_idx >= qi->qf_listcount)
	واپس FAIL;

    qfl = qf_get_list(qi, qf_idx);
    اگر (qf_list_خالی_خانے(qfl))
	واپس FAIL;

    FOR_ALL_QFL_اشیاء(qfl, qfp, i)
    {
	اگر (eidx > 0)
	{
	    اگر (eidx == i)
		واپس get_qfline_اشیاء(qfp, list);
	}
	ورنہ اگر (get_qfline_اشیاء(qfp, list) == FAIL)
	    واپس FAIL;
    }

    واپس OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
شمار {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_اشیاء	= 0x2,
    QF_GETLIST_NR	= 0x4,
    QF_GETLIST_WINID	= 0x8,
    QF_GETLIST_CONTEXT	= 0x10,
    QF_GETLIST_ID	= 0x20,
    QF_GETLIST_IDX	= 0x40,
    QF_GETLIST_سائز	= 0x80,
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
    ساکن صحیح
qf_get_list_from_سطریں(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    صحیح		حالت_2 = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    اگر (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == تہی)
	واپس FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    اگر ((efm_di = dict_ڈھونڈو(what, (char_u *)"efm", -1)) != تہی)
    {
	اگر (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == تہی)
	    واپس FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    اگر (l == تہی)
	واپس FAIL;

    qi = qf_alloc_ڈھیر(QFLT_اندرونی, 1);
    اگر (qi != تہی)
    {
	اگر (qf_init_ext(qi, 0, تہی, تہی, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, تہی, تہی) > 0)
	{
	    (خالی)get_errorlist(qi, تہی, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_جوڑو_list(retdict, "items", l);
    حالت_2 = OK;

    واپس حالت_2;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    ساکن صحیح
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    اگر (qi == تہی)
	واپس 0;
    win = qf_ڈھونڈو_win(qi);
    اگر (win != تہی)
	واپس win->w_id;
    واپس 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    ساکن صحیح
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    صحیح	bufnum = 0;

    اگر (qi != تہی && buflist_findnr(qi->qf_bufnr) != تہی)
	bufnum = qi->qf_bufnr;

    واپس dict_جوڑو_نمبر(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    ساکن صحیح
qf_getprop_keys2flags(dict_T *what, صحیح loclist)
{
    صحیح		flags = QF_GETLIST_NONE;

    اگر (dict_has_کلید(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	اگر (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    اگر (dict_has_کلید(what, "title"))
	flags |= QF_GETLIST_TITLE;

    اگر (dict_has_کلید(what, "nr"))
	flags |= QF_GETLIST_NR;

    اگر (dict_has_کلید(what, "winid"))
	flags |= QF_GETLIST_WINID;

    اگر (dict_has_کلید(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    اگر (dict_has_کلید(what, "id"))
	flags |= QF_GETLIST_ID;

    اگر (dict_has_کلید(what, "items"))
	flags |= QF_GETLIST_اشیاء;

    اگر (dict_has_کلید(what, "idx"))
	flags |= QF_GETLIST_IDX;

    اگر (dict_has_کلید(what, "size"))
	flags |= QF_GETLIST_سائز;

    اگر (dict_has_کلید(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    اگر (loclist && dict_has_کلید(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    اگر (dict_has_کلید(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    اگر (dict_has_کلید(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    واپس flags;
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
    ساکن صحیح
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    صحیح		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    اگر ((di = dict_ڈھونڈو(what, (char_u *)"nr", -1)) != تہی)
    {
	// Use the specified quickfix/location list
	اگر (di->di_tv.v_type == VAR_نمبر)
	{
	    // for zero use the current list
	    اگر (di->di_tv.vval.v_نمبر != 0)
	    {
		qf_idx = di->di_tv.vval.v_نمبر - 1;
		اگر (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	ورنہ اگر (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != تہی
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	ورنہ
	    qf_idx = INVALID_QFIDX;
    }

    اگر ((di = dict_ڈھونڈو(what, (char_u *)"id", -1)) != تہی)
    {
	// Look for a list with the specified id
	اگر (di->di_tv.v_type == VAR_نمبر)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    اگر (di->di_tv.vval.v_نمبر != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_نمبر);
	}
	ورنہ
	    qf_idx = INVALID_QFIDX;
    }

    واپس qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    ساکن صحیح
qf_getprop_defaults(qf_info_T *qi, صحیح flags, صحیح locstack, dict_T *retdict)
{
    صحیح		حالت_2 = OK;

    اگر (flags & QF_GETLIST_TITLE)
	حالت_2 = dict_جوڑو_string_len(retdict, "title", (char_u *)"", 0);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_اشیاء))
    {
	list_T	*l = list_alloc();
	اگر (l != تہی)
	    حالت_2 = dict_جوڑو_list(retdict, "items", l);
	ورنہ
	    حالت_2 = FAIL;
    }
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_NR))
	حالت_2 = dict_جوڑو_نمبر(retdict, "nr", 0);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_WINID))
	حالت_2 = dict_جوڑو_نمبر(retdict, "winid", qf_winid(qi));
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_CONTEXT))
	حالت_2 = dict_جوڑو_string_len(retdict, "context", (char_u *)"", 0);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_ID))
	حالت_2 = dict_جوڑو_نمبر(retdict, "id", 0);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_IDX))
	حالت_2 = dict_جوڑو_نمبر(retdict, "idx", 0);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_سائز))
	حالت_2 = dict_جوڑو_نمبر(retdict, "size", 0);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_TICK))
	حالت_2 = dict_جوڑو_نمبر(retdict, "changedtick", 0);
    اگر ((حالت_2 == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	حالت_2 = dict_جوڑو_نمبر(retdict, "filewinid", 0);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_QFBUFNR))
	حالت_2 = qf_getprop_qfbufnr(qi, retdict);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_QFTF))
	حالت_2 = dict_جوڑو_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    واپس حالت_2;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    ساکن صحیح
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    واپس dict_جوڑو_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    ساکن صحیح
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    صحیح winid = 0;

    اگر (wp != تہی && IS_LL_کھڑکی(wp))
    {
	win_T	*ll_wp = qf_ڈھونڈو_win_with_loclist(qi);
	اگر (ll_wp != تہی)
	    winid = ll_wp->w_id;
    }

    واپس dict_جوڑو_نمبر(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    ساکن صحیح
qf_getprop_اشیاء(qf_info_T *qi, صحیح qf_idx, صحیح eidx, dict_T *retdict)
{
    صحیح		حالت_2 = OK;
    list_T	*l = list_alloc();
    اگر (l != تہی)
    {
	(خالی)get_errorlist(qi, تہی, qf_idx, eidx, l);
	dict_جوڑو_list(retdict, "items", l);
    }
    ورنہ
	حالت_2 = FAIL;

    واپس حالت_2;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    ساکن صحیح
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    صحیح		حالت_2;
    dictitem_T	*di;

    اگر (qfl->qf_ctx != تہی)
    {
	di = dictitem_alloc((char_u *)"context");
	اگر (di != تہی)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    حالت_2 = dict_جوڑو(retdict, di);
	    اگر (حالت_2 == FAIL)
		dictitem_free(di);
	}
	ورنہ
	    حالت_2 = FAIL;
    }
    ورنہ
	حالت_2 = dict_جوڑو_string_len(retdict, "context", (char_u *)"", 0);

    واپس حالت_2;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    ساکن صحیح
qf_getprop_idx(qf_list_T *qfl, صحیح eidx, dict_T *retdict)
{
    اگر (eidx == 0)
    {
	eidx = qfl->qf_اشاریہ;
	اگر (qf_list_خالی_خانے(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    واپس dict_جوڑو_نمبر(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    ساکن صحیح
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    صحیح		حالت_2;

    اگر (qfl->qf_qftf_cb.cb_name != تہی)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	حالت_2 = dict_جوڑو_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    ورنہ
	حالت_2 = dict_جوڑو_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    واپس حالت_2;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    ساکن صحیح
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    صحیح		حالت_2 = OK;
    صحیح		qf_idx = INVALID_QFIDX;
    صحیح		eidx = 0;
    dictitem_T	*di;
    صحیح		flags = QF_GETLIST_NONE;

    اگر ((di = dict_ڈھونڈو(what, (char_u *)"lines", -1)) != تہی)
	واپس qf_get_list_from_سطریں(what, di, retdict);

    اگر (wp != تہی)
	qi = GET_LOC_LIST(wp);
    ورنہ اگر (qi == تہی)
	واپس FAIL;

    flags = qf_getprop_keys2flags(what, (wp != تہی));

    اگر (!qf_ڈھیر_خالی_خانے(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    اگر (qf_ڈھیر_خالی_خانے(qi) || qf_idx == INVALID_QFIDX)
	واپس qf_getprop_defaults(qi, flags, wp != تہی, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    اگر ((di = dict_ڈھونڈو(what, (char_u *)"idx", -1)) != تہی)
    {
	اگر (di->di_tv.v_type != VAR_نمبر)
	    واپس FAIL;
	eidx = di->di_tv.vval.v_نمبر;
    }

    اگر (flags & QF_GETLIST_TITLE)
	حالت_2 = qf_getprop_title(qfl, retdict);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_NR))
	حالت_2 = dict_جوڑو_نمبر(retdict, "nr", qf_idx + 1);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_WINID))
	حالت_2 = dict_جوڑو_نمبر(retdict, "winid", qf_winid(qi));
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_اشیاء))
	حالت_2 = qf_getprop_اشیاء(qi, qf_idx, eidx, retdict);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_CONTEXT))
	حالت_2 = qf_getprop_ctx(qfl, retdict);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_ID))
	حالت_2 = dict_جوڑو_نمبر(retdict, "id", qfl->qf_id);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_IDX))
	حالت_2 = qf_getprop_idx(qfl, eidx, retdict);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_سائز))
	حالت_2 = dict_جوڑو_نمبر(retdict, "size", qfl->qf_گنتی);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_TICK))
	حالت_2 = dict_جوڑو_نمبر(retdict, "changedtick", qfl->qf_changedtick);
    اگر ((حالت_2 == OK) && (wp != تہی) && (flags & QF_GETLIST_FILEWINID))
	حالت_2 = qf_getprop_filewinid(wp, qi, retdict);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_QFBUFNR))
	حالت_2 = qf_getprop_qfbufnr(qi, retdict);
    اگر ((حالت_2 == OK) && (flags & QF_GETLIST_QFTF))
	حالت_2 = qf_getprop_qftf(qfl, retdict);

    واپس حالت_2;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    ساکن صحیح
qf_جوڑو_اندراج_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	صحیح		پہلا_اندراج,
	صحیح		*valid_اندراج)
{
    ساکن صحیح	did_bufnr_emsg;
    char_u	*filename, *module, *نمونہ, *متن, *type;
    صحیح		bufnum, valid, حالت_2, ستون, انتہا_ستون, vcol, nr;
    طویل	lnum, انتہا_lnum;

    اگر (پہلا_اندراج)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (صحیح)dict_get_نمبر(d, "bufnr");
    lnum = (صحیح)dict_get_نمبر(d, "lnum");
    انتہا_lnum = (صحیح)dict_get_نمبر(d, "end_lnum");
    ستون = (صحیح)dict_get_نمبر(d, "col");
    انتہا_ستون = (صحیح)dict_get_نمبر(d, "end_col");
    vcol = (صحیح)dict_get_نمبر(d, "vcol");
    nr = (صحیح)dict_get_نمبر(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    نمونہ = dict_get_string(d, "pattern", TRUE);
    متن = dict_get_string(d, "text", TRUE);
    اگر (متن == تہی)
	متن = vim_strsave((char_u *)"");
    typval_T user_ڈیٹا;
    user_ڈیٹا.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_ڈیٹا);

    valid = TRUE;
    اگر ((filename == تہی && bufnum == 0) || (lnum == 0 && نمونہ == تہی))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    اگر (bufnum != 0 && (buflist_findnr(bufnum) == تہی))
    {
	اگر (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_بفر_nr_not_ملا), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    اگر (dict_has_کلید(d, "valid"))
	valid = (صحیح)dict_get_bool(d, "valid", FALSE);

    حالت_2 =  qf_جوڑو_اندراج(qfl,
			تہی,		// dir
			filename,
			module,
			bufnum,
			متن,
			lnum,
			انتہا_lnum,
			ستون,
			انتہا_ستون,
			vcol,		// vis_col
			نمونہ,	// search pattern
			nr,
			type == تہی ? NUL : *type,
			&user_ڈیٹا,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(نمونہ);
    vim_free(متن);
    vim_free(type);
    clear_tv(&user_ڈیٹا);

    اگر (valid)
	*valid_اندراج = TRUE;

    واپس حالت_2;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    ساکن صحیح
اندراج_is_closer_مقصد_ہدف(
	qfline_T	*اندراج_2,
	qfline_T	*other_اندراج,
	صحیح		ہدف_fnum,
	صحیح		ہدف_lnum,
	صحیح		ہدف_ستون)
{
    // First, compare entries to target file.
    اگر (!ہدف_fnum)
	// Without a target file, we can't know which is closer.
	واپس FALSE;

    صحیح is_ہدف_file = اندراج_2->qf_fnum && اندراج_2->qf_fnum == ہدف_fnum;
    صحیح other_is_ہدف_file = other_اندراج->qf_fnum && other_اندراج->qf_fnum == ہدف_fnum;
    اگر (!is_ہدف_file && other_is_ہدف_file)
	واپس FALSE;
    ورنہ اگر (is_ہدف_file && !other_is_ہدف_file)
	واپس TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    اگر (!ہدف_lnum)
	// Without a target line number, we can't know which is closer.
	واپس FALSE;

    صحیح سطر_فاصلہ = اندراج_2->qf_lnum ? labs(اندراج_2->qf_lnum - ہدف_lnum) : زیادہ_سے_زیادہ_عدد;
    صحیح other_سطر_فاصلہ = other_اندراج->qf_lnum ? labs(other_اندراج->qf_lnum - ہدف_lnum) : زیادہ_سے_زیادہ_عدد;
    اگر (سطر_فاصلہ > other_سطر_فاصلہ)
	واپس FALSE;
    ورنہ اگر (سطر_فاصلہ < other_سطر_فاصلہ)
	واپس TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    اگر (!ہدف_ستون)
	// Without a target column, we can't know which is closer.
	واپس FALSE;

    صحیح عمود_فاصلہ = اندراج_2->qf_ستون ? مطلق_قدر(اندراج_2->qf_ستون - ہدف_ستون) : زیادہ_سے_زیادہ_عدد;
    صحیح other_عمود_فاصلہ = other_اندراج->qf_ستون ? مطلق_قدر(other_اندراج->qf_ستون - ہدف_ستون): زیادہ_سے_زیادہ_عدد;
    اگر (عمود_فاصلہ > other_عمود_فاصلہ)
	واپس FALSE;
    ورنہ اگر (عمود_فاصلہ < other_عمود_فاصلہ)
	واپس TRUE;

    // It's a complete tie! The exact same file, line, and column.
    واپس FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    ساکن صحیح
qf_جوڑو_entries(
	qf_info_T	*qi,
	صحیح		qf_idx,
	list_T		*list,
	char_u		*title,
	صحیح		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_آخری = تہی;
    صحیح		retval = OK;
    صحیح		valid_اندراج = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    صحیح پچھلا_fnum = 0;
    صحیح پچھلا_lnum = 0;
    صحیح پچھلا_ستون = 0;
    اگر (qfl->qf_ptr)
    {
	پچھلا_fnum = qfl->qf_ptr->qf_fnum;
	پچھلا_lnum = qfl->qf_ptr->qf_lnum;
	پچھلا_ستون = qfl->qf_ptr->qf_ستون;
    }

    صحیح select_پہلا_اندراج = FALSE;
    صحیح select_قریب_ترین_اندراج = FALSE;

    اگر (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_پہلا_اندراج = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ورنہ اگر (action == 'a')
    {
	اگر (qf_list_خالی_خانے(qfl))
	    // Appending to empty list, select first entry.
	    select_پہلا_اندراج = TRUE;
	ورنہ
	    // Adding to existing list, use last entry.
	    old_آخری = qfl->qf_آخری;
    }
    ورنہ اگر (action == 'r')
    {
	select_پہلا_اندراج = TRUE;
	qf_free_اشیاء(qfl);
	qf_store_title(qfl, title);
    }
    ورنہ اگر (action == 'u')
    {
	select_قریب_ترین_اندراج = TRUE;
	qf_free_اشیاء(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *اندراج_مقصد_select = تہی;
    صحیح اندراج_مقصد_select_اشاریہ = 0;

    FOR_ALL_LIST_اشیاء(list, li)
    {
	اگر (li->li_tv.v_type != VAR_DICT)
	    جاری; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	اگر (d == تہی)
	    جاری;

	retval = qf_جوڑو_اندراج_from_dict(qfl, d, li == list->lv_پہلا,
								&valid_اندراج);
	اگر (retval == QF_FAIL)
	    توڑو;

	qfline_T *اندراج_2 = qfl->qf_آخری;
	اگر (
	    (select_پہلا_اندراج && اندراج_مقصد_select == تہی) ||
	    (select_قریب_ترین_اندراج &&
		(اندراج_مقصد_select == تہی ||
		 اندراج_is_closer_مقصد_ہدف(اندراج_2, اندراج_مقصد_select, پچھلا_fnum,
					   پچھلا_lnum, پچھلا_ستون))))
	{
	    اندراج_مقصد_select = اندراج_2;
	    اندراج_مقصد_select_اشاریہ = qfl->qf_گنتی;
	}
    }

    // Check if any valid error entries are added to the list.
    اگر (valid_اندراج)
	qfl->qf_nonevalid = FALSE;
    ورنہ اگر (qfl->qf_اشاریہ == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    اگر (اندراج_مقصد_select)
    {
	qfl->qf_ptr = اندراج_مقصد_select;
	qfl->qf_اشاریہ = اندراج_مقصد_select_اشاریہ;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_بفر(qi, old_آخری);

    واپس retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    ساکن صحیح
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	صحیح		action,
	صحیح		*newlist)
{
    dictitem_T	*di;
    صحیح		qf_idx = qi->qf_curlist;    // default is the current list

    اگر ((di = dict_ڈھونڈو(what, (char_u *)"nr", -1)) != تہی)
    {
	// Use the specified quickfix/location list
	اگر (di->di_tv.v_type == VAR_نمبر)
	{
	    // for zero use the current list
	    اگر (di->di_tv.vval.v_نمبر != 0)
		qf_idx = di->di_tv.vval.v_نمبر - 1;

	    اگر ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_ڈھیر_خالی_خانے(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    ورنہ اگر (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		واپس INVALID_QFIDX;
	    ورنہ اگر (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	ورنہ اگر (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != تہی
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    اگر (!qf_ڈھیر_خالی_خانے(qi))
		qf_idx = qi->qf_listcount - 1;
	    ورنہ اگر (*newlist)
		qf_idx = 0;
	    ورنہ
		واپس INVALID_QFIDX;
	}
	ورنہ
	    واپس INVALID_QFIDX;
    }

    اگر (!*newlist && (di = dict_ڈھونڈو(what, (char_u *)"id", -1)) != تہی)
    {
	// Use the quickfix/location list with the specified id
	اگر (di->di_tv.v_type != VAR_نمبر)
	    واپس INVALID_QFIDX;

	واپس qf_id2nr(qi, di->di_tv.vval.v_نمبر);
    }

    واپس qf_idx;
}

/*
 * Set the quickfix list title.
 */
    ساکن صحیح
qf_setprop_title(qf_info_T *qi, صحیح qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    اگر (di->di_tv.v_type != VAR_STRING)
	واپس FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    اگر (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    واپس OK;
}

/*
 * Set quickfix list items/entries.
 */
    ساکن صحیح
qf_setprop_اشیاء(qf_info_T *qi, صحیح qf_idx, dictitem_T *di, صحیح action)
{
    صحیح		retval = FAIL;
    char_u	*title_save;

    اگر (di->di_tv.v_type != VAR_LIST)
	واپس FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_جوڑو_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    واپس retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    ساکن صحیح
qf_setprop_اشیاء_from_سطریں(
	qf_info_T	*qi,
	صحیح		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	صحیح		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    صحیح		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    اگر ((efm_di = dict_ڈھونڈو(what, (char_u *)"efm", -1)) != تہی)
    {
	اگر (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == تہی)
	    واپس FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    اگر (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == تہی)
	واپس FAIL;

    اگر (action == 'r' || action == 'u')
	qf_free_اشیاء(&qi->qf_lists[qf_idx]);
    اگر (qf_init_ext(qi, qf_idx, تہی, تہی, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, تہی, تہی) >= 0)
	retval = OK;

    واپس retval;
}

/*
 * Set quickfix list context.
 */
    ساکن صحیح
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    اگر (ctx != تہی)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    واپس OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    ساکن صحیح
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    صحیح		denote = FALSE;
    صحیح		newidx;
    صحیح		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    اگر (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != تہی
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_گنتی;
    ورنہ
    {
	// Otherwise use the specified index
	newidx = tv_get_نمبر_chk(&di->di_tv, &denote);
	اگر (denote)
	    واپس FAIL;
    }

    اگر (newidx < 1)		// sanity check
	واپس FAIL;
    اگر (newidx > qfl->qf_گنتی)
	newidx = qfl->qf_گنتی;

    old_qfidx = qfl->qf_اشاریہ;
    qf_ptr = get_nth_اندراج(qfl, newidx, &newidx);
    اگر (qf_ptr == تہی)
	واپس FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_اشاریہ = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    اگر (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    واپس OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    ساکن صحیح
qf_setprop_qftf(qf_info_T *qi غیر_مستعمل, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    اگر (check_restricted() || check_secure())
	واپس FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    اگر (cb.cb_name == تہی || *cb.cb_name == NUL)
	واپس OK;
    سیٹ_callback(&qfl->qf_qftf_cb, &cb);
    واپس OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    ساکن صحیح
qf_سیٹ_properties(qf_info_T *qi, dict_T *what, صحیح action, char_u *title)
{
    dictitem_T	*di;
    صحیح		retval = FAIL;
    صحیح		qf_idx;
    صحیح		newlist = FALSE;
    qf_list_T	*qfl;

    اگر (action == ' ' || qf_ڈھیر_خالی_خانے(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    اگر (qf_idx == INVALID_QFIDX)	// List not found
	واپس FAIL;

    اگر (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    اگر ((di = dict_ڈھونڈو(what, (char_u *)"title", -1)) != تہی)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    اگر ((di = dict_ڈھونڈو(what, (char_u *)"items", -1)) != تہی)
	retval = qf_setprop_اشیاء(qi, qf_idx, di, action);
    اگر ((di = dict_ڈھونڈو(what, (char_u *)"lines", -1)) != تہی)
	retval = qf_setprop_اشیاء_from_سطریں(qi, qf_idx, what, di, action);
    اگر ((di = dict_ڈھونڈو(what, (char_u *)"context", -1)) != تہی)
	retval = qf_setprop_context(qfl, di);
    اگر ((di = dict_ڈھونڈو(what, (char_u *)"idx", -1)) != تہی)
	retval = qf_setprop_curidx(qi, qfl, di);
    اگر ((di = dict_ڈھونڈو(what, (char_u *)"quickfixtextfunc", -1)) != تہی)
	retval = qf_setprop_qftf(qi, qfl, di);

    اگر (newlist || retval == OK)
	qf_list_بدلا(qfl);
    اگر (newlist)
	qf_update_بفر(qi, تہی);

    واپس retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    ساکن خالی
qf_free_ڈھیر(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_ڈھونڈو_win(qi);
    win_T	*llwin = تہی;

    اگر (qfwin != تہی)
    {
	// If the quickfix/location list window is open, then clear it
	اگر (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_بفر(qi, تہی);
    }

    اگر (wp != تہی && IS_LL_کھڑکی(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_ڈھونڈو_win_with_loclist(qi);
	اگر (llwin != تہی)
	    wp = llwin;
    }

    qf_free_all(wp);
    اگر (wp == تہی)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    ورنہ اگر (qfwin != تہی)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_ڈھیر(QFLT_LOCATION, wp->w_p_lhi);

	اگر (new_ll != تہی)
	{
	    new_ll->qf_bufnr = qfwin->w_بفر->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    اگر (wp != qfwin)
		win_سیٹ_loclist(wp, new_ll);
	}
    }
}

/*
 * Populate the quickfix list with the items supplied in the list
 * of dictionaries. "title" will be copied to w:quickfix_title.
 * "action" is 'a' for add, 'r' for replace, 'u' for update.  Otherwise
 * create a new list. When "what" is not NULL then only set some properties.
 */
    صحیح
سیٹ_errorlist(
	win_T	*wp,
	list_T	*list,
	صحیح	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    صحیح		retval = OK;

    اگر (wp != تہی)
	qi = ll_get_or_alloc_list(wp);
    ورنہ
	qi = ql_info;
    اگر (qi == تہی)
	واپس FAIL;

    اگر (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_ڈھیر(wp, qi);
	واپس OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    اگر (list->lv_len != 0 && what != تہی)
    {
	semsg(_(e_invalid_دلیل_str),
			 _("cannot have both a list and a \"what\" argument"));
	واپس FAIL;
    }

    incr_quickfix_busy();

    اگر (what != تہی)
	retval = qf_سیٹ_properties(qi, what, action, title);
    ورنہ
    {
	retval = qf_جوڑو_entries(qi, qi->qf_curlist, list, title, action);
	اگر (retval == OK)
	    qf_list_بدلا(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    واپس retval;
}

ساکن صحیح mark_quickfix_user_ڈیٹا(qf_info_T *qi, صحیح copyID)
{
    صحیح abort = FALSE;
    برائے (صحیح i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	اگر (!qfl->qf_has_user_ڈیٹا)
	    جاری;
	qfline_T *qfp;
	صحیح j;
	FOR_ALL_QFL_اشیاء(qfl, qfp, j)
	{
	    typval_T* user_ڈیٹا = &qfp->qf_user_ڈیٹا;
	    اگر (user_ڈیٹا != تہی && user_ڈیٹا->v_type != VAR_نمبر
		&& user_ڈیٹا->v_type != VAR_STRING && user_ڈیٹا->v_type != VAR_FLOAT)
		abort = abort || سیٹ_ref_اندر_شے(user_ڈیٹا, copyID, تہی, تہی, تہی);
	}
    }
    واپس abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    ساکن صحیح
mark_quickfix_ctx(qf_info_T *qi, صحیح copyID)
{
    صحیح		i;
    صحیح		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    برائے (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	اگر (ctx != تہی && ctx->v_type != VAR_نمبر
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || سیٹ_ref_اندر_شے(ctx, copyID, تہی, تہی, تہی);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || سیٹ_ref_اندر_callback(cb, copyID);
    }

    واپس abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    صحیح
سیٹ_ref_اندر_quickfix(صحیح copyID)
{
    صحیح		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    اگر (ql_info == تہی)
	واپس TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    اگر (abort)
	واپس abort;

    abort = mark_quickfix_user_ڈیٹا(ql_info, copyID);
    اگر (abort)
	واپس abort;

    abort = سیٹ_ref_اندر_callback(&qftf_cb, copyID);
    اگر (abort)
	واپس abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	اگر (win->w_llist != تہی)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    اگر (abort)
		واپس abort;

	    abort = mark_quickfix_user_ڈیٹا(win->w_llist, copyID);
	    اگر (abort)
		واپس abort;
	}
	اگر (IS_LL_کھڑکی(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    اگر (abort)
		واپس abort;

	    abort = mark_quickfix_user_ڈیٹا(win->w_llist_ref, copyID);
	    اگر (abort)
		واپس abort;
	}
    }

    واپس abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    ساکن char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_cbuffer:	واپس (char_u *)"cbuffer";
	حالت CMD_cgetbuffer:	واپس (char_u *)"cgetbuffer";
	حالت CMD_caddbuffer:	واپس (char_u *)"caddbuffer";
	حالت CMD_lbuffer:	واپس (char_u *)"lbuffer";
	حالت CMD_lgetbuffer:	واپس (char_u *)"lgetbuffer";
	حالت CMD_laddbuffer:	واپس (char_u *)"laddbuffer";
	طےشدہ:		واپس تہی;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    ساکن صحیح
cbuffer_process_دلائل(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = تہی;

    اگر (*eap->arg == NUL)
	buf = curbuf;
    ورنہ اگر (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((حرف *)eap->arg));

    اگر (buf == تہی)
    {
	emsg(_(e_invalid_دلیل));
	واپس FAIL;
    }

    اگر (buf->b_ml.ml_mfp == تہی)
    {
	emsg(_(e_بفر_is_not_loaded));
	واپس FAIL;
    }

    اگر (eap->addr_گنتی == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_سطر_گنتی;
    }

    اگر (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_سطر_گنتی
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_سطر_گنتی)
    {
	emsg(_(e_invalid_range));
	واپس FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    واپس OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    خالی
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = تہی;
    qf_info_T	*qi;
    char_u	*au_name = تہی;
    صحیح		res;
    int_u	save_qfid;
    win_T	*wp = تہی;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    اگر (au_name != تہی && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    واپس;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_ڈھیر(eap, &wp);
    اگر (qi == تہی)
	واپس;

    اگر (cbuffer_process_دلائل(eap, &buf, &line1, &line2) == FAIL)
	واپس;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    اگر (buf->b_sfname)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "%s (%s)",
		(حرف *)qf_title, (حرف *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, تہی, buf, تہی, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, تہی);
    اگر (qf_ڈھیر_خالی_خانے(qi))
    {
	decr_quickfix_busy();
	واپس;
    }
    اگر (res >= 0)
	qf_list_بدلا(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    اگر (au_name != تہی)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	اگر (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    اگر (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_پہلا(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# اگر defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_cexpr:	    واپس (char_u *)"cexpr";
	حالت CMD_cgetexpr:  واپس (char_u *)"cgetexpr";
	حالت CMD_caddexpr:  واپس (char_u *)"caddexpr";
	حالت CMD_lexpr:	    واپس (char_u *)"lexpr";
	حالت CMD_lgetexpr:  واپس (char_u *)"lgetexpr";
	حالت CMD_laddexpr:  واپس (char_u *)"laddexpr";
	طےشدہ:	    واپس تہی;
    }
}

    صحیح
trigger_cexpr_autocmd(صحیح cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    اگر (au_name != تہی && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	اگر (aborting())
	    واپس FAIL;
    }
    واپس OK;
}

    صحیح
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = تہی;

    qi = qf_cmd_get_or_alloc_ڈھیر(eap, &wp);
    اگر (qi == تہی)
	واپس FAIL;

    اگر ((tv->v_type == VAR_STRING && tv->vval.v_string != تہی)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != تہی))
    {
	صحیح	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, تہی, تہی, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), تہی);
	اگر (qf_ڈھیر_خالی_خانے(qi))
	{
	    decr_quickfix_busy();
	    واپس FAIL;
	}
	اگر (res >= 0)
	    qf_list_بدلا(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	اگر (au_name != تہی)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	اگر (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_پہلا(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	واپس OK;
    }

    emsg(_(e_string_or_list_expected));
    واپس FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    خالی
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    اگر (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	واپس;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_اظہاریہ(eap->arg, eap);
    اگر (tv == تہی)
	واپس;

    (خالی)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    ساکن qf_info_T *
hgr_get_ll(صحیح *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    اگر (bt_help(curwin->w_بفر))
	wp = curwin;
    ورنہ
	// Find an existing help window
	wp = qf_ڈھونڈو_help_win();

    اگر (wp == تہی)	    // Help window not found
	qi = تہی;
    ورنہ
	qi = wp->w_llist;

    اگر (qi == تہی)
    {
	// Allocate a new location list for help text matches
	اگر ((qi = qf_alloc_ڈھیر(QFLT_LOCATION, 1)) == تہی)
	    واپس تہی;
	*new_ll = TRUE;
    }

    واپس qi;
}

/*
 * Search for a pattern in a help file.
 */
    ساکن خالی
hgr_تلاش_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    طویل	lnum;

    fd = mch_fopen((حرف *)fname, "r");
    اگر (fd == تہی)
	واپس;

    lnum = 1;
    جب (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *سطر = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	اگر (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    سطر = string_convert(p_vc, IObuff, تہی);
	    اگر (سطر == تہی)
		سطر = IObuff;
	}

	اگر (vim_regexec(p_regmatch, سطر, (colnr_T)0))
	{
	    صحیح	l = (صحیح)STRLEN(سطر);

	    // remove trailing CR, LF, spaces, etc.
	    جب (l > 0 && سطر[l - 1] <= ' ')
		سطر[--l] = NUL;

	    اگر (qf_جوڑو_اندراج(qfl,
			تہی,	// dir
			fname,
			تہی,
			0,
			سطر,
			lnum,
			0,
			(صحیح)(p_regmatch->startp[0] - سطر)
			+ 1,	//ستون
			(صحیح)(p_regmatch->endp[0] - سطر)
			+ 1,	// end_col
			FALSE,	// vis_col
			تہی,	// search pattern
			0,	// nr
			1,	// type
			تہی,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		اگر (سطر != IObuff)
		    vim_free(سطر);
		توڑو;
	    }
	}
	اگر (سطر != IObuff)
	    vim_free(سطر);
	++lnum;
	سطر_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    ساکن خالی
hgr_تلاش_files_اندر_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    صحیح		fcount;
    char_u	**fnames;
    صحیح		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    جوڑو_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    اگر (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	برائے (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    اگر (lang != تہی
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		جاری;
# endif

	    hgr_تلاش_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    ساکن خالی
hgr_تلاش_اندر_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    اگر (!enc_utf8)
	convert_تیاری(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    جب (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_تلاش_files_اندر_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    اگر (vc.vc_type != CONV_NONE)
	convert_تیاری(&vc, تہی, تہی);
}

/*
 * ":helpgrep {pattern}"
 */
    خالی
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    صحیح		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    صحیح		new_qi = FALSE;
    char_u	*au_name =  تہی;
    char_u	*lang = تہی;
    صحیح		updated = FALSE;

    انتخاب (eap->cmdidx)
    {
	حالت CMD_helpgrep:  au_name = (char_u *)"helpgrep"; توڑو;
	حالت CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; توڑو;
	طےشدہ: توڑو;
    }
    اگر (au_name != تہی && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    واپس;
# endif
    }

    اگر (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	اگر (qi == تہی)
	    واپس;
    }
    ورنہ اگر (qi == تہی)
    {
	emsg(_(e_no_quickfix_ڈھیر));
	واپس;
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    save_cpo_allocated = is_option_allocated("cpo");
    p_cpo = خالی_خانے_option;

    incr_quickfix_busy();

# ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(eap->arg);
# endif
    regmatch.regprog = vim_regcomp(eap->arg, RE_MAGIC + RE_STRING);
    regmatch.rm_ic = FALSE;
    اگر (regmatch.regprog != تہی)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_تلاش_اندر_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_آغاز;
	qfl->qf_اشاریہ = 1;
	qf_list_بدلا(qfl);
	updated = TRUE;
    }

    اگر (p_cpo == خالی_خانے_option)
	p_cpo = save_cpo;
    ورنہ
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	اگر (*p_cpo == NUL)
	    سیٹ_option_قدر_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	اگر (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    اگر (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_بفر(qi, تہی);

    اگر (au_name != تہی)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	اگر (!new_qi && IS_LL_ڈھیر(qi) && qf_ڈھونڈو_win_with_loclist(qi) == تہی)
	{
	    decr_quickfix_busy();
	    واپس;
	}
    }

    // Jump to first match.
    اگر (!qf_list_خالی_خانے(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    ورنہ
	semsg(_(e_no_مطابقت_str_2), eap->arg);

    decr_quickfix_busy();

    اگر (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	اگر (!bt_help(curwin->w_بفر) || curwin->w_llist == qi)
	{
	    اگر (new_qi)
		ll_free_all(&qi);
	}
	ورنہ اگر (curwin->w_llist == تہی && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# اگر defined(EXITFREE)
    خالی
free_quickfix(خالی)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(تہی);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#اگر_ختم // FEAT_QUICKFIX

#اگر defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    ساکن خالی
get_qf_loc_list(صحیح is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    اگر (what_arg->v_type == VAR_UNKNOWN)
    {
	اگر (rettv_list_alloc(rettv) == OK)
	    اگر (is_qf || wp != تہی)
		(خالی)get_errorlist(تہی, wp, -1, 0, rettv->vval.v_list);
    }
    ورنہ
    {
	اگر (rettv_dict_alloc(rettv) == OK)
	    اگر (is_qf || (wp != تہی))
	    {
		اگر (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    اگر (d != تہی)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		ورنہ
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    خالی
f_getloclist(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    خالی
f_getqflist(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_QUICKFIX
    اگر (اندر_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	واپس;

    get_qf_loc_list(TRUE, تہی, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    ساکن خالی
سیٹ_qf_ll_list(
    win_T	*wp غیر_مستعمل,
    typval_T	*list_arg غیر_مستعمل,
    typval_T	*action_arg غیر_مستعمل,
    typval_T	*what_arg غیر_مستعمل,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    صحیح		action = 0;
    ساکن صحیح	recursive = 0;
# endif

    rettv->vval.v_نمبر = -1;

# ifdef FEAT_QUICKFIX
    اگر (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    ورنہ اگر (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    ورنہ
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = تہی;
	صحیح	valid_dict = TRUE;

	اگر (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    اگر (act == تہی)
		واپس;		// type error; errmsg already given
	    اگر ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    ورنہ
		semsg(_(e_invalid_action_str_1), act);
	}
	ورنہ اگر (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	ورنہ
	    emsg(_(e_string_required));

	اگر (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    اگر (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != تہی)
		what = what_arg->vval.v_dict;
	    ورنہ
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	اگر (l != تہی && action && valid_dict
		    && سیٹ_errorlist(wp, l, action,
		     (char_u *)(wp == تہی ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_نمبر = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    خالی
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	واپس;

    win = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    اگر (win != تہی)
	سیٹ_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    خالی
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	واپس;

    سیٹ_qf_ll_list(تہی, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#اگر_ختم
