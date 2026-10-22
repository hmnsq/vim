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
#अन्तर्भाव "vim.h"

#यदि defined(FEAT_CRYPT)
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
प्रकारनिर्देश अचिन्हित पूर्णाङ्क u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
प्रकारनिर्देश संरचना {
    u32_T कुंजियाँ[3];
} zip_state_T;


स्थिर u32_T crc_32_table[256];

/*
 * Fill the CRC table, if not done already.
 */
    स्थिर शून्य
make_crc_tab(शून्य)
{
    u32_T	s, t, v;
    स्थिर पूर्णाङ्क	done = FALSE;

    यदि (done)
	प्रत्यागम;
    प्रति (t = 0; t < 256; t++)
    {
	v = t;
	प्रति (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	crc_32_table[t] = v;
    }
    done = TRUE;
}

#define CRC32(c, b) (crc_32_table[((पूर्णाङ्क)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#define DECRYPT_BYTE_ZIP(कुंजियाँ, t) \
{ \
    short_u temp = (short_u)कुंजियाँ[2] | 2; \
    t = (पूर्णाङ्क)(((अचिन्हित)(temp * (temp ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#define UPDATE_कुंजियाँ_ZIP(कुंजियाँ, c) कुरु { \
    कुंजियाँ[0] = CRC32(कुंजियाँ[0], (c)); \
    कुंजियाँ[1] += कुंजियाँ[0] & 0xff; \
    कुंजियाँ[1] = कुंजियाँ[1] * 134775813L + 1; \
    कुंजियाँ[2] = CRC32(कुंजियाँ[2], (पूर्णाङ्क)(कुंजियाँ[1] >> 24)); \
} यावत् (0)

/*
 * Initialize for encryption/decryption.
 */
    पूर्णाङ्क
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *कुंजी,
    crypt_arg_T     *arg UNUSED)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    यदि (zs == रिक्त)
	प्रत्यागम FAIL;
    state->method_state = zs;

    make_crc_tab();
    zs->कुंजियाँ[0] = 305419896L;
    zs->कुंजियाँ[1] = 591751049L;
    zs->कुंजियाँ[2] = 878082192L;
    प्रति (p = कुंजी; *p != NUL; ++p)
	UPDATE_कुंजियाँ_ZIP(zs->कुंजियाँ, (पूर्णाङ्क)*p);

    प्रत्यागम OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    शून्य
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    माप_प्रकार	len,
    char_u	*to,
    पूर्णाङ्क		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    माप_प्रकार	i;
    पूर्णाङ्क		ztemp, t;

    प्रति (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->कुंजियाँ, t);
	UPDATE_कुंजियाँ_ZIP(zs->कुंजियाँ, ztemp);
	to[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    शून्य
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    माप_प्रकार	len,
    char_u	*to,
    पूर्णाङ्क		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    माप_प्रकार	i;
    short_u	temp;

    प्रति (i = 0; i < len; ++i)
    {
	temp = (short_u)zs->कुंजियाँ[2] | 2;
	temp = (पूर्णाङ्क)(((अचिन्हित)(temp * (temp ^ 1U)) >> 8) & 0xff);
	UPDATE_कुंजियाँ_ZIP(zs->कुंजियाँ, to[i] = from[i] ^ temp);
    }
}

#endif // FEAT_CRYPT
