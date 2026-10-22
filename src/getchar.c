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

#అన్తర్భావ "vim.h"

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

#నిర్వచించు కనీస_కొలత 20			// minimal size for b_str

స్థిర buffheader_T redobuff = {{ఖాళీ, 0, {NUL}}, ఖాళీ, 0, 0, FALSE};
స్థిర buffheader_T old_redobuff = {{ఖాళీ, 0, {NUL}}, ఖాళీ, 0, 0, FALSE};
స్థిర buffheader_T recordbuff = {{ఖాళీ, 0, {NUL}}, ఖాళీ, 0, 0, FALSE};

స్థిర పూర్ణాంకం typeahead_char = 0;		// typeahead char that's not flushed

#నిర్వచితమైతే FEAT_EVAL
స్థిర char_u typedchars[MAXMAPLEN + 1] = { NUL };  // typed chars before map
స్థిర పూర్ణాంకం typedchars_pos = 0;
#అయితే_ముగింపు

/*
 * When block_redo is TRUE the redo buffer will not be changed.
 * Used by edit() to repeat insertions.
 */
స్థిర పూర్ణాంకం	ఖండం_redo = FALSE;

స్థిర పూర్ణాంకం	కీలకం_Noremap = 0;	    // remapping flags

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
#నిర్వచించు RM_YES		0	// tb_noremap: remap
#నిర్వచించు RM_NONE		1	// tb_noremap: don't remap
#నిర్వచించు RM_SCRIPT	2	// tb_noremap: remap local script mappings
#నిర్వచించు RM_ABBR		4	// tb_noremap: don't remap, do abbrev.
#నిర్వచించు RM_SIMPLIFIED	8	// tb_noremap: modifiers merged into the key

// typebuf.tb_buf has three parts: room in front (for result of mappings), the
// middle for typeahead and room for new characters (which needs to be 3 *
// MAXMAPLEN for the Amiga).
#నిర్వచితమైతే FEAT_సాధారణ
// Add extra space to handle large OSC responses in bigger chunks (improve
// performance)
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3) + 2048)
#లేకపోతే // Tiny version
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
#అయితే_ముగింపు
స్థిర char_u	typebuf_init[TYPELEN_INIT];	// initial typebuf.tb_buf
స్థిర char_u	noremapbuf_init[TYPELEN_INIT];	// initial typebuf.tb_noremap

స్థిర size_t	చివరి_recorded_len = 0;	// number of last recorded chars

#నిర్వచితమైతే FEAT_EVAL
mapblock_T	*చివరి_వాడిన_map = ఖాళీ;
పూర్ణాంకం		చివరి_వాడిన_sid = -1;
#అయితే_ముగింపు

స్థిర పూర్ణాంకం	read_readbuf(buffheader_T *buf, పూర్ణాంకం advance);
స్థిర శూన్య	init_typebuf(శూన్య);
స్థిర శూన్య	may_sync_undo(శూన్య);
స్థిర శూన్య	free_typebuf(శూన్య);
స్థిర శూన్య	closescript(శూన్య);
స్థిర శూన్య	updatescript(పూర్ణాంకం c);
స్థిర పూర్ణాంకం	vgetorpeek(పూర్ణాంకం);
స్థిర పూర్ణాంకం	inchar(char_u *buf, పూర్ణాంకం maxlen, దీర్ఘ wait_సమయం);
#నిర్వచితమైతే FEAT_EVAL
స్థిర పూర్ణాంకం	do_కీలకం_ఆదానం_pre(పూర్ణాంకం c);
#అయితే_ముగింపు

/*
 * Free and clear a buffer.
 */
    స్థిర శూన్య
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    కొరకు (p = buf->bh_మొదటి.b_తదుపరి; p != ఖాళీ; p = np)
    {
	np = p->b_తదుపరి;
	vim_free(p);
    }
    buf->bh_మొదటి.b_తదుపరి = ఖాళీ;
    buf->bh_curr = ఖాళీ;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    స్థిర char_u *
