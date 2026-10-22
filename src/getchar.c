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

#ضمن "vim.h"

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

#تعریف_کرو اقل_ناپ 20			// minimal size for b_str

ساكن buffheader_T redobuff = {{عدم, 0, {NUL}}, عدم, 0, 0, FALSE};
ساكن buffheader_T old_redobuff = {{عدم, 0, {NUL}}, عدم, 0, 0, FALSE};
ساكن buffheader_T recordbuff = {{عدم, 0, {NUL}}, عدم, 0, 0, FALSE};

ساكن صحيح typeahead_char = 0;		// typeahead char that's not flushed

#جے_تعریف_شدہ FEAT_EVAL
ساكن char_u typedchars[MAXMAPLEN + 1] = { NUL };  // typed chars before map
ساكن صحيح typedchars_pos = 0;
#جے_انت

/*
 * When block_redo is TRUE the redo buffer will not be changed.
 * Used by edit() to repeat insertions.
 */
ساكن صحيح	بلاک_redo = FALSE;

ساكن صحيح	کلید_Noremap = 0;	    // remapping flags

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
#جے_تعریف_شدہ FEAT_عام
// Add extra space to handle large OSC responses in bigger chunks (improve
// performance)
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3) + 2048)
#وإلا // Tiny version
# define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
#جے_انت
ساكن char_u	typebuf_init[TYPELEN_INIT];	// initial typebuf.tb_buf
ساكن char_u	noremapbuf_init[TYPELEN_INIT];	// initial typebuf.tb_noremap

ساكن size_t	آخری_recorded_len = 0;	// number of last recorded chars

#جے_تعریف_شدہ FEAT_EVAL
mapblock_T	*آخری_ورتیا_map = عدم;
صحيح		آخری_ورتیا_sid = -1;
#جے_انت

ساكن صحيح	read_readbuf(buffheader_T *buf, صحيح advance);
ساكن فراغ	init_typebuf(فراغ);
ساكن فراغ	may_sync_undo(فراغ);
ساكن فراغ	free_typebuf(فراغ);
ساكن فراغ	closescript(فراغ);
ساكن فراغ	updatescript(صحيح c);
ساكن صحيح	vgetorpeek(صحيح);
ساكن صحيح	inchar(char_u *buf, صحيح maxlen, لما wait_ویلا);
#جے_تعریف_شدہ FEAT_EVAL
ساكن صحيح	do_کلید_داخلہ_pre(صحيح c);
#جے_انت

/*
 * Free and clear a buffer.
 */
    ساكن فراغ
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    لكل (p = buf->bh_پہلا.b_اگلا; p != عدم; p = np)
    {
	np = p->b_اگلا;
	vim_free(p);
    }
    buf->bh_پہلا.b_اگلا = عدم;
    buf->bh_curr = عدم;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    ساكن char_u *
