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
#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_CRYPT)
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
প্রকারনির্দেশ unsigned পূর্ণাঙ্ক u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
প্রকারনির্দেশ সংরচনা {
    u32_T চাবিগুলি[3];
} zip_state_T;


স্থির u32_T crc_32_table[256];

/*
 * Fill the CRC table, if not done already.
 */
    স্থির শূন্য
make_crc_tab(শূন্য)
{
    u32_T	s, t, v;
    স্থির পূর্ণাঙ্ক	done = FALSE;

    যদি (done)
	প্রত্যাগম;
    প্রতি (t = 0; t < 256; t++)
    {
	v = t;
	প্রতি (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	crc_32_table[t] = v;
    }
    done = TRUE;
}

#define CRC32(c, b) (crc_32_table[((পূর্ণাঙ্ক)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#define DECRYPT_BYTE_ZIP(চাবিগুলি, t) \
{ \
    short_u temp = (short_u)চাবিগুলি[2] | 2; \
    t = (পূর্ণাঙ্ক)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#define UPDATE_চাবিগুলি_ZIP(চাবিগুলি, c) কুরু { \
    চাবিগুলি[0] = CRC32(চাবিগুলি[0], (c)); \
    চাবিগুলি[1] += চাবিগুলি[0] & 0xff; \
    চাবিগুলি[1] = চাবিগুলি[1] * 134775813L + 1; \
    চাবিগুলি[2] = CRC32(চাবিগুলি[2], (পূর্ণাঙ্ক)(চাবিগুলি[1] >> 24)); \
} যাবত্ (0)

/*
 * Initialize for encryption/decryption.
 */
    পূর্ণাঙ্ক
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *চাবি,
    crypt_arg_T     *arg UNUSED)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    যদি (zs == রিক্ত)
	প্রত্যাগম FAIL;
    state->method_state = zs;

    make_crc_tab();
    zs->চাবিগুলি[0] = 305419896L;
    zs->চাবিগুলি[1] = 591751049L;
    zs->চাবিগুলি[2] = 878082192L;
    প্রতি (p = চাবি; *p != NUL; ++p)
	UPDATE_চাবিগুলি_ZIP(zs->চাবিগুলি, (পূর্ণাঙ্ক)*p);

    প্রত্যাগম OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    শূন্য
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    পূর্ণাঙ্ক		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    পূর্ণাঙ্ক		ztemp, t;

    প্রতি (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->চাবিগুলি, t);
	UPDATE_চাবিগুলি_ZIP(zs->চাবিগুলি, ztemp);
	to[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    শূন্য
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    পূর্ণাঙ্ক		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    short_u	temp;

    প্রতি (i = 0; i < len; ++i)
    {
	temp = (short_u)zs->চাবিগুলি[2] | 2;
	temp = (পূর্ণাঙ্ক)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff);
	UPDATE_চাবিগুলি_ZIP(zs->চাবিগুলি, to[i] = from[i] ^ temp);
    }
}

#endif // FEAT_CRYPT
