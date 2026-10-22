/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * getchar.c: Code related to getting a character from the user or a script
 * file, manipulations with redo buffer and stuff buffer.
 */

#অন্তর্ভাব "vim.h"

/*
 * These buffers are used for storing:
 * - stuffed characters: A command that is translated into another command.
 * - redo characters: will redo the last change.
 * - recorded characters: for the "q" command.
 *
 * The bytes are stored like in the typeahead buffer:
 * - K_SPECIAL introduces a special key (two more bytes follow).  A literal
 *   K_SPECIAL is stored as K_SPECIAL KS_SPECIAL KE_FILLER.
 * - CSI introduces a GUI termcap code (also when gui.in_use is FALSE,
 *   otherwise switching the GUI on would make mappings invalid).
 *   A literal CSI is stored as CSI KS_EXTRA KE_CSI.
 * These translations are also done on multi-byte characters!
 *
 * Escaping CSI bytes is done by the system-specific input functions, called
 * by ui_inchar().
 * Escaping K_SPECIAL is done by inchar().
 * Un-escaping is done by vgetc().
 */

#define MINIMAL_আকার 20			// minimal size for b_str

স্থির buffheader_T redobuff = {{ফাঁকা, 0, {NUL}}, ফাঁকা, 0, 0, FALSE};
স্থির buffheader_T old_redobuff = {{ফাঁকা, 0, {NUL}}, ফাঁকা, 0, 0, FALSE};
স্থির buffheader_T recordbuff = {{ফাঁকা, 0, {NUL}}, ফাঁকা, 0, 0, FALSE};

স্থির পূর্ণসংখ্যা typeahead_char = 0;		// typeahead char that's not flushed

#ifdef FEAT_EVAL
স্থির char_u typedchars[MAXMAPLEN + 1] = { NUL };  // typed chars before map
স্থির পূর্ণসংখ্যা typedchars_pos = 0;
#endif

/*
 * When block_redo is TRUE the redo buffer will not be changed.
 * Used by edit() to repeat insertions.
 */
স্থির পূর্ণসংখ্যা	block_redo = FALSE;

স্থির পূর্ণসংখ্যা	চাবি_Noremap = 0;	    // remapping flags

/*
 * Variables used by vgetorpeek() and flush_buffers().
 *
 * typebuf.tb_buf[] contains all characters that are not consumed yet.
 * typebuf.tb_buf[typebuf.tb_off] is the first valid character.
 * typebuf.tb_buf[typebuf.tb_off + typebuf.tb_len - 1] is the last valid char.
 * typebuf.tb_buf[typebuf.tb_off + typebuf.tb_len] must be NUL.
 * The head of the buffer may contain the result of mappings, abbreviations
 * and @a commands.  The length of this part is typebuf.tb_maplen.
 * typebuf.tb_silent is the part where <silent> applies.
 * After the head are characters that come from the terminal.
 * typebuf.tb_no_abbr_cnt is the number of characters in typebuf.tb_buf that
 * should not be considered for abbreviations.
 * Some parts of typebuf.tb_buf may not be mapped. These parts are remembered
 * in typebuf.tb_noremap[], which is the same length as typebuf.tb_buf and
 * contains RM_NONE for the characters that are not to be remapped.
 * typebuf.tb_noremap[typebuf.tb_off] is the first valid flag.
 * (typebuf has been put in globals.h, because check_termcode() needs it).
 */
#define RM_YES		0	// tb_noremap: remap
#define RM_NONE		1	// tb_noremap: don't remap
#define RM_SCRIPT	2	// tb_noremap: remap local script mappings
#define RM_ABBR		4	// tb_noremap: don't remap, do abbrev.
#define RM_SIMPLIFIED	8	// tb_noremap: modifiers merged into the key

// typebuf.tb_buf has three parts: room in front (for result of mappings), the
// middle for typeahead and room for new characters (which needs to be 3 *
// MAXMAPLEN for the Amiga).
#ifdef FEAT_NORMAL
// Add extra space to handle large OSC responses in bigger chunks (improve
// performance)
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3) + 2048)
#অন্যথা // Tiny version
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
#endif
স্থির char_u	typebuf_init[TYPELEN_INIT];	// initial typebuf.tb_buf
স্থির char_u	noremapbuf_init[TYPELEN_INIT];	// initial typebuf.tb_noremap

স্থির size_t	last_recorded_len = 0;	// number of last recorded chars

#ifdef FEAT_EVAL
mapblock_T	*last_used_map = ফাঁকা;
পূর্ণসংখ্যা		last_used_sid = -1;
#endif

স্থির পূর্ণসংখ্যা	read_readbuf(buffheader_T *buf, পূর্ণসংখ্যা advance);
স্থির শূন্য	init_typebuf(শূন্য);
স্থির শূন্য	may_sync_undo(শূন্য);
স্থির শূন্য	free_typebuf(শূন্য);
স্থির শূন্য	closescript(শূন্য);
স্থির শূন্য	updatescript(পূর্ণসংখ্যা c);
স্থির পূর্ণসংখ্যা	vgetorpeek(পূর্ণসংখ্যা);
স্থির পূর্ণসংখ্যা	inchar(char_u *buf, পূর্ণসংখ্যা maxlen, long wait_time);
#ifdef FEAT_EVAL
স্থির পূর্ণসংখ্যা	do_চাবি_প্রবেশ_pre(পূর্ণসংখ্যা c);
#endif

/*
 * Free and clear a buffer.
 */
    স্থির শূন্য
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    জন্য (p = buf->bh_first.b_পরবর্তী; p != ফাঁকা; p = np)
    {
	np = p->b_পরবর্তী;
	vim_free(p);
    }
    buf->bh_first.b_পরবর্তী = ফাঁকা;
    buf->bh_curr = ফাঁকা;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    স্থির char_u *
