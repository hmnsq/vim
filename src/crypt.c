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
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_CRYPT)
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ	*name;		// encryption name as used in 'cryptmethod'
    ಅಕ್ಷರ	*magic;		// magic bytes stored in file header
    ಪೂರ್ಣಾಂಕ		salt_len;	// length of salt, or 0 when not using salt
    ಪೂರ್ಣಾಂಕ		ಬೀಜ_len;	// length of seed, or 0 when not using seed
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_len;	// additional length in the header needed for storing
				// custom data
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CRYPT_NOT_INPLACE
    ಪೂರ್ಣಾಂಕ		ಕೆಲಸಮಾಡುತ್ತದೆ_inplace;	// encryption/decryption can be done in-place
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ಸಂಪೂರ್ಣ_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    ಪೂರ್ಣಾಂಕ (*self_test_fn)(ಶೂನ್ಯ);

    // Function pointer for initializing encryption/decryption.
    ಪೂರ್ಣಾಂಕ (* init_fn)(cryptstate_T *state, char_u *ಕೀಲಿ, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    ಶೂನ್ಯ (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *ಗಮ್ಯ, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ);
    ಶೂನ್ಯ (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *ಗಮ್ಯ, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    ದೀರ್ಘ (*encode_ಬಫರ್_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ);
    ದೀರ್ಘ (*decode_ಬಫರ್_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    ಶೂನ್ಯ (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ);
    ಶೂನ್ಯ (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ);
} cryptmethod_T;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ crypt_sodium_init_(cryptstate_T *state, char_u *ಕೀಲಿ, crypt_arg_T *arg);
ಸ್ಥಿರ ದೀರ್ಘ crypt_sodium_ಬಫರ್_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_ಹೊರಹೋಗು, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ);
ಸ್ಥಿರ ದೀರ್ಘ crypt_sodium_ಬಫರ್_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_ಹೊರಹೋಗು, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ);
#ಆದರೆ defined(FEAT_SODIUM)
ಸ್ಥಿರ ಶೂನ್ಯ crypt_long_long_ಗಮ್ಯ_char(ದೀರ್ಘ ದೀರ್ಘ n, char_u *s);
ಸ್ಥಿರ ಶೂನ್ಯ crypt_int_ಗಮ್ಯ_char(ಪೂರ್ಣಾಂಕ n, char_u *s);
ಸ್ಥಿರ ದೀರ್ಘ ದೀರ್ಘ crypt_char_ಗಮ್ಯ_long_long(char_u *s);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ crypt_char_ಗಮ್ಯ_int(char_u *s);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ಸ್ಥಿರ ಶೂನ್ಯ crypt_sodium_report_hash_params(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ opslimit, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ ops_def, size_t memlimit, size_t mem_def, ಪೂರ್ಣಾಂಕ alg, ಪೂರ್ಣಾಂಕ alg_def);
#ಆದರೆ_ಅಂತ್ಯ

