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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_CRYPT)
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    u32_T ଚାବିଗୁଡ଼ିକ[3];
} zip_state_T;


ସ୍ଥିର u32_T crc_32_table[256];

/*
 * Fill the CRC table, if not done already.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
make_crc_tab(ଶୂନ୍ୟ)
{
    u32_T	s, t, v;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	done = FALSE;

    ଯଦି (done)
	ଫେରନ୍ତୁ;
    ପାଇଁ (t = 0; t < 256; t++)
    {
	v = t;
	ପାଇଁ (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	crc_32_table[t] = v;
    }
    done = TRUE;
}

#define CRC32(c, b) (crc_32_table[((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#define DECRYPT_BYTE_ZIP(ଚାବିଗୁଡ଼ିକ, t) \
{ \
    short_u temp = (short_u)ଚାବିଗୁଡ଼ିକ[2] | 2; \
    t = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#define UPDATE_ଚାବିଗୁଡ଼ିକ_ZIP(ଚାବିଗୁଡ଼ିକ, c) କର { \
    ଚାବିଗୁଡ଼ିକ[0] = CRC32(ଚାବିଗୁଡ଼ିକ[0], (c)); \
    ଚାବିଗୁଡ଼ିକ[1] += ଚାବିଗୁଡ଼ିକ[0] & 0xff; \
    ଚାବିଗୁଡ଼ିକ[1] = ଚାବିଗୁଡ଼ିକ[1] * 134775813L + 1; \
    ଚାବିଗୁଡ଼ିକ[2] = CRC32(ଚାବିଗୁଡ଼ିକ[2], (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଚାବିଗୁଡ଼ିକ[1] >> 24)); \
} ଯେତେବେଳେ (0)

/*
 * Initialize for encryption/decryption.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *ଚାବି,
    crypt_arg_T     *arg UNUSED)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    ଯଦି (zs == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    state->method_state = zs;

    make_crc_tab();
    zs->ଚାବିଗୁଡ଼ିକ[0] = 305419896L;
    zs->ଚାବିଗୁଡ଼ିକ[1] = 591751049L;
    zs->ଚାବିଗୁଡ଼ିକ[2] = 878082192L;
    ପାଇଁ (p = ଚାବି; *p != NUL; ++p)
	UPDATE_ଚାବିଗୁଡ଼ିକ_ZIP(zs->ଚାବିଗୁଡ଼ିକ, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*p);

    ଫେରନ୍ତୁ OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ଶୂନ୍ୟ
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ztemp, t;

    ପାଇଁ (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->ଚାବିଗୁଡ଼ିକ, t);
	UPDATE_ଚାବିଗୁଡ଼ିକ_ZIP(zs->ଚାବିଗୁଡ଼ିକ, ztemp);
	to[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    ଶୂନ୍ୟ
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last UNUSED)
{
    zip_state_T *zs = state->method_state;
    size_t	i;
    short_u	temp;

    ପାଇଁ (i = 0; i < len; ++i)
    {
	temp = (short_u)zs->ଚାବିଗୁଡ଼ିକ[2] | 2;
	temp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(((unsigned)(temp * (temp ^ 1U)) >> 8) & 0xff);
	UPDATE_ଚାବିଗୁଡ଼ିକ_ZIP(zs->ଚାବିଗୁଡ଼ିକ, to[i] = from[i] ^ temp);
    }
}

#endif // FEAT_CRYPT