get_buffcont(
    buffheader_T	*బఫరు,
    పూర్ణాంకం			dozero,	    // count == zero is not an error
    size_t		*len)	    // the length of the returned buffer
{
    long_u	    లెక్క = 0;
    char_u	    *p = ఖాళీ;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;
    size_t	    i = 0;

    // compute the total length of the string
    కొరకు (bp = బఫరు->bh_మొదటి.b_తదుపరి; bp != ఖాళీ; bp = bp->b_తదుపరి)
	లెక్క += (long_u)bp->b_strlen;

    ఒకవేళ ((లెక్క > 0 || dozero) && (p = alloc(లెక్క + 1)) != ఖాళీ)
    {
	p2 = p;
	కొరకు (bp = బఫరు->bh_మొదటి.b_తదుపరి; bp != ఖాళీ; bp = bp->b_తదుపరి)
	    కొరకు (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
	i = (size_t)(p2 - p);
    }

    ఒకవేళ (len != ఖాళీ)
	*len = i;

    తిరిగి p;
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(శూన్య)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE, &len);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;

    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    ఒకవేళ (len >= చివరి_recorded_len)
    {
	len -= చివరి_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    ఒకవేళ (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    తిరిగి (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    string_T
get_inserted(శూన్య)
{
    size_t len = 0;
    char_u *str = get_buffcont(&redobuff, FALSE, &len);
    string_T ret = { str, len };
    తిరిగి ret;
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    స్థిర శూన్య
కూడు_buff(
    buffheader_T	*buf,
    char_u		*s,
    దీర్ఘ		slen)	// length of "s" or -1
{
    ఒకవేళ (slen < 0)
	slen = (దీర్ఘ)STRLEN(s);
    ఒకవేళ (slen == 0)				// don't add empty strings
	తిరిగి;

    ఒకవేళ (buf->bh_మొదటి.b_తదుపరి == ఖాళీ)	// first add to list
    {
	buf->bh_curr = &(buf->bh_మొదటి);
	buf->bh_create_newblock = TRUE;
    }
    లేకపోతే ఒకవేళ (buf->bh_curr == ఖాళీ)	// buffer has already been read
    {
	iemsg(e_కూడు_గమ్యం_అంతర్గత_బఫరు_that_was_already_read_from);
	తిరిగి;
    }
    లేకపోతే ఒకవేళ (buf->bh_సూచీ != 0)
    {
	mch_memmove(buf->bh_మొదటి.b_తదుపరి->b_str,
		    buf->bh_మొదటి.b_తదుపరి->b_str + buf->bh_సూచీ,
		    (buf->bh_మొదటి.b_తదుపరి->b_strlen - buf->bh_సూచీ) + 1);
	buf->bh_మొదటి.b_తదుపరి->b_strlen -= buf->bh_సూచీ;
	buf->bh_space += buf->bh_సూచీ;
    }
    buf->bh_సూచీ = 0;

    ఒకవేళ (!buf->bh_create_newblock && buf->bh_space >= (పూర్ణాంకం)slen)
    {
	vim_strncpy(buf->bh_curr->b_str + buf->bh_curr->b_strlen, s, (size_t)slen);
	buf->bh_curr->b_strlen += slen;
	buf->bh_space -= slen;
    }
    లేకపోతే
    {
	long_u	    len;
	buffblock_T *p;

	ఒకవేళ (slen < కనీస_కొలత)
	    len = కనీస_కొలత;
	లేకపోతే
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి; // no space, just forget it
	vim_strncpy(p->b_str, s, (size_t)slen);
	p->b_strlen = slen;
	buf->bh_space = (పూర్ణాంకం)(len - slen);
	buf->bh_create_newblock = FALSE;

	p->b_తదుపరి = buf->bh_curr->b_తదుపరి;
	buf->bh_curr->b_తదుపరి = p;
	buf->bh_curr = p;
    }
}

/*
 * Delete "slen" bytes from the end of "buf".
 * Only works when it was just added.
 */
    స్థిర శూన్య
delete_buff_తోక(buffheader_T *buf, పూర్ణాంకం slen)
{
    ఒకవేళ (buf->bh_curr == ఖాళీ)
	తిరిగి;  // nothing to delete
    ఒకవేళ (buf->bh_curr->b_strlen < (size_t)slen)
	తిరిగి;

    buf->bh_curr->b_str[buf->bh_curr->b_strlen - (size_t)slen] = NUL;
    buf->bh_curr->b_strlen -= slen;
    buf->bh_space += slen;
}

/*
 * Add number "n" to buffer "buf".
 */
    స్థిర శూన్య
కూడు_అంకె_buff(buffheader_T *buf, దీర్ఘ n)
{
    char_u	సంఖ్య[32];
    పూర్ణాంకం		numberlen;

    numberlen = vim_snprintf((అక్షరం *)సంఖ్య, పరిమాణం(సంఖ్య), "%ld", n);
    కూడు_buff(buf, సంఖ్య, (దీర్ఘ)numberlen);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    స్థిర శూన్య
కూడు_char_buff(buffheader_T *buf, పూర్ణాంకం c)
{
    char_u	బైట్లు[MB_MAXBYTES + 1];
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    char_u	తాత్కాలిక[4];
    దీర్ఘ	templen;

    ఒకవేళ (IS_SPECIAL(c))
	len = 1;
    లేకపోతే
	len = (*mb_char2bytes)(c, బైట్లు);
    కొరకు (i = 0; i < len; ++i)
    {
	ఒకవేళ (!IS_SPECIAL(c))
	    c = బైట్లు[i];

	ఒకవేళ (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    తాత్కాలిక[0] = K_SPECIAL;
	    తాత్కాలిక[1] = K_రెండవ(c);
	    తాత్కాలిక[2] = K_మూడవ(c);
	    తాత్కాలిక[3] = NUL;
	    templen = 3;
	}
#నిర్వచితమైతే FEAT_GUI
	లేకపోతే ఒకవేళ (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    తాత్కాలిక[0] = CSI;
	    తాత్కాలిక[1] = KS_EXTRA;
	    తాత్కాలిక[2] = (పూర్ణాంకం)KE_CSI;
	    తాత్కాలిక[3] = NUL;
	    templen = 3;
	}
#అయితే_ముగింపు
	లేకపోతే
	{
	    తాత్కాలిక[0] = c;
	    తాత్కాలిక[1] = NUL;
	    templen = 1;
	}
	కూడు_buff(buf, తాత్కాలిక, templen);
    }
}

// First read ahead buffer. Used for translated commands.
స్థిర buffheader_T readbuf1 = {{ఖాళీ, 0, {NUL}}, ఖాళీ, 0, 0, FALSE};

// Second read ahead buffer. Used for redo.
స్థిర buffheader_T readbuf2 = {{ఖాళీ, 0, {NUL}}, ఖాళీ, 0, 0, FALSE};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    స్థిర పూర్ణాంకం
read_readbuffers(పూర్ణాంకం advance)
{
    పూర్ణాంకం c;

    c = read_readbuf(&readbuf1, advance);
    ఒకవేళ (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    తిరిగి c;
}

    స్థిర పూర్ణాంకం
read_readbuf(buffheader_T *buf, పూర్ణాంకం advance)
{
    char_u	c;
    buffblock_T	*curr;

    ఒకవేళ (buf->bh_మొదటి.b_తదుపరి == ఖాళీ)  // buffer is empty
	తిరిగి NUL;

    curr = buf->bh_మొదటి.b_తదుపరి;
    c = curr->b_str[buf->bh_సూచీ];

    ఒకవేళ (advance)
    {
	ఒకవేళ (curr->b_str[++buf->bh_సూచీ] == NUL)
	{
	    buf->bh_మొదటి.b_తదుపరి = curr->b_తదుపరి;
	    vim_free(curr);
	    buf->bh_సూచీ = 0;
	}
    }
    తిరిగి c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    స్థిర శూన్య
ప్రారంభం_stuff(శూన్య)
{
    ఒకవేళ (readbuf1.bh_మొదటి.b_తదుపరి != ఖాళీ)
    {
	readbuf1.bh_curr = &(readbuf1.bh_మొదటి);
	readbuf1.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
    ఒకవేళ (readbuf2.bh_మొదటి.b_తదుపరి != ఖాళీ)
    {
	readbuf2.bh_curr = &(readbuf2.bh_మొదటి);
	readbuf2.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    పూర్ణాంకం
stuff_ఖాళీలు(శూన్య)
{
    తిరిగి (readbuf1.bh_మొదటి.b_తదుపరి == ఖాళీ
	 && readbuf2.bh_మొదటి.b_తదుపరి == ఖాళీ);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * readbuf2.
 */
    పూర్ణాంకం
readbuf1_ఖాళీలు(శూన్య)
{
    తిరిగి (readbuf1.bh_మొదటి.b_తదుపరి == ఖాళీ);
}
#అయితే_ముగింపు

/*
 * Set a typeahead character that won't be flushed.
 */
    శూన్య
typeahead_noflush(పూర్ణాంకం c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    శూన్య
flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    ప్రారంభం_stuff();
    అయితే (read_readbuffers(TRUE) != NUL)
	;

    ఒకవేళ (flush_typeahead == FLUSH_కనీస)
    {
	// remove mapped characters at the start only,
	// but only when enough space left in typebuf
	ఒకవేళ (typebuf.tb_off + typebuf.tb_maplen >= typebuf.tb_buflen)
	{
	    typebuf.tb_off = MAXMAPLEN;
	    typebuf.tb_len = 0;
	}
	లేకపోతే
	{
	    typebuf.tb_off += typebuf.tb_maplen;
	    typebuf.tb_len -= typebuf.tb_maplen;
	}
#ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	ఒకవేళ (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	// remove typeahead
	ఒకవేళ (flush_typeahead == FLUSH_ఆదానం)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    అయితే (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#అయితే_ముగింపు
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
    ఒకవేళ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * The previous contents of the redo buffer is kept in old_redobuffer.
 * This is used for the CTRL-O <.> command in insert mode.
 */
    శూన్య
ResetRedobuff(శూన్య)
{
    ఒకవేళ (ఖండం_redo)
	తిరిగి;

    free_buff(&old_redobuff);
    old_redobuff = redobuff;
    redobuff.bh_మొదటి.b_తదుపరి = ఖాళీ;
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    శూన్య
CancelRedo(శూన్య)
{
    ఒకవేళ (ఖండం_redo)
	తిరిగి;

    free_buff(&redobuff);
    redobuff = old_redobuff;
    old_redobuff.bh_మొదటి.b_తదుపరి = ఖాళీ;
    ప్రారంభం_stuff();
    అయితే (read_readbuffers(TRUE) != NUL)
	;
}

/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
    శూన్య
saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;
    size_t	slen;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_మొదటి.b_తదుపరి = ఖాళీ;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_మొదటి.b_తదుపరి = ఖాళీ;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE, &slen);
    ఒకవేళ (s == ఖాళీ)
	తిరిగి;

    కూడు_buff(&redobuff, s, (దీర్ఘ)slen);
    vim_free(s);
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    శూన్య
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
    శూన్య
Append_గమ్యం_Redobuff(char_u *s)
{
    ఒకవేళ (!ఖండం_redo)
	కూడు_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    శూన్య
Append_గమ్యం_Redobuff_Lit(
    char_u	*str,
    పూర్ణాంకం		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    పూర్ణాంకం		c;
    char_u	*ప్రారంభం;

    ఒకవేళ (ఖండం_redo)
	తిరిగి;

    అయితే (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	ప్రారంభం = s;
	అయితే (*s >= ' ' && *s < DEL && (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	ఒకవేళ (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	ఒకవేళ (s > ప్రారంభం)
	    కూడు_buff(&redobuff, ప్రారంభం, (దీర్ఘ)(s - ప్రారంభం));

	ఒకవేళ (*s == NUL || (len >= 0 && s - str >= len))
	    ఆపు;

	// Handle a special or multibyte character.
	ఒకవేళ (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	లేకపోతే
	    c = *s++;
	ఒకవేళ (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    కూడు_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048
	ఒకవేళ (*s == NUL && c == '0')
	    కూడు_buff(&redobuff, (char_u *)"048", 3L);
	లేకపోతే
	    కూడు_char_buff(&redobuff, c);
    }
}

/*
 * Append "s" to the redo buffer, leaving 3-byte special key codes unmodified
 * and escaping other K_SPECIAL and CSI bytes.
 */
    శూన్య
Append_గమ్యం_Redobuff_Spec(char_u *s)
{
    ఒకవేళ (ఖండం_redo)
	తిరిగి;

    అయితే (*s != NUL)
    {
	ఒకవేళ (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    కూడు_buff(&redobuff, s, 3L);
	    s += 3;
	}
	లేకపోతే
	    కూడు_char_buff(&redobuff, mb_cptr2char_adv(&s));
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    శూన్య
Append_Char_గమ్యం_Redobuff(పూర్ణాంకం c)
{
    ఒకవేళ (!ఖండం_redo)
	కూడు_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    శూన్య
Append_సంఖ్య_గమ్యం_Redobuff(దీర్ఘ n)
{
    ఒకవేళ (!ఖండం_redo)
	కూడు_అంకె_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    శూన్య
stuffReadbuff(char_u *s)
{
    కూడు_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    శూన్య
stuffRedoReadbuff(char_u *s)
{
    కూడు_buff(&readbuf2, s, -1L);
}

    శూన్య
stuffReadbuffLen(char_u *s, దీర్ఘ len)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (కూడు_చివరి_చొప్పించు == 1) // Only add if this is the first call, for
			      // recursive calls, ignore.
	ga_concat_len(&చివరి_చొప్పించు_ga, s, (size_t)len);
#అయితే_ముగింపు
    కూడు_buff(&readbuf1, s, len);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    శూన్య
stuffReadbuffSpec(char_u *s)
{
    పూర్ణాంకం c;

    అయితే (*s != NUL)
    {
	ఒకవేళ (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	లేకపోతే
	{
	    c = mb_cptr2char_adv(&s);
	    ఒకవేళ (c == CAR || c == NL || c == ESC)
		c = ' ';
	    stuffcharReadbuff(c);
	}
    }
}
#అయితే_ముగింపు

/*
 * Append a character to the stuff buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    శూన్య
stuffcharReadbuff(పూర్ణాంకం c)
{
    కూడు_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    శూన్య
stuffnumReadbuff(దీర్ఘ n)
{
    కూడు_అంకె_buff(&readbuf1, n);
}

/*
 * Stuff a string into the typeahead buffer, such that edit() will insert it
 * literally ("literally" TRUE) or interpret is as typed characters.
 */
    శూన్య
stuffescaped(char_u *arg, పూర్ణాంకం literally)
{
    పూర్ణాంకం		c;
    char_u	*ప్రారంభం;

    అయితే (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	ప్రారంభం = arg;
	అయితే ((*arg >= ' ' && *arg < DEL)
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	ఒకవేళ (arg > ప్రారంభం)
	    stuffReadbuffLen(ప్రారంభం, (దీర్ఘ)(arg - ప్రారంభం));

	// stuff a single special character
	ఒకవేళ (*arg != NUL)
	{
	    ఒకవేళ (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    లేకపోతే
		c = *arg++;
	    ఒకవేళ (literally && ((c < ' ' && c != TAB) || c == DEL))
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
    స్థిర పూర్ణాంకం
read_redo(పూర్ణాంకం init, పూర్ణాంకం old_redo)
{
    స్థిర buffblock_T	*bp;
    స్థిర char_u	*p;
    పూర్ణాంకం			c;
    పూర్ణాంకం			n;
    char_u		buf[MB_MAXBYTES + 1];
    పూర్ణాంకం			i;

    ఒకవేళ (init)
    {
	ఒకవేళ (old_redo)
	    bp = old_redobuff.bh_మొదటి.b_తదుపరి;
	లేకపోతే
	    bp = redobuff.bh_మొదటి.b_తదుపరి;
	ఒకవేళ (bp == ఖాళీ)
	    తిరిగి FAIL;
	p = bp->b_str;
	తిరిగి OK;
    }
    ఒకవేళ ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	ఒకవేళ (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	లేకపోతే
	    n = 1;
	కొరకు (i = 0; ; ++i)
	{
	    ఒకవేళ (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = గమ్యం_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#నిర్వచితమైతే FEAT_GUI
	    ఒకవేళ (c == CSI)	// escaped CSI
		p += 2;
#అయితే_ముగింపు
	    ఒకవేళ (*++p == NUL && bp->b_తదుపరి != ఖాళీ)
	    {
		bp = bp->b_తదుపరి;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    ఒకవేళ (i == n - 1)	// last byte of a character
	    {
		ఒకవేళ (n != 1)
		    c = (*mb_ptr2char)(buf);
		ఆపు;
	    }
	    c = *p;
	    ఒకవేళ (c == NUL)	// cannot happen?
		ఆపు;
	}
    }

    తిరిగి c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    స్థిర శూన్య
copy_redo(పూర్ణాంకం old_redo)
{
    పూర్ణాంకం	    c;

    అయితే ((c = read_redo(FALSE, old_redo)) != NUL)
	కూడు_char_buff(&readbuf2, c);
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
    పూర్ణాంకం
ప్రారంభం_redo(దీర్ఘ లెక్క, పూర్ణాంకం old_redo)
{
    పూర్ణాంకం	    c;

    // init the pointers; return if nothing to redo
    ఒకవేళ (read_redo(TRUE, old_redo) == FAIL)
	తిరిగి FAIL;

    c = read_redo(FALSE, old_redo);

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (c == K_SID)
    {
	// Copy the <SID>{sid}; sequence
	కూడు_char_buff(&readbuf2, c);
	కొరకు (;;)
	{
	    c = read_redo(FALSE, old_redo);
	    కూడు_char_buff(&readbuf2, c);
	    ఒకవేళ (!సురక్షిత_isdigit(c))
		ఆపు;
	}
	c = read_redo(FALSE, old_redo);
    }
#అయితే_ముగింపు

    // copy the buffer name, if present
    ఒకవేళ (c == '"')
    {
	కూడు_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	// if a numbered buffer is used, increment the number
	ఒకవేళ (c >= '1' && c < '9')
	    ++c;
	కూడు_char_buff(&readbuf2, c);

	// the expression register should be re-evaluated
	ఒకవేళ (c == '=')
	{
	    కూడు_char_buff(&readbuf2, CAR);
	    cmd_silent = TRUE;
	}

	c = read_redo(FALSE, old_redo);
    }

    ఒకవేళ (c == 'v')   // redo Visual
    {
	VIsual = curwin->w_cursor;
	VIsual_చురుకైన = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    // try to enter the count (in place of a previous count)
    ఒకవేళ (లెక్క)
    {
	అయితే (VIM_ISDIGIT(c))	// skip "old" count
	    c = read_redo(FALSE, old_redo);
	కూడు_అంకె_buff(&readbuf2, లెక్క);
    }

    // copy the rest from the redo buffer into the stuff buffer
    కూడు_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    తిరిగి OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
ప్రారంభం_redo_ins(శూన్య)
{
    పూర్ణాంకం	    c;

    ఒకవేళ (read_redo(TRUE, FALSE) == FAIL)
	తిరిగి FAIL;
    ప్రారంభం_stuff();

    // skip the count and the command character
    అయితే ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	ఒకవేళ (vim_strchr((char_u *)"AaIiRrOo", c) != ఖాళీ)
	{
	    ఒకవేళ (c == 'O' || c == 'o')
		కూడు_buff(&readbuf2, NL_STR, -1L);
	    ఆపు;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    ఖండం_redo = TRUE;
    తిరిగి OK;
}

    శూన్య
stop_redo_ins(శూన్య)
{
    ఖండం_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    స్థిర శూన్య
init_typebuf(శూన్య)
{
    ఒకవేళ (typebuf.tb_buf != ఖాళీ)
	తిరిగి;

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
    పూర్ణాంకం
noremap_కీలకాలు(శూన్య)
{
    తిరిగి కీలకం_Noremap & (RM_NONE|RM_SCRIPT);
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
    పూర్ణాంకం
ins_typebuf(
    char_u	*str,
    పూర్ణాంకం		noremap,
    పూర్ణాంకం		స్థానభ్రంశం,
    పూర్ణాంకం		nottyped,
    పూర్ణాంకం		silent)
{
    char_u	*s1, *s2;
    పూర్ణాంకం		newlen;
    పూర్ణాంకం		addlen;
    పూర్ణాంకం		i;
    పూర్ణాంకం		newoff;
    పూర్ణాంకం		val;
    పూర్ణాంకం		nrm;

    init_typebuf();
    ఒకవేళ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
    state_no_longer_సురక్షిత("ins_typebuf()");

    addlen = (పూర్ణాంకం)STRLEN(str);

    ఒకవేళ (స్థానభ్రంశం == 0 && addlen <= typebuf.tb_off)
    {
	/*
	 * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
	 */
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    లేకపోతే ఒకవేళ (typebuf.tb_len == 0 && typebuf.tb_buflen
					       >= addlen + 3 * (MAXMAPLEN + 4))
    {
	/*
	 * Buffer is empty and string fits in the existing buffer.
	 * Leave some space before and after, if possible.
	 */
	typebuf.tb_off = (typebuf.tb_buflen - addlen - 3 * (MAXMAPLEN + 4)) / 2;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    లేకపోతే
    {
	పూర్ణాంకం extra;

	/*
	 * Need to allocate a new buffer.
	 * In typebuf.tb_buf there must always be room for 3 * (MAXMAPLEN + 4)
	 * characters.  We add some extra room to avoid having to allocate too
	 * often.
	 */
	newoff = MAXMAPLEN + 4;
	extra = addlen + newoff + 4 * (MAXMAPLEN + 4);
	ఒకవేళ (typebuf.tb_len > 2147483647 - extra)
	{
	    // string is getting too long for a 32 bit int
	    emsg(_(e_command_too_complex));    // also calls flush_buffers
	    setcursor();
	    తిరిగి FAIL;
	}
	newlen = typebuf.tb_len + extra;
	s1 = alloc(newlen);
	ఒకవేళ (s1 == ఖాళీ)		    // out of memory
	    తిరిగి FAIL;
	s2 = alloc(newlen);
	ఒకవేళ (s2 == ఖాళీ)		    // out of memory
	{
	    vim_free(s1);
	    తిరిగి FAIL;
	}
	typebuf.tb_buflen = newlen;

	// copy the old chars, before the insertion point
	mch_memmove(s1 + newoff, typebuf.tb_buf + typebuf.tb_off,
							      (size_t)స్థానభ్రంశం);
	// copy the new chars
	mch_memmove(s1 + newoff + స్థానభ్రంశం, str, (size_t)addlen);
	// copy the old chars, after the insertion point, including the	NUL at
	// the end
	mch_memmove(s1 + newoff + స్థానభ్రంశం + addlen,
				     typebuf.tb_buf + typebuf.tb_off + స్థానభ్రంశం,
				       (size_t)(typebuf.tb_len - స్థానభ్రంశం + 1));
	ఒకవేళ (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)స్థానభ్రంశం);
	mch_memmove(s2 + newoff + స్థానభ్రంశం + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + స్థానభ్రంశం,
					   (size_t)(typebuf.tb_len - స్థానభ్రంశం));
	ఒకవేళ (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    // If noremap == REMAP_SCRIPT: do remap script-local mappings.
    ఒకవేళ (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    లేకపోతే ఒకవేళ (noremap == REMAP_SKIP)
	val = RM_ABBR;
    లేకపోతే
	val = RM_NONE;

    /*
     * Adjust typebuf.tb_noremap[] for the new characters:
     * If noremap == REMAP_NONE or REMAP_SCRIPT: new characters are
     *			(sometimes) not remappable
     * If noremap == REMAP_YES: all the new characters are mappable
     * If noremap  > 0: "noremap" characters are not remappable, the rest
     *			mappable
     */
    ఒకవేళ (noremap == REMAP_SKIP)
	nrm = 1;
    లేకపోతే ఒకవేళ (noremap < 0)
	nrm = addlen;
    లేకపోతే
	nrm = noremap;
    కొరకు (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + స్థానభ్రంశం] =
						  (--nrm >= 0) ? val : RM_YES;

    // tb_maplen and tb_silent only remember the length of mapped and/or
    // silent mappings at the start of the buffer, assuming that a mapped
    // sequence doesn't result in typed characters.
    ఒకవేళ (nottyped || typebuf.tb_maplen > స్థానభ్రంశం)
	typebuf.tb_maplen += addlen;
    ఒకవేళ (silent || typebuf.tb_silent > స్థానభ్రంశం)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    ఒకవేళ (typebuf.tb_no_abbr_cnt && స్థానభ్రంశం == 0)	// and not used for abbrev.s
	typebuf.tb_no_abbr_cnt += addlen;

    తిరిగి OK;
}

/*
 * Put character "c" back into the typeahead buffer.
 * Can be used for a character obtained by vgetc() that needs to be put back.
 * Uses cmd_silent, KeyTyped and KeyNoremap to restore the flags belonging to
 * the char.
 * Returns the length of what was inserted.
 */
    పూర్ణాంకం
ins_char_typebuf(పూర్ణాంకం c, పూర్ణాంకం modifiers)
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    పూర్ణాంకం		len = special_గమ్యం_buf(c, modifiers, TRUE, buf);

    buf[len] = NUL;
    (శూన్య)ins_typebuf(buf, కీలకం_Noremap, 0, !కీలకం_Typed, cmd_silent);
    తిరిగి len;
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
    పూర్ణాంకం
typebuf_మారింది(
    పూర్ణాంకం		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    తిరిగి (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#అయితే_ముగింపు
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    పూర్ణాంకం
typebuf_typed(శూన్య)
{
    తిరిగి typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    పూర్ణాంకం
typebuf_maplen(శూన్య)
{
    తిరిగి typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    శూన్య
del_typebuf(పూర్ణాంకం len, పూర్ణాంకం స్థానభ్రంశం)
{
    పూర్ణాంకం	    i;

    ఒకవేళ (len == 0)
	తిరిగి;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    ఒకవేళ (స్థానభ్రంశం == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    లేకపోతే
    {
	i = typebuf.tb_off + స్థానభ్రంశం;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	ఒకవేళ (typebuf.tb_off > MAXMAPLEN)
	{
	    mch_memmove(typebuf.tb_buf + MAXMAPLEN,
			     typebuf.tb_buf + typebuf.tb_off, (size_t)స్థానభ్రంశం);
	    mch_memmove(typebuf.tb_noremap + MAXMAPLEN,
			 typebuf.tb_noremap + typebuf.tb_off, (size_t)స్థానభ్రంశం);
	    typebuf.tb_off = MAXMAPLEN;
	}
	// adjust typebuf.tb_buf (include the NUL at the end)
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + స్థానభ్రంశం,
						     typebuf.tb_buf + i + len,
				       (size_t)(typebuf.tb_len - స్థానభ్రంశం + 1));
	// adjust typebuf.tb_noremap[]
	mch_memmove(typebuf.tb_noremap + typebuf.tb_off + స్థానభ్రంశం,
						 typebuf.tb_noremap + i + len,
					   (size_t)(typebuf.tb_len - స్థానభ్రంశం));
    }

    ఒకవేళ (typebuf.tb_maplen > స్థానభ్రంశం)		// adjust tb_maplen
    {
	ఒకవేళ (typebuf.tb_maplen < స్థానభ్రంశం + len)
	    typebuf.tb_maplen = స్థానభ్రంశం;
	లేకపోతే
	    typebuf.tb_maplen -= len;
    }
    ఒకవేళ (typebuf.tb_silent > స్థానభ్రంశం)		// adjust tb_silent
    {
	ఒకవేళ (typebuf.tb_silent < స్థానభ్రంశం + len)
	    typebuf.tb_silent = స్థానభ్రంశం;
	లేకపోతే
	    typebuf.tb_silent -= len;
    }
    ఒకవేళ (typebuf.tb_no_abbr_cnt > స్థానభ్రంశం)	// adjust tb_no_abbr_cnt
    {
	ఒకవేళ (typebuf.tb_no_abbr_cnt < స్థానభ్రంశం + len)
	    typebuf.tb_no_abbr_cnt = స్థానభ్రంశం;
	లేకపోతే
	    typebuf.tb_no_abbr_cnt -= len;
    }

#ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#అయితే_ముగింపు
    ఒకవేళ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * State for adding bytes to a recording or 'showcmd'.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    పూర్ణాంకం		prev_c;
    size_t	buflen;
    గుర్తులేని	pending_special;
    గుర్తులేని	pending_mbyte;
} gotchars_state_T;

/*
 * Add a single byte to a recording or 'showcmd'.
 * Return TRUE if a full key has been received, FALSE otherwise.
 */
    స్థిర పూర్ణాంకం
gotchars_కూడు_byte(gotchars_state_T *state, char_u byte)
{
    పూర్ణాంకం		c = state->buf[state->buflen++] = byte;
    పూర్ణాంకం		retval = FALSE;
    పూర్ణాంకం		లోపల_special = state->pending_special > 0;
    పూర్ణాంకం		లోపల_mbyte = state->pending_mbyte > 0;

    ఒకవేళ (లోపల_special)
	state->pending_special--;
    లేకపోతే ఒకవేళ (c == K_SPECIAL
#నిర్వచితమైతే FEAT_GUI
		|| c == CSI
#అయితే_ముగింపు
	    )
	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	state->pending_special = 2;

    ఒకవేళ (state->pending_special > 0)
	వెళ్ళు ret_false;

    ఒకవేళ (లోపల_mbyte)
	state->pending_mbyte--;
    లేకపోతే
    {
	ఒకవేళ (లోపల_special)
	{
	    ఒకవేళ (state->prev_c == KS_MODIFIER)
		// When receiving a modifier, wait for the modified key.
		వెళ్ళు ret_false;
	    c = గమ్యం_SPECIAL(state->prev_c, c);
	    // Drop K_FOCUSGAINED, K_FOCUSLOST and K_COMPLETE_DELAY, they are
	    // not useful in a recording.
	    ఒకవేళ (c == K_FOCUSGAINED || c == K_FOCUSLOST || c == K_COMPLETE_DELAY)
		state->buflen = 0;
	}
	// When receiving a multibyte character, store it until we have all
	// the bytes, so that it won't be split between two buffer blocks,
	// and delete_buff_tail() will work properly.
	state->pending_mbyte = MB_BYTE2LEN_CHECK(c) - 1;
    }

    ఒకవేళ (state->pending_mbyte > 0)
	వెళ్ళు ret_false;

    retval = TRUE;
ret_false:
    state->prev_c = c;
    తిరిగి retval;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the record buffer.
 */
    స్థిర శూన్య
gotchars(char_u *chars, పూర్ణాంకం len)
{
    char_u		*s = chars;
    size_t		i;
    పూర్ణాంకం			todo = len;
    స్థిర gotchars_state_T state;

    అయితే (todo-- > 0)
    {
	ఒకవేళ (!gotchars_కూడు_byte(&state, *s++))
	    కొనసాగు;

	// Handle one byte at a time; no translation to be done.
	కొరకు (i = 0; i < state.buflen; ++i)
	    updatescript(state.buf[i]);

	ఒకవేళ (reg_recording != 0)
	{
	    state.buf[state.buflen] = NUL;
	    కూడు_buff(&recordbuff, state.buf, (దీర్ఘ)state.buflen);
	    // remember how many chars were last recorded
	    చివరి_recorded_len += state.buflen;
	}
	state.buflen = 0;
    }

    may_sync_undo();

#నిర్వచితమైతే FEAT_EVAL
    // output "debug mode" message next time in debug mode
    debug_did_సందేశం = FALSE;
#అయితే_ముగింపు

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}

/*
 * Record an <Ignore> key.
 */
    శూన్య
gotchars_ignore(శూన్య)
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
    శూన్య
ungetchars(పూర్ణాంకం len)
{
    ఒకవేళ (reg_recording == 0)
	తిరిగి;

    delete_buff_తోక(&recordbuff, len);
    చివరి_recorded_len -= len;
}

/*
 * Sync undo.  Called when typed characters are obtained from the typeahead
 * buffer, or when a menu is used.
 * Do not sync:
 * - In Insert mode, unless cursor key has been used.
 * - While reading a script file.
 * - When no_u_sync is non-zero.
 */
    స్థిర శూన్య
may_sync_undo(శూన్య)
{
    ఒకవేళ ((!(State & (బహుళకం_చొప్పించు | బహుళకం_CMDLINE)) || arrow_వాడిన)
					       && scriptin[curscript] == ఖాళీ)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    స్థిర పూర్ణాంకం
alloc_typebuf(శూన్య)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    ఒకవేళ (typebuf.tb_buf == ఖాళీ || typebuf.tb_noremap == ఖాళీ)
    {
	free_typebuf();
	తిరిగి FAIL;
    }
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_off = MAXMAPLEN + 4;  // can insert without realloc
    typebuf.tb_len = 0;
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    typebuf.tb_no_abbr_cnt = 0;
    ఒకవేళ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
#ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    typebuf_was_filled = FALSE;
#అయితే_ముగింపు
    తిరిగి OK;
}

/*
 * Free the buffers of "typebuf".
 */
    స్థిర శూన్య
free_typebuf(శూన్య)
{
    ఒకవేళ (typebuf.tb_buf == typebuf_init)
	అంతర్గత_error("Free typebuf 1");
    లేకపోతే
	VIM_CLEAR(typebuf.tb_buf);
    ఒకవేళ (typebuf.tb_noremap == noremapbuf_init)
	అంతర్గత_error("Free typebuf 2");
    లేకపోతే
	VIM_CLEAR(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
స్థిర typebuf_T saved_typebuf[NSCRIPT];

    పూర్ణాంకం
save_typebuf(శూన్య)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    ఒకవేళ (alloc_typebuf() == FAIL)
    {
	closescript();
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

స్థిర పూర్ణాంకం old_char = -1;	// character put back by vungetc()
స్థిర పూర్ణాంకం old_mod_ముసుగు;	// mod_mask for ungotten character
స్థిర పూర్ణాంకం old_mouse_అడ్డువరుస;	// mouse_row related to old_char
స్థిర పూర్ణాంకం old_mouse_నిలువు;	// mouse_col related to old_char
స్థిర పూర్ణాంకం old_కీలకం_Stuffed;	// whether old_char was stuffed

స్థిర పూర్ణాంకం can_get_old_char(శూన్య)
{
    // If the old character was not stuffed and characters have been added to
    // the stuff buffer, need to first get the stuffed characters instead.
    తిరిగి old_char != -1 && (old_కీలకం_Stuffed || stuff_ఖాళీలు());
}

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    శూన్య
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    ఒకవేళ (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_ముసుగు = old_mod_ముసుగు;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_మొదటి.b_తదుపరి = ఖాళీ;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_మొదటి.b_తదుపరి = ఖాళీ;
#నిర్వచితమైతే USE_ఆదానం_BUF
    tp->save_inputbuf = get_ఆదానం_buf();
#అయితే_ముగింపు
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    శూన్య
restore_typeahead(tasave_T *tp, పూర్ణాంకం overwrite వాడని)
{
    ఒకవేళ (tp->typebuf_valid)
    {
	free_typebuf();
	typebuf = tp->save_typebuf;
    }

    old_char = tp->old_char;
    old_mod_ముసుగు = tp->old_mod_ముసుగు;

    free_buff(&readbuf1);
    readbuf1 = tp->save_readbuf1;
    free_buff(&readbuf2);
    readbuf2 = tp->save_readbuf2;
#నిర్వచితమైతే USE_ఆదానం_BUF
    సమితి_ఆదానం_buf(tp->save_inputbuf, overwrite);
#అయితే_ముగింపు
}

/*
 * Open a new script file for the ":source!" command.
 */
    శూన్య
openscript(
    char_u	*name,
    పూర్ణాంకం		directly)	// when TRUE execute directly
{
    ఒకవేళ (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	తిరిగి;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    ఒకవేళ (check_secure())
	తిరిగి;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	తిరిగి;
#అయితే_ముగింపు

    ఒకవేళ (scriptin[curscript] != ఖాళీ)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    ఒకవేళ ((scriptin[curscript] = mch_fopen((అక్షరం *)NameBuff, READBIN)) == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str), name);
	ఒకవేళ (curscript)
	    --curscript;
	తిరిగి;
    }
    ఒకవేళ (save_typebuf() == FAIL)
	తిరిగి;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    ఒకవేళ (directly)
    {
	oparg_T	oa;
	పూర్ణాంకం	oldcurscript;
	పూర్ణాంకం	save_State = State;
	పూర్ణాంకం	save_restart_edit = restart_edit;
	పూర్ణాంకం	save_insertmode = p_im;
	పూర్ణాంకం	save_ముగింపు_op = ముగింపు_op;
	పూర్ణాంకం	save_సందేశం_scroll = సందేశం_scroll;

	State = బహుళకం_సాధారణ;
	సందేశం_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	ముగింపు_op = FALSE;

	oldcurscript = curscript;
	చేయి
	{
	    update_topline_cursor();	// update cursor position and topline
	    సాధారణ_cmd(&oa, FALSE);	// execute one command
	    (శూన్య)vpeekc();		// check for end of file
	}
	అయితే (scriptin[oldcurscript] != ఖాళీ);

	State = save_State;
	సందేశం_scroll = save_సందేశం_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	ముగింపు_op = save_ముగింపు_op;
    }
}

/*
 * Close the currently active input script.
 */
    స్థిర శూన్య
closescript(శూన్య)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = ఖాళీ;
    ఒకవేళ (curscript > 0)
	--curscript;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
close_all_scripts(శూన్య)
{
    అయితే (scriptin[0] != ఖాళీ)
	closescript();
}
#అయితే_ముగింపు

/*
 * Return TRUE when reading keys from a script file.
 */
    పూర్ణాంకం
using_script(శూన్య)
{
    తిరిగి scriptin[curscript] != ఖాళీ;
}

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    శూన్య
before_blocking(శూన్య)
{
    // Tell terminal to flush screen contents before blocking
    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_FLUSH);

    updatescript(0);
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (may_garbage_collect)
	garbage_collect(FALSE);
#అయితే_ముగింపు
}

/*
 * updatescript() is called when a character can be written into the script
 * file or when we have waited some time for a character (c == 0)
 *
 * All the changed memfiles are synced if c == 0 or when the number of typed
 * characters reaches 'updatecount' and 'updatecount' is non-zero.
 */
    స్థిర శూన్య
updatescript(పూర్ణాంకం c)
{
    స్థిర పూర్ణాంకం	    లెక్క = 0;

    ఒకవేళ (c && scriptout)
	putc(c, scriptout);
    ఒకవేళ (c == 0 || (p_uc > 0 && ++లెక్క >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	లెక్క = 0;
    }
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (typedchars_pos < MAXMAPLEN)
    {
	typedchars[typedchars_pos] = c;
	typedchars_pos++;
    }
#అయితే_ముగింపు
}

/*
 * Convert "c_arg" plus "modifiers" to merge the effect of modifyOtherKeys into
 * the character.  Also for when the Kitty key protocol is used.
 */
    పూర్ణాంకం
విలీనం_modify_Other_కీలకాలు(పూర్ణాంకం c_arg, పూర్ణాంకం *modifiers)
{
    పూర్ణాంకం c = c_arg;

    // CTRL only uses the lower 5 bits of the character.
    ఒకవేళ (*modifiers & MOD_ముసుగు_CTRL)
    {
	ఒకవేళ ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	{
	    c &= 0x1f;
	    ఒకవేళ (c == NUL)
		c = K_ZERO;
	}
	లేకపోతే ఒకవేళ (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#నిర్వచితమైతే FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	లేకపోతే ఒకవేళ (c == '2')
	    c = NUL;
	లేకపోతే ఒకవేళ (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	లేకపోతే ఒకవేళ (c == '8')
	    c = BS;
	లేకపోతే ఒకవేళ (c == '?')
	    c = DEL;
#అయితే_ముగింపు
	ఒకవేళ (c != c_arg)
	    *modifiers &= ~MOD_ముసుగు_CTRL;
    }

    // Alt/Meta sets the 8th bit of the character.
    ఒకవేళ ((*modifiers & (MOD_ముసుగు_META | MOD_ముసుగు_ALT))
	    && c >= 0 && c <= 127)
    {
	// Some terminals (esp. Kitty) do not include Shift in the character.
	// Apply it here to get consistency across terminals.  Only do ASCII
	// letters, for other characters it depends on the keyboard layout.
	ఒకవేళ ((*modifiers & MOD_ముసుగు_జరుపు) && c >= 'a' && c <= 'z')
	{
	    c += 'a' - 'A';
	    *modifiers &= ~MOD_ముసుగు_జరుపు;
	}
	c += 0x80;
	*modifiers &= ~(MOD_ముసుగు_META | MOD_ముసుగు_ALT);
    }

    తిరిగి c;
}

/*
 * Add a single byte to 'showcmd' for a partially matched mapping.
 * Call add_to_showcmd() if a full key has been received.
 */
    స్థిర శూన్య
కూడు_byte_గమ్యం_showcmd(char_u byte)
{
    స్థిర gotchars_state_T state;
    char_u	*ptr;
    పూర్ణాంకం		modifiers = 0;
    పూర్ణాంకం		c = NUL;

    ఒకవేళ (!p_sc || సందేశం_silent != 0)
	తిరిగి;

    ఒకవేళ (!gotchars_కూడు_byte(&state, byte))
	తిరిగి;

    state.buf[state.buflen] = NUL;
    state.buflen = 0;

    ptr = state.buf;
    ఒకవేళ (ptr[0] == K_SPECIAL && ptr[1] == KS_MODIFIER && ptr[2] != NUL)
    {
	modifiers = ptr[2];
	ptr += 3;
    }

    ఒకవేళ (*ptr != NUL)
    {
	char_u		*mb_ptr = mb_unescape(&ptr);

	c = mb_ptr != ఖాళీ ? (*mb_ptr2char)(mb_ptr) : *ptr++;
	ఒకవేళ (c <= 0x7f)
	{
	    // Merge modifiers into the key to make the result more readable.
	    పూర్ణాంకం		modifiers_after = modifiers;
	    పూర్ణాంకం		mod_c = విలీనం_modify_Other_కీలకాలు(c, &modifiers_after);

	    ఒకవేళ (modifiers_after == 0)
	    {
		modifiers = 0;
		c = mod_c;
	    }
	}
    }

    // TODO: is there a more readable and yet compact representation of
    // modifiers and special keys?
    ఒకవేళ (modifiers != 0)
    {
	కూడు_గమ్యం_showcmd(K_SPECIAL);
	కూడు_గమ్యం_showcmd(KS_MODIFIER);
	కూడు_గమ్యం_showcmd(modifiers);
    }
    ఒకవేళ (c != NUL)
	కూడు_గమ్యం_showcmd(c);
    అయితే (*ptr != NUL)
	కూడు_గమ్యం_showcmd(*ptr++);
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
    పూర్ణాంకం
vgetc(శూన్య)
{
    పూర్ణాంకం		c, c2;
    పూర్ణాంకం		n;
    char_u	buf[MB_MAXBYTES + 1];
    పూర్ణాంకం		i;

#నిర్వచితమైతే FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    ఒకవేళ (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#అయితే_ముగింపు

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    ఒకవేళ (can_get_old_char())
    {
	c = old_char;
	old_char = -1;
	mod_ముసుగు = old_mod_ముసుగు;
	mouse_అడ్డువరుస = old_mouse_అడ్డువరుస;
	mouse_నిలువు = old_mouse_నిలువు;
    }
    లేకపోతే
    {
	// number of characters recorded from the last vgetc() call
	స్థిర size_t	చివరి_vgetc_recorded_len = 0;

	mod_ముసుగు = 0;
	vgetc_mod_ముసుగు = 0;
	vgetc_char = 0;

	// last_recorded_len can be larger than last_vgetc_recorded_len
	// if peeking records more
	చివరి_recorded_len -= చివరి_vgetc_recorded_len;

	కొరకు (;;)		// this is done twice if there are modifiers
	{
	    పూర్ణాంకం did_inc = FALSE;

	    // No mapping after modifier has been read, using an input method
	    // and when a popup window has disabled mapping.
	    ఒకవేళ (mod_ముసుగు
#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#అయితే_ముగింపు
		    )
	    {
		++no_mapping;
		++allow_కీలకాలు;
		// mod_mask value may change, remember we did the increment
		did_inc = TRUE;
	    }
	    c = vgetorpeek(TRUE);
	    ఒకవేళ (did_inc)
	    {
		--no_mapping;
		--allow_కీలకాలు;
	    }

	    // Get two extra bytes for special keys, handle modifiers.
	    ఒకవేళ (c == K_SPECIAL
#నిర్వచితమైతే FEAT_GUI
		    || c == CSI
#అయితే_ముగింపు
	       )
	    {
		పూర్ణాంకం	    save_allow_కీలకాలు = allow_కీలకాలు;

		++no_mapping;
		allow_కీలకాలు = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_కీలకాలు = save_allow_కీలకాలు;
		ఒకవేళ (c2 == KS_MODIFIER)
		{
		    mod_ముసుగు = c;
		    కొనసాగు;
		}
		c = గమ్యం_SPECIAL(c2, c);

		ఒకవేళ (allow_osc_కీలకం == 0 && c == K_OSC)
		    కొనసాగు;

		// K_ESC is used to avoid ambiguity with the single Esc
		// character that might be the start of an escape sequence.
		// Convert it back to a single Esc here.
		ఒకవేళ (c == K_ESC)
		    c = ESC;

#ఒకవేళ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		// Handle K_TEAROFF here, the caller of vgetc() doesn't need to
		// know that a menu was torn off
		ఒకవేళ (
# ifdef VIMDLL
		    gui.లోపల_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    పూర్ణాంకం		j;

		    // get menu path, it ends with a <CR>
		    కొరకు (j = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[j] = c;
			ఒకవేళ (j < 199)
			    ++j;
		    }
		    name[j] = NUL;
		    gui_make_tearoff(name);
		    కొనసాగు;
		}
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && !defined(USE_GTK4) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		ఒకవేళ (c == K_F10 && gui.menubar != ఖాళీ)
		{
		    gtk_menu_shell_select_మొదటి(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    కొనసాగు;
		}
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
		// Handle focus event here, so that the caller doesn't need to
		// know about it.  Return K_IGNORE so that we loop once (needed
		// if 'lazyredraw' is set).
		ఒకవేళ (c == K_FOCUSGAINED || c == K_FOCUSLOST)
		{
		    ui_focus_change(c == K_FOCUSGAINED);
		    c = K_IGNORE;
		}

		// Translate K_CSI to CSI.  The special key is only used to
		// avoid it being recognized as the start of a special key.
		ఒకవేళ (c == K_CSI)
		    c = CSI;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
		ఒకవేళ (c == K_SID)
		{
		    పూర్ణాంకం	    j;

		    // Handle <SID>{sid};  Do up to 20 digits for safety.
		    చివరి_వాడిన_sid = 0;
		    కొరకు (j = 0; j < 20 && సురక్షిత_isdigit(c = vgetorpeek(TRUE)); ++j)
			చివరి_వాడిన_sid = చివరి_వాడిన_sid * 10 + (c - '0');
		    చివరి_వాడిన_map = ఖాళీ;
		    కొనసాగు;
		}
#అయితే_ముగింపు
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    ఒకవేళ (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		కొరకు (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    ఒకవేళ (buf[i] == K_SPECIAL
#నిర్వచితమైతే FEAT_GUI
			    || (buf[i] == CSI)
#అయితే_ముగింపు
			    )
		    {
			// Must be a K_SPECIAL - KS_SPECIAL - KE_FILLER
			// sequence, which represents a K_SPECIAL (0x80),
			// or a CSI - KS_EXTRA - KE_CSI sequence, which
			// represents a CSI (0x9B),
			// or a K_SPECIAL - KS_EXTRA - KE_CSI, which is CSI
			// too.
			c = vgetorpeek(TRUE);
			ఒకవేళ (vgetorpeek(TRUE) == KE_CSI && c == KS_EXTRA)
			    buf[i] = CSI;
		    }
		}
		--no_mapping;
		c = (*mb_ptr2char)(buf);
	    }

	    ఒకవేళ (vgetc_char == 0)
	    {
		vgetc_mod_ముసుగు = mod_ముసుగు;
		vgetc_char = c;
	    }

	    // A keypad or special function key was not mapped, use it like
	    // its ASCII equivalent.
	    ఎంపిక (c)
	    {
		స్థితి K_KPLUS:	c = '+'; ఆపు;
		స్థితి K_KMINUS:	c = '-'; ఆపు;
		స్థితి K_KDIVIDE:	c = '/'; ఆపు;
		స్థితి K_KMULTIPLY: c = '*'; ఆపు;
		స్థితి K_KENTER:	c = CAR; ఆపు;
		స్థితి K_KPOINT:
#నిర్వచితమైతే MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = Map_Virtual_కీలకం(VK_DECIMAL, 2); ఆపు;
#లేకపోతే
				c = '.'; ఆపు;
#అయితే_ముగింపు
		స్థితి K_K0:	c = '0'; ఆపు;
		స్థితి K_K1:	c = '1'; ఆపు;
		స్థితి K_K2:	c = '2'; ఆపు;
		స్థితి K_K3:	c = '3'; ఆపు;
		స్థితి K_K4:	c = '4'; ఆపు;
		స్థితి K_K5:	c = '5'; ఆపు;
		స్థితి K_K6:	c = '6'; ఆపు;
		స్థితి K_K7:	c = '7'; ఆపు;
		స్థితి K_K8:	c = '8'; ఆపు;
		స్థితి K_K9:	c = '9'; ఆపు;

		స్థితి K_XHOME:
		స్థితి K_ZHOME:	ఒకవేళ (mod_ముసుగు == MOD_ముసుగు_జరుపు)
				{
				    c = K_S_HOME;
				    mod_ముసుగు = 0;
				}
				లేకపోతే ఒకవేళ (mod_ముసుగు == MOD_ముసుగు_CTRL)
				{
				    c = K_C_HOME;
				    mod_ముసుగు = 0;
				}
				లేకపోతే
				    c = K_HOME;
				ఆపు;
		స్థితి K_XEND:
		స్థితి K_ZEND:	ఒకవేళ (mod_ముసుగు == MOD_ముసుగు_జరుపు)
				{
				    c = K_S_చివర;
				    mod_ముసుగు = 0;
				}
				లేకపోతే ఒకవేళ (mod_ముసుగు == MOD_ముసుగు_CTRL)
				{
				    c = K_C_చివర;
				    mod_ముసుగు = 0;
				}
				లేకపోతే
				    c = K_చివర;
				ఆపు;

		స్థితి K_XUP:	c = K_పైకి; ఆపు;
		స్థితి K_XDOWN:	c = K_కిందికి; ఆపు;
		స్థితి K_XLEFT:	c = K_ఎడమ; ఆపు;
		స్థితి K_XRIGHT:	c = K_కుడి; ఆపు;
	    }

	    ఆపు;
	}

	చివరి_vgetc_recorded_len = చివరి_recorded_len;
    }

#నిర్వచితమైతే FEAT_EVAL
    /*
     * In the main loop "may_garbage_collect" can be set to do garbage
     * collection in the first next vgetc().  It's disabled after that to
     * avoid internally used Lists and Dicts to be freed.
     */
    may_garbage_collect = FALSE;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_BEVAL_పదం
    ఒకవేళ (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD
	    && c != K_COMPLETE_DELAY)
    {
	// Don't trigger 'balloonexpr' unless only the mouse was moved.
	bevalexpr_due_సమితి = FALSE;
	ui_remove_balloon();
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    // Only filter keys that do not come from ":normal".  Keys from feedkeys()
    // are filtered.
    ఒకవేళ ((!ex_సాధారణ_busy || లోపల_feedkeys) && popup_do_filter(c))
    {
	ఒకవేళ (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    c = do_కీలకం_ఆదానం_pre(c);

    // Clear the next typedchars_pos
    typedchars_pos = 0;
#అయితే_ముగింపు

    // Need to process the character before we know it's safe to do something
    // else.
    ఒకవేళ (c != K_IGNORE)
	state_no_longer_సురక్షిత("key typed");

    తిరిగి c;
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Handle the KeyInputPre autocommand.
 * "c" is the character that was typed.
 * Return new input character.
 */
    స్థిర పూర్ణాంకం
do_కీలకం_ఆదానం_pre(పూర్ణాంకం c)
{
    పూర్ణాంకం		res = c;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    char_u	curr_బహుళకం[బహుళకం_MAX_నిడివి];
    పూర్ణాంకం		save_State = State;
    save_v_event_T save_v_event;
    dict_T	*v_event;

    // Return quickly when there is nothing to do.
    ఒకవేళ (!has_keyinputpre())
	తిరిగి res;

    ఒకవేళ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = KEY2TERMCAP0(c);
	buf[2] = KEY2TERMCAP1(c);
	buflen = 3;
    }
    లేకపోతే
	buflen = (*mb_char2bytes)(c, buf);
    buf[buflen] = NUL;

    typedchars[typedchars_pos] = NUL;
    vim_unescape_csi(typedchars);

    get_బహుళకం(curr_బహుళకం);

    // Lock the text to avoid weird things from happening.
    ++textlock;
    సమితి_vim_var_string(VV_CHAR, buf, (పూర్ణాంకం)buflen);  // set v:char

    v_event = get_v_event(&save_v_event);
    (శూన్య)dict_కూడు_bool(v_event, "typed", కీలకం_Typed);
    (శూన్య)dict_కూడు_string_len(v_event, "typedchar", typedchars, typedchars_pos);

    ఒకవేళ (apply_autocmds(EVENT_KEYINPUTPRE, curr_బహుళకం, curr_బహుళకం, FALSE, curbuf))
    {
	string_T    v_char;

	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character.
	v_char.string = get_vim_var_str(VV_CHAR);
	ఒకవేళ (STRCMP(buf, v_char.string) != 0)
	{
	    v_char.నిడివి = STRLEN(v_char.string);
	    // Convert special bytes when it is special string.
	    ఒకవేళ (v_char.నిడివి >= 3 && v_char.string[0] == K_SPECIAL)
		res = TERMCAP2_కీలకం(v_char.string[1], v_char.string[2]);
	    లేకపోతే ఒకవేళ (v_char.నిడివి > 0)
		res = PTR2CHAR(v_char.string);
	}
    }

    restore_v_event(v_event, &save_v_event);

    సమితి_vim_var_string(VV_CHAR, ఖాళీ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    తిరిగి res;
}
#అయితే_ముగింపు

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    పూర్ణాంకం
సురక్షిత_vgetc(శూన్య)
{
    పూర్ణాంకం	c;

    c = vgetc();
    ఒకవేళ (c == NUL)
	c = get_keystroke();
    తిరిగి c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 * Does not handle bracketed paste - do not use the result for commands.
 */
    స్థిర పూర్ణాంకం
plain_vgetc_nopaste(శూన్య)
{
    పూర్ణాంకం c;

    చేయి
	c = సురక్షిత_vgetc();
    అయితే (c == K_IGNORE
	    || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
	    || c == K_MOUSEMOVE);
    తిరిగి c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    పూర్ణాంకం
plain_vgetc(శూన్య)
{
    పూర్ణాంకం c = plain_vgetc_nopaste();

    ఒకవేళ (c == K_PS)
	// Only handle the first pasted character.  Drop the rest, since we
	// don't know what to do with it.
	c = bracketed_paste(PASTE_ONE_CHAR, FALSE, ఖాళీ);

    తిరిగి c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 * Returns NUL if no character is available.
 */
    పూర్ణాంకం
vpeekc(శూన్య)
{
    ఒకవేళ (can_get_old_char())
	తిరిగి old_char;
    తిరిగి vgetorpeek(FALSE);
}

#ఒకవేళ defined(FEAT_TERMRESPONSE) || defined(FEAT_TERMINAL)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    పూర్ణాంకం
vpeekc_nomap(శూన్య)
{
    పూర్ణాంకం		c;

    ++no_mapping;
    ++allow_కీలకాలు;
    c = vpeekc();
    --no_mapping;
    --allow_కీలకాలు;
    తిరిగి c;
}
#అయితే_ముగింపు

/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    పూర్ణాంకం
vpeekc_any(శూన్య)
{
    పూర్ణాంకం		c;

    c = vpeekc();
    ఒకవేళ (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    తిరిగి c;
}

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    పూర్ణాంకం
char_avail(శూన్య)
{
    పూర్ణాంకం	    retval;

#నిర్వచితమైతే FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    ఒకవేళ (disable_char_avail_for_testing)
	తిరిగి FALSE;
#అయితే_ముగింపు
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    తిరిగి (retval != NUL);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * "getchar()" and "getcharstr()" functions
 */
    స్థిర శూన్య
getchar_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం allow_సంఖ్య)
{
    varnumber_T		n = 0;
    పూర్ణాంకం			called_emsg_ప్రారంభం = called_emsg;
    పూర్ణాంకం			error = FALSE;
    పూర్ణాంకం			simplify = TRUE;
    char_u		cursor_flag = 'm';

    ఒకవేళ ((లోపల_vim9script()
		&& check_for_opt_bool_or_సంఖ్య_arg(argvars, 0) == FAIL)
	    || (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type == VAR_DICT)
    {
	dict_T		*d = argvars[1].vval.v_dict;
	char_u		*cursor_str;

	ఒకవేళ (allow_సంఖ్య)
	    allow_సంఖ్య = dict_get_bool(d, "number", TRUE);
	లేకపోతే ఒకవేళ (dict_has_కీలకం(d, "number"))
	    semsg(_(e_invalid_వాదం_str), "number");

	simplify = dict_get_bool(d, "simplify", TRUE);

	cursor_str = dict_get_string(d, "cursor", FALSE);
	ఒకవేళ (cursor_str != ఖాళీ)
	{
	    ఒకవేళ (STRCMP(cursor_str, "hide") != 0
		    && STRCMP(cursor_str, "keep") != 0
		    && STRCMP(cursor_str, "msg") != 0)
		semsg(_(e_invalid_విలువ_for_వాదం_str_str), "cursor",
								   cursor_str);
	    లేకపోతే
		cursor_flag = cursor_str[0];
	}
    }

    ఒకవేళ (called_emsg != called_emsg_ప్రారంభం)
	తిరిగి;

# ifdef సందేశం_వరుస
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
# endif

    ఒకవేళ (cursor_flag == 'h')
	cursor_sleep();
    లేకపోతే ఒకవేళ (cursor_flag == 'm')
	windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);

    ++no_mapping;
    ++allow_కీలకాలు;
    ++allow_osc_కీలకం;
    ఒకవేళ (!simplify)
	++no_reduce_కీలకాలు;
    కొరకు (;;)
    {
	ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN
		|| (argvars[0].v_type == VAR_సంఖ్య
			&& argvars[0].vval.v_సంఖ్య == -1))
	    // getchar(): blocking wait.
	    n = plain_vgetc_nopaste();
	లేకపోతే ఒకవేళ (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	లేకపోతే ఒకవేళ (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	లేకపోతే
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = సురక్షిత_vgetc();

	ఒకవేళ (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    కొనసాగు;
	ఆపు;
    }
    --no_mapping;
    --allow_కీలకాలు;
    --allow_osc_కీలకం;
    ఒకవేళ (!simplify)
	--no_reduce_కీలకాలు;

    ఒకవేళ (cursor_flag == 'h')
	cursor_unsleep();

    సమితి_vim_var_nr(VV_MOUSE_WIN, 0);
    సమితి_vim_var_nr(VV_MOUSE_WINID, 0);
    సమితి_vim_var_nr(VV_MOUSE_LNUM, 0);
    సమితి_vim_var_nr(VV_MOUSE_నిలువు, 0);

    ఒకవేళ (n != 0 && (!allow_సంఖ్య || IS_SPECIAL(n) || mod_ముసుగు != 0))
    {
	char_u		తాత్కాలిక[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	పూర్ణాంకం		i = 0;

	// Turn a special key into three bytes, plus modifier.
	ఒకవేళ (mod_ముసుగు != 0)
	{
	    తాత్కాలిక[i++] = K_SPECIAL;
	    తాత్కాలిక[i++] = KS_MODIFIER;
	    తాత్కాలిక[i++] = mod_ముసుగు;
	}
	ఒకవేళ (IS_SPECIAL(n))
	{
	    తాత్కాలిక[i++] = K_SPECIAL;
	    తాత్కాలిక[i++] = K_రెండవ(n);
	    తాత్కాలిక[i++] = K_మూడవ(n);
	}
	లేకపోతే ఒకవేళ (has_mbyte)
	    i += (*mb_char2bytes)(n, తాత్కాలిక + i);
	లేకపోతే
	    తాత్కాలిక[i++] = n;
	తాత్కాలిక[i] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strnsave(తాత్కాలిక, i);

	ఒకవేళ (is_mouse_కీలకం(n))
	{
	    పూర్ణాంకం		అడ్డువరుస = mouse_అడ్డువరుస;
	    పూర్ణాంకం		నిలువు = mouse_నిలువు;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    పూర్ణాంకం		winnr = 1;

	    ఒకవేళ (అడ్డువరుస >= 0 && నిలువు >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);
		ఒకవేళ (win == ఖాళీ)
		    తిరిగి;
		(శూన్య)mouse_comp_pos(win, &అడ్డువరుస, &నిలువు, &lnum, ఖాళీ);
# ifdef FEAT_PROP_POPUP
		ఒకవేళ (WIN_IS_POPUP(win))
		    winnr = 0;
		లేకపోతే
# endif
		    కొరకు (wp = firstwin; wp != win && wp != ఖాళీ;
							       wp = wp->w_తదుపరి)
			++winnr;
		సమితి_vim_var_nr(VV_MOUSE_WIN, winnr);
		సమితి_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		సమితి_vim_var_nr(VV_MOUSE_LNUM, lnum);
		సమితి_vim_var_nr(VV_MOUSE_నిలువు, నిలువు + 1);
	    }
	}
    }
    లేకపోతే ఒకవేళ (!allow_సంఖ్య)
	rettv->v_type = VAR_STRING;
    లేకపోతే
	rettv->vval.v_సంఖ్య = n;
}

/*
 * "getchar()" function
 */
    శూన్య
f_getchar(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, TRUE);
}

/*
 * "getcharstr()" function
 */
    శూన్య
f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, FALSE);
}

/*
 * "getcharmod()" function
 */
    శూన్య
f_getcharmod(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = mod_ముసుగు;
}
#అయితే_ముగింపు // FEAT_EVAL

#ఒకవేళ defined(సందేశం_వరుస)
# define MAX_REPEAT_PARSE 8

/*
 * Process messages that have been queued for netbeans or clientserver.
 * Also check if any jobs have ended.
 * These functions can call arbitrary Vim script and should only be called when
 * it is safe to do so.
 */
    శూన్య
parse_queued_messages(శూన్య)
{
    పూర్ణాంకం	    old_curwin_id;
    పూర్ణాంకం	    old_curbuf_fnum;
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    save_may_garbage_collect = may_garbage_collect;
    స్థిర పూర్ణాంకం entered = 0;
    పూర్ణాంకం	    was_సురక్షిత = get_was_సురక్షిత_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    ఒకవేళ (updating_screen || dont_parse_messages)
	తిరిగి;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    ఒకవేళ (curbuf == ఖాళీ || curwin == ఖాళీ)
	తిరిగి;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    కొరకు (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# ఒకవేళ (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_పని_CHANNEL)
	channel_handle_events(FALSE);
# endif

# ifdef FEAT_NETBEANS_INTG
	// Process the queued netbeans messages.
	netbeans_parse_messages();
# endif
# ifdef FEAT_SOCKETSERVER
	socketserver_parse_messages();
# endif
# ifdef FEAT_పని_CHANNEL
	// Write any buffer lines still to be written.
	channel_write_any_పంక్తులు();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# ఒకవేళ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_పని_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	ఒకవేళ (పని_check_ended())
	    కొనసాగు;
# endif
# ifdef FEAT_TERMINAL
	free_వాడని_terminals();
# endif

# ifdef FEAT_SOUND_MACOSX
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	ఒకవేళ (has_sound_callback_లోపల_వరుస())
	    invoke_sound_callback();
# endif
# ifdef SIGUSR1
	ఒకవేళ (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, ఖాళీ, ఖాళీ, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
# endif
	ఆపు;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    ఒకవేళ (entered == 1 && was_సురక్షిత)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    ఒకవేళ (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}
#అయితే_ముగింపు


రకనిర్వచనం గణన {
    map_ఫలితం_fail,    // failed, break loop
    map_ఫలితం_get,     // get a character from typeahead
    map_ఫలితం_retry,   // try to map again
    map_ఫలితం_nomatch  // no matching mapping, get char
} map_ఫలితం_T;

/*
 * Check if the bytes at the start of the typeahead buffer are a character used
 * in Insert mode completion.  This includes the form with a CTRL modifier.
 * During completion started by complete() keys are mapped as usual.
 */
    స్థిర పూర్ణాంకం
at_ins_compl_కీలకం(శూన్య)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    పూర్ణాంకం	    c = *p;

    ఒకవేళ (typebuf.tb_len > 3
	    && (c == K_SPECIAL || c == CSI)  // CSI is used by the GUI
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_ముసుగు_CTRL)
	    // CTRL-SHIFT-N/P scroll the info popup, so they must not be folded
	    // to the CTRL-N/CTRL-P completion keys here.
	    && !(p[2] & MOD_ముసుగు_జరుపు))
	c = p[3] & 0x1f;
    తిరిగి !ctrl_x_బహుళకం_eval()
	    && ((ctrl_x_బహుళకం_not_default() && vim_is_ctrl_x_కీలకం(c))
		|| (compl_స్థితి_local() && (c == Ctrl_N || c == Ctrl_P)));
}

/*
 * Check if typebuf.tb_buf[] contains a modifier plus key that can be changed
 * into just a key, apply that.
 * Check from typebuf.tb_buf[typebuf.tb_off] to typebuf.tb_buf[typebuf.tb_off
 * + "max_offset"].
 * Return the length of the replaced bytes, 0 if nothing changed, -1 for error.
 */
    స్థిర పూర్ణాంకం
check_simplify_modifier(పూర్ణాంకం max_స్థానభ్రంశం)
{
    పూర్ణాంకం		స్థానభ్రంశం;
    char_u	*tp;

    కొరకు (స్థానభ్రంశం = 0; స్థానభ్రంశం < max_స్థానభ్రంశం; ++స్థానభ్రంశం)
    {
	ఒకవేళ (స్థానభ్రంశం + 3 >= typebuf.tb_len)
	    ఆపు;
	tp = typebuf.tb_buf + typebuf.tb_off + స్థానభ్రంశం;
	ఒకవేళ ((tp[0] == K_SPECIAL || tp[0] == CSI) && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    పూర్ణాంకం modifier = tp[2];
	    పూర్ణాంకం	c = tp[3];
	    పూర్ణాంకం new_c = విలీనం_modify_Other_కీలకాలు(c, &modifier);

	    ఒకవేళ (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		పూర్ణాంకం	len;
		పూర్ణాంకం	కీలకం_స్థానభ్రంశం = స్థానభ్రంశం;

		ఒకవేళ (స్థానభ్రంశం == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_ముసుగు = tp[2];
		}
		ఒకవేళ (IS_SPECIAL(new_c))
		{
		    new_string[0] = K_SPECIAL;
		    new_string[1] = K_రెండవ(new_c);
		    new_string[2] = K_మూడవ(new_c);
		    len = 3;
		}
		లేకపోతే
		    len = mb_char2bytes(new_c, new_string);
		ఒకవేళ (modifier == 0)
		{
		    ఒకవేళ (put_string_లోపల_typebuf(స్థానభ్రంశం, 4, new_string, len,
							ఖాళీ, 0, ఖాళీ) == FAIL)
			తిరిగి -1;
		}
		లేకపోతే
		{
		    tp[2] = modifier;
		    కీలకం_స్థానభ్రంశం = స్థానభ్రంశం + 3;
		    ఒకవేళ (put_string_లోపల_typebuf(కీలకం_స్థానభ్రంశం, 1, new_string, len,
							ఖాళీ, 0, ఖాళీ) == FAIL)
			తిరిగి -1;
		}

		// A mapping for the simplified key can be used for it even
		// when the key protocol is enabled.
		కొరకు (పూర్ణాంకం i = 0; i < len; ++i)
		    typebuf.tb_noremap[typebuf.tb_off + కీలకం_స్థానభ్రంశం + i]
							    |= RM_SIMPLIFIED;
		తిరిగి len;
	    }
	}
    }
    తిరిగి 0;
}

/*
 * Return TRUE if the terminal sends modifiers with various keys.  This is when
 * modifyOtherKeys level 2 is enabled or the kitty keyboard protocol is
 * enabled.
 */
    పూర్ణాంకం
కీలకం_protocol_enabled(శూన్య)
{
    // If xterm has responded to XTQMODKEYS it overrules seenModifyOtherKeys.
    పూర్ణాంకం using_mok = modify_otherkeys_state != MOKS_INITIAL
			? modify_otherkeys_state == MOKS_ENABLED
			: చూసిన_Modify_Other_కీలకాలు;
    తిరిగి using_mok || kitty_protocol_state == KKPS_ENABLED;
}

/*
 * Handle mappings in the typeahead buffer.
 * - When something was mapped, return map_result_retry for recursive mappings.
 * - When nothing mapped and typeahead has a character: return map_result_get.
 * - When there is no match yet, return map_result_nomatch, need to get more
 *   typeahead.
 * - On failure (out of memory) return map_result_fail.
 */
    స్థిర పూర్ణాంకం
handle_mapping(
	    పూర్ణాంకం *keylenp,
	    పూర్ణాంకం *timedout,
	    పూర్ణాంకం *mapdepth)
{
    mapblock_T	*mp = ఖాళీ;
    mapblock_T	*mp2;
    mapblock_T	*mp_సరిపోలిక;
    పూర్ణాంకం		mp_సరిపోలిక_len = 0;
    పూర్ణాంకం		max_mlen = 0;
    పూర్ణాంకం		want_termcode = 0;  // 1 if termcode expected after max_mlen
    పూర్ణాంకం		tb_c1;
    పూర్ణాంకం		mlen;
#నిర్వచితమైతే FEAT_LANGMAP
    పూర్ణాంకం		nolmaplen;
#అయితే_ముగింపు
    పూర్ణాంకం		keylen = *keylenp;
    పూర్ణాంకం		i;
    పూర్ణాంకం		local_State = get_real_state();
    పూర్ణాంకం		is_plug_map = FALSE;
    bool	లోపల_osc = లోపల_osc_sequence(); // If we are in an OSC sequence,
					    // then go straight to
					    // check_termcode() in order to
					    // consume chars.

    // If typeahead starts with <Plug> then remap, even for a "noremap" mapping.
    ఒకవేళ (!లోపల_osc && typebuf.tb_len >= 3
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
    ఒకవేళ (!లోపల_osc && no_mapping == 0 && is_maphash_valid()
	    && (no_zero_mapping == 0 || tb_c1 != '0')
	    && (typebuf.tb_maplen == 0 || is_plug_map
		|| (p_remap
		    && (typebuf.tb_noremap[typebuf.tb_off]
				    & (RM_NONE|RM_ABBR)) == 0))
	    && !(p_paste && (State & (బహుళకం_చొప్పించు | బహుళకం_CMDLINE)))
	    && !(State == బహుళకం_HITRETURN && (tb_c1 == CAR || tb_c1 == ' '))
	    && State != బహుళకం_ASKMORE
	    && State != బహుళకం_CONFIRM
	    && !at_ins_compl_కీలకం())
    {
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.లోపల_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LANGMAP
	ఒకవేళ (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	లేకపోతే
	{
	    LANGMAP_ADJUST(tb_c1, (State & (బహుళకం_CMDLINE | బహుళకం_చొప్పించు)) == 0
					   && get_real_state() != బహుళకం_SELECT);
	    nolmaplen = 0;
	}
#అయితే_ముగింపు
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	ఒకవేళ (mp == ఖాళీ)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = ఖాళీ;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_సరిపోలిక = ఖాళీ;
	mp_సరిపోలిక_len = 0;
	కొరకు ( ; mp != ఖాళీ;
	       mp->m_తదుపరి == ఖాళీ ? (mp = mp2, mp2 = ఖాళీ) : (mp = mp->m_తదుపరి))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    ఒకవేళ (mp->m_కీలకాలు[0] == tb_c1
		    && (mp->m_బహుళకం & local_State)
		    && !(mp->m_simplified && కీలకం_protocol_enabled()
			    && typebuf.tb_maplen == 0
			    && (typebuf.tb_noremap[typebuf.tb_off]
							& RM_SIMPLIFIED) == 0)
		    && ((mp->m_బహుళకం & బహుళకం_LANGMAP) == 0
						    || typebuf.tb_maplen == 0))
	    {
#నిర్వచితమైతే FEAT_LANGMAP
		పూర్ణాంకం	nomap = nolmaplen;
		పూర్ణాంకం	modifiers = 0;
#అయితే_ముగింపు
		// find the match length of this mapping
		కొరకు (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
		    పూర్ణాంకం	c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
#నిర్వచితమైతే FEAT_LANGMAP
		    ఒకవేళ (nomap > 0)
		    {
			ఒకవేళ (nomap == 2 && c2 == KS_MODIFIER)
			    modifiers = 1;
			లేకపోతే ఒకవేళ (nomap == 1 && modifiers == 1)
			    modifiers = c2;
			--nomap;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (c2 == K_SPECIAL)
			    nomap = 2;
			లేకపోతే ఒకవేళ (విలీనం_modify_Other_కీలకాలు(c2, &modifiers) == c2)
			    // Only apply 'langmap' if merging modifiers into
			    // the key will not result in another character,
			    // so that 'langmap' behaves consistently in
			    // different terminals and GUIs.
			    LANGMAP_ADJUST(c2, TRUE);
			modifiers = 0;
		    }
#అయితే_ముగింపు
		    ఒకవేళ (mp->m_కీలకాలు[mlen] != c2)
			ఆపు;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_కీలకాలు;
		    char_u *p2 = mb_unescape(&p1);

		    ఒకవేళ (has_mbyte && p2 != ఖాళీ
					&& MB_BYTE2LEN(tb_c1) > mb_ptr2len(p2))
			mlen = 0;
		}

		// Check an entry whether it matches.
		// - Full match: mlen == keylen
		// - Partly match: mlen == typebuf.tb_len
		keylen = mp->m_keylen;
		ఒకవేళ (mlen == keylen || (mlen == typebuf.tb_len
						   && typebuf.tb_len < keylen))
		{
		    char_u  *s;
		    పూర్ణాంకం	    n;

		    // If only script-local mappings are allowed, check if the
		    // mapping starts with K_SNR.
		    s = typebuf.tb_noremap + typebuf.tb_off;
		    ఒకవేళ ((*s & ~RM_SIMPLIFIED) == RM_SCRIPT
			    && (mp->m_కీలకాలు[0] != K_SPECIAL
				|| mp->m_కీలకాలు[1] != KS_EXTRA
				|| mp->m_కీలకాలు[2] != KE_SNR))
			కొనసాగు;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    కొరకు (n = mlen; --n >= 0; )
			ఒకవేళ (*s++ & (RM_NONE|RM_ABBR))
			    ఆపు;
		    ఒకవేళ (!is_plug_map && n >= 0)
			కొనసాగు;

		    ఒకవేళ (keylen > typebuf.tb_len)
		    {
			ఒకవేళ (!*timedout && !(mp_సరిపోలిక != ఖాళీ
							&& mp_సరిపోలిక->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    ఆపు;
			}
		    }
		    లేకపోతే ఒకవేళ (keylen > mp_సరిపోలిక_len)
		    {
			// found a longer match
			mp_సరిపోలిక = mp;
			mp_సరిపోలిక_len = keylen;
		    }
		}
		లేకపోతే
		{
		    // No match; may have to check for termcode at next
		    // character.

		    // If the first character that didn't match is
		    // K_SPECIAL then check for a termcode.  This isn't perfect
		    // but should work in most cases.
		    ఒకవేళ (max_mlen < mlen)
		    {
			max_mlen = mlen;
			want_termcode = mp->m_కీలకాలు[mlen] == K_SPECIAL;
		    }
		    లేకపోతే ఒకవేళ (max_mlen == mlen && mp->m_కీలకాలు[mlen] == K_SPECIAL)
			want_termcode = 1;

		    // Check termcode for uppercase character to properly
		    // process "ESC[27;2;<ascii code>~" control sequences.
		    ఒకవేళ (ASCII_ISUPPER(mp->m_కీలకాలు[mlen]))
			want_termcode = 1;
		}
	    }
	}

	// If no partly match found, use the longest full match.
	ఒకవేళ (keylen != KEYLEN_PART_MAP && mp_సరిపోలిక != ఖాళీ)
	{
	    mp = mp_సరిపోలిక;
	    keylen = mp_సరిపోలిక_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    ఒకవేళ (!లోపల_osc && *p_pt != NUL && mp == ఖాళీ &&
	    (State & (బహుళకం_చొప్పించు | బహుళకం_సాధారణ)))
    {
	కొరకు (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    ఒకవేళ (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    ఆపు;
	ఒకవేళ (p_pt[mlen] == NUL)	//సరిపోలిక
	{
	    // write chars to script file(s)
	    ఒకవేళ (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    సమితి_option_విలువ_give_err((char_u *)"paste",
						      (దీర్ఘ)!p_paste, ఖాళీ, 0);
	    ఒకవేళ (!(State & బహుళకం_చొప్పించు))
	    {
		సందేశం_నిలువు = 0;
		సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
		సందేశం_clr_eos();		// clear ruler
	    }
	    స్థితి_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    తిరిగి map_ఫలితం_retry;
	}
	// Need more chars for partly match.
	ఒకవేళ (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_కీలకం;
	లేకపోతే ఒకవేళ (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    // May check for a terminal code when there is no mapping or only a partial
    // mapping.  Also check if there is a full mapping with <Esc>, unless timed
    // out, since that is nearly always a partial match with a terminal code.
    ఒకవేళ (లోపల_osc || ((mp == ఖాళీ || max_mlen + want_termcode > mp_సరిపోలిక_len
		    || (mp_సరిపోలిక_len == 1 && *mp->m_కీలకాలు == ESC && !*timedout))
	    && keylen != KEYLEN_PART_MAP))
    {
	పూర్ణాంకం	save_keylen = keylen;

	/*
	 * When no matching mapping found or found a non-matching mapping that
	 * matches at least what the matching mapping matched:
	 * Check if we have a terminal code, when:
	 * - mapping is allowed,
	 * - keys have not been mapped,
	 * - and not an ESC sequence, not in insert mode or p_ek is on,
	 * - and when not timed out,
	 */
	ఒకవేళ (లోపల_osc || no_mapping == 0 || allow_కీలకాలు != 0)
	{
	    ఒకవేళ (లోపల_osc || ((typebuf.tb_maplen == 0
			    || (p_remap && (typebuf.tb_noremap[typebuf.tb_off]
						& ~RM_SIMPLIFIED) == RM_YES))
		    && !*timedout))
		keylen = check_termcode(max_mlen + 1, ఖాళీ, 0, ఖాళీ);
	    లేకపోతే
		keylen = 0;

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    ఒకవేళ (keylen == 0 && save_keylen == KEYLEN_PART_కీలకం && !*timedout)
		keylen = KEYLEN_PART_కీలకం;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
#నిర్వచితమైతే FEAT_TERMINAL
	    check_no_reduce_కీలకాలు();  // may update the no_reduce_keys flag
#అయితే_ముగింపు
	    ఒకవేళ (keylen == 0 && !no_reduce_కీలకాలు)
	    {
		keylen = check_simplify_modifier(max_mlen + 1);
		ఒకవేళ (keylen < 0)
		    // ins_typebuf() failed
		    తిరిగి map_ఫలితం_fail;
	    }

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    ఒకవేళ (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	లేకపోతే
	    keylen = 0;
	ఒకవేళ (keylen == 0)	// no matching terminal code
	{
#నిర్వచితమైతే AMIGA
	    // check for window bounds report
	    ఒకవేళ (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		కొరకు (s = typebuf.tb_buf + typebuf.tb_off + 1;
			   s < typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len
		   && (VIM_ISDIGIT(*s) || *s == ';' || *s == ' ');
			++s)
		    ;
		ఒకవేళ (*s == 'r' || *s == '|') // found one
		{
		    del_typebuf(
			  (పూర్ణాంకం)(s + 1 - (typebuf.tb_buf + typebuf.tb_off)), 0);
		    // get size and redraw screen
		    shell_resized();
		    *keylenp = keylen;
		    తిరిగి map_ఫలితం_retry;
		}
		ఒకవేళ (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_కీలకం;
	    }
	    ఒకవేళ (keylen >= 0)
#అయితే_ముగింపు
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		ఒకవేళ (mp == ఖాళీ)
		{
		    *keylenp = keylen;
		    తిరిగి map_ఫలితం_get;    // get character from typeahead
		}
	}

	ఒకవేళ (keylen > 0)	    // full matching terminal code
	{
#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_MENU)
	    ఒకవేళ (typebuf.tb_len >= 2
		    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
			      && typebuf.tb_buf[typebuf.tb_off + 1] == KS_MENU)
	    {
		పూర్ణాంకం	idx;

		// Using a menu may cause a break in undo!  It's like using
		// gotchars(), but without recording or writing to a script
		// file.
		may_sync_undo();
		del_typebuf(3, 0);
		idx = get_menu_సూచీ(current_menu, local_State);
		ఒకవేళ (idx != MENU_సూచీ_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    ఒకవేళ (VIsual_చురుకైన && VIsual_select
					&& (current_menu->modes & బహుళకం_VISUAL))
		    {
			VIsual_select = FALSE;
			(శూన్య)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->అక్షరశ్రేణులు[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#అయితే_ముగింపు // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    తిరిగి map_ఫలితం_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	ఒకవేళ (mp == ఖాళీ || keylen < 0)
	    keylen = KEYLEN_PART_కీలకం;
	లేకపోతే
	    keylen = mp_సరిపోలిక_len;
    }

    /*
     * complete match
     */
    ఒకవేళ (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#నిర్వచితమైతే FEAT_EVAL
	పూర్ణాంకం	save_m_సమాసం;
	పూర్ణాంకం	save_m_noremap;
	పూర్ణాంకం	save_m_silent;
	char_u	*save_m_కీలకాలు;
	char_u	*save_alt_m_కీలకాలు;
	పూర్ణాంకం	save_alt_m_keylen;
#లేకపోతే
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#అయితే_ముగింపు

	// write chars to script file(s)
	ఒకవేళ (keylen > typebuf.tb_maplen)
	    gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						   keylen - typebuf.tb_maplen);

	cmd_silent = (typebuf.tb_silent > 0);
	del_typebuf(keylen, 0);	// remove the mapped keys

	/*
	 * Put the replacement string in front of mapstr.
	 * The depth check catches ":map x y" and ":map y x".
	 */
	ఒకవేళ (++*mapdepth >= p_mmd)
	{
	    emsg(_(e_recursive_mapping));
	    ఒకవేళ (State & బహుళకం_CMDLINE)
		redrawcmdline();
	    లేకపోతే
		setcursor();
	    flush_buffers(FLUSH_కనీస);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    తిరిగి map_ఫలితం_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	ఒకవేళ (VIsual_చురుకైన && VIsual_select && (mp->m_బహుళకం & బహుళకం_VISUAL))
	{
	    VIsual_select = FALSE;
	    (శూన్య)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#నిర్వచితమైతే FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_సమాసం = mp->m_సమాసం;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_కీలకాలు = ఖాళీ;  // only saved when needed
	save_alt_m_కీలకాలు = ఖాళీ;  // only saved when needed
	save_alt_m_keylen = mp->m_alt != ఖాళీ ? mp->m_alt->m_keylen : 0;

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	ఒకవేళ (mp->m_సమాసం)
	{
	    పూర్ణాంకం save_vgetc_busy = vgetc_busy;
	    పూర్ణాంకం save_may_garbage_collect = may_garbage_collect;
	    పూర్ణాంకం was_screen_నిలువు = screen_cur_నిలువు;
	    పూర్ణాంకం was_screen_అడ్డువరుస = screen_cur_అడ్డువరుస;
	    పూర్ణాంకం prev_did_emsg = did_emsg;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_కీలకాలు = vim_strnsave(mp->m_కీలకాలు, (size_t)mp->m_keylen);
	    save_alt_m_కీలకాలు = mp->m_alt != ఖాళీ
				    ? vim_strnsave(mp->m_alt->m_కీలకాలు,
					     (size_t)save_alt_m_keylen) : ఖాళీ;
	    map_str = eval_map_సమాసం(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_అడ్డువరుస, was_screen_నిలువు);
	    బహిర్గతం_flush();

	    // If an error was displayed and the expression returns an empty
	    // string, generate a <Nop> to allow for a redraw.
	    ఒకవేళ (prev_did_emsg != did_emsg
				       && (map_str == ఖాళీ || *map_str == NUL))
	    {
		char_u	buf[4];

		vim_free(map_str);
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = KE_IGNORE;
		buf[3] = NUL;
		map_str = vim_strnsave(buf, 3);
		ఒకవేళ (State & బహుళకం_CMDLINE)
		{
		    // redraw the command below the error
		    సందేశం_didout = TRUE;
		    ఒకవేళ (సందేశం_అడ్డువరుస < cmdline_అడ్డువరుస)
			సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస;
		    redrawcmd();
		}
	    }

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	లేకపోతే
#అయితే_ముగింపు
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	ఒకవేళ (map_str == ఖాళీ)
	    i = FAIL;
	లేకపోతే
	{
	    పూర్ణాంకం noremap;

#నిర్వచితమైతే FEAT_EVAL
	    చివరి_వాడిన_map = mp;
	    చివరి_వాడిన_sid = -1;
#అయితే_ముగింపు
	    ఒకవేళ (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    లేకపోతే ఒకవేళ (
#నిర్వచితమైతే FEAT_EVAL
		save_m_సమాసం ?
		(save_m_కీలకాలు != ఖాళీ
			&& STRNCMP(map_str, save_m_కీలకాలు, (size_t)keylen) == 0)
		|| (save_alt_m_కీలకాలు != ఖాళీ
			&& STRNCMP(map_str, save_alt_m_కీలకాలు,
					    (size_t)save_alt_m_keylen) == 0) :
#అయితే_ముగింపు
		STRNCMP(map_str, mp->m_కీలకాలు, (size_t)keylen) == 0
		|| (mp->m_alt != ఖాళీ
			&& STRNCMP(map_str, mp->m_alt->m_కీలకాలు,
					    (size_t)mp->m_alt->m_keylen) == 0))
		noremap = REMAP_SKIP;
	    లేకపోతే
		noremap = REMAP_YES;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (save_m_సమాసం)
		vim_free(map_str);
#అయితే_ముగింపు
	}
#నిర్వచితమైతే FEAT_EVAL
	vim_free(save_m_కీలకాలు);
	vim_free(save_alt_m_కీలకాలు);
#అయితే_ముగింపు
	*keylenp = keylen;
	ఒకవేళ (i == FAIL)
	    తిరిగి map_ఫలితం_fail;
	తిరిగి map_ఫలితం_retry;
    }

    *keylenp = keylen;
    తిరిగి map_ఫలితం_nomatch;
}

/*
 * unget one character (can only be done once!)
 * If the character was stuffed, vgetc() will get it next time it is called.
 * Otherwise vgetc() will only get it when the stuff buffer is empty.
 */
    శూన్య
vungetc(పూర్ణాంకం c)
{
    old_char = c;
    old_mod_ముసుగు = mod_ముసుగు;
    old_mouse_అడ్డువరుస = mouse_అడ్డువరుస;
    old_mouse_నిలువు = mouse_నిలువు;
    old_కీలకం_Stuffed = కీలకం_Stuffed;
}

/*
 * When peeking and not getting a character, reg_executing cannot be cleared
 * yet, so set a flag to clear it later.
 */
    స్థిర శూన్య
check_చివర_reg_executing(పూర్ణాంకం advance)
{
    ఒకవేళ (reg_executing != 0 && (typebuf.tb_maplen == 0
						|| pending_చివర_reg_executing))
    {
	ఒకవేళ (advance)
	{
	    reg_executing = 0;
	    pending_చివర_reg_executing = FALSE;
	}
	లేకపోతే
	    pending_చివర_reg_executing = TRUE;
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
    స్థిర పూర్ణాంకం
vgetorpeek(పూర్ణాంకం advance)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		timedout = FALSE;	// waited for more than 'timeoutlen'
					// for mapping to complete or
					// 'ttimeoutlen' for complete key code
    పూర్ణాంకం		mapdepth = 0;		// check for recursive mapping
    పూర్ణాంకం		బహుళకం_deleted = FALSE;   // set when mode has been deleted
    పూర్ణాంకం		new_wcol, new_wrow;
#నిర్వచితమైతే FEAT_GUI
    పూర్ణాంకం		shape_మారింది = FALSE;  // adjusted cursor shape
#అయితే_ముగింపు
    పూర్ణాంకం		n;
    పూర్ణాంకం		old_wcol, old_wrow;
    పూర్ణాంకం		wait_tb_len;

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
    ఒకవేళ (vgetc_busy > 0 && ex_సాధారణ_busy == 0)
	తిరిగి NUL;

    ++vgetc_busy;

    ఒకవేళ (advance)
    {
	కీలకం_Stuffed = FALSE;
	typebuf_was_ఖాళీలు = FALSE;
    }

    init_typebuf();
    ప్రారంభం_stuff();
    check_చివర_reg_executing(advance);
    చేయి
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	ఒకవేళ (typeahead_char != 0)
	{
	    c = typeahead_char;
	    ఒకవేళ (advance)
		typeahead_char = 0;
	}
	లేకపోతే
	    c = read_readbuffers(advance);
	ఒకవేళ (c != NUL && !got_int)
	{
	    ఒకవేళ (advance)
	    {
		// KeyTyped = FALSE;  When the command that stuffed something
		// was typed, behave like the stuffed command was typed.
		// needed for CTRL-W CTRL-] to open a fold, for example.
		కీలకం_Stuffed = TRUE;
	    }
	    ఒకవేళ (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	// no abbreviations now
	}
	లేకపోతే
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    కొరకు (;;)
	    {
		దీర్ఘ	wait_సమయం;
		పూర్ణాంకం	keylen = 0;
		పూర్ణాంకం	showcmd_idx;
		check_చివర_reg_executing(advance);
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		ఒకవేళ (typebuf.tb_maplen)
		    పంక్తి_breakcheck();
		లేకపోతే
		    ui_breakcheck();		// check for CTRL-C
		ఒకవేళ (got_int)
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
		    ఒకవేళ ((c || typebuf.tb_maplen)
				     && (State & (బహుళకం_చొప్పించు | బహుళకం_CMDLINE)))
			c = ESC;
		    లేకపోతే
			c = Ctrl_C;
		    flush_buffers(FLUSH_ఆదానం);	// flush all typeahead

		    ఒకవేళ (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    ఆపు;
		}
		లేకపోతే ఒకవేళ (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_ఫలితం_T ఫలితం = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    ఒకవేళ (ఫలితం == map_ఫలితం_retry)
			// try mapping again
			కొనసాగు;
		    ఒకవేళ (ఫలితం == map_ఫలితం_fail)
		    {
			// failed, use the outer loop
			c = -1;
			ఆపు;
		    }
		    ఒకవేళ (ఫలితం == map_ఫలితం_get)
		    {
/*
 * get a character: 2. from the typeahead buffer
 */
			c = typebuf.tb_buf[typebuf.tb_off];
			ఒకవేళ (advance)	// remove chars from typebuf
			{
			    cmd_silent = (typebuf.tb_silent > 0);
			    ఒకవేళ (typebuf.tb_maplen > 0)
				కీలకం_Typed = FALSE;
			    లేకపోతే
			    {
				కీలకం_Typed = TRUE;
				// write char to script file(s)
				gotchars(typebuf.tb_buf
						 + typebuf.tb_off, 1);
			    }
			    కీలకం_Noremap = typebuf.tb_noremap[typebuf.tb_off]
							    & ~RM_SIMPLIFIED;
			    del_typebuf(1, 0);
			}
			ఆపు;  // got character, break the for loop
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
		ఒకవేళ (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& kitty_protocol_state != KKPS_ENABLED
			&& ex_సాధారణ_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & బహుళకం_చొప్పించు)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_కీలకం && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	నిలువు = 0;
		    char_u	*ptr;

		    ఒకవేళ (బహుళకం_displayed)
		    {
			unshowmode(TRUE);
			బహుళకం_deleted = TRUE;
		    }
#నిర్వచితమైతే FEAT_GUI
		    // may show a different cursor shape
		    ఒకవేళ (gui.లోపల_use && State != బహుళకం_సాధారణ && !cmd_silent)
		    {
			పూర్ణాంకం	    save_State;

			save_State = State;
			State = బహుళకం_సాధారణ;
			gui_update_cursor(TRUE, FALSE);
			State = save_State;
			shape_మారింది = TRUE;
		    }
#అయితే_ముగింపు
		    validate_cursor();
		    old_wcol = curwin->w_wcol;
		    old_wrow = curwin->w_wrow;

		    // move cursor left, if possible
		    ఒకవేళ (curwin->w_cursor.నిలువు != 0)
		    {
			ఒకవేళ (curwin->w_wcol > 0)
			{
			    // After auto-indenting and no text is following,
			    // we are expecting to truncate the trailing
			    // white-space, so find the last non-white
			    // character -- webb
			    ఒకవేళ (did_ai && *skipwhite(ml_get_curline()
						+ curwin->w_cursor.నిలువు) == NUL)
			    {
				chartabsize_T cts;

				curwin->w_wcol = 0;
				ptr = ml_get_curline();
				init_chartabsize_arg(&cts, curwin,
					  curwin->w_cursor.lnum, 0, ptr, ptr);
				అయితే (cts.cts_ptr < ptr + curwin->w_cursor.నిలువు)
				{
				    ఒకవేళ (!VIM_ISWHITE(*cts.cts_ptr))
					curwin->w_wcol = cts.cts_vcol;
				    cts.cts_vcol += lbr_chartabsize(&cts);
				    ఒకవేళ (has_mbyte)
					cts.cts_ptr +=
						   (*mb_ptr2len)(cts.cts_ptr);
				    లేకపోతే
					++cts.cts_ptr;
				}
				clear_chartabsize_arg(&cts);

				curwin->w_wrow = curwin->w_cline_అడ్డువరుస
					   + curwin->w_wcol / curwin->w_వెడల్పు;
				curwin->w_wcol %= curwin->w_వెడల్పు;
				curwin->w_wcol += curwin_నిలువు_off();
				నిలువు = 0;	// no correction needed
			    }
			    లేకపోతే
			    {
				--curwin->w_wcol;
				నిలువు = curwin->w_cursor.నిలువు - 1;
			    }
			}
			లేకపోతే ఒకవేళ (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_వెడల్పు - 1;
			    నిలువు = curwin->w_cursor.నిలువు - 1;
			}
			ఒకవేళ (has_mbyte && నిలువు > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    నిలువు -= (*mb_తల_off)(ptr, ptr + నిలువు);
			    ఒకవేళ ((*mb_ptr2cells)(ptr + నిలువు) > 1)
				--curwin->w_wcol;
			}
		    }
		    setcursor();
		    బహిర్గతం_flush();
		    new_wcol = curwin->w_wcol;
		    new_wrow = curwin->w_wrow;
		    curwin->w_wcol = old_wcol;
		    curwin->w_wrow = old_wrow;
		}
		ఒకవేళ (c < 0)
		    కొనసాగు;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		కొరకు (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		ఒకవేళ (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    కొనసాగు;
		}

		ఒకవేళ (ex_సాధారణ_busy > 0)
		{
		    స్థిర పూర్ణాంకం tc = 0;

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    ఒకవేళ (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			కొనసాగు;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    ఒకవేళ (p_im && (State & బహుళకం_చొప్పించు))
			c = Ctrl_L;
#నిర్వచితమైతే FEAT_TERMINAL
		    లేకపోతే ఒకవేళ (terminal_is_చురుకైన())
			c = K_CANCEL;
#అయితే_ముగింపు
		    లేకపోతే ఒకవేళ ((State & బహుళకం_CMDLINE)
					     || (cmdwin_type > 0 && tc == ESC))
			c = Ctrl_C;
		    లేకపోతే
			c = ESC;
		    tc = c;
		    // set a flag to indicate this wasn't a normal char
		    ఒకవేళ (advance)
			typebuf_was_ఖాళీలు = TRUE;

		    // return from main_loop()
		    ఒకవేళ (pending_exmode_చురుకైన)
			exmode_చురుకైన = EXMODE_సాధారణ;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    ఆపు;
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
		ఒకవేళ (((State & బహుళకం_చొప్పించు) != 0 || p_lz)
			&& (State & బహుళకం_CMDLINE) == 0
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
		పూర్ణాంకం showing_partial = FALSE;
		ఒకవేళ (typebuf.tb_len > 0 && advance && !exmode_చురుకైన)
		{
		    ఒకవేళ (((State & (బహుళకం_సాధారణ | బహుళకం_చొప్పించు))
						      || State == బహుళకం_LANGMAP)
			    && State != బహుళకం_HITRETURN)
		    {
			// this looks nice when typing a dead character map
			ఒకవేళ (State & బహుళకం_చొప్పించు
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
			నెట్టు_showcmd();
			ఒకవేళ (typebuf.tb_len > SHOWCMD_నిలువుల_సంఖ్య)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_నిలువుల_సంఖ్య;
			అయితే (showcmd_idx < typebuf.tb_len)
			    కూడు_byte_గమ్యం_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
		    }

		    // This looks nice when typing a dead character map.
		    // There is no actual command line for get_number().
		    ఒకవేళ ((State & బహుళకం_CMDLINE)
			    && get_cmdline_info()->cmdbuff != ఖాళీ
#ఒకవేళ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
			    && cmdline_నక్షత్రం == 0
#అయితే_ముగింపు
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
		ఒకవేళ (typebuf.tb_len == 0)
		    // timedout may have been set if a mapping with empty RHS
		    // fully matched while longer mappings timed out.
		    timedout = FALSE;

		ఒకవేళ (advance)
		{
		    ఒకవేళ (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_కీలకం)))
			// blocking wait
			wait_సమయం = -1L;
		    లేకపోతే ఒకవేళ (keylen == KEYLEN_PART_కీలకం && p_ttm >= 0)
			wait_సమయం = p_ttm;
		    లేకపోతే
			wait_సమయం = p_tm;
		}
		లేకపోతే
		    wait_సమయం = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_సమయం);

		ఒకవేళ (showcmd_idx != 0)
		    తీయి_showcmd();
		ఒకవేళ (showing_partial)
		{
		    ఒకవేళ (State & బహుళకం_చొప్పించు)
			edit_unputchar();
		    ఒకవేళ ((State & బహుళకం_CMDLINE)
					&& get_cmdline_info()->cmdbuff != ఖాళీ)
			unputcmdline();
		    లేకపోతే
			setcursor();	// put cursor back where it belongs
		}

		ఒకవేళ (c < 0)
		    కొనసాగు;		// end of input script reached
		ఒకవేళ (c == NUL)		// no character available
		{
		    ఒకవేళ (!advance)
			ఆపు;
		    ఒకవేళ (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			కొనసాగు;
		    }
		}
		లేకపోతే
		{	    // allow mapping for just typed characters
		    అయితే (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_చురుకైన = im_get_స్థితి();
#అయితే_ముగింపు
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } అయితే ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    ఒకవేళ (advance && p_smd && సందేశం_silent == 0 && (State & బహుళకం_చొప్పించు))
    {
	ఒకవేళ (c == ESC && !బహుళకం_deleted && !no_mapping && బహుళకం_displayed)
	{
	    ఒకవేళ (typebuf.tb_len && !కీలకం_Typed)
		redraw_cmdline = TRUE;	    // delete mode later
	    లేకపోతే
		unshowmode(FALSE);
	}
	లేకపోతే ఒకవేళ (c != ESC && బహుళకం_deleted)
	{
	    ఒకవేళ (typebuf.tb_len && !కీలకం_Typed)
		redraw_cmdline = TRUE;	    // show mode later
	    లేకపోతే
		showmode();
	}
    }
#నిర్వచితమైతే FEAT_GUI
    // may unshow different cursor shape
    ఒకవేళ (gui.లోపల_use && shape_మారింది)
	gui_update_cursor(TRUE, FALSE);
#అయితే_ముగింపు
    ఒకవేళ (timedout && c == ESC)
    {
	// When recording there will be no timeout.  Add an <Ignore> after the
	// ESC to avoid that it forms a key code with following characters.
	gotchars_ignore();
    }

    --vgetc_busy;

    తిరిగి c;
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
    స్థిర పూర్ణాంకం
inchar(
    char_u	*buf,
    పూర్ణాంకం		maxlen,
    దీర్ఘ	wait_సమయం)	    // milliseconds
{
    పూర్ణాంకం		len = 0;	    // init for GCC
    పూర్ణాంకం		retesc = FALSE;	    // return ESC with gotint
    పూర్ణాంకం		script_char;
    పూర్ణాంకం		tb_change_cnt = typebuf.tb_change_cnt;

    ఒకవేళ (wait_సమయం == -1L || wait_సమయం > 100L)  // flush output before waiting
    {
	cursor_on();
	బహిర్గతం_flush_cursor(FALSE, FALSE);
#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	ఒకవేళ (gui.లోపల_use && postponed_mouseshape)
	    update_mouseshape(-1);
#అయితే_ముగింపు
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    ఒకవేళ (State != బహుళకం_HITRETURN)
    {
	did_outofmem_సందేశం = FALSE;   // display out of memory message (again)
	did_swapwrite_సందేశం = FALSE;  // display swap file write error again
    }
    undo_off = FALSE;		    // restart undo now

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    అయితే (scriptin[curscript] != ఖాళీ && script_char < 0
#నిర్వచితమైతే FEAT_EVAL
	    && !ignore_script
#అయితే_ముగింపు
	    )
    {
#నిర్వచితమైతే సందేశం_వరుస
	parse_queued_messages();
#అయితే_ముగింపు

	ఒకవేళ (got_int || (script_char = getc(scriptin[curscript])) < 0)
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
	    ఒకవేళ (got_int)
		retesc = TRUE;
	    లేకపోతే
		తిరిగి -1;
	}
	లేకపోతే
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    ఒకవేళ (script_char < 0)	// did not get a character from script
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	ఒకవేళ (got_int)
	{
#నిర్వచించు DUM_LEN (MAXMAPLEN * 3 + 3)
	    char_u	dum[DUM_LEN + 1];

	    కొరకు (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		ఒకవేళ (len == 0 || (len == 1 && dum[0] == Ctrl_C))
		    ఆపు;
	    }
	    తిరిగి retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	ఒకవేళ (wait_సమయం == -1L || wait_సమయం > 10L)
	    బహిర్గతం_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_సమయం, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    ఒకవేళ (typebuf_మారింది(tb_change_cnt))
	తిరిగి 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    ఒకవేళ (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    తిరిగి fix_ఆదానం_బఫరు(buf, len);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * "buf[]" must have room to triple the number of bytes!
 * Returns the new length.
 */
    పూర్ణాంకం
fix_ఆదానం_బఫరు(char_u *buf, పూర్ణాంకం len)
{
    పూర్ణాంకం		i;
    char_u	*p = buf;

    /*
     * Two characters are special: NUL and K_SPECIAL.
     * When compiled With the GUI CSI is also special.
     * Replace	     NUL by K_SPECIAL KS_ZERO	 KE_FILLER
     * Replace K_SPECIAL by K_SPECIAL KS_SPECIAL KE_FILLER
     * Replace       CSI by K_SPECIAL KS_EXTRA   KE_CSI
     */
    కొరకు (i = len; --i >= 0; ++p)
    {
#నిర్వచితమైతే FEAT_GUI
	// When the GUI is used any character can come after a CSI, don't
	// escape it.
	ఒకవేళ (gui.లోపల_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
# ifndef MSWIN
	// When not on MS-Windows and the GUI is not used CSI needs to be
	// escaped.
	లేకపోతే ఒకవేళ (!gui.లోపల_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (పూర్ణాంకం)KE_CSI;
	    len += 2;
	}
# endif
	లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (p[0] == NUL || (p[0] == K_SPECIAL
		    // timeout may generate K_CURSORHOLD,
		    // 'autocompletedelay' timeout K_COMPLETE_DELAY
		    && (i < 2 || p[1] != KS_EXTRA
			|| (p[2] != (పూర్ణాంకం)KE_CURSORHOLD
			    && p[2] != (పూర్ణాంకం)KE_COMPLETE_DELAY))
#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
		    // Win32 console passes modifiers
		    && (
# ifdef VIMDLL
			gui.లోపల_use ||
# endif
			(i < 2 || p[1] != KS_MODIFIER))
#అయితే_ముగింపు
		    ))
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    p[2] = K_మూడవ(p[0]);
	    p[1] = K_రెండవ(p[0]);
	    p[0] = K_SPECIAL;
	    p += 2;
	    len += 2;
	}
    }
    *p = NUL;		// add trailing NUL
    తిరిగి len;
}

#ఒకవేళ defined(USE_ఆదానం_BUF)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    పూర్ణాంకం
ఆదానం_అందుబాటు(శూన్య)
{
    తిరిగి (!vim_is_ఆదానం_buf_ఖాళీలు()
# ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#అయితే_ముగింపు

/*
 * Function passed to do_cmdline() to get the command after a <Cmd> key from
 * typeahead.
 */
    స్థిర char_u *
getcmdkeycmd(
	పూర్ణాంకం		promptc వాడని,
	శూన్య		*cookie వాడని,
	పూర్ణాంకం		indent వాడని,
	getline_opt_T	do_concat వాడని)
{
    garray_T	పంక్తి_ga;
    పూర్ణాంకం		c1 = -1;
    పూర్ణాంకం		c2;
    పూర్ణాంకం		cmod = 0;
    పూర్ణాంకం		aborted = FALSE;

    ga_init2(&పంక్తి_ga, 1, 32);

    // no mapping for these characters
    no_mapping++;

    got_int = FALSE;
    అయితే (c1 != NUL && !aborted)
    {
	ఒకవేళ (ga_grow(&పంక్తి_ga, 32) == FAIL)
	{
	    aborted = TRUE;
	    ఆపు;
	}

	ఒకవేళ (vgetorpeek(FALSE) == NUL)
	{
	    // incomplete <Cmd> is an error, because there is not much the user
	    // could do in this state.
	    emsg(_(e_cmd_mapping_must_చివర_with_cr));
	    aborted = TRUE;
	    ఆపు;
	}

	// Get one character at a time.
	c1 = vgetorpeek(TRUE);

	// Get two extra bytes for special keys
	ఒకవేళ (c1 == K_SPECIAL)
	{
	    c1 = vgetorpeek(TRUE);
	    c2 = vgetorpeek(TRUE);
	    ఒకవేళ (c1 == KS_MODIFIER)
	    {
		cmod = c2;
		కొనసాగు;
	    }
	    c1 = గమ్యం_SPECIAL(c1, c2);

	    // K_ESC is used to avoid ambiguity with the single Esc character
	    // that might be the start of an escape sequence.  Convert it back
	    // to a single Esc here.
	    ఒకవేళ (c1 == K_ESC)
		c1 = ESC;

#నిర్వచితమైతే FEAT_GUI
	    // Translate K_CSI to CSI.  The special key is only used to
	    // avoid it being recognized as the start of a special key.
	    ఒకవేళ (c1 == K_CSI)
		c1 = CSI;
#అయితే_ముగింపు
	}

	ఒకవేళ (got_int)
	    aborted = TRUE;
	లేకపోతే ఒకవేళ (c1 == '\r' || c1 == '\n')
	    c1 = NUL;  // end the line
	లేకపోతే ఒకవేళ (c1 == ESC)
	    aborted = TRUE;
	లేకపోతే ఒకవేళ (c1 == K_COMMAND || c1 == K_SCRIPT_COMMAND)
	{
	    // give a nicer error message for this special case
	    emsg(_(e_cmd_mapping_must_చివర_with_cr_before_రెండవ_cmd));
	    aborted = TRUE;
	}
	లేకపోతే ఒకవేళ (c1 == K_SNR)
	{
	    GA_CONCAT_LITERAL(&పంక్తి_ga, "<SNR>");
	}
	లేకపోతే
	{
	    ఒకవేళ (cmod != 0)
	    {
		ga_append(&పంక్తి_ga, K_SPECIAL);
		ga_append(&పంక్తి_ga, KS_MODIFIER);
		ga_append(&పంక్తి_ga, cmod);
	    }
	    ఒకవేళ (IS_SPECIAL(c1))
	    {
		ga_append(&పంక్తి_ga, K_SPECIAL);
		ga_append(&పంక్తి_ga, K_రెండవ(c1));
		ga_append(&పంక్తి_ga, K_మూడవ(c1));
	    }
	    లేకపోతే
		ga_append(&పంక్తి_ga, c1);
	}

	cmod = 0;
    }

    no_mapping--;

    ఒకవేళ (aborted)
	ga_clear(&పంక్తి_ga);

    తిరిగి (char_u *)పంక్తి_ga.ga_దత్తాంశం;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * If there was a mapping we get its SID.  Otherwise, use "last_used_sid", it
 * is set when redo'ing.
 * Put this SID in the redo buffer, so that "." will use the same script
 * context.
 */
    శూన్య
may_కూడు_చివరి_వాడిన_map_గమ్యం_redobuff(శూన్య)
{
    char_u  buf[3 + 20];
    పూర్ణాంకం	    buflen;
    పూర్ణాంకం	    sid = -1;

    ఒకవేళ (చివరి_వాడిన_map != ఖాళీ)
	sid = చివరి_వాడిన_map->m_script_ctx.sc_sid;
    ఒకవేళ (sid < 0)
	sid = చివరి_వాడిన_sid;

    ఒకవేళ (sid < 0)
	తిరిగి;

    // <K_SID>{nr};
    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = KE_SID;
    buflen = 3;

    buflen += vim_snprintf((అక్షరం *)buf + 3, 20, "%d;", sid);
    కూడు_buff(&redobuff, buf, (దీర్ఘ)buflen);
}
#అయితే_ముగింపు

    పూర్ణాంకం
do_cmdkey_command(పూర్ణాంకం కీలకం వాడని, పూర్ణాంకం flags)
{
    పూర్ణాంకం	    res;
#నిర్వచితమైతే FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    ఒకవేళ (కీలకం == K_SCRIPT_COMMAND
		  && (చివరి_వాడిన_map != ఖాళీ || SCRIPT_ID_VALID(చివరి_వాడిన_sid)))
    {
	save_current_sctx = current_sctx;
	ఒకవేళ (చివరి_వాడిన_map != ఖాళీ)
	    current_sctx = చివరి_వాడిన_map->m_script_ctx;
	లేకపోతే
	{
	    current_sctx.sc_sid = చివరి_వాడిన_sid;
	    current_sctx.sc_lnum = 0;
	    current_sctx.sc_సంస్కరణ = SCRIPT_అంశం(చివరి_వాడిన_sid)->sn_సంస్కరణ;
	}
    }
#అయితే_ముగింపు

    res = do_cmdline(ఖాళీ, getcmdkeycmd, ఖాళీ, flags);

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#అయితే_ముగింపు

    తిరిగి res;
}

#ఒకవేళ defined(FEAT_EVAL)
    శూన్య
reset_చివరి_వాడిన_map(mapblock_T *mp)
{
    ఒకవేళ (చివరి_వాడిన_map != mp)
	తిరిగి;

    చివరి_వాడిన_map = ఖాళీ;
    చివరి_వాడిన_sid = -1;
}
#అయితే_ముగింపు
