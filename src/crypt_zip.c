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
تعريفنوع بیعلامت صحيح u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
تعريفنوع بنية {
    u32_T کلیدها[3];
} zip_state_T;


ساكن u32_T بررسی_چرخهای_32_جدول[256];

/*
 * Fill the CRC table, if not done already.
 */
    ساكن فراغ
make_بررسی_چرخهای_tab(فراغ)
{
    u32_T	s, t, v;
    ساكن صحيح	انجامشده = FALSE;

    إذا (انجامشده)
	أرجع;
    لكل (t = 0; t < 256; t++)
    {
	v = t;
	لكل (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	بررسی_چرخهای_32_جدول[t] = v;
    }
    انجامشده = TRUE;
}

#تعریف_کن بررسی_چرخهای_32(c, b) (بررسی_چرخهای_32_جدول[((صحيح)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#تعریف_کن DECRYPT_BYTE_ZIP(کلیدها, t) \
{ \
    short_u موقت = (short_u)کلیدها[2] | 2; \
    t = (صحيح)(((بیعلامت)(موقت * (موقت ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#تعریف_کن UPDATE_کلیدها_ZIP(کلیدها, c) افعل { \
    کلیدها[0] = بررسی_چرخهای_32(کلیدها[0], (c)); \
    کلیدها[1] += کلیدها[0] & 0xff; \
    کلیدها[1] = کلیدها[1] * 134775813L + 1; \
    کلیدها[2] = بررسی_چرخهای_32(کلیدها[2], (صحيح)(کلیدها[1] >> 24)); \
} طالما (0)

/*
 * Initialize for encryption/decryption.
 */
    صحيح
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *کلید,
    crypt_arg_T     *arg استفادهنشده)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    إذا (zs == عدم)
	أرجع FAIL;
    state->روش_state = zs;

    make_بررسی_چرخهای_tab();
    zs->کلیدها[0] = 305419896L;
    zs->کلیدها[1] = 591751049L;
    zs->کلیدها[2] = 878082192L;
    لكل (p = کلید; *p != NUL; ++p)
	UPDATE_کلیدها_ZIP(zs->کلیدها, (صحيح)*p);

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
    char_u	*مقصد,
    صحيح		آخرین استفادهنشده)
{
    zip_state_T *zs = state->روش_state;
    size_t	i;
    صحيح		ztemp, t;

    لكل (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->کلیدها, t);
	UPDATE_کلیدها_ZIP(zs->کلیدها, ztemp);
	مقصد[i] = t ^ ztemp;
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
    char_u	*مقصد,
    صحيح		آخرین استفادهنشده)
{
    zip_state_T *zs = state->روش_state;
    size_t	i;
    short_u	موقت;

    لكل (i = 0; i < len; ++i)
    {
	موقت = (short_u)zs->کلیدها[2] | 2;
	موقت = (صحيح)(((بیعلامت)(موقت * (موقت ^ 1U)) >> 8) & 0xff);
	UPDATE_کلیدها_ZIP(zs->کلیدها, مقصد[i] = from[i] ^ موقت);
    }
}

#پایان_اگر // FEAT_CRYPT
