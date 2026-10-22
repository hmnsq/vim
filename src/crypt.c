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
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_CRYPT)
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ	*name;		// encryption name as used in 'cryptmethod'
    ਅੱਖਰ	*magic;		// magic bytes stored in file header
    ਪੂਰਨ		salt_len;	// length of salt, or 0 when not using salt
    ਪੂਰਨ		ਬੀਜ_len;	// length of seed, or 0 when not using seed
    ਪੂਰਨ		ਜੋੜੋ_len;	// additional length in the header needed for storing
				// custom data
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CRYPT_NOT_INPLACE
    ਪੂਰਨ		ਚੱਲਦਾ_inplace;	// encryption/decryption can be done in-place
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਸਾਰਾ_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    ਪੂਰਨ (*self_test_fn)(ਖਾਲੀ);

    // Function pointer for initializing encryption/decryption.
    ਪੂਰਨ (* init_fn)(cryptstate_T *state, char_u *ਕੁੰਜੀ, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    ਖਾਲੀ (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *ਮੰਜ਼ਿਲ, ਪੂਰਨ ਆਖਰੀ);
    ਖਾਲੀ (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *ਮੰਜ਼ਿਲ, ਪੂਰਨ ਆਖਰੀ);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    ਲੰਮਾ (*encode_ਬਫਰ_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, ਪੂਰਨ ਆਖਰੀ);
    ਲੰਮਾ (*decode_ਬਫਰ_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, ਪੂਰਨ ਆਖਰੀ);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    ਖਾਲੀ (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, ਪੂਰਨ ਆਖਰੀ);
    ਖਾਲੀ (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, ਪੂਰਨ ਆਖਰੀ);
} cryptmethod_T;

ਸਥਿਰ ਪੂਰਨ crypt_sodium_init_(cryptstate_T *state, char_u *ਕੁੰਜੀ, crypt_arg_T *arg);
ਸਥਿਰ ਲੰਮਾ crypt_sodium_ਬਫਰ_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_ਬਾਹਰਲਾ, ਪੂਰਨ ਆਖਰੀ);
ਸਥਿਰ ਲੰਮਾ crypt_sodium_ਬਫਰ_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_ਬਾਹਰਲਾ, ਪੂਰਨ ਆਖਰੀ);
#ਜੇ defined(FEAT_SODIUM)
ਸਥਿਰ ਖਾਲੀ crypt_long_long_ਮੰਜ਼ਿਲ_char(ਲੰਮਾ ਲੰਮਾ n, char_u *s);
ਸਥਿਰ ਖਾਲੀ crypt_int_ਮੰਜ਼ਿਲ_char(ਪੂਰਨ n, char_u *s);
ਸਥਿਰ ਲੰਮਾ ਲੰਮਾ crypt_char_ਮੰਜ਼ਿਲ_long_long(char_u *s);
ਸਥਿਰ ਪੂਰਨ crypt_char_ਮੰਜ਼ਿਲ_int(char_u *s);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ਸਥਿਰ ਖਾਲੀ crypt_sodium_report_hash_params(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ opslimit, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ ops_def, size_t memlimit, size_t mem_def, ਪੂਰਨ alg, ਪੂਰਨ alg_def);
#ਜੇ_ਅੰਤ