get_buffcont(
    buffheader_T	*بفر,
    صحيح			dozero,	    // count == zero is not an error
    size_t		*len)	    // the length of the returned buffer
{
    long_u	    گنتی = 0;
    char_u	    *p = عدم;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;
    size_t	    i = 0;

    // compute the total length of the string
    لكل (bp = بفر->bh_پہلا.b_اگلا; bp != عدم; bp = bp->b_اگلا)
	گنتی += (long_u)bp->b_strlen;

    إذا ((گنتی > 0 || dozero) && (p = alloc(گنتی + 1)) != عدم)
    {
	p2 = p;
	لكل (bp = بفر->bh_پہلا.b_اگلا; bp != عدم; bp = bp->b_اگلا)
	    لكل (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
	i = (size_t)(p2 - p);
    }

    إذا (len != عدم)
	*len = i;

    أرجع p;
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(فراغ)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE, &len);
    إذا (p == عدم)
	أرجع عدم;

    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    إذا (len >= آخری_recorded_len)
    {
	len -= آخری_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    إذا (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    أرجع (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    string_T
get_inserted(فراغ)
{
    size_t len = 0;
    char_u *str = get_buffcont(&redobuff, FALSE, &len);
    string_T ret = { str, len };
    أرجع ret;
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    ساكن فراغ
جوڑو_buff(
    buffheader_T	*buf,
    char_u		*s,
    لما		slen)	// length of "s" or -1
{
    إذا (slen < 0)
	slen = (لما)STRLEN(s);
    إذا (slen == 0)				// don't add empty strings
	أرجع;

    إذا (buf->bh_پہلا.b_اگلا == عدم)	// first add to list
    {
	buf->bh_curr = &(buf->bh_پہلا);
	buf->bh_create_newblock = TRUE;
    }
    وإلا إذا (buf->bh_curr == عدم)	// buffer has already been read
    {
	iemsg(e_جوڑو_منزل_اندرونی_بفر_that_was_already_read_from);
	أرجع;
    }
    وإلا إذا (buf->bh_اشاریہ != 0)
    {
	mch_memmove(buf->bh_پہلا.b_اگلا->b_str,
		    buf->bh_پہلا.b_اگلا->b_str + buf->bh_اشاریہ,
		    (buf->bh_پہلا.b_اگلا->b_strlen - buf->bh_اشاریہ) + 1);
	buf->bh_پہلا.b_اگلا->b_strlen -= buf->bh_اشاریہ;
	buf->bh_space += buf->bh_اشاریہ;
    }
    buf->bh_اشاریہ = 0;

    إذا (!buf->bh_create_newblock && buf->bh_space >= (صحيح)slen)
    {
	vim_strncpy(buf->bh_curr->b_str + buf->bh_curr->b_strlen, s, (size_t)slen);
	buf->bh_curr->b_strlen += slen;
	buf->bh_space -= slen;
    }
    وإلا
    {
	long_u	    len;
	buffblock_T *p;

	إذا (slen < اقل_ناپ)
	    len = اقل_ناپ;
	وإلا
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	إذا (p == عدم)
	    أرجع; // no space, just forget it
	vim_strncpy(p->b_str, s, (size_t)slen);
	p->b_strlen = slen;
	buf->bh_space = (صحيح)(len - slen);
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
    ساكن فراغ
delete_buff_پوچھ(buffheader_T *buf, صحيح slen)
{
    إذا (buf->bh_curr == عدم)
	أرجع;  // nothing to delete
    إذا (buf->bh_curr->b_strlen < (size_t)slen)
	أرجع;

    buf->bh_curr->b_str[buf->bh_curr->b_strlen - (size_t)slen] = NUL;
    buf->bh_curr->b_strlen -= slen;
    buf->bh_space += slen;
}

/*
 * Add number "n" to buffer "buf".
 */
    ساكن فراغ
جوڑو_عدد_buff(buffheader_T *buf, لما n)
{
    char_u	نمبر[32];
    صحيح		numberlen;

    numberlen = vim_snprintf((حرف *)نمبر, حجم(نمبر), "%ld", n);
    جوڑو_buff(buf, نمبر, (لما)numberlen);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    ساكن فراغ
جوڑو_char_buff(buffheader_T *buf, صحيح c)
{
    char_u	بائٹاں[MB_MAXBYTES + 1];
    صحيح		len;
    صحيح		i;
    char_u	عارضی[4];
    لما	templen;

    إذا (IS_SPECIAL(c))
	len = 1;
    وإلا
	len = (*mb_char2bytes)(c, بائٹاں);
    لكل (i = 0; i < len; ++i)
    {
	إذا (!IS_SPECIAL(c))
	    c = بائٹاں[i];

	إذا (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    عارضی[0] = K_SPECIAL;
	    عارضی[1] = K_دوجا(c);
	    عارضی[2] = K_تیجا(c);
	    عارضی[3] = NUL;
	    templen = 3;
	}
#جے_تعریف_شدہ FEAT_GUI
	وإلا إذا (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    عارضی[0] = CSI;
	    عارضی[1] = KS_EXTRA;
	    عارضی[2] = (صحيح)KE_CSI;
	    عارضی[3] = NUL;
	    templen = 3;
	}
#جے_انت
	وإلا
	{
	    عارضی[0] = c;
	    عارضی[1] = NUL;
	    templen = 1;
	}
	جوڑو_buff(buf, عارضی, templen);
    }
}

// First read ahead buffer. Used for translated commands.
ساكن buffheader_T readbuf1 = {{عدم, 0, {NUL}}, عدم, 0, 0, FALSE};

// Second read ahead buffer. Used for redo.
ساكن buffheader_T readbuf2 = {{عدم, 0, {NUL}}, عدم, 0, 0, FALSE};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    ساكن صحيح
read_readbuffers(صحيح advance)
{
    صحيح c;

    c = read_readbuf(&readbuf1, advance);
    إذا (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    أرجع c;
}

    ساكن صحيح
read_readbuf(buffheader_T *buf, صحيح advance)
{
    char_u	c;
    buffblock_T	*curr;

    إذا (buf->bh_پہلا.b_اگلا == عدم)  // buffer is empty
	أرجع NUL;

    curr = buf->bh_پہلا.b_اگلا;
    c = curr->b_str[buf->bh_اشاریہ];

    إذا (advance)
    {
	إذا (curr->b_str[++buf->bh_اشاریہ] == NUL)
	{
	    buf->bh_پہلا.b_اگلا = curr->b_اگلا;
	    vim_free(curr);
	    buf->bh_اشاریہ = 0;
	}
    }
    أرجع c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    ساكن فراغ
شروع_stuff(فراغ)
{
    إذا (readbuf1.bh_پہلا.b_اگلا != عدم)
    {
	readbuf1.bh_curr = &(readbuf1.bh_پہلا);
	readbuf1.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
    إذا (readbuf2.bh_پہلا.b_اگلا != عدم)
    {
	readbuf2.bh_curr = &(readbuf2.bh_پہلا);
	readbuf2.bh_create_newblock = TRUE;		// force a new block to be created (see add_buff())
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    صحيح
stuff_خالی_تھاواں(فراغ)
{
    أرجع (readbuf1.bh_پہلا.b_اگلا == عدم
	 && readbuf2.bh_پہلا.b_اگلا == عدم);
}

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * readbuf2.
 */
    صحيح
readbuf1_خالی_تھاواں(فراغ)
{
    أرجع (readbuf1.bh_پہلا.b_اگلا == عدم);
}
#جے_انت

/*
 * Set a typeahead character that won't be flushed.
 */
    فراغ
typeahead_noflush(صحيح c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    فراغ
flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    شروع_stuff();
    طالما (read_readbuffers(TRUE) != NUL)
	;

    إذا (flush_typeahead == FLUSH_اقل)
    {
	// remove mapped characters at the start only,
	// but only when enough space left in typebuf
	إذا (typebuf.tb_off + typebuf.tb_maplen >= typebuf.tb_buflen)
	{
	    typebuf.tb_off = MAXMAPLEN;
	    typebuf.tb_len = 0;
	}
	وإلا
	{
	    typebuf.tb_off += typebuf.tb_maplen;
	    typebuf.tb_len -= typebuf.tb_maplen;
	}
#إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	إذا (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#جے_انت
    }
    وإلا
    {
	// remove typeahead
	إذا (flush_typeahead == FLUSH_داخلہ)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    طالما (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#جے_انت
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
    إذا (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * The previous contents of the redo buffer is kept in old_redobuffer.
 * This is used for the CTRL-O <.> command in insert mode.
 */
    فراغ
ResetRedobuff(فراغ)
{
    إذا (بلاک_redo)
	أرجع;

    free_buff(&old_redobuff);
    old_redobuff = redobuff;
    redobuff.bh_پہلا.b_اگلا = عدم;
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    فراغ
CancelRedo(فراغ)
{
    إذا (بلاک_redo)
	أرجع;

    free_buff(&redobuff);
    redobuff = old_redobuff;
    old_redobuff.bh_پہلا.b_اگلا = عدم;
    شروع_stuff();
    طالما (read_readbuffers(TRUE) != NUL)
	;
}

/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
    فراغ
saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;
    size_t	slen;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_پہلا.b_اگلا = عدم;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_پہلا.b_اگلا = عدم;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE, &slen);
    إذا (s == عدم)
	أرجع;

    جوڑو_buff(&redobuff, s, (لما)slen);
    vim_free(s);
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    فراغ
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
    فراغ
Append_منزل_Redobuff(char_u *s)
{
    إذا (!بلاک_redo)
	جوڑو_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    فراغ
Append_منزل_Redobuff_Lit(
    char_u	*str,
    صحيح		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    صحيح		c;
    char_u	*شروع;

    إذا (بلاک_redo)
	أرجع;

    طالما (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	شروع = s;
	طالما (*s >= ' ' && *s < DEL && (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	إذا (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	إذا (s > شروع)
	    جوڑو_buff(&redobuff, شروع, (لما)(s - شروع));

	إذا (*s == NUL || (len >= 0 && s - str >= len))
	    اكسر;

	// Handle a special or multibyte character.
	إذا (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	وإلا
	    c = *s++;
	إذا (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    جوڑو_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048
	إذا (*s == NUL && c == '0')
	    جوڑو_buff(&redobuff, (char_u *)"048", 3L);
	وإلا
	    جوڑو_char_buff(&redobuff, c);
    }
}

/*
 * Append "s" to the redo buffer, leaving 3-byte special key codes unmodified
 * and escaping other K_SPECIAL and CSI bytes.
 */
    فراغ
Append_منزل_Redobuff_Spec(char_u *s)
{
    إذا (بلاک_redo)
	أرجع;

    طالما (*s != NUL)
    {
	إذا (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    جوڑو_buff(&redobuff, s, 3L);
	    s += 3;
	}
	وإلا
	    جوڑو_char_buff(&redobuff, mb_cptr2char_adv(&s));
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    فراغ
Append_Char_منزل_Redobuff(صحيح c)
{
    إذا (!بلاک_redo)
	جوڑو_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    فراغ
Append_نمبر_منزل_Redobuff(لما n)
{
    إذا (!بلاک_redo)
	جوڑو_عدد_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    فراغ
stuffReadbuff(char_u *s)
{
    جوڑو_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    فراغ
stuffRedoReadbuff(char_u *s)
{
    جوڑو_buff(&readbuf2, s, -1L);
}

    فراغ
stuffReadbuffLen(char_u *s, لما len)
{
#جے_تعریف_شدہ FEAT_EVAL
    إذا (جوڑو_آخری_داخل_کرو == 1) // Only add if this is the first call, for
			      // recursive calls, ignore.
	ga_concat_len(&آخری_داخل_کرو_ga, s, (size_t)len);
#جے_انت
    جوڑو_buff(&readbuf1, s, len);
}

#إذا defined(FEAT_EVAL)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    فراغ
stuffReadbuffSpec(char_u *s)
{
    صحيح c;

    طالما (*s != NUL)
    {
	إذا (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	وإلا
	{
	    c = mb_cptr2char_adv(&s);
	    إذا (c == CAR || c == NL || c == ESC)
		c = ' ';
	    stuffcharReadbuff(c);
	}
    }
}
#جے_انت

/*
 * Append a character to the stuff buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    فراغ
stuffcharReadbuff(صحيح c)
{
    جوڑو_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    فراغ
stuffnumReadbuff(لما n)
{
    جوڑو_عدد_buff(&readbuf1, n);
}

/*
 * Stuff a string into the typeahead buffer, such that edit() will insert it
 * literally ("literally" TRUE) or interpret is as typed characters.
 */
    فراغ
stuffescaped(char_u *arg, صحيح literally)
{
    صحيح		c;
    char_u	*شروع;

    طالما (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	شروع = arg;
	طالما ((*arg >= ' ' && *arg < DEL)
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	إذا (arg > شروع)
	    stuffReadbuffLen(شروع, (لما)(arg - شروع));

	// stuff a single special character
	إذا (*arg != NUL)
	{
	    إذا (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    وإلا
		c = *arg++;
	    إذا (literally && ((c < ' ' && c != TAB) || c == DEL))
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
    ساكن صحيح
read_redo(صحيح init, صحيح old_redo)
{
    ساكن buffblock_T	*bp;
    ساكن char_u	*p;
    صحيح			c;
    صحيح			n;
    char_u		buf[MB_MAXBYTES + 1];
    صحيح			i;

    إذا (init)
    {
	إذا (old_redo)
	    bp = old_redobuff.bh_پہلا.b_اگلا;
	وإلا
	    bp = redobuff.bh_پہلا.b_اگلا;
	إذا (bp == عدم)
	    أرجع FAIL;
	p = bp->b_str;
	أرجع OK;
    }
    إذا ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	إذا (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	وإلا
	    n = 1;
	لكل (i = 0; ; ++i)
	{
	    إذا (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = منزل_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#جے_تعریف_شدہ FEAT_GUI
	    إذا (c == CSI)	// escaped CSI
		p += 2;
#جے_انت
	    إذا (*++p == NUL && bp->b_اگلا != عدم)
	    {
		bp = bp->b_اگلا;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    إذا (i == n - 1)	// last byte of a character
	    {
		إذا (n != 1)
		    c = (*mb_ptr2char)(buf);
		اكسر;
	    }
	    c = *p;
	    إذا (c == NUL)	// cannot happen?
		اكسر;
	}
    }

    أرجع c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    ساكن فراغ
copy_redo(صحيح old_redo)
{
    صحيح	    c;

    طالما ((c = read_redo(FALSE, old_redo)) != NUL)
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
    صحيح
شروع_redo(لما گنتی, صحيح old_redo)
{
    صحيح	    c;

    // init the pointers; return if nothing to redo
    إذا (read_redo(TRUE, old_redo) == FAIL)
	أرجع FAIL;

    c = read_redo(FALSE, old_redo);

#جے_تعریف_شدہ FEAT_EVAL
    إذا (c == K_SID)
    {
	// Copy the <SID>{sid}; sequence
	جوڑو_char_buff(&readbuf2, c);
	لكل (;;)
	{
	    c = read_redo(FALSE, old_redo);
	    جوڑو_char_buff(&readbuf2, c);
	    إذا (!محفوظ_isdigit(c))
		اكسر;
	}
	c = read_redo(FALSE, old_redo);
    }
#جے_انت

    // copy the buffer name, if present
    إذا (c == '"')
    {
	جوڑو_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	// if a numbered buffer is used, increment the number
	إذا (c >= '1' && c < '9')
	    ++c;
	جوڑو_char_buff(&readbuf2, c);

	// the expression register should be re-evaluated
	إذا (c == '=')
	{
	    جوڑو_char_buff(&readbuf2, CAR);
	    cmd_silent = TRUE;
	}

	c = read_redo(FALSE, old_redo);
    }

    إذا (c == 'v')   // redo Visual
    {
	VIsual = curwin->w_cursor;
	VIsual_سرگرم = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    // try to enter the count (in place of a previous count)
    إذا (گنتی)
    {
	طالما (VIM_ISDIGIT(c))	// skip "old" count
	    c = read_redo(FALSE, old_redo);
	جوڑو_عدد_buff(&readbuf2, گنتی);
    }

    // copy the rest from the redo buffer into the stuff buffer
    جوڑو_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    أرجع OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    صحيح
شروع_redo_ins(فراغ)
{
    صحيح	    c;

    إذا (read_redo(TRUE, FALSE) == FAIL)
	أرجع FAIL;
    شروع_stuff();

    // skip the count and the command character
    طالما ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	إذا (vim_strchr((char_u *)"AaIiRrOo", c) != عدم)
	{
	    إذا (c == 'O' || c == 'o')
		جوڑو_buff(&readbuf2, NL_STR, -1L);
	    اكسر;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    بلاک_redo = TRUE;
    أرجع OK;
}

    فراغ
stop_redo_ins(فراغ)
{
    بلاک_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    ساكن فراغ
init_typebuf(فراغ)
{
    إذا (typebuf.tb_buf != عدم)
	أرجع;

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
    صحيح
noremap_کلیدیں(فراغ)
{
    أرجع کلید_Noremap & (RM_NONE|RM_SCRIPT);
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
    صحيح
ins_typebuf(
    char_u	*str,
    صحيح		noremap,
    صحيح		وتھ,
    صحيح		nottyped,
    صحيح		silent)
{
    char_u	*s1, *s2;
    صحيح		newlen;
    صحيح		addlen;
    صحيح		i;
    صحيح		newoff;
    صحيح		val;
    صحيح		nrm;

    init_typebuf();
    إذا (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
    state_no_longer_محفوظ("ins_typebuf()");

    addlen = (صحيح)STRLEN(str);

    إذا (وتھ == 0 && addlen <= typebuf.tb_off)
    {
	/*
	 * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
	 */
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    وإلا إذا (typebuf.tb_len == 0 && typebuf.tb_buflen
					       >= addlen + 3 * (MAXMAPLEN + 4))
    {
	/*
	 * Buffer is empty and string fits in the existing buffer.
	 * Leave some space before and after, if possible.
	 */
	typebuf.tb_off = (typebuf.tb_buflen - addlen - 3 * (MAXMAPLEN + 4)) / 2;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }
    وإلا
    {
	صحيح extra;

	/*
	 * Need to allocate a new buffer.
	 * In typebuf.tb_buf there must always be room for 3 * (MAXMAPLEN + 4)
	 * characters.  We add some extra room to avoid having to allocate too
	 * often.
	 */
	newoff = MAXMAPLEN + 4;
	extra = addlen + newoff + 4 * (MAXMAPLEN + 4);
	إذا (typebuf.tb_len > 2147483647 - extra)
	{
	    // string is getting too long for a 32 bit int
	    emsg(_(e_command_too_complex));    // also calls flush_buffers
	    setcursor();
	    أرجع FAIL;
	}
	newlen = typebuf.tb_len + extra;
	s1 = alloc(newlen);
	إذا (s1 == عدم)		    // out of memory
	    أرجع FAIL;
	s2 = alloc(newlen);
	إذا (s2 == عدم)		    // out of memory
	{
	    vim_free(s1);
	    أرجع FAIL;
	}
	typebuf.tb_buflen = newlen;

	// copy the old chars, before the insertion point
	mch_memmove(s1 + newoff, typebuf.tb_buf + typebuf.tb_off,
							      (size_t)وتھ);
	// copy the new chars
	mch_memmove(s1 + newoff + وتھ, str, (size_t)addlen);
	// copy the old chars, after the insertion point, including the	NUL at
	// the end
	mch_memmove(s1 + newoff + وتھ + addlen,
				     typebuf.tb_buf + typebuf.tb_off + وتھ,
				       (size_t)(typebuf.tb_len - وتھ + 1));
	إذا (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)وتھ);
	mch_memmove(s2 + newoff + وتھ + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + وتھ,
					   (size_t)(typebuf.tb_len - وتھ));
	إذا (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    // If noremap == REMAP_SCRIPT: do remap script-local mappings.
    إذا (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    وإلا إذا (noremap == REMAP_SKIP)
	val = RM_ABBR;
    وإلا
	val = RM_NONE;

    /*
     * Adjust typebuf.tb_noremap[] for the new characters:
     * If noremap == REMAP_NONE or REMAP_SCRIPT: new characters are
     *			(sometimes) not remappable
     * If noremap == REMAP_YES: all the new characters are mappable
     * If noremap  > 0: "noremap" characters are not remappable, the rest
     *			mappable
     */
    إذا (noremap == REMAP_SKIP)
	nrm = 1;
    وإلا إذا (noremap < 0)
	nrm = addlen;
    وإلا
	nrm = noremap;
    لكل (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + وتھ] =
						  (--nrm >= 0) ? val : RM_YES;

    // tb_maplen and tb_silent only remember the length of mapped and/or
    // silent mappings at the start of the buffer, assuming that a mapped
    // sequence doesn't result in typed characters.
    إذا (nottyped || typebuf.tb_maplen > وتھ)
	typebuf.tb_maplen += addlen;
    إذا (silent || typebuf.tb_silent > وتھ)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    إذا (typebuf.tb_no_abbr_cnt && وتھ == 0)	// and not used for abbrev.s
	typebuf.tb_no_abbr_cnt += addlen;

    أرجع OK;
}

/*
 * Put character "c" back into the typeahead buffer.
 * Can be used for a character obtained by vgetc() that needs to be put back.
 * Uses cmd_silent, KeyTyped and KeyNoremap to restore the flags belonging to
 * the char.
 * Returns the length of what was inserted.
 */
    صحيح
ins_char_typebuf(صحيح c, صحيح modifiers)
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    صحيح		len = special_منزل_buf(c, modifiers, TRUE, buf);

    buf[len] = NUL;
    (فراغ)ins_typebuf(buf, کلید_Noremap, 0, !کلید_Typed, cmd_silent);
    أرجع len;
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
    صحيح
typebuf_بدلیا(
    صحيح		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    أرجع (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#جے_انت
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    صحيح
typebuf_typed(فراغ)
{
    أرجع typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    صحيح
typebuf_maplen(فراغ)
{
    أرجع typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    فراغ
del_typebuf(صحيح len, صحيح وتھ)
{
    صحيح	    i;

    إذا (len == 0)
	أرجع;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    إذا (وتھ == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    وإلا
    {
	i = typebuf.tb_off + وتھ;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	إذا (typebuf.tb_off > MAXMAPLEN)
	{
	    mch_memmove(typebuf.tb_buf + MAXMAPLEN,
			     typebuf.tb_buf + typebuf.tb_off, (size_t)وتھ);
	    mch_memmove(typebuf.tb_noremap + MAXMAPLEN,
			 typebuf.tb_noremap + typebuf.tb_off, (size_t)وتھ);
	    typebuf.tb_off = MAXMAPLEN;
	}
	// adjust typebuf.tb_buf (include the NUL at the end)
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + وتھ,
						     typebuf.tb_buf + i + len,
				       (size_t)(typebuf.tb_len - وتھ + 1));
	// adjust typebuf.tb_noremap[]
	mch_memmove(typebuf.tb_noremap + typebuf.tb_off + وتھ,
						 typebuf.tb_noremap + i + len,
					   (size_t)(typebuf.tb_len - وتھ));
    }

    إذا (typebuf.tb_maplen > وتھ)		// adjust tb_maplen
    {
	إذا (typebuf.tb_maplen < وتھ + len)
	    typebuf.tb_maplen = وتھ;
	وإلا
	    typebuf.tb_maplen -= len;
    }
    إذا (typebuf.tb_silent > وتھ)		// adjust tb_silent
    {
	إذا (typebuf.tb_silent < وتھ + len)
	    typebuf.tb_silent = وتھ;
	وإلا
	    typebuf.tb_silent -= len;
    }
    إذا (typebuf.tb_no_abbr_cnt > وتھ)	// adjust tb_no_abbr_cnt
    {
	إذا (typebuf.tb_no_abbr_cnt < وتھ + len)
	    typebuf.tb_no_abbr_cnt = وتھ;
	وإلا
	    typebuf.tb_no_abbr_cnt -= len;
    }

#إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#جے_انت
    إذا (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * State for adding bytes to a recording or 'showcmd'.
 */
تعريفنوع بنية
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    صحيح		prev_c;
    size_t	buflen;
    بنانشانی	pending_special;
    بنانشانی	pending_mbyte;
} gotchars_state_T;

/*
 * Add a single byte to a recording or 'showcmd'.
 * Return TRUE if a full key has been received, FALSE otherwise.
 */
    ساكن صحيح
gotchars_جوڑو_byte(gotchars_state_T *state, char_u byte)
{
    صحيح		c = state->buf[state->buflen++] = byte;
    صحيح		retval = FALSE;
    صحيح		وچ_special = state->pending_special > 0;
    صحيح		وچ_mbyte = state->pending_mbyte > 0;

    إذا (وچ_special)
	state->pending_special--;
    وإلا إذا (c == K_SPECIAL
#جے_تعریف_شدہ FEAT_GUI
		|| c == CSI
#جے_انت
	    )
	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	state->pending_special = 2;

    إذا (state->pending_special > 0)
	اذهب ret_false;

    إذا (وچ_mbyte)
	state->pending_mbyte--;
    وإلا
    {
	إذا (وچ_special)
	{
	    إذا (state->prev_c == KS_MODIFIER)
		// When receiving a modifier, wait for the modified key.
		اذهب ret_false;
	    c = منزل_SPECIAL(state->prev_c, c);
	    // Drop K_FOCUSGAINED, K_FOCUSLOST and K_COMPLETE_DELAY, they are
	    // not useful in a recording.
	    إذا (c == K_FOCUSGAINED || c == K_FOCUSLOST || c == K_COMPLETE_DELAY)
		state->buflen = 0;
	}
	// When receiving a multibyte character, store it until we have all
	// the bytes, so that it won't be split between two buffer blocks,
	// and delete_buff_tail() will work properly.
	state->pending_mbyte = MB_BYTE2LEN_CHECK(c) - 1;
    }

    إذا (state->pending_mbyte > 0)
	اذهب ret_false;

    retval = TRUE;
ret_false:
    state->prev_c = c;
    أرجع retval;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the record buffer.
 */
    ساكن فراغ
gotchars(char_u *chars, صحيح len)
{
    char_u		*s = chars;
    size_t		i;
    صحيح			todo = len;
    ساكن gotchars_state_T state;

    طالما (todo-- > 0)
    {
	إذا (!gotchars_جوڑو_byte(&state, *s++))
	    استمر;

	// Handle one byte at a time; no translation to be done.
	لكل (i = 0; i < state.buflen; ++i)
	    updatescript(state.buf[i]);

	إذا (reg_recording != 0)
	{
	    state.buf[state.buflen] = NUL;
	    جوڑو_buff(&recordbuff, state.buf, (لما)state.buflen);
	    // remember how many chars were last recorded
	    آخری_recorded_len += state.buflen;
	}
	state.buflen = 0;
    }

    may_sync_undo();

#جے_تعریف_شدہ FEAT_EVAL
    // output "debug mode" message next time in debug mode
    debug_did_پیغام = FALSE;
#جے_انت

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}

/*
 * Record an <Ignore> key.
 */
    فراغ
gotchars_ignore(فراغ)
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
    فراغ
ungetchars(صحيح len)
{
    إذا (reg_recording == 0)
	أرجع;

    delete_buff_پوچھ(&recordbuff, len);
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
    ساكن فراغ
may_sync_undo(فراغ)
{
    إذا ((!(State & (منوال_داخل_کرو | منوال_CMDLINE)) || arrow_ورتیا)
					       && scriptin[curscript] == عدم)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    ساكن صحيح
alloc_typebuf(فراغ)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    إذا (typebuf.tb_buf == عدم || typebuf.tb_noremap == عدم)
    {
	free_typebuf();
	أرجع FAIL;
    }
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_off = MAXMAPLEN + 4;  // can insert without realloc
    typebuf.tb_len = 0;
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    typebuf.tb_no_abbr_cnt = 0;
    إذا (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
#إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    typebuf_was_filled = FALSE;
#جے_انت
    أرجع OK;
}

/*
 * Free the buffers of "typebuf".
 */
    ساكن فراغ
free_typebuf(فراغ)
{
    إذا (typebuf.tb_buf == typebuf_init)
	اندرونی_error("Free typebuf 1");
    وإلا
	VIM_CLEAR(typebuf.tb_buf);
    إذا (typebuf.tb_noremap == noremapbuf_init)
	اندرونی_error("Free typebuf 2");
    وإلا
	VIM_CLEAR(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
ساكن typebuf_T saved_typebuf[NSCRIPT];

    صحيح
save_typebuf(فراغ)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    إذا (alloc_typebuf() == FAIL)
    {
	closescript();
	أرجع FAIL;
    }
    أرجع OK;
}

ساكن صحيح old_char = -1;	// character put back by vungetc()
ساكن صحيح old_mod_نقاب;	// mod_mask for ungotten character
ساكن صحيح old_mouse_صف;	// mouse_row related to old_char
ساكن صحيح old_mouse_خانہ;	// mouse_col related to old_char
ساكن صحيح old_کلید_Stuffed;	// whether old_char was stuffed

ساكن صحيح can_get_old_char(فراغ)
{
    // If the old character was not stuffed and characters have been added to
    // the stuff buffer, need to first get the stuffed characters instead.
    أرجع old_char != -1 && (old_کلید_Stuffed || stuff_خالی_تھاواں());
}

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    فراغ
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    إذا (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_نقاب = old_mod_نقاب;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_پہلا.b_اگلا = عدم;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_پہلا.b_اگلا = عدم;
#جے_تعریف_شدہ USE_داخلہ_BUF
    tp->save_inputbuf = get_داخلہ_buf();
#جے_انت
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    فراغ
restore_typeahead(tasave_T *tp, صحيح overwrite بن_ورتیا)
{
    إذا (tp->typebuf_valid)
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
#جے_تعریف_شدہ USE_داخلہ_BUF
    سیٹ_داخلہ_buf(tp->save_inputbuf, overwrite);
#جے_انت
}

/*
 * Open a new script file for the ":source!" command.
 */
    فراغ
openscript(
    char_u	*name,
    صحيح		directly)	// when TRUE execute directly
{
    إذا (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	أرجع;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    إذا (check_secure())
	أرجع;

#جے_تعریف_شدہ FEAT_EVAL
    إذا (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	أرجع;
#جے_انت

    إذا (scriptin[curscript] != عدم)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    إذا ((scriptin[curscript] = mch_fopen((حرف *)NameBuff, READBIN)) == عدم)
    {
	semsg(_(e_cant_open_file_str), name);
	إذا (curscript)
	    --curscript;
	أرجع;
    }
    إذا (save_typebuf() == FAIL)
	أرجع;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    إذا (directly)
    {
	oparg_T	oa;
	صحيح	oldcurscript;
	صحيح	save_State = State;
	صحيح	save_restart_edit = restart_edit;
	صحيح	save_insertmode = p_im;
	صحيح	save_انت_op = انت_op;
	صحيح	save_پیغام_scroll = پیغام_scroll;

	State = منوال_عام;
	پیغام_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	انت_op = FALSE;

	oldcurscript = curscript;
	افعل
	{
	    update_topline_cursor();	// update cursor position and topline
	    عام_cmd(&oa, FALSE);	// execute one command
	    (فراغ)vpeekc();		// check for end of file
	}
	طالما (scriptin[oldcurscript] != عدم);

	State = save_State;
	پیغام_scroll = save_پیغام_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	انت_op = save_انت_op;
    }
}

/*
 * Close the currently active input script.
 */
    ساكن فراغ
closescript(فراغ)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = عدم;
    إذا (curscript > 0)
	--curscript;
}

#إذا defined(EXITFREE)
    فراغ
close_all_scripts(فراغ)
{
    طالما (scriptin[0] != عدم)
	closescript();
}
#جے_انت

/*
 * Return TRUE when reading keys from a script file.
 */
    صحيح
using_script(فراغ)
{
    أرجع scriptin[curscript] != عدم;
}

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    فراغ
before_blocking(فراغ)
{
    // Tell terminal to flush screen contents before blocking
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_FLUSH);

    updatescript(0);
#جے_تعریف_شدہ FEAT_EVAL
    إذا (may_garbage_collect)
	garbage_collect(FALSE);
#جے_انت
}

/*
 * updatescript() is called when a character can be written into the script
 * file or when we have waited some time for a character (c == 0)
 *
 * All the changed memfiles are synced if c == 0 or when the number of typed
 * characters reaches 'updatecount' and 'updatecount' is non-zero.
 */
    ساكن فراغ
updatescript(صحيح c)
{
    ساكن صحيح	    گنتی = 0;

    إذا (c && scriptout)
	putc(c, scriptout);
    إذا (c == 0 || (p_uc > 0 && ++گنتی >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	گنتی = 0;
    }
#جے_تعریف_شدہ FEAT_EVAL
    إذا (typedchars_pos < MAXMAPLEN)
    {
	typedchars[typedchars_pos] = c;
	typedchars_pos++;
    }
#جے_انت
}

/*
 * Convert "c_arg" plus "modifiers" to merge the effect of modifyOtherKeys into
 * the character.  Also for when the Kitty key protocol is used.
 */
    صحيح
ملاؤ_modify_Other_کلیدیں(صحيح c_arg, صحيح *modifiers)
{
    صحيح c = c_arg;

    // CTRL only uses the lower 5 bits of the character.
    إذا (*modifiers & MOD_نقاب_CTRL)
    {
	إذا ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	{
	    c &= 0x1f;
	    إذا (c == NUL)
		c = K_ZERO;
	}
	وإلا إذا (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#جے_تعریف_شدہ FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	وإلا إذا (c == '2')
	    c = NUL;
	وإلا إذا (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	وإلا إذا (c == '8')
	    c = BS;
	وإلا إذا (c == '?')
	    c = DEL;
#جے_انت
	إذا (c != c_arg)
	    *modifiers &= ~MOD_نقاب_CTRL;
    }

    // Alt/Meta sets the 8th bit of the character.
    إذا ((*modifiers & (MOD_نقاب_META | MOD_نقاب_ALT))
	    && c >= 0 && c <= 127)
    {
	// Some terminals (esp. Kitty) do not include Shift in the character.
	// Apply it here to get consistency across terminals.  Only do ASCII
	// letters, for other characters it depends on the keyboard layout.
	إذا ((*modifiers & MOD_نقاب_کھسکاؤ) && c >= 'a' && c <= 'z')
	{
	    c += 'a' - 'A';
	    *modifiers &= ~MOD_نقاب_کھسکاؤ;
	}
	c += 0x80;
	*modifiers &= ~(MOD_نقاب_META | MOD_نقاب_ALT);
    }

    أرجع c;
}

/*
 * Add a single byte to 'showcmd' for a partially matched mapping.
 * Call add_to_showcmd() if a full key has been received.
 */
    ساكن فراغ
جوڑو_byte_منزل_showcmd(char_u byte)
{
    ساكن gotchars_state_T state;
    char_u	*ptr;
    صحيح		modifiers = 0;
    صحيح		c = NUL;

    إذا (!p_sc || پیغام_silent != 0)
	أرجع;

    إذا (!gotchars_جوڑو_byte(&state, byte))
	أرجع;

    state.buf[state.buflen] = NUL;
    state.buflen = 0;

    ptr = state.buf;
    إذا (ptr[0] == K_SPECIAL && ptr[1] == KS_MODIFIER && ptr[2] != NUL)
    {
	modifiers = ptr[2];
	ptr += 3;
    }

    إذا (*ptr != NUL)
    {
	char_u		*mb_ptr = mb_unescape(&ptr);

	c = mb_ptr != عدم ? (*mb_ptr2char)(mb_ptr) : *ptr++;
	إذا (c <= 0x7f)
	{
	    // Merge modifiers into the key to make the result more readable.
	    صحيح		modifiers_after = modifiers;
	    صحيح		mod_c = ملاؤ_modify_Other_کلیدیں(c, &modifiers_after);

	    إذا (modifiers_after == 0)
	    {
		modifiers = 0;
		c = mod_c;
	    }
	}
    }

    // TODO: is there a more readable and yet compact representation of
    // modifiers and special keys?
    إذا (modifiers != 0)
    {
	جوڑو_منزل_showcmd(K_SPECIAL);
	جوڑو_منزل_showcmd(KS_MODIFIER);
	جوڑو_منزل_showcmd(modifiers);
    }
    إذا (c != NUL)
	جوڑو_منزل_showcmd(c);
    طالما (*ptr != NUL)
	جوڑو_منزل_showcmd(*ptr++);
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
    صحيح
vgetc(فراغ)
{
    صحيح		c, c2;
    صحيح		n;
    char_u	buf[MB_MAXBYTES + 1];
    صحيح		i;

#جے_تعریف_شدہ FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    إذا (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#جے_انت

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    إذا (can_get_old_char())
    {
	c = old_char;
	old_char = -1;
	mod_نقاب = old_mod_نقاب;
	mouse_صف = old_mouse_صف;
	mouse_خانہ = old_mouse_خانہ;
    }
    وإلا
    {
	// number of characters recorded from the last vgetc() call
	ساكن size_t	آخری_vgetc_recorded_len = 0;

	mod_نقاب = 0;
	vgetc_mod_نقاب = 0;
	vgetc_char = 0;

	// last_recorded_len can be larger than last_vgetc_recorded_len
	// if peeking records more
	آخری_recorded_len -= آخری_vgetc_recorded_len;

	لكل (;;)		// this is done twice if there are modifiers
	{
	    صحيح did_inc = FALSE;

	    // No mapping after modifier has been read, using an input method
	    // and when a popup window has disabled mapping.
	    إذا (mod_نقاب
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#جے_انت
#إذا defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#جے_انت
		    )
	    {
		++no_mapping;
		++allow_کلیدیں;
		// mod_mask value may change, remember we did the increment
		did_inc = TRUE;
	    }
	    c = vgetorpeek(TRUE);
	    إذا (did_inc)
	    {
		--no_mapping;
		--allow_کلیدیں;
	    }

	    // Get two extra bytes for special keys, handle modifiers.
	    إذا (c == K_SPECIAL
#جے_تعریف_شدہ FEAT_GUI
		    || c == CSI
#جے_انت
	       )
	    {
		صحيح	    save_allow_کلیدیں = allow_کلیدیں;

		++no_mapping;
		allow_کلیدیں = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_کلیدیں = save_allow_کلیدیں;
		إذا (c2 == KS_MODIFIER)
		{
		    mod_نقاب = c;
		    استمر;
		}
		c = منزل_SPECIAL(c2, c);

		إذا (allow_osc_کلید == 0 && c == K_OSC)
		    استمر;

		// K_ESC is used to avoid ambiguity with the single Esc
		// character that might be the start of an escape sequence.
		// Convert it back to a single Esc here.
		إذا (c == K_ESC)
		    c = ESC;

#إذا defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		// Handle K_TEAROFF here, the caller of vgetc() doesn't need to
		// know that a menu was torn off
		إذا (
# ifdef VIMDLL
		    gui.وچ_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    صحيح		j;

		    // get menu path, it ends with a <CR>
		    لكل (j = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[j] = c;
			إذا (j < 199)
			    ++j;
		    }
		    name[j] = NUL;
		    gui_make_tearoff(name);
		    استمر;
		}
#جے_انت
#إذا defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && !defined(USE_GTK4) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		إذا (c == K_F10 && gui.menubar != عدم)
		{
		    gtk_menu_shell_select_پہلا(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    استمر;
		}
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
		// Handle focus event here, so that the caller doesn't need to
		// know about it.  Return K_IGNORE so that we loop once (needed
		// if 'lazyredraw' is set).
		إذا (c == K_FOCUSGAINED || c == K_FOCUSLOST)
		{
		    ui_focus_change(c == K_FOCUSGAINED);
		    c = K_IGNORE;
		}

		// Translate K_CSI to CSI.  The special key is only used to
		// avoid it being recognized as the start of a special key.
		إذا (c == K_CSI)
		    c = CSI;
#جے_انت
#جے_تعریف_شدہ FEAT_EVAL
		إذا (c == K_SID)
		{
		    صحيح	    j;

		    // Handle <SID>{sid};  Do up to 20 digits for safety.
		    آخری_ورتیا_sid = 0;
		    لكل (j = 0; j < 20 && محفوظ_isdigit(c = vgetorpeek(TRUE)); ++j)
			آخری_ورتیا_sid = آخری_ورتیا_sid * 10 + (c - '0');
		    آخری_ورتیا_map = عدم;
		    استمر;
		}
#جے_انت
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    إذا (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		لكل (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    إذا (buf[i] == K_SPECIAL
#جے_تعریف_شدہ FEAT_GUI
			    || (buf[i] == CSI)
#جے_انت
			    )
		    {
			// Must be a K_SPECIAL - KS_SPECIAL - KE_FILLER
			// sequence, which represents a K_SPECIAL (0x80),
			// or a CSI - KS_EXTRA - KE_CSI sequence, which
			// represents a CSI (0x9B),
			// or a K_SPECIAL - KS_EXTRA - KE_CSI, which is CSI
			// too.
			c = vgetorpeek(TRUE);
			إذا (vgetorpeek(TRUE) == KE_CSI && c == KS_EXTRA)
			    buf[i] = CSI;
		    }
		}
		--no_mapping;
		c = (*mb_ptr2char)(buf);
	    }

	    إذا (vgetc_char == 0)
	    {
		vgetc_mod_نقاب = mod_نقاب;
		vgetc_char = c;
	    }

	    // A keypad or special function key was not mapped, use it like
	    // its ASCII equivalent.
	    بدل (c)
	    {
		حالة K_KPLUS:	c = '+'; اكسر;
		حالة K_KMINUS:	c = '-'; اكسر;
		حالة K_KDIVIDE:	c = '/'; اكسر;
		حالة K_KMULTIPLY: c = '*'; اكسر;
		حالة K_KENTER:	c = CAR; اكسر;
		حالة K_KPOINT:
#جے_تعریف_شدہ MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = Map_Virtual_کلید(VK_DECIMAL, 2); اكسر;
#وإلا
				c = '.'; اكسر;
#جے_انت
		حالة K_K0:	c = '0'; اكسر;
		حالة K_K1:	c = '1'; اكسر;
		حالة K_K2:	c = '2'; اكسر;
		حالة K_K3:	c = '3'; اكسر;
		حالة K_K4:	c = '4'; اكسر;
		حالة K_K5:	c = '5'; اكسر;
		حالة K_K6:	c = '6'; اكسر;
		حالة K_K7:	c = '7'; اكسر;
		حالة K_K8:	c = '8'; اكسر;
		حالة K_K9:	c = '9'; اكسر;

		حالة K_XHOME:
		حالة K_ZHOME:	إذا (mod_نقاب == MOD_نقاب_کھسکاؤ)
				{
				    c = K_S_HOME;
				    mod_نقاب = 0;
				}
				وإلا إذا (mod_نقاب == MOD_نقاب_CTRL)
				{
				    c = K_C_HOME;
				    mod_نقاب = 0;
				}
				وإلا
				    c = K_HOME;
				اكسر;
		حالة K_XEND:
		حالة K_ZEND:	إذا (mod_نقاب == MOD_نقاب_کھسکاؤ)
				{
				    c = K_S_سرا;
				    mod_نقاب = 0;
				}
				وإلا إذا (mod_نقاب == MOD_نقاب_CTRL)
				{
				    c = K_C_سرا;
				    mod_نقاب = 0;
				}
				وإلا
				    c = K_سرا;
				اكسر;

		حالة K_XUP:	c = K_اتے; اكسر;
		حالة K_XDOWN:	c = K_تھلے; اكسر;
		حالة K_XLEFT:	c = K_بایاں; اكسر;
		حالة K_XRIGHT:	c = K_دایاں; اكسر;
	    }

	    اكسر;
	}

	آخری_vgetc_recorded_len = آخری_recorded_len;
    }

#جے_تعریف_شدہ FEAT_EVAL
    /*
     * In the main loop "may_garbage_collect" can be set to do garbage
     * collection in the first next vgetc().  It's disabled after that to
     * avoid internally used Lists and Dicts to be freed.
     */
    may_garbage_collect = FALSE;
#جے_انت

#جے_تعریف_شدہ FEAT_BEVAL_رکن
    إذا (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD
	    && c != K_COMPLETE_DELAY)
    {
	// Don't trigger 'balloonexpr' unless only the mouse was moved.
	bevalexpr_due_سیٹ = FALSE;
	ui_remove_balloon();
    }
#جے_انت
#جے_تعریف_شدہ FEAT_PROP_POPUP
    // Only filter keys that do not come from ":normal".  Keys from feedkeys()
    // are filtered.
    إذا ((!ex_عام_busy || وچ_feedkeys) && popup_do_filter(c))
    {
	إذا (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#جے_انت

#جے_تعریف_شدہ FEAT_EVAL
    c = do_کلید_داخلہ_pre(c);

    // Clear the next typedchars_pos
    typedchars_pos = 0;
#جے_انت

    // Need to process the character before we know it's safe to do something
    // else.
    إذا (c != K_IGNORE)
	state_no_longer_محفوظ("key typed");

    أرجع c;
}

#جے_تعریف_شدہ FEAT_EVAL
/*
 * Handle the KeyInputPre autocommand.
 * "c" is the character that was typed.
 * Return new input character.
 */
    ساكن صحيح
do_کلید_داخلہ_pre(صحيح c)
{
    صحيح		res = c;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    char_u	curr_منوال[منوال_MAX_لمائی];
    صحيح		save_State = State;
    save_v_event_T save_v_event;
    dict_T	*v_event;

    // Return quickly when there is nothing to do.
    إذا (!has_keyinputpre())
	أرجع res;

    إذا (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = KEY2TERMCAP0(c);
	buf[2] = KEY2TERMCAP1(c);
	buflen = 3;
    }
    وإلا
	buflen = (*mb_char2bytes)(c, buf);
    buf[buflen] = NUL;

    typedchars[typedchars_pos] = NUL;
    vim_unescape_csi(typedchars);

    get_منوال(curr_منوال);

    // Lock the text to avoid weird things from happening.
    ++textlock;
    سیٹ_vim_var_string(VV_CHAR, buf, (صحيح)buflen);  // set v:char

    v_event = get_v_event(&save_v_event);
    (فراغ)dict_جوڑو_bool(v_event, "typed", کلید_Typed);
    (فراغ)dict_جوڑو_string_len(v_event, "typedchar", typedchars, typedchars_pos);

    إذا (apply_autocmds(EVENT_KEYINPUTPRE, curr_منوال, curr_منوال, FALSE, curbuf))
    {
	string_T    v_char;

	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character.
	v_char.string = get_vim_var_str(VV_CHAR);
	إذا (STRCMP(buf, v_char.string) != 0)
	{
	    v_char.لمائی = STRLEN(v_char.string);
	    // Convert special bytes when it is special string.
	    إذا (v_char.لمائی >= 3 && v_char.string[0] == K_SPECIAL)
		res = TERMCAP2_کلید(v_char.string[1], v_char.string[2]);
	    وإلا إذا (v_char.لمائی > 0)
		res = PTR2CHAR(v_char.string);
	}
    }

    restore_v_event(v_event, &save_v_event);

    سیٹ_vim_var_string(VV_CHAR, عدم, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    أرجع res;
}
#جے_انت

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    صحيح
محفوظ_vgetc(فراغ)
{
    صحيح	c;

    c = vgetc();
    إذا (c == NUL)
	c = get_keystroke();
    أرجع c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 * Does not handle bracketed paste - do not use the result for commands.
 */
    ساكن صحيح
plain_vgetc_nopaste(فراغ)
{
    صحيح c;

    افعل
	c = محفوظ_vgetc();
    طالما (c == K_IGNORE
	    || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
	    || c == K_MOUSEMOVE);
    أرجع c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    صحيح
plain_vgetc(فراغ)
{
    صحيح c = plain_vgetc_nopaste();

    إذا (c == K_PS)
	// Only handle the first pasted character.  Drop the rest, since we
	// don't know what to do with it.
	c = bracketed_paste(PASTE_ONE_CHAR, FALSE, عدم);

    أرجع c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 * Returns NUL if no character is available.
 */
    صحيح
vpeekc(فراغ)
{
    إذا (can_get_old_char())
	أرجع old_char;
    أرجع vgetorpeek(FALSE);
}

#إذا defined(FEAT_TERMRESPONSE) || defined(FEAT_TERMINAL)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    صحيح
vpeekc_nomap(فراغ)
{
    صحيح		c;

    ++no_mapping;
    ++allow_کلیدیں;
    c = vpeekc();
    --no_mapping;
    --allow_کلیدیں;
    أرجع c;
}
#جے_انت

/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    صحيح
vpeekc_any(فراغ)
{
    صحيح		c;

    c = vpeekc();
    إذا (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    أرجع c;
}

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    صحيح
char_avail(فراغ)
{
    صحيح	    retval;

#جے_تعریف_شدہ FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    إذا (disable_char_avail_for_testing)
	أرجع FALSE;
#جے_انت
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    أرجع (retval != NUL);
}

#إذا defined(FEAT_EVAL)
/*
 * "getchar()" and "getcharstr()" functions
 */
    ساكن فراغ
getchar_common(typval_T *argvars, typval_T *rettv, صحيح allow_نمبر)
{
    varnumber_T		n = 0;
    صحيح			called_emsg_شروع = called_emsg;
    صحيح			error = FALSE;
    صحيح			simplify = TRUE;
    char_u		cursor_flag = 'm';

    إذا ((وچ_vim9script()
		&& check_for_opt_bool_or_نمبر_arg(argvars, 0) == FAIL)
	    || (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type == VAR_DICT)
    {
	dict_T		*d = argvars[1].vval.v_dict;
	char_u		*cursor_str;

	إذا (allow_نمبر)
	    allow_نمبر = dict_get_bool(d, "number", TRUE);
	وإلا إذا (dict_has_کلید(d, "number"))
	    semsg(_(e_invalid_دلیل_str), "number");

	simplify = dict_get_bool(d, "simplify", TRUE);

	cursor_str = dict_get_string(d, "cursor", FALSE);
	إذا (cursor_str != عدم)
	{
	    إذا (STRCMP(cursor_str, "hide") != 0
		    && STRCMP(cursor_str, "keep") != 0
		    && STRCMP(cursor_str, "msg") != 0)
		semsg(_(e_invalid_قدر_for_دلیل_str_str), "cursor",
								   cursor_str);
	    وإلا
		cursor_flag = cursor_str[0];
	}
    }

    إذا (called_emsg != called_emsg_شروع)
	أرجع;

# ifdef سنیہا_قطار
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
# endif

    إذا (cursor_flag == 'h')
	cursor_sleep();
    وإلا إذا (cursor_flag == 'm')
	windgoto(پیغام_صف, cmdline_خانہ_off + پیغام_خانہ);

    ++no_mapping;
    ++allow_کلیدیں;
    ++allow_osc_کلید;
    إذا (!simplify)
	++no_reduce_کلیدیں;
    لكل (;;)
    {
	إذا (argvars[0].v_type == VAR_UNKNOWN
		|| (argvars[0].v_type == VAR_نمبر
			&& argvars[0].vval.v_نمبر == -1))
	    // getchar(): blocking wait.
	    n = plain_vgetc_nopaste();
	وإلا إذا (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	وإلا إذا (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	وإلا
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = محفوظ_vgetc();

	إذا (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    استمر;
	اكسر;
    }
    --no_mapping;
    --allow_کلیدیں;
    --allow_osc_کلید;
    إذا (!simplify)
	--no_reduce_کلیدیں;

    إذا (cursor_flag == 'h')
	cursor_unsleep();

    سیٹ_vim_var_nr(VV_MOUSE_WIN, 0);
    سیٹ_vim_var_nr(VV_MOUSE_WINID, 0);
    سیٹ_vim_var_nr(VV_MOUSE_LNUM, 0);
    سیٹ_vim_var_nr(VV_MOUSE_خانہ, 0);

    إذا (n != 0 && (!allow_نمبر || IS_SPECIAL(n) || mod_نقاب != 0))
    {
	char_u		عارضی[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	صحيح		i = 0;

	// Turn a special key into three bytes, plus modifier.
	إذا (mod_نقاب != 0)
	{
	    عارضی[i++] = K_SPECIAL;
	    عارضی[i++] = KS_MODIFIER;
	    عارضی[i++] = mod_نقاب;
	}
	إذا (IS_SPECIAL(n))
	{
	    عارضی[i++] = K_SPECIAL;
	    عارضی[i++] = K_دوجا(n);
	    عارضی[i++] = K_تیجا(n);
	}
	وإلا إذا (has_mbyte)
	    i += (*mb_char2bytes)(n, عارضی + i);
	وإلا
	    عارضی[i++] = n;
	عارضی[i] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strnsave(عارضی, i);

	إذا (is_mouse_کلید(n))
	{
	    صحيح		صف = mouse_صف;
	    صحيح		خانہ = mouse_خانہ;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    صحيح		winnr = 1;

	    إذا (صف >= 0 && خانہ >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_لبھو_win(&صف, &خانہ, لبھو_POPUP);
		إذا (win == عدم)
		    أرجع;
		(فراغ)mouse_comp_pos(win, &صف, &خانہ, &lnum, عدم);
# ifdef FEAT_PROP_POPUP
		إذا (WIN_IS_POPUP(win))
		    winnr = 0;
		وإلا
# endif
		    لكل (wp = firstwin; wp != win && wp != عدم;
							       wp = wp->w_اگلا)
			++winnr;
		سیٹ_vim_var_nr(VV_MOUSE_WIN, winnr);
		سیٹ_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		سیٹ_vim_var_nr(VV_MOUSE_LNUM, lnum);
		سیٹ_vim_var_nr(VV_MOUSE_خانہ, خانہ + 1);
	    }
	}
    }
    وإلا إذا (!allow_نمبر)
	rettv->v_type = VAR_STRING;
    وإلا
	rettv->vval.v_نمبر = n;
}

/*
 * "getchar()" function
 */
    فراغ
f_getchar(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, TRUE);
}

/*
 * "getcharstr()" function
 */
    فراغ
f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv, FALSE);
}

/*
 * "getcharmod()" function
 */
    فراغ
f_getcharmod(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    rettv->vval.v_نمبر = mod_نقاب;
}
#جے_انت // FEAT_EVAL

#إذا defined(سنیہا_قطار)
# define MAX_REPEAT_PARSE 8

/*
 * Process messages that have been queued for netbeans or clientserver.
 * Also check if any jobs have ended.
 * These functions can call arbitrary Vim script and should only be called when
 * it is safe to do so.
 */
    فراغ
parse_queued_messages(فراغ)
{
    صحيح	    old_curwin_id;
    صحيح	    old_curbuf_fnum;
    صحيح	    i;
    صحيح	    save_may_garbage_collect = may_garbage_collect;
    ساكن صحيح entered = 0;
    صحيح	    was_محفوظ = get_was_محفوظ_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    إذا (updating_screen || dont_parse_messages)
	أرجع;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    إذا (curbuf == عدم || curwin == عدم)
	أرجع;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    لكل (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# إذا (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_کم_CHANNEL)
	channel_handle_events(FALSE);
# endif

# ifdef FEAT_NETBEANS_INTG
	// Process the queued netbeans messages.
	netbeans_parse_messages();
# endif
# ifdef FEAT_SOCKETSERVER
	socketserver_parse_messages();
# endif
# ifdef FEAT_کم_CHANNEL
	// Write any buffer lines still to be written.
	channel_write_any_سطریں();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# إذا defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_کم_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	إذا (کم_check_ended())
	    استمر;
# endif
# ifdef FEAT_TERMINAL
	free_بن_ورتیا_terminals();
# endif

# ifdef FEAT_SOUND_MACOSX
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	إذا (has_sound_callback_وچ_قطار())
	    invoke_sound_callback();
# endif
# ifdef SIGUSR1
	إذا (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, عدم, عدم, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
# endif
	اكسر;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    إذا (entered == 1 && was_محفوظ)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    إذا (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}
#جے_انت


تعريفنوع تعداد {
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
    ساكن صحيح
at_ins_compl_کلید(فراغ)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    صحيح	    c = *p;

    إذا (typebuf.tb_len > 3
	    && (c == K_SPECIAL || c == CSI)  // CSI is used by the GUI
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_نقاب_CTRL)
	    // CTRL-SHIFT-N/P scroll the info popup, so they must not be folded
	    // to the CTRL-N/CTRL-P completion keys here.
	    && !(p[2] & MOD_نقاب_کھسکاؤ))
	c = p[3] & 0x1f;
    أرجع !ctrl_x_منوال_eval()
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
    ساكن صحيح
check_simplify_modifier(صحيح max_وتھ)
{
    صحيح		وتھ;
    char_u	*tp;

    لكل (وتھ = 0; وتھ < max_وتھ; ++وتھ)
    {
	إذا (وتھ + 3 >= typebuf.tb_len)
	    اكسر;
	tp = typebuf.tb_buf + typebuf.tb_off + وتھ;
	إذا ((tp[0] == K_SPECIAL || tp[0] == CSI) && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    صحيح modifier = tp[2];
	    صحيح	c = tp[3];
	    صحيح new_c = ملاؤ_modify_Other_کلیدیں(c, &modifier);

	    إذا (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		صحيح	len;
		صحيح	کلید_وتھ = وتھ;

		إذا (وتھ == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_نقاب = tp[2];
		}
		إذا (IS_SPECIAL(new_c))
		{
		    new_string[0] = K_SPECIAL;
		    new_string[1] = K_دوجا(new_c);
		    new_string[2] = K_تیجا(new_c);
		    len = 3;
		}
		وإلا
		    len = mb_char2bytes(new_c, new_string);
		إذا (modifier == 0)
		{
		    إذا (put_string_وچ_typebuf(وتھ, 4, new_string, len,
							عدم, 0, عدم) == FAIL)
			أرجع -1;
		}
		وإلا
		{
		    tp[2] = modifier;
		    کلید_وتھ = وتھ + 3;
		    إذا (put_string_وچ_typebuf(کلید_وتھ, 1, new_string, len,
							عدم, 0, عدم) == FAIL)
			أرجع -1;
		}

		// A mapping for the simplified key can be used for it even
		// when the key protocol is enabled.
		لكل (صحيح i = 0; i < len; ++i)
		    typebuf.tb_noremap[typebuf.tb_off + کلید_وتھ + i]
							    |= RM_SIMPLIFIED;
		أرجع len;
	    }
	}
    }
    أرجع 0;
}

/*
 * Return TRUE if the terminal sends modifiers with various keys.  This is when
 * modifyOtherKeys level 2 is enabled or the kitty keyboard protocol is
 * enabled.
 */
    صحيح
کلید_protocol_enabled(فراغ)
{
    // If xterm has responded to XTQMODKEYS it overrules seenModifyOtherKeys.
    صحيح using_mok = modify_otherkeys_state != MOKS_INITIAL
			? modify_otherkeys_state == MOKS_ENABLED
			: تکیا_Modify_Other_کلیدیں;
    أرجع using_mok || kitty_protocol_state == KKPS_ENABLED;
}

/*
 * Handle mappings in the typeahead buffer.
 * - When something was mapped, return map_result_retry for recursive mappings.
 * - When nothing mapped and typeahead has a character: return map_result_get.
 * - When there is no match yet, return map_result_nomatch, need to get more
 *   typeahead.
 * - On failure (out of memory) return map_result_fail.
 */
    ساكن صحيح
handle_mapping(
	    صحيح *keylenp,
	    صحيح *timedout,
	    صحيح *mapdepth)
{
    mapblock_T	*mp = عدم;
    mapblock_T	*mp2;
    mapblock_T	*mp_میل;
    صحيح		mp_میل_len = 0;
    صحيح		max_mlen = 0;
    صحيح		want_termcode = 0;  // 1 if termcode expected after max_mlen
    صحيح		tb_c1;
    صحيح		mlen;
#جے_تعریف_شدہ FEAT_LANGMAP
    صحيح		nolmaplen;
#جے_انت
    صحيح		keylen = *keylenp;
    صحيح		i;
    صحيح		local_State = get_real_state();
    صحيح		is_plug_map = FALSE;
    bool	وچ_osc = وچ_osc_sequence(); // If we are in an OSC sequence,
					    // then go straight to
					    // check_termcode() in order to
					    // consume chars.

    // If typeahead starts with <Plug> then remap, even for a "noremap" mapping.
    إذا (!وچ_osc && typebuf.tb_len >= 3
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
    إذا (!وچ_osc && no_mapping == 0 && is_maphash_valid()
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
#جے_تعریف_شدہ FEAT_GUI
	إذا (gui.وچ_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#جے_انت
#جے_تعریف_شدہ FEAT_LANGMAP
	إذا (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	وإلا
	{
	    LANGMAP_ADJUST(tb_c1, (State & (منوال_CMDLINE | منوال_داخل_کرو)) == 0
					   && get_real_state() != منوال_SELECT);
	    nolmaplen = 0;
	}
#جے_انت
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	إذا (mp == عدم)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = عدم;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_میل = عدم;
	mp_میل_len = 0;
	لكل ( ; mp != عدم;
	       mp->m_اگلا == عدم ? (mp = mp2, mp2 = عدم) : (mp = mp->m_اگلا))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    إذا (mp->m_کلیدیں[0] == tb_c1
		    && (mp->m_منوال & local_State)
		    && !(mp->m_simplified && کلید_protocol_enabled()
			    && typebuf.tb_maplen == 0
			    && (typebuf.tb_noremap[typebuf.tb_off]
							& RM_SIMPLIFIED) == 0)
		    && ((mp->m_منوال & منوال_LANGMAP) == 0
						    || typebuf.tb_maplen == 0))
	    {
#جے_تعریف_شدہ FEAT_LANGMAP
		صحيح	nomap = nolmaplen;
		صحيح	modifiers = 0;
#جے_انت
		// find the match length of this mapping
		لكل (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
		    صحيح	c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
#جے_تعریف_شدہ FEAT_LANGMAP
		    إذا (nomap > 0)
		    {
			إذا (nomap == 2 && c2 == KS_MODIFIER)
			    modifiers = 1;
			وإلا إذا (nomap == 1 && modifiers == 1)
			    modifiers = c2;
			--nomap;
		    }
		    وإلا
		    {
			إذا (c2 == K_SPECIAL)
			    nomap = 2;
			وإلا إذا (ملاؤ_modify_Other_کلیدیں(c2, &modifiers) == c2)
			    // Only apply 'langmap' if merging modifiers into
			    // the key will not result in another character,
			    // so that 'langmap' behaves consistently in
			    // different terminals and GUIs.
			    LANGMAP_ADJUST(c2, TRUE);
			modifiers = 0;
		    }
#جے_انت
		    إذا (mp->m_کلیدیں[mlen] != c2)
			اكسر;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_کلیدیں;
		    char_u *p2 = mb_unescape(&p1);

		    إذا (has_mbyte && p2 != عدم
					&& MB_BYTE2LEN(tb_c1) > mb_ptr2len(p2))
			mlen = 0;
		}

		// Check an entry whether it matches.
		// - Full match: mlen == keylen
		// - Partly match: mlen == typebuf.tb_len
		keylen = mp->m_keylen;
		إذا (mlen == keylen || (mlen == typebuf.tb_len
						   && typebuf.tb_len < keylen))
		{
		    char_u  *s;
		    صحيح	    n;

		    // If only script-local mappings are allowed, check if the
		    // mapping starts with K_SNR.
		    s = typebuf.tb_noremap + typebuf.tb_off;
		    إذا ((*s & ~RM_SIMPLIFIED) == RM_SCRIPT
			    && (mp->m_کلیدیں[0] != K_SPECIAL
				|| mp->m_کلیدیں[1] != KS_EXTRA
				|| mp->m_کلیدیں[2] != KE_SNR))
			استمر;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    لكل (n = mlen; --n >= 0; )
			إذا (*s++ & (RM_NONE|RM_ABBR))
			    اكسر;
		    إذا (!is_plug_map && n >= 0)
			استمر;

		    إذا (keylen > typebuf.tb_len)
		    {
			إذا (!*timedout && !(mp_میل != عدم
							&& mp_میل->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    اكسر;
			}
		    }
		    وإلا إذا (keylen > mp_میل_len)
		    {
			// found a longer match
			mp_میل = mp;
			mp_میل_len = keylen;
		    }
		}
		وإلا
		{
		    // No match; may have to check for termcode at next
		    // character.

		    // If the first character that didn't match is
		    // K_SPECIAL then check for a termcode.  This isn't perfect
		    // but should work in most cases.
		    إذا (max_mlen < mlen)
		    {
			max_mlen = mlen;
			want_termcode = mp->m_کلیدیں[mlen] == K_SPECIAL;
		    }
		    وإلا إذا (max_mlen == mlen && mp->m_کلیدیں[mlen] == K_SPECIAL)
			want_termcode = 1;

		    // Check termcode for uppercase character to properly
		    // process "ESC[27;2;<ascii code>~" control sequences.
		    إذا (ASCII_ISUPPER(mp->m_کلیدیں[mlen]))
			want_termcode = 1;
		}
	    }
	}

	// If no partly match found, use the longest full match.
	إذا (keylen != KEYLEN_PART_MAP && mp_میل != عدم)
	{
	    mp = mp_میل;
	    keylen = mp_میل_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    إذا (!وچ_osc && *p_pt != NUL && mp == عدم &&
	    (State & (منوال_داخل_کرو | منوال_عام)))
    {
	لكل (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    إذا (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    اكسر;
	إذا (p_pt[mlen] == NUL)	//میل
	{
	    // write chars to script file(s)
	    إذا (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    سیٹ_option_قدر_give_err((char_u *)"paste",
						      (لما)!p_paste, عدم, 0);
	    إذا (!(State & منوال_داخل_کرو))
	    {
		پیغام_خانہ = 0;
		پیغام_صف = صفاں - 1;
		پیغام_clr_eos();		// clear ruler
	    }
	    حالت_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    أرجع map_نتیجہ_retry;
	}
	// Need more chars for partly match.
	إذا (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_کلید;
	وإلا إذا (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    // May check for a terminal code when there is no mapping or only a partial
    // mapping.  Also check if there is a full mapping with <Esc>, unless timed
    // out, since that is nearly always a partial match with a terminal code.
    إذا (وچ_osc || ((mp == عدم || max_mlen + want_termcode > mp_میل_len
		    || (mp_میل_len == 1 && *mp->m_کلیدیں == ESC && !*timedout))
	    && keylen != KEYLEN_PART_MAP))
    {
	صحيح	save_keylen = keylen;

	/*
	 * When no matching mapping found or found a non-matching mapping that
	 * matches at least what the matching mapping matched:
	 * Check if we have a terminal code, when:
	 * - mapping is allowed,
	 * - keys have not been mapped,
	 * - and not an ESC sequence, not in insert mode or p_ek is on,
	 * - and when not timed out,
	 */
	إذا (وچ_osc || no_mapping == 0 || allow_کلیدیں != 0)
	{
	    إذا (وچ_osc || ((typebuf.tb_maplen == 0
			    || (p_remap && (typebuf.tb_noremap[typebuf.tb_off]
						& ~RM_SIMPLIFIED) == RM_YES))
		    && !*timedout))
		keylen = check_termcode(max_mlen + 1, عدم, 0, عدم);
	    وإلا
		keylen = 0;

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    إذا (keylen == 0 && save_keylen == KEYLEN_PART_کلید && !*timedout)
		keylen = KEYLEN_PART_کلید;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
#جے_تعریف_شدہ FEAT_TERMINAL
	    check_no_reduce_کلیدیں();  // may update the no_reduce_keys flag
#جے_انت
	    إذا (keylen == 0 && !no_reduce_کلیدیں)
	    {
		keylen = check_simplify_modifier(max_mlen + 1);
		إذا (keylen < 0)
		    // ins_typebuf() failed
		    أرجع map_نتیجہ_fail;
	    }

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    إذا (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	وإلا
	    keylen = 0;
	إذا (keylen == 0)	// no matching terminal code
	{
#جے_تعریف_شدہ AMIGA
	    // check for window bounds report
	    إذا (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		لكل (s = typebuf.tb_buf + typebuf.tb_off + 1;
			   s < typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len
		   && (VIM_ISDIGIT(*s) || *s == ';' || *s == ' ');
			++s)
		    ;
		إذا (*s == 'r' || *s == '|') // found one
		{
		    del_typebuf(
			  (صحيح)(s + 1 - (typebuf.tb_buf + typebuf.tb_off)), 0);
		    // get size and redraw screen
		    shell_resized();
		    *keylenp = keylen;
		    أرجع map_نتیجہ_retry;
		}
		إذا (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_کلید;
	    }
	    إذا (keylen >= 0)
#جے_انت
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		إذا (mp == عدم)
		{
		    *keylenp = keylen;
		    أرجع map_نتیجہ_get;    // get character from typeahead
		}
	}

	إذا (keylen > 0)	    // full matching terminal code
	{
#إذا defined(FEAT_GUI) && defined(FEAT_MENU)
	    إذا (typebuf.tb_len >= 2
		    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
			      && typebuf.tb_buf[typebuf.tb_off + 1] == KS_MENU)
	    {
		صحيح	idx;

		// Using a menu may cause a break in undo!  It's like using
		// gotchars(), but without recording or writing to a script
		// file.
		may_sync_undo();
		del_typebuf(3, 0);
		idx = get_menu_اشاریہ(current_menu, local_State);
		إذا (idx != MENU_اشاریہ_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    إذا (VIsual_سرگرم && VIsual_select
					&& (current_menu->modes & منوال_VISUAL))
		    {
			VIsual_select = FALSE;
			(فراغ)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->لڑیاں[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#جے_انت // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    أرجع map_نتیجہ_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	إذا (mp == عدم || keylen < 0)
	    keylen = KEYLEN_PART_کلید;
	وإلا
	    keylen = mp_میل_len;
    }

    /*
     * complete match
     */
    إذا (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#جے_تعریف_شدہ FEAT_EVAL
	صحيح	save_m_اظہاریہ;
	صحيح	save_m_noremap;
	صحيح	save_m_silent;
	char_u	*save_m_کلیدیں;
	char_u	*save_alt_m_کلیدیں;
	صحيح	save_alt_m_keylen;
#وإلا
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#جے_انت

	// write chars to script file(s)
	إذا (keylen > typebuf.tb_maplen)
	    gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						   keylen - typebuf.tb_maplen);

	cmd_silent = (typebuf.tb_silent > 0);
	del_typebuf(keylen, 0);	// remove the mapped keys

	/*
	 * Put the replacement string in front of mapstr.
	 * The depth check catches ":map x y" and ":map y x".
	 */
	إذا (++*mapdepth >= p_mmd)
	{
	    emsg(_(e_recursive_mapping));
	    إذا (State & منوال_CMDLINE)
		redrawcmdline();
	    وإلا
		setcursor();
	    flush_buffers(FLUSH_اقل);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    أرجع map_نتیجہ_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	إذا (VIsual_سرگرم && VIsual_select && (mp->m_منوال & منوال_VISUAL))
	{
	    VIsual_select = FALSE;
	    (فراغ)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#جے_تعریف_شدہ FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_اظہاریہ = mp->m_اظہاریہ;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_کلیدیں = عدم;  // only saved when needed
	save_alt_m_کلیدیں = عدم;  // only saved when needed
	save_alt_m_keylen = mp->m_alt != عدم ? mp->m_alt->m_keylen : 0;

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	إذا (mp->m_اظہاریہ)
	{
	    صحيح save_vgetc_busy = vgetc_busy;
	    صحيح save_may_garbage_collect = may_garbage_collect;
	    صحيح was_screen_خانہ = screen_cur_خانہ;
	    صحيح was_screen_صف = screen_cur_صف;
	    صحيح prev_did_emsg = did_emsg;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_کلیدیں = vim_strnsave(mp->m_کلیدیں, (size_t)mp->m_keylen);
	    save_alt_m_کلیدیں = mp->m_alt != عدم
				    ? vim_strnsave(mp->m_alt->m_کلیدیں,
					     (size_t)save_alt_m_keylen) : عدم;
	    map_str = eval_map_اظہاریہ(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_صف, was_screen_خانہ);
	    باہرلا_flush();

	    // If an error was displayed and the expression returns an empty
	    // string, generate a <Nop> to allow for a redraw.
	    إذا (prev_did_emsg != did_emsg
				       && (map_str == عدم || *map_str == NUL))
	    {
		char_u	buf[4];

		vim_free(map_str);
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = KE_IGNORE;
		buf[3] = NUL;
		map_str = vim_strnsave(buf, 3);
		إذا (State & منوال_CMDLINE)
		{
		    // redraw the command below the error
		    پیغام_didout = TRUE;
		    إذا (پیغام_صف < cmdline_صف)
			پیغام_صف = cmdline_صف;
		    redrawcmd();
		}
	    }

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	وإلا
#جے_انت
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	إذا (map_str == عدم)
	    i = FAIL;
	وإلا
	{
	    صحيح noremap;

#جے_تعریف_شدہ FEAT_EVAL
	    آخری_ورتیا_map = mp;
	    آخری_ورتیا_sid = -1;
#جے_انت
	    إذا (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    وإلا إذا (
#جے_تعریف_شدہ FEAT_EVAL
		save_m_اظہاریہ ?
		(save_m_کلیدیں != عدم
			&& STRNCMP(map_str, save_m_کلیدیں, (size_t)keylen) == 0)
		|| (save_alt_m_کلیدیں != عدم
			&& STRNCMP(map_str, save_alt_m_کلیدیں,
					    (size_t)save_alt_m_keylen) == 0) :
#جے_انت
		STRNCMP(map_str, mp->m_کلیدیں, (size_t)keylen) == 0
		|| (mp->m_alt != عدم
			&& STRNCMP(map_str, mp->m_alt->m_کلیدیں,
					    (size_t)mp->m_alt->m_keylen) == 0))
		noremap = REMAP_SKIP;
	    وإلا
		noremap = REMAP_YES;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#جے_تعریف_شدہ FEAT_EVAL
	    إذا (save_m_اظہاریہ)
		vim_free(map_str);
#جے_انت
	}
#جے_تعریف_شدہ FEAT_EVAL
	vim_free(save_m_کلیدیں);
	vim_free(save_alt_m_کلیدیں);
#جے_انت
	*keylenp = keylen;
	إذا (i == FAIL)
	    أرجع map_نتیجہ_fail;
	أرجع map_نتیجہ_retry;
    }

    *keylenp = keylen;
    أرجع map_نتیجہ_nomatch;
}

/*
 * unget one character (can only be done once!)
 * If the character was stuffed, vgetc() will get it next time it is called.
 * Otherwise vgetc() will only get it when the stuff buffer is empty.
 */
    فراغ
vungetc(صحيح c)
{
    old_char = c;
    old_mod_نقاب = mod_نقاب;
    old_mouse_صف = mouse_صف;
    old_mouse_خانہ = mouse_خانہ;
    old_کلید_Stuffed = کلید_Stuffed;
}

/*
 * When peeking and not getting a character, reg_executing cannot be cleared
 * yet, so set a flag to clear it later.
 */
    ساكن فراغ
check_سرا_reg_executing(صحيح advance)
{
    إذا (reg_executing != 0 && (typebuf.tb_maplen == 0
						|| pending_سرا_reg_executing))
    {
	إذا (advance)
	{
	    reg_executing = 0;
	    pending_سرا_reg_executing = FALSE;
	}
	وإلا
	    pending_سرا_reg_executing = TRUE;
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
    ساكن صحيح
vgetorpeek(صحيح advance)
{
    صحيح		c;
    صحيح		timedout = FALSE;	// waited for more than 'timeoutlen'
					// for mapping to complete or
					// 'ttimeoutlen' for complete key code
    صحيح		mapdepth = 0;		// check for recursive mapping
    صحيح		منوال_deleted = FALSE;   // set when mode has been deleted
    صحيح		new_wcol, new_wrow;
#جے_تعریف_شدہ FEAT_GUI
    صحيح		shape_بدلیا = FALSE;  // adjusted cursor shape
#جے_انت
    صحيح		n;
    صحيح		old_wcol, old_wrow;
    صحيح		wait_tb_len;

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
    إذا (vgetc_busy > 0 && ex_عام_busy == 0)
	أرجع NUL;

    ++vgetc_busy;

    إذا (advance)
    {
	کلید_Stuffed = FALSE;
	typebuf_was_خالی_تھاواں = FALSE;
    }

    init_typebuf();
    شروع_stuff();
    check_سرا_reg_executing(advance);
    افعل
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	إذا (typeahead_char != 0)
	{
	    c = typeahead_char;
	    إذا (advance)
		typeahead_char = 0;
	}
	وإلا
	    c = read_readbuffers(advance);
	إذا (c != NUL && !got_int)
	{
	    إذا (advance)
	    {
		// KeyTyped = FALSE;  When the command that stuffed something
		// was typed, behave like the stuffed command was typed.
		// needed for CTRL-W CTRL-] to open a fold, for example.
		کلید_Stuffed = TRUE;
	    }
	    إذا (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	// no abbreviations now
	}
	وإلا
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    لكل (;;)
	    {
		لما	wait_ویلا;
		صحيح	keylen = 0;
		صحيح	showcmd_idx;
		check_سرا_reg_executing(advance);
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		إذا (typebuf.tb_maplen)
		    سطر_breakcheck();
		وإلا
		    ui_breakcheck();		// check for CTRL-C
		إذا (got_int)
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
		    إذا ((c || typebuf.tb_maplen)
				     && (State & (منوال_داخل_کرو | منوال_CMDLINE)))
			c = ESC;
		    وإلا
			c = Ctrl_C;
		    flush_buffers(FLUSH_داخلہ);	// flush all typeahead

		    إذا (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    اكسر;
		}
		وإلا إذا (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_نتیجہ_T نتیجہ = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    إذا (نتیجہ == map_نتیجہ_retry)
			// try mapping again
			استمر;
		    إذا (نتیجہ == map_نتیجہ_fail)
		    {
			// failed, use the outer loop
			c = -1;
			اكسر;
		    }
		    إذا (نتیجہ == map_نتیجہ_get)
		    {
/*
 * get a character: 2. from the typeahead buffer
 */
			c = typebuf.tb_buf[typebuf.tb_off];
			إذا (advance)	// remove chars from typebuf
			{
			    cmd_silent = (typebuf.tb_silent > 0);
			    إذا (typebuf.tb_maplen > 0)
				کلید_Typed = FALSE;
			    وإلا
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
			اكسر;  // got character, break the for loop
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
		إذا (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& kitty_protocol_state != KKPS_ENABLED
			&& ex_عام_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & منوال_داخل_کرو)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_کلید && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	خانہ = 0;
		    char_u	*ptr;

		    إذا (منوال_displayed)
		    {
			unshowmode(TRUE);
			منوال_deleted = TRUE;
		    }
#جے_تعریف_شدہ FEAT_GUI
		    // may show a different cursor shape
		    إذا (gui.وچ_use && State != منوال_عام && !cmd_silent)
		    {
			صحيح	    save_State;

			save_State = State;
			State = منوال_عام;
			gui_update_cursor(TRUE, FALSE);
			State = save_State;
			shape_بدلیا = TRUE;
		    }
#جے_انت
		    validate_cursor();
		    old_wcol = curwin->w_wcol;
		    old_wrow = curwin->w_wrow;

		    // move cursor left, if possible
		    إذا (curwin->w_cursor.خانہ != 0)
		    {
			إذا (curwin->w_wcol > 0)
			{
			    // After auto-indenting and no text is following,
			    // we are expecting to truncate the trailing
			    // white-space, so find the last non-white
			    // character -- webb
			    إذا (did_ai && *skipwhite(ml_get_curline()
						+ curwin->w_cursor.خانہ) == NUL)
			    {
				chartabsize_T cts;

				curwin->w_wcol = 0;
				ptr = ml_get_curline();
				init_chartabsize_arg(&cts, curwin,
					  curwin->w_cursor.lnum, 0, ptr, ptr);
				طالما (cts.cts_ptr < ptr + curwin->w_cursor.خانہ)
				{
				    إذا (!VIM_ISWHITE(*cts.cts_ptr))
					curwin->w_wcol = cts.cts_vcol;
				    cts.cts_vcol += lbr_chartabsize(&cts);
				    إذا (has_mbyte)
					cts.cts_ptr +=
						   (*mb_ptr2len)(cts.cts_ptr);
				    وإلا
					++cts.cts_ptr;
				}
				clear_chartabsize_arg(&cts);

				curwin->w_wrow = curwin->w_cline_صف
					   + curwin->w_wcol / curwin->w_چوڑائی;
				curwin->w_wcol %= curwin->w_چوڑائی;
				curwin->w_wcol += curwin_خانہ_off();
				خانہ = 0;	// no correction needed
			    }
			    وإلا
			    {
				--curwin->w_wcol;
				خانہ = curwin->w_cursor.خانہ - 1;
			    }
			}
			وإلا إذا (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_چوڑائی - 1;
			    خانہ = curwin->w_cursor.خانہ - 1;
			}
			إذا (has_mbyte && خانہ > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    خانہ -= (*mb_سر_off)(ptr, ptr + خانہ);
			    إذا ((*mb_ptr2cells)(ptr + خانہ) > 1)
				--curwin->w_wcol;
			}
		    }
		    setcursor();
		    باہرلا_flush();
		    new_wcol = curwin->w_wcol;
		    new_wrow = curwin->w_wrow;
		    curwin->w_wcol = old_wcol;
		    curwin->w_wrow = old_wrow;
		}
		إذا (c < 0)
		    استمر;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		لكل (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		إذا (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    استمر;
		}

		إذا (ex_عام_busy > 0)
		{
		    ساكن صحيح tc = 0;

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    إذا (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			استمر;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    إذا (p_im && (State & منوال_داخل_کرو))
			c = Ctrl_L;
#جے_تعریف_شدہ FEAT_TERMINAL
		    وإلا إذا (terminal_is_سرگرم())
			c = K_CANCEL;
#جے_انت
		    وإلا إذا ((State & منوال_CMDLINE)
					     || (cmdwin_type > 0 && tc == ESC))
			c = Ctrl_C;
		    وإلا
			c = ESC;
		    tc = c;
		    // set a flag to indicate this wasn't a normal char
		    إذا (advance)
			typebuf_was_خالی_تھاواں = TRUE;

		    // return from main_loop()
		    إذا (pending_exmode_سرگرم)
			exmode_سرگرم = EXMODE_عام;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    اكسر;
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
		إذا (((State & منوال_داخل_کرو) != 0 || p_lz)
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
		صحيح showing_partial = FALSE;
		إذا (typebuf.tb_len > 0 && advance && !exmode_سرگرم)
		{
		    إذا (((State & (منوال_عام | منوال_داخل_کرو))
						      || State == منوال_LANGMAP)
			    && State != منوال_HITRETURN)
		    {
			// this looks nice when typing a dead character map
			إذا (State & منوال_داخل_کرو
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
			إذا (typebuf.tb_len > SHOWCMD_خانے_گنتی)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_خانے_گنتی;
			طالما (showcmd_idx < typebuf.tb_len)
			    جوڑو_byte_منزل_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
		    }

		    // This looks nice when typing a dead character map.
		    // There is no actual command line for get_number().
		    إذا ((State & منوال_CMDLINE)
			    && get_cmdline_info()->cmdbuff != عدم
#إذا defined(FEAT_CRYPT) || defined(FEAT_EVAL)
			    && cmdline_تارا == 0
#جے_انت
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
		إذا (typebuf.tb_len == 0)
		    // timedout may have been set if a mapping with empty RHS
		    // fully matched while longer mappings timed out.
		    timedout = FALSE;

		إذا (advance)
		{
		    إذا (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_کلید)))
			// blocking wait
			wait_ویلا = -1L;
		    وإلا إذا (keylen == KEYLEN_PART_کلید && p_ttm >= 0)
			wait_ویلا = p_ttm;
		    وإلا
			wait_ویلا = p_tm;
		}
		وإلا
		    wait_ویلا = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_ویلا);

		إذا (showcmd_idx != 0)
		    نکالو_showcmd();
		إذا (showing_partial)
		{
		    إذا (State & منوال_داخل_کرو)
			edit_unputchar();
		    إذا ((State & منوال_CMDLINE)
					&& get_cmdline_info()->cmdbuff != عدم)
			unputcmdline();
		    وإلا
			setcursor();	// put cursor back where it belongs
		}

		إذا (c < 0)
		    استمر;		// end of input script reached
		إذا (c == NUL)		// no character available
		{
		    إذا (!advance)
			اكسر;
		    إذا (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			استمر;
		    }
		}
		وإلا
		{	    // allow mapping for just typed characters
		    طالما (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#جے_تعریف_شدہ HAVE_داخلہ_طریقہ
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_سرگرم = im_get_حالت();
#جے_انت
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } طالما ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    إذا (advance && p_smd && پیغام_silent == 0 && (State & منوال_داخل_کرو))
    {
	إذا (c == ESC && !منوال_deleted && !no_mapping && منوال_displayed)
	{
	    إذا (typebuf.tb_len && !کلید_Typed)
		redraw_cmdline = TRUE;	    // delete mode later
	    وإلا
		unshowmode(FALSE);
	}
	وإلا إذا (c != ESC && منوال_deleted)
	{
	    إذا (typebuf.tb_len && !کلید_Typed)
		redraw_cmdline = TRUE;	    // show mode later
	    وإلا
		showmode();
	}
    }
#جے_تعریف_شدہ FEAT_GUI
    // may unshow different cursor shape
    إذا (gui.وچ_use && shape_بدلیا)
	gui_update_cursor(TRUE, FALSE);
#جے_انت
    إذا (timedout && c == ESC)
    {
	// When recording there will be no timeout.  Add an <Ignore> after the
	// ESC to avoid that it forms a key code with following characters.
	gotchars_ignore();
    }

    --vgetc_busy;

    أرجع c;
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
    ساكن صحيح
inchar(
    char_u	*buf,
    صحيح		maxlen,
    لما	wait_ویلا)	    // milliseconds
{
    صحيح		len = 0;	    // init for GCC
    صحيح		retesc = FALSE;	    // return ESC with gotint
    صحيح		script_char;
    صحيح		tb_change_cnt = typebuf.tb_change_cnt;

    إذا (wait_ویلا == -1L || wait_ویلا > 100L)  // flush output before waiting
    {
	cursor_on();
	باہرلا_flush_cursor(FALSE, FALSE);
#إذا defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	إذا (gui.وچ_use && postponed_mouseshape)
	    update_mouseshape(-1);
#جے_انت
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    إذا (State != منوال_HITRETURN)
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
    طالما (scriptin[curscript] != عدم && script_char < 0
#جے_تعریف_شدہ FEAT_EVAL
	    && !ignore_script
#جے_انت
	    )
    {
#جے_تعریف_شدہ سنیہا_قطار
	parse_queued_messages();
#جے_انت

	إذا (got_int || (script_char = getc(scriptin[curscript])) < 0)
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
	    إذا (got_int)
		retesc = TRUE;
	    وإلا
		أرجع -1;
	}
	وإلا
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    إذا (script_char < 0)	// did not get a character from script
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	إذا (got_int)
	{
#تعریف_کرو DUM_LEN (MAXMAPLEN * 3 + 3)
	    char_u	dum[DUM_LEN + 1];

	    لكل (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		إذا (len == 0 || (len == 1 && dum[0] == Ctrl_C))
		    اكسر;
	    }
	    أرجع retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	إذا (wait_ویلا == -1L || wait_ویلا > 10L)
	    باہرلا_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_ویلا, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    إذا (typebuf_بدلیا(tb_change_cnt))
	أرجع 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    إذا (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    أرجع fix_داخلہ_بفر(buf, len);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * "buf[]" must have room to triple the number of bytes!
 * Returns the new length.
 */
    صحيح
fix_داخلہ_بفر(char_u *buf, صحيح len)
{
    صحيح		i;
    char_u	*p = buf;

    /*
     * Two characters are special: NUL and K_SPECIAL.
     * When compiled With the GUI CSI is also special.
     * Replace	     NUL by K_SPECIAL KS_ZERO	 KE_FILLER
     * Replace K_SPECIAL by K_SPECIAL KS_SPECIAL KE_FILLER
     * Replace       CSI by K_SPECIAL KS_EXTRA   KE_CSI
     */
    لكل (i = len; --i >= 0; ++p)
    {
#جے_تعریف_شدہ FEAT_GUI
	// When the GUI is used any character can come after a CSI, don't
	// escape it.
	إذا (gui.وچ_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
# ifndef MSWIN
	// When not on MS-Windows and the GUI is not used CSI needs to be
	// escaped.
	وإلا إذا (!gui.وچ_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (صحيح)KE_CSI;
	    len += 2;
	}
# endif
	وإلا
#جے_انت
	إذا (p[0] == NUL || (p[0] == K_SPECIAL
		    // timeout may generate K_CURSORHOLD,
		    // 'autocompletedelay' timeout K_COMPLETE_DELAY
		    && (i < 2 || p[1] != KS_EXTRA
			|| (p[2] != (صحيح)KE_CURSORHOLD
			    && p[2] != (صحيح)KE_COMPLETE_DELAY))
#إذا defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
		    // Win32 console passes modifiers
		    && (
# ifdef VIMDLL
			gui.وچ_use ||
# endif
			(i < 2 || p[1] != KS_MODIFIER))
#جے_انت
		    ))
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    p[2] = K_تیجا(p[0]);
	    p[1] = K_دوجا(p[0]);
	    p[0] = K_SPECIAL;
	    p += 2;
	    len += 2;
	}
    }
    *p = NUL;		// add trailing NUL
    أرجع len;
}

#إذا defined(USE_داخلہ_BUF)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    صحيح
داخلہ_دستیاب(فراغ)
{
    أرجع (!vim_is_داخلہ_buf_خالی_تھاواں()
# إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#جے_انت

/*
 * Function passed to do_cmdline() to get the command after a <Cmd> key from
 * typeahead.
 */
    ساكن char_u *
getcmdkeycmd(
	صحيح		promptc بن_ورتیا,
	فراغ		*cookie بن_ورتیا,
	صحيح		indent بن_ورتیا,
	getline_opt_T	do_concat بن_ورتیا)
{
    garray_T	سطر_ga;
    صحيح		c1 = -1;
    صحيح		c2;
    صحيح		cmod = 0;
    صحيح		aborted = FALSE;

    ga_init2(&سطر_ga, 1, 32);

    // no mapping for these characters
    no_mapping++;

    got_int = FALSE;
    طالما (c1 != NUL && !aborted)
    {
	إذا (ga_grow(&سطر_ga, 32) == FAIL)
	{
	    aborted = TRUE;
	    اكسر;
	}

	إذا (vgetorpeek(FALSE) == NUL)
	{
	    // incomplete <Cmd> is an error, because there is not much the user
	    // could do in this state.
	    emsg(_(e_cmd_mapping_must_سرا_with_cr));
	    aborted = TRUE;
	    اكسر;
	}

	// Get one character at a time.
	c1 = vgetorpeek(TRUE);

	// Get two extra bytes for special keys
	إذا (c1 == K_SPECIAL)
	{
	    c1 = vgetorpeek(TRUE);
	    c2 = vgetorpeek(TRUE);
	    إذا (c1 == KS_MODIFIER)
	    {
		cmod = c2;
		استمر;
	    }
	    c1 = منزل_SPECIAL(c1, c2);

	    // K_ESC is used to avoid ambiguity with the single Esc character
	    // that might be the start of an escape sequence.  Convert it back
	    // to a single Esc here.
	    إذا (c1 == K_ESC)
		c1 = ESC;

#جے_تعریف_شدہ FEAT_GUI
	    // Translate K_CSI to CSI.  The special key is only used to
	    // avoid it being recognized as the start of a special key.
	    إذا (c1 == K_CSI)
		c1 = CSI;
#جے_انت
	}

	إذا (got_int)
	    aborted = TRUE;
	وإلا إذا (c1 == '\r' || c1 == '\n')
	    c1 = NUL;  // end the line
	وإلا إذا (c1 == ESC)
	    aborted = TRUE;
	وإلا إذا (c1 == K_COMMAND || c1 == K_SCRIPT_COMMAND)
	{
	    // give a nicer error message for this special case
	    emsg(_(e_cmd_mapping_must_سرا_with_cr_before_دوجا_cmd));
	    aborted = TRUE;
	}
	وإلا إذا (c1 == K_SNR)
	{
	    GA_CONCAT_LITERAL(&سطر_ga, "<SNR>");
	}
	وإلا
	{
	    إذا (cmod != 0)
	    {
		ga_append(&سطر_ga, K_SPECIAL);
		ga_append(&سطر_ga, KS_MODIFIER);
		ga_append(&سطر_ga, cmod);
	    }
	    إذا (IS_SPECIAL(c1))
	    {
		ga_append(&سطر_ga, K_SPECIAL);
		ga_append(&سطر_ga, K_دوجا(c1));
		ga_append(&سطر_ga, K_تیجا(c1));
	    }
	    وإلا
		ga_append(&سطر_ga, c1);
	}

	cmod = 0;
    }

    no_mapping--;

    إذا (aborted)
	ga_clear(&سطر_ga);

    أرجع (char_u *)سطر_ga.ga_انکڑے;
}

#إذا defined(FEAT_EVAL)
/*
 * If there was a mapping we get its SID.  Otherwise, use "last_used_sid", it
 * is set when redo'ing.
 * Put this SID in the redo buffer, so that "." will use the same script
 * context.
 */
    فراغ
may_جوڑو_آخری_ورتیا_map_منزل_redobuff(فراغ)
{
    char_u  buf[3 + 20];
    صحيح	    buflen;
    صحيح	    sid = -1;

    إذا (آخری_ورتیا_map != عدم)
	sid = آخری_ورتیا_map->m_script_ctx.sc_sid;
    إذا (sid < 0)
	sid = آخری_ورتیا_sid;

    إذا (sid < 0)
	أرجع;

    // <K_SID>{nr};
    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = KE_SID;
    buflen = 3;

    buflen += vim_snprintf((حرف *)buf + 3, 20, "%d;", sid);
    جوڑو_buff(&redobuff, buf, (لما)buflen);
}
#جے_انت

    صحيح
do_cmdkey_command(صحيح کلید بن_ورتیا, صحيح flags)
{
    صحيح	    res;
#جے_تعریف_شدہ FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    إذا (کلید == K_SCRIPT_COMMAND
		  && (آخری_ورتیا_map != عدم || SCRIPT_ID_VALID(آخری_ورتیا_sid)))
    {
	save_current_sctx = current_sctx;
	إذا (آخری_ورتیا_map != عدم)
	    current_sctx = آخری_ورتیا_map->m_script_ctx;
	وإلا
	{
	    current_sctx.sc_sid = آخری_ورتیا_sid;
	    current_sctx.sc_lnum = 0;
	    current_sctx.sc_نسخہ = SCRIPT_شے(آخری_ورتیا_sid)->sn_نسخہ;
	}
    }
#جے_انت

    res = do_cmdline(عدم, getcmdkeycmd, عدم, flags);

#جے_تعریف_شدہ FEAT_EVAL
    إذا (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#جے_انت

    أرجع res;
}

#إذا defined(FEAT_EVAL)
    فراغ
reset_آخری_ورتیا_map(mapblock_T *mp)
{
    إذا (آخری_ورتیا_map != mp)
	أرجع;

    آخری_ورتیا_map = عدم;
    آخری_ورتیا_sid = -1;
}
#جے_انت
