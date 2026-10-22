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
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_CRYPT)
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    u32_T ਕੁੰਜੀਆਂ[3];
} zip_state_T;


ਸਥਿਰ u32_T ਚੱਕਰੀ_ਜਾਂਚ_32_ਸਾਰਣੀ[256];

/*
 * Fill the CRC table, if not done already.
 */
    ਸਥਿਰ ਖਾਲੀ
make_ਚੱਕਰੀ_ਜਾਂਚ_tab(ਖਾਲੀ)
{
    u32_T	s, t, v;
    ਸਥਿਰ ਪੂਰਨ	ਪੂਰਾ = FALSE;

    ਜੇ (ਪੂਰਾ)
	ਵਾਪਸ;
    ਲਈ (t = 0; t < 256; t++)
    {
	v = t;
	ਲਈ (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	ਚੱਕਰੀ_ਜਾਂਚ_32_ਸਾਰਣੀ[t] = v;
    }
    ਪੂਰਾ = TRUE;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਚੱਕਰੀ_ਜਾਂਚ_32(c, b) (ਚੱਕਰੀ_ਜਾਂਚ_32_ਸਾਰਣੀ[((ਪੂਰਨ)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECRYPT_BYTE_ZIP(ਕੁੰਜੀਆਂ, t) \
{ \
    short_u ਆਰਜ਼ੀ = (short_u)ਕੁੰਜੀਆਂ[2] | 2; \
    t = (ਪੂਰਨ)(((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਆਰਜ਼ੀ * (ਆਰਜ਼ੀ ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPDATE_ਕੁੰਜੀਆਂ_ZIP(ਕੁੰਜੀਆਂ, c) ਕਰੋ { \
    ਕੁੰਜੀਆਂ[0] = ਚੱਕਰੀ_ਜਾਂਚ_32(ਕੁੰਜੀਆਂ[0], (c)); \
    ਕੁੰਜੀਆਂ[1] += ਕੁੰਜੀਆਂ[0] & 0xff; \
    ਕੁੰਜੀਆਂ[1] = ਕੁੰਜੀਆਂ[1] * 134775813L + 1; \
    ਕੁੰਜੀਆਂ[2] = ਚੱਕਰੀ_ਜਾਂਚ_32(ਕੁੰਜੀਆਂ[2], (ਪੂਰਨ)(ਕੁੰਜੀਆਂ[1] >> 24)); \
} ਜਦੋਂ (0)

/*
 * Initialize for encryption/decryption.
 */
    ਪੂਰਨ
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *ਕੁੰਜੀ,
    crypt_arg_T     *arg ਅਣਵਰਤਿਆ)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    ਜੇ (zs == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    state->ਵਿਧੀ_state = zs;

    make_ਚੱਕਰੀ_ਜਾਂਚ_tab();
    zs->ਕੁੰਜੀਆਂ[0] = 305419896L;
    zs->ਕੁੰਜੀਆਂ[1] = 591751049L;
    zs->ਕੁੰਜੀਆਂ[2] = 878082192L;
    ਲਈ (p = ਕੁੰਜੀ; *p != NUL; ++p)
	UPDATE_ਕੁੰਜੀਆਂ_ZIP(zs->ਕੁੰਜੀਆਂ, (ਪੂਰਨ)*p);

    ਵਾਪਸ OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ਖਾਲੀ
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*ਮੰਜ਼ਿਲ,
    ਪੂਰਨ		ਆਖਰੀ ਅਣਵਰਤਿਆ)
{
    zip_state_T *zs = state->ਵਿਧੀ_state;
    size_t	i;
    ਪੂਰਨ		ztemp, t;

    ਲਈ (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->ਕੁੰਜੀਆਂ, t);
	UPDATE_ਕੁੰਜੀਆਂ_ZIP(zs->ਕੁੰਜੀਆਂ, ztemp);
	ਮੰਜ਼ਿਲ[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    ਖਾਲੀ
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*ਮੰਜ਼ਿਲ,
    ਪੂਰਨ		ਆਖਰੀ ਅਣਵਰਤਿਆ)
{
    zip_state_T *zs = state->ਵਿਧੀ_state;
    size_t	i;
    short_u	ਆਰਜ਼ੀ;

    ਲਈ (i = 0; i < len; ++i)
    {
	ਆਰਜ਼ੀ = (short_u)zs->ਕੁੰਜੀਆਂ[2] | 2;
	ਆਰਜ਼ੀ = (ਪੂਰਨ)(((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਆਰਜ਼ੀ * (ਆਰਜ਼ੀ ^ 1U)) >> 8) & 0xff);
	UPDATE_ਕੁੰਜੀਆਂ_ZIP(zs->ਕੁੰਜੀਆਂ, ਮੰਜ਼ਿਲ[i] = from[i] ^ ਆਰਜ਼ੀ);
    }
}

#ਜੇ_ਅੰਤ // FEAT_CRYPT
