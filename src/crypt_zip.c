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
#شامل "vim.h"

#اگر defined(FEAT_CRYPT)
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
قسمتعریف غیرعلامتی صحیح u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
قسمتعریف ساخت {
    u32_T کلیدیں[3];
} zip_state_T;


ساکن u32_T چکری_جانچ_32_جدول[256];

/*
 * Fill the CRC table, if not done already.
 */
    ساکن خالی
make_چکری_جانچ_tab(خالی)
{
    u32_T	s, t, v;
    ساکن صحیح	مکمل = FALSE;

    اگر (مکمل)
	واپس;
    برائے (t = 0; t < 256; t++)
    {
	v = t;
	برائے (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	چکری_جانچ_32_جدول[t] = v;
    }
    مکمل = TRUE;
}

#تعریف_کرو چکری_جانچ_32(c, b) (چکری_جانچ_32_جدول[((صحیح)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#تعریف_کرو DECRYPT_BYTE_ZIP(کلیدیں, t) \
{ \
    short_u عارضی = (short_u)کلیدیں[2] | 2; \
    t = (صحیح)(((غیرعلامتی)(عارضی * (عارضی ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#تعریف_کرو UPDATE_کلیدیں_ZIP(کلیدیں, c) کرو { \
    کلیدیں[0] = چکری_جانچ_32(کلیدیں[0], (c)); \
    کلیدیں[1] += کلیدیں[0] & 0xff; \
    کلیدیں[1] = کلیدیں[1] * 134775813L + 1; \
    کلیدیں[2] = چکری_جانچ_32(کلیدیں[2], (صحیح)(کلیدیں[1] >> 24)); \
} جب (0)

/*
 * Initialize for encryption/decryption.
 */
    صحیح
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *کلید,
    crypt_arg_T     *arg غیر_مستعمل)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    اگر (zs == تہی)
	واپس FAIL;
    state->طریقہ_state = zs;

    make_چکری_جانچ_tab();
    zs->کلیدیں[0] = 305419896L;
    zs->کلیدیں[1] = 591751049L;
    zs->کلیدیں[2] = 878082192L;
    برائے (p = کلید; *p != NUL; ++p)
	UPDATE_کلیدیں_ZIP(zs->کلیدیں, (صحیح)*p);

    واپس OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    خالی
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*مقصد,
    صحیح		آخری غیر_مستعمل)
{
    zip_state_T *zs = state->طریقہ_state;
    size_t	i;
    صحیح		ztemp, t;

    برائے (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->کلیدیں, t);
	UPDATE_کلیدیں_ZIP(zs->کلیدیں, ztemp);
	مقصد[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    خالی
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*مقصد,
    صحیح		آخری غیر_مستعمل)
{
    zip_state_T *zs = state->طریقہ_state;
    size_t	i;
    short_u	عارضی;

    برائے (i = 0; i < len; ++i)
    {
	عارضی = (short_u)zs->کلیدیں[2] | 2;
	عارضی = (صحیح)(((غیرعلامتی)(عارضی * (عارضی ^ 1U)) >> 8) & 0xff);
	UPDATE_کلیدیں_ZIP(zs->کلیدیں, مقصد[i] = from[i] ^ عارضی);
    }
}

#اگر_ختم // FEAT_CRYPT