// index is method_nr of cryptstate_T, CRYPT_M_*
ਸਥਿਰ cryptmethod_T cryptmethods[CRYPT_M_ਗਿਣਤੀ] = {
    // PK_Zip; very weak
    {
	"zip",
	"VimCrypt~01!",
	0,
	0,
	0,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CRYPT_NOT_INPLACE
	TRUE,
#ਜੇ_ਅੰਤ
	FALSE,
	ਨਿਰਾ,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	ਨਿਰਾ, ਨਿਰਾ,
	crypt_zip_encode, crypt_zip_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; implementation issues.
    {
	"blowfish",
	"VimCrypt~02!",
	8,
	8,
	0,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CRYPT_NOT_INPLACE
	TRUE,
#ਜੇ_ਅੰਤ
	FALSE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	ਨਿਰਾ, ਨਿਰਾ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; fixed.
    {
	"blowfish2",
	"VimCrypt~03!",
	8,
	8,
	0,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CRYPT_NOT_INPLACE
	TRUE,
#ਜੇ_ਅੰਤ
	TRUE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	ਨਿਰਾ, ਨਿਰਾ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ਨਹੀਂਤਾਂ
	16,
#ਜੇ_ਅੰਤ
	8,
	0,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CRYPT_NOT_INPLACE
	FALSE,
#ਜੇ_ਅੰਤ
	FALSE,
	ਨਿਰਾ,
	crypt_sodium_init_,
	ਨਿਰਾ, ਨਿਰਾ,
	crypt_sodium_ਬਫਰ_encode, crypt_sodium_ਬਫਰ_decode,
	ਨਿਰਾ, ਨਿਰਾ,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ਨਹੀਂਤਾਂ
	16,
#ਜੇ_ਅੰਤ
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CRYPT_NOT_INPLACE
	FALSE,
#ਜੇ_ਅੰਤ
	FALSE,
	ਨਿਰਾ,
	crypt_sodium_init_,
	ਨਿਰਾ, ਨਿਰਾ,
	crypt_sodium_ਬਫਰ_encode, crypt_sodium_ਬਫਰ_decode,
	ਨਿਰਾ, ਨਿਰਾ,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#ਜੇ defined(FEAT_SODIUM)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    size_t	    ਗਿਣਤੀ_2;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ   ਕੁੰਜੀ[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define ਲੱਦੋ_dll vim_ਲੱਦੋ_Lib
#   define ਚਿੰਨ੍ਹ_from_dll Get_Proc_ਪਤਾ
#   define close_dll FreeLibrary
#   define ਲੱਦੋ_dll_error GetWin32Error
#  ਨਹੀਂਤਾਂ
#   error Dynamic loading ਦਾ libsodium is not supported ਲਈ now.
//#   define HINSTANCE void*
//#   define SODIUM_PROC void*
//#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
//#   define symbol_from_dll dlsym
//#   define close_dll dlclose
//#   define load_dll_error dlerror
#  endif

#  define sodium_init	    ਲੱਦੋ_sodium
#  define sodium_free	    dll_sodium_free
#  define sodium_malloc	    dll_sodium_malloc
#  define sodium_memzero    dll_sodium_memzero
#  define sodium_mlock	    dll_sodium_mlock
#  define sodium_munlock    dll_sodium_munlock
#  define crypto_secretstream_xchacha20poly1305_init_ਧੱਕੋ \
    dll_crypto_secretstream_xchacha20poly1305_init_ਧੱਕੋ
#  define crypto_secretstream_xchacha20poly1305_ਧੱਕੋ \
    dll_crypto_secretstream_xchacha20poly1305_ਧੱਕੋ
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

ਸਥਿਰ ਪੂਰਨ (*dll_sodium_init)(ਖਾਲੀ) = ਨਿਰਾ;
ਸਥਿਰ ਖਾਲੀ (*dll_sodium_free)(ਖਾਲੀ *) = ਨਿਰਾ;
ਸਥਿਰ ਖਾਲੀ *(*dll_sodium_malloc)(ਅਚਲ size_t) = ਨਿਰਾ;
ਸਥਿਰ ਖਾਲੀ (*dll_sodium_memzero)(ਖਾਲੀ * ਅਚਲ, ਅਚਲ size_t) = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ (*dll_sodium_mlock)(ਖਾਲੀ * ਅਚਲ, ਅਚਲ size_t) = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ (*dll_sodium_munlock)(ਖਾਲੀ * ਅਚਲ, ਅਚਲ size_t) = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ (*dll_crypto_secretstream_xchacha20poly1305_init_ਧੱਕੋ)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ [],
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ []) = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ (*dll_crypto_secretstream_xchacha20poly1305_ਧੱਕੋ)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *c, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ *clen_p,
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *m, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ mlen,
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ad, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ adlen, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਟੈਗ)
    = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ [],
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ []) = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *m, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ *mlen_p, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਟੈਗ_p,
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *c, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ clen,
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ad, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ adlen) = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ (*dll_crypto_pwhash)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ * ਅਚਲ ਬਾਹਰਲਾ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ outlen,
    ਅਚਲ ਅੱਖਰ * ਅਚਲ passwd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ passwdlen,
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ * ਅਚਲ salt,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ opslimit, size_t memlimit, ਪੂਰਨ alg)
    = ਨਿਰਾ;
ਸਥਿਰ ਖਾਲੀ (*dll_randombytes_buf)(ਖਾਲੀ * ਅਚਲ buf, ਅਚਲ size_t ਮਾਪ);
ਸਥਿਰ uint32_t (*dll_randombytes_random)(ਖਾਲੀ);

ਸਥਿਰ ਬਣਤਰ {
    ਅਚਲ ਅੱਖਰ *name;
    SODIUM_PROC *ptr;
} sodium_funcname_ਸਾਰਣੀ[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_ਧੱਕੋ},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_ਧੱਕੋ},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {ਨਿਰਾ, ਨਿਰਾ}
};

    ਸਥਿਰ ਪੂਰਨ
sodium_runtime_link_init(ਪੂਰਨ verbose)
{
    ਸਥਿਰ HINSTANCE hsodium = ਨਿਰਾ;
    ਅਚਲ ਅੱਖਰ *libname = DYNAMIC_SODIUM_DLL;
    ਪੂਰਨ i;

    ਜੇ (hsodium != ਨਿਰਾ)
	ਵਾਪਸ OK;

    hsodium = ਲੱਦੋ_dll(libname);
    ਜੇ (hsodium == ਨਿਰਾ)
    {
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str), libname, ਲੱਦੋ_dll_error());
	ਵਾਪਸ FAIL;
    }

    ਲਈ (i = 0; sodium_funcname_ਸਾਰਣੀ[i].ptr; ++i)
    {
	ਜੇ ((*sodium_funcname_ਸਾਰਣੀ[i].ptr = ਚਿੰਨ੍ਹ_from_dll(hsodium,
			sodium_funcname_ਸਾਰਣੀ[i].name)) == ਨਿਰਾ)
	{
	    close_dll(hsodium);
	    hsodium = ਨਿਰਾ;
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), sodium_funcname_ਸਾਰਣੀ[i].name);
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
ਲੱਦੋ_sodium(ਖਾਲੀ)
{
    ਜੇ (sodium_runtime_link_init(TRUE) == FAIL)
	ਵਾਪਸ -1;
    ਵਾਪਸ dll_sodium_init();
}
# endif

# ਜੇ defined(DYNAMIC_SODIUM)
    ਪੂਰਨ
sodium_enabled(ਪੂਰਨ verbose)
{
    ਵਾਪਸ sodium_runtime_link_init(verbose) == OK;
}
# endif
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRYPT_MAGIC_LEN	12	// cannot change
ਸਥਿਰ ਅਚਲ ਅੱਖਰ crypt_magic_ਸਿਰ[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    ਪੂਰਨ
crypt_ਵਿਧੀ_nr_from_name(char_u *name)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < CRYPT_M_ਗਿਣਤੀ; ++i)
	ਜੇ (STRCMP(name, cryptmethods[i].name) == 0)
	    ਵਾਪਸ i;
    ਵਾਪਸ 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    ਪੂਰਨ
