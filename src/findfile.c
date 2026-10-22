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

#include "vim.h"

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
typedef sa_nracanaa ff_stack
{
    sa_nracanaa ff_stack	*ffs_prev;

    // the fix part (no wildcards) and the part containing the wildcards
    // of the search path
    string_T		ffs_fix_path;
    string_T		ffs_wc_path;

    // files/dirs found in the above directory, matched by the first wildcard
    // of wc_part
    char_u		**ffs_filearray;
    puur_naa_nka			ffs_filearray_size;
    puur_naa_nka			ffs_filearray_cur;   // needed for partly handled dirs

    // to store status of partly handled directories
    // 0: we work on this directory for the first time
    // 1: this directory was partly searched in an earlier step
    puur_naa_nka			ffs_stage;

    // How deep are we in the directory tree?
    // Counts backward from value of level parameter to vim_findfile_init
    puur_naa_nka			ffs_level;

    // Did we already expand '**' to an empty string?
    puur_naa_nka			ffs_star_star_empty;
} ff_stack_T;

/*
 * type for already visited directories or files.
 */
typedef sa_nracanaa ff_visited
{
    sa_nracanaa ff_visited	*ffv_next;

    // Visited directories are different if the wildcard string are
    // different. So we have to save it.
    char_u		*ffv_wc_path;

    // for unix use inode etc for comparison (needed because of links), else
    // use filename.
#ifdef UNIX
    puur_naa_nka			ffv_dev_valid;	// ffv_dev and ffv_ino were set
    dev_t		ffv_dev;	// device number
    ino_t		ffv_ino;	// inode number
#endif
    // The memory for this struct is allocated according to the length of
    // ffv_fname.
    char_u		ffv_fname[1];	// actually longer
} ff_visited_T;

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
typedef sa_nracanaa ff_visited_list_hdr
{
    sa_nracanaa ff_visited_list_hdr	*ffvl_next;

    // the filename the attached visited list is for
    char_u			*ffvl_filename;

    ff_visited_T		*ffvl_visited_list;

} ff_visited_list_hdr_T;


/*
 * '**' can be expanded to several directory levels.
 * Set the default maximum depth.
 */
#define FF_MAX_STAR_STAR_EXPAND ((char_u)30)

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
typedef sa_nracanaa ff_search_ctx_T
{
    ff_stack_T			*ffsc_stack_ptr;
    ff_visited_list_hdr_T	*ffsc_visited_list;
    ff_visited_list_hdr_T	*ffsc_dir_visited_list;
    ff_visited_list_hdr_T	*ffsc_visited_lists_list;
    ff_visited_list_hdr_T	*ffsc_dir_visited_lists_list;
    string_T			ffsc_file_to_search;
    string_T			ffsc_start_dir;
    string_T			ffsc_fix_path;
    string_T			ffsc_wc_path;
    puur_naa_nka				ffsc_level;
    string_T			*ffsc_stopdirs_v;
    puur_naa_nka				ffsc_find_what;
    puur_naa_nka				ffsc_tagfile;
} ff_search_ctx_T;

// locally needed functions
static puur_naa_nka ff_check_visited(ff_visited_T **, char_u *, size_t, char_u *, size_t);
static void vim_findfile_free_visited(void *search_ctx_arg);
static void vim_findfile_free_visited_list(ff_visited_list_hdr_T **list_headp);
static void ff_free_visited_list(ff_visited_T *vl);
static ff_visited_list_hdr_T* ff_get_visited_list(char_u *, size_t, ff_visited_list_hdr_T **list_headp);

static void ff_push(ff_search_ctx_T *search_ctx, ff_stack_T *stack_ptr);
static ff_stack_T *ff_pop(ff_search_ctx_T *search_ctx);
static void ff_clear(ff_search_ctx_T *search_ctx);
static void ff_free_stack_element(ff_stack_T *stack_ptr);
static ff_stack_T *ff_create_stack_element(char_u *, size_t, char_u *, size_t, puur_naa_nka, puur_naa_nka);
static puur_naa_nka ff_path_in_stoplist(char_u *, puur_naa_nka, string_T *);

static string_T ff_expand_buffer = {NULL, 0};	    // used for expanding filenames

#ပေႃး 0
/*
 * if someone likes findfirst/findnext, here are the functions
 * NOT TESTED!!
 */

static void *ff_fn_search_context = NULL;

    char_u *
vim_findfirst(char_u *path, char_u *filename, puur_naa_nka level)
{
    ff_fn_search_context =
	vim_findfile_init(path, filename, NULL, level, TRUE, FALSE,
		ff_fn_search_context, rel_fname);
    ပေႃး (NULL == ff_fn_search_context)
	ပွၵ်ႈ NULL;
    ပေႃးဢမ်ႇ
	ပွၵ်ႈ vim_findnext()
}

    char_u *
vim_findnext(void)
{
    char_u *ret = vim_findfile(ff_fn_search_context);

    ပေႃး (NULL == ret)
    {
	vim_findfile_cleanup(ff_fn_search_context);
	ff_fn_search_context = NULL;
    }
    ပွၵ်ႈ ret;
}
#endif

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
    void *
