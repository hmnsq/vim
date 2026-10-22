/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * fileio.c: read from and write to a file
 */

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#endif
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#endif
#എങ്കിൽ defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#endif

// Is there any system that doesn't have access()?
#define USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#define NONASCII_MASK (((long_u)-1 / 0xFF) * 0x80)

#എങ്കിൽ defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#endif

സ്ഥിതം char_u *next_fenc(char_u **pp, പൂർണ്ണസംഖ്യ *alloced);
#ifdef FEAT_EVAL
സ്ഥിതം char_u *readfile_charconvert(char_u *fname, char_u *fenc, പൂർണ്ണസംഖ്യ *fdp);
#endif
#ifdef FEAT_CRYPT
സ്ഥിതം char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, long *sizep, off_T *filesizep, പൂർണ്ണസംഖ്യ newfile, char_u *fname, പൂർണ്ണസംഖ്യ *did_ask);
#endif
സ്ഥിതം linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
സ്ഥിതം char_u *check_for_bom(char_u *p, long size, പൂർണ്ണസംഖ്യ *lenp, പൂർണ്ണസംഖ്യ flags);

#ifdef FEAT_EVAL
സ്ഥിതം പൂർണ്ണസംഖ്യ readdirex_sort;
#endif

    ശൂന്യം
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    പൂർണ്ണസംഖ്യ		attr)
{
    പൂർണ്ണസംഖ്യ		സന്ദേശം_scroll_save;
    പൂർണ്ണസംഖ്യ		prev_സന്ദേശം_col = സന്ദേശം_col;
    size_t	len;

    എങ്കിൽ (സന്ദേശം_silent != 0)
	തിരികെ;
    സന്ദേശം_add_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    എങ്കിൽ (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    എങ്കിൽ (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    സന്ദേശം_scroll_save = സന്ദേശം_scroll;
    എങ്കിൽ (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	സന്ദേശം_scroll = FALSE;
    എങ്കിൽ (!സന്ദേശം_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    സന്ദേശം_start();
    എങ്കിൽ (prev_സന്ദേശം_col != 0 && സന്ദേശം_col == 0)
	സന്ദേശം_putchar('\r');  // overwrite any previous message.
    സന്ദേശം_scroll = സന്ദേശം_scroll_save;
    സന്ദേശം_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    സന്ദേശം_outtrans_attr(സന്ദേശം_may_trunc(FALSE, IObuff), attr);
    സന്ദേശം_clr_eos();
    out_flush();
    സന്ദേശം_scrolled_ign = FALSE;
}

/*
 * Read lines from file "fname" into the buffer after line "from".
 *
 * 1. We allocate blocks with lalloc, as big as possible.
 * 2. Each block is filled with characters from the file with a single read().
 * 3. The lines are inserted in the buffer with ml_append().
 *
 * (caller must check that fname != NULL, unless READ_STDIN is used)
 *
 * "lines_to_skip" is the number of lines that must be skipped
 * "lines_to_read" is the number of lines that are appended
 * When not recovering lines_to_skip is 0 and lines_to_read MAXLNUM.
 *
 * flags:
 * READ_NEW	starting to edit a new buffer
 * READ_FILTER	reading filter output
 * READ_STDIN	read from stdin instead of a file
 * READ_BUFFER	read from curbuf instead of a file (converting after reading
 *		stdin)
 * READ_NOFILE	do not read a file, only trigger BufReadCmd
 * READ_DUMMY	read into a dummy buffer (to check if file contents changed)
 * READ_KEEP_UNDO  don't clear undo info or read it from a file
 * READ_FIFO	read from fifo/socket instead of a file
 *
 * return FAIL for failure, NOTDONE for directory (failure), or OK
 */
    പൂർണ്ണസംഖ്യ
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	വരികൾ_to_skip,
    linenr_T	വരികൾ_to_read,
    exarg_T	*eap,			// can be NULL!
    പൂർണ്ണസംഖ്യ		flags)
{
    പൂർണ്ണസംഖ്യ		retval = FAIL;	// jump to "theend" instead of returning
    പൂർണ്ണസംഖ്യ		fd = 0;
    പൂർണ്ണസംഖ്യ		newfile = (flags & READ_NEW);
    പൂർണ്ണസംഖ്യ		check_readonly;
    പൂർണ്ണസംഖ്യ		filtering = (flags & READ_FILTER);
    പൂർണ്ണസംഖ്യ		read_stdin = (flags & READ_STDIN);
    പൂർണ്ണസംഖ്യ		read_buffer = (flags & READ_BUFFER);
    പൂർണ്ണസംഖ്യ		read_fifo = (flags & READ_FIFO);
    പൂർണ്ണസംഖ്യ		set_options = newfile || read_buffer
					   || (eap != ശൂന്യ && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_col = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = ശൂന്യ;		// pointer into read buffer
    char_u	*buffer = ശൂന്യ;		// read buffer
    char_u	*new_buffer = ശൂന്യ;	// init to shut up gcc
    char_u	*വരി_start = ശൂന്യ;	// init to shut up gcc
    പൂർണ്ണസംഖ്യ		wasempty;		// buffer was empty before reading
    colnr_T	len;
    long	size = 0;
    char_u	*p;
    off_T	filesize = 0;
    പൂർണ്ണസംഖ്യ		skip_read = FALSE;
#ifdef FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_എണ്ണം = 0;     // counter
    char_u	*cryptkey = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		did_ask_for_താക്കോൽ = FALSE;
#endif
#ifdef FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    പൂർണ്ണസംഖ്യ		read_undo_file = FALSE;
#endif
    പൂർണ്ണസംഖ്യ		split = 0;		// number of split lines
#define UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    പൂർണ്ണസംഖ്യ		error = FALSE;		// errors encountered
    പൂർണ്ണസംഖ്യ		ff_error = EOL_UNKNOWN; // file format with errors
    long	linerest = 0;		// remaining chars in line
#ifdef UNIX
    പൂർണ്ണസംഖ്യ		perm = 0;
    പൂർണ്ണസംഖ്യ		swap_mode = -1;		// protection bits for swap file
#അല്ലെങ്കിൽ
    പൂർണ്ണസംഖ്യ		perm;
#endif
    പൂർണ്ണസംഖ്യ		fileformat = 0;		// end-of-line format
    പൂർണ്ണസംഖ്യ		keep_fileformat = FALSE;
    stat_T	st;
    പൂർണ്ണസംഖ്യ		file_readonly;
    linenr_T	skip_എണ്ണം = 0;
    linenr_T	read_എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		സന്ദേശം_save = സന്ദേശം_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    പൂർണ്ണസംഖ്യ		try_mac;
    പൂർണ്ണസംഖ്യ		try_dos;
    പൂർണ്ണസംഖ്യ		try_unix;
    പൂർണ്ണസംഖ്യ		file_rewind = FALSE;
    പൂർണ്ണസംഖ്യ		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    പൂർണ്ണസംഖ്യ		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    പൂർണ്ണസംഖ്യ		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = ശൂന്യ;	// name of 'charconvert' output file
    പൂർണ്ണസംഖ്യ		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    പൂർണ്ണസംഖ്യ		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_next = ശൂന്യ;	// next item in 'fencs' or NULL
    പൂർണ്ണസംഖ്യ		advance_fenc = FALSE;
    long	real_size = 0;
#ifdef USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#endif
    പൂർണ്ണസംഖ്യ		converted = FALSE;	// TRUE if conversion done
    പൂർണ്ണസംഖ്യ		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    പൂർണ്ണസംഖ്യ		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_start;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    പൂർണ്ണസംഖ്യ		using_b_ffname;
    പൂർണ്ണസംഖ്യ		using_b_fname;
    സ്ഥിതം അക്ഷരം *സന്ദേശം_is_a_directory = N_("is a directory");
#ifdef FEAT_CRYPT
    പൂർണ്ണസംഖ്യ		eof = FALSE;
#endif
#ifdef FEAT_SODIUM
    പൂർണ്ണസംഖ്യ		may_need_lseek = FALSE;
#endif
    size_t	fnamelen = 0;

    curbuf->b_au_did_filetype = false; // reset before triggering any autocommands

    curbuf->b_no_eol_lnum = 0;	// in case it was set by the previous read

    /*
     * If there is no file name yet, use the one for the read file.
     * BF_NOTEDITED is set to reflect this.
     * Don't do this for a read from a filter.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    എങ്കിൽ (curbuf->b_ffname == ശൂന്യ
	    && !filtering
	    && fname != ശൂന്യ
	    && vim_strchr(p_cpo, CPO_FNAMER) != ശൂന്യ
	    && !(flags & READ_DUMMY))
    {
	എങ്കിൽ (set_rw_fname(fname, sfname) == FAIL)
	    പോകുക theend;
    }

    // Remember the initial values of curbuf, curbuf->b_ffname and
    // curbuf->b_fname to detect whether they are altered as a result of
    // executing nasty autocommands.  Also check if "fname" and "sfname"
    // point to one of these values.
    old_curbuf = curbuf;
    old_b_ffname = curbuf->b_ffname;
    old_b_fname = curbuf->b_fname;
    using_b_ffname = (fname == curbuf->b_ffname)
					      || (sfname == curbuf->b_ffname);
    using_b_fname = (fname == curbuf->b_fname) || (sfname == curbuf->b_fname);

    // After reading a file the cursor line changes but we don't want to
    // display the line.
    ex_no_reprint = TRUE;

    // don't display the file info for another buffer now
    need_fileinfo = FALSE;

    /*
     * For Unix: Use the short file name whenever possible.
     * Avoids problems with networks and when directory names are changed.
     * Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
     * another directory, which we don't detect.
     */
    എങ്കിൽ (sfname == ശൂന്യ)
	sfname = fname;
#എങ്കിൽ defined(UNIX)
    fname = sfname;
#endif

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    എങ്കിൽ (!filtering && !read_stdin && !read_buffer)
    {
	orig_start = curbuf->b_op_start;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_start.col = 0;

	എങ്കിൽ (newfile)
	{
	    എങ്കിൽ (apply_autocmds_exarg(EVENT_BUFREADCMD, ശൂന്യ, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#ifdef FEAT_EVAL
		എങ്കിൽ (aborting())
		    retval = FAIL;
#endif
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		എങ്കിൽ (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		പോകുക theend;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, ശൂന്യ, eap))
	{
#ifdef FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#അല്ലെങ്കിൽ
	    retval = OK;
#endif
	    പോകുക theend;
	}

	curbuf->b_op_start = orig_start;

	എങ്കിൽ (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    പോകുക theend;
	}
    }

    എങ്കിൽ ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	സന്ദേശം_scroll = FALSE;	// overwrite previous file message
    അല്ലെങ്കിൽ
	സന്ദേശം_scroll = TRUE;	// don't overwrite previous file message

    എങ്കിൽ (fname != ശൂന്യ && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	എങ്കിൽ (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    സന്ദേശം_end();
	    സന്ദേശം_scroll = സന്ദേശം_save;
	    പോകുക theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	എങ്കിൽ (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(സന്ദേശം_is_a_directory), 0);
	    സന്ദേശം_end();
	    സന്ദേശം_scroll = സന്ദേശം_save;
	    retval = NOTDONE;
	    പോകുക theend;
	}
    }
#എങ്കിൽ defined(UNIX) || defined(VMS)
    എങ്കിൽ (!read_stdin && fname != ശൂന്യ)
	perm = mch_getperm(fname);
#endif

    എങ്കിൽ (!read_stdin && !read_buffer && !read_fifo)
    {
#എങ്കിൽ defined(UNIX) || defined(VMS)
	എങ്കിൽ (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
		      && !S_ISFIFO(perm)	    // ... or fifo
		      && !S_ISSOCK(perm)	    // ... or socket
# ifdef OPEN_CHR_FILES
		      && !(S_ISCHR(perm) && is_dev_fd_file(fname))
			// ... or a character special file named /dev/fd/<n>
# endif
						)
	{
	    /*
	     * On Unix it is possible to read a directory, so we have to
	     * check for it before the mch_open().
	     */
	    എങ്കിൽ (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(സന്ദേശം_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    അല്ലെങ്കിൽ
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    സന്ദേശം_end();
	    സന്ദേശം_scroll = സന്ദേശം_save;
	    പോകുക theend;
	}
#endif
#എങ്കിൽ defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	എങ്കിൽ (!p_odev && mch_nodetype(fname) == കണ്ണി_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    സന്ദേശം_end();
	    സന്ദേശം_scroll = സന്ദേശം_save;
	    പോകുക theend;
	}
#endif
    }

    // Set default or forced 'fileformat' and 'binary'.
    set_file_options(set_options, eap);

    /*
     * When opening a new file we take the readonly flag from the file.
     * Default is r/w, can be set to r/o below.
     * Don't reset it when in readonly mode
     * Only set/reset b_p_ro when BF_CHECK_RO is set.
     */
    check_readonly = (newfile && (curbuf->b_flags & BF_CHECK_RO));
    എങ്കിൽ (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    എങ്കിൽ (newfile && !read_stdin && !read_buffer && !read_fifo)
    {
	// Remember time of file.
	എങ്കിൽ (mch_stat((അക്ഷരം *)fname, &st) >= 0)
	{
	    buf_store_time(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
#ifdef FEAT_CRYPT
	    filesize_disk = st.st_size;
#endif
#ifdef UNIX
	    /*
	     * Use the protection bits of the original file for the swap file.
	     * This makes it possible for others to read the name of the
	     * edited file from the swapfile, but only if they can read the
	     * edited file.
	     * Remove the "write" and "execute" bits for group and others
	     * (they must not write the swapfile).
	     * Add the "read" and "write" bits for the user, otherwise we may
	     * not be able to write to the file ourselves.
	     * Setting the bits is done below, after creating the swap file.
	     */
	    swap_mode = (st.st_mode & 0644) | 0600;
#endif
#ifdef VMS
	    curbuf->b_fab_rfm = st.st_fab_rfm;
	    curbuf->b_fab_rat = st.st_fab_rat;
	    curbuf->b_fab_mrs = st.st_fab_mrs;
#endif
	}
	അല്ലെങ്കിൽ
	{
	    curbuf->b_mtime = 0;
	    curbuf->b_mtime_ns = 0;
	    curbuf->b_mtime_read = 0;
	    curbuf->b_mtime_read_ns = 0;
	    curbuf->b_orig_size = 0;
	    curbuf->b_orig_mode = 0;
	}

	// Reset the "new file" flag.  It will be set again below when the
	// file doesn't exist.
	curbuf->b_flags &= ~(BF_NEW | BF_NEW_W);
    }

/*
 * for UNIX: check readonly with perm and mch_access()
 * for Amiga: check readonly by trying to open the file for writing
 */
    file_readonly = FALSE;
    എങ്കിൽ (read_stdin)
    {
#എങ്കിൽ defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_ദ്വിമാന);
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!read_buffer)
    {
#ifdef USE_MCH_ACCESS
	എങ്കിൽ (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((അക്ഷരം *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((അക്ഷരം *)fname, O_RDONLY | O_EXTRA, 0);
#അല്ലെങ്കിൽ
	എങ്കിൽ (!newfile
		|| readonlymode
		|| (fd = mch_open((അക്ഷരം *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((അക്ഷരം *)fname, O_RDONLY | O_EXTRA, 0);
	}
#endif
    }

    എങ്കിൽ (fd < 0)			    // cannot open at all
    {
#ifndef UNIX
	പൂർണ്ണസംഖ്യ	isdir_f;
#endif
	സന്ദേശം_scroll = സന്ദേശം_save;
#ifndef UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	എങ്കിൽ (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(സന്ദേശം_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	അല്ലെങ്കിൽ
#endif
	    എങ്കിൽ (newfile)
	    {
		എങ്കിൽ (perm < 0
#ifdef ENOENT
			&& errno == ENOENT
#endif
		   )
		{
		    /*
		     * Set the 'new-file' flag, so that when the file has
		     * been created by someone else, a ":w" will complain.
		     */
		    curbuf->b_flags |= BF_NEW;

		    // Create a swap file now, so that other Vims are warned
		    // that we are editing this file.  Don't do this for a
		    // "nofile" or "nowrite" buffer type.
		    എങ്കിൽ (!bt_dontwrite(curbuf))
		    {
			check_need_swap(newfile);
			// SwapExists autocommand may mess things up
			എങ്കിൽ (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
			    പോകുക theend;
			}
		    }
		    എങ്കിൽ (dir_of_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_message(), 0);
		    അല്ലെങ്കിൽ
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#ifdef FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#endif
		    // Set forced 'fileencoding'.
		    എങ്കിൽ (eap != ശൂന്യ)
			set_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#എങ്കിൽ defined(FEAT_EVAL)
		    എങ്കിൽ (!aborting())   // autocmds may abort script processing
#endif
			retval = OK;	    // a new file is not an error
		    പോകുക theend;
		}
		അല്ലെങ്കിൽ
		{
		    filemess(curbuf, sfname, (char_u *)(
#ifdef EFBIG
			    (errno == EFBIG) ? _("[File too big]") :
#endif
#ifdef EOVERFLOW
			    (errno == EOVERFLOW) ? _("[File too big]") :
#endif
						_("[Permission Denied]")), 0);
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
		}
	    }

	പോകുക theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    എങ്കിൽ ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    എങ്കിൽ (set_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	എങ്കിൽ (!read_buffer)
	{
	    curbuf->b_p_eof = FALSE;
	    curbuf->b_start_eof = FALSE;
	    curbuf->b_p_eol = TRUE;
	    curbuf->b_start_eol = TRUE;
	}
	curbuf->b_p_bomb = FALSE;
	curbuf->b_start_bomb = FALSE;
    }

    // Create a swap file now, so that other Vims are warned that we are
    // editing this file.
    // Don't do this for a "nofile" or "nowrite" buffer type.
    എങ്കിൽ (!bt_dontwrite(curbuf))
    {
	check_need_swap(newfile);
	എങ്കിൽ (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	    എങ്കിൽ (!read_buffer)
		close(fd);
	    പോകുക theend;
	}
#ifdef UNIX
	// Set swap file protection bits after creating it.
	എങ്കിൽ (swap_mode > 0 && curbuf->b_ml.ml_mfp != ശൂന്യ
			  && curbuf->b_ml.ml_mfp->mf_fname != ശൂന്യ)
	{
	    char_u *swap_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    എങ്കിൽ ((swap_mode & 044) == 040)
	    {
		stat_T	swap_st;

		എങ്കിൽ (mch_stat((അക്ഷരം *)swap_fname, &swap_st) >= 0
			&& st.st_gid != swap_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    swap_mode &= 0600;
	    }

	    (ശൂന്യം)mch_setperm(swap_fname, (long)swap_mode);
	}
#endif
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    എങ്കിൽ (swap_exists_action == SEA_QUIT)
    {
	എങ്കിൽ (!read_buffer && !read_stdin)
	    close(fd);
	പോകുക theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_start = curbuf->b_op_start;
    curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_start.col = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != ശൂന്യ);
    try_dos = (vim_strchr(p_ffs, 'd') != ശൂന്യ);
    try_unix = (vim_strchr(p_ffs, 'x') != ശൂന്യ);

    എങ്കിൽ (!read_buffer)
    {
	പൂർണ്ണസംഖ്യ	m = സന്ദേശം_scroll;
	പൂർണ്ണസംഖ്യ	n = സന്ദേശം_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	എങ്കിൽ (!read_stdin)
	    close(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	സന്ദേശം_scroll = TRUE;
	എങ്കിൽ (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, ശൂന്യ, sfname,
							  FALSE, curbuf, eap);
	അല്ലെങ്കിൽ എങ്കിൽ (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, ശൂന്യ, sfname,
							  FALSE, curbuf, eap);
	അല്ലെങ്കിൽ എങ്കിൽ (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, ശൂന്യ, sfname,
							  FALSE, curbuf, eap);
	അല്ലെങ്കിൽ
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, ശൂന്യ, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != ശൂന്യ);
	try_dos = (vim_strchr(p_ffs, 'd') != ശൂന്യ);
	try_unix = (vim_strchr(p_ffs, 'x') != ശൂന്യ);
	curbuf->b_op_start = orig_start;

	എങ്കിൽ (സന്ദേശം_scrolled == n)
	    സന്ദേശം_scroll = m;

#ifdef FEAT_EVAL
	എങ്കിൽ (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    സന്ദേശം_scroll = സന്ദേശം_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    പോകുക theend;
	}
#endif
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	എങ്കിൽ (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((അക്ഷരം *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    സന്ദേശം_scroll = സന്ദേശം_save;
	    എങ്കിൽ (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    അല്ലെങ്കിൽ
		emsg(_(e_readpre_autocommands_must_not_change_current_buffer));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    പോകുക theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_EMPTY);

    എങ്കിൽ (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	എങ്കിൽ (read_stdin)
	{
	    എങ്കിൽ (!is_not_a_term())
	    {
#ifndef ALWAYS_USE_GUI
# ifdef VIMDLL
		എങ്കിൽ (!gui.in_use)
# endif
		    mch_സന്ദേശം(_("Vim: Reading from stdin...\n"));
#endif
#ifdef FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		എങ്കിൽ (gui.in_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    എങ്കിൽ (p != ശൂന്യ)
		    {
			gui_write(p, (പൂർണ്ണസംഖ്യ)plen);
			vim_free(p);
		    }
		}
#endif
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!read_buffer)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    സന്ദേശം_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_വരി_എണ്ണം;

    // "++bad=" argument.
    എങ്കിൽ (eap != ശൂന്യ && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	എങ്കിൽ (set_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    അല്ലെങ്കിൽ
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    എങ്കിൽ (eap != ശൂന്യ && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_help)
    {
	char_u	    firstline[80];
	പൂർണ്ണസംഖ്യ	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	എങ്കിൽ (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    എങ്കിൽ (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, SEEK_SET);
		വേണ്ടി (p = firstline; p < firstline + len; ++p)
		    എങ്കിൽ (*p >= 0x80)
		    {
			c = TRUE;
			നിർത്തുക;
		    }
	    }
	}

	എങ്കിൽ (c)
	{
	    fenc_next = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    എങ്കിൽ (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    അല്ലെങ്കിൽ
    {
	fenc_next = p_fencs;		// try items in 'fileencodings'
	fenc = next_fenc(&fenc_next, &fenc_alloced);
    }

    /*
     * Jump back here to retry reading the file in different ways.
     * Reasons to retry:
     * - encoding conversion failed: try another one from "fenc_next"
     * - BOM detected and fenc was set, need to setup conversion
     * - "fileformat" check failed: try another
     *
     * Variables set for special retry actions:
     * "file_rewind"	Rewind the file to start reading it again.
     * "advance_fenc"	Advance "fenc" using "fenc_next".
     * "skip_read"	Re-use already read bytes (BOM detected).
     * "did_iconv"	iconv() conversion failed, try 'charconvert'.
     * "keep_fileformat" Don't reset "fileformat".
     *
     * Other status indicators:
     * "tmpname"	When != NULL did conversion with 'charconvert'.
     *			Output file has to be deleted afterwards.
     * "iconv_fd"	When != -1 did conversion with iconv().
     */
retry:

    എങ്കിൽ (file_rewind)
    {
	എങ്കിൽ (read_buffer)
	{
	    read_buf_lnum = 1;
	    read_buf_col = 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    പോകുക failed;
	}
	// Delete the previously read lines.
	ആകുമ്പോൾ (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	എങ്കിൽ (set_options)
	{
	    curbuf->b_p_bomb = FALSE;
	    curbuf->b_start_bomb = FALSE;
	}
	conv_error = 0;
    }

    /*
     * When retrying with another "fenc" and the first time "fileformat"
     * will be reset.
     */
    എങ്കിൽ (keep_fileformat)
	keep_fileformat = FALSE;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (eap != ശൂന്യ && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	അല്ലെങ്കിൽ എങ്കിൽ (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	അല്ലെങ്കിൽ
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#ifdef USE_ICONV
    എങ്കിൽ (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#endif

    എങ്കിൽ (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	എങ്കിൽ (eap != ശൂന്യ && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    എങ്കിൽ (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (fenc_alloced)
		vim_free(fenc);
	    എങ്കിൽ (fenc_next != ശൂന്യ)
	    {
		fenc = next_fenc(&fenc_next, &fenc_alloced);
	    }
	    അല്ലെങ്കിൽ
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	എങ്കിൽ (tmpname != ശൂന്യ)
	{
	    mch_remove(tmpname);		// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    /*
     * Conversion may be required when the encoding of the file is different
     * from 'encoding' or 'encoding' is UTF-16, UCS-2 or UCS-4.
     */
    fio_flags = 0;
    converted = need_conversion(fenc);
    എങ്കിൽ (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	എങ്കിൽ (STRCMP(fenc, ENC_UCSBOM) == 0)
	    fio_flags = FIO_UCSBOM;

	/*
	 * Check if UCS-2/4 or Latin1 to UTF-8 conversion needs to be
	 * done.  This is handled below after read().  Prepare the
	 * fio_flags to avoid having to parse the string each time.
	 * Also check for Unicode to Latin1 conversion, because iconv()
	 * appears not to handle this correctly.  This works just like
	 * conversion to UTF-8 except how the resulting character is put in
	 * the buffer.
	 */
	അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#ifdef MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	എങ്കിൽ (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#endif

#ifdef MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	എങ്കിൽ (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#endif

#ifdef USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	എങ്കിൽ (fio_flags == 0
# ifdef FEAT_EVAL
		&& !did_iconv
# endif
		)
	    iconv_fd = (iconv_t)my_iconv_open(
				  enc_utf8 ? (char_u *)"utf-8" : p_enc, fenc);
#endif

#ifdef FEAT_EVAL
	/*
	 * Use the 'charconvert' expression when conversion is required
	 * and we can't do it internally or with iconv().
	 */
	എങ്കിൽ (fio_flags == 0 && !read_stdin && !read_buffer && *p_ccv != NUL
						    && !read_fifo
# ifdef USE_ICONV
						    && iconv_fd == (iconv_t)-1
# endif
		)
	{
# ifdef USE_ICONV
	    did_iconv = FALSE;
# endif
	    // Skip conversion when it's already done (retry for wrong
	    // "fileformat").
	    എങ്കിൽ (tmpname == ശൂന്യ)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		എങ്കിൽ (tmpname == ശൂന്യ)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    എങ്കിൽ (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			പോകുക failed;
		    }
		    പോകുക retry;
		}
	    }
	}
	അല്ലെങ്കിൽ
#endif
	{
	    എങ്കിൽ (fio_flags == 0
#ifdef USE_ICONV
		    && iconv_fd == (iconv_t)-1
#endif
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		പോകുക retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    എങ്കിൽ (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#ifdef FEAT_CRYPT
	filesize_എണ്ണം = 0;
#endif
	skip_എണ്ണം = വരികൾ_to_skip;
	read_എണ്ണം = വരികൾ_to_read;
	conv_restlen = 0;
#ifdef FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != ശൂന്യ
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_buffer);
	എങ്കിൽ (read_undo_file)
	    sha256_start(&sha_ctx);
#endif
#ifdef FEAT_CRYPT
	എങ്കിൽ (curbuf->b_cryptstate != ശൂന്യ)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = ശൂന്യ;
	}
#endif
    }

    ആകുമ്പോൾ (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	എങ്കിൽ (!skip_read)
	{
#എങ്കിൽ defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		size = SSIZE_MAX;		    // use max I/O size, 52K
#അല്ലെങ്കിൽ
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		size = 0x10000L + linerest;
		എങ്കിൽ (size > 0x100000L)
		    size = 0x100000L;
#endif
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	എങ്കിൽ (size < 0 || size + linerest + 1 < 0 || linerest >= MAXCOL - size)
	{
	    ++split;
	    *ptr = NL;		    // split line by inserting a NL
	    size = 1;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (!skip_read)
	    {
		വേണ്ടി ( ; size >= 10; size = (long)((long_u)size >> 1))
		{
		    എങ്കിൽ ((new_buffer = lalloc(size + linerest + 1,
							      FALSE)) != ശൂന്യ)
			നിർത്തുക;
		}
		എങ്കിൽ (new_buffer == ശൂന്യ)
		{
		    do_outofmem_സന്ദേശം((long_u)(size * 2 + linerest + 1));
		    error = TRUE;
		    നിർത്തുക;
		}
		എങ്കിൽ (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_buffer, ptr - linerest, (size_t)linerest);
		vim_free(buffer);
		buffer = new_buffer;
		ptr = buffer + linerest;
		വരി_start = buffer;

		// May need room to translate into.
		// For iconv() we don't really know the required space, use a
		// factor ICONV_MULT.
		// latin1 to utf-8: 1 byte becomes up to 2 bytes
		// utf-16 to utf-8: 2 bytes become up to 3 bytes, 4 bytes
		// become up to 4 bytes, size must be multiple of 2
		// ucs-2 to utf-8: 2 bytes become up to 3 bytes, size must be
		// multiple of 2
		// ucs-4 to utf-8: 4 bytes become up to 6 bytes, size must be
		// multiple of 4
		real_size = (പൂർണ്ണസംഖ്യ)size;
#ifdef USE_ICONV
		എങ്കിൽ (iconv_fd != (iconv_t)-1)
		    size = size / ICONV_MULT;
		അല്ലെങ്കിൽ
#endif
		    എങ്കിൽ (fio_flags & FIO_LATIN1)
		    size = size / 2;
		അല്ലെങ്കിൽ എങ്കിൽ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    size = (size * 2 / 3) & ~1;
		അല്ലെങ്കിൽ എങ്കിൽ (fio_flags & FIO_UCS4)
		    size = (size * 2 / 3) & ~3;
		അല്ലെങ്കിൽ എങ്കിൽ (fio_flags == FIO_UCSBOM)
		    size = size / ICONV_MULT;	// worst case
#ifdef MSWIN
		അല്ലെങ്കിൽ എങ്കിൽ (fio_flags & FIO_CODEPAGE)
		    size = size / ICONV_MULT;	// also worst case
#endif
#ifdef MACOS_CONVERT
		അല്ലെങ്കിൽ എങ്കിൽ (fio_flags & FIO_MACROMAN)
		    size = size / ICONV_MULT;	// also worst case
#endif

		എങ്കിൽ (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    size -= conv_restlen;
		}

		എങ്കിൽ (read_buffer)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    എങ്കിൽ (read_buf_lnum > from)
			size = 0;
		    അല്ലെങ്കിൽ
		    {
			പൂർണ്ണസംഖ്യ	n, ni;
			long	tlen;

			tlen = 0;
			വേണ്ടി (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_col;
			    n = ml_get_len(read_buf_lnum) - read_buf_col;
			    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)tlen + n + 1 > size)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (പൂർണ്ണസംഖ്യ)(size - tlen);
				വേണ്ടി (ni = 0; ni < n; ++ni)
				{
				    എങ്കിൽ (p[ni] == NL)
					ptr[tlen++] = NUL;
				    അല്ലെങ്കിൽ
					ptr[tlen++] = p[ni];
				}
				read_buf_col += n;
				നിർത്തുക;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    വേണ്ടി (ni = 0; ni < n; ++ni)
			    {
				എങ്കിൽ (p[ni] == NL)
				    ptr[tlen++] = NUL;
				അല്ലെങ്കിൽ
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_col = 0;
			    എങ്കിൽ (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				എങ്കിൽ (!curbuf->b_p_eol)
				    --tlen;
				size = tlen;
#ifdef FEAT_CRYPT
				eof = TRUE;
#endif
				നിർത്തുക;
			    }
			}
		    }
		}
		അല്ലെങ്കിൽ
		{
		    /*
		     * Read bytes from the file.
		     */
#ifdef FEAT_SODIUM
		    // Let the crypt layer work with a buffer size of 8192
		    //
		    // Sodium encryption requires a fixed block size to
		    // successfully decrypt. However, unfortunately the file
		    // header size changes between xchacha20 and xchacha20v2 by
		    // 'add_len' bytes.
		    // So we will now read the maximum header size + encryption
		    // metadata, but after determining to read an xchacha20
		    // encrypted file, we have to rewind the file descriptor by
		    // 'add_len' bytes in the second round.
		    //
		    // Be careful with changing it, it needs to stay the same
		    // for reading back previously encrypted files!
		    എങ്കിൽ (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			size = WRITEBUFSIZE + crypt_get_max_header_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    അല്ലെങ്കിൽ എങ്കിൽ (filesize > 0 && (curbuf->b_cryptstate != ശൂന്യ
				&& crypt_method_is_sodium(
					     curbuf->b_cryptstate->method_nr)))
		    {
			size = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			എങ്കിൽ (curbuf->b_cryptstate->method_nr == CRYPT_M_SOD
						     && !eof && may_need_lseek)
			{
			    lseek(fd, crypt_get_header_len(
					       curbuf->b_cryptstate->method_nr)
				       - crypt_get_max_header_len(), SEEK_CUR);
			    may_need_lseek = FALSE;
			}
		    }
#endif
		    long read_size = size;
		    size = read_eintr(fd, ptr, read_size);
#ifdef FEAT_CRYPT
		    // Did we reach end of file?
		    filesize_എണ്ണം += size;
		    eof = (size < read_size || filesize_എണ്ണം == filesize_disk);
#endif
		}

#ifdef FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		എങ്കിൽ (filesize == 0 && size > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &size,
						  &filesize, newfile, sfname,
						  &did_ask_for_താക്കോൽ);
# എങ്കിൽ defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    എങ്കിൽ (curbuf->b_cryptstate != ശൂന്യ
				 && !crypt_works_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		എങ്കിൽ (cryptkey != ശൂന്യ && curbuf->b_cryptstate != ശൂന്യ
								   && size > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    എങ്കിൽ (crypt_works_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    size, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    അല്ലെങ്കിൽ
		    {
			char_u	*newptr = ശൂന്യ;
			പൂർണ്ണസംഖ്യ	decrypted_size;

			decrypted_size = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, size,
								 &newptr, eof);

			എങ്കിൽ (decrypted_size < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    നിർത്തുക;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			എങ്കിൽ (decrypted_size == 0)
			    തുടരുക;

			എങ്കിൽ (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_buffer = newptr;
			}
			അല്ലെങ്കിൽ
			{
			    long_u	new_size;

			    // Need new buffer to add bytes carried over.
			    new_size = (long_u)(decrypted_size + linerest + 1);
			    new_buffer = lalloc(new_size, FALSE);
			    എങ്കിൽ (new_buffer == ശൂന്യ)
			    {
				do_outofmem_സന്ദേശം(new_size);
				error = TRUE;
				നിർത്തുക;
			    }

			    mch_memmove(new_buffer, buffer, linerest + conv_restlen);
			    എങ്കിൽ (newptr != ശൂന്യ)
				mch_memmove(new_buffer + linerest + conv_restlen,
					newptr, decrypted_size);
			    vim_free(newptr);
			}

			എങ്കിൽ (new_buffer != ശൂന്യ)
			{
			    vim_free(buffer);
			    buffer = new_buffer;
			    new_buffer = ശൂന്യ;
			    വരി_start = buffer;
			    ptr = buffer + linerest + conv_restlen;
			    real_size = size;
			}
			size = decrypted_size;
		    }
# endif
		}
#endif

		എങ്കിൽ (size <= 0)
		{
		    എങ്കിൽ (size < 0)		    // read error
			error = TRUE;
		    അല്ലെങ്കിൽ എങ്കിൽ (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			എങ്കിൽ (fio_flags != 0
#ifdef USE_ICONV
				|| iconv_fd != (iconv_t)-1
#endif
			   )
			{
			    എങ്കിൽ (can_retry)
				പോകുക rewind_retry;
			    എങ്കിൽ (conv_error == 0)
				conv_error = curbuf->b_ml.ml_വരി_എണ്ണം
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			അല്ലെങ്കിൽ എങ്കിൽ (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_വരി_എണ്ണം
								- linecnt + 1;
			എങ്കിൽ (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			അല്ലെങ്കിൽ
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    എങ്കിൽ (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#ifdef USE_ICONV
				    || iconv_fd != (iconv_t)-1
#endif
			       ))
			    {
				ആകുമ്പോൾ (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#ifdef USE_ICONV
			    എങ്കിൽ (iconv_fd != (iconv_t)-1)
			    {
				iconv_close(iconv_fd);
				iconv_fd = (iconv_t)-1;
			    }
#endif
			}
		    }
		}
	    }
	    skip_read = FALSE;

	    /*
	     * At start of file (or after crypt magic number): Check for BOM.
	     * Also check for a BOM for other Unicode encodings, but not after
	     * converting with 'charconvert' or when a BOM has already been
	     * found.
	     */
	    എങ്കിൽ ((filesize == 0
#ifdef FEAT_CRYPT
		   || (cryptkey != ശൂന്യ
			&& filesize == crypt_get_header_len(
						 crypt_get_method_nr(curbuf)))
#endif
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == ശൂന്യ
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		പൂർണ്ണസംഖ്യ	blen;

		// no BOM detection in a short file or in binary mode
		എങ്കിൽ (size < 2 || curbuf->b_p_bin)
		    ccname = ശൂന്യ;
		അല്ലെങ്കിൽ
		    ccname = check_for_bom(ptr, size, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		എങ്കിൽ (ccname != ശൂന്യ)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    size -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)size);
		    എങ്കിൽ (set_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_start_bomb = TRUE;
		    }
		}

		എങ്കിൽ (fio_flags == FIO_UCSBOM)
		{
		    എങ്കിൽ (ccname == ശൂന്യ)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    അല്ലെങ്കിൽ
		    {
			// BOM detected: set "fenc" and jump back
			എങ്കിൽ (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    പോകുക retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    size += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    എങ്കിൽ (size <= 0)
		നിർത്തുക;


#ifdef USE_ICONV
	    എങ്കിൽ (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		സ്ഥിരം അക്ഷരം	*fromp;
		അക്ഷരം		*മുകൾ;
		size_t		from_size;
		size_t		to_size;

		fromp = (അക്ഷരം *)ptr;
		from_size = size;
		ptr += size;
		മുകൾ = (അക്ഷരം *)ptr;
		to_size = real_size - size;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		ആകുമ്പോൾ ((iconv(iconv_fd, (ശൂന്യം *)&fromp, &from_size,
							       &മുകൾ, &to_size)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_size > CONV_RESTLEN)
		{
		    എങ്കിൽ (can_retry)
			പോകുക rewind_retry;
		    എങ്കിൽ (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)മുകൾ);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_size;
		    എങ്കിൽ (bad_char_behavior == BAD_KEEP)
		    {
			*മുകൾ++ = *(fromp - 1);
			--to_size;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (bad_char_behavior != BAD_DROP)
		    {
			*മുകൾ++ = bad_char_behavior;
			--to_size;
		    }
		}

		എങ്കിൽ (from_size > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_size);
		    conv_restlen = (പൂർണ്ണസംഖ്യ)from_size;
		}

		// move the linerest to before the converted characters
		വരി_start = ptr - linerest;
		mch_memmove(വരി_start, buffer, (size_t)linerest);
		size = (long)((char_u *)മുകൾ - ptr);
	    }
#endif

#ifdef MSWIN
	    എങ്കിൽ (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		പൂർണ്ണസംഖ്യ	ucs2len;
		പൂർണ്ണസംഖ്യ	codepage = FIO_GET_CP(fio_flags);
		പൂർണ്ണസംഖ്യ	bytelen;
		പൂർണ്ണസംഖ്യ	found_bad;
		അക്ഷരം	replstr[2];

		/*
		 * Conversion from an MS-Windows codepage or UTF-8 to UTF-8 or
		 * a codepage, using standard MS-Windows functions.  This
		 * requires two steps:
		 * 1. convert from 'fileencoding' to ucs-2
		 * 2. convert from ucs-2 to 'encoding'
		 *
		 * Because there may be illegal bytes AND an incomplete byte
		 * sequence at the end, we may have to do the conversion one
		 * character at a time to get it right.
		 */

		// Replacement string for WideCharToMultiByte().
		എങ്കിൽ (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		അല്ലെങ്കിൽ
		    replstr[0] = '?';
		replstr[1] = NUL;

		/*
		 * Move the bytes to the end of the buffer, so that we have
		 * room to put the result at the start.
		 */
		src = ptr + real_size - size;
		mch_memmove(src, ptr, size);

		/*
		 * Do the conversion.
		 */
		dst = ptr;
		ആകുമ്പോൾ (size > 0)
		{
		    found_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    എങ്കിൽ (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (പൂർണ്ണസംഖ്യ)utf_ptr2len_len(src, size);
			എങ്കിൽ (bytelen > size)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    എങ്കിൽ (bytelen <= CONV_RESTLEN)
				നിർത്തുക;

			    // weird overlong byte sequence
			    bytelen = size;
			    found_bad = TRUE;
			}
			അല്ലെങ്കിൽ
			{
			    പൂർണ്ണസംഖ്യ	    u8c = utf_ptr2char(src);

			    എങ്കിൽ (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				found_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    അല്ലെങ്കിൽ
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			വേണ്ടി (bytelen = 1; bytelen <= size && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = MultiByteToWideChar(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    എങ്കിൽ (ucs2len > 0)
				നിർത്തുക;
			}
			എങ്കിൽ (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    എങ്കിൽ (size == 1)
				നിർത്തുക;
			    found_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    എങ്കിൽ (!found_bad)
		    {
			പൂർണ്ണസംഖ്യ	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			എങ്കിൽ (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    വേണ്ടി (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			അല്ലെങ്കിൽ
			{
			    BOOL	bad = FALSE;
			    പൂർണ്ണസംഖ്യ		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = WideCharToMultiByte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (പൂർണ്ണസംഖ്യ)(src - dst),
				    replstr, &bad);
			    എങ്കിൽ (bad)
				found_bad = TRUE;
			    അല്ലെങ്കിൽ
				dst += dstlen;
			}
		    }

		    എങ്കിൽ (found_bad)
		    {
			// Deal with bytes we can't convert.
			എങ്കിൽ (can_retry)
			    പോകുക rewind_retry;
			എങ്കിൽ (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			എങ്കിൽ (bad_char_behavior != BAD_DROP)
			{
			    എങ്കിൽ (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    അല്ലെങ്കിൽ
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    size -= bytelen;
		}

		എങ്കിൽ (size > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, size);
		    conv_restlen = size;
		}

		// The new size is equal to how much "dst" was advanced.
		size = (long)(dst - ptr);
	    }
	    അല്ലെങ്കിൽ
#endif
#ifdef MACOS_CONVERT
	    എങ്കിൽ (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		എങ്കിൽ (macroman2enc(ptr, &size, real_size) == FAIL)
		    പോകുക rewind_retry;
	    }
	    അല്ലെങ്കിൽ
#endif
	    എങ്കിൽ (fio_flags != 0)
	    {
		പൂർണ്ണസംഖ്യ	u8c;
		char_u	*dest;
		char_u	*tail = ശൂന്യ;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_size;
		എങ്കിൽ (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + size;
		    എങ്കിൽ (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			tail = ptr + size - 1;
			ആകുമ്പോൾ (tail > ptr && (*tail & 0xc0) == 0x80)
			    --tail;
			എങ്കിൽ (tail + utf_byte2len(*tail) <= ptr + size)
			    tail = ശൂന്യ;
			അല്ലെങ്കിൽ
			    p = tail;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (size & ~1);
		    എങ്കിൽ (size & 1)
			tail = p;
		    എങ്കിൽ ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			എങ്കിൽ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			അല്ലെങ്കിൽ
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			എങ്കിൽ (u8c >= 0xd800 && u8c <= 0xdbff)
			    tail = p;
			അല്ലെങ്കിൽ
			    p += 2;
		    }
		}
		അല്ലെങ്കിൽ //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (size & ~3);
		    എങ്കിൽ (size & 3)
			tail = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		എങ്കിൽ (tail != ശൂന്യ)
		{
		    conv_restlen = (പൂർണ്ണസംഖ്യ)((ptr + size) - tail);
		    mch_memmove(conv_rest, (char_u *)tail, conv_restlen);
		    size -= conv_restlen;
		}


		ആകുമ്പോൾ (p > ptr)
		{
		    എങ്കിൽ (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    അല്ലെങ്കിൽ എങ്കിൽ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			എങ്കിൽ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			അല്ലെങ്കിൽ
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			എങ്കിൽ ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    പൂർണ്ണസംഖ്യ u16c;

			    എങ്കിൽ (p == ptr)
			    {
				// Missing leading word.
				എങ്കിൽ (can_retry)
				    പോകുക rewind_retry;
				എങ്കിൽ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				എങ്കിൽ (bad_char_behavior == BAD_DROP)
				    തുടരുക;
				എങ്കിൽ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    എങ്കിൽ (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    അല്ലെങ്കിൽ
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    എങ്കിൽ (u16c < 0xd800 || u16c > 0xdbff)
			    {
				എങ്കിൽ (can_retry)
				    പോകുക rewind_retry;
				എങ്കിൽ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				എങ്കിൽ (bad_char_behavior == BAD_DROP)
				    തുടരുക;
				എങ്കിൽ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (fio_flags & FIO_UCS4)
		    {
			എങ്കിൽ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (unsigned)*--p << 24;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 8;
			    u8c += *--p;
			}
			അല്ലെങ്കിൽ	// big endian
			{
			    u8c = *--p;
			    u8c += (unsigned)*--p << 8;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 24;
			}
		    }
		    അല്ലെങ്കിൽ    // UTF-8
		    {
			എങ്കിൽ (*--p < 0x80)
			    u8c = *p;
			അല്ലെങ്കിൽ
			{
			    len = utf_head_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    എങ്കിൽ (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				എങ്കിൽ (can_retry)
				    പോകുക rewind_retry;
				എങ്കിൽ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				എങ്കിൽ (bad_char_behavior == BAD_DROP)
				    തുടരുക;
				എങ്കിൽ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    എങ്കിൽ (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(ശൂന്യം)utf_char2bytes(u8c, dest);
		    }
		    അല്ലെങ്കിൽ		// produce Latin1
		    {
			--dest;
			എങ്കിൽ (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    എങ്കിൽ (can_retry)
				പോകുക rewind_retry;
			    എങ്കിൽ (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    എങ്കിൽ (bad_char_behavior == BAD_DROP)
				++dest;
			    അല്ലെങ്കിൽ എങ്കിൽ (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    അല്ലെങ്കിൽ എങ്കിൽ (eap != ശൂന്യ && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    അല്ലെങ്കിൽ
				*dest = 0xBF;
			}
			അല്ലെങ്കിൽ
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		വരി_start = dest - linerest;
		mch_memmove(വരി_start, buffer, (size_t)linerest);
		size = (long)((ptr + real_size) - dest);
		ptr = dest;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8 && !curbuf->b_p_bin)
	    {
		പൂർണ്ണസംഖ്യ  incomplete_tail = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		വേണ്ടി (p = ptr; ; )
		{
		    പൂർണ്ണസംഖ്യ	 todo;
		    പൂർണ്ണസംഖ്യ	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_end = ptr + size;
			ആകുമ്പോൾ (ascii_end - p >= (long)വലുപ്പം(long_u))
			{
			    long_u word;
			    memcpy(&word, p, വലുപ്പം(long_u));
			    എങ്കിൽ (word & NONASCII_MASK)
				നിർത്തുക;
			    p += വലുപ്പം(long_u);
			}
			ആകുമ്പോൾ (p < ascii_end && *p < 0x80)
			    ++p;
		    }

		    todo = (പൂർണ്ണസംഖ്യ)((ptr + size) - p);
		    എങ്കിൽ (todo <= 0)
			നിർത്തുക;
		    എങ്കിൽ (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			എങ്കിൽ (l > todo && !incomplete_tail)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    എങ്കിൽ (p > ptr || filesize > 0)
				incomplete_tail = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    എങ്കിൽ (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				size -= conv_restlen;
				നിർത്തുക;
			    }
			}
			എങ്കിൽ (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    എങ്കിൽ (can_retry && !incomplete_tail)
				നിർത്തുക;
#ifdef USE_ICONV
			    // When we did a conversion report an error.
			    എങ്കിൽ (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#endif
			    // Remember the first linenr with an illegal byte
			    എങ്കിൽ (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    എങ്കിൽ (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--size;
			    }
			    അല്ലെങ്കിൽ
			    {
				എങ്കിൽ (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			അല്ലെങ്കിൽ
			    p += l;
		    }
		}
		എങ്കിൽ (p < ptr + size && !incomplete_tail)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#എങ്കിൽ defined(FEAT_EVAL) && defined(USE_ICONV)
		    എങ്കിൽ (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    അല്ലെങ്കിൽ
#endif
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    പോകുക retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += size;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    എങ്കിൽ (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		എങ്കിൽ (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    എങ്കിൽ (try_mac)
			try_mac = 1;

		    വേണ്ടി (p = ptr; p < ptr + size; ++p)
		    {
			എങ്കിൽ (*p == NL)
			{
			    എങ്കിൽ (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    അല്ലെങ്കിൽ
				fileformat = EOL_UNIX;
			    നിർത്തുക;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    എങ്കിൽ (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			വേണ്ടി (; p >= ptr && *p != CAR; p--)
			    ;
			എങ്കിൽ (p >= ptr)
			{
			    വേണ്ടി (p = ptr; p < ptr + size; ++p)
			    {
				എങ്കിൽ (*p == NL)
				    try_unix++;
				അല്ലെങ്കിൽ എങ്കിൽ (*p == CAR)
				    try_mac++;
			    }
			    എങ്കിൽ (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		എങ്കിൽ (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		എങ്കിൽ (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		എങ്കിൽ (set_options)
		    set_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	എങ്കിൽ (fileformat == EOL_MAC)
	{
	    --ptr;
	    ആകുമ്പോൾ (++ptr, --size >= 0)
	    {
		// catch most common case first
		എങ്കിൽ ((c = *ptr) != NUL && c != CAR && c != NL)
		    തുടരുക;
		എങ്കിൽ (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		അല്ലെങ്കിൽ എങ്കിൽ (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (skip_എണ്ണം == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - വരി_start + 1);
			എങ്കിൽ (ml_append(lnum, വരി_start, len, newfile) == FAIL)
			{
			    error = TRUE;
			    നിർത്തുക;
			}
#ifdef FEAT_PERSISTENT_UNDO
			എങ്കിൽ (read_undo_file)
			    sha256_update(&sha_ctx, വരി_start, len);
#endif
			++lnum;
			എങ്കിൽ (--read_എണ്ണം == 0)
			{
			    error = TRUE;	// break loop
			    വരി_start = ptr;	// nothing left to write
			    നിർത്തുക;
			}
		    }
		    അല്ലെങ്കിൽ
			--skip_എണ്ണം;
		    വരി_start = ptr + 1;
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *end = ptr + size;

	    ആകുമ്പോൾ (ptr < end)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, end - ptr);
		char_u *nul_scan;

		എങ്കിൽ (nl == ശൂന്യ)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    ആകുമ്പോൾ ((nul_scan = (char_u *)memchr(ptr, NUL,
						      end - ptr)) != ശൂന്യ)
		    {
			*nul_scan = NL;
			ptr = nul_scan + 1;
		    }
		    ptr = end;
		    നിർത്തുക;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *scan = ptr;
		    ആകുമ്പോൾ ((nul_scan = (char_u *)memchr(scan, NUL,
						       nl - scan)) != ശൂന്യ)
		    {
			*nul_scan = NL;
			scan = nul_scan + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		എങ്കിൽ (skip_എണ്ണം == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - വരി_start + 1);
		    എങ്കിൽ (fileformat == EOL_DOS)
		    {
			എങ്കിൽ (ptr > വരി_start && ptr[-1] == CAR)
			{
			    // remove CR before NL
			    ptr[-1] = NUL;
			    --len;
			}
			/*
			 * Reading in Dos format, but no CR-LF found!
			 * When 'fileformats' includes "unix", delete all
			 * the lines read so far and start all over again.
			 * Otherwise give an error message later.
			 */
			അല്ലെങ്കിൽ എങ്കിൽ (ff_error != EOL_DOS)
			{
			    എങ്കിൽ (   try_unix
				&& !read_stdin
				&& (read_buffer
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				എങ്കിൽ (set_options)
				    set_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				പോകുക retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    എങ്കിൽ (ml_append(lnum, വരി_start, len, newfile) == FAIL)
		    {
			error = TRUE;
			നിർത്തുക;
		    }
#ifdef FEAT_PERSISTENT_UNDO
		    എങ്കിൽ (read_undo_file)
			sha256_update(&sha_ctx, വരി_start, len);
#endif
		    ++lnum;
		    എങ്കിൽ (--read_എണ്ണം == 0)
		    {
			error = TRUE;	    // break loop
			വരി_start = ptr;   // nothing left to write
			നിർത്തുക;
		    }
		}
		അല്ലെങ്കിൽ
		    --skip_എണ്ണം;
		വരി_start = ptr + 1;
		++ptr;
	    }
	    size = -1;
	}
	linerest = (long)(ptr - വരി_start);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    എങ്കിൽ (error && read_എണ്ണം == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    എങ്കിൽ (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	എങ്കിൽ (set_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    എങ്കിൽ (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	എങ്കിൽ (set_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - വരി_start + 1);
	എങ്കിൽ (ml_append(lnum, വരി_start, len, newfile) == FAIL)
	    error = TRUE;
	അല്ലെങ്കിൽ
	{
#ifdef FEAT_PERSISTENT_UNDO
	    എങ്കിൽ (read_undo_file)
		sha256_update(&sha_ctx, വരി_start, len);
#endif
	    read_no_eol_lnum = ++lnum;
	}
    }

    എങ്കിൽ (set_options)
	save_file_ff(curbuf);		// remember the current file format

#ifdef FEAT_CRYPT
    എങ്കിൽ (curbuf->b_cryptstate != ശൂന്യ)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = ശൂന്യ;
    }
    എങ്കിൽ (cryptkey != ശൂന്യ && cryptkey != curbuf->b_p_താക്കോൽ)
	crypt_free_താക്കോൽ(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#endif

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    എങ്കിൽ (set_options)
	set_string_option_direct((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    എങ്കിൽ (fenc_alloced)
	vim_free(fenc);
#ifdef USE_ICONV
    എങ്കിൽ (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#endif

    എങ്കിൽ (!read_buffer && !read_stdin)
	close(fd);				// errors are ignored
#ifdef HAVE_FD_CLOEXEC
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ fdflags = fcntl(fd, F_GETFD);

	എങ്കിൽ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (ശൂന്യം)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#endif
    vim_free(buffer);

#ifdef HAVE_DUP
    എങ്കിൽ (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#endif

    എങ്കിൽ (tmpname != ശൂന്യ)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    എങ്കിൽ (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	എങ്കിൽ (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	{
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#endif
	    ml_delete(curbuf->b_ml.ml_വരി_എണ്ണം);
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#endif
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_വരി_എണ്ണം - linecnt;
	എങ്കിൽ (filesize == 0)
	    linecnt = 0;
	എങ്കിൽ (newfile || read_buffer)
	{
	    redraw_curbuf_later(UPD_NOT_VALID);
#ifdef FEAT_DIFF
	    // After reading the text into the buffer the diff info needs to
	    // be updated.
	    diff_invalidate(curbuf);
#endif
#ifdef FEAT_FOLDING
	    // All folds in the window are invalid now.  Mark them for update
	    // before triggering autocommands.
	    foldUpdateAll(curwin);
#endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (linecnt)		// appended at least one line
	    appended_വരികൾ_mark(from, linecnt);

#ifndef ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	എങ്കിൽ (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#endif

	എങ്കിൽ (got_int)
	{
	    എങ്കിൽ (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		എങ്കിൽ (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    സന്ദേശം_scroll = സന്ദേശം_save;
#ifdef FEAT_VIMINFO
	    check_marks_read();
#endif
	    retval = OK;	// an interrupt isn't really an error
	    പോകുക theend;
	}

	എങ്കിൽ (!filtering && !(flags & READ_DUMMY))
	{
	    പൂർണ്ണസംഖ്യ	buflen;

	    സന്ദേശം_add_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (പൂർണ്ണസംഖ്യ)STRLEN(IObuff);
#ifdef UNIX
	    എങ്കിൽ (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    എങ്കിൽ (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    എങ്കിൽ (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#endif
	    എങ്കിൽ (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    എങ്കിൽ (read_no_eol_lnum)
	    {
		സന്ദേശം_add_eol();
		c = TRUE;
	    }
	    എങ്കിൽ (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    എങ്കിൽ (split)
	    {
		buflen += vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    എങ്കിൽ (notconverted)
	    {
		buflen += vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (converted)
	    {
		buflen += vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#ifdef FEAT_CRYPT
	    എങ്കിൽ (cryptkey != ശൂന്യ)
	    {
		crypt_append_സന്ദേശം(curbuf);
		c = TRUE;
	    }
#endif
	    എങ്കിൽ (conv_error != 0)
	    {
		vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (long)conv_error);
		c = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (illegal_byte > 0)
	    {
		vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (long)illegal_byte);
		c = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (error)
	    {
		vim_snprintf((അക്ഷരം *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    എങ്കിൽ (സന്ദേശം_add_fileformat(fileformat))
		c = TRUE;
#ifdef FEAT_CRYPT
	    എങ്കിൽ (cryptkey != ശൂന്യ)
		സന്ദേശം_add_വരികൾ(c, (long)linecnt, filesize
			 - crypt_get_header_len(crypt_get_method_nr(curbuf)));
	    അല്ലെങ്കിൽ
#endif
		സന്ദേശം_add_വരികൾ(c, (long)linecnt, filesize);

	    VIM_CLEAR(keep_സന്ദേശം);
	    സന്ദേശം_scrolled_ign = TRUE;
#ifdef ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    എങ്കിൽ (read_stdin || read_buffer)
		p = സന്ദേശം_may_trunc(FALSE, IObuff);
	    അല്ലെങ്കിൽ
#endif
	    {
		എങ്കിൽ (സന്ദേശം_col > 0)
		    സന്ദേശം_putchar('\r');  // overwrite previous message
		p = (char_u *)സന്ദേശം_trunc_attr((അക്ഷരം *)IObuff, FALSE, 0);
	    }
	    എങ്കിൽ (read_stdin || read_buffer || restart_edit != 0
		    || (സന്ദേശം_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		set_keep_സന്ദേശം(p, 0);
	    സന്ദേശം_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	എങ്കിൽ (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	എങ്കിൽ (exmode_active)
	    curwin->w_cursor.lnum = from + linecnt;
	അല്ലെങ്കിൽ
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set '[ and '] marks to the newly read lines.
	    curbuf->b_op_start.lnum = from + 1;
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.lnum = from + linecnt;
	    curbuf->b_op_end.col = 0;
	}

#ifdef MSWIN
	/*
	 * Work around a weird problem: When a file has two links (only
	 * possible on NTFS) and we write through one link, then stat() it
	 * through the other link, the timestamp information may be wrong.
	 * It's correct again after reading the file, thus reset the timestamp
	 * here.
	 */
	എങ്കിൽ (newfile && !read_stdin && !read_buffer
					 && mch_stat((അക്ഷരം *)fname, &st) >= 0)
	{
	    buf_store_time(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#endif
    }
    സന്ദേശം_scroll = സന്ദേശം_save;

#ifdef FEAT_VIMINFO
    /*
     * Get the marks before executing autocommands, so they can be used there.
     */
    check_marks_read();
#endif

    /*
     * We remember if the last line of the read didn't have
     * an eol even when 'binary' is off, to support turning 'fixeol' off,
     * or writing the read again with 'binary' on.  The latter is required
     * for ":autocmd FileReadPost *.gz set bin|'[,']!gunzip" to work.
     */
    curbuf->b_no_eol_lnum = read_no_eol_lnum;

    // When reloading a buffer put the cursor at the first line that is
    // different.
    എങ്കിൽ (flags & READ_KEEP_UNDO)
	u_find_first_changed();

#ifdef FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    എങ്കിൽ (read_undo_file)
    {
	char_u	hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_read_undo(ശൂന്യ, hash, fname);
    }
#endif

    എങ്കിൽ (!read_stdin && !read_fifo && (!read_buffer || sfname != ശൂന്യ))
    {
	പൂർണ്ണസംഖ്യ m = സന്ദേശം_scroll;
	പൂർണ്ണസംഖ്യ n = സന്ദേശം_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	എങ്കിൽ (set_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	സന്ദേശം_scroll = TRUE;
	എങ്കിൽ (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, ശൂന്യ, sfname,
							  FALSE, curbuf, eap);
	അല്ലെങ്കിൽ എങ്കിൽ (newfile || (read_buffer && sfname != ശൂന്യ))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, ശൂന്യ, sfname,
							  FALSE, curbuf, eap);
	    എങ്കിൽ (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	അല്ലെങ്കിൽ
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, ശൂന്യ, eap);
	എങ്കിൽ (സന്ദേശം_scrolled == n)
	    സന്ദേശം_scroll = m;
#ifdef FEAT_EVAL
	എങ്കിൽ (aborting())	    // autocmds may abort script processing
	    പോകുക theend;
#endif
    }

    എങ്കിൽ (!(recoverymode && error))
	retval = OK;

theend:
    എങ്കിൽ (curbuf->b_ml.ml_mfp != ശൂന്യ
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    തിരികെ retval;
}

#എങ്കിൽ defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    പൂർണ്ണസംഖ്യ
is_dev_fd_file(char_u *fname)
{
    തിരികെ (STRNCMP(fname, "/dev/fd/", 8) == 0
	    && VIM_ISDIGIT(fname[8])
	    && *skipdigits(fname + 9) == NUL
	    && (fname[9] != NUL
		|| (fname[8] != '0' && fname[8] != '1' && fname[8] != '2')));
}
#endif

/*
 * From the current line count and characters read after that, estimate the
 * line number where we are now.
 * Used for error messages that include a line number.
 */
    സ്ഥിതം linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_വരി_എണ്ണം - linecnt + 1;
    വേണ്ടി (s = p; s < endp; ++s)
	എങ്കിൽ (*s == '\n')
	    ++lnum;
    തിരികെ lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (unsigned)STRLEN(buf->b_p_fenc));
    എങ്കിൽ (eap->cmd == ശൂന്യ)
	തിരികെ FAIL;

    sprintf((അക്ഷരം *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    തിരികെ OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    ശൂന്യം
set_file_options(പൂർണ്ണസംഖ്യ set_options, exarg_T *eap)
{
    // set default 'fileformat'
    എങ്കിൽ (set_options)
    {
	എങ്കിൽ (eap != ശൂന്യ && eap->force_ff != 0)
	    set_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	അല്ലെങ്കിൽ എങ്കിൽ (*p_ffs != NUL)
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    എങ്കിൽ (eap != ശൂന്യ && eap->force_bin != 0)
    {
	പൂർണ്ണസംഖ്യ	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	set_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    ശൂന്യം
set_forced_fenc(exarg_T *eap)
{
    എങ്കിൽ (eap->force_enc == 0)
	തിരികെ;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    എങ്കിൽ (fenc != ശൂന്യ)
	set_string_option_direct((char_u *)"fenc", -1,
		fenc, OPT_FREE|OPT_LOCAL, 0);
    vim_free(fenc);
}

/*
 * Find next fileencoding to use from 'fileencodings'.
 * "pp" points to fenc_next.  It's advanced to the next item.
 * When there are no more items, an empty string is returned and *pp is set to
 * NULL.
 * When *pp is not set to NULL, the result is in allocated memory and "alloced"
 * is set to TRUE.
 */
    സ്ഥിതം char_u *
next_fenc(char_u **pp, പൂർണ്ണസംഖ്യ *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    എങ്കിൽ (**pp == NUL)
    {
	*pp = ശൂന്യ;
	തിരികെ (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    എങ്കിൽ (p == ശൂന്യ)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    അല്ലെങ്കിൽ
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	എങ്കിൽ (r != ശൂന്യ)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    എങ്കിൽ (r != ശൂന്യ)
	*alloced = TRUE;
    അല്ലെങ്കിൽ
    {
	// out of memory
	r = (char_u *)"";
	*pp = ശൂന്യ;
    }
    തിരികെ r;
}

#ifdef FEAT_EVAL
/*
 * Convert a file with the 'charconvert' expression.
 * This closes the file which is to be read, converts it and opens the
 * resulting file for reading.
 * Returns name of the resulting converted file (the caller should delete it
 * after reading it).
 * Returns NULL if the conversion failed ("*fdp" is not set) .
 */
    സ്ഥിതം char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    പൂർണ്ണസംഖ്യ		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    അക്ഷരം	*errmsg = ശൂന്യ;

    tmpname = vim_tempname('r', FALSE);
    എങ്കിൽ (tmpname == ശൂന്യ)
	errmsg = _("Can't find temp file for conversion");
    അല്ലെങ്കിൽ
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	എങ്കിൽ (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	എങ്കിൽ (errmsg == ശൂന്യ && (*fdp = mch_open((അക്ഷരം *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    എങ്കിൽ (errmsg != ശൂന്യ)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	സന്ദേശം(errmsg);
	എങ്കിൽ (tmpname != ശൂന്യ)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    എങ്കിൽ (*fdp < 0)
	*fdp = mch_open((അക്ഷരം *)fname, O_RDONLY | O_EXTRA, 0);

    തിരികെ tmpname;
}
#endif

#എങ്കിൽ defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    സ്ഥിതം char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    long	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    പൂർണ്ണസംഖ്യ		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    പൂർണ്ണസംഖ്യ		*did_ask)	// flag: whether already asked for key
{
    പൂർണ്ണസംഖ്യ method = crypt_method_nr_from_magic((അക്ഷരം *)ptr, *sizep);
    പൂർണ്ണസംഖ്യ b_p_ro = curbuf->b_p_ro;

    എങ്കിൽ (method >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_set_cm_option(curbuf, method);
	എങ്കിൽ (cryptkey == ശൂന്യ && !*did_ask)
	{
	    എങ്കിൽ (*curbuf->b_p_താക്കോൽ)
	    {
		cryptkey = curbuf->b_p_താക്കോൽ;
		crypt_check_swapfile_curbuf();
	    }
	    അല്ലെങ്കിൽ
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_താക്കോൽ_സന്ദേശം), fname);
		സന്ദേശം_scroll = TRUE;
		crypt_check_method(method);
		cryptkey = crypt_get_താക്കോൽ(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		എങ്കിൽ (cryptkey != ശൂന്യ && *cryptkey == NUL)
		{
		    എങ്കിൽ (cryptkey != curbuf->b_p_താക്കോൽ)
			vim_free(cryptkey);
		    cryptkey = ശൂന്യ;
		}
	    }
	}

	എങ്കിൽ (cryptkey != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ header_len;

	    header_len = crypt_get_header_len(method);
	    എങ്കിൽ (*sizep < header_len)
	    {
		// invalid header, buffer can't be encrypted
		എങ്കിൽ (cryptkey != curbuf->b_p_താക്കോൽ)
		    vim_free(cryptkey);
		തിരികെ ശൂന്യ;
	    }

	    curbuf->b_cryptstate = crypt_create_from_header(
							method, cryptkey, ptr);
	    crypt_set_cm_option(curbuf, method);

	    // Remove cryptmethod specific header from the text.
	    *filesizep += header_len;
	    *sizep -= header_len;
	    mch_memmove(ptr, ptr + header_len, (size_t)*sizep);

	    // Restore the read-only flag.
	    curbuf->b_p_ro = b_p_ro;
	}
    }
    // When starting to edit a new file which does not have encryption, clear
    // the 'key' option, except when starting up (called with -x argument)
    അല്ലെങ്കിൽ എങ്കിൽ (newfile && *curbuf->b_p_താക്കോൽ != NUL && !starting)
	set_option_മൂല്യം_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    തിരികെ cryptkey;
}
#endif  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    പൂർണ്ണസംഖ്യ
check_file_readonly(
    char_u	*fname,		// full path to file
    പൂർണ്ണസംഖ്യ		perm UNUSED)	// known permissions on file
{
#ifndef USE_MCH_ACCESS
    പൂർണ്ണസംഖ്യ	    fd = 0;
#endif

    തിരികെ (
#ifdef USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((അക്ഷരം *)fname, W_OK)
#അല്ലെങ്കിൽ
	(fd = mch_open((അക്ഷരം *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#endif
	);
}

#എങ്കിൽ defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    പൂർണ്ണസംഖ്യ
vim_fsync(പൂർണ്ണസംഖ്യ fd)
{
    പൂർണ്ണസംഖ്യ r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    എങ്കിൽ (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    തിരികെ r;
}
#endif

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    പൂർണ്ണസംഖ്യ
set_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    എങ്കിൽ (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
#ifdef FEAT_EVAL
    എങ്കിൽ (aborting())	    // autocmds may abort script processing
	തിരികെ FAIL;
#endif
    എങ്കിൽ (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	തിരികെ FAIL;
    }

    എങ്കിൽ (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    എങ്കിൽ (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
#ifdef FEAT_EVAL
    എങ്കിൽ (aborting())	    // autocmds may abort script processing
	തിരികെ FAIL;
#endif

    // Do filetype detection now if 'filetype' is empty.
    എങ്കിൽ (*curbuf->b_p_ft == NUL)
    {
	എങ്കിൽ (au_has_group((char_u *)"filetypedetect"))
	    (ശൂന്യം)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, ശൂന്യ);
	do_modelines(0);
    }

    തിരികെ OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    ശൂന്യം
സന്ദേശം_add_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    എങ്കിൽ (fname == ശൂന്യ)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    പൂർണ്ണസംഖ്യ
സന്ദേശം_add_fileformat(പൂർണ്ണസംഖ്യ eol_type)
{
#ifndef USE_CRNL
    എങ്കിൽ (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[dos]") : _("[dos format]"));
	തിരികെ TRUE;
    }
#endif
    എങ്കിൽ (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[mac]") : _("[mac format]"));
	തിരികെ TRUE;
    }
#ifdef USE_CRNL
    എങ്കിൽ (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[unix]") : _("[unix format]"));
	തിരികെ TRUE;
    }
#endif
    തിരികെ FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    ശൂന്യം
സന്ദേശം_add_വരികൾ(
    പൂർണ്ണസംഖ്യ	    തിരുകുക_space,
    long    lnum,
    off_T   nchars)
{
    പൂർണ്ണസംഖ്യ  len = (പൂർണ്ണസംഖ്യ)STRLEN(IObuff);

    എങ്കിൽ (shortmess(SHM_വരികൾ))
	vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), തിരുകുക_space ? " " : "", lnum, (varnumber_T)nchars);
    അല്ലെങ്കിൽ
    {
	len += vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), തിരുകുക_space ? " " : "", lnum);
	vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    ശൂന്യം
സന്ദേശം_add_eol(ശൂന്യം)
{
    STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]") : _("[Incomplete last line]"));
}

    പൂർണ്ണസംഖ്യ
time_differs(stat_T *st, long mtime, long mtime_ns UNUSED)
{
    തിരികെ
#ifdef ST_MTIM_NSEC
	(long)st->ST_MTIM_NSEC != mtime_ns ||
#endif
#എങ്കിൽ defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(long)st->st_mtime - mtime > 1 || mtime - (long)st->st_mtime > 1
#അല്ലെങ്കിൽ
	(long)st->st_mtime != mtime
#endif
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    പൂർണ്ണസംഖ്യ
need_conversion(char_u *fenc)
{
    പൂർണ്ണസംഖ്യ		same_encoding;
    പൂർണ്ണസംഖ്യ		enc_flags;
    പൂർണ്ണസംഖ്യ		fenc_flags;

    എങ്കിൽ (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    അല്ലെങ്കിൽ
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    എങ്കിൽ (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	തിരികെ enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    തിരികെ !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    പൂർണ്ണസംഖ്യ
get_fio_flags(char_u *ptr)
{
    പൂർണ്ണസംഖ്യ		prop;

    എങ്കിൽ (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    എങ്കിൽ (prop & ENC_UNICODE)
    {
	എങ്കിൽ (prop & ENC_2BYTE)
	{
	    എങ്കിൽ (prop & ENC_ENDIAN_L)
		തിരികെ FIO_UCS2 | FIO_ENDIAN_L;
	    തിരികെ FIO_UCS2;
	}
	എങ്കിൽ (prop & ENC_4BYTE)
	{
	    എങ്കിൽ (prop & ENC_ENDIAN_L)
		തിരികെ FIO_UCS4 | FIO_ENDIAN_L;
	    തിരികെ FIO_UCS4;
	}
	എങ്കിൽ (prop & ENC_2WORD)
	{
	    എങ്കിൽ (prop & ENC_ENDIAN_L)
		തിരികെ FIO_UTF16 | FIO_ENDIAN_L;
	    തിരികെ FIO_UTF16;
	}
	തിരികെ FIO_UTF8;
    }
    എങ്കിൽ (prop & ENC_LATIN1)
	തിരികെ FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    തിരികെ 0;
}

#എങ്കിൽ defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    പൂർണ്ണസംഖ്യ
get_win_fio_flags(char_u *ptr)
{
    പൂർണ്ണസംഖ്യ		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    എങ്കിൽ (!enc_utf8 && enc_codepage <= 0)
	തിരികെ 0;

    cp = encname2codepage(ptr);
    എങ്കിൽ (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	എങ്കിൽ (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	അല്ലെങ്കിൽ
# endif
	    തിരികെ 0;
    }
    തിരികെ FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#endif

#എങ്കിൽ defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    പൂർണ്ണസംഖ്യ
get_mac_fio_flags(char_u *ptr)
{
    എങ്കിൽ ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	തിരികെ FIO_MACROMAN;
    തിരികെ 0;
}
#endif

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    സ്ഥിതം char_u *
check_for_bom(
    char_u	*p,
    long	size,
    പൂർണ്ണസംഖ്യ		*lenp,
    പൂർണ്ണസംഖ്യ		flags)
{
    അക്ഷരം	*name = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len = 2;

    എങ്കിൽ (p[0] == 0xef && p[1] == 0xbb && size >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p[0] == 0xff && p[1] == 0xfe)
    {
	എങ്കിൽ (size >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	അല്ലെങ്കിൽ എങ്കിൽ (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	എങ്കിൽ (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	അല്ലെങ്കിൽ
	    name = "utf-16";	// FE FF
    }
    അല്ലെങ്കിൽ എങ്കിൽ (size >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    തിരികെ (char_u *)name;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns "full_path" or pointer into "full_path" if shortened.
 */
    char_u *
shorten_fname1(char_u *full_path)
{
    char_u	*dirname;
    char_u	*p = full_path;

    dirname = alloc(MAXPATHL);
    എങ്കിൽ (dirname == ശൂന്യ)
	തിരികെ full_path;
    എങ്കിൽ (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(full_path, dirname);
	എങ്കിൽ (p == ശൂന്യ || *p == NUL)
	    p = full_path;
    }
    vim_free(dirname);
    തിരികെ p;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns NULL if not shorter name possible, pointer into "full_path"
 * otherwise.
 */
    char_u *
shorten_fname(char_u *full_path, char_u *dir_name)
{
    പൂർണ്ണസംഖ്യ		len;
    char_u	*p;

    എങ്കിൽ (full_path == ശൂന്യ)
	തിരികെ ശൂന്യ;
    len = (പൂർണ്ണസംഖ്യ)STRLEN(dir_name);
    എങ്കിൽ (fnamencmp(dir_name, full_path, len) == 0)
    {
	p = full_path + len;
#എങ്കിൽ defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	എങ്കിൽ (!((len > 2) && (*(p - 2) == ':')))
#endif
	{
	    എങ്കിൽ (vim_ispathsep(*p))
		ചെയ്യുക
		    ++p;
		ആകുമ്പോൾ (vim_ispathsep_nocolon(*p));
#ifndef VMS   // the path separator is always part of the path
	    അല്ലെങ്കിൽ
		p = ശൂന്യ;
#endif
	}
    }
#എങ്കിൽ defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    അല്ലെങ്കിൽ എങ്കിൽ (len > 3
	    && TOUPPER_LOC(full_path[0]) == TOUPPER_LOC(dir_name[0])
	    && full_path[1] == ':'
	    && vim_ispathsep(full_path[2]))
	p = full_path + 2;
#endif
    അല്ലെങ്കിൽ
	p = ശൂന്യ;
    തിരികെ p;
}

/*
 * Shorten filename of a buffer.
 * When "force" is TRUE: Use full path from now on for files currently being
 * edited, both for file name and swap file name.  Try to shorten the file
 * names a bit, if safe to do so.
 * When "force" is FALSE: Only try to shorten absolute file names.
 * For buffers that have buftype "nofile" or "scratch": never change the file
 * name.
 */
    ശൂന്യം
shorten_buf_fname(buf_T *buf, char_u *dirname, പൂർണ്ണസംഖ്യ force)
{
    char_u	*p;

    എങ്കിൽ (buf->b_fname != ശൂന്യ
	    && !bt_nofilename(buf)
	    && !path_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == ശൂന്യ
		|| mch_isFullName(buf->b_sfname)))
    {
	എങ്കിൽ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	എങ്കിൽ (p == ശൂന്യ || buf->b_fname == ശൂന്യ)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    ശൂന്യം
shorten_fnames(പൂർണ്ണസംഖ്യ force)
{
    char_u	dirname[MAXPATHL];
    buf_T	*buf;

    mch_dirname(dirname, MAXPATHL);
    FOR_ALL_BUFFERS(buf)
    {
	shorten_buf_fname(buf, dirname, force);

	// Always make the swap file name a full path, a "nofile" buffer may
	// also have a swap file.
	mf_fullname(buf->b_ml.ml_mfp);
    }
    status_redraw_all();
    redraw_tabline = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#endif
}

#എങ്കിൽ (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    ശൂന്യം
shorten_filenames(char_u **fnames, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    എങ്കിൽ (fnames == ശൂന്യ || എണ്ണം < 1)
	തിരികെ;
    mch_dirname(dirname, വലുപ്പം(dirname));
    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
    {
	എങ്കിൽ ((p = shorten_fname(fnames[i], dirname)) != ശൂന്യ)
	{
	    // shorten_fname() returns pointer in given "fnames[i]".  If free
	    // "fnames[i]" first, "p" becomes invalid.  So we need to copy
	    // "p" first then free fnames[i].
	    p = vim_strsave(p);
	    vim_free(fnames[i]);
	    fnames[i] = p;
	}
    }
}
#endif

/*
 * Add extension to file name - change path/fo.o.h to path/fo.o.h.ext or
 * fo_o_h.ext for MSDOS or when shortname option set.
 *
 * Assumed that fname is a valid name found in the filesystem we assure that
 * the return value is a different name and ends in 'ext'.
 * "ext" MUST be at most 4 characters long if it starts with a dot, 3
 * characters otherwise.
 * Space for the returned name is allocated, must be freed later.
 * Returns NULL when out of memory.
 */
    char_u *
modname(
    char_u *fname,
    char_u *ext,
    പൂർണ്ണസംഖ്യ	    prepend_dot)	// may prepend a '.' to file name
{
    തിരികെ buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    പൂർണ്ണസംഖ്യ	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    പൂർണ്ണസംഖ്യ	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    size_t	ptrlen;
    പൂർണ്ണസംഖ്യ		fnamelen, extlen;

    extlen = (പൂർണ്ണസംഖ്യ)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    എങ്കിൽ (fname == ശൂന്യ || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	എങ്കിൽ (retval == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (പൂർണ്ണസംഖ്യ)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    തിരികെ ശൂന്യ;
	}
	എങ്കിൽ (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    അല്ലെങ്കിൽ
    {
	fnamelen = (പൂർണ്ണസംഖ്യ)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	എങ്കിൽ (retval == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	STRCPY(retval, fname);
#ifdef VMS
	vms_remove_version(retval); // we do not need versions here
#endif
    }

    /*
     * search backwards until we hit a '/', '\' or ':' replacing all '.'
     * by '_' for MSDOS or when shortname option set and ext starts with a dot.
     * Then truncate what is after the '/', '\' or ':' to 8 characters for
     * MSDOS and 26 characters for AMIGA, a lot more for UNIX.
     */
    വേണ്ടി (ptr = retval + fnamelen; ptr > retval; MB_PTR_BACK(retval, ptr))
    {
	എങ്കിൽ (*ext == '.' && shortname)
	    എങ്കിൽ (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	എങ്കിൽ (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    നിർത്തുക;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    എങ്കിൽ (ptrlen > (unsigned)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    എങ്കിൽ (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	എങ്കിൽ (fname == ശൂന്യ || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    എങ്കിൽ (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	അല്ലെങ്കിൽ എങ്കിൽ (*ext == '.')
	{
	    എങ്കിൽ ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	അല്ലെങ്കിൽ എങ്കിൽ ((e = vim_strchr(ptr, '.')) == ശൂന്യ)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	അല്ലെങ്കിൽ എങ്കിൽ ((പൂർണ്ണസംഖ്യ)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#ifdef MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    അല്ലെങ്കിൽ എങ്കിൽ ((fname == ശൂന്യ || *fname == NUL) && *ext == '.')
	*s++ = '_';
#endif

    /*
     * Append the extension.
     * ext can start with '.' and cannot exceed 3 more characters.
     */
    STRCPY(s, ext);
    /*
     * Prepend the dot.
     */
    എങ്കിൽ (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (size_t)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    എങ്കിൽ (fname != ശൂന്യ && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	ആകുമ്പോൾ (--s >= ptr)
	{
	    എങ്കിൽ (*s != '_')
	    {
		*s = '_';
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    തിരികെ retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    പൂർണ്ണസംഖ്യ
vim_fgets(char_u *buf, പൂർണ്ണസംഖ്യ size, FILE *fp)
{
    അക്ഷരം	*eof;
#define FGETS_SIZE 200
    അക്ഷരം	tbuf[FGETS_SIZE];

    // safety check
    എങ്കിൽ (size < 2)
    {
	എങ്കിൽ (size == 1)
	    buf[0] = NUL;
	തിരികെ TRUE;
    }

    buf[size - 2] = NUL;
    eof = fgets((അക്ഷരം *)buf, size, fp);
    എങ്കിൽ (buf[size - 2] != NUL && buf[size - 2] != '\n')
    {
	buf[size - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	ചെയ്യുക
	{
	    tbuf[FGETS_SIZE - 2] = NUL;
	    vim_ignoredp = fgets((അക്ഷരം *)tbuf, FGETS_SIZE, fp);
	} ആകുമ്പോൾ (tbuf[FGETS_SIZE - 2] != NUL && tbuf[FGETS_SIZE - 2] != '\n');
    }
    തിരികെ (eof == ശൂന്യ);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    പൂർണ്ണസംഖ്യ
vim_rename(char_u *from, char_u *to)
{
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		ret;
#ifdef AMIGA
    BPTR	flock;
#endif
    stat_T	st;
    പൂർണ്ണസംഖ്യ		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    എങ്കിൽ (fnamecmp(from, to) == 0)
    {
	എങ്കിൽ (p_fic && STRCMP(gettail(from), gettail(to)) != 0)
	    use_tmp_file = TRUE;
	അല്ലെങ്കിൽ
	    തിരികെ 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    എങ്കിൽ (mch_stat((അക്ഷരം *)from, &st) < 0)
	തിരികെ -1;

#ifdef UNIX
    {
	stat_T	st_to;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	എങ്കിൽ (mch_stat((അക്ഷരം *)to, &st_to) >= 0
		&& st.st_dev == st_to.st_dev
		&& st.st_ino == st_to.st_ino)
	    use_tmp_file = TRUE;
    }
#endif
#ifdef MSWIN
    {
	BY_HANDLE_FILE_INFORMATION info1, info2;

	// It's possible for the source and destination to be the same file.
	// In that case go through a temp file name.  This makes rename("foo",
	// "./foo") a no-op (in a complicated way).
	എങ്കിൽ (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(to, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_സംഖ്യ == info2.dw_Volume_Serial_സംഖ്യ
		&& info1.n_File_Index_മുകളിലെ == info2.n_File_Index_മുകളിലെ
		&& info1.n_File_Index_താഴെയുള്ള == info2.n_File_Index_താഴെയുള്ള)
	    use_tmp_file = TRUE;
    }
#endif

    എങ്കിൽ (use_tmp_file)
    {
	അക്ഷരം	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	എങ്കിൽ (STRLEN(from) >= MAXPATHL - 5)
	    തിരികെ -1;
	STRCPY(tempname, from);
	വേണ്ടി (n = 123; n < 99999; ++n)
	{
	    sprintf((അക്ഷരം *)gettail((char_u *)tempname), "%d", n);
	    എങ്കിൽ (mch_stat(tempname, &st) < 0)
	    {
		എങ്കിൽ (mch_rename((അക്ഷരം *)from, tempname) == 0)
		{
		    എങ്കിൽ (mch_rename(tempname, (അക്ഷരം *)to) == 0)
			തിരികെ 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (ശൂന്യം)mch_rename(tempname, (അക്ഷരം *)from);
		    തിരികെ -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		തിരികെ -1;
	    }
	}
	തിരികെ -1;
    }

    /*
     * Delete the "to" file, this is required on some systems to make the
     * mch_rename() work, on other systems it makes sure that we don't have
     * two files when the mch_rename() fails.
     */

#ifdef AMIGA
    /*
     * With MSDOS-compatible filesystems (crossdos, messydos) it is possible
     * that the name of the "to" file is the same as the "from" file, even
     * though the names are different. To avoid the chance of accidentally
     * deleting the "from" file (horror!) we lock it during the remove.
     *
     * When used for making a backup before writing the file: This should not
     * happen with ":w", because startscript() should detect this problem and
     * set buf->b_shortname, causing modname() to return a correct ".bak" file
     * name.  This problem does exist with ":w filename", but then the
     * original file will be somewhere else so the backup isn't really
     * important. If autoscripting is off the rename may fail.
     */
    flock = Lock((UBYTE *)from, (long)VIM_ACCESS_READ);
#endif
    mch_remove(to);
#ifdef AMIGA
    എങ്കിൽ (flock)
	UnLock(flock);
#endif

    /*
     * First try a normal rename, return if it works.
     */
    എങ്കിൽ (mch_rename((അക്ഷരം *)from, (അക്ഷരം *)to) == 0)
	തിരികെ 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, to);
    എങ്കിൽ (ret != OK)
	തിരികെ -1;

    /*
     * Remove copied original file
     */
    എങ്കിൽ (mch_stat((അക്ഷരം *)from, &st) >= 0)
	mch_remove(from);

    തിരികെ 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    പൂർണ്ണസംഖ്യ
vim_copyfile(char_u *from, char_u *to)
{
    പൂർണ്ണസംഖ്യ		fd_in;
    പൂർണ്ണസംഖ്യ		fd_out;
    പൂർണ്ണസംഖ്യ		n;
    അക്ഷരം	*errmsg = ശൂന്യ;
    അക്ഷരം	*buffer;
    long	perm;
#ifdef HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#endif

#ifdef HAVE_READLINK
    പൂർണ്ണസംഖ്യ		ret;
    പൂർണ്ണസംഖ്യ		len;
    stat_T	st;
    അക്ഷരം	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((അക്ഷരം *)from, &st);
    എങ്കിൽ (ret >= 0 && S_ISLNK(st.st_mode))
    {
	ret = -1;

	len = readlink((അക്ഷരം *)from, linkbuf, MAXPATHL);
	എങ്കിൽ (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (അക്ഷരം *)to);
	}

	തിരികെ ret == 0 ? OK : FAIL;
    }
#endif

    perm = mch_getperm(from);
#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#endif
    fd_in = mch_open((അക്ഷരം *)from, O_RDONLY|O_EXTRA, 0);
    എങ്കിൽ (fd_in == -1)
    {
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	തിരികെ FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_out = mch_open((അക്ഷരം *)to,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (പൂർണ്ണസംഖ്യ)perm);
    എങ്കിൽ (fd_out == -1)
    {
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	തിരികെ FAIL;
    }

    buffer = alloc(WRITEBUFSIZE);
    എങ്കിൽ (buffer == ശൂന്യ)
    {
	close(fd_out);
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	തിരികെ FAIL;
    }

    ആകുമ്പോൾ ((n = read_eintr(fd_in, buffer, WRITEBUFSIZE)) > 0)
	എങ്കിൽ (write_eintr(fd_out, buffer, n) != n)
	{
	    errmsg = _(e_error_writing_to_str);
	    നിർത്തുക;
	}

    vim_free(buffer);
    close(fd_in);
    എങ്കിൽ (close(fd_out) < 0)
	errmsg = _(e_error_closing_str);
    എങ്കിൽ (n < 0)
    {
	errmsg = _(e_error_reading_str);
	to = from;
    }
#ifndef UNIX	    // for Unix mch_open() already set the permission
    mch_setperm(to, perm);
#endif
#ifdef HAVE_ACL
    mch_set_acl(to, acl);
    mch_free_acl(acl);
#endif
#എങ്കിൽ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, to);
#endif
    എങ്കിൽ (errmsg != ശൂന്യ)
    {
	semsg(errmsg, to);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    പൂർണ്ണസംഖ്യ
check_timestamps(
    പൂർണ്ണസംഖ്യ		focus)		// called for GUI focus event
{
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		didit = 0;
    പൂർണ്ണസംഖ്യ		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    എങ്കിൽ (no_check_timestamps > 0)
	തിരികെ FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    എങ്കിൽ (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	തിരികെ FALSE;
    }

    എങ്കിൽ (!stuff_empty() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_lock > 0 || allbuf_lock > 0)
	need_check_timestamps = TRUE;		// check later
    അല്ലെങ്കിൽ
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    എങ്കിൽ (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		എങ്കിൽ (didit < n)
		    didit = n;
		എങ്കിൽ (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    തുടരുക;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	എങ്കിൽ (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    സന്ദേശം_puts("\n");
	    out_flush();
	}
    }
    തിരികെ didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
move_വരികൾ(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    പൂർണ്ണസംഖ്യ		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    വേണ്ടി (lnum = 1; lnum <= frombuf->b_ml.ml_വരി_എണ്ണം; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	എങ്കിൽ (p == ശൂന്യ || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    നിർത്തുക;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    എങ്കിൽ (retval != FAIL)
    {
	curbuf = frombuf;
	വേണ്ടി (lnum = curbuf->b_ml.ml_വരി_എണ്ണം; lnum > 0; --lnum)
	    എങ്കിൽ (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		നിർത്തുക;
	    }
    }

    curbuf = tbuf;
    തിരികെ retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    പൂർണ്ണസംഖ്യ
buf_check_timestamp(
    buf_T	*buf,
    പൂർണ്ണസംഖ്യ		focus UNUSED)	// called for GUI focus event
{
    stat_T	st;
    പൂർണ്ണസംഖ്യ		stat_res;
    പൂർണ്ണസംഖ്യ		retval = 0;
    അക്ഷരം	*mesg = ശൂന്യ;
    അക്ഷരം	*mesg2 = "";
    പൂർണ്ണസംഖ്യ		helpmesg = FALSE;
    എണ്ണൽ {
	RELOAD_NONE,
	RELOAD_NORMAL,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#എങ്കിൽ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    പൂർണ്ണസംഖ്യ		can_reload = FALSE;
#endif
    off_T	orig_size = buf->b_orig_size;
    പൂർണ്ണസംഖ്യ		orig_mode = buf->b_orig_mode;
#ifdef FEAT_GUI
    പൂർണ്ണസംഖ്യ		save_mouse_correct = need_mouse_correct;
#endif
    സ്ഥിതം പൂർണ്ണസംഖ്യ	busy = FALSE;
    പൂർണ്ണസംഖ്യ		n;
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    എങ്കിൽ (buf->b_ffname == ശൂന്യ
	    || buf->b_ml.ml_mfp == ശൂന്യ
	    || !bt_normal(buf)
	    || buf->b_saving
	    || busy
#ifdef FEAT_NETBEANS_INTG
	    || isNetbeansBuffer(buf)
#endif
#ifdef FEAT_TERMINAL
	    || buf->b_term != ശൂന്യ
#endif
	    )
	തിരികെ 0;

    എങ്കിൽ (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((അക്ഷരം *)buf->b_ffname, &st)) < 0
		|| time_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_size
#ifdef HAVE_ST_MODE
		|| (പൂർണ്ണസംഖ്യ)st.st_mode != buf->b_orig_mode
#അല്ലെങ്കിൽ
		|| mch_getperm(buf->b_ffname) != buf->b_orig_mode
#endif
		))
    {
	long prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	എങ്കിൽ (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_size = 0;
	    buf->b_orig_mode = 0;
	}
	അല്ലെങ്കിൽ
	    buf_store_time(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	എങ്കിൽ (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	അല്ലെങ്കിൽ എങ്കിൽ ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !bufIsChanged(buf) && stat_res >= 0)
	    reload = RELOAD_NORMAL;
	അല്ലെങ്കിൽ
	{
	    അക്ഷരം    *reason;
#ifdef FEAT_EVAL
	    size_t  reasonlen;
#endif

	    എങ്കിൽ (stat_res < 0)
	    {
		reason = "deleted";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (bufIsChanged(buf))
	    {
		reason = "conflict";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("conflict");
#endif
	    }
	    /*
	     * Check if the file contents really changed to avoid giving a
	     * warning when only the timestamp was set (e.g., checked out of
	     * CVS).  Always warn when the buffer was changed.
	     */
	    അല്ലെങ്കിൽ എങ്കിൽ (orig_size != buf->b_orig_size || buf_contents_changed(buf))
	    {
		reason = "changed";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (orig_mode != buf->b_orig_mode)
	    {
		reason = "mode";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#endif
	    }
	    അല്ലെങ്കിൽ
	    {
		reason = "time";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("time");
#endif
	    }

	    /*
	     * Only give the warning if there are no FileChangedShell
	     * autocommands.
	     * Avoid being called recursively by setting "busy".
	     */
	    busy = TRUE;
#ifdef FEAT_EVAL
	    set_vim_var_string(VV_FCS_REASON, (char_u *)reason, (പൂർണ്ണസംഖ്യ)reasonlen);
	    set_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#endif
	    ++allbuf_lock;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_lock;
	    busy = FALSE;
	    എങ്കിൽ (n)
	    {
		എങ്കിൽ (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_buffer));
#ifdef FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		എങ്കിൽ (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_NORMAL;
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s, "ask") == 0)
		    n = FALSE;
		അല്ലെങ്കിൽ
#endif
		    തിരികെ 2;
	    }
	    എങ്കിൽ (!n)
	    {
		എങ്കിൽ (*reason == 'd')
		{
		    // Only give the message once.
		    എങ്കിൽ (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_available);
		}
		അല്ലെങ്കിൽ
		{
		    helpmesg = TRUE;
#എങ്കിൽ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#endif
		    എങ്കിൽ (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    അല്ലെങ്കിൽ
		    {
			// Only timestamp changed, store it to avoid a warning
			// in check_mtime() later.
			buf->b_mtime_read = buf->b_mtime;
			buf->b_mtime_read_ns = buf->b_mtime_ns;
		    }
		}
	    }
	}

    }
    അല്ലെങ്കിൽ എങ്കിൽ ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#എങ്കിൽ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#endif
    }

    എങ്കിൽ (mesg != ശൂന്യ)
    {
	char_u *path;

	path = home_replace_save(buf, buf->b_fname);
	എങ്കിൽ (path != ശൂന്യ)
	{
	    size_t  tbufsize;
	    അക്ഷരം    *tbuf;

	    എങ്കിൽ (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(path) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    എങ്കിൽ (tbuf != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, path);
#ifdef FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		set_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#endif
#എങ്കിൽ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		എങ്കിൽ (can_reload)
		{
		    എങ്കിൽ (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    തിരഞ്ഞെടുപ്പ് (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, ശൂന്യ, TRUE))
		    {
			അവസ്ഥ 2:
			    reload = RELOAD_NORMAL;
			    നിർത്തുക;
			അവസ്ഥ 3:
			    reload = RELOAD_DETECT;
			    നിർത്തുക;
		    }
		}
		അല്ലെങ്കിൽ
#endif
		എങ്കിൽ (State > MODE_NORMAL_BUSY || (State & MODE_CMDLINE)
								 || already_warned)
		{
		    എങ്കിൽ (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (!autocmd_busy)
		    {
			സന്ദേശം_start();
			സന്ദേശം_puts_attr(tbuf, HL_ATTR(HLF_E) + സന്ദേശം_HIST);
			എങ്കിൽ (*mesg2 != NUL)
			    സന്ദേശം_puts_attr(mesg2, HL_ATTR(HLF_W) + സന്ദേശം_HIST);
			സന്ദേശം_clr_eos();
			(ശൂന്യം)സന്ദേശം_end();
			എങ്കിൽ (emsg_silent == 0 && !in_assert_fails)
			{
			    out_flush();
#ifdef FEAT_GUI
			    എങ്കിൽ (!focus)
#endif
				// give the user some time to think about it
				ui_delay(1004L, TRUE);

			    // don't redraw and erase the message
			    redraw_cmdline = FALSE;
			}
		    }
		    already_warned = TRUE;
		}
	    }

	    vim_free(tbuf);
	    vim_free(path);
	}
    }

    എങ്കിൽ (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_mode, reload == RELOAD_DETECT);
#ifdef FEAT_PERSISTENT_UNDO
	എങ്കിൽ (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != ശൂന്യ)
	{
	    char_u	    hash[UNDO_HASH_SIZE];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(ശൂന്യ, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#endif
    }

    // Trigger FileChangedShell when the file was changed in any way.
    എങ്കിൽ (bufref_valid(&bufref) && retval != 0)
	(ശൂന്യം)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#ifdef FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#endif

    തിരികെ retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    ശൂന്യം
buf_reload(buf_T *buf, പൂർണ്ണസംഖ്യ orig_mode, പൂർണ്ണസംഖ്യ reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    പൂർണ്ണസംഖ്യ		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    പൂർണ്ണസംഖ്യ		saved = OK;
    aco_save_T	aco;
    പൂർണ്ണസംഖ്യ		flags = READ_NEW;
    പൂർണ്ണസംഖ്യ		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    എങ്കിൽ (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	തിരികെ;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    എങ്കിൽ (reload_options)
	CLEAR_FIELD(ea);
    അല്ലെങ്കിൽ
	prepped = prep_exarg(&ea, buf);

    എങ്കിൽ (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	എങ്കിൽ (p_ur < 0 || curbuf->b_ml.ml_വരി_എണ്ണം <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_വരി_എണ്ണം + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	എങ്കിൽ (BUFEMPTY() || saved == FAIL)
	    savebuf = ശൂന്യ;
	അല്ലെങ്കിൽ
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(ശൂന്യ, ശൂന്യ, (linenr_T)1, BLN_DUMMY);
	    set_bufref(&bufref, savebuf);
	    എങ്കിൽ (savebuf != ശൂന്യ && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_buffer = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_buffer = buf;
	    }
	    എങ്കിൽ (savebuf == ശൂന്യ || saved == FAIL || buf != curbuf
				      || move_വരികൾ(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	എങ്കിൽ (saved == OK)
	{
	    പൂർണ്ണസംഖ്യ old_സന്ദേശം_silent = സന്ദേശം_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    എങ്കിൽ (shortmess(SHM_FILEINFO))
		സന്ദേശം_silent = 1;

	    എങ്കിൽ (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#എങ്കിൽ defined(FEAT_EVAL)
		എങ്കിൽ (!aborting())
#endif
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		എങ്കിൽ (savebuf != ശൂന്യ && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    ആകുമ്പോൾ (!BUFEMPTY())
			എങ്കിൽ (ml_delete(buf->b_ml.ml_വരി_എണ്ണം) == FAIL)
			    നിർത്തുക;
		    (ശൂന്യം)move_വരികൾ(savebuf, buf);
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		എങ്കിൽ ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		അല്ലെങ്കിൽ
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    സന്ദേശം_silent = old_സന്ദേശം_silent;
	}
	vim_free(ea.cmd);

	എങ്കിൽ (savebuf != ശൂന്യ && bufref_valid(&bufref))
	    wipe_buffer(savebuf, FALSE);

#ifdef FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#endif

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	എങ്കിൽ (old_topline > curbuf->b_ml.ml_വരി_എണ്ണം)
	    curwin->w_topline = curbuf->b_ml.ml_വരി_എണ്ണം;
	അല്ലെങ്കിൽ
	    curwin->w_topline = old_topline;
	curwin->w_cursor = old_cursor;
	check_cursor();
	update_topline();
	curbuf->b_keep_filetype = false;
#ifdef FEAT_FOLDING
	{
	    win_T	*wp;
	    tabpage_T	*tp;

	    // Update folds unless they are defined manually.
	    FOR_ALL_TAB_WINDOWS(tp, wp)
		എങ്കിൽ (wp->w_buffer == curwin->w_buffer
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#endif
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	എങ്കിൽ (orig_mode == curbuf->b_orig_mode)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    ശൂന്യം
buf_store_time(buf_T *buf, stat_T *st, char_u *fname UNUSED)
{
    buf->b_mtime = (long)st->st_mtime;
#ifdef ST_MTIM_NSEC
    buf->b_mtime_ns = (long)st->ST_MTIM_NSEC;
#അല്ലെങ്കിൽ
    buf->b_mtime_ns = 0;
#endif
    buf->b_orig_size = st->st_size;
#ifdef HAVE_ST_MODE
    buf->b_orig_mode = (പൂർണ്ണസംഖ്യ)st->st_mode;
#അല്ലെങ്കിൽ
    buf->b_orig_mode = mch_getperm(fname);
#endif
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    ശൂന്യം
write_lnum_adjust(linenr_T offset)
{
    എങ്കിൽ (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += offset;
}

// Subfuncions for readdirex()
#ifdef FEAT_EVAL
# ifdef MSWIN
    സ്ഥിതം char_u *
getfpermwfd(WIN32_FIND_DATAW *wfd, char_u *perm)
{
    stat_T	    st;
    unsigned short  st_mode;
    DWORD	    flag = wfd->dwFileAttributes;
    WCHAR	    *wp;

    st_mode = (flag & FILE_ATTRIBUTE_DIRECTORY)
					? (_S_IFDIR | _S_IEXEC) : _S_IFREG;
    st_mode |= (flag & FILE_ATTRIBUTE_READONLY)
					? _S_IREAD : (_S_IREAD | _S_IWRITE);

    wp = wcsrchr(wfd->cFileName, L'.');
    എങ്കിൽ (wp != ശൂന്യ)
    {
	എങ്കിൽ (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_mode |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_mode |= (st_mode & 0700) >> 3;
    st_mode |= (st_mode & 0700) >> 6;

    st.st_mode = st_mode;
    തിരികെ getfpermst(&st, perm);
}

    സ്ഥിതം ശൂന്യം
getftypewfd(WIN32_FIND_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dwFileAttributes;
    DWORD tag = wfd->dwReserved0;

    എങ്കിൽ (flag & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	എങ്കിൽ (tag == IO_REPARSE_TAG_MOUNT_POINT)
	{
	    ret->string = (char_u *)"junction";
	    ret->length = STRLEN_LITERAL("junction");
	    തിരികെ;
	}
	എങ്കിൽ (tag == IO_REPARSE_TAG_SYMLINK)
	{
	    എങ്കിൽ (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->length = STRLEN_LITERAL("linkd");
		തിരികെ;
	    }

	    ret->string = (char_u *)"link";
	    ret->length = STRLEN_LITERAL("link");
	    തിരികെ;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->length = STRLEN_LITERAL("reparse");
	തിരികെ;
    }

    എങ്കിൽ (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->length = STRLEN_LITERAL("dir");
	തിരികെ;
    }

    ret->string = (char_u *)"file";
    ret->length = STRLEN_LITERAL("file");
}

    സ്ഥിതം dict_T *
create_readdirex_item(WIN32_FIND_DATAW *wfd)
{
    dict_T	*item;
    char_u	*p;
    varnumber_T	size, time;
    char_u	permbuf[] = "---------";
    string_T	s;

    item = dict_alloc();
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ ശൂന്യ;
    item->dv_refcount++;

    p = utf16_to_enc(wfd->cFileName, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	പോകുക theend;
    എങ്കിൽ (dict_add_string(item, "name", p) == FAIL)
    {
	vim_free(p);
	പോകുക theend;
    }
    vim_free(p);

    size = (((varnumber_T)wfd->n_File_Size_മുകളിലെ) << 32) | wfd->n_File_Size_താഴെയുള്ള;
    എങ്കിൽ (dict_add_സംഖ്യ(item, "size", size) == FAIL)
	പോകുക theend;

    // Convert FILETIME to unix time.
    time = (((((varnumber_T)wfd->ftLastWriteTime.dw_മുകളിലെ_Date_Time) << 32) |
		wfd->ftLastWriteTime.dw_താഴെയുള്ള_Date_Time)
	    - 116444736000000000) / 10000000;
    എങ്കിൽ (dict_add_സംഖ്യ(item, "time", time) == FAIL)
	പോകുക theend;

    getftypewfd(wfd, &s);
    എങ്കിൽ (dict_add_string_len(item, "type", s.string, (പൂർണ്ണസംഖ്യ)s.length) == FAIL)
	പോകുക theend;
    എങ്കിൽ (dict_add_string(item, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	പോകുക theend;

    എങ്കിൽ (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	പോകുക theend;
    എങ്കിൽ (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	പോകുക theend;

    തിരികെ item;

theend:
    dict_unref(item);
    തിരികെ ശൂന്യ;
}
# അല്ലെങ്കിൽ
    സ്ഥിതം dict_T *
create_readdirex_item(char_u *path, char_u *name)
{
    dict_T	*item;
    അക്ഷരം	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    പൂർണ്ണസംഖ്യ		ret, link = FALSE;
    varnumber_T	size;
    char_u	permbuf[] = "---------";
    string_T	q = {ശൂന്യ, 0};
    ഘടന passwd *pw;
    ഘടന group  *gr;

    item = dict_alloc();
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ ശൂന്യ;
    item->dv_refcount++;

    pathlen = STRLEN(path);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    എങ്കിൽ (p == ശൂന്യ)
	പോകുക theend;
    എങ്കിൽ (pathlen > 0 && path[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", path, name);
    അല്ലെങ്കിൽ
	vim_snprintf(p, len, "%s/%s", path, name);
    ret = mch_lstat(p, &st);
    എങ്കിൽ (ret >= 0 && S_ISLNK(st.st_mode))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	എങ്കിൽ (ret < 0)
	    STR_LITERAL_SET(q, "link");
    }
    vim_free(p);

    എങ്കിൽ (dict_add_string_len(item, "name", name, (പൂർണ്ണസംഖ്യ)namelen) == FAIL)
	പോകുക theend;

    എങ്കിൽ (ret >= 0)
    {
	size = (varnumber_T)st.st_size;
	എങ്കിൽ (S_ISDIR(st.st_mode))
	    size = 0;
	// non-perfect check for overflow
	അല്ലെങ്കിൽ എങ്കിൽ ((off_T)size != (off_T)st.st_size)
	    size = -2;
	എങ്കിൽ (dict_add_സംഖ്യ(item, "size", size) == FAIL)
	    പോകുക theend;
	എങ്കിൽ (dict_add_സംഖ്യ(item, "time", (varnumber_T)st.st_mtime) == FAIL)
	    പോകുക theend;

	എങ്കിൽ (link)
	{
	    എങ്കിൽ (S_ISDIR(st.st_mode))
		STR_LITERAL_SET(q, "linkd");
	    അല്ലെങ്കിൽ
		STR_LITERAL_SET(q, "link");
	}
	അല്ലെങ്കിൽ
	{
	    q.string = getftypest(&st);
	    q.length = STRLEN(q.string);
	}
	എങ്കിൽ (dict_add_string_len(item, "type", q.string, (പൂർണ്ണസംഖ്യ)q.length) == FAIL)
	    പോകുക theend;
	എങ്കിൽ (dict_add_string(item, "perm", getfpermst(&st, permbuf)) == FAIL)
	    പോകുക theend;

	pw = getpwuid(st.st_uid);
	എങ്കിൽ (pw == ശൂന്യ)
	    STR_LITERAL_SET(q, "");
	അല്ലെങ്കിൽ
	{
	    q.string = (char_u *)pw->pw_name;
	    q.length = STRLEN(q.string);
	}
	എങ്കിൽ (dict_add_string_len(item, "user", q.string, (പൂർണ്ണസംഖ്യ)q.length) == FAIL)
	    പോകുക theend;
#  എങ്കിൽ !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	എങ്കിൽ (gr == ശൂന്യ)
	    STR_LITERAL_SET(q, "");
	അല്ലെങ്കിൽ
	{
	    q.string = (char_u *)gr->gr_name;
	    q.length = STRLEN(q.string);
	}
#  endif
	എങ്കിൽ (dict_add_string_len(item, "group", q.string, (പൂർണ്ണസംഖ്യ)q.length) == FAIL)
	    പോകുക theend;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (dict_add_സംഖ്യ(item, "size", -1) == FAIL)
	    പോകുക theend;
	എങ്കിൽ (dict_add_സംഖ്യ(item, "time", -1) == FAIL)
	    പോകുക theend;
	എങ്കിൽ (q.string == ശൂന്യ)
	{
	    എങ്കിൽ (dict_add_string_len(item, "type", (char_u *)"", 0) == FAIL)
		പോകുക theend;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (dict_add_string_len(item, "type", q.string, (പൂർണ്ണസംഖ്യ)q.length) == FAIL)
		പോകുക theend;
	}
	എങ്കിൽ (dict_add_string_len(item, "perm", (char_u *)"", 0) == FAIL)
	    പോകുക theend;
	എങ്കിൽ (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	    പോകുക theend;
	എങ്കിൽ (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	    പോകുക theend;
    }
    തിരികെ item;

theend:
    dict_unref(item);
    തിരികെ ശൂന്യ;
}
# endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
compare_readdirex_item(സ്ഥിരം ശൂന്യം *p1, സ്ഥിരം ശൂന്യം *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    എങ്കിൽ (readdirex_sort == READDIR_SORT_BYTE)
	തിരികെ STRCMP(name1, name2);
    എങ്കിൽ (readdirex_sort == READDIR_SORT_IC)
	തിരികെ STRICMP(name1, name2);

    തിരികെ STRCOLL(name1, name2);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
compare_readdir_item(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    എങ്കിൽ (readdirex_sort == READDIR_SORT_BYTE)
	തിരികെ STRCMP(*(അക്ഷരം **)s1, *(അക്ഷരം **)s2);
    എങ്കിൽ (readdirex_sort == READDIR_SORT_IC)
	തിരികെ STRICMP(*(അക്ഷരം **)s1, *(അക്ഷരം **)s2);

    തിരികെ STRCOLL(*(അക്ഷരം **)s1, *(അക്ഷരം **)s2);
}
#endif

#എങ്കിൽ defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    പൂർണ്ണസംഖ്യ
readdir_core(
    garray_T	*gap,
    char_u	*path,
    പൂർണ്ണസംഖ്യ		withattr UNUSED,
    ശൂന്യം	*context,
    പൂർണ്ണസംഖ്യ		(*checkitem)(ശൂന്യം *context, ശൂന്യം *item),
    പൂർണ്ണസംഖ്യ		sort)
{
    പൂർണ്ണസംഖ്യ			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    പൂർണ്ണസംഖ്യ			ok;
    HANDLE		hFind = INVALID_HANDLE_മൂല്യം;
    WIN32_FIND_DATAW    wfd;
    WCHAR		*wn = ശൂന്യ;	// UTF-16 name, NULL when not used.
    char_u		*p_end;
# അല്ലെങ്കിൽ
    DIR			*dirp;
    ഘടന dirent	*dp;
# endif

    ga_init2(gap, വലുപ്പം(ശൂന്യം *), 20);

# ifdef FEAT_EVAL
#  define FREE_ITEM(item)   ചെയ്യുക { \
	എങ്കിൽ (withattr) \
	    dict_unref((dict_T*)(item)); \
	അല്ലെങ്കിൽ \
	    vim_free(item); \
    } ആകുമ്പോൾ (0)

    readdirex_sort = READDIR_SORT_BYTE;
# അല്ലെങ്കിൽ
#  define FREE_ITEM(item)   vim_free(item)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ FAIL;
    STRNCPY(buf, path, MAXPATHL-5);
    p = p_end = buf + STRLEN(buf);
    MB_PTR_BACK(buf, p);
    എങ്കിൽ (*p == '\\' || *p == '/')
	p_end = p;
    STRCPY(p_end, "\\*");

    wn = enc_to_utf16(buf, ശൂന്യ);
    എങ്കിൽ (wn != ശൂന്യ)
	hFind = FindFirstFileW(wn, &wfd);
    ok = (hFind != INVALID_HANDLE_മൂല്യം);
    എങ്കിൽ (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    അല്ലെങ്കിൽ
    {
	ആകുമ്പോൾ (ok)
	{
	    പൂർണ്ണസംഖ്യ	    ignore;
	    ശൂന്യം    *item;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    എങ്കിൽ (ignore)
	    {
		ok = FindNextFileW(hFind, &wfd);
		തുടരുക;
	    }
#  ifdef FEAT_EVAL
	    എങ്കിൽ (withattr)
		item = (ശൂന്യം*)create_readdirex_item(&wfd);
	    അല്ലെങ്കിൽ
#  endif
		item = (ശൂന്യം*)utf16_to_enc(wfd.cFileName, ശൂന്യ);
	    എങ്കിൽ (item == ശൂന്യ)
	    {
		failed = TRUE;
		നിർത്തുക;
	    }

	    എങ്കിൽ (!ignore && checkitem != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ r = checkitem(context, item);

		എങ്കിൽ (r < 0)
		{
		    FREE_ITEM(item);
		    നിർത്തുക;
		}
		എങ്കിൽ (r == 0)
		    ignore = TRUE;
	    }

	    എങ്കിൽ (!ignore)
	    {
		എങ്കിൽ (ga_grow(gap, 1) == OK)
		    ((ശൂന്യം**)gap->ga_data)[gap->ga_len++] = item;
		അല്ലെങ്കിൽ
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    നിർത്തുക;
		}
	    }
	    അല്ലെങ്കിൽ
		FREE_ITEM(item);

	    ok = FindNextFileW(hFind, &wfd);
	}
	FindClose(hFind);
    }

    vim_free(buf);
    vim_free(wn);
# അല്ലെങ്കിൽ	// MSWIN
    dirp = opendir((അക്ഷരം *)path);
    എങ്കിൽ (dirp == ശൂന്യ)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (;;)
	{
	    പൂർണ്ണസംഖ്യ	    ignore;
	    ശൂന്യം    *item;

	    dp = readdir(dirp);
	    എങ്കിൽ (dp == ശൂന്യ)
		നിർത്തുക;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    എങ്കിൽ (ignore)
		തുടരുക;
#  ifdef FEAT_EVAL
	    എങ്കിൽ (withattr)
		item = (ശൂന്യം*)create_readdirex_item(path, p);
	    അല്ലെങ്കിൽ
#  endif
		item = (ശൂന്യം*)vim_strsave(p);
	    എങ്കിൽ (item == ശൂന്യ)
	    {
		failed = TRUE;
		നിർത്തുക;
	    }

	    എങ്കിൽ (checkitem != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ r = checkitem(context, item);

		എങ്കിൽ (r < 0)
		{
		    FREE_ITEM(item);
		    നിർത്തുക;
		}
		എങ്കിൽ (r == 0)
		    ignore = TRUE;
	    }

	    എങ്കിൽ (!ignore)
	    {
		എങ്കിൽ (ga_grow(gap, 1) == OK)
		    ((ശൂന്യം**)gap->ga_data)[gap->ga_len++] = item;
		അല്ലെങ്കിൽ
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    നിർത്തുക;
		}
	    }
	    അല്ലെങ്കിൽ
		FREE_ITEM(item);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_ITEM

    എങ്കിൽ (!failed && gap->ga_len > 0 && sort > READDIR_SORT_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_sort = sort;
	എങ്കിൽ (withattr)
	    qsort((ശൂന്യം*)gap->ga_data, (size_t)gap->ga_len, വലുപ്പം(dict_T*),
		    compare_readdirex_item);
	അല്ലെങ്കിൽ
	    qsort((ശൂന്യം*)gap->ga_data, (size_t)gap->ga_len, വലുപ്പം(char_u *),
		    compare_readdir_item);
# അല്ലെങ്കിൽ
	    sort_strings((char_u **)gap->ga_data, gap->ga_len);
# endif
    }

    തിരികെ failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    പൂർണ്ണസംഖ്യ
delete_recursive(char_u *name)
{
    പൂർണ്ണസംഖ്യ result = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    എങ്കിൽ (
# എങ്കിൽ defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# അല്ലെങ്കിൽ
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *exp = vim_strsave(name);
	garray_T    ga;

	എങ്കിൽ (exp == ശൂന്യ)
	    തിരികെ -1;
	എങ്കിൽ (readdir_core(&ga, exp, FALSE, ശൂന്യ, ശൂന്യ, READDIR_SORT_NONE) == OK)
	{
	    പൂർണ്ണസംഖ്യ	len = vim_snprintf((അക്ഷരം *)NameBuff, MAXPATHL, "%s/", exp);
	    പൂർണ്ണസംഖ്യ	i;

	    വേണ്ടി (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((അക്ഷരം *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_data)[i]);
		എങ്കിൽ (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    result = -1;
	    }
	    ga_clear_strings(&ga);
	    എങ്കിൽ (mch_rmdir(exp) != 0)
		result = -1;
	}
	അല്ലെങ്കിൽ
	    result = -1;
	vim_free(exp);
    }
    അല്ലെങ്കിൽ
	result = mch_remove(name) == 0 ? 0 : -1;

    തിരികെ result;
}
#endif

#എങ്കിൽ defined(TEMPDIRNAMES)
സ്ഥിതം long	temp_എണ്ണം = 0;		// Temp filename counter.

# എങ്കിൽ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   സ്ഥിതം ശൂന്യം
vim_opentempdir(ശൂന്യം)
{
    DIR *dp = ശൂന്യ;

    എങ്കിൽ (vim_tempdir_dp != ശൂന്യ)
	തിരികെ;

    dp = opendir((സ്ഥിരം അക്ഷരം*)vim_tempdir);
    എങ്കിൽ (dp == ശൂന്യ)
	തിരികെ;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   സ്ഥിതം ശൂന്യം
vim_closetempdir(ശൂന്യം)
{
    എങ്കിൽ (vim_tempdir_dp == ശൂന്യ)
	തിരികെ;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = ശൂന്യ;
}

/*
 * Return true if the temp directory we created is gone.
 */
    സ്ഥിതം bool
vim_tempdir_gone(ശൂന്യം)
{
    stat_T	st;

    എങ്കിൽ (vim_tempdir_dp == ശൂന്യ)
	തിരികെ false;
    തിരികെ fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    ശൂന്യം
vim_deltempdir(ശൂന്യം)
{
    എങ്കിൽ (vim_tempdir == ശൂന്യ)
	തിരികെ;

# എങ്കിൽ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_closetempdir();
# endif
    // remove the trailing path separator
    gettail(vim_tempdir)[-1] = NUL;
    delete_recursive(vim_tempdir);
    VIM_CLEAR(vim_tempdir);
}

/*
 * Directory "tempdir" was created.  Expand this name to a full path and put
 * it in "vim_tempdir".  This avoids that using ":cd" would confuse us.
 * "tempdir" must be no longer than MAXPATHL.
 */
    സ്ഥിതം ശൂന്യം
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (vim_FullName(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    എങ്കിൽ (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += വലുപ്പം(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# എങ്കിൽ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_opentempdir();
# endif
    vim_free(buf);
}
#endif

/*
 * vim_tempname(): Return a unique name that can be used for a temp file.
 *
 * The temp file is NOT guaranteed to be created.  If "keep" is FALSE it is
 * guaranteed to NOT be created.
 *
 * The returned pointer is to allocated memory.
 * The returned pointer is NULL if no valid name was found.
 */
    char_u  *
vim_tempname(
    പൂർണ്ണസംഖ്യ	    extra_char UNUSED,  // char to use in the name instead of '?'
    പൂർണ്ണസംഖ്യ	    keep UNUSED)
{
#ifdef USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#അല്ലെങ്കിലെങ്കിൽ defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#അല്ലെങ്കിൽ
    char_u	itmp[TEMPNAMELEN];
#endif

#ifdef TEMPDIRNAMES
    സ്ഥിതം അക്ഷരം	*(tempdirs[]) = {TEMPDIRNAMES};
    പൂർണ്ണസംഖ്യ		i;
# ifndef EEXIST
    stat_T	st;
# endif

# എങ്കിൽ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    എങ്കിൽ (vim_tempdir != ശൂന്യ && vim_tempdir_gone())
    {
	vim_closetempdir();
	VIM_CLEAR(vim_tempdir);
    }
# endif

    /*
     * This will create a directory for private use by this instance of Vim.
     * This is done once, and the same directory is used for all temp files.
     * This method avoids security problems because of symlink attacks et al.
     * It's also a bit faster, because we only need to check for an existing
     * file when creating the directory and not for each temp file.
     */
    എങ്കിൽ (vim_tempdir == ശൂന്യ)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    long	nr;
	    long	off;
# endif
	    size_t	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    എങ്കിൽ (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		എങ്കിൽ (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += വലുപ്പം(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  എങ്കിൽ defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    എങ്കിൽ (mkdtemp((അക്ഷരം *)itmp) != ശൂന്യ)
			vim_settempdir(itmp);
#  എങ്കിൽ defined(UNIX) || defined(VMS)
		    (ശൂന്യം)umask(umask_save);
#  endif
		}
# അല്ലെങ്കിൽ
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (long)time(ശൂന്യ)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		വേണ്ടി (off = 0; off < 10000L; ++off)
		{
		    പൂർണ്ണസംഖ്യ		r;
#  എങ്കിൽ defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((അക്ഷരം *)itmp + itmplen, വലുപ്പം(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    എങ്കിൽ (mch_stat((അക്ഷരം *)itmp, &st) >= 0)
			തുടരുക;
#  endif
#  എങ്കിൽ defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  എങ്കിൽ defined(UNIX) || defined(VMS)
		    (ശൂന്യം)umask(umask_save);
#  endif
		    എങ്കിൽ (r == 0)
		    {
			vim_settempdir(itmp);
			നിർത്തുക;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    എങ്കിൽ (errno != EEXIST)
#  endif
			നിർത്തുക;
		}
# endif // HAVE_MKDTEMP
		എങ്കിൽ (vim_tempdir != ശൂന്യ)
		    നിർത്തുക;
	    }
	}
    }

    എങ്കിൽ (vim_tempdir != ശൂന്യ)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	പൂർണ്ണസംഖ്യ itmplen = vim_snprintf((അക്ഷരം *)itmp, വലുപ്പം(itmp), "%s%ld", vim_tempdir, temp_എണ്ണം++);
	തിരികെ vim_strnsave(itmp, (size_t)itmplen);
    }

    തിരികെ ശൂന്യ;

#അല്ലെങ്കിൽ // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wszTempFile[_MAX_PATH + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    long	i;

    wcscpy(itmp, L"");
    എങ്കിൽ (GetTempPathW(_MAX_PATH, wszTempFile) == 0)
    {
	wszTempFile[0] = L'.';	// GetTempPathW() failed, use current dir
	wszTempFile[1] = L'\\';
	wszTempFile[2] = NUL;
    }
    wcscpy(buf4, L"VIM");

    // randomize the name to avoid collisions
    i = mch_get_pid() + extra_char;
    buf4[1] = chartab[i % 36];
    buf4[2] = chartab[(i / 36) % 36];
    എങ്കിൽ (GetTempFileNameW(wszTempFile, buf4, 0, itmp) == 0)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(ശൂന്യം)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_to_enc(itmp, ശൂന്യ);
    എങ്കിൽ (retval == ശൂന്യ)
	തിരികെ ശൂന്യ;
    shname = gettail(p_sh);
    എങ്കിൽ ((*p_shcf == '-' && !(strstr((അക്ഷരം *)shname, "powershell") != ശൂന്യ
			     || strstr((അക്ഷരം *)shname, "pwsh") != ശൂന്യ ))
								    || p_ssl)
	വേണ്ടി (p = retval; *p; ++p)
	    എങ്കിൽ (*p == '\\')
		*p = '/';
    തിരികെ retval;

# അല്ലെങ്കിൽ // MSWIN
    പൂർണ്ണസംഖ്യ		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((അക്ഷരം *)itmp);
    എങ്കിൽ (p == ശൂന്യ || *p == NUL)
	തിരികെ ശൂന്യ;
#  അല്ലെങ്കിൽ

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((അക്ഷരം *)itmp, വലുപ്പം(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (അക്ഷരം *)itmp);
    എങ്കിൽ (p != ശൂന്യ)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((അക്ഷരം *)itmp, വലുപ്പം(itmp), "%s.txt", p);
	സ്വതന്ത്രം(p);
    }
    അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
#   അല്ലെങ്കിൽ
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    എങ്കിൽ ((p = vim_strchr(itmp, '?')) != ശൂന്യ)
	*p = extra_char;
    എങ്കിൽ (mktemp((അക്ഷരം *)itmp) == ശൂന്യ)
	തിരികെ ശൂന്യ;
#   endif
#  endif

    തിരികെ vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#endif // TEMPDIRNAMES
}

#എങ്കിൽ defined(BACKSLASH_IN_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    ശൂന്യം
forward_slash(char_u *fname)
{
    char_u	*p;

    എങ്കിൽ (path_with_url(fname))
	തിരികെ;
    വേണ്ടി (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	എങ്കിൽ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '\\')
	    *p = '/';
}
#endif

/*
 * Try matching a filename with a "pattern" ("prog" is NULL), or use the
 * precompiled regprog "prog" ("pattern" is NULL).  That avoids calling
 * vim_regcomp() often.
 * Used for autocommands and 'wildignore'.
 * Returns TRUE if there is a match, FALSE otherwise.
 */
    പൂർണ്ണസംഖ്യ
match_file_pat(
    char_u	*pattern,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*tail,			// tail of path
    പൂർണ്ണസംഖ്യ		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		result = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    എങ്കിൽ (prog != ശൂന്യ)
	regmatch.regprog = *prog;
    അല്ലെങ്കിൽ
	regmatch.regprog = vim_regcomp(pattern, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    എങ്കിൽ (regmatch.regprog != ശൂന്യ
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != ശൂന്യ
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, tail, (colnr_T)0))))
	result = TRUE;

    എങ്കിൽ (prog != ശൂന്യ)
	*prog = regmatch.regprog;
    അല്ലെങ്കിൽ
	vim_regfree(regmatch.regprog);
    തിരികെ result;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    പൂർണ്ണസംഖ്യ
match_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*tail;
    char_u	*regpat;
    അക്ഷരം	allow_dirs;
    പൂർണ്ണസംഖ്യ		match;
    char_u	*p;

    tail = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    ആകുമ്പോൾ (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_to_reg_pat(buf, ശൂന്യ, &allow_dirs, FALSE);
	എങ്കിൽ (regpat == ശൂന്യ)
	    നിർത്തുക;
	match = match_file_pat(regpat, ശൂന്യ, ffname, sfname,
						       tail, (പൂർണ്ണസംഖ്യ)allow_dirs);
	vim_free(regpat);
	എങ്കിൽ (match)
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Convert the given pattern "pat" which has shell style wildcards in it, into
 * a regular expression, and return the result in allocated memory.  If there
 * is a directory path separator to be matched, then TRUE is put in
 * allow_dirs, otherwise FALSE is put there -- webb.
 * Handle backslashes before special characters, like "\*" and "\ ".
 *
 * no_bslash only makes a difference, when BACKSLASH_IN_FILENAME is defined
 *
 * Returns NULL when out of memory.
 */
    char_u *
file_pat_to_reg_pat(
    char_u	*pat,
    char_u	*pat_end,	// first char after pattern or NULL
    അക്ഷരം	*allow_dirs,	// Result passed back out in here
    പൂർണ്ണസംഖ്യ		no_bslash UNUSED) // Don't use a backward slash as pathsep
{
    പൂർണ്ണസംഖ്യ		size = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		nested = 0;
    പൂർണ്ണസംഖ്യ		add_dollar = TRUE;

    എങ്കിൽ (allow_dirs != ശൂന്യ)
	*allow_dirs = FALSE;
    എങ്കിൽ (pat_end == ശൂന്യ)
	pat_end = pat + STRLEN(pat);

    വേണ്ടി (p = pat; p < pat_end; p++)
    {
	തിരഞ്ഞെടുപ്പ് (*p)
	{
	    അവസ്ഥ '*':
	    അവസ്ഥ '.':
	    അവസ്ഥ ',':
	    അവസ്ഥ '{':
	    അവസ്ഥ '}':
	    അവസ്ഥ '~':
		size += 2;	// extra backslash
		നിർത്തുക;
#ifdef BACKSLASH_IN_FILENAME
	    അവസ്ഥ '\\':
	    അവസ്ഥ '/':
		size += 4;	// could become "[\/]"
		നിർത്തുക;
#endif
	    സ്ഥിരസ്ഥിതി:
		size++;
		എങ്കിൽ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++size;
		}
		നിർത്തുക;
	}
    }
    reg_pat = alloc(size + 1);
    എങ്കിൽ (reg_pat == ശൂന്യ)
	തിരികെ ശൂന്യ;

    i = 0;

    എങ്കിൽ (pat[0] == '*')
	ആകുമ്പോൾ (pat[0] == '*' && pat < pat_end - 1)
	    pat++;
    അല്ലെങ്കിൽ
	reg_pat[i++] = '^';
    endp = pat_end - 1;
    എങ്കിൽ (endp >= pat && *endp == '*')
    {
	ആകുമ്പോൾ (endp - pat > 0 && *endp == '*')
	    endp--;
	add_dollar = FALSE;
    }
    വേണ്ടി (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	തിരഞ്ഞെടുപ്പ് (*p)
	{
	    അവസ്ഥ '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		ആകുമ്പോൾ (p[1] == '*')	// "**" matches like "*"
		    ++p;
		നിർത്തുക;
	    അവസ്ഥ '.':
	    അവസ്ഥ '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		നിർത്തുക;
	    അവസ്ഥ '?':
		reg_pat[i++] = '.';
		നിർത്തുക;
	    അവസ്ഥ '\\':
		എങ്കിൽ (p[1] == NUL)
		    നിർത്തുക;
#ifdef BACKSLASH_IN_FILENAME
		എങ്കിൽ (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    എങ്കിൽ ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			എങ്കിൽ (allow_dirs != ശൂന്യ)
			    *allow_dirs = TRUE;
			നിർത്തുക;
		    }
		}
#endif
		// Undo escaping from ExpandEscape():
		// foo\?bar -> foo?bar
		// foo\%bar -> foo%bar
		// foo\,bar -> foo,bar
		// foo\ bar -> foo bar
		// Don't unescape \, * and others that are also special in a
		// regexp.
		// An escaped { must be unescaped since we use magic not
		// verymagic.  Use "\\\{n,m\}"" to get "\{n,m}".
		എങ്കിൽ (*++p == '?'
#ifdef BACKSLASH_IN_FILENAME
			&& no_bslash
#endif
			)
		    reg_pat[i++] = '?';
		അല്ലെങ്കിൽ
		    എങ്കിൽ (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    അല്ലെങ്കിൽ എങ്കിൽ (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (allow_dirs != ശൂന്യ && vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
				&& (!no_bslash || *p != '\\')
#endif
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			എങ്കിൽ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		നിർത്തുക;
#ifdef BACKSLASH_IN_FILENAME
	    അവസ്ഥ '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		എങ്കിൽ (allow_dirs != ശൂന്യ)
		    *allow_dirs = TRUE;
		നിർത്തുക;
#endif
	    അവസ്ഥ '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		നിർത്തുക;
	    അവസ്ഥ '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		നിർത്തുക;
	    അവസ്ഥ ',':
		എങ്കിൽ (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		അല്ലെങ്കിൽ
		    reg_pat[i++] = ',';
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		എങ്കിൽ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		അല്ലെങ്കിൽ എങ്കിൽ (allow_dirs != ശൂന്യ && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		നിർത്തുക;
	}
    }
    എങ്കിൽ (add_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    എങ്കിൽ (nested != 0)
    {
	എങ്കിൽ (nested < 0)
	    emsg(_(e_missing_open_curly));
	അല്ലെങ്കിൽ
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    തിരികെ reg_pat;
}

#എങ്കിൽ defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
read_eintr(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, size_t bufsize)
{
    long ret;

    വേണ്ടി (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	എങ്കിൽ (ret >= 0 || errno != EINTR)
	    നിർത്തുക;
    }
    തിരികെ ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
write_eintr(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, size_t bufsize)
{
    long    ret = 0;
    long    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    ആകുമ്പോൾ (ret < (long)bufsize)
    {
	wlen = vim_write(fd, (അക്ഷരം *)buf + ret, bufsize - ret);
	എങ്കിൽ (wlen < 0)
	{
	    എങ്കിൽ (errno != EINTR)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	    ret += wlen;
    }
    തിരികെ ret;
}
#endif
