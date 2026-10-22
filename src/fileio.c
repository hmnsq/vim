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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#ಆದರೆ_ಅಂತ್ಯ

// Is there any system that doesn't have access()?
#ವ್ಯಾಖ್ಯಾನಿಸು USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#ವ್ಯಾಖ್ಯಾನಿಸು NONASCII_ಮುಖವಾಡ (((long_u)-1 / 0xFF) * 0x80)

#ಆದರೆ defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ char_u *ಮುಂದಿನ_fenc(char_u **pp, ಪೂರ್ಣಾಂಕ *alloced);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ char_u *readfile_charconvert(char_u *fname, char_u *fenc, ಪೂರ್ಣಾಂಕ *fdp);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
ಸ್ಥಿರ char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, ದೀರ್ಘ *sizep, off_T *filesizep, ಪೂರ್ಣಾಂಕ newfile, char_u *fname, ಪೂರ್ಣಾಂಕ *did_ask);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
ಸ್ಥಿರ char_u *check_for_bom(char_u *p, ದೀರ್ಘ ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ *lenp, ಪೂರ್ಣಾಂಕ flags);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ readdirex_ವಿಂಗಡಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		attr)
{
    ಪೂರ್ಣಾಂಕ		ಸಂದೇಶ_scroll_save;
    ಪೂರ್ಣಾಂಕ		prev_ಸಂದೇಶ_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು;
    size_t	len;

    ಆದರೆ (ಸಂದೇಶ_silent != 0)
	ಹಿಂತಿರುಗಿ;
    ಸಂದೇಶ_ಕೂಡಿಸು_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    ಆದರೆ (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    ಆದರೆ (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    ಸಂದೇಶ_scroll_save = ಸಂದೇಶ_scroll;
    ಆದರೆ (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	ಸಂದೇಶ_scroll = FALSE;
    ಆದರೆ (!ಸಂದೇಶ_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    ಸಂದೇಶ_ಆರಂಭ();
    ಆದರೆ (prev_ಸಂದೇಶ_ಲಂಬಸಾಲು != 0 && ಸಂದೇಶ_ಲಂಬಸಾಲು == 0)
	ಸಂದೇಶ_putchar('\r');  // overwrite any previous message.
    ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll_save;
    ಸಂದೇಶ_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    ಸಂದೇಶ_outtrans_attr(ಸಂದೇಶ_may_trunc(FALSE, IObuff), attr);
    ಸಂದೇಶ_clr_eos();
    ಹೊರಹೋಗು_flush();
    ಸಂದೇಶ_scrolled_ign = FALSE;
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
    ಪೂರ್ಣಾಂಕ
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	ಸಾಲುಗಳು_ಗಮ್ಯ_skip,
    linenr_T	ಸಾಲುಗಳು_ಗಮ್ಯ_read,
    exarg_T	*eap,			// can be NULL!
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಪೂರ್ಣಾಂಕ		retval = FAIL;	// jump to "theend" instead of returning
    ಪೂರ್ಣಾಂಕ		fd = 0;
    ಪೂರ್ಣಾಂಕ		newfile = (flags & READ_NEW);
    ಪೂರ್ಣಾಂಕ		check_readonly;
    ಪೂರ್ಣಾಂಕ		filtering = (flags & READ_FILTER);
    ಪೂರ್ಣಾಂಕ		read_stdin = (flags & READ_STDIN);
    ಪೂರ್ಣಾಂಕ		read_ಬಫರ್ = (flags & READ_ಬಫರ್);
    ಪೂರ್ಣಾಂಕ		read_fifo = (flags & READ_FIFO);
    ಪೂರ್ಣಾಂಕ		ಗಣ_options = newfile || read_ಬಫರ್
					   || (eap != ಖಾಲಿ && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_ಲಂಬಸಾಲು = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = ಖಾಲಿ;		// pointer into read buffer
    char_u	*ಬಫರ್ = ಖಾಲಿ;		// read buffer
    char_u	*new_ಬಫರ್ = ಖಾಲಿ;	// init to shut up gcc
    char_u	*ಸಾಲು_ಆರಂಭ = ಖಾಲಿ;	// init to shut up gcc
    ಪೂರ್ಣಾಂಕ		wasempty;		// buffer was empty before reading
    colnr_T	len;
    ದೀರ್ಘ	ಗಾತ್ರ_2 = 0;
    char_u	*p;
    off_T	filesize = 0;
    ಪೂರ್ಣಾಂಕ		skip_read = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_ಎಣಿಕೆ = 0;     // counter
    char_u	*cryptkey = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		did_ask_for_ಕೀಲಿ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    ಪೂರ್ಣಾಂಕ		read_undo_file = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ವಿಭಜಿಸು = 0;		// number of split lines
#ವ್ಯಾಖ್ಯಾನಿಸು UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    ಪೂರ್ಣಾಂಕ		error = FALSE;		// errors encountered
    ಪೂರ್ಣಾಂಕ		ff_error = EOL_UNKNOWN; // file format with errors
    ದೀರ್ಘ	linerest = 0;		// remaining chars in line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಪೂರ್ಣಾಂಕ		perm = 0;
    ಪೂರ್ಣಾಂಕ		ಅದಲುಬದಲು_ಬಹುಲಕ = -1;		// protection bits for swap file
#ಇಲ್ಲದಿದ್ದರೆ
    ಪೂರ್ಣಾಂಕ		perm;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		fileformat = 0;		// end-of-line format
    ಪೂರ್ಣಾಂಕ		keep_fileformat = FALSE;
    stat_T	st;
    ಪೂರ್ಣಾಂಕ		file_readonly;
    linenr_T	skip_ಎಣಿಕೆ = 0;
    linenr_T	read_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		ಸಂದೇಶ_save = ಸಂದೇಶ_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    ಪೂರ್ಣಾಂಕ		try_mac;
    ಪೂರ್ಣಾಂಕ		try_dos;
    ಪೂರ್ಣಾಂಕ		try_unix;
    ಪೂರ್ಣಾಂಕ		file_rewind = FALSE;
    ಪೂರ್ಣಾಂಕ		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    ಪೂರ್ಣಾಂಕ		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    ಪೂರ್ಣಾಂಕ		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = ಖಾಲಿ;	// name of 'charconvert' output file
    ಪೂರ್ಣಾಂಕ		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    ಪೂರ್ಣಾಂಕ		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_ಮುಂದಿನ = ಖಾಲಿ;	// next item in 'fencs' or NULL
    ಪೂರ್ಣಾಂಕ		advance_fenc = FALSE;
    ದೀರ್ಘ	real_ಗಾತ್ರ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		converted = FALSE;	// TRUE if conversion done
    ಪೂರ್ಣಾಂಕ		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    ಪೂರ್ಣಾಂಕ		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_ಆರಂಭ;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    ಪೂರ್ಣಾಂಕ		using_b_ffname;
    ಪೂರ್ಣಾಂಕ		using_b_fname;
    ಸ್ಥಿರ ಅಕ್ಷರ *ಸಂದೇಶ_is_a_directory = N_("is a directory");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಪೂರ್ಣಾಂಕ		eof = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
    ಪೂರ್ಣಾಂಕ		may_need_lseek = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    size_t	fnamelen = 0;

    curbuf->b_au_did_filetype = false; // reset before triggering any autocommands

    curbuf->b_no_eol_lnum = 0;	// in case it was set by the previous read

    /*
     * If there is no file name yet, use the one for the read file.
     * BF_NOTEDITED is set to reflect this.
     * Don't do this for a read from a filter.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    ಆದರೆ (curbuf->b_ffname == ಖಾಲಿ
	    && !filtering
	    && fname != ಖಾಲಿ
	    && vim_strchr(p_cpo, CPO_FNAMER) != ಖಾಲಿ
	    && !(flags & READ_DUMMY))
    {
	ಆದರೆ (ಗಣ_rw_fname(fname, sfname) == FAIL)
	    ಹೋಗು theend;
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
    ಆದರೆ (sfname == ಖಾಲಿ)
	sfname = fname;
#ಆದರೆ defined(UNIX)
    fname = sfname;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    ಆದರೆ (!filtering && !read_stdin && !read_ಬಫರ್)
    {
	orig_ಆರಂಭ = curbuf->b_op_ಆರಂಭ;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_ಆರಂಭ.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = 0;

	ಆದರೆ (newfile)
	{
	    ಆದರೆ (apply_autocmds_exarg(EVENT_BUFREADCMD, ಖಾಲಿ, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಆದರೆ (aborting())
		    retval = FAIL;
#ಆದರೆ_ಅಂತ್ಯ
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		ಆದರೆ (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		ಹೋಗು theend;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, ಖಾಲಿ, eap))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#ಇಲ್ಲದಿದ್ದರೆ
	    retval = OK;
#ಆದರೆ_ಅಂತ್ಯ
	    ಹೋಗು theend;
	}

	curbuf->b_op_ಆರಂಭ = orig_ಆರಂಭ;

	ಆದರೆ (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    ಹೋಗು theend;
	}
    }

    ಆದರೆ ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	ಸಂದೇಶ_scroll = FALSE;	// overwrite previous file message
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_scroll = TRUE;	// don't overwrite previous file message

    ಆದರೆ (fname != ಖಾಲಿ && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	ಆದರೆ (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    ಸಂದೇಶ_ಕೊನೆ();
	    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
	    ಹೋಗು theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	ಆದರೆ (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(ಸಂದೇಶ_is_a_directory), 0);
	    ಸಂದೇಶ_ಕೊನೆ();
	    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
	    retval = NOTDONE;
	    ಹೋಗು theend;
	}
    }
#ಆದರೆ defined(UNIX) || defined(VMS)
    ಆದರೆ (!read_stdin && fname != ಖಾಲಿ)
	perm = mch_getperm(fname);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!read_stdin && !read_ಬಫರ್ && !read_fifo)
    {
#ಆದರೆ defined(UNIX) || defined(VMS)
	ಆದರೆ (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
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
	    ಆದರೆ (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(ಸಂದೇಶ_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    ಸಂದೇಶ_ಕೊನೆ();
	    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
	    ಹೋಗು theend;
	}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	ಆದರೆ (!p_odev && mch_nodetype(fname) == ಗಂಟು_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    ಸಂದೇಶ_ಕೊನೆ();
	    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
	    ಹೋಗು theend;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Set default or forced 'fileformat' and 'binary'.
    ಗಣ_file_options(ಗಣ_options, eap);

    /*
     * When opening a new file we take the readonly flag from the file.
     * Default is r/w, can be set to r/o below.
     * Don't reset it when in readonly mode
     * Only set/reset b_p_ro when BF_CHECK_RO is set.
     */
    check_readonly = (newfile && (curbuf->b_flags & BF_CHECK_RO));
    ಆದರೆ (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    ಆದರೆ (newfile && !read_stdin && !read_ಬಫರ್ && !read_fifo)
    {
	// Remember time of file.
	ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) >= 0)
	{
	    buf_store_ಸಮಯ(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	    filesize_disk = st.st_size;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
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
	    ಅದಲುಬದಲು_ಬಹುಲಕ = (st.st_ಬಹುಲಕ & 0644) | 0600;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	    curbuf->b_fab_rfm = st.st_fab_rfm;
	    curbuf->b_fab_rat = st.st_fab_rat;
	    curbuf->b_fab_mrs = st.st_fab_mrs;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    curbuf->b_mtime = 0;
	    curbuf->b_mtime_ns = 0;
	    curbuf->b_mtime_read = 0;
	    curbuf->b_mtime_read_ns = 0;
	    curbuf->b_orig_ಗಾತ್ರ = 0;
	    curbuf->b_orig_ಬಹುಲಕ = 0;
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
    ಆದರೆ (read_stdin)
    {
#ಆದರೆ defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_ದ್ವಿಮಾನ);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!read_ಬಫರ್)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MCH_ACCESS
	ಆದರೆ (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((ಅಕ್ಷರ *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0);
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (!newfile
		|| readonlymode
		|| (fd = mch_open((ಅಕ್ಷರ *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (fd < 0)			    // cannot open at all
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX
	ಪೂರ್ಣಾಂಕ	isdir_f;
#ಆದರೆ_ಅಂತ್ಯ
	ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	ಆದರೆ (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(ಸಂದೇಶ_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (newfile)
	    {
		ಆದರೆ (perm < 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENOENT
			&& errno == ENOENT
#ಆದರೆ_ಅಂತ್ಯ
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
		    ಆದರೆ (!bt_dontwrite(curbuf))
		    {
			check_need_ಅದಲುಬದಲು(newfile);
			// SwapExists autocommand may mess things up
			ಆದರೆ (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_ಬದಲಾಯಿತು_ಬಫರ್_or_ಬಫರ್_name));
			    ಹೋಗು theend;
			}
		    }
		    ಆದರೆ (dir_ದ_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_ಸಂದೇಶ(), 0);
		    ಇಲ್ಲದಿದ್ದರೆ
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#ಆದರೆ_ಅಂತ್ಯ
		    // Set forced 'fileencoding'.
		    ಆದರೆ (eap != ಖಾಲಿ)
			ಗಣ_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#ಆದರೆ defined(FEAT_EVAL)
		    ಆದರೆ (!aborting())   // autocmds may abort script processing
#ಆದರೆ_ಅಂತ್ಯ
			retval = OK;	    // a new file is not an error
		    ಹೋಗು theend;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    filemess(curbuf, sfname, (char_u *)(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EFBIG
			    (errno == EFBIG) ? _("[File too big]") :
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EOVERFLOW
			    (errno == EOVERFLOW) ? _("[File too big]") :
#ಆದರೆ_ಅಂತ್ಯ
						_("[Permission Denied]")), 0);
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
		}
	    }

	ಹೋಗು theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    ಆದರೆ ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    ಆದರೆ (ಗಣ_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	ಆದರೆ (!read_ಬಫರ್)
	{
	    curbuf->b_p_eof = FALSE;
	    curbuf->b_ಆರಂಭ_eof = FALSE;
	    curbuf->b_p_eol = TRUE;
	    curbuf->b_ಆರಂಭ_eol = TRUE;
	}
	curbuf->b_p_bomb = FALSE;
	curbuf->b_ಆರಂಭ_bomb = FALSE;
    }

    // Create a swap file now, so that other Vims are warned that we are
    // editing this file.
    // Don't do this for a "nofile" or "nowrite" buffer type.
    ಆದರೆ (!bt_dontwrite(curbuf))
    {
	check_need_ಅದಲುಬದಲು(newfile);
	ಆದರೆ (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_ಬದಲಾಯಿತು_ಬಫರ್_or_ಬಫರ್_name));
	    ಆದರೆ (!read_ಬಫರ್)
		close(fd);
	    ಹೋಗು theend;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	// Set swap file protection bits after creating it.
	ಆದರೆ (ಅದಲುಬದಲು_ಬಹುಲಕ > 0 && curbuf->b_ml.ml_mfp != ಖಾಲಿ
			  && curbuf->b_ml.ml_mfp->mf_fname != ಖಾಲಿ)
	{
	    char_u *ಅದಲುಬದಲು_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    ಆದರೆ ((ಅದಲುಬದಲು_ಬಹುಲಕ & 044) == 040)
	    {
		stat_T	ಅದಲುಬದಲು_st;

		ಆದರೆ (mch_stat((ಅಕ್ಷರ *)ಅದಲುಬದಲು_fname, &ಅದಲುಬದಲು_st) >= 0
			&& st.st_gid != ಅದಲುಬದಲು_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    ಅದಲುಬದಲು_ಬಹುಲಕ &= 0600;
	    }

	    (ಶೂನ್ಯ)mch_setperm(ಅದಲುಬದಲು_fname, (ದೀರ್ಘ)ಅದಲುಬದಲು_ಬಹುಲಕ);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    ಆದರೆ (ಅದಲುಬದಲು_exists_action == SEA_QUIT)
    {
	ಆದರೆ (!read_ಬಫರ್ && !read_stdin)
	    close(fd);
	ಹೋಗು theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_ಆರಂಭ = curbuf->b_op_ಆರಂಭ;
    curbuf->b_op_ಆರಂಭ.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != ಖಾಲಿ);
    try_dos = (vim_strchr(p_ffs, 'd') != ಖಾಲಿ);
    try_unix = (vim_strchr(p_ffs, 'x') != ಖಾಲಿ);

    ಆದರೆ (!read_ಬಫರ್)
    {
	ಪೂರ್ಣಾಂಕ	m = ಸಂದೇಶ_scroll;
	ಪೂರ್ಣಾಂಕ	n = ಸಂದೇಶ_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	ಆದರೆ (!read_stdin)
	    close(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	ಸಂದೇಶ_scroll = TRUE;
	ಆದರೆ (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, ಖಾಲಿ, sfname,
							  FALSE, curbuf, eap);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, ಖಾಲಿ, sfname,
							  FALSE, curbuf, eap);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, ಖಾಲಿ, sfname,
							  FALSE, curbuf, eap);
	ಇಲ್ಲದಿದ್ದರೆ
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, ಖಾಲಿ, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != ಖಾಲಿ);
	try_dos = (vim_strchr(p_ffs, 'd') != ಖಾಲಿ);
	try_unix = (vim_strchr(p_ffs, 'x') != ಖಾಲಿ);
	curbuf->b_op_ಆರಂಭ = orig_ಆರಂಭ;

	ಆದರೆ (ಸಂದೇಶ_scrolled == n)
	    ಸಂದೇಶ_scroll = m;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    ಹೋಗು theend;
	}
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	ಆದರೆ (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
	    ಆದರೆ (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_readpre_autocommands_must_not_change_current_ಬಫರ್));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    ಹೋಗು theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು);

    ಆದರೆ (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	ಆದರೆ (read_stdin)
	{
	    ಆದರೆ (!is_not_a_ಪದ())
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ALWAYS_USE_GUI
# ifdef VIMDLL
		ಆದರೆ (!gui.ಒಳ_use)
# endif
		    mch_ಸಂದೇಶ(_("Vim: Reading from stdin...\n"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		ಆದರೆ (gui.ಒಳ_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			gui_write(p, (ಪೂರ್ಣಾಂಕ)plen);
			vim_free(p);
		    }
		}
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!read_ಬಫರ್)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    ಸಂದೇಶ_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // "++bad=" argument.
    ಆದರೆ (eap != ಖಾಲಿ && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	ಆದರೆ (ಗಣ_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    ಆದರೆ (eap != ಖಾಲಿ && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_help)
    {
	char_u	    firstline[80];
	ಪೂರ್ಣಾಂಕ	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	ಆದರೆ (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    ಆದರೆ (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, SEEK_SET);
		ಗಾಗಿ (p = firstline; p < firstline + len; ++p)
		    ಆದರೆ (*p >= 0x80)
		    {
			c = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
	    }
	}

	ಆದರೆ (c)
	{
	    fenc_ಮುಂದಿನ = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    ಆದರೆ (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fenc_ಮುಂದಿನ = p_fencs;		// try items in 'fileencodings'
	fenc = ಮುಂದಿನ_fenc(&fenc_ಮುಂದಿನ, &fenc_alloced);
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

    ಆದರೆ (file_rewind)
    {
	ಆದರೆ (read_ಬಫರ್)
	{
	    read_buf_lnum = 1;
	    read_buf_ಲಂಬಸಾಲು = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    ಹೋಗು failed;
	}
	// Delete the previously read lines.
	ಆಗ (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	ಆದರೆ (ಗಣ_options)
	{
	    curbuf->b_p_bomb = FALSE;
	    curbuf->b_ಆರಂಭ_bomb = FALSE;
	}
	conv_error = 0;
    }

    /*
     * When retrying with another "fenc" and the first time "fileformat"
     * will be reset.
     */
    ಆದರೆ (keep_fileformat)
	keep_fileformat = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (eap != ಖಾಲಿ && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	ಇಲ್ಲದಿದ್ದರೆ
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
    ಆದರೆ (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	ಆದರೆ (eap != ಖಾಲಿ && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    ಆದರೆ (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (fenc_alloced)
		vim_free(fenc);
	    ಆದರೆ (fenc_ಮುಂದಿನ != ಖಾಲಿ)
	    {
		fenc = ಮುಂದಿನ_fenc(&fenc_ಮುಂದಿನ, &fenc_alloced);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	ಆದರೆ (tmpname != ಖಾಲಿ)
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
    ಆದರೆ (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	ಆದರೆ (STRCMP(fenc, ENC_UCSBOM) == 0)
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
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	ಆದರೆ (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	ಆದರೆ (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	ಆದರೆ (fio_flags == 0
# ifdef FEAT_EVAL
		&& !did_iconv
# endif
		)
	    iconv_fd = (iconv_t)my_iconv_open(
				  enc_utf8 ? (char_u *)"utf-8" : p_enc, fenc);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	/*
	 * Use the 'charconvert' expression when conversion is required
	 * and we can't do it internally or with iconv().
	 */
	ಆದರೆ (fio_flags == 0 && !read_stdin && !read_ಬಫರ್ && *p_ccv != NUL
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
	    ಆದರೆ (tmpname == ಖಾಲಿ)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		ಆದರೆ (tmpname == ಖಾಲಿ)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    ಆದರೆ (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			ಹೋಗು failed;
		    }
		    ಹೋಗು retry;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ಆದರೆ (fio_flags == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
		    && iconv_fd == (iconv_t)-1
#ಆದರೆ_ಅಂತ್ಯ
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		ಹೋಗು retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    ಆದರೆ (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	filesize_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
	skip_ಎಣಿಕೆ = ಸಾಲುಗಳು_ಗಮ್ಯ_skip;
	read_ಎಣಿಕೆ = ಸಾಲುಗಳು_ಗಮ್ಯ_read;
	conv_restlen = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != ಖಾಲಿ
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_ಬಫರ್);
	ಆದರೆ (read_undo_file)
	    sha256_ಆರಂಭ(&sha_ctx);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	ಆದರೆ (curbuf->b_cryptstate != ಖಾಲಿ)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = ಖಾಲಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆಗ (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	ಆದರೆ (!skip_read)
	{
#ಆದರೆ defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		ಗಾತ್ರ_2 = SSIZE_MAX;		    // use max I/O size, 52K
#ಇಲ್ಲದಿದ್ದರೆ
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		ಗಾತ್ರ_2 = 0x10000L + linerest;
		ಆದರೆ (ಗಾತ್ರ_2 > 0x100000L)
		    ಗಾತ್ರ_2 = 0x100000L;
#ಆದರೆ_ಅಂತ್ಯ
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	ಆದರೆ (ಗಾತ್ರ_2 < 0 || ಗಾತ್ರ_2 + linerest + 1 < 0 || linerest >= MAXCOL - ಗಾತ್ರ_2)
	{
	    ++ವಿಭಜಿಸು;
	    *ptr = NL;		    // split line by inserting a NL
	    ಗಾತ್ರ_2 = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (!skip_read)
	    {
		ಗಾಗಿ ( ; ಗಾತ್ರ_2 >= 10; ಗಾತ್ರ_2 = (ದೀರ್ಘ)((long_u)ಗಾತ್ರ_2 >> 1))
		{
		    ಆದರೆ ((new_ಬಫರ್ = lalloc(ಗಾತ್ರ_2 + linerest + 1,
							      FALSE)) != ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (new_ಬಫರ್ == ಖಾಲಿ)
		{
		    do_outofmem_ಸಂದೇಶ((long_u)(ಗಾತ್ರ_2 * 2 + linerest + 1));
		    error = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_ಬಫರ್, ptr - linerest, (size_t)linerest);
		vim_free(ಬಫರ್);
		ಬಫರ್ = new_ಬಫರ್;
		ptr = ಬಫರ್ + linerest;
		ಸಾಲು_ಆರಂಭ = ಬಫರ್;

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
		real_ಗಾತ್ರ = (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ_2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
		ಆದರೆ (iconv_fd != (iconv_t)-1)
		    ಗಾತ್ರ_2 = ಗಾತ್ರ_2 / ICONV_MULT;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (fio_flags & FIO_LATIN1)
		    ಗಾತ್ರ_2 = ಗಾತ್ರ_2 / 2;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    ಗಾತ್ರ_2 = (ಗಾತ್ರ_2 * 2 / 3) & ~1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fio_flags & FIO_UCS4)
		    ಗಾತ್ರ_2 = (ಗಾತ್ರ_2 * 2 / 3) & ~3;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fio_flags == FIO_UCSBOM)
		    ಗಾತ್ರ_2 = ಗಾತ್ರ_2 / ICONV_MULT;	// worst case
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fio_flags & FIO_CODEPAGE)
		    ಗಾತ್ರ_2 = ಗಾತ್ರ_2 / ICONV_MULT;	// also worst case
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_CONVERT
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fio_flags & FIO_MACROMAN)
		    ಗಾತ್ರ_2 = ಗಾತ್ರ_2 / ICONV_MULT;	// also worst case
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    ಗಾತ್ರ_2 -= conv_restlen;
		}

		ಆದರೆ (read_ಬಫರ್)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    ಆದರೆ (read_buf_lnum > from)
			ಗಾತ್ರ_2 = 0;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಪೂರ್ಣಾಂಕ	n, ni;
			ದೀರ್ಘ	tlen;

			tlen = 0;
			ಗಾಗಿ (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_ಲಂಬಸಾಲು;
			    n = ml_get_len(read_buf_lnum) - read_buf_ಲಂಬಸಾಲು;
			    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)tlen + n + 1 > ಗಾತ್ರ_2)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (ಪೂರ್ಣಾಂಕ)(ಗಾತ್ರ_2 - tlen);
				ಗಾಗಿ (ni = 0; ni < n; ++ni)
				{
				    ಆದರೆ (p[ni] == NL)
					ptr[tlen++] = NUL;
				    ಇಲ್ಲದಿದ್ದರೆ
					ptr[tlen++] = p[ni];
				}
				read_buf_ಲಂಬಸಾಲು += n;
				ನಿಲ್ಲಿಸು;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    ಗಾಗಿ (ni = 0; ni < n; ++ni)
			    {
				ಆದರೆ (p[ni] == NL)
				    ptr[tlen++] = NUL;
				ಇಲ್ಲದಿದ್ದರೆ
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_ಲಂಬಸಾಲು = 0;
			    ಆದರೆ (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				ಆದರೆ (!curbuf->b_p_eol)
				    --tlen;
				ಗಾತ್ರ_2 = tlen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
				eof = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
			    }
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    /*
		     * Read bytes from the file.
		     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
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
		    ಆದರೆ (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			ಗಾತ್ರ_2 = WRITEBUFSIZE + crypt_get_max_ಶೀರ್ಷಿಕೆ_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filesize > 0 && (curbuf->b_cryptstate != ಖಾಲಿ
				&& crypt_ವಿಧಾನ_is_sodium(
					     curbuf->b_cryptstate->ವಿಧಾನ_nr)))
		    {
			ಗಾತ್ರ_2 = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			ಆದರೆ (curbuf->b_cryptstate->ವಿಧಾನ_nr == CRYPT_M_SOD
						     && !eof && may_need_lseek)
			{
			    lseek(fd, crypt_get_ಶೀರ್ಷಿಕೆ_len(
					       curbuf->b_cryptstate->ವಿಧಾನ_nr)
				       - crypt_get_max_ಶೀರ್ಷಿಕೆ_len(), SEEK_CUR);
			    may_need_lseek = FALSE;
			}
		    }
#ಆದರೆ_ಅಂತ್ಯ
		    ದೀರ್ಘ read_ಗಾತ್ರ = ಗಾತ್ರ_2;
		    ಗಾತ್ರ_2 = read_eintr(fd, ptr, read_ಗಾತ್ರ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
		    // Did we reach end of file?
		    filesize_ಎಣಿಕೆ += ಗಾತ್ರ_2;
		    eof = (ಗಾತ್ರ_2 < read_ಗಾತ್ರ || filesize_ಎಣಿಕೆ == filesize_disk);
#ಆದರೆ_ಅಂತ್ಯ
		}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		ಆದರೆ (filesize == 0 && ಗಾತ್ರ_2 > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &ಗಾತ್ರ_2,
						  &filesize, newfile, sfname,
						  &did_ask_for_ಕೀಲಿ);
# ಆದರೆ defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    ಆದರೆ (curbuf->b_cryptstate != ಖಾಲಿ
				 && !crypt_ಕೆಲಸಮಾಡುತ್ತದೆ_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		ಆದರೆ (cryptkey != ಖಾಲಿ && curbuf->b_cryptstate != ಖಾಲಿ
								   && ಗಾತ್ರ_2 > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    ಆದರೆ (crypt_ಕೆಲಸಮಾಡುತ್ತದೆ_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    ಗಾತ್ರ_2, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			char_u	*newptr = ಖಾಲಿ;
			ಪೂರ್ಣಾಂಕ	decrypted_ಗಾತ್ರ;

			decrypted_ಗಾತ್ರ = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, ಗಾತ್ರ_2,
								 &newptr, eof);

			ಆದರೆ (decrypted_ಗಾತ್ರ < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    ನಿಲ್ಲಿಸು;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			ಆದರೆ (decrypted_ಗಾತ್ರ == 0)
			    ಮುಂದುವರಿ;

			ಆದರೆ (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_ಬಫರ್ = newptr;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    long_u	new_ಗಾತ್ರ;

			    // Need new buffer to add bytes carried over.
			    new_ಗಾತ್ರ = (long_u)(decrypted_ಗಾತ್ರ + linerest + 1);
			    new_ಬಫರ್ = lalloc(new_ಗಾತ್ರ, FALSE);
			    ಆದರೆ (new_ಬಫರ್ == ಖಾಲಿ)
			    {
				do_outofmem_ಸಂದೇಶ(new_ಗಾತ್ರ);
				error = TRUE;
				ನಿಲ್ಲಿಸು;
			    }

			    mch_memmove(new_ಬಫರ್, ಬಫರ್, linerest + conv_restlen);
			    ಆದರೆ (newptr != ಖಾಲಿ)
				mch_memmove(new_ಬಫರ್ + linerest + conv_restlen,
					newptr, decrypted_ಗಾತ್ರ);
			    vim_free(newptr);
			}

			ಆದರೆ (new_ಬಫರ್ != ಖಾಲಿ)
			{
			    vim_free(ಬಫರ್);
			    ಬಫರ್ = new_ಬಫರ್;
			    new_ಬಫರ್ = ಖಾಲಿ;
			    ಸಾಲು_ಆರಂಭ = ಬಫರ್;
			    ptr = ಬಫರ್ + linerest + conv_restlen;
			    real_ಗಾತ್ರ = ಗಾತ್ರ_2;
			}
			ಗಾತ್ರ_2 = decrypted_ಗಾತ್ರ;
		    }
# endif
		}
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ (ಗಾತ್ರ_2 <= 0)
		{
		    ಆದರೆ (ಗಾತ್ರ_2 < 0)		    // read error
			error = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			ಆದರೆ (fio_flags != 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
				|| iconv_fd != (iconv_t)-1
#ಆದರೆ_ಅಂತ್ಯ
			   )
			{
			    ಆದರೆ (can_retry)
				ಹೋಗು rewind_retry;
			    ಆದರೆ (conv_error == 0)
				conv_error = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
								- linecnt + 1;
			ಆದರೆ (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    ಆದರೆ (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
				    || iconv_fd != (iconv_t)-1
#ಆದರೆ_ಅಂತ್ಯ
			       ))
			    {
				ಆಗ (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
			    ಆದರೆ (iconv_fd != (iconv_t)-1)
			    {
				iconv_close(iconv_fd);
				iconv_fd = (iconv_t)-1;
			    }
#ಆದರೆ_ಅಂತ್ಯ
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
	    ಆದರೆ ((filesize == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
		   || (cryptkey != ಖಾಲಿ
			&& filesize == crypt_get_ಶೀರ್ಷಿಕೆ_len(
						 crypt_get_ವಿಧಾನ_nr(curbuf)))
#ಆದರೆ_ಅಂತ್ಯ
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == ಖಾಲಿ
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		ಪೂರ್ಣಾಂಕ	blen;

		// no BOM detection in a short file or in binary mode
		ಆದರೆ (ಗಾತ್ರ_2 < 2 || curbuf->b_p_bin)
		    ccname = ಖಾಲಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		    ccname = check_for_bom(ptr, ಗಾತ್ರ_2, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		ಆದರೆ (ccname != ಖಾಲಿ)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    ಗಾತ್ರ_2 -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)ಗಾತ್ರ_2);
		    ಆದರೆ (ಗಣ_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_ಆರಂಭ_bomb = TRUE;
		    }
		}

		ಆದರೆ (fio_flags == FIO_UCSBOM)
		{
		    ಆದರೆ (ccname == ಖಾಲಿ)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// BOM detected: set "fenc" and jump back
			ಆದರೆ (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    ಹೋಗು retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    ಗಾತ್ರ_2 += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    ಆದರೆ (ಗಾತ್ರ_2 <= 0)
		ನಿಲ್ಲಿಸು;


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
	    ಆದರೆ (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*fromp;
		ಅಕ್ಷರ		*ತುದಿ;
		size_t		from_ಗಾತ್ರ;
		size_t		ಗಮ್ಯ_ಗಾತ್ರ;

		fromp = (ಅಕ್ಷರ *)ptr;
		from_ಗಾತ್ರ = ಗಾತ್ರ_2;
		ptr += ಗಾತ್ರ_2;
		ತುದಿ = (ಅಕ್ಷರ *)ptr;
		ಗಮ್ಯ_ಗಾತ್ರ = real_ಗಾತ್ರ - ಗಾತ್ರ_2;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		ಆಗ ((iconv(iconv_fd, (ಶೂನ್ಯ *)&fromp, &from_ಗಾತ್ರ,
							       &ತುದಿ, &ಗಮ್ಯ_ಗಾತ್ರ)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_ಗಾತ್ರ > CONV_RESTLEN)
		{
		    ಆದರೆ (can_retry)
			ಹೋಗು rewind_retry;
		    ಆದರೆ (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)ತುದಿ);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_ಗಾತ್ರ;
		    ಆದರೆ (bad_char_behavior == BAD_KEEP)
		    {
			*ತುದಿ++ = *(fromp - 1);
			--ಗಮ್ಯ_ಗಾತ್ರ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bad_char_behavior != BAD_DROP)
		    {
			*ತುದಿ++ = bad_char_behavior;
			--ಗಮ್ಯ_ಗಾತ್ರ;
		    }
		}

		ಆದರೆ (from_ಗಾತ್ರ > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_ಗಾತ್ರ);
		    conv_restlen = (ಪೂರ್ಣಾಂಕ)from_ಗಾತ್ರ;
		}

		// move the linerest to before the converted characters
		ಸಾಲು_ಆರಂಭ = ptr - linerest;
		mch_memmove(ಸಾಲು_ಆರಂಭ, ಬಫರ್, (size_t)linerest);
		ಗಾತ್ರ_2 = (ದೀರ್ಘ)((char_u *)ತುದಿ - ptr);
	    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    ಆದರೆ (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		ಪೂರ್ಣಾಂಕ	ucs2len;
		ಪೂರ್ಣಾಂಕ	codepage = FIO_GET_CP(fio_flags);
		ಪೂರ್ಣಾಂಕ	bytelen;
		ಪೂರ್ಣಾಂಕ	ಸಿಕ್ಕಿತು_bad;
		ಅಕ್ಷರ	replstr[2];

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
		ಆದರೆ (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		ಇಲ್ಲದಿದ್ದರೆ
		    replstr[0] = '?';
		replstr[1] = NUL;

		/*
		 * Move the bytes to the end of the buffer, so that we have
		 * room to put the result at the start.
		 */
		src = ptr + real_ಗಾತ್ರ - ಗಾತ್ರ_2;
		mch_memmove(src, ptr, ಗಾತ್ರ_2);

		/*
		 * Do the conversion.
		 */
		dst = ptr;
		ಆಗ (ಗಾತ್ರ_2 > 0)
		{
		    ಸಿಕ್ಕಿತು_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    ಆದರೆ (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (ಪೂರ್ಣಾಂಕ)utf_ptr2len_len(src, ಗಾತ್ರ_2);
			ಆದರೆ (bytelen > ಗಾತ್ರ_2)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    ಆದರೆ (bytelen <= CONV_RESTLEN)
				ನಿಲ್ಲಿಸು;

			    // weird overlong byte sequence
			    bytelen = ಗಾತ್ರ_2;
			    ಸಿಕ್ಕಿತು_bad = TRUE;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಪೂರ್ಣಾಂಕ	    u8c = utf_ptr2char(src);

			    ಆದರೆ (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				ಸಿಕ್ಕಿತು_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			ಗಾಗಿ (bytelen = 1; bytelen <= ಗಾತ್ರ_2 && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = Multi_Byte_ಗಮ್ಯ_Wide_Char(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    ಆದರೆ (ucs2len > 0)
				ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    ಆದರೆ (ಗಾತ್ರ_2 == 1)
				ನಿಲ್ಲಿಸು;
			    ಸಿಕ್ಕಿತು_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    ಆದರೆ (!ಸಿಕ್ಕಿತು_bad)
		    {
			ಪೂರ್ಣಾಂಕ	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			ಆದರೆ (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    ಗಾಗಿ (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    BOOL	bad = FALSE;
			    ಪೂರ್ಣಾಂಕ		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = Wide_Char_ಗಮ್ಯ_Multi_Byte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (ಪೂರ್ಣಾಂಕ)(src - dst),
				    replstr, &bad);
			    ಆದರೆ (bad)
				ಸಿಕ್ಕಿತು_bad = TRUE;
			    ಇಲ್ಲದಿದ್ದರೆ
				dst += dstlen;
			}
		    }

		    ಆದರೆ (ಸಿಕ್ಕಿತು_bad)
		    {
			// Deal with bytes we can't convert.
			ಆದರೆ (can_retry)
			    ಹೋಗು rewind_retry;
			ಆದರೆ (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			ಆದರೆ (bad_char_behavior != BAD_DROP)
			{
			    ಆದರೆ (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    ಗಾತ್ರ_2 -= bytelen;
		}

		ಆದರೆ (ಗಾತ್ರ_2 > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, ಗಾತ್ರ_2);
		    conv_restlen = ಗಾತ್ರ_2;
		}

		// The new size is equal to how much "dst" was advanced.
		ಗಾತ್ರ_2 = (ದೀರ್ಘ)(dst - ptr);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_CONVERT
	    ಆದರೆ (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		ಆದರೆ (macroman2enc(ptr, &ಗಾತ್ರ_2, real_ಗಾತ್ರ) == FAIL)
		    ಹೋಗು rewind_retry;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (fio_flags != 0)
	    {
		ಪೂರ್ಣಾಂಕ	u8c;
		char_u	*dest;
		char_u	*ಬಾಲ = ಖಾಲಿ;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_ಗಾತ್ರ;
		ಆದರೆ (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + ಗಾತ್ರ_2;
		    ಆದರೆ (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			ಬಾಲ = ptr + ಗಾತ್ರ_2 - 1;
			ಆಗ (ಬಾಲ > ptr && (*ಬಾಲ & 0xc0) == 0x80)
			    --ಬಾಲ;
			ಆದರೆ (ಬಾಲ + utf_byte2len(*ಬಾಲ) <= ptr + ಗಾತ್ರ_2)
			    ಬಾಲ = ಖಾಲಿ;
			ಇಲ್ಲದಿದ್ದರೆ
			    p = ಬಾಲ;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (ಗಾತ್ರ_2 & ~1);
		    ಆದರೆ (ಗಾತ್ರ_2 & 1)
			ಬಾಲ = p;
		    ಆದರೆ ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			ಆದರೆ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			ಆದರೆ (u8c >= 0xd800 && u8c <= 0xdbff)
			    ಬಾಲ = p;
			ಇಲ್ಲದಿದ್ದರೆ
			    p += 2;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (ಗಾತ್ರ_2 & ~3);
		    ಆದರೆ (ಗಾತ್ರ_2 & 3)
			ಬಾಲ = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		ಆದರೆ (ಬಾಲ != ಖಾಲಿ)
		{
		    conv_restlen = (ಪೂರ್ಣಾಂಕ)((ptr + ಗಾತ್ರ_2) - ಬಾಲ);
		    mch_memmove(conv_rest, (char_u *)ಬಾಲ, conv_restlen);
		    ಗಾತ್ರ_2 -= conv_restlen;
		}


		ಆಗ (p > ptr)
		{
		    ಆದರೆ (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			ಆದರೆ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			ಆದರೆ ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    ಪೂರ್ಣಾಂಕ u16c;

			    ಆದರೆ (p == ptr)
			    {
				// Missing leading word.
				ಆದರೆ (can_retry)
				    ಹೋಗು rewind_retry;
				ಆದರೆ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				ಆದರೆ (bad_char_behavior == BAD_DROP)
				    ಮುಂದುವರಿ;
				ಆದರೆ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    ಆದರೆ (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    ಆದರೆ (u16c < 0xd800 || u16c > 0xdbff)
			    {
				ಆದರೆ (can_retry)
				    ಹೋಗು rewind_retry;
				ಆದರೆ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				ಆದರೆ (bad_char_behavior == BAD_DROP)
				    ಮುಂದುವರಿ;
				ಆದರೆ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fio_flags & FIO_UCS4)
		    {
			ಆದರೆ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (ಚಿಹ್ನೆರಹಿತ)*--p << 24;
			    u8c += (ಚಿಹ್ನೆರಹಿತ)*--p << 16;
			    u8c += (ಚಿಹ್ನೆರಹಿತ)*--p << 8;
			    u8c += *--p;
			}
			ಇಲ್ಲದಿದ್ದರೆ	// big endian
			{
			    u8c = *--p;
			    u8c += (ಚಿಹ್ನೆರಹಿತ)*--p << 8;
			    u8c += (ಚಿಹ್ನೆರಹಿತ)*--p << 16;
			    u8c += (ಚಿಹ್ನೆರಹಿತ)*--p << 24;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ    // UTF-8
		    {
			ಆದರೆ (*--p < 0x80)
			    u8c = *p;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    len = utf_ತಲೆ_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    ಆದರೆ (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				ಆದರೆ (can_retry)
				    ಹೋಗು rewind_retry;
				ಆದರೆ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				ಆದರೆ (bad_char_behavior == BAD_DROP)
				    ಮುಂದುವರಿ;
				ಆದರೆ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    ಆದರೆ (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(ಶೂನ್ಯ)utf_char2bytes(u8c, dest);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ		// produce Latin1
		    {
			--dest;
			ಆದರೆ (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    ಆದರೆ (can_retry)
				ಹೋಗು rewind_retry;
			    ಆದರೆ (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    ಆದರೆ (bad_char_behavior == BAD_DROP)
				++dest;
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap != ಖಾಲಿ && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    ಇಲ್ಲದಿದ್ದರೆ
				*dest = 0xBF;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		ಸಾಲು_ಆರಂಭ = dest - linerest;
		mch_memmove(ಸಾಲು_ಆರಂಭ, ಬಫರ್, (size_t)linerest);
		ಗಾತ್ರ_2 = (ದೀರ್ಘ)((ptr + real_ಗಾತ್ರ) - dest);
		ptr = dest;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8 && !curbuf->b_p_bin)
	    {
		ಪೂರ್ಣಾಂಕ  incomplete_ಬಾಲ = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		ಗಾಗಿ (p = ptr; ; )
		{
		    ಪೂರ್ಣಾಂಕ	 todo;
		    ಪೂರ್ಣಾಂಕ	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_ಕೊನೆ = ptr + ಗಾತ್ರ_2;
			ಆಗ (ascii_ಕೊನೆ - p >= (ದೀರ್ಘ)ಗಾತ್ರ(long_u))
			{
			    long_u ಶಬ್ದ;
			    ಸ್ಮರಣೆ_ನಕಲು(&ಶಬ್ದ, p, ಗಾತ್ರ(long_u));
			    ಆದರೆ (ಶಬ್ದ & NONASCII_ಮುಖವಾಡ)
				ನಿಲ್ಲಿಸು;
			    p += ಗಾತ್ರ(long_u);
			}
			ಆಗ (p < ascii_ಕೊನೆ && *p < 0x80)
			    ++p;
		    }

		    todo = (ಪೂರ್ಣಾಂಕ)((ptr + ಗಾತ್ರ_2) - p);
		    ಆದರೆ (todo <= 0)
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			ಆದರೆ (l > todo && !incomplete_ಬಾಲ)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    ಆದರೆ (p > ptr || filesize > 0)
				incomplete_ಬಾಲ = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    ಆದರೆ (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				ಗಾತ್ರ_2 -= conv_restlen;
				ನಿಲ್ಲಿಸು;
			    }
			}
			ಆದರೆ (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    ಆದರೆ (can_retry && !incomplete_ಬಾಲ)
				ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
			    // When we did a conversion report an error.
			    ಆದರೆ (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#ಆದರೆ_ಅಂತ್ಯ
			    // Remember the first linenr with an illegal byte
			    ಆದರೆ (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    ಆದರೆ (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--ಗಾತ್ರ_2;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				ಆದರೆ (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    p += l;
		    }
		}
		ಆದರೆ (p < ptr + ಗಾತ್ರ_2 && !incomplete_ಬಾಲ)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#ಆದರೆ defined(FEAT_EVAL) && defined(USE_ICONV)
		    ಆದರೆ (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    ಹೋಗು retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += ಗಾತ್ರ_2;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    ಆದರೆ (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		ಆದರೆ (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    ಆದರೆ (try_mac)
			try_mac = 1;

		    ಗಾಗಿ (p = ptr; p < ptr + ಗಾತ್ರ_2; ++p)
		    {
			ಆದರೆ (*p == NL)
			{
			    ಆದರೆ (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    ಇಲ್ಲದಿದ್ದರೆ
				fileformat = EOL_UNIX;
			    ನಿಲ್ಲಿಸು;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    ಆದರೆ (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			ಗಾಗಿ (; p >= ptr && *p != CAR; p--)
			    ;
			ಆದರೆ (p >= ptr)
			{
			    ಗಾಗಿ (p = ptr; p < ptr + ಗಾತ್ರ_2; ++p)
			    {
				ಆದರೆ (*p == NL)
				    try_unix++;
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == CAR)
				    try_mac++;
			    }
			    ಆದರೆ (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		ಆದರೆ (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		ಆದರೆ (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		ಆದರೆ (ಗಣ_options)
		    ಗಣ_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	ಆದರೆ (fileformat == EOL_MAC)
	{
	    --ptr;
	    ಆಗ (++ptr, --ಗಾತ್ರ_2 >= 0)
	    {
		// catch most common case first
		ಆದರೆ ((c = *ptr) != NUL && c != CAR && c != NL)
		    ಮುಂದುವರಿ;
		ಆದರೆ (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (skip_ಎಣಿಕೆ == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - ಸಾಲು_ಆರಂಭ + 1);
			ಆದರೆ (ml_append(lnum, ಸಾಲು_ಆರಂಭ, len, newfile) == FAIL)
			{
			    error = TRUE;
			    ನಿಲ್ಲಿಸು;
			}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
			ಆದರೆ (read_undo_file)
			    sha256_update(&sha_ctx, ಸಾಲು_ಆರಂಭ, len);
#ಆದರೆ_ಅಂತ್ಯ
			++lnum;
			ಆದರೆ (--read_ಎಣಿಕೆ == 0)
			{
			    error = TRUE;	// break loop
			    ಸಾಲು_ಆರಂಭ = ptr;	// nothing left to write
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			--skip_ಎಣಿಕೆ;
		    ಸಾಲು_ಆರಂಭ = ptr + 1;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *ಕೊನೆ = ptr + ಗಾತ್ರ_2;

	    ಆಗ (ptr < ಕೊನೆ)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, ಕೊನೆ - ptr);
		char_u *nul_ಪರಿಶೀಲನೆ;

		ಆದರೆ (nl == ಖಾಲಿ)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    ಆಗ ((nul_ಪರಿಶೀಲನೆ = (char_u *)memchr(ptr, NUL,
						      ಕೊನೆ - ptr)) != ಖಾಲಿ)
		    {
			*nul_ಪರಿಶೀಲನೆ = NL;
			ptr = nul_ಪರಿಶೀಲನೆ + 1;
		    }
		    ptr = ಕೊನೆ;
		    ನಿಲ್ಲಿಸು;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *ಪರಿಶೀಲನೆ = ptr;
		    ಆಗ ((nul_ಪರಿಶೀಲನೆ = (char_u *)memchr(ಪರಿಶೀಲನೆ, NUL,
						       nl - ಪರಿಶೀಲನೆ)) != ಖಾಲಿ)
		    {
			*nul_ಪರಿಶೀಲನೆ = NL;
			ಪರಿಶೀಲನೆ = nul_ಪರಿಶೀಲನೆ + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		ಆದರೆ (skip_ಎಣಿಕೆ == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - ಸಾಲು_ಆರಂಭ + 1);
		    ಆದರೆ (fileformat == EOL_DOS)
		    {
			ಆದರೆ (ptr > ಸಾಲು_ಆರಂಭ && ptr[-1] == CAR)
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
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ff_error != EOL_DOS)
			{
			    ಆದರೆ (   try_unix
				&& !read_stdin
				&& (read_ಬಫರ್
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				ಆದರೆ (ಗಣ_options)
				    ಗಣ_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				ಹೋಗು retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    ಆದರೆ (ml_append(lnum, ಸಾಲು_ಆರಂಭ, len, newfile) == FAIL)
		    {
			error = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
		    ಆದರೆ (read_undo_file)
			sha256_update(&sha_ctx, ಸಾಲು_ಆರಂಭ, len);
#ಆದರೆ_ಅಂತ್ಯ
		    ++lnum;
		    ಆದರೆ (--read_ಎಣಿಕೆ == 0)
		    {
			error = TRUE;	    // break loop
			ಸಾಲು_ಆರಂಭ = ptr;   // nothing left to write
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    --skip_ಎಣಿಕೆ;
		ಸಾಲು_ಆರಂಭ = ptr + 1;
		++ptr;
	    }
	    ಗಾತ್ರ_2 = -1;
	}
	linerest = (ದೀರ್ಘ)(ptr - ಸಾಲು_ಆರಂಭ);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    ಆದರೆ (error && read_ಎಣಿಕೆ == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    ಆದರೆ (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	ಆದರೆ (ಗಣ_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    ಆದರೆ (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	ಆದರೆ (ಗಣ_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - ಸಾಲು_ಆರಂಭ + 1);
	ಆದರೆ (ml_append(lnum, ಸಾಲು_ಆರಂಭ, len, newfile) == FAIL)
	    error = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
	    ಆದರೆ (read_undo_file)
		sha256_update(&sha_ctx, ಸಾಲು_ಆರಂಭ, len);
#ಆದರೆ_ಅಂತ್ಯ
	    read_no_eol_lnum = ++lnum;
	}
    }

    ಆದರೆ (ಗಣ_options)
	save_file_ff(curbuf);		// remember the current file format

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಆದರೆ (curbuf->b_cryptstate != ಖಾಲಿ)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = ಖಾಲಿ;
    }
    ಆದರೆ (cryptkey != ಖಾಲಿ && cryptkey != curbuf->b_p_ಕೀಲಿ)
	crypt_free_ಕೀಲಿ(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#ಆದರೆ_ಅಂತ್ಯ

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    ಆದರೆ (ಗಣ_options)
	ಗಣ_string_option_ನೇರ((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    ಆದರೆ (fenc_alloced)
	vim_free(fenc);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
    ಆದರೆ (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!read_ಬಫರ್ && !read_stdin)
	close(fd);				// errors are ignored
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FD_CLOEXEC
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ fdflags = fcntl(fd, F_GETFD);

	ಆದರೆ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (ಶೂನ್ಯ)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#ಆದರೆ_ಅಂತ್ಯ
    vim_free(ಬಫರ್);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_DUP
    ಆದರೆ (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (tmpname != ಖಾಲಿ)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    ಆದರೆ (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	ಆದರೆ (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#ಆದರೆ_ಅಂತ್ಯ
	    ml_delete(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#ಆದರೆ_ಅಂತ್ಯ
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - linecnt;
	ಆದರೆ (filesize == 0)
	    linecnt = 0;
	ಆದರೆ (newfile || read_ಬಫರ್)
	{
	    redraw_curbuf_later(UPD_NOT_VALID);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    // After reading the text into the buffer the diff info needs to
	    // be updated.
	    diff_invalidate(curbuf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // All folds in the window are invalid now.  Mark them for update
	    // before triggering autocommands.
	    foldUpdateAll(curwin);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (linecnt)		// appended at least one line
	    appended_ಸಾಲುಗಳು_mark(from, linecnt);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	ಆದರೆ (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (got_int)
	{
	    ಆದರೆ (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		ಆದರೆ (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
	    check_marks_read();
#ಆದರೆ_ಅಂತ್ಯ
	    retval = OK;	// an interrupt isn't really an error
	    ಹೋಗು theend;
	}

	ಆದರೆ (!filtering && !(flags & READ_DUMMY))
	{
	    ಪೂರ್ಣಾಂಕ	buflen;

	    ಸಂದೇಶ_ಕೂಡಿಸು_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (ಪೂರ್ಣಾಂಕ)STRLEN(IObuff);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	    ಆದರೆ (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    ಆದರೆ (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    ಆದರೆ (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    ಆದರೆ (read_no_eol_lnum)
	    {
		ಸಂದೇಶ_ಕೂಡಿಸು_eol();
		c = TRUE;
	    }
	    ಆದರೆ (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    ಆದರೆ (ವಿಭಜಿಸು)
	    {
		buflen += vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    ಆದರೆ (notconverted)
	    {
		buflen += vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (converted)
	    {
		buflen += vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	    ಆದರೆ (cryptkey != ಖಾಲಿ)
	    {
		crypt_append_ಸಂದೇಶ(curbuf);
		c = TRUE;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (conv_error != 0)
	    {
		vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (ದೀರ್ಘ)conv_error);
		c = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (illegal_byte > 0)
	    {
		vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (ದೀರ್ಘ)illegal_byte);
		c = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (error)
	    {
		vim_snprintf((ಅಕ್ಷರ *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    ಆದರೆ (ಸಂದೇಶ_ಕೂಡಿಸು_fileformat(fileformat))
		c = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	    ಆದರೆ (cryptkey != ಖಾಲಿ)
		ಸಂದೇಶ_ಕೂಡಿಸು_ಸಾಲುಗಳು(c, (ದೀರ್ಘ)linecnt, filesize
			 - crypt_get_ಶೀರ್ಷಿಕೆ_len(crypt_get_ವಿಧಾನ_nr(curbuf)));
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಸಂದೇಶ_ಕೂಡಿಸು_ಸಾಲುಗಳು(c, (ದೀರ್ಘ)linecnt, filesize);

	    VIM_CLEAR(keep_ಸಂದೇಶ);
	    ಸಂದೇಶ_scrolled_ign = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    ಆದರೆ (read_stdin || read_ಬಫರ್)
		p = ಸಂದೇಶ_may_trunc(FALSE, IObuff);
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು > 0)
		    ಸಂದೇಶ_putchar('\r');  // overwrite previous message
		p = (char_u *)ಸಂದೇಶ_trunc_attr((ಅಕ್ಷರ *)IObuff, FALSE, 0);
	    }
	    ಆದರೆ (read_stdin || read_ಬಫರ್ || restart_edit != 0
		    || (ಸಂದೇಶ_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		ಗಣ_keep_ಸಂದೇಶ(p, 0);
	    ಸಂದೇಶ_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	ಆದರೆ (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	ಆದರೆ (exmode_ಸಕ್ರಿಯ)
	    curwin->w_cursor.lnum = from + linecnt;
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set '[ and '] marks to the newly read lines.
	    curbuf->b_op_ಆರಂಭ.lnum = from + 1;
	    curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = 0;
	    curbuf->b_op_ಕೊನೆ.lnum = from + linecnt;
	    curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	/*
	 * Work around a weird problem: When a file has two links (only
	 * possible on NTFS) and we write through one link, then stat() it
	 * through the other link, the timestamp information may be wrong.
	 * It's correct again after reading the file, thus reset the timestamp
	 * here.
	 */
	ಆದರೆ (newfile && !read_stdin && !read_ಬಫರ್
					 && mch_stat((ಅಕ್ಷರ *)fname, &st) >= 0)
	{
	    buf_store_ಸಮಯ(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    /*
     * Get the marks before executing autocommands, so they can be used there.
     */
    check_marks_read();
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * We remember if the last line of the read didn't have
     * an eol even when 'binary' is off, to support turning 'fixeol' off,
     * or writing the read again with 'binary' on.  The latter is required
     * for ":autocmd FileReadPost *.gz set bin|'[,']!gunzip" to work.
     */
    curbuf->b_no_eol_lnum = read_no_eol_lnum;

    // When reloading a buffer put the cursor at the first line that is
    // different.
    ಆದರೆ (flags & READ_KEEP_UNDO)
	u_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬದಲಾಯಿತು();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    ಆದರೆ (read_undo_file)
    {
	char_u	hash[UNDO_HASH_ಗಾತ್ರ];

	sha256_ಮುಕ್ತಾಯ(&sha_ctx, hash);
	u_read_undo(ಖಾಲಿ, hash, fname);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!read_stdin && !read_fifo && (!read_ಬಫರ್ || sfname != ಖಾಲಿ))
    {
	ಪೂರ್ಣಾಂಕ m = ಸಂದೇಶ_scroll;
	ಪೂರ್ಣಾಂಕ n = ಸಂದೇಶ_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	ಆದರೆ (ಗಣ_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	ಸಂದೇಶ_scroll = TRUE;
	ಆದರೆ (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, ಖಾಲಿ, sfname,
							  FALSE, curbuf, eap);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (newfile || (read_ಬಫರ್ && sfname != ಖಾಲಿ))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, ಖಾಲಿ, sfname,
							  FALSE, curbuf, eap);
	    ಆದರೆ (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, ಖಾಲಿ, eap);
	ಆದರೆ (ಸಂದೇಶ_scrolled == n)
	    ಸಂದೇಶ_scroll = m;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (aborting())	    // autocmds may abort script processing
	    ಹೋಗು theend;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (!(recoverymode && error))
	retval = OK;

theend:
    ಆದರೆ (curbuf->b_ml.ml_mfp != ಖಾಲಿ
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    ಹಿಂತಿರುಗಿ retval;
}

#ಆದರೆ defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    ಪೂರ್ಣಾಂಕ
is_dev_fd_file(char_u *fname)
{
    ಹಿಂತಿರುಗಿ (STRNCMP(fname, "/dev/fd/", 8) == 0
	    && VIM_ISDIGIT(fname[8])
	    && *skipdigits(fname + 9) == NUL
	    && (fname[9] != NUL
		|| (fname[8] != '0' && fname[8] != '1' && fname[8] != '2')));
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * From the current line count and characters read after that, estimate the
 * line number where we are now.
 * Used for error messages that include a line number.
 */
    ಸ್ಥಿರ linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - linecnt + 1;
    ಗಾಗಿ (s = p; s < endp; ++s)
	ಆದರೆ (*s == '\n')
	    ++lnum;
    ಹಿಂತಿರುಗಿ lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (ಚಿಹ್ನೆರಹಿತ)STRLEN(buf->b_p_fenc));
    ಆದರೆ (eap->cmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    sprintf((ಅಕ್ಷರ *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    ಶೂನ್ಯ
ಗಣ_file_options(ಪೂರ್ಣಾಂಕ ಗಣ_options, exarg_T *eap)
{
    // set default 'fileformat'
    ಆದರೆ (ಗಣ_options)
    {
	ಆದರೆ (eap != ಖಾಲಿ && eap->force_ff != 0)
	    ಗಣ_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_ffs != NUL)
	    ಗಣ_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    ಆದರೆ (eap != ಖಾಲಿ && eap->force_bin != 0)
    {
	ಪೂರ್ಣಾಂಕ	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	ಗಣ_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    ಶೂನ್ಯ
ಗಣ_forced_fenc(exarg_T *eap)
{
    ಆದರೆ (eap->force_enc == 0)
	ಹಿಂತಿರುಗಿ;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    ಆದರೆ (fenc != ಖಾಲಿ)
	ಗಣ_string_option_ನೇರ((char_u *)"fenc", -1,
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
    ಸ್ಥಿರ char_u *
ಮುಂದಿನ_fenc(char_u **pp, ಪೂರ್ಣಾಂಕ *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    ಆದರೆ (**pp == NUL)
    {
	*pp = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    ಆದರೆ (p == ಖಾಲಿ)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	ಆದರೆ (r != ಖಾಲಿ)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    ಆದರೆ (r != ಖಾಲಿ)
	*alloced = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// out of memory
	r = (char_u *)"";
	*pp = ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ r;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Convert a file with the 'charconvert' expression.
 * This closes the file which is to be read, converts it and opens the
 * resulting file for reading.
 * Returns name of the resulting converted file (the caller should delete it
 * after reading it).
 * Returns NULL if the conversion failed ("*fdp" is not set) .
 */
    ಸ್ಥಿರ char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    ಪೂರ್ಣಾಂಕ		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;

    tmpname = vim_tempname('r', FALSE);
    ಆದರೆ (tmpname == ಖಾಲಿ)
	errmsg = _("Can't find temp file for conversion");
    ಇಲ್ಲದಿದ್ದರೆ
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	ಆದರೆ (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	ಆದರೆ (errmsg == ಖಾಲಿ && (*fdp = mch_open((ಅಕ್ಷರ *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    ಆದರೆ (errmsg != ಖಾಲಿ)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	ಸಂದೇಶ(errmsg);
	ಆದರೆ (tmpname != ಖಾಲಿ)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    ಆದರೆ (*fdp < 0)
	*fdp = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0);

    ಹಿಂತಿರುಗಿ tmpname;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    ಸ್ಥಿರ char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    ದೀರ್ಘ	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    ಪೂರ್ಣಾಂಕ		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    ಪೂರ್ಣಾಂಕ		*did_ask)	// flag: whether already asked for key
{
    ಪೂರ್ಣಾಂಕ ವಿಧಾನ = crypt_ವಿಧಾನ_nr_from_magic((ಅಕ್ಷರ *)ptr, *sizep);
    ಪೂರ್ಣಾಂಕ b_p_ro = curbuf->b_p_ro;

    ಆದರೆ (ವಿಧಾನ >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_ಗಣ_cm_option(curbuf, ವಿಧಾನ);
	ಆದರೆ (cryptkey == ಖಾಲಿ && !*did_ask)
	{
	    ಆದರೆ (*curbuf->b_p_ಕೀಲಿ)
	    {
		cryptkey = curbuf->b_p_ಕೀಲಿ;
		crypt_check_swapfile_curbuf();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_ಕೀಲಿ_ಸಂದೇಶ), fname);
		ಸಂದೇಶ_scroll = TRUE;
		crypt_check_ವಿಧಾನ(ವಿಧಾನ);
		cryptkey = crypt_get_ಕೀಲಿ(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		ಆದರೆ (cryptkey != ಖಾಲಿ && *cryptkey == NUL)
		{
		    ಆದರೆ (cryptkey != curbuf->b_p_ಕೀಲಿ)
			vim_free(cryptkey);
		    cryptkey = ಖಾಲಿ;
		}
	    }
	}

	ಆದರೆ (cryptkey != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ ಶೀರ್ಷಿಕೆ_len;

	    ಶೀರ್ಷಿಕೆ_len = crypt_get_ಶೀರ್ಷಿಕೆ_len(ವಿಧಾನ);
	    ಆದರೆ (*sizep < ಶೀರ್ಷಿಕೆ_len)
	    {
		// invalid header, buffer can't be encrypted
		ಆದರೆ (cryptkey != curbuf->b_p_ಕೀಲಿ)
		    vim_free(cryptkey);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }

	    curbuf->b_cryptstate = crypt_create_from_ಶೀರ್ಷಿಕೆ(
							ವಿಧಾನ, cryptkey, ptr);
	    crypt_ಗಣ_cm_option(curbuf, ವಿಧಾನ);

	    // Remove cryptmethod specific header from the text.
	    *filesizep += ಶೀರ್ಷಿಕೆ_len;
	    *sizep -= ಶೀರ್ಷಿಕೆ_len;
	    mch_memmove(ptr, ptr + ಶೀರ್ಷಿಕೆ_len, (size_t)*sizep);

	    // Restore the read-only flag.
	    curbuf->b_p_ro = b_p_ro;
	}
    }
    // When starting to edit a new file which does not have encryption, clear
    // the 'key' option, except when starting up (called with -x argument)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (newfile && *curbuf->b_p_ಕೀಲಿ != NUL && !starting)
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    ಹಿಂತಿರುಗಿ cryptkey;
}
#ಆದರೆ_ಅಂತ್ಯ  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    ಪೂರ್ಣಾಂಕ
check_file_readonly(
    char_u	*fname,		// full path to file
    ಪೂರ್ಣಾಂಕ		perm ಬಳಸದ)	// known permissions on file
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_MCH_ACCESS
    ಪೂರ್ಣಾಂಕ	    fd = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((ಅಕ್ಷರ *)fname, W_OK)
#ಇಲ್ಲದಿದ್ದರೆ
	(fd = mch_open((ಅಕ್ಷರ *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#ಆದರೆ_ಅಂತ್ಯ
	);
}

#ಆದರೆ defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    ಪೂರ್ಣಾಂಕ
vim_fsync(ಪೂರ್ಣಾಂಕ fd)
{
    ಪೂರ್ಣಾಂಕ r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    ಆದರೆ (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    ಹಿಂತಿರುಗಿ r;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    ಆದರೆ (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (aborting())	    // autocmds may abort script processing
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_ಬದಲಾಯಿತು_ಬಫರ್_or_ಬಫರ್_name));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    ಆದರೆ (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (aborting())	    // autocmds may abort script processing
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

    // Do filetype detection now if 'filetype' is empty.
    ಆದರೆ (*curbuf->b_p_ft == NUL)
    {
	ಆದರೆ (au_has_ಗುಂಪು((char_u *)"filetypedetect"))
	    (ಶೂನ್ಯ)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, ಖಾಲಿ);
	do_modelines(0);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_ಕೂಡಿಸು_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    ಆದರೆ (fname == ಖಾಲಿ)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_ಕೂಡಿಸು_fileformat(ಪೂರ್ಣಾಂಕ eol_type)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_CRNL
    ಆದರೆ (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_ಪಠ್ಯ) ? _("[dos]") : _("[dos format]"));
	ಹಿಂತಿರುಗಿ TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_ಪಠ್ಯ) ? _("[mac]") : _("[mac format]"));
	ಹಿಂತಿರುಗಿ TRUE;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CRNL
    ಆದರೆ (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_ಪಠ್ಯ) ? _("[unix]") : _("[unix format]"));
	ಹಿಂತಿರುಗಿ TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_ಕೂಡಿಸು_ಸಾಲುಗಳು(
    ಪೂರ್ಣಾಂಕ	    ಸೇರಿಸು_space,
    ದೀರ್ಘ    lnum,
    off_T   nchars)
{
    ಪೂರ್ಣಾಂಕ  len = (ಪೂರ್ಣಾಂಕ)STRLEN(IObuff);

    ಆದರೆ (shortmess(SHM_ಸಾಲುಗಳು))
	vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), ಸೇರಿಸು_space ? " " : "", lnum, (varnumber_T)nchars);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	len += vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), ಸೇರಿಸು_space ? " " : "", lnum);
	vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_ಕೂಡಿಸು_eol(ಶೂನ್ಯ)
{
    STRCAT(IObuff, shortmess(SHM_ಕೊನೆಯ) ? _("[noeol]") : _("[Incomplete last line]"));
}

    ಪೂರ್ಣಾಂಕ
ಸಮಯ_differs(stat_T *st, ದೀರ್ಘ mtime, ದೀರ್ಘ mtime_ns ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ST_MTIM_NSEC
	(ದೀರ್ಘ)st->ST_MTIM_NSEC != mtime_ns ||
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(ದೀರ್ಘ)st->st_mtime - mtime > 1 || mtime - (ದೀರ್ಘ)st->st_mtime > 1
#ಇಲ್ಲದಿದ್ದರೆ
	(ದೀರ್ಘ)st->st_mtime != mtime
#ಆದರೆ_ಅಂತ್ಯ
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    ಪೂರ್ಣಾಂಕ
need_conversion(char_u *fenc)
{
    ಪೂರ್ಣಾಂಕ		same_encoding;
    ಪೂರ್ಣಾಂಕ		enc_flags;
    ಪೂರ್ಣಾಂಕ		fenc_flags;

    ಆದರೆ (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    ಆದರೆ (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	ಹಿಂತಿರುಗಿ enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    ಹಿಂತಿರುಗಿ !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    ಪೂರ್ಣಾಂಕ
get_fio_flags(char_u *ptr)
{
    ಪೂರ್ಣಾಂಕ		prop;

    ಆದರೆ (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    ಆದರೆ (prop & ENC_UNICODE)
    {
	ಆದರೆ (prop & ENC_2BYTE)
	{
	    ಆದರೆ (prop & ENC_ENDIAN_L)
		ಹಿಂತಿರುಗಿ FIO_UCS2 | FIO_ENDIAN_L;
	    ಹಿಂತಿರುಗಿ FIO_UCS2;
	}
	ಆದರೆ (prop & ENC_4BYTE)
	{
	    ಆದರೆ (prop & ENC_ENDIAN_L)
		ಹಿಂತಿರುಗಿ FIO_UCS4 | FIO_ENDIAN_L;
	    ಹಿಂತಿರುಗಿ FIO_UCS4;
	}
	ಆದರೆ (prop & ENC_2_ಶಬ್ದ)
	{
	    ಆದರೆ (prop & ENC_ENDIAN_L)
		ಹಿಂತಿರುಗಿ FIO_UTF16 | FIO_ENDIAN_L;
	    ಹಿಂತಿರುಗಿ FIO_UTF16;
	}
	ಹಿಂತಿರುಗಿ FIO_UTF8;
    }
    ಆದರೆ (prop & ENC_LATIN1)
	ಹಿಂತಿರುಗಿ FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    ಪೂರ್ಣಾಂಕ
get_win_fio_flags(char_u *ptr)
{
    ಪೂರ್ಣಾಂಕ		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    ಆದರೆ (!enc_utf8 && enc_codepage <= 0)
	ಹಿಂತಿರುಗಿ 0;

    cp = encname2codepage(ptr);
    ಆದರೆ (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	ಆದರೆ (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    ಪೂರ್ಣಾಂಕ
get_mac_fio_flags(char_u *ptr)
{
    ಆದರೆ ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	ಹಿಂತಿರುಗಿ FIO_MACROMAN;
    ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    ಸ್ಥಿರ char_u *
check_for_bom(
    char_u	*p,
    ದೀರ್ಘ	ಗಾತ್ರ_2,
    ಪೂರ್ಣಾಂಕ		*lenp,
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಅಕ್ಷರ	*name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len = 2;

    ಆದರೆ (p[0] == 0xef && p[1] == 0xbb && ಗಾತ್ರ_2 >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == 0xff && p[1] == 0xfe)
    {
	ಆದರೆ (ಗಾತ್ರ_2 >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	ಆದರೆ (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	ಇಲ್ಲದಿದ್ದರೆ
	    name = "utf-16";	// FE FF
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಾತ್ರ_2 >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    ಹಿಂತಿರುಗಿ (char_u *)name;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns "full_path" or pointer into "full_path" if shortened.
 */
    char_u *
shorten_fname1(char_u *ತುಂಬಿದ_ಮಾರ್ಗ)
{
    char_u	*dirname;
    char_u	*p = ತುಂಬಿದ_ಮಾರ್ಗ;

    dirname = alloc(MAXPATHL);
    ಆದರೆ (dirname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ತುಂಬಿದ_ಮಾರ್ಗ;
    ಆದರೆ (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(ತುಂಬಿದ_ಮಾರ್ಗ, dirname);
	ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
	    p = ತುಂಬಿದ_ಮಾರ್ಗ;
    }
    vim_free(dirname);
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns NULL if not shorter name possible, pointer into "full_path"
 * otherwise.
 */
    char_u *
shorten_fname(char_u *ತುಂಬಿದ_ಮಾರ್ಗ, char_u *dir_name)
{
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*p;

    ಆದರೆ (ತುಂಬಿದ_ಮಾರ್ಗ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(dir_name);
    ಆದರೆ (fnamencmp(dir_name, ತುಂಬಿದ_ಮಾರ್ಗ, len) == 0)
    {
	p = ತುಂಬಿದ_ಮಾರ್ಗ + len;
#ಆದರೆ defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	ಆದರೆ (!((len > 2) && (*(p - 2) == ':')))
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ಆದರೆ (vim_ispathsep(*p))
		ಮಾಡು
		    ++p;
		ಆಗ (vim_ispathsep_nocolon(*p));
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VMS   // the path separator is always part of the path
	    ಇಲ್ಲದಿದ್ದರೆ
		p = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
#ಆದರೆ defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > 3
	    && TOUPPER_LOC(ತುಂಬಿದ_ಮಾರ್ಗ[0]) == TOUPPER_LOC(dir_name[0])
	    && ತುಂಬಿದ_ಮಾರ್ಗ[1] == ':'
	    && vim_ispathsep(ತುಂಬಿದ_ಮಾರ್ಗ[2]))
	p = ತುಂಬಿದ_ಮಾರ್ಗ + 2;
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
	p = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ p;
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
    ಶೂನ್ಯ
shorten_buf_fname(buf_T *buf, char_u *dirname, ಪೂರ್ಣಾಂಕ force)
{
    char_u	*p;

    ಆದರೆ (buf->b_fname != ಖಾಲಿ
	    && !bt_nofilename(buf)
	    && !ಮಾರ್ಗ_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == ಖಾಲಿ
		|| mch_is_ತುಂಬಿದ_Name(buf->b_sfname)))
    {
	ಆದರೆ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	ಆದರೆ (p == ಖಾಲಿ || buf->b_fname == ಖಾಲಿ)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    ಶೂನ್ಯ
shorten_fnames(ಪೂರ್ಣಾಂಕ force)
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
    ಸ್ಥಿತಿ_redraw_all();
    redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    ಶೂನ್ಯ
shorten_filenames(char_u **fnames, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    ಆದರೆ (fnames == ಖಾಲಿ || ಎಣಿಕೆ_2 < 1)
	ಹಿಂತಿರುಗಿ;
    mch_dirname(dirname, ಗಾತ್ರ(dirname));
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	ಆದರೆ ((p = shorten_fname(fnames[i], dirname)) != ಖಾಲಿ)
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
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ	    prepend_dot)	// may prepend a '.' to file name
{
    ಹಿಂತಿರುಗಿ buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    ಪೂರ್ಣಾಂಕ	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    ಪೂರ್ಣಾಂಕ	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    size_t	ptrlen;
    ಪೂರ್ಣಾಂಕ		fnamelen, extlen;

    extlen = (ಪೂರ್ಣಾಂಕ)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    ಆದರೆ (fname == ಖಾಲಿ || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	ಆದರೆ (retval == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (ಪೂರ್ಣಾಂಕ)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fnamelen = (ಪೂರ್ಣಾಂಕ)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	ಆದರೆ (retval == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	STRCPY(retval, fname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	vms_remove_ಆವೃತ್ತಿ(retval); // we do not need versions here
#ಆದರೆ_ಅಂತ್ಯ
    }

    /*
     * search backwards until we hit a '/', '\' or ':' replacing all '.'
     * by '_' for MSDOS or when shortname option set and ext starts with a dot.
     * Then truncate what is after the '/', '\' or ':' to 8 characters for
     * MSDOS and 26 characters for AMIGA, a lot more for UNIX.
     */
    ಗಾಗಿ (ptr = retval + fnamelen; ptr > retval; MB_PTR_ಹಿಂಭಾಗ(retval, ptr))
    {
	ಆದರೆ (*ext == '.' && shortname)
	    ಆದರೆ (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	ಆದರೆ (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    ನಿಲ್ಲಿಸು;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    ಆದರೆ (ptrlen > (ಚಿಹ್ನೆರಹಿತ)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    ಆದರೆ (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	ಆದರೆ (fname == ಖಾಲಿ || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    ಆದರೆ (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ext == '.')
	{
	    ಆದರೆ ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((e = vim_strchr(ptr, '.')) == ಖಾಲಿ)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಪೂರ್ಣಾಂಕ)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((fname == ಖಾಲಿ || *fname == NUL) && *ext == '.')
	*s++ = '_';
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Append the extension.
     * ext can start with '.' and cannot exceed 3 more characters.
     */
    STRCPY(s, ext);
    /*
     * Prepend the dot.
     */
    ಆದರೆ (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (size_t)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    ಆದರೆ (fname != ಖಾಲಿ && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	ಆಗ (--s >= ptr)
	{
	    ಆದರೆ (*s != '_')
	    {
		*s = '_';
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    ಪೂರ್ಣಾಂಕ
vim_fgets(char_u *buf, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2, FILE *fp)
{
    ಅಕ್ಷರ	*eof;
#ವ್ಯಾಖ್ಯಾನಿಸು FGETS_ಗಾತ್ರ 200
    ಅಕ್ಷರ	tbuf[FGETS_ಗಾತ್ರ];

    // safety check
    ಆದರೆ (ಗಾತ್ರ_2 < 2)
    {
	ಆದರೆ (ಗಾತ್ರ_2 == 1)
	    buf[0] = NUL;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    buf[ಗಾತ್ರ_2 - 2] = NUL;
    eof = fgets((ಅಕ್ಷರ *)buf, ಗಾತ್ರ_2, fp);
    ಆದರೆ (buf[ಗಾತ್ರ_2 - 2] != NUL && buf[ಗಾತ್ರ_2 - 2] != '\n')
    {
	buf[ಗಾತ್ರ_2 - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	ಮಾಡು
	{
	    tbuf[FGETS_ಗಾತ್ರ - 2] = NUL;
	    vim_ignoredp = fgets((ಅಕ್ಷರ *)tbuf, FGETS_ಗಾತ್ರ, fp);
	} ಆಗ (tbuf[FGETS_ಗಾತ್ರ - 2] != NUL && tbuf[FGETS_ಗಾತ್ರ - 2] != '\n');
    }
    ಹಿಂತಿರುಗಿ (eof == ಖಾಲಿ);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    ಪೂರ್ಣಾಂಕ
vim_rename(char_u *from, char_u *ಗಮ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		ret;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
    BPTR	flock;
#ಆದರೆ_ಅಂತ್ಯ
    stat_T	st;
    ಪೂರ್ಣಾಂಕ		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    ಆದರೆ (fnamecmp(from, ಗಮ್ಯ) == 0)
    {
	ಆದರೆ (p_fic && STRCMP(gettail(from), gettail(ಗಮ್ಯ)) != 0)
	    use_tmp_file = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)from, &st) < 0)
	ಹಿಂತಿರುಗಿ -1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    {
	stat_T	st_ಗಮ್ಯ;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	ಆದರೆ (mch_stat((ಅಕ್ಷರ *)ಗಮ್ಯ, &st_ಗಮ್ಯ) >= 0
		&& st.st_dev == st_ಗಮ್ಯ.st_dev
		&& st.st_ino == st_ಗಮ್ಯ.st_ino)
	    use_tmp_file = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    {
	BY_HANDLE_FILE_INFORMATION info1, info2;

	// It's possible for the source and destination to be the same file.
	// In that case go through a temp file name.  This makes rename("foo",
	// "./foo") a no-op (in a complicated way).
	ಆದರೆ (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(ಗಮ್ಯ, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_ಸಂಖ್ಯೆ == info2.dw_Volume_Serial_ಸಂಖ್ಯೆ
		&& info1.n_File_ಸೂಚ್ಯಂಕ_ಮೇಲಿನ == info2.n_File_ಸೂಚ್ಯಂಕ_ಮೇಲಿನ
		&& info1.n_File_ಸೂಚ್ಯಂಕ_ಕೆಳಗಿನ == info2.n_File_ಸೂಚ್ಯಂಕ_ಕೆಳಗಿನ)
	    use_tmp_file = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (use_tmp_file)
    {
	ಅಕ್ಷರ	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	ಆದರೆ (STRLEN(from) >= MAXPATHL - 5)
	    ಹಿಂತಿರುಗಿ -1;
	STRCPY(tempname, from);
	ಗಾಗಿ (n = 123; n < 99999; ++n)
	{
	    sprintf((ಅಕ್ಷರ *)gettail((char_u *)tempname), "%d", n);
	    ಆದರೆ (mch_stat(tempname, &st) < 0)
	    {
		ಆದರೆ (mch_rename((ಅಕ್ಷರ *)from, tempname) == 0)
		{
		    ಆದರೆ (mch_rename(tempname, (ಅಕ್ಷರ *)ಗಮ್ಯ) == 0)
			ಹಿಂತಿರುಗಿ 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (ಶೂನ್ಯ)mch_rename(tempname, (ಅಕ್ಷರ *)from);
		    ಹಿಂತಿರುಗಿ -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		ಹಿಂತಿರುಗಿ -1;
	    }
	}
	ಹಿಂತಿರುಗಿ -1;
    }

    /*
     * Delete the "to" file, this is required on some systems to make the
     * mch_rename() work, on other systems it makes sure that we don't have
     * two files when the mch_rename() fails.
     */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
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
    flock = ಬೀಗ_2((UBYTE *)from, (ದೀರ್ಘ)VIM_ACCESS_READ);
#ಆದರೆ_ಅಂತ್ಯ
    mch_remove(ಗಮ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
    ಆದರೆ (flock)
	Un_ಬೀಗ(flock);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * First try a normal rename, return if it works.
     */
    ಆದರೆ (mch_rename((ಅಕ್ಷರ *)from, (ಅಕ್ಷರ *)ಗಮ್ಯ) == 0)
	ಹಿಂತಿರುಗಿ 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, ಗಮ್ಯ);
    ಆದರೆ (ret != OK)
	ಹಿಂತಿರುಗಿ -1;

    /*
     * Remove copied original file
     */
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)from, &st) >= 0)
	mch_remove(from);

    ಹಿಂತಿರುಗಿ 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    ಪೂರ್ಣಾಂಕ
vim_copyfile(char_u *from, char_u *ಗಮ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		fd_ಒಳ;
    ಪೂರ್ಣಾಂಕ		fd_ಹೊರಹೋಗು;
    ಪೂರ್ಣಾಂಕ		n;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    ಅಕ್ಷರ	*ಬಫರ್;
    ದೀರ್ಘ	perm;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_READLINK
    ಪೂರ್ಣಾಂಕ		ret;
    ಪೂರ್ಣಾಂಕ		len;
    stat_T	st;
    ಅಕ್ಷರ	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((ಅಕ್ಷರ *)from, &st);
    ಆದರೆ (ret >= 0 && S_ISLNK(st.st_ಬಹುಲಕ))
    {
	ret = -1;

	len = readlink((ಅಕ್ಷರ *)from, linkbuf, MAXPATHL);
	ಆದರೆ (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (ಅಕ್ಷರ *)ಗಮ್ಯ);
	}

	ಹಿಂತಿರುಗಿ ret == 0 ? OK : FAIL;
    }
#ಆದರೆ_ಅಂತ್ಯ

    perm = mch_getperm(from);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#ಆದರೆ_ಅಂತ್ಯ
    fd_ಒಳ = mch_open((ಅಕ್ಷರ *)from, O_RDONLY|O_EXTRA, 0);
    ಆದರೆ (fd_ಒಳ == -1)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
	mch_free_acl(acl);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_ಹೊರಹೋಗು = mch_open((ಅಕ್ಷರ *)ಗಮ್ಯ,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (ಪೂರ್ಣಾಂಕ)perm);
    ಆದರೆ (fd_ಹೊರಹೋಗು == -1)
    {
	close(fd_ಒಳ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
	mch_free_acl(acl);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಬಫರ್ = alloc(WRITEBUFSIZE);
    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
    {
	close(fd_ಹೊರಹೋಗು);
	close(fd_ಒಳ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
	mch_free_acl(acl);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆಗ ((n = read_eintr(fd_ಒಳ, ಬಫರ್, WRITEBUFSIZE)) > 0)
	ಆದರೆ (write_eintr(fd_ಹೊರಹೋಗು, ಬಫರ್, n) != n)
	{
	    errmsg = _(e_error_writing_ಗಮ್ಯ_str);
	    ನಿಲ್ಲಿಸು;
	}

    vim_free(ಬಫರ್);
    close(fd_ಒಳ);
    ಆದರೆ (close(fd_ಹೊರಹೋಗು) < 0)
	errmsg = _(e_error_closing_str);
    ಆದರೆ (n < 0)
    {
	errmsg = _(e_error_reading_str);
	ಗಮ್ಯ = from;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX	    // for Unix mch_open() already set the permission
    mch_setperm(ಗಮ್ಯ, perm);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
    mch_ಗಣ_acl(ಗಮ್ಯ, acl);
    mch_free_acl(acl);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, ಗಮ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (errmsg != ಖಾಲಿ)
    {
	semsg(errmsg, ಗಮ್ಯ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    ಪೂರ್ಣಾಂಕ
check_timestamps(
    ಪೂರ್ಣಾಂಕ		focus)		// called for GUI focus event
{
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		didit = 0;
    ಪೂರ್ಣಾಂಕ		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    ಆದರೆ (no_check_timestamps > 0)
	ಹಿಂತಿರುಗಿ FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    ಆದರೆ (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಆದರೆ (!stuff_ಖಾಲಿಗಳು() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_ಬೀಗ > 0 || allbuf_ಬೀಗ > 0)
	need_check_timestamps = TRUE;		// check later
    ಇಲ್ಲದಿದ್ದರೆ
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    ಆದರೆ (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		ಗಣ_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		ಆದರೆ (didit < n)
		    didit = n;
		ಆದರೆ (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    ಮುಂದುವರಿ;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	ಆದರೆ (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    ಸಂದೇಶ_puts("\n");
	    ಹೊರಹೋಗು_flush();
	}
    }
    ಹಿಂತಿರುಗಿ didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
move_ಸಾಲುಗಳು(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    ಪೂರ್ಣಾಂಕ		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    ಗಾಗಿ (lnum = 1; lnum <= frombuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	ಆದರೆ (p == ಖಾಲಿ || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    ನಿಲ್ಲಿಸು;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    ಆದರೆ (retval != FAIL)
    {
	curbuf = frombuf;
	ಗಾಗಿ (lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; lnum > 0; --lnum)
	    ಆದರೆ (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
    }

    curbuf = tbuf;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    ಪೂರ್ಣಾಂಕ
buf_check_timestamp(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		focus ಬಳಸದ)	// called for GUI focus event
{
    stat_T	st;
    ಪೂರ್ಣಾಂಕ		stat_res;
    ಪೂರ್ಣಾಂಕ		retval = 0;
    ಅಕ್ಷರ	*mesg = ಖಾಲಿ;
    ಅಕ್ಷರ	*mesg2 = "";
    ಪೂರ್ಣಾಂಕ		helpmesg = FALSE;
    ಎಣಿಕೆ {
	RELOAD_NONE,
	RELOAD_ಸಾಮಾನ್ಯ,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#ಆದರೆ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    ಪೂರ್ಣಾಂಕ		can_reload = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    off_T	orig_ಗಾತ್ರ = buf->b_orig_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		orig_ಬಹುಲಕ = buf->b_orig_ಬಹುಲಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಪೂರ್ಣಾಂಕ		save_mouse_correct = need_mouse_correct;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	busy = FALSE;
    ಪೂರ್ಣಾಂಕ		n;
    bufref_T	bufref;

    ಗಣ_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    ಆದರೆ (buf->b_ffname == ಖಾಲಿ
	    || buf->b_ml.ml_mfp == ಖಾಲಿ
	    || !bt_ಸಾಮಾನ್ಯ(buf)
	    || buf->b_saving
	    || busy
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    || is_Netbeans_ಬಫರ್(buf)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    || buf->b_ಪದ != ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((ಅಕ್ಷರ *)buf->b_ffname, &st)) < 0
		|| ಸಮಯ_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ST_ಬಹುಲಕ
		|| (ಪೂರ್ಣಾಂಕ)st.st_ಬಹುಲಕ != buf->b_orig_ಬಹುಲಕ
#ಇಲ್ಲದಿದ್ದರೆ
		|| mch_getperm(buf->b_ffname) != buf->b_orig_ಬಹುಲಕ
#ಆದರೆ_ಅಂತ್ಯ
		))
    {
	ದೀರ್ಘ prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	ಆದರೆ (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_ಗಾತ್ರ = 0;
	    buf->b_orig_ಬಹುಲಕ = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    buf_store_ಸಮಯ(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	ಆದರೆ (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !buf_Is_ಬದಲಾಯಿತು(buf) && stat_res >= 0)
	    reload = RELOAD_ಸಾಮಾನ್ಯ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಕ್ಷರ    *reason;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    size_t  reasonlen;
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (stat_res < 0)
	    {
		reason = "deleted";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(buf))
	    {
		reason = "conflict";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("conflict");
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    /*
	     * Check if the file contents really changed to avoid giving a
	     * warning when only the timestamp was set (e.g., checked out of
	     * CVS).  Always warn when the buffer was changed.
	     */
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (orig_ಗಾತ್ರ != buf->b_orig_ಗಾತ್ರ || buf_contents_ಬದಲಾಯಿತು(buf))
	    {
		reason = "changed";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (orig_ಬಹುಲಕ != buf->b_orig_ಬಹುಲಕ)
	    {
		reason = "mode";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		reason = "time";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("time");
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    /*
	     * Only give the warning if there are no FileChangedShell
	     * autocommands.
	     * Avoid being called recursively by setting "busy".
	     */
	    busy = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಗಣ_vim_var_string(VV_FCS_REASON, (char_u *)reason, (ಪೂರ್ಣಾಂಕ)reasonlen);
	    ಗಣ_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#ಆದರೆ_ಅಂತ್ಯ
	    ++allbuf_ಬೀಗ;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_ಬೀಗ;
	    busy = FALSE;
	    ಆದರೆ (n)
	    {
		ಆದರೆ (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_ಬಫರ್));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		ಆದರೆ (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_ಸಾಮಾನ್ಯ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s, "ask") == 0)
		    n = FALSE;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    ಹಿಂತಿರುಗಿ 2;
	    }
	    ಆದರೆ (!n)
	    {
		ಆದರೆ (*reason == 'd')
		{
		    // Only give the message once.
		    ಆದರೆ (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_ಲಭ್ಯ);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    helpmesg = TRUE;
#ಆದರೆ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
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
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#ಆದರೆ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (mesg != ಖಾಲಿ)
    {
	char_u *ಮಾರ್ಗ;

	ಮಾರ್ಗ = home_replace_save(buf, buf->b_fname);
	ಆದರೆ (ಮಾರ್ಗ != ಖಾಲಿ)
	{
	    size_t  tbufsize;
	    ಅಕ್ಷರ    *tbuf;

	    ಆದರೆ (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(ಮಾರ್ಗ) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    ಆದರೆ (tbuf != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, ಮಾರ್ಗ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		ಗಣ_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		ಆದರೆ (can_reload)
		{
		    ಆದರೆ (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    ಆಯ್ಕೆ (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, ಖಾಲಿ, TRUE))
		    {
			ಸ್ಥಿತಿ 2:
			    reload = RELOAD_ಸಾಮಾನ್ಯ;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 3:
			    reload = RELOAD_DETECT;
			    ನಿಲ್ಲಿಸು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (State > ಬಹುಲಕ_ಸಾಮಾನ್ಯ_BUSY || (State & ಬಹುಲಕ_CMDLINE)
								 || already_warned)
		{
		    ಆದರೆ (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (!autocmd_busy)
		    {
			ಸಂದೇಶ_ಆರಂಭ();
			ಸಂದೇಶ_puts_attr(tbuf, HL_ATTR(HLF_E) + ಸಂದೇಶ_HIST);
			ಆದರೆ (*mesg2 != NUL)
			    ಸಂದೇಶ_puts_attr(mesg2, HL_ATTR(HLF_W) + ಸಂದೇಶ_HIST);
			ಸಂದೇಶ_clr_eos();
			(ಶೂನ್ಯ)ಸಂದೇಶ_ಕೊನೆ();
			ಆದರೆ (emsg_silent == 0 && !ಒಳ_assert_fails)
			{
			    ಹೊರಹೋಗು_flush();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			    ಆದರೆ (!focus)
#ಆದರೆ_ಅಂತ್ಯ
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
	    vim_free(ಮಾರ್ಗ);
	}
    }

    ಆದರೆ (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_ಬಹುಲಕ, reload == RELOAD_DETECT);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
	ಆದರೆ (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != ಖಾಲಿ)
	{
	    char_u	    hash[UNDO_HASH_ಗಾತ್ರ];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(ಖಾಲಿ, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Trigger FileChangedShell when the file was changed in any way.
    ಆದರೆ (bufref_valid(&bufref) && retval != 0)
	(ಶೂನ್ಯ)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    ಶೂನ್ಯ
buf_reload(buf_T *buf, ಪೂರ್ಣಾಂಕ orig_ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    ಪೂರ್ಣಾಂಕ		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    ಪೂರ್ಣಾಂಕ		saved = OK;
    aco_save_T	aco;
    ಪೂರ್ಣಾಂಕ		flags = READ_NEW;
    ಪೂರ್ಣಾಂಕ		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    ಆದರೆ (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	ಹಿಂತಿರುಗಿ;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    ಆದರೆ (reload_options)
	CLEAR_ಕ್ಷೇತ್ರ(ea);
    ಇಲ್ಲದಿದ್ದರೆ
	prepped = prep_exarg(&ea, buf);

    ಆದರೆ (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	ಆದರೆ (p_ur < 0 || curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	ಆದರೆ (BUFEMPTY() || saved == FAIL)
	    savebuf = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(ಖಾಲಿ, ಖಾಲಿ, (linenr_T)1, BLN_DUMMY);
	    ಗಣ_bufref(&bufref, savebuf);
	    ಆದರೆ (savebuf != ಖಾಲಿ && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_ಬಫರ್ = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_ಬಫರ್ = buf;
	    }
	    ಆದರೆ (savebuf == ಖಾಲಿ || saved == FAIL || buf != curbuf
				      || move_ಸಾಲುಗಳು(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	ಆದರೆ (saved == OK)
	{
	    ಪೂರ್ಣಾಂಕ old_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    ಆದರೆ (shortmess(SHM_FILEINFO))
		ಸಂದೇಶ_silent = 1;

	    ಆದರೆ (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#ಆದರೆ defined(FEAT_EVAL)
		ಆದರೆ (!aborting())
#ಆದರೆ_ಅಂತ್ಯ
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		ಆದರೆ (savebuf != ಖಾಲಿ && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    ಆಗ (!BUFEMPTY())
			ಆದರೆ (ml_delete(buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ) == FAIL)
			    ನಿಲ್ಲಿಸು;
		    (ಶೂನ್ಯ)move_ಸಾಲುಗಳು(savebuf, buf);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		ಆದರೆ ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    ಸಂದೇಶ_silent = old_ಸಂದೇಶ_silent;
	}
	vim_free(ea.cmd);

	ಆದರೆ (savebuf != ಖಾಲಿ && bufref_valid(&bufref))
	    wipe_ಬಫರ್(savebuf, FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#ಆದರೆ_ಅಂತ್ಯ

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	ಆದರೆ (old_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    curwin->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_topline = old_topline;
	curwin->w_cursor = old_cursor;
	check_cursor();
	update_topline();
	curbuf->b_keep_filetype = false;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	{
	    win_T	*wp;
	    tabpage_T	*tp;

	    // Update folds unless they are defined manually.
	    FOR_ALL_TAB_WINDOWS(tp, wp)
		ಆದರೆ (wp->w_ಬಫರ್ == curwin->w_ಬಫರ್
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#ಆದರೆ_ಅಂತ್ಯ
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	ಆದರೆ (orig_ಬಹುಲಕ == curbuf->b_orig_ಬಹುಲಕ)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    ಶೂನ್ಯ
buf_store_ಸಮಯ(buf_T *buf, stat_T *st, char_u *fname ಬಳಸದ)
{
    buf->b_mtime = (ದೀರ್ಘ)st->st_mtime;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ST_MTIM_NSEC
    buf->b_mtime_ns = (ದೀರ್ಘ)st->ST_MTIM_NSEC;
#ಇಲ್ಲದಿದ್ದರೆ
    buf->b_mtime_ns = 0;
#ಆದರೆ_ಅಂತ್ಯ
    buf->b_orig_ಗಾತ್ರ = st->st_size;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ST_ಬಹುಲಕ
    buf->b_orig_ಬಹುಲಕ = (ಪೂರ್ಣಾಂಕ)st->st_ಬಹುಲಕ;
#ಇಲ್ಲದಿದ್ದರೆ
    buf->b_orig_ಬಹುಲಕ = mch_getperm(fname);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    ಶೂನ್ಯ
write_lnum_adjust(linenr_T ಸ್ಥಾನಾಂತರ)
{
    ಆದರೆ (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += ಸ್ಥಾನಾಂತರ;
}

// Subfuncions for readdirex()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# ifdef MSWIN
    ಸ್ಥಿರ char_u *
getfpermwfd(WIN32_ಕಂಡುಹಿಡಿ_DATAW *wfd, char_u *perm)
{
    stat_T	    st;
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ  st_ಬಹುಲಕ;
    DWORD	    flag = wfd->dw_File_ಗುಣಲಕ್ಷಣಗಳು;
    WCHAR	    *wp;

    st_ಬಹುಲಕ = (flag & FILE_ATTRIBUTE_DIRECTORY)
					? (_S_IFDIR | _S_IEXEC) : _S_IFREG;
    st_ಬಹುಲಕ |= (flag & FILE_ATTRIBUTE_READONLY)
					? _S_IREAD : (_S_IREAD | _S_IWRITE);

    wp = wcsrchr(wfd->cFileName, L'.');
    ಆದರೆ (wp != ಖಾಲಿ)
    {
	ಆದರೆ (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_ಬಹುಲಕ |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_ಬಹುಲಕ |= (st_ಬಹುಲಕ & 0700) >> 3;
    st_ಬಹುಲಕ |= (st_ಬಹುಲಕ & 0700) >> 6;

    st.st_ಬಹುಲಕ = st_ಬಹುಲಕ;
    ಹಿಂತಿರುಗಿ getfpermst(&st, perm);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
getftypewfd(WIN32_ಕಂಡುಹಿಡಿ_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dw_File_ಗುಣಲಕ್ಷಣಗಳು;
    DWORD ಟ್ಯಾಗ್ = wfd->dwReserved0;

    ಆದರೆ (flag & FILE_ATTRIBUTE_REPARSE_ಬಿಂದು)
    {
	ಆದರೆ (ಟ್ಯಾಗ್ == IO_REPARSE_ಟ್ಯಾಗ್_MOUNT_ಬಿಂದು)
	{
	    ret->string = (char_u *)"junction";
	    ret->ಉದ್ದಳತೆ = STRLEN_LITERAL("junction");
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (ಟ್ಯಾಗ್ == IO_REPARSE_ಟ್ಯಾಗ್_SYMLINK)
	{
	    ಆದರೆ (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->ಉದ್ದಳತೆ = STRLEN_LITERAL("linkd");
		ಹಿಂತಿರುಗಿ;
	    }

	    ret->string = (char_u *)"link";
	    ret->ಉದ್ದಳತೆ = STRLEN_LITERAL("link");
	    ಹಿಂತಿರುಗಿ;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->ಉದ್ದಳತೆ = STRLEN_LITERAL("reparse");
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->ಉದ್ದಳತೆ = STRLEN_LITERAL("dir");
	ಹಿಂತಿರುಗಿ;
    }

    ret->string = (char_u *)"file";
    ret->ಉದ್ದಳತೆ = STRLEN_LITERAL("file");
}

    ಸ್ಥಿರ dict_T *
create_readdirex_ಅಂಶ(WIN32_ಕಂಡುಹಿಡಿ_DATAW *wfd)
{
    dict_T	*ಅಂಶ;
    char_u	*p;
    varnumber_T	ಗಾತ್ರ_2, ಸಮಯ;
    char_u	permbuf[] = "---------";
    string_T	s;

    ಅಂಶ = dict_alloc();
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಅಂಶ->dv_refcount++;

    p = utf16_ಗಮ್ಯ_enc(wfd->cFileName, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹೋಗು theend;
    ಆದರೆ (dict_ಕೂಡಿಸು_string(ಅಂಶ, "name", p) == FAIL)
    {
	vim_free(p);
	ಹೋಗು theend;
    }
    vim_free(p);

    ಗಾತ್ರ_2 = (((varnumber_T)wfd->n_File_ಗಾತ್ರ_ಮೇಲಿನ) << 32) | wfd->n_File_ಗಾತ್ರ_ಕೆಳಗಿನ;
    ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(ಅಂಶ, "size", ಗಾತ್ರ_2) == FAIL)
	ಹೋಗು theend;

    // Convert FILETIME to unix time.
    ಸಮಯ = (((((varnumber_T)wfd->ft_ಕೊನೆಯ_Write_ಸಮಯ.dw_ಮೇಲಿನ_Date_ಸಮಯ) << 32) |
		wfd->ft_ಕೊನೆಯ_Write_ಸಮಯ.dw_ಕೆಳಗಿನ_Date_ಸಮಯ)
	    - 116444736000000000) / 10000000;
    ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(ಅಂಶ, "time", ಸಮಯ) == FAIL)
	ಹೋಗು theend;

    getftypewfd(wfd, &s);
    ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "type", s.string, (ಪೂರ್ಣಾಂಕ)s.ಉದ್ದಳತೆ) == FAIL)
	ಹೋಗು theend;
    ಆದರೆ (dict_ಕೂಡಿಸು_string(ಅಂಶ, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	ಹೋಗು theend;

    ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "user", (char_u *)"", 0) == FAIL)
	ಹೋಗು theend;
    ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "group", (char_u *)"", 0) == FAIL)
	ಹೋಗು theend;

    ಹಿಂತಿರುಗಿ ಅಂಶ;

theend:
    dict_unref(ಅಂಶ);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
# ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ dict_T *
create_readdirex_ಅಂಶ(char_u *ಮಾರ್ಗ, char_u *name)
{
    dict_T	*ಅಂಶ;
    ಅಕ್ಷರ	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    ಪೂರ್ಣಾಂಕ		ret, link = FALSE;
    varnumber_T	ಗಾತ್ರ_2;
    char_u	permbuf[] = "---------";
    string_T	q = {ಖಾಲಿ, 0};
    ರಚನೆ passwd *pw;
    ರಚನೆ ಗುಂಪು  *gr;

    ಅಂಶ = dict_alloc();
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಅಂಶ->dv_refcount++;

    pathlen = STRLEN(ಮಾರ್ಗ);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹೋಗು theend;
    ಆದರೆ (pathlen > 0 && ಮಾರ್ಗ[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", ಮಾರ್ಗ, name);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_snprintf(p, len, "%s/%s", ಮಾರ್ಗ, name);
    ret = mch_lstat(p, &st);
    ಆದರೆ (ret >= 0 && S_ISLNK(st.st_ಬಹುಲಕ))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	ಆದರೆ (ret < 0)
	    STR_LITERAL_ಗಣ(q, "link");
    }
    vim_free(p);

    ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "name", name, (ಪೂರ್ಣಾಂಕ)namelen) == FAIL)
	ಹೋಗು theend;

    ಆದರೆ (ret >= 0)
    {
	ಗಾತ್ರ_2 = (varnumber_T)st.st_size;
	ಆದರೆ (S_ISDIR(st.st_ಬಹುಲಕ))
	    ಗಾತ್ರ_2 = 0;
	// non-perfect check for overflow
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((off_T)ಗಾತ್ರ_2 != (off_T)st.st_size)
	    ಗಾತ್ರ_2 = -2;
	ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(ಅಂಶ, "size", ಗಾತ್ರ_2) == FAIL)
	    ಹೋಗು theend;
	ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(ಅಂಶ, "time", (varnumber_T)st.st_mtime) == FAIL)
	    ಹೋಗು theend;

	ಆದರೆ (link)
	{
	    ಆದರೆ (S_ISDIR(st.st_ಬಹುಲಕ))
		STR_LITERAL_ಗಣ(q, "linkd");
	    ಇಲ್ಲದಿದ್ದರೆ
		STR_LITERAL_ಗಣ(q, "link");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    q.string = getftypest(&st);
	    q.ಉದ್ದಳತೆ = STRLEN(q.string);
	}
	ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "type", q.string, (ಪೂರ್ಣಾಂಕ)q.ಉದ್ದಳತೆ) == FAIL)
	    ಹೋಗು theend;
	ಆದರೆ (dict_ಕೂಡಿಸು_string(ಅಂಶ, "perm", getfpermst(&st, permbuf)) == FAIL)
	    ಹೋಗು theend;

	pw = getpwuid(st.st_uid);
	ಆದರೆ (pw == ಖಾಲಿ)
	    STR_LITERAL_ಗಣ(q, "");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    q.string = (char_u *)pw->pw_name;
	    q.ಉದ್ದಳತೆ = STRLEN(q.string);
	}
	ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "user", q.string, (ಪೂರ್ಣಾಂಕ)q.ಉದ್ದಳತೆ) == FAIL)
	    ಹೋಗು theend;
#  ಆದರೆ !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	ಆದರೆ (gr == ಖಾಲಿ)
	    STR_LITERAL_ಗಣ(q, "");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    q.string = (char_u *)gr->gr_name;
	    q.ಉದ್ದಳತೆ = STRLEN(q.string);
	}
#  endif
	ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "group", q.string, (ಪೂರ್ಣಾಂಕ)q.ಉದ್ದಳತೆ) == FAIL)
	    ಹೋಗು theend;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(ಅಂಶ, "size", -1) == FAIL)
	    ಹೋಗು theend;
	ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(ಅಂಶ, "time", -1) == FAIL)
	    ಹೋಗು theend;
	ಆದರೆ (q.string == ಖಾಲಿ)
	{
	    ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "type", (char_u *)"", 0) == FAIL)
		ಹೋಗು theend;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "type", q.string, (ಪೂರ್ಣಾಂಕ)q.ಉದ್ದಳತೆ) == FAIL)
		ಹೋಗು theend;
	}
	ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "perm", (char_u *)"", 0) == FAIL)
	    ಹೋಗು theend;
	ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "user", (char_u *)"", 0) == FAIL)
	    ಹೋಗು theend;
	ಆದರೆ (dict_ಕೂಡಿಸು_string_len(ಅಂಶ, "group", (char_u *)"", 0) == FAIL)
	    ಹೋಗು theend;
    }
    ಹಿಂತಿರುಗಿ ಅಂಶ;

theend:
    dict_unref(ಅಂಶ);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
# endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compare_readdirex_ಅಂಶ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    ಆದರೆ (readdirex_ವಿಂಗಡಿಸು == READDIR_ವಿಂಗಡಿಸು_BYTE)
	ಹಿಂತಿರುಗಿ STRCMP(name1, name2);
    ಆದರೆ (readdirex_ವಿಂಗಡಿಸು == READDIR_ವಿಂಗಡಿಸು_IC)
	ಹಿಂತಿರುಗಿ STRICMP(name1, name2);

    ಹಿಂತಿರುಗಿ STRCOLL(name1, name2);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compare_readdir_ಅಂಶ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ಆದರೆ (readdirex_ವಿಂಗಡಿಸು == READDIR_ವಿಂಗಡಿಸು_BYTE)
	ಹಿಂತಿರುಗಿ STRCMP(*(ಅಕ್ಷರ **)s1, *(ಅಕ್ಷರ **)s2);
    ಆದರೆ (readdirex_ವಿಂಗಡಿಸು == READDIR_ವಿಂಗಡಿಸು_IC)
	ಹಿಂತಿರುಗಿ STRICMP(*(ಅಕ್ಷರ **)s1, *(ಅಕ್ಷರ **)s2);

    ಹಿಂತಿರುಗಿ STRCOLL(*(ಅಕ್ಷರ **)s1, *(ಅಕ್ಷರ **)s2);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    ಪೂರ್ಣಾಂಕ
readdir_core(
    garray_T	*gap,
    char_u	*ಮಾರ್ಗ,
    ಪೂರ್ಣಾಂಕ		withattr ಬಳಸದ,
    ಶೂನ್ಯ	*context,
    ಪೂರ್ಣಾಂಕ		(*checkitem)(ಶೂನ್ಯ *context, ಶೂನ್ಯ *ಅಂಶ),
    ಪೂರ್ಣಾಂಕ		ವಿಂಗಡಿಸು)
{
    ಪೂರ್ಣಾಂಕ			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    ಪೂರ್ಣಾಂಕ			ok;
    HANDLE		h_ಕಂಡುಹಿಡಿ = INVALID_HANDLE_ಮೌಲ್ಯ;
    WIN32_ಕಂಡುಹಿಡಿ_DATAW    wfd;
    WCHAR		*wn = ಖಾಲಿ;	// UTF-16 name, NULL when not used.
    char_u		*p_ಕೊನೆ;
# ಇಲ್ಲದಿದ್ದರೆ
    DIR			*dirp;
    ರಚನೆ dirent	*dp;
# endif

    ga_init2(gap, ಗಾತ್ರ(ಶೂನ್ಯ *), 20);

# ifdef FEAT_EVAL
#  define FREE_ಅಂಶ(ಅಂಶ)   ಮಾಡು { \
	ಆದರೆ (withattr) \
	    dict_unref((dict_T*)(ಅಂಶ)); \
	ಇಲ್ಲದಿದ್ದರೆ \
	    vim_free(ಅಂಶ); \
    } ಆಗ (0)

    readdirex_ವಿಂಗಡಿಸು = READDIR_ವಿಂಗಡಿಸು_BYTE;
# ಇಲ್ಲದಿದ್ದರೆ
#  define FREE_ಅಂಶ(ಅಂಶ)   vim_free(ಅಂಶ)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    STRNCPY(buf, ಮಾರ್ಗ, MAXPATHL-5);
    p = p_ಕೊನೆ = buf + STRLEN(buf);
    MB_PTR_ಹಿಂಭಾಗ(buf, p);
    ಆದರೆ (*p == '\\' || *p == '/')
	p_ಕೊನೆ = p;
    STRCPY(p_ಕೊನೆ, "\\*");

    wn = enc_ಗಮ್ಯ_utf16(buf, ಖಾಲಿ);
    ಆದರೆ (wn != ಖಾಲಿ)
	h_ಕಂಡುಹಿಡಿ = ಕಂಡುಹಿಡಿ_ಮೊದಲ_File_W(wn, &wfd);
    ok = (h_ಕಂಡುಹಿಡಿ != INVALID_HANDLE_ಮೌಲ್ಯ);
    ಆದರೆ (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), ಮಾರ್ಗ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಗ (ok)
	{
	    ಪೂರ್ಣಾಂಕ	    ignore;
	    ಶೂನ್ಯ    *ಅಂಶ;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    ಆದರೆ (ignore)
	    {
		ok = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_File_W(h_ಕಂಡುಹಿಡಿ, &wfd);
		ಮುಂದುವರಿ;
	    }
#  ifdef FEAT_EVAL
	    ಆದರೆ (withattr)
		ಅಂಶ = (ಶೂನ್ಯ*)create_readdirex_ಅಂಶ(&wfd);
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
		ಅಂಶ = (ಶೂನ್ಯ*)utf16_ಗಮ್ಯ_enc(wfd.cFileName, ಖಾಲಿ);
	    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	    {
		failed = TRUE;
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (!ignore && checkitem != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ r = checkitem(context, ಅಂಶ);

		ಆದರೆ (r < 0)
		{
		    FREE_ಅಂಶ(ಅಂಶ);
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (r == 0)
		    ignore = TRUE;
	    }

	    ಆದರೆ (!ignore)
	    {
		ಆದರೆ (ga_grow(gap, 1) == OK)
		    ((ಶೂನ್ಯ**)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = ಅಂಶ;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    failed = TRUE;
		    FREE_ಅಂಶ(ಅಂಶ);
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		FREE_ಅಂಶ(ಅಂಶ);

	    ok = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_File_W(h_ಕಂಡುಹಿಡಿ, &wfd);
	}
	ಕಂಡುಹಿಡಿ_Close(h_ಕಂಡುಹಿಡಿ);
    }

    vim_free(buf);
    vim_free(wn);
# ಇಲ್ಲದಿದ್ದರೆ	// MSWIN
    dirp = opendir((ಅಕ್ಷರ *)ಮಾರ್ಗ);
    ಆದರೆ (dirp == ಖಾಲಿ)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), ಮಾರ್ಗ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (;;)
	{
	    ಪೂರ್ಣಾಂಕ	    ignore;
	    ಶೂನ್ಯ    *ಅಂಶ;

	    dp = readdir(dirp);
	    ಆದರೆ (dp == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    ಆದರೆ (ignore)
		ಮುಂದುವರಿ;
#  ifdef FEAT_EVAL
	    ಆದರೆ (withattr)
		ಅಂಶ = (ಶೂನ್ಯ*)create_readdirex_ಅಂಶ(ಮಾರ್ಗ, p);
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
		ಅಂಶ = (ಶೂನ್ಯ*)vim_strsave(p);
	    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	    {
		failed = TRUE;
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (checkitem != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ r = checkitem(context, ಅಂಶ);

		ಆದರೆ (r < 0)
		{
		    FREE_ಅಂಶ(ಅಂಶ);
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (r == 0)
		    ignore = TRUE;
	    }

	    ಆದರೆ (!ignore)
	    {
		ಆದರೆ (ga_grow(gap, 1) == OK)
		    ((ಶೂನ್ಯ**)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = ಅಂಶ;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    failed = TRUE;
		    FREE_ಅಂಶ(ಅಂಶ);
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		FREE_ಅಂಶ(ಅಂಶ);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_ಅಂಶ

    ಆದರೆ (!failed && gap->ga_len > 0 && ವಿಂಗಡಿಸು > READDIR_ವಿಂಗಡಿಸು_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_ವಿಂಗಡಿಸು = ವಿಂಗಡಿಸು;
	ಆದರೆ (withattr)
	    qsort((ಶೂನ್ಯ*)gap->ga_ದತ್ತಾಂಶ, (size_t)gap->ga_len, ಗಾತ್ರ(dict_T*),
		    compare_readdirex_ಅಂಶ);
	ಇಲ್ಲದಿದ್ದರೆ
	    qsort((ಶೂನ್ಯ*)gap->ga_ದತ್ತಾಂಶ, (size_t)gap->ga_len, ಗಾತ್ರ(char_u *),
		    compare_readdir_ಅಂಶ);
# ಇಲ್ಲದಿದ್ದರೆ
	    ವಿಂಗಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು((char_u **)gap->ga_ದತ್ತಾಂಶ, gap->ga_len);
# endif
    }

    ಹಿಂತಿರುಗಿ failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    ಪೂರ್ಣಾಂಕ
delete_recursive(char_u *name)
{
    ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    ಆದರೆ (
# ಆದರೆ defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# ಇಲ್ಲದಿದ್ದರೆ
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *ಘಾತಾಂಕ = vim_strsave(name);
	garray_T    ga;

	ಆದರೆ (ಘಾತಾಂಕ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (readdir_core(&ga, ಘಾತಾಂಕ, FALSE, ಖಾಲಿ, ಖಾಲಿ, READDIR_ವಿಂಗಡಿಸು_NONE) == OK)
	{
	    ಪೂರ್ಣಾಂಕ	len = vim_snprintf((ಅಕ್ಷರ *)NameBuff, MAXPATHL, "%s/", ಘಾತಾಂಕ);
	    ಪೂರ್ಣಾಂಕ	i;

	    ಗಾಗಿ (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((ಅಕ್ಷರ *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_ದತ್ತಾಂಶ)[i]);
		ಆದರೆ (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    ಫಲಿತಾಂಶ = -1;
	    }
	    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
	    ಆದರೆ (mch_rmdir(ಘಾತಾಂಕ) != 0)
		ಫಲಿತಾಂಶ = -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಫಲಿತಾಂಶ = -1;
	vim_free(ಘಾತಾಂಕ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಫಲಿತಾಂಶ = mch_remove(name) == 0 ? 0 : -1;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(TEMPDIRNAMES)
ಸ್ಥಿರ ದೀರ್ಘ	ತಾತ್ಕಾಲಿಕ_ಎಣಿಕೆ = 0;		// Temp filename counter.

# ಆದರೆ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   ಸ್ಥಿರ ಶೂನ್ಯ
vim_opentempdir(ಶೂನ್ಯ)
{
    DIR *dp = ಖಾಲಿ;

    ಆದರೆ (vim_tempdir_dp != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    dp = opendir((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)vim_tempdir);
    ಆದರೆ (dp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   ಸ್ಥಿರ ಶೂನ್ಯ
vim_closetempdir(ಶೂನ್ಯ)
{
    ಆದರೆ (vim_tempdir_dp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = ಖಾಲಿ;
}

/*
 * Return true if the temp directory we created is gone.
 */
    ಸ್ಥಿರ bool
vim_tempdir_gone(ಶೂನ್ಯ)
{
    stat_T	st;

    ಆದರೆ (vim_tempdir_dp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;
    ಹಿಂತಿರುಗಿ fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    ಶೂನ್ಯ
vim_deltempdir(ಶೂನ್ಯ)
{
    ಆದರೆ (vim_tempdir == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

# ಆದರೆ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (vim_ತುಂಬಿದ_Name(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    ಆದರೆ (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += ಗಾತ್ರ(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# ಆದರೆ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_opentempdir();
# endif
    vim_free(buf);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ	    extra_char ಬಳಸದ,  // char to use in the name instead of '?'
    ಪೂರ್ಣಾಂಕ	    keep ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#ಅಥವಾಆದರೆ defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#ಇಲ್ಲದಿದ್ದರೆ
    char_u	itmp[TEMPNAMELEN];
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TEMPDIRNAMES
    ಸ್ಥಿರ ಅಕ್ಷರ	*(tempdirs[]) = {TEMPDIRNAMES};
    ಪೂರ್ಣಾಂಕ		i;
# ifndef EEXIST
    stat_T	st;
# endif

# ಆದರೆ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    ಆದರೆ (vim_tempdir != ಖಾಲಿ && vim_tempdir_gone())
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
    ಆದರೆ (vim_tempdir == ಖಾಲಿ)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    ದೀರ್ಘ	nr;
	    ದೀರ್ಘ	off;
# endif
	    size_t	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    ಆದರೆ (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		ಆದರೆ (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += ಗಾತ್ರ(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  ಆದರೆ defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    ಆದರೆ (mkdtemp((ಅಕ್ಷರ *)itmp) != ಖಾಲಿ)
			vim_settempdir(itmp);
#  ಆದರೆ defined(UNIX) || defined(VMS)
		    (ಶೂನ್ಯ)umask(umask_save);
#  endif
		}
# ಇಲ್ಲದಿದ್ದರೆ
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (ದೀರ್ಘ)ಸಮಯ(ಖಾಲಿ)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		ಗಾಗಿ (off = 0; off < 10000L; ++off)
		{
		    ಪೂರ್ಣಾಂಕ		r;
#  ಆದರೆ defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((ಅಕ್ಷರ *)itmp + itmplen, ಗಾತ್ರ(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)itmp, &st) >= 0)
			ಮುಂದುವರಿ;
#  endif
#  ಆದರೆ defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  ಆದರೆ defined(UNIX) || defined(VMS)
		    (ಶೂನ್ಯ)umask(umask_save);
#  endif
		    ಆದರೆ (r == 0)
		    {
			vim_settempdir(itmp);
			ನಿಲ್ಲಿಸು;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    ಆದರೆ (errno != EEXIST)
#  endif
			ನಿಲ್ಲಿಸು;
		}
# endif // HAVE_MKDTEMP
		ಆದರೆ (vim_tempdir != ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (vim_tempdir != ಖಾಲಿ)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	ಪೂರ್ಣಾಂಕ itmplen = vim_snprintf((ಅಕ್ಷರ *)itmp, ಗಾತ್ರ(itmp), "%s%ld", vim_tempdir, ತಾತ್ಕಾಲಿಕ_ಎಣಿಕೆ++);
	ಹಿಂತಿರುಗಿ vim_strnsave(itmp, (size_t)itmplen);
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ಇಲ್ಲದಿದ್ದರೆ // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wsz_ತಾತ್ಕಾಲಿಕ_File[MAX_ಮಾರ್ಗ_2 + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    ದೀರ್ಘ	i;

    wcscpy(itmp, L"");
    ಆದರೆ (Get_ತಾತ್ಕಾಲಿಕ_ಮಾರ್ಗ_W(MAX_ಮಾರ್ಗ_2, wsz_ತಾತ್ಕಾಲಿಕ_File) == 0)
    {
	wsz_ತಾತ್ಕಾಲಿಕ_File[0] = L'.';	// GetTempPathW() failed, use current dir
	wsz_ತಾತ್ಕಾಲಿಕ_File[1] = L'\\';
	wsz_ತಾತ್ಕಾಲಿಕ_File[2] = NUL;
    }
    wcscpy(buf4, L"VIM");

    // randomize the name to avoid collisions
    i = mch_get_pid() + extra_char;
    buf4[1] = chartab[i % 36];
    buf4[2] = chartab[(i / 36) % 36];
    ಆದರೆ (Get_ತಾತ್ಕಾಲಿಕ_File_Name_W(wsz_ತಾತ್ಕಾಲಿಕ_File, buf4, 0, itmp) == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(ಶೂನ್ಯ)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_ಗಮ್ಯ_enc(itmp, ಖಾಲಿ);
    ಆದರೆ (retval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    shname = gettail(p_sh);
    ಆದರೆ ((*p_shcf == '-' && !(ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)shname, "powershell") != ಖಾಲಿ
			     || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)shname, "pwsh") != ಖಾಲಿ ))
								    || p_ssl)
	ಗಾಗಿ (p = retval; *p; ++p)
	    ಆದರೆ (*p == '\\')
		*p = '/';
    ಹಿಂತಿರುಗಿ retval;

# ಇಲ್ಲದಿದ್ದರೆ // MSWIN
    ಪೂರ್ಣಾಂಕ		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((ಅಕ್ಷರ *)itmp);
    ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#  ಇಲ್ಲದಿದ್ದರೆ

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((ಅಕ್ಷರ *)itmp, ಗಾತ್ರ(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (ಅಕ್ಷರ *)itmp);
    ಆದರೆ (p != ಖಾಲಿ)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((ಅಕ್ಷರ *)itmp, ಗಾತ್ರ(itmp), "%s.txt", p);
	ಬಿಡುಗಡೆ(p);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#   ಇಲ್ಲದಿದ್ದರೆ
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    ಆದರೆ ((p = vim_strchr(itmp, '?')) != ಖಾಲಿ)
	*p = extra_char;
    ಆದರೆ (mktemp((ಅಕ್ಷರ *)itmp) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#   endif
#  endif

    ಹಿಂತಿರುಗಿ vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#ಆದರೆ_ಅಂತ್ಯ // TEMPDIRNAMES
}

#ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    ಶೂನ್ಯ
forward_slash(char_u *fname)
{
    char_u	*p;

    ಆದರೆ (ಮಾರ್ಗ_with_url(fname))
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	ಆದರೆ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\\')
	    *p = '/';
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Try matching a filename with a "pattern" ("prog" is NULL), or use the
 * precompiled regprog "prog" ("pattern" is NULL).  That avoids calling
 * vim_regcomp() often.
 * Used for autocommands and 'wildignore'.
 * Returns TRUE if there is a match, FALSE otherwise.
 */
    ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_file_pat(
    char_u	*ನಮೂನೆ,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*ಬಾಲ,			// tail of path
    ಪೂರ್ಣಾಂಕ		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    ಆದರೆ (prog != ಖಾಲಿ)
	regmatch.regprog = *prog;
    ಇಲ್ಲದಿದ್ದರೆ
	regmatch.regprog = vim_regcomp(ನಮೂನೆ, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    ಆದರೆ (regmatch.regprog != ಖಾಲಿ
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != ಖಾಲಿ
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, ಬಾಲ, (colnr_T)0))))
	ಫಲಿತಾಂಶ = TRUE;

    ಆದರೆ (prog != ಖಾಲಿ)
	*prog = regmatch.regprog;
    ಇಲ್ಲದಿದ್ದರೆ
	vim_regfree(regmatch.regprog);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*ಬಾಲ;
    char_u	*regpat;
    ಅಕ್ಷರ	allow_dirs;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;
    char_u	*p;

    ಬಾಲ = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    ಆಗ (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_ಗಮ್ಯ_reg_pat(buf, ಖಾಲಿ, &allow_dirs, FALSE);
	ಆದರೆ (regpat == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ_file_pat(regpat, ಖಾಲಿ, ffname, sfname,
						       ಬಾಲ, (ಪೂರ್ಣಾಂಕ)allow_dirs);
	vim_free(regpat);
	ಆದರೆ (ಹೊಂದಿಕೆ)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
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
file_pat_ಗಮ್ಯ_reg_pat(
    char_u	*pat,
    char_u	*pat_ಕೊನೆ,	// first char after pattern or NULL
    ಅಕ್ಷರ	*allow_dirs,	// Result passed back out in here
    ಪೂರ್ಣಾಂಕ		no_bslash ಬಳಸದ) // Don't use a backward slash as pathsep
{
    ಪೂರ್ಣಾಂಕ		ಗಾತ್ರ_2 = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		nested = 0;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_dollar = TRUE;

    ಆದರೆ (allow_dirs != ಖಾಲಿ)
	*allow_dirs = FALSE;
    ಆದರೆ (pat_ಕೊನೆ == ಖಾಲಿ)
	pat_ಕೊನೆ = pat + STRLEN(pat);

    ಗಾಗಿ (p = pat; p < pat_ಕೊನೆ; p++)
    {
	ಆಯ್ಕೆ (*p)
	{
	    ಸ್ಥಿತಿ '*':
	    ಸ್ಥಿತಿ '.':
	    ಸ್ಥಿತಿ ',':
	    ಸ್ಥಿತಿ '{':
	    ಸ್ಥಿತಿ '}':
	    ಸ್ಥಿತಿ '~':
		ಗಾತ್ರ_2 += 2;	// extra backslash
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    ಸ್ಥಿತಿ '\\':
	    ಸ್ಥಿತಿ '/':
		ಗಾತ್ರ_2 += 4;	// could become "[\/]"
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಗಾತ್ರ_2++;
		ಆದರೆ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++ಗಾತ್ರ_2;
		}
		ನಿಲ್ಲಿಸು;
	}
    }
    reg_pat = alloc(ಗಾತ್ರ_2 + 1);
    ಆದರೆ (reg_pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    i = 0;

    ಆದರೆ (pat[0] == '*')
	ಆಗ (pat[0] == '*' && pat < pat_ಕೊನೆ - 1)
	    pat++;
    ಇಲ್ಲದಿದ್ದರೆ
	reg_pat[i++] = '^';
    endp = pat_ಕೊನೆ - 1;
    ಆದರೆ (endp >= pat && *endp == '*')
    {
	ಆಗ (endp - pat > 0 && *endp == '*')
	    endp--;
	ಕೂಡಿಸು_dollar = FALSE;
    }
    ಗಾಗಿ (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	ಆಯ್ಕೆ (*p)
	{
	    ಸ್ಥಿತಿ '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		ಆಗ (p[1] == '*')	// "**" matches like "*"
		    ++p;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '.':
	    ಸ್ಥಿತಿ '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '?':
		reg_pat[i++] = '.';
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '\\':
		ಆದರೆ (p[1] == NUL)
		    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
		ಆದರೆ (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    ಆದರೆ ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			ಆದರೆ (allow_dirs != ಖಾಲಿ)
			    *allow_dirs = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		}
#ಆದರೆ_ಅಂತ್ಯ
		// Undo escaping from ExpandEscape():
		// foo\?bar -> foo?bar
		// foo\%bar -> foo%bar
		// foo\,bar -> foo,bar
		// foo\ bar -> foo bar
		// Don't unescape \, * and others that are also special in a
		// regexp.
		// An escaped { must be unescaped since we use magic not
		// verymagic.  Use "\\\{n,m\}"" to get "\{n,m}".
		ಆದರೆ (*++p == '?'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
			&& no_bslash
#ಆದರೆ_ಅಂತ್ಯ
			)
		    reg_pat[i++] = '?';
		ಇಲ್ಲದಿದ್ದರೆ
		    ಆದರೆ (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (allow_dirs != ಖಾಲಿ && vim_ispathsep(*p)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
				&& (!no_bslash || *p != '\\')
#ಆದರೆ_ಅಂತ್ಯ
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			ಆದರೆ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    ಸ್ಥಿತಿ '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		ಆದರೆ (allow_dirs != ಖಾಲಿ)
		    *allow_dirs = TRUE;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ',':
		ಆದರೆ (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    reg_pat[i++] = ',';
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಆದರೆ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (allow_dirs != ಖಾಲಿ && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (ಕೂಡಿಸು_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    ಆದರೆ (nested != 0)
    {
	ಆದರೆ (nested < 0)
	    emsg(_(e_missing_open_curly));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    ಹಿಂತಿರುಗಿ reg_pat;
}

#ಆದರೆ defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    ದೀರ್ಘ
read_eintr(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, size_t bufsize)
{
    ದೀರ್ಘ ret;

    ಗಾಗಿ (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	ಆದರೆ (ret >= 0 || errno != EINTR)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    ದೀರ್ಘ
write_eintr(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, size_t bufsize)
{
    ದೀರ್ಘ    ret = 0;
    ದೀರ್ಘ    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    ಆಗ (ret < (ದೀರ್ಘ)bufsize)
    {
	wlen = vim_write(fd, (ಅಕ್ಷರ *)buf + ret, bufsize - ret);
	ಆದರೆ (wlen < 0)
	{
	    ಆದರೆ (errno != EINTR)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ret += wlen;
    }
    ಹಿಂತಿರುಗಿ ret;
}
#ಆದರೆ_ಅಂತ್ಯ