vim_findfile_init(
    char_u	*path,
    char_u	*filename,
    size_t	filenamelen,
    char_u	*stopdirs UNUSED,
    puur_naa_nka		level,
    puur_naa_nka		free_visited,
    puur_naa_nka		find_what,
    void	*search_ctx_arg,
    puur_naa_nka		tagfile,	// expanding names of tags files
    char_u	*rel_fname)	// file name to use for "."
{
    char_u		*wc_part;
    ff_stack_T		*sptr;
    ff_search_ctx_T	*search_ctx;
    puur_naa_nka			add_sep;

    // If a search context is given by the caller, reuse it, else allocate a
    // new one.
    ပေႃး (search_ctx_arg != NULL)
	search_ctx = search_ctx_arg;
    ပေႃးဢမ်ႇ
    {
	search_ctx = ALLOC_CLEAR_ONE(ff_search_ctx_T);
	ပေႃး (search_ctx == NULL)
	    goto error_return;
    }
    search_ctx->ffsc_find_what = find_what;
    search_ctx->ffsc_tagfile = tagfile;

    // clear the search context, but NOT the visited lists
    ff_clear(search_ctx);

    // clear visited list if wanted
    ပေႃး (free_visited == TRUE)
	vim_findfile_free_visited(search_ctx);
    ပေႃးဢမ်ႇ
    {
	// Reuse old visited lists. Get the visited list for the given
	// filename. If no list for the current filename exists, creates a new
	// one.
	search_ctx->ffsc_visited_list = ff_get_visited_list(filename,
					filenamelen, &search_ctx->ffsc_visited_lists_list);
	ပေႃး (search_ctx->ffsc_visited_list == NULL)
	    goto error_return;
	search_ctx->ffsc_dir_visited_list = ff_get_visited_list(filename,
				    filenamelen, &search_ctx->ffsc_dir_visited_lists_list);
	ပေႃး (search_ctx->ffsc_dir_visited_list == NULL)
	    goto error_return;
    }

    ပေႃး (ff_expand_buffer.string == NULL)
    {
	ff_expand_buffer.length = 0;
	ff_expand_buffer.string = alloc(MAXPATHL);
	ပေႃး (ff_expand_buffer.string == NULL)
	    goto error_return;
    }

    // Store information on starting dir now if path is relative.
    // If path is absolute, we do that later.
    ပေႃး (path[0] == '.'
	    && (vim_ispathsep(path[1]) || path[1] == NUL)
	    && (!tagfile || vim_strchr(p_cpo, CPO_DOTTAG) == NULL)
	    && rel_fname != NULL)
    {
	puur_naa_nka	len = (puur_naa_nka)(gettail(rel_fname) - rel_fname);

	ပေႃး (!vim_isAbsName(rel_fname) && len + 1 < MAXPATHL)
	{
	    // Make the start dir an absolute path name.
	    vim_strncpy(ff_expand_buffer.string, rel_fname, len);
	    ff_expand_buffer.length = len;

	    search_ctx->ffsc_start_dir.string = FullName_save(ff_expand_buffer.string, FALSE);
	    ပေႃး (search_ctx->ffsc_start_dir.string == NULL)
		goto error_return;
	    search_ctx->ffsc_start_dir.length = STRLEN(search_ctx->ffsc_start_dir.string);
	}
	ပေႃးဢမ်ႇ
	{
	    search_ctx->ffsc_start_dir.length = len;
	    search_ctx->ffsc_start_dir.string = vim_strnsave(rel_fname,
		search_ctx->ffsc_start_dir.length);
	    ပေႃး (search_ctx->ffsc_start_dir.string == NULL)
		goto error_return;
	}

	ပေႃး (*++path != NUL)
	    ++path;
    }
    ပေႃးဢမ်ႇ ပေႃး (*path == NUL || !vim_isAbsName(path))
    {
#ifdef BACKSLASH_IN_FILENAME
	// "c:dir" needs "c:" to be expanded, otherwise use current dir
	ပေႃး (*path != NUL && path[1] == ':')
	{
	    char_u  drive[3];

	    drive[0] = path[0];
	    drive[1] = ':';
	    drive[2] = NUL;
	    ပေႃး (vim_FullName(drive, ff_expand_buffer.string, MAXPATHL, TRUE) == FAIL)
		goto error_return;
	    path += 2;
	}
	ပေႃးဢမ်ႇ
#endif
	ပေႃး (mch_dirname(ff_expand_buffer.string, MAXPATHL) == FAIL)
	    goto error_return;

	ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);

	search_ctx->ffsc_start_dir.length = ff_expand_buffer.length;
	search_ctx->ffsc_start_dir.string = vim_strnsave(ff_expand_buffer.string,
	    search_ctx->ffsc_start_dir.length);
	ပေႃး (search_ctx->ffsc_start_dir.string == NULL)
	    goto error_return;
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
    ပေႃး (stopdirs != NULL)
    {
	char_u	*walker = stopdirs;
	puur_naa_nka	dircount;

	ၶၢဝ်း (*walker == ';')
	    walker++;

	dircount = 1;
	search_ctx->ffsc_stopdirs_v = ALLOC_ONE(string_T);

	ပေႃး (search_ctx->ffsc_stopdirs_v != NULL)
	{
	    string_T	*tmp;			// for convenience

	    do
	    {
		char_u	*helper;
		void	*ptr;
		size_t	len;

		helper = walker;
		ptr = vim_realloc(search_ctx->ffsc_stopdirs_v,
					   (dircount + 1) * parimaa_na(string_T));
		ပေႃး (ptr)
		    search_ctx->ffsc_stopdirs_v = ptr;
		ပေႃးဢမ်ႇ
		    // ignore, keep what we have and continue
		    bha_nga;
		walker = vim_strchr(walker, ';');
		len = walker ? (size_t)(walker - helper) : STRLEN(helper);
		// "" means ascent till top of directory tree.

		ပေႃး (*helper != NUL && !vim_isAbsName(helper)
							 && len + 1 < MAXPATHL)
		{
		    // Make the stop dir an absolute path name.
		    vim_strncpy(ff_expand_buffer.string, helper, len);
		    ff_expand_buffer.length = len;

		    tmp = &search_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->string = FullName_save(ff_expand_buffer.string, FALSE);
		    ပေႃး (tmp->string != NULL)
			tmp->length = STRLEN(tmp->string);
		}
		ပေႃးဢမ်ႇ
		{
		    tmp = &search_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->length = len;
		    tmp->string = vim_strnsave(helper, tmp->length);
		    ပေႃး (tmp->string == NULL)
			tmp->length = 0;
		}
		ပေႃး (walker)
		    walker++;
		dircount++;

	    } ၶၢဝ်း (walker != NULL);

	    tmp = &search_ctx->ffsc_stopdirs_v[dircount - 1];
	    tmp->string = NULL;
	    tmp->length = 0;
	}
    }

    search_ctx->ffsc_level = level;

    /*
     * split into:
     *  -fix path
     *  -wildcard_stuff (might be NULL)
     */
    wc_part = vim_strchr(path, '*');
    ပေႃး (wc_part != NULL)
    {
	puur_naa_nka	llevel;
	var_na	*errpt;

	// save the fix part of the path
	search_ctx->ffsc_fix_path.length = (size_t)(wc_part - path);
	search_ctx->ffsc_fix_path.string = vim_strnsave(path,
	    search_ctx->ffsc_fix_path.length);
	ပေႃး (search_ctx->ffsc_fix_path.string == NULL)
	    goto error_return;

	/*
	 * copy wc_path and add restricts to the '**' wildcard.
	 * The octet after a '**' is used as a (binary) counter.
	 * So '**3' is transposed to '**^C' ('^C' is ASCII value 3)
	 * or '**76' is transposed to '**N'( 'N' is ASCII value 76).
	 * If no restrict is given after '**' the default is used.
	 * Due to this technique the path looks awful if you print it as a
	 * string.
	 */
	ff_expand_buffer.length = 0;
	ၶၢဝ်း (*wc_part != NUL)
	{
	    ပေႃး (ff_expand_buffer.length + 5 >= MAXPATHL)
	    {
		emsg(_(e_path_too_long_for_completion));
		bha_nga;
	    }
	    ပေႃး (STRNCMP(wc_part, "**", 2) == 0)
	    {
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;

		llevel = strtol((var_na *)wc_part, &errpt, 10);
		ပေႃး ((char_u *)errpt != wc_part && llevel > 0 && llevel < 255)
		    ff_expand_buffer.string[ff_expand_buffer.length++] = llevel;
		ပေႃးဢမ်ႇ ပေႃး ((char_u *)errpt != wc_part && llevel == 0)
		    // restrict is 0 -> remove already added '**'
		    ff_expand_buffer.length -= 2;
		ပေႃးဢမ်ႇ
		    ff_expand_buffer.string[ff_expand_buffer.length++] = FF_MAX_STAR_STAR_EXPAND;
		wc_part = (char_u *)errpt;
		ပေႃး (*wc_part != NUL && !vim_ispathsep(*wc_part))
		{
		    semsg(_(e_invalid_path_number_must_be_at_end_of_path_or_be_followed_by_str), PATHSEPSTR);
		    goto error_return;
		}
	    }
	    ပေႃးဢမ်ႇ
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;
	}
	ff_expand_buffer.string[ff_expand_buffer.length] = NUL;

	search_ctx->ffsc_wc_path.length = ff_expand_buffer.length;
	search_ctx->ffsc_wc_path.string = vim_strnsave(ff_expand_buffer.string,
	    search_ctx->ffsc_wc_path.length);
	ပေႃး (search_ctx->ffsc_wc_path.string == NULL)
	    goto error_return;
    }
    ပေႃးဢမ်ႇ
    {
	search_ctx->ffsc_fix_path.length = STRLEN(path);
	search_ctx->ffsc_fix_path.string = vim_strnsave(path,
	    search_ctx->ffsc_fix_path.length);
	ပေႃး (search_ctx->ffsc_fix_path.string == NULL)
	    goto error_return;
    }

    ပေႃး (search_ctx->ffsc_start_dir.string == NULL)
    {
	// store the fix part as startdir.
	// This is needed if the parameter path is fully qualified.
	search_ctx->ffsc_start_dir.length = search_ctx->ffsc_fix_path.length;
	search_ctx->ffsc_start_dir.string = vim_strnsave(search_ctx->ffsc_fix_path.string,
	    search_ctx->ffsc_start_dir.length);
	ပေႃး (search_ctx->ffsc_start_dir.string == NULL)
	    goto error_return;
	search_ctx->ffsc_fix_path.string[0] = NUL;
	search_ctx->ffsc_fix_path.length = 0;
    }

    // create an absolute path
    ပေႃး (search_ctx->ffsc_start_dir.length
	    + search_ctx->ffsc_fix_path.length + 3 >= MAXPATHL)
    {
	emsg(_(e_path_too_long_for_completion));
	goto error_return;
    }

    add_sep = !after_pathsep(search_ctx->ffsc_start_dir.string,
	search_ctx->ffsc_start_dir.string + search_ctx->ffsc_start_dir.length);
    ff_expand_buffer.length = vim_snprintf(
	    (var_na *)ff_expand_buffer.string,
	    MAXPATHL,
	    "%s%s",
	    search_ctx->ffsc_start_dir.string,
	    add_sep ? PATHSEPSTR : "");

    {
	size_t	bufsize = ff_expand_buffer.length + search_ctx->ffsc_fix_path.length + 1;
	char_u	*buf = alloc(bufsize);

	ပေႃး (buf == NULL)
	    goto error_return;

	vim_snprintf(
		(var_na *)buf,
		bufsize,
		"%s%s",
		ff_expand_buffer.string,
		search_ctx->ffsc_fix_path.string);
	ပေႃး (mch_isdir(buf))
	{
	    ပေႃး (search_ctx->ffsc_fix_path.length > 0)
	    {
		add_sep = !after_pathsep(search_ctx->ffsc_fix_path.string,
		    search_ctx->ffsc_fix_path.string + search_ctx->ffsc_fix_path.length);
		ff_expand_buffer.length += vim_snprintf(
			(var_na *)ff_expand_buffer.string + ff_expand_buffer.length,
			MAXPATHL - ff_expand_buffer.length,
			"%s%s",
			search_ctx->ffsc_fix_path.string,
			add_sep ? PATHSEPSTR : "");
	    }
	}
	ပေႃးဢမ်ႇ
	{
	    char_u *p = gettail(search_ctx->ffsc_fix_path.string);
	    puur_naa_nka    len = (puur_naa_nka)search_ctx->ffsc_fix_path.length;

	    ပေႃး (p > search_ctx->ffsc_fix_path.string)
	    {
		// do not add '..' to the path and start upwards searching
		len = (puur_naa_nka)(p - search_ctx->ffsc_fix_path.string) - 1;
		ပေႃး ((len >= 2
			&& STRNCMP(search_ctx->ffsc_fix_path.string, "..", 2) == 0)
			&& (len == 2 || search_ctx->ffsc_fix_path.string[2] == PATHSEP))
		{
		    vim_free(buf);
		    goto error_return;
		}

		add_sep = !after_pathsep(search_ctx->ffsc_fix_path.string,
		    search_ctx->ffsc_fix_path.string + search_ctx->ffsc_fix_path.length);
		ff_expand_buffer.length += vim_snprintf(
			(var_na *)ff_expand_buffer.string + ff_expand_buffer.length,
			MAXPATHL - ff_expand_buffer.length,
			"%.*s%s",
			len,
			search_ctx->ffsc_fix_path.string,
			add_sep ? PATHSEPSTR : "");
	    }

	    ပေႃး (search_ctx->ffsc_wc_path.string != NULL)
	    {
		size_t	tempsize = (search_ctx->ffsc_fix_path.length - len)
				+ search_ctx->ffsc_wc_path.length
				+ 1;
		char_u	*temp = alloc(tempsize);

		ပေႃး (temp == NULL)
		{
		    vim_free(buf);
		    vim_free(temp);
		    goto error_return;
		}

		search_ctx->ffsc_wc_path.length = vim_snprintf(
			    (var_na *)temp,
			    tempsize,
			    "%s%s",
			    search_ctx->ffsc_fix_path.string + len,
			    search_ctx->ffsc_wc_path.string);
		vim_free(search_ctx->ffsc_wc_path.string);
		search_ctx->ffsc_wc_path.string = temp;
	    }
	}
	vim_free(buf);
    }

    sptr = ff_create_stack_element(ff_expand_buffer.string,
			ff_expand_buffer.length,
			search_ctx->ffsc_wc_path.string,
			search_ctx->ffsc_wc_path.length,
			level,
			0);

    ပေႃး (sptr == NULL)
	goto error_return;

    ff_push(search_ctx, sptr);

    search_ctx->ffsc_file_to_search.length = filenamelen;
    search_ctx->ffsc_file_to_search.string = vim_strnsave(filename,
	search_ctx->ffsc_file_to_search.length);
    ပေႃး (search_ctx->ffsc_file_to_search.string == NULL)
	goto error_return;

    ပွၵ်ႈ search_ctx;

error_return:
    /*
     * We clear the search context now!
     * Even when the caller gave us a (perhaps valid) context we free it here,
     * as we might have already destroyed it.
     */
    vim_findfile_cleanup(search_ctx);
    ပွၵ်ႈ NULL;
}

/*
 * Get the stopdir string.  Check that ';' is not escaped.
 */
    char_u *
vim_findfile_stopdir(char_u *buf)
{
    char_u	*r_ptr = buf;
    char_u	*r_ptr_end = NULL;	    // points to NUL at end of string "r_ptr"

    ၶၢဝ်း (*r_ptr != NUL && *r_ptr != ';')
    {
	ပေႃး (r_ptr[0] == '\\' && r_ptr[1] == ';')
	{
	    // Overwrite the escape char,
	    // use STRLEN(r_ptr) to move the trailing '\0'.
	    ပေႃး (r_ptr_end == NULL)
		r_ptr_end = r_ptr + STRLEN(r_ptr);
	    mch_memmove(r_ptr, r_ptr + 1,
		(size_t)(r_ptr_end - (r_ptr + 1)) + 1);	// +1 for NUL
	    r_ptr++;
	    --r_ptr_end;
	}
	r_ptr++;
    }
    ပေႃး (*r_ptr == ';')
    {
	*r_ptr = NUL;
	r_ptr++;
    }
    ပေႃးဢမ်ႇ ပေႃး (*r_ptr == NUL)
	r_ptr = NULL;
    ပွၵ်ႈ r_ptr;
}

/*
 * Clean up the given search context. Can handle a NULL pointer.
 */
    void
