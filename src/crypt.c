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
#include "vim.h"

#ถ้า defined(FEAT_CRYPT)
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

typedef sa_nracanaa {
    var_na	*name;		// encryption name as used in 'cryptmethod'
    var_na	*magic;		// magic bytes stored in file header
    จำนวนเต็ม		salt_len;	// length of salt, or 0 when not using salt
    จำนวนเต็ม		seed_len;	// length of seed, or 0 when not using seed
    จำนวนเต็ม		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    จำนวนเต็ม		works_inplace;	// encryption/decryption can be done in-place
#endif
    จำนวนเต็ม		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    จำนวนเต็ม (*self_test_fn)(void);

    // Function pointer for initializing encryption/decryption.
    จำนวนเต็ม (* init_fn)(cryptstate_T *state, char_u *key, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    void (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, จำนวนเต็ม last);
    void (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, จำนวนเต็ม last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, จำนวนเต็ม last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, จำนวนเต็ม last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    void (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, จำนวนเต็ม last);
    void (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, จำนวนเต็ม last);
} cryptmethod_T;

static จำนวนเต็ม crypt_sodium_init_(cryptstate_T *state, char_u *key, crypt_arg_T *arg);
static long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, จำนวนเต็ม last);
static long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, จำนวนเต็ม last);
#ถ้า defined(FEAT_SODIUM)
static void crypt_long_long_to_char(long long n, char_u *s);
static void crypt_int_to_char(จำนวนเต็ม n, char_u *s);
static long long crypt_char_to_long_long(char_u *s);
static จำนวนเต็ม crypt_char_to_int(char_u *s);
#endif
#ถ้า defined(FEAT_EVAL) && defined(FEAT_SODIUM)
static void crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, จำนวนเต็ม alg, จำนวนเต็ม alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
static cryptmethod_T cryptmethods[CRYPT_M_COUNT] = {
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
	NULL,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	NULL, NULL,
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
	NULL, NULL,
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
	NULL, NULL,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#มิฉะนั้น
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	NULL,
	crypt_sodium_init_,
	NULL, NULL,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	NULL, NULL,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#มิฉะนั้น
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	NULL,
	crypt_sodium_init_,
	NULL, NULL,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	NULL, NULL,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#ถ้า defined(FEAT_SODIUM)
typedef sa_nracanaa {
    size_t	    count;
    unsigned var_na   key[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define symbol_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  มิฉะนั้น
#   error Dynamic loading of libsodium is not supported สำหรับ now.
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
#  define crypto_secretstream_xchacha20poly1305_init_push \
    dll_crypto_secretstream_xchacha20poly1305_init_push
#  define crypto_secretstream_xchacha20poly1305_push \
    dll_crypto_secretstream_xchacha20poly1305_push
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

static จำนวนเต็ม (*dll_sodium_init)(void) = NULL;
static void (*dll_sodium_free)(void *) = NULL;
static void *(*dll_sodium_malloc)(const size_t) = NULL;
static void (*dll_sodium_memzero)(void * const, const size_t) = NULL;
static จำนวนเต็ม (*dll_sodium_mlock)(void * const, const size_t) = NULL;
static จำนวนเต็ม (*dll_sodium_munlock)(void * const, const size_t) = NULL;
static จำนวนเต็ม (*dll_crypto_secretstream_xchacha20poly1305_init_push)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned var_na [],
    const unsigned var_na []) = NULL;
static จำนวนเต็ม (*dll_crypto_secretstream_xchacha20poly1305_push)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned var_na *c, unsigned long long *clen_p,
    const unsigned var_na *m, unsigned long long mlen,
    const unsigned var_na *ad, unsigned long long adlen, unsigned var_na tag)
    = NULL;
static จำนวนเต็ม (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    const unsigned var_na [],
    const unsigned var_na []) = NULL;
static จำนวนเต็ม (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned var_na *m, unsigned long long *mlen_p, unsigned var_na *tag_p,
    const unsigned var_na *c, unsigned long long clen,
    const unsigned var_na *ad, unsigned long long adlen) = NULL;
static จำนวนเต็ม (*dll_crypto_pwhash)(unsigned var_na * const out,
    unsigned long long outlen,
    const var_na * const passwd, unsigned long long passwdlen,
    const unsigned var_na * const salt,
    unsigned long long opslimit, size_t memlimit, จำนวนเต็ม alg)
    = NULL;
static void (*dll_randombytes_buf)(void * const buf, const size_t size);
static uint32_t (*dll_randombytes_random)(void);

static sa_nracanaa {
    const var_na *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_push},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_push},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {NULL, NULL}
};

    static จำนวนเต็ม
