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

#تعریف_کرو اقل_سائز 20			// minimal size for b_str

ساکن buffheader_T redobuff = {{تہی, 0, {NUL}}, تہی, 0, 0, FALSE};
ساکن buffheader_T old_redobuff = {{تہی, 0, {NUL}}, تہی, 0, 0, FALSE};
ساکن buffheader_T recordbuff = {{تہی, 0, {NUL}}, تہی, 0, 0, FALSE};

ساکن صحیح typeahead_char = 0;		// typeahead char that's not flushed

#اگر_تعریف_شدہ FEAT_EVAL
ساکن char_u typedchars[MAXMAPLEN + 1] = { NUL };  // typed chars before map
ساکن صحیح typedchars_pos = 0;
#اگر_ختم

/*
 * When block_redo is TRUE the redo buffer will not be changed.
 * Used by edit() to repeat insertions.
 */
ساکن صحیح	بلاک_redo = FALSE;

ساکن صحیح	کلید_Noremap = 0;	    // remapping flags

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
#تعریف_کرو RM_YES		0	// tb_noremap: remap
#تعریف_کرو RM_NONE		1	// tb_noremap: don't remap
#تعریف_کرو RM_SCRIPT	2	// tb_noremap: remap local script mappings
#تعریف_کرو RM_ABBR		4	// tb_noremap: don't remap, do abbrev.
#تعریف_کرو RM_SIMPLIFIED	8	// tb_noremap: modifiers merged into the key

// typebuf.tb_buf has three parts: room in front (for result of mappings), the
// middle for typeahead and room for new characters (which needs to be 3 *
// MAXMAPLEN for the Amiga).
#اگر_تعریف_شدہ FEAT_معمول
// Add extra space to handle large OSC responses in bigger chunks (improve
// performance)
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3) + 2048)
#ورنہ // Tiny version
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
#اگر_ختم
ساکن char_u	typebuf_init[TYPELEN_INIT];	// initial typebuf.tb_buf
ساکن char_u	noremapbuf_init[TYPELEN_INIT];	// initial typebuf.tb_noremap

ساکن size_t	آخری_recorded_len = 0;	// number of last recorded chars

#اگر_تعریف_شدہ FEAT_EVAL
mapblock_T	*آخری_مستعمل_map = تہی;
صحیح		آخری_مستعمل_sid = -1;
#اگر_ختم

ساکن صحیح	read_readbuf(buffheader_T *buf, صحیح advance);
ساکن خالی	init_typebuf(خالی);
ساکن خالی	may_sync_undo(خالی);
ساکن خالی	free_typebuf(خالی);
ساکن خالی	closescript(خالی);
ساکن خالی	updatescript(صحیح c);
ساکن صحیح	vgetorpeek(صحیح);
ساکن صحیح	inchar(char_u *buf, صحیح maxlen, طویل wait_وقت);
#اگر_تعریف_شدہ FEAT_EVAL
ساکن صحیح	do_کلید_داخلہ_pre(صحیح c);
#اگر_ختم

/*
 * Free and clear a buffer.
 */
    ساکن خالی
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    برائے (p = buf->bh_پہلا.b_اگلا; p != تہی; p = np)
    {
	np = p->b_اگلا;
	vim_free(p);
    }
    buf->bh_پہلا.b_اگلا = تہی;
    buf->bh_curr = تہی;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    ساکن char_u *
