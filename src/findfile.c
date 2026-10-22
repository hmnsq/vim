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

#అన్తర్భావ "vim.h"

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
రకనిర్వచనం నిర్మాణం ff_దొంతర
{
    నిర్మాణం ff_దొంతర	*ffs_prev;

    // the fix part (no wildcards) and the part containing the wildcards
    // of the search path
    string_T		ffs_fix_మార్గం;
    string_T		ffs_wc_మార్గం;

    // files/dirs found in the above directory, matched by the first wildcard
    // of wc_part
    char_u		**ffs_filearray;
    పూర్ణాంకం			ffs_filearray_కొలత;
    పూర్ణాంకం			ffs_filearray_cur;   // needed for partly handled dirs

    // to store status of partly handled directories
    // 0: we work on this directory for the first time
    // 1: this directory was partly searched in an earlier step
    పూర్ణాంకం			ffs_stage;

    // How deep are we in the directory tree?
    // Counts backward from value of level parameter to vim_findfile_init
    పూర్ణాంకం			ffs_level;

    // Did we already expand '**' to an empty string?
    పూర్ణాంకం			ffs_నక్షత్రం_నక్షత్రం_ఖాళీలు;
} ff_దొంతర_T;

/*
 * type for already visited directories or files.
 */
రకనిర్వచనం నిర్మాణం ff_సందర్శించిన
{
    నిర్మాణం ff_సందర్శించిన	*ffv_తదుపరి;

    // Visited directories are different if the wildcard string are
    // different. So we have to save it.
    char_u		*ffv_wc_మార్గం;

    // for unix use inode etc for comparison (needed because of links), else
    // use filename.
#నిర్వచితమైతే UNIX
    పూర్ణాంకం			ffv_dev_valid;	// ffv_dev and ffv_ino were set
    dev_t		ffv_dev;	// device number
    ino_t		ffv_ino;	// inode number
#అయితే_ముగింపు
    // The memory for this struct is allocated according to the length of
    // ffv_fname.
    char_u		ffv_fname[1];	// actually longer
} ff_సందర్శించిన_T;

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
రకనిర్వచనం నిర్మాణం ff_సందర్శించిన_list_hdr
{
    నిర్మాణం ff_సందర్శించిన_list_hdr	*ffvl_తదుపరి;

    // the filename the attached visited list is for
    char_u			*ffvl_filename;

    ff_సందర్శించిన_T		*ffvl_సందర్శించిన_list;

} ff_సందర్శించిన_list_hdr_T;


/*
 * '**' can be expanded to several directory levels.
 * Set the default maximum depth.
 */
#నిర్వచించు FF_MAX_నక్షత్రం_నక్షత్రం_EXPAND ((char_u)30)

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
రకనిర్వచనం నిర్మాణం ff_శోధన_ctx_T
{
    ff_దొంతర_T			*ffsc_దొంతర_ptr;
    ff_సందర్శించిన_list_hdr_T	*ffsc_సందర్శించిన_list;
    ff_సందర్శించిన_list_hdr_T	*ffsc_dir_సందర్శించిన_list;
    ff_సందర్శించిన_list_hdr_T	*ffsc_సందర్శించిన_lists_list;
    ff_సందర్శించిన_list_hdr_T	*ffsc_dir_సందర్శించిన_lists_list;
    string_T			ffsc_file_గమ్యం_శోధన;
    string_T			ffsc_ప్రారంభం_dir;
    string_T			ffsc_fix_మార్గం;
    string_T			ffsc_wc_మార్గం;
    పూర్ణాంకం				ffsc_level;
    string_T			*ffsc_stopdirs_v;
    పూర్ణాంకం				ffsc_కనుగొను_what;
    పూర్ణాంకం				ffsc_tagfile;
} ff_శోధన_ctx_T;

// locally needed functions
స్థిర పూర్ణాంకం ff_check_సందర్శించిన(ff_సందర్శించిన_T **, char_u *, size_t, char_u *, size_t);
స్థిర శూన్య vim_findfile_free_సందర్శించిన(శూన్య *శోధన_ctx_arg);
స్థిర శూన్య vim_findfile_free_సందర్శించిన_list(ff_సందర్శించిన_list_hdr_T **list_headp);
స్థిర శూన్య ff_free_సందర్శించిన_list(ff_సందర్శించిన_T *vl);
స్థిర ff_సందర్శించిన_list_hdr_T* ff_get_సందర్శించిన_list(char_u *, size_t, ff_సందర్శించిన_list_hdr_T **list_headp);

స్థిర శూన్య ff_నెట్టు(ff_శోధన_ctx_T *శోధన_ctx, ff_దొంతర_T *దొంతర_ptr);
స్థిర ff_దొంతర_T *ff_తీయి(ff_శోధన_ctx_T *శోధన_ctx);
స్థిర శూన్య ff_clear(ff_శోధన_ctx_T *శోధన_ctx);
స్థిర శూన్య ff_free_దొంతర_element(ff_దొంతర_T *దొంతర_ptr);
స్థిర ff_దొంతర_T *ff_create_దొంతర_element(char_u *, size_t, char_u *, size_t, పూర్ణాంకం, పూర్ణాంకం);
స్థిర పూర్ణాంకం ff_మార్గం_లోపల_stoplist(char_u *, పూర్ణాంకం, string_T *);

స్థిర string_T ff_expand_బఫరు = {ఖాళీ, 0};	    // used for expanding filenames

#ఒకవేళ 0
/*
 * if someone likes findfirst/findnext, here are the functions
 * NOT TESTED!!
 */

స్థిర శూన్య *ff_fn_శోధన_context = ఖాళీ;

    char_u *
vim_findfirst(char_u *మార్గం, char_u *filename, పూర్ణాంకం level)
{
    ff_fn_శోధన_context =
	vim_findfile_init(మార్గం, filename, ఖాళీ, level, TRUE, FALSE,
		ff_fn_శోధన_context, rel_fname);
    ఒకవేళ (ఖాళీ == ff_fn_శోధన_context)
	తిరిగి ఖాళీ;
    లేకపోతే
	తిరిగి vim_findnext()
}

    char_u *
vim_findnext(శూన్య)
{
    char_u *ret = vim_findfile(ff_fn_శోధన_context);

    ఒకవేళ (ఖాళీ == ret)
    {
	vim_findfile_cleanup(ff_fn_శోధన_context);
	ff_fn_శోధన_context = ఖాళీ;
    }
    తిరిగి ret;
}
#అయితే_ముగింపు

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
    శూన్య *
