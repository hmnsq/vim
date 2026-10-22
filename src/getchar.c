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

#شامل "vim.h"

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

#define MINIMAL_SIZE 20			// minimal size for b_str

ایستا buffheader_T redobuff = {{پوچ, 0, {NUL}}, پوچ, 0, 0, FALSE};
ایستا buffheader_T old_redobuff = {{پوچ, 0, {NUL}}, پوچ, 0, 0, FALSE};
ایستا buffheader_T recordbuff = {{پوچ, 0, {NUL}}, پوچ, 0, 0, FALSE};

ایستا صحیح typeahead_char = 0;		// typeahead char that's not flushed

#ifdef FEAT_EVAL
ایستا char_u typedchars[MAXMAPLEN + 1] = { NUL };  // typed chars before map
ایستا صحیح typedchars_pos = 0;
#endif

/*
 * When block_redo is TRUE the redo buffer will not be changed.
 * Used by edit() to repeat insertions.
 */
ایستا صحیح	block_redo = FALSE;

ایستا صحیح	کلید_Noremap = 0;	    // remapping flags

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
#وگرنه // Tiny version
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
#endif
ایستا char_u	typebuf_init[TYPELEN_INIT];	// initial typebuf.tb_buf
ایستا char_u	noremapbuf_init[TYPELEN_INIT];	// initial typebuf.tb_noremap

ایستا size_t	last_recorded_len = 0;	// number of last recorded chars

#ifdef FEAT_EVAL
mapblock_T	*last_used_map = پوچ;
صحیح		last_used_sid = -1;
#endif

ایستا صحیح	read_readbuf(buffheader_T *buf, صحیح advance);
ایستا تهی	init_typebuf(تهی);
ایستا تهی	may_sync_undo(تهی);
ایستا تهی	free_typebuf(تهی);
ایستا تهی	closescript(تهی);
ایستا تهی	updatescript(صحیح c);
ایستا صحیح	vgetorpeek(صحیح);
ایستا صحیح	inchar(char_u *buf, صحیح maxlen, long wait_time);
#ifdef FEAT_EVAL
ایستا صحیح	do_کلید_درون_داد_pre(صحیح c);
#endif

/*
 * Free and clear a buffer.
 */
    ایستا تهی
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    برای (p = buf->bh_first.b_next; p != پوچ; p = np)
    {
	np = p->b_next;
	vim_free(p);
    }
    buf->bh_first.b_next = پوچ;
    buf->bh_curr = پوچ;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    ایستا char_u *
