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
#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_CRYPT)
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

প্রকারনির্দেশ সংরচনা {
    বর্ণ	*name;		// encryption name as used in 'cryptmethod'
    বর্ণ	*magic;		// magic bytes stored in file header
    পূর্ণাঙ্ক		salt_len;	// length of salt, or 0 when not using salt
    পূর্ণাঙ্ক		seed_len;	// length of seed, or 0 when not using seed
    পূর্ণাঙ্ক		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    পূর্ণাঙ্ক		works_inplace;	// encryption/decryption can be done in-place
#endif
    পূর্ণাঙ্ক		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    পূর্ণাঙ্ক (*self_test_fn)(শূন্য);

    // Function pointer for initializing encryption/decryption.
    পূর্ণাঙ্ক (* init_fn)(cryptstate_T *state, char_u *চাবি, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    শূন্য (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, পূর্ণাঙ্ক last);
    শূন্য (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, পূর্ণাঙ্ক last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, পূর্ণাঙ্ক last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, পূর্ণাঙ্ক last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    শূন্য (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, পূর্ণাঙ্ক last);
    শূন্য (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, পূর্ণাঙ্ক last);
} cryptmethod_T;

স্থির পূর্ণাঙ্ক crypt_sodium_init_(cryptstate_T *state, char_u *চাবি, crypt_arg_T *arg);
স্থির long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, পূর্ণাঙ্ক last);
স্থির long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, পূর্ণাঙ্ক last);
#যদি defined(FEAT_SODIUM)
স্থির শূন্য crypt_long_long_to_char(long long n, char_u *s);
স্থির শূন্য crypt_int_to_char(পূর্ণাঙ্ক n, char_u *s);
স্থির long long crypt_char_to_long_long(char_u *s);
স্থির পূর্ণাঙ্ক crypt_char_to_int(char_u *s);
#endif
#যদি defined(FEAT_EVAL) && defined(FEAT_SODIUM)
স্থির শূন্য crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, পূর্ণাঙ্ক alg, পূর্ণাঙ্ক alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
স্থির cryptmethod_T cryptmethods[CRYPT_M_গুনতি] = {
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
	রিক্ত,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	রিক্ত, রিক্ত,
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
	রিক্ত, রিক্ত,
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
	রিক্ত, রিক্ত,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#অন্যথা
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	রিক্ত,
	crypt_sodium_init_,
	রিক্ত, রিক্ত,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	রিক্ত, রিক্ত,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#অন্যথা
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	রিক্ত,
	crypt_sodium_init_,
	রিক্ত, রিক্ত,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	রিক্ত, রিক্ত,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#যদি defined(FEAT_SODIUM)
প্রকারনির্দেশ সংরচনা {
    size_t	    গুনতি;
    unsigned বর্ণ   চাবি[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define প্রতীক_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  অন্যথা
#   error Dynamic loading of libsodium is not supported প্রতি now.
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
#  define crypto_secretstream_xchacha20poly1305_init_ঠেলো \
    dll_crypto_secretstream_xchacha20poly1305_init_ঠেলো
#  define crypto_secretstream_xchacha20poly1305_ঠেলো \
    dll_crypto_secretstream_xchacha20poly1305_ঠেলো
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

স্থির পূর্ণাঙ্ক (*dll_sodium_init)(শূন্য) = রিক্ত;
স্থির শূন্য (*dll_sodium_free)(শূন্য *) = রিক্ত;
স্থির শূন্য *(*dll_sodium_malloc)(ধ্রুব size_t) = রিক্ত;
স্থির শূন্য (*dll_sodium_memzero)(শূন্য * ধ্রুব, ধ্রুব size_t) = রিক্ত;
স্থির পূর্ণাঙ্ক (*dll_sodium_mlock)(শূন্য * ধ্রুব, ধ্রুব size_t) = রিক্ত;
স্থির পূর্ণাঙ্ক (*dll_sodium_munlock)(শূন্য * ধ্রুব, ধ্রুব size_t) = রিক্ত;
স্থির পূর্ণাঙ্ক (*dll_crypto_secretstream_xchacha20poly1305_init_ঠেলো)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned বর্ণ [],
    ধ্রুব unsigned বর্ণ []) = রিক্ত;
স্থির পূর্ণাঙ্ক (*dll_crypto_secretstream_xchacha20poly1305_ঠেলো)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned বর্ণ *c, unsigned long long *clen_p,
    ধ্রুব unsigned বর্ণ *m, unsigned long long mlen,
    ধ্রুব unsigned বর্ণ *ad, unsigned long long adlen, unsigned বর্ণ tag)
    = রিক্ত;
স্থির পূর্ণাঙ্ক (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ধ্রুব unsigned বর্ণ [],
    ধ্রুব unsigned বর্ণ []) = রিক্ত;
স্থির পূর্ণাঙ্ক (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned বর্ণ *m, unsigned long long *mlen_p, unsigned বর্ণ *tag_p,
    ধ্রুব unsigned বর্ণ *c, unsigned long long clen,
    ধ্রুব unsigned বর্ণ *ad, unsigned long long adlen) = রিক্ত;
স্থির পূর্ণাঙ্ক (*dll_crypto_pwhash)(unsigned বর্ণ * ধ্রুব out,
    unsigned long long outlen,
    ধ্রুব বর্ণ * ধ্রুব passwd, unsigned long long passwdlen,
    ধ্রুব unsigned বর্ণ * ধ্রুব salt,
    unsigned long long opslimit, size_t memlimit, পূর্ণাঙ্ক alg)
    = রিক্ত;
স্থির শূন্য (*dll_randombytes_buf)(শূন্য * ধ্রুব buf, ধ্রুব size_t size);
স্থির uint32_t (*dll_randombytes_random)(শূন্য);

স্থির সংরচনা {
    ধ্রুব বর্ণ *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_ঠেলো},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_ঠেলো},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {রিক্ত, রিক্ত}
};

    স্থির পূর্ণাঙ্ক
