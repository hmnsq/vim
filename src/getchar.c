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

#ಅನ್ತರ್ಭಾವ "vim.h"

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

#ವ್ಯಾಖ್ಯಾನಿಸು ಕನಿಷ್ಠತಮ_ಗಾತ್ರ 20			// minimal size for b_str

ಸ್ಥಿರ buffheader_T redobuff = {{ಖಾಲಿ, 0, {NUL}}, ಖಾಲಿ, 0, 0, FALSE};
ಸ್ಥಿರ buffheader_T old_redobuff = {{ಖಾಲಿ, 0, {NUL}}, ಖಾಲಿ, 0, 0, FALSE};
ಸ್ಥಿರ buffheader_T recordbuff = {{ಖಾಲಿ, 0, {NUL}}, ಖಾಲಿ, 0, 0, FALSE};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ typeahead_char = 0;		// typeahead char that's not flushed

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ char_u typedchars[MAXMAPLEN + 1] = { NUL };  // typed chars before map
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ typedchars_pos = 0;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * When block_redo is TRUE the redo buffer will not be changed.
 * Used by edit() to repeat insertions.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಖಂಡ_redo = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಕೀಲಿ_Noremap = 0;	    // remapping flags

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
#ವ್ಯಾಖ್ಯಾನಿಸು RM_YES		0	// tb_noremap: remap
#ವ್ಯಾಖ್ಯಾನಿಸು RM_NONE		1	// tb_noremap: don't remap
#ವ್ಯಾಖ್ಯಾನಿಸು RM_SCRIPT	2	// tb_noremap: remap local script mappings
#ವ್ಯಾಖ್ಯಾನಿಸು RM_ABBR		4	// tb_noremap: don't remap, do abbrev.
#ವ್ಯಾಖ್ಯಾನಿಸು RM_SIMPLIFIED	8	// tb_noremap: modifiers merged into the key

// typebuf.tb_buf has three parts: room in front (for result of mappings), the
// middle for typeahead and room for new characters (which needs to be 3 *
// MAXMAPLEN for the Amiga).
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
// Add extra space to handle large OSC responses in bigger chunks (improve
// performance)
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3) + 2048)
#ಇಲ್ಲದಿದ್ದರೆ // Tiny version
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ char_u	typebuf_init[TYPELEN_INIT];	// initial typebuf.tb_buf
ಸ್ಥಿರ char_u	noremapbuf_init[TYPELEN_INIT];	// initial typebuf.tb_noremap