get_buffcont(
    buffheader_T	*buffer,
    صحیح			dozero,	    // count == zero is not an error
    size_t		*len)	    // the length of the returned buffer
{
    long_u	    شمار = 0;
    char_u	    *p = پوچ;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;
    size_t	    i = 0;

    // compute the total length of the string
    برای (bp = buffer->bh_first.b_next; bp != پوچ; bp = bp->b_next)
	شمار += (long_u)bp->b_strlen;

    اگر ((شمار > 0 || dozero) && (p = alloc(شمار + 1)) != پوچ)
    {
	p2 = p;
	برای (bp = buffer->bh_first.b_next; bp != پوچ; bp = bp->b_next)
	    برای (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
	i = (size_t)(p2 - p);
    }

    اگر (len != پوچ)
	*len = i;

    بازگشت p;
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(تهی)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE, &len);
    اگر (p == پوچ)
	بازگشت پوچ;

    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    اگر (len >= last_recorded_len)
    {
	len -= last_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    اگر (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    بازگشت (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    string_T
get_inserted(تهی)
{
    size_t len = 0;
    char_u *str = get_buffcont(&redobuff, FALSE, &len);
    string_T ret = { str, len };
    بازگشت ret;
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    ایستا تهی
add_buff(
    buffheader_T	*buf,
    char_u		*s,
    long		slen)	// length of "s" or -1
{
    اگر (slen < 0)
	slen = (long)STRLEN(s);
    اگر (slen == 0)				// don't add empty strings
	بازگشت;

    اگر (buf->bh_first.b_next == پوچ)	// first add to list
    {
	buf->bh_curr = &(buf->bh_first);
	buf->bh_create_newblock = TRUE;
    }
    وگرنه اگر (buf->bh_curr == پوچ)	// buffer has already been read
    {
	iemsg(e_add_to_internal_buffer_that_was_already_read_from);
	بازگشت;
    }
    وگرنه اگر (buf->bh_index != 0)
    {
	mch_memmove(buf->bh_first.b_next->b_str,
		    buf->bh_first.b_next->b_str + buf->bh_index,
		    (buf->bh_first.b_next->b_strlen - buf->bh_index) + 1);
	buf->bh_first.b_next->b_strlen -= buf->bh_index;
	buf->bh_space += buf->bh_index;
    }
    buf->bh_index = 0;

    اگر (!buf->bh_create_newblock && buf->bh_space >= (صحیح)slen)
    {
	vim_strncpy(buf->bh_curr->b_str + buf->bh_curr->b_strlen, s, (size_t)slen);
	buf->bh_curr->b_strlen += slen;
	buf->bh_space -= slen;
    }
    وگرنه
    {
	long_u	    len;
	buffblock_T *p;

	اگر (slen < MINIMAL_SIZE)
	    len = MINIMAL_SIZE;
	وگرنه
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	اگر (p == پوچ)
	    بازگشت; // no space, just forget it
	vim_strncpy(p->b_str, s, (size_t)slen);
	p->b_strlen = slen;
	buf->bh_space = (صحیح)(len - slen);
	buf->bh_create_newblock = FALSE;

	p->b_next = buf->bh_curr->b_next;
	buf->bh_curr->b_next = p;
	buf->bh_curr = p;
    }
}

/*
 * Delete "slen" bytes from the end of "buf".
 * Only works when it was just added.
 */
    ایستا تهی
delete_buff_tail(buffheader_T *buf, صحیح slen)
{
    اگر (buf->bh_curr == پوچ)
	بازگشت;  // nothing to delete
    اگر (buf->bh_curr->b_strlen < (size_t)slen)
	بازگشت;

    buf->bh_curr->b_str[buf->bh_curr->b_strlen - (size_t)slen] = NUL;
    buf->bh_curr->b_strlen -= slen;
    buf->bh_space += slen;
}

/*
 * Add number "n" to buffer "buf".
 */
    ایستا تهی
add_رقم_buff(buffheader_T *buf, long n)
{
    char_u	عدد[32];
    صحیح		numberlen;

    numberlen = vim_snprintf((نویسه *)عدد, اندازه(عدد), "%ld", n);
    add_buff(buf, عدد, (long)numberlen);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ایستا تهی
add_char_buff(buffheader_T *buf, صحیح c)
{
    char_u	bytes[MB_MAXBYTES + 1];
    صحیح		len;
    صحیح		i;
    char_u	temp[4];
    long	templen;

    اگر (IS_SPECIAL(c))
	len = 1;
    وگرنه
	len = (*mb_char2bytes)(c, bytes);
    برای (i = 0; i < len; ++i)
    {
	اگر (!IS_SPECIAL(c))
	    c = bytes[i];

	اگر (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    temp[0] = K_SPECIAL;
	    temp[1] = K_SECOND(c);
	    temp[2] = K_THIRD(c);
	    temp[3] = NUL;
	    templen = 3;
	}
#ifdef FEAT_GUI
	وگرنه اگر (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    temp[0] = CSI;
	    temp[1] = KS_EXTRA;
	    temp[2] = (صحیح)KE_CSI;
	    temp[3] = NUL;
	    templen = 3;
	}
#endif
	وگرنه
	{
	    temp[0] = c;
	    temp[1] = NUL;
	    templen = 1;
	}
	add_buff(buf, temp, templen);
    }
}

// First read ahead buffer. Used for translated commands.
ایستا buffheader_T readbuf1 = {{پوچ, 0, {NUL}}, پوچ, 0, 0, FALSE};

// Second read ahead buffer. Used for redo.
ایستا buffheader_T readbuf2 = {{پوچ, 0, {NUL}}, پوچ, 0, 0, FALSE};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    ایستا صحیح
read_readbuffers(صحیح advance)
{
    صحیح c;

    c = read_readbuf(&readbuf1, advance);
    اگر (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    بازگشت c;
}

    ایستا صحیح
read_readbuf(buffheader_T *buf, صحیح advance)
{
    char_u	c;
    buffblock_T	*curr;

    اگر (buf->bh_first.b_next == پوچ)  // buffer is empty
	بازگشت NUL;

    curr = buf->bh_first.b_next;
    c = curr->b_str[buf->bh_index];

    اگر (advance)
    {
	اگر (curr->b_str[++buf->bh_index] == NUL)
	{
	    buf->bh_first.b_next = curr->b_next;
	    vim_free(curr);
	    buf->bh_index = 0;
	}
    }
    بازگشت c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    ایستا تهی
start_stuff(تهی)
{
    اگر (readbuf1.bh_first.b_next != پوچ)
    {
	readbuf1.bh_curr = &(readbuf1.bh_first);
	readbuf1.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
    اگر (readbuf2.bh_first.b_next != پوچ)
    {
	readbuf2.bh_curr = &(readbuf2.bh_first);
	readbuf2.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    صحیح
stuff_empty(تهی)
{
    بازگشت (readbuf1.bh_first.b_next == پوچ
	 && readbuf2.bh_first.b_next == پوچ);
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * readbuf2.
 */
    صحیح
readbuf1_empty(تهی)
{
    بازگشت (readbuf1.bh_first.b_next == پوچ);
}
#endif

/*
 * Set a typeahead character that won't be flushed.
 */
    تهی
typeahead_noflush(صحیح c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    تهی
flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    start_stuff();
    وقتی (read_readbuffers(TRUE) != NUL)
	;

    اگر (flush_typeahead == FLUSH_MINIMAL)
    {
	// remove mapped characters at the start only,
	// but only when enough space left in typebuf
	اگر (typebuf.tb_off + typebuf.tb_maplen >= typebuf.tb_buflen)
	{
	    typebuf.tb_off = MAXMAPLEN;
	    typebuf.tb_len = 0;
	}
	وگرنه
	{
	    typebuf.tb_off += typebuf.tb_maplen;
	    typebuf.tb_len -= typebuf.tb_maplen;
	}
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	اگر (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#endif
    }
    وگرنه
    {
	// remove typeahead
	اگر (flush_typeahead == FLUSH_درون_داد)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    وقتی (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#endif
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
    اگر (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * The previous contents of the redo buffer is kept in old_redobuffer.
 * This is used for the CTRL-O <.> command in insert mode.
 */
    تهی
ResetRedobuff(تهی)
{
    اگر (block_redo)
	بازگشت;

    free_buff(&old_redobuff);
    old_redobuff = redobuff;
    redobuff.bh_first.b_next = پوچ;
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    تهی
CancelRedo(تهی)
{
    اگر (block_redo)
	بازگشت;

    free_buff(&redobuff);
    redobuff = old_redobuff;
    old_redobuff.bh_first.b_next = پوچ;
    start_stuff();
    وقتی (read_readbuffers(TRUE) != NUL)
	;
}

/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
    تهی
saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;
    size_t	slen;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_first.b_next = پوچ;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_first.b_next = پوچ;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE, &slen);
    اگر (s == پوچ)
	بازگشت;

    add_buff(&redobuff, s, (long)slen);
    vim_free(s);
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    تهی
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
    تهی
AppendToRedobuff(char_u *s)
{
    اگر (!block_redo)
	add_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    تهی
AppendToRedobuffLit(
    char_u	*str,
    صحیح		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    صحیح		c;
    char_u	*start;

    اگر (block_redo)
	بازگشت;

    وقتی (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	start = s;
	وقتی (*s >= ' ' && *s < DEL && (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	اگر (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	اگر (s > start)
	    add_buff(&redobuff, start, (long)(s - start));

	اگر (*s == NUL || (len >= 0 && s - str >= len))
	    بشکن;

	// Handle a special or multibyte character.
	اگر (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	وگرنه
	    c = *s++;
	اگر (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    add_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048
	اگر (*s == NUL && c == '0')
	    add_buff(&redobuff, (char_u *)"048", 3L);
	وگرنه
	    add_char_buff(&redobuff, c);
    }
}

/*
 * Append "s" to the redo buffer, leaving 3-byte special key codes unmodified
 * and escaping other K_SPECIAL and CSI bytes.
 */
    تهی
AppendToRedobuffSpec(char_u *s)
{
    اگر (block_redo)
	بازگشت;

    وقتی (*s != NUL)
    {
	اگر (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    add_buff(&redobuff, s, 3L);
	    s += 3;
	}
	وگرنه
	    add_char_buff(&redobuff, mb_cptr2char_adv(&s));
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    تهی
AppendCharToRedobuff(صحیح c)
{
    اگر (!block_redo)
	add_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    تهی
Append_عدد_To_Redobuff(long n)
{
    اگر (!block_redo)
	add_رقم_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    تهی
stuffReadbuff(char_u *s)
{
    add_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    تهی
stuffRedoReadbuff(char_u *s)
{
    add_buff(&readbuf2, s, -1L);
}

    تهی
stuffReadbuffLen(char_u *s, long len)
{
#ifdef FEAT_EVAL
    اگر (add_last_درج == 1) // Only add if this is the first call, for
			      // recursive calls, ignore.
	ga_concat_len(&last_درج_ga, s, (size_t)len);
#endif
    add_buff(&readbuf1, s, len);
}

#اگر defined(FEAT_EVAL)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    تهی
stuffReadbuffSpec(char_u *s)
{
    صحیح c;

    وقتی (*s != NUL)
    {
	اگر (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	وگرنه
	{
	    c = mb_cptr2char_adv(&s);
	    اگر (c == CAR || c == NL || c == ESC)
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
    تهی
stuffcharReadbuff(صحیح c)
{
    add_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    تهی
stuffnumReadbuff(long n)
{
    add_رقم_buff(&readbuf1, n);
}

/*
 * Stuff a string into the typeahead buffer, such that edit() will insert it
 * literally ("literally" TRUE) or interpret is as typed characters.
 */
    تهی
stuffescaped(char_u *arg, صحیح literally)
{
    صحیح		c;
    char_u	*start;

    وقتی (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	start = arg;
	وقتی ((*arg >= ' ' && *arg < DEL)
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	اگر (arg > start)
	    stuffReadbuffLen(start, (long)(arg - start));

	// stuff a single special character
	اگر (*arg != NUL)
	{
	    اگر (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    وگرنه
		c = *arg++;
	    اگر (literally && ((c < ' ' && c != TAB) || c == DEL))
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
    ایستا صحیح
read_redo(صحیح init, صحیح old_redo)
{
    ایستا buffblock_T	*bp;
    ایستا char_u	*p;
    صحیح			c;
    صحیح			n;
    char_u		buf[MB_MAXBYTES + 1];
    صحیح			i;

    اگر (init)
    {
	اگر (old_redo)
	    bp = old_redobuff.bh_first.b_next;
	وگرنه
	    bp = redobuff.bh_first.b_next;
	اگر (bp == پوچ)
	    بازگشت FAIL;
	p = bp->b_str;
	بازگشت OK;
    }
    اگر ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	اگر (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	وگرنه
	    n = 1;
	برای (i = 0; ; ++i)
	{
	    اگر (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = TO_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#ifdef FEAT_GUI
	    اگر (c == CSI)	// escaped CSI
		p += 2;
#endif
	    اگر (*++p == NUL && bp->b_next != پوچ)
	    {
		bp = bp->b_next;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    اگر (i == n - 1)	// last byte of a character
	    {
		اگر (n != 1)
		    c = (*mb_ptr2char)(buf);
		بشکن;
	    }
	    c = *p;
	    اگر (c == NUL)	// cannot happen?
		بشکن;
	}
    }

    بازگشت c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    ایستا تهی
copy_redo(صحیح old_redo)
{
    صحیح	    c;

    وقتی ((c = read_redo(FALSE, old_redo)) != NUL)
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
    صحیح
start_redo(long شمار, صحیح old_redo)
{
    صحیح	    c;

    // init the pointers; return if nothing to redo
    اگر (read_redo(TRUE, old_redo) == FAIL)
	بازگشت FAIL;

    c = read_redo(FALSE, old_redo);

#ifdef FEAT_EVAL
    اگر (c == K_SID)
    {
	// Copy the <SID>{sid}; sequence
	add_char_buff(&readbuf2, c);
	برای (;;)
	{
	    c = read_redo(FALSE, old_redo);
	    add_char_buff(&readbuf2, c);
	    اگر (!SAFE_isdigit(c))
		بشکن;
	}
	c = read_redo(FALSE, old_redo);
    }
#endif

    // copy the buffer name, if present
    اگر (c == '"')
    {
	add_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	// if a numbered buffer is used, increment the number
	اگر (c >= '1' && c < '9')
	    ++c;
	add_char_buff(&readbuf2, c);

	// the expression register should be re-evaluated
	اگر (c == '=')
	{
	    add_char_buff(&readbuf2, CAR);
	    cmd_silent = TRUE;
	}

	c = read_redo(FALSE, old_redo);
    }

    اگر (c == 'v')   // redo Visual
    {
	VIsual = curwin->w_cursor;
	VIsual_active = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    // try to enter the count (in place of a previous count)
    اگر (شمار)
    {
	وقتی (VIM_ISDIGIT(c))	// skip "old" count
	    c = read_redo(FALSE, old_redo);
	add_رقم_buff(&readbuf2, شمار);
    }

    // copy the rest from the redo buffer into the stuff buffer
    add_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    بازگشت OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    صحیح
start_redo_ins(تهی)
{
    صحیح	    c;

    اگر (read_redo(TRUE, FALSE) == FAIL)
	بازگشت FAIL;
    start_stuff();

    // skip the count and the command character
    وقتی ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	اگر (vim_strchr((char_u *)"AaIiRrOo", c) != پوچ)
	{
	    اگر (c == 'O' || c == 'o')
		add_buff(&readbuf2, NL_STR, -1L);
	    بشکن;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    block_redo = TRUE;
    بازگشت OK;
}

    تهی
stop_redo_ins(تهی)
{
    block_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    ایستا تهی
init_typebuf(تهی)
{
    اگر (typebuf.tb_buf != پوچ)
	بازگشت;

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
    صحیح
noremap_کلیدها(تهی)
{
    بازگشت کلید_Noremap & (RM_NONE|RM_SCRIPT);
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
    صحیح
ins_typebuf(
    char_u	*str,
    صحیح		noremap,
    صحیح		offset,
    صحیح		nottyped,
    صحیح		silent)
{
    char_u	*s1, *s2;
    صحیح		newlen;
    صحیح		addlen;
    صحیح		i;
    صحیح		newoff;
    صحیح		val;
    صحیح		nrm;

    init_typebuf();
    اگر (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
    state_no_longer_safe("ins_typebuf()");

    addlen = (صحیح)STRLEN(str);

    اگر (offset == 0 && addlen <= typebuf.tb_off)
    {
	/*
	 * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
	 */
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    وگرنه اگر (typebuf.tb_len == 0 && typebuf.tb_buflen
					       >= addlen + 3 * (MAXMAPLEN + 4))
    {
	/*
	 * Buffer is empty and string fits in the existing buffer.
	 * Leave some space before and after, if possible.
	 */
	typebuf.tb_off = (typebuf.tb_buflen - addlen - 3 * (MAXMAPLEN + 4)) / 2;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    وگرنه
    {
	صحیح extra;

	/*
	 * Need to allocate a new buffer.
	 * In typebuf.tb_buf there must always be room for 3 * (MAXMAPLEN + 4)
	 * characters.  We add some extra room to avoid having to allocate too
	 * often.
	 */
	newoff = MAXMAPLEN + 4;
	extra = addlen + newoff + 4 * (MAXMAPLEN + 4);
	اگر (typebuf.tb_len > 2147483647 - extra)
	{
	    // string is getting too long for a 32 bit int
	    emsg(_(e_command_too_complex));    // also calls flush_buffers
	    setcursor();
	    بازگشت FAIL;
	}
	newlen = typebuf.tb_len + extra;
	s1 = alloc(newlen);
	اگر (s1 == پوچ)		    // out of memory
	    بازگشت FAIL;
	s2 = alloc(newlen);
	اگر (s2 == پوچ)		    // out of memory
	{
	    vim_free(s1);
	    بازگشت FAIL;
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
	اگر (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)offset);
	mch_memmove(s2 + newoff + offset + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + offset,
					   (size_t)(typebuf.tb_len - offset));
	اگر (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    // If noremap == REMAP_SCRIPT: do remap script-local mappings.
    اگر (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    وگرنه اگر (noremap == REMAP_SKIP)
	val = RM_ABBR;
    وگرنه
	val = RM_NONE;

    /*
     * Adjust typebuf.tb_noremap[] for the new characters:
     * If noremap == REMAP_NONE or REMAP_SCRIPT: new characters are
     *			(sometimes) not remappable
     * If noremap == REMAP_YES: all the new characters are mappable
     * If noremap  > 0: "noremap" characters are not remappable, the rest
     *			mappable
     */
    اگر (noremap == REMAP_SKIP)
	nrm = 1;
    وگرنه اگر (noremap < 0)
	nrm = addlen;
    وگرنه
	nrm = noremap;
    برای (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + offset] =
						  (--nrm >= 0) ? val : RM_YES;

    // tb_maplen and tb_silent only remember the length of mapped and/or
    // silent mappings at the start of the buffer, assuming that a mapped
    // sequence doesn't result in typed characters.
    اگر (nottyped || typebuf.tb_maplen > offset)
	typebuf.tb_maplen += addlen;
    اگر (silent || typebuf.tb_silent > offset)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    اگر (typebuf.tb_no_abbr_cnt && offset == 0)	// and not used for abbrev.s
	typebuf.tb_no_abbr_cnt += addlen;

    بازگشت OK;
}

/*
 * Put character "c" back into the typeahead buffer.
 * Can be used for a character obtained by vgetc() that needs to be put back.
 * Uses cmd_silent, KeyTyped and KeyNoremap to restore the flags belonging to
 * the char.
 * Returns the length of what was inserted.
 */
    صحیح
ins_char_typebuf(صحیح c, صحیح modifiers)
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    صحیح		len = special_to_buf(c, modifiers, TRUE, buf);

    buf[len] = NUL;
    (تهی)ins_typebuf(buf, کلید_Noremap, 0, !کلید_Typed, cmd_silent);
    بازگشت len;
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
    صحیح
typebuf_changed(
    صحیح		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    بازگشت (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#endif
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    صحیح
typebuf_typed(تهی)
{
    بازگشت typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    صحیح
typebuf_maplen(تهی)
{
    بازگشت typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    تهی
del_typebuf(صحیح len, صحیح offset)
{
    صحیح	    i;

    اگر (len == 0)
	بازگشت;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    اگر (offset == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    وگرنه
    {
	i = typebuf.tb_off + offset;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	اگر (typebuf.tb_off > MAXMAPLEN)
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

    اگر (typebuf.tb_maplen > offset)		// adjust tb_maplen
    {
	اگر (typebuf.tb_maplen < offset + len)
	    typebuf.tb_maplen = offset;
	وگرنه
	    typebuf.tb_maplen -= len;
    }
    اگر (typebuf.tb_silent > offset)		// adjust tb_silent
    {
	اگر (typebuf.tb_silent < offset + len)
	    typebuf.tb_silent = offset;
	وگرنه
	    typebuf.tb_silent -= len;
    }
    اگر (typebuf.tb_no_abbr_cnt > offset)	// adjust tb_no_abbr_cnt
    {
	اگر (typebuf.tb_no_abbr_cnt < offset + len)
	    typebuf.tb_no_abbr_cnt = offset;
	وگرنه
	    typebuf.tb_no_abbr_cnt -= len;
    }

#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#endif
    اگر (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * State for adding bytes to a recording or 'showcmd'.
 */
تعریفنوع ساختار
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    صحیح		prev_c;
    size_t	buflen;
    unsigned	pending_special;
    unsigned	pending_mbyte;
} gotchars_state_T;

/*
 * Add a single byte to a recording or 'showcmd'.
 * Return TRUE if a full key has been received, FALSE otherwise.
 */
    ایستا صحیح
gotchars_add_byte(gotchars_state_T *state, char_u byte)
{
    صحیح		c = state->buf[state->buflen++] = byte;
    صحیح		retval = FALSE;
    صحیح		in_special = state->pending_special > 0;
    صحیح		in_mbyte = state->pending_mbyte > 0;

    اگر (in_special)
	state->pending_special--;
    وگرنه اگر (c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	    )
	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	state->pending_special = 2;

    اگر (state->pending_special > 0)
	برو ret_false;

    اگر (in_mbyte)
	state->pending_mbyte--;
    وگرنه
    {
	اگر (in_special)
	{
	    اگر (state->prev_c == KS_MODIFIER)
		// When receiving a modifier, wait for the modified key.
		برو ret_false;
	    c = TO_SPECIAL(state->prev_c, c);
	    // Drop K_FOCUSGAINED, K_FOCUSLOST and K_COMPLETE_DELAY, they are
	    // not useful in a recording.
	    اگر (c == K_FOCUSGAINED || c == K_FOCUSLOST || c == K_COMPLETE_DELAY)
		state->buflen = 0;
	}
	// When receiving a multibyte character, store it until we have all
	// the bytes, so that it won't be split between two buffer blocks,
	// and delete_buff_tail() will work properly.
	state->pending_mbyte = MB_BYTE2LEN_CHECK(c) - 1;
    }

    اگر (state->pending_mbyte > 0)
	برو ret_false;

    retval = TRUE;
ret_false:
    state->prev_c = c;
    بازگشت retval;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the record buffer.
 */
    ایستا تهی
gotchars(char_u *chars, صحیح len)
{
    char_u		*s = chars;
    size_t		i;
    صحیح			todo = len;
    ایستا gotchars_state_T state;

    وقتی (todo-- > 0)
    {
	اگر (!gotchars_add_byte(&state, *s++))
	    ادامه;

	// Handle one byte at a time; no translation to be done.
	برای (i = 0; i < state.buflen; ++i)
	    updatescript(state.buf[i]);

	اگر (reg_recording != 0)
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
    debug_did_پیام = FALSE;
#endif

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}

/*
 * Record an <Ignore> key.
 */
    تهی
gotchars_ignore(تهی)
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
    تهی
ungetchars(صحیح len)
{
    اگر (reg_recording == 0)
	بازگشت;

    delete_buff_tail(&recordbuff, len);
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
    ایستا تهی
may_sync_undo(تهی)
{
    اگر ((!(State & (MODE_درج | MODE_CMDLINE)) || arrow_used)
					       && scriptin[curscript] == پوچ)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    ایستا صحیح
alloc_typebuf(تهی)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    اگر (typebuf.tb_buf == پوچ || typebuf.tb_noremap == پوچ)
    {
	free_typebuf();
	بازگشت FAIL;
    }
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_off = MAXMAPLEN + 4;  // can insert without realloc
    typebuf.tb_len = 0;
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    typebuf.tb_no_abbr_cnt = 0;
    اگر (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    typebuf_was_filled = FALSE;
#endif
    بازگشت OK;
}

/*
 * Free the buffers of "typebuf".
 */
    ایستا تهی
free_typebuf(تهی)
{
    اگر (typebuf.tb_buf == typebuf_init)
	internal_error("Free typebuf 1");
    وگرنه
	VIM_CLEAR(typebuf.tb_buf);
    اگر (typebuf.tb_noremap == noremapbuf_init)
	internal_error("Free typebuf 2");
    وگرنه
	VIM_CLEAR(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
ایستا typebuf_T saved_typebuf[NSCRIPT];

    صحیح
save_typebuf(تهی)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    اگر (alloc_typebuf() == FAIL)
    {
	closescript();
	بازگشت FAIL;
    }
    بازگشت OK;
}

ایستا صحیح old_char = -1;	// character put back by vungetc()
ایستا صحیح old_mod_mask;	// mod_mask for ungotten character
ایستا صحیح old_mouse_row;	// mouse_row related to old_char
ایستا صحیح old_mouse_col;	// mouse_col related to old_char
ایستا صحیح old_کلید_Stuffed;	// whether old_char was stuffed

ایستا صحیح can_get_old_char(تهی)
{
    // If the old character was not stuffed and characters have been added to
    // the stuff buffer, need to first get the stuffed characters instead.
    بازگشت old_char != -1 && (old_کلید_Stuffed || stuff_empty());
}

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    تهی
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    اگر (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_mask = old_mod_mask;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_first.b_next = پوچ;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_first.b_next = پوچ;
#ifdef USE_درون_داد_BUF
    tp->save_inputbuf = get_درون_داد_buf();
#endif
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    تهی
restore_typeahead(tasave_T *tp, صحیح overwrite UNUSED)
{
    اگر (tp->typebuf_valid)
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
#ifdef USE_درون_داد_BUF
    set_درون_داد_buf(tp->save_inputbuf, overwrite);
#endif
}

/*
 * Open a new script file for the ":source!" command.
 */
    تهی
openscript(
    char_u	*name,
    صحیح		directly)	// when TRUE execute directly
{
    اگر (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	بازگشت;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    اگر (check_secure())
	بازگشت;

#ifdef FEAT_EVAL
    اگر (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	بازگشت;
#endif

    اگر (scriptin[curscript] != پوچ)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    اگر ((scriptin[curscript] = mch_fopen((نویسه *)NameBuff, READBIN)) == پوچ)
    {
	semsg(_(e_cant_open_file_str), name);
	اگر (curscript)
	    --curscript;
	بازگشت;
    }
    اگر (save_typebuf() == FAIL)
	بازگشت;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    اگر (directly)
    {
	oparg_T	oa;
	صحیح	oldcurscript;
	صحیح	save_State = State;
	صحیح	save_restart_edit = restart_edit;
	صحیح	save_insertmode = p_im;
	صحیح	save_finish_op = finish_op;
	صحیح	save_پیام_scroll = پیام_scroll;

	State = MODE_NORMAL;
	پیام_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	finish_op = FALSE;

	oldcurscript = curscript;
	انجامبده
	{
	    update_topline_cursor();	// update cursor position and topline
	    normal_cmd(&oa, FALSE);	// execute one command
	    (تهی)vpeekc();		// check for end of file
	}
	وقتی (scriptin[oldcurscript] != پوچ);

	State = save_State;
	پیام_scroll = save_پیام_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	finish_op = save_finish_op;
    }
}

/*
 * Close the currently active input script.
 */
    ایستا تهی
closescript(تهی)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = پوچ;
    اگر (curscript > 0)
	--curscript;
}

#اگر defined(EXITFREE)
    تهی
close_all_scripts(تهی)
{
    وقتی (scriptin[0] != پوچ)
	closescript();
}
#endif

/*
 * Return TRUE when reading keys from a script file.
 */
    صحیح
using_script(تهی)
{
    بازگشت scriptin[curscript] != پوچ;
}

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    تهی
before_blocking(تهی)
{
    // Tell terminal to flush screen contents before blocking
    term_set_sync_output(TERM_SYNC_OUTPUT_FLUSH);

    updatescript(0);
#ifdef FEAT_EVAL
    اگر (may_garbage_collect)
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
    ایستا تهی
updatescript(صحیح c)
{
    ایستا صحیح	    شمار = 0;

    اگر (c && scriptout)
	putc(c, scriptout);
    اگر (c == 0 || (p_uc > 0 && ++شمار >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	شمار = 0;
    }
#ifdef FEAT_EVAL
    اگر (typedchars_pos < MAXMAPLEN)
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
    صحیح
merge_modify_Other_کلیدها(صحیح c_arg, صحیح *modifiers)
{
    صحیح c = c_arg;

    // CTRL only uses the lower 5 bits of the character.
    اگر (*modifiers & MOD_MASK_CTRL)
    {
	اگر ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	{
	    c &= 0x1f;
	    اگر (c == NUL)
		c = K_ZERO;
	}
	وگرنه اگر (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#ifdef FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	وگرنه اگر (c == '2')
	    c = NUL;
	وگرنه اگر (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	وگرنه اگر (c == '8')
	    c = BS;
	وگرنه اگر (c == '?')
	    c = DEL;
#endif
	اگر (c != c_arg)
	    *modifiers &= ~MOD_MASK_CTRL;
    }

    // Alt/Meta sets the 8th bit of the character.
    اگر ((*modifiers & (MOD_MASK_META | MOD_MASK_ALT))
	    && c >= 0 && c <= 127)
    {
	// Some terminals (esp. Kitty) do not include Shift in the character.
	// Apply it here to get consistency across terminals.  Only do ASCII
	// letters, for other characters it depends on the keyboard layout.
	اگر ((*modifiers & MOD_MASK_SHIFT) && c >= 'a' && c <= 'z')
	{
	    c += 'a' - 'A';
	    *modifiers &= ~MOD_MASK_SHIFT;
	}
	c += 0x80;
	*modifiers &= ~(MOD_MASK_META | MOD_MASK_ALT);
    }

    بازگشت c;
}

/*
 * Add a single byte to 'showcmd' for a partially matched mapping.
 * Call add_to_showcmd() if a full key has been received.
 */
    ایستا تهی
add_byte_to_showcmd(char_u byte)
{
    ایستا gotchars_state_T state;
    char_u	*ptr;
    صحیح		modifiers = 0;
    صحیح		c = NUL;

    اگر (!p_sc || پیام_silent != 0)
	بازگشت;

    اگر (!gotchars_add_byte(&state, byte))
	بازگشت;

    state.buf[state.buflen] = NUL;
    state.buflen = 0;

    ptr = state.buf;
    اگر (ptr[0] == K_SPECIAL && ptr[1] == KS_MODIFIER && ptr[2] != NUL)
    {
	modifiers = ptr[2];
	ptr += 3;
    }

    اگر (*ptr != NUL)
    {
	char_u		*mb_ptr = mb_unescape(&ptr);

	c = mb_ptr != پوچ ? (*mb_ptr2char)(mb_ptr) : *ptr++;
	اگر (c <= 0x7f)
	{
	    // Merge modifiers into the key to make the result more readable.
	    صحیح		modifiers_after = modifiers;
	    صحیح		mod_c = merge_modify_Other_کلیدها(c, &modifiers_after);

	    اگر (modifiers_after == 0)
	    {
		modifiers = 0;
		c = mod_c;
	    }
	}
    }

    // TODO: is there a more readable and yet compact representation of
    // modifiers and special keys?
    اگر (modifiers != 0)
    {
	add_to_showcmd(K_SPECIAL);
	add_to_showcmd(KS_MODIFIER);
	add_to_showcmd(modifiers);
    }
    اگر (c != NUL)
	add_to_showcmd(c);
    وقتی (*ptr != NUL)
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
    صحیح
vgetc(تهی)
{
    صحیح		c, c2;
    صحیح		n;
    char_u	buf[MB_MAXBYTES + 1];
    صحیح		i;

#ifdef FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    اگر (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#endif

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    اگر (can_get_old_char())
    {
	c = old_char;
	old_char = -1;
	mod_mask = old_mod_mask;
	mouse_row = old_mouse_row;
	mouse_col = old_mouse_col;
    }
    وگرنه
    {
	// number of characters recorded from the last vgetc() call
	ایستا size_t	last_vgetc_recorded_len = 0;

	mod_mask = 0;
	vgetc_mod_mask = 0;
	vgetc_char = 0;

	// last_recorded_len can be larger than last_vgetc_recorded_len
	// if peeking records more
	last_recorded_len -= last_vgetc_recorded_len;

	برای (;;)		// this is done twice if there are modifiers
	{
	    صحیح did_inc = FALSE;

	    // No mapping after modifier has been read, using an input method
	    // and when a popup window has disabled mapping.
	    اگر (mod_mask
#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#endif
#اگر defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#endif
		    )
	    {
		++no_mapping;
		++allow_کلیدها;
		// mod_mask value may change, remember we did the increment
		did_inc = TRUE;
	    }
	    c = vgetorpeek(TRUE);
	    اگر (did_inc)
	    {
		--no_mapping;
		--allow_کلیدها;
	    }

	    // Get two extra bytes for special keys, handle modifiers.
	    اگر (c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	       )
	    {
		صحیح	    save_allow_کلیدها = allow_کلیدها;

		++no_mapping;
		allow_کلیدها = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_کلیدها = save_allow_کلیدها;
		اگر (c2 == KS_MODIFIER)
		{
		    mod_mask = c;
		    ادامه;
		}
		c = TO_SPECIAL(c2, c);

		اگر (allow_osc_کلید == 0 && c == K_OSC)
		    ادامه;

		// K_ESC is used to avoid ambiguity with the single Esc
		// character that might be the start of an escape sequence.
		// Convert it back to a single Esc here.
		اگر (c == K_ESC)
		    c = ESC;

#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		// Handle K_TEAROFF here, the caller of vgetc() doesn't need to
		// know that a menu was torn off
		اگر (
# ifdef VIMDLL
		    gui.in_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    صحیح		j;

		    // get menu path, it ends with a <CR>
		    برای (j = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[j] = c;
			اگر (j < 199)
			    ++j;
		    }
		    name[j] = NUL;
		    gui_make_tearoff(name);
		    ادامه;
		}
#endif
#اگر defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && !defined(USE_GTK4) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		اگر (c == K_F10 && gui.menubar != پوچ)
		{
		    gtk_menu_shell_select_first(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    ادامه;
		}
#endif
#ifdef FEAT_GUI
		// Handle focus event here, so that the caller doesn't need to
		// know about it.  Return K_IGNORE so that we loop once (needed
		// if 'lazyredraw' is set).
		اگر (c == K_FOCUSGAINED || c == K_FOCUSLOST)
		{
		    ui_focus_change(c == K_FOCUSGAINED);
		    c = K_IGNORE;
		}

		// Translate K_CSI to CSI.  The special key is only used to
		// avoid it being recognized as the start of a special key.
		اگر (c == K_CSI)
		    c = CSI;
#endif
#ifdef FEAT_EVAL
		اگر (c == K_SID)
		{
		    صحیح	    j;

		    // Handle <SID>{sid};  Do up to 20 digits for safety.
		    last_used_sid = 0;
		    برای (j = 0; j < 20 && SAFE_isdigit(c = vgetorpeek(TRUE)); ++j)
			last_used_sid = last_used_sid * 10 + (c - '0');
		    last_used_map = پوچ;
		    ادامه;
		}
#endif
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    اگر (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		برای (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    اگر (buf[i] == K_SPECIAL
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
			اگر (vgetorpeek(TRUE) == KE_CSI && c == KS_EXTRA)
			    buf[i] = CSI;
		    }
		}
		--no_mapping;
		c = (*mb_ptr2char)(buf);
	    }

	    اگر (vgetc_char == 0)
	    {
		vgetc_mod_mask = mod_mask;
		vgetc_char = c;
	    }

	    // A keypad or special function key was not mapped, use it like
	    // its ASCII equivalent.
	    انتخاب (c)
	    {
		حالت K_KPLUS:	c = '+'; بشکن;
		حالت K_KMINUS:	c = '-'; بشکن;
		حالت K_KDIVIDE:	c = '/'; بشکن;
		حالت K_KMULTIPLY: c = '*'; بشکن;
		حالت K_KENTER:	c = CAR; بشکن;
		حالت K_KPOINT:
#ifdef MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = Map_Virtual_کلید(VK_DECIMAL, 2); بشکن;
#وگرنه
				c = '.'; بشکن;
#endif
		حالت K_K0:	c = '0'; بشکن;
		حالت K_K1:	c = '1'; بشکن;
		حالت K_K2:	c = '2'; بشکن;
		حالت K_K3:	c = '3'; بشکن;
		حالت K_K4:	c = '4'; بشکن;
		حالت K_K5:	c = '5'; بشکن;
		حالت K_K6:	c = '6'; بشکن;
		حالت K_K7:	c = '7'; بشکن;
		حالت K_K8:	c = '8'; بشکن;
		حالت K_K9:	c = '9'; بشکن;

		حالت K_XHOME:
		حالت K_ZHOME:	اگر (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_HOME;
				    mod_mask = 0;
				}
				وگرنه اگر (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_HOME;
				    mod_mask = 0;
				}
				وگرنه
				    c = K_HOME;
				بشکن;
		حالت K_XEND:
		حالت K_ZEND:	اگر (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_END;
				    mod_mask = 0;
				}
				وگرنه اگر (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_END;
				    mod_mask = 0;
				}
				وگرنه
				    c = K_END;
				بشکن;

		حالت K_XUP:	c = K_UP; بشکن;
		حالت K_XDOWN:	c = K_DOWN; بشکن;
		حالت K_XLEFT:	c = K_چپ; بشکن;
		حالت K_XRIGHT:	c = K_راست; بشکن;
	    }

	    بشکن;
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
    اگر (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD
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
    اگر ((!ex_normal_busy || in_feedkeys) && popup_do_filter(c))
    {
	اگر (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#endif

#ifdef FEAT_EVAL
    c = do_کلید_درون_داد_pre(c);

    // Clear the next typedchars_pos
    typedchars_pos = 0;
#endif

    // Need to process the character before we know it's safe to do something
    // else.
    اگر (c != K_IGNORE)
	state_no_longer_safe("key typed");

    بازگشت c;
}

#ifdef FEAT_EVAL
/*
 * Handle the KeyInputPre autocommand.
 * "c" is the character that was typed.
 * Return new input character.
 */
    ایستا صحیح
do_کلید_درون_داد_pre(صحیح c)
{
    صحیح		res = c;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    char_u	curr_mode[MODE_MAX_LENGTH];
    صحیح		save_State = State;
    save_v_event_T save_v_event;
    dict_T	*v_event;

    // Return quickly when there is nothing to do.
    اگر (!has_keyinputpre())
	بازگشت res;

    اگر (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = KEY2TERMCAP0(c);
	buf[2] = KEY2TERMCAP1(c);
	buflen = 3;
    }
    وگرنه
	buflen = (*mb_char2bytes)(c, buf);
    buf[buflen] = NUL;

    typedchars[typedchars_pos] = NUL;
    vim_unescape_csi(typedchars);

    get_mode(curr_mode);

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (صحیح)buflen);  // set v:char

    v_event = get_v_event(&save_v_event);
    (تهی)dict_add_bool(v_event, "typed", کلید_Typed);
    (تهی)dict_add_string_len(v_event, "typedchar", typedchars, typedchars_pos);

    اگر (apply_autocmds(EVENT_KEYINPUTPRE, curr_mode, curr_mode, FALSE, curbuf))
    {
	string_T    v_char;

	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character.
	v_char.string = get_vim_var_str(VV_CHAR);
	اگر (STRCMP(buf, v_char.string) != 0)
	{
	    v_char.length = STRLEN(v_char.string);
	    // Convert special bytes when it is special string.
	    اگر (v_char.length >= 3 && v_char.string[0] == K_SPECIAL)
		res = TERMCAP2_کلید(v_char.string[1], v_char.string[2]);
	    وگرنه اگر (v_char.length > 0)
		res = PTR2CHAR(v_char.string);
	}
    }

    restore_v_event(v_event, &save_v_event);

    set_vim_var_string(VV_CHAR, پوچ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    بازگشت res;
}
#endif

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    صحیح
safe_vgetc(تهی)
{
    صحیح	c;

    c = vgetc();
    اگر (c == NUL)
	c = get_keystroke();
    بازگشت c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 * Does not handle bracketed paste - do not use the result for commands.
 */
    ایستا صحیح
plain_vgetc_nopaste(تهی)
{
    صحیح c;

    انجامبده
	c = safe_vgetc();
    وقتی (c == K_IGNORE
	    || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
	    || c == K_MOUSEMOVE);
    بازگشت c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    صحیح
plain_vgetc(تهی)
{
    صحیح c = plain_vgetc_nopaste();

    اگر (c == K_PS)
	// Only handle the first pasted character.  Drop the rest, since we
	// don't know what to do with it.
	c = bracketed_paste(PASTE_ONE_CHAR, FALSE, پوچ);

    بازگشت c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 * Returns NUL if no character is available.
 */
    صحیح
vpeekc(تهی)
{
    اگر (can_get_old_char())
	بازگشت old_char;
    بازگشت vgetorpeek(FALSE);
}

#اگر defined(FEAT_TERMRESPONSE) || defined(FEAT_TERMINAL)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    صحیح
vpeekc_nomap(تهی)
{
    صحیح		c;

    ++no_mapping;
    ++allow_کلیدها;
    c = vpeekc();
    --no_mapping;
    --allow_کلیدها;
    بازگشت c;
}
#endif

/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    صحیح
vpeekc_any(تهی)
{
    صحیح		c;

    c = vpeekc();
    اگر (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    بازگشت c;
}

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    صحیح
char_avail(تهی)
{
    صحیح	    retval;

#ifdef FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    اگر (disable_char_avail_for_testing)
	بازگشت FALSE;
#endif
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    بازگشت (retval != NUL);
}

#اگر defined(FEAT_EVAL)
/*
 * "getchar()" and "getcharstr()" functions
 */
    ایستا تهی
getchar_common(typval_T *argvars, typval_T *rettv, صحیح allow_عدد)
{
    varnumber_T		n = 0;
    صحیح			called_emsg_start = called_emsg;
    صحیح			error = FALSE;
    صحیح			simplify = TRUE;
    char_u		cursor_flag = 'm';

    اگر ((in_vim9script()
		&& check_for_opt_bool_or_عدد_arg(argvars, 0) == FAIL)
	    || (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type == VAR_DICT)
    {
	dict_T		*d = argvars[1].vval.v_dict;
	char_u		*cursor_str;

	اگر (allow_عدد)
	    allow_عدد = dict_get_bool(d, "number", TRUE);
	وگرنه اگر (dict_has_کلید(d, "number"))
	    semsg(_(e_invalid_argument_str), "number");

	simplify = dict_get_bool(d, "simplify", TRUE);

	cursor_str = dict_get_string(d, "cursor", FALSE);
	اگر (cursor_str != پوچ)
	{
	    اگر (STRCMP(cursor_str, "hide") != 0
		    && STRCMP(cursor_str, "keep") != 0
		    && STRCMP(cursor_str, "msg") != 0)
		semsg(_(e_invalid_مقدار_for_argument_str_str), "cursor",
								   cursor_str);
	    وگرنه
		cursor_flag = cursor_str[0];
	}
    }

    اگر (called_emsg != called_emsg_start)
	بازگشت;

# ifdef MESSAGE_QUEUE
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
# endif

    اگر (cursor_flag == 'h')
	cursor_sleep();
    وگرنه اگر (cursor_flag == 'm')
	windgoto(پیام_row, cmdline_col_off + پیام_col);

    ++no_mapping;
    ++allow_کلیدها;
    ++allow_osc_کلید;
    اگر (!simplify)
	++no_reduce_کلیدها;
    برای (;;)
    {
	اگر (argvars[0].v_type == VAR_UNKNOWN
		|| (argvars[0].v_type == VAR_عدد
			&& argvars[0].vval.v_عدد == -1))
	    // getchar(): blocking wait.
	    n = plain_vgetc_nopaste();
	وگرنه اگر (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	وگرنه اگر (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	وگرنه
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = safe_vgetc();

	اگر (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    ادامه;
	بشکن;
    }
    --no_mapping;
    --allow_کلیدها;
    --allow_osc_کلید;
    اگر (!simplify)
	--no_reduce_کلیدها;

    اگر (cursor_flag == 'h')
	cursor_unsleep();

    set_vim_var_nr(VV_MOUSE_WIN, 0);
    set_vim_var_nr(VV_MOUSE_WINID, 0);
    set_vim_var_nr(VV_MOUSE_LNUM, 0);
    set_vim_var_nr(VV_MOUSE_COL, 0);

    اگر (n != 0 && (!allow_عدد || IS_SPECIAL(n) || mod_mask != 0))
    {
	char_u		temp[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	صحیح		i = 0;

	// Turn a special key into three bytes, plus modifier.
	اگر (mod_mask != 0)
	{
	    temp[i++] = K_SPECIAL;
	    temp[i++] = KS_MODIFIER;
	    temp[i++] = mod_mask;
	}
	اگر (IS_SPECIAL(n))
	{
	    temp[i++] = K_SPECIAL;
	    temp[i++] = K_SECOND(n);
	    temp[i++] = K_THIRD(n);
	}
	وگرنه اگر (has_mbyte)
	    i += (*mb_char2bytes)(n, temp + i);
	وگرنه
	    temp[i++] = n;
	temp[i] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strnsave(temp, i);

	اگر (is_mouse_کلید(n))
	{
	    صحیح		row = mouse_row;
	    صحیح		col = mouse_col;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    صحیح		winnr = 1;

	    اگر (row >= 0 && col >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_find_win(&row, &col, FIND_POPUP);
		اگر (win == پوچ)
		    بازگشت;
		(تهی)mouse_comp_pos(win, &row, &col, &lnum, پوچ);
# ifdef FEAT_PROP_POPUP
		اگر (WIN_IS_POPUP(win))
		    winnr = 0;
		وگرنه
# endif
		    برای (wp = firstwin; wp != win && wp != پوچ;
							       wp = wp->w_next)
			++winnr;
		set_vim_var_nr(VV_MOUSE_WIN, winnr);
		set_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		set_vim_var_nr(VV_MOUSE_LNUM, lnum);
		set_vim_var_nr(VV_MOUSE_COL, col + 1);
	    }
	}
    }
    وگرنه اگر (!allow_عدد)
	rettv->v_type = VAR_STRING;
    وگرنه
	rettv->vval.v_عدد = n;
}

/*
 * "getchar()" function
 */
    تهی
f_getchar(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, TRUE);
}

/*
 * "getcharstr()" function
 */
    تهی
f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, FALSE);
}

/*
 * "getcharmod()" function
 */
    تهی
f_getcharmod(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = mod_mask;
}
#endif // FEAT_EVAL

#اگر defined(MESSAGE_QUEUE)
# define MAX_REPEAT_PARSE 8

/*
 * Process messages that have been queued for netbeans or clientserver.
 * Also check if any jobs have ended.
 * These functions can call arbitrary Vim script and should only be called when
 * it is safe to do so.
 */
    تهی
parse_queued_messages(تهی)
{
    صحیح	    old_curwin_id;
    صحیح	    old_curbuf_fnum;
    صحیح	    i;
    صحیح	    save_may_garbage_collect = may_garbage_collect;
    ایستا صحیح entered = 0;
    صحیح	    was_safe = get_was_safe_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    اگر (updating_screen || dont_parse_messages)
	بازگشت;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    اگر (curbuf == پوچ || curwin == پوچ)
	بازگشت;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    برای (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# اگر (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_JOB_CHANNEL)
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
	channel_write_any_سطرها();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# اگر defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_JOB_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	اگر (job_check_ended())
	    ادامه;
# endif
# ifdef FEAT_TERMINAL
	free_unused_terminals();
# endif

# ifdef FEAT_SOUND_MACOSX
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	اگر (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef SIGUSR1
	اگر (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, پوچ, پوچ, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
# endif
	بشکن;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    اگر (entered == 1 && was_safe)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    اگر (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}
#endif


تعریفنوع شمارش {
    map_result_fail,    // failed, break loop
    map_result_get,     // get a character from typeahead
    map_result_retry,   // try to map again
    map_result_nomatch  // no matching mapping, get char
} map_result_T;

/*
 * Check if the bytes at the start of the typeahead buffer are a character used
 * in Insert mode completion.  This includes the form with a CTRL modifier.
 * During completion started by complete() keys are mapped as usual.
 */
    ایستا صحیح
at_ins_compl_کلید(تهی)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    صحیح	    c = *p;

    اگر (typebuf.tb_len > 3
	    && (c == K_SPECIAL || c == CSI)  // CSI is used by the GUI
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_MASK_CTRL)
	    // CTRL-SHIFT-N/P scroll the info popup, so they must not be folded
	    // to the CTRL-N/CTRL-P completion keys here.
	    && !(p[2] & MOD_MASK_SHIFT))
	c = p[3] & 0x1f;
    بازگشت !ctrl_x_mode_eval()
	    && ((ctrl_x_mode_not_default() && vim_is_ctrl_x_کلید(c))
		|| (compl_status_local() && (c == Ctrl_N || c == Ctrl_P)));
}

/*
 * Check if typebuf.tb_buf[] contains a modifier plus key that can be changed
 * into just a key, apply that.
 * Check from typebuf.tb_buf[typebuf.tb_off] to typebuf.tb_buf[typebuf.tb_off
 * + "max_offset"].
 * Return the length of the replaced bytes, 0 if nothing changed, -1 for error.
 */
    ایستا صحیح
check_simplify_modifier(صحیح max_offset)
{
    صحیح		offset;
    char_u	*tp;

    برای (offset = 0; offset < max_offset; ++offset)
    {
	اگر (offset + 3 >= typebuf.tb_len)
	    بشکن;
	tp = typebuf.tb_buf + typebuf.tb_off + offset;
	اگر ((tp[0] == K_SPECIAL || tp[0] == CSI) && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    صحیح modifier = tp[2];
	    صحیح	c = tp[3];
	    صحیح new_c = merge_modify_Other_کلیدها(c, &modifier);

	    اگر (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		صحیح	len;
		صحیح	کلید_offset = offset;

		اگر (offset == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_mask = tp[2];
		}
		اگر (IS_SPECIAL(new_c))
		{
		    new_string[0] = K_SPECIAL;
		    new_string[1] = K_SECOND(new_c);
		    new_string[2] = K_THIRD(new_c);
		    len = 3;
		}
		وگرنه
		    len = mb_char2bytes(new_c, new_string);
		اگر (modifier == 0)
		{
		    اگر (put_string_in_typebuf(offset, 4, new_string, len,
							پوچ, 0, پوچ) == FAIL)
			بازگشت -1;
		}
		وگرنه
		{
		    tp[2] = modifier;
		    کلید_offset = offset + 3;
		    اگر (put_string_in_typebuf(کلید_offset, 1, new_string, len,
							پوچ, 0, پوچ) == FAIL)
			بازگشت -1;
		}

		// A mapping for the simplified key can be used for it even
		// when the key protocol is enabled.
		برای (صحیح i = 0; i < len; ++i)
		    typebuf.tb_noremap[typebuf.tb_off + کلید_offset + i]
							    |= RM_SIMPLIFIED;
		بازگشت len;
	    }
	}
    }
    بازگشت 0;
}

/*
 * Return TRUE if the terminal sends modifiers with various keys.  This is when
 * modifyOtherKeys level 2 is enabled or the kitty keyboard protocol is
 * enabled.
 */
    صحیح
کلید_protocol_enabled(تهی)
{
    // If xterm has responded to XTQMODKEYS it overrules seenModifyOtherKeys.
    صحیح using_mok = modify_otherkeys_state != MOKS_INITIAL
			? modify_otherkeys_state == MOKS_ENABLED
			: seen_Modify_Other_کلیدها;
    بازگشت using_mok || kitty_protocol_state == KKPS_ENABLED;
}

/*
 * Handle mappings in the typeahead buffer.
 * - When something was mapped, return map_result_retry for recursive mappings.
 * - When nothing mapped and typeahead has a character: return map_result_get.
 * - When there is no match yet, return map_result_nomatch, need to get more
 *   typeahead.
 * - On failure (out of memory) return map_result_fail.
 */
    ایستا صحیح
handle_mapping(
	    صحیح *keylenp,
	    صحیح *timedout,
	    صحیح *mapdepth)
{
    mapblock_T	*mp = پوچ;
    mapblock_T	*mp2;
    mapblock_T	*mp_match;
    صحیح		mp_match_len = 0;
    صحیح		max_mlen = 0;
    صحیح		want_termcode = 0;  // 1 if termcode expected after max_mlen
    صحیح		tb_c1;
    صحیح		mlen;
#ifdef FEAT_LANGMAP
    صحیح		nolmaplen;
#endif
    صحیح		keylen = *keylenp;
    صحیح		i;
    صحیح		local_State = get_real_state();
    صحیح		is_plug_map = FALSE;
    bool	in_osc = in_osc_sequence(); // If we are in an OSC sequence,
					    // then go straight to
					    // check_termcode() in order to
					    // consume chars.

    // If typeahead starts with <Plug> then remap, even for a "noremap" mapping.
    اگر (!in_osc && typebuf.tb_len >= 3
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
    اگر (!in_osc && no_mapping == 0 && is_maphash_valid()
	    && (no_zero_mapping == 0 || tb_c1 != '0')
	    && (typebuf.tb_maplen == 0 || is_plug_map
		|| (p_remap
		    && (typebuf.tb_noremap[typebuf.tb_off]
				    & (RM_NONE|RM_ABBR)) == 0))
	    && !(p_paste && (State & (MODE_درج | MODE_CMDLINE)))
	    && !(State == MODE_HITRETURN && (tb_c1 == CAR || tb_c1 == ' '))
	    && State != MODE_ASKMORE
	    && State != MODE_CONFIRM
	    && !at_ins_compl_کلید())
    {
#ifdef FEAT_GUI
	اگر (gui.in_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#endif
#ifdef FEAT_LANGMAP
	اگر (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	وگرنه
	{
	    LANGMAP_ADJUST(tb_c1, (State & (MODE_CMDLINE | MODE_درج)) == 0
					   && get_real_state() != MODE_SELECT);
	    nolmaplen = 0;
	}
#endif
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	اگر (mp == پوچ)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = پوچ;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_match = پوچ;
	mp_match_len = 0;
	برای ( ; mp != پوچ;
	       mp->m_next == پوچ ? (mp = mp2, mp2 = پوچ) : (mp = mp->m_next))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    اگر (mp->m_کلیدها[0] == tb_c1
		    && (mp->m_mode & local_State)
		    && !(mp->m_simplified && کلید_protocol_enabled()
			    && typebuf.tb_maplen == 0
			    && (typebuf.tb_noremap[typebuf.tb_off]
							& RM_SIMPLIFIED) == 0)
		    && ((mp->m_mode & MODE_LANGMAP) == 0
						    || typebuf.tb_maplen == 0))
	    {
#ifdef FEAT_LANGMAP
		صحیح	nomap = nolmaplen;
		صحیح	modifiers = 0;
#endif
		// find the match length of this mapping
		برای (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
		    صحیح	c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
#ifdef FEAT_LANGMAP
		    اگر (nomap > 0)
		    {
			اگر (nomap == 2 && c2 == KS_MODIFIER)
			    modifiers = 1;
			وگرنه اگر (nomap == 1 && modifiers == 1)
			    modifiers = c2;
			--nomap;
		    }
		    وگرنه
		    {
			اگر (c2 == K_SPECIAL)
			    nomap = 2;
			وگرنه اگر (merge_modify_Other_کلیدها(c2, &modifiers) == c2)
			    // Only apply 'langmap' if merging modifiers into
			    // the key will not result in another character,
			    // so that 'langmap' behaves consistently in
			    // different terminals and GUIs.
			    LANGMAP_ADJUST(c2, TRUE);
			modifiers = 0;
		    }
#endif
		    اگر (mp->m_کلیدها[mlen] != c2)
			بشکن;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_کلیدها;
		    char_u *p2 = mb_unescape(&p1);

		    اگر (has_mbyte && p2 != پوچ
					&& MB_BYTE2LEN(tb_c1) > mb_ptr2len(p2))
			mlen = 0;
		}

		// Check an entry whether it matches.
		// - Full match: mlen == keylen
		// - Partly match: mlen == typebuf.tb_len
		keylen = mp->m_keylen;
		اگر (mlen == keylen || (mlen == typebuf.tb_len
						   && typebuf.tb_len < keylen))
		{
		    char_u  *s;
		    صحیح	    n;

		    // If only script-local mappings are allowed, check if the
		    // mapping starts with K_SNR.
		    s = typebuf.tb_noremap + typebuf.tb_off;
		    اگر ((*s & ~RM_SIMPLIFIED) == RM_SCRIPT
			    && (mp->m_کلیدها[0] != K_SPECIAL
				|| mp->m_کلیدها[1] != KS_EXTRA
				|| mp->m_کلیدها[2] != KE_SNR))
			ادامه;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    برای (n = mlen; --n >= 0; )
			اگر (*s++ & (RM_NONE|RM_ABBR))
			    بشکن;
		    اگر (!is_plug_map && n >= 0)
			ادامه;

		    اگر (keylen > typebuf.tb_len)
		    {
			اگر (!*timedout && !(mp_match != پوچ
							&& mp_match->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    بشکن;
			}
		    }
		    وگرنه اگر (keylen > mp_match_len)
		    {
			// found a longer match
			mp_match = mp;
			mp_match_len = keylen;
		    }
		}
		وگرنه
		{
		    // No match; may have to check for termcode at next
		    // character.

		    // If the first character that didn't match is
		    // K_SPECIAL then check for a termcode.  This isn't perfect
		    // but should work in most cases.
		    اگر (max_mlen < mlen)
		    {
			max_mlen = mlen;
			want_termcode = mp->m_کلیدها[mlen] == K_SPECIAL;
		    }
		    وگرنه اگر (max_mlen == mlen && mp->m_کلیدها[mlen] == K_SPECIAL)
			want_termcode = 1;

		    // Check termcode for uppercase character to properly
		    // process "ESC[27;2;<ascii code>~" control sequences.
		    اگر (ASCII_ISUPPER(mp->m_کلیدها[mlen]))
			want_termcode = 1;
		}
	    }
	}

	// If no partly match found, use the longest full match.
	اگر (keylen != KEYLEN_PART_MAP && mp_match != پوچ)
	{
	    mp = mp_match;
	    keylen = mp_match_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    اگر (!in_osc && *p_pt != NUL && mp == پوچ &&
	    (State & (MODE_درج | MODE_NORMAL)))
    {
	برای (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    اگر (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    بشکن;
	اگر (p_pt[mlen] == NUL)	// match
	{
	    // write chars to script file(s)
	    اگر (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    set_option_مقدار_give_err((char_u *)"paste",
						      (long)!p_paste, پوچ, 0);
	    اگر (!(State & MODE_درج))
	    {
		پیام_col = 0;
		پیام_row = Rows - 1;
		پیام_clr_eos();		// clear ruler
	    }
	    status_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    بازگشت map_result_retry;
	}
	// Need more chars for partly match.
	اگر (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_کلید;
	وگرنه اگر (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    // May check for a terminal code when there is no mapping or only a partial
    // mapping.  Also check if there is a full mapping with <Esc>, unless timed
    // out, since that is nearly always a partial match with a terminal code.
    اگر (in_osc || ((mp == پوچ || max_mlen + want_termcode > mp_match_len
		    || (mp_match_len == 1 && *mp->m_کلیدها == ESC && !*timedout))
	    && keylen != KEYLEN_PART_MAP))
    {
	صحیح	save_keylen = keylen;

	/*
	 * When no matching mapping found or found a non-matching mapping that
	 * matches at least what the matching mapping matched:
	 * Check if we have a terminal code, when:
	 * - mapping is allowed,
	 * - keys have not been mapped,
	 * - and not an ESC sequence, not in insert mode or p_ek is on,
	 * - and when not timed out,
	 */
	اگر (in_osc || no_mapping == 0 || allow_کلیدها != 0)
	{
	    اگر (in_osc || ((typebuf.tb_maplen == 0
			    || (p_remap && (typebuf.tb_noremap[typebuf.tb_off]
						& ~RM_SIMPLIFIED) == RM_YES))
		    && !*timedout))
		keylen = check_termcode(max_mlen + 1, پوچ, 0, پوچ);
	    وگرنه
		keylen = 0;

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    اگر (keylen == 0 && save_keylen == KEYLEN_PART_کلید && !*timedout)
		keylen = KEYLEN_PART_کلید;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
#ifdef FEAT_TERMINAL
	    check_no_reduce_کلیدها();  // may update the no_reduce_keys flag
#endif
	    اگر (keylen == 0 && !no_reduce_کلیدها)
	    {
		keylen = check_simplify_modifier(max_mlen + 1);
		اگر (keylen < 0)
		    // ins_typebuf() failed
		    بازگشت map_result_fail;
	    }

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    اگر (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	وگرنه
	    keylen = 0;
	اگر (keylen == 0)	// no matching terminal code
	{
#ifdef AMIGA
	    // check for window bounds report
	    اگر (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		برای (s = typebuf.tb_buf + typebuf.tb_off + 1;
			   s < typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len
		   && (VIM_ISDIGIT(*s) || *s == ';' || *s == ' ');
			++s)
		    ;
		اگر (*s == 'r' || *s == '|') // found one
		{
		    del_typebuf(
			  (صحیح)(s + 1 - (typebuf.tb_buf + typebuf.tb_off)), 0);
		    // get size and redraw screen
		    shell_resized();
		    *keylenp = keylen;
		    بازگشت map_result_retry;
		}
		اگر (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_کلید;
	    }
	    اگر (keylen >= 0)
#endif
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		اگر (mp == پوچ)
		{
		    *keylenp = keylen;
		    بازگشت map_result_get;    // get character from typeahead
		}
	}

	اگر (keylen > 0)	    // full matching terminal code
	{
#اگر defined(FEAT_GUI) && defined(FEAT_MENU)
	    اگر (typebuf.tb_len >= 2
		    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
			      && typebuf.tb_buf[typebuf.tb_off + 1] == KS_MENU)
	    {
		صحیح	idx;

		// Using a menu may cause a break in undo!  It's like using
		// gotchars(), but without recording or writing to a script
		// file.
		may_sync_undo();
		del_typebuf(3, 0);
		idx = get_menu_index(current_menu, local_State);
		اگر (idx != MENU_INDEX_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    اگر (VIsual_active && VIsual_select
					&& (current_menu->modes & MODE_VISUAL))
		    {
			VIsual_select = FALSE;
			(تهی)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->strings[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#endif // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    بازگشت map_result_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	اگر (mp == پوچ || keylen < 0)
	    keylen = KEYLEN_PART_کلید;
	وگرنه
	    keylen = mp_match_len;
    }

    /*
     * complete match
     */
    اگر (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#ifdef FEAT_EVAL
	صحیح	save_m_عبارت;
	صحیح	save_m_noremap;
	صحیح	save_m_silent;
	char_u	*save_m_کلیدها;
	char_u	*save_alt_m_کلیدها;
	صحیح	save_alt_m_keylen;
#وگرنه
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#endif

	// write chars to script file(s)
	اگر (keylen > typebuf.tb_maplen)
	    gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						   keylen - typebuf.tb_maplen);

	cmd_silent = (typebuf.tb_silent > 0);
	del_typebuf(keylen, 0);	// remove the mapped keys

	/*
	 * Put the replacement string in front of mapstr.
	 * The depth check catches ":map x y" and ":map y x".
	 */
	اگر (++*mapdepth >= p_mmd)
	{
	    emsg(_(e_recursive_mapping));
	    اگر (State & MODE_CMDLINE)
		redrawcmdline();
	    وگرنه
		setcursor();
	    flush_buffers(FLUSH_MINIMAL);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    بازگشت map_result_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	اگر (VIsual_active && VIsual_select && (mp->m_mode & MODE_VISUAL))
	{
	    VIsual_select = FALSE;
	    (تهی)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#ifdef FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_عبارت = mp->m_عبارت;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_کلیدها = پوچ;  // only saved when needed
	save_alt_m_کلیدها = پوچ;  // only saved when needed
	save_alt_m_keylen = mp->m_alt != پوچ ? mp->m_alt->m_keylen : 0;

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	اگر (mp->m_عبارت)
	{
	    صحیح save_vgetc_busy = vgetc_busy;
	    صحیح save_may_garbage_collect = may_garbage_collect;
	    صحیح was_screen_col = screen_cur_col;
	    صحیح was_screen_row = screen_cur_row;
	    صحیح prev_did_emsg = did_emsg;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_کلیدها = vim_strnsave(mp->m_کلیدها, (size_t)mp->m_keylen);
	    save_alt_m_کلیدها = mp->m_alt != پوچ
				    ? vim_strnsave(mp->m_alt->m_کلیدها,
					     (size_t)save_alt_m_keylen) : پوچ;
	    map_str = eval_map_عبارت(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_row, was_screen_col);
	    out_flush();

	    // If an error was displayed and the expression returns an empty
	    // string, generate a <Nop> to allow for a redraw.
	    اگر (prev_did_emsg != did_emsg
				       && (map_str == پوچ || *map_str == NUL))
	    {
		char_u	buf[4];

		vim_free(map_str);
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = KE_IGNORE;
		buf[3] = NUL;
		map_str = vim_strnsave(buf, 3);
		اگر (State & MODE_CMDLINE)
		{
		    // redraw the command below the error
		    پیام_didout = TRUE;
		    اگر (پیام_row < cmdline_row)
			پیام_row = cmdline_row;
		    redrawcmd();
		}
	    }

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	وگرنه
#endif
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	اگر (map_str == پوچ)
	    i = FAIL;
	وگرنه
	{
	    صحیح noremap;

#ifdef FEAT_EVAL
	    last_used_map = mp;
	    last_used_sid = -1;
#endif
	    اگر (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    وگرنه اگر (
#ifdef FEAT_EVAL
		save_m_عبارت ?
		(save_m_کلیدها != پوچ
			&& STRNCMP(map_str, save_m_کلیدها, (size_t)keylen) == 0)
		|| (save_alt_m_کلیدها != پوچ
			&& STRNCMP(map_str, save_alt_m_کلیدها,
					    (size_t)save_alt_m_keylen) == 0) :
#endif
		STRNCMP(map_str, mp->m_کلیدها, (size_t)keylen) == 0
		|| (mp->m_alt != پوچ
			&& STRNCMP(map_str, mp->m_alt->m_کلیدها,
					    (size_t)mp->m_alt->m_keylen) == 0))
		noremap = REMAP_SKIP;
	    وگرنه
		noremap = REMAP_YES;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#ifdef FEAT_EVAL
	    اگر (save_m_عبارت)
		vim_free(map_str);
#endif
	}
#ifdef FEAT_EVAL
	vim_free(save_m_کلیدها);
	vim_free(save_alt_m_کلیدها);
#endif
	*keylenp = keylen;
	اگر (i == FAIL)
	    بازگشت map_result_fail;
	بازگشت map_result_retry;
    }

    *keylenp = keylen;
    بازگشت map_result_nomatch;
}

/*
 * unget one character (can only be done once!)
 * If the character was stuffed, vgetc() will get it next time it is called.
 * Otherwise vgetc() will only get it when the stuff buffer is empty.
 */
    تهی
vungetc(صحیح c)
{
    old_char = c;
    old_mod_mask = mod_mask;
    old_mouse_row = mouse_row;
    old_mouse_col = mouse_col;
    old_کلید_Stuffed = کلید_Stuffed;
}

/*
 * When peeking and not getting a character, reg_executing cannot be cleared
 * yet, so set a flag to clear it later.
 */
    ایستا تهی
check_end_reg_executing(صحیح advance)
{
    اگر (reg_executing != 0 && (typebuf.tb_maplen == 0
						|| pending_end_reg_executing))
    {
	اگر (advance)
	{
	    reg_executing = 0;
	    pending_end_reg_executing = FALSE;
	}
	وگرنه
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
    ایستا صحیح
vgetorpeek(صحیح advance)
{
    صحیح		c;
    صحیح		timedout = FALSE;	// waited for more than 'timeoutlen'
					// for mapping to complete or
					// 'ttimeoutlen' for complete key code
    صحیح		mapdepth = 0;		// check for recursive mapping
    صحیح		mode_deleted = FALSE;   // set when mode has been deleted
    صحیح		new_wcol, new_wrow;
#ifdef FEAT_GUI
    صحیح		shape_changed = FALSE;  // adjusted cursor shape
#endif
    صحیح		n;
    صحیح		old_wcol, old_wrow;
    صحیح		wait_tb_len;

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
    اگر (vgetc_busy > 0 && ex_normal_busy == 0)
	بازگشت NUL;

    ++vgetc_busy;

    اگر (advance)
    {
	کلید_Stuffed = FALSE;
	typebuf_was_empty = FALSE;
    }

    init_typebuf();
    start_stuff();
    check_end_reg_executing(advance);
    انجامبده
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	اگر (typeahead_char != 0)
	{
	    c = typeahead_char;
	    اگر (advance)
		typeahead_char = 0;
	}
	وگرنه
	    c = read_readbuffers(advance);
	اگر (c != NUL && !got_int)
	{
	    اگر (advance)
	    {
		// KeyTyped = FALSE;  When the command that stuffed something
		// was typed, behave like the stuffed command was typed.
		// needed for CTRL-W CTRL-] to open a fold, for example.
		کلید_Stuffed = TRUE;
	    }
	    اگر (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	// no abbreviations now
	}
	وگرنه
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    برای (;;)
	    {
		long	wait_time;
		صحیح	keylen = 0;
		صحیح	showcmd_idx;
		check_end_reg_executing(advance);
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		اگر (typebuf.tb_maplen)
		    سطر_breakcheck();
		وگرنه
		    ui_breakcheck();		// check for CTRL-C
		اگر (got_int)
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
		    اگر ((c || typebuf.tb_maplen)
				     && (State & (MODE_درج | MODE_CMDLINE)))
			c = ESC;
		    وگرنه
			c = Ctrl_C;
		    flush_buffers(FLUSH_درون_داد);	// flush all typeahead

		    اگر (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    بشکن;
		}
		وگرنه اگر (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_result_T result = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    اگر (result == map_result_retry)
			// try mapping again
			ادامه;
		    اگر (result == map_result_fail)
		    {
			// failed, use the outer loop
			c = -1;
			بشکن;
		    }
		    اگر (result == map_result_get)
		    {
/*
 * get a character: 2. from the typeahead buffer
 */
			c = typebuf.tb_buf[typebuf.tb_off];
			اگر (advance)	// remove chars from typebuf
			{
			    cmd_silent = (typebuf.tb_silent > 0);
			    اگر (typebuf.tb_maplen > 0)
				کلید_Typed = FALSE;
			    وگرنه
			    {
				کلید_Typed = TRUE;
				// write char to script file(s)
				gotchars(typebuf.tb_buf
						 + typebuf.tb_off, 1);
			    }
			    کلید_Noremap = typebuf.tb_noremap[typebuf.tb_off]
							    & ~RM_SIMPLIFIED;
			    del_typebuf(1, 0);
			}
			بشکن;  // got character, break the for loop
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
		اگر (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& kitty_protocol_state != KKPS_ENABLED
			&& ex_normal_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & MODE_درج)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_کلید && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	col = 0;
		    char_u	*ptr;

		    اگر (mode_displayed)
		    {
			unshowmode(TRUE);
			mode_deleted = TRUE;
		    }
#ifdef FEAT_GUI
		    // may show a different cursor shape
		    اگر (gui.in_use && State != MODE_NORMAL && !cmd_silent)
		    {
			صحیح	    save_State;

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
		    اگر (curwin->w_cursor.col != 0)
		    {
			اگر (curwin->w_wcol > 0)
			{
			    // After auto-indenting and no text is following,
			    // we are expecting to truncate the trailing
			    // white-space, so find the last non-white
			    // character -- webb
			    اگر (did_ai && *skipwhite(ml_get_curline()
						+ curwin->w_cursor.col) == NUL)
			    {
				chartabsize_T cts;

				curwin->w_wcol = 0;
				ptr = ml_get_curline();
				init_chartabsize_arg(&cts, curwin,
					  curwin->w_cursor.lnum, 0, ptr, ptr);
				وقتی (cts.cts_ptr < ptr + curwin->w_cursor.col)
				{
				    اگر (!VIM_ISWHITE(*cts.cts_ptr))
					curwin->w_wcol = cts.cts_vcol;
				    cts.cts_vcol += lbr_chartabsize(&cts);
				    اگر (has_mbyte)
					cts.cts_ptr +=
						   (*mb_ptr2len)(cts.cts_ptr);
				    وگرنه
					++cts.cts_ptr;
				}
				clear_chartabsize_arg(&cts);

				curwin->w_wrow = curwin->w_cline_row
					   + curwin->w_wcol / curwin->w_width;
				curwin->w_wcol %= curwin->w_width;
				curwin->w_wcol += curwin_col_off();
				col = 0;	// no correction needed
			    }
			    وگرنه
			    {
				--curwin->w_wcol;
				col = curwin->w_cursor.col - 1;
			    }
			}
			وگرنه اگر (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_width - 1;
			    col = curwin->w_cursor.col - 1;
			}
			اگر (has_mbyte && col > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    col -= (*mb_head_off)(ptr, ptr + col);
			    اگر ((*mb_ptr2cells)(ptr + col) > 1)
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
		اگر (c < 0)
		    ادامه;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		برای (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		اگر (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    ادامه;
		}

		اگر (ex_normal_busy > 0)
		{
		    ایستا صحیح tc = 0;

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    اگر (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			ادامه;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    اگر (p_im && (State & MODE_درج))
			c = Ctrl_L;
#ifdef FEAT_TERMINAL
		    وگرنه اگر (terminal_is_active())
			c = K_CANCEL;
#endif
		    وگرنه اگر ((State & MODE_CMDLINE)
					     || (cmdwin_type > 0 && tc == ESC))
			c = Ctrl_C;
		    وگرنه
			c = ESC;
		    tc = c;
		    // set a flag to indicate this wasn't a normal char
		    اگر (advance)
			typebuf_was_empty = TRUE;

		    // return from main_loop()
		    اگر (pending_exmode_active)
			exmode_active = EXMODE_NORMAL;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    بشکن;
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
		اگر (((State & MODE_درج) != 0 || p_lz)
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
		صحیح showing_partial = FALSE;
		اگر (typebuf.tb_len > 0 && advance && !exmode_active)
		{
		    اگر (((State & (MODE_NORMAL | MODE_درج))
						      || State == MODE_LANGMAP)
			    && State != MODE_HITRETURN)
		    {
			// this looks nice when typing a dead character map
			اگر (State & MODE_درج
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
			افزودن_showcmd();
			اگر (typebuf.tb_len > SHOWCMD_COLS)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_COLS;
			وقتی (showcmd_idx < typebuf.tb_len)
			    add_byte_to_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
		    }

		    // This looks nice when typing a dead character map.
		    // There is no actual command line for get_number().
		    اگر ((State & MODE_CMDLINE)
			    && get_cmdline_info()->cmdbuff != پوچ
#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
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
		اگر (typebuf.tb_len == 0)
		    // timedout may have been set if a mapping with empty RHS
		    // fully matched while longer mappings timed out.
		    timedout = FALSE;

		اگر (advance)
		{
		    اگر (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_کلید)))
			// blocking wait
			wait_time = -1L;
		    وگرنه اگر (keylen == KEYLEN_PART_کلید && p_ttm >= 0)
			wait_time = p_ttm;
		    وگرنه
			wait_time = p_tm;
		}
		وگرنه
		    wait_time = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_time);

		اگر (showcmd_idx != 0)
		    برداشت_showcmd();
		اگر (showing_partial)
		{
		    اگر (State & MODE_درج)
			edit_unputchar();
		    اگر ((State & MODE_CMDLINE)
					&& get_cmdline_info()->cmdbuff != پوچ)
			unputcmdline();
		    وگرنه
			setcursor();	// put cursor back where it belongs
		}

		اگر (c < 0)
		    ادامه;		// end of input script reached
		اگر (c == NUL)		// no character available
		{
		    اگر (!advance)
			بشکن;
		    اگر (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			ادامه;
		    }
		}
		وگرنه
		{	    // allow mapping for just typed characters
		    وقتی (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#ifdef HAVE_درون_داد_METHOD
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_active = im_get_status();
#endif
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } وقتی ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    اگر (advance && p_smd && پیام_silent == 0 && (State & MODE_درج))
    {
	اگر (c == ESC && !mode_deleted && !no_mapping && mode_displayed)
	{
	    اگر (typebuf.tb_len && !کلید_Typed)
		redraw_cmdline = TRUE;	    // delete mode later
	    وگرنه
		unshowmode(FALSE);
	}
	وگرنه اگر (c != ESC && mode_deleted)
	{
	    اگر (typebuf.tb_len && !کلید_Typed)
		redraw_cmdline = TRUE;	    // show mode later
	    وگرنه
		showmode();
	}
    }
#ifdef FEAT_GUI
    // may unshow different cursor shape
    اگر (gui.in_use && shape_changed)
	gui_update_cursor(TRUE, FALSE);
#endif
    اگر (timedout && c == ESC)
    {
	// When recording there will be no timeout.  Add an <Ignore> after the
	// ESC to avoid that it forms a key code with following characters.
	gotchars_ignore();
    }

    --vgetc_busy;

    بازگشت c;
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
    ایستا صحیح
inchar(
    char_u	*buf,
    صحیح		maxlen,
    long	wait_time)	    // milliseconds
{
    صحیح		len = 0;	    // init for GCC
    صحیح		retesc = FALSE;	    // return ESC with gotint
    صحیح		script_char;
    صحیح		tb_change_cnt = typebuf.tb_change_cnt;

    اگر (wait_time == -1L || wait_time > 100L)  // flush output before waiting
    {
	cursor_on();
	out_flush_cursor(FALSE, FALSE);
#اگر defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	اگر (gui.in_use && postponed_mouseshape)
	    update_mouseshape(-1);
#endif
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    اگر (State != MODE_HITRETURN)
    {
	did_outofmem_پیام = FALSE;   // display out of memory message (again)
	did_swapwrite_پیام = FALSE;  // display swap file write error again
    }
    undo_off = FALSE;		    // restart undo now

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    وقتی (scriptin[curscript] != پوچ && script_char < 0
#ifdef FEAT_EVAL
	    && !ignore_script
#endif
	    )
    {
#ifdef MESSAGE_QUEUE
	parse_queued_messages();
#endif

	اگر (got_int || (script_char = getc(scriptin[curscript])) < 0)
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
	    اگر (got_int)
		retesc = TRUE;
	    وگرنه
		بازگشت -1;
	}
	وگرنه
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    اگر (script_char < 0)	// did not get a character from script
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	اگر (got_int)
	{
#define DUM_LEN (MAXMAPLEN * 3 + 3)
	    char_u	dum[DUM_LEN + 1];

	    برای (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		اگر (len == 0 || (len == 1 && dum[0] == Ctrl_C))
		    بشکن;
	    }
	    بازگشت retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	اگر (wait_time == -1L || wait_time > 10L)
	    out_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_time, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    اگر (typebuf_changed(tb_change_cnt))
	بازگشت 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    اگر (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    بازگشت fix_درون_داد_buffer(buf, len);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * "buf[]" must have room to triple the number of bytes!
 * Returns the new length.
 */
    صحیح
fix_درون_داد_buffer(char_u *buf, صحیح len)
{
    صحیح		i;
    char_u	*p = buf;

    /*
     * Two characters are special: NUL and K_SPECIAL.
     * When compiled With the GUI CSI is also special.
     * Replace	     NUL by K_SPECIAL KS_ZERO	 KE_FILLER
     * Replace K_SPECIAL by K_SPECIAL KS_SPECIAL KE_FILLER
     * Replace       CSI by K_SPECIAL KS_EXTRA   KE_CSI
     */
    برای (i = len; --i >= 0; ++p)
    {
#ifdef FEAT_GUI
	// When the GUI is used any character can come after a CSI, don't
	// escape it.
	اگر (gui.in_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
# ifndef MSWIN
	// When not on MS-Windows and the GUI is not used CSI needs to be
	// escaped.
	وگرنه اگر (!gui.in_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (صحیح)KE_CSI;
	    len += 2;
	}
# endif
	وگرنه
#endif
	اگر (p[0] == NUL || (p[0] == K_SPECIAL
		    // timeout may generate K_CURSORHOLD,
		    // 'autocompletedelay' timeout K_COMPLETE_DELAY
		    && (i < 2 || p[1] != KS_EXTRA
			|| (p[2] != (صحیح)KE_CURSORHOLD
			    && p[2] != (صحیح)KE_COMPLETE_DELAY))
#اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
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
    بازگشت len;
}

#اگر defined(USE_درون_داد_BUF)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    صحیح
درون_داد_available(تهی)
{
    بازگشت (!vim_is_درون_داد_buf_empty()
# اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#endif

/*
 * Function passed to do_cmdline() to get the command after a <Cmd> key from
 * typeahead.
 */
    ایستا char_u *
getcmdkeycmd(
	صحیح		promptc UNUSED,
	تهی		*cookie UNUSED,
	صحیح		indent UNUSED,
	getline_opt_T	do_concat UNUSED)
{
    garray_T	سطر_ga;
    صحیح		c1 = -1;
    صحیح		c2;
    صحیح		cmod = 0;
    صحیح		aborted = FALSE;

    ga_init2(&سطر_ga, 1, 32);

    // no mapping for these characters
    no_mapping++;

    got_int = FALSE;
    وقتی (c1 != NUL && !aborted)
    {
	اگر (ga_grow(&سطر_ga, 32) == FAIL)
	{
	    aborted = TRUE;
	    بشکن;
	}

	اگر (vgetorpeek(FALSE) == NUL)
	{
	    // incomplete <Cmd> is an error, because there is not much the user
	    // could do in this state.
	    emsg(_(e_cmd_mapping_must_end_with_cr));
	    aborted = TRUE;
	    بشکن;
	}

	// Get one character at a time.
	c1 = vgetorpeek(TRUE);

	// Get two extra bytes for special keys
	اگر (c1 == K_SPECIAL)
	{
	    c1 = vgetorpeek(TRUE);
	    c2 = vgetorpeek(TRUE);
	    اگر (c1 == KS_MODIFIER)
	    {
		cmod = c2;
		ادامه;
	    }
	    c1 = TO_SPECIAL(c1, c2);

	    // K_ESC is used to avoid ambiguity with the single Esc character
	    // that might be the start of an escape sequence.  Convert it back
	    // to a single Esc here.
	    اگر (c1 == K_ESC)
		c1 = ESC;

#ifdef FEAT_GUI
	    // Translate K_CSI to CSI.  The special key is only used to
	    // avoid it being recognized as the start of a special key.
	    اگر (c1 == K_CSI)
		c1 = CSI;
#endif
	}

	اگر (got_int)
	    aborted = TRUE;
	وگرنه اگر (c1 == '\r' || c1 == '\n')
	    c1 = NUL;  // end the line
	وگرنه اگر (c1 == ESC)
	    aborted = TRUE;
	وگرنه اگر (c1 == K_COMMAND || c1 == K_SCRIPT_COMMAND)
	{
	    // give a nicer error message for this special case
	    emsg(_(e_cmd_mapping_must_end_with_cr_before_second_cmd));
	    aborted = TRUE;
	}
	وگرنه اگر (c1 == K_SNR)
	{
	    GA_CONCAT_LITERAL(&سطر_ga, "<SNR>");
	}
	وگرنه
	{
	    اگر (cmod != 0)
	    {
		ga_append(&سطر_ga, K_SPECIAL);
		ga_append(&سطر_ga, KS_MODIFIER);
		ga_append(&سطر_ga, cmod);
	    }
	    اگر (IS_SPECIAL(c1))
	    {
		ga_append(&سطر_ga, K_SPECIAL);
		ga_append(&سطر_ga, K_SECOND(c1));
		ga_append(&سطر_ga, K_THIRD(c1));
	    }
	    وگرنه
		ga_append(&سطر_ga, c1);
	}

	cmod = 0;
    }

    no_mapping--;

    اگر (aborted)
	ga_clear(&سطر_ga);

    بازگشت (char_u *)سطر_ga.ga_data;
}

#اگر defined(FEAT_EVAL)
/*
 * If there was a mapping we get its SID.  Otherwise, use "last_used_sid", it
 * is set when redo'ing.
 * Put this SID in the redo buffer, so that "." will use the same script
 * context.
 */
    تهی
may_add_last_used_map_to_redobuff(تهی)
{
    char_u  buf[3 + 20];
    صحیح	    buflen;
    صحیح	    sid = -1;

    اگر (last_used_map != پوچ)
	sid = last_used_map->m_script_ctx.sc_sid;
    اگر (sid < 0)
	sid = last_used_sid;

    اگر (sid < 0)
	بازگشت;

    // <K_SID>{nr};
    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = KE_SID;
    buflen = 3;

    buflen += vim_snprintf((نویسه *)buf + 3, 20, "%d;", sid);
    add_buff(&redobuff, buf, (long)buflen);
}
#endif

    صحیح
do_cmdkey_command(صحیح کلید UNUSED, صحیح flags)
{
    صحیح	    res;
#ifdef FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    اگر (کلید == K_SCRIPT_COMMAND
		  && (last_used_map != پوچ || SCRIPT_ID_VALID(last_used_sid)))
    {
	save_current_sctx = current_sctx;
	اگر (last_used_map != پوچ)
	    current_sctx = last_used_map->m_script_ctx;
	وگرنه
	{
	    current_sctx.sc_sid = last_used_sid;
	    current_sctx.sc_lnum = 0;
	    current_sctx.sc_version = SCRIPT_ITEM(last_used_sid)->sn_version;
	}
    }
#endif

    res = do_cmdline(پوچ, getcmdkeycmd, پوچ, flags);

#ifdef FEAT_EVAL
    اگر (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#endif

    بازگشت res;
}

#اگر defined(FEAT_EVAL)
    تهی
reset_last_used_map(mapblock_T *mp)
{
    اگر (last_used_map != mp)
	بازگشت;

    last_used_map = پوچ;
    last_used_sid = -1;
}
#endif
