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

#അന്തര്ഭാവ "vim.h"

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

സ്ഥിതം buffheader_T redobuff = {{ശൂന്യ, 0, {NUL}}, ശൂന്യ, 0, 0, FALSE};
സ്ഥിതം buffheader_T old_redobuff = {{ശൂന്യ, 0, {NUL}}, ശൂന്യ, 0, 0, FALSE};
സ്ഥിതം buffheader_T recordbuff = {{ശൂന്യ, 0, {NUL}}, ശൂന്യ, 0, 0, FALSE};

സ്ഥിതം പൂർണ്ണസംഖ്യ typeahead_char = 0;		// typeahead char that's not flushed

#ifdef FEAT_EVAL
സ്ഥിതം char_u typedchars[MAXMAPLEN + 1] = { NUL };  // typed chars before map
സ്ഥിതം പൂർണ്ണസംഖ്യ typedchars_pos = 0;
#endif

/*
 * When block_redo is TRUE the redo buffer will not be changed.
 * Used by edit() to repeat insertions.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ	block_redo = FALSE;

സ്ഥിതം പൂർണ്ണസംഖ്യ	താക്കോൽ_Noremap = 0;	    // remapping flags

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
#അല്ലെങ്കിൽ // Tiny version
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
#endif
സ്ഥിതം char_u	typebuf_init[TYPELEN_INIT];	// initial typebuf.tb_buf
സ്ഥിതം char_u	noremapbuf_init[TYPELEN_INIT];	// initial typebuf.tb_noremap

സ്ഥിതം size_t	last_recorded_len = 0;	// number of last recorded chars

#ifdef FEAT_EVAL
mapblock_T	*last_used_map = ശൂന്യ;
പൂർണ്ണസംഖ്യ		last_used_sid = -1;
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ	read_readbuf(buffheader_T *buf, പൂർണ്ണസംഖ്യ advance);
സ്ഥിതം ശൂന്യം	init_typebuf(ശൂന്യം);
സ്ഥിതം ശൂന്യം	may_sync_undo(ശൂന്യം);
സ്ഥിതം ശൂന്യം	free_typebuf(ശൂന്യം);
സ്ഥിതം ശൂന്യം	closescript(ശൂന്യം);
സ്ഥിതം ശൂന്യം	updatescript(പൂർണ്ണസംഖ്യ c);
സ്ഥിതം പൂർണ്ണസംഖ്യ	vgetorpeek(പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ	inchar(char_u *buf, പൂർണ്ണസംഖ്യ maxlen, long wait_time);
#ifdef FEAT_EVAL
സ്ഥിതം പൂർണ്ണസംഖ്യ	do_താക്കോൽ_നിവേശം_pre(പൂർണ്ണസംഖ്യ c);
#endif

/*
 * Free and clear a buffer.
 */
    സ്ഥിതം ശൂന്യം
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    വേണ്ടി (p = buf->bh_first.b_next; p != ശൂന്യ; p = np)
    {
	np = p->b_next;
	vim_free(p);
    }
    buf->bh_first.b_next = ശൂന്യ;
    buf->bh_curr = ശൂന്യ;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    സ്ഥിതം char_u *
