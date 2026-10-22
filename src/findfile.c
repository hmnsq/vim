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

#ضمن "vim.h"

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
تعريفنوع بنية ff_ڍير
{
    بنية ff_ڍير	*ffs_prev;

    // the fix part (no wildcards) and the part containing the wildcards
    // of the search path
    string_T		ffs_fix_رستو;
    string_T		ffs_wc_رستو;

    // files/dirs found in the above directory, matched by the first wildcard
    // of wc_part
    char_u		**ffs_filearray;
    صحيح			ffs_filearray_سائيز;
    صحيح			ffs_filearray_cur;   // needed for partly handled dirs

    // to store status of partly handled directories
    // 0: we work on this directory for the first time
    // 1: this directory was partly searched in an earlier step
    صحيح			ffs_stage;

    // How deep are we in the directory tree?
    // Counts backward from value of level parameter to vim_findfile_init
    صحيح			ffs_level;

    // Did we already expand '**' to an empty string?
    صحيح			ffs_تارو_تارو_خالي_جايون;
} ff_ڍير_T;

/*
 * type for already visited directories or files.
 */
تعريفنوع بنية ff_گهميل
{
    بنية ff_گهميل	*ffv_ايندڙ;

    // Visited directories are different if the wildcard string are
    // different. So we have to save it.
    char_u		*ffv_wc_رستو;

    // for unix use inode etc for comparison (needed because of links), else
    // use filename.
#جيڪڏهن_وصف_ٿيل UNIX
    صحيح			ffv_dev_valid;	// ffv_dev and ffv_ino were set
    dev_t		ffv_dev;	// device number
    ino_t		ffv_ino;	// inode number
#جيڪڏهن_پورو
    // The memory for this struct is allocated according to the length of
    // ffv_fname.
    char_u		ffv_fname[1];	// actually longer
} ff_گهميل_T;

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
تعريفنوع بنية ff_گهميل_list_hdr
{
    بنية ff_گهميل_list_hdr	*ffvl_ايندڙ;

    // the filename the attached visited list is for
    char_u			*ffvl_filename;

    ff_گهميل_T		*ffvl_گهميل_list;

} ff_گهميل_list_hdr_T;


/*
 * '**' can be expanded to several directory levels.
 * Set the default maximum depth.
 */
#وصف_ڪريو FF_MAX_تارو_تارو_EXPAND ((char_u)30)

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
تعريفنوع بنية ff_ڳولا_ctx_T
{
    ff_ڍير_T			*ffsc_ڍير_ptr;
    ff_گهميل_list_hdr_T	*ffsc_گهميل_list;
    ff_گهميل_list_hdr_T	*ffsc_dir_گهميل_list;
    ff_گهميل_list_hdr_T	*ffsc_گهميل_lists_list;
    ff_گهميل_list_hdr_T	*ffsc_dir_گهميل_lists_list;
    string_T			ffsc_file_منزل_ڳولا;
    string_T			ffsc_شروعات_dir;
    string_T			ffsc_fix_رستو;
    string_T			ffsc_wc_رستو;
    صحيح				ffsc_level;
    string_T			*ffsc_stopdirs_v;
    صحيح				ffsc_ڳوليو_what;
    صحيح				ffsc_tagfile;
} ff_ڳولا_ctx_T;

// locally needed functions
ساكن صحيح ff_check_گهميل(ff_گهميل_T **, char_u *, size_t, char_u *, size_t);
ساكن فراغ vim_findfile_free_گهميل(فراغ *ڳولا_ctx_arg);
ساكن فراغ vim_findfile_free_گهميل_list(ff_گهميل_list_hdr_T **list_headp);
ساكن فراغ ff_free_گهميل_list(ff_گهميل_T *vl);
ساكن ff_گهميل_list_hdr_T* ff_get_گهميل_list(char_u *, size_t, ff_گهميل_list_hdr_T **list_headp);

ساكن فراغ ff_ڌڪو(ff_ڳولا_ctx_T *ڳولا_ctx, ff_ڍير_T *ڍير_ptr);
ساكن ff_ڍير_T *ff_ڪڍو(ff_ڳولا_ctx_T *ڳولا_ctx);
ساكن فراغ ff_clear(ff_ڳولا_ctx_T *ڳولا_ctx);
ساكن فراغ ff_free_ڍير_element(ff_ڍير_T *ڍير_ptr);
ساكن ff_ڍير_T *ff_create_ڍير_element(char_u *, size_t, char_u *, size_t, صحيح, صحيح);
ساكن صحيح ff_رستو_اندريون_stoplist(char_u *, صحيح, string_T *);

ساكن string_T ff_expand_بفر = {عدم, 0};	    // used for expanding filenames

#إذا 0
/*
 * if someone likes findfirst/findnext, here are the functions
 * NOT TESTED!!
 */

ساكن فراغ *ff_fn_ڳولا_context = عدم;

    char_u *
vim_findfirst(char_u *رستو, char_u *filename, صحيح level)
{
    ff_fn_ڳولا_context =
	vim_findfile_init(رستو, filename, عدم, level, TRUE, FALSE,
		ff_fn_ڳولا_context, rel_fname);
    إذا (عدم == ff_fn_ڳولا_context)
	أرجع عدم;
    وإلا
	أرجع vim_findnext()
}

    char_u *
vim_findnext(فراغ)
{
    char_u *ret = vim_findfile(ff_fn_ڳولا_context);

    إذا (عدم == ret)
    {
	vim_findfile_cleanup(ff_fn_ڳولا_context);
	ff_fn_ڳولا_context = عدم;
    }
    أرجع ret;
}
#جيڪڏهن_پورو

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
    فراغ *
