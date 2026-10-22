/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * findfile.c: Search for files in directories listed in 'path'
 */

#شامل "vim.h"

/*
 * File searching functions for 'path', 'tags' and 'cdpath' options.
 * External visible functions:
 * vim_findfile_init()		creates/initialises the search context
 * vim_findfile_free_visited()	free list of visited files/dirs of search
 *				context
 * vim_findfile()		find a file in the search context
 * vim_findfile_cleanup()	cleanup/free search context created by
 *				vim_findfile_init()
 *
 * All static functions and variables start with 'ff_'
 *
 * In general it works like this:
 * First you create yourself a search context by calling vim_findfile_init().
 * It is possible to give a search context from a previous call to
 * vim_findfile_init(), so it can be reused. After this you call vim_findfile()
 * until you are satisfied with the result or it returns NULL. On every call it
 * returns the next file which matches the conditions given to
 * vim_findfile_init(). If it doesn't find a next file it returns NULL.
 *
 * It is possible to call vim_findfile_init() again to reinitialise your search
 * with some new parameters. Don't forget to pass your old search context to
 * it, so it can reuse it and especially reuse the list of already visited
 * directories. If you want to delete the list of already visited directories
 * simply call vim_findfile_free_visited().
 *
 * When you are done call vim_findfile_cleanup() to free the search context.
 *
 * The function vim_findfile_init() has a long comment, which describes the
 * needed parameters.
 *
 *
 *
 * ATTENTION:
 * ==========
 *	Also we use an allocated search context here, these functions are NOT
 *	thread-safe!
 *
 *	To minimize parameter passing (or because I'm to lazy), only the
 *	external visible functions get a search context as a parameter. This is
 *	then assigned to a static global, which is used throughout the local
 *	functions.
 */

/*
 * type for the directory search stack
 */
قسمتعریف ساخت ff_ڈھیر
{
    ساخت ff_ڈھیر	*ffs_پچھلا;

    // the fix part (no wildcards) and the part containing the wildcards
    // of the search path
    string_T		ffs_fix_راستہ;
    string_T		ffs_wc_راستہ;

    // files/dirs found in the above directory, matched by the first wildcard
    // of wc_part
    char_u		**ffs_filearray;
    صحیح			ffs_filearray_سائز;
    صحیح			ffs_filearray_cur;   // needed for partly handled dirs

    // to store status of partly handled directories
    // 0: we work on this directory for the first time
    // 1: this directory was partly searched in an earlier step
    صحیح			ffs_stage;

    // How deep are we in the directory tree?
    // Counts backward from value of level parameter to vim_findfile_init
    صحیح			ffs_level;

    // Did we already expand '**' to an empty string?
    صحیح			ffs_ستارہ_ستارہ_خالی_خانے;
} ff_ڈھیر_T;

/*
 * type for already visited directories or files.
 */
قسمتعریف ساخت ff_دیکھا
{
    ساخت ff_دیکھا	*ffv_اگلا;

    // Visited directories are different if the wildcard string are
    // different. So we have to save it.
    char_u		*ffv_wc_راستہ;

    // for unix use inode etc for comparison (needed because of links), else
    // use filename.
#اگر_تعریف_شدہ UNIX
    صحیح			ffv_dev_valid;	// ffv_dev and ffv_ino were set
    dev_t		ffv_dev;	// device number
    ino_t		ffv_ino;	// inode number
#اگر_ختم
    // The memory for this struct is allocated according to the length of
    // ffv_fname.
    char_u		ffv_fname[1];	// actually longer
} ff_دیکھا_T;

/*
 * We might have to manage several visited lists during a search.
 * This is especially needed for the tags option. If tags is set to:
 *      "./++/tags,./++/TAGS,++/tags"  (replace + with *)
 * So we have to do 3 searches:
 *   1) search from the current files directory downward for the file "tags"
 *   2) search from the current files directory downward for the file "TAGS"
 *   3) search from Vims current directory downwards for the file "tags"
 * As you can see, the first and the third search are for the same file, so for
 * the third search we can use the visited list of the first search. For the
 * second search we must start from a empty visited list.
 * The struct ff_visited_list_hdr is used to manage a linked list of already
 * visited lists.
 */
قسمتعریف ساخت ff_دیکھا_list_hdr
{
    ساخت ff_دیکھا_list_hdr	*ffvl_اگلا;

    // the filename the attached visited list is for
    char_u			*ffvl_filename;

    ff_دیکھا_T		*ffvl_دیکھا_list;

} ff_دیکھا_list_hdr_T;


/*
 * '**' can be expanded to several directory levels.
 * Set the default maximum depth.
 */
#تعریف_کرو FF_MAX_ستارہ_ستارہ_EXPAND ((char_u)30)

/*
 * The search context:
 *   ffsc_stack_ptr:	the stack for the dirs to search
 *   ffsc_visited_list: the currently active visited list
 *   ffsc_dir_visited_list: the currently active visited list for search dirs
 *   ffsc_visited_lists_list: the list of all visited lists
 *   ffsc_dir_visited_lists_list: the list of all visited lists for search dirs
 *   ffsc_file_to_search:     the file to search for
 *   ffsc_start_dir:	the starting directory, if search path was relative
 *   ffsc_fix_path:	the fix part of the given path (without wildcards)
 *			Needed for upward search.
 *   ffsc_wc_path:	the part of the given path containing wildcards
 *   ffsc_level:	how many levels of dirs to search downwards
 *   ffsc_stopdirs_v:	array of stop directories for upward search
 *   ffsc_find_what:	FINDFILE_BOTH, FINDFILE_DIR or FINDFILE_FILE
 *   ffsc_tagfile:	searching for tags file, don't use 'suffixesadd'
 */
قسمتعریف ساخت ff_تلاش_ctx_T
{
    ff_ڈھیر_T			*ffsc_ڈھیر_ptr;
    ff_دیکھا_list_hdr_T	*ffsc_دیکھا_list;
    ff_دیکھا_list_hdr_T	*ffsc_dir_دیکھا_list;
    ff_دیکھا_list_hdr_T	*ffsc_دیکھا_lists_list;
    ff_دیکھا_list_hdr_T	*ffsc_dir_دیکھا_lists_list;
    string_T			ffsc_file_مقصد_تلاش;
    string_T			ffsc_آغاز_dir;
    string_T			ffsc_fix_راستہ;
    string_T			ffsc_wc_راستہ;
    صحیح				ffsc_level;
    string_T			*ffsc_stopdirs_v;
    صحیح				ffsc_ڈھونڈو_what;
    صحیح				ffsc_tagfile;
} ff_تلاش_ctx_T;

// locally needed functions
ساکن صحیح ff_check_دیکھا(ff_دیکھا_T **, char_u *, size_t, char_u *, size_t);
ساکن خالی vim_findfile_free_دیکھا(خالی *تلاش_ctx_arg);
ساکن خالی vim_findfile_free_دیکھا_list(ff_دیکھا_list_hdr_T **list_headp);
ساکن خالی ff_free_دیکھا_list(ff_دیکھا_T *vl);
ساکن ff_دیکھا_list_hdr_T* ff_get_دیکھا_list(char_u *, size_t, ff_دیکھا_list_hdr_T **list_headp);

ساکن خالی ff_ڈالو(ff_تلاش_ctx_T *تلاش_ctx, ff_ڈھیر_T *ڈھیر_ptr);
ساکن ff_ڈھیر_T *ff_نکالو(ff_تلاش_ctx_T *تلاش_ctx);
ساکن خالی ff_clear(ff_تلاش_ctx_T *تلاش_ctx);
ساکن خالی ff_free_ڈھیر_element(ff_ڈھیر_T *ڈھیر_ptr);
ساکن ff_ڈھیر_T *ff_create_ڈھیر_element(char_u *, size_t, char_u *, size_t, صحیح, صحیح);
ساکن صحیح ff_راستہ_اندر_stoplist(char_u *, صحیح, string_T *);

ساکن string_T ff_expand_بفر = {تہی, 0};	    // used for expanding filenames

#اگر 0
/*
 * if someone likes findfirst/findnext, here are the functions
 * NOT TESTED!!
 */

ساکن خالی *ff_fn_تلاش_context = تہی;

    char_u *
vim_findfirst(char_u *راستہ, char_u *filename, صحیح level)
{
    ff_fn_تلاش_context =
	vim_findfile_init(راستہ, filename, تہی, level, TRUE, FALSE,
		ff_fn_تلاش_context, rel_fname);
    اگر (تہی == ff_fn_تلاش_context)
	واپس تہی;
    ورنہ
	واپس vim_findnext()
}

    char_u *
vim_findnext(خالی)
{
    char_u *ret = vim_findfile(ff_fn_تلاش_context);

    اگر (تہی == ret)
    {
	vim_findfile_cleanup(ff_fn_تلاش_context);
	ff_fn_تلاش_context = تہی;
    }
    واپس ret;
}
#اگر_ختم

/*
 * Initialization routine for vim_findfile().
 *
 * Returns the newly allocated search context or NULL if an error occurred.
 *
 * Don't forget to clean up by calling vim_findfile_cleanup() if you are done
 * with the search context.
 *
 * Find the file 'filename' in the directory 'path'.
 * The parameter 'path' may contain wildcards. If so only search 'level'
 * directories deep. The parameter 'level' is the absolute maximum and is
 * not related to restricts given to the '**' wildcard. If 'level' is 100
 * and you use '**200' vim_findfile() will stop after 100 levels.
 *
 * 'filename' cannot contain wildcards!  It is used as-is, no backslashes to
 * escape special characters.
 *
 * If 'stopdirs' is not NULL and nothing is found downward, the search is
 * restarted on the next higher directory level. This is repeated until the
 * start-directory of a search is contained in 'stopdirs'. 'stopdirs' has the
 * format ";*<dirname>*\(;<dirname>\)*;\=$".
 *
 * If the 'path' is relative, the starting dir for the search is either VIM's
 * current dir or if the path starts with "./" the current files dir.
 * If the 'path' is absolute, the starting dir is that part of the path before
 * the first wildcard.
 *
 * Upward search is only done on the starting dir.
 *
 * If 'free_visited' is TRUE the list of already visited files/directories is
 * cleared. Set this to FALSE if you just want to search from another
 * directory, but want to be sure that no directory from a previous search is
 * searched again. This is useful if you search for a file at different places.
 * The list of visited files/dirs can also be cleared with the function
 * vim_findfile_free_visited().
 *
 * Set the parameter 'find_what' to FINDFILE_DIR if you want to search for
 * directories only, FINDFILE_FILE for files only, FINDFILE_BOTH for both.
 *
 * A search context returned by a previous call to vim_findfile_init() can be
 * passed in the parameter "search_ctx_arg".  This context is reused and
 * reinitialized with the new parameters.  The list of already visited
 * directories from this context is only deleted if the parameter
 * "free_visited" is true.  Be aware that the passed "search_ctx_arg" is freed
 * if the reinitialization fails.
 *
 * If you don't have a search context from a previous call "search_ctx_arg"
 * must be NULL.
 *
 * This function silently ignores a few errors, vim_findfile() will have
 * limited functionality then.
 */
    خالی *
