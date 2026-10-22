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

#ಯದಿ defined(FEAT_CRYPT)
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ವರ್ಣ	*name;		// encryption name as used in 'cryptmethod'
    ವರ್ಣ	*magic;		// magic bytes stored in file header
    ಪೂರ್ಣಾಙ್ಕ		salt_len;	// length of salt, or 0 when not using salt
    ಪೂರ್ಣಾಙ್ಕ		seed_len;	// length of seed, or 0 when not using seed
    ಪೂರ್ಣಾಙ್ಕ		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    ಪೂರ್ಣಾಙ್ಕ		works_inplace;	// encryption/decryption can be done in-place
#endif
    ಪೂರ್ಣಾಙ್ಕ		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    ಪೂರ್ಣಾಙ್ಕ (*self_test_fn)(ಶೂನ್ಯ);

    // Function pointer for initializing encryption/decryption.
    ಪೂರ್ಣಾಙ್ಕ (* init_fn)(cryptstate_T *state, char_u *ಕೀಲಿ, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    ಶೂನ್ಯ (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, ಪೂರ್ಣಾಙ್ಕ last);
    ಶೂನ್ಯ (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, ಪೂರ್ಣಾಙ್ಕ last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, ಪೂರ್ಣಾಙ್ಕ last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, ಪೂರ್ಣಾಙ್ಕ last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    ಶೂನ್ಯ (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, ಪೂರ್ಣಾಙ್ಕ last);
    ಶೂನ್ಯ (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, ಪೂರ್ಣಾಙ್ಕ last);
} cryptmethod_T;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ crypt_sodium_init_(cryptstate_T *state, char_u *ಕೀಲಿ, crypt_arg_T *arg);
ಸ್ಥಿರ long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, ಪೂರ್ಣಾಙ್ಕ last);
ಸ್ಥಿರ long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, ಪೂರ್ಣಾಙ್ಕ last);
#ಯದಿ defined(FEAT_SODIUM)
ಸ್ಥಿರ ಶೂನ್ಯ crypt_long_long_to_char(long long n, char_u *s);
ಸ್ಥಿರ ಶೂನ್ಯ crypt_int_to_char(ಪೂರ್ಣಾಙ್ಕ n, char_u *s);
ಸ್ಥಿರ long long crypt_char_to_long_long(char_u *s);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ crypt_char_to_int(char_u *s);
#endif
#ಯದಿ defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ಸ್ಥಿರ ಶೂನ್ಯ crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, ಪೂರ್ಣಾಙ್ಕ alg, ಪೂರ್ಣಾಙ್ಕ alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
ಸ್ಥಿರ cryptmethod_T cryptmethods[CRYPT_M_ಎಣಿಕೆ] = {
    // PK_Zip; very weak
    {
	"zip",
	"VimCrypt~01!",
	0,
	0,
	0,
#ifdef CRYPT_NOT_INPLACE
	TRUE,
#endif
	FALSE,
	ರಿಕ್ತ,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	ರಿಕ್ತ, ರಿಕ್ತ,
	crypt_zip_encode, crypt_zip_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; implementation issues.
    {
	"blowfish",
	"VimCrypt~02!",
	8,
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	TRUE,
#endif
	FALSE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	ರಿಕ್ತ, ರಿಕ್ತ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; fixed.
    {
	"blowfish2",
	"VimCrypt~03!",
	8,
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	TRUE,
#endif
	TRUE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	ರಿಕ್ತ, ರಿಕ್ತ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ಅನ್ಯಥಾ
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	ರಿಕ್ತ,
	crypt_sodium_init_,
	ರಿಕ್ತ, ರಿಕ್ತ,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	ರಿಕ್ತ, ರಿಕ್ತ,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ಅನ್ಯಥಾ
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	ರಿಕ್ತ,
	crypt_sodium_init_,
	ರಿಕ್ತ, ರಿಕ್ತ,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	ರಿಕ್ತ, ರಿಕ್ತ,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#ಯದಿ defined(FEAT_SODIUM)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    size_t	    ಎಣಿಕೆ;
    unsigned ವರ್ಣ   ಕೀಲಿ[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define ಚಿಹ್ನೆ_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  ಅನ್ಯಥಾ
#   error Dynamic loading of libsodium is not supported ಪ್ರತಿ now.
//#   define HINSTANCE void*
//#   define SODIUM_PROC void*
//#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
//#   define symbol_from_dll dlsym
//#   define close_dll dlclose
//#   define load_dll_error dlerror
#  endif

#  define sodium_init	    load_sodium
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_sodium_init)(ಶೂನ್ಯ) = ರಿಕ್ತ;
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_sodium_free)(ಶೂನ್ಯ *) = ರಿಕ್ತ;
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_sodium_malloc)(ಧ್ರುವ size_t) = ರಿಕ್ತ;
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_sodium_memzero)(ಶೂನ್ಯ * ಧ್ರುವ, ಧ್ರುವ size_t) = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_sodium_mlock)(ಶೂನ್ಯ * ಧ್ರುವ, ಧ್ರುವ size_t) = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_sodium_munlock)(ಶೂನ್ಯ * ಧ್ರುವ, ಧ್ರುವ size_t) = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_crypto_secretstream_xchacha20poly1305_init_ತಳ್ಳು)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned ವರ್ಣ [],
    ಧ್ರುವ unsigned ವರ್ಣ []) = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_crypto_secretstream_xchacha20poly1305_ತಳ್ಳು)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned ವರ್ಣ *c, unsigned long long *clen_p,
    ಧ್ರುವ unsigned ವರ್ಣ *m, unsigned long long mlen,
    ಧ್ರುವ unsigned ವರ್ಣ *ad, unsigned long long adlen, unsigned ವರ್ಣ tag)
    = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ಧ್ರುವ unsigned ವರ್ಣ [],
    ಧ್ರುವ unsigned ವರ್ಣ []) = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned ವರ್ಣ *m, unsigned long long *mlen_p, unsigned ವರ್ಣ *tag_p,
    ಧ್ರುವ unsigned ವರ್ಣ *c, unsigned long long clen,
    ಧ್ರುವ unsigned ವರ್ಣ *ad, unsigned long long adlen) = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_crypto_pwhash)(unsigned ವರ್ಣ * ಧ್ರುವ out,
    unsigned long long outlen,
    ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ passwd, unsigned long long passwdlen,
    ಧ್ರುವ unsigned ವರ್ಣ * ಧ್ರುವ salt,
    unsigned long long opslimit, size_t memlimit, ಪೂರ್ಣಾಙ್ಕ alg)
    = ರಿಕ್ತ;
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_randombytes_buf)(ಶೂನ್ಯ * ಧ್ರುವ buf, ಧ್ರುವ size_t size);
ಸ್ಥಿರ uint32_t (*dll_randombytes_random)(ಶೂನ್ಯ);

