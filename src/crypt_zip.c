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
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_CRYPT)
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
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned ಪೂರ್ಣಾಙ್ಕ u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    u32_T ಕೀಲಿಗಳು[3];
} zip_state_T;


ಸ್ಥಿರ u32_T crc_32_table[256];

/*
 * Fill the CRC table, if not done already.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
make_crc_tab(ಶೂನ್ಯ)
{
    u32_T	s, t, v;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	done = FALSE;

    ಯದಿ (done)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (t = 0; t < 256; t++)
    {
	v = t;
	ಪ್ರತಿ (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	crc_32_table[t] = v;
    }
    done = TRUE;
}

#define CRC32(c, b) (crc_32_table[((ಪೂರ್ಣಾಙ್ಕ)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#define DECRYPT_BYTE_ZIP(ಕೀಲಿಗಳು, t) \
{ \
    short_u temp = (short_u)ಕೀಲಿಗಳು[2] | 2; \
    t = (ಪೂರ್ಣಾಙ್ಕ)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#define UPDATE_ಕೀಲಿಗಳು_ZIP(ಕೀಲಿಗಳು, c) ಕುರು { \
    ಕೀಲಿಗಳು[0] = CRC32(ಕೀಲಿಗಳು[0], (c)); \
    ಕೀಲಿಗಳು[1] += ಕೀಲಿಗಳು[0] & 0xff; \
    ಕೀಲಿಗಳು[1] = ಕೀಲಿಗಳು[1] * 134775813L + 1; \
    ಕೀಲಿಗಳು[2] = CRC32(ಕೀಲಿಗಳು[2], (ಪೂರ್ಣಾಙ್ಕ)(ಕೀಲಿಗಳು[1] >> 24)); \
} ಯಾವತ್ (0)

/*
 * Initialize for encryption/decryption.
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *ಕೀಲಿ,
    crypt_arg_T     *arg UNUSED)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    ಯದಿ (zs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    state->method_state = zs;

    make_crc_tab();
    zs->ಕೀಲಿಗಳು[0] = 305419896L;
    zs->ಕೀಲಿಗಳು[1] = 591751049L;
    zs->ಕೀಲಿಗಳು[2] = 878082192L;
    ಪ್ರತಿ (p = ಕೀಲಿ; *p != NUL; ++p)
	UPDATE_ಕೀಲಿಗಳು_ZIP(zs->ಕೀಲಿಗಳು, (ಪೂರ್ಣಾಙ್ಕ)*p);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ಶೂನ್ಯ
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    ಪೂರ್ಣಾಙ್ಕ		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    ಪೂರ್ಣಾಙ್ಕ		ztemp, t;

    ಪ್ರತಿ (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->ಕೀಲಿಗಳು, t);
	UPDATE_ಕೀಲಿಗಳು_ZIP(zs->ಕೀಲಿಗಳು, ztemp);
	to[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    ಶೂನ್ಯ
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    ಪೂರ್ಣಾಙ್ಕ		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    short_u	temp;

    ಪ್ರತಿ (i = 0; i < len; ++i)
    {
	temp = (short_u)zs->ಕೀಲಿಗಳು[2] | 2;
	temp = (ಪೂರ್ಣಾಙ್ಕ)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff);
	UPDATE_ಕೀಲಿಗಳು_ZIP(zs->ಕೀಲಿಗಳು, to[i] = from[i] ^ temp);
    }
}

#endif // FEAT_CRYPT
