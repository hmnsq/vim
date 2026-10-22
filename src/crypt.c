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
#अन्तर्भाव "vim.h"

#सचे defined(FEAT_CRYPT)
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

प्रकारनिर्देश संरचना {
    वर्ण	*name;		// encryption name as used in 'cryptmethod'
    वर्ण	*magic;		// magic bytes stored in file header
    पूर्णाङ्क		salt_len;	// length of salt, or 0 when not using salt
    पूर्णाङ्क		seed_len;	// length of seed, or 0 when not using seed
    पूर्णाङ्क		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    पूर्णाङ्क		works_inplace;	// encryption/decryption can be done in-place
#endif
    पूर्णाङ्क		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    पूर्णाङ्क (*self_test_fn)(शून्य);

    // Function pointer for initializing encryption/decryption.
    पूर्णाङ्क (* init_fn)(cryptstate_T *state, char_u *कुञ्चिका, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    शून्य (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, पूर्णाङ्क last);
    शून्य (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, पूर्णाङ्क last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, पूर्णाङ्क last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, पूर्णाङ्क last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    शून्य (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, पूर्णाङ्क last);
    शून्य (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, पूर्णाङ्क last);
} cryptmethod_T;

स्थिर पूर्णाङ्क crypt_sodium_init_(cryptstate_T *state, char_u *कुञ्चिका, crypt_arg_T *arg);
स्थिर long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, पूर्णाङ्क last);
स्थिर long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, पूर्णाङ्क last);
#सचे defined(FEAT_SODIUM)
स्थिर शून्य crypt_long_long_to_char(long long n, char_u *s);
स्थिर शून्य crypt_int_to_char(पूर्णाङ्क n, char_u *s);
स्थिर long long crypt_char_to_long_long(char_u *s);
स्थिर पूर्णाङ्क crypt_char_to_int(char_u *s);
#endif
#सचे defined(FEAT_EVAL) && defined(FEAT_SODIUM)
स्थिर शून्य crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, पूर्णाङ्क alg, पूर्णाङ्क alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
स्थिर cryptmethod_T cryptmethods[CRYPT_M_गणनम्] = {
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
	रिक्त,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	रिक्त, रिक्त,
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
	रिक्त, रिक्त,
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
	रिक्त, रिक्त,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#अञ्ञथा
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	रिक्त,
	crypt_sodium_init_,
	रिक्त, रिक्त,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	रिक्त, रिक्त,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#अञ्ञथा
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	रिक्त,
	crypt_sodium_init_,
	रिक्त, रिक्त,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	रिक्त, रिक्त,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#सचे defined(FEAT_SODIUM)
प्रकारनिर्देश संरचना {
    size_t	    गणनम्;
    unsigned वर्ण   कुञ्चिका[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define चिह्नम्_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  अञ्ञथा
#   error Dynamic loading of libsodium is not supported अत्थाय now.
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
#  define crypto_secretstream_xchacha20poly1305_init_प्रक्षिप \
    dll_crypto_secretstream_xchacha20poly1305_init_प्रक्षिप
#  define crypto_secretstream_xchacha20poly1305_प्रक्षिप \
    dll_crypto_secretstream_xchacha20poly1305_प्रक्षिप
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

स्थिर पूर्णाङ्क (*dll_sodium_init)(शून्य) = रिक्त;
स्थिर शून्य (*dll_sodium_free)(शून्य *) = रिक्त;
स्थिर शून्य *(*dll_sodium_malloc)(ध्रुव size_t) = रिक्त;
स्थिर शून्य (*dll_sodium_memzero)(शून्य * ध्रुव, ध्रुव size_t) = रिक्त;
स्थिर पूर्णाङ्क (*dll_sodium_mlock)(शून्य * ध्रुव, ध्रुव size_t) = रिक्त;
स्थिर पूर्णाङ्क (*dll_sodium_munlock)(शून्य * ध्रुव, ध्रुव size_t) = रिक्त;
स्थिर पूर्णाङ्क (*dll_crypto_secretstream_xchacha20poly1305_init_प्रक्षिप)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned वर्ण [],
    ध्रुव unsigned वर्ण []) = रिक्त;
स्थिर पूर्णाङ्क (*dll_crypto_secretstream_xchacha20poly1305_प्रक्षिप)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned वर्ण *c, unsigned long long *clen_p,
    ध्रुव unsigned वर्ण *m, unsigned long long mlen,
    ध्रुव unsigned वर्ण *ad, unsigned long long adlen, unsigned वर्ण tag)
    = रिक्त;
स्थिर पूर्णाङ्क (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ध्रुव unsigned वर्ण [],
    ध्रुव unsigned वर्ण []) = रिक्त;
स्थिर पूर्णाङ्क (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned वर्ण *m, unsigned long long *mlen_p, unsigned वर्ण *tag_p,
    ध्रुव unsigned वर्ण *c, unsigned long long clen,
    ध्रुव unsigned वर्ण *ad, unsigned long long adlen) = रिक्त;
स्थिर पूर्णाङ्क (*dll_crypto_pwhash)(unsigned वर्ण * ध्रुव out,
    unsigned long long outlen,
    ध्रुव वर्ण * ध्रुव passwd, unsigned long long passwdlen,
    ध्रुव unsigned वर्ण * ध्रुव salt,
    unsigned long long opslimit, size_t memlimit, पूर्णाङ्क alg)
    = रिक्त;
स्थिर शून्य (*dll_randombytes_buf)(शून्य * ध्रुव buf, ध्रुव size_t size);
स्थिर uint32_t (*dll_randombytes_random)(शून्य);

स्थिर संरचना {
    ध्रुव वर्ण *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_प्रक्षिप},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_प्रक्षिप},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {रिक्त, रिक्त}
};

    स्थिर पूर्णाङ्क