get_buffcont(
    buffheader_T	*buffer,
    പൂർണ്ണസംഖ്യ			dozero,	    // count == zero is not an error
    size_t		*len)	    // the length of the returned buffer
{
    long_u	    എണ്ണം = 0;
    char_u	    *p = ശൂന്യ;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;
    size_t	    i = 0;

    // compute the total length of the string
    വേണ്ടി (bp = buffer->bh_first.b_next; bp != ശൂന്യ; bp = bp->b_next)
	എണ്ണം += (long_u)bp->b_strlen;

    എങ്കിൽ ((എണ്ണം > 0 || dozero) && (p = alloc(എണ്ണം + 1)) != ശൂന്യ)
    {
	p2 = p;
	വേണ്ടി (bp = buffer->bh_first.b_next; bp != ശൂന്യ; bp = bp->b_next)
	    വേണ്ടി (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
	i = (size_t)(p2 - p);
    }

    എങ്കിൽ (len != ശൂന്യ)
	*len = i;

    തിരികെ p;
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(ശൂന്യം)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE, &len);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;

    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    എങ്കിൽ (len >= last_recorded_len)
    {
	len -= last_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    എങ്കിൽ (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    തിരികെ (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    string_T
get_inserted(ശൂന്യം)
{
    size_t len = 0;
    char_u *str = get_buffcont(&redobuff, FALSE, &len);
    string_T ret = { str, len };
    തിരികെ ret;
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    സ്ഥിതം ശൂന്യം
add_buff(
    buffheader_T	*buf,
    char_u		*s,
    long		slen)	// length of "s" or -1
{
    എങ്കിൽ (slen < 0)
	slen = (long)STRLEN(s);
    എങ്കിൽ (slen == 0)				// don't add empty strings
	തിരികെ;

    എങ്കിൽ (buf->bh_first.b_next == ശൂന്യ)	// first add to list
    {
	buf->bh_curr = &(buf->bh_first);
	buf->bh_create_newblock = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (buf->bh_curr == ശൂന്യ)	// buffer has already been read
    {
	iemsg(e_add_to_internal_buffer_that_was_already_read_from);
	തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (buf->bh_index != 0)
    {
	mch_memmove(buf->bh_first.b_next->b_str,
		    buf->bh_first.b_next->b_str + buf->bh_index,
		    (buf->bh_first.b_next->b_strlen - buf->bh_index) + 1);
	buf->bh_first.b_next->b_strlen -= buf->bh_index;
	buf->bh_space += buf->bh_index;
    }
    buf->bh_index = 0;

    എങ്കിൽ (!buf->bh_create_newblock && buf->bh_space >= (പൂർണ്ണസംഖ്യ)slen)
    {
	vim_strncpy(buf->bh_curr->b_str + buf->bh_curr->b_strlen, s, (size_t)slen);
	buf->bh_curr->b_strlen += slen;
	buf->bh_space -= slen;
    }
    അല്ലെങ്കിൽ
    {
	long_u	    len;
	buffblock_T *p;

	എങ്കിൽ (slen < MINIMAL_SIZE)
	    len = MINIMAL_SIZE;
	അല്ലെങ്കിൽ
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ; // no space, just forget it
	vim_strncpy(p->b_str, s, (size_t)slen);
	p->b_strlen = slen;
	buf->bh_space = (പൂർണ്ണസംഖ്യ)(len - slen);
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
    സ്ഥിതം ശൂന്യം
delete_buff_tail(buffheader_T *buf, പൂർണ്ണസംഖ്യ slen)
{
    എങ്കിൽ (buf->bh_curr == ശൂന്യ)
	തിരികെ;  // nothing to delete
    എങ്കിൽ (buf->bh_curr->b_strlen < (size_t)slen)
	തിരികെ;

    buf->bh_curr->b_str[buf->bh_curr->b_strlen - (size_t)slen] = NUL;
    buf->bh_curr->b_strlen -= slen;
    buf->bh_space += slen;
}

/*
 * Add number "n" to buffer "buf".
 */
    സ്ഥിതം ശൂന്യം
add_അക്കം_buff(buffheader_T *buf, long n)
{
    char_u	സംഖ്യ[32];
    പൂർണ്ണസംഖ്യ		numberlen;

    numberlen = vim_snprintf((അക്ഷരം *)സംഖ്യ, വലുപ്പം(സംഖ്യ), "%ld", n);
    add_buff(buf, സംഖ്യ, (long)numberlen);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    സ്ഥിതം ശൂന്യം
add_char_buff(buffheader_T *buf, പൂർണ്ണസംഖ്യ c)
{
    char_u	bytes[MB_MAXBYTES + 1];
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i;
    char_u	temp[4];
    long	templen;

    എങ്കിൽ (IS_SPECIAL(c))
	len = 1;
    അല്ലെങ്കിൽ
	len = (*mb_char2bytes)(c, bytes);
    വേണ്ടി (i = 0; i < len; ++i)
    {
	എങ്കിൽ (!IS_SPECIAL(c))
	    c = bytes[i];

	എങ്കിൽ (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    temp[0] = K_SPECIAL;
	    temp[1] = K_SECOND(c);
	    temp[2] = K_THIRD(c);
	    temp[3] = NUL;
	    templen = 3;
	}
#ifdef FEAT_GUI
	അല്ലെങ്കിൽ എങ്കിൽ (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    temp[0] = CSI;
	    temp[1] = KS_EXTRA;
	    temp[2] = (പൂർണ്ണസംഖ്യ)KE_CSI;
	    temp[3] = NUL;
	    templen = 3;
	}
#endif
	അല്ലെങ്കിൽ
	{
	    temp[0] = c;
	    temp[1] = NUL;
	    templen = 1;
	}
	add_buff(buf, temp, templen);
    }
}

// First read ahead buffer. Used for translated commands.
സ്ഥിതം buffheader_T readbuf1 = {{ശൂന്യ, 0, {NUL}}, ശൂന്യ, 0, 0, FALSE};

// Second read ahead buffer. Used for redo.
സ്ഥിതം buffheader_T readbuf2 = {{ശൂന്യ, 0, {NUL}}, ശൂന്യ, 0, 0, FALSE};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_readbuffers(പൂർണ്ണസംഖ്യ advance)
{
    പൂർണ്ണസംഖ്യ c;

    c = read_readbuf(&readbuf1, advance);
    എങ്കിൽ (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    തിരികെ c;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_readbuf(buffheader_T *buf, പൂർണ്ണസംഖ്യ advance)
{
    char_u	c;
    buffblock_T	*curr;

    എങ്കിൽ (buf->bh_first.b_next == ശൂന്യ)  // buffer is empty
	തിരികെ NUL;

    curr = buf->bh_first.b_next;
    c = curr->b_str[buf->bh_index];

    എങ്കിൽ (advance)
    {
	എങ്കിൽ (curr->b_str[++buf->bh_index] == NUL)
	{
	    buf->bh_first.b_next = curr->b_next;
	    vim_free(curr);
	    buf->bh_index = 0;
	}
    }
    തിരികെ c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    സ്ഥിതം ശൂന്യം
start_stuff(ശൂന്യം)
{
    എങ്കിൽ (readbuf1.bh_first.b_next != ശൂന്യ)
    {
	readbuf1.bh_curr = &(readbuf1.bh_first);
	readbuf1.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
    എങ്കിൽ (readbuf2.bh_first.b_next != ശൂന്യ)
    {
	readbuf2.bh_curr = &(readbuf2.bh_first);
	readbuf2.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    പൂർണ്ണസംഖ്യ
stuff_empty(ശൂന്യം)
{
    തിരികെ (readbuf1.bh_first.b_next == ശൂന്യ
	 && readbuf2.bh_first.b_next == ശൂന്യ);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * readbuf2.
 */
    പൂർണ്ണസംഖ്യ
readbuf1_empty(ശൂന്യം)
{
    തിരികെ (readbuf1.bh_first.b_next == ശൂന്യ);
}
#endif

/*
 * Set a typeahead character that won't be flushed.
 */
    ശൂന്യം
typeahead_noflush(പൂർണ്ണസംഖ്യ c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    ശൂന്യം
flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    start_stuff();
    ആകുമ്പോൾ (read_readbuffers(TRUE) != NUL)
	;

    എങ്കിൽ (flush_typeahead == FLUSH_MINIMAL)
    {
	// remove mapped characters at the start only,
	// but only when enough space left in typebuf
	എങ്കിൽ (typebuf.tb_off + typebuf.tb_maplen >= typebuf.tb_buflen)
	{
	    typebuf.tb_off = MAXMAPLEN;
	    typebuf.tb_len = 0;
	}
	അല്ലെങ്കിൽ
	{
	    typebuf.tb_off += typebuf.tb_maplen;
	    typebuf.tb_len -= typebuf.tb_maplen;
	}
#എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	എങ്കിൽ (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#endif
    }
    അല്ലെങ്കിൽ
    {
	// remove typeahead
	എങ്കിൽ (flush_typeahead == FLUSH_നിവേശം)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    ആകുമ്പോൾ (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#endif
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
    എങ്കിൽ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * The previous contents of the redo buffer is kept in old_redobuffer.
 * This is used for the CTRL-O <.> command in insert mode.
 */
    ശൂന്യം
ResetRedobuff(ശൂന്യം)
{
    എങ്കിൽ (block_redo)
	തിരികെ;

    free_buff(&old_redobuff);
    old_redobuff = redobuff;
    redobuff.bh_first.b_next = ശൂന്യ;
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    ശൂന്യം
CancelRedo(ശൂന്യം)
{
    എങ്കിൽ (block_redo)
	തിരികെ;

    free_buff(&redobuff);
    redobuff = old_redobuff;
    old_redobuff.bh_first.b_next = ശൂന്യ;
    start_stuff();
    ആകുമ്പോൾ (read_readbuffers(TRUE) != NUL)
	;
}

/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
    ശൂന്യം
saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;
    size_t	slen;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_first.b_next = ശൂന്യ;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_first.b_next = ശൂന്യ;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE, &slen);
    എങ്കിൽ (s == ശൂന്യ)
	തിരികെ;

    add_buff(&redobuff, s, (long)slen);
    vim_free(s);
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    ശൂന്യം
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
    ശൂന്യം
AppendToRedobuff(char_u *s)
{
    എങ്കിൽ (!block_redo)
	add_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    ശൂന്യം
AppendToRedobuffLit(
    char_u	*str,
    പൂർണ്ണസംഖ്യ		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    പൂർണ്ണസംഖ്യ		c;
    char_u	*start;

    എങ്കിൽ (block_redo)
	തിരികെ;

    ആകുമ്പോൾ (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	start = s;
	ആകുമ്പോൾ (*s >= ' ' && *s < DEL && (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	എങ്കിൽ (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	എങ്കിൽ (s > start)
	    add_buff(&redobuff, start, (long)(s - start));

	എങ്കിൽ (*s == NUL || (len >= 0 && s - str >= len))
	    നിർത്തുക;

	// Handle a special or multibyte character.
	എങ്കിൽ (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	അല്ലെങ്കിൽ
	    c = *s++;
	എങ്കിൽ (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    add_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048
	എങ്കിൽ (*s == NUL && c == '0')
	    add_buff(&redobuff, (char_u *)"048", 3L);
	അല്ലെങ്കിൽ
	    add_char_buff(&redobuff, c);
    }
}

/*
 * Append "s" to the redo buffer, leaving 3-byte special key codes unmodified
 * and escaping other K_SPECIAL and CSI bytes.
 */
    ശൂന്യം
AppendToRedobuffSpec(char_u *s)
{
    എങ്കിൽ (block_redo)
	തിരികെ;

    ആകുമ്പോൾ (*s != NUL)
    {
	എങ്കിൽ (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    add_buff(&redobuff, s, 3L);
	    s += 3;
	}
	അല്ലെങ്കിൽ
	    add_char_buff(&redobuff, mb_cptr2char_adv(&s));
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ശൂന്യം
AppendCharToRedobuff(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (!block_redo)
	add_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    ശൂന്യം
Append_സംഖ്യ_To_Redobuff(long n)
{
    എങ്കിൽ (!block_redo)
	add_അക്കം_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    ശൂന്യം
stuffReadbuff(char_u *s)
{
    add_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    ശൂന്യം
stuffRedoReadbuff(char_u *s)
{
    add_buff(&readbuf2, s, -1L);
}

    ശൂന്യം
stuffReadbuffLen(char_u *s, long len)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (add_last_തിരുകുക == 1) // Only add if this is the first call, for
			      // recursive calls, ignore.
	ga_concat_len(&last_തിരുകുക_ga, s, (size_t)len);
#endif
    add_buff(&readbuf1, s, len);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    ശൂന്യം
stuffReadbuffSpec(char_u *s)
{
    പൂർണ്ണസംഖ്യ c;

    ആകുമ്പോൾ (*s != NUL)
    {
	എങ്കിൽ (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	അല്ലെങ്കിൽ
	{
	    c = mb_cptr2char_adv(&s);
	    എങ്കിൽ (c == CAR || c == NL || c == ESC)
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
    ശൂന്യം
stuffcharReadbuff(പൂർണ്ണസംഖ്യ c)
{
    add_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    ശൂന്യം
stuffnumReadbuff(long n)
{
    add_അക്കം_buff(&readbuf1, n);
}

/*
 * Stuff a string into the typeahead buffer, such that edit() will insert it
 * literally ("literally" TRUE) or interpret is as typed characters.
 */
    ശൂന്യം
stuffescaped(char_u *arg, പൂർണ്ണസംഖ്യ literally)
{
    പൂർണ്ണസംഖ്യ		c;
    char_u	*start;

    ആകുമ്പോൾ (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	start = arg;
	ആകുമ്പോൾ ((*arg >= ' ' && *arg < DEL)
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	എങ്കിൽ (arg > start)
	    stuffReadbuffLen(start, (long)(arg - start));

	// stuff a single special character
	എങ്കിൽ (*arg != NUL)
	{
	    എങ്കിൽ (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    അല്ലെങ്കിൽ
		c = *arg++;
	    എങ്കിൽ (literally && ((c < ' ' && c != TAB) || c == DEL))
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_redo(പൂർണ്ണസംഖ്യ init, പൂർണ്ണസംഖ്യ old_redo)
{
    സ്ഥിതം buffblock_T	*bp;
    സ്ഥിതം char_u	*p;
    പൂർണ്ണസംഖ്യ			c;
    പൂർണ്ണസംഖ്യ			n;
    char_u		buf[MB_MAXBYTES + 1];
    പൂർണ്ണസംഖ്യ			i;

    എങ്കിൽ (init)
    {
	എങ്കിൽ (old_redo)
	    bp = old_redobuff.bh_first.b_next;
	അല്ലെങ്കിൽ
	    bp = redobuff.bh_first.b_next;
	എങ്കിൽ (bp == ശൂന്യ)
	    തിരികെ FAIL;
	p = bp->b_str;
	തിരികെ OK;
    }
    എങ്കിൽ ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	എങ്കിൽ (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	അല്ലെങ്കിൽ
	    n = 1;
	വേണ്ടി (i = 0; ; ++i)
	{
	    എങ്കിൽ (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = TO_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#ifdef FEAT_GUI
	    എങ്കിൽ (c == CSI)	// escaped CSI
		p += 2;
#endif
	    എങ്കിൽ (*++p == NUL && bp->b_next != ശൂന്യ)
	    {
		bp = bp->b_next;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    എങ്കിൽ (i == n - 1)	// last byte of a character
	    {
		എങ്കിൽ (n != 1)
		    c = (*mb_ptr2char)(buf);
		നിർത്തുക;
	    }
	    c = *p;
	    എങ്കിൽ (c == NUL)	// cannot happen?
		നിർത്തുക;
	}
    }

    തിരികെ c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    സ്ഥിതം ശൂന്യം
copy_redo(പൂർണ്ണസംഖ്യ old_redo)
{
    പൂർണ്ണസംഖ്യ	    c;

    ആകുമ്പോൾ ((c = read_redo(FALSE, old_redo)) != NUL)
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
    പൂർണ്ണസംഖ്യ
start_redo(long എണ്ണം, പൂർണ്ണസംഖ്യ old_redo)
{
    പൂർണ്ണസംഖ്യ	    c;

    // init the pointers; return if nothing to redo
    എങ്കിൽ (read_redo(TRUE, old_redo) == FAIL)
	തിരികെ FAIL;

    c = read_redo(FALSE, old_redo);

#ifdef FEAT_EVAL
    എങ്കിൽ (c == K_SID)
    {
	// Copy the <SID>{sid}; sequence
	add_char_buff(&readbuf2, c);
	വേണ്ടി (;;)
	{
	    c = read_redo(FALSE, old_redo);
	    add_char_buff(&readbuf2, c);
	    എങ്കിൽ (!SAFE_isdigit(c))
		നിർത്തുക;
	}
	c = read_redo(FALSE, old_redo);
    }
#endif

    // copy the buffer name, if present
    എങ്കിൽ (c == '"')
    {
	add_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	// if a numbered buffer is used, increment the number
	എങ്കിൽ (c >= '1' && c < '9')
	    ++c;
	add_char_buff(&readbuf2, c);

	// the expression register should be re-evaluated
	എങ്കിൽ (c == '=')
	{
	    add_char_buff(&readbuf2, CAR);
	    cmd_silent = TRUE;
	}

	c = read_redo(FALSE, old_redo);
    }

    എങ്കിൽ (c == 'v')   // redo Visual
    {
	VIsual = curwin->w_cursor;
	VIsual_active = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    // try to enter the count (in place of a previous count)
    എങ്കിൽ (എണ്ണം)
    {
	ആകുമ്പോൾ (VIM_ISDIGIT(c))	// skip "old" count
	    c = read_redo(FALSE, old_redo);
	add_അക്കം_buff(&readbuf2, എണ്ണം);
    }

    // copy the rest from the redo buffer into the stuff buffer
    add_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    തിരികെ OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
start_redo_ins(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    c;

    എങ്കിൽ (read_redo(TRUE, FALSE) == FAIL)
	തിരികെ FAIL;
    start_stuff();

    // skip the count and the command character
    ആകുമ്പോൾ ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	എങ്കിൽ (vim_strchr((char_u *)"AaIiRrOo", c) != ശൂന്യ)
	{
	    എങ്കിൽ (c == 'O' || c == 'o')
		add_buff(&readbuf2, NL_STR, -1L);
	    നിർത്തുക;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    block_redo = TRUE;
    തിരികെ OK;
}

    ശൂന്യം
stop_redo_ins(ശൂന്യം)
{
    block_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    സ്ഥിതം ശൂന്യം
init_typebuf(ശൂന്യം)
{
    എങ്കിൽ (typebuf.tb_buf != ശൂന്യ)
	തിരികെ;

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
    പൂർണ്ണസംഖ്യ
noremap_താക്കോലുകൾ(ശൂന്യം)
{
    തിരികെ താക്കോൽ_Noremap & (RM_NONE|RM_SCRIPT);
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
    പൂർണ്ണസംഖ്യ
ins_typebuf(
    char_u	*str,
    പൂർണ്ണസംഖ്യ		noremap,
    പൂർണ്ണസംഖ്യ		offset,
    പൂർണ്ണസംഖ്യ		nottyped,
    പൂർണ്ണസംഖ്യ		silent)
{
    char_u	*s1, *s2;
    പൂർണ്ണസംഖ്യ		newlen;
    പൂർണ്ണസംഖ്യ		addlen;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		newoff;
    പൂർണ്ണസംഖ്യ		val;
    പൂർണ്ണസംഖ്യ		nrm;

    init_typebuf();
    എങ്കിൽ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
    state_no_longer_safe("ins_typebuf()");

    addlen = (പൂർണ്ണസംഖ്യ)STRLEN(str);

    എങ്കിൽ (offset == 0 && addlen <= typebuf.tb_off)
    {
	/*
	 * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
	 */
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (typebuf.tb_len == 0 && typebuf.tb_buflen
					       >= addlen + 3 * (MAXMAPLEN + 4))
    {
	/*
	 * Buffer is empty and string fits in the existing buffer.
	 * Leave some space before and after, if possible.
	 */
	typebuf.tb_off = (typebuf.tb_buflen - addlen - 3 * (MAXMAPLEN + 4)) / 2;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ extra;

	/*
	 * Need to allocate a new buffer.
	 * In typebuf.tb_buf there must always be room for 3 * (MAXMAPLEN + 4)
	 * characters.  We add some extra room to avoid having to allocate too
	 * often.
	 */
	newoff = MAXMAPLEN + 4;
	extra = addlen + newoff + 4 * (MAXMAPLEN + 4);
	എങ്കിൽ (typebuf.tb_len > 2147483647 - extra)
	{
	    // string is getting too long for a 32 bit int
	    emsg(_(e_command_too_complex));    // also calls flush_buffers
	    setcursor();
	    തിരികെ FAIL;
	}
	newlen = typebuf.tb_len + extra;
	s1 = alloc(newlen);
	എങ്കിൽ (s1 == ശൂന്യ)		    // out of memory
	    തിരികെ FAIL;
	s2 = alloc(newlen);
	എങ്കിൽ (s2 == ശൂന്യ)		    // out of memory
	{
	    vim_free(s1);
	    തിരികെ FAIL;
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
	എങ്കിൽ (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)offset);
	mch_memmove(s2 + newoff + offset + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + offset,
					   (size_t)(typebuf.tb_len - offset));
	എങ്കിൽ (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    // If noremap == REMAP_SCRIPT: do remap script-local mappings.
    എങ്കിൽ (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    അല്ലെങ്കിൽ എങ്കിൽ (noremap == REMAP_SKIP)
	val = RM_ABBR;
    അല്ലെങ്കിൽ
	val = RM_NONE;

    /*
     * Adjust typebuf.tb_noremap[] for the new characters:
     * If noremap == REMAP_NONE or REMAP_SCRIPT: new characters are
     *			(sometimes) not remappable
     * If noremap == REMAP_YES: all the new characters are mappable
     * If noremap  > 0: "noremap" characters are not remappable, the rest
     *			mappable
     */
    എങ്കിൽ (noremap == REMAP_SKIP)
	nrm = 1;
    അല്ലെങ്കിൽ എങ്കിൽ (noremap < 0)
	nrm = addlen;
    അല്ലെങ്കിൽ
	nrm = noremap;
    വേണ്ടി (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + offset] =
						  (--nrm >= 0) ? val : RM_YES;

    // tb_maplen and tb_silent only remember the length of mapped and/or
    // silent mappings at the start of the buffer, assuming that a mapped
    // sequence doesn't result in typed characters.
    എങ്കിൽ (nottyped || typebuf.tb_maplen > offset)
	typebuf.tb_maplen += addlen;
    എങ്കിൽ (silent || typebuf.tb_silent > offset)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    എങ്കിൽ (typebuf.tb_no_abbr_cnt && offset == 0)	// and not used for abbrev.s
	typebuf.tb_no_abbr_cnt += addlen;

    തിരികെ OK;
}

/*
 * Put character "c" back into the typeahead buffer.
 * Can be used for a character obtained by vgetc() that needs to be put back.
 * Uses cmd_silent, KeyTyped and KeyNoremap to restore the flags belonging to
 * the char.
 * Returns the length of what was inserted.
 */
    പൂർണ്ണസംഖ്യ
ins_char_typebuf(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ modifiers)
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    പൂർണ്ണസംഖ്യ		len = special_to_buf(c, modifiers, TRUE, buf);

    buf[len] = NUL;
    (ശൂന്യം)ins_typebuf(buf, താക്കോൽ_Noremap, 0, !താക്കോൽ_Typed, cmd_silent);
    തിരികെ len;
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
    പൂർണ്ണസംഖ്യ
typebuf_changed(
    പൂർണ്ണസംഖ്യ		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    തിരികെ (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#endif
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    പൂർണ്ണസംഖ്യ
typebuf_typed(ശൂന്യം)
{
    തിരികെ typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    പൂർണ്ണസംഖ്യ
typebuf_maplen(ശൂന്യം)
{
    തിരികെ typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    ശൂന്യം
del_typebuf(പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ offset)
{
    പൂർണ്ണസംഖ്യ	    i;

    എങ്കിൽ (len == 0)
	തിരികെ;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    എങ്കിൽ (offset == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    അല്ലെങ്കിൽ
    {
	i = typebuf.tb_off + offset;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	എങ്കിൽ (typebuf.tb_off > MAXMAPLEN)
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

    എങ്കിൽ (typebuf.tb_maplen > offset)		// adjust tb_maplen
    {
	എങ്കിൽ (typebuf.tb_maplen < offset + len)
	    typebuf.tb_maplen = offset;
	അല്ലെങ്കിൽ
	    typebuf.tb_maplen -= len;
    }
    എങ്കിൽ (typebuf.tb_silent > offset)		// adjust tb_silent
    {
	എങ്കിൽ (typebuf.tb_silent < offset + len)
	    typebuf.tb_silent = offset;
	അല്ലെങ്കിൽ
	    typebuf.tb_silent -= len;
    }
    എങ്കിൽ (typebuf.tb_no_abbr_cnt > offset)	// adjust tb_no_abbr_cnt
    {
	എങ്കിൽ (typebuf.tb_no_abbr_cnt < offset + len)
	    typebuf.tb_no_abbr_cnt = offset;
	അല്ലെങ്കിൽ
	    typebuf.tb_no_abbr_cnt -= len;
    }

#എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#endif
    എങ്കിൽ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * State for adding bytes to a recording or 'showcmd'.
 */
തരനിർവചനം ഘടന
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    പൂർണ്ണസംഖ്യ		prev_c;
    size_t	buflen;
    unsigned	pending_special;
    unsigned	pending_mbyte;
} gotchars_state_T;

/*
 * Add a single byte to a recording or 'showcmd'.
 * Return TRUE if a full key has been received, FALSE otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gotchars_add_byte(gotchars_state_T *state, char_u byte)
{
    പൂർണ്ണസംഖ്യ		c = state->buf[state->buflen++] = byte;
    പൂർണ്ണസംഖ്യ		retval = FALSE;
    പൂർണ്ണസംഖ്യ		in_special = state->pending_special > 0;
    പൂർണ്ണസംഖ്യ		in_mbyte = state->pending_mbyte > 0;

    എങ്കിൽ (in_special)
	state->pending_special--;
    അല്ലെങ്കിൽ എങ്കിൽ (c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	    )
	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	state->pending_special = 2;

    എങ്കിൽ (state->pending_special > 0)
	പോകുക ret_false;

    എങ്കിൽ (in_mbyte)
	state->pending_mbyte--;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (in_special)
	{
	    എങ്കിൽ (state->prev_c == KS_MODIFIER)
		// When receiving a modifier, wait for the modified key.
		പോകുക ret_false;
	    c = TO_SPECIAL(state->prev_c, c);
	    // Drop K_FOCUSGAINED, K_FOCUSLOST and K_COMPLETE_DELAY, they are
	    // not useful in a recording.
	    എങ്കിൽ (c == K_FOCUSGAINED || c == K_FOCUSLOST || c == K_COMPLETE_DELAY)
		state->buflen = 0;
	}
	// When receiving a multibyte character, store it until we have all
	// the bytes, so that it won't be split between two buffer blocks,
	// and delete_buff_tail() will work properly.
	state->pending_mbyte = MB_BYTE2LEN_CHECK(c) - 1;
    }

    എങ്കിൽ (state->pending_mbyte > 0)
	പോകുക ret_false;

    retval = TRUE;
ret_false:
    state->prev_c = c;
    തിരികെ retval;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the record buffer.
 */
    സ്ഥിതം ശൂന്യം
gotchars(char_u *chars, പൂർണ്ണസംഖ്യ len)
{
    char_u		*s = chars;
    size_t		i;
    പൂർണ്ണസംഖ്യ			todo = len;
    സ്ഥിതം gotchars_state_T state;

    ആകുമ്പോൾ (todo-- > 0)
    {
	എങ്കിൽ (!gotchars_add_byte(&state, *s++))
	    തുടരുക;

	// Handle one byte at a time; no translation to be done.
	വേണ്ടി (i = 0; i < state.buflen; ++i)
	    updatescript(state.buf[i]);

	എങ്കിൽ (reg_recording != 0)
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
    debug_did_സന്ദേശം = FALSE;
#endif

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}

/*
 * Record an <Ignore> key.
 */
    ശൂന്യം
gotchars_ignore(ശൂന്യം)
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
    ശൂന്യം
ungetchars(പൂർണ്ണസംഖ്യ len)
{
    എങ്കിൽ (reg_recording == 0)
	തിരികെ;

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
    സ്ഥിതം ശൂന്യം
may_sync_undo(ശൂന്യം)
{
    എങ്കിൽ ((!(State & (MODE_തിരുകുക | MODE_CMDLINE)) || arrow_used)
					       && scriptin[curscript] == ശൂന്യ)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
alloc_typebuf(ശൂന്യം)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    എങ്കിൽ (typebuf.tb_buf == ശൂന്യ || typebuf.tb_noremap == ശൂന്യ)
    {
	free_typebuf();
	തിരികെ FAIL;
    }
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_off = MAXMAPLEN + 4;  // can insert without realloc
    typebuf.tb_len = 0;
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    typebuf.tb_no_abbr_cnt = 0;
    എങ്കിൽ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
#എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    typebuf_was_filled = FALSE;
#endif
    തിരികെ OK;
}

/*
 * Free the buffers of "typebuf".
 */
    സ്ഥിതം ശൂന്യം
free_typebuf(ശൂന്യം)
{
    എങ്കിൽ (typebuf.tb_buf == typebuf_init)
	internal_error("Free typebuf 1");
    അല്ലെങ്കിൽ
	VIM_CLEAR(typebuf.tb_buf);
    എങ്കിൽ (typebuf.tb_noremap == noremapbuf_init)
	internal_error("Free typebuf 2");
    അല്ലെങ്കിൽ
	VIM_CLEAR(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
സ്ഥിതം typebuf_T saved_typebuf[NSCRIPT];

    പൂർണ്ണസംഖ്യ
save_typebuf(ശൂന്യം)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    എങ്കിൽ (alloc_typebuf() == FAIL)
    {
	closescript();
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ old_char = -1;	// character put back by vungetc()
സ്ഥിതം പൂർണ്ണസംഖ്യ old_mod_mask;	// mod_mask for ungotten character
സ്ഥിതം പൂർണ്ണസംഖ്യ old_mouse_row;	// mouse_row related to old_char
സ്ഥിതം പൂർണ്ണസംഖ്യ old_mouse_col;	// mouse_col related to old_char
സ്ഥിതം പൂർണ്ണസംഖ്യ old_താക്കോൽ_Stuffed;	// whether old_char was stuffed

സ്ഥിതം പൂർണ്ണസംഖ്യ can_get_old_char(ശൂന്യം)
{
    // If the old character was not stuffed and characters have been added to
    // the stuff buffer, need to first get the stuffed characters instead.
    തിരികെ old_char != -1 && (old_താക്കോൽ_Stuffed || stuff_empty());
}

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    ശൂന്യം
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    എങ്കിൽ (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_mask = old_mod_mask;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_first.b_next = ശൂന്യ;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_first.b_next = ശൂന്യ;
#ifdef USE_നിവേശം_BUF
    tp->save_inputbuf = get_നിവേശം_buf();
#endif
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    ശൂന്യം
restore_typeahead(tasave_T *tp, പൂർണ്ണസംഖ്യ overwrite UNUSED)
{
    എങ്കിൽ (tp->typebuf_valid)
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
#ifdef USE_നിവേശം_BUF
    set_നിവേശം_buf(tp->save_inputbuf, overwrite);
#endif
}

/*
 * Open a new script file for the ":source!" command.
 */
    ശൂന്യം
openscript(
    char_u	*name,
    പൂർണ്ണസംഖ്യ		directly)	// when TRUE execute directly
{
    എങ്കിൽ (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	തിരികെ;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    എങ്കിൽ (check_secure())
	തിരികെ;

#ifdef FEAT_EVAL
    എങ്കിൽ (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	തിരികെ;
#endif

    എങ്കിൽ (scriptin[curscript] != ശൂന്യ)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    എങ്കിൽ ((scriptin[curscript] = mch_fopen((അക്ഷരം *)NameBuff, READBIN)) == ശൂന്യ)
    {
	semsg(_(e_cant_open_file_str), name);
	എങ്കിൽ (curscript)
	    --curscript;
	തിരികെ;
    }
    എങ്കിൽ (save_typebuf() == FAIL)
	തിരികെ;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    എങ്കിൽ (directly)
    {
	oparg_T	oa;
	പൂർണ്ണസംഖ്യ	oldcurscript;
	പൂർണ്ണസംഖ്യ	save_State = State;
	പൂർണ്ണസംഖ്യ	save_restart_edit = restart_edit;
	പൂർണ്ണസംഖ്യ	save_insertmode = p_im;
	പൂർണ്ണസംഖ്യ	save_finish_op = finish_op;
	പൂർണ്ണസംഖ്യ	save_സന്ദേശം_scroll = സന്ദേശം_scroll;

	State = MODE_NORMAL;
	സന്ദേശം_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	finish_op = FALSE;

	oldcurscript = curscript;
	ചെയ്യുക
	{
	    update_topline_cursor();	// update cursor position and topline
	    normal_cmd(&oa, FALSE);	// execute one command
	    (ശൂന്യം)vpeekc();		// check for end of file
	}
	ആകുമ്പോൾ (scriptin[oldcurscript] != ശൂന്യ);

	State = save_State;
	സന്ദേശം_scroll = save_സന്ദേശം_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	finish_op = save_finish_op;
    }
}

/*
 * Close the currently active input script.
 */
    സ്ഥിതം ശൂന്യം
closescript(ശൂന്യം)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = ശൂന്യ;
    എങ്കിൽ (curscript > 0)
	--curscript;
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
close_all_scripts(ശൂന്യം)
{
    ആകുമ്പോൾ (scriptin[0] != ശൂന്യ)
	closescript();
}
#endif

/*
 * Return TRUE when reading keys from a script file.
 */
    പൂർണ്ണസംഖ്യ
using_script(ശൂന്യം)
{
    തിരികെ scriptin[curscript] != ശൂന്യ;
}

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    ശൂന്യം
before_blocking(ശൂന്യം)
{
    // Tell terminal to flush screen contents before blocking
    term_set_sync_output(TERM_SYNC_OUTPUT_FLUSH);

    updatescript(0);
#ifdef FEAT_EVAL
    എങ്കിൽ (may_garbage_collect)
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
    സ്ഥിതം ശൂന്യം
updatescript(പൂർണ്ണസംഖ്യ c)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    എണ്ണം = 0;

    എങ്കിൽ (c && scriptout)
	putc(c, scriptout);
    എങ്കിൽ (c == 0 || (p_uc > 0 && ++എണ്ണം >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	എണ്ണം = 0;
    }
#ifdef FEAT_EVAL
    എങ്കിൽ (typedchars_pos < MAXMAPLEN)
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
    പൂർണ്ണസംഖ്യ
merge_modify_Other_താക്കോലുകൾ(പൂർണ്ണസംഖ്യ c_arg, പൂർണ്ണസംഖ്യ *modifiers)
{
    പൂർണ്ണസംഖ്യ c = c_arg;

    // CTRL only uses the lower 5 bits of the character.
    എങ്കിൽ (*modifiers & MOD_MASK_CTRL)
    {
	എങ്കിൽ ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	{
	    c &= 0x1f;
	    എങ്കിൽ (c == NUL)
		c = K_ZERO;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#ifdef FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	അല്ലെങ്കിൽ എങ്കിൽ (c == '2')
	    c = NUL;
	അല്ലെങ്കിൽ എങ്കിൽ (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	അല്ലെങ്കിൽ എങ്കിൽ (c == '8')
	    c = BS;
	അല്ലെങ്കിൽ എങ്കിൽ (c == '?')
	    c = DEL;
#endif
	എങ്കിൽ (c != c_arg)
	    *modifiers &= ~MOD_MASK_CTRL;
    }

    // Alt/Meta sets the 8th bit of the character.
    എങ്കിൽ ((*modifiers & (MOD_MASK_META | MOD_MASK_ALT))
	    && c >= 0 && c <= 127)
    {
	// Some terminals (esp. Kitty) do not include Shift in the character.
	// Apply it here to get consistency across terminals.  Only do ASCII
	// letters, for other characters it depends on the keyboard layout.
	എങ്കിൽ ((*modifiers & MOD_MASK_SHIFT) && c >= 'a' && c <= 'z')
	{
	    c += 'a' - 'A';
	    *modifiers &= ~MOD_MASK_SHIFT;
	}
	c += 0x80;
	*modifiers &= ~(MOD_MASK_META | MOD_MASK_ALT);
    }

    തിരികെ c;
}

/*
 * Add a single byte to 'showcmd' for a partially matched mapping.
 * Call add_to_showcmd() if a full key has been received.
 */
    സ്ഥിതം ശൂന്യം
add_byte_to_showcmd(char_u byte)
{
    സ്ഥിതം gotchars_state_T state;
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		modifiers = 0;
    പൂർണ്ണസംഖ്യ		c = NUL;

    എങ്കിൽ (!p_sc || സന്ദേശം_silent != 0)
	തിരികെ;

    എങ്കിൽ (!gotchars_add_byte(&state, byte))
	തിരികെ;

    state.buf[state.buflen] = NUL;
    state.buflen = 0;

    ptr = state.buf;
    എങ്കിൽ (ptr[0] == K_SPECIAL && ptr[1] == KS_MODIFIER && ptr[2] != NUL)
    {
	modifiers = ptr[2];
	ptr += 3;
    }

    എങ്കിൽ (*ptr != NUL)
    {
	char_u		*mb_ptr = mb_unescape(&ptr);

	c = mb_ptr != ശൂന്യ ? (*mb_ptr2char)(mb_ptr) : *ptr++;
	എങ്കിൽ (c <= 0x7f)
	{
	    // Merge modifiers into the key to make the result more readable.
	    പൂർണ്ണസംഖ്യ		modifiers_after = modifiers;
	    പൂർണ്ണസംഖ്യ		mod_c = merge_modify_Other_താക്കോലുകൾ(c, &modifiers_after);

	    എങ്കിൽ (modifiers_after == 0)
	    {
		modifiers = 0;
		c = mod_c;
	    }
	}
    }

    // TODO: is there a more readable and yet compact representation of
    // modifiers and special keys?
    എങ്കിൽ (modifiers != 0)
    {
	add_to_showcmd(K_SPECIAL);
	add_to_showcmd(KS_MODIFIER);
	add_to_showcmd(modifiers);
    }
    എങ്കിൽ (c != NUL)
	add_to_showcmd(c);
    ആകുമ്പോൾ (*ptr != NUL)
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
    പൂർണ്ണസംഖ്യ
vgetc(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		c, c2;
    പൂർണ്ണസംഖ്യ		n;
    char_u	buf[MB_MAXBYTES + 1];
    പൂർണ്ണസംഖ്യ		i;

#ifdef FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    എങ്കിൽ (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#endif

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    എങ്കിൽ (can_get_old_char())
    {
	c = old_char;
	old_char = -1;
	mod_mask = old_mod_mask;
	mouse_row = old_mouse_row;
	mouse_col = old_mouse_col;
    }
    അല്ലെങ്കിൽ
    {
	// number of characters recorded from the last vgetc() call
	സ്ഥിതം size_t	last_vgetc_recorded_len = 0;

	mod_mask = 0;
	vgetc_mod_mask = 0;
	vgetc_char = 0;

	// last_recorded_len can be larger than last_vgetc_recorded_len
	// if peeking records more
	last_recorded_len -= last_vgetc_recorded_len;

	വേണ്ടി (;;)		// this is done twice if there are modifiers
	{
	    പൂർണ്ണസംഖ്യ did_inc = FALSE;

	    // No mapping after modifier has been read, using an input method
	    // and when a popup window has disabled mapping.
	    എങ്കിൽ (mod_mask
#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#endif
#എങ്കിൽ defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#endif
		    )
	    {
		++no_mapping;
		++allow_താക്കോലുകൾ;
		// mod_mask value may change, remember we did the increment
		did_inc = TRUE;
	    }
	    c = vgetorpeek(TRUE);
	    എങ്കിൽ (did_inc)
	    {
		--no_mapping;
		--allow_താക്കോലുകൾ;
	    }

	    // Get two extra bytes for special keys, handle modifiers.
	    എങ്കിൽ (c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	       )
	    {
		പൂർണ്ണസംഖ്യ	    save_allow_താക്കോലുകൾ = allow_താക്കോലുകൾ;

		++no_mapping;
		allow_താക്കോലുകൾ = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_താക്കോലുകൾ = save_allow_താക്കോലുകൾ;
		എങ്കിൽ (c2 == KS_MODIFIER)
		{
		    mod_mask = c;
		    തുടരുക;
		}
		c = TO_SPECIAL(c2, c);

		എങ്കിൽ (allow_osc_താക്കോൽ == 0 && c == K_OSC)
		    തുടരുക;

		// K_ESC is used to avoid ambiguity with the single Esc
		// character that might be the start of an escape sequence.
		// Convert it back to a single Esc here.
		എങ്കിൽ (c == K_ESC)
		    c = ESC;

#എങ്കിൽ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		// Handle K_TEAROFF here, the caller of vgetc() doesn't need to
		// know that a menu was torn off
		എങ്കിൽ (
# ifdef VIMDLL
		    gui.in_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    പൂർണ്ണസംഖ്യ		j;

		    // get menu path, it ends with a <CR>
		    വേണ്ടി (j = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[j] = c;
			എങ്കിൽ (j < 199)
			    ++j;
		    }
		    name[j] = NUL;
		    gui_make_tearoff(name);
		    തുടരുക;
		}
#endif
#എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && !defined(USE_GTK4) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		എങ്കിൽ (c == K_F10 && gui.menubar != ശൂന്യ)
		{
		    gtk_menu_shell_select_first(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    തുടരുക;
		}
#endif
#ifdef FEAT_GUI
		// Handle focus event here, so that the caller doesn't need to
		// know about it.  Return K_IGNORE so that we loop once (needed
		// if 'lazyredraw' is set).
		എങ്കിൽ (c == K_FOCUSGAINED || c == K_FOCUSLOST)
		{
		    ui_focus_change(c == K_FOCUSGAINED);
		    c = K_IGNORE;
		}

		// Translate K_CSI to CSI.  The special key is only used to
		// avoid it being recognized as the start of a special key.
		എങ്കിൽ (c == K_CSI)
		    c = CSI;
#endif
#ifdef FEAT_EVAL
		എങ്കിൽ (c == K_SID)
		{
		    പൂർണ്ണസംഖ്യ	    j;

		    // Handle <SID>{sid};  Do up to 20 digits for safety.
		    last_used_sid = 0;
		    വേണ്ടി (j = 0; j < 20 && SAFE_isdigit(c = vgetorpeek(TRUE)); ++j)
			last_used_sid = last_used_sid * 10 + (c - '0');
		    last_used_map = ശൂന്യ;
		    തുടരുക;
		}
#endif
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    എങ്കിൽ (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		വേണ്ടി (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    എങ്കിൽ (buf[i] == K_SPECIAL
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
			എങ്കിൽ (vgetorpeek(TRUE) == KE_CSI && c == KS_EXTRA)
			    buf[i] = CSI;
		    }
		}
		--no_mapping;
		c = (*mb_ptr2char)(buf);
	    }

	    എങ്കിൽ (vgetc_char == 0)
	    {
		vgetc_mod_mask = mod_mask;
		vgetc_char = c;
	    }

	    // A keypad or special function key was not mapped, use it like
	    // its ASCII equivalent.
	    തിരഞ്ഞെടുപ്പ് (c)
	    {
		അവസ്ഥ K_KPLUS:	c = '+'; നിർത്തുക;
		അവസ്ഥ K_KMINUS:	c = '-'; നിർത്തുക;
		അവസ്ഥ K_KDIVIDE:	c = '/'; നിർത്തുക;
		അവസ്ഥ K_KMULTIPLY: c = '*'; നിർത്തുക;
		അവസ്ഥ K_KENTER:	c = CAR; നിർത്തുക;
		അവസ്ഥ K_KPOINT:
#ifdef MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = Map_Virtual_താക്കോൽ(VK_DECIMAL, 2); നിർത്തുക;
#അല്ലെങ്കിൽ
				c = '.'; നിർത്തുക;
#endif
		അവസ്ഥ K_K0:	c = '0'; നിർത്തുക;
		അവസ്ഥ K_K1:	c = '1'; നിർത്തുക;
		അവസ്ഥ K_K2:	c = '2'; നിർത്തുക;
		അവസ്ഥ K_K3:	c = '3'; നിർത്തുക;
		അവസ്ഥ K_K4:	c = '4'; നിർത്തുക;
		അവസ്ഥ K_K5:	c = '5'; നിർത്തുക;
		അവസ്ഥ K_K6:	c = '6'; നിർത്തുക;
		അവസ്ഥ K_K7:	c = '7'; നിർത്തുക;
		അവസ്ഥ K_K8:	c = '8'; നിർത്തുക;
		അവസ്ഥ K_K9:	c = '9'; നിർത്തുക;

		അവസ്ഥ K_XHOME:
		അവസ്ഥ K_ZHOME:	എങ്കിൽ (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_HOME;
				    mod_mask = 0;
				}
				അല്ലെങ്കിൽ എങ്കിൽ (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_HOME;
				    mod_mask = 0;
				}
				അല്ലെങ്കിൽ
				    c = K_HOME;
				നിർത്തുക;
		അവസ്ഥ K_XEND:
		അവസ്ഥ K_ZEND:	എങ്കിൽ (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_END;
				    mod_mask = 0;
				}
				അല്ലെങ്കിൽ എങ്കിൽ (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_END;
				    mod_mask = 0;
				}
				അല്ലെങ്കിൽ
				    c = K_END;
				നിർത്തുക;

		അവസ്ഥ K_XUP:	c = K_UP; നിർത്തുക;
		അവസ്ഥ K_XDOWN:	c = K_DOWN; നിർത്തുക;
		അവസ്ഥ K_XLEFT:	c = K_ഇടത്; നിർത്തുക;
		അവസ്ഥ K_XRIGHT:	c = K_വലത്; നിർത്തുക;
	    }

	    നിർത്തുക;
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
    എങ്കിൽ (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD
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
    എങ്കിൽ ((!ex_normal_busy || in_feedkeys) && popup_do_filter(c))
    {
	എങ്കിൽ (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#endif

#ifdef FEAT_EVAL
    c = do_താക്കോൽ_നിവേശം_pre(c);

    // Clear the next typedchars_pos
    typedchars_pos = 0;
#endif

    // Need to process the character before we know it's safe to do something
    // else.
    എങ്കിൽ (c != K_IGNORE)
	state_no_longer_safe("key typed");

    തിരികെ c;
}

#ifdef FEAT_EVAL
/*
 * Handle the KeyInputPre autocommand.
 * "c" is the character that was typed.
 * Return new input character.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_താക്കോൽ_നിവേശം_pre(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ		res = c;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    char_u	curr_mode[MODE_MAX_LENGTH];
    പൂർണ്ണസംഖ്യ		save_State = State;
    save_v_event_T save_v_event;
    dict_T	*v_event;

    // Return quickly when there is nothing to do.
    എങ്കിൽ (!has_keyinputpre())
	തിരികെ res;

    എങ്കിൽ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = KEY2TERMCAP0(c);
	buf[2] = KEY2TERMCAP1(c);
	buflen = 3;
    }
    അല്ലെങ്കിൽ
	buflen = (*mb_char2bytes)(c, buf);
    buf[buflen] = NUL;

    typedchars[typedchars_pos] = NUL;
    vim_unescape_csi(typedchars);

    get_mode(curr_mode);

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (പൂർണ്ണസംഖ്യ)buflen);  // set v:char

    v_event = get_v_event(&save_v_event);
    (ശൂന്യം)dict_add_bool(v_event, "typed", താക്കോൽ_Typed);
    (ശൂന്യം)dict_add_string_len(v_event, "typedchar", typedchars, typedchars_pos);

    എങ്കിൽ (apply_autocmds(EVENT_KEYINPUTPRE, curr_mode, curr_mode, FALSE, curbuf))
    {
	string_T    v_char;

	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character.
	v_char.string = get_vim_var_str(VV_CHAR);
	എങ്കിൽ (STRCMP(buf, v_char.string) != 0)
	{
	    v_char.length = STRLEN(v_char.string);
	    // Convert special bytes when it is special string.
	    എങ്കിൽ (v_char.length >= 3 && v_char.string[0] == K_SPECIAL)
		res = TERMCAP2_താക്കോൽ(v_char.string[1], v_char.string[2]);
	    അല്ലെങ്കിൽ എങ്കിൽ (v_char.length > 0)
		res = PTR2CHAR(v_char.string);
	}
    }

    restore_v_event(v_event, &save_v_event);

    set_vim_var_string(VV_CHAR, ശൂന്യ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    തിരികെ res;
}
#endif

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    പൂർണ്ണസംഖ്യ
safe_vgetc(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	c;

    c = vgetc();
    എങ്കിൽ (c == NUL)
	c = get_keystroke();
    തിരികെ c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 * Does not handle bracketed paste - do not use the result for commands.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
plain_vgetc_nopaste(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ c;

    ചെയ്യുക
	c = safe_vgetc();
    ആകുമ്പോൾ (c == K_IGNORE
	    || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
	    || c == K_MOUSEMOVE);
    തിരികെ c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    പൂർണ്ണസംഖ്യ
plain_vgetc(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ c = plain_vgetc_nopaste();

    എങ്കിൽ (c == K_PS)
	// Only handle the first pasted character.  Drop the rest, since we
	// don't know what to do with it.
	c = bracketed_paste(PASTE_ONE_CHAR, FALSE, ശൂന്യ);

    തിരികെ c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 * Returns NUL if no character is available.
 */
    പൂർണ്ണസംഖ്യ
vpeekc(ശൂന്യം)
{
    എങ്കിൽ (can_get_old_char())
	തിരികെ old_char;
    തിരികെ vgetorpeek(FALSE);
}

#എങ്കിൽ defined(FEAT_TERMRESPONSE) || defined(FEAT_TERMINAL)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    പൂർണ്ണസംഖ്യ
vpeekc_nomap(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		c;

    ++no_mapping;
    ++allow_താക്കോലുകൾ;
    c = vpeekc();
    --no_mapping;
    --allow_താക്കോലുകൾ;
    തിരികെ c;
}
#endif

/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    പൂർണ്ണസംഖ്യ
vpeekc_any(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		c;

    c = vpeekc();
    എങ്കിൽ (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    തിരികെ c;
}

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    പൂർണ്ണസംഖ്യ
char_avail(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    retval;

#ifdef FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    എങ്കിൽ (disable_char_avail_for_testing)
	തിരികെ FALSE;
#endif
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    തിരികെ (retval != NUL);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * "getchar()" and "getcharstr()" functions
 */
    സ്ഥിതം ശൂന്യം
getchar_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ allow_സംഖ്യ)
{
    varnumber_T		n = 0;
    പൂർണ്ണസംഖ്യ			called_emsg_start = called_emsg;
    പൂർണ്ണസംഖ്യ			error = FALSE;
    പൂർണ്ണസംഖ്യ			simplify = TRUE;
    char_u		cursor_flag = 'm';

    എങ്കിൽ ((in_vim9script()
		&& check_for_opt_bool_or_സംഖ്യ_arg(argvars, 0) == FAIL)
	    || (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type == VAR_DICT)
    {
	dict_T		*d = argvars[1].vval.v_dict;
	char_u		*cursor_str;

	എങ്കിൽ (allow_സംഖ്യ)
	    allow_സംഖ്യ = dict_get_bool(d, "number", TRUE);
	അല്ലെങ്കിൽ എങ്കിൽ (dict_has_താക്കോൽ(d, "number"))
	    semsg(_(e_invalid_argument_str), "number");

	simplify = dict_get_bool(d, "simplify", TRUE);

	cursor_str = dict_get_string(d, "cursor", FALSE);
	എങ്കിൽ (cursor_str != ശൂന്യ)
	{
	    എങ്കിൽ (STRCMP(cursor_str, "hide") != 0
		    && STRCMP(cursor_str, "keep") != 0
		    && STRCMP(cursor_str, "msg") != 0)
		semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "cursor",
								   cursor_str);
	    അല്ലെങ്കിൽ
		cursor_flag = cursor_str[0];
	}
    }

    എങ്കിൽ (called_emsg != called_emsg_start)
	തിരികെ;

# ifdef MESSAGE_QUEUE
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
# endif

    എങ്കിൽ (cursor_flag == 'h')
	cursor_sleep();
    അല്ലെങ്കിൽ എങ്കിൽ (cursor_flag == 'm')
	windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);

    ++no_mapping;
    ++allow_താക്കോലുകൾ;
    ++allow_osc_താക്കോൽ;
    എങ്കിൽ (!simplify)
	++no_reduce_താക്കോലുകൾ;
    വേണ്ടി (;;)
    {
	എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN
		|| (argvars[0].v_type == VAR_സംഖ്യ
			&& argvars[0].vval.v_സംഖ്യ == -1))
	    // getchar(): blocking wait.
	    n = plain_vgetc_nopaste();
	അല്ലെങ്കിൽ എങ്കിൽ (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	അല്ലെങ്കിൽ എങ്കിൽ (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	അല്ലെങ്കിൽ
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = safe_vgetc();

	എങ്കിൽ (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    തുടരുക;
	നിർത്തുക;
    }
    --no_mapping;
    --allow_താക്കോലുകൾ;
    --allow_osc_താക്കോൽ;
    എങ്കിൽ (!simplify)
	--no_reduce_താക്കോലുകൾ;

    എങ്കിൽ (cursor_flag == 'h')
	cursor_unsleep();

    set_vim_var_nr(VV_MOUSE_WIN, 0);
    set_vim_var_nr(VV_MOUSE_WINID, 0);
    set_vim_var_nr(VV_MOUSE_LNUM, 0);
    set_vim_var_nr(VV_MOUSE_COL, 0);

    എങ്കിൽ (n != 0 && (!allow_സംഖ്യ || IS_SPECIAL(n) || mod_mask != 0))
    {
	char_u		temp[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	പൂർണ്ണസംഖ്യ		i = 0;

	// Turn a special key into three bytes, plus modifier.
	എങ്കിൽ (mod_mask != 0)
	{
	    temp[i++] = K_SPECIAL;
	    temp[i++] = KS_MODIFIER;
	    temp[i++] = mod_mask;
	}
	എങ്കിൽ (IS_SPECIAL(n))
	{
	    temp[i++] = K_SPECIAL;
	    temp[i++] = K_SECOND(n);
	    temp[i++] = K_THIRD(n);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	    i += (*mb_char2bytes)(n, temp + i);
	അല്ലെങ്കിൽ
	    temp[i++] = n;
	temp[i] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strnsave(temp, i);

	എങ്കിൽ (is_mouse_താക്കോൽ(n))
	{
	    പൂർണ്ണസംഖ്യ		row = mouse_row;
	    പൂർണ്ണസംഖ്യ		col = mouse_col;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    പൂർണ്ണസംഖ്യ		winnr = 1;

	    എങ്കിൽ (row >= 0 && col >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_find_win(&row, &col, FIND_POPUP);
		എങ്കിൽ (win == ശൂന്യ)
		    തിരികെ;
		(ശൂന്യം)mouse_comp_pos(win, &row, &col, &lnum, ശൂന്യ);
# ifdef FEAT_PROP_POPUP
		എങ്കിൽ (WIN_IS_POPUP(win))
		    winnr = 0;
		അല്ലെങ്കിൽ
# endif
		    വേണ്ടി (wp = firstwin; wp != win && wp != ശൂന്യ;
							       wp = wp->w_next)
			++winnr;
		set_vim_var_nr(VV_MOUSE_WIN, winnr);
		set_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		set_vim_var_nr(VV_MOUSE_LNUM, lnum);
		set_vim_var_nr(VV_MOUSE_COL, col + 1);
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!allow_സംഖ്യ)
	rettv->v_type = VAR_STRING;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = n;
}

/*
 * "getchar()" function
 */
    ശൂന്യം
f_getchar(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, TRUE);
}

/*
 * "getcharstr()" function
 */
    ശൂന്യം
f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, FALSE);
}

/*
 * "getcharmod()" function
 */
    ശൂന്യം
f_getcharmod(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = mod_mask;
}
#endif // FEAT_EVAL

#എങ്കിൽ defined(MESSAGE_QUEUE)
# define MAX_REPEAT_PARSE 8

/*
 * Process messages that have been queued for netbeans or clientserver.
 * Also check if any jobs have ended.
 * These functions can call arbitrary Vim script and should only be called when
 * it is safe to do so.
 */
    ശൂന്യം
parse_queued_messages(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    old_curwin_id;
    പൂർണ്ണസംഖ്യ	    old_curbuf_fnum;
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    save_may_garbage_collect = may_garbage_collect;
    സ്ഥിതം പൂർണ്ണസംഖ്യ entered = 0;
    പൂർണ്ണസംഖ്യ	    was_safe = get_was_safe_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    എങ്കിൽ (updating_screen || dont_parse_messages)
	തിരികെ;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    എങ്കിൽ (curbuf == ശൂന്യ || curwin == ശൂന്യ)
	തിരികെ;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    വേണ്ടി (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# എങ്കിൽ (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_JOB_CHANNEL)
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
	channel_write_any_വരികൾ();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# എങ്കിൽ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_JOB_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	എങ്കിൽ (job_check_ended())
	    തുടരുക;
# endif
# ifdef FEAT_TERMINAL
	free_unused_terminals();
# endif

# ifdef FEAT_SOUND_MACOSX
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	എങ്കിൽ (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef SIGUSR1
	എങ്കിൽ (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
# endif
	നിർത്തുക;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    എങ്കിൽ (entered == 1 && was_safe)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    എങ്കിൽ (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}
#endif


തരനിർവചനം എണ്ണൽ {
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
at_ins_compl_താക്കോൽ(ശൂന്യം)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    പൂർണ്ണസംഖ്യ	    c = *p;

    എങ്കിൽ (typebuf.tb_len > 3
	    && (c == K_SPECIAL || c == CSI)  // CSI is used by the GUI
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_MASK_CTRL)
	    // CTRL-SHIFT-N/P scroll the info popup, so they must not be folded
	    // to the CTRL-N/CTRL-P completion keys here.
	    && !(p[2] & MOD_MASK_SHIFT))
	c = p[3] & 0x1f;
    തിരികെ !ctrl_x_mode_eval()
	    && ((ctrl_x_mode_not_default() && vim_is_ctrl_x_താക്കോൽ(c))
		|| (compl_status_local() && (c == Ctrl_N || c == Ctrl_P)));
}

/*
 * Check if typebuf.tb_buf[] contains a modifier plus key that can be changed
 * into just a key, apply that.
 * Check from typebuf.tb_buf[typebuf.tb_off] to typebuf.tb_buf[typebuf.tb_off
 * + "max_offset"].
 * Return the length of the replaced bytes, 0 if nothing changed, -1 for error.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_simplify_modifier(പൂർണ്ണസംഖ്യ max_offset)
{
    പൂർണ്ണസംഖ്യ		offset;
    char_u	*tp;

    വേണ്ടി (offset = 0; offset < max_offset; ++offset)
    {
	എങ്കിൽ (offset + 3 >= typebuf.tb_len)
	    നിർത്തുക;
	tp = typebuf.tb_buf + typebuf.tb_off + offset;
	എങ്കിൽ ((tp[0] == K_SPECIAL || tp[0] == CSI) && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    പൂർണ്ണസംഖ്യ modifier = tp[2];
	    പൂർണ്ണസംഖ്യ	c = tp[3];
	    പൂർണ്ണസംഖ്യ new_c = merge_modify_Other_താക്കോലുകൾ(c, &modifier);

	    എങ്കിൽ (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		പൂർണ്ണസംഖ്യ	len;
		പൂർണ്ണസംഖ്യ	താക്കോൽ_offset = offset;

		എങ്കിൽ (offset == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_mask = tp[2];
		}
		എങ്കിൽ (IS_SPECIAL(new_c))
		{
		    new_string[0] = K_SPECIAL;
		    new_string[1] = K_SECOND(new_c);
		    new_string[2] = K_THIRD(new_c);
		    len = 3;
		}
		അല്ലെങ്കിൽ
		    len = mb_char2bytes(new_c, new_string);
		എങ്കിൽ (modifier == 0)
		{
		    എങ്കിൽ (put_string_in_typebuf(offset, 4, new_string, len,
							ശൂന്യ, 0, ശൂന്യ) == FAIL)
			തിരികെ -1;
		}
		അല്ലെങ്കിൽ
		{
		    tp[2] = modifier;
		    താക്കോൽ_offset = offset + 3;
		    എങ്കിൽ (put_string_in_typebuf(താക്കോൽ_offset, 1, new_string, len,
							ശൂന്യ, 0, ശൂന്യ) == FAIL)
			തിരികെ -1;
		}

		// A mapping for the simplified key can be used for it even
		// when the key protocol is enabled.
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len; ++i)
		    typebuf.tb_noremap[typebuf.tb_off + താക്കോൽ_offset + i]
							    |= RM_SIMPLIFIED;
		തിരികെ len;
	    }
	}
    }
    തിരികെ 0;
}

/*
 * Return TRUE if the terminal sends modifiers with various keys.  This is when
 * modifyOtherKeys level 2 is enabled or the kitty keyboard protocol is
 * enabled.
 */
    പൂർണ്ണസംഖ്യ
താക്കോൽ_protocol_enabled(ശൂന്യം)
{
    // If xterm has responded to XTQMODKEYS it overrules seenModifyOtherKeys.
    പൂർണ്ണസംഖ്യ using_mok = modify_otherkeys_state != MOKS_INITIAL
			? modify_otherkeys_state == MOKS_ENABLED
			: seen_Modify_Other_താക്കോലുകൾ;
    തിരികെ using_mok || kitty_protocol_state == KKPS_ENABLED;
}

/*
 * Handle mappings in the typeahead buffer.
 * - When something was mapped, return map_result_retry for recursive mappings.
 * - When nothing mapped and typeahead has a character: return map_result_get.
 * - When there is no match yet, return map_result_nomatch, need to get more
 *   typeahead.
 * - On failure (out of memory) return map_result_fail.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_mapping(
	    പൂർണ്ണസംഖ്യ *keylenp,
	    പൂർണ്ണസംഖ്യ *timedout,
	    പൂർണ്ണസംഖ്യ *mapdepth)
{
    mapblock_T	*mp = ശൂന്യ;
    mapblock_T	*mp2;
    mapblock_T	*mp_match;
    പൂർണ്ണസംഖ്യ		mp_match_len = 0;
    പൂർണ്ണസംഖ്യ		max_mlen = 0;
    പൂർണ്ണസംഖ്യ		want_termcode = 0;  // 1 if termcode expected after max_mlen
    പൂർണ്ണസംഖ്യ		tb_c1;
    പൂർണ്ണസംഖ്യ		mlen;
#ifdef FEAT_LANGMAP
    പൂർണ്ണസംഖ്യ		nolmaplen;
#endif
    പൂർണ്ണസംഖ്യ		keylen = *keylenp;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		local_State = get_real_state();
    പൂർണ്ണസംഖ്യ		is_plug_map = FALSE;
    bool	in_osc = in_osc_sequence(); // If we are in an OSC sequence,
					    // then go straight to
					    // check_termcode() in order to
					    // consume chars.

    // If typeahead starts with <Plug> then remap, even for a "noremap" mapping.
    എങ്കിൽ (!in_osc && typebuf.tb_len >= 3
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
    എങ്കിൽ (!in_osc && no_mapping == 0 && is_maphash_valid()
	    && (no_zero_mapping == 0 || tb_c1 != '0')
	    && (typebuf.tb_maplen == 0 || is_plug_map
		|| (p_remap
		    && (typebuf.tb_noremap[typebuf.tb_off]
				    & (RM_NONE|RM_ABBR)) == 0))
	    && !(p_paste && (State & (MODE_തിരുകുക | MODE_CMDLINE)))
	    && !(State == MODE_HITRETURN && (tb_c1 == CAR || tb_c1 == ' '))
	    && State != MODE_ASKMORE
	    && State != MODE_CONFIRM
	    && !at_ins_compl_താക്കോൽ())
    {
#ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#endif
#ifdef FEAT_LANGMAP
	എങ്കിൽ (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	അല്ലെങ്കിൽ
	{
	    LANGMAP_ADJUST(tb_c1, (State & (MODE_CMDLINE | MODE_തിരുകുക)) == 0
					   && get_real_state() != MODE_SELECT);
	    nolmaplen = 0;
	}
#endif
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	എങ്കിൽ (mp == ശൂന്യ)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = ശൂന്യ;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_match = ശൂന്യ;
	mp_match_len = 0;
	വേണ്ടി ( ; mp != ശൂന്യ;
	       mp->m_next == ശൂന്യ ? (mp = mp2, mp2 = ശൂന്യ) : (mp = mp->m_next))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    എങ്കിൽ (mp->m_താക്കോലുകൾ[0] == tb_c1
		    && (mp->m_mode & local_State)
		    && !(mp->m_simplified && താക്കോൽ_protocol_enabled()
			    && typebuf.tb_maplen == 0
			    && (typebuf.tb_noremap[typebuf.tb_off]
							& RM_SIMPLIFIED) == 0)
		    && ((mp->m_mode & MODE_LANGMAP) == 0
						    || typebuf.tb_maplen == 0))
	    {
#ifdef FEAT_LANGMAP
		പൂർണ്ണസംഖ്യ	nomap = nolmaplen;
		പൂർണ്ണസംഖ്യ	modifiers = 0;
#endif
		// find the match length of this mapping
		വേണ്ടി (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
		    പൂർണ്ണസംഖ്യ	c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
#ifdef FEAT_LANGMAP
		    എങ്കിൽ (nomap > 0)
		    {
			എങ്കിൽ (nomap == 2 && c2 == KS_MODIFIER)
			    modifiers = 1;
			അല്ലെങ്കിൽ എങ്കിൽ (nomap == 1 && modifiers == 1)
			    modifiers = c2;
			--nomap;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (c2 == K_SPECIAL)
			    nomap = 2;
			അല്ലെങ്കിൽ എങ്കിൽ (merge_modify_Other_താക്കോലുകൾ(c2, &modifiers) == c2)
			    // Only apply 'langmap' if merging modifiers into
			    // the key will not result in another character,
			    // so that 'langmap' behaves consistently in
			    // different terminals and GUIs.
			    LANGMAP_ADJUST(c2, TRUE);
			modifiers = 0;
		    }
#endif
		    എങ്കിൽ (mp->m_താക്കോലുകൾ[mlen] != c2)
			നിർത്തുക;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_താക്കോലുകൾ;
		    char_u *p2 = mb_unescape(&p1);

		    എങ്കിൽ (has_mbyte && p2 != ശൂന്യ
					&& MB_BYTE2LEN(tb_c1) > mb_ptr2len(p2))
			mlen = 0;
		}

		// Check an entry whether it matches.
		// - Full match: mlen == keylen
		// - Partly match: mlen == typebuf.tb_len
		keylen = mp->m_keylen;
		എങ്കിൽ (mlen == keylen || (mlen == typebuf.tb_len
						   && typebuf.tb_len < keylen))
		{
		    char_u  *s;
		    പൂർണ്ണസംഖ്യ	    n;

		    // If only script-local mappings are allowed, check if the
		    // mapping starts with K_SNR.
		    s = typebuf.tb_noremap + typebuf.tb_off;
		    എങ്കിൽ ((*s & ~RM_SIMPLIFIED) == RM_SCRIPT
			    && (mp->m_താക്കോലുകൾ[0] != K_SPECIAL
				|| mp->m_താക്കോലുകൾ[1] != KS_EXTRA
				|| mp->m_താക്കോലുകൾ[2] != KE_SNR))
			തുടരുക;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    വേണ്ടി (n = mlen; --n >= 0; )
			എങ്കിൽ (*s++ & (RM_NONE|RM_ABBR))
			    നിർത്തുക;
		    എങ്കിൽ (!is_plug_map && n >= 0)
			തുടരുക;

		    എങ്കിൽ (keylen > typebuf.tb_len)
		    {
			എങ്കിൽ (!*timedout && !(mp_match != ശൂന്യ
							&& mp_match->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    നിർത്തുക;
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (keylen > mp_match_len)
		    {
			// found a longer match
			mp_match = mp;
			mp_match_len = keylen;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // No match; may have to check for termcode at next
		    // character.

		    // If the first character that didn't match is
		    // K_SPECIAL then check for a termcode.  This isn't perfect
		    // but should work in most cases.
		    എങ്കിൽ (max_mlen < mlen)
		    {
			max_mlen = mlen;
			want_termcode = mp->m_താക്കോലുകൾ[mlen] == K_SPECIAL;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (max_mlen == mlen && mp->m_താക്കോലുകൾ[mlen] == K_SPECIAL)
			want_termcode = 1;

		    // Check termcode for uppercase character to properly
		    // process "ESC[27;2;<ascii code>~" control sequences.
		    എങ്കിൽ (ASCII_ISUPPER(mp->m_താക്കോലുകൾ[mlen]))
			want_termcode = 1;
		}
	    }
	}

	// If no partly match found, use the longest full match.
	എങ്കിൽ (keylen != KEYLEN_PART_MAP && mp_match != ശൂന്യ)
	{
	    mp = mp_match;
	    keylen = mp_match_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    എങ്കിൽ (!in_osc && *p_pt != NUL && mp == ശൂന്യ &&
	    (State & (MODE_തിരുകുക | MODE_NORMAL)))
    {
	വേണ്ടി (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    എങ്കിൽ (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    നിർത്തുക;
	എങ്കിൽ (p_pt[mlen] == NUL)	// match
	{
	    // write chars to script file(s)
	    എങ്കിൽ (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    set_option_മൂല്യം_give_err((char_u *)"paste",
						      (long)!p_paste, ശൂന്യ, 0);
	    എങ്കിൽ (!(State & MODE_തിരുകുക))
	    {
		സന്ദേശം_col = 0;
		സന്ദേശം_row = Rows - 1;
		സന്ദേശം_clr_eos();		// clear ruler
	    }
	    status_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    തിരികെ map_result_retry;
	}
	// Need more chars for partly match.
	എങ്കിൽ (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_താക്കോൽ;
	അല്ലെങ്കിൽ എങ്കിൽ (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    // May check for a terminal code when there is no mapping or only a partial
    // mapping.  Also check if there is a full mapping with <Esc>, unless timed
    // out, since that is nearly always a partial match with a terminal code.
    എങ്കിൽ (in_osc || ((mp == ശൂന്യ || max_mlen + want_termcode > mp_match_len
		    || (mp_match_len == 1 && *mp->m_താക്കോലുകൾ == ESC && !*timedout))
	    && keylen != KEYLEN_PART_MAP))
    {
	പൂർണ്ണസംഖ്യ	save_keylen = keylen;

	/*
	 * When no matching mapping found or found a non-matching mapping that
	 * matches at least what the matching mapping matched:
	 * Check if we have a terminal code, when:
	 * - mapping is allowed,
	 * - keys have not been mapped,
	 * - and not an ESC sequence, not in insert mode or p_ek is on,
	 * - and when not timed out,
	 */
	എങ്കിൽ (in_osc || no_mapping == 0 || allow_താക്കോലുകൾ != 0)
	{
	    എങ്കിൽ (in_osc || ((typebuf.tb_maplen == 0
			    || (p_remap && (typebuf.tb_noremap[typebuf.tb_off]
						& ~RM_SIMPLIFIED) == RM_YES))
		    && !*timedout))
		keylen = check_termcode(max_mlen + 1, ശൂന്യ, 0, ശൂന്യ);
	    അല്ലെങ്കിൽ
		keylen = 0;

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    എങ്കിൽ (keylen == 0 && save_keylen == KEYLEN_PART_താക്കോൽ && !*timedout)
		keylen = KEYLEN_PART_താക്കോൽ;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
#ifdef FEAT_TERMINAL
	    check_no_reduce_താക്കോലുകൾ();  // may update the no_reduce_keys flag
#endif
	    എങ്കിൽ (keylen == 0 && !no_reduce_താക്കോലുകൾ)
	    {
		keylen = check_simplify_modifier(max_mlen + 1);
		എങ്കിൽ (keylen < 0)
		    // ins_typebuf() failed
		    തിരികെ map_result_fail;
	    }

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    എങ്കിൽ (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	അല്ലെങ്കിൽ
	    keylen = 0;
	എങ്കിൽ (keylen == 0)	// no matching terminal code
	{
#ifdef AMIGA
	    // check for window bounds report
	    എങ്കിൽ (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		വേണ്ടി (s = typebuf.tb_buf + typebuf.tb_off + 1;
			   s < typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len
		   && (VIM_ISDIGIT(*s) || *s == ';' || *s == ' ');
			++s)
		    ;
		എങ്കിൽ (*s == 'r' || *s == '|') // found one
		{
		    del_typebuf(
			  (പൂർണ്ണസംഖ്യ)(s + 1 - (typebuf.tb_buf + typebuf.tb_off)), 0);
		    // get size and redraw screen
		    shell_resized();
		    *keylenp = keylen;
		    തിരികെ map_result_retry;
		}
		എങ്കിൽ (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_താക്കോൽ;
	    }
	    എങ്കിൽ (keylen >= 0)
#endif
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		എങ്കിൽ (mp == ശൂന്യ)
		{
		    *keylenp = keylen;
		    തിരികെ map_result_get;    // get character from typeahead
		}
	}

	എങ്കിൽ (keylen > 0)	    // full matching terminal code
	{
#എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_MENU)
	    എങ്കിൽ (typebuf.tb_len >= 2
		    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
			      && typebuf.tb_buf[typebuf.tb_off + 1] == KS_MENU)
	    {
		പൂർണ്ണസംഖ്യ	idx;

		// Using a menu may cause a break in undo!  It's like using
		// gotchars(), but without recording or writing to a script
		// file.
		may_sync_undo();
		del_typebuf(3, 0);
		idx = get_menu_index(current_menu, local_State);
		എങ്കിൽ (idx != MENU_INDEX_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    എങ്കിൽ (VIsual_active && VIsual_select
					&& (current_menu->modes & MODE_VISUAL))
		    {
			VIsual_select = FALSE;
			(ശൂന്യം)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->strings[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#endif // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    തിരികെ map_result_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	എങ്കിൽ (mp == ശൂന്യ || keylen < 0)
	    keylen = KEYLEN_PART_താക്കോൽ;
	അല്ലെങ്കിൽ
	    keylen = mp_match_len;
    }

    /*
     * complete match
     */
    എങ്കിൽ (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#ifdef FEAT_EVAL
	പൂർണ്ണസംഖ്യ	save_m_വ്യഞ്ജകം;
	പൂർണ്ണസംഖ്യ	save_m_noremap;
	പൂർണ്ണസംഖ്യ	save_m_silent;
	char_u	*save_m_താക്കോലുകൾ;
	char_u	*save_alt_m_താക്കോലുകൾ;
	പൂർണ്ണസംഖ്യ	save_alt_m_keylen;
#അല്ലെങ്കിൽ
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#endif

	// write chars to script file(s)
	എങ്കിൽ (keylen > typebuf.tb_maplen)
	    gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						   keylen - typebuf.tb_maplen);

	cmd_silent = (typebuf.tb_silent > 0);
	del_typebuf(keylen, 0);	// remove the mapped keys

	/*
	 * Put the replacement string in front of mapstr.
	 * The depth check catches ":map x y" and ":map y x".
	 */
	എങ്കിൽ (++*mapdepth >= p_mmd)
	{
	    emsg(_(e_recursive_mapping));
	    എങ്കിൽ (State & MODE_CMDLINE)
		redrawcmdline();
	    അല്ലെങ്കിൽ
		setcursor();
	    flush_buffers(FLUSH_MINIMAL);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    തിരികെ map_result_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	എങ്കിൽ (VIsual_active && VIsual_select && (mp->m_mode & MODE_VISUAL))
	{
	    VIsual_select = FALSE;
	    (ശൂന്യം)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#ifdef FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_വ്യഞ്ജകം = mp->m_വ്യഞ്ജകം;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_താക്കോലുകൾ = ശൂന്യ;  // only saved when needed
	save_alt_m_താക്കോലുകൾ = ശൂന്യ;  // only saved when needed
	save_alt_m_keylen = mp->m_alt != ശൂന്യ ? mp->m_alt->m_keylen : 0;

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	എങ്കിൽ (mp->m_വ്യഞ്ജകം)
	{
	    പൂർണ്ണസംഖ്യ save_vgetc_busy = vgetc_busy;
	    പൂർണ്ണസംഖ്യ save_may_garbage_collect = may_garbage_collect;
	    പൂർണ്ണസംഖ്യ was_screen_col = screen_cur_col;
	    പൂർണ്ണസംഖ്യ was_screen_row = screen_cur_row;
	    പൂർണ്ണസംഖ്യ prev_did_emsg = did_emsg;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_താക്കോലുകൾ = vim_strnsave(mp->m_താക്കോലുകൾ, (size_t)mp->m_keylen);
	    save_alt_m_താക്കോലുകൾ = mp->m_alt != ശൂന്യ
				    ? vim_strnsave(mp->m_alt->m_താക്കോലുകൾ,
					     (size_t)save_alt_m_keylen) : ശൂന്യ;
	    map_str = eval_map_വ്യഞ്ജകം(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_row, was_screen_col);
	    out_flush();

	    // If an error was displayed and the expression returns an empty
	    // string, generate a <Nop> to allow for a redraw.
	    എങ്കിൽ (prev_did_emsg != did_emsg
				       && (map_str == ശൂന്യ || *map_str == NUL))
	    {
		char_u	buf[4];

		vim_free(map_str);
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = KE_IGNORE;
		buf[3] = NUL;
		map_str = vim_strnsave(buf, 3);
		എങ്കിൽ (State & MODE_CMDLINE)
		{
		    // redraw the command below the error
		    സന്ദേശം_didout = TRUE;
		    എങ്കിൽ (സന്ദേശം_row < cmdline_row)
			സന്ദേശം_row = cmdline_row;
		    redrawcmd();
		}
	    }

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	അല്ലെങ്കിൽ
#endif
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	എങ്കിൽ (map_str == ശൂന്യ)
	    i = FAIL;
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ noremap;

#ifdef FEAT_EVAL
	    last_used_map = mp;
	    last_used_sid = -1;
#endif
	    എങ്കിൽ (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    അല്ലെങ്കിൽ എങ്കിൽ (
#ifdef FEAT_EVAL
		save_m_വ്യഞ്ജകം ?
		(save_m_താക്കോലുകൾ != ശൂന്യ
			&& STRNCMP(map_str, save_m_താക്കോലുകൾ, (size_t)keylen) == 0)
		|| (save_alt_m_താക്കോലുകൾ != ശൂന്യ
			&& STRNCMP(map_str, save_alt_m_താക്കോലുകൾ,
					    (size_t)save_alt_m_keylen) == 0) :
#endif
		STRNCMP(map_str, mp->m_താക്കോലുകൾ, (size_t)keylen) == 0
		|| (mp->m_alt != ശൂന്യ
			&& STRNCMP(map_str, mp->m_alt->m_താക്കോലുകൾ,
					    (size_t)mp->m_alt->m_keylen) == 0))
		noremap = REMAP_SKIP;
	    അല്ലെങ്കിൽ
		noremap = REMAP_YES;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#ifdef FEAT_EVAL
	    എങ്കിൽ (save_m_വ്യഞ്ജകം)
		vim_free(map_str);
#endif
	}
#ifdef FEAT_EVAL
	vim_free(save_m_താക്കോലുകൾ);
	vim_free(save_alt_m_താക്കോലുകൾ);
#endif
	*keylenp = keylen;
	എങ്കിൽ (i == FAIL)
	    തിരികെ map_result_fail;
	തിരികെ map_result_retry;
    }

    *keylenp = keylen;
    തിരികെ map_result_nomatch;
}

/*
 * unget one character (can only be done once!)
 * If the character was stuffed, vgetc() will get it next time it is called.
 * Otherwise vgetc() will only get it when the stuff buffer is empty.
 */
    ശൂന്യം
vungetc(പൂർണ്ണസംഖ്യ c)
{
    old_char = c;
    old_mod_mask = mod_mask;
    old_mouse_row = mouse_row;
    old_mouse_col = mouse_col;
    old_താക്കോൽ_Stuffed = താക്കോൽ_Stuffed;
}

/*
 * When peeking and not getting a character, reg_executing cannot be cleared
 * yet, so set a flag to clear it later.
 */
    സ്ഥിതം ശൂന്യം
check_end_reg_executing(പൂർണ്ണസംഖ്യ advance)
{
    എങ്കിൽ (reg_executing != 0 && (typebuf.tb_maplen == 0
						|| pending_end_reg_executing))
    {
	എങ്കിൽ (advance)
	{
	    reg_executing = 0;
	    pending_end_reg_executing = FALSE;
	}
	അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vgetorpeek(പൂർണ്ണസംഖ്യ advance)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		timedout = FALSE;	// waited for more than 'timeoutlen'
					// for mapping to complete or
					// 'ttimeoutlen' for complete key code
    പൂർണ്ണസംഖ്യ		mapdepth = 0;		// check for recursive mapping
    പൂർണ്ണസംഖ്യ		mode_deleted = FALSE;   // set when mode has been deleted
    പൂർണ്ണസംഖ്യ		new_wcol, new_wrow;
#ifdef FEAT_GUI
    പൂർണ്ണസംഖ്യ		shape_changed = FALSE;  // adjusted cursor shape
#endif
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		old_wcol, old_wrow;
    പൂർണ്ണസംഖ്യ		wait_tb_len;

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
    എങ്കിൽ (vgetc_busy > 0 && ex_normal_busy == 0)
	തിരികെ NUL;

    ++vgetc_busy;

    എങ്കിൽ (advance)
    {
	താക്കോൽ_Stuffed = FALSE;
	typebuf_was_empty = FALSE;
    }

    init_typebuf();
    start_stuff();
    check_end_reg_executing(advance);
    ചെയ്യുക
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	എങ്കിൽ (typeahead_char != 0)
	{
	    c = typeahead_char;
	    എങ്കിൽ (advance)
		typeahead_char = 0;
	}
	അല്ലെങ്കിൽ
	    c = read_readbuffers(advance);
	എങ്കിൽ (c != NUL && !got_int)
	{
	    എങ്കിൽ (advance)
	    {
		// KeyTyped = FALSE;  When the command that stuffed something
		// was typed, behave like the stuffed command was typed.
		// needed for CTRL-W CTRL-] to open a fold, for example.
		താക്കോൽ_Stuffed = TRUE;
	    }
	    എങ്കിൽ (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	// no abbreviations now
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    വേണ്ടി (;;)
	    {
		long	wait_time;
		പൂർണ്ണസംഖ്യ	keylen = 0;
		പൂർണ്ണസംഖ്യ	showcmd_idx;
		check_end_reg_executing(advance);
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		എങ്കിൽ (typebuf.tb_maplen)
		    വരി_breakcheck();
		അല്ലെങ്കിൽ
		    ui_breakcheck();		// check for CTRL-C
		എങ്കിൽ (got_int)
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
		    എങ്കിൽ ((c || typebuf.tb_maplen)
				     && (State & (MODE_തിരുകുക | MODE_CMDLINE)))
			c = ESC;
		    അല്ലെങ്കിൽ
			c = Ctrl_C;
		    flush_buffers(FLUSH_നിവേശം);	// flush all typeahead

		    എങ്കിൽ (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    നിർത്തുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_result_T result = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    എങ്കിൽ (result == map_result_retry)
			// try mapping again
			തുടരുക;
		    എങ്കിൽ (result == map_result_fail)
		    {
			// failed, use the outer loop
			c = -1;
			നിർത്തുക;
		    }
		    എങ്കിൽ (result == map_result_get)
		    {
/*
 * get a character: 2. from the typeahead buffer
 */
			c = typebuf.tb_buf[typebuf.tb_off];
			എങ്കിൽ (advance)	// remove chars from typebuf
			{
			    cmd_silent = (typebuf.tb_silent > 0);
			    എങ്കിൽ (typebuf.tb_maplen > 0)
				താക്കോൽ_Typed = FALSE;
			    അല്ലെങ്കിൽ
			    {
				താക്കോൽ_Typed = TRUE;
				// write char to script file(s)
				gotchars(typebuf.tb_buf
						 + typebuf.tb_off, 1);
			    }
			    താക്കോൽ_Noremap = typebuf.tb_noremap[typebuf.tb_off]
							    & ~RM_SIMPLIFIED;
			    del_typebuf(1, 0);
			}
			നിർത്തുക;  // got character, break the for loop
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
		എങ്കിൽ (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& kitty_protocol_state != KKPS_ENABLED
			&& ex_normal_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & MODE_തിരുകുക)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_താക്കോൽ && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	col = 0;
		    char_u	*ptr;

		    എങ്കിൽ (mode_displayed)
		    {
			unshowmode(TRUE);
			mode_deleted = TRUE;
		    }
#ifdef FEAT_GUI
		    // may show a different cursor shape
		    എങ്കിൽ (gui.in_use && State != MODE_NORMAL && !cmd_silent)
		    {
			പൂർണ്ണസംഖ്യ	    save_State;

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
		    എങ്കിൽ (curwin->w_cursor.col != 0)
		    {
			എങ്കിൽ (curwin->w_wcol > 0)
			{
			    // After auto-indenting and no text is following,
			    // we are expecting to truncate the trailing
			    // white-space, so find the last non-white
			    // character -- webb
			    എങ്കിൽ (did_ai && *skipwhite(ml_get_curline()
						+ curwin->w_cursor.col) == NUL)
			    {
				chartabsize_T cts;

				curwin->w_wcol = 0;
				ptr = ml_get_curline();
				init_chartabsize_arg(&cts, curwin,
					  curwin->w_cursor.lnum, 0, ptr, ptr);
				ആകുമ്പോൾ (cts.cts_ptr < ptr + curwin->w_cursor.col)
				{
				    എങ്കിൽ (!VIM_ISWHITE(*cts.cts_ptr))
					curwin->w_wcol = cts.cts_vcol;
				    cts.cts_vcol += lbr_chartabsize(&cts);
				    എങ്കിൽ (has_mbyte)
					cts.cts_ptr +=
						   (*mb_ptr2len)(cts.cts_ptr);
				    അല്ലെങ്കിൽ
					++cts.cts_ptr;
				}
				clear_chartabsize_arg(&cts);

				curwin->w_wrow = curwin->w_cline_row
					   + curwin->w_wcol / curwin->w_width;
				curwin->w_wcol %= curwin->w_width;
				curwin->w_wcol += curwin_col_off();
				col = 0;	// no correction needed
			    }
			    അല്ലെങ്കിൽ
			    {
				--curwin->w_wcol;
				col = curwin->w_cursor.col - 1;
			    }
			}
			അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_width - 1;
			    col = curwin->w_cursor.col - 1;
			}
			എങ്കിൽ (has_mbyte && col > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    col -= (*mb_head_off)(ptr, ptr + col);
			    എങ്കിൽ ((*mb_ptr2cells)(ptr + col) > 1)
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
		എങ്കിൽ (c < 0)
		    തുടരുക;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		വേണ്ടി (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		എങ്കിൽ (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    തുടരുക;
		}

		എങ്കിൽ (ex_normal_busy > 0)
		{
		    സ്ഥിതം പൂർണ്ണസംഖ്യ tc = 0;

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    എങ്കിൽ (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			തുടരുക;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    എങ്കിൽ (p_im && (State & MODE_തിരുകുക))
			c = Ctrl_L;
#ifdef FEAT_TERMINAL
		    അല്ലെങ്കിൽ എങ്കിൽ (terminal_is_active())
			c = K_CANCEL;
#endif
		    അല്ലെങ്കിൽ എങ്കിൽ ((State & MODE_CMDLINE)
					     || (cmdwin_type > 0 && tc == ESC))
			c = Ctrl_C;
		    അല്ലെങ്കിൽ
			c = ESC;
		    tc = c;
		    // set a flag to indicate this wasn't a normal char
		    എങ്കിൽ (advance)
			typebuf_was_empty = TRUE;

		    // return from main_loop()
		    എങ്കിൽ (pending_exmode_active)
			exmode_active = EXMODE_NORMAL;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    നിർത്തുക;
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
		എങ്കിൽ (((State & MODE_തിരുകുക) != 0 || p_lz)
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
		പൂർണ്ണസംഖ്യ showing_partial = FALSE;
		എങ്കിൽ (typebuf.tb_len > 0 && advance && !exmode_active)
		{
		    എങ്കിൽ (((State & (MODE_NORMAL | MODE_തിരുകുക))
						      || State == MODE_LANGMAP)
			    && State != MODE_HITRETURN)
		    {
			// this looks nice when typing a dead character map
			എങ്കിൽ (State & MODE_തിരുകുക
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
			തള്ളുക_showcmd();
			എങ്കിൽ (typebuf.tb_len > SHOWCMD_COLS)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_COLS;
			ആകുമ്പോൾ (showcmd_idx < typebuf.tb_len)
			    add_byte_to_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
		    }

		    // This looks nice when typing a dead character map.
		    // There is no actual command line for get_number().
		    എങ്കിൽ ((State & MODE_CMDLINE)
			    && get_cmdline_info()->cmdbuff != ശൂന്യ
#എങ്കിൽ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
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
		എങ്കിൽ (typebuf.tb_len == 0)
		    // timedout may have been set if a mapping with empty RHS
		    // fully matched while longer mappings timed out.
		    timedout = FALSE;

		എങ്കിൽ (advance)
		{
		    എങ്കിൽ (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_താക്കോൽ)))
			// blocking wait
			wait_time = -1L;
		    അല്ലെങ്കിൽ എങ്കിൽ (keylen == KEYLEN_PART_താക്കോൽ && p_ttm >= 0)
			wait_time = p_ttm;
		    അല്ലെങ്കിൽ
			wait_time = p_tm;
		}
		അല്ലെങ്കിൽ
		    wait_time = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_time);

		എങ്കിൽ (showcmd_idx != 0)
		    എടുക്കുക_showcmd();
		എങ്കിൽ (showing_partial)
		{
		    എങ്കിൽ (State & MODE_തിരുകുക)
			edit_unputchar();
		    എങ്കിൽ ((State & MODE_CMDLINE)
					&& get_cmdline_info()->cmdbuff != ശൂന്യ)
			unputcmdline();
		    അല്ലെങ്കിൽ
			setcursor();	// put cursor back where it belongs
		}

		എങ്കിൽ (c < 0)
		    തുടരുക;		// end of input script reached
		എങ്കിൽ (c == NUL)		// no character available
		{
		    എങ്കിൽ (!advance)
			നിർത്തുക;
		    എങ്കിൽ (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			തുടരുക;
		    }
		}
		അല്ലെങ്കിൽ
		{	    // allow mapping for just typed characters
		    ആകുമ്പോൾ (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#ifdef HAVE_നിവേശം_METHOD
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_active = im_get_status();
#endif
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } ആകുമ്പോൾ ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    എങ്കിൽ (advance && p_smd && സന്ദേശം_silent == 0 && (State & MODE_തിരുകുക))
    {
	എങ്കിൽ (c == ESC && !mode_deleted && !no_mapping && mode_displayed)
	{
	    എങ്കിൽ (typebuf.tb_len && !താക്കോൽ_Typed)
		redraw_cmdline = TRUE;	    // delete mode later
	    അല്ലെങ്കിൽ
		unshowmode(FALSE);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c != ESC && mode_deleted)
	{
	    എങ്കിൽ (typebuf.tb_len && !താക്കോൽ_Typed)
		redraw_cmdline = TRUE;	    // show mode later
	    അല്ലെങ്കിൽ
		showmode();
	}
    }
#ifdef FEAT_GUI
    // may unshow different cursor shape
    എങ്കിൽ (gui.in_use && shape_changed)
	gui_update_cursor(TRUE, FALSE);
#endif
    എങ്കിൽ (timedout && c == ESC)
    {
	// When recording there will be no timeout.  Add an <Ignore> after the
	// ESC to avoid that it forms a key code with following characters.
	gotchars_ignore();
    }

    --vgetc_busy;

    തിരികെ c;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
inchar(
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		maxlen,
    long	wait_time)	    // milliseconds
{
    പൂർണ്ണസംഖ്യ		len = 0;	    // init for GCC
    പൂർണ്ണസംഖ്യ		retesc = FALSE;	    // return ESC with gotint
    പൂർണ്ണസംഖ്യ		script_char;
    പൂർണ്ണസംഖ്യ		tb_change_cnt = typebuf.tb_change_cnt;

    എങ്കിൽ (wait_time == -1L || wait_time > 100L)  // flush output before waiting
    {
	cursor_on();
	out_flush_cursor(FALSE, FALSE);
#എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	എങ്കിൽ (gui.in_use && postponed_mouseshape)
	    update_mouseshape(-1);
#endif
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    എങ്കിൽ (State != MODE_HITRETURN)
    {
	did_outofmem_സന്ദേശം = FALSE;   // display out of memory message (again)
	did_swapwrite_സന്ദേശം = FALSE;  // display swap file write error again
    }
    undo_off = FALSE;		    // restart undo now

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    ആകുമ്പോൾ (scriptin[curscript] != ശൂന്യ && script_char < 0
#ifdef FEAT_EVAL
	    && !ignore_script
#endif
	    )
    {
#ifdef MESSAGE_QUEUE
	parse_queued_messages();
#endif

	എങ്കിൽ (got_int || (script_char = getc(scriptin[curscript])) < 0)
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
	    എങ്കിൽ (got_int)
		retesc = TRUE;
	    അല്ലെങ്കിൽ
		തിരികെ -1;
	}
	അല്ലെങ്കിൽ
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    എങ്കിൽ (script_char < 0)	// did not get a character from script
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	എങ്കിൽ (got_int)
	{
#define DUM_LEN (MAXMAPLEN * 3 + 3)
	    char_u	dum[DUM_LEN + 1];

	    വേണ്ടി (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		എങ്കിൽ (len == 0 || (len == 1 && dum[0] == Ctrl_C))
		    നിർത്തുക;
	    }
	    തിരികെ retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	എങ്കിൽ (wait_time == -1L || wait_time > 10L)
	    out_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_time, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    എങ്കിൽ (typebuf_changed(tb_change_cnt))
	തിരികെ 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    എങ്കിൽ (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    തിരികെ fix_നിവേശം_buffer(buf, len);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * "buf[]" must have room to triple the number of bytes!
 * Returns the new length.
 */
    പൂർണ്ണസംഖ്യ
fix_നിവേശം_buffer(char_u *buf, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p = buf;

    /*
     * Two characters are special: NUL and K_SPECIAL.
     * When compiled With the GUI CSI is also special.
     * Replace	     NUL by K_SPECIAL KS_ZERO	 KE_FILLER
     * Replace K_SPECIAL by K_SPECIAL KS_SPECIAL KE_FILLER
     * Replace       CSI by K_SPECIAL KS_EXTRA   KE_CSI
     */
    വേണ്ടി (i = len; --i >= 0; ++p)
    {
#ifdef FEAT_GUI
	// When the GUI is used any character can come after a CSI, don't
	// escape it.
	എങ്കിൽ (gui.in_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
# ifndef MSWIN
	// When not on MS-Windows and the GUI is not used CSI needs to be
	// escaped.
	അല്ലെങ്കിൽ എങ്കിൽ (!gui.in_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (പൂർണ്ണസംഖ്യ)KE_CSI;
	    len += 2;
	}
# endif
	അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (p[0] == NUL || (p[0] == K_SPECIAL
		    // timeout may generate K_CURSORHOLD,
		    // 'autocompletedelay' timeout K_COMPLETE_DELAY
		    && (i < 2 || p[1] != KS_EXTRA
			|| (p[2] != (പൂർണ്ണസംഖ്യ)KE_CURSORHOLD
			    && p[2] != (പൂർണ്ണസംഖ്യ)KE_COMPLETE_DELAY))
#എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
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
    തിരികെ len;
}

#എങ്കിൽ defined(USE_നിവേശം_BUF)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    പൂർണ്ണസംഖ്യ
നിവേശം_available(ശൂന്യം)
{
    തിരികെ (!vim_is_നിവേശം_buf_empty()
# എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#endif

/*
 * Function passed to do_cmdline() to get the command after a <Cmd> key from
 * typeahead.
 */
    സ്ഥിതം char_u *
getcmdkeycmd(
	പൂർണ്ണസംഖ്യ		promptc UNUSED,
	ശൂന്യം		*cookie UNUSED,
	പൂർണ്ണസംഖ്യ		indent UNUSED,
	getline_opt_T	do_concat UNUSED)
{
    garray_T	വരി_ga;
    പൂർണ്ണസംഖ്യ		c1 = -1;
    പൂർണ്ണസംഖ്യ		c2;
    പൂർണ്ണസംഖ്യ		cmod = 0;
    പൂർണ്ണസംഖ്യ		aborted = FALSE;

    ga_init2(&വരി_ga, 1, 32);

    // no mapping for these characters
    no_mapping++;

    got_int = FALSE;
    ആകുമ്പോൾ (c1 != NUL && !aborted)
    {
	എങ്കിൽ (ga_grow(&വരി_ga, 32) == FAIL)
	{
	    aborted = TRUE;
	    നിർത്തുക;
	}

	എങ്കിൽ (vgetorpeek(FALSE) == NUL)
	{
	    // incomplete <Cmd> is an error, because there is not much the user
	    // could do in this state.
	    emsg(_(e_cmd_mapping_must_end_with_cr));
	    aborted = TRUE;
	    നിർത്തുക;
	}

	// Get one character at a time.
	c1 = vgetorpeek(TRUE);

	// Get two extra bytes for special keys
	എങ്കിൽ (c1 == K_SPECIAL)
	{
	    c1 = vgetorpeek(TRUE);
	    c2 = vgetorpeek(TRUE);
	    എങ്കിൽ (c1 == KS_MODIFIER)
	    {
		cmod = c2;
		തുടരുക;
	    }
	    c1 = TO_SPECIAL(c1, c2);

	    // K_ESC is used to avoid ambiguity with the single Esc character
	    // that might be the start of an escape sequence.  Convert it back
	    // to a single Esc here.
	    എങ്കിൽ (c1 == K_ESC)
		c1 = ESC;

#ifdef FEAT_GUI
	    // Translate K_CSI to CSI.  The special key is only used to
	    // avoid it being recognized as the start of a special key.
	    എങ്കിൽ (c1 == K_CSI)
		c1 = CSI;
#endif
	}

	എങ്കിൽ (got_int)
	    aborted = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (c1 == '\r' || c1 == '\n')
	    c1 = NUL;  // end the line
	അല്ലെങ്കിൽ എങ്കിൽ (c1 == ESC)
	    aborted = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (c1 == K_COMMAND || c1 == K_SCRIPT_COMMAND)
	{
	    // give a nicer error message for this special case
	    emsg(_(e_cmd_mapping_must_end_with_cr_before_second_cmd));
	    aborted = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c1 == K_SNR)
	{
	    GA_CONCAT_LITERAL(&വരി_ga, "<SNR>");
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (cmod != 0)
	    {
		ga_append(&വരി_ga, K_SPECIAL);
		ga_append(&വരി_ga, KS_MODIFIER);
		ga_append(&വരി_ga, cmod);
	    }
	    എങ്കിൽ (IS_SPECIAL(c1))
	    {
		ga_append(&വരി_ga, K_SPECIAL);
		ga_append(&വരി_ga, K_SECOND(c1));
		ga_append(&വരി_ga, K_THIRD(c1));
	    }
	    അല്ലെങ്കിൽ
		ga_append(&വരി_ga, c1);
	}

	cmod = 0;
    }

    no_mapping--;

    എങ്കിൽ (aborted)
	ga_clear(&വരി_ga);

    തിരികെ (char_u *)വരി_ga.ga_data;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * If there was a mapping we get its SID.  Otherwise, use "last_used_sid", it
 * is set when redo'ing.
 * Put this SID in the redo buffer, so that "." will use the same script
 * context.
 */
    ശൂന്യം
may_add_last_used_map_to_redobuff(ശൂന്യം)
{
    char_u  buf[3 + 20];
    പൂർണ്ണസംഖ്യ	    buflen;
    പൂർണ്ണസംഖ്യ	    sid = -1;

    എങ്കിൽ (last_used_map != ശൂന്യ)
	sid = last_used_map->m_script_ctx.sc_sid;
    എങ്കിൽ (sid < 0)
	sid = last_used_sid;

    എങ്കിൽ (sid < 0)
	തിരികെ;

    // <K_SID>{nr};
    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = KE_SID;
    buflen = 3;

    buflen += vim_snprintf((അക്ഷരം *)buf + 3, 20, "%d;", sid);
    add_buff(&redobuff, buf, (long)buflen);
}
#endif

    പൂർണ്ണസംഖ്യ
do_cmdkey_command(പൂർണ്ണസംഖ്യ താക്കോൽ UNUSED, പൂർണ്ണസംഖ്യ flags)
{
    പൂർണ്ണസംഖ്യ	    res;
#ifdef FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    എങ്കിൽ (താക്കോൽ == K_SCRIPT_COMMAND
		  && (last_used_map != ശൂന്യ || SCRIPT_ID_VALID(last_used_sid)))
    {
	save_current_sctx = current_sctx;
	എങ്കിൽ (last_used_map != ശൂന്യ)
	    current_sctx = last_used_map->m_script_ctx;
	അല്ലെങ്കിൽ
	{
	    current_sctx.sc_sid = last_used_sid;
	    current_sctx.sc_lnum = 0;
	    current_sctx.sc_version = SCRIPT_ITEM(last_used_sid)->sn_version;
	}
    }
#endif

    res = do_cmdline(ശൂന്യ, getcmdkeycmd, ശൂന്യ, flags);

#ifdef FEAT_EVAL
    എങ്കിൽ (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#endif

    തിരികെ res;
}

#എങ്കിൽ defined(FEAT_EVAL)
    ശൂന്യം
reset_last_used_map(mapblock_T *mp)
{
    എങ്കിൽ (last_used_map != mp)
	തിരികെ;

    last_used_map = ശൂന്യ;
    last_used_sid = -1;
}
#endif
