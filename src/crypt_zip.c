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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_CRYPT)
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
வகைவரை unsigned முழு u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
வகைவரை கட்டமைப்பு {
    u32_T சாவிகள்[3];
} zip_state_T;


நிலையான u32_T crc_32_table[256];

/*
 * Fill the CRC table, if not done already.
 */
    நிலையான வெறுமை
make_crc_tab(வெறுமை)
{
    u32_T	s, t, v;
    நிலையான முழு	done = FALSE;

    எனில் (done)
	திரும்பு;
    க்கு (t = 0; t < 256; t++)
    {
	v = t;
	க்கு (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	crc_32_table[t] = v;
    }
    done = TRUE;
}

#define CRC32(c, b) (crc_32_table[((முழு)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#define DECRYPT_BYTE_ZIP(சாவிகள், t) \
{ \
    short_u temp = (short_u)சாவிகள்[2] | 2; \
    t = (முழு)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#define UPDATE_சாவிகள்_ZIP(சாவிகள், c) செய் { \
    சாவிகள்[0] = CRC32(சாவிகள்[0], (c)); \
    சாவிகள்[1] += சாவிகள்[0] & 0xff; \
    சாவிகள்[1] = சாவிகள்[1] * 134775813L + 1; \
    சாவிகள்[2] = CRC32(சாவிகள்[2], (முழு)(சாவிகள்[1] >> 24)); \
} போது (0)

/*
 * Initialize for encryption/decryption.
 */
    முழு
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *சாவி,
    crypt_arg_T     *arg UNUSED)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    எனில் (zs == வெற்று)
	திரும்பு FAIL;
    state->method_state = zs;

    make_crc_tab();
    zs->சாவிகள்[0] = 305419896L;
    zs->சாவிகள்[1] = 591751049L;
    zs->சாவிகள்[2] = 878082192L;
    க்கு (p = சாவி; *p != NUL; ++p)
	UPDATE_சாவிகள்_ZIP(zs->சாவிகள், (முழு)*p);

    திரும்பு OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    வெறுமை
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    முழு		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    முழு		ztemp, t;

    க்கு (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->சாவிகள், t);
	UPDATE_சாவிகள்_ZIP(zs->சாவிகள், ztemp);
	to[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    வெறுமை
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    முழு		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    short_u	temp;

    க்கு (i = 0; i < len; ++i)
    {
	temp = (short_u)zs->சாவிகள்[2] | 2;
	temp = (முழு)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff);
	UPDATE_சாவிகள்_ZIP(zs->சாவிகள், to[i] = from[i] ^ temp);
    }
}

#endif // FEAT_CRYPT
