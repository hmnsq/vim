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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_CRYPT)
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

வகைவரை கட்டமைப்பு {
    எழுத்து	*name;		// encryption name as used in 'cryptmethod'
    எழுத்து	*magic;		// magic bytes stored in file header
    முழு		salt_len;	// length of salt, or 0 when not using salt
    முழு		seed_len;	// length of seed, or 0 when not using seed
    முழு		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    முழு		works_inplace;	// encryption/decryption can be done in-place
#endif
    முழு		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    முழு (*self_test_fn)(வெறுமை);

    // Function pointer for initializing encryption/decryption.
    முழு (* init_fn)(cryptstate_T *state, char_u *சாவி, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    வெறுமை (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, முழு last);
    வெறுமை (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, முழு last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, முழு last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, முழு last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    வெறுமை (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, முழு last);
    வெறுமை (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, முழு last);
} cryptmethod_T;

நிலையான முழு crypt_sodium_init_(cryptstate_T *state, char_u *சாவி, crypt_arg_T *arg);
நிலையான long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, முழு last);
நிலையான long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, முழு last);
#எனில் defined(FEAT_SODIUM)
நிலையான வெறுமை crypt_long_long_to_char(long long n, char_u *s);
நிலையான வெறுமை crypt_int_to_char(முழு n, char_u *s);
நிலையான long long crypt_char_to_long_long(char_u *s);
நிலையான முழு crypt_char_to_int(char_u *s);
#endif
#எனில் defined(FEAT_EVAL) && defined(FEAT_SODIUM)
நிலையான வெறுமை crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, முழு alg, முழு alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
நிலையான cryptmethod_T cryptmethods[CRYPT_M_மொத்தம்] = {
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
	வெற்று,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	வெற்று, வெற்று,
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
	வெற்று, வெற்று,
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
	வெற்று, வெற்று,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#இல்லையெனில்
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	வெற்று,
	crypt_sodium_init_,
	வெற்று, வெற்று,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	வெற்று, வெற்று,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#இல்லையெனில்
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	வெற்று,
	crypt_sodium_init_,
	வெற்று, வெற்று,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	வெற்று, வெற்று,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#எனில் defined(FEAT_SODIUM)
வகைவரை கட்டமைப்பு {
    size_t	    மொத்தம்;
    unsigned எழுத்து   சாவி[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define குறியீடு_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  இல்லையெனில்
#   error Dynamic loading of libsodium is not supported க்கு now.
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
#  define crypto_secretstream_xchacha20poly1305_init_தள்ளு \
    dll_crypto_secretstream_xchacha20poly1305_init_தள்ளு
#  define crypto_secretstream_xchacha20poly1305_தள்ளு \
    dll_crypto_secretstream_xchacha20poly1305_தள்ளு
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

நிலையான முழு (*dll_sodium_init)(வெறுமை) = வெற்று;
நிலையான வெறுமை (*dll_sodium_free)(வெறுமை *) = வெற்று;
நிலையான வெறுமை *(*dll_sodium_malloc)(மாறிலி size_t) = வெற்று;
நிலையான வெறுமை (*dll_sodium_memzero)(வெறுமை * மாறிலி, மாறிலி size_t) = வெற்று;
நிலையான முழு (*dll_sodium_mlock)(வெறுமை * மாறிலி, மாறிலி size_t) = வெற்று;
நிலையான முழு (*dll_sodium_munlock)(வெறுமை * மாறிலி, மாறிலி size_t) = வெற்று;
நிலையான முழு (*dll_crypto_secretstream_xchacha20poly1305_init_தள்ளு)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned எழுத்து [],
    மாறிலி unsigned எழுத்து []) = வெற்று;
நிலையான முழு (*dll_crypto_secretstream_xchacha20poly1305_தள்ளு)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned எழுத்து *c, unsigned long long *clen_p,
    மாறிலி unsigned எழுத்து *m, unsigned long long mlen,
    மாறிலி unsigned எழுத்து *ad, unsigned long long adlen, unsigned எழுத்து tag)
    = வெற்று;
நிலையான முழு (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    மாறிலி unsigned எழுத்து [],
    மாறிலி unsigned எழுத்து []) = வெற்று;
நிலையான முழு (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned எழுத்து *m, unsigned long long *mlen_p, unsigned எழுத்து *tag_p,
    மாறிலி unsigned எழுத்து *c, unsigned long long clen,
    மாறிலி unsigned எழுத்து *ad, unsigned long long adlen) = வெற்று;
நிலையான முழு (*dll_crypto_pwhash)(unsigned எழுத்து * மாறிலி out,
    unsigned long long outlen,
    மாறிலி எழுத்து * மாறிலி passwd, unsigned long long passwdlen,
    மாறிலி unsigned எழுத்து * மாறிலி salt,
    unsigned long long opslimit, size_t memlimit, முழு alg)
    = வெற்று;
நிலையான வெறுமை (*dll_randombytes_buf)(வெறுமை * மாறிலி buf, மாறிலி size_t size);
நிலையான uint32_t (*dll_randombytes_random)(வெறுமை);

நிலையான கட்டமைப்பு {
    மாறிலி எழுத்து *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_தள்ளு},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_தள்ளு},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {வெற்று, வெற்று}
};

    நிலையான முழு
