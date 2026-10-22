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
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_CRYPT)
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
തരനിർവചനം unsigned പൂർണ്ണസംഖ്യ u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
തരനിർവചനം ഘടന {
    u32_T താക്കോലുകൾ[3];
} zip_state_T;


സ്ഥിതം u32_T crc_32_table[256];

/*
 * Fill the CRC table, if not done already.
 */
    സ്ഥിതം ശൂന്യം
make_crc_tab(ശൂന്യം)
{
    u32_T	s, t, v;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	done = FALSE;

    എങ്കിൽ (done)
	തിരികെ;
    വേണ്ടി (t = 0; t < 256; t++)
    {
	v = t;
	വേണ്ടി (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	crc_32_table[t] = v;
    }
    done = TRUE;
}

#define CRC32(c, b) (crc_32_table[((പൂർണ്ണസംഖ്യ)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#define DECRYPT_BYTE_ZIP(താക്കോലുകൾ, t) \
{ \
    short_u temp = (short_u)താക്കോലുകൾ[2] | 2; \
    t = (പൂർണ്ണസംഖ്യ)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#define UPDATE_താക്കോലുകൾ_ZIP(താക്കോലുകൾ, c) ചെയ്യുക { \
    താക്കോലുകൾ[0] = CRC32(താക്കോലുകൾ[0], (c)); \
    താക്കോലുകൾ[1] += താക്കോലുകൾ[0] & 0xff; \
    താക്കോലുകൾ[1] = താക്കോലുകൾ[1] * 134775813L + 1; \
    താക്കോലുകൾ[2] = CRC32(താക്കോലുകൾ[2], (പൂർണ്ണസംഖ്യ)(താക്കോലുകൾ[1] >> 24)); \
} ആകുമ്പോൾ (0)

/*
 * Initialize for encryption/decryption.
 */
    പൂർണ്ണസംഖ്യ
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *താക്കോൽ,
    crypt_arg_T     *arg UNUSED)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    എങ്കിൽ (zs == ശൂന്യ)
	തിരികെ FAIL;
    state->method_state = zs;

    make_crc_tab();
    zs->താക്കോലുകൾ[0] = 305419896L;
    zs->താക്കോലുകൾ[1] = 591751049L;
    zs->താക്കോലുകൾ[2] = 878082192L;
    വേണ്ടി (p = താക്കോൽ; *p != NUL; ++p)
	UPDATE_താക്കോലുകൾ_ZIP(zs->താക്കോലുകൾ, (പൂർണ്ണസംഖ്യ)*p);

    തിരികെ OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ശൂന്യം
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    പൂർണ്ണസംഖ്യ		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    പൂർണ്ണസംഖ്യ		ztemp, t;

    വേണ്ടി (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->താക്കോലുകൾ, t);
	UPDATE_താക്കോലുകൾ_ZIP(zs->താക്കോലുകൾ, ztemp);
	to[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    ശൂന്യം
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    പൂർണ്ണസംഖ്യ		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    short_u	temp;

    വേണ്ടി (i = 0; i < len; ++i)
    {
	temp = (short_u)zs->താക്കോലുകൾ[2] | 2;
	temp = (പൂർണ്ണസംഖ്യ)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff);
	UPDATE_താക്കോലുകൾ_ZIP(zs->താക്കോലുകൾ, to[i] = from[i] ^ temp);
    }
}

#endif // FEAT_CRYPT