vim_findfile_init(
    char_u	*راستہ,
    char_u	*filename,
    size_t	filenamelen,
    char_u	*stopdirs غیر_مستعمل,
    صحیح		level,
    صحیح		free_دیکھا,
    صحیح		ڈھونڈو_what,
    خالی	*تلاش_ctx_arg,
    صحیح		tagfile,	// expanding names of tags files
    char_u	*rel_fname)	// file name to use for "."
{
    char_u		*wc_part;
    ff_ڈھیر_T		*sptr;
    ff_تلاش_ctx_T	*تلاش_ctx;
    صحیح			جوڑو_sep;

    // If a search context is given by the caller, reuse it, else allocate a
    // new one.
    اگر (تلاش_ctx_arg != تہی)
	تلاش_ctx = تلاش_ctx_arg;
    ورنہ
    {
	تلاش_ctx = ALLOC_CLEAR_ONE(ff_تلاش_ctx_T);
	اگر (تلاش_ctx == تہی)
	    جاؤ error_return;
    }
    تلاش_ctx->ffsc_ڈھونڈو_what = ڈھونڈو_what;
    تلاش_ctx->ffsc_tagfile = tagfile;

    // clear the search context, but NOT the visited lists
    ff_clear(تلاش_ctx);

    // clear visited list if wanted
    اگر (free_دیکھا == TRUE)
	vim_findfile_free_دیکھا(تلاش_ctx);
    ورنہ
    {
	// Reuse old visited lists. Get the visited list for the given
	// filename. If no list for the current filename exists, creates a new
	// one.
	تلاش_ctx->ffsc_دیکھا_list = ff_get_دیکھا_list(filename,
					filenamelen, &تلاش_ctx->ffsc_دیکھا_lists_list);
	اگر (تلاش_ctx->ffsc_دیکھا_list == تہی)
	    جاؤ error_return;
	تلاش_ctx->ffsc_dir_دیکھا_list = ff_get_دیکھا_list(filename,
				    filenamelen, &تلاش_ctx->ffsc_dir_دیکھا_lists_list);
	اگر (تلاش_ctx->ffsc_dir_دیکھا_list == تہی)
	    جاؤ error_return;
    }

    اگر (ff_expand_بفر.string == تہی)
    {
	ff_expand_بفر.لمبائی = 0;
	ff_expand_بفر.string = alloc(MAXPATHL);
	اگر (ff_expand_بفر.string == تہی)
	    جاؤ error_return;
    }

    // Store information on starting dir now if path is relative.
    // If path is absolute, we do that later.
    اگر (راستہ[0] == '.'
	    && (vim_ispathsep(راستہ[1]) || راستہ[1] == NUL)
	    && (!tagfile || vim_strchr(p_cpo, CPO_DOTTAG) == تہی)
	    && rel_fname != تہی)
    {
	صحیح	len = (صحیح)(gettail(rel_fname) - rel_fname);

	اگر (!vim_isAbsName(rel_fname) && len + 1 < MAXPATHL)
	{
	    // Make the start dir an absolute path name.
	    vim_strncpy(ff_expand_بفر.string, rel_fname, len);
	    ff_expand_بفر.لمبائی = len;

	    تلاش_ctx->ffsc_آغاز_dir.string = بھرا_Name_save(ff_expand_بفر.string, FALSE);
	    اگر (تلاش_ctx->ffsc_آغاز_dir.string == تہی)
		جاؤ error_return;
	    تلاش_ctx->ffsc_آغاز_dir.لمبائی = STRLEN(تلاش_ctx->ffsc_آغاز_dir.string);
	}
	ورنہ
	{
	    تلاش_ctx->ffsc_آغاز_dir.لمبائی = len;
	    تلاش_ctx->ffsc_آغاز_dir.string = vim_strnsave(rel_fname,
		تلاش_ctx->ffsc_آغاز_dir.لمبائی);
	    اگر (تلاش_ctx->ffsc_آغاز_dir.string == تہی)
		جاؤ error_return;
	}

	اگر (*++راستہ != NUL)
	    ++راستہ;
    }
    ورنہ اگر (*راستہ == NUL || !vim_isAbsName(راستہ))
    {
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	// "c:dir" needs "c:" to be expanded, otherwise use current dir
	اگر (*راستہ != NUL && راستہ[1] == ':')
	{
	    char_u  drive[3];

	    drive[0] = راستہ[0];
	    drive[1] = ':';
	    drive[2] = NUL;
	    اگر (vim_بھرا_Name(drive, ff_expand_بفر.string, MAXPATHL, TRUE) == FAIL)
		جاؤ error_return;
	    راستہ += 2;
	}
	ورنہ
#اگر_ختم
	اگر (mch_dirname(ff_expand_بفر.string, MAXPATHL) == FAIL)
	    جاؤ error_return;

	ff_expand_بفر.لمبائی = STRLEN(ff_expand_بفر.string);

	تلاش_ctx->ffsc_آغاز_dir.لمبائی = ff_expand_بفر.لمبائی;
	تلاش_ctx->ffsc_آغاز_dir.string = vim_strnsave(ff_expand_بفر.string,
	    تلاش_ctx->ffsc_آغاز_dir.لمبائی);
	اگر (تلاش_ctx->ffsc_آغاز_dir.string == تہی)
	    جاؤ error_return;
    }

    /*
     * If stopdirs are given, split them into an array of pointers.
     * If this fails (mem allocation), there is no upward search at all or a
     * stop directory is not recognized -> continue silently.
     * If stopdirs just contains a ";" or is empty,
     * search_ctx->ffsc_stopdirs_v will only contain a  NULL pointer. This
     * is handled as unlimited upward search.  See function
     * ff_path_in_stoplist() for details.
     */
    اگر (stopdirs != تہی)
    {
	char_u	*walker = stopdirs;
	صحیح	dircount;

	جب (*walker == ';')
	    walker++;

	dircount = 1;
	تلاش_ctx->ffsc_stopdirs_v = ALLOC_ONE(string_T);

	اگر (تلاش_ctx->ffsc_stopdirs_v != تہی)
	{
	    string_T	*tmp;			// for convenience

	    کرو
	    {
		char_u	*helper;
		خالی	*ptr;
		size_t	len;

		helper = walker;
		ptr = vim_realloc(تلاش_ctx->ffsc_stopdirs_v,
					   (dircount + 1) * حجم(string_T));
		اگر (ptr)
		    تلاش_ctx->ffsc_stopdirs_v = ptr;
		ورنہ
		    // ignore, keep what we have and continue
		    توڑو;
		walker = vim_strchr(walker, ';');
		len = walker ? (size_t)(walker - helper) : STRLEN(helper);
		// "" means ascent till top of directory tree.

		اگر (*helper != NUL && !vim_isAbsName(helper)
							 && len + 1 < MAXPATHL)
		{
		    // Make the stop dir an absolute path name.
		    vim_strncpy(ff_expand_بفر.string, helper, len);
		    ff_expand_بفر.لمبائی = len;

		    tmp = &تلاش_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->string = بھرا_Name_save(ff_expand_بفر.string, FALSE);
		    اگر (tmp->string != تہی)
			tmp->لمبائی = STRLEN(tmp->string);
		}
		ورنہ
		{
		    tmp = &تلاش_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->لمبائی = len;
		    tmp->string = vim_strnsave(helper, tmp->لمبائی);
		    اگر (tmp->string == تہی)
			tmp->لمبائی = 0;
		}
		اگر (walker)
		    walker++;
		dircount++;

	    } جب (walker != تہی);

	    tmp = &تلاش_ctx->ffsc_stopdirs_v[dircount - 1];
	    tmp->string = تہی;
	    tmp->لمبائی = 0;
	}
    }

    تلاش_ctx->ffsc_level = level;

    /*
     * split into:
     *  -fix path
     *  -wildcard_stuff (might be NULL)
     */
    wc_part = vim_strchr(راستہ, '*');
    اگر (wc_part != تہی)
    {
	صحیح	llevel;
	حرف	*errpt;

	// save the fix part of the path
	تلاش_ctx->ffsc_fix_راستہ.لمبائی = (size_t)(wc_part - راستہ);
	تلاش_ctx->ffsc_fix_راستہ.string = vim_strnsave(راستہ,
	    تلاش_ctx->ffsc_fix_راستہ.لمبائی);
	اگر (تلاش_ctx->ffsc_fix_راستہ.string == تہی)
	    جاؤ error_return;

	/*
	 * copy wc_path and add restricts to the '**' wildcard.
	 * The octet after a '**' is used as a (binary) counter.
	 * So '**3' is transposed to '**^C' ('^C' is ASCII value 3)
	 * or '**76' is transposed to '**N'( 'N' is ASCII value 76).
	 * If no restrict is given after '**' the default is used.
	 * Due to this technique the path looks awful if you print it as a
	 * string.
	 */
	ff_expand_بفر.لمبائی = 0;
	جب (*wc_part != NUL)
	{
	    اگر (ff_expand_بفر.لمبائی + 5 >= MAXPATHL)
	    {
		emsg(_(e_راستہ_too_long_for_completion));
		توڑو;
	    }
	    اگر (STRNCMP(wc_part, "**", 2) == 0)
	    {
		ff_expand_بفر.string[ff_expand_بفر.لمبائی++] = *wc_part++;
		ff_expand_بفر.string[ff_expand_بفر.لمبائی++] = *wc_part++;

		llevel = strtol((حرف *)wc_part, &errpt, 10);
		اگر ((char_u *)errpt != wc_part && llevel > 0 && llevel < 255)
		    ff_expand_بفر.string[ff_expand_بفر.لمبائی++] = llevel;
		ورنہ اگر ((char_u *)errpt != wc_part && llevel == 0)
		    // restrict is 0 -> remove already added '**'
		    ff_expand_بفر.لمبائی -= 2;
		ورنہ
		    ff_expand_بفر.string[ff_expand_بفر.لمبائی++] = FF_MAX_ستارہ_ستارہ_EXPAND;
		wc_part = (char_u *)errpt;
		اگر (*wc_part != NUL && !vim_ispathsep(*wc_part))
		{
		    semsg(_(e_invalid_راستہ_نمبر_must_be_at_انتہا_کا_راستہ_or_be_followed_by_str), PATHSEPSTR);
		    جاؤ error_return;
		}
	    }
	    ورنہ
		ff_expand_بفر.string[ff_expand_بفر.لمبائی++] = *wc_part++;
	}
	ff_expand_بفر.string[ff_expand_بفر.لمبائی] = NUL;

	تلاش_ctx->ffsc_wc_راستہ.لمبائی = ff_expand_بفر.لمبائی;
	تلاش_ctx->ffsc_wc_راستہ.string = vim_strnsave(ff_expand_بفر.string,
	    تلاش_ctx->ffsc_wc_راستہ.لمبائی);
	اگر (تلاش_ctx->ffsc_wc_راستہ.string == تہی)
	    جاؤ error_return;
    }
    ورنہ
    {
	تلاش_ctx->ffsc_fix_راستہ.لمبائی = STRLEN(راستہ);
	تلاش_ctx->ffsc_fix_راستہ.string = vim_strnsave(راستہ,
	    تلاش_ctx->ffsc_fix_راستہ.لمبائی);
	اگر (تلاش_ctx->ffsc_fix_راستہ.string == تہی)
	    جاؤ error_return;
    }

    اگر (تلاش_ctx->ffsc_آغاز_dir.string == تہی)
    {
	// store the fix part as startdir.
	// This is needed if the parameter path is fully qualified.
	تلاش_ctx->ffsc_آغاز_dir.لمبائی = تلاش_ctx->ffsc_fix_راستہ.لمبائی;
	تلاش_ctx->ffsc_آغاز_dir.string = vim_strnsave(تلاش_ctx->ffsc_fix_راستہ.string,
	    تلاش_ctx->ffsc_آغاز_dir.لمبائی);
	اگر (تلاش_ctx->ffsc_آغاز_dir.string == تہی)
	    جاؤ error_return;
	تلاش_ctx->ffsc_fix_راستہ.string[0] = NUL;
	تلاش_ctx->ffsc_fix_راستہ.لمبائی = 0;
    }

    // create an absolute path
    اگر (تلاش_ctx->ffsc_آغاز_dir.لمبائی
	    + تلاش_ctx->ffsc_fix_راستہ.لمبائی + 3 >= MAXPATHL)
    {
	emsg(_(e_راستہ_too_long_for_completion));
	جاؤ error_return;
    }

    جوڑو_sep = !after_pathsep(تلاش_ctx->ffsc_آغاز_dir.string,
	تلاش_ctx->ffsc_آغاز_dir.string + تلاش_ctx->ffsc_آغاز_dir.لمبائی);
    ff_expand_بفر.لمبائی = vim_snprintf(
	    (حرف *)ff_expand_بفر.string,
	    MAXPATHL,
	    "%s%s",
	    تلاش_ctx->ffsc_آغاز_dir.string,
	    جوڑو_sep ? PATHSEPSTR : "");

    {
	size_t	bufsize = ff_expand_بفر.لمبائی + تلاش_ctx->ffsc_fix_راستہ.لمبائی + 1;
	char_u	*buf = alloc(bufsize);

	اگر (buf == تہی)
	    جاؤ error_return;

	vim_snprintf(
		(حرف *)buf,
		bufsize,
		"%s%s",
		ff_expand_بفر.string,
		تلاش_ctx->ffsc_fix_راستہ.string);
	اگر (mch_isdir(buf))
	{
	    اگر (تلاش_ctx->ffsc_fix_راستہ.لمبائی > 0)
	    {
		جوڑو_sep = !after_pathsep(تلاش_ctx->ffsc_fix_راستہ.string,
		    تلاش_ctx->ffsc_fix_راستہ.string + تلاش_ctx->ffsc_fix_راستہ.لمبائی);
		ff_expand_بفر.لمبائی += vim_snprintf(
			(حرف *)ff_expand_بفر.string + ff_expand_بفر.لمبائی,
			MAXPATHL - ff_expand_بفر.لمبائی,
			"%s%s",
			تلاش_ctx->ffsc_fix_راستہ.string,
			جوڑو_sep ? PATHSEPSTR : "");
	    }
	}
	ورنہ
	{
	    char_u *p = gettail(تلاش_ctx->ffsc_fix_راستہ.string);
	    صحیح    len = (صحیح)تلاش_ctx->ffsc_fix_راستہ.لمبائی;

	    اگر (p > تلاش_ctx->ffsc_fix_راستہ.string)
	    {
		// do not add '..' to the path and start upwards searching
		len = (صحیح)(p - تلاش_ctx->ffsc_fix_راستہ.string) - 1;
		اگر ((len >= 2
			&& STRNCMP(تلاش_ctx->ffsc_fix_راستہ.string, "..", 2) == 0)
			&& (len == 2 || تلاش_ctx->ffsc_fix_راستہ.string[2] == PATHSEP))
		{
		    vim_free(buf);
		    جاؤ error_return;
		}

		جوڑو_sep = !after_pathsep(تلاش_ctx->ffsc_fix_راستہ.string,
		    تلاش_ctx->ffsc_fix_راستہ.string + تلاش_ctx->ffsc_fix_راستہ.لمبائی);
		ff_expand_بفر.لمبائی += vim_snprintf(
			(حرف *)ff_expand_بفر.string + ff_expand_بفر.لمبائی,
			MAXPATHL - ff_expand_بفر.لمبائی,
			"%.*s%s",
			len,
			تلاش_ctx->ffsc_fix_راستہ.string,
			جوڑو_sep ? PATHSEPSTR : "");
	    }

	    اگر (تلاش_ctx->ffsc_wc_راستہ.string != تہی)
	    {
		size_t	tempsize = (تلاش_ctx->ffsc_fix_راستہ.لمبائی - len)
				+ تلاش_ctx->ffsc_wc_راستہ.لمبائی
				+ 1;
		char_u	*عارضی = alloc(tempsize);

		اگر (عارضی == تہی)
		{
		    vim_free(buf);
		    vim_free(عارضی);
		    جاؤ error_return;
		}

		تلاش_ctx->ffsc_wc_راستہ.لمبائی = vim_snprintf(
			    (حرف *)عارضی,
			    tempsize,
			    "%s%s",
			    تلاش_ctx->ffsc_fix_راستہ.string + len,
			    تلاش_ctx->ffsc_wc_راستہ.string);
		vim_free(تلاش_ctx->ffsc_wc_راستہ.string);
		تلاش_ctx->ffsc_wc_راستہ.string = عارضی;
	    }
	}
	vim_free(buf);
    }

    sptr = ff_create_ڈھیر_element(ff_expand_بفر.string,
			ff_expand_بفر.لمبائی,
			تلاش_ctx->ffsc_wc_راستہ.string,
			تلاش_ctx->ffsc_wc_راستہ.لمبائی,
			level,
			0);

    اگر (sptr == تہی)
	جاؤ error_return;

    ff_ڈالو(تلاش_ctx, sptr);

    تلاش_ctx->ffsc_file_مقصد_تلاش.لمبائی = filenamelen;
    تلاش_ctx->ffsc_file_مقصد_تلاش.string = vim_strnsave(filename,
	تلاش_ctx->ffsc_file_مقصد_تلاش.لمبائی);
    اگر (تلاش_ctx->ffsc_file_مقصد_تلاش.string == تہی)
	جاؤ error_return;

    واپس تلاش_ctx;

error_return:
    /*
     * We clear the search context now!
     * Even when the caller gave us a (perhaps valid) context we free it here,
     * as we might have already destroyed it.
     */
    vim_findfile_cleanup(تلاش_ctx);
    واپس تہی;
}