get_buffcont(
    buffheader_T	*বাফার,
    পূর্ণসংখ্যা			dozero,	    // count == zero is not an error
    size_t		*len)	    // the length of the returned buffer
{
    long_u	    গুনতি = 0;
    char_u	    *p = ফাঁকা;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;
    size_t	    i = 0;

    // compute the total length of the string
    জন্য (bp = বাফার->bh_first.b_পরবর্তী; bp != ফাঁকা; bp = bp->b_পরবর্তী)
	গুনতি += (long_u)bp->b_strlen;

    যদি ((গুনতি > 0 || dozero) && (p = alloc(গুনতি + 1)) != ফাঁকা)
    {
	p2 = p;
	জন্য (bp = বাফার->bh_first.b_পরবর্তী; bp != ফাঁকা; bp = bp->b_পরবর্তী)
	    জন্য (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
	i = (size_t)(p2 - p);
    }

    যদি (len != ফাঁকা)
	*len = i;

    ফেরত p;
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(শূন্য)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE, &len);
    যদি (p == ফাঁকা)
	ফেরত ফাঁকা;

    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    যদি (len >= last_recorded_len)
    {
	len -= last_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    যদি (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    ফেরত (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    string_T
get_inserted(শূন্য)
{
    size_t len = 0;
    char_u *str = get_buffcont(&redobuff, FALSE, &len);
    string_T ret = { str, len };
    ফেরত ret;
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    স্থির শূন্য
add_buff(
    buffheader_T	*buf,
    char_u		*s,
    long		slen)	// length of "s" or -1
{
    যদি (slen < 0)
	slen = (long)STRLEN(s);
    যদি (slen == 0)				// don't add empty strings
	ফেরত;

    যদি (buf->bh_first.b_পরবর্তী == ফাঁকা)	// first add to list
    {
	buf->bh_curr = &(buf->bh_first);
	buf->bh_create_newblock = TRUE;
    }
    অন্যথা যদি (buf->bh_curr == ফাঁকা)	// buffer has already been read
    {
	iemsg(e_add_to_internal_বাফার_that_was_already_read_from);
	ফেরত;
    }
    অন্যথা যদি (buf->bh_সূচক != 0)
    {
	mch_memmove(buf->bh_first.b_পরবর্তী->b_str,
		    buf->bh_first.b_পরবর্তী->b_str + buf->bh_সূচক,
		    (buf->bh_first.b_পরবর্তী->b_strlen - buf->bh_সূচক) + 1);
	buf->bh_first.b_পরবর্তী->b_strlen -= buf->bh_সূচক;
	buf->bh_space += buf->bh_সূচক;
    }
    buf->bh_সূচক = 0;

    যদি (!buf->bh_create_newblock && buf->bh_space >= (পূর্ণসংখ্যা)slen)
    {
	vim_strncpy(buf->bh_curr->b_str + buf->bh_curr->b_strlen, s, (size_t)slen);
	buf->bh_curr->b_strlen += slen;
	buf->bh_space -= slen;
    }
    অন্যথা
    {
	long_u	    len;
	buffblock_T *p;

	যদি (slen < MINIMAL_আকার)
	    len = MINIMAL_আকার;
	অন্যথা
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	যদি (p == ফাঁকা)
	    ফেরত; // no space, just forget it
	vim_strncpy(p->b_str, s, (size_t)slen);
	p->b_strlen = slen;
	buf->bh_space = (পূর্ণসংখ্যা)(len - slen);
	buf->bh_create_newblock = FALSE;

	p->b_পরবর্তী = buf->bh_curr->b_পরবর্তী;
	buf->bh_curr->b_পরবর্তী = p;
	buf->bh_curr = p;
    }
}

/*
 * Delete "slen" bytes from the end of "buf".
 * Only works when it was just added.
 */
    স্থির শূন্য
delete_buff_লেজ(buffheader_T *buf, পূর্ণসংখ্যা slen)
{
    যদি (buf->bh_curr == ফাঁকা)
	ফেরত;  // nothing to delete
    যদি (buf->bh_curr->b_strlen < (size_t)slen)
	ফেরত;

    buf->bh_curr->b_str[buf->bh_curr->b_strlen - (size_t)slen] = NUL;
    buf->bh_curr->b_strlen -= slen;
    buf->bh_space += slen;
}

/*
 * Add number "n" to buffer "buf".
 */
    স্থির শূন্য
add_অঙ্ক_buff(buffheader_T *buf, long n)
{
    char_u	সংখ্যা[32];
    পূর্ণসংখ্যা		numberlen;

    numberlen = vim_snprintf((অক্ষর *)সংখ্যা, আকার(সংখ্যা), "%ld", n);
    add_buff(buf, সংখ্যা, (long)numberlen);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    স্থির শূন্য
add_char_buff(buffheader_T *buf, পূর্ণসংখ্যা c)
{
    char_u	bytes[MB_MAXBYTES + 1];
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		i;
    char_u	অস্থায়ী[4];
    long	templen;

    যদি (IS_SPECIAL(c))
	len = 1;
    অন্যথা
	len = (*mb_char2bytes)(c, bytes);
    জন্য (i = 0; i < len; ++i)
    {
	যদি (!IS_SPECIAL(c))
	    c = bytes[i];

	যদি (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    অস্থায়ী[0] = K_SPECIAL;
	    অস্থায়ী[1] = K_SECOND(c);
	    অস্থায়ী[2] = K_THIRD(c);
	    অস্থায়ী[3] = NUL;
	    templen = 3;
	}
#ifdef FEAT_GUI
	অন্যথা যদি (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    অস্থায়ী[0] = CSI;
	    অস্থায়ী[1] = KS_EXTRA;
	    অস্থায়ী[2] = (পূর্ণসংখ্যা)KE_CSI;
	    অস্থায়ী[3] = NUL;
	    templen = 3;
	}
#endif
	অন্যথা
	{
	    অস্থায়ী[0] = c;
	    অস্থায়ী[1] = NUL;
	    templen = 1;
	}
	add_buff(buf, অস্থায়ী, templen);
    }
}

// First read ahead buffer. Used for translated commands.
স্থির buffheader_T readbuf1 = {{ফাঁকা, 0, {NUL}}, ফাঁকা, 0, 0, FALSE};

// Second read ahead buffer. Used for redo.
স্থির buffheader_T readbuf2 = {{ফাঁকা, 0, {NUL}}, ফাঁকা, 0, 0, FALSE};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    স্থির পূর্ণসংখ্যা
read_readbuffers(পূর্ণসংখ্যা advance)
{
    পূর্ণসংখ্যা c;

    c = read_readbuf(&readbuf1, advance);
    যদি (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    ফেরত c;
}

    স্থির পূর্ণসংখ্যা
read_readbuf(buffheader_T *buf, পূর্ণসংখ্যা advance)
{
    char_u	c;
    buffblock_T	*curr;

    যদি (buf->bh_first.b_পরবর্তী == ফাঁকা)  // buffer is empty
	ফেরত NUL;

    curr = buf->bh_first.b_পরবর্তী;
    c = curr->b_str[buf->bh_সূচক];

    যদি (advance)
    {
	যদি (curr->b_str[++buf->bh_সূচক] == NUL)
	{
	    buf->bh_first.b_পরবর্তী = curr->b_পরবর্তী;
	    vim_free(curr);
	    buf->bh_সূচক = 0;
	}
    }
    ফেরত c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    স্থির শূন্য
start_stuff(শূন্য)
{
    যদি (readbuf1.bh_first.b_পরবর্তী != ফাঁকা)
    {
	readbuf1.bh_curr = &(readbuf1.bh_first);
	readbuf1.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
    যদি (readbuf2.bh_first.b_পরবর্তী != ফাঁকা)
    {
	readbuf2.bh_curr = &(readbuf2.bh_first);
	readbuf2.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    পূর্ণসংখ্যা
stuff_empty(শূন্য)
{
    ফেরত (readbuf1.bh_first.b_পরবর্তী == ফাঁকা
	 && readbuf2.bh_first.b_পরবর্তী == ফাঁকা);
}

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * readbuf2.
 */
    পূর্ণসংখ্যা
readbuf1_empty(শূন্য)
{
    ফেরত (readbuf1.bh_first.b_পরবর্তী == ফাঁকা);
}
#endif

/*
 * Set a typeahead character that won't be flushed.
 */
    শূন্য
typeahead_noflush(পূর্ণসংখ্যা c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    শূন্য
flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    start_stuff();
    যখন (read_readbuffers(TRUE) != NUL)
	;

    যদি (flush_typeahead == FLUSH_MINIMAL)
    {
	// remove mapped characters at the start only,
	// but only when enough space left in typebuf
	যদি (typebuf.tb_off + typebuf.tb_maplen >= typebuf.tb_buflen)
	{
	    typebuf.tb_off = MAXMAPLEN;
	    typebuf.tb_len = 0;
	}
	অন্যথা
	{
	    typebuf.tb_off += typebuf.tb_maplen;
	    typebuf.tb_len -= typebuf.tb_maplen;
	}
#যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	যদি (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#endif
    }
    অন্যথা
    {
	// remove typeahead
	যদি (flush_typeahead == FLUSH_প্রবেশ)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    যখন (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#endif
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
    যদি (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * The previous contents of the redo buffer is kept in old_redobuffer.
 * This is used for the CTRL-O <.> command in insert mode.
 */
    শূন্য
ResetRedobuff(শূন্য)
{
    যদি (block_redo)
	ফেরত;

    free_buff(&old_redobuff);
    old_redobuff = redobuff;
    redobuff.bh_first.b_পরবর্তী = ফাঁকা;
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    শূন্য
CancelRedo(শূন্য)
{
    যদি (block_redo)
	ফেরত;

    free_buff(&redobuff);
    redobuff = old_redobuff;
    old_redobuff.bh_first.b_পরবর্তী = ফাঁকা;
    start_stuff();
    যখন (read_readbuffers(TRUE) != NUL)
	;
}

/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
    শূন্য
saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;
    size_t	slen;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_first.b_পরবর্তী = ফাঁকা;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_first.b_পরবর্তী = ফাঁকা;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE, &slen);
    যদি (s == ফাঁকা)
	ফেরত;

    add_buff(&redobuff, s, (long)slen);
    vim_free(s);
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    শূন্য
restoreRedobuff(save_redo_T *save_redo)
{
    free_buff(&redobuff);
    redobuff = save_redo->sr_redobuff;
    free_buff(&old_redobuff);
    old_redobuff = save_redo->sr_old_redobuff;
}

/*
 * Append "s" to the redo buffer.
 * K_SPECIAL and CSI should already have been escaped.
 */
    শূন্য
AppendToRedobuff(char_u *s)
{
    যদি (!block_redo)
	add_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    শূন্য
AppendToRedobuffLit(
    char_u	*str,
    পূর্ণসংখ্যা		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    পূর্ণসংখ্যা		c;
    char_u	*start;

    যদি (block_redo)
	ফেরত;

    যখন (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	start = s;
	যখন (*s >= ' ' && *s < DEL && (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	যদি (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	যদি (s > start)
	    add_buff(&redobuff, start, (long)(s - start));

	যদি (*s == NUL || (len >= 0 && s - str >= len))
	    বিরতি;

	// Handle a special or multibyte character.
	যদি (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	অন্যথা
	    c = *s++;
	যদি (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    add_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048
	যদি (*s == NUL && c == '0')
	    add_buff(&redobuff, (char_u *)"048", 3L);
	অন্যথা
	    add_char_buff(&redobuff, c);
    }
}

/*
 * Append "s" to the redo buffer, leaving 3-byte special key codes unmodified
 * and escaping other K_SPECIAL and CSI bytes.
 */
    শূন্য
AppendToRedobuffSpec(char_u *s)
{
    যদি (block_redo)
	ফেরত;

    যখন (*s != NUL)
    {
	যদি (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    add_buff(&redobuff, s, 3L);
	    s += 3;
	}
	অন্যথা
	    add_char_buff(&redobuff, mb_cptr2char_adv(&s));
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    শূন্য
AppendCharToRedobuff(পূর্ণসংখ্যা c)
{
    যদি (!block_redo)
	add_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    শূন্য
Append_সংখ্যা_To_Redobuff(long n)
{
    যদি (!block_redo)
	add_অঙ্ক_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    শূন্য
stuffReadbuff(char_u *s)
{
    add_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    শূন্য
stuffRedoReadbuff(char_u *s)
{
    add_buff(&readbuf2, s, -1L);
}

    শূন্য
stuffReadbuffLen(char_u *s, long len)
{
#ifdef FEAT_EVAL
    যদি (add_last_ঢোকাও == 1) // Only add if this is the first call, for
			      // recursive calls, ignore.
	ga_concat_len(&last_ঢোকাও_ga, s, (size_t)len);
#endif
    add_buff(&readbuf1, s, len);
}

#যদি defined(FEAT_EVAL)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    শূন্য
stuffReadbuffSpec(char_u *s)
{
    পূর্ণসংখ্যা c;

    যখন (*s != NUL)
    {
	যদি (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	অন্যথা
	{
	    c = mb_cptr2char_adv(&s);
	    যদি (c == CAR || c == NL || c == ESC)
		c = ' ';
	    stuffcharReadbuff(c);
	}
    }
}
#endif

/*
 * Append a character to the stuff buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    শূন্য
stuffcharReadbuff(পূর্ণসংখ্যা c)
{
    add_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    শূন্য
stuffnumReadbuff(long n)
{
    add_অঙ্ক_buff(&readbuf1, n);
}

/*
 * Stuff a string into the typeahead buffer, such that edit() will insert it
 * literally ("literally" TRUE) or interpret is as typed characters.
 */
    শূন্য
stuffescaped(char_u *arg, পূর্ণসংখ্যা literally)
{
    পূর্ণসংখ্যা		c;
    char_u	*start;

    যখন (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	start = arg;
	যখন ((*arg >= ' ' && *arg < DEL)
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	যদি (arg > start)
	    stuffReadbuffLen(start, (long)(arg - start));

	// stuff a single special character
	যদি (*arg != NUL)
	{
	    যদি (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    অন্যথা
		c = *arg++;
	    যদি (literally && ((c < ' ' && c != TAB) || c == DEL))
		stuffcharReadbuff(Ctrl_V);
	    stuffcharReadbuff(c);
	}
    }
}

/*
 * Read a character from the redo buffer.  Translates K_SPECIAL, CSI and
 * multibyte characters.
 * The redo buffer is left as it is.
 * If init is TRUE, prepare for redo, return FAIL if nothing to redo, OK
 * otherwise.
 * If old is TRUE, use old_redobuff instead of redobuff.
 */
    স্থির পূর্ণসংখ্যা
read_redo(পূর্ণসংখ্যা init, পূর্ণসংখ্যা old_redo)
{
    স্থির buffblock_T	*bp;
    স্থির char_u	*p;
    পূর্ণসংখ্যা			c;
    পূর্ণসংখ্যা			n;
    char_u		buf[MB_MAXBYTES + 1];
    পূর্ণসংখ্যা			i;

    যদি (init)
    {
	যদি (old_redo)
	    bp = old_redobuff.bh_first.b_পরবর্তী;
	অন্যথা
	    bp = redobuff.bh_first.b_পরবর্তী;
	যদি (bp == ফাঁকা)
	    ফেরত FAIL;
	p = bp->b_str;
	ফেরত OK;
    }
    যদি ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	যদি (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	অন্যথা
	    n = 1;
	জন্য (i = 0; ; ++i)
	{
	    যদি (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = TO_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#ifdef FEAT_GUI
	    যদি (c == CSI)	// escaped CSI
		p += 2;
#endif
	    যদি (*++p == NUL && bp->b_পরবর্তী != ফাঁকা)
	    {
		bp = bp->b_পরবর্তী;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    যদি (i == n - 1)	// last byte of a character
	    {
		যদি (n != 1)
		    c = (*mb_ptr2char)(buf);
		বিরতি;
	    }
	    c = *p;
	    যদি (c == NUL)	// cannot happen?
		বিরতি;
	}
    }

    ফেরত c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    স্থির শূন্য
copy_redo(পূর্ণসংখ্যা old_redo)
{
    পূর্ণসংখ্যা	    c;

    যখন ((c = read_redo(FALSE, old_redo)) != NUL)
	add_char_buff(&readbuf2, c);
}

/*
 * Stuff the redo buffer into readbuf2.
 * Insert the redo count into the command.
 * If "old_redo" is TRUE, the last but one command is repeated
 * instead of the last command (inserting text). This is used for
 * CTRL-O <.> in insert mode
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
start_redo(long গুনতি, পূর্ণসংখ্যা old_redo)
{
    পূর্ণসংখ্যা	    c;

    // init the pointers; return if nothing to redo
    যদি (read_redo(TRUE, old_redo) == FAIL)
	ফেরত FAIL;

    c = read_redo(FALSE, old_redo);

#ifdef FEAT_EVAL
    যদি (c == K_SID)
    {
	// Copy the <SID>{sid}; sequence
	add_char_buff(&readbuf2, c);
	জন্য (;;)
	{
	    c = read_redo(FALSE, old_redo);
	    add_char_buff(&readbuf2, c);
	    যদি (!SAFE_isdigit(c))
		বিরতি;
	}
	c = read_redo(FALSE, old_redo);
    }
#endif

    // copy the buffer name, if present
    যদি (c == '"')
    {
	add_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	// if a numbered buffer is used, increment the number
	যদি (c >= '1' && c < '9')
	    ++c;
	add_char_buff(&readbuf2, c);

	// the expression register should be re-evaluated
	যদি (c == '=')
	{
	    add_char_buff(&readbuf2, CAR);
	    cmd_silent = TRUE;
	}

	c = read_redo(FALSE, old_redo);
    }

    যদি (c == 'v')   // redo Visual
    {
	VIsual = curwin->w_cursor;
	VIsual_active = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    // try to enter the count (in place of a previous count)
    যদি (গুনতি)
    {
	যখন (VIM_ISDIGIT(c))	// skip "old" count
	    c = read_redo(FALSE, old_redo);
	add_অঙ্ক_buff(&readbuf2, গুনতি);
    }

    // copy the rest from the redo buffer into the stuff buffer
    add_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    ফেরত OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
start_redo_ins(শূন্য)
{
    পূর্ণসংখ্যা	    c;

    যদি (read_redo(TRUE, FALSE) == FAIL)
	ফেরত FAIL;
    start_stuff();

    // skip the count and the command character
    যখন ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	যদি (vim_strchr((char_u *)"AaIiRrOo", c) != ফাঁকা)
	{
	    যদি (c == 'O' || c == 'o')
		add_buff(&readbuf2, NL_STR, -1L);
	    বিরতি;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    block_redo = TRUE;
    ফেরত OK;
}

    শূন্য
stop_redo_ins(শূন্য)
{
    block_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    স্থির শূন্য
init_typebuf(শূন্য)
{
    যদি (typebuf.tb_buf != ফাঁকা)
	ফেরত;

    typebuf.tb_buf = typebuf_init;
    typebuf.tb_noremap = noremapbuf_init;
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_len = 0;
    typebuf.tb_off = MAXMAPLEN + 4;
    typebuf.tb_change_cnt = 1;
}

/*
 * Returns TRUE when keys cannot be remapped.
 */
    পূর্ণসংখ্যা
noremap_চাবিগুলি(শূন্য)
{
    ফেরত চাবি_Noremap & (RM_NONE|RM_SCRIPT);
}

/*
 * Insert a string in position 'offset' in the typeahead buffer (for "@r"
 * and ":normal" command, vgetorpeek() and check_termcode()).
 *
 * If "noremap" is REMAP_YES, new string can be mapped again.
 * If "noremap" is REMAP_NONE, new string cannot be mapped again.
 * If "noremap" is REMAP_SKIP, first char of new string cannot be mapped again,
 * but abbreviations are allowed.
 * If "noremap" is REMAP_SCRIPT, new string cannot be mapped again, except for
 *			script-local mappings.
 * If "noremap" is > 0, that many characters of the new string cannot be mapped.
 *
 * If "nottyped" is TRUE, the string does not return KeyTyped (don't use when
 * "offset" is non-zero!).
 *
 * If "silent" is TRUE, cmd_silent is set when the characters are obtained.
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
ins_typebuf(
    char_u	*str,
    পূর্ণসংখ্যা		noremap,
    পূর্ণসংখ্যা		offset,
    পূর্ণসংখ্যা		nottyped,
    পূর্ণসংখ্যা		silent)
{
    char_u	*s1, *s2;
    পূর্ণসংখ্যা		newlen;
    পূর্ণসংখ্যা		addlen;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		newoff;
    পূর্ণসংখ্যা		val;
    পূর্ণসংখ্যা		nrm;

    init_typebuf();
    যদি (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
    state_no_longer_safe("ins_typebuf()");

    addlen = (পূর্ণসংখ্যা)STRLEN(str);

    যদি (offset == 0 && addlen <= typebuf.tb_off)
    {
	/*
	 * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
	 */
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    অন্যথা যদি (typebuf.tb_len == 0 && typebuf.tb_buflen
					       >= addlen + 3 * (MAXMAPLEN + 4))
    {
	/*
	 * Buffer is empty and string fits in the existing buffer.
	 * Leave some space before and after, if possible.
	 */
	typebuf.tb_off = (typebuf.tb_buflen - addlen - 3 * (MAXMAPLEN + 4)) / 2;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    অন্যথা
    {
	পূর্ণসংখ্যা extra;

	/*
	 * Need to allocate a new buffer.
	 * In typebuf.tb_buf there must always be room for 3 * (MAXMAPLEN + 4)
	 * characters.  We add some extra room to avoid having to allocate too
	 * often.
	 */
	newoff = MAXMAPLEN + 4;
	extra = addlen + newoff + 4 * (MAXMAPLEN + 4);
	যদি (typebuf.tb_len > 2147483647 - extra)
	{
	    // string is getting too long for a 32 bit int
	    emsg(_(e_command_too_complex));    // also calls flush_buffers
	    setcursor();
	    ফেরত FAIL;
	}
	newlen = typebuf.tb_len + extra;
	s1 = alloc(newlen);
	যদি (s1 == ফাঁকা)		    // out of memory
	    ফেরত FAIL;
	s2 = alloc(newlen);
	যদি (s2 == ফাঁকা)		    // out of memory
	{
	    vim_free(s1);
	    ফেরত FAIL;
	}
	typebuf.tb_buflen = newlen;

	// copy the old chars, before the insertion point
	mch_memmove(s1 + newoff, typebuf.tb_buf + typebuf.tb_off,
							      (size_t)offset);
	// copy the new chars
	mch_memmove(s1 + newoff + offset, str, (size_t)addlen);
	// copy the old chars, after the insertion point, including the	NUL at
	// the end
	mch_memmove(s1 + newoff + offset + addlen,
				     typebuf.tb_buf + typebuf.tb_off + offset,
				       (size_t)(typebuf.tb_len - offset + 1));
	যদি (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)offset);
	mch_memmove(s2 + newoff + offset + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + offset,
					   (size_t)(typebuf.tb_len - offset));
	যদি (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    // If noremap == REMAP_SCRIPT: do remap script-local mappings.
    যদি (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    অন্যথা যদি (noremap == REMAP_SKIP)
	val = RM_ABBR;
    অন্যথা
	val = RM_NONE;

    /*
     * Adjust typebuf.tb_noremap[] for the new characters:
     * If noremap == REMAP_NONE or REMAP_SCRIPT: new characters are
     *			(sometimes) not remappable
     * If noremap == REMAP_YES: all the new characters are mappable
     * If noremap  > 0: "noremap" characters are not remappable, the rest
     *			mappable
     */
    যদি (noremap == REMAP_SKIP)
	nrm = 1;
    অন্যথা যদি (noremap < 0)
	nrm = addlen;
    অন্যথা
	nrm = noremap;
    জন্য (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + offset] =
						  (--nrm >= 0) ? val : RM_YES;

    // tb_maplen and tb_silent only remember the length of mapped and/or
    // silent mappings at the start of the buffer, assuming that a mapped
    // sequence doesn't result in typed characters.
    যদি (nottyped || typebuf.tb_maplen > offset)
	typebuf.tb_maplen += addlen;
    যদি (silent || typebuf.tb_silent > offset)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    যদি (typebuf.tb_no_abbr_cnt && offset == 0)	// and not used for abbrev.s
	typebuf.tb_no_abbr_cnt += addlen;

    ফেরত OK;
}

/*
 * Put character "c" back into the typeahead buffer.
 * Can be used for a character obtained by vgetc() that needs to be put back.
 * Uses cmd_silent, KeyTyped and KeyNoremap to restore the flags belonging to
 * the char.
 * Returns the length of what was inserted.
 */
    পূর্ণসংখ্যা
ins_char_typebuf(পূর্ণসংখ্যা c, পূর্ণসংখ্যা modifiers)
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    পূর্ণসংখ্যা		len = special_to_buf(c, modifiers, TRUE, buf);

    buf[len] = NUL;
    (শূন্য)ins_typebuf(buf, চাবি_Noremap, 0, !চাবি_Typed, cmd_silent);
    ফেরত len;
}

/*
 * Return TRUE if the typeahead buffer was changed (while waiting for a
 * character to arrive).  Happens when a message was received from a client or
 * from feedkeys().
 * But check in a more generic way to avoid trouble: When "typebuf.tb_buf"
 * changed it was reallocated and the old pointer can no longer be used.
 * Or "typebuf.tb_off" may have been changed and we would overwrite characters
 * that was just added.
 */
    পূর্ণসংখ্যা
typebuf_changed(
    পূর্ণসংখ্যা		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    ফেরত (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#endif
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    পূর্ণসংখ্যা
typebuf_typed(শূন্য)
{
    ফেরত typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    পূর্ণসংখ্যা
typebuf_maplen(শূন্য)
{
    ফেরত typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    শূন্য
del_typebuf(পূর্ণসংখ্যা len, পূর্ণসংখ্যা offset)
{
    পূর্ণসংখ্যা	    i;

    যদি (len == 0)
	ফেরত;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    যদি (offset == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    অন্যথা
    {
	i = typebuf.tb_off + offset;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	যদি (typebuf.tb_off > MAXMAPLEN)
	{
	    mch_memmove(typebuf.tb_buf + MAXMAPLEN,
			     typebuf.tb_buf + typebuf.tb_off, (size_t)offset);
	    mch_memmove(typebuf.tb_noremap + MAXMAPLEN,
			 typebuf.tb_noremap + typebuf.tb_off, (size_t)offset);
	    typebuf.tb_off = MAXMAPLEN;
	}
	// adjust typebuf.tb_buf (include the NUL at the end)
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + offset,
						     typebuf.tb_buf + i + len,
				       (size_t)(typebuf.tb_len - offset + 1));
	// adjust typebuf.tb_noremap[]
	mch_memmove(typebuf.tb_noremap + typebuf.tb_off + offset,
						 typebuf.tb_noremap + i + len,
					   (size_t)(typebuf.tb_len - offset));
    }

    যদি (typebuf.tb_maplen > offset)		// adjust tb_maplen
    {
	যদি (typebuf.tb_maplen < offset + len)
	    typebuf.tb_maplen = offset;
	অন্যথা
	    typebuf.tb_maplen -= len;
    }
    যদি (typebuf.tb_silent > offset)		// adjust tb_silent
    {
	যদি (typebuf.tb_silent < offset + len)
	    typebuf.tb_silent = offset;
	অন্যথা
	    typebuf.tb_silent -= len;
    }
    যদি (typebuf.tb_no_abbr_cnt > offset)	// adjust tb_no_abbr_cnt
    {
	যদি (typebuf.tb_no_abbr_cnt < offset + len)
	    typebuf.tb_no_abbr_cnt = offset;
	অন্যথা
	    typebuf.tb_no_abbr_cnt -= len;
    }

#যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#endif
    যদি (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * State for adding bytes to a recording or 'showcmd'.
 */
প্রকারলেখ কাঠামো
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    পূর্ণসংখ্যা		পূর্ব_c;
    size_t	buflen;
    unsigned	pending_special;
    unsigned	pending_mbyte;
} gotchars_state_T;

/*
 * Add a single byte to a recording or 'showcmd'.
 * Return TRUE if a full key has been received, FALSE otherwise.
 */
    স্থির পূর্ণসংখ্যা
gotchars_add_byte(gotchars_state_T *state, char_u byte)
{
    পূর্ণসংখ্যা		c = state->buf[state->buflen++] = byte;
    পূর্ণসংখ্যা		retval = FALSE;
    পূর্ণসংখ্যা		in_special = state->pending_special > 0;
    পূর্ণসংখ্যা		in_mbyte = state->pending_mbyte > 0;

    যদি (in_special)
	state->pending_special--;
    অন্যথা যদি (c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	    )
	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	state->pending_special = 2;

    যদি (state->pending_special > 0)
	যাও ret_false;

    যদি (in_mbyte)
	state->pending_mbyte--;
    অন্যথা
    {
	যদি (in_special)
	{
	    যদি (state->পূর্ব_c == KS_MODIFIER)
		// When receiving a modifier, wait for the modified key.
		যাও ret_false;
	    c = TO_SPECIAL(state->পূর্ব_c, c);
	    // Drop K_FOCUSGAINED, K_FOCUSLOST and K_COMPLETE_DELAY, they are
	    // not useful in a recording.
	    যদি (c == K_FOCUSGAINED || c == K_FOCUSLOST || c == K_COMPLETE_DELAY)
		state->buflen = 0;
	}
	// When receiving a multibyte character, store it until we have all
	// the bytes, so that it won't be split between two buffer blocks,
	// and delete_buff_tail() will work properly.
	state->pending_mbyte = MB_BYTE2LEN_CHECK(c) - 1;
    }

    যদি (state->pending_mbyte > 0)
	যাও ret_false;

    retval = TRUE;
ret_false:
    state->পূর্ব_c = c;
    ফেরত retval;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the record buffer.
 */
    স্থির শূন্য
gotchars(char_u *chars, পূর্ণসংখ্যা len)
{
    char_u		*s = chars;
    size_t		i;
    পূর্ণসংখ্যা			todo = len;
    স্থির gotchars_state_T state;

    যখন (todo-- > 0)
    {
	যদি (!gotchars_add_byte(&state, *s++))
	    চালু;

	// Handle one byte at a time; no translation to be done.
	জন্য (i = 0; i < state.buflen; ++i)
	    updatescript(state.buf[i]);

	যদি (reg_recording != 0)
	{
	    state.buf[state.buflen] = NUL;
	    add_buff(&recordbuff, state.buf, (long)state.buflen);
	    // remember how many chars were last recorded
	    last_recorded_len += state.buflen;
	}
	state.buflen = 0;
    }

    may_sync_undo();

#ifdef FEAT_EVAL
    // output "debug mode" message next time in debug mode
    debug_did_বার্তা = FALSE;
#endif

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}

/*
 * Record an <Ignore> key.
 */
    শূন্য
gotchars_ignore(শূন্য)
{
    char_u nop_buf[3] = { K_SPECIAL, KS_EXTRA, KE_IGNORE };
    gotchars(nop_buf, 3);
}

/*
 * Undo the last gotchars() for "len" bytes.  To be used when putting a typed
 * character back into the typeahead buffer, thus gotchars() will be called
 * again.
 * Only affects recorded characters.
 */
    শূন্য
ungetchars(পূর্ণসংখ্যা len)
{
    যদি (reg_recording == 0)
	ফেরত;

    delete_buff_লেজ(&recordbuff, len);
    last_recorded_len -= len;
}

/*
 * Sync undo.  Called when typed characters are obtained from the typeahead
 * buffer, or when a menu is used.
 * Do not sync:
 * - In Insert mode, unless cursor key has been used.
 * - While reading a script file.
 * - When no_u_sync is non-zero.
 */
    স্থির শূন্য
may_sync_undo(শূন্য)
{
    যদি ((!(State & (MODE_ঢোকাও | MODE_CMDLINE)) || arrow_used)
					       && scriptin[curscript] == ফাঁকা)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    স্থির পূর্ণসংখ্যা
alloc_typebuf(শূন্য)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    যদি (typebuf.tb_buf == ফাঁকা || typebuf.tb_noremap == ফাঁকা)
    {
	free_typebuf();
	ফেরত FAIL;
    }
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_off = MAXMAPLEN + 4;  // can insert without realloc
    typebuf.tb_len = 0;
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    typebuf.tb_no_abbr_cnt = 0;
    যদি (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
#যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    typebuf_was_filled = FALSE;
#endif
    ফেরত OK;
}

/*
 * Free the buffers of "typebuf".
 */
    স্থির শূন্য
free_typebuf(শূন্য)
{
    যদি (typebuf.tb_buf == typebuf_init)
	internal_error("Free typebuf 1");
    অন্যথা
	VIM_CLEAR(typebuf.tb_buf);
    যদি (typebuf.tb_noremap == noremapbuf_init)
	internal_error("Free typebuf 2");
    অন্যথা
	VIM_CLEAR(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
স্থির typebuf_T saved_typebuf[NSCRIPT];

    পূর্ণসংখ্যা
save_typebuf(শূন্য)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    যদি (alloc_typebuf() == FAIL)
    {
	closescript();
	ফেরত FAIL;
    }
    ফেরত OK;
}

স্থির পূর্ণসংখ্যা old_char = -1;	// character put back by vungetc()
স্থির পূর্ণসংখ্যা old_mod_mask;	// mod_mask for ungotten character
স্থির পূর্ণসংখ্যা old_mouse_row;	// mouse_row related to old_char
স্থির পূর্ণসংখ্যা old_mouse_col;	// mouse_col related to old_char
স্থির পূর্ণসংখ্যা old_চাবি_Stuffed;	// whether old_char was stuffed

স্থির পূর্ণসংখ্যা can_get_old_char(শূন্য)
{
    // If the old character was not stuffed and characters have been added to
    // the stuff buffer, need to first get the stuffed characters instead.
    ফেরত old_char != -1 && (old_চাবি_Stuffed || stuff_empty());
}

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    শূন্য
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    যদি (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_mask = old_mod_mask;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_first.b_পরবর্তী = ফাঁকা;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_first.b_পরবর্তী = ফাঁকা;
#ifdef USE_প্রবেশ_BUF
    tp->save_inputbuf = get_প্রবেশ_buf();
#endif
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    শূন্য
restore_typeahead(tasave_T *tp, পূর্ণসংখ্যা overwrite UNUSED)
{
    যদি (tp->typebuf_valid)
    {
	free_typebuf();
	typebuf = tp->save_typebuf;
    }

    old_char = tp->old_char;
    old_mod_mask = tp->old_mod_mask;

    free_buff(&readbuf1);
    readbuf1 = tp->save_readbuf1;
    free_buff(&readbuf2);
    readbuf2 = tp->save_readbuf2;
#ifdef USE_প্রবেশ_BUF
    set_প্রবেশ_buf(tp->save_inputbuf, overwrite);
#endif
}

/*
 * Open a new script file for the ":source!" command.
 */
    শূন্য
openscript(
    char_u	*name,
    পূর্ণসংখ্যা		directly)	// when TRUE execute directly
{
    যদি (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	ফেরত;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    যদি (check_secure())
	ফেরত;

#ifdef FEAT_EVAL
    যদি (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	ফেরত;
#endif

    যদি (scriptin[curscript] != ফাঁকা)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    যদি ((scriptin[curscript] = mch_fopen((অক্ষর *)NameBuff, READBIN)) == ফাঁকা)
    {
	semsg(_(e_cant_open_file_str), name);
	যদি (curscript)
	    --curscript;
	ফেরত;
    }
    যদি (save_typebuf() == FAIL)
	ফেরত;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    যদি (directly)
    {
	oparg_T	oa;
	পূর্ণসংখ্যা	oldcurscript;
	পূর্ণসংখ্যা	save_State = State;
	পূর্ণসংখ্যা	save_restart_edit = restart_edit;
	পূর্ণসংখ্যা	save_insertmode = p_im;
	পূর্ণসংখ্যা	save_finish_op = finish_op;
	পূর্ণসংখ্যা	save_বার্তা_scroll = বার্তা_scroll;

	State = MODE_NORMAL;
	বার্তা_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	finish_op = FALSE;

	oldcurscript = curscript;
	করো
	{
	    update_topline_cursor();	// update cursor position and topline
	    normal_cmd(&oa, FALSE);	// execute one command
	    (শূন্য)vpeekc();		// check for end of file
	}
	যখন (scriptin[oldcurscript] != ফাঁকা);

	State = save_State;
	বার্তা_scroll = save_বার্তা_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	finish_op = save_finish_op;
    }
}

/*
 * Close the currently active input script.
 */
    স্থির শূন্য
closescript(শূন্য)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = ফাঁকা;
    যদি (curscript > 0)
	--curscript;
}

#যদি defined(EXITFREE)
    শূন্য
close_all_scripts(শূন্য)
{
    যখন (scriptin[0] != ফাঁকা)
	closescript();
}
#endif

/*
 * Return TRUE when reading keys from a script file.
 */
    পূর্ণসংখ্যা
using_script(শূন্য)
{
    ফেরত scriptin[curscript] != ফাঁকা;
}

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    শূন্য
before_blocking(শূন্য)
{
    // Tell terminal to flush screen contents before blocking
    term_set_sync_output(TERM_SYNC_OUTPUT_FLUSH);

    updatescript(0);
#ifdef FEAT_EVAL
    যদি (may_garbage_collect)
	garbage_collect(FALSE);
#endif
}

/*
 * updatescript() is called when a character can be written into the script
 * file or when we have waited some time for a character (c == 0)
 *
 * All the changed memfiles are synced if c == 0 or when the number of typed
 * characters reaches 'updatecount' and 'updatecount' is non-zero.
 */
    স্থির শূন্য
updatescript(পূর্ণসংখ্যা c)
{
    স্থির পূর্ণসংখ্যা	    গুনতি = 0;

    যদি (c && scriptout)
	putc(c, scriptout);
    যদি (c == 0 || (p_uc > 0 && ++গুনতি >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	গুনতি = 0;
    }
#ifdef FEAT_EVAL
    যদি (typedchars_pos < MAXMAPLEN)
    {
	typedchars[typedchars_pos] = c;
	typedchars_pos++;
    }
#endif
}

/*
 * Convert "c_arg" plus "modifiers" to merge the effect of modifyOtherKeys into
 * the character.  Also for when the Kitty key protocol is used.
 */
    পূর্ণসংখ্যা
merge_modify_Other_চাবিগুলি(পূর্ণসংখ্যা c_arg, পূর্ণসংখ্যা *modifiers)
{
    পূর্ণসংখ্যা c = c_arg;

    // CTRL only uses the lower 5 bits of the character.
    যদি (*modifiers & MOD_MASK_CTRL)
    {
	যদি ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	{
	    c &= 0x1f;
	    যদি (c == NUL)
		c = K_ZERO;
	}
	অন্যথা যদি (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#ifdef FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	অন্যথা যদি (c == '2')
	    c = NUL;
	অন্যথা যদি (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	অন্যথা যদি (c == '8')
	    c = BS;
	অন্যথা যদি (c == '?')
	    c = DEL;
#endif
	যদি (c != c_arg)
	    *modifiers &= ~MOD_MASK_CTRL;
    }

    // Alt/Meta sets the 8th bit of the character.
    যদি ((*modifiers & (MOD_MASK_META | MOD_MASK_ALT))
	    && c >= 0 && c <= 127)
    {
	// Some terminals (esp. Kitty) do not include Shift in the character.
	// Apply it here to get consistency across terminals.  Only do ASCII
	// letters, for other characters it depends on the keyboard layout.
	যদি ((*modifiers & MOD_MASK_SHIFT) && c >= 'a' && c <= 'z')
	{
	    c += 'a' - 'A';
	    *modifiers &= ~MOD_MASK_SHIFT;
	}
	c += 0x80;
	*modifiers &= ~(MOD_MASK_META | MOD_MASK_ALT);
    }

    ফেরত c;
}

/*
 * Add a single byte to 'showcmd' for a partially matched mapping.
 * Call add_to_showcmd() if a full key has been received.
 */
    স্থির শূন্য
add_byte_to_showcmd(char_u byte)
{
    স্থির gotchars_state_T state;
    char_u	*ptr;
    পূর্ণসংখ্যা		modifiers = 0;
    পূর্ণসংখ্যা		c = NUL;

    যদি (!p_sc || বার্তা_silent != 0)
	ফেরত;

    যদি (!gotchars_add_byte(&state, byte))
	ফেরত;

    state.buf[state.buflen] = NUL;
    state.buflen = 0;

    ptr = state.buf;
    যদি (ptr[0] == K_SPECIAL && ptr[1] == KS_MODIFIER && ptr[2] != NUL)
    {
	modifiers = ptr[2];
	ptr += 3;
    }

    যদি (*ptr != NUL)
    {
	char_u		*mb_ptr = mb_unescape(&ptr);

	c = mb_ptr != ফাঁকা ? (*mb_ptr2char)(mb_ptr) : *ptr++;
	যদি (c <= 0x7f)
	{
	    // Merge modifiers into the key to make the result more readable.
	    পূর্ণসংখ্যা		modifiers_after = modifiers;
	    পূর্ণসংখ্যা		mod_c = merge_modify_Other_চাবিগুলি(c, &modifiers_after);

	    যদি (modifiers_after == 0)
	    {
		modifiers = 0;
		c = mod_c;
	    }
	}
    }

    // TODO: is there a more readable and yet compact representation of
    // modifiers and special keys?
    যদি (modifiers != 0)
    {
	add_to_showcmd(K_SPECIAL);
	add_to_showcmd(KS_MODIFIER);
	add_to_showcmd(modifiers);
    }
    যদি (c != NUL)
	add_to_showcmd(c);
    যখন (*ptr != NUL)
	add_to_showcmd(*ptr++);
}

/*
 * Get the next input character.
 * Can return a special key or a multi-byte character.
 * Can return NUL when called recursively, use safe_vgetc() if that's not
 * wanted.
 * This translates escaped K_SPECIAL and CSI bytes to a K_SPECIAL or CSI byte.
 * Collects the bytes of a multibyte character into the whole character.
 * Returns the modifiers in the global "mod_mask".
 */
    পূর্ণসংখ্যা
vgetc(শূন্য)
{
    পূর্ণসংখ্যা		c, c2;
    পূর্ণসংখ্যা		n;
    char_u	buf[MB_MAXBYTES + 1];
    পূর্ণসংখ্যা		i;

#ifdef FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    যদি (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#endif

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    যদি (can_get_old_char())
    {
	c = old_char;
	old_char = -1;
	mod_mask = old_mod_mask;
	mouse_row = old_mouse_row;
	mouse_col = old_mouse_col;
    }
    অন্যথা
    {
	// number of characters recorded from the last vgetc() call
	স্থির size_t	last_vgetc_recorded_len = 0;

	mod_mask = 0;
	vgetc_mod_mask = 0;
	vgetc_char = 0;

	// last_recorded_len can be larger than last_vgetc_recorded_len
	// if peeking records more
	last_recorded_len -= last_vgetc_recorded_len;

	জন্য (;;)		// this is done twice if there are modifiers
	{
	    পূর্ণসংখ্যা did_inc = FALSE;

	    // No mapping after modifier has been read, using an input method
	    // and when a popup window has disabled mapping.
	    যদি (mod_mask
#যদি defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#endif
#যদি defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#endif
		    )
	    {
		++no_mapping;
		++allow_চাবিগুলি;
		// mod_mask value may change, remember we did the increment
		did_inc = TRUE;
	    }
	    c = vgetorpeek(TRUE);
	    যদি (did_inc)
	    {
		--no_mapping;
		--allow_চাবিগুলি;
	    }

	    // Get two extra bytes for special keys, handle modifiers.
	    যদি (c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	       )
	    {
		পূর্ণসংখ্যা	    save_allow_চাবিগুলি = allow_চাবিগুলি;

		++no_mapping;
		allow_চাবিগুলি = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_চাবিগুলি = save_allow_চাবিগুলি;
		যদি (c2 == KS_MODIFIER)
		{
		    mod_mask = c;
		    চালু;
		}
		c = TO_SPECIAL(c2, c);

		যদি (allow_osc_চাবি == 0 && c == K_OSC)
		    চালু;

		// K_ESC is used to avoid ambiguity with the single Esc
		// character that might be the start of an escape sequence.
		// Convert it back to a single Esc here.
		যদি (c == K_ESC)
		    c = ESC;

#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		// Handle K_TEAROFF here, the caller of vgetc() doesn't need to
		// know that a menu was torn off
		যদি (
# ifdef VIMDLL
		    gui.in_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    পূর্ণসংখ্যা		j;

		    // get menu path, it ends with a <CR>
		    জন্য (j = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[j] = c;
			যদি (j < 199)
			    ++j;
		    }
		    name[j] = NUL;
		    gui_make_tearoff(name);
		    চালু;
		}
#endif
#যদি defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && !defined(USE_GTK4) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		যদি (c == K_F10 && gui.menubar != ফাঁকা)
		{
		    gtk_menu_shell_select_first(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    চালু;
		}
#endif
#ifdef FEAT_GUI
		// Handle focus event here, so that the caller doesn't need to
		// know about it.  Return K_IGNORE so that we loop once (needed
		// if 'lazyredraw' is set).
		যদি (c == K_FOCUSGAINED || c == K_FOCUSLOST)
		{
		    ui_focus_change(c == K_FOCUSGAINED);
		    c = K_IGNORE;
		}

		// Translate K_CSI to CSI.  The special key is only used to
		// avoid it being recognized as the start of a special key.
		যদি (c == K_CSI)
		    c = CSI;
#endif
#ifdef FEAT_EVAL
		যদি (c == K_SID)
		{
		    পূর্ণসংখ্যা	    j;

		    // Handle <SID>{sid};  Do up to 20 digits for safety.
		    last_used_sid = 0;
		    জন্য (j = 0; j < 20 && SAFE_isdigit(c = vgetorpeek(TRUE)); ++j)
			last_used_sid = last_used_sid * 10 + (c - '0');
		    last_used_map = ফাঁকা;
		    চালু;
		}
#endif
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    যদি (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		জন্য (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    যদি (buf[i] == K_SPECIAL
#ifdef FEAT_GUI
			    || (buf[i] == CSI)
#endif
			    )
		    {
			// Must be a K_SPECIAL - KS_SPECIAL - KE_FILLER
			// sequence, which represents a K_SPECIAL (0x80),
			// or a CSI - KS_EXTRA - KE_CSI sequence, which
			// represents a CSI (0x9B),
			// or a K_SPECIAL - KS_EXTRA - KE_CSI, which is CSI
			// too.
			c = vgetorpeek(TRUE);
			যদি (vgetorpeek(TRUE) == KE_CSI && c == KS_EXTRA)
			    buf[i] = CSI;
		    }
		}
		--no_mapping;
		c = (*mb_ptr2char)(buf);
	    }

	    যদি (vgetc_char == 0)
	    {
		vgetc_mod_mask = mod_mask;
		vgetc_char = c;
	    }

	    // A keypad or special function key was not mapped, use it like
	    // its ASCII equivalent.
	    নির্বাচন (c)
	    {
		ক্ষেত্র K_KPLUS:	c = '+'; বিরতি;
		ক্ষেত্র K_KMINUS:	c = '-'; বিরতি;
		ক্ষেত্র K_KDIVIDE:	c = '/'; বিরতি;
		ক্ষেত্র K_KMULTIPLY: c = '*'; বিরতি;
		ক্ষেত্র K_KENTER:	c = CAR; বিরতি;
		ক্ষেত্র K_KPOINT:
#ifdef MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = Map_Virtual_চাবি(VK_DECIMAL, 2); বিরতি;
#অন্যথা
				c = '.'; বিরতি;
#endif
		ক্ষেত্র K_K0:	c = '0'; বিরতি;
		ক্ষেত্র K_K1:	c = '1'; বিরতি;
		ক্ষেত্র K_K2:	c = '2'; বিরতি;
		ক্ষেত্র K_K3:	c = '3'; বিরতি;
		ক্ষেত্র K_K4:	c = '4'; বিরতি;
		ক্ষেত্র K_K5:	c = '5'; বিরতি;
		ক্ষেত্র K_K6:	c = '6'; বিরতি;
		ক্ষেত্র K_K7:	c = '7'; বিরতি;
		ক্ষেত্র K_K8:	c = '8'; বিরতি;
		ক্ষেত্র K_K9:	c = '9'; বিরতি;

		ক্ষেত্র K_XHOME:
		ক্ষেত্র K_ZHOME:	যদি (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_HOME;
				    mod_mask = 0;
				}
				অন্যথা যদি (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_HOME;
				    mod_mask = 0;
				}
				অন্যথা
				    c = K_HOME;
				বিরতি;
		ক্ষেত্র K_XEND:
		ক্ষেত্র K_ZEND:	যদি (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_END;
				    mod_mask = 0;
				}
				অন্যথা যদি (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_END;
				    mod_mask = 0;
				}
				অন্যথা
				    c = K_END;
				বিরতি;

		ক্ষেত্র K_XUP:	c = K_UP; বিরতি;
		ক্ষেত্র K_XDOWN:	c = K_DOWN; বিরতি;
		ক্ষেত্র K_XLEFT:	c = K_বাম; বিরতি;
		ক্ষেত্র K_XRIGHT:	c = K_ডান; বিরতি;
	    }

	    বিরতি;
	}

	last_vgetc_recorded_len = last_recorded_len;
    }

#ifdef FEAT_EVAL
    /*
     * In the main loop "may_garbage_collect" can be set to do garbage
     * collection in the first next vgetc().  It's disabled after that to
     * avoid internally used Lists and Dicts to be freed.
     */
    may_garbage_collect = FALSE;
#endif

#ifdef FEAT_BEVAL_TERM
    যদি (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD
	    && c != K_COMPLETE_DELAY)
    {
	// Don't trigger 'balloonexpr' unless only the mouse was moved.
	bevalexpr_due_set = FALSE;
	ui_remove_balloon();
    }
#endif
#ifdef FEAT_PROP_POPUP
    // Only filter keys that do not come from ":normal".  Keys from feedkeys()
    // are filtered.
    যদি ((!ex_normal_busy || in_feedkeys) && popup_do_filter(c))
    {
	যদি (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#endif

#ifdef FEAT_EVAL
    c = do_চাবি_প্রবেশ_pre(c);

    // Clear the next typedchars_pos
    typedchars_pos = 0;
#endif

    // Need to process the character before we know it's safe to do something
    // else.
    যদি (c != K_IGNORE)
	state_no_longer_safe("key typed");

    ফেরত c;
}

#ifdef FEAT_EVAL
/*
 * Handle the KeyInputPre autocommand.
 * "c" is the character that was typed.
 * Return new input character.
 */
    স্থির পূর্ণসংখ্যা
do_চাবি_প্রবেশ_pre(পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা		res = c;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    char_u	curr_mode[MODE_MAX_দৈর্ঘ্য];
    পূর্ণসংখ্যা		save_State = State;
    save_v_event_T save_v_event;
    dict_T	*v_event;

    // Return quickly when there is nothing to do.
    যদি (!has_keyinputpre())
	ফেরত res;

    যদি (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = KEY2TERMCAP0(c);
	buf[2] = KEY2TERMCAP1(c);
	buflen = 3;
    }
    অন্যথা
	buflen = (*mb_char2bytes)(c, buf);
    buf[buflen] = NUL;

    typedchars[typedchars_pos] = NUL;
    vim_unescape_csi(typedchars);

    get_mode(curr_mode);

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (পূর্ণসংখ্যা)buflen);  // set v:char

    v_event = get_v_event(&save_v_event);
    (শূন্য)dict_add_bool(v_event, "typed", চাবি_Typed);
    (শূন্য)dict_add_string_len(v_event, "typedchar", typedchars, typedchars_pos);

    যদি (apply_autocmds(EVENT_KEYINPUTPRE, curr_mode, curr_mode, FALSE, curbuf))
    {
	string_T    v_char;

	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character.
	v_char.string = get_vim_var_str(VV_CHAR);
	যদি (STRCMP(buf, v_char.string) != 0)
	{
	    v_char.দৈর্ঘ্য = STRLEN(v_char.string);
	    // Convert special bytes when it is special string.
	    যদি (v_char.দৈর্ঘ্য >= 3 && v_char.string[0] == K_SPECIAL)
		res = TERMCAP2_চাবি(v_char.string[1], v_char.string[2]);
	    অন্যথা যদি (v_char.দৈর্ঘ্য > 0)
		res = PTR2CHAR(v_char.string);
	}
    }

    restore_v_event(v_event, &save_v_event);

    set_vim_var_string(VV_CHAR, ফাঁকা, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    ফেরত res;
}
#endif

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    পূর্ণসংখ্যা
safe_vgetc(শূন্য)
{
    পূর্ণসংখ্যা	c;

    c = vgetc();
    যদি (c == NUL)
	c = get_keystroke();
    ফেরত c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 * Does not handle bracketed paste - do not use the result for commands.
 */
    স্থির পূর্ণসংখ্যা
plain_vgetc_nopaste(শূন্য)
{
    পূর্ণসংখ্যা c;

    করো
	c = safe_vgetc();
    যখন (c == K_IGNORE
	    || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
	    || c == K_MOUSEMOVE);
    ফেরত c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    পূর্ণসংখ্যা
plain_vgetc(শূন্য)
{
    পূর্ণসংখ্যা c = plain_vgetc_nopaste();

    যদি (c == K_PS)
	// Only handle the first pasted character.  Drop the rest, since we
	// don't know what to do with it.
	c = bracketed_paste(PASTE_ONE_CHAR, FALSE, ফাঁকা);

    ফেরত c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 * Returns NUL if no character is available.
 */
    পূর্ণসংখ্যা
vpeekc(শূন্য)
{
    যদি (can_get_old_char())
	ফেরত old_char;
    ফেরত vgetorpeek(FALSE);
}

#যদি defined(FEAT_TERMRESPONSE) || defined(FEAT_TERMINAL)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    পূর্ণসংখ্যা
vpeekc_nomap(শূন্য)
{
    পূর্ণসংখ্যা		c;

    ++no_mapping;
    ++allow_চাবিগুলি;
    c = vpeekc();
    --no_mapping;
    --allow_চাবিগুলি;
    ফেরত c;
}
#endif

/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    পূর্ণসংখ্যা
vpeekc_any(শূন্য)
{
    পূর্ণসংখ্যা		c;

    c = vpeekc();
    যদি (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    ফেরত c;
}

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    পূর্ণসংখ্যা
char_avail(শূন্য)
{
    পূর্ণসংখ্যা	    retval;

#ifdef FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    যদি (disable_char_avail_for_testing)
	ফেরত FALSE;
#endif
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    ফেরত (retval != NUL);
}

#যদি defined(FEAT_EVAL)
/*
 * "getchar()" and "getcharstr()" functions
 */
    স্থির শূন্য
getchar_common(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা allow_সংখ্যা)
{
    varnumber_T		n = 0;
    পূর্ণসংখ্যা			called_emsg_start = called_emsg;
    পূর্ণসংখ্যা			error = FALSE;
    পূর্ণসংখ্যা			simplify = TRUE;
    char_u		cursor_flag = 'm';

    যদি ((in_vim9script()
		&& check_for_opt_bool_or_সংখ্যা_arg(argvars, 0) == FAIL)
	    || (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type == VAR_DICT)
    {
	dict_T		*d = argvars[1].vval.v_dict;
	char_u		*cursor_str;

	যদি (allow_সংখ্যা)
	    allow_সংখ্যা = dict_get_bool(d, "number", TRUE);
	অন্যথা যদি (dict_has_চাবি(d, "number"))
	    semsg(_(e_invalid_argument_str), "number");

	simplify = dict_get_bool(d, "simplify", TRUE);

	cursor_str = dict_get_string(d, "cursor", FALSE);
	যদি (cursor_str != ফাঁকা)
	{
	    যদি (STRCMP(cursor_str, "hide") != 0
		    && STRCMP(cursor_str, "keep") != 0
		    && STRCMP(cursor_str, "msg") != 0)
		semsg(_(e_invalid_মান_for_argument_str_str), "cursor",
								   cursor_str);
	    অন্যথা
		cursor_flag = cursor_str[0];
	}
    }

    যদি (called_emsg != called_emsg_start)
	ফেরত;

# ifdef MESSAGE_QUEUE
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
# endif

    যদি (cursor_flag == 'h')
	cursor_sleep();
    অন্যথা যদি (cursor_flag == 'm')
	windgoto(বার্তা_row, cmdline_col_off + বার্তা_col);

    ++no_mapping;
    ++allow_চাবিগুলি;
    ++allow_osc_চাবি;
    যদি (!simplify)
	++no_reduce_চাবিগুলি;
    জন্য (;;)
    {
	যদি (argvars[0].v_type == VAR_UNKNOWN
		|| (argvars[0].v_type == VAR_সংখ্যা
			&& argvars[0].vval.v_সংখ্যা == -1))
	    // getchar(): blocking wait.
	    n = plain_vgetc_nopaste();
	অন্যথা যদি (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	অন্যথা যদি (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	অন্যথা
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = safe_vgetc();

	যদি (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    চালু;
	বিরতি;
    }
    --no_mapping;
    --allow_চাবিগুলি;
    --allow_osc_চাবি;
    যদি (!simplify)
	--no_reduce_চাবিগুলি;

    যদি (cursor_flag == 'h')
	cursor_unsleep();

    set_vim_var_nr(VV_MOUSE_WIN, 0);
    set_vim_var_nr(VV_MOUSE_WINID, 0);
    set_vim_var_nr(VV_MOUSE_LNUM, 0);
    set_vim_var_nr(VV_MOUSE_COL, 0);

    যদি (n != 0 && (!allow_সংখ্যা || IS_SPECIAL(n) || mod_mask != 0))
    {
	char_u		অস্থায়ী[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	পূর্ণসংখ্যা		i = 0;

	// Turn a special key into three bytes, plus modifier.
	যদি (mod_mask != 0)
	{
	    অস্থায়ী[i++] = K_SPECIAL;
	    অস্থায়ী[i++] = KS_MODIFIER;
	    অস্থায়ী[i++] = mod_mask;
	}
	যদি (IS_SPECIAL(n))
	{
	    অস্থায়ী[i++] = K_SPECIAL;
	    অস্থায়ী[i++] = K_SECOND(n);
	    অস্থায়ী[i++] = K_THIRD(n);
	}
	অন্যথা যদি (has_mbyte)
	    i += (*mb_char2bytes)(n, অস্থায়ী + i);
	অন্যথা
	    অস্থায়ী[i++] = n;
	অস্থায়ী[i] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strnsave(অস্থায়ী, i);

	যদি (is_mouse_চাবি(n))
	{
	    পূর্ণসংখ্যা		row = mouse_row;
	    পূর্ণসংখ্যা		col = mouse_col;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    পূর্ণসংখ্যা		winnr = 1;

	    যদি (row >= 0 && col >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_find_win(&row, &col, FIND_POPUP);
		যদি (win == ফাঁকা)
		    ফেরত;
		(শূন্য)mouse_comp_pos(win, &row, &col, &lnum, ফাঁকা);
# ifdef FEAT_PROP_POPUP
		যদি (WIN_IS_POPUP(win))
		    winnr = 0;
		অন্যথা
# endif
		    জন্য (wp = firstwin; wp != win && wp != ফাঁকা;
							       wp = wp->w_পরবর্তী)
			++winnr;
		set_vim_var_nr(VV_MOUSE_WIN, winnr);
		set_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		set_vim_var_nr(VV_MOUSE_LNUM, lnum);
		set_vim_var_nr(VV_MOUSE_COL, col + 1);
	    }
	}
    }
    অন্যথা যদি (!allow_সংখ্যা)
	rettv->v_type = VAR_STRING;
    অন্যথা
	rettv->vval.v_সংখ্যা = n;
}

/*
 * "getchar()" function
 */
    শূন্য
f_getchar(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, TRUE);
}

/*
 * "getcharstr()" function
 */
    শূন্য
f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, FALSE);
}

/*
 * "getcharmod()" function
 */
    শূন্য
f_getcharmod(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = mod_mask;
}
#endif // FEAT_EVAL

#যদি defined(MESSAGE_QUEUE)
# define MAX_REPEAT_PARSE 8

/*
 * Process messages that have been queued for netbeans or clientserver.
 * Also check if any jobs have ended.
 * These functions can call arbitrary Vim script and should only be called when
 * it is safe to do so.
 */
    শূন্য
parse_queued_messages(শূন্য)
{
    পূর্ণসংখ্যা	    old_curwin_id;
    পূর্ণসংখ্যা	    old_curbuf_fnum;
    পূর্ণসংখ্যা	    i;
    পূর্ণসংখ্যা	    save_may_garbage_collect = may_garbage_collect;
    স্থির পূর্ণসংখ্যা entered = 0;
    পূর্ণসংখ্যা	    was_safe = get_was_safe_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    যদি (updating_screen || dont_parse_messages)
	ফেরত;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    যদি (curbuf == ফাঁকা || curwin == ফাঁকা)
	ফেরত;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    জন্য (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# যদি (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_JOB_CHANNEL)
	channel_handle_events(FALSE);
# endif

# ifdef FEAT_NETBEANS_INTG
	// Process the queued netbeans messages.
	netbeans_parse_messages();
# endif
# ifdef FEAT_SOCKETSERVER
	socketserver_parse_messages();
# endif
# ifdef FEAT_JOB_CHANNEL
	// Write any buffer lines still to be written.
	channel_write_any_সারিগুলি();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# যদি defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_JOB_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	যদি (job_check_ended())
	    চালু;
# endif
# ifdef FEAT_TERMINAL
	free_unused_terminals();
# endif

# ifdef FEAT_SOUND_MACOSX
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	যদি (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef SIGUSR1
	যদি (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, ফাঁকা, ফাঁকা, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
# endif
	বিরতি;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    যদি (entered == 1 && was_safe)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    যদি (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}
#endif


প্রকারলেখ গণনা {
    map_ফলাফল_fail,    // failed, break loop
    map_ফলাফল_get,     // get a character from typeahead
    map_ফলাফল_retry,   // try to map again
    map_ফলাফল_nomatch  // no matching mapping, get char
} map_ফলাফল_T;

/*
 * Check if the bytes at the start of the typeahead buffer are a character used
 * in Insert mode completion.  This includes the form with a CTRL modifier.
 * During completion started by complete() keys are mapped as usual.
 */
    স্থির পূর্ণসংখ্যা
at_ins_compl_চাবি(শূন্য)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    পূর্ণসংখ্যা	    c = *p;

    যদি (typebuf.tb_len > 3
	    && (c == K_SPECIAL || c == CSI)  // CSI is used by the GUI
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_MASK_CTRL)
	    // CTRL-SHIFT-N/P scroll the info popup, so they must not be folded
	    // to the CTRL-N/CTRL-P completion keys here.
	    && !(p[2] & MOD_MASK_SHIFT))
	c = p[3] & 0x1f;
    ফেরত !ctrl_x_mode_eval()
	    && ((ctrl_x_mode_not_default() && vim_is_ctrl_x_চাবি(c))
		|| (compl_status_local() && (c == Ctrl_N || c == Ctrl_P)));
}

/*
 * Check if typebuf.tb_buf[] contains a modifier plus key that can be changed
 * into just a key, apply that.
 * Check from typebuf.tb_buf[typebuf.tb_off] to typebuf.tb_buf[typebuf.tb_off
 * + "max_offset"].
 * Return the length of the replaced bytes, 0 if nothing changed, -1 for error.
 */
    স্থির পূর্ণসংখ্যা
check_simplify_modifier(পূর্ণসংখ্যা max_offset)
{
    পূর্ণসংখ্যা		offset;
    char_u	*tp;

    জন্য (offset = 0; offset < max_offset; ++offset)
    {
	যদি (offset + 3 >= typebuf.tb_len)
	    বিরতি;
	tp = typebuf.tb_buf + typebuf.tb_off + offset;
	যদি ((tp[0] == K_SPECIAL || tp[0] == CSI) && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    পূর্ণসংখ্যা modifier = tp[2];
	    পূর্ণসংখ্যা	c = tp[3];
	    পূর্ণসংখ্যা new_c = merge_modify_Other_চাবিগুলি(c, &modifier);

	    যদি (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		পূর্ণসংখ্যা	len;
		পূর্ণসংখ্যা	চাবি_offset = offset;

		যদি (offset == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_mask = tp[2];
		}
		যদি (IS_SPECIAL(new_c))
		{
		    new_string[0] = K_SPECIAL;
		    new_string[1] = K_SECOND(new_c);
		    new_string[2] = K_THIRD(new_c);
		    len = 3;
		}
		অন্যথা
		    len = mb_char2bytes(new_c, new_string);
		যদি (modifier == 0)
		{
		    যদি (put_string_in_typebuf(offset, 4, new_string, len,
							ফাঁকা, 0, ফাঁকা) == FAIL)
			ফেরত -1;
		}
		অন্যথা
		{
		    tp[2] = modifier;
		    চাবি_offset = offset + 3;
		    যদি (put_string_in_typebuf(চাবি_offset, 1, new_string, len,
							ফাঁকা, 0, ফাঁকা) == FAIL)
			ফেরত -1;
		}

		// A mapping for the simplified key can be used for it even
		// when the key protocol is enabled.
		জন্য (পূর্ণসংখ্যা i = 0; i < len; ++i)
		    typebuf.tb_noremap[typebuf.tb_off + চাবি_offset + i]
							    |= RM_SIMPLIFIED;
		ফেরত len;
	    }
	}
    }
    ফেরত 0;
}

/*
 * Return TRUE if the terminal sends modifiers with various keys.  This is when
 * modifyOtherKeys level 2 is enabled or the kitty keyboard protocol is
 * enabled.
 */
    পূর্ণসংখ্যা
চাবি_protocol_enabled(শূন্য)
{
    // If xterm has responded to XTQMODKEYS it overrules seenModifyOtherKeys.
    পূর্ণসংখ্যা using_mok = modify_otherkeys_state != MOKS_INITIAL
			? modify_otherkeys_state == MOKS_ENABLED
			: seen_Modify_Other_চাবিগুলি;
    ফেরত using_mok || kitty_protocol_state == KKPS_ENABLED;
}

/*
 * Handle mappings in the typeahead buffer.
 * - When something was mapped, return map_result_retry for recursive mappings.
 * - When nothing mapped and typeahead has a character: return map_result_get.
 * - When there is no match yet, return map_result_nomatch, need to get more
 *   typeahead.
 * - On failure (out of memory) return map_result_fail.
 */
    স্থির পূর্ণসংখ্যা
handle_mapping(
	    পূর্ণসংখ্যা *keylenp,
	    পূর্ণসংখ্যা *timedout,
	    পূর্ণসংখ্যা *mapdepth)
{
    mapblock_T	*mp = ফাঁকা;
    mapblock_T	*mp2;
    mapblock_T	*mp_match;
    পূর্ণসংখ্যা		mp_match_len = 0;
    পূর্ণসংখ্যা		max_mlen = 0;
    পূর্ণসংখ্যা		want_termcode = 0;  // 1 if termcode expected after max_mlen
    পূর্ণসংখ্যা		tb_c1;
    পূর্ণসংখ্যা		mlen;
#ifdef FEAT_LANGMAP
    পূর্ণসংখ্যা		nolmaplen;
#endif
    পূর্ণসংখ্যা		keylen = *keylenp;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		local_State = get_real_state();
    পূর্ণসংখ্যা		is_plug_map = FALSE;
    bool	in_osc = in_osc_sequence(); // If we are in an OSC sequence,
					    // then go straight to
					    // check_termcode() in order to
					    // consume chars.

    // If typeahead starts with <Plug> then remap, even for a "noremap" mapping.
    যদি (!in_osc && typebuf.tb_len >= 3
	    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
	    && typebuf.tb_buf[typebuf.tb_off + 1] == KS_EXTRA
	    && typebuf.tb_buf[typebuf.tb_off + 2] == KE_PLUG)
	is_plug_map = TRUE;

    /*
     * Check for a mappable key sequence.
     * Walk through one maphash[] list until we find an entry that matches.
     *
     * Don't look for mappings if:
     * - no_mapping set: mapping disabled (e.g. for CTRL-V)
     * - maphash_valid not set: no mappings present.
     * - typebuf.tb_buf[typebuf.tb_off] should not be remapped
     * - in insert or cmdline mode and 'paste' option set
     * - waiting for "hit return to continue" and CR or SPACE typed
     * - waiting for a char with --more--
     * - in Ctrl-X mode (not started by complete()), and we get a valid char
     *   for that mode
     * - currently receiving OSC sequence
     */
    tb_c1 = typebuf.tb_buf[typebuf.tb_off];
    যদি (!in_osc && no_mapping == 0 && is_maphash_valid()
	    && (no_zero_mapping == 0 || tb_c1 != '0')
	    && (typebuf.tb_maplen == 0 || is_plug_map
		|| (p_remap
		    && (typebuf.tb_noremap[typebuf.tb_off]
				    & (RM_NONE|RM_ABBR)) == 0))
	    && !(p_paste && (State & (MODE_ঢোকাও | MODE_CMDLINE)))
	    && !(State == MODE_HITRETURN && (tb_c1 == CAR || tb_c1 == ' '))
	    && State != MODE_ASKMORE
	    && State != MODE_CONFIRM
	    && !at_ins_compl_চাবি())
    {
#ifdef FEAT_GUI
	যদি (gui.in_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#endif
#ifdef FEAT_LANGMAP
	যদি (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	অন্যথা
	{
	    LANGMAP_ADJUST(tb_c1, (State & (MODE_CMDLINE | MODE_ঢোকাও)) == 0
					   && get_real_state() != MODE_SELECT);
	    nolmaplen = 0;
	}
#endif
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	যদি (mp == ফাঁকা)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = ফাঁকা;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_match = ফাঁকা;
	mp_match_len = 0;
	জন্য ( ; mp != ফাঁকা;
	       mp->m_পরবর্তী == ফাঁকা ? (mp = mp2, mp2 = ফাঁকা) : (mp = mp->m_পরবর্তী))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    যদি (mp->m_চাবিগুলি[0] == tb_c1
		    && (mp->m_mode & local_State)
		    && !(mp->m_simplified && চাবি_protocol_enabled()
			    && typebuf.tb_maplen == 0
			    && (typebuf.tb_noremap[typebuf.tb_off]
							& RM_SIMPLIFIED) == 0)
		    && ((mp->m_mode & MODE_LANGMAP) == 0
						    || typebuf.tb_maplen == 0))
	    {
#ifdef FEAT_LANGMAP
		পূর্ণসংখ্যা	nomap = nolmaplen;
		পূর্ণসংখ্যা	modifiers = 0;
#endif
		// find the match length of this mapping
		জন্য (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
		    পূর্ণসংখ্যা	c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
#ifdef FEAT_LANGMAP
		    যদি (nomap > 0)
		    {
			যদি (nomap == 2 && c2 == KS_MODIFIER)
			    modifiers = 1;
			অন্যথা যদি (nomap == 1 && modifiers == 1)
			    modifiers = c2;
			--nomap;
		    }
		    অন্যথা
		    {
			যদি (c2 == K_SPECIAL)
			    nomap = 2;
			অন্যথা যদি (merge_modify_Other_চাবিগুলি(c2, &modifiers) == c2)
			    // Only apply 'langmap' if merging modifiers into
			    // the key will not result in another character,
			    // so that 'langmap' behaves consistently in
			    // different terminals and GUIs.
			    LANGMAP_ADJUST(c2, TRUE);
			modifiers = 0;
		    }
#endif
		    যদি (mp->m_চাবিগুলি[mlen] != c2)
			বিরতি;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_চাবিগুলি;
		    char_u *p2 = mb_unescape(&p1);

		    যদি (has_mbyte && p2 != ফাঁকা
					&& MB_BYTE2LEN(tb_c1) > mb_ptr2len(p2))
			mlen = 0;
		}

		// Check an entry whether it matches.
		// - Full match: mlen == keylen
		// - Partly match: mlen == typebuf.tb_len
		keylen = mp->m_keylen;
		যদি (mlen == keylen || (mlen == typebuf.tb_len
						   && typebuf.tb_len < keylen))
		{
		    char_u  *s;
		    পূর্ণসংখ্যা	    n;

		    // If only script-local mappings are allowed, check if the
		    // mapping starts with K_SNR.
		    s = typebuf.tb_noremap + typebuf.tb_off;
		    যদি ((*s & ~RM_SIMPLIFIED) == RM_SCRIPT
			    && (mp->m_চাবিগুলি[0] != K_SPECIAL
				|| mp->m_চাবিগুলি[1] != KS_EXTRA
				|| mp->m_চাবিগুলি[2] != KE_SNR))
			চালু;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    জন্য (n = mlen; --n >= 0; )
			যদি (*s++ & (RM_NONE|RM_ABBR))
			    বিরতি;
		    যদি (!is_plug_map && n >= 0)
			চালু;

		    যদি (keylen > typebuf.tb_len)
		    {
			যদি (!*timedout && !(mp_match != ফাঁকা
							&& mp_match->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    বিরতি;
			}
		    }
		    অন্যথা যদি (keylen > mp_match_len)
		    {
			// found a longer match
			mp_match = mp;
			mp_match_len = keylen;
		    }
		}
		অন্যথা
		{
		    // No match; may have to check for termcode at next
		    // character.

		    // If the first character that didn't match is
		    // K_SPECIAL then check for a termcode.  This isn't perfect
		    // but should work in most cases.
		    যদি (max_mlen < mlen)
		    {
			max_mlen = mlen;
			want_termcode = mp->m_চাবিগুলি[mlen] == K_SPECIAL;
		    }
		    অন্যথা যদি (max_mlen == mlen && mp->m_চাবিগুলি[mlen] == K_SPECIAL)
			want_termcode = 1;

		    // Check termcode for uppercase character to properly
		    // process "ESC[27;2;<ascii code>~" control sequences.
		    যদি (ASCII_ISUPPER(mp->m_চাবিগুলি[mlen]))
			want_termcode = 1;
		}
	    }
	}

	// If no partly match found, use the longest full match.
	যদি (keylen != KEYLEN_PART_MAP && mp_match != ফাঁকা)
	{
	    mp = mp_match;
	    keylen = mp_match_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    যদি (!in_osc && *p_pt != NUL && mp == ফাঁকা &&
	    (State & (MODE_ঢোকাও | MODE_NORMAL)))
    {
	জন্য (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    যদি (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    বিরতি;
	যদি (p_pt[mlen] == NUL)	// match
	{
	    // write chars to script file(s)
	    যদি (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    set_option_মান_give_err((char_u *)"paste",
						      (long)!p_paste, ফাঁকা, 0);
	    যদি (!(State & MODE_ঢোকাও))
	    {
		বার্তা_col = 0;
		বার্তা_row = Rows - 1;
		বার্তা_clr_eos();		// clear ruler
	    }
	    status_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    ফেরত map_ফলাফল_retry;
	}
	// Need more chars for partly match.
	যদি (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_চাবি;
	অন্যথা যদি (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    // May check for a terminal code when there is no mapping or only a partial
    // mapping.  Also check if there is a full mapping with <Esc>, unless timed
    // out, since that is nearly always a partial match with a terminal code.
    যদি (in_osc || ((mp == ফাঁকা || max_mlen + want_termcode > mp_match_len
		    || (mp_match_len == 1 && *mp->m_চাবিগুলি == ESC && !*timedout))
	    && keylen != KEYLEN_PART_MAP))
    {
	পূর্ণসংখ্যা	save_keylen = keylen;

	/*
	 * When no matching mapping found or found a non-matching mapping that
	 * matches at least what the matching mapping matched:
	 * Check if we have a terminal code, when:
	 * - mapping is allowed,
	 * - keys have not been mapped,
	 * - and not an ESC sequence, not in insert mode or p_ek is on,
	 * - and when not timed out,
	 */
	যদি (in_osc || no_mapping == 0 || allow_চাবিগুলি != 0)
	{
	    যদি (in_osc || ((typebuf.tb_maplen == 0
			    || (p_remap && (typebuf.tb_noremap[typebuf.tb_off]
						& ~RM_SIMPLIFIED) == RM_YES))
		    && !*timedout))
		keylen = check_termcode(max_mlen + 1, ফাঁকা, 0, ফাঁকা);
	    অন্যথা
		keylen = 0;

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    যদি (keylen == 0 && save_keylen == KEYLEN_PART_চাবি && !*timedout)
		keylen = KEYLEN_PART_চাবি;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
#ifdef FEAT_TERMINAL
	    check_no_reduce_চাবিগুলি();  // may update the no_reduce_keys flag
#endif
	    যদি (keylen == 0 && !no_reduce_চাবিগুলি)
	    {
		keylen = check_simplify_modifier(max_mlen + 1);
		যদি (keylen < 0)
		    // ins_typebuf() failed
		    ফেরত map_ফলাফল_fail;
	    }

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    যদি (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	অন্যথা
	    keylen = 0;
	যদি (keylen == 0)	// no matching terminal code
	{
#ifdef AMIGA
	    // check for window bounds report
	    যদি (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		জন্য (s = typebuf.tb_buf + typebuf.tb_off + 1;
			   s < typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len
		   && (VIM_ISDIGIT(*s) || *s == ';' || *s == ' ');
			++s)
		    ;
		যদি (*s == 'r' || *s == '|') // found one
		{
		    del_typebuf(
			  (পূর্ণসংখ্যা)(s + 1 - (typebuf.tb_buf + typebuf.tb_off)), 0);
		    // get size and redraw screen
		    shell_resized();
		    *keylenp = keylen;
		    ফেরত map_ফলাফল_retry;
		}
		যদি (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_চাবি;
	    }
	    যদি (keylen >= 0)
#endif
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		যদি (mp == ফাঁকা)
		{
		    *keylenp = keylen;
		    ফেরত map_ফলাফল_get;    // get character from typeahead
		}
	}

	যদি (keylen > 0)	    // full matching terminal code
	{
#যদি defined(FEAT_GUI) && defined(FEAT_MENU)
	    যদি (typebuf.tb_len >= 2
		    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
			      && typebuf.tb_buf[typebuf.tb_off + 1] == KS_MENU)
	    {
		পূর্ণসংখ্যা	idx;

		// Using a menu may cause a break in undo!  It's like using
		// gotchars(), but without recording or writing to a script
		// file.
		may_sync_undo();
		del_typebuf(3, 0);
		idx = get_menu_সূচক(current_menu, local_State);
		যদি (idx != MENU_সূচক_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    যদি (VIsual_active && VIsual_select
					&& (current_menu->modes & MODE_VISUAL))
		    {
			VIsual_select = FALSE;
			(শূন্য)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->strings[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#endif // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    ফেরত map_ফলাফল_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	যদি (mp == ফাঁকা || keylen < 0)
	    keylen = KEYLEN_PART_চাবি;
	অন্যথা
	    keylen = mp_match_len;
    }

    /*
     * complete match
     */
    যদি (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#ifdef FEAT_EVAL
	পূর্ণসংখ্যা	save_m_রাশিমালা;
	পূর্ণসংখ্যা	save_m_noremap;
	পূর্ণসংখ্যা	save_m_silent;
	char_u	*save_m_চাবিগুলি;
	char_u	*save_alt_m_চাবিগুলি;
	পূর্ণসংখ্যা	save_alt_m_keylen;
#অন্যথা
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#endif

	// write chars to script file(s)
	যদি (keylen > typebuf.tb_maplen)
	    gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						   keylen - typebuf.tb_maplen);

	cmd_silent = (typebuf.tb_silent > 0);
	del_typebuf(keylen, 0);	// remove the mapped keys

	/*
	 * Put the replacement string in front of mapstr.
	 * The depth check catches ":map x y" and ":map y x".
	 */
	যদি (++*mapdepth >= p_mmd)
	{
	    emsg(_(e_recursive_mapping));
	    যদি (State & MODE_CMDLINE)
		redrawcmdline();
	    অন্যথা
		setcursor();
	    flush_buffers(FLUSH_MINIMAL);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    ফেরত map_ফলাফল_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	যদি (VIsual_active && VIsual_select && (mp->m_mode & MODE_VISUAL))
	{
	    VIsual_select = FALSE;
	    (শূন্য)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#ifdef FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_রাশিমালা = mp->m_রাশিমালা;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_চাবিগুলি = ফাঁকা;  // only saved when needed
	save_alt_m_চাবিগুলি = ফাঁকা;  // only saved when needed
	save_alt_m_keylen = mp->m_alt != ফাঁকা ? mp->m_alt->m_keylen : 0;

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	যদি (mp->m_রাশিমালা)
	{
	    পূর্ণসংখ্যা save_vgetc_busy = vgetc_busy;
	    পূর্ণসংখ্যা save_may_garbage_collect = may_garbage_collect;
	    পূর্ণসংখ্যা was_screen_col = screen_cur_col;
	    পূর্ণসংখ্যা was_screen_row = screen_cur_row;
	    পূর্ণসংখ্যা পূর্ব_did_emsg = did_emsg;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_চাবিগুলি = vim_strnsave(mp->m_চাবিগুলি, (size_t)mp->m_keylen);
	    save_alt_m_চাবিগুলি = mp->m_alt != ফাঁকা
				    ? vim_strnsave(mp->m_alt->m_চাবিগুলি,
					     (size_t)save_alt_m_keylen) : ফাঁকা;
	    map_str = eval_map_রাশিমালা(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_row, was_screen_col);
	    out_flush();

	    // If an error was displayed and the expression returns an empty
	    // string, generate a <Nop> to allow for a redraw.
	    যদি (পূর্ব_did_emsg != did_emsg
				       && (map_str == ফাঁকা || *map_str == NUL))
	    {
		char_u	buf[4];

		vim_free(map_str);
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = KE_IGNORE;
		buf[3] = NUL;
		map_str = vim_strnsave(buf, 3);
		যদি (State & MODE_CMDLINE)
		{
		    // redraw the command below the error
		    বার্তা_didout = TRUE;
		    যদি (বার্তা_row < cmdline_row)
			বার্তা_row = cmdline_row;
		    redrawcmd();
		}
	    }

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	অন্যথা
#endif
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	যদি (map_str == ফাঁকা)
	    i = FAIL;
	অন্যথা
	{
	    পূর্ণসংখ্যা noremap;

#ifdef FEAT_EVAL
	    last_used_map = mp;
	    last_used_sid = -1;
#endif
	    যদি (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    অন্যথা যদি (
#ifdef FEAT_EVAL
		save_m_রাশিমালা ?
		(save_m_চাবিগুলি != ফাঁকা
			&& STRNCMP(map_str, save_m_চাবিগুলি, (size_t)keylen) == 0)
		|| (save_alt_m_চাবিগুলি != ফাঁকা
			&& STRNCMP(map_str, save_alt_m_চাবিগুলি,
					    (size_t)save_alt_m_keylen) == 0) :
#endif
		STRNCMP(map_str, mp->m_চাবিগুলি, (size_t)keylen) == 0
		|| (mp->m_alt != ফাঁকা
			&& STRNCMP(map_str, mp->m_alt->m_চাবিগুলি,
					    (size_t)mp->m_alt->m_keylen) == 0))
		noremap = REMAP_SKIP;
	    অন্যথা
		noremap = REMAP_YES;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#ifdef FEAT_EVAL
	    যদি (save_m_রাশিমালা)
		vim_free(map_str);
#endif
	}
#ifdef FEAT_EVAL
	vim_free(save_m_চাবিগুলি);
	vim_free(save_alt_m_চাবিগুলি);
#endif
	*keylenp = keylen;
	যদি (i == FAIL)
	    ফেরত map_ফলাফল_fail;
	ফেরত map_ফলাফল_retry;
    }

    *keylenp = keylen;
    ফেরত map_ফলাফল_nomatch;
}

/*
 * unget one character (can only be done once!)
 * If the character was stuffed, vgetc() will get it next time it is called.
 * Otherwise vgetc() will only get it when the stuff buffer is empty.
 */
    শূন্য
vungetc(পূর্ণসংখ্যা c)
{
    old_char = c;
    old_mod_mask = mod_mask;
    old_mouse_row = mouse_row;
    old_mouse_col = mouse_col;
    old_চাবি_Stuffed = চাবি_Stuffed;
}

/*
 * When peeking and not getting a character, reg_executing cannot be cleared
 * yet, so set a flag to clear it later.
 */
    স্থির শূন্য
check_end_reg_executing(পূর্ণসংখ্যা advance)
{
    যদি (reg_executing != 0 && (typebuf.tb_maplen == 0
						|| pending_end_reg_executing))
    {
	যদি (advance)
	{
	    reg_executing = 0;
	    pending_end_reg_executing = FALSE;
	}
	অন্যথা
	    pending_end_reg_executing = TRUE;
    }

}

/*
 * Get a byte:
 * 1. from the stuffbuffer
 *	This is used for abbreviated commands like "D" -> "d$".
 *	Also used to redo a command for ".".
 * 2. from the typeahead buffer
 *	Stores text obtained previously but not used yet.
 *	Also stores the result of mappings.
 *	Also used for the ":normal" command.
 * 3. from the user
 *	This may do a blocking wait if "advance" is TRUE.
 *
 * if "advance" is TRUE (vgetc()):
 *	Really get the character.
 *	KeyTyped is set to TRUE in the case the user typed the key.
 *	KeyStuffed is TRUE if the character comes from the stuff buffer.
 * if "advance" is FALSE (vpeekc()):
 *	Just look whether there is a character available.
 *	Return NUL if not.
 *
 * When "no_mapping" is zero, checks for mappings in the current mode.
 * Only returns one byte (of a multi-byte character).
 * K_SPECIAL and CSI may be escaped, need to get two more bytes then.
 */
    স্থির পূর্ণসংখ্যা
vgetorpeek(পূর্ণসংখ্যা advance)
{
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		timedout = FALSE;	// waited for more than 'timeoutlen'
					// for mapping to complete or
					// 'ttimeoutlen' for complete key code
    পূর্ণসংখ্যা		mapdepth = 0;		// check for recursive mapping
    পূর্ণসংখ্যা		mode_deleted = FALSE;   // set when mode has been deleted
    পূর্ণসংখ্যা		new_wcol, new_wrow;
#ifdef FEAT_GUI
    পূর্ণসংখ্যা		shape_changed = FALSE;  // adjusted cursor shape
#endif
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		old_wcol, old_wrow;
    পূর্ণসংখ্যা		wait_tb_len;

    /*
     * This function doesn't work very well when called recursively.  This may
     * happen though, because of:
     * 1. The call to add_to_showcmd().	char_avail() is then used to check if
     * there is a character available, which calls this function.  In that
     * case we must return NUL, to indicate no character is available.
     * 2. A GUI callback function writes to the screen, causing a
     * wait_return().
     * Using ":normal" can also do this, but it saves the typeahead buffer,
     * thus it should be OK.  But don't get a key from the user then.
     */
    যদি (vgetc_busy > 0 && ex_normal_busy == 0)
	ফেরত NUL;

    ++vgetc_busy;

    যদি (advance)
    {
	চাবি_Stuffed = FALSE;
	typebuf_was_empty = FALSE;
    }

    init_typebuf();
    start_stuff();
    check_end_reg_executing(advance);
    করো
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	যদি (typeahead_char != 0)
	{
	    c = typeahead_char;
	    যদি (advance)
		typeahead_char = 0;
	}
	অন্যথা
	    c = read_readbuffers(advance);
	যদি (c != NUL && !got_int)
	{
	    যদি (advance)
	    {
		// KeyTyped = FALSE;  When the command that stuffed something
		// was typed, behave like the stuffed command was typed.
		// needed for CTRL-W CTRL-] to open a fold, for example.
		চাবি_Stuffed = TRUE;
	    }
	    যদি (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	// no abbreviations now
	}
	অন্যথা
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    জন্য (;;)
	    {
		long	wait_time;
		পূর্ণসংখ্যা	keylen = 0;
		পূর্ণসংখ্যা	showcmd_idx;
		check_end_reg_executing(advance);
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		যদি (typebuf.tb_maplen)
		    সারি_breakcheck();
		অন্যথা
		    ui_breakcheck();		// check for CTRL-C
		যদি (got_int)
		{
		    // flush all input
		    c = inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 0L);

		    /*
		     * If inchar() returns TRUE (script file was active) or we
		     * are inside a mapping, get out of Insert mode.
		     * Otherwise we behave like having gotten a CTRL-C.
		     * As a result typing CTRL-C in insert mode will
		     * really insert a CTRL-C.
		     */
		    যদি ((c || typebuf.tb_maplen)
				     && (State & (MODE_ঢোকাও | MODE_CMDLINE)))
			c = ESC;
		    অন্যথা
			c = Ctrl_C;
		    flush_buffers(FLUSH_প্রবেশ);	// flush all typeahead

		    যদি (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    বিরতি;
		}
		অন্যথা যদি (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_ফলাফল_T ফলাফল = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    যদি (ফলাফল == map_ফলাফল_retry)
			// try mapping again
			চালু;
		    যদি (ফলাফল == map_ফলাফল_fail)
		    {
			// failed, use the outer loop
			c = -1;
			বিরতি;
		    }
		    যদি (ফলাফল == map_ফলাফল_get)
		    {
/*
 * get a character: 2. from the typeahead buffer
 */
			c = typebuf.tb_buf[typebuf.tb_off];
			যদি (advance)	// remove chars from typebuf
			{
			    cmd_silent = (typebuf.tb_silent > 0);
			    যদি (typebuf.tb_maplen > 0)
				চাবি_Typed = FALSE;
			    অন্যথা
			    {
				চাবি_Typed = TRUE;
				// write char to script file(s)
				gotchars(typebuf.tb_buf
						 + typebuf.tb_off, 1);
			    }
			    চাবি_Noremap = typebuf.tb_noremap[typebuf.tb_off]
							    & ~RM_SIMPLIFIED;
			    del_typebuf(1, 0);
			}
			বিরতি;  // got character, break the for loop
		    }

		    // not enough characters, get more
		}

/*
 * get a character: 3. from the user - handle <Esc> in Insert mode
 */
		/*
		 * Special case: if we get an <ESC> in Insert mode and there
		 * are no more characters at once, we pretend to go out of
		 * Insert mode.  This prevents the one second delay after
		 * typing an <ESC>.  If we get something after all, we may
		 * have to redisplay the mode. That the cursor is in the wrong
		 * place does not matter.
		 * Do not do this if the kitty keyboard protocol is used, every
		 * <ESC> is the start of an escape sequence then.
		 */
		c = 0;
		new_wcol = curwin->w_wcol;
		new_wrow = curwin->w_wrow;
		যদি (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& kitty_protocol_state != KKPS_ENABLED
			&& ex_normal_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & MODE_ঢোকাও)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_চাবি && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	col = 0;
		    char_u	*ptr;

		    যদি (mode_displayed)
		    {
			unshowmode(TRUE);
			mode_deleted = TRUE;
		    }
#ifdef FEAT_GUI
		    // may show a different cursor shape
		    যদি (gui.in_use && State != MODE_NORMAL && !cmd_silent)
		    {
			পূর্ণসংখ্যা	    save_State;

			save_State = State;
			State = MODE_NORMAL;
			gui_update_cursor(TRUE, FALSE);
			State = save_State;
			shape_changed = TRUE;
		    }
#endif
		    validate_cursor();
		    old_wcol = curwin->w_wcol;
		    old_wrow = curwin->w_wrow;

		    // move cursor left, if possible
		    যদি (curwin->w_cursor.col != 0)
		    {
			যদি (curwin->w_wcol > 0)
			{
			    // After auto-indenting and no text is following,
			    // we are expecting to truncate the trailing
			    // white-space, so find the last non-white
			    // character -- webb
			    যদি (did_ai && *skipwhite(ml_get_curline()
						+ curwin->w_cursor.col) == NUL)
			    {
				chartabsize_T cts;

				curwin->w_wcol = 0;
				ptr = ml_get_curline();
				init_chartabsize_arg(&cts, curwin,
					  curwin->w_cursor.lnum, 0, ptr, ptr);
				যখন (cts.cts_ptr < ptr + curwin->w_cursor.col)
				{
				    যদি (!VIM_ISWHITE(*cts.cts_ptr))
					curwin->w_wcol = cts.cts_vcol;
				    cts.cts_vcol += lbr_chartabsize(&cts);
				    যদি (has_mbyte)
					cts.cts_ptr +=
						   (*mb_ptr2len)(cts.cts_ptr);
				    অন্যথা
					++cts.cts_ptr;
				}
				clear_chartabsize_arg(&cts);

				curwin->w_wrow = curwin->w_cline_row
					   + curwin->w_wcol / curwin->w_width;
				curwin->w_wcol %= curwin->w_width;
				curwin->w_wcol += curwin_col_off();
				col = 0;	// no correction needed
			    }
			    অন্যথা
			    {
				--curwin->w_wcol;
				col = curwin->w_cursor.col - 1;
			    }
			}
			অন্যথা যদি (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_width - 1;
			    col = curwin->w_cursor.col - 1;
			}
			যদি (has_mbyte && col > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    col -= (*mb_শীর্ষ_off)(ptr, ptr + col);
			    যদি ((*mb_ptr2cells)(ptr + col) > 1)
				--curwin->w_wcol;
			}
		    }
		    setcursor();
		    out_flush();
		    new_wcol = curwin->w_wcol;
		    new_wrow = curwin->w_wrow;
		    curwin->w_wcol = old_wcol;
		    curwin->w_wrow = old_wrow;
		}
		যদি (c < 0)
		    চালু;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		জন্য (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		যদি (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    চালু;
		}

		যদি (ex_normal_busy > 0)
		{
		    স্থির পূর্ণসংখ্যা tc = 0;

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    যদি (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			চালু;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    যদি (p_im && (State & MODE_ঢোকাও))
			c = Ctrl_L;
#ifdef FEAT_TERMINAL
		    অন্যথা যদি (terminal_is_active())
			c = K_CANCEL;
#endif
		    অন্যথা যদি ((State & MODE_CMDLINE)
					     || (cmdwin_type > 0 && tc == ESC))
			c = Ctrl_C;
		    অন্যথা
			c = ESC;
		    tc = c;
		    // set a flag to indicate this wasn't a normal char
		    যদি (advance)
			typebuf_was_empty = TRUE;

		    // return from main_loop()
		    যদি (pending_exmode_active)
			exmode_active = EXMODE_NORMAL;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    বিরতি;
		}

/*
 * get a character: 3. from the user - update display
 */
		// In insert mode a screen update is skipped when characters
		// are still available.  But when those available characters
		// are part of a mapping, and we are going to do a blocking
		// wait here.  Need to update the screen to display the
		// changed text so far. Also for when 'lazyredraw' is set and
		// redrawing was postponed because there was something in the
		// input buffer (e.g., termresponse).
		যদি (((State & MODE_ঢোকাও) != 0 || p_lz)
			&& (State & MODE_CMDLINE) == 0
			&& advance && must_redraw != 0 && !need_wait_return)
		{
		    update_screen(0);
		    setcursor(); // put cursor back where it belongs
		}

		/*
		 * If we have a partial match (and are going to wait for more
		 * input from the user), show the partially matched characters
		 * to the user with showcmd.
		 */
		showcmd_idx = 0;
		পূর্ণসংখ্যা showing_partial = FALSE;
		যদি (typebuf.tb_len > 0 && advance && !exmode_active)
		{
		    যদি (((State & (MODE_NORMAL | MODE_ঢোকাও))
						      || State == MODE_LANGMAP)
			    && State != MODE_HITRETURN)
		    {
			// this looks nice when typing a dead character map
			যদি (State & MODE_ঢোকাও
			    && ptr2cells(typebuf.tb_buf + typebuf.tb_off
						   + typebuf.tb_len - 1) == 1)
			{
			    edit_putchar(typebuf.tb_buf[typebuf.tb_off
						+ typebuf.tb_len - 1], FALSE);
			    setcursor(); // put cursor back where it belongs
			    showing_partial = TRUE;
			}
			// need to use the col and row from above here
			old_wcol = curwin->w_wcol;
			old_wrow = curwin->w_wrow;
			curwin->w_wcol = new_wcol;
			curwin->w_wrow = new_wrow;
			ঠেলো_showcmd();
			যদি (typebuf.tb_len > SHOWCMD_COLS)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_COLS;
			যখন (showcmd_idx < typebuf.tb_len)
			    add_byte_to_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
		    }

		    // This looks nice when typing a dead character map.
		    // There is no actual command line for get_number().
		    যদি ((State & MODE_CMDLINE)
			    && get_cmdline_info()->cmdbuff != ফাঁকা
#যদি defined(FEAT_CRYPT) || defined(FEAT_EVAL)
			    && cmdline_star == 0
#endif
			    && ptr2cells(typebuf.tb_buf + typebuf.tb_off
						   + typebuf.tb_len - 1) == 1)
		    {
			putcmdline(typebuf.tb_buf[typebuf.tb_off
						+ typebuf.tb_len - 1], FALSE);
			showing_partial = TRUE;
		    }
		}

/*
 * get a character: 3. from the user - get it
 */
		যদি (typebuf.tb_len == 0)
		    // timedout may have been set if a mapping with empty RHS
		    // fully matched while longer mappings timed out.
		    timedout = FALSE;

		যদি (advance)
		{
		    যদি (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_চাবি)))
			// blocking wait
			wait_time = -1L;
		    অন্যথা যদি (keylen == KEYLEN_PART_চাবি && p_ttm >= 0)
			wait_time = p_ttm;
		    অন্যথা
			wait_time = p_tm;
		}
		অন্যথা
		    wait_time = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_time);

		যদি (showcmd_idx != 0)
		    বের_করো_showcmd();
		যদি (showing_partial)
		{
		    যদি (State & MODE_ঢোকাও)
			edit_unputchar();
		    যদি ((State & MODE_CMDLINE)
					&& get_cmdline_info()->cmdbuff != ফাঁকা)
			unputcmdline();
		    অন্যথা
			setcursor();	// put cursor back where it belongs
		}

		যদি (c < 0)
		    চালু;		// end of input script reached
		যদি (c == NUL)		// no character available
		{
		    যদি (!advance)
			বিরতি;
		    যদি (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			চালু;
		    }
		}
		অন্যথা
		{	    // allow mapping for just typed characters
		    যখন (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#ifdef HAVE_প্রবেশ_METHOD
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_active = im_get_status();
#endif
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } যখন ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    যদি (advance && p_smd && বার্তা_silent == 0 && (State & MODE_ঢোকাও))
    {
	যদি (c == ESC && !mode_deleted && !no_mapping && mode_displayed)
	{
	    যদি (typebuf.tb_len && !চাবি_Typed)
		redraw_cmdline = TRUE;	    // delete mode later
	    অন্যথা
		unshowmode(FALSE);
	}
	অন্যথা যদি (c != ESC && mode_deleted)
	{
	    যদি (typebuf.tb_len && !চাবি_Typed)
		redraw_cmdline = TRUE;	    // show mode later
	    অন্যথা
		showmode();
	}
    }
#ifdef FEAT_GUI
    // may unshow different cursor shape
    যদি (gui.in_use && shape_changed)
	gui_update_cursor(TRUE, FALSE);
#endif
    যদি (timedout && c == ESC)
    {
	// When recording there will be no timeout.  Add an <Ignore> after the
	// ESC to avoid that it forms a key code with following characters.
	gotchars_ignore();
    }

    --vgetc_busy;

    ফেরত c;
}

/*
 * inchar() - get one character from
 *	1. a scriptfile
 *	2. the keyboard
 *
 *  As many characters as we can get (up to 'maxlen') are put in "buf" and
 *  NUL terminated (buffer length must be 'maxlen' + 1).
 *  Minimum for "maxlen" is 3!!!!
 *
 *  "tb_change_cnt" is the value of typebuf.tb_change_cnt if "buf" points into
 *  it.  When typebuf.tb_change_cnt changes (e.g., when a message is received
 *  from a remote client) "buf" can no longer be used.  "tb_change_cnt" is 0
 *  otherwise.
 *
 *  If we got an interrupt all input is read until none is available.
 *
 *  If wait_time == 0  there is no waiting for the char.
 *  If wait_time == n  we wait for n msec for a character to arrive.
 *  If wait_time == -1 we wait forever for a character to arrive.
 *
 *  Return the number of obtained characters.
 *  Return -1 when end of input script reached.
 */
    স্থির পূর্ণসংখ্যা
inchar(
    char_u	*buf,
    পূর্ণসংখ্যা		maxlen,
    long	wait_time)	    // milliseconds
{
    পূর্ণসংখ্যা		len = 0;	    // init for GCC
    পূর্ণসংখ্যা		retesc = FALSE;	    // return ESC with gotint
    পূর্ণসংখ্যা		script_char;
    পূর্ণসংখ্যা		tb_change_cnt = typebuf.tb_change_cnt;

    যদি (wait_time == -1L || wait_time > 100L)  // flush output before waiting
    {
	cursor_on();
	out_flush_cursor(FALSE, FALSE);
#যদি defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	যদি (gui.in_use && postponed_mouseshape)
	    update_mouseshape(-1);
#endif
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    যদি (State != MODE_HITRETURN)
    {
	did_outofmem_বার্তা = FALSE;   // display out of memory message (again)
	did_swapwrite_বার্তা = FALSE;  // display swap file write error again
    }
    undo_off = FALSE;		    // restart undo now

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    যখন (scriptin[curscript] != ফাঁকা && script_char < 0
#ifdef FEAT_EVAL
	    && !ignore_script
#endif
	    )
    {
#ifdef MESSAGE_QUEUE
	parse_queued_messages();
#endif

	যদি (got_int || (script_char = getc(scriptin[curscript])) < 0)
	{
	    // Reached EOF.
	    // Careful: closescript() frees typebuf.tb_buf[] and buf[] may
	    // point inside typebuf.tb_buf[].  Don't use buf[] after this!
	    closescript();
	    /*
	     * When reading script file is interrupted, return an ESC to get
	     * back to normal mode.
	     * Otherwise return -1, because typebuf.tb_buf[] has changed.
	     */
	    যদি (got_int)
		retesc = TRUE;
	    অন্যথা
		ফেরত -1;
	}
	অন্যথা
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    যদি (script_char < 0)	// did not get a character from script
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	যদি (got_int)
	{
#define DUM_LEN (MAXMAPLEN * 3 + 3)
	    char_u	dum[DUM_LEN + 1];

	    জন্য (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		যদি (len == 0 || (len == 1 && dum[0] == Ctrl_C))
		    বিরতি;
	    }
	    ফেরত retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	যদি (wait_time == -1L || wait_time > 10L)
	    out_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_time, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    যদি (typebuf_changed(tb_change_cnt))
	ফেরত 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    যদি (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    ফেরত fix_প্রবেশ_বাফার(buf, len);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * "buf[]" must have room to triple the number of bytes!
 * Returns the new length.
 */
    পূর্ণসংখ্যা
fix_প্রবেশ_বাফার(char_u *buf, পূর্ণসংখ্যা len)
{
    পূর্ণসংখ্যা		i;
    char_u	*p = buf;

    /*
     * Two characters are special: NUL and K_SPECIAL.
     * When compiled With the GUI CSI is also special.
     * Replace	     NUL by K_SPECIAL KS_ZERO	 KE_FILLER
     * Replace K_SPECIAL by K_SPECIAL KS_SPECIAL KE_FILLER
     * Replace       CSI by K_SPECIAL KS_EXTRA   KE_CSI
     */
    জন্য (i = len; --i >= 0; ++p)
    {
#ifdef FEAT_GUI
	// When the GUI is used any character can come after a CSI, don't
	// escape it.
	যদি (gui.in_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
# ifndef MSWIN
	// When not on MS-Windows and the GUI is not used CSI needs to be
	// escaped.
	অন্যথা যদি (!gui.in_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (পূর্ণসংখ্যা)KE_CSI;
	    len += 2;
	}
# endif
	অন্যথা
#endif
	যদি (p[0] == NUL || (p[0] == K_SPECIAL
		    // timeout may generate K_CURSORHOLD,
		    // 'autocompletedelay' timeout K_COMPLETE_DELAY
		    && (i < 2 || p[1] != KS_EXTRA
			|| (p[2] != (পূর্ণসংখ্যা)KE_CURSORHOLD
			    && p[2] != (পূর্ণসংখ্যা)KE_COMPLETE_DELAY))
#যদি defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
		    // Win32 console passes modifiers
		    && (
# ifdef VIMDLL
			gui.in_use ||
# endif
			(i < 2 || p[1] != KS_MODIFIER))
#endif
		    ))
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    p[2] = K_THIRD(p[0]);
	    p[1] = K_SECOND(p[0]);
	    p[0] = K_SPECIAL;
	    p += 2;
	    len += 2;
	}
    }
    *p = NUL;		// add trailing NUL
    ফেরত len;
}

#যদি defined(USE_প্রবেশ_BUF)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    পূর্ণসংখ্যা
প্রবেশ_available(শূন্য)
{
    ফেরত (!vim_is_প্রবেশ_buf_empty()
# যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#endif

/*
 * Function passed to do_cmdline() to get the command after a <Cmd> key from
 * typeahead.
 */
    স্থির char_u *
getcmdkeycmd(
	পূর্ণসংখ্যা		promptc UNUSED,
	শূন্য		*cookie UNUSED,
	পূর্ণসংখ্যা		indent UNUSED,
	getline_opt_T	do_concat UNUSED)
{
    garray_T	সারি_ga;
    পূর্ণসংখ্যা		c1 = -1;
    পূর্ণসংখ্যা		c2;
    পূর্ণসংখ্যা		cmod = 0;
    পূর্ণসংখ্যা		aborted = FALSE;

    ga_init2(&সারি_ga, 1, 32);

    // no mapping for these characters
    no_mapping++;

    got_int = FALSE;
    যখন (c1 != NUL && !aborted)
    {
	যদি (ga_grow(&সারি_ga, 32) == FAIL)
	{
	    aborted = TRUE;
	    বিরতি;
	}

	যদি (vgetorpeek(FALSE) == NUL)
	{
	    // incomplete <Cmd> is an error, because there is not much the user
	    // could do in this state.
	    emsg(_(e_cmd_mapping_must_end_with_cr));
	    aborted = TRUE;
	    বিরতি;
	}

	// Get one character at a time.
	c1 = vgetorpeek(TRUE);

	// Get two extra bytes for special keys
	যদি (c1 == K_SPECIAL)
	{
	    c1 = vgetorpeek(TRUE);
	    c2 = vgetorpeek(TRUE);
	    যদি (c1 == KS_MODIFIER)
	    {
		cmod = c2;
		চালু;
	    }
	    c1 = TO_SPECIAL(c1, c2);

	    // K_ESC is used to avoid ambiguity with the single Esc character
	    // that might be the start of an escape sequence.  Convert it back
	    // to a single Esc here.
	    যদি (c1 == K_ESC)
		c1 = ESC;

#ifdef FEAT_GUI
	    // Translate K_CSI to CSI.  The special key is only used to
	    // avoid it being recognized as the start of a special key.
	    যদি (c1 == K_CSI)
		c1 = CSI;
#endif
	}

	যদি (got_int)
	    aborted = TRUE;
	অন্যথা যদি (c1 == '\r' || c1 == '\n')
	    c1 = NUL;  // end the line
	অন্যথা যদি (c1 == ESC)
	    aborted = TRUE;
	অন্যথা যদি (c1 == K_COMMAND || c1 == K_SCRIPT_COMMAND)
	{
	    // give a nicer error message for this special case
	    emsg(_(e_cmd_mapping_must_end_with_cr_before_second_cmd));
	    aborted = TRUE;
	}
	অন্যথা যদি (c1 == K_SNR)
	{
	    GA_CONCAT_LITERAL(&সারি_ga, "<SNR>");
	}
	অন্যথা
	{
	    যদি (cmod != 0)
	    {
		ga_append(&সারি_ga, K_SPECIAL);
		ga_append(&সারি_ga, KS_MODIFIER);
		ga_append(&সারি_ga, cmod);
	    }
	    যদি (IS_SPECIAL(c1))
	    {
		ga_append(&সারি_ga, K_SPECIAL);
		ga_append(&সারি_ga, K_SECOND(c1));
		ga_append(&সারি_ga, K_THIRD(c1));
	    }
	    অন্যথা
		ga_append(&সারি_ga, c1);
	}

	cmod = 0;
    }

    no_mapping--;

    যদি (aborted)
	ga_clear(&সারি_ga);

    ফেরত (char_u *)সারি_ga.ga_তথ্য;
}

#যদি defined(FEAT_EVAL)
/*
 * If there was a mapping we get its SID.  Otherwise, use "last_used_sid", it
 * is set when redo'ing.
 * Put this SID in the redo buffer, so that "." will use the same script
 * context.
 */
    শূন্য
may_add_last_used_map_to_redobuff(শূন্য)
{
    char_u  buf[3 + 20];
    পূর্ণসংখ্যা	    buflen;
    পূর্ণসংখ্যা	    sid = -1;

    যদি (last_used_map != ফাঁকা)
	sid = last_used_map->m_script_ctx.sc_sid;
    যদি (sid < 0)
	sid = last_used_sid;

    যদি (sid < 0)
	ফেরত;

    // <K_SID>{nr};
    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = KE_SID;
    buflen = 3;

    buflen += vim_snprintf((অক্ষর *)buf + 3, 20, "%d;", sid);
    add_buff(&redobuff, buf, (long)buflen);
}
#endif

    পূর্ণসংখ্যা
do_cmdkey_command(পূর্ণসংখ্যা চাবি UNUSED, পূর্ণসংখ্যা flags)
{
    পূর্ণসংখ্যা	    res;
#ifdef FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    যদি (চাবি == K_SCRIPT_COMMAND
		  && (last_used_map != ফাঁকা || SCRIPT_ID_VALID(last_used_sid)))
    {
	save_current_sctx = current_sctx;
	যদি (last_used_map != ফাঁকা)
	    current_sctx = last_used_map->m_script_ctx;
	অন্যথা
	{
	    current_sctx.sc_sid = last_used_sid;
	    current_sctx.sc_lnum = 0;
	    current_sctx.sc_version = SCRIPT_ITEM(last_used_sid)->sn_version;
	}
    }
#endif

    res = do_cmdline(ফাঁকা, getcmdkeycmd, ফাঁকা, flags);

#ifdef FEAT_EVAL
    যদি (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#endif

    ফেরত res;
}

#যদি defined(FEAT_EVAL)
    শূন্য
reset_last_used_map(mapblock_T *mp)
{
    যদি (last_used_map != mp)
	ফেরত;

    last_used_map = ফাঁকা;
    last_used_sid = -1;
}
#endif