sodium_runtime_link_init(จำนวนเต็ม verbose)
{
    static HINSTANCE hsodium = NULL;
    const var_na *libname = DYNAMIC_SODIUM_DLL;
    จำนวนเต็ม i;

    ถ้า (hsodium != NULL)
	คืน OK;

    hsodium = load_dll(libname);
    ถ้า (hsodium == NULL)
    {
	ถ้า (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	คืน FAIL;
    }

    สำหรับ (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	ถ้า ((*sodium_funcname_table[i].ptr = symbol_from_dll(hsodium,
			sodium_funcname_table[i].name)) == NULL)
	{
	    close_dll(hsodium);
	    hsodium = NULL;
	    ถ้า (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    คืน FAIL;
	}
    }
    คืน OK;
}

    static จำนวนเต็ม
load_sodium(void)
{
    ถ้า (sodium_runtime_link_init(TRUE) == FAIL)
	คืน -1;
    คืน dll_sodium_init();
}
# endif

# ถ้า defined(DYNAMIC_SODIUM)
    จำนวนเต็ม
sodium_enabled(จำนวนเต็ม verbose)
{
    คืน sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
static const var_na crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    จำนวนเต็ม
crypt_method_nr_from_name(char_u *name)
{
    จำนวนเต็ม i;

    สำหรับ (i = 0; i < CRYPT_M_COUNT; ++i)
	ถ้า (STRCMP(name, cryptmethods[i].name) == 0)
	    คืน i;
    คืน 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    จำนวนเต็ม
crypt_method_nr_from_magic(var_na *ptr, จำนวนเต็ม len)
{
    จำนวนเต็ม i;

    ถ้า (len < CRYPT_MAGIC_LEN)
	คืน -1;

    สำหรับ (i = 0; i < CRYPT_M_COUNT; i++)
	ถ้า (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    คืน i;

    i = (จำนวนเต็ม)STRLEN(crypt_magic_head);
    ถ้า (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    คืน -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    จำนวนเต็ม
crypt_works_inplace(cryptstate_T *state)
{
    คืน cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    จำนวนเต็ม
crypt_get_method_nr(buf_T *buf)
{
    คืน crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    จำนวนเต็ม
crypt_method_is_sodium(จำนวนเต็ม method)
{
    คืน method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    จำนวนเต็ม
crypt_whole_undofile(จำนวนเต็ม method_nr)
{
    คืน cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    จำนวนเต็ม
crypt_get_header_len(จำนวนเต็ม method_nr)
{
    คืน CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#ถ้า defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    จำนวนเต็ม
crypt_get_max_header_len(void)
{
    จำนวนเต็ม i;
    จำนวนเต็ม max = 0;
    จำนวนเต็ม temp = 0;

    สำหรับ (i = 0; i < CRYPT_M_COUNT; ++i)
    {
	temp = crypt_get_header_len(i);
	ถ้า (temp > max)
	    max = temp;
    }
    คืน max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    void
crypt_set_cm_option(buf_T *buf, จำนวนเต็ม method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    จำนวนเต็ม
crypt_self_test(void)
{
    จำนวนเต็ม method_nr = crypt_get_method_nr(curbuf);

    ถ้า (cryptmethods[method_nr].self_test_fn == NULL)
	คืน OK;
    คืน cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    จำนวนเต็ม		method_nr,
    char_u	*key,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    ถ้า (state == NULL)
	คืน state;

    state->method_nr = method_nr;
    ถ้า (cryptmethods[method_nr].init_fn(state, key, crypt_arg) == FAIL)
    {
	vim_free(state);
	คืน NULL;
    }
    คืน state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    จำนวนเต็ม		method_nr,
    char_u	*key,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    ถ้า (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    ถ้า (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    ถ้า (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    คืน crypt_create(method_nr, key, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *key)
{
    จำนวนเต็ม		method_nr;
    จำนวนเต็ม		header_len;
    var_na	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    ถ้า (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	คืน NULL;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    ถ้า (method_nr < 0)
	คืน NULL;

    header_len = crypt_get_header_len(method_nr);
    ถ้า ((buffer = alloc(header_len)) == NULL)
	คืน NULL;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    ถ้า (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	คืน NULL;
    }

    state = crypt_create_from_header(method_nr, key, buffer);
    vim_free(buffer);
    คืน state;
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
    จำนวนเต็ม	    method_nr,
    char_u  *key,
    char_u  **header,
    จำนวนเต็ม	    *header_len)
{
    จำนวนเต็ม	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    ถ้า (*header == NULL)
	คืน NULL;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    ถ้า (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	ถ้า (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	ถ้า (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	ถ้า (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	ถ้า (sodium_init() >= 0)
	{
	    ถ้า (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    ถ้า (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	มิฉะนั้น
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, key, &arg);
    ถ้า (state == NULL)
	VIM_CLEAR(*header);
    คืน state;
}

/*
 * Free the crypt state.
 */
    void
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    ถ้า (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->key,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, parimaa_na(sodium_state_T));
	sodium_free(state->method_state);
    }
    มิฉะนั้น
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
    จำนวนเต็ม		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    ถ้า (method->encode_buffer_fn != NULL)
	// Has buffer function, pass through.
	คืน method->encode_buffer_fn(state, from, len, newptr, last);
    ถ้า (len == 0)
	// Not buffering, just return EOF.
	คืน (long)len;

    *newptr = alloc(len + 50);
    ถ้า (*newptr == NULL)
	คืน -1;
    method->encode_fn(state, from, len, *newptr, last);
    คืน (long)len;
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
    จำนวนเต็ม		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    ถ้า (method->decode_buffer_fn != NULL)
	// Has buffer function, pass through.
	คืน method->decode_buffer_fn(state, ptr, len, newptr, last);

    ถ้า (len == 0)
	// Not buffering, just return EOF.
	คืน len;

    *newptr = alloc(len);
    ถ้า (*newptr == NULL)
	คืน -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    คืน len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    void
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    จำนวนเต็ม		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#ถ้า 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    void
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    จำนวนเต็ม		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    void
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    จำนวนเต็ม		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    void
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    จำนวนเต็ม		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    void
crypt_free_key(char_u *key)
{
    // Create a safe memset which cannot be optimized away by compiler
    static void *(* ca_ncala vim_memset_safe)(void *s, จำนวนเต็ม c, size_t n) =
	memset;
    ถ้า (key != NULL)
    {
#ifdef FEAT_SODIUM
	ถ้า (sodium_init() >= 0)
	    sodium_memzero(key, STRLEN(key));
	มิฉะนั้น
#endif
	    vim_memset_safe(key, 0, STRLEN(key));
	vim_free(key);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    void
crypt_check_method(จำนวนเต็ม method)
{
    ถ้า (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	msg_scroll = TRUE;
	msg(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    void
crypt_check_swapfile_curbuf(void)
{
#ifdef FEAT_SODIUM
    จำนวนเต็ม method = crypt_get_method_nr(curbuf);
    ถ้า (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_value_give_err((char_u *)"swf", 0, NULL, OPT_LOCAL);
	msg_scroll = TRUE;
	msg(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    void
crypt_check_current_method(void)
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
crypt_get_key(
    จำนวนเต็ม		store,
    จำนวนเต็ม		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = NULL;
    จำนวนเต็ม		round;

    สำหรับ (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = msg_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		NULL);
	cmdline_star = FALSE;

	ถ้า (p1 == NULL)
	    bha_nga;

	ถ้า (round == twice)
	{
	    ถ้า (p2 != NULL && STRCMP(p1, p2) != 0)
	    {
		msg(_("Keys don't match!"));
		crypt_free_key(p1);
		crypt_free_key(p2);
		p2 = NULL;
		round = -1;		// do it again
		continue;
	    }

	    ถ้า (store)
	    {
		set_option_value_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_key(p1);
		p1 = curbuf->b_p_key;
		crypt_check_swapfile_curbuf();
	    }
	    bha_nga;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    ถ้า (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	ถ้า (msg_didout)
	    msg_putchar('\n');
	need_wait_return = FALSE;
	msg_didout = FALSE;
    }

    crypt_free_key(p2);
    คืน p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    void
crypt_append_msg(
    buf_T *buf)
{
    ถ้า (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    มิฉะนั้น
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    static จำนวนเต็ม
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*key UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned var_na	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    จำนวนเต็ม			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    จำนวนเต็ม			alg;

    ถ้า (sodium_init() < 0)
	คืน FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(parimaa_na(sodium_state_T));
    sodium_memzero(sd_state, parimaa_na(sodium_state_T));

    ถ้า ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# ถ้า 0
	// For testing
	ถ้า (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	ถ้า (crypto_pwhash(dkey, parimaa_na(dkey), (const var_na *)key, STRLEN(key),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    คืน FAIL;
	}
	memcpy(sd_state->key, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->key, crypto_box_SEEDBYTES);
	retval += sodium_mlock(key, STRLEN(key));

	ถ้า (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    คืน FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	ถ้า (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != NULL)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += parimaa_na(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += parimaa_na(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, parimaa_na(opslimit) + parimaa_na(memlimit) + parimaa_na(alg));
	}
    }
    มิฉะนั้น
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	จำนวนเต็ม	size = parimaa_na(opslimit) +
	    parimaa_na(memlimit) + parimaa_na(alg);

	// Reading parameters from file
	ถ้า (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    คืน FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += parimaa_na(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += parimaa_na(memlimit);
	alg = crypt_char_to_int(p);
	p += parimaa_na(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	ถ้า (crypto_pwhash(dkey, parimaa_na(dkey), (const var_na *)key, STRLEN(key),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    คืน FAIL;
	}
	memcpy(sd_state->key, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->key, crypto_box_SEEDBYTES);
	retval += sodium_mlock(key, STRLEN(key));

	ถ้า (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    คืน FAIL;
	}
    }
    sd_state->count = 0;
    state->method_state = sd_state;

    คืน OK;
#มิฉะนั้น
    emsg(_(e_libsodium_not_built_in));
    คืน FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#ถ้า 0  // Currently unused
    void
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    จำนวนเต็ม		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned var_na  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    ถ้า (sod_st->count == 0)
    {
	ถ้า (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    คืน;
	}
	crypto_secretstream_xchacha20poly1305_init_push(&sod_st->state,
							      to, sod_st->key);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ถ้า (sod_st->count && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	คืน;
    }

    crypto_secretstream_xchacha20poly1305_push(&sod_st->state, to, NULL,
						      from, len, NULL, 0, tag);

    sod_st->count++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#ถ้า 0  // Currently unused
    void
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    จำนวนเต็ม		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned var_na  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    ถ้า (sod_st->count == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	คืน;
    }

    buf_out = (char_u *)alloc(len);

    ถ้า (buf_out == NULL)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	คืน;
    }
    ถ้า (sod_st->count == 0)
    {
	ถ้า (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->key) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    goto fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	ถ้า (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ถ้า (sod_st->count && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	goto fail;
    }
    ถ้า (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, NULL, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	goto fail;
    }
    sod_st->count++;

    ถ้า (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	goto fail;
    }
    ถ้า (p1 == p2)
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
    static long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    จำนวนเต็ม		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned var_na	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    จำนวนเต็ม			length;
    sodium_state_T	*sod_st = state->method_state;
    จำนวนเต็ม			first = (sod_st->count == 0);

    length = (จำนวนเต็ม)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    ถ้า (*buf_out == NULL)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	คืน -1;
    }
    ptr = *buf_out;

    ถ้า (first)
    {
	crypto_secretstream_xchacha20poly1305_init_push(&sod_st->state,
		ptr, sod_st->key);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_push(&sod_st->state, ptr,
	    &out_len, from, len, NULL, 0, tag);

    sod_st->count++;
    คืน out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#มิฉะนั้น
    คืน -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    static long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    จำนวนเต็ม		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned var_na  tag;
    unsigned long long out_len;

    ถ้า (sod_st->count == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    ถ้า (*buf_out == NULL)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	คืน -1;
    }

    ถ้า (sod_st->count == 0)
    {
	ถ้า (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->key) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    คืน -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->count++;
    }
    ถ้า (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, NULL, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	คืน -1;
    }

    ถ้า (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    คืน (long) out_len;
#มิฉะนั้น
    คืน -1;
#endif
}

#ถ้า defined(FEAT_SODIUM)
    void
crypt_sodium_lock_key(char_u *key)
{
    ถ้า (sodium_init() >= 0)
	sodium_mlock(key, STRLEN(key));
}

    จำนวนเต็ม
crypt_sodium_munlock(void *const addr, const size_t len)
{
    คืน sodium_munlock(addr, len);
}

    void
crypt_sodium_randombytes_buf(void *const buf, const size_t size)
{
    randombytes_buf(buf, size);
}

    จำนวนเต็ม
crypt_sodium_init(void)
{
    คืน sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(void)
{
    คืน randombytes_random();
}

# ถ้า defined(FEAT_EVAL)
    static void
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	จำนวนเต็ม alg,
	จำนวนเต็ม alg_def)
{
    ถ้า (p_verbose > 0)
    {
	verbose_enter();
	ถ้า (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	มิฉะนั้น
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	ถ้า (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	มิฉะนั้น
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	ถ้า (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	มิฉะนั้น
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    static void
crypt_long_long_to_char(long long n, char_u *s)
{
    จำนวนเต็ม i;
    สำหรับ (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    static void
crypt_int_to_char(จำนวนเต็ม n, char_u *s)
{
    จำนวนเต็ม i;
    สำหรับ (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    static long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    จำนวนเต็ม i;
    สำหรับ (i = 7; i >= 0; i--)
    {
	ถ้า (i == 7)
	    retval = s[i];
	มิฉะนั้น
	    retval |= s[i];
	ถ้า (i > 0)
	    retval <<= 8;
    }
    คืน retval;
}

    static จำนวนเต็ม
crypt_char_to_int(char_u *s)
{
    จำนวนเต็ม retval = 0;
    จำนวนเต็ม i;

    สำหรับ (i = 3; i >= 0; i--)
    {
	ถ้า (i == 3)
	    retval = s[i];
	มิฉะนั้น
	    retval |= s[i];
	ถ้า (i > 0)
	    retval <<= 8;
    }
    คืน retval;
}
#endif

#endif // FEAT_CRYPT