/*
 * Get the stopdir string.  Check that ';' is not escaped.
 */
    char_u *
vim_findfile_stopdir(char_u *buf)
{
    char_u	*r_ptr = buf;
    char_u	*r_ptr_انتہا = تہی;	    // points to NUL at end of string "r_ptr"

    جب (*r_ptr != NUL && *r_ptr != ';')
    {
	اگر (r_ptr[0] == '\\' && r_ptr[1] == ';')
	{
	    // Overwrite the escape char,
	    // use STRLEN(r_ptr) to move the trailing '\0'.
	    اگر (r_ptr_انتہا == تہی)
		r_ptr_انتہا = r_ptr + STRLEN(r_ptr);
	    mch_memmove(r_ptr, r_ptr + 1,
		(size_t)(r_ptr_انتہا - (r_ptr + 1)) + 1);	// +1 for NUL
	    r_ptr++;
	    --r_ptr_انتہا;
	}
	r_ptr++;
    }
    اگر (*r_ptr == ';')
    {
	*r_ptr = NUL;
	r_ptr++;
    }
    ورنہ اگر (*r_ptr == NUL)
	r_ptr = تہی;
    واپس r_ptr;
}

/*
 * Clean up the given search context. Can handle a NULL pointer.
 */
    خالی
vim_findfile_cleanup(خالی *ctx)
{
    اگر (ctx == تہی)
	واپس;

    vim_findfile_free_دیکھا(ctx);
    ff_clear(ctx);
    vim_free(ctx);
}

/*
 * Find a file in a search context.
 * The search context was created with vim_findfile_init() above.
 * Return a pointer to an allocated file name or NULL if nothing found.
 * To get all matching files call this function until you get NULL.
 *
 * If the passed search_context is NULL, NULL is returned.
 *
 * The search algorithm is depth first. To change this replace the
 * stack with a list (don't forget to leave partly searched directories on the
 * top of the list).
 */
    char_u *
vim_findfile(خالی *تلاش_ctx_arg)
{
    string_T	file_راستہ;
    string_T	rest_کا_wildcards;
    char_u	*راستہ_انتہا = تہی;
    ff_ڈھیر_T	*stackp;
    ff_تلاش_ctx_T *تلاش_ctx;

    اگر (تلاش_ctx_arg == تہی)
	واپس تہی;

    تلاش_ctx = (ff_تلاش_ctx_T *)تلاش_ctx_arg;

    /*
     * filepath is used as buffer for various actions and as the storage to
     * return a found filename.
     */
    اگر ((file_راستہ.string = alloc(MAXPATHL)) == تہی)
	واپس تہی;

    // store the end of the start dir -- needed for upward search
    اگر (تلاش_ctx->ffsc_آغاز_dir.string != تہی)
	راستہ_انتہا = &تلاش_ctx->ffsc_آغاز_dir.string[
					    تلاش_ctx->ffsc_آغاز_dir.لمبائی];

    // upward search loop
    برائے (;;)
    {
	// downward search loop
	برائے (;;)
	{
	    // check if user wants to stop the search
	    ui_breakcheck();
	    اگر (got_int)
		توڑو;

	    // get directory to work on from stack
	    stackp = ff_نکالو(تلاش_ctx);
	    اگر (stackp == تہی)
		توڑو;

	    /*
	     * TODO: decide if we leave this test in
	     *
	     * GOOD: don't search a directory(-tree) twice.
	     * BAD:  - check linked list for every new directory entered.
	     *       - check for double files also done below
	     *
	     * Here we check if we already searched this directory.
	     * We already searched a directory if:
	     * 1) The directory is the same.
	     * 2) We would use the same wildcard string.
	     *
	     * Good if you have links on same directory via several ways
	     *  or you have selfreferences in directories (e.g. SuSE Linux 6.3:
	     *  /etc/rc.d/init.d is linked to /etc/rc.d -> endless loop)
	     *
	     * This check is only needed for directories we work on for the
	     * first time (hence stackp->ff_filearray == NULL)
	     */
	    اگر (stackp->ffs_filearray == تہی
		    && ff_check_دیکھا(&تلاش_ctx->ffsc_dir_دیکھا_list
							  ->ffvl_دیکھا_list,
			    stackp->ffs_fix_راستہ.string,
			    stackp->ffs_fix_راستہ.لمبائی,
			    stackp->ffs_wc_راستہ.string,
			    stackp->ffs_wc_راستہ.لمبائی) == FAIL)
	    {
#اگر_تعریف_شدہ FF_VERBOSE
		اگر (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("Already Searched: %s (%s)",
				   stackp->ffs_fix_راستہ.string, stackp->ffs_wc_راستہ.string);
		    // don't overwrite this either
		    پیغام_puts("\n");
		    verbose_leave_scroll();
		}
#اگر_ختم
		ff_free_ڈھیر_element(stackp);
		جاری;
	    }
#اگر_تعریف_شدہ FF_VERBOSE
	    ورنہ اگر (p_verbose >= 5)
	    {
		verbose_enter_scroll();
		smsg("Searching: %s (%s)",
				   stackp->ffs_fix_راستہ.string, stackp->ffs_wc_راستہ.string);
		// don't overwrite this either
		پیغام_puts("\n");
		verbose_leave_scroll();
	    }
#اگر_ختم

	    // check depth
	    اگر (stackp->ffs_level <= 0)
	    {
		ff_free_ڈھیر_element(stackp);
		جاری;
	    }

	    file_راستہ.string[0] = NUL;
	    file_راستہ.لمبائی = 0;

	    /*
	     * If no filearray till now expand wildcards
	     * The function expand_wildcards() can handle an array of paths
	     * and all possible expands are returned in one array. We use this
	     * to handle the expansion of '**' into an empty string.
	     */
	    اگر (stackp->ffs_filearray == تہی)
	    {
		char_u *dirptrs[2];

		// we use filepath to build the path expand_wildcards() should
		// expand.
		dirptrs[0] = file_راستہ.string;
		dirptrs[1] = تہی;

		// if we have a start dir copy it in
		اگر (!vim_isAbsName(stackp->ffs_fix_راستہ.string)
						&& تلاش_ctx->ffsc_آغاز_dir.string)
		{
		    اگر (تلاش_ctx->ffsc_آغاز_dir.لمبائی + 1 < MAXPATHL)
		    {
			صحیح جوڑو_sep = !after_pathsep(تلاش_ctx->ffsc_آغاز_dir.string,
			    تلاش_ctx->ffsc_آغاز_dir.string + تلاش_ctx->ffsc_آغاز_dir.لمبائی);
			file_راستہ.لمبائی = vim_snprintf(
			    (حرف *)file_راستہ.string,
			    MAXPATHL,
			    "%s%s",
			    تلاش_ctx->ffsc_آغاز_dir.string,
			    جوڑو_sep ? PATHSEPSTR : "");
		    }
		    ورنہ
		    {
			ff_free_ڈھیر_element(stackp);
			جاؤ fail;
		    }
		}

		// append the fix part of the search path
		اگر (file_راستہ.لمبائی + stackp->ffs_fix_راستہ.لمبائی + 1 < MAXPATHL)
		{
		    صحیح جوڑو_sep = !after_pathsep(stackp->ffs_fix_راستہ.string,
			stackp->ffs_fix_راستہ.string + stackp->ffs_fix_راستہ.لمبائی);
		    file_راستہ.لمبائی += vim_snprintf(
			(حرف *)file_راستہ.string + file_راستہ.لمبائی,
			MAXPATHL - file_راستہ.لمبائی,
			"%s%s",
			stackp->ffs_fix_راستہ.string,
			جوڑو_sep ? PATHSEPSTR : "");
		}
		ورنہ
		{
		    ff_free_ڈھیر_element(stackp);
		    جاؤ fail;
		}

		rest_کا_wildcards.string = stackp->ffs_wc_راستہ.string;
		rest_کا_wildcards.لمبائی = stackp->ffs_wc_راستہ.لمبائی;
		اگر (*rest_کا_wildcards.string != NUL)
		{
		    اگر (STRNCMP(rest_کا_wildcards.string, "**", 2) == 0)
		    {
			char_u	*p;

			// pointer to the restrict byte
			// The restrict byte is not a character!
			p = rest_کا_wildcards.string + 2;

			اگر (*p > 0)
			{
			    (*p)--;
			    اگر (file_راستہ.لمبائی + 1 < MAXPATHL)
				file_راستہ.string[file_راستہ.لمبائی++] = '*';
			    ورنہ
			    {
				ff_free_ڈھیر_element(stackp);
				جاؤ fail;
			    }
			}

			اگر (*p == 0)
			{
			    // remove '**<numb> from wildcards
			    mch_memmove(rest_کا_wildcards.string,
					rest_کا_wildcards.string + 3,
					(size_t)(rest_کا_wildcards.لمبائی - 3) + 1);    // +1 for NUL
			    rest_کا_wildcards.لمبائی -= 3;
			    stackp->ffs_wc_راستہ.لمبائی = rest_کا_wildcards.لمبائی;
			}
			ورنہ
			{
			    rest_کا_wildcards.string += 3;
			    rest_کا_wildcards.لمبائی -= 3;
			}

			اگر (stackp->ffs_ستارہ_ستارہ_خالی_خانے == 0)
			{
			    // if not done before, expand '**' to empty
			    stackp->ffs_ستارہ_ستارہ_خالی_خانے = 1;
			    dirptrs[1] = stackp->ffs_fix_راستہ.string;
			}
		    }

		    /*
		     * Here we copy until the next path separator or the end of
		     * the path. If we stop at a path separator, there is
		     * still something else left. This is handled below by
		     * pushing every directory returned from expand_wildcards()
		     * on the stack again for further search.
		     */
		    جب (*rest_کا_wildcards.string
			    && !vim_ispathsep(*rest_کا_wildcards.string))
		    {
			اگر (file_راستہ.لمبائی + 1 < MAXPATHL)
			{
			    file_راستہ.string[file_راستہ.لمبائی++] = *rest_کا_wildcards.string++;
			    --rest_کا_wildcards.لمبائی;
			}
			ورنہ
			{
			    ff_free_ڈھیر_element(stackp);
			    جاؤ fail;
			}
		    }

		    file_راستہ.string[file_راستہ.لمبائی] = NUL;
		    اگر (vim_ispathsep(*rest_کا_wildcards.string))
		    {
			rest_کا_wildcards.string++;
			rest_کا_wildcards.لمبائی--;
		    }
		}

		/*
		 * Expand wildcards like "*" and "$VAR".
		 * If the path is a URL don't try this.
		 */
		اگر (راستہ_with_url(dirptrs[0]))
		{
		    stackp->ffs_filearray = ALLOC_ONE(char_u *);
		    اگر (stackp->ffs_filearray != تہی
			    && (stackp->ffs_filearray[0]
				= vim_strnsave(dirptrs[0], file_راستہ.لمبائی)) != تہی)
			stackp->ffs_filearray_سائز = 1;
		    ورنہ
			stackp->ffs_filearray_سائز = 0;
		}
		ورنہ
		    // Add EW_NOTWILD because the expanded path may contain
		    // wildcard characters that are to be taken literally.
		    // This is a bit of a hack.
		    expand_wildcards((dirptrs[1] == تہی) ? 1 : 2, dirptrs,
			    &stackp->ffs_filearray_سائز,
			    &stackp->ffs_filearray,
			    EW_DIR|EW_ADDSLASH|EW_SILENT|EW_NOTWILD);

		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 0;
	    }
	    ورنہ
	    {
		rest_کا_wildcards.string = &stackp->ffs_wc_راستہ.string[
							stackp->ffs_wc_راستہ.لمبائی];
		rest_کا_wildcards.لمبائی = 0;
	    }

	    اگر (stackp->ffs_stage == 0)
	    {
		صحیح i;

		// this is the first time we work on this directory
		اگر (*rest_کا_wildcards.string == NUL)
		{
		    size_t  len;
		    char_u  *suf;

		    /*
		     * We don't have further wildcards to expand, so we have to
		     * check for the final file now.
		     */
		    برائے (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_سائز; ++i)
		    {
			اگر (!راستہ_with_url(stackp->ffs_filearray[i])
				      && !mch_isdir(stackp->ffs_filearray[i]))
			    جاری;   // not a directory

			// prepare the filename to be checked for existence
			//نیچے
			len = STRLEN(stackp->ffs_filearray[i]);
			اگر (len + 1 + تلاش_ctx->ffsc_file_مقصد_تلاش.لمبائی
									< MAXPATHL)
			{
			    صحیح	جوڑو_sep = !after_pathsep(stackp->ffs_filearray[i],
				    stackp->ffs_filearray[i] + len);
			    file_راستہ.لمبائی = vim_snprintf(
				    (حرف *)file_راستہ.string,
				    MAXPATHL,
				    "%s%s%s",
				    stackp->ffs_filearray[i],
				    جوڑو_sep ? PATHSEPSTR : "",
				    تلاش_ctx->ffsc_file_مقصد_تلاش.string);
			}
			ورنہ
			{
			    ff_free_ڈھیر_element(stackp);
			    جاؤ fail;
			}

			/*
			 * Try without extra suffix and then with suffixes
			 * from 'suffixesadd'.
			 */
			len = file_راستہ.لمبائی;
			اگر (تلاش_ctx->ffsc_tagfile)
			    suf = (char_u *)"";
			ورنہ
			    suf = curbuf->b_p_sua;
			برائے (;;)
			{
			    // if file exists and we didn't already find it
			    اگر ((راستہ_with_url(file_راستہ.string)
				  || (mch_getperm(file_راستہ.string) >= 0
				      && (تلاش_ctx->ffsc_ڈھونڈو_what
							      == FINDFILE_BOTH
					  || ((تلاش_ctx->ffsc_ڈھونڈو_what
							      == FINDFILE_DIR)
						   == mch_isdir(file_راستہ.string)))))
#اگر_غیر_تعریف_شدہ FF_VERBOSE
				    && (ff_check_دیکھا(
					    &تلاش_ctx->ffsc_دیکھا_list
							   ->ffvl_دیکھا_list,
					    file_راستہ.string,
					    file_راستہ.لمبائی,
					    (char_u *)"", 0) == OK)
#اگر_ختم
			       )
			    {
#اگر_تعریف_شدہ FF_VERBOSE
				اگر (ff_check_دیکھا(
					    &تلاش_ctx->ffsc_دیکھا_list
							   ->ffvl_دیکھا_list,
					      file_راستہ.string,
					      file_راستہ.لمبائی,
					      (char_u *)"", 0) == FAIL)
				{
				    اگر (p_verbose >= 5)
				    {
					verbose_enter_scroll();
					smsg("Already: %s", file_راستہ.string);
					// don't overwrite this either
					پیغام_puts("\n");
					verbose_leave_scroll();
				    }
				    جاری;
				}
#اگر_ختم

				// push dir to examine rest of subdirs later
				stackp->ffs_filearray_cur = i + 1;
				ff_ڈالو(تلاش_ctx, stackp);

				اگر (!راستہ_with_url(file_راستہ.string))
				    file_راستہ.لمبائی = simplify_filename(file_راستہ.string);

				اگر (mch_dirname(ff_expand_بفر.string, MAXPATHL)
									== OK)
				{
				    char_u  *p;

				    ff_expand_بفر.لمبائی = STRLEN(ff_expand_بفر.string);
				    p = shorten_fname(file_راستہ.string,
							    ff_expand_بفر.string);
				    اگر (p != تہی)
				    {
					mch_memmove(file_راستہ.string, p,
					    (size_t)((file_راستہ.string + file_راستہ.لمبائی) - p) + 1);  // +1 for NUL
					file_راستہ.لمبائی -= (p - file_راستہ.string);
				    }
				}
#اگر_تعریف_شدہ FF_VERBOSE
				اگر (p_verbose >= 5)
				{
				    verbose_enter_scroll();
				    smsg("HIT: %s", file_راستہ.string);
				    // don't overwrite this either
				    پیغام_puts("\n");
				    verbose_leave_scroll();
				}
#اگر_ختم
				واپس file_راستہ.string;
			    }

			    // Not found or found already, try next suffix.
			    اگر (*suf == NUL)
				توڑو;
			    file_راستہ.لمبائی = len + copy_option_part(&suf,
				  file_راستہ.string + len,
				  (صحیح)(MAXPATHL - len), ",");
			}
		    }
		}
		ورنہ
		{
		    /*
		     * still wildcards left, push the directories for further
		     * search
		     */
		    برائے (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_سائز; ++i)
		    {
			اگر (!mch_isdir(stackp->ffs_filearray[i]))
			    جاری;	// not a directory

			ff_ڈالو(تلاش_ctx,
				ff_create_ڈھیر_element(
						     stackp->ffs_filearray[i],
						     STRLEN(stackp->ffs_filearray[i]),
						     rest_کا_wildcards.string,
						     rest_کا_wildcards.لمبائی,
						     stackp->ffs_level - 1, 0));
		    }
		}
		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 1;
	    }

	    /*
	     * if wildcards contains '**' we have to descent till we reach the
	     * leaves of the directory tree.
	     */
	    اگر (STRNCMP(stackp->ffs_wc_راستہ.string, "**", 2) == 0)
	    {
		صحیح i;

		برائے (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_سائز; ++i)
		{
		    اگر (fnamecmp(stackp->ffs_filearray[i],
						   stackp->ffs_fix_راستہ.string) == 0)
			جاری; // don't repush same directory
		    اگر (!mch_isdir(stackp->ffs_filearray[i]))
			جاری;   // not a directory
		    ff_ڈالو(تلاش_ctx,
			    ff_create_ڈھیر_element(
				stackp->ffs_filearray[i],
				STRLEN(stackp->ffs_filearray[i]),
				stackp->ffs_wc_راستہ.string,
				stackp->ffs_wc_راستہ.لمبائی,
				stackp->ffs_level - 1, 1));
		}
	    }

	    // we are done with the current directory
	    ff_free_ڈھیر_element(stackp);
	}

	// If we reached this, we didn't find anything downwards.
	// Let's check if we should do an upward search.
	اگر (تلاش_ctx->ffsc_آغاز_dir.string
		&& تلاش_ctx->ffsc_stopdirs_v != تہی && !got_int)
	{
	    ff_ڈھیر_T  *sptr;
	    // path_end may point to the NUL or the previous path separator
	    صحیح plen = (راستہ_انتہا - تلاش_ctx->ffsc_آغاز_dir.string)
							  + (*راستہ_انتہا != NUL);

	    // is the last starting directory in the stop list?
	    اگر (ff_راستہ_اندر_stoplist(تلاش_ctx->ffsc_آغاز_dir.string,
				    plen, تلاش_ctx->ffsc_stopdirs_v) == TRUE)
		توڑو;

	    // cut of last dir
	    جب (راستہ_انتہا > تلاش_ctx->ffsc_آغاز_dir.string
						  && vim_ispathsep(*راستہ_انتہا))
		راستہ_انتہا--;
	    جب (راستہ_انتہا > تلاش_ctx->ffsc_آغاز_dir.string
					      && !vim_ispathsep(راستہ_انتہا[-1]))
		راستہ_انتہا--;
	    *راستہ_انتہا = NUL;

	    // we may have shortened search_ctx->ffsc_start_dir, so update it's length
	    تلاش_ctx->ffsc_آغاز_dir.لمبائی = (size_t)(راستہ_انتہا - تلاش_ctx->ffsc_آغاز_dir.string);
	    راستہ_انتہا--;

	    اگر (*تلاش_ctx->ffsc_آغاز_dir.string == NUL)
		توڑو;

	    اگر (تلاش_ctx->ffsc_آغاز_dir.لمبائی + 1
		    + تلاش_ctx->ffsc_fix_راستہ.لمبائی < MAXPATHL)
	    {
		صحیح جوڑو_sep = !after_pathsep(تلاش_ctx->ffsc_آغاز_dir.string,
			    تلاش_ctx->ffsc_آغاز_dir.string + تلاش_ctx->ffsc_آغاز_dir.لمبائی);
		file_راستہ.لمبائی = vim_snprintf(
			(حرف *)file_راستہ.string,
			MAXPATHL,
			"%s%s%s",
			تلاش_ctx->ffsc_آغاز_dir.string,
			جوڑو_sep ? PATHSEPSTR : "",
			تلاش_ctx->ffsc_fix_راستہ.string);
	    }
	    ورنہ
		جاؤ fail;

	    // create a new stack entry
	    sptr = ff_create_ڈھیر_element(file_راستہ.string, file_راستہ.لمبائی,
		    تلاش_ctx->ffsc_wc_راستہ.string, تلاش_ctx->ffsc_wc_راستہ.لمبائی,
		    تلاش_ctx->ffsc_level, 0);
	    اگر (sptr == تہی)
		توڑو;
	    ff_ڈالو(تلاش_ctx, sptr);
	}
	ورنہ
	    توڑو;
    }

fail:
    vim_free(file_راستہ.string);
    واپس تہی;
}