sodium_runtime_link_init(পূর্ণাঙ্ক verbose)
{
    স্থির HINSTANCE hsodium = রিক্ত;
    ধ্রুব বর্ণ *libname = DYNAMIC_SODIUM_DLL;
    পূর্ণাঙ্ক i;

    যদি (hsodium != রিক্ত)
	প্রত্যাগম OK;

    hsodium = load_dll(libname);
    যদি (hsodium == রিক্ত)
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	প্রত্যাগম FAIL;
    }

    প্রতি (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	যদি ((*sodium_funcname_table[i].ptr = প্রতীক_from_dll(hsodium,
			sodium_funcname_table[i].name)) == রিক্ত)
	{
	    close_dll(hsodium);
	    hsodium = রিক্ত;
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    প্রত্যাগম FAIL;
	}
    }
    প্রত্যাগম OK;
}

    স্থির পূর্ণাঙ্ক
load_sodium(শূন্য)
{
    যদি (sodium_runtime_link_init(TRUE) == FAIL)
	প্রত্যাগম -1;
    প্রত্যাগম dll_sodium_init();
}
# endif

# যদি defined(DYNAMIC_SODIUM)
    পূর্ণাঙ্ক
sodium_enabled(পূর্ণাঙ্ক verbose)
{
    প্রত্যাগম sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
স্থির ধ্রুব বর্ণ crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    পূর্ণাঙ্ক
crypt_method_nr_from_name(char_u *name)
{
    পূর্ণাঙ্ক i;

    প্রতি (i = 0; i < CRYPT_M_গুনতি; ++i)
	যদি (STRCMP(name, cryptmethods[i].name) == 0)
	    প্রত্যাগম i;
    প্রত্যাগম 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    পূর্ণাঙ্ক
crypt_method_nr_from_magic(বর্ণ *ptr, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক i;

    যদি (len < CRYPT_MAGIC_LEN)
	প্রত্যাগম -1;

    প্রতি (i = 0; i < CRYPT_M_গুনতি; i++)
	যদি (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    প্রত্যাগম i;

    i = (পূর্ণাঙ্ক)STRLEN(crypt_magic_head);
    যদি (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    প্রত্যাগম -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    পূর্ণাঙ্ক
crypt_works_inplace(cryptstate_T *state)
{
    প্রত্যাগম cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    পূর্ণাঙ্ক
crypt_get_method_nr(buf_T *buf)
{
    প্রত্যাগম crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    পূর্ণাঙ্ক
crypt_method_is_sodium(পূর্ণাঙ্ক method)
{
    প্রত্যাগম method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    পূর্ণাঙ্ক
crypt_whole_undofile(পূর্ণাঙ্ক method_nr)
{
    প্রত্যাগম cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    পূর্ণাঙ্ক
crypt_get_header_len(পূর্ণাঙ্ক method_nr)
{
    প্রত্যাগম CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#যদি defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    পূর্ণাঙ্ক
crypt_get_max_header_len(শূন্য)
{
    পূর্ণাঙ্ক i;
    পূর্ণাঙ্ক max = 0;
    পূর্ণাঙ্ক temp = 0;

    প্রতি (i = 0; i < CRYPT_M_গুনতি; ++i)
    {
	temp = crypt_get_header_len(i);
	যদি (temp > max)
	    max = temp;
    }
    প্রত্যাগম max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    শূন্য
crypt_set_cm_option(buf_T *buf, পূর্ণাঙ্ক method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    পূর্ণাঙ্ক
crypt_self_test(শূন্য)
{
    পূর্ণাঙ্ক method_nr = crypt_get_method_nr(curbuf);

    যদি (cryptmethods[method_nr].self_test_fn == রিক্ত)
	প্রত্যাগম OK;
    প্রত্যাগম cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    পূর্ণাঙ্ক		method_nr,
    char_u	*চাবি,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    যদি (state == রিক্ত)
	প্রত্যাগম state;

    state->method_nr = method_nr;
    যদি (cryptmethods[method_nr].init_fn(state, চাবি, crypt_arg) == FAIL)
    {
	vim_free(state);
	প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    পূর্ণাঙ্ক		method_nr,
    char_u	*চাবি,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    যদি (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    যদি (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    যদি (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    প্রত্যাগম crypt_create(method_nr, চাবি, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *চাবি)
{
    পূর্ণাঙ্ক		method_nr;
    পূর্ণাঙ্ক		header_len;
    বর্ণ	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    যদি (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	প্রত্যাগম রিক্ত;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    যদি (method_nr < 0)
	প্রত্যাগম রিক্ত;

    header_len = crypt_get_header_len(method_nr);
    যদি ((buffer = alloc(header_len)) == রিক্ত)
	প্রত্যাগম রিক্ত;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    যদি (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	প্রত্যাগম রিক্ত;
    }

    state = crypt_create_from_header(method_nr, চাবি, buffer);
    vim_free(buffer);
    প্রত্যাগম state;
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
    পূর্ণাঙ্ক	    method_nr,
    char_u  *চাবি,
    char_u  **header,
    পূর্ণাঙ্ক	    *header_len)
{
    পূর্ণাঙ্ক	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    যদি (*header == রিক্ত)
	প্রত্যাগম রিক্ত;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    যদি (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	যদি (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	যদি (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	যদি (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	যদি (sodium_init() >= 0)
	{
	    যদি (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    যদি (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	অন্যথা
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, চাবি, &arg);
    যদি (state == রিক্ত)
	VIM_CLEAR(*header);
    প্রত্যাগম state;
}

/*
 * Free the crypt state.
 */
    শূন্য
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    যদি (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->চাবি,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, পরিমাণ(sodium_state_T));
	sodium_free(state->method_state);
    }
    অন্যথা
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
    পূর্ণাঙ্ক		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    যদি (method->encode_buffer_fn != রিক্ত)
	// Has buffer function, pass through.
	প্রত্যাগম method->encode_buffer_fn(state, from, len, newptr, last);
    যদি (len == 0)
	// Not buffering, just return EOF.
	প্রত্যাগম (long)len;

    *newptr = alloc(len + 50);
    যদি (*newptr == রিক্ত)
	প্রত্যাগম -1;
    method->encode_fn(state, from, len, *newptr, last);
    প্রত্যাগম (long)len;
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
    পূর্ণাঙ্ক		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    যদি (method->decode_buffer_fn != রিক্ত)
	// Has buffer function, pass through.
	প্রত্যাগম method->decode_buffer_fn(state, ptr, len, newptr, last);

    যদি (len == 0)
	// Not buffering, just return EOF.
	প্রত্যাগম len;

    *newptr = alloc(len);
    যদি (*newptr == রিক্ত)
	প্রত্যাগম -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    প্রত্যাগম len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    শূন্য
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    পূর্ণাঙ্ক		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#যদি 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    শূন্য
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    পূর্ণাঙ্ক		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    শূন্য
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    পূর্ণাঙ্ক		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    শূন্য
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    পূর্ণাঙ্ক		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    শূন্য
crypt_free_চাবি(char_u *চাবি)
{
    // Create a safe memset which cannot be optimized away by compiler
    স্থির শূন্য *(* চঞ্চল vim_memset_safe)(শূন্য *s, পূর্ণাঙ্ক c, size_t n) =
	memset;
    যদি (চাবি != রিক্ত)
    {
#ifdef FEAT_SODIUM
	যদি (sodium_init() >= 0)
	    sodium_memzero(চাবি, STRLEN(চাবি));
	অন্যথা
#endif
	    vim_memset_safe(চাবি, 0, STRLEN(চাবি));
	vim_free(চাবি);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    শূন্য
crypt_check_method(পূর্ণাঙ্ক method)
{
    যদি (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	বার্তা_scroll = TRUE;
	বার্তা(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    শূন্য
crypt_check_swapfile_curbuf(শূন্য)
{
#ifdef FEAT_SODIUM
    পূর্ণাঙ্ক method = crypt_get_method_nr(curbuf);
    যদি (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_মান_give_err((char_u *)"swf", 0, রিক্ত, OPT_LOCAL);
	বার্তা_scroll = TRUE;
	বার্তা(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    শূন্য
crypt_check_current_method(শূন্য)
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
crypt_get_চাবি(
    পূর্ণাঙ্ক		store,
    পূর্ণাঙ্ক		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = রিক্ত;
    পূর্ণাঙ্ক		round;

    প্রতি (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = বার্তা_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		রিক্ত);
	cmdline_star = FALSE;

	যদি (p1 == রিক্ত)
	    ভঙ্গ;

	যদি (round == twice)
	{
	    যদি (p2 != রিক্ত && STRCMP(p1, p2) != 0)
	    {
		বার্তা(_("Keys don't match!"));
		crypt_free_চাবি(p1);
		crypt_free_চাবি(p2);
		p2 = রিক্ত;
		round = -1;		// do it again
		অনুবর্ত;
	    }

	    যদি (store)
	    {
		set_option_মান_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_চাবি(p1);
		p1 = curbuf->b_p_চাবি;
		crypt_check_swapfile_curbuf();
	    }
	    ভঙ্গ;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    যদি (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	যদি (বার্তা_didout)
	    বার্তা_putchar('\n');
	need_wait_return = FALSE;
	বার্তা_didout = FALSE;
    }

    crypt_free_চাবি(p2);
    প্রত্যাগম p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    শূন্য
crypt_append_বার্তা(
    buf_T *buf)
{
    যদি (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    অন্যথা
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    স্থির পূর্ণাঙ্ক
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*চাবি UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned বর্ণ	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    পূর্ণাঙ্ক			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    পূর্ণাঙ্ক			alg;

    যদি (sodium_init() < 0)
	প্রত্যাগম FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(পরিমাণ(sodium_state_T));
    sodium_memzero(sd_state, পরিমাণ(sodium_state_T));

    যদি ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# যদি 0
	// For testing
	যদি (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	যদি (crypto_pwhash(dkey, পরিমাণ(dkey), (ধ্রুব বর্ণ *)চাবি, STRLEN(চাবি),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    প্রত্যাগম FAIL;
	}
	memcpy(sd_state->চাবি, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->চাবি, crypto_box_SEEDBYTES);
	retval += sodium_mlock(চাবি, STRLEN(চাবি));

	যদি (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    প্রত্যাগম FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	যদি (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != রিক্ত)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += পরিমাণ(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += পরিমাণ(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, পরিমাণ(opslimit) + পরিমাণ(memlimit) + পরিমাণ(alg));
	}
    }
    অন্যথা
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	পূর্ণাঙ্ক	size = পরিমাণ(opslimit) +
	    পরিমাণ(memlimit) + পরিমাণ(alg);

	// Reading parameters from file
	যদি (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    প্রত্যাগম FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += পরিমাণ(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += পরিমাণ(memlimit);
	alg = crypt_char_to_int(p);
	p += পরিমাণ(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	যদি (crypto_pwhash(dkey, পরিমাণ(dkey), (ধ্রুব বর্ণ *)চাবি, STRLEN(চাবি),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    প্রত্যাগম FAIL;
	}
	memcpy(sd_state->চাবি, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->চাবি, crypto_box_SEEDBYTES);
	retval += sodium_mlock(চাবি, STRLEN(চাবি));

	যদি (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    প্রত্যাগম FAIL;
	}
    }
    sd_state->গুনতি = 0;
    state->method_state = sd_state;

    প্রত্যাগম OK;
#অন্যথা
    emsg(_(e_libsodium_not_built_in));
    প্রত্যাগম FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#যদি 0  // Currently unused
    শূন্য
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    পূর্ণাঙ্ক		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned বর্ণ  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    যদি (sod_st->গুনতি == 0)
    {
	যদি (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    প্রত্যাগম;
	}
	crypto_secretstream_xchacha20poly1305_init_ঠেলো(&sod_st->state,
							      to, sod_st->চাবি);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    যদি (sod_st->গুনতি && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	প্রত্যাগম;
    }

    crypto_secretstream_xchacha20poly1305_ঠেলো(&sod_st->state, to, রিক্ত,
						      from, len, রিক্ত, 0, tag);

    sod_st->গুনতি++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#যদি 0  // Currently unused
    শূন্য
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    পূর্ণাঙ্ক		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned বর্ণ  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    যদি (sod_st->গুনতি == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	প্রত্যাগম;
    }

    buf_out = (char_u *)alloc(len);

    যদি (buf_out == রিক্ত)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	প্রত্যাগম;
    }
    যদি (sod_st->গুনতি == 0)
    {
	যদি (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->চাবি) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    গচ্ছ fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	যদি (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    যদি (sod_st->গুনতি && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	গচ্ছ fail;
    }
    যদি (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, রিক্ত, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	গচ্ছ fail;
    }
    sod_st->গুনতি++;

    যদি (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	গচ্ছ fail;
    }
    যদি (p1 == p2)
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
    স্থির long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    পূর্ণাঙ্ক		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned বর্ণ	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    পূর্ণাঙ্ক			length;
    sodium_state_T	*sod_st = state->method_state;
    পূর্ণাঙ্ক			first = (sod_st->গুনতি == 0);

    length = (পূর্ণাঙ্ক)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    যদি (*buf_out == রিক্ত)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	প্রত্যাগম -1;
    }
    ptr = *buf_out;

    যদি (first)
    {
	crypto_secretstream_xchacha20poly1305_init_ঠেলো(&sod_st->state,
		ptr, sod_st->চাবি);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ঠেলো(&sod_st->state, ptr,
	    &out_len, from, len, রিক্ত, 0, tag);

    sod_st->গুনতি++;
    প্রত্যাগম out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#অন্যথা
    প্রত্যাগম -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    স্থির long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    পূর্ণাঙ্ক		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned বর্ণ  tag;
    unsigned long long out_len;

    যদি (sod_st->গুনতি == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    যদি (*buf_out == রিক্ত)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	প্রত্যাগম -1;
    }

    যদি (sod_st->গুনতি == 0)
    {
	যদি (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->চাবি) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    প্রত্যাগম -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->গুনতি++;
    }
    যদি (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, রিক্ত, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	প্রত্যাগম -1;
    }

    যদি (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    প্রত্যাগম (long) out_len;
#অন্যথা
    প্রত্যাগম -1;
#endif
}

#যদি defined(FEAT_SODIUM)
    শূন্য
crypt_sodium_lock_চাবি(char_u *চাবি)
{
    যদি (sodium_init() >= 0)
	sodium_mlock(চাবি, STRLEN(চাবি));
}

    পূর্ণাঙ্ক
crypt_sodium_munlock(শূন্য *ধ্রুব addr, ধ্রুব size_t len)
{
    প্রত্যাগম sodium_munlock(addr, len);
}

    শূন্য
crypt_sodium_randombytes_buf(শূন্য *ধ্রুব buf, ধ্রুব size_t size)
{
    randombytes_buf(buf, size);
}

    পূর্ণাঙ্ক
crypt_sodium_init(শূন্য)
{
    প্রত্যাগম sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(শূন্য)
{
    প্রত্যাগম randombytes_random();
}

# যদি defined(FEAT_EVAL)
    স্থির শূন্য
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	পূর্ণাঙ্ক alg,
	পূর্ণাঙ্ক alg_def)
{
    যদি (p_verbose > 0)
    {
	verbose_enter();
	যদি (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	অন্যথা
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	যদি (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	অন্যথা
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	যদি (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	অন্যথা
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    স্থির শূন্য
crypt_long_long_to_char(long long n, char_u *s)
{
    পূর্ণাঙ্ক i;
    প্রতি (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    স্থির শূন্য
crypt_int_to_char(পূর্ণাঙ্ক n, char_u *s)
{
    পূর্ণাঙ্ক i;
    প্রতি (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    স্থির long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    পূর্ণাঙ্ক i;
    প্রতি (i = 7; i >= 0; i--)
    {
	যদি (i == 7)
	    retval = s[i];
	অন্যথা
	    retval |= s[i];
	যদি (i > 0)
	    retval <<= 8;
    }
    প্রত্যাগম retval;
}

    স্থির পূর্ণাঙ্ক
crypt_char_to_int(char_u *s)
{
    পূর্ণাঙ্ক retval = 0;
    পূর্ণাঙ্ক i;

    প্রতি (i = 3; i >= 0; i--)
    {
	যদি (i == 3)
	    retval = s[i];
	অন্যথা
	    retval |= s[i];
	যদি (i > 0)
	    retval <<= 8;
    }
    প্রত্যাগম retval;
}
#endif

#endif // FEAT_CRYPT
