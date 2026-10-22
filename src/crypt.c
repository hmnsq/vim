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
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_CRYPT)
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

തരനിർവചനം ഘടന {
    അക്ഷരം	*name;		// encryption name as used in 'cryptmethod'
    അക്ഷരം	*magic;		// magic bytes stored in file header
    പൂർണ്ണസംഖ്യ		salt_len;	// length of salt, or 0 when not using salt
    പൂർണ്ണസംഖ്യ		seed_len;	// length of seed, or 0 when not using seed
    പൂർണ്ണസംഖ്യ		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    പൂർണ്ണസംഖ്യ		works_inplace;	// encryption/decryption can be done in-place
#endif
    പൂർണ്ണസംഖ്യ		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    പൂർണ്ണസംഖ്യ (*self_test_fn)(ശൂന്യം);

    // Function pointer for initializing encryption/decryption.
    പൂർണ്ണസംഖ്യ (* init_fn)(cryptstate_T *state, char_u *താക്കോൽ, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    ശൂന്യം (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, പൂർണ്ണസംഖ്യ last);
    ശൂന്യം (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, പൂർണ്ണസംഖ്യ last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, പൂർണ്ണസംഖ്യ last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, പൂർണ്ണസംഖ്യ last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    ശൂന്യം (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, പൂർണ്ണസംഖ്യ last);
    ശൂന്യം (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, പൂർണ്ണസംഖ്യ last);
} cryptmethod_T;

സ്ഥിതം പൂർണ്ണസംഖ്യ crypt_sodium_init_(cryptstate_T *state, char_u *താക്കോൽ, crypt_arg_T *arg);
സ്ഥിതം long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, പൂർണ്ണസംഖ്യ last);
സ്ഥിതം long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, പൂർണ്ണസംഖ്യ last);
#എങ്കിൽ defined(FEAT_SODIUM)
സ്ഥിതം ശൂന്യം crypt_long_long_to_char(long long n, char_u *s);
സ്ഥിതം ശൂന്യം crypt_int_to_char(പൂർണ്ണസംഖ്യ n, char_u *s);
സ്ഥിതം long long crypt_char_to_long_long(char_u *s);
സ്ഥിതം പൂർണ്ണസംഖ്യ crypt_char_to_int(char_u *s);
#endif
#എങ്കിൽ defined(FEAT_EVAL) && defined(FEAT_SODIUM)
സ്ഥിതം ശൂന്യം crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, പൂർണ്ണസംഖ്യ alg, പൂർണ്ണസംഖ്യ alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
സ്ഥിതം cryptmethod_T cryptmethods[CRYPT_M_എണ്ണം] = {
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
	ശൂന്യ,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	ശൂന്യ, ശൂന്യ,
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
	ശൂന്യ, ശൂന്യ,
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
	ശൂന്യ, ശൂന്യ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#അല്ലെങ്കിൽ
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	ശൂന്യ,
	crypt_sodium_init_,
	ശൂന്യ, ശൂന്യ,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	ശൂന്യ, ശൂന്യ,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#അല്ലെങ്കിൽ
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	ശൂന്യ,
	crypt_sodium_init_,
	ശൂന്യ, ശൂന്യ,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	ശൂന്യ, ശൂന്യ,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#എങ്കിൽ defined(FEAT_SODIUM)
തരനിർവചനം ഘടന {
    size_t	    എണ്ണം;
    unsigned അക്ഷരം   താക്കോൽ[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define ചിഹ്നം_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  അല്ലെങ്കിൽ
#   error Dynamic loading of libsodium is not supported വേണ്ടി now.
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
#  define crypto_secretstream_xchacha20poly1305_init_തള്ളുക \
    dll_crypto_secretstream_xchacha20poly1305_init_തള്ളുക
#  define crypto_secretstream_xchacha20poly1305_തള്ളുക \
    dll_crypto_secretstream_xchacha20poly1305_തള്ളുക
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_sodium_init)(ശൂന്യം) = ശൂന്യ;
സ്ഥിതം ശൂന്യം (*dll_sodium_free)(ശൂന്യം *) = ശൂന്യ;
സ്ഥിതം ശൂന്യം *(*dll_sodium_malloc)(സ്ഥിരം size_t) = ശൂന്യ;
സ്ഥിതം ശൂന്യം (*dll_sodium_memzero)(ശൂന്യം * സ്ഥിരം, സ്ഥിരം size_t) = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_sodium_mlock)(ശൂന്യം * സ്ഥിരം, സ്ഥിരം size_t) = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_sodium_munlock)(ശൂന്യം * സ്ഥിരം, സ്ഥിരം size_t) = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_crypto_secretstream_xchacha20poly1305_init_തള്ളുക)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned അക്ഷരം [],
    സ്ഥിരം unsigned അക്ഷരം []) = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_crypto_secretstream_xchacha20poly1305_തള്ളുക)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned അക്ഷരം *c, unsigned long long *clen_p,
    സ്ഥിരം unsigned അക്ഷരം *m, unsigned long long mlen,
    സ്ഥിരം unsigned അക്ഷരം *ad, unsigned long long adlen, unsigned അക്ഷരം tag)
    = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    സ്ഥിരം unsigned അക്ഷരം [],
    സ്ഥിരം unsigned അക്ഷരം []) = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned അക്ഷരം *m, unsigned long long *mlen_p, unsigned അക്ഷരം *tag_p,
    സ്ഥിരം unsigned അക്ഷരം *c, unsigned long long clen,
    സ്ഥിരം unsigned അക്ഷരം *ad, unsigned long long adlen) = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_crypto_pwhash)(unsigned അക്ഷരം * സ്ഥിരം out,
    unsigned long long outlen,
    സ്ഥിരം അക്ഷരം * സ്ഥിരം passwd, unsigned long long passwdlen,
    സ്ഥിരം unsigned അക്ഷരം * സ്ഥിരം salt,
    unsigned long long opslimit, size_t memlimit, പൂർണ്ണസംഖ്യ alg)
    = ശൂന്യ;
