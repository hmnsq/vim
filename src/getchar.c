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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿਊਨਤਮ_ਮਾਪ 20			// minimal size for b_str

ਸਥਿਰ buffheader_T redobuff = {{ਨਿਰਾ, 0, {NUL}}, ਨਿਰਾ, 0, 0, FALSE};
ਸਥਿਰ buffheader_T old_redobuff = {{ਨਿਰਾ, 0, {NUL}}, ਨਿਰਾ, 0, 0, FALSE};
ਸਥਿਰ buffheader_T recordbuff = {{ਨਿਰਾ, 0, {NUL}}, ਨਿਰਾ, 0, 0, FALSE};

ਸਥਿਰ ਪੂਰਨ typeahead_char = 0;		// typeahead char that's not flushed

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ char_u typedchars[MAXMAPLEN + 1] = { NUL };  // typed chars before map
ਸਥਿਰ ਪੂਰਨ typedchars_pos = 0;
#ਜੇ_ਅੰਤ

/*
 * When block_redo is TRUE the redo buffer will not be changed.
 * Used by edit() to repeat insertions.
 */
ਸਥਿਰ ਪੂਰਨ	ਖੰਡ_redo = FALSE;

ਸਥਿਰ ਪੂਰਨ	ਕੁੰਜੀ_Noremap = 0;	    // remapping flags

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RM_YES		0	// tb_noremap: remap
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RM_NONE		1	// tb_noremap: don't remap
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RM_SCRIPT	2	// tb_noremap: remap local script mappings
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RM_ABBR		4	// tb_noremap: don't remap, do abbrev.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RM_SIMPLIFIED	8	// tb_noremap: modifiers merged into the key

// typebuf.tb_buf has three parts: room in front (for result of mappings), the
// middle for typeahead and room for new characters (which needs to be 3 *
// MAXMAPLEN for the Amiga).
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਸਾਧਾਰਨ
// Add extra space to handle large OSC responses in bigger chunks (improve
// performance)
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3) + 2048)
#ਨਹੀਂਤਾਂ // Tiny version
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
#ਜੇ_ਅੰਤ
ਸਥਿਰ char_u	typebuf_init[TYPELEN_INIT];	// initial typebuf.tb_buf
ਸਥਿਰ char_u	noremapbuf_init[TYPELEN_INIT];	// initial typebuf.tb_noremap

