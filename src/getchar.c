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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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

ସ୍ଥିର buffheader_T redobuff = {{ଖାଲି, 0, {NUL}}, ଖାଲି, 0, 0, FALSE};
ସ୍ଥିର buffheader_T old_redobuff = {{ଖାଲି, 0, {NUL}}, ଖାଲି, 0, 0, FALSE};
ସ୍ଥିର buffheader_T recordbuff = {{ଖାଲି, 0, {NUL}}, ଖାଲି, 0, 0, FALSE};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା typeahead_char = 0;		// typeahead char that's not flushed

#ifdef FEAT_EVAL
ସ୍ଥିର char_u typedchars[MAXMAPLEN + 1] = { NUL };  // typed chars before map
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା typedchars_pos = 0;
#endif

/*
 * When block_redo is TRUE the redo buffer will not be changed.
 * Used by edit() to repeat insertions.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	block_redo = FALSE;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଚାବି_Noremap = 0;	    // remapping flags

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
#ଅନ୍ୟଥା // Tiny version
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
#endif
ସ୍ଥିର char_u	typebuf_init[TYPELEN_INIT];	// initial typebuf.tb_buf
ସ୍ଥିର char_u	noremapbuf_init[TYPELEN_INIT];	// initial typebuf.tb_noremap

ସ୍ଥିର size_t	last_recorded_len = 0;	// number of last recorded chars

#ifdef FEAT_EVAL
mapblock_T	*last_used_map = ଖାଲି;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last_used_sid = -1;
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	read_readbuf(buffheader_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା advance);
ସ୍ଥିର ଶୂନ୍ୟ	init_typebuf(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	may_sync_undo(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	free_typebuf(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	closescript(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	updatescript(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	vgetorpeek(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	inchar(char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen, long wait_time);
#ifdef FEAT_EVAL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_ଚାବି_ନିବେଶ_pre(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
#endif

/*
 * Free and clear a buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    ପାଇଁ (p = buf->bh_first.b_next; p != ଖାଲି; p = np)
    {
	np = p->b_next;
	vim_free(p);
    }
    buf->bh_first.b_next = ଖାଲି;
    buf->bh_curr = ଖାଲି;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    ସ୍ଥିର char_u *
get_buffcont(
    buffheader_T	*buffer,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			dozero,	    // count == zero is not an error
    size_t		*len)	    // the length of the returned buffer
{
    long_u	    ଗଣନା_2 = 0;
    char_u	    *p = ଖାଲି;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;
    size_t	    i = 0;

    // compute the total length of the string
    ପାଇଁ (bp = buffer->bh_first.b_next; bp != ଖାଲି; bp = bp->b_next)
	ଗଣନା_2 += (long_u)bp->b_strlen;

    ଯଦି ((ଗଣନା_2 > 0 || dozero) && (p = alloc(ଗଣନା_2 + 1)) != ଖାଲି)
    {
	p2 = p;
	ପାଇଁ (bp = buffer->bh_first.b_next; bp != ଖାଲି; bp = bp->b_next)
	    ପାଇଁ (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
	i = (size_t)(p2 - p);
    }

    ଯଦି (len != ଖାଲି)
	*len = i;

    ଫେରନ୍ତୁ p;
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(ଶୂନ୍ୟ)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE, &len);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    ଯଦି (len >= last_recorded_len)
    {
	len -= last_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    ଯଦି (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    ଫେରନ୍ତୁ (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    string_T
get_inserted(ଶୂନ୍ୟ)
{
    size_t len = 0;
    char_u *str = get_buffcont(&redobuff, FALSE, &len);
    string_T ret = { str, len };
    ଫେରନ୍ତୁ ret;
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_buff(
    buffheader_T	*buf,
    char_u		*s,
    long		slen)	// length of "s" or -1
{
    ଯଦି (slen < 0)
	slen = (long)STRLEN(s);
    ଯଦି (slen == 0)				// don't add empty strings
	ଫେରନ୍ତୁ;

    ଯଦି (buf->bh_first.b_next == ଖାଲି)	// first add to list
    {
	buf->bh_curr = &(buf->bh_first);
	buf->bh_create_newblock = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (buf->bh_curr == ଖାଲି)	// buffer has already been read
    {
	iemsg(e_add_to_internal_buffer_that_was_already_read_from);
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା ଯଦି (buf->bh_index != 0)
    {
	mch_memmove(buf->bh_first.b_next->b_str,
		    buf->bh_first.b_next->b_str + buf->bh_index,
		    (buf->bh_first.b_next->b_strlen - buf->bh_index) + 1);
	buf->bh_first.b_next->b_strlen -= buf->bh_index;
	buf->bh_space += buf->bh_index;
    }
    buf->bh_index = 0;

    ଯଦି (!buf->bh_create_newblock && buf->bh_space >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)slen)
    {
	vim_strncpy(buf->bh_curr->b_str + buf->bh_curr->b_strlen, s, (size_t)slen);
	buf->bh_curr->b_strlen += slen;
	buf->bh_space -= slen;
    }
    ଅନ୍ୟଥା
    {
	long_u	    len;
	buffblock_T *p;

	ଯଦି (slen < MINIMAL_SIZE)
	    len = MINIMAL_SIZE;
	ଅନ୍ୟଥା
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ; // no space, just forget it
	vim_strncpy(p->b_str, s, (size_t)slen);
	p->b_strlen = slen;
	buf->bh_space = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(len - slen);
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
    ସ୍ଥିର ଶୂନ୍ୟ
delete_buff_tail(buffheader_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା slen)
{
    ଯଦି (buf->bh_curr == ଖାଲି)
	ଫେରନ୍ତୁ;  // nothing to delete
    ଯଦି (buf->bh_curr->b_strlen < (size_t)slen)
	ଫେରନ୍ତୁ;

    buf->bh_curr->b_str[buf->bh_curr->b_strlen - (size_t)slen] = NUL;
    buf->bh_curr->b_strlen -= slen;
    buf->bh_space += slen;
}

/*
 * Add number "n" to buffer "buf".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_ଅଙ୍କ_buff(buffheader_T *buf, long n)
{
    char_u	ସଂଖ୍ୟା[32];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		numberlen;

    numberlen = vim_snprintf((ଅକ୍ଷର *)ସଂଖ୍ୟା, ଆକାର(ସଂଖ୍ୟା), "%ld", n);
    add_buff(buf, ସଂଖ୍ୟା, (long)numberlen);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_char_buff(buffheader_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u	bytes[MB_MAXBYTES + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	temp[4];
    long	templen;

    ଯଦି (IS_SPECIAL(c))
	len = 1;
    ଅନ୍ୟଥା
	len = (*mb_char2bytes)(c, bytes);
    ପାଇଁ (i = 0; i < len; ++i)
    {
	ଯଦି (!IS_SPECIAL(c))
	    c = bytes[i];

	ଯଦି (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    temp[0] = K_SPECIAL;
	    temp[1] = K_SECOND(c);
	    temp[2] = K_THIRD(c);
	    temp[3] = NUL;
	    templen = 3;
	}
#ifdef FEAT_GUI
	ଅନ୍ୟଥା ଯଦି (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    temp[0] = CSI;
	    temp[1] = KS_EXTRA;
	    temp[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
	    temp[3] = NUL;
	    templen = 3;
	}
#endif
	ଅନ୍ୟଥା
	{
	    temp[0] = c;
	    temp[1] = NUL;
	    templen = 1;
	}
	add_buff(buf, temp, templen);
    }
}

// First read ahead buffer. Used for translated commands.
ସ୍ଥିର buffheader_T readbuf1 = {{ଖାଲି, 0, {NUL}}, ଖାଲି, 0, 0, FALSE};

// Second read ahead buffer. Used for redo.
ସ୍ଥିର buffheader_T readbuf2 = {{ଖାଲି, 0, {NUL}}, ଖାଲି, 0, 0, FALSE};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_readbuffers(ପୂର୍ଣ୍ଣସଂଖ୍ୟା advance)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

    c = read_readbuf(&readbuf1, advance);
    ଯଦି (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    ଫେରନ୍ତୁ c;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_readbuf(buffheader_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା advance)
{
    char_u	c;
    buffblock_T	*curr;

    ଯଦି (buf->bh_first.b_next == ଖାଲି)  // buffer is empty
	ଫେରନ୍ତୁ NUL;

    curr = buf->bh_first.b_next;
    c = curr->b_str[buf->bh_index];

    ଯଦି (advance)
    {
	ଯଦି (curr->b_str[++buf->bh_index] == NUL)
	{
	    buf->bh_first.b_next = curr->b_next;
	    vim_free(curr);
	    buf->bh_index = 0;
	}
    }
    ଫେରନ୍ତୁ c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
start_stuff(ଶୂନ୍ୟ)
{
    ଯଦି (readbuf1.bh_first.b_next != ଖାଲି)
    {
	readbuf1.bh_curr = &(readbuf1.bh_first);
	readbuf1.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
    ଯଦି (readbuf2.bh_first.b_next != ଖାଲି)
    {
	readbuf2.bh_curr = &(readbuf2.bh_first);
	readbuf2.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
stuff_empty(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (readbuf1.bh_first.b_next == ଖାଲି
	 && readbuf2.bh_first.b_next == ଖାଲି);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * readbuf2.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
readbuf1_empty(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (readbuf1.bh_first.b_next == ଖାଲି);
}
#endif

/*
 * Set a typeahead character that won't be flushed.
 */
    ଶୂନ୍ୟ