vim_findfile_cleanup(void *ctx)
{
    ပေႃး (ctx == NULL)
	ပွၵ်ႈ;

    vim_findfile_free_visited(ctx);
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
vim_findfile(void *search_ctx_arg)
{
    string_T	file_path;
    string_T	rest_of_wildcards;
    char_u	*path_end = NULL;
    ff_stack_T	*stackp;
    ff_search_ctx_T *search_ctx;

    ပေႃး (search_ctx_arg == NULL)
	ပွၵ်ႈ NULL;

    search_ctx = (ff_search_ctx_T *)search_ctx_arg;

    /*
     * filepath is used as buffer for various actions and as the storage to
     * return a found filename.
     */
    ပေႃး ((file_path.string = alloc(MAXPATHL)) == NULL)
	ပွၵ်ႈ NULL;

    // store the end of the start dir -- needed for upward search
    ပေႃး (search_ctx->ffsc_start_dir.string != NULL)
	path_end = &search_ctx->ffsc_start_dir.string[
					    search_ctx->ffsc_start_dir.length];

    // upward search loop
    for (;;)
    {
	// downward search loop
	for (;;)
	{
	    // check if user wants to stop the search
	    ui_breakcheck();
	    ပေႃး (got_int)
		bha_nga;

	    // get directory to work on from stack
	    stackp = ff_pop(search_ctx);
	    ပေႃး (stackp == NULL)
		bha_nga;

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
	    ပေႃး (stackp->ffs_filearray == NULL
		    && ff_check_visited(&search_ctx->ffsc_dir_visited_list
							  ->ffvl_visited_list,
			    stackp->ffs_fix_path.string,
			    stackp->ffs_fix_path.length,
			    stackp->ffs_wc_path.string,
			    stackp->ffs_wc_path.length) == FAIL)
	    {
#ifdef FF_VERBOSE
		ပေႃး (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("Already Searched: %s (%s)",
				   stackp->ffs_fix_path.string, stackp->ffs_wc_path.string);
		    // don't overwrite this either
		    msg_puts("\n");
		    verbose_leave_scroll();
		}
#endif
		ff_free_stack_element(stackp);
		continue;
	    }
#ifdef FF_VERBOSE
	    ပေႃးဢမ်ႇ ပေႃး (p_verbose >= 5)
	    {
		verbose_enter_scroll();
		smsg("Searching: %s (%s)",
				   stackp->ffs_fix_path.string, stackp->ffs_wc_path.string);
		// don't overwrite this either
		msg_puts("\n");
		verbose_leave_scroll();
	    }
#endif

	    // check depth
	    ပေႃး (stackp->ffs_level <= 0)
	    {
		ff_free_stack_element(stackp);
		continue;
	    }

	    file_path.string[0] = NUL;
	    file_path.length = 0;

	    /*
	     * If no filearray till now expand wildcards
	     * The function expand_wildcards() can handle an array of paths
	     * and all possible expands are returned in one array. We use this
	     * to handle the expansion of '**' into an empty string.
	     */
	    ပေႃး (stackp->ffs_filearray == NULL)
	    {
		char_u *dirptrs[2];

		// we use filepath to build the path expand_wildcards() should
		// expand.
		dirptrs[0] = file_path.string;
		dirptrs[1] = NULL;

		// if we have a start dir copy it in
		ပေႃး (!vim_isAbsName(stackp->ffs_fix_path.string)
						&& search_ctx->ffsc_start_dir.string)
		{
		    ပေႃး (search_ctx->ffsc_start_dir.length + 1 < MAXPATHL)
		    {
			puur_naa_nka add_sep = !after_pathsep(search_ctx->ffsc_start_dir.string,
			    search_ctx->ffsc_start_dir.string + search_ctx->ffsc_start_dir.length);
			file_path.length = vim_snprintf(
			    (var_na *)file_path.string,
			    MAXPATHL,
			    "%s%s",
			    search_ctx->ffsc_start_dir.string,
			    add_sep ? PATHSEPSTR : "");
		    }
		    ပေႃးဢမ်ႇ
		    {
			ff_free_stack_element(stackp);
			goto fail;
		    }
		}

		// append the fix part of the search path
		ပေႃး (file_path.length + stackp->ffs_fix_path.length + 1 < MAXPATHL)
		{
		    puur_naa_nka add_sep = !after_pathsep(stackp->ffs_fix_path.string,
			stackp->ffs_fix_path.string + stackp->ffs_fix_path.length);
		    file_path.length += vim_snprintf(
			(var_na *)file_path.string + file_path.length,
			MAXPATHL - file_path.length,
			"%s%s",
			stackp->ffs_fix_path.string,
			add_sep ? PATHSEPSTR : "");
		}
		ပေႃးဢမ်ႇ
		{
		    ff_free_stack_element(stackp);
		    goto fail;
		}

		rest_of_wildcards.string = stackp->ffs_wc_path.string;
		rest_of_wildcards.length = stackp->ffs_wc_path.length;
		ပေႃး (*rest_of_wildcards.string != NUL)
		{
		    ပေႃး (STRNCMP(rest_of_wildcards.string, "**", 2) == 0)
		    {
			char_u	*p;

			// pointer to the restrict byte
			// The restrict byte is not a character!
			p = rest_of_wildcards.string + 2;

			ပေႃး (*p > 0)
			{
			    (*p)--;
			    ပေႃး (file_path.length + 1 < MAXPATHL)
				file_path.string[file_path.length++] = '*';
			    ပေႃးဢမ်ႇ
			    {
				ff_free_stack_element(stackp);
				goto fail;
			    }
			}

			ပေႃး (*p == 0)
			{
			    // remove '**<numb> from wildcards
			    mch_memmove(rest_of_wildcards.string,
					rest_of_wildcards.string + 3,
					(size_t)(rest_of_wildcards.length - 3) + 1);    // +1 for NUL
			    rest_of_wildcards.length -= 3;
			    stackp->ffs_wc_path.length = rest_of_wildcards.length;
			}
			ပေႃးဢမ်ႇ
			{
			    rest_of_wildcards.string += 3;
			    rest_of_wildcards.length -= 3;
			}

			ပေႃး (stackp->ffs_star_star_empty == 0)
			{
			    // if not done before, expand '**' to empty
			    stackp->ffs_star_star_empty = 1;
			    dirptrs[1] = stackp->ffs_fix_path.string;
			}
		    }

		    /*
		     * Here we copy until the next path separator or the end of
		     * the path. If we stop at a path separator, there is
		     * still something else left. This is handled below by
		     * pushing every directory returned from expand_wildcards()
		     * on the stack again for further search.
		     */
		    ၶၢဝ်း (*rest_of_wildcards.string
			    && !vim_ispathsep(*rest_of_wildcards.string))
		    {
			ပေႃး (file_path.length + 1 < MAXPATHL)
			{
			    file_path.string[file_path.length++] = *rest_of_wildcards.string++;
			    --rest_of_wildcards.length;
			}
			ပေႃးဢမ်ႇ
			{
			    ff_free_stack_element(stackp);
			    goto fail;
			}
		    }

		    file_path.string[file_path.length] = NUL;
		    ပေႃး (vim_ispathsep(*rest_of_wildcards.string))
		    {
			rest_of_wildcards.string++;
			rest_of_wildcards.length--;
		    }
		}

		/*
		 * Expand wildcards like "*" and "$VAR".
		 * If the path is a URL don't try this.
		 */
		ပေႃး (path_with_url(dirptrs[0]))
		{
		    stackp->ffs_filearray = ALLOC_ONE(char_u *);
		    ပေႃး (stackp->ffs_filearray != NULL
			    && (stackp->ffs_filearray[0]
				= vim_strnsave(dirptrs[0], file_path.length)) != NULL)
			stackp->ffs_filearray_size = 1;
		    ပေႃးဢမ်ႇ
			stackp->ffs_filearray_size = 0;
		}
		ပေႃးဢမ်ႇ
		    // Add EW_NOTWILD because the expanded path may contain
		    // wildcard characters that are to be taken literally.
		    // This is a bit of a hack.
		    expand_wildcards((dirptrs[1] == NULL) ? 1 : 2, dirptrs,
			    &stackp->ffs_filearray_size,
			    &stackp->ffs_filearray,
			    EW_DIR|EW_ADDSLASH|EW_SILENT|EW_NOTWILD);

		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 0;
	    }
	    ပေႃးဢမ်ႇ
	    {
		rest_of_wildcards.string = &stackp->ffs_wc_path.string[
							stackp->ffs_wc_path.length];
		rest_of_wildcards.length = 0;
	    }

	    ပေႃး (stackp->ffs_stage == 0)
	    {
		puur_naa_nka i;

		// this is the first time we work on this directory
		ပေႃး (*rest_of_wildcards.string == NUL)
		{
		    size_t  len;
		    char_u  *suf;

		    /*
		     * We don't have further wildcards to expand, so we have to
		     * check for the final file now.
		     */
		    for (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		    {
			ပေႃး (!path_with_url(stackp->ffs_filearray[i])
				      && !mch_isdir(stackp->ffs_filearray[i]))
			    continue;   // not a directory

			// prepare the filename to be checked for existence
			// below
			len = STRLEN(stackp->ffs_filearray[i]);
			ပေႃး (len + 1 + search_ctx->ffsc_file_to_search.length
									< MAXPATHL)
			{
			    puur_naa_nka	add_sep = !after_pathsep(stackp->ffs_filearray[i],
				    stackp->ffs_filearray[i] + len);
			    file_path.length = vim_snprintf(
				    (var_na *)file_path.string,
				    MAXPATHL,
				    "%s%s%s",
				    stackp->ffs_filearray[i],
				    add_sep ? PATHSEPSTR : "",
				    search_ctx->ffsc_file_to_search.string);
			}
			ပေႃးဢမ်ႇ
			{
			    ff_free_stack_element(stackp);
			    goto fail;
			}

			/*
			 * Try without extra suffix and then with suffixes
			 * from 'suffixesadd'.
			 */
			len = file_path.length;
			ပေႃး (search_ctx->ffsc_tagfile)
			    suf = (char_u *)"";
			ပေႃးဢမ်ႇ
			    suf = curbuf->b_p_sua;
			for (;;)
			{
			    // if file exists and we didn't already find it
			    ပေႃး ((path_with_url(file_path.string)
				  || (mch_getperm(file_path.string) >= 0
				      && (search_ctx->ffsc_find_what
							      == FINDFILE_BOTH
					  || ((search_ctx->ffsc_find_what
							      == FINDFILE_DIR)
						   == mch_isdir(file_path.string)))))
#ifndef FF_VERBOSE
				    && (ff_check_visited(
					    &search_ctx->ffsc_visited_list
							   ->ffvl_visited_list,
					    file_path.string,
					    file_path.length,
					    (char_u *)"", 0) == OK)
#endif
			       )
			    {
#ifdef FF_VERBOSE
				ပေႃး (ff_check_visited(
					    &search_ctx->ffsc_visited_list
							   ->ffvl_visited_list,
					      file_path.string,
					      file_path.length,
					      (char_u *)"", 0) == FAIL)
				{
				    ပေႃး (p_verbose >= 5)
				    {
					verbose_enter_scroll();
					smsg("Already: %s", file_path.string);
					// don't overwrite this either
					msg_puts("\n");
					verbose_leave_scroll();
				    }
				    continue;
				}
#endif

				// push dir to examine rest of subdirs later
				stackp->ffs_filearray_cur = i + 1;
				ff_push(search_ctx, stackp);

				ပေႃး (!path_with_url(file_path.string))
				    file_path.length = simplify_filename(file_path.string);

				ပေႃး (mch_dirname(ff_expand_buffer.string, MAXPATHL)
									== OK)
				{
				    char_u  *p;

				    ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);
				    p = shorten_fname(file_path.string,
							    ff_expand_buffer.string);
				    ပေႃး (p != NULL)
				    {
					mch_memmove(file_path.string, p,
					    (size_t)((file_path.string + file_path.length) - p) + 1);  // +1 for NUL
					file_path.length -= (p - file_path.string);
				    }
				}
#ifdef FF_VERBOSE
				ပေႃး (p_verbose >= 5)
				{
				    verbose_enter_scroll();
				    smsg("HIT: %s", file_path.string);
				    // don't overwrite this either
				    msg_puts("\n");
				    verbose_leave_scroll();
				}
#endif
				ပွၵ်ႈ file_path.string;
			    }

			    // Not found or found already, try next suffix.
			    ပေႃး (*suf == NUL)
				bha_nga;
			    file_path.length = len + copy_option_part(&suf,
				  file_path.string + len,
				  (puur_naa_nka)(MAXPATHL - len), ",");
			}
		    }
		}
		ပေႃးဢမ်ႇ
		{
		    /*
		     * still wildcards left, push the directories for further
		     * search
		     */
		    for (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		    {
			ပေႃး (!mch_isdir(stackp->ffs_filearray[i]))
			    continue;	// not a directory

			ff_push(search_ctx,
				ff_create_stack_element(
						     stackp->ffs_filearray[i],
						     STRLEN(stackp->ffs_filearray[i]),
						     rest_of_wildcards.string,
						     rest_of_wildcards.length,
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
	    ပေႃး (STRNCMP(stackp->ffs_wc_path.string, "**", 2) == 0)
	    {
		puur_naa_nka i;

		for (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		{
		    ပေႃး (fnamecmp(stackp->ffs_filearray[i],
						   stackp->ffs_fix_path.string) == 0)
			continue; // don't repush same directory
		    ပေႃး (!mch_isdir(stackp->ffs_filearray[i]))
			continue;   // not a directory
		    ff_push(search_ctx,
			    ff_create_stack_element(
				stackp->ffs_filearray[i],
				STRLEN(stackp->ffs_filearray[i]),
				stackp->ffs_wc_path.string,
				stackp->ffs_wc_path.length,
				stackp->ffs_level - 1, 1));
		}
	    }

	    // we are done with the current directory
	    ff_free_stack_element(stackp);
	}

	// If we reached this, we didn't find anything downwards.
	// Let's check if we should do an upward search.
	ပေႃး (search_ctx->ffsc_start_dir.string
		&& search_ctx->ffsc_stopdirs_v != NULL && !got_int)
	{
	    ff_stack_T  *sptr;
	    // path_end may point to the NUL or the previous path separator
	    puur_naa_nka plen = (path_end - search_ctx->ffsc_start_dir.string)
							  + (*path_end != NUL);

	    // is the last starting directory in the stop list?
	    ပေႃး (ff_path_in_stoplist(search_ctx->ffsc_start_dir.string,
				    plen, search_ctx->ffsc_stopdirs_v) == TRUE)
		bha_nga;

	    // cut of last dir
	    ၶၢဝ်း (path_end > search_ctx->ffsc_start_dir.string
						  && vim_ispathsep(*path_end))
		path_end--;
	    ၶၢဝ်း (path_end > search_ctx->ffsc_start_dir.string
					      && !vim_ispathsep(path_end[-1]))
		path_end--;
	    *path_end = NUL;

	    // we may have shortened search_ctx->ffsc_start_dir, so update it's length
	    search_ctx->ffsc_start_dir.length = (size_t)(path_end - search_ctx->ffsc_start_dir.string);
	    path_end--;

	    ပေႃး (*search_ctx->ffsc_start_dir.string == NUL)
		bha_nga;

	    ပေႃး (search_ctx->ffsc_start_dir.length + 1
		    + search_ctx->ffsc_fix_path.length < MAXPATHL)
	    {
		puur_naa_nka add_sep = !after_pathsep(search_ctx->ffsc_start_dir.string,
			    search_ctx->ffsc_start_dir.string + search_ctx->ffsc_start_dir.length);
		file_path.length = vim_snprintf(
			(var_na *)file_path.string,
			MAXPATHL,
			"%s%s%s",
			search_ctx->ffsc_start_dir.string,
			add_sep ? PATHSEPSTR : "",
			search_ctx->ffsc_fix_path.string);
	    }
	    ပေႃးဢမ်ႇ
		goto fail;

	    // create a new stack entry
	    sptr = ff_create_stack_element(file_path.string, file_path.length,
		    search_ctx->ffsc_wc_path.string, search_ctx->ffsc_wc_path.length,
		    search_ctx->ffsc_level, 0);
	    ပေႃး (sptr == NULL)
		bha_nga;
	    ff_push(search_ctx, sptr);
	}
	ပေႃးဢမ်ႇ
	    bha_nga;
    }

fail:
    vim_free(file_path.string);
    ပွၵ်ႈ NULL;
}

/*
 * Free the list of lists of visited files and directories
 * Can handle it if the passed search_context is NULL;
 */
    static void
vim_findfile_free_visited(void *search_ctx_arg)
{
    ff_search_ctx_T *search_ctx;

    ပေႃး (search_ctx_arg == NULL)
	ပွၵ်ႈ;

    search_ctx = (ff_search_ctx_T *)search_ctx_arg;
    vim_findfile_free_visited_list(&search_ctx->ffsc_visited_lists_list);
    vim_findfile_free_visited_list(&search_ctx->ffsc_dir_visited_lists_list);
}

    static void
vim_findfile_free_visited_list(ff_visited_list_hdr_T **list_headp)
{
    ff_visited_list_hdr_T *vp;

    ၶၢဝ်း (*list_headp != NULL)
    {
	vp = (*list_headp)->ffvl_next;
	ff_free_visited_list((*list_headp)->ffvl_visited_list);

	vim_free((*list_headp)->ffvl_filename);
	vim_free(*list_headp);
	*list_headp = vp;
    }
    *list_headp = NULL;
}

    static void
ff_free_visited_list(ff_visited_T *vl)
{
    ff_visited_T *vp;

    ၶၢဝ်း (vl != NULL)
    {
	vp = vl->ffv_next;
	vim_free(vl->ffv_wc_path);
	vim_free(vl);
	vl = vp;
    }
    vl = NULL;
}

/*
 * Returns the already visited list for the given filename. If none is found it
 * allocates a new one.
 */
    static ff_visited_list_hdr_T*
ff_get_visited_list(
    char_u			*filename,
    size_t			filenamelen,
    ff_visited_list_hdr_T	**list_headp)
{
    ff_visited_list_hdr_T  *retptr = NULL;

    // check if a visited list for the given filename exists
    ပေႃး (*list_headp != NULL)
    {
	retptr = *list_headp;
	ၶၢဝ်း (retptr != NULL)
	{
	    ပေႃး (fnamecmp(filename, retptr->ffvl_filename) == 0)
	    {
#ifdef FF_VERBOSE
		ပေႃး (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("ff_get_visited_list: FOUND list for %s",
								    filename);
		    // don't overwrite this either
		    msg_puts("\n");
		    verbose_leave_scroll();
		}
#endif
		ပွၵ်ႈ retptr;
	    }
	    retptr = retptr->ffvl_next;
	}
    }

#ifdef FF_VERBOSE
    ပေႃး (p_verbose >= 5)
    {
	verbose_enter_scroll();
	smsg("ff_get_visited_list: new list for %s", filename);
	// don't overwrite this either
	msg_puts("\n");
	verbose_leave_scroll();
    }
#endif

    /*
     * if we reach this we didn't find a list and we have to allocate new list
     */
    retptr = ALLOC_ONE(ff_visited_list_hdr_T);
    ပေႃး (retptr == NULL)
	ပွၵ်ႈ NULL;

    retptr->ffvl_visited_list = NULL;
    retptr->ffvl_filename = vim_strnsave(filename, filenamelen);
    ပေႃး (retptr->ffvl_filename == NULL)
    {
	vim_free(retptr);
	ပွၵ်ႈ NULL;
    }
    retptr->ffvl_next = *list_headp;
    *list_headp = retptr;

    ပွၵ်ႈ retptr;
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
    static puur_naa_nka
ff_wc_equal(char_u *s1, char_u *s2)
{
    puur_naa_nka		i, j;
    puur_naa_nka		c1 = NUL;
    puur_naa_nka		c2 = NUL;
    puur_naa_nka		prev1 = NUL;
    puur_naa_nka		prev2 = NUL;

    ပေႃး (s1 == s2)
	ပွၵ်ႈ TRUE;

    ပေႃး (s1 == NULL || s2 == NULL)
	ပွၵ်ႈ FALSE;

    for (i = 0, j = 0; s1[i] != NUL && s2[j] != NUL;)
    {
	c1 = PTR2CHAR(s1 + i);
	c2 = PTR2CHAR(s2 + j);

	ပေႃး ((p_fic ? MB_TOLOWER(c1) != MB_TOLOWER(c2) : c1 != c2)
		&& (prev1 != '*' || prev2 != '*'))
	    ပွၵ်ႈ FALSE;
	prev2 = prev1;
	prev1 = c1;

	i += mb_ptr2len(s1 + i);
	j += mb_ptr2len(s2 + j);
    }
    ပွၵ်ႈ s1[i] == s2[j];
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
    static puur_naa_nka
ff_check_visited(
    ff_visited_T	**visited_list,
    char_u		*fname,
    size_t		fnamelen,
    char_u		*wc_path,
    size_t		wc_pathlen)
{
    ff_visited_T	*vp;
#ifdef UNIX
    stat_T		st;
    puur_naa_nka			url = FALSE;
#endif

    // For a URL we only compare the name, otherwise we compare the
    // device/inode (unix) or the full path name (not Unix).
    ပေႃး (path_with_url(fname))
    {
	vim_strncpy(ff_expand_buffer.string, fname, fnamelen);
	ff_expand_buffer.length = fnamelen;
#ifdef UNIX
	url = TRUE;
#endif
    }
    ပေႃးဢမ်ႇ
    {
	ff_expand_buffer.string[0] = NUL;
	ff_expand_buffer.length = 0;
#ifdef UNIX
	ပေႃး (mch_stat((var_na *)fname, &st) < 0)
	    ပွၵ်ႈ FAIL;
#ပေႃးဢမ်ႇ
	ပေႃး (vim_FullName(fname, ff_expand_buffer.string, MAXPATHL, TRUE) == FAIL)
	    ပွၵ်ႈ FAIL;
	ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);
#endif
    }

    // check against list of already visited files
    for (vp = *visited_list; vp != NULL; vp = vp->ffv_next)
    {
	ပေႃး (
#ifdef UNIX
		!url ? (vp->ffv_dev_valid && vp->ffv_dev == st.st_dev
						  && vp->ffv_ino == st.st_ino)
		     :
#endif
		fnamecmp(vp->ffv_fname, ff_expand_buffer.string) == 0
	   )
	{
	    // are the wildcard parts equal
	    ပေႃး (ff_wc_equal(vp->ffv_wc_path, wc_path) == TRUE)
		// already visited
		ပွၵ်ႈ FAIL;
	}
    }

    /*
     * New file/dir.  Add it to the list of visited files/dirs.
     */
    vp = alloc(
	     offsetof(ff_visited_T, ffv_fname) + ff_expand_buffer.length + 1);
    ပေႃး (vp == NULL)
	ပွၵ်ႈ OK;

#ifdef UNIX
    ပေႃး (!url)
    {
	vp->ffv_dev_valid = TRUE;
	vp->ffv_dev = st.st_dev;
	vp->ffv_ino = st.st_ino;
	vp->ffv_fname[0] = NUL;
    }
    ပေႃးဢမ်ႇ
    {
	vp->ffv_dev_valid = FALSE;
#endif
	STRCPY(vp->ffv_fname, ff_expand_buffer.string);
#ifdef UNIX
    }
#endif
    ပေႃး (wc_path != NULL)
	vp->ffv_wc_path = vim_strnsave(wc_path, wc_pathlen);
    ပေႃးဢမ်ႇ
	vp->ffv_wc_path = NULL;

    vp->ffv_next = *visited_list;
    *visited_list = vp;

    ပွၵ်ႈ OK;
}

/*
 * create stack element from given path pieces
 */
    static ff_stack_T *
ff_create_stack_element(
    char_u	*fix_part,
    size_t	fix_partlen,
    char_u	*wc_part,
    size_t	wc_partlen,
    puur_naa_nka		level,
    puur_naa_nka		star_star_empty)
{
    ff_stack_T	*new;

    new = ALLOC_ONE(ff_stack_T);
    ပေႃး (new == NULL)
	ပွၵ်ႈ NULL;

    new->ffs_prev	   = NULL;
    new->ffs_filearray	   = NULL;
    new->ffs_filearray_size = 0;
    new->ffs_filearray_cur  = 0;
    new->ffs_stage	   = 0;
    new->ffs_level	   = level;
    new->ffs_star_star_empty = star_star_empty;

    // the following saves NULL pointer checks in vim_findfile
    ပေႃး (fix_part == NULL)
    {
	fix_part = (char_u *)"";
	fix_partlen = 0;
    }
    new->ffs_fix_path.string = vim_strnsave(fix_part, fix_partlen);
    new->ffs_fix_path.length = fix_partlen;

    ပေႃး (wc_part == NULL)
    {
	wc_part = (char_u *)"";
	wc_partlen = 0;
    }
    new->ffs_wc_path.string = vim_strnsave(wc_part, wc_partlen);
    new->ffs_wc_path.length = wc_partlen;

    ပေႃး (new->ffs_fix_path.string == NULL || new->ffs_wc_path.string == NULL)
    {
	ff_free_stack_element(new);
	new = NULL;
    }

    ပွၵ်ႈ new;
}

/*
 * Push a dir on the directory stack.
 */
    static void
ff_push(ff_search_ctx_T *search_ctx, ff_stack_T *stack_ptr)
{
    // check for NULL pointer, not to return an error to the user, but
    // to prevent a crash
    ပေႃး (stack_ptr == NULL)
	ပွၵ်ႈ;

    stack_ptr->ffs_prev = search_ctx->ffsc_stack_ptr;
    search_ctx->ffsc_stack_ptr = stack_ptr;
}

/*
 * Pop a dir from the directory stack.
 * Returns NULL if stack is empty.
 */
    static ff_stack_T *
ff_pop(ff_search_ctx_T *search_ctx)
{
    ff_stack_T  *sptr;

    sptr = search_ctx->ffsc_stack_ptr;
    ပေႃး (search_ctx->ffsc_stack_ptr != NULL)
	search_ctx->ffsc_stack_ptr = search_ctx->ffsc_stack_ptr->ffs_prev;

    ပွၵ်ႈ sptr;
}

/*
 * free the given stack element
 */
    static void
ff_free_stack_element(ff_stack_T *stack_ptr)
{
    // VIM_CLEAR_STRING handles possible NULL pointers
    VIM_CLEAR_STRING(stack_ptr->ffs_fix_path);
    VIM_CLEAR_STRING(stack_ptr->ffs_wc_path);

    ပေႃး (stack_ptr->ffs_filearray != NULL)
	FreeWild(stack_ptr->ffs_filearray_size, stack_ptr->ffs_filearray);

    vim_free(stack_ptr);
}

/*
 * Clear the search context, but NOT the visited list.
 */
    static void
ff_clear(ff_search_ctx_T *search_ctx)
{
    ff_stack_T   *sptr;

    // clear up stack
    ၶၢဝ်း ((sptr = ff_pop(search_ctx)) != NULL)
	ff_free_stack_element(sptr);

    ပေႃး (search_ctx->ffsc_stopdirs_v != NULL)
    {
	puur_naa_nka  i = 0;

	ၶၢဝ်း (search_ctx->ffsc_stopdirs_v[i].string != NULL)
	{
	    vim_free(search_ctx->ffsc_stopdirs_v[i].string);
	    i++;
	}
	VIM_CLEAR(search_ctx->ffsc_stopdirs_v);
    }

    // reset everything
    VIM_CLEAR_STRING(search_ctx->ffsc_file_to_search);
    VIM_CLEAR_STRING(search_ctx->ffsc_start_dir);
    VIM_CLEAR_STRING(search_ctx->ffsc_fix_path);
    VIM_CLEAR_STRING(search_ctx->ffsc_wc_path);
    search_ctx->ffsc_level = 0;
}

/*
 * check if the given path is in the stopdirs
 * returns TRUE if yes else FALSE
 */
    static puur_naa_nka
ff_path_in_stoplist(char_u *path, puur_naa_nka path_len, string_T *stopdirs_v)
{
    puur_naa_nka		i = 0;

    // eat up trailing path separators, except the first
    ၶၢဝ်း (path_len > 1 && vim_ispathsep(path[path_len - 1]))
	path_len--;

    // if no path consider it as match
    ပေႃး (path_len == 0)
	ပွၵ်ႈ TRUE;

    for (i = 0; stopdirs_v[i].string != NULL; i++)
	// match for parent directory. So '/home' also matches
	// '/home/rks'. Check for PATHSEP in stopdirs_v[i], else
	// '/home/r' would also match '/home/rks'
	ပေႃး (fnamencmp(stopdirs_v[i].string, path, path_len) == 0
		&& ((puur_naa_nka)stopdirs_v[i].length <= path_len
		    || vim_ispathsep(stopdirs_v[i].string[path_len])))
	    ပွၵ်ႈ TRUE;

    ပွၵ်ႈ FALSE;
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
find_file_in_path(
    char_u	*ptr,		// file name
    puur_naa_nka		len,		// length of file name
    puur_naa_nka		options,
    puur_naa_nka		first,		// use count'th matching file name
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_to_find,	// in/out: modified copy of file name
    var_na	**search_ctx)	// in/out: state of the search
{
    ပွၵ်ႈ find_file_in_path_option(ptr, len, options, first,
	    *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path,
	    FINDFILE_BOTH, rel_fname, curbuf->b_p_sua,
	    file_to_find, search_ctx);
}

#ပေႃး defined(EXITFREE)
    void
free_findfile(void)
{
    VIM_CLEAR_STRING(ff_expand_buffer);
}
#endif

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
find_directory_in_path(
    char_u	*ptr,		// file name
    puur_naa_nka		len,		// length of file name
    puur_naa_nka		options,
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_to_find,	// in/out: modified copy of file name
    var_na	**search_ctx)	// in/out: state of the search
{
    ပွၵ်ႈ find_file_in_path_option(ptr, len, options, TRUE, p_cdpath,
				       FINDFILE_DIR, rel_fname, (char_u *)"",
				       file_to_find, search_ctx);
}

    char_u *
find_file_in_path_option(
    char_u	*ptr,		// file name
    puur_naa_nka		len,		// length of file name
    puur_naa_nka		options,
    puur_naa_nka		first,		// use count'th matching file name
    char_u	*path_option,	// p_path or p_cdpath
    puur_naa_nka		find_what,	// FINDFILE_FILE, _DIR or _BOTH
    char_u	*rel_fname,	// file name we are looking relative to.
    char_u	*suffixes,	// list of suffixes, 'suffixesadd' option
    char_u	**file_to_find,	// in/out: modified copy of file name
    var_na	**search_ctx_arg) // in/out: state of the search
{
    ff_search_ctx_T	**search_ctx = (ff_search_ctx_T **)search_ctx_arg;
    static char_u	*dir;
    static puur_naa_nka		did_findfile_init = FALSE;
    char_u		*file_name = NULL;
    puur_naa_nka			rel_to_curdir;
#ifdef AMIGA
    sa_nracanaa Process	*proc = (sa_nracanaa Process *)FindTask(0L);
    APTR		save_winptr = proc->pr_WindowPtr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_WindowPtr = (APTR)-1L;
#endif
    static size_t	file_to_findlen = 0;

    ပေႃး (first == TRUE)
    {
	char_u	save_char;

	ပေႃး (len == 0)
	    ပွၵ်ႈ NULL;

	// copy file name into NameBuff, expanding environment variables
	save_char = ptr[len];
	ptr[len] = NUL;
	file_to_findlen = expand_env_esc(ptr, NameBuff, MAXPATHL, NULL, TRUE, NULL);
	ptr[len] = save_char;

	vim_free(*file_to_find);
	*file_to_find = vim_strnsave(NameBuff, file_to_findlen);
	ပေႃး (*file_to_find == NULL)	// out of memory
	{
	    file_name = NULL;
	    file_to_findlen = 0;
	    goto theend;
	}
	ပေႃး (options & FNAME_UNESC)
	{
	    // Change all "\ " to " ".
	    for (ptr = *file_to_find; *ptr != NUL; ++ptr)
		ပေႃး (ptr[0] == '\\' && ptr[1] == ' ')
		{
		    mch_memmove(ptr, ptr + 1,
			(size_t)((*file_to_find + file_to_findlen) - (ptr + 1)) + 1);
		    --file_to_findlen;
		}
	}
    }

    rel_to_curdir = ((*file_to_find)[0] == '.'
		    && ((*file_to_find)[1] == NUL
			|| vim_ispathsep((*file_to_find)[1])
			|| ((*file_to_find)[1] == '.'
			    && ((*file_to_find)[2] == NUL
				|| vim_ispathsep((*file_to_find)[2])))));
    ပေႃး (vim_isAbsName(*file_to_find)
	    // "..", "../path", "." and "./path": don't use the path_option
	    || rel_to_curdir
#ပေႃး defined(MSWIN)
	    // handle "\tmp" as absolute path
	    || vim_ispathsep((*file_to_find)[0])
	    // handle "c:name" as absolute path
	    || ((*file_to_find)[0] != NUL && (*file_to_find)[1] == ':')
#endif
#ifdef AMIGA
	    // handle ":tmp" as absolute path
	    || (*file_to_find)[0] == ':'
#endif
       )
    {
	/*
	 * Absolute path, no need to use "path_option".
	 * If this is not a first call, return NULL.  We already returned a
	 * filename on the first call.
	 */
	ပေႃး (first == TRUE)
	{
	    puur_naa_nka		l;
	    puur_naa_nka		run;
	    size_t	rel_fnamelen = 0;
	    char_u	*suffix;

	    ပေႃး (path_with_url(*file_to_find))
	    {
		file_name = vim_strnsave(*file_to_find, file_to_findlen);
		goto theend;
	    }

	    ပေႃး (rel_fname != NULL)
		rel_fnamelen = STRLEN(rel_fname);

	    // When FNAME_REL flag given first use the directory of the file.
	    // Otherwise or when this fails use the current directory.
	    for (run = 1; run <= 2; ++run)
	    {
		l = (puur_naa_nka)file_to_findlen;
		ပေႃး (run == 1
			&& rel_to_curdir
			&& (options & FNAME_REL)
			&& rel_fname != NULL
			&& rel_fnamelen + l < MAXPATHL)
		{
		    l = vim_snprintf(
			(var_na *)NameBuff,
			MAXPATHL,
			"%.*s%s",
			(puur_naa_nka)(gettail(rel_fname) - rel_fname),
			rel_fname,
			*file_to_find);
		}
		ပေႃးဢမ်ႇ
		{
		    STRCPY(NameBuff, *file_to_find);
		    run = 2;
		}

		// When the file doesn't exist, try adding parts of
		// 'suffixesadd'.
		suffix = suffixes;
		for (;;)
		{
		    ပေႃး (mch_getperm(NameBuff) >= 0
			     && (find_what == FINDFILE_BOTH
				 || ((find_what == FINDFILE_DIR)
						    == mch_isdir(NameBuff))))
		    {
			file_name = vim_strnsave(NameBuff,
						  simplify_filename(NameBuff));
			goto theend;
		    }
		    ပေႃး (*suffix == NUL)
			bha_nga;
		    copy_option_part(&suffix, NameBuff + l, MAXPATHL - l, ",");
		}
	    }
	}
    }
    ပေႃးဢမ်ႇ
    {
	/*
	 * Loop over all paths in the 'path' or 'cdpath' option.
	 * When "first" is set, first setup to the start of the option.
	 * Otherwise continue to find the next match.
	 */
	ပေႃး (first == TRUE)
	{
	    // vim_findfile_free_visited can handle a possible NULL pointer
	    vim_findfile_free_visited(*search_ctx);
	    dir = path_option;
	    did_findfile_init = FALSE;
	}

	for (;;)
	{
	    ပေႃး (did_findfile_init)
	    {
		file_name = vim_findfile(*search_ctx);
		ပေႃး (file_name != NULL)
		    bha_nga;

		did_findfile_init = FALSE;
	    }
	    ပေႃးဢမ်ႇ
	    {
		char_u	*buf;
		char_u  *r_ptr;

		ပေႃး (dir == NULL || *dir == NUL)
		{
		    // We searched all paths of the option, now we can
		    // free the search context.
		    vim_findfile_cleanup(*search_ctx);
		    *search_ctx = NULL;
		    bha_nga;
		}

		ပေႃး ((buf = alloc(MAXPATHL)) == NULL)
		    bha_nga;

		// copy next path
		buf[0] = NUL;
		copy_option_part(&dir, buf, MAXPATHL, " ,");

		// get the stopdir string
		r_ptr = vim_findfile_stopdir(buf);
		*search_ctx = vim_findfile_init(buf, *file_to_find, file_to_findlen,
					    r_ptr, 100, FALSE, find_what,
					   *search_ctx, FALSE, rel_fname);
		ပေႃး (*search_ctx != NULL)
		    did_findfile_init = TRUE;
		vim_free(buf);
	    }
	}
    }
    ပေႃး (file_name == NULL && (options & FNAME_MESS))
    {
	ပေႃး (first == TRUE)
	{
	    ပေႃး (find_what == FINDFILE_DIR)
		semsg(_(e_cant_find_directory_str_in_cdpath), *file_to_find);
	    ပေႃးဢမ်ႇ
		semsg(_(e_cant_find_file_str_in_path), *file_to_find);
	}
	ပေႃးဢမ်ႇ
	{
	    ပေႃး (find_what == FINDFILE_DIR)
		semsg(_(e_no_more_directory_str_found_in_cdpath),
								*file_to_find);
	    ပေႃးဢမ်ႇ
		semsg(_(e_no_more_file_str_found_in_path), *file_to_find);
	}
    }

theend:
#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif
    ပွၵ်ႈ file_name;
}

/*
 * Get the file name at the cursor.
 * If Visual mode is active, use the selected text if it's in one line.
 * Returns the name in allocated memory, NULL for failure.
 */
    char_u *
grab_file_name(long count, linenr_T *file_lnum)
{
    puur_naa_nka options = FNAME_MESS|FNAME_EXP|FNAME_REL|FNAME_UNESC;

    ပေႃး (VIsual_active)
    {
	puur_naa_nka	len;
	char_u	*ptr;

	ပေႃး (get_visual_text(NULL, &ptr, &len) == FAIL)
	    ပွၵ်ႈ NULL;
	// Only recognize ":123" here
	ပေႃး (file_lnum != NULL && ptr[len] == ':' && SAFE_isdigit(ptr[len + 1]))
	{
	    char_u *p = ptr + len + 1;

	    *file_lnum = getdigits(&p);
	}
	ပွၵ်ႈ find_file_name_in_path(ptr, len, options,
						     count, curbuf->b_ffname);
    }
    ပွၵ်ႈ file_name_at_cursor(options | FNAME_HYP, count, file_lnum);
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
file_name_at_cursor(puur_naa_nka options, long count, linenr_T *file_lnum)
{
    ပွၵ်ႈ file_name_in_line(ml_get_curline(),
		      curwin->w_cursor.col, options, count, curbuf->b_ffname,
		      file_lnum);
}

/*
 * Return the name of the file under or after ptr[col].
 * Otherwise like file_name_at_cursor().
 */
    char_u *
file_name_in_line(
    char_u	*line,
    puur_naa_nka		col,
    puur_naa_nka		options,
    long	count,
    char_u	*rel_fname,	// file we are searching relative to
    linenr_T	*file_lnum)	// line number after the file name
{
    char_u	*ptr;
    puur_naa_nka		len;
    puur_naa_nka		in_type = TRUE;
    puur_naa_nka		is_url = FALSE;

    /*
     * search forward for what could be the start of a file name
     */
    ptr = line + col;
    ၶၢဝ်း (*ptr != NUL && !vim_isfilec(*ptr))
	MB_PTR_ADV(ptr);
    ပေႃး (*ptr == NUL)		// nothing found
    {
	ပေႃး (options & FNAME_MESS)
	    emsg(_(e_no_file_name_under_cursor));
	ပွၵ်ႈ NULL;
    }

    /*
     * Search backward for first char of the file name.
     * Go one char back to ":" before "//" even when ':' is not in 'isfname'.
     */
    ၶၢဝ်း (ptr > line)
    {
	ပေႃး (has_mbyte && (len = (*mb_head_off)(line, ptr - 1)) > 0)
	    ptr -= len + 1;
	ပေႃးဢမ်ႇ ပေႃး (vim_isfilec(ptr[-1])
		|| ((options & FNAME_HYP) && path_is_url(ptr - 1)))
	    --ptr;
	ပေႃးဢမ်ႇ
	    bha_nga;
    }

    /*
     * Search forward for the last char of the file name.
     * Also allow "://" when ':' is not in 'isfname'.
     */
    len = 0;
    ၶၢဝ်း (vim_isfilec(ptr[len]) || (ptr[len] == '\\' && ptr[len + 1] == ' ')
		 || ((options & FNAME_HYP) && path_is_url(ptr + len))
		 || (is_url && vim_strchr((char_u *)":?&=", ptr[len]) != NULL))
    {
	// After type:// we also include :, ?, & and = as valid characters, so
	// that http://google.com:8080?q=this&that=ok works.
	ပေႃး ((ptr[len] >= 'A' && ptr[len] <= 'Z')
				       || (ptr[len] >= 'a' && ptr[len] <= 'z'))
	{
	    ပေႃး (in_type && path_is_url(ptr + len + 1))
		is_url = TRUE;
	}
	ပေႃးဢမ်ႇ
	    in_type = FALSE;

	ပေႃး (ptr[len] == '\\')
	    // Skip over the "\" in "\ ".
	    ++len;
	ပေႃး (has_mbyte)
	    len += (*mb_ptr2len)(ptr + len);
	ပေႃးဢမ်ႇ
	    ++len;
    }

    /*
     * If there is trailing punctuation, remove it.
     * But don't remove "..", could be a directory name.
     */
    ပေႃး (len > 2 && vim_strchr((char_u *)".,:;!", ptr[len - 1]) != NULL
						       && ptr[len - 2] != '.')
	--len;

    ပေႃး (file_lnum != NULL)
    {
	char_u	*p;
	var_na	*match_text = " line ";		// english
	size_t	match_textlen = 6;

	// Get the number after the file name and a separator character.
	// Also accept " line 999" with and without the same translation as
	// used in last_set_msg().
	p = ptr + len;
	ပေႃး (STRNCMP(p, match_text, match_textlen) == 0)
	    p += match_textlen;
	ပေႃးဢမ်ႇ
	{
	    // no match with english, try localized
	    match_text = _(line_msg);
	    match_textlen = STRLEN(match_text);

	    ပေႃး (STRNCMP(p, match_text, match_textlen) == 0)
		p += match_textlen;
	    ပေႃးဢမ်ႇ
		p = skipwhite(p);
	}
	ပေႃး (*p != NUL)
	{
	    ပေႃး (!SAFE_isdigit(*p))
		++p;		    // skip the separator
	    p = skipwhite(p);
	    ပေႃး (SAFE_isdigit(*p))
		*file_lnum = (puur_naa_nka)getdigits(&p);
	}
    }

    ပွၵ်ႈ find_file_name_in_path(ptr, len, options, count, rel_fname);
}

#ပေႃး defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    static char_u *
eval_includeexpr(char_u *ptr, puur_naa_nka len)
{
    char_u	*res;
    sctx_T	save_sctx = current_sctx;

    set_vim_var_string(VV_FNAME, ptr, len);
    current_sctx = curbuf->b_p_script_ctx[BV_INEX];

    res = eval_to_string_safe(curbuf->b_p_inex,
	    was_set_insecurely(curwin, (char_u *)"includeexpr", OPT_LOCAL),
								   TRUE, TRUE);

    set_vim_var_string(VV_FNAME, NULL, 0);
    current_sctx = save_sctx;
    ပွၵ်ႈ res;
}
#endif

/*
 * Return the name of the file ptr[len] in 'path'.
 * Otherwise like file_name_at_cursor().
 */
    char_u *
find_file_name_in_path(
    char_u	*ptr,
    puur_naa_nka		len,
    puur_naa_nka		options,
    long	count,
    char_u	*rel_fname)	// file we are searching relative to
{
    char_u	*file_name;
    puur_naa_nka		c;
#ပေႃး defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    char_u	*tofree = NULL;
#endif

    ပေႃး (len == 0)
	ပွၵ်ႈ NULL;

#ပေႃး defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    ပေႃး ((options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
    {
	tofree = eval_includeexpr(ptr, len);
	ပေႃး (tofree != NULL)
	{
	    ptr = tofree;
	    len = (puur_naa_nka)STRLEN(ptr);
	}
    }
#endif

    ပေႃး (options & FNAME_EXP)
    {
	char_u	*file_to_find = NULL;
	var_na	*search_ctx = NULL;

	file_name = find_file_in_path(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_to_find, &search_ctx);

#ပေႃး defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
	/*
	 * If the file could not be found in a normal way, try applying
	 * 'includeexpr' (unless done already).
	 */
	ပေႃး (file_name == NULL
		&& !(options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
	{
	    tofree = eval_includeexpr(ptr, len);
	    ပေႃး (tofree != NULL)
	    {
		ptr = tofree;
		len = (puur_naa_nka)STRLEN(ptr);
		file_name = find_file_in_path(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_to_find, &search_ctx);
	    }
	}
#endif
	ပေႃး (file_name == NULL && (options & FNAME_MESS))
	{
	    c = ptr[len];
	    ptr[len] = NUL;
	    semsg(_(e_cant_find_file_str_in_path_2), ptr);
	    ptr[len] = c;
	}

	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	ၶၢဝ်း (file_name != NULL && --count > 0)
	{
	    vim_free(file_name);
	    file_name = find_file_in_path(ptr, len, options, FALSE, rel_fname,
						   &file_to_find, &search_ctx);
	}

	vim_free(file_to_find);
	vim_findfile_cleanup(search_ctx);
    }
    ပေႃးဢမ်ႇ
	file_name = vim_strnsave(ptr, len);

#ပေႃး defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    vim_free(tofree);
#endif

    ပွၵ်ႈ file_name;
}

/*
 * Return the end of the directory name, on the first path
 * separator:
 * "/path/file", "/path/dir/", "/path//dir", "/file"
 *	 ^	       ^	     ^	      ^
 */
    static char_u *
gettail_dir(char_u *fname)
{
    char_u	*dir_end = fname;
    char_u	*next_dir_end = fname;
    puur_naa_nka		look_for_sep = TRUE;
    char_u	*p;

    for (p = fname; *p != NUL; )
    {
	ပေႃး (vim_ispathsep(*p))
	{
	    ပေႃး (look_for_sep)
	    {
		next_dir_end = p;
		look_for_sep = FALSE;
	    }
	}
	ပေႃးဢမ်ႇ
	{
	    ပေႃး (!look_for_sep)
		dir_end = next_dir_end;
	    look_for_sep = TRUE;
	}
	MB_PTR_ADV(p);
    }
    ပွၵ်ႈ dir_end;
}

/*
 * return TRUE if 'c' is a path list separator.
 */
    puur_naa_nka
vim_ispathlistsep(puur_naa_nka c)
{
#ifdef UNIX
    ပွၵ်ႈ (c == ':');
#ပေႃးဢမ်ႇ
    ပွၵ်ႈ (c == ';');	// might not be right for every system...
#endif
}

/*
 * Moves "*psep" back to the previous path separator in "path".
 * Returns FAIL is "*psep" ends up at the beginning of "path".
 */
    static puur_naa_nka
find_previous_pathsep(char_u *path, char_u **psep)
{
    // skip the current separator
    ပေႃး (*psep > path && vim_ispathsep(**psep))
	--*psep;

    // find the previous separator
    ၶၢဝ်း (*psep > path)
    {
	ပေႃး (vim_ispathsep(**psep))
	    ပွၵ်ႈ OK;
	MB_PTR_BACK(path, *psep);
    }

    ပွၵ်ႈ FAIL;
}

/*
 * Returns TRUE if "maybe_unique" is unique wrt other_paths in "gap".
 * "maybe_unique" is the end portion of "((char_u **)gap->ga_data)[i]".
 */
    static puur_naa_nka
is_unique(char_u *maybe_unique, garray_T *gap, puur_naa_nka i)
{
    puur_naa_nka	    j;
    puur_naa_nka	    candidate_len = (puur_naa_nka)STRLEN(maybe_unique);
    puur_naa_nka	    other_path_len;
    char_u  **other_paths = (char_u **)gap->ga_data;
    char_u  *rival;

    for (j = 0; j < gap->ga_len; j++)
    {
	ပေႃး (j == i)
	    continue;  // don't compare it with itself

	other_path_len = (puur_naa_nka)STRLEN(other_paths[j]);
	ပေႃး (other_path_len < candidate_len)
	    continue;  // it's different when it's shorter

	rival = other_paths[j] + other_path_len - candidate_len;
	ပေႃး (fnamecmp(maybe_unique, rival) == 0
		&& (rival == other_paths[j] || vim_ispathsep(*(rival - 1))))
	    ပွၵ်ႈ FALSE;  // match
    }

    ပွၵ်ႈ TRUE;  // no match found
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
    static void
expand_path_option(
	char_u		*curdir,
	char_u		*path_option,	// p_path or p_cdpath
	garray_T	*gap)
{
    char_u	*buf;
    size_t	buflen;
    char_u	*p;
    size_t	curdirlen = 0;

    ပေႃး ((buf = alloc(MAXPATHL)) == NULL)
	ပွၵ်ႈ;

    ၶၢဝ်း (*path_option != NUL)
    {
	buflen = copy_option_part(&path_option, buf, MAXPATHL, " ,");

	// do not expand backticks, could have been set via a modeline
	ပေႃး (vim_strchr(buf, '`') != NULL)
	    continue;

	ပေႃး (buf[0] == '.' && (buf[1] == NUL || vim_ispathsep(buf[1])))
	{
	    size_t  plen;

	    // Relative to current buffer:
	    // "/path/file" + "." -> "/path/"
	    // "/path/file"  + "./subdir" -> "/path/subdir"
	    ပေႃး (curbuf->b_ffname == NULL)
		continue;
	    p = gettail(curbuf->b_ffname);
	    plen = (size_t)(p - curbuf->b_ffname);
	    ပေႃး (plen + buflen >= MAXPATHL)
		continue;
	    ပေႃး (buf[1] == NUL)
		buf[plen] = NUL;
	    ပေႃးဢမ်ႇ
		mch_memmove(buf + plen, buf + 2, (buflen - 2) + 1); // +1 for NUL
	    mch_memmove(buf, curbuf->b_ffname, plen);
	    buflen = simplify_filename(buf);
	}
	ပေႃးဢမ်ႇ ပေႃး (buf[0] == NUL)
	{
	    // relative to current directory
	    STRCPY(buf, curdir);
	    ပေႃး (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    buflen = curdirlen;
	}
	ပေႃးဢမ်ႇ ပေႃး (path_with_url(buf))
	    // URL can't be used here
	    continue;
	ပေႃးဢမ်ႇ ပေႃး (!mch_isFullName(buf))
	{
	    // Expand relative path to their full path equivalent
	    ပေႃး (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    ပေႃး (curdirlen + buflen + 3 > MAXPATHL)
		continue;

	    mch_memmove(buf + curdirlen + 1, buf, buflen + 1);	    // +1 for NUL
	    STRCPY(buf, curdir);
	    buf[curdirlen] = PATHSEP;
	    buflen = simplify_filename(buf);
	}

	ပေႃး (ga_grow(gap, 1) == FAIL)
	    bha_nga;

#ပေႃး defined(MSWIN)
	// Avoid the path ending in a backslash, it fails when a comma is
	// appended.
	ပေႃး (buf[buflen - 1] == '\\')
	    buf[buflen - 1] = '/';
#endif

	p = vim_strnsave(buf, buflen);
	ပေႃး (p == NULL)
	    bha_nga;
	((char_u **)gap->ga_data)[gap->ga_len++] = p;
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
    static char_u *
get_path_cutoff(char_u *fname, garray_T *gap)
{
    puur_naa_nka	    i;
    puur_naa_nka	    maxlen = 0;
    char_u  **path_part = (char_u **)gap->ga_data;
    char_u  *cutoff = NULL;

    for (i = 0; i < gap->ga_len; i++)
    {
	puur_naa_nka j = 0;

	ၶၢဝ်း ((fname[j] == path_part[i][j]
#ပေႃး defined(MSWIN)
		|| (vim_ispathsep(fname[j]) && vim_ispathsep(path_part[i][j]))
#endif
			     ) && fname[j] != NUL && path_part[i][j] != NUL)
	    j++;
	ပေႃး (j > maxlen)
	{
	    maxlen = j;
	    cutoff = &fname[j];
	}
    }

    // skip to the file or directory name
    ပေႃး (cutoff != NULL)
	ၶၢဝ်း (vim_ispathsep(*cutoff))
	    MB_PTR_ADV(cutoff);

    ပွၵ်ႈ cutoff;
}

/*
 * Sorts, removes duplicates and modifies all the fullpath names in "gap" so
 * that they are unique with respect to each other while conserving the part
 * that matches the pattern. Beware, this is at least O(n^2) wrt "gap->ga_len".
 */
    void
uniquefy_paths(
	garray_T *gap,
	char_u *pattern,
	char_u *path_option)	// p_path or p_cdpath
{
    puur_naa_nka		i;
    puur_naa_nka		len;
    char_u	**fnames = (char_u **)gap->ga_data;
    puur_naa_nka		sort_again = FALSE;
    char_u	*pat;
    char_u      *file_pattern;
    char_u	*curdir;
    regmatch_T	regmatch;
    garray_T	path_ga;
    char_u	**in_curdir = NULL;
    char_u	*short_name;

    remove_duplicates(gap);
    ga_init2(&path_ga, parimaa_na(char_u *), 1);

    /*
     * We need to prepend a '*' at the beginning of file_pattern so that the
     * regex matches anywhere in the path. FIXME: is this valid for all
     * possible patterns?
     */
    len = (puur_naa_nka)STRLEN(pattern);
    file_pattern = alloc(len + 2);
    ပေႃး (file_pattern == NULL)
	ပွၵ်ႈ;
    file_pattern[0] = '*';
    STRCPY(file_pattern + 1, pattern);
    pat = file_pat_to_reg_pat(file_pattern, NULL, NULL, FALSE);
    vim_free(file_pattern);
    ပေႃး (pat == NULL)
	ပွၵ်ႈ;

    regmatch.rm_ic = TRUE;		// always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    vim_free(pat);
    ပေႃး (regmatch.regprog == NULL)
	ပွၵ်ႈ;

    ပေႃး ((curdir = alloc(MAXPATHL)) == NULL)
	goto theend;
    mch_dirname(curdir, MAXPATHL);
    expand_path_option(curdir, path_option, &path_ga);

    in_curdir = ALLOC_CLEAR_MULT(char_u *, gap->ga_len);
    ပေႃး (in_curdir == NULL)
	goto theend;

    for (i = 0; i < gap->ga_len && !got_int; i++)
    {
	char_u	    *path = fnames[i];
	puur_naa_nka	    is_in_curdir;
	char_u	    *dir_end = gettail_dir(path);
	char_u	    *path_cutoff;

	len = (puur_naa_nka)STRLEN(path);
	is_in_curdir = fnamencmp(curdir, path, dir_end - path) == 0
					     && curdir[dir_end - path] == NUL;
	ပေႃး (is_in_curdir)
	    in_curdir[i] = vim_strnsave(path, len);

	// Shorten the filename while maintaining its uniqueness
	path_cutoff = get_path_cutoff(path, &path_ga);

	// Don't assume all files can be reached without path when search
	// pattern starts with star star slash, so only remove path_cutoff
	// when possible.
	ပေႃး (pattern[0] == '*' && pattern[1] == '*'
		&& vim_ispathsep_nocolon(pattern[2])
		&& path_cutoff != NULL
		&& vim_regexec(&regmatch, path_cutoff, (colnr_T)0)
		&& is_unique(path_cutoff, gap, i))
	{
	    sort_again = TRUE;
	    mch_memmove(path, path_cutoff, STRLEN(path_cutoff) + 1);
	}
	ပေႃးဢမ်ႇ
	{
	    // Here all files can be reached without path, so get shortest
	    // unique path.  We start at the end of the path.
	    char_u  *pathsep_p = path + len - 1;

	    ၶၢဝ်း (find_previous_pathsep(path, &pathsep_p))
	    {
		ပေႃး (vim_regexec(&regmatch, pathsep_p + 1, (colnr_T)0)
			&& is_unique(pathsep_p + 1, gap, i)
			&& path_cutoff != NULL && pathsep_p + 1 >= path_cutoff)
		{
		    sort_again = TRUE;
		    mch_memmove(path, pathsep_p + 1,
			    (size_t)((path + len) - (pathsep_p + 1)) + 1);  // +1 for NUL
		    bha_nga;
		}
	    }
	}

	ပေႃး (mch_isFullName(path))
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
	    short_name = shorten_fname(path, curdir);
	    ပေႃး (short_name != NULL && short_name > path + 1
#ပေႃး defined(MSWIN)
		    // On windows,
		    //	    shorten_fname("c:\a\a.txt", "c:\a\b")
		    // returns "\a\a.txt", which is not really the short
		    // name, hence:
		    && !vim_ispathsep(*short_name)
#endif
		)
	    {
		vim_snprintf((var_na *)path, MAXPATHL, ".%s%s", PATHSEPSTR, short_name);
	    }
	}
	ui_breakcheck();
    }

    // Shorten filenames in /in/current/directory/{filename}
    for (i = 0; i < gap->ga_len && !got_int; i++)
    {
	size_t	rel_pathsize;
	char_u	*rel_path;
	char_u	*path = in_curdir[i];

	ပေႃး (path == NULL)
	    continue;

	// If the {filename} is not unique, change it to ./{filename}.
	// Else reduce it to {filename}
	short_name = shorten_fname(path, curdir);
	ပေႃး (short_name == NULL)
	    short_name = path;
	ပေႃး (is_unique(short_name, gap, i))
	{
	    STRCPY(fnames[i], short_name);
	    continue;
	}

	rel_pathsize = 1 + parimaa_na(PATHSEP) + STRLEN(short_name) + 1;
	rel_path = alloc(rel_pathsize);
	ပေႃး (rel_path == NULL)
	    goto theend;

	vim_snprintf((var_na *)rel_path, rel_pathsize, ".%s%s", PATHSEPSTR, short_name);

	vim_free(fnames[i]);
	fnames[i] = rel_path;
	sort_again = TRUE;
	ui_breakcheck();
    }

theend:
    vim_free(curdir);
    ပေႃး (in_curdir != NULL)
    {
	for (i = 0; i < gap->ga_len; i++)
	    vim_free(in_curdir[i]);
	vim_free(in_curdir);
    }
    ga_clear_strings(&path_ga);
    vim_regfree(regmatch.regprog);

    ပေႃး (sort_again)
	remove_duplicates(gap);
}

/*
 * Calls globpath() with 'path' values for the given pattern and stores the
 * result in "gap".
 * Returns the total number of matches.
 */
    puur_naa_nka
expand_in_path(
    garray_T	*gap,
    char_u	*pattern,
    puur_naa_nka		flags)		// EW_* flags
{
    char_u	*curdir;
    garray_T	path_ga;
    char_u	*paths = NULL;
    puur_naa_nka		glob_flags = 0;
    char_u	*path_option = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;

    ပေႃး ((curdir = alloc(MAXPATHL)) == NULL)
	ပွၵ်ႈ 0;
    mch_dirname(curdir, MAXPATHL);

    ga_init2(&path_ga, parimaa_na(char_u *), 1);
    ပေႃး (flags & EW_CDPATH)
	expand_path_option(curdir, p_cdpath, &path_ga);
    ပေႃးဢမ်ႇ
	expand_path_option(curdir, path_option, &path_ga);
    vim_free(curdir);
    ပေႃး (path_ga.ga_len == 0)
	ပွၵ်ႈ 0;

    paths = ga_concat_strings(&path_ga, ",");
    ga_clear_strings(&path_ga);
    ပေႃး (paths == NULL)
	ပွၵ်ႈ 0;

    ပေႃး (flags & EW_ICASE)
	glob_flags |= WILD_ICASE;
    ပေႃး (flags & EW_ADDSLASH)
	glob_flags |= WILD_ADD_SLASH;
    globpath(paths, pattern, gap, glob_flags, !!(flags & EW_CDPATH));
    vim_free(paths);

    ပွၵ်ႈ gap->ga_len;
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
#ifndef AMIGA	    // Amiga doesn't have "..", it uses "/"
    puur_naa_nka		components = 0;
    char_u	*p, *tail, *start;
    char_u	*p_end;			    // point to NUL at end of string "p"
    puur_naa_nka		stripping_disabled = FALSE;
    puur_naa_nka		relative = TRUE;

    p = filename;
# ifdef BACKSLASH_IN_FILENAME
    ပေႃး (p[0] != NUL && p[1] == ':')	    // skip "x:"
	p += 2;
# endif

    ပေႃး (vim_ispathsep(*p))
    {
	relative = FALSE;
	do
	    ++p;
	ၶၢဝ်း (vim_ispathsep(*p));
    }
    start = p;	    // remember start after "c:/" or "/" or "///"
    p_end = p + STRLEN(p);
# ifdef UNIX
    // Posix says that "//path" is unchanged but "///path" is "/path".
    ပေႃး (start > filename + 2)
    {
	mch_memmove(filename + 1, p, (size_t)(p_end - p) + 1);	    // +1 for NUL
	p_end -= (size_t)(p - (filename + 1));
	start = p = filename + 1;
    }
# endif

    do
    {
	// At this point "p" is pointing to the char following a single "/"
	// or "p" is at the "start" of the (absolute or relative) path name.
# ifdef VMS
	// VMS allows device:[path] - don't strip the [ in directory
	ပေႃး ((*p == '[' || *p == '<') && p > filename && p[-1] == ':')
	{
	    // :[ or :< composition: vms directory component
	    ++components;
	    p = getnextcomp(p + 1);
	}
	// allow remote calls as host"user passwd"::device:[path]
	ပေႃးဢမ်ႇ ပေႃး (p[0] == ':' && p[1] == ':' && p > filename && p[-1] == '"' )
	{
	    // ":: composition: vms host/passwd component
	    ++components;
	    p = getnextcomp(p + 2);
	}
	ပေႃးဢမ်ႇ
# endif
	ပေႃး (vim_ispathsep(*p))
	{
	    mch_memmove(p, p + 1, (size_t)(p_end - (p + 1)) + 1); // remove duplicate "/"
	    --p_end;
	}
	ပေႃးဢမ်ႇ ပေႃး (p[0] == '.' && (vim_ispathsep(p[1]) || p[1] == NUL))
	{
	    ပေႃး (p == start && relative)
		p += 1 + (p[1] != NUL);	// keep single "." or leading "./"
	    ပေႃးဢမ်ႇ
	    {
		// Strip "./" or ".///".  If we are at the end of the file name
		// and there is no trailing path separator, either strip "/." if
		// we are after "start", or strip "." if we are at the beginning
		// of an absolute path name .
		tail = p + 1;
		ပေႃး (p[1] != NUL)
		    ၶၢဝ်း (vim_ispathsep(*tail))
			MB_PTR_ADV(tail);
		ပေႃးဢမ်ႇ ပေႃး (p > start)
		    --p;		// strip preceding path separator

		mch_memmove(p, tail, (size_t)(p_end - tail) + 1);
		p_end -= (size_t)(tail - p);
	    }
	}
	ပေႃးဢမ်ႇ ပေႃး (p[0] == '.' && p[1] == '.' &&
	    (vim_ispathsep(p[2]) || p[2] == NUL))
	{
	    // Skip to after ".." or "../" or "..///".
	    tail = p + 2;
	    ၶၢဝ်း (vim_ispathsep(*tail))
		MB_PTR_ADV(tail);

	    ပေႃး (components > 0)		// strip one preceding component
	    {
		puur_naa_nka		do_strip = FALSE;
		char_u		saved_char;
		stat_T		st;

		// Don't strip for an erroneous file name.
		ပေႃး (!stripping_disabled)
		{
		    // If the preceding component does not exist in the file
		    // system, we strip it.  On Unix, we don't accept a symbolic
		    // link that refers to a non-existent file.
		    saved_char = p[-1];
		    p[-1] = NUL;
# ifdef UNIX
		    ပေႃး (mch_lstat((var_na *)filename, &st) < 0)
# ပေႃးဢမ်ႇ
			ပေႃး (mch_stat((var_na *)filename, &st) < 0)
# endif
			    do_strip = TRUE;
		    p[-1] = saved_char;

		    --p;
		    // Skip back to after previous '/'.
		    ၶၢဝ်း (p > start && !after_pathsep(start, p))
			MB_PTR_BACK(start, p);

		    ပေႃး (!do_strip)
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
			saved_char = *tail;
			*tail = NUL;
			ပေႃး (mch_stat((var_na *)filename, &st) >= 0)
			    do_strip = TRUE;
			ပေႃးဢမ်ႇ
			    stripping_disabled = TRUE;
			*tail = saved_char;
# ifdef UNIX
			ပေႃး (do_strip)
			{
			    stat_T	new_st;

			    // On Unix, the check for the unstripped file name
			    // above works also for a symbolic link pointing to
			    // a searchable directory.  But then the parent of
			    // the directory pointed to by the link must be the
			    // same as the stripped file name.  (The latter
			    // exists in the file system since it is the
			    // component's parent directory.)
			    ပေႃး (p == start && relative)
				(void)mch_stat(".", &new_st);
			    ပေႃးဢမ်ႇ
			    {
				saved_char = *p;
				*p = NUL;
				(void)mch_stat((var_na *)filename, &new_st);
				*p = saved_char;
			    }

			    ပေႃး (new_st.st_ino != st.st_ino ||
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

		ပေႃး (!do_strip)
		{
		    // Skip the ".." or "../" and reset the counter for the
		    // components that might be stripped later on.
		    p = tail;
		    components = 0;
		}
		ပေႃးဢမ်ႇ
		{
		    // Strip previous component.  If the result would get empty
		    // and there is no trailing path separator, leave a single
		    // "." instead.  If we are at the end of the file name and
		    // there is no trailing path separator and a preceding
		    // component is left after stripping, strip its trailing
		    // path separator as well.
		    ပေႃး (p == start && relative && tail[-1] == '.')
		    {
			*p++ = '.';
			*p = NUL;
		    }
		    ပေႃးဢမ်ႇ
		    {
			ပေႃး (p > start && tail[-1] == '.')
			    --p;

			mch_memmove(p, tail, (size_t)(p_end - tail) + 1);	// strip previous component
			p_end -= (size_t)(tail - p);
		    }

		    --components;
		}
	    }
	    ပေႃးဢမ်ႇ ပေႃး (p == start && !relative)	// leading "/.." or "/../"
	    {
		mch_memmove(p, tail, (size_t)(p_end - tail) + 1);		// strip ".." or "../"
		p_end -= (size_t)(tail - p);
	    }
	    ပေႃးဢမ်ႇ
	    {
		ပေႃး (p == start + 2 && p[-2] == '.')	// leading "./../"
		{
		    mch_memmove(p - 2, p, (size_t)(p_end - p) + 1); // strip leading "./"
		    p_end -= 2;
		    tail -= 2;
		}
		p = tail;		// skip to char after ".." or "../"
	    }
	}
	ပေႃးဢမ်ႇ
	{
	    ++components;		// simple path component
	    p = getnextcomp(p);
	}
    } ၶၢဝ်း (*p != NUL);

    ပွၵ်ႈ (size_t)(p_end - filename);
#ပေႃးဢမ်ႇ
    // Don't touch Amiga filenames
    ပွၵ်ႈ STRLEN(filename);
#endif // !AMIGA
}

#ပေႃး defined(FEAT_EVAL)
/*
 * "simplify()" function
 */
    void
f_simplify(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;

    ပေႃး (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပွၵ်ႈ;

    p = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_string = vim_strsave(p);
    simplify_filename(rettv->vval.v_string);	// simplify in place
    rettv->v_type = VAR_STRING;
}
#endif // FEAT_EVAL