/*
 * Free the list of lists of visited files and directories
 * Can handle it if the passed search_context is NULL;
 */
    ساکن خالی
vim_findfile_free_دیکھا(خالی *تلاش_ctx_arg)
{
    ff_تلاش_ctx_T *تلاش_ctx;

    اگر (تلاش_ctx_arg == تہی)
	واپس;

    تلاش_ctx = (ff_تلاش_ctx_T *)تلاش_ctx_arg;
    vim_findfile_free_دیکھا_list(&تلاش_ctx->ffsc_دیکھا_lists_list);
    vim_findfile_free_دیکھا_list(&تلاش_ctx->ffsc_dir_دیکھا_lists_list);
}

    ساکن خالی
vim_findfile_free_دیکھا_list(ff_دیکھا_list_hdr_T **list_headp)
{
    ff_دیکھا_list_hdr_T *vp;

    جب (*list_headp != تہی)
    {
	vp = (*list_headp)->ffvl_اگلا;
	ff_free_دیکھا_list((*list_headp)->ffvl_دیکھا_list);

	vim_free((*list_headp)->ffvl_filename);
	vim_free(*list_headp);
	*list_headp = vp;
    }
    *list_headp = تہی;
}

    ساکن خالی
ff_free_دیکھا_list(ff_دیکھا_T *vl)
{
    ff_دیکھا_T *vp;

    جب (vl != تہی)
    {
	vp = vl->ffv_اگلا;
	vim_free(vl->ffv_wc_راستہ);
	vim_free(vl);
	vl = vp;
    }
    vl = تہی;
}

/*
 * Returns the already visited list for the given filename. If none is found it
 * allocates a new one.
 */
    ساکن ff_دیکھا_list_hdr_T*
ff_get_دیکھا_list(
    char_u			*filename,
    size_t			filenamelen,
    ff_دیکھا_list_hdr_T	**list_headp)
{
    ff_دیکھا_list_hdr_T  *retptr = تہی;

    // check if a visited list for the given filename exists
    اگر (*list_headp != تہی)
    {
	retptr = *list_headp;
	جب (retptr != تہی)
	{
	    اگر (fnamecmp(filename, retptr->ffvl_filename) == 0)
	    {
#اگر_تعریف_شدہ FF_VERBOSE
		اگر (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("ff_get_visited_list: FOUND list for %s",
								    filename);
		    // don't overwrite this either
		    پیغام_puts("\n");
		    verbose_leave_scroll();
		}
#اگر_ختم
		واپس retptr;
	    }
	    retptr = retptr->ffvl_اگلا;
	}
    }

#اگر_تعریف_شدہ FF_VERBOSE
    اگر (p_verbose >= 5)
    {
	verbose_enter_scroll();
	smsg("ff_get_visited_list: new list for %s", filename);
	// don't overwrite this either
	پیغام_puts("\n");
	verbose_leave_scroll();
    }
#اگر_ختم

    /*
     * if we reach this we didn't find a list and we have to allocate new list
     */
    retptr = ALLOC_ONE(ff_دیکھا_list_hdr_T);
    اگر (retptr == تہی)
	واپس تہی;

    retptr->ffvl_دیکھا_list = تہی;
    retptr->ffvl_filename = vim_strnsave(filename, filenamelen);
    اگر (retptr->ffvl_filename == تہی)
    {
	vim_free(retptr);
	واپس تہی;
    }
    retptr->ffvl_اگلا = *list_headp;
    *list_headp = retptr;

    واپس retptr;
}

/*
 * check if two wildcard paths are equal. Returns TRUE or FALSE.
 * They are equal if:
 *  - both paths are NULL
 *  - they have the same length
 *  - char by char comparison is OK
 *  - the only differences are in the counters behind a '**', so
 *    '**\20' is equal to '**\24'
 */
    ساکن صحیح
ff_wc_equal(char_u *s1, char_u *s2)
{
    صحیح		i, j;
    صحیح		c1 = NUL;
    صحیح		c2 = NUL;
    صحیح		prev1 = NUL;
    صحیح		prev2 = NUL;

    اگر (s1 == s2)
	واپس TRUE;

    اگر (s1 == تہی || s2 == تہی)
	واپس FALSE;

    برائے (i = 0, j = 0; s1[i] != NUL && s2[j] != NUL;)
    {
	c1 = PTR2CHAR(s1 + i);
	c2 = PTR2CHAR(s2 + j);

	اگر ((p_fic ? MB_TOLOWER(c1) != MB_TOLOWER(c2) : c1 != c2)
		&& (prev1 != '*' || prev2 != '*'))
	    واپس FALSE;
	prev2 = prev1;
	prev1 = c1;

	i += mb_ptr2len(s1 + i);
	j += mb_ptr2len(s2 + j);
    }
    واپس s1[i] == s2[j];
}

