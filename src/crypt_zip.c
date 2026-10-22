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
#ضمن "vim.h"

#إذا defined(FEAT_CRYPT)
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
تعريفنوع unsigned صحيح u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
تعريفنوع بنية {
    u32_T ئاچقۇچلار[3];
} zip_state_T;


ساكن u32_T crc_32_table[256];

/*
 * Fill the CRC table, if not done already.
 */
    ساكن فراغ
make_crc_tab(فراغ)
{
    u32_T	s, t, v;
    ساكن صحيح	done = FALSE;

    إذا (done)
	أرجع;
    لكل (t = 0; t < 256; t++)
    {
	v = t;
	لكل (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	crc_32_table[t] = v;
    }
    done = TRUE;
}

#define CRC32(c, b) (crc_32_table[((صحيح)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#define DECRYPT_BYTE_ZIP(ئاچقۇچلار, t) \
{ \
    short_u temp = (short_u)ئاچقۇچلار[2] | 2; \
    t = (صحيح)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#define UPDATE_ئاچقۇچلار_ZIP(ئاچقۇچلار, c) افعل { \
    ئاچقۇچلار[0] = CRC32(ئاچقۇچلار[0], (c)); \
    ئاچقۇچلار[1] += ئاچقۇچلار[0] & 0xff; \
    ئاچقۇچلار[1] = ئاچقۇچلار[1] * 134775813L + 1; \
    ئاچقۇچلار[2] = CRC32(ئاچقۇچلار[2], (صحيح)(ئاچقۇچلار[1] >> 24)); \
} طالما (0)

/*
 * Initialize for encryption/decryption.
 */
    صحيح
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *ئاچقۇچ,
    crypt_arg_T     *arg UNUSED)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    إذا (zs == عدم)
	أرجع FAIL;
    state->method_state = zs;

    make_crc_tab();
    zs->ئاچقۇچلار[0] = 305419896L;
    zs->ئاچقۇچلار[1] = 591751049L;
    zs->ئاچقۇچلار[2] = 878082192L;
    لكل (p = ئاچقۇچ; *p != NUL; ++p)
	UPDATE_ئاچقۇچلار_ZIP(zs->ئاچقۇچلار, (صحيح)*p);

    أرجع OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    فراغ
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    صحيح		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    صحيح		ztemp, t;

    لكل (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->ئاچقۇچلار, t);
	UPDATE_ئاچقۇچلار_ZIP(zs->ئاچقۇچلار, ztemp);
	to[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    فراغ
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    صحيح		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    short_u	temp;

    لكل (i = 0; i < len; ++i)
    {
	temp = (short_u)zs->ئاچقۇچلار[2] | 2;
	temp = (صحيح)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff);
	UPDATE_ئاچقۇچلار_ZIP(zs->ئاچقۇچلار, to[i] = from[i] ^ temp);
    }
}

#endif // FEAT_CRYPT