ਸਥਿਰ size_t	ਆਖਰੀ_recorded_len = 0;	// number of last recorded chars

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
mapblock_T	*ਆਖਰੀ_ਵਰਤਿਆ_map = ਨਿਰਾ;
ਪੂਰਨ		ਆਖਰੀ_ਵਰਤਿਆ_sid = -1;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ	read_readbuf(buffheader_T *buf, ਪੂਰਨ advance);
ਸਥਿਰ ਖਾਲੀ	init_typebuf(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	may_sync_undo(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	free_typebuf(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	closescript(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	updatescript(ਪੂਰਨ c);
ਸਥਿਰ ਪੂਰਨ	vgetorpeek(ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ	inchar(char_u *buf, ਪੂਰਨ maxlen, ਲੰਮਾ wait_ਸਮਾਂ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਪੂਰਨ	do_ਕੁੰਜੀ_ਨਿਵੇਸ਼_pre(ਪੂਰਨ c);
#ਜੇ_ਅੰਤ

/*
 * Free and clear a buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    ਲਈ (p = buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ; p != ਨਿਰਾ; p = np)
    {
	np = p->b_ਅਗਲਾ;
	vim_free(p);
    }
    buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ = ਨਿਰਾ;
    buf->bh_curr = ਨਿਰਾ;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    ਸਥਿਰ char_u *
get_buffcont(
    buffheader_T	*ਬਫਰ,
    ਪੂਰਨ			dozero,	    // count == zero is not an error
    size_t		*len)	    // the length of the returned buffer
{
    long_u	    ਗਿਣਤੀ_2 = 0;
    char_u	    *p = ਨਿਰਾ;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;
    size_t	    i = 0;

    // compute the total length of the string
    ਲਈ (bp = ਬਫਰ->bh_ਪਹਿਲਾ.b_ਅਗਲਾ; bp != ਨਿਰਾ; bp = bp->b_ਅਗਲਾ)
	ਗਿਣਤੀ_2 += (long_u)bp->b_strlen;

    ਜੇ ((ਗਿਣਤੀ_2 > 0 || dozero) && (p = alloc(ਗਿਣਤੀ_2 + 1)) != ਨਿਰਾ)
    {
	p2 = p;
	ਲਈ (bp = ਬਫਰ->bh_ਪਹਿਲਾ.b_ਅਗਲਾ; bp != ਨਿਰਾ; bp = bp->b_ਅਗਲਾ)
	    ਲਈ (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
	i = (size_t)(p2 - p);
    }

    ਜੇ (len != ਨਿਰਾ)
	*len = i;

    ਵਾਪਸ p;
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(ਖਾਲੀ)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE, &len);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    ਜੇ (len >= ਆਖਰੀ_recorded_len)
    {
	len -= ਆਖਰੀ_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    ਜੇ (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    ਵਾਪਸ (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    string_T
get_inserted(ਖਾਲੀ)
{
    size_t len = 0;
    char_u *str = get_buffcont(&redobuff, FALSE, &len);
    string_T ret = { str, len };
    ਵਾਪਸ ret;
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_buff(
    buffheader_T	*buf,
    char_u		*s,
    ਲੰਮਾ		slen)	// length of "s" or -1
{
    ਜੇ (slen < 0)
	slen = (ਲੰਮਾ)STRLEN(s);
    ਜੇ (slen == 0)				// don't add empty strings
	ਵਾਪਸ;

    ਜੇ (buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ == ਨਿਰਾ)	// first add to list
    {
	buf->bh_curr = &(buf->bh_ਪਹਿਲਾ);
	buf->bh_create_newblock = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (buf->bh_curr == ਨਿਰਾ)	// buffer has already been read
    {
	iemsg(e_ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਅੰਦਰੂਨੀ_ਬਫਰ_that_was_already_read_from);
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (buf->bh_ਸੂਚਕਾਂਕ != 0)
    {
	mch_memmove(buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ->b_str,
		    buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ->b_str + buf->bh_ਸੂਚਕਾਂਕ,
		    (buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ->b_strlen - buf->bh_ਸੂਚਕਾਂਕ) + 1);
	buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ->b_strlen -= buf->bh_ਸੂਚਕਾਂਕ;
	buf->bh_space += buf->bh_ਸੂਚਕਾਂਕ;
    }
    buf->bh_ਸੂਚਕਾਂਕ = 0;

    ਜੇ (!buf->bh_create_newblock && buf->bh_space >= (ਪੂਰਨ)slen)
    {
	vim_strncpy(buf->bh_curr->b_str + buf->bh_curr->b_strlen, s, (size_t)slen);
	buf->bh_curr->b_strlen += slen;
	buf->bh_space -= slen;
    }
    ਨਹੀਂਤਾਂ
    {
	long_u	    len;
	buffblock_T *p;

	ਜੇ (slen < ਨਿਊਨਤਮ_ਮਾਪ)
	    len = ਨਿਊਨਤਮ_ਮਾਪ;
	ਨਹੀਂਤਾਂ
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ; // no space, just forget it
	vim_strncpy(p->b_str, s, (size_t)slen);
	p->b_strlen = slen;
	buf->bh_space = (ਪੂਰਨ)(len - slen);
	buf->bh_create_newblock = FALSE;

	p->b_ਅਗਲਾ = buf->bh_curr->b_ਅਗਲਾ;
	buf->bh_curr->b_ਅਗਲਾ = p;
	buf->bh_curr = p;
    }
}

/*
 * Delete "slen" bytes from the end of "buf".
 * Only works when it was just added.
 */
    ਸਥਿਰ ਖਾਲੀ
delete_buff_ਪੂਛ(buffheader_T *buf, ਪੂਰਨ slen)
{
    ਜੇ (buf->bh_curr == ਨਿਰਾ)
	ਵਾਪਸ;  // nothing to delete
    ਜੇ (buf->bh_curr->b_strlen < (size_t)slen)
	ਵਾਪਸ;

    buf->bh_curr->b_str[buf->bh_curr->b_strlen - (size_t)slen] = NUL;
    buf->bh_curr->b_strlen -= slen;
    buf->bh_space += slen;
}

/*
 * Add number "n" to buffer "buf".
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_ਅੰਕ_buff(buffheader_T *buf, ਲੰਮਾ n)
{
    char_u	ਸੰਖਿਆ[32];
    ਪੂਰਨ		numberlen;

    numberlen = vim_snprintf((ਅੱਖਰ *)ਸੰਖਿਆ, ਆਕਾਰ(ਸੰਖਿਆ), "%ld", n);
    ਜੋੜੋ_buff(buf, ਸੰਖਿਆ, (ਲੰਮਾ)numberlen);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_char_buff(buffheader_T *buf, ਪੂਰਨ c)
{
    char_u	ਬਾਈਟਾਂ[MB_MAXBYTES + 1];
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    char_u	ਆਰਜ਼ੀ[4];
    ਲੰਮਾ	templen;

    ਜੇ (IS_SPECIAL(c))
	len = 1;
    ਨਹੀਂਤਾਂ
	len = (*mb_char2bytes)(c, ਬਾਈਟਾਂ);
    ਲਈ (i = 0; i < len; ++i)
    {
	ਜੇ (!IS_SPECIAL(c))
	    c = ਬਾਈਟਾਂ[i];

	ਜੇ (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    ਆਰਜ਼ੀ[0] = K_SPECIAL;
	    ਆਰਜ਼ੀ[1] = K_ਦੂਜਾ(c);
	    ਆਰਜ਼ੀ[2] = K_ਤੀਜਾ(c);
	    ਆਰਜ਼ੀ[3] = NUL;
	    templen = 3;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਨਹੀਂਤਾਂ ਜੇ (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    ਆਰਜ਼ੀ[0] = CSI;
	    ਆਰਜ਼ੀ[1] = KS_EXTRA;
	    ਆਰਜ਼ੀ[2] = (ਪੂਰਨ)KE_CSI;
	    ਆਰਜ਼ੀ[3] = NUL;
	    templen = 3;
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ
	{
	    ਆਰਜ਼ੀ[0] = c;
	    ਆਰਜ਼ੀ[1] = NUL;
	    templen = 1;
	}
	ਜੋੜੋ_buff(buf, ਆਰਜ਼ੀ, templen);
    }
}

// First read ahead buffer. Used for translated commands.
ਸਥਿਰ buffheader_T readbuf1 = {{ਨਿਰਾ, 0, {NUL}}, ਨਿਰਾ, 0, 0, FALSE};

// Second read ahead buffer. Used for redo.
ਸਥਿਰ buffheader_T readbuf2 = {{ਨਿਰਾ, 0, {NUL}}, ਨਿਰਾ, 0, 0, FALSE};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    ਸਥਿਰ ਪੂਰਨ
read_readbuffers(ਪੂਰਨ advance)
{
    ਪੂਰਨ c;

    c = read_readbuf(&readbuf1, advance);
    ਜੇ (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    ਵਾਪਸ c;
}

    ਸਥਿਰ ਪੂਰਨ
read_readbuf(buffheader_T *buf, ਪੂਰਨ advance)
{
    char_u	c;
    buffblock_T	*curr;

    ਜੇ (buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ == ਨਿਰਾ)  // buffer is empty
	ਵਾਪਸ NUL;

    curr = buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ;
    c = curr->b_str[buf->bh_ਸੂਚਕਾਂਕ];

    ਜੇ (advance)
    {
	ਜੇ (curr->b_str[++buf->bh_ਸੂਚਕਾਂਕ] == NUL)
	{
	    buf->bh_ਪਹਿਲਾ.b_ਅਗਲਾ = curr->b_ਅਗਲਾ;
	    vim_free(curr);
	    buf->bh_ਸੂਚਕਾਂਕ = 0;
	}
    }
    ਵਾਪਸ c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    ਸਥਿਰ ਖਾਲੀ
ਸ਼ੁਰੂ_stuff(ਖਾਲੀ)
{
    ਜੇ (readbuf1.bh_ਪਹਿਲਾ.b_ਅਗਲਾ != ਨਿਰਾ)
    {
	readbuf1.bh_curr = &(readbuf1.bh_ਪਹਿਲਾ);
	readbuf1.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
    ਜੇ (readbuf2.bh_ਪਹਿਲਾ.b_ਅਗਲਾ != ਨਿਰਾ)
    {
	readbuf2.bh_curr = &(readbuf2.bh_ਪਹਿਲਾ);
	readbuf2.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    ਪੂਰਨ
stuff_ਖਾਲੀ_ਥਾਵਾਂ(ਖਾਲੀ)
{
    ਵਾਪਸ (readbuf1.bh_ਪਹਿਲਾ.b_ਅਗਲਾ == ਨਿਰਾ
	 && readbuf2.bh_ਪਹਿਲਾ.b_ਅਗਲਾ == ਨਿਰਾ);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * readbuf2.
 */
    ਪੂਰਨ
readbuf1_ਖਾਲੀ_ਥਾਵਾਂ(ਖਾਲੀ)
{
    ਵਾਪਸ (readbuf1.bh_ਪਹਿਲਾ.b_ਅਗਲਾ == ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

/*
 * Set a typeahead character that won't be flushed.
 */
    ਖਾਲੀ
typeahead_noflush(ਪੂਰਨ c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    ਖਾਲੀ
flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    ਸ਼ੁਰੂ_stuff();
    ਜਦੋਂ (read_readbuffers(TRUE) != NUL)
	;

    ਜੇ (flush_typeahead == FLUSH_ਨਿਊਨਤਮ)
    {
	// remove mapped characters at the start only,
	// but only when enough space left in typebuf
	ਜੇ (typebuf.tb_off + typebuf.tb_maplen >= typebuf.tb_buflen)
	{
	    typebuf.tb_off = MAXMAPLEN;
	    typebuf.tb_len = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    typebuf.tb_off += typebuf.tb_maplen;
	    typebuf.tb_len -= typebuf.tb_maplen;
	}
#ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	ਜੇ (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	// remove typeahead
	ਜੇ (flush_typeahead == FLUSH_ਨਿਵੇਸ਼)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    ਜਦੋਂ (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#ਜੇ_ਅੰਤ
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
    ਜੇ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * The previous contents of the redo buffer is kept in old_redobuffer.
 * This is used for the CTRL-O <.> command in insert mode.
 */
    ਖਾਲੀ
ResetRedobuff(ਖਾਲੀ)
{
    ਜੇ (ਖੰਡ_redo)
	ਵਾਪਸ;

    free_buff(&old_redobuff);
    old_redobuff = redobuff;
    redobuff.bh_ਪਹਿਲਾ.b_ਅਗਲਾ = ਨਿਰਾ;
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    ਖਾਲੀ
CancelRedo(ਖਾਲੀ)
{
    ਜੇ (ਖੰਡ_redo)
	ਵਾਪਸ;

    free_buff(&redobuff);
    redobuff = old_redobuff;
    old_redobuff.bh_ਪਹਿਲਾ.b_ਅਗਲਾ = ਨਿਰਾ;
    ਸ਼ੁਰੂ_stuff();
    ਜਦੋਂ (read_readbuffers(TRUE) != NUL)
	;
}

/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
    ਖਾਲੀ
saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;
    size_t	slen;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_ਪਹਿਲਾ.b_ਅਗਲਾ = ਨਿਰਾ;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_ਪਹਿਲਾ.b_ਅਗਲਾ = ਨਿਰਾ;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE, &slen);
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੋੜੋ_buff(&redobuff, s, (ਲੰਮਾ)slen);
    vim_free(s);
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    ਖਾਲੀ
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
    ਖਾਲੀ
Append_ਮੰਜ਼ਿਲ_Redobuff(char_u *s)
{
    ਜੇ (!ਖੰਡ_redo)
	ਜੋੜੋ_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    ਖਾਲੀ
Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(
    char_u	*str,
    ਪੂਰਨ		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    ਪੂਰਨ		c;
    char_u	*ਸ਼ੁਰੂ;

    ਜੇ (ਖੰਡ_redo)
	ਵਾਪਸ;

    ਜਦੋਂ (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	ਸ਼ੁਰੂ = s;
	ਜਦੋਂ (*s >= ' ' && *s < DEL && (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	ਜੇ (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	ਜੇ (s > ਸ਼ੁਰੂ)
	    ਜੋੜੋ_buff(&redobuff, ਸ਼ੁਰੂ, (ਲੰਮਾ)(s - ਸ਼ੁਰੂ));

	ਜੇ (*s == NUL || (len >= 0 && s - str >= len))
	    ਰੋਕੋ;

	// Handle a special or multibyte character.
	ਜੇ (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	ਨਹੀਂਤਾਂ
	    c = *s++;
	ਜੇ (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    ਜੋੜੋ_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048
	ਜੇ (*s == NUL && c == '0')
	    ਜੋੜੋ_buff(&redobuff, (char_u *)"048", 3L);
	ਨਹੀਂਤਾਂ
	    ਜੋੜੋ_char_buff(&redobuff, c);
    }
}

/*
 * Append "s" to the redo buffer, leaving 3-byte special key codes unmodified
 * and escaping other K_SPECIAL and CSI bytes.
 */
    ਖਾਲੀ
Append_ਮੰਜ਼ਿਲ_Redobuff_Spec(char_u *s)
{
    ਜੇ (ਖੰਡ_redo)
	ਵਾਪਸ;

    ਜਦੋਂ (*s != NUL)
    {
	ਜੇ (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    ਜੋੜੋ_buff(&redobuff, s, 3L);
	    s += 3;
	}
	ਨਹੀਂਤਾਂ
	    ਜੋੜੋ_char_buff(&redobuff, mb_cptr2char_adv(&s));
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ਖਾਲੀ
Append_Char_ਮੰਜ਼ਿਲ_Redobuff(ਪੂਰਨ c)
{
    ਜੇ (!ਖੰਡ_redo)
	ਜੋੜੋ_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    ਖਾਲੀ
Append_ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Redobuff(ਲੰਮਾ n)
{
    ਜੇ (!ਖੰਡ_redo)
	ਜੋੜੋ_ਅੰਕ_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    ਖਾਲੀ
stuffReadbuff(char_u *s)
{
    ਜੋੜੋ_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    ਖਾਲੀ
stuffRedoReadbuff(char_u *s)
{
    ਜੋੜੋ_buff(&readbuf2, s, -1L);
}

    ਖਾਲੀ
stuffReadbuffLen(char_u *s, ਲੰਮਾ len)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (ਜੋੜੋ_ਆਖਰੀ_ਪਾਓ == 1) // Only add if this is the first call, for
			      // recursive calls, ignore.
	ga_concat_len(&ਆਖਰੀ_ਪਾਓ_ga, s, (size_t)len);
#ਜੇ_ਅੰਤ
    ਜੋੜੋ_buff(&readbuf1, s, len);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    ਖਾਲੀ
stuffReadbuffSpec(char_u *s)
{
    ਪੂਰਨ c;

    ਜਦੋਂ (*s != NUL)
    {
	ਜੇ (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	ਨਹੀਂਤਾਂ
	{
	    c = mb_cptr2char_adv(&s);
	    ਜੇ (c == CAR || c == NL || c == ESC)
		c = ' ';
	    stuffcharReadbuff(c);
	}
    }
}
#ਜੇ_ਅੰਤ

/*
 * Append a character to the stuff buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ਖਾਲੀ
stuffcharReadbuff(ਪੂਰਨ c)
{
    ਜੋੜੋ_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    ਖਾਲੀ
stuffnumReadbuff(ਲੰਮਾ n)
{
    ਜੋੜੋ_ਅੰਕ_buff(&readbuf1, n);
}

/*
 * Stuff a string into the typeahead buffer, such that edit() will insert it
 * literally ("literally" TRUE) or interpret is as typed characters.
 */
    ਖਾਲੀ
stuffescaped(char_u *arg, ਪੂਰਨ literally)
{
    ਪੂਰਨ		c;
    char_u	*ਸ਼ੁਰੂ;

    ਜਦੋਂ (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	ਸ਼ੁਰੂ = arg;
	ਜਦੋਂ ((*arg >= ' ' && *arg < DEL)
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	ਜੇ (arg > ਸ਼ੁਰੂ)
	    stuffReadbuffLen(ਸ਼ੁਰੂ, (ਲੰਮਾ)(arg - ਸ਼ੁਰੂ));

	// stuff a single special character
	ਜੇ (*arg != NUL)
	{
	    ਜੇ (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    ਨਹੀਂਤਾਂ
		c = *arg++;
	    ਜੇ (literally && ((c < ' ' && c != TAB) || c == DEL))
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
    ਸਥਿਰ ਪੂਰਨ
read_redo(ਪੂਰਨ init, ਪੂਰਨ old_redo)
{
    ਸਥਿਰ buffblock_T	*bp;
    ਸਥਿਰ char_u	*p;
    ਪੂਰਨ			c;
    ਪੂਰਨ			n;
    char_u		buf[MB_MAXBYTES + 1];
    ਪੂਰਨ			i;

    ਜੇ (init)
    {
	ਜੇ (old_redo)
	    bp = old_redobuff.bh_ਪਹਿਲਾ.b_ਅਗਲਾ;
	ਨਹੀਂਤਾਂ
	    bp = redobuff.bh_ਪਹਿਲਾ.b_ਅਗਲਾ;
	ਜੇ (bp == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	p = bp->b_str;
	ਵਾਪਸ OK;
    }
    ਜੇ ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	ਜੇ (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	ਨਹੀਂਤਾਂ
	    n = 1;
	ਲਈ (i = 0; ; ++i)
	{
	    ਜੇ (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = ਮੰਜ਼ਿਲ_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    ਜੇ (c == CSI)	// escaped CSI
		p += 2;
#ਜੇ_ਅੰਤ
	    ਜੇ (*++p == NUL && bp->b_ਅਗਲਾ != ਨਿਰਾ)
	    {
		bp = bp->b_ਅਗਲਾ;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    ਜੇ (i == n - 1)	// last byte of a character
	    {
		ਜੇ (n != 1)
		    c = (*mb_ptr2char)(buf);
		ਰੋਕੋ;
	    }
	    c = *p;
	    ਜੇ (c == NUL)	// cannot happen?
		ਰੋਕੋ;
	}
    }

    ਵਾਪਸ c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    ਸਥਿਰ ਖਾਲੀ
copy_redo(ਪੂਰਨ old_redo)
{
    ਪੂਰਨ	    c;

    ਜਦੋਂ ((c = read_redo(FALSE, old_redo)) != NUL)
	ਜੋੜੋ_char_buff(&readbuf2, c);
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
    ਪੂਰਨ
ਸ਼ੁਰੂ_redo(ਲੰਮਾ ਗਿਣਤੀ_2, ਪੂਰਨ old_redo)
{
    ਪੂਰਨ	    c;

    // init the pointers; return if nothing to redo
    ਜੇ (read_redo(TRUE, old_redo) == FAIL)
	ਵਾਪਸ FAIL;

    c = read_redo(FALSE, old_redo);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (c == K_SID)
    {
	// Copy the <SID>{sid}; sequence
	ਜੋੜੋ_char_buff(&readbuf2, c);
	ਲਈ (;;)
	{
	    c = read_redo(FALSE, old_redo);
	    ਜੋੜੋ_char_buff(&readbuf2, c);
	    ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(c))
		ਰੋਕੋ;
	}
	c = read_redo(FALSE, old_redo);
    }
#ਜੇ_ਅੰਤ

    // copy the buffer name, if present
    ਜੇ (c == '"')
    {
	ਜੋੜੋ_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	// if a numbered buffer is used, increment the number
	ਜੇ (c >= '1' && c < '9')
	    ++c;
	ਜੋੜੋ_char_buff(&readbuf2, c);

	// the expression register should be re-evaluated
	ਜੇ (c == '=')
	{
	    ਜੋੜੋ_char_buff(&readbuf2, CAR);
	    cmd_silent = TRUE;
	}

	c = read_redo(FALSE, old_redo);
    }

    ਜੇ (c == 'v')   // redo Visual
    {
	VIsual = curwin->w_cursor;
	VIsual_ਸਰਗਰਮ = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    // try to enter the count (in place of a previous count)
    ਜੇ (ਗਿਣਤੀ_2)
    {
	ਜਦੋਂ (VIM_ISDIGIT(c))	// skip "old" count
	    c = read_redo(FALSE, old_redo);
	ਜੋੜੋ_ਅੰਕ_buff(&readbuf2, ਗਿਣਤੀ_2);
    }

    // copy the rest from the redo buffer into the stuff buffer
    ਜੋੜੋ_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    ਵਾਪਸ OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
ਸ਼ੁਰੂ_redo_ins(ਖਾਲੀ)
{
    ਪੂਰਨ	    c;

    ਜੇ (read_redo(TRUE, FALSE) == FAIL)
	ਵਾਪਸ FAIL;
    ਸ਼ੁਰੂ_stuff();

    // skip the count and the command character
    ਜਦੋਂ ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	ਜੇ (vim_strchr((char_u *)"AaIiRrOo", c) != ਨਿਰਾ)
	{
	    ਜੇ (c == 'O' || c == 'o')
		ਜੋੜੋ_buff(&readbuf2, NL_STR, -1L);
	    ਰੋਕੋ;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    ਖੰਡ_redo = TRUE;
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
stop_redo_ins(ਖਾਲੀ)
{
    ਖੰਡ_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    ਸਥਿਰ ਖਾਲੀ
init_typebuf(ਖਾਲੀ)
{
    ਜੇ (typebuf.tb_buf != ਨਿਰਾ)
	ਵਾਪਸ;

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
    ਪੂਰਨ
noremap_ਕੁੰਜੀਆਂ(ਖਾਲੀ)
{
    ਵਾਪਸ ਕੁੰਜੀ_Noremap & (RM_NONE|RM_SCRIPT);
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
    ਪੂਰਨ
ins_typebuf(
    char_u	*str,
    ਪੂਰਨ		noremap,
    ਪੂਰਨ		ਵਿੱਥ,
    ਪੂਰਨ		nottyped,
    ਪੂਰਨ		silent)
{
    char_u	*s1, *s2;
    ਪੂਰਨ		newlen;
    ਪੂਰਨ		addlen;
    ਪੂਰਨ		i;
    ਪੂਰਨ		newoff;
    ਪੂਰਨ		val;
    ਪੂਰਨ		nrm;

    init_typebuf();
    ਜੇ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
    state_no_longer_ਸੁਰੱਖਿਅਤ("ins_typebuf()");

    addlen = (ਪੂਰਨ)STRLEN(str);

    ਜੇ (ਵਿੱਥ == 0 && addlen <= typebuf.tb_off)
    {
	/*
	 * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
	 */
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    ਨਹੀਂਤਾਂ ਜੇ (typebuf.tb_len == 0 && typebuf.tb_buflen
					       >= addlen + 3 * (MAXMAPLEN + 4))
    {
	/*
	 * Buffer is empty and string fits in the existing buffer.
	 * Leave some space before and after, if possible.
	 */
	typebuf.tb_off = (typebuf.tb_buflen - addlen - 3 * (MAXMAPLEN + 4)) / 2;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ extra;

	/*
	 * Need to allocate a new buffer.
	 * In typebuf.tb_buf there must always be room for 3 * (MAXMAPLEN + 4)
	 * characters.  We add some extra room to avoid having to allocate too
	 * often.
	 */
	newoff = MAXMAPLEN + 4;
	extra = addlen + newoff + 4 * (MAXMAPLEN + 4);
	ਜੇ (typebuf.tb_len > 2147483647 - extra)
	{
	    // string is getting too long for a 32 bit int
	    emsg(_(e_command_too_complex));    // also calls flush_buffers
	    setcursor();
	    ਵਾਪਸ FAIL;
	}
	newlen = typebuf.tb_len + extra;
	s1 = alloc(newlen);
	ਜੇ (s1 == ਨਿਰਾ)		    // out of memory
	    ਵਾਪਸ FAIL;
	s2 = alloc(newlen);
	ਜੇ (s2 == ਨਿਰਾ)		    // out of memory
	{
	    vim_free(s1);
	    ਵਾਪਸ FAIL;
	}
	typebuf.tb_buflen = newlen;

	// copy the old chars, before the insertion point
	mch_memmove(s1 + newoff, typebuf.tb_buf + typebuf.tb_off,
							      (size_t)ਵਿੱਥ);
	// copy the new chars
	mch_memmove(s1 + newoff + ਵਿੱਥ, str, (size_t)addlen);
	// copy the old chars, after the insertion point, including the	NUL at
	// the end
	mch_memmove(s1 + newoff + ਵਿੱਥ + addlen,
				     typebuf.tb_buf + typebuf.tb_off + ਵਿੱਥ,
				       (size_t)(typebuf.tb_len - ਵਿੱਥ + 1));
	ਜੇ (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)ਵਿੱਥ);
	mch_memmove(s2 + newoff + ਵਿੱਥ + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + ਵਿੱਥ,
					   (size_t)(typebuf.tb_len - ਵਿੱਥ));
	ਜੇ (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    // If noremap == REMAP_SCRIPT: do remap script-local mappings.
    ਜੇ (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    ਨਹੀਂਤਾਂ ਜੇ (noremap == REMAP_SKIP)
	val = RM_ABBR;
    ਨਹੀਂਤਾਂ
	val = RM_NONE;

    /*
     * Adjust typebuf.tb_noremap[] for the new characters:
     * If noremap == REMAP_NONE or REMAP_SCRIPT: new characters are
     *			(sometimes) not remappable
     * If noremap == REMAP_YES: all the new characters are mappable
     * If noremap  > 0: "noremap" characters are not remappable, the rest
     *			mappable
     */
    ਜੇ (noremap == REMAP_SKIP)
	nrm = 1;
    ਨਹੀਂਤਾਂ ਜੇ (noremap < 0)
	nrm = addlen;
    ਨਹੀਂਤਾਂ
	nrm = noremap;
    ਲਈ (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + ਵਿੱਥ] =
						  (--nrm >= 0) ? val : RM_YES;

    // tb_maplen and tb_silent only remember the length of mapped and/or
    // silent mappings at the start of the buffer, assuming that a mapped
    // sequence doesn't result in typed characters.
    ਜੇ (nottyped || typebuf.tb_maplen > ਵਿੱਥ)
	typebuf.tb_maplen += addlen;
    ਜੇ (silent || typebuf.tb_silent > ਵਿੱਥ)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    ਜੇ (typebuf.tb_no_abbr_cnt && ਵਿੱਥ == 0)	// and not used for abbrev.s
	typebuf.tb_no_abbr_cnt += addlen;

    ਵਾਪਸ OK;
}

/*
 * Put character "c" back into the typeahead buffer.
 * Can be used for a character obtained by vgetc() that needs to be put back.
 * Uses cmd_silent, KeyTyped and KeyNoremap to restore the flags belonging to
 * the char.
 * Returns the length of what was inserted.
 */
    ਪੂਰਨ
ins_char_typebuf(ਪੂਰਨ c, ਪੂਰਨ modifiers)
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    ਪੂਰਨ		len = special_ਮੰਜ਼ਿਲ_buf(c, modifiers, TRUE, buf);

    buf[len] = NUL;
    (ਖਾਲੀ)ins_typebuf(buf, ਕੁੰਜੀ_Noremap, 0, !ਕੁੰਜੀ_Typed, cmd_silent);
    ਵਾਪਸ len;
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
    ਪੂਰਨ
typebuf_ਬਦਲਿਆ(
    ਪੂਰਨ		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    ਵਾਪਸ (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#ਜੇ_ਅੰਤ
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    ਪੂਰਨ
typebuf_typed(ਖਾਲੀ)
{
    ਵਾਪਸ typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    ਪੂਰਨ
typebuf_maplen(ਖਾਲੀ)
{
    ਵਾਪਸ typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    ਖਾਲੀ
del_typebuf(ਪੂਰਨ len, ਪੂਰਨ ਵਿੱਥ)
{
    ਪੂਰਨ	    i;

    ਜੇ (len == 0)
	ਵਾਪਸ;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    ਜੇ (ਵਿੱਥ == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    ਨਹੀਂਤਾਂ
    {
	i = typebuf.tb_off + ਵਿੱਥ;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	ਜੇ (typebuf.tb_off > MAXMAPLEN)
	{
	    mch_memmove(typebuf.tb_buf + MAXMAPLEN,
			     typebuf.tb_buf + typebuf.tb_off, (size_t)ਵਿੱਥ);
	    mch_memmove(typebuf.tb_noremap + MAXMAPLEN,
			 typebuf.tb_noremap + typebuf.tb_off, (size_t)ਵਿੱਥ);
	    typebuf.tb_off = MAXMAPLEN;
	}
	// adjust typebuf.tb_buf (include the NUL at the end)
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + ਵਿੱਥ,
						     typebuf.tb_buf + i + len,
				       (size_t)(typebuf.tb_len - ਵਿੱਥ + 1));
	// adjust typebuf.tb_noremap[]
	mch_memmove(typebuf.tb_noremap + typebuf.tb_off + ਵਿੱਥ,
						 typebuf.tb_noremap + i + len,
					   (size_t)(typebuf.tb_len - ਵਿੱਥ));
    }

    ਜੇ (typebuf.tb_maplen > ਵਿੱਥ)		// adjust tb_maplen
    {
	ਜੇ (typebuf.tb_maplen < ਵਿੱਥ + len)
	    typebuf.tb_maplen = ਵਿੱਥ;
	ਨਹੀਂਤਾਂ
	    typebuf.tb_maplen -= len;
    }
    ਜੇ (typebuf.tb_silent > ਵਿੱਥ)		// adjust tb_silent
    {
	ਜੇ (typebuf.tb_silent < ਵਿੱਥ + len)
	    typebuf.tb_silent = ਵਿੱਥ;
	ਨਹੀਂਤਾਂ
	    typebuf.tb_silent -= len;
    }
    ਜੇ (typebuf.tb_no_abbr_cnt > ਵਿੱਥ)	// adjust tb_no_abbr_cnt
    {
	ਜੇ (typebuf.tb_no_abbr_cnt < ਵਿੱਥ + len)
	    typebuf.tb_no_abbr_cnt = ਵਿੱਥ;
	ਨਹੀਂਤਾਂ
	    typebuf.tb_no_abbr_cnt -= len;
    }

#ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#ਜੇ_ਅੰਤ
    ਜੇ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * State for adding bytes to a recording or 'showcmd'.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    ਪੂਰਨ		prev_c;
    size_t	buflen;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	pending_special;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	pending_mbyte;
} gotchars_state_T;

/*
 * Add a single byte to a recording or 'showcmd'.
 * Return TRUE if a full key has been received, FALSE otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
gotchars_ਜੋੜੋ_byte(gotchars_state_T *state, char_u byte)
{
    ਪੂਰਨ		c = state->buf[state->buflen++] = byte;
    ਪੂਰਨ		retval = FALSE;
    ਪੂਰਨ		ਵਿੱਚ_special = state->pending_special > 0;
    ਪੂਰਨ		ਵਿੱਚ_mbyte = state->pending_mbyte > 0;

    ਜੇ (ਵਿੱਚ_special)
	state->pending_special--;
    ਨਹੀਂਤਾਂ ਜੇ (c == K_SPECIAL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		|| c == CSI
#ਜੇ_ਅੰਤ
	    )
	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	state->pending_special = 2;

    ਜੇ (state->pending_special > 0)
	ਜਾਓ ret_false;

    ਜੇ (ਵਿੱਚ_mbyte)
	state->pending_mbyte--;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਵਿੱਚ_special)
	{
	    ਜੇ (state->prev_c == KS_MODIFIER)
		// When receiving a modifier, wait for the modified key.
		ਜਾਓ ret_false;
	    c = ਮੰਜ਼ਿਲ_SPECIAL(state->prev_c, c);
	    // Drop K_FOCUSGAINED, K_FOCUSLOST and K_COMPLETE_DELAY, they are
	    // not useful in a recording.
	    ਜੇ (c == K_FOCUSGAINED || c == K_FOCUSLOST || c == K_COMPLETE_DELAY)
		state->buflen = 0;
	}
	// When receiving a multibyte character, store it until we have all
	// the bytes, so that it won't be split between two buffer blocks,
	// and delete_buff_tail() will work properly.
	state->pending_mbyte = MB_BYTE2LEN_CHECK(c) - 1;
    }

    ਜੇ (state->pending_mbyte > 0)
	ਜਾਓ ret_false;

    retval = TRUE;
ret_false:
    state->prev_c = c;
    ਵਾਪਸ retval;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the record buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
gotchars(char_u *chars, ਪੂਰਨ len)
{
    char_u		*s = chars;
    size_t		i;
    ਪੂਰਨ			todo = len;
    ਸਥਿਰ gotchars_state_T state;

    ਜਦੋਂ (todo-- > 0)
    {
	ਜੇ (!gotchars_ਜੋੜੋ_byte(&state, *s++))
	    ਜਾਰੀ;

	// Handle one byte at a time; no translation to be done.
	ਲਈ (i = 0; i < state.buflen; ++i)
	    updatescript(state.buf[i]);

	ਜੇ (reg_recording != 0)
	{
	    state.buf[state.buflen] = NUL;
	    ਜੋੜੋ_buff(&recordbuff, state.buf, (ਲੰਮਾ)state.buflen);
	    // remember how many chars were last recorded
	    ਆਖਰੀ_recorded_len += state.buflen;
	}
	state.buflen = 0;
    }

    may_sync_undo();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // output "debug mode" message next time in debug mode
    debug_did_ਸੁਨੇਹਾ = FALSE;
#ਜੇ_ਅੰਤ

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}

/*
 * Record an <Ignore> key.
 */
    ਖਾਲੀ
gotchars_ignore(ਖਾਲੀ)
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
    ਖਾਲੀ
ungetchars(ਪੂਰਨ len)
{
    ਜੇ (reg_recording == 0)
	ਵਾਪਸ;

    delete_buff_ਪੂਛ(&recordbuff, len);
    ਆਖਰੀ_recorded_len -= len;
}

/*
 * Sync undo.  Called when typed characters are obtained from the typeahead
 * buffer, or when a menu is used.
 * Do not sync:
 * - In Insert mode, unless cursor key has been used.
 * - While reading a script file.
 * - When no_u_sync is non-zero.
 */
    ਸਥਿਰ ਖਾਲੀ
may_sync_undo(ਖਾਲੀ)
{
    ਜੇ ((!(State & (ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE)) || arrow_ਵਰਤਿਆ)
					       && scriptin[curscript] == ਨਿਰਾ)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
alloc_typebuf(ਖਾਲੀ)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    ਜੇ (typebuf.tb_buf == ਨਿਰਾ || typebuf.tb_noremap == ਨਿਰਾ)
    {
	free_typebuf();
	ਵਾਪਸ FAIL;
    }
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_off = MAXMAPLEN + 4;  // can insert without realloc
    typebuf.tb_len = 0;
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    typebuf.tb_no_abbr_cnt = 0;
    ਜੇ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
#ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    typebuf_was_filled = FALSE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ OK;
}

/*
 * Free the buffers of "typebuf".
 */
    ਸਥਿਰ ਖਾਲੀ
free_typebuf(ਖਾਲੀ)
{
    ਜੇ (typebuf.tb_buf == typebuf_init)
	ਅੰਦਰੂਨੀ_error("Free typebuf 1");
    ਨਹੀਂਤਾਂ
	VIM_CLEAR(typebuf.tb_buf);
    ਜੇ (typebuf.tb_noremap == noremapbuf_init)
	ਅੰਦਰੂਨੀ_error("Free typebuf 2");
    ਨਹੀਂਤਾਂ
	VIM_CLEAR(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
ਸਥਿਰ typebuf_T saved_typebuf[NSCRIPT];

    ਪੂਰਨ
save_typebuf(ਖਾਲੀ)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    ਜੇ (alloc_typebuf() == FAIL)
    {
	closescript();
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

ਸਥਿਰ ਪੂਰਨ old_char = -1;	// character put back by vungetc()
ਸਥਿਰ ਪੂਰਨ old_mod_ਨਕਾਬ;	// mod_mask for ungotten character
ਸਥਿਰ ਪੂਰਨ old_mouse_ਪੰਕਤੀ;	// mouse_row related to old_char
ਸਥਿਰ ਪੂਰਨ old_mouse_ਖਾਨਾ;	// mouse_col related to old_char
ਸਥਿਰ ਪੂਰਨ old_ਕੁੰਜੀ_Stuffed;	// whether old_char was stuffed

ਸਥਿਰ ਪੂਰਨ can_get_old_char(ਖਾਲੀ)
{
    // If the old character was not stuffed and characters have been added to
    // the stuff buffer, need to first get the stuffed characters instead.
    ਵਾਪਸ old_char != -1 && (old_ਕੁੰਜੀ_Stuffed || stuff_ਖਾਲੀ_ਥਾਵਾਂ());
}

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    ਖਾਲੀ
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    ਜੇ (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_ਨਕਾਬ = old_mod_ਨਕਾਬ;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_ਪਹਿਲਾ.b_ਅਗਲਾ = ਨਿਰਾ;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_ਪਹਿਲਾ.b_ਅਗਲਾ = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਨਿਵੇਸ਼_BUF
    tp->save_inputbuf = get_ਨਿਵੇਸ਼_buf();
#ਜੇ_ਅੰਤ
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    ਖਾਲੀ
restore_typeahead(tasave_T *tp, ਪੂਰਨ overwrite ਅਣਵਰਤਿਆ)
{
    ਜੇ (tp->typebuf_valid)
    {
	free_typebuf();
	typebuf = tp->save_typebuf;
    }

    old_char = tp->old_char;
    old_mod_ਨਕਾਬ = tp->old_mod_ਨਕਾਬ;

    free_buff(&readbuf1);
    readbuf1 = tp->save_readbuf1;
    free_buff(&readbuf2);
    readbuf2 = tp->save_readbuf2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਨਿਵੇਸ਼_BUF
    ਸੈੱਟ_ਨਿਵੇਸ਼_buf(tp->save_inputbuf, overwrite);
#ਜੇ_ਅੰਤ
}

/*
 * Open a new script file for the ":source!" command.
 */
    ਖਾਲੀ
openscript(
    char_u	*name,
    ਪੂਰਨ		directly)	// when TRUE execute directly
{
    ਜੇ (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	ਵਾਪਸ;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    ਜੇ (check_secure())
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    ਜੇ (scriptin[curscript] != ਨਿਰਾ)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    ਜੇ ((scriptin[curscript] = mch_fopen((ਅੱਖਰ *)NameBuff, READBIN)) == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str), name);
	ਜੇ (curscript)
	    --curscript;
	ਵਾਪਸ;
    }
    ਜੇ (save_typebuf() == FAIL)
	ਵਾਪਸ;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    ਜੇ (directly)
    {
	oparg_T	oa;
	ਪੂਰਨ	oldcurscript;
	ਪੂਰਨ	save_State = State;
	ਪੂਰਨ	save_restart_edit = restart_edit;
	ਪੂਰਨ	save_insertmode = p_im;
	ਪੂਰਨ	save_ਅੰਤ_op = ਅੰਤ_op;
	ਪੂਰਨ	save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;

	State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
	ਸੁਨੇਹਾ_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	ਅੰਤ_op = FALSE;

	oldcurscript = curscript;
	ਕਰੋ
	{
	    update_topline_cursor();	// update cursor position and topline
	    ਸਾਧਾਰਨ_cmd(&oa, FALSE);	// execute one command
	    (ਖਾਲੀ)vpeekc();		// check for end of file
	}
	ਜਦੋਂ (scriptin[oldcurscript] != ਨਿਰਾ);

	State = save_State;
	ਸੁਨੇਹਾ_scroll = save_ਸੁਨੇਹਾ_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	ਅੰਤ_op = save_ਅੰਤ_op;
    }
}

/*
 * Close the currently active input script.
 */
    ਸਥਿਰ ਖਾਲੀ
closescript(ਖਾਲੀ)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = ਨਿਰਾ;
    ਜੇ (curscript > 0)
	--curscript;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
close_all_scripts(ਖਾਲੀ)
{
    ਜਦੋਂ (scriptin[0] != ਨਿਰਾ)
	closescript();
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE when reading keys from a script file.
 */
    ਪੂਰਨ
using_script(ਖਾਲੀ)
{
    ਵਾਪਸ scriptin[curscript] != ਨਿਰਾ;
}

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    ਖਾਲੀ
before_blocking(ਖਾਲੀ)
{
    // Tell terminal to flush screen contents before blocking
    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_FLUSH);

    updatescript(0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (may_garbage_collect)
	garbage_collect(FALSE);
#ਜੇ_ਅੰਤ
}

/*
 * updatescript() is called when a character can be written into the script
 * file or when we have waited some time for a character (c == 0)
 *
 * All the changed memfiles are synced if c == 0 or when the number of typed
 * characters reaches 'updatecount' and 'updatecount' is non-zero.
 */
    ਸਥਿਰ ਖਾਲੀ
updatescript(ਪੂਰਨ c)
{
    ਸਥਿਰ ਪੂਰਨ	    ਗਿਣਤੀ_2 = 0;

    ਜੇ (c && scriptout)
	putc(c, scriptout);
    ਜੇ (c == 0 || (p_uc > 0 && ++ਗਿਣਤੀ_2 >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	ਗਿਣਤੀ_2 = 0;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (typedchars_pos < MAXMAPLEN)
    {
	typedchars[typedchars_pos] = c;
	typedchars_pos++;
    }
#ਜੇ_ਅੰਤ
}

/*
 * Convert "c_arg" plus "modifiers" to merge the effect of modifyOtherKeys into
 * the character.  Also for when the Kitty key protocol is used.
 */
    ਪੂਰਨ
ਮਿਲਾਓ_modify_Other_ਕੁੰਜੀਆਂ(ਪੂਰਨ c_arg, ਪੂਰਨ *modifiers)
{
    ਪੂਰਨ c = c_arg;

    // CTRL only uses the lower 5 bits of the character.
    ਜੇ (*modifiers & MOD_ਨਕਾਬ_CTRL)
    {
	ਜੇ ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	{
	    c &= 0x1f;
	    ਜੇ (c == NUL)
		c = K_ZERO;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	ਨਹੀਂਤਾਂ ਜੇ (c == '2')
	    c = NUL;
	ਨਹੀਂਤਾਂ ਜੇ (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	ਨਹੀਂਤਾਂ ਜੇ (c == '8')
	    c = BS;
	ਨਹੀਂਤਾਂ ਜੇ (c == '?')
	    c = DEL;
#ਜੇ_ਅੰਤ
	ਜੇ (c != c_arg)
	    *modifiers &= ~MOD_ਨਕਾਬ_CTRL;
    }

    // Alt/Meta sets the 8th bit of the character.
    ਜੇ ((*modifiers & (MOD_ਨਕਾਬ_META | MOD_ਨਕਾਬ_ALT))
	    && c >= 0 && c <= 127)
    {
	// Some terminals (esp. Kitty) do not include Shift in the character.
	// Apply it here to get consistency across terminals.  Only do ASCII
	// letters, for other characters it depends on the keyboard layout.
	ਜੇ ((*modifiers & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ) && c >= 'a' && c <= 'z')
	{
	    c += 'a' - 'A';
	    *modifiers &= ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
	}
	c += 0x80;
	*modifiers &= ~(MOD_ਨਕਾਬ_META | MOD_ਨਕਾਬ_ALT);
    }

    ਵਾਪਸ c;
}

/*
 * Add a single byte to 'showcmd' for a partially matched mapping.
 * Call add_to_showcmd() if a full key has been received.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_byte_ਮੰਜ਼ਿਲ_showcmd(char_u byte)
{
    ਸਥਿਰ gotchars_state_T state;
    char_u	*ptr;
    ਪੂਰਨ		modifiers = 0;
    ਪੂਰਨ		c = NUL;

    ਜੇ (!p_sc || ਸੁਨੇਹਾ_silent != 0)
	ਵਾਪਸ;

    ਜੇ (!gotchars_ਜੋੜੋ_byte(&state, byte))
	ਵਾਪਸ;

    state.buf[state.buflen] = NUL;
    state.buflen = 0;

    ptr = state.buf;
    ਜੇ (ptr[0] == K_SPECIAL && ptr[1] == KS_MODIFIER && ptr[2] != NUL)
    {
	modifiers = ptr[2];
	ptr += 3;
    }

    ਜੇ (*ptr != NUL)
    {
	char_u		*mb_ptr = mb_unescape(&ptr);

	c = mb_ptr != ਨਿਰਾ ? (*mb_ptr2char)(mb_ptr) : *ptr++;
	ਜੇ (c <= 0x7f)
	{
	    // Merge modifiers into the key to make the result more readable.
	    ਪੂਰਨ		modifiers_after = modifiers;
	    ਪੂਰਨ		mod_c = ਮਿਲਾਓ_modify_Other_ਕੁੰਜੀਆਂ(c, &modifiers_after);

	    ਜੇ (modifiers_after == 0)
	    {
		modifiers = 0;
		c = mod_c;
	    }
	}
    }

    // TODO: is there a more readable and yet compact representation of
    // modifiers and special keys?
    ਜੇ (modifiers != 0)
    {
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd(K_SPECIAL);
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd(KS_MODIFIER);
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd(modifiers);
    }
    ਜੇ (c != NUL)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd(c);
    ਜਦੋਂ (*ptr != NUL)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd(*ptr++);
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
    ਪੂਰਨ
vgetc(ਖਾਲੀ)
{
    ਪੂਰਨ		c, c2;
    ਪੂਰਨ		n;
    char_u	buf[MB_MAXBYTES + 1];
    ਪੂਰਨ		i;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    ਜੇ (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#ਜੇ_ਅੰਤ

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    ਜੇ (can_get_old_char())
    {
	c = old_char;
	old_char = -1;
	mod_ਨਕਾਬ = old_mod_ਨਕਾਬ;
	mouse_ਪੰਕਤੀ = old_mouse_ਪੰਕਤੀ;
	mouse_ਖਾਨਾ = old_mouse_ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	// number of characters recorded from the last vgetc() call
	ਸਥਿਰ size_t	ਆਖਰੀ_vgetc_recorded_len = 0;

	mod_ਨਕਾਬ = 0;
	vgetc_mod_ਨਕਾਬ = 0;
	vgetc_char = 0;

	// last_recorded_len can be larger than last_vgetc_recorded_len
	// if peeking records more
	ਆਖਰੀ_recorded_len -= ਆਖਰੀ_vgetc_recorded_len;

	ਲਈ (;;)		// this is done twice if there are modifiers
	{
	    ਪੂਰਨ did_inc = FALSE;

	    // No mapping after modifier has been read, using an input method
	    // and when a popup window has disabled mapping.
	    ਜੇ (mod_ਨਕਾਬ
#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#ਜੇ_ਅੰਤ
		    )
	    {
		++no_mapping;
		++allow_ਕੁੰਜੀਆਂ;
		// mod_mask value may change, remember we did the increment
		did_inc = TRUE;
	    }
	    c = vgetorpeek(TRUE);
	    ਜੇ (did_inc)
	    {
		--no_mapping;
		--allow_ਕੁੰਜੀਆਂ;
	    }

	    // Get two extra bytes for special keys, handle modifiers.
	    ਜੇ (c == K_SPECIAL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    || c == CSI
#ਜੇ_ਅੰਤ
	       )
	    {
		ਪੂਰਨ	    save_allow_ਕੁੰਜੀਆਂ = allow_ਕੁੰਜੀਆਂ;

		++no_mapping;
		allow_ਕੁੰਜੀਆਂ = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_ਕੁੰਜੀਆਂ = save_allow_ਕੁੰਜੀਆਂ;
		ਜੇ (c2 == KS_MODIFIER)
		{
		    mod_ਨਕਾਬ = c;
		    ਜਾਰੀ;
		}
		c = ਮੰਜ਼ਿਲ_SPECIAL(c2, c);

		ਜੇ (allow_osc_ਕੁੰਜੀ == 0 && c == K_OSC)
		    ਜਾਰੀ;

		// K_ESC is used to avoid ambiguity with the single Esc
		// character that might be the start of an escape sequence.
		// Convert it back to a single Esc here.
		ਜੇ (c == K_ESC)
		    c = ESC;

#ਜੇ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		// Handle K_TEAROFF here, the caller of vgetc() doesn't need to
		// know that a menu was torn off
		ਜੇ (
# ifdef VIMDLL
		    gui.ਵਿੱਚ_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    ਪੂਰਨ		j;

		    // get menu path, it ends with a <CR>
		    ਲਈ (j = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[j] = c;
			ਜੇ (j < 199)
			    ++j;
		    }
		    name[j] = NUL;
		    gui_make_tearoff(name);
		    ਜਾਰੀ;
		}
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && !defined(USE_GTK4) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		ਜੇ (c == K_F10 && gui.menubar != ਨਿਰਾ)
		{
		    gtk_menu_shell_select_ਪਹਿਲਾ(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    ਜਾਰੀ;
		}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		// Handle focus event here, so that the caller doesn't need to
		// know about it.  Return K_IGNORE so that we loop once (needed
		// if 'lazyredraw' is set).
		ਜੇ (c == K_FOCUSGAINED || c == K_FOCUSLOST)
		{
		    ui_focus_change(c == K_FOCUSGAINED);
		    c = K_IGNORE;
		}

		// Translate K_CSI to CSI.  The special key is only used to
		// avoid it being recognized as the start of a special key.
		ਜੇ (c == K_CSI)
		    c = CSI;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਜੇ (c == K_SID)
		{
		    ਪੂਰਨ	    j;

		    // Handle <SID>{sid};  Do up to 20 digits for safety.
		    ਆਖਰੀ_ਵਰਤਿਆ_sid = 0;
		    ਲਈ (j = 0; j < 20 && ਸੁਰੱਖਿਅਤ_isdigit(c = vgetorpeek(TRUE)); ++j)
			ਆਖਰੀ_ਵਰਤਿਆ_sid = ਆਖਰੀ_ਵਰਤਿਆ_sid * 10 + (c - '0');
		    ਆਖਰੀ_ਵਰਤਿਆ_map = ਨਿਰਾ;
		    ਜਾਰੀ;
		}
#ਜੇ_ਅੰਤ
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    ਜੇ (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		ਲਈ (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    ਜੇ (buf[i] == K_SPECIAL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			    || (buf[i] == CSI)
#ਜੇ_ਅੰਤ
			    )
		    {
			// Must be a K_SPECIAL - KS_SPECIAL - KE_FILLER
			// sequence, which represents a K_SPECIAL (0x80),
			// or a CSI - KS_EXTRA - KE_CSI sequence, which
			// represents a CSI (0x9B),
			// or a K_SPECIAL - KS_EXTRA - KE_CSI, which is CSI
			// too.
			c = vgetorpeek(TRUE);
			ਜੇ (vgetorpeek(TRUE) == KE_CSI && c == KS_EXTRA)
			    buf[i] = CSI;
		    }
		}
		--no_mapping;
		c = (*mb_ptr2char)(buf);
	    }

	    ਜੇ (vgetc_char == 0)
	    {
		vgetc_mod_ਨਕਾਬ = mod_ਨਕਾਬ;
		vgetc_char = c;
	    }

	    // A keypad or special function key was not mapped, use it like
	    // its ASCII equivalent.
	    ਚੋਣ (c)
	    {
		ਹਾਲਤ K_KPLUS:	c = '+'; ਰੋਕੋ;
		ਹਾਲਤ K_KMINUS:	c = '-'; ਰੋਕੋ;
		ਹਾਲਤ K_KDIVIDE:	c = '/'; ਰੋਕੋ;
		ਹਾਲਤ K_KMULTIPLY: c = '*'; ਰੋਕੋ;
		ਹਾਲਤ K_KENTER:	c = CAR; ਰੋਕੋ;
		ਹਾਲਤ K_KPOINT:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = Map_Virtual_ਕੁੰਜੀ(VK_DECIMAL, 2); ਰੋਕੋ;
#ਨਹੀਂਤਾਂ
				c = '.'; ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		ਹਾਲਤ K_K0:	c = '0'; ਰੋਕੋ;
		ਹਾਲਤ K_K1:	c = '1'; ਰੋਕੋ;
		ਹਾਲਤ K_K2:	c = '2'; ਰੋਕੋ;
		ਹਾਲਤ K_K3:	c = '3'; ਰੋਕੋ;
		ਹਾਲਤ K_K4:	c = '4'; ਰੋਕੋ;
		ਹਾਲਤ K_K5:	c = '5'; ਰੋਕੋ;
		ਹਾਲਤ K_K6:	c = '6'; ਰੋਕੋ;
		ਹਾਲਤ K_K7:	c = '7'; ਰੋਕੋ;
		ਹਾਲਤ K_K8:	c = '8'; ਰੋਕੋ;
		ਹਾਲਤ K_K9:	c = '9'; ਰੋਕੋ;

		ਹਾਲਤ K_XHOME:
		ਹਾਲਤ K_ZHOME:	ਜੇ (mod_ਨਕਾਬ == MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
				{
				    c = K_S_HOME;
				    mod_ਨਕਾਬ = 0;
				}
				ਨਹੀਂਤਾਂ ਜੇ (mod_ਨਕਾਬ == MOD_ਨਕਾਬ_CTRL)
				{
				    c = K_C_HOME;
				    mod_ਨਕਾਬ = 0;
				}
				ਨਹੀਂਤਾਂ
				    c = K_HOME;
				ਰੋਕੋ;
		ਹਾਲਤ K_XEND:
		ਹਾਲਤ K_ZEND:	ਜੇ (mod_ਨਕਾਬ == MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
				{
				    c = K_S_ਸਿਰਾ;
				    mod_ਨਕਾਬ = 0;
				}
				ਨਹੀਂਤਾਂ ਜੇ (mod_ਨਕਾਬ == MOD_ਨਕਾਬ_CTRL)
				{
				    c = K_C_ਸਿਰਾ;
				    mod_ਨਕਾਬ = 0;
				}
				ਨਹੀਂਤਾਂ
				    c = K_ਸਿਰਾ;
				ਰੋਕੋ;

		ਹਾਲਤ K_XUP:	c = K_ਉੱਪਰ; ਰੋਕੋ;
		ਹਾਲਤ K_XDOWN:	c = K_ਥੱਲੇ; ਰੋਕੋ;
		ਹਾਲਤ K_XLEFT:	c = K_ਖੱਬਾ; ਰੋਕੋ;
		ਹਾਲਤ K_XRIGHT:	c = K_ਸੱਜਾ; ਰੋਕੋ;
	    }

	    ਰੋਕੋ;
	}

	ਆਖਰੀ_vgetc_recorded_len = ਆਖਰੀ_recorded_len;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    /*
     * In the main loop "may_garbage_collect" can be set to do garbage
     * collection in the first next vgetc().  It's disabled after that to
     * avoid internally used Lists and Dicts to be freed.
     */
    may_garbage_collect = FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
    ਜੇ (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD
	    && c != K_COMPLETE_DELAY)
    {
	// Don't trigger 'balloonexpr' unless only the mouse was moved.
	bevalexpr_due_ਸੈੱਟ = FALSE;
	ui_remove_balloon();
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // Only filter keys that do not come from ":normal".  Keys from feedkeys()
    // are filtered.
    ਜੇ ((!ex_ਸਾਧਾਰਨ_busy || ਵਿੱਚ_feedkeys) && popup_do_filter(c))
    {
	ਜੇ (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    c = do_ਕੁੰਜੀ_ਨਿਵੇਸ਼_pre(c);

    // Clear the next typedchars_pos
    typedchars_pos = 0;
#ਜੇ_ਅੰਤ

    // Need to process the character before we know it's safe to do something
    // else.
    ਜੇ (c != K_IGNORE)
	state_no_longer_ਸੁਰੱਖਿਅਤ("key typed");

    ਵਾਪਸ c;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Handle the KeyInputPre autocommand.
 * "c" is the character that was typed.
 * Return new input character.
 */
    ਸਥਿਰ ਪੂਰਨ
do_ਕੁੰਜੀ_ਨਿਵੇਸ਼_pre(ਪੂਰਨ c)
{
    ਪੂਰਨ		res = c;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    char_u	curr_ਬਹੁਲਕ[ਬਹੁਲਕ_MAX_ਲੰਮਾਈ];
    ਪੂਰਨ		save_State = State;
    save_v_event_T save_v_event;
    dict_T	*v_event;

    // Return quickly when there is nothing to do.
    ਜੇ (!has_keyinputpre())
	ਵਾਪਸ res;

    ਜੇ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = KEY2TERMCAP0(c);
	buf[2] = KEY2TERMCAP1(c);
	buflen = 3;
    }
    ਨਹੀਂਤਾਂ
	buflen = (*mb_char2bytes)(c, buf);
    buf[buflen] = NUL;

    typedchars[typedchars_pos] = NUL;
    vim_unescape_csi(typedchars);

    get_ਬਹੁਲਕ(curr_ਬਹੁਲਕ);

    // Lock the text to avoid weird things from happening.
    ++textlock;
    ਸੈੱਟ_vim_var_string(VV_CHAR, buf, (ਪੂਰਨ)buflen);  // set v:char

    v_event = get_v_event(&save_v_event);
    (ਖਾਲੀ)dict_ਜੋੜੋ_bool(v_event, "typed", ਕੁੰਜੀ_Typed);
    (ਖਾਲੀ)dict_ਜੋੜੋ_string_len(v_event, "typedchar", typedchars, typedchars_pos);

    ਜੇ (apply_autocmds(EVENT_KEYINPUTPRE, curr_ਬਹੁਲਕ, curr_ਬਹੁਲਕ, FALSE, curbuf))
    {
	string_T    v_char;

	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character.
	v_char.string = get_vim_var_str(VV_CHAR);
	ਜੇ (STRCMP(buf, v_char.string) != 0)
	{
	    v_char.ਲੰਮਾਈ = STRLEN(v_char.string);
	    // Convert special bytes when it is special string.
	    ਜੇ (v_char.ਲੰਮਾਈ >= 3 && v_char.string[0] == K_SPECIAL)
		res = TERMCAP2_ਕੁੰਜੀ(v_char.string[1], v_char.string[2]);
	    ਨਹੀਂਤਾਂ ਜੇ (v_char.ਲੰਮਾਈ > 0)
		res = PTR2CHAR(v_char.string);
	}
    }

    restore_v_event(v_event, &save_v_event);

    ਸੈੱਟ_vim_var_string(VV_CHAR, ਨਿਰਾ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    ਵਾਪਸ res;
}
#ਜੇ_ਅੰਤ

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    ਪੂਰਨ
ਸੁਰੱਖਿਅਤ_vgetc(ਖਾਲੀ)
{
    ਪੂਰਨ	c;

    c = vgetc();
    ਜੇ (c == NUL)
	c = get_keystroke();
    ਵਾਪਸ c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 * Does not handle bracketed paste - do not use the result for commands.
 */
    ਸਥਿਰ ਪੂਰਨ
plain_vgetc_nopaste(ਖਾਲੀ)
{
    ਪੂਰਨ c;

    ਕਰੋ
	c = ਸੁਰੱਖਿਅਤ_vgetc();
    ਜਦੋਂ (c == K_IGNORE
	    || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
	    || c == K_MOUSEMOVE);
    ਵਾਪਸ c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    ਪੂਰਨ
plain_vgetc(ਖਾਲੀ)
{
    ਪੂਰਨ c = plain_vgetc_nopaste();

    ਜੇ (c == K_PS)
	// Only handle the first pasted character.  Drop the rest, since we
	// don't know what to do with it.
	c = bracketed_paste(PASTE_ONE_CHAR, FALSE, ਨਿਰਾ);

    ਵਾਪਸ c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 * Returns NUL if no character is available.
 */
    ਪੂਰਨ
vpeekc(ਖਾਲੀ)
{
    ਜੇ (can_get_old_char())
	ਵਾਪਸ old_char;
    ਵਾਪਸ vgetorpeek(FALSE);
}

#ਜੇ defined(FEAT_TERMRESPONSE) || defined(FEAT_TERMINAL)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    ਪੂਰਨ
vpeekc_nomap(ਖਾਲੀ)
{
    ਪੂਰਨ		c;

    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;
    c = vpeekc();
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;
    ਵਾਪਸ c;
}
#ਜੇ_ਅੰਤ

/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    ਪੂਰਨ
vpeekc_any(ਖਾਲੀ)
{
    ਪੂਰਨ		c;

    c = vpeekc();
    ਜੇ (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    ਵਾਪਸ c;
}

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    ਪੂਰਨ
char_avail(ਖਾਲੀ)
{
    ਪੂਰਨ	    retval;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    ਜੇ (disable_char_avail_for_testing)
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    ਵਾਪਸ (retval != NUL);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * "getchar()" and "getcharstr()" functions
 */
    ਸਥਿਰ ਖਾਲੀ
getchar_common(typval_T *argvars, typval_T *rettv, ਪੂਰਨ allow_ਸੰਖਿਆ)
{
    varnumber_T		n = 0;
    ਪੂਰਨ			called_emsg_ਸ਼ੁਰੂ = called_emsg;
    ਪੂਰਨ			error = FALSE;
    ਪੂਰਨ			simplify = TRUE;
    char_u		cursor_flag = 'm';

    ਜੇ ((ਵਿੱਚ_vim9script()
		&& check_for_opt_bool_or_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	    || (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type == VAR_DICT)
    {
	dict_T		*d = argvars[1].vval.v_dict;
	char_u		*cursor_str;

	ਜੇ (allow_ਸੰਖਿਆ)
	    allow_ਸੰਖਿਆ = dict_get_bool(d, "number", TRUE);
	ਨਹੀਂਤਾਂ ਜੇ (dict_has_ਕੁੰਜੀ(d, "number"))
	    semsg(_(e_invalid_ਦਲੀਲ_str), "number");

	simplify = dict_get_bool(d, "simplify", TRUE);

	cursor_str = dict_get_string(d, "cursor", FALSE);
	ਜੇ (cursor_str != ਨਿਰਾ)
	{
	    ਜੇ (STRCMP(cursor_str, "hide") != 0
		    && STRCMP(cursor_str, "keep") != 0
		    && STRCMP(cursor_str, "msg") != 0)
		semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "cursor",
								   cursor_str);
	    ਨਹੀਂਤਾਂ
		cursor_flag = cursor_str[0];
	}
    }

    ਜੇ (called_emsg != called_emsg_ਸ਼ੁਰੂ)
	ਵਾਪਸ;

# ifdef ਸੁਨੇਹਾ_ਕਤਾਰ
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
# endif

    ਜੇ (cursor_flag == 'h')
	cursor_sleep();
    ਨਹੀਂਤਾਂ ਜੇ (cursor_flag == 'm')
	windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);

    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;
    ++allow_osc_ਕੁੰਜੀ;
    ਜੇ (!simplify)
	++no_reduce_ਕੁੰਜੀਆਂ;
    ਲਈ (;;)
    {
	ਜੇ (argvars[0].v_type == VAR_UNKNOWN
		|| (argvars[0].v_type == VAR_ਸੰਖਿਆ
			&& argvars[0].vval.v_ਸੰਖਿਆ == -1))
	    // getchar(): blocking wait.
	    n = plain_vgetc_nopaste();
	ਨਹੀਂਤਾਂ ਜੇ (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	ਨਹੀਂਤਾਂ ਜੇ (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	ਨਹੀਂਤਾਂ
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = ਸੁਰੱਖਿਅਤ_vgetc();

	ਜੇ (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    ਜਾਰੀ;
	ਰੋਕੋ;
    }
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;
    --allow_osc_ਕੁੰਜੀ;
    ਜੇ (!simplify)
	--no_reduce_ਕੁੰਜੀਆਂ;

    ਜੇ (cursor_flag == 'h')
	cursor_unsleep();

    ਸੈੱਟ_vim_var_nr(VV_MOUSE_WIN, 0);
    ਸੈੱਟ_vim_var_nr(VV_MOUSE_WINID, 0);
    ਸੈੱਟ_vim_var_nr(VV_MOUSE_LNUM, 0);
    ਸੈੱਟ_vim_var_nr(VV_MOUSE_ਖਾਨਾ, 0);

    ਜੇ (n != 0 && (!allow_ਸੰਖਿਆ || IS_SPECIAL(n) || mod_ਨਕਾਬ != 0))
    {
	char_u		ਆਰਜ਼ੀ[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	ਪੂਰਨ		i = 0;

	// Turn a special key into three bytes, plus modifier.
	ਜੇ (mod_ਨਕਾਬ != 0)
	{
	    ਆਰਜ਼ੀ[i++] = K_SPECIAL;
	    ਆਰਜ਼ੀ[i++] = KS_MODIFIER;
	    ਆਰਜ਼ੀ[i++] = mod_ਨਕਾਬ;
	}
	ਜੇ (IS_SPECIAL(n))
	{
	    ਆਰਜ਼ੀ[i++] = K_SPECIAL;
	    ਆਰਜ਼ੀ[i++] = K_ਦੂਜਾ(n);
	    ਆਰਜ਼ੀ[i++] = K_ਤੀਜਾ(n);
	}
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	    i += (*mb_char2bytes)(n, ਆਰਜ਼ੀ + i);
	ਨਹੀਂਤਾਂ
	    ਆਰਜ਼ੀ[i++] = n;
	ਆਰਜ਼ੀ[i] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strnsave(ਆਰਜ਼ੀ, i);

	ਜੇ (is_mouse_ਕੁੰਜੀ(n))
	{
	    ਪੂਰਨ		ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
	    ਪੂਰਨ		ਖਾਨਾ = mouse_ਖਾਨਾ;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    ਪੂਰਨ		winnr = 1;

	    ਜੇ (ਪੰਕਤੀ >= 0 && ਖਾਨਾ >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);
		ਜੇ (win == ਨਿਰਾ)
		    ਵਾਪਸ;
		(ਖਾਲੀ)mouse_comp_pos(win, &ਪੰਕਤੀ, &ਖਾਨਾ, &lnum, ਨਿਰਾ);
# ifdef FEAT_PROP_POPUP
		ਜੇ (WIN_IS_POPUP(win))
		    winnr = 0;
		ਨਹੀਂਤਾਂ
# endif
		    ਲਈ (wp = firstwin; wp != win && wp != ਨਿਰਾ;
							       wp = wp->w_ਅਗਲਾ)
			++winnr;
		ਸੈੱਟ_vim_var_nr(VV_MOUSE_WIN, winnr);
		ਸੈੱਟ_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		ਸੈੱਟ_vim_var_nr(VV_MOUSE_LNUM, lnum);
		ਸੈੱਟ_vim_var_nr(VV_MOUSE_ਖਾਨਾ, ਖਾਨਾ + 1);
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (!allow_ਸੰਖਿਆ)
	rettv->v_type = VAR_STRING;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = n;
}

/*
 * "getchar()" function
 */
    ਖਾਲੀ
f_getchar(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, TRUE);
}

/*
 * "getcharstr()" function
 */
    ਖਾਲੀ
f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, FALSE);
}

/*
 * "getcharmod()" function
 */
    ਖਾਲੀ
f_getcharmod(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = mod_ਨਕਾਬ;
}
#ਜੇ_ਅੰਤ // FEAT_EVAL

#ਜੇ defined(ਸੁਨੇਹਾ_ਕਤਾਰ)
# define MAX_REPEAT_PARSE 8

/*
 * Process messages that have been queued for netbeans or clientserver.
 * Also check if any jobs have ended.
 * These functions can call arbitrary Vim script and should only be called when
 * it is safe to do so.
 */
    ਖਾਲੀ
parse_queued_messages(ਖਾਲੀ)
{
    ਪੂਰਨ	    old_curwin_id;
    ਪੂਰਨ	    old_curbuf_fnum;
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    save_may_garbage_collect = may_garbage_collect;
    ਸਥਿਰ ਪੂਰਨ entered = 0;
    ਪੂਰਨ	    was_ਸੁਰੱਖਿਅਤ = get_was_ਸੁਰੱਖਿਅਤ_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    ਜੇ (updating_screen || dont_parse_messages)
	ਵਾਪਸ;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    ਜੇ (curbuf == ਨਿਰਾ || curwin == ਨਿਰਾ)
	ਵਾਪਸ;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    ਲਈ (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# ਜੇ (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_ਕੰਮ_CHANNEL)
	channel_handle_events(FALSE);
# endif

# ifdef FEAT_NETBEANS_INTG
	// Process the queued netbeans messages.
	netbeans_parse_messages();
# endif
# ifdef FEAT_SOCKETSERVER
	socketserver_parse_messages();
# endif
# ifdef FEAT_ਕੰਮ_CHANNEL
	// Write any buffer lines still to be written.
	channel_write_any_ਸਤਰਾਂ();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# ਜੇ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_ਕੰਮ_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	ਜੇ (ਕੰਮ_check_ended())
	    ਜਾਰੀ;
# endif
# ifdef FEAT_TERMINAL
	free_ਅਣਵਰਤਿਆ_terminals();
# endif

# ifdef FEAT_SOUND_MACOSX
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	ਜੇ (has_sound_callback_ਵਿੱਚ_ਕਤਾਰ())
	    invoke_sound_callback();
# endif
# ifdef SIGUSR1
	ਜੇ (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
# endif
	ਰੋਕੋ;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    ਜੇ (entered == 1 && was_ਸੁਰੱਖਿਅਤ)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    ਜੇ (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}
#ਜੇ_ਅੰਤ


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    map_ਨਤੀਜਾ_fail,    // failed, break loop
    map_ਨਤੀਜਾ_get,     // get a character from typeahead
    map_ਨਤੀਜਾ_retry,   // try to map again
    map_ਨਤੀਜਾ_nomatch  // no matching mapping, get char
} map_ਨਤੀਜਾ_T;

/*
 * Check if the bytes at the start of the typeahead buffer are a character used
 * in Insert mode completion.  This includes the form with a CTRL modifier.
 * During completion started by complete() keys are mapped as usual.
 */
    ਸਥਿਰ ਪੂਰਨ
at_ins_compl_ਕੁੰਜੀ(ਖਾਲੀ)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    ਪੂਰਨ	    c = *p;

    ਜੇ (typebuf.tb_len > 3
	    && (c == K_SPECIAL || c == CSI)  // CSI is used by the GUI
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_ਨਕਾਬ_CTRL)
	    // CTRL-SHIFT-N/P scroll the info popup, so they must not be folded
	    // to the CTRL-N/CTRL-P completion keys here.
	    && !(p[2] & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ))
	c = p[3] & 0x1f;
    ਵਾਪਸ !ctrl_x_ਬਹੁਲਕ_eval()
	    && ((ctrl_x_ਬਹੁਲਕ_not_default() && vim_is_ctrl_x_ਕੁੰਜੀ(c))
		|| (compl_ਸਥਿਤੀ_local() && (c == Ctrl_N || c == Ctrl_P)));
}

/*
 * Check if typebuf.tb_buf[] contains a modifier plus key that can be changed
 * into just a key, apply that.
 * Check from typebuf.tb_buf[typebuf.tb_off] to typebuf.tb_buf[typebuf.tb_off
 * + "max_offset"].
 * Return the length of the replaced bytes, 0 if nothing changed, -1 for error.
 */
    ਸਥਿਰ ਪੂਰਨ
check_simplify_modifier(ਪੂਰਨ max_ਵਿੱਥ)
{
    ਪੂਰਨ		ਵਿੱਥ;
    char_u	*tp;

    ਲਈ (ਵਿੱਥ = 0; ਵਿੱਥ < max_ਵਿੱਥ; ++ਵਿੱਥ)
    {
	ਜੇ (ਵਿੱਥ + 3 >= typebuf.tb_len)
	    ਰੋਕੋ;
	tp = typebuf.tb_buf + typebuf.tb_off + ਵਿੱਥ;
	ਜੇ ((tp[0] == K_SPECIAL || tp[0] == CSI) && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    ਪੂਰਨ modifier = tp[2];
	    ਪੂਰਨ	c = tp[3];
	    ਪੂਰਨ new_c = ਮਿਲਾਓ_modify_Other_ਕੁੰਜੀਆਂ(c, &modifier);

	    ਜੇ (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		ਪੂਰਨ	len;
		ਪੂਰਨ	ਕੁੰਜੀ_ਵਿੱਥ = ਵਿੱਥ;

		ਜੇ (ਵਿੱਥ == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_ਨਕਾਬ = tp[2];
		}
		ਜੇ (IS_SPECIAL(new_c))
		{
		    new_string[0] = K_SPECIAL;
		    new_string[1] = K_ਦੂਜਾ(new_c);
		    new_string[2] = K_ਤੀਜਾ(new_c);
		    len = 3;
		}
		ਨਹੀਂਤਾਂ
		    len = mb_char2bytes(new_c, new_string);
		ਜੇ (modifier == 0)
		{
		    ਜੇ (put_string_ਵਿੱਚ_typebuf(ਵਿੱਥ, 4, new_string, len,
							ਨਿਰਾ, 0, ਨਿਰਾ) == FAIL)
			ਵਾਪਸ -1;
		}
		ਨਹੀਂਤਾਂ
		{
		    tp[2] = modifier;
		    ਕੁੰਜੀ_ਵਿੱਥ = ਵਿੱਥ + 3;
		    ਜੇ (put_string_ਵਿੱਚ_typebuf(ਕੁੰਜੀ_ਵਿੱਥ, 1, new_string, len,
							ਨਿਰਾ, 0, ਨਿਰਾ) == FAIL)
			ਵਾਪਸ -1;
		}

		// A mapping for the simplified key can be used for it even
		// when the key protocol is enabled.
		ਲਈ (ਪੂਰਨ i = 0; i < len; ++i)
		    typebuf.tb_noremap[typebuf.tb_off + ਕੁੰਜੀ_ਵਿੱਥ + i]
							    |= RM_SIMPLIFIED;
		ਵਾਪਸ len;
	    }
	}
    }
    ਵਾਪਸ 0;
}

/*
 * Return TRUE if the terminal sends modifiers with various keys.  This is when
 * modifyOtherKeys level 2 is enabled or the kitty keyboard protocol is
 * enabled.
 */
    ਪੂਰਨ
ਕੁੰਜੀ_protocol_enabled(ਖਾਲੀ)
{
    // If xterm has responded to XTQMODKEYS it overrules seenModifyOtherKeys.
    ਪੂਰਨ using_mok = modify_otherkeys_state != MOKS_INITIAL
			? modify_otherkeys_state == MOKS_ENABLED
			: ਵੇਖਿਆ_Modify_Other_ਕੁੰਜੀਆਂ;
    ਵਾਪਸ using_mok || kitty_protocol_state == KKPS_ENABLED;
}

/*
 * Handle mappings in the typeahead buffer.
 * - When something was mapped, return map_result_retry for recursive mappings.
 * - When nothing mapped and typeahead has a character: return map_result_get.
 * - When there is no match yet, return map_result_nomatch, need to get more
 *   typeahead.
 * - On failure (out of memory) return map_result_fail.
 */
    ਸਥਿਰ ਪੂਰਨ
handle_mapping(
	    ਪੂਰਨ *keylenp,
	    ਪੂਰਨ *timedout,
	    ਪੂਰਨ *mapdepth)
{
    mapblock_T	*mp = ਨਿਰਾ;
    mapblock_T	*mp2;
    mapblock_T	*mp_ਮੇਲ;
    ਪੂਰਨ		mp_ਮੇਲ_len = 0;
    ਪੂਰਨ		max_mlen = 0;
    ਪੂਰਨ		want_termcode = 0;  // 1 if termcode expected after max_mlen
    ਪੂਰਨ		tb_c1;
    ਪੂਰਨ		mlen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
    ਪੂਰਨ		nolmaplen;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		keylen = *keylenp;
    ਪੂਰਨ		i;
    ਪੂਰਨ		local_State = get_real_state();
    ਪੂਰਨ		is_plug_map = FALSE;
    bool	ਵਿੱਚ_osc = ਵਿੱਚ_osc_sequence(); // If we are in an OSC sequence,
					    // then go straight to
					    // check_termcode() in order to
					    // consume chars.

    // If typeahead starts with <Plug> then remap, even for a "noremap" mapping.
    ਜੇ (!ਵਿੱਚ_osc && typebuf.tb_len >= 3
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
    ਜੇ (!ਵਿੱਚ_osc && no_mapping == 0 && is_maphash_valid()
	    && (no_zero_mapping == 0 || tb_c1 != '0')
	    && (typebuf.tb_maplen == 0 || is_plug_map
		|| (p_remap
		    && (typebuf.tb_noremap[typebuf.tb_off]
				    & (RM_NONE|RM_ABBR)) == 0))
	    && !(p_paste && (State & (ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE)))
	    && !(State == ਬਹੁਲਕ_HITRETURN && (tb_c1 == CAR || tb_c1 == ' '))
	    && State != ਬਹੁਲਕ_ASKMORE
	    && State != ਬਹੁਲਕ_CONFIRM
	    && !at_ins_compl_ਕੁੰਜੀ())
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
	ਜੇ (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	ਨਹੀਂਤਾਂ
	{
	    LANGMAP_ADJUST(tb_c1, (State & (ਬਹੁਲਕ_CMDLINE | ਬਹੁਲਕ_ਪਾਓ)) == 0
					   && get_real_state() != ਬਹੁਲਕ_SELECT);
	    nolmaplen = 0;
	}
#ਜੇ_ਅੰਤ
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	ਜੇ (mp == ਨਿਰਾ)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = ਨਿਰਾ;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_ਮੇਲ = ਨਿਰਾ;
	mp_ਮੇਲ_len = 0;
	ਲਈ ( ; mp != ਨਿਰਾ;
	       mp->m_ਅਗਲਾ == ਨਿਰਾ ? (mp = mp2, mp2 = ਨਿਰਾ) : (mp = mp->m_ਅਗਲਾ))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    ਜੇ (mp->m_ਕੁੰਜੀਆਂ[0] == tb_c1
		    && (mp->m_ਬਹੁਲਕ & local_State)
		    && !(mp->m_simplified && ਕੁੰਜੀ_protocol_enabled()
			    && typebuf.tb_maplen == 0
			    && (typebuf.tb_noremap[typebuf.tb_off]
							& RM_SIMPLIFIED) == 0)
		    && ((mp->m_ਬਹੁਲਕ & ਬਹੁਲਕ_LANGMAP) == 0
						    || typebuf.tb_maplen == 0))
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
		ਪੂਰਨ	nomap = nolmaplen;
		ਪੂਰਨ	modifiers = 0;
#ਜੇ_ਅੰਤ
		// find the match length of this mapping
		ਲਈ (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
		    ਪੂਰਨ	c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
		    ਜੇ (nomap > 0)
		    {
			ਜੇ (nomap == 2 && c2 == KS_MODIFIER)
			    modifiers = 1;
			ਨਹੀਂਤਾਂ ਜੇ (nomap == 1 && modifiers == 1)
			    modifiers = c2;
			--nomap;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (c2 == K_SPECIAL)
			    nomap = 2;
			ਨਹੀਂਤਾਂ ਜੇ (ਮਿਲਾਓ_modify_Other_ਕੁੰਜੀਆਂ(c2, &modifiers) == c2)
			    // Only apply 'langmap' if merging modifiers into
			    // the key will not result in another character,
			    // so that 'langmap' behaves consistently in
			    // different terminals and GUIs.
			    LANGMAP_ADJUST(c2, TRUE);
			modifiers = 0;
		    }
#ਜੇ_ਅੰਤ
		    ਜੇ (mp->m_ਕੁੰਜੀਆਂ[mlen] != c2)
			ਰੋਕੋ;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_ਕੁੰਜੀਆਂ;
		    char_u *p2 = mb_unescape(&p1);

		    ਜੇ (has_mbyte && p2 != ਨਿਰਾ
					&& MB_BYTE2LEN(tb_c1) > mb_ptr2len(p2))
			mlen = 0;
		}

		// Check an entry whether it matches.
		// - Full match: mlen == keylen
		// - Partly match: mlen == typebuf.tb_len
		keylen = mp->m_keylen;
		ਜੇ (mlen == keylen || (mlen == typebuf.tb_len
						   && typebuf.tb_len < keylen))
		{
		    char_u  *s;
		    ਪੂਰਨ	    n;

		    // If only script-local mappings are allowed, check if the
		    // mapping starts with K_SNR.
		    s = typebuf.tb_noremap + typebuf.tb_off;
		    ਜੇ ((*s & ~RM_SIMPLIFIED) == RM_SCRIPT
			    && (mp->m_ਕੁੰਜੀਆਂ[0] != K_SPECIAL
				|| mp->m_ਕੁੰਜੀਆਂ[1] != KS_EXTRA
				|| mp->m_ਕੁੰਜੀਆਂ[2] != KE_SNR))
			ਜਾਰੀ;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    ਲਈ (n = mlen; --n >= 0; )
			ਜੇ (*s++ & (RM_NONE|RM_ABBR))
			    ਰੋਕੋ;
		    ਜੇ (!is_plug_map && n >= 0)
			ਜਾਰੀ;

		    ਜੇ (keylen > typebuf.tb_len)
		    {
			ਜੇ (!*timedout && !(mp_ਮੇਲ != ਨਿਰਾ
							&& mp_ਮੇਲ->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    ਰੋਕੋ;
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (keylen > mp_ਮੇਲ_len)
		    {
			// found a longer match
			mp_ਮੇਲ = mp;
			mp_ਮੇਲ_len = keylen;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // No match; may have to check for termcode at next
		    // character.

		    // If the first character that didn't match is
		    // K_SPECIAL then check for a termcode.  This isn't perfect
		    // but should work in most cases.
		    ਜੇ (max_mlen < mlen)
		    {
			max_mlen = mlen;
			want_termcode = mp->m_ਕੁੰਜੀਆਂ[mlen] == K_SPECIAL;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (max_mlen == mlen && mp->m_ਕੁੰਜੀਆਂ[mlen] == K_SPECIAL)
			want_termcode = 1;

		    // Check termcode for uppercase character to properly
		    // process "ESC[27;2;<ascii code>~" control sequences.
		    ਜੇ (ASCII_ISUPPER(mp->m_ਕੁੰਜੀਆਂ[mlen]))
			want_termcode = 1;
		}
	    }
	}

	// If no partly match found, use the longest full match.
	ਜੇ (keylen != KEYLEN_PART_MAP && mp_ਮੇਲ != ਨਿਰਾ)
	{
	    mp = mp_ਮੇਲ;
	    keylen = mp_ਮੇਲ_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    ਜੇ (!ਵਿੱਚ_osc && *p_pt != NUL && mp == ਨਿਰਾ &&
	    (State & (ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_ਸਾਧਾਰਨ)))
    {
	ਲਈ (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    ਜੇ (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    ਰੋਕੋ;
	ਜੇ (p_pt[mlen] == NUL)	//ਮੇਲ
	{
	    // write chars to script file(s)
	    ਜੇ (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"paste",
						      (ਲੰਮਾ)!p_paste, ਨਿਰਾ, 0);
	    ਜੇ (!(State & ਬਹੁਲਕ_ਪਾਓ))
	    {
		ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
		ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
		ਸੁਨੇਹਾ_clr_eos();		// clear ruler
	    }
	    ਸਥਿਤੀ_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    ਵਾਪਸ map_ਨਤੀਜਾ_retry;
	}
	// Need more chars for partly match.
	ਜੇ (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_ਕੁੰਜੀ;
	ਨਹੀਂਤਾਂ ਜੇ (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    // May check for a terminal code when there is no mapping or only a partial
    // mapping.  Also check if there is a full mapping with <Esc>, unless timed
    // out, since that is nearly always a partial match with a terminal code.
    ਜੇ (ਵਿੱਚ_osc || ((mp == ਨਿਰਾ || max_mlen + want_termcode > mp_ਮੇਲ_len
		    || (mp_ਮੇਲ_len == 1 && *mp->m_ਕੁੰਜੀਆਂ == ESC && !*timedout))
	    && keylen != KEYLEN_PART_MAP))
    {
	ਪੂਰਨ	save_keylen = keylen;

	/*
	 * When no matching mapping found or found a non-matching mapping that
	 * matches at least what the matching mapping matched:
	 * Check if we have a terminal code, when:
	 * - mapping is allowed,
	 * - keys have not been mapped,
	 * - and not an ESC sequence, not in insert mode or p_ek is on,
	 * - and when not timed out,
	 */
	ਜੇ (ਵਿੱਚ_osc || no_mapping == 0 || allow_ਕੁੰਜੀਆਂ != 0)
	{
	    ਜੇ (ਵਿੱਚ_osc || ((typebuf.tb_maplen == 0
			    || (p_remap && (typebuf.tb_noremap[typebuf.tb_off]
						& ~RM_SIMPLIFIED) == RM_YES))
		    && !*timedout))
		keylen = check_termcode(max_mlen + 1, ਨਿਰਾ, 0, ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
		keylen = 0;

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    ਜੇ (keylen == 0 && save_keylen == KEYLEN_PART_ਕੁੰਜੀ && !*timedout)
		keylen = KEYLEN_PART_ਕੁੰਜੀ;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    check_no_reduce_ਕੁੰਜੀਆਂ();  // may update the no_reduce_keys flag
#ਜੇ_ਅੰਤ
	    ਜੇ (keylen == 0 && !no_reduce_ਕੁੰਜੀਆਂ)
	    {
		keylen = check_simplify_modifier(max_mlen + 1);
		ਜੇ (keylen < 0)
		    // ins_typebuf() failed
		    ਵਾਪਸ map_ਨਤੀਜਾ_fail;
	    }

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    ਜੇ (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	ਨਹੀਂਤਾਂ
	    keylen = 0;
	ਜੇ (keylen == 0)	// no matching terminal code
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
	    // check for window bounds report
	    ਜੇ (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		ਲਈ (s = typebuf.tb_buf + typebuf.tb_off + 1;
			   s < typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len
		   && (VIM_ISDIGIT(*s) || *s == ';' || *s == ' ');
			++s)
		    ;
		ਜੇ (*s == 'r' || *s == '|') // found one
		{
		    del_typebuf(
			  (ਪੂਰਨ)(s + 1 - (typebuf.tb_buf + typebuf.tb_off)), 0);
		    // get size and redraw screen
		    shell_resized();
		    *keylenp = keylen;
		    ਵਾਪਸ map_ਨਤੀਜਾ_retry;
		}
		ਜੇ (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_ਕੁੰਜੀ;
	    }
	    ਜੇ (keylen >= 0)
#ਜੇ_ਅੰਤ
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		ਜੇ (mp == ਨਿਰਾ)
		{
		    *keylenp = keylen;
		    ਵਾਪਸ map_ਨਤੀਜਾ_get;    // get character from typeahead
		}
	}

	ਜੇ (keylen > 0)	    // full matching terminal code
	{
#ਜੇ defined(FEAT_GUI) && defined(FEAT_MENU)
	    ਜੇ (typebuf.tb_len >= 2
		    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
			      && typebuf.tb_buf[typebuf.tb_off + 1] == KS_MENU)
	    {
		ਪੂਰਨ	idx;

		// Using a menu may cause a break in undo!  It's like using
		// gotchars(), but without recording or writing to a script
		// file.
		may_sync_undo();
		del_typebuf(3, 0);
		idx = get_menu_ਸੂਚਕਾਂਕ(current_menu, local_State);
		ਜੇ (idx != MENU_ਸੂਚਕਾਂਕ_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    ਜੇ (VIsual_ਸਰਗਰਮ && VIsual_select
					&& (current_menu->modes & ਬਹੁਲਕ_VISUAL))
		    {
			VIsual_select = FALSE;
			(ਖਾਲੀ)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->ਲੜੀਆਂ[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#ਜੇ_ਅੰਤ // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    ਵਾਪਸ map_ਨਤੀਜਾ_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	ਜੇ (mp == ਨਿਰਾ || keylen < 0)
	    keylen = KEYLEN_PART_ਕੁੰਜੀ;
	ਨਹੀਂਤਾਂ
	    keylen = mp_ਮੇਲ_len;
    }

    /*
     * complete match
     */
    ਜੇ (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਪੂਰਨ	save_m_ਵਿਅੰਜਕ;
	ਪੂਰਨ	save_m_noremap;
	ਪੂਰਨ	save_m_silent;
	char_u	*save_m_ਕੁੰਜੀਆਂ;
	char_u	*save_alt_m_ਕੁੰਜੀਆਂ;
	ਪੂਰਨ	save_alt_m_keylen;
#ਨਹੀਂਤਾਂ
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#ਜੇ_ਅੰਤ

	// write chars to script file(s)
	ਜੇ (keylen > typebuf.tb_maplen)
	    gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						   keylen - typebuf.tb_maplen);

	cmd_silent = (typebuf.tb_silent > 0);
	del_typebuf(keylen, 0);	// remove the mapped keys

	/*
	 * Put the replacement string in front of mapstr.
	 * The depth check catches ":map x y" and ":map y x".
	 */
	ਜੇ (++*mapdepth >= p_mmd)
	{
	    emsg(_(e_recursive_mapping));
	    ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
		redrawcmdline();
	    ਨਹੀਂਤਾਂ
		setcursor();
	    flush_buffers(FLUSH_ਨਿਊਨਤਮ);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    ਵਾਪਸ map_ਨਤੀਜਾ_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	ਜੇ (VIsual_ਸਰਗਰਮ && VIsual_select && (mp->m_ਬਹੁਲਕ & ਬਹੁਲਕ_VISUAL))
	{
	    VIsual_select = FALSE;
	    (ਖਾਲੀ)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_ਵਿਅੰਜਕ = mp->m_ਵਿਅੰਜਕ;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_ਕੁੰਜੀਆਂ = ਨਿਰਾ;  // only saved when needed
	save_alt_m_ਕੁੰਜੀਆਂ = ਨਿਰਾ;  // only saved when needed
	save_alt_m_keylen = mp->m_alt != ਨਿਰਾ ? mp->m_alt->m_keylen : 0;

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	ਜੇ (mp->m_ਵਿਅੰਜਕ)
	{
	    ਪੂਰਨ save_vgetc_busy = vgetc_busy;
	    ਪੂਰਨ save_may_garbage_collect = may_garbage_collect;
	    ਪੂਰਨ was_screen_ਖਾਨਾ = screen_cur_ਖਾਨਾ;
	    ਪੂਰਨ was_screen_ਪੰਕਤੀ = screen_cur_ਪੰਕਤੀ;
	    ਪੂਰਨ prev_did_emsg = did_emsg;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_ਕੁੰਜੀਆਂ = vim_strnsave(mp->m_ਕੁੰਜੀਆਂ, (size_t)mp->m_keylen);
	    save_alt_m_ਕੁੰਜੀਆਂ = mp->m_alt != ਨਿਰਾ
				    ? vim_strnsave(mp->m_alt->m_ਕੁੰਜੀਆਂ,
					     (size_t)save_alt_m_keylen) : ਨਿਰਾ;
	    map_str = eval_map_ਵਿਅੰਜਕ(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_ਪੰਕਤੀ, was_screen_ਖਾਨਾ);
	    ਬਾਹਰਲਾ_flush();

	    // If an error was displayed and the expression returns an empty
	    // string, generate a <Nop> to allow for a redraw.
	    ਜੇ (prev_did_emsg != did_emsg
				       && (map_str == ਨਿਰਾ || *map_str == NUL))
	    {
		char_u	buf[4];

		vim_free(map_str);
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = KE_IGNORE;
		buf[3] = NUL;
		map_str = vim_strnsave(buf, 3);
		ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
		{
		    // redraw the command below the error
		    ਸੁਨੇਹਾ_didout = TRUE;
		    ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ < cmdline_ਪੰਕਤੀ)
			ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;
		    redrawcmd();
		}
	    }

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	ਜੇ (map_str == ਨਿਰਾ)
	    i = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ noremap;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਆਖਰੀ_ਵਰਤਿਆ_map = mp;
	    ਆਖਰੀ_ਵਰਤਿਆ_sid = -1;
#ਜੇ_ਅੰਤ
	    ਜੇ (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    ਨਹੀਂਤਾਂ ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		save_m_ਵਿਅੰਜਕ ?
		(save_m_ਕੁੰਜੀਆਂ != ਨਿਰਾ
			&& STRNCMP(map_str, save_m_ਕੁੰਜੀਆਂ, (size_t)keylen) == 0)
		|| (save_alt_m_ਕੁੰਜੀਆਂ != ਨਿਰਾ
			&& STRNCMP(map_str, save_alt_m_ਕੁੰਜੀਆਂ,
					    (size_t)save_alt_m_keylen) == 0) :
#ਜੇ_ਅੰਤ
		STRNCMP(map_str, mp->m_ਕੁੰਜੀਆਂ, (size_t)keylen) == 0
		|| (mp->m_alt != ਨਿਰਾ
			&& STRNCMP(map_str, mp->m_alt->m_ਕੁੰਜੀਆਂ,
					    (size_t)mp->m_alt->m_keylen) == 0))
		noremap = REMAP_SKIP;
	    ਨਹੀਂਤਾਂ
		noremap = REMAP_YES;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (save_m_ਵਿਅੰਜਕ)
		vim_free(map_str);
#ਜੇ_ਅੰਤ
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	vim_free(save_m_ਕੁੰਜੀਆਂ);
	vim_free(save_alt_m_ਕੁੰਜੀਆਂ);
#ਜੇ_ਅੰਤ
	*keylenp = keylen;
	ਜੇ (i == FAIL)
	    ਵਾਪਸ map_ਨਤੀਜਾ_fail;
	ਵਾਪਸ map_ਨਤੀਜਾ_retry;
    }

    *keylenp = keylen;
    ਵਾਪਸ map_ਨਤੀਜਾ_nomatch;
}

/*
 * unget one character (can only be done once!)
 * If the character was stuffed, vgetc() will get it next time it is called.
 * Otherwise vgetc() will only get it when the stuff buffer is empty.
 */
    ਖਾਲੀ
vungetc(ਪੂਰਨ c)
{
    old_char = c;
    old_mod_ਨਕਾਬ = mod_ਨਕਾਬ;
    old_mouse_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    old_mouse_ਖਾਨਾ = mouse_ਖਾਨਾ;
    old_ਕੁੰਜੀ_Stuffed = ਕੁੰਜੀ_Stuffed;
}

/*
 * When peeking and not getting a character, reg_executing cannot be cleared
 * yet, so set a flag to clear it later.
 */
    ਸਥਿਰ ਖਾਲੀ
check_ਸਿਰਾ_reg_executing(ਪੂਰਨ advance)
{
    ਜੇ (reg_executing != 0 && (typebuf.tb_maplen == 0
						|| pending_ਸਿਰਾ_reg_executing))
    {
	ਜੇ (advance)
	{
	    reg_executing = 0;
	    pending_ਸਿਰਾ_reg_executing = FALSE;
	}
	ਨਹੀਂਤਾਂ
	    pending_ਸਿਰਾ_reg_executing = TRUE;
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
    ਸਥਿਰ ਪੂਰਨ
vgetorpeek(ਪੂਰਨ advance)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		timedout = FALSE;	// waited for more than 'timeoutlen'
					// for mapping to complete or
					// 'ttimeoutlen' for complete key code
    ਪੂਰਨ		mapdepth = 0;		// check for recursive mapping
    ਪੂਰਨ		ਬਹੁਲਕ_deleted = FALSE;   // set when mode has been deleted
    ਪੂਰਨ		new_wcol, new_wrow;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਪੂਰਨ		shape_ਬਦਲਿਆ = FALSE;  // adjusted cursor shape
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		n;
    ਪੂਰਨ		old_wcol, old_wrow;
    ਪੂਰਨ		wait_tb_len;

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
    ਜੇ (vgetc_busy > 0 && ex_ਸਾਧਾਰਨ_busy == 0)
	ਵਾਪਸ NUL;

    ++vgetc_busy;

    ਜੇ (advance)
    {
	ਕੁੰਜੀ_Stuffed = FALSE;
	typebuf_was_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
    }

    init_typebuf();
    ਸ਼ੁਰੂ_stuff();
    check_ਸਿਰਾ_reg_executing(advance);
    ਕਰੋ
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	ਜੇ (typeahead_char != 0)
	{
	    c = typeahead_char;
	    ਜੇ (advance)
		typeahead_char = 0;
	}
	ਨਹੀਂਤਾਂ
	    c = read_readbuffers(advance);
	ਜੇ (c != NUL && !got_int)
	{
	    ਜੇ (advance)
	    {
		// KeyTyped = FALSE;  When the command that stuffed something
		// was typed, behave like the stuffed command was typed.
		// needed for CTRL-W CTRL-] to open a fold, for example.
		ਕੁੰਜੀ_Stuffed = TRUE;
	    }
	    ਜੇ (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	// no abbreviations now
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    ਲਈ (;;)
	    {
		ਲੰਮਾ	wait_ਸਮਾਂ;
		ਪੂਰਨ	keylen = 0;
		ਪੂਰਨ	showcmd_idx;
		check_ਸਿਰਾ_reg_executing(advance);
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		ਜੇ (typebuf.tb_maplen)
		    ਸਤਰ_breakcheck();
		ਨਹੀਂਤਾਂ
		    ui_breakcheck();		// check for CTRL-C
		ਜੇ (got_int)
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
		    ਜੇ ((c || typebuf.tb_maplen)
				     && (State & (ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE)))
			c = ESC;
		    ਨਹੀਂਤਾਂ
			c = Ctrl_C;
		    flush_buffers(FLUSH_ਨਿਵੇਸ਼);	// flush all typeahead

		    ਜੇ (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_ਨਤੀਜਾ_T ਨਤੀਜਾ = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    ਜੇ (ਨਤੀਜਾ == map_ਨਤੀਜਾ_retry)
			// try mapping again
			ਜਾਰੀ;
		    ਜੇ (ਨਤੀਜਾ == map_ਨਤੀਜਾ_fail)
		    {
			// failed, use the outer loop
			c = -1;
			ਰੋਕੋ;
		    }
		    ਜੇ (ਨਤੀਜਾ == map_ਨਤੀਜਾ_get)
		    {
/*
 * get a character: 2. from the typeahead buffer
 */
			c = typebuf.tb_buf[typebuf.tb_off];
			ਜੇ (advance)	// remove chars from typebuf
			{
			    cmd_silent = (typebuf.tb_silent > 0);
			    ਜੇ (typebuf.tb_maplen > 0)
				ਕੁੰਜੀ_Typed = FALSE;
			    ਨਹੀਂਤਾਂ
			    {
				ਕੁੰਜੀ_Typed = TRUE;
				// write char to script file(s)
				gotchars(typebuf.tb_buf
						 + typebuf.tb_off, 1);
			    }
			    ਕੁੰਜੀ_Noremap = typebuf.tb_noremap[typebuf.tb_off]
							    & ~RM_SIMPLIFIED;
			    del_typebuf(1, 0);
			}
			ਰੋਕੋ;  // got character, break the for loop
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
		ਜੇ (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& kitty_protocol_state != KKPS_ENABLED
			&& ex_ਸਾਧਾਰਨ_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & ਬਹੁਲਕ_ਪਾਓ)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_ਕੁੰਜੀ && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	ਖਾਨਾ = 0;
		    char_u	*ptr;

		    ਜੇ (ਬਹੁਲਕ_displayed)
		    {
			unshowmode(TRUE);
			ਬਹੁਲਕ_deleted = TRUE;
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    // may show a different cursor shape
		    ਜੇ (gui.ਵਿੱਚ_use && State != ਬਹੁਲਕ_ਸਾਧਾਰਨ && !cmd_silent)
		    {
			ਪੂਰਨ	    save_State;

			save_State = State;
			State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
			gui_update_cursor(TRUE, FALSE);
			State = save_State;
			shape_ਬਦਲਿਆ = TRUE;
		    }
#ਜੇ_ਅੰਤ
		    validate_cursor();
		    old_wcol = curwin->w_wcol;
		    old_wrow = curwin->w_wrow;

		    // move cursor left, if possible
		    ਜੇ (curwin->w_cursor.ਖਾਨਾ != 0)
		    {
			ਜੇ (curwin->w_wcol > 0)
			{
			    // After auto-indenting and no text is following,
			    // we are expecting to truncate the trailing
			    // white-space, so find the last non-white
			    // character -- webb
			    ਜੇ (did_ai && *skipwhite(ml_get_curline()
						+ curwin->w_cursor.ਖਾਨਾ) == NUL)
			    {
				chartabsize_T cts;

				curwin->w_wcol = 0;
				ptr = ml_get_curline();
				init_chartabsize_arg(&cts, curwin,
					  curwin->w_cursor.lnum, 0, ptr, ptr);
				ਜਦੋਂ (cts.cts_ptr < ptr + curwin->w_cursor.ਖਾਨਾ)
				{
				    ਜੇ (!VIM_ISWHITE(*cts.cts_ptr))
					curwin->w_wcol = cts.cts_vcol;
				    cts.cts_vcol += lbr_chartabsize(&cts);
				    ਜੇ (has_mbyte)
					cts.cts_ptr +=
						   (*mb_ptr2len)(cts.cts_ptr);
				    ਨਹੀਂਤਾਂ
					++cts.cts_ptr;
				}
				clear_chartabsize_arg(&cts);

				curwin->w_wrow = curwin->w_cline_ਪੰਕਤੀ
					   + curwin->w_wcol / curwin->w_ਚੌੜਾਈ;
				curwin->w_wcol %= curwin->w_ਚੌੜਾਈ;
				curwin->w_wcol += curwin_ਖਾਨਾ_off();
				ਖਾਨਾ = 0;	// no correction needed
			    }
			    ਨਹੀਂਤਾਂ
			    {
				--curwin->w_wcol;
				ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ - 1;
			    }
			}
			ਨਹੀਂਤਾਂ ਜੇ (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_ਚੌੜਾਈ - 1;
			    ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ - 1;
			}
			ਜੇ (has_mbyte && ਖਾਨਾ > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    ਖਾਨਾ -= (*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ);
			    ਜੇ ((*mb_ptr2cells)(ptr + ਖਾਨਾ) > 1)
				--curwin->w_wcol;
			}
		    }
		    setcursor();
		    ਬਾਹਰਲਾ_flush();
		    new_wcol = curwin->w_wcol;
		    new_wrow = curwin->w_wrow;
		    curwin->w_wcol = old_wcol;
		    curwin->w_wrow = old_wrow;
		}
		ਜੇ (c < 0)
		    ਜਾਰੀ;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		ਲਈ (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		ਜੇ (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    ਜਾਰੀ;
		}

		ਜੇ (ex_ਸਾਧਾਰਨ_busy > 0)
		{
		    ਸਥਿਰ ਪੂਰਨ tc = 0;

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    ਜੇ (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			ਜਾਰੀ;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    ਜੇ (p_im && (State & ਬਹੁਲਕ_ਪਾਓ))
			c = Ctrl_L;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		    ਨਹੀਂਤਾਂ ਜੇ (terminal_is_ਸਰਗਰਮ())
			c = K_CANCEL;
#ਜੇ_ਅੰਤ
		    ਨਹੀਂਤਾਂ ਜੇ ((State & ਬਹੁਲਕ_CMDLINE)
					     || (cmdwin_type > 0 && tc == ESC))
			c = Ctrl_C;
		    ਨਹੀਂਤਾਂ
			c = ESC;
		    tc = c;
		    // set a flag to indicate this wasn't a normal char
		    ਜੇ (advance)
			typebuf_was_ਖਾਲੀ_ਥਾਵਾਂ = TRUE;

		    // return from main_loop()
		    ਜੇ (pending_exmode_ਸਰਗਰਮ)
			exmode_ਸਰਗਰਮ = EXMODE_ਸਾਧਾਰਨ;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    ਰੋਕੋ;
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
		ਜੇ (((State & ਬਹੁਲਕ_ਪਾਓ) != 0 || p_lz)
			&& (State & ਬਹੁਲਕ_CMDLINE) == 0
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
		ਪੂਰਨ showing_partial = FALSE;
		ਜੇ (typebuf.tb_len > 0 && advance && !exmode_ਸਰਗਰਮ)
		{
		    ਜੇ (((State & (ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_ਪਾਓ))
						      || State == ਬਹੁਲਕ_LANGMAP)
			    && State != ਬਹੁਲਕ_HITRETURN)
		    {
			// this looks nice when typing a dead character map
			ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ
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
			ਧੱਕੋ_showcmd();
			ਜੇ (typebuf.tb_len > SHOWCMD_ਖਾਨੇ_ਗਿਣਤੀ)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_ਖਾਨੇ_ਗਿਣਤੀ;
			ਜਦੋਂ (showcmd_idx < typebuf.tb_len)
			    ਜੋੜੋ_byte_ਮੰਜ਼ਿਲ_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
		    }

		    // This looks nice when typing a dead character map.
		    // There is no actual command line for get_number().
		    ਜੇ ((State & ਬਹੁਲਕ_CMDLINE)
			    && get_cmdline_info()->cmdbuff != ਨਿਰਾ
#ਜੇ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
			    && cmdline_ਤਾਰਾ == 0
#ਜੇ_ਅੰਤ
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
		ਜੇ (typebuf.tb_len == 0)
		    // timedout may have been set if a mapping with empty RHS
		    // fully matched while longer mappings timed out.
		    timedout = FALSE;

		ਜੇ (advance)
		{
		    ਜੇ (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_ਕੁੰਜੀ)))
			// blocking wait
			wait_ਸਮਾਂ = -1L;
		    ਨਹੀਂਤਾਂ ਜੇ (keylen == KEYLEN_PART_ਕੁੰਜੀ && p_ttm >= 0)
			wait_ਸਮਾਂ = p_ttm;
		    ਨਹੀਂਤਾਂ
			wait_ਸਮਾਂ = p_tm;
		}
		ਨਹੀਂਤਾਂ
		    wait_ਸਮਾਂ = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_ਸਮਾਂ);

		ਜੇ (showcmd_idx != 0)
		    ਕੱਢੋ_showcmd();
		ਜੇ (showing_partial)
		{
		    ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
			edit_unputchar();
		    ਜੇ ((State & ਬਹੁਲਕ_CMDLINE)
					&& get_cmdline_info()->cmdbuff != ਨਿਰਾ)
			unputcmdline();
		    ਨਹੀਂਤਾਂ
			setcursor();	// put cursor back where it belongs
		}

		ਜੇ (c < 0)
		    ਜਾਰੀ;		// end of input script reached
		ਜੇ (c == NUL)		// no character available
		{
		    ਜੇ (!advance)
			ਰੋਕੋ;
		    ਜੇ (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			ਜਾਰੀ;
		    }
		}
		ਨਹੀਂਤਾਂ
		{	    // allow mapping for just typed characters
		    ਜਦੋਂ (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_ਸਰਗਰਮ = im_get_ਸਥਿਤੀ();
#ਜੇ_ਅੰਤ
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } ਜਦੋਂ ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    ਜੇ (advance && p_smd && ਸੁਨੇਹਾ_silent == 0 && (State & ਬਹੁਲਕ_ਪਾਓ))
    {
	ਜੇ (c == ESC && !ਬਹੁਲਕ_deleted && !no_mapping && ਬਹੁਲਕ_displayed)
	{
	    ਜੇ (typebuf.tb_len && !ਕੁੰਜੀ_Typed)
		redraw_cmdline = TRUE;	    // delete mode later
	    ਨਹੀਂਤਾਂ
		unshowmode(FALSE);
	}
	ਨਹੀਂਤਾਂ ਜੇ (c != ESC && ਬਹੁਲਕ_deleted)
	{
	    ਜੇ (typebuf.tb_len && !ਕੁੰਜੀ_Typed)
		redraw_cmdline = TRUE;	    // show mode later
	    ਨਹੀਂਤਾਂ
		showmode();
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // may unshow different cursor shape
    ਜੇ (gui.ਵਿੱਚ_use && shape_ਬਦਲਿਆ)
	gui_update_cursor(TRUE, FALSE);
#ਜੇ_ਅੰਤ
    ਜੇ (timedout && c == ESC)
    {
	// When recording there will be no timeout.  Add an <Ignore> after the
	// ESC to avoid that it forms a key code with following characters.
	gotchars_ignore();
    }

    --vgetc_busy;

    ਵਾਪਸ c;
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
    ਸਥਿਰ ਪੂਰਨ
inchar(
    char_u	*buf,
    ਪੂਰਨ		maxlen,
    ਲੰਮਾ	wait_ਸਮਾਂ)	    // milliseconds
{
    ਪੂਰਨ		len = 0;	    // init for GCC
    ਪੂਰਨ		retesc = FALSE;	    // return ESC with gotint
    ਪੂਰਨ		script_char;
    ਪੂਰਨ		tb_change_cnt = typebuf.tb_change_cnt;

    ਜੇ (wait_ਸਮਾਂ == -1L || wait_ਸਮਾਂ > 100L)  // flush output before waiting
    {
	cursor_on();
	ਬਾਹਰਲਾ_flush_cursor(FALSE, FALSE);
#ਜੇ defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	ਜੇ (gui.ਵਿੱਚ_use && postponed_mouseshape)
	    update_mouseshape(-1);
#ਜੇ_ਅੰਤ
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    ਜੇ (State != ਬਹੁਲਕ_HITRETURN)
    {
	did_outofmem_ਸੁਨੇਹਾ = FALSE;   // display out of memory message (again)
	did_swapwrite_ਸੁਨੇਹਾ = FALSE;  // display swap file write error again
    }
    undo_off = FALSE;		    // restart undo now

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    ਜਦੋਂ (scriptin[curscript] != ਨਿਰਾ && script_char < 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && !ignore_script
#ਜੇ_ਅੰਤ
	    )
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
	parse_queued_messages();
#ਜੇ_ਅੰਤ

	ਜੇ (got_int || (script_char = getc(scriptin[curscript])) < 0)
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
	    ਜੇ (got_int)
		retesc = TRUE;
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    ਜੇ (script_char < 0)	// did not get a character from script
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	ਜੇ (got_int)
	{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUM_LEN (MAXMAPLEN * 3 + 3)
	    char_u	dum[DUM_LEN + 1];

	    ਲਈ (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		ਜੇ (len == 0 || (len == 1 && dum[0] == Ctrl_C))
		    ਰੋਕੋ;
	    }
	    ਵਾਪਸ retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	ਜੇ (wait_ਸਮਾਂ == -1L || wait_ਸਮਾਂ > 10L)
	    ਬਾਹਰਲਾ_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_ਸਮਾਂ, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    ਜੇ (typebuf_ਬਦਲਿਆ(tb_change_cnt))
	ਵਾਪਸ 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    ਜੇ (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    ਵਾਪਸ fix_ਨਿਵੇਸ਼_ਬਫਰ(buf, len);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * "buf[]" must have room to triple the number of bytes!
 * Returns the new length.
 */
    ਪੂਰਨ
fix_ਨਿਵੇਸ਼_ਬਫਰ(char_u *buf, ਪੂਰਨ len)
{
    ਪੂਰਨ		i;
    char_u	*p = buf;

    /*
     * Two characters are special: NUL and K_SPECIAL.
     * When compiled With the GUI CSI is also special.
     * Replace	     NUL by K_SPECIAL KS_ZERO	 KE_FILLER
     * Replace K_SPECIAL by K_SPECIAL KS_SPECIAL KE_FILLER
     * Replace       CSI by K_SPECIAL KS_EXTRA   KE_CSI
     */
    ਲਈ (i = len; --i >= 0; ++p)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	// When the GUI is used any character can come after a CSI, don't
	// escape it.
	ਜੇ (gui.ਵਿੱਚ_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
# ifndef MSWIN
	// When not on MS-Windows and the GUI is not used CSI needs to be
	// escaped.
	ਨਹੀਂਤਾਂ ਜੇ (!gui.ਵਿੱਚ_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (ਪੂਰਨ)KE_CSI;
	    len += 2;
	}
# endif
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (p[0] == NUL || (p[0] == K_SPECIAL
		    // timeout may generate K_CURSORHOLD,
		    // 'autocompletedelay' timeout K_COMPLETE_DELAY
		    && (i < 2 || p[1] != KS_EXTRA
			|| (p[2] != (ਪੂਰਨ)KE_CURSORHOLD
			    && p[2] != (ਪੂਰਨ)KE_COMPLETE_DELAY))
#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
		    // Win32 console passes modifiers
		    && (
# ifdef VIMDLL
			gui.ਵਿੱਚ_use ||
# endif
			(i < 2 || p[1] != KS_MODIFIER))
#ਜੇ_ਅੰਤ
		    ))
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    p[2] = K_ਤੀਜਾ(p[0]);
	    p[1] = K_ਦੂਜਾ(p[0]);
	    p[0] = K_SPECIAL;
	    p += 2;
	    len += 2;
	}
    }
    *p = NUL;		// add trailing NUL
    ਵਾਪਸ len;
}

#ਜੇ defined(USE_ਨਿਵੇਸ਼_BUF)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    ਪੂਰਨ
ਨਿਵੇਸ਼_ਉਪਲਬਧ(ਖਾਲੀ)
{
    ਵਾਪਸ (!vim_is_ਨਿਵੇਸ਼_buf_ਖਾਲੀ_ਥਾਵਾਂ()
# ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#ਜੇ_ਅੰਤ

/*
 * Function passed to do_cmdline() to get the command after a <Cmd> key from
 * typeahead.
 */
    ਸਥਿਰ char_u *
getcmdkeycmd(
	ਪੂਰਨ		promptc ਅਣਵਰਤਿਆ,
	ਖਾਲੀ		*cookie ਅਣਵਰਤਿਆ,
	ਪੂਰਨ		indent ਅਣਵਰਤਿਆ,
	getline_opt_T	do_concat ਅਣਵਰਤਿਆ)
{
    garray_T	ਸਤਰ_ga;
    ਪੂਰਨ		c1 = -1;
    ਪੂਰਨ		c2;
    ਪੂਰਨ		cmod = 0;
    ਪੂਰਨ		aborted = FALSE;

    ga_init2(&ਸਤਰ_ga, 1, 32);

    // no mapping for these characters
    no_mapping++;

    got_int = FALSE;
    ਜਦੋਂ (c1 != NUL && !aborted)
    {
	ਜੇ (ga_grow(&ਸਤਰ_ga, 32) == FAIL)
	{
	    aborted = TRUE;
	    ਰੋਕੋ;
	}

	ਜੇ (vgetorpeek(FALSE) == NUL)
	{
	    // incomplete <Cmd> is an error, because there is not much the user
	    // could do in this state.
	    emsg(_(e_cmd_mapping_must_ਸਿਰਾ_with_cr));
	    aborted = TRUE;
	    ਰੋਕੋ;
	}

	// Get one character at a time.
	c1 = vgetorpeek(TRUE);

	// Get two extra bytes for special keys
	ਜੇ (c1 == K_SPECIAL)
	{
	    c1 = vgetorpeek(TRUE);
	    c2 = vgetorpeek(TRUE);
	    ਜੇ (c1 == KS_MODIFIER)
	    {
		cmod = c2;
		ਜਾਰੀ;
	    }
	    c1 = ਮੰਜ਼ਿਲ_SPECIAL(c1, c2);

	    // K_ESC is used to avoid ambiguity with the single Esc character
	    // that might be the start of an escape sequence.  Convert it back
	    // to a single Esc here.
	    ਜੇ (c1 == K_ESC)
		c1 = ESC;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    // Translate K_CSI to CSI.  The special key is only used to
	    // avoid it being recognized as the start of a special key.
	    ਜੇ (c1 == K_CSI)
		c1 = CSI;
#ਜੇ_ਅੰਤ
	}

	ਜੇ (got_int)
	    aborted = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (c1 == '\r' || c1 == '\n')
	    c1 = NUL;  // end the line
	ਨਹੀਂਤਾਂ ਜੇ (c1 == ESC)
	    aborted = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (c1 == K_COMMAND || c1 == K_SCRIPT_COMMAND)
	{
	    // give a nicer error message for this special case
	    emsg(_(e_cmd_mapping_must_ਸਿਰਾ_with_cr_before_ਦੂਜਾ_cmd));
	    aborted = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c1 == K_SNR)
	{
	    GA_CONCAT_LITERAL(&ਸਤਰ_ga, "<SNR>");
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (cmod != 0)
	    {
		ga_append(&ਸਤਰ_ga, K_SPECIAL);
		ga_append(&ਸਤਰ_ga, KS_MODIFIER);
		ga_append(&ਸਤਰ_ga, cmod);
	    }
	    ਜੇ (IS_SPECIAL(c1))
	    {
		ga_append(&ਸਤਰ_ga, K_SPECIAL);
		ga_append(&ਸਤਰ_ga, K_ਦੂਜਾ(c1));
		ga_append(&ਸਤਰ_ga, K_ਤੀਜਾ(c1));
	    }
	    ਨਹੀਂਤਾਂ
		ga_append(&ਸਤਰ_ga, c1);
	}

	cmod = 0;
    }

    no_mapping--;

    ਜੇ (aborted)
	ga_clear(&ਸਤਰ_ga);

    ਵਾਪਸ (char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * If there was a mapping we get its SID.  Otherwise, use "last_used_sid", it
 * is set when redo'ing.
 * Put this SID in the redo buffer, so that "." will use the same script
 * context.
 */
    ਖਾਲੀ
may_ਜੋੜੋ_ਆਖਰੀ_ਵਰਤਿਆ_map_ਮੰਜ਼ਿਲ_redobuff(ਖਾਲੀ)
{
    char_u  buf[3 + 20];
    ਪੂਰਨ	    buflen;
    ਪੂਰਨ	    sid = -1;

    ਜੇ (ਆਖਰੀ_ਵਰਤਿਆ_map != ਨਿਰਾ)
	sid = ਆਖਰੀ_ਵਰਤਿਆ_map->m_script_ctx.sc_sid;
    ਜੇ (sid < 0)
	sid = ਆਖਰੀ_ਵਰਤਿਆ_sid;

    ਜੇ (sid < 0)
	ਵਾਪਸ;

    // <K_SID>{nr};
    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = KE_SID;
    buflen = 3;

    buflen += vim_snprintf((ਅੱਖਰ *)buf + 3, 20, "%d;", sid);
    ਜੋੜੋ_buff(&redobuff, buf, (ਲੰਮਾ)buflen);
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
do_cmdkey_command(ਪੂਰਨ ਕੁੰਜੀ ਅਣਵਰਤਿਆ, ਪੂਰਨ flags)
{
    ਪੂਰਨ	    res;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    ਜੇ (ਕੁੰਜੀ == K_SCRIPT_COMMAND
		  && (ਆਖਰੀ_ਵਰਤਿਆ_map != ਨਿਰਾ || SCRIPT_ID_VALID(ਆਖਰੀ_ਵਰਤਿਆ_sid)))
    {
	save_current_sctx = current_sctx;
	ਜੇ (ਆਖਰੀ_ਵਰਤਿਆ_map != ਨਿਰਾ)
	    current_sctx = ਆਖਰੀ_ਵਰਤਿਆ_map->m_script_ctx;
	ਨਹੀਂਤਾਂ
	{
	    current_sctx.sc_sid = ਆਖਰੀ_ਵਰਤਿਆ_sid;
	    current_sctx.sc_lnum = 0;
	    current_sctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਵਸਤੂ(ਆਖਰੀ_ਵਰਤਿਆ_sid)->sn_ਸੰਸਕਰਣ;
	}
    }
#ਜੇ_ਅੰਤ

    res = do_cmdline(ਨਿਰਾ, getcmdkeycmd, ਨਿਰਾ, flags);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ res;
}

#ਜੇ defined(FEAT_EVAL)
    ਖਾਲੀ
reset_ਆਖਰੀ_ਵਰਤਿਆ_map(mapblock_T *mp)
{
    ਜੇ (ਆਖਰੀ_ਵਰਤਿਆ_map != mp)
	ਵਾਪਸ;

    ਆਖਰੀ_ਵਰਤਿਆ_map = ਨਿਰਾ;
    ਆਖਰੀ_ਵਰਤਿਆ_sid = -1;
}
#ਜੇ_ਅੰਤ
