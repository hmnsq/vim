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
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_CRYPT)
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
రకనిర్వచనం గుర్తులేని పూర్ణాంకం u32_T;	// int is at least 32 bits

// The state of encryption, referenced by cryptstate_T.
రకనిర్వచనం నిర్మాణం {
    u32_T కీలకాలు[3];
} zip_state_T;


స్థిర u32_T చక్రీయ_తనిఖీ_32_పట్టిక[256];

/*
 * Fill the CRC table, if not done already.
 */
    స్థిర శూన్య
make_చక్రీయ_తనిఖీ_tab(శూన్య)
{
    u32_T	s, t, v;
    స్థిర పూర్ణాంకం	పూర్తి = FALSE;

    ఒకవేళ (పూర్తి)
	తిరిగి;
    కొరకు (t = 0; t < 256; t++)
    {
	v = t;
	కొరకు (s = 0; s < 8; s++)
	    v = (v >> 1) ^ ((v & 1) * (u32_T)0xedb88320L);
	చక్రీయ_తనిఖీ_32_పట్టిక[t] = v;
    }
    పూర్తి = TRUE;
}

#నిర్వచించు చక్రీయ_తనిఖీ_32(c, b) (చక్రీయ_తనిఖీ_32_పట్టిక[((పూర్ణాంకం)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/*
 * Return the next byte in the pseudo-random sequence.
 */
#నిర్వచించు DECRYPT_BYTE_ZIP(కీలకాలు, t) \
{ \
    short_u తాత్కాలిక = (short_u)కీలకాలు[2] | 2; \
    t = (పూర్ణాంకం)(((గుర్తులేని)(తాత్కాలిక * (తాత్కాలిక ^ 1U)) >> 8) & 0xff); \
}

/*
 * Update the encryption keys with the next byte of plain text.
 */
#నిర్వచించు UPDATE_కీలకాలు_ZIP(కీలకాలు, c) చేయి { \
    కీలకాలు[0] = చక్రీయ_తనిఖీ_32(కీలకాలు[0], (c)); \
    కీలకాలు[1] += కీలకాలు[0] & 0xff; \
    కీలకాలు[1] = కీలకాలు[1] * 134775813L + 1; \
    కీలకాలు[2] = చక్రీయ_తనిఖీ_32(కీలకాలు[2], (పూర్ణాంకం)(కీలకాలు[1] >> 24)); \
} అయితే (0)

/*
 * Initialize for encryption/decryption.
 */
    పూర్ణాంకం
crypt_zip_init(
    cryptstate_T    *state,
    char_u	    *కీలకం,
    crypt_arg_T     *arg వాడని)
{
    char_u	*p;
    zip_state_T	*zs;

    zs = ALLOC_ONE(zip_state_T);
    ఒకవేళ (zs == ఖాళీ)
	తిరిగి FAIL;
    state->పద్ధతి_state = zs;

    make_చక్రీయ_తనిఖీ_tab();
    zs->కీలకాలు[0] = 305419896L;
    zs->కీలకాలు[1] = 591751049L;
    zs->కీలకాలు[2] = 878082192L;
    కొరకు (p = కీలకం; *p != NUL; ++p)
	UPDATE_కీలకాలు_ZIP(zs->కీలకాలు, (పూర్ణాంకం)*p);

    తిరిగి OK;
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    శూన్య
crypt_zip_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*గమ్యం,
    పూర్ణాంకం		చివరి వాడని)
{
    zip_state_T *zs = state->పద్ధతి_state;
    size_t	i;
    పూర్ణాంకం		ztemp, t;

    కొరకు (i = 0; i < len; ++i)
    {
	ztemp = from[i];
	DECRYPT_BYTE_ZIP(zs->కీలకాలు, t);
	UPDATE_కీలకాలు_ZIP(zs->కీలకాలు, ztemp);
	గమ్యం[i] = t ^ ztemp;
    }
}

/*
 * Decrypt "from[len]" into "to[len]".
 */
    శూన్య
crypt_zip_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*గమ్యం,
    పూర్ణాంకం		చివరి వాడని)
{
    zip_state_T *zs = state->పద్ధతి_state;
    size_t	i;
    short_u	తాత్కాలిక;

    కొరకు (i = 0; i < len; ++i)
    {
	తాత్కాలిక = (short_u)zs->కీలకాలు[2] | 2;
	తాత్కాలిక = (పూర్ణాంకం)(((గుర్తులేని)(తాత్కాలిక * (తాత్కాలిక ^ 1U)) >> 8) & 0xff);
	UPDATE_కీలకాలు_ZIP(zs->కీలకాలు, గమ్యం[i] = from[i] ^ తాత్కాలిక);
    }
}

#అయితే_ముగింపు // FEAT_CRYPT