/*
 * maintains the list of already visited files and dirs
 * returns FAIL if the given file/dir is already in the list
 * returns OK if it is newly added
 *
 * TODO: What to do on memory allocation problems?
 *	 -> return TRUE - Better the file is found several times instead of
 *	    never.
 */
    ساکن صحیح
ff_check_دیکھا(
    ff_دیکھا_T	**دیکھا_list,
    char_u		*fname,
    size_t		fnamelen,
    char_u		*wc_راستہ,
    size_t		wc_pathlen)
{
    ff_دیکھا_T	*vp;
#اگر_تعریف_شدہ UNIX
    stat_T		st;
    صحیح			url = FALSE;
#اگر_ختم

    // For a URL we only compare the name, otherwise we compare the
    // device/inode (unix) or the full path name (not Unix).
    اگر (راستہ_with_url(fname))
    {
	vim_strncpy(ff_expand_بفر.string, fname, fnamelen);
	ff_expand_بفر.لمبائی = fnamelen;
#اگر_تعریف_شدہ UNIX
	url = TRUE;
#اگر_ختم
    }
    ورنہ
    {
	ff_expand_بفر.string[0] = NUL;
	ff_expand_بفر.لمبائی = 0;
#اگر_تعریف_شدہ UNIX
	اگر (mch_stat((حرف *)fname, &st) < 0)
	    واپس FAIL;
#ورنہ
	اگر (vim_بھرا_Name(fname, ff_expand_بفر.string, MAXPATHL, TRUE) == FAIL)
	    واپس FAIL;
	ff_expand_بفر.لمبائی = STRLEN(ff_expand_بفر.string);
#اگر_ختم
    }

    // check against list of already visited files
    برائے (vp = *دیکھا_list; vp != تہی; vp = vp->ffv_اگلا)
    {
	اگر (
#اگر_تعریف_شدہ UNIX
		!url ? (vp->ffv_dev_valid && vp->ffv_dev == st.st_dev
						  && vp->ffv_ino == st.st_ino)
		     :
#اگر_ختم
		fnamecmp(vp->ffv_fname, ff_expand_بفر.string) == 0
	   )
	{
	    // are the wildcard parts equal
	    اگر (ff_wc_equal(vp->ffv_wc_راستہ, wc_راستہ) == TRUE)
		// already visited
		واپس FAIL;
	}
    }

    /*
     * New file/dir.  Add it to the list of visited files/dirs.
     */
    vp = alloc(
	     offsetof(ff_دیکھا_T, ffv_fname) + ff_expand_بفر.لمبائی + 1);
    اگر (vp == تہی)
	واپس OK;

#اگر_تعریف_شدہ UNIX
    اگر (!url)
    {
	vp->ffv_dev_valid = TRUE;
	vp->ffv_dev = st.st_dev;
	vp->ffv_ino = st.st_ino;
	vp->ffv_fname[0] = NUL;
    }
    ورنہ
    {
	vp->ffv_dev_valid = FALSE;
#اگر_ختم
	STRCPY(vp->ffv_fname, ff_expand_بفر.string);
#اگر_تعریف_شدہ UNIX
    }
#اگر_ختم
    اگر (wc_راستہ != تہی)
	vp->ffv_wc_راستہ = vim_strnsave(wc_راستہ, wc_pathlen);
    ورنہ
	vp->ffv_wc_راستہ = تہی;

    vp->ffv_اگلا = *دیکھا_list;
    *دیکھا_list = vp;

    واپس OK;
}

/*
 * create stack element from given path pieces
 */
    ساکن ff_ڈھیر_T *
ff_create_ڈھیر_element(
    char_u	*fix_part,
    size_t	fix_partlen,
    char_u	*wc_part,
    size_t	wc_partlen,
    صحیح		level,
    صحیح		ستارہ_ستارہ_خالی_خانے)
{
    ff_ڈھیر_T	*new;

    new = ALLOC_ONE(ff_ڈھیر_T);
    اگر (new == تہی)
	واپس تہی;

    new->ffs_پچھلا	   = تہی;
    new->ffs_filearray	   = تہی;
    new->ffs_filearray_سائز = 0;
    new->ffs_filearray_cur  = 0;
    new->ffs_stage	   = 0;
    new->ffs_level	   = level;
    new->ffs_ستارہ_ستارہ_خالی_خانے = ستارہ_ستارہ_خالی_خانے;

    // the following saves NULL pointer checks in vim_findfile
    اگر (fix_part == تہی)
    {
	fix_part = (char_u *)"";
	fix_partlen = 0;
    }
    new->ffs_fix_راستہ.string = vim_strnsave(fix_part, fix_partlen);
    new->ffs_fix_راستہ.لمبائی = fix_partlen;

    اگر (wc_part == تہی)
    {
	wc_part = (char_u *)"";
	wc_partlen = 0;
    }
    new->ffs_wc_راستہ.string = vim_strnsave(wc_part, wc_partlen);
    new->ffs_wc_راستہ.لمبائی = wc_partlen;

    اگر (new->ffs_fix_راستہ.string == تہی || new->ffs_wc_راستہ.string == تہی)
    {
	ff_free_ڈھیر_element(new);
	new = تہی;
    }

    واپس new;
}

/*
 * Push a dir on the directory stack.
 */
    ساکن خالی
ff_ڈالو(ff_تلاش_ctx_T *تلاش_ctx, ff_ڈھیر_T *ڈھیر_ptr)
{
    // check for NULL pointer, not to return an error to the user, but
    // to prevent a crash
    اگر (ڈھیر_ptr == تہی)
	واپس;

    ڈھیر_ptr->ffs_پچھلا = تلاش_ctx->ffsc_ڈھیر_ptr;
    تلاش_ctx->ffsc_ڈھیر_ptr = ڈھیر_ptr;
}

/*
 * Pop a dir from the directory stack.
 * Returns NULL if stack is empty.
 */
    ساکن ff_ڈھیر_T *
ff_نکالو(ff_تلاش_ctx_T *تلاش_ctx)
{
    ff_ڈھیر_T  *sptr;

    sptr = تلاش_ctx->ffsc_ڈھیر_ptr;
    اگر (تلاش_ctx->ffsc_ڈھیر_ptr != تہی)
	تلاش_ctx->ffsc_ڈھیر_ptr = تلاش_ctx->ffsc_ڈھیر_ptr->ffs_پچھلا;

    واپس sptr;
}

/*
 * free the given stack element
 */
    ساکن خالی
ff_free_ڈھیر_element(ff_ڈھیر_T *ڈھیر_ptr)
{
    // VIM_CLEAR_STRING handles possible NULL pointers
    VIM_CLEAR_STRING(ڈھیر_ptr->ffs_fix_راستہ);
    VIM_CLEAR_STRING(ڈھیر_ptr->ffs_wc_راستہ);

    اگر (ڈھیر_ptr->ffs_filearray != تہی)
	FreeWild(ڈھیر_ptr->ffs_filearray_سائز, ڈھیر_ptr->ffs_filearray);

    vim_free(ڈھیر_ptr);
}

/*
 * Clear the search context, but NOT the visited list.
 */
    ساکن خالی
ff_clear(ff_تلاش_ctx_T *تلاش_ctx)
{
    ff_ڈھیر_T   *sptr;

    // clear up stack
    جب ((sptr = ff_نکالو(تلاش_ctx)) != تہی)
	ff_free_ڈھیر_element(sptr);

    اگر (تلاش_ctx->ffsc_stopdirs_v != تہی)
    {
	صحیح  i = 0;

	جب (تلاش_ctx->ffsc_stopdirs_v[i].string != تہی)
	{
	    vim_free(تلاش_ctx->ffsc_stopdirs_v[i].string);
	    i++;
	}
	VIM_CLEAR(تلاش_ctx->ffsc_stopdirs_v);
    }

    // reset everything
    VIM_CLEAR_STRING(تلاش_ctx->ffsc_file_مقصد_تلاش);
    VIM_CLEAR_STRING(تلاش_ctx->ffsc_آغاز_dir);
    VIM_CLEAR_STRING(تلاش_ctx->ffsc_fix_راستہ);
    VIM_CLEAR_STRING(تلاش_ctx->ffsc_wc_راستہ);
    تلاش_ctx->ffsc_level = 0;
}

/*
 * check if the given path is in the stopdirs
 * returns TRUE if yes else FALSE
 */
    ساکن صحیح
ff_راستہ_اندر_stoplist(char_u *راستہ, صحیح راستہ_len, string_T *stopdirs_v)
{
    صحیح		i = 0;

    // eat up trailing path separators, except the first
    جب (راستہ_len > 1 && vim_ispathsep(راستہ[راستہ_len - 1]))
	راستہ_len--;

    // if no path consider it as match
    اگر (راستہ_len == 0)
	واپس TRUE;

    برائے (i = 0; stopdirs_v[i].string != تہی; i++)
	// match for parent directory. So '/home' also matches
	// '/home/rks'. Check for PATHSEP in stopdirs_v[i], else
	// '/home/r' would also match '/home/rks'
	اگر (fnamencmp(stopdirs_v[i].string, راستہ, راستہ_len) == 0
		&& ((صحیح)stopdirs_v[i].لمبائی <= راستہ_len
		    || vim_ispathsep(stopdirs_v[i].string[راستہ_len])))
	    واپس TRUE;

    واپس FALSE;
}

/*
 * Find the file name "ptr[len]" in the path.  Also finds directory names.
 *
 * On the first call set the parameter 'first' to TRUE to initialize
 * the search.  For repeating calls to FALSE.
 *
 * Repeating calls will return other files called 'ptr[len]' from the path.
 *
 * Only on the first call 'ptr' and 'len' are used.  For repeating calls they
 * don't need valid values.
 *
 * If nothing found on the first call the option FNAME_MESS will issue the
 * message:
 *	    'Can't find file "<file>" in path'
 * On repeating calls:
 *	    'No more file "<file>" found in path'
 *
 * options:
 * FNAME_MESS	    give error message when not found
 *
 * Uses NameBuff[]!
 *
 * Returns an allocated string for the file name.  NULL for error.
 *
 */
    char_u *
ڈھونڈو_file_اندر_راستہ(
    char_u	*ptr,		// file name
    صحیح		len,		// length of file name
    صحیح		options,
    صحیح		پہلا,		// use count'th matching file name
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_مقصد_ڈھونڈو,	// in/out: modified copy of file name
    حرف	**تلاش_ctx)	// in/out: state of the search
{
    واپس ڈھونڈو_file_اندر_راستہ_option(ptr, len, options, پہلا,
	    *curbuf->b_p_راستہ == NUL ? p_راستہ : curbuf->b_p_راستہ,
	    FINDFILE_BOTH, rel_fname, curbuf->b_p_sua,
	    file_مقصد_ڈھونڈو, تلاش_ctx);
}

#اگر defined(EXITFREE)
    خالی
free_findfile(خالی)
{
    VIM_CLEAR_STRING(ff_expand_بفر);
}
#اگر_ختم

/*
 * Find the directory name "ptr[len]" in the path.
 *
 * options:
 * FNAME_MESS	    give error message when not found
 * FNAME_UNESC	    unescape backslashes.
 *
 * Uses NameBuff[]!
 *
 * Returns an allocated string for the file name.  NULL for error.
 */
    char_u *
ڈھونڈو_directory_اندر_راستہ(
    char_u	*ptr,		// file name
    صحیح		len,		// length of file name
    صحیح		options,
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_مقصد_ڈھونڈو,	// in/out: modified copy of file name
    حرف	**تلاش_ctx)	// in/out: state of the search
{
    واپس ڈھونڈو_file_اندر_راستہ_option(ptr, len, options, TRUE, p_cdpath,
				       FINDFILE_DIR, rel_fname, (char_u *)"",
				       file_مقصد_ڈھونڈو, تلاش_ctx);
}

    char_u *
