/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * crypt.c: Generic encryption support.
 */
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_CRYPT)
/*
 * Optional encryption support.
 * Mohsin Ahmed, mosh@sasi.com, 1998-09-24
 * Based on zip/crypt sources.
 * Refactored by David Leadbeater, 2014.
 *
 * NOTE FOR USA: Since 2000 exporting this code from the USA is allowed to
 * most countries.  There are a few exceptions, but that still should not be a
 * problem since this code was originally created in Europe and India.
 *
 * Blowfish addition originally made by Mohsin Ahmed (2010‑03‑14).
 * Original link (www.cs.albany.edu/~mosh) is no longer available.
 * Based on blowfish by Bruce Schneier (http://www.schneier.com/blowfish.html)
 * and sha256 by Christophe Devine.
 */

రకనిర్వచనం నిర్మాణం {
    అక్షరం	*name;		// encryption name as used in 'cryptmethod'
    అక్షరం	*magic;		// magic bytes stored in file header
    పూర్ణాంకం		salt_len;	// length of salt, or 0 when not using salt
    పూర్ణాంకం		విత్తనం_len;	// length of seed, or 0 when not using seed
    పూర్ణాంకం		కూడు_len;	// additional length in the header needed for storing
				// custom data
#నిర్వచితమైతే CRYPT_NOT_INPLACE
    పూర్ణాంకం		పనిచేస్తుంది_inplace;	// encryption/decryption can be done in-place
#అయితే_ముగింపు
    పూర్ణాంకం		సంపూర్ణ_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    పూర్ణాంకం (*self_test_fn)(శూన్య);

    // Function pointer for initializing encryption/decryption.
    పూర్ణాంకం (* init_fn)(cryptstate_T *state, char_u *కీలకం, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    శూన్య (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *గమ్యం, పూర్ణాంకం చివరి);
    శూన్య (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *గమ్యం, పూర్ణాంకం చివరి);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    దీర్ఘ (*encode_బఫరు_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, పూర్ణాంకం చివరి);
    దీర్ఘ (*decode_బఫరు_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, పూర్ణాంకం చివరి);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    శూన్య (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, పూర్ణాంకం చివరి);
    శూన్య (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, పూర్ణాంకం చివరి);
} cryptmethod_T;

స్థిర పూర్ణాంకం crypt_sodium_init_(cryptstate_T *state, char_u *కీలకం, crypt_arg_T *arg);
స్థిర దీర్ఘ crypt_sodium_బఫరు_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_బహిర్గతం, పూర్ణాంకం చివరి);
స్థిర దీర్ఘ crypt_sodium_బఫరు_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_బహిర్గతం, పూర్ణాంకం చివరి);
#ఒకవేళ defined(FEAT_SODIUM)
స్థిర శూన్య crypt_long_long_గమ్యం_char(దీర్ఘ దీర్ఘ n, char_u *s);
స్థిర శూన్య crypt_int_గమ్యం_char(పూర్ణాంకం n, char_u *s);
స్థిర దీర్ఘ దీర్ఘ crypt_char_గమ్యం_long_long(char_u *s);
స్థిర పూర్ణాంకం crypt_char_గమ్యం_int(char_u *s);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL) && defined(FEAT_SODIUM)
స్థిర శూన్య crypt_sodium_report_hash_params(గుర్తులేని దీర్ఘ దీర్ఘ opslimit, గుర్తులేని దీర్ఘ దీర్ఘ ops_def, size_t memlimit, size_t mem_def, పూర్ణాంకం alg, పూర్ణాంకం alg_def);
#అయితే_ముగింపు