get_buffcont(
    buffheader_T	*بفر,
    صحیح			dozero,	    // count == zero is not an error
    size_t		*len)	    // the length of the returned buffer
{
    long_u	    گنتی = 0;
    char_u	    *p = تہی;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;
    size_t	    i = 0;

    // compute the total length of the string
    برائے (bp = بفر->bh_پہلا.b_اگلا; bp != تہی; bp = bp->b_اگلا)
	گنتی += (long_u)bp->b_strlen;

    اگر ((گنتی > 0 || dozero) && (p = alloc(گنتی + 1)) != تہی)
    {
	p2 = p;
	برائے (bp = بفر->bh_پہلا.b_اگلا; bp != تہی; bp = bp->b_اگلا)
	    برائے (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
	i = (size_t)(p2 - p);
    }

    اگر (len != تہی)
	*len = i;

    واپس p;
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(خالی)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE, &len);
    اگر (p == تہی)
	واپس تہی;

    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    اگر (len >= آخری_recorded_len)
    {
	len -= آخری_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    اگر (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    واپس (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    string_T
get_inserted(خالی)
{
    size_t len = 0;
    char_u *str = get_buffcont(&redobuff, FALSE, &len);
    string_T ret = { str, len };
    واپس ret;
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    ساکن خالی
جوڑو_buff(
    buffheader_T	*buf,
    char_u		*s,
    طویل		slen)	// length of "s" or -1
{
    اگر (slen < 0)
	slen = (طویل)STRLEN(s);
    اگر (slen == 0)				// don't add empty strings
	واپس;

    اگر (buf->bh_پہلا.b_اگلا == تہی)	// first add to list
    {
	buf->bh_curr = &(buf->bh_پہلا);
	buf->bh_create_newblock = TRUE;
    }
    ورنہ اگر (buf->bh_curr == تہی)	// buffer has already been read
    {
	iemsg(e_جوڑو_مقصد_اندرونی_بفر_that_was_already_read_from);
	واپس;
    }
    ورنہ اگر (buf->bh_اشاریہ != 0)
    {
	mch_memmove(buf->bh_پہلا.b_اگلا->b_str,
		    buf->bh_پہلا.b_اگلا->b_str + buf->bh_اشاریہ,
		    (buf->bh_پہلا.b_اگلا->b_strlen - buf->bh_اشاریہ) + 1);
	buf->bh_پہلا.b_اگلا->b_strlen -= buf->bh_اشاریہ;
	buf->bh_space += buf->bh_اشاریہ;
    }
    buf->bh_اشاریہ = 0;

    اگر (!buf->bh_create_newblock && buf->bh_space >= (صحیح)slen)
    {
	vim_strncpy(buf->bh_curr->b_str + buf->bh_curr->b_strlen, s, (size_t)slen);
	buf->bh_curr->b_strlen += slen;
	buf->bh_space -= slen;
    }
    ورنہ
    {
	long_u	    len;
	buffblock_T *p;

	اگر (slen < اقل_سائز)
	    len = اقل_سائز;
	ورنہ
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	اگر (p == تہی)
	    واپس; // no space, just forget it
	vim_strncpy(p->b_str, s, (size_t)slen);
	p->b_strlen = slen;
	buf->bh_space = (صحیح)(len - slen);
	buf->bh_create_newblock = FALSE;

	p->b_اگلا = buf->bh_curr->b_اگلا;
	buf->bh_curr->b_اگلا = p;
	buf->bh_curr = p;
    }
}

/*
 * Delete "slen" bytes from the end of "buf".
 * Only works when it was just added.
 */
    ساکن خالی
delete_buff_دم(buffheader_T *buf, صحیح slen)
{
    اگر (buf->bh_curr == تہی)
	واپس;  // nothing to delete
    اگر (buf->bh_curr->b_strlen < (size_t)slen)
	واپس;

    buf->bh_curr->b_str[buf->bh_curr->b_strlen - (size_t)slen] = NUL;
    buf->bh_curr->b_strlen -= slen;
    buf->bh_space += slen;
}

/*
 * Add number "n" to buffer "buf".
 */
    ساکن خالی
جوڑو_عدد_buff(buffheader_T *buf, طویل n)
{
    char_u	نمبر[32];
    صحیح		numberlen;

    numberlen = vim_snprintf((حرف *)نمبر, حجم(نمبر), "%ld", n);
    جوڑو_buff(buf, نمبر, (طویل)numberlen);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ساکن خالی
جوڑو_char_buff(buffheader_T *buf, صحیح c)
{
    char_u	بائٹیں[MB_MAXBYTES + 1];
    صحیح		len;
    صحیح		i;
    char_u	عارضی[4];
    طویل	templen;

    اگر (IS_SPECIAL(c))
	len = 1;
    ورنہ
	len = (*mb_char2bytes)(c, بائٹیں);
    برائے (i = 0; i < len; ++i)
    {
	اگر (!IS_SPECIAL(c))
	    c = بائٹیں[i];

	اگر (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    عارضی[0] = K_SPECIAL;
	    عارضی[1] = K_دوسرا(c);
	    عارضی[2] = K_تیسرا(c);
	    عارضی[3] = NUL;
	    templen = 3;
	}
#اگر_تعریف_شدہ FEAT_GUI
	ورنہ اگر (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    عارضی[0] = CSI;
	    عارضی[1] = KS_EXTRA;
	    عارضی[2] = (صحیح)KE_CSI;
	    عارضی[3] = NUL;
	    templen = 3;
	}
#اگر_ختم
	ورنہ
	{
	    عارضی[0] = c;
	    عارضی[1] = NUL;
	    templen = 1;
	}
	جوڑو_buff(buf, عارضی, templen);
    }
}

// First read ahead buffer. Used for translated commands.
ساکن buffheader_T readbuf1 = {{تہی, 0, {NUL}}, تہی, 0, 0, FALSE};

// Second read ahead buffer. Used for redo.
ساکن buffheader_T readbuf2 = {{تہی, 0, {NUL}}, تہی, 0, 0, FALSE};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    ساکن صحیح
read_readbuffers(صحیح advance)
{
    صحیح c;

    c = read_readbuf(&readbuf1, advance);
    اگر (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    واپس c;
}

    ساکن صحیح
read_readbuf(buffheader_T *buf, صحیح advance)
{
    char_u	c;
    buffblock_T	*curr;

    اگر (buf->bh_پہلا.b_اگلا == تہی)  // buffer is empty
	واپس NUL;

    curr = buf->bh_پہلا.b_اگلا;
    c = curr->b_str[buf->bh_اشاریہ];

    اگر (advance)
    {
	اگر (curr->b_str[++buf->bh_اشاریہ] == NUL)
	{
	    buf->bh_پہلا.b_اگلا = curr->b_اگلا;
	    vim_free(curr);
	    buf->bh_اشاریہ = 0;
	}
    }
    واپس c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    ساکن خالی
آغاز_stuff(خالی)
{
    اگر (readbuf1.bh_پہلا.b_اگلا != تہی)
    {
	readbuf1.bh_curr = &(readbuf1.bh_پہلا);
	readbuf1.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
    اگر (readbuf2.bh_پہلا.b_اگلا != تہی)
    {
	readbuf2.bh_curr = &(readbuf2.bh_پہلا);
	readbuf2.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    صحیح
stuff_خالی_خانے(خالی)
{
    واپس (readbuf1.bh_پہلا.b_اگلا == تہی
	 && readbuf2.bh_پہلا.b_اگلا == تہی);
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * readbuf2.
 */
    صحیح
readbuf1_خالی_خانے(خالی)
{
    واپس (readbuf1.bh_پہلا.b_اگلا == تہی);
}
#اگر_ختم

/*
 * Set a typeahead character that won't be flushed.
 */
    خالی
typeahead_noflush(صحیح c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    خالی
flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    آغاز_stuff();
    جب (read_readbuffers(TRUE) != NUL)
	;

    اگر (flush_typeahead == FLUSH_اقل)
    {
	// remove mapped characters at the start only,
	// but only when enough space left in typebuf
	اگر (typebuf.tb_off + typebuf.tb_maplen >= typebuf.tb_buflen)
	{
	    typebuf.tb_off = MAXMAPLEN;
	    typebuf.tb_len = 0;
	}
	ورنہ
	{
	    typebuf.tb_off += typebuf.tb_maplen;
	    typebuf.tb_len -= typebuf.tb_maplen;
	}
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	اگر (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#اگر_ختم
    }
    ورنہ
    {
	// remove typeahead
	اگر (flush_typeahead == FLUSH_داخلہ)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    جب (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#اگر_ختم
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
    خالی
ResetRedobuff(خالی)
{
    اگر (بلاک_redo)
	واپس;

    free_buff(&old_redobuff);
    old_redobuff = redobuff;
    redobuff.bh_پہلا.b_اگلا = تہی;
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    خالی
CancelRedo(خالی)
{
    اگر (بلاک_redo)
	واپس;

    free_buff(&redobuff);
    redobuff = old_redobuff;
    old_redobuff.bh_پہلا.b_اگلا = تہی;
    آغاز_stuff();
    جب (read_readbuffers(TRUE) != NUL)
	;
}

/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
    خالی
saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;
    size_t	slen;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_پہلا.b_اگلا = تہی;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_پہلا.b_اگلا = تہی;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE, &slen);
    اگر (s == تہی)
	واپس;

    جوڑو_buff(&redobuff, s, (طویل)slen);
    vim_free(s);
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    خالی
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
    خالی
Append_مقصد_Redobuff(char_u *s)
{
    اگر (!بلاک_redo)
	جوڑو_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    خالی
Append_مقصد_Redobuff_Lit(
    char_u	*str,
    صحیح		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    صحیح		c;
    char_u	*آغاز;

    اگر (بلاک_redo)
	واپس;

    جب (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	آغاز = s;
	جب (*s >= ' ' && *s < DEL && (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	اگر (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	اگر (s > آغاز)
	    جوڑو_buff(&redobuff, آغاز, (طویل)(s - آغاز));

	اگر (*s == NUL || (len >= 0 && s - str >= len))
	    توڑو;

	// Handle a special or multibyte character.
	اگر (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	ورنہ
	    c = *s++;
	اگر (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    جوڑو_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048
	اگر (*s == NUL && c == '0')
	    جوڑو_buff(&redobuff, (char_u *)"048", 3L);
	ورنہ
	    جوڑو_char_buff(&redobuff, c);
    }
}

/*
 * Append "s" to the redo buffer, leaving 3-byte special key codes unmodified
 * and escaping other K_SPECIAL and CSI bytes.
 */
    خالی
Append_مقصد_Redobuff_Spec(char_u *s)
{
    اگر (بلاک_redo)
	واپس;

    جب (*s != NUL)
    {
	اگر (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    جوڑو_buff(&redobuff, s, 3L);
	    s += 3;
	}
	ورنہ
	    جوڑو_char_buff(&redobuff, mb_cptr2char_adv(&s));
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    خالی
Append_Char_مقصد_Redobuff(صحیح c)
{
    اگر (!بلاک_redo)
	جوڑو_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    خالی
Append_نمبر_مقصد_Redobuff(طویل n)
{
    اگر (!بلاک_redo)
	جوڑو_عدد_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    خالی
stuffReadbuff(char_u *s)
{
    جوڑو_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    خالی
stuffRedoReadbuff(char_u *s)
{
    جوڑو_buff(&readbuf2, s, -1L);
}

    خالی
stuffReadbuffLen(char_u *s, طویل len)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (جوڑو_آخری_داخل_کرو == 1) // Only add if this is the first call, for
			      // recursive calls, ignore.
	ga_concat_len(&آخری_داخل_کرو_ga, s, (size_t)len);
#اگر_ختم
    جوڑو_buff(&readbuf1, s, len);
}

#اگر defined(FEAT_EVAL)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    خالی
stuffReadbuffSpec(char_u *s)
{
    صحیح c;

    جب (*s != NUL)
    {
	اگر (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	ورنہ
	{
	    c = mb_cptr2char_adv(&s);
	    اگر (c == CAR || c == NL || c == ESC)
		c = ' ';
	    stuffcharReadbuff(c);
	}
    }
}
#اگر_ختم

/*
 * Append a character to the stuff buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    خالی
stuffcharReadbuff(صحیح c)
{
    جوڑو_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    خالی
stuffnumReadbuff(طویل n)
{
    جوڑو_عدد_buff(&readbuf1, n);
}

/*
 * Stuff a string into the typeahead buffer, such that edit() will insert it
 * literally ("literally" TRUE) or interpret is as typed characters.
 */
    خالی
stuffescaped(char_u *arg, صحیح literally)
{
    صحیح		c;
    char_u	*آغاز;

    جب (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	آغاز = arg;
	جب ((*arg >= ' ' && *arg < DEL)
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	اگر (arg > آغاز)
	    stuffReadbuffLen(آغاز, (طویل)(arg - آغاز));

	// stuff a single special character
	اگر (*arg != NUL)
	{
	    اگر (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    ورنہ
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
    ساکن صحیح
read_redo(صحیح init, صحیح old_redo)
{
    ساکن buffblock_T	*bp;
    ساکن char_u	*p;
    صحیح			c;
    صحیح			n;
    char_u		buf[MB_MAXBYTES + 1];
    صحیح			i;

    اگر (init)
    {
	اگر (old_redo)
	    bp = old_redobuff.bh_پہلا.b_اگلا;
	ورنہ
	    bp = redobuff.bh_پہلا.b_اگلا;
	اگر (bp == تہی)
	    واپس FAIL;
	p = bp->b_str;
	واپس OK;
    }
    اگر ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	اگر (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	ورنہ
	    n = 1;
	برائے (i = 0; ; ++i)
	{
	    اگر (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = مقصد_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#اگر_تعریف_شدہ FEAT_GUI
	    اگر (c == CSI)	// escaped CSI
		p += 2;
#اگر_ختم
	    اگر (*++p == NUL && bp->b_اگلا != تہی)
	    {
		bp = bp->b_اگلا;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    اگر (i == n - 1)	// last byte of a character
	    {
		اگر (n != 1)
		    c = (*mb_ptr2char)(buf);
		توڑو;
	    }
	    c = *p;
	    اگر (c == NUL)	// cannot happen?
		توڑو;
	}
    }

    واپس c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    ساکن خالی
copy_redo(صحیح old_redo)
{
    صحیح	    c;

    جب ((c = read_redo(FALSE, old_redo)) != NUL)
	جوڑو_char_buff(&readbuf2, c);
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
آغاز_redo(طویل گنتی, صحیح old_redo)
{
    صحیح	    c;

    // init the pointers; return if nothing to redo
    اگر (read_redo(TRUE, old_redo) == FAIL)
	واپس FAIL;

    c = read_redo(FALSE, old_redo);

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (c == K_SID)
    {
	// Copy the <SID>{sid}; sequence
	جوڑو_char_buff(&readbuf2, c);
	برائے (;;)
	{
	    c = read_redo(FALSE, old_redo);
	    جوڑو_char_buff(&readbuf2, c);
	    اگر (!محفوظ_isdigit(c))
		توڑو;
	}
	c = read_redo(FALSE, old_redo);
    }
#اگر_ختم

    // copy the buffer name, if present
    اگر (c == '"')
    {
	جوڑو_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	// if a numbered buffer is used, increment the number
	اگر (c >= '1' && c < '9')
	    ++c;
	جوڑو_char_buff(&readbuf2, c);

	// the expression register should be re-evaluated
	اگر (c == '=')
	{
	    جوڑو_char_buff(&readbuf2, CAR);
	    cmd_silent = TRUE;
	}

	c = read_redo(FALSE, old_redo);
    }

    اگر (c == 'v')   // redo Visual
    {
	VIsual = curwin->w_cursor;
	VIsual_فعال = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    // try to enter the count (in place of a previous count)
    اگر (گنتی)
    {
	جب (VIM_ISDIGIT(c))	// skip "old" count
	    c = read_redo(FALSE, old_redo);
	جوڑو_عدد_buff(&readbuf2, گنتی);
    }

    // copy the rest from the redo buffer into the stuff buffer
    جوڑو_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    واپس OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    صحیح
آغاز_redo_ins(خالی)
{
    صحیح	    c;

    اگر (read_redo(TRUE, FALSE) == FAIL)
	واپس FAIL;
    آغاز_stuff();

    // skip the count and the command character
    جب ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	اگر (vim_strchr((char_u *)"AaIiRrOo", c) != تہی)
	{
	    اگر (c == 'O' || c == 'o')
		جوڑو_buff(&readbuf2, NL_STR, -1L);
	    توڑو;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    بلاک_redo = TRUE;
    واپس OK;
}

    خالی
stop_redo_ins(خالی)
{
    بلاک_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    ساکن خالی
init_typebuf(خالی)
{
    اگر (typebuf.tb_buf != تہی)
	واپس;

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
noremap_کلیدیں(خالی)
{
    واپس کلید_Noremap & (RM_NONE|RM_SCRIPT);
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
    صحیح		انحراف,
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
    state_no_longer_محفوظ("ins_typebuf()");

    addlen = (صحیح)STRLEN(str);

    اگر (انحراف == 0 && addlen <= typebuf.tb_off)
    {
	/*
	 * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
	 */
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    ورنہ اگر (typebuf.tb_len == 0 && typebuf.tb_buflen
					       >= addlen + 3 * (MAXMAPLEN + 4))
    {
	/*
	 * Buffer is empty and string fits in the existing buffer.
	 * Leave some space before and after, if possible.
	 */
	typebuf.tb_off = (typebuf.tb_buflen - addlen - 3 * (MAXMAPLEN + 4)) / 2;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    ورنہ
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
	    واپس FAIL;
	}
	newlen = typebuf.tb_len + extra;
	s1 = alloc(newlen);
	اگر (s1 == تہی)		    // out of memory
	    واپس FAIL;
	s2 = alloc(newlen);
	اگر (s2 == تہی)		    // out of memory
	{
	    vim_free(s1);
	    واپس FAIL;
	}
	typebuf.tb_buflen = newlen;

	// copy the old chars, before the insertion point
	mch_memmove(s1 + newoff, typebuf.tb_buf + typebuf.tb_off,
							      (size_t)انحراف);
	// copy the new chars
	mch_memmove(s1 + newoff + انحراف, str, (size_t)addlen);
	// copy the old chars, after the insertion point, including the	NUL at
	// the end
	mch_memmove(s1 + newoff + انحراف + addlen,
				     typebuf.tb_buf + typebuf.tb_off + انحراف,
				       (size_t)(typebuf.tb_len - انحراف + 1));
	اگر (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)انحراف);
	mch_memmove(s2 + newoff + انحراف + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + انحراف,
					   (size_t)(typebuf.tb_len - انحراف));
	اگر (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    // If noremap == REMAP_SCRIPT: do remap script-local mappings.
    اگر (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    ورنہ اگر (noremap == REMAP_SKIP)
	val = RM_ABBR;
    ورنہ
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
    ورنہ اگر (noremap < 0)
	nrm = addlen;
    ورنہ
	nrm = noremap;
    برائے (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + انحراف] =
						  (--nrm >= 0) ? val : RM_YES;

    // tb_maplen and tb_silent only remember the length of mapped and/or
    // silent mappings at the start of the buffer, assuming that a mapped
    // sequence doesn't result in typed characters.
    اگر (nottyped || typebuf.tb_maplen > انحراف)
	typebuf.tb_maplen += addlen;
    اگر (silent || typebuf.tb_silent > انحراف)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    اگر (typebuf.tb_no_abbr_cnt && انحراف == 0)	// and not used for abbrev.s
	typebuf.tb_no_abbr_cnt += addlen;

    واپس OK;
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
    صحیح		len = special_مقصد_buf(c, modifiers, TRUE, buf);

    buf[len] = NUL;
    (خالی)ins_typebuf(buf, کلید_Noremap, 0, !کلید_Typed, cmd_silent);
    واپس len;
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
typebuf_بدلا(
    صحیح		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    واپس (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#اگر_ختم
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    صحیح
typebuf_typed(خالی)
{
    واپس typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    صحیح
typebuf_maplen(خالی)
{
    واپس typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    خالی
del_typebuf(صحیح len, صحیح انحراف)
{
    صحیح	    i;

    اگر (len == 0)
	واپس;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    اگر (انحراف == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    ورنہ
    {
	i = typebuf.tb_off + انحراف;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	اگر (typebuf.tb_off > MAXMAPLEN)
	{
	    mch_memmove(typebuf.tb_buf + MAXMAPLEN,
			     typebuf.tb_buf + typebuf.tb_off, (size_t)انحراف);
	    mch_memmove(typebuf.tb_noremap + MAXMAPLEN,
			 typebuf.tb_noremap + typebuf.tb_off, (size_t)انحراف);
	    typebuf.tb_off = MAXMAPLEN;
	}
	// adjust typebuf.tb_buf (include the NUL at the end)
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + انحراف,
						     typebuf.tb_buf + i + len,
				       (size_t)(typebuf.tb_len - انحراف + 1));
	// adjust typebuf.tb_noremap[]
	mch_memmove(typebuf.tb_noremap + typebuf.tb_off + انحراف,
						 typebuf.tb_noremap + i + len,
					   (size_t)(typebuf.tb_len - انحراف));
    }

    اگر (typebuf.tb_maplen > انحراف)		// adjust tb_maplen
    {
	اگر (typebuf.tb_maplen < انحراف + len)
	    typebuf.tb_maplen = انحراف;
	ورنہ
	    typebuf.tb_maplen -= len;
    }
    اگر (typebuf.tb_silent > انحراف)		// adjust tb_silent
    {
	اگر (typebuf.tb_silent < انحراف + len)
	    typebuf.tb_silent = انحراف;
	ورنہ
	    typebuf.tb_silent -= len;
    }
    اگر (typebuf.tb_no_abbr_cnt > انحراف)	// adjust tb_no_abbr_cnt
    {
	اگر (typebuf.tb_no_abbr_cnt < انحراف + len)
	    typebuf.tb_no_abbr_cnt = انحراف;
	ورنہ
	    typebuf.tb_no_abbr_cnt -= len;
    }

#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#اگر_ختم
    اگر (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * State for adding bytes to a recording or 'showcmd'.
 */
قسمتعریف ساخت
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    صحیح		پچھلا_c;
    size_t	buflen;
    غیرعلامتی	pending_special;
    غیرعلامتی	pending_mbyte;
} gotchars_state_T;

/*
 * Add a single byte to a recording or 'showcmd'.
 * Return TRUE if a full key has been received, FALSE otherwise.
 */
    ساکن صحیح
gotchars_جوڑو_byte(gotchars_state_T *state, char_u byte)
{
    صحیح		c = state->buf[state->buflen++] = byte;
    صحیح		retval = FALSE;
    صحیح		اندر_special = state->pending_special > 0;
    صحیح		اندر_mbyte = state->pending_mbyte > 0;

    اگر (اندر_special)
	state->pending_special--;
    ورنہ اگر (c == K_SPECIAL
#اگر_تعریف_شدہ FEAT_GUI
		|| c == CSI
#اگر_ختم
	    )
	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	state->pending_special = 2;

    اگر (state->pending_special > 0)
	جاؤ ret_false;

    اگر (اندر_mbyte)
	state->pending_mbyte--;
    ورنہ
    {
	اگر (اندر_special)
	{
	    اگر (state->پچھلا_c == KS_MODIFIER)
		// When receiving a modifier, wait for the modified key.
		جاؤ ret_false;
	    c = مقصد_SPECIAL(state->پچھلا_c, c);
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
	جاؤ ret_false;

    retval = TRUE;
ret_false:
    state->پچھلا_c = c;
    واپس retval;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the record buffer.
 */
    ساکن خالی
gotchars(char_u *chars, صحیح len)
{
    char_u		*s = chars;
    size_t		i;
    صحیح			todo = len;
    ساکن gotchars_state_T state;

    جب (todo-- > 0)
    {
	اگر (!gotchars_جوڑو_byte(&state, *s++))
	    جاری;

	// Handle one byte at a time; no translation to be done.
	برائے (i = 0; i < state.buflen; ++i)
	    updatescript(state.buf[i]);

	اگر (reg_recording != 0)
	{
	    state.buf[state.buflen] = NUL;
	    جوڑو_buff(&recordbuff, state.buf, (طویل)state.buflen);
	    // remember how many chars were last recorded
	    آخری_recorded_len += state.buflen;
	}
	state.buflen = 0;
    }

    may_sync_undo();

#اگر_تعریف_شدہ FEAT_EVAL
    // output "debug mode" message next time in debug mode
    debug_did_پیغام = FALSE;
#اگر_ختم

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}

/*
 * Record an <Ignore> key.
 */
    خالی
gotchars_ignore(خالی)
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
    خالی
ungetchars(صحیح len)
{
    اگر (reg_recording == 0)
	واپس;

    delete_buff_دم(&recordbuff, len);
    آخری_recorded_len -= len;
}

/*
 * Sync undo.  Called when typed characters are obtained from the typeahead
 * buffer, or when a menu is used.
 * Do not sync:
 * - In Insert mode, unless cursor key has been used.
 * - While reading a script file.
 * - When no_u_sync is non-zero.
 */
    ساکن خالی
may_sync_undo(خالی)
{
    اگر ((!(State & (منوال_داخل_کرو | منوال_CMDLINE)) || arrow_مستعمل)
					       && scriptin[curscript] == تہی)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    ساکن صحیح
alloc_typebuf(خالی)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    اگر (typebuf.tb_buf == تہی || typebuf.tb_noremap == تہی)
    {
	free_typebuf();
	واپس FAIL;
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
#اگر_ختم
    واپس OK;
}

/*
 * Free the buffers of "typebuf".
 */
    ساکن خالی
free_typebuf(خالی)
{
    اگر (typebuf.tb_buf == typebuf_init)
	اندرونی_error("Free typebuf 1");
    ورنہ
	VIM_CLEAR(typebuf.tb_buf);
    اگر (typebuf.tb_noremap == noremapbuf_init)
	اندرونی_error("Free typebuf 2");
    ورنہ
	VIM_CLEAR(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
ساکن typebuf_T saved_typebuf[NSCRIPT];

    صحیح
save_typebuf(خالی)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    اگر (alloc_typebuf() == FAIL)
    {
	closescript();
	واپس FAIL;
    }
    واپس OK;
}

ساکن صحیح old_char = -1;	// character put back by vungetc()
ساکن صحیح old_mod_نقاب;	// mod_mask for ungotten character
ساکن صحیح old_mouse_صف;	// mouse_row related to old_char
ساکن صحیح old_mouse_ستون;	// mouse_col related to old_char
ساکن صحیح old_کلید_Stuffed;	// whether old_char was stuffed

ساکن صحیح can_get_old_char(خالی)
{
    // If the old character was not stuffed and characters have been added to
    // the stuff buffer, need to first get the stuffed characters instead.
    واپس old_char != -1 && (old_کلید_Stuffed || stuff_خالی_خانے());
}

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    خالی
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    اگر (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_نقاب = old_mod_نقاب;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_پہلا.b_اگلا = تہی;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_پہلا.b_اگلا = تہی;
#اگر_تعریف_شدہ USE_داخلہ_BUF
    tp->save_inputbuf = get_داخلہ_buf();
#اگر_ختم
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    خالی
restore_typeahead(tasave_T *tp, صحیح overwrite غیر_مستعمل)
{
    اگر (tp->typebuf_valid)
    {
	free_typebuf();
	typebuf = tp->save_typebuf;
    }

    old_char = tp->old_char;
    old_mod_نقاب = tp->old_mod_نقاب;

    free_buff(&readbuf1);
    readbuf1 = tp->save_readbuf1;
    free_buff(&readbuf2);
    readbuf2 = tp->save_readbuf2;
#اگر_تعریف_شدہ USE_داخلہ_BUF
    سیٹ_داخلہ_buf(tp->save_inputbuf, overwrite);
#اگر_ختم
}

/*
 * Open a new script file for the ":source!" command.
 */
    خالی
openscript(
    char_u	*name,
    صحیح		directly)	// when TRUE execute directly
{
    اگر (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	واپس;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    اگر (check_secure())
	واپس;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	واپس;
#اگر_ختم

    اگر (scriptin[curscript] != تہی)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    اگر ((scriptin[curscript] = mch_fopen((حرف *)NameBuff, READBIN)) == تہی)
    {
	semsg(_(e_cant_open_file_str), name);
	اگر (curscript)
	    --curscript;
	واپس;
    }
    اگر (save_typebuf() == FAIL)
	واپس;

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
	صحیح	save_اختتام_op = اختتام_op;
	صحیح	save_پیغام_scroll = پیغام_scroll;

	State = منوال_معمول;
	پیغام_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	اختتام_op = FALSE;

	oldcurscript = curscript;
	کرو
	{
	    update_topline_cursor();	// update cursor position and topline
	    معمول_cmd(&oa, FALSE);	// execute one command
	    (خالی)vpeekc();		// check for end of file
	}
	جب (scriptin[oldcurscript] != تہی);

	State = save_State;
	پیغام_scroll = save_پیغام_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	اختتام_op = save_اختتام_op;
    }
}

/*
 * Close the currently active input script.
 */
    ساکن خالی
closescript(خالی)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = تہی;
    اگر (curscript > 0)
	--curscript;
}

#اگر defined(EXITFREE)
    خالی
close_all_scripts(خالی)
{
    جب (scriptin[0] != تہی)
	closescript();
}
#اگر_ختم

/*
 * Return TRUE when reading keys from a script file.
 */
    صحیح
using_script(خالی)
{
    واپس scriptin[curscript] != تہی;
}

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    خالی
before_blocking(خالی)
{
    // Tell terminal to flush screen contents before blocking
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_FLUSH);

    updatescript(0);
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (may_garbage_collect)
	garbage_collect(FALSE);
#اگر_ختم
}

/*
 * updatescript() is called when a character can be written into the script
 * file or when we have waited some time for a character (c == 0)
 *
 * All the changed memfiles are synced if c == 0 or when the number of typed
 * characters reaches 'updatecount' and 'updatecount' is non-zero.
 */
    ساکن خالی
updatescript(صحیح c)
{
    ساکن صحیح	    گنتی = 0;

    اگر (c && scriptout)
	putc(c, scriptout);
    اگر (c == 0 || (p_uc > 0 && ++گنتی >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	گنتی = 0;
    }
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (typedchars_pos < MAXMAPLEN)
    {
	typedchars[typedchars_pos] = c;
	typedchars_pos++;
    }
#اگر_ختم
}

/*
 * Convert "c_arg" plus "modifiers" to merge the effect of modifyOtherKeys into
 * the character.  Also for when the Kitty key protocol is used.
 */
    صحیح
ملاؤ_modify_Other_کلیدیں(صحیح c_arg, صحیح *modifiers)
{
    صحیح c = c_arg;

    // CTRL only uses the lower 5 bits of the character.
    اگر (*modifiers & MOD_نقاب_CTRL)
    {
	اگر ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	{
	    c &= 0x1f;
	    اگر (c == NUL)
		c = K_ZERO;
	}
	ورنہ اگر (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#اگر_تعریف_شدہ FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	ورنہ اگر (c == '2')
	    c = NUL;
	ورنہ اگر (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	ورنہ اگر (c == '8')
	    c = BS;
	ورنہ اگر (c == '?')
	    c = DEL;
#اگر_ختم
	اگر (c != c_arg)
	    *modifiers &= ~MOD_نقاب_CTRL;
    }

    // Alt/Meta sets the 8th bit of the character.
    اگر ((*modifiers & (MOD_نقاب_META | MOD_نقاب_ALT))
	    && c >= 0 && c <= 127)
    {
	// Some terminals (esp. Kitty) do not include Shift in the character.
	// Apply it here to get consistency across terminals.  Only do ASCII
	// letters, for other characters it depends on the keyboard layout.
	اگر ((*modifiers & MOD_نقاب_سرکاؤ) && c >= 'a' && c <= 'z')
	{
	    c += 'a' - 'A';
	    *modifiers &= ~MOD_نقاب_سرکاؤ;
	}
	c += 0x80;
	*modifiers &= ~(MOD_نقاب_META | MOD_نقاب_ALT);
    }

    واپس c;
}

/*
 * Add a single byte to 'showcmd' for a partially matched mapping.
 * Call add_to_showcmd() if a full key has been received.
 */
    ساکن خالی
جوڑو_byte_مقصد_showcmd(char_u byte)
{
    ساکن gotchars_state_T state;
    char_u	*ptr;
    صحیح		modifiers = 0;
    صحیح		c = NUL;

    اگر (!p_sc || پیغام_silent != 0)
	واپس;

    اگر (!gotchars_جوڑو_byte(&state, byte))
	واپس;

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

	c = mb_ptr != تہی ? (*mb_ptr2char)(mb_ptr) : *ptr++;
	اگر (c <= 0x7f)
	{
	    // Merge modifiers into the key to make the result more readable.
	    صحیح		modifiers_after = modifiers;
	    صحیح		mod_c = ملاؤ_modify_Other_کلیدیں(c, &modifiers_after);

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
	جوڑو_مقصد_showcmd(K_SPECIAL);
	جوڑو_مقصد_showcmd(KS_MODIFIER);
	جوڑو_مقصد_showcmd(modifiers);
    }
    اگر (c != NUL)
	جوڑو_مقصد_showcmd(c);
    جب (*ptr != NUL)
	جوڑو_مقصد_showcmd(*ptr++);
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
vgetc(خالی)
{
    صحیح		c, c2;
    صحیح		n;
    char_u	buf[MB_MAXBYTES + 1];
    صحیح		i;

#اگر_تعریف_شدہ FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    اگر (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#اگر_ختم

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    اگر (can_get_old_char())
    {
	c = old_char;
	old_char = -1;
	mod_نقاب = old_mod_نقاب;
	mouse_صف = old_mouse_صف;
	mouse_ستون = old_mouse_ستون;
    }
    ورنہ
    {
	// number of characters recorded from the last vgetc() call
	ساکن size_t	آخری_vgetc_recorded_len = 0;

	mod_نقاب = 0;
	vgetc_mod_نقاب = 0;
	vgetc_char = 0;

	// last_recorded_len can be larger than last_vgetc_recorded_len
	// if peeking records more
	آخری_recorded_len -= آخری_vgetc_recorded_len;

	برائے (;;)		// this is done twice if there are modifiers
	{
	    صحیح did_inc = FALSE;

	    // No mapping after modifier has been read, using an input method
	    // and when a popup window has disabled mapping.
	    اگر (mod_نقاب
#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#اگر_ختم
#اگر defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#اگر_ختم
		    )
	    {
		++no_mapping;
		++allow_کلیدیں;
		// mod_mask value may change, remember we did the increment
		did_inc = TRUE;
	    }
	    c = vgetorpeek(TRUE);
	    اگر (did_inc)
	    {
		--no_mapping;
		--allow_کلیدیں;
	    }

	    // Get two extra bytes for special keys, handle modifiers.
	    اگر (c == K_SPECIAL
#اگر_تعریف_شدہ FEAT_GUI
		    || c == CSI
#اگر_ختم
	       )
	    {
		صحیح	    save_allow_کلیدیں = allow_کلیدیں;

		++no_mapping;
		allow_کلیدیں = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_کلیدیں = save_allow_کلیدیں;
		اگر (c2 == KS_MODIFIER)
		{
		    mod_نقاب = c;
		    جاری;
		}
		c = مقصد_SPECIAL(c2, c);

		اگر (allow_osc_کلید == 0 && c == K_OSC)
		    جاری;

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
		    gui.اندر_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    صحیح		j;

		    // get menu path, it ends with a <CR>
		    برائے (j = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[j] = c;
			اگر (j < 199)
			    ++j;
		    }
		    name[j] = NUL;
		    gui_make_tearoff(name);
		    جاری;
		}
#اگر_ختم
#اگر defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && !defined(USE_GTK4) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		اگر (c == K_F10 && gui.menubar != تہی)
		{
		    gtk_menu_shell_select_پہلا(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    جاری;
		}
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
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
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
		اگر (c == K_SID)
		{
		    صحیح	    j;

		    // Handle <SID>{sid};  Do up to 20 digits for safety.
		    آخری_مستعمل_sid = 0;
		    برائے (j = 0; j < 20 && محفوظ_isdigit(c = vgetorpeek(TRUE)); ++j)
			آخری_مستعمل_sid = آخری_مستعمل_sid * 10 + (c - '0');
		    آخری_مستعمل_map = تہی;
		    جاری;
		}
#اگر_ختم
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    اگر (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		برائے (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    اگر (buf[i] == K_SPECIAL
#اگر_تعریف_شدہ FEAT_GUI
			    || (buf[i] == CSI)
#اگر_ختم
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
		vgetc_mod_نقاب = mod_نقاب;
		vgetc_char = c;
	    }

	    // A keypad or special function key was not mapped, use it like
	    // its ASCII equivalent.
	    انتخاب (c)
	    {
		حالت K_KPLUS:	c = '+'; توڑو;
		حالت K_KMINUS:	c = '-'; توڑو;
		حالت K_KDIVIDE:	c = '/'; توڑو;
		حالت K_KMULTIPLY: c = '*'; توڑو;
		حالت K_KENTER:	c = CAR; توڑو;
		حالت K_KPOINT:
#اگر_تعریف_شدہ MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = Map_Virtual_کلید(VK_DECIMAL, 2); توڑو;
#ورنہ
				c = '.'; توڑو;
#اگر_ختم
		حالت K_K0:	c = '0'; توڑو;
		حالت K_K1:	c = '1'; توڑو;
		حالت K_K2:	c = '2'; توڑو;
		حالت K_K3:	c = '3'; توڑو;
		حالت K_K4:	c = '4'; توڑو;
		حالت K_K5:	c = '5'; توڑو;
		حالت K_K6:	c = '6'; توڑو;
		حالت K_K7:	c = '7'; توڑو;
		حالت K_K8:	c = '8'; توڑو;
		حالت K_K9:	c = '9'; توڑو;

		حالت K_XHOME:
		حالت K_ZHOME:	اگر (mod_نقاب == MOD_نقاب_سرکاؤ)
				{
				    c = K_S_HOME;
				    mod_نقاب = 0;
				}
				ورنہ اگر (mod_نقاب == MOD_نقاب_CTRL)
				{
				    c = K_C_HOME;
				    mod_نقاب = 0;
				}
				ورنہ
				    c = K_HOME;
				توڑو;
		حالت K_XEND:
		حالت K_ZEND:	اگر (mod_نقاب == MOD_نقاب_سرکاؤ)
				{
				    c = K_S_انتہا;
				    mod_نقاب = 0;
				}
				ورنہ اگر (mod_نقاب == MOD_نقاب_CTRL)
				{
				    c = K_C_انتہا;
				    mod_نقاب = 0;
				}
				ورنہ
				    c = K_انتہا;
				توڑو;

		حالت K_XUP:	c = K_اوپر; توڑو;
		حالت K_XDOWN:	c = K_نیچے_کو; توڑو;
		حالت K_XLEFT:	c = K_بایاں; توڑو;
		حالت K_XRIGHT:	c = K_دایاں; توڑو;
	    }

	    توڑو;
	}

	آخری_vgetc_recorded_len = آخری_recorded_len;
    }

#اگر_تعریف_شدہ FEAT_EVAL
    /*
     * In the main loop "may_garbage_collect" can be set to do garbage
     * collection in the first next vgetc().  It's disabled after that to
     * avoid internally used Lists and Dicts to be freed.
     */
    may_garbage_collect = FALSE;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_BEVAL_رکن
    اگر (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD
	    && c != K_COMPLETE_DELAY)
    {
	// Don't trigger 'balloonexpr' unless only the mouse was moved.
	bevalexpr_due_سیٹ = FALSE;
	ui_remove_balloon();
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // Only filter keys that do not come from ":normal".  Keys from feedkeys()
    // are filtered.
    اگر ((!ex_معمول_busy || اندر_feedkeys) && popup_do_filter(c))
    {
	اگر (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    c = do_کلید_داخلہ_pre(c);

    // Clear the next typedchars_pos
    typedchars_pos = 0;
#اگر_ختم

    // Need to process the character before we know it's safe to do something
    // else.
    اگر (c != K_IGNORE)
	state_no_longer_محفوظ("key typed");

    واپس c;
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Handle the KeyInputPre autocommand.
 * "c" is the character that was typed.
 * Return new input character.
 */
    ساکن صحیح
do_کلید_داخلہ_pre(صحیح c)
{
    صحیح		res = c;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    char_u	curr_منوال[منوال_MAX_لمبائی];
    صحیح		save_State = State;
    save_v_event_T save_v_event;
    dict_T	*v_event;

    // Return quickly when there is nothing to do.
    اگر (!has_keyinputpre())
	واپس res;

    اگر (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = KEY2TERMCAP0(c);
	buf[2] = KEY2TERMCAP1(c);
	buflen = 3;
    }
    ورنہ
	buflen = (*mb_char2bytes)(c, buf);
    buf[buflen] = NUL;

    typedchars[typedchars_pos] = NUL;
    vim_unescape_csi(typedchars);

    get_منوال(curr_منوال);

    // Lock the text to avoid weird things from happening.
    ++textlock;
    سیٹ_vim_var_string(VV_CHAR, buf, (صحیح)buflen);  // set v:char

    v_event = get_v_event(&save_v_event);
    (خالی)dict_جوڑو_bool(v_event, "typed", کلید_Typed);
    (خالی)dict_جوڑو_string_len(v_event, "typedchar", typedchars, typedchars_pos);

    اگر (apply_autocmds(EVENT_KEYINPUTPRE, curr_منوال, curr_منوال, FALSE, curbuf))
    {
	string_T    v_char;

	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character.
	v_char.string = get_vim_var_str(VV_CHAR);
	اگر (STRCMP(buf, v_char.string) != 0)
	{
	    v_char.لمبائی = STRLEN(v_char.string);
	    // Convert special bytes when it is special string.
	    اگر (v_char.لمبائی >= 3 && v_char.string[0] == K_SPECIAL)
		res = TERMCAP2_کلید(v_char.string[1], v_char.string[2]);
	    ورنہ اگر (v_char.لمبائی > 0)
		res = PTR2CHAR(v_char.string);
	}
    }

    restore_v_event(v_event, &save_v_event);

    سیٹ_vim_var_string(VV_CHAR, تہی, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    واپس res;
}
#اگر_ختم

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    صحیح
محفوظ_vgetc(خالی)
{
    صحیح	c;

    c = vgetc();
    اگر (c == NUL)
	c = get_keystroke();
    واپس c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 * Does not handle bracketed paste - do not use the result for commands.
 */
    ساکن صحیح
plain_vgetc_nopaste(خالی)
{
    صحیح c;

    کرو
	c = محفوظ_vgetc();
    جب (c == K_IGNORE
	    || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
	    || c == K_MOUSEMOVE);
    واپس c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    صحیح
plain_vgetc(خالی)
{
    صحیح c = plain_vgetc_nopaste();

    اگر (c == K_PS)
	// Only handle the first pasted character.  Drop the rest, since we
	// don't know what to do with it.
	c = bracketed_paste(PASTE_ONE_CHAR, FALSE, تہی);

    واپس c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 * Returns NUL if no character is available.
 */
    صحیح
vpeekc(خالی)
{
    اگر (can_get_old_char())
	واپس old_char;
    واپس vgetorpeek(FALSE);
}

#اگر defined(FEAT_TERMRESPONSE) || defined(FEAT_TERMINAL)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    صحیح
vpeekc_nomap(خالی)
{
    صحیح		c;

    ++no_mapping;
    ++allow_کلیدیں;
    c = vpeekc();
    --no_mapping;
    --allow_کلیدیں;
    واپس c;
}
#اگر_ختم

/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    صحیح
vpeekc_any(خالی)
{
    صحیح		c;

    c = vpeekc();
    اگر (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    واپس c;
}

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    صحیح
char_avail(خالی)
{
    صحیح	    retval;

#اگر_تعریف_شدہ FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    اگر (disable_char_avail_for_testing)
	واپس FALSE;
#اگر_ختم
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    واپس (retval != NUL);
}

#اگر defined(FEAT_EVAL)
/*
 * "getchar()" and "getcharstr()" functions
 */
    ساکن خالی
getchar_common(typval_T *argvars, typval_T *rettv, صحیح allow_نمبر)
{
    varnumber_T		n = 0;
    صحیح			called_emsg_آغاز = called_emsg;
    صحیح			error = FALSE;
    صحیح			simplify = TRUE;
    char_u		cursor_flag = 'm';

    اگر ((اندر_vim9script()
		&& check_for_opt_bool_or_نمبر_arg(argvars, 0) == FAIL)
	    || (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type == VAR_DICT)
    {
	dict_T		*d = argvars[1].vval.v_dict;
	char_u		*cursor_str;

	اگر (allow_نمبر)
	    allow_نمبر = dict_get_bool(d, "number", TRUE);
	ورنہ اگر (dict_has_کلید(d, "number"))
	    semsg(_(e_invalid_دلیل_str), "number");

	simplify = dict_get_bool(d, "simplify", TRUE);

	cursor_str = dict_get_string(d, "cursor", FALSE);
	اگر (cursor_str != تہی)
	{
	    اگر (STRCMP(cursor_str, "hide") != 0
		    && STRCMP(cursor_str, "keep") != 0
		    && STRCMP(cursor_str, "msg") != 0)
		semsg(_(e_invalid_قدر_for_دلیل_str_str), "cursor",
								   cursor_str);
	    ورنہ
		cursor_flag = cursor_str[0];
	}
    }

    اگر (called_emsg != called_emsg_آغاز)
	واپس;

# ifdef پیغام_قطار
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
# endif

    اگر (cursor_flag == 'h')
	cursor_sleep();
    ورنہ اگر (cursor_flag == 'm')
	windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);

    ++no_mapping;
    ++allow_کلیدیں;
    ++allow_osc_کلید;
    اگر (!simplify)
	++no_reduce_کلیدیں;
    برائے (;;)
    {
	اگر (argvars[0].v_type == VAR_UNKNOWN
		|| (argvars[0].v_type == VAR_نمبر
			&& argvars[0].vval.v_نمبر == -1))
	    // getchar(): blocking wait.
	    n = plain_vgetc_nopaste();
	ورنہ اگر (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	ورنہ اگر (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	ورنہ
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = محفوظ_vgetc();

	اگر (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    جاری;
	توڑو;
    }
    --no_mapping;
    --allow_کلیدیں;
    --allow_osc_کلید;
    اگر (!simplify)
	--no_reduce_کلیدیں;

    اگر (cursor_flag == 'h')
	cursor_unsleep();

    سیٹ_vim_var_nr(VV_MOUSE_WIN, 0);
    سیٹ_vim_var_nr(VV_MOUSE_WINID, 0);
    سیٹ_vim_var_nr(VV_MOUSE_LNUM, 0);
    سیٹ_vim_var_nr(VV_MOUSE_ستون, 0);

    اگر (n != 0 && (!allow_نمبر || IS_SPECIAL(n) || mod_نقاب != 0))
    {
	char_u		عارضی[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	صحیح		i = 0;

	// Turn a special key into three bytes, plus modifier.
	اگر (mod_نقاب != 0)
	{
	    عارضی[i++] = K_SPECIAL;
	    عارضی[i++] = KS_MODIFIER;
	    عارضی[i++] = mod_نقاب;
	}
	اگر (IS_SPECIAL(n))
	{
	    عارضی[i++] = K_SPECIAL;
	    عارضی[i++] = K_دوسرا(n);
	    عارضی[i++] = K_تیسرا(n);
	}
	ورنہ اگر (has_mbyte)
	    i += (*mb_char2bytes)(n, عارضی + i);
	ورنہ
	    عارضی[i++] = n;
	عارضی[i] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strnsave(عارضی, i);

	اگر (is_mouse_کلید(n))
	{
	    صحیح		صف = mouse_صف;
	    صحیح		ستون = mouse_ستون;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    صحیح		winnr = 1;

	    اگر (صف >= 0 && ستون >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);
		اگر (win == تہی)
		    واپس;
		(خالی)mouse_comp_pos(win, &صف, &ستون, &lnum, تہی);
# ifdef FEAT_PROP_POPUP
		اگر (WIN_IS_POPUP(win))
		    winnr = 0;
		ورنہ
# endif
		    برائے (wp = firstwin; wp != win && wp != تہی;
							       wp = wp->w_اگلا)
			++winnr;
		سیٹ_vim_var_nr(VV_MOUSE_WIN, winnr);
		سیٹ_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		سیٹ_vim_var_nr(VV_MOUSE_LNUM, lnum);
		سیٹ_vim_var_nr(VV_MOUSE_ستون, ستون + 1);
	    }
	}
    }
    ورنہ اگر (!allow_نمبر)
	rettv->v_type = VAR_STRING;
    ورنہ
	rettv->vval.v_نمبر = n;
}

/*
 * "getchar()" function
 */
    خالی
f_getchar(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, TRUE);
}

/*
 * "getcharstr()" function
 */
    خالی
f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, FALSE);
}

/*
 * "getcharmod()" function
 */
    خالی
f_getcharmod(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = mod_نقاب;
}
#اگر_ختم // FEAT_EVAL

#اگر defined(پیغام_قطار)
# define MAX_REPEAT_PARSE 8

/*
 * Process messages that have been queued for netbeans or clientserver.
 * Also check if any jobs have ended.
 * These functions can call arbitrary Vim script and should only be called when
 * it is safe to do so.
 */
    خالی
parse_queued_messages(خالی)
{
    صحیح	    old_curwin_id;
    صحیح	    old_curbuf_fnum;
    صحیح	    i;
    صحیح	    save_may_garbage_collect = may_garbage_collect;
    ساکن صحیح entered = 0;
    صحیح	    was_محفوظ = get_was_محفوظ_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    اگر (updating_screen || dont_parse_messages)
	واپس;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    اگر (curbuf == تہی || curwin == تہی)
	واپس;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    برائے (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# اگر (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_کام_CHANNEL)
	channel_handle_events(FALSE);
# endif

# ifdef FEAT_NETBEANS_INTG
	// Process the queued netbeans messages.
	netbeans_parse_messages();
# endif
# ifdef FEAT_SOCKETSERVER
	socketserver_parse_messages();
# endif
# ifdef FEAT_کام_CHANNEL
	// Write any buffer lines still to be written.
	channel_write_any_سطریں();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# اگر defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_کام_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	اگر (کام_check_ended())
	    جاری;
# endif
# ifdef FEAT_TERMINAL
	free_غیر_مستعمل_terminals();
# endif

# ifdef FEAT_SOUND_MACOSX
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	اگر (has_sound_callback_اندر_قطار())
	    invoke_sound_callback();
# endif
# ifdef SIGUSR1
	اگر (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, تہی, تہی, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
# endif
	توڑو;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    اگر (entered == 1 && was_محفوظ)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    اگر (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}
#اگر_ختم


قسمتعریف شمار {
    map_نتیجہ_fail,    // failed, break loop
    map_نتیجہ_get,     // get a character from typeahead
    map_نتیجہ_retry,   // try to map again
    map_نتیجہ_nomatch  // no matching mapping, get char
} map_نتیجہ_T;

/*
 * Check if the bytes at the start of the typeahead buffer are a character used
 * in Insert mode completion.  This includes the form with a CTRL modifier.
 * During completion started by complete() keys are mapped as usual.
 */
    ساکن صحیح
at_ins_compl_کلید(خالی)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    صحیح	    c = *p;

    اگر (typebuf.tb_len > 3
	    && (c == K_SPECIAL || c == CSI)  // CSI is used by the GUI
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_نقاب_CTRL)
	    // CTRL-SHIFT-N/P scroll the info popup, so they must not be folded
	    // to the CTRL-N/CTRL-P completion keys here.
	    && !(p[2] & MOD_نقاب_سرکاؤ))
	c = p[3] & 0x1f;
    واپس !ctrl_x_منوال_eval()
	    && ((ctrl_x_منوال_not_default() && vim_is_ctrl_x_کلید(c))
		|| (compl_حالت_local() && (c == Ctrl_N || c == Ctrl_P)));
}

/*
 * Check if typebuf.tb_buf[] contains a modifier plus key that can be changed
 * into just a key, apply that.
 * Check from typebuf.tb_buf[typebuf.tb_off] to typebuf.tb_buf[typebuf.tb_off
 * + "max_offset"].
 * Return the length of the replaced bytes, 0 if nothing changed, -1 for error.
 */
    ساکن صحیح
check_simplify_modifier(صحیح max_انحراف)
{
    صحیح		انحراف;
    char_u	*tp;

    برائے (انحراف = 0; انحراف < max_انحراف; ++انحراف)
    {
	اگر (انحراف + 3 >= typebuf.tb_len)
	    توڑو;
	tp = typebuf.tb_buf + typebuf.tb_off + انحراف;
	اگر ((tp[0] == K_SPECIAL || tp[0] == CSI) && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    صحیح modifier = tp[2];
	    صحیح	c = tp[3];
	    صحیح new_c = ملاؤ_modify_Other_کلیدیں(c, &modifier);

	    اگر (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		صحیح	len;
		صحیح	کلید_انحراف = انحراف;

		اگر (انحراف == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_نقاب = tp[2];
		}
		اگر (IS_SPECIAL(new_c))
		{
		    new_string[0] = K_SPECIAL;
		    new_string[1] = K_دوسرا(new_c);
		    new_string[2] = K_تیسرا(new_c);
		    len = 3;
		}
		ورنہ
		    len = mb_char2bytes(new_c, new_string);
		اگر (modifier == 0)
		{
		    اگر (put_string_اندر_typebuf(انحراف, 4, new_string, len,
							تہی, 0, تہی) == FAIL)
			واپس -1;
		}
		ورنہ
		{
		    tp[2] = modifier;
		    کلید_انحراف = انحراف + 3;
		    اگر (put_string_اندر_typebuf(کلید_انحراف, 1, new_string, len,
							تہی, 0, تہی) == FAIL)
			واپس -1;
		}

		// A mapping for the simplified key can be used for it even
		// when the key protocol is enabled.
		برائے (صحیح i = 0; i < len; ++i)
		    typebuf.tb_noremap[typebuf.tb_off + کلید_انحراف + i]
							    |= RM_SIMPLIFIED;
		واپس len;
	    }
	}
    }
    واپس 0;
}

/*
 * Return TRUE if the terminal sends modifiers with various keys.  This is when
 * modifyOtherKeys level 2 is enabled or the kitty keyboard protocol is
 * enabled.
 */
    صحیح
کلید_protocol_enabled(خالی)
{
    // If xterm has responded to XTQMODKEYS it overrules seenModifyOtherKeys.
    صحیح using_mok = modify_otherkeys_state != MOKS_INITIAL
			? modify_otherkeys_state == MOKS_ENABLED
			: نظر_آیا_Modify_Other_کلیدیں;
    واپس using_mok || kitty_protocol_state == KKPS_ENABLED;
}

/*
 * Handle mappings in the typeahead buffer.
 * - When something was mapped, return map_result_retry for recursive mappings.
 * - When nothing mapped and typeahead has a character: return map_result_get.
 * - When there is no match yet, return map_result_nomatch, need to get more
 *   typeahead.
 * - On failure (out of memory) return map_result_fail.
 */
    ساکن صحیح
handle_mapping(
	    صحیح *keylenp,
	    صحیح *timedout,
	    صحیح *mapdepth)
{
    mapblock_T	*mp = تہی;
    mapblock_T	*mp2;
    mapblock_T	*mp_مطابقت;
    صحیح		mp_مطابقت_len = 0;
    صحیح		max_mlen = 0;
    صحیح		want_termcode = 0;  // 1 if termcode expected after max_mlen
    صحیح		tb_c1;
    صحیح		mlen;
#اگر_تعریف_شدہ FEAT_LANGMAP
    صحیح		nolmaplen;
#اگر_ختم
    صحیح		keylen = *keylenp;
    صحیح		i;
    صحیح		local_State = get_real_state();
    صحیح		is_plug_map = FALSE;
    bool	اندر_osc = اندر_osc_sequence(); // If we are in an OSC sequence,
					    // then go straight to
					    // check_termcode() in order to
					    // consume chars.

    // If typeahead starts with <Plug> then remap, even for a "noremap" mapping.
    اگر (!اندر_osc && typebuf.tb_len >= 3
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
    اگر (!اندر_osc && no_mapping == 0 && is_maphash_valid()
	    && (no_zero_mapping == 0 || tb_c1 != '0')
	    && (typebuf.tb_maplen == 0 || is_plug_map
		|| (p_remap
		    && (typebuf.tb_noremap[typebuf.tb_off]
				    & (RM_NONE|RM_ABBR)) == 0))
	    && !(p_paste && (State & (منوال_داخل_کرو | منوال_CMDLINE)))
	    && !(State == منوال_HITRETURN && (tb_c1 == CAR || tb_c1 == ' '))
	    && State != منوال_ASKMORE
	    && State != منوال_CONFIRM
	    && !at_ins_compl_کلید())
    {
#اگر_تعریف_شدہ FEAT_GUI
	اگر (gui.اندر_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LANGMAP
	اگر (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	ورنہ
	{
	    LANGMAP_ADJUST(tb_c1, (State & (منوال_CMDLINE | منوال_داخل_کرو)) == 0
					   && get_real_state() != منوال_SELECT);
	    nolmaplen = 0;
	}
#اگر_ختم
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	اگر (mp == تہی)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = تہی;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_مطابقت = تہی;
	mp_مطابقت_len = 0;
	برائے ( ; mp != تہی;
	       mp->m_اگلا == تہی ? (mp = mp2, mp2 = تہی) : (mp = mp->m_اگلا))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    اگر (mp->m_کلیدیں[0] == tb_c1
		    && (mp->m_منوال & local_State)
		    && !(mp->m_simplified && کلید_protocol_enabled()
			    && typebuf.tb_maplen == 0
			    && (typebuf.tb_noremap[typebuf.tb_off]
							& RM_SIMPLIFIED) == 0)
		    && ((mp->m_منوال & منوال_LANGMAP) == 0
						    || typebuf.tb_maplen == 0))
	    {
#اگر_تعریف_شدہ FEAT_LANGMAP
		صحیح	nomap = nolmaplen;
		صحیح	modifiers = 0;
#اگر_ختم
		// find the match length of this mapping
		برائے (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
		    صحیح	c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
#اگر_تعریف_شدہ FEAT_LANGMAP
		    اگر (nomap > 0)
		    {
			اگر (nomap == 2 && c2 == KS_MODIFIER)
			    modifiers = 1;
			ورنہ اگر (nomap == 1 && modifiers == 1)
			    modifiers = c2;
			--nomap;
		    }
		    ورنہ
		    {
			اگر (c2 == K_SPECIAL)
			    nomap = 2;
			ورنہ اگر (ملاؤ_modify_Other_کلیدیں(c2, &modifiers) == c2)
			    // Only apply 'langmap' if merging modifiers into
			    // the key will not result in another character,
			    // so that 'langmap' behaves consistently in
			    // different terminals and GUIs.
			    LANGMAP_ADJUST(c2, TRUE);
			modifiers = 0;
		    }
#اگر_ختم
		    اگر (mp->m_کلیدیں[mlen] != c2)
			توڑو;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_کلیدیں;
		    char_u *p2 = mb_unescape(&p1);

		    اگر (has_mbyte && p2 != تہی
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
			    && (mp->m_کلیدیں[0] != K_SPECIAL
				|| mp->m_کلیدیں[1] != KS_EXTRA
				|| mp->m_کلیدیں[2] != KE_SNR))
			جاری;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    برائے (n = mlen; --n >= 0; )
			اگر (*s++ & (RM_NONE|RM_ABBR))
			    توڑو;
		    اگر (!is_plug_map && n >= 0)
			جاری;

		    اگر (keylen > typebuf.tb_len)
		    {
			اگر (!*timedout && !(mp_مطابقت != تہی
							&& mp_مطابقت->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    توڑو;
			}
		    }
		    ورنہ اگر (keylen > mp_مطابقت_len)
		    {
			// found a longer match
			mp_مطابقت = mp;
			mp_مطابقت_len = keylen;
		    }
		}
		ورنہ
		{
		    // No match; may have to check for termcode at next
		    // character.

		    // If the first character that didn't match is
		    // K_SPECIAL then check for a termcode.  This isn't perfect
		    // but should work in most cases.
		    اگر (max_mlen < mlen)
		    {
			max_mlen = mlen;
			want_termcode = mp->m_کلیدیں[mlen] == K_SPECIAL;
		    }
		    ورنہ اگر (max_mlen == mlen && mp->m_کلیدیں[mlen] == K_SPECIAL)
			want_termcode = 1;

		    // Check termcode for uppercase character to properly
		    // process "ESC[27;2;<ascii code>~" control sequences.
		    اگر (ASCII_ISUPPER(mp->m_کلیدیں[mlen]))
			want_termcode = 1;
		}
	    }
	}

	// If no partly match found, use the longest full match.
	اگر (keylen != KEYLEN_PART_MAP && mp_مطابقت != تہی)
	{
	    mp = mp_مطابقت;
	    keylen = mp_مطابقت_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    اگر (!اندر_osc && *p_pt != NUL && mp == تہی &&
	    (State & (منوال_داخل_کرو | منوال_معمول)))
    {
	برائے (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    اگر (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    توڑو;
	اگر (p_pt[mlen] == NUL)	//مطابقت
	{
	    // write chars to script file(s)
	    اگر (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    سیٹ_option_قدر_give_err((char_u *)"paste",
						      (طویل)!p_paste, تہی, 0);
	    اگر (!(State & منوال_داخل_کرو))
	    {
		پیغام_ستون = 0;
		پیغام_صف = صفیں - 1;
		پیغام_clr_eos();		// clear ruler
	    }
	    حالت_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    واپس map_نتیجہ_retry;
	}
	// Need more chars for partly match.
	اگر (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_کلید;
	ورنہ اگر (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    // May check for a terminal code when there is no mapping or only a partial
    // mapping.  Also check if there is a full mapping with <Esc>, unless timed
    // out, since that is nearly always a partial match with a terminal code.
    اگر (اندر_osc || ((mp == تہی || max_mlen + want_termcode > mp_مطابقت_len
		    || (mp_مطابقت_len == 1 && *mp->m_کلیدیں == ESC && !*timedout))
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
	اگر (اندر_osc || no_mapping == 0 || allow_کلیدیں != 0)
	{
	    اگر (اندر_osc || ((typebuf.tb_maplen == 0
			    || (p_remap && (typebuf.tb_noremap[typebuf.tb_off]
						& ~RM_SIMPLIFIED) == RM_YES))
		    && !*timedout))
		keylen = check_termcode(max_mlen + 1, تہی, 0, تہی);
	    ورنہ
		keylen = 0;

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    اگر (keylen == 0 && save_keylen == KEYLEN_PART_کلید && !*timedout)
		keylen = KEYLEN_PART_کلید;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
#اگر_تعریف_شدہ FEAT_TERMINAL
	    check_no_reduce_کلیدیں();  // may update the no_reduce_keys flag
#اگر_ختم
	    اگر (keylen == 0 && !no_reduce_کلیدیں)
	    {
		keylen = check_simplify_modifier(max_mlen + 1);
		اگر (keylen < 0)
		    // ins_typebuf() failed
		    واپس map_نتیجہ_fail;
	    }

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    اگر (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	ورنہ
	    keylen = 0;
	اگر (keylen == 0)	// no matching terminal code
	{
#اگر_تعریف_شدہ AMIGA
	    // check for window bounds report
	    اگر (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		برائے (s = typebuf.tb_buf + typebuf.tb_off + 1;
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
		    واپس map_نتیجہ_retry;
		}
		اگر (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_کلید;
	    }
	    اگر (keylen >= 0)
#اگر_ختم
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		اگر (mp == تہی)
		{
		    *keylenp = keylen;
		    واپس map_نتیجہ_get;    // get character from typeahead
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
		idx = get_menu_اشاریہ(current_menu, local_State);
		اگر (idx != MENU_اشاریہ_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    اگر (VIsual_فعال && VIsual_select
					&& (current_menu->modes & منوال_VISUAL))
		    {
			VIsual_select = FALSE;
			(خالی)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->لڑیاں[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#اگر_ختم // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    واپس map_نتیجہ_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	اگر (mp == تہی || keylen < 0)
	    keylen = KEYLEN_PART_کلید;
	ورنہ
	    keylen = mp_مطابقت_len;
    }

    /*
     * complete match
     */
    اگر (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#اگر_تعریف_شدہ FEAT_EVAL
	صحیح	save_m_اظہاریہ;
	صحیح	save_m_noremap;
	صحیح	save_m_silent;
	char_u	*save_m_کلیدیں;
	char_u	*save_alt_m_کلیدیں;
	صحیح	save_alt_m_keylen;
#ورنہ
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#اگر_ختم

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
	    اگر (State & منوال_CMDLINE)
		redrawcmdline();
	    ورنہ
		setcursor();
	    flush_buffers(FLUSH_اقل);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    واپس map_نتیجہ_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	اگر (VIsual_فعال && VIsual_select && (mp->m_منوال & منوال_VISUAL))
	{
	    VIsual_select = FALSE;
	    (خالی)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#اگر_تعریف_شدہ FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_اظہاریہ = mp->m_اظہاریہ;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_کلیدیں = تہی;  // only saved when needed
	save_alt_m_کلیدیں = تہی;  // only saved when needed
	save_alt_m_keylen = mp->m_alt != تہی ? mp->m_alt->m_keylen : 0;

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	اگر (mp->m_اظہاریہ)
	{
	    صحیح save_vgetc_busy = vgetc_busy;
	    صحیح save_may_garbage_collect = may_garbage_collect;
	    صحیح was_screen_ستون = screen_cur_ستون;
	    صحیح was_screen_صف = screen_cur_صف;
	    صحیح پچھلا_did_emsg = did_emsg;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_کلیدیں = vim_strnsave(mp->m_کلیدیں, (size_t)mp->m_keylen);
	    save_alt_m_کلیدیں = mp->m_alt != تہی
				    ? vim_strnsave(mp->m_alt->m_کلیدیں,
					     (size_t)save_alt_m_keylen) : تہی;
	    map_str = eval_map_اظہاریہ(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_صف, was_screen_ستون);
	    برآمد_flush();

	    // If an error was displayed and the expression returns an empty
	    // string, generate a <Nop> to allow for a redraw.
	    اگر (پچھلا_did_emsg != did_emsg
				       && (map_str == تہی || *map_str == NUL))
	    {
		char_u	buf[4];

		vim_free(map_str);
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = KE_IGNORE;
		buf[3] = NUL;
		map_str = vim_strnsave(buf, 3);
		اگر (State & منوال_CMDLINE)
		{
		    // redraw the command below the error
		    پیغام_didout = TRUE;
		    اگر (پیغام_صف < cmdline_صف)
			پیغام_صف = cmdline_صف;
		    redrawcmd();
		}
	    }

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	ورنہ
#اگر_ختم
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	اگر (map_str == تہی)
	    i = FAIL;
	ورنہ
	{
	    صحیح noremap;

#اگر_تعریف_شدہ FEAT_EVAL
	    آخری_مستعمل_map = mp;
	    آخری_مستعمل_sid = -1;
#اگر_ختم
	    اگر (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    ورنہ اگر (
#اگر_تعریف_شدہ FEAT_EVAL
		save_m_اظہاریہ ?
		(save_m_کلیدیں != تہی
			&& STRNCMP(map_str, save_m_کلیدیں, (size_t)keylen) == 0)
		|| (save_alt_m_کلیدیں != تہی
			&& STRNCMP(map_str, save_alt_m_کلیدیں,
					    (size_t)save_alt_m_keylen) == 0) :
#اگر_ختم
		STRNCMP(map_str, mp->m_کلیدیں, (size_t)keylen) == 0
		|| (mp->m_alt != تہی
			&& STRNCMP(map_str, mp->m_alt->m_کلیدیں,
					    (size_t)mp->m_alt->m_keylen) == 0))
		noremap = REMAP_SKIP;
	    ورنہ
		noremap = REMAP_YES;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (save_m_اظہاریہ)
		vim_free(map_str);
#اگر_ختم
	}
#اگر_تعریف_شدہ FEAT_EVAL
	vim_free(save_m_کلیدیں);
	vim_free(save_alt_m_کلیدیں);
#اگر_ختم
	*keylenp = keylen;
	اگر (i == FAIL)
	    واپس map_نتیجہ_fail;
	واپس map_نتیجہ_retry;
    }

    *keylenp = keylen;
    واپس map_نتیجہ_nomatch;
}

/*
 * unget one character (can only be done once!)
 * If the character was stuffed, vgetc() will get it next time it is called.
 * Otherwise vgetc() will only get it when the stuff buffer is empty.
 */
    خالی
vungetc(صحیح c)
{
    old_char = c;
    old_mod_نقاب = mod_نقاب;
    old_mouse_صف = mouse_صف;
    old_mouse_ستون = mouse_ستون;
    old_کلید_Stuffed = کلید_Stuffed;
}

/*
 * When peeking and not getting a character, reg_executing cannot be cleared
 * yet, so set a flag to clear it later.
 */
    ساکن خالی
check_انتہا_reg_executing(صحیح advance)
{
    اگر (reg_executing != 0 && (typebuf.tb_maplen == 0
						|| pending_انتہا_reg_executing))
    {
	اگر (advance)
	{
	    reg_executing = 0;
	    pending_انتہا_reg_executing = FALSE;
	}
	ورنہ
	    pending_انتہا_reg_executing = TRUE;
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
    ساکن صحیح
vgetorpeek(صحیح advance)
{
    صحیح		c;
    صحیح		timedout = FALSE;	// waited for more than 'timeoutlen'
					// for mapping to complete or
					// 'ttimeoutlen' for complete key code
    صحیح		mapdepth = 0;		// check for recursive mapping
    صحیح		منوال_deleted = FALSE;   // set when mode has been deleted
    صحیح		new_wcol, new_wrow;
#اگر_تعریف_شدہ FEAT_GUI
    صحیح		shape_بدلا = FALSE;  // adjusted cursor shape
#اگر_ختم
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
    اگر (vgetc_busy > 0 && ex_معمول_busy == 0)
	واپس NUL;

    ++vgetc_busy;

    اگر (advance)
    {
	کلید_Stuffed = FALSE;
	typebuf_was_خالی_خانے = FALSE;
    }

    init_typebuf();
    آغاز_stuff();
    check_انتہا_reg_executing(advance);
    کرو
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
	ورنہ
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
	ورنہ
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    برائے (;;)
	    {
		طویل	wait_وقت;
		صحیح	keylen = 0;
		صحیح	showcmd_idx;
		check_انتہا_reg_executing(advance);
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		اگر (typebuf.tb_maplen)
		    سطر_breakcheck();
		ورنہ
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
				     && (State & (منوال_داخل_کرو | منوال_CMDLINE)))
			c = ESC;
		    ورنہ
			c = Ctrl_C;
		    flush_buffers(FLUSH_داخلہ);	// flush all typeahead

		    اگر (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    توڑو;
		}
		ورنہ اگر (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_نتیجہ_T نتیجہ = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    اگر (نتیجہ == map_نتیجہ_retry)
			// try mapping again
			جاری;
		    اگر (نتیجہ == map_نتیجہ_fail)
		    {
			// failed, use the outer loop
			c = -1;
			توڑو;
		    }
		    اگر (نتیجہ == map_نتیجہ_get)
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
			    ورنہ
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
			توڑو;  // got character, break the for loop
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
			&& ex_معمول_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & منوال_داخل_کرو)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_کلید && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	ستون = 0;
		    char_u	*ptr;

		    اگر (منوال_displayed)
		    {
			unshowmode(TRUE);
			منوال_deleted = TRUE;
		    }
#اگر_تعریف_شدہ FEAT_GUI
		    // may show a different cursor shape
		    اگر (gui.اندر_use && State != منوال_معمول && !cmd_silent)
		    {
			صحیح	    save_State;

			save_State = State;
			State = منوال_معمول;
			gui_update_cursor(TRUE, FALSE);
			State = save_State;
			shape_بدلا = TRUE;
		    }
#اگر_ختم
		    validate_cursor();
		    old_wcol = curwin->w_wcol;
		    old_wrow = curwin->w_wrow;

		    // move cursor left, if possible
		    اگر (curwin->w_cursor.ستون != 0)
		    {
			اگر (curwin->w_wcol > 0)
			{
			    // After auto-indenting and no text is following,
			    // we are expecting to truncate the trailing
			    // white-space, so find the last non-white
			    // character -- webb
			    اگر (did_ai && *skipwhite(ml_get_curline()
						+ curwin->w_cursor.ستون) == NUL)
			    {
				chartabsize_T cts;

				curwin->w_wcol = 0;
				ptr = ml_get_curline();
				init_chartabsize_arg(&cts, curwin,
					  curwin->w_cursor.lnum, 0, ptr, ptr);
				جب (cts.cts_ptr < ptr + curwin->w_cursor.ستون)
				{
				    اگر (!VIM_ISWHITE(*cts.cts_ptr))
					curwin->w_wcol = cts.cts_vcol;
				    cts.cts_vcol += lbr_chartabsize(&cts);
				    اگر (has_mbyte)
					cts.cts_ptr +=
						   (*mb_ptr2len)(cts.cts_ptr);
				    ورنہ
					++cts.cts_ptr;
				}
				clear_chartabsize_arg(&cts);

				curwin->w_wrow = curwin->w_cline_صف
					   + curwin->w_wcol / curwin->w_چوڑائی;
				curwin->w_wcol %= curwin->w_چوڑائی;
				curwin->w_wcol += curwin_ستون_off();
				ستون = 0;	// no correction needed
			    }
			    ورنہ
			    {
				--curwin->w_wcol;
				ستون = curwin->w_cursor.ستون - 1;
			    }
			}
			ورنہ اگر (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_چوڑائی - 1;
			    ستون = curwin->w_cursor.ستون - 1;
			}
			اگر (has_mbyte && ستون > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    ستون -= (*mb_سر_off)(ptr, ptr + ستون);
			    اگر ((*mb_ptr2cells)(ptr + ستون) > 1)
				--curwin->w_wcol;
			}
		    }
		    setcursor();
		    برآمد_flush();
		    new_wcol = curwin->w_wcol;
		    new_wrow = curwin->w_wrow;
		    curwin->w_wcol = old_wcol;
		    curwin->w_wrow = old_wrow;
		}
		اگر (c < 0)
		    جاری;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		برائے (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		اگر (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    جاری;
		}

		اگر (ex_معمول_busy > 0)
		{
		    ساکن صحیح tc = 0;

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    اگر (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			جاری;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    اگر (p_im && (State & منوال_داخل_کرو))
			c = Ctrl_L;
#اگر_تعریف_شدہ FEAT_TERMINAL
		    ورنہ اگر (terminal_is_فعال())
			c = K_CANCEL;
#اگر_ختم
		    ورنہ اگر ((State & منوال_CMDLINE)
					     || (cmdwin_type > 0 && tc == ESC))
			c = Ctrl_C;
		    ورنہ
			c = ESC;
		    tc = c;
		    // set a flag to indicate this wasn't a normal char
		    اگر (advance)
			typebuf_was_خالی_خانے = TRUE;

		    // return from main_loop()
		    اگر (pending_exmode_فعال)
			exmode_فعال = EXMODE_معمول;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    توڑو;
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
		اگر (((State & منوال_داخل_کرو) != 0 || p_lz)
			&& (State & منوال_CMDLINE) == 0
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
		اگر (typebuf.tb_len > 0 && advance && !exmode_فعال)
		{
		    اگر (((State & (منوال_معمول | منوال_داخل_کرو))
						      || State == منوال_LANGMAP)
			    && State != منوال_HITRETURN)
		    {
			// this looks nice when typing a dead character map
			اگر (State & منوال_داخل_کرو
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
			ڈالو_showcmd();
			اگر (typebuf.tb_len > SHOWCMD_ستون_تعداد)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_ستون_تعداد;
			جب (showcmd_idx < typebuf.tb_len)
			    جوڑو_byte_مقصد_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
		    }

		    // This looks nice when typing a dead character map.
		    // There is no actual command line for get_number().
		    اگر ((State & منوال_CMDLINE)
			    && get_cmdline_info()->cmdbuff != تہی
#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
			    && cmdline_ستارہ == 0
#اگر_ختم
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
			wait_وقت = -1L;
		    ورنہ اگر (keylen == KEYLEN_PART_کلید && p_ttm >= 0)
			wait_وقت = p_ttm;
		    ورنہ
			wait_وقت = p_tm;
		}
		ورنہ
		    wait_وقت = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_وقت);

		اگر (showcmd_idx != 0)
		    نکالو_showcmd();
		اگر (showing_partial)
		{
		    اگر (State & منوال_داخل_کرو)
			edit_unputchar();
		    اگر ((State & منوال_CMDLINE)
					&& get_cmdline_info()->cmdbuff != تہی)
			unputcmdline();
		    ورنہ
			setcursor();	// put cursor back where it belongs
		}

		اگر (c < 0)
		    جاری;		// end of input script reached
		اگر (c == NUL)		// no character available
		{
		    اگر (!advance)
			توڑو;
		    اگر (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			جاری;
		    }
		}
		ورنہ
		{	    // allow mapping for just typed characters
		    جب (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_فعال = im_get_حالت();
#اگر_ختم
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } جب ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    اگر (advance && p_smd && پیغام_silent == 0 && (State & منوال_داخل_کرو))
    {
	اگر (c == ESC && !منوال_deleted && !no_mapping && منوال_displayed)
	{
	    اگر (typebuf.tb_len && !کلید_Typed)
		redraw_cmdline = TRUE;	    // delete mode later
	    ورنہ
		unshowmode(FALSE);
	}
	ورنہ اگر (c != ESC && منوال_deleted)
	{
	    اگر (typebuf.tb_len && !کلید_Typed)
		redraw_cmdline = TRUE;	    // show mode later
	    ورنہ
		showmode();
	}
    }
#اگر_تعریف_شدہ FEAT_GUI
    // may unshow different cursor shape
    اگر (gui.اندر_use && shape_بدلا)
	gui_update_cursor(TRUE, FALSE);
#اگر_ختم
    اگر (timedout && c == ESC)
    {
	// When recording there will be no timeout.  Add an <Ignore> after the
	// ESC to avoid that it forms a key code with following characters.
	gotchars_ignore();
    }

    --vgetc_busy;

    واپس c;
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
    ساکن صحیح
inchar(
    char_u	*buf,
    صحیح		maxlen,
    طویل	wait_وقت)	    // milliseconds
{
    صحیح		len = 0;	    // init for GCC
    صحیح		retesc = FALSE;	    // return ESC with gotint
    صحیح		script_char;
    صحیح		tb_change_cnt = typebuf.tb_change_cnt;

    اگر (wait_وقت == -1L || wait_وقت > 100L)  // flush output before waiting
    {
	cursor_on();
	برآمد_flush_cursor(FALSE, FALSE);
#اگر defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	اگر (gui.اندر_use && postponed_mouseshape)
	    update_mouseshape(-1);
#اگر_ختم
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    اگر (State != منوال_HITRETURN)
    {
	did_outofmem_پیغام = FALSE;   // display out of memory message (again)
	did_swapwrite_پیغام = FALSE;  // display swap file write error again
    }
    undo_off = FALSE;		    // restart undo now

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    جب (scriptin[curscript] != تہی && script_char < 0
#اگر_تعریف_شدہ FEAT_EVAL
	    && !ignore_script
#اگر_ختم
	    )
    {
#اگر_تعریف_شدہ پیغام_قطار
	parse_queued_messages();
#اگر_ختم

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
	    ورنہ
		واپس -1;
	}
	ورنہ
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
#تعریف_کرو DUM_LEN (MAXMAPLEN * 3 + 3)
	    char_u	dum[DUM_LEN + 1];

	    برائے (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		اگر (len == 0 || (len == 1 && dum[0] == Ctrl_C))
		    توڑو;
	    }
	    واپس retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	اگر (wait_وقت == -1L || wait_وقت > 10L)
	    برآمد_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_وقت, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    اگر (typebuf_بدلا(tb_change_cnt))
	واپس 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    اگر (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    واپس fix_داخلہ_بفر(buf, len);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * "buf[]" must have room to triple the number of bytes!
 * Returns the new length.
 */
    صحیح
fix_داخلہ_بفر(char_u *buf, صحیح len)
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
    برائے (i = len; --i >= 0; ++p)
    {
#اگر_تعریف_شدہ FEAT_GUI
	// When the GUI is used any character can come after a CSI, don't
	// escape it.
	اگر (gui.اندر_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
# ifndef MSWIN
	// When not on MS-Windows and the GUI is not used CSI needs to be
	// escaped.
	ورنہ اگر (!gui.اندر_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (صحیح)KE_CSI;
	    len += 2;
	}
# endif
	ورنہ
#اگر_ختم
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
			gui.اندر_use ||
# endif
			(i < 2 || p[1] != KS_MODIFIER))
#اگر_ختم
		    ))
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    p[2] = K_تیسرا(p[0]);
	    p[1] = K_دوسرا(p[0]);
	    p[0] = K_SPECIAL;
	    p += 2;
	    len += 2;
	}
    }
    *p = NUL;		// add trailing NUL
    واپس len;
}

#اگر defined(USE_داخلہ_BUF)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    صحیح
داخلہ_دستیاب(خالی)
{
    واپس (!vim_is_داخلہ_buf_خالی_خانے()
# اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#اگر_ختم

/*
 * Function passed to do_cmdline() to get the command after a <Cmd> key from
 * typeahead.
 */
    ساکن char_u *
getcmdkeycmd(
	صحیح		promptc غیر_مستعمل,
	خالی		*cookie غیر_مستعمل,
	صحیح		indent غیر_مستعمل,
	getline_opt_T	do_concat غیر_مستعمل)
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
    جب (c1 != NUL && !aborted)
    {
	اگر (ga_grow(&سطر_ga, 32) == FAIL)
	{
	    aborted = TRUE;
	    توڑو;
	}

	اگر (vgetorpeek(FALSE) == NUL)
	{
	    // incomplete <Cmd> is an error, because there is not much the user
	    // could do in this state.
	    emsg(_(e_cmd_mapping_must_انتہا_with_cr));
	    aborted = TRUE;
	    توڑو;
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
		جاری;
	    }
	    c1 = مقصد_SPECIAL(c1, c2);

	    // K_ESC is used to avoid ambiguity with the single Esc character
	    // that might be the start of an escape sequence.  Convert it back
	    // to a single Esc here.
	    اگر (c1 == K_ESC)
		c1 = ESC;

#اگر_تعریف_شدہ FEAT_GUI
	    // Translate K_CSI to CSI.  The special key is only used to
	    // avoid it being recognized as the start of a special key.
	    اگر (c1 == K_CSI)
		c1 = CSI;
#اگر_ختم
	}

	اگر (got_int)
	    aborted = TRUE;
	ورنہ اگر (c1 == '\r' || c1 == '\n')
	    c1 = NUL;  // end the line
	ورنہ اگر (c1 == ESC)
	    aborted = TRUE;
	ورنہ اگر (c1 == K_COMMAND || c1 == K_SCRIPT_COMMAND)
	{
	    // give a nicer error message for this special case
	    emsg(_(e_cmd_mapping_must_انتہا_with_cr_before_دوسرا_cmd));
	    aborted = TRUE;
	}
	ورنہ اگر (c1 == K_SNR)
	{
	    GA_CONCAT_LITERAL(&سطر_ga, "<SNR>");
	}
	ورنہ
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
		ga_append(&سطر_ga, K_دوسرا(c1));
		ga_append(&سطر_ga, K_تیسرا(c1));
	    }
	    ورنہ
		ga_append(&سطر_ga, c1);
	}

	cmod = 0;
    }

    no_mapping--;

    اگر (aborted)
	ga_clear(&سطر_ga);

    واپس (char_u *)سطر_ga.ga_ڈیٹا;
}

#اگر defined(FEAT_EVAL)
/*
 * If there was a mapping we get its SID.  Otherwise, use "last_used_sid", it
 * is set when redo'ing.
 * Put this SID in the redo buffer, so that "." will use the same script
 * context.
 */
    خالی
may_جوڑو_آخری_مستعمل_map_مقصد_redobuff(خالی)
{
    char_u  buf[3 + 20];
    صحیح	    buflen;
    صحیح	    sid = -1;

    اگر (آخری_مستعمل_map != تہی)
	sid = آخری_مستعمل_map->m_script_ctx.sc_sid;
    اگر (sid < 0)
	sid = آخری_مستعمل_sid;

    اگر (sid < 0)
	واپس;

    // <K_SID>{nr};
    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = KE_SID;
    buflen = 3;

    buflen += vim_snprintf((حرف *)buf + 3, 20, "%d;", sid);
    جوڑو_buff(&redobuff, buf, (طویل)buflen);
}
#اگر_ختم

    صحیح
do_cmdkey_command(صحیح کلید غیر_مستعمل, صحیح flags)
{
    صحیح	    res;
#اگر_تعریف_شدہ FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    اگر (کلید == K_SCRIPT_COMMAND
		  && (آخری_مستعمل_map != تہی || SCRIPT_ID_VALID(آخری_مستعمل_sid)))
    {
	save_current_sctx = current_sctx;
	اگر (آخری_مستعمل_map != تہی)
	    current_sctx = آخری_مستعمل_map->m_script_ctx;
	ورنہ
	{
	    current_sctx.sc_sid = آخری_مستعمل_sid;
	    current_sctx.sc_lnum = 0;
	    current_sctx.sc_نسخہ = SCRIPT_شے(آخری_مستعمل_sid)->sn_نسخہ;
	}
    }
#اگر_ختم

    res = do_cmdline(تہی, getcmdkeycmd, تہی, flags);

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#اگر_ختم

    واپس res;
}

#اگر defined(FEAT_EVAL)
    خالی
reset_آخری_مستعمل_map(mapblock_T *mp)
{
    اگر (آخری_مستعمل_map != mp)
	واپس;

    آخری_مستعمل_map = تہی;
    آخری_مستعمل_sid = -1;
}
#اگر_ختم