vim_findfile_init(
    char_u	*మార్గం,
    char_u	*filename,
    size_t	filenamelen,
    char_u	*stopdirs వాడని,
    పూర్ణాంకం		level,
    పూర్ణాంకం		free_సందర్శించిన,
    పూర్ణాంకం		కనుగొను_what,
    శూన్య	*శోధన_ctx_arg,
    పూర్ణాంకం		tagfile,	// expanding names of tags files
    char_u	*rel_fname)	// file name to use for "."
{
    char_u		*wc_part;
    ff_దొంతర_T		*sptr;
    ff_శోధన_ctx_T	*శోధన_ctx;
    పూర్ణాంకం			కూడు_sep;

    // If a search context is given by the caller, reuse it, else allocate a
    // new one.
    ఒకవేళ (శోధన_ctx_arg != ఖాళీ)
	శోధన_ctx = శోధన_ctx_arg;
    లేకపోతే
    {
	శోధన_ctx = ALLOC_CLEAR_ONE(ff_శోధన_ctx_T);
	ఒకవేళ (శోధన_ctx == ఖాళీ)
	    వెళ్ళు error_return;
    }
    శోధన_ctx->ffsc_కనుగొను_what = కనుగొను_what;
    శోధన_ctx->ffsc_tagfile = tagfile;

    // clear the search context, but NOT the visited lists
    ff_clear(శోధన_ctx);

    // clear visited list if wanted
    ఒకవేళ (free_సందర్శించిన == TRUE)
	vim_findfile_free_సందర్శించిన(శోధన_ctx);
    లేకపోతే
    {
	// Reuse old visited lists. Get the visited list for the given
	// filename. If no list for the current filename exists, creates a new
	// one.
	శోధన_ctx->ffsc_సందర్శించిన_list = ff_get_సందర్శించిన_list(filename,
					filenamelen, &శోధన_ctx->ffsc_సందర్శించిన_lists_list);
	ఒకవేళ (శోధన_ctx->ffsc_సందర్శించిన_list == ఖాళీ)
	    వెళ్ళు error_return;
	శోధన_ctx->ffsc_dir_సందర్శించిన_list = ff_get_సందర్శించిన_list(filename,
				    filenamelen, &శోధన_ctx->ffsc_dir_సందర్శించిన_lists_list);
	ఒకవేళ (శోధన_ctx->ffsc_dir_సందర్శించిన_list == ఖాళీ)
	    వెళ్ళు error_return;
    }

    ఒకవేళ (ff_expand_బఫరు.string == ఖాళీ)
    {
	ff_expand_బఫరు.నిడివి = 0;
	ff_expand_బఫరు.string = alloc(MAXPATHL);
	ఒకవేళ (ff_expand_బఫరు.string == ఖాళీ)
	    వెళ్ళు error_return;
    }

    // Store information on starting dir now if path is relative.
    // If path is absolute, we do that later.
    ఒకవేళ (మార్గం[0] == '.'
	    && (vim_ispathsep(మార్గం[1]) || మార్గం[1] == NUL)
	    && (!tagfile || vim_strchr(p_cpo, CPO_DOTTAG) == ఖాళీ)
	    && rel_fname != ఖాళీ)
    {
	పూర్ణాంకం	len = (పూర్ణాంకం)(gettail(rel_fname) - rel_fname);

	ఒకవేళ (!vim_isAbsName(rel_fname) && len + 1 < MAXPATHL)
	{
	    // Make the start dir an absolute path name.
	    vim_strncpy(ff_expand_బఫరు.string, rel_fname, len);
	    ff_expand_బఫరు.నిడివి = len;

	    శోధన_ctx->ffsc_ప్రారంభం_dir.string = నిండిన_Name_save(ff_expand_బఫరు.string, FALSE);
	    ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.string == ఖాళీ)
		వెళ్ళు error_return;
	    శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి = STRLEN(శోధన_ctx->ffsc_ప్రారంభం_dir.string);
	}
	లేకపోతే
	{
	    శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి = len;
	    శోధన_ctx->ffsc_ప్రారంభం_dir.string = vim_strnsave(rel_fname,
		శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి);
	    ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.string == ఖాళీ)
		వెళ్ళు error_return;
	}

	ఒకవేళ (*++మార్గం != NUL)
	    ++మార్గం;
    }
    లేకపోతే ఒకవేళ (*మార్గం == NUL || !vim_isAbsName(మార్గం))
    {
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	// "c:dir" needs "c:" to be expanded, otherwise use current dir
	ఒకవేళ (*మార్గం != NUL && మార్గం[1] == ':')
	{
	    char_u  drive[3];

	    drive[0] = మార్గం[0];
	    drive[1] = ':';
	    drive[2] = NUL;
	    ఒకవేళ (vim_నిండిన_Name(drive, ff_expand_బఫరు.string, MAXPATHL, TRUE) == FAIL)
		వెళ్ళు error_return;
	    మార్గం += 2;
	}
	లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (mch_dirname(ff_expand_బఫరు.string, MAXPATHL) == FAIL)
	    వెళ్ళు error_return;

	ff_expand_బఫరు.నిడివి = STRLEN(ff_expand_బఫరు.string);

	శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి = ff_expand_బఫరు.నిడివి;
	శోధన_ctx->ffsc_ప్రారంభం_dir.string = vim_strnsave(ff_expand_బఫరు.string,
	    శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి);
	ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.string == ఖాళీ)
	    వెళ్ళు error_return;
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
    ఒకవేళ (stopdirs != ఖాళీ)
    {
	char_u	*walker = stopdirs;
	పూర్ణాంకం	dircount;

	అయితే (*walker == ';')
	    walker++;

	dircount = 1;
	శోధన_ctx->ffsc_stopdirs_v = ALLOC_ONE(string_T);

	ఒకవేళ (శోధన_ctx->ffsc_stopdirs_v != ఖాళీ)
	{
	    string_T	*tmp;			// for convenience

	    చేయి
	    {
		char_u	*helper;
		శూన్య	*ptr;
		size_t	len;

		helper = walker;
		ptr = vim_realloc(శోధన_ctx->ffsc_stopdirs_v,
					   (dircount + 1) * పరిమాణం(string_T));
		ఒకవేళ (ptr)
		    శోధన_ctx->ffsc_stopdirs_v = ptr;
		లేకపోతే
		    // ignore, keep what we have and continue
		    ఆపు;
		walker = vim_strchr(walker, ';');
		len = walker ? (size_t)(walker - helper) : STRLEN(helper);
		// "" means ascent till top of directory tree.

		ఒకవేళ (*helper != NUL && !vim_isAbsName(helper)
							 && len + 1 < MAXPATHL)
		{
		    // Make the stop dir an absolute path name.
		    vim_strncpy(ff_expand_బఫరు.string, helper, len);
		    ff_expand_బఫరు.నిడివి = len;

		    tmp = &శోధన_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->string = నిండిన_Name_save(ff_expand_బఫరు.string, FALSE);
		    ఒకవేళ (tmp->string != ఖాళీ)
			tmp->నిడివి = STRLEN(tmp->string);
		}
		లేకపోతే
		{
		    tmp = &శోధన_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->నిడివి = len;
		    tmp->string = vim_strnsave(helper, tmp->నిడివి);
		    ఒకవేళ (tmp->string == ఖాళీ)
			tmp->నిడివి = 0;
		}
		ఒకవేళ (walker)
		    walker++;
		dircount++;

	    } అయితే (walker != ఖాళీ);

	    tmp = &శోధన_ctx->ffsc_stopdirs_v[dircount - 1];
	    tmp->string = ఖాళీ;
	    tmp->నిడివి = 0;
	}
    }

    శోధన_ctx->ffsc_level = level;

    /*
     * split into:
     *  -fix path
     *  -wildcard_stuff (might be NULL)
     */
    wc_part = vim_strchr(మార్గం, '*');
    ఒకవేళ (wc_part != ఖాళీ)
    {
	పూర్ణాంకం	llevel;
	అక్షరం	*errpt;

	// save the fix part of the path
	శోధన_ctx->ffsc_fix_మార్గం.నిడివి = (size_t)(wc_part - మార్గం);
	శోధన_ctx->ffsc_fix_మార్గం.string = vim_strnsave(మార్గం,
	    శోధన_ctx->ffsc_fix_మార్గం.నిడివి);
	ఒకవేళ (శోధన_ctx->ffsc_fix_మార్గం.string == ఖాళీ)
	    వెళ్ళు error_return;

	/*
	 * copy wc_path and add restricts to the '**' wildcard.
	 * The octet after a '**' is used as a (binary) counter.
	 * So '**3' is transposed to '**^C' ('^C' is ASCII value 3)
	 * or '**76' is transposed to '**N'( 'N' is ASCII value 76).
	 * If no restrict is given after '**' the default is used.
	 * Due to this technique the path looks awful if you print it as a
	 * string.
	 */
	ff_expand_బఫరు.నిడివి = 0;
	అయితే (*wc_part != NUL)
	{
	    ఒకవేళ (ff_expand_బఫరు.నిడివి + 5 >= MAXPATHL)
	    {
		emsg(_(e_మార్గం_too_long_for_completion));
		ఆపు;
	    }
	    ఒకవేళ (STRNCMP(wc_part, "**", 2) == 0)
	    {
		ff_expand_బఫరు.string[ff_expand_బఫరు.నిడివి++] = *wc_part++;
		ff_expand_బఫరు.string[ff_expand_బఫరు.నిడివి++] = *wc_part++;

		llevel = strtol((అక్షరం *)wc_part, &errpt, 10);
		ఒకవేళ ((char_u *)errpt != wc_part && llevel > 0 && llevel < 255)
		    ff_expand_బఫరు.string[ff_expand_బఫరు.నిడివి++] = llevel;
		లేకపోతే ఒకవేళ ((char_u *)errpt != wc_part && llevel == 0)
		    // restrict is 0 -> remove already added '**'
		    ff_expand_బఫరు.నిడివి -= 2;
		లేకపోతే
		    ff_expand_బఫరు.string[ff_expand_బఫరు.నిడివి++] = FF_MAX_నక్షత్రం_నక్షత్రం_EXPAND;
		wc_part = (char_u *)errpt;
		ఒకవేళ (*wc_part != NUL && !vim_ispathsep(*wc_part))
		{
		    semsg(_(e_invalid_మార్గం_సంఖ్య_must_be_at_చివర_యొక్క_మార్గం_or_be_followed_by_str), PATHSEPSTR);
		    వెళ్ళు error_return;
		}
	    }
	    లేకపోతే
		ff_expand_బఫరు.string[ff_expand_బఫరు.నిడివి++] = *wc_part++;
	}
	ff_expand_బఫరు.string[ff_expand_బఫరు.నిడివి] = NUL;

	శోధన_ctx->ffsc_wc_మార్గం.నిడివి = ff_expand_బఫరు.నిడివి;
	శోధన_ctx->ffsc_wc_మార్గం.string = vim_strnsave(ff_expand_బఫరు.string,
	    శోధన_ctx->ffsc_wc_మార్గం.నిడివి);
	ఒకవేళ (శోధన_ctx->ffsc_wc_మార్గం.string == ఖాళీ)
	    వెళ్ళు error_return;
    }
    లేకపోతే
    {
	శోధన_ctx->ffsc_fix_మార్గం.నిడివి = STRLEN(మార్గం);
	శోధన_ctx->ffsc_fix_మార్గం.string = vim_strnsave(మార్గం,
	    శోధన_ctx->ffsc_fix_మార్గం.నిడివి);
	ఒకవేళ (శోధన_ctx->ffsc_fix_మార్గం.string == ఖాళీ)
	    వెళ్ళు error_return;
    }

    ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.string == ఖాళీ)
    {
	// store the fix part as startdir.
	// This is needed if the parameter path is fully qualified.
	శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి = శోధన_ctx->ffsc_fix_మార్గం.నిడివి;
	శోధన_ctx->ffsc_ప్రారంభం_dir.string = vim_strnsave(శోధన_ctx->ffsc_fix_మార్గం.string,
	    శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి);
	ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.string == ఖాళీ)
	    వెళ్ళు error_return;
	శోధన_ctx->ffsc_fix_మార్గం.string[0] = NUL;
	శోధన_ctx->ffsc_fix_మార్గం.నిడివి = 0;
    }

    // create an absolute path
    ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి
	    + శోధన_ctx->ffsc_fix_మార్గం.నిడివి + 3 >= MAXPATHL)
    {
	emsg(_(e_మార్గం_too_long_for_completion));
	వెళ్ళు error_return;
    }

    కూడు_sep = !after_pathsep(శోధన_ctx->ffsc_ప్రారంభం_dir.string,
	శోధన_ctx->ffsc_ప్రారంభం_dir.string + శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి);
    ff_expand_బఫరు.నిడివి = vim_snprintf(
	    (అక్షరం *)ff_expand_బఫరు.string,
	    MAXPATHL,
	    "%s%s",
	    శోధన_ctx->ffsc_ప్రారంభం_dir.string,
	    కూడు_sep ? PATHSEPSTR : "");

    {
	size_t	bufsize = ff_expand_బఫరు.నిడివి + శోధన_ctx->ffsc_fix_మార్గం.నిడివి + 1;
	char_u	*buf = alloc(bufsize);

	ఒకవేళ (buf == ఖాళీ)
	    వెళ్ళు error_return;

	vim_snprintf(
		(అక్షరం *)buf,
		bufsize,
		"%s%s",
		ff_expand_బఫరు.string,
		శోధన_ctx->ffsc_fix_మార్గం.string);
	ఒకవేళ (mch_isdir(buf))
	{
	    ఒకవేళ (శోధన_ctx->ffsc_fix_మార్గం.నిడివి > 0)
	    {
		కూడు_sep = !after_pathsep(శోధన_ctx->ffsc_fix_మార్గం.string,
		    శోధన_ctx->ffsc_fix_మార్గం.string + శోధన_ctx->ffsc_fix_మార్గం.నిడివి);
		ff_expand_బఫరు.నిడివి += vim_snprintf(
			(అక్షరం *)ff_expand_బఫరు.string + ff_expand_బఫరు.నిడివి,
			MAXPATHL - ff_expand_బఫరు.నిడివి,
			"%s%s",
			శోధన_ctx->ffsc_fix_మార్గం.string,
			కూడు_sep ? PATHSEPSTR : "");
	    }
	}
	లేకపోతే
	{
	    char_u *p = gettail(శోధన_ctx->ffsc_fix_మార్గం.string);
	    పూర్ణాంకం    len = (పూర్ణాంకం)శోధన_ctx->ffsc_fix_మార్గం.నిడివి;

	    ఒకవేళ (p > శోధన_ctx->ffsc_fix_మార్గం.string)
	    {
		// do not add '..' to the path and start upwards searching
		len = (పూర్ణాంకం)(p - శోధన_ctx->ffsc_fix_మార్గం.string) - 1;
		ఒకవేళ ((len >= 2
			&& STRNCMP(శోధన_ctx->ffsc_fix_మార్గం.string, "..", 2) == 0)
			&& (len == 2 || శోధన_ctx->ffsc_fix_మార్గం.string[2] == PATHSEP))
		{
		    vim_free(buf);
		    వెళ్ళు error_return;
		}

		కూడు_sep = !after_pathsep(శోధన_ctx->ffsc_fix_మార్గం.string,
		    శోధన_ctx->ffsc_fix_మార్గం.string + శోధన_ctx->ffsc_fix_మార్గం.నిడివి);
		ff_expand_బఫరు.నిడివి += vim_snprintf(
			(అక్షరం *)ff_expand_బఫరు.string + ff_expand_బఫరు.నిడివి,
			MAXPATHL - ff_expand_బఫరు.నిడివి,
			"%.*s%s",
			len,
			శోధన_ctx->ffsc_fix_మార్గం.string,
			కూడు_sep ? PATHSEPSTR : "");
	    }

	    ఒకవేళ (శోధన_ctx->ffsc_wc_మార్గం.string != ఖాళీ)
	    {
		size_t	tempsize = (శోధన_ctx->ffsc_fix_మార్గం.నిడివి - len)
				+ శోధన_ctx->ffsc_wc_మార్గం.నిడివి
				+ 1;
		char_u	*తాత్కాలిక = alloc(tempsize);

		ఒకవేళ (తాత్కాలిక == ఖాళీ)
		{
		    vim_free(buf);
		    vim_free(తాత్కాలిక);
		    వెళ్ళు error_return;
		}

		శోధన_ctx->ffsc_wc_మార్గం.నిడివి = vim_snprintf(
			    (అక్షరం *)తాత్కాలిక,
			    tempsize,
			    "%s%s",
			    శోధన_ctx->ffsc_fix_మార్గం.string + len,
			    శోధన_ctx->ffsc_wc_మార్గం.string);
		vim_free(శోధన_ctx->ffsc_wc_మార్గం.string);
		శోధన_ctx->ffsc_wc_మార్గం.string = తాత్కాలిక;
	    }
	}
	vim_free(buf);
    }

    sptr = ff_create_దొంతర_element(ff_expand_బఫరు.string,
			ff_expand_బఫరు.నిడివి,
			శోధన_ctx->ffsc_wc_మార్గం.string,
			శోధన_ctx->ffsc_wc_మార్గం.నిడివి,
			level,
			0);

    ఒకవేళ (sptr == ఖాళీ)
	వెళ్ళు error_return;

    ff_నెట్టు(శోధన_ctx, sptr);

    శోధన_ctx->ffsc_file_గమ్యం_శోధన.నిడివి = filenamelen;
    శోధన_ctx->ffsc_file_గమ్యం_శోధన.string = vim_strnsave(filename,
	శోధన_ctx->ffsc_file_గమ్యం_శోధన.నిడివి);
    ఒకవేళ (శోధన_ctx->ffsc_file_గమ్యం_శోధన.string == ఖాళీ)
	వెళ్ళు error_return;

    తిరిగి శోధన_ctx;

error_return:
    /*
     * We clear the search context now!
     * Even when the caller gave us a (perhaps valid) context we free it here,
     * as we might have already destroyed it.
     */
    vim_findfile_cleanup(శోధన_ctx);
    తిరిగి ఖాళీ;
}

/*
 * Get the stopdir string.  Check that ';' is not escaped.
 */
    char_u *
vim_findfile_stopdir(char_u *buf)
{
    char_u	*r_ptr = buf;
    char_u	*r_ptr_చివర = ఖాళీ;	    // points to NUL at end of string "r_ptr"

    అయితే (*r_ptr != NUL && *r_ptr != ';')
    {
	ఒకవేళ (r_ptr[0] == '\\' && r_ptr[1] == ';')
	{
	    // Overwrite the escape char,
	    // use STRLEN(r_ptr) to move the trailing '\0'.
	    ఒకవేళ (r_ptr_చివర == ఖాళీ)
		r_ptr_చివర = r_ptr + STRLEN(r_ptr);
	    mch_memmove(r_ptr, r_ptr + 1,
		(size_t)(r_ptr_చివర - (r_ptr + 1)) + 1);	// +1 for NUL
	    r_ptr++;
	    --r_ptr_చివర;
	}
	r_ptr++;
    }
    ఒకవేళ (*r_ptr == ';')
    {
	*r_ptr = NUL;
	r_ptr++;
    }
    లేకపోతే ఒకవేళ (*r_ptr == NUL)
	r_ptr = ఖాళీ;
    తిరిగి r_ptr;
}

/*
 * Clean up the given search context. Can handle a NULL pointer.
 */
    శూన్య