ڈھونڈو_file_اندر_راستہ_option(
    char_u	*ptr,		// file name
    صحیح		len,		// length of file name
    صحیح		options,
    صحیح		پہلا,		// use count'th matching file name
    char_u	*راستہ_option,	// p_path or p_cdpath
    صحیح		ڈھونڈو_what,	// FINDFILE_FILE, _DIR or _BOTH
    char_u	*rel_fname,	// file name we are looking relative to.
    char_u	*suffixes,	// list of suffixes, 'suffixesadd' option
    char_u	**file_مقصد_ڈھونڈو,	// in/out: modified copy of file name
    حرف	**تلاش_ctx_arg) // in/out: state of the search
{
    ff_تلاش_ctx_T	**تلاش_ctx = (ff_تلاش_ctx_T **)تلاش_ctx_arg;
    ساکن char_u	*dir;
    ساکن صحیح		did_findfile_init = FALSE;
    char_u		*file_name = تہی;
    صحیح			rel_مقصد_curdir;
#اگر_تعریف_شدہ AMIGA
    ساخت Process	*proc = (ساخت Process *)ڈھونڈو_Task(0L);
    APTR		save_winptr = proc->pr_کھڑکی_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_کھڑکی_Ptr = (APTR)-1L;
#اگر_ختم
    ساکن size_t	file_مقصد_findlen = 0;

    اگر (پہلا == TRUE)
    {
	char_u	save_char;

	اگر (len == 0)
	    واپس تہی;

	// copy file name into NameBuff, expanding environment variables
	save_char = ptr[len];
	ptr[len] = NUL;
	file_مقصد_findlen = expand_env_esc(ptr, NameBuff, MAXPATHL, تہی, TRUE, تہی);
	ptr[len] = save_char;

	vim_free(*file_مقصد_ڈھونڈو);
	*file_مقصد_ڈھونڈو = vim_strnsave(NameBuff, file_مقصد_findlen);
	اگر (*file_مقصد_ڈھونڈو == تہی)	// out of memory
	{
	    file_name = تہی;
	    file_مقصد_findlen = 0;
	    جاؤ theend;
	}
	اگر (options & FNAME_UNESC)
	{
	    // Change all "\ " to " ".
	    برائے (ptr = *file_مقصد_ڈھونڈو; *ptr != NUL; ++ptr)
		اگر (ptr[0] == '\\' && ptr[1] == ' ')
		{
		    mch_memmove(ptr, ptr + 1,
			(size_t)((*file_مقصد_ڈھونڈو + file_مقصد_findlen) - (ptr + 1)) + 1);
		    --file_مقصد_findlen;
		}
	}
    }

    rel_مقصد_curdir = ((*file_مقصد_ڈھونڈو)[0] == '.'
		    && ((*file_مقصد_ڈھونڈو)[1] == NUL
			|| vim_ispathsep((*file_مقصد_ڈھونڈو)[1])
			|| ((*file_مقصد_ڈھونڈو)[1] == '.'
			    && ((*file_مقصد_ڈھونڈو)[2] == NUL
				|| vim_ispathsep((*file_مقصد_ڈھونڈو)[2])))));
    اگر (vim_isAbsName(*file_مقصد_ڈھونڈو)
	    // "..", "../path", "." and "./path": don't use the path_option
	    || rel_مقصد_curdir
#اگر defined(MSWIN)
	    // handle "\tmp" as absolute path
	    || vim_ispathsep((*file_مقصد_ڈھونڈو)[0])
	    // handle "c:name" as absolute path
	    || ((*file_مقصد_ڈھونڈو)[0] != NUL && (*file_مقصد_ڈھونڈو)[1] == ':')
#اگر_ختم
#اگر_تعریف_شدہ AMIGA
	    // handle ":tmp" as absolute path
	    || (*file_مقصد_ڈھونڈو)[0] == ':'
#اگر_ختم
       )
    {
	/*
	 * Absolute path, no need to use "path_option".
	 * If this is not a first call, return NULL.  We already returned a
	 * filename on the first call.
	 */
	اگر (پہلا == TRUE)
	{
	    صحیح		l;
	    صحیح		تسلسل;
	    size_t	rel_fnamelen = 0;
	    char_u	*suffix;

	    اگر (راستہ_with_url(*file_مقصد_ڈھونڈو))
	    {
		file_name = vim_strnsave(*file_مقصد_ڈھونڈو, file_مقصد_findlen);
		جاؤ theend;
	    }

	    اگر (rel_fname != تہی)
		rel_fnamelen = STRLEN(rel_fname);

	    // When FNAME_REL flag given first use the directory of the file.
	    // Otherwise or when this fails use the current directory.
	    برائے (تسلسل = 1; تسلسل <= 2; ++تسلسل)
	    {
		l = (صحیح)file_مقصد_findlen;
		اگر (تسلسل == 1
			&& rel_مقصد_curdir
			&& (options & FNAME_REL)
			&& rel_fname != تہی
			&& rel_fnamelen + l < MAXPATHL)
		{
		    l = vim_snprintf(
			(حرف *)NameBuff,
			MAXPATHL,
			"%.*s%s",
			(صحیح)(gettail(rel_fname) - rel_fname),
			rel_fname,
			*file_مقصد_ڈھونڈو);
		}
		ورنہ
		{
		    STRCPY(NameBuff, *file_مقصد_ڈھونڈو);
		    تسلسل = 2;
		}

		// When the file doesn't exist, try adding parts of
		// 'suffixesadd'.
		suffix = suffixes;
		برائے (;;)
		{
		    اگر (mch_getperm(NameBuff) >= 0
			     && (ڈھونڈو_what == FINDFILE_BOTH
				 || ((ڈھونڈو_what == FINDFILE_DIR)
						    == mch_isdir(NameBuff))))
		    {
			file_name = vim_strnsave(NameBuff,
						  simplify_filename(NameBuff));
			جاؤ theend;
		    }
		    اگر (*suffix == NUL)
			توڑو;
		    copy_option_part(&suffix, NameBuff + l, MAXPATHL - l, ",");
		}
	    }
	}
    }
    ورنہ
    {
	/*
	 * Loop over all paths in the 'path' or 'cdpath' option.
	 * When "first" is set, first setup to the start of the option.
	 * Otherwise continue to find the next match.
	 */
	اگر (پہلا == TRUE)
	{
	    // vim_findfile_free_visited can handle a possible NULL pointer
	    vim_findfile_free_دیکھا(*تلاش_ctx);
	    dir = راستہ_option;
	    did_findfile_init = FALSE;
	}

	برائے (;;)
	{
	    اگر (did_findfile_init)
	    {
		file_name = vim_findfile(*تلاش_ctx);
		اگر (file_name != تہی)
		    توڑو;

		did_findfile_init = FALSE;
	    }
	    ورنہ
	    {
		char_u	*buf;
		char_u  *r_ptr;

		اگر (dir == تہی || *dir == NUL)
		{
		    // We searched all paths of the option, now we can
		    // free the search context.
		    vim_findfile_cleanup(*تلاش_ctx);
		    *تلاش_ctx = تہی;
		    توڑو;
		}

		اگر ((buf = alloc(MAXPATHL)) == تہی)
		    توڑو;

		// copy next path
		buf[0] = NUL;
		copy_option_part(&dir, buf, MAXPATHL, " ,");

		// get the stopdir string
		r_ptr = vim_findfile_stopdir(buf);
		*تلاش_ctx = vim_findfile_init(buf, *file_مقصد_ڈھونڈو, file_مقصد_findlen,
					    r_ptr, 100, FALSE, ڈھونڈو_what,
					   *تلاش_ctx, FALSE, rel_fname);
		اگر (*تلاش_ctx != تہی)
		    did_findfile_init = TRUE;
		vim_free(buf);
	    }
	}
    }
    اگر (file_name == تہی && (options & FNAME_MESS))
    {
	اگر (پہلا == TRUE)
	{
	    اگر (ڈھونڈو_what == FINDFILE_DIR)
		semsg(_(e_cant_ڈھونڈو_directory_str_اندر_cdpath), *file_مقصد_ڈھونڈو);
	    ورنہ
		semsg(_(e_cant_ڈھونڈو_file_str_اندر_راستہ), *file_مقصد_ڈھونڈو);
	}
	ورنہ
	{
	    اگر (ڈھونڈو_what == FINDFILE_DIR)
		semsg(_(e_no_مزید_directory_str_ملا_اندر_cdpath),
								*file_مقصد_ڈھونڈو);
	    ورنہ
		semsg(_(e_no_مزید_file_str_ملا_اندر_راستہ), *file_مقصد_ڈھونڈو);
	}
    }

theend:
#اگر_تعریف_شدہ AMIGA
    proc->pr_کھڑکی_Ptr = save_winptr;
#اگر_ختم
    واپس file_name;
}

/*
 * Get the file name at the cursor.
 * If Visual mode is active, use the selected text if it's in one line.
 * Returns the name in allocated memory, NULL for failure.
 */
    char_u *
grab_file_name(طویل گنتی, linenr_T *file_lnum)
{
    صحیح options = FNAME_MESS|FNAME_EXP|FNAME_REL|FNAME_UNESC;

    اگر (VIsual_فعال)
    {
	صحیح	len;
	char_u	*ptr;

	اگر (get_visual_متن(تہی, &ptr, &len) == FAIL)
	    واپس تہی;
	// Only recognize ":123" here
	اگر (file_lnum != تہی && ptr[len] == ':' && محفوظ_isdigit(ptr[len + 1]))
	{
	    char_u *p = ptr + len + 1;

	    *file_lnum = getdigits(&p);
	}
	واپس ڈھونڈو_file_name_اندر_راستہ(ptr, len, options,
						     گنتی, curbuf->b_ffname);
    }
    واپس file_name_at_cursor(options | FNAME_HYP, گنتی, file_lnum);
}

/*
 * Return the file name under or after the cursor.
 *
 * The 'path' option is searched if the file name is not absolute.
 * The string returned has been alloc'ed and should be freed by the caller.
 * NULL is returned if the file name or file is not found.
 *
 * options:
 * FNAME_MESS	    give error messages
 * FNAME_EXP	    expand to path
 * FNAME_HYP	    check for hypertext link
 * FNAME_INCL	    apply "includeexpr"
 */
    char_u *
file_name_at_cursor(صحیح options, طویل گنتی, linenr_T *file_lnum)
{
    واپس file_name_اندر_سطر(ml_get_curline(),
		      curwin->w_cursor.ستون, options, گنتی, curbuf->b_ffname,
		      file_lnum);
}

/*
 * Return the name of the file under or after ptr[col].
 * Otherwise like file_name_at_cursor().
 */
    char_u *
file_name_اندر_سطر(
    char_u	*سطر,
    صحیح		ستون,
    صحیح		options,
    طویل	گنتی,
    char_u	*rel_fname,	// file we are searching relative to
    linenr_T	*file_lnum)	// line number after the file name
{
    char_u	*ptr;
    صحیح		len;
    صحیح		اندر_type = TRUE;
    صحیح		is_url = FALSE;

    /*
     * search forward for what could be the start of a file name
     */
    ptr = سطر + ستون;
    جب (*ptr != NUL && !vim_isfilec(*ptr))
	MB_PTR_ADV(ptr);
    اگر (*ptr == NUL)		// nothing found
    {
	اگر (options & FNAME_MESS)
	    emsg(_(e_no_file_name_under_cursor));
	واپس تہی;
    }

    /*
     * Search backward for first char of the file name.
     * Go one char back to ":" before "//" even when ':' is not in 'isfname'.
     */
    جب (ptr > سطر)
    {
	اگر (has_mbyte && (len = (*mb_سر_off)(سطر, ptr - 1)) > 0)
	    ptr -= len + 1;
	ورنہ اگر (vim_isfilec(ptr[-1])
		|| ((options & FNAME_HYP) && راستہ_is_url(ptr - 1)))
	    --ptr;
	ورنہ
	    توڑو;
    }

    /*
     * Search forward for the last char of the file name.
     * Also allow "://" when ':' is not in 'isfname'.
     */
    len = 0;
    جب (vim_isfilec(ptr[len]) || (ptr[len] == '\\' && ptr[len + 1] == ' ')
		 || ((options & FNAME_HYP) && راستہ_is_url(ptr + len))
		 || (is_url && vim_strchr((char_u *)":?&=", ptr[len]) != تہی))
    {
	// After type:// we also include :, ?, & and = as valid characters, so
	// that http://google.com:8080?q=this&that=ok works.
	اگر ((ptr[len] >= 'A' && ptr[len] <= 'Z')
				       || (ptr[len] >= 'a' && ptr[len] <= 'z'))
	{
	    اگر (اندر_type && راستہ_is_url(ptr + len + 1))
		is_url = TRUE;
	}
	ورنہ
	    اندر_type = FALSE;

	اگر (ptr[len] == '\\')
	    // Skip over the "\" in "\ ".
	    ++len;
	اگر (has_mbyte)
	    len += (*mb_ptr2len)(ptr + len);
	ورنہ
	    ++len;
    }

    /*
     * If there is trailing punctuation, remove it.
     * But don't remove "..", could be a directory name.
     */
    اگر (len > 2 && vim_strchr((char_u *)".,:;!", ptr[len - 1]) != تہی
						       && ptr[len - 2] != '.')
	--len;

    اگر (file_lnum != تہی)
    {
	char_u	*p;
	حرف	*مطابقت_متن = " line ";		// english
	size_t	مطابقت_textlen = 6;

	// Get the number after the file name and a separator character.
	// Also accept " line 999" with and without the same translation as
	// used in last_set_msg().
	p = ptr + len;
	اگر (STRNCMP(p, مطابقت_متن, مطابقت_textlen) == 0)
	    p += مطابقت_textlen;
	ورنہ
	{
	    // no match with english, try localized
	    مطابقت_متن = _(سطر_پیغام);
	    مطابقت_textlen = STRLEN(مطابقت_متن);

	    اگر (STRNCMP(p, مطابقت_متن, مطابقت_textlen) == 0)
		p += مطابقت_textlen;
	    ورنہ
		p = skipwhite(p);
	}
	اگر (*p != NUL)
	{
	    اگر (!محفوظ_isdigit(*p))
		++p;		    // skip the separator
	    p = skipwhite(p);
	    اگر (محفوظ_isdigit(*p))
		*file_lnum = (صحیح)getdigits(&p);
	}
    }

    واپس ڈھونڈو_file_name_اندر_راستہ(ptr, len, options, گنتی, rel_fname);
}

#اگر defined(FEAT_ڈھونڈو_ID) && defined(FEAT_EVAL)
    ساکن char_u *
eval_includeexpr(char_u *ptr, صحیح len)
{
    char_u	*res;
    sctx_T	save_sctx = current_sctx;

    سیٹ_vim_var_string(VV_FNAME, ptr, len);
    current_sctx = curbuf->b_p_script_ctx[BV_INEX];

    res = eval_مقصد_string_محفوظ(curbuf->b_p_inex,
	    was_سیٹ_insecurely(curwin, (char_u *)"includeexpr", OPT_LOCAL),
								   TRUE, TRUE);

    سیٹ_vim_var_string(VV_FNAME, تہی, 0);
    current_sctx = save_sctx;
    واپس res;
}
#اگر_ختم