sodium_runtime_link_init(முழு verbose)
{
    நிலையான HINSTANCE hsodium = வெற்று;
    மாறிலி எழுத்து *libname = DYNAMIC_SODIUM_DLL;
    முழு i;

    எனில் (hsodium != வெற்று)
	திரும்பு OK;

    hsodium = load_dll(libname);
    எனில் (hsodium == வெற்று)
    {
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	திரும்பு FAIL;
    }

    க்கு (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	எனில் ((*sodium_funcname_table[i].ptr = குறியீடு_from_dll(hsodium,
			sodium_funcname_table[i].name)) == வெற்று)
	{
	    close_dll(hsodium);
	    hsodium = வெற்று;
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}

    நிலையான முழு
load_sodium(வெறுமை)
{
    எனில் (sodium_runtime_link_init(TRUE) == FAIL)
	திரும்பு -1;
    திரும்பு dll_sodium_init();
}
# endif

# எனில் defined(DYNAMIC_SODIUM)
    முழு
sodium_enabled(முழு verbose)
{
    திரும்பு sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
நிலையான மாறிலி எழுத்து crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    முழு
crypt_method_nr_from_name(char_u *name)
{
    முழு i;

    க்கு (i = 0; i < CRYPT_M_மொத்தம்; ++i)
	எனில் (STRCMP(name, cryptmethods[i].name) == 0)
	    திரும்பு i;
    திரும்பு 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    முழு
crypt_method_nr_from_magic(எழுத்து *ptr, முழு len)
{
    முழு i;

    எனில் (len < CRYPT_MAGIC_LEN)
	திரும்பு -1;

    க்கு (i = 0; i < CRYPT_M_மொத்தம்; i++)
	எனில் (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    திரும்பு i;

    i = (முழு)STRLEN(crypt_magic_head);
    எனில் (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    திரும்பு -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    முழு
crypt_works_inplace(cryptstate_T *state)
{
    திரும்பு cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    முழு
crypt_get_method_nr(buf_T *buf)
{
    திரும்பு crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    முழு
crypt_method_is_sodium(முழு method)
{
    திரும்பு method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    முழு
crypt_whole_undofile(முழு method_nr)
{
    திரும்பு cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    முழு
crypt_get_header_len(முழு method_nr)
{
    திரும்பு CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#எனில் defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    முழு
crypt_get_max_header_len(வெறுமை)
{
    முழு i;
    முழு max = 0;
    முழு temp = 0;

    க்கு (i = 0; i < CRYPT_M_மொத்தம்; ++i)
    {
	temp = crypt_get_header_len(i);
	எனில் (temp > max)
	    max = temp;
    }
    திரும்பு max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    வெறுமை
crypt_set_cm_option(buf_T *buf, முழு method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    முழு
crypt_self_test(வெறுமை)
{
    முழு method_nr = crypt_get_method_nr(curbuf);

    எனில் (cryptmethods[method_nr].self_test_fn == வெற்று)
	திரும்பு OK;
    திரும்பு cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    முழு		method_nr,
    char_u	*சாவி,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    எனில் (state == வெற்று)
	திரும்பு state;

    state->method_nr = method_nr;
    எனில் (cryptmethods[method_nr].init_fn(state, சாவி, crypt_arg) == FAIL)
    {
	vim_free(state);
	திரும்பு வெற்று;
    }
    திரும்பு state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    முழு		method_nr,
    char_u	*சாவி,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    எனில் (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    எனில் (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    எனில் (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    திரும்பு crypt_create(method_nr, சாவி, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *சாவி)
{
    முழு		method_nr;
    முழு		header_len;
    எழுத்து	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    எனில் (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	திரும்பு வெற்று;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    எனில் (method_nr < 0)
	திரும்பு வெற்று;

    header_len = crypt_get_header_len(method_nr);
    எனில் ((buffer = alloc(header_len)) == வெற்று)
	திரும்பு வெற்று;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    எனில் (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	திரும்பு வெற்று;
    }

    state = crypt_create_from_header(method_nr, சாவி, buffer);
    vim_free(buffer);
    திரும்பு state;
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
    முழு	    method_nr,
    char_u  *சாவி,
    char_u  **header,
    முழு	    *header_len)
{
    முழு	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    எனில் (*header == வெற்று)
	திரும்பு வெற்று;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    எனில் (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	எனில் (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	எனில் (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	எனில் (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	எனில் (sodium_init() >= 0)
	{
	    எனில் (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    எனில் (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	இல்லையெனில்
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, சாவி, &arg);
    எனில் (state == வெற்று)
	VIM_CLEAR(*header);
    திரும்பு state;
}

/*
 * Free the crypt state.
 */
    வெறுமை
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    எனில் (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->சாவி,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, அளவு(sodium_state_T));
	sodium_free(state->method_state);
    }
    இல்லையெனில்
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
    முழு		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    எனில் (method->encode_buffer_fn != வெற்று)
	// Has buffer function, pass through.
	திரும்பு method->encode_buffer_fn(state, from, len, newptr, last);
    எனில் (len == 0)
	// Not buffering, just return EOF.
	திரும்பு (long)len;

    *newptr = alloc(len + 50);
    எனில் (*newptr == வெற்று)
	திரும்பு -1;
    method->encode_fn(state, from, len, *newptr, last);
    திரும்பு (long)len;
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
    முழு		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    எனில் (method->decode_buffer_fn != வெற்று)
	// Has buffer function, pass through.
	திரும்பு method->decode_buffer_fn(state, ptr, len, newptr, last);

    எனில் (len == 0)
	// Not buffering, just return EOF.
	திரும்பு len;

    *newptr = alloc(len);
    எனில் (*newptr == வெற்று)
	திரும்பு -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    திரும்பு len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    வெறுமை
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    முழு		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#எனில் 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    வெறுமை
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    முழு		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    வெறுமை
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    முழு		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    வெறுமை
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    முழு		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    வெறுமை
crypt_free_சாவி(char_u *சாவி)
{
    // Create a safe memset which cannot be optimized away by compiler
    நிலையான வெறுமை *(* நிலையற்ற vim_memset_safe)(வெறுமை *s, முழு c, size_t n) =
	memset;
    எனில் (சாவி != வெற்று)
    {
#ifdef FEAT_SODIUM
	எனில் (sodium_init() >= 0)
	    sodium_memzero(சாவி, STRLEN(சாவி));
	இல்லையெனில்
#endif
	    vim_memset_safe(சாவி, 0, STRLEN(சாவி));
	vim_free(சாவி);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    வெறுமை
crypt_check_method(முழு method)
{
    எனில் (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	செய்தி_scroll = TRUE;
	செய்தி(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    வெறுமை
crypt_check_swapfile_curbuf(வெறுமை)
{
#ifdef FEAT_SODIUM
    முழு method = crypt_get_method_nr(curbuf);
    எனில் (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_மதிப்பு_give_err((char_u *)"swf", 0, வெற்று, OPT_LOCAL);
	செய்தி_scroll = TRUE;
	செய்தி(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    வெறுமை
crypt_check_current_method(வெறுமை)
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
crypt_get_சாவி(
    முழு		store,
    முழு		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = வெற்று;
    முழு		round;

    க்கு (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = செய்தி_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		வெற்று);
	cmdline_star = FALSE;

	எனில் (p1 == வெற்று)
	    நிறுத்து;

	எனில் (round == twice)
	{
	    எனில் (p2 != வெற்று && STRCMP(p1, p2) != 0)
	    {
		செய்தி(_("Keys don't match!"));
		crypt_free_சாவி(p1);
		crypt_free_சாவி(p2);
		p2 = வெற்று;
		round = -1;		// do it again
		தொடர்;
	    }

	    எனில் (store)
	    {
		set_option_மதிப்பு_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_சாவி(p1);
		p1 = curbuf->b_p_சாவி;
		crypt_check_swapfile_curbuf();
	    }
	    நிறுத்து;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    எனில் (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	எனில் (செய்தி_didout)
	    செய்தி_putchar('\n');
	need_wait_return = FALSE;
	செய்தி_didout = FALSE;
    }

    crypt_free_சாவி(p2);
    திரும்பு p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    வெறுமை
crypt_append_செய்தி(
    buf_T *buf)
{
    எனில் (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    இல்லையெனில்
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    நிலையான முழு
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*சாவி UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned எழுத்து	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    முழு			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    முழு			alg;

    எனில் (sodium_init() < 0)
	திரும்பு FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(அளவு(sodium_state_T));
    sodium_memzero(sd_state, அளவு(sodium_state_T));

    எனில் ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# எனில் 0
	// For testing
	எனில் (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	எனில் (crypto_pwhash(dkey, அளவு(dkey), (மாறிலி எழுத்து *)சாவி, STRLEN(சாவி),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    திரும்பு FAIL;
	}
	memcpy(sd_state->சாவி, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->சாவி, crypto_box_SEEDBYTES);
	retval += sodium_mlock(சாவி, STRLEN(சாவி));

	எனில் (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    திரும்பு FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	எனில் (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != வெற்று)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += அளவு(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += அளவு(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, அளவு(opslimit) + அளவு(memlimit) + அளவு(alg));
	}
    }
    இல்லையெனில்
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	முழு	size = அளவு(opslimit) +
	    அளவு(memlimit) + அளவு(alg);

	// Reading parameters from file
	எனில் (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    திரும்பு FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += அளவு(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += அளவு(memlimit);
	alg = crypt_char_to_int(p);
	p += அளவு(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	எனில் (crypto_pwhash(dkey, அளவு(dkey), (மாறிலி எழுத்து *)சாவி, STRLEN(சாவி),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    திரும்பு FAIL;
	}
	memcpy(sd_state->சாவி, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->சாவி, crypto_box_SEEDBYTES);
	retval += sodium_mlock(சாவி, STRLEN(சாவி));

	எனில் (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    திரும்பு FAIL;
	}
    }
    sd_state->மொத்தம் = 0;
    state->method_state = sd_state;

    திரும்பு OK;
#இல்லையெனில்
    emsg(_(e_libsodium_not_built_in));
    திரும்பு FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#எனில் 0  // Currently unused
    வெறுமை
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    முழு		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned எழுத்து  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    எனில் (sod_st->மொத்தம் == 0)
    {
	எனில் (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    திரும்பு;
	}
	crypto_secretstream_xchacha20poly1305_init_தள்ளு(&sod_st->state,
							      to, sod_st->சாவி);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    எனில் (sod_st->மொத்தம் && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	திரும்பு;
    }

    crypto_secretstream_xchacha20poly1305_தள்ளு(&sod_st->state, to, வெற்று,
						      from, len, வெற்று, 0, tag);

    sod_st->மொத்தம்++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#எனில் 0  // Currently unused
    வெறுமை
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    முழு		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned எழுத்து  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    எனில் (sod_st->மொத்தம் == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	திரும்பு;
    }

    buf_out = (char_u *)alloc(len);

    எனில் (buf_out == வெற்று)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	திரும்பு;
    }
    எனில் (sod_st->மொத்தம் == 0)
    {
	எனில் (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->சாவி) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    செல் fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	எனில் (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    எனில் (sod_st->மொத்தம் && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	செல் fail;
    }
    எனில் (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, வெற்று, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	செல் fail;
    }
    sod_st->மொத்தம்++;

    எனில் (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	செல் fail;
    }
    எனில் (p1 == p2)
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
    நிலையான long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    முழு		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned எழுத்து	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    முழு			length;
    sodium_state_T	*sod_st = state->method_state;
    முழு			first = (sod_st->மொத்தம் == 0);

    length = (முழு)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    எனில் (*buf_out == வெற்று)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	திரும்பு -1;
    }
    ptr = *buf_out;

    எனில் (first)
    {
	crypto_secretstream_xchacha20poly1305_init_தள்ளு(&sod_st->state,
		ptr, sod_st->சாவி);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_தள்ளு(&sod_st->state, ptr,
	    &out_len, from, len, வெற்று, 0, tag);

    sod_st->மொத்தம்++;
    திரும்பு out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#இல்லையெனில்
    திரும்பு -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    நிலையான long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    முழு		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned எழுத்து  tag;
    unsigned long long out_len;

    எனில் (sod_st->மொத்தம் == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    எனில் (*buf_out == வெற்று)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	திரும்பு -1;
    }

    எனில் (sod_st->மொத்தம் == 0)
    {
	எனில் (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->சாவி) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    திரும்பு -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->மொத்தம்++;
    }
    எனில் (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, வெற்று, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	திரும்பு -1;
    }

    எனில் (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    திரும்பு (long) out_len;
#இல்லையெனில்
    திரும்பு -1;
#endif
}

#எனில் defined(FEAT_SODIUM)
    வெறுமை
crypt_sodium_lock_சாவி(char_u *சாவி)
{
    எனில் (sodium_init() >= 0)
	sodium_mlock(சாவி, STRLEN(சாவி));
}

    முழு
crypt_sodium_munlock(வெறுமை *மாறிலி addr, மாறிலி size_t len)
{
    திரும்பு sodium_munlock(addr, len);
}

    வெறுமை
crypt_sodium_randombytes_buf(வெறுமை *மாறிலி buf, மாறிலி size_t size)
{
    randombytes_buf(buf, size);
}

    முழு
crypt_sodium_init(வெறுமை)
{
    திரும்பு sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(வெறுமை)
{
    திரும்பு randombytes_random();
}

# எனில் defined(FEAT_EVAL)
    நிலையான வெறுமை
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	முழு alg,
	முழு alg_def)
{
    எனில் (p_verbose > 0)
    {
	verbose_enter();
	எனில் (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	இல்லையெனில்
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	எனில் (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	இல்லையெனில்
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	எனில் (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	இல்லையெனில்
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    நிலையான வெறுமை
crypt_long_long_to_char(long long n, char_u *s)
{
    முழு i;
    க்கு (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    நிலையான வெறுமை
crypt_int_to_char(முழு n, char_u *s)
{
    முழு i;
    க்கு (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    நிலையான long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    முழு i;
    க்கு (i = 7; i >= 0; i--)
    {
	எனில் (i == 7)
	    retval = s[i];
	இல்லையெனில்
	    retval |= s[i];
	எனில் (i > 0)
	    retval <<= 8;
    }
    திரும்பு retval;
}

    நிலையான முழு
crypt_char_to_int(char_u *s)
{
    முழு retval = 0;
    முழு i;

    க்கு (i = 3; i >= 0; i--)
    {
	எனில் (i == 3)
	    retval = s[i];
	இல்லையெனில்
	    retval |= s[i];
	எனில் (i > 0)
	    retval <<= 8;
    }
    திரும்பு retval;
}
#endif

#endif // FEAT_CRYPT