vim_findfile_cleanup(శూన్య *ctx)
{
    ఒకవేళ (ctx == ఖాళీ)
	తిరిగి;

    vim_findfile_free_సందర్శించిన(ctx);
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
vim_findfile(శూన్య *శోధన_ctx_arg)
{
    string_T	file_మార్గం;
    string_T	rest_యొక్క_wildcards;
    char_u	*మార్గం_చివర = ఖాళీ;
    ff_దొంతర_T	*stackp;
    ff_శోధన_ctx_T *శోధన_ctx;

    ఒకవేళ (శోధన_ctx_arg == ఖాళీ)
	తిరిగి ఖాళీ;

    శోధన_ctx = (ff_శోధన_ctx_T *)శోధన_ctx_arg;

    /*
     * filepath is used as buffer for various actions and as the storage to
     * return a found filename.
     */
    ఒకవేళ ((file_మార్గం.string = alloc(MAXPATHL)) == ఖాళీ)
	తిరిగి ఖాళీ;

    // store the end of the start dir -- needed for upward search
    ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.string != ఖాళీ)
	మార్గం_చివర = &శోధన_ctx->ffsc_ప్రారంభం_dir.string[
					    శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి];

    // upward search loop
    కొరకు (;;)
    {
	// downward search loop
	కొరకు (;;)
	{
	    // check if user wants to stop the search
	    ui_breakcheck();
	    ఒకవేళ (got_int)
		ఆపు;

	    // get directory to work on from stack
	    stackp = ff_తీయి(శోధన_ctx);
	    ఒకవేళ (stackp == ఖాళీ)
		ఆపు;

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
	    ఒకవేళ (stackp->ffs_filearray == ఖాళీ
		    && ff_check_సందర్శించిన(&శోధన_ctx->ffsc_dir_సందర్శించిన_list
							  ->ffvl_సందర్శించిన_list,
			    stackp->ffs_fix_మార్గం.string,
			    stackp->ffs_fix_మార్గం.నిడివి,
			    stackp->ffs_wc_మార్గం.string,
			    stackp->ffs_wc_మార్గం.నిడివి) == FAIL)
	    {
#నిర్వచితమైతే FF_VERBOSE
		ఒకవేళ (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("Already Searched: %s (%s)",
				   stackp->ffs_fix_మార్గం.string, stackp->ffs_wc_మార్గం.string);
		    // don't overwrite this either
		    సందేశం_puts("\n");
		    verbose_leave_scroll();
		}
#అయితే_ముగింపు
		ff_free_దొంతర_element(stackp);
		కొనసాగు;
	    }
#నిర్వచితమైతే FF_VERBOSE
	    లేకపోతే ఒకవేళ (p_verbose >= 5)
	    {
		verbose_enter_scroll();
		smsg("Searching: %s (%s)",
				   stackp->ffs_fix_మార్గం.string, stackp->ffs_wc_మార్గం.string);
		// don't overwrite this either
		సందేశం_puts("\n");
		verbose_leave_scroll();
	    }
#అయితే_ముగింపు

	    // check depth
	    ఒకవేళ (stackp->ffs_level <= 0)
	    {
		ff_free_దొంతర_element(stackp);
		కొనసాగు;
	    }

	    file_మార్గం.string[0] = NUL;
	    file_మార్గం.నిడివి = 0;

	    /*
	     * If no filearray till now expand wildcards
	     * The function expand_wildcards() can handle an array of paths
	     * and all possible expands are returned in one array. We use this
	     * to handle the expansion of '**' into an empty string.
	     */
	    ఒకవేళ (stackp->ffs_filearray == ఖాళీ)
	    {
		char_u *dirptrs[2];

		// we use filepath to build the path expand_wildcards() should
		// expand.
		dirptrs[0] = file_మార్గం.string;
		dirptrs[1] = ఖాళీ;

		// if we have a start dir copy it in
		ఒకవేళ (!vim_isAbsName(stackp->ffs_fix_మార్గం.string)
						&& శోధన_ctx->ffsc_ప్రారంభం_dir.string)
		{
		    ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి + 1 < MAXPATHL)
		    {
			పూర్ణాంకం కూడు_sep = !after_pathsep(శోధన_ctx->ffsc_ప్రారంభం_dir.string,
			    శోధన_ctx->ffsc_ప్రారంభం_dir.string + శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి);
			file_మార్గం.నిడివి = vim_snprintf(
			    (అక్షరం *)file_మార్గం.string,
			    MAXPATHL,
			    "%s%s",
			    శోధన_ctx->ffsc_ప్రారంభం_dir.string,
			    కూడు_sep ? PATHSEPSTR : "");
		    }
		    లేకపోతే
		    {
			ff_free_దొంతర_element(stackp);
			వెళ్ళు fail;
		    }
		}

		// append the fix part of the search path
		ఒకవేళ (file_మార్గం.నిడివి + stackp->ffs_fix_మార్గం.నిడివి + 1 < MAXPATHL)
		{
		    పూర్ణాంకం కూడు_sep = !after_pathsep(stackp->ffs_fix_మార్గం.string,
			stackp->ffs_fix_మార్గం.string + stackp->ffs_fix_మార్గం.నిడివి);
		    file_మార్గం.నిడివి += vim_snprintf(
			(అక్షరం *)file_మార్గం.string + file_మార్గం.నిడివి,
			MAXPATHL - file_మార్గం.నిడివి,
			"%s%s",
			stackp->ffs_fix_మార్గం.string,
			కూడు_sep ? PATHSEPSTR : "");
		}
		లేకపోతే
		{
		    ff_free_దొంతర_element(stackp);
		    వెళ్ళు fail;
		}

		rest_యొక్క_wildcards.string = stackp->ffs_wc_మార్గం.string;
		rest_యొక్క_wildcards.నిడివి = stackp->ffs_wc_మార్గం.నిడివి;
		ఒకవేళ (*rest_యొక్క_wildcards.string != NUL)
		{
		    ఒకవేళ (STRNCMP(rest_యొక్క_wildcards.string, "**", 2) == 0)
		    {
			char_u	*p;

			// pointer to the restrict byte
			// The restrict byte is not a character!
			p = rest_యొక్క_wildcards.string + 2;

			ఒకవేళ (*p > 0)
			{
			    (*p)--;
			    ఒకవేళ (file_మార్గం.నిడివి + 1 < MAXPATHL)
				file_మార్గం.string[file_మార్గం.నిడివి++] = '*';
			    లేకపోతే
			    {
				ff_free_దొంతర_element(stackp);
				వెళ్ళు fail;
			    }
			}

			ఒకవేళ (*p == 0)
			{
			    // remove '**<numb> from wildcards
			    mch_memmove(rest_యొక్క_wildcards.string,
					rest_యొక్క_wildcards.string + 3,
					(size_t)(rest_యొక్క_wildcards.నిడివి - 3) + 1);    // +1 for NUL
			    rest_యొక్క_wildcards.నిడివి -= 3;
			    stackp->ffs_wc_మార్గం.నిడివి = rest_యొక్క_wildcards.నిడివి;
			}
			లేకపోతే
			{
			    rest_యొక్క_wildcards.string += 3;
			    rest_యొక్క_wildcards.నిడివి -= 3;
			}

			ఒకవేళ (stackp->ffs_నక్షత్రం_నక్షత్రం_ఖాళీలు == 0)
			{
			    // if not done before, expand '**' to empty
			    stackp->ffs_నక్షత్రం_నక్షత్రం_ఖాళీలు = 1;
			    dirptrs[1] = stackp->ffs_fix_మార్గం.string;
			}
		    }

		    /*
		     * Here we copy until the next path separator or the end of
		     * the path. If we stop at a path separator, there is
		     * still something else left. This is handled below by
		     * pushing every directory returned from expand_wildcards()
		     * on the stack again for further search.
		     */
		    అయితే (*rest_యొక్క_wildcards.string
			    && !vim_ispathsep(*rest_యొక్క_wildcards.string))
		    {
			ఒకవేళ (file_మార్గం.నిడివి + 1 < MAXPATHL)
			{
			    file_మార్గం.string[file_మార్గం.నిడివి++] = *rest_యొక్క_wildcards.string++;
			    --rest_యొక్క_wildcards.నిడివి;
			}
			లేకపోతే
			{
			    ff_free_దొంతర_element(stackp);
			    వెళ్ళు fail;
			}
		    }

		    file_మార్గం.string[file_మార్గం.నిడివి] = NUL;
		    ఒకవేళ (vim_ispathsep(*rest_యొక్క_wildcards.string))
		    {
			rest_యొక్క_wildcards.string++;
			rest_యొక్క_wildcards.నిడివి--;
		    }
		}

		/*
		 * Expand wildcards like "*" and "$VAR".
		 * If the path is a URL don't try this.
		 */
		ఒకవేళ (మార్గం_with_url(dirptrs[0]))
		{
		    stackp->ffs_filearray = ALLOC_ONE(char_u *);
		    ఒకవేళ (stackp->ffs_filearray != ఖాళీ
			    && (stackp->ffs_filearray[0]
				= vim_strnsave(dirptrs[0], file_మార్గం.నిడివి)) != ఖాళీ)
			stackp->ffs_filearray_కొలత = 1;
		    లేకపోతే
			stackp->ffs_filearray_కొలత = 0;
		}
		లేకపోతే
		    // Add EW_NOTWILD because the expanded path may contain
		    // wildcard characters that are to be taken literally.
		    // This is a bit of a hack.
		    expand_wildcards((dirptrs[1] == ఖాళీ) ? 1 : 2, dirptrs,
			    &stackp->ffs_filearray_కొలత,
			    &stackp->ffs_filearray,
			    EW_DIR|EW_ADDSLASH|EW_SILENT|EW_NOTWILD);

		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 0;
	    }
	    లేకపోతే
	    {
		rest_యొక్క_wildcards.string = &stackp->ffs_wc_మార్గం.string[
							stackp->ffs_wc_మార్గం.నిడివి];
		rest_యొక్క_wildcards.నిడివి = 0;
	    }

	    ఒకవేళ (stackp->ffs_stage == 0)
	    {
		పూర్ణాంకం i;

		// this is the first time we work on this directory
		ఒకవేళ (*rest_యొక్క_wildcards.string == NUL)
		{
		    size_t  len;
		    char_u  *suf;

		    /*
		     * We don't have further wildcards to expand, so we have to
		     * check for the final file now.
		     */
		    కొరకు (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_కొలత; ++i)
		    {
			ఒకవేళ (!మార్గం_with_url(stackp->ffs_filearray[i])
				      && !mch_isdir(stackp->ffs_filearray[i]))
			    కొనసాగు;   // not a directory

			// prepare the filename to be checked for existence
			//క్రింద
			len = STRLEN(stackp->ffs_filearray[i]);
			ఒకవేళ (len + 1 + శోధన_ctx->ffsc_file_గమ్యం_శోధన.నిడివి
									< MAXPATHL)
			{
			    పూర్ణాంకం	కూడు_sep = !after_pathsep(stackp->ffs_filearray[i],
				    stackp->ffs_filearray[i] + len);
			    file_మార్గం.నిడివి = vim_snprintf(
				    (అక్షరం *)file_మార్గం.string,
				    MAXPATHL,
				    "%s%s%s",
				    stackp->ffs_filearray[i],
				    కూడు_sep ? PATHSEPSTR : "",
				    శోధన_ctx->ffsc_file_గమ్యం_శోధన.string);
			}
			లేకపోతే
			{
			    ff_free_దొంతర_element(stackp);
			    వెళ్ళు fail;
			}

			/*
			 * Try without extra suffix and then with suffixes
			 * from 'suffixesadd'.
			 */
			len = file_మార్గం.నిడివి;
			ఒకవేళ (శోధన_ctx->ffsc_tagfile)
			    suf = (char_u *)"";
			లేకపోతే
			    suf = curbuf->b_p_sua;
			కొరకు (;;)
			{
			    // if file exists and we didn't already find it
			    ఒకవేళ ((మార్గం_with_url(file_మార్గం.string)
				  || (mch_getperm(file_మార్గం.string) >= 0
				      && (శోధన_ctx->ffsc_కనుగొను_what
							      == FINDFILE_BOTH
					  || ((శోధన_ctx->ffsc_కనుగొను_what
							      == FINDFILE_DIR)
						   == mch_isdir(file_మార్గం.string)))))
#నిర్వచితం_కాకపోతే FF_VERBOSE
				    && (ff_check_సందర్శించిన(
					    &శోధన_ctx->ffsc_సందర్శించిన_list
							   ->ffvl_సందర్శించిన_list,
					    file_మార్గం.string,
					    file_మార్గం.నిడివి,
					    (char_u *)"", 0) == OK)
#అయితే_ముగింపు
			       )
			    {
#నిర్వచితమైతే FF_VERBOSE
				ఒకవేళ (ff_check_సందర్శించిన(
					    &శోధన_ctx->ffsc_సందర్శించిన_list
							   ->ffvl_సందర్శించిన_list,
					      file_మార్గం.string,
					      file_మార్గం.నిడివి,
					      (char_u *)"", 0) == FAIL)
				{
				    ఒకవేళ (p_verbose >= 5)
				    {
					verbose_enter_scroll();
					smsg("Already: %s", file_మార్గం.string);
					// don't overwrite this either
					సందేశం_puts("\n");
					verbose_leave_scroll();
				    }
				    కొనసాగు;
				}
#అయితే_ముగింపు

				// push dir to examine rest of subdirs later
				stackp->ffs_filearray_cur = i + 1;
				ff_నెట్టు(శోధన_ctx, stackp);

				ఒకవేళ (!మార్గం_with_url(file_మార్గం.string))
				    file_మార్గం.నిడివి = simplify_filename(file_మార్గం.string);

				ఒకవేళ (mch_dirname(ff_expand_బఫరు.string, MAXPATHL)
									== OK)
				{
				    char_u  *p;

				    ff_expand_బఫరు.నిడివి = STRLEN(ff_expand_బఫరు.string);
				    p = shorten_fname(file_మార్గం.string,
							    ff_expand_బఫరు.string);
				    ఒకవేళ (p != ఖాళీ)
				    {
					mch_memmove(file_మార్గం.string, p,
					    (size_t)((file_మార్గం.string + file_మార్గం.నిడివి) - p) + 1);  // +1 for NUL
					file_మార్గం.నిడివి -= (p - file_మార్గం.string);
				    }
				}
#నిర్వచితమైతే FF_VERBOSE
				ఒకవేళ (p_verbose >= 5)
				{
				    verbose_enter_scroll();
				    smsg("HIT: %s", file_మార్గం.string);
				    // don't overwrite this either
				    సందేశం_puts("\n");
				    verbose_leave_scroll();
				}
#అయితే_ముగింపు
				తిరిగి file_మార్గం.string;
			    }

			    // Not found or found already, try next suffix.
			    ఒకవేళ (*suf == NUL)
				ఆపు;
			    file_మార్గం.నిడివి = len + copy_option_part(&suf,
				  file_మార్గం.string + len,
				  (పూర్ణాంకం)(MAXPATHL - len), ",");
			}
		    }
		}
		లేకపోతే
		{
		    /*
		     * still wildcards left, push the directories for further
		     * search
		     */
		    కొరకు (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_కొలత; ++i)
		    {
			ఒకవేళ (!mch_isdir(stackp->ffs_filearray[i]))
			    కొనసాగు;	// not a directory

			ff_నెట్టు(శోధన_ctx,
				ff_create_దొంతర_element(
						     stackp->ffs_filearray[i],
						     STRLEN(stackp->ffs_filearray[i]),
						     rest_యొక్క_wildcards.string,
						     rest_యొక్క_wildcards.నిడివి,
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
	    ఒకవేళ (STRNCMP(stackp->ffs_wc_మార్గం.string, "**", 2) == 0)
	    {
		పూర్ణాంకం i;

		కొరకు (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_కొలత; ++i)
		{
		    ఒకవేళ (fnamecmp(stackp->ffs_filearray[i],
						   stackp->ffs_fix_మార్గం.string) == 0)
			కొనసాగు; // don't repush same directory
		    ఒకవేళ (!mch_isdir(stackp->ffs_filearray[i]))
			కొనసాగు;   // not a directory
		    ff_నెట్టు(శోధన_ctx,
			    ff_create_దొంతర_element(
				stackp->ffs_filearray[i],
				STRLEN(stackp->ffs_filearray[i]),
				stackp->ffs_wc_మార్గం.string,
				stackp->ffs_wc_మార్గం.నిడివి,
				stackp->ffs_level - 1, 1));
		}
	    }

	    // we are done with the current directory
	    ff_free_దొంతర_element(stackp);
	}

	// If we reached this, we didn't find anything downwards.
	// Let's check if we should do an upward search.
	ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.string
		&& శోధన_ctx->ffsc_stopdirs_v != ఖాళీ && !got_int)
	{
	    ff_దొంతర_T  *sptr;
	    // path_end may point to the NUL or the previous path separator
	    పూర్ణాంకం plen = (మార్గం_చివర - శోధన_ctx->ffsc_ప్రారంభం_dir.string)
							  + (*మార్గం_చివర != NUL);

	    // is the last starting directory in the stop list?
	    ఒకవేళ (ff_మార్గం_లోపల_stoplist(శోధన_ctx->ffsc_ప్రారంభం_dir.string,
				    plen, శోధన_ctx->ffsc_stopdirs_v) == TRUE)
		ఆపు;

	    // cut of last dir
	    అయితే (మార్గం_చివర > శోధన_ctx->ffsc_ప్రారంభం_dir.string
						  && vim_ispathsep(*మార్గం_చివర))
		మార్గం_చివర--;
	    అయితే (మార్గం_చివర > శోధన_ctx->ffsc_ప్రారంభం_dir.string
					      && !vim_ispathsep(మార్గం_చివర[-1]))
		మార్గం_చివర--;
	    *మార్గం_చివర = NUL;

	    // we may have shortened search_ctx->ffsc_start_dir, so update it's length
	    శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి = (size_t)(మార్గం_చివర - శోధన_ctx->ffsc_ప్రారంభం_dir.string);
	    మార్గం_చివర--;

	    ఒకవేళ (*శోధన_ctx->ffsc_ప్రారంభం_dir.string == NUL)
		ఆపు;

	    ఒకవేళ (శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి + 1
		    + శోధన_ctx->ffsc_fix_మార్గం.నిడివి < MAXPATHL)
	    {
		పూర్ణాంకం కూడు_sep = !after_pathsep(శోధన_ctx->ffsc_ప్రారంభం_dir.string,
			    శోధన_ctx->ffsc_ప్రారంభం_dir.string + శోధన_ctx->ffsc_ప్రారంభం_dir.నిడివి);
		file_మార్గం.నిడివి = vim_snprintf(
			(అక్షరం *)file_మార్గం.string,
			MAXPATHL,
			"%s%s%s",
			శోధన_ctx->ffsc_ప్రారంభం_dir.string,
			కూడు_sep ? PATHSEPSTR : "",
			శోధన_ctx->ffsc_fix_మార్గం.string);
	    }
	    లేకపోతే
		వెళ్ళు fail;

	    // create a new stack entry
	    sptr = ff_create_దొంతర_element(file_మార్గం.string, file_మార్గం.నిడివి,
		    శోధన_ctx->ffsc_wc_మార్గం.string, శోధన_ctx->ffsc_wc_మార్గం.నిడివి,
		    శోధన_ctx->ffsc_level, 0);
	    ఒకవేళ (sptr == ఖాళీ)
		ఆపు;
	    ff_నెట్టు(శోధన_ctx, sptr);
	}
	లేకపోతే
	    ఆపు;
    }

fail:
    vim_free(file_మార్గం.string);
    తిరిగి ఖాళీ;
}

/*
 * Free the list of lists of visited files and directories
 * Can handle it if the passed search_context is NULL;
 */
    స్థిర శూన్య
vim_findfile_free_సందర్శించిన(శూన్య *శోధన_ctx_arg)
{
    ff_శోధన_ctx_T *శోధన_ctx;

    ఒకవేళ (శోధన_ctx_arg == ఖాళీ)
	తిరిగి;

    శోధన_ctx = (ff_శోధన_ctx_T *)శోధన_ctx_arg;
    vim_findfile_free_సందర్శించిన_list(&శోధన_ctx->ffsc_సందర్శించిన_lists_list);
    vim_findfile_free_సందర్శించిన_list(&శోధన_ctx->ffsc_dir_సందర్శించిన_lists_list);
}

    స్థిర శూన్య
vim_findfile_free_సందర్శించిన_list(ff_సందర్శించిన_list_hdr_T **list_headp)
{
    ff_సందర్శించిన_list_hdr_T *vp;

    అయితే (*list_headp != ఖాళీ)
    {
	vp = (*list_headp)->ffvl_తదుపరి;
	ff_free_సందర్శించిన_list((*list_headp)->ffvl_సందర్శించిన_list);

	vim_free((*list_headp)->ffvl_filename);
	vim_free(*list_headp);
	*list_headp = vp;
    }
    *list_headp = ఖాళీ;
}

    స్థిర శూన్య
ff_free_సందర్శించిన_list(ff_సందర్శించిన_T *vl)
{
    ff_సందర్శించిన_T *vp;

    అయితే (vl != ఖాళీ)
    {
	vp = vl->ffv_తదుపరి;
	vim_free(vl->ffv_wc_మార్గం);
	vim_free(vl);
	vl = vp;
    }
    vl = ఖాళీ;
}

/*
 * Returns the already visited list for the given filename. If none is found it
 * allocates a new one.
 */
    స్థిర ff_సందర్శించిన_list_hdr_T*
ff_get_సందర్శించిన_list(
    char_u			*filename,
    size_t			filenamelen,
    ff_సందర్శించిన_list_hdr_T	**list_headp)
{
    ff_సందర్శించిన_list_hdr_T  *retptr = ఖాళీ;

    // check if a visited list for the given filename exists
    ఒకవేళ (*list_headp != ఖాళీ)
    {
	retptr = *list_headp;
	అయితే (retptr != ఖాళీ)
	{
	    ఒకవేళ (fnamecmp(filename, retptr->ffvl_filename) == 0)
	    {
#నిర్వచితమైతే FF_VERBOSE
		ఒకవేళ (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("ff_get_visited_list: FOUND list for %s",
								    filename);
		    // don't overwrite this either
		    సందేశం_puts("\n");
		    verbose_leave_scroll();
		}
#అయితే_ముగింపు
		తిరిగి retptr;
	    }
	    retptr = retptr->ffvl_తదుపరి;
	}
    }

#నిర్వచితమైతే FF_VERBOSE
    ఒకవేళ (p_verbose >= 5)
    {
	verbose_enter_scroll();
	smsg("ff_get_visited_list: new list for %s", filename);
	// don't overwrite this either
	సందేశం_puts("\n");
	verbose_leave_scroll();
    }
#అయితే_ముగింపు

    /*
     * if we reach this we didn't find a list and we have to allocate new list
     */
    retptr = ALLOC_ONE(ff_సందర్శించిన_list_hdr_T);
    ఒకవేళ (retptr == ఖాళీ)
	తిరిగి ఖాళీ;

    retptr->ffvl_సందర్శించిన_list = ఖాళీ;
    retptr->ffvl_filename = vim_strnsave(filename, filenamelen);
    ఒకవేళ (retptr->ffvl_filename == ఖాళీ)
    {
	vim_free(retptr);
	తిరిగి ఖాళీ;
    }
    retptr->ffvl_తదుపరి = *list_headp;
    *list_headp = retptr;

    తిరిగి retptr;
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
    స్థిర పూర్ణాంకం
ff_wc_equal(char_u *s1, char_u *s2)
{
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		c1 = NUL;
    పూర్ణాంకం		c2 = NUL;
    పూర్ణాంకం		prev1 = NUL;
    పూర్ణాంకం		prev2 = NUL;

    ఒకవేళ (s1 == s2)
	తిరిగి TRUE;

    ఒకవేళ (s1 == ఖాళీ || s2 == ఖాళీ)
	తిరిగి FALSE;

    కొరకు (i = 0, j = 0; s1[i] != NUL && s2[j] != NUL;)
    {
	c1 = PTR2CHAR(s1 + i);
	c2 = PTR2CHAR(s2 + j);

	ఒకవేళ ((p_fic ? MB_TOLOWER(c1) != MB_TOLOWER(c2) : c1 != c2)
		&& (prev1 != '*' || prev2 != '*'))
	    తిరిగి FALSE;
	prev2 = prev1;
	prev1 = c1;

	i += mb_ptr2len(s1 + i);
	j += mb_ptr2len(s2 + j);
    }
    తిరిగి s1[i] == s2[j];
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
    స్థిర పూర్ణాంకం
ff_check_సందర్శించిన(
    ff_సందర్శించిన_T	**సందర్శించిన_list,
    char_u		*fname,
    size_t		fnamelen,
    char_u		*wc_మార్గం,
    size_t		wc_pathlen)
{
    ff_సందర్శించిన_T	*vp;
#నిర్వచితమైతే UNIX
    stat_T		st;
    పూర్ణాంకం			url = FALSE;
#అయితే_ముగింపు

    // For a URL we only compare the name, otherwise we compare the
    // device/inode (unix) or the full path name (not Unix).
    ఒకవేళ (మార్గం_with_url(fname))
    {
	vim_strncpy(ff_expand_బఫరు.string, fname, fnamelen);
	ff_expand_బఫరు.నిడివి = fnamelen;
#నిర్వచితమైతే UNIX
	url = TRUE;
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	ff_expand_బఫరు.string[0] = NUL;
	ff_expand_బఫరు.నిడివి = 0;
#నిర్వచితమైతే UNIX
	ఒకవేళ (mch_stat((అక్షరం *)fname, &st) < 0)
	    తిరిగి FAIL;
#లేకపోతే
	ఒకవేళ (vim_నిండిన_Name(fname, ff_expand_బఫరు.string, MAXPATHL, TRUE) == FAIL)
	    తిరిగి FAIL;
	ff_expand_బఫరు.నిడివి = STRLEN(ff_expand_బఫరు.string);
#అయితే_ముగింపు
    }

    // check against list of already visited files
    కొరకు (vp = *సందర్శించిన_list; vp != ఖాళీ; vp = vp->ffv_తదుపరి)
    {
	ఒకవేళ (
#నిర్వచితమైతే UNIX
		!url ? (vp->ffv_dev_valid && vp->ffv_dev == st.st_dev
						  && vp->ffv_ino == st.st_ino)
		     :
#అయితే_ముగింపు
		fnamecmp(vp->ffv_fname, ff_expand_బఫరు.string) == 0
	   )
	{
	    // are the wildcard parts equal
	    ఒకవేళ (ff_wc_equal(vp->ffv_wc_మార్గం, wc_మార్గం) == TRUE)
		// already visited
		తిరిగి FAIL;
	}
    }

    /*
     * New file/dir.  Add it to the list of visited files/dirs.
     */
    vp = alloc(
	     offsetof(ff_సందర్శించిన_T, ffv_fname) + ff_expand_బఫరు.నిడివి + 1);
    ఒకవేళ (vp == ఖాళీ)
	తిరిగి OK;

#నిర్వచితమైతే UNIX
    ఒకవేళ (!url)
    {
	vp->ffv_dev_valid = TRUE;
	vp->ffv_dev = st.st_dev;
	vp->ffv_ino = st.st_ino;
	vp->ffv_fname[0] = NUL;
    }
    లేకపోతే
    {
	vp->ffv_dev_valid = FALSE;
#అయితే_ముగింపు
	STRCPY(vp->ffv_fname, ff_expand_బఫరు.string);
#నిర్వచితమైతే UNIX
    }
#అయితే_ముగింపు
    ఒకవేళ (wc_మార్గం != ఖాళీ)
	vp->ffv_wc_మార్గం = vim_strnsave(wc_మార్గం, wc_pathlen);
    లేకపోతే
	vp->ffv_wc_మార్గం = ఖాళీ;

    vp->ffv_తదుపరి = *సందర్శించిన_list;
    *సందర్శించిన_list = vp;

    తిరిగి OK;
}

/*
 * create stack element from given path pieces
 */
    స్థిర ff_దొంతర_T *
ff_create_దొంతర_element(
    char_u	*fix_part,
    size_t	fix_partlen,
    char_u	*wc_part,
    size_t	wc_partlen,
    పూర్ణాంకం		level,
    పూర్ణాంకం		నక్షత్రం_నక్షత్రం_ఖాళీలు)
{
    ff_దొంతర_T	*new;

    new = ALLOC_ONE(ff_దొంతర_T);
    ఒకవేళ (new == ఖాళీ)
	తిరిగి ఖాళీ;

    new->ffs_prev	   = ఖాళీ;
    new->ffs_filearray	   = ఖాళీ;
    new->ffs_filearray_కొలత = 0;
    new->ffs_filearray_cur  = 0;
    new->ffs_stage	   = 0;
    new->ffs_level	   = level;
    new->ffs_నక్షత్రం_నక్షత్రం_ఖాళీలు = నక్షత్రం_నక్షత్రం_ఖాళీలు;

    // the following saves NULL pointer checks in vim_findfile
    ఒకవేళ (fix_part == ఖాళీ)
    {
	fix_part = (char_u *)"";
	fix_partlen = 0;
    }
    new->ffs_fix_మార్గం.string = vim_strnsave(fix_part, fix_partlen);
    new->ffs_fix_మార్గం.నిడివి = fix_partlen;

    ఒకవేళ (wc_part == ఖాళీ)
    {
	wc_part = (char_u *)"";
	wc_partlen = 0;
    }
    new->ffs_wc_మార్గం.string = vim_strnsave(wc_part, wc_partlen);
    new->ffs_wc_మార్గం.నిడివి = wc_partlen;

    ఒకవేళ (new->ffs_fix_మార్గం.string == ఖాళీ || new->ffs_wc_మార్గం.string == ఖాళీ)
    {
	ff_free_దొంతర_element(new);
	new = ఖాళీ;
    }

    తిరిగి new;
}

/*
 * Push a dir on the directory stack.
 */
    స్థిర శూన్య
ff_నెట్టు(ff_శోధన_ctx_T *శోధన_ctx, ff_దొంతర_T *దొంతర_ptr)
{
    // check for NULL pointer, not to return an error to the user, but
    // to prevent a crash
    ఒకవేళ (దొంతర_ptr == ఖాళీ)
	తిరిగి;

    దొంతర_ptr->ffs_prev = శోధన_ctx->ffsc_దొంతర_ptr;
    శోధన_ctx->ffsc_దొంతర_ptr = దొంతర_ptr;
}

/*
 * Pop a dir from the directory stack.
 * Returns NULL if stack is empty.
 */
    స్థిర ff_దొంతర_T *
ff_తీయి(ff_శోధన_ctx_T *శోధన_ctx)
{
    ff_దొంతర_T  *sptr;

    sptr = శోధన_ctx->ffsc_దొంతర_ptr;
    ఒకవేళ (శోధన_ctx->ffsc_దొంతర_ptr != ఖాళీ)
	శోధన_ctx->ffsc_దొంతర_ptr = శోధన_ctx->ffsc_దొంతర_ptr->ffs_prev;

    తిరిగి sptr;
}

/*
 * free the given stack element
 */
    స్థిర శూన్య
ff_free_దొంతర_element(ff_దొంతర_T *దొంతర_ptr)
{
    // VIM_CLEAR_STRING handles possible NULL pointers
    VIM_CLEAR_STRING(దొంతర_ptr->ffs_fix_మార్గం);
    VIM_CLEAR_STRING(దొంతర_ptr->ffs_wc_మార్గం);

    ఒకవేళ (దొంతర_ptr->ffs_filearray != ఖాళీ)
	FreeWild(దొంతర_ptr->ffs_filearray_కొలత, దొంతర_ptr->ffs_filearray);

    vim_free(దొంతర_ptr);
}

/*
 * Clear the search context, but NOT the visited list.
 */
    స్థిర శూన్య
ff_clear(ff_శోధన_ctx_T *శోధన_ctx)
{
    ff_దొంతర_T   *sptr;

    // clear up stack
    అయితే ((sptr = ff_తీయి(శోధన_ctx)) != ఖాళీ)
	ff_free_దొంతర_element(sptr);

    ఒకవేళ (శోధన_ctx->ffsc_stopdirs_v != ఖాళీ)
    {
	పూర్ణాంకం  i = 0;

	అయితే (శోధన_ctx->ffsc_stopdirs_v[i].string != ఖాళీ)
	{
	    vim_free(శోధన_ctx->ffsc_stopdirs_v[i].string);
	    i++;
	}
	VIM_CLEAR(శోధన_ctx->ffsc_stopdirs_v);
    }

    // reset everything
    VIM_CLEAR_STRING(శోధన_ctx->ffsc_file_గమ్యం_శోధన);
    VIM_CLEAR_STRING(శోధన_ctx->ffsc_ప్రారంభం_dir);
    VIM_CLEAR_STRING(శోధన_ctx->ffsc_fix_మార్గం);
    VIM_CLEAR_STRING(శోధన_ctx->ffsc_wc_మార్గం);
    శోధన_ctx->ffsc_level = 0;
}

/*
 * check if the given path is in the stopdirs
 * returns TRUE if yes else FALSE
 */
    స్థిర పూర్ణాంకం
ff_మార్గం_లోపల_stoplist(char_u *మార్గం, పూర్ణాంకం మార్గం_len, string_T *stopdirs_v)
{
    పూర్ణాంకం		i = 0;

    // eat up trailing path separators, except the first
    అయితే (మార్గం_len > 1 && vim_ispathsep(మార్గం[మార్గం_len - 1]))
	మార్గం_len--;

    // if no path consider it as match
    ఒకవేళ (మార్గం_len == 0)
	తిరిగి TRUE;

    కొరకు (i = 0; stopdirs_v[i].string != ఖాళీ; i++)
	// match for parent directory. So '/home' also matches
	// '/home/rks'. Check for PATHSEP in stopdirs_v[i], else
	// '/home/r' would also match '/home/rks'
	ఒకవేళ (fnamencmp(stopdirs_v[i].string, మార్గం, మార్గం_len) == 0
		&& ((పూర్ణాంకం)stopdirs_v[i].నిడివి <= మార్గం_len
		    || vim_ispathsep(stopdirs_v[i].string[మార్గం_len])))
	    తిరిగి TRUE;

    తిరిగి FALSE;
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
కనుగొను_file_లోపల_మార్గం(
    char_u	*ptr,		// file name
    పూర్ణాంకం		len,		// length of file name
    పూర్ణాంకం		options,
    పూర్ణాంకం		మొదటి,		// use count'th matching file name
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_గమ్యం_కనుగొను,	// in/out: modified copy of file name
    అక్షరం	**శోధన_ctx)	// in/out: state of the search
{
    తిరిగి కనుగొను_file_లోపల_మార్గం_option(ptr, len, options, మొదటి,
	    *curbuf->b_p_మార్గం == NUL ? p_మార్గం : curbuf->b_p_మార్గం,
	    FINDFILE_BOTH, rel_fname, curbuf->b_p_sua,
	    file_గమ్యం_కనుగొను, శోధన_ctx);
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_findfile(శూన్య)
{
    VIM_CLEAR_STRING(ff_expand_బఫరు);
}
#అయితే_ముగింపు

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
కనుగొను_directory_లోపల_మార్గం(
    char_u	*ptr,		// file name
    పూర్ణాంకం		len,		// length of file name
    పూర్ణాంకం		options,
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_గమ్యం_కనుగొను,	// in/out: modified copy of file name
    అక్షరం	**శోధన_ctx)	// in/out: state of the search
{
    తిరిగి కనుగొను_file_లోపల_మార్గం_option(ptr, len, options, TRUE, p_cdpath,
				       FINDFILE_DIR, rel_fname, (char_u *)"",
				       file_గమ్యం_కనుగొను, శోధన_ctx);
}

    char_u *
కనుగొను_file_లోపల_మార్గం_option(
    char_u	*ptr,		// file name
    పూర్ణాంకం		len,		// length of file name
    పూర్ణాంకం		options,
    పూర్ణాంకం		మొదటి,		// use count'th matching file name
    char_u	*మార్గం_option,	// p_path or p_cdpath
    పూర్ణాంకం		కనుగొను_what,	// FINDFILE_FILE, _DIR or _BOTH
    char_u	*rel_fname,	// file name we are looking relative to.
    char_u	*suffixes,	// list of suffixes, 'suffixesadd' option
    char_u	**file_గమ్యం_కనుగొను,	// in/out: modified copy of file name
    అక్షరం	**శోధన_ctx_arg) // in/out: state of the search
{
    ff_శోధన_ctx_T	**శోధన_ctx = (ff_శోధన_ctx_T **)శోధన_ctx_arg;
    స్థిర char_u	*dir;
    స్థిర పూర్ణాంకం		did_findfile_init = FALSE;
    char_u		*file_name = ఖాళీ;
    పూర్ణాంకం			rel_గమ్యం_curdir;
#నిర్వచితమైతే AMIGA
    నిర్మాణం Process	*proc = (నిర్మాణం Process *)కనుగొను_Task(0L);
    APTR		save_winptr = proc->pr_కిటికీ_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_కిటికీ_Ptr = (APTR)-1L;
#అయితే_ముగింపు
    స్థిర size_t	file_గమ్యం_findlen = 0;

    ఒకవేళ (మొదటి == TRUE)
    {
	char_u	save_char;

	ఒకవేళ (len == 0)
	    తిరిగి ఖాళీ;

	// copy file name into NameBuff, expanding environment variables
	save_char = ptr[len];
	ptr[len] = NUL;
	file_గమ్యం_findlen = expand_env_esc(ptr, NameBuff, MAXPATHL, ఖాళీ, TRUE, ఖాళీ);
	ptr[len] = save_char;

	vim_free(*file_గమ్యం_కనుగొను);
	*file_గమ్యం_కనుగొను = vim_strnsave(NameBuff, file_గమ్యం_findlen);
	ఒకవేళ (*file_గమ్యం_కనుగొను == ఖాళీ)	// out of memory
	{
	    file_name = ఖాళీ;
	    file_గమ్యం_findlen = 0;
	    వెళ్ళు theend;
	}
	ఒకవేళ (options & FNAME_UNESC)
	{
	    // Change all "\ " to " ".
	    కొరకు (ptr = *file_గమ్యం_కనుగొను; *ptr != NUL; ++ptr)
		ఒకవేళ (ptr[0] == '\\' && ptr[1] == ' ')
		{
		    mch_memmove(ptr, ptr + 1,
			(size_t)((*file_గమ్యం_కనుగొను + file_గమ్యం_findlen) - (ptr + 1)) + 1);
		    --file_గమ్యం_findlen;
		}
	}
    }

    rel_గమ్యం_curdir = ((*file_గమ్యం_కనుగొను)[0] == '.'
		    && ((*file_గమ్యం_కనుగొను)[1] == NUL
			|| vim_ispathsep((*file_గమ్యం_కనుగొను)[1])
			|| ((*file_గమ్యం_కనుగొను)[1] == '.'
			    && ((*file_గమ్యం_కనుగొను)[2] == NUL
				|| vim_ispathsep((*file_గమ్యం_కనుగొను)[2])))));
    ఒకవేళ (vim_isAbsName(*file_గమ్యం_కనుగొను)
	    // "..", "../path", "." and "./path": don't use the path_option
	    || rel_గమ్యం_curdir
#ఒకవేళ defined(MSWIN)
	    // handle "\tmp" as absolute path
	    || vim_ispathsep((*file_గమ్యం_కనుగొను)[0])
	    // handle "c:name" as absolute path
	    || ((*file_గమ్యం_కనుగొను)[0] != NUL && (*file_గమ్యం_కనుగొను)[1] == ':')
#అయితే_ముగింపు
#నిర్వచితమైతే AMIGA
	    // handle ":tmp" as absolute path
	    || (*file_గమ్యం_కనుగొను)[0] == ':'
#అయితే_ముగింపు
       )
    {
	/*
	 * Absolute path, no need to use "path_option".
	 * If this is not a first call, return NULL.  We already returned a
	 * filename on the first call.
	 */
	ఒకవేళ (మొదటి == TRUE)
	{
	    పూర్ణాంకం		l;
	    పూర్ణాంకం		నిరంతరం;
	    size_t	rel_fnamelen = 0;
	    char_u	*suffix;

	    ఒకవేళ (మార్గం_with_url(*file_గమ్యం_కనుగొను))
	    {
		file_name = vim_strnsave(*file_గమ్యం_కనుగొను, file_గమ్యం_findlen);
		వెళ్ళు theend;
	    }

	    ఒకవేళ (rel_fname != ఖాళీ)
		rel_fnamelen = STRLEN(rel_fname);

	    // When FNAME_REL flag given first use the directory of the file.
	    // Otherwise or when this fails use the current directory.
	    కొరకు (నిరంతరం = 1; నిరంతరం <= 2; ++నిరంతరం)
	    {
		l = (పూర్ణాంకం)file_గమ్యం_findlen;
		ఒకవేళ (నిరంతరం == 1
			&& rel_గమ్యం_curdir
			&& (options & FNAME_REL)
			&& rel_fname != ఖాళీ
			&& rel_fnamelen + l < MAXPATHL)
		{
		    l = vim_snprintf(
			(అక్షరం *)NameBuff,
			MAXPATHL,
			"%.*s%s",
			(పూర్ణాంకం)(gettail(rel_fname) - rel_fname),
			rel_fname,
			*file_గమ్యం_కనుగొను);
		}
		లేకపోతే
		{
		    STRCPY(NameBuff, *file_గమ్యం_కనుగొను);
		    నిరంతరం = 2;
		}

		// When the file doesn't exist, try adding parts of
		// 'suffixesadd'.
		suffix = suffixes;
		కొరకు (;;)
		{
		    ఒకవేళ (mch_getperm(NameBuff) >= 0
			     && (కనుగొను_what == FINDFILE_BOTH
				 || ((కనుగొను_what == FINDFILE_DIR)
						    == mch_isdir(NameBuff))))
		    {
			file_name = vim_strnsave(NameBuff,
						  simplify_filename(NameBuff));
			వెళ్ళు theend;
		    }
		    ఒకవేళ (*suffix == NUL)
			ఆపు;
		    copy_option_part(&suffix, NameBuff + l, MAXPATHL - l, ",");
		}
	    }
	}
    }
    లేకపోతే
    {
	/*
	 * Loop over all paths in the 'path' or 'cdpath' option.
	 * When "first" is set, first setup to the start of the option.
	 * Otherwise continue to find the next match.
	 */
	ఒకవేళ (మొదటి == TRUE)
	{
	    // vim_findfile_free_visited can handle a possible NULL pointer
	    vim_findfile_free_సందర్శించిన(*శోధన_ctx);
	    dir = మార్గం_option;
	    did_findfile_init = FALSE;
	}

	కొరకు (;;)
	{
	    ఒకవేళ (did_findfile_init)
	    {
		file_name = vim_findfile(*శోధన_ctx);
		ఒకవేళ (file_name != ఖాళీ)
		    ఆపు;

		did_findfile_init = FALSE;
	    }
	    లేకపోతే
	    {
		char_u	*buf;
		char_u  *r_ptr;

		ఒకవేళ (dir == ఖాళీ || *dir == NUL)
		{
		    // We searched all paths of the option, now we can
		    // free the search context.
		    vim_findfile_cleanup(*శోధన_ctx);
		    *శోధన_ctx = ఖాళీ;
		    ఆపు;
		}

		ఒకవేళ ((buf = alloc(MAXPATHL)) == ఖాళీ)
		    ఆపు;

		// copy next path
		buf[0] = NUL;
		copy_option_part(&dir, buf, MAXPATHL, " ,");

		// get the stopdir string
		r_ptr = vim_findfile_stopdir(buf);
		*శోధన_ctx = vim_findfile_init(buf, *file_గమ్యం_కనుగొను, file_గమ్యం_findlen,
					    r_ptr, 100, FALSE, కనుగొను_what,
					   *శోధన_ctx, FALSE, rel_fname);
		ఒకవేళ (*శోధన_ctx != ఖాళీ)
		    did_findfile_init = TRUE;
		vim_free(buf);
	    }
	}
    }
    ఒకవేళ (file_name == ఖాళీ && (options & FNAME_MESS))
    {
	ఒకవేళ (మొదటి == TRUE)
	{
	    ఒకవేళ (కనుగొను_what == FINDFILE_DIR)
		semsg(_(e_cant_కనుగొను_directory_str_లోపల_cdpath), *file_గమ్యం_కనుగొను);
	    లేకపోతే
		semsg(_(e_cant_కనుగొను_file_str_లోపల_మార్గం), *file_గమ్యం_కనుగొను);
	}
	లేకపోతే
	{
	    ఒకవేళ (కనుగొను_what == FINDFILE_DIR)
		semsg(_(e_no_మరిన్ని_directory_str_దొరికింది_లోపల_cdpath),
								*file_గమ్యం_కనుగొను);
	    లేకపోతే
		semsg(_(e_no_మరిన్ని_file_str_దొరికింది_లోపల_మార్గం), *file_గమ్యం_కనుగొను);
	}
    }

theend:
#నిర్వచితమైతే AMIGA
    proc->pr_కిటికీ_Ptr = save_winptr;
#అయితే_ముగింపు
    తిరిగి file_name;
}

/*
 * Get the file name at the cursor.
 * If Visual mode is active, use the selected text if it's in one line.
 * Returns the name in allocated memory, NULL for failure.
 */
    char_u *
grab_file_name(దీర్ఘ లెక్క, linenr_T *file_lnum)
{
    పూర్ణాంకం options = FNAME_MESS|FNAME_EXP|FNAME_REL|FNAME_UNESC;

    ఒకవేళ (VIsual_చురుకైన)
    {
	పూర్ణాంకం	len;
	char_u	*ptr;

	ఒకవేళ (get_visual_పాఠ్యం(ఖాళీ, &ptr, &len) == FAIL)
	    తిరిగి ఖాళీ;
	// Only recognize ":123" here
	ఒకవేళ (file_lnum != ఖాళీ && ptr[len] == ':' && సురక్షిత_isdigit(ptr[len + 1]))
	{
	    char_u *p = ptr + len + 1;

	    *file_lnum = getdigits(&p);
	}
	తిరిగి కనుగొను_file_name_లోపల_మార్గం(ptr, len, options,
						     లెక్క, curbuf->b_ffname);
    }
    తిరిగి file_name_at_cursor(options | FNAME_HYP, లెక్క, file_lnum);
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
file_name_at_cursor(పూర్ణాంకం options, దీర్ఘ లెక్క, linenr_T *file_lnum)
{
    తిరిగి file_name_లోపల_పంక్తి(ml_get_curline(),
		      curwin->w_cursor.నిలువు, options, లెక్క, curbuf->b_ffname,
		      file_lnum);
}

/*
 * Return the name of the file under or after ptr[col].
 * Otherwise like file_name_at_cursor().
 */
    char_u *
file_name_లోపల_పంక్తి(
    char_u	*పంక్తి,
    పూర్ణాంకం		నిలువు,
    పూర్ణాంకం		options,
    దీర్ఘ	లెక్క,
    char_u	*rel_fname,	// file we are searching relative to
    linenr_T	*file_lnum)	// line number after the file name
{
    char_u	*ptr;
    పూర్ణాంకం		len;
    పూర్ణాంకం		లోపల_type = TRUE;
    పూర్ణాంకం		is_url = FALSE;

    /*
     * search forward for what could be the start of a file name
     */
    ptr = పంక్తి + నిలువు;
    అయితే (*ptr != NUL && !vim_isfilec(*ptr))
	MB_PTR_ADV(ptr);
    ఒకవేళ (*ptr == NUL)		// nothing found
    {
	ఒకవేళ (options & FNAME_MESS)
	    emsg(_(e_no_file_name_under_cursor));
	తిరిగి ఖాళీ;
    }

    /*
     * Search backward for first char of the file name.
     * Go one char back to ":" before "//" even when ':' is not in 'isfname'.
     */
    అయితే (ptr > పంక్తి)
    {
	ఒకవేళ (has_mbyte && (len = (*mb_తల_off)(పంక్తి, ptr - 1)) > 0)
	    ptr -= len + 1;
	లేకపోతే ఒకవేళ (vim_isfilec(ptr[-1])
		|| ((options & FNAME_HYP) && మార్గం_is_url(ptr - 1)))
	    --ptr;
	లేకపోతే
	    ఆపు;
    }

    /*
     * Search forward for the last char of the file name.
     * Also allow "://" when ':' is not in 'isfname'.
     */
    len = 0;
    అయితే (vim_isfilec(ptr[len]) || (ptr[len] == '\\' && ptr[len + 1] == ' ')
		 || ((options & FNAME_HYP) && మార్గం_is_url(ptr + len))
		 || (is_url && vim_strchr((char_u *)":?&=", ptr[len]) != ఖాళీ))
    {
	// After type:// we also include :, ?, & and = as valid characters, so
	// that http://google.com:8080?q=this&that=ok works.
	ఒకవేళ ((ptr[len] >= 'A' && ptr[len] <= 'Z')
				       || (ptr[len] >= 'a' && ptr[len] <= 'z'))
	{
	    ఒకవేళ (లోపల_type && మార్గం_is_url(ptr + len + 1))
		is_url = TRUE;
	}
	లేకపోతే
	    లోపల_type = FALSE;

	ఒకవేళ (ptr[len] == '\\')
	    // Skip over the "\" in "\ ".
	    ++len;
	ఒకవేళ (has_mbyte)
	    len += (*mb_ptr2len)(ptr + len);
	లేకపోతే
	    ++len;
    }

    /*
     * If there is trailing punctuation, remove it.
     * But don't remove "..", could be a directory name.
     */
    ఒకవేళ (len > 2 && vim_strchr((char_u *)".,:;!", ptr[len - 1]) != ఖాళీ
						       && ptr[len - 2] != '.')
	--len;

    ఒకవేళ (file_lnum != ఖాళీ)
    {
	char_u	*p;
	అక్షరం	*సరిపోలిక_పాఠ్యం = " line ";		// english
	size_t	సరిపోలిక_textlen = 6;

	// Get the number after the file name and a separator character.
	// Also accept " line 999" with and without the same translation as
	// used in last_set_msg().
	p = ptr + len;
	ఒకవేళ (STRNCMP(p, సరిపోలిక_పాఠ్యం, సరిపోలిక_textlen) == 0)
	    p += సరిపోలిక_textlen;
	లేకపోతే
	{
	    // no match with english, try localized
	    సరిపోలిక_పాఠ్యం = _(పంక్తి_సందేశం);
	    సరిపోలిక_textlen = STRLEN(సరిపోలిక_పాఠ్యం);

	    ఒకవేళ (STRNCMP(p, సరిపోలిక_పాఠ్యం, సరిపోలిక_textlen) == 0)
		p += సరిపోలిక_textlen;
	    లేకపోతే
		p = skipwhite(p);
	}
	ఒకవేళ (*p != NUL)
	{
	    ఒకవేళ (!సురక్షిత_isdigit(*p))
		++p;		    // skip the separator
	    p = skipwhite(p);
	    ఒకవేళ (సురక్షిత_isdigit(*p))
		*file_lnum = (పూర్ణాంకం)getdigits(&p);
	}
    }

    తిరిగి కనుగొను_file_name_లోపల_మార్గం(ptr, len, options, లెక్క, rel_fname);
}

#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_EVAL)
    స్థిర char_u *
eval_includeexpr(char_u *ptr, పూర్ణాంకం len)
{
    char_u	*res;
    sctx_T	save_sctx = current_sctx;

    సమితి_vim_var_string(VV_FNAME, ptr, len);
    current_sctx = curbuf->b_p_script_ctx[BV_INEX];

    res = eval_గమ్యం_string_సురక్షిత(curbuf->b_p_inex,
	    was_సమితి_insecurely(curwin, (char_u *)"includeexpr", OPT_LOCAL),
								   TRUE, TRUE);

    సమితి_vim_var_string(VV_FNAME, ఖాళీ, 0);
    current_sctx = save_sctx;
    తిరిగి res;
}
#అయితే_ముగింపు

/*
 * Return the name of the file ptr[len] in 'path'.
 * Otherwise like file_name_at_cursor().
 */
    char_u *
కనుగొను_file_name_లోపల_మార్గం(
    char_u	*ptr,
    పూర్ణాంకం		len,
    పూర్ణాంకం		options,
    దీర్ఘ	లెక్క,
    char_u	*rel_fname)	// file we are searching relative to
{
    char_u	*file_name;
    పూర్ణాంకం		c;
#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_EVAL)
    char_u	*tofree = ఖాళీ;
#అయితే_ముగింపు

    ఒకవేళ (len == 0)
	తిరిగి ఖాళీ;

#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_EVAL)
    ఒకవేళ ((options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
    {
	tofree = eval_includeexpr(ptr, len);
	ఒకవేళ (tofree != ఖాళీ)
	{
	    ptr = tofree;
	    len = (పూర్ణాంకం)STRLEN(ptr);
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (options & FNAME_EXP)
    {
	char_u	*file_గమ్యం_కనుగొను = ఖాళీ;
	అక్షరం	*శోధన_ctx = ఖాళీ;

	file_name = కనుగొను_file_లోపల_మార్గం(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_గమ్యం_కనుగొను, &శోధన_ctx);

#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_EVAL)
	/*
	 * If the file could not be found in a normal way, try applying
	 * 'includeexpr' (unless done already).
	 */
	ఒకవేళ (file_name == ఖాళీ
		&& !(options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
	{
	    tofree = eval_includeexpr(ptr, len);
	    ఒకవేళ (tofree != ఖాళీ)
	    {
		ptr = tofree;
		len = (పూర్ణాంకం)STRLEN(ptr);
		file_name = కనుగొను_file_లోపల_మార్గం(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_గమ్యం_కనుగొను, &శోధన_ctx);
	    }
	}
#అయితే_ముగింపు
	ఒకవేళ (file_name == ఖాళీ && (options & FNAME_MESS))
	{
	    c = ptr[len];
	    ptr[len] = NUL;
	    semsg(_(e_cant_కనుగొను_file_str_లోపల_మార్గం_2), ptr);
	    ptr[len] = c;
	}

	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	అయితే (file_name != ఖాళీ && --లెక్క > 0)
	{
	    vim_free(file_name);
	    file_name = కనుగొను_file_లోపల_మార్గం(ptr, len, options, FALSE, rel_fname,
						   &file_గమ్యం_కనుగొను, &శోధన_ctx);
	}

	vim_free(file_గమ్యం_కనుగొను);
	vim_findfile_cleanup(శోధన_ctx);
    }
    లేకపోతే
	file_name = vim_strnsave(ptr, len);

#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_EVAL)
    vim_free(tofree);
#అయితే_ముగింపు

    తిరిగి file_name;
}

/*
 * Return the end of the directory name, on the first path
 * separator:
 * "/path/file", "/path/dir/", "/path//dir", "/file"
 *	 ^	       ^	     ^	      ^
 */
    స్థిర char_u *
gettail_dir(char_u *fname)
{
    char_u	*dir_చివర = fname;
    char_u	*తదుపరి_dir_చివర = fname;
    పూర్ణాంకం		look_for_sep = TRUE;
    char_u	*p;

    కొరకు (p = fname; *p != NUL; )
    {
	ఒకవేళ (vim_ispathsep(*p))
	{
	    ఒకవేళ (look_for_sep)
	    {
		తదుపరి_dir_చివర = p;
		look_for_sep = FALSE;
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (!look_for_sep)
		dir_చివర = తదుపరి_dir_చివర;
	    look_for_sep = TRUE;
	}
	MB_PTR_ADV(p);
    }
    తిరిగి dir_చివర;
}

/*
 * return TRUE if 'c' is a path list separator.
 */
    పూర్ణాంకం
vim_ispathlistsep(పూర్ణాంకం c)
{
#నిర్వచితమైతే UNIX
    తిరిగి (c == ':');
#లేకపోతే
    తిరిగి (c == ';');	// might not be right for every system...
#అయితే_ముగింపు
}

/*
 * Moves "*psep" back to the previous path separator in "path".
 * Returns FAIL is "*psep" ends up at the beginning of "path".
 */
    స్థిర పూర్ణాంకం
కనుగొను_మునుపటి_pathsep(char_u *మార్గం, char_u **psep)
{
    // skip the current separator
    ఒకవేళ (*psep > మార్గం && vim_ispathsep(**psep))
	--*psep;

    // find the previous separator
    అయితే (*psep > మార్గం)
    {
	ఒకవేళ (vim_ispathsep(**psep))
	    తిరిగి OK;
	MB_PTR_వెనుక(మార్గం, *psep);
    }

    తిరిగి FAIL;
}

/*
 * Returns TRUE if "maybe_unique" is unique wrt other_paths in "gap".
 * "maybe_unique" is the end portion of "((char_u **)gap->ga_data)[i]".
 */
    స్థిర పూర్ణాంకం
is_unique(char_u *maybe_unique, garray_T *gap, పూర్ణాంకం i)
{
    పూర్ణాంకం	    j;
    పూర్ణాంకం	    candidate_len = (పూర్ణాంకం)STRLEN(maybe_unique);
    పూర్ణాంకం	    other_మార్గం_len;
    char_u  **other_paths = (char_u **)gap->ga_దత్తాంశం;
    char_u  *rival;

    కొరకు (j = 0; j < gap->ga_len; j++)
    {
	ఒకవేళ (j == i)
	    కొనసాగు;  // don't compare it with itself

	other_మార్గం_len = (పూర్ణాంకం)STRLEN(other_paths[j]);
	ఒకవేళ (other_మార్గం_len < candidate_len)
	    కొనసాగు;  // it's different when it's shorter

	rival = other_paths[j] + other_మార్గం_len - candidate_len;
	ఒకవేళ (fnamecmp(maybe_unique, rival) == 0
		&& (rival == other_paths[j] || vim_ispathsep(*(rival - 1))))
	    తిరిగి FALSE;  //సరిపోలిక
    }

    తిరిగి TRUE;  // no match found
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
    స్థిర శూన్య
expand_మార్గం_option(
	char_u		*curdir,
	char_u		*మార్గం_option,	// p_path or p_cdpath
	garray_T	*gap)
{
    char_u	*buf;
    size_t	buflen;
    char_u	*p;
    size_t	curdirlen = 0;

    ఒకవేళ ((buf = alloc(MAXPATHL)) == ఖాళీ)
	తిరిగి;

    అయితే (*మార్గం_option != NUL)
    {
	buflen = copy_option_part(&మార్గం_option, buf, MAXPATHL, " ,");

	// do not expand backticks, could have been set via a modeline
	ఒకవేళ (vim_strchr(buf, '`') != ఖాళీ)
	    కొనసాగు;

	ఒకవేళ (buf[0] == '.' && (buf[1] == NUL || vim_ispathsep(buf[1])))
	{
	    size_t  plen;

	    // Relative to current buffer:
	    // "/path/file" + "." -> "/path/"
	    // "/path/file"  + "./subdir" -> "/path/subdir"
	    ఒకవేళ (curbuf->b_ffname == ఖాళీ)
		కొనసాగు;
	    p = gettail(curbuf->b_ffname);
	    plen = (size_t)(p - curbuf->b_ffname);
	    ఒకవేళ (plen + buflen >= MAXPATHL)
		కొనసాగు;
	    ఒకవేళ (buf[1] == NUL)
		buf[plen] = NUL;
	    లేకపోతే
		mch_memmove(buf + plen, buf + 2, (buflen - 2) + 1); // +1 for NUL
	    mch_memmove(buf, curbuf->b_ffname, plen);
	    buflen = simplify_filename(buf);
	}
	లేకపోతే ఒకవేళ (buf[0] == NUL)
	{
	    // relative to current directory
	    STRCPY(buf, curdir);
	    ఒకవేళ (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    buflen = curdirlen;
	}
	లేకపోతే ఒకవేళ (మార్గం_with_url(buf))
	    // URL can't be used here
	    కొనసాగు;
	లేకపోతే ఒకవేళ (!mch_is_నిండిన_Name(buf))
	{
	    // Expand relative path to their full path equivalent
	    ఒకవేళ (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    ఒకవేళ (curdirlen + buflen + 3 > MAXPATHL)
		కొనసాగు;

	    mch_memmove(buf + curdirlen + 1, buf, buflen + 1);	    // +1 for NUL
	    STRCPY(buf, curdir);
	    buf[curdirlen] = PATHSEP;
	    buflen = simplify_filename(buf);
	}

	ఒకవేళ (ga_grow(gap, 1) == FAIL)
	    ఆపు;

#ఒకవేళ defined(MSWIN)
	// Avoid the path ending in a backslash, it fails when a comma is
	// appended.
	ఒకవేళ (buf[buflen - 1] == '\\')
	    buf[buflen - 1] = '/';
#అయితే_ముగింపు

	p = vim_strnsave(buf, buflen);
	ఒకవేళ (p == ఖాళీ)
	    ఆపు;
	((char_u **)gap->ga_దత్తాంశం)[gap->ga_len++] = p;
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
    స్థిర char_u *
get_మార్గం_cutoff(char_u *fname, garray_T *gap)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    maxlen = 0;
    char_u  **మార్గం_part = (char_u **)gap->ga_దత్తాంశం;
    char_u  *cutoff = ఖాళీ;

    కొరకు (i = 0; i < gap->ga_len; i++)
    {
	పూర్ణాంకం j = 0;

	అయితే ((fname[j] == మార్గం_part[i][j]
#ఒకవేళ defined(MSWIN)
		|| (vim_ispathsep(fname[j]) && vim_ispathsep(మార్గం_part[i][j]))
#అయితే_ముగింపు
			     ) && fname[j] != NUL && మార్గం_part[i][j] != NUL)
	    j++;
	ఒకవేళ (j > maxlen)
	{
	    maxlen = j;
	    cutoff = &fname[j];
	}
    }

    // skip to the file or directory name
    ఒకవేళ (cutoff != ఖాళీ)
	అయితే (vim_ispathsep(*cutoff))
	    MB_PTR_ADV(cutoff);

    తిరిగి cutoff;
}

/*
 * Sorts, removes duplicates and modifies all the fullpath names in "gap" so
 * that they are unique with respect to each other while conserving the part
 * that matches the pattern. Beware, this is at least O(n^2) wrt "gap->ga_len".
 */
    శూన్య
uniquefy_paths(
	garray_T *gap,
	char_u *నమూనా,
	char_u *మార్గం_option)	// p_path or p_cdpath
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		len;
    char_u	**fnames = (char_u **)gap->ga_దత్తాంశం;
    పూర్ణాంకం		క్రమీకరించు_again = FALSE;
    char_u	*pat;
    char_u      *file_నమూనా;
    char_u	*curdir;
    regmatch_T	regmatch;
    garray_T	మార్గం_ga;
    char_u	**లోపల_curdir = ఖాళీ;
    char_u	*short_name;

    remove_duplicates(gap);
    ga_init2(&మార్గం_ga, పరిమాణం(char_u *), 1);

    /*
     * We need to prepend a '*' at the beginning of file_pattern so that the
     * regex matches anywhere in the path. FIXME: is this valid for all
     * possible patterns?
     */
    len = (పూర్ణాంకం)STRLEN(నమూనా);
    file_నమూనా = alloc(len + 2);
    ఒకవేళ (file_నమూనా == ఖాళీ)
	తిరిగి;
    file_నమూనా[0] = '*';
    STRCPY(file_నమూనా + 1, నమూనా);
    pat = file_pat_గమ్యం_reg_pat(file_నమూనా, ఖాళీ, ఖాళీ, FALSE);
    vim_free(file_నమూనా);
    ఒకవేళ (pat == ఖాళీ)
	తిరిగి;

    regmatch.rm_ic = TRUE;		// always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    vim_free(pat);
    ఒకవేళ (regmatch.regprog == ఖాళీ)
	తిరిగి;

    ఒకవేళ ((curdir = alloc(MAXPATHL)) == ఖాళీ)
	వెళ్ళు theend;
    mch_dirname(curdir, MAXPATHL);
    expand_మార్గం_option(curdir, మార్గం_option, &మార్గం_ga);

    లోపల_curdir = ALLOC_CLEAR_MULT(char_u *, gap->ga_len);
    ఒకవేళ (లోపల_curdir == ఖాళీ)
	వెళ్ళు theend;

    కొరకు (i = 0; i < gap->ga_len && !got_int; i++)
    {
	char_u	    *మార్గం = fnames[i];
	పూర్ణాంకం	    is_లోపల_curdir;
	char_u	    *dir_చివర = gettail_dir(మార్గం);
	char_u	    *మార్గం_cutoff;

	len = (పూర్ణాంకం)STRLEN(మార్గం);
	is_లోపల_curdir = fnamencmp(curdir, మార్గం, dir_చివర - మార్గం) == 0
					     && curdir[dir_చివర - మార్గం] == NUL;
	ఒకవేళ (is_లోపల_curdir)
	    లోపల_curdir[i] = vim_strnsave(మార్గం, len);

	// Shorten the filename while maintaining its uniqueness
	మార్గం_cutoff = get_మార్గం_cutoff(మార్గం, &మార్గం_ga);

	// Don't assume all files can be reached without path when search
	// pattern starts with star star slash, so only remove path_cutoff
	// when possible.
	ఒకవేళ (నమూనా[0] == '*' && నమూనా[1] == '*'
		&& vim_ispathsep_nocolon(నమూనా[2])
		&& మార్గం_cutoff != ఖాళీ
		&& vim_regexec(&regmatch, మార్గం_cutoff, (colnr_T)0)
		&& is_unique(మార్గం_cutoff, gap, i))
	{
	    క్రమీకరించు_again = TRUE;
	    mch_memmove(మార్గం, మార్గం_cutoff, STRLEN(మార్గం_cutoff) + 1);
	}
	లేకపోతే
	{
	    // Here all files can be reached without path, so get shortest
	    // unique path.  We start at the end of the path.
	    char_u  *pathsep_p = మార్గం + len - 1;

	    అయితే (కనుగొను_మునుపటి_pathsep(మార్గం, &pathsep_p))
	    {
		ఒకవేళ (vim_regexec(&regmatch, pathsep_p + 1, (colnr_T)0)
			&& is_unique(pathsep_p + 1, gap, i)
			&& మార్గం_cutoff != ఖాళీ && pathsep_p + 1 >= మార్గం_cutoff)
		{
		    క్రమీకరించు_again = TRUE;
		    mch_memmove(మార్గం, pathsep_p + 1,
			    (size_t)((మార్గం + len) - (pathsep_p + 1)) + 1);  // +1 for NUL
		    ఆపు;
		}
	    }
	}

	ఒకవేళ (mch_is_నిండిన_Name(మార్గం))
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
	    short_name = shorten_fname(మార్గం, curdir);
	    ఒకవేళ (short_name != ఖాళీ && short_name > మార్గం + 1
#ఒకవేళ defined(MSWIN)
		    // On windows,
		    //	    shorten_fname("c:\a\a.txt", "c:\a\b")
		    // returns "\a\a.txt", which is not really the short
		    // name, hence:
		    && !vim_ispathsep(*short_name)
#అయితే_ముగింపు
		)
	    {
		vim_snprintf((అక్షరం *)మార్గం, MAXPATHL, ".%s%s", PATHSEPSTR, short_name);
	    }
	}
	ui_breakcheck();
    }

    // Shorten filenames in /in/current/directory/{filename}
    కొరకు (i = 0; i < gap->ga_len && !got_int; i++)
    {
	size_t	rel_pathsize;
	char_u	*rel_మార్గం;
	char_u	*మార్గం = లోపల_curdir[i];

	ఒకవేళ (మార్గం == ఖాళీ)
	    కొనసాగు;

	// If the {filename} is not unique, change it to ./{filename}.
	// Else reduce it to {filename}
	short_name = shorten_fname(మార్గం, curdir);
	ఒకవేళ (short_name == ఖాళీ)
	    short_name = మార్గం;
	ఒకవేళ (is_unique(short_name, gap, i))
	{
	    STRCPY(fnames[i], short_name);
	    కొనసాగు;
	}

	rel_pathsize = 1 + పరిమాణం(PATHSEP) + STRLEN(short_name) + 1;
	rel_మార్గం = alloc(rel_pathsize);
	ఒకవేళ (rel_మార్గం == ఖాళీ)
	    వెళ్ళు theend;

	vim_snprintf((అక్షరం *)rel_మార్గం, rel_pathsize, ".%s%s", PATHSEPSTR, short_name);

	vim_free(fnames[i]);
	fnames[i] = rel_మార్గం;
	క్రమీకరించు_again = TRUE;
	ui_breakcheck();
    }

theend:
    vim_free(curdir);
    ఒకవేళ (లోపల_curdir != ఖాళీ)
    {
	కొరకు (i = 0; i < gap->ga_len; i++)
	    vim_free(లోపల_curdir[i]);
	vim_free(లోపల_curdir);
    }
    ga_clear_అక్షరశ్రేణులు(&మార్గం_ga);
    vim_regfree(regmatch.regprog);

    ఒకవేళ (క్రమీకరించు_again)
	remove_duplicates(gap);
}

/*
 * Calls globpath() with 'path' values for the given pattern and stores the
 * result in "gap".
 * Returns the total number of matches.
 */
    పూర్ణాంకం
expand_లోపల_మార్గం(
    garray_T	*gap,
    char_u	*నమూనా,
    పూర్ణాంకం		flags)		// EW_* flags
{
    char_u	*curdir;
    garray_T	మార్గం_ga;
    char_u	*paths = ఖాళీ;
    పూర్ణాంకం		glob_flags = 0;
    char_u	*మార్గం_option = *curbuf->b_p_మార్గం == NUL ? p_మార్గం : curbuf->b_p_మార్గం;

    ఒకవేళ ((curdir = alloc(MAXPATHL)) == ఖాళీ)
	తిరిగి 0;
    mch_dirname(curdir, MAXPATHL);

    ga_init2(&మార్గం_ga, పరిమాణం(char_u *), 1);
    ఒకవేళ (flags & EW_CDPATH)
	expand_మార్గం_option(curdir, p_cdpath, &మార్గం_ga);
    లేకపోతే
	expand_మార్గం_option(curdir, మార్గం_option, &మార్గం_ga);
    vim_free(curdir);
    ఒకవేళ (మార్గం_ga.ga_len == 0)
	తిరిగి 0;

    paths = ga_concat_అక్షరశ్రేణులు(&మార్గం_ga, ",");
    ga_clear_అక్షరశ్రేణులు(&మార్గం_ga);
    ఒకవేళ (paths == ఖాళీ)
	తిరిగి 0;

    ఒకవేళ (flags & EW_ICASE)
	glob_flags |= WILD_ICASE;
    ఒకవేళ (flags & EW_ADDSLASH)
	glob_flags |= WILD_కూడు_SLASH;
    globpath(paths, నమూనా, gap, glob_flags, !!(flags & EW_CDPATH));
    vim_free(paths);

    తిరిగి gap->ga_len;
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
#నిర్వచితం_కాకపోతే AMIGA	    // Amiga doesn't have "..", it uses "/"
    పూర్ణాంకం		components = 0;
    char_u	*p, *తోక, *ప్రారంభం;
    char_u	*p_చివర;			    // point to NUL at end of string "p"
    పూర్ణాంకం		stripping_disabled = FALSE;
    పూర్ణాంకం		relative = TRUE;

    p = filename;
# ifdef BACKSLASH_లోపల_FILENAME
    ఒకవేళ (p[0] != NUL && p[1] == ':')	    // skip "x:"
	p += 2;
# endif

    ఒకవేళ (vim_ispathsep(*p))
    {
	relative = FALSE;
	చేయి
	    ++p;
	అయితే (vim_ispathsep(*p));
    }
    ప్రారంభం = p;	    // remember start after "c:/" or "/" or "///"
    p_చివర = p + STRLEN(p);
# ifdef UNIX
    // Posix says that "//path" is unchanged but "///path" is "/path".
    ఒకవేళ (ప్రారంభం > filename + 2)
    {
	mch_memmove(filename + 1, p, (size_t)(p_చివర - p) + 1);	    // +1 for NUL
	p_చివర -= (size_t)(p - (filename + 1));
	ప్రారంభం = p = filename + 1;
    }
# endif

    చేయి
    {
	// At this point "p" is pointing to the char following a single "/"
	// or "p" is at the "start" of the (absolute or relative) path name.
# ifdef VMS
	// VMS allows device:[path] - don't strip the [ in directory
	ఒకవేళ ((*p == '[' || *p == '<') && p > filename && p[-1] == ':')
	{
	    // :[ or :< composition: vms directory component
	    ++components;
	    p = getnextcomp(p + 1);
	}
	// allow remote calls as host"user passwd"::device:[path]
	లేకపోతే ఒకవేళ (p[0] == ':' && p[1] == ':' && p > filename && p[-1] == '"' )
	{
	    // ":: composition: vms host/passwd component
	    ++components;
	    p = getnextcomp(p + 2);
	}
	లేకపోతే
# endif
	ఒకవేళ (vim_ispathsep(*p))
	{
	    mch_memmove(p, p + 1, (size_t)(p_చివర - (p + 1)) + 1); // remove duplicate "/"
	    --p_చివర;
	}
	లేకపోతే ఒకవేళ (p[0] == '.' && (vim_ispathsep(p[1]) || p[1] == NUL))
	{
	    ఒకవేళ (p == ప్రారంభం && relative)
		p += 1 + (p[1] != NUL);	// keep single "." or leading "./"
	    లేకపోతే
	    {
		// Strip "./" or ".///".  If we are at the end of the file name
		// and there is no trailing path separator, either strip "/." if
		// we are after "start", or strip "." if we are at the beginning
		// of an absolute path name .
		తోక = p + 1;
		ఒకవేళ (p[1] != NUL)
		    అయితే (vim_ispathsep(*తోక))
			MB_PTR_ADV(తోక);
		లేకపోతే ఒకవేళ (p > ప్రారంభం)
		    --p;		// strip preceding path separator

		mch_memmove(p, తోక, (size_t)(p_చివర - తోక) + 1);
		p_చివర -= (size_t)(తోక - p);
	    }
	}
	లేకపోతే ఒకవేళ (p[0] == '.' && p[1] == '.' &&
	    (vim_ispathsep(p[2]) || p[2] == NUL))
	{
	    // Skip to after ".." or "../" or "..///".
	    తోక = p + 2;
	    అయితే (vim_ispathsep(*తోక))
		MB_PTR_ADV(తోక);

	    ఒకవేళ (components > 0)		// strip one preceding component
	    {
		పూర్ణాంకం		do_strip = FALSE;
		char_u		saved_char;
		stat_T		st;

		// Don't strip for an erroneous file name.
		ఒకవేళ (!stripping_disabled)
		{
		    // If the preceding component does not exist in the file
		    // system, we strip it.  On Unix, we don't accept a symbolic
		    // link that refers to a non-existent file.
		    saved_char = p[-1];
		    p[-1] = NUL;
# ifdef UNIX
		    ఒకవేళ (mch_lstat((అక్షరం *)filename, &st) < 0)
# లేకపోతే
			ఒకవేళ (mch_stat((అక్షరం *)filename, &st) < 0)
# endif
			    do_strip = TRUE;
		    p[-1] = saved_char;

		    --p;
		    // Skip back to after previous '/'.
		    అయితే (p > ప్రారంభం && !after_pathsep(ప్రారంభం, p))
			MB_PTR_వెనుక(ప్రారంభం, p);

		    ఒకవేళ (!do_strip)
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
			saved_char = *తోక;
			*తోక = NUL;
			ఒకవేళ (mch_stat((అక్షరం *)filename, &st) >= 0)
			    do_strip = TRUE;
			లేకపోతే
			    stripping_disabled = TRUE;
			*తోక = saved_char;
# ifdef UNIX
			ఒకవేళ (do_strip)
			{
			    stat_T	new_st;

			    // On Unix, the check for the unstripped file name
			    // above works also for a symbolic link pointing to
			    // a searchable directory.  But then the parent of
			    // the directory pointed to by the link must be the
			    // same as the stripped file name.  (The latter
			    // exists in the file system since it is the
			    // component's parent directory.)
			    ఒకవేళ (p == ప్రారంభం && relative)
				(శూన్య)mch_stat(".", &new_st);
			    లేకపోతే
			    {
				saved_char = *p;
				*p = NUL;
				(శూన్య)mch_stat((అక్షరం *)filename, &new_st);
				*p = saved_char;
			    }

			    ఒకవేళ (new_st.st_ino != st.st_ino ||
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

		ఒకవేళ (!do_strip)
		{
		    // Skip the ".." or "../" and reset the counter for the
		    // components that might be stripped later on.
		    p = తోక;
		    components = 0;
		}
		లేకపోతే
		{
		    // Strip previous component.  If the result would get empty
		    // and there is no trailing path separator, leave a single
		    // "." instead.  If we are at the end of the file name and
		    // there is no trailing path separator and a preceding
		    // component is left after stripping, strip its trailing
		    // path separator as well.
		    ఒకవేళ (p == ప్రారంభం && relative && తోక[-1] == '.')
		    {
			*p++ = '.';
			*p = NUL;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (p > ప్రారంభం && తోక[-1] == '.')
			    --p;

			mch_memmove(p, తోక, (size_t)(p_చివర - తోక) + 1);	// strip previous component
			p_చివర -= (size_t)(తోక - p);
		    }

		    --components;
		}
	    }
	    లేకపోతే ఒకవేళ (p == ప్రారంభం && !relative)	// leading "/.." or "/../"
	    {
		mch_memmove(p, తోక, (size_t)(p_చివర - తోక) + 1);		// strip ".." or "../"
		p_చివర -= (size_t)(తోక - p);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (p == ప్రారంభం + 2 && p[-2] == '.')	// leading "./../"
		{
		    mch_memmove(p - 2, p, (size_t)(p_చివర - p) + 1); // strip leading "./"
		    p_చివర -= 2;
		    తోక -= 2;
		}
		p = తోక;		// skip to char after ".." or "../"
	    }
	}
	లేకపోతే
	{
	    ++components;		// simple path component
	    p = getnextcomp(p);
	}
    } అయితే (*p != NUL);

    తిరిగి (size_t)(p_చివర - filename);
#లేకపోతే
    // Don't touch Amiga filenames
    తిరిగి STRLEN(filename);
#అయితే_ముగింపు // !AMIGA
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * "simplify()" function
 */
    శూన్య
f_simplify(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    p = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_string = vim_strsave(p);
    simplify_filename(rettv->vval.v_string);	// simplify in place
    rettv->v_type = VAR_STRING;
}
#అయితే_ముగింపు // FEAT_EVAL
