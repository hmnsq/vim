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

#ضمن "vim.h"

#إذا defined(FEAT_QUICKFIX)

بنية dir_ډېری_T
{
    بنية dir_ډېری_T	*بل;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
تعريفنوع بنية qfline_S qfline_T;
بنية qfline_S
{
    qfline_T	*qf_بل;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_پای_ټکی_lnum;	// line number when the error has range or zero
    صحيح		qf_fnum;	// file number for the line
    صحيح		qf_ستون;		// column where the error occurred
    صحيح		qf_پای_ټکی_ستون;	// column when the error has range or zero
    صحيح		qf_nr;		// error number
    char_u	*qf_module;	// module name for this error
    char_u	*qf_fname;	// different filename if there're hard links
    char_u	*qf_نمونه;	// search pattern for the error
    char_u	*qf_متن;	// description of the error
    char_u	qf_viscol;	// set to TRUE if qf_col and qf_end_col is
				// screen column
    char_u	qf_cleared;	// set to TRUE if line has been deleted
    char_u	qf_type;	// type of the error (mostly 'E'); 1 for
				// :helpgrep
    typval_T	qf_user_معلومات;	// custom user data associated with this item
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
تعريفنوع تعداد
{
    QFLT_QUICKFIX, // Quickfix list - global list
    QFLT_LOCATION, // Location list - per window list
    QFLT_دننی  // Internal - Temporary list used by getqflist()/getloclist()
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
تعريفنوع بنية qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_پیل;	// pointer to the first error
    qfline_T	*qf_وروستی;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    صحيح		qf_شمېر;	// number of errors (0 means empty list)
    صحيح		qf_شاخص;	// current index in the error list
    صحيح		qf_nonevalid;	// TRUE if not a single valid entry found
    صحيح		qf_has_user_معلومات; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    بنية dir_ډېری_T	*qf_dir_ډېری;
    char_u		*qf_directory;
    بنية dir_ډېری_T	*qf_file_ډېری;
    char_u		*qf_currfile;
    صحيح			qf_multiline;
    صحيح			qf_multiignore;
    صحيح			qf_multiscan;
    اوږد		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
بنية qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    صحيح		qf_refcount;
    صحيح		qf_listcount;	    // current number of lists
    صحيح		qf_curlist;	    // current error list
    صحيح         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    صحيح		qf_bufnr;	    // quickfix window buffer number
};

ساكن qf_info_T ql_info_actual; // global quickfix list
ساكن qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
ساكن int_u وروستی_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
تعريفنوع بنية efm_S efm_T;
بنية efm_S
{
    regprog_T	    *prog;	// pre-formatted part of 'errorformat'
    efm_T	    *بل;	// pointer to next (NULL if last)
    char_u	    addr[FMT_PATTERNS]; // indices of used % patterns
    char_u	    مختاړی;	// prefix of this format line:
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
    صحيح		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
تعريفنوع بنية qf_delq_S
{
    بنية qf_delq_S	*بل;
    qf_info_T		*qi;
} qf_delq_T;
ساكن qf_delq_T *qf_delq_سر = عدم;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
ساكن صحيح	quickfix_busy = 0;

ساكن efm_T	*fmt_پیل = عدم; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
ساكن callback_T qftf_cb;

ساكن فراغ     qf_راوباسه_ډېری(qf_info_T *qi, صحيح adjust);
ساكن فراغ	qf_new_list(qf_info_T *qi, char_u *qf_title);
ساكن صحيح	qf_جمع_کړه_ثبت(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, صحيح bufnum, char_u *mesg, اوږد lnum, اوږد پای_ټکی_lnum, صحيح ستون, صحيح پای_ټکی_ستون, صحيح vis_ستون, char_u *نمونه, صحيح nr, صحيح type, typval_T *user_معلومات, صحيح valid);
ساكن صحيح      qf_resize_ډېری_بنسټ(qf_info_T *qi, صحيح n);
ساكن فراغ     qf_sync_llw_منزل_win(win_T *llw);
ساكن فراغ     qf_sync_win_منزل_llw(win_T *pwp);
ساكن qf_info_T *qf_alloc_ډېری(qfltype_T qfltype, صحيح n);
ساكن qf_list_T *qf_alloc_list_ډېری(صحيح n);
ساكن فراغ	qf_free(qf_list_T *qfl);
ساكن char_u	*qf_types(صحيح, صحيح);
ساكن صحيح	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
ساكن char_u	*qf_ټېل_dir(char_u *, بنية dir_ډېری_T **, صحيح is_file_ډېری);
ساكن char_u	*qf_راوباسه_dir(بنية dir_ډېری_T **);
ساكن char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
ساكن win_T    *qf_پیدا_کړه_win_with_loclist(qf_info_T *ll);
ساكن فراغ	qf_jump_newwin(qf_info_T *qi, صحيح dir, صحيح errornr, صحيح forceit, صحيح newwin);
ساكن فراغ	qf_fmt_متن(garray_T *gap, char_u *متن);
ساكن فراغ	qf_range_متن(garray_T *gap, qfline_T *qfp);
ساكن صحيح	qf_win_pos_update(qf_info_T *qi, صحيح old_qf_شاخص);
ساكن win_T	*qf_پیدا_کړه_win(qf_info_T *qi);
ساكن buf_T	*qf_پیدا_کړه_buf(qf_info_T *qi);
ساكن فراغ	qf_update_بفر(qf_info_T *qi, qfline_T *old_وروستی);
ساكن فراغ	qf_fill_بفر(qf_list_T *qfl, buf_T *buf, qfline_T *old_وروستی, صحيح qf_winid);
ساكن buf_T	*بارول_dummy_بفر(char_u *fname, char_u *dirname_پیل, char_u *resulting_dir);
ساكن فراغ	wipe_dummy_بفر(buf_T *buf, char_u *dirname_پیل);
ساكن فراغ	unload_dummy_بفر(buf_T *buf, char_u *dirname_پیل);
ساكن qf_info_T *ll_get_or_alloc_list(win_T *);
ساكن صحيح	ثبت_is_closer_منزل_موخه(qfline_T *ثبت, qfline_T *other_ثبت, صحيح موخه_fnum, صحيح موخه_lnum, صحيح موخه_ستون);

// Quickfix window check helper macro
# define IS_QF_کړکۍ(wp) (bt_quickfix((wp)->w_بفر) && (wp)->w_llist_ref == عدم)
// Location list window check helper macro
# define IS_LL_کړکۍ(wp) (bt_quickfix((wp)->w_بفر) && (wp)->w_llist_ref != عدم)

// Quickfix and location list stack check helper macros
# define IS_QF_ډېری(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_ډېری(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_کړکۍ(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_توکي(qfl, qfp, i) \
		    لكل ((i) = 1, (qfp) = (qfl)->qf_پیل; \
			    !got_int && (i) <= (qfl)->qf_شمېر && (qfp) != عدم; \
			    ++(i), (qfp) = (qfp)->qf_بل)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
ساكن char_u   *qf_وروستی_bufname = عدم;
ساكن bufref_T  qf_وروستی_bufref = {عدم, 0, 0};

ساكن garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    ساكن garray_T *
qfga_get(فراغ)
{
    ساكن صحيح initialized = FALSE;

    إذا (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    أرجع &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    ساكن فراغ
qfga_clear(فراغ)
{
    إذا (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    وإلا
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define کرښه_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
ساكن بنية fmtpattern
{
    char_u	convchar;
    حرف	*نمونه;
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
# define FMT_نمونه_M 8
	{'m', ".\\+"},		// 8
# define FMT_نمونه_R 9
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
    ساكن char_u *
efmpat_منزل_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	صحيح	idx,
	صحيح	پړاو)
{
    char_u	*srcptr;

    إذا (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_دننه_format_string), *efmpat);
	أرجع عدم;
    }
    إذا ((idx && idx < FMT_نمونه_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->مختاړی) != عدم)
	    || (idx == FMT_نمونه_R
		&& vim_strchr((char_u *)"OPQ", efminfo->مختاړی) == عدم))
    {
	semsg(_(e_unexpected_chr_دننه_format_str), *efmpat);
	أرجع عدم;
    }
    efminfo->addr[idx] = (char_u)++پړاو;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_دننه_FILENAME
    إذا (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    إذا (*efmpat == 'f' && efmpat[1] != NUL)
    {
	إذا (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	وإلا
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    وإلا
    {
	srcptr = (char_u *)fmt_pat[idx].نمونه;
	طالما ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    أرجع regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    ساكن char_u *
scanf_fmt_منزل_regpat(
	char_u	**pefmp,
	char_u	*efm,
	صحيح	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    إذا (*efmp == '[' || *efmp == '\\')
    {
	إذا ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    إذا (efmp[1] == '^')
		*regpat++ = *++efmp;
	    إذا (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		طالما (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		إذا (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_دننه_format_string));
		    أرجع عدم;
		}
	    }
	}
	وإلا إذا (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    وإلا
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_دننه_format_string), *efmp);
	أرجع عدم;
    }

    *pefmp = efmp;

    أرجع regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    ساكن char_u *
efm_analyze_مختاړی(char_u *efmp, efm_T *efminfo)
{
    إذا (vim_strchr((char_u *)"+-", *efmp) != عدم)
	efminfo->flags = *efmp++;
    إذا (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != عدم)
	efminfo->مختاړی = *efmp;
    وإلا
    {
	semsg(_(e_invalid_chr_دننه_format_string_مختاړی), *efmp);
	أرجع عدم;
    }

    أرجع efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    ساكن صحيح
efm_منزل_regpat(
	char_u	*efm,
	صحيح	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    صحيح		پړاو;
    صحيح		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    پړاو = 0;
    لكل (efmp = efm; efmp < efm + len; ++efmp)
    {
	إذا (*efmp == '%')
	{
	    ++efmp;
	    لكل (idx = 0; idx < FMT_PATTERNS; ++idx)
		إذا (fmt_pat[idx].convchar == *efmp)
		    اكسر;
	    إذا (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_منزل_regpat(efmp, ptr, fmt_ptr, idx, پړاو);
		إذا (ptr == عدم)
		    أرجع FAIL;
		پړاو++;
	    }
	    وإلا إذا (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_منزل_regpat(&efmp, efm, len, ptr);
		إذا (ptr == عدم)
		    أرجع FAIL;
	    }
	    وإلا إذا (vim_strchr((char_u *)"%\\.^$~[", *efmp) != عدم)
		*ptr++ = *efmp;		// regexp magic characters
	    وإلا إذا (*efmp == '#')
		*ptr++ = '*';
	    وإلا إذا (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    وإلا إذا (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_مختاړی(efmp, fmt_ptr);
		إذا (efmp == عدم)
		    أرجع FAIL;
	    }
	    وإلا
	    {
		semsg(_(e_invalid_chr_دننه_format_string), *efmp);
		أرجع FAIL;
	    }
	}
	وإلا			// copy normal character
	{
	    إذا (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    وإلا إذا (vim_strchr((char_u *)".*^$~[", *efmp) != عدم)
		*ptr++ = '\\';	// escape regexp atoms
	    إذا (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    أرجع OK;
}

/*
 * Free the 'errorformat' information list
 */
    ساكن فراغ
free_efm_list(efm_T **efm_لومړی)
{
    efm_T *efm_ptr;

    لكل (efm_ptr = *efm_لومړی; efm_ptr != عدم; efm_ptr = *efm_لومړی)
    {
	*efm_لومړی = efm_ptr->بل;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_پیل = عدم;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    ساكن صحيح
efm_regpat_bufsz(char_u *efm)
{
    صحيح sz;
    صحيح i;

    sz = (FMT_PATTERNS * 3) + ((صحيح)STRLEN(efm) << 2);
    لكل (i = FMT_PATTERNS; i > 0; )
	sz += (صحيح)STRLEN(fmt_pat[--i].نمونه);
# ifdef BACKSLASH_دننه_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# وإلا
    sz += 2; // "%f" can become two chars longer
# endif

    أرجع sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    ساكن صحيح
efm_option_part_len(char_u *efm)
{
    صحيح len;

    لكل (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	إذا (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    أرجع len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    ساكن efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = عدم;
    efm_T	*fmt_لومړی = عدم;
    efm_T	*fmt_وروستی = عدم;
    char_u	*fmtstr = عدم;
    صحيح		len;
    صحيح		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    إذا ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == عدم)
	اذهب parse_efm_error;

    طالما (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	إذا (fmt_ptr == عدم)
	    اذهب parse_efm_error;
	إذا (fmt_لومړی == عدم)	    // first one
	    fmt_لومړی = fmt_ptr;
	وإلا
	    fmt_وروستی->بل = fmt_ptr;
	fmt_وروستی = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	إذا (efm_منزل_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    اذهب parse_efm_error;
	إذا ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == عدم)
	    اذهب parse_efm_error;
	// Advance to next part
	efm = skip_منزل_option_part(efm + len);	// skip comma and spaces
    }

    إذا (fmt_لومړی == عدم)	// nothing found
	emsg(_(e_errorformat_لري_no_نمونه));

    اذهب parse_efm_پای_ټکی;

parse_efm_error:
    free_efm_list(&fmt_لومړی);

parse_efm_پای_ټکی:
    vim_free(fmtstr);

    أرجع fmt_لومړی;
}

تعداد {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_پای_ټکی_د_ننوت = 2,
    QF_NOMEM = 3,
    QF_IGNORE_کرښه = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
تعريفنوع بنية {
    char_u	*linebuf;
    صحيح		linelen;
    char_u	*growbuf;
    صحيح		growbufsiz;
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
    ساكن char_u *
qf_grow_linebuf(qfstate_T *state, صحيح newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > کرښه_MAXLEN ? کرښه_MAXLEN - 1 : newsz;
    إذا (state->growbuf == عدم)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	إذا (state->growbuf == عدم)
	    أرجع عدم;
	state->growbufsiz = state->linelen;
    }
    وإلا إذا (state->linelen > state->growbufsiz)
    {
	إذا ((p = vim_realloc(state->growbuf, state->linelen + 1)) == عدم)
	    أرجع عدم;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    أرجع state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    ساكن صحيح
qf_get_بل_str_کرښه(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    صحيح		len;

    إذا (*p_str == NUL) // Reached the end of the string
	أرجع QF_پای_ټکی_د_ننوت;

    p = vim_strchr(p_str, '\n');
    إذا (p != عدم)
	len = (صحيح)(p - p_str) + 1;
    وإلا
	len = (صحيح)STRLEN(p_str);

    إذا (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	إذا (state->linebuf == عدم)
	    أرجع QF_NOMEM;
    }
    وإلا
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    أرجع QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    ساكن صحيح
qf_get_بل_list_کرښه(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    صحيح		len;

    طالما (p_li != عدم
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == عدم))
	p_li = p_li->li_بل;	// Skip non-string items

    إذا (p_li == عدم)		// End of the list
    {
	state->p_li = عدم;
	أرجع QF_پای_ټکی_د_ننوت;
    }

    len = (صحيح)STRLEN(p_li->li_tv.vval.v_string);
    إذا (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	إذا (state->linebuf == عدم)
	    أرجع QF_NOMEM;
    }
    وإلا
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_بل;	// next item
    أرجع QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    ساكن صحيح
qf_get_بل_buf_کرښه(qfstate_T *state)
{
    char_u	*p_buf = عدم;
    صحيح		len;

    // Get the next line from the supplied buffer
    إذا (state->buflnum > state->lnumlast)
	أرجع QF_پای_ټکی_د_ننوت;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    إذا (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	إذا (state->linebuf == عدم)
	    أرجع QF_NOMEM;
    }
    وإلا
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    أرجع QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    ساكن صحيح
qf_get_بل_file_کرښه(qfstate_T *state)
{
    صحيح	    discard;
    صحيح	    growbuflen;

    إذا (fgets((حرف *)IObuff, IOSIZE, state->fd) == عدم)
	أرجع QF_پای_ټکی_د_ننوت;

    discard = FALSE;
    state->linelen = (صحيح)STRLEN(IObuff);
    إذا (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	إذا (state->growbuf == عدم)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    إذا (state->growbuf == عدم)
		أرجع QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	حافظه_نقل(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	لكل (;;)
	{
	    char_u	*p;

	    إذا (fgets((حرف *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == عدم)
		اكسر;
	    state->linelen = (صحيح)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    إذا ((state->growbuf)[growbuflen - 1] == '\n')
		اكسر;
	    إذا (state->growbufsiz == کرښه_MAXLEN)
	    {
		discard = TRUE;
		اكسر;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < کرښه_MAXLEN
		? 2 * state->growbufsiz : کرښه_MAXLEN;
	    إذا ((p = vim_realloc(state->growbuf, state->growbufsiz)) == عدم)
		أرجع QF_NOMEM;
	    state->growbuf = p;
	}

	طالما (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    إذا (fgets((حرف *)IObuff, IOSIZE, state->fd) == عدم
		    || (صحيح)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		اكسر;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    وإلا
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    إذا (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*کرښه;

	کرښه = string_convert(&state->vc, state->linebuf, &state->linelen);
	إذا (کرښه != عدم)
	{
	    إذا (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, کرښه);
		vim_free(کرښه);
	    }
	    وإلا
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = کرښه;
		state->growbufsiz = state->linelen < کرښه_MAXLEN
						? state->linelen : کرښه_MAXLEN;
	    }
	}
    }

    أرجع QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    ساكن صحيح
qf_get_nextline(qfstate_T *state)
{
    صحيح حالت = QF_FAIL;

    إذا (state->fd == عدم)
    {
	إذا (state->tv != عدم)
	{
	    إذا (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		حالت = qf_get_بل_str_کرښه(state);
	    وإلا إذا (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		حالت = qf_get_بل_list_کرښه(state);
	}
	وإلا
	    // Get the next line from the supplied buffer
	    حالت = qf_get_بل_buf_کرښه(state);
    }
    وإلا
	// Get the next line from the supplied file
	حالت = qf_get_بل_file_کرښه(state);

    إذا (حالت != QF_OK)
	أرجع حالت;

    // remove newline/CR from the line
    إذا (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	إذا (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    أرجع QF_OK;
}

تعريفنوع بنية {
    char_u	*namebuf;
    صحيح		bnr;
    char_u	*module;
    char_u	*errmsg;
    صحيح		errmsglen;
    اوږد	lnum;
    اوږد	پای_ټکی_lnum;
    صحيح		ستون;
    صحيح		پای_ټکی_ستون;
    char_u	use_viscol;
    char_u	*نمونه;
    صحيح		enr;
    صحيح		type;
    typval_T	*user_معلومات;
    صحيح		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    ساكن صحيح
qf_parse_fmt_f(regmatch_T *rmp, صحيح midx, qffields_T *fields, صحيح مختاړی)
{
    صحيح c;

    إذا (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	أرجع QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    إذا (vim_strchr((char_u *)"OPQ", مختاړی) != عدم
	    && mch_getperm(fields->namebuf) == -1)
	أرجع QF_FAIL;

    أرجع QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    ساكن صحيح
qf_parse_fmt_b(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    إذا (rmp->startp[midx] == عدم)
	أرجع QF_FAIL;
    صحيح bnr = (صحيح)atol((حرف *)rmp->startp[midx]);
    إذا (buflist_findnr(bnr) == عدم)
	أرجع QF_FAIL;
    fields->bnr = bnr;
    أرجع QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    ساكن صحيح
qf_parse_fmt_n(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    إذا (rmp->startp[midx] == عدم)
	أرجع QF_FAIL;
    fields->enr = (صحيح)atol((حرف *)rmp->startp[midx]);
    أرجع QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    ساكن صحيح
qf_parse_fmt_l(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    إذا (rmp->startp[midx] == عدم)
	أرجع QF_FAIL;
    fields->lnum = atol((حرف *)rmp->startp[midx]);
    أرجع QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    ساكن صحيح
qf_parse_fmt_e(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    إذا (rmp->startp[midx] == عدم)
	أرجع QF_FAIL;
    fields->پای_ټکی_lnum = atol((حرف *)rmp->startp[midx]);
    أرجع QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ساكن صحيح
qf_parse_fmt_c(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    إذا (rmp->startp[midx] == عدم)
	أرجع QF_FAIL;
    fields->ستون = (صحيح)atol((حرف *)rmp->startp[midx]);
    أرجع QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    ساكن صحيح
qf_parse_fmt_k(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    إذا (rmp->startp[midx] == عدم)
	أرجع QF_FAIL;
    fields->پای_ټکی_ستون = (صحيح)atol((حرف *)rmp->startp[midx]);
    أرجع QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    ساكن صحيح
qf_parse_fmt_t(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    إذا (rmp->startp[midx] == عدم)
	أرجع QF_FAIL;
    fields->type = *rmp->startp[midx];
    أرجع QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    ساكن صحيح
copy_nonerror_کرښه(char_u *linebuf, صحيح linelen, qffields_T *fields)
{
    char_u	*p;

    إذا (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	إذا ((p = vim_realloc(fields->errmsg, linelen + 1)) == عدم)
	    أرجع QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    أرجع QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    ساكن صحيح
qf_parse_fmt_m(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    char_u	*p;
    صحيح		len;

    إذا (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	أرجع QF_FAIL;
    len = (صحيح)(rmp->endp[midx] - rmp->startp[midx]);
    إذا (len >= fields->errmsglen)
    {
	// len + null terminator
	إذا ((p = vim_realloc(fields->errmsg, len + 1)) == عدم)
	    أرجع QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    أرجع QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    ساكن صحيح
qf_parse_fmt_r(regmatch_T *rmp, صحيح midx, char_u **لکۍ)
{
    إذا (rmp->startp[midx] == عدم)
	أرجع QF_FAIL;
    *لکۍ = rmp->startp[midx];
    أرجع QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ساكن صحيح
qf_parse_fmt_p(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    char_u	*سمون_ptr;

    إذا (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	أرجع QF_FAIL;
    fields->ستون = 0;
    لكل (سمون_ptr = rmp->startp[midx]; سمون_ptr != rmp->endp[midx];
								++سمون_ptr)
    {
	++fields->ستون;
	إذا (*سمون_ptr == TAB)
	{
	    fields->ستون += 7;
	    fields->ستون -= fields->ستون % 8;
	}
    }
    ++fields->ستون;
    fields->use_viscol = TRUE;
    أرجع QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ساكن صحيح
qf_parse_fmt_v(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    إذا (rmp->startp[midx] == عدم)
	أرجع QF_FAIL;
    fields->ستون = (صحيح)atol((حرف *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    أرجع QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    ساكن صحيح
qf_parse_fmt_s(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    صحيح		len;

    إذا (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	أرجع QF_FAIL;
    len = (صحيح)(rmp->endp[midx] - rmp->startp[midx]);
    إذا (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->نمونه, "^\\V");
    STRNCAT(fields->نمونه, rmp->startp[midx], len);
    fields->نمونه[len + 3] = '\\';
    fields->نمونه[len + 4] = '$';
    fields->نمونه[len + 5] = NUL;
    أرجع QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    ساكن صحيح
qf_parse_fmt_o(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    صحيح		len;

    إذا (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	أرجع QF_FAIL;
    len = (صحيح)(rmp->endp[midx] - rmp->startp[midx]);
    إذا (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    أرجع QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
ساكن صحيح (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, صحيح, qffields_T *) =
{
    عدم, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    عدم, // %r
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
    ساكن صحيح
qf_parse_سمون(
	char_u		*linebuf,
	صحيح		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	صحيح		qf_multiline,
	صحيح		qf_multiscan,
	char_u		**لکۍ)
{
    صحيح		idx = fmt_ptr->مختاړی;
    صحيح		i;
    صحيح		midx;
    صحيح		حالت;

    إذا ((idx == 'C' || idx == 'Z') && !qf_multiline)
	أرجع QF_FAIL;
    إذا (vim_strchr((char_u *)"EWIN", idx) != عدم)
	fields->type = idx;
    وإلا
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    لكل (i = 0; i < FMT_PATTERNS; i++)
    {
	حالت = QF_OK;
	midx = (صحيح)fmt_ptr->addr[i];
	إذا (i == 0 && midx > 0)				// %f
	    حالت = qf_parse_fmt_f(regmatch, midx, fields, idx);
	وإلا إذا (i == FMT_نمونه_M)
	{
	    إذا (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		حالت = copy_nonerror_کرښه(linebuf, linelen, fields);
	    وإلا إذا (midx > 0)				// %m
		حالت = qf_parse_fmt_m(regmatch, midx, fields);
	}
	وإلا إذا (i == FMT_نمونه_R && midx > 0)	// %r
	    حالت = qf_parse_fmt_r(regmatch, midx, لکۍ);
	وإلا إذا (midx > 0)				// others
	    حالت = (qf_parse_fmt[i])(regmatch, midx, fields);

	إذا (حالت != QF_OK)
	    أرجع حالت;
    }

    أرجع QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    ساكن صحيح
qf_parse_get_fields(
	char_u		*linebuf,
	صحيح		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	صحيح		qf_multiline,
	صحيح		qf_multiscan,
	char_u		**لکۍ)
{
    regmatch_T	regmatch;
    صحيح		حالت = QF_FAIL;
    صحيح		r;

    إذا (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->مختاړی) == عدم)
	أرجع QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->نمونه[0] = NUL;
    إذا (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->پای_ټکی_lnum = 0;
    fields->ستون = 0;
    fields->پای_ټکی_ستون = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *لکۍ = عدم;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    إذا (r)
	حالت = qf_parse_سمون(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, لکۍ);

    أرجع حالت;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    ساكن صحيح
qf_parse_dir_pfx(صحيح idx, qffields_T *fields, qf_list_T *qfl)
{
    إذا (idx == 'D')				// enter directory
    {
	إذا (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_تشه_directory_name));
	    أرجع QF_FAIL;
	}
	qfl->qf_directory =
	    qf_ټېل_dir(fields->namebuf, &qfl->qf_dir_ډېری, FALSE);
	إذا (qfl->qf_directory == عدم)
	    أرجع QF_FAIL;
    }
    وإلا إذا (idx == 'X')			// leave directory
	qfl->qf_directory = qf_راوباسه_dir(&qfl->qf_dir_ډېری);

    أرجع QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    ساكن صحيح
qf_parse_file_pfx(
	صحيح idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *لکۍ)
{
    fields->valid = FALSE;
    إذا (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	إذا (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_ټېل_dir(fields->namebuf, &qfl->qf_file_ډېری, TRUE);
	وإلا إذا (idx == 'Q')
	    qfl->qf_currfile = qf_راوباسه_dir(&qfl->qf_file_ډېری);
	*fields->namebuf = NUL;
	إذا (لکۍ && *لکۍ)
	{
	    qfl->qf_multiscan = TRUE;
	    أرجع QF_MULTISCAN;
	}
    }

    أرجع QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    ساكن صحيح
qf_parse_کرښه_nomatch(char_u *linebuf, صحيح linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    أرجع copy_nonerror_کرښه(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    ساكن صحيح
qf_parse_multiline_pfx(
	صحيح idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    صحيح			len;

    إذا (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_وروستی;

	إذا (qfprev == عدم)
	    أرجع QF_FAIL;
	إذا (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (صحيح)STRLEN(qfprev->qf_متن);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    إذا (ptr == عدم)
		أرجع QF_FAIL;
	    STRCPY(ptr, qfprev->qf_متن);
	    vim_free(qfprev->qf_متن);
	    qfprev->qf_متن = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	إذا (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	إذا (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	إذا (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	إذا (!qfprev->qf_پای_ټکی_lnum)
	    qfprev->qf_پای_ټکی_lnum = fields->پای_ټکی_lnum;
	إذا (!qfprev->qf_ستون)
	{
	    qfprev->qf_ستون = fields->ستون;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	إذا (!qfprev->qf_پای_ټکی_ستون)
	    qfprev->qf_پای_ټکی_ستون = fields->پای_ټکی_ستون;
	إذا (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != عدم
		    ? fields->namebuf
		    : qfl->qf_currfile != عدم && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    إذا (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    کرښه_breakcheck();

    أرجع QF_IGNORE_کرښه;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    ساكن صحيح
qf_parse_کرښه(
	qf_list_T	*qfl,
	char_u		*linebuf,
	صحيح		linelen,
	efm_T		*fmt_لومړی,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    صحيح			idx = 0;
    char_u		*لکۍ = عدم;
    صحيح			حالت;

restofline:
    // If there was no %> item start at the first pattern
    إذا (fmt_پیل == عدم)
	fmt_ptr = fmt_لومړی;
    وإلا
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_پیل;
	fmt_پیل = عدم;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    لكل ( ; fmt_ptr != عدم; fmt_ptr = fmt_ptr->بل)
    {
	idx = fmt_ptr->مختاړی;
	حالت = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &لکۍ);
	إذا (حالت == QF_NOMEM)
	    أرجع حالت;
	إذا (حالت == QF_OK)
	    اكسر;
    }
    qfl->qf_multiscan = FALSE;

    إذا (fmt_ptr == عدم || idx == 'D' || idx == 'X')
    {
	إذا (fmt_ptr != عدم)
	{
	    // 'D' and 'X' directory specifiers
	    حالت = qf_parse_dir_pfx(idx, fields, qfl);
	    إذا (حالت != QF_OK)
		أرجع حالت;
	}

	حالت = qf_parse_کرښه_nomatch(linebuf, linelen, fields);
	إذا (حالت != QF_OK)
	    أرجع حالت;

	إذا (fmt_ptr == عدم)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    وإلا إذا (fmt_ptr != عدم)
    {
	// honor %> item
	إذا (fmt_ptr->conthere)
	    fmt_پیل = fmt_ptr;

	إذا (vim_strchr((char_u *)"AEWIN", idx) != عدم)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	وإلا إذا (vim_strchr((char_u *)"CZ", idx) != عدم)
	{				// continuation of multi-line msg
	    حالت = qf_parse_multiline_pfx(idx, qfl, fields);
	    إذا (حالت != QF_OK)
		أرجع حالت;
	}
	وإلا إذا (vim_strchr((char_u *)"OPQ", idx) != عدم)
	{				// global file names
	    حالت = qf_parse_file_pfx(idx, fields, qfl, لکۍ);
	    إذا (حالت == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(لکۍ);
		صحيح new_linelen = (صحيح)STRLEN(s);
		إذا (new_linelen >= linelen)
		    أرجع QF_IGNORE_کرښه;
		linebuf = s;
		linelen = new_linelen;
		اذهب restofline;
	    }
	}
	إذا (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    إذا (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    أرجع QF_IGNORE_کرښه;
	}
    }

    أرجع QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    ساكن صحيح
qf_ډېری_تشه(qf_info_T *qi)
{
    أرجع qi == عدم || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    ساكن صحيح
qf_list_تشه(qf_list_T *qfl)
{
    أرجع qfl == عدم || qfl->qf_شمېر <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    ساكن صحيح
qf_list_has_valid_entries(qf_list_T *qfl)
{
    أرجع !qf_list_تشه(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    ساكن qf_list_T *
qf_get_list(qf_info_T *qi, صحيح idx)
{
    أرجع &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    ساكن صحيح
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->نمونه = alloc_id(CMDBUFFSIZE + 1, aid_qf_نمونه);
    إذا (pfields->namebuf == عدم || pfields->errmsg == عدم
		|| pfields->نمونه == عدم || pfields->module == عدم)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    ساكن فراغ
qf_free_fields(qffields_T *pfields)
{
    vim_free(pfields->namebuf);
    vim_free(pfields->module);
    vim_free(pfields->errmsg);
    vim_free(pfields->نمونه);
}

/*
 * Setup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ساكن صحيح
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
    إذا (enc != عدم && *enc != NUL)
	convert_تیاری(&pstate->vc, enc, p_enc);

    إذا (efile != عدم && (pstate->fd = mch_fopen((حرف *)efile, "r")) == عدم)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	أرجع FAIL;
    }

    إذا (tv != عدم)
    {
	إذا (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	وإلا إذا (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_لومړی;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    أرجع OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ساكن فراغ
qf_cleanup_state(qfstate_T *pstate)
{
    إذا (pstate->fd != عدم)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    إذا (pstate->vc.vc_type != CONV_NONE)
	convert_تیاری(&pstate->vc, عدم, عدم);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    ساكن صحيح
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_لومړی,
	qfstate_T	*state,
	qffields_T	*fields)
{
    صحيح		    حالت;

    // Get the next line from a file/buffer/list/string
    حالت = qf_get_nextline(state);
    إذا (حالت != QF_OK)
	أرجع حالت;

    حالت = qf_parse_کرښه(qfl, state->linebuf, state->linelen,
	    fmt_لومړی, fields);
    إذا (حالت != QF_OK)
	أرجع حالت;

    أرجع qf_جمع_کړه_ثبت(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != عدم)
		? fields->namebuf
		: ((qfl->qf_currfile != عدم && fields->valid)
		    ? qfl->qf_currfile : (char_u *)عدم),
		fields->module,
		fields->bnr,
		fields->errmsg,
		fields->lnum,
		fields->پای_ټکی_lnum,
		fields->ستون,
		fields->پای_ټکی_ستون,
		fields->use_viscol,
		fields->نمونه,
		fields->enr,
		fields->type,
		fields->user_معلومات,
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
    ساكن صحيح
qf_init_ext(
    qf_info_T	    *qi,
    صحيح		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    صحيح		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_وروستی = عدم;
    صحيح		    adding = FALSE;
    ساكن efm_T    *fmt_لومړی = عدم;
    char_u	    *efm;
    ساكن char_u   *وروستی_efm = عدم;
    صحيح		    retval = -1;	// default: return error flag
    صحيح		    حالت;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_وروستی_bufname);

    CLEAR_ډګر(state);
    CLEAR_ډګر(fields);
    إذا ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_تیاری_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	اذهب qf_init_پای_ټکی;

    إذا (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    وإلا
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	إذا (!qf_list_تشه(qfl))
	    old_وروستی = qfl->qf_وروستی;
    }

    // Use the local value of 'errorformat' if it's set.
    إذا (errorformat == p_efm && tv == عدم && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    وإلا
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    إذا (وروستی_efm == عدم || (STRCMP(وروستی_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(وروستی_efm);
	free_efm_list(&fmt_لومړی);

	// parse the current 'efm'
	fmt_لومړی = parse_efm_option(efm);
	إذا (fmt_لومړی != عدم)
	    وروستی_efm = vim_strsave(efm);
    }

    إذا (fmt_لومړی == عدم)	// nothing found
	اذهب error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    طالما (!got_int)
    {
	حالت = qf_init_process_nextline(qfl, fmt_لومړی, &state, &fields);
	إذا (حالت == QF_NOMEM)		// memory alloc failure
	    اذهب qf_init_پای_ټکی;
	إذا (حالت == QF_پای_ټکی_د_ننوت)	// end of input
	    اكسر;
	إذا (حالت == QF_FAIL)
	    اذهب error2;

	کرښه_breakcheck();
    }
    إذا (state.fd == عدم || !ferror(state.fd))
    {
	إذا (qfl->qf_شاخص == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_پیل;
	    qfl->qf_شاخص = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	وإلا
	{
	    qfl->qf_nonevalid = FALSE;
	    إذا (qfl->qf_ptr == عدم)
		qfl->qf_ptr = qfl->qf_پیل;
	}
	// return number of matches
	retval = qfl->qf_شمېر;
	اذهب qf_init_پای_ټکی;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    إذا (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	إذا (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_پای_ټکی:
    إذا (qf_idx == qi->qf_curlist)
	qf_update_بفر(qi, old_وروستی);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    أرجع retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    صحيح
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	صحيح	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    إذا (wp != عدم)
	qi = ll_get_or_alloc_list(wp);
    إذا (qi == عدم)
	أرجع FAIL;

    أرجع qf_init_ext(qi, qi->qf_curlist, efile, curbuf, عدم, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    ساكن فراغ
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    إذا (title == عدم)
	أرجع;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    إذا (p != عدم)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    ساكن char_u *
qf_cmdtitle(char_u *cmd)
{
    ساكن char_u qftitle_str[IOSIZE];

    vim_snprintf((حرف *)qftitle_str, IOSIZE, ":%s", (حرف *)cmd);
    أرجع qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    ساكن qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    أرجع qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    ساكن فراغ
qf_راوباسه_ډېری(qf_info_T *qi, صحيح adjust)
{
    صحيح i;
    qf_free(&qi->qf_lists[0]);
    لكل (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, حجم(*qi->qf_lists));

    إذا (adjust)
    {
	qi->qf_listcount--;
	إذا (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	وإلا
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    ساكن فراغ
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    طالما (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    إذا (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_راوباسه_ډېری(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    وإلا
	qi->qf_curlist = qi->qf_listcount++;

    qfl = qf_get_curlist(qi);
    CLEAR_اشاره_ګر(qfl);
    qf_store_title(qfl, qf_title);
    qfl->qfl_type = qi->qfl_type;
    qfl->qf_id = ++وروستی_qf_id;
    qfl->qf_has_user_معلومات = FALSE;
}

/*
 * Queue location list stack delete request.
 */
    ساكن فراغ
locstack_کتار_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    إذا (q == عدم)
	أرجع;

    q->qi = qi;
    q->بل = qf_delq_سر;
    qf_delq_سر = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    صحيح
qf_ډېری_get_bufnr(فراغ)
{
    إذا (ql_info == عدم)
	أرجع INVALID_QFBUFNR;
    أرجع ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    ساكن فراغ
wipe_qf_بفر(qf_info_T *qi)
{
    buf_T	*qfbuf;

    إذا (qi->qf_bufnr == INVALID_QFBUFNR)
	أرجع;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    إذا (qfbuf != عدم && qfbuf->b_nwindows == 0)
    {
	صحيح buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	إذا (curwin->w_بفر == عدم)
	{
	    curwin->w_بفر = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_بفر(عدم, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	إذا (buf_was_null)
	    curwin->w_بفر = عدم;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    ساكن فراغ
qf_free_list_ډېری_توکي(qf_info_T *qi)
{
    لكل (صحيح i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    ساكن فراغ
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_ډېری_توکي(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    ساكن فراغ
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    إذا (qi == عدم)
	أرجع;
    *pqi = عدم;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    إذا (quickfix_busy > 0)
    {
	locstack_کتار_delreq(qi);
	أرجع;
    }

    qi->qf_refcount--;
    إذا (qi->qf_refcount < 1)
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
    فراغ
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    إذا (wp != عدم)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    وإلا إذا (qi != عدم)
	qf_free_list_ډېری_توکي(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    ساكن فراغ
incr_quickfix_busy(فراغ)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    ساكن فراغ
decr_quickfix_busy(فراغ)
{
    إذا (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	طالما (qf_delq_سر != عدم)
	{
	    qf_delq_T	*q = qf_delq_سر;

	    qf_delq_سر = q->بل;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_دننی_ERROR
    إذا (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# إذا defined(EXITFREE)
    فراغ
check_quickfix_busy(فراغ)
{
    إذا (quickfix_busy != 0)
    {
	semsg("quickfix_busy not zero on exit: %ld", (اوږد)quickfix_busy);
#  ifdef ABORT_ON_دننی_ERROR
	abort();
#  endif
    }
}
# endif

/*
 * Add an entry to the end of the list of errors.
 * Returns QF_OK on success or QF_FAIL on a memory allocation failure.
 */
    ساكن صحيح
qf_جمع_کړه_ثبت(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    صحيح		bufnum,		// buffer number or zero
    char_u	*mesg,		//پیغام
    اوږد	lnum,		// line number
    اوږد	پای_ټکی_lnum,	// line number for end
    صحيح		ستون,		//ستنه
    صحيح		پای_ټکی_ستون,	// column for end
    صحيح		vis_ستون,	// using visual column
    char_u	*نمونه,	// search pattern
    صحيح		nr,		// error number
    صحيح		type,		// type character
    typval_T	*user_معلومات,     // custom user data or NULL
    صحيح		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = عدم;
    char_u	*p = عدم;

    إذا ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == عدم)
	أرجع QF_FAIL;
    إذا (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	إذا (buf != عدم)
	    buf->b_has_qf_ثبت |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ثبت : BUF_HAS_LL_ثبت;
    }
    وإلا
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    إذا (fname != عدم)
	fullname = fix_fname(fname);
    qfp->qf_fname = عدم;
    إذا (buf != عدم &&
	buf->b_ffname != عدم && fullname != عدم)
    {
	إذا (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    إذا (p != عدم)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    إذا ((qfp->qf_متن = vim_strsave(mesg)) == عدم)
    {
	vim_free(qfp);
	أرجع QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_پای_ټکی_lnum = پای_ټکی_lnum;
    qfp->qf_ستون = ستون;
    qfp->qf_پای_ټکی_ستون = پای_ټکی_ستون;
    qfp->qf_viscol = vis_ستون;
    إذا (user_معلومات == عدم || user_معلومات->v_type == VAR_UNKNOWN)
	qfp->qf_user_معلومات.v_type = VAR_UNKNOWN;
    وإلا
    {
	copy_tv(user_معلومات, &qfp->qf_user_معلومات);
	qfl->qf_has_user_معلومات = TRUE;
    }
    إذا (نمونه == عدم || *نمونه == NUL)
	qfp->qf_نمونه = عدم;
    وإلا إذا ((qfp->qf_نمونه = vim_strsave(نمونه)) == عدم)
    {
	vim_free(qfp->qf_متن);
	vim_free(qfp);
	أرجع QF_FAIL;
    }
    إذا (module == عدم || *module == NUL)
	qfp->qf_module = عدم;
    وإلا إذا ((qfp->qf_module = vim_strsave(module)) == عدم)
    {
	vim_free(qfp->qf_متن);
	vim_free(qfp->qf_نمونه);
	vim_free(qfp);
	أرجع QF_FAIL;
    }
    qfp->qf_nr = nr;
    إذا (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_وروستی;
    إذا (qf_list_تشه(qfl))		// first element in the list
    {
	qfl->qf_پیل = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_شاخص = 0;
	qfp->qf_prev = عدم;
    }
    وإلا
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_بل = qfp;
    }
    qfp->qf_بل = عدم;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_شمېر;
    إذا (qfl->qf_شاخص == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_شاخص = qfl->qf_شمېر;
	qfl->qf_ptr = qfp;
    }

    أرجع QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    صحيح
qf_resize_ډېری(صحيح n)
{
    إذا (ql_info == عدم)
	أرجع FAIL;

    إذا (qf_resize_ډېری_بنسټ(ql_info, n) == FAIL)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    صحيح
ll_resize_ډېری(win_T *wp, صحيح n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    إذا (IS_LL_کړکۍ(wp))
	qf_sync_llw_منزل_win(wp);
    وإلا
	qf_sync_win_منزل_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    إذا (qi == عدم)
	أرجع FAIL;

    إذا (qf_resize_ډېری_بنسټ(qi, n) == FAIL)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    ساكن صحيح
qf_resize_ډېری_بنسټ(qf_info_T *qi, صحيح n)
{
    qf_list_T *new;
    صحيح amount_منزل_rm = 0, i;

    إذا (qi == عدم)
	أرجع FAIL;

    size_t lsz = حجم(*qi->qf_lists);

    إذا (n == qi->qf_maxcount)
	أرجع OK;
    وإلا إذا (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_منزل_rm = qi->qf_listcount - n;

	لكل (i = 0; i < amount_منزل_rm; i++)
	    qf_راوباسه_ډېری(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    إذا (new == عدم)
	أرجع FAIL;

    // fill with zeroes any newly allocated memory
    إذا (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_بفر(qi, عدم);

    أرجع OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   فراغ
qf_init_ډېری(فراغ)
{
    ql_info = qf_alloc_ډېری(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    ساكن فراغ
qf_sync_llw_منزل_win(win_T *llw)
{
    win_T *wp = qf_پیدا_کړه_win_with_loclist(llw->w_llist_ref);

    إذا (wp != عدم)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    ساكن فراغ
qf_sync_win_منزل_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    إذا (llw != عدم)
	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_llist_ref == llw && bt_quickfix(wp->w_بفر))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		أرجع;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    ساكن qf_info_T *
qf_alloc_ډېری(qfltype_T qfltype, صحيح n)
{
    qf_info_T *qi;

    إذا (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    وإلا
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	إذا (qi == عدم)
	    أرجع عدم;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_ډېری(n);
    إذا (qi->qf_lists == عدم)
    {
	إذا (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	أرجع عدم;
    }
    qi->qf_maxcount = n;

    أرجع qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    ساكن qf_list_T *
qf_alloc_list_ډېری(صحيح n)
{
    أرجع ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    ساكن qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    إذا (IS_LL_کړکۍ(wp))
	// For a location list window, use the referenced location list
	أرجع wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    إذا (wp->w_llist == عدم)
	// new location list
	wp->w_llist = qf_alloc_ډېری(QFLT_LOCATION, wp->w_p_lhi);

    أرجع wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    ساكن qf_info_T *
qf_cmd_get_ډېری(exarg_T *eap, صحيح print_emsg)
{
    qf_info_T	*qi = ql_info;

    إذا (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	إذا (qi == عدم)
	{
	    إذا (print_emsg)
		emsg(_(e_no_location_list));
	    أرجع عدم;
	}
    }
    إذا (qi == عدم && print_emsg)
	emsg(_(e_no_quickfix_ډېری));

    أرجع qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    ساكن qf_info_T *
qf_cmd_get_or_alloc_ډېری(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    إذا (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	إذا (qi == عدم)
	    أرجع عدم;
	*pwinp = curwin;
    }

    أرجع qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    ساكن صحيح
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *منزل_qfl)
{
    صحيح		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_توکي(from_qfl, from_qfp, i)
    {
	إذا (qf_جمع_کړه_ثبت(منزل_qfl,
		    عدم,
		    عدم,
		    from_qfp->qf_module,
		    0,
		    from_qfp->qf_متن,
		    from_qfp->qf_lnum,
		    from_qfp->qf_پای_ټکی_lnum,
		    from_qfp->qf_ستون,
		    from_qfp->qf_پای_ټکی_ستون,
		    from_qfp->qf_viscol,
		    from_qfp->qf_نمونه,
		    from_qfp->qf_nr,
		    0,
		    &from_qfp->qf_user_معلومات,
		    from_qfp->qf_valid) == QF_FAIL)
	    أرجع FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = منزل_qfl->qf_وروستی;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	إذا (from_qfl->qf_ptr == from_qfp)
	    منزل_qfl->qf_ptr = prevp;		// current location
    }

    أرجع OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    ساكن صحيح
copy_loclist(qf_list_T *from_qfl, qf_list_T *منزل_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    منزل_qfl->qfl_type = from_qfl->qfl_type;
    منزل_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    منزل_qfl->qf_has_user_معلومات = from_qfl->qf_has_user_معلومات;
    منزل_qfl->qf_شمېر = 0;
    منزل_qfl->qf_شاخص = 0;
    منزل_qfl->qf_پیل = عدم;
    منزل_qfl->qf_وروستی = عدم;
    منزل_qfl->qf_ptr = عدم;
    إذا (from_qfl->qf_title != عدم)
	منزل_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    وإلا
	منزل_qfl->qf_title = عدم;
    إذا (from_qfl->qf_ctx != عدم)
    {
	منزل_qfl->qf_ctx = alloc_tv();
	إذا (منزل_qfl->qf_ctx != عدم)
	    copy_tv(from_qfl->qf_ctx, منزل_qfl->qf_ctx);
    }
    وإلا
	منزل_qfl->qf_ctx = عدم;
    إذا (from_qfl->qf_qftf_cb.cb_name != عدم)
	copy_callback(&منزل_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    وإلا
	منزل_qfl->qf_qftf_cb.cb_name = عدم;

    إذا (from_qfl->qf_شمېر)
	إذا (copy_loclist_entries(from_qfl, منزل_qfl) == FAIL)
	    أرجع FAIL;

    منزل_qfl->qf_شاخص = from_qfl->qf_شاخص;	// current index in the list

    // Assign a new ID for the location list
    منزل_qfl->qf_id = ++وروستی_qf_id;
    منزل_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    إذا (منزل_qfl->qf_nonevalid)
    {
	منزل_qfl->qf_ptr = منزل_qfl->qf_پیل;
	منزل_qfl->qf_شاخص = 1;
    }

    أرجع OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    فراغ
copy_loclist_ډېری(win_T *from, win_T *منزل)
{
    qf_info_T	*qi;
    صحيح		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    إذا (IS_LL_کړکۍ(from))
	qi = from->w_llist_ref;
    وإلا
	qi = from->w_llist;

    إذا (qi == عدم)		    // no location list to copy
	أرجع;

    // allocate a new location list, set size of stack to 'from' window value
    إذا ((منزل->w_llist = qf_alloc_ډېری(QFLT_LOCATION, from->w_p_lhi)) == عدم)
	أرجع;
    وإلا
	// set 'to' lhi to reflect new value
	منزل->w_p_lhi = منزل->w_llist->qf_maxcount;

    منزل->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    لكل (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	منزل->w_llist->qf_curlist = idx;

	إذا (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(منزل->w_llist, idx)) == FAIL)
	{
	    qf_free_all(منزل);
	    أرجع;
	}
    }

    منزل->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    ساكن صحيح
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {عدم, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    إذا (fname == عدم || *fname == NUL)		// no file name
	أرجع 0;

# ifdef VMS
    vms_remove_نسخه(fname);
# endif
# ifdef BACKSLASH_دننه_FILENAME
    إذا (directory != عدم)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    إذا (directory != عدم && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != عدم)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	إذا (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    إذا (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    وإلا
	    {
		ptr.اوږدتیا = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    إذا (ptr.string == عدم)
		أرجع 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.اوږدتیا = ptr.اوږدتیا;
    }
    وإلا
    {
	bufname.string = fname;
	bufname.اوږدتیا = fname_len;
    }

    إذا (qf_وروستی_bufname != عدم && STRCMP(bufname.string, qf_وروستی_bufname) == 0
	    && bufref_valid(&qf_وروستی_bufref))
    {
	buf = qf_وروستی_bufref.br_buf;
	vim_free(ptr.string);
    }
    وإلا
    {
	vim_free(qf_وروستی_bufname);
	buf = buflist_new(bufname.string, عدم, (linenr_T)0, BLN_NOOPT);
	إذا (bufname.string == ptr.string)
	    qf_وروستی_bufname = bufname.string;
	وإلا
	    qf_وروستی_bufname = vim_strnsave(bufname.string, bufname.اوږدتیا);
	سېټ_bufref(&qf_وروستی_bufref, buf);
    }
    إذا (buf == عدم)
	أرجع 0;

    buf->b_has_qf_ثبت =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ثبت : BUF_HAS_LL_ثبت;
    أرجع buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    ساكن char_u *
qf_ټېل_dir(char_u *dirbuf, بنية dir_ډېری_T **stackptr, صحيح is_file_ډېری)
{
    بنية dir_ډېری_T  *ds_new;
    بنية dir_ډېری_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(بنية dir_ډېری_T, aid_qf_dirstack);
    إذا (ds_new == عدم)
	أرجع عدم;

    // push the new element onto the stack
    ds_new->بل = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    إذا (vim_isAbsName(dirbuf)
	    || (*stackptr)->بل == عدم
	    || is_file_ډېری)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    وإلا
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->بل;
	(*stackptr)->dirname = عدم;
	طالما (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    إذا (dirname.string == عدم)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->بل;
		vim_free(ds_ptr);

		أرجع عدم;
	    }
	    إذا (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		اكسر;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->بل;
	}

	// clean up all dirs we already left
	طالما ((*stackptr)->بل != ds_new)
	{
	    ds_ptr = (*stackptr)->بل;
	    (*stackptr)->بل = (*stackptr)->بل->بل;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	إذا (ds_new == عدم)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    إذا ((*stackptr)->dirname != عدم)
	أرجع (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->بل;
    vim_free(ds_ptr);

    أرجع عدم;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    ساكن char_u *
qf_راوباسه_dir(بنية dir_ډېری_T **stackptr)
{
    بنية dir_ډېری_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    إذا (*stackptr != عدم)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->بل;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    أرجع *stackptr ? (*stackptr)->dirname : عدم;
}

/*
 * clean up directory stack
 */
    ساكن فراغ
qf_clean_dir_ډېری(بنية dir_ډېری_T **stackptr)
{
    بنية dir_ډېری_T  *ds_ptr;

    طالما ((ds_ptr = *stackptr) != عدم)
    {
	*stackptr = (*stackptr)->بل;
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
    ساكن char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    بنية dir_ډېری_T     *ds_ptr;
    بنية dir_ډېری_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    إذا (qfl->qf_dir_ډېری == عدم)
	أرجع عدم;

    ds_ptr = qfl->qf_dir_ډېری->بل;
    fullname.string = عدم;
    fullname.اوږدتیا = 0;
    filename_len = STRLEN(filename);
    طالما (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	إذا ((fullname.string != عدم) && (mch_getperm(fullname.string) >= 0))
	    اكسر;

	ds_ptr = ds_ptr->بل;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    طالما (qfl->qf_dir_ډېری->بل != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_ډېری->بل;
	qfl->qf_dir_ډېری->بل = qfl->qf_dir_ډېری->بل->بل;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    أرجع ds_ptr == عدم ? عدم : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    ساكن صحيح
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    صحيح		i;

    إذا (wp != عدم)
    {
	إذا (!win_valid(wp))
	    أرجع FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    إذا (qi == عدم)
	أرجع FALSE;

    لكل (i = 0; i < qi->qf_listcount; ++i)
	إذا (qi->qf_lists[i].qf_id == qf_id)
	    أرجع TRUE;

    أرجع FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    ساكن صحيح
is_qf_ثبت_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    صحيح		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_توکي(qfl, qfp, i)
	إذا (qfp == qf_ptr)
	    اكسر;

    إذا (i > qfl->qf_شمېر) // Entry is not found
	أرجع FALSE;

    أرجع TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    ساكن qfline_T *
get_بل_valid_ثبت(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	صحيح		*qf_شاخص,
	صحيح		dir)
{
    صحيح			idx;
    صحيح			old_qf_fnum;

    idx = *qf_شاخص;
    old_qf_fnum = qf_ptr->qf_fnum;

    افعل
    {
	إذا (idx == qfl->qf_شمېر || qf_ptr->qf_بل == عدم)
	    أرجع عدم;
	++idx;
	qf_ptr = qf_ptr->qf_بل;
    } طالما ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_شاخص = idx;
    أرجع qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    ساكن qfline_T *
get_prev_valid_ثبت(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	صحيح		*qf_شاخص,
	صحيح		dir)
{
    صحيح			idx;
    صحيح			old_qf_fnum;

    idx = *qf_شاخص;
    old_qf_fnum = qf_ptr->qf_fnum;

    افعل
    {
	إذا (idx == 1 || qf_ptr->qf_prev == عدم)
	    أرجع عدم;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } طالما ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_شاخص = idx;
    أرجع qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    ساكن qfline_T *
get_nth_valid_ثبت(
	qf_list_T	*qfl,
	صحيح		errornr,
	صحيح		dir,
	صحيح		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    صحيح			qf_idx = qfl->qf_شاخص;
    qfline_T		*prev_qf_ptr;
    صحيح			prev_شاخص;
    حرف		*err = e_no_نور_توکي;

    طالما (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_شاخص = qf_idx;

	إذا (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_بل_valid_ثبت(qfl, qf_ptr, &qf_idx, dir);
	وإلا
	    qf_ptr = get_prev_valid_ثبت(qfl, qf_ptr, &qf_idx, dir);
	إذا (qf_ptr == عدم)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_شاخص;
	    إذا (err != عدم)
	    {
		emsg(_(err));
		أرجع عدم;
	    }
	    اكسر;
	}

	err = عدم;
    }

    *new_qfidx = qf_idx;
    أرجع qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    ساكن qfline_T *
get_nth_ثبت(qf_list_T *qfl, صحيح errornr, صحيح *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    صحيح		qf_idx = qfl->qf_شاخص;

    // New error number is less than the current error number
    طالما (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != عدم)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    طالما (errornr > qf_idx && qf_idx < qfl->qf_شمېر &&
						qf_ptr->qf_بل != عدم)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_بل;
    }

    *new_qfidx = qf_idx;
    أرجع qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    ساكن qfline_T *
qf_get_ثبت(
	qf_list_T	*qfl,
	صحيح		errornr,
	صحيح		dir,
	صحيح		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    صحيح		qfidx = qfl->qf_شاخص;

    إذا (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_ثبت(qfl, errornr, dir, &qfidx);
    وإلا إذا (errornr != 0)	// go to specified number
	qf_ptr = get_nth_ثبت(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    أرجع qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    ساكن win_T *
qf_پیدا_کړه_help_win(فراغ)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	إذا (bt_help(wp->w_بفر))
	    أرجع wp;

    أرجع عدم;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    ساكن فراغ
win_سېټ_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    ساكن صحيح
jump_منزل_help_کړکۍ(qf_info_T *qi, صحيح newwin, صحيح *opened_کړکۍ)
{
    win_T	*wp;
    صحيح		flags;

    إذا (cmdmod.cmod_tab != 0 || newwin)
	wp = عدم;
    وإلا
	wp = qf_پیدا_کړه_help_win();
    إذا (wp != عدم && wp->w_بفر->b_nwindows > 0)
	win_enter(wp, TRUE);
    وإلا
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	إذا (cmdmod.cmod_وېشل == 0 && curwin->w_پلنوالی != Columns
		&& curwin->w_پلنوالی < 80)
	    flags |= WSP_سر;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	إذا (IS_LL_ډېری(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	إذا (win_وېشل(0, flags) == FAIL)
	    أرجع FAIL;

	*opened_کړکۍ = TRUE;

	إذا (curwin->w_لوړوالی < p_hh)
	    win_setheight((صحيح)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	إذا (IS_LL_ډېری(qi) && !newwin)
	    win_سېټ_loclist(curwin, qi);
    }

    إذا (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    أرجع OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    ساكن win_T *
qf_پیدا_کړه_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_llist == ll && !bt_quickfix(wp->w_بفر))
	    أرجع wp;

    أرجع عدم;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    ساكن win_T *
qf_پیدا_کړه_win_with_نورمال_buf(فراغ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (bt_نورمال(wp->w_بفر))
	    أرجع wp;

    أرجع عدم;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    ساكن صحيح
qf_goto_tabwin_with_file(صحيح fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	إذا (wp->w_بفر->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    أرجع TRUE;
	}

    أرجع FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    ساكن صحيح
qf_open_new_file_win(qf_info_T *ll_ref)
{
    صحيح		flags;

    flags = WSP_ABOVE;
    إذا (ll_ref != عدم)
	flags |= WSP_NEWLOC;
    إذا (win_وېشل(0, flags) == FAIL)
	أرجع FAIL;		// not enough room for window
    p_swb = تشه_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    إذا (ll_ref != عدم)
	// The new window should use the location list from the
	// location list window
	win_سېټ_loclist(curwin, ll_ref);
    أرجع OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    ساكن فراغ
qf_goto_win_with_ll_file(win_T *use_win, صحيح qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    إذا (win == عدم)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    إذا (win->w_بفر->b_fnum == qf_fnum)
		اكسر;
	إذا (win == عدم)
	{
	    // Find a previous usable window
	    win = curwin;
	    افعل
	    {
		إذا (bt_نورمال(win->w_بفر))
		    اكسر;
		إذا (win->w_prev == عدم)
		    win = lastwin;	// wrap around the top
		وإلا
		    win = win->w_prev; // go to previous window
	    } طالما (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    إذا (win->w_llist == عدم && ll_ref != عدم)
	win_سېټ_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    ساكن فراغ
qf_goto_win_with_qfl_file(صحيح qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = عدم;
    لكل (;;)
    {
	إذا (win->w_بفر->b_fnum == qf_fnum)
	    اكسر;
	إذا (win->w_prev == عدم)
	    win = lastwin;	// wrap around the top
	وإلا
	    win = win->w_prev;	// go to previous window

	إذا (IS_QF_کړکۍ(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    إذا ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    وإلا إذا (altwin != عدم)
		win = altwin;
	    وإلا إذا (curwin->w_prev != عدم)
		win = curwin->w_prev;
	    وإلا
		win = curwin->w_بل;
	    اكسر;
	}

	// Remember a usable window.
	إذا (altwin == عدم && !win->w_p_pvw && !win->w_p_wfb &&
		bt_نورمال(win->w_بفر))
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
    ساكن صحيح
qf_jump_منزل_usable_کړکۍ(صحيح qf_fnum, صحيح newwin, صحيح *opened_کړکۍ)
{
    win_T	*usable_wp = عدم;
    صحيح		usable_win = FALSE;
    qf_info_T	*ll_ref = عدم;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    إذا (!newwin)
	ll_ref = curwin->w_llist_ref;

    إذا (ll_ref != عدم)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_پیدا_کړه_win_with_loclist(ll_ref);
	إذا (usable_wp != عدم)
	    usable_win = TRUE;
    }

    إذا (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_پیدا_کړه_win_with_نورمال_buf();
	إذا (win != عدم)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    إذا (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    إذا ((ONE_کړکۍ && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	إذا (qf_open_new_file_win(ll_ref) != OK)
	    أرجع FAIL;
	*opened_کړکۍ = TRUE;	// close it when fail
    }
    وإلا
    {
	إذا (curwin->w_llist_ref != عدم)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	وإلا					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    أرجع OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    ساكن صحيح
qf_jump_edit_بفر(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	صحيح		forceit,
	صحيح		prev_winid,
	صحيح		*opened_کړکۍ)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    صحيح		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    صحيح		retval = OK;
    صحيح		old_qf_curlist = qi->qf_curlist;
    صحيح		save_qfid = qfl->qf_id;

    إذا (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	إذا (!can_abandon(curbuf, forceit))
	{
	    no_write_پیغام();
	    أرجع FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, عدم, عدم, عدم, (linenr_T)1,
		ECMD_HIDE + ECMD_سېټ_HELP,
		prev_winid == curwin->w_id ? curwin : عدم);
    }
    وإلا
    {
	صحيح	fnum = qf_ptr->qf_fnum;

	إذا (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    إذا (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_منزل_بفر));
		أرجع FAIL;
	    }

	    إذا (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_بفر))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    إذا (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		إذا (win_وېشل(0, 0) == OK)
		    *opened_کړکۍ = TRUE;

		إذا (curwin->w_p_wfb)
		{
		    // Autocommands set 'winfixbuf' or sent us to another window
		    // with it set, or we failed to split the window.  Give up,
		    // but don't return immediately, as they may have messed
		    // with the list.
		    emsg(_(e_winfixbuf_cannot_go_منزل_بفر));
		    retval = FAIL;
		}
	    }
	}

	إذا (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    إذا (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	إذا (wp == عدم && curwin->w_llist != qi)
	{
	    emsg(_(e_current_کړکۍ_was_closed));
	    *opened_کړکۍ = FALSE;
	    أرجع QF_ABORT;
	}
    }

    إذا (qfl_type == QFLT_QUICKFIX && !qflist_valid(عدم, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_بدل_شوی));
	أرجع QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    إذا (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_ثبت_present(qfl, qf_ptr))
    {
	إذا (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_بدل_شوی));
	وإلا
	    emsg(_(e_current_location_list_was_بدل_شوی));
	أرجع QF_ABORT;
    }

    أرجع retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    ساكن صحيح
qf_screen_ستون_منزل_idx(colnr_T vcol)
{
    char_u	*کرښه = ml_get_curline();
    char_u	*p = کرښه;
    colnr_T	ستون = 0;

    طالما (*p != NUL && ستون < vcol)
    {
	إذا (*p == TAB)
	    ستون += 8 - (ستون % 8);
	وإلا
	    ستون += ptr2cells(p);
	إذا (ستون > vcol)
	    اكسر;
	MB_PTR_ADV(p);
    }
    أرجع (صحيح)(p - کرښه);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    ساكن فراغ
qf_jump_goto_کرښه(
	linenr_T	qf_lnum,
	صحيح		qf_ستون,
	char_u		qf_viscol,
	char_u		*qf_نمونه)
{
    linenr_T		i;

    إذا (qf_نمونه == عدم)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	إذا (i > 0)
	{
	    إذا (i > curbuf->b_ml.ml_کرښه_شمېر)
		i = curbuf->b_ml.ml_کرښه_شمېر;
	    curwin->w_cursor.lnum = i;
	}
	إذا (qf_ستون > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    إذا (qf_viscol == TRUE)
		curwin->w_cursor.ستون = qf_screen_ستون_منزل_idx(qf_ستون - 1);
	    وإلا
		curwin->w_cursor.ستون = qf_ستون - 1;
	    curwin->w_سېټ_curswant = true;
	    check_cursor();
	}
	وإلا
	    beginline(BL_WHITE | BL_FIX);
    }
    وإلا
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	إذا (!do_پلټنه(عدم, '/', '/', qf_نمونه, STRLEN(qf_نمونه), (اوږد)1, پلټنه_KEEP, عدم))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    ساكن فراغ
qf_jump_print_پیغام(
	qf_info_T	*qi,
	صحيح		qf_شاخص,
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
    إذا (!پیغام_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_شاخص,
	    qf_get_curlist(qi)->qf_شمېر,
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
    إذا (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	پیغام_scroll = TRUE;
    وإلا إذا (!پیغام_scrolled && shortmess(SHM_OVERALL))
	پیغام_scroll = FALSE;
    پیغام_attr_keep((حرف *)gap->ga_معلومات, 0, TRUE);
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
    ساكن صحيح
qf_jump_open_کړکۍ(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	صحيح		newwin,
	صحيح		*opened_کړکۍ)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    صحيح		old_changedtick = qfl->qf_changedtick;
    صحيح		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    إذا (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_بفر)
						      || cmdmod.cmod_tab != 0))
	إذا (jump_منزل_help_کړکۍ(qi, newwin, opened_کړکۍ) == FAIL)
	    أرجع FAIL;
    إذا (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_ثبت_present(qfl, qf_ptr))
    {
	إذا (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_بدل_شوی));
	وإلا
	    emsg(_(e_current_location_list_was_بدل_شوی));
	أرجع QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    إذا (bt_quickfix(curbuf) && !*opened_کړکۍ)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	إذا (qf_ptr->qf_fnum == 0)
	    أرجع NOTDONE;

	إذا (qf_jump_منزل_usable_کړکۍ(qf_ptr->qf_fnum, newwin,
						opened_کړکۍ) == FAIL)
	    أرجع FAIL;
    }
    إذا (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_ثبت_present(qfl, qf_ptr))
    {
	إذا (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_بدل_شوی));
	وإلا
	    emsg(_(e_current_location_list_was_بدل_شوی));
	أرجع QF_ABORT;
    }

    أرجع OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    ساكن صحيح
qf_jump_منزل_بفر(
	qf_info_T	*qi,
	صحيح		qf_شاخص,
	qfline_T	*qf_ptr,
	صحيح		forceit,
	صحيح		prev_winid,
	صحيح		*opened_کړکۍ,
	صحيح		openfold,
	صحيح		print_پیغام)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    صحيح		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    إذا (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_بفر(qi, qf_ptr, forceit, prev_winid,
						opened_کړکۍ);
	إذا (retval != OK)
	    أرجع retval;
    }

    // When not switched to another buffer, still need to set pc mark
    إذا (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_کرښه(qf_ptr->qf_lnum, qf_ptr->qf_ستون, qf_ptr->qf_viscol,
	    qf_ptr->qf_نمونه);

# ifdef FEAT_FOLDING
    إذا ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    إذا (print_پیغام)
	qf_jump_print_پیغام(qi, qf_شاخص, qf_ptr, old_curbuf, old_lnum);

    أرجع retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    فراغ
qf_jump(qf_info_T	*qi,
	صحيح		dir,
	صحيح		errornr,
	صحيح		forceit)
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
    ساكن فراغ
qf_jump_newwin(qf_info_T	*qi,
	صحيح		dir,
	صحيح		errornr,
	صحيح		forceit,
	صحيح		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    صحيح			qf_شاخص;
    صحيح			old_qf_شاخص;
    char_u		*old_swb = p_swb;
    بینښه		old_swb_flags = swb_flags;
    صحيح			prev_winid;
    صحيح			opened_کړکۍ = FALSE;
    صحيح			print_پیغام = TRUE;
    صحيح			old_کلی_Typed = کلی_Typed; // getting file may reset it
    صحيح			retval = OK;

    إذا (qi == عدم)
    {
	إذا (ql_info == عدم)
	{
	    emsg(_(e_no_quickfix_ډېری));
	    أرجع;
	}
	qi = ql_info;
    }

    إذا (qf_ډېری_تشه(qi) || qf_list_تشه(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	أرجع;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_شاخص = qfl->qf_شاخص;
    old_qf_شاخص = qf_شاخص;

    qf_ptr = qf_get_ثبت(qfl, errornr, dir, &qf_شاخص);
    إذا (qf_ptr == عدم)
    {
	qf_ptr = old_qf_ptr;
	qf_شاخص = old_qf_شاخص;
	اذهب theend;
    }

    qfl->qf_شاخص = qf_شاخص;
    qfl->qf_ptr = qf_ptr;
    إذا (qf_win_pos_update(qi, old_qf_شاخص))
	// No need to print the error message if it's visible in the error
	//کړکۍ
	print_پیغام = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_کړکۍ(qi, qf_ptr, newwin, &opened_کړکۍ);
    إذا (retval == FAIL)
	اذهب failed;
    إذا (retval == QF_ABORT)
    {
	qi = عدم;
	qf_ptr = عدم;
	اذهب theend;
    }
    إذا (retval == NOTDONE)
	اذهب theend;

    retval = qf_jump_منزل_بفر(qi, qf_شاخص, qf_ptr, forceit, prev_winid,
				  &opened_کړکۍ, old_کلی_Typed, print_پیغام);
    إذا (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = عدم;
	qf_ptr = عدم;
    }

    إذا (retval != OK)
    {
	إذا (opened_کړکۍ)
	    win_close(curwin, TRUE);    // Close opened window
	إذا (qf_ptr != عدم && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_شاخص = old_qf_شاخص;
	}
    }
theend:
    إذا (qi != عدم)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_شاخص = qf_شاخص;
    }
    إذا (p_swb != old_swb && p_swb == تشه_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
ساكن صحيح	qfFileAttr;
ساكن صحيح	qfSepAttr;
ساكن صحيح	qf_کرښه_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    ساكن فراغ
qf_list_ثبت(qfline_T *qfp, صحيح qf_idx, صحيح cursel)
{
    char_u	*fname;
    buf_T	*buf;
    صحيح		filter_ثبت;
    garray_T	*gap;

    fname = عدم;
    إذا (qfp->qf_module != عدم && *qfp->qf_module != NUL)
	vim_snprintf((حرف *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(حرف *)qfp->qf_module);
    وإلا
    {
	إذا (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != عدم)
	{
	    إذا (qfp->qf_fname == عدم)
		fname = buf->b_fname;
	    وإلا
		fname = qfp->qf_fname;
	    إذا (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	إذا (fname == عدم)
	    sprintf((حرف *)IObuff, "%2d", qf_idx);
	وإلا
	    vim_snprintf((حرف *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (حرف *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_ثبت = TRUE;
    إذا (qfp->qf_module != عدم && *qfp->qf_module != NUL)
	filter_ثبت &= پیغام_filtered(qfp->qf_module);
    إذا (filter_ثبت && fname != عدم)
	filter_ثبت &= پیغام_filtered(fname);
    إذا (filter_ثبت && qfp->qf_نمونه != عدم)
	filter_ثبت &= پیغام_filtered(qfp->qf_نمونه);
    إذا (filter_ثبت)
	filter_ثبت &= پیغام_filtered(qfp->qf_متن);
    إذا (filter_ثبت)
	أرجع;

    پیغام_putchar('\n');
    پیغام_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    إذا (qfp->qf_lnum != 0)
	پیغام_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    إذا (qfp->qf_lnum != 0)
	qf_range_متن(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    پیغام_puts_attr((حرف *)gap->ga_معلومات, qf_کرښه_Attr);
    پیغام_puts_attr(":", qfSepAttr);
    إذا (qfp->qf_نمونه != عدم)
    {
	gap = qfga_get();
	qf_fmt_متن(gap, qfp->qf_نمونه);
	ga_append(gap, NUL);
	پیغام_puts((حرف *)gap->ga_معلومات);
	پیغام_puts_attr(":", qfSepAttr);
    }
    پیغام_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_متن(gap, (fname != عدم || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_متن) : qfp->qf_متن);
    ga_append(gap, NUL);
    پیغام_prt_کرښه((char_u *)gap->ga_معلومات, FALSE);
    بهر_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    فراغ
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحيح		i;
    صحيح		idx1 = 1;
    صحيح		idx2 = -1;
    char_u	*arg = eap->arg;
    صحيح		plus = FALSE;
    صحيح		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    إذا ((qi = qf_cmd_get_ډېری(eap, TRUE)) == عدم)
	أرجع;

    إذا (qf_ډېری_تشه(qi) || qf_list_تشه(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	أرجع;
    }
    إذا (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    إذا (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	أرجع;
    }
    qfl = qf_get_curlist(qi);
    إذا (plus)
    {
	i = qfl->qf_شاخص;
	idx2 = i + idx1;
	idx1 = i;
    }
    وإلا
    {
	i = qfl->qf_شمېر;
	إذا (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	إذا (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    إذا (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    إذا (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_کرښه_Attr = syn_name2attr((char_u *)"qfLineNr");
    إذا (qf_کرښه_Attr == 0)
	qf_کرښه_Attr = HL_ATTR(HLF_N);

    إذا (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_توکي(qfl, qfp, i)
    {
	إذا ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_ثبت(qfp, i, i == qfl->qf_شاخص);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    ساكن فراغ
qf_fmt_متن(garray_T *gap, char_u *متن)
{
    char_u	*p = متن;
    طالما (*p != NUL)
    {
	إذا (*p == '\n')
	{
	    ga_append(gap, ' ');
	    طالما (*++p != NUL)
		إذا (!VIM_ISWHITE(*p) && *p != '\n')
		    اكسر;
	}
	وإلا
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    ساكن فراغ
qf_range_متن(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.اوږدتیا = vim_snprintf_safelen((حرف *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    إذا (qfp->qf_پای_ټکی_lnum > 0 && qfp->qf_lnum != qfp->qf_پای_ټکی_lnum)
    {
	buf.اوږدتیا += vim_snprintf_safelen((حرف *)buf.string + buf.اوږدتیا,
	    IOSIZE - buf.اوږدتیا, "-%ld", qfp->qf_پای_ټکی_lnum);
    }
    إذا (qfp->qf_ستون > 0)
    {
	buf.اوږدتیا += vim_snprintf_safelen((حرف *)buf.string + buf.اوږدتیا,
	    IOSIZE - buf.اوږدتیا, " col %d", qfp->qf_ستون);
	إذا (qfp->qf_پای_ټکی_ستون > 0 && qfp->qf_ستون != qfp->qf_پای_ټکی_ستون)
	    buf.اوږدتیا += vim_snprintf_safelen((حرف *)buf.string + buf.اوږدتیا,
		IOSIZE - buf.اوږدتیا, "-%d", qfp->qf_پای_ټکی_ستون);
    }

    ga_concat_len(gap, buf.string, buf.اوږدتیا);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    ساكن فراغ
qf_پیغام(qf_info_T *qi, صحيح which, حرف *lead)
{
    حرف   *title = (حرف *)qi->qf_lists[which].qf_title;
    صحيح    شمېر = qi->qf_lists[which].qf_شمېر;
    char_u buf[IOSIZE];

    vim_snprintf((حرف *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    شمېر);

    إذا (title != عدم)
    {
	size_t	len = STRLEN(buf);

	إذا (len < 34)
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
    فراغ
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    صحيح		شمېر;

    إذا ((qi = qf_cmd_get_ډېری(eap, TRUE)) == عدم)
	أرجع;

    إذا (eap->addr_شمېر != 0)
	شمېر = eap->line2;
    وإلا
	شمېر = 1;
    طالما (شمېر--)
    {
	إذا (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    إذا (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_د_quickfix_ډېری));
		اكسر;
	    }
	    --qi->qf_curlist;
	}
	وإلا
	{
	    إذا (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_سر_د_quickfix_ډېری));
		اكسر;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_پیغام(qi, qi->qf_curlist, "");
    qf_update_بفر(qi, عدم);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    فراغ
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_ډېری(eap, FALSE);
    صحيح		i;

    إذا (eap->addr_شمېر > 0)
    {
	إذا (qi == عدم)
	{
	    emsg(_(e_no_location_list));
	    أرجع;
	}

	// Jump to the specified quickfix list
	إذا (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_پیغام(qi, qi->qf_curlist, "");
	    qf_update_بفر(qi, عدم);
	}
	وإلا
	    emsg(_(e_invalid_range));

	أرجع;
    }

    إذا (qf_ډېری_تشه(qi))
	پیغام(_("No entries"));
    وإلا
	لكل (i = 0; i < qi->qf_listcount; ++i)
	    qf_پیغام(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    ساكن فراغ
qf_free_توکي(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    صحيح		stop = FALSE;

    طالما (qfl->qf_شمېر && qfl->qf_پیل != عدم)
    {
	qfp = qfl->qf_پیل;
	qfpnext = qfp->qf_بل;
	إذا (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_متن);
	    vim_free(qfp->qf_نمونه);
	    clear_tv(&qfp->qf_user_معلومات);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    إذا (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_شمېر = 1;
	    وإلا
		qfl->qf_پیل = qfpnext;
	}
	--qfl->qf_شمېر;
    }

    qfl->qf_شاخص = 0;
    qfl->qf_پیل = عدم;
    qfl->qf_وروستی = عدم;
    qfl->qf_ptr = عدم;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_ډېری(&qfl->qf_dir_ډېری);
    qfl->qf_directory = عدم;
    qf_clean_dir_ډېری(&qfl->qf_file_ډېری);
    qfl->qf_currfile = عدم;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    ساكن فراغ
qf_free(qf_list_T *qfl)
{
    qf_free_توکي(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = عدم;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   فراغ
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	اوږد	amount,
	اوږد	amount_after)
{
    صحيح		i;
    qfline_T	*qfp;
    صحيح		idx;
    qf_info_T	*qi = ql_info;
    صحيح		وموندل_one = FALSE;
    صحيح		buf_has_flag = wp == عدم ? BUF_HAS_QF_ثبت : BUF_HAS_LL_ثبت;

    إذا (!(curbuf->b_has_qf_ثبت & buf_has_flag))
	أرجع;
    إذا (wp != عدم)
    {
	إذا (wp->w_llist == عدم)
	    أرجع;
	qi = wp->w_llist;
    }
    وإلا إذا (qi == عدم)
	أرجع;

    لكل (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	إذا (!qf_list_تشه(qfl))
	    FOR_ALL_QFL_توکي(qfl, qfp, i)
		إذا (qfp->qf_fnum == curbuf->b_fnum)
		{
		    وموندل_one = TRUE;
		    إذا (qfp->qf_cleared)
			استمر;

		    إذا (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			إذا (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			وإلا
			    qfp->qf_lnum += amount;
		    }
		    وإلا إذا (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    إذا (!وموندل_one)
	curbuf->b_has_qf_ثبت &= ~buf_has_flag;
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
    ساكن char_u *
qf_types(صحيح c, صحيح nr)
{
    ساكن char_u	buf[20];
    ساكن char_u	cc[3];
    char_u		*p;

    إذا (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    وإلا إذا (c == 'I' || c == 'i')
	p = (char_u *)" info";
    وإلا إذا (c == 'N' || c == 'n')
	p = (char_u *)" note";
    وإلا إذا (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    وإلا إذا (c == 0 || c == 1)
	p = (char_u *)"";
    وإلا
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    إذا (nr <= 0)
	أرجع p;

    sprintf((حرف *)buf, "%s %3d", (حرف *)p, nr);
    أرجع buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    فراغ
qf_view_پایله(صحيح وېشل)
{
    qf_info_T   *qi = ql_info;

    إذا (IS_LL_کړکۍ(curwin))
	qi = GET_LOC_LIST(curwin);
    وإلا إذا (qi == عدم)
    {
	emsg(_(e_no_quickfix_ډېری));
	أرجع;
    }

    إذا (qf_list_تشه(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	أرجع;
    }

    إذا (وېشل)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (اوږد)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	أرجع;
    }

    do_cmdline_cmd((char_u *)(IS_LL_کړکۍ(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    فراغ
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    إذا ((qi = qf_cmd_get_ډېری(eap, TRUE)) == عدم)
	أرجع;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_پیدا_کړه_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    إذا (qf_ډېری_تشه(qi)
	    || qfl->qf_nonevalid
	    || qf_list_تشه(qfl))
    {
	إذا (win != عدم)
	    ex_cclose(eap);
    }
    وإلا إذا (win == عدم)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    فراغ
ex_cclose(exarg_T *eap)
{
    win_T	*win = عدم;
    qf_info_T	*qi;

    إذا ((qi = qf_cmd_get_ډېری(eap, FALSE)) == عدم)
	أرجع;

    // Find existing quickfix window and close it.
    win = qf_پیدا_کړه_win(qi);
    إذا (win != عدم)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    ساكن فراغ
qf_سېټ_title_var(qf_list_T *qfl)
{
    إذا (qfl->qf_title != عدم)
	سېټ_دننی_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    ساكن صحيح
qf_goto_cwindow(qf_info_T *qi, صحيح resize, صحيح sz, صحيح vertsplit)
{
    win_T	*win;

    win = qf_پیدا_کړه_win(qi);
    إذا (win == عدم)
	أرجع FAIL;

    win_goto(win);
    إذا (resize)
    {
	إذا (vertsplit)
	{
	    إذا (sz != win->w_پلنوالی)
		win_setwidth(sz);
	}
	وإلا إذا (sz != win->w_لوړوالی && win->w_لوړوالی
		       + win->w_حالت_لوړوالی + tabline_لوړوالی() < cmdline_لیکه)
	    win_setheight(sz);
    }

    أرجع OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    ساكن فراغ
qf_سېټ_cwindow_options(فراغ)
{
    // switch off 'swapfile'
    سېټ_option_ارزښت_give_err((char_u *)"swf", 0L, عدم, OPT_LOCAL);
    سېټ_option_ارزښت_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    سېټ_option_ارزښت_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    سېټ_option_ارزښت_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    ساكن صحيح
qf_open_new_cwindow(qf_info_T *qi, صحيح لوړوالی)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    صحيح		flags = 0;
    win_T	*win;

    qf_buf = qf_پیدا_کړه_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    إذا (IS_QF_ډېری(qi) && cmdmod.cmod_وېشل == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    إذا (cmdmod.cmod_وېشل == 0)
	flags = WSP_لاندې;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    إذا (IS_QF_ډېری(qi))
	flags |= WSP_QUICKFIX;

    إذا (win_وېشل(لوړوالی, flags) == FAIL)
	أرجع FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    إذا (IS_LL_ډېری(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    إذا (oldwin != curwin)
	oldwin = عدم;  // don't store info when in another window
    إذا (qf_buf != عدم)
    {
	// Use the existing quickfix buffer
	إذا (do_ecmd(qf_buf->b_fnum, عدم, عدم, عدم, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    أرجع FAIL;
    }
    وإلا
    {
	// Create a new quickfix buffer
	إذا (do_ecmd(0, عدم, عدم, عدم, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    أرجع FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    إذا (!bt_quickfix(curbuf))
	qf_سېټ_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    إذا (curtab == prevtab && curwin->w_پلنوالی == Columns)
	win_setheight(لوړوالی);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    إذا (win_valid(win))
	prevwin = win;

    أرجع OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    فراغ
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    صحيح		لوړوالی;
    صحيح		حالت = FAIL;
    صحيح		lnum;

    إذا ((qi = qf_cmd_get_ډېری(eap, TRUE)) == عدم)
	أرجع;

    incr_quickfix_busy();

    إذا (eap->addr_شمېر != 0)
	لوړوالی = eap->line2;
    وإلا
	لوړوالی = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    إذا (cmdmod.cmod_tab == 0)
	حالت = qf_goto_cwindow(qi, eap->addr_شمېر != 0, لوړوالی,
						cmdmod.cmod_وېشل & WSP_VERT);
    إذا (حالت == FAIL)
	إذا (qf_open_new_cwindow(qi, لوړوالی) == FAIL)
	{
	    decr_quickfix_busy();
	    أرجع;
	}

    qfl = qf_get_curlist(qi);
    qf_سېټ_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_شاخص;

    // Fill the buffer with the quickfix list.
    qf_fill_بفر(qfl, curbuf, عدم, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ستون = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    ساكن فراغ
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
    فراغ
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    إذا ((qi = qf_cmd_get_ډېری(eap, TRUE)) == عدم)
	أرجع;

    win = qf_پیدا_کړه_win(qi);
    إذا (win != عدم && win->w_cursor.lnum != win->w_بفر->b_ml.ml_کرښه_شمېر)
	qf_win_goto(win, win->w_بفر->b_ml.ml_کرښه_شمېر);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_ثبت(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    إذا (IS_LL_کړکۍ(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    وإلا إذا (qi == عدم)
	أرجع 0;

    أرجع qf_get_curlist(qi)->qf_شاخص;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    ساكن صحيح
qf_win_pos_update(
    qf_info_T	*qi,
    صحيح		old_qf_شاخص)	// previous qf_index or zero
{
    win_T	*win;
    صحيح		qf_شاخص = qf_get_curlist(qi)->qf_شاخص;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_پیدا_کړه_win(qi);
    إذا (win != عدم
	    && qf_شاخص <= win->w_بفر->b_ml.ml_کرښه_شمېر
	    && old_qf_شاخص != qf_شاخص)
    {
	إذا (qf_شاخص > old_qf_شاخص)
	{
	    win->w_redraw_سر = old_qf_شاخص;
	    win->w_redraw_bot = qf_شاخص;
	}
	وإلا
	{
	    win->w_redraw_سر = qf_شاخص;
	    win->w_redraw_bot = old_qf_شاخص;
	}
	qf_win_goto(win, qf_شاخص);
    }
    أرجع win != عدم;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    ساكن صحيح
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    إذا (buf_valid(win->w_بفر) && bt_quickfix(win->w_بفر))
	إذا ((IS_QF_ډېری(qi) && win->w_llist_ref == عدم)
		|| (IS_LL_ډېری(qi) && win->w_llist_ref == qi))
	    أرجع TRUE;

    أرجع FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    ساكن win_T *
qf_پیدا_کړه_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	إذا (is_qf_win(win, qi))
	    أرجع win;
    أرجع عدم;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    ساكن buf_T *
qf_پیدا_کړه_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    إذا (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	إذا (qfbuf != عدم)
	    أرجع qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	إذا (is_qf_win(win, qi))
	    أرجع win->w_بفر;

    أرجع عدم;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    حرف *
did_سېټ_quickfixtextfunc(optset_T *دلیلونه ناکارول_شوی)
{
    إذا (option_سېټ_callback_func(p_qftf, &qftf_cb) == FAIL)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    ساكن فراغ
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	إذا (is_qf_win(win, qi))
	{
	    curwin = win;
	    qf_سېټ_title_var(qfl);
	}
    }
    curwin = save_curwin;
}

/*
 * Find the quickfix buffer.  If it exists, update the contents.
 */
    ساكن فراغ
qf_update_بفر(qf_info_T *qi, qfline_T *old_وروستی)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_پیدا_کړه_buf(qi);
    إذا (buf == عدم)
	أرجع;

    linenr_T	old_کرښه_شمېر = buf->b_ml.ml_کرښه_شمېر;
    صحيح		qf_winid = 0;

    إذا (IS_LL_ډېری(qi))
    {
	إذا (curwin->w_llist == qi)
	    win = curwin;
	وإلا
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_پیدا_کړه_win_with_loclist(qi);
	    إذا (win == عدم)
		// File window is not found. Find the location list window.
		win = qf_پیدا_کړه_win(qi);
	    إذا (win == عدم)
		أرجع;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    صحيح do_fill = TRUE;
    إذا (old_وروستی == عدم)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    إذا (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_بفر(qf_get_curlist(qi), buf, old_وروستی, qf_winid);
	++CHANGEDTICK(buf);

	إذا (old_وروستی == عدم)
	{
	    (فراغ)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    إذا ((win = qf_پیدا_کړه_win(qi)) != عدم && old_کرښه_شمېر < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    ساكن صحيح
qf_buf_جمع_کړه_کرښه(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	صحيح		لومړی_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    إذا (qftf_str != عدم && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    وإلا
    {
	إذا (qfp->qf_module != عدم)
	    ga_concat(gap, qfp->qf_module);
	وإلا إذا (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != عدم
		&& errbuf->b_fname != عدم)
	{
	    إذا (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    وإلا
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		إذا (لومړی_bufline && (errbuf->b_sfname == عدم
				|| mch_is_ډک_Name(errbuf->b_sfname)))
		{
		    إذا (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		إذا (qfp->qf_fname == عدم)
		    ga_concat(gap, errbuf->b_fname);
		وإلا
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	إذا (qfp->qf_lnum > 0)
	{
	    qf_range_متن(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	وإلا إذا (qfp->qf_نمونه != عدم)
	    qf_fmt_متن(gap, qfp->qf_نمونه);
	ga_append(gap, '|');
	ga_append(gap, ' ');

	// Remove newlines and leading whitespace from the text.
	// For an unrecognized line keep the indent, the compiler may
	// mark a word with ^^^^.
	qf_fmt_متن(gap, gap->ga_len > 3 ? skipwhite(qfp->qf_متن)
							       : qfp->qf_متن);
    }

    ga_append(gap, NUL);
    إذا (ml_append_buf(buf, lnum, gap->ga_معلومات, gap->ga_len, FALSE) == FAIL)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    ساكن list_T *
call_qftf_func(qf_list_T *qfl, صحيح qf_winid, اوږد پیل_idx, اوږد پای_ټکی_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = عدم;
    ساكن صحيح	recursive = FALSE;

    إذا (recursive)
	أرجع عدم;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    إذا (qfl->qf_qftf_cb.cb_name != عدم)
	cb = &qfl->qf_qftf_cb;
    إذا (cb->cb_name != عدم)
    {
	typval_T	دلیلونه[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	إذا ((d = dict_alloc_قلف(VAR_FIXED)) == عدم)
	{
	    recursive = FALSE;
	    أرجع عدم;
	}
	dict_جمع_کړه_شمېره(d, "quickfix", (اوږد)IS_QF_LIST(qfl));
	dict_جمع_کړه_شمېره(d, "winid", (اوږد)qf_winid);
	dict_جمع_کړه_شمېره(d, "id", (اوږد)qfl->qf_id);
	dict_جمع_کړه_شمېره(d, "start_idx", پیل_idx);
	dict_جمع_کړه_شمېره(d, "end_idx", پای_ټکی_idx);
	++d->dv_refcount;
	دلیلونه[0].v_type = VAR_DICT;
	دلیلونه[0].vval.v_dict = d;

	qftf_list = عدم;
	textlock++;
	إذا (call_callback(cb, 0, &rettv, 1, دلیلونه) != FAIL)
	{
	    إذا (rettv.v_type == VAR_LIST)
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
    أرجع qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    ساكن فراغ
qf_fill_بفر(qf_list_T *qfl, buf_T *buf, qfline_T *old_وروستی, صحيح qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    صحيح		old_کلی_Typed = کلی_Typed;
    list_T	*qftf_list = عدم;
    listitem_T	*qftf_li = عدم;

    إذا (old_وروستی == عدم)
    {
	win_T		*wp;
	tabpage_T	*tp;

	إذا (buf != curbuf)
	{
	    دننی_error("qf_fill_buffer()");
	    أرجع;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	طالما ((curbuf->b_ml.ml_flags & ML_تشه) == 0)
	    (فراغ)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    إذا (wp->w_بفر == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    إذا (qfl != عدم && qfl->qf_پیل != عدم)
    {
	char_u		dirname[MAXPATHL];
	صحيح		invalid_val = FALSE;
	صحيح		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	إذا (old_وروستی == عدم)
	{
	    qfp = qfl->qf_پیل;
	    lnum = 0;
	}
	وإلا
	{
	    إذا (old_وروستی->qf_بل != عدم)
		qfp = old_وروستی->qf_بل;
	    وإلا
		qfp = old_وروستی;
	    lnum = buf->b_ml.ml_کرښه_شمېر;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (اوږد)(lnum + 1),
							(اوږد)qfl->qf_شمېر);
	إذا (qftf_list != عدم)
	    qftf_li = qftf_list->lv_لومړی;

	طالما (lnum < qfl->qf_شمېر)
	{
	    char_u	*qftf_str = عدم;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    إذا (qftf_li != عدم && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		إذا (qftf_str == عدم)
		    invalid_val = TRUE;
	    }

	    إذا (qf_buf_جمع_کړه_کرښه(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		اكسر;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_بل;
	    إذا (qfp == عدم)
		اكسر;

	    إذا (qftf_li != عدم)
		qftf_li = qftf_li->li_بل;
	}

	إذا (old_وروستی == عدم)
	    // Delete the empty line which is now at the end
	    (فراغ)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    إذا (old_وروستی == عدم)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_قلف;
	سېټ_option_ارزښت_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", عدم,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", عدم,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_قلف;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    کلی_Typed = old_کلی_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    ساكن فراغ
qf_list_بدل_شوی(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    ساكن صحيح
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    صحيح		qf_idx;

    لكل (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	إذا (qi->qf_lists[qf_idx].qf_id == qfid)
	    أرجع qf_idx;
    أرجع INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    ساكن صحيح
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    صحيح curlist;

    إذا (qf_get_curlist(qi)->qf_id == save_qfid)
	أرجع OK;

    curlist = qf_id2nr(qi, save_qfid);
    إذا (curlist < 0)
	// list is not present
	أرجع FAIL;
    qi->qf_curlist = curlist;
    أرجع OK;
}

/*
 * Jump to the first entry if there is one.
 */
    ساكن فراغ
qf_jump_لومړی(qf_info_T *qi, int_u save_qfid, صحيح forceit)
{
    إذا (qf_restore_list(qi, save_qfid) == FAIL)
	أرجع;


    إذا (!check_can_سېټ_curbuf_forceit(forceit))
	أرجع;


    // Autocommands might have cleared the list, check for that.
    إذا (!qf_list_تشه(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    صحيح
grep_دننی(cmdidx_T cmdidx)
{
    أرجع ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    ساكن char_u *
make_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_make:	    أرجع (char_u *)"make";
	حالة CMD_lmake:	    أرجع (char_u *)"lmake";
	حالة CMD_grep:	    أرجع (char_u *)"grep";
	حالة CMD_lgrep:	    أرجع (char_u *)"lgrep";
	حالة CMD_grepadd:   أرجع (char_u *)"grepadd";
	حالة CMD_lgrepadd:  أرجع (char_u *)"lgrepadd";
	افتراضي: أرجع عدم;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    ساكن char_u *
get_mef_name(فراغ)
{
    char_u	*p;
    char_u	*name;
    ساكن صحيح	پیل = -1;
    ساكن صحيح	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    إذا (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	إذا (name == عدم)
	    emsg(_(e_cant_get_لنډمهاله_file_name));
	أرجع name;
    }

    لكل (p = p_mef; *p; ++p)
	إذا (p[0] == '#' && p[1] == '#')
	    اكسر;

    إذا (*p == NUL)
	أرجع vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    لكل (;;)
    {
	إذا (پیل == -1)
	    پیل = mch_get_pid();
	وإلا
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	إذا (name == عدم)
	    اكسر;
	STRCPY(name, p_mef);
	sprintf((حرف *)name + (p - p_mef), "%d%d", پیل, off);
	STRCAT(name, p + 2);
	إذا (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((حرف *)name, &sb) < 0
# endif
		)
	    اكسر;
	vim_free(name);
    }
    أرجع name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    ساكن char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    بینښه	len;

    len = (بینښه)STRLEN(p_shq) * 2 + (بینښه)STRLEN(makecmd) + 1;
    إذا (*p_sp != NUL)
	len += (بینښه)STRLEN(p_sp) + (بینښه)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    إذا (cmd == عدم)
	أرجع عدم;
    sprintf((حرف *)cmd, "%s%s%s", (حرف *)p_shq, (حرف *)makecmd,
							       (حرف *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    إذا (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    إذا (پیغام_ستون == 0)
	پیغام_didout = FALSE;
    پیغام_پیل();
    پیغام_puts(":!");
    پیغام_outtrans(cmd);		// show what we are doing

    أرجع cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    فراغ
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = عدم;
    win_T	*wp = عدم;
    qf_info_T	*qi = ql_info;
    صحيح		res;
    char_u	*au_name = عدم;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    صحيح		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    إذا (grep_دننی(eap->cmdidx))
    {
	ex_vimgrep(eap);
	أرجع;
    }

    au_name = make_get_auname(eap->cmdidx);
    إذا (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	إذا (aborting())
	    أرجع;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    إذا (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    إذا (fname == عدم)
	أرجع;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    إذا (cmd == عدم)
    {
	vim_free(fname);
	أرجع;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    بهر_flush();
		// read window status report and redraw before message
    (فراغ)char_avail();
# endif

    incr_quickfix_busy();

    إذا (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    إذا (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    إذا (wp != عدم)
    {
	qi = GET_LOC_LIST(wp);
	إذا (qi == عدم)
	    اذهب cleanup;
    }
    وإلا إذا (qi == عدم)
	اذهب cleanup;

    إذا (res >= 0)
	qf_list_بدل_شوی(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    إذا (au_name != عدم)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    إذا (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_لومړی(qi, save_qfid, FALSE);

cleanup:
    decr_quickfix_busy();
    mch_remove(fname);
    vim_free(fname);
    vim_free(cmd);
}

/*
 * Returns the number of entries in the current quickfix/location list.
 */
    صحيح
qf_get_اندازه(exarg_T *eap)
{
    qf_info_T	*qi;

    إذا ((qi = qf_cmd_get_ډېری(eap, FALSE)) == عدم)
	أرجع 0;
    أرجع qf_get_curlist(qi)->qf_شمېر;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    صحيح
qf_get_valid_اندازه(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحيح		i, sz = 0;
    صحيح		prev_fnum = 0;

    إذا ((qi = qf_cmd_get_ډېری(eap, FALSE)) == عدم)
	أرجع 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_توکي(qfl, qfp, i)
    {
	إذا (qfp->qf_valid)
	{
	    إذا (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    وإلا إذا (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    أرجع sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    صحيح
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    إذا ((qi = qf_cmd_get_ډېری(eap, FALSE)) == عدم)
	أرجع 0;

    أرجع qf_get_curlist(qi)->qf_شاخص;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    صحيح
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحيح		i, eidx = 0;
    صحيح		prev_fnum = 0;

    إذا ((qi = qf_cmd_get_ډېری(eap, FALSE)) == عدم)
	أرجع 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_پیل;

    // check if the list has valid errors
    إذا (!qf_list_has_valid_entries(qfl))
	أرجع 1;

    لكل (i = 1; i <= qfl->qf_شاخص && qfp!= عدم; i++, qfp = qfp->qf_بل)
    {
	إذا (qfp->qf_valid)
	{
	    إذا (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		إذا (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    وإلا
		eidx++;
	}
    }

    أرجع eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    ساكن صحيح
qf_get_nth_valid_ثبت(qf_list_T *qfl, صحيح n, صحيح fdo)
{
    qfline_T	*qfp;
    صحيح		i, eidx;
    صحيح		prev_fnum = 0;

    // check if the list has valid errors
    إذا (!qf_list_has_valid_entries(qfl))
	أرجع 1;

    eidx = 0;
    FOR_ALL_QFL_توکي(qfl, qfp, i)
    {
	إذا (qfp->qf_valid)
	{
	    إذا (fdo)
	    {
		إذا (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    وإلا
		eidx++;
	}

	إذا (eidx == n)
	    اكسر;
    }

    إذا (i <= qfl->qf_شمېر)
	أرجع i;
    وإلا
	أرجع 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    فراغ
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    صحيح		errornr;

    إذا ((qi = qf_cmd_get_ډېری(eap, TRUE)) == عدم)
	أرجع;

    إذا (eap->addr_شمېر > 0)
	errornr = (صحيح)eap->line2;
    وإلا
    {
	بدل (eap->cmdidx)
	{
	    حالة CMD_cc: حالة CMD_ll:
		errornr = 0;
		اكسر;
	    حالة CMD_crewind: حالة CMD_lrewind: حالة CMD_cfirst:
	    حالة CMD_lfirst:
		errornr = 1;
		اكسر;
	    افتراضي:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    إذا (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_ثبت(qf_get_curlist(qi),
		eap->addr_شمېر > 0 ? (صحيح)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    فراغ
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    صحيح		errornr;
    صحيح		dir;

    إذا ((qi = qf_cmd_get_ډېری(eap, TRUE)) == عدم)
	أرجع;

    إذا (eap->addr_شمېر > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (صحيح)eap->line2;
    وإلا
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    بدل (eap->cmdidx)
    {
	حالة CMD_cnext: حالة CMD_lnext: حالة CMD_cdo: حالة CMD_ldo:
	    dir = FORWARD;
	    اكسر;
	حالة CMD_cprevious: حالة CMD_lprevious: حالة CMD_c_بل:
	حالة CMD_l_بل:
	    dir = BACKWARD;
	    اكسر;
	حالة CMD_cnfile: حالة CMD_lnfile: حالة CMD_cfdo: حالة CMD_lfdo:
	    dir = FORWARD_FILE;
	    اكسر;
	حالة CMD_cpfile: حالة CMD_lpfile: حالة CMD_cNfile: حالة CMD_lNfile:
	    dir = BACKWARD_FILE;
	    اكسر;
	افتراضي:
	    dir = FORWARD;
	    اكسر;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    ساكن qfline_T *
qf_پیدا_کړه_لومړی_ثبت_دننه_buf(qf_list_T *qfl, صحيح bnr, صحيح *errornr)
{
    qfline_T	*qfp = عدم;
    صحيح		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_توکي(qfl, qfp, idx)
	إذا (qfp->qf_fnum == bnr)
	    اكسر;

    *errornr = idx;
    أرجع qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ساكن qfline_T *
qf_پیدا_کړه_لومړی_ثبت_on_کرښه(qfline_T *ثبت, صحيح *errornr)
{
    طالما (!got_int
	    && ثبت->qf_prev != عدم
	    && ثبت->qf_fnum == ثبت->qf_prev->qf_fnum
	    && ثبت->qf_lnum == ثبت->qf_prev->qf_lnum)
    {
	ثبت = ثبت->qf_prev;
	--*errornr;
    }

    أرجع ثبت;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ساكن qfline_T *
qf_پیدا_کړه_وروستی_ثبت_on_کرښه(qfline_T *ثبت, صحيح *errornr)
{
    طالما (!got_int &&
	    ثبت->qf_بل != عدم
	    && ثبت->qf_fnum == ثبت->qf_بل->qf_fnum
	    && ثبت->qf_lnum == ثبت->qf_بل->qf_lnum)
    {
	ثبت = ثبت->qf_بل;
	++*errornr;
    }

    أرجع ثبت;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    ساكن صحيح
qf_ثبت_after_pos(qfline_T *qfp, pos_T *pos, صحيح linewise)
{
    إذا (linewise)
	أرجع qfp->qf_lnum > pos->lnum;
    وإلا
	أرجع (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ستون > pos->ستون));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    ساكن صحيح
qf_ثبت_before_pos(qfline_T *qfp, pos_T *pos, صحيح linewise)
{
    إذا (linewise)
	أرجع qfp->qf_lnum < pos->lnum;
    وإلا
	أرجع (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ستون < pos->ستون));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    ساكن صحيح
qf_ثبت_on_or_after_pos(qfline_T *qfp, pos_T *pos, صحيح linewise)
{
    إذا (linewise)
	أرجع qfp->qf_lnum >= pos->lnum;
    وإلا
	أرجع (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ستون >= pos->ستون));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    ساكن صحيح
qf_ثبت_on_or_before_pos(qfline_T *qfp, pos_T *pos, صحيح linewise)
{
    إذا (linewise)
	أرجع qfp->qf_lnum <= pos->lnum;
    وإلا
	أرجع (qfp->qf_lnum < pos->lnum ||
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
    ساكن qfline_T *
qf_پیدا_کړه_ثبت_after_pos(
	صحيح		bnr,
	pos_T		*pos,
	صحيح		linewise,
	qfline_T	*qfp,
	صحيح		*errornr)
{
    إذا (qf_ثبت_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	أرجع qfp;

    // Find the entry just before or at the position 'pos'
    طالما (qfp->qf_بل != عدم
	    && qfp->qf_بل->qf_fnum == bnr
	    && qf_ثبت_on_or_before_pos(qfp->qf_بل, pos, linewise))
    {
	qfp = qfp->qf_بل;
	++*errornr;
    }

    إذا (qfp->qf_بل == عدم || qfp->qf_بل->qf_fnum != bnr)
	// No entries found after position 'pos'
	أرجع عدم;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_بل;
    ++*errornr;

    أرجع qfp;
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
    ساكن qfline_T *
qf_پیدا_کړه_ثبت_before_pos(
	صحيح		bnr,
	pos_T		*pos,
	صحيح		linewise,
	qfline_T	*qfp,
	صحيح		*errornr)
{
    // Find the entry just before the position 'pos'
    طالما (qfp->qf_بل != عدم
	    && qfp->qf_بل->qf_fnum == bnr
	    && qf_ثبت_before_pos(qfp->qf_بل, pos, linewise))
    {
	qfp = qfp->qf_بل;
	++*errornr;
    }

    إذا (qf_ثبت_on_or_after_pos(qfp, pos, linewise))
	أرجع عدم;

    إذا (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_پیدا_کړه_لومړی_ثبت_on_کرښه(qfp, errornr);

    أرجع qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    ساكن qfline_T *
qf_پیدا_کړه_closest_ثبت(
	qf_list_T	*qfl,
	صحيح		bnr,
	pos_T		*pos,
	صحيح		dir,
	صحيح		linewise,
	صحيح		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_پیدا_کړه_لومړی_ثبت_دننه_buf(qfl, bnr, errornr);
    إذا (qfp == عدم)
	أرجع عدم;		// no entry in this file

    إذا (dir == FORWARD)
	qfp = qf_پیدا_کړه_ثبت_after_pos(bnr, pos, linewise, qfp, errornr);
    وإلا
	qfp = qf_پیدا_کړه_ثبت_before_pos(bnr, pos, linewise, qfp, errornr);

    أرجع qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ساكن فراغ
qf_get_nth_لاندې_ثبت(qfline_T *ثبت_arg, صحيح n, صحيح linewise, صحيح *errornr)
{
    qfline_T *ثبت = ثبت_arg;

    طالما (n-- > 0 && !got_int)
    {
	صحيح		لومړی_errornr = *errornr;

	إذا (linewise)
	    // Treat all the entries on the same line in this file as one
	    ثبت = qf_پیدا_کړه_وروستی_ثبت_on_کرښه(ثبت, errornr);

	إذا (ثبت->qf_بل == عدم
		|| ثبت->qf_بل->qf_fnum != ثبت->qf_fnum)
	{
	    إذا (linewise)
		*errornr = لومړی_errornr;
	    اكسر;
	}

	ثبت = ثبت->qf_بل;
	++*errornr;
    }
}

/*
 * Get the nth quickfix entry above the specified entry.  Searches backwards in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ساكن فراغ
qf_get_nth_above_ثبت(qfline_T *ثبت, صحيح n, صحيح linewise, صحيح *errornr)
{
    طالما (n-- > 0 && !got_int)
    {
	إذا (ثبت->qf_prev == عدم
		|| ثبت->qf_prev->qf_fnum != ثبت->qf_fnum)
	    اكسر;

	ثبت = ثبت->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	إذا (linewise)
	    ثبت = qf_پیدا_کړه_لومړی_ثبت_on_کرښه(ثبت, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    ساكن صحيح
qf_پیدا_کړه_nth_adj_ثبت(
	qf_list_T	*qfl,
	صحيح		bnr,
	pos_T		*pos,
	صحيح		n,
	صحيح		dir,
	صحيح		linewise)
{
    qfline_T	*adj_ثبت;
    صحيح		errornr;

    // Find an entry closest to the specified position
    adj_ثبت = qf_پیدا_کړه_closest_ثبت(qfl, bnr, pos, dir, linewise, &errornr);
    إذا (adj_ثبت == عدم)
	أرجع 0;

    إذا (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	إذا (dir == FORWARD)
	    qf_get_nth_لاندې_ثبت(adj_ثبت, n, linewise, &errornr);
	وإلا
	    qf_get_nth_above_ثبت(adj_ثبت, n, linewise, &errornr);
    }

    أرجع errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    فراغ
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    صحيح		dir;
    صحيح		buf_has_flag;
    صحيح		errornr = 0;
    pos_T	pos;

    إذا (eap->addr_شمېر > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	أرجع;
    }

    // Check whether the current buffer has any quickfix entries
    إذا (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ثبت;
    وإلا
	buf_has_flag = BUF_HAS_LL_ثبت;
    إذا (!(curbuf->b_has_qf_ثبت & buf_has_flag))
    {
	emsg(_(e_no_errors));
	أرجع;
    }

    إذا ((qi = qf_cmd_get_ډېری(eap, TRUE)) == عدم)
	أرجع;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    إذا (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	أرجع;
    }

    إذا (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    وإلا
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.ستون++;
    errornr = qf_پیدا_کړه_nth_adj_ثبت(qfl, curbuf->b_fnum, &pos,
				eap->addr_شمېر > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    إذا (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    وإلا
	emsg(_(e_no_نور_توکي));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    ساكن char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_cfile:	    أرجع (char_u *)"cfile";
	حالة CMD_cgetfile:  أرجع (char_u *)"cgetfile";
	حالة CMD_caddfile:  أرجع (char_u *)"caddfile";
	حالة CMD_lfile:	    أرجع (char_u *)"lfile";
	حالة CMD_lgetfile:  أرجع (char_u *)"lgetfile";
	حالة CMD_laddfile:  أرجع (char_u *)"laddfile";
	افتراضي:	    أرجع عدم;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    فراغ
ex_cfile(exarg_T *eap)
{
    char_u	*enc = عدم;
    win_T	*wp = عدم;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = عدم;
    int_u	save_qfid = 0;		// init for gcc
    صحيح		res;

    au_name = cfile_get_auname(eap->cmdidx);
    إذا (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							عدم, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	إذا (aborting())
	    أرجع;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    إذا (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   عدم, عدم,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), عدم);
	إذا (browse_file == عدم)
	    أرجع;
	سېټ_string_option_مستقیم((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    وإلا
# endif
    إذا (*eap->arg != NUL)
	سېټ_string_option_مستقیم((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    إذا (is_loclist_cmd(eap->cmdidx))
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
    إذا (wp != عدم)
    {
	qi = GET_LOC_LIST(wp);
	إذا (qi == عدم)
	{
	    decr_quickfix_busy();
	    أرجع;
	}
    }
    وإلا إذا (qi == عدم)
    {
	decr_quickfix_busy();
	أرجع;
    }
    إذا (res >= 0)
	qf_list_بدل_شوی(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    إذا (au_name != عدم)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, عدم, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    إذا (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_لومړی(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    ساكن char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_vimgrep:     أرجع (char_u *)"vimgrep";
	حالة CMD_lvimgrep:    أرجع (char_u *)"lvimgrep";
	حالة CMD_vimgrepadd:  أرجع (char_u *)"vimgrepadd";
	حالة CMD_lvimgrepadd: أرجع (char_u *)"lvimgrepadd";
	حالة CMD_grep:	      أرجع (char_u *)"grep";
	حالة CMD_lgrep:	      أرجع (char_u *)"lgrep";
	حالة CMD_grepadd:     أرجع (char_u *)"grepadd";
	حالة CMD_lgrepadd:    أرجع (char_u *)"lgrepadd";
	افتراضي: أرجع عدم;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    ساكن فراغ
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = عدم;

    إذا (s == عدم || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	إذا (وروستی_پلټنه_pat() == عدم)
	{
	    emsg(_(e_no_پخوانی_regular_افاده));
	    أرجع;
	}
	regmatch->regprog = vim_regcomp(وروستی_پلټنه_pat(), RE_MAGIC);
    }
    وإلا
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    ساكن فراغ
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    پیغام_پیل();
    p = پیغام_strtrunc(fname, TRUE);
    إذا (p == عدم)
	پیغام_outtrans(fname);
    وإلا
    {
	پیغام_outtrans(p);
	vim_free(p);
    }
    پیغام_clr_eos();
    پیغام_didout = FALSE;	    // overwrite this message
    پیغام_nowait = TRUE;	    // don't wait for this message
    پیغام_ستون = 0;
    بهر_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    ساكن buf_T *
vgr_بارول_dummy_buf(
	char_u *fname,
	char_u *dirname_پیل,
	char_u *dirname_now)
{
    صحيح		save_mls;
# إذا defined(FEAT_SYN_HL)
    char_u	*save_ei = عدم;
# endif
    buf_T	*buf;

# إذا defined(FEAT_SYN_HL)
    // Don't do Filetype autocommands to avoid loading syntax and
    // indent scripts, a great speed improvement.
    save_ei = au_event_disable(",Filetype");
# endif
    // Don't use modelines here, it's useless.
    save_mls = p_mls;
    p_mls = 0;

    // Load file into a buffer, so that 'fileencoding' is detected,
    // autocommands applied, etc.
    buf = بارول_dummy_بفر(fname, dirname_پیل, dirname_now);

    p_mls = save_mls;
# إذا defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    أرجع buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    ساكن صحيح
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    إذا (!qflist_valid(wp, qfid))
    {
	إذا (wp != عدم)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_بدل_شوی));
	    أرجع FALSE;
	}
	وإلا
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    أرجع TRUE;
	}
    }

    إذا (qf_restore_list(qi, qfid) == FAIL)
	أرجع FALSE;

    أرجع TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    ساكن صحيح
vgr_سمون_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	اوږد	    *tomatch,
	صحيح	    duplicate_name,
	صحيح	    flags)
{
    صحيح		وموندل_سمون = FALSE;
    اوږد	lnum;
    colnr_T	ستون;
    صحيح		pat_len = (صحيح)STRLEN(spat);
    إذا (pat_len > FUZZY_سمون_MAX_LEN)
	pat_len = FUZZY_سمون_MAX_LEN;

    لكل (lnum = 1; lnum <= buf->b_ml.ml_کرښه_شمېر && *tomatch > 0; ++lnum)
    {
	ستون = 0;
	إذا (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    طالما (vim_regexec_multi(regmatch, curwin, buf, lnum,
								ستون, عدم) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		إذا (qf_جمع_کړه_ثبت(qfl,
			    عدم,	// dir
			    fname,
			    عدم,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].ستون + 1,
			    regmatch->endpos[0].ستون + 1,
			    FALSE,	// vis_col
			    عدم,	// search pattern
			    0,		// nr
			    0,		// type
			    عدم,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    اكسر;
		}
		وموندل_سمون = TRUE;
		إذا (--*tomatch == 0)
		    اكسر;
		إذا ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    اكسر;
		ستون = regmatch->endpos[0].ستون
		    + (ستون == regmatch->endpos[0].ستون);
		إذا (ستون > ml_get_buf_len(buf, lnum))
		    اكسر;
	    }
	}
	وإلا
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    صحيح	    score;
	    int_u   matches[FUZZY_سمون_MAX_LEN];
	    int_u   sz = ARRAY_اوږدتیا(matches);

	    // Fuzzy string match
	    CLEAR_ډګر(matches);
	    طالما (fuzzy_سمون(str + ستون, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		إذا (qf_جمع_کړه_ثبت(qfl,
			    عدم,	// dir
			    fname,
			    عدم,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + ستون + 1,
			    0,
			    FALSE,	// vis_col
			    عدم,	// search pattern
			    0,		// nr
			    0,		// type
			    عدم,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    اكسر;
		}
		وموندل_سمون = TRUE;
		إذا (--*tomatch == 0)
		    اكسر;
		إذا ((flags & VGR_GLOBAL) == 0)
		    اكسر;
		ستون = matches[pat_len - 1] + ستون + 1;
		إذا (ستون > linelen)
		    اكسر;
	    }
	}
	کرښه_breakcheck();
	إذا (got_int)
	    اكسر;
    }

    أرجع وموندل_سمون;
}

/*
 * Jump to the first match and update the directory.
 */
    ساكن فراغ
vgr_jump_منزل_سمون(
	qf_info_T   *qi,
	صحيح	    forceit,
	صحيح	    *redraw_for_dummy,
	buf_T	    *لومړی_سمون_buf,
	char_u	    *موخه_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    إذا (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    إذا (curbuf == لومړی_سمون_buf && موخه_dir != عدم)
    {
	exarg_T ea;

	CLEAR_ډګر(ea);
	ea.arg = موخه_dir;
	ea.cmdidx = CMD_lcd;
	ex_cd(&ea);
    }
}

/*
 * :vimgrep command arguments
 */
تعريفنوع بنية
{
    اوږد	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    صحيح		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    صحيح		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_دلیلونه_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    ساكن صحيح
vgr_process_دلیلونه(
	exarg_T		*eap,
	vgr_دلیلونه_T	*دلیلونه)
{
    char_u	*p;

    CLEAR_اشاره_ګر(دلیلونه);

    دلیلونه->regmatch.regprog = عدم;
    دلیلونه->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    إذا (eap->addr_شمېر > 0)
	دلیلونه->tomatch = eap->line2;
    وإلا
	دلیلونه->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &دلیلونه->spat, &دلیلونه->flags);
    إذا (p == عدم)
    {
	emsg(_(e_invalid_پلټنه_نمونه_or_delimiter));
	أرجع FAIL;
    }

    vgr_init_regmatch(&دلیلونه->regmatch, دلیلونه->spat);
    إذا (دلیلونه->regmatch.regprog == عدم)
	أرجع FAIL;

    p = skipwhite(p);
    إذا (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_نمونه));
	أرجع FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    إذا ((get_arglist_exp(p, &دلیلونه->fcount, &دلیلونه->fnames, TRUE) == FAIL) ||
	دلیلونه->fcount == 0)
    {
	emsg(_(e_no_سمون));
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    ساكن صحيح
existing_swapfile(buf_T *buf)
{
    إذا (buf->b_ml.ml_mfp != عدم && buf->b_ml.ml_mfp->mf_fname != عدم)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	أرجع fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    أرجع FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    ساكن صحيح
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_دلیلونه_T	*cmd_دلیلونه,
	صحيح		*redraw_for_dummy,
	buf_T		**لومړی_سمون_buf,
	char_u		**موخه_dir)
{
    صحيح		حالت = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    صحيح		fi;
    buf_T	*buf;
    صحيح		duplicate_name = FALSE;
    صحيح		using_dummy;
    char_u	*dirname_پیل = عدم;
    char_u	*dirname_now = عدم;
    صحيح		وموندل_سمون;
    aco_save_T	aco;

    dirname_پیل = alloc_id(MAXPATHL, aid_qf_dirname_پیل);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    إذا (dirname_پیل == عدم || dirname_now == عدم)
	اذهب theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_پیل, MAXPATHL);

    seconds = (time_t)0;
    لكل (fi = 0; fi < cmd_دلیلونه->fcount && !got_int && cmd_دلیلونه->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_دلیلونه->fnames[fi]);
	إذا (وخت(عدم) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = وخت(عدم);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_دلیلونه->fnames[fi]);
	إذا (buf == عدم || buf->b_ml.ml_mfp == عدم)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != عدم);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_بارول_dummy_buf(fname, dirname_پیل, dirname_now);
	}
	وإلا
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	إذا (!vgr_qflist_valid(wp, qi, save_qfid, cmd_دلیلونه->qf_title))
	    اذهب theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	إذا (buf == عدم)
	{
	    إذا (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	وإلا
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    وموندل_سمون = vgr_سمون_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_دلیلونه->spat, &cmd_دلیلونه->regmatch,
		    &cmd_دلیلونه->tomatch, duplicate_name, cmd_دلیلونه->flags);

	    إذا (using_dummy)
	    {
		إذا (وموندل_سمون && *لومړی_سمون_buf == عدم)
		    *لومړی_سمون_buf = buf;
		إذا (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_بفر(buf, dirname_پیل);
		    buf = عدم;
		}
		وإلا إذا ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    إذا (!وموندل_سمون)
		    {
			wipe_dummy_بفر(buf, dirname_پیل);
			buf = عدم;
		    }
		    وإلا إذا (buf != *لومړی_سمون_buf
					|| (cmd_دلیلونه->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_بفر(buf, dirname_پیل);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = عدم;
		    }
		}

		إذا (buf != عدم)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    إذا (buf == *لومړی_سمون_buf
			    && *موخه_dir == عدم
			    && STRCMP(dirname_پیل, dirname_now) != 0)
			*موخه_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    إذا (curbuf == buf)
		    {
# إذا defined(FEAT_SYN_HL)
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

    حالت = OK;

theend:
    vim_free(dirname_now);
    vim_free(dirname_پیل);
    أرجع حالت;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    فراغ
ex_vimgrep(exarg_T *eap)
{
    vgr_دلیلونه_T	دلیلونه;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = عدم;
    صحيح		redraw_for_dummy = FALSE;
    buf_T	*لومړی_سمون_buf = عدم;
    char_u	*موخه_dir = عدم;
    char_u	*au_name =  عدم;
    صحيح		حالت;

    إذا (!check_can_سېټ_curbuf_forceit(eap->forceit))
	أرجع;

    au_name = vgr_get_auname(eap->cmdidx);
    إذا (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	إذا (aborting())
	    أرجع;
# endif
    }

    qi = qf_cmd_get_or_alloc_ډېری(eap, &wp);
    إذا (qi == عدم)
	أرجع;

    إذا (vgr_process_دلیلونه(eap, &دلیلونه) == FAIL)
	اذهب theend;

    إذا ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_ډېری_تشه(qi))
	// make place for a new list
	qf_new_list(qi, دلیلونه.qf_title);

    incr_quickfix_busy();

    حالت = vgr_process_files(wp, qi, &دلیلونه, &redraw_for_dummy,
						&لومړی_سمون_buf, &موخه_dir);
    إذا (حالت != OK)
    {
	FreeWild(دلیلونه.fcount, دلیلونه.fnames);
	decr_quickfix_busy();
	اذهب theend;
    }

    FreeWild(دلیلونه.fcount, دلیلونه.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_پیل;
    qfl->qf_شاخص = 1;
    qf_list_بدل_شوی(qfl);

    qf_update_بفر(qi, عدم);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    إذا (au_name != عدم)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    إذا (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	اذهب theend;
    }

    // Jump to first match.
    إذا (!qf_list_تشه(qf_get_curlist(qi)))
    {
	إذا ((دلیلونه.flags & VGR_NOJUMP) == 0)
	    vgr_jump_منزل_سمون(qi, eap->forceit, &redraw_for_dummy,
		    لومړی_سمون_buf, موخه_dir);
    }
    وإلا
	semsg(_(e_no_سمون_str_2), دلیلونه.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    إذا (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# وإلا
	redraw_later(UPD_NOT_VALID);
# endif
    }

theend:
    vim_free(دلیلونه.qf_title);
    vim_free(موخه_dir);
    vim_regfree(دلیلونه.regmatch.regprog);
}

/*
 * Restore current working directory to "dirname_start" if they differ, taking
 * into account whether it is set locally or globally.
 */
    ساكن فراغ
restore_پیل_dir(char_u *dirname_پیل)
{
    char_u *dirname_now = alloc(MAXPATHL);

    إذا (dirname_now == عدم)
	أرجع;

    mch_dirname(dirname_now, MAXPATHL);
    إذا (STRCMP(dirname_پیل, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_ډګر(ea);
	ea.arg = dirname_پیل;
	ea.cmdidx = (curwin->w_localdir == عدم) ? CMD_cd : CMD_lcd;
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
    ساكن buf_T *
بارول_dummy_بفر(
    char_u	*fname,
    char_u	*dirname_پیل,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_منزل_wipe;
    صحيح		failed = TRUE;
    aco_save_T	aco;
    صحيح		readfile_پایله;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(عدم, عدم, (linenr_T)1, BLN_DUMMY);
    إذا (newbuf == عدم)
	أرجع عدم;
    سېټ_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    إذا (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	إذا (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (فراغ)setfname(curbuf, fname, عدم, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_بدلون(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_منزل_wipe.br_buf = عدم;
	    readfile_پایله = readfile(fname, عدم,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			عدم, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    إذا (readfile_پایله == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		إذا (curbuf != newbuf)
		{
		    // Bloody autocommands changed the buffer!  Can happen when
		    // using netrw and editing a remote file.  Use the current
		    // buffer instead, delete the dummy one after restoring the
		    // window stuff.
		    سېټ_bufref(&newbuf_منزل_wipe, newbuf);
		    newbuf = curbuf;
		}
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);

	    إذا (newbuf_منزل_wipe.br_buf != عدم && bufref_valid(&newbuf_منزل_wipe))
	    {
		بلاک_autocmds();
		wipe_dummy_بفر(newbuf_منزل_wipe.br_buf, عدم);
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
    restore_پیل_dir(dirname_پیل);

    إذا (!bufref_valid(&newbufref))
	أرجع عدم;
    إذا (failed)
    {
	wipe_dummy_بفر(newbuf, dirname_پیل);
	أرجع عدم;
    }
    أرجع newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    ساكن فراغ
wipe_dummy_بفر(buf_T *buf, char_u *dirname_پیل)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    طالما (buf->b_nwindows > 0)
    {
	صحيح	    did_one = FALSE;
	win_T	    *wp;

	إذا (firstwin->w_بل != عدم)
	    FOR_ALL_WINDOWS(wp)
		إذا (wp->w_بفر == buf)
		{
		    إذا (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    اكسر;
		}
	إذا (!did_one)
	    اذهب fail;
    }

    إذا (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# إذا defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_بفر(buf, TRUE);

# إذا defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	إذا (dirname_پیل != عدم)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_پیل_dir(dirname_پیل);

	أرجع;
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
    ساكن فراغ
unload_dummy_بفر(buf_T *buf, char_u *dirname_پیل)
{
    إذا (curbuf == buf)		// safety check
	أرجع;

    close_بفر(عدم, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_پیل_dir(dirname_پیل);
}

# إذا defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    ساكن صحيح
get_qfline_توکي(qfline_T *qfp, list_T *list)
{
    صحيح		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    إذا (bufnum != 0 && (buflist_findnr(bufnum) == عدم))
	bufnum = 0;

    إذا ((dict = dict_alloc()) == عدم)
	أرجع FAIL;
    إذا (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	أرجع FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    إذا (dict_جمع_کړه_شمېره(dict, "bufnr", (اوږد)bufnum) == FAIL
	    || dict_جمع_کړه_شمېره(dict, "lnum",     (اوږد)qfp->qf_lnum) == FAIL
	    || dict_جمع_کړه_شمېره(dict, "end_lnum", (اوږد)qfp->qf_پای_ټکی_lnum) == FAIL
	    || dict_جمع_کړه_شمېره(dict, "col",      (اوږد)qfp->qf_ستون) == FAIL
	    || dict_جمع_کړه_شمېره(dict, "end_col",  (اوږد)qfp->qf_پای_ټکی_ستون) == FAIL
	    || dict_جمع_کړه_شمېره(dict, "vcol",     (اوږد)qfp->qf_viscol) == FAIL
	    || dict_جمع_کړه_شمېره(dict, "nr",       (اوږد)qfp->qf_nr) == FAIL
	    || dict_جمع_کړه_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_جمع_کړه_string(dict, "pattern", qfp->qf_نمونه) == FAIL
	    || dict_جمع_کړه_string(dict, "text", qfp->qf_متن) == FAIL
	    || dict_جمع_کړه_string_len(dict, "type", buf, (صحيح)buflen) == FAIL
	    || (qfp->qf_user_معلومات.v_type != VAR_UNKNOWN
		&& dict_جمع_کړه_tv(dict, "user_data", &qfp->qf_user_معلومات) == FAIL )
	    || dict_جمع_کړه_شمېره(dict, "valid", (اوږد)qfp->qf_valid) == FAIL)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    ساكن صحيح
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	صحيح		qf_idx,
	صحيح		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحيح		i;

    إذا (qi == عدم)
    {
	qi = ql_info;
	إذا (wp != عدم)
	    qi = GET_LOC_LIST(wp);
	إذا (qi == عدم)
	    أرجع FAIL;

    }

    إذا (eidx < 0)
	أرجع OK;

    إذا (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    إذا (qf_idx >= qi->qf_listcount)
	أرجع FAIL;

    qfl = qf_get_list(qi, qf_idx);
    إذا (qf_list_تشه(qfl))
	أرجع FAIL;

    FOR_ALL_QFL_توکي(qfl, qfp, i)
    {
	إذا (eidx > 0)
	{
	    إذا (eidx == i)
		أرجع get_qfline_توکي(qfp, list);
	}
	وإلا إذا (get_qfline_توکي(qfp, list) == FAIL)
	    أرجع FAIL;
    }

    أرجع OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
تعداد {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_توکي	= 0x2,
    QF_GETLIST_NR	= 0x4,
    QF_GETLIST_WINID	= 0x8,
    QF_GETLIST_CONTEXT	= 0x10,
    QF_GETLIST_ID	= 0x20,
    QF_GETLIST_IDX	= 0x40,
    QF_GETLIST_اندازه	= 0x80,
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
    ساكن صحيح
qf_get_list_from_کرښې(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    صحيح		حالت = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    إذا (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == عدم)
	أرجع FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    إذا ((efm_di = dict_پیدا_کړه(what, (char_u *)"efm", -1)) != عدم)
    {
	إذا (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == عدم)
	    أرجع FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    إذا (l == عدم)
	أرجع FAIL;

    qi = qf_alloc_ډېری(QFLT_دننی, 1);
    إذا (qi != عدم)
    {
	إذا (qf_init_ext(qi, 0, عدم, عدم, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, عدم, عدم) > 0)
	{
	    (فراغ)get_errorlist(qi, عدم, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_جمع_کړه_list(retdict, "items", l);
    حالت = OK;

    أرجع حالت;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    ساكن صحيح
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    إذا (qi == عدم)
	أرجع 0;
    win = qf_پیدا_کړه_win(qi);
    إذا (win != عدم)
	أرجع win->w_id;
    أرجع 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    ساكن صحيح
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    صحيح	bufnum = 0;

    إذا (qi != عدم && buflist_findnr(qi->qf_bufnr) != عدم)
	bufnum = qi->qf_bufnr;

    أرجع dict_جمع_کړه_شمېره(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    ساكن صحيح
qf_getprop_keys2flags(dict_T *what, صحيح loclist)
{
    صحيح		flags = QF_GETLIST_NONE;

    إذا (dict_has_کلی(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	إذا (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    إذا (dict_has_کلی(what, "title"))
	flags |= QF_GETLIST_TITLE;

    إذا (dict_has_کلی(what, "nr"))
	flags |= QF_GETLIST_NR;

    إذا (dict_has_کلی(what, "winid"))
	flags |= QF_GETLIST_WINID;

    إذا (dict_has_کلی(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    إذا (dict_has_کلی(what, "id"))
	flags |= QF_GETLIST_ID;

    إذا (dict_has_کلی(what, "items"))
	flags |= QF_GETLIST_توکي;

    إذا (dict_has_کلی(what, "idx"))
	flags |= QF_GETLIST_IDX;

    إذا (dict_has_کلی(what, "size"))
	flags |= QF_GETLIST_اندازه;

    إذا (dict_has_کلی(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    إذا (loclist && dict_has_کلی(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    إذا (dict_has_کلی(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    إذا (dict_has_کلی(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    أرجع flags;
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
    ساكن صحيح
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    صحيح		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    إذا ((di = dict_پیدا_کړه(what, (char_u *)"nr", -1)) != عدم)
    {
	// Use the specified quickfix/location list
	إذا (di->di_tv.v_type == VAR_شمېره)
	{
	    // for zero use the current list
	    إذا (di->di_tv.vval.v_شمېره != 0)
	    {
		qf_idx = di->di_tv.vval.v_شمېره - 1;
		إذا (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	وإلا إذا (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != عدم
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	وإلا
	    qf_idx = INVALID_QFIDX;
    }

    إذا ((di = dict_پیدا_کړه(what, (char_u *)"id", -1)) != عدم)
    {
	// Look for a list with the specified id
	إذا (di->di_tv.v_type == VAR_شمېره)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    إذا (di->di_tv.vval.v_شمېره != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_شمېره);
	}
	وإلا
	    qf_idx = INVALID_QFIDX;
    }

    أرجع qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    ساكن صحيح
qf_getprop_defaults(qf_info_T *qi, صحيح flags, صحيح locstack, dict_T *retdict)
{
    صحيح		حالت = OK;

    إذا (flags & QF_GETLIST_TITLE)
	حالت = dict_جمع_کړه_string_len(retdict, "title", (char_u *)"", 0);
    إذا ((حالت == OK) && (flags & QF_GETLIST_توکي))
    {
	list_T	*l = list_alloc();
	إذا (l != عدم)
	    حالت = dict_جمع_کړه_list(retdict, "items", l);
	وإلا
	    حالت = FAIL;
    }
    إذا ((حالت == OK) && (flags & QF_GETLIST_NR))
	حالت = dict_جمع_کړه_شمېره(retdict, "nr", 0);
    إذا ((حالت == OK) && (flags & QF_GETLIST_WINID))
	حالت = dict_جمع_کړه_شمېره(retdict, "winid", qf_winid(qi));
    إذا ((حالت == OK) && (flags & QF_GETLIST_CONTEXT))
	حالت = dict_جمع_کړه_string_len(retdict, "context", (char_u *)"", 0);
    إذا ((حالت == OK) && (flags & QF_GETLIST_ID))
	حالت = dict_جمع_کړه_شمېره(retdict, "id", 0);
    إذا ((حالت == OK) && (flags & QF_GETLIST_IDX))
	حالت = dict_جمع_کړه_شمېره(retdict, "idx", 0);
    إذا ((حالت == OK) && (flags & QF_GETLIST_اندازه))
	حالت = dict_جمع_کړه_شمېره(retdict, "size", 0);
    إذا ((حالت == OK) && (flags & QF_GETLIST_TICK))
	حالت = dict_جمع_کړه_شمېره(retdict, "changedtick", 0);
    إذا ((حالت == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	حالت = dict_جمع_کړه_شمېره(retdict, "filewinid", 0);
    إذا ((حالت == OK) && (flags & QF_GETLIST_QFBUFNR))
	حالت = qf_getprop_qfbufnr(qi, retdict);
    إذا ((حالت == OK) && (flags & QF_GETLIST_QFTF))
	حالت = dict_جمع_کړه_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    أرجع حالت;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    ساكن صحيح
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    أرجع dict_جمع_کړه_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    ساكن صحيح
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    صحيح winid = 0;

    إذا (wp != عدم && IS_LL_کړکۍ(wp))
    {
	win_T	*ll_wp = qf_پیدا_کړه_win_with_loclist(qi);
	إذا (ll_wp != عدم)
	    winid = ll_wp->w_id;
    }

    أرجع dict_جمع_کړه_شمېره(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    ساكن صحيح
qf_getprop_توکي(qf_info_T *qi, صحيح qf_idx, صحيح eidx, dict_T *retdict)
{
    صحيح		حالت = OK;
    list_T	*l = list_alloc();
    إذا (l != عدم)
    {
	(فراغ)get_errorlist(qi, عدم, qf_idx, eidx, l);
	dict_جمع_کړه_list(retdict, "items", l);
    }
    وإلا
	حالت = FAIL;

    أرجع حالت;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    ساكن صحيح
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    صحيح		حالت;
    dictitem_T	*di;

    إذا (qfl->qf_ctx != عدم)
    {
	di = dictitem_alloc((char_u *)"context");
	إذا (di != عدم)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    حالت = dict_جمع_کړه(retdict, di);
	    إذا (حالت == FAIL)
		dictitem_free(di);
	}
	وإلا
	    حالت = FAIL;
    }
    وإلا
	حالت = dict_جمع_کړه_string_len(retdict, "context", (char_u *)"", 0);

    أرجع حالت;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    ساكن صحيح
qf_getprop_idx(qf_list_T *qfl, صحيح eidx, dict_T *retdict)
{
    إذا (eidx == 0)
    {
	eidx = qfl->qf_شاخص;
	إذا (qf_list_تشه(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    أرجع dict_جمع_کړه_شمېره(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    ساكن صحيح
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    صحيح		حالت;

    إذا (qfl->qf_qftf_cb.cb_name != عدم)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	حالت = dict_جمع_کړه_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    وإلا
	حالت = dict_جمع_کړه_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    أرجع حالت;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    ساكن صحيح
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    صحيح		حالت = OK;
    صحيح		qf_idx = INVALID_QFIDX;
    صحيح		eidx = 0;
    dictitem_T	*di;
    صحيح		flags = QF_GETLIST_NONE;

    إذا ((di = dict_پیدا_کړه(what, (char_u *)"lines", -1)) != عدم)
	أرجع qf_get_list_from_کرښې(what, di, retdict);

    إذا (wp != عدم)
	qi = GET_LOC_LIST(wp);
    وإلا إذا (qi == عدم)
	أرجع FAIL;

    flags = qf_getprop_keys2flags(what, (wp != عدم));

    إذا (!qf_ډېری_تشه(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    إذا (qf_ډېری_تشه(qi) || qf_idx == INVALID_QFIDX)
	أرجع qf_getprop_defaults(qi, flags, wp != عدم, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    إذا ((di = dict_پیدا_کړه(what, (char_u *)"idx", -1)) != عدم)
    {
	إذا (di->di_tv.v_type != VAR_شمېره)
	    أرجع FAIL;
	eidx = di->di_tv.vval.v_شمېره;
    }

    إذا (flags & QF_GETLIST_TITLE)
	حالت = qf_getprop_title(qfl, retdict);
    إذا ((حالت == OK) && (flags & QF_GETLIST_NR))
	حالت = dict_جمع_کړه_شمېره(retdict, "nr", qf_idx + 1);
    إذا ((حالت == OK) && (flags & QF_GETLIST_WINID))
	حالت = dict_جمع_کړه_شمېره(retdict, "winid", qf_winid(qi));
    إذا ((حالت == OK) && (flags & QF_GETLIST_توکي))
	حالت = qf_getprop_توکي(qi, qf_idx, eidx, retdict);
    إذا ((حالت == OK) && (flags & QF_GETLIST_CONTEXT))
	حالت = qf_getprop_ctx(qfl, retdict);
    إذا ((حالت == OK) && (flags & QF_GETLIST_ID))
	حالت = dict_جمع_کړه_شمېره(retdict, "id", qfl->qf_id);
    إذا ((حالت == OK) && (flags & QF_GETLIST_IDX))
	حالت = qf_getprop_idx(qfl, eidx, retdict);
    إذا ((حالت == OK) && (flags & QF_GETLIST_اندازه))
	حالت = dict_جمع_کړه_شمېره(retdict, "size", qfl->qf_شمېر);
    إذا ((حالت == OK) && (flags & QF_GETLIST_TICK))
	حالت = dict_جمع_کړه_شمېره(retdict, "changedtick", qfl->qf_changedtick);
    إذا ((حالت == OK) && (wp != عدم) && (flags & QF_GETLIST_FILEWINID))
	حالت = qf_getprop_filewinid(wp, qi, retdict);
    إذا ((حالت == OK) && (flags & QF_GETLIST_QFBUFNR))
	حالت = qf_getprop_qfbufnr(qi, retdict);
    إذا ((حالت == OK) && (flags & QF_GETLIST_QFTF))
	حالت = qf_getprop_qftf(qfl, retdict);

    أرجع حالت;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    ساكن صحيح
qf_جمع_کړه_ثبت_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	صحيح		لومړی_ثبت,
	صحيح		*valid_ثبت)
{
    ساكن صحيح	did_bufnr_emsg;
    char_u	*filename, *module, *نمونه, *متن, *type;
    صحيح		bufnum, valid, حالت, ستون, پای_ټکی_ستون, vcol, nr;
    اوږد	lnum, پای_ټکی_lnum;

    إذا (لومړی_ثبت)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (صحيح)dict_get_شمېره(d, "bufnr");
    lnum = (صحيح)dict_get_شمېره(d, "lnum");
    پای_ټکی_lnum = (صحيح)dict_get_شمېره(d, "end_lnum");
    ستون = (صحيح)dict_get_شمېره(d, "col");
    پای_ټکی_ستون = (صحيح)dict_get_شمېره(d, "end_col");
    vcol = (صحيح)dict_get_شمېره(d, "vcol");
    nr = (صحيح)dict_get_شمېره(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    نمونه = dict_get_string(d, "pattern", TRUE);
    متن = dict_get_string(d, "text", TRUE);
    إذا (متن == عدم)
	متن = vim_strsave((char_u *)"");
    typval_T user_معلومات;
    user_معلومات.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_معلومات);

    valid = TRUE;
    إذا ((filename == عدم && bufnum == 0) || (lnum == 0 && نمونه == عدم))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    إذا (bufnum != 0 && (buflist_findnr(bufnum) == عدم))
    {
	إذا (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_بفر_nr_not_وموندل), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    إذا (dict_has_کلی(d, "valid"))
	valid = (صحيح)dict_get_bool(d, "valid", FALSE);

    حالت =  qf_جمع_کړه_ثبت(qfl,
			عدم,		// dir
			filename,
			module,
			bufnum,
			متن,
			lnum,
			پای_ټکی_lnum,
			ستون,
			پای_ټکی_ستون,
			vcol,		// vis_col
			نمونه,	// search pattern
			nr,
			type == عدم ? NUL : *type,
			&user_معلومات,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(نمونه);
    vim_free(متن);
    vim_free(type);
    clear_tv(&user_معلومات);

    إذا (valid)
	*valid_ثبت = TRUE;

    أرجع حالت;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    ساكن صحيح
ثبت_is_closer_منزل_موخه(
	qfline_T	*ثبت,
	qfline_T	*other_ثبت,
	صحيح		موخه_fnum,
	صحيح		موخه_lnum,
	صحيح		موخه_ستون)
{
    // First, compare entries to target file.
    إذا (!موخه_fnum)
	// Without a target file, we can't know which is closer.
	أرجع FALSE;

    صحيح is_موخه_file = ثبت->qf_fnum && ثبت->qf_fnum == موخه_fnum;
    صحيح other_is_موخه_file = other_ثبت->qf_fnum && other_ثبت->qf_fnum == موخه_fnum;
    إذا (!is_موخه_file && other_is_موخه_file)
	أرجع FALSE;
    وإلا إذا (is_موخه_file && !other_is_موخه_file)
	أرجع TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    إذا (!موخه_lnum)
	// Without a target line number, we can't know which is closer.
	أرجع FALSE;

    صحيح کرښه_واټن = ثبت->qf_lnum ? labs(ثبت->qf_lnum - موخه_lnum) : لوی_عدد;
    صحيح other_کرښه_واټن = other_ثبت->qf_lnum ? labs(other_ثبت->qf_lnum - موخه_lnum) : لوی_عدد;
    إذا (کرښه_واټن > other_کرښه_واټن)
	أرجع FALSE;
    وإلا إذا (کرښه_واټن < other_کرښه_واټن)
	أرجع TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    إذا (!موخه_ستون)
	// Without a target column, we can't know which is closer.
	أرجع FALSE;

    صحيح ستنه_واټن = ثبت->qf_ستون ? مطلق_قیمت(ثبت->qf_ستون - موخه_ستون) : لوی_عدد;
    صحيح other_ستنه_واټن = other_ثبت->qf_ستون ? مطلق_قیمت(other_ثبت->qf_ستون - موخه_ستون): لوی_عدد;
    إذا (ستنه_واټن > other_ستنه_واټن)
	أرجع FALSE;
    وإلا إذا (ستنه_واټن < other_ستنه_واټن)
	أرجع TRUE;

    // It's a complete tie! The exact same file, line, and column.
    أرجع FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    ساكن صحيح
qf_جمع_کړه_entries(
	qf_info_T	*qi,
	صحيح		qf_idx,
	list_T		*list,
	char_u		*title,
	صحيح		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_وروستی = عدم;
    صحيح		retval = OK;
    صحيح		valid_ثبت = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    صحيح prev_fnum = 0;
    صحيح prev_lnum = 0;
    صحيح prev_ستون = 0;
    إذا (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_ستون = qfl->qf_ptr->qf_ستون;
    }

    صحيح select_لومړی_ثبت = FALSE;
    صحيح select_تر_ټولو_نږدې_ثبت = FALSE;

    إذا (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_لومړی_ثبت = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    وإلا إذا (action == 'a')
    {
	إذا (qf_list_تشه(qfl))
	    // Appending to empty list, select first entry.
	    select_لومړی_ثبت = TRUE;
	وإلا
	    // Adding to existing list, use last entry.
	    old_وروستی = qfl->qf_وروستی;
    }
    وإلا إذا (action == 'r')
    {
	select_لومړی_ثبت = TRUE;
	qf_free_توکي(qfl);
	qf_store_title(qfl, title);
    }
    وإلا إذا (action == 'u')
    {
	select_تر_ټولو_نږدې_ثبت = TRUE;
	qf_free_توکي(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *ثبت_منزل_select = عدم;
    صحيح ثبت_منزل_select_شاخص = 0;

    FOR_ALL_LIST_توکي(list, li)
    {
	إذا (li->li_tv.v_type != VAR_DICT)
	    استمر; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	إذا (d == عدم)
	    استمر;

	retval = qf_جمع_کړه_ثبت_from_dict(qfl, d, li == list->lv_لومړی,
								&valid_ثبت);
	إذا (retval == QF_FAIL)
	    اكسر;

	qfline_T *ثبت = qfl->qf_وروستی;
	إذا (
	    (select_لومړی_ثبت && ثبت_منزل_select == عدم) ||
	    (select_تر_ټولو_نږدې_ثبت &&
		(ثبت_منزل_select == عدم ||
		 ثبت_is_closer_منزل_موخه(ثبت, ثبت_منزل_select, prev_fnum,
					   prev_lnum, prev_ستون))))
	{
	    ثبت_منزل_select = ثبت;
	    ثبت_منزل_select_شاخص = qfl->qf_شمېر;
	}
    }

    // Check if any valid error entries are added to the list.
    إذا (valid_ثبت)
	qfl->qf_nonevalid = FALSE;
    وإلا إذا (qfl->qf_شاخص == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    إذا (ثبت_منزل_select)
    {
	qfl->qf_ptr = ثبت_منزل_select;
	qfl->qf_شاخص = ثبت_منزل_select_شاخص;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_بفر(qi, old_وروستی);

    أرجع retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    ساكن صحيح
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	صحيح		action,
	صحيح		*newlist)
{
    dictitem_T	*di;
    صحيح		qf_idx = qi->qf_curlist;    // default is the current list

    إذا ((di = dict_پیدا_کړه(what, (char_u *)"nr", -1)) != عدم)
    {
	// Use the specified quickfix/location list
	إذا (di->di_tv.v_type == VAR_شمېره)
	{
	    // for zero use the current list
	    إذا (di->di_tv.vval.v_شمېره != 0)
		qf_idx = di->di_tv.vval.v_شمېره - 1;

	    إذا ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_ډېری_تشه(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    وإلا إذا (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		أرجع INVALID_QFIDX;
	    وإلا إذا (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	وإلا إذا (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != عدم
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    إذا (!qf_ډېری_تشه(qi))
		qf_idx = qi->qf_listcount - 1;
	    وإلا إذا (*newlist)
		qf_idx = 0;
	    وإلا
		أرجع INVALID_QFIDX;
	}
	وإلا
	    أرجع INVALID_QFIDX;
    }

    إذا (!*newlist && (di = dict_پیدا_کړه(what, (char_u *)"id", -1)) != عدم)
    {
	// Use the quickfix/location list with the specified id
	إذا (di->di_tv.v_type != VAR_شمېره)
	    أرجع INVALID_QFIDX;

	أرجع qf_id2nr(qi, di->di_tv.vval.v_شمېره);
    }

    أرجع qf_idx;
}

/*
 * Set the quickfix list title.
 */
    ساكن صحيح
qf_setprop_title(qf_info_T *qi, صحيح qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    إذا (di->di_tv.v_type != VAR_STRING)
	أرجع FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    إذا (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    أرجع OK;
}

/*
 * Set quickfix list items/entries.
 */
    ساكن صحيح
qf_setprop_توکي(qf_info_T *qi, صحيح qf_idx, dictitem_T *di, صحيح action)
{
    صحيح		retval = FAIL;
    char_u	*title_save;

    إذا (di->di_tv.v_type != VAR_LIST)
	أرجع FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_جمع_کړه_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    أرجع retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    ساكن صحيح
qf_setprop_توکي_from_کرښې(
	qf_info_T	*qi,
	صحيح		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	صحيح		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    صحيح		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    إذا ((efm_di = dict_پیدا_کړه(what, (char_u *)"efm", -1)) != عدم)
    {
	إذا (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == عدم)
	    أرجع FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    إذا (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == عدم)
	أرجع FAIL;

    إذا (action == 'r' || action == 'u')
	qf_free_توکي(&qi->qf_lists[qf_idx]);
    إذا (qf_init_ext(qi, qf_idx, عدم, عدم, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, عدم, عدم) >= 0)
	retval = OK;

    أرجع retval;
}

/*
 * Set quickfix list context.
 */
    ساكن صحيح
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    إذا (ctx != عدم)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    أرجع OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    ساكن صحيح
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    صحيح		denote = FALSE;
    صحيح		newidx;
    صحيح		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    إذا (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != عدم
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_شمېر;
    وإلا
    {
	// Otherwise use the specified index
	newidx = tv_get_شمېره_chk(&di->di_tv, &denote);
	إذا (denote)
	    أرجع FAIL;
    }

    إذا (newidx < 1)		// sanity check
	أرجع FAIL;
    إذا (newidx > qfl->qf_شمېر)
	newidx = qfl->qf_شمېر;

    old_qfidx = qfl->qf_شاخص;
    qf_ptr = get_nth_ثبت(qfl, newidx, &newidx);
    إذا (qf_ptr == عدم)
	أرجع FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_شاخص = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    إذا (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    أرجع OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    ساكن صحيح
qf_setprop_qftf(qf_info_T *qi ناکارول_شوی, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    إذا (check_restricted() || check_secure())
	أرجع FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    إذا (cb.cb_name == عدم || *cb.cb_name == NUL)
	أرجع OK;
    سېټ_callback(&qfl->qf_qftf_cb, &cb);
    أرجع OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    ساكن صحيح
qf_سېټ_properties(qf_info_T *qi, dict_T *what, صحيح action, char_u *title)
{
    dictitem_T	*di;
    صحيح		retval = FAIL;
    صحيح		qf_idx;
    صحيح		newlist = FALSE;
    qf_list_T	*qfl;

    إذا (action == ' ' || qf_ډېری_تشه(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    إذا (qf_idx == INVALID_QFIDX)	// List not found
	أرجع FAIL;

    إذا (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    إذا ((di = dict_پیدا_کړه(what, (char_u *)"title", -1)) != عدم)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    إذا ((di = dict_پیدا_کړه(what, (char_u *)"items", -1)) != عدم)
	retval = qf_setprop_توکي(qi, qf_idx, di, action);
    إذا ((di = dict_پیدا_کړه(what, (char_u *)"lines", -1)) != عدم)
	retval = qf_setprop_توکي_from_کرښې(qi, qf_idx, what, di, action);
    إذا ((di = dict_پیدا_کړه(what, (char_u *)"context", -1)) != عدم)
	retval = qf_setprop_context(qfl, di);
    إذا ((di = dict_پیدا_کړه(what, (char_u *)"idx", -1)) != عدم)
	retval = qf_setprop_curidx(qi, qfl, di);
    إذا ((di = dict_پیدا_کړه(what, (char_u *)"quickfixtextfunc", -1)) != عدم)
	retval = qf_setprop_qftf(qi, qfl, di);

    إذا (newlist || retval == OK)
	qf_list_بدل_شوی(qfl);
    إذا (newlist)
	qf_update_بفر(qi, عدم);

    أرجع retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    ساكن فراغ
qf_free_ډېری(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_پیدا_کړه_win(qi);
    win_T	*llwin = عدم;

    إذا (qfwin != عدم)
    {
	// If the quickfix/location list window is open, then clear it
	إذا (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_بفر(qi, عدم);
    }

    إذا (wp != عدم && IS_LL_کړکۍ(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_پیدا_کړه_win_with_loclist(qi);
	إذا (llwin != عدم)
	    wp = llwin;
    }

    qf_free_all(wp);
    إذا (wp == عدم)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    وإلا إذا (qfwin != عدم)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_ډېری(QFLT_LOCATION, wp->w_p_lhi);

	إذا (new_ll != عدم)
	{
	    new_ll->qf_bufnr = qfwin->w_بفر->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    إذا (wp != qfwin)
		win_سېټ_loclist(wp, new_ll);
	}
    }
}

/*
 * Populate the quickfix list with the items supplied in the list
 * of dictionaries. "title" will be copied to w:quickfix_title.
 * "action" is 'a' for add, 'r' for replace, 'u' for update.  Otherwise
 * create a new list. When "what" is not NULL then only set some properties.
 */
    صحيح
سېټ_errorlist(
	win_T	*wp,
	list_T	*list,
	صحيح	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    صحيح		retval = OK;

    إذا (wp != عدم)
	qi = ll_get_or_alloc_list(wp);
    وإلا
	qi = ql_info;
    إذا (qi == عدم)
	أرجع FAIL;

    إذا (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_ډېری(wp, qi);
	أرجع OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    إذا (list->lv_len != 0 && what != عدم)
    {
	semsg(_(e_invalid_دلیل_str),
			 _("cannot have both a list and a \"what\" argument"));
	أرجع FAIL;
    }

    incr_quickfix_busy();

    إذا (what != عدم)
	retval = qf_سېټ_properties(qi, what, action, title);
    وإلا
    {
	retval = qf_جمع_کړه_entries(qi, qi->qf_curlist, list, title, action);
	إذا (retval == OK)
	    qf_list_بدل_شوی(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    أرجع retval;
}

ساكن صحيح mark_quickfix_user_معلومات(qf_info_T *qi, صحيح copyID)
{
    صحيح abort = FALSE;
    لكل (صحيح i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	إذا (!qfl->qf_has_user_معلومات)
	    استمر;
	qfline_T *qfp;
	صحيح j;
	FOR_ALL_QFL_توکي(qfl, qfp, j)
	{
	    typval_T* user_معلومات = &qfp->qf_user_معلومات;
	    إذا (user_معلومات != عدم && user_معلومات->v_type != VAR_شمېره
		&& user_معلومات->v_type != VAR_STRING && user_معلومات->v_type != VAR_FLOAT)
		abort = abort || سېټ_ref_دننه_توکی(user_معلومات, copyID, عدم, عدم, عدم);
	}
    }
    أرجع abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    ساكن صحيح
mark_quickfix_ctx(qf_info_T *qi, صحيح copyID)
{
    صحيح		i;
    صحيح		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    لكل (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	إذا (ctx != عدم && ctx->v_type != VAR_شمېره
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || سېټ_ref_دننه_توکی(ctx, copyID, عدم, عدم, عدم);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || سېټ_ref_دننه_callback(cb, copyID);
    }

    أرجع abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    صحيح
سېټ_ref_دننه_quickfix(صحيح copyID)
{
    صحيح		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    إذا (ql_info == عدم)
	أرجع TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    إذا (abort)
	أرجع abort;

    abort = mark_quickfix_user_معلومات(ql_info, copyID);
    إذا (abort)
	أرجع abort;

    abort = سېټ_ref_دننه_callback(&qftf_cb, copyID);
    إذا (abort)
	أرجع abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	إذا (win->w_llist != عدم)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    إذا (abort)
		أرجع abort;

	    abort = mark_quickfix_user_معلومات(win->w_llist, copyID);
	    إذا (abort)
		أرجع abort;
	}
	إذا (IS_LL_کړکۍ(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    إذا (abort)
		أرجع abort;

	    abort = mark_quickfix_user_معلومات(win->w_llist_ref, copyID);
	    إذا (abort)
		أرجع abort;
	}
    }

    أرجع abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    ساكن char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_cbuffer:	أرجع (char_u *)"cbuffer";
	حالة CMD_cgetbuffer:	أرجع (char_u *)"cgetbuffer";
	حالة CMD_caddbuffer:	أرجع (char_u *)"caddbuffer";
	حالة CMD_lbuffer:	أرجع (char_u *)"lbuffer";
	حالة CMD_lgetbuffer:	أرجع (char_u *)"lgetbuffer";
	حالة CMD_laddbuffer:	أرجع (char_u *)"laddbuffer";
	افتراضي:		أرجع عدم;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    ساكن صحيح
cbuffer_process_دلیلونه(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = عدم;

    إذا (*eap->arg == NUL)
	buf = curbuf;
    وإلا إذا (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((حرف *)eap->arg));

    إذا (buf == عدم)
    {
	emsg(_(e_invalid_دلیل));
	أرجع FAIL;
    }

    إذا (buf->b_ml.ml_mfp == عدم)
    {
	emsg(_(e_بفر_is_not_loaded));
	أرجع FAIL;
    }

    إذا (eap->addr_شمېر == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_کرښه_شمېر;
    }

    إذا (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_کرښه_شمېر
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_کرښه_شمېر)
    {
	emsg(_(e_invalid_range));
	أرجع FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    أرجع OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    فراغ
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = عدم;
    qf_info_T	*qi;
    char_u	*au_name = عدم;
    صحيح		res;
    int_u	save_qfid;
    win_T	*wp = عدم;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    إذا (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	إذا (aborting())
	    أرجع;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_ډېری(eap, &wp);
    إذا (qi == عدم)
	أرجع;

    إذا (cbuffer_process_دلیلونه(eap, &buf, &line1, &line2) == FAIL)
	أرجع;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    إذا (buf->b_sfname)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "%s (%s)",
		(حرف *)qf_title, (حرف *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, عدم, buf, عدم, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, عدم);
    إذا (qf_ډېری_تشه(qi))
    {
	decr_quickfix_busy();
	أرجع;
    }
    إذا (res >= 0)
	qf_list_بدل_شوی(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    إذا (au_name != عدم)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	إذا (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    إذا (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_لومړی(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# إذا defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_cexpr:	    أرجع (char_u *)"cexpr";
	حالة CMD_cgetexpr:  أرجع (char_u *)"cgetexpr";
	حالة CMD_caddexpr:  أرجع (char_u *)"caddexpr";
	حالة CMD_lexpr:	    أرجع (char_u *)"lexpr";
	حالة CMD_lgetexpr:  أرجع (char_u *)"lgetexpr";
	حالة CMD_laddexpr:  أرجع (char_u *)"laddexpr";
	افتراضي:	    أرجع عدم;
    }
}

    صحيح
trigger_cexpr_autocmd(صحيح cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    إذا (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	إذا (aborting())
	    أرجع FAIL;
    }
    أرجع OK;
}

    صحيح
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = عدم;

    qi = qf_cmd_get_or_alloc_ډېری(eap, &wp);
    إذا (qi == عدم)
	أرجع FAIL;

    إذا ((tv->v_type == VAR_STRING && tv->vval.v_string != عدم)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != عدم))
    {
	صحيح	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, عدم, عدم, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), عدم);
	إذا (qf_ډېری_تشه(qi))
	{
	    decr_quickfix_busy();
	    أرجع FAIL;
	}
	إذا (res >= 0)
	    qf_list_بدل_شوی(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	إذا (au_name != عدم)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	إذا (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_لومړی(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	أرجع OK;
    }

    emsg(_(e_string_or_list_expected));
    أرجع FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    فراغ
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    إذا (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	أرجع;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_بیان(eap->arg, eap);
    إذا (tv == عدم)
	أرجع;

    (فراغ)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    ساكن qf_info_T *
hgr_get_ll(صحيح *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    إذا (bt_help(curwin->w_بفر))
	wp = curwin;
    وإلا
	// Find an existing help window
	wp = qf_پیدا_کړه_help_win();

    إذا (wp == عدم)	    // Help window not found
	qi = عدم;
    وإلا
	qi = wp->w_llist;

    إذا (qi == عدم)
    {
	// Allocate a new location list for help text matches
	إذا ((qi = qf_alloc_ډېری(QFLT_LOCATION, 1)) == عدم)
	    أرجع عدم;
	*new_ll = TRUE;
    }

    أرجع qi;
}

/*
 * Search for a pattern in a help file.
 */
    ساكن فراغ
hgr_پلټنه_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    اوږد	lnum;

    fd = mch_fopen((حرف *)fname, "r");
    إذا (fd == عدم)
	أرجع;

    lnum = 1;
    طالما (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *کرښه = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	إذا (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    کرښه = string_convert(p_vc, IObuff, عدم);
	    إذا (کرښه == عدم)
		کرښه = IObuff;
	}

	إذا (vim_regexec(p_regmatch, کرښه, (colnr_T)0))
	{
	    صحيح	l = (صحيح)STRLEN(کرښه);

	    // remove trailing CR, LF, spaces, etc.
	    طالما (l > 0 && کرښه[l - 1] <= ' ')
		کرښه[--l] = NUL;

	    إذا (qf_جمع_کړه_ثبت(qfl,
			عدم,	// dir
			fname,
			عدم,
			0,
			کرښه,
			lnum,
			0,
			(صحيح)(p_regmatch->startp[0] - کرښه)
			+ 1,	//ستون
			(صحيح)(p_regmatch->endp[0] - کرښه)
			+ 1,	// end_col
			FALSE,	// vis_col
			عدم,	// search pattern
			0,	// nr
			1,	// type
			عدم,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		إذا (کرښه != IObuff)
		    vim_free(کرښه);
		اكسر;
	    }
	}
	إذا (کرښه != IObuff)
	    vim_free(کرښه);
	++lnum;
	کرښه_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    ساكن فراغ
hgr_پلټنه_files_دننه_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    صحيح		fcount;
    char_u	**fnames;
    صحيح		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    جمع_کړه_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    إذا (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	لكل (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    إذا (lang != عدم
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		استمر;
# endif

	    hgr_پلټنه_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    ساكن فراغ
hgr_پلټنه_دننه_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    إذا (!enc_utf8)
	convert_تیاری(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    طالما (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_پلټنه_files_دننه_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    إذا (vc.vc_type != CONV_NONE)
	convert_تیاری(&vc, عدم, عدم);
}

/*
 * ":helpgrep {pattern}"
 */
    فراغ
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    صحيح		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    صحيح		new_qi = FALSE;
    char_u	*au_name =  عدم;
    char_u	*lang = عدم;
    صحيح		updated = FALSE;

    بدل (eap->cmdidx)
    {
	حالة CMD_helpgrep:  au_name = (char_u *)"helpgrep"; اكسر;
	حالة CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; اكسر;
	افتراضي: اكسر;
    }
    إذا (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	إذا (aborting())
	    أرجع;
# endif
    }

    إذا (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	إذا (qi == عدم)
	    أرجع;
    }
    وإلا إذا (qi == عدم)
    {
	emsg(_(e_no_quickfix_ډېری));
	أرجع;
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    save_cpo_allocated = is_option_allocated("cpo");
    p_cpo = تشه_option;

    incr_quickfix_busy();

# ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(eap->arg);
# endif
    regmatch.regprog = vim_regcomp(eap->arg, RE_MAGIC + RE_STRING);
    regmatch.rm_ic = FALSE;
    إذا (regmatch.regprog != عدم)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_پلټنه_دننه_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_پیل;
	qfl->qf_شاخص = 1;
	qf_list_بدل_شوی(qfl);
	updated = TRUE;
    }

    إذا (p_cpo == تشه_option)
	p_cpo = save_cpo;
    وإلا
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	إذا (*p_cpo == NUL)
	    سېټ_option_ارزښت_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	إذا (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    إذا (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_بفر(qi, عدم);

    إذا (au_name != عدم)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	إذا (!new_qi && IS_LL_ډېری(qi) && qf_پیدا_کړه_win_with_loclist(qi) == عدم)
	{
	    decr_quickfix_busy();
	    أرجع;
	}
    }

    // Jump to first match.
    إذا (!qf_list_تشه(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    وإلا
	semsg(_(e_no_سمون_str_2), eap->arg);

    decr_quickfix_busy();

    إذا (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	إذا (!bt_help(curwin->w_بفر) || curwin->w_llist == qi)
	{
	    إذا (new_qi)
		ll_free_all(&qi);
	}
	وإلا إذا (curwin->w_llist == عدم && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# إذا defined(EXITFREE)
    فراغ
free_quickfix(فراغ)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(عدم);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#د_که_پای // FEAT_QUICKFIX

#إذا defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    ساكن فراغ
get_qf_loc_list(صحيح is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    إذا (what_arg->v_type == VAR_UNKNOWN)
    {
	إذا (rettv_list_alloc(rettv) == OK)
	    إذا (is_qf || wp != عدم)
		(فراغ)get_errorlist(عدم, wp, -1, 0, rettv->vval.v_list);
    }
    وإلا
    {
	إذا (rettv_dict_alloc(rettv) == OK)
	    إذا (is_qf || (wp != عدم))
	    {
		إذا (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    إذا (d != عدم)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		وإلا
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    فراغ
f_getloclist(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    wp = پیدا_کړه_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    فراغ
f_getqflist(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
# ifdef FEAT_QUICKFIX
    إذا (دننه_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	أرجع;

    get_qf_loc_list(TRUE, عدم, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    ساكن فراغ
سېټ_qf_ll_list(
    win_T	*wp ناکارول_شوی,
    typval_T	*list_arg ناکارول_شوی,
    typval_T	*action_arg ناکارول_شوی,
    typval_T	*what_arg ناکارول_شوی,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    صحيح		action = 0;
    ساكن صحيح	recursive = 0;
# endif

    rettv->vval.v_شمېره = -1;

# ifdef FEAT_QUICKFIX
    إذا (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    وإلا إذا (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    وإلا
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = عدم;
	صحيح	valid_dict = TRUE;

	إذا (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    إذا (act == عدم)
		أرجع;		// type error; errmsg already given
	    إذا ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    وإلا
		semsg(_(e_invalid_action_str_1), act);
	}
	وإلا إذا (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	وإلا
	    emsg(_(e_string_required));

	إذا (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    إذا (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != عدم)
		what = what_arg->vval.v_dict;
	    وإلا
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	إذا (l != عدم && action && valid_dict
		    && سېټ_errorlist(wp, l, action,
		     (char_u *)(wp == عدم ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_شمېره = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    فراغ
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_شمېره = -1;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	أرجع;

    win = پیدا_کړه_win_by_nr_or_id(&argvars[0]);
    إذا (win != عدم)
	سېټ_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    فراغ
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	أرجع;

    سېټ_qf_ll_list(عدم, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#د_که_پای