sodium_runtime_link_init(पूर्णाङ्क verbose)
{
    स्थिर HINSTANCE hsodium = रिक्त;
    ध्रुव वर्ण *libname = DYNAMIC_SODIUM_DLL;
    पूर्णाङ्क i;

    सचे (hsodium != रिक्त)
	पटिनिवत्त OK;

    hsodium = load_dll(libname);
    सचे (hsodium == रिक्त)
    {
	सचे (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	पटिनिवत्त FAIL;
    }

    अत्थाय (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	सचे ((*sodium_funcname_table[i].ptr = चिह्नम्_from_dll(hsodium,
			sodium_funcname_table[i].name)) == रिक्त)
	{
	    close_dll(hsodium);
	    hsodium = रिक्त;
	    सचे (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    पटिनिवत्त FAIL;
	}
    }
    पटिनिवत्त OK;
}

    स्थिर पूर्णाङ्क
load_sodium(शून्य)
{
    सचे (sodium_runtime_link_init(TRUE) == FAIL)
	पटिनिवत्त -1;
    पटिनिवत्त dll_sodium_init();
}
# endif

# सचे defined(DYNAMIC_SODIUM)
    पूर्णाङ्क
sodium_enabled(पूर्णाङ्क verbose)
{
    पटिनिवत्त sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
स्थिर ध्रुव वर्ण crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    पूर्णाङ्क
crypt_method_nr_from_name(char_u *name)
{
    पूर्णाङ्क i;

    अत्थाय (i = 0; i < CRYPT_M_गणनम्; ++i)
	सचे (STRCMP(name, cryptmethods[i].name) == 0)
	    पटिनिवत्त i;
    पटिनिवत्त 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    पूर्णाङ्क
crypt_method_nr_from_magic(वर्ण *ptr, पूर्णाङ्क len)
{
    पूर्णाङ्क i;

    सचे (len < CRYPT_MAGIC_LEN)
	पटिनिवत्त -1;

    अत्थाय (i = 0; i < CRYPT_M_गणनम्; i++)
	सचे (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    पटिनिवत्त i;

    i = (पूर्णाङ्क)STRLEN(crypt_magic_head);
    सचे (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    पटिनिवत्त -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    पूर्णाङ्क
crypt_works_inplace(cryptstate_T *state)
{
    पटिनिवत्त cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    पूर्णाङ्क
crypt_get_method_nr(buf_T *buf)
{
    पटिनिवत्त crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    पूर्णाङ्क
crypt_method_is_sodium(पूर्णाङ्क method)
{
    पटिनिवत्त method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    पूर्णाङ्क
crypt_whole_undofile(पूर्णाङ्क method_nr)
{
    पटिनिवत्त cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    पूर्णाङ्क
crypt_get_header_len(पूर्णाङ्क method_nr)
{
    पटिनिवत्त CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#सचे defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    पूर्णाङ्क
crypt_get_max_header_len(शून्य)
{
    पूर्णाङ्क i;
    पूर्णाङ्क max = 0;
    पूर्णाङ्क temp = 0;

    अत्थाय (i = 0; i < CRYPT_M_गणनम्; ++i)
    {
	temp = crypt_get_header_len(i);
	सचे (temp > max)
	    max = temp;
    }
    पटिनिवत्त max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    शून्य
crypt_set_cm_option(buf_T *buf, पूर्णाङ्क method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    पूर्णाङ्क
crypt_self_test(शून्य)
{
    पूर्णाङ्क method_nr = crypt_get_method_nr(curbuf);

    सचे (cryptmethods[method_nr].self_test_fn == रिक्त)
	पटिनिवत्त OK;
    पटिनिवत्त cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    पूर्णाङ्क		method_nr,
    char_u	*कुञ्चिका,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    सचे (state == रिक्त)
	पटिनिवत्त state;

    state->method_nr = method_nr;
    सचे (cryptmethods[method_nr].init_fn(state, कुञ्चिका, crypt_arg) == FAIL)
    {
	vim_free(state);
	पटिनिवत्त रिक्त;
    }
    पटिनिवत्त state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    पूर्णाङ्क		method_nr,
    char_u	*कुञ्चिका,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    सचे (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    सचे (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    सचे (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    पटिनिवत्त crypt_create(method_nr, कुञ्चिका, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *कुञ्चिका)
{
    पूर्णाङ्क		method_nr;
    पूर्णाङ्क		header_len;
    वर्ण	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    सचे (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	पटिनिवत्त रिक्त;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    सचे (method_nr < 0)
	पटिनिवत्त रिक्त;

    header_len = crypt_get_header_len(method_nr);
    सचे ((buffer = alloc(header_len)) == रिक्त)
	पटिनिवत्त रिक्त;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    सचे (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	पटिनिवत्त रिक्त;
    }

    state = crypt_create_from_header(method_nr, कुञ्चिका, buffer);
    vim_free(buffer);
    पटिनिवत्त state;
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
    पूर्णाङ्क	    method_nr,
    char_u  *कुञ्चिका,
    char_u  **header,
    पूर्णाङ्क	    *header_len)
{
    पूर्णाङ्क	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    सचे (*header == रिक्त)
	पटिनिवत्त रिक्त;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    सचे (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	सचे (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	सचे (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	सचे (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	सचे (sodium_init() >= 0)
	{
	    सचे (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    सचे (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	अञ्ञथा
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, कुञ्चिका, &arg);
    सचे (state == रिक्त)
	VIM_CLEAR(*header);
    पटिनिवत्त state;
}

/*
 * Free the crypt state.
 */
    शून्य
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    सचे (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->कुञ्चिका,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, परिमाण(sodium_state_T));
	sodium_free(state->method_state);
    }
    अञ्ञथा
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
    पूर्णाङ्क		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    सचे (method->encode_buffer_fn != रिक्त)
	// Has buffer function, pass through.
	पटिनिवत्त method->encode_buffer_fn(state, from, len, newptr, last);
    सचे (len == 0)
	// Not buffering, just return EOF.
	पटिनिवत्त (long)len;

    *newptr = alloc(len + 50);
    सचे (*newptr == रिक्त)
	पटिनिवत्त -1;
    method->encode_fn(state, from, len, *newptr, last);
    पटिनिवत्त (long)len;
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
    पूर्णाङ्क		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    सचे (method->decode_buffer_fn != रिक्त)
	// Has buffer function, pass through.
	पटिनिवत्त method->decode_buffer_fn(state, ptr, len, newptr, last);

    सचे (len == 0)
	// Not buffering, just return EOF.
	पटिनिवत्त len;

    *newptr = alloc(len);
    सचे (*newptr == रिक्त)
	पटिनिवत्त -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    पटिनिवत्त len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    शून्य
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    पूर्णाङ्क		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#सचे 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    शून्य
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    पूर्णाङ्क		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    शून्य
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    पूर्णाङ्क		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    शून्य
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    पूर्णाङ्क		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    शून्य
crypt_free_कुञ्चिका(char_u *कुञ्चिका)
{
    // Create a safe memset which cannot be optimized away by compiler
    स्थिर शून्य *(* चञ्चल vim_memset_safe)(शून्य *s, पूर्णाङ्क c, size_t n) =
	memset;
    सचे (कुञ्चिका != रिक्त)
    {
#ifdef FEAT_SODIUM
	सचे (sodium_init() >= 0)
	    sodium_memzero(कुञ्चिका, STRLEN(कुञ्चिका));
	अञ्ञथा
#endif
	    vim_memset_safe(कुञ्चिका, 0, STRLEN(कुञ्चिका));
	vim_free(कुञ्चिका);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    शून्य
crypt_check_method(पूर्णाङ्क method)
{
    सचे (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	सन्देशः_scroll = TRUE;
	सन्देशः(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    शून्य
crypt_check_swapfile_curbuf(शून्य)
{
#ifdef FEAT_SODIUM
    पूर्णाङ्क method = crypt_get_method_nr(curbuf);
    सचे (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_मानम्_give_err((char_u *)"swf", 0, रिक्त, OPT_LOCAL);
	सन्देशः_scroll = TRUE;
	सन्देशः(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    शून्य
crypt_check_current_method(शून्य)
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
crypt_get_कुञ्चिका(
    पूर्णाङ्क		store,
    पूर्णाङ्क		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = रिक्त;
    पूर्णाङ्क		round;

    अत्थाय (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = सन्देशः_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		रिक्त);
	cmdline_star = FALSE;

	सचे (p1 == रिक्त)
	    भङ्ग;

	सचे (round == twice)
	{
	    सचे (p2 != रिक्त && STRCMP(p1, p2) != 0)
	    {
		सन्देशः(_("Keys don't match!"));
		crypt_free_कुञ्चिका(p1);
		crypt_free_कुञ्चिका(p2);
		p2 = रिक्त;
		round = -1;		// do it again
		अनुवर्त;
	    }

	    सचे (store)
	    {
		set_option_मानम्_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_कुञ्चिका(p1);
		p1 = curbuf->b_p_कुञ्चिका;
		crypt_check_swapfile_curbuf();
	    }
	    भङ्ग;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    सचे (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	सचे (सन्देशः_didout)
	    सन्देशः_putchar('\n');
	need_wait_return = FALSE;
	सन्देशः_didout = FALSE;
    }

    crypt_free_कुञ्चिका(p2);
    पटिनिवत्त p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    शून्य
crypt_append_सन्देशः(
    buf_T *buf)
{
    सचे (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    अञ्ञथा
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    स्थिर पूर्णाङ्क
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*कुञ्चिका UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned वर्ण	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    पूर्णाङ्क			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    पूर्णाङ्क			alg;

    सचे (sodium_init() < 0)
	पटिनिवत्त FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(परिमाण(sodium_state_T));
    sodium_memzero(sd_state, परिमाण(sodium_state_T));

    सचे ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# सचे 0
	// For testing
	सचे (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	सचे (crypto_pwhash(dkey, परिमाण(dkey), (ध्रुव वर्ण *)कुञ्चिका, STRLEN(कुञ्चिका),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    पटिनिवत्त FAIL;
	}
	memcpy(sd_state->कुञ्चिका, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->कुञ्चिका, crypto_box_SEEDBYTES);
	retval += sodium_mlock(कुञ्चिका, STRLEN(कुञ्चिका));

	सचे (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    पटिनिवत्त FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	सचे (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != रिक्त)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += परिमाण(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += परिमाण(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, परिमाण(opslimit) + परिमाण(memlimit) + परिमाण(alg));
	}
    }
    अञ्ञथा
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	पूर्णाङ्क	size = परिमाण(opslimit) +
	    परिमाण(memlimit) + परिमाण(alg);

	// Reading parameters from file
	सचे (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    पटिनिवत्त FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += परिमाण(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += परिमाण(memlimit);
	alg = crypt_char_to_int(p);
	p += परिमाण(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	सचे (crypto_pwhash(dkey, परिमाण(dkey), (ध्रुव वर्ण *)कुञ्चिका, STRLEN(कुञ्चिका),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    पटिनिवत्त FAIL;
	}
	memcpy(sd_state->कुञ्चिका, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->कुञ्चिका, crypto_box_SEEDBYTES);
	retval += sodium_mlock(कुञ्चिका, STRLEN(कुञ्चिका));

	सचे (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    पटिनिवत्त FAIL;
	}
    }
    sd_state->गणनम् = 0;
    state->method_state = sd_state;

    पटिनिवत्त OK;
#अञ्ञथा
    emsg(_(e_libsodium_not_built_in));
    पटिनिवत्त FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#सचे 0  // Currently unused
    शून्य
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    पूर्णाङ्क		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned वर्ण  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    सचे (sod_st->गणनम् == 0)
    {
	सचे (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    पटिनिवत्त;
	}
	crypto_secretstream_xchacha20poly1305_init_प्रक्षिप(&sod_st->state,
							      to, sod_st->कुञ्चिका);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    सचे (sod_st->गणनम् && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	पटिनिवत्त;
    }

    crypto_secretstream_xchacha20poly1305_प्रक्षिप(&sod_st->state, to, रिक्त,
						      from, len, रिक्त, 0, tag);

    sod_st->गणनम्++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#सचे 0  // Currently unused
    शून्य
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    पूर्णाङ्क		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned वर्ण  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    सचे (sod_st->गणनम् == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	पटिनिवत्त;
    }

    buf_out = (char_u *)alloc(len);

    सचे (buf_out == रिक्त)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	पटिनिवत्त;
    }
    सचे (sod_st->गणनम् == 0)
    {
	सचे (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->कुञ्चिका) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    गच्छ fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	सचे (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    सचे (sod_st->गणनम् && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	गच्छ fail;
    }
    सचे (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, रिक्त, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	गच्छ fail;
    }
    sod_st->गणनम्++;

    सचे (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	गच्छ fail;
    }
    सचे (p1 == p2)
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
    स्थिर long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    पूर्णाङ्क		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned वर्ण	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    पूर्णाङ्क			length;
    sodium_state_T	*sod_st = state->method_state;
    पूर्णाङ्क			first = (sod_st->गणनम् == 0);

    length = (पूर्णाङ्क)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    सचे (*buf_out == रिक्त)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	पटिनिवत्त -1;
    }
    ptr = *buf_out;

    सचे (first)
    {
	crypto_secretstream_xchacha20poly1305_init_प्रक्षिप(&sod_st->state,
		ptr, sod_st->कुञ्चिका);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_प्रक्षिप(&sod_st->state, ptr,
	    &out_len, from, len, रिक्त, 0, tag);

    sod_st->गणनम्++;
    पटिनिवत्त out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#अञ्ञथा
    पटिनिवत्त -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    स्थिर long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    पूर्णाङ्क		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned वर्ण  tag;
    unsigned long long out_len;

    सचे (sod_st->गणनम् == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    सचे (*buf_out == रिक्त)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	पटिनिवत्त -1;
    }

    सचे (sod_st->गणनम् == 0)
    {
	सचे (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->कुञ्चिका) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    पटिनिवत्त -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->गणनम्++;
    }
    सचे (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, रिक्त, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	पटिनिवत्त -1;
    }

    सचे (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    पटिनिवत्त (long) out_len;
#अञ्ञथा
    पटिनिवत्त -1;
#endif
}

#सचे defined(FEAT_SODIUM)
    शून्य
crypt_sodium_lock_कुञ्चिका(char_u *कुञ्चिका)
{
    सचे (sodium_init() >= 0)
	sodium_mlock(कुञ्चिका, STRLEN(कुञ्चिका));
}

    पूर्णाङ्क
crypt_sodium_munlock(शून्य *ध्रुव addr, ध्रुव size_t len)
{
    पटिनिवत्त sodium_munlock(addr, len);
}

    शून्य
crypt_sodium_randombytes_buf(शून्य *ध्रुव buf, ध्रुव size_t size)
{
    randombytes_buf(buf, size);
}

    पूर्णाङ्क
crypt_sodium_init(शून्य)
{
    पटिनिवत्त sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(शून्य)
{
    पटिनिवत्त randombytes_random();
}

# सचे defined(FEAT_EVAL)
    स्थिर शून्य
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	पूर्णाङ्क alg,
	पूर्णाङ्क alg_def)
{
    सचे (p_verbose > 0)
    {
	verbose_enter();
	सचे (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	अञ्ञथा
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	सचे (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	अञ्ञथा
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	सचे (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	अञ्ञथा
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    स्थिर शून्य
crypt_long_long_to_char(long long n, char_u *s)
{
    पूर्णाङ्क i;
    अत्थाय (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    स्थिर शून्य
crypt_int_to_char(पूर्णाङ्क n, char_u *s)
{
    पूर्णाङ्क i;
    अत्थाय (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    स्थिर long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    पूर्णाङ्क i;
    अत्थाय (i = 7; i >= 0; i--)
    {
	सचे (i == 7)
	    retval = s[i];
	अञ्ञथा
	    retval |= s[i];
	सचे (i > 0)
	    retval <<= 8;
    }
    पटिनिवत्त retval;
}

    स्थिर पूर्णाङ्क
crypt_char_to_int(char_u *s)
{
    पूर्णाङ्क retval = 0;
    पूर्णाङ्क i;

    अत्थाय (i = 3; i >= 0; i--)
    {
	सचे (i == 3)
	    retval = s[i];
	अञ्ञथा
	    retval |= s[i];
	सचे (i > 0)
	    retval <<= 8;
    }
    पटिनिवत्त retval;
}
#endif

#endif // FEAT_CRYPT
