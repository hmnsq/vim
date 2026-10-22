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
تعریفنوع unsigned صحیح u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
تعریفنوع ساختار {
    u32_T کلیدها[3];
} zip_state_T;


ایستا u32_T crc_32_table[256];

/*
 * Fill the CRC table, if not done already.
 */
    ایستا تهی
make_crc_tab(تهی)
{
    u32_T	s, t, v;
    ایستا صحیح	done = FALSE;

    اگر (done)
	بازگشت;
    برای (t = 0; t < 256; t++)
    {
	v = t;
	برای (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	crc_32_table[t] = v;
    }
    done = TRUE;
}

#define CRC32(c, b) (crc_32_table[((صحیح)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#define DECRYPT_BYTE_ZIP(کلیدها, t) \
{ \
    short_u temp = (short_u)کلیدها[2] | 2; \
    t = (صحیح)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#define UPDATE_کلیدها_ZIP(کلیدها, c) انجامبده { \
    کلیدها[0] = CRC32(کلیدها[0], (c)); \
    کلیدها[1] += کلیدها[0] & 0xff; \
    کلیدها[1] = کلیدها[1] * 134775813L + 1; \
    کلیدها[2] = CRC32(کلیدها[2], (صحیح)(کلیدها[1] >> 24)); \
} وقتی (0)

/*
 * Initialize for encryption/decryption.
 */
    صحیح
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *کلید,
    crypt_arg_T     *arg UNUSED)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    اگر (zs == پوچ)
	بازگشت FAIL;
    state->method_state = zs;

    make_crc_tab();
    zs->کلیدها[0] = 305419896L;
    zs->کلیدها[1] = 591751049L;
    zs->کلیدها[2] = 878082192L;
    برای (p = کلید; *p != NUL; ++p)
	UPDATE_کلیدها_ZIP(zs->کلیدها, (صحیح)*p);

    بازگشت OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    تهی
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    صحیح		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    صحیح		ztemp, t;

    برای (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->کلیدها, t);
	UPDATE_کلیدها_ZIP(zs->کلیدها, ztemp);
	to[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    تهی
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    صحیح		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    short_u	temp;

    برای (i = 0; i < len; ++i)
    {
	temp = (short_u)zs->کلیدها[2] | 2;
	temp = (صحیح)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff);
	UPDATE_کلیدها_ZIP(zs->کلیدها, to[i] = from[i] ^ temp);
    }
}

#endif // FEAT_CRYPT