/*
 * Return the name of the file ptr[len] in 'path'.
 * Otherwise like file_name_at_cursor().
 */
    char_u *
ڈھونڈو_file_name_اندر_راستہ(
    char_u	*ptr,
    صحیح		len,
    صحیح		options,
    طویل	گنتی,
    char_u	*rel_fname)	// file we are searching relative to
{
    char_u	*file_name;
    صحیح		c;
#اگر defined(FEAT_ڈھونڈو_ID) && defined(FEAT_EVAL)
    char_u	*tofree = تہی;
#اگر_ختم

    اگر (len == 0)
	واپس تہی;

#اگر defined(FEAT_ڈھونڈو_ID) && defined(FEAT_EVAL)
    اگر ((options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
    {
	tofree = eval_includeexpr(ptr, len);
	اگر (tofree != تہی)
	{
	    ptr = tofree;
	    len = (صحیح)STRLEN(ptr);
	}
    }
#اگر_ختم

    اگر (options & FNAME_EXP)
    {
	char_u	*file_مقصد_ڈھونڈو = تہی;
	حرف	*تلاش_ctx = تہی;

	file_name = ڈھونڈو_file_اندر_راستہ(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_مقصد_ڈھونڈو, &تلاش_ctx);

#اگر defined(FEAT_ڈھونڈو_ID) && defined(FEAT_EVAL)
	/*
	 * If the file could not be found in a normal way, try applying
	 * 'includeexpr' (unless done already).
	 */
	اگر (file_name == تہی
		&& !(options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
	{
	    tofree = eval_includeexpr(ptr, len);
	    اگر (tofree != تہی)
	    {
		ptr = tofree;
		len = (صحیح)STRLEN(ptr);
		file_name = ڈھونڈو_file_اندر_راستہ(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_مقصد_ڈھونڈو, &تلاش_ctx);
	    }
	}
#اگر_ختم
	اگر (file_name == تہی && (options & FNAME_MESS))
	{
	    c = ptr[len];
	    ptr[len] = NUL;
	    semsg(_(e_cant_ڈھونڈو_file_str_اندر_راستہ_2), ptr);
	    ptr[len] = c;
	}

	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	جب (file_name != تہی && --گنتی > 0)
	{
	    vim_free(file_name);
	    file_name = ڈھونڈو_file_اندر_راستہ(ptr, len, options, FALSE, rel_fname,
						   &file_مقصد_ڈھونڈو, &تلاش_ctx);
	}

	vim_free(file_مقصد_ڈھونڈو);
	vim_findfile_cleanup(تلاش_ctx);
    }
    ورنہ
	file_name = vim_strnsave(ptr, len);

#اگر defined(FEAT_ڈھونڈو_ID) && defined(FEAT_EVAL)
    vim_free(tofree);
#اگر_ختم

    واپس file_name;
}

/*
 * Return the end of the directory name, on the first path
 * separator:
 * "/path/file", "/path/dir/", "/path//dir", "/file"
 *	 ^	       ^	     ^	      ^
 */
    ساکن char_u *
gettail_dir(char_u *fname)
{
    char_u	*dir_انتہا = fname;
    char_u	*اگلا_dir_انتہا = fname;
    صحیح		look_for_sep = TRUE;
    char_u	*p;

    برائے (p = fname; *p != NUL; )
    {
	اگر (vim_ispathsep(*p))
	{
	    اگر (look_for_sep)
	    {
		اگلا_dir_انتہا = p;
		look_for_sep = FALSE;
	    }
	}
	ورنہ
	{
	    اگر (!look_for_sep)
		dir_انتہا = اگلا_dir_انتہا;
	    look_for_sep = TRUE;
	}
	MB_PTR_ADV(p);
    }
    واپس dir_انتہا;
}

/*
 * return TRUE if 'c' is a path list separator.
 */
    صحیح
vim_ispathlistsep(صحیح c)
{
#اگر_تعریف_شدہ UNIX
    واپس (c == ':');
#ورنہ
    واپس (c == ';');	// might not be right for every system...
#اگر_ختم
}

/*
 * Moves "*psep" back to the previous path separator in "path".
 * Returns FAIL is "*psep" ends up at the beginning of "path".
 */
    ساکن صحیح
ڈھونڈو_پچھلا_pathsep(char_u *راستہ, char_u **psep)
{
    // skip the current separator
    اگر (*psep > راستہ && vim_ispathsep(**psep))
	--*psep;

    // find the previous separator
    جب (*psep > راستہ)
    {
	اگر (vim_ispathsep(**psep))
	    واپس OK;
	MB_PTR_عقب(راستہ, *psep);
    }

    واپس FAIL;
}

/*
 * Returns TRUE if "maybe_unique" is unique wrt other_paths in "gap".
 * "maybe_unique" is the end portion of "((char_u **)gap->ga_data)[i]".
 */
    ساکن صحیح
is_unique(char_u *maybe_unique, garray_T *gap, صحیح i)
{
    صحیح	    j;
    صحیح	    candidate_len = (صحیح)STRLEN(maybe_unique);
    صحیح	    other_راستہ_len;
    char_u  **other_paths = (char_u **)gap->ga_ڈیٹا;
    char_u  *rival;

    برائے (j = 0; j < gap->ga_len; j++)
    {
	اگر (j == i)
	    جاری;  // don't compare it with itself

	other_راستہ_len = (صحیح)STRLEN(other_paths[j]);
	اگر (other_راستہ_len < candidate_len)
	    جاری;  // it's different when it's shorter

	rival = other_paths[j] + other_راستہ_len - candidate_len;
	اگر (fnamecmp(maybe_unique, rival) == 0
		&& (rival == other_paths[j] || vim_ispathsep(*(rival - 1))))
	    واپس FALSE;  //مطابقت
    }

    واپس TRUE;  // no match found
}

/*
 * Split the 'path' option into an array of strings in garray_T.  Relative
 * paths are expanded to their equivalent fullpath.  This includes the "."
 * (relative to current buffer directory) and empty path (relative to current
 * directory) notations.
 *
 * TODO: handle upward search (;) and path limiter (**N) notations by
 * expanding each into their equivalent path(s).
 */
    ساکن خالی
expand_راستہ_option(
	char_u		*curdir,
	char_u		*راستہ_option,	// p_path or p_cdpath
	garray_T	*gap)
{
    char_u	*buf;
    size_t	buflen;
    char_u	*p;
    size_t	curdirlen = 0;

    اگر ((buf = alloc(MAXPATHL)) == تہی)
	واپس;

    جب (*راستہ_option != NUL)
    {
	buflen = copy_option_part(&راستہ_option, buf, MAXPATHL, " ,");

	// do not expand backticks, could have been set via a modeline
	اگر (vim_strchr(buf, '`') != تہی)
	    جاری;

	اگر (buf[0] == '.' && (buf[1] == NUL || vim_ispathsep(buf[1])))
	{
	    size_t  plen;

	    // Relative to current buffer:
	    // "/path/file" + "." -> "/path/"
	    // "/path/file"  + "./subdir" -> "/path/subdir"
	    اگر (curbuf->b_ffname == تہی)
		جاری;
	    p = gettail(curbuf->b_ffname);
	    plen = (size_t)(p - curbuf->b_ffname);
	    اگر (plen + buflen >= MAXPATHL)
		جاری;
	    اگر (buf[1] == NUL)
		buf[plen] = NUL;
	    ورنہ
		mch_memmove(buf + plen, buf + 2, (buflen - 2) + 1); // +1 for NUL
	    mch_memmove(buf, curbuf->b_ffname, plen);
	    buflen = simplify_filename(buf);
	}
	ورنہ اگر (buf[0] == NUL)
	{
	    // relative to current directory
	    STRCPY(buf, curdir);
	    اگر (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    buflen = curdirlen;
	}
	ورنہ اگر (راستہ_with_url(buf))
	    // URL can't be used here
	    جاری;
	ورنہ اگر (!mch_is_بھرا_Name(buf))
	{
	    // Expand relative path to their full path equivalent
	    اگر (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    اگر (curdirlen + buflen + 3 > MAXPATHL)
		جاری;

	    mch_memmove(buf + curdirlen + 1, buf, buflen + 1);	    // +1 for NUL
	    STRCPY(buf, curdir);
	    buf[curdirlen] = PATHSEP;
	    buflen = simplify_filename(buf);
	}

	اگر (ga_grow(gap, 1) == FAIL)
	    توڑو;

#اگر defined(MSWIN)
	// Avoid the path ending in a backslash, it fails when a comma is
	// appended.
	اگر (buf[buflen - 1] == '\\')
	    buf[buflen - 1] = '/';
#اگر_ختم

	p = vim_strnsave(buf, buflen);
	اگر (p == تہی)
	    توڑو;
	((char_u **)gap->ga_ڈیٹا)[gap->ga_len++] = p;
    }

    vim_free(buf);
}

/*
 * Returns a pointer to the file or directory name in "fname" that matches the
 * longest path in "ga"p, or NULL if there is no match. For example:
 *
 *    path: /foo/bar/baz
 *   fname: /foo/bar/baz/quux.txt
 * returns:		 ^this
 */
    ساکن char_u *
get_راستہ_cutoff(char_u *fname, garray_T *gap)
{
    صحیح	    i;
    صحیح	    maxlen = 0;
    char_u  **راستہ_part = (char_u **)gap->ga_ڈیٹا;
    char_u  *cutoff = تہی;

    برائے (i = 0; i < gap->ga_len; i++)
    {
	صحیح j = 0;

	جب ((fname[j] == راستہ_part[i][j]
#اگر defined(MSWIN)
		|| (vim_ispathsep(fname[j]) && vim_ispathsep(راستہ_part[i][j]))
#اگر_ختم
			     ) && fname[j] != NUL && راستہ_part[i][j] != NUL)
	    j++;
	اگر (j > maxlen)
	{
	    maxlen = j;
	    cutoff = &fname[j];
	}
    }

    // skip to the file or directory name
    اگر (cutoff != تہی)
	جب (vim_ispathsep(*cutoff))
	    MB_PTR_ADV(cutoff);

    واپس cutoff;
}

/*
 * Sorts, removes duplicates and modifies all the fullpath names in "gap" so
 * that they are unique with respect to each other while conserving the part
 * that matches the pattern. Beware, this is at least O(n^2) wrt "gap->ga_len".
 */
    خالی
uniquefy_paths(
	garray_T *gap,
	char_u *نمونہ,
	char_u *راستہ_option)	// p_path or p_cdpath
{
    صحیح		i;
    صحیح		len;
    char_u	**fnames = (char_u **)gap->ga_ڈیٹا;
    صحیح		ترتیب_دو_again = FALSE;
    char_u	*pat;
    char_u      *file_نمونہ;
    char_u	*curdir;
    regmatch_T	regmatch;
    garray_T	راستہ_ga;
    char_u	**اندر_curdir = تہی;
    char_u	*short_name;

    remove_duplicates(gap);
    ga_init2(&راستہ_ga, حجم(char_u *), 1);

    /*
     * We need to prepend a '*' at the beginning of file_pattern so that the
     * regex matches anywhere in the path. FIXME: is this valid for all
     * possible patterns?
     */
    len = (صحیح)STRLEN(نمونہ);
    file_نمونہ = alloc(len + 2);
    اگر (file_نمونہ == تہی)
	واپس;
    file_نمونہ[0] = '*';
    STRCPY(file_نمونہ + 1, نمونہ);
    pat = file_pat_مقصد_reg_pat(file_نمونہ, تہی, تہی, FALSE);
    vim_free(file_نمونہ);
    اگر (pat == تہی)
	واپس;

    regmatch.rm_ic = TRUE;		// always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    vim_free(pat);
    اگر (regmatch.regprog == تہی)
	واپس;

    اگر ((curdir = alloc(MAXPATHL)) == تہی)
	جاؤ theend;
    mch_dirname(curdir, MAXPATHL);
    expand_راستہ_option(curdir, راستہ_option, &راستہ_ga);

    اندر_curdir = ALLOC_CLEAR_MULT(char_u *, gap->ga_len);
    اگر (اندر_curdir == تہی)
	جاؤ theend;

    برائے (i = 0; i < gap->ga_len && !got_int; i++)
    {
	char_u	    *راستہ = fnames[i];
	صحیح	    is_اندر_curdir;
	char_u	    *dir_انتہا = gettail_dir(راستہ);
	char_u	    *راستہ_cutoff;

	len = (صحیح)STRLEN(راستہ);
	is_اندر_curdir = fnamencmp(curdir, راستہ, dir_انتہا - راستہ) == 0
					     && curdir[dir_انتہا - راستہ] == NUL;
	اگر (is_اندر_curdir)
	    اندر_curdir[i] = vim_strnsave(راستہ, len);

	// Shorten the filename while maintaining its uniqueness
	راستہ_cutoff = get_راستہ_cutoff(راستہ, &راستہ_ga);

	// Don't assume all files can be reached without path when search
	// pattern starts with star star slash, so only remove path_cutoff
	// when possible.
	اگر (نمونہ[0] == '*' && نمونہ[1] == '*'
		&& vim_ispathsep_nocolon(نمونہ[2])
		&& راستہ_cutoff != تہی
		&& vim_regexec(&regmatch, راستہ_cutoff, (colnr_T)0)
		&& is_unique(راستہ_cutoff, gap, i))
	{
	    ترتیب_دو_again = TRUE;
	    mch_memmove(راستہ, راستہ_cutoff, STRLEN(راستہ_cutoff) + 1);
	}
	ورنہ
	{
	    // Here all files can be reached without path, so get shortest
	    // unique path.  We start at the end of the path.
	    char_u  *pathsep_p = راستہ + len - 1;

	    جب (ڈھونڈو_پچھلا_pathsep(راستہ, &pathsep_p))
	    {
		اگر (vim_regexec(&regmatch, pathsep_p + 1, (colnr_T)0)
			&& is_unique(pathsep_p + 1, gap, i)
			&& راستہ_cutoff != تہی && pathsep_p + 1 >= راستہ_cutoff)
		{
		    ترتیب_دو_again = TRUE;
		    mch_memmove(راستہ, pathsep_p + 1,
			    (size_t)((راستہ + len) - (pathsep_p + 1)) + 1);  // +1 for NUL
		    توڑو;
		}
	    }
	}

	اگر (mch_is_بھرا_Name(راستہ))
	{
	    /*
	     * Last resort: shorten relative to curdir if possible.
	     * 'possible' means:
	     * 1. It is under the current directory.
	     * 2. The result is actually shorter than the original.
	     *
	     *	    Before		  curdir	After
	     *	    /foo/bar/file.txt	  /foo/bar	./file.txt
	     *	    c:\foo\bar\file.txt   c:\foo\bar	.\file.txt
	     *	    /file.txt		  /		/file.txt
	     *	    c:\file.txt		  c:\		.\file.txt
	     */
	    short_name = shorten_fname(راستہ, curdir);
	    اگر (short_name != تہی && short_name > راستہ + 1
#اگر defined(MSWIN)
		    // On windows,
		    //	    shorten_fname("c:\a\a.txt", "c:\a\b")
		    // returns "\a\a.txt", which is not really the short
		    // name, hence:
		    && !vim_ispathsep(*short_name)
#اگر_ختم
		)
	    {
		vim_snprintf((حرف *)راستہ, MAXPATHL, ".%s%s", PATHSEPSTR, short_name);
	    }
	}
	ui_breakcheck();
    }

    // Shorten filenames in /in/current/directory/{filename}
    برائے (i = 0; i < gap->ga_len && !got_int; i++)
    {
	size_t	rel_pathsize;
	char_u	*rel_راستہ;
	char_u	*راستہ = اندر_curdir[i];

	اگر (راستہ == تہی)
	    جاری;

	// If the {filename} is not unique, change it to ./{filename}.
	// Else reduce it to {filename}
	short_name = shorten_fname(راستہ, curdir);
	اگر (short_name == تہی)
	    short_name = راستہ;
	اگر (is_unique(short_name, gap, i))
	{
	    STRCPY(fnames[i], short_name);
	    جاری;
	}

	rel_pathsize = 1 + حجم(PATHSEP) + STRLEN(short_name) + 1;
	rel_راستہ = alloc(rel_pathsize);
	اگر (rel_راستہ == تہی)
	    جاؤ theend;

	vim_snprintf((حرف *)rel_راستہ, rel_pathsize, ".%s%s", PATHSEPSTR, short_name);

	vim_free(fnames[i]);
	fnames[i] = rel_راستہ;
	ترتیب_دو_again = TRUE;
	ui_breakcheck();
    }

theend:
    vim_free(curdir);
    اگر (اندر_curdir != تہی)
    {
	برائے (i = 0; i < gap->ga_len; i++)
	    vim_free(اندر_curdir[i]);
	vim_free(اندر_curdir);
    }
    ga_clear_لڑیاں(&راستہ_ga);
    vim_regfree(regmatch.regprog);

    اگر (ترتیب_دو_again)
	remove_duplicates(gap);
}

/*
 * Calls globpath() with 'path' values for the given pattern and stores the
 * result in "gap".
 * Returns the total number of matches.
 */
    صحیح
expand_اندر_راستہ(
    garray_T	*gap,
    char_u	*نمونہ,
    صحیح		flags)		// EW_* flags
{
    char_u	*curdir;
    garray_T	راستہ_ga;
    char_u	*paths = تہی;
    صحیح		glob_flags = 0;
    char_u	*راستہ_option = *curbuf->b_p_راستہ == NUL ? p_راستہ : curbuf->b_p_راستہ;

    اگر ((curdir = alloc(MAXPATHL)) == تہی)
	واپس 0;
    mch_dirname(curdir, MAXPATHL);

    ga_init2(&راستہ_ga, حجم(char_u *), 1);
    اگر (flags & EW_CDPATH)
	expand_راستہ_option(curdir, p_cdpath, &راستہ_ga);
    ورنہ
	expand_راستہ_option(curdir, راستہ_option, &راستہ_ga);
    vim_free(curdir);
    اگر (راستہ_ga.ga_len == 0)
	واپس 0;

    paths = ga_concat_لڑیاں(&راستہ_ga, ",");
    ga_clear_لڑیاں(&راستہ_ga);
    اگر (paths == تہی)
	واپس 0;

    اگر (flags & EW_ICASE)
	glob_flags |= WILD_ICASE;
    اگر (flags & EW_ADDSLASH)
	glob_flags |= WILD_جوڑو_SLASH;
    globpath(paths, نمونہ, gap, glob_flags, !!(flags & EW_CDPATH));
    vim_free(paths);

    واپس gap->ga_len;
}


/*
 * Converts a file name into a canonical form. It simplifies a file name into
 * its simplest form by stripping out unneeded components, if any.  The
 * resulting file name is simplified in place and will either be the same
 * length as that supplied, or shorter.
 */
    size_t
simplify_filename(char_u *filename)
{
#اگر_غیر_تعریف_شدہ AMIGA	    // Amiga doesn't have "..", it uses "/"
    صحیح		components = 0;
    char_u	*p, *دم, *آغاز;
    char_u	*p_انتہا;			    // point to NUL at end of string "p"
    صحیح		stripping_disabled = FALSE;
    صحیح		relative = TRUE;

    p = filename;
# ifdef BACKSLASH_اندر_FILENAME
    اگر (p[0] != NUL && p[1] == ':')	    // skip "x:"
	p += 2;
# endif

    اگر (vim_ispathsep(*p))
    {
	relative = FALSE;
	کرو
	    ++p;
	جب (vim_ispathsep(*p));
    }
    آغاز = p;	    // remember start after "c:/" or "/" or "///"
    p_انتہا = p + STRLEN(p);
# ifdef UNIX
    // Posix says that "//path" is unchanged but "///path" is "/path".
    اگر (آغاز > filename + 2)
    {
	mch_memmove(filename + 1, p, (size_t)(p_انتہا - p) + 1);	    // +1 for NUL
	p_انتہا -= (size_t)(p - (filename + 1));
	آغاز = p = filename + 1;
    }
# endif

    کرو
    {
	// At this point "p" is pointing to the char following a single "/"
	// or "p" is at the "start" of the (absolute or relative) path name.
# ifdef VMS
	// VMS allows device:[path] - don't strip the [ in directory
	اگر ((*p == '[' || *p == '<') && p > filename && p[-1] == ':')
	{
	    // :[ or :< composition: vms directory component
	    ++components;
	    p = getnextcomp(p + 1);
	}
	// allow remote calls as host"user passwd"::device:[path]
	ورنہ اگر (p[0] == ':' && p[1] == ':' && p > filename && p[-1] == '"' )
	{
	    // ":: composition: vms host/passwd component
	    ++components;
	    p = getnextcomp(p + 2);
	}
	ورنہ
# endif
	اگر (vim_ispathsep(*p))
	{
	    mch_memmove(p, p + 1, (size_t)(p_انتہا - (p + 1)) + 1); // remove duplicate "/"
	    --p_انتہا;
	}
	ورنہ اگر (p[0] == '.' && (vim_ispathsep(p[1]) || p[1] == NUL))
	{
	    اگر (p == آغاز && relative)
		p += 1 + (p[1] != NUL);	// keep single "." or leading "./"
	    ورنہ
	    {
		// Strip "./" or ".///".  If we are at the end of the file name
		// and there is no trailing path separator, either strip "/." if
		// we are after "start", or strip "." if we are at the beginning
		// of an absolute path name .
		دم = p + 1;
		اگر (p[1] != NUL)
		    جب (vim_ispathsep(*دم))
			MB_PTR_ADV(دم);
		ورنہ اگر (p > آغاز)
		    --p;		// strip preceding path separator

		mch_memmove(p, دم, (size_t)(p_انتہا - دم) + 1);
		p_انتہا -= (size_t)(دم - p);
	    }
	}
	ورنہ اگر (p[0] == '.' && p[1] == '.' &&
	    (vim_ispathsep(p[2]) || p[2] == NUL))
	{
	    // Skip to after ".." or "../" or "..///".
	    دم = p + 2;
	    جب (vim_ispathsep(*دم))
		MB_PTR_ADV(دم);

	    اگر (components > 0)		// strip one preceding component
	    {
		صحیح		do_strip = FALSE;
		char_u		saved_char;
		stat_T		st;

		// Don't strip for an erroneous file name.
		اگر (!stripping_disabled)
		{
		    // If the preceding component does not exist in the file
		    // system, we strip it.  On Unix, we don't accept a symbolic
		    // link that refers to a non-existent file.
		    saved_char = p[-1];
		    p[-1] = NUL;
# ifdef UNIX
		    اگر (mch_lstat((حرف *)filename, &st) < 0)
# ورنہ
			اگر (mch_stat((حرف *)filename, &st) < 0)
# endif
			    do_strip = TRUE;
		    p[-1] = saved_char;

		    --p;
		    // Skip back to after previous '/'.
		    جب (p > آغاز && !after_pathsep(آغاز, p))
			MB_PTR_عقب(آغاز, p);

		    اگر (!do_strip)
		    {
			// If the component exists in the file system, check
			// that stripping it won't change the meaning of the
			// file name.  First get information about the
			// unstripped file name.  This may fail if the component
			// to strip is not a searchable directory (but a regular
			// file, for instance), since the trailing "/.." cannot
			// be applied then.  We don't strip it then since we
			// don't want to replace an erroneous file name by
			// a valid one, and we disable stripping of later
			// components.
			saved_char = *دم;
			*دم = NUL;
			اگر (mch_stat((حرف *)filename, &st) >= 0)
			    do_strip = TRUE;
			ورنہ
			    stripping_disabled = TRUE;
			*دم = saved_char;
# ifdef UNIX
			اگر (do_strip)
			{
			    stat_T	new_st;

			    // On Unix, the check for the unstripped file name
			    // above works also for a symbolic link pointing to
			    // a searchable directory.  But then the parent of
			    // the directory pointed to by the link must be the
			    // same as the stripped file name.  (The latter
			    // exists in the file system since it is the
			    // component's parent directory.)
			    اگر (p == آغاز && relative)
				(خالی)mch_stat(".", &new_st);
			    ورنہ
			    {
				saved_char = *p;
				*p = NUL;
				(خالی)mch_stat((حرف *)filename, &new_st);
				*p = saved_char;
			    }

			    اگر (new_st.st_ino != st.st_ino ||
				new_st.st_dev != st.st_dev)
			    {
				do_strip = FALSE;
				// We don't disable stripping of later
				// components since the unstripped path name is
				// still valid.
			    }
			}
# endif
		    }
		}

		اگر (!do_strip)
		{
		    // Skip the ".." or "../" and reset the counter for the
		    // components that might be stripped later on.
		    p = دم;
		    components = 0;
		}
		ورنہ
		{
		    // Strip previous component.  If the result would get empty
		    // and there is no trailing path separator, leave a single
		    // "." instead.  If we are at the end of the file name and
		    // there is no trailing path separator and a preceding
		    // component is left after stripping, strip its trailing
		    // path separator as well.
		    اگر (p == آغاز && relative && دم[-1] == '.')
		    {
			*p++ = '.';
			*p = NUL;
		    }
		    ورنہ
		    {
			اگر (p > آغاز && دم[-1] == '.')
			    --p;

			mch_memmove(p, دم, (size_t)(p_انتہا - دم) + 1);	// strip previous component
			p_انتہا -= (size_t)(دم - p);
		    }

		    --components;
		}
	    }
	    ورنہ اگر (p == آغاز && !relative)	// leading "/.." or "/../"
	    {
		mch_memmove(p, دم, (size_t)(p_انتہا - دم) + 1);		// strip ".." or "../"
		p_انتہا -= (size_t)(دم - p);
	    }
	    ورنہ
	    {
		اگر (p == آغاز + 2 && p[-2] == '.')	// leading "./../"
		{
		    mch_memmove(p - 2, p, (size_t)(p_انتہا - p) + 1); // strip leading "./"
		    p_انتہا -= 2;
		    دم -= 2;
		}
		p = دم;		// skip to char after ".." or "../"
	    }
	}
	ورنہ
	{
	    ++components;		// simple path component
	    p = getnextcomp(p);
	}
    } جب (*p != NUL);

    واپس (size_t)(p_انتہا - filename);
#ورنہ
    // Don't touch Amiga filenames
    واپس STRLEN(filename);
#اگر_ختم // !AMIGA
}

#اگر defined(FEAT_EVAL)
/*
 * "simplify()" function
 */
    خالی
f_simplify(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    p = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_string = vim_strsave(p);
    simplify_filename(rettv->vval.v_string);	// simplify in place
    rettv->v_type = VAR_STRING;
}
#اگر_ختم // FEAT_EVAL