ಸ್ಥಿರ ಸಂರಚನಾ {
    ಧ್ರುವ ವರ್ಣ *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
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
    {ರಿಕ್ತ, ರಿಕ್ತ}
};

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sodium_runtime_link_init(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಸ್ಥಿರ HINSTANCE hsodium = ರಿಕ್ತ;
    ಧ್ರುವ ವರ್ಣ *libname = DYNAMIC_SODIUM_DLL;
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (hsodium != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    hsodium = load_dll(libname);
    ಯದಿ (hsodium == ರಿಕ್ತ)
    {
	ಯದಿ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತಿ (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	ಯದಿ ((*sodium_funcname_table[i].ptr = ಚಿಹ್ನೆ_from_dll(hsodium,
			sodium_funcname_table[i].name)) == ರಿಕ್ತ)
	{
	    close_dll(hsodium);
	    hsodium = ರಿಕ್ತ;
	    ಯದಿ (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
load_sodium(ಶೂನ್ಯ)
{
    ಯದಿ (sodium_runtime_link_init(TRUE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ dll_sodium_init();
}
# endif

# ಯದಿ defined(DYNAMIC_SODIUM)
    ಪೂರ್ಣಾಙ್ಕ
sodium_enabled(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಪ್ರತ್ಯಾಗಮ sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_method_nr_from_name(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < CRYPT_M_ಎಣಿಕೆ; ++i)
	ಯದಿ (STRCMP(name, cryptmethods[i].name) == 0)
	    ಪ್ರತ್ಯಾಗಮ i;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_method_nr_from_magic(ವರ್ಣ *ptr, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (len < CRYPT_MAGIC_LEN)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಪ್ರತಿ (i = 0; i < CRYPT_M_ಎಣಿಕೆ; i++)
	ಯದಿ (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    ಪ್ರತ್ಯಾಗಮ i;

    i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(crypt_magic_head);
    ಯದಿ (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    ಪ್ರತ್ಯಾಗಮ -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_works_inplace(cryptstate_T *state)
{
    ಪ್ರತ್ಯಾಗಮ cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_get_method_nr(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_method_is_sodium(ಪೂರ್ಣಾಙ್ಕ method)
{
    ಪ್ರತ್ಯಾಗಮ method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_whole_undofile(ಪೂರ್ಣಾಙ್ಕ method_nr)
{
    ಪ್ರತ್ಯಾಗಮ cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_get_header_len(ಪೂರ್ಣಾಙ್ಕ method_nr)
{
    ಪ್ರತ್ಯಾಗಮ CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#ಯದಿ defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_get_max_header_len(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ max = 0;
    ಪೂರ್ಣಾಙ್ಕ temp = 0;

    ಪ್ರತಿ (i = 0; i < CRYPT_M_ಎಣಿಕೆ; ++i)
    {
	temp = crypt_get_header_len(i);
	ಯದಿ (temp > max)
	    max = temp;
    }
    ಪ್ರತ್ಯಾಗಮ max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    ಶೂನ್ಯ
crypt_set_cm_option(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
crypt_self_test(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ method_nr = crypt_get_method_nr(curbuf);

    ಯದಿ (cryptmethods[method_nr].self_test_fn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    ಪೂರ್ಣಾಙ್ಕ		method_nr,
    char_u	*ಕೀಲಿ,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    ಯದಿ (state == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ state;

    state->method_nr = method_nr;
    ಯದಿ (cryptmethods[method_nr].init_fn(state, ಕೀಲಿ, crypt_arg) == FAIL)
    {
	vim_free(state);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    ಪೂರ್ಣಾಙ್ಕ		method_nr,
    char_u	*ಕೀಲಿ,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    ಯದಿ (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    ಯದಿ (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    ಯದಿ (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    ಪ್ರತ್ಯಾಗಮ crypt_create(method_nr, ಕೀಲಿ, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *ಕೀಲಿ)
{
    ಪೂರ್ಣಾಙ್ಕ		method_nr;
    ಪೂರ್ಣಾಙ್ಕ		header_len;
    ವರ್ಣ	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    ಯದಿ (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    ಯದಿ (method_nr < 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    header_len = crypt_get_header_len(method_nr);
    ಯದಿ ((buffer = alloc(header_len)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    ಯದಿ (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    state = crypt_create_from_header(method_nr, ಕೀಲಿ, buffer);
    vim_free(buffer);
    ಪ್ರತ್ಯಾಗಮ state;
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
    ಪೂರ್ಣಾಙ್ಕ	    method_nr,
    char_u  *ಕೀಲಿ,
    char_u  **header,
    ಪೂರ್ಣಾಙ್ಕ	    *header_len)
{
    ಪೂರ್ಣಾಙ್ಕ	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    ಯದಿ (*header == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    ಯದಿ (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	ಯದಿ (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	ಯದಿ (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	ಯದಿ (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	ಯದಿ (sodium_init() >= 0)
	{
	    ಯದಿ (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    ಯದಿ (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	ಅನ್ಯಥಾ
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, ಕೀಲಿ, &arg);
    ಯದಿ (state == ರಿಕ್ತ)
	VIM_CLEAR(*header);
    ಪ್ರತ್ಯಾಗಮ state;
}

/*
 * Free the crypt state.
 */
    ಶೂನ್ಯ
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    ಯದಿ (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->ಕೀಲಿ,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, ಪರಿಮಾಣ(sodium_state_T));
	sodium_free(state->method_state);
    }
    ಅನ್ಯಥಾ
#endif
	vim_free(state->method_state);
    vim_free(state);
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Encode "from[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    long
crypt_encode_alloc(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	**newptr,
    ಪೂರ್ಣಾಙ್ಕ		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    ಯದಿ (method->encode_buffer_fn != ರಿಕ್ತ)
	// Has buffer function, pass through.
	ಪ್ರತ್ಯಾಗಮ method->encode_buffer_fn(state, from, len, newptr, last);
    ಯದಿ (len == 0)
	// Not buffering, just return EOF.
	ಪ್ರತ್ಯಾಗಮ (long)len;

    *newptr = alloc(len + 50);
    ಯದಿ (*newptr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    method->encode_fn(state, from, len, *newptr, last);
    ಪ್ರತ್ಯಾಗಮ (long)len;
}

/*
 * Decrypt "ptr[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    long
crypt_decode_alloc(
    cryptstate_T *state,
    char_u	*ptr,
    long	len,
    char_u      **newptr,
    ಪೂರ್ಣಾಙ್ಕ		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    ಯದಿ (method->decode_buffer_fn != ರಿಕ್ತ)
	// Has buffer function, pass through.
	ಪ್ರತ್ಯಾಗಮ method->decode_buffer_fn(state, ptr, len, newptr, last);

    ಯದಿ (len == 0)
	// Not buffering, just return EOF.
	ಪ್ರತ್ಯಾಗಮ len;

    *newptr = alloc(len);
    ಯದಿ (*newptr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    ಪ್ರತ್ಯಾಗಮ len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    ಶೂನ್ಯ
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    ಪೂರ್ಣಾಙ್ಕ		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#ಯದಿ 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    ಶೂನ್ಯ
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    ಪೂರ್ಣಾಙ್ಕ		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    ಶೂನ್ಯ
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    ಪೂರ್ಣಾಙ್ಕ		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    ಶೂನ್ಯ
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    ಪೂರ್ಣಾಙ್ಕ		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    ಶೂನ್ಯ
crypt_free_ಕೀಲಿ(char_u *ಕೀಲಿ)
{
    // Create a safe memset which cannot be optimized away by compiler
    ಸ್ಥಿರ ಶೂನ್ಯ *(* ಚಞ್ಚಲ vim_memset_safe)(ಶೂನ್ಯ *s, ಪೂರ್ಣಾಙ್ಕ c, size_t n) =
	memset;
    ಯದಿ (ಕೀಲಿ != ರಿಕ್ತ)
    {
#ifdef FEAT_SODIUM
	ಯದಿ (sodium_init() >= 0)
	    sodium_memzero(ಕೀಲಿ, STRLEN(ಕೀಲಿ));
	ಅನ್ಯಥಾ
#endif
	    vim_memset_safe(ಕೀಲಿ, 0, STRLEN(ಕೀಲಿ));
	vim_free(ಕೀಲಿ);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    ಶೂನ್ಯ
crypt_check_method(ಪೂರ್ಣಾಙ್ಕ method)
{
    ಯದಿ (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
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
#ifdef FEAT_SODIUM
    ಪೂರ್ಣಾಙ್ಕ method = crypt_get_method_nr(curbuf);
    ಯದಿ (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"swf", 0, ರಿಕ್ತ, OPT_LOCAL);
	ಸಂದೇಶ_scroll = TRUE;
	ಸಂದೇಶ(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    ಶೂನ್ಯ
crypt_check_current_method(ಶೂನ್ಯ)
{
    crypt_check_method(crypt_get_method_nr(curbuf));
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
    ಪೂರ್ಣಾಙ್ಕ		store,
    ಪೂರ್ಣಾಙ್ಕ		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		round;

    ಪ್ರತಿ (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = ಸಂದೇಶ_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		ರಿಕ್ತ);
	cmdline_star = FALSE;

	ಯದಿ (p1 == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	ಯದಿ (round == twice)
	{
	    ಯದಿ (p2 != ರಿಕ್ತ && STRCMP(p1, p2) != 0)
	    {
		ಸಂದೇಶ(_("Keys don't match!"));
		crypt_free_ಕೀಲಿ(p1);
		crypt_free_ಕೀಲಿ(p2);
		p2 = ರಿಕ್ತ;
		round = -1;		// do it again
		ಅನುವರ್ತ;
	    }

	    ಯದಿ (store)
	    {
		set_option_ಮೌಲ್ಯ_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_ಕೀಲಿ(p1);
		p1 = curbuf->b_p_ಕೀಲಿ;
		crypt_check_swapfile_curbuf();
	    }
	    ಭಙ್ಗ;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    ಯದಿ (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	ಯದಿ (ಸಂದೇಶ_didout)
	    ಸಂದೇಶ_putchar('\n');
	need_wait_return = FALSE;
	ಸಂದೇಶ_didout = FALSE;
    }

    crypt_free_ಕೀಲಿ(p2);
    ಪ್ರತ್ಯಾಗಮ p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    ಶೂನ್ಯ
crypt_append_ಸಂದೇಶ(
    buf_T *buf)
{
    ಯದಿ (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    ಅನ್ಯಥಾ
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*ಕೀಲಿ UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned ವರ್ಣ	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    ಪೂರ್ಣಾಙ್ಕ			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    ಪೂರ್ಣಾಙ್ಕ			alg;

    ಯದಿ (sodium_init() < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(ಪರಿಮಾಣ(sodium_state_T));
    sodium_memzero(sd_state, ಪರಿಮಾಣ(sodium_state_T));

    ಯದಿ ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# ಯದಿ 0
	// For testing
	ಯದಿ (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	ಯದಿ (crypto_pwhash(dkey, ಪರಿಮಾಣ(dkey), (ಧ್ರುವ ವರ್ಣ *)ಕೀಲಿ, STRLEN(ಕೀಲಿ),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	memcpy(sd_state->ಕೀಲಿ, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ಕೀಲಿ, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ಕೀಲಿ, STRLEN(ಕೀಲಿ));

	ಯದಿ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	ಯದಿ (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != ರಿಕ್ತ)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += ಪರಿಮಾಣ(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += ಪರಿಮಾಣ(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, ಪರಿಮಾಣ(opslimit) + ಪರಿಮಾಣ(memlimit) + ಪರಿಮಾಣ(alg));
	}
    }
    ಅನ್ಯಥಾ
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	ಪೂರ್ಣಾಙ್ಕ	size = ಪರಿಮಾಣ(opslimit) +
	    ಪರಿಮಾಣ(memlimit) + ಪರಿಮಾಣ(alg);

	// Reading parameters from file
	ಯದಿ (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += ಪರಿಮಾಣ(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += ಪರಿಮಾಣ(memlimit);
	alg = crypt_char_to_int(p);
	p += ಪರಿಮಾಣ(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	ಯದಿ (crypto_pwhash(dkey, ಪರಿಮಾಣ(dkey), (ಧ್ರುವ ವರ್ಣ *)ಕೀಲಿ, STRLEN(ಕೀಲಿ),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	memcpy(sd_state->ಕೀಲಿ, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ಕೀಲಿ, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ಕೀಲಿ, STRLEN(ಕೀಲಿ));

	ಯದಿ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    sd_state->ಎಣಿಕೆ = 0;
    state->method_state = sd_state;

    ಪ್ರತ್ಯಾಗಮ OK;
#ಅನ್ಯಥಾ
    emsg(_(e_libsodium_not_built_in));
    ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#ಯದಿ 0  // Currently unused
    ಶೂನ್ಯ
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned ವರ್ಣ  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    ಯದಿ (sod_st->ಎಣಿಕೆ == 0)
    {
	ಯದಿ (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	crypto_secretstream_xchacha20poly1305_init_ತಳ್ಳು(&sod_st->state,
							      to, sod_st->ಕೀಲಿ);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ಯದಿ (sod_st->ಎಣಿಕೆ && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	ಪ್ರತ್ಯಾಗಮ;
    }

    crypto_secretstream_xchacha20poly1305_ತಳ್ಳು(&sod_st->state, to, ರಿಕ್ತ,
						      from, len, ರಿಕ್ತ, 0, tag);

    sod_st->ಎಣಿಕೆ++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#ಯದಿ 0  // Currently unused
    ಶೂನ್ಯ
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned ವರ್ಣ  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    ಯದಿ (sod_st->ಎಣಿಕೆ == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	ಪ್ರತ್ಯಾಗಮ;
    }

    buf_out = (char_u *)alloc(len);

    ಯದಿ (buf_out == ರಿಕ್ತ)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (sod_st->ಎಣಿಕೆ == 0)
    {
	ಯದಿ (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->ಕೀಲಿ) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    ಗಚ್ಛ fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	ಯದಿ (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ಯದಿ (sod_st->ಎಣಿಕೆ && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	ಗಚ್ಛ fail;
    }
    ಯದಿ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, ರಿಕ್ತ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	ಗಚ್ಛ fail;
    }
    sod_st->ಎಣಿಕೆ++;

    ಯದಿ (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	ಗಚ್ಛ fail;
    }
    ಯದಿ (p1 == p2)
	mch_memmove(p2, buf_out, buf_len);

fail:
    vim_free(buf_out);
# endif
}
#endif

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ಸ್ಥಿರ long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned ವರ್ಣ	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    ಪೂರ್ಣಾಙ್ಕ			length;
    sodium_state_T	*sod_st = state->method_state;
    ಪೂರ್ಣಾಙ್ಕ			first = (sod_st->ಎಣಿಕೆ == 0);

    length = (ಪೂರ್ಣಾಙ್ಕ)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    ಯದಿ (*buf_out == ರಿಕ್ತ)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ptr = *buf_out;

    ಯದಿ (first)
    {
	crypto_secretstream_xchacha20poly1305_init_ತಳ್ಳು(&sod_st->state,
		ptr, sod_st->ಕೀಲಿ);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ತಳ್ಳು(&sod_st->state, ptr,
	    &out_len, from, len, ರಿಕ್ತ, 0, tag);

    sod_st->ಎಣಿಕೆ++;
    ಪ್ರತ್ಯಾಗಮ out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ಸ್ಥಿರ long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned ವರ್ಣ  tag;
    unsigned long long out_len;

    ಯದಿ (sod_st->ಎಣಿಕೆ == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    ಯದಿ (*buf_out == ರಿಕ್ತ)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (sod_st->ಎಣಿಕೆ == 0)
    {
	ಯದಿ (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->ಕೀಲಿ) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->ಎಣಿಕೆ++;
    }
    ಯದಿ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, ರಿಕ್ತ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    ಪ್ರತ್ಯಾಗಮ (long) out_len;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ -1;
#endif
}

#ಯದಿ defined(FEAT_SODIUM)
    ಶೂನ್ಯ
crypt_sodium_lock_ಕೀಲಿ(char_u *ಕೀಲಿ)
{
    ಯದಿ (sodium_init() >= 0)
	sodium_mlock(ಕೀಲಿ, STRLEN(ಕೀಲಿ));
}

    ಪೂರ್ಣಾಙ್ಕ
crypt_sodium_munlock(ಶೂನ್ಯ *ಧ್ರುವ addr, ಧ್ರುವ size_t len)
{
    ಪ್ರತ್ಯಾಗಮ sodium_munlock(addr, len);
}

    ಶೂನ್ಯ
crypt_sodium_randombytes_buf(ಶೂನ್ಯ *ಧ್ರುವ buf, ಧ್ರುವ size_t size)
{
    randombytes_buf(buf, size);
}

    ಪೂರ್ಣಾಙ್ಕ
crypt_sodium_init(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ randombytes_random();
}

# ಯದಿ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಶೂನ್ಯ
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	ಪೂರ್ಣಾಙ್ಕ alg,
	ಪೂರ್ಣಾಙ್ಕ alg_def)
{
    ಯದಿ (p_verbose > 0)
    {
	verbose_enter();
	ಯದಿ (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	ಅನ್ಯಥಾ
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	ಯದಿ (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	ಅನ್ಯಥಾ
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	ಯದಿ (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	ಅನ್ಯಥಾ
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
crypt_long_long_to_char(long long n, char_u *s)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
crypt_int_to_char(ಪೂರ್ಣಾಙ್ಕ n, char_u *s)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    ಸ್ಥಿರ long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ (i = 7; i >= 0; i--)
    {
	ಯದಿ (i == 7)
	    retval = s[i];
	ಅನ್ಯಥಾ
	    retval |= s[i];
	ಯದಿ (i > 0)
	    retval <<= 8;
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
crypt_char_to_int(char_u *s)
{
    ಪೂರ್ಣಾಙ್ಕ retval = 0;
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 3; i >= 0; i--)
    {
	ಯದಿ (i == 3)
	    retval = s[i];
	ಅನ್ಯಥಾ
	    retval |= s[i];
	ಯದಿ (i > 0)
	    retval <<= 8;
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}
#endif

#endif // FEAT_CRYPT