// index is method_nr of cryptstate_T, CRYPT_M_*
ಸ್ಥಿರ cryptmethod_T cryptmethods[CRYPT_M_ಎಣಿಕೆ] = {
    // PK_Zip; very weak
    {
	"zip",
	"VimCrypt~01!",
	0,
	0,
	0,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CRYPT_NOT_INPLACE
	TRUE,
#ಆದರೆ_ಅಂತ್ಯ
	FALSE,
	ಖಾಲಿ,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	ಖಾಲಿ, ಖಾಲಿ,
	crypt_zip_encode, crypt_zip_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; implementation issues.
    {
	"blowfish",
	"VimCrypt~02!",
	8,
	8,
	0,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CRYPT_NOT_INPLACE
	TRUE,
#ಆದರೆ_ಅಂತ್ಯ
	FALSE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	ಖಾಲಿ, ಖಾಲಿ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; fixed.
    {
	"blowfish2",
	"VimCrypt~03!",
	8,
	8,
	0,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CRYPT_NOT_INPLACE
	TRUE,
#ಆದರೆ_ಅಂತ್ಯ
	TRUE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	ಖಾಲಿ, ಖಾಲಿ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ಇಲ್ಲದಿದ್ದರೆ
	16,
#ಆದರೆ_ಅಂತ್ಯ
	8,
	0,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CRYPT_NOT_INPLACE
	FALSE,
#ಆದರೆ_ಅಂತ್ಯ
	FALSE,
	ಖಾಲಿ,
	crypt_sodium_init_,
	ಖಾಲಿ, ಖಾಲಿ,
	crypt_sodium_ಬಫರ್_encode, crypt_sodium_ಬಫರ್_decode,
	ಖಾಲಿ, ಖಾಲಿ,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ಇಲ್ಲದಿದ್ದರೆ
	16,
#ಆದರೆ_ಅಂತ್ಯ
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CRYPT_NOT_INPLACE
	FALSE,
#ಆದರೆ_ಅಂತ್ಯ
	FALSE,
	ಖಾಲಿ,
	crypt_sodium_init_,
	ಖಾಲಿ, ಖಾಲಿ,
	crypt_sodium_ಬಫರ್_encode, crypt_sodium_ಬಫರ್_decode,
	ಖಾಲಿ, ಖಾಲಿ,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#ಆದರೆ defined(FEAT_SODIUM)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    size_t	    ಎಣಿಕೆ_2;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ   ಕೀಲಿ[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define ತುಂಬು_dll vim_ತುಂಬು_Lib
#   define ಚಿಹ್ನೆ_from_dll Get_Proc_ವಿಳಾಸ
#   define close_dll FreeLibrary
#   define ತುಂಬು_dll_error GetWin32Error
#  ಇಲ್ಲದಿದ್ದರೆ
#   error Dynamic loading ದ libsodium is not supported ಗಾಗಿ now.
//#   define HINSTANCE void*
//#   define SODIUM_PROC void*
//#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
//#   define symbol_from_dll dlsym
//#   define close_dll dlclose
//#   define load_dll_error dlerror
#  endif

#  define sodium_init	    ತುಂಬು_sodium
#  define sodium_free	    dll_sodium_free
#  define sodium_malloc	    dll_sodium_malloc
#  define sodium_memzero    dll_sodium_memzero
#  define sodium_mlock	    dll_sodium_mlock
#  define sodium_munlock    dll_sodium_munlock
#  define crypto_secretstream_xchacha20poly1305_init_ತಳ್ಳು \
    dll_crypto_secretstream_xchacha20poly1305_init_ತಳ್ಳು
#  define crypto_secretstream_xchacha20poly1305_ತಳ್ಳು \
    dll_crypto_secretstream_xchacha20poly1305_ತಳ್ಳು
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_sodium_init)(ಶೂನ್ಯ) = ಖಾಲಿ;
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_sodium_free)(ಶೂನ್ಯ *) = ಖಾಲಿ;
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_sodium_malloc)(ಸ್ಥಿರಾಂಕ size_t) = ಖಾಲಿ;
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_sodium_memzero)(ಶೂನ್ಯ * ಸ್ಥಿರಾಂಕ, ಸ್ಥಿರಾಂಕ size_t) = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_sodium_mlock)(ಶೂನ್ಯ * ಸ್ಥಿರಾಂಕ, ಸ್ಥಿರಾಂಕ size_t) = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_sodium_munlock)(ಶೂನ್ಯ * ಸ್ಥಿರಾಂಕ, ಸ್ಥಿರಾಂಕ size_t) = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_crypto_secretstream_xchacha20poly1305_init_ತಳ್ಳು)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ [],
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ []) = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_crypto_secretstream_xchacha20poly1305_ತಳ್ಳು)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *c, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ *clen_p,
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *m, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ mlen,
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ad, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ adlen, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಟ್ಯಾಗ್)
    = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ [],
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ []) = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *m, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ *mlen_p, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಟ್ಯಾಗ್_p,
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *c, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ clen,
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ad, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ adlen) = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_crypto_pwhash)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಹೊರಹೋಗು,
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ outlen,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ passwd, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ passwdlen,
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ salt,
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ opslimit, size_t memlimit, ಪೂರ್ಣಾಂಕ alg)
    = ಖಾಲಿ;
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_randombytes_buf)(ಶೂನ್ಯ * ಸ್ಥಿರಾಂಕ buf, ಸ್ಥಿರಾಂಕ size_t ಗಾತ್ರ_2);
ಸ್ಥಿರ uint32_t (*dll_randombytes_random)(ಶೂನ್ಯ);