vim_findfile_init(
    char_u	*رستو,
    char_u	*filename,
    size_t	filenamelen,
    char_u	*stopdirs اڻ_استعمال_ٿيل,
    صحيح		level,
    صحيح		free_گهميل,
    صحيح		ڳوليو_what,
    فراغ	*ڳولا_ctx_arg,
    صحيح		tagfile,	// expanding names of tags files
    char_u	*rel_fname)	// file name to use for "."
{
    char_u		*wc_part;
    ff_ڍير_T		*sptr;
    ff_ڳولا_ctx_T	*ڳولا_ctx;
    صحيح			جوڙيو_sep;

    // If a search context is given by the caller, reuse it, else allocate a
    // new one.
    إذا (ڳولا_ctx_arg != عدم)
	ڳولا_ctx = ڳولا_ctx_arg;
    وإلا
    {
	ڳولا_ctx = ALLOC_CLEAR_ONE(ff_ڳولا_ctx_T);
	إذا (ڳولا_ctx == عدم)
	    اذهب error_return;
    }
    ڳولا_ctx->ffsc_ڳوليو_what = ڳوليو_what;
    ڳولا_ctx->ffsc_tagfile = tagfile;

    // clear the search context, but NOT the visited lists
    ff_clear(ڳولا_ctx);

    // clear visited list if wanted
    إذا (free_گهميل == TRUE)
	vim_findfile_free_گهميل(ڳولا_ctx);
    وإلا
    {
	// Reuse old visited lists. Get the visited list for the given
	// filename. If no list for the current filename exists, creates a new
	// one.
	ڳولا_ctx->ffsc_گهميل_list = ff_get_گهميل_list(filename,
					filenamelen, &ڳولا_ctx->ffsc_گهميل_lists_list);
	إذا (ڳولا_ctx->ffsc_گهميل_list == عدم)
	    اذهب error_return;
	ڳولا_ctx->ffsc_dir_گهميل_list = ff_get_گهميل_list(filename,
				    filenamelen, &ڳولا_ctx->ffsc_dir_گهميل_lists_list);
	إذا (ڳولا_ctx->ffsc_dir_گهميل_list == عدم)
	    اذهب error_return;
    }

    إذا (ff_expand_بفر.string == عدم)
    {
	ff_expand_بفر.ڊيگهائي = 0;
	ff_expand_بفر.string = alloc(MAXPATHL);
	إذا (ff_expand_بفر.string == عدم)
	    اذهب error_return;
    }

    // Store information on starting dir now if path is relative.
    // If path is absolute, we do that later.
    إذا (رستو[0] == '.'
	    && (vim_ispathsep(رستو[1]) || رستو[1] == NUL)
	    && (!tagfile || vim_strchr(p_cpo, CPO_DOTTAG) == عدم)
	    && rel_fname != عدم)
    {
	صحيح	len = (صحيح)(gettail(rel_fname) - rel_fname);

	إذا (!vim_isAbsName(rel_fname) && len + 1 < MAXPATHL)
	{
	    // Make the start dir an absolute path name.
	    vim_strncpy(ff_expand_بفر.string, rel_fname, len);
	    ff_expand_بفر.ڊيگهائي = len;

	    ڳولا_ctx->ffsc_شروعات_dir.string = ڀريل_Name_save(ff_expand_بفر.string, FALSE);
	    إذا (ڳولا_ctx->ffsc_شروعات_dir.string == عدم)
		اذهب error_return;
	    ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي = STRLEN(ڳولا_ctx->ffsc_شروعات_dir.string);
	}
	وإلا
	{
	    ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي = len;
	    ڳولا_ctx->ffsc_شروعات_dir.string = vim_strnsave(rel_fname,
		ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي);
	    إذا (ڳولا_ctx->ffsc_شروعات_dir.string == عدم)
		اذهب error_return;
	}

	إذا (*++رستو != NUL)
	    ++رستو;
    }
    وإلا إذا (*رستو == NUL || !vim_isAbsName(رستو))
    {
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
	// "c:dir" needs "c:" to be expanded, otherwise use current dir
	إذا (*رستو != NUL && رستو[1] == ':')
	{
	    char_u  drive[3];

	    drive[0] = رستو[0];
	    drive[1] = ':';
	    drive[2] = NUL;
	    إذا (vim_ڀريل_Name(drive, ff_expand_بفر.string, MAXPATHL, TRUE) == FAIL)
		اذهب error_return;
	    رستو += 2;
	}
	وإلا
#جيڪڏهن_پورو
	إذا (mch_dirname(ff_expand_بفر.string, MAXPATHL) == FAIL)
	    اذهب error_return;

	ff_expand_بفر.ڊيگهائي = STRLEN(ff_expand_بفر.string);

	ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي = ff_expand_بفر.ڊيگهائي;
	ڳولا_ctx->ffsc_شروعات_dir.string = vim_strnsave(ff_expand_بفر.string,
	    ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي);
	إذا (ڳولا_ctx->ffsc_شروعات_dir.string == عدم)
	    اذهب error_return;
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
    إذا (stopdirs != عدم)
    {
	char_u	*walker = stopdirs;
	صحيح	dircount;

	طالما (*walker == ';')
	    walker++;

	dircount = 1;
	ڳولا_ctx->ffsc_stopdirs_v = ALLOC_ONE(string_T);

	إذا (ڳولا_ctx->ffsc_stopdirs_v != عدم)
	{
	    string_T	*tmp;			// for convenience

	    افعل
	    {
		char_u	*helper;
		فراغ	*ptr;
		size_t	len;

		helper = walker;
		ptr = vim_realloc(ڳولا_ctx->ffsc_stopdirs_v,
					   (dircount + 1) * حجم(string_T));
		إذا (ptr)
		    ڳولا_ctx->ffsc_stopdirs_v = ptr;
		وإلا
		    // ignore, keep what we have and continue
		    اكسر;
		walker = vim_strchr(walker, ';');
		len = walker ? (size_t)(walker - helper) : STRLEN(helper);
		// "" means ascent till top of directory tree.

		إذا (*helper != NUL && !vim_isAbsName(helper)
							 && len + 1 < MAXPATHL)
		{
		    // Make the stop dir an absolute path name.
		    vim_strncpy(ff_expand_بفر.string, helper, len);
		    ff_expand_بفر.ڊيگهائي = len;

		    tmp = &ڳولا_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->string = ڀريل_Name_save(ff_expand_بفر.string, FALSE);
		    إذا (tmp->string != عدم)
			tmp->ڊيگهائي = STRLEN(tmp->string);
		}
		وإلا
		{
		    tmp = &ڳولا_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->ڊيگهائي = len;
		    tmp->string = vim_strnsave(helper, tmp->ڊيگهائي);
		    إذا (tmp->string == عدم)
			tmp->ڊيگهائي = 0;
		}
		إذا (walker)
		    walker++;
		dircount++;

	    } طالما (walker != عدم);

	    tmp = &ڳولا_ctx->ffsc_stopdirs_v[dircount - 1];
	    tmp->string = عدم;
	    tmp->ڊيگهائي = 0;
	}
    }

    ڳولا_ctx->ffsc_level = level;

    /*
     * split into:
     *  -fix path
     *  -wildcard_stuff (might be NULL)
     */
    wc_part = vim_strchr(رستو, '*');
    إذا (wc_part != عدم)
    {
	صحيح	llevel;
	حرف	*errpt;

	// save the fix part of the path
	ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي = (size_t)(wc_part - رستو);
	ڳولا_ctx->ffsc_fix_رستو.string = vim_strnsave(رستو,
	    ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي);
	إذا (ڳولا_ctx->ffsc_fix_رستو.string == عدم)
	    اذهب error_return;

	/*
	 * copy wc_path and add restricts to the '**' wildcard.
	 * The octet after a '**' is used as a (binary) counter.
	 * So '**3' is transposed to '**^C' ('^C' is ASCII value 3)
	 * or '**76' is transposed to '**N'( 'N' is ASCII value 76).
	 * If no restrict is given after '**' the default is used.
	 * Due to this technique the path looks awful if you print it as a
	 * string.
	 */
	ff_expand_بفر.ڊيگهائي = 0;
	طالما (*wc_part != NUL)
	{
	    إذا (ff_expand_بفر.ڊيگهائي + 5 >= MAXPATHL)
	    {
		emsg(_(e_رستو_too_long_for_completion));
		اكسر;
	    }
	    إذا (STRNCMP(wc_part, "**", 2) == 0)
	    {
		ff_expand_بفر.string[ff_expand_بفر.ڊيگهائي++] = *wc_part++;
		ff_expand_بفر.string[ff_expand_بفر.ڊيگهائي++] = *wc_part++;

		llevel = strtol((حرف *)wc_part, &errpt, 10);
		إذا ((char_u *)errpt != wc_part && llevel > 0 && llevel < 255)
		    ff_expand_بفر.string[ff_expand_بفر.ڊيگهائي++] = llevel;
		وإلا إذا ((char_u *)errpt != wc_part && llevel == 0)
		    // restrict is 0 -> remove already added '**'
		    ff_expand_بفر.ڊيگهائي -= 2;
		وإلا
		    ff_expand_بفر.string[ff_expand_بفر.ڊيگهائي++] = FF_MAX_تارو_تارو_EXPAND;
		wc_part = (char_u *)errpt;
		إذا (*wc_part != NUL && !vim_ispathsep(*wc_part))
		{
		    semsg(_(e_invalid_رستو_عدد_must_be_at_پڇاڙي_جو_رستو_or_be_followed_by_str), PATHSEPSTR);
		    اذهب error_return;
		}
	    }
	    وإلا
		ff_expand_بفر.string[ff_expand_بفر.ڊيگهائي++] = *wc_part++;
	}
	ff_expand_بفر.string[ff_expand_بفر.ڊيگهائي] = NUL;

	ڳولا_ctx->ffsc_wc_رستو.ڊيگهائي = ff_expand_بفر.ڊيگهائي;
	ڳولا_ctx->ffsc_wc_رستو.string = vim_strnsave(ff_expand_بفر.string,
	    ڳولا_ctx->ffsc_wc_رستو.ڊيگهائي);
	إذا (ڳولا_ctx->ffsc_wc_رستو.string == عدم)
	    اذهب error_return;
    }
    وإلا
    {
	ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي = STRLEN(رستو);
	ڳولا_ctx->ffsc_fix_رستو.string = vim_strnsave(رستو,
	    ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي);
	إذا (ڳولا_ctx->ffsc_fix_رستو.string == عدم)
	    اذهب error_return;
    }

    إذا (ڳولا_ctx->ffsc_شروعات_dir.string == عدم)
    {
	// store the fix part as startdir.
	// This is needed if the parameter path is fully qualified.
	ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي = ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي;
	ڳولا_ctx->ffsc_شروعات_dir.string = vim_strnsave(ڳولا_ctx->ffsc_fix_رستو.string,
	    ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي);
	إذا (ڳولا_ctx->ffsc_شروعات_dir.string == عدم)
	    اذهب error_return;
	ڳولا_ctx->ffsc_fix_رستو.string[0] = NUL;
	ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي = 0;
    }

    // create an absolute path
    إذا (ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي
	    + ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي + 3 >= MAXPATHL)
    {
	emsg(_(e_رستو_too_long_for_completion));
	اذهب error_return;
    }

    جوڙيو_sep = !after_pathsep(ڳولا_ctx->ffsc_شروعات_dir.string,
	ڳولا_ctx->ffsc_شروعات_dir.string + ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي);
    ff_expand_بفر.ڊيگهائي = vim_snprintf(
	    (حرف *)ff_expand_بفر.string,
	    MAXPATHL,
	    "%s%s",
	    ڳولا_ctx->ffsc_شروعات_dir.string,
	    جوڙيو_sep ? PATHSEPSTR : "");

    {
	size_t	bufsize = ff_expand_بفر.ڊيگهائي + ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي + 1;
	char_u	*buf = alloc(bufsize);

	إذا (buf == عدم)
	    اذهب error_return;

	vim_snprintf(
		(حرف *)buf,
		bufsize,
		"%s%s",
		ff_expand_بفر.string,
		ڳولا_ctx->ffsc_fix_رستو.string);
	إذا (mch_isdir(buf))
	{
	    إذا (ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي > 0)
	    {
		جوڙيو_sep = !after_pathsep(ڳولا_ctx->ffsc_fix_رستو.string,
		    ڳولا_ctx->ffsc_fix_رستو.string + ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي);
		ff_expand_بفر.ڊيگهائي += vim_snprintf(
			(حرف *)ff_expand_بفر.string + ff_expand_بفر.ڊيگهائي,
			MAXPATHL - ff_expand_بفر.ڊيگهائي,
			"%s%s",
			ڳولا_ctx->ffsc_fix_رستو.string,
			جوڙيو_sep ? PATHSEPSTR : "");
	    }
	}
	وإلا
	{
	    char_u *p = gettail(ڳولا_ctx->ffsc_fix_رستو.string);
	    صحيح    len = (صحيح)ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي;

	    إذا (p > ڳولا_ctx->ffsc_fix_رستو.string)
	    {
		// do not add '..' to the path and start upwards searching
		len = (صحيح)(p - ڳولا_ctx->ffsc_fix_رستو.string) - 1;
		إذا ((len >= 2
			&& STRNCMP(ڳولا_ctx->ffsc_fix_رستو.string, "..", 2) == 0)
			&& (len == 2 || ڳولا_ctx->ffsc_fix_رستو.string[2] == PATHSEP))
		{
		    vim_free(buf);
		    اذهب error_return;
		}

		جوڙيو_sep = !after_pathsep(ڳولا_ctx->ffsc_fix_رستو.string,
		    ڳولا_ctx->ffsc_fix_رستو.string + ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي);
		ff_expand_بفر.ڊيگهائي += vim_snprintf(
			(حرف *)ff_expand_بفر.string + ff_expand_بفر.ڊيگهائي,
			MAXPATHL - ff_expand_بفر.ڊيگهائي,
			"%.*s%s",
			len,
			ڳولا_ctx->ffsc_fix_رستو.string,
			جوڙيو_sep ? PATHSEPSTR : "");
	    }

	    إذا (ڳولا_ctx->ffsc_wc_رستو.string != عدم)
	    {
		size_t	tempsize = (ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي - len)
				+ ڳولا_ctx->ffsc_wc_رستو.ڊيگهائي
				+ 1;
		char_u	*عارضي = alloc(tempsize);

		إذا (عارضي == عدم)
		{
		    vim_free(buf);
		    vim_free(عارضي);
		    اذهب error_return;
		}

		ڳولا_ctx->ffsc_wc_رستو.ڊيگهائي = vim_snprintf(
			    (حرف *)عارضي,
			    tempsize,
			    "%s%s",
			    ڳولا_ctx->ffsc_fix_رستو.string + len,
			    ڳولا_ctx->ffsc_wc_رستو.string);
		vim_free(ڳولا_ctx->ffsc_wc_رستو.string);
		ڳولا_ctx->ffsc_wc_رستو.string = عارضي;
	    }
	}
	vim_free(buf);
    }

    sptr = ff_create_ڍير_element(ff_expand_بفر.string,
			ff_expand_بفر.ڊيگهائي,
			ڳولا_ctx->ffsc_wc_رستو.string,
			ڳولا_ctx->ffsc_wc_رستو.ڊيگهائي,
			level,
			0);

    إذا (sptr == عدم)
	اذهب error_return;

    ff_ڌڪو(ڳولا_ctx, sptr);

    ڳولا_ctx->ffsc_file_منزل_ڳولا.ڊيگهائي = filenamelen;
    ڳولا_ctx->ffsc_file_منزل_ڳولا.string = vim_strnsave(filename,
	ڳولا_ctx->ffsc_file_منزل_ڳولا.ڊيگهائي);
    إذا (ڳولا_ctx->ffsc_file_منزل_ڳولا.string == عدم)
	اذهب error_return;

    أرجع ڳولا_ctx;

error_return:
    /*
     * We clear the search context now!
     * Even when the caller gave us a (perhaps valid) context we free it here,
     * as we might have already destroyed it.
     */
    vim_findfile_cleanup(ڳولا_ctx);
    أرجع عدم;
}

/*
 * Get the stopdir string.  Check that ';' is not escaped.
 */
    char_u *
vim_findfile_stopdir(char_u *buf)
{
    char_u	*r_ptr = buf;
    char_u	*r_ptr_پڇاڙي = عدم;	    // points to NUL at end of string "r_ptr"

    طالما (*r_ptr != NUL && *r_ptr != ';')
    {
	إذا (r_ptr[0] == '\\' && r_ptr[1] == ';')
	{
	    // Overwrite the escape char,
	    // use STRLEN(r_ptr) to move the trailing '\0'.
	    إذا (r_ptr_پڇاڙي == عدم)
		r_ptr_پڇاڙي = r_ptr + STRLEN(r_ptr);
	    mch_memmove(r_ptr, r_ptr + 1,
		(size_t)(r_ptr_پڇاڙي - (r_ptr + 1)) + 1);	// +1 for NUL
	    r_ptr++;
	    --r_ptr_پڇاڙي;
	}
	r_ptr++;
    }
    إذا (*r_ptr == ';')
    {
	*r_ptr = NUL;
	r_ptr++;
    }
    وإلا إذا (*r_ptr == NUL)
	r_ptr = عدم;
    أرجع r_ptr;
}

/*
 * Clean up the given search context. Can handle a NULL pointer.
 */
    فراغ
vim_findfile_cleanup(فراغ *ctx)
{
    إذا (ctx == عدم)
	أرجع;

    vim_findfile_free_گهميل(ctx);
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
vim_findfile(فراغ *ڳولا_ctx_arg)
{
    string_T	file_رستو;
    string_T	rest_جو_wildcards;
    char_u	*رستو_پڇاڙي = عدم;
    ff_ڍير_T	*stackp;
    ff_ڳولا_ctx_T *ڳولا_ctx;

    إذا (ڳولا_ctx_arg == عدم)
	أرجع عدم;

    ڳولا_ctx = (ff_ڳولا_ctx_T *)ڳولا_ctx_arg;

    /*
     * filepath is used as buffer for various actions and as the storage to
     * return a found filename.
     */
    إذا ((file_رستو.string = alloc(MAXPATHL)) == عدم)
	أرجع عدم;

    // store the end of the start dir -- needed for upward search
    إذا (ڳولا_ctx->ffsc_شروعات_dir.string != عدم)
	رستو_پڇاڙي = &ڳولا_ctx->ffsc_شروعات_dir.string[
					    ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي];

    // upward search loop
    لكل (;;)
    {
	// downward search loop
	لكل (;;)
	{
	    // check if user wants to stop the search
	    ui_breakcheck();
	    إذا (got_int)
		اكسر;

	    // get directory to work on from stack
	    stackp = ff_ڪڍو(ڳولا_ctx);
	    إذا (stackp == عدم)
		اكسر;

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
	    إذا (stackp->ffs_filearray == عدم
		    && ff_check_گهميل(&ڳولا_ctx->ffsc_dir_گهميل_list
							  ->ffvl_گهميل_list,
			    stackp->ffs_fix_رستو.string,
			    stackp->ffs_fix_رستو.ڊيگهائي,
			    stackp->ffs_wc_رستو.string,
			    stackp->ffs_wc_رستو.ڊيگهائي) == FAIL)
	    {
#جيڪڏهن_وصف_ٿيل FF_VERBOSE
		إذا (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("Already Searched: %s (%s)",
				   stackp->ffs_fix_رستو.string, stackp->ffs_wc_رستو.string);
		    // don't overwrite this either
		    نياپو_puts("\n");
		    verbose_leave_scroll();
		}
#جيڪڏهن_پورو
		ff_free_ڍير_element(stackp);
		استمر;
	    }
#جيڪڏهن_وصف_ٿيل FF_VERBOSE
	    وإلا إذا (p_verbose >= 5)
	    {
		verbose_enter_scroll();
		smsg("Searching: %s (%s)",
				   stackp->ffs_fix_رستو.string, stackp->ffs_wc_رستو.string);
		// don't overwrite this either
		نياپو_puts("\n");
		verbose_leave_scroll();
	    }
#جيڪڏهن_پورو

	    // check depth
	    إذا (stackp->ffs_level <= 0)
	    {
		ff_free_ڍير_element(stackp);
		استمر;
	    }

	    file_رستو.string[0] = NUL;
	    file_رستو.ڊيگهائي = 0;

	    /*
	     * If no filearray till now expand wildcards
	     * The function expand_wildcards() can handle an array of paths
	     * and all possible expands are returned in one array. We use this
	     * to handle the expansion of '**' into an empty string.
	     */
	    إذا (stackp->ffs_filearray == عدم)
	    {
		char_u *dirptrs[2];

		// we use filepath to build the path expand_wildcards() should
		// expand.
		dirptrs[0] = file_رستو.string;
		dirptrs[1] = عدم;

		// if we have a start dir copy it in
		إذا (!vim_isAbsName(stackp->ffs_fix_رستو.string)
						&& ڳولا_ctx->ffsc_شروعات_dir.string)
		{
		    إذا (ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي + 1 < MAXPATHL)
		    {
			صحيح جوڙيو_sep = !after_pathsep(ڳولا_ctx->ffsc_شروعات_dir.string,
			    ڳولا_ctx->ffsc_شروعات_dir.string + ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي);
			file_رستو.ڊيگهائي = vim_snprintf(
			    (حرف *)file_رستو.string,
			    MAXPATHL,
			    "%s%s",
			    ڳولا_ctx->ffsc_شروعات_dir.string,
			    جوڙيو_sep ? PATHSEPSTR : "");
		    }
		    وإلا
		    {
			ff_free_ڍير_element(stackp);
			اذهب fail;
		    }
		}

		// append the fix part of the search path
		إذا (file_رستو.ڊيگهائي + stackp->ffs_fix_رستو.ڊيگهائي + 1 < MAXPATHL)
		{
		    صحيح جوڙيو_sep = !after_pathsep(stackp->ffs_fix_رستو.string,
			stackp->ffs_fix_رستو.string + stackp->ffs_fix_رستو.ڊيگهائي);
		    file_رستو.ڊيگهائي += vim_snprintf(
			(حرف *)file_رستو.string + file_رستو.ڊيگهائي,
			MAXPATHL - file_رستو.ڊيگهائي,
			"%s%s",
			stackp->ffs_fix_رستو.string,
			جوڙيو_sep ? PATHSEPSTR : "");
		}
		وإلا
		{
		    ff_free_ڍير_element(stackp);
		    اذهب fail;
		}

		rest_جو_wildcards.string = stackp->ffs_wc_رستو.string;
		rest_جو_wildcards.ڊيگهائي = stackp->ffs_wc_رستو.ڊيگهائي;
		إذا (*rest_جو_wildcards.string != NUL)
		{
		    إذا (STRNCMP(rest_جو_wildcards.string, "**", 2) == 0)
		    {
			char_u	*p;

			// pointer to the restrict byte
			// The restrict byte is not a character!
			p = rest_جو_wildcards.string + 2;

			إذا (*p > 0)
			{
			    (*p)--;
			    إذا (file_رستو.ڊيگهائي + 1 < MAXPATHL)
				file_رستو.string[file_رستو.ڊيگهائي++] = '*';
			    وإلا
			    {
				ff_free_ڍير_element(stackp);
				اذهب fail;
			    }
			}

			إذا (*p == 0)
			{
			    // remove '**<numb> from wildcards
			    mch_memmove(rest_جو_wildcards.string,
					rest_جو_wildcards.string + 3,
					(size_t)(rest_جو_wildcards.ڊيگهائي - 3) + 1);    // +1 for NUL
			    rest_جو_wildcards.ڊيگهائي -= 3;
			    stackp->ffs_wc_رستو.ڊيگهائي = rest_جو_wildcards.ڊيگهائي;
			}
			وإلا
			{
			    rest_جو_wildcards.string += 3;
			    rest_جو_wildcards.ڊيگهائي -= 3;
			}

			إذا (stackp->ffs_تارو_تارو_خالي_جايون == 0)
			{
			    // if not done before, expand '**' to empty
			    stackp->ffs_تارو_تارو_خالي_جايون = 1;
			    dirptrs[1] = stackp->ffs_fix_رستو.string;
			}
		    }

		    /*
		     * Here we copy until the next path separator or the end of
		     * the path. If we stop at a path separator, there is
		     * still something else left. This is handled below by
		     * pushing every directory returned from expand_wildcards()
		     * on the stack again for further search.
		     */
		    طالما (*rest_جو_wildcards.string
			    && !vim_ispathsep(*rest_جو_wildcards.string))
		    {
			إذا (file_رستو.ڊيگهائي + 1 < MAXPATHL)
			{
			    file_رستو.string[file_رستو.ڊيگهائي++] = *rest_جو_wildcards.string++;
			    --rest_جو_wildcards.ڊيگهائي;
			}
			وإلا
			{
			    ff_free_ڍير_element(stackp);
			    اذهب fail;
			}
		    }

		    file_رستو.string[file_رستو.ڊيگهائي] = NUL;
		    إذا (vim_ispathsep(*rest_جو_wildcards.string))
		    {
			rest_جو_wildcards.string++;
			rest_جو_wildcards.ڊيگهائي--;
		    }
		}

		/*
		 * Expand wildcards like "*" and "$VAR".
		 * If the path is a URL don't try this.
		 */
		إذا (رستو_with_url(dirptrs[0]))
		{
		    stackp->ffs_filearray = ALLOC_ONE(char_u *);
		    إذا (stackp->ffs_filearray != عدم
			    && (stackp->ffs_filearray[0]
				= vim_strnsave(dirptrs[0], file_رستو.ڊيگهائي)) != عدم)
			stackp->ffs_filearray_سائيز = 1;
		    وإلا
			stackp->ffs_filearray_سائيز = 0;
		}
		وإلا
		    // Add EW_NOTWILD because the expanded path may contain
		    // wildcard characters that are to be taken literally.
		    // This is a bit of a hack.
		    expand_wildcards((dirptrs[1] == عدم) ? 1 : 2, dirptrs,
			    &stackp->ffs_filearray_سائيز,
			    &stackp->ffs_filearray,
			    EW_DIR|EW_ADDSLASH|EW_SILENT|EW_NOTWILD);

		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 0;
	    }
	    وإلا
	    {
		rest_جو_wildcards.string = &stackp->ffs_wc_رستو.string[
							stackp->ffs_wc_رستو.ڊيگهائي];
		rest_جو_wildcards.ڊيگهائي = 0;
	    }

	    إذا (stackp->ffs_stage == 0)
	    {
		صحيح i;

		// this is the first time we work on this directory
		إذا (*rest_جو_wildcards.string == NUL)
		{
		    size_t  len;
		    char_u  *suf;

		    /*
		     * We don't have further wildcards to expand, so we have to
		     * check for the final file now.
		     */
		    لكل (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_سائيز; ++i)
		    {
			إذا (!رستو_with_url(stackp->ffs_filearray[i])
				      && !mch_isdir(stackp->ffs_filearray[i]))
			    استمر;   // not a directory

			// prepare the filename to be checked for existence
			//هيٺ
			len = STRLEN(stackp->ffs_filearray[i]);
			إذا (len + 1 + ڳولا_ctx->ffsc_file_منزل_ڳولا.ڊيگهائي
									< MAXPATHL)
			{
			    صحيح	جوڙيو_sep = !after_pathsep(stackp->ffs_filearray[i],
				    stackp->ffs_filearray[i] + len);
			    file_رستو.ڊيگهائي = vim_snprintf(
				    (حرف *)file_رستو.string,
				    MAXPATHL,
				    "%s%s%s",
				    stackp->ffs_filearray[i],
				    جوڙيو_sep ? PATHSEPSTR : "",
				    ڳولا_ctx->ffsc_file_منزل_ڳولا.string);
			}
			وإلا
			{
			    ff_free_ڍير_element(stackp);
			    اذهب fail;
			}

			/*
			 * Try without extra suffix and then with suffixes
			 * from 'suffixesadd'.
			 */
			len = file_رستو.ڊيگهائي;
			إذا (ڳولا_ctx->ffsc_tagfile)
			    suf = (char_u *)"";
			وإلا
			    suf = curbuf->b_p_sua;
			لكل (;;)
			{
			    // if file exists and we didn't already find it
			    إذا ((رستو_with_url(file_رستو.string)
				  || (mch_getperm(file_رستو.string) >= 0
				      && (ڳولا_ctx->ffsc_ڳوليو_what
							      == FINDFILE_BOTH
					  || ((ڳولا_ctx->ffsc_ڳوليو_what
							      == FINDFILE_DIR)
						   == mch_isdir(file_رستو.string)))))
#جيڪڏهن_وصف_نه_ٿيل FF_VERBOSE
				    && (ff_check_گهميل(
					    &ڳولا_ctx->ffsc_گهميل_list
							   ->ffvl_گهميل_list,
					    file_رستو.string,
					    file_رستو.ڊيگهائي,
					    (char_u *)"", 0) == OK)
#جيڪڏهن_پورو
			       )
			    {
#جيڪڏهن_وصف_ٿيل FF_VERBOSE
				إذا (ff_check_گهميل(
					    &ڳولا_ctx->ffsc_گهميل_list
							   ->ffvl_گهميل_list,
					      file_رستو.string,
					      file_رستو.ڊيگهائي,
					      (char_u *)"", 0) == FAIL)
				{
				    إذا (p_verbose >= 5)
				    {
					verbose_enter_scroll();
					smsg("Already: %s", file_رستو.string);
					// don't overwrite this either
					نياپو_puts("\n");
					verbose_leave_scroll();
				    }
				    استمر;
				}
#جيڪڏهن_پورو

				// push dir to examine rest of subdirs later
				stackp->ffs_filearray_cur = i + 1;
				ff_ڌڪو(ڳولا_ctx, stackp);

				إذا (!رستو_with_url(file_رستو.string))
				    file_رستو.ڊيگهائي = simplify_filename(file_رستو.string);

				إذا (mch_dirname(ff_expand_بفر.string, MAXPATHL)
									== OK)
				{
				    char_u  *p;

				    ff_expand_بفر.ڊيگهائي = STRLEN(ff_expand_بفر.string);
				    p = shorten_fname(file_رستو.string,
							    ff_expand_بفر.string);
				    إذا (p != عدم)
				    {
					mch_memmove(file_رستو.string, p,
					    (size_t)((file_رستو.string + file_رستو.ڊيگهائي) - p) + 1);  // +1 for NUL
					file_رستو.ڊيگهائي -= (p - file_رستو.string);
				    }
				}
#جيڪڏهن_وصف_ٿيل FF_VERBOSE
				إذا (p_verbose >= 5)
				{
				    verbose_enter_scroll();
				    smsg("HIT: %s", file_رستو.string);
				    // don't overwrite this either
				    نياپو_puts("\n");
				    verbose_leave_scroll();
				}
#جيڪڏهن_پورو
				أرجع file_رستو.string;
			    }

			    // Not found or found already, try next suffix.
			    إذا (*suf == NUL)
				اكسر;
			    file_رستو.ڊيگهائي = len + copy_option_part(&suf,
				  file_رستو.string + len,
				  (صحيح)(MAXPATHL - len), ",");
			}
		    }
		}
		وإلا
		{
		    /*
		     * still wildcards left, push the directories for further
		     * search
		     */
		    لكل (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_سائيز; ++i)
		    {
			إذا (!mch_isdir(stackp->ffs_filearray[i]))
			    استمر;	// not a directory

			ff_ڌڪو(ڳولا_ctx,
				ff_create_ڍير_element(
						     stackp->ffs_filearray[i],
						     STRLEN(stackp->ffs_filearray[i]),
						     rest_جو_wildcards.string,
						     rest_جو_wildcards.ڊيگهائي,
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
	    إذا (STRNCMP(stackp->ffs_wc_رستو.string, "**", 2) == 0)
	    {
		صحيح i;

		لكل (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_سائيز; ++i)
		{
		    إذا (fnamecmp(stackp->ffs_filearray[i],
						   stackp->ffs_fix_رستو.string) == 0)
			استمر; // don't repush same directory
		    إذا (!mch_isdir(stackp->ffs_filearray[i]))
			استمر;   // not a directory
		    ff_ڌڪو(ڳولا_ctx,
			    ff_create_ڍير_element(
				stackp->ffs_filearray[i],
				STRLEN(stackp->ffs_filearray[i]),
				stackp->ffs_wc_رستو.string,
				stackp->ffs_wc_رستو.ڊيگهائي,
				stackp->ffs_level - 1, 1));
		}
	    }

	    // we are done with the current directory
	    ff_free_ڍير_element(stackp);
	}

	// If we reached this, we didn't find anything downwards.
	// Let's check if we should do an upward search.
	إذا (ڳولا_ctx->ffsc_شروعات_dir.string
		&& ڳولا_ctx->ffsc_stopdirs_v != عدم && !got_int)
	{
	    ff_ڍير_T  *sptr;
	    // path_end may point to the NUL or the previous path separator
	    صحيح plen = (رستو_پڇاڙي - ڳولا_ctx->ffsc_شروعات_dir.string)
							  + (*رستو_پڇاڙي != NUL);

	    // is the last starting directory in the stop list?
	    إذا (ff_رستو_اندريون_stoplist(ڳولا_ctx->ffsc_شروعات_dir.string,
				    plen, ڳولا_ctx->ffsc_stopdirs_v) == TRUE)
		اكسر;

	    // cut of last dir
	    طالما (رستو_پڇاڙي > ڳولا_ctx->ffsc_شروعات_dir.string
						  && vim_ispathsep(*رستو_پڇاڙي))
		رستو_پڇاڙي--;
	    طالما (رستو_پڇاڙي > ڳولا_ctx->ffsc_شروعات_dir.string
					      && !vim_ispathsep(رستو_پڇاڙي[-1]))
		رستو_پڇاڙي--;
	    *رستو_پڇاڙي = NUL;

	    // we may have shortened search_ctx->ffsc_start_dir, so update it's length
	    ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي = (size_t)(رستو_پڇاڙي - ڳولا_ctx->ffsc_شروعات_dir.string);
	    رستو_پڇاڙي--;

	    إذا (*ڳولا_ctx->ffsc_شروعات_dir.string == NUL)
		اكسر;

	    إذا (ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي + 1
		    + ڳولا_ctx->ffsc_fix_رستو.ڊيگهائي < MAXPATHL)
	    {
		صحيح جوڙيو_sep = !after_pathsep(ڳولا_ctx->ffsc_شروعات_dir.string,
			    ڳولا_ctx->ffsc_شروعات_dir.string + ڳولا_ctx->ffsc_شروعات_dir.ڊيگهائي);
		file_رستو.ڊيگهائي = vim_snprintf(
			(حرف *)file_رستو.string,
			MAXPATHL,
			"%s%s%s",
			ڳولا_ctx->ffsc_شروعات_dir.string,
			جوڙيو_sep ? PATHSEPSTR : "",
			ڳولا_ctx->ffsc_fix_رستو.string);
	    }
	    وإلا
		اذهب fail;

	    // create a new stack entry
	    sptr = ff_create_ڍير_element(file_رستو.string, file_رستو.ڊيگهائي,
		    ڳولا_ctx->ffsc_wc_رستو.string, ڳولا_ctx->ffsc_wc_رستو.ڊيگهائي,
		    ڳولا_ctx->ffsc_level, 0);
	    إذا (sptr == عدم)
		اكسر;
	    ff_ڌڪو(ڳولا_ctx, sptr);
	}
	وإلا
	    اكسر;
    }

fail:
    vim_free(file_رستو.string);
    أرجع عدم;
}

/*
 * Free the list of lists of visited files and directories
 * Can handle it if the passed search_context is NULL;
 */
    ساكن فراغ
vim_findfile_free_گهميل(فراغ *ڳولا_ctx_arg)
{
    ff_ڳولا_ctx_T *ڳولا_ctx;

    إذا (ڳولا_ctx_arg == عدم)
	أرجع;

    ڳولا_ctx = (ff_ڳولا_ctx_T *)ڳولا_ctx_arg;
    vim_findfile_free_گهميل_list(&ڳولا_ctx->ffsc_گهميل_lists_list);
    vim_findfile_free_گهميل_list(&ڳولا_ctx->ffsc_dir_گهميل_lists_list);
}

    ساكن فراغ
vim_findfile_free_گهميل_list(ff_گهميل_list_hdr_T **list_headp)
{
    ff_گهميل_list_hdr_T *vp;

    طالما (*list_headp != عدم)
    {
	vp = (*list_headp)->ffvl_ايندڙ;
	ff_free_گهميل_list((*list_headp)->ffvl_گهميل_list);

	vim_free((*list_headp)->ffvl_filename);
	vim_free(*list_headp);
	*list_headp = vp;
    }
    *list_headp = عدم;
}

    ساكن فراغ
ff_free_گهميل_list(ff_گهميل_T *vl)
{
    ff_گهميل_T *vp;

    طالما (vl != عدم)
    {
	vp = vl->ffv_ايندڙ;
	vim_free(vl->ffv_wc_رستو);
	vim_free(vl);
	vl = vp;
    }
    vl = عدم;
}

/*
 * Returns the already visited list for the given filename. If none is found it
 * allocates a new one.
 */
    ساكن ff_گهميل_list_hdr_T*
ff_get_گهميل_list(
    char_u			*filename,
    size_t			filenamelen,
    ff_گهميل_list_hdr_T	**list_headp)
{
    ff_گهميل_list_hdr_T  *retptr = عدم;

    // check if a visited list for the given filename exists
    إذا (*list_headp != عدم)
    {
	retptr = *list_headp;
	طالما (retptr != عدم)
	{
	    إذا (fnamecmp(filename, retptr->ffvl_filename) == 0)
	    {
#جيڪڏهن_وصف_ٿيل FF_VERBOSE
		إذا (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("ff_get_visited_list: FOUND list for %s",
								    filename);
		    // don't overwrite this either
		    نياپو_puts("\n");
		    verbose_leave_scroll();
		}
#جيڪڏهن_پورو
		أرجع retptr;
	    }
	    retptr = retptr->ffvl_ايندڙ;
	}
    }

#جيڪڏهن_وصف_ٿيل FF_VERBOSE
    إذا (p_verbose >= 5)
    {
	verbose_enter_scroll();
	smsg("ff_get_visited_list: new list for %s", filename);
	// don't overwrite this either
	نياپو_puts("\n");
	verbose_leave_scroll();
    }
#جيڪڏهن_پورو

    /*
     * if we reach this we didn't find a list and we have to allocate new list
     */
    retptr = ALLOC_ONE(ff_گهميل_list_hdr_T);
    إذا (retptr == عدم)
	أرجع عدم;

    retptr->ffvl_گهميل_list = عدم;
    retptr->ffvl_filename = vim_strnsave(filename, filenamelen);
    إذا (retptr->ffvl_filename == عدم)
    {
	vim_free(retptr);
	أرجع عدم;
    }
    retptr->ffvl_ايندڙ = *list_headp;
    *list_headp = retptr;

    أرجع retptr;
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
    ساكن صحيح
ff_wc_equal(char_u *s1, char_u *s2)
{
    صحيح		i, j;
    صحيح		c1 = NUL;
    صحيح		c2 = NUL;
    صحيح		prev1 = NUL;
    صحيح		prev2 = NUL;

    إذا (s1 == s2)
	أرجع TRUE;

    إذا (s1 == عدم || s2 == عدم)
	أرجع FALSE;

    لكل (i = 0, j = 0; s1[i] != NUL && s2[j] != NUL;)
    {
	c1 = PTR2CHAR(s1 + i);
	c2 = PTR2CHAR(s2 + j);

	إذا ((p_fic ? MB_TOLOWER(c1) != MB_TOLOWER(c2) : c1 != c2)
		&& (prev1 != '*' || prev2 != '*'))
	    أرجع FALSE;
	prev2 = prev1;
	prev1 = c1;

	i += mb_ptr2len(s1 + i);
	j += mb_ptr2len(s2 + j);
    }
    أرجع s1[i] == s2[j];
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
    ساكن صحيح
ff_check_گهميل(
    ff_گهميل_T	**گهميل_list,
    char_u		*fname,
    size_t		fnamelen,
    char_u		*wc_رستو,
    size_t		wc_pathlen)
{
    ff_گهميل_T	*vp;
#جيڪڏهن_وصف_ٿيل UNIX
    stat_T		st;
    صحيح			url = FALSE;
#جيڪڏهن_پورو

    // For a URL we only compare the name, otherwise we compare the
    // device/inode (unix) or the full path name (not Unix).
    إذا (رستو_with_url(fname))
    {
	vim_strncpy(ff_expand_بفر.string, fname, fnamelen);
	ff_expand_بفر.ڊيگهائي = fnamelen;
#جيڪڏهن_وصف_ٿيل UNIX
	url = TRUE;
#جيڪڏهن_پورو
    }
    وإلا
    {
	ff_expand_بفر.string[0] = NUL;
	ff_expand_بفر.ڊيگهائي = 0;
#جيڪڏهن_وصف_ٿيل UNIX
	إذا (mch_stat((حرف *)fname, &st) < 0)
	    أرجع FAIL;
#وإلا
	إذا (vim_ڀريل_Name(fname, ff_expand_بفر.string, MAXPATHL, TRUE) == FAIL)
	    أرجع FAIL;
	ff_expand_بفر.ڊيگهائي = STRLEN(ff_expand_بفر.string);
#جيڪڏهن_پورو
    }

    // check against list of already visited files
    لكل (vp = *گهميل_list; vp != عدم; vp = vp->ffv_ايندڙ)
    {
	إذا (
#جيڪڏهن_وصف_ٿيل UNIX
		!url ? (vp->ffv_dev_valid && vp->ffv_dev == st.st_dev
						  && vp->ffv_ino == st.st_ino)
		     :
#جيڪڏهن_پورو
		fnamecmp(vp->ffv_fname, ff_expand_بفر.string) == 0
	   )
	{
	    // are the wildcard parts equal
	    إذا (ff_wc_equal(vp->ffv_wc_رستو, wc_رستو) == TRUE)
		// already visited
		أرجع FAIL;
	}
    }

    /*
     * New file/dir.  Add it to the list of visited files/dirs.
     */
    vp = alloc(
	     offsetof(ff_گهميل_T, ffv_fname) + ff_expand_بفر.ڊيگهائي + 1);
    إذا (vp == عدم)
	أرجع OK;

#جيڪڏهن_وصف_ٿيل UNIX
    إذا (!url)
    {
	vp->ffv_dev_valid = TRUE;
	vp->ffv_dev = st.st_dev;
	vp->ffv_ino = st.st_ino;
	vp->ffv_fname[0] = NUL;
    }
    وإلا
    {
	vp->ffv_dev_valid = FALSE;
#جيڪڏهن_پورو
	STRCPY(vp->ffv_fname, ff_expand_بفر.string);
#جيڪڏهن_وصف_ٿيل UNIX
    }
#جيڪڏهن_پورو
    إذا (wc_رستو != عدم)
	vp->ffv_wc_رستو = vim_strnsave(wc_رستو, wc_pathlen);
    وإلا
	vp->ffv_wc_رستو = عدم;

    vp->ffv_ايندڙ = *گهميل_list;
    *گهميل_list = vp;

    أرجع OK;
}

/*
 * create stack element from given path pieces
 */
    ساكن ff_ڍير_T *
ff_create_ڍير_element(
    char_u	*fix_part,
    size_t	fix_partlen,
    char_u	*wc_part,
    size_t	wc_partlen,
    صحيح		level,
    صحيح		تارو_تارو_خالي_جايون)
{
    ff_ڍير_T	*new;

    new = ALLOC_ONE(ff_ڍير_T);
    إذا (new == عدم)
	أرجع عدم;

    new->ffs_prev	   = عدم;
    new->ffs_filearray	   = عدم;
    new->ffs_filearray_سائيز = 0;
    new->ffs_filearray_cur  = 0;
    new->ffs_stage	   = 0;
    new->ffs_level	   = level;
    new->ffs_تارو_تارو_خالي_جايون = تارو_تارو_خالي_جايون;

    // the following saves NULL pointer checks in vim_findfile
    إذا (fix_part == عدم)
    {
	fix_part = (char_u *)"";
	fix_partlen = 0;
    }
    new->ffs_fix_رستو.string = vim_strnsave(fix_part, fix_partlen);
    new->ffs_fix_رستو.ڊيگهائي = fix_partlen;

    إذا (wc_part == عدم)
    {
	wc_part = (char_u *)"";
	wc_partlen = 0;
    }
    new->ffs_wc_رستو.string = vim_strnsave(wc_part, wc_partlen);
    new->ffs_wc_رستو.ڊيگهائي = wc_partlen;

    إذا (new->ffs_fix_رستو.string == عدم || new->ffs_wc_رستو.string == عدم)
    {
	ff_free_ڍير_element(new);
	new = عدم;
    }

    أرجع new;
}

/*
 * Push a dir on the directory stack.
 */
    ساكن فراغ
ff_ڌڪو(ff_ڳولا_ctx_T *ڳولا_ctx, ff_ڍير_T *ڍير_ptr)
{
    // check for NULL pointer, not to return an error to the user, but
    // to prevent a crash
    إذا (ڍير_ptr == عدم)
	أرجع;

    ڍير_ptr->ffs_prev = ڳولا_ctx->ffsc_ڍير_ptr;
    ڳولا_ctx->ffsc_ڍير_ptr = ڍير_ptr;
}

/*
 * Pop a dir from the directory stack.
 * Returns NULL if stack is empty.
 */
    ساكن ff_ڍير_T *
ff_ڪڍو(ff_ڳولا_ctx_T *ڳولا_ctx)
{
    ff_ڍير_T  *sptr;

    sptr = ڳولا_ctx->ffsc_ڍير_ptr;
    إذا (ڳولا_ctx->ffsc_ڍير_ptr != عدم)
	ڳولا_ctx->ffsc_ڍير_ptr = ڳولا_ctx->ffsc_ڍير_ptr->ffs_prev;

    أرجع sptr;
}

/*
 * free the given stack element
 */
    ساكن فراغ
ff_free_ڍير_element(ff_ڍير_T *ڍير_ptr)
{
    // VIM_CLEAR_STRING handles possible NULL pointers
    VIM_CLEAR_STRING(ڍير_ptr->ffs_fix_رستو);
    VIM_CLEAR_STRING(ڍير_ptr->ffs_wc_رستو);

    إذا (ڍير_ptr->ffs_filearray != عدم)
	FreeWild(ڍير_ptr->ffs_filearray_سائيز, ڍير_ptr->ffs_filearray);

    vim_free(ڍير_ptr);
}

/*
 * Clear the search context, but NOT the visited list.
 */
    ساكن فراغ
ff_clear(ff_ڳولا_ctx_T *ڳولا_ctx)
{
    ff_ڍير_T   *sptr;

    // clear up stack
    طالما ((sptr = ff_ڪڍو(ڳولا_ctx)) != عدم)
	ff_free_ڍير_element(sptr);

    إذا (ڳولا_ctx->ffsc_stopdirs_v != عدم)
    {
	صحيح  i = 0;

	طالما (ڳولا_ctx->ffsc_stopdirs_v[i].string != عدم)
	{
	    vim_free(ڳولا_ctx->ffsc_stopdirs_v[i].string);
	    i++;
	}
	VIM_CLEAR(ڳولا_ctx->ffsc_stopdirs_v);
    }

    // reset everything
    VIM_CLEAR_STRING(ڳولا_ctx->ffsc_file_منزل_ڳولا);
    VIM_CLEAR_STRING(ڳولا_ctx->ffsc_شروعات_dir);
    VIM_CLEAR_STRING(ڳولا_ctx->ffsc_fix_رستو);
    VIM_CLEAR_STRING(ڳولا_ctx->ffsc_wc_رستو);
    ڳولا_ctx->ffsc_level = 0;
}

/*
 * check if the given path is in the stopdirs
 * returns TRUE if yes else FALSE
 */
    ساكن صحيح
ff_رستو_اندريون_stoplist(char_u *رستو, صحيح رستو_len, string_T *stopdirs_v)
{
    صحيح		i = 0;

    // eat up trailing path separators, except the first
    طالما (رستو_len > 1 && vim_ispathsep(رستو[رستو_len - 1]))
	رستو_len--;

    // if no path consider it as match
    إذا (رستو_len == 0)
	أرجع TRUE;

    لكل (i = 0; stopdirs_v[i].string != عدم; i++)
	// match for parent directory. So '/home' also matches
	// '/home/rks'. Check for PATHSEP in stopdirs_v[i], else
	// '/home/r' would also match '/home/rks'
	إذا (fnamencmp(stopdirs_v[i].string, رستو, رستو_len) == 0
		&& ((صحيح)stopdirs_v[i].ڊيگهائي <= رستو_len
		    || vim_ispathsep(stopdirs_v[i].string[رستو_len])))
	    أرجع TRUE;

    أرجع FALSE;
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
ڳوليو_file_اندريون_رستو(
    char_u	*ptr,		// file name
    صحيح		len,		// length of file name
    صحيح		options,
    صحيح		پهريون,		// use count'th matching file name
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_منزل_ڳوليو,	// in/out: modified copy of file name
    حرف	**ڳولا_ctx)	// in/out: state of the search
{
    أرجع ڳوليو_file_اندريون_رستو_option(ptr, len, options, پهريون,
	    *curbuf->b_p_رستو == NUL ? p_رستو : curbuf->b_p_رستو,
	    FINDFILE_BOTH, rel_fname, curbuf->b_p_sua,
	    file_منزل_ڳوليو, ڳولا_ctx);
}

#إذا defined(EXITFREE)
    فراغ
free_findfile(فراغ)
{
    VIM_CLEAR_STRING(ff_expand_بفر);
}
#جيڪڏهن_پورو

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
ڳوليو_directory_اندريون_رستو(
    char_u	*ptr,		// file name
    صحيح		len,		// length of file name
    صحيح		options,
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_منزل_ڳوليو,	// in/out: modified copy of file name
    حرف	**ڳولا_ctx)	// in/out: state of the search
{
    أرجع ڳوليو_file_اندريون_رستو_option(ptr, len, options, TRUE, p_cdpath,
				       FINDFILE_DIR, rel_fname, (char_u *)"",
				       file_منزل_ڳوليو, ڳولا_ctx);
}

    char_u *
ڳوليو_file_اندريون_رستو_option(
    char_u	*ptr,		// file name
    صحيح		len,		// length of file name
    صحيح		options,
    صحيح		پهريون,		// use count'th matching file name
    char_u	*رستو_option,	// p_path or p_cdpath
    صحيح		ڳوليو_what,	// FINDFILE_FILE, _DIR or _BOTH
    char_u	*rel_fname,	// file name we are looking relative to.
    char_u	*suffixes,	// list of suffixes, 'suffixesadd' option
    char_u	**file_منزل_ڳوليو,	// in/out: modified copy of file name
    حرف	**ڳولا_ctx_arg) // in/out: state of the search
{
    ff_ڳولا_ctx_T	**ڳولا_ctx = (ff_ڳولا_ctx_T **)ڳولا_ctx_arg;
    ساكن char_u	*dir;
    ساكن صحيح		did_findfile_init = FALSE;
    char_u		*file_name = عدم;
    صحيح			rel_منزل_curdir;
#جيڪڏهن_وصف_ٿيل AMIGA
    بنية Process	*proc = (بنية Process *)ڳوليو_Task(0L);
    APTR		save_winptr = proc->pr_دري_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_دري_Ptr = (APTR)-1L;
#جيڪڏهن_پورو
    ساكن size_t	file_منزل_findlen = 0;

    إذا (پهريون == TRUE)
    {
	char_u	save_char;

	إذا (len == 0)
	    أرجع عدم;

	// copy file name into NameBuff, expanding environment variables
	save_char = ptr[len];
	ptr[len] = NUL;
	file_منزل_findlen = expand_env_esc(ptr, NameBuff, MAXPATHL, عدم, TRUE, عدم);
	ptr[len] = save_char;

	vim_free(*file_منزل_ڳوليو);
	*file_منزل_ڳوليو = vim_strnsave(NameBuff, file_منزل_findlen);
	إذا (*file_منزل_ڳوليو == عدم)	// out of memory
	{
	    file_name = عدم;
	    file_منزل_findlen = 0;
	    اذهب theend;
	}
	إذا (options & FNAME_UNESC)
	{
	    // Change all "\ " to " ".
	    لكل (ptr = *file_منزل_ڳوليو; *ptr != NUL; ++ptr)
		إذا (ptr[0] == '\\' && ptr[1] == ' ')
		{
		    mch_memmove(ptr, ptr + 1,
			(size_t)((*file_منزل_ڳوليو + file_منزل_findlen) - (ptr + 1)) + 1);
		    --file_منزل_findlen;
		}
	}
    }

    rel_منزل_curdir = ((*file_منزل_ڳوليو)[0] == '.'
		    && ((*file_منزل_ڳوليو)[1] == NUL
			|| vim_ispathsep((*file_منزل_ڳوليو)[1])
			|| ((*file_منزل_ڳوليو)[1] == '.'
			    && ((*file_منزل_ڳوليو)[2] == NUL
				|| vim_ispathsep((*file_منزل_ڳوليو)[2])))));
    إذا (vim_isAbsName(*file_منزل_ڳوليو)
	    // "..", "../path", "." and "./path": don't use the path_option
	    || rel_منزل_curdir
#إذا defined(MSWIN)
	    // handle "\tmp" as absolute path
	    || vim_ispathsep((*file_منزل_ڳوليو)[0])
	    // handle "c:name" as absolute path
	    || ((*file_منزل_ڳوليو)[0] != NUL && (*file_منزل_ڳوليو)[1] == ':')
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل AMIGA
	    // handle ":tmp" as absolute path
	    || (*file_منزل_ڳوليو)[0] == ':'
#جيڪڏهن_پورو
       )
    {
	/*
	 * Absolute path, no need to use "path_option".
	 * If this is not a first call, return NULL.  We already returned a
	 * filename on the first call.
	 */
	إذا (پهريون == TRUE)
	{
	    صحيح		l;
	    صحيح		لاڳيتو;
	    size_t	rel_fnamelen = 0;
	    char_u	*suffix;

	    إذا (رستو_with_url(*file_منزل_ڳوليو))
	    {
		file_name = vim_strnsave(*file_منزل_ڳوليو, file_منزل_findlen);
		اذهب theend;
	    }

	    إذا (rel_fname != عدم)
		rel_fnamelen = STRLEN(rel_fname);

	    // When FNAME_REL flag given first use the directory of the file.
	    // Otherwise or when this fails use the current directory.
	    لكل (لاڳيتو = 1; لاڳيتو <= 2; ++لاڳيتو)
	    {
		l = (صحيح)file_منزل_findlen;
		إذا (لاڳيتو == 1
			&& rel_منزل_curdir
			&& (options & FNAME_REL)
			&& rel_fname != عدم
			&& rel_fnamelen + l < MAXPATHL)
		{
		    l = vim_snprintf(
			(حرف *)NameBuff,
			MAXPATHL,
			"%.*s%s",
			(صحيح)(gettail(rel_fname) - rel_fname),
			rel_fname,
			*file_منزل_ڳوليو);
		}
		وإلا
		{
		    STRCPY(NameBuff, *file_منزل_ڳوليو);
		    لاڳيتو = 2;
		}

		// When the file doesn't exist, try adding parts of
		// 'suffixesadd'.
		suffix = suffixes;
		لكل (;;)
		{
		    إذا (mch_getperm(NameBuff) >= 0
			     && (ڳوليو_what == FINDFILE_BOTH
				 || ((ڳوليو_what == FINDFILE_DIR)
						    == mch_isdir(NameBuff))))
		    {
			file_name = vim_strnsave(NameBuff,
						  simplify_filename(NameBuff));
			اذهب theend;
		    }
		    إذا (*suffix == NUL)
			اكسر;
		    copy_option_part(&suffix, NameBuff + l, MAXPATHL - l, ",");
		}
	    }
	}
    }
    وإلا
    {
	/*
	 * Loop over all paths in the 'path' or 'cdpath' option.
	 * When "first" is set, first setup to the start of the option.
	 * Otherwise continue to find the next match.
	 */
	إذا (پهريون == TRUE)
	{
	    // vim_findfile_free_visited can handle a possible NULL pointer
	    vim_findfile_free_گهميل(*ڳولا_ctx);
	    dir = رستو_option;
	    did_findfile_init = FALSE;
	}

	لكل (;;)
	{
	    إذا (did_findfile_init)
	    {
		file_name = vim_findfile(*ڳولا_ctx);
		إذا (file_name != عدم)
		    اكسر;

		did_findfile_init = FALSE;
	    }
	    وإلا
	    {
		char_u	*buf;
		char_u  *r_ptr;

		إذا (dir == عدم || *dir == NUL)
		{
		    // We searched all paths of the option, now we can
		    // free the search context.
		    vim_findfile_cleanup(*ڳولا_ctx);
		    *ڳولا_ctx = عدم;
		    اكسر;
		}

		إذا ((buf = alloc(MAXPATHL)) == عدم)
		    اكسر;

		// copy next path
		buf[0] = NUL;
		copy_option_part(&dir, buf, MAXPATHL, " ,");

		// get the stopdir string
		r_ptr = vim_findfile_stopdir(buf);
		*ڳولا_ctx = vim_findfile_init(buf, *file_منزل_ڳوليو, file_منزل_findlen,
					    r_ptr, 100, FALSE, ڳوليو_what,
					   *ڳولا_ctx, FALSE, rel_fname);
		إذا (*ڳولا_ctx != عدم)
		    did_findfile_init = TRUE;
		vim_free(buf);
	    }
	}
    }
    إذا (file_name == عدم && (options & FNAME_MESS))
    {
	إذا (پهريون == TRUE)
	{
	    إذا (ڳوليو_what == FINDFILE_DIR)
		semsg(_(e_cant_ڳوليو_directory_str_اندريون_cdpath), *file_منزل_ڳوليو);
	    وإلا
		semsg(_(e_cant_ڳوليو_file_str_اندريون_رستو), *file_منزل_ڳوليو);
	}
	وإلا
	{
	    إذا (ڳوليو_what == FINDFILE_DIR)
		semsg(_(e_no_وڌيڪ_directory_str_مليو_اندريون_cdpath),
								*file_منزل_ڳوليو);
	    وإلا
		semsg(_(e_no_وڌيڪ_file_str_مليو_اندريون_رستو), *file_منزل_ڳوليو);
	}
    }

theend:
#جيڪڏهن_وصف_ٿيل AMIGA
    proc->pr_دري_Ptr = save_winptr;
#جيڪڏهن_پورو
    أرجع file_name;
}

/*
 * Get the file name at the cursor.
 * If Visual mode is active, use the selected text if it's in one line.
 * Returns the name in allocated memory, NULL for failure.
 */
    char_u *
grab_file_name(ڊگهو ڳڻپ, linenr_T *file_lnum)
{
    صحيح options = FNAME_MESS|FNAME_EXP|FNAME_REL|FNAME_UNESC;

    إذا (VIsual_سرگرم)
    {
	صحيح	len;
	char_u	*ptr;

	إذا (get_visual_متن(عدم, &ptr, &len) == FAIL)
	    أرجع عدم;
	// Only recognize ":123" here
	إذا (file_lnum != عدم && ptr[len] == ':' && محفوظ_isdigit(ptr[len + 1]))
	{
	    char_u *p = ptr + len + 1;

	    *file_lnum = getdigits(&p);
	}
	أرجع ڳوليو_file_name_اندريون_رستو(ptr, len, options,
						     ڳڻپ, curbuf->b_ffname);
    }
    أرجع file_name_at_cursor(options | FNAME_HYP, ڳڻپ, file_lnum);
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
file_name_at_cursor(صحيح options, ڊگهو ڳڻپ, linenr_T *file_lnum)
{
    أرجع file_name_اندريون_سٽ(ml_get_curline(),
		      curwin->w_cursor.کانو, options, ڳڻپ, curbuf->b_ffname,
		      file_lnum);
}

/*
 * Return the name of the file under or after ptr[col].
 * Otherwise like file_name_at_cursor().
 */
    char_u *
file_name_اندريون_سٽ(
    char_u	*سٽ,
    صحيح		کانو,
    صحيح		options,
    ڊگهو	ڳڻپ,
    char_u	*rel_fname,	// file we are searching relative to
    linenr_T	*file_lnum)	// line number after the file name
{
    char_u	*ptr;
    صحيح		len;
    صحيح		اندريون_type = TRUE;
    صحيح		is_url = FALSE;

    /*
     * search forward for what could be the start of a file name
     */
    ptr = سٽ + کانو;
    طالما (*ptr != NUL && !vim_isfilec(*ptr))
	MB_PTR_ADV(ptr);
    إذا (*ptr == NUL)		// nothing found
    {
	إذا (options & FNAME_MESS)
	    emsg(_(e_no_file_name_under_cursor));
	أرجع عدم;
    }

    /*
     * Search backward for first char of the file name.
     * Go one char back to ":" before "//" even when ':' is not in 'isfname'.
     */
    طالما (ptr > سٽ)
    {
	إذا (has_mbyte && (len = (*mb_سر_off)(سٽ, ptr - 1)) > 0)
	    ptr -= len + 1;
	وإلا إذا (vim_isfilec(ptr[-1])
		|| ((options & FNAME_HYP) && رستو_is_url(ptr - 1)))
	    --ptr;
	وإلا
	    اكسر;
    }

    /*
     * Search forward for the last char of the file name.
     * Also allow "://" when ':' is not in 'isfname'.
     */
    len = 0;
    طالما (vim_isfilec(ptr[len]) || (ptr[len] == '\\' && ptr[len + 1] == ' ')
		 || ((options & FNAME_HYP) && رستو_is_url(ptr + len))
		 || (is_url && vim_strchr((char_u *)":?&=", ptr[len]) != عدم))
    {
	// After type:// we also include :, ?, & and = as valid characters, so
	// that http://google.com:8080?q=this&that=ok works.
	إذا ((ptr[len] >= 'A' && ptr[len] <= 'Z')
				       || (ptr[len] >= 'a' && ptr[len] <= 'z'))
	{
	    إذا (اندريون_type && رستو_is_url(ptr + len + 1))
		is_url = TRUE;
	}
	وإلا
	    اندريون_type = FALSE;

	إذا (ptr[len] == '\\')
	    // Skip over the "\" in "\ ".
	    ++len;
	إذا (has_mbyte)
	    len += (*mb_ptr2len)(ptr + len);
	وإلا
	    ++len;
    }

    /*
     * If there is trailing punctuation, remove it.
     * But don't remove "..", could be a directory name.
     */
    إذا (len > 2 && vim_strchr((char_u *)".,:;!", ptr[len - 1]) != عدم
						       && ptr[len - 2] != '.')
	--len;

    إذا (file_lnum != عدم)
    {
	char_u	*p;
	حرف	*ميلاپ_متن = " line ";		// english
	size_t	ميلاپ_textlen = 6;

	// Get the number after the file name and a separator character.
	// Also accept " line 999" with and without the same translation as
	// used in last_set_msg().
	p = ptr + len;
	إذا (STRNCMP(p, ميلاپ_متن, ميلاپ_textlen) == 0)
	    p += ميلاپ_textlen;
	وإلا
	{
	    // no match with english, try localized
	    ميلاپ_متن = _(سٽ_نياپو);
	    ميلاپ_textlen = STRLEN(ميلاپ_متن);

	    إذا (STRNCMP(p, ميلاپ_متن, ميلاپ_textlen) == 0)
		p += ميلاپ_textlen;
	    وإلا
		p = skipwhite(p);
	}
	إذا (*p != NUL)
	{
	    إذا (!محفوظ_isdigit(*p))
		++p;		    // skip the separator
	    p = skipwhite(p);
	    إذا (محفوظ_isdigit(*p))
		*file_lnum = (صحيح)getdigits(&p);
	}
    }

    أرجع ڳوليو_file_name_اندريون_رستو(ptr, len, options, ڳڻپ, rel_fname);
}

#إذا defined(FEAT_ڳوليو_ID) && defined(FEAT_EVAL)
    ساكن char_u *
eval_includeexpr(char_u *ptr, صحيح len)
{
    char_u	*res;
    sctx_T	save_sctx = current_sctx;

    سيٽ_vim_var_string(VV_FNAME, ptr, len);
    current_sctx = curbuf->b_p_script_ctx[BV_INEX];

    res = eval_منزل_string_محفوظ(curbuf->b_p_inex,
	    was_سيٽ_insecurely(curwin, (char_u *)"includeexpr", OPT_LOCAL),
								   TRUE, TRUE);

    سيٽ_vim_var_string(VV_FNAME, عدم, 0);
    current_sctx = save_sctx;
    أرجع res;
}
#جيڪڏهن_پورو

/*
 * Return the name of the file ptr[len] in 'path'.
 * Otherwise like file_name_at_cursor().
 */
    char_u *
ڳوليو_file_name_اندريون_رستو(
    char_u	*ptr,
    صحيح		len,
    صحيح		options,
    ڊگهو	ڳڻپ,
    char_u	*rel_fname)	// file we are searching relative to
{
    char_u	*file_name;
    صحيح		c;
#إذا defined(FEAT_ڳوليو_ID) && defined(FEAT_EVAL)
    char_u	*tofree = عدم;
#جيڪڏهن_پورو

    إذا (len == 0)
	أرجع عدم;

#إذا defined(FEAT_ڳوليو_ID) && defined(FEAT_EVAL)
    إذا ((options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
    {
	tofree = eval_includeexpr(ptr, len);
	إذا (tofree != عدم)
	{
	    ptr = tofree;
	    len = (صحيح)STRLEN(ptr);
	}
    }
#جيڪڏهن_پورو

    إذا (options & FNAME_EXP)
    {
	char_u	*file_منزل_ڳوليو = عدم;
	حرف	*ڳولا_ctx = عدم;

	file_name = ڳوليو_file_اندريون_رستو(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_منزل_ڳوليو, &ڳولا_ctx);

#إذا defined(FEAT_ڳوليو_ID) && defined(FEAT_EVAL)
	/*
	 * If the file could not be found in a normal way, try applying
	 * 'includeexpr' (unless done already).
	 */
	إذا (file_name == عدم
		&& !(options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
	{
	    tofree = eval_includeexpr(ptr, len);
	    إذا (tofree != عدم)
	    {
		ptr = tofree;
		len = (صحيح)STRLEN(ptr);
		file_name = ڳوليو_file_اندريون_رستو(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_منزل_ڳوليو, &ڳولا_ctx);
	    }
	}
#جيڪڏهن_پورو
	إذا (file_name == عدم && (options & FNAME_MESS))
	{
	    c = ptr[len];
	    ptr[len] = NUL;
	    semsg(_(e_cant_ڳوليو_file_str_اندريون_رستو_2), ptr);
	    ptr[len] = c;
	}

	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	طالما (file_name != عدم && --ڳڻپ > 0)
	{
	    vim_free(file_name);
	    file_name = ڳوليو_file_اندريون_رستو(ptr, len, options, FALSE, rel_fname,
						   &file_منزل_ڳوليو, &ڳولا_ctx);
	}

	vim_free(file_منزل_ڳوليو);
	vim_findfile_cleanup(ڳولا_ctx);
    }
    وإلا
	file_name = vim_strnsave(ptr, len);

#إذا defined(FEAT_ڳوليو_ID) && defined(FEAT_EVAL)
    vim_free(tofree);
#جيڪڏهن_پورو

    أرجع file_name;
}

/*
 * Return the end of the directory name, on the first path
 * separator:
 * "/path/file", "/path/dir/", "/path//dir", "/file"
 *	 ^	       ^	     ^	      ^
 */
    ساكن char_u *
gettail_dir(char_u *fname)
{
    char_u	*dir_پڇاڙي = fname;
    char_u	*ايندڙ_dir_پڇاڙي = fname;
    صحيح		look_for_sep = TRUE;
    char_u	*p;

    لكل (p = fname; *p != NUL; )
    {
	إذا (vim_ispathsep(*p))
	{
	    إذا (look_for_sep)
	    {
		ايندڙ_dir_پڇاڙي = p;
		look_for_sep = FALSE;
	    }
	}
	وإلا
	{
	    إذا (!look_for_sep)
		dir_پڇاڙي = ايندڙ_dir_پڇاڙي;
	    look_for_sep = TRUE;
	}
	MB_PTR_ADV(p);
    }
    أرجع dir_پڇاڙي;
}

/*
 * return TRUE if 'c' is a path list separator.
 */
    صحيح
vim_ispathlistsep(صحيح c)
{
#جيڪڏهن_وصف_ٿيل UNIX
    أرجع (c == ':');
#وإلا
    أرجع (c == ';');	// might not be right for every system...
#جيڪڏهن_پورو
}

/*
 * Moves "*psep" back to the previous path separator in "path".
 * Returns FAIL is "*psep" ends up at the beginning of "path".
 */
    ساكن صحيح
ڳوليو_اڳيون_pathsep(char_u *رستو, char_u **psep)
{
    // skip the current separator
    إذا (*psep > رستو && vim_ispathsep(**psep))
	--*psep;

    // find the previous separator
    طالما (*psep > رستو)
    {
	إذا (vim_ispathsep(**psep))
	    أرجع OK;
	MB_PTR_پٺ(رستو, *psep);
    }

    أرجع FAIL;
}

/*
 * Returns TRUE if "maybe_unique" is unique wrt other_paths in "gap".
 * "maybe_unique" is the end portion of "((char_u **)gap->ga_data)[i]".
 */
    ساكن صحيح
is_unique(char_u *maybe_unique, garray_T *gap, صحيح i)
{
    صحيح	    j;
    صحيح	    candidate_len = (صحيح)STRLEN(maybe_unique);
    صحيح	    other_رستو_len;
    char_u  **other_paths = (char_u **)gap->ga_معلومات;
    char_u  *rival;

    لكل (j = 0; j < gap->ga_len; j++)
    {
	إذا (j == i)
	    استمر;  // don't compare it with itself

	other_رستو_len = (صحيح)STRLEN(other_paths[j]);
	إذا (other_رستو_len < candidate_len)
	    استمر;  // it's different when it's shorter

	rival = other_paths[j] + other_رستو_len - candidate_len;
	إذا (fnamecmp(maybe_unique, rival) == 0
		&& (rival == other_paths[j] || vim_ispathsep(*(rival - 1))))
	    أرجع FALSE;  //ميلاپ
    }

    أرجع TRUE;  // no match found
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
    ساكن فراغ
expand_رستو_option(
	char_u		*curdir,
	char_u		*رستو_option,	// p_path or p_cdpath
	garray_T	*gap)
{
    char_u	*buf;
    size_t	buflen;
    char_u	*p;
    size_t	curdirlen = 0;

    إذا ((buf = alloc(MAXPATHL)) == عدم)
	أرجع;

    طالما (*رستو_option != NUL)
    {
	buflen = copy_option_part(&رستو_option, buf, MAXPATHL, " ,");

	// do not expand backticks, could have been set via a modeline
	إذا (vim_strchr(buf, '`') != عدم)
	    استمر;

	إذا (buf[0] == '.' && (buf[1] == NUL || vim_ispathsep(buf[1])))
	{
	    size_t  plen;

	    // Relative to current buffer:
	    // "/path/file" + "." -> "/path/"
	    // "/path/file"  + "./subdir" -> "/path/subdir"
	    إذا (curbuf->b_ffname == عدم)
		استمر;
	    p = gettail(curbuf->b_ffname);
	    plen = (size_t)(p - curbuf->b_ffname);
	    إذا (plen + buflen >= MAXPATHL)
		استمر;
	    إذا (buf[1] == NUL)
		buf[plen] = NUL;
	    وإلا
		mch_memmove(buf + plen, buf + 2, (buflen - 2) + 1); // +1 for NUL
	    mch_memmove(buf, curbuf->b_ffname, plen);
	    buflen = simplify_filename(buf);
	}
	وإلا إذا (buf[0] == NUL)
	{
	    // relative to current directory
	    STRCPY(buf, curdir);
	    إذا (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    buflen = curdirlen;
	}
	وإلا إذا (رستو_with_url(buf))
	    // URL can't be used here
	    استمر;
	وإلا إذا (!mch_is_ڀريل_Name(buf))
	{
	    // Expand relative path to their full path equivalent
	    إذا (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    إذا (curdirlen + buflen + 3 > MAXPATHL)
		استمر;

	    mch_memmove(buf + curdirlen + 1, buf, buflen + 1);	    // +1 for NUL
	    STRCPY(buf, curdir);
	    buf[curdirlen] = PATHSEP;
	    buflen = simplify_filename(buf);
	}

	إذا (ga_grow(gap, 1) == FAIL)
	    اكسر;

#إذا defined(MSWIN)
	// Avoid the path ending in a backslash, it fails when a comma is
	// appended.
	إذا (buf[buflen - 1] == '\\')
	    buf[buflen - 1] = '/';
#جيڪڏهن_پورو

	p = vim_strnsave(buf, buflen);
	إذا (p == عدم)
	    اكسر;
	((char_u **)gap->ga_معلومات)[gap->ga_len++] = p;
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
    ساكن char_u *
get_رستو_cutoff(char_u *fname, garray_T *gap)
{
    صحيح	    i;
    صحيح	    maxlen = 0;
    char_u  **رستو_part = (char_u **)gap->ga_معلومات;
    char_u  *cutoff = عدم;

    لكل (i = 0; i < gap->ga_len; i++)
    {
	صحيح j = 0;

	طالما ((fname[j] == رستو_part[i][j]
#إذا defined(MSWIN)
		|| (vim_ispathsep(fname[j]) && vim_ispathsep(رستو_part[i][j]))
#جيڪڏهن_پورو
			     ) && fname[j] != NUL && رستو_part[i][j] != NUL)
	    j++;
	إذا (j > maxlen)
	{
	    maxlen = j;
	    cutoff = &fname[j];
	}
    }

    // skip to the file or directory name
    إذا (cutoff != عدم)
	طالما (vim_ispathsep(*cutoff))
	    MB_PTR_ADV(cutoff);

    أرجع cutoff;
}

/*
 * Sorts, removes duplicates and modifies all the fullpath names in "gap" so
 * that they are unique with respect to each other while conserving the part
 * that matches the pattern. Beware, this is at least O(n^2) wrt "gap->ga_len".
 */
    فراغ
uniquefy_paths(
	garray_T *gap,
	char_u *نمونو,
	char_u *رستو_option)	// p_path or p_cdpath
{
    صحيح		i;
    صحيح		len;
    char_u	**fnames = (char_u **)gap->ga_معلومات;
    صحيح		ترتيب_ڏيو_again = FALSE;
    char_u	*pat;
    char_u      *file_نمونو;
    char_u	*curdir;
    regmatch_T	regmatch;
    garray_T	رستو_ga;
    char_u	**اندريون_curdir = عدم;
    char_u	*short_name;

    remove_duplicates(gap);
    ga_init2(&رستو_ga, حجم(char_u *), 1);

    /*
     * We need to prepend a '*' at the beginning of file_pattern so that the
     * regex matches anywhere in the path. FIXME: is this valid for all
     * possible patterns?
     */
    len = (صحيح)STRLEN(نمونو);
    file_نمونو = alloc(len + 2);
    إذا (file_نمونو == عدم)
	أرجع;
    file_نمونو[0] = '*';
    STRCPY(file_نمونو + 1, نمونو);
    pat = file_pat_منزل_reg_pat(file_نمونو, عدم, عدم, FALSE);
    vim_free(file_نمونو);
    إذا (pat == عدم)
	أرجع;

    regmatch.rm_ic = TRUE;		// always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    vim_free(pat);
    إذا (regmatch.regprog == عدم)
	أرجع;

    إذا ((curdir = alloc(MAXPATHL)) == عدم)
	اذهب theend;
    mch_dirname(curdir, MAXPATHL);
    expand_رستو_option(curdir, رستو_option, &رستو_ga);

    اندريون_curdir = ALLOC_CLEAR_MULT(char_u *, gap->ga_len);
    إذا (اندريون_curdir == عدم)
	اذهب theend;

    لكل (i = 0; i < gap->ga_len && !got_int; i++)
    {
	char_u	    *رستو = fnames[i];
	صحيح	    is_اندريون_curdir;
	char_u	    *dir_پڇاڙي = gettail_dir(رستو);
	char_u	    *رستو_cutoff;

	len = (صحيح)STRLEN(رستو);
	is_اندريون_curdir = fnamencmp(curdir, رستو, dir_پڇاڙي - رستو) == 0
					     && curdir[dir_پڇاڙي - رستو] == NUL;
	إذا (is_اندريون_curdir)
	    اندريون_curdir[i] = vim_strnsave(رستو, len);

	// Shorten the filename while maintaining its uniqueness
	رستو_cutoff = get_رستو_cutoff(رستو, &رستو_ga);

	// Don't assume all files can be reached without path when search
	// pattern starts with star star slash, so only remove path_cutoff
	// when possible.
	إذا (نمونو[0] == '*' && نمونو[1] == '*'
		&& vim_ispathsep_nocolon(نمونو[2])
		&& رستو_cutoff != عدم
		&& vim_regexec(&regmatch, رستو_cutoff, (colnr_T)0)
		&& is_unique(رستو_cutoff, gap, i))
	{
	    ترتيب_ڏيو_again = TRUE;
	    mch_memmove(رستو, رستو_cutoff, STRLEN(رستو_cutoff) + 1);
	}
	وإلا
	{
	    // Here all files can be reached without path, so get shortest
	    // unique path.  We start at the end of the path.
	    char_u  *pathsep_p = رستو + len - 1;

	    طالما (ڳوليو_اڳيون_pathsep(رستو, &pathsep_p))
	    {
		إذا (vim_regexec(&regmatch, pathsep_p + 1, (colnr_T)0)
			&& is_unique(pathsep_p + 1, gap, i)
			&& رستو_cutoff != عدم && pathsep_p + 1 >= رستو_cutoff)
		{
		    ترتيب_ڏيو_again = TRUE;
		    mch_memmove(رستو, pathsep_p + 1,
			    (size_t)((رستو + len) - (pathsep_p + 1)) + 1);  // +1 for NUL
		    اكسر;
		}
	    }
	}

	إذا (mch_is_ڀريل_Name(رستو))
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
	    short_name = shorten_fname(رستو, curdir);
	    إذا (short_name != عدم && short_name > رستو + 1
#إذا defined(MSWIN)
		    // On windows,
		    //	    shorten_fname("c:\a\a.txt", "c:\a\b")
		    // returns "\a\a.txt", which is not really the short
		    // name, hence:
		    && !vim_ispathsep(*short_name)
#جيڪڏهن_پورو
		)
	    {
		vim_snprintf((حرف *)رستو, MAXPATHL, ".%s%s", PATHSEPSTR, short_name);
	    }
	}
	ui_breakcheck();
    }

    // Shorten filenames in /in/current/directory/{filename}
    لكل (i = 0; i < gap->ga_len && !got_int; i++)
    {
	size_t	rel_pathsize;
	char_u	*rel_رستو;
	char_u	*رستو = اندريون_curdir[i];

	إذا (رستو == عدم)
	    استمر;

	// If the {filename} is not unique, change it to ./{filename}.
	// Else reduce it to {filename}
	short_name = shorten_fname(رستو, curdir);
	إذا (short_name == عدم)
	    short_name = رستو;
	إذا (is_unique(short_name, gap, i))
	{
	    STRCPY(fnames[i], short_name);
	    استمر;
	}

	rel_pathsize = 1 + حجم(PATHSEP) + STRLEN(short_name) + 1;
	rel_رستو = alloc(rel_pathsize);
	إذا (rel_رستو == عدم)
	    اذهب theend;

	vim_snprintf((حرف *)rel_رستو, rel_pathsize, ".%s%s", PATHSEPSTR, short_name);

	vim_free(fnames[i]);
	fnames[i] = rel_رستو;
	ترتيب_ڏيو_again = TRUE;
	ui_breakcheck();
    }

theend:
    vim_free(curdir);
    إذا (اندريون_curdir != عدم)
    {
	لكل (i = 0; i < gap->ga_len; i++)
	    vim_free(اندريون_curdir[i]);
	vim_free(اندريون_curdir);
    }
    ga_clear_لڙيون(&رستو_ga);
    vim_regfree(regmatch.regprog);

    إذا (ترتيب_ڏيو_again)
	remove_duplicates(gap);
}

/*
 * Calls globpath() with 'path' values for the given pattern and stores the
 * result in "gap".
 * Returns the total number of matches.
 */
    صحيح
expand_اندريون_رستو(
    garray_T	*gap,
    char_u	*نمونو,
    صحيح		flags)		// EW_* flags
{
    char_u	*curdir;
    garray_T	رستو_ga;
    char_u	*paths = عدم;
    صحيح		glob_flags = 0;
    char_u	*رستو_option = *curbuf->b_p_رستو == NUL ? p_رستو : curbuf->b_p_رستو;

    إذا ((curdir = alloc(MAXPATHL)) == عدم)
	أرجع 0;
    mch_dirname(curdir, MAXPATHL);

    ga_init2(&رستو_ga, حجم(char_u *), 1);
    إذا (flags & EW_CDPATH)
	expand_رستو_option(curdir, p_cdpath, &رستو_ga);
    وإلا
	expand_رستو_option(curdir, رستو_option, &رستو_ga);
    vim_free(curdir);
    إذا (رستو_ga.ga_len == 0)
	أرجع 0;

    paths = ga_concat_لڙيون(&رستو_ga, ",");
    ga_clear_لڙيون(&رستو_ga);
    إذا (paths == عدم)
	أرجع 0;

    إذا (flags & EW_ICASE)
	glob_flags |= WILD_ICASE;
    إذا (flags & EW_ADDSLASH)
	glob_flags |= WILD_جوڙيو_SLASH;
    globpath(paths, نمونو, gap, glob_flags, !!(flags & EW_CDPATH));
    vim_free(paths);

    أرجع gap->ga_len;
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
#جيڪڏهن_وصف_نه_ٿيل AMIGA	    // Amiga doesn't have "..", it uses "/"
    صحيح		components = 0;
    char_u	*p, *پڇ, *شروعات;
    char_u	*p_پڇاڙي;			    // point to NUL at end of string "p"
    صحيح		stripping_disabled = FALSE;
    صحيح		relative = TRUE;

    p = filename;
# ifdef BACKSLASH_اندريون_FILENAME
    إذا (p[0] != NUL && p[1] == ':')	    // skip "x:"
	p += 2;
# endif

    إذا (vim_ispathsep(*p))
    {
	relative = FALSE;
	افعل
	    ++p;
	طالما (vim_ispathsep(*p));
    }
    شروعات = p;	    // remember start after "c:/" or "/" or "///"
    p_پڇاڙي = p + STRLEN(p);
# ifdef UNIX
    // Posix says that "//path" is unchanged but "///path" is "/path".
    إذا (شروعات > filename + 2)
    {
	mch_memmove(filename + 1, p, (size_t)(p_پڇاڙي - p) + 1);	    // +1 for NUL
	p_پڇاڙي -= (size_t)(p - (filename + 1));
	شروعات = p = filename + 1;
    }
# endif

    افعل
    {
	// At this point "p" is pointing to the char following a single "/"
	// or "p" is at the "start" of the (absolute or relative) path name.
# ifdef VMS
	// VMS allows device:[path] - don't strip the [ in directory
	إذا ((*p == '[' || *p == '<') && p > filename && p[-1] == ':')
	{
	    // :[ or :< composition: vms directory component
	    ++components;
	    p = getnextcomp(p + 1);
	}
	// allow remote calls as host"user passwd"::device:[path]
	وإلا إذا (p[0] == ':' && p[1] == ':' && p > filename && p[-1] == '"' )
	{
	    // ":: composition: vms host/passwd component
	    ++components;
	    p = getnextcomp(p + 2);
	}
	وإلا
# endif
	إذا (vim_ispathsep(*p))
	{
	    mch_memmove(p, p + 1, (size_t)(p_پڇاڙي - (p + 1)) + 1); // remove duplicate "/"
	    --p_پڇاڙي;
	}
	وإلا إذا (p[0] == '.' && (vim_ispathsep(p[1]) || p[1] == NUL))
	{
	    إذا (p == شروعات && relative)
		p += 1 + (p[1] != NUL);	// keep single "." or leading "./"
	    وإلا
	    {
		// Strip "./" or ".///".  If we are at the end of the file name
		// and there is no trailing path separator, either strip "/." if
		// we are after "start", or strip "." if we are at the beginning
		// of an absolute path name .
		پڇ = p + 1;
		إذا (p[1] != NUL)
		    طالما (vim_ispathsep(*پڇ))
			MB_PTR_ADV(پڇ);
		وإلا إذا (p > شروعات)
		    --p;		// strip preceding path separator

		mch_memmove(p, پڇ, (size_t)(p_پڇاڙي - پڇ) + 1);
		p_پڇاڙي -= (size_t)(پڇ - p);
	    }
	}
	وإلا إذا (p[0] == '.' && p[1] == '.' &&
	    (vim_ispathsep(p[2]) || p[2] == NUL))
	{
	    // Skip to after ".." or "../" or "..///".
	    پڇ = p + 2;
	    طالما (vim_ispathsep(*پڇ))
		MB_PTR_ADV(پڇ);

	    إذا (components > 0)		// strip one preceding component
	    {
		صحيح		do_strip = FALSE;
		char_u		saved_char;
		stat_T		st;

		// Don't strip for an erroneous file name.
		إذا (!stripping_disabled)
		{
		    // If the preceding component does not exist in the file
		    // system, we strip it.  On Unix, we don't accept a symbolic
		    // link that refers to a non-existent file.
		    saved_char = p[-1];
		    p[-1] = NUL;
# ifdef UNIX
		    إذا (mch_lstat((حرف *)filename, &st) < 0)
# وإلا
			إذا (mch_stat((حرف *)filename, &st) < 0)
# endif
			    do_strip = TRUE;
		    p[-1] = saved_char;

		    --p;
		    // Skip back to after previous '/'.
		    طالما (p > شروعات && !after_pathsep(شروعات, p))
			MB_PTR_پٺ(شروعات, p);

		    إذا (!do_strip)
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
			saved_char = *پڇ;
			*پڇ = NUL;
			إذا (mch_stat((حرف *)filename, &st) >= 0)
			    do_strip = TRUE;
			وإلا
			    stripping_disabled = TRUE;
			*پڇ = saved_char;
# ifdef UNIX
			إذا (do_strip)
			{
			    stat_T	new_st;

			    // On Unix, the check for the unstripped file name
			    // above works also for a symbolic link pointing to
			    // a searchable directory.  But then the parent of
			    // the directory pointed to by the link must be the
			    // same as the stripped file name.  (The latter
			    // exists in the file system since it is the
			    // component's parent directory.)
			    إذا (p == شروعات && relative)
				(فراغ)mch_stat(".", &new_st);
			    وإلا
			    {
				saved_char = *p;
				*p = NUL;
				(فراغ)mch_stat((حرف *)filename, &new_st);
				*p = saved_char;
			    }

			    إذا (new_st.st_ino != st.st_ino ||
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

		إذا (!do_strip)
		{
		    // Skip the ".." or "../" and reset the counter for the
		    // components that might be stripped later on.
		    p = پڇ;
		    components = 0;
		}
		وإلا
		{
		    // Strip previous component.  If the result would get empty
		    // and there is no trailing path separator, leave a single
		    // "." instead.  If we are at the end of the file name and
		    // there is no trailing path separator and a preceding
		    // component is left after stripping, strip its trailing
		    // path separator as well.
		    إذا (p == شروعات && relative && پڇ[-1] == '.')
		    {
			*p++ = '.';
			*p = NUL;
		    }
		    وإلا
		    {
			إذا (p > شروعات && پڇ[-1] == '.')
			    --p;

			mch_memmove(p, پڇ, (size_t)(p_پڇاڙي - پڇ) + 1);	// strip previous component
			p_پڇاڙي -= (size_t)(پڇ - p);
		    }

		    --components;
		}
	    }
	    وإلا إذا (p == شروعات && !relative)	// leading "/.." or "/../"
	    {
		mch_memmove(p, پڇ, (size_t)(p_پڇاڙي - پڇ) + 1);		// strip ".." or "../"
		p_پڇاڙي -= (size_t)(پڇ - p);
	    }
	    وإلا
	    {
		إذا (p == شروعات + 2 && p[-2] == '.')	// leading "./../"
		{
		    mch_memmove(p - 2, p, (size_t)(p_پڇاڙي - p) + 1); // strip leading "./"
		    p_پڇاڙي -= 2;
		    پڇ -= 2;
		}
		p = پڇ;		// skip to char after ".." or "../"
	    }
	}
	وإلا
	{
	    ++components;		// simple path component
	    p = getnextcomp(p);
	}
    } طالما (*p != NUL);

    أرجع (size_t)(p_پڇاڙي - filename);
#وإلا
    // Don't touch Amiga filenames
    أرجع STRLEN(filename);
#جيڪڏهن_پورو // !AMIGA
}

#إذا defined(FEAT_EVAL)
/*
 * "simplify()" function
 */
    فراغ
f_simplify(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;

    إذا (اندريون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    p = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_string = vim_strsave(p);
    simplify_filename(rettv->vval.v_string);	// simplify in place
    rettv->v_type = VAR_STRING;
}
#جيڪڏهن_پورو // FEAT_EVAL
