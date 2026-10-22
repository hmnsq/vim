/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * crypt_zip.c: Zip encryption support.
 */
#ضمن "vim.h"

#إذا defined(FEAT_CRYPT)
/*
 * Optional encryption support.
 * Mohsin Ahmed, mosh@sasi.com, 98-09-24
 * Based on zip/crypt sources.
 *
 * NOTE FOR USA: Since 2000 exporting this code from the USA is allowed to
 * most countries.  There are a few exceptions, but that still should not be a
 * problem since this code was originally created in Europe and India.
 */

// Need a type that should be 32 bits. 64 also works but wastes space.
تعريفنوع بینښه صحيح u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
تعريفنوع بنية {
    u32_T کلیګانې[3];
} zip_state_T;


ساكن u32_T دوراني_ازموینه_32_جدول[256];

/*
 * Fill the CRC table, if not done already.
 */
    ساكن فراغ
make_دوراني_ازموینه_tab(فراغ)
{
    u32_T	s, t, v;
    ساكن صحيح	بشپړ = FALSE;

    إذا (بشپړ)
	أرجع;
    لكل (t = 0; t < 256; t++)
    {
	v = t;
	لكل (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	دوراني_ازموینه_32_جدول[t] = v;
    }
    بشپړ = TRUE;
}

#تعریف_کړه دوراني_ازموینه_32(c, b) (دوراني_ازموینه_32_جدول[((صحيح)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#تعریف_کړه DECRYPT_BYTE_ZIP(کلیګانې, t) \
{ \
    short_u لنډمهاله = (short_u)کلیګانې[2] | 2; \
    t = (صحيح)(((بینښه)(لنډمهاله * (لنډمهاله ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#تعریف_کړه UPDATE_کلیګانې_ZIP(کلیګانې, c) افعل { \
    کلیګانې[0] = دوراني_ازموینه_32(کلیګانې[0], (c)); \
    کلیګانې[1] += کلیګانې[0] & 0xff; \
    کلیګانې[1] = کلیګانې[1] * 134775813L + 1; \
    کلیګانې[2] = دوراني_ازموینه_32(کلیګانې[2], (صحيح)(کلیګانې[1] >> 24)); \
} طالما (0)

/*
 * Initialize for encryption/decryption.
 */
    صحيح
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *کلی,
    crypt_arg_T     *arg ناکارول_شوی)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    إذا (zs == عدم)
	أرجع FAIL;
    state->لاره_state = zs;

    make_دوراني_ازموینه_tab();
    zs->کلیګانې[0] = 305419896L;
    zs->کلیګانې[1] = 591751049L;
    zs->کلیګانې[2] = 878082192L;
    لكل (p = کلی; *p != NUL; ++p)
	UPDATE_کلیګانې_ZIP(zs->کلیګانې, (صحيح)*p);

    أرجع OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    فراغ
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*منزل,
    صحيح		وروستی ناکارول_شوی)
{
    zip_state_T *zs = state->لاره_state;
    size_t	i;
    صحيح		ztemp, t;

    لكل (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->کلیګانې, t);
	UPDATE_کلیګانې_ZIP(zs->کلیګانې, ztemp);
	منزل[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    فراغ
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*منزل,
    صحيح		وروستی ناکارول_شوی)
{
    zip_state_T *zs = state->لاره_state;
    size_t	i;
    short_u	لنډمهاله;

    لكل (i = 0; i < len; ++i)
    {
	لنډمهاله = (short_u)zs->کلیګانې[2] | 2;
	لنډمهاله = (صحيح)(((بینښه)(لنډمهاله * (لنډمهاله ^ 1U)) >> 8) & 0xff);
	UPDATE_کلیګانې_ZIP(zs->کلیګانې, منزل[i] = from[i] ^ لنډمهاله);
    }
}

#د_که_پای // FEAT_CRYPT