സ്ഥിതം ശൂന്യം (*dll_randombytes_buf)(ശൂന്യം * സ്ഥിരം buf, സ്ഥിരം size_t size);
സ്ഥിതം uint32_t (*dll_randombytes_random)(ശൂന്യം);

സ്ഥിതം ഘടന {
    സ്ഥിരം അക്ഷരം *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_തള്ളുക},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_തള്ളുക},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {ശൂന്യ, ശൂന്യ}
};

    സ്ഥിതം പൂർണ്ണസംഖ്യ
sodium_runtime_link_init(പൂർണ്ണസംഖ്യ verbose)
{
    സ്ഥിതം HINSTANCE hsodium = ശൂന്യ;
    സ്ഥിരം അക്ഷരം *libname = DYNAMIC_SODIUM_DLL;
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (hsodium != ശൂന്യ)
	തിരികെ OK;

    hsodium = load_dll(libname);
    എങ്കിൽ (hsodium == ശൂന്യ)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	തിരികെ FAIL;
    }

    വേണ്ടി (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	എങ്കിൽ ((*sodium_funcname_table[i].ptr = ചിഹ്നം_from_dll(hsodium,
			sodium_funcname_table[i].name)) == ശൂന്യ)
	{
	    close_dll(hsodium);
	    hsodium = ശൂന്യ;
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
load_sodium(ശൂന്യം)
{
    എങ്കിൽ (sodium_runtime_link_init(TRUE) == FAIL)
	തിരികെ -1;
    തിരികെ dll_sodium_init();
}
# endif

# എങ്കിൽ defined(DYNAMIC_SODIUM)
    പൂർണ്ണസംഖ്യ
sodium_enabled(പൂർണ്ണസംഖ്യ verbose)
{
    തിരികെ sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
സ്ഥിതം സ്ഥിരം അക്ഷരം crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    പൂർണ്ണസംഖ്യ
crypt_method_nr_from_name(char_u *name)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < CRYPT_M_എണ്ണം; ++i)
	എങ്കിൽ (STRCMP(name, cryptmethods[i].name) == 0)
	    തിരികെ i;
    തിരികെ 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    പൂർണ്ണസംഖ്യ
crypt_method_nr_from_magic(അക്ഷരം *ptr, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (len < CRYPT_MAGIC_LEN)
	തിരികെ -1;

    വേണ്ടി (i = 0; i < CRYPT_M_എണ്ണം; i++)
	എങ്കിൽ (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    തിരികെ i;

    i = (പൂർണ്ണസംഖ്യ)STRLEN(crypt_magic_head);
    എങ്കിൽ (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    തിരികെ -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    പൂർണ്ണസംഖ്യ
crypt_works_inplace(cryptstate_T *state)
{
    തിരികെ cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    പൂർണ്ണസംഖ്യ
crypt_get_method_nr(buf_T *buf)
{
    തിരികെ crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    പൂർണ്ണസംഖ്യ
crypt_method_is_sodium(പൂർണ്ണസംഖ്യ method)
{
    തിരികെ method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    പൂർണ്ണസംഖ്യ
crypt_whole_undofile(പൂർണ്ണസംഖ്യ method_nr)
{
    തിരികെ cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    പൂർണ്ണസംഖ്യ
crypt_get_header_len(പൂർണ്ണസംഖ്യ method_nr)
{
    തിരികെ CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#എങ്കിൽ defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    പൂർണ്ണസംഖ്യ
crypt_get_max_header_len(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;
    പൂർണ്ണസംഖ്യ max = 0;
    പൂർണ്ണസംഖ്യ temp = 0;

    വേണ്ടി (i = 0; i < CRYPT_M_എണ്ണം; ++i)
    {
	temp = crypt_get_header_len(i);
	എങ്കിൽ (temp > max)
	    max = temp;
    }
    തിരികെ max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    ശൂന്യം
crypt_set_cm_option(buf_T *buf, പൂർണ്ണസംഖ്യ method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    പൂർണ്ണസംഖ്യ
crypt_self_test(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ method_nr = crypt_get_method_nr(curbuf);

    എങ്കിൽ (cryptmethods[method_nr].self_test_fn == ശൂന്യ)
	തിരികെ OK;
    തിരികെ cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    പൂർണ്ണസംഖ്യ		method_nr,
    char_u	*താക്കോൽ,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    എങ്കിൽ (state == ശൂന്യ)
	തിരികെ state;

    state->method_nr = method_nr;
    എങ്കിൽ (cryptmethods[method_nr].init_fn(state, താക്കോൽ, crypt_arg) == FAIL)
    {
	vim_free(state);
	തിരികെ ശൂന്യ;
    }
    തിരികെ state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    പൂർണ്ണസംഖ്യ		method_nr,
    char_u	*താക്കോൽ,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    എങ്കിൽ (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    എങ്കിൽ (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    എങ്കിൽ (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    തിരികെ crypt_create(method_nr, താക്കോൽ, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *താക്കോൽ)
{
    പൂർണ്ണസംഖ്യ		method_nr;
    പൂർണ്ണസംഖ്യ		header_len;
    അക്ഷരം	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    എങ്കിൽ (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	തിരികെ ശൂന്യ;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    എങ്കിൽ (method_nr < 0)
	തിരികെ ശൂന്യ;

    header_len = crypt_get_header_len(method_nr);
    എങ്കിൽ ((buffer = alloc(header_len)) == ശൂന്യ)
	തിരികെ ശൂന്യ;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    എങ്കിൽ (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	തിരികെ ശൂന്യ;
    }

    state = crypt_create_from_header(method_nr, താക്കോൽ, buffer);
    vim_free(buffer);
    തിരികെ state;
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
    പൂർണ്ണസംഖ്യ	    method_nr,
    char_u  *താക്കോൽ,
    char_u  **header,
    പൂർണ്ണസംഖ്യ	    *header_len)
{
    പൂർണ്ണസംഖ്യ	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    എങ്കിൽ (*header == ശൂന്യ)
	തിരികെ ശൂന്യ;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    എങ്കിൽ (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	എങ്കിൽ (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	എങ്കിൽ (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	എങ്കിൽ (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	എങ്കിൽ (sodium_init() >= 0)
	{
	    എങ്കിൽ (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    എങ്കിൽ (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	അല്ലെങ്കിൽ
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, താക്കോൽ, &arg);
    എങ്കിൽ (state == ശൂന്യ)
	VIM_CLEAR(*header);
    തിരികെ state;
}

/*
 * Free the crypt state.
 */
    ശൂന്യം
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    എങ്കിൽ (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->താക്കോൽ,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, വലുപ്പം(sodium_state_T));
	sodium_free(state->method_state);
    }
    അല്ലെങ്കിൽ
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
    പൂർണ്ണസംഖ്യ		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    എങ്കിൽ (method->encode_buffer_fn != ശൂന്യ)
	// Has buffer function, pass through.
	തിരികെ method->encode_buffer_fn(state, from, len, newptr, last);
    എങ്കിൽ (len == 0)
	// Not buffering, just return EOF.
	തിരികെ (long)len;

    *newptr = alloc(len + 50);
    എങ്കിൽ (*newptr == ശൂന്യ)
	തിരികെ -1;
    method->encode_fn(state, from, len, *newptr, last);
    തിരികെ (long)len;
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
    പൂർണ്ണസംഖ്യ		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    എങ്കിൽ (method->decode_buffer_fn != ശൂന്യ)
	// Has buffer function, pass through.
	തിരികെ method->decode_buffer_fn(state, ptr, len, newptr, last);

    എങ്കിൽ (len == 0)
	// Not buffering, just return EOF.
	തിരികെ len;

    *newptr = alloc(len);
    എങ്കിൽ (*newptr == ശൂന്യ)
	തിരികെ -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    തിരികെ len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    ശൂന്യം
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    പൂർണ്ണസംഖ്യ		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#എങ്കിൽ 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    ശൂന്യം
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    പൂർണ്ണസംഖ്യ		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    ശൂന്യം
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    പൂർണ്ണസംഖ്യ		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    ശൂന്യം
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    പൂർണ്ണസംഖ്യ		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    ശൂന്യം
crypt_free_താക്കോൽ(char_u *താക്കോൽ)
{
    // Create a safe memset which cannot be optimized away by compiler
    സ്ഥിതം ശൂന്യം *(* അസ്ഥിര vim_memset_safe)(ശൂന്യം *s, പൂർണ്ണസംഖ്യ c, size_t n) =
	memset;
    എങ്കിൽ (താക്കോൽ != ശൂന്യ)
    {
#ifdef FEAT_SODIUM
	എങ്കിൽ (sodium_init() >= 0)
	    sodium_memzero(താക്കോൽ, STRLEN(താക്കോൽ));
	അല്ലെങ്കിൽ
#endif
	    vim_memset_safe(താക്കോൽ, 0, STRLEN(താക്കോൽ));
	vim_free(താക്കോൽ);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    ശൂന്യം
crypt_check_method(പൂർണ്ണസംഖ്യ method)
{
    എങ്കിൽ (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	സന്ദേശം_scroll = TRUE;
	സന്ദേശം(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    ശൂന്യം
crypt_check_swapfile_curbuf(ശൂന്യം)
{
#ifdef FEAT_SODIUM
    പൂർണ്ണസംഖ്യ method = crypt_get_method_nr(curbuf);
    എങ്കിൽ (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_മൂല്യം_give_err((char_u *)"swf", 0, ശൂന്യ, OPT_LOCAL);
	സന്ദേശം_scroll = TRUE;
	സന്ദേശം(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    ശൂന്യം
crypt_check_current_method(ശൂന്യം)
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
crypt_get_താക്കോൽ(
    പൂർണ്ണസംഖ്യ		store,
    പൂർണ്ണസംഖ്യ		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		round;

    വേണ്ടി (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = സന്ദേശം_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		ശൂന്യ);
	cmdline_star = FALSE;

	എങ്കിൽ (p1 == ശൂന്യ)
	    നിർത്തുക;

	എങ്കിൽ (round == twice)
	{
	    എങ്കിൽ (p2 != ശൂന്യ && STRCMP(p1, p2) != 0)
	    {
		സന്ദേശം(_("Keys don't match!"));
		crypt_free_താക്കോൽ(p1);
		crypt_free_താക്കോൽ(p2);
		p2 = ശൂന്യ;
		round = -1;		// do it again
		തുടരുക;
	    }

	    എങ്കിൽ (store)
	    {
		set_option_മൂല്യം_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_താക്കോൽ(p1);
		p1 = curbuf->b_p_താക്കോൽ;
		crypt_check_swapfile_curbuf();
	    }
	    നിർത്തുക;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    എങ്കിൽ (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	എങ്കിൽ (സന്ദേശം_didout)
	    സന്ദേശം_putchar('\n');
	need_wait_return = FALSE;
	സന്ദേശം_didout = FALSE;
    }

    crypt_free_താക്കോൽ(p2);
    തിരികെ p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    ശൂന്യം
crypt_append_സന്ദേശം(
    buf_T *buf)
{
    എങ്കിൽ (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    അല്ലെങ്കിൽ
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*താക്കോൽ UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned അക്ഷരം	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    പൂർണ്ണസംഖ്യ			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    പൂർണ്ണസംഖ്യ			alg;

    എങ്കിൽ (sodium_init() < 0)
	തിരികെ FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(വലുപ്പം(sodium_state_T));
    sodium_memzero(sd_state, വലുപ്പം(sodium_state_T));

    എങ്കിൽ ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# എങ്കിൽ 0
	// For testing
	എങ്കിൽ (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	എങ്കിൽ (crypto_pwhash(dkey, വലുപ്പം(dkey), (സ്ഥിരം അക്ഷരം *)താക്കോൽ, STRLEN(താക്കോൽ),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    തിരികെ FAIL;
	}
	memcpy(sd_state->താക്കോൽ, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->താക്കോൽ, crypto_box_SEEDBYTES);
	retval += sodium_mlock(താക്കോൽ, STRLEN(താക്കോൽ));

	എങ്കിൽ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    തിരികെ FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	എങ്കിൽ (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != ശൂന്യ)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += വലുപ്പം(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += വലുപ്പം(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, വലുപ്പം(opslimit) + വലുപ്പം(memlimit) + വലുപ്പം(alg));
	}
    }
    അല്ലെങ്കിൽ
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	പൂർണ്ണസംഖ്യ	size = വലുപ്പം(opslimit) +
	    വലുപ്പം(memlimit) + വലുപ്പം(alg);

	// Reading parameters from file
	എങ്കിൽ (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    തിരികെ FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += വലുപ്പം(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += വലുപ്പം(memlimit);
	alg = crypt_char_to_int(p);
	p += വലുപ്പം(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	എങ്കിൽ (crypto_pwhash(dkey, വലുപ്പം(dkey), (സ്ഥിരം അക്ഷരം *)താക്കോൽ, STRLEN(താക്കോൽ),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    തിരികെ FAIL;
	}
	memcpy(sd_state->താക്കോൽ, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->താക്കോൽ, crypto_box_SEEDBYTES);
	retval += sodium_mlock(താക്കോൽ, STRLEN(താക്കോൽ));

	എങ്കിൽ (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    തിരികെ FAIL;
	}
    }
    sd_state->എണ്ണം = 0;
    state->method_state = sd_state;

    തിരികെ OK;
#അല്ലെങ്കിൽ
    emsg(_(e_libsodium_not_built_in));
    തിരികെ FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#എങ്കിൽ 0  // Currently unused
    ശൂന്യം
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    പൂർണ്ണസംഖ്യ		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned അക്ഷരം  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    എങ്കിൽ (sod_st->എണ്ണം == 0)
    {
	എങ്കിൽ (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    തിരികെ;
	}
	crypto_secretstream_xchacha20poly1305_init_തള്ളുക(&sod_st->state,
							      to, sod_st->താക്കോൽ);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    എങ്കിൽ (sod_st->എണ്ണം && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	തിരികെ;
    }

    crypto_secretstream_xchacha20poly1305_തള്ളുക(&sod_st->state, to, ശൂന്യ,
						      from, len, ശൂന്യ, 0, tag);

    sod_st->എണ്ണം++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#എങ്കിൽ 0  // Currently unused
    ശൂന്യം
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    പൂർണ്ണസംഖ്യ		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned അക്ഷരം  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    എങ്കിൽ (sod_st->എണ്ണം == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	തിരികെ;
    }

    buf_out = (char_u *)alloc(len);

    എങ്കിൽ (buf_out == ശൂന്യ)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	തിരികെ;
    }
    എങ്കിൽ (sod_st->എണ്ണം == 0)
    {
	എങ്കിൽ (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->താക്കോൽ) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    പോകുക fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	എങ്കിൽ (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    എങ്കിൽ (sod_st->എണ്ണം && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	പോകുക fail;
    }
    എങ്കിൽ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, ശൂന്യ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	പോകുക fail;
    }
    sod_st->എണ്ണം++;

    എങ്കിൽ (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	പോകുക fail;
    }
    എങ്കിൽ (p1 == p2)
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
    സ്ഥിതം long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    പൂർണ്ണസംഖ്യ		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned അക്ഷരം	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    പൂർണ്ണസംഖ്യ			length;
    sodium_state_T	*sod_st = state->method_state;
    പൂർണ്ണസംഖ്യ			first = (sod_st->എണ്ണം == 0);

    length = (പൂർണ്ണസംഖ്യ)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    എങ്കിൽ (*buf_out == ശൂന്യ)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	തിരികെ -1;
    }
    ptr = *buf_out;

    എങ്കിൽ (first)
    {
	crypto_secretstream_xchacha20poly1305_init_തള്ളുക(&sod_st->state,
		ptr, sod_st->താക്കോൽ);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_തള്ളുക(&sod_st->state, ptr,
	    &out_len, from, len, ശൂന്യ, 0, tag);

    sod_st->എണ്ണം++;
    തിരികെ out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#അല്ലെങ്കിൽ
    തിരികെ -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    സ്ഥിതം long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    പൂർണ്ണസംഖ്യ		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned അക്ഷരം  tag;
    unsigned long long out_len;

    എങ്കിൽ (sod_st->എണ്ണം == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    എങ്കിൽ (*buf_out == ശൂന്യ)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	തിരികെ -1;
    }

    എങ്കിൽ (sod_st->എണ്ണം == 0)
    {
	എങ്കിൽ (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->താക്കോൽ) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    തിരികെ -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->എണ്ണം++;
    }
    എങ്കിൽ (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, ശൂന്യ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	തിരികെ -1;
    }

    എങ്കിൽ (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    തിരികെ (long) out_len;
#അല്ലെങ്കിൽ
    തിരികെ -1;
#endif
}

#എങ്കിൽ defined(FEAT_SODIUM)
    ശൂന്യം
crypt_sodium_lock_താക്കോൽ(char_u *താക്കോൽ)
{
    എങ്കിൽ (sodium_init() >= 0)
	sodium_mlock(താക്കോൽ, STRLEN(താക്കോൽ));
}

    പൂർണ്ണസംഖ്യ
crypt_sodium_munlock(ശൂന്യം *സ്ഥിരം addr, സ്ഥിരം size_t len)
{
    തിരികെ sodium_munlock(addr, len);
}

    ശൂന്യം
crypt_sodium_randombytes_buf(ശൂന്യം *സ്ഥിരം buf, സ്ഥിരം size_t size)
{
    randombytes_buf(buf, size);
}

    പൂർണ്ണസംഖ്യ
crypt_sodium_init(ശൂന്യം)
{
    തിരികെ sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(ശൂന്യം)
{
    തിരികെ randombytes_random();
}

# എങ്കിൽ defined(FEAT_EVAL)
    സ്ഥിതം ശൂന്യം
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	പൂർണ്ണസംഖ്യ alg,
	പൂർണ്ണസംഖ്യ alg_def)
{
    എങ്കിൽ (p_verbose > 0)
    {
	verbose_enter();
	എങ്കിൽ (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	അല്ലെങ്കിൽ
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	എങ്കിൽ (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	അല്ലെങ്കിൽ
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	എങ്കിൽ (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	അല്ലെങ്കിൽ
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    സ്ഥിതം ശൂന്യം
crypt_long_long_to_char(long long n, char_u *s)
{
    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    സ്ഥിതം ശൂന്യം
crypt_int_to_char(പൂർണ്ണസംഖ്യ n, char_u *s)
{
    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    സ്ഥിതം long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി (i = 7; i >= 0; i--)
    {
	എങ്കിൽ (i == 7)
	    retval = s[i];
	അല്ലെങ്കിൽ
	    retval |= s[i];
	എങ്കിൽ (i > 0)
	    retval <<= 8;
    }
    തിരികെ retval;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
crypt_char_to_int(char_u *s)
{
    പൂർണ്ണസംഖ്യ retval = 0;
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 3; i >= 0; i--)
    {
	എങ്കിൽ (i == 3)
	    retval = s[i];
	അല്ലെങ്കിൽ
	    retval |= s[i];
	എങ്കിൽ (i > 0)
	    retval <<= 8;
    }
    തിരികെ retval;
}
#endif

#endif // FEAT_CRYPT