crypt_ਵਿਧੀ_nr_from_magic(ਅੱਖਰ *ptr, ਪੂਰਨ len)
{
    ਪੂਰਨ i;

    ਜੇ (len < CRYPT_MAGIC_LEN)
	ਵਾਪਸ -1;

    ਲਈ (i = 0; i < CRYPT_M_ਗਿਣਤੀ; i++)
	ਜੇ (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    ਵਾਪਸ i;

    i = (ਪੂਰਨ)STRLEN(crypt_magic_ਸਿਰ);
    ਜੇ (len >= i && memcmp(ptr, crypt_magic_ਸਿਰ, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_ਵਿਧੀ));

    ਵਾਪਸ -1;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    ਪੂਰਨ
crypt_ਚੱਲਦਾ_inplace(cryptstate_T *state)
{
    ਵਾਪਸ cryptmethods[state->ਵਿਧੀ_nr].ਚੱਲਦਾ_inplace;
}
#ਜੇ_ਅੰਤ

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    ਪੂਰਨ
crypt_get_ਵਿਧੀ_nr(buf_T *buf)
{
    ਵਾਪਸ crypt_ਵਿਧੀ_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    ਪੂਰਨ
crypt_ਵਿਧੀ_is_sodium(ਪੂਰਨ ਵਿਧੀ)
{
    ਵਾਪਸ ਵਿਧੀ == CRYPT_M_SOD || ਵਿਧੀ == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    ਪੂਰਨ
crypt_ਸਾਰਾ_undofile(ਪੂਰਨ ਵਿਧੀ_nr)
{
    ਵਾਪਸ cryptmethods[ਵਿਧੀ_nr].ਸਾਰਾ_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    ਪੂਰਨ
crypt_get_ਸਿਰਲੇਖ_len(ਪੂਰਨ ਵਿਧੀ_nr)
{
    ਵਾਪਸ CRYPT_MAGIC_LEN
	+ cryptmethods[ਵਿਧੀ_nr].salt_len
	+ cryptmethods[ਵਿਧੀ_nr].ਬੀਜ_len
	+ cryptmethods[ਵਿਧੀ_nr].ਜੋੜੋ_len;
}


#ਜੇ defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    ਪੂਰਨ
crypt_get_max_ਸਿਰਲੇਖ_len(ਖਾਲੀ)
{
    ਪੂਰਨ i;
    ਪੂਰਨ max = 0;
    ਪੂਰਨ ਆਰਜ਼ੀ = 0;

    ਲਈ (i = 0; i < CRYPT_M_ਗਿਣਤੀ; ++i)
    {
	ਆਰਜ਼ੀ = crypt_get_ਸਿਰਲੇਖ_len(i);
	ਜੇ (ਆਰਜ਼ੀ > max)
	    max = ਆਰਜ਼ੀ;
    }
    ਵਾਪਸ max;
}
#ਜੇ_ਅੰਤ

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    ਖਾਲੀ
crypt_ਸੈੱਟ_cm_option(buf_T *buf, ਪੂਰਨ ਵਿਧੀ_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[ਵਿਧੀ_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    ਪੂਰਨ
crypt_self_test(ਖਾਲੀ)
{
    ਪੂਰਨ ਵਿਧੀ_nr = crypt_get_ਵਿਧੀ_nr(curbuf);

    ਜੇ (cryptmethods[ਵਿਧੀ_nr].self_test_fn == ਨਿਰਾ)
	ਵਾਪਸ OK;
    ਵਾਪਸ cryptmethods[ਵਿਧੀ_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    ਪੂਰਨ		ਵਿਧੀ_nr,
    char_u	*ਕੁੰਜੀ,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    ਜੇ (state == ਨਿਰਾ)
	ਵਾਪਸ state;

    state->ਵਿਧੀ_nr = ਵਿਧੀ_nr;
    ਜੇ (cryptmethods[ਵਿਧੀ_nr].init_fn(state, ਕੁੰਜੀ, crypt_arg) == FAIL)
    {
	vim_free(state);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_ਸਿਰਲੇਖ(
    ਪੂਰਨ		ਵਿਧੀ_nr,
    char_u	*ਕੁੰਜੀ,
    char_u	*ਸਿਰਲੇਖ)
{
    crypt_arg_T arg;

    CLEAR_ਖੇਤਰ(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[ਵਿਧੀ_nr].salt_len;
    arg.cat_ਬੀਜ_len = cryptmethods[ਵਿਧੀ_nr].ਬੀਜ_len;
    arg.cat_ਜੋੜੋ_len = cryptmethods[ਵਿਧੀ_nr].ਜੋੜੋ_len;
    ਜੇ (arg.cat_salt_len > 0)
	arg.cat_salt = ਸਿਰਲੇਖ + CRYPT_MAGIC_LEN;
    ਜੇ (arg.cat_ਬੀਜ_len > 0)
	arg.cat_ਬੀਜ = ਸਿਰਲੇਖ + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    ਜੇ (arg.cat_ਜੋੜੋ_len > 0)
	arg.cat_ਜੋੜੋ = ਸਿਰਲੇਖ + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_ਬੀਜ_len;

    ਵਾਪਸ crypt_create(ਵਿਧੀ_nr, ਕੁੰਜੀ, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *ਕੁੰਜੀ)
{
    ਪੂਰਨ		ਵਿਧੀ_nr;
    ਪੂਰਨ		ਸਿਰਲੇਖ_len;
    ਅੱਖਰ	magic_ਬਫਰ[CRYPT_MAGIC_LEN];
    char_u	*ਬਫਰ;
    cryptstate_T *state;

    ਜੇ (fread(magic_ਬਫਰ, CRYPT_MAGIC_LEN, 1, fp) != 1)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਿਧੀ_nr = crypt_ਵਿਧੀ_nr_from_magic(magic_ਬਫਰ, CRYPT_MAGIC_LEN);
    ਜੇ (ਵਿਧੀ_nr < 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਸਿਰਲੇਖ_len = crypt_get_ਸਿਰਲੇਖ_len(ਵਿਧੀ_nr);
    ਜੇ ((ਬਫਰ = alloc(ਸਿਰਲੇਖ_len)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    mch_memmove(ਬਫਰ, magic_ਬਫਰ, CRYPT_MAGIC_LEN);
    ਜੇ (ਸਿਰਲੇਖ_len > CRYPT_MAGIC_LEN
	    && fread(ਬਫਰ + CRYPT_MAGIC_LEN,
				    ਸਿਰਲੇਖ_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(ਬਫਰ);
	ਵਾਪਸ ਨਿਰਾ;
    }

    state = crypt_create_from_ਸਿਰਲੇਖ(ਵਿਧੀ_nr, ਕੁੰਜੀ, ਬਫਰ);
    vim_free(ਬਫਰ);
    ਵਾਪਸ state;
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
    ਪੂਰਨ	    ਵਿਧੀ_nr,
    char_u  *ਕੁੰਜੀ,
    char_u  **ਸਿਰਲੇਖ,
    ਪੂਰਨ	    *ਸਿਰਲੇਖ_len)
{
    ਪੂਰਨ	    len = crypt_get_ਸਿਰਲੇਖ_len(ਵਿਧੀ_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_ਖੇਤਰ(arg);
    arg.cat_salt_len = cryptmethods[ਵਿਧੀ_nr].salt_len;
    arg.cat_ਬੀਜ_len = cryptmethods[ਵਿਧੀ_nr].ਬੀਜ_len;
    arg.cat_ਜੋੜੋ_len  = cryptmethods[ਵਿਧੀ_nr].ਜੋੜੋ_len;
    arg.cat_init_from_file = FALSE;

    *ਸਿਰਲੇਖ_len = len;
    *ਸਿਰਲੇਖ = alloc(len);
    ਜੇ (*ਸਿਰਲੇਖ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    mch_memmove(*ਸਿਰਲੇਖ, cryptmethods[ਵਿਧੀ_nr].magic, CRYPT_MAGIC_LEN);
    ਜੇ (arg.cat_salt_len > 0 || arg.cat_ਬੀਜ_len > 0 || arg.cat_ਜੋੜੋ_len > 0)
    {
	ਜੇ (arg.cat_salt_len > 0)
	    arg.cat_salt = *ਸਿਰਲੇਖ + CRYPT_MAGIC_LEN;
	ਜੇ (arg.cat_ਬੀਜ_len > 0)
	    arg.cat_ਬੀਜ = *ਸਿਰਲੇਖ + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	ਜੇ (arg.cat_ਜੋੜੋ_len > 0)
	    arg.cat_ਜੋੜੋ = *ਸਿਰਲੇਖ + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_ਬੀਜ_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
	ਜੇ (sodium_init() >= 0)
	{
	    ਜੇ (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    ਜੇ (arg.cat_ਬੀਜ_len > 0)
		randombytes_buf(arg.cat_ਬੀਜ, arg.cat_ਬੀਜ_len);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    sha2_ਬੀਜ(arg.cat_salt, arg.cat_salt_len, arg.cat_ਬੀਜ, arg.cat_ਬੀਜ_len);
    }
    state = crypt_create(ਵਿਧੀ_nr, ਕੁੰਜੀ, &arg);
    ਜੇ (state == ਨਿਰਾ)
	VIM_CLEAR(*ਸਿਰਲੇਖ);
    ਵਾਪਸ state;
}

/*
 * Free the crypt state.
 */
    ਖਾਲੀ
crypt_free_state(cryptstate_T *state)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
    ਜੇ (crypt_ਵਿਧੀ_is_sodium(state->ਵਿਧੀ_nr))
    {
	sodium_munlock(((sodium_state_T *)state->ਵਿਧੀ_state)->ਕੁੰਜੀ,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->ਵਿਧੀ_state, ਆਕਾਰ(sodium_state_T));
	sodium_free(state->ਵਿਧੀ_state);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	vim_free(state->ਵਿਧੀ_state);
    vim_free(state);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CRYPT_NOT_INPLACE
/*
 * Encode "from[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    ਲੰਮਾ
crypt_encode_alloc(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	**newptr,
    ਪੂਰਨ		ਆਖਰੀ)
{
    cryptmethod_T *ਵਿਧੀ = &cryptmethods[state->ਵਿਧੀ_nr];

    ਜੇ (ਵਿਧੀ->encode_ਬਫਰ_fn != ਨਿਰਾ)
	// Has buffer function, pass through.
	ਵਾਪਸ ਵਿਧੀ->encode_ਬਫਰ_fn(state, from, len, newptr, ਆਖਰੀ);
    ਜੇ (len == 0)
	// Not buffering, just return EOF.
	ਵਾਪਸ (ਲੰਮਾ)len;

    *newptr = alloc(len + 50);
    ਜੇ (*newptr == ਨਿਰਾ)
	ਵਾਪਸ -1;
    ਵਿਧੀ->encode_fn(state, from, len, *newptr, ਆਖਰੀ);
    ਵਾਪਸ (ਲੰਮਾ)len;
}

/*
 * Decrypt "ptr[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    ਲੰਮਾ
crypt_decode_alloc(
    cryptstate_T *state,
    char_u	*ptr,
    ਲੰਮਾ	len,
    char_u      **newptr,
    ਪੂਰਨ		ਆਖਰੀ)
{
    cryptmethod_T *ਵਿਧੀ = &cryptmethods[state->ਵਿਧੀ_nr];

    ਜੇ (ਵਿਧੀ->decode_ਬਫਰ_fn != ਨਿਰਾ)
	// Has buffer function, pass through.
	ਵਾਪਸ ਵਿਧੀ->decode_ਬਫਰ_fn(state, ptr, len, newptr, ਆਖਰੀ);

    ਜੇ (len == 0)
	// Not buffering, just return EOF.
	ਵਾਪਸ len;

    *newptr = alloc(len);
    ਜੇ (*newptr == ਨਿਰਾ)
	ਵਾਪਸ -1;
    ਵਿਧੀ->decode_fn(state, ptr, len, *newptr, ਆਖਰੀ);
    ਵਾਪਸ len;
}
#ਜੇ_ਅੰਤ

/*
 * Encrypting "from[len]" into "to[len]".
 */
    ਖਾਲੀ
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*ਮੰਜ਼ਿਲ,
    ਪੂਰਨ		ਆਖਰੀ)
{
    cryptmethods[state->ਵਿਧੀ_nr].encode_fn(state, from, len, ਮੰਜ਼ਿਲ, ਆਖਰੀ);
}

#ਜੇ 0  //ਅਣਵਰਤਿਆ
/*
 * decrypting "from[len]" into "to[len]".
 */
    ਖਾਲੀ
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*ਮੰਜ਼ਿਲ,
    ਪੂਰਨ		ਆਖਰੀ)
{
    cryptmethods[state->ਵਿਧੀ_nr].decode_fn(state, from, len, ਮੰਜ਼ਿਲ, ਆਖਰੀ);
}
#ਜੇ_ਅੰਤ

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    ਖਾਲੀ
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    ਪੂਰਨ		ਆਖਰੀ)
{
    cryptmethods[state->ਵਿਧੀ_nr].encode_inplace_fn(state, buf, len,
								    buf, ਆਖਰੀ);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    ਖਾਲੀ
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    ਪੂਰਨ		ਆਖਰੀ)
{
    cryptmethods[state->ਵਿਧੀ_nr].decode_inplace_fn(state, buf, len,
								    buf, ਆਖਰੀ);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    ਖਾਲੀ
crypt_free_ਕੁੰਜੀ(char_u *ਕੁੰਜੀ)
{
    // Create a safe memset which cannot be optimized away by compiler
    ਸਥਿਰ ਖਾਲੀ *(* ਅਸਥਿਰ vim_memset_ਸੁਰੱਖਿਅਤ)(ਖਾਲੀ *s, ਪੂਰਨ c, size_t n) =
	memset;
    ਜੇ (ਕੁੰਜੀ != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
	ਜੇ (sodium_init() >= 0)
	    sodium_memzero(ਕੁੰਜੀ, STRLEN(ਕੁੰਜੀ));
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    vim_memset_ਸੁਰੱਖਿਅਤ(ਕੁੰਜੀ, 0, STRLEN(ਕੁੰਜੀ));
	vim_free(ਕੁੰਜੀ);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    ਖਾਲੀ
crypt_check_ਵਿਧੀ(ਪੂਰਨ ਵਿਧੀ)
{
    ਜੇ (ਵਿਧੀ < CRYPT_M_BF2 || ਵਿਧੀ == CRYPT_M_SOD)
    {
	ਸੁਨੇਹਾ_scroll = TRUE;
	ਸੁਨੇਹਾ(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    ਖਾਲੀ
crypt_check_swapfile_curbuf(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
    ਪੂਰਨ ਵਿਧੀ = crypt_get_ਵਿਧੀ_nr(curbuf);
    ਜੇ (crypt_ਵਿਧੀ_is_sodium(ਵਿਧੀ))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"swf", 0, ਨਿਰਾ, OPT_LOCAL);
	ਸੁਨੇਹਾ_scroll = TRUE;
	ਸੁਨੇਹਾ(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
crypt_check_current_ਵਿਧੀ(ਖਾਲੀ)
{
    crypt_check_ਵਿਧੀ(crypt_get_ਵਿਧੀ_nr(curbuf));
}

/*
 * Ask the user for a crypt key.
 * When "store" is TRUE, the new key is stored in the 'key' option, and the
 * 'key' option value is returned: Don't free it.
 * When "store" is FALSE, the typed key is returned in allocated memory.
 * Returns NULL on failure.
 */
    char_u *
crypt_get_ਕੁੰਜੀ(
    ਪੂਰਨ		store,
    ਪੂਰਨ		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = ਨਿਰਾ;
    ਪੂਰਨ		ਗੇੜਾ;

    ਲਈ (ਗੇੜਾ = 0; ; ++ਗੇੜਾ)
    {
	cmdline_ਤਾਰਾ = TRUE;
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	p1 = getcmdline_prompt(NUL, ਗੇੜਾ == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		ਨਿਰਾ);
	cmdline_ਤਾਰਾ = FALSE;

	ਜੇ (p1 == ਨਿਰਾ)
	    ਰੋਕੋ;

	ਜੇ (ਗੇੜਾ == twice)
	{
	    ਜੇ (p2 != ਨਿਰਾ && STRCMP(p1, p2) != 0)
	    {
		ਸੁਨੇਹਾ(_("Keys don't match!"));
		crypt_free_ਕੁੰਜੀ(p1);
		crypt_free_ਕੁੰਜੀ(p2);
		p2 = ਨਿਰਾ;
		ਗੇੜਾ = -1;		// do it again
		ਜਾਰੀ;
	    }

	    ਜੇ (store)
	    {
		ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_ਕੁੰਜੀ(p1);
		p1 = curbuf->b_p_ਕੁੰਜੀ;
		crypt_check_swapfile_curbuf();
	    }
	    ਰੋਕੋ;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    ਜੇ (!crypt_ਵਿਧੀ_is_sodium(crypt_get_ਵਿਧੀ_nr(curbuf)))
    {
	ਜੇ (ਸੁਨੇਹਾ_didout)
	    ਸੁਨੇਹਾ_putchar('\n');
	need_wait_return = FALSE;
	ਸੁਨੇਹਾ_didout = FALSE;
    }

    crypt_free_ਕੁੰਜੀ(p2);
    ਵਾਪਸ p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    ਖਾਲੀ
crypt_append_ਸੁਨੇਹਾ(
    buf_T *buf)
{
    ਜੇ (crypt_get_ਵਿਧੀ_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    ਨਹੀਂਤਾਂ
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    ਸਥਿਰ ਪੂਰਨ
crypt_sodium_init_(
    cryptstate_T	*state ਅਣਵਰਤਿਆ,
    char_u		*ਕੁੰਜੀ ਅਣਵਰਤਿਆ,
    crypt_arg_T		*arg ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    ਪੂਰਨ			retval = 0;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ	opslimit;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ	memlimit;
    ਪੂਰਨ			alg;

    ਜੇ (sodium_init() < 0)
	ਵਾਪਸ FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(ਆਕਾਰ(sodium_state_T));
    sodium_memzero(sd_state, ਆਕਾਰ(sodium_state_T));

    ਜੇ ((state->ਵਿਧੀ_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->ਵਿਧੀ_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# ਜੇ 0
	// For testing
	ਜੇ (state->ਵਿਧੀ_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	ਜੇ (crypto_pwhash(dkey, ਆਕਾਰ(dkey), (ਅਚਲ ਅੱਖਰ *)ਕੁੰਜੀ, STRLEN(ਕੁੰਜੀ),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    ਵਾਪਸ FAIL;
	}
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(sd_state->ਕੁੰਜੀ, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ਕੁੰਜੀ, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ਕੁੰਜੀ, STRLEN(ਕੁੰਜੀ));

	ਜੇ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    ਵਾਪਸ FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	ਜੇ (state->ਵਿਧੀ_nr == CRYPT_M_SOD2 && arg->cat_ਜੋੜੋ != ਨਿਰਾ)
	{
	    char_u	ਬਫਰ[20];
	    char_u	*p = ਬਫਰ;
	    vim_memset(ਬਫਰ, 0, 20);

	    crypt_long_long_ਮੰਜ਼ਿਲ_char(opslimit, p);
	    p += ਆਕਾਰ(opslimit);

	    crypt_long_long_ਮੰਜ਼ਿਲ_char(memlimit, p);
	    p += ਆਕਾਰ(memlimit);

	    crypt_int_ਮੰਜ਼ਿਲ_char(alg, p);
	    ਸਮ੍ਰਿਤੀ_ਨਕਲ(arg->cat_ਜੋੜੋ, ਬਫਰ, ਆਕਾਰ(opslimit) + ਆਕਾਰ(memlimit) + ਆਕਾਰ(alg));
	}
    }
    ਨਹੀਂਤਾਂ
    {
	char_u	ਬਫਰ[20];
	char_u	*p = ਬਫਰ;
	vim_memset(ਬਫਰ, 0, 20);
	ਪੂਰਨ	ਮਾਪ = ਆਕਾਰ(opslimit) +
	    ਆਕਾਰ(memlimit) + ਆਕਾਰ(alg);

	// Reading parameters from file
	ਜੇ (arg->cat_ਜੋੜੋ_len < ਮਾਪ)
	{
	    sodium_free(sd_state);
	    ਵਾਪਸ FAIL;
	}

	// derive the key from the file header
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(p, arg->cat_ਜੋੜੋ, ਮਾਪ);
	arg->cat_ਜੋੜੋ += ਮਾਪ;

	opslimit = crypt_char_ਮੰਜ਼ਿਲ_long_long(p);
	p += ਆਕਾਰ(opslimit);
	memlimit = crypt_char_ਮੰਜ਼ਿਲ_long_long(p);
	p += ਆਕਾਰ(memlimit);
	alg = crypt_char_ਮੰਜ਼ਿਲ_int(p);
	p += ਆਕਾਰ(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	ਜੇ (crypto_pwhash(dkey, ਆਕਾਰ(dkey), (ਅਚਲ ਅੱਖਰ *)ਕੁੰਜੀ, STRLEN(ਕੁੰਜੀ),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    ਵਾਪਸ FAIL;
	}
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(sd_state->ਕੁੰਜੀ, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ਕੁੰਜੀ, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ਕੁੰਜੀ, STRLEN(ਕੁੰਜੀ));

	ਜੇ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    ਵਾਪਸ FAIL;
	}
    }
    sd_state->ਗਿਣਤੀ_2 = 0;
    state->ਵਿਧੀ_state = sd_state;

    ਵਾਪਸ OK;
#ਨਹੀਂਤਾਂ
    emsg(_(e_libsodium_not_built_ਵਿੱਚ));
    ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#ਜੇ 0  // Currently unused
    ਖਾਲੀ
crypt_sodium_encode(
    cryptstate_T *state ਅਣਵਰਤਿਆ,
    char_u	*from ਅਣਵਰਤਿਆ,
    size_t	len ਅਣਵਰਤਿਆ,
    char_u	*ਮੰਜ਼ਿਲ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		ਆਖਰੀ ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->ਵਿਧੀ_state;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ  ਟੈਗ = ਆਖਰੀ
			? crypto_secretstream_xchacha20poly1305_ਟੈਗ_FINAL  : 0;

    ਜੇ (sod_st->ਗਿਣਤੀ_2 == 0)
    {
	ਜੇ (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_ਸਿਰਲੇਖ));
	    ਵਾਪਸ;
	}
	crypto_secretstream_xchacha20poly1305_init_ਧੱਕੋ(&sod_st->state,
							      ਮੰਜ਼ਿਲ, sod_st->ਕੁੰਜੀ);
	ਮੰਜ਼ਿਲ += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ਜੇ (sod_st->ਗਿਣਤੀ_2 && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_ਬਫਰ));
	ਵਾਪਸ;
    }

    crypto_secretstream_xchacha20poly1305_ਧੱਕੋ(&sod_st->state, ਮੰਜ਼ਿਲ, ਨਿਰਾ,
						      from, len, ਨਿਰਾ, 0, ਟੈਗ);

    sod_st->ਗਿਣਤੀ_2++;
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#ਜੇ 0  // Currently unused
    ਖਾਲੀ
crypt_sodium_decode(
    cryptstate_T *state ਅਣਵਰਤਿਆ,
    char_u	*from ਅਣਵਰਤਿਆ,
    size_t	len ਅਣਵਰਤਿਆ,
    char_u	*ਮੰਜ਼ਿਲ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		ਆਖਰੀ ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->ਵਿਧੀ_state;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ  ਟੈਗ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ buf_len;
    char_u *p1 = from;
    char_u *p2 = ਮੰਜ਼ਿਲ;
    char_u *buf_ਬਾਹਰਲਾ;

    ਜੇ (sod_st->ਗਿਣਤੀ_2 == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_ਸਿਰਲੇਖ));
	ਵਾਪਸ;
    }

    buf_ਬਾਹਰਲਾ = (char_u *)alloc(len);

    ਜੇ (buf_ਬਾਹਰਲਾ == ਨਿਰਾ)
    {
	emsg(_(e_libsodium_cannot_allocate_ਬਫਰ));
	ਵਾਪਸ;
    }
    ਜੇ (sod_st->ਗਿਣਤੀ_2 == 0)
    {
	ਜੇ (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->ਕੁੰਜੀ) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_ਸਿਰਲੇਖ_incomplete));
	    ਜਾਓ fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	ਜੇ (p1 == p2)
	    ਮੰਜ਼ਿਲ += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ਜੇ (sod_st->ਗਿਣਤੀ_2 && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_ਬਫਰ));
	ਜਾਓ fail;
    }
    ਜੇ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_ਬਾਹਰਲਾ, &buf_len, &ਟੈਗ, from, len, ਨਿਰਾ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	ਜਾਓ fail;
    }
    sod_st->ਗਿਣਤੀ_2++;

    ਜੇ (ਟੈਗ == crypto_secretstream_xchacha20poly1305_ਟੈਗ_FINAL && !ਆਖਰੀ)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	ਜਾਓ fail;
    }
    ਜੇ (p1 == p2)
	mch_memmove(p2, buf_ਬਾਹਰਲਾ, buf_len);

fail:
    vim_free(buf_ਬਾਹਰਲਾ);
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ਸਥਿਰ ਲੰਮਾ
crypt_sodium_ਬਫਰ_encode(
    cryptstate_T *state ਅਣਵਰਤਿਆ,
    char_u	*from ਅਣਵਰਤਿਆ,
    size_t	len ਅਣਵਰਤਿਆ,
    char_u	**buf_ਬਾਹਰਲਾ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		ਆਖਰੀ ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ	ਬਾਹਰਲਾ_len;
    char_u		*ptr;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ	ਟੈਗ = ਆਖਰੀ
			? crypto_secretstream_xchacha20poly1305_ਟੈਗ_FINAL  : 0;
    ਪੂਰਨ			ਲੰਮਾਈ;
    sodium_state_T	*sod_st = state->ਵਿਧੀ_state;
    ਪੂਰਨ			ਪਹਿਲਾ = (sod_st->ਗਿਣਤੀ_2 == 0);

    ਲੰਮਾਈ = (ਪੂਰਨ)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (ਪਹਿਲਾ ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_ਬਾਹਰਲਾ = alloc_clear(ਲੰਮਾਈ);
    ਜੇ (*buf_ਬਾਹਰਲਾ == ਨਿਰਾ)
    {
	emsg(_(e_libsodium_cannot_allocate_ਬਫਰ));
	ਵਾਪਸ -1;
    }
    ptr = *buf_ਬਾਹਰਲਾ;

    ਜੇ (ਪਹਿਲਾ)
    {
	crypto_secretstream_xchacha20poly1305_init_ਧੱਕੋ(&sod_st->state,
		ptr, sod_st->ਕੁੰਜੀ);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ਧੱਕੋ(&sod_st->state, ptr,
	    &ਬਾਹਰਲਾ_len, from, len, ਨਿਰਾ, 0, ਟੈਗ);

    sod_st->ਗਿਣਤੀ_2++;
    ਵਾਪਸ ਬਾਹਰਲਾ_len + (ਪਹਿਲਾ
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ -1;
#ਜੇ_ਅੰਤ
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ਸਥਿਰ ਲੰਮਾ
crypt_sodium_ਬਫਰ_decode(
    cryptstate_T *state ਅਣਵਰਤਿਆ,
    char_u	*from ਅਣਵਰਤਿਆ,
    size_t	len ਅਣਵਰਤਿਆ,
    char_u	**buf_ਬਾਹਰਲਾ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		ਆਖਰੀ ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->ਵਿਧੀ_state;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ  ਟੈਗ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ ਬਾਹਰਲਾ_len;

    ਜੇ (sod_st->ਗਿਣਤੀ_2 == 0
	    && state->ਵਿਧੀ_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].ਜੋੜੋ_len;

    *buf_ਬਾਹਰਲਾ = alloc_clear(len);
    ਜੇ (*buf_ਬਾਹਰਲਾ == ਨਿਰਾ)
    {
	emsg(_(e_libsodium_cannot_allocate_ਬਫਰ));
	ਵਾਪਸ -1;
    }

    ਜੇ (sod_st->ਗਿਣਤੀ_2 == 0)
    {
	ਜੇ (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->ਕੁੰਜੀ) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_ਸਿਰਲੇਖ_incomplete));
	    ਵਾਪਸ -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->ਗਿਣਤੀ_2++;
    }
    ਜੇ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_ਬਾਹਰਲਾ, &ਬਾਹਰਲਾ_len, &ਟੈਗ, from, len, ਨਿਰਾ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	ਵਾਪਸ -1;
    }

    ਜੇ (ਟੈਗ == crypto_secretstream_xchacha20poly1305_ਟੈਗ_FINAL && !ਆਖਰੀ)
	emsg(_(e_libsodium_decryption_failed_premature));
    ਵਾਪਸ (ਲੰਮਾ) ਬਾਹਰਲਾ_len;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ -1;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_SODIUM)
    ਖਾਲੀ
crypt_sodium_ਤਾਲਾ_ਕੁੰਜੀ(char_u *ਕੁੰਜੀ)
{
    ਜੇ (sodium_init() >= 0)
	sodium_mlock(ਕੁੰਜੀ, STRLEN(ਕੁੰਜੀ));
}

    ਪੂਰਨ
crypt_sodium_munlock(ਖਾਲੀ *ਅਚਲ addr, ਅਚਲ size_t len)
{
    ਵਾਪਸ sodium_munlock(addr, len);
}

    ਖਾਲੀ
crypt_sodium_randombytes_buf(ਖਾਲੀ *ਅਚਲ buf, ਅਚਲ size_t ਮਾਪ)
{
    randombytes_buf(buf, ਮਾਪ);
}

    ਪੂਰਨ
crypt_sodium_init(ਖਾਲੀ)
{
    ਵਾਪਸ sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(ਖਾਲੀ)
{
    ਵਾਪਸ randombytes_random();
}

# ਜੇ defined(FEAT_EVAL)
    ਸਥਿਰ ਖਾਲੀ
crypt_sodium_report_hash_params(
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ opslimit,
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ ops_def,
	size_t memlimit,
	size_t mem_def,
	ਪੂਰਨ alg,
	ਪੂਰਨ alg_def)
{
    ਜੇ (p_verbose > 0)
    {
	verbose_enter();
	ਜੇ (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	ਨਹੀਂਤਾਂ
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	ਜੇ (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)memlimit);
	ਨਹੀਂਤਾਂ
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)memlimit);
	ਜੇ (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	ਨਹੀਂਤਾਂ
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ਸਥਿਰ ਖਾਲੀ
crypt_long_long_ਮੰਜ਼ਿਲ_char(ਲੰਮਾ ਲੰਮਾ n, char_u *s)
{
    ਪੂਰਨ i;
    ਲਈ (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ)n >> 8;
    }
}

    ਸਥਿਰ ਖਾਲੀ
crypt_int_ਮੰਜ਼ਿਲ_char(ਪੂਰਨ n, char_u *s)
{
    ਪੂਰਨ i;
    ਲਈ (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ)n >> 8;
    }
}

    ਸਥਿਰ ਲੰਮਾ ਲੰਮਾ
crypt_char_ਮੰਜ਼ਿਲ_long_long(char_u *s)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ    retval = 0;
    ਪੂਰਨ i;
    ਲਈ (i = 7; i >= 0; i--)
    {
	ਜੇ (i == 7)
	    retval = s[i];
	ਨਹੀਂਤਾਂ
	    retval |= s[i];
	ਜੇ (i > 0)
	    retval <<= 8;
    }
    ਵਾਪਸ retval;
}

    ਸਥਿਰ ਪੂਰਨ
crypt_char_ਮੰਜ਼ਿਲ_int(char_u *s)
{
    ਪੂਰਨ retval = 0;
    ਪੂਰਨ i;

    ਲਈ (i = 3; i >= 0; i--)
    {
	ਜੇ (i == 3)
	    retval = s[i];
	ਨਹੀਂਤਾਂ
	    retval |= s[i];
	ਜੇ (i > 0)
	    retval <<= 8;
    }
    ਵਾਪਸ retval;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ // FEAT_CRYPT