ಸ್ಥಿರ size_t	ಕೊನೆಯ_recorded_len = 0;	// number of last recorded chars

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
mapblock_T	*ಕೊನೆಯ_ಬಳಸಿದ_map = ಖಾಲಿ;
ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_ಬಳಸಿದ_sid = -1;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	read_readbuf(buffheader_T *buf, ಪೂರ್ಣಾಂಕ advance);
ಸ್ಥಿರ ಶೂನ್ಯ	init_typebuf(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	may_sync_undo(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	free_typebuf(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	closescript(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	updatescript(ಪೂರ್ಣಾಂಕ c);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	vgetorpeek(ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	inchar(char_u *buf, ಪೂರ್ಣಾಂಕ maxlen, ದೀರ್ಘ wait_ಸಮಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	do_ಕೀಲಿ_ಒಳಹರಿವು_pre(ಪೂರ್ಣಾಂಕ c);
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Free and clear a buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    ಗಾಗಿ (p = buf->bh_ಮೊದಲ.b_ಮುಂದಿನ; p != ಖಾಲಿ; p = np)
    {
	np = p->b_ಮುಂದಿನ;
	vim_free(p);
    }
    buf->bh_ಮೊದಲ.b_ಮುಂದಿನ = ಖಾಲಿ;
    buf->bh_curr = ಖಾಲಿ;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    ಸ್ಥಿರ char_u *
get_buffcont(
    buffheader_T	*ಬಫರ್,
    ಪೂರ್ಣಾಂಕ			dozero,	    // count == zero is not an error
    size_t		*len)	    // the length of the returned buffer
{
    long_u	    ಎಣಿಕೆ_2 = 0;
    char_u	    *p = ಖಾಲಿ;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;
    size_t	    i = 0;

    // compute the total length of the string
    ಗಾಗಿ (bp = ಬಫರ್->bh_ಮೊದಲ.b_ಮುಂದಿನ; bp != ಖಾಲಿ; bp = bp->b_ಮುಂದಿನ)
	ಎಣಿಕೆ_2 += (long_u)bp->b_strlen;

    ಆದರೆ ((ಎಣಿಕೆ_2 > 0 || dozero) && (p = alloc(ಎಣಿಕೆ_2 + 1)) != ಖಾಲಿ)
    {
	p2 = p;
	ಗಾಗಿ (bp = ಬಫರ್->bh_ಮೊದಲ.b_ಮುಂದಿನ; bp != ಖಾಲಿ; bp = bp->b_ಮುಂದಿನ)
	    ಗಾಗಿ (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
	i = (size_t)(p2 - p);
    }

    ಆದರೆ (len != ಖಾಲಿ)
	*len = i;

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(ಶೂನ್ಯ)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE, &len);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    ಆದರೆ (len >= ಕೊನೆಯ_recorded_len)
    {
	len -= ಕೊನೆಯ_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    ಆದರೆ (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    ಹಿಂತಿರುಗಿ (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    string_T
get_inserted(ಶೂನ್ಯ)
{
    size_t len = 0;
    char_u *str = get_buffcont(&redobuff, FALSE, &len);
    string_T ret = { str, len };
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_buff(
    buffheader_T	*buf,
    char_u		*s,
    ದೀರ್ಘ		slen)	// length of "s" or -1
{
    ಆದರೆ (slen < 0)
	slen = (ದೀರ್ಘ)STRLEN(s);
    ಆದರೆ (slen == 0)				// don't add empty strings
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (buf->bh_ಮೊದಲ.b_ಮುಂದಿನ == ಖಾಲಿ)	// first add to list
    {
	buf->bh_curr = &(buf->bh_ಮೊದಲ);
	buf->bh_create_newblock = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->bh_curr == ಖಾಲಿ)	// buffer has already been read
    {
	iemsg(e_ಕೂಡಿಸು_ಗಮ್ಯ_ಆಂತರಿಕ_ಬಫರ್_that_was_already_read_from);
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->bh_ಸೂಚ್ಯಂಕ != 0)
    {
	mch_memmove(buf->bh_ಮೊದಲ.b_ಮುಂದಿನ->b_str,
		    buf->bh_ಮೊದಲ.b_ಮುಂದಿನ->b_str + buf->bh_ಸೂಚ್ಯಂಕ,
		    (buf->bh_ಮೊದಲ.b_ಮುಂದಿನ->b_strlen - buf->bh_ಸೂಚ್ಯಂಕ) + 1);
	buf->bh_ಮೊದಲ.b_ಮುಂದಿನ->b_strlen -= buf->bh_ಸೂಚ್ಯಂಕ;
	buf->bh_space += buf->bh_ಸೂಚ್ಯಂಕ;
    }
    buf->bh_ಸೂಚ್ಯಂಕ = 0;

    ಆದರೆ (!buf->bh_create_newblock && buf->bh_space >= (ಪೂರ್ಣಾಂಕ)slen)
    {
	vim_strncpy(buf->bh_curr->b_str + buf->bh_curr->b_strlen, s, (size_t)slen);
	buf->bh_curr->b_strlen += slen;
	buf->bh_space -= slen;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	long_u	    len;
	buffblock_T *p;

	ಆದರೆ (slen < ಕನಿಷ್ಠತಮ_ಗಾತ್ರ)
	    len = ಕನಿಷ್ಠತಮ_ಗಾತ್ರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ; // no space, just forget it
	vim_strncpy(p->b_str, s, (size_t)slen);
	p->b_strlen = slen;
	buf->bh_space = (ಪೂರ್ಣಾಂಕ)(len - slen);
	buf->bh_create_newblock = FALSE;

	p->b_ಮುಂದಿನ = buf->bh_curr->b_ಮುಂದಿನ;
	buf->bh_curr->b_ಮುಂದಿನ = p;
	buf->bh_curr = p;
    }
}

/*
 * Delete "slen" bytes from the end of "buf".
 * Only works when it was just added.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
delete_buff_ಬಾಲ(buffheader_T *buf, ಪೂರ್ಣಾಂಕ slen)
{
    ಆದರೆ (buf->bh_curr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // nothing to delete
    ಆದರೆ (buf->bh_curr->b_strlen < (size_t)slen)
	ಹಿಂತಿರುಗಿ;

    buf->bh_curr->b_str[buf->bh_curr->b_strlen - (size_t)slen] = NUL;
    buf->bh_curr->b_strlen -= slen;
    buf->bh_space += slen;
}

/*
 * Add number "n" to buffer "buf".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_ಅಂಕೆ_buff(buffheader_T *buf, ದೀರ್ಘ n)
{
    char_u	ಸಂಖ್ಯೆ[32];
    ಪೂರ್ಣಾಂಕ		numberlen;

    numberlen = vim_snprintf((ಅಕ್ಷರ *)ಸಂಖ್ಯೆ, ಗಾತ್ರ(ಸಂಖ್ಯೆ), "%ld", n);
    ಕೂಡಿಸು_buff(buf, ಸಂಖ್ಯೆ, (ದೀರ್ಘ)numberlen);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_char_buff(buffheader_T *buf, ಪೂರ್ಣಾಂಕ c)
{
    char_u	ಬೈಟುಗಳು[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	ತಾತ್ಕಾಲಿಕ[4];
    ದೀರ್ಘ	templen;

    ಆದರೆ (IS_SPECIAL(c))
	len = 1;
    ಇಲ್ಲದಿದ್ದರೆ
	len = (*mb_char2bytes)(c, ಬೈಟುಗಳು);
    ಗಾಗಿ (i = 0; i < len; ++i)
    {
	ಆದರೆ (!IS_SPECIAL(c))
	    c = ಬೈಟುಗಳು[i];

	ಆದರೆ (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    ತಾತ್ಕಾಲಿಕ[0] = K_SPECIAL;
	    ತಾತ್ಕಾಲಿಕ[1] = K_ಎರಡನೇ(c);
	    ತಾತ್ಕಾಲಿಕ[2] = K_ಮೂರನೇ(c);
	    ತಾತ್ಕಾಲಿಕ[3] = NUL;
	    templen = 3;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    ತಾತ್ಕಾಲಿಕ[0] = CSI;
	    ತಾತ್ಕಾಲಿಕ[1] = KS_EXTRA;
	    ತಾತ್ಕಾಲಿಕ[2] = (ಪೂರ್ಣಾಂಕ)KE_CSI;
	    ತಾತ್ಕಾಲಿಕ[3] = NUL;
	    templen = 3;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ತಾತ್ಕಾಲಿಕ[0] = c;
	    ತಾತ್ಕಾಲಿಕ[1] = NUL;
	    templen = 1;
	}
	ಕೂಡಿಸು_buff(buf, ತಾತ್ಕಾಲಿಕ, templen);
    }
}

// First read ahead buffer. Used for translated commands.
ಸ್ಥಿರ buffheader_T readbuf1 = {{ಖಾಲಿ, 0, {NUL}}, ಖಾಲಿ, 0, 0, FALSE};

// Second read ahead buffer. Used for redo.
ಸ್ಥಿರ buffheader_T readbuf2 = {{ಖಾಲಿ, 0, {NUL}}, ಖಾಲಿ, 0, 0, FALSE};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_readbuffers(ಪೂರ್ಣಾಂಕ advance)
{
    ಪೂರ್ಣಾಂಕ c;

    c = read_readbuf(&readbuf1, advance);
    ಆದರೆ (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    ಹಿಂತಿರುಗಿ c;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_readbuf(buffheader_T *buf, ಪೂರ್ಣಾಂಕ advance)
{
    char_u	c;
    buffblock_T	*curr;

    ಆದರೆ (buf->bh_ಮೊದಲ.b_ಮುಂದಿನ == ಖಾಲಿ)  // buffer is empty
	ಹಿಂತಿರುಗಿ NUL;

    curr = buf->bh_ಮೊದಲ.b_ಮುಂದಿನ;
    c = curr->b_str[buf->bh_ಸೂಚ್ಯಂಕ];

    ಆದರೆ (advance)
    {
	ಆದರೆ (curr->b_str[++buf->bh_ಸೂಚ್ಯಂಕ] == NUL)
	{
	    buf->bh_ಮೊದಲ.b_ಮುಂದಿನ = curr->b_ಮುಂದಿನ;
	    vim_free(curr);
	    buf->bh_ಸೂಚ್ಯಂಕ = 0;
	}
    }
    ಹಿಂತಿರುಗಿ c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಆರಂಭ_stuff(ಶೂನ್ಯ)
{
    ಆದರೆ (readbuf1.bh_ಮೊದಲ.b_ಮುಂದಿನ != ಖಾಲಿ)
    {
	readbuf1.bh_curr = &(readbuf1.bh_ಮೊದಲ);
	readbuf1.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
    ಆದರೆ (readbuf2.bh_ಮೊದಲ.b_ಮುಂದಿನ != ಖಾಲಿ)
    {
	readbuf2.bh_curr = &(readbuf2.bh_ಮೊದಲ);
	readbuf2.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    ಪೂರ್ಣಾಂಕ
stuff_ಖಾಲಿಗಳು(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (readbuf1.bh_ಮೊದಲ.b_ಮುಂದಿನ == ಖಾಲಿ
	 && readbuf2.bh_ಮೊದಲ.b_ಮುಂದಿನ == ಖಾಲಿ);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * readbuf2.
 */
    ಪೂರ್ಣಾಂಕ
readbuf1_ಖಾಲಿಗಳು(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (readbuf1.bh_ಮೊದಲ.b_ಮುಂದಿನ == ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set a typeahead character that won't be flushed.
 */
    ಶೂನ್ಯ
typeahead_noflush(ಪೂರ್ಣಾಂಕ c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    ಶೂನ್ಯ
flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    ಆರಂಭ_stuff();
    ಆಗ (read_readbuffers(TRUE) != NUL)
	;

    ಆದರೆ (flush_typeahead == FLUSH_ಕನಿಷ್ಠತಮ)
    {
	// remove mapped characters at the start only,
	// but only when enough space left in typebuf
	ಆದರೆ (typebuf.tb_off + typebuf.tb_maplen >= typebuf.tb_buflen)
	{
	    typebuf.tb_off = MAXMAPLEN;
	    typebuf.tb_len = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    typebuf.tb_off += typebuf.tb_maplen;
	    typebuf.tb_len -= typebuf.tb_maplen;
	}
#ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	ಆದರೆ (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// remove typeahead
	ಆದರೆ (flush_typeahead == FLUSH_ಒಳಹರಿವು)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    ಆಗ (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
    ಆದರೆ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * The previous contents of the redo buffer is kept in old_redobuffer.
 * This is used for the CTRL-O <.> command in insert mode.
 */
    ಶೂನ್ಯ
ResetRedobuff(ಶೂನ್ಯ)
{
    ಆದರೆ (ಖಂಡ_redo)
	ಹಿಂತಿರುಗಿ;

    free_buff(&old_redobuff);
    old_redobuff = redobuff;
    redobuff.bh_ಮೊದಲ.b_ಮುಂದಿನ = ಖಾಲಿ;
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    ಶೂನ್ಯ
CancelRedo(ಶೂನ್ಯ)
{
    ಆದರೆ (ಖಂಡ_redo)
	ಹಿಂತಿರುಗಿ;

    free_buff(&redobuff);
    redobuff = old_redobuff;
    old_redobuff.bh_ಮೊದಲ.b_ಮುಂದಿನ = ಖಾಲಿ;
    ಆರಂಭ_stuff();
    ಆಗ (read_readbuffers(TRUE) != NUL)
	;
}

/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
    ಶೂನ್ಯ
saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;
    size_t	slen;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_ಮೊದಲ.b_ಮುಂದಿನ = ಖಾಲಿ;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_ಮೊದಲ.b_ಮುಂದಿನ = ಖಾಲಿ;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE, &slen);
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಕೂಡಿಸು_buff(&redobuff, s, (ದೀರ್ಘ)slen);
    vim_free(s);
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
Append_ಗಮ್ಯ_Redobuff(char_u *s)
{
    ಆದರೆ (!ಖಂಡ_redo)
	ಕೂಡಿಸು_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    ಶೂನ್ಯ
Append_ಗಮ್ಯ_Redobuff_Lit(
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*ಆರಂಭ;

    ಆದರೆ (ಖಂಡ_redo)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	ಆರಂಭ = s;
	ಆಗ (*s >= ' ' && *s < DEL && (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	ಆದರೆ (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	ಆದರೆ (s > ಆರಂಭ)
	    ಕೂಡಿಸು_buff(&redobuff, ಆರಂಭ, (ದೀರ್ಘ)(s - ಆರಂಭ));

	ಆದರೆ (*s == NUL || (len >= 0 && s - str >= len))
	    ನಿಲ್ಲಿಸು;

	// Handle a special or multibyte character.
	ಆದರೆ (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	ಇಲ್ಲದಿದ್ದರೆ
	    c = *s++;
	ಆದರೆ (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    ಕೂಡಿಸು_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048
	ಆದರೆ (*s == NUL && c == '0')
	    ಕೂಡಿಸು_buff(&redobuff, (char_u *)"048", 3L);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೂಡಿಸು_char_buff(&redobuff, c);
    }
}

/*
 * Append "s" to the redo buffer, leaving 3-byte special key codes unmodified
 * and escaping other K_SPECIAL and CSI bytes.
 */
    ಶೂನ್ಯ
Append_ಗಮ್ಯ_Redobuff_Spec(char_u *s)
{
    ಆದರೆ (ಖಂಡ_redo)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (*s != NUL)
    {
	ಆದರೆ (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    ಕೂಡಿಸು_buff(&redobuff, s, 3L);
	    s += 3;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೂಡಿಸು_char_buff(&redobuff, mb_cptr2char_adv(&s));
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ಶೂನ್ಯ
Append_Char_ಗಮ್ಯ_Redobuff(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (!ಖಂಡ_redo)
	ಕೂಡಿಸು_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    ಶೂನ್ಯ
Append_ಸಂಖ್ಯೆ_ಗಮ್ಯ_Redobuff(ದೀರ್ಘ n)
{
    ಆದರೆ (!ಖಂಡ_redo)
	ಕೂಡಿಸು_ಅಂಕೆ_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    ಶೂನ್ಯ
stuffReadbuff(char_u *s)
{
    ಕೂಡಿಸು_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    ಶೂನ್ಯ
stuffRedoReadbuff(char_u *s)
{
    ಕೂಡಿಸು_buff(&readbuf2, s, -1L);
}

    ಶೂನ್ಯ
stuffReadbuffLen(char_u *s, ದೀರ್ಘ len)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (ಕೂಡಿಸು_ಕೊನೆಯ_ಸೇರಿಸು == 1) // Only add if this is the first call, for
			      // recursive calls, ignore.
	ga_concat_len(&ಕೊನೆಯ_ಸೇರಿಸು_ga, s, (size_t)len);
#ಆದರೆ_ಅಂತ್ಯ
    ಕೂಡಿಸು_buff(&readbuf1, s, len);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    ಶೂನ್ಯ
stuffReadbuffSpec(char_u *s)
{
    ಪೂರ್ಣಾಂಕ c;

    ಆಗ (*s != NUL)
    {
	ಆದರೆ (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    c = mb_cptr2char_adv(&s);
	    ಆದರೆ (c == CAR || c == NL || c == ESC)
		c = ' ';
	    stuffcharReadbuff(c);
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Append a character to the stuff buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ಶೂನ್ಯ
stuffcharReadbuff(ಪೂರ್ಣಾಂಕ c)
{
    ಕೂಡಿಸು_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    ಶೂನ್ಯ
stuffnumReadbuff(ದೀರ್ಘ n)
{
    ಕೂಡಿಸು_ಅಂಕೆ_buff(&readbuf1, n);
}

/*
 * Stuff a string into the typeahead buffer, such that edit() will insert it
 * literally ("literally" TRUE) or interpret is as typed characters.
 */
    ಶೂನ್ಯ
stuffescaped(char_u *arg, ಪೂರ್ಣಾಂಕ literally)
{
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*ಆರಂಭ;

    ಆಗ (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	ಆರಂಭ = arg;
	ಆಗ ((*arg >= ' ' && *arg < DEL)
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	ಆದರೆ (arg > ಆರಂಭ)
	    stuffReadbuffLen(ಆರಂಭ, (ದೀರ್ಘ)(arg - ಆರಂಭ));

	// stuff a single special character
	ಆದರೆ (*arg != NUL)
	{
	    ಆದರೆ (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		c = *arg++;
	    ಆದರೆ (literally && ((c < ' ' && c != TAB) || c == DEL))
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_redo(ಪೂರ್ಣಾಂಕ init, ಪೂರ್ಣಾಂಕ old_redo)
{
    ಸ್ಥಿರ buffblock_T	*bp;
    ಸ್ಥಿರ char_u	*p;
    ಪೂರ್ಣಾಂಕ			c;
    ಪೂರ್ಣಾಂಕ			n;
    char_u		buf[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಂಕ			i;

    ಆದರೆ (init)
    {
	ಆದರೆ (old_redo)
	    bp = old_redobuff.bh_ಮೊದಲ.b_ಮುಂದಿನ;
	ಇಲ್ಲದಿದ್ದರೆ
	    bp = redobuff.bh_ಮೊದಲ.b_ಮುಂದಿನ;
	ಆದರೆ (bp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	p = bp->b_str;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	ಆದರೆ (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	ಇಲ್ಲದಿದ್ದರೆ
	    n = 1;
	ಗಾಗಿ (i = 0; ; ++i)
	{
	    ಆದರೆ (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = ಗಮ್ಯ_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    ಆದರೆ (c == CSI)	// escaped CSI
		p += 2;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (*++p == NUL && bp->b_ಮುಂದಿನ != ಖಾಲಿ)
	    {
		bp = bp->b_ಮುಂದಿನ;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    ಆದರೆ (i == n - 1)	// last byte of a character
	    {
		ಆದರೆ (n != 1)
		    c = (*mb_ptr2char)(buf);
		ನಿಲ್ಲಿಸು;
	    }
	    c = *p;
	    ಆದರೆ (c == NUL)	// cannot happen?
		ನಿಲ್ಲಿಸು;
	}
    }

    ಹಿಂತಿರುಗಿ c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_redo(ಪೂರ್ಣಾಂಕ old_redo)
{
    ಪೂರ್ಣಾಂಕ	    c;

    ಆಗ ((c = read_redo(FALSE, old_redo)) != NUL)
	ಕೂಡಿಸು_char_buff(&readbuf2, c);
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
    ಪೂರ್ಣಾಂಕ
ಆರಂಭ_redo(ದೀರ್ಘ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ old_redo)
{
    ಪೂರ್ಣಾಂಕ	    c;

    // init the pointers; return if nothing to redo
    ಆದರೆ (read_redo(TRUE, old_redo) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    c = read_redo(FALSE, old_redo);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (c == K_SID)
    {
	// Copy the <SID>{sid}; sequence
	ಕೂಡಿಸು_char_buff(&readbuf2, c);
	ಗಾಗಿ (;;)
	{
	    c = read_redo(FALSE, old_redo);
	    ಕೂಡಿಸು_char_buff(&readbuf2, c);
	    ಆದರೆ (!ಸುರಕ್ಷಿತ_isdigit(c))
		ನಿಲ್ಲಿಸು;
	}
	c = read_redo(FALSE, old_redo);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // copy the buffer name, if present
    ಆದರೆ (c == '"')
    {
	ಕೂಡಿಸು_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	// if a numbered buffer is used, increment the number
	ಆದರೆ (c >= '1' && c < '9')
	    ++c;
	ಕೂಡಿಸು_char_buff(&readbuf2, c);

	// the expression register should be re-evaluated
	ಆದರೆ (c == '=')
	{
	    ಕೂಡಿಸು_char_buff(&readbuf2, CAR);
	    cmd_silent = TRUE;
	}

	c = read_redo(FALSE, old_redo);
    }

    ಆದರೆ (c == 'v')   // redo Visual
    {
	VIsual = curwin->w_cursor;
	VIsual_ಸಕ್ರಿಯ = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    // try to enter the count (in place of a previous count)
    ಆದರೆ (ಎಣಿಕೆ_2)
    {
	ಆಗ (VIM_ISDIGIT(c))	// skip "old" count
	    c = read_redo(FALSE, old_redo);
	ಕೂಡಿಸು_ಅಂಕೆ_buff(&readbuf2, ಎಣಿಕೆ_2);
    }

    // copy the rest from the redo buffer into the stuff buffer
    ಕೂಡಿಸು_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
ಆರಂಭ_redo_ins(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    c;

    ಆದರೆ (read_redo(TRUE, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆರಂಭ_stuff();

    // skip the count and the command character
    ಆಗ ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	ಆದರೆ (vim_strchr((char_u *)"AaIiRrOo", c) != ಖಾಲಿ)
	{
	    ಆದರೆ (c == 'O' || c == 'o')
		ಕೂಡಿಸು_buff(&readbuf2, NL_STR, -1L);
	    ನಿಲ್ಲಿಸು;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    ಖಂಡ_redo = TRUE;
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
stop_redo_ins(ಶೂನ್ಯ)
{
    ಖಂಡ_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_typebuf(ಶೂನ್ಯ)
{
    ಆದರೆ (typebuf.tb_buf != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

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
    ಪೂರ್ಣಾಂಕ
noremap_ಕೀಲಿಗಳು(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಕೀಲಿ_Noremap & (RM_NONE|RM_SCRIPT);
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
    ಪೂರ್ಣಾಂಕ
ins_typebuf(
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		noremap,
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ,
    ಪೂರ್ಣಾಂಕ		nottyped,
    ಪೂರ್ಣಾಂಕ		silent)
{
    char_u	*s1, *s2;
    ಪೂರ್ಣಾಂಕ		newlen;
    ಪೂರ್ಣಾಂಕ		addlen;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		newoff;
    ಪೂರ್ಣಾಂಕ		val;
    ಪೂರ್ಣಾಂಕ		nrm;

    init_typebuf();
    ಆದರೆ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
    state_no_longer_ಸುರಕ್ಷಿತ("ins_typebuf()");

    addlen = (ಪೂರ್ಣಾಂಕ)STRLEN(str);

    ಆದರೆ (ಸ್ಥಾನಾಂತರ == 0 && addlen <= typebuf.tb_off)
    {
	/*
	 * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
	 */
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (typebuf.tb_len == 0 && typebuf.tb_buflen
					       >= addlen + 3 * (MAXMAPLEN + 4))
    {
	/*
	 * Buffer is empty and string fits in the existing buffer.
	 * Leave some space before and after, if possible.
	 */
	typebuf.tb_off = (typebuf.tb_buflen - addlen - 3 * (MAXMAPLEN + 4)) / 2;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ extra;

	/*
	 * Need to allocate a new buffer.
	 * In typebuf.tb_buf there must always be room for 3 * (MAXMAPLEN + 4)
	 * characters.  We add some extra room to avoid having to allocate too
	 * often.
	 */
	newoff = MAXMAPLEN + 4;
	extra = addlen + newoff + 4 * (MAXMAPLEN + 4);
	ಆದರೆ (typebuf.tb_len > 2147483647 - extra)
	{
	    // string is getting too long for a 32 bit int
	    emsg(_(e_command_too_complex));    // also calls flush_buffers
	    setcursor();
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	newlen = typebuf.tb_len + extra;
	s1 = alloc(newlen);
	ಆದರೆ (s1 == ಖಾಲಿ)		    // out of memory
	    ಹಿಂತಿರುಗಿ FAIL;
	s2 = alloc(newlen);
	ಆದರೆ (s2 == ಖಾಲಿ)		    // out of memory
	{
	    vim_free(s1);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	typebuf.tb_buflen = newlen;

	// copy the old chars, before the insertion point
	mch_memmove(s1 + newoff, typebuf.tb_buf + typebuf.tb_off,
							      (size_t)ಸ್ಥಾನಾಂತರ);
	// copy the new chars
	mch_memmove(s1 + newoff + ಸ್ಥಾನಾಂತರ, str, (size_t)addlen);
	// copy the old chars, after the insertion point, including the	NUL at
	// the end
	mch_memmove(s1 + newoff + ಸ್ಥಾನಾಂತರ + addlen,
				     typebuf.tb_buf + typebuf.tb_off + ಸ್ಥಾನಾಂತರ,
				       (size_t)(typebuf.tb_len - ಸ್ಥಾನಾಂತರ + 1));
	ಆದರೆ (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)ಸ್ಥಾನಾಂತರ);
	mch_memmove(s2 + newoff + ಸ್ಥಾನಾಂತರ + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + ಸ್ಥಾನಾಂತರ,
					   (size_t)(typebuf.tb_len - ಸ್ಥಾನಾಂತರ));
	ಆದರೆ (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    // If noremap == REMAP_SCRIPT: do remap script-local mappings.
    ಆದರೆ (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (noremap == REMAP_SKIP)
	val = RM_ABBR;
    ಇಲ್ಲದಿದ್ದರೆ
	val = RM_NONE;

    /*
     * Adjust typebuf.tb_noremap[] for the new characters:
     * If noremap == REMAP_NONE or REMAP_SCRIPT: new characters are
     *			(sometimes) not remappable
     * If noremap == REMAP_YES: all the new characters are mappable
     * If noremap  > 0: "noremap" characters are not remappable, the rest
     *			mappable
     */
    ಆದರೆ (noremap == REMAP_SKIP)
	nrm = 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (noremap < 0)
	nrm = addlen;
    ಇಲ್ಲದಿದ್ದರೆ
	nrm = noremap;
    ಗಾಗಿ (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + ಸ್ಥಾನಾಂತರ] =
						  (--nrm >= 0) ? val : RM_YES;

    // tb_maplen and tb_silent only remember the length of mapped and/or
    // silent mappings at the start of the buffer, assuming that a mapped
    // sequence doesn't result in typed characters.
    ಆದರೆ (nottyped || typebuf.tb_maplen > ಸ್ಥಾನಾಂತರ)
	typebuf.tb_maplen += addlen;
    ಆದರೆ (silent || typebuf.tb_silent > ಸ್ಥಾನಾಂತರ)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    ಆದರೆ (typebuf.tb_no_abbr_cnt && ಸ್ಥಾನಾಂತರ == 0)	// and not used for abbrev.s
	typebuf.tb_no_abbr_cnt += addlen;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Put character "c" back into the typeahead buffer.
 * Can be used for a character obtained by vgetc() that needs to be put back.
 * Uses cmd_silent, KeyTyped and KeyNoremap to restore the flags belonging to
 * the char.
 * Returns the length of what was inserted.
 */
    ಪೂರ್ಣಾಂಕ
ins_char_typebuf(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ modifiers)
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    ಪೂರ್ಣಾಂಕ		len = special_ಗಮ್ಯ_buf(c, modifiers, TRUE, buf);

    buf[len] = NUL;
    (ಶೂನ್ಯ)ins_typebuf(buf, ಕೀಲಿ_Noremap, 0, !ಕೀಲಿ_Typed, cmd_silent);
    ಹಿಂತಿರುಗಿ len;
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
    ಪೂರ್ಣಾಂಕ
typebuf_ಬದಲಾಯಿತು(
    ಪೂರ್ಣಾಂಕ		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    ಹಿಂತಿರುಗಿ (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#ಆದರೆ_ಅಂತ್ಯ
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    ಪೂರ್ಣಾಂಕ
typebuf_typed(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    ಪೂರ್ಣಾಂಕ
typebuf_maplen(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    ಶೂನ್ಯ
del_typebuf(ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    ಆದರೆ (ಸ್ಥಾನಾಂತರ == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    ಇಲ್ಲದಿದ್ದರೆ
    {
	i = typebuf.tb_off + ಸ್ಥಾನಾಂತರ;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	ಆದರೆ (typebuf.tb_off > MAXMAPLEN)
	{
	    mch_memmove(typebuf.tb_buf + MAXMAPLEN,
			     typebuf.tb_buf + typebuf.tb_off, (size_t)ಸ್ಥಾನಾಂತರ);
	    mch_memmove(typebuf.tb_noremap + MAXMAPLEN,
			 typebuf.tb_noremap + typebuf.tb_off, (size_t)ಸ್ಥಾನಾಂತರ);
	    typebuf.tb_off = MAXMAPLEN;
	}
	// adjust typebuf.tb_buf (include the NUL at the end)
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + ಸ್ಥಾನಾಂತರ,
						     typebuf.tb_buf + i + len,
				       (size_t)(typebuf.tb_len - ಸ್ಥಾನಾಂತರ + 1));
	// adjust typebuf.tb_noremap[]
	mch_memmove(typebuf.tb_noremap + typebuf.tb_off + ಸ್ಥಾನಾಂತರ,
						 typebuf.tb_noremap + i + len,
					   (size_t)(typebuf.tb_len - ಸ್ಥಾನಾಂತರ));
    }

    ಆದರೆ (typebuf.tb_maplen > ಸ್ಥಾನಾಂತರ)		// adjust tb_maplen
    {
	ಆದರೆ (typebuf.tb_maplen < ಸ್ಥಾನಾಂತರ + len)
	    typebuf.tb_maplen = ಸ್ಥಾನಾಂತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    typebuf.tb_maplen -= len;
    }
    ಆದರೆ (typebuf.tb_silent > ಸ್ಥಾನಾಂತರ)		// adjust tb_silent
    {
	ಆದರೆ (typebuf.tb_silent < ಸ್ಥಾನಾಂತರ + len)
	    typebuf.tb_silent = ಸ್ಥಾನಾಂತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    typebuf.tb_silent -= len;
    }
    ಆದರೆ (typebuf.tb_no_abbr_cnt > ಸ್ಥಾನಾಂತರ)	// adjust tb_no_abbr_cnt
    {
	ಆದರೆ (typebuf.tb_no_abbr_cnt < ಸ್ಥಾನಾಂತರ + len)
	    typebuf.tb_no_abbr_cnt = ಸ್ಥಾನಾಂತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    typebuf.tb_no_abbr_cnt -= len;
    }

#ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * State for adding bytes to a recording or 'showcmd'.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    ಪೂರ್ಣಾಂಕ		prev_c;
    size_t	buflen;
    ಚಿಹ್ನೆರಹಿತ	pending_special;
    ಚಿಹ್ನೆರಹಿತ	pending_mbyte;
} gotchars_state_T;

/*
 * Add a single byte to a recording or 'showcmd'.
 * Return TRUE if a full key has been received, FALSE otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gotchars_ಕೂಡಿಸು_byte(gotchars_state_T *state, char_u byte)
{
    ಪೂರ್ಣಾಂಕ		c = state->buf[state->buflen++] = byte;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
    ಪೂರ್ಣಾಂಕ		ಒಳ_special = state->pending_special > 0;
    ಪೂರ್ಣಾಂಕ		ಒಳ_mbyte = state->pending_mbyte > 0;

    ಆದರೆ (ಒಳ_special)
	state->pending_special--;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == K_SPECIAL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		|| c == CSI
#ಆದರೆ_ಅಂತ್ಯ
	    )
	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	state->pending_special = 2;

    ಆದರೆ (state->pending_special > 0)
	ಹೋಗು ret_false;

    ಆದರೆ (ಒಳ_mbyte)
	state->pending_mbyte--;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಒಳ_special)
	{
	    ಆದರೆ (state->prev_c == KS_MODIFIER)
		// When receiving a modifier, wait for the modified key.
		ಹೋಗು ret_false;
	    c = ಗಮ್ಯ_SPECIAL(state->prev_c, c);
	    // Drop K_FOCUSGAINED, K_FOCUSLOST and K_COMPLETE_DELAY, they are
	    // not useful in a recording.
	    ಆದರೆ (c == K_FOCUSGAINED || c == K_FOCUSLOST || c == K_COMPLETE_DELAY)
		state->buflen = 0;
	}
	// When receiving a multibyte character, store it until we have all
	// the bytes, so that it won't be split between two buffer blocks,
	// and delete_buff_tail() will work properly.
	state->pending_mbyte = MB_BYTE2LEN_CHECK(c) - 1;
    }

    ಆದರೆ (state->pending_mbyte > 0)
	ಹೋಗು ret_false;

    retval = TRUE;
ret_false:
    state->prev_c = c;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the record buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gotchars(char_u *chars, ಪೂರ್ಣಾಂಕ len)
{
    char_u		*s = chars;
    size_t		i;
    ಪೂರ್ಣಾಂಕ			todo = len;
    ಸ್ಥಿರ gotchars_state_T state;

    ಆಗ (todo-- > 0)
    {
	ಆದರೆ (!gotchars_ಕೂಡಿಸು_byte(&state, *s++))
	    ಮುಂದುವರಿ;

	// Handle one byte at a time; no translation to be done.
	ಗಾಗಿ (i = 0; i < state.buflen; ++i)
	    updatescript(state.buf[i]);

	ಆದರೆ (reg_recording != 0)
	{
	    state.buf[state.buflen] = NUL;
	    ಕೂಡಿಸು_buff(&recordbuff, state.buf, (ದೀರ್ಘ)state.buflen);
	    // remember how many chars were last recorded
	    ಕೊನೆಯ_recorded_len += state.buflen;
	}
	state.buflen = 0;
    }

    may_sync_undo();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // output "debug mode" message next time in debug mode
    debug_did_ಸಂದೇಶ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}

/*
 * Record an <Ignore> key.
 */
    ಶೂನ್ಯ
gotchars_ignore(ಶೂನ್ಯ)
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
    ಶೂನ್ಯ
ungetchars(ಪೂರ್ಣಾಂಕ len)
{
    ಆದರೆ (reg_recording == 0)
	ಹಿಂತಿರುಗಿ;

    delete_buff_ಬಾಲ(&recordbuff, len);
    ಕೊನೆಯ_recorded_len -= len;
}

/*
 * Sync undo.  Called when typed characters are obtained from the typeahead
 * buffer, or when a menu is used.
 * Do not sync:
 * - In Insert mode, unless cursor key has been used.
 * - While reading a script file.
 * - When no_u_sync is non-zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_sync_undo(ಶೂನ್ಯ)
{
    ಆದರೆ ((!(State & (ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE)) || arrow_ಬಳಸಿದ)
					       && scriptin[curscript] == ಖಾಲಿ)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
alloc_typebuf(ಶೂನ್ಯ)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    ಆದರೆ (typebuf.tb_buf == ಖಾಲಿ || typebuf.tb_noremap == ಖಾಲಿ)
    {
	free_typebuf();
	ಹಿಂತಿರುಗಿ FAIL;
    }
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_off = MAXMAPLEN + 4;  // can insert without realloc
    typebuf.tb_len = 0;
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    typebuf.tb_no_abbr_cnt = 0;
    ಆದರೆ (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
#ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    typebuf_was_filled = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Free the buffers of "typebuf".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_typebuf(ಶೂನ್ಯ)
{
    ಆದರೆ (typebuf.tb_buf == typebuf_init)
	ಆಂತರಿಕ_error("Free typebuf 1");
    ಇಲ್ಲದಿದ್ದರೆ
	VIM_CLEAR(typebuf.tb_buf);
    ಆದರೆ (typebuf.tb_noremap == noremapbuf_init)
	ಆಂತರಿಕ_error("Free typebuf 2");
    ಇಲ್ಲದಿದ್ದರೆ
	VIM_CLEAR(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
ಸ್ಥಿರ typebuf_T saved_typebuf[NSCRIPT];

    ಪೂರ್ಣಾಂಕ
save_typebuf(ಶೂನ್ಯ)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    ಆದರೆ (alloc_typebuf() == FAIL)
    {
	closescript();
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_char = -1;	// character put back by vungetc()
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_mod_ಮುಖವಾಡ;	// mod_mask for ungotten character
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_mouse_ಅಡ್ಡಸಾಲು;	// mouse_row related to old_char
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_mouse_ಲಂಬಸಾಲು;	// mouse_col related to old_char
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_ಕೀಲಿ_Stuffed;	// whether old_char was stuffed

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ can_get_old_char(ಶೂನ್ಯ)
{
    // If the old character was not stuffed and characters have been added to
    // the stuff buffer, need to first get the stuffed characters instead.
    ಹಿಂತಿರುಗಿ old_char != -1 && (old_ಕೀಲಿ_Stuffed || stuff_ಖಾಲಿಗಳು());
}

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    ಶೂನ್ಯ
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    ಆದರೆ (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_ಮುಖವಾಡ = old_mod_ಮುಖವಾಡ;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_ಮೊದಲ.b_ಮುಂದಿನ = ಖಾಲಿ;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_ಮೊದಲ.b_ಮುಂದಿನ = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಒಳಹರಿವು_BUF
    tp->save_inputbuf = get_ಒಳಹರಿವು_buf();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    ಶೂನ್ಯ
restore_typeahead(tasave_T *tp, ಪೂರ್ಣಾಂಕ overwrite ಬಳಸದ)
{
    ಆದರೆ (tp->typebuf_valid)
    {
	free_typebuf();
	typebuf = tp->save_typebuf;
    }

    old_char = tp->old_char;
    old_mod_ಮುಖವಾಡ = tp->old_mod_ಮುಖವಾಡ;

    free_buff(&readbuf1);
    readbuf1 = tp->save_readbuf1;
    free_buff(&readbuf2);
    readbuf2 = tp->save_readbuf2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಒಳಹರಿವು_BUF
    ಗಣ_ಒಳಹರಿವು_buf(tp->save_inputbuf, overwrite);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Open a new script file for the ":source!" command.
 */
    ಶೂನ್ಯ
openscript(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		directly)	// when TRUE execute directly
{
    ಆದರೆ (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	ಹಿಂತಿರುಗಿ;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (scriptin[curscript] != ಖಾಲಿ)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    ಆದರೆ ((scriptin[curscript] = mch_fopen((ಅಕ್ಷರ *)NameBuff, READBIN)) == ಖಾಲಿ)
    {
	semsg(_(e_cant_open_file_str), name);
	ಆದರೆ (curscript)
	    --curscript;
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (save_typebuf() == FAIL)
	ಹಿಂತಿರುಗಿ;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    ಆದರೆ (directly)
    {
	oparg_T	oa;
	ಪೂರ್ಣಾಂಕ	oldcurscript;
	ಪೂರ್ಣಾಂಕ	save_State = State;
	ಪೂರ್ಣಾಂಕ	save_restart_edit = restart_edit;
	ಪೂರ್ಣಾಂಕ	save_insertmode = p_im;
	ಪೂರ್ಣಾಂಕ	save_ಮುಕ್ತಾಯ_op = ಮುಕ್ತಾಯ_op;
	ಪೂರ್ಣಾಂಕ	save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;

	State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
	ಸಂದೇಶ_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	ಮುಕ್ತಾಯ_op = FALSE;

	oldcurscript = curscript;
	ಮಾಡು
	{
	    update_topline_cursor();	// update cursor position and topline
	    ಸಾಮಾನ್ಯ_cmd(&oa, FALSE);	// execute one command
	    (ಶೂನ್ಯ)vpeekc();		// check for end of file
	}
	ಆಗ (scriptin[oldcurscript] != ಖಾಲಿ);

	State = save_State;
	ಸಂದೇಶ_scroll = save_ಸಂದೇಶ_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	ಮುಕ್ತಾಯ_op = save_ಮುಕ್ತಾಯ_op;
    }
}

/*
 * Close the currently active input script.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
closescript(ಶೂನ್ಯ)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = ಖಾಲಿ;
    ಆದರೆ (curscript > 0)
	--curscript;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
close_all_scripts(ಶೂನ್ಯ)
{
    ಆಗ (scriptin[0] != ಖಾಲಿ)
	closescript();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE when reading keys from a script file.
 */
    ಪೂರ್ಣಾಂಕ
using_script(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ scriptin[curscript] != ಖಾಲಿ;
}

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    ಶೂನ್ಯ
before_blocking(ಶೂನ್ಯ)
{
    // Tell terminal to flush screen contents before blocking
    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_FLUSH);

    updatescript(0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (may_garbage_collect)
	garbage_collect(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * updatescript() is called when a character can be written into the script
 * file or when we have waited some time for a character (c == 0)
 *
 * All the changed memfiles are synced if c == 0 or when the number of typed
 * characters reaches 'updatecount' and 'updatecount' is non-zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
updatescript(ಪೂರ್ಣಾಂಕ c)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2 = 0;

    ಆದರೆ (c && scriptout)
	putc(c, scriptout);
    ಆದರೆ (c == 0 || (p_uc > 0 && ++ಎಣಿಕೆ_2 >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	ಎಣಿಕೆ_2 = 0;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (typedchars_pos < MAXMAPLEN)
    {
	typedchars[typedchars_pos] = c;
	typedchars_pos++;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Convert "c_arg" plus "modifiers" to merge the effect of modifyOtherKeys into
 * the character.  Also for when the Kitty key protocol is used.
 */
    ಪೂರ್ಣಾಂಕ
ವಿಲೀನ_modify_Other_ಕೀಲಿಗಳು(ಪೂರ್ಣಾಂಕ c_arg, ಪೂರ್ಣಾಂಕ *modifiers)
{
    ಪೂರ್ಣಾಂಕ c = c_arg;

    // CTRL only uses the lower 5 bits of the character.
    ಆದರೆ (*modifiers & MOD_ಮುಖವಾಡ_CTRL)
    {
	ಆದರೆ ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	{
	    c &= 0x1f;
	    ಆದರೆ (c == NUL)
		c = K_ZERO;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '2')
	    c = NUL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '8')
	    c = BS;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '?')
	    c = DEL;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (c != c_arg)
	    *modifiers &= ~MOD_ಮುಖವಾಡ_CTRL;
    }

    // Alt/Meta sets the 8th bit of the character.
    ಆದರೆ ((*modifiers & (MOD_ಮುಖವಾಡ_META | MOD_ಮುಖವಾಡ_ALT))
	    && c >= 0 && c <= 127)
    {
	// Some terminals (esp. Kitty) do not include Shift in the character.
	// Apply it here to get consistency across terminals.  Only do ASCII
	// letters, for other characters it depends on the keyboard layout.
	ಆದರೆ ((*modifiers & MOD_ಮುಖವಾಡ_ಸರಿಸು) && c >= 'a' && c <= 'z')
	{
	    c += 'a' - 'A';
	    *modifiers &= ~MOD_ಮುಖವಾಡ_ಸರಿಸು;
	}
	c += 0x80;
	*modifiers &= ~(MOD_ಮುಖವಾಡ_META | MOD_ಮುಖವಾಡ_ALT);
    }

    ಹಿಂತಿರುಗಿ c;
}

/*
 * Add a single byte to 'showcmd' for a partially matched mapping.
 * Call add_to_showcmd() if a full key has been received.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_byte_ಗಮ್ಯ_showcmd(char_u byte)
{
    ಸ್ಥಿರ gotchars_state_T state;
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		modifiers = 0;
    ಪೂರ್ಣಾಂಕ		c = NUL;

    ಆದರೆ (!p_sc || ಸಂದೇಶ_silent != 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!gotchars_ಕೂಡಿಸು_byte(&state, byte))
	ಹಿಂತಿರುಗಿ;

    state.buf[state.buflen] = NUL;
    state.buflen = 0;

    ptr = state.buf;
    ಆದರೆ (ptr[0] == K_SPECIAL && ptr[1] == KS_MODIFIER && ptr[2] != NUL)
    {
	modifiers = ptr[2];
	ptr += 3;
    }

    ಆದರೆ (*ptr != NUL)
    {
	char_u		*mb_ptr = mb_unescape(&ptr);

	c = mb_ptr != ಖಾಲಿ ? (*mb_ptr2char)(mb_ptr) : *ptr++;
	ಆದರೆ (c <= 0x7f)
	{
	    // Merge modifiers into the key to make the result more readable.
	    ಪೂರ್ಣಾಂಕ		modifiers_after = modifiers;
	    ಪೂರ್ಣಾಂಕ		mod_c = ವಿಲೀನ_modify_Other_ಕೀಲಿಗಳು(c, &modifiers_after);

	    ಆದರೆ (modifiers_after == 0)
	    {
		modifiers = 0;
		c = mod_c;
	    }
	}
    }

    // TODO: is there a more readable and yet compact representation of
    // modifiers and special keys?
    ಆದರೆ (modifiers != 0)
    {
	ಕೂಡಿಸು_ಗಮ್ಯ_showcmd(K_SPECIAL);
	ಕೂಡಿಸು_ಗಮ್ಯ_showcmd(KS_MODIFIER);
	ಕೂಡಿಸು_ಗಮ್ಯ_showcmd(modifiers);
    }
    ಆದರೆ (c != NUL)
	ಕೂಡಿಸು_ಗಮ್ಯ_showcmd(c);
    ಆಗ (*ptr != NUL)
	ಕೂಡಿಸು_ಗಮ್ಯ_showcmd(*ptr++);
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
    ಪೂರ್ಣಾಂಕ
vgetc(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		c, c2;
    ಪೂರ್ಣಾಂಕ		n;
    char_u	buf[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಂಕ		i;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    ಆದರೆ (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    ಆದರೆ (can_get_old_char())
    {
	c = old_char;
	old_char = -1;
	mod_ಮುಖವಾಡ = old_mod_ಮುಖವಾಡ;
	mouse_ಅಡ್ಡಸಾಲು = old_mouse_ಅಡ್ಡಸಾಲು;
	mouse_ಲಂಬಸಾಲು = old_mouse_ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// number of characters recorded from the last vgetc() call
	ಸ್ಥಿರ size_t	ಕೊನೆಯ_vgetc_recorded_len = 0;

	mod_ಮುಖವಾಡ = 0;
	vgetc_mod_ಮುಖವಾಡ = 0;
	vgetc_char = 0;

	// last_recorded_len can be larger than last_vgetc_recorded_len
	// if peeking records more
	ಕೊನೆಯ_recorded_len -= ಕೊನೆಯ_vgetc_recorded_len;

	ಗಾಗಿ (;;)		// this is done twice if there are modifiers
	{
	    ಪೂರ್ಣಾಂಕ did_inc = FALSE;

	    // No mapping after modifier has been read, using an input method
	    // and when a popup window has disabled mapping.
	    ಆದರೆ (mod_ಮುಖವಾಡ
#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#ಆದರೆ_ಅಂತ್ಯ
		    )
	    {
		++no_mapping;
		++allow_ಕೀಲಿಗಳು;
		// mod_mask value may change, remember we did the increment
		did_inc = TRUE;
	    }
	    c = vgetorpeek(TRUE);
	    ಆದರೆ (did_inc)
	    {
		--no_mapping;
		--allow_ಕೀಲಿಗಳು;
	    }

	    // Get two extra bytes for special keys, handle modifiers.
	    ಆದರೆ (c == K_SPECIAL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    || c == CSI
#ಆದರೆ_ಅಂತ್ಯ
	       )
	    {
		ಪೂರ್ಣಾಂಕ	    save_allow_ಕೀಲಿಗಳು = allow_ಕೀಲಿಗಳು;

		++no_mapping;
		allow_ಕೀಲಿಗಳು = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_ಕೀಲಿಗಳು = save_allow_ಕೀಲಿಗಳು;
		ಆದರೆ (c2 == KS_MODIFIER)
		{
		    mod_ಮುಖವಾಡ = c;
		    ಮುಂದುವರಿ;
		}
		c = ಗಮ್ಯ_SPECIAL(c2, c);

		ಆದರೆ (allow_osc_ಕೀಲಿ == 0 && c == K_OSC)
		    ಮುಂದುವರಿ;

		// K_ESC is used to avoid ambiguity with the single Esc
		// character that might be the start of an escape sequence.
		// Convert it back to a single Esc here.
		ಆದರೆ (c == K_ESC)
		    c = ESC;

#ಆದರೆ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		// Handle K_TEAROFF here, the caller of vgetc() doesn't need to
		// know that a menu was torn off
		ಆದರೆ (
# ifdef VIMDLL
		    gui.ಒಳ_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    ಪೂರ್ಣಾಂಕ		j;

		    // get menu path, it ends with a <CR>
		    ಗಾಗಿ (j = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[j] = c;
			ಆದರೆ (j < 199)
			    ++j;
		    }
		    name[j] = NUL;
		    gui_make_tearoff(name);
		    ಮುಂದುವರಿ;
		}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && !defined(USE_GTK4) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		ಆದರೆ (c == K_F10 && gui.menubar != ಖಾಲಿ)
		{
		    gtk_menu_shell_select_ಮೊದಲ(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    ಮುಂದುವರಿ;
		}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		// Handle focus event here, so that the caller doesn't need to
		// know about it.  Return K_IGNORE so that we loop once (needed
		// if 'lazyredraw' is set).
		ಆದರೆ (c == K_FOCUSGAINED || c == K_FOCUSLOST)
		{
		    ui_focus_change(c == K_FOCUSGAINED);
		    c = K_IGNORE;
		}

		// Translate K_CSI to CSI.  The special key is only used to
		// avoid it being recognized as the start of a special key.
		ಆದರೆ (c == K_CSI)
		    c = CSI;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಆದರೆ (c == K_SID)
		{
		    ಪೂರ್ಣಾಂಕ	    j;

		    // Handle <SID>{sid};  Do up to 20 digits for safety.
		    ಕೊನೆಯ_ಬಳಸಿದ_sid = 0;
		    ಗಾಗಿ (j = 0; j < 20 && ಸುರಕ್ಷಿತ_isdigit(c = vgetorpeek(TRUE)); ++j)
			ಕೊನೆಯ_ಬಳಸಿದ_sid = ಕೊನೆಯ_ಬಳಸಿದ_sid * 10 + (c - '0');
		    ಕೊನೆಯ_ಬಳಸಿದ_map = ಖಾಲಿ;
		    ಮುಂದುವರಿ;
		}
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    ಆದರೆ (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		ಗಾಗಿ (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    ಆದರೆ (buf[i] == K_SPECIAL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			    || (buf[i] == CSI)
#ಆದರೆ_ಅಂತ್ಯ
			    )
		    {
			// Must be a K_SPECIAL - KS_SPECIAL - KE_FILLER
			// sequence, which represents a K_SPECIAL (0x80),
			// or a CSI - KS_EXTRA - KE_CSI sequence, which
			// represents a CSI (0x9B),
			// or a K_SPECIAL - KS_EXTRA - KE_CSI, which is CSI
			// too.
			c = vgetorpeek(TRUE);
			ಆದರೆ (vgetorpeek(TRUE) == KE_CSI && c == KS_EXTRA)
			    buf[i] = CSI;
		    }
		}
		--no_mapping;
		c = (*mb_ptr2char)(buf);
	    }

	    ಆದರೆ (vgetc_char == 0)
	    {
		vgetc_mod_ಮುಖವಾಡ = mod_ಮುಖವಾಡ;
		vgetc_char = c;
	    }

	    // A keypad or special function key was not mapped, use it like
	    // its ASCII equivalent.
	    ಆಯ್ಕೆ (c)
	    {
		ಸ್ಥಿತಿ K_KPLUS:	c = '+'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_KMINUS:	c = '-'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_KDIVIDE:	c = '/'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_KMULTIPLY: c = '*'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_KENTER:	c = CAR; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_KPOINT:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = Map_Virtual_ಕೀಲಿ(VK_DECIMAL, 2); ನಿಲ್ಲಿಸು;
#ಇಲ್ಲದಿದ್ದರೆ
				c = '.'; ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		ಸ್ಥಿತಿ K_K0:	c = '0'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_K1:	c = '1'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_K2:	c = '2'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_K3:	c = '3'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_K4:	c = '4'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_K5:	c = '5'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_K6:	c = '6'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_K7:	c = '7'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_K8:	c = '8'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_K9:	c = '9'; ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ K_XHOME:
		ಸ್ಥಿತಿ K_ZHOME:	ಆದರೆ (mod_ಮುಖವಾಡ == MOD_ಮುಖವಾಡ_ಸರಿಸು)
				{
				    c = K_S_HOME;
				    mod_ಮುಖವಾಡ = 0;
				}
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mod_ಮುಖವಾಡ == MOD_ಮುಖವಾಡ_CTRL)
				{
				    c = K_C_HOME;
				    mod_ಮುಖವಾಡ = 0;
				}
				ಇಲ್ಲದಿದ್ದರೆ
				    c = K_HOME;
				ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_XEND:
		ಸ್ಥಿತಿ K_ZEND:	ಆದರೆ (mod_ಮುಖವಾಡ == MOD_ಮುಖವಾಡ_ಸರಿಸು)
				{
				    c = K_S_ಕೊನೆ;
				    mod_ಮುಖವಾಡ = 0;
				}
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mod_ಮುಖವಾಡ == MOD_ಮುಖವಾಡ_CTRL)
				{
				    c = K_C_ಕೊನೆ;
				    mod_ಮುಖವಾಡ = 0;
				}
				ಇಲ್ಲದಿದ್ದರೆ
				    c = K_ಕೊನೆ;
				ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ K_XUP:	c = K_ಮೇಲೆ; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_XDOWN:	c = K_ಕೆಳಕ್ಕೆ; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_XLEFT:	c = K_ಎಡ; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_XRIGHT:	c = K_ಬಲ; ನಿಲ್ಲಿಸು;
	    }

	    ನಿಲ್ಲಿಸು;
	}

	ಕೊನೆಯ_vgetc_recorded_len = ಕೊನೆಯ_recorded_len;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    /*
     * In the main loop "may_garbage_collect" can be set to do garbage
     * collection in the first next vgetc().  It's disabled after that to
     * avoid internally used Lists and Dicts to be freed.
     */
    may_garbage_collect = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
    ಆದರೆ (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD
	    && c != K_COMPLETE_DELAY)
    {
	// Don't trigger 'balloonexpr' unless only the mouse was moved.
	bevalexpr_due_ಗಣ = FALSE;
	ui_remove_balloon();
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // Only filter keys that do not come from ":normal".  Keys from feedkeys()
    // are filtered.
    ಆದರೆ ((!ex_ಸಾಮಾನ್ಯ_busy || ಒಳ_feedkeys) && popup_do_filter(c))
    {
	ಆದರೆ (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    c = do_ಕೀಲಿ_ಒಳಹರಿವು_pre(c);

    // Clear the next typedchars_pos
    typedchars_pos = 0;
#ಆದರೆ_ಅಂತ್ಯ

    // Need to process the character before we know it's safe to do something
    // else.
    ಆದರೆ (c != K_IGNORE)
	state_no_longer_ಸುರಕ್ಷಿತ("key typed");

    ಹಿಂತಿರುಗಿ c;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Handle the KeyInputPre autocommand.
 * "c" is the character that was typed.
 * Return new input character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_ಕೀಲಿ_ಒಳಹರಿವು_pre(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ		res = c;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    char_u	curr_ಬಹುಲಕ[ಬಹುಲಕ_MAX_ಉದ್ದಳತೆ];
    ಪೂರ್ಣಾಂಕ		save_State = State;
    save_v_event_T save_v_event;
    dict_T	*v_event;

    // Return quickly when there is nothing to do.
    ಆದರೆ (!has_keyinputpre())
	ಹಿಂತಿರುಗಿ res;

    ಆದರೆ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = KEY2TERMCAP0(c);
	buf[2] = KEY2TERMCAP1(c);
	buflen = 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	buflen = (*mb_char2bytes)(c, buf);
    buf[buflen] = NUL;

    typedchars[typedchars_pos] = NUL;
    vim_unescape_csi(typedchars);

    get_ಬಹುಲಕ(curr_ಬಹುಲಕ);

    // Lock the text to avoid weird things from happening.
    ++textlock;
    ಗಣ_vim_var_string(VV_CHAR, buf, (ಪೂರ್ಣಾಂಕ)buflen);  // set v:char

    v_event = get_v_event(&save_v_event);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_bool(v_event, "typed", ಕೀಲಿ_Typed);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(v_event, "typedchar", typedchars, typedchars_pos);

    ಆದರೆ (apply_autocmds(EVENT_KEYINPUTPRE, curr_ಬಹುಲಕ, curr_ಬಹುಲಕ, FALSE, curbuf))
    {
	string_T    v_char;

	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character.
	v_char.string = get_vim_var_str(VV_CHAR);
	ಆದರೆ (STRCMP(buf, v_char.string) != 0)
	{
	    v_char.ಉದ್ದಳತೆ = STRLEN(v_char.string);
	    // Convert special bytes when it is special string.
	    ಆದರೆ (v_char.ಉದ್ದಳತೆ >= 3 && v_char.string[0] == K_SPECIAL)
		res = TERMCAP2_ಕೀಲಿ(v_char.string[1], v_char.string[2]);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (v_char.ಉದ್ದಳತೆ > 0)
		res = PTR2CHAR(v_char.string);
	}
    }

    restore_v_event(v_event, &save_v_event);

    ಗಣ_vim_var_string(VV_CHAR, ಖಾಲಿ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    ಪೂರ್ಣಾಂಕ
ಸುರಕ್ಷಿತ_vgetc(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	c;

    c = vgetc();
    ಆದರೆ (c == NUL)
	c = get_keystroke();
    ಹಿಂತಿರುಗಿ c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 * Does not handle bracketed paste - do not use the result for commands.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
plain_vgetc_nopaste(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ c;

    ಮಾಡು
	c = ಸುರಕ್ಷಿತ_vgetc();
    ಆಗ (c == K_IGNORE
	    || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
	    || c == K_MOUSEMOVE);
    ಹಿಂತಿರುಗಿ c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    ಪೂರ್ಣಾಂಕ
plain_vgetc(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ c = plain_vgetc_nopaste();

    ಆದರೆ (c == K_PS)
	// Only handle the first pasted character.  Drop the rest, since we
	// don't know what to do with it.
	c = bracketed_paste(PASTE_ONE_CHAR, FALSE, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 * Returns NUL if no character is available.
 */
    ಪೂರ್ಣಾಂಕ
vpeekc(ಶೂನ್ಯ)
{
    ಆದರೆ (can_get_old_char())
	ಹಿಂತಿರುಗಿ old_char;
    ಹಿಂತಿರುಗಿ vgetorpeek(FALSE);
}

#ಆದರೆ defined(FEAT_TERMRESPONSE) || defined(FEAT_TERMINAL)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    ಪೂರ್ಣಾಂಕ
vpeekc_nomap(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		c;

    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;
    c = vpeekc();
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;
    ಹಿಂತಿರುಗಿ c;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    ಪೂರ್ಣಾಂಕ
vpeekc_any(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		c;

    c = vpeekc();
    ಆದರೆ (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    ಹಿಂತಿರುಗಿ c;
}

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    ಪೂರ್ಣಾಂಕ
char_avail(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    retval;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    ಆದರೆ (disable_char_avail_for_testing)
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    ಹಿಂತಿರುಗಿ (retval != NUL);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * "getchar()" and "getcharstr()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
getchar_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ allow_ಸಂಖ್ಯೆ)
{
    varnumber_T		n = 0;
    ಪೂರ್ಣಾಂಕ			called_emsg_ಆರಂಭ = called_emsg;
    ಪೂರ್ಣಾಂಕ			error = FALSE;
    ಪೂರ್ಣಾಂಕ			simplify = TRUE;
    char_u		cursor_flag = 'm';

    ಆದರೆ ((ಒಳ_vim9script()
		&& check_for_opt_bool_or_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	    || (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type == VAR_DICT)
    {
	dict_T		*d = argvars[1].vval.v_dict;
	char_u		*cursor_str;

	ಆದರೆ (allow_ಸಂಖ್ಯೆ)
	    allow_ಸಂಖ್ಯೆ = dict_get_bool(d, "number", TRUE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dict_has_ಕೀಲಿ(d, "number"))
	    semsg(_(e_invalid_ವಾದ_str), "number");

	simplify = dict_get_bool(d, "simplify", TRUE);

	cursor_str = dict_get_string(d, "cursor", FALSE);
	ಆದರೆ (cursor_str != ಖಾಲಿ)
	{
	    ಆದರೆ (STRCMP(cursor_str, "hide") != 0
		    && STRCMP(cursor_str, "keep") != 0
		    && STRCMP(cursor_str, "msg") != 0)
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "cursor",
								   cursor_str);
	    ಇಲ್ಲದಿದ್ದರೆ
		cursor_flag = cursor_str[0];
	}
    }

    ಆದರೆ (called_emsg != called_emsg_ಆರಂಭ)
	ಹಿಂತಿರುಗಿ;

# ifdef ಸಂದೇಶ_ಸರತಿ
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
# endif

    ಆದರೆ (cursor_flag == 'h')
	cursor_sleep();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cursor_flag == 'm')
	windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);

    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;
    ++allow_osc_ಕೀಲಿ;
    ಆದರೆ (!simplify)
	++no_reduce_ಕೀಲಿಗಳು;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN
		|| (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ
			&& argvars[0].vval.v_ಸಂಖ್ಯೆ == -1))
	    // getchar(): blocking wait.
	    n = plain_vgetc_nopaste();
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = ಸುರಕ್ಷಿತ_vgetc();

	ಆದರೆ (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    ಮುಂದುವರಿ;
	ನಿಲ್ಲಿಸು;
    }
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;
    --allow_osc_ಕೀಲಿ;
    ಆದರೆ (!simplify)
	--no_reduce_ಕೀಲಿಗಳು;

    ಆದರೆ (cursor_flag == 'h')
	cursor_unsleep();

    ಗಣ_vim_var_nr(VV_MOUSE_WIN, 0);
    ಗಣ_vim_var_nr(VV_MOUSE_WINID, 0);
    ಗಣ_vim_var_nr(VV_MOUSE_LNUM, 0);
    ಗಣ_vim_var_nr(VV_MOUSE_ಲಂಬಸಾಲು, 0);

    ಆದರೆ (n != 0 && (!allow_ಸಂಖ್ಯೆ || IS_SPECIAL(n) || mod_ಮುಖವಾಡ != 0))
    {
	char_u		ತಾತ್ಕಾಲಿಕ[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	ಪೂರ್ಣಾಂಕ		i = 0;

	// Turn a special key into three bytes, plus modifier.
	ಆದರೆ (mod_ಮುಖವಾಡ != 0)
	{
	    ತಾತ್ಕಾಲಿಕ[i++] = K_SPECIAL;
	    ತಾತ್ಕಾಲಿಕ[i++] = KS_MODIFIER;
	    ತಾತ್ಕಾಲಿಕ[i++] = mod_ಮುಖವಾಡ;
	}
	ಆದರೆ (IS_SPECIAL(n))
	{
	    ತಾತ್ಕಾಲಿಕ[i++] = K_SPECIAL;
	    ತಾತ್ಕಾಲಿಕ[i++] = K_ಎರಡನೇ(n);
	    ತಾತ್ಕಾಲಿಕ[i++] = K_ಮೂರನೇ(n);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	    i += (*mb_char2bytes)(n, ತಾತ್ಕಾಲಿಕ + i);
	ಇಲ್ಲದಿದ್ದರೆ
	    ತಾತ್ಕಾಲಿಕ[i++] = n;
	ತಾತ್ಕಾಲಿಕ[i] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strnsave(ತಾತ್ಕಾಲಿಕ, i);

	ಆದರೆ (is_mouse_ಕೀಲಿ(n))
	{
	    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
	    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    ಪೂರ್ಣಾಂಕ		winnr = 1;

	    ಆದರೆ (ಅಡ್ಡಸಾಲು >= 0 && ಲಂಬಸಾಲು >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);
		ಆದರೆ (win == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ;
		(ಶೂನ್ಯ)mouse_comp_pos(win, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &lnum, ಖಾಲಿ);
# ifdef FEAT_PROP_POPUP
		ಆದರೆ (WIN_IS_POPUP(win))
		    winnr = 0;
		ಇಲ್ಲದಿದ್ದರೆ
# endif
		    ಗಾಗಿ (wp = firstwin; wp != win && wp != ಖಾಲಿ;
							       wp = wp->w_ಮುಂದಿನ)
			++winnr;
		ಗಣ_vim_var_nr(VV_MOUSE_WIN, winnr);
		ಗಣ_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		ಗಣ_vim_var_nr(VV_MOUSE_LNUM, lnum);
		ಗಣ_vim_var_nr(VV_MOUSE_ಲಂಬಸಾಲು, ಲಂಬಸಾಲು + 1);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!allow_ಸಂಖ್ಯೆ)
	rettv->v_type = VAR_STRING;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * "getchar()" function
 */
    ಶೂನ್ಯ
f_getchar(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, TRUE);
}

/*
 * "getcharstr()" function
 */
    ಶೂನ್ಯ
f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, FALSE);
}

/*
 * "getcharmod()" function
 */
    ಶೂನ್ಯ
f_getcharmod(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = mod_ಮುಖವಾಡ;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

#ಆದರೆ defined(ಸಂದೇಶ_ಸರತಿ)
# define MAX_REPEAT_PARSE 8

/*
 * Process messages that have been queued for netbeans or clientserver.
 * Also check if any jobs have ended.
 * These functions can call arbitrary Vim script and should only be called when
 * it is safe to do so.
 */
    ಶೂನ್ಯ
parse_queued_messages(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    old_curwin_id;
    ಪೂರ್ಣಾಂಕ	    old_curbuf_fnum;
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    save_may_garbage_collect = may_garbage_collect;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ entered = 0;
    ಪೂರ್ಣಾಂಕ	    was_ಸುರಕ್ಷಿತ = get_was_ಸುರಕ್ಷಿತ_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    ಆದರೆ (updating_screen || dont_parse_messages)
	ಹಿಂತಿರುಗಿ;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    ಆದರೆ (curbuf == ಖಾಲಿ || curwin == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    ಗಾಗಿ (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# ಆದರೆ (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_ಕೆಲಸ_CHANNEL)
	channel_handle_events(FALSE);
# endif

# ifdef FEAT_NETBEANS_INTG
	// Process the queued netbeans messages.
	netbeans_parse_messages();
# endif
# ifdef FEAT_SOCKETSERVER
	socketserver_parse_messages();
# endif
# ifdef FEAT_ಕೆಲಸ_CHANNEL
	// Write any buffer lines still to be written.
	channel_write_any_ಸಾಲುಗಳು();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# ಆದರೆ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_ಕೆಲಸ_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	ಆದರೆ (ಕೆಲಸ_check_ended())
	    ಮುಂದುವರಿ;
# endif
# ifdef FEAT_TERMINAL
	free_ಬಳಸದ_terminals();
# endif

# ifdef FEAT_SOUND_MACOSX
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	ಆದರೆ (has_sound_callback_ಒಳ_ಸರತಿ())
	    invoke_sound_callback();
# endif
# ifdef SIGUSR1
	ಆದರೆ (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
# endif
	ನಿಲ್ಲಿಸು;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    ಆದರೆ (entered == 1 && was_ಸುರಕ್ಷಿತ)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    ಆದರೆ (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}
#ಆದರೆ_ಅಂತ್ಯ


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    map_ಫಲಿತಾಂಶ_fail,    // failed, break loop
    map_ಫಲಿತಾಂಶ_get,     // get a character from typeahead
    map_ಫಲಿತಾಂಶ_retry,   // try to map again
    map_ಫಲಿತಾಂಶ_nomatch  // no matching mapping, get char
} map_ಫಲಿತಾಂಶ_T;

/*
 * Check if the bytes at the start of the typeahead buffer are a character used
 * in Insert mode completion.  This includes the form with a CTRL modifier.
 * During completion started by complete() keys are mapped as usual.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
at_ins_compl_ಕೀಲಿ(ಶೂನ್ಯ)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    ಪೂರ್ಣಾಂಕ	    c = *p;

    ಆದರೆ (typebuf.tb_len > 3
	    && (c == K_SPECIAL || c == CSI)  // CSI is used by the GUI
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_ಮುಖವಾಡ_CTRL)
	    // CTRL-SHIFT-N/P scroll the info popup, so they must not be folded
	    // to the CTRL-N/CTRL-P completion keys here.
	    && !(p[2] & MOD_ಮುಖವಾಡ_ಸರಿಸು))
	c = p[3] & 0x1f;
    ಹಿಂತಿರುಗಿ !ctrl_x_ಬಹುಲಕ_eval()
	    && ((ctrl_x_ಬಹುಲಕ_not_default() && vim_is_ctrl_x_ಕೀಲಿ(c))
		|| (compl_ಸ್ಥಿತಿ_local() && (c == Ctrl_N || c == Ctrl_P)));
}

/*
 * Check if typebuf.tb_buf[] contains a modifier plus key that can be changed
 * into just a key, apply that.
 * Check from typebuf.tb_buf[typebuf.tb_off] to typebuf.tb_buf[typebuf.tb_off
 * + "max_offset"].
 * Return the length of the replaced bytes, 0 if nothing changed, -1 for error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_simplify_modifier(ಪೂರ್ಣಾಂಕ max_ಸ್ಥಾನಾಂತರ)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;
    char_u	*tp;

    ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = 0; ಸ್ಥಾನಾಂತರ < max_ಸ್ಥಾನಾಂತರ; ++ಸ್ಥಾನಾಂತರ)
    {
	ಆದರೆ (ಸ್ಥಾನಾಂತರ + 3 >= typebuf.tb_len)
	    ನಿಲ್ಲಿಸು;
	tp = typebuf.tb_buf + typebuf.tb_off + ಸ್ಥಾನಾಂತರ;
	ಆದರೆ ((tp[0] == K_SPECIAL || tp[0] == CSI) && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    ಪೂರ್ಣಾಂಕ modifier = tp[2];
	    ಪೂರ್ಣಾಂಕ	c = tp[3];
	    ಪೂರ್ಣಾಂಕ new_c = ವಿಲೀನ_modify_Other_ಕೀಲಿಗಳು(c, &modifier);

	    ಆದರೆ (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		ಪೂರ್ಣಾಂಕ	len;
		ಪೂರ್ಣಾಂಕ	ಕೀಲಿ_ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ;

		ಆದರೆ (ಸ್ಥಾನಾಂತರ == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_ಮುಖವಾಡ = tp[2];
		}
		ಆದರೆ (IS_SPECIAL(new_c))
		{
		    new_string[0] = K_SPECIAL;
		    new_string[1] = K_ಎರಡನೇ(new_c);
		    new_string[2] = K_ಮೂರನೇ(new_c);
		    len = 3;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    len = mb_char2bytes(new_c, new_string);
		ಆದರೆ (modifier == 0)
		{
		    ಆದರೆ (put_string_ಒಳ_typebuf(ಸ್ಥಾನಾಂತರ, 4, new_string, len,
							ಖಾಲಿ, 0, ಖಾಲಿ) == FAIL)
			ಹಿಂತಿರುಗಿ -1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    tp[2] = modifier;
		    ಕೀಲಿ_ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ + 3;
		    ಆದರೆ (put_string_ಒಳ_typebuf(ಕೀಲಿ_ಸ್ಥಾನಾಂತರ, 1, new_string, len,
							ಖಾಲಿ, 0, ಖಾಲಿ) == FAIL)
			ಹಿಂತಿರುಗಿ -1;
		}

		// A mapping for the simplified key can be used for it even
		// when the key protocol is enabled.
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; ++i)
		    typebuf.tb_noremap[typebuf.tb_off + ಕೀಲಿ_ಸ್ಥಾನಾಂತರ + i]
							    |= RM_SIMPLIFIED;
		ಹಿಂತಿರುಗಿ len;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Return TRUE if the terminal sends modifiers with various keys.  This is when
 * modifyOtherKeys level 2 is enabled or the kitty keyboard protocol is
 * enabled.
 */
    ಪೂರ್ಣಾಂಕ
ಕೀಲಿ_protocol_enabled(ಶೂನ್ಯ)
{
    // If xterm has responded to XTQMODKEYS it overrules seenModifyOtherKeys.
    ಪೂರ್ಣಾಂಕ using_mok = modify_otherkeys_state != MOKS_INITIAL
			? modify_otherkeys_state == MOKS_ENABLED
			: ನೋಡಿದ_Modify_Other_ಕೀಲಿಗಳು;
    ಹಿಂತಿರುಗಿ using_mok || kitty_protocol_state == KKPS_ENABLED;
}

/*
 * Handle mappings in the typeahead buffer.
 * - When something was mapped, return map_result_retry for recursive mappings.
 * - When nothing mapped and typeahead has a character: return map_result_get.
 * - When there is no match yet, return map_result_nomatch, need to get more
 *   typeahead.
 * - On failure (out of memory) return map_result_fail.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_mapping(
	    ಪೂರ್ಣಾಂಕ *keylenp,
	    ಪೂರ್ಣಾಂಕ *timedout,
	    ಪೂರ್ಣಾಂಕ *mapdepth)
{
    mapblock_T	*mp = ಖಾಲಿ;
    mapblock_T	*mp2;
    mapblock_T	*mp_ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		mp_ಹೊಂದಿಕೆ_len = 0;
    ಪೂರ್ಣಾಂಕ		max_mlen = 0;
    ಪೂರ್ಣಾಂಕ		want_termcode = 0;  // 1 if termcode expected after max_mlen
    ಪೂರ್ಣಾಂಕ		tb_c1;
    ಪೂರ್ಣಾಂಕ		mlen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
    ಪೂರ್ಣಾಂಕ		nolmaplen;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		keylen = *keylenp;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		local_State = get_real_state();
    ಪೂರ್ಣಾಂಕ		is_plug_map = FALSE;
    bool	ಒಳ_osc = ಒಳ_osc_sequence(); // If we are in an OSC sequence,
					    // then go straight to
					    // check_termcode() in order to
					    // consume chars.

    // If typeahead starts with <Plug> then remap, even for a "noremap" mapping.
    ಆದರೆ (!ಒಳ_osc && typebuf.tb_len >= 3
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
    ಆದರೆ (!ಒಳ_osc && no_mapping == 0 && is_maphash_valid()
	    && (no_zero_mapping == 0 || tb_c1 != '0')
	    && (typebuf.tb_maplen == 0 || is_plug_map
		|| (p_remap
		    && (typebuf.tb_noremap[typebuf.tb_off]
				    & (RM_NONE|RM_ABBR)) == 0))
	    && !(p_paste && (State & (ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE)))
	    && !(State == ಬಹುಲಕ_HITRETURN && (tb_c1 == CAR || tb_c1 == ' '))
	    && State != ಬಹುಲಕ_ASKMORE
	    && State != ಬಹುಲಕ_CONFIRM
	    && !at_ins_compl_ಕೀಲಿ())
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
	ಆದರೆ (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    LANGMAP_ADJUST(tb_c1, (State & (ಬಹುಲಕ_CMDLINE | ಬಹುಲಕ_ಸೇರಿಸು)) == 0
					   && get_real_state() != ಬಹುಲಕ_SELECT);
	    nolmaplen = 0;
	}
#ಆದರೆ_ಅಂತ್ಯ
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	ಆದರೆ (mp == ಖಾಲಿ)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = ಖಾಲಿ;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_ಹೊಂದಿಕೆ = ಖಾಲಿ;
	mp_ಹೊಂದಿಕೆ_len = 0;
	ಗಾಗಿ ( ; mp != ಖಾಲಿ;
	       mp->m_ಮುಂದಿನ == ಖಾಲಿ ? (mp = mp2, mp2 = ಖಾಲಿ) : (mp = mp->m_ಮುಂದಿನ))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    ಆದರೆ (mp->m_ಕೀಲಿಗಳು[0] == tb_c1
		    && (mp->m_ಬಹುಲಕ & local_State)
		    && !(mp->m_simplified && ಕೀಲಿ_protocol_enabled()
			    && typebuf.tb_maplen == 0
			    && (typebuf.tb_noremap[typebuf.tb_off]
							& RM_SIMPLIFIED) == 0)
		    && ((mp->m_ಬಹುಲಕ & ಬಹುಲಕ_LANGMAP) == 0
						    || typebuf.tb_maplen == 0))
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
		ಪೂರ್ಣಾಂಕ	nomap = nolmaplen;
		ಪೂರ್ಣಾಂಕ	modifiers = 0;
#ಆದರೆ_ಅಂತ್ಯ
		// find the match length of this mapping
		ಗಾಗಿ (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
		    ಪೂರ್ಣಾಂಕ	c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
		    ಆದರೆ (nomap > 0)
		    {
			ಆದರೆ (nomap == 2 && c2 == KS_MODIFIER)
			    modifiers = 1;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nomap == 1 && modifiers == 1)
			    modifiers = c2;
			--nomap;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (c2 == K_SPECIAL)
			    nomap = 2;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಿಲೀನ_modify_Other_ಕೀಲಿಗಳು(c2, &modifiers) == c2)
			    // Only apply 'langmap' if merging modifiers into
			    // the key will not result in another character,
			    // so that 'langmap' behaves consistently in
			    // different terminals and GUIs.
			    LANGMAP_ADJUST(c2, TRUE);
			modifiers = 0;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (mp->m_ಕೀಲಿಗಳು[mlen] != c2)
			ನಿಲ್ಲಿಸು;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_ಕೀಲಿಗಳು;
		    char_u *p2 = mb_unescape(&p1);

		    ಆದರೆ (has_mbyte && p2 != ಖಾಲಿ
					&& MB_BYTE2LEN(tb_c1) > mb_ptr2len(p2))
			mlen = 0;
		}

		// Check an entry whether it matches.
		// - Full match: mlen == keylen
		// - Partly match: mlen == typebuf.tb_len
		keylen = mp->m_keylen;
		ಆದರೆ (mlen == keylen || (mlen == typebuf.tb_len
						   && typebuf.tb_len < keylen))
		{
		    char_u  *s;
		    ಪೂರ್ಣಾಂಕ	    n;

		    // If only script-local mappings are allowed, check if the
		    // mapping starts with K_SNR.
		    s = typebuf.tb_noremap + typebuf.tb_off;
		    ಆದರೆ ((*s & ~RM_SIMPLIFIED) == RM_SCRIPT
			    && (mp->m_ಕೀಲಿಗಳು[0] != K_SPECIAL
				|| mp->m_ಕೀಲಿಗಳು[1] != KS_EXTRA
				|| mp->m_ಕೀಲಿಗಳು[2] != KE_SNR))
			ಮುಂದುವರಿ;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    ಗಾಗಿ (n = mlen; --n >= 0; )
			ಆದರೆ (*s++ & (RM_NONE|RM_ABBR))
			    ನಿಲ್ಲಿಸು;
		    ಆದರೆ (!is_plug_map && n >= 0)
			ಮುಂದುವರಿ;

		    ಆದರೆ (keylen > typebuf.tb_len)
		    {
			ಆದರೆ (!*timedout && !(mp_ಹೊಂದಿಕೆ != ಖಾಲಿ
							&& mp_ಹೊಂದಿಕೆ->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (keylen > mp_ಹೊಂದಿಕೆ_len)
		    {
			// found a longer match
			mp_ಹೊಂದಿಕೆ = mp;
			mp_ಹೊಂದಿಕೆ_len = keylen;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // No match; may have to check for termcode at next
		    // character.

		    // If the first character that didn't match is
		    // K_SPECIAL then check for a termcode.  This isn't perfect
		    // but should work in most cases.
		    ಆದರೆ (max_mlen < mlen)
		    {
			max_mlen = mlen;
			want_termcode = mp->m_ಕೀಲಿಗಳು[mlen] == K_SPECIAL;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (max_mlen == mlen && mp->m_ಕೀಲಿಗಳು[mlen] == K_SPECIAL)
			want_termcode = 1;

		    // Check termcode for uppercase character to properly
		    // process "ESC[27;2;<ascii code>~" control sequences.
		    ಆದರೆ (ASCII_ISUPPER(mp->m_ಕೀಲಿಗಳು[mlen]))
			want_termcode = 1;
		}
	    }
	}

	// If no partly match found, use the longest full match.
	ಆದರೆ (keylen != KEYLEN_PART_MAP && mp_ಹೊಂದಿಕೆ != ಖಾಲಿ)
	{
	    mp = mp_ಹೊಂದಿಕೆ;
	    keylen = mp_ಹೊಂದಿಕೆ_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    ಆದರೆ (!ಒಳ_osc && *p_pt != NUL && mp == ಖಾಲಿ &&
	    (State & (ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_ಸಾಮಾನ್ಯ)))
    {
	ಗಾಗಿ (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    ಆದರೆ (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    ನಿಲ್ಲಿಸು;
	ಆದರೆ (p_pt[mlen] == NUL)	//ಹೊಂದಿಕೆ
	{
	    // write chars to script file(s)
	    ಆದರೆ (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"paste",
						      (ದೀರ್ಘ)!p_paste, ಖಾಲಿ, 0);
	    ಆದರೆ (!(State & ಬಹುಲಕ_ಸೇರಿಸು))
	    {
		ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
		ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
		ಸಂದೇಶ_clr_eos();		// clear ruler
	    }
	    ಸ್ಥಿತಿ_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    ಹಿಂತಿರುಗಿ map_ಫಲಿತಾಂಶ_retry;
	}
	// Need more chars for partly match.
	ಆದರೆ (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_ಕೀಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    // May check for a terminal code when there is no mapping or only a partial
    // mapping.  Also check if there is a full mapping with <Esc>, unless timed
    // out, since that is nearly always a partial match with a terminal code.
    ಆದರೆ (ಒಳ_osc || ((mp == ಖಾಲಿ || max_mlen + want_termcode > mp_ಹೊಂದಿಕೆ_len
		    || (mp_ಹೊಂದಿಕೆ_len == 1 && *mp->m_ಕೀಲಿಗಳು == ESC && !*timedout))
	    && keylen != KEYLEN_PART_MAP))
    {
	ಪೂರ್ಣಾಂಕ	save_keylen = keylen;

	/*
	 * When no matching mapping found or found a non-matching mapping that
	 * matches at least what the matching mapping matched:
	 * Check if we have a terminal code, when:
	 * - mapping is allowed,
	 * - keys have not been mapped,
	 * - and not an ESC sequence, not in insert mode or p_ek is on,
	 * - and when not timed out,
	 */
	ಆದರೆ (ಒಳ_osc || no_mapping == 0 || allow_ಕೀಲಿಗಳು != 0)
	{
	    ಆದರೆ (ಒಳ_osc || ((typebuf.tb_maplen == 0
			    || (p_remap && (typebuf.tb_noremap[typebuf.tb_off]
						& ~RM_SIMPLIFIED) == RM_YES))
		    && !*timedout))
		keylen = check_termcode(max_mlen + 1, ಖಾಲಿ, 0, ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
		keylen = 0;

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    ಆದರೆ (keylen == 0 && save_keylen == KEYLEN_PART_ಕೀಲಿ && !*timedout)
		keylen = KEYLEN_PART_ಕೀಲಿ;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    check_no_reduce_ಕೀಲಿಗಳು();  // may update the no_reduce_keys flag
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (keylen == 0 && !no_reduce_ಕೀಲಿಗಳು)
	    {
		keylen = check_simplify_modifier(max_mlen + 1);
		ಆದರೆ (keylen < 0)
		    // ins_typebuf() failed
		    ಹಿಂತಿರುಗಿ map_ಫಲಿತಾಂಶ_fail;
	    }

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    ಆದರೆ (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    keylen = 0;
	ಆದರೆ (keylen == 0)	// no matching terminal code
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
	    // check for window bounds report
	    ಆದರೆ (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		ಗಾಗಿ (s = typebuf.tb_buf + typebuf.tb_off + 1;
			   s < typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len
		   && (VIM_ISDIGIT(*s) || *s == ';' || *s == ' ');
			++s)
		    ;
		ಆದರೆ (*s == 'r' || *s == '|') // found one
		{
		    del_typebuf(
			  (ಪೂರ್ಣಾಂಕ)(s + 1 - (typebuf.tb_buf + typebuf.tb_off)), 0);
		    // get size and redraw screen
		    shell_resized();
		    *keylenp = keylen;
		    ಹಿಂತಿರುಗಿ map_ಫಲಿತಾಂಶ_retry;
		}
		ಆದರೆ (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_ಕೀಲಿ;
	    }
	    ಆದರೆ (keylen >= 0)
#ಆದರೆ_ಅಂತ್ಯ
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		ಆದರೆ (mp == ಖಾಲಿ)
		{
		    *keylenp = keylen;
		    ಹಿಂತಿರುಗಿ map_ಫಲಿತಾಂಶ_get;    // get character from typeahead
		}
	}

	ಆದರೆ (keylen > 0)	    // full matching terminal code
	{
#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_MENU)
	    ಆದರೆ (typebuf.tb_len >= 2
		    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
			      && typebuf.tb_buf[typebuf.tb_off + 1] == KS_MENU)
	    {
		ಪೂರ್ಣಾಂಕ	idx;

		// Using a menu may cause a break in undo!  It's like using
		// gotchars(), but without recording or writing to a script
		// file.
		may_sync_undo();
		del_typebuf(3, 0);
		idx = get_menu_ಸೂಚ್ಯಂಕ(current_menu, local_State);
		ಆದರೆ (idx != MENU_ಸೂಚ್ಯಂಕ_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && VIsual_select
					&& (current_menu->modes & ಬಹುಲಕ_VISUAL))
		    {
			VIsual_select = FALSE;
			(ಶೂನ್ಯ)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->ಅಕ್ಷರಸರಣಿಗಳು[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    ಹಿಂತಿರುಗಿ map_ಫಲಿತಾಂಶ_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	ಆದರೆ (mp == ಖಾಲಿ || keylen < 0)
	    keylen = KEYLEN_PART_ಕೀಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    keylen = mp_ಹೊಂದಿಕೆ_len;
    }

    /*
     * complete match
     */
    ಆದರೆ (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಪೂರ್ಣಾಂಕ	save_m_ಅಭಿವ್ಯಕ್ತಿ;
	ಪೂರ್ಣಾಂಕ	save_m_noremap;
	ಪೂರ್ಣಾಂಕ	save_m_silent;
	char_u	*save_m_ಕೀಲಿಗಳು;
	char_u	*save_alt_m_ಕೀಲಿಗಳು;
	ಪೂರ್ಣಾಂಕ	save_alt_m_keylen;
#ಇಲ್ಲದಿದ್ದರೆ
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#ಆದರೆ_ಅಂತ್ಯ

	// write chars to script file(s)
	ಆದರೆ (keylen > typebuf.tb_maplen)
	    gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						   keylen - typebuf.tb_maplen);

	cmd_silent = (typebuf.tb_silent > 0);
	del_typebuf(keylen, 0);	// remove the mapped keys

	/*
	 * Put the replacement string in front of mapstr.
	 * The depth check catches ":map x y" and ":map y x".
	 */
	ಆದರೆ (++*mapdepth >= p_mmd)
	{
	    emsg(_(e_recursive_mapping));
	    ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
		redrawcmdline();
	    ಇಲ್ಲದಿದ್ದರೆ
		setcursor();
	    flush_buffers(FLUSH_ಕನಿಷ್ಠತಮ);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    ಹಿಂತಿರುಗಿ map_ಫಲಿತಾಂಶ_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	ಆದರೆ (VIsual_ಸಕ್ರಿಯ && VIsual_select && (mp->m_ಬಹುಲಕ & ಬಹುಲಕ_VISUAL))
	{
	    VIsual_select = FALSE;
	    (ಶೂನ್ಯ)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_ಅಭಿವ್ಯಕ್ತಿ = mp->m_ಅಭಿವ್ಯಕ್ತಿ;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_ಕೀಲಿಗಳು = ಖಾಲಿ;  // only saved when needed
	save_alt_m_ಕೀಲಿಗಳು = ಖಾಲಿ;  // only saved when needed
	save_alt_m_keylen = mp->m_alt != ಖಾಲಿ ? mp->m_alt->m_keylen : 0;

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	ಆದರೆ (mp->m_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    ಪೂರ್ಣಾಂಕ save_vgetc_busy = vgetc_busy;
	    ಪೂರ್ಣಾಂಕ save_may_garbage_collect = may_garbage_collect;
	    ಪೂರ್ಣಾಂಕ was_screen_ಲಂಬಸಾಲು = screen_cur_ಲಂಬಸಾಲು;
	    ಪೂರ್ಣಾಂಕ was_screen_ಅಡ್ಡಸಾಲು = screen_cur_ಅಡ್ಡಸಾಲು;
	    ಪೂರ್ಣಾಂಕ prev_did_emsg = did_emsg;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_ಕೀಲಿಗಳು = vim_strnsave(mp->m_ಕೀಲಿಗಳು, (size_t)mp->m_keylen);
	    save_alt_m_ಕೀಲಿಗಳು = mp->m_alt != ಖಾಲಿ
				    ? vim_strnsave(mp->m_alt->m_ಕೀಲಿಗಳು,
					     (size_t)save_alt_m_keylen) : ಖಾಲಿ;
	    map_str = eval_map_ಅಭಿವ್ಯಕ್ತಿ(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_ಅಡ್ಡಸಾಲು, was_screen_ಲಂಬಸಾಲು);
	    ಹೊರಹೋಗು_flush();

	    // If an error was displayed and the expression returns an empty
	    // string, generate a <Nop> to allow for a redraw.
	    ಆದರೆ (prev_did_emsg != did_emsg
				       && (map_str == ಖಾಲಿ || *map_str == NUL))
	    {
		char_u	buf[4];

		vim_free(map_str);
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = KE_IGNORE;
		buf[3] = NUL;
		map_str = vim_strnsave(buf, 3);
		ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
		{
		    // redraw the command below the error
		    ಸಂದೇಶ_didout = TRUE;
		    ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು < cmdline_ಅಡ್ಡಸಾಲು)
			ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;
		    redrawcmd();
		}
	    }

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	ಆದರೆ (map_str == ಖಾಲಿ)
	    i = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ noremap;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಕೊನೆಯ_ಬಳಸಿದ_map = mp;
	    ಕೊನೆಯ_ಬಳಸಿದ_sid = -1;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		save_m_ಅಭಿವ್ಯಕ್ತಿ ?
		(save_m_ಕೀಲಿಗಳು != ಖಾಲಿ
			&& STRNCMP(map_str, save_m_ಕೀಲಿಗಳು, (size_t)keylen) == 0)
		|| (save_alt_m_ಕೀಲಿಗಳು != ಖಾಲಿ
			&& STRNCMP(map_str, save_alt_m_ಕೀಲಿಗಳು,
					    (size_t)save_alt_m_keylen) == 0) :
#ಆದರೆ_ಅಂತ್ಯ
		STRNCMP(map_str, mp->m_ಕೀಲಿಗಳು, (size_t)keylen) == 0
		|| (mp->m_alt != ಖಾಲಿ
			&& STRNCMP(map_str, mp->m_alt->m_ಕೀಲಿಗಳು,
					    (size_t)mp->m_alt->m_keylen) == 0))
		noremap = REMAP_SKIP;
	    ಇಲ್ಲದಿದ್ದರೆ
		noremap = REMAP_YES;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (save_m_ಅಭಿವ್ಯಕ್ತಿ)
		vim_free(map_str);
#ಆದರೆ_ಅಂತ್ಯ
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	vim_free(save_m_ಕೀಲಿಗಳು);
	vim_free(save_alt_m_ಕೀಲಿಗಳು);
#ಆದರೆ_ಅಂತ್ಯ
	*keylenp = keylen;
	ಆದರೆ (i == FAIL)
	    ಹಿಂತಿರುಗಿ map_ಫಲಿತಾಂಶ_fail;
	ಹಿಂತಿರುಗಿ map_ಫಲಿತಾಂಶ_retry;
    }

    *keylenp = keylen;
    ಹಿಂತಿರುಗಿ map_ಫಲಿತಾಂಶ_nomatch;
}

/*
 * unget one character (can only be done once!)
 * If the character was stuffed, vgetc() will get it next time it is called.
 * Otherwise vgetc() will only get it when the stuff buffer is empty.
 */
    ಶೂನ್ಯ
vungetc(ಪೂರ್ಣಾಂಕ c)
{
    old_char = c;
    old_mod_ಮುಖವಾಡ = mod_ಮುಖವಾಡ;
    old_mouse_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    old_mouse_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
    old_ಕೀಲಿ_Stuffed = ಕೀಲಿ_Stuffed;
}

/*
 * When peeking and not getting a character, reg_executing cannot be cleared
 * yet, so set a flag to clear it later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_ಕೊನೆ_reg_executing(ಪೂರ್ಣಾಂಕ advance)
{
    ಆದರೆ (reg_executing != 0 && (typebuf.tb_maplen == 0
						|| pending_ಕೊನೆ_reg_executing))
    {
	ಆದರೆ (advance)
	{
	    reg_executing = 0;
	    pending_ಕೊನೆ_reg_executing = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    pending_ಕೊನೆ_reg_executing = TRUE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vgetorpeek(ಪೂರ್ಣಾಂಕ advance)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		timedout = FALSE;	// waited for more than 'timeoutlen'
					// for mapping to complete or
					// 'ttimeoutlen' for complete key code
    ಪೂರ್ಣಾಂಕ		mapdepth = 0;		// check for recursive mapping
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ_deleted = FALSE;   // set when mode has been deleted
    ಪೂರ್ಣಾಂಕ		new_wcol, new_wrow;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಪೂರ್ಣಾಂಕ		shape_ಬದಲಾಯಿತು = FALSE;  // adjusted cursor shape
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		old_wcol, old_wrow;
    ಪೂರ್ಣಾಂಕ		wait_tb_len;

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
    ಆದರೆ (vgetc_busy > 0 && ex_ಸಾಮಾನ್ಯ_busy == 0)
	ಹಿಂತಿರುಗಿ NUL;

    ++vgetc_busy;

    ಆದರೆ (advance)
    {
	ಕೀಲಿ_Stuffed = FALSE;
	typebuf_was_ಖಾಲಿಗಳು = FALSE;
    }

    init_typebuf();
    ಆರಂಭ_stuff();
    check_ಕೊನೆ_reg_executing(advance);
    ಮಾಡು
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	ಆದರೆ (typeahead_char != 0)
	{
	    c = typeahead_char;
	    ಆದರೆ (advance)
		typeahead_char = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    c = read_readbuffers(advance);
	ಆದರೆ (c != NUL && !got_int)
	{
	    ಆದರೆ (advance)
	    {
		// KeyTyped = FALSE;  When the command that stuffed something
		// was typed, behave like the stuffed command was typed.
		// needed for CTRL-W CTRL-] to open a fold, for example.
		ಕೀಲಿ_Stuffed = TRUE;
	    }
	    ಆದರೆ (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	// no abbreviations now
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    ಗಾಗಿ (;;)
	    {
		ದೀರ್ಘ	wait_ಸಮಯ;
		ಪೂರ್ಣಾಂಕ	keylen = 0;
		ಪೂರ್ಣಾಂಕ	showcmd_idx;
		check_ಕೊನೆ_reg_executing(advance);
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		ಆದರೆ (typebuf.tb_maplen)
		    ಸಾಲು_breakcheck();
		ಇಲ್ಲದಿದ್ದರೆ
		    ui_breakcheck();		// check for CTRL-C
		ಆದರೆ (got_int)
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
		    ಆದರೆ ((c || typebuf.tb_maplen)
				     && (State & (ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE)))
			c = ESC;
		    ಇಲ್ಲದಿದ್ದರೆ
			c = Ctrl_C;
		    flush_buffers(FLUSH_ಒಳಹರಿವು);	// flush all typeahead

		    ಆದರೆ (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_ಫಲಿತಾಂಶ_T ಫಲಿತಾಂಶ = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    ಆದರೆ (ಫಲಿತಾಂಶ == map_ಫಲಿತಾಂಶ_retry)
			// try mapping again
			ಮುಂದುವರಿ;
		    ಆದರೆ (ಫಲಿತಾಂಶ == map_ಫಲಿತಾಂಶ_fail)
		    {
			// failed, use the outer loop
			c = -1;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ಫಲಿತಾಂಶ == map_ಫಲಿತಾಂಶ_get)
		    {
/*
 * get a character: 2. from the typeahead buffer
 */
			c = typebuf.tb_buf[typebuf.tb_off];
			ಆದರೆ (advance)	// remove chars from typebuf
			{
			    cmd_silent = (typebuf.tb_silent > 0);
			    ಆದರೆ (typebuf.tb_maplen > 0)
				ಕೀಲಿ_Typed = FALSE;
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				ಕೀಲಿ_Typed = TRUE;
				// write char to script file(s)
				gotchars(typebuf.tb_buf
						 + typebuf.tb_off, 1);
			    }
			    ಕೀಲಿ_Noremap = typebuf.tb_noremap[typebuf.tb_off]
							    & ~RM_SIMPLIFIED;
			    del_typebuf(1, 0);
			}
			ನಿಲ್ಲಿಸು;  // got character, break the for loop
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
		ಆದರೆ (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& kitty_protocol_state != KKPS_ENABLED
			&& ex_ಸಾಮಾನ್ಯ_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & ಬಹುಲಕ_ಸೇರಿಸು)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_ಕೀಲಿ && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	ಲಂಬಸಾಲು = 0;
		    char_u	*ptr;

		    ಆದರೆ (ಬಹುಲಕ_displayed)
		    {
			unshowmode(TRUE);
			ಬಹುಲಕ_deleted = TRUE;
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    // may show a different cursor shape
		    ಆದರೆ (gui.ಒಳ_use && State != ಬಹುಲಕ_ಸಾಮಾನ್ಯ && !cmd_silent)
		    {
			ಪೂರ್ಣಾಂಕ	    save_State;

			save_State = State;
			State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
			gui_update_cursor(TRUE, FALSE);
			State = save_State;
			shape_ಬದಲಾಯಿತು = TRUE;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		    validate_cursor();
		    old_wcol = curwin->w_wcol;
		    old_wrow = curwin->w_wrow;

		    // move cursor left, if possible
		    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು != 0)
		    {
			ಆದರೆ (curwin->w_wcol > 0)
			{
			    // After auto-indenting and no text is following,
			    // we are expecting to truncate the trailing
			    // white-space, so find the last non-white
			    // character -- webb
			    ಆದರೆ (did_ai && *skipwhite(ml_get_curline()
						+ curwin->w_cursor.ಲಂಬಸಾಲು) == NUL)
			    {
				chartabsize_T cts;

				curwin->w_wcol = 0;
				ptr = ml_get_curline();
				init_chartabsize_arg(&cts, curwin,
					  curwin->w_cursor.lnum, 0, ptr, ptr);
				ಆಗ (cts.cts_ptr < ptr + curwin->w_cursor.ಲಂಬಸಾಲು)
				{
				    ಆದರೆ (!VIM_ISWHITE(*cts.cts_ptr))
					curwin->w_wcol = cts.cts_vcol;
				    cts.cts_vcol += lbr_chartabsize(&cts);
				    ಆದರೆ (has_mbyte)
					cts.cts_ptr +=
						   (*mb_ptr2len)(cts.cts_ptr);
				    ಇಲ್ಲದಿದ್ದರೆ
					++cts.cts_ptr;
				}
				clear_chartabsize_arg(&cts);

				curwin->w_wrow = curwin->w_cline_ಅಡ್ಡಸಾಲು
					   + curwin->w_wcol / curwin->w_ಅಗಲ;
				curwin->w_wcol %= curwin->w_ಅಗಲ;
				curwin->w_wcol += curwin_ಲಂಬಸಾಲು_off();
				ಲಂಬಸಾಲು = 0;	// no correction needed
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				--curwin->w_wcol;
				ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು - 1;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_ಅಗಲ - 1;
			    ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು - 1;
			}
			ಆದರೆ (has_mbyte && ಲಂಬಸಾಲು > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು);
			    ಆದರೆ ((*mb_ptr2cells)(ptr + ಲಂಬಸಾಲು) > 1)
				--curwin->w_wcol;
			}
		    }
		    setcursor();
		    ಹೊರಹೋಗು_flush();
		    new_wcol = curwin->w_wcol;
		    new_wrow = curwin->w_wrow;
		    curwin->w_wcol = old_wcol;
		    curwin->w_wrow = old_wrow;
		}
		ಆದರೆ (c < 0)
		    ಮುಂದುವರಿ;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		ಗಾಗಿ (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		ಆದರೆ (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    ಮುಂದುವರಿ;
		}

		ಆದರೆ (ex_ಸಾಮಾನ್ಯ_busy > 0)
		{
		    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tc = 0;

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    ಆದರೆ (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			ಮುಂದುವರಿ;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    ಆದರೆ (p_im && (State & ಬಹುಲಕ_ಸೇರಿಸು))
			c = Ctrl_L;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (terminal_is_ಸಕ್ರಿಯ())
			c = K_CANCEL;
#ಆದರೆ_ಅಂತ್ಯ
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE)
					     || (cmdwin_type > 0 && tc == ESC))
			c = Ctrl_C;
		    ಇಲ್ಲದಿದ್ದರೆ
			c = ESC;
		    tc = c;
		    // set a flag to indicate this wasn't a normal char
		    ಆದರೆ (advance)
			typebuf_was_ಖಾಲಿಗಳು = TRUE;

		    // return from main_loop()
		    ಆದರೆ (pending_exmode_ಸಕ್ರಿಯ)
			exmode_ಸಕ್ರಿಯ = EXMODE_ಸಾಮಾನ್ಯ;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    ನಿಲ್ಲಿಸು;
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
		ಆದರೆ (((State & ಬಹುಲಕ_ಸೇರಿಸು) != 0 || p_lz)
			&& (State & ಬಹುಲಕ_CMDLINE) == 0
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
		ಪೂರ್ಣಾಂಕ showing_partial = FALSE;
		ಆದರೆ (typebuf.tb_len > 0 && advance && !exmode_ಸಕ್ರಿಯ)
		{
		    ಆದರೆ (((State & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_ಸೇರಿಸು))
						      || State == ಬಹುಲಕ_LANGMAP)
			    && State != ಬಹುಲಕ_HITRETURN)
		    {
			// this looks nice when typing a dead character map
			ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು
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
			ತಳ್ಳು_showcmd();
			ಆದರೆ (typebuf.tb_len > SHOWCMD_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
			ಆಗ (showcmd_idx < typebuf.tb_len)
			    ಕೂಡಿಸು_byte_ಗಮ್ಯ_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
		    }

		    // This looks nice when typing a dead character map.
		    // There is no actual command line for get_number().
		    ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE)
			    && get_cmdline_info()->cmdbuff != ಖಾಲಿ
#ಆದರೆ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
			    && cmdline_ನಕ್ಷತ್ರ == 0
#ಆದರೆ_ಅಂತ್ಯ
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
		ಆದರೆ (typebuf.tb_len == 0)
		    // timedout may have been set if a mapping with empty RHS
		    // fully matched while longer mappings timed out.
		    timedout = FALSE;

		ಆದರೆ (advance)
		{
		    ಆದರೆ (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_ಕೀಲಿ)))
			// blocking wait
			wait_ಸಮಯ = -1L;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (keylen == KEYLEN_PART_ಕೀಲಿ && p_ttm >= 0)
			wait_ಸಮಯ = p_ttm;
		    ಇಲ್ಲದಿದ್ದರೆ
			wait_ಸಮಯ = p_tm;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    wait_ಸಮಯ = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_ಸಮಯ);

		ಆದರೆ (showcmd_idx != 0)
		    ತೆಗೆ_showcmd();
		ಆದರೆ (showing_partial)
		{
		    ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
			edit_unputchar();
		    ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE)
					&& get_cmdline_info()->cmdbuff != ಖಾಲಿ)
			unputcmdline();
		    ಇಲ್ಲದಿದ್ದರೆ
			setcursor();	// put cursor back where it belongs
		}

		ಆದರೆ (c < 0)
		    ಮುಂದುವರಿ;		// end of input script reached
		ಆದರೆ (c == NUL)		// no character available
		{
		    ಆದರೆ (!advance)
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			ಮುಂದುವರಿ;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{	    // allow mapping for just typed characters
		    ಆಗ (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_ಸಕ್ರಿಯ = im_get_ಸ್ಥಿತಿ();
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } ಆಗ ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    ಆದರೆ (advance && p_smd && ಸಂದೇಶ_silent == 0 && (State & ಬಹುಲಕ_ಸೇರಿಸು))
    {
	ಆದರೆ (c == ESC && !ಬಹುಲಕ_deleted && !no_mapping && ಬಹುಲಕ_displayed)
	{
	    ಆದರೆ (typebuf.tb_len && !ಕೀಲಿ_Typed)
		redraw_cmdline = TRUE;	    // delete mode later
	    ಇಲ್ಲದಿದ್ದರೆ
		unshowmode(FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c != ESC && ಬಹುಲಕ_deleted)
	{
	    ಆದರೆ (typebuf.tb_len && !ಕೀಲಿ_Typed)
		redraw_cmdline = TRUE;	    // show mode later
	    ಇಲ್ಲದಿದ್ದರೆ
		showmode();
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // may unshow different cursor shape
    ಆದರೆ (gui.ಒಳ_use && shape_ಬದಲಾಯಿತು)
	gui_update_cursor(TRUE, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (timedout && c == ESC)
    {
	// When recording there will be no timeout.  Add an <Ignore> after the
	// ESC to avoid that it forms a key code with following characters.
	gotchars_ignore();
    }

    --vgetc_busy;

    ಹಿಂತಿರುಗಿ c;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
inchar(
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		maxlen,
    ದೀರ್ಘ	wait_ಸಮಯ)	    // milliseconds
{
    ಪೂರ್ಣಾಂಕ		len = 0;	    // init for GCC
    ಪೂರ್ಣಾಂಕ		retesc = FALSE;	    // return ESC with gotint
    ಪೂರ್ಣಾಂಕ		script_char;
    ಪೂರ್ಣಾಂಕ		tb_change_cnt = typebuf.tb_change_cnt;

    ಆದರೆ (wait_ಸಮಯ == -1L || wait_ಸಮಯ > 100L)  // flush output before waiting
    {
	cursor_on();
	ಹೊರಹೋಗು_flush_cursor(FALSE, FALSE);
#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	ಆದರೆ (gui.ಒಳ_use && postponed_mouseshape)
	    update_mouseshape(-1);
#ಆದರೆ_ಅಂತ್ಯ
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    ಆದರೆ (State != ಬಹುಲಕ_HITRETURN)
    {
	did_outofmem_ಸಂದೇಶ = FALSE;   // display out of memory message (again)
	did_swapwrite_ಸಂದೇಶ = FALSE;  // display swap file write error again
    }
    undo_off = FALSE;		    // restart undo now

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    ಆಗ (scriptin[curscript] != ಖಾಲಿ && script_char < 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && !ignore_script
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
	parse_queued_messages();
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (got_int || (script_char = getc(scriptin[curscript])) < 0)
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
	    ಆದರೆ (got_int)
		retesc = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    ಆದರೆ (script_char < 0)	// did not get a character from script
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	ಆದರೆ (got_int)
	{
#ವ್ಯಾಖ್ಯಾನಿಸು DUM_LEN (MAXMAPLEN * 3 + 3)
	    char_u	dum[DUM_LEN + 1];

	    ಗಾಗಿ (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		ಆದರೆ (len == 0 || (len == 1 && dum[0] == Ctrl_C))
		    ನಿಲ್ಲಿಸು;
	    }
	    ಹಿಂತಿರುಗಿ retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	ಆದರೆ (wait_ಸಮಯ == -1L || wait_ಸಮಯ > 10L)
	    ಹೊರಹೋಗು_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_ಸಮಯ, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    ಆದರೆ (typebuf_ಬದಲಾಯಿತು(tb_change_cnt))
	ಹಿಂತಿರುಗಿ 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    ಆದರೆ (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    ಹಿಂತಿರುಗಿ fix_ಒಳಹರಿವು_ಬಫರ್(buf, len);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * "buf[]" must have room to triple the number of bytes!
 * Returns the new length.
 */
    ಪೂರ್ಣಾಂಕ
fix_ಒಳಹರಿವು_ಬಫರ್(char_u *buf, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p = buf;

    /*
     * Two characters are special: NUL and K_SPECIAL.
     * When compiled With the GUI CSI is also special.
     * Replace	     NUL by K_SPECIAL KS_ZERO	 KE_FILLER
     * Replace K_SPECIAL by K_SPECIAL KS_SPECIAL KE_FILLER
     * Replace       CSI by K_SPECIAL KS_EXTRA   KE_CSI
     */
    ಗಾಗಿ (i = len; --i >= 0; ++p)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	// When the GUI is used any character can come after a CSI, don't
	// escape it.
	ಆದರೆ (gui.ಒಳ_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
# ifndef MSWIN
	// When not on MS-Windows and the GUI is not used CSI needs to be
	// escaped.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!gui.ಒಳ_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (ಪೂರ್ಣಾಂಕ)KE_CSI;
	    len += 2;
	}
# endif
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (p[0] == NUL || (p[0] == K_SPECIAL
		    // timeout may generate K_CURSORHOLD,
		    // 'autocompletedelay' timeout K_COMPLETE_DELAY
		    && (i < 2 || p[1] != KS_EXTRA
			|| (p[2] != (ಪೂರ್ಣಾಂಕ)KE_CURSORHOLD
			    && p[2] != (ಪೂರ್ಣಾಂಕ)KE_COMPLETE_DELAY))
#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
		    // Win32 console passes modifiers
		    && (
# ifdef VIMDLL
			gui.ಒಳ_use ||
# endif
			(i < 2 || p[1] != KS_MODIFIER))
#ಆದರೆ_ಅಂತ್ಯ
		    ))
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    p[2] = K_ಮೂರನೇ(p[0]);
	    p[1] = K_ಎರಡನೇ(p[0]);
	    p[0] = K_SPECIAL;
	    p += 2;
	    len += 2;
	}
    }
    *p = NUL;		// add trailing NUL
    ಹಿಂತಿರುಗಿ len;
}

#ಆದರೆ defined(USE_ಒಳಹರಿವು_BUF)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    ಪೂರ್ಣಾಂಕ
ಒಳಹರಿವು_ಲಭ್ಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (!vim_is_ಒಳಹರಿವು_buf_ಖಾಲಿಗಳು()
# ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Function passed to do_cmdline() to get the command after a <Cmd> key from
 * typeahead.
 */
    ಸ್ಥಿರ char_u *
getcmdkeycmd(
	ಪೂರ್ಣಾಂಕ		promptc ಬಳಸದ,
	ಶೂನ್ಯ		*cookie ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ		indent ಬಳಸದ,
	getline_opt_T	do_concat ಬಳಸದ)
{
    garray_T	ಸಾಲು_ga;
    ಪೂರ್ಣಾಂಕ		c1 = -1;
    ಪೂರ್ಣಾಂಕ		c2;
    ಪೂರ್ಣಾಂಕ		cmod = 0;
    ಪೂರ್ಣಾಂಕ		aborted = FALSE;

    ga_init2(&ಸಾಲು_ga, 1, 32);

    // no mapping for these characters
    no_mapping++;

    got_int = FALSE;
    ಆಗ (c1 != NUL && !aborted)
    {
	ಆದರೆ (ga_grow(&ಸಾಲು_ga, 32) == FAIL)
	{
	    aborted = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (vgetorpeek(FALSE) == NUL)
	{
	    // incomplete <Cmd> is an error, because there is not much the user
	    // could do in this state.
	    emsg(_(e_cmd_mapping_must_ಕೊನೆ_with_cr));
	    aborted = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	// Get one character at a time.
	c1 = vgetorpeek(TRUE);

	// Get two extra bytes for special keys
	ಆದರೆ (c1 == K_SPECIAL)
	{
	    c1 = vgetorpeek(TRUE);
	    c2 = vgetorpeek(TRUE);
	    ಆದರೆ (c1 == KS_MODIFIER)
	    {
		cmod = c2;
		ಮುಂದುವರಿ;
	    }
	    c1 = ಗಮ್ಯ_SPECIAL(c1, c2);

	    // K_ESC is used to avoid ambiguity with the single Esc character
	    // that might be the start of an escape sequence.  Convert it back
	    // to a single Esc here.
	    ಆದರೆ (c1 == K_ESC)
		c1 = ESC;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    // Translate K_CSI to CSI.  The special key is only used to
	    // avoid it being recognized as the start of a special key.
	    ಆದರೆ (c1 == K_CSI)
		c1 = CSI;
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ (got_int)
	    aborted = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c1 == '\r' || c1 == '\n')
	    c1 = NUL;  // end the line
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c1 == ESC)
	    aborted = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c1 == K_COMMAND || c1 == K_SCRIPT_COMMAND)
	{
	    // give a nicer error message for this special case
	    emsg(_(e_cmd_mapping_must_ಕೊನೆ_with_cr_before_ಎರಡನೇ_cmd));
	    aborted = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c1 == K_SNR)
	{
	    GA_CONCAT_LITERAL(&ಸಾಲು_ga, "<SNR>");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (cmod != 0)
	    {
		ga_append(&ಸಾಲು_ga, K_SPECIAL);
		ga_append(&ಸಾಲು_ga, KS_MODIFIER);
		ga_append(&ಸಾಲು_ga, cmod);
	    }
	    ಆದರೆ (IS_SPECIAL(c1))
	    {
		ga_append(&ಸಾಲು_ga, K_SPECIAL);
		ga_append(&ಸಾಲು_ga, K_ಎರಡನೇ(c1));
		ga_append(&ಸಾಲು_ga, K_ಮೂರನೇ(c1));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ga_append(&ಸಾಲು_ga, c1);
	}

	cmod = 0;
    }

    no_mapping--;

    ಆದರೆ (aborted)
	ga_clear(&ಸಾಲು_ga);

    ಹಿಂತಿರುಗಿ (char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * If there was a mapping we get its SID.  Otherwise, use "last_used_sid", it
 * is set when redo'ing.
 * Put this SID in the redo buffer, so that "." will use the same script
 * context.
 */
    ಶೂನ್ಯ
may_ಕೂಡಿಸು_ಕೊನೆಯ_ಬಳಸಿದ_map_ಗಮ್ಯ_redobuff(ಶೂನ್ಯ)
{
    char_u  buf[3 + 20];
    ಪೂರ್ಣಾಂಕ	    buflen;
    ಪೂರ್ಣಾಂಕ	    sid = -1;

    ಆದರೆ (ಕೊನೆಯ_ಬಳಸಿದ_map != ಖಾಲಿ)
	sid = ಕೊನೆಯ_ಬಳಸಿದ_map->m_script_ctx.sc_sid;
    ಆದರೆ (sid < 0)
	sid = ಕೊನೆಯ_ಬಳಸಿದ_sid;

    ಆದರೆ (sid < 0)
	ಹಿಂತಿರುಗಿ;

    // <K_SID>{nr};
    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = KE_SID;
    buflen = 3;

    buflen += vim_snprintf((ಅಕ್ಷರ *)buf + 3, 20, "%d;", sid);
    ಕೂಡಿಸು_buff(&redobuff, buf, (ದೀರ್ಘ)buflen);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
do_cmdkey_command(ಪೂರ್ಣಾಂಕ ಕೀಲಿ ಬಳಸದ, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ	    res;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    ಆದರೆ (ಕೀಲಿ == K_SCRIPT_COMMAND
		  && (ಕೊನೆಯ_ಬಳಸಿದ_map != ಖಾಲಿ || SCRIPT_ID_VALID(ಕೊನೆಯ_ಬಳಸಿದ_sid)))
    {
	save_current_sctx = current_sctx;
	ಆದರೆ (ಕೊನೆಯ_ಬಳಸಿದ_map != ಖಾಲಿ)
	    current_sctx = ಕೊನೆಯ_ಬಳಸಿದ_map->m_script_ctx;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    current_sctx.sc_sid = ಕೊನೆಯ_ಬಳಸಿದ_sid;
	    current_sctx.sc_lnum = 0;
	    current_sctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಅಂಶ(ಕೊನೆಯ_ಬಳಸಿದ_sid)->sn_ಆವೃತ್ತಿ;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    res = do_cmdline(ಖಾಲಿ, getcmdkeycmd, ಖಾಲಿ, flags);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ res;
}

#ಆದರೆ defined(FEAT_EVAL)
    ಶೂನ್ಯ
reset_ಕೊನೆಯ_ಬಳಸಿದ_map(mapblock_T *mp)
{
    ಆದರೆ (ಕೊನೆಯ_ಬಳಸಿದ_map != mp)
	ಹಿಂತಿರುಗಿ;

    ಕೊನೆಯ_ಬಳಸಿದ_map = ಖಾಲಿ;
    ಕೊನೆಯ_ಬಳಸಿದ_sid = -1;
}
#ಆದರೆ_ಅಂತ್ಯ
