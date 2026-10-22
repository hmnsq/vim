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
#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_CRYPT)
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
પ્રકારવ્યાખ્યા unsigned પૂર્ણાંક u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
પ્રકારવ્યાખ્યા સંરચના {
    u32_T ચાવીઓ[3];
} zip_state_T;


સ્થિર u32_T crc_32_table[256];

/*
 * Fill the CRC table, if not done already.
 */
    સ્થિર શૂન્ય
make_crc_tab(શૂન્ય)
{
    u32_T	s, t, v;
    સ્થિર પૂર્ણાંક	done = FALSE;

    જો (done)
	પાછા;
    માટે (t = 0; t < 256; t++)
    {
	v = t;
	માટે (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	crc_32_table[t] = v;
    }
    done = TRUE;
}

#define CRC32(c, b) (crc_32_table[((પૂર્ણાંક)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#define DECRYPT_BYTE_ZIP(ચાવીઓ, t) \
{ \
    short_u temp = (short_u)ચાવીઓ[2] | 2; \
    t = (પૂર્ણાંક)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#define UPDATE_ચાવીઓ_ZIP(ચાવીઓ, c) કરો { \
    ચાવીઓ[0] = CRC32(ચાવીઓ[0], (c)); \
    ચાવીઓ[1] += ચાવીઓ[0] & 0xff; \
    ચાવીઓ[1] = ચાવીઓ[1] * 134775813L + 1; \
    ચાવીઓ[2] = CRC32(ચાવીઓ[2], (પૂર્ણાંક)(ચાવીઓ[1] >> 24)); \
} જ્યારે (0)

/*
 * Initialize for encryption/decryption.
 */
    પૂર્ણાંક
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *ચાવી,
    crypt_arg_T     *arg UNUSED)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    જો (zs == ખાલી)
	પાછા FAIL;
    state->method_state = zs;

    make_crc_tab();
    zs->ચાવીઓ[0] = 305419896L;
    zs->ચાવીઓ[1] = 591751049L;
    zs->ચાવીઓ[2] = 878082192L;
    માટે (p = ચાવી; *p != NUL; ++p)
	UPDATE_ચાવીઓ_ZIP(zs->ચાવીઓ, (પૂર્ણાંક)*p);

    પાછા OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    શૂન્ય
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    પૂર્ણાંક		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    પૂર્ણાંક		ztemp, t;

    માટે (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->ચાવીઓ, t);
	UPDATE_ચાવીઓ_ZIP(zs->ચાવીઓ, ztemp);
	to[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    શૂન્ય
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    પૂર્ણાંક		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    short_u	temp;

    માટે (i = 0; i < len; ++i)
    {
	temp = (short_u)zs->ચાવીઓ[2] | 2;
	temp = (પૂર્ણાંક)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff);
	UPDATE_ચાવીઓ_ZIP(zs->ચાવીઓ, to[i] = from[i] ^ temp);
    }
}

#endif // FEAT_CRYPT
