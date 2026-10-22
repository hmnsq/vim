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

#ಆದರೆ defined(FEAT_CRYPT)
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    u32_T ಕೀಲಿಗಳು[3];
} zip_state_T;


ಸ್ಥಿರ u32_T ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32_ಕೋಷ್ಟಕ[256];

/*
 * Fill the CRC table, if not done already.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
make_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_tab(ಶೂನ್ಯ)
{
    u32_T	s, t, v;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಮುಗಿದ = FALSE;

    ಆದರೆ (ಮುಗಿದ)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (t = 0; t < 256; t++)
    {
	v = t;
	ಗಾಗಿ (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32_ಕೋಷ್ಟಕ[t] = v;
    }
    ಮುಗಿದ = TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32(c, b) (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32_ಕೋಷ್ಟಕ[((ಪೂರ್ಣಾಂಕ)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DECRYPT_BYTE_ZIP(ಕೀಲಿಗಳು, t) \
{ \
    short_u ತಾತ್ಕಾಲಿಕ = (short_u)ಕೀಲಿಗಳು[2] | 2; \
    t = (ಪೂರ್ಣಾಂಕ)(((ಚಿಹ್ನೆರಹಿತ)(ತಾತ್ಕಾಲಿಕ * (ತಾತ್ಕಾಲಿಕ ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UPDATE_ಕೀಲಿಗಳು_ZIP(ಕೀಲಿಗಳು, c) ಮಾಡು { \
    ಕೀಲಿಗಳು[0] = ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32(ಕೀಲಿಗಳು[0], (c)); \
    ಕೀಲಿಗಳು[1] += ಕೀಲಿಗಳು[0] & 0xff; \
    ಕೀಲಿಗಳು[1] = ಕೀಲಿಗಳು[1] * 134775813L + 1; \
    ಕೀಲಿಗಳು[2] = ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32(ಕೀಲಿಗಳು[2], (ಪೂರ್ಣಾಂಕ)(ಕೀಲಿಗಳು[1] >> 24)); \
} ಆಗ (0)

/*
 * Initialize for encryption/decryption.
 */
    ಪೂರ್ಣಾಂಕ
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *ಕೀಲಿ,
    crypt_arg_T     *arg ಬಳಸದ)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    ಆದರೆ (zs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    state->ವಿಧಾನ_state = zs;

    make_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_tab();
    zs->ಕೀಲಿಗಳು[0] = 305419896L;
    zs->ಕೀಲಿಗಳು[1] = 591751049L;
    zs->ಕೀಲಿಗಳು[2] = 878082192L;
    ಗಾಗಿ (p = ಕೀಲಿ; *p != NUL; ++p)
	UPDATE_ಕೀಲಿಗಳು_ZIP(zs->ಕೀಲಿಗಳು, (ಪೂರ್ಣಾಂಕ)*p);

    ಹಿಂತಿರುಗಿ OK;
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
    char_u	*ಗಮ್ಯ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ ಬಳಸದ)
{
    zip_state_T *zs = state->ವಿಧಾನ_state;
    size_t	i;
    ಪೂರ್ಣಾಂಕ		ztemp, t;

    ಗಾಗಿ (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->ಕೀಲಿಗಳು, t);
	UPDATE_ಕೀಲಿಗಳು_ZIP(zs->ಕೀಲಿಗಳು, ztemp);
	ಗಮ್ಯ[i] = t ^ ztemp;
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
    char_u	*ಗಮ್ಯ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ ಬಳಸದ)
{
    zip_state_T *zs = state->ವಿಧಾನ_state;
    size_t	i;
    short_u	ತಾತ್ಕಾಲಿಕ;

    ಗಾಗಿ (i = 0; i < len; ++i)
    {
	ತಾತ್ಕಾಲಿಕ = (short_u)zs->ಕೀಲಿಗಳು[2] | 2;
	ತಾತ್ಕಾಲಿಕ = (ಪೂರ್ಣಾಂಕ)(((ಚಿಹ್ನೆರಹಿತ)(ತಾತ್ಕಾಲಿಕ * (ತಾತ್ಕಾಲಿಕ ^ 1U)) >> 8) & 0xff);
	UPDATE_ಕೀಲಿಗಳು_ZIP(zs->ಕೀಲಿಗಳು, ಗಮ್ಯ[i] = from[i] ^ ತಾತ್ಕಾಲಿಕ);
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_CRYPT
