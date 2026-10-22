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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#ਜੇ_ਅੰਤ
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#ਜੇ_ਅੰਤ
#ਜੇ defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#ਜੇ_ਅੰਤ

// Is there any system that doesn't have access()?
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NONASCII_ਨਕਾਬ (((long_u)-1 / 0xFF) * 0x80)

#ਜੇ defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#ਜੇ_ਅੰਤ

ਸਥਿਰ char_u *ਅਗਲਾ_fenc(char_u **pp, ਪੂਰਨ *alloced);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ char_u *readfile_charconvert(char_u *fname, char_u *fenc, ਪੂਰਨ *fdp);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
ਸਥਿਰ char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, ਲੰਮਾ *sizep, off_T *filesizep, ਪੂਰਨ newfile, char_u *fname, ਪੂਰਨ *did_ask);
#ਜੇ_ਅੰਤ
ਸਥਿਰ linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
ਸਥਿਰ char_u *check_for_bom(char_u *p, ਲੰਮਾ ਮਾਪ, ਪੂਰਨ *lenp, ਪੂਰਨ flags);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਪੂਰਨ readdirex_ਛਾਂਟੋ;
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    ਪੂਰਨ		attr)
{
    ਪੂਰਨ		ਸੁਨੇਹਾ_scroll_save;
    ਪੂਰਨ		prev_ਸੁਨੇਹਾ_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ;
    size_t	len;

    ਜੇ (ਸੁਨੇਹਾ_silent != 0)
	ਵਾਪਸ;
    ਸੁਨੇਹਾ_ਜੋੜੋ_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    ਜੇ (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    ਜੇ (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    ਸੁਨੇਹਾ_scroll_save = ਸੁਨੇਹਾ_scroll;
    ਜੇ (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	ਸੁਨੇਹਾ_scroll = FALSE;
    ਜੇ (!ਸੁਨੇਹਾ_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਜੇ (prev_ਸੁਨੇਹਾ_ਖਾਨਾ != 0 && ਸੁਨੇਹਾ_ਖਾਨਾ == 0)
	ਸੁਨੇਹਾ_putchar('\r');  // overwrite any previous message.
    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll_save;
    ਸੁਨੇਹਾ_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    ਸੁਨੇਹਾ_outtrans_attr(ਸੁਨੇਹਾ_may_trunc(FALSE, IObuff), attr);
    ਸੁਨੇਹਾ_clr_eos();
    ਬਾਹਰਲਾ_flush();
    ਸੁਨੇਹਾ_scrolled_ign = FALSE;
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
    ਪੂਰਨ
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_skip,
    linenr_T	ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_read,
    exarg_T	*eap,			// can be NULL!
    ਪੂਰਨ		flags)
{
    ਪੂਰਨ		retval = FAIL;	// jump to "theend" instead of returning
    ਪੂਰਨ		fd = 0;
    ਪੂਰਨ		newfile = (flags & READ_NEW);
    ਪੂਰਨ		check_readonly;
    ਪੂਰਨ		filtering = (flags & READ_FILTER);
    ਪੂਰਨ		read_stdin = (flags & READ_STDIN);
    ਪੂਰਨ		read_ਬਫਰ = (flags & READ_ਬਫਰ);
    ਪੂਰਨ		read_fifo = (flags & READ_FIFO);
    ਪੂਰਨ		ਸੈੱਟ_options = newfile || read_ਬਫਰ
					   || (eap != ਨਿਰਾ && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_ਖਾਨਾ = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = ਨਿਰਾ;		// pointer into read buffer
    char_u	*ਬਫਰ = ਨਿਰਾ;		// read buffer
    char_u	*new_ਬਫਰ = ਨਿਰਾ;	// init to shut up gcc
    char_u	*ਸਤਰ_ਸ਼ੁਰੂ = ਨਿਰਾ;	// init to shut up gcc
    ਪੂਰਨ		wasempty;		// buffer was empty before reading
    colnr_T	len;
    ਲੰਮਾ	ਮਾਪ = 0;
    char_u	*p;
    off_T	filesize = 0;
    ਪੂਰਨ		skip_read = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_ਗਿਣਤੀ = 0;     // counter
    char_u	*cryptkey = ਨਿਰਾ;
    ਪੂਰਨ		did_ask_for_ਕੁੰਜੀ = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    ਪੂਰਨ		read_undo_file = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਵੰਡੋ = 0;		// number of split lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    ਪੂਰਨ		error = FALSE;		// errors encountered
    ਪੂਰਨ		ff_error = EOL_UNKNOWN; // file format with errors
    ਲੰਮਾ	linerest = 0;		// remaining chars in line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਪੂਰਨ		perm = 0;
    ਪੂਰਨ		ਅਦਲਾ_ਬਦਲੀ_ਬਹੁਲਕ = -1;		// protection bits for swap file
#ਨਹੀਂਤਾਂ
    ਪੂਰਨ		perm;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		fileformat = 0;		// end-of-line format
    ਪੂਰਨ		keep_fileformat = FALSE;
    stat_T	st;
    ਪੂਰਨ		file_readonly;
    linenr_T	skip_ਗਿਣਤੀ = 0;
    linenr_T	read_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		ਸੁਨੇਹਾ_save = ਸੁਨੇਹਾ_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    ਪੂਰਨ		try_mac;
    ਪੂਰਨ		try_dos;
    ਪੂਰਨ		try_unix;
    ਪੂਰਨ		file_rewind = FALSE;
    ਪੂਰਨ		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    ਪੂਰਨ		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    ਪੂਰਨ		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = ਨਿਰਾ;	// name of 'charconvert' output file
    ਪੂਰਨ		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    ਪੂਰਨ		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_ਅਗਲਾ = ਨਿਰਾ;	// next item in 'fencs' or NULL
    ਪੂਰਨ		advance_fenc = FALSE;
    ਲੰਮਾ	real_ਮਾਪ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    ਪੂਰਨ		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		converted = FALSE;	// TRUE if conversion done
    ਪੂਰਨ		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    ਪੂਰਨ		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_ਸ਼ੁਰੂ;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    ਪੂਰਨ		using_b_ffname;
    ਪੂਰਨ		using_b_fname;
    ਸਥਿਰ ਅੱਖਰ *ਸੁਨੇਹਾ_is_a_directory = N_("is a directory");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਪੂਰਨ		eof = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
    ਪੂਰਨ		may_need_lseek = FALSE;
#ਜੇ_ਅੰਤ
    size_t	fnamelen = 0;

    curbuf->b_au_did_filetype = false; // reset before triggering any autocommands

    curbuf->b_no_eol_lnum = 0;	// in case it was set by the previous read

    /*
     * If there is no file name yet, use the one for the read file.
     * BF_NOTEDITED is set to reflect this.
     * Don't do this for a read from a filter.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    ਜੇ (curbuf->b_ffname == ਨਿਰਾ
	    && !filtering
	    && fname != ਨਿਰਾ
	    && vim_strchr(p_cpo, CPO_FNAMER) != ਨਿਰਾ
	    && !(flags & READ_DUMMY))
    {
	ਜੇ (ਸੈੱਟ_rw_fname(fname, sfname) == FAIL)
	    ਜਾਓ theend;
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
    ਜੇ (sfname == ਨਿਰਾ)
	sfname = fname;
#ਜੇ defined(UNIX)
    fname = sfname;
#ਜੇ_ਅੰਤ

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    ਜੇ (!filtering && !read_stdin && !read_ਬਫਰ)
    {
	orig_ਸ਼ੁਰੂ = curbuf->b_op_ਸ਼ੁਰੂ;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_ਸ਼ੁਰੂ.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = 0;

	ਜੇ (newfile)
	{
	    ਜੇ (apply_autocmds_exarg(EVENT_BUFREADCMD, ਨਿਰਾ, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਜੇ (aborting())
		    retval = FAIL;
#ਜੇ_ਅੰਤ
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		ਜੇ (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		ਜਾਓ theend;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, ਨਿਰਾ, eap))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#ਨਹੀਂਤਾਂ
	    retval = OK;
#ਜੇ_ਅੰਤ
	    ਜਾਓ theend;
	}

	curbuf->b_op_ਸ਼ੁਰੂ = orig_ਸ਼ੁਰੂ;

	ਜੇ (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    ਜਾਓ theend;
	}
    }

    ਜੇ ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	ਸੁਨੇਹਾ_scroll = FALSE;	// overwrite previous file message
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_scroll = TRUE;	// don't overwrite previous file message

    ਜੇ (fname != ਨਿਰਾ && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	ਜੇ (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    ਸੁਨੇਹਾ_ਸਿਰਾ();
	    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
	    ਜਾਓ theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	ਜੇ (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(ਸੁਨੇਹਾ_is_a_directory), 0);
	    ਸੁਨੇਹਾ_ਸਿਰਾ();
	    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
	    retval = NOTDONE;
	    ਜਾਓ theend;
	}
    }
#ਜੇ defined(UNIX) || defined(VMS)
    ਜੇ (!read_stdin && fname != ਨਿਰਾ)
	perm = mch_getperm(fname);
#ਜੇ_ਅੰਤ

    ਜੇ (!read_stdin && !read_ਬਫਰ && !read_fifo)
    {
#ਜੇ defined(UNIX) || defined(VMS)
	ਜੇ (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
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
	    ਜੇ (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(ਸੁਨੇਹਾ_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    ਨਹੀਂਤਾਂ
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    ਸੁਨੇਹਾ_ਸਿਰਾ();
	    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
	    ਜਾਓ theend;
	}
#ਜੇ_ਅੰਤ
#ਜੇ defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	ਜੇ (!p_odev && mch_nodetype(fname) == ਗੰਢ_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    ਸੁਨੇਹਾ_ਸਿਰਾ();
	    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
	    ਜਾਓ theend;
	}
#ਜੇ_ਅੰਤ
    }

    // Set default or forced 'fileformat' and 'binary'.
    ਸੈੱਟ_file_options(ਸੈੱਟ_options, eap);

    /*
     * When opening a new file we take the readonly flag from the file.
     * Default is r/w, can be set to r/o below.
     * Don't reset it when in readonly mode
     * Only set/reset b_p_ro when BF_CHECK_RO is set.
     */
    check_readonly = (newfile && (curbuf->b_flags & BF_CHECK_RO));
    ਜੇ (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    ਜੇ (newfile && !read_stdin && !read_ਬਫਰ && !read_fifo)
    {
	// Remember time of file.
	ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) >= 0)
	{
	    buf_store_ਸਮਾਂ(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	    filesize_disk = st.st_size;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
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
	    ਅਦਲਾ_ਬਦਲੀ_ਬਹੁਲਕ = (st.st_ਬਹੁਲਕ & 0644) | 0600;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
	    curbuf->b_fab_rfm = st.st_fab_rfm;
	    curbuf->b_fab_rat = st.st_fab_rat;
	    curbuf->b_fab_mrs = st.st_fab_mrs;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    curbuf->b_mtime = 0;
	    curbuf->b_mtime_ns = 0;
	    curbuf->b_mtime_read = 0;
	    curbuf->b_mtime_read_ns = 0;
	    curbuf->b_orig_ਮਾਪ = 0;
	    curbuf->b_orig_ਬਹੁਲਕ = 0;
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
    ਜੇ (read_stdin)
    {
#ਜੇ defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_ਦੁਆਧਾਰੀ);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (!read_ਬਫਰ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MCH_ACCESS
	ਜੇ (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((ਅੱਖਰ *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0);
#ਨਹੀਂਤਾਂ
	ਜੇ (!newfile
		|| readonlymode
		|| (fd = mch_open((ਅੱਖਰ *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0);
	}
#ਜੇ_ਅੰਤ
    }

    ਜੇ (fd < 0)			    // cannot open at all
    {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX
	ਪੂਰਨ	isdir_f;
#ਜੇ_ਅੰਤ
	ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	ਜੇ (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(ਸੁਨੇਹਾ_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜੇ (newfile)
	    {
		ਜੇ (perm < 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENOENT
			&& errno == ENOENT
#ਜੇ_ਅੰਤ
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
		    ਜੇ (!bt_dontwrite(curbuf))
		    {
			check_need_ਅਦਲਾ_ਬਦਲੀ(newfile);
			// SwapExists autocommand may mess things up
			ਜੇ (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_ਬਦਲਿਆ_ਬਫਰ_or_ਬਫਰ_name));
			    ਜਾਓ theend;
			}
		    }
		    ਜੇ (dir_ਦਾ_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_ਸੁਨੇਹਾ(), 0);
		    ਨਹੀਂਤਾਂ
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#ਜੇ_ਅੰਤ
		    // Set forced 'fileencoding'.
		    ਜੇ (eap != ਨਿਰਾ)
			ਸੈੱਟ_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#ਜੇ defined(FEAT_EVAL)
		    ਜੇ (!aborting())   // autocmds may abort script processing
#ਜੇ_ਅੰਤ
			retval = OK;	    // a new file is not an error
		    ਜਾਓ theend;
		}
		ਨਹੀਂਤਾਂ
		{
		    filemess(curbuf, sfname, (char_u *)(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EFBIG
			    (errno == EFBIG) ? _("[File too big]") :
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EOVERFLOW
			    (errno == EOVERFLOW) ? _("[File too big]") :
#ਜੇ_ਅੰਤ
						_("[Permission Denied]")), 0);
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
		}
	    }

	ਜਾਓ theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    ਜੇ ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    ਜੇ (ਸੈੱਟ_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	ਜੇ (!read_ਬਫਰ)
	{
	    curbuf->b_p_eof = FALSE;
	    curbuf->b_ਸ਼ੁਰੂ_eof = FALSE;
	    curbuf->b_p_eol = TRUE;
	    curbuf->b_ਸ਼ੁਰੂ_eol = TRUE;
	}
	curbuf->b_p_bomb = FALSE;
	curbuf->b_ਸ਼ੁਰੂ_bomb = FALSE;
    }

    // Create a swap file now, so that other Vims are warned that we are
    // editing this file.
    // Don't do this for a "nofile" or "nowrite" buffer type.
    ਜੇ (!bt_dontwrite(curbuf))
    {
	check_need_ਅਦਲਾ_ਬਦਲੀ(newfile);
	ਜੇ (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_ਬਦਲਿਆ_ਬਫਰ_or_ਬਫਰ_name));
	    ਜੇ (!read_ਬਫਰ)
		close(fd);
	    ਜਾਓ theend;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	// Set swap file protection bits after creating it.
	ਜੇ (ਅਦਲਾ_ਬਦਲੀ_ਬਹੁਲਕ > 0 && curbuf->b_ml.ml_mfp != ਨਿਰਾ
			  && curbuf->b_ml.ml_mfp->mf_fname != ਨਿਰਾ)
	{
	    char_u *ਅਦਲਾ_ਬਦਲੀ_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    ਜੇ ((ਅਦਲਾ_ਬਦਲੀ_ਬਹੁਲਕ & 044) == 040)
	    {
		stat_T	ਅਦਲਾ_ਬਦਲੀ_st;

		ਜੇ (mch_stat((ਅੱਖਰ *)ਅਦਲਾ_ਬਦਲੀ_fname, &ਅਦਲਾ_ਬਦਲੀ_st) >= 0
			&& st.st_gid != ਅਦਲਾ_ਬਦਲੀ_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    ਅਦਲਾ_ਬਦਲੀ_ਬਹੁਲਕ &= 0600;
	    }

	    (ਖਾਲੀ)mch_setperm(ਅਦਲਾ_ਬਦਲੀ_fname, (ਲੰਮਾ)ਅਦਲਾ_ਬਦਲੀ_ਬਹੁਲਕ);
	}
#ਜੇ_ਅੰਤ
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_QUIT)
    {
	ਜੇ (!read_ਬਫਰ && !read_stdin)
	    close(fd);
	ਜਾਓ theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_ਸ਼ੁਰੂ = curbuf->b_op_ਸ਼ੁਰੂ;
    curbuf->b_op_ਸ਼ੁਰੂ.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != ਨਿਰਾ);
    try_dos = (vim_strchr(p_ffs, 'd') != ਨਿਰਾ);
    try_unix = (vim_strchr(p_ffs, 'x') != ਨਿਰਾ);

    ਜੇ (!read_ਬਫਰ)
    {
	ਪੂਰਨ	m = ਸੁਨੇਹਾ_scroll;
	ਪੂਰਨ	n = ਸੁਨੇਹਾ_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	ਜੇ (!read_stdin)
	    close(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	ਸੁਨੇਹਾ_scroll = TRUE;
	ਜੇ (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, ਨਿਰਾ, sfname,
							  FALSE, curbuf, eap);
	ਨਹੀਂਤਾਂ ਜੇ (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, ਨਿਰਾ, sfname,
							  FALSE, curbuf, eap);
	ਨਹੀਂਤਾਂ ਜੇ (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, ਨਿਰਾ, sfname,
							  FALSE, curbuf, eap);
	ਨਹੀਂਤਾਂ
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, ਨਿਰਾ, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != ਨਿਰਾ);
	try_dos = (vim_strchr(p_ffs, 'd') != ਨਿਰਾ);
	try_unix = (vim_strchr(p_ffs, 'x') != ਨਿਰਾ);
	curbuf->b_op_ਸ਼ੁਰੂ = orig_ਸ਼ੁਰੂ;

	ਜੇ (ਸੁਨੇਹਾ_scrolled == n)
	    ਸੁਨੇਹਾ_scroll = m;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    ਜਾਓ theend;
	}
#ਜੇ_ਅੰਤ
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	ਜੇ (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
	    ਜੇ (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    ਨਹੀਂਤਾਂ
		emsg(_(e_readpre_autocommands_must_not_change_current_ਬਫਰ));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    ਜਾਓ theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ);

    ਜੇ (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	ਜੇ (read_stdin)
	{
	    ਜੇ (!is_not_a_ਪਦ())
	    {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ALWAYS_USE_GUI
# ifdef VIMDLL
		ਜੇ (!gui.ਵਿੱਚ_use)
# endif
		    mch_ਸੁਨੇਹਾ(_("Vim: Reading from stdin...\n"));
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		ਜੇ (gui.ਵਿੱਚ_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    ਜੇ (p != ਨਿਰਾ)
		    {
			gui_write(p, (ਪੂਰਨ)plen);
			vim_free(p);
		    }
		}
#ਜੇ_ਅੰਤ
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (!read_ਬਫਰ)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    ਸੁਨੇਹਾ_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    // "++bad=" argument.
    ਜੇ (eap != ਨਿਰਾ && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	ਜੇ (ਸੈੱਟ_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    ਨਹੀਂਤਾਂ
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    ਜੇ (eap != ਨਿਰਾ && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_help)
    {
	char_u	    firstline[80];
	ਪੂਰਨ	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	ਜੇ (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    ਜੇ (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, SEEK_SET);
		ਲਈ (p = firstline; p < firstline + len; ++p)
		    ਜੇ (*p >= 0x80)
		    {
			c = TRUE;
			ਰੋਕੋ;
		    }
	    }
	}

	ਜੇ (c)
	{
	    fenc_ਅਗਲਾ = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    ਜੇ (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	fenc_ਅਗਲਾ = p_fencs;		// try items in 'fileencodings'
	fenc = ਅਗਲਾ_fenc(&fenc_ਅਗਲਾ, &fenc_alloced);
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

    ਜੇ (file_rewind)
    {
	ਜੇ (read_ਬਫਰ)
	{
	    read_buf_lnum = 1;
	    read_buf_ਖਾਨਾ = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    ਜਾਓ failed;
	}
	// Delete the previously read lines.
	ਜਦੋਂ (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	ਜੇ (ਸੈੱਟ_options)
	{
	    curbuf->b_p_bomb = FALSE;
	    curbuf->b_ਸ਼ੁਰੂ_bomb = FALSE;
	}
	conv_error = 0;
    }

    /*
     * When retrying with another "fenc" and the first time "fileformat"
     * will be reset.
     */
    ਜੇ (keep_fileformat)
	keep_fileformat = FALSE;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (eap != ਨਿਰਾ && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	ਨਹੀਂਤਾਂ ਜੇ (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	ਨਹੀਂਤਾਂ
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
    ਜੇ (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	ਜੇ (eap != ਨਿਰਾ && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    ਜੇ (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (fenc_alloced)
		vim_free(fenc);
	    ਜੇ (fenc_ਅਗਲਾ != ਨਿਰਾ)
	    {
		fenc = ਅਗਲਾ_fenc(&fenc_ਅਗਲਾ, &fenc_alloced);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	ਜੇ (tmpname != ਨਿਰਾ)
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
    ਜੇ (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	ਜੇ (STRCMP(fenc, ENC_UCSBOM) == 0)
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
	ਨਹੀਂਤਾਂ ਜੇ (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	ਜੇ (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	ਜੇ (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	ਜੇ (fio_flags == 0
# ifdef FEAT_EVAL
		&& !did_iconv
# endif
		)
	    iconv_fd = (iconv_t)my_iconv_open(
				  enc_utf8 ? (char_u *)"utf-8" : p_enc, fenc);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	/*
	 * Use the 'charconvert' expression when conversion is required
	 * and we can't do it internally or with iconv().
	 */
	ਜੇ (fio_flags == 0 && !read_stdin && !read_ਬਫਰ && *p_ccv != NUL
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
	    ਜੇ (tmpname == ਨਿਰਾ)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		ਜੇ (tmpname == ਨਿਰਾ)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    ਜੇ (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			ਜਾਓ failed;
		    }
		    ਜਾਓ retry;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    ਜੇ (fio_flags == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
		    && iconv_fd == (iconv_t)-1
#ਜੇ_ਅੰਤ
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		ਜਾਓ retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    ਜੇ (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	filesize_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ
	skip_ਗਿਣਤੀ = ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_skip;
	read_ਗਿਣਤੀ = ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_read;
	conv_restlen = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != ਨਿਰਾ
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_ਬਫਰ);
	ਜੇ (read_undo_file)
	    sha256_ਸ਼ੁਰੂ(&sha_ctx);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	ਜੇ (curbuf->b_cryptstate != ਨਿਰਾ)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = ਨਿਰਾ;
	}
#ਜੇ_ਅੰਤ
    }

    ਜਦੋਂ (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	ਜੇ (!skip_read)
	{
#ਜੇ defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		ਮਾਪ = SSIZE_MAX;		    // use max I/O size, 52K
#ਨਹੀਂਤਾਂ
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		ਮਾਪ = 0x10000L + linerest;
		ਜੇ (ਮਾਪ > 0x100000L)
		    ਮਾਪ = 0x100000L;
#ਜੇ_ਅੰਤ
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	ਜੇ (ਮਾਪ < 0 || ਮਾਪ + linerest + 1 < 0 || linerest >= MAXCOL - ਮਾਪ)
	{
	    ++ਵੰਡੋ;
	    *ptr = NL;		    // split line by inserting a NL
	    ਮਾਪ = 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (!skip_read)
	    {
		ਲਈ ( ; ਮਾਪ >= 10; ਮਾਪ = (ਲੰਮਾ)((long_u)ਮਾਪ >> 1))
		{
		    ਜੇ ((new_ਬਫਰ = lalloc(ਮਾਪ + linerest + 1,
							      FALSE)) != ਨਿਰਾ)
			ਰੋਕੋ;
		}
		ਜੇ (new_ਬਫਰ == ਨਿਰਾ)
		{
		    do_outofmem_ਸੁਨੇਹਾ((long_u)(ਮਾਪ * 2 + linerest + 1));
		    error = TRUE;
		    ਰੋਕੋ;
		}
		ਜੇ (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_ਬਫਰ, ptr - linerest, (size_t)linerest);
		vim_free(ਬਫਰ);
		ਬਫਰ = new_ਬਫਰ;
		ptr = ਬਫਰ + linerest;
		ਸਤਰ_ਸ਼ੁਰੂ = ਬਫਰ;

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
		real_ਮਾਪ = (ਪੂਰਨ)ਮਾਪ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
		ਜੇ (iconv_fd != (iconv_t)-1)
		    ਮਾਪ = ਮਾਪ / ICONV_MULT;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    ਜੇ (fio_flags & FIO_LATIN1)
		    ਮਾਪ = ਮਾਪ / 2;
		ਨਹੀਂਤਾਂ ਜੇ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    ਮਾਪ = (ਮਾਪ * 2 / 3) & ~1;
		ਨਹੀਂਤਾਂ ਜੇ (fio_flags & FIO_UCS4)
		    ਮਾਪ = (ਮਾਪ * 2 / 3) & ~3;
		ਨਹੀਂਤਾਂ ਜੇ (fio_flags == FIO_UCSBOM)
		    ਮਾਪ = ਮਾਪ / ICONV_MULT;	// worst case
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
		ਨਹੀਂਤਾਂ ਜੇ (fio_flags & FIO_CODEPAGE)
		    ਮਾਪ = ਮਾਪ / ICONV_MULT;	// also worst case
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_CONVERT
		ਨਹੀਂਤਾਂ ਜੇ (fio_flags & FIO_MACROMAN)
		    ਮਾਪ = ਮਾਪ / ICONV_MULT;	// also worst case
#ਜੇ_ਅੰਤ

		ਜੇ (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    ਮਾਪ -= conv_restlen;
		}

		ਜੇ (read_ਬਫਰ)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    ਜੇ (read_buf_lnum > from)
			ਮਾਪ = 0;
		    ਨਹੀਂਤਾਂ
		    {
			ਪੂਰਨ	n, ni;
			ਲੰਮਾ	tlen;

			tlen = 0;
			ਲਈ (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_ਖਾਨਾ;
			    n = ml_get_len(read_buf_lnum) - read_buf_ਖਾਨਾ;
			    ਜੇ ((ਪੂਰਨ)tlen + n + 1 > ਮਾਪ)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (ਪੂਰਨ)(ਮਾਪ - tlen);
				ਲਈ (ni = 0; ni < n; ++ni)
				{
				    ਜੇ (p[ni] == NL)
					ptr[tlen++] = NUL;
				    ਨਹੀਂਤਾਂ
					ptr[tlen++] = p[ni];
				}
				read_buf_ਖਾਨਾ += n;
				ਰੋਕੋ;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    ਲਈ (ni = 0; ni < n; ++ni)
			    {
				ਜੇ (p[ni] == NL)
				    ptr[tlen++] = NUL;
				ਨਹੀਂਤਾਂ
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_ਖਾਨਾ = 0;
			    ਜੇ (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				ਜੇ (!curbuf->b_p_eol)
				    --tlen;
				ਮਾਪ = tlen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
				eof = TRUE;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
			    }
			}
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    /*
		     * Read bytes from the file.
		     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
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
		    ਜੇ (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			ਮਾਪ = WRITEBUFSIZE + crypt_get_max_ਸਿਰਲੇਖ_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    ਨਹੀਂਤਾਂ ਜੇ (filesize > 0 && (curbuf->b_cryptstate != ਨਿਰਾ
				&& crypt_ਵਿਧੀ_is_sodium(
					     curbuf->b_cryptstate->ਵਿਧੀ_nr)))
		    {
			ਮਾਪ = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			ਜੇ (curbuf->b_cryptstate->ਵਿਧੀ_nr == CRYPT_M_SOD
						     && !eof && may_need_lseek)
			{
			    lseek(fd, crypt_get_ਸਿਰਲੇਖ_len(
					       curbuf->b_cryptstate->ਵਿਧੀ_nr)
				       - crypt_get_max_ਸਿਰਲੇਖ_len(), SEEK_CUR);
			    may_need_lseek = FALSE;
			}
		    }
#ਜੇ_ਅੰਤ
		    ਲੰਮਾ read_ਮਾਪ = ਮਾਪ;
		    ਮਾਪ = read_eintr(fd, ptr, read_ਮਾਪ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
		    // Did we reach end of file?
		    filesize_ਗਿਣਤੀ += ਮਾਪ;
		    eof = (ਮਾਪ < read_ਮਾਪ || filesize_ਗਿਣਤੀ == filesize_disk);
#ਜੇ_ਅੰਤ
		}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		ਜੇ (filesize == 0 && ਮਾਪ > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &ਮਾਪ,
						  &filesize, newfile, sfname,
						  &did_ask_for_ਕੁੰਜੀ);
# ਜੇ defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    ਜੇ (curbuf->b_cryptstate != ਨਿਰਾ
				 && !crypt_ਚੱਲਦਾ_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		ਜੇ (cryptkey != ਨਿਰਾ && curbuf->b_cryptstate != ਨਿਰਾ
								   && ਮਾਪ > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    ਜੇ (crypt_ਚੱਲਦਾ_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    ਮਾਪ, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    ਨਹੀਂਤਾਂ
		    {
			char_u	*newptr = ਨਿਰਾ;
			ਪੂਰਨ	decrypted_ਮਾਪ;

			decrypted_ਮਾਪ = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, ਮਾਪ,
								 &newptr, eof);

			ਜੇ (decrypted_ਮਾਪ < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    ਰੋਕੋ;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			ਜੇ (decrypted_ਮਾਪ == 0)
			    ਜਾਰੀ;

			ਜੇ (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_ਬਫਰ = newptr;
			}
			ਨਹੀਂਤਾਂ
			{
			    long_u	new_ਮਾਪ;

			    // Need new buffer to add bytes carried over.
			    new_ਮਾਪ = (long_u)(decrypted_ਮਾਪ + linerest + 1);
			    new_ਬਫਰ = lalloc(new_ਮਾਪ, FALSE);
			    ਜੇ (new_ਬਫਰ == ਨਿਰਾ)
			    {
				do_outofmem_ਸੁਨੇਹਾ(new_ਮਾਪ);
				error = TRUE;
				ਰੋਕੋ;
			    }

			    mch_memmove(new_ਬਫਰ, ਬਫਰ, linerest + conv_restlen);
			    ਜੇ (newptr != ਨਿਰਾ)
				mch_memmove(new_ਬਫਰ + linerest + conv_restlen,
					newptr, decrypted_ਮਾਪ);
			    vim_free(newptr);
			}

			ਜੇ (new_ਬਫਰ != ਨਿਰਾ)
			{
			    vim_free(ਬਫਰ);
			    ਬਫਰ = new_ਬਫਰ;
			    new_ਬਫਰ = ਨਿਰਾ;
			    ਸਤਰ_ਸ਼ੁਰੂ = ਬਫਰ;
			    ptr = ਬਫਰ + linerest + conv_restlen;
			    real_ਮਾਪ = ਮਾਪ;
			}
			ਮਾਪ = decrypted_ਮਾਪ;
		    }
# endif
		}
#ਜੇ_ਅੰਤ

		ਜੇ (ਮਾਪ <= 0)
		{
		    ਜੇ (ਮਾਪ < 0)		    // read error
			error = TRUE;
		    ਨਹੀਂਤਾਂ ਜੇ (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			ਜੇ (fio_flags != 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
				|| iconv_fd != (iconv_t)-1
#ਜੇ_ਅੰਤ
			   )
			{
			    ਜੇ (can_retry)
				ਜਾਓ rewind_retry;
			    ਜੇ (conv_error == 0)
				conv_error = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			ਨਹੀਂਤਾਂ ਜੇ (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
								- linecnt + 1;
			ਜੇ (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			ਨਹੀਂਤਾਂ
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    ਜੇ (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
				    || iconv_fd != (iconv_t)-1
#ਜੇ_ਅੰਤ
			       ))
			    {
				ਜਦੋਂ (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
			    ਜੇ (iconv_fd != (iconv_t)-1)
			    {
				iconv_close(iconv_fd);
				iconv_fd = (iconv_t)-1;
			    }
#ਜੇ_ਅੰਤ
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
	    ਜੇ ((filesize == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
		   || (cryptkey != ਨਿਰਾ
			&& filesize == crypt_get_ਸਿਰਲੇਖ_len(
						 crypt_get_ਵਿਧੀ_nr(curbuf)))
#ਜੇ_ਅੰਤ
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == ਨਿਰਾ
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		ਪੂਰਨ	blen;

		// no BOM detection in a short file or in binary mode
		ਜੇ (ਮਾਪ < 2 || curbuf->b_p_bin)
		    ccname = ਨਿਰਾ;
		ਨਹੀਂਤਾਂ
		    ccname = check_for_bom(ptr, ਮਾਪ, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		ਜੇ (ccname != ਨਿਰਾ)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    ਮਾਪ -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)ਮਾਪ);
		    ਜੇ (ਸੈੱਟ_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_ਸ਼ੁਰੂ_bomb = TRUE;
		    }
		}

		ਜੇ (fio_flags == FIO_UCSBOM)
		{
		    ਜੇ (ccname == ਨਿਰਾ)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// BOM detected: set "fenc" and jump back
			ਜੇ (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    ਜਾਓ retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    ਮਾਪ += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    ਜੇ (ਮਾਪ <= 0)
		ਰੋਕੋ;


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
	    ਜੇ (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		ਅਚਲ ਅੱਖਰ	*fromp;
		ਅੱਖਰ		*ਸਿਖਰ;
		size_t		from_ਮਾਪ;
		size_t		ਮੰਜ਼ਿਲ_ਮਾਪ;

		fromp = (ਅੱਖਰ *)ptr;
		from_ਮਾਪ = ਮਾਪ;
		ptr += ਮਾਪ;
		ਸਿਖਰ = (ਅੱਖਰ *)ptr;
		ਮੰਜ਼ਿਲ_ਮਾਪ = real_ਮਾਪ - ਮਾਪ;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		ਜਦੋਂ ((iconv(iconv_fd, (ਖਾਲੀ *)&fromp, &from_ਮਾਪ,
							       &ਸਿਖਰ, &ਮੰਜ਼ਿਲ_ਮਾਪ)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_ਮਾਪ > CONV_RESTLEN)
		{
		    ਜੇ (can_retry)
			ਜਾਓ rewind_retry;
		    ਜੇ (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)ਸਿਖਰ);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_ਮਾਪ;
		    ਜੇ (bad_char_behavior == BAD_KEEP)
		    {
			*ਸਿਖਰ++ = *(fromp - 1);
			--ਮੰਜ਼ਿਲ_ਮਾਪ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (bad_char_behavior != BAD_DROP)
		    {
			*ਸਿਖਰ++ = bad_char_behavior;
			--ਮੰਜ਼ਿਲ_ਮਾਪ;
		    }
		}

		ਜੇ (from_ਮਾਪ > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_ਮਾਪ);
		    conv_restlen = (ਪੂਰਨ)from_ਮਾਪ;
		}

		// move the linerest to before the converted characters
		ਸਤਰ_ਸ਼ੁਰੂ = ptr - linerest;
		mch_memmove(ਸਤਰ_ਸ਼ੁਰੂ, ਬਫਰ, (size_t)linerest);
		ਮਾਪ = (ਲੰਮਾ)((char_u *)ਸਿਖਰ - ptr);
	    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    ਜੇ (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		ਪੂਰਨ	ucs2len;
		ਪੂਰਨ	codepage = FIO_GET_CP(fio_flags);
		ਪੂਰਨ	bytelen;
		ਪੂਰਨ	ਮਿਲਿਆ_bad;
		ਅੱਖਰ	replstr[2];

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
		ਜੇ (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		ਨਹੀਂਤਾਂ
		    replstr[0] = '?';
		replstr[1] = NUL;

		/*
		 * Move the bytes to the end of the buffer, so that we have
		 * room to put the result at the start.
		 */
		src = ptr + real_ਮਾਪ - ਮਾਪ;
		mch_memmove(src, ptr, ਮਾਪ);

		/*
		 * Do the conversion.
		 */
		dst = ptr;
		ਜਦੋਂ (ਮਾਪ > 0)
		{
		    ਮਿਲਿਆ_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    ਜੇ (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (ਪੂਰਨ)utf_ptr2len_len(src, ਮਾਪ);
			ਜੇ (bytelen > ਮਾਪ)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    ਜੇ (bytelen <= CONV_RESTLEN)
				ਰੋਕੋ;

			    // weird overlong byte sequence
			    bytelen = ਮਾਪ;
			    ਮਿਲਿਆ_bad = TRUE;
			}
			ਨਹੀਂਤਾਂ
			{
			    ਪੂਰਨ	    u8c = utf_ptr2char(src);

			    ਜੇ (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				ਮਿਲਿਆ_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    ਨਹੀਂਤਾਂ
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			ਲਈ (bytelen = 1; bytelen <= ਮਾਪ && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    ਜੇ (ucs2len > 0)
				ਰੋਕੋ;
			}
			ਜੇ (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    ਜੇ (ਮਾਪ == 1)
				ਰੋਕੋ;
			    ਮਿਲਿਆ_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    ਜੇ (!ਮਿਲਿਆ_bad)
		    {
			ਪੂਰਨ	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			ਜੇ (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    ਲਈ (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			ਨਹੀਂਤਾਂ
			{
			    BOOL	bad = FALSE;
			    ਪੂਰਨ		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (ਪੂਰਨ)(src - dst),
				    replstr, &bad);
			    ਜੇ (bad)
				ਮਿਲਿਆ_bad = TRUE;
			    ਨਹੀਂਤਾਂ
				dst += dstlen;
			}
		    }

		    ਜੇ (ਮਿਲਿਆ_bad)
		    {
			// Deal with bytes we can't convert.
			ਜੇ (can_retry)
			    ਜਾਓ rewind_retry;
			ਜੇ (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			ਜੇ (bad_char_behavior != BAD_DROP)
			{
			    ਜੇ (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    ਨਹੀਂਤਾਂ
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    ਮਾਪ -= bytelen;
		}

		ਜੇ (ਮਾਪ > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, ਮਾਪ);
		    conv_restlen = ਮਾਪ;
		}

		// The new size is equal to how much "dst" was advanced.
		ਮਾਪ = (ਲੰਮਾ)(dst - ptr);
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_CONVERT
	    ਜੇ (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		ਜੇ (macroman2enc(ptr, &ਮਾਪ, real_ਮਾਪ) == FAIL)
		    ਜਾਓ rewind_retry;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜੇ (fio_flags != 0)
	    {
		ਪੂਰਨ	u8c;
		char_u	*dest;
		char_u	*ਪੂਛ = ਨਿਰਾ;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_ਮਾਪ;
		ਜੇ (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + ਮਾਪ;
		    ਜੇ (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			ਪੂਛ = ptr + ਮਾਪ - 1;
			ਜਦੋਂ (ਪੂਛ > ptr && (*ਪੂਛ & 0xc0) == 0x80)
			    --ਪੂਛ;
			ਜੇ (ਪੂਛ + utf_byte2len(*ਪੂਛ) <= ptr + ਮਾਪ)
			    ਪੂਛ = ਨਿਰਾ;
			ਨਹੀਂਤਾਂ
			    p = ਪੂਛ;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (ਮਾਪ & ~1);
		    ਜੇ (ਮਾਪ & 1)
			ਪੂਛ = p;
		    ਜੇ ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			ਜੇ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			ਨਹੀਂਤਾਂ
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			ਜੇ (u8c >= 0xd800 && u8c <= 0xdbff)
			    ਪੂਛ = p;
			ਨਹੀਂਤਾਂ
			    p += 2;
		    }
		}
		ਨਹੀਂਤਾਂ //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (ਮਾਪ & ~3);
		    ਜੇ (ਮਾਪ & 3)
			ਪੂਛ = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		ਜੇ (ਪੂਛ != ਨਿਰਾ)
		{
		    conv_restlen = (ਪੂਰਨ)((ptr + ਮਾਪ) - ਪੂਛ);
		    mch_memmove(conv_rest, (char_u *)ਪੂਛ, conv_restlen);
		    ਮਾਪ -= conv_restlen;
		}


		ਜਦੋਂ (p > ptr)
		{
		    ਜੇ (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    ਨਹੀਂਤਾਂ ਜੇ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			ਜੇ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			ਨਹੀਂਤਾਂ
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			ਜੇ ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    ਪੂਰਨ u16c;

			    ਜੇ (p == ptr)
			    {
				// Missing leading word.
				ਜੇ (can_retry)
				    ਜਾਓ rewind_retry;
				ਜੇ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				ਜੇ (bad_char_behavior == BAD_DROP)
				    ਜਾਰੀ;
				ਜੇ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    ਜੇ (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    ਨਹੀਂਤਾਂ
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    ਜੇ (u16c < 0xd800 || u16c > 0xdbff)
			    {
				ਜੇ (can_retry)
				    ਜਾਓ rewind_retry;
				ਜੇ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				ਜੇ (bad_char_behavior == BAD_DROP)
				    ਜਾਰੀ;
				ਜੇ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (fio_flags & FIO_UCS4)
		    {
			ਜੇ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (ਬਿਨਾਨਿਸ਼ਾਨੀ)*--p << 24;
			    u8c += (ਬਿਨਾਨਿਸ਼ਾਨੀ)*--p << 16;
			    u8c += (ਬਿਨਾਨਿਸ਼ਾਨੀ)*--p << 8;
			    u8c += *--p;
			}
			ਨਹੀਂਤਾਂ	// big endian
			{
			    u8c = *--p;
			    u8c += (ਬਿਨਾਨਿਸ਼ਾਨੀ)*--p << 8;
			    u8c += (ਬਿਨਾਨਿਸ਼ਾਨੀ)*--p << 16;
			    u8c += (ਬਿਨਾਨਿਸ਼ਾਨੀ)*--p << 24;
			}
		    }
		    ਨਹੀਂਤਾਂ    // UTF-8
		    {
			ਜੇ (*--p < 0x80)
			    u8c = *p;
			ਨਹੀਂਤਾਂ
			{
			    len = utf_ਸਿਰ_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    ਜੇ (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				ਜੇ (can_retry)
				    ਜਾਓ rewind_retry;
				ਜੇ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				ਜੇ (bad_char_behavior == BAD_DROP)
				    ਜਾਰੀ;
				ਜੇ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    ਜੇ (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(ਖਾਲੀ)utf_char2bytes(u8c, dest);
		    }
		    ਨਹੀਂਤਾਂ		// produce Latin1
		    {
			--dest;
			ਜੇ (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    ਜੇ (can_retry)
				ਜਾਓ rewind_retry;
			    ਜੇ (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    ਜੇ (bad_char_behavior == BAD_DROP)
				++dest;
			    ਨਹੀਂਤਾਂ ਜੇ (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    ਨਹੀਂਤਾਂ ਜੇ (eap != ਨਿਰਾ && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    ਨਹੀਂਤਾਂ
				*dest = 0xBF;
			}
			ਨਹੀਂਤਾਂ
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		ਸਤਰ_ਸ਼ੁਰੂ = dest - linerest;
		mch_memmove(ਸਤਰ_ਸ਼ੁਰੂ, ਬਫਰ, (size_t)linerest);
		ਮਾਪ = (ਲੰਮਾ)((ptr + real_ਮਾਪ) - dest);
		ptr = dest;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8 && !curbuf->b_p_bin)
	    {
		ਪੂਰਨ  incomplete_ਪੂਛ = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		ਲਈ (p = ptr; ; )
		{
		    ਪੂਰਨ	 todo;
		    ਪੂਰਨ	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_ਸਿਰਾ = ptr + ਮਾਪ;
			ਜਦੋਂ (ascii_ਸਿਰਾ - p >= (ਲੰਮਾ)ਆਕਾਰ(long_u))
			{
			    long_u ਸ਼ਬਦ;
			    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ਸ਼ਬਦ, p, ਆਕਾਰ(long_u));
			    ਜੇ (ਸ਼ਬਦ & NONASCII_ਨਕਾਬ)
				ਰੋਕੋ;
			    p += ਆਕਾਰ(long_u);
			}
			ਜਦੋਂ (p < ascii_ਸਿਰਾ && *p < 0x80)
			    ++p;
		    }

		    todo = (ਪੂਰਨ)((ptr + ਮਾਪ) - p);
		    ਜੇ (todo <= 0)
			ਰੋਕੋ;
		    ਜੇ (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			ਜੇ (l > todo && !incomplete_ਪੂਛ)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    ਜੇ (p > ptr || filesize > 0)
				incomplete_ਪੂਛ = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    ਜੇ (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				ਮਾਪ -= conv_restlen;
				ਰੋਕੋ;
			    }
			}
			ਜੇ (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    ਜੇ (can_retry && !incomplete_ਪੂਛ)
				ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
			    // When we did a conversion report an error.
			    ਜੇ (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#ਜੇ_ਅੰਤ
			    // Remember the first linenr with an illegal byte
			    ਜੇ (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    ਜੇ (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--ਮਾਪ;
			    }
			    ਨਹੀਂਤਾਂ
			    {
				ਜੇ (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			ਨਹੀਂਤਾਂ
			    p += l;
		    }
		}
		ਜੇ (p < ptr + ਮਾਪ && !incomplete_ਪੂਛ)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#ਜੇ defined(FEAT_EVAL) && defined(USE_ICONV)
		    ਜੇ (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    ਜਾਓ retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += ਮਾਪ;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    ਜੇ (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		ਜੇ (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    ਜੇ (try_mac)
			try_mac = 1;

		    ਲਈ (p = ptr; p < ptr + ਮਾਪ; ++p)
		    {
			ਜੇ (*p == NL)
			{
			    ਜੇ (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    ਨਹੀਂਤਾਂ
				fileformat = EOL_UNIX;
			    ਰੋਕੋ;
			}
			ਨਹੀਂਤਾਂ ਜੇ (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    ਜੇ (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			ਲਈ (; p >= ptr && *p != CAR; p--)
			    ;
			ਜੇ (p >= ptr)
			{
			    ਲਈ (p = ptr; p < ptr + ਮਾਪ; ++p)
			    {
				ਜੇ (*p == NL)
				    try_unix++;
				ਨਹੀਂਤਾਂ ਜੇ (*p == CAR)
				    try_mac++;
			    }
			    ਜੇ (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		ਜੇ (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		ਜੇ (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		ਜੇ (ਸੈੱਟ_options)
		    ਸੈੱਟ_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	ਜੇ (fileformat == EOL_MAC)
	{
	    --ptr;
	    ਜਦੋਂ (++ptr, --ਮਾਪ >= 0)
	    {
		// catch most common case first
		ਜੇ ((c = *ptr) != NUL && c != CAR && c != NL)
		    ਜਾਰੀ;
		ਜੇ (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		ਨਹੀਂਤਾਂ ਜੇ (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (skip_ਗਿਣਤੀ == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - ਸਤਰ_ਸ਼ੁਰੂ + 1);
			ਜੇ (ml_append(lnum, ਸਤਰ_ਸ਼ੁਰੂ, len, newfile) == FAIL)
			{
			    error = TRUE;
			    ਰੋਕੋ;
			}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
			ਜੇ (read_undo_file)
			    sha256_update(&sha_ctx, ਸਤਰ_ਸ਼ੁਰੂ, len);
#ਜੇ_ਅੰਤ
			++lnum;
			ਜੇ (--read_ਗਿਣਤੀ == 0)
			{
			    error = TRUE;	// break loop
			    ਸਤਰ_ਸ਼ੁਰੂ = ptr;	// nothing left to write
			    ਰੋਕੋ;
			}
		    }
		    ਨਹੀਂਤਾਂ
			--skip_ਗਿਣਤੀ;
		    ਸਤਰ_ਸ਼ੁਰੂ = ptr + 1;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *ਸਿਰਾ = ptr + ਮਾਪ;

	    ਜਦੋਂ (ptr < ਸਿਰਾ)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, ਸਿਰਾ - ptr);
		char_u *nul_ਜਾਂਚ;

		ਜੇ (nl == ਨਿਰਾ)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    ਜਦੋਂ ((nul_ਜਾਂਚ = (char_u *)memchr(ptr, NUL,
						      ਸਿਰਾ - ptr)) != ਨਿਰਾ)
		    {
			*nul_ਜਾਂਚ = NL;
			ptr = nul_ਜਾਂਚ + 1;
		    }
		    ptr = ਸਿਰਾ;
		    ਰੋਕੋ;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *ਜਾਂਚ = ptr;
		    ਜਦੋਂ ((nul_ਜਾਂਚ = (char_u *)memchr(ਜਾਂਚ, NUL,
						       nl - ਜਾਂਚ)) != ਨਿਰਾ)
		    {
			*nul_ਜਾਂਚ = NL;
			ਜਾਂਚ = nul_ਜਾਂਚ + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		ਜੇ (skip_ਗਿਣਤੀ == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - ਸਤਰ_ਸ਼ੁਰੂ + 1);
		    ਜੇ (fileformat == EOL_DOS)
		    {
			ਜੇ (ptr > ਸਤਰ_ਸ਼ੁਰੂ && ptr[-1] == CAR)
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
			ਨਹੀਂਤਾਂ ਜੇ (ff_error != EOL_DOS)
			{
			    ਜੇ (   try_unix
				&& !read_stdin
				&& (read_ਬਫਰ
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				ਜੇ (ਸੈੱਟ_options)
				    ਸੈੱਟ_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				ਜਾਓ retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    ਜੇ (ml_append(lnum, ਸਤਰ_ਸ਼ੁਰੂ, len, newfile) == FAIL)
		    {
			error = TRUE;
			ਰੋਕੋ;
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
		    ਜੇ (read_undo_file)
			sha256_update(&sha_ctx, ਸਤਰ_ਸ਼ੁਰੂ, len);
#ਜੇ_ਅੰਤ
		    ++lnum;
		    ਜੇ (--read_ਗਿਣਤੀ == 0)
		    {
			error = TRUE;	    // break loop
			ਸਤਰ_ਸ਼ੁਰੂ = ptr;   // nothing left to write
			ਰੋਕੋ;
		    }
		}
		ਨਹੀਂਤਾਂ
		    --skip_ਗਿਣਤੀ;
		ਸਤਰ_ਸ਼ੁਰੂ = ptr + 1;
		++ptr;
	    }
	    ਮਾਪ = -1;
	}
	linerest = (ਲੰਮਾ)(ptr - ਸਤਰ_ਸ਼ੁਰੂ);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    ਜੇ (error && read_ਗਿਣਤੀ == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    ਜੇ (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	ਜੇ (ਸੈੱਟ_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    ਜੇ (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	ਜੇ (ਸੈੱਟ_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - ਸਤਰ_ਸ਼ੁਰੂ + 1);
	ਜੇ (ml_append(lnum, ਸਤਰ_ਸ਼ੁਰੂ, len, newfile) == FAIL)
	    error = TRUE;
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
	    ਜੇ (read_undo_file)
		sha256_update(&sha_ctx, ਸਤਰ_ਸ਼ੁਰੂ, len);
#ਜੇ_ਅੰਤ
	    read_no_eol_lnum = ++lnum;
	}
    }

    ਜੇ (ਸੈੱਟ_options)
	save_file_ff(curbuf);		// remember the current file format

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਜੇ (curbuf->b_cryptstate != ਨਿਰਾ)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = ਨਿਰਾ;
    }
    ਜੇ (cryptkey != ਨਿਰਾ && cryptkey != curbuf->b_p_ਕੁੰਜੀ)
	crypt_free_ਕੁੰਜੀ(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#ਜੇ_ਅੰਤ

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    ਜੇ (ਸੈੱਟ_options)
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    ਜੇ (fenc_alloced)
	vim_free(fenc);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
    ਜੇ (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#ਜੇ_ਅੰਤ

    ਜੇ (!read_ਬਫਰ && !read_stdin)
	close(fd);				// errors are ignored
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FD_CLOEXEC
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ fdflags = fcntl(fd, F_GETFD);

	ਜੇ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (ਖਾਲੀ)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#ਜੇ_ਅੰਤ
    vim_free(ਬਫਰ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_DUP
    ਜੇ (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (tmpname != ਨਿਰਾ)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    ਜੇ (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	ਜੇ (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#ਜੇ_ਅੰਤ
	    ml_delete(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#ਜੇ_ਅੰਤ
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - linecnt;
	ਜੇ (filesize == 0)
	    linecnt = 0;
	ਜੇ (newfile || read_ਬਫਰ)
	{
	    redraw_curbuf_later(UPD_NOT_VALID);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    // After reading the text into the buffer the diff info needs to
	    // be updated.
	    diff_invalidate(curbuf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // All folds in the window are invalid now.  Mark them for update
	    // before triggering autocommands.
	    foldUpdateAll(curwin);
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (linecnt)		// appended at least one line
	    appended_ਸਤਰਾਂ_mark(from, linecnt);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	ਜੇ (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#ਜੇ_ਅੰਤ

	ਜੇ (got_int)
	{
	    ਜੇ (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		ਜੇ (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
	    check_marks_read();
#ਜੇ_ਅੰਤ
	    retval = OK;	// an interrupt isn't really an error
	    ਜਾਓ theend;
	}

	ਜੇ (!filtering && !(flags & READ_DUMMY))
	{
	    ਪੂਰਨ	buflen;

	    ਸੁਨੇਹਾ_ਜੋੜੋ_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (ਪੂਰਨ)STRLEN(IObuff);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	    ਜੇ (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    ਜੇ (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    ਜੇ (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#ਜੇ_ਅੰਤ
	    ਜੇ (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    ਜੇ (read_no_eol_lnum)
	    {
		ਸੁਨੇਹਾ_ਜੋੜੋ_eol();
		c = TRUE;
	    }
	    ਜੇ (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    ਜੇ (ਵੰਡੋ)
	    {
		buflen += vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    ਜੇ (notconverted)
	    {
		buflen += vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (converted)
	    {
		buflen += vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	    ਜੇ (cryptkey != ਨਿਰਾ)
	    {
		crypt_append_ਸੁਨੇਹਾ(curbuf);
		c = TRUE;
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (conv_error != 0)
	    {
		vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (ਲੰਮਾ)conv_error);
		c = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (illegal_byte > 0)
	    {
		vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (ਲੰਮਾ)illegal_byte);
		c = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (error)
	    {
		vim_snprintf((ਅੱਖਰ *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    ਜੇ (ਸੁਨੇਹਾ_ਜੋੜੋ_fileformat(fileformat))
		c = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	    ਜੇ (cryptkey != ਨਿਰਾ)
		ਸੁਨੇਹਾ_ਜੋੜੋ_ਸਤਰਾਂ(c, (ਲੰਮਾ)linecnt, filesize
			 - crypt_get_ਸਿਰਲੇਖ_len(crypt_get_ਵਿਧੀ_nr(curbuf)));
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਸੁਨੇਹਾ_ਜੋੜੋ_ਸਤਰਾਂ(c, (ਲੰਮਾ)linecnt, filesize);

	    VIM_CLEAR(keep_ਸੁਨੇਹਾ);
	    ਸੁਨੇਹਾ_scrolled_ign = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    ਜੇ (read_stdin || read_ਬਫਰ)
		p = ਸੁਨੇਹਾ_may_trunc(FALSE, IObuff);
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ > 0)
		    ਸੁਨੇਹਾ_putchar('\r');  // overwrite previous message
		p = (char_u *)ਸੁਨੇਹਾ_trunc_attr((ਅੱਖਰ *)IObuff, FALSE, 0);
	    }
	    ਜੇ (read_stdin || read_ਬਫਰ || restart_edit != 0
		    || (ਸੁਨੇਹਾ_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		ਸੈੱਟ_keep_ਸੁਨੇਹਾ(p, 0);
	    ਸੁਨੇਹਾ_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	ਜੇ (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	ਜੇ (exmode_ਸਰਗਰਮ)
	    curwin->w_cursor.lnum = from + linecnt;
	ਨਹੀਂਤਾਂ
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set '[ and '] marks to the newly read lines.
	    curbuf->b_op_ਸ਼ੁਰੂ.lnum = from + 1;
	    curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
	    curbuf->b_op_ਸਿਰਾ.lnum = from + linecnt;
	    curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	/*
	 * Work around a weird problem: When a file has two links (only
	 * possible on NTFS) and we write through one link, then stat() it
	 * through the other link, the timestamp information may be wrong.
	 * It's correct again after reading the file, thus reset the timestamp
	 * here.
	 */
	ਜੇ (newfile && !read_stdin && !read_ਬਫਰ
					 && mch_stat((ਅੱਖਰ *)fname, &st) >= 0)
	{
	    buf_store_ਸਮਾਂ(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#ਜੇ_ਅੰਤ
    }
    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    /*
     * Get the marks before executing autocommands, so they can be used there.
     */
    check_marks_read();
#ਜੇ_ਅੰਤ

    /*
     * We remember if the last line of the read didn't have
     * an eol even when 'binary' is off, to support turning 'fixeol' off,
     * or writing the read again with 'binary' on.  The latter is required
     * for ":autocmd FileReadPost *.gz set bin|'[,']!gunzip" to work.
     */
    curbuf->b_no_eol_lnum = read_no_eol_lnum;

    // When reloading a buffer put the cursor at the first line that is
    // different.
    ਜੇ (flags & READ_KEEP_UNDO)
	u_ਲੱਭੋ_ਪਹਿਲਾ_ਬਦਲਿਆ();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    ਜੇ (read_undo_file)
    {
	char_u	hash[UNDO_HASH_ਮਾਪ];

	sha256_ਅੰਤ(&sha_ctx, hash);
	u_read_undo(ਨਿਰਾ, hash, fname);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (!read_stdin && !read_fifo && (!read_ਬਫਰ || sfname != ਨਿਰਾ))
    {
	ਪੂਰਨ m = ਸੁਨੇਹਾ_scroll;
	ਪੂਰਨ n = ਸੁਨੇਹਾ_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	ਜੇ (ਸੈੱਟ_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	ਸੁਨੇਹਾ_scroll = TRUE;
	ਜੇ (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, ਨਿਰਾ, sfname,
							  FALSE, curbuf, eap);
	ਨਹੀਂਤਾਂ ਜੇ (newfile || (read_ਬਫਰ && sfname != ਨਿਰਾ))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, ਨਿਰਾ, sfname,
							  FALSE, curbuf, eap);
	    ਜੇ (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	ਨਹੀਂਤਾਂ
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, ਨਿਰਾ, eap);
	ਜੇ (ਸੁਨੇਹਾ_scrolled == n)
	    ਸੁਨੇਹਾ_scroll = m;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (aborting())	    // autocmds may abort script processing
	    ਜਾਓ theend;
#ਜੇ_ਅੰਤ
    }

    ਜੇ (!(recoverymode && error))
	retval = OK;

theend:
    ਜੇ (curbuf->b_ml.ml_mfp != ਨਿਰਾ
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    ਵਾਪਸ retval;
}

#ਜੇ defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    ਪੂਰਨ
is_dev_fd_file(char_u *fname)
{
    ਵਾਪਸ (STRNCMP(fname, "/dev/fd/", 8) == 0
	    && VIM_ISDIGIT(fname[8])
	    && *skipdigits(fname + 9) == NUL
	    && (fname[9] != NUL
		|| (fname[8] != '0' && fname[8] != '1' && fname[8] != '2')));
}
#ਜੇ_ਅੰਤ

/*
 * From the current line count and characters read after that, estimate the
 * line number where we are now.
 * Used for error messages that include a line number.
 */
    ਸਥਿਰ linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - linecnt + 1;
    ਲਈ (s = p; s < endp; ++s)
	ਜੇ (*s == '\n')
	    ++lnum;
    ਵਾਪਸ lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(buf->b_p_fenc));
    ਜੇ (eap->cmd == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    sprintf((ਅੱਖਰ *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    ਵਾਪਸ OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    ਖਾਲੀ
ਸੈੱਟ_file_options(ਪੂਰਨ ਸੈੱਟ_options, exarg_T *eap)
{
    // set default 'fileformat'
    ਜੇ (ਸੈੱਟ_options)
    {
	ਜੇ (eap != ਨਿਰਾ && eap->force_ff != 0)
	    ਸੈੱਟ_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	ਨਹੀਂਤਾਂ ਜੇ (*p_ffs != NUL)
	    ਸੈੱਟ_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    ਜੇ (eap != ਨਿਰਾ && eap->force_bin != 0)
    {
	ਪੂਰਨ	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	ਸੈੱਟ_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    ਖਾਲੀ
ਸੈੱਟ_forced_fenc(exarg_T *eap)
{
    ਜੇ (eap->force_enc == 0)
	ਵਾਪਸ;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    ਜੇ (fenc != ਨਿਰਾ)
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"fenc", -1,
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
    ਸਥਿਰ char_u *
ਅਗਲਾ_fenc(char_u **pp, ਪੂਰਨ *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    ਜੇ (**pp == NUL)
    {
	*pp = ਨਿਰਾ;
	ਵਾਪਸ (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    ਜੇ (p == ਨਿਰਾ)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    ਨਹੀਂਤਾਂ
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	ਜੇ (r != ਨਿਰਾ)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    ਜੇ (r != ਨਿਰਾ)
	*alloced = TRUE;
    ਨਹੀਂਤਾਂ
    {
	// out of memory
	r = (char_u *)"";
	*pp = ਨਿਰਾ;
    }
    ਵਾਪਸ r;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Convert a file with the 'charconvert' expression.
 * This closes the file which is to be read, converts it and opens the
 * resulting file for reading.
 * Returns name of the resulting converted file (the caller should delete it
 * after reading it).
 * Returns NULL if the conversion failed ("*fdp" is not set) .
 */
    ਸਥਿਰ char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    ਪੂਰਨ		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;

    tmpname = vim_tempname('r', FALSE);
    ਜੇ (tmpname == ਨਿਰਾ)
	errmsg = _("Can't find temp file for conversion");
    ਨਹੀਂਤਾਂ
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	ਜੇ (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	ਜੇ (errmsg == ਨਿਰਾ && (*fdp = mch_open((ਅੱਖਰ *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    ਜੇ (errmsg != ਨਿਰਾ)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	ਸੁਨੇਹਾ(errmsg);
	ਜੇ (tmpname != ਨਿਰਾ)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    ਜੇ (*fdp < 0)
	*fdp = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0);

    ਵਾਪਸ tmpname;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    ਸਥਿਰ char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    ਲੰਮਾ	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    ਪੂਰਨ		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    ਪੂਰਨ		*did_ask)	// flag: whether already asked for key
{
    ਪੂਰਨ ਵਿਧੀ = crypt_ਵਿਧੀ_nr_from_magic((ਅੱਖਰ *)ptr, *sizep);
    ਪੂਰਨ b_p_ro = curbuf->b_p_ro;

    ਜੇ (ਵਿਧੀ >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_ਸੈੱਟ_cm_option(curbuf, ਵਿਧੀ);
	ਜੇ (cryptkey == ਨਿਰਾ && !*did_ask)
	{
	    ਜੇ (*curbuf->b_p_ਕੁੰਜੀ)
	    {
		cryptkey = curbuf->b_p_ਕੁੰਜੀ;
		crypt_check_swapfile_curbuf();
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_ਕੁੰਜੀ_ਸੁਨੇਹਾ), fname);
		ਸੁਨੇਹਾ_scroll = TRUE;
		crypt_check_ਵਿਧੀ(ਵਿਧੀ);
		cryptkey = crypt_get_ਕੁੰਜੀ(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		ਜੇ (cryptkey != ਨਿਰਾ && *cryptkey == NUL)
		{
		    ਜੇ (cryptkey != curbuf->b_p_ਕੁੰਜੀ)
			vim_free(cryptkey);
		    cryptkey = ਨਿਰਾ;
		}
	    }
	}

	ਜੇ (cryptkey != ਨਿਰਾ)
	{
	    ਪੂਰਨ ਸਿਰਲੇਖ_len;

	    ਸਿਰਲੇਖ_len = crypt_get_ਸਿਰਲੇਖ_len(ਵਿਧੀ);
	    ਜੇ (*sizep < ਸਿਰਲੇਖ_len)
	    {
		// invalid header, buffer can't be encrypted
		ਜੇ (cryptkey != curbuf->b_p_ਕੁੰਜੀ)
		    vim_free(cryptkey);
		ਵਾਪਸ ਨਿਰਾ;
	    }

	    curbuf->b_cryptstate = crypt_create_from_ਸਿਰਲੇਖ(
							ਵਿਧੀ, cryptkey, ptr);
	    crypt_ਸੈੱਟ_cm_option(curbuf, ਵਿਧੀ);

	    // Remove cryptmethod specific header from the text.
	    *filesizep += ਸਿਰਲੇਖ_len;
	    *sizep -= ਸਿਰਲੇਖ_len;
	    mch_memmove(ptr, ptr + ਸਿਰਲੇਖ_len, (size_t)*sizep);

	    // Restore the read-only flag.
	    curbuf->b_p_ro = b_p_ro;
	}
    }
    // When starting to edit a new file which does not have encryption, clear
    // the 'key' option, except when starting up (called with -x argument)
    ਨਹੀਂਤਾਂ ਜੇ (newfile && *curbuf->b_p_ਕੁੰਜੀ != NUL && !starting)
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    ਵਾਪਸ cryptkey;
}
#ਜੇ_ਅੰਤ  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    ਪੂਰਨ
check_file_readonly(
    char_u	*fname,		// full path to file
    ਪੂਰਨ		perm ਅਣਵਰਤਿਆ)	// known permissions on file
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_MCH_ACCESS
    ਪੂਰਨ	    fd = 0;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((ਅੱਖਰ *)fname, W_OK)
#ਨਹੀਂਤਾਂ
	(fd = mch_open((ਅੱਖਰ *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#ਜੇ_ਅੰਤ
	);
}

#ਜੇ defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    ਪੂਰਨ
vim_fsync(ਪੂਰਨ fd)
{
    ਪੂਰਨ r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    ਜੇ (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    ਵਾਪਸ r;
}
#ਜੇ_ਅੰਤ

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    ਪੂਰਨ
ਸੈੱਟ_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    ਜੇ (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (aborting())	    // autocmds may abort script processing
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
    ਜੇ (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_ਬਦਲਿਆ_ਬਫਰ_or_ਬਫਰ_name));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    ਜੇ (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (aborting())	    // autocmds may abort script processing
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

    // Do filetype detection now if 'filetype' is empty.
    ਜੇ (*curbuf->b_p_ft == NUL)
    {
	ਜੇ (au_has_ਸਮੂਹ((char_u *)"filetypedetect"))
	    (ਖਾਲੀ)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, ਨਿਰਾ);
	do_modelines(0);
    }

    ਵਾਪਸ OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_ਜੋੜੋ_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    ਜੇ (fname == ਨਿਰਾ)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    ਪੂਰਨ
ਸੁਨੇਹਾ_ਜੋੜੋ_fileformat(ਪੂਰਨ eol_type)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_CRNL
    ਜੇ (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_ਪਾਠ) ? _("[dos]") : _("[dos format]"));
	ਵਾਪਸ TRUE;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_ਪਾਠ) ? _("[mac]") : _("[mac format]"));
	ਵਾਪਸ TRUE;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CRNL
    ਜੇ (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_ਪਾਠ) ? _("[unix]") : _("[unix format]"));
	ਵਾਪਸ TRUE;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_ਜੋੜੋ_ਸਤਰਾਂ(
    ਪੂਰਨ	    ਪਾਓ_space,
    ਲੰਮਾ    lnum,
    off_T   nchars)
{
    ਪੂਰਨ  len = (ਪੂਰਨ)STRLEN(IObuff);

    ਜੇ (shortmess(SHM_ਸਤਰਾਂ))
	vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), ਪਾਓ_space ? " " : "", lnum, (varnumber_T)nchars);
    ਨਹੀਂਤਾਂ
    {
	len += vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), ਪਾਓ_space ? " " : "", lnum);
	vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_ਜੋੜੋ_eol(ਖਾਲੀ)
{
    STRCAT(IObuff, shortmess(SHM_ਆਖਰੀ) ? _("[noeol]") : _("[Incomplete last line]"));
}

    ਪੂਰਨ
ਸਮਾਂ_differs(stat_T *st, ਲੰਮਾ mtime, ਲੰਮਾ mtime_ns ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ST_MTIM_NSEC
	(ਲੰਮਾ)st->ST_MTIM_NSEC != mtime_ns ||
#ਜੇ_ਅੰਤ
#ਜੇ defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(ਲੰਮਾ)st->st_mtime - mtime > 1 || mtime - (ਲੰਮਾ)st->st_mtime > 1
#ਨਹੀਂਤਾਂ
	(ਲੰਮਾ)st->st_mtime != mtime
#ਜੇ_ਅੰਤ
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    ਪੂਰਨ
need_conversion(char_u *fenc)
{
    ਪੂਰਨ		same_encoding;
    ਪੂਰਨ		enc_flags;
    ਪੂਰਨ		fenc_flags;

    ਜੇ (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    ਜੇ (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	ਵਾਪਸ enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    ਵਾਪਸ !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    ਪੂਰਨ
get_fio_flags(char_u *ptr)
{
    ਪੂਰਨ		prop;

    ਜੇ (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    ਜੇ (prop & ENC_UNICODE)
    {
	ਜੇ (prop & ENC_2BYTE)
	{
	    ਜੇ (prop & ENC_ENDIAN_L)
		ਵਾਪਸ FIO_UCS2 | FIO_ENDIAN_L;
	    ਵਾਪਸ FIO_UCS2;
	}
	ਜੇ (prop & ENC_4BYTE)
	{
	    ਜੇ (prop & ENC_ENDIAN_L)
		ਵਾਪਸ FIO_UCS4 | FIO_ENDIAN_L;
	    ਵਾਪਸ FIO_UCS4;
	}
	ਜੇ (prop & ENC_2_ਸ਼ਬਦ)
	{
	    ਜੇ (prop & ENC_ENDIAN_L)
		ਵਾਪਸ FIO_UTF16 | FIO_ENDIAN_L;
	    ਵਾਪਸ FIO_UTF16;
	}
	ਵਾਪਸ FIO_UTF8;
    }
    ਜੇ (prop & ENC_LATIN1)
	ਵਾਪਸ FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    ਵਾਪਸ 0;
}

#ਜੇ defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    ਪੂਰਨ
get_win_fio_flags(char_u *ptr)
{
    ਪੂਰਨ		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    ਜੇ (!enc_utf8 && enc_codepage <= 0)
	ਵਾਪਸ 0;

    cp = encname2codepage(ptr);
    ਜੇ (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	ਜੇ (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	ਨਹੀਂਤਾਂ
# endif
	    ਵਾਪਸ 0;
    }
    ਵਾਪਸ FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    ਪੂਰਨ
get_mac_fio_flags(char_u *ptr)
{
    ਜੇ ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	ਵਾਪਸ FIO_MACROMAN;
    ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    ਸਥਿਰ char_u *
check_for_bom(
    char_u	*p,
    ਲੰਮਾ	ਮਾਪ,
    ਪੂਰਨ		*lenp,
    ਪੂਰਨ		flags)
{
    ਅੱਖਰ	*name = ਨਿਰਾ;
    ਪੂਰਨ		len = 2;

    ਜੇ (p[0] == 0xef && p[1] == 0xbb && ਮਾਪ >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    ਨਹੀਂਤਾਂ ਜੇ (p[0] == 0xff && p[1] == 0xfe)
    {
	ਜੇ (ਮਾਪ >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	ਨਹੀਂਤਾਂ ਜੇ (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	ਨਹੀਂਤਾਂ ਜੇ (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    ਨਹੀਂਤਾਂ ਜੇ (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	ਜੇ (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	ਨਹੀਂਤਾਂ
	    name = "utf-16";	// FE FF
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਮਾਪ >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    ਵਾਪਸ (char_u *)name;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns "full_path" or pointer into "full_path" if shortened.
 */
    char_u *
shorten_fname1(char_u *ਭਰਿਆ_ਰਸਤਾ)
{
    char_u	*dirname;
    char_u	*p = ਭਰਿਆ_ਰਸਤਾ;

    dirname = alloc(MAXPATHL);
    ਜੇ (dirname == ਨਿਰਾ)
	ਵਾਪਸ ਭਰਿਆ_ਰਸਤਾ;
    ਜੇ (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(ਭਰਿਆ_ਰਸਤਾ, dirname);
	ਜੇ (p == ਨਿਰਾ || *p == NUL)
	    p = ਭਰਿਆ_ਰਸਤਾ;
    }
    vim_free(dirname);
    ਵਾਪਸ p;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns NULL if not shorter name possible, pointer into "full_path"
 * otherwise.
 */
    char_u *
shorten_fname(char_u *ਭਰਿਆ_ਰਸਤਾ, char_u *dir_name)
{
    ਪੂਰਨ		len;
    char_u	*p;

    ਜੇ (ਭਰਿਆ_ਰਸਤਾ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    len = (ਪੂਰਨ)STRLEN(dir_name);
    ਜੇ (fnamencmp(dir_name, ਭਰਿਆ_ਰਸਤਾ, len) == 0)
    {
	p = ਭਰਿਆ_ਰਸਤਾ + len;
#ਜੇ defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	ਜੇ (!((len > 2) && (*(p - 2) == ':')))
#ਜੇ_ਅੰਤ
	{
	    ਜੇ (vim_ispathsep(*p))
		ਕਰੋ
		    ++p;
		ਜਦੋਂ (vim_ispathsep_nocolon(*p));
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VMS   // the path separator is always part of the path
	    ਨਹੀਂਤਾਂ
		p = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	}
    }
#ਜੇ defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    ਨਹੀਂਤਾਂ ਜੇ (len > 3
	    && TOUPPER_LOC(ਭਰਿਆ_ਰਸਤਾ[0]) == TOUPPER_LOC(dir_name[0])
	    && ਭਰਿਆ_ਰਸਤਾ[1] == ':'
	    && vim_ispathsep(ਭਰਿਆ_ਰਸਤਾ[2]))
	p = ਭਰਿਆ_ਰਸਤਾ + 2;
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
	p = ਨਿਰਾ;
    ਵਾਪਸ p;
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
    ਖਾਲੀ
shorten_buf_fname(buf_T *buf, char_u *dirname, ਪੂਰਨ force)
{
    char_u	*p;

    ਜੇ (buf->b_fname != ਨਿਰਾ
	    && !bt_nofilename(buf)
	    && !ਰਸਤਾ_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == ਨਿਰਾ
		|| mch_is_ਭਰਿਆ_Name(buf->b_sfname)))
    {
	ਜੇ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	ਜੇ (p != ਨਿਰਾ)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	ਜੇ (p == ਨਿਰਾ || buf->b_fname == ਨਿਰਾ)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    ਖਾਲੀ
shorten_fnames(ਪੂਰਨ force)
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
    ਸਥਿਤੀ_redraw_all();
    redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#ਜੇ_ਅੰਤ
}

#ਜੇ (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    ਖਾਲੀ
shorten_filenames(char_u **fnames, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਪੂਰਨ		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    ਜੇ (fnames == ਨਿਰਾ || ਗਿਣਤੀ_2 < 1)
	ਵਾਪਸ;
    mch_dirname(dirname, ਆਕਾਰ(dirname));
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	ਜੇ ((p = shorten_fname(fnames[i], dirname)) != ਨਿਰਾ)
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
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ	    prepend_dot)	// may prepend a '.' to file name
{
    ਵਾਪਸ buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    ਪੂਰਨ	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    ਪੂਰਨ	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    size_t	ptrlen;
    ਪੂਰਨ		fnamelen, extlen;

    extlen = (ਪੂਰਨ)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    ਜੇ (fname == ਨਿਰਾ || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	ਜੇ (retval == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (ਪੂਰਨ)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    ਨਹੀਂਤਾਂ
    {
	fnamelen = (ਪੂਰਨ)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	ਜੇ (retval == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	STRCPY(retval, fname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
	vms_remove_ਸੰਸਕਰਣ(retval); // we do not need versions here
#ਜੇ_ਅੰਤ
    }

    /*
     * search backwards until we hit a '/', '\' or ':' replacing all '.'
     * by '_' for MSDOS or when shortname option set and ext starts with a dot.
     * Then truncate what is after the '/', '\' or ':' to 8 characters for
     * MSDOS and 26 characters for AMIGA, a lot more for UNIX.
     */
    ਲਈ (ptr = retval + fnamelen; ptr > retval; MB_PTR_ਪਿੱਠ(retval, ptr))
    {
	ਜੇ (*ext == '.' && shortname)
	    ਜੇ (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	ਜੇ (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    ਰੋਕੋ;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    ਜੇ (ptrlen > (ਬਿਨਾਨਿਸ਼ਾਨੀ)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    ਜੇ (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	ਜੇ (fname == ਨਿਰਾ || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    ਜੇ (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	ਨਹੀਂਤਾਂ ਜੇ (*ext == '.')
	{
	    ਜੇ ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	ਨਹੀਂਤਾਂ ਜੇ ((e = vim_strchr(ptr, '.')) == ਨਿਰਾ)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	ਨਹੀਂਤਾਂ ਜੇ ((ਪੂਰਨ)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    ਨਹੀਂਤਾਂ ਜੇ ((fname == ਨਿਰਾ || *fname == NUL) && *ext == '.')
	*s++ = '_';
#ਜੇ_ਅੰਤ

    /*
     * Append the extension.
     * ext can start with '.' and cannot exceed 3 more characters.
     */
    STRCPY(s, ext);
    /*
     * Prepend the dot.
     */
    ਜੇ (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (size_t)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    ਜੇ (fname != ਨਿਰਾ && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	ਜਦੋਂ (--s >= ptr)
	{
	    ਜੇ (*s != '_')
	    {
		*s = '_';
		ਰੋਕੋ;
	    }
	}
	ਜੇ (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    ਵਾਪਸ retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    ਪੂਰਨ
vim_fgets(char_u *buf, ਪੂਰਨ ਮਾਪ, FILE *fp)
{
    ਅੱਖਰ	*eof;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FGETS_ਮਾਪ 200
    ਅੱਖਰ	tbuf[FGETS_ਮਾਪ];

    // safety check
    ਜੇ (ਮਾਪ < 2)
    {
	ਜੇ (ਮਾਪ == 1)
	    buf[0] = NUL;
	ਵਾਪਸ TRUE;
    }

    buf[ਮਾਪ - 2] = NUL;
    eof = fgets((ਅੱਖਰ *)buf, ਮਾਪ, fp);
    ਜੇ (buf[ਮਾਪ - 2] != NUL && buf[ਮਾਪ - 2] != '\n')
    {
	buf[ਮਾਪ - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	ਕਰੋ
	{
	    tbuf[FGETS_ਮਾਪ - 2] = NUL;
	    vim_ignoredp = fgets((ਅੱਖਰ *)tbuf, FGETS_ਮਾਪ, fp);
	} ਜਦੋਂ (tbuf[FGETS_ਮਾਪ - 2] != NUL && tbuf[FGETS_ਮਾਪ - 2] != '\n');
    }
    ਵਾਪਸ (eof == ਨਿਰਾ);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    ਪੂਰਨ
vim_rename(char_u *from, char_u *ਮੰਜ਼ਿਲ)
{
    ਪੂਰਨ		n;
    ਪੂਰਨ		ret;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
    BPTR	flock;
#ਜੇ_ਅੰਤ
    stat_T	st;
    ਪੂਰਨ		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    ਜੇ (fnamecmp(from, ਮੰਜ਼ਿਲ) == 0)
    {
	ਜੇ (p_fic && STRCMP(gettail(from), gettail(ਮੰਜ਼ਿਲ)) != 0)
	    use_tmp_file = TRUE;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    ਜੇ (mch_stat((ਅੱਖਰ *)from, &st) < 0)
	ਵਾਪਸ -1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    {
	stat_T	st_ਮੰਜ਼ਿਲ;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	ਜੇ (mch_stat((ਅੱਖਰ *)ਮੰਜ਼ਿਲ, &st_ਮੰਜ਼ਿਲ) >= 0
		&& st.st_dev == st_ਮੰਜ਼ਿਲ.st_dev
		&& st.st_ino == st_ਮੰਜ਼ਿਲ.st_ino)
	    use_tmp_file = TRUE;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    {
	BY_HANDLE_FILE_INFORMATION info1, info2;

	// It's possible for the source and destination to be the same file.
	// In that case go through a temp file name.  This makes rename("foo",
	// "./foo") a no-op (in a complicated way).
	ਜੇ (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(ਮੰਜ਼ਿਲ, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_ਸੰਖਿਆ == info2.dw_Volume_Serial_ਸੰਖਿਆ
		&& info1.n_File_ਸੂਚਕਾਂਕ_ਉੱਚਾ == info2.n_File_ਸੂਚਕਾਂਕ_ਉੱਚਾ
		&& info1.n_File_ਸੂਚਕਾਂਕ_ਨੀਵਾਂ == info2.n_File_ਸੂਚਕਾਂਕ_ਨੀਵਾਂ)
	    use_tmp_file = TRUE;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (use_tmp_file)
    {
	ਅੱਖਰ	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	ਜੇ (STRLEN(from) >= MAXPATHL - 5)
	    ਵਾਪਸ -1;
	STRCPY(tempname, from);
	ਲਈ (n = 123; n < 99999; ++n)
	{
	    sprintf((ਅੱਖਰ *)gettail((char_u *)tempname), "%d", n);
	    ਜੇ (mch_stat(tempname, &st) < 0)
	    {
		ਜੇ (mch_rename((ਅੱਖਰ *)from, tempname) == 0)
		{
		    ਜੇ (mch_rename(tempname, (ਅੱਖਰ *)ਮੰਜ਼ਿਲ) == 0)
			ਵਾਪਸ 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (ਖਾਲੀ)mch_rename(tempname, (ਅੱਖਰ *)from);
		    ਵਾਪਸ -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		ਵਾਪਸ -1;
	    }
	}
	ਵਾਪਸ -1;
    }

    /*
     * Delete the "to" file, this is required on some systems to make the
     * mch_rename() work, on other systems it makes sure that we don't have
     * two files when the mch_rename() fails.
     */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
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
    flock = ਤਾਲਾ_2((UBYTE *)from, (ਲੰਮਾ)VIM_ACCESS_READ);
#ਜੇ_ਅੰਤ
    mch_remove(ਮੰਜ਼ਿਲ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
    ਜੇ (flock)
	Un_ਤਾਲਾ(flock);
#ਜੇ_ਅੰਤ

    /*
     * First try a normal rename, return if it works.
     */
    ਜੇ (mch_rename((ਅੱਖਰ *)from, (ਅੱਖਰ *)ਮੰਜ਼ਿਲ) == 0)
	ਵਾਪਸ 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, ਮੰਜ਼ਿਲ);
    ਜੇ (ret != OK)
	ਵਾਪਸ -1;

    /*
     * Remove copied original file
     */
    ਜੇ (mch_stat((ਅੱਖਰ *)from, &st) >= 0)
	mch_remove(from);

    ਵਾਪਸ 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    ਪੂਰਨ
vim_copyfile(char_u *from, char_u *ਮੰਜ਼ਿਲ)
{
    ਪੂਰਨ		fd_ਵਿੱਚ;
    ਪੂਰਨ		fd_ਬਾਹਰਲਾ;
    ਪੂਰਨ		n;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    ਅੱਖਰ	*ਬਫਰ;
    ਲੰਮਾ	perm;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_READLINK
    ਪੂਰਨ		ret;
    ਪੂਰਨ		len;
    stat_T	st;
    ਅੱਖਰ	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((ਅੱਖਰ *)from, &st);
    ਜੇ (ret >= 0 && S_ISLNK(st.st_ਬਹੁਲਕ))
    {
	ret = -1;

	len = readlink((ਅੱਖਰ *)from, linkbuf, MAXPATHL);
	ਜੇ (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (ਅੱਖਰ *)ਮੰਜ਼ਿਲ);
	}

	ਵਾਪਸ ret == 0 ? OK : FAIL;
    }
#ਜੇ_ਅੰਤ

    perm = mch_getperm(from);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#ਜੇ_ਅੰਤ
    fd_ਵਿੱਚ = mch_open((ਅੱਖਰ *)from, O_RDONLY|O_EXTRA, 0);
    ਜੇ (fd_ਵਿੱਚ == -1)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
	mch_free_acl(acl);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_ਬਾਹਰਲਾ = mch_open((ਅੱਖਰ *)ਮੰਜ਼ਿਲ,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (ਪੂਰਨ)perm);
    ਜੇ (fd_ਬਾਹਰਲਾ == -1)
    {
	close(fd_ਵਿੱਚ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
	mch_free_acl(acl);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ FAIL;
    }

    ਬਫਰ = alloc(WRITEBUFSIZE);
    ਜੇ (ਬਫਰ == ਨਿਰਾ)
    {
	close(fd_ਬਾਹਰਲਾ);
	close(fd_ਵਿੱਚ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
	mch_free_acl(acl);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ FAIL;
    }

    ਜਦੋਂ ((n = read_eintr(fd_ਵਿੱਚ, ਬਫਰ, WRITEBUFSIZE)) > 0)
	ਜੇ (write_eintr(fd_ਬਾਹਰਲਾ, ਬਫਰ, n) != n)
	{
	    errmsg = _(e_error_writing_ਮੰਜ਼ਿਲ_str);
	    ਰੋਕੋ;
	}

    vim_free(ਬਫਰ);
    close(fd_ਵਿੱਚ);
    ਜੇ (close(fd_ਬਾਹਰਲਾ) < 0)
	errmsg = _(e_error_closing_str);
    ਜੇ (n < 0)
    {
	errmsg = _(e_error_reading_str);
	ਮੰਜ਼ਿਲ = from;
    }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX	    // for Unix mch_open() already set the permission
    mch_setperm(ਮੰਜ਼ਿਲ, perm);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    mch_ਸੈੱਟ_acl(ਮੰਜ਼ਿਲ, acl);
    mch_free_acl(acl);
#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, ਮੰਜ਼ਿਲ);
#ਜੇ_ਅੰਤ
    ਜੇ (errmsg != ਨਿਰਾ)
    {
	semsg(errmsg, ਮੰਜ਼ਿਲ);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

ਸਥਿਰ ਪੂਰਨ already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    ਪੂਰਨ
check_timestamps(
    ਪੂਰਨ		focus)		// called for GUI focus event
{
    buf_T	*buf;
    ਪੂਰਨ		didit = 0;
    ਪੂਰਨ		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    ਜੇ (no_check_timestamps > 0)
	ਵਾਪਸ FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    ਜੇ (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	ਵਾਪਸ FALSE;
    }

    ਜੇ (!stuff_ਖਾਲੀ_ਥਾਵਾਂ() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_ਤਾਲਾ > 0 || allbuf_ਤਾਲਾ > 0)
	need_check_timestamps = TRUE;		// check later
    ਨਹੀਂਤਾਂ
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    ਜੇ (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		ਸੈੱਟ_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		ਜੇ (didit < n)
		    didit = n;
		ਜੇ (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    ਜਾਰੀ;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	ਜੇ (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    ਸੁਨੇਹਾ_puts("\n");
	    ਬਾਹਰਲਾ_flush();
	}
    }
    ਵਾਪਸ didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    ਸਥਿਰ ਪੂਰਨ
move_ਸਤਰਾਂ(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    ਪੂਰਨ		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    ਲਈ (lnum = 1; lnum <= frombuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	ਜੇ (p == ਨਿਰਾ || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    ਰੋਕੋ;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    ਜੇ (retval != FAIL)
    {
	curbuf = frombuf;
	ਲਈ (lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; lnum > 0; --lnum)
	    ਜੇ (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		ਰੋਕੋ;
	    }
    }

    curbuf = tbuf;
    ਵਾਪਸ retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    ਪੂਰਨ
buf_check_timestamp(
    buf_T	*buf,
    ਪੂਰਨ		focus ਅਣਵਰਤਿਆ)	// called for GUI focus event
{
    stat_T	st;
    ਪੂਰਨ		stat_res;
    ਪੂਰਨ		retval = 0;
    ਅੱਖਰ	*mesg = ਨਿਰਾ;
    ਅੱਖਰ	*mesg2 = "";
    ਪੂਰਨ		helpmesg = FALSE;
    ਗਿਣਤੀ {
	RELOAD_NONE,
	RELOAD_ਸਾਧਾਰਨ,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#ਜੇ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    ਪੂਰਨ		can_reload = FALSE;
#ਜੇ_ਅੰਤ
    off_T	orig_ਮਾਪ = buf->b_orig_ਮਾਪ;
    ਪੂਰਨ		orig_ਬਹੁਲਕ = buf->b_orig_ਬਹੁਲਕ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਪੂਰਨ		save_mouse_correct = need_mouse_correct;
#ਜੇ_ਅੰਤ
    ਸਥਿਰ ਪੂਰਨ	busy = FALSE;
    ਪੂਰਨ		n;
    bufref_T	bufref;

    ਸੈੱਟ_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    ਜੇ (buf->b_ffname == ਨਿਰਾ
	    || buf->b_ml.ml_mfp == ਨਿਰਾ
	    || !bt_ਸਾਧਾਰਨ(buf)
	    || buf->b_saving
	    || busy
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    || is_Netbeans_ਬਫਰ(buf)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    || buf->b_ਪਦ != ਨਿਰਾ
#ਜੇ_ਅੰਤ
	    )
	ਵਾਪਸ 0;

    ਜੇ (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((ਅੱਖਰ *)buf->b_ffname, &st)) < 0
		|| ਸਮਾਂ_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_ਮਾਪ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ST_ਬਹੁਲਕ
		|| (ਪੂਰਨ)st.st_ਬਹੁਲਕ != buf->b_orig_ਬਹੁਲਕ
#ਨਹੀਂਤਾਂ
		|| mch_getperm(buf->b_ffname) != buf->b_orig_ਬਹੁਲਕ
#ਜੇ_ਅੰਤ
		))
    {
	ਲੰਮਾ prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	ਜੇ (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_ਮਾਪ = 0;
	    buf->b_orig_ਬਹੁਲਕ = 0;
	}
	ਨਹੀਂਤਾਂ
	    buf_store_ਸਮਾਂ(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	ਜੇ (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	ਨਹੀਂਤਾਂ ਜੇ ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !buf_Is_ਬਦਲਿਆ(buf) && stat_res >= 0)
	    reload = RELOAD_ਸਾਧਾਰਨ;
	ਨਹੀਂਤਾਂ
	{
	    ਅੱਖਰ    *reason;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    size_t  reasonlen;
#ਜੇ_ਅੰਤ

	    ਜੇ (stat_res < 0)
	    {
		reason = "deleted";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (buf_Is_ਬਦਲਿਆ(buf))
	    {
		reason = "conflict";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("conflict");
#ਜੇ_ਅੰਤ
	    }
	    /*
	     * Check if the file contents really changed to avoid giving a
	     * warning when only the timestamp was set (e.g., checked out of
	     * CVS).  Always warn when the buffer was changed.
	     */
	    ਨਹੀਂਤਾਂ ਜੇ (orig_ਮਾਪ != buf->b_orig_ਮਾਪ || buf_contents_ਬਦਲਿਆ(buf))
	    {
		reason = "changed";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (orig_ਬਹੁਲਕ != buf->b_orig_ਬਹੁਲਕ)
	    {
		reason = "mode";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
	    {
		reason = "time";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("time");
#ਜੇ_ਅੰਤ
	    }

	    /*
	     * Only give the warning if there are no FileChangedShell
	     * autocommands.
	     * Avoid being called recursively by setting "busy".
	     */
	    busy = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਸੈੱਟ_vim_var_string(VV_FCS_REASON, (char_u *)reason, (ਪੂਰਨ)reasonlen);
	    ਸੈੱਟ_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#ਜੇ_ਅੰਤ
	    ++allbuf_ਤਾਲਾ;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_ਤਾਲਾ;
	    busy = FALSE;
	    ਜੇ (n)
	    {
		ਜੇ (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_ਬਫਰ));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		ਜੇ (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_ਸਾਧਾਰਨ;
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s, "ask") == 0)
		    n = FALSE;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    ਵਾਪਸ 2;
	    }
	    ਜੇ (!n)
	    {
		ਜੇ (*reason == 'd')
		{
		    // Only give the message once.
		    ਜੇ (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_ਉਪਲਬਧ);
		}
		ਨਹੀਂਤਾਂ
		{
		    helpmesg = TRUE;
#ਜੇ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#ਜੇ_ਅੰਤ
		    ਜੇ (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    ਨਹੀਂਤਾਂ
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
    ਨਹੀਂਤਾਂ ਜੇ ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#ਜੇ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#ਜੇ_ਅੰਤ
    }

    ਜੇ (mesg != ਨਿਰਾ)
    {
	char_u *ਰਸਤਾ;

	ਰਸਤਾ = home_replace_save(buf, buf->b_fname);
	ਜੇ (ਰਸਤਾ != ਨਿਰਾ)
	{
	    size_t  tbufsize;
	    ਅੱਖਰ    *tbuf;

	    ਜੇ (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(ਰਸਤਾ) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    ਜੇ (tbuf != ਨਿਰਾ)
	    {
		ਪੂਰਨ tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, ਰਸਤਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		ਸੈੱਟ_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		ਜੇ (can_reload)
		{
		    ਜੇ (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    ਚੋਣ (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, ਨਿਰਾ, TRUE))
		    {
			ਹਾਲਤ 2:
			    reload = RELOAD_ਸਾਧਾਰਨ;
			    ਰੋਕੋ;
			ਹਾਲਤ 3:
			    reload = RELOAD_DETECT;
			    ਰੋਕੋ;
		    }
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਜੇ (State > ਬਹੁਲਕ_ਸਾਧਾਰਨ_BUSY || (State & ਬਹੁਲਕ_CMDLINE)
								 || already_warned)
		{
		    ਜੇ (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (!autocmd_busy)
		    {
			ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
			ਸੁਨੇਹਾ_puts_attr(tbuf, HL_ATTR(HLF_E) + ਸੁਨੇਹਾ_HIST);
			ਜੇ (*mesg2 != NUL)
			    ਸੁਨੇਹਾ_puts_attr(mesg2, HL_ATTR(HLF_W) + ਸੁਨੇਹਾ_HIST);
			ਸੁਨੇਹਾ_clr_eos();
			(ਖਾਲੀ)ਸੁਨੇਹਾ_ਸਿਰਾ();
			ਜੇ (emsg_silent == 0 && !ਵਿੱਚ_assert_fails)
			{
			    ਬਾਹਰਲਾ_flush();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			    ਜੇ (!focus)
#ਜੇ_ਅੰਤ
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
	    vim_free(ਰਸਤਾ);
	}
    }

    ਜੇ (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_ਬਹੁਲਕ, reload == RELOAD_DETECT);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
	ਜੇ (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != ਨਿਰਾ)
	{
	    char_u	    hash[UNDO_HASH_ਮਾਪ];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(ਨਿਰਾ, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#ਜੇ_ਅੰਤ
    }

    // Trigger FileChangedShell when the file was changed in any way.
    ਜੇ (bufref_valid(&bufref) && retval != 0)
	(ਖਾਲੀ)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    ਖਾਲੀ
buf_reload(buf_T *buf, ਪੂਰਨ orig_ਬਹੁਲਕ, ਪੂਰਨ reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    ਪੂਰਨ		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    ਪੂਰਨ		saved = OK;
    aco_save_T	aco;
    ਪੂਰਨ		flags = READ_NEW;
    ਪੂਰਨ		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    ਜੇ (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	ਵਾਪਸ;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    ਜੇ (reload_options)
	CLEAR_ਖੇਤਰ(ea);
    ਨਹੀਂਤਾਂ
	prepped = prep_exarg(&ea, buf);

    ਜੇ (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	ਜੇ (p_ur < 0 || curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	ਜੇ (BUFEMPTY() || saved == FAIL)
	    savebuf = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(ਨਿਰਾ, ਨਿਰਾ, (linenr_T)1, BLN_DUMMY);
	    ਸੈੱਟ_bufref(&bufref, savebuf);
	    ਜੇ (savebuf != ਨਿਰਾ && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_ਬਫਰ = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_ਬਫਰ = buf;
	    }
	    ਜੇ (savebuf == ਨਿਰਾ || saved == FAIL || buf != curbuf
				      || move_ਸਤਰਾਂ(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	ਜੇ (saved == OK)
	{
	    ਪੂਰਨ old_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    ਜੇ (shortmess(SHM_FILEINFO))
		ਸੁਨੇਹਾ_silent = 1;

	    ਜੇ (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#ਜੇ defined(FEAT_EVAL)
		ਜੇ (!aborting())
#ਜੇ_ਅੰਤ
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		ਜੇ (savebuf != ਨਿਰਾ && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    ਜਦੋਂ (!BUFEMPTY())
			ਜੇ (ml_delete(buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ) == FAIL)
			    ਰੋਕੋ;
		    (ਖਾਲੀ)move_ਸਤਰਾਂ(savebuf, buf);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		ਜੇ ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		ਨਹੀਂਤਾਂ
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    ਸੁਨੇਹਾ_silent = old_ਸੁਨੇਹਾ_silent;
	}
	vim_free(ea.cmd);

	ਜੇ (savebuf != ਨਿਰਾ && bufref_valid(&bufref))
	    wipe_ਬਫਰ(savebuf, FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#ਜੇ_ਅੰਤ

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	ਜੇ (old_topline > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    curwin->w_topline = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	ਨਹੀਂਤਾਂ
	    curwin->w_topline = old_topline;
	curwin->w_cursor = old_cursor;
	check_cursor();
	update_topline();
	curbuf->b_keep_filetype = false;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	{
	    win_T	*wp;
	    tabpage_T	*tp;

	    // Update folds unless they are defined manually.
	    FOR_ALL_TAB_WINDOWS(tp, wp)
		ਜੇ (wp->w_ਬਫਰ == curwin->w_ਬਫਰ
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#ਜੇ_ਅੰਤ
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	ਜੇ (orig_ਬਹੁਲਕ == curbuf->b_orig_ਬਹੁਲਕ)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    ਖਾਲੀ
buf_store_ਸਮਾਂ(buf_T *buf, stat_T *st, char_u *fname ਅਣਵਰਤਿਆ)
{
    buf->b_mtime = (ਲੰਮਾ)st->st_mtime;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ST_MTIM_NSEC
    buf->b_mtime_ns = (ਲੰਮਾ)st->ST_MTIM_NSEC;
#ਨਹੀਂਤਾਂ
    buf->b_mtime_ns = 0;
#ਜੇ_ਅੰਤ
    buf->b_orig_ਮਾਪ = st->st_size;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ST_ਬਹੁਲਕ
    buf->b_orig_ਬਹੁਲਕ = (ਪੂਰਨ)st->st_ਬਹੁਲਕ;
#ਨਹੀਂਤਾਂ
    buf->b_orig_ਬਹੁਲਕ = mch_getperm(fname);
#ਜੇ_ਅੰਤ
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    ਖਾਲੀ
write_lnum_adjust(linenr_T ਵਿੱਥ)
{
    ਜੇ (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += ਵਿੱਥ;
}

// Subfuncions for readdirex()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
# ifdef MSWIN
    ਸਥਿਰ char_u *
getfpermwfd(WIN32_ਲੱਭੋ_DATAW *wfd, char_u *perm)
{
    stat_T	    st;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ  st_ਬਹੁਲਕ;
    DWORD	    flag = wfd->dw_File_ਗੁਣ;
    WCHAR	    *wp;

    st_ਬਹੁਲਕ = (flag & FILE_ATTRIBUTE_DIRECTORY)
					? (_S_IFDIR | _S_IEXEC) : _S_IFREG;
    st_ਬਹੁਲਕ |= (flag & FILE_ATTRIBUTE_READONLY)
					? _S_IREAD : (_S_IREAD | _S_IWRITE);

    wp = wcsrchr(wfd->cFileName, L'.');
    ਜੇ (wp != ਨਿਰਾ)
    {
	ਜੇ (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_ਬਹੁਲਕ |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_ਬਹੁਲਕ |= (st_ਬਹੁਲਕ & 0700) >> 3;
    st_ਬਹੁਲਕ |= (st_ਬਹੁਲਕ & 0700) >> 6;

    st.st_ਬਹੁਲਕ = st_ਬਹੁਲਕ;
    ਵਾਪਸ getfpermst(&st, perm);
}

    ਸਥਿਰ ਖਾਲੀ
getftypewfd(WIN32_ਲੱਭੋ_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dw_File_ਗੁਣ;
    DWORD ਟੈਗ = wfd->dwReserved0;

    ਜੇ (flag & FILE_ATTRIBUTE_REPARSE_ਬਿੰਦੂ)
    {
	ਜੇ (ਟੈਗ == IO_REPARSE_ਟੈਗ_MOUNT_ਬਿੰਦੂ)
	{
	    ret->string = (char_u *)"junction";
	    ret->ਲੰਮਾਈ = STRLEN_LITERAL("junction");
	    ਵਾਪਸ;
	}
	ਜੇ (ਟੈਗ == IO_REPARSE_ਟੈਗ_SYMLINK)
	{
	    ਜੇ (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->ਲੰਮਾਈ = STRLEN_LITERAL("linkd");
		ਵਾਪਸ;
	    }

	    ret->string = (char_u *)"link";
	    ret->ਲੰਮਾਈ = STRLEN_LITERAL("link");
	    ਵਾਪਸ;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->ਲੰਮਾਈ = STRLEN_LITERAL("reparse");
	ਵਾਪਸ;
    }

    ਜੇ (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->ਲੰਮਾਈ = STRLEN_LITERAL("dir");
	ਵਾਪਸ;
    }

    ret->string = (char_u *)"file";
    ret->ਲੰਮਾਈ = STRLEN_LITERAL("file");
}

    ਸਥਿਰ dict_T *
create_readdirex_ਵਸਤੂ(WIN32_ਲੱਭੋ_DATAW *wfd)
{
    dict_T	*ਵਸਤੂ;
    char_u	*p;
    varnumber_T	ਮਾਪ, ਸਮਾਂ;
    char_u	permbuf[] = "---------";
    string_T	s;

    ਵਸਤੂ = dict_alloc();
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਸਤੂ->dv_refcount++;

    p = utf16_ਮੰਜ਼ਿਲ_enc(wfd->cFileName, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਜਾਓ theend;
    ਜੇ (dict_ਜੋੜੋ_string(ਵਸਤੂ, "name", p) == FAIL)
    {
	vim_free(p);
	ਜਾਓ theend;
    }
    vim_free(p);

    ਮਾਪ = (((varnumber_T)wfd->n_File_ਮਾਪ_ਉੱਚਾ) << 32) | wfd->n_File_ਮਾਪ_ਨੀਵਾਂ;
    ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(ਵਸਤੂ, "size", ਮਾਪ) == FAIL)
	ਜਾਓ theend;

    // Convert FILETIME to unix time.
    ਸਮਾਂ = (((((varnumber_T)wfd->ft_ਆਖਰੀ_Write_ਸਮਾਂ.dw_ਉੱਚਾ_Date_ਸਮਾਂ) << 32) |
		wfd->ft_ਆਖਰੀ_Write_ਸਮਾਂ.dw_ਨੀਵਾਂ_Date_ਸਮਾਂ)
	    - 116444736000000000) / 10000000;
    ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(ਵਸਤੂ, "time", ਸਮਾਂ) == FAIL)
	ਜਾਓ theend;

    getftypewfd(wfd, &s);
    ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "type", s.string, (ਪੂਰਨ)s.ਲੰਮਾਈ) == FAIL)
	ਜਾਓ theend;
    ਜੇ (dict_ਜੋੜੋ_string(ਵਸਤੂ, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	ਜਾਓ theend;

    ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "user", (char_u *)"", 0) == FAIL)
	ਜਾਓ theend;
    ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "group", (char_u *)"", 0) == FAIL)
	ਜਾਓ theend;

    ਵਾਪਸ ਵਸਤੂ;

theend:
    dict_unref(ਵਸਤੂ);
    ਵਾਪਸ ਨਿਰਾ;
}
# ਨਹੀਂਤਾਂ
    ਸਥਿਰ dict_T *
create_readdirex_ਵਸਤੂ(char_u *ਰਸਤਾ, char_u *name)
{
    dict_T	*ਵਸਤੂ;
    ਅੱਖਰ	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    ਪੂਰਨ		ret, link = FALSE;
    varnumber_T	ਮਾਪ;
    char_u	permbuf[] = "---------";
    string_T	q = {ਨਿਰਾ, 0};
    ਬਣਤਰ passwd *pw;
    ਬਣਤਰ ਸਮੂਹ  *gr;

    ਵਸਤੂ = dict_alloc();
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਸਤੂ->dv_refcount++;

    pathlen = STRLEN(ਰਸਤਾ);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    ਜੇ (p == ਨਿਰਾ)
	ਜਾਓ theend;
    ਜੇ (pathlen > 0 && ਰਸਤਾ[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", ਰਸਤਾ, name);
    ਨਹੀਂਤਾਂ
	vim_snprintf(p, len, "%s/%s", ਰਸਤਾ, name);
    ret = mch_lstat(p, &st);
    ਜੇ (ret >= 0 && S_ISLNK(st.st_ਬਹੁਲਕ))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	ਜੇ (ret < 0)
	    STR_LITERAL_ਸੈੱਟ(q, "link");
    }
    vim_free(p);

    ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "name", name, (ਪੂਰਨ)namelen) == FAIL)
	ਜਾਓ theend;

    ਜੇ (ret >= 0)
    {
	ਮਾਪ = (varnumber_T)st.st_size;
	ਜੇ (S_ISDIR(st.st_ਬਹੁਲਕ))
	    ਮਾਪ = 0;
	// non-perfect check for overflow
	ਨਹੀਂਤਾਂ ਜੇ ((off_T)ਮਾਪ != (off_T)st.st_size)
	    ਮਾਪ = -2;
	ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(ਵਸਤੂ, "size", ਮਾਪ) == FAIL)
	    ਜਾਓ theend;
	ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(ਵਸਤੂ, "time", (varnumber_T)st.st_mtime) == FAIL)
	    ਜਾਓ theend;

	ਜੇ (link)
	{
	    ਜੇ (S_ISDIR(st.st_ਬਹੁਲਕ))
		STR_LITERAL_ਸੈੱਟ(q, "linkd");
	    ਨਹੀਂਤਾਂ
		STR_LITERAL_ਸੈੱਟ(q, "link");
	}
	ਨਹੀਂਤਾਂ
	{
	    q.string = getftypest(&st);
	    q.ਲੰਮਾਈ = STRLEN(q.string);
	}
	ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "type", q.string, (ਪੂਰਨ)q.ਲੰਮਾਈ) == FAIL)
	    ਜਾਓ theend;
	ਜੇ (dict_ਜੋੜੋ_string(ਵਸਤੂ, "perm", getfpermst(&st, permbuf)) == FAIL)
	    ਜਾਓ theend;

	pw = getpwuid(st.st_uid);
	ਜੇ (pw == ਨਿਰਾ)
	    STR_LITERAL_ਸੈੱਟ(q, "");
	ਨਹੀਂਤਾਂ
	{
	    q.string = (char_u *)pw->pw_name;
	    q.ਲੰਮਾਈ = STRLEN(q.string);
	}
	ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "user", q.string, (ਪੂਰਨ)q.ਲੰਮਾਈ) == FAIL)
	    ਜਾਓ theend;
#  ਜੇ !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	ਜੇ (gr == ਨਿਰਾ)
	    STR_LITERAL_ਸੈੱਟ(q, "");
	ਨਹੀਂਤਾਂ
	{
	    q.string = (char_u *)gr->gr_name;
	    q.ਲੰਮਾਈ = STRLEN(q.string);
	}
#  endif
	ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "group", q.string, (ਪੂਰਨ)q.ਲੰਮਾਈ) == FAIL)
	    ਜਾਓ theend;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(ਵਸਤੂ, "size", -1) == FAIL)
	    ਜਾਓ theend;
	ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(ਵਸਤੂ, "time", -1) == FAIL)
	    ਜਾਓ theend;
	ਜੇ (q.string == ਨਿਰਾ)
	{
	    ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "type", (char_u *)"", 0) == FAIL)
		ਜਾਓ theend;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "type", q.string, (ਪੂਰਨ)q.ਲੰਮਾਈ) == FAIL)
		ਜਾਓ theend;
	}
	ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "perm", (char_u *)"", 0) == FAIL)
	    ਜਾਓ theend;
	ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "user", (char_u *)"", 0) == FAIL)
	    ਜਾਓ theend;
	ਜੇ (dict_ਜੋੜੋ_string_len(ਵਸਤੂ, "group", (char_u *)"", 0) == FAIL)
	    ਜਾਓ theend;
    }
    ਵਾਪਸ ਵਸਤੂ;

theend:
    dict_unref(ਵਸਤੂ);
    ਵਾਪਸ ਨਿਰਾ;
}
# endif

    ਸਥਿਰ ਪੂਰਨ
compare_readdirex_ਵਸਤੂ(ਅਚਲ ਖਾਲੀ *p1, ਅਚਲ ਖਾਲੀ *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    ਜੇ (readdirex_ਛਾਂਟੋ == READDIR_ਛਾਂਟੋ_BYTE)
	ਵਾਪਸ STRCMP(name1, name2);
    ਜੇ (readdirex_ਛਾਂਟੋ == READDIR_ਛਾਂਟੋ_IC)
	ਵਾਪਸ STRICMP(name1, name2);

    ਵਾਪਸ STRCOLL(name1, name2);
}

    ਸਥਿਰ ਪੂਰਨ
compare_readdir_ਵਸਤੂ(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਜੇ (readdirex_ਛਾਂਟੋ == READDIR_ਛਾਂਟੋ_BYTE)
	ਵਾਪਸ STRCMP(*(ਅੱਖਰ **)s1, *(ਅੱਖਰ **)s2);
    ਜੇ (readdirex_ਛਾਂਟੋ == READDIR_ਛਾਂਟੋ_IC)
	ਵਾਪਸ STRICMP(*(ਅੱਖਰ **)s1, *(ਅੱਖਰ **)s2);

    ਵਾਪਸ STRCOLL(*(ਅੱਖਰ **)s1, *(ਅੱਖਰ **)s2);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    ਪੂਰਨ
readdir_core(
    garray_T	*gap,
    char_u	*ਰਸਤਾ,
    ਪੂਰਨ		withattr ਅਣਵਰਤਿਆ,
    ਖਾਲੀ	*context,
    ਪੂਰਨ		(*checkitem)(ਖਾਲੀ *context, ਖਾਲੀ *ਵਸਤੂ),
    ਪੂਰਨ		ਛਾਂਟੋ)
{
    ਪੂਰਨ			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    ਪੂਰਨ			ok;
    HANDLE		h_ਲੱਭੋ = INVALID_HANDLE_ਮੁੱਲ;
    WIN32_ਲੱਭੋ_DATAW    wfd;
    WCHAR		*wn = ਨਿਰਾ;	// UTF-16 name, NULL when not used.
    char_u		*p_ਸਿਰਾ;
# ਨਹੀਂਤਾਂ
    DIR			*dirp;
    ਬਣਤਰ dirent	*dp;
# endif

    ga_init2(gap, ਆਕਾਰ(ਖਾਲੀ *), 20);

# ifdef FEAT_EVAL
#  define FREE_ਵਸਤੂ(ਵਸਤੂ)   ਕਰੋ { \
	ਜੇ (withattr) \
	    dict_unref((dict_T*)(ਵਸਤੂ)); \
	ਨਹੀਂਤਾਂ \
	    vim_free(ਵਸਤੂ); \
    } ਜਦੋਂ (0)

    readdirex_ਛਾਂਟੋ = READDIR_ਛਾਂਟੋ_BYTE;
# ਨਹੀਂਤਾਂ
#  define FREE_ਵਸਤੂ(ਵਸਤੂ)   vim_free(ਵਸਤੂ)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    STRNCPY(buf, ਰਸਤਾ, MAXPATHL-5);
    p = p_ਸਿਰਾ = buf + STRLEN(buf);
    MB_PTR_ਪਿੱਠ(buf, p);
    ਜੇ (*p == '\\' || *p == '/')
	p_ਸਿਰਾ = p;
    STRCPY(p_ਸਿਰਾ, "\\*");

    wn = enc_ਮੰਜ਼ਿਲ_utf16(buf, ਨਿਰਾ);
    ਜੇ (wn != ਨਿਰਾ)
	h_ਲੱਭੋ = ਲੱਭੋ_ਪਹਿਲਾ_File_W(wn, &wfd);
    ok = (h_ਲੱਭੋ != INVALID_HANDLE_ਮੁੱਲ);
    ਜੇ (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), ਰਸਤਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜਦੋਂ (ok)
	{
	    ਪੂਰਨ	    ignore;
	    ਖਾਲੀ    *ਵਸਤੂ;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    ਜੇ (ignore)
	    {
		ok = ਲੱਭੋ_ਅਗਲਾ_File_W(h_ਲੱਭੋ, &wfd);
		ਜਾਰੀ;
	    }
#  ifdef FEAT_EVAL
	    ਜੇ (withattr)
		ਵਸਤੂ = (ਖਾਲੀ*)create_readdirex_ਵਸਤੂ(&wfd);
	    ਨਹੀਂਤਾਂ
#  endif
		ਵਸਤੂ = (ਖਾਲੀ*)utf16_ਮੰਜ਼ਿਲ_enc(wfd.cFileName, ਨਿਰਾ);
	    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	    {
		failed = TRUE;
		ਰੋਕੋ;
	    }

	    ਜੇ (!ignore && checkitem != ਨਿਰਾ)
	    {
		ਪੂਰਨ r = checkitem(context, ਵਸਤੂ);

		ਜੇ (r < 0)
		{
		    FREE_ਵਸਤੂ(ਵਸਤੂ);
		    ਰੋਕੋ;
		}
		ਜੇ (r == 0)
		    ignore = TRUE;
	    }

	    ਜੇ (!ignore)
	    {
		ਜੇ (ga_grow(gap, 1) == OK)
		    ((ਖਾਲੀ**)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = ਵਸਤੂ;
		ਨਹੀਂਤਾਂ
		{
		    failed = TRUE;
		    FREE_ਵਸਤੂ(ਵਸਤੂ);
		    ਰੋਕੋ;
		}
	    }
	    ਨਹੀਂਤਾਂ
		FREE_ਵਸਤੂ(ਵਸਤੂ);

	    ok = ਲੱਭੋ_ਅਗਲਾ_File_W(h_ਲੱਭੋ, &wfd);
	}
	ਲੱਭੋ_Close(h_ਲੱਭੋ);
    }

    vim_free(buf);
    vim_free(wn);
# ਨਹੀਂਤਾਂ	// MSWIN
    dirp = opendir((ਅੱਖਰ *)ਰਸਤਾ);
    ਜੇ (dirp == ਨਿਰਾ)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), ਰਸਤਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (;;)
	{
	    ਪੂਰਨ	    ignore;
	    ਖਾਲੀ    *ਵਸਤੂ;

	    dp = readdir(dirp);
	    ਜੇ (dp == ਨਿਰਾ)
		ਰੋਕੋ;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    ਜੇ (ignore)
		ਜਾਰੀ;
#  ifdef FEAT_EVAL
	    ਜੇ (withattr)
		ਵਸਤੂ = (ਖਾਲੀ*)create_readdirex_ਵਸਤੂ(ਰਸਤਾ, p);
	    ਨਹੀਂਤਾਂ
#  endif
		ਵਸਤੂ = (ਖਾਲੀ*)vim_strsave(p);
	    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	    {
		failed = TRUE;
		ਰੋਕੋ;
	    }

	    ਜੇ (checkitem != ਨਿਰਾ)
	    {
		ਪੂਰਨ r = checkitem(context, ਵਸਤੂ);

		ਜੇ (r < 0)
		{
		    FREE_ਵਸਤੂ(ਵਸਤੂ);
		    ਰੋਕੋ;
		}
		ਜੇ (r == 0)
		    ignore = TRUE;
	    }

	    ਜੇ (!ignore)
	    {
		ਜੇ (ga_grow(gap, 1) == OK)
		    ((ਖਾਲੀ**)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = ਵਸਤੂ;
		ਨਹੀਂਤਾਂ
		{
		    failed = TRUE;
		    FREE_ਵਸਤੂ(ਵਸਤੂ);
		    ਰੋਕੋ;
		}
	    }
	    ਨਹੀਂਤਾਂ
		FREE_ਵਸਤੂ(ਵਸਤੂ);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_ਵਸਤੂ

    ਜੇ (!failed && gap->ga_len > 0 && ਛਾਂਟੋ > READDIR_ਛਾਂਟੋ_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_ਛਾਂਟੋ = ਛਾਂਟੋ;
	ਜੇ (withattr)
	    qsort((ਖਾਲੀ*)gap->ga_ਅੰਕੜੇ, (size_t)gap->ga_len, ਆਕਾਰ(dict_T*),
		    compare_readdirex_ਵਸਤੂ);
	ਨਹੀਂਤਾਂ
	    qsort((ਖਾਲੀ*)gap->ga_ਅੰਕੜੇ, (size_t)gap->ga_len, ਆਕਾਰ(char_u *),
		    compare_readdir_ਵਸਤੂ);
# ਨਹੀਂਤਾਂ
	    ਛਾਂਟੋ_ਲੜੀਆਂ((char_u **)gap->ga_ਅੰਕੜੇ, gap->ga_len);
# endif
    }

    ਵਾਪਸ failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    ਪੂਰਨ
delete_recursive(char_u *name)
{
    ਪੂਰਨ ਨਤੀਜਾ = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    ਜੇ (
# ਜੇ defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# ਨਹੀਂਤਾਂ
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *ਘਾਤਾਂਕ = vim_strsave(name);
	garray_T    ga;

	ਜੇ (ਘਾਤਾਂਕ == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	ਜੇ (readdir_core(&ga, ਘਾਤਾਂਕ, FALSE, ਨਿਰਾ, ਨਿਰਾ, READDIR_ਛਾਂਟੋ_NONE) == OK)
	{
	    ਪੂਰਨ	len = vim_snprintf((ਅੱਖਰ *)NameBuff, MAXPATHL, "%s/", ਘਾਤਾਂਕ);
	    ਪੂਰਨ	i;

	    ਲਈ (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((ਅੱਖਰ *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_ਅੰਕੜੇ)[i]);
		ਜੇ (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    ਨਤੀਜਾ = -1;
	    }
	    ga_clear_ਲੜੀਆਂ(&ga);
	    ਜੇ (mch_rmdir(ਘਾਤਾਂਕ) != 0)
		ਨਤੀਜਾ = -1;
	}
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ = -1;
	vim_free(ਘਾਤਾਂਕ);
    }
    ਨਹੀਂਤਾਂ
	ਨਤੀਜਾ = mch_remove(name) == 0 ? 0 : -1;

    ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(TEMPDIRNAMES)
ਸਥਿਰ ਲੰਮਾ	ਆਰਜ਼ੀ_ਗਿਣਤੀ = 0;		// Temp filename counter.

# ਜੇ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   ਸਥਿਰ ਖਾਲੀ
vim_opentempdir(ਖਾਲੀ)
{
    DIR *dp = ਨਿਰਾ;

    ਜੇ (vim_tempdir_dp != ਨਿਰਾ)
	ਵਾਪਸ;

    dp = opendir((ਅਚਲ ਅੱਖਰ*)vim_tempdir);
    ਜੇ (dp == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   ਸਥਿਰ ਖਾਲੀ
vim_closetempdir(ਖਾਲੀ)
{
    ਜੇ (vim_tempdir_dp == ਨਿਰਾ)
	ਵਾਪਸ;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = ਨਿਰਾ;
}

/*
 * Return true if the temp directory we created is gone.
 */
    ਸਥਿਰ bool
vim_tempdir_gone(ਖਾਲੀ)
{
    stat_T	st;

    ਜੇ (vim_tempdir_dp == ਨਿਰਾ)
	ਵਾਪਸ false;
    ਵਾਪਸ fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    ਖਾਲੀ
vim_deltempdir(ਖਾਲੀ)
{
    ਜੇ (vim_tempdir == ਨਿਰਾ)
	ਵਾਪਸ;

# ਜੇ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    ਸਥਿਰ ਖਾਲੀ
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (vim_ਭਰਿਆ_Name(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    ਜੇ (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += ਆਕਾਰ(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# ਜੇ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_opentempdir();
# endif
    vim_free(buf);
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ	    extra_char ਅਣਵਰਤਿਆ,  // char to use in the name instead of '?'
    ਪੂਰਨ	    keep ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#ਜਾਂਜੇ defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#ਨਹੀਂਤਾਂ
    char_u	itmp[TEMPNAMELEN];
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TEMPDIRNAMES
    ਸਥਿਰ ਅੱਖਰ	*(tempdirs[]) = {TEMPDIRNAMES};
    ਪੂਰਨ		i;
# ifndef EEXIST
    stat_T	st;
# endif

# ਜੇ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    ਜੇ (vim_tempdir != ਨਿਰਾ && vim_tempdir_gone())
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
    ਜੇ (vim_tempdir == ਨਿਰਾ)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    ਲੰਮਾ	nr;
	    ਲੰਮਾ	off;
# endif
	    size_t	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    ਜੇ (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		ਜੇ (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += ਆਕਾਰ(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  ਜੇ defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    ਜੇ (mkdtemp((ਅੱਖਰ *)itmp) != ਨਿਰਾ)
			vim_settempdir(itmp);
#  ਜੇ defined(UNIX) || defined(VMS)
		    (ਖਾਲੀ)umask(umask_save);
#  endif
		}
# ਨਹੀਂਤਾਂ
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (ਲੰਮਾ)ਸਮਾਂ(ਨਿਰਾ)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		ਲਈ (off = 0; off < 10000L; ++off)
		{
		    ਪੂਰਨ		r;
#  ਜੇ defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((ਅੱਖਰ *)itmp + itmplen, ਆਕਾਰ(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    ਜੇ (mch_stat((ਅੱਖਰ *)itmp, &st) >= 0)
			ਜਾਰੀ;
#  endif
#  ਜੇ defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  ਜੇ defined(UNIX) || defined(VMS)
		    (ਖਾਲੀ)umask(umask_save);
#  endif
		    ਜੇ (r == 0)
		    {
			vim_settempdir(itmp);
			ਰੋਕੋ;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    ਜੇ (errno != EEXIST)
#  endif
			ਰੋਕੋ;
		}
# endif // HAVE_MKDTEMP
		ਜੇ (vim_tempdir != ਨਿਰਾ)
		    ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (vim_tempdir != ਨਿਰਾ)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	ਪੂਰਨ itmplen = vim_snprintf((ਅੱਖਰ *)itmp, ਆਕਾਰ(itmp), "%s%ld", vim_tempdir, ਆਰਜ਼ੀ_ਗਿਣਤੀ++);
	ਵਾਪਸ vim_strnsave(itmp, (size_t)itmplen);
    }

    ਵਾਪਸ ਨਿਰਾ;

#ਨਹੀਂਤਾਂ // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wsz_ਆਰਜ਼ੀ_File[MAX_ਰਸਤਾ_2 + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    ਲੰਮਾ	i;

    wcscpy(itmp, L"");
    ਜੇ (Get_ਆਰਜ਼ੀ_ਰਸਤਾ_W(MAX_ਰਸਤਾ_2, wsz_ਆਰਜ਼ੀ_File) == 0)
    {
	wsz_ਆਰਜ਼ੀ_File[0] = L'.';	// GetTempPathW() failed, use current dir
	wsz_ਆਰਜ਼ੀ_File[1] = L'\\';
	wsz_ਆਰਜ਼ੀ_File[2] = NUL;
    }
    wcscpy(buf4, L"VIM");

    // randomize the name to avoid collisions
    i = mch_get_pid() + extra_char;
    buf4[1] = chartab[i % 36];
    buf4[2] = chartab[(i / 36) % 36];
    ਜੇ (Get_ਆਰਜ਼ੀ_File_Name_W(wsz_ਆਰਜ਼ੀ_File, buf4, 0, itmp) == 0)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(ਖਾਲੀ)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_ਮੰਜ਼ਿਲ_enc(itmp, ਨਿਰਾ);
    ਜੇ (retval == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    shname = gettail(p_sh);
    ਜੇ ((*p_shcf == '-' && !(ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)shname, "powershell") != ਨਿਰਾ
			     || ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)shname, "pwsh") != ਨਿਰਾ ))
								    || p_ssl)
	ਲਈ (p = retval; *p; ++p)
	    ਜੇ (*p == '\\')
		*p = '/';
    ਵਾਪਸ retval;

# ਨਹੀਂਤਾਂ // MSWIN
    ਪੂਰਨ		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((ਅੱਖਰ *)itmp);
    ਜੇ (p == ਨਿਰਾ || *p == NUL)
	ਵਾਪਸ ਨਿਰਾ;
#  ਨਹੀਂਤਾਂ

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((ਅੱਖਰ *)itmp, ਆਕਾਰ(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (ਅੱਖਰ *)itmp);
    ਜੇ (p != ਨਿਰਾ)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((ਅੱਖਰ *)itmp, ਆਕਾਰ(itmp), "%s.txt", p);
	ਮੁਕਤ(p);
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
#   ਨਹੀਂਤਾਂ
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    ਜੇ ((p = vim_strchr(itmp, '?')) != ਨਿਰਾ)
	*p = extra_char;
    ਜੇ (mktemp((ਅੱਖਰ *)itmp) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
#   endif
#  endif

    ਵਾਪਸ vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#ਜੇ_ਅੰਤ // TEMPDIRNAMES
}

#ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    ਖਾਲੀ
forward_slash(char_u *fname)
{
    char_u	*p;

    ਜੇ (ਰਸਤਾ_with_url(fname))
	ਵਾਪਸ;
    ਲਈ (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	ਜੇ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	ਨਹੀਂਤਾਂ ਜੇ (*p == '\\')
	    *p = '/';
}
#ਜੇ_ਅੰਤ

/*
 * Try matching a filename with a "pattern" ("prog" is NULL), or use the
 * precompiled regprog "prog" ("pattern" is NULL).  That avoids calling
 * vim_regcomp() often.
 * Used for autocommands and 'wildignore'.
 * Returns TRUE if there is a match, FALSE otherwise.
 */
    ਪੂਰਨ
ਮੇਲ_file_pat(
    char_u	*ਨਮੂਨਾ,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*ਪੂਛ,			// tail of path
    ਪੂਰਨ		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    ਪੂਰਨ		ਨਤੀਜਾ = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    ਜੇ (prog != ਨਿਰਾ)
	regmatch.regprog = *prog;
    ਨਹੀਂਤਾਂ
	regmatch.regprog = vim_regcomp(ਨਮੂਨਾ, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    ਜੇ (regmatch.regprog != ਨਿਰਾ
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != ਨਿਰਾ
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, ਪੂਛ, (colnr_T)0))))
	ਨਤੀਜਾ = TRUE;

    ਜੇ (prog != ਨਿਰਾ)
	*prog = regmatch.regprog;
    ਨਹੀਂਤਾਂ
	vim_regfree(regmatch.regprog);
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    ਪੂਰਨ
ਮੇਲ_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*ਪੂਛ;
    char_u	*regpat;
    ਅੱਖਰ	allow_dirs;
    ਪੂਰਨ		ਮੇਲ;
    char_u	*p;

    ਪੂਛ = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    ਜਦੋਂ (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_ਮੰਜ਼ਿਲ_reg_pat(buf, ਨਿਰਾ, &allow_dirs, FALSE);
	ਜੇ (regpat == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਮੇਲ = ਮੇਲ_file_pat(regpat, ਨਿਰਾ, ffname, sfname,
						       ਪੂਛ, (ਪੂਰਨ)allow_dirs);
	vim_free(regpat);
	ਜੇ (ਮੇਲ)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
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
file_pat_ਮੰਜ਼ਿਲ_reg_pat(
    char_u	*pat,
    char_u	*pat_ਸਿਰਾ,	// first char after pattern or NULL
    ਅੱਖਰ	*allow_dirs,	// Result passed back out in here
    ਪੂਰਨ		no_bslash ਅਣਵਰਤਿਆ) // Don't use a backward slash as pathsep
{
    ਪੂਰਨ		ਮਾਪ = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		nested = 0;
    ਪੂਰਨ		ਜੋੜੋ_dollar = TRUE;

    ਜੇ (allow_dirs != ਨਿਰਾ)
	*allow_dirs = FALSE;
    ਜੇ (pat_ਸਿਰਾ == ਨਿਰਾ)
	pat_ਸਿਰਾ = pat + STRLEN(pat);

    ਲਈ (p = pat; p < pat_ਸਿਰਾ; p++)
    {
	ਚੋਣ (*p)
	{
	    ਹਾਲਤ '*':
	    ਹਾਲਤ '.':
	    ਹਾਲਤ ',':
	    ਹਾਲਤ '{':
	    ਹਾਲਤ '}':
	    ਹਾਲਤ '~':
		ਮਾਪ += 2;	// extra backslash
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    ਹਾਲਤ '\\':
	    ਹਾਲਤ '/':
		ਮਾਪ += 4;	// could become "[\/]"
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    ਮੂਲ:
		ਮਾਪ++;
		ਜੇ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++ਮਾਪ;
		}
		ਰੋਕੋ;
	}
    }
    reg_pat = alloc(ਮਾਪ + 1);
    ਜੇ (reg_pat == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    i = 0;

    ਜੇ (pat[0] == '*')
	ਜਦੋਂ (pat[0] == '*' && pat < pat_ਸਿਰਾ - 1)
	    pat++;
    ਨਹੀਂਤਾਂ
	reg_pat[i++] = '^';
    endp = pat_ਸਿਰਾ - 1;
    ਜੇ (endp >= pat && *endp == '*')
    {
	ਜਦੋਂ (endp - pat > 0 && *endp == '*')
	    endp--;
	ਜੋੜੋ_dollar = FALSE;
    }
    ਲਈ (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	ਚੋਣ (*p)
	{
	    ਹਾਲਤ '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		ਜਦੋਂ (p[1] == '*')	// "**" matches like "*"
		    ++p;
		ਰੋਕੋ;
	    ਹਾਲਤ '.':
	    ਹਾਲਤ '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		ਰੋਕੋ;
	    ਹਾਲਤ '?':
		reg_pat[i++] = '.';
		ਰੋਕੋ;
	    ਹਾਲਤ '\\':
		ਜੇ (p[1] == NUL)
		    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
		ਜੇ (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    ਜੇ ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			ਜੇ (allow_dirs != ਨਿਰਾ)
			    *allow_dirs = TRUE;
			ਰੋਕੋ;
		    }
		}
#ਜੇ_ਅੰਤ
		// Undo escaping from ExpandEscape():
		// foo\?bar -> foo?bar
		// foo\%bar -> foo%bar
		// foo\,bar -> foo,bar
		// foo\ bar -> foo bar
		// Don't unescape \, * and others that are also special in a
		// regexp.
		// An escaped { must be unescaped since we use magic not
		// verymagic.  Use "\\\{n,m\}"" to get "\{n,m}".
		ਜੇ (*++p == '?'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
			&& no_bslash
#ਜੇ_ਅੰਤ
			)
		    reg_pat[i++] = '?';
		ਨਹੀਂਤਾਂ
		    ਜੇ (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    ਨਹੀਂਤਾਂ ਜੇ (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (allow_dirs != ਨਿਰਾ && vim_ispathsep(*p)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
				&& (!no_bslash || *p != '\\')
#ਜੇ_ਅੰਤ
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			ਜੇ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    ਹਾਲਤ '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		ਜੇ (allow_dirs != ਨਿਰਾ)
		    *allow_dirs = TRUE;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		ਰੋਕੋ;
	    ਹਾਲਤ '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		ਰੋਕੋ;
	    ਹਾਲਤ ',':
		ਜੇ (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		ਨਹੀਂਤਾਂ
		    reg_pat[i++] = ',';
		ਰੋਕੋ;
	    ਮੂਲ:
		ਜੇ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		ਨਹੀਂਤਾਂ ਜੇ (allow_dirs != ਨਿਰਾ && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		ਰੋਕੋ;
	}
    }
    ਜੇ (ਜੋੜੋ_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    ਜੇ (nested != 0)
    {
	ਜੇ (nested < 0)
	    emsg(_(e_missing_open_curly));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    ਵਾਪਸ reg_pat;
}

#ਜੇ defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    ਲੰਮਾ
read_eintr(ਪੂਰਨ fd, ਖਾਲੀ *buf, size_t bufsize)
{
    ਲੰਮਾ ret;

    ਲਈ (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	ਜੇ (ret >= 0 || errno != EINTR)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    ਲੰਮਾ
write_eintr(ਪੂਰਨ fd, ਖਾਲੀ *buf, size_t bufsize)
{
    ਲੰਮਾ    ret = 0;
    ਲੰਮਾ    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    ਜਦੋਂ (ret < (ਲੰਮਾ)bufsize)
    {
	wlen = vim_write(fd, (ਅੱਖਰ *)buf + ret, bufsize - ret);
	ਜੇ (wlen < 0)
	{
	    ਜੇ (errno != EINTR)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	    ret += wlen;
    }
    ਵਾਪਸ ret;
}
#ਜੇ_ਅੰਤ