// index is method_nr of cryptstate_T, CRYPT_M_*
స్థిర cryptmethod_T cryptmethods[CRYPT_M_లెక్క] = {
    // PK_Zip; very weak
    {
	"zip",
	"VimCrypt~01!",
	0,
	0,
	0,
#నిర్వచితమైతే CRYPT_NOT_INPLACE
	TRUE,
#అయితే_ముగింపు
	FALSE,
	ఖాళీ,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	ఖాళీ, ఖాళీ,
	crypt_zip_encode, crypt_zip_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; implementation issues.
    {
	"blowfish",
	"VimCrypt~02!",
	8,
	8,
	0,
#నిర్వచితమైతే CRYPT_NOT_INPLACE
	TRUE,
#అయితే_ముగింపు
	FALSE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	ఖాళీ, ఖాళీ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; fixed.
    {
	"blowfish2",
	"VimCrypt~03!",
	8,
	8,
	0,
#నిర్వచితమైతే CRYPT_NOT_INPLACE
	TRUE,
#అయితే_ముగింపు
	TRUE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	ఖాళీ, ఖాళీ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#నిర్వచితమైతే FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#లేకపోతే
	16,
#అయితే_ముగింపు
	8,
	0,
#నిర్వచితమైతే CRYPT_NOT_INPLACE
	FALSE,
#అయితే_ముగింపు
	FALSE,
	ఖాళీ,
	crypt_sodium_init_,
	ఖాళీ, ఖాళీ,
	crypt_sodium_బఫరు_encode, crypt_sodium_బఫరు_decode,
	ఖాళీ, ఖాళీ,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#నిర్వచితమైతే FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#లేకపోతే
	16,
#అయితే_ముగింపు
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#నిర్వచితమైతే CRYPT_NOT_INPLACE
	FALSE,
#అయితే_ముగింపు
	FALSE,
	ఖాళీ,
	crypt_sodium_init_,
	ఖాళీ, ఖాళీ,
	crypt_sodium_బఫరు_encode, crypt_sodium_బఫరు_decode,
	ఖాళీ, ఖాళీ,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#ఒకవేళ defined(FEAT_SODIUM)
రకనిర్వచనం నిర్మాణం {
    size_t	    లెక్క;
    గుర్తులేని అక్షరం   కీలకం[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define ఎక్కించు_dll vim_ఎక్కించు_Lib
#   define చిహ్నం_from_dll Get_Proc_చిరునామా
#   define close_dll FreeLibrary
#   define ఎక్కించు_dll_error GetWin32Error
#  లేకపోతే
#   error Dynamic loading యొక్క libsodium is not supported కొరకు now.
//#   define HINSTANCE void*
//#   define SODIUM_PROC void*
//#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
//#   define symbol_from_dll dlsym
//#   define close_dll dlclose
//#   define load_dll_error dlerror
#  endif

#  define sodium_init	    ఎక్కించు_sodium
#  define sodium_free	    dll_sodium_free
#  define sodium_malloc	    dll_sodium_malloc
#  define sodium_memzero    dll_sodium_memzero
#  define sodium_mlock	    dll_sodium_mlock
#  define sodium_munlock    dll_sodium_munlock
#  define crypto_secretstream_xchacha20poly1305_init_నెట్టు \
    dll_crypto_secretstream_xchacha20poly1305_init_నెట్టు
#  define crypto_secretstream_xchacha20poly1305_నెట్టు \
    dll_crypto_secretstream_xchacha20poly1305_నెట్టు
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

స్థిర పూర్ణాంకం (*dll_sodium_init)(శూన్య) = ఖాళీ;
స్థిర శూన్య (*dll_sodium_free)(శూన్య *) = ఖాళీ;
స్థిర శూన్య *(*dll_sodium_malloc)(స్థిరరాశి size_t) = ఖాళీ;
స్థిర శూన్య (*dll_sodium_memzero)(శూన్య * స్థిరరాశి, స్థిరరాశి size_t) = ఖాళీ;
స్థిర పూర్ణాంకం (*dll_sodium_mlock)(శూన్య * స్థిరరాశి, స్థిరరాశి size_t) = ఖాళీ;
స్థిర పూర్ణాంకం (*dll_sodium_munlock)(శూన్య * స్థిరరాశి, స్థిరరాశి size_t) = ఖాళీ;
స్థిర పూర్ణాంకం (*dll_crypto_secretstream_xchacha20poly1305_init_నెట్టు)
   (crypto_secretstream_xchacha20poly1305_state *state,
    గుర్తులేని అక్షరం [],
    స్థిరరాశి గుర్తులేని అక్షరం []) = ఖాళీ;
స్థిర పూర్ణాంకం (*dll_crypto_secretstream_xchacha20poly1305_నెట్టు)
   (crypto_secretstream_xchacha20poly1305_state *state,
    గుర్తులేని అక్షరం *c, గుర్తులేని దీర్ఘ దీర్ఘ *clen_p,
    స్థిరరాశి గుర్తులేని అక్షరం *m, గుర్తులేని దీర్ఘ దీర్ఘ mlen,
    స్థిరరాశి గుర్తులేని అక్షరం *ad, గుర్తులేని దీర్ఘ దీర్ఘ adlen, గుర్తులేని అక్షరం ట్యాగ్)
    = ఖాళీ;
స్థిర పూర్ణాంకం (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    స్థిరరాశి గుర్తులేని అక్షరం [],
    స్థిరరాశి గుర్తులేని అక్షరం []) = ఖాళీ;
స్థిర పూర్ణాంకం (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    గుర్తులేని అక్షరం *m, గుర్తులేని దీర్ఘ దీర్ఘ *mlen_p, గుర్తులేని అక్షరం *ట్యాగ్_p,
    స్థిరరాశి గుర్తులేని అక్షరం *c, గుర్తులేని దీర్ఘ దీర్ఘ clen,
    స్థిరరాశి గుర్తులేని అక్షరం *ad, గుర్తులేని దీర్ఘ దీర్ఘ adlen) = ఖాళీ;
స్థిర పూర్ణాంకం (*dll_crypto_pwhash)(గుర్తులేని అక్షరం * స్థిరరాశి బహిర్గతం,
    గుర్తులేని దీర్ఘ దీర్ఘ outlen,
    స్థిరరాశి అక్షరం * స్థిరరాశి passwd, గుర్తులేని దీర్ఘ దీర్ఘ passwdlen,
    స్థిరరాశి గుర్తులేని అక్షరం * స్థిరరాశి salt,
    గుర్తులేని దీర్ఘ దీర్ఘ opslimit, size_t memlimit, పూర్ణాంకం alg)
    = ఖాళీ;
స్థిర శూన్య (*dll_randombytes_buf)(శూన్య * స్థిరరాశి buf, స్థిరరాశి size_t కొలత);
స్థిర uint32_t (*dll_randombytes_random)(శూన్య);

స్థిర నిర్మాణం {
    స్థిరరాశి అక్షరం *name;
    SODIUM_PROC *ptr;
} sodium_funcname_పట్టిక[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_నెట్టు},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_నెట్టు},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {ఖాళీ, ఖాళీ}
};

    స్థిర పూర్ణాంకం
sodium_runtime_link_init(పూర్ణాంకం verbose)
{
    స్థిర HINSTANCE hsodium = ఖాళీ;
    స్థిరరాశి అక్షరం *libname = DYNAMIC_SODIUM_DLL;
    పూర్ణాంకం i;

    ఒకవేళ (hsodium != ఖాళీ)
	తిరిగి OK;

    hsodium = ఎక్కించు_dll(libname);
    ఒకవేళ (hsodium == ఖాళీ)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_str_str), libname, ఎక్కించు_dll_error());
	తిరిగి FAIL;
    }

    కొరకు (i = 0; sodium_funcname_పట్టిక[i].ptr; ++i)
    {
	ఒకవేళ ((*sodium_funcname_పట్టిక[i].ptr = చిహ్నం_from_dll(hsodium,
			sodium_funcname_పట్టిక[i].name)) == ఖాళీ)
	{
	    close_dll(hsodium);
	    hsodium = ఖాళీ;
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str), sodium_funcname_పట్టిక[i].name);
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
ఎక్కించు_sodium(శూన్య)
{
    ఒకవేళ (sodium_runtime_link_init(TRUE) == FAIL)
	తిరిగి -1;
    తిరిగి dll_sodium_init();
}
# endif

# ఒకవేళ defined(DYNAMIC_SODIUM)
    పూర్ణాంకం
sodium_enabled(పూర్ణాంకం verbose)
{
    తిరిగి sodium_runtime_link_init(verbose) == OK;
}
# endif
#అయితే_ముగింపు

#నిర్వచించు CRYPT_MAGIC_LEN	12	// cannot change
స్థిర స్థిరరాశి అక్షరం crypt_magic_తల[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    పూర్ణాంకం
crypt_పద్ధతి_nr_from_name(char_u *name)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < CRYPT_M_లెక్క; ++i)
	ఒకవేళ (STRCMP(name, cryptmethods[i].name) == 0)
	    తిరిగి i;
    తిరిగి 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    పూర్ణాంకం
crypt_పద్ధతి_nr_from_magic(అక్షరం *ptr, పూర్ణాంకం len)
{
    పూర్ణాంకం i;

    ఒకవేళ (len < CRYPT_MAGIC_LEN)
	తిరిగి -1;

    కొరకు (i = 0; i < CRYPT_M_లెక్క; i++)
	ఒకవేళ (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    తిరిగి i;

    i = (పూర్ణాంకం)STRLEN(crypt_magic_తల);
    ఒకవేళ (len >= i && memcmp(ptr, crypt_magic_తల, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_పద్ధతి));

    తిరిగి -1;
}

#నిర్వచితమైతే CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    పూర్ణాంకం
crypt_పనిచేస్తుంది_inplace(cryptstate_T *state)
{
    తిరిగి cryptmethods[state->పద్ధతి_nr].పనిచేస్తుంది_inplace;
}
#అయితే_ముగింపు

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    పూర్ణాంకం
crypt_get_పద్ధతి_nr(buf_T *buf)
{
    తిరిగి crypt_పద్ధతి_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    పూర్ణాంకం
crypt_పద్ధతి_is_sodium(పూర్ణాంకం పద్ధతి)
{
    తిరిగి పద్ధతి == CRYPT_M_SOD || పద్ధతి == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    పూర్ణాంకం
crypt_సంపూర్ణ_undofile(పూర్ణాంకం పద్ధతి_nr)
{
    తిరిగి cryptmethods[పద్ధతి_nr].సంపూర్ణ_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    పూర్ణాంకం
crypt_get_శీర్షిక_len(పూర్ణాంకం పద్ధతి_nr)
{
    తిరిగి CRYPT_MAGIC_LEN
	+ cryptmethods[పద్ధతి_nr].salt_len
	+ cryptmethods[పద్ధతి_nr].విత్తనం_len
	+ cryptmethods[పద్ధతి_nr].కూడు_len;
}


#ఒకవేళ defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    పూర్ణాంకం
crypt_get_max_శీర్షిక_len(శూన్య)
{
    పూర్ణాంకం i;
    పూర్ణాంకం max = 0;
    పూర్ణాంకం తాత్కాలిక = 0;

    కొరకు (i = 0; i < CRYPT_M_లెక్క; ++i)
    {
	తాత్కాలిక = crypt_get_శీర్షిక_len(i);
	ఒకవేళ (తాత్కాలిక > max)
	    max = తాత్కాలిక;
    }
    తిరిగి max;
}
#అయితే_ముగింపు

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    శూన్య
crypt_సమితి_cm_option(buf_T *buf, పూర్ణాంకం పద్ధతి_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[పద్ధతి_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    పూర్ణాంకం
crypt_self_test(శూన్య)
{
    పూర్ణాంకం పద్ధతి_nr = crypt_get_పద్ధతి_nr(curbuf);

    ఒకవేళ (cryptmethods[పద్ధతి_nr].self_test_fn == ఖాళీ)
	తిరిగి OK;
    తిరిగి cryptmethods[పద్ధతి_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    పూర్ణాంకం		పద్ధతి_nr,
    char_u	*కీలకం,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    ఒకవేళ (state == ఖాళీ)
	తిరిగి state;

    state->పద్ధతి_nr = పద్ధతి_nr;
    ఒకవేళ (cryptmethods[పద్ధతి_nr].init_fn(state, కీలకం, crypt_arg) == FAIL)
    {
	vim_free(state);
	తిరిగి ఖాళీ;
    }
    తిరిగి state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_శీర్షిక(
    పూర్ణాంకం		పద్ధతి_nr,
    char_u	*కీలకం,
    char_u	*శీర్షిక)
{
    crypt_arg_T arg;

    CLEAR_క్షేత్రం(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[పద్ధతి_nr].salt_len;
    arg.cat_విత్తనం_len = cryptmethods[పద్ధతి_nr].విత్తనం_len;
    arg.cat_కూడు_len = cryptmethods[పద్ధతి_nr].కూడు_len;
    ఒకవేళ (arg.cat_salt_len > 0)
	arg.cat_salt = శీర్షిక + CRYPT_MAGIC_LEN;
    ఒకవేళ (arg.cat_విత్తనం_len > 0)
	arg.cat_విత్తనం = శీర్షిక + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    ఒకవేళ (arg.cat_కూడు_len > 0)
	arg.cat_కూడు = శీర్షిక + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_విత్తనం_len;

    తిరిగి crypt_create(పద్ధతి_nr, కీలకం, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *కీలకం)
{
    పూర్ణాంకం		పద్ధతి_nr;
    పూర్ణాంకం		శీర్షిక_len;
    అక్షరం	magic_బఫరు[CRYPT_MAGIC_LEN];
    char_u	*బఫరు;
    cryptstate_T *state;

    ఒకవేళ (fread(magic_బఫరు, CRYPT_MAGIC_LEN, 1, fp) != 1)
	తిరిగి ఖాళీ;
    పద్ధతి_nr = crypt_పద్ధతి_nr_from_magic(magic_బఫరు, CRYPT_MAGIC_LEN);
    ఒకవేళ (పద్ధతి_nr < 0)
	తిరిగి ఖాళీ;

    శీర్షిక_len = crypt_get_శీర్షిక_len(పద్ధతి_nr);
    ఒకవేళ ((బఫరు = alloc(శీర్షిక_len)) == ఖాళీ)
	తిరిగి ఖాళీ;
    mch_memmove(బఫరు, magic_బఫరు, CRYPT_MAGIC_LEN);
    ఒకవేళ (శీర్షిక_len > CRYPT_MAGIC_LEN
	    && fread(బఫరు + CRYPT_MAGIC_LEN,
				    శీర్షిక_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(బఫరు);
	తిరిగి ఖాళీ;
    }

    state = crypt_create_from_శీర్షిక(పద్ధతి_nr, కీలకం, బఫరు);
    vim_free(బఫరు);
    తిరిగి state;
}

/*
 * Allocate a cryptstate_T for writing and initialize it with "key".
 * Allocates and fills in the header and stores it in "header", setting
 * "header_len".  The header may include salt and seed, depending on
 * cryptmethod.  Caller must free header.
 * Returns the state or NULL on failure.
 */
    cryptstate_T *
crypt_create_for_writing(
    పూర్ణాంకం	    పద్ధతి_nr,
    char_u  *కీలకం,
    char_u  **శీర్షిక,
    పూర్ణాంకం	    *శీర్షిక_len)
{
    పూర్ణాంకం	    len = crypt_get_శీర్షిక_len(పద్ధతి_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_క్షేత్రం(arg);
    arg.cat_salt_len = cryptmethods[పద్ధతి_nr].salt_len;
    arg.cat_విత్తనం_len = cryptmethods[పద్ధతి_nr].విత్తనం_len;
    arg.cat_కూడు_len  = cryptmethods[పద్ధతి_nr].కూడు_len;
    arg.cat_init_from_file = FALSE;

    *శీర్షిక_len = len;
    *శీర్షిక = alloc(len);
    ఒకవేళ (*శీర్షిక == ఖాళీ)
	తిరిగి ఖాళీ;

    mch_memmove(*శీర్షిక, cryptmethods[పద్ధతి_nr].magic, CRYPT_MAGIC_LEN);
    ఒకవేళ (arg.cat_salt_len > 0 || arg.cat_విత్తనం_len > 0 || arg.cat_కూడు_len > 0)
    {
	ఒకవేళ (arg.cat_salt_len > 0)
	    arg.cat_salt = *శీర్షిక + CRYPT_MAGIC_LEN;
	ఒకవేళ (arg.cat_విత్తనం_len > 0)
	    arg.cat_విత్తనం = *శీర్షిక + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	ఒకవేళ (arg.cat_కూడు_len > 0)
	    arg.cat_కూడు = *శీర్షిక + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_విత్తనం_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#నిర్వచితమైతే FEAT_SODIUM
	ఒకవేళ (sodium_init() >= 0)
	{
	    ఒకవేళ (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    ఒకవేళ (arg.cat_విత్తనం_len > 0)
		randombytes_buf(arg.cat_విత్తనం, arg.cat_విత్తనం_len);
	}
	లేకపోతే
#అయితే_ముగింపు
	    sha2_విత్తనం(arg.cat_salt, arg.cat_salt_len, arg.cat_విత్తనం, arg.cat_విత్తనం_len);
    }
    state = crypt_create(పద్ధతి_nr, కీలకం, &arg);
    ఒకవేళ (state == ఖాళీ)
	VIM_CLEAR(*శీర్షిక);
    తిరిగి state;
}

/*
 * Free the crypt state.
 */
    శూన్య
crypt_free_state(cryptstate_T *state)
{
#నిర్వచితమైతే FEAT_SODIUM
    ఒకవేళ (crypt_పద్ధతి_is_sodium(state->పద్ధతి_nr))
    {
	sodium_munlock(((sodium_state_T *)state->పద్ధతి_state)->కీలకం,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->పద్ధతి_state, పరిమాణం(sodium_state_T));
	sodium_free(state->పద్ధతి_state);
    }
    లేకపోతే
#అయితే_ముగింపు
	vim_free(state->పద్ధతి_state);
    vim_free(state);
}

#నిర్వచితమైతే CRYPT_NOT_INPLACE
/*
 * Encode "from[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    దీర్ఘ
crypt_encode_alloc(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	**newptr,
    పూర్ణాంకం		చివరి)
{
    cryptmethod_T *పద్ధతి = &cryptmethods[state->పద్ధతి_nr];

    ఒకవేళ (పద్ధతి->encode_బఫరు_fn != ఖాళీ)
	// Has buffer function, pass through.
	తిరిగి పద్ధతి->encode_బఫరు_fn(state, from, len, newptr, చివరి);
    ఒకవేళ (len == 0)
	// Not buffering, just return EOF.
	తిరిగి (దీర్ఘ)len;

    *newptr = alloc(len + 50);
    ఒకవేళ (*newptr == ఖాళీ)
	తిరిగి -1;
    పద్ధతి->encode_fn(state, from, len, *newptr, చివరి);
    తిరిగి (దీర్ఘ)len;
}

/*
 * Decrypt "ptr[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    దీర్ఘ
crypt_decode_alloc(
    cryptstate_T *state,
    char_u	*ptr,
    దీర్ఘ	len,
    char_u      **newptr,
    పూర్ణాంకం		చివరి)
{
    cryptmethod_T *పద్ధతి = &cryptmethods[state->పద్ధతి_nr];

    ఒకవేళ (పద్ధతి->decode_బఫరు_fn != ఖాళీ)
	// Has buffer function, pass through.
	తిరిగి పద్ధతి->decode_బఫరు_fn(state, ptr, len, newptr, చివరి);

    ఒకవేళ (len == 0)
	// Not buffering, just return EOF.
	తిరిగి len;

    *newptr = alloc(len);
    ఒకవేళ (*newptr == ఖాళీ)
	తిరిగి -1;
    పద్ధతి->decode_fn(state, ptr, len, *newptr, చివరి);
    తిరిగి len;
}
#అయితే_ముగింపు

/*
 * Encrypting "from[len]" into "to[len]".
 */
    శూన్య
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*గమ్యం,
    పూర్ణాంకం		చివరి)
{
    cryptmethods[state->పద్ధతి_nr].encode_fn(state, from, len, గమ్యం, చివరి);
}

#ఒకవేళ 0  //వాడని
/*
 * decrypting "from[len]" into "to[len]".
 */
    శూన్య
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*గమ్యం,
    పూర్ణాంకం		చివరి)
{
    cryptmethods[state->పద్ధతి_nr].decode_fn(state, from, len, గమ్యం, చివరి);
}
#అయితే_ముగింపు

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    శూన్య
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    పూర్ణాంకం		చివరి)
{
    cryptmethods[state->పద్ధతి_nr].encode_inplace_fn(state, buf, len,
								    buf, చివరి);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    శూన్య
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    పూర్ణాంకం		చివరి)
{
    cryptmethods[state->పద్ధతి_nr].decode_inplace_fn(state, buf, len,
								    buf, చివరి);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    శూన్య
crypt_free_కీలకం(char_u *కీలకం)
{
    // Create a safe memset which cannot be optimized away by compiler
    స్థిర శూన్య *(* అస్థిర vim_memset_సురక్షిత)(శూన్య *s, పూర్ణాంకం c, size_t n) =
	memset;
    ఒకవేళ (కీలకం != ఖాళీ)
    {
#నిర్వచితమైతే FEAT_SODIUM
	ఒకవేళ (sodium_init() >= 0)
	    sodium_memzero(కీలకం, STRLEN(కీలకం));
	లేకపోతే
#అయితే_ముగింపు
	    vim_memset_సురక్షిత(కీలకం, 0, STRLEN(కీలకం));
	vim_free(కీలకం);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    శూన్య
crypt_check_పద్ధతి(పూర్ణాంకం పద్ధతి)
{
    ఒకవేళ (పద్ధతి < CRYPT_M_BF2 || పద్ధతి == CRYPT_M_SOD)
    {
	సందేశం_scroll = TRUE;
	సందేశం(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    శూన్య
crypt_check_swapfile_curbuf(శూన్య)
{
#నిర్వచితమైతే FEAT_SODIUM
    పూర్ణాంకం పద్ధతి = crypt_get_పద్ధతి_nr(curbuf);
    ఒకవేళ (crypt_పద్ధతి_is_sodium(పద్ధతి))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	సమితి_option_విలువ_give_err((char_u *)"swf", 0, ఖాళీ, OPT_LOCAL);
	సందేశం_scroll = TRUE;
	సందేశం(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#అయితే_ముగింపు
}

    శూన్య
crypt_check_current_పద్ధతి(శూన్య)
{
    crypt_check_పద్ధతి(crypt_get_పద్ధతి_nr(curbuf));
}

/*
 * Ask the user for a crypt key.
 * When "store" is TRUE, the new key is stored in the 'key' option, and the
 * 'key' option value is returned: Don't free it.
 * When "store" is FALSE, the typed key is returned in allocated memory.
 * Returns NULL on failure.
 */
    char_u *
crypt_get_కీలకం(
    పూర్ణాంకం		store,
    పూర్ణాంకం		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = ఖాళీ;
    పూర్ణాంకం		విడత;

    కొరకు (విడత = 0; ; ++విడత)
    {
	cmdline_నక్షత్రం = TRUE;
	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
	p1 = getcmdline_prompt(NUL, విడత == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		ఖాళీ);
	cmdline_నక్షత్రం = FALSE;

	ఒకవేళ (p1 == ఖాళీ)
	    ఆపు;

	ఒకవేళ (విడత == twice)
	{
	    ఒకవేళ (p2 != ఖాళీ && STRCMP(p1, p2) != 0)
	    {
		సందేశం(_("Keys don't match!"));
		crypt_free_కీలకం(p1);
		crypt_free_కీలకం(p2);
		p2 = ఖాళీ;
		విడత = -1;		// do it again
		కొనసాగు;
	    }

	    ఒకవేళ (store)
	    {
		సమితి_option_విలువ_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_కీలకం(p1);
		p1 = curbuf->b_p_కీలకం;
		crypt_check_swapfile_curbuf();
	    }
	    ఆపు;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    ఒకవేళ (!crypt_పద్ధతి_is_sodium(crypt_get_పద్ధతి_nr(curbuf)))
    {
	ఒకవేళ (సందేశం_didout)
	    సందేశం_putchar('\n');
	need_wait_return = FALSE;
	సందేశం_didout = FALSE;
    }

    crypt_free_కీలకం(p2);
    తిరిగి p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    శూన్య
crypt_append_సందేశం(
    buf_T *buf)
{
    ఒకవేళ (crypt_get_పద్ధతి_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    లేకపోతే
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    స్థిర పూర్ణాంకం
crypt_sodium_init_(
    cryptstate_T	*state వాడని,
    char_u		*కీలకం వాడని,
    crypt_arg_T		*arg వాడని)
{
#నిర్వచితమైతే FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    గుర్తులేని అక్షరం	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    పూర్ణాంకం			retval = 0;
    గుర్తులేని దీర్ఘ దీర్ఘ	opslimit;
    గుర్తులేని దీర్ఘ దీర్ఘ	memlimit;
    పూర్ణాంకం			alg;

    ఒకవేళ (sodium_init() < 0)
	తిరిగి FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(పరిమాణం(sodium_state_T));
    sodium_memzero(sd_state, పరిమాణం(sodium_state_T));

    ఒకవేళ ((state->పద్ధతి_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->పద్ధతి_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# ఒకవేళ 0
	// For testing
	ఒకవేళ (state->పద్ధతి_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	ఒకవేళ (crypto_pwhash(dkey, పరిమాణం(dkey), (స్థిరరాశి అక్షరం *)కీలకం, STRLEN(కీలకం),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    తిరిగి FAIL;
	}
	జ్ఞాపకం_నకలు(sd_state->కీలకం, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->కీలకం, crypto_box_SEEDBYTES);
	retval += sodium_mlock(కీలకం, STRLEN(కీలకం));

	ఒకవేళ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    తిరిగి FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	ఒకవేళ (state->పద్ధతి_nr == CRYPT_M_SOD2 && arg->cat_కూడు != ఖాళీ)
	{
	    char_u	బఫరు[20];
	    char_u	*p = బఫరు;
	    vim_memset(బఫరు, 0, 20);

	    crypt_long_long_గమ్యం_char(opslimit, p);
	    p += పరిమాణం(opslimit);

	    crypt_long_long_గమ్యం_char(memlimit, p);
	    p += పరిమాణం(memlimit);

	    crypt_int_గమ్యం_char(alg, p);
	    జ్ఞాపకం_నకలు(arg->cat_కూడు, బఫరు, పరిమాణం(opslimit) + పరిమాణం(memlimit) + పరిమాణం(alg));
	}
    }
    లేకపోతే
    {
	char_u	బఫరు[20];
	char_u	*p = బఫరు;
	vim_memset(బఫరు, 0, 20);
	పూర్ణాంకం	కొలత = పరిమాణం(opslimit) +
	    పరిమాణం(memlimit) + పరిమాణం(alg);

	// Reading parameters from file
	ఒకవేళ (arg->cat_కూడు_len < కొలత)
	{
	    sodium_free(sd_state);
	    తిరిగి FAIL;
	}

	// derive the key from the file header
	జ్ఞాపకం_నకలు(p, arg->cat_కూడు, కొలత);
	arg->cat_కూడు += కొలత;

	opslimit = crypt_char_గమ్యం_long_long(p);
	p += పరిమాణం(opslimit);
	memlimit = crypt_char_గమ్యం_long_long(p);
	p += పరిమాణం(memlimit);
	alg = crypt_char_గమ్యం_int(p);
	p += పరిమాణం(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	ఒకవేళ (crypto_pwhash(dkey, పరిమాణం(dkey), (స్థిరరాశి అక్షరం *)కీలకం, STRLEN(కీలకం),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    తిరిగి FAIL;
	}
	జ్ఞాపకం_నకలు(sd_state->కీలకం, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->కీలకం, crypto_box_SEEDBYTES);
	retval += sodium_mlock(కీలకం, STRLEN(కీలకం));

	ఒకవేళ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    తిరిగి FAIL;
	}
    }
    sd_state->లెక్క = 0;
    state->పద్ధతి_state = sd_state;

    తిరిగి OK;
#లేకపోతే
    emsg(_(e_libsodium_not_built_లోపల));
    తిరిగి FAIL;
#అయితే_ముగింపు
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#ఒకవేళ 0  // Currently unused
    శూన్య
crypt_sodium_encode(
    cryptstate_T *state వాడని,
    char_u	*from వాడని,
    size_t	len వాడని,
    char_u	*గమ్యం వాడని,
    పూర్ణాంకం		చివరి వాడని)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->పద్ధతి_state;
    గుర్తులేని అక్షరం  ట్యాగ్ = చివరి
			? crypto_secretstream_xchacha20poly1305_ట్యాగ్_FINAL  : 0;

    ఒకవేళ (sod_st->లెక్క == 0)
    {
	ఒకవేళ (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_శీర్షిక));
	    తిరిగి;
	}
	crypto_secretstream_xchacha20poly1305_init_నెట్టు(&sod_st->state,
							      గమ్యం, sod_st->కీలకం);
	గమ్యం += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ఒకవేళ (sod_st->లెక్క && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_బఫరు));
	తిరిగి;
    }

    crypto_secretstream_xchacha20poly1305_నెట్టు(&sod_st->state, గమ్యం, ఖాళీ,
						      from, len, ఖాళీ, 0, ట్యాగ్);

    sod_st->లెక్క++;
# endif
}
#అయితే_ముగింపు

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#ఒకవేళ 0  // Currently unused
    శూన్య
crypt_sodium_decode(
    cryptstate_T *state వాడని,
    char_u	*from వాడని,
    size_t	len వాడని,
    char_u	*గమ్యం వాడని,
    పూర్ణాంకం		చివరి వాడని)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->పద్ధతి_state;
    గుర్తులేని అక్షరం  ట్యాగ్;
    గుర్తులేని దీర్ఘ దీర్ఘ buf_len;
    char_u *p1 = from;
    char_u *p2 = గమ్యం;
    char_u *buf_బహిర్గతం;

    ఒకవేళ (sod_st->లెక్క == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_శీర్షిక));
	తిరిగి;
    }

    buf_బహిర్గతం = (char_u *)alloc(len);

    ఒకవేళ (buf_బహిర్గతం == ఖాళీ)
    {
	emsg(_(e_libsodium_cannot_allocate_బఫరు));
	తిరిగి;
    }
    ఒకవేళ (sod_st->లెక్క == 0)
    {
	ఒకవేళ (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->కీలకం) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_శీర్షిక_incomplete));
	    వెళ్ళు fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	ఒకవేళ (p1 == p2)
	    గమ్యం += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ఒకవేళ (sod_st->లెక్క && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_బఫరు));
	వెళ్ళు fail;
    }
    ఒకవేళ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_బహిర్గతం, &buf_len, &ట్యాగ్, from, len, ఖాళీ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	వెళ్ళు fail;
    }
    sod_st->లెక్క++;

    ఒకవేళ (ట్యాగ్ == crypto_secretstream_xchacha20poly1305_ట్యాగ్_FINAL && !చివరి)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	వెళ్ళు fail;
    }
    ఒకవేళ (p1 == p2)
	mch_memmove(p2, buf_బహిర్గతం, buf_len);

fail:
    vim_free(buf_బహిర్గతం);
# endif
}
#అయితే_ముగింపు

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    స్థిర దీర్ఘ
crypt_sodium_బఫరు_encode(
    cryptstate_T *state వాడని,
    char_u	*from వాడని,
    size_t	len వాడని,
    char_u	**buf_బహిర్గతం వాడని,
    పూర్ణాంకం		చివరి వాడని)
{
#నిర్వచితమైతే FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    గుర్తులేని దీర్ఘ దీర్ఘ	బహిర్గతం_len;
    char_u		*ptr;
    గుర్తులేని అక్షరం	ట్యాగ్ = చివరి
			? crypto_secretstream_xchacha20poly1305_ట్యాగ్_FINAL  : 0;
    పూర్ణాంకం			నిడివి;
    sodium_state_T	*sod_st = state->పద్ధతి_state;
    పూర్ణాంకం			మొదటి = (sod_st->లెక్క == 0);

    నిడివి = (పూర్ణాంకం)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (మొదటి ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_బహిర్గతం = alloc_clear(నిడివి);
    ఒకవేళ (*buf_బహిర్గతం == ఖాళీ)
    {
	emsg(_(e_libsodium_cannot_allocate_బఫరు));
	తిరిగి -1;
    }
    ptr = *buf_బహిర్గతం;

    ఒకవేళ (మొదటి)
    {
	crypto_secretstream_xchacha20poly1305_init_నెట్టు(&sod_st->state,
		ptr, sod_st->కీలకం);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_నెట్టు(&sod_st->state, ptr,
	    &బహిర్గతం_len, from, len, ఖాళీ, 0, ట్యాగ్);

    sod_st->లెక్క++;
    తిరిగి బహిర్గతం_len + (మొదటి
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#లేకపోతే
    తిరిగి -1;
#అయితే_ముగింపు
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    స్థిర దీర్ఘ
crypt_sodium_బఫరు_decode(
    cryptstate_T *state వాడని,
    char_u	*from వాడని,
    size_t	len వాడని,
    char_u	**buf_బహిర్గతం వాడని,
    పూర్ణాంకం		చివరి వాడని)
{
#నిర్వచితమైతే FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->పద్ధతి_state;
    గుర్తులేని అక్షరం  ట్యాగ్;
    గుర్తులేని దీర్ఘ దీర్ఘ బహిర్గతం_len;

    ఒకవేళ (sod_st->లెక్క == 0
	    && state->పద్ధతి_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].కూడు_len;

    *buf_బహిర్గతం = alloc_clear(len);
    ఒకవేళ (*buf_బహిర్గతం == ఖాళీ)
    {
	emsg(_(e_libsodium_cannot_allocate_బఫరు));
	తిరిగి -1;
    }

    ఒకవేళ (sod_st->లెక్క == 0)
    {
	ఒకవేళ (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->కీలకం) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_శీర్షిక_incomplete));
	    తిరిగి -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->లెక్క++;
    }
    ఒకవేళ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_బహిర్గతం, &బహిర్గతం_len, &ట్యాగ్, from, len, ఖాళీ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	తిరిగి -1;
    }

    ఒకవేళ (ట్యాగ్ == crypto_secretstream_xchacha20poly1305_ట్యాగ్_FINAL && !చివరి)
	emsg(_(e_libsodium_decryption_failed_premature));
    తిరిగి (దీర్ఘ) బహిర్గతం_len;
#లేకపోతే
    తిరిగి -1;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_SODIUM)
    శూన్య
crypt_sodium_తాళం_కీలకం(char_u *కీలకం)
{
    ఒకవేళ (sodium_init() >= 0)
	sodium_mlock(కీలకం, STRLEN(కీలకం));
}

    పూర్ణాంకం
crypt_sodium_munlock(శూన్య *స్థిరరాశి addr, స్థిరరాశి size_t len)
{
    తిరిగి sodium_munlock(addr, len);
}

    శూన్య
crypt_sodium_randombytes_buf(శూన్య *స్థిరరాశి buf, స్థిరరాశి size_t కొలత)
{
    randombytes_buf(buf, కొలత);
}

    పూర్ణాంకం
crypt_sodium_init(శూన్య)
{
    తిరిగి sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(శూన్య)
{
    తిరిగి randombytes_random();
}

# ఒకవేళ defined(FEAT_EVAL)
    స్థిర శూన్య
crypt_sodium_report_hash_params(
	గుర్తులేని దీర్ఘ దీర్ఘ opslimit,
	గుర్తులేని దీర్ఘ దీర్ఘ ops_def,
	size_t memlimit,
	size_t mem_def,
	పూర్ణాంకం alg,
	పూర్ణాంకం alg_def)
{
    ఒకవేళ (p_verbose > 0)
    {
	verbose_enter();
	ఒకవేళ (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	లేకపోతే
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	ఒకవేళ (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (గుర్తులేని దీర్ఘ)memlimit);
	లేకపోతే
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (గుర్తులేని దీర్ఘ)memlimit);
	ఒకవేళ (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	లేకపోతే
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    స్థిర శూన్య
crypt_long_long_గమ్యం_char(దీర్ఘ దీర్ఘ n, char_u *s)
{
    పూర్ణాంకం i;
    కొరకు (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (గుర్తులేని దీర్ఘ దీర్ఘ)n >> 8;
    }
}

    స్థిర శూన్య
crypt_int_గమ్యం_char(పూర్ణాంకం n, char_u *s)
{
    పూర్ణాంకం i;
    కొరకు (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (గుర్తులేని దీర్ఘ దీర్ఘ)n >> 8;
    }
}

    స్థిర దీర్ఘ దీర్ఘ
crypt_char_గమ్యం_long_long(char_u *s)
{
    గుర్తులేని దీర్ఘ దీర్ఘ    retval = 0;
    పూర్ణాంకం i;
    కొరకు (i = 7; i >= 0; i--)
    {
	ఒకవేళ (i == 7)
	    retval = s[i];
	లేకపోతే
	    retval |= s[i];
	ఒకవేళ (i > 0)
	    retval <<= 8;
    }
    తిరిగి retval;
}

    స్థిర పూర్ణాంకం
crypt_char_గమ్యం_int(char_u *s)
{
    పూర్ణాంకం retval = 0;
    పూర్ణాంకం i;

    కొరకు (i = 3; i >= 0; i--)
    {
	ఒకవేళ (i == 3)
	    retval = s[i];
	లేకపోతే
	    retval |= s[i];
	ఒకవేళ (i > 0)
	    retval <<= 8;
    }
    తిరిగి retval;
}
#అయితే_ముగింపు

#అయితే_ముగింపు // FEAT_CRYPT