typeahead_noflush(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    ଶୂନ୍ୟ
flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    start_stuff();
    ଯେତେବେଳେ (read_readbuffers(TRUE) != NUL)
	;

    ଯଦି (flush_typeahead == FLUSH_MINIMAL)
    {
	// remove mapped characters at the start only,
	// but only when enough space left in typebuf
	ଯଦି (typebuf.tb_off + typebuf.tb_maplen >= typebuf.tb_buflen)
	{
	    typebuf.tb_off = MAXMAPLEN;
	    typebuf.tb_len = 0;
	}
	ଅନ୍ୟଥା
	{
	    typebuf.tb_off += typebuf.tb_maplen;
	    typebuf.tb_len -= typebuf.tb_maplen;
	}
#ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	ଯଦି (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#endif
    }
    ଅନ୍ୟଥା
    {
	// remove typeahead
	ଯଦି (flush_typeahead == FLUSH_ନିବେଶ)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    ଯେତେବେଳେ (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#endif
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
    ଯଦି (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * The previous contents of the redo buffer is kept in old_redobuffer.
 * This is used for the CTRL-O <.> command in insert mode.
 */
    ଶୂନ୍ୟ
ResetRedobuff(ଶୂନ୍ୟ)
{
    ଯଦି (block_redo)
	ଫେରନ୍ତୁ;

    free_buff(&old_redobuff);
    old_redobuff = redobuff;
    redobuff.bh_first.b_next = ଖାଲି;
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    ଶୂନ୍ୟ
CancelRedo(ଶୂନ୍ୟ)
{
    ଯଦି (block_redo)
	ଫେରନ୍ତୁ;

    free_buff(&redobuff);
    redobuff = old_redobuff;
    old_redobuff.bh_first.b_next = ଖାଲି;
    start_stuff();
    ଯେତେବେଳେ (read_readbuffers(TRUE) != NUL)
	;
}

/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
    ଶୂନ୍ୟ
saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;
    size_t	slen;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_first.b_next = ଖାଲି;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_first.b_next = ଖାଲି;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE, &slen);
    ଯଦି (s == ଖାଲି)
	ଫେରନ୍ତୁ;

    add_buff(&redobuff, s, (long)slen);
    vim_free(s);
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
AppendToRedobuff(char_u *s)
{
    ଯଦି (!block_redo)
	add_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    ଶୂନ୍ୟ
AppendToRedobuffLit(
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*start;

    ଯଦି (block_redo)
	ଫେରନ୍ତୁ;

    ଯେତେବେଳେ (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	start = s;
	ଯେତେବେଳେ (*s >= ' ' && *s < DEL && (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	ଯଦି (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	ଯଦି (s > start)
	    add_buff(&redobuff, start, (long)(s - start));

	ଯଦି (*s == NUL || (len >= 0 && s - str >= len))
	    ଭାଙ୍ଗ;

	// Handle a special or multibyte character.
	ଯଦି (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	ଅନ୍ୟଥା
	    c = *s++;
	ଯଦି (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    add_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048
	ଯଦି (*s == NUL && c == '0')
	    add_buff(&redobuff, (char_u *)"048", 3L);
	ଅନ୍ୟଥା
	    add_char_buff(&redobuff, c);
    }
}

/*
 * Append "s" to the redo buffer, leaving 3-byte special key codes unmodified
 * and escaping other K_SPECIAL and CSI bytes.
 */
    ଶୂନ୍ୟ
AppendToRedobuffSpec(char_u *s)
{
    ଯଦି (block_redo)
	ଫେରନ୍ତୁ;

    ଯେତେବେଳେ (*s != NUL)
    {
	ଯଦି (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    add_buff(&redobuff, s, 3L);
	    s += 3;
	}
	ଅନ୍ୟଥା
	    add_char_buff(&redobuff, mb_cptr2char_adv(&s));
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ଶୂନ୍ୟ
AppendCharToRedobuff(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (!block_redo)
	add_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    ଶୂନ୍ୟ
Append_ସଂଖ୍ୟା_To_Redobuff(long n)
{
    ଯଦି (!block_redo)
	add_ଅଙ୍କ_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    ଶୂନ୍ୟ
stuffReadbuff(char_u *s)
{
    add_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    ଶୂନ୍ୟ
stuffRedoReadbuff(char_u *s)
{
    add_buff(&readbuf2, s, -1L);
}

    ଶୂନ୍ୟ
stuffReadbuffLen(char_u *s, long len)
{
#ifdef FEAT_EVAL
    ଯଦି (add_last_ଭର୍ତ୍ତି == 1) // Only add if this is the first call, for
			      // recursive calls, ignore.
	ga_concat_len(&last_ଭର୍ତ୍ତି_ga, s, (size_t)len);
#endif
    add_buff(&readbuf1, s, len);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    ଶୂନ୍ୟ
stuffReadbuffSpec(char_u *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

    ଯେତେବେଳେ (*s != NUL)
    {
	ଯଦି (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	ଅନ୍ୟଥା
	{
	    c = mb_cptr2char_adv(&s);
	    ଯଦି (c == CAR || c == NL || c == ESC)
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
    ଶୂନ୍ୟ
stuffcharReadbuff(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    add_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    ଶୂନ୍ୟ
stuffnumReadbuff(long n)
{
    add_ଅଙ୍କ_buff(&readbuf1, n);
}

/*
 * Stuff a string into the typeahead buffer, such that edit() will insert it
 * literally ("literally" TRUE) or interpret is as typed characters.
 */
    ଶୂନ୍ୟ
stuffescaped(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା literally)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*start;

    ଯେତେବେଳେ (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	start = arg;
	ଯେତେବେଳେ ((*arg >= ' ' && *arg < DEL)
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	ଯଦି (arg > start)
	    stuffReadbuffLen(start, (long)(arg - start));

	// stuff a single special character
	ଯଦି (*arg != NUL)
	{
	    ଯଦି (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    ଅନ୍ୟଥା
		c = *arg++;
	    ଯଦି (literally && ((c < ' ' && c != TAB) || c == DEL))
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_redo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା init, ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_redo)
{
    ସ୍ଥିର buffblock_T	*bp;
    ସ୍ଥିର char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			n;
    char_u		buf[MB_MAXBYTES + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;

    ଯଦି (init)
    {
	ଯଦି (old_redo)
	    bp = old_redobuff.bh_first.b_next;
	ଅନ୍ୟଥା
	    bp = redobuff.bh_first.b_next;
	ଯଦି (bp == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	p = bp->b_str;
	ଫେରନ୍ତୁ OK;
    }
    ଯଦି ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	ଯଦି (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	ଅନ୍ୟଥା
	    n = 1;
	ପାଇଁ (i = 0; ; ++i)
	{
	    ଯଦି (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = TO_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#ifdef FEAT_GUI
	    ଯଦି (c == CSI)	// escaped CSI
		p += 2;
#endif
	    ଯଦି (*++p == NUL && bp->b_next != ଖାଲି)
	    {
		bp = bp->b_next;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    ଯଦି (i == n - 1)	// last byte of a character
	    {
		ଯଦି (n != 1)
		    c = (*mb_ptr2char)(buf);
		ଭାଙ୍ଗ;
	    }
	    c = *p;
	    ଯଦି (c == NUL)	// cannot happen?
		ଭାଙ୍ଗ;
	}
    }

    ଫେରନ୍ତୁ c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
copy_redo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_redo)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;

    ଯେତେବେଳେ ((c = read_redo(FALSE, old_redo)) != NUL)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
start_redo(long ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_redo)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;

    // init the pointers; return if nothing to redo
    ଯଦି (read_redo(TRUE, old_redo) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    c = read_redo(FALSE, old_redo);

#ifdef FEAT_EVAL
    ଯଦି (c == K_SID)
    {
	// Copy the <SID>{sid}; sequence
	add_char_buff(&readbuf2, c);
	ପାଇଁ (;;)
	{
	    c = read_redo(FALSE, old_redo);
	    add_char_buff(&readbuf2, c);
	    ଯଦି (!SAFE_isdigit(c))
		ଭାଙ୍ଗ;
	}
	c = read_redo(FALSE, old_redo);
    }
#endif

    // copy the buffer name, if present
    ଯଦି (c == '"')
    {
	add_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	// if a numbered buffer is used, increment the number
	ଯଦି (c >= '1' && c < '9')
	    ++c;
	add_char_buff(&readbuf2, c);

	// the expression register should be re-evaluated
	ଯଦି (c == '=')
	{
	    add_char_buff(&readbuf2, CAR);
	    cmd_silent = TRUE;
	}

	c = read_redo(FALSE, old_redo);
    }

    ଯଦି (c == 'v')   // redo Visual
    {
	VIsual = curwin->w_cursor;
	VIsual_active = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    // try to enter the count (in place of a previous count)
    ଯଦି (ଗଣନା_2)
    {
	ଯେତେବେଳେ (VIM_ISDIGIT(c))	// skip "old" count
	    c = read_redo(FALSE, old_redo);
	add_ଅଙ୍କ_buff(&readbuf2, ଗଣନା_2);
    }

    // copy the rest from the redo buffer into the stuff buffer
    add_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    ଫେରନ୍ତୁ OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
start_redo_ins(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;

    ଯଦି (read_redo(TRUE, FALSE) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    start_stuff();

    // skip the count and the command character
    ଯେତେବେଳେ ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	ଯଦି (vim_strchr((char_u *)"AaIiRrOo", c) != ଖାଲି)
	{
	    ଯଦି (c == 'O' || c == 'o')
		add_buff(&readbuf2, NL_STR, -1L);
	    ଭାଙ୍ଗ;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    block_redo = TRUE;
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
stop_redo_ins(ଶୂନ୍ୟ)
{
    block_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_typebuf(ଶୂନ୍ୟ)
{
    ଯଦି (typebuf.tb_buf != ଖାଲି)
	ଫେରନ୍ତୁ;

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
noremap_ଚାବିଗୁଡ଼ିକ(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ଚାବି_Noremap & (RM_NONE|RM_SCRIPT);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_typebuf(
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noremap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		offset,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nottyped,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent)
{
    char_u	*s1, *s2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		addlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newoff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		val;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nrm;

    init_typebuf();
    ଯଦି (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
    state_no_longer_safe("ins_typebuf()");

    addlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);

    ଯଦି (offset == 0 && addlen <= typebuf.tb_off)
    {
	/*
	 * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
	 */
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    ଅନ୍ୟଥା ଯଦି (typebuf.tb_len == 0 && typebuf.tb_buflen
					       >= addlen + 3 * (MAXMAPLEN + 4))
    {
	/*
	 * Buffer is empty and string fits in the existing buffer.
	 * Leave some space before and after, if possible.
	 */
	typebuf.tb_off = (typebuf.tb_buflen - addlen - 3 * (MAXMAPLEN + 4)) / 2;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra;

	/*
	 * Need to allocate a new buffer.
	 * In typebuf.tb_buf there must always be room for 3 * (MAXMAPLEN + 4)
	 * characters.  We add some extra room to avoid having to allocate too
	 * often.
	 */
	newoff = MAXMAPLEN + 4;
	extra = addlen + newoff + 4 * (MAXMAPLEN + 4);
	ଯଦି (typebuf.tb_len > 2147483647 - extra)
	{
	    // string is getting too long for a 32 bit int
	    emsg(_(e_command_too_complex));    // also calls flush_buffers
	    setcursor();
	    ଫେରନ୍ତୁ FAIL;
	}
	newlen = typebuf.tb_len + extra;
	s1 = alloc(newlen);
	ଯଦି (s1 == ଖାଲି)		    // out of memory
	    ଫେରନ୍ତୁ FAIL;
	s2 = alloc(newlen);
	ଯଦି (s2 == ଖାଲି)		    // out of memory
	{
	    vim_free(s1);
	    ଫେରନ୍ତୁ FAIL;
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
	ଯଦି (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)offset);
	mch_memmove(s2 + newoff + offset + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + offset,
					   (size_t)(typebuf.tb_len - offset));
	ଯଦି (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    // If noremap == REMAP_SCRIPT: do remap script-local mappings.
    ଯଦି (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    ଅନ୍ୟଥା ଯଦି (noremap == REMAP_SKIP)
	val = RM_ABBR;
    ଅନ୍ୟଥା
	val = RM_NONE;

    /*
     * Adjust typebuf.tb_noremap[] for the new characters:
     * If noremap == REMAP_NONE or REMAP_SCRIPT: new characters are
     *			(sometimes) not remappable
     * If noremap == REMAP_YES: all the new characters are mappable
     * If noremap  > 0: "noremap" characters are not remappable, the rest
     *			mappable
     */
    ଯଦି (noremap == REMAP_SKIP)
	nrm = 1;
    ଅନ୍ୟଥା ଯଦି (noremap < 0)
	nrm = addlen;
    ଅନ୍ୟଥା
	nrm = noremap;
    ପାଇଁ (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + offset] =
						  (--nrm >= 0) ? val : RM_YES;

    // tb_maplen and tb_silent only remember the length of mapped and/or
    // silent mappings at the start of the buffer, assuming that a mapped
    // sequence doesn't result in typed characters.
    ଯଦି (nottyped || typebuf.tb_maplen > offset)
	typebuf.tb_maplen += addlen;
    ଯଦି (silent || typebuf.tb_silent > offset)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    ଯଦି (typebuf.tb_no_abbr_cnt && offset == 0)	// and not used for abbrev.s
	typebuf.tb_no_abbr_cnt += addlen;

    ଫେରନ୍ତୁ OK;
}

/*
 * Put character "c" back into the typeahead buffer.
 * Can be used for a character obtained by vgetc() that needs to be put back.
 * Uses cmd_silent, KeyTyped and KeyNoremap to restore the flags belonging to
 * the char.
 * Returns the length of what was inserted.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_char_typebuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା modifiers)
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = special_to_buf(c, modifiers, TRUE, buf);

    buf[len] = NUL;
    (ଶୂନ୍ୟ)ins_typebuf(buf, ଚାବି_Noremap, 0, !ଚାବି_Typed, cmd_silent);
    ଫେରନ୍ତୁ len;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typebuf_changed(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    ଫେରନ୍ତୁ (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#endif
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typebuf_typed(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typebuf_maplen(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    ଶୂନ୍ୟ
del_typebuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ଯଦି (len == 0)
	ଫେରନ୍ତୁ;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    ଯଦି (offset == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    ଅନ୍ୟଥା
    {
	i = typebuf.tb_off + offset;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	ଯଦି (typebuf.tb_off > MAXMAPLEN)
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

    ଯଦି (typebuf.tb_maplen > offset)		// adjust tb_maplen
    {
	ଯଦି (typebuf.tb_maplen < offset + len)
	    typebuf.tb_maplen = offset;
	ଅନ୍ୟଥା
	    typebuf.tb_maplen -= len;
    }
    ଯଦି (typebuf.tb_silent > offset)		// adjust tb_silent
    {
	ଯଦି (typebuf.tb_silent < offset + len)
	    typebuf.tb_silent = offset;
	ଅନ୍ୟଥା
	    typebuf.tb_silent -= len;
    }
    ଯଦି (typebuf.tb_no_abbr_cnt > offset)	// adjust tb_no_abbr_cnt
    {
	ଯଦି (typebuf.tb_no_abbr_cnt < offset + len)
	    typebuf.tb_no_abbr_cnt = offset;
	ଅନ୍ୟଥା
	    typebuf.tb_no_abbr_cnt -= len;
    }

#ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#endif
    ଯଦି (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * State for adding bytes to a recording or 'showcmd'.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_c;
    size_t	buflen;
    unsigned	pending_special;
    unsigned	pending_mbyte;
} gotchars_state_T;

/*
 * Add a single byte to a recording or 'showcmd'.
 * Return TRUE if a full key has been received, FALSE otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gotchars_add_byte(gotchars_state_T *state, char_u byte)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = state->buf[state->buflen++] = byte;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_special = state->pending_special > 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_mbyte = state->pending_mbyte > 0;

    ଯଦି (in_special)
	state->pending_special--;
    ଅନ୍ୟଥା ଯଦି (c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	    )
	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	state->pending_special = 2;

    ଯଦି (state->pending_special > 0)
	ଯାଅ ret_false;

    ଯଦି (in_mbyte)
	state->pending_mbyte--;
    ଅନ୍ୟଥା
    {
	ଯଦି (in_special)
	{
	    ଯଦି (state->prev_c == KS_MODIFIER)
		// When receiving a modifier, wait for the modified key.
		ଯାଅ ret_false;
	    c = TO_SPECIAL(state->prev_c, c);
	    // Drop K_FOCUSGAINED, K_FOCUSLOST and K_COMPLETE_DELAY, they are
	    // not useful in a recording.
	    ଯଦି (c == K_FOCUSGAINED || c == K_FOCUSLOST || c == K_COMPLETE_DELAY)
		state->buflen = 0;
	}
	// When receiving a multibyte character, store it until we have all
	// the bytes, so that it won't be split between two buffer blocks,
	// and delete_buff_tail() will work properly.
	state->pending_mbyte = MB_BYTE2LEN_CHECK(c) - 1;
    }

    ଯଦି (state->pending_mbyte > 0)
	ଯାଅ ret_false;

    retval = TRUE;
ret_false:
    state->prev_c = c;
    ଫେରନ୍ତୁ retval;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the record buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gotchars(char_u *chars, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u		*s = chars;
    size_t		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			todo = len;
    ସ୍ଥିର gotchars_state_T state;

    ଯେତେବେଳେ (todo-- > 0)
    {
	ଯଦି (!gotchars_add_byte(&state, *s++))
	    ଜାରି;

	// Handle one byte at a time; no translation to be done.
	ପାଇଁ (i = 0; i < state.buflen; ++i)
	    updatescript(state.buf[i]);

	ଯଦି (reg_recording != 0)
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
    debug_did_ସନ୍ଦେଶ = FALSE;
#endif

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}

/*
 * Record an <Ignore> key.
 */
    ଶୂନ୍ୟ
gotchars_ignore(ଶୂନ୍ୟ)
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
    ଶୂନ୍ୟ
ungetchars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଯଦି (reg_recording == 0)
	ଫେରନ୍ତୁ;

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
    ସ୍ଥିର ଶୂନ୍ୟ
may_sync_undo(ଶୂନ୍ୟ)
{
    ଯଦି ((!(State & (MODE_ଭର୍ତ୍ତି | MODE_CMDLINE)) || arrow_used)
					       && scriptin[curscript] == ଖାଲି)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
alloc_typebuf(ଶୂନ୍ୟ)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    ଯଦି (typebuf.tb_buf == ଖାଲି || typebuf.tb_noremap == ଖାଲି)
    {
	free_typebuf();
	ଫେରନ୍ତୁ FAIL;
    }
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_off = MAXMAPLEN + 4;  // can insert without realloc
    typebuf.tb_len = 0;
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    typebuf.tb_no_abbr_cnt = 0;
    ଯଦି (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
#ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    typebuf_was_filled = FALSE;
#endif
    ଫେରନ୍ତୁ OK;
}

/*
 * Free the buffers of "typebuf".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_typebuf(ଶୂନ୍ୟ)
{
    ଯଦି (typebuf.tb_buf == typebuf_init)
	internal_error("Free typebuf 1");
    ଅନ୍ୟଥା
	VIM_CLEAR(typebuf.tb_buf);
    ଯଦି (typebuf.tb_noremap == noremapbuf_init)
	internal_error("Free typebuf 2");
    ଅନ୍ୟଥା
	VIM_CLEAR(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
ସ୍ଥିର typebuf_T saved_typebuf[NSCRIPT];

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
save_typebuf(ଶୂନ୍ୟ)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    ଯଦି (alloc_typebuf() == FAIL)
    {
	closescript();
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_char = -1;	// character put back by vungetc()
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_mod_mask;	// mod_mask for ungotten character
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_mouse_row;	// mouse_row related to old_char
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_mouse_col;	// mouse_col related to old_char
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_ଚାବି_Stuffed;	// whether old_char was stuffed

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା can_get_old_char(ଶୂନ୍ୟ)
{
    // If the old character was not stuffed and characters have been added to
    // the stuff buffer, need to first get the stuffed characters instead.
    ଫେରନ୍ତୁ old_char != -1 && (old_ଚାବି_Stuffed || stuff_empty());
}

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    ଶୂନ୍ୟ
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    ଯଦି (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_mask = old_mod_mask;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_first.b_next = ଖାଲି;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_first.b_next = ଖାଲି;
#ifdef USE_ନିବେଶ_BUF
    tp->save_inputbuf = get_ନିବେଶ_buf();
#endif
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    ଶୂନ୍ୟ
restore_typeahead(tasave_T *tp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା overwrite UNUSED)
{
    ଯଦି (tp->typebuf_valid)
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
#ifdef USE_ନିବେଶ_BUF
    set_ନିବେଶ_buf(tp->save_inputbuf, overwrite);
#endif
}

/*
 * Open a new script file for the ":source!" command.
 */
    ଶୂନ୍ୟ
openscript(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		directly)	// when TRUE execute directly
{
    ଯଦି (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	ଫେରନ୍ତୁ;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

#ifdef FEAT_EVAL
    ଯଦି (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	ଫେରନ୍ତୁ;
#endif

    ଯଦି (scriptin[curscript] != ଖାଲି)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    ଯଦି ((scriptin[curscript] = mch_fopen((ଅକ୍ଷର *)NameBuff, READBIN)) == ଖାଲି)
    {
	semsg(_(e_cant_open_file_str), name);
	ଯଦି (curscript)
	    --curscript;
	ଫେରନ୍ତୁ;
    }
    ଯଦି (save_typebuf() == FAIL)
	ଫେରନ୍ତୁ;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    ଯଦି (directly)
    {
	oparg_T	oa;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	oldcurscript;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_State = State;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_restart_edit = restart_edit;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_insertmode = p_im;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_finish_op = finish_op;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll;

	State = MODE_NORMAL;
	ସନ୍ଦେଶ_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	finish_op = FALSE;

	oldcurscript = curscript;
	କର
	{
	    update_topline_cursor();	// update cursor position and topline
	    normal_cmd(&oa, FALSE);	// execute one command
	    (ଶୂନ୍ୟ)vpeekc();		// check for end of file
	}
	ଯେତେବେଳେ (scriptin[oldcurscript] != ଖାଲି);

	State = save_State;
	ସନ୍ଦେଶ_scroll = save_ସନ୍ଦେଶ_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	finish_op = save_finish_op;
    }
}

/*
 * Close the currently active input script.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
closescript(ଶୂନ୍ୟ)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = ଖାଲି;
    ଯଦି (curscript > 0)
	--curscript;
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
close_all_scripts(ଶୂନ୍ୟ)
{
    ଯେତେବେଳେ (scriptin[0] != ଖାଲି)
	closescript();
}
#endif

/*
 * Return TRUE when reading keys from a script file.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
using_script(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ scriptin[curscript] != ଖାଲି;
}

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    ଶୂନ୍ୟ
before_blocking(ଶୂନ୍ୟ)
{
    // Tell terminal to flush screen contents before blocking
    term_set_sync_output(TERM_SYNC_OUTPUT_FLUSH);

    updatescript(0);
#ifdef FEAT_EVAL
    ଯଦି (may_garbage_collect)
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
    ସ୍ଥିର ଶୂନ୍ୟ
updatescript(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2 = 0;

    ଯଦି (c && scriptout)
	putc(c, scriptout);
    ଯଦି (c == 0 || (p_uc > 0 && ++ଗଣନା_2 >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	ଗଣନା_2 = 0;
    }
#ifdef FEAT_EVAL
    ଯଦି (typedchars_pos < MAXMAPLEN)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
merge_modify_Other_ଚାବିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c_arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *modifiers)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = c_arg;

    // CTRL only uses the lower 5 bits of the character.
    ଯଦି (*modifiers & MOD_MASK_CTRL)
    {
	ଯଦି ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	{
	    c &= 0x1f;
	    ଯଦି (c == NUL)
		c = K_ZERO;
	}
	ଅନ୍ୟଥା ଯଦି (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#ifdef FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	ଅନ୍ୟଥା ଯଦି (c == '2')
	    c = NUL;
	ଅନ୍ୟଥା ଯଦି (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	ଅନ୍ୟଥା ଯଦି (c == '8')
	    c = BS;
	ଅନ୍ୟଥା ଯଦି (c == '?')
	    c = DEL;
#endif
	ଯଦି (c != c_arg)
	    *modifiers &= ~MOD_MASK_CTRL;
    }

    // Alt/Meta sets the 8th bit of the character.
    ଯଦି ((*modifiers & (MOD_MASK_META | MOD_MASK_ALT))
	    && c >= 0 && c <= 127)
    {
	// Some terminals (esp. Kitty) do not include Shift in the character.
	// Apply it here to get consistency across terminals.  Only do ASCII
	// letters, for other characters it depends on the keyboard layout.
	ଯଦି ((*modifiers & MOD_MASK_SHIFT) && c >= 'a' && c <= 'z')
	{
	    c += 'a' - 'A';
	    *modifiers &= ~MOD_MASK_SHIFT;
	}
	c += 0x80;
	*modifiers &= ~(MOD_MASK_META | MOD_MASK_ALT);
    }

    ଫେରନ୍ତୁ c;
}

/*
 * Add a single byte to 'showcmd' for a partially matched mapping.
 * Call add_to_showcmd() if a full key has been received.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_byte_to_showcmd(char_u byte)
{
    ସ୍ଥିର gotchars_state_T state;
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = NUL;

    ଯଦି (!p_sc || ସନ୍ଦେଶ_silent != 0)
	ଫେରନ୍ତୁ;

    ଯଦି (!gotchars_add_byte(&state, byte))
	ଫେରନ୍ତୁ;

    state.buf[state.buflen] = NUL;
    state.buflen = 0;

    ptr = state.buf;
    ଯଦି (ptr[0] == K_SPECIAL && ptr[1] == KS_MODIFIER && ptr[2] != NUL)
    {
	modifiers = ptr[2];
	ptr += 3;
    }

    ଯଦି (*ptr != NUL)
    {
	char_u		*mb_ptr = mb_unescape(&ptr);

	c = mb_ptr != ଖାଲି ? (*mb_ptr2char)(mb_ptr) : *ptr++;
	ଯଦି (c <= 0x7f)
	{
	    // Merge modifiers into the key to make the result more readable.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers_after = modifiers;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mod_c = merge_modify_Other_ଚାବିଗୁଡ଼ିକ(c, &modifiers_after);

	    ଯଦି (modifiers_after == 0)
	    {
		modifiers = 0;
		c = mod_c;
	    }
	}
    }

    // TODO: is there a more readable and yet compact representation of
    // modifiers and special keys?
    ଯଦି (modifiers != 0)
    {
	add_to_showcmd(K_SPECIAL);
	add_to_showcmd(KS_MODIFIER);
	add_to_showcmd(modifiers);
    }
    ଯଦି (c != NUL)
	add_to_showcmd(c);
    ଯେତେବେଳେ (*ptr != NUL)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vgetc(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c, c2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    char_u	buf[MB_MAXBYTES + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

#ifdef FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    ଯଦି (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#endif

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    ଯଦି (can_get_old_char())
    {
	c = old_char;
	old_char = -1;
	mod_mask = old_mod_mask;
	mouse_row = old_mouse_row;
	mouse_col = old_mouse_col;
    }
    ଅନ୍ୟଥା
    {
	// number of characters recorded from the last vgetc() call
	ସ୍ଥିର size_t	last_vgetc_recorded_len = 0;

	mod_mask = 0;
	vgetc_mod_mask = 0;
	vgetc_char = 0;

	// last_recorded_len can be larger than last_vgetc_recorded_len
	// if peeking records more
	last_recorded_len -= last_vgetc_recorded_len;

	ପାଇଁ (;;)		// this is done twice if there are modifiers
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_inc = FALSE;

	    // No mapping after modifier has been read, using an input method
	    // and when a popup window has disabled mapping.
	    ଯଦି (mod_mask
#ଯଦି defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#endif
#ଯଦି defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#endif
		    )
	    {
		++no_mapping;
		++allow_ଚାବିଗୁଡ଼ିକ;
		// mod_mask value may change, remember we did the increment
		did_inc = TRUE;
	    }
	    c = vgetorpeek(TRUE);
	    ଯଦି (did_inc)
	    {
		--no_mapping;
		--allow_ଚାବିଗୁଡ଼ିକ;
	    }

	    // Get two extra bytes for special keys, handle modifiers.
	    ଯଦି (c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	       )
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_allow_ଚାବିଗୁଡ଼ିକ = allow_ଚାବିଗୁଡ଼ିକ;

		++no_mapping;
		allow_ଚାବିଗୁଡ଼ିକ = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_ଚାବିଗୁଡ଼ିକ = save_allow_ଚାବିଗୁଡ଼ିକ;
		ଯଦି (c2 == KS_MODIFIER)
		{
		    mod_mask = c;
		    ଜାରି;
		}
		c = TO_SPECIAL(c2, c);

		ଯଦି (allow_osc_ଚାବି == 0 && c == K_OSC)
		    ଜାରି;

		// K_ESC is used to avoid ambiguity with the single Esc
		// character that might be the start of an escape sequence.
		// Convert it back to a single Esc here.
		ଯଦି (c == K_ESC)
		    c = ESC;

#ଯଦି defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		// Handle K_TEAROFF here, the caller of vgetc() doesn't need to
		// know that a menu was torn off
		ଯଦି (
# ifdef VIMDLL
		    gui.in_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;

		    // get menu path, it ends with a <CR>
		    ପାଇଁ (j = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[j] = c;
			ଯଦି (j < 199)
			    ++j;
		    }
		    name[j] = NUL;
		    gui_make_tearoff(name);
		    ଜାରି;
		}
#endif
#ଯଦି defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && !defined(USE_GTK4) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		ଯଦି (c == K_F10 && gui.menubar != ଖାଲି)
		{
		    gtk_menu_shell_select_first(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    ଜାରି;
		}
#endif
#ifdef FEAT_GUI
		// Handle focus event here, so that the caller doesn't need to
		// know about it.  Return K_IGNORE so that we loop once (needed
		// if 'lazyredraw' is set).
		ଯଦି (c == K_FOCUSGAINED || c == K_FOCUSLOST)
		{
		    ui_focus_change(c == K_FOCUSGAINED);
		    c = K_IGNORE;
		}

		// Translate K_CSI to CSI.  The special key is only used to
		// avoid it being recognized as the start of a special key.
		ଯଦି (c == K_CSI)
		    c = CSI;
#endif
#ifdef FEAT_EVAL
		ଯଦି (c == K_SID)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    j;

		    // Handle <SID>{sid};  Do up to 20 digits for safety.
		    last_used_sid = 0;
		    ପାଇଁ (j = 0; j < 20 && SAFE_isdigit(c = vgetorpeek(TRUE)); ++j)
			last_used_sid = last_used_sid * 10 + (c - '0');
		    last_used_map = ଖାଲି;
		    ଜାରି;
		}
#endif
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    ଯଦି (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		ପାଇଁ (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    ଯଦି (buf[i] == K_SPECIAL
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
			ଯଦି (vgetorpeek(TRUE) == KE_CSI && c == KS_EXTRA)
			    buf[i] = CSI;
		    }
		}
		--no_mapping;
		c = (*mb_ptr2char)(buf);
	    }

	    ଯଦି (vgetc_char == 0)
	    {
		vgetc_mod_mask = mod_mask;
		vgetc_char = c;
	    }

	    // A keypad or special function key was not mapped, use it like
	    // its ASCII equivalent.
	    ବାଛ (c)
	    {
		ସ୍ଥିତି K_KPLUS:	c = '+'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_KMINUS:	c = '-'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_KDIVIDE:	c = '/'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_KMULTIPLY: c = '*'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_KENTER:	c = CAR; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_KPOINT:
#ifdef MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = Map_Virtual_ଚାବି(VK_DECIMAL, 2); ଭାଙ୍ଗ;
#ଅନ୍ୟଥା
				c = '.'; ଭାଙ୍ଗ;
#endif
		ସ୍ଥିତି K_K0:	c = '0'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_K1:	c = '1'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_K2:	c = '2'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_K3:	c = '3'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_K4:	c = '4'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_K5:	c = '5'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_K6:	c = '6'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_K7:	c = '7'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_K8:	c = '8'; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_K9:	c = '9'; ଭାଙ୍ଗ;

		ସ୍ଥିତି K_XHOME:
		ସ୍ଥିତି K_ZHOME:	ଯଦି (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_HOME;
				    mod_mask = 0;
				}
				ଅନ୍ୟଥା ଯଦି (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_HOME;
				    mod_mask = 0;
				}
				ଅନ୍ୟଥା
				    c = K_HOME;
				ଭାଙ୍ଗ;
		ସ୍ଥିତି K_XEND:
		ସ୍ଥିତି K_ZEND:	ଯଦି (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_END;
				    mod_mask = 0;
				}
				ଅନ୍ୟଥା ଯଦି (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_END;
				    mod_mask = 0;
				}
				ଅନ୍ୟଥା
				    c = K_END;
				ଭାଙ୍ଗ;

		ସ୍ଥିତି K_XUP:	c = K_UP; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_XDOWN:	c = K_DOWN; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_XLEFT:	c = K_ବାମ; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_XRIGHT:	c = K_ଡାହାଣ; ଭାଙ୍ଗ;
	    }

	    ଭାଙ୍ଗ;
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
    ଯଦି (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD
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
    ଯଦି ((!ex_normal_busy || in_feedkeys) && popup_do_filter(c))
    {
	ଯଦି (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#endif

#ifdef FEAT_EVAL
    c = do_ଚାବି_ନିବେଶ_pre(c);

    // Clear the next typedchars_pos
    typedchars_pos = 0;
#endif

    // Need to process the character before we know it's safe to do something
    // else.
    ଯଦି (c != K_IGNORE)
	state_no_longer_safe("key typed");

    ଫେରନ୍ତୁ c;
}

#ifdef FEAT_EVAL
/*
 * Handle the KeyInputPre autocommand.
 * "c" is the character that was typed.
 * Return new input character.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_ଚାବି_ନିବେଶ_pre(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = c;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    char_u	curr_mode[MODE_MAX_LENGTH];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_State = State;
    save_v_event_T save_v_event;
    dict_T	*v_event;

    // Return quickly when there is nothing to do.
    ଯଦି (!has_keyinputpre())
	ଫେରନ୍ତୁ res;

    ଯଦି (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = KEY2TERMCAP0(c);
	buf[2] = KEY2TERMCAP1(c);
	buflen = 3;
    }
    ଅନ୍ୟଥା
	buflen = (*mb_char2bytes)(c, buf);
    buf[buflen] = NUL;

    typedchars[typedchars_pos] = NUL;
    vim_unescape_csi(typedchars);

    get_mode(curr_mode);

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);  // set v:char

    v_event = get_v_event(&save_v_event);
    (ଶୂନ୍ୟ)dict_add_bool(v_event, "typed", ଚାବି_Typed);
    (ଶୂନ୍ୟ)dict_add_string_len(v_event, "typedchar", typedchars, typedchars_pos);

    ଯଦି (apply_autocmds(EVENT_KEYINPUTPRE, curr_mode, curr_mode, FALSE, curbuf))
    {
	string_T    v_char;

	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character.
	v_char.string = get_vim_var_str(VV_CHAR);
	ଯଦି (STRCMP(buf, v_char.string) != 0)
	{
	    v_char.length = STRLEN(v_char.string);
	    // Convert special bytes when it is special string.
	    ଯଦି (v_char.length >= 3 && v_char.string[0] == K_SPECIAL)
		res = TERMCAP2_ଚାବି(v_char.string[1], v_char.string[2]);
	    ଅନ୍ୟଥା ଯଦି (v_char.length > 0)
		res = PTR2CHAR(v_char.string);
	}
    }

    restore_v_event(v_event, &save_v_event);

    set_vim_var_string(VV_CHAR, ଖାଲି, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    ଫେରନ୍ତୁ res;
}
#endif

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
safe_vgetc(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c;

    c = vgetc();
    ଯଦି (c == NUL)
	c = get_keystroke();
    ଫେରନ୍ତୁ c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 * Does not handle bracketed paste - do not use the result for commands.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plain_vgetc_nopaste(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

    କର
	c = safe_vgetc();
    ଯେତେବେଳେ (c == K_IGNORE
	    || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
	    || c == K_MOUSEMOVE);
    ଫେରନ୍ତୁ c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plain_vgetc(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = plain_vgetc_nopaste();

    ଯଦି (c == K_PS)
	// Only handle the first pasted character.  Drop the rest, since we
	// don't know what to do with it.
	c = bracketed_paste(PASTE_ONE_CHAR, FALSE, ଖାଲି);

    ଫେରନ୍ତୁ c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 * Returns NUL if no character is available.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vpeekc(ଶୂନ୍ୟ)
{
    ଯଦି (can_get_old_char())
	ଫେରନ୍ତୁ old_char;
    ଫେରନ୍ତୁ vgetorpeek(FALSE);
}

#ଯଦି defined(FEAT_TERMRESPONSE) || defined(FEAT_TERMINAL)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vpeekc_nomap(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ++no_mapping;
    ++allow_ଚାବିଗୁଡ଼ିକ;
    c = vpeekc();
    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;
    ଫେରନ୍ତୁ c;
}
#endif

/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vpeekc_any(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    c = vpeekc();
    ଯଦି (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    ଫେରନ୍ତୁ c;
}

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
char_avail(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval;

#ifdef FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    ଯଦି (disable_char_avail_for_testing)
	ଫେରନ୍ତୁ FALSE;
#endif
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    ଫେରନ୍ତୁ (retval != NUL);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * "getchar()" and "getcharstr()" functions
 */
    ସ୍ଥିର ଶୂନ୍ୟ
getchar_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow_ସଂଖ୍ୟା)
{
    varnumber_T		n = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			called_emsg_start = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			simplify = TRUE;
    char_u		cursor_flag = 'm';

    ଯଦି ((in_vim9script()
		&& check_for_opt_bool_or_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	    || (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type == VAR_DICT)
    {
	dict_T		*d = argvars[1].vval.v_dict;
	char_u		*cursor_str;

	ଯଦି (allow_ସଂଖ୍ୟା)
	    allow_ସଂଖ୍ୟା = dict_get_bool(d, "number", TRUE);
	ଅନ୍ୟଥା ଯଦି (dict_has_ଚାବି(d, "number"))
	    semsg(_(e_invalid_argument_str), "number");

	simplify = dict_get_bool(d, "simplify", TRUE);

	cursor_str = dict_get_string(d, "cursor", FALSE);
	ଯଦି (cursor_str != ଖାଲି)
	{
	    ଯଦି (STRCMP(cursor_str, "hide") != 0
		    && STRCMP(cursor_str, "keep") != 0
		    && STRCMP(cursor_str, "msg") != 0)
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "cursor",
								   cursor_str);
	    ଅନ୍ୟଥା
		cursor_flag = cursor_str[0];
	}
    }

    ଯଦି (called_emsg != called_emsg_start)
	ଫେରନ୍ତୁ;

# ifdef MESSAGE_QUEUE
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
# endif

    ଯଦି (cursor_flag == 'h')
	cursor_sleep();
    ଅନ୍ୟଥା ଯଦି (cursor_flag == 'm')
	windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);

    ++no_mapping;
    ++allow_ଚାବିଗୁଡ଼ିକ;
    ++allow_osc_ଚାବି;
    ଯଦି (!simplify)
	++no_reduce_ଚାବିଗୁଡ଼ିକ;
    ପାଇଁ (;;)
    {
	ଯଦି (argvars[0].v_type == VAR_UNKNOWN
		|| (argvars[0].v_type == VAR_ସଂଖ୍ୟା
			&& argvars[0].vval.v_ସଂଖ୍ୟା == -1))
	    // getchar(): blocking wait.
	    n = plain_vgetc_nopaste();
	ଅନ୍ୟଥା ଯଦି (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	ଅନ୍ୟଥା ଯଦି (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	ଅନ୍ୟଥା
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = safe_vgetc();

	ଯଦି (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    ଜାରି;
	ଭାଙ୍ଗ;
    }
    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;
    --allow_osc_ଚାବି;
    ଯଦି (!simplify)
	--no_reduce_ଚାବିଗୁଡ଼ିକ;

    ଯଦି (cursor_flag == 'h')
	cursor_unsleep();

    set_vim_var_nr(VV_MOUSE_WIN, 0);
    set_vim_var_nr(VV_MOUSE_WINID, 0);
    set_vim_var_nr(VV_MOUSE_LNUM, 0);
    set_vim_var_nr(VV_MOUSE_COL, 0);

    ଯଦି (n != 0 && (!allow_ସଂଖ୍ୟା || IS_SPECIAL(n) || mod_mask != 0))
    {
	char_u		temp[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;

	// Turn a special key into three bytes, plus modifier.
	ଯଦି (mod_mask != 0)
	{
	    temp[i++] = K_SPECIAL;
	    temp[i++] = KS_MODIFIER;
	    temp[i++] = mod_mask;
	}
	ଯଦି (IS_SPECIAL(n))
	{
	    temp[i++] = K_SPECIAL;
	    temp[i++] = K_SECOND(n);
	    temp[i++] = K_THIRD(n);
	}
	ଅନ୍ୟଥା ଯଦି (has_mbyte)
	    i += (*mb_char2bytes)(n, temp + i);
	ଅନ୍ୟଥା
	    temp[i++] = n;
	temp[i] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strnsave(temp, i);

	ଯଦି (is_mouse_ଚାବି(n))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = mouse_row;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = mouse_col;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winnr = 1;

	    ଯଦି (row >= 0 && col >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_find_win(&row, &col, FIND_POPUP);
		ଯଦି (win == ଖାଲି)
		    ଫେରନ୍ତୁ;
		(ଶୂନ୍ୟ)mouse_comp_pos(win, &row, &col, &lnum, ଖାଲି);
# ifdef FEAT_PROP_POPUP
		ଯଦି (WIN_IS_POPUP(win))
		    winnr = 0;
		ଅନ୍ୟଥା
# endif
		    ପାଇଁ (wp = firstwin; wp != win && wp != ଖାଲି;
							       wp = wp->w_next)
			++winnr;
		set_vim_var_nr(VV_MOUSE_WIN, winnr);
		set_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		set_vim_var_nr(VV_MOUSE_LNUM, lnum);
		set_vim_var_nr(VV_MOUSE_COL, col + 1);
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (!allow_ସଂଖ୍ୟା)
	rettv->v_type = VAR_STRING;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = n;
}

/*
 * "getchar()" function
 */
    ଶୂନ୍ୟ
f_getchar(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, TRUE);
}

/*
 * "getcharstr()" function
 */
    ଶୂନ୍ୟ
f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, FALSE);
}

/*
 * "getcharmod()" function
 */
    ଶୂନ୍ୟ
f_getcharmod(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = mod_mask;
}
#endif // FEAT_EVAL

#ଯଦି defined(MESSAGE_QUEUE)
# define MAX_REPEAT_PARSE 8

/*
 * Process messages that have been queued for netbeans or clientserver.
 * Also check if any jobs have ended.
 * These functions can call arbitrary Vim script and should only be called when
 * it is safe to do so.
 */
    ଶୂନ୍ୟ
parse_queued_messages(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    old_curwin_id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    old_curbuf_fnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_may_garbage_collect = may_garbage_collect;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା entered = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    was_safe = get_was_safe_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    ଯଦି (updating_screen || dont_parse_messages)
	ଫେରନ୍ତୁ;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    ଯଦି (curbuf == ଖାଲି || curwin == ଖାଲି)
	ଫେରନ୍ତୁ;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    ପାଇଁ (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# ଯଦି (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_JOB_CHANNEL)
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
	channel_write_any_ଧାଡ଼ିଗୁଡ଼ିକ();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# ଯଦି defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_JOB_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	ଯଦି (job_check_ended())
	    ଜାରି;
# endif
# ifdef FEAT_TERMINAL
	free_unused_terminals();
# endif

# ifdef FEAT_SOUND_MACOSX
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	ଯଦି (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef SIGUSR1
	ଯଦି (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, ଖାଲି, ଖାଲି, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
# endif
	ଭାଙ୍ଗ;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    ଯଦି (entered == 1 && was_safe)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    ଯଦି (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}
#endif


ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
at_ins_compl_ଚାବି(ଶୂନ୍ୟ)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c = *p;

    ଯଦି (typebuf.tb_len > 3
	    && (c == K_SPECIAL || c == CSI)  // CSI is used by the GUI
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_MASK_CTRL)
	    // CTRL-SHIFT-N/P scroll the info popup, so they must not be folded
	    // to the CTRL-N/CTRL-P completion keys here.
	    && !(p[2] & MOD_MASK_SHIFT))
	c = p[3] & 0x1f;
    ଫେରନ୍ତୁ !ctrl_x_mode_eval()
	    && ((ctrl_x_mode_not_default() && vim_is_ctrl_x_ଚାବି(c))
		|| (compl_status_local() && (c == Ctrl_N || c == Ctrl_P)));
}

/*
 * Check if typebuf.tb_buf[] contains a modifier plus key that can be changed
 * into just a key, apply that.
 * Check from typebuf.tb_buf[typebuf.tb_off] to typebuf.tb_buf[typebuf.tb_off
 * + "max_offset"].
 * Return the length of the replaced bytes, 0 if nothing changed, -1 for error.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_simplify_modifier(ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_offset)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		offset;
    char_u	*tp;

    ପାଇଁ (offset = 0; offset < max_offset; ++offset)
    {
	ଯଦି (offset + 3 >= typebuf.tb_len)
	    ଭାଙ୍ଗ;
	tp = typebuf.tb_buf + typebuf.tb_off + offset;
	ଯଦି ((tp[0] == K_SPECIAL || tp[0] == CSI) && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା modifier = tp[2];
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c = tp[3];
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_c = merge_modify_Other_ଚାବିଗୁଡ଼ିକ(c, &modifier);

	    ଯଦି (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଚାବି_offset = offset;

		ଯଦି (offset == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_mask = tp[2];
		}
		ଯଦି (IS_SPECIAL(new_c))
		{
		    new_string[0] = K_SPECIAL;
		    new_string[1] = K_SECOND(new_c);
		    new_string[2] = K_THIRD(new_c);
		    len = 3;
		}
		ଅନ୍ୟଥା
		    len = mb_char2bytes(new_c, new_string);
		ଯଦି (modifier == 0)
		{
		    ଯଦି (put_string_in_typebuf(offset, 4, new_string, len,
							ଖାଲି, 0, ଖାଲି) == FAIL)
			ଫେରନ୍ତୁ -1;
		}
		ଅନ୍ୟଥା
		{
		    tp[2] = modifier;
		    ଚାବି_offset = offset + 3;
		    ଯଦି (put_string_in_typebuf(ଚାବି_offset, 1, new_string, len,
							ଖାଲି, 0, ଖାଲି) == FAIL)
			ଫେରନ୍ତୁ -1;
		}

		// A mapping for the simplified key can be used for it even
		// when the key protocol is enabled.
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; ++i)
		    typebuf.tb_noremap[typebuf.tb_off + ଚାବି_offset + i]
							    |= RM_SIMPLIFIED;
		ଫେରନ୍ତୁ len;
	    }
	}
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Return TRUE if the terminal sends modifiers with various keys.  This is when
 * modifyOtherKeys level 2 is enabled or the kitty keyboard protocol is
 * enabled.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଚାବି_protocol_enabled(ଶୂନ୍ୟ)
{
    // If xterm has responded to XTQMODKEYS it overrules seenModifyOtherKeys.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା using_mok = modify_otherkeys_state != MOKS_INITIAL
			? modify_otherkeys_state == MOKS_ENABLED
			: seen_Modify_Other_ଚାବିଗୁଡ଼ିକ;
    ଫେରନ୍ତୁ using_mok || kitty_protocol_state == KKPS_ENABLED;
}

/*
 * Handle mappings in the typeahead buffer.
 * - When something was mapped, return map_result_retry for recursive mappings.
 * - When nothing mapped and typeahead has a character: return map_result_get.
 * - When there is no match yet, return map_result_nomatch, need to get more
 *   typeahead.
 * - On failure (out of memory) return map_result_fail.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_mapping(
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *keylenp,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *timedout,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *mapdepth)
{
    mapblock_T	*mp = ଖାଲି;
    mapblock_T	*mp2;
    mapblock_T	*mp_match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mp_match_len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_mlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		want_termcode = 0;  // 1 if termcode expected after max_mlen
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tb_c1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mlen;
#ifdef FEAT_LANGMAP
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nolmaplen;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keylen = *keylenp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		local_State = get_real_state();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_plug_map = FALSE;
    bool	in_osc = in_osc_sequence(); // If we are in an OSC sequence,
					    // then go straight to
					    // check_termcode() in order to
					    // consume chars.

    // If typeahead starts with <Plug> then remap, even for a "noremap" mapping.
    ଯଦି (!in_osc && typebuf.tb_len >= 3
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
    ଯଦି (!in_osc && no_mapping == 0 && is_maphash_valid()
	    && (no_zero_mapping == 0 || tb_c1 != '0')
	    && (typebuf.tb_maplen == 0 || is_plug_map
		|| (p_remap
		    && (typebuf.tb_noremap[typebuf.tb_off]
				    & (RM_NONE|RM_ABBR)) == 0))
	    && !(p_paste && (State & (MODE_ଭର୍ତ୍ତି | MODE_CMDLINE)))
	    && !(State == MODE_HITRETURN && (tb_c1 == CAR || tb_c1 == ' '))
	    && State != MODE_ASKMORE
	    && State != MODE_CONFIRM
	    && !at_ins_compl_ଚାବି())
    {
#ifdef FEAT_GUI
	ଯଦି (gui.in_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#endif
#ifdef FEAT_LANGMAP
	ଯଦି (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	ଅନ୍ୟଥା
	{
	    LANGMAP_ADJUST(tb_c1, (State & (MODE_CMDLINE | MODE_ଭର୍ତ୍ତି)) == 0
					   && get_real_state() != MODE_SELECT);
	    nolmaplen = 0;
	}
#endif
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	ଯଦି (mp == ଖାଲି)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = ଖାଲି;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_match = ଖାଲି;
	mp_match_len = 0;
	ପାଇଁ ( ; mp != ଖାଲି;
	       mp->m_next == ଖାଲି ? (mp = mp2, mp2 = ଖାଲି) : (mp = mp->m_next))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    ଯଦି (mp->m_ଚାବିଗୁଡ଼ିକ[0] == tb_c1
		    && (mp->m_mode & local_State)
		    && !(mp->m_simplified && ଚାବି_protocol_enabled()
			    && typebuf.tb_maplen == 0
			    && (typebuf.tb_noremap[typebuf.tb_off]
							& RM_SIMPLIFIED) == 0)
		    && ((mp->m_mode & MODE_LANGMAP) == 0
						    || typebuf.tb_maplen == 0))
	    {
#ifdef FEAT_LANGMAP
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nomap = nolmaplen;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	modifiers = 0;
#endif
		// find the match length of this mapping
		ପାଇଁ (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
#ifdef FEAT_LANGMAP
		    ଯଦି (nomap > 0)
		    {
			ଯଦି (nomap == 2 && c2 == KS_MODIFIER)
			    modifiers = 1;
			ଅନ୍ୟଥା ଯଦି (nomap == 1 && modifiers == 1)
			    modifiers = c2;
			--nomap;
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (c2 == K_SPECIAL)
			    nomap = 2;
			ଅନ୍ୟଥା ଯଦି (merge_modify_Other_ଚାବିଗୁଡ଼ିକ(c2, &modifiers) == c2)
			    // Only apply 'langmap' if merging modifiers into
			    // the key will not result in another character,
			    // so that 'langmap' behaves consistently in
			    // different terminals and GUIs.
			    LANGMAP_ADJUST(c2, TRUE);
			modifiers = 0;
		    }
#endif
		    ଯଦି (mp->m_ଚାବିଗୁଡ଼ିକ[mlen] != c2)
			ଭାଙ୍ଗ;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_ଚାବିଗୁଡ଼ିକ;
		    char_u *p2 = mb_unescape(&p1);

		    ଯଦି (has_mbyte && p2 != ଖାଲି
					&& MB_BYTE2LEN(tb_c1) > mb_ptr2len(p2))
			mlen = 0;
		}

		// Check an entry whether it matches.
		// - Full match: mlen == keylen
		// - Partly match: mlen == typebuf.tb_len
		keylen = mp->m_keylen;
		ଯଦି (mlen == keylen || (mlen == typebuf.tb_len
						   && typebuf.tb_len < keylen))
		{
		    char_u  *s;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n;

		    // If only script-local mappings are allowed, check if the
		    // mapping starts with K_SNR.
		    s = typebuf.tb_noremap + typebuf.tb_off;
		    ଯଦି ((*s & ~RM_SIMPLIFIED) == RM_SCRIPT
			    && (mp->m_ଚାବିଗୁଡ଼ିକ[0] != K_SPECIAL
				|| mp->m_ଚାବିଗୁଡ଼ିକ[1] != KS_EXTRA
				|| mp->m_ଚାବିଗୁଡ଼ିକ[2] != KE_SNR))
			ଜାରି;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    ପାଇଁ (n = mlen; --n >= 0; )
			ଯଦି (*s++ & (RM_NONE|RM_ABBR))
			    ଭାଙ୍ଗ;
		    ଯଦି (!is_plug_map && n >= 0)
			ଜାରି;

		    ଯଦି (keylen > typebuf.tb_len)
		    {
			ଯଦି (!*timedout && !(mp_match != ଖାଲି
							&& mp_match->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    ଭାଙ୍ଗ;
			}
		    }
		    ଅନ୍ୟଥା ଯଦି (keylen > mp_match_len)
		    {
			// found a longer match
			mp_match = mp;
			mp_match_len = keylen;
		    }
		}
		ଅନ୍ୟଥା
		{
		    // No match; may have to check for termcode at next
		    // character.

		    // If the first character that didn't match is
		    // K_SPECIAL then check for a termcode.  This isn't perfect
		    // but should work in most cases.
		    ଯଦି (max_mlen < mlen)
		    {
			max_mlen = mlen;
			want_termcode = mp->m_ଚାବିଗୁଡ଼ିକ[mlen] == K_SPECIAL;
		    }
		    ଅନ୍ୟଥା ଯଦି (max_mlen == mlen && mp->m_ଚାବିଗୁଡ଼ିକ[mlen] == K_SPECIAL)
			want_termcode = 1;

		    // Check termcode for uppercase character to properly
		    // process "ESC[27;2;<ascii code>~" control sequences.
		    ଯଦି (ASCII_ISUPPER(mp->m_ଚାବିଗୁଡ଼ିକ[mlen]))
			want_termcode = 1;
		}
	    }
	}

	// If no partly match found, use the longest full match.
	ଯଦି (keylen != KEYLEN_PART_MAP && mp_match != ଖାଲି)
	{
	    mp = mp_match;
	    keylen = mp_match_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    ଯଦି (!in_osc && *p_pt != NUL && mp == ଖାଲି &&
	    (State & (MODE_ଭର୍ତ୍ତି | MODE_NORMAL)))
    {
	ପାଇଁ (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    ଯଦି (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    ଭାଙ୍ଗ;
	ଯଦି (p_pt[mlen] == NUL)	// match
	{
	    // write chars to script file(s)
	    ଯଦି (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    set_option_ମୂଲ୍ୟ_give_err((char_u *)"paste",
						      (long)!p_paste, ଖାଲି, 0);
	    ଯଦି (!(State & MODE_ଭର୍ତ୍ତି))
	    {
		ସନ୍ଦେଶ_col = 0;
		ସନ୍ଦେଶ_row = Rows - 1;
		ସନ୍ଦେଶ_clr_eos();		// clear ruler
	    }
	    status_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    ଫେରନ୍ତୁ map_result_retry;
	}
	// Need more chars for partly match.
	ଯଦି (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_ଚାବି;
	ଅନ୍ୟଥା ଯଦି (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    // May check for a terminal code when there is no mapping or only a partial
    // mapping.  Also check if there is a full mapping with <Esc>, unless timed
    // out, since that is nearly always a partial match with a terminal code.
    ଯଦି (in_osc || ((mp == ଖାଲି || max_mlen + want_termcode > mp_match_len
		    || (mp_match_len == 1 && *mp->m_ଚାବିଗୁଡ଼ିକ == ESC && !*timedout))
	    && keylen != KEYLEN_PART_MAP))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_keylen = keylen;

	/*
	 * When no matching mapping found or found a non-matching mapping that
	 * matches at least what the matching mapping matched:
	 * Check if we have a terminal code, when:
	 * - mapping is allowed,
	 * - keys have not been mapped,
	 * - and not an ESC sequence, not in insert mode or p_ek is on,
	 * - and when not timed out,
	 */
	ଯଦି (in_osc || no_mapping == 0 || allow_ଚାବିଗୁଡ଼ିକ != 0)
	{
	    ଯଦି (in_osc || ((typebuf.tb_maplen == 0
			    || (p_remap && (typebuf.tb_noremap[typebuf.tb_off]
						& ~RM_SIMPLIFIED) == RM_YES))
		    && !*timedout))
		keylen = check_termcode(max_mlen + 1, ଖାଲି, 0, ଖାଲି);
	    ଅନ୍ୟଥା
		keylen = 0;

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    ଯଦି (keylen == 0 && save_keylen == KEYLEN_PART_ଚାବି && !*timedout)
		keylen = KEYLEN_PART_ଚାବି;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
#ifdef FEAT_TERMINAL
	    check_no_reduce_ଚାବିଗୁଡ଼ିକ();  // may update the no_reduce_keys flag
#endif
	    ଯଦି (keylen == 0 && !no_reduce_ଚାବିଗୁଡ଼ିକ)
	    {
		keylen = check_simplify_modifier(max_mlen + 1);
		ଯଦି (keylen < 0)
		    // ins_typebuf() failed
		    ଫେରନ୍ତୁ map_result_fail;
	    }

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    ଯଦି (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	ଅନ୍ୟଥା
	    keylen = 0;
	ଯଦି (keylen == 0)	// no matching terminal code
	{
#ifdef AMIGA
	    // check for window bounds report
	    ଯଦି (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		ପାଇଁ (s = typebuf.tb_buf + typebuf.tb_off + 1;
			   s < typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len
		   && (VIM_ISDIGIT(*s) || *s == ';' || *s == ' ');
			++s)
		    ;
		ଯଦି (*s == 'r' || *s == '|') // found one
		{
		    del_typebuf(
			  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s + 1 - (typebuf.tb_buf + typebuf.tb_off)), 0);
		    // get size and redraw screen
		    shell_resized();
		    *keylenp = keylen;
		    ଫେରନ୍ତୁ map_result_retry;
		}
		ଯଦି (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_ଚାବି;
	    }
	    ଯଦି (keylen >= 0)
#endif
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		ଯଦି (mp == ଖାଲି)
		{
		    *keylenp = keylen;
		    ଫେରନ୍ତୁ map_result_get;    // get character from typeahead
		}
	}

	ଯଦି (keylen > 0)	    // full matching terminal code
	{
#ଯଦି defined(FEAT_GUI) && defined(FEAT_MENU)
	    ଯଦି (typebuf.tb_len >= 2
		    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
			      && typebuf.tb_buf[typebuf.tb_off + 1] == KS_MENU)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx;

		// Using a menu may cause a break in undo!  It's like using
		// gotchars(), but without recording or writing to a script
		// file.
		may_sync_undo();
		del_typebuf(3, 0);
		idx = get_menu_index(current_menu, local_State);
		ଯଦି (idx != MENU_INDEX_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    ଯଦି (VIsual_active && VIsual_select
					&& (current_menu->modes & MODE_VISUAL))
		    {
			VIsual_select = FALSE;
			(ଶୂନ୍ୟ)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->strings[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#endif // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    ଫେରନ୍ତୁ map_result_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	ଯଦି (mp == ଖାଲି || keylen < 0)
	    keylen = KEYLEN_PART_ଚାବି;
	ଅନ୍ୟଥା
	    keylen = mp_match_len;
    }

    /*
     * complete match
     */
    ଯଦି (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#ifdef FEAT_EVAL
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_m_ପଦସମଷ୍ଟି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_m_noremap;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_m_silent;
	char_u	*save_m_ଚାବିଗୁଡ଼ିକ;
	char_u	*save_alt_m_ଚାବିଗୁଡ଼ିକ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_alt_m_keylen;
#ଅନ୍ୟଥା
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#endif

	// write chars to script file(s)
	ଯଦି (keylen > typebuf.tb_maplen)
	    gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						   keylen - typebuf.tb_maplen);

	cmd_silent = (typebuf.tb_silent > 0);
	del_typebuf(keylen, 0);	// remove the mapped keys

	/*
	 * Put the replacement string in front of mapstr.
	 * The depth check catches ":map x y" and ":map y x".
	 */
	ଯଦି (++*mapdepth >= p_mmd)
	{
	    emsg(_(e_recursive_mapping));
	    ଯଦି (State & MODE_CMDLINE)
		redrawcmdline();
	    ଅନ୍ୟଥା
		setcursor();
	    flush_buffers(FLUSH_MINIMAL);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    ଫେରନ୍ତୁ map_result_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	ଯଦି (VIsual_active && VIsual_select && (mp->m_mode & MODE_VISUAL))
	{
	    VIsual_select = FALSE;
	    (ଶୂନ୍ୟ)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#ifdef FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_ପଦସମଷ୍ଟି = mp->m_ପଦସମଷ୍ଟି;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_ଚାବିଗୁଡ଼ିକ = ଖାଲି;  // only saved when needed
	save_alt_m_ଚାବିଗୁଡ଼ିକ = ଖାଲି;  // only saved when needed
	save_alt_m_keylen = mp->m_alt != ଖାଲି ? mp->m_alt->m_keylen : 0;

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	ଯଦି (mp->m_ପଦସମଷ୍ଟି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_vgetc_busy = vgetc_busy;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_may_garbage_collect = may_garbage_collect;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା was_screen_col = screen_cur_col;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା was_screen_row = screen_cur_row;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_did_emsg = did_emsg;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_ଚାବିଗୁଡ଼ିକ = vim_strnsave(mp->m_ଚାବିଗୁଡ଼ିକ, (size_t)mp->m_keylen);
	    save_alt_m_ଚାବିଗୁଡ଼ିକ = mp->m_alt != ଖାଲି
				    ? vim_strnsave(mp->m_alt->m_ଚାବିଗୁଡ଼ିକ,
					     (size_t)save_alt_m_keylen) : ଖାଲି;
	    map_str = eval_map_ପଦସମଷ୍ଟି(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_row, was_screen_col);
	    out_flush();

	    // If an error was displayed and the expression returns an empty
	    // string, generate a <Nop> to allow for a redraw.
	    ଯଦି (prev_did_emsg != did_emsg
				       && (map_str == ଖାଲି || *map_str == NUL))
	    {
		char_u	buf[4];

		vim_free(map_str);
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = KE_IGNORE;
		buf[3] = NUL;
		map_str = vim_strnsave(buf, 3);
		ଯଦି (State & MODE_CMDLINE)
		{
		    // redraw the command below the error
		    ସନ୍ଦେଶ_didout = TRUE;
		    ଯଦି (ସନ୍ଦେଶ_row < cmdline_row)
			ସନ୍ଦେଶ_row = cmdline_row;
		    redrawcmd();
		}
	    }

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	ଅନ୍ୟଥା
#endif
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	ଯଦି (map_str == ଖାଲି)
	    i = FAIL;
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା noremap;

#ifdef FEAT_EVAL
	    last_used_map = mp;
	    last_used_sid = -1;
#endif
	    ଯଦି (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    ଅନ୍ୟଥା ଯଦି (
#ifdef FEAT_EVAL
		save_m_ପଦସମଷ୍ଟି ?
		(save_m_ଚାବିଗୁଡ଼ିକ != ଖାଲି
			&& STRNCMP(map_str, save_m_ଚାବିଗୁଡ଼ିକ, (size_t)keylen) == 0)
		|| (save_alt_m_ଚାବିଗୁଡ଼ିକ != ଖାଲି
			&& STRNCMP(map_str, save_alt_m_ଚାବିଗୁଡ଼ିକ,
					    (size_t)save_alt_m_keylen) == 0) :
#endif
		STRNCMP(map_str, mp->m_ଚାବିଗୁଡ଼ିକ, (size_t)keylen) == 0
		|| (mp->m_alt != ଖାଲି
			&& STRNCMP(map_str, mp->m_alt->m_ଚାବିଗୁଡ଼ିକ,
					    (size_t)mp->m_alt->m_keylen) == 0))
		noremap = REMAP_SKIP;
	    ଅନ୍ୟଥା
		noremap = REMAP_YES;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#ifdef FEAT_EVAL
	    ଯଦି (save_m_ପଦସମଷ୍ଟି)
		vim_free(map_str);
#endif
	}
#ifdef FEAT_EVAL
	vim_free(save_m_ଚାବିଗୁଡ଼ିକ);
	vim_free(save_alt_m_ଚାବିଗୁଡ଼ିକ);
#endif
	*keylenp = keylen;
	ଯଦି (i == FAIL)
	    ଫେରନ୍ତୁ map_result_fail;
	ଫେରନ୍ତୁ map_result_retry;
    }

    *keylenp = keylen;
    ଫେରନ୍ତୁ map_result_nomatch;
}

/*
 * unget one character (can only be done once!)
 * If the character was stuffed, vgetc() will get it next time it is called.
 * Otherwise vgetc() will only get it when the stuff buffer is empty.
 */
    ଶୂନ୍ୟ
vungetc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    old_char = c;
    old_mod_mask = mod_mask;
    old_mouse_row = mouse_row;
    old_mouse_col = mouse_col;
    old_ଚାବି_Stuffed = ଚାବି_Stuffed;
}

/*
 * When peeking and not getting a character, reg_executing cannot be cleared
 * yet, so set a flag to clear it later.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
check_end_reg_executing(ପୂର୍ଣ୍ଣସଂଖ୍ୟା advance)
{
    ଯଦି (reg_executing != 0 && (typebuf.tb_maplen == 0
						|| pending_end_reg_executing))
    {
	ଯଦି (advance)
	{
	    reg_executing = 0;
	    pending_end_reg_executing = FALSE;
	}
	ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vgetorpeek(ପୂର୍ଣ୍ଣସଂଖ୍ୟା advance)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timedout = FALSE;	// waited for more than 'timeoutlen'
					// for mapping to complete or
					// 'ttimeoutlen' for complete key code
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mapdepth = 0;		// check for recursive mapping
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode_deleted = FALSE;   // set when mode has been deleted
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_wcol, new_wrow;
#ifdef FEAT_GUI
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		shape_changed = FALSE;  // adjusted cursor shape
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_wcol, old_wrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wait_tb_len;

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
    ଯଦି (vgetc_busy > 0 && ex_normal_busy == 0)
	ଫେରନ୍ତୁ NUL;

    ++vgetc_busy;

    ଯଦି (advance)
    {
	ଚାବି_Stuffed = FALSE;
	typebuf_was_empty = FALSE;
    }

    init_typebuf();
    start_stuff();
    check_end_reg_executing(advance);
    କର
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	ଯଦି (typeahead_char != 0)
	{
	    c = typeahead_char;
	    ଯଦି (advance)
		typeahead_char = 0;
	}
	ଅନ୍ୟଥା
	    c = read_readbuffers(advance);
	ଯଦି (c != NUL && !got_int)
	{
	    ଯଦି (advance)
	    {
		// KeyTyped = FALSE;  When the command that stuffed something
		// was typed, behave like the stuffed command was typed.
		// needed for CTRL-W CTRL-] to open a fold, for example.
		ଚାବି_Stuffed = TRUE;
	    }
	    ଯଦି (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	// no abbreviations now
	}
	ଅନ୍ୟଥା
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    ପାଇଁ (;;)
	    {
		long	wait_time;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	keylen = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	showcmd_idx;
		check_end_reg_executing(advance);
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		ଯଦି (typebuf.tb_maplen)
		    ଧାଡ଼ି_breakcheck();
		ଅନ୍ୟଥା
		    ui_breakcheck();		// check for CTRL-C
		ଯଦି (got_int)
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
		    ଯଦି ((c || typebuf.tb_maplen)
				     && (State & (MODE_ଭର୍ତ୍ତି | MODE_CMDLINE)))
			c = ESC;
		    ଅନ୍ୟଥା
			c = Ctrl_C;
		    flush_buffers(FLUSH_ନିବେଶ);	// flush all typeahead

		    ଯଦି (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା ଯଦି (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_result_T result = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    ଯଦି (result == map_result_retry)
			// try mapping again
			ଜାରି;
		    ଯଦି (result == map_result_fail)
		    {
			// failed, use the outer loop
			c = -1;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (result == map_result_get)
		    {
/*
 * get a character: 2. from the typeahead buffer
 */
			c = typebuf.tb_buf[typebuf.tb_off];
			ଯଦି (advance)	// remove chars from typebuf
			{
			    cmd_silent = (typebuf.tb_silent > 0);
			    ଯଦି (typebuf.tb_maplen > 0)
				ଚାବି_Typed = FALSE;
			    ଅନ୍ୟଥା
			    {
				ଚାବି_Typed = TRUE;
				// write char to script file(s)
				gotchars(typebuf.tb_buf
						 + typebuf.tb_off, 1);
			    }
			    ଚାବି_Noremap = typebuf.tb_noremap[typebuf.tb_off]
							    & ~RM_SIMPLIFIED;
			    del_typebuf(1, 0);
			}
			ଭାଙ୍ଗ;  // got character, break the for loop
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
		ଯଦି (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& kitty_protocol_state != KKPS_ENABLED
			&& ex_normal_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & MODE_ଭର୍ତ୍ତି)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_ଚାବି && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	col = 0;
		    char_u	*ptr;

		    ଯଦି (mode_displayed)
		    {
			unshowmode(TRUE);
			mode_deleted = TRUE;
		    }
#ifdef FEAT_GUI
		    // may show a different cursor shape
		    ଯଦି (gui.in_use && State != MODE_NORMAL && !cmd_silent)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_State;

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
		    ଯଦି (curwin->w_cursor.col != 0)
		    {
			ଯଦି (curwin->w_wcol > 0)
			{
			    // After auto-indenting and no text is following,
			    // we are expecting to truncate the trailing
			    // white-space, so find the last non-white
			    // character -- webb
			    ଯଦି (did_ai && *skipwhite(ml_get_curline()
						+ curwin->w_cursor.col) == NUL)
			    {
				chartabsize_T cts;

				curwin->w_wcol = 0;
				ptr = ml_get_curline();
				init_chartabsize_arg(&cts, curwin,
					  curwin->w_cursor.lnum, 0, ptr, ptr);
				ଯେତେବେଳେ (cts.cts_ptr < ptr + curwin->w_cursor.col)
				{
				    ଯଦି (!VIM_ISWHITE(*cts.cts_ptr))
					curwin->w_wcol = cts.cts_vcol;
				    cts.cts_vcol += lbr_chartabsize(&cts);
				    ଯଦି (has_mbyte)
					cts.cts_ptr +=
						   (*mb_ptr2len)(cts.cts_ptr);
				    ଅନ୍ୟଥା
					++cts.cts_ptr;
				}
				clear_chartabsize_arg(&cts);

				curwin->w_wrow = curwin->w_cline_row
					   + curwin->w_wcol / curwin->w_width;
				curwin->w_wcol %= curwin->w_width;
				curwin->w_wcol += curwin_col_off();
				col = 0;	// no correction needed
			    }
			    ଅନ୍ୟଥା
			    {
				--curwin->w_wcol;
				col = curwin->w_cursor.col - 1;
			    }
			}
			ଅନ୍ୟଥା ଯଦି (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_width - 1;
			    col = curwin->w_cursor.col - 1;
			}
			ଯଦି (has_mbyte && col > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    col -= (*mb_head_off)(ptr, ptr + col);
			    ଯଦି ((*mb_ptr2cells)(ptr + col) > 1)
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
		ଯଦି (c < 0)
		    ଜାରି;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		ପାଇଁ (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		ଯଦି (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    ଜାରି;
		}

		ଯଦି (ex_normal_busy > 0)
		{
		    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tc = 0;

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    ଯଦି (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			ଜାରି;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    ଯଦି (p_im && (State & MODE_ଭର୍ତ୍ତି))
			c = Ctrl_L;
#ifdef FEAT_TERMINAL
		    ଅନ୍ୟଥା ଯଦି (terminal_is_active())
			c = K_CANCEL;
#endif
		    ଅନ୍ୟଥା ଯଦି ((State & MODE_CMDLINE)
					     || (cmdwin_type > 0 && tc == ESC))
			c = Ctrl_C;
		    ଅନ୍ୟଥା
			c = ESC;
		    tc = c;
		    // set a flag to indicate this wasn't a normal char
		    ଯଦି (advance)
			typebuf_was_empty = TRUE;

		    // return from main_loop()
		    ଯଦି (pending_exmode_active)
			exmode_active = EXMODE_NORMAL;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    ଭାଙ୍ଗ;
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
		ଯଦି (((State & MODE_ଭର୍ତ୍ତି) != 0 || p_lz)
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
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା showing_partial = FALSE;
		ଯଦି (typebuf.tb_len > 0 && advance && !exmode_active)
		{
		    ଯଦି (((State & (MODE_NORMAL | MODE_ଭର୍ତ୍ତି))
						      || State == MODE_LANGMAP)
			    && State != MODE_HITRETURN)
		    {
			// this looks nice when typing a dead character map
			ଯଦି (State & MODE_ଭର୍ତ୍ତି
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
			ଠେଲ_showcmd();
			ଯଦି (typebuf.tb_len > SHOWCMD_COLS)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_COLS;
			ଯେତେବେଳେ (showcmd_idx < typebuf.tb_len)
			    add_byte_to_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
		    }

		    // This looks nice when typing a dead character map.
		    // There is no actual command line for get_number().
		    ଯଦି ((State & MODE_CMDLINE)
			    && get_cmdline_info()->cmdbuff != ଖାଲି
#ଯଦି defined(FEAT_CRYPT) || defined(FEAT_EVAL)
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
		ଯଦି (typebuf.tb_len == 0)
		    // timedout may have been set if a mapping with empty RHS
		    // fully matched while longer mappings timed out.
		    timedout = FALSE;

		ଯଦି (advance)
		{
		    ଯଦି (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_ଚାବି)))
			// blocking wait
			wait_time = -1L;
		    ଅନ୍ୟଥା ଯଦି (keylen == KEYLEN_PART_ଚାବି && p_ttm >= 0)
			wait_time = p_ttm;
		    ଅନ୍ୟଥା
			wait_time = p_tm;
		}
		ଅନ୍ୟଥା
		    wait_time = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_time);

		ଯଦି (showcmd_idx != 0)
		    କାଢ଼_showcmd();
		ଯଦି (showing_partial)
		{
		    ଯଦି (State & MODE_ଭର୍ତ୍ତି)
			edit_unputchar();
		    ଯଦି ((State & MODE_CMDLINE)
					&& get_cmdline_info()->cmdbuff != ଖାଲି)
			unputcmdline();
		    ଅନ୍ୟଥା
			setcursor();	// put cursor back where it belongs
		}

		ଯଦି (c < 0)
		    ଜାରି;		// end of input script reached
		ଯଦି (c == NUL)		// no character available
		{
		    ଯଦି (!advance)
			ଭାଙ୍ଗ;
		    ଯଦି (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			ଜାରି;
		    }
		}
		ଅନ୍ୟଥା
		{	    // allow mapping for just typed characters
		    ଯେତେବେଳେ (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#ifdef HAVE_ନିବେଶ_METHOD
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_active = im_get_status();
#endif
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } ଯେତେବେଳେ ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    ଯଦି (advance && p_smd && ସନ୍ଦେଶ_silent == 0 && (State & MODE_ଭର୍ତ୍ତି))
    {
	ଯଦି (c == ESC && !mode_deleted && !no_mapping && mode_displayed)
	{
	    ଯଦି (typebuf.tb_len && !ଚାବି_Typed)
		redraw_cmdline = TRUE;	    // delete mode later
	    ଅନ୍ୟଥା
		unshowmode(FALSE);
	}
	ଅନ୍ୟଥା ଯଦି (c != ESC && mode_deleted)
	{
	    ଯଦି (typebuf.tb_len && !ଚାବି_Typed)
		redraw_cmdline = TRUE;	    // show mode later
	    ଅନ୍ୟଥା
		showmode();
	}
    }
#ifdef FEAT_GUI
    // may unshow different cursor shape
    ଯଦି (gui.in_use && shape_changed)
	gui_update_cursor(TRUE, FALSE);
#endif
    ଯଦି (timedout && c == ESC)
    {
	// When recording there will be no timeout.  Add an <Ignore> after the
	// ESC to avoid that it forms a key code with following characters.
	gotchars_ignore();
    }

    --vgetc_busy;

    ଫେରନ୍ତୁ c;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
inchar(
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen,
    long	wait_time)	    // milliseconds
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;	    // init for GCC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retesc = FALSE;	    // return ESC with gotint
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		script_char;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tb_change_cnt = typebuf.tb_change_cnt;

    ଯଦି (wait_time == -1L || wait_time > 100L)  // flush output before waiting
    {
	cursor_on();
	out_flush_cursor(FALSE, FALSE);
#ଯଦି defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	ଯଦି (gui.in_use && postponed_mouseshape)
	    update_mouseshape(-1);
#endif
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    ଯଦି (State != MODE_HITRETURN)
    {
	did_outofmem_ସନ୍ଦେଶ = FALSE;   // display out of memory message (again)
	did_swapwrite_ସନ୍ଦେଶ = FALSE;  // display swap file write error again
    }
    undo_off = FALSE;		    // restart undo now

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    ଯେତେବେଳେ (scriptin[curscript] != ଖାଲି && script_char < 0
#ifdef FEAT_EVAL
	    && !ignore_script
#endif
	    )
    {
#ifdef MESSAGE_QUEUE
	parse_queued_messages();
#endif

	ଯଦି (got_int || (script_char = getc(scriptin[curscript])) < 0)
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
	    ଯଦି (got_int)
		retesc = TRUE;
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    ଯଦି (script_char < 0)	// did not get a character from script
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	ଯଦି (got_int)
	{
#define DUM_LEN (MAXMAPLEN * 3 + 3)
	    char_u	dum[DUM_LEN + 1];

	    ପାଇଁ (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		ଯଦି (len == 0 || (len == 1 && dum[0] == Ctrl_C))
		    ଭାଙ୍ଗ;
	    }
	    ଫେରନ୍ତୁ retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	ଯଦି (wait_time == -1L || wait_time > 10L)
	    out_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_time, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    ଯଦି (typebuf_changed(tb_change_cnt))
	ଫେରନ୍ତୁ 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    ଯଦି (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    ଫେରନ୍ତୁ fix_ନିବେଶ_buffer(buf, len);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * "buf[]" must have room to triple the number of bytes!
 * Returns the new length.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fix_ନିବେଶ_buffer(char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p = buf;

    /*
     * Two characters are special: NUL and K_SPECIAL.
     * When compiled With the GUI CSI is also special.
     * Replace	     NUL by K_SPECIAL KS_ZERO	 KE_FILLER
     * Replace K_SPECIAL by K_SPECIAL KS_SPECIAL KE_FILLER
     * Replace       CSI by K_SPECIAL KS_EXTRA   KE_CSI
     */
    ପାଇଁ (i = len; --i >= 0; ++p)
    {
#ifdef FEAT_GUI
	// When the GUI is used any character can come after a CSI, don't
	// escape it.
	ଯଦି (gui.in_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
# ifndef MSWIN
	// When not on MS-Windows and the GUI is not used CSI needs to be
	// escaped.
	ଅନ୍ୟଥା ଯଦି (!gui.in_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
	    len += 2;
	}
# endif
	ଅନ୍ୟଥା
#endif
	ଯଦି (p[0] == NUL || (p[0] == K_SPECIAL
		    // timeout may generate K_CURSORHOLD,
		    // 'autocompletedelay' timeout K_COMPLETE_DELAY
		    && (i < 2 || p[1] != KS_EXTRA
			|| (p[2] != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CURSORHOLD
			    && p[2] != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_COMPLETE_DELAY))
#ଯଦି defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
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
    ଫେରନ୍ତୁ len;
}

#ଯଦି defined(USE_ନିବେଶ_BUF)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ନିବେଶ_available(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (!vim_is_ନିବେଶ_buf_empty()
# ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#endif

/*
 * Function passed to do_cmdline() to get the command after a <Cmd> key from
 * typeahead.
 */
    ସ୍ଥିର char_u *
getcmdkeycmd(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		promptc UNUSED,
	ଶୂନ୍ୟ		*cookie UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent UNUSED,
	getline_opt_T	do_concat UNUSED)
{
    garray_T	ଧାଡ଼ି_ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1 = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmod = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		aborted = FALSE;

    ga_init2(&ଧାଡ଼ି_ga, 1, 32);

    // no mapping for these characters
    no_mapping++;

    got_int = FALSE;
    ଯେତେବେଳେ (c1 != NUL && !aborted)
    {
	ଯଦି (ga_grow(&ଧାଡ଼ି_ga, 32) == FAIL)
	{
	    aborted = TRUE;
	    ଭାଙ୍ଗ;
	}

	ଯଦି (vgetorpeek(FALSE) == NUL)
	{
	    // incomplete <Cmd> is an error, because there is not much the user
	    // could do in this state.
	    emsg(_(e_cmd_mapping_must_end_with_cr));
	    aborted = TRUE;
	    ଭାଙ୍ଗ;
	}

	// Get one character at a time.
	c1 = vgetorpeek(TRUE);

	// Get two extra bytes for special keys
	ଯଦି (c1 == K_SPECIAL)
	{
	    c1 = vgetorpeek(TRUE);
	    c2 = vgetorpeek(TRUE);
	    ଯଦି (c1 == KS_MODIFIER)
	    {
		cmod = c2;
		ଜାରି;
	    }
	    c1 = TO_SPECIAL(c1, c2);

	    // K_ESC is used to avoid ambiguity with the single Esc character
	    // that might be the start of an escape sequence.  Convert it back
	    // to a single Esc here.
	    ଯଦି (c1 == K_ESC)
		c1 = ESC;

#ifdef FEAT_GUI
	    // Translate K_CSI to CSI.  The special key is only used to
	    // avoid it being recognized as the start of a special key.
	    ଯଦି (c1 == K_CSI)
		c1 = CSI;
#endif
	}

	ଯଦି (got_int)
	    aborted = TRUE;
	ଅନ୍ୟଥା ଯଦି (c1 == '\r' || c1 == '\n')
	    c1 = NUL;  // end the line
	ଅନ୍ୟଥା ଯଦି (c1 == ESC)
	    aborted = TRUE;
	ଅନ୍ୟଥା ଯଦି (c1 == K_COMMAND || c1 == K_SCRIPT_COMMAND)
	{
	    // give a nicer error message for this special case
	    emsg(_(e_cmd_mapping_must_end_with_cr_before_second_cmd));
	    aborted = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (c1 == K_SNR)
	{
	    GA_CONCAT_LITERAL(&ଧାଡ଼ି_ga, "<SNR>");
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (cmod != 0)
	    {
		ga_append(&ଧାଡ଼ି_ga, K_SPECIAL);
		ga_append(&ଧାଡ଼ି_ga, KS_MODIFIER);
		ga_append(&ଧାଡ଼ି_ga, cmod);
	    }
	    ଯଦି (IS_SPECIAL(c1))
	    {
		ga_append(&ଧାଡ଼ି_ga, K_SPECIAL);
		ga_append(&ଧାଡ଼ି_ga, K_SECOND(c1));
		ga_append(&ଧାଡ଼ି_ga, K_THIRD(c1));
	    }
	    ଅନ୍ୟଥା
		ga_append(&ଧାଡ଼ି_ga, c1);
	}

	cmod = 0;
    }

    no_mapping--;

    ଯଦି (aborted)
	ga_clear(&ଧାଡ଼ି_ga);

    ଫେରନ୍ତୁ (char_u *)ଧାଡ଼ି_ga.ga_data;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * If there was a mapping we get its SID.  Otherwise, use "last_used_sid", it
 * is set when redo'ing.
 * Put this SID in the redo buffer, so that "." will use the same script
 * context.
 */
    ଶୂନ୍ୟ
may_add_last_used_map_to_redobuff(ଶୂନ୍ୟ)
{
    char_u  buf[3 + 20];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    buflen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sid = -1;

    ଯଦି (last_used_map != ଖାଲି)
	sid = last_used_map->m_script_ctx.sc_sid;
    ଯଦି (sid < 0)
	sid = last_used_sid;

    ଯଦି (sid < 0)
	ଫେରନ୍ତୁ;

    // <K_SID>{nr};
    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = KE_SID;
    buflen = 3;

    buflen += vim_snprintf((ଅକ୍ଷର *)buf + 3, 20, "%d;", sid);
    add_buff(&redobuff, buf, (long)buflen);
}
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_cmdkey_command(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    res;
#ifdef FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    ଯଦି (ଚାବି == K_SCRIPT_COMMAND
		  && (last_used_map != ଖାଲି || SCRIPT_ID_VALID(last_used_sid)))
    {
	save_current_sctx = current_sctx;
	ଯଦି (last_used_map != ଖାଲି)
	    current_sctx = last_used_map->m_script_ctx;
	ଅନ୍ୟଥା
	{
	    current_sctx.sc_sid = last_used_sid;
	    current_sctx.sc_lnum = 0;
	    current_sctx.sc_version = SCRIPT_ITEM(last_used_sid)->sn_version;
	}
    }
#endif

    res = do_cmdline(ଖାଲି, getcmdkeycmd, ଖାଲି, flags);

#ifdef FEAT_EVAL
    ଯଦି (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#endif

    ଫେରନ୍ତୁ res;
}

#ଯଦି defined(FEAT_EVAL)
    ଶୂନ୍ୟ
reset_last_used_map(mapblock_T *mp)
{
    ଯଦି (last_used_map != mp)
	ଫେରନ୍ତୁ;

    last_used_map = ଖାଲି;
    last_used_sid = -1;
}
#endif