ಸ್ಥಿರ ರಚನೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
    SODIUM_PROC *ptr;
} sodium_funcname_ಕೋಷ್ಟಕ[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_ತಳ್ಳು},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_ತಳ್ಳು},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {ಖಾಲಿ, ಖಾಲಿ}
};

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sodium_runtime_link_init(ಪೂರ್ಣಾಂಕ verbose)
{
    ಸ್ಥಿರ HINSTANCE hsodium = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libname = DYNAMIC_SODIUM_DLL;
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (hsodium != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    hsodium = ತುಂಬು_dll(libname);
    ಆದರೆ (hsodium == ಖಾಲಿ)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_str_str), libname, ತುಂಬು_dll_error());
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಗಾಗಿ (i = 0; sodium_funcname_ಕೋಷ್ಟಕ[i].ptr; ++i)
    {
	ಆದರೆ ((*sodium_funcname_ಕೋಷ್ಟಕ[i].ptr = ಚಿಹ್ನೆ_from_dll(hsodium,
			sodium_funcname_ಕೋಷ್ಟಕ[i].name)) == ಖಾಲಿ)
	{
	    close_dll(hsodium);
	    hsodium = ಖಾಲಿ;
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str), sodium_funcname_ಕೋಷ್ಟಕ[i].name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ತುಂಬು_sodium(ಶೂನ್ಯ)
{
    ಆದರೆ (sodium_runtime_link_init(TRUE) == FAIL)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ dll_sodium_init();
}
# endif

# ಆದರೆ defined(DYNAMIC_SODIUM)
    ಪೂರ್ಣಾಂಕ
sodium_enabled(ಪೂರ್ಣಾಂಕ verbose)
{
    ಹಿಂತಿರುಗಿ sodium_runtime_link_init(verbose) == OK;
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CRYPT_MAGIC_LEN	12	// cannot change
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ crypt_magic_ತಲೆ[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    ಪೂರ್ಣಾಂಕ
crypt_ವಿಧಾನ_nr_from_name(char_u *name)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < CRYPT_M_ಎಣಿಕೆ; ++i)
	ಆದರೆ (STRCMP(name, cryptmethods[i].name) == 0)
	    ಹಿಂತಿರುಗಿ i;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    ಪೂರ್ಣಾಂಕ
crypt_ವಿಧಾನ_nr_from_magic(ಅಕ್ಷರ *ptr, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (len < CRYPT_MAGIC_LEN)
	ಹಿಂತಿರುಗಿ -1;

    ಗಾಗಿ (i = 0; i < CRYPT_M_ಎಣಿಕೆ; i++)
	ಆದರೆ (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    ಹಿಂತಿರುಗಿ i;

    i = (ಪೂರ್ಣಾಂಕ)STRLEN(crypt_magic_ತಲೆ);
    ಆದರೆ (len >= i && memcmp(ptr, crypt_magic_ತಲೆ, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_ವಿಧಾನ));

    ಹಿಂತಿರುಗಿ -1;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    ಪೂರ್ಣಾಂಕ
crypt_ಕೆಲಸಮಾಡುತ್ತದೆ_inplace(cryptstate_T *state)
{
    ಹಿಂತಿರುಗಿ cryptmethods[state->ವಿಧಾನ_nr].ಕೆಲಸಮಾಡುತ್ತದೆ_inplace;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    ಪೂರ್ಣಾಂಕ
crypt_get_ವಿಧಾನ_nr(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ crypt_ವಿಧಾನ_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    ಪೂರ್ಣಾಂಕ
crypt_ವಿಧಾನ_is_sodium(ಪೂರ್ಣಾಂಕ ವಿಧಾನ)
{
    ಹಿಂತಿರುಗಿ ವಿಧಾನ == CRYPT_M_SOD || ವಿಧಾನ == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    ಪೂರ್ಣಾಂಕ
crypt_ಸಂಪೂರ್ಣ_undofile(ಪೂರ್ಣಾಂಕ ವಿಧಾನ_nr)
{
    ಹಿಂತಿರುಗಿ cryptmethods[ವಿಧಾನ_nr].ಸಂಪೂರ್ಣ_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    ಪೂರ್ಣಾಂಕ
crypt_get_ಶೀರ್ಷಿಕೆ_len(ಪೂರ್ಣಾಂಕ ವಿಧಾನ_nr)
{
    ಹಿಂತಿರುಗಿ CRYPT_MAGIC_LEN
	+ cryptmethods[ವಿಧಾನ_nr].salt_len
	+ cryptmethods[ವಿಧಾನ_nr].ಬೀಜ_len
	+ cryptmethods[ವಿಧಾನ_nr].ಕೂಡಿಸು_len;
}


#ಆದರೆ defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    ಪೂರ್ಣಾಂಕ
crypt_get_max_ಶೀರ್ಷಿಕೆ_len(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ max = 0;
    ಪೂರ್ಣಾಂಕ ತಾತ್ಕಾಲಿಕ = 0;

    ಗಾಗಿ (i = 0; i < CRYPT_M_ಎಣಿಕೆ; ++i)
    {
	ತಾತ್ಕಾಲಿಕ = crypt_get_ಶೀರ್ಷಿಕೆ_len(i);
	ಆದರೆ (ತಾತ್ಕಾಲಿಕ > max)
	    max = ತಾತ್ಕಾಲಿಕ;
    }
    ಹಿಂತಿರುಗಿ max;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    ಶೂನ್ಯ
crypt_ಗಣ_cm_option(buf_T *buf, ಪೂರ್ಣಾಂಕ ವಿಧಾನ_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[ವಿಧಾನ_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    ಪೂರ್ಣಾಂಕ
crypt_self_test(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ವಿಧಾನ_nr = crypt_get_ವಿಧಾನ_nr(curbuf);

    ಆದರೆ (cryptmethods[ವಿಧಾನ_nr].self_test_fn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ cryptmethods[ವಿಧಾನ_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    ಪೂರ್ಣಾಂಕ		ವಿಧಾನ_nr,
    char_u	*ಕೀಲಿ,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    ಆದರೆ (state == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ state;

    state->ವಿಧಾನ_nr = ವಿಧಾನ_nr;
    ಆದರೆ (cryptmethods[ವಿಧಾನ_nr].init_fn(state, ಕೀಲಿ, crypt_arg) == FAIL)
    {
	vim_free(state);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_ಶೀರ್ಷಿಕೆ(
    ಪೂರ್ಣಾಂಕ		ವಿಧಾನ_nr,
    char_u	*ಕೀಲಿ,
    char_u	*ಶೀರ್ಷಿಕೆ)
{
    crypt_arg_T arg;

    CLEAR_ಕ್ಷೇತ್ರ(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[ವಿಧಾನ_nr].salt_len;
    arg.cat_ಬೀಜ_len = cryptmethods[ವಿಧಾನ_nr].ಬೀಜ_len;
    arg.cat_ಕೂಡಿಸು_len = cryptmethods[ವಿಧಾನ_nr].ಕೂಡಿಸು_len;
    ಆದರೆ (arg.cat_salt_len > 0)
	arg.cat_salt = ಶೀರ್ಷಿಕೆ + CRYPT_MAGIC_LEN;
    ಆದರೆ (arg.cat_ಬೀಜ_len > 0)
	arg.cat_ಬೀಜ = ಶೀರ್ಷಿಕೆ + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    ಆದರೆ (arg.cat_ಕೂಡಿಸು_len > 0)
	arg.cat_ಕೂಡಿಸು = ಶೀರ್ಷಿಕೆ + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_ಬೀಜ_len;

    ಹಿಂತಿರುಗಿ crypt_create(ವಿಧಾನ_nr, ಕೀಲಿ, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *ಕೀಲಿ)
{
    ಪೂರ್ಣಾಂಕ		ವಿಧಾನ_nr;
    ಪೂರ್ಣಾಂಕ		ಶೀರ್ಷಿಕೆ_len;
    ಅಕ್ಷರ	magic_ಬಫರ್[CRYPT_MAGIC_LEN];
    char_u	*ಬಫರ್;
    cryptstate_T *state;

    ಆದರೆ (fread(magic_ಬಫರ್, CRYPT_MAGIC_LEN, 1, fp) != 1)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ವಿಧಾನ_nr = crypt_ವಿಧಾನ_nr_from_magic(magic_ಬಫರ್, CRYPT_MAGIC_LEN);
    ಆದರೆ (ವಿಧಾನ_nr < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಶೀರ್ಷಿಕೆ_len = crypt_get_ಶೀರ್ಷಿಕೆ_len(ವಿಧಾನ_nr);
    ಆದರೆ ((ಬಫರ್ = alloc(ಶೀರ್ಷಿಕೆ_len)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    mch_memmove(ಬಫರ್, magic_ಬಫರ್, CRYPT_MAGIC_LEN);
    ಆದರೆ (ಶೀರ್ಷಿಕೆ_len > CRYPT_MAGIC_LEN
	    && fread(ಬಫರ್ + CRYPT_MAGIC_LEN,
				    ಶೀರ್ಷಿಕೆ_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(ಬಫರ್);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    state = crypt_create_from_ಶೀರ್ಷಿಕೆ(ವಿಧಾನ_nr, ಕೀಲಿ, ಬಫರ್);
    vim_free(ಬಫರ್);
    ಹಿಂತಿರುಗಿ state;
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
    ಪೂರ್ಣಾಂಕ	    ವಿಧಾನ_nr,
    char_u  *ಕೀಲಿ,
    char_u  **ಶೀರ್ಷಿಕೆ,
    ಪೂರ್ಣಾಂಕ	    *ಶೀರ್ಷಿಕೆ_len)
{
    ಪೂರ್ಣಾಂಕ	    len = crypt_get_ಶೀರ್ಷಿಕೆ_len(ವಿಧಾನ_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_ಕ್ಷೇತ್ರ(arg);
    arg.cat_salt_len = cryptmethods[ವಿಧಾನ_nr].salt_len;
    arg.cat_ಬೀಜ_len = cryptmethods[ವಿಧಾನ_nr].ಬೀಜ_len;
    arg.cat_ಕೂಡಿಸು_len  = cryptmethods[ವಿಧಾನ_nr].ಕೂಡಿಸು_len;
    arg.cat_init_from_file = FALSE;

    *ಶೀರ್ಷಿಕೆ_len = len;
    *ಶೀರ್ಷಿಕೆ = alloc(len);
    ಆದರೆ (*ಶೀರ್ಷಿಕೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    mch_memmove(*ಶೀರ್ಷಿಕೆ, cryptmethods[ವಿಧಾನ_nr].magic, CRYPT_MAGIC_LEN);
    ಆದರೆ (arg.cat_salt_len > 0 || arg.cat_ಬೀಜ_len > 0 || arg.cat_ಕೂಡಿಸು_len > 0)
    {
	ಆದರೆ (arg.cat_salt_len > 0)
	    arg.cat_salt = *ಶೀರ್ಷಿಕೆ + CRYPT_MAGIC_LEN;
	ಆದರೆ (arg.cat_ಬೀಜ_len > 0)
	    arg.cat_ಬೀಜ = *ಶೀರ್ಷಿಕೆ + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	ಆದರೆ (arg.cat_ಕೂಡಿಸು_len > 0)
	    arg.cat_ಕೂಡಿಸು = *ಶೀರ್ಷಿಕೆ + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_ಬೀಜ_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
	ಆದರೆ (sodium_init() >= 0)
	{
	    ಆದರೆ (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    ಆದರೆ (arg.cat_ಬೀಜ_len > 0)
		randombytes_buf(arg.cat_ಬೀಜ, arg.cat_ಬೀಜ_len);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    sha2_ಬೀಜ(arg.cat_salt, arg.cat_salt_len, arg.cat_ಬೀಜ, arg.cat_ಬೀಜ_len);
    }
    state = crypt_create(ವಿಧಾನ_nr, ಕೀಲಿ, &arg);
    ಆದರೆ (state == ಖಾಲಿ)
	VIM_CLEAR(*ಶೀರ್ಷಿಕೆ);
    ಹಿಂತಿರುಗಿ state;
}

/*
 * Free the crypt state.
 */
    ಶೂನ್ಯ
crypt_free_state(cryptstate_T *state)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
    ಆದರೆ (crypt_ವಿಧಾನ_is_sodium(state->ವಿಧಾನ_nr))
    {
	sodium_munlock(((sodium_state_T *)state->ವಿಧಾನ_state)->ಕೀಲಿ,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->ವಿಧಾನ_state, ಗಾತ್ರ(sodium_state_T));
	sodium_free(state->ವಿಧಾನ_state);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	vim_free(state->ವಿಧಾನ_state);
    vim_free(state);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CRYPT_NOT_INPLACE
/*
 * Encode "from[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    ದೀರ್ಘ
crypt_encode_alloc(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	**newptr,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ)
{
    cryptmethod_T *ವಿಧಾನ = &cryptmethods[state->ವಿಧಾನ_nr];

    ಆದರೆ (ವಿಧಾನ->encode_ಬಫರ್_fn != ಖಾಲಿ)
	// Has buffer function, pass through.
	ಹಿಂತಿರುಗಿ ವಿಧಾನ->encode_ಬಫರ್_fn(state, from, len, newptr, ಕೊನೆಯ);
    ಆದರೆ (len == 0)
	// Not buffering, just return EOF.
	ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)len;

    *newptr = alloc(len + 50);
    ಆದರೆ (*newptr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    ವಿಧಾನ->encode_fn(state, from, len, *newptr, ಕೊನೆಯ);
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)len;
}

/*
 * Decrypt "ptr[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    ದೀರ್ಘ
crypt_decode_alloc(
    cryptstate_T *state,
    char_u	*ptr,
    ದೀರ್ಘ	len,
    char_u      **newptr,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ)
{
    cryptmethod_T *ವಿಧಾನ = &cryptmethods[state->ವಿಧಾನ_nr];

    ಆದರೆ (ವಿಧಾನ->decode_ಬಫರ್_fn != ಖಾಲಿ)
	// Has buffer function, pass through.
	ಹಿಂತಿರುಗಿ ವಿಧಾನ->decode_ಬಫರ್_fn(state, ptr, len, newptr, ಕೊನೆಯ);

    ಆದರೆ (len == 0)
	// Not buffering, just return EOF.
	ಹಿಂತಿರುಗಿ len;

    *newptr = alloc(len);
    ಆದರೆ (*newptr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    ವಿಧಾನ->decode_fn(state, ptr, len, *newptr, ಕೊನೆಯ);
    ಹಿಂತಿರುಗಿ len;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Encrypting "from[len]" into "to[len]".
 */
    ಶೂನ್ಯ
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*ಗಮ್ಯ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ)
{
    cryptmethods[state->ವಿಧಾನ_nr].encode_fn(state, from, len, ಗಮ್ಯ, ಕೊನೆಯ);
}

#ಆದರೆ 0  //ಬಳಸದ
/*
 * decrypting "from[len]" into "to[len]".
 */
    ಶೂನ್ಯ
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*ಗಮ್ಯ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ)
{
    cryptmethods[state->ವಿಧಾನ_nr].decode_fn(state, from, len, ಗಮ್ಯ, ಕೊನೆಯ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    ಶೂನ್ಯ
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ)
{
    cryptmethods[state->ವಿಧಾನ_nr].encode_inplace_fn(state, buf, len,
								    buf, ಕೊನೆಯ);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    ಶೂನ್ಯ
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ)
{
    cryptmethods[state->ವಿಧಾನ_nr].decode_inplace_fn(state, buf, len,
								    buf, ಕೊನೆಯ);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    ಶೂನ್ಯ
crypt_free_ಕೀಲಿ(char_u *ಕೀಲಿ)
{
    // Create a safe memset which cannot be optimized away by compiler
    ಸ್ಥಿರ ಶೂನ್ಯ *(* ಅಸ್ಥಿರ vim_memset_ಸುರಕ್ಷಿತ)(ಶೂನ್ಯ *s, ಪೂರ್ಣಾಂಕ c, size_t n) =
	memset;
    ಆದರೆ (ಕೀಲಿ != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
	ಆದರೆ (sodium_init() >= 0)
	    sodium_memzero(ಕೀಲಿ, STRLEN(ಕೀಲಿ));
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    vim_memset_ಸುರಕ್ಷಿತ(ಕೀಲಿ, 0, STRLEN(ಕೀಲಿ));
	vim_free(ಕೀಲಿ);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    ಶೂನ್ಯ
crypt_check_ವಿಧಾನ(ಪೂರ್ಣಾಂಕ ವಿಧಾನ)
{
    ಆದರೆ (ವಿಧಾನ < CRYPT_M_BF2 || ವಿಧಾನ == CRYPT_M_SOD)
    {
	ಸಂದೇಶ_scroll = TRUE;
	ಸಂದೇಶ(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    ಶೂನ್ಯ
crypt_check_swapfile_curbuf(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
    ಪೂರ್ಣಾಂಕ ವಿಧಾನ = crypt_get_ವಿಧಾನ_nr(curbuf);
    ಆದರೆ (crypt_ವಿಧಾನ_is_sodium(ವಿಧಾನ))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"swf", 0, ಖಾಲಿ, OPT_LOCAL);
	ಸಂದೇಶ_scroll = TRUE;
	ಸಂದೇಶ(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
crypt_check_current_ವಿಧಾನ(ಶೂನ್ಯ)
{
    crypt_check_ವಿಧಾನ(crypt_get_ವಿಧಾನ_nr(curbuf));
}

/*
 * Ask the user for a crypt key.
 * When "store" is TRUE, the new key is stored in the 'key' option, and the
 * 'key' option value is returned: Don't free it.
 * When "store" is FALSE, the typed key is returned in allocated memory.
 * Returns NULL on failure.
 */
    char_u *
crypt_get_ಕೀಲಿ(
    ಪೂರ್ಣಾಂಕ		store,
    ಪೂರ್ಣಾಂಕ		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;

    ಗಾಗಿ (ಸುತ್ತು = 0; ; ++ಸುತ್ತು)
    {
	cmdline_ನಕ್ಷತ್ರ = TRUE;
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	p1 = getcmdline_prompt(NUL, ಸುತ್ತು == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		ಖಾಲಿ);
	cmdline_ನಕ್ಷತ್ರ = FALSE;

	ಆದರೆ (p1 == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (ಸುತ್ತು == twice)
	{
	    ಆದರೆ (p2 != ಖಾಲಿ && STRCMP(p1, p2) != 0)
	    {
		ಸಂದೇಶ(_("Keys don't match!"));
		crypt_free_ಕೀಲಿ(p1);
		crypt_free_ಕೀಲಿ(p2);
		p2 = ಖಾಲಿ;
		ಸುತ್ತು = -1;		// do it again
		ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (store)
	    {
		ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_ಕೀಲಿ(p1);
		p1 = curbuf->b_p_ಕೀಲಿ;
		crypt_check_swapfile_curbuf();
	    }
	    ನಿಲ್ಲಿಸು;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    ಆದರೆ (!crypt_ವಿಧಾನ_is_sodium(crypt_get_ವಿಧಾನ_nr(curbuf)))
    {
	ಆದರೆ (ಸಂದೇಶ_didout)
	    ಸಂದೇಶ_putchar('\n');
	need_wait_return = FALSE;
	ಸಂದೇಶ_didout = FALSE;
    }

    crypt_free_ಕೀಲಿ(p2);
    ಹಿಂತಿರುಗಿ p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    ಶೂನ್ಯ
crypt_append_ಸಂದೇಶ(
    buf_T *buf)
{
    ಆದರೆ (crypt_get_ವಿಧಾನ_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
crypt_sodium_init_(
    cryptstate_T	*state ಬಳಸದ,
    char_u		*ಕೀಲಿ ಬಳಸದ,
    crypt_arg_T		*arg ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    ಪೂರ್ಣಾಂಕ			retval = 0;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ	opslimit;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ	memlimit;
    ಪೂರ್ಣಾಂಕ			alg;

    ಆದರೆ (sodium_init() < 0)
	ಹಿಂತಿರುಗಿ FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(ಗಾತ್ರ(sodium_state_T));
    sodium_memzero(sd_state, ಗಾತ್ರ(sodium_state_T));

    ಆದರೆ ((state->ವಿಧಾನ_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->ವಿಧಾನ_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# ಆದರೆ 0
	// For testing
	ಆದರೆ (state->ವಿಧಾನ_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	ಆದರೆ (crypto_pwhash(dkey, ಗಾತ್ರ(dkey), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಕೀಲಿ, STRLEN(ಕೀಲಿ),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಸ್ಮರಣೆ_ನಕಲು(sd_state->ಕೀಲಿ, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ಕೀಲಿ, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ಕೀಲಿ, STRLEN(ಕೀಲಿ));

	ಆದರೆ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	ಆದರೆ (state->ವಿಧಾನ_nr == CRYPT_M_SOD2 && arg->cat_ಕೂಡಿಸು != ಖಾಲಿ)
	{
	    char_u	ಬಫರ್[20];
	    char_u	*p = ಬಫರ್;
	    vim_memset(ಬಫರ್, 0, 20);

	    crypt_long_long_ಗಮ್ಯ_char(opslimit, p);
	    p += ಗಾತ್ರ(opslimit);

	    crypt_long_long_ಗಮ್ಯ_char(memlimit, p);
	    p += ಗಾತ್ರ(memlimit);

	    crypt_int_ಗಮ್ಯ_char(alg, p);
	    ಸ್ಮರಣೆ_ನಕಲು(arg->cat_ಕೂಡಿಸು, ಬಫರ್, ಗಾತ್ರ(opslimit) + ಗಾತ್ರ(memlimit) + ಗಾತ್ರ(alg));
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	ಬಫರ್[20];
	char_u	*p = ಬಫರ್;
	vim_memset(ಬಫರ್, 0, 20);
	ಪೂರ್ಣಾಂಕ	ಗಾತ್ರ_2 = ಗಾತ್ರ(opslimit) +
	    ಗಾತ್ರ(memlimit) + ಗಾತ್ರ(alg);

	// Reading parameters from file
	ಆದರೆ (arg->cat_ಕೂಡಿಸು_len < ಗಾತ್ರ_2)
	{
	    sodium_free(sd_state);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// derive the key from the file header
	ಸ್ಮರಣೆ_ನಕಲು(p, arg->cat_ಕೂಡಿಸು, ಗಾತ್ರ_2);
	arg->cat_ಕೂಡಿಸು += ಗಾತ್ರ_2;

	opslimit = crypt_char_ಗಮ್ಯ_long_long(p);
	p += ಗಾತ್ರ(opslimit);
	memlimit = crypt_char_ಗಮ್ಯ_long_long(p);
	p += ಗಾತ್ರ(memlimit);
	alg = crypt_char_ಗಮ್ಯ_int(p);
	p += ಗಾತ್ರ(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	ಆದರೆ (crypto_pwhash(dkey, ಗಾತ್ರ(dkey), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಕೀಲಿ, STRLEN(ಕೀಲಿ),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಸ್ಮರಣೆ_ನಕಲು(sd_state->ಕೀಲಿ, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ಕೀಲಿ, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ಕೀಲಿ, STRLEN(ಕೀಲಿ));

	ಆದರೆ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    sd_state->ಎಣಿಕೆ_2 = 0;
    state->ವಿಧಾನ_state = sd_state;

    ಹಿಂತಿರುಗಿ OK;
#ಇಲ್ಲದಿದ್ದರೆ
    emsg(_(e_libsodium_not_built_ಒಳ));
    ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#ಆದರೆ 0  // Currently unused
    ಶೂನ್ಯ
crypt_sodium_encode(
    cryptstate_T *state ಬಳಸದ,
    char_u	*from ಬಳಸದ,
    size_t	len ಬಳಸದ,
    char_u	*ಗಮ್ಯ ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ ಬಳಸದ)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->ವಿಧಾನ_state;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ  ಟ್ಯಾಗ್ = ಕೊನೆಯ
			? crypto_secretstream_xchacha20poly1305_ಟ್ಯಾಗ್_FINAL  : 0;

    ಆದರೆ (sod_st->ಎಣಿಕೆ_2 == 0)
    {
	ಆದರೆ (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_ಶೀರ್ಷಿಕೆ));
	    ಹಿಂತಿರುಗಿ;
	}
	crypto_secretstream_xchacha20poly1305_init_ತಳ್ಳು(&sod_st->state,
							      ಗಮ್ಯ, sod_st->ಕೀಲಿ);
	ಗಮ್ಯ += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ಆದರೆ (sod_st->ಎಣಿಕೆ_2 && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_ಬಫರ್));
	ಹಿಂತಿರುಗಿ;
    }

    crypto_secretstream_xchacha20poly1305_ತಳ್ಳು(&sod_st->state, ಗಮ್ಯ, ಖಾಲಿ,
						      from, len, ಖಾಲಿ, 0, ಟ್ಯಾಗ್);

    sod_st->ಎಣಿಕೆ_2++;
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#ಆದರೆ 0  // Currently unused
    ಶೂನ್ಯ
crypt_sodium_decode(
    cryptstate_T *state ಬಳಸದ,
    char_u	*from ಬಳಸದ,
    size_t	len ಬಳಸದ,
    char_u	*ಗಮ್ಯ ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ ಬಳಸದ)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->ವಿಧಾನ_state;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ  ಟ್ಯಾಗ್;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ buf_len;
    char_u *p1 = from;
    char_u *p2 = ಗಮ್ಯ;
    char_u *buf_ಹೊರಹೋಗು;

    ಆದರೆ (sod_st->ಎಣಿಕೆ_2 == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_ಶೀರ್ಷಿಕೆ));
	ಹಿಂತಿರುಗಿ;
    }

    buf_ಹೊರಹೋಗು = (char_u *)alloc(len);

    ಆದರೆ (buf_ಹೊರಹೋಗು == ಖಾಲಿ)
    {
	emsg(_(e_libsodium_cannot_allocate_ಬಫರ್));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (sod_st->ಎಣಿಕೆ_2 == 0)
    {
	ಆದರೆ (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->ಕೀಲಿ) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_ಶೀರ್ಷಿಕೆ_incomplete));
	    ಹೋಗು fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	ಆದರೆ (p1 == p2)
	    ಗಮ್ಯ += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ಆದರೆ (sod_st->ಎಣಿಕೆ_2 && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_ಬಫರ್));
	ಹೋಗು fail;
    }
    ಆದರೆ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_ಹೊರಹೋಗು, &buf_len, &ಟ್ಯಾಗ್, from, len, ಖಾಲಿ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	ಹೋಗು fail;
    }
    sod_st->ಎಣಿಕೆ_2++;

    ಆದರೆ (ಟ್ಯಾಗ್ == crypto_secretstream_xchacha20poly1305_ಟ್ಯಾಗ್_FINAL && !ಕೊನೆಯ)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	ಹೋಗು fail;
    }
    ಆದರೆ (p1 == p2)
	mch_memmove(p2, buf_ಹೊರಹೋಗು, buf_len);

fail:
    vim_free(buf_ಹೊರಹೋಗು);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
crypt_sodium_ಬಫರ್_encode(
    cryptstate_T *state ಬಳಸದ,
    char_u	*from ಬಳಸದ,
    size_t	len ಬಳಸದ,
    char_u	**buf_ಹೊರಹೋಗು ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ	ಹೊರಹೋಗು_len;
    char_u		*ptr;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ	ಟ್ಯಾಗ್ = ಕೊನೆಯ
			? crypto_secretstream_xchacha20poly1305_ಟ್ಯಾಗ್_FINAL  : 0;
    ಪೂರ್ಣಾಂಕ			ಉದ್ದಳತೆ;
    sodium_state_T	*sod_st = state->ವಿಧಾನ_state;
    ಪೂರ್ಣಾಂಕ			ಮೊದಲ = (sod_st->ಎಣಿಕೆ_2 == 0);

    ಉದ್ದಳತೆ = (ಪೂರ್ಣಾಂಕ)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (ಮೊದಲ ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_ಹೊರಹೋಗು = alloc_clear(ಉದ್ದಳತೆ);
    ಆದರೆ (*buf_ಹೊರಹೋಗು == ಖಾಲಿ)
    {
	emsg(_(e_libsodium_cannot_allocate_ಬಫರ್));
	ಹಿಂತಿರುಗಿ -1;
    }
    ptr = *buf_ಹೊರಹೋಗು;

    ಆದರೆ (ಮೊದಲ)
    {
	crypto_secretstream_xchacha20poly1305_init_ತಳ್ಳು(&sod_st->state,
		ptr, sod_st->ಕೀಲಿ);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ತಳ್ಳು(&sod_st->state, ptr,
	    &ಹೊರಹೋಗು_len, from, len, ಖಾಲಿ, 0, ಟ್ಯಾಗ್);

    sod_st->ಎಣಿಕೆ_2++;
    ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು_len + (ಮೊದಲ
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ -1;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
crypt_sodium_ಬಫರ್_decode(
    cryptstate_T *state ಬಳಸದ,
    char_u	*from ಬಳಸದ,
    size_t	len ಬಳಸದ,
    char_u	**buf_ಹೊರಹೋಗು ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->ವಿಧಾನ_state;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ  ಟ್ಯಾಗ್;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ ಹೊರಹೋಗು_len;

    ಆದರೆ (sod_st->ಎಣಿಕೆ_2 == 0
	    && state->ವಿಧಾನ_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].ಕೂಡಿಸು_len;

    *buf_ಹೊರಹೋಗು = alloc_clear(len);
    ಆದರೆ (*buf_ಹೊರಹೋಗು == ಖಾಲಿ)
    {
	emsg(_(e_libsodium_cannot_allocate_ಬಫರ್));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (sod_st->ಎಣಿಕೆ_2 == 0)
    {
	ಆದರೆ (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->ಕೀಲಿ) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_ಶೀರ್ಷಿಕೆ_incomplete));
	    ಹಿಂತಿರುಗಿ -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->ಎಣಿಕೆ_2++;
    }
    ಆದರೆ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_ಹೊರಹೋಗು, &ಹೊರಹೋಗು_len, &ಟ್ಯಾಗ್, from, len, ಖಾಲಿ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (ಟ್ಯಾಗ್ == crypto_secretstream_xchacha20poly1305_ಟ್ಯಾಗ್_FINAL && !ಕೊನೆಯ)
	emsg(_(e_libsodium_decryption_failed_premature));
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ) ಹೊರಹೋಗು_len;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ -1;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_SODIUM)
    ಶೂನ್ಯ
crypt_sodium_ಬೀಗ_ಕೀಲಿ(char_u *ಕೀಲಿ)
{
    ಆದರೆ (sodium_init() >= 0)
	sodium_mlock(ಕೀಲಿ, STRLEN(ಕೀಲಿ));
}

    ಪೂರ್ಣಾಂಕ
crypt_sodium_munlock(ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ addr, ಸ್ಥಿರಾಂಕ size_t len)
{
    ಹಿಂತಿರುಗಿ sodium_munlock(addr, len);
}

    ಶೂನ್ಯ
crypt_sodium_randombytes_buf(ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ buf, ಸ್ಥಿರಾಂಕ size_t ಗಾತ್ರ_2)
{
    randombytes_buf(buf, ಗಾತ್ರ_2);
}

    ಪೂರ್ಣಾಂಕ
crypt_sodium_init(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ randombytes_random();
}

# ಆದರೆ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಶೂನ್ಯ
crypt_sodium_report_hash_params(
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ opslimit,
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ ops_def,
	size_t memlimit,
	size_t mem_def,
	ಪೂರ್ಣಾಂಕ alg,
	ಪೂರ್ಣಾಂಕ alg_def)
{
    ಆದರೆ (p_verbose > 0)
    {
	verbose_enter();
	ಆದರೆ (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	ಇಲ್ಲದಿದ್ದರೆ
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	ಆದರೆ (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)memlimit);
	ಇಲ್ಲದಿದ್ದರೆ
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)memlimit);
	ಆದರೆ (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	ಇಲ್ಲದಿದ್ದರೆ
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
crypt_long_long_ಗಮ್ಯ_char(ದೀರ್ಘ ದೀರ್ಘ n, char_u *s)
{
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ)n >> 8;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
crypt_int_ಗಮ್ಯ_char(ಪೂರ್ಣಾಂಕ n, char_u *s)
{
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ)n >> 8;
    }
}

    ಸ್ಥಿರ ದೀರ್ಘ ದೀರ್ಘ
crypt_char_ಗಮ್ಯ_long_long(char_u *s)
{
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ    retval = 0;
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ (i = 7; i >= 0; i--)
    {
	ಆದರೆ (i == 7)
	    retval = s[i];
	ಇಲ್ಲದಿದ್ದರೆ
	    retval |= s[i];
	ಆದರೆ (i > 0)
	    retval <<= 8;
    }
    ಹಿಂತಿರುಗಿ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
crypt_char_ಗಮ್ಯ_int(char_u *s)
{
    ಪೂರ್ಣಾಂಕ retval = 0;
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 3; i >= 0; i--)
    {
	ಆದರೆ (i == 3)
	    retval = s[i];
	ಇಲ್ಲದಿದ್ದರೆ
	    retval |= s[i];
	ಆದರೆ (i > 0)
	    retval <<= 8;
    }
    ಹಿಂತಿರುಗಿ retval;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ // FEAT_CRYPT
